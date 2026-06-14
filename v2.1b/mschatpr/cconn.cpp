//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include "StdAfx.H"
#include "CConn.H"
#include "CDebug.H"
#include "CCError.H"
#include "PrCnst.H"
#include "IrcMsg.H"

// for ASSERT and FAIL
SZTHISFILE

//--------------------------------------------------------------------------------------------
//
// CChatConn
// 
//--------------------------------------------------------------------------------------------

CChatConn::CChatConn(DWORD cbBufferMax)
		  :CConnection(cbBufferMax)
{
    //  Initialize SSPI related variables.
    //
	m_pFuncTbl			= NULL;
	m_hSecLib			= NULL;
	m_bCredential		= FALSE;

	g_lMaxMsgLength		= g_lDefaultMaxMsgLength;

	m_hrLastError		= NOERROR;

	m_bLoggedOn			= FALSE;
	m_bIrcXServer		= FALSE;
	m_bRegistered		= FALSE;
	m_bProtectedPosting	= FALSE;
	m_bStopPosting		= FALSE;

	m_pPrSecurity		= NULL;
	m_hwndMess			= NULL;

	m_lPoolIndex		= 0L;
	m_lPoolFree			= g_nIrcMsgPool;
	bInitIrcMsgPool();
}


CChatConn::~CChatConn(void)
{
	OutputDebugThreadIdString("CChatConn::~CChatConn - Enter\n");

	HrLogOff();
	ASSERT(!bIsLoggedOn(), "Still Logged On in CChatConn::~CChatConn");
	HrClose();
	bFreeIrcMsgPool();

	if (m_pPrSecurity)
		delete [] (BYTE*) m_pPrSecurity;

    //  Release the SSPI resources if allocated.
    if (m_pFuncTbl)
	{
        if (m_bCredential)
		    (*(m_pFuncTbl->FreeCredentialHandle))(&m_hCredential);
		FreeLibrary(m_hSecLib);	// REGISB:  UmeshM puts in Chatsock that this call produces a GPF on Win95
	}
}


BOOL CChatConn::bInitIrcMsgPool(void)
{
	for (SHORT n = 0; n < g_nIrcMsgPool; n++)
	{
		m_rgPrIrcMsg[n].bFree = TRUE;
		m_rgPrIrcMsg[n].byteParams = 0;
		m_rgPrIrcMsg[n].szPrefix[0] = g_chEOS;
		m_rgPrIrcMsg[n].szCommand[0] = g_chEOS;
		for (SHORT m = 0; m < g_nMaxParams; m++)
		{
			m_rgPrIrcMsg[n].szParams[m] = NULL;
			m_rgPrIrcMsg[n].nParamsLen[m] = 0;
		}
	}
	return TRUE;
}


BOOL CChatConn::bFreeIrcMsgPool(void)
{
	for (SHORT n = 0; n < g_nIrcMsgPool; n++)
		for (SHORT m = 0; m < g_nMaxParams; m++)
			if (m_rgPrIrcMsg[n].szParams[m])
				delete [] m_rgPrIrcMsg[n].szParams[m];
	return TRUE;
}


void CChatConn::FreeIrcMsg(PPRIRCMSG pPrIrcMsg)
{ 	
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatConn::FreeIrcMsg");
	pPrIrcMsg->bFree = TRUE;
	InterlockedIncrement(&m_lPoolFree);
	ASSERT(m_lPoolFree <= g_nIrcMsgPool, "m_lPoolFree > g_nIrcMsgPool in CChatConn::FreeIrcMsg");
}


HRESULT CChatConn::HrGetSecurity(PPRSECURITY *ppPrSecurity)
{
	ASSERT(ppPrSecurity, "ppPrSecurity is NULL in CChatConn::bGetSecurity");

	*ppPrSecurity = m_pPrSecurity;

	return m_pPrSecurity ? NOERROR : E_FAIL;
}


HRESULT CChatConn::HrWaitForNextReplies(DWORD dwTimeOut, BOOL bGetAllReplies, PPRIRCMSG rgPrIrcMsg, UINT *pcPrIrcMsgCount)
{
	INT			cbRead = 0;
	INT			cbGot = 0;
	CHAR		szBuf[g_nMaxLength*2];
	CHAR		*sz = szBuf;
	CHAR		*szAnswer = NULL;
	HRESULT		hr;
	BOOL		bGotReplyEnd;
	UINT		cPrIrcMsgCount, iPrIrcMsg = 0;

	OutputDebugThreadIdString("CChatConn::HrWaitForNextReplies - Enter\n");

	ASSERT(rgPrIrcMsg, "rgPrIrcMsg is NULL in CChatConn::HrWaitForNextReplies");
	ASSERT(pcPrIrcMsgCount, "pcPrIrcMsgCount is NULL in CChatConn::HrWaitForNextReplies");

	cPrIrcMsgCount = *pcPrIrcMsgCount;
	*pcPrIrcMsgCount = 0;

	while (dwTimeOut && FAILED(hr = HrIsDataWaiting(1L)))
		dwTimeOut--;

	if (FAILED(hr))
		return CC_E_TIMEOUT;

	::ZeroMemory((PVOID) szBuf, g_nMaxLength*2);

	do
	{		
		// Get the next server's answer
		if (FAILED(hr = HrRecv((PVOID) (sz + cbRead), (g_nMaxLength*2 - cbRead), 0, &cbGot)))
			return hr;
		cbRead += cbGot;
		bGotReplyEnd = (*(sz + cbGot - 1) == g_chLF);
	}
	while ((!bGotReplyEnd || bGetAllReplies) && SUCCEEDED(HrIsDataWaiting(0L, 500000L)));

	szAnswer = szBuf;
	do
	{
		// Get the next answer
		if (iPrIrcMsg >= cPrIrcMsgCount)
			return CC_E_BUFFERTOOSMALL;

		sz = szAnswer;
		while (*sz != g_chLF)
			sz++;
		if (*(sz-1) == g_chCR)
			sz--;
		*sz = g_chEOS;

		ZeroMemory(&rgPrIrcMsg[iPrIrcMsg], sizeof(PRIRCMSG));

		if (FAILED(hr = HrFillInIrcMsg(&rgPrIrcMsg[iPrIrcMsg], szAnswer)))
			return hr;

		// is there another answer waiting for us?
		szAnswer = ++sz;
		if (*szAnswer == g_chLF)
			szAnswer++;
		iPrIrcMsg++;
	}
	while (*szAnswer != g_chEOS);

	*pcPrIrcMsgCount = iPrIrcMsg;

	return NOERROR;
}


