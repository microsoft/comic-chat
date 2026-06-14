//****************************************************************************
//  Module:     Chappy.EXE
//  File:       ChappyView.cpp : implementation of the CChappyView class
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

#include "ChppyDoc.h"
#include "ChppyVw.h"
#include "Globals.h"
#include "EnterDlg.h"
#include "chtitems.h"
#include "mainfrm.h"
#include "LogInDlg.h"
#include "PWordDlg.h"
#include "KickDlg.h"
#include "BanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChappyView

IMPLEMENT_DYNCREATE(CChappyView, CView)

BEGIN_MESSAGE_MAP(CChappyView, CView)
	//{{AFX_MSG_MAP(CChappyView)
	ON_WM_CREATE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_MESSAGE_TAB, OnSelchangeTab1)
	ON_WM_SIZE()
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_WM_DESTROY()
	ON_COMMAND(ID_BTN_SEND, OnBtnSend)
	ON_COMMAND(ID_BTN_WHISPER, OnBtnWhisper)
	ON_COMMAND(ID_ROOM_CONNECT, OnRoomConnect)
	ON_COMMAND(ID_BTN_DO, OnBtnDo)
	ON_COMMAND(ID_BTN_THINK, OnBtnThink)
	ON_COMMAND(ID_ROOM_DISCONNECT, OnRoomDisconnect)
	ON_UPDATE_COMMAND_UI(ID_ROOM_CONNECT, OnUpdateRoomConnect)
	ON_UPDATE_COMMAND_UI(ID_ROOM_DISCONNECT, OnUpdateRoomDisconnect)
	ON_COMMAND(ID_SERVER_LOGIN, OnRoomLogin)
	ON_UPDATE_COMMAND_UI(ID_SERVER_LOGIN, OnUpdateRoomLogin)
	ON_UPDATE_COMMAND_UI(ID_SERVER_LOGOUT, OnUpdateRoomLogout)
	ON_COMMAND(ID_SERVER_LOGOUT, OnRoomLogout)
	ON_COMMAND(ID_ROOM_LISTROOMS, OnRoomListrooms)
	ON_COMMAND(ID_ROOMLIST_ENTERROOM, OnRoomListEnterRoom)
	ON_UPDATE_COMMAND_UI(ID_ROOMLIST_ENTERROOM, OnUpdateRoomListEnterRoom)
	ON_UPDATE_COMMAND_UI(ID_ROOM_LISTROOMS, OnUpdateRoomListRooms)
	ON_COMMAND(ID_PARTLIST_KICK, OnPartlistKick)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_UPDATE_COMMAND_UI(ID_PARTLIST_BAN, OnUpdatePartlistBan)
	ON_COMMAND(ID_PARTLIST_BAN, OnPartlistBan)
	ON_COMMAND(ID_SERVER_BANUSER, OnServerBanuser)
	ON_UPDATE_COMMAND_UI(ID_SERVER_BANUSER, OnUpdateServerBanuser)
	ON_UPDATE_COMMAND_UI(ID_PARTLIST_KICK, OnUpdatePartlistKick)
	ON_COMMAND(ID_PARTLIST_MAKEHOST, OnPartlistMakehost)
	ON_UPDATE_COMMAND_UI(ID_PARTLIST_MAKEHOST, OnUpdatePartlistMakehost)
	ON_COMMAND(ID_PARTLIST_MAKEPARTICIPANT, OnPartlistMakeparticipant)
	ON_UPDATE_COMMAND_UI(ID_PARTLIST_MAKEPARTICIPANT, OnUpdatePartlistMakeparticipant)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_HISTORY_CLEAR, OnHistoryClear)
	ON_UPDATE_COMMAND_UI(ID_HISTORY_CLEAR, OnUpdateHistoryClear)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

//	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
//n	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)


//EnableToolTips(TRUE);

/////////////////////////////////////////////////////////////////////////////
// CChappyView construction/destruction

CChappyView::CChappyView()
{
	// TODO: add construction code here
	m_cx = m_cy = 0;
	m_iLastTab = -1; //first chat room
	m_iVisibleTab = 0;
	m_iServerConnectionState = csDisconnected;
	m_iServerConnectionType = -1;
	for (long i=0; i < MAX_ROOMS; ++i)
	{
		m_ChatRoom[i] = NULL;
	}
}

CChappyView::~CChappyView()
{
	//disconnect from the server
	//m_ocxChat.Disconnect();
	for (long i=0; i < MAX_ROOMS; ++i)
	{
		if (m_ChatRoom[i])
		{
			delete(m_ChatRoom[i]);
			m_ChatRoom[i] = NULL;
		}
	}
}

BOOL CChappyView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}


/////////////////////////////////////////////////////////////////////////////
// CChappyView diagnostics

#ifdef _DEBUG
void CChappyView::AssertValid() const
{
	CView::AssertValid();
}

void CChappyView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CChappyDoc* CChappyView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChappyDoc)));
	return (CChappyDoc*)m_pDocument;
}
#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CChappyView drawing

void CChappyView::OnDraw(CDC* pDC)
{
	//TRACE0("DRAW \n");
	//CChappyDoc* pDoc = GetDocument();
	//ASSERT_VALID(pDoc);
	
	
	int iParticListSize = 0;
	if ( (LIST_TAB != m_iVisibleTab) && (MESSAGE_TAB != m_iVisibleTab) )
	{
		//the tab contains a room
		iParticListSize = m_ChatRoom[m_iVisibleTab]->m_ctrlPartListWnd.GetCx();
	
		//TRACE1("size = %u", iParticListSize);
		m_ChatRoom[m_iVisibleTab]->m_ctrlPartListWnd.MoveWindow(	m_cx - iParticListSize, 
																0, 
																iParticListSize, 
																m_cy, 
																TRUE);
	}

	m_ctrlTabWnd.MoveWindow(0, 
							0, 
							m_cx - iParticListSize, 
							m_cy - INPUT_RICHEDIT_SIZE, 
							TRUE);


	//the history/room list window
	HWND hWnd = NULL;
	switch (m_iVisibleTab) 
	{
		case LIST_TAB:
			hWnd = m_ctrlRoomList;
			break;
		case MESSAGE_TAB:
			//server and private messages
			hWnd = m_ctrlPrivateMsg;
			break;
		default:
			//all the other tabs are rooms 
			hWnd = m_ChatRoom[m_iVisibleTab]->m_ctrlHistoryWnd;
	}

	::MoveWindow(	hWnd,
					6, 
					6, 
					m_cx - iParticListSize - 20, 
					m_cy - INPUT_RICHEDIT_SIZE - 42, 
					TRUE);

	m_ctrlSendToolBar.MoveWindow(	0, 
									m_cy - INPUT_RICHEDIT_SIZE, 
									SEND_TB_SIZE , 
									INPUT_RICHEDIT_SIZE, 
									FALSE);


	m_ctrlInputWnd.MoveWindow(		SEND_TB_SIZE , 
									m_cy - INPUT_RICHEDIT_SIZE, 
									m_cx - iParticListSize - SEND_TB_SIZE, 
									INPUT_RICHEDIT_SIZE, 
									TRUE);


}

