//****************************************************************************
//  Module:     Chappy.EXE
//  File:       ChappyView.h : interface of the CChappyView class
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef __CHAPPYVIEW_H__
#define __CHAPPYVIEW_H__

#include "globals.h"

#define LIST_TAB		0
#define MESSAGE_TAB		1
#define MAX_ROOMS		50

#define _countof(array) (sizeof(array)/sizeof(array[0]))

#include "InputRE.h"
#include "RTab.h"
#include "TB.h"
#include "Room.h"
#include "ChppyDoc.h"
//#include "mschatx.h"
#include "mschatpr.h"
#include "RListCtr.h"
#include "History.h"

#define INPUT_RICHEDIT_SIZE 85
#define SEND_TB_SIZE		29 

class CChappyView : public CView
{
protected: // create from serialization only
	CChappyView();
	DECLARE_DYNCREATE(CChappyView)

// Attributes
public:
	CChappyDoc* GetDocument();

// Operations
public:
	//inline CWnd &GetHistoryCtrl()	{ASSERT (m_iVisibleTab); return m_ctrlHistoryWnd[m_iVisibleTab];};
	inline CWnd &GetTabCtrl()		{return m_ctrlTabWnd;};
	inline CWnd &GetToolBarCtrl()	{return m_ctrlSendToolBar;};
	inline CWnd &GetInputCtrl()		{return m_ctrlInputWnd;};

	inline long GetServerConnectionState()	{return m_iServerConnectionState;}; 
	inline long GetServerConnectionType()	{return m_iServerConnectionType;}; 
	inline long GetCurrentChannelState()	{return ((csLogged <= m_iServerConnectionState)			&& 
													 (IsRoomTab(m_iVisibleTab))						&&
													 (m_ChatRoom[m_iVisibleTab]->GetChannelObj()))
													? m_ChatRoom[m_iVisibleTab]->GetChannelObj().GetChannelState() 
													: chsClosed;};

	inline BOOL IsRoomTab(long iTabNumber)	{return (m_iVisibleTab > 1);};

	inline long GetTabFromIndex(LPDISPATCH Channel_p) { return (GetIndexFromChannel(Channel_p));};


	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChappyView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	//}}AFX_VIRTUAL

// Implementation
public:
	void EnterRoom(long iTab, CString &szRoom, CString &szPassword);
	long AddTab(CString &szTitle_p);
	
	long	m_iServerConnectionState;
	long	m_iServerConnectionType;

	void UpdateStatusUI();
	long GetIndexFromChannel(LPDISPATCH Channel_p);
	void PostChatMessage(long iMsgType_p);
	virtual ~CChappyView();

	BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:	
	BOOL			m_bShowRoomList;
	int				m_cy;			//window size
	int				m_cx;

	int				m_iLastTab;		//last tab added
	int				m_iVisibleTab;	//current tab selected

	CRoom			*m_ChatRoom[MAX_ROOMS];	//the list of rooms 
										//if you connect to more then 256 rooms you are a freak !
	
	//the chat OCX
	//IMsChatPr		m_IChatPr;
	CMsChatPr		m_ocxChat;

	CString			m_szMyAlias;