HRESULT CChatConn::HrReceiveData(PVOID pvRecv, INT cbData, INT *pcbTaken)
{
	HRESULT hr = NOERROR;
	CHAR	*szCur = (CHAR*) pvRecv;
	CHAR	*szEnd = NULL;
	SHORT	cbTerminator;

	// OutputDebugThreadIdString("CChatConn::HrReceiveData - Enter\n");

	ASSERT(pvRecv, "pvRecv is NULL in CChatConn::HrReceiveData");
	ASSERT(pcbTaken, "pcbTaken is NULL in CChatConn::HrReceiveData");

	*pcbTaken = 0;

	while (*pcbTaken < cbData && !m_bStopPosting)
	{
		if (0L == m_lPoolFree)
		{
			// no more structure available to post - give the hand to the other thread a bit
			OutputDebugThreadIdString("CChatConn::HrReceiveData - Structure Pool Full!\n");
			Sleep(250);
		}

		// Find the next \r, \n or \r\n in the incoming data
		szEnd = szCur;
		while (szEnd < (CHAR*) pvRecv + cbData && *szEnd != g_chLF)
			szEnd++;
		
		if (szEnd == (CHAR*) pvRecv + cbData)
			// no more complete message available
			return NOERROR;

		ASSERT(*szEnd == g_chLF, "*szEnd != g_chLF in CChatConn::HrReceiveData");

		// There is a complete message to eat
		if (*(szEnd-1) == g_chCR)
		{
			szEnd--;
			cbTerminator = 2;
			//#ifdef DEBUG
			//	sprintf(g_szDebugStr, "   --CRLF-- %X\n", szEnd);
			//	OutputDebugThreadIdString(g_szDebugStr);
			//#endif
		}
		else
		{
			cbTerminator = 1;
			//#ifdef DEBUG
			//	sprintf(g_szDebugStr, "   --LF-- %X\n", szEnd);
			//	OutputDebugThreadIdString(g_szDebugStr);
			//#endif
		}
		*szEnd = g_chEOS;

		// m_lPoolIndex is the first one that might be available
		while (!m_rgPrIrcMsg[m_lPoolIndex].bFree)
		{
			m_lPoolIndex++;
			m_lPoolIndex %= g_nIrcMsgPool;
		}

		if (FAILED(hr = HrFillInIrcMsg(&m_rgPrIrcMsg[m_lPoolIndex], szCur)))
			return hr;
		*pcbTaken += szEnd - szCur + cbTerminator;
		szCur = szEnd + cbTerminator;

		//#ifdef DEBUG
		//	DumpIrcMsg(&m_rgPrIrcMsg[m_lPoolIndex]);
		//#endif // DEBUG

		InterlockedDecrement(&m_lPoolFree);
		ASSERT(m_lPoolFree >= 0, "m_lPoolFree < 0 in CChatConn::HrReceiveData");

		BOOL bPost = bPostNewIrcMsg(WM_U_IRC_MSG, (WPARAM) &m_rgPrIrcMsg[m_lPoolIndex], (LPARAM) TRUE);
		ASSERT(bPost, "bPostNewIrcMsg failed in CChatConn::HrReceiveData");
		if (!bPost)
		{
			hr = GetLastError();
			InterlockedIncrement(&m_lPoolFree);
		}
		else
		{
			m_lPoolIndex++;
			m_lPoolIndex %= g_nIrcMsgPool;
		}
	}
	return hr;
}


#ifdef DEBUG
void CChatConn::DumpIrcMsg(PPRIRCMSG pPrIrcMsg)
{
	CHAR szIncoming[2048];

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatConn::DumpIrcMsg");

	g_cs.Lock();

	if (pPrIrcMsg->szPrefix[0])
	{
		sprintf(szIncoming, "Prefix:'%s', ", pPrIrcMsg->szPrefix);
		OutputDebugString(szIncoming);
	}
	sprintf(szIncoming, "Command:'%s'\n", pPrIrcMsg->szCommand);
	OutputDebugString(szIncoming);

	for (BYTE byte = 0; byte < pPrIrcMsg->byteParams; byte++)
	{
		sprintf(szIncoming, "Param:'%s'\n", pPrIrcMsg->szParams[byte]);
		OutputDebugString(szIncoming);
	}

	g_cs.Unlock();
}
#endif // DEBUG


HRESULT CChatConn::HrCopyNextWord(CHAR *szDest, CHAR **pszSrc, INT *pcLen)
{
	CHAR *szCurSrc  = *pszSrc;
	CHAR *szCurDest = szDest;

	ASSERT(szDest, "szDest is NULL in CChatConn::HrCopyNextWord");
	ASSERT(pszSrc,  "pszSrc is NULL in CChatConn::HrCopyNextWord");
	ASSERT(*pszSrc, "*pszSrc is NULL in CChatConn::HrCopyNextWord");

	if (pcLen)
		*pcLen = 0;

	while (szCurSrc && *szCurSrc != g_chEOS && *szCurSrc != g_chSpace)
		*szCurDest++ = *szCurSrc++;
	*szCurDest = g_chEOS;

	if (pcLen)
		*pcLen = szCurSrc - *pszSrc;

	// skip the space if space there is
	if (szCurSrc && *szCurSrc == g_chSpace)
		szCurSrc++;
	*pszSrc = szCurSrc;

	return NOERROR;
}


