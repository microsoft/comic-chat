/*
	*****************************************************************
	*																*
	*	Module 			: TextView.cpp, TextView for Microsoft Chat	*
	*																*
	*	Author 			: RamuM, 11/1/96							*
	*																*
	*	Current Owner	: RamuM										*
	*																*
	*	Changes			:											*
	*																*
	*	To Dos			: UniCode Support							*
	*					Support selective text buffer extraction	*
	*																*
	*****************************************************************
*/



//TODO: Support selective text buffer extraction
#ifndef __TEXTVIEW_H__
#include "textview.h"
#endif  __TEXTVIEW_H__

#include "CCommon.H"
#include "CDebug.H"
#include "winerror.h"

#include "TVRes.h"
#include "limits.h"
#include "format.h"
#include <winnls.h>

// for ASSERT and FAIL
//
SZTHISFILE

extern "C" BYTE GetCorrectCharSet();

#ifdef DEBUGCharFormat
void DumpCharFormat(CHARFORMAT* pcf, LPCTSTR szText)
{
	OutputDebugString(szText);

	if (pcf->dwMask & CFM_BOLD)
	{
		if (pcf->dwEffects & CFE_BOLD)
			OutputDebugString("Set BOLD\n");
		else
			OutputDebugString("Reset BOLD\n");
	}

	if (pcf->dwMask & CFM_COLOR)
	{
		if (pcf->dwEffects & CFE_AUTOCOLOR)
			OutputDebugString("Set AUTOCOLOR\n");
		else
			OutputDebugString("Reset AUTOCOLOR\n");

		_stprintf(g_szDebugStr, _T("Set Color %ld\n"), pcf->crTextColor);
		OutputDebugString(g_szDebugStr);
	}

	if (pcf->dwMask & CFM_FACE)
	{
		_stprintf(g_szDebugStr, _T("Set FontName %s\n"), pcf->szFaceName);
		OutputDebugString(g_szDebugStr);
	}

	if (pcf->dwMask & CFM_ITALIC)
	{
		if (pcf->dwEffects & CFE_ITALIC)
			OutputDebugString("Set ITALIC\n");
		else
			OutputDebugString("Reset ITALIC\n");
	}
		
	if (pcf->dwMask & CFM_OFFSET)
	{
		_stprintf(g_szDebugStr, _T("Set yOffSet %ld\n"), pcf->yOffset);
		OutputDebugString(g_szDebugStr);
	}

	if (pcf->dwMask & CFM_PROTECTED)
	{
		if (pcf->dwEffects & CFE_PROTECTED)
			OutputDebugString("Set PROTECTED\n");
		else
			OutputDebugString("Reset PROTECTED\n");
	}
		
	if (pcf->dwMask & CFM_SIZE)
	{
		_stprintf(g_szDebugStr, _T("Set yHeight %ld\n"), pcf->yHeight);
		OutputDebugString(g_szDebugStr);
	}

	if (pcf->dwMask & CFM_STRIKEOUT)
	{
		if (pcf->dwEffects & CFE_STRIKEOUT)
			OutputDebugString("Set STRIKEOUT\n");
		else
			OutputDebugString("Reset STRIKEOUT\n");
	}

	if (pcf->dwMask & CFM_UNDERLINE)
	{
		if (pcf->dwEffects & CFE_UNDERLINE)
			OutputDebugString("Set UNDERLINE\n");
		else
			OutputDebugString("Reset UNDERLINE\n");
	}

	if (pcf->dwMask & CFM_LINK)
	{
		if (pcf->dwEffects & CFE_LINK)
			OutputDebugString("Set LINK\n");
		else
			OutputDebugString("Reset LINK\n");
	}
}
#else
#define DumpCharFormat(pcf, szText)
#endif // DEBUG

// Can be optimized to remove these Global variables
CHARFORMAT gDefTextFormat = { sizeof(CHARFORMAT), 
								CFM_COLOR, 
								CFE_AUTOCOLOR } ;

CHARFORMAT gDefHeadFormat = { sizeof(CHARFORMAT), 
								CFM_COLOR, 
								0,
								0,0,RGB(0,0,255) } ;

CHARFORMAT gDefInfoFormat = { sizeof(CHARFORMAT), 
								CFM_COLOR, 
								0,
								0,0,RGB(0,128,128) } ;

CHARFORMAT gDefURLFormat  = { sizeof(CHARFORMAT), 
								CFM_COLOR|CFM_UNDERLINE|CFM_LINK, 
								CFE_LINK | CFE_UNDERLINE,
								0,0,RGB(0,0,255) } ;


LRESULT CALLBACK TVWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	TEXTVIEWCLASSNAME	*pCtrl = (TEXTVIEWCLASSNAME*) GetWindowLong(hwnd, GWL_USERDATA);

	ASSERT(pCtrl, "pCtrl is NULL");

#ifdef STRICT
	WNDPROC lpfnWinProc = pCtrl->m_lpfnProcTV;
#else 
	FARPROC lpfnWinProc = pCtrl->m_lpfnProcTV;
#endif // DEBUG 

#ifdef DEBUG

	TCHAR temp[256];
	BOOL bIcon, bVis;
	if (WM_STYLECHANGING == uMsg)
	{
		bIcon = IsIconic(hwnd);
		bVis = IsWindowVisible(hwnd);

		wsprintf(temp,"WM_STYLECHANGING: OldStyle %x, new Style %x (%d,%d) \r\n",((LPSTYLESTRUCT) lParam)->styleOld, ((LPSTYLESTRUCT) lParam)->styleNew,bIcon,bVis);
		OutputDebugThreadIdString(temp);
	}

	if (WM_STYLECHANGED == uMsg)
	{
		bIcon = IsIconic(hwnd);
		bVis = IsWindowVisible(hwnd);

		wsprintf(temp,"WM_STYLECHANGED: OldStyle %x, new Style %x (%d,%d) \r\n",((LPSTYLESTRUCT) lParam)->styleOld, ((LPSTYLESTRUCT) lParam)->styleNew,bIcon,bVis);
		OutputDebugThreadIdString(temp);
	}
#endif DEBUG

	if (uMsg == WM_VSCROLL)
	{
		int nScrollCode = (int) LOWORD(wParam); // scroll bar value 
		
		if ((SB_THUMBTRACK == nScrollCode)) // || 
//			(SB_THUMBPOSITION == nScrollCode))
		{
//			OutputDebugThreadIdString("TVWndProc - Thumb Track True\n");
			pCtrl->m_bThumb = TRUE;
		}
		else
		{
//			OutputDebugThreadIdString("TVWndProc - Thumb Track False\n");
			pCtrl->m_bThumb = FALSE;
		}
	}

    return CallWindowProc(lpfnWinProc, hwnd, uMsg, wParam, lParam);
}    


TEXTVIEWCLASSNAME::TEXTVIEWCLASSNAME()
{
	// Initialize the defaults;
	m_hwnd = NULL;
	m_hInstance = NULL;

	m_dwBuffSize = m_dwBuffMaxSize = 0;

	m_fBuffCutOff	= TEXT_VIEW_BUFFER_CUTOFF;
	m_fBuffFull		= TEXT_VIEW_BUFFER_INFORM_FULL;

	m_hInstRichEd32 = NULL;
	m_hCursorHand	= NULL;
	
	ZeroMemory(&m_cfFont, sizeof(CHARFORMAT));
	m_cfFont.cbSize = sizeof(CHARFORMAT);
	m_cfFont.dwMask	= CFM_FACE|CFM_SIZE|CFM_OFFSET|CFM_COLOR|CFM_BOLD|CFM_ITALIC|CFM_STRIKEOUT|CFM_UNDERLINE|CFM_CHARSET;
	m_cfFont.crTextColor = GetSysColor(COLOR_WINDOWTEXT);

//	m_nLineHeight	= m_nUpperMargin = 0;

	m_nInsertBlank	= TEXT_VIEW_BLANK_NEVER;
	m_nAutoScroll	= TEXT_VIEW_AUTOSCROLL_ALWAYS;

	m_bHeader		= FALSE;
	m_bCallHeader	= FALSE;
	m_bDBCSSystem	= FALSE;

	m_bHeaderSeparate = TRUE;

	m_bNewBrowser	= TRUE;

	m_mtLastMsgType	= mtURL;

//	m_nLogPixelsY = 0;

//	SetDefaultResourceIDs();
	ZeroMessageStrings();

	bSetDefaultMsgTypeProperties();
}



TEXTVIEWCLASSNAME::~TEXTVIEWCLASSNAME()
{
	if ( NULL != m_hInstRichEd32)	// Also means we created this window
	{
		::DestroyWindow(m_hwnd);
		::FreeLibrary(m_hInstRichEd32);
		m_hwnd = NULL;
	}

	if ( NULL != m_hwnd )	// We don't own this window, but since
							// have it destroy it also
	{
		ASSERT(m_hwnd != NULL, "m_hwnd is valid in CTextView::~CTextView");
		::DestroyWindow(m_hwnd);
	}

	if (NULL != m_hCursorHand)
		::DeleteObject(m_hCursorHand);

	// Delete if any allocated
	ClearDefaultMsgTypeProperties();

	// Clear all message Strings
	ClearMessageStrings();

}


BOOL TEXTVIEWCLASSNAME::bCreateTextViewWindow(TCHAR *szName, DWORD dwExStyle, DWORD dwStyle, 
									  int x, int y, int nWidth, int nHeight, 
									  HWND hwndParent, HINSTANCE hInstance)
{
	if ( NULL != m_hwnd )
		return FALSE;

	if (NULL == (m_hInstRichEd32 = ::LoadLibrary(TEXT_VIEW_WND_DLL)))
		return FALSE;

	HWND hwnd = CreateWindowEx(dwExStyle,
							TEXT_VIEW_WND_CLASS_NAME, 
							szName, 
							dwStyle|TEXT_VIEW_WND_FLAGS, 
							x, y, nWidth, nHeight, 
							hwndParent, NULL, hInstance, NULL);

	if (NULL != hwnd)	
		return AttachTextViewHWnd(hwnd,hInstance);
	else
		return FALSE;
}