/////////////////////////////////////////////////////////////////////////////
// CChappyView message handlers

void CChappyView::OnDestroy() 
{
	//disconnect from the server
	if (csConnected <= m_iServerConnectionState)
	{
		OnRoomLogout();
	}

	//commctrl faults if do not destroy the windows in the proper order
	m_ctrlRoomList.DestroyWindow();
	m_ctrlPrivateMsg.DestroyWindow();
	m_ctrlTabWnd.DestroyWindow();
	m_ctrlSendToolBar.DestroyWindow();
	m_ctrlInputWnd.DestroyWindow();

	CView::OnDestroy();

}


int CChappyView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//some random initial size
	RECT rec;
	rec.left	= 0;
	rec.top		= 0;
	rec.right	= 100;
	rec.bottom	= 100;

	BOOL ret = FALSE;
	
	//create the tab control
	ret = m_ctrlTabWnd.Create(	WS_CHILD | WS_BORDER | WS_VISIBLE | WS_TABSTOP | WS_THICKFRAME |
								TCS_BOTTOM | TCS_TABS , 
								rec, this, IDC_MESSAGE_TAB);
	ASSERT (ret);

	//create the input rich edit control
	ret = m_ctrlInputWnd.Create(WS_CHILD | WS_BORDER |  WS_VISIBLE | WS_VSCROLL |
								ES_MULTILINE | ES_AUTOVSCROLL, 
								rec, 
								this,
								IDC_INPUT_RICHEDIT);
	ASSERT (ret);

//	//set the font for the input richedit
//	CHARFORMAT cf;
//	m_ctrlInputWnd.GetDefaultCharFormat(cf);
//
//
//	cf.cbSize = sizeof (CHARFORMAT); 
//	cf.dwMask = CFM_FACE ;
//	//cf.dwEffects;
//	//cf.yHeight;
//	//cf.yOffset;
//	//cf.crTextColor=RGB(250,0,0);
//	//cf.bCharSet;
//	//cf.bPitchAndFamily;
//	sprintf(cf.szFaceName, "Script");
//
//	m_ctrlInputWnd.SetDefaultCharFormat(cf);
//	
	
	//create the toolbar with the send/wisper buttons
	ret = m_ctrlSendToolBar.Create(	this,//GetParent(),
									WS_CHILD |  WS_VISIBLE |
									TBSTYLE_TOOLTIPS | CBRS_RIGHT | CCS_NORESIZE , 
									IDR_SEND_TOOLBAR);
	ASSERT (ret);

	//load the toolbar
	ret = m_ctrlSendToolBar.LoadToolBar(IDR_SEND_TOOLBAR);
	ASSERT (ret);

	//enable the tooltips 
	ret = m_ctrlSendToolBar.EnableToolTips(TRUE);
	ASSERT (ret);
	ret = EnableToolTips(TRUE);
	ASSERT (ret);

	//** The OCX **
	RECT rect;

	rect.left	= 1;
	rect.top	= 1;
	rect.right	= 5000;
	rect.bottom	= 5000;

	if (!m_ocxChat.Create(NULL, WS_CHILD, rect, this, IDC_OCX_CHAT))
	{
		AfxMessageBox (IDS_FAIL_TO_LOAD_OCX);
		return -1;
	}

	//create the list of Chat rooms
	CString szTabName;
	szTabName.LoadString (IDS_ROOM_LIST_TAB);
	long iTab = AddTab(szTabName);
	m_ctrlTabWnd.ChangeImage(iTab, IMG_ROOM_LIST);
	m_ctrlRoomList.Create(	LVS_REPORT | LVS_SINGLESEL | LVS_SORTASCENDING,
							rect,
							&m_ctrlTabWnd,
							IDC_ROOM_LIST);
	ASSERT(m_ctrlRoomList);

	//create a history windows for private and server messages
	szTabName.LoadString (IDS_MESSAGES_TAB);
	iTab = AddTab(szTabName);
	m_ctrlTabWnd.ChangeImage(iTab, IMG_OFF);
	m_ctrlPrivateMsg.Create(WS_CHILD | WS_BORDER | WS_VSCROLL |
							ES_MULTILINE | ES_AUTOVSCROLL| WS_EX_NOPARENTNOTIFY, 
							rec, 
							&m_ctrlTabWnd,
							IDC_HISTORY_RICHEDIT); 
	ASSERT(m_ctrlPrivateMsg);

	//Log in a chat server
	OnRoomLogin();

	m_ctrlRoomList.ShowWindow(SW_SHOW);
	//insert the first chat room
	//OnFileNew();

	return 0;
}


void CChappyView::OnFileNew() 
{
	// new room
	CString	szTmp;
	szTmp.LoadString(IDS_CHAT_TAB);
	long iTab = AddTab(szTmp);
	m_ctrlTabWnd.ChangeImage(iTab, IMG_OFF);

	//create a new chat room
	m_ChatRoom[m_iLastTab] = (CRoom *) new CRoom;
	m_ChatRoom[m_iLastTab]->Create(m_ctrlTabWnd, m_ocxChat.GetChannels());

	if (!m_iLastTab) m_ChatRoom[m_iLastTab]->OnFocus(TRUE); // the first tab needs to get focus 

	//give focus to the new tab
	m_ctrlTabWnd.SetCurrentTab(m_iLastTab);

	//redraw and update status
	Invalidate();

}