HRESULT CChatConn::HrCopyNextParam(CHAR **pszDest, SHORT *pnParamLen, CHAR **pszSrc, INT *pcLen)
{
	CHAR	*szCurSrc  = *pszSrc;
	CHAR	*szCurDest = *pszDest;
	INT		cLen = 0;

	ASSERT(pszDest, "pszDest is NULL in CChatConn::HrCopyNextParam");
	ASSERT(pnParamLen, "pnParamLen is NULL in CChatConn::HrCopyNextParam");
	ASSERT(pszSrc,  "pszSrc is NULL in CChatConn::HrCopyNextParam");
	ASSERT(*pszSrc, "*pszSrc is NULL in CChatConn::HrCopyNextParam");

	if (pcLen)
		*pcLen = 0;

	// if first character is a colon, then the param is all the remaining string
	if (g_chColon == *szCurSrc)
	{
		cLen = lstrlen(szCurSrc)-1;
		(*pszSrc)++;	// skip the colon
	}
	else
		while (szCurSrc && g_chEOS != *szCurSrc && g_chSpace != *szCurSrc)
		{
			cLen++;
			szCurSrc++;
		}

	if (cLen >= *pnParamLen)
	{
		if (szCurDest)
			delete [] szCurDest;
		szCurDest = new CHAR[cLen+1];
		if (!szCurDest)
			return E_OUTOFMEMORY;
		*pszDest = szCurDest;
		*pnParamLen = cLen+1;
	}

	lstrcpyn(szCurDest, *pszSrc, cLen+1);

	if (pcLen)
		*pcLen = cLen;
	*pszSrc += cLen;
	if (*pszSrc && g_chSpace == **pszSrc)
		(*pszSrc)++;	// skip the space

	return NOERROR;
}


HRESULT CChatConn::HrFillInIrcMsg(PPRIRCMSG pPrIrcMsg, CHAR *szSrc)
{
	CHAR	*szCur = szSrc;
	BYTE	byte;
	INT		cLen;
	HRESULT	hr;

	// szSrc points to a complete IRC incoming message
	// OutputDebugThreadIdString("CChatConn::HrFillInIrcMsg - Enter\n");
	#ifdef DEBUG
		g_cs.Lock();
		OutputDebugString(">");
		OutputDebugString(szSrc);
		OutputDebugString("<\n");
		g_cs.Unlock();
	#endif // DEBUG

	ASSERT(szSrc, "szSrc is NULL in CChatConn::HrFillInIrcMsg");
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatConn::HrFillInIrcMsg");

	pPrIrcMsg->uCode = 0;
	pPrIrcMsg->bFree = FALSE;
	pPrIrcMsg->byteParams = 0;
	*(pPrIrcMsg->szPrefix) = g_chEOS;
	*(pPrIrcMsg->szCommand) = g_chEOS;
	for (byte = 0; byte < g_nMaxParams; byte++)
		if (pPrIrcMsg->szParams[byte])
			*(pPrIrcMsg->szParams[byte]) = g_chEOS;

	// Get the first piece
	if (FAILED(hr = HrCopyNextWord(pPrIrcMsg->szCommand, &szCur, &cLen)))
		return hr;

	// 
	// Do we have a prefix?
	//
	if (g_chColon == pPrIrcMsg->szCommand[0])
	{
		// Yes, we do
		lstrcpyn(pPrIrcMsg->szPrefix, pPrIrcMsg->szCommand+1, cLen); // skip the initial colon
		// Get the command this time
		if (FAILED(hr = HrCopyNextWord(pPrIrcMsg->szCommand, &szCur, &cLen)))
			return hr;
	}

	byte = 0;
	while (szCur && *szCur != g_chEOS)
	{
		// Get the next parameter
		if (FAILED(hr = HrCopyNextParam(&pPrIrcMsg->szParams[byte], &(pPrIrcMsg->nParamsLen[byte]), &szCur, &cLen)))
			return hr;
		byte++;
	}

	pPrIrcMsg->byteParams = byte;

	// Now fill in uCode member
	CHAR	ch = pPrIrcMsg->szCommand[0];
	INT		i  = 0;
 	if (bIsDigit(ch))
	{
		// Result or Error Code
		do
		{
			pPrIrcMsg->uCode *= 10;
			pPrIrcMsg->uCode += (ch - '0');
			ch = pPrIrcMsg->szCommand[++i];
		}
		while (bIsDigit(ch));
	}

	return NOERROR;
}


BOOL CChatConn::bPostNewIrcMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	ASSERT(m_hwndMess, "m_hwndMess is NULL in CChatConn::bPostNewIrcMsg");
	BOOL bRet;

	if (m_bProtectedPosting)
		m_csPosting.Lock();
	
	bRet = ::PostMessage(m_hwndMess, uMsg, wParam, lParam);

	if (m_bProtectedPosting)
		m_csPosting.Unlock();

	return bRet;
}


BOOL CChatConn::bIsLoggedOn(void)
{
	BOOL bRet;

	m_csData.Lock();

	bRet = m_bLoggedOn;

	m_csData.Unlock();

	return bRet;
}


void CChatConn::SetLoggedOn(BOOL bSet)
{		
	m_csData.Lock();
	
	m_bLoggedOn = bSet;

	m_csData.Unlock();
}


void CChatConn::SetRegistered(BOOL bSet)
{
	m_csData.Lock();

	m_bRegistered = bSet;

	m_csData.Unlock();
}


