#include "stdafx.h"
#include "..\inc\urlutil.h"

#include "chat.h"	// just so we can tell if it's embedded...
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatDoc.h"
#include "ui.h"		// for GetPrimaryView
#include "format.h"
#include "ccommon.h"
#include "rtfctrl.h"

extern CChatApp theApp;

extern "C" void *GetMime();

COLORREF linkColor = RGB(0, 0, 255);

CUrlRec g_urlRec;


const char* SzSkipOneFormat(const char *szInput, WORD *pwFormat)
{
	const char *szRead = szInput;
	WORD wFormat;

	ASSERT(szRead);

	if (pwFormat)
		wFormat = *pwFormat;

	switch (*szRead)
	{
		case chCtlColor:
		{
			// Maybe skip the colors too - format is ^c<color1>,<color2> where color1 and color2 are >=0 and <= 99
			// or ^c,<color> where color is >=0 and <= 99 
			// or ^c<color> where color is >=0 and <= 99
			if ('0' > *(szRead+1) || '9' < *(szRead+1))
			{
				if (*(szRead+1) == ',')
				{
					// ^c,?   case
					szRead++;
					if ('0' <= *(szRead+1) && '9' >= *(szRead+1))
					{
						// ^c,X? case
						// we already know that the foreground color is omitted and therefore takes the default value
						// also a background color is given
						if (pwFormat)
						{
							wFormat &= ~wForeground;
							wFormat |= wBackground;
							wFormat &= 0xFF00; // erase the old colors
						}
						szRead++;
						if ('0' <= *(szRead+1) && '9' >= *(szRead+1))
						{
							// ^c,XY case
							if (pwFormat)
								wFormat |= (((*szRead - '0') * 10 + (*(szRead+1) - '0')) % 16);
							szRead++;
						}
						else
						{
							// ^c,XR case
							if (pwFormat)
								wFormat |= (*szRead - '0');
						}
					}
					else
					{
						// ^c,R case: this is just a lonely ^c ==> back to default colors
						if (pwFormat)
						{
							wFormat &= ~wForeground;
							wFormat &= ~wBackground;
							wFormat &= 0xFF00;	// erase the old colors
						}
						szRead--;
					}
				}
				else
				{
					// this is just a lonely ^c ==> back to default colors
					if (pwFormat)
					{
						wFormat &= ~wForeground;
						wFormat &= ~wBackground;
						wFormat &= 0xFF00;	// erase the old colors
					}
				}
				break;
			}
			
			szRead++;
			// we already know that a foreground color is specified
			if (pwFormat)
			{
				wFormat |= wForeground;
				wFormat &= 0xFF0F;
			}
			if ('0' <= *(szRead+1) && '9' >= *(szRead+1))
			{
				// ^cWX case
				if (pwFormat)
					wFormat |= (((*szRead - '0') * 10 + (*(szRead+1) - '0')) % 16) << 4;
				szRead++;
				if (',' == *(szRead+1))
				{
					szRead++;
					if ('0' <= *(szRead+1) && '9' >= *(szRead+1))
					{
						// ^cWX,Y case
						if (pwFormat)
						{
							wFormat |= wBackground;
							wFormat &= 0xFFF0;
						}
						szRead++;
						if ('0' <= *(szRead+1) && '9' >= *(szRead+1))
						{
							// ^cWX,YZ case
							if (pwFormat)
								wFormat |= ((*szRead - '0') * 10 + (*(szRead+1) - '0')) % 16;
							szRead++;
						}
						else
						{
							// ^cWX,YR case
							if (pwFormat)
								wFormat |= (*szRead - '0');
						}
					}
					else
					{
						// ^cWX,R case
						szRead--;
					}
				}
			}
			else
			{
				if (',' == *(szRead+1))
				{
					// ^cW, case
					if (pwFormat)
						wFormat |= (*szRead - '0') << 4;
					szRead++;
					if ('0' <= *(szRead+1) && '9' >= *(szRead+1))
					{
						// ^cW,X case
						if (pwFormat)
						{
							wFormat |= wBackground;
							wFormat &= 0xFFF0;
						}

						szRead++;
						if ('0' <= *(szRead+1) && '9' >= *(szRead+1))
						{
							// ^cW,XY case
							if (pwFormat)
								wFormat |= ((*szRead - '0') * 10 + (*(szRead+1) - '0')) % 16;
							szRead++;
						}
						else
						{
							// ^cW,XR case
							if (pwFormat)
								wFormat |= (*szRead - '0');
						}
					}
					else
					{
						// ^cW,R case
						szRead--;
					}
				}
				else
				{
					// ^cWR case
					if (pwFormat)
						wFormat |= (*szRead - '0') << 4;
				}
			}
			break;
		}

		case chCtlBold:
			if (pwFormat)
			{
				if (wFormat & wBold)
					wFormat &= ~wBold;
				else
					wFormat |= wBold;
			}
			break;

		case chCtlItalic:
			if (pwFormat)
			{
				if (wFormat & wItalic)
					wFormat &= ~wItalic;
				else
					wFormat |= wItalic;
			}
			break;

		case chCtlFixedPitchFont:
			if (pwFormat)
			{
				if (wFormat & wFixedPitch)
					wFormat &= ~wFixedPitch;
				else
					wFormat |= wFixedPitch;
			}
			break;

		case chCtlUnderline:
			if (pwFormat)
			{
				if (wFormat & wUnderline)
					wFormat &= ~wUnderline;
				else
					wFormat |= wUnderline;
			}
			break;

		case chCtlSymbol:
			if (pwFormat)
			{
				if (wFormat & wSymbol)
					wFormat &= ~wSymbol;
				else
					wFormat |= wSymbol;
			}
			break;

		default:
			ASSERT(0);
	}

	if (pwFormat)
		*pwFormat = wFormat;

	return szRead + 1;
}


