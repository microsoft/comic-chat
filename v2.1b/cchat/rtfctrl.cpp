// Created		: RegisB, 08/28/97
//
// RtfCtrl.cpp	: implementation file

#include "stdafx.h"
#include "rtfctrl.h"
#include "format.h"
#include "colordlg.h"
#include "chat.h"
#include "ccommon.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

extern "C" BYTE GetCorrectCharSet();

SHORT CRtfCtrl::m_nFixedPitchIndex = nGetSpecialFontIndex(TRUE /*bFixedPitchFont*/);
SHORT CRtfCtrl::m_nSymbolIndex = nGetSpecialFontIndex(FALSE /*bFixedPitchFont*/);

/////////////////////////////////////////////////////////////////////////////
// External statics

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRtfCtrl

IMPLEMENT_DYNCREATE(CRtfCtrl, CRichEditCtrl)

CRtfCtrl::CRtfCtrl()
{
	m_bAcceptMultiLine	= TRUE;
	m_bDeleteFont		= FALSE;

	//OutputDebugString("CRtfCtrl::CRtfCtrl - m_bSelectAll set to TRUE\n");
	//OutputDebugString("CRtfCtrl::CRtfCtrl - m_bColorWnd set to FALSE\n");

	m_bSelectAll		= TRUE;
	m_bColorWnd			= FALSE;
	m_pTBCtrl			= NULL;
	m_nBoldID			= 0;
	m_nItalicID			= 0;
	m_nUnderlineID		= 0;
	m_nFixedPitchID		= 0;
	m_nSymbolID			= 0;

	m_dwStyles			= 0L;
	m_dwExStyles		= 0L;

	m_prgdwFormatting	= NULL;
	m_pFont				= NULL;

	m_strText			= _T("");
	m_crTextColor		= GetSysColor(COLOR_WINDOWTEXT);
}


CRtfCtrl::~CRtfCtrl()
{
	if (m_prgdwFormatting)
	{
		m_prgdwFormatting->RemoveAll();
		delete m_prgdwFormatting;
	}
	if (m_bDeleteFont)
		m_font.DeleteObject();
}


void CRtfCtrl::SetToolBarData(CToolBarCtrl *pTBCtrl, INT nBoldID, INT nItalicID, INT nUnderlineID, INT nFixedPitchID, INT nSymbolID)
{
	ASSERT(pTBCtrl);

	m_pTBCtrl		= pTBCtrl;
	m_nBoldID		= nBoldID;
	m_nItalicID		= nItalicID;
	m_nUnderlineID	= nUnderlineID;
	m_nFixedPitchID	= nFixedPitchID;
	m_nSymbolID		= nSymbolID;
}


void CRtfCtrl::DefineDefaultCharFormat()
{
	LOGFONT	logFont;
	HFONT	hfont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	ASSERT(hfont);
	if (hfont)
	{
		GetObject(hfont, sizeof(LOGFONT), (LPVOID) &logFont);
		logFont.lfCharSet = GetCorrectCharSet();
		MatchFont(logFont);
		m_font.CreateFontIndirect(&logFont);
		m_bDeleteFont = TRUE;
		m_pFont = &m_font;

	}
}


void CRtfCtrl::UseDefaultCharFormat()
{
	if (m_pFont)
	{
		LOGFONT		logFont;
		CHARFORMAT	cfDefault;

		m_pFont->GetLogFont(&logFont);
		bLOGFONTToCHARFORMAT(&logFont, m_crTextColor, 0L, &cfDefault);
		SetDefaultCharFormat(cfDefault);
	}
}


BOOL CRtfCtrl::bSetIndent(LONG lIndent)
{
	PARAFORMAT	pf;

	pf.dwMask = PFM_STARTINDENT;
	pf.dxStartIndent = lIndent;
	pf.cbSize = sizeof(PARAFORMAT);

	return SetParaFormat(pf);
}