BOOL CChatConn::bIsRegistered(void)
{
	BOOL bRet;

	m_csData.Lock();

	bRet = m_bRegistered;

	m_csData.Unlock();

	return bRet;
}


HRESULT CChatConn::HrSendAndDelete(BYTE *pb, INT cb)
{
	ASSERT(pb && cb, "!pb || !cb in CChatConn::HrSendAndDelete");

	HRESULT hr = HrSend((PVOID) pb, cb);
	//
	// delete the storage for the msg
	//
	delete [] pb;

	return hr;
}


HRESULT CChatConn::HrFillInSecurityPackages(PPRIRCMSG pPrIrcMsg)
{
	DWORD	dwcb = 0L;
	BYTE	*pb = NULL;
	INT		cch;
	CHAR	*sz, *szCopy, *szSecuPack;

	// pPrIrcMsg->szPrefix	  = "keezer"
	// pPrIrcMsg->szCommand	  = "800"
	// pPrIrcMsg->szParams[0] = "*"						// no nickname specified yet
	// pPrIrcMsg->szParams[1] = "0|1"					// we asked to turn into the IRCX mode or not
	// pPrIrcMsg->szParams[2] = "0"						// IRCX version number
	// pPrIrcMsg->szParams[3] = "NTLM,ANON"				// security packages
	// pPrIrcMsg->szParams[4] = "512"					// max message length
	// pPrIrcMsg->szParams[5] = "*"						// ???

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatConn::HrFillInSecurityPackages");
	ASSERT(!m_pPrSecurity, "m_pPrSecurity NOT NULL in CChatConn::HrFillInSecurityPackages");

	if ('*' == pPrIrcMsg->szParams[4][0])
	{
		// we don't have any secu packages on the server
		g_lMaxMsgLength = SzToL(pPrIrcMsg->szParams[3]);
	}
	else
	{
		// we do have some secu packages on this server
		ASSERT('*' == pPrIrcMsg->szParams[5][0], "Unexpected param in CChatConn::HrFillInSecurityPackages");
		dwcb = lstrlen(pPrIrcMsg->szParams[3])+1;  //REGISB: is more than needed if anon allowed
		g_lMaxMsgLength = SzToL(pPrIrcMsg->szParams[4]);
	}

	// Alloc the block and copy security models
	dwcb += sizeof(PRSECURITY);
	pb = new BYTE[dwcb];
	if (!pb)
	{
		ASSERT(FALSE, "Out Of Memory in CChatConn::HrFillInSecurityPackages");
		return E_OUTOFMEMORY;
	}

	m_pPrSecurity = (PPRSECURITY) pb;
	m_pPrSecurity->dwcb = dwcb;
	m_pPrSecurity->cPackages = 0;
	m_pPrSecurity->bAnonAllowed = FALSE;

	szCopy	= (CHAR*) (m_pPrSecurity + 1);
	sz = pPrIrcMsg->szParams[3];

	while (bGetNextSubParam(&sz, &szSecuPack, &cch))
	{
		if (0 != lstrcmpi(g_szAnon, szSecuPack))
		{
			m_pPrSecurity->cPackages++;
			lstrcpyn(szCopy, szSecuPack, cch+1);
			szCopy += cch+1;
		}
		else
			m_pPrSecurity->bAnonAllowed = TRUE;
	}

	return NOERROR;
}


HRESULT CChatConn::HrIsServerIrcX(CHAR *szServer)
{
	OutputDebugThreadIdString("CChatConn::HrIsServerIrcX - Enter\n");

	const SHORT	nPrIrcMsgCount = 8;
	PRIRCMSG	rgPrIrcMsg[nPrIrcMsgCount];
	UINT		iPrIrcMsg, cPrIrcMsgCount = nPrIrcMsgCount;
	HRESULT		hr;

	// Server is not IRCX by default
	m_bIrcXServer = FALSE;

	// This is what we send here: "MODE ISIRCX\r\n"
	if (FAILED(hr = HrSend((PVOID) g_szModeIsIrcX, lstrlen(g_szModeIsIrcX))))
		return hr;

	for (iPrIrcMsg = 0; iPrIrcMsg < nPrIrcMsgCount; iPrIrcMsg++)
		for (SHORT m = 0; m < g_nMaxParams; m++)
			rgPrIrcMsg[iPrIrcMsg].szParams[m] = NULL;

	// Read off the responses. We can either get an Error or a RPL_IRCX
	hr = HrWaitForNextReplies(g_dwIsIrcXTimeout, TRUE /*bGetAllReplies*/, (PPRIRCMSG) rgPrIrcMsg, &cPrIrcMsgCount);
	// Some IRC servers might not respond at all to g_szModeIsIrcX. We have to poll the socket to
	// see if any data came back..
	if (CC_E_TIMEOUT == hr)
	{
		hr = NOERROR; // assuming this is an IRC server since we can't identify it
		goto exit;
	}
	if (FAILED(hr))
		goto exit;

	for (iPrIrcMsg = 0; iPrIrcMsg < cPrIrcMsgCount; iPrIrcMsg++)
	{
		if (rgPrIrcMsg[iPrIrcMsg].uCode)
		{
			// Is it an Error or a Result Code?
			if (bIsReplyCode(rgPrIrcMsg[iPrIrcMsg].uCode) && !m_bIrcXServer)
			{
				ASSERT(rgPrIrcMsg[iPrIrcMsg].uCode == RPL_IRCX, "rgPrIrcMsg[iPrIrcMsg].uCode != RPL_IRCX in CChatConn::HrIsServerIrcX");
				m_bIrcXServer = TRUE;
				// get the packages from the prIrcMsg parameters
				// the result should be something like ":keezer 800 0 0 NTLM,ANON 512 *"
				if (FAILED(hr = HrFillInSecurityPackages(&rgPrIrcMsg[iPrIrcMsg])))
					goto exit;

				// This is what we send here: "IRCX\r\n"
				if (FAILED(hr = HrSend((PVOID) g_szIrcX, lstrlen(g_szIrcX))))
					goto exit;

				if (FAILED(hr = HrWaitForNextReplies(g_dwIsIrcXTimeout, FALSE /*bGetAllReplies*/, (PPRIRCMSG) rgPrIrcMsg, &cPrIrcMsgCount)))
					goto exit;

				break;
			}
			else
			{
				// It's an error, most likely "You're not registered"
				ASSERT(rgPrIrcMsg[iPrIrcMsg].uCode == ERR_NOTREGISTERED, "rgPrIrcMsg[iPrIrcMsg].uCode != ERR_NOTREGISTERED in CChatConn::HrIsServerIrcX");
			}
		}
		else
		{
			// It's a command, most likely NOTICE AUTH :...
			// Send the command to the main thread and expose it to the user synchronously
			::SendMessage(m_hwndMess, WM_U_IRC_MSG, (WPARAM) &rgPrIrcMsg[iPrIrcMsg], (LPARAM) FALSE);
		}
	}

	hr = NOERROR;

exit:
	for (iPrIrcMsg = 0; iPrIrcMsg < nPrIrcMsgCount; iPrIrcMsg++)
		for (SHORT m = 0; m < g_nMaxParams; m++)
			if (rgPrIrcMsg[iPrIrcMsg].szParams[m])
				delete [] rgPrIrcMsg[iPrIrcMsg].szParams[m];
	return hr;
}