short nResettingSequence(const char *szInput, char *szResetSeq)
{
	short		nRetVal = 0;
	const char*	szRead = szInput;
	WORD		wFormat = 0;

	ASSERT(szInput);
	ASSERT(szResetSeq);

	while (g_chEOS != *szRead)
	{
		switch (*szRead)
		{
			case chCtlColor:
			case chCtlBold:
			case chCtlItalic:
			case chCtlFixedPitchFont:
			case chCtlUnderline:
			case chCtlSymbol:
				szRead = (char*) SzSkipOneFormat((const char*) szRead, &wFormat);
				break;

			default:
				if (IsDBCSLeadByte((BYTE) *szRead))	// so that we don't have a problem if one of the formatting
													// control character happens to be a trail byte in the string
					szRead++;
				szRead++;
		}
	}

	if (wFormat)
	{
		if (wFormat & wBold)
			szResetSeq[nRetVal++] = chCtlBold;
		if (wFormat & wItalic)
			szResetSeq[nRetVal++] = chCtlItalic;
		if (wFormat & wUnderline)
			szResetSeq[nRetVal++] = chCtlUnderline;
		if (wFormat & wFixedPitch)
			szResetSeq[nRetVal++] = chCtlFixedPitchFont;
		if (wFormat & wSymbol)
			szResetSeq[nRetVal++] = chCtlSymbol;
		if ((wFormat & wForeground) || (wFormat & wBackground))
			szResetSeq[nRetVal++] = chCtlColor;
	}

	szResetSeq[nRetVal] = g_chEOS;
	return nRetVal;
}


char* SzControlLess(char *szInput, CDWordArray *prgdwFormatting)
{
	// Formatting DWORD constitution:
	// OOOOEEFB	where	- OOOO is offset of first character
	//					- EE are effects (bold, italic, etc...)
	//					- F is foreground color
	//					- B is background color

	if (prgdwFormatting)
		prgdwFormatting->RemoveAll();

	ASSERT(szInput);
	char *szOutput = NULL, *szRead = szInput, *szWrite;
	WORD wFormat = 0;
	BOOL bNewFormatInPlace = FALSE;

	while (g_chEOS != *szRead)
	{
		switch (*szRead)
		{
			case chCtlColor:
			case chCtlBold:
			case chCtlItalic:
			case chCtlFixedPitchFont:
			case chCtlUnderline:
			case chCtlSymbol:
				bNewFormatInPlace = TRUE;
				szRead = (char*) SzSkipOneFormat((const char*) szRead, &wFormat);
				break;

			default:
				if (!szOutput)
				{
					//szWrite = szOutput = szRead;
					szWrite = szOutput = szInput;
				}	
				else
				{
					szWrite++;
					//*szWrite = *szRead;
				}
				*szWrite = *szRead;

				if (bNewFormatInPlace && prgdwFormatting)
				{
					// Add a new item to our prgdwFormatting array
					// Offset == szWrite - szOutput
					prgdwFormatting->Add(MAKELONG(wFormat, szWrite - szOutput));
					bNewFormatInPlace = FALSE;
				}

				if (IsDBCSLeadByte((BYTE) *szRead))	// so that we don't have a problem if one of the formatting
				{									// control character happens to be a trail byte in the string
					szWrite++; szRead++;
					*szWrite = *szRead;
				}
				szRead++;
		}
	}

	if (!szOutput)
		szOutput = szInput;
	else
	{
		szWrite++;
		*szWrite = g_chEOS;
	}

	return szOutput;
}


char* SzReplaceFormattedString(const char *szCLReplaceWhat, 
							   const char *szCLReplaceBy, 
							   const char *szCLReplaceIn,
							   CDWordArray *prgdwReplaceByFormatting,
							   CDWordArray *prgdwReplaceInFormatting,
							   UINT uFlags)
{
	LPSTR			szReplaced;
	LONG			cpMin, cpTmp;
	FINDTEXTEX		fte;
	CRtfCtrl		rtfIn, rtfBy;
	CRichEditCtrl	rtfTmp;
	UINT			cbLenBy, cbLenWhat;
	CString			strReplaced;
	RECT			rect;

	ASSERT(szCLReplaceWhat);
	ASSERT(szCLReplaceBy);
	ASSERT(szCLReplaceIn);

	cbLenBy = strlen(szCLReplaceBy);
	cbLenWhat = strlen(szCLReplaceWhat);

	rect.left	= 0;
	rect.top	= 0;
	rect.right	= 50;
	rect.bottom	= 20;

	if (!rtfIn.Create(WS_CHILD, rect, GetFrame(), ID_MAINFRAMERICH1) ||
		!rtfBy.Create(WS_CHILD, rect, GetFrame(), ID_MAINFRAMERICH2) ||
		!rtfTmp.Create(WS_CHILD, rect, GetFrame(), ID_MAINFRAMERICH3))
		return NULL;

	rtfTmp.Paste();

	fte.chrg.cpMin = 0;
	fte.chrg.cpMax = 99999;
	fte.lpstrText = (char*) szCLReplaceWhat;

	rtfIn.DefineDefaultCharFormat();
	rtfBy.DefineDefaultCharFormat();
	rtfIn.UseDefaultCharFormat(FALSE);
	rtfBy.UseDefaultCharFormat(FALSE);

	rtfIn.bSetWindowFormattedText(szCLReplaceIn, prgdwReplaceInFormatting);
	rtfBy.bSetWindowFormattedText(szCLReplaceBy, prgdwReplaceByFormatting);

	rtfBy.SetSel(0, -1);
	rtfBy.Copy();

	// while (-1 != rtfIn.FindText(0L, &fte))
	while (-1 != ::SendMessage(rtfIn.m_hWnd, EM_FINDTEXTEX, (WPARAM) uFlags, (LPARAM) &fte))
	{
		fte.chrgText.cpMax = fte.chrgText.cpMin;
		rtfIn.SetSel(fte.chrgText);
		rtfIn.Paste();
		fte.chrgText.cpMin += cbLenBy;
		fte.chrgText.cpMax += cbLenBy + cbLenWhat;
		rtfIn.SetSel(fte.chrgText);
		rtfIn.ReplaceSel("");
		fte.chrg.cpMin = fte.chrgText.cpMin;
	}

	CDWordArray* prgdwFormatting = PRGDWGetFormatting(&rtfIn, rtfIn.m_pFont, rtfIn.m_crTextColor);
	rtfIn.GetWindowText(strReplaced);

	if (prgdwFormatting)
	{
		szReplaced = SzControlFull((const char*) strReplaced, prgdwFormatting);
		FreeAndNullFormatting(&prgdwFormatting);
	}
	else
	{
		if (szReplaced = new char[strReplaced.GetLength()+1])
			strcpy(szReplaced, strReplaced);
	}

	rtfTmp.SetSel(0, -1);
	rtfTmp.Copy();

	return szReplaced;
}


