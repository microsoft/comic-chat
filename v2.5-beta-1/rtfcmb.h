#ifndef __RTFCMB_H__
#define __RTFCMB_H__

// Created		: RegisB, 02/16/98
//
// RtfCmb.H	: header file

#include "rtfctrl.h"

// #define WM_U_TAB	(WM_USER+36)

/////////////////////////////////////////////////////////////////////////////
// CRtfCmbEdit window

class CRtfCmbEdit : public CRtfCtrl
{
	DECLARE_DYNCREATE(CRtfCmbEdit)

// Construction
public:
	CRtfCmbEdit() { m_pParent = NULL; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRtfCmbEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual			~CRtfCmbEdit() {};

	void			SetParent(CWnd* pParent) { m_pParent = pParent; }

	// Generated message map functions
protected:
	//{{AFX_MSG(CRtfCmbEdit)
	afx_msg void	OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnKillFocus(CWnd* pNewWnd);
//	afx_msg UINT	OnGetDlgCode();
//	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Private attributes
	CWnd*	m_pParent;
};


/////////////////////////////////////////////////////////////////////////////
// CRtfCmb window

class CRtfCmb : public CComboBox
{
	DECLARE_DYNCREATE(CRtfCmb)

// Construction
public:
	CRtfCmb();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRtfCmb)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual			~CRtfCmb();
	CWnd*			RedirectFocus();
	CRtfCmbEdit*	GetRtfCmbEdit()	{ return m_pRtfCtrl; }
	BOOL			bSetRtfMode(BOOL bRtfMode);
	BOOL			bGetRtfMode()	{ return m_bRtfMode; }
	BOOL			bAttachRtfCtrl(UINT nID);
	void			RedirectSelection();

	void			SetWindowText(LPCTSTR szString);
	int				GetWindowText(LPTSTR szStringBuf, int nMaxCount) const;
	void			GetWindowText(CString& rString) const;
	BOOL			LimitText(int nMaxChars);


	// Generated message map functions
protected:
	//{{AFX_MSG(CRtfCmb)
	afx_msg void	OnDropDown();
	afx_msg void	OnCloseUp();
	afx_msg void	OnEditChange();
	afx_msg void	OnSelEndOK();
	afx_msg void	OnShowWindow(BOOL bShow, UINT nStatus);

//	afx_msg UINT	OnGetDlgCode();
//	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void	OnLButtonUp(UINT nFlags, CPoint point);

//	afx_msg LONG	OnTabOut(UINT u, LONG l);
//	afx_msg void	OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
//	afx_msg int		OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message);
//	afx_msg void	OnPaint();
//	afx_msg void	OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
//	afx_msg void	OnSetFocus(CWnd* pOldWnd);
//	afx_msg void	OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	// Private attributes
	BOOL			m_bAttached;
	BOOL			m_bRtfMode;
	CRtfCmbEdit*	m_pRtfCtrl;
};

/////////////////////////////////////////////////////////////////////////////
#endif __RTFCMB_H__