BEGIN_MESSAGE_MAP(CRtfCtrl, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRtfCtrl)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_SETCOLOR, OnSetColor)
	ON_COMMAND(ID_SWITCHBOLD, OnSwitchBold)
	ON_COMMAND(ID_SWITCHITALIC, OnSwitchItalic)
	ON_COMMAND(ID_SWITCHUNDERLINED, OnSwitchUnderlined)
	ON_COMMAND(ID_SWITCHFIXEDPITCH, OnSwitchFixedPitch)
	ON_COMMAND(ID_SWITCHSYMBOL, OnSwitchSymbol)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRtfCtrl message handlers

void CRtfCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	WORD wFormat = 0;

	if (GetKeyState(VK_CONTROL) & 0x8000)
	{
		if (1 == nRepCnt)
			switch (nChar)
			{
				default:
					break;

				case g_chLF:
					if (!m_bAcceptMultiLine)
						return;
					break;

				case chCtrlB:
					wFormat = wBold;
					break;

				case chCtrlI:
					wFormat = wItalic;
					break;

				case chCtrlU:
					wFormat = wUnderline;
					break;

				case chCtrlF:
					wFormat = wFixedPitch;
					break;

				case chCtrlD:
					wFormat = wSymbol;
					break;

				case chCtrlK:
					wFormat = wForeground;
			}
		else
			if (nChar == g_chLF && !m_bAcceptMultiLine)
				return;

	}

	if (wFormat)
		SwitchSelectionFormat(wFormat);
	else
		CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
}


void CRtfCtrl::OnSetFocus(CWnd* pOldWnd)
{
	if (m_bSelectAll)
	{
		//OutputDebugString("CRtfCtrl::OnSetFocus - Selecting all text\n");
		SetSel(0, GetTextLength());
	}

	CRichEditCtrl::OnSetFocus(pOldWnd);
}


void CRtfCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	if (!m_bColorWnd)
	{
		m_bSelectAll = TRUE;
		//OutputDebugString("CRtfCtrl::OnKillFocus - m_bSelectAll set to TRUE\n");
	}
	CRichEditCtrl::OnKillFocus(pNewWnd);
}


void CRtfCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	m_bSelectAll = FALSE;
	//OutputDebugString("CRtfCtrl::OnLButtonDown - m_bSelectAll set to FALSE\n");
	CRichEditCtrl::OnLButtonDown(nFlags, point);
}


void CRtfCtrl::OnSetColor()
{
	//OutputDebugString("OnSetColor\n");
	SwitchSelectionFormat(wForeground);
}


void CRtfCtrl::OnSwitchBold()
{
	//OutputDebugString("OnSwitchBold\n");
	SwitchSelectionFormat(wBold);
}


void CRtfCtrl::OnSwitchItalic()
{
	//OutputDebugString("OnSwitchItalic\n");
	SwitchSelectionFormat(wItalic);
}


void CRtfCtrl::OnSwitchUnderlined()
{
	//OutputDebugString("OnSwitchUnderlined\n");
	SwitchSelectionFormat(wUnderline);
}


void CRtfCtrl::OnSwitchFixedPitch()
{
	//OutputDebugString("OnSwitchFixedPitch\n");
	SwitchSelectionFormat(wFixedPitch);
}


void CRtfCtrl::OnSwitchSymbol()
{
	//OutputDebugString("OnSwitchSymbol\n");
	SwitchSelectionFormat(wSymbol);
}


void CRtfCtrl::ShowFormattingPopUp(LONG x, LONG y)
{
	CMenu menu;
	POINT screenPoint;
	WORD  wFormat = wGetConsistentFormats();

	screenPoint.x = x;
	screenPoint.y = y;
	ClientToScreen(&screenPoint);
	menu.LoadMenu(IDR_FORMATTING);

	if (wFormat & wBold)
		menu.GetSubMenu(0)->CheckMenuItem(ID_SWITCHBOLD, MF_CHECKED);
	if (wFormat & wItalic)
		menu.GetSubMenu(0)->CheckMenuItem(ID_SWITCHITALIC, MF_CHECKED);
	if (wFormat & wUnderline)
		menu.GetSubMenu(0)->CheckMenuItem(ID_SWITCHUNDERLINED, MF_CHECKED);
	if (wFormat & wFixedPitch)
		menu.GetSubMenu(0)->CheckMenuItem(ID_SWITCHFIXEDPITCH, MF_CHECKED);
	if (wFormat & wSymbol)
		menu.GetSubMenu(0)->CheckMenuItem(ID_SWITCHSYMBOL, MF_CHECKED);

	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
									   screenPoint.x, screenPoint.y, (CWnd*) this);
}