HRESULT CChatConn::HrIdentifyServer(CHAR *szServer)
{
	HRESULT	hr = NOERROR;

	OutputDebugThreadIdString("CChatConn::HrIdentifyServer - Enter\n");

	if (m_pPrSecurity)
	{	// free the previous structure
		delete [] (BYTE*) m_pPrSecurity;
		m_pPrSecurity = NULL;
	}

	if (FAILED(hr = HrIsServerIrcX(szServer)))
		return hr;

#ifdef DEBUG
	if (m_bIrcXServer)
		OutputDebugThreadIdString("CChatConn::HrIdentifyServer - We deal with an IRCX server\n");
	else
		OutputDebugThreadIdString("CChatConn::HrIdentifyServer - We deal with an IRC server\n");
#endif // DEBUG

	if (!m_bIrcXServer)
	{
		m_pPrSecurity = (PPRSECURITY) new BYTE[sizeof(PRSECURITY)];
		m_pPrSecurity->dwcb = sizeof(PRSECURITY);
		m_pPrSecurity->cPackages = 0;
		m_pPrSecurity->bAnonAllowed = TRUE;
	}
	return NOERROR;
}


HRESULT CChatConn::HrConnection(CHAR *szServer, UINT uPort)
{
	// So that we will register again
	SetRegistered(FALSE);

	return CConnection::HrConnection(szServer, uPort);
}


//
//	CHAR	*szNickname			IN *your nickname. Required.
//	CHAR	*szUserName			IN *user name to use. Required.
//	CHAR	*szRealName			IN *real name to use (description). Not Required.
//	CHAR	*szPassword			IN *password. Required if szUserName is NOT NULL 
//								On authenticated login, you will be prompted by the security
//								dll if szUserName or szPassword are NULL
//	CHAR	*szSecurityPackage	IN *a null terminated string that specifies the security package
//								to use during authentication. NULL for Anonymous connections
//
HRESULT CChatConn::HrLogin(CHAR *szNickname, CHAR *szUserName, CHAR *szRealName, CHAR *szPassword, CHAR *szSecurityPackage)
{
	HRESULT		hr = NOERROR;
	BYTE		*pb = NULL;
	INT			cb = 0;

	OutputDebugThreadIdString("CChatConn::HrLogin - Enter\n");

	ASSERT(szNickname, "szNickname is NULL in CChatConn::HrLogin");
	ASSERT(szUserName, "szUserName is NULL in CChatConn::HrLogin");

	if (bIsLoggedOn())
		return NOERROR;	// already logged on

	if (szSecurityPackage)
	{
		if (FAILED(hr = HrAuthenticate(szUserName, szPassword, szSecurityPackage)))
			return hr;
		SetRegistered(TRUE);
	}

	// Send the PASS and NICK command, then construct the USER command
	if (FAILED(hr = HrMakeLoginMsg(szNickname,
								   szUserName,
								   szRealName,
								   szPassword,						
								   &pb,
								   &cb)))
		return hr;

	// Send the request to the server
	if (pb && cb)
	{
		// Send the USER command
		if (FAILED(hr = HrSendAndDelete(pb, cb)))
			return hr;
		// From now on we are registered until we disconnect
		SetRegistered(TRUE);
	}

	return NOERROR;
}