short nFillFormatting(char *szFormatting, WORD wCurFormat, WORD wNextFormat, char chFirstFormattedChar)
{
	BYTE	byteForeground, byteBackground;
	short	cbReturn = 0;

	ASSERT(szFormatting);
	*szFormatting = '\0';

	if ((wNextFormat & wBold) != (wCurFormat & wBold))
		szFormatting[cbReturn++] = chCtlBold;
	if ((wNextFormat & wItalic) != (wCurFormat & wItalic))
		szFormatting[cbReturn++] = chCtlItalic;
	if ((wNextFormat & wUnderline) != (wCurFormat & wUnderline))
		szFormatting[cbReturn++] = chCtlUnderline;
	if ((wNextFormat & wFixedPitch) != (wCurFormat & wFixedPitch))
		szFormatting[cbReturn++] = chCtlFixedPitchFont;
	if ((wNextFormat & wSymbol) != (wCurFormat & wSymbol))
		szFormatting[cbReturn++] = chCtlSymbol;
	if (((wNextFormat & wForeground) != (wCurFormat & wForeground)) ||
		((wNextFormat & 0x00F0) != (wCurFormat & 0x000F)) ||	// has foreground color changed?
		((wNextFormat & 0x000F) != (wCurFormat & 0x000F)))		// has background color changed?
	{
		szFormatting[cbReturn++] = chCtlColor;
		if (wNextFormat & wForeground)
		{
			byteForeground = ((wNextFormat >> 4) & 0x000F) + 16;
			szFormatting[cbReturn++] = '0' + (byteForeground/10);
			szFormatting[cbReturn++] = '0' + (byteForeground%10);
				if (((wNextFormat & wBackground) != (wCurFormat & wBackground)) ||
				((wNextFormat & 0x000F) != (wCurFormat & 0x000F)))
			{
				if (wNextFormat & wBackground)
				{
					szFormatting[cbReturn++] = g_chComma;
					byteBackground = (wNextFormat & 0x000F) + 16;
					szFormatting[cbReturn++] = '0' + (byteBackground/10);
					szFormatting[cbReturn++] = '0' + (byteBackground%10);
				}
			}
		}
		// else no more foreground formatting
		else
		{
			if (chFirstFormattedChar >= '0' && chFirstFormattedChar <= '9')
			{
				// hack to fix the ^k33 (for example) case (^k33 becomes ^k0133)
				//szFormatting[cbReturn++] = ',';  // used to be ^k,0033  but that looks bad in mirc
				szFormatting[cbReturn++] = '0';
				szFormatting[cbReturn++] = '1';
			}
		}
	}

	szFormatting[cbReturn] = '\0';
	return cbReturn;
}


char* SzControlFull(const char *szInput, CDWordArray *prgdwFormatting)
{
	ASSERT(szInput);
	ASSERT(prgdwFormatting);

	// cSections = number of chunks with given format
	int		i, cbReturn, cSections = prgdwFormatting->GetSize();

	// each section potentially increases the size of the string by ^b^i^u^f^cWX,YX = 10 bytes
	char	*szReturn = new char[cbReturn = (strlen(szInput) + MAX_FORMATTINGPERBYTE * cSections + 1)];

	if (!szReturn)
		return NULL;	// OOM condition

	DWORD	dwElement;
	WORD	wCurFormat = 0, wNextFormat, wCurOffset = 0, wNextOffset;

	ZeroMemory(szReturn, cbReturn);
	cbReturn = 0;

	for (i = 0; i < cSections; i++)
	{
		dwElement = prgdwFormatting->GetAt(i);
		wNextFormat = LOWORD(dwElement);
		wNextOffset = HIWORD(dwElement);
		strncpy(szReturn+cbReturn, szInput+wCurOffset, wNextOffset-wCurOffset);
		cbReturn += (wNextOffset-wCurOffset);
		cbReturn += nFillFormatting(szReturn+cbReturn, wCurFormat, wNextFormat, *(szInput+wNextOffset));
		if ((wNextFormat & wLink) != (wCurFormat & wLink))
			szReturn[cbReturn++] = chCtlLink;		// i don't think this is ever used...
		wCurFormat = wNextFormat;
		wCurOffset = wNextOffset;
	}
	strcpy(szReturn+cbReturn, szInput+wCurOffset);

	return szReturn;
}