void CRtfCtrl::MatchButtonsToSelection()
{
	CHARFORMAT	cf;
	DWORD		dwMaskConsistent = GetSelectionCharFormat(cf);

	if ((dwMaskConsistent & CFM_BOLD) && (cf.dwEffects & CFE_BOLD))
	{
		// Check Bold button
		if (m_pTBCtrl && m_nBoldID > 0)
			m_pTBCtrl->CheckButton(m_nBoldID, TRUE);
	}
	else
	{
		// Uncheck Bold button
		if (m_pTBCtrl && m_nBoldID > 0)
			m_pTBCtrl->CheckButton(m_nBoldID, FALSE);
	}

	if ((dwMaskConsistent & CFM_ITALIC) && (cf.dwEffects & CFE_ITALIC))
	{
		// press Italic button
		if (m_pTBCtrl && m_nItalicID > 0)
			m_pTBCtrl->CheckButton(m_nItalicID, TRUE);
	}
	else
	{
		// release Italic button
		if (m_pTBCtrl && m_nItalicID > 0)
			m_pTBCtrl->CheckButton(m_nItalicID, FALSE);
	}

	if ((dwMaskConsistent & CFM_UNDERLINE) && (cf.dwEffects & CFE_UNDERLINE))
	{
		// press Underline button
		if (m_pTBCtrl && m_nUnderlineID > 0)
			m_pTBCtrl->CheckButton(m_nUnderlineID, TRUE);
	}
	else
	{
		// release Underline button
		if (m_pTBCtrl && m_nUnderlineID > 0)
			m_pTBCtrl->CheckButton(m_nUnderlineID, FALSE);
	}

	if (m_nFixedPitchIndex >= 0)
	{
		if ((dwMaskConsistent & CFM_FACE) && !_tcsicmp(cf.szFaceName, FIXEDPITCHFACENAMES[m_nFixedPitchIndex]))
		{
			// press FixedPitch button
			if (m_pTBCtrl && m_nFixedPitchID > 0)
				m_pTBCtrl->CheckButton(m_nFixedPitchID, TRUE);
			// release Symbol button
			if (m_pTBCtrl && m_nSymbolID > 0)
				m_pTBCtrl->CheckButton(m_nSymbolID, FALSE);
		}
		else
		{
			// release FixedPitch button
			if (m_pTBCtrl && m_nFixedPitchID > 0)
				m_pTBCtrl->CheckButton(m_nFixedPitchID, FALSE);
		}
	}

	if (m_nSymbolIndex >= 0)
	{
		if ((dwMaskConsistent & CFM_FACE) && !_tcsicmp(cf.szFaceName, SYMBOLFACENAMES[m_nSymbolIndex]))
		{
			// press Symbol button
			if (m_pTBCtrl && m_nSymbolID > 0)
				m_pTBCtrl->CheckButton(m_nSymbolID, TRUE);
			// release FixedPitch button
			if (m_pTBCtrl && m_nFixedPitchID > 0)
				m_pTBCtrl->CheckButton(m_nFixedPitchID, FALSE);
		}
		else
		{
			// release Symbol button
			if (m_pTBCtrl && m_nSymbolID > 0)
				m_pTBCtrl->CheckButton(m_nSymbolID, FALSE);
		}
	}
}