void CChappyView::OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TRACE0("CChappyView::OnSelchangeTab1\n");
	
	int iOldTab;

	iOldTab = m_iVisibleTab;
	m_iVisibleTab = m_ctrlTabWnd.GetCurFocus();

	//hide them all
	m_ctrlRoomList.ShowWindow(SW_HIDE);
	m_ctrlPrivateMsg.ShowWindow(SW_HIDE);

	if ( (LIST_TAB != iOldTab) && (MESSAGE_TAB != iOldTab) )
	{
		if (m_ChatRoom[iOldTab]) 
		{
			m_ChatRoom[iOldTab]->OnFocus(FALSE);
		}
	}

	
	switch (m_iVisibleTab) 
	{
		case LIST_TAB:
			m_ctrlRoomList.ShowWindow(SW_SHOW);
			break;
		case MESSAGE_TAB:
			//server and private messages
			m_ctrlPrivateMsg.ShowWindow(SW_SHOW);
			m_ctrlTabWnd.ChangeImage(m_iVisibleTab, IMG_OFF);
			break;
		default:
			//all the other tabs are rooms
			if (m_ChatRoom[m_iVisibleTab])
			{
				m_ChatRoom[m_iVisibleTab]->OnFocus(TRUE);
				m_ctrlTabWnd.ChangeImage(m_iVisibleTab, IMG_OFF);
			}
	}

	

	Invalidate();

	*pResult = 0;
}

void CChappyView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	m_cx = cx;
	m_cy = cy;
}

BOOL CChappyView::OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult )
{
	TRACE0 ("OnToolTipNotify\n");

	// need to handle both ANSI and UNICODE versions of the message
	TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
	TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	UINT nID = pNMHDR->idFrom;
	TRACE ("  HWnd: %u\n",nID);
	
	if (((pNMHDR->code == TTN_NEEDTEXTA) || (pNMHDR->code == TTN_NEEDTEXTW)) && 
		(pTTTA->uFlags & TTF_IDISHWND) )
	{
        // idFrom is actually the HWND of the tool
        nID = ::GetDlgCtrlID((HWND)nID);
       	TRACE ("  ID: %u\n",nID);
	}

	if	( (nID == ID_BTN_SEND) ||
		  (nID == ID_BTN_WHISPER) )
	{
		CString szTipText;
		szTipText.LoadString(nID);

        //pTTT->lpszText =(LPTSTR) (LPCTSTR) szTipText;// MAKEINTRESOURCE(nID);
        pTTTA->hinst = AfxGetResourceHandle();
		pTTTW->hinst = AfxGetResourceHandle();
   
#ifndef _UNICODE
		if (pNMHDR->code == TTN_NEEDTEXTA)
			lstrcpyn(pTTTA->szText, (LPTSTR) (LPCTSTR) szTipText, _countof(pTTTA->szText));
		else
			_mbstowcsz(pTTTW->szText, (LPTSTR) (LPCTSTR) szTipText, _countof(pTTTA->szText));
#else
		if (pNMHDR->code == TTN_NEEDTEXTA)
			_wcstombsz(pTTTA->szText, (LPTSTR) (LPCTSTR) szTipText, _countof(pTTTA->szText));
		else
			lstrcpyn(pTTTW->szText, (LPTSTR) (LPCTSTR) szTipText, _countof(pTTTA->szText));
#endif

	// bring the tooltip window above other popup windows
	::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE|SWP_NOOWNERZORDER);

		return TRUE;
	}

    return FALSE;
}


void CChappyView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();
	
	BOOL ret = EnableToolTips(TRUE);
	ASSERT(ret);
	
}

//////////////////////////////////////////////////////////////////////////////
//  OCX Event handlers
//////////////////////////////////////////////////////////////////////////////

BEGIN_EVENTSINK_MAP(CChappyView, CView)
    //{{AFX_EVENTSINK_MAP(CChappyView)

	ON_EVENT(CChappyView, IDC_OCX_CHAT, 116 /* OnAddMember */, OnOnAddMemberMschatpr, VTS_DISPATCH VTS_BSTR VTS_DISPATCH)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 114 /* OnBeginEnumeration */, OnOnBeginEnumerationMschatpr, VTS_DISPATCH VTS_I4)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 113 /* OnChannelError */, OnOnChannelErrorMschatpr, VTS_DISPATCH VTS_I4 VTS_BSTR)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 123 /* OnChannelProperty */, OnOnChannelPropertyMschatpr, VTS_DISPATCH VTS_DISPATCH)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 124 /* OnChannelPropertyChanged */, OnOnChannelPropertyChangedMschatpr, VTS_DISPATCH VTS_BSTR VTS_BSTR VTS_VARIANT VTS_VARIANT)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 112 /* OnChannelState */, OnOnChannelStateMschatpr, VTS_DISPATCH VTS_I4)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 111 /* OnConnectionError */, OnOnConnectionErrorMschatpr, VTS_I4 VTS_BSTR VTS_VARIANT)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 110 /* OnConnectionState */, OnOnConnectionStateMschatpr, VTS_I4)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 117 /* OnDelMember */, OnOnDelMemberMschatpr, VTS_DISPATCH VTS_BSTR)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 115 /* OnEndEnumeration */, OnOnEndEnumerationMschatpr, VTS_DISPATCH VTS_I4)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 126 /* OnInvitation */, OnOnInvitationMschatpr, VTS_BSTR VTS_DISPATCH)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 127 /* OnKnock */, OnOnKnockMschatpr, VTS_DISPATCH VTS_DISPATCH VTS_I4)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 122 /* OnMemberKicked */, OnOnMemberKickedMschatpr, VTS_DISPATCH VTS_BSTR VTS_BSTR VTS_BSTR)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 120 /* OnMemberProperty */, OnOnMemberPropertyMschatpr, VTS_DISPATCH VTS_DISPATCH)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 121 /* OnMemberPropertyChanged */, OnOnMemberPropertyChangedMschatpr, VTS_DISPATCH VTS_BSTR VTS_BSTR VTS_BSTR VTS_VARIANT VTS_VARIANT)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 130 /* OnMessage */, OnOnMessageMschatpr, VTS_DISPATCH VTS_BSTR VTS_I4 VTS_VARIANT VTS_VARIANT VTS_VARIANT)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 128 /* OnPrivateMessage */, OnOnPrivateMessageMschatpr, VTS_BSTR VTS_I4 VTS_VARIANT VTS_VARIANT)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 131 /* OnProtocolMessage */, OnOnProtocolMessageMschatpr, VTS_BSTR VTS_BSTR VTS_VARIANT VTS_PBOOL)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 125 /* OnServerProperty */, OnOnServerPropertyMschatpr, VTS_DISPATCH)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 129 /* OnServerTextMessage */, OnOnServerTextMessageMschatpr, VTS_I4 VTS_BSTR)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 118 /* OnUserProperty */, OnOnUserPropertyMschatpr, VTS_DISPATCH)
	ON_EVENT(CChappyView, IDC_OCX_CHAT, 119 /* OnUserPropertyChanged */, OnOnUserPropertyChangedMschatpr, VTS_BSTR VTS_BSTR VTS_BSTR VTS_VARIANT VTS_VARIANT)


	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()