CDWordArray* PRGDWGetFormatting(CRichEditCtrl *pRichEdit, CFont *pDefaultFont, COLORREF crDefaultColor)
{
	// Formatting DWORD constitution:
	// OOOOEEFB	where	- OOOO is offset of first character
	//					- EE are effects (bold, italic, etc...)
	//					- F is foreground color
	//					- B is background color
	CDWordArray *prgdwFormatting = NULL;
	SHORT		nLength, nIndex, nCharWidth;
	CHARFORMAT	cf;
	BOOL		bBold, bItalic, bUnderline, bSymbol, bSymbolTmp, bFixedPitch, bFixedPitchTmp, bFontChanged;
	COLORREF	cr;
	WORD		wFormat = 0;
	LOGFONT		lfDefaultFont;
	LPTSTR		szString, szTmp, szTmp2;

	ASSERT(pRichEdit);
	ASSERT(pDefaultFont);

	pDefaultFont->GetLogFont(&lfDefaultFont);

	nLength = (short) pRichEdit->GetTextLength();
	szTmp = szString = new TCHAR[nLength+1];
	pRichEdit->GetWindowText(szString, nLength+1);

	bBold		= lfDefaultFont.lfWeight >= 700;
	bItalic		= lfDefaultFont.lfItalic;
	bUnderline	= lfDefaultFont.lfUnderline;
	bFixedPitch	= FFixedPitchFont(lfDefaultFont.lfFaceName) > -1;	// REGISB: lfDefaultFont.lfPitchAndFamily & FIXED_PITCH; is buggy solution (always 0x22) 
	bSymbol		= FSymbolFont(lfDefaultFont.lfFaceName) > -1;
	cr			= crDefaultColor;

	if (bBold)
		wFormat |= wBold;
	if (bItalic)
		wFormat |= wItalic;
	if (bUnderline)
		wFormat |= wUnderline;
	if (bFixedPitch)
		wFormat |= wFixedPitch;
	if (bSymbol)
		wFormat |= wSymbol;

	szTmp = CharNext(szString);

	for (nIndex = szTmp-szString, nCharWidth = nIndex; 
		 nCharWidth > 0; 
		 szTmp2 = CharNext(szTmp), nCharWidth = szTmp2-szTmp, nIndex += nCharWidth, szTmp = szTmp2)
	{
		pRichEdit->SetSel(nIndex, nIndex);
		pRichEdit->GetSelectionCharFormat(cf);
		bFontChanged = FALSE;
		if ((cf.dwEffects & CFE_BOLD && !bBold) || (!(cf.dwEffects & CFE_BOLD) && bBold))
		{
			bFontChanged = TRUE;
			bBold = cf.dwEffects & CFE_BOLD;
			if (bBold)
				wFormat |= wBold;
			else
				wFormat &= ~wBold;
		}
		if ((cf.dwEffects & CFE_ITALIC && !bItalic) || (!(cf.dwEffects & CFE_ITALIC) && bItalic))
		{
			bFontChanged = TRUE;
			bItalic = cf.dwEffects & CFE_ITALIC;
			if (bItalic)
				wFormat |= wItalic;
			else
				wFormat &= ~wItalic;
		}
		if ((cf.dwEffects & CFE_UNDERLINE && !bUnderline) || (!(cf.dwEffects & CFE_UNDERLINE) && bUnderline))
		{
			bFontChanged = TRUE;
			bUnderline = cf.dwEffects & CFE_UNDERLINE;
			if (bUnderline)
				wFormat |= wUnderline;
			else
				wFormat &= ~wUnderline;
		}
		bFixedPitchTmp = FFixedPitchFont(cf.szFaceName) > -1;
		//REGISB: using cf.bPitchAndFamily & FIXED_PITCH is boggus
		if ((bFixedPitchTmp && !bFixedPitch) || (!bFixedPitchTmp && bFixedPitch))
		{
			bFontChanged = TRUE;
			bFixedPitch = bFixedPitchTmp;
			if (bFixedPitch)
				wFormat |= wFixedPitch;
			else
				wFormat &= ~wFixedPitch;
		}
		bSymbolTmp = FSymbolFont(cf.szFaceName) > -1;
		if ((bSymbolTmp && !bSymbol) || (!bSymbolTmp && bSymbol))
		{
			bFontChanged = TRUE;
			bSymbol = bSymbolTmp;
			if (bSymbol)
				wFormat |= wSymbol;
			else
				wFormat &= ~wSymbol;
		}
		if (cr != cf.crTextColor)
		{
			bFontChanged = TRUE;
			cr = cf.crTextColor;
			wFormat &= 0xFF0F;
			if (cr != crDefaultColor)
			{
				wFormat |= wForeground;
				wFormat |= (GetColorCode(cr) << 4);
			}
			else
				wFormat &= ~wForeground;
		}
		if (bFontChanged)
		{
			if (!prgdwFormatting)
			{
				prgdwFormatting = new CDWordArray;
				if (!prgdwFormatting)
				{
					if (szString)
						delete [] szString;
					return NULL;
				}
			}
			prgdwFormatting->Add(MAKELONG(wFormat, nIndex-nCharWidth));
		}
	}

	if (szString)
		delete [] szString;

	return prgdwFormatting;
}