void CRtfCtrl::SwitchSelectionFormat(WORD wFormat)
{
	ASSERT(wFormat);

	CHARFORMAT	cf;
	DWORD		dwEffects = 0L, dwMask = 0L;
	DWORD		dwMaskConsistent = GetSelectionCharFormat(cf);
	static WORD	wLangID = 0;

	if (wFormat & wForeground)
	{
		LONG		lInitialColor = -1;
		COLORREF	cr;

		if (dwMaskConsistent & CFM_COLOR)
			lInitialColor = cf.crTextColor;

		CColorDlg colorDlg(lInitialColor, this);

		m_bColorWnd = TRUE;
		m_bSelectAll = FALSE;
		//OutputDebugString("CRtfCtrl::SwitchSelectionFormat - m_bColorWnd set to TRUE\n");
		//OutputDebugString("CRtfCtrl::SwitchSelectionFormat - m_bSelectAll set to FALSE\n");
		int iRet = colorDlg.DoModal();
		m_bColorWnd = FALSE;
		//OutputDebugString("CRtfCtrl::SwitchSelectionFormat - m_bColorWnd set to FALSE\n");

		if (colorDlg.GetSelectedColorRGB(&cr))
		{
			// make sure the user does not choose the backgound color!
			dwMask |= CFM_COLOR;

			if (cr == GetSysColor(COLOR_WINDOW))
				cf.crTextColor = GetSysColor(COLOR_WINDOWTEXT);
			else
				cf.crTextColor = cr;
		}
	}
	
	if (wFormat & wBold)
	{
		if ((dwMaskConsistent & CFM_BOLD) && !(cf.dwEffects & CFE_BOLD))
		{
			// Selection needs to be bolded
			dwMask |= CFM_BOLD;
			dwEffects |= CFE_BOLD;

			// Check Bold button
			if (m_pTBCtrl && m_nBoldID > 0)
				m_pTBCtrl->CheckButton(m_nBoldID, TRUE);
		}
		else
		{
			// Selection needs to be unbolded
			dwMask |= CFM_BOLD;

			// Uncheck Bold button
			if (m_pTBCtrl && m_nBoldID > 0)
				m_pTBCtrl->CheckButton(m_nBoldID, FALSE);
		}
	}

	if (wFormat & wItalic)
	{
		if ((dwMaskConsistent & CFM_ITALIC) && !(cf.dwEffects & CFE_ITALIC))
		{
			// Selection needs to be italized
			dwMask |= CFM_ITALIC;
			dwEffects |= CFE_ITALIC;

			// press Italic button
			if (m_pTBCtrl && m_nItalicID > 0)
				m_pTBCtrl->CheckButton(m_nItalicID, TRUE);
		}
		else
		{
			// Selection needs to be unitalized
			dwMask |= CFM_ITALIC;

			// release Italic button
			if (m_pTBCtrl && m_nItalicID > 0)
				m_pTBCtrl->CheckButton(m_nItalicID, FALSE);
		}
	}

	if (wFormat & wUnderline)
	{
		if ((dwMaskConsistent & CFM_UNDERLINE) && !(cf.dwEffects & CFE_UNDERLINE))
		{
			// Selection needs to be underlined
			dwMask |= CFM_UNDERLINE;
			dwEffects |= CFE_UNDERLINE;

			// press Underline button
			if (m_pTBCtrl && m_nUnderlineID > 0)
				m_pTBCtrl->CheckButton(m_nUnderlineID, TRUE);
		}
		else
		{
			// Selection needs to be un-underlined
			dwMask |= CFM_UNDERLINE;

			// release Underline button
			if (m_pTBCtrl && m_nUnderlineID > 0)
				m_pTBCtrl->CheckButton(m_nUnderlineID, FALSE);
		}
	}

	if ((wFormat & wFixedPitch) && m_nFixedPitchIndex >= 0)
	{
		if ((dwMaskConsistent & CFM_FACE) && _tcsicmp(cf.szFaceName, FIXEDPITCHFACENAMES[m_nFixedPitchIndex]))
		{
			// Selection needs to use a fixed pitch font
			dwMask |= CFM_FACE;
			_tcscpy(cf.szFaceName, FIXEDPITCHFACENAMES[m_nFixedPitchIndex]);
			cf.bPitchAndFamily = FF_DONTCARE | FIXED_PITCH;

			// press FixedPitch button
			if (m_pTBCtrl && m_nFixedPitchID > 0)
				m_pTBCtrl->CheckButton(m_nFixedPitchID, TRUE);
			// release Symbol button
			if (m_pTBCtrl && m_nSymbolID > 0)
				m_pTBCtrl->CheckButton(m_nSymbolID, FALSE);
		}
		else
		{
			// Selection needs to be un-fixed pitched
			dwMask |= CFM_FACE;
			// select the default font
			if (theApp.m_bComicView) 
			{
				if (FFixedPitchFont(theApp.m_comicsFont.lfFaceName) >= 0 || FSymbolFont(theApp.m_comicsFont.lfFaceName) >= 0 || (theApp.m_comicsFont.lfPitchAndFamily & FIXED_PITCH))
				{
					// use the default GUI face name
					strcpy(cf.szFaceName, theApp.m_szGuiFaceName);
					cf.bPitchAndFamily = theApp.m_lfGuiPitchAndFamily;
				}
				else
				{
					_tcscpy(cf.szFaceName, theApp.m_comicsFont.lfFaceName);
					cf.bPitchAndFamily = theApp.m_comicsFont.lfPitchAndFamily;
				}
			}
			else
			{
				if (FFixedPitchFont(theApp.m_textFont.lfFaceName) >= 0 || FSymbolFont(theApp.m_textFont.lfFaceName) >= 0 || (theApp.m_textFont.lfPitchAndFamily & FIXED_PITCH))
				{
					// use the default GUI face name
					strcpy(cf.szFaceName, theApp.m_szGuiFaceName);
					cf.bPitchAndFamily = theApp.m_lfGuiPitchAndFamily;
				}
				else
				{
					_tcscpy(cf.szFaceName, theApp.m_textFont.lfFaceName);
					cf.bPitchAndFamily = theApp.m_textFont.lfPitchAndFamily;
				}
			}

			// release FixedPitch button
			if (m_pTBCtrl && m_nFixedPitchID > 0)
				m_pTBCtrl->CheckButton(m_nFixedPitchID, FALSE);
		}
	}

	if ((wFormat & wSymbol) && m_nSymbolIndex >= 0)
	{
		UINT cp = GetACP();
		if ((dwMaskConsistent & CFM_FACE) && _tcsicmp(cf.szFaceName, SYMBOLFACENAMES[m_nSymbolIndex]))
		{
			// Selection needs to use a symbol font
			dwMask |= CFM_FACE;
			_tcscpy(cf.szFaceName, SYMBOLFACENAMES[m_nSymbolIndex]);
			cf.bPitchAndFamily &= ~FIXED_PITCH;
			cf.bPitchAndFamily &= ~VARIABLE_PITCH;

			wLangID = LOWORD(GetKeyboardLayout(0L));
 
			// Fix 4765 PRC and TC
			if (1253 != cp && 1251 != cp)
			{
				dwMask |= CFM_CHARSET;
				cf.bCharSet = SYMBOL_CHARSET;
			}

			// press Symbol button
			if (m_pTBCtrl && m_nSymbolID > 0)
				m_pTBCtrl->CheckButton(m_nSymbolID, TRUE);
			// release FixedPitch button
			if (m_pTBCtrl && m_nFixedPitchID > 0)
				m_pTBCtrl->CheckButton(m_nFixedPitchID, FALSE);
		}
		else
		{
			// Selection needs to be un-symbolized
			dwMask |= CFM_FACE;

			// Fix 4765 PRC and TC
			if (1253 != cp && 1251 != cp && wLangID != 0x0409)
			{
				dwMask |= CFM_CHARSET;
				cf.bCharSet = theApp.m_charSet;
			}

			// select the default font
			if (theApp.m_bComicView)
			{
				if (FFixedPitchFont(theApp.m_comicsFont.lfFaceName) >= 0 || FSymbolFont(theApp.m_comicsFont.lfFaceName) >= 0 || (theApp.m_comicsFont.lfPitchAndFamily & FIXED_PITCH))
				{
					// use the default GUI face name
					strcpy(cf.szFaceName, theApp.m_szGuiFaceName);
					cf.bPitchAndFamily = theApp.m_lfGuiPitchAndFamily;
				}
				else
				{
					_tcscpy(cf.szFaceName, theApp.m_comicsFont.lfFaceName);
					cf.bPitchAndFamily = theApp.m_comicsFont.lfPitchAndFamily;
				}
			}
			else
			{
				if (FFixedPitchFont(theApp.m_textFont.lfFaceName) >= 0 || FSymbolFont(theApp.m_textFont.lfFaceName) >= 0 || (theApp.m_textFont.lfPitchAndFamily & FIXED_PITCH))
				{
					// use the default GUI face name
					strcpy(cf.szFaceName, theApp.m_szGuiFaceName);
					cf.bPitchAndFamily = theApp.m_lfGuiPitchAndFamily;
				}
				else
				{
					_tcscpy(cf.szFaceName, theApp.m_textFont.lfFaceName);
					cf.bPitchAndFamily = theApp.m_textFont.lfPitchAndFamily;
				}
			}

			// release Symbol button
			if (m_pTBCtrl && m_nSymbolID > 0)
				m_pTBCtrl->CheckButton(m_nSymbolID, FALSE);
		}
	}

	if (dwMask)
	{
		cf.dwEffects = dwEffects;
		cf.dwMask = dwMask;
		SetSelectionCharFormat(cf);
	}
}


