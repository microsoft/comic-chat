/*
	*****************************************************************
	*																*
	*	Module 			: CCommon.cpp  - Chat Common Code			*
	*																*
	*	Author 			: RegisB, 08/12/96							*
	*																*
	*	Current Owner	: RegisB									*
	*																*
	*	Changes			: Merged with CDebug.cpp, 08/26/96			*
	*																*
	*	To Dos			:											*
	*																*
	*****************************************************************
*/

#include "CCommon.H"
#include "CDebug.H"

// for ASSERT and FAIL
//
SZTHISFILE


// Determines if a string uses extended characters i.e. non ASCII
// characters above 127
BOOL bExtendedString(LPCTSTR szInStr)
{
	ASSERT(szInStr, "szInStr is NULL in bExtendedString");

	while (g_chEOS != *szInStr)
	{
		if (0x7F < (unsigned char) *szInStr)
			return TRUE;
		szInStr++;
	}
	return FALSE;
}


BOOL bExtendedWideString(LPCWSTR wszInStr)
{
	ASSERT(wszInStr, "wszInStr is NULL in bExtendedWideString");

	LPCWSTR	wszTmp = wszInStr;
	TCHAR	chFirst, chSecond;

	while (L'\0' != *wszTmp)
	{
		chFirst = *((LPCTSTR) wszTmp);
		chSecond = *(((LPCTSTR) wszTmp) + 1);
		if ((0x7F < (unsigned char) chFirst) ||
			(0 != chSecond))
			return TRUE;
		wszTmp++;
	}

	return FALSE;
}


BOOL bExtendedChannelName(LPCTSTR szChannelName, BOOL bAcceptUpperAnsi)
{
	LPCTSTR szTmp = szChannelName;

	ASSERT(szChannelName, "szChannelName is NULL in bExtendedChannelName");

	while (g_chEOS != *szTmp)
	{
		if ((0x7F < (unsigned char) *szTmp && !bAcceptUpperAnsi) ||
		    (*szTmp == g_chSpace) ||
			(*szTmp == g_chBell) ||
			(*szTmp == g_chCR) ||
			(*szTmp == g_chLF) ||
			(*szTmp == g_chComma))
			return TRUE;
		szTmp++;
	}
	return FALSE;
}


BOOL bExtendedWideChannelName(LPCWSTR wszChannelName)
{
	LPCWSTR	wszTmp = wszChannelName;
	TCHAR	chFirst, chSecond;
	
	ASSERT(wszChannelName, "wszChannelName is NULL in bExtendedWideChannelName");

	while (L'\0' != *wszTmp)
	{
		chFirst = *((LPCTSTR) wszTmp);
		chSecond = *(((LPCTSTR) wszTmp) + 1);
		if ((0x7F < (unsigned char) chFirst) ||
		    (chFirst == g_chSpace) ||
			(chFirst == g_chBell) ||
			(chFirst == g_chCR) ||
			(chFirst == g_chLF) ||
			(chFirst == g_chComma) ||
			(0 != chSecond))
			return TRUE;
		wszTmp++;
	}

	return FALSE;	
}


BOOL bExtendedNickname(LPCTSTR szNickname)
{
	LPCTSTR szTmp = szNickname;

	ASSERT(szNickname, "szNickname is NULL in bExtendedNickname");

	// the first character has to be a-z, or A-Z, or []{}_|`^
	if (!((*szTmp >= 'a' && *szTmp <= 'z') ||
		  (*szTmp >= 'A' && *szTmp <= 'Z') ||
		  (*szTmp == '[') ||
		  (*szTmp == ']') ||
		  (*szTmp == '{') ||
		  (*szTmp == '}') ||
		  (*szTmp == '_') ||
		  (*szTmp == '|') ||
		  (*szTmp == '`') ||
		  (*szTmp == '^')
		 )
	   )
		return TRUE;

	szTmp++;

	// for the rest of the nickname, we need a-z, A-Z, []{}_-|`^, or 0-9
	while (g_chEOS != *szTmp)
	{
		if (!((*szTmp >= 'a' && *szTmp <= 'z') ||
			  (*szTmp >= 'A' && *szTmp <= 'Z') ||
			  (*szTmp >= '0' && *szTmp <= '9') ||
			  (*szTmp == '[') ||
			  (*szTmp == ']') ||
			  (*szTmp == '{') ||
			  (*szTmp == '}') ||
			  (*szTmp == '_') ||
			  (*szTmp == '-') ||
			  (*szTmp == '|') ||
			  (*szTmp == '`') ||
			  (*szTmp == '^')
			 )
		   )
			return TRUE;
		szTmp++;
	}

	return FALSE;
}