CSize GetFormattedTextExtent(CDC *pdc, LPCTSTR szInput, DWORD cbLen, CDWordArray *prgdwFormatting)
{
	CSize			size(0, 0), sizeTmp(0, 0);
	//int			iMaxAscent = 0, iMaxDescent = 0;
	//TEXTMETRIC	tm;
	static short	nFixedPitchIndex = nGetSpecialFontIndex(TRUE);
	static short	nSymbolIndex = nGetSpecialFontIndex(FALSE);

	if (!pdc || !szInput)
	{
		ASSERT(FALSE);
		return size;
	}

	if (!bSizorPresent(prgdwFormatting))
		return pdc->GetTextExtent(szInput, cbLen ? cbLen : _tcslen(szInput));

	// here comes the tough part!

	// characteristics that affect the extent size are: wBold, wItalic, wUnderline, wFixedPitch, wSymbol and wBackground
	CFont*	pOldFont = pdc->GetCurrentFont();
	CFont	fontTmp;
	LOGFONT logFontOld, logFontTmp;
	LPCTSTR	szInputTmp = szInput;
	BOOL	bTransparency = FALSE;
	TCHAR*	szTransparent = NULL;

	if (!pOldFont)
	{
		ASSERT(FALSE);
		return size;
	}

	pOldFont->GetLogFont(&logFontOld);
	pOldFont->GetLogFont(&logFontTmp);

	ASSERT(prgdwFormatting);

	int iUpper = prgdwFormatting->GetUpperBound();

	ASSERT(iUpper >= 0);
	
	WORD wFormat, wOffset, wStart = 0, wCurLen = 0, wNextLen = 0, wMaxLen = (WORD) (cbLen ? cbLen : _tcslen(szInput));

	for (int i = 0; i <= iUpper; i++)
	{
		wFormat = LOWORD(prgdwFormatting->GetAt(i));
		wOffset = HIWORD(prgdwFormatting->GetAt(i));

		fontTmp.CreateFontIndirect(&logFontTmp);
		pdc->SelectObject(&fontTmp);

		if (wOffset > wMaxLen)
			wNextLen = wMaxLen;
		else
			wNextLen = wOffset;

		if (bTransparency)
		{
			if (!szTransparent)
			{
				if (!(szTransparent = new TCHAR[wMaxLen+1]))
					return size;
				FillMemory(szTransparent, wMaxLen, g_chTransparent);
				szTransparent[wMaxLen] = g_chEOS;
			}
			sizeTmp = pdc->GetTextExtent(szTransparent, wNextLen - wCurLen);
		}
		else
			sizeTmp = pdc->GetTextExtent(szInputTmp, wNextLen - wCurLen);

		pdc->SelectObject(pOldFont);
		fontTmp.DeleteObject();

		if (sizeTmp.cx)
		{
			size.cx += sizeTmp.cx;
			size.cy = max(size.cy, sizeTmp.cy);

			//pdc->GetTextMetrics(&tm);
			//iMaxAscent = max(iMaxAscent, tm.tmAscent);
			//iMaxDescent = max(iMaxDescent, tm.tmDescent);
		}

		szInputTmp = szInput + wOffset;

		wCurLen = wOffset;

		if (wOffset >= wMaxLen)
			break;

		if (wFormat & wBold)
			logFontTmp.lfWeight = 700;
		else
			logFontTmp.lfWeight = logFontOld.lfWeight;

		if (wFormat & wItalic)
			logFontTmp.lfItalic = TRUE;
		else
			logFontTmp.lfItalic = logFontOld.lfItalic;
		
		if (wFormat & wUnderline)
			logFontTmp.lfUnderline = TRUE;
		else
			logFontTmp.lfUnderline = logFontOld.lfUnderline;

		if (nFixedPitchIndex >= 0 && (wFormat & wFixedPitch))
		{
			_tcscpy(logFontTmp.lfFaceName, FIXEDPITCHFACENAMES[nFixedPitchIndex]);
			logFontTmp.lfPitchAndFamily |= FIXED_PITCH;
			logFontTmp.lfPitchAndFamily &= ~VARIABLE_PITCH;
		}

		if (nSymbolIndex >= 0 && (wFormat & wSymbol))
		{
			_tcscpy(logFontTmp.lfFaceName, SYMBOLFACENAMES[nSymbolIndex]);
			logFontTmp.lfPitchAndFamily &= ~FIXED_PITCH;
			logFontTmp.lfPitchAndFamily &= ~VARIABLE_PITCH;
		}

		if (!(wFormat & wFixedPitch) && !(wFormat & wSymbol))
		{
			_tcscpy(logFontTmp.lfFaceName, logFontOld.lfFaceName);
			logFontTmp.lfPitchAndFamily = logFontOld.lfPitchAndFamily;
		}

		bTransparency = ((wFormat & wForeground) &&
						 (wFormat & wBackground) &&
				         ((wFormat >> 4) & 0x000F) == (wFormat & 0x000F));
	}
	
	if (wMaxLen > wCurLen)
	{
		fontTmp.CreateFontIndirect(&logFontTmp);
		pdc->SelectObject(&fontTmp);

		if (bTransparency)
		{
			if (!szTransparent)
			{
				if (!(szTransparent = new TCHAR[wMaxLen+1]))
					return size;
				FillMemory(szTransparent, wMaxLen, g_chTransparent);
				szTransparent[wMaxLen] = g_chEOS;
			}
			sizeTmp = pdc->GetTextExtent(szTransparent, wMaxLen - wCurLen);
		}
		else
			sizeTmp = pdc->GetTextExtent(szInputTmp, wMaxLen - wCurLen);

		// pdc->GetTextMetrics(&tm);

		size.cx += sizeTmp.cx;
		size.cy = max(size.cy, sizeTmp.cy);

		//iMaxAscent = max(iMaxAscent, tm.tmAscent);
		//iMaxDescent = max(iMaxDescent, tm.tmDescent);
	}

	pdc->SelectObject(pOldFont);
	fontTmp.DeleteObject();

	if (szTransparent)
		delete [] szTransparent;

	// size.cy = iMaxAscent + iMaxDescent;
	return size;
}


BYTE GetColorCode(COLORREF cr)
{
	switch (cr)
	{
	default:
		return 1;
	case RGB(255,255,255):
		return 0;
	case RGB(0,0,128):
		return 2;
	case RGB(0,128,0):
		return 3;
	case RGB(255,0,0):
		return 4;
	case RGB(128,0,0):
		return 5;
	case RGB(128,0,128):
		return 6;
	case RGB(128,128,0):
		return 7;
	case RGB(255,255,0):
		return 8;
	case RGB(0,255,0):
		return 9;
	case RGB(0,128,128):
		return 10;
	case RGB(0,255,255):
		return 11;
	case RGB(0,0,255):
		return 12;
	case RGB(255,0,255):
		return 13;
	case RGB(128,128,128):
		return 14;
	case RGB(192,192,192):
		return 15;
	}
}


COLORREF GetRBGColor(BYTE byteCode)
{
	switch (byteCode)
	{
	default:
		return RGB(0,0,0);
	case 0:
		return RGB(255,255,255);
	case 2:
		return RGB(0,0,128);
	case 3:
		return RGB(0,128,0);
	case 4:
		return RGB(255,0,0);
	case 5:
		return RGB(128,0,0);
	case 6:
		return RGB(128,0,128);
	case 7:
		return RGB(128,128,0);
	case 8:
		return RGB(255,255,0);
	case 9:
		return RGB(0,255,0);
	case 10:
		return RGB(0,128,128);
	case 11:
		return RGB(0,255,255);
	case 12:
		return RGB(0,0,255);
	case 13:
		return RGB(255,0,255);
	case 14:
		return RGB(128,128,128);
	case 15:
		return RGB(192,192,192);
	}
}


void FreeAndNullFormatting(CDWordArray **pprgdwFormatting)
{
	if (!*pprgdwFormatting)
		return;

	(*pprgdwFormatting)->RemoveAll();
	delete *pprgdwFormatting;
	*pprgdwFormatting = NULL;
}


CDWordArray* CopyFormatting(CDWordArray *prgdwFormatting)
{
	if (!prgdwFormatting) 
		return NULL;

	CDWordArray* prgdwNewFormatting = new CDWordArray;
	if (!prgdwNewFormatting) 
		return NULL;

	int iUpper = prgdwFormatting->GetUpperBound();
	
	for (int i = 0; i <= iUpper; i++)
		prgdwNewFormatting->Add(prgdwFormatting->GetAt(i));

	return prgdwNewFormatting;
}


