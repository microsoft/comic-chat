#ifndef __RTFCTRL_H__
#define __RTFCTRL_H__

// Created		: RegisB, 08/28/97
//
// RtfCtrl.H	: header file

const char chCtrlB	= 0x02;		// Bold
const char chCtrlD	= 0x04;		// Symbol
const char chCtrlF	= 0x06;		// Fixed Pitch
const char chCtrlI	= 0x09;		// Italic
const char chCtrlK	= 0x0B;		// Color
const char chCtrlU	= 0x15;		// Underline

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

	CDWordArray*	m_prgdwFormatting;
	CFont			m_font, *m_pFont;
	COLORREF		m_crTextColor;
	CString			m_strText;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRtfCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual			~CRtfCtrl();

	void			DefineDefaultCharFormat();
	void			UseDefaultCharFormat();
	void			SetStylesBeforeCreation(DWORD dwStyles, DWORD dwExStyles)
						{ m_dwStyles = dwStyles; m_dwExStyles = dwExStyles; }
	void			SwitchSelectionFormat(WORD wFormat);
	void			MatchButtonsToSelection();
	void			ShowFormattingPopUp(LONG x, LONG y);
	void			SetToolBarData(CToolBarCtrl *pTBCtrl, INT nBoldID, INT nItalicID, INT nUnderlineID, INT nFixedPitchID, INT nSymbolID);
	void			SetAcceptMultiLine(BOOL bAcceptMultiLine)
						{ m_bAcceptMultiLine = bAcceptMultiLine; }
	BOOL			bSetWindowFormattedText(CString strIn, CDWordArray *prgdwFormatting);
	BOOL			bSetTextColor(COLORREF crTextColor);
	BOOL			bSetIndent(LONG lIndent);
	WORD			wGetConsistentFormats(void);

	// Generated message map functions
protected:
	//{{AFX_MSG(CRtfCtrl)
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
	virtual BOOL	PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
#endif __RTFCTRL_H__
