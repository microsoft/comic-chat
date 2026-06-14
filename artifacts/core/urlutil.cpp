//=--------------------------------------------------------------------------=
// UrlRec.Cpp:		Implementation of the CUrlRec class
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

#include "UrlUtil.H"
#include "DlyLdDll.H"

#include "CDebug.H"
#include <wininet.h>


// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
//=--------------------------------------------------------------------------=
// CUrlRec::HrCanonicalizeUrl
//=--------------------------------------------------------------------------=
// checks if a string is a valid URL and canonicalize it
//
// Parameters:
//    LPCTSTR		- [in] potential URL string
//    LPDWORD		- [in]  the length of szURL, the string to test (does not include the terminating null).
//					- [out] the length of the canonical URL (does not include the terminating null) or
//							zero if the function fails
//    LPTSTR*		- [out] the address of the canonical URL if the function succeeds or NULL if it fails.
//							can be null if the caller foes not want the canonical form.
//
// Output:
//    HRESULT		NOERROR, S_FALSE, E_OUTOFMEMORY, E_FAIL
//
// Notes:
//		The caller is in charge of freeing *pszCanonURL if the function is successful
HRESULT CUrlRec::HrCanonicalizeUrl(LPCTSTR cszURL, LPDWORD pdwURLLength, LPTSTR *pszCanonURL)
{
	const nCanonExt = 16;

	ASSERT(pdwURLLength, "pdwURLLength is NULL in CUrlRec::HrCanonicalizeUrl");

	HRESULT	hr = NOERROR;
	BOOL	bRet;
	LPTSTR	szCanon = NULL;
	DWORD	dwCanonLength = *pdwURLLength + nCanonExt;

	szCanon = new TCHAR[dwCanonLength+1];
	if (!szCanon)
	{
		hr = E_OUTOFMEMORY;
		goto exit;
	}

	bRet = InternetCanonicalizeUrl(cszURL, szCanon, &dwCanonLength, 0L);
	if (!bRet)
	{
		switch (GetLastError())
		{
			case ERROR_INSUFFICIENT_BUFFER:
				delete [] szCanon;
				szCanon = new TCHAR[dwCanonLength+1];
				if (!szCanon)
				{
					hr = E_OUTOFMEMORY;
					goto exit;
				}
				bRet = InternetCanonicalizeUrl(cszURL, szCanon, &dwCanonLength, 0L);			
		}
	}

exit:
	if (bRet)
	{
		hr = NOERROR;
		if (pszCanonURL)
		{
			*pszCanonURL = szCanon;
			*pdwURLLength = dwCanonLength;
		}
		else
		{
			delete [] szCanon;
			*pdwURLLength = 0L;
		}
	}
	else
	{
		switch (GetLastError())
		{
			case ERROR_BAD_PATHNAME:
			case ERROR_INTERNET_INVALID_URL:
				hr = S_FALSE;
				break;
			default:
				hr = E_FAIL;
		}
		if (pszCanonURL)
			*pszCanonURL = NULL;
		*pdwURLLength = 0L;
	}
	return hr;
}


//=--------------------------------------------------------------------------=
// CUrlRec::bIsUrlPrefix
//=--------------------------------------------------------------------------=
// is a given string a URL prefix?
//
// Parameters:
//    LPCTSTR		- [in] string to test
//
// Output:
//    BOOL
//
// Notes:
//
BOOL CUrlRec::bIsUrlPrefix(LPCTSTR cszUrlPrefix, BOOL *pbForBrowser)
{
	//
	// Make sure that the stuff to the left of the colon is a KNOWN prefix
	//
	LPTSTR	pszURLPrefixs;
	int		isz;
	
	pszURLPrefixs = (LPTSTR) szURLPREFIXSBROWSER;
	
	for (isz = 0; NULL != pszURLPrefixs[isz]; isz += (_tcslen(pszURLPrefixs + isz) + 1))
	{
		//
		// URL protocol types are not case-sensitive
		//
		if (0 == _tcsicmp(cszUrlPrefix, &pszURLPrefixs[isz]))
		{
			if (pbForBrowser)
				*pbForBrowser = TRUE;
			return TRUE;
		}
	}	

	pszURLPrefixs = (LPTSTR) szURLPREFIXS;
	
	for (isz = 0; NULL != pszURLPrefixs[isz]; isz += (_tcslen(pszURLPrefixs + isz) + 1))
	{
		//
		// URL protocol types are not case-sensitive
		//
		if (0 == _tcsicmp(cszUrlPrefix, &pszURLPrefixs[isz]))
		{
			if (pbForBrowser)
				*pbForBrowser = FALSE;
			return TRUE;
		}
	}	

	return FALSE;
}


