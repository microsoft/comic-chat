//****************************************************************************
//  Module:     Chappy.EXE
//  File:       Room.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************
#ifndef __ROOM_H__
#define __ROOM_H__


#define IDC_OCX_CHAT	2000

#include "RoomHstr.h"
#include "MSChatx.h"
#include "PLTree.h"
#include "chtitems.h"


/////////////////////////////////////////////////////////////////////////////
// CRoom thread

class CRoom 
{

// Attributes
private:
	long m_iIndex;
	CString m_szKey;
	
	IChannelsCollection m_IChannels;
	IChannelObject		m_IChannel;

public:
	CRoomHistory	m_ctrlHistoryWnd;
	CPartListTree	m_ctrlPartListWnd;	


// Operations
public:
	void MakeHost(BOOL bFlag);
	void BanMember(CChatItems & IChatItem_p);
	void KickMember(CString & szReason_p);
	CString GetSelectedMember();
	//constructor and distructor
	CRoom(); 
	~CRoom();

	inline IChannelObject	GetChannelObj()			{ return m_IChannel;};
	inline LPDISPATCH		GetChannel()			{ return m_IChannel.m_lpDispatch;};
	inline long				GetIndex()				{ return m_iIndex;};
	inline CRoomHistory		*GetRoomHistoryCtrlPtr()	{ return &m_ctrlHistoryWnd;};

//	void operator =( const CRoom& src );
//	bool operator== ( const CRoom &src ) const;

	BOOL	Create(CWnd &wndParent_p, LPDISPATCH lpIChannels_p);
	BOOL	EnterRoom(long iIndex_p, CString &szRoomName_p, CString &szRoomKey_p);
	void	LeaveChannel();
	void	DestroyRoom();
	void	SendMessage(LPCTSTR SenderNickname_p, long iMsgType_p, CString &szMsgTxt_p);
	
	void	OnFocus(BOOL bFocus_p);
	
};

/////////////////////////////////////////////////////////////////////////////

#endif //__ROOM_H__