BOOL TEXTVIEWCLASSNAME::AttachTextViewHWnd(HWND hwnd, HINSTANCE hInstance)
{
	ASSERT(hwnd,"hwnd is NULL in CTextView::AttachTextViewHWnd");
	ASSERT(hwnd,"hInstance is NULL in CTextView::AttachTextViewHWnd");

	if (NULL != m_hwnd)
		return FALSE;

	m_hInstance = hInstance;

	if (!(m_hCursorHand = ::LoadCursor(m_hInstance, MAKEINTRESOURCE(IDC_CURSORHAND))))
		m_hCursorHand = ::LoadCursor(NULL, IDC_ARROW);

	if (m_hwnd = hwnd)
	{
		SetWindowLong(m_hwnd, GWL_USERDATA, (long) this);

#ifdef STRICT
		m_lpfnProcTV = (WNDPROC) SetWindowLong(m_hwnd, GWL_WNDPROC, (long) TVWndProc);
#else
		m_lpfnProcTV = (FARPROC) SetWindowLong(m_hwnd, GWL_WNDPROC, (long) TVWndProc);
#endif // STRICT

		m_bThumb = FALSE;

		LOGFONT	lf;
		HFONT	hfont;
		UINT	uCharSet;
			
		hfont = (HFONT) ::GetStockObject(DEFAULT_GUI_FONT);
		::GetObject(hfont, sizeof(LOGFONT), (LPVOID) &lf);

		if (lf.lfCharSet != (uCharSet = GetCorrectCharSet()))
		{
			lf.lfCharSet = uCharSet;
			MatchFont(lf);
		}

		bLOGFONTToCHARFORMAT(&lf, m_cfFont.crTextColor, m_cfFont.dwMask, &m_cfFont);

		m_cfFont.dwMask |= CFM_LINK;	// Reset Links.
		::SendMessage(m_hwnd, WM_SETFONT, (WPARAM) hfont, MAKELPARAM(TRUE, 0) /* fRedraw */);

		//	::SendMessage(m_hwnd, EM_SETLIMITTEXT, (WPARAM) m_dwBuffMaxSize, 0L);
		m_dwBuffMaxSize = ::SendMessage(m_hwnd, EM_GETLIMITTEXT, 0, 0);

		DWORD dwMask = ::SendMessage(m_hwnd, EM_GETEVENTMASK, 0L, (LPARAM) ENM_LINK);
		::SendMessage(m_hwnd, EM_SETEVENTMASK, 0L, (LPARAM) (dwMask | ENM_LINK));

		bReSetMessageStrings();
		dwClearTextViewBuffer(0);	// in case the object is being reused.
		return TRUE;
	}
	else
		return FALSE;
}


HWND TEXTVIEWCLASSNAME::DetachTextViewHWnd()
{
	if (NULL == m_hwnd)
		return NULL;
	
	if (NULL != m_hInstRichEd32)
	{
		// Since we are detaching this window, we don't need to keep thsi
		::FreeLibrary(m_hInstRichEd32);
		m_hInstRichEd32 = NULL;
	}

	if (NULL != m_hCursorHand)
	{
		::DeleteObject(m_hCursorHand);
		m_hCursorHand = NULL;
	}

	// TODO: Should the Font also get deleted from the window?
//	if (NULL != m_hFont)
//		::DeleteObject(m_hFont);

	// Reset the Window Proc
	SetWindowLong(m_hwnd, GWL_WNDPROC, (long) m_lpfnProcTV);

	HWND hwnd = m_hwnd;
	m_hwnd = NULL;
	m_hInstance = NULL;

//	m_nLineHeight = m_nUpperMargin = 0;

	return hwnd;
}


BOOL TEXTVIEWCLASSNAME::bSetTextViewBufferMaxSize(DWORD dwBuffLength)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::bSetTextViewBufferMaxSize");

	::SendMessage(m_hwnd, EM_SETLIMITTEXT, (WPARAM) dwBuffLength, 0L);
	m_dwBuffMaxSize = ::SendMessage(m_hwnd, EM_GETLIMITTEXT, 0, 0);
	return m_dwBuffMaxSize == dwBuffLength;
}


DWORD TEXTVIEWCLASSNAME::dwGetTextViewBuffer(LPTSTR szBuffer, DWORD dwBuffLength)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::dwGetTextViewBuffer");

	return ::SendMessage(m_hwnd, WM_GETTEXT, (WPARAM) dwBuffLength, (LPARAM) szBuffer);
}


DWORD TEXTVIEWCLASSNAME::dwClearTextViewBuffer(DWORD dwMinCut)
{
	BOOL		bScroll, bRestoreSel, bShowWnd = FALSE, bFound = FALSE;
	CHARRANGE	crSav;
	LPTSTR		szHist = NULL, szHistTmp = NULL;
	DWORD		cbHist;

	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::dwClearTextViewBuffer");

	if (dwMinCut == 0)		// Clear the entire History
	{
		::SendMessage(m_hwnd, WM_SETTEXT, 0L, 0L);
		cbHist = m_dwBuffSize;
		m_dwBuffSize = 0L;
		return cbHist;
	}

	// Check for current Selection
	::SendMessage(m_hwnd, EM_EXGETSEL, 0L, (LPARAM) &crSav);
	if ((crSav.cpMin != crSav.cpMax) || (crSav.cpMin != (long) m_dwBuffSize))
		bRestoreSel = TRUE;
	else
		bRestoreSel = FALSE;

	// Check for scrolling status
	if (!IsWindowVisible(m_hwnd))
		bScroll = TRUE;	// to make things simplier in this rare case
	else if (TEXT_VIEW_AUTOSCROLL_ALWAYS == m_nAutoScroll)
		bScroll	= bAutoScrollTextView(TRUE /*bJustCheckEndInView*/);
	else if (TEXT_VIEW_AUTOSCROLL_NEVER == m_nAutoScroll)
		bScroll = FALSE;
	else if ((!bRestoreSel) ||
				(((m_nAutoScroll & TEXT_VIEW_AUTOSCROLL_NOSELECT) &&
					(crSav.cpMin == crSav.cpMax)) ||
				((m_nAutoScroll & TEXT_VIEW_AUTOSCROLL_NOMIDDLE) &&
					(crSav.cpMin != crSav.cpMax))))
		bScroll	= bAutoScrollTextView(TRUE /*bJustCheckEndInView*/);
	else
		bScroll = FALSE;

	// hoping to find a CR within 4K after the cutting edge
	szHist = new TCHAR[2048];
	if (!szHist)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		return (DWORD) -1;
	}
	szHist[0] = g_chEOS;

	// cut at least m_fBuffCutOff*100 % of the history
	if (dwMinCut < (DWORD) (m_fBuffCutOff * m_dwBuffMaxSize))
		dwMinCut = (DWORD) (m_fBuffCutOff * m_dwBuffMaxSize);

	// Set current selection to (cutting point) -> (cutting point + 4K)

	::SendMessage(m_hwnd, EM_SETSEL, (WPARAM) dwMinCut, (LPARAM) dwMinCut+2046);
	::SendMessage(m_hwnd, EM_GETSELTEXT, (WPARAM) 0, (LPARAM) szHist);

	// find the first LF after cbMinCut characters
	szHistTmp = szHist;

	while (*szHistTmp != g_chEOS)
	{
		if (*szHistTmp == g_chLF)
		{
			bFound = TRUE;
			cbHist = dwMinCut + (szHistTmp - szHist) + 1;
			break;
		}
		else
			szHistTmp = CharNext(szHistTmp);
	}

	if (!bFound)
	{
		// Couldn't find a LF, so we empty the text box
		::SendMessage(m_hwnd, WM_SETTEXT, 0L, 0L);
		cbHist = m_dwBuffSize;
		m_dwBuffSize = 0L;
		return cbHist;
	}
	else
	{
		// select cbHist first bytes in text box
		::SendMessage(m_hwnd, EM_SETSEL, (WPARAM) 0, (LPARAM) cbHist);
		// and erase them
		::SendMessage(m_hwnd, EM_REPLACESEL, (WPARAM) FALSE, (LPARAM) _T(""));
		// readjust m_dwBuffSize
		m_dwBuffSize -= cbHist;
		#ifdef DEBUG
			DWORD cbRecvLen = ::SendMessage(m_hwnd, WM_GETTEXTLENGTH, 0L, 0L);
			ASSERT (cbRecvLen == m_dwBuffSize, "Wrong m_dwBuffSize in CTextView::dwClearTextViewBuffer");
		#endif // DEBUG
	}

	if (bScroll)
	{
		// Select end of history
		::SendMessage(m_hwnd, EM_SETSEL, (WPARAM) m_dwBuffSize, (LPARAM) m_dwBuffSize);
		// Make the caret visible
		::SendMessage(m_hwnd, EM_SCROLLCARET, 0L, 0L);
	}
	else
	{
		// Restore any Text Selections
		if (bRestoreSel)
		{
			crSav.cpMin = (cbHist+1 >= (DWORD) crSav.cpMin) ? 0 : crSav.cpMin - cbHist;
			crSav.cpMax = (cbHist+1 >= (DWORD) crSav.cpMax) ? 0 : crSav.cpMax - cbHist;

			::SendMessage(m_hwnd, EM_EXSETSEL, 0L, (LPARAM) &crSav);
		}
	}

	delete [] szHist;

	return cbHist;
}


DWORD TEXTVIEWCLASSNAME::dwGetSelectedTextSize()
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::dwGetSelectedTextSize");
	DWORD dwStart, dwEnd;

	// get current selection marks
	::SendMessage(m_hwnd, EM_GETSEL, (WPARAM) &dwStart, (LPARAM) &dwEnd);

	return dwEnd-dwStart;
}


DWORD TEXTVIEWCLASSNAME::dwGetSelectedText(LPTSTR szBuffer, DWORD dwBuffLength)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::dwGetSelectedText");
	DWORD dwStart, dwEnd;

	// get current selection marks
	::SendMessage(m_hwnd, EM_GETSEL, (WPARAM) &dwStart, (LPARAM) &dwEnd);
	if ((dwEnd-dwStart) >= dwBuffLength)
		return 0;

	::SendMessage(m_hwnd, EM_GETSELTEXT, (WPARAM) 0, (LPARAM) szBuffer);

	return dwEnd-dwStart;
}


BOOL TEXTVIEWCLASSNAME::bSetInsertBlank(short nInsert)
{
	if (nInsert < TEXT_VIEW_BLANK_MIN || nInsert > TEXT_VIEW_BLANK_MAX)
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	m_nInsertBlank = nInsert;
	return TRUE;
}



BOOL TEXTVIEWCLASSNAME::bSetAutoScroll(short nAutoScroll)
{
	if (nAutoScroll < TEXT_VIEW_AUTOSCROLL_MIN || nAutoScroll > TEXT_VIEW_AUTOSCROLL_MAX)
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	m_nAutoScroll = nAutoScroll;
	return TRUE;
}


BOOL TEXTVIEWCLASSNAME::bSetTextViewDefaultFormat(CHARFORMAT *pCharFormat)
{
	ASSERT(pCharFormat, "pCharFormat is NULL in CTextView::bSetTextViewDefaultFont");

	m_cfFont = *pCharFormat;
	return TRUE;
}



BOOL TEXTVIEWCLASSNAME::bGetTextViewDefaultFormat(CHARFORMAT **ppCharFormat)
{
	ASSERT(ppCharFormat, "ppCharFormat is NULL in CTextView::bGetTextViewDefaultFont");

	if (!ppCharFormat)
		return FALSE;

	*ppCharFormat = &m_cfFont;
	return TRUE;
}


BOOL TEXTVIEWCLASSNAME::bSetDefaultMessageFormat(CHARFORMAT *pCharFormat, MSG_TYPE MsgType, BOOL bHeader)
{
	ASSERT (MsgType >= 0 && MsgType <= mtURL, "MsgType out of range in CTextView::bSetDefaultMessageFormat");
	ASSERT ((!bHeader || (bHeader && (MsgType < mtBeginActions))), "MsgType greater than mtBeginActions in CTextView::bSetDefaultMessageFormat");

	if (mtURL == MsgType)
	{
		m_URLMsgTypeProp.CharFormat = pCharFormat? *pCharFormat : gDefURLFormat;
	}
	else if (MsgType < mtBeginInfo)
	{
		if (bHeader)
			m_HeadMsgTypeProp.CharFormat = pCharFormat? *pCharFormat : gDefHeadFormat;
		else
			m_TextMsgTypeProp.CharFormat = pCharFormat? *pCharFormat : gDefTextFormat;
	}
	else
	{
		m_InfoMsgTypeProp.CharFormat = pCharFormat? *pCharFormat : gDefInfoFormat;
	}

	return TRUE;
}


