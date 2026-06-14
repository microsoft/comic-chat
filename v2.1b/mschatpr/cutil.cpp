/*
	*****************************************************************
	*																*
	*	Module 			: CUtil.cpp  - Chat Common Code				*
	*																*
	*	Author 			: RegisB, 08/12/96							*
	*																*
	*	Current Owner	: RegisB									*
	*																*
	*	Changes			: Merged with CDebug.cpp, 08/26/96			*
	*																*
	*****************************************************************
*/

#include "StdAfx.H"
#include "OleCtl.H"
#include "CUtil.H"
#include "CCError.H"
#include "CDebug.H"
#include "PrErr.H"
#include "PrCnst.H"
#include "PrRes.H"


// for ASSERT and FAIL
//
SZTHISFILE


BYTE byteSizeOfVariant(VARTYPE vt)
{
    switch (vt)
    {
		case VT_UI1:
		    return sizeof(char);
		case VT_I2:
		    return sizeof(short);
		case VT_I4:
			return sizeof(long);
		case VT_R4:  
		    return sizeof(float);
		case VT_R8:
		    return sizeof(double);
		case VT_BOOL:
			return sizeof(VARIANT_BOOL);
		case VT_ERROR:
			return sizeof(SCODE);
		case VT_CY:
			return sizeof(CY);
		case VT_DATE:
			return sizeof(DATE);
		default :
			return 0;
    }
}


////////////////////////////////////////////////////////////////////////////
// Converts a string that represents a decimal number into a long
////////////////////////////////////////////////////////////////////////////
LONG SzToL(LPCTSTR szStr)
{
	LONG	l = 0L;
	TCHAR	ch;

	ASSERT(szStr, "szStr is NULL in SzToL");

	while (1L)
	{
		ch = *szStr;
		if (g_chEOS != ch && '0' <= ch && '9' >= ch)
		{
			l *= 10;
			l += ch - '0';
		}
		else
			break;
		szStr++;
	}
	
	return l;
}


////////////////////////////////////////////////////////////////////////////
// Converts a string that represents an hexadecimal number into a long
////////////////////////////////////////////////////////////////////////////
LONG HexSzToL(LPCTSTR szStr)
{
	LONG	l = 0L;
	TCHAR	ch;

	ASSERT(szStr, "szStr is NULL in HexSzToL");

	while (1L)
	{
		ch = *szStr;
		if (g_chEOS != ch && '0' <= ch && '9' >= ch)
		{
			l *= 16;
			l += ch - '0';
		}
		else
			if (g_chEOS != ch && 'a' <= ch && 'f' >= ch)
			{
				l *= 16;
				l += 10 + ch - 'a';
			}
			else
				if (g_chEOS != ch && 'A' <= ch && 'F' >= ch)
				{
					l *= 16;
					l += 10 + ch - 'A';
				}
				else
					break;
		szStr++;
	}
	
	return l;
}


INT WStricmp(LPCWSTR wszStr1, LPCWSTR wszStr2)
{
	TCHAR	ch1, ch2;
	LPCWSTR	wszTmp1 = wszStr1, wszTmp2 = wszStr2;

	ASSERT(wszStr1, "wszStr1 is NULL in WStricmp");
	ASSERT(wszStr2, "wszStr2 is NULL in WStricmp");

	while (L'\0' != *wszTmp1 && L'\0' != *wszTmp2)
	{
		ch1 = (TCHAR) *wszTmp1;
		ch2 = (TCHAR) *wszTmp2;
		if (CharUpper((LPTSTR) ch1) != CharUpper((LPTSTR) ch2))
			return CharUpper((LPTSTR) ch1) - CharUpper((LPTSTR) ch2);
		wszTmp1++;
		wszTmp2++;
	}

	return CharUpper((LPTSTR) (TCHAR) *wszTmp1) - CharUpper((LPTSTR) (TCHAR) *wszTmp2);
}


////////////////////////////////////////////////////////////////////////////
// Is a string within another one?
// Returns the first occurrence of szStr2 in szStr1
////////////////////////////////////////////////////////////////////////////
LPTSTR SzSubStr(LPCTSTR szStr1, LPCTSTR szStr2)
{
	LPTSTR	szTmp1, szTmp2, szTmp3 = (LPTSTR) szStr1;

	if (!szStr1 || !szStr2)
		return NULL;

	while (*szTmp3 != g_chEOS)	// Could optimize this using the lenght of the strings
	{
		szTmp1 = szTmp3;
		szTmp2 = (LPTSTR) szStr2;
		while ((*szTmp1 != g_chEOS) && (*szTmp2 != g_chEOS) && (*szTmp1 == *szTmp2))
		{
			szTmp1++;
			szTmp2++;
		}
		if (*szTmp2 == g_chEOS)
			return szTmp3;
		szTmp3++;
	}

	return FALSE;
}


////////////////////////////////////////////////////////////////////////////
// Replaces all occurrences of a string by another in a string
// Returns the resulting string, or NULL if OOM
////////////////////////////////////////////////////////////////////////////
LPTSTR	SzReplaceStr(LPCTSTR szInStr, LPCTSTR szOldStr, LPCTSTR szNewStr)
{
	ASSERT(szInStr,  "szInStr  is NULL in SzReplaceStr");
	ASSERT(szOldStr, "szOldStr is NULL in SzReplaceStr");
	ASSERT(szNewStr, "szNewStr is NULL in SzReplaceStr");

	LPTSTR	szResult, szOccu, szOutTmp;
	LPCTSTR szInTmp = szInStr;

	INT		cbInStrLen  = lstrlen(szInStr);
	INT		cbOldStrLen = lstrlen(szOldStr);
	INT		cbNewStrLen = lstrlen(szNewStr);
	INT		cOccu = 0;

	if (cbNewStrLen > cbOldStrLen)
	{
		// The resulting string will probably be longer than the original one
		while (szOccu = SzSubStr(szInTmp, szOldStr))
		{
			cOccu++;
			szInTmp = szOccu+cbOldStrLen;
		}
		// Allocate enough to compensate the additional characters
		szResult = new TCHAR[cbInStrLen+1+(cbNewStrLen-cbOldStrLen)*cOccu];
	}
	else
		// The resulting string can't be longer than the original one
		// So we just allocate the same amount of memory
		szResult = new TCHAR[cbInStrLen+1];

	if (!szResult)
		return NULL;

	szInTmp = szInStr;
	szOutTmp = szResult;

	while (szOccu = SzSubStr(szInTmp, szOldStr))
	{
		CopyMemory((PVOID) szOutTmp, (const VOID*) szInTmp, (DWORD) (szOccu-szInTmp));
		szOutTmp += (szOccu-szInTmp);
		CopyMemory((PVOID) szOutTmp, (const VOID*) szNewStr, (DWORD) cbNewStrLen);
		szOutTmp += cbNewStrLen;
		szInTmp = szOccu+cbOldStrLen;
	}

	// Copy the remaining string
	CopyMemory((PVOID) szOutTmp, (const VOID*) szInTmp, (DWORD) (cbInStrLen - (szInTmp-szInStr) + 1));
	
	return szResult;
}