BOOL bExtendedWideNickname(LPCWSTR wszNickname)
{
	LPCWSTR	wszTmp = wszNickname;
	TCHAR	chFirst, chSecond;
	
	ASSERT(wszNickname, "wszNickname is NULL in bExtendedWideNickname");

	chFirst = *((LPCTSTR) wszNickname);
	chSecond = *(((LPCTSTR) wszNickname) + 1);

	// the first character has to be a-z, or A-Z, or []{}_|`^
	if (!(
		  (chFirst >= 'a' && chFirst <= 'z') ||
		  (chFirst >= 'A' && chFirst <= 'Z') ||
		  (chFirst == '[') ||
		  (chFirst == ']') ||
		  (chFirst == '{') ||
		  (chFirst == '}') ||
		  (chFirst == '_') ||
		  (chFirst == '|') ||
		  (chFirst == '`') ||
		  (chFirst == '^')
		 ) ||
		(0 != chSecond)
	   )
		return TRUE;

	wszTmp++;

	// for the rest of the nickname, we need a-z, A-Z, []{}_-|`^, or 0-9
	while (L'\0' != *wszTmp)
	{
		chFirst = *((LPCTSTR) wszTmp);
		chSecond = *(((LPCTSTR) wszTmp) + 1);
		if (!(
			  (chFirst >= 'a' && chFirst <= 'z') ||
			  (chFirst >= 'A' && chFirst <= 'Z') ||
			  (chFirst >= '0' && chFirst <= '9') ||
			  (chFirst == '[') ||
			  (chFirst == ']') ||
			  (chFirst == '{') ||
			  (chFirst == '}') ||
			  (chFirst == '_') ||
			  (chFirst == '-') ||
			  (chFirst == '|') ||
			  (chFirst == '`') ||
			  (chFirst == '^')
			 ) ||
			(0 != chSecond)
		   )
			return TRUE;
		wszTmp++;
	}

	return FALSE;	
}


BOOL bConvertWideStringToUTF8(LPCWSTR wszInStr, INT cchIn, LPTSTR *pszOutStr, INT *pcchOut, BOOL bNickname, BOOL bChannelName, BOOL bPostProcess, BOOL bEscapeWildcards)
{
	ASSERT(wszInStr, "wszInStr in NULL in bConvertWideStringToUTF8");
	ASSERT(pszOutStr, "pszOutStr is NULL in bConvertWideStringToUTF8");
	ASSERT(!bNickname || !bChannelName, "Both bNickname and bChannelName set in bConvertWideStringToUTF8");

	LPCWSTR	wszTmpIn = wszInStr;
	LPTSTR	szTmpOut;
	INT		cchInL = cchIn ? cchIn : lstrlenW(wszInStr);
	INT		cchOutL = 0;

	*pszOutStr = NULL;

	if (pcchOut)
		*pcchOut = 0;

	ASSERT(cchInL > 0, "szInStr is empty in bConvertWideStringToUTF8");

	// Allocate tripple size buffer
	szTmpOut = *pszOutStr = (LPTSTR) new TCHAR[3*(cchInL+1)];

	if (!*pszOutStr)
		return FALSE;

	if (bNickname)
	{
		// Put a single quote as a prefix
		*szTmpOut++ = g_chExtNckPfx;
		cchOutL = 1;
	}
	else if (bChannelName)
	{
		// Put a '#', '&', '%#' or '%&' as a prefix
		if (*wszTmpIn == L'%')
		{
			*szTmpOut++ = g_chExtChnPfx;
			cchOutL = 1;
			wszTmpIn++;
		}
		else
		{
			*szTmpOut++ = g_chExtChnPfx;
			cchOutL = 1;
		}

		if (*wszTmpIn == L'#' || *wszTmpIn == L'&')
		{
			*szTmpOut++ = (TCHAR) *wszTmpIn;
			cchOutL++;
			wszTmpIn++;
		}
		else
		{
			// Add a '#' if prefix is missing
			*szTmpOut++ = g_chGblChnPfx;
			cchOutL++;
		}
	}

    while (*wszTmpIn)
    {
        if (bPostProcess && *wszTmpIn == L'\0')	// REGISB: Don't know if this can ever happen
        {
            *szTmpOut++ = '\\';
            *szTmpOut++ = '0';
            cchOutL += 2;
        }
        else if (bPostProcess && *wszTmpIn == L'\n')
        {
            *szTmpOut++ = '\\';
            *szTmpOut++ = 'n';
            cchOutL += 2;
        }
        else if (bPostProcess && *wszTmpIn == L'\r')
        {
            *szTmpOut++ = '\\';
            *szTmpOut++ = 'r';
            cchOutL += 2;
        }
        else if (bPostProcess && *wszTmpIn == L'\t')
        {
            *szTmpOut++ = '\\';
            *szTmpOut++ = 't';
            cchOutL += 2;
        }
        else if (bPostProcess && *wszTmpIn == L' ')
        {
            *szTmpOut++ = '\\';
            *szTmpOut++ = 'b';
            cchOutL += 2;
        }
        else if (bPostProcess && *wszTmpIn == L',')
        {
            *szTmpOut++ = '\\';
            *szTmpOut++ = 'c';
            cchOutL += 2;
        }
        else if (bPostProcess && *wszTmpIn == L'\\')
        {
            *szTmpOut++ = '\\';
            *szTmpOut++ = '\\';
            cchOutL += 2;
        }
		else if (bEscapeWildcards && *wszTmpIn == L'*')
		{
            *szTmpOut++ = '\\';
            *szTmpOut++ = '*';
            cchOutL += 2;
		}
		else if (bEscapeWildcards && *wszTmpIn == L'?')
		{
            *szTmpOut++ = '\\';
            *szTmpOut++ = '?';
            cchOutL += 2;
		}
        else if ((unsigned short) *wszTmpIn <= 0x7F)
        {
            *szTmpOut++ = (TCHAR) *wszTmpIn;
            cchOutL += 1;
        }
        else if ((unsigned short) *wszTmpIn <= 0x07FF)
        {
            *szTmpOut++ = 0xC0 | ((*wszTmpIn >> 6) & 0x1F);	// Kent's code says 0x03 instead
            *szTmpOut++ = 0x80 | (*wszTmpIn & 0x3F);
            cchOutL += 2;
        }
        else
        {
            *szTmpOut++ = 0xE0 | ((*wszTmpIn >> 12) & 0x0F);
            *szTmpOut++ = 0x80 | ((*wszTmpIn >> 6) & 0x3F);
            *szTmpOut++ = 0x80 | (*wszTmpIn & 0x3F);
			cchOutL += 3;
        }

        wszTmpIn++;
    }
	
	*szTmpOut = '\0';

	if (pcchOut)
		*pcchOut = cchOutL;

	return TRUE;
}