void CChappyView::OnOnMessageMschatpr(LPDISPATCH Channel, LPCTSTR SenderNickname, long MessageType, const VARIANT FAR& Message, const VARIANT FAR& RecipientNicknames, const VARIANT FAR& DataMessageTag)
{
	TRACE ("--- CChappyView::OnOnMessageMschatpr - %u %s %u %u \n", Channel, SenderNickname, MessageType, Message.vt );
	
	if (((VT_BSTR == Message.vt) || ((VT_BYREF | VT_VARIANT) == Message.vt))&& 
		((msgtNormal == MessageType) || (msgtWhisper == MessageType)) || (msgtAction == MessageType)) //comic chat sends info messages
	{
		CString szTmp;
		if (VT_BSTR == Message.vt)
		{
			szTmp = Message.bstrVal; 
		}
		else if ((VT_BYREF | VT_VARIANT) == Message.vt)
		{
			szTmp = Message.pvarVal->bstrVal;
		}

		long iTab = GetTabFromIndex(Channel);
		if (m_ChatRoom[iTab])
		{
			long iMT = IMG_OFF;
			if (iTab != m_iVisibleTab)
			{
				if (msgtNormal == MessageType) 
				{
					iMT = IMG_SAY;
				}
				else if (msgtWhisper == MessageType)
				{
					iMT = IMG_WHISPER;
				} 
				else if (msgtAction == MessageType)
				{
					iMT = IMG_DO;
				}
				m_ctrlTabWnd.ChangeImage(iTab, iMT);
			}

			m_ChatRoom[iTab]->m_ctrlHistoryWnd.UpdateMessageUI(SenderNickname, MessageType, szTmp);		
		}
	}
	
}

void CChappyView::OnOnPrivateMessageMschatpr(LPCTSTR SenderNickname, long PrivateMessageType, const VARIANT FAR& Message, const VARIANT FAR& DataMessageTag)
{
	TRACE0 ("--- CChappyView::OnOnPrivateMessageMschatpr\n");
	
	if ((VT_BSTR == Message.vt) && (0x20 > PrivateMessageType)) //comic chat sends info messages
	{
		CString szTmp(Message.bstrVal); 

		m_ctrlPrivateMsg.AddText("- Private message from ", TRUE);
		m_ctrlPrivateMsg.UpdateMessageUI(SenderNickname, PrivateMessageType, szTmp);		
		m_ctrlTabWnd.ChangeImage(1, IMG_IMPORTANT);
		//m_ChatRoom[m_iVisibleTab].AddText("--- End Private message ---", TRUE);
	}
}

void CChappyView::OnOnServerTextMessageMschatpr(long ServerMessageType, LPCTSTR Text) 
{
	TRACE0 ("--- CChappyView::OnOnServerTextMessageMschatpr\n");

	static long iCurrentType_s = -1;
	m_ctrlPrivateMsg.SetTextColor(RGB(0, 64, 128)); //nice blue
	//Post a message in the history control
	//CString szInMsg;
	if (ServerMessageType != iCurrentType_s)  
	{
		//print the message type only once
		iCurrentType_s = ServerMessageType;
		CString szTmp;
		szTmp.LoadString(IDS_SMT_NORMAL + ServerMessageType);
		m_ctrlPrivateMsg.AddText(szTmp, TRUE);	
	}
	m_ctrlPrivateMsg.AddText("\t");
	m_ctrlPrivateMsg.AddText(Text);
	m_ctrlPrivateMsg.AddText("\n");
	m_ctrlPrivateMsg.SetTextColor(GetSysColor (COLOR_WINDOWTEXT)); //default text color
}

void CChappyView::OnOnAddMemberMschatpr(LPDISPATCH Channel, LPCTSTR Nickname, LPDISPATCH MemberItems) 
{
	TRACE2 ("--- CChappyView::OnOnAddMemberMschatpr: Channel: %u Nickname: %s \n",Channel,Nickname);
	
	//add the user to the listview control
	HTREEITEM iTreeItem;
	long iTab = GetTabFromIndex(Channel);
	

	ASSERT (m_ChatRoom[iTab]);	
	iTreeItem = m_ChatRoom[iTab]->m_ctrlPartListWnd.InsertItem(Nickname);
	

	ASSERT (Channel);
	IChannelObject	IChannel(Channel);
	IChannel.m_bAutoRelease = FALSE;  //do not release the object when done

	//let's go get some of thouse hard to find properties
	IChatItems	ITmpChatItems = IChannel.GetMemberProperty("IPAddress", COleVariant(Nickname));
	ASSERT (ITmpChatItems);
	
	//let's see what the server returned
	COleSafeArray vValidItemsArray(ITmpChatItems.GetValidItems());
	
	TRACE1 ("Array Dimensions: %i \n",vValidItemsArray.GetDim());
	
	long iSize = (long) vValidItemsArray.GetOneDimSize();
	TRACE1 (" Number of elements: %i \n",iSize);

	HLOCAL element = LocalAlloc (GMEM_ZEROINIT | LMEM_FIXED ,vValidItemsArray.GetElemSize()+2);//+2 just to be safe
	for (long i = 0; i <iSize ; ++i)
	{	
		COleVariant data((LPCSTR)element);
		vValidItemsArray.GetElement(&i, (void *) data);
		CString szItemName (data.bstrVal);
		TRACE2 ("%u. Item Name: %s\n",i, szItemName);	
		COleVariant vItem(ITmpChatItems.GetItem(szItemName));
		
		CString szTextOut;
		if (VT_BSTR == vItem.vt)
		{
			CString szItem(vItem.bstrVal);
			szTextOut.Format("%s: %s", szItemName, szItem);	
		}
		else if ((VT_I2 == vItem.vt) || (VT_I4 == vItem.vt))
		{
			long iItem = (VT_I2 == vItem.vt) ? vItem.iVal : vItem.lVal;
			szTextOut.Format("%s: %u", szItemName, iItem);
		}
		
		if (!szTextOut.IsEmpty())
		{
			TRACE0 (szTextOut); 
			TRACE0 ("\n");
			
			//insert the item

			m_ChatRoom[iTab]->m_ctrlPartListWnd.InsertItem(szTextOut, iTreeItem);
			
		}
	}
	LocalFree(element);

	//Post a message in the history control
	CString szInMsg;
	CString szTmp;
	szTmp.LoadString(IDS_ADD_MEMBER);
	szInMsg.Format("%s %s\n\n",Nickname, szTmp); //blabla entered the room
	m_ChatRoom[iTab]->m_ctrlHistoryWnd.AddText(szInMsg, TRUE);
	
//	m_ctrlPartListWnd.InsertItem("Name", item);
//	m_ctrlPartListWnd.InsertItem("e-mail", item);


}