//=--------------------------------------------------------------------------=
// CUrlRec::bIsUrlSuffix
//=--------------------------------------------------------------------------=
// is a given string a URL suffix?
//
// Parameters:
//    LPTSTR		- [in] string to test
//
// Output:
//    BOOL			TRUE if string is a URL suffix
//
// Notes:
//
BOOL CUrlRec::bIsUrlSuffix(LPCTSTR cszUrlSuffix)
{
	BYTE	fb;
	LPCTSTR	pch;
	int		iEnd, iLast;

	if (!cszUrlSuffix || *cszUrlSuffix == chUrlEOS)
		return FALSE;

	iEnd	= 0;
	iLast	= 0;
	pch		= cszUrlSuffix;	
	for (pch; *pch; pch = CharNext(pch), ++iEnd)
	{
		if (iBytesofChar((BYTE) *pch) == 2)
			iLast = iEnd + 1;
		else
		{
			fb = bLegalForURL[(BYTE) *pch];

			if (!fb || !(fb & 0x01))
				//
				// Anything set to NULL in the Map is an illegal char
				//
				return FALSE;
			
			if (!(fb & 0x10))
				//
				// Not a terminating char
				//
				iLast = iEnd + 1;
		}
	}
	
	return (iLast != 1);
}


//=--------------------------------------------------------------------------=
// CUrlRec::FindPreceedingWord
//=--------------------------------------------------------------------------=
// finds working breaking preceeding a given point
//
// Parameters:
//    LPCTSTR		- [in] beginning of the string
//    LPCTSTR		- [in] starting point which is a colon (in a URL?)
//
// Output:
//    LPCTSTR		points to the beginning of the preceeding word
//
// Notes:
//			This routine searches backwards in a string, which in general is not OK 
//			for internationalization. However, based on the fact that URLs are ANSI,
//			this should produce fine results
LPCTSTR CUrlRec::FindPreceedingWord(LPCTSTR cszStart, LPCTSTR cszColon) 
{
	LPTSTR szTmp = CharPrev(cszStart, cszColon);

	while (szTmp >= cszStart && bIsUrlChar((const UCHAR) *szTmp) && !_istpunct(*szTmp)) szTmp--;
	
	return ++szTmp;
}


//=--------------------------------------------------------------------------=
// CUrlRec::FindURLEnd
//=--------------------------------------------------------------------------=
// finds the end of a potential URL
//
// Parameters:
//    LPCTSTR		- [in] starting point which is a colon (in a URL?)
//
// Output:
//    LPCTSTR		character after potential URL
//
// Notes:
//
LPCTSTR CUrlRec::FindUrlEnd(LPCTSTR cszColon) 
{
	LPCTSTR szEnd = cszColon;

	// REGISB removed quick fix on 12/12/97
	// Quick fix for Korean #1273
	//if (949 == GetACP())
	//{
	//	while (
	//		   (iBytesofChar((BYTE) *szEnd) == 1) && 
	//		   bIsUrlChar((const UCHAR) *szEnd)
	//		  )
	//		szEnd++;
	//}
	//else
	//{
	while (
		   (iBytesofChar((BYTE) *szEnd) == 2) || 
		   bIsUrlChar((const UCHAR) *szEnd)
		  )
		szEnd = CharNext(szEnd);
	//}

	while (*szEnd != chUrlBackSlash && *szEnd != chUrlSlash && // backup past trailing punctuation (we
		   _istpunct(*--szEnd) && szEnd > cszColon);	 // still accept trailing slash and backslash)

	return CharNext(szEnd);  // return character after URL
}