BOOL bCopyStr(LPTSTR *pszDest, LPCTSTR szSrc)
{
	ASSERT(pszDest, "pszDest is NULL in bCopyStr");
	ASSERT(szSrc, "szSrc is NULL in bCopyStr");

	INT cLen = lstrlen(szSrc);

	if (*pszDest == NULL)
		*pszDest = new TCHAR[cLen+1];
	else
		if (lstrlen(*pszDest) < cLen)
		{
			delete [] *pszDest;
			*pszDest = new TCHAR[cLen+1];
		}

	if (!*pszDest)
		return FALSE;
	else
	{
		lstrcpyn(*pszDest, szSrc, cLen+1);
		return TRUE;
	}
}


BOOL bCopyChanStr(LPTSTR *pszDest, LPCTSTR szSrc)
{
	ASSERT(pszDest, "pszDest is NULL in bCopyChanStr");
	ASSERT(szSrc, "szSrc is NULL in bCopyChanStr");

	INT cLen = lstrlen(szSrc);

	if (g_chGblChnPfx != *szSrc && g_chLclChnPfx != *szSrc && g_chExtChnPfx != *szSrc)
		// Need to add a '#' prefix
		cLen++;

	if (*pszDest == NULL)
		*pszDest = new TCHAR[cLen+1];
	else
		if (lstrlen(*pszDest) < cLen)
		{
			delete [] *pszDest;
			*pszDest = new TCHAR[cLen+1];
		}

	if (!*pszDest)
		return FALSE;
	else
	{
		LPTSTR	szDest = *pszDest;

		// Adding '#' if channel prefix is missing
		if (g_chGblChnPfx != *szSrc && g_chLclChnPfx != *szSrc && g_chExtChnPfx != *szSrc)
			*szDest++ = g_chGblChnPfx;
		lstrcpyn(szDest, szSrc, cLen+1);
		return TRUE;
	}
}


BOOL bOperateStr(LPTSTR *pszDest, SHORT nOpType, BOOL bExtendedNickname)
{
	LPTSTR	szTmp;
	DWORD	cLen;

	ASSERT(pszDest, "pszDest is NULL in bOperateStr");
	ASSERT(nOpType >= 0 && nOpType <= 2, "nOpType out of range in bOperateStr");

	if (!*pszDest)
		return bCopyStr(pszDest, "*");

	if (!(szTmp = new TCHAR[(cLen = lstrlen(*pszDest))+3]))
		return FALSE;

	switch (nOpType)
	{
	default:
		ASSERT(FALSE, "Unexpected operator type in in bOperateStr");
		return TRUE;

	case 0: // "Contains"
		if (bExtendedNickname)
		{
			ASSERT(g_chExtNckPfx == *pszDest[0], "Unexpected extended nickname in bOperateStr");
			szTmp[0] = g_chExtNckPfx;
			szTmp[1] = '*';
			lstrcpyn(szTmp+2, (*pszDest)+1, cLen);
		}
		else
		{
			szTmp[0] = '*';
			lstrcpyn(szTmp+1, *pszDest, cLen+1);
		}
		szTmp[cLen+1] = '*';
		szTmp[cLen+2] = g_chEOS;
		break;

	case 1: // "EndsWith"
		if (bExtendedNickname)
		{
			ASSERT(g_chExtNckPfx == *pszDest[0], "Unexpected extended nickname in bOperateStr");
			szTmp[0] = g_chExtNckPfx;
			szTmp[1] = '*';
			lstrcpyn(szTmp+2, (*pszDest)+1, cLen);
		}
		else
		{
			szTmp[0] = '*';
			lstrcpyn(szTmp+1, *pszDest, cLen+1);
		}
		szTmp[cLen+1] = g_chEOS;
		break;

	case 2: // "StartsWith"
		lstrcpyn(szTmp, *pszDest, cLen+1);
		szTmp[cLen] = '*';
		szTmp[cLen+1] = g_chEOS;
	}

	BOOL bRet = bCopyStr(pszDest, szTmp);
	delete [] szTmp;
	return bRet;
}


void FreePrChannelContent(PPRCHANNEL pPrChannel)
{
	if (!pPrChannel)
		return;

	if (pPrChannel->szName)
		delete [] pPrChannel->szName;
	if (pPrChannel->szTopic)
		delete [] pPrChannel->szTopic;
	if (pPrChannel->szKeyword)
		delete [] pPrChannel->szKeyword;
	if (pPrChannel->szHostKey)
		delete [] pPrChannel->szHostKey;
	if (pPrChannel->szOwnerKey)
		delete [] pPrChannel->szOwnerKey;
	if (pPrChannel->szRating)
		delete [] pPrChannel->szRating;
	if (pPrChannel->szAccount)
		delete [] pPrChannel->szAccount;
	if (pPrChannel->szClientData)
		delete [] pPrChannel->szClientData;
	if (pPrChannel->szClientGuid)
		delete [] pPrChannel->szClientGuid;
	if (pPrChannel->szServicePath)
		delete [] pPrChannel->szServicePath;
	if (pPrChannel->szOnJoin)
		delete [] pPrChannel->szOnJoin;
	if (pPrChannel->szOnPart)
		delete [] pPrChannel->szOnPart;
	if (pPrChannel->szLanguage)
		delete [] pPrChannel->szLanguage;
	if (pPrChannel->szSubject)
		delete [] pPrChannel->szSubject;

	if (pPrChannel->psaBannedList)
		SafeArrayDestroy(pPrChannel->psaBannedList);
	if (pPrChannel->psaCloneList)
		SafeArrayDestroy(pPrChannel->psaCloneList);

	ZeroMemory(pPrChannel, sizeof(PRCHANNEL));
}