void CChappyView::OnOnDelMemberMschatpr(LPDISPATCH Channel, LPCTSTR Nickname) 
{
	TRACE0 ("--- CChappyView::OnOnDelMemberMschatpr\n");
	long iTab = GetTabFromIndex(Channel);
	
	ASSERT(m_ChatRoom[iTab]);
	m_ChatRoom[iTab]->m_ctrlPartListWnd.DeleteItemFromText (Nickname);

	//Post a message in the history control
	CString szInMsg;
	CString szTmp;
	szTmp.LoadString(IDS_DEL_MEMBER);
	szInMsg.Format("%s %s\n\n",Nickname, szTmp); //blabla exit the room
	m_ChatRoom[iTab]->m_ctrlHistoryWnd.AddText(szInMsg, TRUE);
	
}

void CChappyView::OnOnChannelStateMschatpr(LPDISPATCH Channel, long NewChannelState) 
{
	TRACE1 ("--- CChappyView::OnOnChannelStateMschatpr %u\n", NewChannelState);
	long iTab = GetTabFromIndex(Channel);

	ASSERT(m_ChatRoom[iTab]);

	if (chsClosed == NewChannelState)
	{
		m_ChatRoom[iTab]->DestroyRoom();
		//delete (m_ChatRoom[iTab]);
		//m_ctrlTabWnd.DeleteItem(iTab);

		//rename the tab
		CString szEmptyRoomTab;
		szEmptyRoomTab.LoadString(IDS_CHAT_TAB);
		m_ctrlTabWnd.RenameTab(iTab, szEmptyRoomTab);

	}
	
}

void CChappyView::OnOnConnectionStateMschatpr(long NewConnectionState) 
{
	TRACE1 ("--- CChappyView::OnOnConnectionStateMschatpr, State: %u\n", NewConnectionState);
	
	m_iServerConnectionState = NewConnectionState;
	m_iServerConnectionType = (csConnected <= m_iServerConnectionState) ? m_ocxChat.GetConnectionType() : -1;

	//as soon as we are logged in we display the list of rooms (if requested)
	if (m_bShowRoomList && (csLogged == m_iServerConnectionState)) 
	{
		OnRoomListrooms();
	}
	else if (csLogged == m_iServerConnectionState) //just once
	{
		//no room list
		CString szNoRoomList;
		szNoRoomList.LoadString(IDS_NO_ROOM_LIST_MSG);
		m_ctrlRoomList.AddColumn(szNoRoomList);	
	}
	else if (csDisconnected == m_iServerConnectionState) //just once
	{
		//clean up !!
		//CleanUp();
	}
}

void CChappyView::OnOnChannelPropertyMschatpr(LPDISPATCH Channel, LPDISPATCH ChannelItems) 
{
	TRACE1 ("--- CChappyView::OnOnChannelPropertyMschatpr: Channel: %u\n",Channel);

	static int iItemIndex = -1;

	if (NULL == Channel) 
	{
		//populate the room list
		CChatItems	ITmpChatItems (ChannelItems);
		ITmpChatItems.m_bAutoRelease = FALSE;
		ASSERT (ITmpChatItems);
		CString szType = ITmpChatItems.GetAssociatedType();

		//let's see what the server returned
		COleSafeArray vValidItemsArray(ITmpChatItems.GetValidItems());
		
		TRACE1 ("Array Dimensions: %i \n",vValidItemsArray.GetDim());
		
		long iSize = (long) vValidItemsArray.GetOneDimSize();
		TRACE1 (" Number of elements: %i \n",iSize);

		HLOCAL element = LocalAlloc (GMEM_ZEROINIT | LMEM_FIXED ,vValidItemsArray.GetElemSize()+2);//+2 just to be safe
		for (long i = 0; i <iSize ; ++i)
		{	
			COleVariant data((LPCSTR)element);
			vValidItemsArray.GetElement(&i, (void *) data);
			CString szItemName (data.bstrVal);
			TRACE2 ("%u. Item Name: %s\n",i, szItemName);	
			
			//insert the a new column if we do not have one already
			if ( i >= m_ctrlRoomList.GetNumOfColumns())
			{
				m_ctrlRoomList.AddColumn(szItemName);	
			}
			
			//get the item
			COleVariant vItem(ITmpChatItems.GetItem(szItemName));
			
			CString szTextOut;
			if (VT_BSTR == vItem.vt)
			{
				CString szItem(vItem.bstrVal);
				szTextOut.Format("%s", szItem);	
			}
			else if ((VT_I2 == vItem.vt) || (VT_I4 == vItem.vt))
			{
				long iItem = (VT_I2 == vItem.vt) ? vItem.iVal : vItem.lVal;
				szTextOut.Format("%u", iItem);
			}

			if (!szTextOut.IsEmpty())
			{
				TRACE0 (szTextOut); 
				TRACE0 ("\n");
				
				if (0 == i) // means that this is the room name
				{
					//insert the item
					m_ctrlRoomList.InsertNextItem(szTextOut);
				}
				else
				{
					//add the sub items to the current item
					m_ctrlRoomList.AddSubItem(i, szTextOut);
				}	
			}
		}
		LocalFree(element);	
	}
}

void CChappyView::OnOnMemberKickedMschatpr(LPDISPATCH Channel, LPCTSTR KickedNickname, LPCTSTR KickerNickname, LPCTSTR Reason) 
{
	TRACE0 ("--- CChappyView::OnOnMemberKickedMschatpr\n");	

	ASSERT (Channel);
	IChannelObject	IChannel(Channel);
	IChannel.m_bAutoRelease = FALSE;  //do not release the object when done

	CString szFormat;
	szFormat.LoadString(IDS_KICKED_MSG);
	CString szMessage;
	szMessage.Format ((LPCTSTR) szFormat, KickedNickname, IChannel.GetChannelName(), Reason);
	
	TRACE0 (szMessage);
	TRACE0 ("\n");

	AfxMessageBox(szMessage);
}