BOOL TEXTVIEWCLASSNAME::bSetMessageFormat(CHARFORMAT *pCharFormat, MSG_TYPE MsgType, MEMBER_STATUS MembFrom, BOOL bHeader)
{
	ASSERT (MsgType >= 0 && MsgType <= mtURL, "MsgType out of range in CTextView::bSetMessageFormat");
	ASSERT (MembFrom < msEndEnum, "MembFrom greater than msEndEnum in CTextView::bSetMessageFormat");
	ASSERT ((!bHeader || (bHeader && (MsgType < mtBeginActions))), "MsgType greater than mtBeginActions in CTextView::bSetMessageFormat");

	DumpCharFormat(pCharFormat, "In bSetMessageFormat:\n");

	// REGISB: added 12/09/97
	if (pCharFormat && 
		g_chEOS != pCharFormat->szFaceName[0] && (pCharFormat->dwMask & CFM_FACE) && 
		ANSI_CHARSET != pCharFormat->bCharSet && (pCharFormat->dwMask & CFM_CHARSET))
	{
		LOGFONT	lf;

		ZeroMemory(&lf, sizeof(LOGFONT));

		strcpy(lf.lfFaceName, pCharFormat->szFaceName);
		lf.lfCharSet = pCharFormat->bCharSet;

		MatchFont(lf);
		strcpy(pCharFormat->szFaceName, lf.lfFaceName);
	}

	if (mtURL == MsgType)
	{
		m_URLMsgTypeProp.CharFormat = pCharFormat? *pCharFormat : gDefURLFormat;
		return TRUE;
	}

	if (MsgType < mtBeginInfo)
	{
		//TODO: minimize the code in this if-else block
		if (bHeader)
		{
			if (!pCharFormat)
			{
				if (m_pMsgTypePropHead[MembFrom][MsgType])
				{
					delete m_pMsgTypePropHead[MembFrom][MsgType];
					m_pMsgTypePropHead[MembFrom][MsgType] = NULL;
				}
				return TRUE;
			}

			if (!m_pMsgTypePropHead[MembFrom][MsgType])
			{
				if (NULL == (m_pMsgTypePropHead[MembFrom][MsgType] = new MSG_TYPE_PROP))
					return FALSE;
			}
			
			m_pMsgTypePropHead[MembFrom][MsgType]->CharFormat = *pCharFormat;
		}
		else
		{
			if (!pCharFormat)
			{
				if (m_pMsgTypePropText[MembFrom][MsgType])
				{
					delete m_pMsgTypePropText[MembFrom][MsgType];
					m_pMsgTypePropText[MembFrom][MsgType] = NULL;
				}
				return TRUE;
			}

			if (!m_pMsgTypePropText[MembFrom][MsgType])
			{
				if (NULL == (m_pMsgTypePropText[MembFrom][MsgType] = new MSG_TYPE_PROP))
					return FALSE;
			}
			
			m_pMsgTypePropText[MembFrom][MsgType]->CharFormat = *pCharFormat;
		}
	}
	else
	{
		ASSERT (MsgType >= mtBeginInfo && MsgType < mtURL, "MsgType out of range in CTextView::bSetMessageFormat");
		if (!pCharFormat)
		{
			if (m_pMsgTypePropInfo[MsgType-mtBeginInfo])
			{
				delete m_pMsgTypePropInfo[MsgType-mtBeginInfo];
				m_pMsgTypePropInfo[MsgType-mtBeginInfo] = NULL;
			}
			return TRUE;
		}

		if (!m_pMsgTypePropInfo[MsgType-mtBeginInfo])
		{
			if (NULL == (m_pMsgTypePropInfo[MsgType-mtBeginInfo] = new MSG_TYPE_PROP))
				return FALSE;
		}
		
		m_pMsgTypePropInfo[MsgType-mtBeginInfo]->CharFormat = *pCharFormat;
	}

	// Verify Integrity?
	return TRUE;
}


BOOL TEXTVIEWCLASSNAME::bGetDefaultMessageFormat(CHARFORMAT **ppCharFormat, MSG_TYPE MsgType, BOOL bHeader)
{
	ASSERT (MsgType >= 0 && MsgType <= mtURL, "MsgType out of range in CTextView::bGetDefaultMessageFormat");
	ASSERT ((!bHeader || (bHeader && (MsgType < mtBeginActions))), "MsgType greater than mtBeginActions in CTextView::bGetDefaultMessageFormat");

	if (!ppCharFormat)
		return FALSE;

	if (mtURL == MsgType)
	{
		*ppCharFormat = &m_URLMsgTypeProp.CharFormat;
	}
	else if (MsgType < mtBeginInfo)
	{
		if (bHeader)
			*ppCharFormat = &m_HeadMsgTypeProp.CharFormat;
		else
			*ppCharFormat = &m_TextMsgTypeProp.CharFormat;
	}
	else
	{
		*ppCharFormat = &m_InfoMsgTypeProp.CharFormat;
	}
	return TRUE;
}


BOOL TEXTVIEWCLASSNAME::bGetMessageFormat(CHARFORMAT **ppCharFormat, MSG_TYPE MsgType, MEMBER_STATUS MembFrom, BOOL bHeader)
{
	ASSERT (MsgType >= 0 && MsgType <= mtURL, "MsgType out of range in CTextView::bGetMessageFormat");
	ASSERT (MembFrom < msEndEnum, "MembFrom greater than mfEndEnum in CTextView::bGetMessageFormat");
	ASSERT ((!bHeader || (bHeader && (MsgType < mtBeginActions))), "MsgType greater than mtBeginActions in CTextView::bGetMessageFormat");

	if (!ppCharFormat)
		return FALSE;

	if (mtURL == MsgType)
	{
		*ppCharFormat = &m_URLMsgTypeProp.CharFormat;
	}
	else if (MsgType < mtBeginInfo)
	{
		if (bHeader)
			*ppCharFormat = m_pMsgTypePropHead[MembFrom][MsgType] ?
							&m_pMsgTypePropHead[MembFrom][MsgType]->CharFormat :
							NULL;
		else
			*ppCharFormat = m_pMsgTypePropText[MembFrom][MsgType] ?
							&m_pMsgTypePropText[MembFrom][MsgType]->CharFormat :
							NULL;
	}
	else
	{
		*ppCharFormat = m_pMsgTypePropInfo[MsgType-mtBeginInfo] ?
						&m_pMsgTypePropInfo[MsgType-mtBeginInfo]->CharFormat :
						NULL;
	}
	// Verify Integrity?
	return NULL != *ppCharFormat;
}


// Displays text "<Alias> has [joined|left] the conversation."
// TODO: Use MembAs to display more information.
INT TEXTVIEWCLASSNAME::iDisplayMemberStatus(LPCTSTR szNickname, DWORD dwNameLen,
									MSG_TYPE MsgType, MEMBER_STATUS MembAs,
									CHARFORMAT *pCharFormat)
{
	TCHAR	*szMess, *szStatus;
	LPTSTR	szText = NULL;
	DWORD	dwTextLen = 0L;
	DWORD	dwStatusLen = 0L;
	INT		iRetVal = 0;

	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::iDisplayMemberStatus.");
	ASSERT(szNickname, "szNickName is NULL in CTextView::iDisplayMemberStatus.");
	ASSERT(0 <= MembAs && MembAs < msEndEnum, "MembAs out of range in CTextView::iDisplayMemberStatus.");
	ASSERT(MsgType == mtJoin || MsgType == mtLeave,"Unsupported MSG_TYPE type in CTextView::iDisplayMemberStatus");

	dwTextLen = m_nMsgTypeLen[MsgType];
	szMess	  = m_szMsgType[MsgType];
	dwStatusLen = m_nMembStatusLen[MembAs];
	szStatus	= m_szMembStatus[MembAs];

	dwTextLen += dwStatusLen + (dwNameLen ? dwNameLen : _tcslen(szNickname)) - 4; 	// -4 comes from the %1%2 which are replaced
	
	if (!bCanAdd2Buffer(dwTextLen))
		return -1;

	szText = new TCHAR[dwTextLen+1];
	if (!szText)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		return -1;
	}

	LPCTSTR pszArgs[] = { szStatus, szNickname };

	if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
					   szMess, 0, 0, szText, dwTextLen+1, (va_list*) pszArgs))
	{
		delete [] szText;
		return -1;
	}

	iRetVal = iDisplayMsgText(szText, dwTextLen, MsgType, MembAs,
								/*bShowURLs*/ FALSE, /*bInformFull*/ TRUE, 
								/*bAppend*/ FALSE, /*lIndent*/ 0,
								pCharFormat);

	delete [] szText;

	return iRetVal;
}




// displays the name of the sender and recipients in case of a whisper
// in the Text View Buffer
INT TEXTVIEWCLASSNAME::iDisplayMsgHeader(DWORD dwMessToFollow, 
								LPCTSTR szFrom, DWORD dwFromLen,
								LPCTSTR szTo, DWORD dwToLen,
								MSG_TYPE MsgType, MEMBER_STATUS MembFrom,						 
								CHARFORMAT *pCharFormat)

