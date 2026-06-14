/////////////////////////////////////////////////////////////////////////////
//                                                                         //
//            INTL.C                                                       //
//                                                                         //
//            Copyright(c) Microsoft Corp., 1996                           //
//                                                                         //
//     This file contains routines that are specific to double byte or    //
//     characterset conversion.                                            //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

//#include "all.h"
#include "windows.h"
#include "intl.h"

#define PUBLIC
#define GLOBALDEF

// for now, just use GetTextExtentPoint32 instead of IntlGetTextExtentPoint -djk
// REGISB replaced on 09/30/97 #define IntlGetTextExtentPoint(pMime, hdc, start, len, lpSize)   GetTextExtentPoint32(hdc, start, len, lpSize)

#define FEATURE_INTL

/* if FEATURE_INTL is off, dont compile this file */
#ifdef  FEATURE_INTL

#include "tchar.h"
#include "inetreg.h"
//#include "htaccess.h"

#define MAX_CHARSET_NAMESTR   64

// we can have autodetect function for other lang than japanese 
#define RES_STRING_ENCODE_AUTODETECT  RES_STRING_ENCODE_CP932_AUTO

#if 0
// Used for window property to remember the font created
const TCHAR c_szPropDlgFont[] = TEXT("DefaultDlgFont");

// Used for the value name of ISO639 patch
const TCHAR c_szLangPatch[] = TEXT("LangPatch");
const TCHAR c_szCharset[] =  TEXT("charset");

// the default table for iso639 patch
const struct {
    char *c_szWrong;
    char *c_szRight;
} iso639patch[] = {{"jp", "ja", }, {"ch", "zh", }, {"he", "iw",},};
#endif

// Following definitions are just to make it human readable...
//
#define fDoubleByte	TRUE
#define fSingleByte	FALSE

#define MAX_SABBREVLANGNAME 8

// returns TRUE if the given pointer is anything breakable 
// for FarEast codepage.
//
// REGISB 10/14/97, added \t, \r, \n as breakable characters
#define IsBreakableChar(pMime, pchar) \
            (*(pchar) == _TEXT(' ') || \
            *(pchar) == _TEXT('\t') || \
            *(pchar) == _TEXT('\r') || \
            *(pchar) == _TEXT('\n') || \
            IsDBCSLeadByteEx(pMime->iCp, *(pchar)) || \
            IsWrapUpChar(pMime, fSingleByte, pchar)  || \
            IsWrapDownChar(pMime, fSingleByte, pchar))

// returns # of bytes of a char based on the leadbyte range of 
// the given codepage. it assumes the given byte is either of a 
// valid leadbyte or a single byte character.
//
#define BytesofChar(pMime, ch)	(IsDBCSLeadByteEx((pMime)->iCp, (ch))?(2):(1))


#if 0
//
// IsDBCSLeadByteDoc

//
//  Determine whether the character is a DBCS lead byte for the
//  specified document.
//
//  IN: pdoc  - www document
//      ch    - character to test
//

BOOL IsDBCSLeadByteDoc(struct _www *pdoc, BYTE ch)
{
    return IsDBCSLeadByteEx(GETMIMECP(pdoc), ch);
}
#endif


// IsStringBreakable
//
// IN: pMime  - PSCRIPTINFO describing code page
//     lpsz   - a pointer to the middle of a string to see if it's breakable.
//     lpszSt - a top of string.
//     len    - length of the entire string.
//
// returns TRUE If the given pointer points to the left of breakable 
// characters. 
// 
BOOL IsStringBreakable(PSCRIPTINFO pMime, LPCTSTR lpsz, LPCTSTR lpszSt, int len)
{
	if (lpsz < lpszSt+len && *lpsz)
	{
        lpsz += BytesofChar(pMime, *lpsz);
		if (lpsz < lpszSt+len)
		{
            if (IsBreakableChar(pMime, lpsz))
				return TRUE;
		}
	}
	return FALSE;
}

// FindBreakableCharForFE
//
// This tries to find any double byte, single byte wrap-up/down char
// for the given codepage and returns the pointer of it.
// Also does just a part of punctuation.
//							 d// IN: pMime     - MIMECSET describing code page
//     lpszStart - pointer to the top of string where we see if breakable.
//     textLen   - the length of the string given.
//
// OUT: pbWrapupAtTop - place that we return TRUE if we break at the top of
//                      string and found wrap-up character(s).
//                      this is used in caller so it can distinguish the case
//                      we have to leave punctuation chars at the end of
//                      avobe line.       
//
// RETURN: a pointer to a breakable character after adjusting wrap-up 
//         punctuation
//
LPTSTR FindBreakableCharForFE(PSCRIPTINFO pMime, LPTSTR lpszStart, int textLen, BOOL *pbWrapupAtTop)
{
	LPTSTR lpsz = lpszStart;
	LPTSTR pNext;
	int    len = textLen;
	int    iWrapupByte = 0;

	*pbWrapupAtTop =  TRUE;
	while (len--)
	{
        if (IsWrapUpChar(pMime, fDoubleByte, lpsz))
		{
			iWrapupByte = 2;
			break;
		}
        else if (IsWrapUpChar(pMime, fSingleByte, lpsz) || *lpsz == _TEXT(' '))
		{
			iWrapupByte = 1;
			break;
		}

		*pbWrapupAtTop = FALSE;

        if (IsWrapDownChar(pMime, fSingleByte, lpsz) || IsDBCSLeadByteEx(pMime->iCp, *lpsz))
			break;

		lpsz++;
	}

	if (len > 0)
	{
		// if we've found a wrap-up character for the codepage, it's possible
		// the next of the character is a wrap-up character too. In the case
		// we take the next one to return.
		//
		if (iWrapupByte > 0)
		{
			pNext = lpsz+iWrapupByte;
			// make sure we don't go beyond the end of text
			if (pNext > lpszStart+textLen-1)
				goto Return_lpsz;

            if (IsWrapUpChar(pMime, fDoubleByte, pNext)
               || IsWrapUpChar(pMime, fSingleByte, pNext))
				lpsz = pNext;
		}
Return_lpsz:
		return lpsz;
	}
	else
		return NULL;
}

// Perf: Should make leadbyte range table instead of using NLS
//---------------------------------------------------------------------------
//
// CharNextEx
// works just as if it is CharNext but takes codepage to distinguish leadbyte 
//
//---------------------------------------------------------------------------
LPTSTR CharNextEx(int codepage, LPCTSTR lpsz)
{
    if (IsDBCSLeadByteEx(codepage, *lpsz))
        return (LPTSTR)lpsz+2;
    else
        return (LPTSTR)lpsz+1;
}

//---------------------------------------------------------------------------
//
// CharPrevEx
// works just as if it is CharPrev but takes codepage to distinguish leadbyte 
//
//---------------------------------------------------------------------------
BOOL IsTrailByte(int codepage, LPCTSTR pszSt, LPCTSTR pszCur)
{
    LPCTSTR psz = pszCur;

    // if the given pointer is at the top of string, at least it's not a trail
    // byte.
    //
    if (psz <= pszSt) return FALSE;

    while (psz > pszSt)
    {
        psz--;
        if (!IsDBCSLeadByteEx(codepage, *psz))
        {
            // This is either a trail byte of double byte char
            // or a single byte character we've first seen.
            // Thus, the next pointer must be at either of a leadbyte 
            // or pszCur itself.
 
            psz++; 
            break; 
        } 
    }
    // Now psz can point to:
    //     1) a leadbyte of double byte character.
    //     2) pszSt
    //     3) pszCur
    //
    // if psz == pszSt, psz should point to a valid double byte char. 
    //                  because we didn't hit the above if statement.
    //
    // if psz == pszCur, the *(pszCur-1) was non lead byte so pszCur can't
    //                   be a trail byte.
    //
    // Thus, we can see pszCur as trail byte pointer if the distance from
    // psz is not DBCS boundary that is 2.
    // 
    return (pszCur-psz) & 1;
}

LPTSTR CharPrevEx(int codepage, LPCTSTR lpszSt, LPCTSTR lpsz)
{

    if (lpsz <= lpszSt) return (LPTSTR)lpsz;

    return IsTrailByte(codepage, lpszSt, lpsz-1) ? (LPTSTR)lpsz-2 : (LPTSTR)lpsz-1;
}

// AdjustPunctuation
//
// Adjusts *pbreak if we're on a FarEast punctuation character.
// Assumes ptext[*pbreak] is a character that we're going to break.
//
// Return: # of bytes adjusted.
//
int AdjustPunctuation(PSCRIPTINFO pMime, LPCTSTR pSt, LPCTSTR ptext, int *pbreak, int len)
{
	int    iAdj = 0;
	LPTSTR pNext;
	int    nBytesofBreakChar;

//	XX_Assert(((pSt) && (ptext)), ("AdjustPunctuation():NULL pointer in params"));

	// If the given char points to the one of those wrap-down characters,
	// we want to break on the character right before it so that
	// we can put it on the next line.
	// ex)"DDDDSSSSDDD[" <- we don't want to keep this brace at the end of line.
	//
    if (IsWrapDownChar(pMime, fDoubleByte, ptext)
       ||IsWrapDownChar(pMime, fSingleByte, ptext))
	{
        iAdj = IsTrailByte(pMime->iCp, pSt, ptext-1)? (-2) : (-1);
	}
	else 
	{
		// If the given char points to the left of wrap-up characters, 
		// we want to break right after wrap-up characters, so we can 
		// leave it on the previous chunk.
		//
		// ex) we don't want to have this brace at 
		//     the head of line like this
		//                   "...DDSDSDSDB <- ptext points the blank.
		// wrap-up char    -> ]SSDDSSSDDD"
		//
		pNext = NULL;
        nBytesofBreakChar = BytesofChar(pMime, *ptext);

		// ok, see if the next of break char is those wrap-up types.
		// a space can be handled as a 'wrap-up' character.
		//
        if (IsWrapUpChar(pMime, fDoubleByte, ptext+nBytesofBreakChar) ||
            IsWrapUpChar(pMime, fSingleByte, ptext+nBytesofBreakChar) ||
			*(ptext+nBytesofBreakChar) == _TEXT(' '))
		{
			// Yes it is. Make pNext point to the wrap-up. 
			//
			pNext = (LPTSTR)ptext+nBytesofBreakChar;
            nBytesofBreakChar = BytesofChar(pMime, *pNext);
		}

		// we have to see if the wrap-up characters are nested.
		// This is just such like nested right braces or nested '!' 
		// at the end of line.
		// 
		if (pNext != NULL)
		{
            if (IsWrapUpChar(pMime, fDoubleByte, pNext+nBytesofBreakChar)
                ||IsWrapUpChar(pMime, fSingleByte, pNext+nBytesofBreakChar))
				pNext +=nBytesofBreakChar;

			iAdj = pNext-ptext;
		}
	}

	// Make sure that we do not go beyond text boundary.
	//
	if (pSt <= (ptext+iAdj) && (ptext+iAdj) <= (pSt+len-1))
	{
		if (pbreak)
			*pbreak += iAdj;
		return iAdj;
	}
	else
		return 0;
}


BOOL bSizorPresent(DWORD *prgdwFormatting, int cFormats)
{
	WORD	wFormat;
	int		i;

	if (!prgdwFormatting || !cFormats)
		return FALSE;

	for (i = 0; i < cFormats; i++)
	{
		wFormat = LOWORD(prgdwFormatting[i]);
		if (
			(wFormat & wBold) ||
			(wFormat & wItalic) ||
			(wFormat & wUnderline) ||
			(wFormat & wFixedPitch) ||
			(wFormat & wSymbol)
		   )
			return TRUE;
	}
	return FALSE;
}