	CRoomsTab		m_ctrlTabWnd;
	CTB				m_ctrlSendToolBar;
	CInputREdit		m_ctrlInputWnd;
	CRoomListCtrl	m_ctrlRoomList;
	CHistory		m_ctrlPrivateMsg;

private:
	void CleanUp();

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CChappyView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelchangeTab1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnFileNew();
	afx_msg void OnDestroy();
	afx_msg void OnBtnSend();
	afx_msg void OnBtnWhisper();
	afx_msg void OnRoomConnect();
	afx_msg void OnBtnDo();
	afx_msg void OnBtnThink();
	afx_msg void OnRoomDisconnect();
	afx_msg void OnUpdateRoomConnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRoomDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnRoomLogin();
	afx_msg void OnUpdateRoomLogin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRoomLogout(CCmdUI* pCmdUI);
	afx_msg void OnRoomLogout();
	afx_msg void OnRoomListrooms();
	afx_msg void OnRoomListEnterRoom();
	afx_msg void OnUpdateRoomListEnterRoom(CCmdUI* pCmdUI);
	afx_msg void OnOnBeginEnumerationMschatpr(LPDISPATCH Channel, long EnumType);
	afx_msg void OnOnEndEnumerationMschatpr(LPDISPATCH Channel, long EnumType);
	afx_msg void OnOnMessageMschatpr(LPDISPATCH Channel, LPCTSTR SenderNickname, long MessageType, const VARIANT FAR& Message, const VARIANT FAR& RecipientNicknames, const VARIANT FAR& DataMessageTag);
	afx_msg void OnOnPrivateMessageMschatpr(LPCTSTR SenderNickname, long PrivateMessageType, const VARIANT FAR& Message, const VARIANT FAR& DataMessageTag);
	afx_msg void OnOnAddMemberMschatpr(LPDISPATCH Channel, LPCTSTR Nickname, LPDISPATCH MemberItems);
	afx_msg void OnOnDelMemberMschatpr(LPDISPATCH Channel, LPCTSTR Nickname);
	afx_msg void OnOnChannelStateMschatpr(LPDISPATCH Channel, long NewChannelState);
	afx_msg void OnOnConnectionStateMschatpr(long NewConnectionState);
	afx_msg void OnOnProtocolMessageMschatpr(LPCTSTR MessagePrefix, LPCTSTR MessageCommand, const VARIANT FAR& MessageParameters, BOOL FAR* EnableDefault);
	afx_msg void OnOnServerTextMessageMschatpr(long ServerMessageType, LPCTSTR Text);
	afx_msg void OnOnUserPropertyMschatpr(LPDISPATCH UserItems);
	afx_msg void OnOnUserPropertyChangedMschatpr(LPCTSTR ModifiedNickname, LPCTSTR ModifierNickname, LPCTSTR UserPropertyName, const VARIANT FAR& OldUserProperty, const VARIANT FAR& NewUserProperty);
	afx_msg void OnOnMemberPropertyMschatpr(LPDISPATCH Channel, LPDISPATCH MemberItems);
	afx_msg void OnOnMemberPropertyChangedMschatpr(LPDISPATCH Channel, LPCTSTR ModifiedNickname, LPCTSTR ModifierNickname, LPCTSTR MemberPropertyName, const VARIANT FAR& OldMemberProperty, const VARIANT FAR& NewMemberProperty);
	afx_msg void OnOnChannelPropertyMschatpr(LPDISPATCH Channel, LPDISPATCH ChannelItems);
	afx_msg void OnOnChannelPropertyChangedMschatpr(LPDISPATCH Channel, LPCTSTR ModifierNickname, LPCTSTR ChannelPropertyName, const VARIANT FAR& OldChannelProperty, const VARIANT FAR& NewChannelProperty);
	afx_msg void OnOnServerPropertyMschatpr(LPDISPATCH ServerItems);
	afx_msg void OnOnChannelErrorMschatpr(LPDISPATCH Channel, long ErrorCode, LPCTSTR Description);
	afx_msg void OnOnConnectionErrorMschatpr(long ErrorCode, LPCTSTR Description, const VARIANT FAR& ChannelName);
	afx_msg void OnOnInvitationMschatpr(LPCTSTR ChannelName, LPDISPATCH InviterItems);
	afx_msg void OnOnKnockMschatpr(LPDISPATCH Channel, LPDISPATCH KnockerItems, long ErrorCode);
	afx_msg void OnOnMemberKickedMschatpr(LPDISPATCH Channel, LPCTSTR KickedNickname, LPCTSTR KickerNickname, LPCTSTR Reason);
	afx_msg void OnUpdateRoomListRooms(CCmdUI* pCmdUI);
	afx_msg void OnPartlistKick();
	afx_msg void OnFileSave();
	afx_msg void OnUpdatePartlistBan(CCmdUI* pCmdUI);
	afx_msg void OnPartlistBan();
	afx_msg void OnServerBanuser();
	afx_msg void OnUpdateServerBanuser(CCmdUI* pCmdUI);
	afx_msg void OnUpdatePartlistKick(CCmdUI* pCmdUI);
	afx_msg void OnPartlistMakehost();
	afx_msg void OnUpdatePartlistMakehost(CCmdUI* pCmdUI);
	afx_msg void OnPartlistMakeparticipant();
	afx_msg void OnUpdatePartlistMakeparticipant(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnHistoryClear();
	afx_msg void OnUpdateHistoryClear(CCmdUI* pCmdUI);
	DECLARE_EVENTSINK_MAP()
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ChappyView.cpp
inline CChappyDoc* CChappyView::GetDocument()
   { return (CChappyDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

#endif //__CHAPPYVIEW_H__