void CChappyView::OnOnChannelErrorMschatpr(LPDISPATCH Channel, long ErrorCode, LPCTSTR Description) 
{
	TRACE0 ("--- CChappyView::OnOnChannelErrorMschatpr\n");
	ASSERT (Channel);
	IChannelObject	IChannel(Channel);
	IChannel.m_bAutoRelease = FALSE;  //do not release the object when done

	CString szFormat;
	szFormat.LoadString(IDS_CHANNEL_ERROR_MSG);
	CString szMessage;
	szMessage.Format ((LPCTSTR) szFormat, IChannel.GetChannelName(), ErrorCode, Description);
	
	TRACE0 (szMessage);
	TRACE0 ("\n");

	AfxMessageBox(szMessage);
}

void CChappyView::OnOnConnectionErrorMschatpr(long ErrorCode, LPCTSTR Description, const VARIANT FAR& ChannelName) 
{
	TRACE0 ("--- CChappyView::OnOnConnectionErrorMschatpr\n");

	CString szFormat;
	szFormat.LoadString(IDS_CONNECTION_ERROR_MSG);
	CString szMessage;
	szMessage.Format ((LPCTSTR) szFormat, ErrorCode, Description);
	
	TRACE0 (szMessage);
	TRACE0 ("\n");

	AfxMessageBox(szMessage);
}


void CChappyView::OnOnChannelPropertyChangedMschatpr(LPDISPATCH Channel, LPCTSTR ModifierNickname, LPCTSTR ChannelPropertyName, const VARIANT FAR& OldChannelProperty, const VARIANT FAR& NewChannelProperty) 
{
	TRACE0 ("--- CChappyView::OnOnChannelPropertyChangedMschatpr\n");	
}

void CChappyView::OnOnServerPropertyMschatpr(LPDISPATCH ServerItems) 
{
	TRACE0 ("--- CChappyView::OnOnServerPropertyMschatpr\n");
}

void CChappyView::OnOnProtocolMessageMschatpr(LPCTSTR MessagePrefix, LPCTSTR MessageCommand, const VARIANT FAR& MessageParameters, BOOL FAR* EnableDefault) 
{
	TRACE0 ("--- CChappyView::OnOnProtocolMessageMschatpr\n");
}

void CChappyView::OnOnUserPropertyMschatpr(LPDISPATCH UserItems) 
{
	TRACE0 ("--- CChappyView::OnOnUserPropertyMschatpr\n");
}

void CChappyView::OnOnUserPropertyChangedMschatpr(LPCTSTR ModifiedNickname, LPCTSTR ModifierNickname, LPCTSTR UserPropertyName, const VARIANT FAR& OldUserProperty, const VARIANT FAR& NewUserProperty) 
{
	TRACE0 ("--- CChappyView::OnOnUserPropertyChangedMschatpr\n");
}

void CChappyView::OnOnMemberPropertyMschatpr(LPDISPATCH Channel, LPDISPATCH MemberItems) 
{
	TRACE0 ("--- CChappyView::OnOnMemberPropertyMschatpr\n");
}

void CChappyView::OnOnMemberPropertyChangedMschatpr(LPDISPATCH Channel, LPCTSTR ModifiedNickname, LPCTSTR ModifierNickname, LPCTSTR MemberPropertyName, const VARIANT FAR& OldMemberProperty, const VARIANT FAR& NewMemberProperty) 
{
	TRACE0 ("--- CChappyView::OnOnMemberPropertyChangedMschatpr\n");	
}

void CChappyView::OnOnInvitationMschatpr(LPCTSTR ChannelName, LPDISPATCH InviterItems) 
{
	TRACE0 ("--- CChappyView::OnOnInvitationMschatpr\n");
}
void CChappyView::OnOnKnockMschatpr(LPDISPATCH Channel, LPDISPATCH KnockerItems, long ErrorCode) 
{
	TRACE0 ("--- CChappyView::OnOnKnockMschatpr\n");
}

void CChappyView::OnOnBeginEnumerationMschatpr(LPDISPATCH Channel, long EnumType) 
{
	TRACE0 ("--- CChappyView::OnOnBeginEnumerationMschatpr\n");
}

void CChappyView::OnOnEndEnumerationMschatpr(LPDISPATCH Channel, long EnumType) 
{
	TRACE2 ("--- CChappyView::OnOnEndEnumerationMschatpr - %u %u\n", Channel, EnumType);

}


//////////////////////////////////////////////////////////////////////////////
//  Command message handler
//////////////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////////////
//  Sending chat messages
//////////////////////////////////////////////////////////////////////////////

void CChappyView::OnBtnSend() 
{
	PostChatMessage(msgtNormal);
}

void CChappyView::OnBtnWhisper() 
{
	PostChatMessage(msgtWhisper);
}

void CChappyView::OnBtnDo() 
{
	PostChatMessage(msgtAction);	
}

void CChappyView::OnBtnThink() 
{
	PostChatMessage(msgtThought);		
}

void CChappyView::PostChatMessage(long iMsgType_p)
{
	
	if (!IsRoomTab(m_iVisibleTab)) 
	{
		//not in a room tab
		return;
	}

	ASSERT(m_ChatRoom[m_iVisibleTab]);

	//select all
	m_ctrlInputWnd.SetSel(0,-1);
	if (SEL_EMPTY == m_ctrlInputWnd.GetSelectionType())
	{
		//notting to send
		return ;
	}
	
	//get the selection text
	CString szSendText = m_ctrlInputWnd.GetSelText();
	//The control clean itself on VK_RETURN but we need to clean it 
	//if we hit the toobar button
	m_ctrlInputWnd.Clear();

	//Send the text if we got any
	if ("" == szSendText) 
	{
		//notting to send
		return ;
	}
		
	m_ChatRoom[m_iVisibleTab]->SendMessage(m_szMyAlias, iMsgType_p, szSendText);

	//clean up the control
	m_ctrlInputWnd.SetSel(0,-1);
	m_ctrlInputWnd.Clear(); 
}