// REGISB added 09/30/97 in order to use the already existing GetFormattedTextExtent function
// GetFormattedTextExtent(CDC *pdc, LPCTSTR szInput, DWORD cbLen, CDWordArray *prgdwFormatting);
BOOL IntlGetTextExtentPoint(PSCRIPTINFO pMime, HDC hdc, LPCTSTR szInput, int cbLen, DWORD *prgdwFormatting, int cFormats, LPCTSTR szFixedPitchName, LPCTSTR szSymbolName, LPSIZE lpSize)
{
	SIZE	size = {0, 0}, sizeTmp = {0, 0};
	WORD	wFormat, wOffset, wStart = 0, wCurLen = 0, wNextLen = 0, wMaxLen = (WORD) (cbLen ? cbLen : _tcslen(szInput));
	HFONT	hOldFont = NULL, hFontTmp = NULL;
	LOGFONT	logFontOld, logFontTmp;
	LPCTSTR	szInputTmp = szInput;
	int		i;

	if (!hdc || !szInput || !lpSize)
		return FALSE;

	if (!bSizorPresent(prgdwFormatting, cFormats))
		return GetTextExtentPoint32(hdc, szInput, cbLen, lpSize);

	// here comes the tough part!

	// characteristics that affect the extent size are: wBold, wItalic, wUnderline, wFixedPitch and wSymbol
	
	hOldFont = GetCurrentObject(hdc, OBJ_FONT);
	if (!hOldFont)
		return FALSE;

	if (!GetObject(hOldFont, sizeof(LOGFONT), &logFontOld))
		return FALSE;
	if (!GetObject(hOldFont, sizeof(LOGFONT), &logFontTmp))
		return FALSE;

	for (i = 0; i < cFormats; i++)
	{
		wFormat = LOWORD(prgdwFormatting[i]);
		wOffset = HIWORD(prgdwFormatting[i]);

		hFontTmp = CreateFontIndirect(&logFontTmp);
		SelectObject(hdc, hFontTmp);

		if (wOffset > wMaxLen)
			wNextLen = wMaxLen;
		else
			wNextLen = wOffset;

		GetTextExtentPoint32(hdc, szInputTmp, wNextLen - wCurLen, &sizeTmp);

		SelectObject(hdc, hOldFont);
		DeleteObject(hFontTmp);

		if (sizeTmp.cx)
		{
			size.cx += sizeTmp.cx;
			size.cy = max(size.cy, sizeTmp.cy);
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

		if (szFixedPitchName && (wFormat & wFixedPitch))
		{
			_tcscpy(logFontTmp.lfFaceName, szFixedPitchName);
			logFontTmp.lfPitchAndFamily |= FIXED_PITCH;
			logFontTmp.lfPitchAndFamily &= ~VARIABLE_PITCH;
		}

		if (szSymbolName && (wFormat & wSymbol))
		{
			_tcscpy(logFontTmp.lfFaceName, szSymbolName);
			logFontTmp.lfPitchAndFamily &= ~FIXED_PITCH;
			logFontTmp.lfPitchAndFamily &= ~VARIABLE_PITCH;
		}

		if (!(wFormat & wFixedPitch) && !(wFormat & wSymbol))
		{
			_tcscpy(logFontTmp.lfFaceName, logFontOld.lfFaceName);
			logFontTmp.lfPitchAndFamily = logFontOld.lfPitchAndFamily;
		}
	}
	
	if (wMaxLen > wCurLen)
	{
		hFontTmp = CreateFontIndirect(&logFontTmp);
		SelectObject(hdc, hFontTmp);

		GetTextExtentPoint32(hdc, szInputTmp, wMaxLen - wCurLen, &sizeTmp);

		size.cx += sizeTmp.cx;
		size.cy = max(size.cy, sizeTmp.cy);
	}

	SelectObject(hdc, hOldFont);
	DeleteObject(hFontTmp);

	lpSize->cx = size.cx;
	lpSize->cy = size.cy;

	return TRUE;
}


// FindSubStringForINTLThatFits
//
// This routine is INTL equivalent of FindSubStingThatFits that works 
// only for US ASCII/Western character set. This was separated from the
// original function in order to avoid additional page hit when just 
// the regular US code path is working.
// 
// This also additionally takes care of:
// 1) FarEast specific punctuation i.e, those wrap-up characters and 
//    wrap-down characters.
// 2) Character set conversion by calling IntlGetTextExtentPoint()
//    instead of GetTextExtentPointA().
// 3) Double Byte specific word break that is, to break in the middle of
//    word if the word consists of double bytes.
//
// Note that why we don't process everything in UNICODE here is because, 
//     - we have to treat single byte char and double byte char 
//       differently anyway.
//     - we can't use GetTextExtentPointW on FE win95 because of a BUG of 
//       GDI on the platform. We use ANSI version if we're on the native
//       platform. It is taken care of within myGetTextExtent().
//       It should earn more perf on the native platform too.
//

BOOL FindSubStringForINTLThatFits(
	void	*vMime,
	HDC		hdc,
	LPCTSTR	szString,	// The string that we are trying to break up
	int		cbString,	// The length ofthe string
	DWORD	*prgdwFormatting,	// String formatting
	int		cFormats,			// Number of elements in the formatting array
	LPCTSTR	szFixedPitchName,	// Name of fixed pitch font to use
	LPCTSTR	szSymbolName,		// Name of symbol font to use
	int		*pcbFit,	// Pointer to location that holds the actual length of 
						// sub string that fits, broken at a space.
	BOOL	*pbHasBlankOrAlike,
	LPSIZE	lpSize,		// Send back the size of the substring that fits in this.
	int		nMaxExtent	// The size in which we are trying to fit in this string
)
{
	PSCRIPTINFO pMime = (PSCRIPTINFO) vMime;
	int			iBack;
	int			iFront;
	int			iCurr;
	int			iTmpCurr;
	SIZE		CurrSize;
	SIZE		FrontSize;
    int			codepage = pMime->iCp;
    BOOL		bFECodePage = _IsFECodePage(codepage);
	LPTSTR		p;
	int			iByteSizeofChar; // this shows size of a char pointed in n byte.


//	XX_Assert(((pdoc) && (szString) && (pcbFit) && (pbHasBlankOrAlike)), ("FindSubStringForINTLThatFits():NULL pointer in params"));

	*pbHasBlankOrAlike = FALSE;
	if(cbString <= 0){
		return FALSE;	
	}

	iBack = cbString-1;
	iFront = 0;

	iTmpCurr = 0;

	p = (LPTSTR) &szString[iBack];
    if (bFECodePage)
	{
		// If the end of the string is a double byte char, 
		// back up 1 byte so p can point to a correct leadbyte.
		if (IsTrailByte(codepage, szString, p))
			p--;
	}

	while (TRUE)
	{

		// For FarEast, we can break without a space if a word
		// consists of double byte or we find one of those 
		// punctuation characters
		//
		if (bFECodePage)
		{
            if (IsBreakableChar(pMime, p) && *p != _TEXT('.'))
				// hack -- I added the '.' above, since for FE this was breaking on
				// the MSChat continuation characters, causing an infinite loop when
				// linesize was 1.  Eventually, do something more elegant.
				break;
		}
		else
		{
			if (*p == _TEXT(' '))
				break;
		}

		if (p == szString)
			// Gosh -- Nothing Fits since this is one big word
			return FALSE;

		p = CharPrevEx(codepage, szString, p);
	}
	iBack = p - szString;

	if((iBack + 1) > nMaxExtent){
		// Even if Every Char is of width 1 we can't fit
		iTmpCurr = nMaxExtent;

		if (bFECodePage && IsTrailByte(codepage, szString, &(szString[iTmpCurr])))
			iTmpCurr--;

		while(iTmpCurr < iBack)
		{
			if (bFECodePage)
			{
                if (IsBreakableChar(pMime, &(szString[iTmpCurr])))
					break;
			}
			else
			{
				if (szString[iTmpCurr] == _TEXT(' '))
					break;
			}
			
			// double byte is breakable char for any Fareast codepage, 
			// so let's not bother NLS here.
			iTmpCurr++;
		}
		// Hopefully we reduced the string we are measuring here
		iBack = iTmpCurr;
	}

	*pbHasBlankOrAlike = TRUE;

	if (bFECodePage)
	{
        iByteSizeofChar = BytesofChar(pMime, szString[iBack]);
	}
	else
		iByteSizeofChar = 1;

	// Check if the largest possible substring fits 
	//
    if (!(IntlGetTextExtentPoint(pMime, hdc, szString, iBack + iByteSizeofChar, prgdwFormatting, cFormats, szFixedPitchName, szSymbolName, lpSize)))
		goto ErrorExit;

	if(lpSize->cx <= nMaxExtent){
		// largest possible substring fits.
		FrontSize = *lpSize;
		iFront = iBack;
		goto Return_FoundFits;
	}
	
	// Check if the smallest possible substring will fit
	// we know that atleast one substring exists
	p = (LPTSTR) szString;
	while(TRUE)
	{
		if (bFECodePage)
		{
            if (IsBreakableChar(pMime, p))
				break;
		}
		else
		{
			if (*p == _TEXT(' '))
				break;
		}
		p = CharNextEx(codepage, p);
		if(p == &(szString[iBack]))
		{
			// Whoops --- we already know that lpSize has the 
			// measure of the one sub-string that does not fit 
			// so we set the default value for *pcbFit and return
			*pcbFit = iBack;
			return FALSE;
		}
	}
	iFront = p - szString;
	// There are two or more substrings
	
	// Now see if there's punctuation character at the break again.
	if (bFECodePage)
	{
        iByteSizeofChar = BytesofChar(pMime, szString[iFront]);
	}
	else
		iByteSizeofChar = 1;

	// Check if the smallest  substring fits
    if(!(IntlGetTextExtentPoint(pMime, hdc, szString, (iFront + iByteSizeofChar), prgdwFormatting, cFormats, szFixedPitchName, szSymbolName, lpSize)))
		goto ErrorExit;

	*pcbFit = iFront; // Filling in default value
	if(lpSize->cx > nMaxExtent){
		// smallest possible substring does not fit.
		return FALSE;
	}
	
	FrontSize = *lpSize;

	// Ok - we now do an approximate binary search on the blank spaces
	// between iFront and iBack.
	// iFront always indicates a sub-string that fits within
	// nMaxExtent and iBack always indicates a sub-string that
	// does not fit within nMaxExtent.
	// Further -- there is always atleast one integer between iFront
	// and iBack
	// We start off with iCurr = (iFront + iBack)/2 and look for a space
	// in either direction. If no space if found - we are done.
	// If a space if found -- we make a text measurement and 
	// change either iFront or iBack according as if it fits or does not.
	// When we pop out of the while loop, iFront has the index that 
	// we want and FrontSize has the measurements that we desire.

	while((iFront + iByteSizeofChar) < iBack){
		iCurr = (iFront + iBack)/2;

		// iCurr can be a trail byte of double byte char here
		if (bFECodePage)
		{
			if (IsTrailByte(codepage, szString, &(szString[iCurr])))
				iCurr++;
		}
		
		for(iTmpCurr = iCurr; iTmpCurr < iBack; iTmpCurr++)
		{
			if (bFECodePage)
			{
                if (IsStringBreakable(pMime, &(szString[iTmpCurr]), szString, cbString))
					break;

				// Here we don't have to worry about
				// double byte to increment iTmpCurr
				// 'cause we break at double byte anyway.
			}
			else
			{
				if(szString[iTmpCurr] == _TEXT(' '))
					break;
			}
		}

		if (bFECodePage)
		{
			// Here we have to process FarEast punctuation.
			//
            iByteSizeofChar = BytesofChar(pMime, szString[iTmpCurr]);
		}
		else
			iByteSizeofChar = 1;

		if (iTmpCurr < iBack)
		{
            if(!(IntlGetTextExtentPoint(pMime, hdc, szString, (iTmpCurr + iByteSizeofChar), prgdwFormatting, cFormats, szFixedPitchName, szSymbolName, &CurrSize)))
				goto ErrorExit;

			if(CurrSize.cx <= nMaxExtent){
				iFront = iTmpCurr;
				FrontSize = CurrSize;
			}else
				iBack = iTmpCurr;
		}


		if(iTmpCurr < iBack )
			continue; 

		// Keep going backward. This is to make sure
		// iTmpCurr doesn't point to the same character
		// that we broke at the previous loop.
		//
		if (bFECodePage)
			iCurr-=IsTrailByte(codepage, szString, &(szString[iCurr])-1)? 2:1;

		for(iTmpCurr = iCurr; iTmpCurr > iFront; iTmpCurr--){

			if (bFECodePage)
			{
                if (IsStringBreakable(pMime, &(szString[iTmpCurr]), szString, cbString))
					break;

				// see if the next char we'll take is 2 byte char
				if (IsTrailByte(codepage, szString, &(szString[iTmpCurr])-1))
					iTmpCurr--;
			}
			else
			{
				if(szString[iTmpCurr] == _TEXT(' '))
					break;
			}

		}  

		if (bFECodePage)
		{
            iByteSizeofChar = BytesofChar(pMime, szString[iTmpCurr]);
		}
		else
			iByteSizeofChar = 1;

		if (iTmpCurr > iFront)
		{

            if(!(IntlGetTextExtentPoint(pMime, hdc, szString, (iTmpCurr + iByteSizeofChar), prgdwFormatting, cFormats, szFixedPitchName, szSymbolName, &CurrSize)))
				goto ErrorExit;

			if(CurrSize.cx <= nMaxExtent){
				iFront = iTmpCurr;
				FrontSize = CurrSize;
			}else
				iBack = iTmpCurr;
		}

		if(iTmpCurr == iFront)
			break; // Since there are no more blanks left
	}
Return_FoundFits:
	if (bFECodePage && 
        AdjustPunctuation(pMime, szString, &(szString[iFront]), &iFront, cbString))
	{
		// if we adjusted iFront for punctuation, we have to measure
		// the length again. Here we don't have to check MaxExtent again
		// because we want to include the punctuation character at the end of
		// line.
		//
        iByteSizeofChar = BytesofChar(pMime, szString[iFront]);
        if(!(IntlGetTextExtentPoint(pMime, hdc, szString, (iFront + iByteSizeofChar), prgdwFormatting, cFormats, szFixedPitchName, szSymbolName, &FrontSize)))
			goto ErrorExit;
		
	}

	*lpSize = FrontSize;
	if (iByteSizeofChar == 1 && szString[iFront] == ' ')  // if split at space, iFront = everything up to space
		*pcbFit = iFront;
	else {
		iByteSizeofChar = bFECodePage ? BytesofChar(pMime, szString[iFront]) : 1;
		*pcbFit = iFront + iByteSizeofChar;  // djk -- added the iByteSizeofChar, since otherwise iFront points before break character
	}

	if((*pcbFit != 0) || ((*pcbFit == cbString - 1)))
		return TRUE;
	else
		return FALSE;
		

ErrorExit:
	return FALSE;	

}

// GetTextExtentOfLongestWordForINTL
//
// This routine is INTL equivalent of FindSubStingThatFits which is only for
// ANSI character set.
// 
// This routine is called when table layout us being done with the 
// measuringMinMax = TABLE_MEASURE_MIN.
// It finds the longest word in the string and measures that and sends out its size.
//
// Leave lpSize->cy unchanged!  GetTextExtentOfLongestWord assumes that
// it will remain zero.

// REGISB: looks like this function is never called.
void GetTextExtentOfLongestWordForINTL(
	PSCRIPTINFO pMime, 
	HDC			hdc, 
	LPCSTR		szString,   // The string that we are searching
	int			cbString,  // The length of the string
	DWORD		*prgdwFormatting,	// String formatting
	int			cFormats,			// Number of elements in the formatting array
	LPCTSTR		szFixedPitchName,	// Name of fixed pitch font to use
	LPCTSTR		szSymbolName,		// Name of symbol font to use
	LPSIZE		lpSize,  // The size of the longest word after measurement
	ULONG		lFlags
)
{
	long		lLongWordExtent=0;
	int			iLongNdx;
	int			iCurrNdx;
	long		lCurrSize=0;
	int			iLastNdx;
	TEXTMETRIC	tm;
	long		lMaxCharWidth;
	SIZE		tmpSize;
    int			codepage = pMime->iCp;
    BOOL		bFECodePage = _IsFECodePage(codepage);
	int			iByteSizeofChar; // this shows size of a char pointed in n byte.
	int			iAdj;
	BOOL		bTryMeasureIt;
	BOOL		bFoundBreak=FALSE;

	if(lFlags & ELEFLAG_NOBREAK)
	{
        IntlGetTextExtentPoint(pMime, hdc, szString, cbString, prgdwFormatting, cFormats, szFixedPitchName, szSymbolName, lpSize);
		return;
	}

	GetTextMetrics(hdc, &tm);
	lMaxCharWidth = tm.tmMaxCharWidth;
	
	iLastNdx = iCurrNdx = iLongNdx = 0;

	while(szString[iCurrNdx] == _TEXT(' ')) 
		iCurrNdx++;

	iLastNdx = iLongNdx = iCurrNdx;
	bTryMeasureIt = FALSE;

	while(iCurrNdx < cbString)
	{
		if (bFECodePage)
		{
            iByteSizeofChar = BytesofChar(pMime, szString[iCurrNdx]);

            if (IsBreakableChar(pMime, &(szString[iCurrNdx])))
			{
				// See if the breakable char is a wrap-up char.
				// if the char is to be wrapped-up (i.e., iAdj >0), 
				// include it in a word.
				//
                if ((iAdj = AdjustPunctuation(pMime, szString, &(szString[iCurrNdx]), NULL, cbString)) > 0)
				{
					lCurrSize += iAdj;

					// we're going to skip the same # of bytes that we adjusted.
					//
					iByteSizeofChar = iAdj;
				}

				bTryMeasureIt = TRUE;
			}
		}
		else
		{
			if(szString[iCurrNdx] == _TEXT(' '))
				bTryMeasureIt = TRUE;
			iByteSizeofChar =1;
		}

        // BUGBUG -- raymondc thinks this has the same bug that was in the
        // SBCS build, where a two-word phrase with a short first word and
        // long second word causes us to forget to measure the second word.

		if (bTryMeasureIt)
		{
			bTryMeasureIt = !bTryMeasureIt;
			if((lMaxCharWidth * lCurrSize) >  lLongWordExtent)
			{
				// It's worth measuring this word
                IntlGetTextExtentPoint(pMime,
									   hdc,
									   &(szString[iLastNdx]),
									   lCurrSize,
									   NULL,	// TO BE FIXED if used
									   0,		// TO BE FIXED if used
									   szFixedPitchName,
									   szSymbolName,
									   &tmpSize);

				if(tmpSize.cx > lLongWordExtent)
					lLongWordExtent = tmpSize.cx;
			}	
			iLastNdx = iCurrNdx + iByteSizeofChar;
			lCurrSize = 0;
			bFoundBreak = TRUE;
		}
	  
		lCurrSize+=iByteSizeofChar;
		iCurrNdx+=iByteSizeofChar;
	}

	
	if(!bFoundBreak)
		// there is no space in the word - measure the whole word
        IntlGetTextExtentPoint(pMime,
							   hdc,
							   szString,
							   cbString,
							   prgdwFormatting,
							   cFormats,
							   szFixedPitchName,
							   szSymbolName,
							   lpSize);
	else
		lpSize->cx = lLongWordExtent; 
}

/*****************************************************************************
 *
 *  Wrap Up and Wrap Down Characters (Xxx = code page number)
 *
 *  DBCS line breaking rules are as follows:
 *
 *  o   You can break at a space.
 *  o   You cannot break immediately before a "wrap up" character.
 *  o   You cannot break immediately after a "wrap down" character.
 *  o   You can otherwise break at any DBCS character.
 *
 *  A "wrap up" character is a mark of closing punctuation, like ")"
 *  or ".", and a "wrap down" character is a mark of opening punctuation,
 *  like "(" or "[".
 *
 *  All IsWrapUpXxx and IsWrapDownXxx characters take the following
 *  parameters:
 *
 *  IN: char *psz  -     a pointer to string to be examined.
 *      BOOL fDBCS -     TRUE  to test if it is DBCS wordwrap char
 *                       FALSE to test if it is SBCS wrodwrap char
 *
 *
 *  RETURNS: TRUE if psz points to a wordwrap char
 *
 *  NOTE: these functions have to be totally rewritten when we use
 *  unicode stream
 *
 *****************************************************************************/

///////////////////////////////////////////////////////////////////////////////
//
// SBCS (shouldn't be called)
//

BOOL IsWrapSbcs(BOOL fDBCS, LPCTSTR psz)
{
//    XX_DMsg(DBG_TEXT, ("IsWrapUp/DownChar: unsupported codepage!\n"));
    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////
//
// JAPAN (932)
//

// DBCS WrapDownCharacter
#define IsWrapDownCharJ(p)	(*(p) == '\201' && \
                                ((*((p)+1) >= '\145' && *((p)+1) <= '\171') && \
							  (*((p)+1) & 1)))
// SBCS WrapDownCharacter
#define IsWrapDownCharJ2(p)  ((*(p) == '\050') || (*(p) == '\074') || \
                              (*(p) == '\133') || (*(p) == '\173') || \
							 (*(p) == '\242'))