HRESULT CChatConn::HrAuthenticate(CHAR *szUserName, CHAR *szPassword, CHAR *szSecurityPackage)
{
    SEC_WINNT_AUTH_IDENTITY AuthData, *pAuthData;
    INIT_SECURITY_INTERFACE	addrProcISI;
    DWORD					dwSecStatus;
    DWORD					dwLifeTime;
	PRIRCMSG				prIrcMsg;
	UINT					cPrIrcMsgCount = 1;

    //  If this is the first authentication attempt then load the SECURITY.DLL and obtain
    //  the security entry point.
	if (!m_pFuncTbl)
	{
        OSVERSIONINFO VerInfo;

        //  Find out which security DLL to use, depending on whether we are on NT or Win95
        VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		if (!GetVersionEx(&VerInfo))
			return GetLastError();	// REGISB: Might want to have a custom error code

		//  Load the platform specific version of the security dll.
        if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
			m_hSecLib = ::LoadLibrary(_T("SECUR32.DLL"));
        else
			m_hSecLib = ::LoadLibrary(_T("SECURITY.DLL"));

		//  If the security component is not found then fail (this should never happen).
		if (!m_hSecLib)
			return GetLastError();	// REGISB: put correct error code here

		//  Retrieve the security entrypoint.
		addrProcISI = (INIT_SECURITY_INTERFACE) GetProcAddress(m_hSecLib, SECURITY_ENTRYPOINT);       
		if (!addrProcISI)
		{
			FreeLibrary(m_hSecLib);	// REGISB:  UmeshM puts in Chatsock that this call produces a GPF on Win95
			m_hSecLib = NULL;
			return E_FAIL;	// REGISB: put correct error code here
		}

		// Get the SSPI function table
		m_pFuncTbl = (*addrProcISI)();
		if (!m_pFuncTbl)
		{
			FreeLibrary(m_hSecLib); // REGISB:  UmeshM puts in Chatsock that this call produces a GPF on Win95
			m_hSecLib = NULL;
			return E_FAIL;	// REGISB: put correct error code here
		}
	}

    //  If credentials from a previous authentication is still allocated, release.
    if (m_bCredential)
	{
		(*(m_pFuncTbl->FreeCredentialHandle))(&m_hCredential);
		m_bCredential = FALSE;
	}

    //  If the caller provided an username and password, use them over the default.
    if (szUserName && szPassword)
    {
        AuthData.User = (PBYTE) szUserName;
        AuthData.UserLength = lstrlen(szUserName);
        AuthData.Password = (PBYTE) szPassword;
        AuthData.PasswordLength = lstrlen(szPassword);
        AuthData.Domain = NULL;
        AuthData.DomainLength = 0;
        AuthData.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
        pAuthData = &AuthData;
    }
    else
    {
        pAuthData = NULL;
    }

    //  Get a credential handle for this client to use in future SSPI calls.
    //      SEC_E_SECPKG_NOT_FOUND  080090305  - SSPI package not installed.
    dwSecStatus = (*(m_pFuncTbl->AcquireCredentialsHandle)) (NULL, szSecurityPackage, 
                      SECPKG_CRED_OUTBOUND,  NULL, pAuthData, NULL, NULL, 
                      &m_hCredential, &dwLifeTime);
    if (dwSecStatus != NO_ERROR)
        return E_FAIL;	// REGISB: put correct error code here

    //  Remember that a valid credential is available.
	m_bCredential = TRUE;

    //  Send the initial authentication message.
	HRESULT hr;
	if (FAILED(hr = HrGenerateAndSendAuthMsg(NULL, szSecurityPackage)))
		return hr;

	for (SHORT m = 0; m < g_nMaxParams; m++)
		prIrcMsg.szParams[m] = NULL;

    //  Perform the authentication handshake session with the chat server.
    while (SUCCEEDED(HrIsSocketValid()))
    {
        //  Read the next message or fail.
		hr = HrWaitForNextReplies(g_dwLoginTimeout, FALSE /*bGetAllReplies*/, &prIrcMsg, &cPrIrcMsgCount);
		if (FAILED(hr))
			goto exit;

		ASSERT(cPrIrcMsgCount == 1, "cPrIrcMsgCount != 1 in CChatConn::HrAuthenticate");

        //  If anything but an AUTH message is returned, then error.
		SHORT nCmd = NGetCmd(prIrcMsg.szCommand);
        if (cmdidAuth != nCmd)
		{
			// The authentication failed, we probably got the error
			// ":<servername> 910 * <secupackage> : Authentication failed
			ASSERT(0 == lstrcmp(prIrcMsg.szCommand, "910"), "Unexpected server reply in CChatConn::HrAuthenticate");
			hr = CC_E_AUTHFAILED;
			goto exit;
		}

        //  If the last authentication message was received then we are done.
        if (prIrcMsg.szParams[1][0] == '*')
            goto exit;

        //  Process the reply from the server.
	    hr = HrGenerateAndSendAuthMsg(prIrcMsg.szParams[2], szSecurityPackage);
		if (FAILED(hr))
			goto exit;
    }

    hr = E_FAIL;	// REGISB: put correct error code here

exit:
	for (m = 0; m < g_nMaxParams; m++)
		if (prIrcMsg.szParams[m])
			delete [] prIrcMsg.szParams[m];

	return hr;
}


HRESULT CChatConn::HrMakeLoginMsg(CHAR *szNickname, CHAR *szUserName, CHAR *szRealName, CHAR *szPassword, BYTE **ppb, INT *pcb)
{
	HRESULT hr;
	
	OutputDebugThreadIdString("CChatConn::HrMakeLoginMsg - Enter\n");

	//
	// In IRC, the login sequence requires both a NICK and a USER msg, with the NICK msg
	// going first. The USER msg will be sent by the core routines...so we send off a NICK
	// msg here.
	// NOTE: if the PASS is provided, then PASS goes first
	//
	if (szPassword)
	{
		if (FAILED(hr = HrMakePassMsg(ppb, pcb, szPassword)))
			return hr;

		if (FAILED(hr = HrSendAndDelete(*ppb, *pcb)))
			return hr;
	}

	if (FAILED(hr = HrMakeNickMsg(ppb, pcb, szNickname)))
		return hr;

	if (FAILED(hr = HrSendAndDelete(*ppb, *pcb)))
		return hr;

	if (bIsRegistered())
	{
		*ppb = NULL;
		*pcb = 0;
	}
	else
		hr = HrMakeUserMsg(ppb, pcb, szUserName, (szRealName ? szRealName : szUserName));
			
	return hr;
}


