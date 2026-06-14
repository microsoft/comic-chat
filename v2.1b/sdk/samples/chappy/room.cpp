//****************************************************************************
//  Module:     Chappy.EXE
//  File:       Room.cpp : implementation file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************



#include "stdafx.h"
#include "Chappy.h"
#include "Room.h"
#include "Globals.h"
#include "kickDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CRoom::CRoom()
{
}

CRoom::~CRoom()
{
	if (m_IChannels.m_lpDispatch)
	{
		m_IChannels.ReleaseDispatch();
		m_IChannels = NULL;
	}
}

//|bool CRoom::operator== ( const CRoom &src ) const
//|{
//|	if ((m_IChannel	== src.m_IChannel))
//|	{
//|		return TRUE;
//|	}
//|	else
//|	{
//|		return FALSE;
//|	}
//|}
//|
//|void CRoom::operator= ( const CRoom& src )
//|{
//|	m_IChannel		= src.m_IChannel;
//|}


BOOL CRoom::Create(CWnd &wndParent_p, LPDISPATCH lpIChannels_p) 
{
	//ASSERT (!m_ctrlHistoryWnd);  //if we already have a control something is wrong
	ASSERT (&wndParent_p);
	ASSERT (lpIChannels_p);

	m_IChannels.AttachDispatch(lpIChannels_p, TRUE);

	//create the input rich edit control	
	RECT rec;
	rec.left	= 100;
	rec.top		= 100;
	rec.right	= 200;
	rec.bottom	= 200;

	int bRet = m_ctrlHistoryWnd.Create(WS_CHILD | WS_BORDER | WS_VSCROLL |
										ES_MULTILINE | ES_AUTOVSCROLL| WS_EX_NOPARENTNOTIFY, 
										rec, 
										&wndParent_p,
										IDC_HISTORY_RICHEDIT); 
	ASSERT (bRet);

	//create the list of participant control
	bRet = m_ctrlPartListWnd.Create(WS_CHILD | WS_BORDER | WS_VSCROLL | WS_TABSTOP | WS_THICKFRAME |
									TVS_HASLINES | TVS_HASBUTTONS | TVS_SHOWSELALWAYS | TVS_LINESATROOT, 
									rec, 
									wndParent_p.GetParent(),
									IDC_PART_LIST_TREE);
	ASSERT (bRet);	

	
	//we do not have a channel yet
	m_IChannel	= NULL;
	m_iIndex	= 0;

//|	//create the Channel item
//|	//to instaciate the OCX you can get the CLSID directly usign MIDL or use CLSIDFromProgID
//|	const CLSID CLSID_MsChatPr = {0x039397c0,0x46df,0x11d0,{0x99,0xcb,0x00,0xc0,0x4f,0xd6,0x44,0x97}};
//|	//const CLSID CLSID_Channels = {0x011eee42,0x46df,0x11d0,{0x99,0xcb,0x00,0xc0,0x4f,0xd6,0x44,0x97}};
//|	//const CLSID CLSID_Channel = {0x011eee44,0x46df,0x11d0,{0x99,0xcb,0x00,0xc0,0x4f,0xd6,0x44,0x97}};
//|
//|	// create the chat object that we'll drive through OLE automation
//|	COleException e;
//|	CLSID clsid = CLSID_MsChatPr; 
//|
//|	if (CLSIDFromProgID(OLESTR("MsChatX.Microsoft Chat Protocol Control.2"), &clsid) != NOERROR)
//|	{
//|		AfxMessageBox("Failed to create");
//|		//EndDialog(IDABORT);
//|		return FALSE;
//|	}
//|
//|	// try to get the active calculator before creating a new one
//|	LPUNKNOWN lpUnk;
//|	LPDISPATCH lpDispatch;
//|	if (GetActiveObject(clsid, NULL, &lpUnk) == NOERROR)
//|	{
//|		HRESULT hr = lpUnk->QueryInterface(IID_IDispatch, 
//|			(LPVOID*)&lpDispatch);
//|		lpUnk->Release();
//|		if (hr == NOERROR)
//|			m_IChannel.AttachDispatch(lpDispatch, TRUE);
//|	}
//|
//|	// if not dispatch ptr attached yet, need to create one
//|	if (m_IChannel.m_lpDispatch == NULL && 
//|		!m_IChannel.CreateDispatch(clsid, &e))
//|	{
//|		AfxMessageBox("CreateDispatch() failed\n");
//|		return FALSE;	
//|	}
//|
//|	COleVariant vChannel(_T("#TestAR"));
//|	COleVariant vKey(_T(""));
//|
//|	m_IChannel.JoinChannel(vChannel,vKey);

	return TRUE;
}