BOOL bConvertUTF8StringToWide(LPCTSTR szInStr, INT cchIn, LPWSTR *pwszOutStr, INT *pcchOut, BOOL bNickname, BOOL bChannelName, BOOL bPostProcess)
{
	ASSERT(szInStr, "szInStr in NULL in bConvertUTF8StringToWide");
	ASSERT(pwszOutStr, "pwszOutStr is NULL in bConvertUTF8StringToWide");
	ASSERT(!bNickname || !bChannelName, "Both bNickname and bChannelName set in bConvertUTF8StringToWide");

	LPCTSTR	szTmpIn = szInStr;
	LPWSTR	wszTmpOut;
	INT		cchInL = cchIn ? cchIn : lstrlen(szInStr);
	INT		cchOutL = 0;

	*pwszOutStr = NULL;

	if (pcchOut)
		*pcchOut = 0;

	ASSERT(cchInL > 0, "szInStr is empty in bConvertUTF8StringToWide");

	// Allocate same size buffer
	wszTmpOut = *pwszOutStr = (LPWSTR) new WCHAR[cchInL+1];

	if (!*pwszOutStr)
		return FALSE;

    // For nicknames, skip the initial single quote
	if (bNickname)
	{
		ASSERT(g_chExtNckPfx == *szInStr, "Nickname has no single quote prefix in bConvertUTF8StringToWide");
		szTmpIn++;
		cchInL--;
	}
	else if (bChannelName)
	{
		// Put a '%#' or '%&' as a prefix
		ASSERT(*szTmpIn == '%' || *szTmpIn == '#' || *szTmpIn == '&', "Unexpected channel prefix in bConvertUTF8StringToWide");

		if (*szTmpIn == g_chExtChnPfx)
		{
			szTmpIn++;
			cchInL--;
			*wszTmpOut++ = L'%';
	        cchOutL++;
		}

		if (*szTmpIn == g_chGblChnPfx || *szTmpIn == g_chLclChnPfx)
		{
			*wszTmpOut++ = (WCHAR) *szTmpIn;
	        cchOutL++;
			szTmpIn++;
			cchInL--;
		}
	}

    while (g_chEOS != *szTmpIn)
    {
        // The backslash is an escape character, convert back to raw.
        if (bPostProcess && *szTmpIn == '\\')
        {
            szTmpIn++;
			cchInL--;
 
            if (*szTmpIn == '0')		// REGISB: Don't think this can ever happen
            {
                *wszTmpOut++ = L'\0';
            }
            else if (*szTmpIn == 'n')
            {
                *wszTmpOut++ = L'\n';
            }
            else if (*szTmpIn == 'r')
            {
                *wszTmpOut++ = L'\r';
            }
            else if (*szTmpIn == 't')
            {
                *wszTmpOut++ = L'\t';
            }
            else if (*szTmpIn == 'b')
            {
                *wszTmpOut++ = L' ';
            }
            else if (*szTmpIn == 'c')
            {
                *wszTmpOut++ = L',';
            }
            else if (*szTmpIn == '\\')
            {
                *wszTmpOut++ = L'\\';
            }
        }
        else if ((unsigned short) *szTmpIn <= 0x7F)
        {
            *wszTmpOut++ = (WCHAR) *szTmpIn;
        }
        else if ((*szTmpIn & 0xE0) == 0xC0)
        {
            //  Must have at least two remaining characters in the string.
            if (cchInL >= 2)
            {								 // Kent's code uses 0x03 instead !?
                *wszTmpOut++ = ((szTmpIn[0] & 0x1F) << 6) | (szTmpIn[1] & 0x3F);

                szTmpIn++;
                cchInL--;
            }
            else
                *wszTmpOut++ = L'?';
        }
        else
        {
            //  Must have at least three remaining characters in the string.
            if (cchInL >= 3)
            {
                *wszTmpOut++ = ((szTmpIn[0] & 0x0F) << 12) | ((szTmpIn[1] & 0x3F) << 6) | (szTmpIn[2] & 0x3F);

                szTmpIn += 2;
                cchInL -= 2;
            }
            else
                *wszTmpOut++ = L'?';
        }

        // Skip to the next character.
        cchOutL++;
        szTmpIn++;
		cchInL--;
    }

	*wszTmpOut = L'\0';

	if (pcchOut)
		*pcchOut = cchOutL;

	return TRUE;
}