//=--------------------------------------------------------------------------=
// CUrlRec::HrIdentifyUrls
//=--------------------------------------------------------------------------=
// finds the potential URLs in a string and returns their boundaries and number
//
// Parameters:
//    LPCTSTR		- [in] message to treat
//
// Output:
//    SCODE			S_OK, CTL_E_OUTOFMEMORY
//
// Notes:
//
HRESULT CUrlRec::HrIdentifyUrls(LPCTSTR cszText, LPINT rgnUrlBounds, LPINT pnUrlNum)
{	
	BOOL	bPrefix;
	LPCTSTR cszStart = cszText;
	HRESULT	hr = NOERROR;
	INT		nUrlCnt = 0;

	ASSERT(rgnUrlBounds, "rgnUrlBounds is NULL in CUrlRec::HrIdentifyUrls");
	ASSERT(pnUrlNum, "pnUrlNum is NULL in CUrlRec::HrIdentifyUrls");

	while (TRUE)
	{
		LPTSTR szColon = _tcschr(cszStart, chUrlColon);
		if (szColon)
		{
			LPCTSTR cszWord = FindPreceedingWord(cszStart, szColon);

			*szColon = chUrlEOS;
			bPrefix = bIsUrlPrefix(cszWord, NULL);
			*szColon = chUrlColon;
			if (!bPrefix)
			{
				cszStart = szColon+1;
				continue;
			}

			LPCTSTR cszEnd = FindUrlEnd(szColon);

			DWORD	dwLen = cszEnd-cszWord;
			LPTSTR	szPossibleURL = new TCHAR[dwLen+1];

			if (!szPossibleURL)
			{
				*pnUrlNum = 0;
				return E_OUTOFMEMORY;
			}
			_tcsnccpy(szPossibleURL, cszWord, dwLen);
			szPossibleURL[dwLen] = chUrlEOS;

			if (ENSURE_LOADED(g_hinstWinInet, c_tszWinInetDLL))
			{
				hr = HrCanonicalizeUrl(szPossibleURL, &dwLen, NULL);
				switch (hr)
				{
					case NOERROR:
						if (nUrlCnt < *pnUrlNum)
						{
							//Optimize multiply by 2? - RamuM
							*(rgnUrlBounds+nUrlCnt*2)   = (INT) (cszWord - cszText);
							*(rgnUrlBounds+nUrlCnt*2+1) = (INT) (cszEnd  - cszText);
							nUrlCnt++;
						}
						else
						{
							// not enough room in rgnUrlBounds to hold all the Urls							
							delete [] szPossibleURL;
							ASSERT(*pnUrlNum == nUrlCnt, "*pnUrlNum != nUrlCnt in CUrlRec::HrIdentifyUrls");
							return S_FALSE;
						}
						break;
					case S_FALSE:
						break;
					default:
						delete [] szPossibleURL;
						*pnUrlNum = 0;
						return hr;
				}
			}
			else
			{
				if (bIsUrlSuffix(szPossibleURL+(szColon-cszWord)+1))
				{
					if (nUrlCnt < *pnUrlNum)
					{
						*(rgnUrlBounds+nUrlCnt)   = (INT) (cszWord - cszText);
						*(rgnUrlBounds+nUrlCnt+1) = (INT) (cszEnd  - cszText);
						nUrlCnt++;
					}
					else
					{
						// not enough room in rgnUrlBounds to hold all the Urls
						delete [] szPossibleURL;
						ASSERT(*pnUrlNum == nUrlCnt, "*pnUrlNum != nUrlCnt in CUrlRec::HrIdentifyUrls");
						return S_FALSE;
					}
				}
			}

			delete [] szPossibleURL;
			cszStart = cszEnd;
		} 
		else 
			break;
	}
	*pnUrlNum = nUrlCnt;
	return NOERROR;
}