// DBCS WrapUpCharacter
#define IsWrapUpCharJ(p)     (*(p) == '\201' && \
                              ((*((p)+1) >= '\101' && *((p)+1) <= '\111') || \
                              ((*((p)+1) >= '\146' && *((p)+1) <= '\172') && \
                              !(*((p)+1) & 1))))

// SBCS WrapUpCharacter
#define IsWrapUpCharJ2(p)    ((*(p) == '\051') || (*(p) == '\054') || \
                              (*(p) == '\056') || (*(p) == '\076') || \
                              (*(p) == '\135') || (*(p) == '\175') || \
                              (*(p) == '\243'))

BOOL IsWrapDown932(BOOL fDBCS, LPCTSTR psz)
{
    return fDBCS ? IsWrapDownCharJ(psz) : IsWrapDownCharJ2(psz);
}

BOOL IsWrapUp932(BOOL fDBCS, LPCTSTR psz)
{
    return fDBCS ? IsWrapUpCharJ(psz) : IsWrapUpCharJ2(psz);
}

///////////////////////////////////////////////////////////////////////////////
//
// KOREA (949)
//

// DBCS WrapDownCharacter 0xa1ae-0xa1bc and 0xa1cc, 0xa3a4, 0xa3a8, 0xa3db, 0xa3dc and 0xa3f8.
#define IsWrapDownCharK(p) ((*(p) == '\241' && \
                            (((*((p)+1) >= '\256' && *((p)+1) <= '\274') && \
                            !(*((p)+1) & 1)) || \
                             (*((p)+1) == '\314'))) || \
                            (*(p) == '\243' && \
                             ((*((p)+1) == '\244') || (*((p)+1) == '\250') || \
                             (*((p)+1) == '\333') || (*((p)+1) == '\334') || \
                             (*((p)+1) == '\370'))))

// SBCS WrapDownCharacter
#define IsWrapDownCharK2(p)  ((*(p) == '\044') || (*(p) == '\050') || \
                              (*(p) == '\133') || (*(p) == '\134') || \
                              (*(p) == '\173'))

// DBCS WrapUpCharacter 0xa1af-0xa1bd and 0xa1c6-0xa1c9 and 0xa1cb, 0xa3a1, 0xa3a5, 0xa3a9, 0xa3ac,
// 0xa3ae, 0xa3ba, 0xa3bb, 0xa3bf, 0xa3dd, and 0xa3fd.
#define IsWrapUpCharK(p) \
((*(p) == '\241' &&  \
 (((*((p)+1) >= '\257' && *((p)+1) <= '\275') && (*((p)+1) & 1)) || \
  (*((p)+1) >= '\306' && *((p)+1) <= '\311') || (*((p)+1) == '\313'))) || \
 (*(p) == '\243' && \
  ((*((p)+1) == '\241') || (*((p)+1) == '\245') || (*((p)+1) == '\251') || \
  (*((p)+1) == '\254') || (*((p)+1) == '\256') || (*((p)+1) == '\272') || \
  (*((p)+1) == '\273') || (*((p)+1) == '\277') || (*((p)+1) == '\335') || \
  (*((p)+1) == '\375'))))

