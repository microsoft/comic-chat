//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       C2View.h : interface of the CChat2000View class
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

//{{AFX_INCLUDES()
#include "mschat.h"
//}}AFX_INCLUDES

#include "partic.h"

#define POPUPMENU	3

class CChat2000View : public CView
{
protected: // create from serialization only
	CChat2000View();
	DECLARE_DYNCREATE(CChat2000View)

// Attributes
public:
	CString GetHistory();
	CChat2000Doc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChat2000View)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChat2000View();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//the OCX !
	CMSChat	m_ocxChat;
	
	//OCX does not expose the list of participants, we have to do the job
	//		I did not put the list into the OCX itself to avoid an not-so-elegant
	//		passing back and forth notifications from the VIEW to the OCX
	CList <CParticipant, CParticipant&> m_ListOfPart;
	POSITION m_posThisParticipant;
	
	//used to keep the video device caps to resize the OCX
	int		m_iXppli;
	int		m_iYppli;
	
	//track if the view has focus or not. Useful to pop up menus
	BOOL	m_bActive;

	//participant list manager
	void			AddParticipant		(CParticipant &Part_p);
	void			AddParticipant		(long ParticipantID, LPCTSTR Alias, short Status);
	void			DelParticipantID	(short iID_p);
	void			DelAll				();
	POSITION		FindParticipantID	(short iID_p);
	CParticipant	*GetParticipantPos	(POSITION Pos_p);
	CParticipant	*GetParticipantID	(short iID_p);
	BOOL			IsValidPos			(POSITION Pos_p);
	CString			GetParticipantAlias	();


// Generated message map functions
protected:
	//{{AFX_MSG(CChat2000View)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnChatEnterRoom();
	afx_msg void OnChatExitRoom();
	afx_msg void OnUpdateChatEnterRoom(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChatExitRoom(CCmdUI* pCmdUI);
	afx_msg void OnOnStateChangedOcxChat(short NewState);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnParentNotify(UINT message, LPARAM lParam);
	afx_msg void OnChatAboutmschat();
	afx_msg void OnChatKickParticipant();
	afx_msg void OnUpdateChatKickparticipant(CCmdUI* pCmdUI);
	afx_msg void OnOnParticipantStatusChangedOcxChat(long ParticipantID, short Status);
	afx_msg void OnOnBeginEnumerationOcxChat();
	afx_msg void OnOnEndEnumerationOcxChat();
	afx_msg void OnOnExitParticipantOcxChat(long ParticipantID);
	afx_msg void OnOnEnterParticipantOcxChat(long ParticipantID, LPCTSTR Alias, short Status);
	afx_msg void OnChatClearhistory();
	afx_msg void OnChatBanparticipant();
	afx_msg void OnUpdateChatBanparticipant(CCmdUI* pCmdUI);
	afx_msg void OnChatShowinfo();
	afx_msg void OnOnErrorOcxChat(short ErrorCode, LPCTSTR Description);
	afx_msg void OnChatSetroomtopic();
	afx_msg void OnUpdateChatSetroomtopic(CCmdUI* pCmdUI);
	DECLARE_EVENTSINK_MAP() //needed to trap events from the OCX
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void UpdateStatusUI(short NewState) ;
	void UpdateStatusUI() ;
};

#ifndef _DEBUG  // debug version in C2View.cpp
inline CChat2000Doc* CChat2000View::GetDocument()
   { return (CChat2000Doc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