void FreePrMemberContent(PPRMEMBER pPrMember)
{
	if (!pPrMember)
		return;

	if (pPrMember->szNickname)
		delete [] pPrMember->szNickname;
	if (pPrMember->szUserName)
		delete [] pPrMember->szUserName;
	if (pPrMember->szIPAddress)
		delete [] pPrMember->szIPAddress;

	ZeroMemory(pPrMember, sizeof(PRMEMBER));
}


void FreePrUserContent(PPRUSER pPrUser)
{
	if (!pPrUser)
		return;

	if (pPrUser->psaChannels)
		SafeArrayDestroy(pPrUser->psaChannels);
	if (pPrUser->psaChannelsOwner)
		SafeArrayDestroy(pPrUser->psaChannelsOwner);
	if (pPrUser->psaChannelsHost)
		SafeArrayDestroy(pPrUser->psaChannelsHost);
	if (pPrUser->psaChannelsVoice)
		SafeArrayDestroy(pPrUser->psaChannelsVoice);
	if (pPrUser->szNickname)
		delete [] pPrUser->szNickname;
	if (pPrUser->szRealName)
		delete [] pPrUser->szRealName;
	if (pPrUser->szUserName)
		delete [] pPrUser->szUserName;
	if (pPrUser->szIPAddress)
		delete [] pPrUser->szIPAddress;
	if (pPrUser->szServerName)
		delete [] pPrUser->szServerName;
	if (pPrUser->szServerInfo)
		delete [] pPrUser->szServerInfo;
	if (pPrUser->szAway)
		delete [] pPrUser->szAway;

	ZeroMemory(pPrUser, sizeof(PRUSER));
}


SHORT NGetChannelPropertyIndex(LPCTSTR szChannelPropertyName)
{
	SHORT nRet = 0;
	
	while (0 != lstrcmpi(g_rgszIrcxChannelPropertyNames[nRet], szChannelPropertyName) && nRet < g_nChannelPropertyNames)
		nRet++;

	return (nRet == g_nChannelPropertyNames) ? -1 : nRet;
}


SHORT NGetPropertyIndex(SHORT nPropType, LPCWSTR wszPropertyName)
{
	ASSERT(nPropType >= 0 && nPropType <= 4, "Unexpected at value in NGetPropertyIndex");
	ASSERT(wszPropertyName, "wszPropertyName is NULL in NGetPropertyIndex");

	//
	// binary search the appropriate property table
	//
	SHORT	nMiddle;
	SHORT	nStart, nEnd;	// search range
	SHORT	nRet;
	LPCWSTR *rgwszPropertyNames;
	
	nStart = 0;
	switch (nPropType)
	{
	case 0:
		nEnd = g_nUserPropertyNames - 1;
		rgwszPropertyNames = (LPCWSTR*) g_rgwszUserPropertyNames;
		break;
	case 1:
		nEnd = g_nMemberPropertyNames - 1;
		rgwszPropertyNames = (LPCWSTR*) g_rgwszMemberPropertyNames;
		break;
	case 2:
		nEnd = g_nChannelPropertyNames - 1;
		rgwszPropertyNames = (LPCWSTR*) g_rgwszChannelPropertyNames;
		break;
	case 3:
		nEnd = g_nServerPropertyNames - 1;
		rgwszPropertyNames = (LPCWSTR*) g_rgwszServerPropertyNames;
		break;
	case 4:
		nEnd = g_nQueryOperators - 1;
		rgwszPropertyNames = (LPCWSTR*) g_rgwszQueryOperators;
	}

	do
	{
		nMiddle = (nEnd - nStart)/2 + nStart;

		nRet = WStricmp(wszPropertyName, rgwszPropertyNames[nMiddle]);
		if (0 == nRet) // a match
			return nMiddle;
		
		if (nStart == nEnd)
			break;

		if (nRet < 0)
		{
			//
			// The property is less than
			//
			nEnd = nMiddle;
		}
		else
		{
			if (nMiddle != nStart)
				nStart = nMiddle;
			else
				nStart = nEnd;
		}
	}
	while (TRUE);
		
	return -1;	// not found
}


LPTSTR SzIdentityFromUserNameAndIPAddress(LPCTSTR szUserName, LPCTSTR szIPAddress)
{
	DWORD	cLen1 = lstrlen(szUserName);
	DWORD	cLen2 = lstrlen(szIPAddress);
	TCHAR*	szIdentity = new TCHAR[cLen1+cLen2+2];

	if (!szIdentity)
		return NULL;

	lstrcpyn(szIdentity, szUserName, cLen1+1);
	szIdentity[cLen1] = '@';
	szIdentity[cLen1+1] = g_chEOS;
	lstrcat(szIdentity, szIPAddress);

	return szIdentity;
}


////////////////////////////////////////////////////////////////////////////
// Encoding routine
// Input:	szSrc, the string to encode
// Output:	*pszDst, pointer to receive the newly allocated encoded string
// Encoding rules:
//			'\' -> "\\"
//			' ' -> "\b"
//			',' -> "\c"
//			'*' -> "\*"
//			'?' -> "\?"
// Return Value:	TRUE if resulting string could be allocated and encoded
//					FALSE if OOM
////////////////////////////////////////////////////////////////////////////
BOOL bEncodeMask(LPTSTR szSrc, LPTSTR *pszDst)
{
	ASSERT(szSrc, "szSrc  is NULL in bEncodeMask");
	ASSERT(pszDst,"pszDst is NULL in bEncodeMask");

	LPTSTR	szDst;
	LPTSTR	szTmp;

	*pszDst = NULL;

	if (!(szDst = new TCHAR[2*lstrlen(szSrc)+1]))	// String might extend as much as twice its original size
		return FALSE;
	
	szTmp = szDst;
	while(g_chEOS != *szSrc)
	{
		switch (*szSrc)
		{
			case '*':
				*szTmp++ = g_chBackSlash;
				*szTmp = '*';
				break;
			case '?':
				*szTmp++ = g_chBackSlash;
				*szTmp = '?';
				break;
			case g_chBackSlash:
				*szTmp++ = g_chBackSlash;
				*szTmp = g_chBackSlash;
				break;
			case g_chSpace:
				*szTmp++ = g_chBackSlash;
				*szTmp = 'b';
				break;
			case g_chComma:
				*szTmp++ = g_chBackSlash;
				*szTmp = 'c';
				break;
			default:
				*szTmp = *szSrc;
		}
		szTmp++;
		szSrc++;
	}
	*szTmp = g_chEOS;

	*pszDst = szDst;
	return TRUE;
}