HRESULT CChatConn::HrLogOff(void)
{
	HRESULT	hr;
	BYTE	*pb;
	INT		cb;

	if (!bIsLoggedOn())
		return CC_E_NOTLOGGED;

	SetLoggedOn(FALSE);
	SetRegistered(FALSE);

	if (SUCCEEDED(HrIsSocketValid()))
	{
		// Send a log off msg
		if (FAILED(hr = HrMakeLogOffMsg(&pb, &cb)))
			return hr;

		return HrSendAndDelete(pb, cb);
	}
	else
		return TRUE;
}


//	CHAR	*szName				IN *channel name. Required.
//	CHAR	*szKeyword			IN *keyword for this channel (can be NULL)
//	LONG	lModes				IN *channel modes
//	LONG	lMaxMemberCount		IN *max # of members allowed in this channel. To use server default, set to 0L
BOOL CChatConn::bCreateChannel(CHAR	*szName, CHAR *szKeyword, LONG lModes, LONG lMaxMemberCount)
{
	OutputDebugThreadIdString("CChatConn::bCreateChannel - Enter\n");

	BYTE	*pb;
	INT		cb;
	CHAR	szModes[100];	//at most 31 mode chars, and at most 31 + or - to go with them..
							//.. so 64 + 36 = 100 chars should more than cover it :-)
	CHAR	szVal[36];

	szModes[0] = g_chEOS;

	INT		cch = MapToIRCMode(atChannel, lModes, (CHAR*) szModes, 63);

	// REGISB, 09/09/97: Workaround for bug 2824  'l' comes before 'k'
	if (lMaxMemberCount > 0L)
	{
		szModes[cch] = 'l';
		szModes[++cch] = g_chEOS;
	}

	if (szKeyword)
	{
		szModes[cch] = 'k';
		szModes[++cch] = g_chEOS;
	}

	if (lMaxMemberCount > 0L)
	{
		wsprintf(szVal, " %ld", lMaxMemberCount);
		lstrcat(szModes, szVal);
	}

	if (FAILED(m_hrLastError = HrMakeCreateMsg(&pb, &cb, szName, szModes, szKeyword)))
		return FALSE;
	if (FAILED(m_hrLastError = HrSendAndDelete(pb, cb)))
		return FALSE;

	return TRUE;
}


//+-------------------------------------------------------------------------------------------
//
//  Method:     HrGenerateAndSendAuthMsg(szBlob, szSecurityPackage)
//
//  Synopsis:   This function calls InitializeSecurityContext to generate 
//              an authentication message and sends it to the ICP server.  
//              It generates different authentication messages depending on 
//              whether there's security token from the server to be used 
//              as input message or not (i.e. whether szBlob is NULL).
//
//  Args:       [szBlob] -- Pointer to blob from the server.
//
//--------------------------------------------------------------------------------------------

HRESULT CChatConn::HrGenerateAndSendAuthMsg(CHAR *szBlob, CHAR *szSecurityPackage)
{
    PCtxtHandle		phCurrContext;
    SecBufferDesc	inSecDesc, outSecDesc;
    SecBuffer		inSecBuffer, outSecBuffer;
    PSecBufferDesc	pInSecDesc;
    ULONG			ulContextReq;
    ULONG			ulContextAttrib;
    DWORD			dwExpireTime;
    DWORD			dwStatus;
    BYTE			pbBuffer[4096];
	LPTSTR			szStr;
	UINT			cbBlob;
	HRESULT			hr;
	BYTE			*pb = NULL;
	INT				cb = 0;

    if (szBlob == NULL)
    {
        // This is the first time this HrGenerateAndSendAuthMsg() is called, so the
        // message generated will be a NEGOTIATE_MSG.
        phCurrContext = NULL;
        pInSecDesc = NULL;
    }
    else
    {
		if (!bStringToData(szBlob, (PBYTE) szBlob, &cbBlob))
			return E_FAIL;

        // Since we have received a CHALLENGE_MSG from the server, we should
        // generate a AUTHENTICATE_MSG to send to server.
        phCurrContext = &m_hContext;

        // Setup API's input security buffer to pass the client's negotiate
        // message to the SSPI.
        inSecDesc.ulVersion = 0;
        inSecDesc.cBuffers = 1;
        inSecDesc.pBuffers = &inSecBuffer;

        inSecBuffer.cbBuffer = cbBlob;
        inSecBuffer.BufferType = SECBUFFER_TOKEN;
        inSecBuffer.pvBuffer = (PVOID) szBlob;

        pInSecDesc = &inSecDesc;
    }

    // Setup API's output security buffer for receiving challenge message
    // from the SSPI.
    // Pass the client message buffer to SSPI via pvBuffer.

    outSecDesc.ulVersion = 0;
    outSecDesc.cBuffers = 1;
    outSecDesc.pBuffers = &outSecBuffer;

    outSecBuffer.cbBuffer = sizeof(pbBuffer);
    outSecBuffer.BufferType = SECBUFFER_TOKEN;
    outSecBuffer.pvBuffer = pbBuffer;

    ulContextReq = 0L;

    // Generate a negotiate/authenticate message to be sent to the server.
    dwStatus = (*(m_pFuncTbl->InitializeSecurityContext)) (
                                &m_hCredential,			// phCredential
                                phCurrContext,			// phContext
                                NULL,					// pszTargetName
                                ulContextReq,			// fContextReq
                                0L,						// reserved1
                                SECURITY_NATIVE_DREP,	// TargetDataRep
                                pInSecDesc,				// pInput
                                0L,						// reserved2
                                &m_hContext,			// phNewContext
                                &outSecDesc,			// pOutput negotiate msg
                                &ulContextAttrib,		// pfContextAttribute
                                &dwExpireTime);			// ptsLifeTime

	if ((dwStatus == SEC_I_COMPLETE_NEEDED) ||
		(dwStatus == SEC_I_COMPLETE_AND_CONTINUE))
		dwStatus = (*(m_pFuncTbl->CompleteAuthToken))(&m_hContext, &outSecDesc);

    //  If SSPI does not have user credential, and if we have not prompt 
    //  user for credential
    if (dwStatus == SEC_E_NO_CREDENTIALS && 
        (ulContextReq & (ISC_REQ_PROMPT_FOR_CREDS | ISC_REQ_USE_SUPPLIED_CREDS)) == 0L)
	{
        ulContextReq = ISC_REQ_PROMPT_FOR_CREDS;

        //  Call SSPI again, but this time ask SSPI to prompt 
        //  for user credentials
        dwStatus = (*(m_pFuncTbl->InitializeSecurityContext)) (
                                &m_hCredential,			// phCredential
                                phCurrContext,			// phContext
                                NULL,					// pszTargetName
                                ulContextReq,			// fContextReq
                                0L,						// reserved1
                                SECURITY_NATIVE_DREP,	// TargetDataRep
                                pInSecDesc,				// pInput
                                0L,						// reserved2
                                &m_hContext,			// phNewContext
                                &outSecDesc,			// pOutput negotiate msg
                                &ulContextAttrib,		// pfContextAttribute
                                &dwExpireTime);			// ptsLifeTime

        //  NOTE: This time SSPI will not return SEC_E_NO_CREDENTIALS again 
        //  unless the user does not wish to supply any credential (i.e. user 
        //  hits CANCEL button on the UI box).  So if SSPI returns 
        //  SEC_E_NO_CREDENTIALS again, we definitely do not want to ask 
        //  SSPI to prompt for user credentials.
    }

	if (!bDataToString((PBYTE)(outSecBuffer.pvBuffer), (UINT) outSecBuffer.cbBuffer, &szStr, TRUE))
		return E_OUTOFMEMORY;

	if (SUCCEEDED(hr = HrMakeAuthMsg(&pb, &cb, szSecurityPackage, szBlob ? "S" : "I", (CHAR*) szStr)))
		hr = HrSendAndDelete(pb, cb);

	delete [] szStr;

    return hr;
}