// SBCS WrapUpCharacter
#define IsWrapUpCharK2(p)        ((*(p) == '\041') || (*(p) == '\045') || \
                                 (*(p) == '\051') || (*(p) == '\054') || \
                                 (*(p) == '\056') || (*(p) == '\072') || \
                                 (*(p) == '\073') || (*(p) == '\077') || \
                                 (*(p) == '\135') || (*(p) == '\175'))

BOOL IsWrapDown949(BOOL fDBCS, LPCTSTR psz)
{
    return fDBCS ? IsWrapDownCharK(psz) : IsWrapDownCharK2(psz);
}

BOOL IsWrapUp949(BOOL fDBCS, LPCTSTR psz)
{
    return fDBCS ? IsWrapUpCharK(psz) : IsWrapUpCharK2(psz);
}

///////////////////////////////////////////////////////////////////////////////
//
// TAIWAN (950 - Traditional Chinese)
//

// DBCS WrapDownCharacter 0xa15d-0xa17d and 0xa1a1-0xa1ab
#define IsWrapDownCharT(p) (*(p) == '\241' && \
                           (((*((p)+1) >= '\135' && *((p)+1) <= '\175') || \
                             (*((p)+1) >= '\241' && *((p)+1) <= '\253')) && \
                            (*((p)+1) & 1)))
// SBCS WrapDownCharacter
#define IsWrapDownCharT2(p)  ((*(p) == '\050') || (*(p) == '\074') || \
                                                         (*(p) == '\133') || (*(p) == '\173'))
// DBCS WrapUpCharacter 0xa141-0xa149, 0xa14d-0xa154, 0xa15e-0xa17e, and 0xa1a2-0xa1ac
#define IsWrapUpCharT(p)   (*(p) == '\241' && \
                           ((*((p)+1) >= '\101' && *((p)+1) <= '\111') || \
                            (*((p)+1) >= '\115' && *((p)+1) <= '\124') || \
                            (((*((p)+1) >= '\136' && *((p)+1) <= '\176') || \
                             (*((p)+1) >= '\242' && *((p)+1) <= '\254')) && \
                             !(*((p)+1) & 1))))
// SBCS WrapUpCharacter
#define IsWrapUpCharT2(p)	((*(p) == '\051') || (*(p) == '\054') || \
							             (*(p) == '\056') || (*(p) == '\076') || \
                           (*(p) == '\135') || (*(p) == '\175'))


BOOL IsWrapDown950(BOOL fDBCS, LPCTSTR psz)
{
    return fDBCS ? IsWrapDownCharT(psz) : IsWrapDownCharT2(psz);
}

BOOL IsWrapUp950(BOOL fDBCS, LPCTSTR psz)
{
    return fDBCS ? IsWrapUpCharT(psz) : IsWrapUpCharT2(psz);
}

///////////////////////////////////////////////////////////////////////////////
//
// CHINA (936 - Simplified Chinese)
//

// DBCS WrapDownCharacter
#define IsWrapDownCharC(p) ((*(p) == '\241' && \
                            ((*((p)+1) == '\256') || (*((p)+1) == '\260') || \
                             (*((p)+1) == '\262') || (*((p)+1) == '\264') || \
                             (*((p)+1) == '\266') || (*((p)+1) == '\270') || \
                             (*((p)+1) == '\272') || (*((p)+1) == '\274') || \
                             (*((p)+1) == '\244'))) || \
                             (*(p) == '\243' && \
                             ((*((p)+1) == '\250') || (*((p)+1) == '\333') || \
                              (*((p)+1) == '\373') || (*((p)+1) == '\256'))))
// SBCS WrapDownCharacter
#define IsWrapDownCharC2(p)  ((*(p) == '\050') || (*(p) == '\133') || (*(p) == '\173'))
// DBCS WrapUpCharacterC
#define IsWrapUpCharC(p) ((*(p) == '\241' && \
          (((*((p)+1) >= '\242' && *((p)+1) <= '\255') && (*((p)+1) & 1)) || \
           (*((p)+1) == '\257') || (*((p)+1) == '\261') || \
           (*((p)+1) == '\263') || (*((p)+1) == '\265') || \
           (*((p)+1) == '\267') || (*((p)+1) == '\271') || \
           (*((p)+1) == '\273') || (*((p)+1) == '\275') || \
           (*((p)+1) == '\277') || (*((p)+1) == '\303'))) || \
                          (*(p) == '\243' && \
           ((*((p)+1) == '\241') || (*((p)+1) == '\242') || \
            (*((p)+1) == '\247') || (*((p)+1) == '\251') || \
            (*((p)+1) == '\254') || (*((p)+1) == '\256') || \
            (*((p)+1) == '\272') || (*((p)+1) == '\273') || \
            (*((p)+1) == '\277') || (*((p)+1) == '\335') || \
            (*((p)+1) == '\340') || (*((p)+1) == '\363') || \
            (*((p)+1) == '\375'))))
// SBCS WrapUpCharacter
#define IsWrapUpCharC2(p) ((*(p) == '\041') || (*(p) == '\051') || \
                           (*(p) == '\054') || (*(p) == '\056') || \
                           (*(p) == '\072') || (*(p) == '\073') || \
                           (*(p) == '\077') || (*(p) == '\135') || \
                           (*(p) == '\175'))


BOOL IsWrapDown936(BOOL fDBCS, LPCTSTR psz)
{
    return fDBCS ? IsWrapDownCharC(psz) : IsWrapDownCharC2(psz);
}

BOOL IsWrapUp936(BOOL fDBCS, LPCTSTR psz)
{
    return fDBCS ? IsWrapUpCharC(psz) : IsWrapUpCharC2(psz);
}

#if 0
/*****************************************************************************
 *
 *  aMimeCharSet
 *
 *  Knowledge of character sets goes here.
 *
 *****************************************************************************/
PSCRIPTINFOTABLE _aMimeCharSet;

// Our minimum default knowledge of charsets
// Used for failure case
static const MIMECSET aDefaultCSet = 
{ _IsFECodePage(1252), TRUE, FALSE, "Western", 
"Windows-1252", "Windows-1252", 1252,1252, ICHRCNV_NONE, IsWrapSbcs, IsWrapSbcs,};

static const MIMECSETTABLE aDefaultCSetTable[] =
{
    {
        NULL,
        (PMIMECSETTABLE)&aDefaultCSetTable[1],
    },
    
    {
        (PMIMECSET)&aDefaultCSet,
        NULL,
    },
};

#endif

#define RES_STRING_LANG_CP1252				0
#define RES_STRING_ENCODE_CP1252_ANSI		0
#define RES_STRING_LANG_CP932				0
#define RES_STRING_ENCODE_CP932_JIS			0
#define RES_STRING_LANG_CP932				0
#define RES_STRING_ENCODE_CP932_EUC			0
#define RES_STRING_LANG_CP932				0
#define RES_STRING_ENCODE_CP932_SJIS		0
#define RES_STRING_LANG_CP949				0
#define RES_STRING_ENCODE_CP949_KSC			0
#define RES_STRING_LANG_CP1250				0
#define RES_STRING_ENCODE_CP1250_ANSI		0
#define RES_STRING_LANG_CP1250				0
#define	RES_STRING_ENCODE_CP1250_8859_2		0
#define RES_STRING_LANG_CP1251				0
#define RES_STRING_ENCODE_CP1251_ANSI		0
#define RES_STRING_LANG_CP1251				0
#define RES_STRING_ENCODE_CP1251_KOI8_R		0
#define RES_STRING_LANG_CP874				0
#define RES_STRING_ENCODE_CP874_ANSI		0
#define RES_STRING_LANG_CP950				0
#define RES_STRING_ENCODE_CP950_BIG5		0
#define RES_STRING_LANG_CP936				0
#define RES_STRING_ENCODE_CP936_GB			0
#define RES_STRING_LANG_CP1253				0
#define RES_STRING_ENCODE_CP1253_ANSI		0
#define RES_STRING_LANG_CP1254				0
#define RES_STRING_ENCODE_CP1254_ANSI		0
#define RES_STRING_LANG_CP1255				0
#define RES_STRING_ENCODE_CP1255_ANSI		0
#define RES_STRING_LANG_CP1256				0
#define RES_STRING_ENCODE_CP1256_ANSI		0
#define RES_STRING_LANG_CP1257				0
#define RES_STRING_ENCODE_CP1257_ANSI		0
#define RES_STRING_LANG_CP1258				0
#define RES_STRING_ENCODE_CP1258_ANSI		0

static SCRIPTINFO aDefScriptInfo[] =
{
    { RES_STRING_LANG_CP1252, RES_STRING_ENCODE_CP1252_ANSI, 
      1252,  1252, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP932,  RES_STRING_ENCODE_CP932_JIS,  
      932, 50220, ICHRCNV_JIS, IsWrapUp932, IsWrapDown932, },

    { RES_STRING_LANG_CP932,  RES_STRING_ENCODE_CP932_EUC,
      932, 51932, ICHRCNV_EUCJP, IsWrapUp932, IsWrapDown932, },

    { RES_STRING_LANG_CP932,  RES_STRING_ENCODE_CP932_SJIS,
      932,   932, ICHRCNV_SJIS, IsWrapUp932, IsWrapDown932, },

    { RES_STRING_LANG_CP949,  RES_STRING_ENCODE_CP949_KSC,
      949,   949, ICHRCNV_NONE, IsWrapUp949, IsWrapDown949, }, 

    { RES_STRING_LANG_CP1250, RES_STRING_ENCODE_CP1250_ANSI,
      1250,  1250, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    }, 

    { RES_STRING_LANG_CP1250, RES_STRING_ENCODE_CP1250_8859_2,
      1250, 28592, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP1251, RES_STRING_ENCODE_CP1251_ANSI,
      1251,  1251, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP1251, RES_STRING_ENCODE_CP1251_KOI8_R,
      1251, 20866, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP874,  RES_STRING_ENCODE_CP874_ANSI,
      874,   874, ICHRCNV_NONE, IsWrapSbcs, IsWrapSbcs, },

    { RES_STRING_LANG_CP950,  RES_STRING_ENCODE_CP950_BIG5,
      950,   950, ICHRCNV_NONE, IsWrapUp950, IsWrapDown950, },

    { RES_STRING_LANG_CP936,  RES_STRING_ENCODE_CP936_GB,
      936,   936, ICHRCNV_NONE, IsWrapUp936, IsWrapDown936, },

    { RES_STRING_LANG_CP1253, RES_STRING_ENCODE_CP1253_ANSI,
      1253,  1253, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP1254, RES_STRING_ENCODE_CP1254_ANSI,
      1254,  1254, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP1255, RES_STRING_ENCODE_CP1255_ANSI,
      1255,  1255, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP1256, RES_STRING_ENCODE_CP1256_ANSI,
      1256,  1256, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP1257, RES_STRING_ENCODE_CP1257_ANSI,
      1257,  1257, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },

    { RES_STRING_LANG_CP1258, RES_STRING_ENCODE_CP1258_ANSI,
      1258,  1258, ICHRCNV_NONE, IsWrapSbcs,  IsWrapSbcs,    },
};

#if 0

GLOBALDEF PUBLIC LPTSTR pDefaultAliases[] = 
{
"windows-1252,us-ascii,iso8859-1,ascii,iso_8859-1,iso-8859-1,ANSI_X3.4-1968,iso-ir-6,ANSI_X3.4-1986,ISO_646.irv:1991,ISO646-US,us,IBM367,cp367,csASCII,latin1,iso_8859-1,iso_8859-1:1987,iso-ir-100,ibm819,cp819",
"csISO2022JP,iso-2022-jp",
"Extended_UNIX_Code_Packed_Format_for_Japanese,csEUCPkdFmtJapanese,x-euc-jp",
"shift_jis,x-sjis,ms_Kanji,csShiftJIS",
"ks_c_5601,ks_c_5601-1987,korean,csKSC56011987",
"windows-1250,x-cp1250",
"iso8859-2,iso-8859-2,iso_8859-2,latin2, iso_8859-2:1987, iso-ir-101, l2, csISOLatin2",
"windows-1251, x-cp1251",
"csKOI8R, koi8-r, koi-8",
"windows-874",
"big5, csbig5, x-x-big5",
"GB_2312-80, iso-ir-58, chinese, csISO58GB231280, csGB2312, gb2312",
"windows-1253, iso-8859-7, ISO_8859-7:1987, iso-ir-126, ISO_8859-7, ELOT_928, ECMA-118, greek, greek8, csISOLatinGreek",
"windows-1254",
"windows-1255, ISO_8859-8:1988, iso-ir-138, ISO_8859-8, ISO-8859-8, hebrew, csISOLatinHebrew",
"windows-1256",
"windows-1257",
"windows-1258",
""
};