{
	TCHAR	*szMess, *szStatus;
	LPTSTR	szText		= NULL;
	DWORD	dwTextLen	= 0L;
	DWORD	dwStatusLen = 0L;
	DWORD	dwMsgLen	= 0L;
	INT		iRetVal		= 0;

	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::iDisplayMsgHeader");
	ASSERT(szFrom, "szFrom is NULL in CTextView::iDisplayMsgHeader");
	ASSERT(0 <= MembFrom && MembFrom < msEndEnum, "MembFrom out of range in CTextView::iDisplayMsgHeader.");
	ASSERT(MsgType >= 0 && MsgType < mtBeginActions, "Unexpected MessageType value in CTextView::iDisplayMsgHeader");

	dwTextLen = m_nMsgTypeLen[MsgType];
	szMess	  = m_szMsgType[MsgType];
	dwStatusLen = m_nMembStatusLen[MembFrom];
	szStatus	= m_szMembStatus[MembFrom];

	if (MsgType == mtWhisper)
	{
		ASSERT(szTo, "szTo is NULL in CTextView::iDisplayMsgHeader");
		dwTextLen += dwStatusLen + (dwFromLen ? dwFromLen : _tcslen(szFrom)) + 
						(dwToLen ? dwToLen : _tcslen(szTo)) - 6;	// -6 for %1, %2 and %3 in IDS
	}
	else
		dwTextLen += dwStatusLen + (dwFromLen ? dwFromLen : _tcslen(szFrom)) - 4;	// -4 for %1 and %2 in IDS

	// adding CR after last message + CR after sender's name + CR for empty line (not a big deal if we actually don't)
	dwMsgLen = dwTextLen + dwMessToFollow + (m_dwBuffSize?1:0) + 1 + ((TEXT_VIEW_BLANK_NEVER != m_nInsertBlank)?1:0);
	
	if (!bCanAdd2Buffer(dwMsgLen))
		return -1;

	szText = new TCHAR[dwTextLen+1];
	if (!szText)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		return -1;
	}

	if (mtWhisper == MsgType)
	{
		LPCTSTR pszArgs[] = { szStatus, szFrom, szTo };
		if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
							szMess, 0, 0, szText, dwTextLen+1, (va_list*) pszArgs))
		{
			delete [] szText;
			return -1;
		}
	}
	else
	{
		LPCTSTR pszArgs[] = { szStatus, szFrom };
		if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
							szMess, 0, 0, szText, dwTextLen+1, (va_list*) pszArgs))
		{
			delete [] szText;
			return -1;
		}
	}

	// We need Header Character formatting
	if (NULL == pCharFormat)
	{
		if (m_pMsgTypePropHead[MembFrom][MsgType])
			pCharFormat = &m_pMsgTypePropHead[MembFrom][MsgType]->CharFormat;
		else
			pCharFormat = &m_HeadMsgTypeProp.CharFormat;
	}

	
	m_bCallHeader = TRUE; // WindowUpdates can be frozen until text is Displayed

	iRetVal = iDisplayMsgText(szText, dwTextLen, MsgType, MembFrom,
								/*bShowURLs*/ FALSE, /*bInformFull*/ TRUE, 
								/*bAppend*/ FALSE, /*lIndent*/ 0,
								pCharFormat);
	delete [] szText;

	m_bCallHeader = FALSE;
	m_bHeader = TRUE;	// So that next call to DisplayMsgText
						// will not insert an additional blank line.
	return iRetVal;
}