WORD Sc2Sz(SCODE sc, LPTSTR szError, LPCTSTR szChannelName, LPCTSTR szNickname, BOOL bIrcX, BYTE byteCharSet)
{
	WORD	wIDS;
	LPTSTR	szChannelNameC = (LPTSTR) szChannelName;
	LPTSTR	szNicknameC = (LPTSTR) szNickname;
	BOOL	bFreeConverted = FALSE;

	if (sc >=  CPR_E_FAILURE && sc <= CPR_E_CHANOBJ_KEYNOTUNIQUE)
	{
		wIDS = (WORD) (SCODE_CODE(sc) + IDS_CPR_E_FAILURE - SCODE_CODE(CPR_E_FAILURE));
	}
	else
	switch (sc)
	{
		//		case CPR_E_<ZORG>:
		//			wIDS = IDS_CPR_E_<ZORG>;
		//			break;
		/*
		case CPR_E_FAILURE:
			wIDS = IDS_CPR_E_FAILURE;
			break;
		case CPR_E_INVALIDARG:
			wIDS = IDS_CPR_E_INVALIDARG;
			break;
		//
		// Connection errors
		//
		case CPR_E_ALREADYCONNECTING:
			wIDS = IDS_CPR_E_ALREADYCONNECTING;
			break;
		case CPR_E_ALREADYCONNECTED:
			wIDS = IDS_CPR_E_ALREADYCONNECTED;
			break;
		case CPR_E_ALREADYDISCONNECTING:
			wIDS = IDS_CPR_E_ALREADYDISCONNECTING;
			break;
		case CPR_E_NOOPINPROGRESS:
			wIDS = IDS_CPR_E_NOOPINPROGRESS;
			break;
		case CPR_E_DISCONNECTFIRST:
			wIDS = IDS_CPR_E_DISCONNECTFIRST;
			break;
		case CPR_E_NOTCONNECTED:
			wIDS = IDS_CPR_E_NOTCONNECTED;
			break;
		case CPR_E_HOSTNOTFOUND:
			wIDS = IDS_CPR_E_HOSTNOTFOUND;
			break;
		case CPR_E_SOCKETCREATE:
			wIDS = IDS_CPR_E_SOCKETCREATE;
			break;
		case CPR_E_CANTCONNECT:
			wIDS = IDS_CPR_E_CANTCONNECT;
			break;
		case CPR_E_SOCKETERROR:
			wIDS = IDS_CPR_E_SOCKETERROR;
			break;
		case CPR_E_INVALIDSOCKET:
			wIDS = IDS_CPR_E_INVALIDSOCKET;
			break;
		case CPR_E_LOSTCONNECTION:
			wIDS = IDS_CPR_E_LOSTCONNECTION;
			break;
		case CPR_E_SOCKETCLOSED:
			wIDS = IDS_CPR_E_SOCKETCLOSED;
			break;
		case CPR_E_NETWORKDOWN:
			wIDS = IDS_CPR_E_NETWORKDOWN;
			break;
		case CPR_E_HOSTDROPPEDCONNECTION:
			wIDS = IDS_CPR_E_HOSTDROPPEDCONNECTION;
			break;
		//
		// Login errors
		//
		case CPR_E_ALREADYLOGGING:
			wIDS = IDS_CPR_E_ALREADYLOGGING;
			break;
		case CPR_E_ALREADYLOGGED:
			wIDS = IDS_CPR_E_ALREADYLOGGED;
			break;
		case CPR_E_NOTLOGGED:
			wIDS = IDS_CPR_E_NOTLOGGED;
			break;
		case CPR_E_ALREADYREGISTERED:
			wIDS = IDS_CPR_E_ALREADYREGISTERED;
			break;
		case CPR_E_NOTREGISTERED:
			wIDS = IDS_CPR_E_NOTREGISTERED;
			break;
		case CPR_E_BADSECUPACK:
			wIDS = IDS_CPR_E_BADSECUPACK;
			break;
		case CPR_E_LOGINFAILED:
			wIDS = IDS_CPR_E_LOGINFAILED;
			break;
		case CPR_E_AUTHFAILED:
			wIDS = IDS_CPR_E_AUTHFAILED;
			break;
		case CPR_E_AUTHNOTAVAIL:
			wIDS = IDS_CPR_E_AUTHNOTAVAIL;
			break;
		case CPR_E_AUTHENTICATEDONLY:
			wIDS = IDS_CPR_E_AUTHENTICATEDONLY;
			break;
		//
		// Channel Errors
		//
		case CPR_E_ALREADYOPEN:
			wIDS = IDS_CPR_E_ALREADYOPEN;
			break;
		case CPR_E_ALREADYOPENING:
			wIDS = IDS_CPR_E_ALREADYOPENING;
			break;
		case CPR_E_NOTOPEN:
			wIDS = IDS_CPR_E_NOTOPEN;
			break;
		case CPR_E_NOTCLOSED:
			wIDS = IDS_CPR_E_NOTCLOSED;
			break;
		case CPR_E_OPENNOTUNIQUE:
			wIDS = IDS_CPR_E_OPENNOTUNIQUE;
			break;
		case CPR_E_CHANNELBADNAME:
			wIDS = IDS_CPR_E_CHANNELBADNAME;
			break;
		case CPR_E_CHANNELBADTOPIC:
			wIDS = IDS_CPR_E_CHANNELBADTOPIC;
			break;
		case CPR_E_CHANNELBADKEY:
			wIDS = IDS_CPR_E_CHANNELBADKEY;
			break;
		case CPR_E_CHANNELBADHOSTKEY:
			wIDS = IDS_CPR_E_CHANNELBADHOSTKEY;
			break;
		case CPR_E_CHANNELBADOWNERKEY:
			wIDS = IDS_CPR_E_CHANNELBADOWNERKEY;
			break;
		case CPR_E_CHANNELBADMODES:
			wIDS = IDS_CPR_E_CHANNELBADMODES;
			break;
		case CPR_E_BADMAXMEMBERCOUNT:
			wIDS = IDS_CPR_E_BADMAXMEMBERCOUNT;
			break;
		case CPR_E_CHANNELFULL:
			wIDS = IDS_CPR_E_CHANNELFULL;
			break;
		case CPR_E_TOOMANYCHANNELS:
			wIDS = IDS_CPR_E_TOOMANYCHANNELS;
			break;
		case CPR_E_NOTONCHANNEL:
			wIDS = IDS_CPR_E_NOTONCHANNEL;
			break;
		case CPR_E_ALREADYONCHANNEL:
			wIDS = IDS_CPR_E_ALREADYONCHANNEL;
			break;
		case CPR_E_NOSUCHCHANNEL:
			wIDS = IDS_CPR_E_NOSUCHCHANNEL;
			break;
		case CPR_E_INVITEONLYCHANNEL:
			wIDS = IDS_CPR_E_INVITEONLYCHANNEL;
			break;
		case CPR_E_CHANNELKEYALREADYSET:
			wIDS = IDS_CPR_E_CHANNELKEYALREADYSET;
			break;

		//
		// Property Errors
		// 
		case CPR_E_BADPROPERTY:
			wIDS = IDS_CPR_E_BADPROPERTY;
			break;
		case CPR_E_BADVALUE:
			wIDS = IDS_CPR_E_BADVALUE;
			break;
		case CPR_E_PROPNOTREADABLE:
			wIDS = IDS_CPR_E_PROPNOTREADABLE;
			break;
		case CPR_E_PROPREADONLY:
			wIDS = IDS_CPR_E_PROPREADONLY;
			break;
		case CPR_E_PROPNOTWRITEABLE:
			wIDS = IDS_CPR_E_PROPNOTWRITEABLE;
			break;
		//
		// Member Errors
		// 
		case CPR_E_NOSUCHMEMBER:
			wIDS = IDS_CPR_E_NOSUCHMEMBER;
			break;
		case CPR_E_MEMBERBADMODES:
			wIDS = IDS_CPR_E_MEMBERBADMODES;
			break;
		//
		// User Errors
		//
		case CPR_E_BADNICKNAME:
			wIDS = IDS_CPR_E_BADNICKNAME;
			break;
		case CPR_E_BADUSERNAME:
			wIDS = IDS_CPR_E_BADUSERNAME;
			break;
		case CPR_E_BADREALNAME:
			wIDS = IDS_CPR_E_BADREALNAME;
			break;
		case CPR_E_BADPASSWORD:
			wIDS = IDS_CPR_E_BADPASSWORD;
			break;
		case CPR_E_NICKNAMEINUSE:
			wIDS = IDS_CPR_E_NICKNAMEINUSE;
			break;
		case CPR_E_NICKCOLLISION:
			wIDS = IDS_CPR_E_NICKCOLLISION;
			break;
		case CPR_E_NOSUCHNICK:
			wIDS = IDS_CPR_E_NOSUCHNICK;
			break;
		case CPR_E_USERBADMODES:
			wIDS = IDS_CPR_E_USERBADMODES;
			break;
		case CPR_E_NICKTOOFAST:
			wIDS = IDS_CPR_E_NICKTOOFAST;
			break;
		case CPR_E_NICKNOCHANGE:
			wIDS = IDS_CPR_E_NICKNOCHANGE;
			break;

		//
		// Sending
		//
		case CPR_E_TOOMUCHDATA:
			wIDS = IDS_CPR_E_TOOMUCHDATA;
			break;
		case CPR_E_CANTSEND:
			wIDS = IDS_CPR_E_CANTSEND;
			break;
		case CPR_E_INVALIDRECIPIENTLIST:
			wIDS = IDS_CPR_E_INVALIDRECIPIENTLIST;
			break;
		case CPR_E_BADREASON:
			wIDS = IDS_CPR_E_BADREASON;
			break;
		case CPR_E_INVALIDAWAYMESSAGE:
			wIDS = IDS_CPR_E_INVALIDAWAYMESSAGE;
			break;
		case CPR_E_NULLMSG:
			wIDS = IDS_CPR_E_NULLMSG;
			break;
		case CPR_E_BADMSGTYPE:
			wIDS = IDS_CPR_E_BADMSGTYPE;
			break;
		case CPR_E_NOWHISPER:
			wIDS = IDS_CPR_E_NOWHISPER;
			break;
		case CPR_E_BADTAG:
			wIDS = IDS_CPR_E_BADTAG;
			break;

		//
		// Protocol Errors
		//
		case CPR_E_SERVER:
			wIDS = IDS_CPR_E_SERVER;
			break;
		case CPR_E_NOTIRCX:
			wIDS = IDS_CPR_E_NOTIRCX;
			break;
		case CPR_E_PROPMODE:
			wIDS = IDS_CPR_E_PROPMODE;
			break;
		case CPR_E_NOMOTD:
			sc = IDS_CPR_E_NOMOTD;
			break;
		case CPR_E_UNKNOWNCOMMAND:
			wIDS = IDS_CPR_E_UNKNOWNCOMMAND;
			break;

		//
		// Access Rights errors
		//
		case CPR_E_NOTSYSOP:
			wIDS = IDS_CPR_E_NOTSYSOP;
			break;
		case CPR_E_NOTOWNER:
			wIDS = IDS_CPR_E_NOTOWNER;
			break;
		case CPR_E_NOTHOST:
			wIDS = IDS_CPR_E_NOTHOST;
			break;
		case CPR_E_RATINGBLOCK:
			wIDS = IDS_CPR_E_RATINGBLOCK;
			break;
		case CPR_E_BANNEDFROMCHANNEL:
			wIDS = IDS_CPR_E_BANNEDFROMCHANNEL;
			break;
		case CPR_E_BANNEDFROMSERVER:
			wIDS = IDS_CPR_E_BANNEDFROMSERVER;
			break;
		case CPR_E_WILLBEBANNED:
			wIDS = IDS_CPR_E_WILLBEBANNED;
			break;
		case CPR_E_CANTIGNORE:
			wIDS = IDS_CPR_E_CANTIGNORE;
			break;
		case CPR_E_NOJOINMICONLY:
			wIDS = IDS_CPR_E_NOJOINMICONLY;
			break;
		case CPR_E_NOJOINREMOTE:
			wIDS = IDS_CPR_E_NOJOINREMOTE;
			break;
		case CPR_E_NOJOINDYNAMIC:
			wIDS = IDS_CPR_E_NOJOINDYNAMIC;
			break;
		case CPR_E_NODYNAMICCHANNELS:
			wIDS = IDS_CPR_E_NODYNAMICCHANNELS;
			break;
		case CPR_E_AUTHONLY:
			wIDS = IDS_CPR_E_AUTHONLY;
			break;
		case CPR_E_OVERFLOWABORT:
			wIDS = IDS_CPR_E_OVERFLOWABORT;
			break;
		case CPR_E_CANTCHANGEUSERMODE:
			wIDS = IDS_CPR_E_CANTCHANGEUSERMODE;
			break;

		//
		// ChatItems related errors
		//
		case CPR_E_ITEMNOTAVAILABLE:
			wIDS = IDS_CPR_E_ITEMNOTAVAILABLE;
			break;
		case CPR_E_ITEMNAMENA:
			wIDS = IDS_CPR_E_ITEMNAMENA;
			break;
		case CPR_E_INVALIDASSOCIATEDTYPE:
			wIDS = IDS_CPR_E_INVALIDASSOCIATEDTYPE;
			break;
		case CPR_E_INVALIDITEMNAME:
			wIDS = IDS_CPR_E_INVALIDITEMNAME;
			break;
		case CPR_E_INVALIDITEMVALUE:
			wIDS = IDS_CPR_E_INVALIDITEMVALUE;
			break;
		case CPR_E_NOTOPERATOR:
			wIDS = IDS_CPR_E_NOTOPERATOR;
			break;
		case CPR_E_TIMEOUT:
			wIDS = IDS_CPR_E_TIMEOUT;
			break;

		//
		// Channel Collection Errors
		//
		case CPR_E_CHANOBJ_BADINDEX:
			wIDS = IDS_CPR_E_CHANOBJ_BADINDEX;
			break;
		case CPR_E_CHANOBJ_INDEXOOB:
			wIDS = IDS_CPR_E_CHANOBJ_INDEXOOB;
			break;
		case CPR_E_CHANOBJ_DELETED:
			wIDS = IDS_CPR_E_CHANOBJ_DELETED;
			break;
		case CPR_E_CHANOBJ_BADKEY:
			wIDS = IDS_CPR_E_CHANOBJ_BADKEY;
			break;
		case CPR_E_CHANOBJ_KEYNOTUNIQUE:
			wIDS = IDS_CPR_E_CHANOBJ_KEYNOTUNIQUE;
			break;
		*/

		case CTL_E_OUTOFMEMORY:
			// "Out of memory"
			wIDS = IDS_CTL_E_OUTOFMEMORY;
			break;
		case CTL_E_SETNOTSUPPORTED:
			// "Property is read-only"
			wIDS = IDS_CTL_E_SETNOTSUPPORTED;
			break;
		case CTL_E_SETNOTSUPPORTEDATRUNTIME:
			// "Property is read-only at runtime."
			wIDS = IDS_CTL_E_SETNOTSUPPORTEDATRUNTIME;
			break;
		case CTL_E_GETNOTSUPPORTED:
			// "Property is write-only"
			wIDS = IDS_CTL_E_GETNOTSUPPORTED;
			break;
		case CTL_E_INVALIDPROPERTYVALUE:
			// "Trying to set an invalid property value"
			wIDS = IDS_CTL_E_INVALIDPROPERTYVALUE;
			break;
		case CTL_E_PERMISSIONDENIED:
			// "Permission is denied"
			wIDS = IDS_CTL_E_PERMISSIONDENIED;
			break;
		case CTL_E_OVERFLOW:
			// "Overflow error"
			wIDS = IDS_CTL_E_OVERFLOW;
			break;
		case CTL_E_ILLEGALFUNCTIONCALL:
			// "Illegal function call"
			wIDS = IDS_CTL_E_ILLEGALFUNCTIONCALL;
			break;

		default:
			// "General failure"
			wIDS = IDS_CPR_E_FAILURE;
	}

	USES_CONVERSION;

	if (szChannelName && bIrcX)
	{
		if (g_chExtChnPfx == *szChannelName)
		{
			// Extended channel name that needs to be converted from UTF8
			LPWSTR wszChannelName;
			if (bConvertUTF8StringToWide(szChannelName, 0 /*cchIn*/, &wszChannelName, NULL /*pcchOut*/, FALSE, TRUE))
			{
				ASSERT(wszChannelName, "wszChannelName in NULL in Sc2Sz");
				szChannelNameC = W2T(wszChannelName);
				delete [] wszChannelName;
			}
		}
		else
		{
			// Might have to convert from Jis to ShiftJis, etc...
			bConvertString(TRUE, byteCharSet, (LPTSTR) szChannelName, 0, &szChannelNameC, NULL, &bFreeConverted);
		}
	}

	if (szNickname && g_chExtNckPfx == *szNickname)
	{
		// Extended nickname that needs to be converted from UTF8
		LPWSTR wszNickname;
		if (bConvertUTF8StringToWide(szNickname, 0 /*cchIn*/, &wszNickname, NULL /*pcchOut*/, TRUE /*bNickname*/))
		{
			ASSERT(wszNickname, "wszNickname in NULL in Sc2Sz");

			szNicknameC = W2T(wszNickname);
			delete [] wszNickname;
		}
	}		 

	if (szError)
	{
		TCHAR		szUnknown[32];
		LPCTSTR*	pszArgs = NULL;
		LPTSTR		szText  = NULL;

		if (!::LoadString(_Module.GetModuleInstance(), IDS_UNKNOWN, szUnknown, 31)		 ||
			!::LoadString(_Module.GetModuleInstance(), wIDS, szError, g_nMaxLengthError) || 
			wIDS == IDS_CTL_E_OUTOFMEMORY)
		{
			lstrcpy(szError, g_szOOM);
			return IDS_CTL_E_OUTOFMEMORY;
		}

		if (SzSubStr(szError, "%1"))
		{
			if (SzSubStr(szError, "%2"))
			{
				// The error string contains both %1 and %2
				pszArgs = new LPCTSTR[2];
				pszArgs[0] = szChannelNameC ? szChannelNameC : szUnknown;
				pszArgs[1] = szNicknameC ? szNicknameC : szUnknown;
			}
			else
			{
				// The error string contains a %1
				pszArgs = new LPCTSTR[1];
				pszArgs[0] = szChannelNameC ? szChannelNameC : szUnknown;
			}
		}
		else
		{
			if (SzSubStr(szError, "%2"))
			{
				// The error string contains a %2
				pszArgs = new LPCTSTR[2];
				pszArgs[0] = g_szEmpty;
				pszArgs[1] = szNicknameC ? szNicknameC : szUnknown;
			}
		}
		if (pszArgs)
		{
			if (!bCopyStr(&szText, szError))
			{
				delete pszArgs;
				lstrcpy(szError, g_szOOM);
				return IDS_CTL_E_OUTOFMEMORY;
			}

			if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
							   szText, 0, 0, szError, g_nMaxLengthError, (LPTSTR*) pszArgs))
			{
				ASSERT(FALSE, "Not enough space available in Sc2Sz");
				szError[0] = g_chEOS;
			}

			delete [] pszArgs;
			delete [] szText;
		}
	}

	if (bFreeConverted && szChannelNameC)
		delete [] szChannelNameC;

	return wIDS;
}