LPCTSTR SzNextUTF8Char(LPCTSTR szInStr)
{
	ASSERT(szInStr, "szInStr is NULL in SzNextUTF8Char");

	if (!*szInStr)
		return szInStr;

    if (*szInStr == '\\')
    {
		switch (*(szInStr+1))
		{
			case '0':
				return szInStr+1;

			case 'n':
			case 'r':
			case 't':
			case 'b':
			case 'c':
			case '\\':
				return szInStr+2;

			default:
				return szInStr+1;
        }
	}
    else
		if ((unsigned short) *szInStr <= 0x7F)
				return szInStr+1;
        else 
			if ((*szInStr & 0xE0) == 0xC0)
			{
				//  Should have at least two remaining characters in the string.
				if (*(szInStr+1))
					return szInStr+2;
				else
					return szInStr+1;
			}
	        else
		    {
				//  Should have at least three remaining characters in the string.
				if (*(szInStr+1) && *(szInStr+2))
					return szInStr+3;
				else
					return szInStr+1;
			}
}


BOOL bSB2DBKatakana(CHAR *szInStr, INT cchIn, CHAR **pszOutStr, INT *pcchOut, BOOL *pbFree)
{
	ASSERT(szInStr, "szInStr is NULL in bSB2DBKatakana");
	ASSERT(pszOutStr, "pszOutStr is NULL in bSB2DBKatakana");

	INT		cchInL = cchIn ? cchIn : lstrlen(szInStr);
	INT		cchOutL = 0, iRet;

	CHAR	*szNewStr = NULL;
	CHAR	*szTmp = szInStr;
	CHAR	*szSBKatakanaStart = NULL;	// start of a SB Katakana substring
	CHAR	*szNoneSBKatakanaStart = szInStr;

	*pszOutStr = NULL;

	if (pcchOut)
		*pcchOut = 0;

	if (pbFree)
		*pbFree = FALSE;

	if (cchInL <= 0)
		goto LUnchanged;

	while (*szTmp)
	{
		// find next SB Katakana substring
		while (*szTmp && ((UCHAR) *szTmp < 0xa1 || (UCHAR) *szTmp > 0xdf))
			szTmp = CharNext(szTmp);

		if (*szTmp)
		{
			// we found a beginning of SB Katakana substring
			szSBKatakanaStart = szTmp;
			if (!szNewStr)
			{
				szNewStr = new CHAR[2*cchInL+1];
				if (!szNewStr)
					return FALSE;
				*szNewStr = g_chEOS;
			}
			// append original string from szNoneSBKatakanaStart point to szSBKatakanaStart point
			cchOutL += szSBKatakanaStart - szNoneSBKatakanaStart;
			strncat(szNewStr, szNoneSBKatakanaStart, szSBKatakanaStart - szNoneSBKatakanaStart);

			// find end of SB Katakana substring
			while ((UCHAR) *szTmp >= 0xa1 && (UCHAR) *szTmp <= 0xdf)
				szTmp = CharNext(szTmp);

			szNoneSBKatakanaStart = szTmp;

			// append converted string from szSBKatakanaStart point to szNoneSBKatakanaStart point
			iRet = LCMapStringA(LOCALE_SYSTEM_DEFAULT,
								LCMAP_FULLWIDTH,
								szSBKatakanaStart,
								szNoneSBKatakanaStart - szSBKatakanaStart,
								szNewStr + cchOutL,
								2 * cchInL + 1 - cchOutL);
			ASSERT(iRet, "LCMapStringA failed in bSB2DBKatakana");
			cchOutL += iRet;
			szNewStr[cchOutL] = g_chEOS;
		}
	}

	if (szNewStr)
	{
		if (szNoneSBKatakanaStart)
			strcat(szNewStr, szNoneSBKatakanaStart);
		*pszOutStr = szNewStr;
		if (pcchOut)
			*pcchOut = cchOutL + lstrlen(szNoneSBKatakanaStart);
		if (pbFree)
			*pbFree = TRUE;
		return TRUE;
	}

LUnchanged:
	*pszOutStr = szInStr;
	if (pcchOut)
		*pcchOut = cchInL;
	return TRUE;
}


