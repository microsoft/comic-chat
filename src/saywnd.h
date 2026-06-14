// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#ifndef _SAYWND_H
#define _SAYWND_H

// saywnd.h : header file
//
/////////////////////////////////////////////////////////////////////////////
// Convenience macros

#define CHAR_RETURN	0xd
#define LINE_FEED	0xa
#define CTRL_B		0x2
#define CTRL_P		0x10
#define CTRL_Q		0x11
#define CTRL_U		0x15
#define CTRL_X		0x18
#define TAB			0x09
#define SHIFTTAB	0x0F
/////////////////////////////////////////////////////////////////////////////
// CSayCtrl window

class CSayCtrl : public CEdit
{
	DECLARE_DYNCREATE(CSayCtrl)

// Construction
public:
	CSayCtrl();

// Attributes
public:
//	UINT m_timerid;
//	void KillBotherTimer();
// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSayCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSayCtrl();
	BOOL IsEmpty();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSayCtrl)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
//	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnMaxtext();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CSayWnd window

class CSayWnd : public CFrameWnd
{
	DECLARE_DYNCREATE(CSayWnd)
protected:
	CSayCtrl	m_wndSayCtrl;
	CToolBar	m_wndGestBar;
	CToolBar	m_wndSayBar;
	CImageList	m_sayBarImages;		// DPI-scaled button images (high-DPI only)
	CBitmap		m_sayBarBmp;

// Construction
public:
	CSayWnd();

// Attributes
public:
   HWND GetSayEdit() 	{	return m_wndSayCtrl.m_hWnd;  };

// Operations
public:
	void SetFocusToSayWnd();
	BOOL TextEntered() { return(m_wndSayCtrl.LineLength()); }	// should be made more efficient (set a flag?)

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSayWnd)
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSayWnd();
	BOOL IsEmpty();

	// Generated message map functions
public:
	//{{AFX_MSG(CSayWnd)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateActions(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnActionsEmote();
	afx_msg void OnActionsSay();
	afx_msg void OnActionsThink();
	afx_msg void OnActionsWhisper();
//	afx_msg void OnUpdateActionsSay(CCmdUI* pCmdUI);
//	afx_msg void OnUpdateActionsWhisper(CCmdUI* pCmdUI);
	afx_msg void OnSendAction();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif 