void CChappyView::OnHistoryClear() 
{
	TRACE("CChappyView::OnFileSave\n");

	ASSERT (m_iVisibleTab > LIST_TAB);

	CHistory	*ctrlVisibleHistory;
	
	if (m_iVisibleTab == MESSAGE_TAB)
	{
		ctrlVisibleHistory = &m_ctrlPrivateMsg;
	}
	else
	{
		ctrlVisibleHistory = m_ChatRoom[m_iVisibleTab]->GetRoomHistoryCtrlPtr();
	}

	ctrlVisibleHistory->ClearAll();

}


//////////////////////////////////////////////////////////////////////////////
//  Log in and out of servers
//////////////////////////////////////////////////////////////////////////////

void CChappyView::OnRoomLogin() 
{
	ASSERT (m_ocxChat);

	long iState = GetServerConnectionState();

	//we enable the menu only if we are disconnected or logged
	ASSERT ( (csLogged == iState) || (csDisconnected == iState));

	CLogInDlg LogInDlg;
	
	
	LogInDlg.m_szNickname.LoadString(IDS_DEFAULT_NICKNAME);
	LogInDlg.m_szServerName.LoadString(IDS_DEFAULT_SERVER); 

	int iRet = LogInDlg.DoModal();
	if (IDOK == iRet)
	{
		//this will tell us if we display the room list at startup
		m_bShowRoomList = LogInDlg.m_bShowRoomList;

		COleVariant vRealName;//(_T("myrealname"));
		COleVariant vUserName; //the ocx will get windows user name
		COleVariant vSecurity;(_T("NTLM;ALL"));
		COleVariant vPassword(LogInDlg.m_szPassword);
		//COleVariant vUserModes(1L,VT_I4);
		m_szMyAlias	= LogInDlg.m_szNickname;
		m_ocxChat.Login(LogInDlg.m_szServerName, LogInDlg.m_szNickname, vUserName , vRealName, vPassword, vSecurity);
		//clean the list view
		m_ctrlRoomList.ClearList();
	}
}


void CChappyView::OnRoomLogout() 
{
	long iState = GetServerConnectionState();

	ASSERT (csDisconnected != iState);
	ASSERT (csDisconnecting != iState);

	m_ctrlPrivateMsg.AddText("\n");

	if ( (csConnecting == iState) ||
		 (csLogging == iState) )
	{
		m_ocxChat.CancelConnectLogin();
	}
	else
	{
		m_ocxChat.Disconnect();
	}

	//clean the list view
	m_ctrlRoomList.ClearList();
}


//////////////////////////////////////////////////////////////////////////////
//  Enter and exit rooms
//////////////////////////////////////////////////////////////////////////////

void CChappyView::OnRoomListEnterRoom() 
{
	
	CString szRoomName = m_ctrlRoomList.GetItemText(m_ctrlRoomList.GetSelectedItem(), 0);
	
	CPWordDlg	dlgPW;
	if (IDOK == dlgPW.DoModal())
	{	
		//add the tab
		OnFileNew();
		//enter room
		EnterRoom(m_iLastTab, szRoomName, dlgPW.m_szPassword);
	}
	
}

void CChappyView::EnterRoom(long iTab, CString & szRoom, CString & szPassword)
{
	//we never use the fist 2 items in the array of rooms
	ASSERT(m_ChatRoom[iTab]);

	m_ChatRoom[iTab]->EnterRoom(iTab, szRoom, szPassword);

	//if I succed change the name of the tab
	m_ctrlTabWnd.RenameTab(iTab, szRoom);
	m_ctrlTabWnd.SetCurrentTab(m_iLastTab);

	//The tab grabs focus and covers the history, a repaint fixes it
	Invalidate();
}

void CChappyView::OnRoomConnect() 
{
	ASSERT(m_ocxChat);

	CEnterDlg	dlgEnter;
		
	int iDlgRet = dlgEnter.DoModal();

	if (iDlgRet == IDOK)
	{

		TRACE1("Room URL: %s\n",(LPCTSTR) dlgEnter.m_szRoom);
		
		//enter room
		EnterRoom(m_iVisibleTab, dlgEnter.m_szRoom, dlgEnter.m_szPassword);

	}
	else
	{
		if (iDlgRet != IDCANCEL)
		{
			AfxMessageBox ("Error Displaying the dialog: %i", iDlgRet);
		}
	}
	
}

void CChappyView::OnRoomDisconnect() 
{
	if (csLogged == m_iServerConnectionState)
	{
		//exit the room
		ASSERT (m_ChatRoom[m_iVisibleTab]);
		m_ChatRoom[m_iVisibleTab]->LeaveChannel();
	}
}


//////////////////////////////////////////////////////////////////////////////
//  UI Update handlers
//////////////////////////////////////////////////////////////////////////////

void CChappyView::OnUpdateRoomConnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable ((csLogged == m_iServerConnectionState)	&&
					(IsRoomTab(m_iVisibleTab))				&&
					(chsClosed == GetCurrentChannelState())) ;
	
}

void CChappyView::OnUpdateRoomDisconnect(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable ((csLogged == m_iServerConnectionState)	&& 
					(IsRoomTab(m_iVisibleTab))				&&
					(chsClosed != GetCurrentChannelState()));
}


void CChappyView::OnUpdateRoomLogin(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable (csDisconnected == m_iServerConnectionState);
	
}

void CChappyView::OnUpdateRoomLogout(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable ((csDisconnected != m_iServerConnectionState) &&
					(csDisconnecting != m_iServerConnectionState));	

	CString szNewMenuText;
	if ((csConnecting == m_iServerConnectionState) ||
		(csLogging == m_iServerConnectionState))
	{
		szNewMenuText.LoadString(IDS_MNU_CANCEL_LOGGING);
	}
	else
	{
		szNewMenuText.LoadString(IDS_MNU_LOGOUT);
	}

	pCmdUI->SetText(szNewMenuText);

}

void CChappyView::OnUpdateRoomListEnterRoom(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable( (-1 != m_ctrlRoomList.GetSelectedItem()) );	
}

void CChappyView::OnUpdateRoomListRooms(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable ((csDisconnected != m_iServerConnectionState) &&
					(csDisconnecting != m_iServerConnectionState));	

}

void CChappyView::OnUpdatePartlistKick(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((IsRoomTab(m_iVisibleTab))	&&
					(!m_ChatRoom[m_iVisibleTab]->GetSelectedMember().IsEmpty()));
}

void CChappyView::OnUpdatePartlistBan(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((IsRoomTab(m_iVisibleTab))	&&
					(!m_ChatRoom[m_iVisibleTab]->GetSelectedMember().IsEmpty()));
	
}