// puts text in the text view
INT TEXTVIEWCLASSNAME::iDisplayMsgText(LPCTSTR szText, DWORD dwTextLen, 
								MSG_TYPE MsgType, MEMBER_STATUS MembFrom,
								BOOL bShowURLs, BOOL bInformFull, 
								BOOL bAppend, LONG lIndent,
								CHARFORMAT *pCharFormat,
								DWORD *prgdwFormatting,
								INT cFormats)
{
	BOOL		bScroll, bRestoreSel;
	BOOL		bShowWnd = FALSE;
	BOOL		bVisible;
	CHARRANGE	crSav;
	INT			iRetVal = 0;
	LRESULT		lr;
	DWORD		dwTxtLn = dwTextLen ? dwTextLen : _tcslen(szText), dwLenTmp;
	LPTSTR		szTxt = (LPTSTR) szText;

	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::iDisplayMsgText");

	// REGISB, 10/09/97: Bug 1976 workaround
	if (m_bDBCSSystem && IsDBCSLeadByteEx(GetACP(), (BYTE) szText[dwTxtLn-1]))	// Last byte could be a lead-byte in DBCS system
	{
		LPTSTR	szPrev = CharPrev(szText, szText+dwTxtLn-1);
		LPTSTR	szLast = szPrev;
		while (*szLast)
		{
			szPrev = szLast;
			szLast = CharNext(szLast);
		}
		if (szPrev == szText+dwTxtLn-1)
		{
			// Last byte is a lead-byte
			szTxt = new TCHAR[dwTxtLn+1];
			if (szTxt)
			{
				_tcscpy(szTxt, szText);
				szTxt[--dwTxtLn] = g_chEOS;
			}
			else
				szTxt = (LPTSTR) szText;
		}
	}


	// To freeze and update text and scroll bar at the same time
//	if (!m_bHeader)
//		::LockWindowUpdate(m_hwnd);

	// Check for current Selection
	::SendMessage(m_hwnd, EM_EXGETSEL, 0L, (LPARAM) &crSav);
	if ((crSav.cpMin != crSav.cpMax) || (crSav.cpMin != (long) m_dwBuffSize))
		bRestoreSel = TRUE;
	else
		bRestoreSel = FALSE;

	// See if we need to check for Scrolling, optimized for general case performance.
	// Note: bAutoScrollTextView() is used only to see if the end is in view

	if (TEXT_VIEW_AUTOSCROLL_ALWAYS == m_nAutoScroll)
		bScroll	= bAutoScrollTextView(TRUE /*bJustCheckEndInView*/);
	else if (TEXT_VIEW_AUTOSCROLL_NEVER == m_nAutoScroll)
		bScroll = FALSE;
	else if ( (!bRestoreSel) ||
				( ((m_nAutoScroll & TEXT_VIEW_AUTOSCROLL_NOSELECT) &&
					(crSav.cpMin == crSav.cpMax)) ||
				((m_nAutoScroll & TEXT_VIEW_AUTOSCROLL_NOMIDDLE) &&
					(crSav.cpMin != crSav.cpMax)) ) )
		bScroll	= bAutoScrollTextView(TRUE /*bJustCheckEndInView*/);
	else
		bScroll = FALSE;

	if (!bScroll)	// Disable AutoVScroll
	{
		// OutputDebugThreadIdString("Disabling AutoVScroll\r\n");
		bVisible = IsWindowVisible(m_hwnd);
		bShowWnd = TRUE;

		::SendMessage(m_hwnd, EM_SETOPTIONS, ECOOP_AND, ~ECO_AUTOVSCROLL);
		::ShowWindow(m_hwnd, bVisible ? SW_SHOW : SW_HIDE);
	}

	// Move the selection to the end so that text can be appended
	CHARRANGE	cr;
	cr.cpMin = cr.cpMax = m_dwBuffSize;
	::SendMessage(m_hwnd, EM_EXSETSEL, 0L, (LPARAM) &cr);

	// No Indents or CRs if appending
	if (!bAppend)
	{
		if (m_dwBuffSize)	// add a line feed if the history is not empty
		{
			if (m_bHeaderSeparate || !m_bHeader)
			{
				::SendMessage(m_hwnd, EM_REPLACESEL, 0L, (LPARAM) (LPCTSTR) g_szLF);
				m_dwBuffSize++;
			}
			else
			{
				CHARFORMAT cf;
				cf.cbSize = sizeof(CHARFORMAT);
				::SendMessage(m_hwnd, EM_GETCHARFORMAT, (WPARAM) TRUE, (LPARAM) &cf);
				cf.dwMask = CFM_UNDERLINE;
				cf.dwEffects = 0L;
				::SendMessage(m_hwnd, EM_SETCHARFORMAT, (WPARAM) SCF_SELECTION, (LPARAM) &cf);
				::SendMessage(m_hwnd, EM_REPLACESEL, 0L, (LPARAM) (LPCTSTR) g_szHeaderTab);
				m_dwBuffSize += g_nHeaderTabLen;
			}

			// If previous call was by DisplayHeader DoNot add Blank
			if (((m_nInsertBlank & TEXT_VIEW_BLANK_ALWAYS) &&
					!(m_bHeader)) || 
				 ((MsgType != m_mtLastMsgType) && 
					(m_nInsertBlank & TEXT_VIEW_BLANK_DIFFTYPES)))
			{
				// add another carriage return if we want to skip a line
				::SendMessage(m_hwnd, EM_REPLACESEL, 0L, (LPARAM) (LPCTSTR) g_szLF);
				m_dwBuffSize++;
			}
			// Reset Previous Header Call Info.
			//if (m_bHeader)
			//	m_bHeader = FALSE;
		}

		if (m_bHeaderSeparate || !m_bHeader)
			bSetIndent(lIndent);	// Continue printing, but return an error

		// Reset Previous Header Call Info.
		if (m_bHeader)
			m_bHeader = FALSE;
	}

	if (NULL == pCharFormat)
	{
		if (m_pMsgTypePropText[MembFrom][MsgType])
			pCharFormat = &m_pMsgTypePropText[MembFrom][MsgType]->CharFormat;
		else
			pCharFormat = &m_TextMsgTypeProp.CharFormat;
	}

	ASSERT(pCharFormat, "pCharFormat is NULL in CTextView::iDisplayMsgText");

	m_mtLastMsgType = MsgType;

	// Combine the CharFormat & default font
	lr = ::SendMessage(m_hwnd, EM_SETCHARFORMAT, (WPARAM) SCF_SELECTION, (LPARAM) &m_cfFont);
	ASSERT(lr, "1st EM_SETCHARFORMAT failed in CTextView::iDisplayMsgText");

	if (NULL == prgdwFormatting)
	{
		// Currently ignoring the return value even if it can't be set
		lr = ::SendMessage(m_hwnd, EM_SETCHARFORMAT, (WPARAM) SCF_SELECTION, (LPARAM) pCharFormat);		
		ASSERT(lr, "2nd EM_SETCHARFORMAT failed in CTextView::iDisplayMsgText");

		::SendMessage(m_hwnd, EM_REPLACESEL, 0L, (LPARAM) szTxt);
	}
	else
	{
		ASSERT(cFormats > 0, "cFormats <= 0 in CTextView::iDisplayMsgText");
		INT			iFormat;
		WORD		wCurOffset, wNextOffset, wNextFormat = 0;
		CHARFORMAT	cfChunk;
		LPTSTR		szChunk;
		TCHAR		szFaceName[LF_FACESIZE];
		BOOL		bTransparency = FALSE;

		if (!(szChunk = new TCHAR[dwTxtLn+1]))
		{
			if (szTxt != szText)
				delete [] szTxt;
			return -1;
		}

		if ((pCharFormat->dwMask & CFM_COLOR) && (pCharFormat->dwEffects & CFE_AUTOCOLOR))
		{
			pCharFormat->dwEffects &= ~CFE_AUTOCOLOR;
			pCharFormat->crTextColor = GetSysColor(COLOR_WINDOWTEXT);
		}

		CopyMemory((PVOID) &cfChunk, (CONST VOID*) pCharFormat, sizeof(CHARFORMAT));
		cfChunk.dwMask = CFM_BOLD | CFM_ITALIC | CFM_UNDERLINE | CFM_COLOR | CFM_FACE;
		if (pCharFormat->dwMask & CFM_SIZE)
			cfChunk.dwMask |= CFM_SIZE;
		cfChunk.dwEffects = 0L;
		if (pCharFormat->dwMask & CFM_FACE)
			_tcscpy(szFaceName, pCharFormat->szFaceName);
		else
		{
			_tcscpy(cfChunk.szFaceName, m_cfFont.szFaceName);
			_tcscpy(szFaceName, m_cfFont.szFaceName);
		}
		ASSERT(szFaceName[0] != g_chEOS, "szFaceName is empty in CTextView::iDisplayMsgText");

		static short nFixedPitchIndex = nGetSpecialFontIndex(TRUE);
		static short nSymbolIndex = nGetSpecialFontIndex(FALSE);

		COLORREF	crDefTextColor = cfChunk.crTextColor;
		
		for (iFormat = 0, wCurOffset = 0; iFormat < cFormats; iFormat++, wCurOffset = wNextOffset)
		{
			wNextOffset = HIWORD(prgdwFormatting[iFormat]);
			if (wNextOffset-wCurOffset)
			{
				if (bTransparency)
				{
					FillMemory(szChunk, wNextOffset-wCurOffset, g_chTransparent);
					szChunk[wNextOffset-wCurOffset] = g_chEOS;
				}
				else
				{
					_tcsncpy(szChunk, szTxt+wCurOffset, wNextOffset-wCurOffset);
					szChunk[wNextOffset-wCurOffset] = g_chEOS;
				
					// Bug 3188 workaround - strange bug!
					if (szChunk[wNextOffset-wCurOffset-1] == g_chCR)
					{
						dwTxtLn--;
						szChunk[wNextOffset-wCurOffset-1] = g_chEOS;
						ASSERT(szTxt[wNextOffset-1] != g_chEOS, "szTxt[wNextOffset-1] == g_chEOS in CTextView::iDisplayMsgText");
						ASSERT(szTxt[wNextOffset] == g_chLF, "szTxt[wNextOffset] != g_chLF in CTextView::iDisplayMsgText");
					}
				}

				cfChunk.dwEffects |= pCharFormat->dwEffects;
				lr = ::SendMessage(m_hwnd, EM_SETCHARFORMAT, (WPARAM) SCF_SELECTION, (LPARAM) &cfChunk);
				ASSERT(lr, "EM_SETCHARFORMAT failed in CTextView::iDisplayMsgText");
				::SendMessage(m_hwnd, EM_REPLACESEL, 0L, (LPARAM) szChunk);
			}
			
			wNextFormat = LOWORD(prgdwFormatting[iFormat]);
			
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

			if (nFixedPitchIndex >= 0 && (wNextFormat & wFixedPitch))
			{
				_tcscpy(cfChunk.szFaceName, FIXEDPITCHFACENAMES[nFixedPitchIndex]);
				cfChunk.bPitchAndFamily &= ~VARIABLE_PITCH;
				cfChunk.bPitchAndFamily |= FIXED_PITCH;
			}

			if (nSymbolIndex >= 0 && (wNextFormat & wSymbol))
			{
				_tcscpy(cfChunk.szFaceName, SYMBOLFACENAMES[nSymbolIndex]);
				cfChunk.bPitchAndFamily &= ~VARIABLE_PITCH;
				cfChunk.bPitchAndFamily &= ~FIXED_PITCH;
				cfChunk.bCharSet = SYMBOL_CHARSET;
				cfChunk.dwMask |= CFM_CHARSET;
			}
			else
				cfChunk.bCharSet = m_cfFont.bCharSet;

			if (
				(nFixedPitchIndex < 0 && wNextFormat & wFixedPitch) ||
				(nSymbolIndex < 0 && wNextFormat & wSymbol) ||
				(!(wNextFormat & wFixedPitch) && !(wNextFormat & wSymbol))
			   )
			{
				_tcscpy(cfChunk.szFaceName, szFaceName);
				cfChunk.bPitchAndFamily &= ~VARIABLE_PITCH;
				cfChunk.bPitchAndFamily &= ~FIXED_PITCH;
			}

			bTransparency = FALSE;
			if (wNextFormat & wForeground)
			{
				COLORREF crForeground = GetRBGColor((wNextFormat >> 4) & 0x000F);
				if (crForeground != GetSysColor(COLOR_WINDOW))
					cfChunk.crTextColor = crForeground;
				else
					cfChunk.crTextColor = crDefTextColor;

				if ((wNextFormat & wBackground) &&
					((wNextFormat >> 4) & 0x000F) == (wNextFormat & 0x000F))
				{
					// Sender wants transparency
					cfChunk.crTextColor = crForeground;
					bTransparency = TRUE;
				}

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

		cfChunk.dwEffects |= pCharFormat->dwEffects;
		lr = ::SendMessage(m_hwnd, EM_SETCHARFORMAT, (WPARAM) SCF_SELECTION, (LPARAM) &cfChunk);
		ASSERT(lr, "EM_SETCHARFORMAT failed in CTextView::iDisplayMsgText");
		if (bTransparency)
		{
			dwLenTmp = _tcslen(szTxt+wNextOffset);
			FillMemory(szChunk, dwLenTmp, g_chTransparent);
			szChunk[dwLenTmp] = g_chEOS;
		}
		else
		{
			_tcscpy(szChunk, szTxt+wNextOffset);
			if (szChunk[_tcslen(szChunk)-1] == g_chCR)
			{
				// Bug 3188 workaround - strange bug!
				szChunk[_tcslen(szChunk)-1] = g_chEOS;
				dwTxtLn--;
			}
		}

		::SendMessage(m_hwnd, EM_REPLACESEL, 0L, (LPARAM) szChunk);

		delete [] szChunk;
	}

	if (bShowURLs)		// recognize the URLs if asked to
	{
		INT nUrlBounds[MAX_URL_INTEXT*2];		
		INT	nUrlNum = MAX_URL_INTEXT;		// max urls in single text line

		m_urlrec.HrIdentifyUrls(szTxt, nUrlBounds, &nUrlNum);
		// continue execution even if HrIdentifyUrls fails

		// we only show the first 16 URLs
		if (nUrlNum > 0)
			RegisterTextLinks(m_dwBuffSize, nUrlBounds, nUrlNum);		// show the potential URLs if asked to
	}

	m_dwBuffSize += dwTxtLn;

#ifdef DEBUG
	DWORD cbRecvLen = ::SendMessage(m_hwnd, WM_GETTEXTLENGTH, 0L, 0L);
	ASSERT (cbRecvLen == m_dwBuffSize, "Wrong m_dwBuffSize in CTextView::iDisplayMsgText");
	//if (cbRecvLen != m_dwBuffSize)
	//{
	//	char szDbg[800];
	//	sprintf(szDbg, "!! Assertion: '%s' '%d' '%d' '%d'\n", szText, dwTxtLn, prgdwFormatting, cFormats);
	//	OutputDebugString(szDbg);
	//}
#endif // DEBUG

	if (bInformFull)
	{
		if (m_dwBuffSize >= m_dwBuffMaxSize * m_fBuffFull)
			iRetVal = 1;
	}

	// Restore any Text Selections
	if (bRestoreSel)
	{
		if (bScroll)	// do not scroll back to the selection
		{
			bVisible = IsWindowVisible(m_hwnd);		
			bShowWnd = TRUE;

			::SendMessage(m_hwnd, EM_SETOPTIONS, ECOOP_AND, ~ECO_AUTOVSCROLL);
			::ShowWindow(m_hwnd,bVisible?SW_SHOW:SW_HIDE);
		}
		::SendMessage(m_hwnd, EM_EXSETSEL, 0L, (LPARAM) &crSav);
	}

	// Leave the window with AutoScroll mode
//	OutputDebugThreadIdString("Enabling AutoVScroll\r\n");
	if (bShowWnd)
	{
		::SendMessage(m_hwnd, EM_SETOPTIONS, ECOOP_OR, ECO_AUTOVSCROLL);
		::ShowWindow(m_hwnd,bVisible?SW_SHOW:SW_HIDE);
	}

	// To freeze and update text and scroll bar at the same time
//	if (!m_bCallHeader)
//		::LockWindowUpdate(NULL);

	if (szTxt != szText)
		delete [] szTxt;

	return iRetVal;
}


// displays the name of the sender and and their action
INT TEXTVIEWCLASSNAME::iDisplayAction(LPCTSTR szFrom, DWORD dwFromLen,
								LPCTSTR szAction, DWORD dwActionLen, 
								MEMBER_STATUS MembFrom,
								BOOL bShowURLs,
								CHARFORMAT *pCharFormat,
								DWORD *prgdwFormatting,
								INT cFormats)
{
	TCHAR	*szMess, *szStatus;
	LPTSTR	szText		= NULL;
	LPTSTR	szFullAction= NULL;
	DWORD	dwFrmLen	= 0L;
	DWORD	dwTextLen	= 0L;
	DWORD	dwStatusLen = 0L;
	DWORD	dwMsgLen	= 0L;
	INT		iRetVal		= 0;
	DWORD	*prgdwLocalFormatting = NULL;

	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::iDisplayAction");
	ASSERT(0 <= MembFrom && MembFrom < msEndEnum, "MembFrom out of range in CTextView::iDisplayAction.");

	dwTextLen = m_nMsgTypeLen[mtAction];
	szMess	  = m_szMsgType[mtAction];
	dwStatusLen = m_nMembStatusLen[MembFrom];
	szStatus	= m_szMembStatus[MembFrom];

	if (szFrom)
	{
		dwFrmLen = dwFromLen ? dwFromLen : _tcslen(szFrom);
		dwTextLen += dwStatusLen + dwFrmLen +
					(dwActionLen ? dwActionLen : _tcslen(szAction)) - 3;	// +1 for space after szFrom and -4 for %1%2
	}
	else
		dwTextLen += dwStatusLen + (dwActionLen ? dwActionLen : _tcslen(szAction)) - 4;	// -4 for %1%2

	// adding CR after last message + CR for empty line
	dwMsgLen = dwTextLen + (m_dwBuffSize?1:0) + ((TEXT_VIEW_BLANK_NEVER != m_nInsertBlank)?1:0);

	if (!bCanAdd2Buffer(dwMsgLen))
		return -1;

	szText = new TCHAR[dwTextLen+1];
	if (!szText)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		return -1;
	}

	szFullAction = new TCHAR[dwTextLen+1];
	if (prgdwFormatting && cFormats)
		prgdwLocalFormatting = new DWORD[cFormats];
	if (!szFullAction || (!prgdwLocalFormatting && prgdwFormatting))
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		return -1;
	}

	if (prgdwLocalFormatting)
	{
		ASSERT(cFormats > 0, "cFormats <= 0 in CTextView::iDisplayAction");
		CopyMemory(prgdwLocalFormatting, prgdwFormatting, cFormats*sizeof(DWORD));
	}

	if (szFrom)
	{
		_tcscpy(szFullAction, szFrom);
		_tcscat(szFullAction, g_szSpace);
		_tcscat(szFullAction, szAction);
		PushFormattingOffsetsDW(prgdwLocalFormatting, cFormats, (short)(dwFrmLen + _tcslen(szStatus) + 1));
	}
	else
	{
		_tcscpy(szFullAction, szAction);
		PushFormattingOffsetsDW(prgdwLocalFormatting, cFormats, _tcslen(szStatus));
	}

	LPCTSTR pszArgs[] = { szStatus, szFullAction };
	if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
						szMess, 0, 0, szText, dwTextLen+1, (va_list*) pszArgs))
	{
		iRetVal = -1;
		goto exit;
	}

	iRetVal = iDisplayMsgText(szText, 
							  dwTextLen, 
							  mtAction, 
							  MembFrom,
							  bShowURLs /*bShowURLs*/,
							  TRUE /*bInformFull*/, 
							  FALSE /*bAppend*/,
							  0 /*lIndent*/,
							  pCharFormat,
							  prgdwLocalFormatting,
							  cFormats);

exit:
	delete [] szText;
	delete [] szFullAction;

	if (prgdwLocalFormatting) 
		delete [] prgdwLocalFormatting;

	return iRetVal;
}