// GetCodepageFromAlias
//
// Purpose: Read the MIME database entry to find out
//          what windows codepage will be associated
//          with the alias name
//
// In:      hkey - the key for MIME charset db 
//          szAlias - alias name
//          bRecurse - TRUE if getting in recursively
//
// Out:     pBaseCharset - the buffer where any available 
//          info on the base charset returns
//
// returns  TRUE if successful, FALSE otherwise
//
typedef struct {
    TCHAR szName[MAX_CHARSET_NAMESTR];
    int   iCp;
    int   iInetCp;
} BASECHARSET;

BOOL GetCodepageFromAlias(HKEY hkey, LPCTSTR szAlias, BASECHARSET *pBaseCharset, BOOL bRecurse)
{
    TCHAR szCharset[MAX_CHARSET_NAMESTR];
    HKEY hKeyAlias;
    DWORD dwCp = 0;
    DWORD  iType, iSize;

    if (!pBaseCharset)
        return FALSE;

    // pressume that we'll fail
    pBaseCharset->iCp = pBaseCharset->iInetCp = 0;
    pBaseCharset->szName[0] =  TEXT('\0');

    if (RegOpenKeyEx(hkey, szAlias, 0, KEY_QUERY_VALUE, &hKeyAlias) == ERROR_SUCCESS)
    {
        // see if the alias is a 'real' charset
        iSize = sizeof(pBaseCharset->iCp);
        if (RegQueryValueEx(hKeyAlias, REGSTR_VAL_CODEPAGE, 0, 
                            &iType, (BYTE *)&pBaseCharset->iCp, &iSize) == ERROR_SUCCESS
           && iType == REGSTR_VAL_CODEPAGE_TYPE)
        {
            // since this is a base charset that has a codepage value in it,
            // we copy its name for return. 
            // REVISIT: not sure if lstrcpyn stops at null so make room for 
            //          the ending null (-sizeof(TCHAR))
            //
            lstrcpyn(pBaseCharset->szName, szAlias, sizeof(pBaseCharset->szName)-sizeof(TCHAR));

            // check if we have internet codepage separately
            if (RegQueryValueEx(hKeyAlias, REGSTR_VAL_INETENCODING, 0,
                                &iType, (BYTE *)&pBaseCharset->iInetCp, &iSize) !=
                                ERROR_SUCCESS
               || iType != REGSTR_VAL_INETENCODING_TYPE)
            {
                // the entry doesn't have a valid value, but we don't
                // have to fail here, because the charset may not need
                // a separate codepage value.
                pBaseCharset->iInetCp = pBaseCharset->iCp;
            }
        }
        else if (bRecurse == FALSE)
        {
            // the alias is not a real charset.
            // we have to refer to the one specified at aliasfor value 
            // and try again.  
            // this will fail if the registry has more than one-nested entry,
            iSize = sizeof(szCharset);
            if (RegQueryValueEx(hKeyAlias, REGSTR_VAL_ALIASTO, 0, &iType, (BYTE *)szCharset, &iSize) == ERROR_SUCCESS
               && iType == REGSTR_VAL_ALIASTO_TYPE)
            {
                // found the real charset for the alias, lets recurse to try again.
                GetCodepageFromAlias(hkey, szCharset, pBaseCharset, TRUE);
            }
        }
        // [comments] 
        // if (bRecurse == TRUE)
        // we found no codepage value at the charset key
        // which is in this case supposed to be a 'real' charset.
        // we won't seek for another charset key anymore...
        

        RegCloseKey(hKeyAlias);
    }
    return (pBaseCharset->iCp != 0);
}


// CreateMimeCsetEntry
//
// Purpose: Find an entry of MIMECSET for the given
//          codepage and charset name. Create one if not found.
//          This is one time creation. The table is not modified
//          during session
//
// In:      pTbl - pointer to the array of MIMECSETTABLE
//          CodePage - windows codepage ID
//          szCharset - the name of charset
//
//
MIMECSETTABLE * CreateMimeCsetEntry(MIMECSETTABLE *pTbl, BASECHARSET *pBaseCharset)
{
    MIMECSETTABLE * pentry;
    MIMECSETTABLE * pcreate;
    MIMECSETTABLE * newEntry;

    if (!pBaseCharset) return NULL;
    if (!pTbl) return NULL;

    for (pcreate = pentry = pTbl; pentry != NULL; pentry = pentry->pnext)
    {
        if (pentry->pmime && pentry->pmime->CodePage == pBaseCharset->iCp)
        {
            if (pentry->pmime->AltCP == pBaseCharset->iInetCp)
                return pentry;

            // this is where we'll add an entry in case 
            // we don't find it. we'll grab the last entry
            // that has same codepage id
            pcreate = pentry;
        }
        // take the last entry if no codepage matched
        if (!pentry->pnext)
            pcreate = pentry;
    }

    // insert an entry to the codepage
    newEntry = (MIMECSETTABLE *) GTR_MALLOC(sizeof(MIMECSETTABLE));
    if (newEntry)
    {
#ifdef DEBUG
        memset(newEntry, 0, sizeof(MIMECSETTABLE));
#endif
        if (newEntry->pmime = (PMIMECSET)GTR_MALLOC(sizeof(MIMECSET)))
        {
#ifdef DEBUG
            memset(newEntry->pmime, 0, sizeof(MIMECSET));
#endif
            newEntry->pnext = pcreate->pnext;
            pcreate->pnext = newEntry;

            // setting codepages to the entry
            newEntry->pmime->CodePage = pBaseCharset->iCp;
            newEntry->pmime->AltCP = pBaseCharset->iInetCp;
            newEntry->pmime->fValidCodePage = IsValidCodePage(newEntry->pmime->CodePage);
            newEntry->pmime->fValidAltCodePage = IsValidCodePage(newEntry->pmime->AltCP);
            newEntry->pmime->fFECodePage       = _IsFECodePage(newEntry->pmime->CodePage);

        }
        else
        {
            // delete the entry on failure
            GTR_FREE(newEntry);
            newEntry = NULL;
        }
    }
    return newEntry;
}


BOOL IsSameBaseCharset(const char *alias_list, const char *alias)
{
	LPCTSTR p;
    if (!alias_list) return FALSE;

    while (*alias_list != _TEXT('\0') )
    {
        // Skip leading white space or , before a token
        while (*alias_list == _TEXT(' ') || *alias_list == _TEXT(','))
        {
            // No concern for double byte here, because
            // chars >127 are not allowed in the charaset tag
            //
            ++alias_list; 
        }

        // Find the end of the current token
        p = StrChr(alias_list,_TEXT(','));
        if ( p == NULL )
            p = &alias_list[lstrlen(alias_list)];

        // Now see if we have a match
        if (StrCmpNI(alias_list, alias, p-alias_list) == 0)
            return TRUE;
        
        alias_list = p;
    }

    return FALSE;
}

// AddAliasToCharset
//
// Purpose: add an aliase to the entry. this function
//          assumes there's no duplicate occurrences of
//          a charset name in the alias table, so if 
//          there's any, it just adds to the table.
//
//
//
BOOL AddAliasToCharset(PMIMECSETTABLE pCset, LPCTSTR szAlias)
{
    int cAlias;
    ASSERT(pCset);
    ASSERT(pCset->pmime);
    ASSERT(szAlias);

    if (!pCset || !pCset->pmime || !szAlias) return FALSE;

    cAlias = lstrlen(szAlias)+1;

    // see if the entry has alias name table
    if (!pCset->pmime->AliasName)
    {
        if (pCset->pmime->AliasName = (LPTSTR)GTR_MALLOC(sizeof(TCHAR)*cAlias))
            lstrcpy(pCset->pmime->AliasName, szAlias);
        else
            return FALSE;
    }
    else 
    {
        LPTSTR szAliasName = pCset->pmime->AliasName;
        int len = lstrlen(szAliasName);

        // check if we already have the alias
        if(IsSameBaseCharset(pCset->pmime->AliasName, szAlias))
            return TRUE;

        // we already have the table, but alias. add it.
        // each alias is separated with a comma, except at the end
        pCset->pmime->AliasName = (LPTSTR)GTR_REALLOC(szAliasName, sizeof(TCHAR)*(len+cAlias+1));
        if (pCset->pmime->AliasName)
        {
            pCset->pmime->AliasName[len] = TEXT(',');
            lstrcpy(&(pCset->pmime->AliasName[len+1]),szAlias);

            // not sure if GTR_REALLOC puts nulls for us
            pCset->pmime->AliasName[len+cAlias] = TEXT('\0');
        }
        else
            return FALSE;
    }
    return TRUE;
}

// CreateMimeCsetTable
//
// Purpose: Load the MIME database charset extention
//          it allocates memory for the character set 
//          knowledge base. fills it with the basic information
//          This does not expect to be called more than once
//          per session (called from InitProcess())
//
// In:      ppCSetTable - buffer for a pointer 
// Out:     A pointer to the head of MIMECSETTABLE entries
//
BOOL CreateMimeCsetTable(MIMECSETTABLE ** ppCSetTable)
{
    MIMECSETTABLE * pTbl; // pointer to the begining of the table
    MIMECSETTABLE * pCset;// pointer to the current entry
    HKEY    hkey;
    DWORD   dwIndex;
    char    szAlias[MAX_CHARSET_NAMESTR];
    char    szKeyName[MAX_PATH];
    BASECHARSET BaseCharset;

    ASSERT(ppCSetTable);

    // pressume failure
    *ppCSetTable = NULL;

    // Allocate memory for charset table
    // 
    if(!(pTbl = (MIMECSETTABLE *)GTR_MALLOC(sizeof(MIMECSETTABLE))))
	    return FALSE;

#ifdef DEBUG
    memset(pTbl, 0, sizeof(MIMECSETTABLE));
#endif

    // Put codepage 1252 at the beginning since it is required anyway
    BaseCharset.iCp = aDefaultCSet.CodePage;
    BaseCharset.iInetCp = aDefaultCSet.AltCP;
    if(!CreateMimeCsetEntry(pTbl, &BaseCharset))
    {
        FreeMimeCsetTable(pTbl);
        return FALSE;
    }

    // read in the knowledge of charset from the registry
    // 1) enumrate charset from MIME database charset key
    // 2) if the key has a 'Codepage' value, check if the entry is
    //    already created then create one if not.
    // 3) if the key has 'AliasToCharset' value, load it to our
    //    internal alias table upon real charset's codepage
    //
    // Get the key for MIME database charset extension
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, REGSTR_KEY_MIME_DATABASE_CHARSET, 
                    0, KEY_QUERY_VALUE, &hkey) 
                    == ERROR_SUCCESS)
    {
        ASSERT(hkey);        
        dwIndex = 0;
        // enumrate keys under 'charset'
        while (RegEnumKey(hkey, dwIndex++, (LPSTR)szAlias, sizeof(szAlias)) == ERROR_SUCCESS)
        {


            // get base charset for the alias and codepage value
            GetCodepageFromAlias(hkey, szAlias, &BaseCharset, FALSE);

            // allocate memory for the entry if not already had one
            if (!(pCset = CreateMimeCsetEntry(pTbl, &BaseCharset)))
            {
                // allocation error, bail out
                ASSERTMSG(FALSE, ("mshtml: CreateMimeCsetTable-failed to create charset entry"));
                // this is fatal
                break;
            }
            // add the alias name to the charset entry
            // since szAlias is a key name at the same level,
            // there won't be a dup. AddAliasToCharset won't check it.
            if (!AddAliasToCharset(pCset, szAlias))
            {
                // problem with memory but not totally fatal..
                ASSERTMSG(FALSE, ("mshtml: CreateMimeCsetTable-failed to create alias entry"));
                break;
            }
        }
        RegCloseKey(hkey);
    }

    wsprintf(szKeyName, REGSTR_KEY_MIME_DATABASE_CODEPAGE "\\%d", GetACP());

    // make sure we have a charset entry for the system codepage
    // look for 'web charset' under mime database
    if (RegOpenKeyEx(HKEY_CLASSES_ROOT, szKeyName, 0, KEY_QUERY_VALUE, &hkey) == ERROR_SUCCESS)
    {
        int iType;
        int iSize = sizeof(szAlias);

        if (RegQueryValueEx(hkey, REGSTR_VAL_WEBCHARSET, 0, 
                            &iType, (BYTE *)szAlias, &iSize) == ERROR_SUCCESS)
        {
            BaseCharset.iCp = BaseCharset.iInetCp = GetACP();
            if (pCset = CreateMimeCsetEntry(pTbl, &BaseCharset))
            {
                if (AddAliasToCharset(pCset, szAlias))
                {
                    // OK, set this table back to the caller
                    *ppCSetTable = pTbl;
                }
            }
        }
        RegCloseKey(hkey);
    }
    else
        *ppCSetTable = NULL;

    if (!*ppCSetTable)
    {
        // create default database if MIME database is not setup
        int i;
        for (i=0; i<ARRAYSIZE(aDefScriptInfo); i++)
        {
            BaseCharset.iCp     = aDefScriptInfo[i].iCp;
            BaseCharset.iInetCp = aDefScriptInfo[i].iInetCp;
            if (pCset = CreateMimeCsetEntry(pTbl, &BaseCharset))
            {
                if (!AddAliasToCharset(pCset, pDefaultAliases[i]))
                {
                    // allocation error
                    ASSERTMSG(FALSE, ("mshtml: CreateMimeCsetTable-failed to create alias entry"));
                }
                 
            }
            else
            {
                // not much more we can do.
                FreeMimeCsetTable(pTbl);
                break;
            }
        }
        *ppCSetTable = pTbl;
    }

    return (*ppCSetTable != NULL);
}