/* Kent's code is remplaced by our bStringToData and bDataToString in CCommon.cpp
//+-------------------------------------------------------------------------------------------
//
//  Function:   HrDecode(szSrc, pbDst, pcbDst)
//
//  Synopsis:   Decodes a text string back into binary.
//
//  Args:       [pSrc]  -- Pointer to the text string to decode.
//              [pDst]  -- Location to write the decoded string.
//              [pcDst] -- Location to store the number of bytes stored in pDst.
//
//  Returns:    True if successful, else false if invalid string to decode.
//
//--------------------------------------------------------------------------------------------

HRESULT CChatConn::HrDecode(CHAR *szSrc, PBYTE pbDst, UINT *pcbDst)
{
	UINT cbBytes = 0;

	ASSERT(szSrc, "szSrc is NULL in CChatConn::HrDecode");
	ASSERT(pbDst, "pbDst is NULL in CChatConn::HrDecode");
	ASSERT(pcbDst, "pcbDst is NULL in CChatConn::HrDecode");

	while (*szSrc)
	{
		if (*szSrc == '\\')
		{
			szSrc++;

			switch (*szSrc)
			{
			case 'b':
				*pbDst++ = ' ';
				break;

			case 'c':
				*pbDst++ = ',';
				break;

			case 'n':
				*pbDst++ = '\n';
				break;

			case 'r':
				*pbDst++ = '\r';
				break;

			case 't':
				*pbDst++ = '\007';
				break;

			case '\\':
				*pbDst++ = '\\';
				break;

			case '0':
				*pbDst++ = '\000';
				break;

			default:
				*pcbDst = 0;
				return E_FAIL;
			}
		}
		else
			*pbDst++ = (BYTE) *szSrc;

		szSrc++;
		cbBytes++;
	}

	*pcbDst = cbBytes;

	return NOERROR;
}


//+-------------------------------------------------------------------------------------------
//
//  Function:   HrEncode(pbSrc, cbSrc, pbDst, bMessage)
//
//  Synopsis:   Encodes a binary array into something IRC will parse.
//
//  Args:       [pSrc]     -- Pointer to the binary bytes to encode.
//              [cSrc]     -- Number of bytes to encode.
//              [pDst]     -- Location to write the encoded version.
//              [bMessage] -- True if string will be the last argument (follows colon).
//
//  Returns:    None.
//
//--------------------------------------------------------------------------------------------

HRESULT CChatConn::HrEncode(PBYTE pbSrc, UINT cbSrc, PBYTE pbDst, BOOL bMessage)
{
	ASSERT(pbSrc, "pbSrc is NULL in CChatConn::HrEncode");

	while (cbSrc--)
	{
		switch (*pbSrc)
		{
		case ' ':
			if (bMessage)
				*pbDst++ = ' ';
			else
			{
				*pbDst++ = '\\';
				*pbDst++ = 'b';
			}
			break;

		case ',':
			if (bMessage)
				*pbDst++ = ',';
			else
			{
				*pbDst++ = '\\';
				*pbDst++ = 'c';
			}
			break;

		case '\n':
			*pbDst++ = '\\';
			*pbDst++ = 'n';
			break;

		case '\r':
			*pbDst++ = '\\';
			*pbDst++ = 'r';
			break;

		case '\007':
			*pbDst++ = '\\';
			*pbDst++ = 't';
			break;

		case '\\':
			*pbDst++ = '\\';
			*pbDst++ = '\\';
			break;

		case '\0':
			*pbDst++ = '\\';
			*pbDst++ = '0';
			break;

		default:
			*pbDst++ = *pbSrc;
			break;
		}

		pbSrc++;
	}

	*pbDst = '\0';

	return NOERROR;
}
*/