CDWordArray* CopyLinksFormatting(CDWordArray *prgdwFormatting)
{
	if (!prgdwFormatting) 
		return NULL;

	CDWordArray*	prgdwNewFormatting = NULL;
	int				iUpper = prgdwFormatting->GetUpperBound();
	DWORD			dwElement;
	BOOL			bInLink = FALSE;
	
	for (int i = 0; i <= iUpper; i++)
	{
		dwElement = prgdwFormatting->GetAt(i);
		if (LOWORD(dwElement) & wLink && !bInLink)
		{
			bInLink = TRUE;
			if (!prgdwNewFormatting)
			{
				prgdwNewFormatting = new CDWordArray;
				if (!prgdwNewFormatting) 
					return NULL;
			}
			prgdwNewFormatting->Add(MAKELONG(wLink, HIWORD(dwElement)));
		}
		else
		{
			if (!(LOWORD(dwElement) & wLink) && bInLink)
			{
				bInLink = FALSE;
				if (!prgdwNewFormatting)
				{
					prgdwNewFormatting = new CDWordArray;
					if (!prgdwNewFormatting) 
						return NULL;
				}
				prgdwNewFormatting->Add(MAKELONG(0, HIWORD(dwElement)));
			}
		}
	}

	return prgdwNewFormatting;
}


BOOL bFormattingsEqual(CDWordArray* prgdwFormatting1, CDWordArray *prgdwFormatting2)
{
	int iCnt1, iCnt2;

	iCnt1 = prgdwFormatting1 ? prgdwFormatting1->GetSize() : 0;
	iCnt2 = prgdwFormatting2 ? prgdwFormatting2->GetSize() : 0;

	if (iCnt1 != iCnt2)
		return FALSE;

	if (iCnt1 == 0)
		return TRUE;

	for (int i = 0; i < iCnt1; i++)
		if (prgdwFormatting1->GetAt(i) != prgdwFormatting2->GetAt(i))
			return FALSE;

	return TRUE;
}


CDWordArray* AddFormat(CDWordArray *prgdwFormatting, DWORD dwElement)
{
	if (!prgdwFormatting)
	{
		prgdwFormatting = new CDWordArray;
		if (!prgdwFormatting)
			return NULL;
	}
	prgdwFormatting->Add(dwElement);
	return prgdwFormatting;
}


CDWordArray* InsertFormat(CDWordArray *prgdwFormatting, BOOL bAddFormat, WORD wFormat, WORD wOffset)
{
	if (!prgdwFormatting || !prgdwFormatting->GetSize())
	{
		if (!prgdwFormatting)
		{
			prgdwFormatting = new CDWordArray;
			if (!prgdwFormatting)
				return NULL;
		}
		ASSERT(bAddFormat);
		prgdwFormatting->Add(MAKELONG(wFormat, wOffset));
	}
	else
	{
		// find the biggest offset behind wOffset
		WORD	wOffsetTmp, wFormatTmp;
		int		i, iUpper = prgdwFormatting->GetUpperBound();

		for (i = 0; i <= iUpper; i++)
		{
			wOffsetTmp = HIWORD(prgdwFormatting->GetAt(i));
			if (wOffsetTmp >= wOffset)
				break;
		}

		if (wOffsetTmp == wOffset)
			wFormatTmp = LOWORD(prgdwFormatting->GetAt(i));
		else
			if (i == 0)
				wFormatTmp = 0;
			else
				wFormatTmp = LOWORD(prgdwFormatting->GetAt(i-1));

		if (bAddFormat)
			wFormatTmp |= wFormat;
		else
			wFormatTmp &= ~wFormat;

		if (wOffsetTmp == wOffset)
			prgdwFormatting->SetAt(i, MAKELONG(wFormatTmp, wOffset));
		else
			prgdwFormatting->InsertAt(i, MAKELONG(wFormatTmp, wOffset));

		if (!bAddFormat)
		{
			// need to set wFormat behind index i
			for (int j = i-1; j > 0; j--)
			{
				wFormatTmp = LOWORD(prgdwFormatting->GetAt(j));
				wOffsetTmp = HIWORD(prgdwFormatting->GetAt(j));
				if (wFormatTmp & wFormat)
					break;
				else
				{
					wFormatTmp |= wFormat;
					prgdwFormatting->SetAt(j, MAKELONG(wFormatTmp, wOffsetTmp));
				}
			}
		}
	}
	return prgdwFormatting;
}


CDWordArray* CutFormattingArray(CDWordArray *prgdwFormatting, SHORT nNewStringLenth)
{
	if (!prgdwFormatting)
		return NULL;

	int		iUpper = prgdwFormatting->GetUpperBound();
	DWORD	dwElement;

	for (int i = 0; i <= iUpper; i++)
	{
		dwElement = prgdwFormatting->GetAt(i);
		if (HIWORD(dwElement) >= nNewStringLenth)
		{
			prgdwFormatting->RemoveAt(i);
			i--;
			iUpper--;
		}
	}

	if (!prgdwFormatting->GetSize())
		FreeAndNullFormatting(&prgdwFormatting);

	return prgdwFormatting;
}


CDWordArray* PullFormattingOffsets(CDWordArray *prgdwFormatting, SHORT nDeltaOffset)
{
	WORD wLatestFormat = 0;

	if (!prgdwFormatting)
		return NULL;

	if (!nDeltaOffset)
		return CopyFormatting(prgdwFormatting);

	int				iUpper = prgdwFormatting->GetUpperBound();
	DWORD			dwElement;
	CDWordArray*	prgdwNewFormatting = NULL;
	
	for (int i = 0; i <= iUpper; i++)
	{
		dwElement = prgdwFormatting->GetAt(i);
		if (HIWORD(dwElement) >= nDeltaOffset/* && (prgdwNewFormatting || LOWORD(dwElement) > 0)*/)
		{
			if (!prgdwNewFormatting)
			{
				prgdwNewFormatting = new CDWordArray;
				if (!prgdwNewFormatting)
					return NULL;
				if (wLatestFormat)
					prgdwNewFormatting->Add(MAKELONG(wLatestFormat, 0));
			}
			prgdwNewFormatting->Add(MAKELONG(LOWORD(dwElement), HIWORD(dwElement)-nDeltaOffset));
		}
		else
			wLatestFormat = LOWORD(dwElement);
	}
	
	if (!prgdwNewFormatting && wLatestFormat)
	{
		prgdwNewFormatting = new CDWordArray;
		if (!prgdwNewFormatting)
			return NULL;
		prgdwNewFormatting->Add(MAKELONG(wLatestFormat, 0));
	}

	return prgdwNewFormatting;
}