INT	TEXTVIEWCLASSNAME::iDisplayInfo(LPCTSTR szFrom, DWORD dwFromLen,
							LPCTSTR szTo, DWORD dwToLen,
							LPCTSTR szInfo, DWORD dwInfoLen, 
							MSG_TYPE MsgType,
							MEMBER_STATUS MembChange,
							CHARFORMAT *pCharFormat,
							DWORD *prgdwFormatting,
							INT cFormats)
{
	TCHAR		szStatus[g_nMaxLengthSmall];
	TCHAR		*szMess;
	TCHAR		szNoReason[g_nMaxLengthSmall];
	LPTSTR		szText = NULL;
	DWORD		dwMsgLen = 0L;
	DWORD		dwStatusLen = 0L;
	DWORD		dwNoReasonLen = 0L;
	DWORD		dwTextLen = 0L;
	INT			iRetVal = 0;
	LPCTSTR*	pszArgs = NULL;
	DWORD*		prgdwLocalFormatting = prgdwFormatting;
	BOOL		bFreeLocalFormatting = FALSE;

	ASSERT(m_hwnd, "m_hwnd is NULL in CTextView::iDisplayInfo");
	ASSERT(0 <= MembChange && MembChange < msEndEnum, "MembChange out of range in CTextView::iDisplayInfo.");
	ASSERT(mtBeginInfo <= MsgType && MsgType < mtEndEnum, "MsgType out of range in CTextView::iDisplayInfo.");

	dwTextLen = m_nMsgTypeLen[MsgType];
	szMess	  = m_szMsgType[MsgType];

	// cases are:	
	// mtGetInfo:      "<szFrom><szTo><szInfo>."
	// mtStatusChange: "<szTo> has been made a <szStatus>."
	// mtAliasChange:  "<szFrom> is now known as <szTo>."
	// mtTopicChange:  "The topic of the chat is now '<szInfo>'."
	// mtGetRealname   "The real name of <szTo> is <szInfo>."
	// mtKicked:       "The host <szFrom> kicked <szTo> (<szInfo>|<szNoReason>)."
	
	switch (MsgType)
	{
		case mtGetInfo:
			pszArgs = new LPCTSTR[3];
			if (szFrom)
				pszArgs[0] = szFrom;
			else
				pszArgs[0] = g_szEmpty;
			if (szTo)
				pszArgs[1] = szTo;
			else
				pszArgs[1] = g_szEmpty;
			if (szInfo)
				pszArgs[2] = szInfo;
			else
				pszArgs[2] = g_szEmpty;
			dwTextLen += (dwFromLen ? dwFromLen : _tcslen(pszArgs[0])) + (dwToLen ? dwToLen : _tcslen(pszArgs[1])) + (dwInfoLen ? dwInfoLen : _tcslen(pszArgs[2])) - 6;	// -6 for %1, %2 and %3 in IDS
			
			if (prgdwFormatting && cFormats)
			{
				if (!(prgdwLocalFormatting = new DWORD[cFormats]))
				{
					::SetLastError(ERROR_OUTOFMEMORY);
					return -1;
				}

				bFreeLocalFormatting = TRUE;

				CopyMemory(prgdwLocalFormatting, prgdwFormatting, cFormats*sizeof(DWORD));

				PushFormattingOffsetsDW(prgdwLocalFormatting, cFormats, _tcslen(pszArgs[0]) + _tcslen(pszArgs[1]));
			}
			break;

		case mtStatusChange:
			::SetLastError(NOERROR);
			if (!(dwStatusLen = ::LoadString(m_hInstance, IDS_HOSTSTATUS_INFO+MembChange, (LPTSTR) szStatus, g_nMaxLengthSmall-1)) && NOERROR != GetLastError())
				return -1;
			dwTextLen += dwStatusLen + (dwToLen ? dwToLen : _tcslen(szTo)) - 4;	// -4 for %1 and %2 in IDS
			pszArgs = new LPCTSTR[2];
			pszArgs[0] = szTo;
			pszArgs[1] = szStatus;
			break;

		case mtAliasChange:
			dwTextLen += (dwFromLen ? dwFromLen : _tcslen(szFrom)) + (dwToLen ? dwToLen : _tcslen(szTo)) - 4;	// -4 for %1 and %2 in IDS
			pszArgs = new LPCTSTR[2];
			pszArgs[0] = szFrom;
			pszArgs[1] = szTo;
			break;

		case mtTopicChange:
			dwTextLen += (dwInfoLen ? dwInfoLen : _tcslen(szInfo)) - 2;	// -2 for %1 in IDS
			pszArgs = new LPCTSTR[1];
			pszArgs[0] = szInfo;
			break;

		case mtGetRealname:
			dwTextLen += (dwInfoLen ? dwInfoLen : _tcslen(szInfo)) + (dwToLen ? dwToLen : _tcslen(szTo)) - 4;	// -4 for %1 and %2 in IDS
			pszArgs = new LPCTSTR[2];
			pszArgs[0] = szTo;
			pszArgs[1] = szInfo;
			break;

		case mtKicked:			
			dwTextLen += (dwFromLen ? dwFromLen : _tcslen(szFrom)) + (dwToLen ? dwToLen : _tcslen(szTo)) - 6;	// -6 for %1, %2 and %3 in IDS
			pszArgs = new LPCTSTR[3];
			pszArgs[0] = szFrom;
			pszArgs[1] = szTo;
			if (szInfo)
			{
				dwTextLen += (dwInfoLen ? dwInfoLen : _tcslen(szInfo));
				pszArgs[2] = szInfo;
			}
			else
			{
				if (!(dwNoReasonLen = ::LoadString(m_hInstance, IDS_NOKICKREASON_INFO, (LPTSTR) szNoReason, g_nMaxLengthSmall-1)))
					return -1;
				dwTextLen += dwNoReasonLen;
				pszArgs[2] = szNoReason;
			}
			break;

		default:
			ASSERT(FALSE, "Unexpected MsgType value in CTextView::iDisplayInfo");
			::SetLastError(ERROR_INVALID_PARAMETER);
			return -1;
	}
	
	// adding CR after last message + CR for empty line
	dwMsgLen = dwTextLen + (m_dwBuffSize?1:0) + ((TEXT_VIEW_BLANK_NEVER != m_nInsertBlank)?1:0);

	if (!bCanAdd2Buffer(dwMsgLen))
		return -1;

	szText = new TCHAR[dwTextLen+1];
	if (!szText)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		return -1;
	}

	if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
						szMess, 0, 0, szText, dwTextLen+1, (va_list*) pszArgs))
	{
		delete [] szText;
		return -1;
	}

	// We need Info Character formatting
	if (NULL == pCharFormat)
	{
		if (m_pMsgTypePropInfo[MsgType-mtBeginInfo])
			pCharFormat = &m_pMsgTypePropInfo[MsgType-mtBeginInfo]->CharFormat;
		else
			pCharFormat = &m_InfoMsgTypeProp.CharFormat;
	}

	iRetVal = iDisplayMsgText(szText, dwTextLen, MsgType, MembChange,
								/*bShowURLs*/ FALSE, /*bInformFull*/ TRUE, 
								/*bAppend*/ FALSE, /*lIndent*/ 0,
							  pCharFormat,
							  prgdwLocalFormatting,
							  cFormats);

	delete [] szText;
	delete [] pszArgs;

	if (bFreeLocalFormatting && prgdwLocalFormatting)
		delete [] prgdwLocalFormatting;

	return iRetVal;
}