void CRoom::OnFocus(BOOL bFocus_p)
{
	m_ctrlHistoryWnd.ShowWindow(bFocus_p ? SW_SHOW : SW_HIDE);
	m_ctrlPartListWnd.ShowWindow(bFocus_p ? SW_SHOW : SW_HIDE);
}

void CRoom::SendMessage(LPCTSTR SenderNickname_p, long iMsgType_p, CString & szMsgTxt_p)
{
	TRACE1("Send Messge : '%s'\n", szMsgTxt_p);

	if (!m_IChannel) return;

	COleVariant vMsgTxt(szMsgTxt_p);
	COleVariant vRecipients("");
	COleVariant vDataMessageTag;
	vDataMessageTag.Clear();

	vRecipients.Clear();
	if (msgtWhisper == iMsgType_p )
	{
		vRecipients = GetSelectedMember();
		if (vRecipients == COleVariant(""))
		{
			AfxMessageBox(IDS_NO_MEMBER_SELECTED);
			return;
		}
	}
	
	m_IChannel.SendMessage(iMsgType_p, vMsgTxt, vRecipients, vDataMessageTag);
	m_ctrlHistoryWnd.UpdateMessageUI(SenderNickname_p, iMsgType_p, szMsgTxt_p);	
}

void CRoom::LeaveChannel()
{
	//exit if we do not have a channel object (not in a room)
	if (!m_IChannel) return;

	//leave the channel
	if (chsClosed != m_IChannel.GetChannelState())
	{
		m_IChannel.LeaveChannel();
	}
	//m_ctrlPartListWnd.DeleteAllItems();
}

//call this function async
void CRoom::DestroyRoom()
{
	if (m_ctrlPartListWnd)
	{
		m_ctrlPartListWnd.DeleteAllItems();
	}

	//release the IChannel object
	m_IChannel.ReleaseDispatch();
	m_IChannel = NULL;
	
	//remove the channel from the collection
	m_IChannels.Remove(COleVariant(m_szKey));

}


BOOL CRoom::EnterRoom(long iIndex_p, CString &szRoomName_p, CString &szRoomKey_p)
{

	ASSERT (!m_IChannel); //if we already have a channel something is wrong
	ASSERT (!szRoomName_p.IsEmpty());

	m_iIndex = iIndex_p;
	m_szKey.Format("%s%i",szRoomName_p,iIndex_p);

	//Add a channel to the collection 
	//the ocx takes care of pushing in the new channel if you pass an index that
	//already exists. If you pass in an invalid index, on the other hand, the OCX complains
	COleVariant vIndex(1L);
	COleVariant vKey(m_szKey);
	
	LPDISPATCH lpIChannel = NULL;
	lpIChannel = m_IChannels.Add (vIndex, vKey);
	m_IChannel.AttachDispatch(lpIChannel, TRUE);
	ASSERT (m_IChannel);

	//enter the room
	COleVariant vChannel (szRoomName_p) ;
	COleVariant vChannelKey(szRoomKey_p);
	m_IChannel.JoinChannel(vChannel,vChannelKey);

	return TRUE;
}

CString CRoom::GetSelectedMember()
{
	HTREEITEM iSel = m_ctrlPartListWnd.GetSelItem();
	return ((iSel) ?  m_ctrlPartListWnd.GetItemText(iSel) : "");
}

void CRoom::KickMember(CString & szReason_p)
{
	{
		CString szMember = GetSelectedMember();
		TRACE2("-- Kick: %s Reason: %s",szMember, szReason_p); 
		m_IChannel.KickMember((LPCSTR)szMember, COleVariant(szReason_p));
	}
}

void CRoom::BanMember(CChatItems &IChatItem_p)
{ 
		ASSERT (IChatItem_p);
		IChatItem_p.SetAssociatedType("Query");
		IChatItem_p.SetItem ("Nickname", COleVariant(GetSelectedMember()));
		m_IChannel.BanMembers(IChatItem_p, TRUE);
}

void CRoom::MakeHost(BOOL bFlag)
{
	m_IChannel.ChangeMemberProperty ("Modes", COleVariant((long)(bFlag ? mmHost : mmNotHost)),COleVariant(GetSelectedMember()));
}