BOOL
GetCodepageInfoFromIEReg(HKEY hkeyRoot, int codepage,
                       LPTSTR szScript, int cScript, 
                       LPTSTR szEncoding, int cEncoding,
                       BOOL *pbAutoDetect)
{
    TCHAR sz[MAX_CHARSET_NAMESTR];
    TCHAR szKeyName[MAX_CHARSET_NAMESTR];
    HKEY hkey;
    int iType, iSize;

    ASSERT(pbAutoDetect);

    wsprintf(szKeyName, "%d", codepage);

    if (hkeyRoot &&
        RegOpenKeyEx(hkeyRoot, szKeyName, 0, KEY_QUERY_VALUE, &hkey)
        == ERROR_SUCCESS)
    {
        // load friendly script name (known as language name)
        if ((iSize = cScript) > 0 &&
             (RegQueryValueEx(hkey, REGSTR_VAL_FONT_SCRIPT,
                             0, &iType, (BYTE *)szScript, &iSize) 
                             != ERROR_SUCCESS
            || iType != REGSTR_VAL_FONT_SCRIPT_TYPE))
        {
            szScript[0] = TEXT('\0');
        }
        // load friendly encoding name (known as mime charset name)
        // load it only when we are at real codepage
        if ((iSize = cEncoding) > 0 &&
            (RegQueryValueEx(hkey, REGSTR_VAL_ENCODENAME,
                            0, &iType, (BYTE *)szEncoding, &iSize) 
                            != ERROR_SUCCESS
                || iType != REGSTR_VAL_ENCODENAME_TYPE))
        {
                szEncoding[0] = TEXT('\0');
        }
        // load a flag that indicates there's autodetection.
        iSize = sizeof(sz);
        if (RegQueryValueEx(hkey, REGSTR_VAL_AUTODETECT,
               0, &iType, (BYTE *)sz, &iSize) != ERROR_SUCCESS
            || iType != REGSTR_VAL_AUTODETECT_TYPE)
        {
            sz[0] = TEXT('\0');
        }
        *pbAutoDetect = (lstrcmp(sz, TEXT("yes")) == 0);

        // check if we already know that this codepage
        // requires 'auto detect' entry 
        if (!*pbAutoDetect && IsCodepageNeedAutoDetect(codepage))
            *pbAutoDetect = TRUE;

        RegCloseKey(hkey);
        return TRUE;
    } 
    return FALSE;
}

//  SetCodepageInfo
//
//  Load script name, encoding name, various flags for mime entry
//  Language name here is strictly speaking script name for a codepage.
//  MIME database gives us the default, but we should load the our own
//  setting which is unique per installation
//
void 
SetCodepageInfo(MIMECSETTABLE * pTbl,  PMIMECSET pmime, 
                LPCTSTR szScript, LPCTSTR szEncoding, BOOL bAutoDetect)
{
    int nEntry;
    int nDefEntry = -1;
    TCHAR sz[MAX_CHARSET_NAMESTR];
    ASSERT(pmime);

    // Try to find default information if our registry doesn't
    // have it..
    // we need to scan this anyway because now we're assigning
    // our internal index of ichrcnv and wordwrap func to fareast
    // encodings. ichrcnv should become to take a string for an ID.
    // wordwrap characters can be a separate entry in the reg
    // so we can load wrap-up/wrap-down characters from there.
    // 
    nEntry = ARRAYSIZE(aDefScriptInfo);
    while(nEntry--)
    {
       // if the alternative codepage for its internet encoding
       // is available, check with it first
       if (pmime->CodePage == aDefScriptInfo[nEntry].iCp)
       {
           nDefEntry = nEntry;
       }
       if (pmime->AltCP == aDefScriptInfo[nEntry].iInetCp)
       {
           // found a match with alternative codepage
           // note nDefEntry == nEntry this case because
           // codepage values are always same if inet 
           // codepages are equal.
           break;
       }
    }

    if (!pmime->Lang_str)
    {
        lstrcpyn(sz, szScript, sizeof(sz));
        if (!sz[0])
        {
            // registry doesn't have a script name for codepage
            if (nDefEntry >= 0)
            {
                // load res if known charset
                LoadString(wg.hInstance, aDefScriptInfo[nDefEntry].iLangID,
                           sz, sizeof(sz));
            }
            // BUGBUG: if unknown, probably we can try MIME database..
            //         but not here, we should do it in somewhere in 
            //         GetCodepageFromAlias()
        }
        if (sz[0])
        {
            pmime->Lang_str = GTR_strdup(sz);
        }
    }
    if (!pmime->Mime_str)
    {
        lstrcpyn(sz, szEncoding, sizeof(sz));
        if (!sz[0])
        {
            if (nDefEntry >= 0 && aDefScriptInfo[nDefEntry].iInetCp == pmime->AltCP)
            {
                // load res if known charset
                LoadString(wg.hInstance, aDefScriptInfo[nDefEntry].iEncodeID,
                               sz, sizeof(sz));
            }
            else if (pmime->AliasName)
            {
                // if unknown, try the first alias name (charset name)
                LPTSTR p = StrChr(pmime->AliasName,TEXT(','));

                if (p == NULL)
                    p = &pmime->AliasName[lstrlen(pmime->AliasName)];

                lstrcpyn(sz, pmime->AliasName, p-pmime->AliasName+1);
            }
        }
        if (sz[0])
        {
            pmime->Mime_str = GTR_strdup(sz);
        }
    }

    // setup wordwrap functions, index for ISO converter
    // BUGBUG1: wordwrap functions can use characters from registry
    //          if specified. See IsWrapUp/Down macro in intl.h
    // BUGBUG2: ISO converter (ichrcnv) should take a name of encoding 
    //          so it can be independent from MSHTML implementation.
    //          at least ichrcnv should take both codepage and index
    if (nDefEntry >= 0)
    {
        if (!pmime->IsWrapUp)
            pmime->IsWrapUp = aDefScriptInfo[nDefEntry].IsWrapUp;

        if (!pmime->IsWrapDown)
            pmime->IsWrapDown = aDefScriptInfo[nDefEntry].IsWrapDown;

        if (!pmime->iChrCnv)
            pmime->iChrCnv = aDefScriptInfo[nDefEntry].iChrCnv;

        // if ichrcnv can translate this charset to windows charset,
        // we'll see this as valid codepage even if the codepage
        // isn't supported directly with NLS.
        if (pmime->iChrCnv)
            pmime->fValidAltCodePage = TRUE;
    }

    // if we have more than one alternative codepage, it is possible
    // that we support 'auto detection' within one windows codepage.
    // (like for Japanese script)
    if (bAutoDetect)
    {
        BASECHARSET bc;
        MIMECSETTABLE * pAuto;

        // insert the autodetect entry to the table
        bc.iCp = pmime->CodePage;
        bc.iInetCp = CP_AUTODETECT;
        bc.szName[0] = 0;
        if (pAuto=CreateMimeCsetEntry(pTbl, &bc))
        {
            if (pAuto->pmime) 
            {
                if (pmime->Lang_str)
                    pAuto->pmime->Lang_str = GTR_strdup(pmime->Lang_str);

                // load localizable string for 'auto detect'
                LoadString(wg.hInstance, RES_STRING_ENCODE_AUTODETECT,
                               sz, sizeof(sz));

                if (sz[0])
                    pAuto->pmime->Mime_str = GTR_strdup(sz);

                // use same wordwrap functions 
                pAuto->pmime->IsWrapUp = pmime->IsWrapUp;
                pAuto->pmime->IsWrapDown = pmime->IsWrapDown;
                // set a special value for autodetect 
                pAuto->pmime->iChrCnv = ICHRCNV_AUTO;
            }
        }
    }
}

//  InitMimeCharsetTable
//
//  This is the only place we override the const-ness of aMimeCharSet.
//
BOOL InitMimeCharsetTable(void)
{
    TCHAR szScript[MAX_CHARSET_NAMESTR];
    TCHAR szEncoding[MAX_CHARSET_NAMESTR];
    MIMECSETTABLE *pMimeTbl;
    HKEY     hkeyRoot;
    int      iCpAddedAutoDetect;
    BOOL     bAutoDetect;
    // Create a table for character set knowledge
    // out of MIME database extention.
    if (!CreateMimeCsetTable(&_aMimeCharSet))
    {
        // this is really fatal
        ASSERTMSG(FALSE, ("MimeCharTable creation failure"));

        // Use minimal default table, which at least has 1252
        _aMimeCharSet = (MIMECSETTABLE *)aDefaultCSetTable;
    }
    pMimeTbl = _aMimeCharSet;
    // Now load our own information from registry
    // If this fails, we set 0 to the key, so we'll use
    // our default values where it's possible
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REGSTR_PATH_INTERNATIONAL, 
                     0, KEY_QUERY_VALUE, &hkeyRoot) != ERROR_SUCCESS)
    {
        hkeyRoot = 0; // used as flag
    }


    iCpAddedAutoDetect = -1; // impossible codepage
    for (pMimeTbl = pMimeTbl->pnext; pMimeTbl != 0; pMimeTbl = pMimeTbl->pnext)
    {
        // fill up the pmime entry
        if (pMimeTbl->pmime && pMimeTbl->pmime->fValidCodePage)
        {
            // Note AltCp == CodePage if the charset doesn't have
            // separate internet encoding so try altcp first.
            // we don't have to look into registry if the codepage is
            // not valid, but want to load script info here if the
            // codepage is valid. this is because we may be able to 
            // support the codepage if our character set converter can 
            // translate it to windows codepage. 
            // (example:JIS altcp == 50220)
            //
            if (!pMimeTbl->pmime->fValidAltCodePage ||
                !GetCodepageInfoFromIEReg(hkeyRoot, pMimeTbl->pmime->AltCP,
                                   szScript, sizeof(szScript),
                                   szEncoding, sizeof(szEncoding),
                                   &bAutoDetect))
            {
                // try with windows codepage. can't use encoding name
                szScript[0] = szEncoding[0] = TEXT('\0');
                {
                    GetCodepageInfoFromIEReg(hkeyRoot, pMimeTbl->pmime->CodePage,
                                             szScript, sizeof(szScript),
                                             NULL, 0,
                                             &bAutoDetect);
                }
            }

            // a little trick to avoid adding 'auto detect' more
            // than once.
            if (bAutoDetect == TRUE)
            {
                if (iCpAddedAutoDetect == pMimeTbl->pmime->CodePage)
                    bAutoDetect = FALSE;
                else // remember the codepage we added 'autodetect'
                    iCpAddedAutoDetect = pMimeTbl->pmime->CodePage;
            }

            // setup script/encoding information for the codepage
            // 
            SetCodepageInfo(pMimeTbl, pMimeTbl->pmime,
                            szScript, szEncoding, bAutoDetect);
        }
    }
    if (hkeyRoot)
    {
        RegCloseKey(hkeyRoot);
    }
    // Initialize listbox for 'language' menu.
    return TRUE;
}


