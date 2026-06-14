#ifndef __RTFCTRL_H__
#define __RTFCTRL_H__

// Created		: RegisB, 08/28/97
//
// RtfCtrl.H	: header file

#include "doskey.h"


/////////////////////////////////////////////////////////////////////////////
// CAccelTable 
// A little class that does accelerator translation, much the same way as
// TranslateAccelerator, but with two differences.
//	- It is not tied to a window/menu. It's up to the caller to decide 
//    whether the command for an accelerator key is available or disabled.
//  - It doesn't generate any messages - it just returns a command ID.
//    This gives the caller complete control over the accelerator.
// This is used by the RTF control to handle formatting key shortcuts.

class CAccelTable
{
public:
	CAccelTable(UINT nID) { m_nID = nID; m_pAccel = NULL; m_nCount = 0; }
	~CAccelTable() { free (m_pAccel); }
	UINT Lookup(UINT nMsg, UINT nChar, UINT nRepCount, UINT nFlags);
protected:
	UINT 	m_nID;
	LPACCEL m_pAccel;
	int 	m_nCount;
};

/////////////////////////////////////////////////////////////////////////////
// CRtfCtrl window

class CRtfCtrl : public CRichEditCtrl
{
	DECLARE_DYNCREATE(CRtfCtrl)

// Construction
public:
	CRtfCtrl();

// Attributes
	static SHORT	m_nFixedPitchIndex;
	static SHORT	m_nSymbolIndex;

	CToolBarCtrl*	m_pTBCtrl;
	INT				m_nBoldID;
	INT				m_nItalicID;
	INT				m_nUnderlineID;
	INT				m_nFixedPitchID;
	INT				m_nSymbolID;

	DWORD			m_dwStyles;
	DWORD			m_dwExStyles;
	
	BOOL			m_bDeleteFont;
	BOOL			m_bAcceptMultiLine;
	BOOL			m_bSelectAll;
	BOOL			m_bColorWnd;
	BOOL			m_bRerouteMenuInit;
	UINT			m_nPopupMenuIndex;

	CDWordArray*	m_prgdwFormatting;
	CFont			m_font, *m_pFont;
	COLORREF		m_crTextColor;
	CString			m_strText;
	CDosKey*		m_pDosKey;
	WORD 			m_wMenuFormatStyles;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRtfCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual			~CRtfCtrl();

	void			DefineDefaultCharFormat();
	void			UseDefaultCharFormat(BOOL bUpdateSelection = FALSE);
	void			SetStylesBeforeCreation(DWORD dwStyles, DWORD dwExStyles)
						{ m_dwStyles = dwStyles; m_dwExStyles = dwExStyles; }
	void			SwitchSelectionFormat(WORD wFormat);
	void			MatchButtonsToSelection();
	void			ShowFormattingPopUp(LONG x, LONG y);
	void			SetDosKey(CDosKey* pDosKey) { m_pDosKey = pDosKey; }
	CDosKey*		GetDosKey() { return m_pDosKey; }
	void			SetToolBarData(CToolBarCtrl *pTBCtrl, INT nBoldID, INT nItalicID, INT nUnderlineID, INT nFixedPitchID, INT nSymbolID);
	void			SetAcceptMultiLine(BOOL bAcceptMultiLine)
						{ m_bAcceptMultiLine = bAcceptMultiLine; }
	BOOL			bSetWindowFormattedText(CString strIn, CDWordArray *prgdwFormatting);
	BOOL			bSetTextColor(COLORREF crTextColor);
	BOOL			bSetIndent(LONG lIndent);
	BOOL			bShowDosKeyEntry(BOOL bPrevious);
	WORD			wGetConsistentFormats(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CRtfCtrl)
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void	OnSetFocus(CWnd* pOldWnd);
	afx_msg void	OnKillFocus(CWnd* pNewWnd);
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);

	afx_msg void	OnSetColor();
	afx_msg void	OnSwitchBold();
	afx_msg void	OnSwitchItalic();
	afx_msg void	OnSwitchUnderlined();
	afx_msg void	OnSwitchFixedPitch();
	afx_msg void	OnSwitchSymbol();
	afx_msg void	OnUpdateSwitchBold(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateSwitchItalic(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateSwitchUnderlined(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateSwitchFixedPitch(CCmdUI* pCmdUI);
	afx_msg void	OnUpdateSwitchSymbol(CCmdUI* pCmdUI);
	afx_msg void	OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void 	OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	afx_msg void 	OnEnterIdle(UINT nWhy, CWnd* pWho);
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_MSG
    virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif __RTFCTRL_H__