WORD CRtfCtrl::wGetConsistentFormats(void)
{
	WORD		wFormat = 0;
	CHARFORMAT	cf;
	DWORD		dwMaskConsistent = GetSelectionCharFormat(cf);

	if ((dwMaskConsistent & CFM_BOLD) && (cf.dwEffects & CFE_BOLD))
		wFormat |= wBold;
	if ((dwMaskConsistent & CFM_ITALIC) && (cf.dwEffects & CFE_ITALIC))
		wFormat |= wItalic;
	if ((dwMaskConsistent & CFM_UNDERLINE) && (cf.dwEffects & CFE_UNDERLINE))
		wFormat |= wUnderline;
	if ((m_nFixedPitchIndex >= 0) &&
		(dwMaskConsistent & CFM_FACE) && 
		!_tcsicmp(cf.szFaceName, FIXEDPITCHFACENAMES[m_nFixedPitchIndex]))
		wFormat |= wFixedPitch;
	if ((m_nSymbolIndex >= 0) &&
		(dwMaskConsistent & CFM_FACE) && 
		!_tcsicmp(cf.szFaceName, SYMBOLFACENAMES[m_nFixedPitchIndex]))
		wFormat |= wSymbol;

	return wFormat;
}


BOOL CRtfCtrl::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style	 |= m_dwStyles;
	cs.dwExStyle |= m_dwExStyles;
	return CRichEditCtrl::PreCreateWindow(cs);
}