BOOL TEXTVIEWCLASSNAME::bHandleLink(ENLINK *penlink)
{
	BOOL			bRet = TRUE;
	TEXTRANGE		tr;
	TCHAR			szUrl[g_nMaxLength];

	if (WM_SETCURSOR == penlink->msg)
	{
		// Set the cursor to a HAND
		::SetCursor(m_hCursorHand);
		return TRUE;
	}
	
	// Only respond to full clicks.. and if the control key is down, let them edit/copy it
	if (WM_LBUTTONDOWN != penlink->msg || (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		return FALSE;

	// Must make sure that the URL isn't too long..if it is, we will just have to truncate it
	tr.chrg.cpMin = penlink->chrg.cpMin;
	tr.chrg.cpMax = penlink->chrg.cpMax;
	
	if ((tr.chrg.cpMax - tr.chrg.cpMin) > (sizeof(szUrl) - 1))
		tr.chrg.cpMax = sizeof(szUrl) - 1;

	tr.lpstrText = szUrl; // let Richedit place the URL in the rest of the buffer
	
	// Get the selection
	::SendMessage(m_hwnd, EM_GETTEXTRANGE, 0L, (LPARAM) &tr);
	
	// Eliminate an \r and \n at the end
	int cch;

	cch = _tcslen(szUrl) - 1;
	while (cch >= 0)
	{
		switch(szUrl[cch])
		{
			default:
				szUrl[cch+1] = g_chEOS;
				goto launch;
			
			case g_chLF:
			case g_chCR:
			case g_chSpace:
				break;					
		}
		--cch;
	}

launch:	
/*	// Do we need to start a new instance of the browser?
	bStartNewBrowser = bUrlNeedsNewBrowser(szUrl, szExePath);

	// Launch it
	
	hCursor = ::SetCursor(::LoadCursor(NULL, IDC_APPSTARTING));
*/
	bRet = m_urlrec.bLaunchUrl(m_hwnd,szUrl,m_bNewBrowser);

//	::SetCursor(hCursor);

	return bRet;
}


//=--------------------------------------------------------------------------=
// CTextView::bAutoScrollTextView
//=--------------------------------------------------------------------------=
// resets the variables, or checks if the last history line is visible
// or scrolls the history window to make the last line visible
//
// Parameters:
//    BOOL				- [in] TRUE if we just want to reset the variables
//    BOOL				- [in] TRUE if we just want to see if the last line is visible
//
// Output:
//    BOOL				TRUE if we scrolled the history window
//
// Notes:				
//
BOOL TEXTVIEWCLASSNAME::bAutoScrollTextView(BOOL bCheckEndInView)
{
	BOOL		bEndInView = TRUE;
	BOOL		bRet;

	SCROLLINFO si;

	si.cbSize = sizeof(si);
	si.fMask = SIF_ALL;

	if (bRet = ::GetScrollInfo(m_hwnd,SB_VERT,&si))
	{
//		if ((si.nMin+ si.nTrackPos + (int)si.nPage) < si.nMax)
		if ( m_bThumb || ((si.nMin+ si.nPos + (int)si.nPage) < si.nMax))
			bEndInView = FALSE;
	}
	
//	char temp[256];

	if (bCheckEndInView)
	{

//		if (bRet)
//		{
//			wsprintf(temp,"EndinView: %d with %d - %d ( %d ) and nPos is %d and nTrackPos is %d\r\n",bEndInView,si.nMin,si.nMax,si.nPage,si.nPos, si.nTrackPos);
//			OutputDebugThreadIdString(temp);
//		}

		return bEndInView;
	}
	else if ((bRet) && ((si.nMin+ si.nPos + (int)si.nPage) < si.nMax))
	{
//		wsprintf(temp,"AutoScrolling %d - %d ( %d ) and nPos is %d\r\n",si.nMin,si.nMax,si.nPage,si.nPos);
//		OutputDebugThreadIdString(temp);

	// 16bit scroll limitation for WM_VSCROLL
		if (si.nMax < SHRT_MAX)
		{
			::SendMessage(m_hwnd, WM_VSCROLL,
							MAKELPARAM(SB_THUMBPOSITION,si.nMax-si.nPage/*+DEFAULT_BOTTOM_BORDER*/), 
							0L);
		} 
		else
		{
			::SendMessage(m_hwnd, WM_VSCROLL, SB_BOTTOM, 0L);
			::SendMessage(m_hwnd, WM_VSCROLL, SB_PAGEUP, 0L);
			::SendMessage(m_hwnd, WM_VSCROLL, SB_LINEDOWN, 0L);
		}
	}
#ifdef DEBUG
	else
	{
//		wsprintf(temp,"***** No Scroll %d - %d ( %d ) and nPos is %d with bRet %d\r\n",si.nMin,si.nMax,si.nPage,si.nPos,bRet);
//		OutputDebugThreadIdString(temp);
	}
#endif DEBUG

	return TRUE;
}



BOOL TEXTVIEWCLASSNAME::bCanAdd2Buffer(DWORD dwMsgLen)
{
	// Should be common RamuM	
	if (m_dwBuffSize + dwMsgLen > (DWORD) m_dwBuffMaxSize)
	{
		// not enough empty room in history to hold the message
		if (dwMsgLen > (DWORD) m_dwBuffMaxSize)
		{
			// history buffer is too small to hold message
			::SetLastError(ERROR_INVALID_USER_BUFFER);
			return FALSE;
		}

		DWORD dwRetVal = dwClearTextViewBuffer(dwMsgLen+m_dwBuffSize-m_dwBuffMaxSize+1);

		// Do we need to return iRetVal?
		if ( dwRetVal == -1 )
			return FALSE;
	}

	return TRUE;
}


//TODO: member Status based URL colors
void TEXTVIEWCLASSNAME::RegisterTextLinks(int iStartPoint, LPINT nUrlBounds, INT nUrlNum)
{
	CHARFORMAT	cfOld;
	CHARRANGE	crSav;
	LPARAM		lNotifSuppression;

	ASSERT(m_hwnd,  "m_hwnd  is NULL in CTextView::RegisterTextLinks");

	// Save the current selection and then hide it
	::SendMessage(m_hwnd, EM_EXGETSEL, 0L, (LPARAM) &crSav);
	::SendMessage(m_hwnd, EM_HIDESELECTION, (WPARAM) TRUE, (LPARAM) FALSE);

	// And save and reset the Event notification mask to NOT send any right now
	lNotifSuppression = ::SendMessage(m_hwnd, EM_SETEVENTMASK, 0L, 0L);

	// also save default character formatting
	::SendMessage(m_hwnd, EM_GETCHARFORMAT, (WPARAM) FALSE, (LPARAM) &cfOld);

//		m_urlrec.m_urllist.SetStartPoint(cch);
//		m_urlrec.m_urllist.FDoActionOnList(ACT_SHOW_URL);	// regisb: treat return value

	while (nUrlNum)
	{
		CHARRANGE	cr;

		cr.cpMin = iStartPoint + nUrlBounds[nUrlNum*2-2];
		cr.cpMax = iStartPoint + nUrlBounds[nUrlNum*2-1];
		::SendMessage(m_hwnd, EM_EXSETSEL,		(WPARAM) 0L,			(LPARAM) &cr);
		::SendMessage(m_hwnd, EM_SETCHARFORMAT,	(WPARAM) SCF_SELECTION,	(LPARAM) &m_URLMsgTypeProp.CharFormat);

		nUrlNum--;
	}

	// restoration
	::SendMessage(m_hwnd, EM_EXSETSEL,		(WPARAM) 0L,			(LPARAM) &crSav);
	::SendMessage(m_hwnd, EM_HIDESELECTION,	(WPARAM) FALSE,			(LPARAM) FALSE);		
	::SendMessage(m_hwnd, EM_SETCHARFORMAT,	(WPARAM) SCF_SELECTION,	(LPARAM) &cfOld);
	::SendMessage(m_hwnd, EM_SETEVENTMASK,	(WPARAM) 0L,			(LPARAM) lNotifSuppression);
}



BOOL TEXTVIEWCLASSNAME::bSetIndent(LONG lIndent)
{
	PARAFORMAT	pf;
	LRESULT		lr;

	pf.dwMask = PFM_STARTINDENT;
	pf.dxStartIndent = lIndent + 72;	// 72 is magic number for small offset from left
	pf.cbSize = sizeof(PARAFORMAT);
	lr = ::SendMessage(m_hwnd, EM_SETPARAFORMAT, 0L, (LPARAM) &pf);
	
	if (0 != lr)
		::SetLastError(ERROR_INVALID_PARAMETER);

	return lr != 0;
}


//=--------------------------------------------------------------------------=
// CTextView::SetDefaultMsgTypeProperties
//=--------------------------------------------------------------------------=
// sets the colors to be used in the history text box. the colors are customized
// if the system window back color is white
//
// Parameters:
//    NONE
//
// Output:
//    NONE
//
// Notes:				
//
BOOL TEXTVIEWCLASSNAME::bSetDefaultMsgTypeProperties()
{
	int mt;
	int ms;

	// First Set all defaults.
	for (ms = 0; ms < msEndEnum; ms++)
	{
		for (mt = 0; mt < mtBeginInfo; mt++)
			m_pMsgTypePropText[ms][mt] = NULL;

		for (mt = 0; mt < mtBeginActions; mt++)
			m_pMsgTypePropHead[ms][mt] = NULL;
	}

	for (mt = 0; mt < (mtEndEnum-mtBeginInfo); mt++)
		m_pMsgTypePropInfo[mt] = NULL;

	m_URLMsgTypeProp.CharFormat  = gDefURLFormat;
	m_HeadMsgTypeProp.CharFormat = gDefHeadFormat;
	m_TextMsgTypeProp.CharFormat = gDefTextFormat;
	m_InfoMsgTypeProp.CharFormat = gDefInfoFormat;

	CHARFORMAT CharFormatTemp;

	ZeroMemory(&CharFormatTemp, sizeof(CHARFORMAT));

	CharFormatTemp.cbSize = sizeof(CHARFORMAT);
	CharFormatTemp.dwMask  = CFM_COLOR;
	CharFormatTemp.dwEffects = 0 ;

	// Override specifics.
	for (ms = 0; ms < msEndEnum; ms++)
	{
		// All Actions
		CharFormatTemp.dwMask		|= CFM_BOLD;
		CharFormatTemp.dwEffects	|= CFE_BOLD;
		CharFormatTemp.crTextColor	= RGB(0,128,0);
		bSetMessageFormat(&CharFormatTemp, mtJoin, (MEMBER_STATUS) ms);
		CharFormatTemp.crTextColor	= RGB(255,0,0);
		bSetMessageFormat(&CharFormatTemp, mtLeave, (MEMBER_STATUS) ms);
		CharFormatTemp.dwMask		&= ~CFM_BOLD;
		CharFormatTemp.dwEffects	&= ~CFE_BOLD;

		CharFormatTemp.crTextColor	= RGB(255,0,255);
		bSetMessageFormat(&CharFormatTemp, mtBroadcast, (MEMBER_STATUS) ms);
		
		CharFormatTemp.crTextColor	= RGB(0,0,128);
		bSetMessageFormat(&CharFormatTemp, mtThought, (MEMBER_STATUS) ms, TRUE);

		CharFormatTemp.dwMask		|= CFM_ITALIC;
		CharFormatTemp.dwEffects	|= CFE_ITALIC;
		CharFormatTemp.crTextColor	= RGB(128,0,128);
		bSetMessageFormat(&CharFormatTemp, mtAction, (MEMBER_STATUS) ms);
		CharFormatTemp.dwMask		&= ~CFM_ITALIC;
		CharFormatTemp.dwEffects	&= ~CFE_ITALIC;

		CharFormatTemp.dwMask		|= CFM_UNDERLINE;
		CharFormatTemp.dwEffects	|= CFE_UNDERLINE;
		CharFormatTemp.crTextColor = RGB(128,0,0);
		bSetMessageFormat(&CharFormatTemp, mtWhisper, (MEMBER_STATUS) ms, TRUE);
		CharFormatTemp.dwMask		&= ~CFM_UNDERLINE;
		CharFormatTemp.dwEffects	&= ~CFE_UNDERLINE;

		// Regular Messages, Header & Text
		CharFormatTemp.crTextColor = RGB(128,128,128);
		bSetMessageFormat(&CharFormatTemp, mtPrivate, (MEMBER_STATUS) ms, TRUE);	// Header
		bSetMessageFormat(&CharFormatTemp, mtReply, (MEMBER_STATUS) ms, TRUE);		// Header
	}

	// Bold, any host related headers
//	CharFormatTemp				= gDefHeadFormat;
/* Bolding Host related headers is not default anymore, RamuM 3/18

	ZeroMemory(&CharFormatTemp,sizeof(CharFormatTemp));
	CharFormatTemp.dwMask		= CFM_BOLD;
	CharFormatTemp.dwEffects	= CFE_BOLD;

	bAddMSMsgFormat(&CharFormatTemp,msHost,TRUE);
*/

	return TRUE;
	//TODO: Define consts for specific colors
}

// To add on to existing Message type properties based on the Member_status
// Will modify the existing formats or will add a new format if doesn't exist
// previously, starting from the default format for header / text
// Basically a helper function, currently only dwMask and dwEffects are used.
BOOL TEXTVIEWCLASSNAME::bAddMSMsgFormat(CHARFORMAT *pCharFormat, MEMBER_STATUS Member, BOOL bHeader )
{
	CHARFORMAT CharFormatTemp;

	// Get the default
	if (bHeader)
		CharFormatTemp = m_HeadMsgTypeProp.CharFormat;
	else 
		CharFormatTemp = m_TextMsgTypeProp.CharFormat;

	CharFormatTemp.dwMask		|= pCharFormat->dwMask;
	CharFormatTemp.dwEffects	|= pCharFormat->dwEffects;
	// if specific color is specified in the over-ride, make sure that takes
	// the precedence.
	if (pCharFormat->dwMask & CFM_COLOR)
	{
		if (!(pCharFormat->dwEffects & CFE_AUTOCOLOR))
		{
			CharFormatTemp.dwEffects &= ~CFE_AUTOCOLOR;
			CharFormatTemp.crTextColor = pCharFormat->crTextColor;
		}
	}
	// TODO: Add the rest of the stuff

	CHARFORMAT *pCharFormatTemp = NULL;
	int mt = (bHeader)? mtBeginActions : mtEndEnum;	// find the bounds
	for (mt -= 1; mt >= 0; mt--)
	{
		if (bGetMessageFormat(&pCharFormatTemp,(MSG_TYPE)mt,Member,bHeader))
		{
			pCharFormatTemp->dwMask		|= pCharFormat->dwMask;
			pCharFormatTemp->dwEffects	|= pCharFormat->dwEffects;
			// if specific color is specified in the over-ride, make sure that takes
			// the precedence.
			if (pCharFormat->dwMask & CFM_COLOR)
			{
				if (!(pCharFormat->dwEffects & CFE_AUTOCOLOR))
				{
					pCharFormatTemp->dwEffects &= ~CFE_AUTOCOLOR;
					pCharFormatTemp->crTextColor = pCharFormat->crTextColor;
				}
			}
			// TODO: Add the rest of the stuff
		}
		else
			bSetMessageFormat(&CharFormatTemp,(MSG_TYPE)mt,Member,bHeader);
	}
	return TRUE;
}


// To Delete or Reset specific MessageTypeProperties
BOOL TEXTVIEWCLASSNAME::bReSetDefaultMsgTypeProperties(BOOL bReset)
{
	ClearDefaultMsgTypeProperties();

	if (bReset)
		return bSetDefaultMsgTypeProperties();

	return TRUE;
}


void TEXTVIEWCLASSNAME::ClearDefaultMsgTypeProperties()
{
	int ms, mt;
	for (ms = 0; ms < msEndEnum; ms++)
	{
		for (mt = 0; mt < mtBeginInfo; mt++)
		{
			if (m_pMsgTypePropText[ms][mt])
				delete m_pMsgTypePropText[ms][mt];
			m_pMsgTypePropText[ms][mt] = NULL;
		}

		for (mt = 0; mt < mtBeginActions; mt++)
		{
			if (m_pMsgTypePropHead[ms][mt])
				delete m_pMsgTypePropHead[ms][mt];
			m_pMsgTypePropHead[ms][mt] = NULL;
		}
	}

	for (mt = 0; mt < (mtEndEnum-mtBeginInfo); mt++)
	{
		if (m_pMsgTypePropInfo[mt])
			delete m_pMsgTypePropInfo[mt];
		m_pMsgTypePropInfo[mt] = NULL;
	}
}


//=--------------------------------------------------------------------------=
// CTextView::bReSetDefaultMessageStrings
//=--------------------------------------------------------------------------=
// defines the default resource IDs used to load the history headers and info
//
// Parameters:
//    NONE
//
// Output:
//    NONE
//
// Notes:				
//
BOOL TEXTVIEWCLASSNAME::bReSetMessageStrings(void)
{
	int mt;
	int ms;
	TCHAR szTemp[g_nMaxLength];
	int nTextLen;

	ClearMessageStrings();
	ZeroMessageStrings();

	for (mt = 0; mt < mtEndEnum; mt++)
	{
		nTextLen = ::LoadString(m_hInstance, IDS_NORMAL_HEADER + mt, 
										(LPTSTR) szTemp, g_nMaxLength-1) ;
		

		bSetString(&m_szMsgType[mt],&m_nMsgTypeLen[mt],szTemp,nTextLen);
	}

	for (ms = 0; ms < msEndEnum; ms++)
	{
		nTextLen = ::LoadString(m_hInstance, IDS_HOST_HEADER + ms, 
										(LPTSTR) szTemp, g_nMaxLength-1);

		bSetString(&m_szMembStatus[ms],&m_nMembStatusLen[ms],szTemp,nTextLen);
	}

	return TRUE;
}


void TEXTVIEWCLASSNAME::ClearMessageStrings(void)
{
	int mt, ms;
	for (mt = 0; mt < mtEndEnum; mt++)
	{
		if (NULL != m_szMsgType[mt])
		{
			delete [] m_szMsgType[mt];
//			m_nMsgTypeLen[mt] = 0;
		}
	}

	for (ms = 0; ms < msEndEnum; ms++)
	{
		if (NULL != m_szMembStatus[ms])
		{
			delete [] m_szMembStatus[ms];
//			m_nMembStatusLen[ms] = 0;
		}
	}

}


void TEXTVIEWCLASSNAME::ZeroMessageStrings(void)
{
	ZeroMemory(&m_szMsgType, sizeof(m_szMsgType));
	ZeroMemory(&m_szMembStatus, sizeof(m_szMembStatus));
	ZeroMemory(&m_nMsgTypeLen, sizeof(m_nMsgTypeLen));
	ZeroMemory(&m_nMembStatusLen, sizeof(m_nMembStatusLen));
}

//=--------------------------------------------------------------------------=
// CTextView::bGetMessageString
//=--------------------------------------------------------------------------=
// returns the currently set resource ID for a given message type OR 
// member status
//
// Parameters:
//    MSG_TYPE			- [in] message type of resource ID to get 
//	  MEMBER_STATUS		- [in] member status of resource ID to get
//
// Output:
//
// Notes:				Only one parameter should be valid, the other should be < 0
//
BOOL TEXTVIEWCLASSNAME::bGetMessageString(LPCTSTR *pszMsgString, DWORD *pdwMsgLen, MSG_TYPE MsgType, MEMBER_STATUS Memb)
{
	if ((MsgType >= 0 && Memb >= 0) || (MsgType >= mtEndEnum) || (Memb > msEndEnum))
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	ASSERT(pszMsgString,"pszmsgString is NULL in CTextView::bGetMessageString");

	if (MsgType >= 0)
	{
		*pszMsgString = m_szMsgType[MsgType];
		if (pdwMsgLen)
			*pdwMsgLen = m_nMsgTypeLen[MsgType];
	}
	else
	{
		ASSERT(Memb >= 0, "Memb < 0 in CTextView::uGetResourceID");
		*pszMsgString =  m_szMembStatus[Memb];
		if (pdwMsgLen)
			*pdwMsgLen = m_nMembStatusLen[MsgType];
	}

	return TRUE;
}


//=--------------------------------------------------------------------------=
// CTextView::bSetMessageString
//=--------------------------------------------------------------------------=
// sets a resource ID given a message type OR member status
//
// Parameters:
//    MSG_TYPE			- [in] message type of resource ID to set
//	  MEMBER_STATUS		- [in] member status of resource ID to set
//
// Output:
//    BOOL				- TRUE if the function succeeds
//
// Notes:				Only one parameter should be valid, the other should be < 0
//
BOOL TEXTVIEWCLASSNAME::bSetMessageString(LPCTSTR pszMsgString, DWORD dwMsgLen, MSG_TYPE MsgType, MEMBER_STATUS Memb)
{
	if ((MsgType >= 0 && Memb >= 0) || (MsgType >= mtEndEnum) || (Memb > msEndEnum))
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}
	if (MsgType >= 0)
	{
		bSetString(&m_szMsgType[MsgType],&m_nMsgTypeLen[MsgType],pszMsgString,dwMsgLen);
	}
	else
	{
		ASSERT(Memb >= 0, "Memb < 0 in CTextView::uGetResourceID");
		bSetString(&m_szMembStatus[Memb],&m_nMembStatusLen[Memb],pszMsgString,dwMsgLen);
	}
	return TRUE;
}


BOOL TEXTVIEWCLASSNAME::bSetString(LPTSTR *pszDest, USHORT *pnLen, LPCTSTR szSrc, DWORD dwLen)
{
	// Can be optimized to use same string for string length less than
	// current one.

	if (NULL != *pszDest)
		delete [] (*pszDest);

	if (-1 == dwLen)
		dwLen = _tcslen(szSrc);

	*pszDest = new TCHAR[dwLen+1];

	if (NULL == *pszDest)
	{
		::SetLastError(ERROR_OUTOFMEMORY);
		return FALSE;
	}

	_tcscpy(*pszDest,szSrc);
	*pnLen = (USHORT) dwLen;
	return TRUE;
}


/* Old AutoScroll

BOOL TEXTVIEWCLASSNAME::bAutoScrollTextView(BOOL bResetVars, BOOL bCheckEndInView)
{

	RECT		rc;
	POINT		posEnd;
	int			cLines, cVis;
	LRESULT		chLast;

	// TODO : Remove this old code
	if (bResetVars)
	{
//		m_nLineHeight = m_nUpperMargin = 0;
		return FALSE;
	}


	if (!::GetClientRect(m_hwnd, &rc))
		return FALSE;

	ASSERT(rc.right >= 0,"ClientRect width is <0 in CTextView::bAutoScrollTextView");
	ASSERT(rc.bottom >= 0,"ClientRect height is <0 in CTextView::bAutoScrollTextView");

	cLines = ::SendMessage(m_hwnd, EM_GETLINECOUNT, 0L, 0L);

	// What is the last completely visible line?
	posEnd.x = rc.left;		// Amazingly, MAKELPARAM doesn't work
	posEnd.y = rc.bottom;
	chLast = ::SendMessage(m_hwnd, EM_CHARFROMPOS, 0L, (LPARAM) &posEnd);
	cVis   = ::SendMessage(m_hwnd, EM_LINEFROMCHAR, (WPARAM)chLast, 0L) + 1;

	char tt[256];
	wsprintf(tt,"cVis is %d and cLines is %d; %d of %d\r\n",cVis,cLines,chLast,m_dwBuffSize);
	OutputDebugString(tt);

	if (bCheckEndInView)
		return cLines <= cVis;
	else
	{
		if (cLines >= cVis)
		{
			CHARFORMAT cf;

			// Get the current char height
			cf.cbSize = sizeof(cf);
			cf.dwMask = CFM_SIZE;
			::SendMessage(m_hwnd, EM_GETCHARFORMAT, (WPARAM) 0, (LPARAM) &cf);
			int height = MulDiv(cf.yHeight,m_nLogPixelsY,1440);

			// Get the position of the last character in the textbuffer
			::SendMessage(m_hwnd, EM_POSFROMCHAR, (WPARAM) &posEnd, (LPARAM) m_dwBuffSize);

			wsprintf(tt,"posEnd.Y is %d and height is %d for rcBottom %d\r\n",posEnd.y,height,rc.bottom);
			OutputDebugString(tt);

			// If the bottom of the last line falls beyond current rect, Scroll
			if ((posEnd.y + height) > rc.bottom)
			{
				::SendMessage(m_hwnd, EM_LINESCROLL, 0L, (LPARAM)(cLines - cVis + 1));
				wsprintf(tt,"Scrolling %d Lines\r\n",cLines-cVis+1);
				OutputDebugString(tt);
			}
		}
		return TRUE;
	}

}

*/

/* Old stuff
//=--------------------------------------------------------------------------=
// CTextView::bSaveAndMoveSel
//=--------------------------------------------------------------------------=
// saves the current selection range in the Text View window
//
// Parameters:
//    CHARRANGE			- [out] current history selection range
//
// Output:
//    BOOL				TRUE if range was set
//						FALSE if no selection in history
//
// Notes:				
//
BOOL TEXTVIEWCLASSNAME::bSaveAndMoveSel(CHARRANGE *pcrSav)
{
	long		lTextLen;
	CHARRANGE	cr;

	ASSERT(pcrSav, "pcrSav is NULL in CTextView::FSaveAndMoveSel");

	::SendMessage(m_hwnd, EM_EXGETSEL, 0L, (LPARAM) pcrSav);
	lTextLen = ::SendMessage(m_hwnd, WM_GETTEXTLENGTH, 0L, 0L);
	if (pcrSav->cpMin != pcrSav->cpMax || pcrSav->cpMin != lTextLen)
	{
		cr.cpMin = cr.cpMax = lTextLen;
		::SendMessage(m_hwnd, EM_EXSETSEL, 0L, (LPARAM) &cr);
		return TRUE;
	}
	else
		return FALSE;
}

*/