//=--------------------------------------------------------------------------=
// CUrlRec::bLaunchUrl
//=--------------------------------------------------------------------------=
// checks if a new instance of the browser needs to be launched or not and
// then opens the Url
//
// Parameters:
//    HWND		- [in] parent window for the ShellExecute call
//    LPCTSTR	- [in] Url to launch
//
// Output:
//    BOOL			TRUE if the Url was successfully opened
//
// Notes:
//
BOOL CUrlRec::bLaunchUrl(HWND hwndParent, LPCTSTR cszUrl, BOOL bNewBrowser)
{
	HCURSOR		hCursor	= NULL;
	TCHAR		szExePath[MAX_PATH] = _T("");
	LPTSTR		szColon, szUrlTmp = NULL;
	UINT		uErr = 0;

	hCursor = ::SetCursor(::LoadCursor(NULL, IDC_APPSTARTING));

	if (!(szUrlTmp = _strdup(cszUrl)))
		goto exit;
	
	// Should be optimized with identify URLs- RamuM
	if (szColon = _tcschr(szUrlTmp, chUrlColon))
	{
		LPCTSTR cszWord = FindPreceedingWord(szUrlTmp, szColon);

		*szColon = chUrlEOS;
		BOOL bPrefix = bIsUrlPrefix(cszWord, NULL);
		if (!bPrefix)
			goto exit;

		*szColon = chUrlColon;
		if (!bIsUrlSuffix(szColon+1))
			goto exit;
	}
	else
		goto exit;

	if (bNewBrowser && bUrlNeedsNewBrowser(szUrlTmp, szExePath))
		uErr = (UINT) ShellExecute(hwndParent, NULL, szExePath, cszUrl, NULL, SW_SHOWNORMAL);
	else
		uErr = (UINT) ShellExecute(hwndParent, NULL, cszUrl, NULL, NULL, SW_SHOWNORMAL);

exit:
	if (hCursor)
		::SetCursor(hCursor);

	if (szUrlTmp)
		free(szUrlTmp);

	return uErr > 32;
}


BOOL CUrlRec::bUrlNeedsNewBrowser(LPCTSTR cszUrl, LPTSTR szExePath)
{
	const TCHAR szFilePrefix[] = "TOTO";
	const TCHAR szFileSuffix[] = ".HTM";
	const short MAX_HTMBUFF    = 32;

	HANDLE	hFile = NULL;
	TCHAR	szNumber[MAX_HTMBUFF];
	TCHAR	szWinDir[MAX_PATH];
	TCHAR	szHtmTmp[MAX_PATH+MAX_HTMBUFF];
	BOOL	bPrefix;
	BOOL	bForBrowser;
	UINT	uNumber = 1;
	UINT	uWinDirLen = 0;	
	UINT	uExe = 0;

	// does the url start with http:, https:, ftp: or gopher?
	LPTSTR	szColon = _tcschr(cszUrl, chUrlColon);

//	ASSERT(szColon, "szColon is NULL in CUrlRec::bUrlNeedsNewBrowser");

	if (!szColon)		// No Prefix, can not assume it's browser
		return(FALSE);	// Ramum, 4/8/97

	*szColon = chUrlEOS;
	bPrefix  = bIsUrlPrefix(cszUrl, &bForBrowser);
	*szColon = chUrlColon;
	
	ASSERT(bPrefix, "bPrefix is FALSE in CUrlRec::bUrlNeedsNewBrowser");

	if (!bForBrowser)
		return FALSE;

	if (*szExePath != chUrlEOS)
		return TRUE;

	if (0 == (uWinDirLen = GetWindowsDirectory((LPTSTR) szWinDir, MAX_PATH)))
		return FALSE;

	if (chUrlBackSlash != szWinDir[uWinDirLen-1])
		_tcscat(szWinDir, szUrlBackSlash);

	do
	{
		_tcscpy(szHtmTmp, szWinDir);
		_tcscat(szHtmTmp, szFilePrefix);
		_itot(uNumber, szNumber, 10);
		_tcscat(szHtmTmp, szNumber);
		_tcscat(szHtmTmp, szFileSuffix);

		uNumber *= 2;

		hFile = CreateFile(szHtmTmp, 
						  0L /*dwDesiredAccess*/, 
						  0L /*dwShareMode*/,
						  NULL /*lpSecurityAttributes*/,
						  CREATE_NEW /*dwCreationDistribution*/,
						  FILE_ATTRIBUTE_TEMPORARY /*dwFlagsAndAttributes*/,
						  NULL /*hTemplateFile*/);
	}
	while (INVALID_HANDLE_VALUE == hFile && uNumber <= 1024);

	if (INVALID_HANDLE_VALUE != hFile)
	{
		uExe = (UINT) FindExecutable(szHtmTmp, szWinDir, szExePath);
		CloseHandle(hFile);
		DeleteFile(szHtmTmp);
		return (uExe > 32);
	}
	else
		return FALSE;
}