void PushFormattingOffsets(CDWordArray *prgdwFormatting, SHORT nDeltaOffset)
{
	if (!prgdwFormatting || !nDeltaOffset)
		return;

	int		iUpper = prgdwFormatting->GetUpperBound();
	DWORD	dwElement;
	
	for (int i = 0; i <= iUpper; i++)
	{
		dwElement = prgdwFormatting->GetAt(i);
		prgdwFormatting->SetAt(i, MAKELONG(LOWORD(dwElement), HIWORD(dwElement)+nDeltaOffset));
	}
}


void PushFormattingOffsetsDW(DWORD *prgdwFormatting, INT cFormats, SHORT nDeltaOffset)
{
	if (!prgdwFormatting || !cFormats || !nDeltaOffset)
		return;

	DWORD	dwElement;
	
	for (int i = 0; i < cFormats; i++)
	{
		dwElement = prgdwFormatting[i];
		prgdwFormatting[i] = MAKELONG(LOWORD(dwElement), HIWORD(dwElement)+nDeltaOffset);
	}
}


CDWordArray* IdentifyURLs(CDWordArray* prgdwFormatting, const char *szMesg)
{
	int nUrlBounds[MAX_URL_INTEXT*2];
	int	nUrlNum = MAX_URL_INTEXT, i;

	g_urlRec.HrIdentifyUrls(szMesg, nUrlBounds, &nUrlNum);
	// if HrIdentifyUrls fails because there are too many URLs, we just show the first 16 ones

	if (!nUrlNum)
		return prgdwFormatting; // we didn't find any URL

	if (!prgdwFormatting)
	{
		// No formatting other than URLs
		for (i = 0; i < nUrlNum; i++)
		{
			prgdwFormatting = AddFormat(prgdwFormatting, MAKELONG(wLink, nUrlBounds[2*i]));
			prgdwFormatting = AddFormat(prgdwFormatting, MAKELONG(0, nUrlBounds[2*i+1]));
		}
	}
	else
	{
		// Add the URL formats to the existing array of DWORDs.
		for (i = 0; i < nUrlNum; i++)
		{
			prgdwFormatting = InsertFormat(prgdwFormatting, TRUE,  wLink, nUrlBounds[2*i]);
			prgdwFormatting = InsertFormat(prgdwFormatting, FALSE, wLink, nUrlBounds[2*i+1]);
		}
	}

	return prgdwFormatting;
}

CDWordArray* 
MarkHotLinks(
CDWordArray* prgdwFormatting, 
char *szMesg,					// This string is modified!
char cIdentifier)
{
	BOOL  bHotlinkOn = FALSE;
	LPSTR pszDest = NULL;
	LPSTR pszSrc = szMesg;
	while (*pszSrc) {
		if (*pszSrc == cIdentifier) {
			if (pszDest == NULL) {
				pszDest = pszSrc;
			}
			bHotlinkOn = !bHotlinkOn;
			if (prgdwFormatting != NULL) {
				prgdwFormatting = InsertFormat (prgdwFormatting, 
					bHotlinkOn, wLink, (WORD)(pszDest - szMesg));
			}
			else {
				prgdwFormatting = AddFormat (prgdwFormatting, 
					MAKELONG(bHotlinkOn ? wLink : 0, (WORD)(pszDest - szMesg)));
			}
			pszSrc++;
		}
		else if (pszDest != NULL) {
			if (IsDBCSLeadByte (*pszSrc)) {
				*(pszDest++) = *(pszSrc++);
			}
			*(pszDest++) = *(pszSrc++);
		}
		else {
			pszSrc = CharNext (pszSrc);
		}
	}

	if (pszDest != NULL) {
		if (bHotlinkOn) {
			prgdwFormatting = InsertFormat (prgdwFormatting, 
					FALSE, wLink, (WORD)(pszDest - szMesg));
		}
		*pszDest = '\0';
	}

	return prgdwFormatting;
}


BOOL bSizorPresent(CDWordArray *prgdwFormatting)
{
	if (!prgdwFormatting)
		return FALSE;

	WORD	wFormat;
	int		iUpper = prgdwFormatting->GetUpperBound();

	for (int i = 0; i <= iUpper; i++)
	{
		wFormat = LOWORD(prgdwFormatting->GetAt(i));
		if (
			(wFormat & wBold) ||
			(wFormat & wItalic) ||
			(wFormat & wUnderline) ||
			(wFormat & wFixedPitch) ||
			(wFormat & wSymbol) ||
			(wFormat & wBackground)
		   )
			return TRUE;
	}

	return FALSE;
}


BOOL bURLPresent(CDWordArray *prgdwFormatting)
{
	if (!prgdwFormatting)
		return FALSE;

	int	iUpper = prgdwFormatting->GetUpperBound();

	for (int i = 0; i <= iUpper; i++)
		if (LOWORD(prgdwFormatting->GetAt(i)) & wLink)
			return TRUE;

	return FALSE;
}


BOOL FLaunchBrowser(LPCTSTR cszURL)
{
	return(g_urlRec.bLaunchUrl(GetFrame()->GetSafeHwnd(), cszURL, theApp.m_bEmbedded));
}


short FFixedPitchFont(LPCTSTR cszFaceName)
{
	for (short iFontIndex = 0; iFontIndex < FIXEDPITCHNUMBER; iFontIndex++)
		if (0 == _tcsicmp(cszFaceName, FIXEDPITCHFACENAMES[iFontIndex]))
			return iFontIndex;
	return -1;
}