void FreeMimeCsetTable(MIMECSETTABLE * pMimeTbl)
{
    MIMECSETTABLE * pdelTbl;
    PMIMECSET      pmime;

    // we might have assigned hardcode table
    // in failure case...
    if (!pMimeTbl || pMimeTbl == aDefaultCSetTable)
        return;

    while (pMimeTbl->pnext)
    {
        if (pmime=pMimeTbl->pnext->pmime)
        {
            // free string buffer from the mime entry
            if (pmime->Lang_str)
            {
                GTR_FREE(pmime->Lang_str);
                pmime->Lang_str = NULL;
            }
            if (pmime->Mime_str)
            {
                GTR_FREE(pmime->Mime_str);
                pmime->Mime_str = NULL;
            }
            if (pmime->AliasName)
            {
                GTR_FREE(pmime->AliasName);
                pmime->AliasName = NULL;
            }
            // free the mime entry itself
            GTR_FREE(pmime);
        }
        // free the table entry
        pdelTbl  = pMimeTbl;
        pMimeTbl = pMimeTbl->pnext;
        GTR_FREE(pdelTbl);
    }
    GTR_FREE(pMimeTbl);
}

// MimeCharsetTable_Cleanup
//
// Called from _TerminateProcess()
//
void MimeCharsetTable_Cleanup(void)
{
    MimeCharSetList_Cleanup();
    FreeMimeCsetTable(_aMimeCharSet);
    _aMimeCharSet = NULL;
}

// GetMimeCharsetEntry
//
// Go through MIMECSETTABLE list and find the nearest match with
// the given a codepage and an encoding name
// Returns an index.
//
//
int GetMimeCharsetEntry(int codepage, LPTSTR sz)
{
    const MIMECSETTABLE * pEntry = (MIMECSETTABLE *)aMimeCharSet;
    PMIMECSET pmime;
    int i,iDefault=-1;


    if (!pEntry) return 0;

    for (pEntry=pEntry->pnext, i = 0; pEntry != NULL; pEntry = pEntry->pnext)
    {
        if (!(pmime=pEntry->pmime)) break;
        if (!pmime->fValidCodePage)
            continue;


        if (pmime->CodePage == codepage && iDefault == -1)
            iDefault = i;

        if (sz && lstrcmpi(pmime->Mime_str, sz) == 0)
            break;

        i++;
    }

    if (!pEntry)
        return iDefault >0 ? iDefault:0;

    return i;
}


// The first object is always 1252
// This can be called with position == 0, expected to return
// pmime for Western script
//
PMIMECSET GetMimeCharsetEntryAt(int position)
{
    MIMECSETTABLE * pEntry;

//    ASSERT(aMimeCharSet);
    if (!aMimeCharSet) 
        return (PMIMECSET)&aDefaultCSet;

    for(pEntry=aMimeCharSet->pnext; position > 0 && pEntry; pEntry = pEntry->pnext)
    {
       position--;
    }

//    ASSERT(pEntry);

    // to prevent any gpf at callers side
    return ((pEntry && pEntry->pmime) 
           ? pEntry->pmime : (PMIMECSET)&aDefaultCSet);
}


HWND ghwndMimeCharSet = NULL;   // listbox window handle which contains list of languages


BOOL MimeCharSetList_Init(MIMECSETTABLE * pTbl)
{
    char    sz[256];
    MIMECSETTABLE * pentry;
    int iCSetIdx;

    if (!pTbl) return FALSE;

    if (ghwndMimeCharSet != NULL)
        DestroyWindow(ghwndMimeCharSet);

    // BUGBUG: Can we create this listbox storage as a child window here?
    ghwndMimeCharSet = CreateWindow("LISTBOX", NULL, WS_POPUP | WS_DISABLED | LBS_SORT,
                                    0, 0, 0, 0, NULL, NULL, wg.hInstance, NULL);
    if (ghwndMimeCharSet)
    {
        iCSetIdx = 0;
        for (pentry = pTbl->pnext; pentry != NULL; pentry = pentry->pnext, iCSetIdx++)
        {
            if (!pentry->pmime) break; // shouldn't happen

            // codepages that are not installed won't show up
            if (pentry->pmime->fValidCodePage && pentry->pmime->fValidAltCodePage)
            {
                // set this pmime to the item data
                // so we can use it later
                wsprintf(sz, "%s (%s)", pentry->pmime->Lang_str, pentry->pmime->Mime_str);
            
                ListBox_SetItemData(ghwndMimeCharSet, 
                                    ListBox_AddString(ghwndMimeCharSet, sz), 
                                    iCSetIdx);
            }
        }
        return TRUE;
    }

    return FALSE;

}

BOOL MimeCharSetList_Cleanup(void)
{
    BOOL bRet = TRUE;

    if (ghwndMimeCharSet != NULL)
    {
        bRet = DestroyWindow(ghwndMimeCharSet);
        //Since we destroyed it, reset the global to NULL. Else, we RIP later!
        ghwndMimeCharSet = NULL;
    }

    return bRet;
}

LONG GetNumMimeCSet(void)
{
    return ListBox_GetCount(ghwndMimeCharSet);
}

void CheckMenuMimeCharSet(PMWIN tw, HMENU hMenu)
{
    int iNumMimeCSet = GetNumMimeCSet();
    PMIMECSET pmimeDoc;
    PMIMECSET pmimeItem;
    int i; 
    int  iCSetIdx;

    if (iNumMimeCSet > 1)
    {
        if (tw && tw->w3doc)
        {
            pmimeDoc = tw->w3doc->pMime;
            for (i = 0; i < iNumMimeCSet; i++)
            {
                UINT uCheck;

                iCSetIdx = ListBox_GetItemData(ghwndMimeCharSet, i);
                pmimeItem = GetMimeCharsetEntryAt(iCSetIdx);

                if (pmimeDoc == pmimeItem)
                    uCheck = MF_CHECKED | MF_BYCOMMAND;
                else
                    uCheck = MF_UNCHECKED | MF_BYCOMMAND;

                CheckMenuItem(hMenu, iCSetIdx+RES_MENU_ITEM_MIMECSET__FIRST__, uCheck);
            }
        }
    }
}

HMENU CreateMimeCSetMenu(void)
{
    int i, iID, iNumMimeCSet = GetNumMimeCSet();
    HMENU hMenu = NULL;
    char buffer[256];

    if (iNumMimeCSet > 1)
    {
        if (hMenu = CreatePopupMenu())
        {
            for (i = 0; i < iNumMimeCSet; i++)
            {
                ListBox_GetText(ghwndMimeCharSet, i, buffer);
                iID = ListBox_GetItemData(ghwndMimeCharSet, i);
                AppendMenu(hMenu, MF_ENABLED, iID+RES_MENU_ITEM_MIMECSET__FIRST__, buffer);
            }
        }
    }
    return hMenu;
}

HRESULT ShowMimeCSetMenu(HWND hWnd, struct Mwin *tw, LPARAM lParam)
{
    HMENU hmenuLang;
    POINT pt = {GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam)};

    if (tw && tw->w3doc)
    {
        // this is not used?? 
        // int iMimeCharSet = GetMimeCharsetEntry(GETMIMECP(tw->w3doc), tw->w3doc->pMime->Mime_str) + RES_MENU_ITEM_MIMECSET__FIRST__;

        if (hmenuLang = CreateMimeCSetMenu())
        {
            CheckMenuMimeCharSet(tw, hmenuLang);
            TrackPopupMenuEx(hmenuLang, TPM_LEFTALIGN, pt.x, pt.y, hWnd, NULL);
            DestroyMenu(hmenuLang);

            return S_OK;
        }
    }
    return E_FAIL;
}

DWORD GetMimeCSetName(struct Mwin *tw, WCHAR *pwsz, int cwch)
{
    int i, iNumMimeCSet = GetNumMimeCSet();
    char sz[256] = {0};
    DWORD dwRet = 0L;
    PMIMECSET pmimeDoc;
    PMIMECSET pmimeItem;
    int  iCSetIdx;

    if (tw && tw->w3doc)
    {
        pmimeDoc = tw->w3doc->pMime;
        for (i = 0; i < iNumMimeCSet; i++)
        {
            iCSetIdx = ListBox_GetItemData(ghwndMimeCharSet, i);
            pmimeItem = GetMimeCharsetEntryAt(iCSetIdx);
            if (pmimeItem == pmimeDoc)
            {
                ListBox_GetText(ghwndMimeCharSet, i, sz);
                break;
            }
        }
        dwRet = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, sz, -1, pwsz, cwch);
    }
    return dwRet;
}

void AddLanguageMenu(HMENU hmenuView)
{
    HMENU hmenuLanguages = NULL;

    if (hmenuView)
    {
        char buffer[256];

        if (GTR_formatmsg(RES_STRING_LANGUAGE_MENUTEXT, buffer, sizeof(buffer)))
        {
            if (hmenuLanguages = CreateMimeCSetMenu())
            {
                // We insert Languages menu after Fonts menu.
                InsertMenu(hmenuView, 5, MF_POPUP | MF_BYPOSITION, (UINT)hmenuLanguages, buffer);
            }
        }
    }
}


// SetCharsetFromAlias
//
// this sets an internal index to the _aMimeCharSet table
// based on the given charset alias name.
//
// in: alias
// out: w3doc->pMime, w3doc->flags
//
void SetCharsetFromAlias(const char *alias, struct Mwin *tw)
{
    MIMECSETTABLE * pTbl = _aMimeCharSet;
    PMIMECSET pMime;
    int       iChrCnv=ICHRCNV_NONE;

    ASSERT(pTbl);
    ASSERT(tw);
    ASSERT(tw->w3doc);
    ASSERT(alias);

    if (!pTbl || !tw->w3doc)
        return;

    if (!alias)
        return;

    // Get the current charconv index.
    if (tw->w3doc->pMime) 
        iChrCnv=tw->w3doc->pMime->iChrCnv;
	

    for (pTbl = pTbl->pnext; pTbl != NULL; pTbl = pTbl->pnext)
    {
        pMime = pTbl->pmime;

        if (pMime && IsSameBaseCharset(pMime->AliasName, alias))
        {
            if (pMime->fValidCodePage 
            && (pMime->AltCP == pMime->CodePage || pMime->fValidAltCodePage))
            {
                tw->w3doc->pMime = pMime;
                if (IsFEPMime(tw->w3doc->pMime)) 
                {
                    tw->w3doc->flags |= W3DOC_FLAG_FECP;

                    // Re-initialize ichrcnv if we have to start
                    // converting in different encoding
                    //
                    if (pMime->iChrCnv != ICHRCNV_NONE && pMime->iChrCnv != iChrCnv) 
                    { 
                        CharConv_Init(&tw->pChrcnvContext); 
                    } 
                } 
                else
                { 
                    tw->w3doc->flags &= ~W3DOC_FLAG_FECP; 
                } 
            } 
            break; 
        } 
    }

    // do nothing if the given alias is not within our list

}