BOOL CRtfCtrl::bSetWindowFormattedText(CString strIn, CDWordArray *prgdwFormatting)
{
	if (!prgdwFormatting || !prgdwFormatting->GetSize() || !m_pFont)
		SetWindowText(strIn);
	else
	{
		INT			iFormat;
		WORD		wCurOffset, wNextOffset, wCurFormat = 0, wNextFormat;
		LOGFONT		lf;
		CHARFORMAT	cfChunk;
		LPTSTR		szChunk;
		TCHAR		szFaceName[LF_FACESIZE];
		BYTE		bytePitchAndFamily;
		DWORD		dwElement;
		LPCTSTR		szIn = (LPCTSTR) strIn;

		SetWindowText("");

		if (!(szChunk = new TCHAR[_tcslen(szIn)+1]))
			return FALSE;

		m_pFont->GetLogFont(&lf);
		bLOGFONTToCHARFORMAT(&lf, m_crTextColor, 0L, &cfChunk);
		_tcscpy(szFaceName, lf.lfFaceName);
		bytePitchAndFamily = lf.lfPitchAndFamily;
		cfChunk.dwEffects = 0L;

		static short nFixedPitchIndex = nGetSpecialFontIndex(TRUE);
		static short nSymbolIndex = nGetSpecialFontIndex(FALSE);

		COLORREF	crDefTextColor = cfChunk.crTextColor;
		
		for (iFormat = 0, wCurOffset = 0; iFormat < prgdwFormatting->GetSize(); iFormat++, wCurOffset = wNextOffset)
		{
			dwElement = prgdwFormatting->GetAt(iFormat);
			wNextOffset = HIWORD(dwElement);
			if (wNextOffset-wCurOffset)
			{
				_tcsncpy(szChunk, szIn+wCurOffset, wNextOffset-wCurOffset);
				szChunk[wNextOffset-wCurOffset] = g_chEOS;
				SetSelectionCharFormat(cfChunk);
				ReplaceSel(szChunk);
			}
			
			wNextFormat = LOWORD(dwElement);
			
			if (wNextFormat & wBold)
				cfChunk.dwEffects |= CFE_BOLD;
			else
				cfChunk.dwEffects &= ~CFE_BOLD;

			if (wNextFormat & wItalic)
				cfChunk.dwEffects |= CFE_ITALIC;
			else
				cfChunk.dwEffects &= ~CFE_ITALIC;

			if (wNextFormat & wUnderline)
				cfChunk.dwEffects |= CFE_UNDERLINE;
			else
				cfChunk.dwEffects &= ~CFE_UNDERLINE;

			if (nFixedPitchIndex >= 0 && wNextFormat & wFixedPitch)
			{
				_tcscpy(cfChunk.szFaceName, FIXEDPITCHFACENAMES[nFixedPitchIndex]);
				cfChunk.bPitchAndFamily |= FIXED_PITCH;
				cfChunk.bPitchAndFamily &= ~VARIABLE_PITCH;
			}

			if (nSymbolIndex >= 0 && wNextFormat & wSymbol)
				_tcscpy(cfChunk.szFaceName, SYMBOLFACENAMES[nSymbolIndex]);

			if (
				(nFixedPitchIndex < 0 && wNextFormat & wFixedPitch) ||
				(nSymbolIndex < 0 && wNextFormat & wSymbol) ||
				(!(wNextFormat & wFixedPitch) && !(wNextFormat & wSymbol))
			   )
			{
				_tcscpy(cfChunk.szFaceName, szFaceName);
				cfChunk.bPitchAndFamily = bytePitchAndFamily;
			}

			if (wNextFormat & wForeground)
			{
				COLORREF crForeground = GetRBGColor((wNextFormat >> 4) & 0x000F);
				if (crForeground != GetSysColor(COLOR_WINDOW))
					cfChunk.crTextColor = crForeground;
				else
					cfChunk.crTextColor = crDefTextColor;

				if ((wNextFormat & wBackground) &&
					((wNextFormat >> 4) & 0x000F) == (wNextFormat & 0x000F))
					// Sender wants transparency
					cfChunk.crTextColor = GetSysColor(COLOR_WINDOW);

				//if (wNextFormat & wBackground)
				//{
				//	byteForeground = (wNextFormat >> 4) & 0x000F;
				//	cf2.crTextColor = GetRBGColor(byteForeground);
				//
				//	cf2.dwMask |= CFM_BACKCOLOR;
				//	byteBackground = wNextFormat & 0x000F;
				//	cf2.crBackColor = GetRBGColor(byteBackground);
				//	lr = ::SendMessage(m_hwnd, EM_SETCHARFORMAT, (WPARAM) SCF_SELECTION, (LPARAM) &cf2);		
				//	ASSERT(lr, "EM_SETCHARFORMAT >2< failed in CTextView::iDisplayMsgText");
				//}
			}
			else
				cfChunk.crTextColor = crDefTextColor;
		}
		SetSelectionCharFormat(cfChunk);
		ReplaceSel(szIn+wNextOffset);

		delete [] szChunk;
	}

	return TRUE;
}


BOOL CRtfCtrl::bSetTextColor(COLORREF crTextColor)
{
	CHARFORMAT	cf;

	ZeroMemory(&cf, sizeof(CHARFORMAT));
	cf.cbSize = sizeof(CHARFORMAT);
	cf.dwMask = CFM_COLOR;
	m_crTextColor = cf.crTextColor = crTextColor;
	return SetDefaultCharFormat(cf);
}

