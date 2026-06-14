#ifndef _SAYWND_H
#define _SAYWND_H

// saywnd.h : header file
//

#include "rtfctrl.h"

#define CHAR_RETURN	0x0D
#define LINE_FEED	0x0A
#define CTRL_B		0x02
#define CTRL_L		0x0C
#define CTRL_P		0x10
#define CTRL_Q		0x11
#define CTRL_X		0x18
#define TAB			0x09
#define SHIFTTAB	0x0F

#define ID_SAYCTRL	1		// ID of child control SayCtrl

/////////////////////////////////////////////////////////////////////////////
// CSayCtrl window

class CSayCtrl : public CRtfCtrl
{
	DECLARE_DYNCREATE(CSayCtrl)

// Construction
public:
	CSayCtrl();
	BOOL	m_bWhisperSay;

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
	void	ValidateIMEEntry();
	BOOL	IsEmpty();
	BOOL	bEmptyAndIndent();
	
	// Generated message map functions
protected:
	//{{AFX_MSG(CSayCtrl)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnMaxtext();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
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
	CToolBar	m_wndSayBar;
	CFont*		m_fontText;

public:
	BOOL		m_bWhisperSay;
	UINT*		m_balloons;
	UINT		m_cntBalloons;
	UINT		m_cxSayBar;
	UINT		m_IDB_BALLOONS;

	CSayWnd();
	CSayWnd(BOOL bWhisperSay);

	HWND		GetSayEdit() 	{ return m_wndSayCtrl.m_hWnd; };
	BOOL		SetFont(LOGFONT &logFont, BOOL bMatchButtonsToSelection = FALSE);
	CFont*		GetFont()		{ return m_fontText; }
	COLORREF	GetTextColor()	{ return m_wndSayCtrl.m_crTextColor; }
	void		SetToolBarInfo(BOOL bWhisperSay);
	void		SetFormattingToolBarInfo(CToolBarCtrl *pTBCtrl, INT nBoldID, INT nItalicID, INT nUnderlineID, INT nFixedPitchID, INT nSymbolID);

// Operations
public:
	void	SetFocusToSayWnd() { m_wndSayCtrl.SetFocus(); }
	BOOL	TextEntered() { return(m_wndSayCtrl.LineLength()); }	// should be made more efficient (set a flag?)

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
	void SendReturn() { m_wndSayCtrl.SendMessage(WM_CHAR, CHAR_RETURN, 0); }
	void SwitchSelectionFormat(WORD wFormat);
	WORD wGetConsistentFormats() { return m_wndSayCtrl.wGetConsistentFormats(); }

	// Generated message map functions
public:
	//{{AFX_MSG(CSayWnd)
	afx_msg void OnSaySelChanged(NMHDR *pNotifyStruct, LRESULT *pLResult);
	afx_msg void OnSayFilter(NMHDR *pNotifyStruct, LRESULT *pLResult);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnUpdateActions(CCmdUI* pCmdUI);
	afx_msg void OnPaint();
	afx_msg void OnActionsSay();
	afx_msg void OnActionsThink();
	afx_msg void OnActionsWhisper();
	afx_msg void OnSendAction();
	afx_msg void OnPlaySound();
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern CSayWnd *GetSay();

#endif 