// GetMimeCharset
//
// This function now accept aliases.
// if sz matches any of aliases for the base character set,
// this returns the index for the imime.
//
void GetMimeCharset(LPTSTR sz, HTRequest *request)
{
    PMIMECSETTABLE pTbl;
    PMIMECSET    pMime;

    ASSERT(aMimeCharSet);
    if (!aMimeCharSet)
        return;

    request->pMime = 0;         /* Assume invalid */
    TrimWhiteSpace(sz);

    for (pTbl=aMimeCharSet->pnext; pTbl!= 0; pTbl=pTbl->pnext)
    {
        if (pTbl->pmime)
            pMime = pTbl->pmime;
        else
            continue;

        if (IsSameBaseCharset(pMime->AliasName, sz))
        {
            if (pMime->fValidCodePage 
            && (pMime->CodePage == pMime->AltCP || pMime->fValidAltCodePage))
            {
                request->pMime = pMime;
            }
            break;
        }
    }
    return;
}

//
// SetDefaultDialogFont
//
// purpose: set font to the given control of the dialog
//          with platform's default character set so that
//          user can see whatever string in the native 
//          language on the platform.
//
// in:      hDlg - the parent window handle of the given control
//          idCtl - ID of the control
//
// note:    this will store created font with window property
//          so that we can destroy it later.
//
void SetDefaultDialogFont(HWND hDlg, int idCtl)
{
    HFONT hfont;
    HFONT hfontGui;
    LOGFONT lf;
    LOGFONT lfGui;

    hfont = GetWindowFont(hDlg);
    GetObject(hfont, sizeof(LOGFONT), &lf);

    hfontGui = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    GetObject(hfontGui, sizeof(LOGFONT), &lfGui);
    if (lfGui.lfCharSet == lf.lfCharSet)
    {
        // if the dialog already has correct character set
        // don't do anything.
        return;
    }

    // If we already have hfont created, use it.
    if(!(hfont = (HFONT)GetProp(hDlg, c_szPropDlgFont)))
    {
        // no we don't. 
        // check if the height of the stock font is just what we want, 
        // if not then we have to create one.
        if (lfGui.lfHeight != lf.lfHeight)
        {
            lfGui.lfHeight = lf.lfHeight;
            if (!(hfont=CreateFontIndirect(&lfGui)))
            {
                // in failure case we use the DEFAULT_GUI_FONT as it is.
                hfont = hfontGui;
            }
        }
        else // no problem to use stock object.
            hfont = hfontGui;

        // Save this so we can delete it later.
        if (hfont != hfontGui)
            SetProp(hDlg, c_szPropDlgFont, hfont);
    }

    SetWindowFont(GetDlgItem(hDlg, idCtl), hfont, FALSE);
}

//
// RemoveDefaultDialogFont
//
// purpose: Destroy the font we used to set gui default font
//          Also removes the window property used to store the font.
//
// in:      hDlg - the parent window handle of the given control
// 
// note:    
void RemoveDefaultDialogFont(HWND hDlg)
{
    HFONT hfont;
    if(hfont = (HFONT)GetProp(hDlg, c_szPropDlgFont))
    {
        DeleteObject(hfont);
        RemoveProp(hDlg, c_szPropDlgFont);
    }
}
//
// PatchLangForISO639
//
// purpose: Fix up abbreviated lang name to make it ISO639 standard.
//          
//
// in:      szLang - lang name which comes from GetLocaleInfo()
//          cp     - default codepage for the lang.
// 
// note:    assumes being called once to initialize wg.
//
void PatchLangForISO639(UINT cp, TCHAR *szLang, int cchLang)
{
    HKEY hkey, hkeycp;
    TCHAR szCp[MAX_SABBREVLANGNAME];
    TCHAR szSabLangPatch[MAX_SABBREVLANGNAME];
    UINT iType, iSize , i;

    ASSERT(szLang);

    wsprintf(szCp, "%ld", cp);
    szSabLangPatch[0] = TEXT('\0');

    // first look at registry to see if there's any user defined patch
    //
    if (RegOpenKeyEx(HKEY_CURRENT_USER, REGSTR_PATH_INTERNATIONAL, 0, KEY_ALL_ACCESS, &hkey) == ERROR_SUCCESS)
    {
        ASSERT(hkey);
        if (RegOpenKeyEx(hkey, szCp, 0, KEY_ALL_ACCESS, &hkeycp) == ERROR_SUCCESS)
        {
            ASSERT(hkeycp);

            iSize = ARRAYSIZE (szSabLangPatch);
            if (RegQueryValueEx(hkeycp, c_szLangPatch, 0, &iType, (BYTE *)szSabLangPatch, &iSize) != ERROR_SUCCESS)
                szSabLangPatch[0] = TEXT('\0'); // just to make sure

            RegCloseKey(hkeycp);
        }
        RegCloseKey(hkey);
    }

    if ( !szSabLangPatch[0] )
    {
        // look into table for default patch

        for (i =0; i < ARRAYSIZE(iso639patch); i++)
        {
            if (lstrcmpi(szLang, iso639patch[i].c_szWrong) == 0) 
                lstrcpy(szSabLangPatch, iso639patch[i].c_szRight);
        }
    }

    // copy the patch if we found it.
    if (szSabLangPatch[0])
        lstrcpyn(szLang, szSabLangPatch, cchLang);

}

//
// GetCharsetFromContent
//
// purpose: Extract charset parameter out of HTTP-EQUIV META tag
//
// ex.<META HTTP-EQUIV="Content-Type" CONTENT="text/html;charset=Windows-1251">
// will return a pointer to "Windows-1251"
//
// in:  szContent - value of CONTENT attribute
// ret: a pointer to the value which "charset=" parameter specifies in success
//      otherwise NULL.
//
// note: IE currently doesn't support any other Content-Type parameters
// 
//
LPCTSTR GetCharsetFromContent(LPCTSTR szContent)
{
    LPCTSTR psz;

    if (!szContent) return NULL;

    psz = szContent;

    // skip the value part of string whatever it is (text/html, image/gif..)
    //
    while((psz =_tcschr(psz,_T(';'))) && *(++psz))
    {
        // if we find some param after ';', skip the leading space
        //
        while(*psz==_T(' ')) 
            psz++;
        
        // if the param is charset, take the value. '=' may or may not
        // sticks right after the param name.
        //
        if (_tcsnicmp(psz, c_szCharset, (ARRAYSIZE(c_szCharset)-1)*sizeof(TCHAR))==0)
        {
            // skip until we see the actual param value
            //
            if ((psz = _tcschr(psz, _T('='))) && *(++psz))
            {
                while(*psz==_T(' ')) 
                    psz++;

                if (*psz)
                {
                    return psz;
                } 
            }
            // Invalid charset parameter!
            break;
        }
        // next param comes after next ';' if any
    }
    return NULL;
}

// TranslateStrCodepage
//
// Purpose: Translate the given string to windows codepage.
//          Does ansi->wide->ansi conversion if the string 
//          is in alternative codepage.
//			The caller has to be responsible for freeing
//			memry for the duplicated string.
//
// In:      pSzFrom   - str in cp we translate from
//          cchFrom   - # of character in bytes
//          pSzTo     - a pointer to a buffer in cp we want to translate
//          cchTo     - # of character in bytes
//          pMime     - PMIMECSET describing code page 
//          bRestore  - perform windows -> alternative translation
//                      when specified.
// Out:     # of characters in bytes
//
int TranslateStrCodepage(char *pSzFrom, int cchFrom, char *pSzTo, int cchTo, PMIMECSET pMime, BOOL bRestore)
{
	int cch = 0;
	int cchW;
	WCHAR *szwBuf;
    int cpFrom, cpTo;

	ASSERT(pSzFrom);
	ASSERT(pSzTo);
	ASSERT(pMime);

	if (IsInstalledPMime(pMime) && IsAltInstalledPMime(pMime))
	{
		if (bRestore)
		{
		// restore the given string to the alternative codepage
			cpFrom = pMime->CodePage;
			cpTo   = pMime->AltCP;
		}
		else
		{
			cpTo = pMime->CodePage;
			cpFrom = pMime->AltCP;
		}
	}
	else
	{
		// codepages are not valid...
		// do something makes sense
		if (pSzFrom != pSzTo)
			lstrcpy(pSzTo, pSzFrom);

		return cchFrom;
	}

	cchW = MultiByteToWideChar(cpFrom, MB_PRECOMPOSED, pSzFrom, cchFrom, NULL, 0);
	szwBuf = GTR_MALLOC(sizeof(WCHAR)*cchW);

	if(szwBuf)
	{
		cchW = MultiByteToWideChar(cpFrom, MB_PRECOMPOSED, pSzFrom, cchFrom, szwBuf, cchW);
		cch = WideCharToMultiByte(cpTo, 0, szwBuf, cchW, pSzTo, cchTo, NULL, NULL);
	}
	if(szwBuf)	GTR_FREE(szwBuf);
	return cch;
}
#endif

static PSCRIPTINFO g_pMime;
void *GetMime() { return g_pMime; }
void SetMime(DWORD charSet) {
	int i, len;
	UINT codepage = GetACP();
	if (_IsFECodePage(codepage)) {
		len = sizeof(aDefScriptInfo) / sizeof(SCRIPTINFO);
		for (i = 0; i < len; i++) {
			if ((UINT) aDefScriptInfo[i].iCp == codepage) {
				g_pMime = aDefScriptInfo+i;
				return;
			}
		}
	}
	g_pMime = NULL;
}

// REGISB added 10/14/97 for ForceLineBreak
int	iBytesofChar(BYTE ch)
{
	if (!g_pMime)
		return 1;
	else
		return BytesofChar(g_pMime, ch);
}

#endif  /* FEATURE_INTL */


// REGISB: added 12/09/97
BYTE GetCorrectCharSet()
{
	LOGFONT		lf;
	HFONT		hfont;
	CHARSETINFO charsetInfo;
			
	hfont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	GetObject(hfont, sizeof(LOGFONT), (LPVOID) &lf);

	if (lf.lfCharSet == 88)
		// FIX for Taiwan bug: For some reason, DEFAULT_GUI_FONT
		// is set to charSet 88 (doesn't map to any real charSet)
		// on Taiwanese systems.  So change to CHINESEBIG5_CHARSET = 136.
		return CHINESEBIG5_CHARSET;	
	else
		if (lf.lfCharSet == 0)
		{
			LCID dwLCID = GetSystemDefaultLCID();

			switch (LOWORD(dwLCID))
			{
			case 0x0405:	// Czech
			case 0x040E:	// Hungarian
			case 0x0415:	// Polish
			case 0x0424:	// Slovenian
				return EASTEUROPE_CHARSET;

			case 0x0413:	// Dutch
			case 0x0409:	// General like English
			case 0x040F:	// Icelandic
			case 0x041D:	// Nordic
			case 0x0414:	// Norway/Danish
			case 0x040A:	// Spanish
			case 0x040B:	// Swedish/Finish
				return ANSI_CHARSET;
			
			case 0x0401:	// Arabic
				return ARABIC_CHARSET;
			
			case 0x0408:	// Greek
				return GREEK_CHARSET;
			
			case 0x040D:	// Hebrew
				return HEBREW_CHARSET;
			
			case 0x0419:	// Cyrillic
				return RUSSIAN_CHARSET;
			
			case 0x041F:	// Turkish
				return TURKISH_CHARSET;
			
			case 0x0411:	// Japanese
				return SHIFTJIS_CHARSET;
			
			case 0x0804:	// Simplified Chinese
				return GB2312_CHARSET;
			
			case 0x0404:	// Traditional Chinese
				return CHINESEBIG5_CHARSET;
			
			case 0x0412:	// Korean
				return HANGEUL_CHARSET;
			
			case 0x041E:	// Thai
				return THAI_CHARSET;
			
			default:
				if (TranslateCharsetInfo((DWORD FAR*) GetACP(), &charsetInfo, TCI_SRCCODEPAGE))
					return (BYTE) charsetInfo.ciCharset;
				else
					return DEFAULT_CHARSET;
			}
		}
		else
			return lf.lfCharSet;
}