//=--------------------------------------------------------------------------=
// GetScodeFromHResult
//=--------------------------------------------------------------------------=
// gets the OCX scode associated to a hresult
//
// Parameters:
//    HRESULT				- [in]  the hresult
//
// Output:
//    SCODE
//
// Notes:
//    
SCODE GetScodeFromHResult(HRESULT hr)
{
	SCODE	sc = S_OK;

	switch (hr)
	{
		case NOERROR:		// or ERROR_SUCCESS:
			sc = S_OK;
			break;

		case S_FALSE:
			sc = S_FALSE;
			break;

		case E_NOTIMPL:
		case E_UNEXPECTED:
			sc = CPR_E_FAILURE;
			break;

		case E_OUTOFMEMORY:
		case ERROR_OUTOFMEMORY:
			sc = CTL_E_OUTOFMEMORY;
			break;

		case E_INVALIDARG:
		case ERROR_INVALID_PARAMETER:
			sc = CPR_E_INVALIDARG;
			break;

		case E_FAIL:
			sc = CPR_E_FAILURE;
			break;

		//
		// Internal Errors
		//
		case CC_E_BUFFERTOOSMALL:
		case CC_E_NOTQUERIEDOBJECT:
			ASSERT(FALSE, "Unexpected internal error in GetScodeFromHResult");
			sc = CPR_E_FAILURE;
			break;

		//
		// Connection errors
		//
		case CC_E_ALREADYCONNECTING:
			sc = CPR_E_ALREADYCONNECTING;
			break;
		case CC_E_ALREADYCONNECTED:
			sc = CPR_E_ALREADYCONNECTED;
			break;
		case CC_E_ALREADYDISCONNECTING:
			sc = CPR_E_ALREADYDISCONNECTING;
			break;
		case CC_E_NOOPINPROGRESS:
			sc = CPR_E_NOOPINPROGRESS;
			break;
		case CC_E_DISCONNECTFIRST:
			sc = CPR_E_DISCONNECTFIRST;
			break;
		case CC_E_NOTCONNECTED:
			sc = CPR_E_NOTCONNECTED;
			break;
		case CC_E_HOSTNOTFOUND:
			sc = CPR_E_HOSTNOTFOUND;
			break;
		case CC_E_SOCKETCREATE:
			sc = CPR_E_SOCKETCREATE;
			break;
		case CC_E_CANTCONNECT:
			sc = CPR_E_CANTCONNECT;
			break;
		case CC_E_NOSOCKETDATA:
			sc = CPR_E_FAILURE;
			break;
		case CC_E_SOCKETERROR:
			sc = CPR_E_SOCKETERROR;
			break;
		case CC_E_LOSTCONNECTION:
			sc = CPR_E_LOSTCONNECTION;
			break;
		case CC_E_SOCKETCLOSED:
			sc = CPR_E_SOCKETCLOSED;
			break;
		case CC_E_NETWORKDOWN:
			sc = CPR_E_NETWORKDOWN;
			break;
		case CC_E_HOSTDROPPEDCONNECTION:
			sc = CPR_E_HOSTDROPPEDCONNECTION;
			break;
		//
		// Login errors
		//
		case CC_E_ALREADYLOGGING:
			sc = CPR_E_ALREADYLOGGING;
			break;
		case CC_E_ALREADYLOGGED:
			sc = CPR_E_ALREADYLOGGED;
			break;
		case CC_E_NOTLOGGED:
			sc = CPR_E_NOTLOGGED;
			break;
		case CC_E_ALREADYREGISTERED:
			sc = CPR_E_ALREADYREGISTERED;
			break;
		case CC_E_NOTREGISTERED:
			sc = CPR_E_NOTREGISTERED;
			break;
		case CC_E_LOGINFAILED:
			sc = CPR_E_LOGINFAILED;
			break;
		case CC_E_AUTHFAILED:
			sc = CPR_E_AUTHFAILED;
			break;
		case CC_E_AUTHNOTAVAIL:
			sc = CPR_E_AUTHNOTAVAIL;
			break;
		case CC_E_AUTHENTICATEDONLY:
			sc = CPR_E_AUTHENTICATEDONLY;
			break;
		//
		// Channel Errors
		//
		case CC_E_ALREADYOPEN:
			sc = CPR_E_ALREADYOPEN;
			break;
		case CC_E_ALREADYOPENING:
			sc = CPR_E_ALREADYOPENING;
			break;
		case CC_E_NOTOPEN:
			sc = CPR_E_NOTOPEN;
			break;
		case CC_E_OPENNOTUNIQUE:
			sc = CPR_E_OPENNOTUNIQUE;
			break;
		case CC_E_CHANNELBADNAME:
			sc = CPR_E_CHANNELBADNAME;
			break;
		case CC_E_CHANNELBADKEY:
			sc = CPR_E_CHANNELBADKEY;
			break;
		case CC_E_CHANNELBADMODES:
			sc = CPR_E_CHANNELBADMODES;
			break;
		case CC_E_NOTONCHANNEL:
			sc = CPR_E_NOTONCHANNEL;
			break;
		case CC_E_NOSUCHCHANNEL:
			sc = CPR_E_NOSUCHCHANNEL;
			break;
		case CC_E_CHANNELFULL:
			sc = CPR_E_CHANNELFULL;
			break;
		case CC_E_ALREADYONCHANNEL:
			sc = CPR_E_ALREADYONCHANNEL;
			break;
		case CC_E_TOOMANYCHANNELS:
			sc = CPR_E_TOOMANYCHANNELS;
			break;
		case CC_E_INVITEONLYCHANNEL:
			sc = CPR_E_INVITEONLYCHANNEL;
			break;
		case CC_E_CHANNELKEYALREADYSET:
			sc = CPR_E_CHANNELKEYALREADYSET;
			break;
		//
		// Property Errors
		// 
		case CC_E_BADPROPERTY:
			sc = CPR_E_BADPROPERTY;
			break;
		case CC_E_BADVALUE:
			sc = CPR_E_BADVALUE;
			break;
		case CC_E_PROPNOTREADABLE:
			sc = CPR_E_PROPNOTREADABLE;
			break;
		case CC_E_PROPREADONLY:
			sc = CPR_E_PROPREADONLY;
			break;
		case CC_E_PROPNOTWRITEABLE:
			sc = CPR_E_PROPNOTWRITEABLE;
			break;
		//
		// Member Errors
		// 
		case CC_E_NOSUCHMEMBER:
			sc = CPR_E_NOSUCHMEMBER;
			break;
		case CC_E_MEMBERBADMODES:
			sc = CPR_E_MEMBERBADMODES;
			break;
		//
		// User Errors
		//
		case CC_E_BADNICKNAME:
			sc = CPR_E_BADNICKNAME;
			break;
		case CC_E_BADPASSWORD:
			sc = CPR_E_BADPASSWORD;
			break;
		case CC_E_NICKNAMEINUSE:
			sc = CPR_E_NICKNAMEINUSE;
			break;
		case CC_E_NICKCOLLISION:
			sc = CPR_E_NICKCOLLISION;
			break;
		case CC_E_NOSUCHNICK:
			sc = CPR_E_NOSUCHNICK;
			break;
		case CC_E_USERBADMODES:
			sc = CPR_E_USERBADMODES;
			break;
		case CC_E_NICKTOOFAST:
			sc = CPR_E_NICKTOOFAST;
			break;
		case CC_E_NICKNOCHANGE:
			sc = CPR_E_NICKNOCHANGE;
			break;
		//
		// Sending
		//
		case CC_E_TOOMUCHDATA:
			sc = CPR_E_TOOMUCHDATA;
			break;
		case CC_E_CANTSEND:
			sc = CPR_E_CANTSEND;
			break;
		case CC_E_INVALIDRECIPIENTLIST:
			sc = CPR_E_INVALIDRECIPIENTLIST;
			break;
		case CC_E_NULLMSG:
			sc = CPR_E_NULLMSG;
			break;
		case CC_E_BADMSGTYPE:
			sc = CPR_E_BADMSGTYPE;
			break;
		case CC_E_NOWHISPER:
			sc = CPR_E_NOWHISPER;
			break;
		case CC_E_BADTAG:
			sc = CPR_E_BADTAG;
			break;
		//
		// Protocol Errors
		//
		case CC_E_SERVER:
			sc = CPR_E_SERVER;
			break;
		case CC_E_NOTIRCX:
			sc = CPR_E_NOTIRCX;
			break;
		case CC_E_PROPMODE:
			sc = CPR_E_PROPMODE;
			break;
		case CC_E_NOMOTD:
			sc = CPR_E_NOMOTD;
			break;
		case CC_E_UNKNOWNCOMMAND:
			sc = CPR_E_UNKNOWNCOMMAND;
			break;
		//
		// Access Rights errors
		//
		case CC_E_NOTSYSOP:
			sc = CPR_E_NOTSYSOP;
			break;
		case CC_E_NOTOWNER:
			sc = CPR_E_NOTOWNER;
			break;
		case CC_E_NOTHOST:
			sc = CPR_E_NOTHOST;
			break;
		case CC_E_RATINGBLOCK:
			sc = CPR_E_RATINGBLOCK;
			break;
		case CC_E_BANNEDFROMCHANNEL:
			sc = CPR_E_BANNEDFROMCHANNEL;
			break;
		case CC_E_BANNEDFROMSERVER:
			sc = CPR_E_BANNEDFROMSERVER;
			break;
		case CC_E_WILLBEBANNED:
			sc = CPR_E_WILLBEBANNED;
			break;
		case CC_E_PERMISSIONDENIED:
			sc = CTL_E_PERMISSIONDENIED;
			break;
		case CC_E_CANTIGNORE:
			sc = CPR_E_CANTIGNORE;
			break;
		case CC_E_NOJOINMICONLY:
			sc = CPR_E_NOJOINMICONLY;
			break;
		case CC_E_NOJOINREMOTE:
			sc = CPR_E_NOJOINREMOTE;
			break;
		case CC_E_NOJOINDYNAMIC:
			sc = CPR_E_NOJOINDYNAMIC;
			break;
		case CC_E_NODYNAMICCHANNELS:
			sc = CPR_E_NODYNAMICCHANNELS;
			break;
		case CC_E_AUTHONLY:
			sc = CPR_E_AUTHONLY;
			break;
		case CC_E_OVERFLOWABORT:
			sc = CPR_E_OVERFLOWABORT;
			break;
		case CC_E_CANTCHANGEUSERMODE:
			sc = CPR_E_CANTCHANGEUSERMODE;
			break;
		//
		// ChatItems related errors
		//
		case CC_E_ITEMNOTAVAILABLE:
			sc = CPR_E_ITEMNOTAVAILABLE;
			break;
		case CC_E_ITEMNAMENA:
			sc = CPR_E_ITEMNAMENA;
			break;
		case CC_E_INVALIDITEMNAME:
			sc = CPR_E_INVALIDITEMNAME;
			break;
		case CC_E_INVALIDITEMVALUE:
			sc = CPR_E_INVALIDITEMVALUE;
			break;
		case CC_E_NOTOPERATOR:
			sc = CPR_E_NOTOPERATOR;
			break;
		case CC_E_TIMEOUT:
			sc = CPR_E_TIMEOUT;
			break;

		default:
			sc = CPR_E_FAILURE;
	}

	ASSERT(sc != CPR_E_FAILURE, "sc == CPR_E_FAILURE in GetScodeFromHResult");

	return sc;
}