void CChappyView::OnUpdateServerBanuser(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable ((csDisconnected != m_iServerConnectionState) &&
					(csDisconnecting != m_iServerConnectionState));	
}

void CChappyView::OnUpdatePartlistMakehost(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((IsRoomTab(m_iVisibleTab))	&&
					(!m_ChatRoom[m_iVisibleTab]->GetSelectedMember().IsEmpty()));
	
}

void CChappyView::OnUpdatePartlistMakeparticipant(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable((IsRoomTab(m_iVisibleTab))	&&
					(!m_ChatRoom[m_iVisibleTab]->GetSelectedMember().IsEmpty()));
}

void CChappyView::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_iVisibleTab > LIST_TAB);
	
}

void CChappyView::OnUpdateHistoryClear(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_iVisibleTab > LIST_TAB);
}


//////////////////////////////////////////////////////////////////////////////
//  Room list
//////////////////////////////////////////////////////////////////////////////

void CChappyView::OnRoomListrooms() 
{
	TRACE0 ("--- CChappyView::OnRoomListrooms\n");
	
	//clean the list view
	m_ctrlRoomList.ClearList();

	CChatItems	ITmpChatItems = m_ocxChat.GetNewChatItems(); 
	ASSERT (ITmpChatItems);
	ITmpChatItems.SetAssociatedType("Query");
	m_ocxChat.ListChannels(ITmpChatItems);
	//the list is populated asyncronusly in OnOnChannelPropertyMschatpr
}

//////////////////////////////////////////////////////////////////////////////
//  Kick and Ban
//////////////////////////////////////////////////////////////////////////////

void CChappyView::OnPartlistKick() 
{
	CKickDlg	dlgKick;
	
	if (IDOK == dlgKick.DoModal()) 
	{
		m_ChatRoom[m_iVisibleTab]->KickMember(dlgKick.m_szReason);
	}
}

void CChappyView::OnPartlistBan() 
{
	m_ChatRoom[m_iVisibleTab]->BanMember(m_ocxChat.GetNewChatItems());
}

void CChappyView::OnServerBanuser() 
{
	//pop the dialog
	CBanDlg dlgBan;

	dlgBan.m_iBan		= 1;
	dlgBan.m_iOperation = 0;
	dlgBan.m_iProperty	= 0;
	dlgBan.m_iDuration	= 1;

	if ( IDOK == dlgBan.DoModal())
	{
		
		CChatItems	ITmpChatItems = m_ocxChat.GetNewChatItems(); 
		ASSERT (ITmpChatItems);
		ITmpChatItems.SetAssociatedType("Query");
	
		CString szProperty;
		szProperty.LoadString(IDS_IP_ADDRESS_ITEM+dlgBan.m_iProperty);
		ITmpChatItems.SetItem (szProperty, COleVariant(dlgBan.m_szValue));

		if (0 < dlgBan.m_iOperation)
		{
			CString szOperationName;
			szOperationName.LoadString(IDS_IP_ADDRESS_OP_NAME+dlgBan.m_iProperty);
			CString szOperation;
			szOperation.LoadString(IDS_CONTAINS_OP+dlgBan.m_iOperation-1);
			ITmpChatItems.SetItem (szOperationName, COleVariant(szOperation));
		}

		m_ocxChat.BanUsers(ITmpChatItems, dlgBan.m_iBan, COleVariant(dlgBan.m_szReason), COleVariant(dlgBan.m_iDuration));

		
	}	
}

//////////////////////////////////////////////////////////////////////////////
//  change members property
//////////////////////////////////////////////////////////////////////////////

void CChappyView::OnPartlistMakehost() 
{
	m_ChatRoom[m_iVisibleTab]->MakeHost(TRUE);	
}

void CChappyView::OnPartlistMakeparticipant() 
{
	m_ChatRoom[m_iVisibleTab]->MakeHost(FALSE);	
}



//////////////////////////////////////////////////////////////////////////////
//Utilities
//////////////////////////////////////////////////////////////////////////////

long CChappyView::GetIndexFromChannel(LPDISPATCH Channel_p)
{
	for (int i = 0; i <= m_iLastTab; ++i)
	{
		
		if	(m_ChatRoom[i] &&  
			(Channel_p == m_ChatRoom[i]->GetChannel())) 
		{
			return m_ChatRoom[i]->GetIndex();
		}
	}
	
	return -1;

//
//	ASSERT (Channel_p);
//	IChannelObject	IChannel(Channel_p);
//	IChannel.m_bAutoRelease = FALSE;  //do not release the object when done
//	//The tabs are in sync with the indexes
//	return (IChannel.GetIndex());

}

long CChappyView::AddTab(CString & szTitle_p)
{
	m_iLastTab++;
	
	//insert a new tab
	TC_ITEM tcItem;
	tcItem.mask = TCIF_TEXT;

	char szTabTitle[256];
	sprintf(szTabTitle, " %s ",(LPCTSTR) szTitle_p); //e.g." Rooms "
	tcItem.pszText = szTabTitle;

	int ret =  m_ctrlTabWnd.InsertItem(m_iLastTab ,&tcItem);
	ASSERT (m_iLastTab==ret);
	return m_iLastTab;
}

//|void CChappyView::CleanUp()
//|{
//|	for (long i=0; i < MAX_ROOMS; ++i)
//|	{
//|		if (m_ChatRoom[i])
//|		{
//|			m_ChatRoom[i]->DestroyRoom();
//|
//|			CString szEmptyRoomTab;
//|			szEmptyRoomTab.LoadString(IDS_CHAT_TAB);
//|			m_ctrlTabWnd.RenameTab(i, szEmptyRoomTab);
//|		}
//|	}
//|}

//////////////////////////////////////////////////////////////////////////////
//  SAVE
//////////////////////////////////////////////////////////////////////////////

void CChappyView::OnFileSave() 
{
	TRACE("CChappyView::OnFileSave\n");

	ASSERT (m_iVisibleTab > LIST_TAB);

	CHistory	*ctrlVisibleHistory;
	
	if (m_iVisibleTab == MESSAGE_TAB)
	{
		ctrlVisibleHistory = &m_ctrlPrivateMsg;
	}
	else
	{
		ctrlVisibleHistory = m_ChatRoom[m_iVisibleTab]->GetRoomHistoryCtrlPtr();
	}

	ctrlVisibleHistory->OnFileSave();

}