BOOL bConvertString(BOOL bIncomingText, BYTE byteCharSet, CHAR *szInStr, INT cchIn, CHAR **pszOutStr, INT *pcchOut, BOOL *pbFree)
{
	const UINT	uCP_EastEurUnicode	= 1250;
	const UINT	uCP_RussianUnicode	= 1251;
	const UINT	uCP_EastEurISO88592 = 28592;
	const UINT	uCP_RussianKOI8		= 20866;
	
	ASSERT(szInStr, "szInStr is NULL in bConvertString");
	ASSERT(pszOutStr, "pszOutStr is NULL in bConvertString");

	INT		cchInL = cchIn ? cchIn : lstrlen(szInStr);
	INT		cchOutL;
	UINT	uCP1 = uCP_RussianUnicode;
	UINT	uCP2 = uCP_RussianKOI8;

	*pszOutStr = NULL;

	if (pcchOut)
		*pcchOut = 0;

	if (pbFree)
		*pbFree = FALSE;

	if (cchInL <= 0)
		goto LUnchanged;

	switch (byteCharSet)
	{
		case ANSI_CHARSET:
			goto LUnchanged;

		case SHIFTJIS_CHARSET:
		{
			if (bIncomingText)
				cchOutL = OurJIS_to_ShiftJIS((UCHAR*) szInStr, cchInL+1, (UCHAR**) pszOutStr);
			else
				// REGISB 12/03/97 call bSB2DBKatakana if codepage is 932
				cchOutL = OurShiftJIS_to_JIS((UCHAR*) szInStr, cchInL+1, (UCHAR**) pszOutStr);

			if (cchOutL <= 0)
				// Jis <-> ShiftJis conversion failed, we just return identity
				goto LUnchanged;
			
			if (pcchOut)
				*pcchOut = cchOutL-1;
			
			break;
		}

		case EASTEUROPE_CHARSET:
		{
			uCP1 = uCP_EastEurUnicode;
			uCP2 = uCP_EastEurISO88592;
		}
		case RUSSIAN_CHARSET:
		{
			WCHAR	*szWideStr = (WCHAR*) new WCHAR[cchInL+1];
			INT		cchWideStr, cchSingleStr;

			if (!szWideStr)
				return FALSE;

			if (!(cchWideStr = MultiByteToWideChar(bIncomingText ? uCP2 : uCP1, 0L, szInStr, cchInL+1, szWideStr, cchInL+1)))
			{
				ASSERT(ERROR_INSUFFICIENT_BUFFER == GetLastError(), "Error != OOM in bConvertString");
				if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
				{
					delete [] szWideStr;
					return FALSE;
				}
				else
					goto LUnchanged;
			}

			*pszOutStr = (CHAR*) new CHAR[2*cchWideStr+1];

			if (!*pszOutStr)
			{
				delete [] szWideStr;
				return FALSE;
			}

			if (!(cchSingleStr = WideCharToMultiByte(bIncomingText ? uCP1 : uCP2, 0L, szWideStr, cchWideStr, *pszOutStr, 2*cchWideStr+1, NULL, NULL)))
			{
				ASSERT(ERROR_INSUFFICIENT_BUFFER == GetLastError(), "Error != OOM in bConvertString");
				if (ERROR_INSUFFICIENT_BUFFER == GetLastError())
				{
					delete [] szWideStr;
					delete [] *pszOutStr;
					*pszOutStr = NULL;
					return FALSE;
				}
				else
					goto LUnchanged;
			}

			if (pcchOut)
				*pcchOut = cchSingleStr;

			delete [] szWideStr;

			break;
		}

		default:
			goto LUnchanged;
	}

	if (pbFree)
		*pbFree = TRUE;

	return TRUE;


LUnchanged:
	*pszOutStr = szInStr;
	if (pcchOut)
		*pcchOut = cchInL;
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Encoding routine:
// pbSrc:		byte array to translate
// cbSrc:		number of bytes to translate
// pszDst:		pointer where resulting string should be placed
// bAfterColon:	TRUE is the resulting string is behind a colon
// Encoding rules:
//				'\0' -> "\0"
//				'\'  -> "\\"
//				'\n' -> "\n"
//				'\r' -> "\r"
//				'\t' -> "\t"
//				' '  -> "\b"
//				','  -> "\c"
// The memory allocation for the resulting string is done internally.
// The caller should deallocate the string if the function succeeds
// Function failure means an OOM condition
////////////////////////////////////////////////////////////////////////////
BOOL bDataToString(PBYTE pbSrc, UINT cbSrc, LPTSTR *pszDst, BOOL bAfterColon)
{
	ASSERT(pbSrc, "pbSrc is NULL in bDataToString");
	ASSERT(cbSrc, "cbSrc is NULL in bDataToString");
	ASSERT(pszDst, "pszDst is NULL in bDataToString");

	LPTSTR	szDst;
	LPTSTR	szTmp;
	UINT	cbTmp;

	*pszDst = NULL;

	if (!(szDst = new TCHAR[2*cbSrc+1]))	// Data might extend as much as twice its original size
		return FALSE;
	
	szTmp = szDst;
	for (cbTmp = 0; cbTmp < cbSrc; cbTmp++)
	{
		switch ((TCHAR) *pbSrc)
		{
			case g_chEOS:
				*szTmp++ = g_chBackSlash;
				*szTmp = '0';
				break;
			case g_chBackSlash:
				*szTmp++ = g_chBackSlash;
				*szTmp = g_chBackSlash;
				break;
			case g_chLF:
				*szTmp++ = g_chBackSlash;
				*szTmp = 'n';
				break;
			case g_chCR:
				*szTmp++ = g_chBackSlash;
				*szTmp = 'r';
				break;
			case g_chTab:
				*szTmp++ = g_chBackSlash;
				*szTmp = 't';
				break;
			case g_chSpace:
				if (bAfterColon)
					*szTmp = ' ';
				else
				{
					*szTmp++ = g_chBackSlash;
					*szTmp = 'b';
				}
				break;
			case g_chComma:
				if (bAfterColon)
					*szTmp = ',';
				else
				{
					*szTmp++ = g_chBackSlash;
					*szTmp = 'c';
				}
				break;
			default:
				*szTmp = (TCHAR) *pbSrc;
		}
		szTmp++;
		pbSrc++;
	}
	*szTmp = g_chEOS;

	*pszDst = szDst;
	return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Decoding routine:
// szSrc:	string to decode
// pbDst:	resulting byte array
// *pcbDst:	length in bytes of result
// Decoding rules:
//			"\0" -> '\0'
//			"\n" -> '\n'
//			"\r" -> '\r'
//			"\t" -> '\t'
//			"\b" -> ' '
//			"\c" -> ','
//			"\\" -> '\'
// The resulting block is at most as big as the original one, so we can 
// overwrite the original string, therefore the caller can set pbDst to szSrc
// No mem allocation is done in this function
// Function failure means that couldn't properly decode string
////////////////////////////////////////////////////////////////////////////
BOOL bStringToData(LPTSTR szSrc, PBYTE pbDst, UINT *pcbDst)
{
	LPTSTR	szRead = szSrc;
	LPTSTR	szWrite = (LPTSTR) pbDst;

	ASSERT(szSrc,   "szSrc  is NULL in bStringToData");
	ASSERT(pbDst,	"pbDst  is NULL in bStringToData");
	ASSERT(pcbDst,  "pcbDst is NULL in bStringToData");

	while (*szRead != g_chEOS)
	{
		if (g_chBackSlash == *szRead)
		{
			switch (*(szRead+1))
			{
				case '0':
					*szWrite = g_chEOS;
					break;
				case 'n':
					*szWrite = g_chLF;
					break;
				case 'r':
					*szWrite = g_chCR;
					break;
				case 't':
					*szWrite = g_chTab;
					break;
				case 'b':
					*szWrite = g_chSpace;
					break;
				case 'c':
					*szWrite = g_chComma;
					break;
				case g_chBackSlash:
					*szWrite = g_chBackSlash;
					break;
				default:
					// ASSERT(FALSE, "Unexpected string format in bStringToData");
					pbDst = NULL;
					*pcbDst = 0L;
					return FALSE;
			}
			szRead++;
		}
		else
			*szWrite = *szRead;

		szWrite++;
		szRead++;
	}
	*szWrite = g_chEOS;

	*pcbDst = (UINT) ((PBYTE) szWrite - pbDst);

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Encoding routine:
// szSrc:	string to encode
// pszDst:	pointer where resulting string should be placed
// pbFree:	set to TRUE if at least one character was quoted and the caller 
//          needs to free the resulting string	
// Encoding rules:
//				'<Q>'  -> "<Q><Q>"
//				'<Q>n' -> "<Q>n"
//				'<Q>r' -> "<Q>r"
// The memory allocation for the resulting string is done internally.
// The caller should deallocate the string if the function succeeds and 
// pbFree is set to TRUE
// Function failure means an OOM condition
////////////////////////////////////////////////////////////////////////////
BOOL bLowLevelQuoting(TCHAR chQuotingChar, BOOL bTreatAsByteArray, LPCTSTR szSrc, LPTSTR *pszDst, BOOL *pbFree, BOOL bRemoveCarriageReturns)
{
	ASSERT(szSrc,  "szSrc  is NULL in bLowLevelQuoting");
	ASSERT(pszDst, "pszDst is NULL in bLowLevelQuoting");
	ASSERT(pbFree, "pbFree is NULL in bLowLevelQuoting");

	LPTSTR	szTmpDst;
	LPCTSTR	szTmpSrcNC, szTmpSrc = szSrc;
	UINT	cToBeQuoted = 0, i;

	*pszDst = NULL;
	*pbFree = FALSE;

	while (g_chEOS != *szTmpSrc)
	{
		if (*szTmpSrc == chQuotingChar || *szTmpSrc == g_chLF || *szTmpSrc == g_chCR)
			cToBeQuoted++;
		szTmpSrc = bTreatAsByteArray ? szTmpSrc+1 : CharNext(szTmpSrc);
	}

	if (0 == cToBeQuoted)
	{
		*pszDst = (LPTSTR) szSrc;
		return TRUE;
	}

	if (!(*pszDst = new TCHAR[(szTmpSrc-szSrc) + cToBeQuoted + 1]))
		return FALSE;
	
	szTmpDst = *pszDst;
	szTmpSrc = szSrc;

	while (*szTmpSrc != g_chEOS)
	{
		switch (*szTmpSrc)
		{
			case g_chLF:
				*szTmpDst++ = chQuotingChar;
				*szTmpDst++ = 'n';
				szTmpSrc++;
				break;

			case g_chCR:
				if (!bRemoveCarriageReturns)
				{
					*szTmpDst++ = chQuotingChar;
					*szTmpDst++ = 'r';
				}
				szTmpSrc++;
				break;

			default:
				if (*szTmpSrc == chQuotingChar)
					*szTmpDst++ = chQuotingChar;
				szTmpSrcNC = bTreatAsByteArray ? szTmpSrc+1 : CharNext(szTmpSrc);
				for (i = 0; i < (UINT) (szTmpSrcNC - szTmpSrc); i++)
					*szTmpDst++ = *(szTmpSrc+i);
				szTmpSrc = szTmpSrcNC;
		}
	}
	*szTmpDst = g_chEOS;

	*pbFree = TRUE;

	return TRUE;
}


////////////////////////////////////////////////////////////////////////////
// Decoding routine:
// szSrc:	string to decode
// szDst:	resulting string
// Decoding rules:
//				"<Q><Q>" -> '<Q>'
//				"<Q>n" -> '<Q>n'
//				"<Q>r" -> '<Q>r'
// The resulting block is at most as big as the original one, so we can 
// overwrite the original string, therefore the caller can set szDst to szSrc
// No mem allocation is done in this function
// Function failure means that couldn't properly decode string
////////////////////////////////////////////////////////////////////////////
BOOL bLowLevelUnquoting(TCHAR chQuotingChar, BOOL bTreatAsByteArray, LPCTSTR szSrc, LPTSTR szDst)
{
	LPCTSTR	szReadNC, szRead  = szSrc;
	LPTSTR	szWrite = szDst;
	BOOL	bQuotedChar = FALSE;
	BOOL	bQuotedString = TRUE;
	UINT	i;

	ASSERT(szSrc, "szSrc  is NULL in bLowLevelUnquoting");
	ASSERT(szDst, "szDst  is NULL in bLowLevelUnquoting");

	// First check if the string seems to be low level quoted or not
	while (*szRead != g_chEOS)
	{
		if (chQuotingChar == *szRead)
		{
			switch (*(szRead+1))
			{
				case 'n':
				case 'r':
					bQuotedChar = TRUE;
					szRead++;
					break;

				default:
					if (chQuotingChar == *(szRead+1))
						szRead++;
					else
					{
						bQuotedString = FALSE;
						goto Unquote;
					}
			}
		}
		szRead = bTreatAsByteArray ? szRead+1 : CharNext(szRead);
	}

Unquote:
	if ((!bQuotedChar || !bQuotedString) && szDst == szSrc)
		return TRUE;

	szRead = szSrc;

	if (!bQuotedString)
	{
		while (*szRead != g_chEOS)
			*szWrite++ = *szRead++;
	}
	else
	{
		while (*szRead != g_chEOS)
		{
			if (chQuotingChar == *szRead)
			{
				switch (*(szRead+1))
				{
					case 'n':
						*szWrite = g_chLF;
						break;
					case 'r':
						*szWrite = g_chCR;
						break;
					default:
						if (chQuotingChar == *(szRead+1))
							*szWrite = chQuotingChar;
						else
						{
							ASSERT(FALSE, "Unexpected string format in bLowLevelUnquoting");
							szDst = NULL;
							return FALSE;
						}
				}
				szRead++;
			}
			else
			{
				szReadNC = bTreatAsByteArray ? szRead+1 : CharNext(szRead);
				for (i = 0; i < (UINT) (szReadNC - szRead); i++)
					*(szWrite+i) = *(szRead+i);
			}

			szWrite = bTreatAsByteArray ? szWrite+1 : CharNext(szWrite);
			szRead = bTreatAsByteArray ? szRead+1 : CharNext(szRead);
		}
	}

	*szWrite = g_chEOS;

	return TRUE;
}



#ifdef DEBUG
//
// contains various methods that will only really see any use in DEBUG builds
//

//=--------------------------------------------------------------------------=
// Private Constants
//---------------------------------------------------------------------------=
//
static const char szFormat[]  = "%s\nFile %s, Line %d";
static const char szFormat2[] = "%s\n%s\nFile %s, Line %d";

#define _SERVERNAME_ "ActiveX Framework"

static const char szTitle[]  = _SERVERNAME_ " Assertion  (Abort = UAE, Retry = INT 3, Ignore = Continue)";


//=--------------------------------------------------------------------------=
// Local functions
//=--------------------------------------------------------------------------=
int NEAR _IdMsgBox(LPSTR pszText, LPCSTR pszTitle, UINT mbFlags);

//=--------------------------------------------------------------------------=
// DisplayAssert
//=--------------------------------------------------------------------------=
// Display an assert message box with the given pszMsg, pszAssert, source
// file name, and line number. The resulting message box has Abort, Retry,
// Ignore buttons with Abort as the default.  Abort does a FatalAppExit;
// Retry does an int 3 then returns; Ignore just returns.
//
VOID DisplayAssert
(
    LPSTR	 pszMsg,
    LPSTR	 pszAssert,
    LPSTR	 pszFile,
    UINT	 line
)
{
    char	szMsg[250];
    LPSTR	lpszText;

    lpszText = pszMsg;		// Assume no file & line # info

    // If C file assert, where you've got a file name and a line #
    //
    if (pszFile) {

        // Then format the assert nicely
        //
        wsprintf(szMsg, szFormat, (pszMsg&&*pszMsg) ? pszMsg : pszAssert, pszFile, line);
        lpszText = szMsg;
    }

    // Put up a dialog box
    //
    switch (_IdMsgBox(lpszText, szTitle, MB_ICONHAND|MB_ABORTRETRYIGNORE|MB_SYSTEMMODAL)) {
        case IDABORT:
            FatalAppExit(0, lpszText);
            return;

        case IDRETRY:
            // call the win32 api to break us.
            //
            DebugBreak();
            return;
    }

    return;
}


//=---------------------------------------------------------------------------=
// Beefed-up version of WinMessageBox.
//=---------------------------------------------------------------------------=
//
int NEAR _IdMsgBox
(
    LPSTR	pszText,
    LPCSTR	pszTitle,
    UINT	mbFlags
)
{
    HWND hwndActive;
    int  id;

    hwndActive = GetActiveWindow();

    id = MessageBox(hwndActive, pszText, pszTitle, mbFlags);

    return id;
}


// Debug functions
TCHAR		g_szDebugStr[g_cbDebugStr];
CLock		g_cs;

void Cop(short n)
{
	_stprintf(g_szDebugStr, _T("Cop# %d Enter "), n);
	OutputDebugString(g_szDebugStr);
	HGLOBAL hCop;

	hCop = GlobalAlloc(GMEM_MOVEABLE, 4000);
	if (hCop)
		GlobalFree(hCop);
	OutputDebugString(_T("& Leave\n"));
}

void OutputDebugThreadIdString(char* szDebug)
{
	TCHAR szConcat[g_cbDebugStr];

	g_cs.Lock();

	_stprintf(szConcat, _T("%X: %s"), GetCurrentThreadId(), szDebug);
	OutputDebugString(szConcat);

	//Cop(0);
	
	g_cs.Unlock();

	return;
}

void EnterCriticalSectionDebug(LPCRITICAL_SECTION lpCriticalSection)
{
//	_stprintf(g_szDebugStr, _T("Entering CS %ld\n"), lpCriticalSection);
//	OutputDebugThreadIdString(g_szDebugStr);

	EnterCriticalSection(lpCriticalSection);
}

void LeaveCriticalSectionDebug(LPCRITICAL_SECTION lpCriticalSection)
{
//	_stprintf(g_szDebugStr, _T("Leaving CS %ld\n"), lpCriticalSection);
//	OutputDebugThreadIdString(g_szDebugStr);

	LeaveCriticalSection(lpCriticalSection);
}

#endif // DEBUG