short FSymbolFont(LPCTSTR cszFaceName)
{
	for (short iFontIndex = 0; iFontIndex < SYMBOLNUMBER; iFontIndex++)
		if (0 == _tcsicmp(cszFaceName, SYMBOLFACENAMES[iFontIndex]))
			return iFontIndex;
	return -1;
}


int CALLBACK EnumFontFamExProc(ENUMLOGFONTEX *lpelfe, NEWTEXTMETRICEX *lpntme, int FontType, LPARAM lParam)
{
	return 0;	// As soon as we find an existing font we stop the enumeration
}


short nGetSpecialFontIndex(BOOL bFixedPitchFont)
{
	HDC				hdc;
	LOGFONT			lf;
	const char* const*	szFace = bFixedPitchFont ? FIXEDPITCHFACENAMES : SYMBOLFACENAMES;
	short			iRet, i, iIndexMax = bFixedPitchFont ? FIXEDPITCHNUMBER-1 : SYMBOLNUMBER-1;

	ZeroMemory(&lf, sizeof(LOGFONT));
	lf.lfWeight = FW_NORMAL;
	lf.lfCharSet = DEFAULT_CHARSET;

	hdc = GetDC(NULL);
	if (!hdc) return -1;

	for (i = 0; i <= iIndexMax; i++)
	{
		_tcscpy(lf.lfFaceName, szFace[i]);
		if (0 == (iRet = EnumFontFamiliesEx(hdc,					// handle to device context
											&lf,					// pointer to logical font information
											(FONTENUMPROC) EnumFontFamExProc,	// pointer to callback function
											0L,						// application-supplied data
											0L						// reserved; must be zero
										   )))
			break;
	}

	ReleaseDC(NULL, hdc);

	return (iRet > 0) ? -1 : i;
}


//=--------------------------------------------------------------------------=
// bLOGFONTToCHARFORMAT
//=--------------------------------------------------------------------------=
// converts a LOGFONT structure + COLORREF value into a CHARFORMAT structure
//
// Parameters:
//    LOGFONT*			- [in]  LOGFONT structure to convert
//	  COLORREF			- [in]  COLORREF to convert
//    DWORD				- [in]  which fields in the CHARFORMAT structure are we interested in?
//    CHARFORMAT		- [out] the resulting CHARFORMAT structure
//
// Output:
//    BOOL				- TRUE if the function succeeds
//
// Notes:
//	  dwMask can be any combination of CFM_FACE, CFM_SIZE, CFM_OFFSET, CFM_COLOR, CFM_ITALIC, CFM_STRIKEOUT, CFM_UNDERLINE
//    if dwMask is 0L then all the CHARFORMAT fields are filled
//
BOOL bLOGFONTToCHARFORMAT(LOGFONT *pLogFont, COLORREF crColor, DWORD dwMask, CHARFORMAT *pCharFormat)
{
	ASSERT(pLogFont);
	ASSERT(pCharFormat);

	if (!pLogFont || !pCharFormat)
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	ZeroMemory(pCharFormat, sizeof(CHARFORMAT));

	pCharFormat->cbSize = sizeof(CHARFORMAT);

	pCharFormat->dwMask = (dwMask) ? dwMask : (CFM_FACE | CFM_SIZE | CFM_OFFSET | CFM_COLOR |
											   CFM_ITALIC | CFM_STRIKEOUT | CFM_UNDERLINE | CFM_CHARSET);

	if ((pLogFont->lfWeight) && ((dwMask & CFM_BOLD) || !dwMask))
		pCharFormat->dwMask |= CFM_BOLD;

	if ((pLogFont->lfWeight >= 700) && (pCharFormat->dwMask & CFM_BOLD)) 
		pCharFormat->dwEffects |= CFE_BOLD;

	if ((pLogFont->lfItalic) && (pCharFormat->dwMask & CFM_ITALIC))
		pCharFormat->dwEffects |= CFE_ITALIC;

	if ((pLogFont->lfUnderline) && (pCharFormat->dwMask & CFM_UNDERLINE))
		pCharFormat->dwEffects |= CFE_UNDERLINE;

	if ((pLogFont->lfStrikeOut) && (pCharFormat->dwMask & CFM_STRIKEOUT))
		pCharFormat->dwEffects |= CFE_STRIKEOUT;

	if (pCharFormat->dwMask & CFM_SIZE)
	{
		// convert from pixels to twips
		HDC hDC = ::GetDC(NULL);
		if (hDC)
		{
			pCharFormat->yHeight = (abs(pLogFont->lfHeight) * 1440) / GetDeviceCaps(hDC, LOGPIXELSY);
			::ReleaseDC(NULL, hDC);
		}
		else
		{
			pCharFormat->yHeight = 1000; // TBU
		}
	}

	if (pCharFormat->dwMask & CFM_COLOR)
		pCharFormat->crTextColor = crColor;

	pCharFormat->bCharSet = pLogFont->lfCharSet;
	pCharFormat->bPitchAndFamily = pLogFont->lfPitchAndFamily;

	if (pCharFormat->dwMask & CFM_FACE)
		_tcscpy(pCharFormat->szFaceName, pLogFont->lfFaceName);

	return TRUE;
}

void MatchFont(LOGFONT &lf)
{
	HDC hDC = ::GetDC(NULL);
	if (hDC)
	{
		HFONT	hFont, hOldFont;
		CHAR	szPhysFaceName[LF_FACESIZE];

		hFont = ::CreateFontIndirect(&lf);
		hOldFont = (HFONT) ::SelectObject(hDC, hFont);

		if (GetTextFace(hDC, LF_FACESIZE, szPhysFaceName))
			strcpy(lf.lfFaceName, szPhysFaceName);

		if (lf.lfCharSet != GetTextCharset(hDC))
		{
			lf.lfFaceName[0] = '\0';
			::DeleteObject(hFont);
			hFont = ::CreateFontIndirect(&lf);
			::SelectObject(hDC, hFont);
			if (GetTextFace(hDC, LF_FACESIZE, szPhysFaceName))
				strcpy(lf.lfFaceName, szPhysFaceName);
		}

		::SelectObject(hDC, hOldFont);
		::DeleteObject(hFont);

		::ReleaseDC(NULL, hDC);
	} 
}
