//=--------------------------------------------------------------------------=
// CCSock.Cpp
//=--------------------------------------------------------------------------=

#include "StdAfx.H"
#include "CCSock.H"
#include "IsspErr.H"
#include "IrcMsg.H"
#include "PrRes.H"

// for ASSERT and FAIL
//
SZTHISFILE


//=--------------------------------------------------------------------------=
// DwSocketThreadProcIdentDLoop
//=--------------------------------------------------------------------------=
// starts the identd loop in a separate thread
//
// Parameters:
//    PVOID		- [in] pointer to socket object
//
// Output:
//    DWORD
//
// Notes:
//
DWORD __stdcall DwSocketThreadProcIdentDLoop(PVOID pvData)
{
	CChatSocket *pSocket;
	
	ASSERT(pvData, "pvData is NULL in DwSocketThreadProcIdentDLoop");

	pSocket = (CChatSocket*) pvData;

	return pSocket->HrThreadProcIdentDLoop();
}


//=--------------------------------------------------------------------------=
// DwSocketThreadProcSocketLoop
//=--------------------------------------------------------------------------=
// starts a socket message polling loop in a separate thread
//
// Parameters:
//    PVOID		- [in] pointer to socket object
//
// Output:
//    DWORD
//
// Notes:
//
DWORD __stdcall DwSocketThreadProcSocketLoop(PVOID pvData)
{
	CChatSocket *pSocket;
	
	ASSERT(pvData, "pvData is NULL in DwSocketThreadProcSocketLoop");

	pSocket = (CChatSocket*) pvData;

	return pSocket->HrThreadProcSocketLoop();
}


/////////////////////////////////////////////////////////////////////////////
// CChatSocket::CChatSocket - Constructor
CChatSocket::CChatSocket(void)
{
	m_hwndMess					= NULL;
	m_hwndMessEx				= NULL;
	m_hwndParent				= NULL;

	m_hMsgThread				= NULL;
	m_hIdentDThread				= NULL;

	m_csState					= csDisconnected;

	m_uPort						= 0;
	m_szServerName				= NULL;
	m_szSecurity				= NULL;
	m_szNickname				= NULL;
	m_bstrNickname				= NULL;
	m_szUTF8Nickname			= NULL;
	m_szUserName				= NULL;
	m_szRealName				= NULL;
	m_szUserPwd					= NULL;

	m_lUserModes				= 0L;
	
	m_lPropInProgress			= 0L;
	m_lWhoIsInProgress			= 0L;
	m_lListInProgress			= 0L;
	m_lChannelListingInProgress	= 0L;
	m_lMemberListingInProgress	= 0L;
	m_lUserChanListingInProgress= 0L;
	m_lWhoIsListingInProgress	= 0L;
	m_lWhoListingInProgress		= 0L;
	m_lBannedListInProgress		= 0L;
	m_lModeIsInProgress			= 0L;

	m_lOpenChannels				= 0L;
	m_lOpeningChannels			= 0L;

	ZeroMemory(&m_prServer, sizeof(PRSERVER));
	m_prServer.lChannelCount = -1L;
	m_prServer.lNetInvisibleCount = -1L;
	m_prServer.lNetServerCount = -1L;
	m_prServer.lNetUserCount = -1L;
	m_prServer.lNodeServerCount = -1L;
	m_prServer.lNodeUserCount = -1L;
	m_prServer.lSysopCount = -1L;
	m_prServer.lUnknownConnectionCount = -1L;

	ZeroMemory(&m_prUser, sizeof(PRUSER));

	m_prUser.psaChannels = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxUserChannels);
	m_prUser.psaChannelsOwner = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxUserChannels);
	m_prUser.psaChannelsHost = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxUserChannels);
	m_prUser.psaChannelsVoice = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxUserChannels);
	ASSERT(m_prUser.psaChannels && 
		m_prUser.psaChannelsOwner && 
		m_prUser.psaChannelsHost && 
		m_prUser.psaChannelsVoice, 
		"Could not allocate channel vectors in CChatSocket::CChatSocket");

	ZeroMemory(&m_prChannel, sizeof(PRCHANNEL));

	m_hrLastError				= NOERROR;

	m_bTryIdentD				= FALSE;
	m_bLoginAlso				= FALSE;
	m_bCancelOperation			= FALSE;
	m_bFiredUserChanBeginEnum	= FALSE;

	m_bCanViewUnrated			= bCanViewUnrated(NULL, FALSE /*bPromptOverride*/);

	m_cconn.bInitSocketDLL();

	for (SHORT nCnt = 0; nCnt < g_nServerPropertyNames; nCnt++)
		m_rgbServerPropLocal[nCnt] = FALSE;

	// Take care of the CharSet
	HFONT	hfont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	LOGFONT	lf;
	GetObject(hfont, sizeof(LOGFONT), (LPVOID) &lf);

	m_byteCharSet = lf.lfCharSet;

	if (88 == m_byteCharSet)	// REGISB 04/24/97: Chinese (Taiwan) Windows bug work around
		m_byteCharSet = 136;

	if (0 == m_byteCharSet)		// REGISB 04/29/97: Chinese (PRC) Windows bug work around
	{
		LCID dwLCID = GetSystemDefaultLCID();
		if (0x0804 == LOWORD(dwLCID))
			m_byteCharSet = 134;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CChatSocket::~CChatSocket - Destructor
CChatSocket::~CChatSocket(void)
{
	OutputDebugThreadIdString("CChatSocket::~CChatSocket - Enter\n");

	BOOL bRet = m_listChannel.bDeleteList();
	ASSERT(bRet, "Channel's bDeleteList() failed in CChatSocket::~CChatSocket");

	ASSERT(m_cconn.bStopPosting() || m_lPropInProgress == 0L, "m_lPropInProgress != 0L in CChatSocket::~CChatSocket");
	ASSERT(m_cconn.bStopPosting() || m_lWhoIsInProgress == 0L, "m_lWhoIsInProgress != 0L in CChatSocket::~CChatSocket");
	ASSERT(m_cconn.bStopPosting() || m_lListInProgress == 0L, "m_lListInProgress != 0L in CChatSocket::~CChatSocket");
	ASSERT(m_cconn.bStopPosting() || m_lWhoIsListingInProgress == 0L, "m_lWhoIsListingInProgress != 0L in CChatSocket::~CChatSocket");
	ASSERT(m_cconn.bStopPosting() || m_lWhoListingInProgress == 0L, "m_lWhoListingInProgress != 0L in CChatSocket::~CChatSocket");
	ASSERT(m_cconn.bStopPosting() || m_lChannelListingInProgress == 0L, "m_lChannelListingInProgress != 0L in CChatSocket::~CChatSocket");
	ASSERT(m_cconn.bStopPosting() || m_lBannedListInProgress == 0L, "m_lBannedListInProgress != 0L in CChatSocket::~CChatSocket");
	ASSERT(m_cconn.bStopPosting() || m_lModeIsInProgress == 0L, "m_lModeIsInProgress != 0L in CChatSocket::~CChatSocket");

	ASSERT(0L == m_listQuery.DwGetCellCount(), "Query List Cell Count > 0 in CChatSocket::~CChatSocket");

	bRet = m_listQuery.bDeleteList();
	ASSERT(bRet, "Query's bDeleteList() failed in CChatSocket::~CChatSocket");

	HrCloseChatSocket(FALSE);

	ResetLoginInfo();

	if (m_szServerName)
		delete [] m_szServerName;

	if (m_prServer.psaInfo)
		SafeArrayDestroy(m_prServer.psaInfo);

	if (m_prServer.psaIgnored)
		SafeArrayDestroy(m_prServer.psaIgnored);

	FreePrUserContent(&m_prUser);
	FreePrChannelContent(&m_prChannel);
	
	m_cconn.FreeSocketDLL();
	OutputDebugThreadIdString("CChatSocket::~CChatSocket - Leave\n");
}


void CChatSocket::ResetLoginInfo(void)
{
	if (m_szNickname)
	{
		delete [] m_szNickname;
		m_szNickname = NULL;
	}

	if (m_szUTF8Nickname)
	{
		delete [] m_szUTF8Nickname;
		m_szUTF8Nickname = NULL;
	}

	if (m_bstrNickname)
	{
		SysFreeString(m_bstrNickname);
		m_bstrNickname = NULL;
	}

	if (m_szRealName)
	{
		delete [] m_szRealName;
		m_szRealName = NULL;
	}

	if (m_szUserName)
	{
		delete [] m_szUserName;
		m_szUserName = NULL;
	}

	if (m_szUserPwd)
	{
		delete [] m_szUserPwd;
		m_szUserPwd = NULL;
	}

	if (m_szSecurity)
	{
		delete [] m_szSecurity;
		m_szSecurity = NULL;
	}

	m_lUserModes = 0L;
}


BOOL CChatSocket::bCheckLogged(void)
{
	switch (m_csState)
	{
		case csDisconnected:
		case csConnecting:
		case csConnected:
		case csLogging:
			m_hrLastError = CC_E_NOTLOGGED;
			return FALSE;

		case csLogged:
			return TRUE;

		case csDisconnecting:
			m_hrLastError = CC_E_ALREADYDISCONNECTING;
			return FALSE;

		default:
			ASSERT(FALSE, "Unexpected Connection State in CChatSocket::bCheckLogged");
			m_hrLastError = E_FAIL;
			return FALSE;
	}
}


//=--------------------------------------------------------------------------=
// CChatSocket::bSetNickname
//=--------------------------------------------------------------------------=
// sets the m_szNickname and m_bstrNickname members, and also m_szUTF8Nickname if provided
//
// Parameters:
//    LPCTSTR		- [in] the ANSI version
//	  LPCTSTR		- [in] the UTF8 version
//	  BSTR			- [in] the UNICODE version
//
// Output:
//    BOOL			TRUE is successfull
//
// Notes:
//	  m_hrLastError contains error code in case of an error
//
BOOL CChatSocket::bSetNickname(LPCTSTR cszNickname, LPCTSTR cszUTF8Nickname, BSTR bstrNickname)
{
	// we do require a nickname - it cannot be empty
	if (!cszNickname || *cszNickname == g_chEOS)
	{
		m_hrLastError = CC_E_BADNICKNAME;
		return FALSE;
	}

	if (!bCopyStr(&m_szNickname, cszNickname))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	
	if (cszUTF8Nickname && !bCopyStr(&m_szUTF8Nickname, cszUTF8Nickname))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (m_bstrNickname)
		SysFreeString(m_bstrNickname);

	if (bstrNickname)
		m_bstrNickname = SysAllocString(bstrNickname);
	else
		m_bstrNickname = A2BSTR(cszNickname);

	if (!m_bstrNickname)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	return TRUE;
}


HRESULT CChatSocket::HrPostNewConnectionState(enumConnectionState csNewState)
{
	if (m_csState != csNewState)
	{
		if (!::PostMessage(m_hwndMess, WM_U_CONNECTIONSTATE, (WPARAM) (m_csState = csNewState), 0L))
			return GetLastError();
		else
			return NOERROR;
	}
	else
		return NOERROR;
}


HRESULT CChatSocket::HrPostNewChannelState(CChatChannel* pChannel, enumChannelState chsNewState)
{
	ASSERT(pChannel, "pChannel is NULL in CChatSocket::HrPostNewChannelState");
	
	if (pChannel->m_chs != chsNewState)
	{
		if (!::PostMessage(m_hwndMess, WM_U_CHANNELSTATE, (WPARAM) pChannel, (LPARAM) (pChannel->m_chs = chsNewState)))
			return GetLastError();
		else
			return NOERROR;
	}
	else
		return NOERROR;
}


HRESULT CChatSocket::HrPostEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bPostToEx)
{
	if (!::PostMessage(bPostToEx ? m_hwndMessEx : m_hwndMess, uMsg, wParam, lParam))
	{
		ASSERT(FALSE, "::PostMessage failed in CChatSocket::HrPostEvent");
		return GetLastError();
	}
	else
		return NOERROR;
}


HRESULT CChatSocket::HrSendEvent(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HRESULT hr = ::SendMessage(m_hwndMess, uMsg, wParam, lParam);

	ASSERT(SUCCEEDED(hr), ":SendMessage failed in CChatSocket::HrSendEvent");

	return hr;
}


//=--------------------------------------------------------------------------=
// CChatSocket::HrCloseChatSocket
//=--------------------------------------------------------------------------=
// closes the chat socket
//
// Parameters:
//    NONE
//
// Output:
//    BOOL
//
// Notes:
//
HRESULT CChatSocket::HrCloseChatSocket(BOOL bCleanUp)
{
	HRESULT hr1, hr2;

	OutputDebugThreadIdString("CChatSocket::HrCloseChatSocket - Enter\n");

	// Always LogOff before closing the socket
	hr1 = m_cconn.HrLogOff();
	if (CC_E_NOTLOGGED == hr1)
		hr1 = NOERROR;

	ASSERT(!m_cconn.bIsLoggedOn(), "Still Logged On in CChatSocket::HrCloseChatSocket");
	hr2 = m_cconn.HrClose(bCleanUp);
	
	return SUCCEEDED(hr1) ? hr2 : hr1;
}



BOOL CChatSocket::bFatalConnectionError(HRESULT hr)
{
	switch (hr)
	{
		case CC_E_LOSTCONNECTION:
		case CC_E_SOCKETCLOSED:
		case CC_E_NETWORKDOWN:
		case CC_E_HOSTDROPPEDCONNECTION:
			return TRUE;
		default:
			return FALSE;
	}
}


BOOL CChatSocket::bCloseAllChannels(void)
{
	CChatChannel		*pChannel = NULL;
	enumChannelState	chs = chsOpen;
	BOOL				bRet = TRUE;

	OutputDebugThreadIdString("CChatSocket::bCloseAllChannels - Closing open channels...\n");
	// Go through our open channels in the collection and notify the user of their closure
	while (0L < m_lOpenChannels)
	{
		if (m_listChannel.bFindCellFromData(1L,
											g_nSearchByState,
											(PVOID) &chs,
											g_nSearchNone,
											NULL,
											(PVOID*) &pChannel,
											NULL))
		{
			// Got an open channel
			ASSERT(pChannel, "pChannel is NULL in CChatSocket::bCloseAllChannels");
			bRet &= bLeftChannel(pChannel);
		}
	}

	return bRet;
}


//=--------------------------------------------------------------------------=
// CChatSocket::bLeftChannel
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bLeftChannel(CChatChannel* pChannel)
{
	// the user left a channel
	OutputDebugThreadIdString("CChatSocket::bLeftChannel - Enter\n");

	ASSERT(pChannel, "pChannel is NULL in CChatSocket::bLeftChannel");

	// update the currently open channel count
	InterlockedDecrement(&m_lOpenChannels);

	ASSERT(m_lOpenChannels >= 0, "m_lOpenChannels < 0 in CChatSocket::bLeftChannel");

	m_hrLastError = HrPostNewChannelState(pChannel, chsClosed);

	// empty the member hash table
	pChannel->bEmptyMemberTable();

	// ...and also reset the bGostMemList boolean for the next session, etc...
	pChannel->ResetChannelInfo();

	if (m_csState == csDisconnecting && 0L == m_lOpenChannels)
		// all the channels were closed, we now want to close the socket connection
		bCloseConnection(FALSE /*bSync*/);

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bGetSecurityPackages
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bGetSecurityPackages(LPTSTR szSecurity, DWORD cbSecurity, BOOL* pbAnonymousAllowed)
{
	PPRSECURITY	pPrSecurity = NULL;

	OutputDebugThreadIdString("CChatSocket::bGetSecurityPackages - Enter\n");

	ASSERT(szSecurity || pbAnonymousAllowed, "szSecurity and pbAnonymousAllowed are NULL in CChatSocket::bGetSecurityPackages");

	switch (m_csState)
	{
		case csDisconnected:
		case csConnecting:
			m_hrLastError = CC_E_NOTCONNECTED;
			return FALSE;

		case csDisconnecting:
			m_hrLastError = CC_E_ALREADYDISCONNECTING;
			return FALSE;

		// case csLogging:
		// case csConnected:
		// case csLogged:
			// we're fine
	}

	// Get the server's available security packages
	if (FAILED(m_hrLastError = m_cconn.HrGetSecurity(&pPrSecurity)))
		return FALSE;

	ASSERT(pPrSecurity, "pPrSecurity is NULL in CChatSocket::bGetSecurityPackages");

	if (pbAnonymousAllowed)
		*pbAnonymousAllowed = pPrSecurity->bAnonAllowed;

	if (szSecurity)
	{
		if (pPrSecurity->cPackages > 0)
		{
			// go through all the available packages
			DWORD	cbLen = 0L;
			SHORT	cCnt = 1;
			LPTSTR	szPackageAvailable;

			szPackageAvailable = (LPTSTR) pPrSecurity + sizeof(PRSECURITY);

			cbLen = lstrlen(szPackageAvailable);
			if (cbLen >= cbSecurity)
			{
				m_hrLastError = CC_E_BUFFERTOOSMALL;
				return FALSE;
			}
			else
				lstrcpyn(szSecurity, szPackageAvailable, cbLen+1);

			while (cCnt < pPrSecurity->cPackages)
			{
				cCnt++;
				szPackageAvailable += lstrlen(szPackageAvailable) + 1;
				cbLen += lstrlen(szPackageAvailable) + 1;
				if (cbLen >= cbSecurity)
				{
					m_hrLastError = CC_E_BUFFERTOOSMALL;
					return FALSE;
				}
				else
				{
					lstrcat(szSecurity, g_szSColon);
					lstrcat(szSecurity, szPackageAvailable);
				}
			}
		}
		else
			*szSecurity = g_chEOS;
	}

	return TRUE;
}


//=--------------------------------------------------------------------------=
// CChatSocket::HrLogin
//=--------------------------------------------------------------------------=
// 
//
// Parameters:
//    NONE
//
// Output:
//    HRESULT
//
// Notes:
//    
HRESULT CChatSocket::HrLogin(void)
{
	HRESULT			hr = NOERROR;
	PPRSECURITY		pPrSecurity = NULL;

	OutputDebugThreadIdString("CChatSocket::HrLogin - Enter\n");

	ASSERT(!m_cconn.bIsLoggedOn(), "Already Logged On in CChatSocket::HrLogin");

	HrPostNewConnectionState(csLogging);

	// If we deal with an IRCX server and the nick is extended, then we'd rather use the
	// UTF8 encoded nickname
	if (bIsIrcXSocket() && m_szUTF8Nickname)
		if (!bCopyStr(&m_szNickname, m_szUTF8Nickname))
			return E_OUTOFMEMORY;

	if (!bIsIrcXSocket())
		// if we're using an IRC socket, we can only try an anonymous login
		return HrTryLogin(NULL);

	// Get the server's available security packages
	if (FAILED(hr = m_cconn.HrGetSecurity(&pPrSecurity)))
		return hr;

	ASSERT(pPrSecurity, "pPrSecurity is NULL in CChatSocket::HrLogin");

	if (!lstrcmpi(m_szSecurity, g_szAnon))
	{
		if (!pPrSecurity->bAnonAllowed)
			// try to log in anonymously and server only allows authentication
			return CC_E_AUTHENTICATEDONLY;
		else
			// try an anonymous login
			return HrTryLogin(NULL);
	}

	if (m_szSecurity[0] == g_chEOS)
	{
		// need to try first available authentication or then anonymous
		if (pPrSecurity->cPackages > 0)
			// there is at least one security package available, let's try it
			// get the first security package listed
			return HrTryLogin((LPTSTR) pPrSecurity + sizeof(PRSECURITY));
		else
			// server doesn't provide any authentication, let's try an anonymous login
			return HrTryLogin(NULL);
	}

	// need to treat a list of security packages that might include the ANON keyword
	// let's trigger the HrTryLogin for the first item in m_szSecurity

	return HrIterateLogin(pPrSecurity, CC_E_LOGINFAILED);
}


//=--------------------------------------------------------------------------=
// CChatSocket::HrIterateLogin
//=--------------------------------------------------------------------------=
HRESULT CChatSocket::HrIterateLogin(PPRSECURITY pPrSecurity, HRESULT hrLatest)
{
	HRESULT			hr = NOERROR;
	HRESULT			hrLogin = NOERROR;
	TCHAR			szPackageToTry[g_nMaxLengthSmall+1];
	LPTSTR			szPackageIndex = NULL;
	LPTSTR			szPackageAvailable = NULL;

	OutputDebugThreadIdString("CChatSocket::HrIterateLogin - Enter\n");

	ASSERT(!m_cconn.bIsLoggedOn(), "Already Logged On in CChatSocket::HrIterateLogin");

	if (g_chEOS == m_szSecurity[0] || m_bCancelOperation)
		return m_bCancelOperation ? NOERROR : hrLatest;

	// try the first package listed in m_szSecurity
	INT i = 0;
	INT j = lstrlen(m_szSecurity);
	while (m_szSecurity[i] != g_chEOS && m_szSecurity[i] != g_chSColon)
	{
		szPackageToTry[i] = m_szSecurity[i];
		i++;
	}
	szPackageToTry[i] = g_chEOS;

	// update m_szSecurity
	if (m_szSecurity[i] == g_chEOS)
		m_szSecurity[0] = g_chEOS;
	else
		MoveMemory((PVOID) m_szSecurity, (PVOID) (m_szSecurity+i+1), j-i);

	// is this package available on the server?

	if (!lstrcmpi(szPackageToTry, g_szAnon))
	{
		if (pPrSecurity->bAnonAllowed)
		{
			// try an anonymous login
			if (SUCCEEDED(hr = HrTryLogin(NULL)))
				return NOERROR;
			else
				return HrIterateLogin(pPrSecurity, hr);
		}
		else
			return HrIterateLogin(pPrSecurity, hrLatest);
	}

	// let's treat each package listed in m_szSecurity
	if (pPrSecurity->cPackages > 0)
	{
		// go through all the available packages
		SHORT cCnt = 1;
		szPackageAvailable = (LPTSTR) pPrSecurity + sizeof(PRSECURITY);

		while (cCnt <= pPrSecurity->cPackages && lstrcmpi(szPackageToTry, szPackageAvailable))
		{
			cCnt++;
			szPackageAvailable += lstrlen(szPackageAvailable)+1;
		}
		
		if (cCnt <= pPrSecurity->cPackages)
		{
			// found an available package
			if (SUCCEEDED(hr = HrTryLogin(szPackageAvailable)))
				return NOERROR;
			else
				return HrIterateLogin(pPrSecurity, hr);
		}
		else
			// this package is not available on the server, try the next one
			return HrIterateLogin(pPrSecurity, CC_E_AUTHNOTAVAIL);
	}
	else
		// no packages are available on the server at all
		return CC_E_AUTHNOTAVAIL;
}


//=--------------------------------------------------------------------------=
// CChatSocket::HrTryLogin
//=--------------------------------------------------------------------------=
HRESULT CChatSocket::HrTryLogin(LPCTSTR szSecurityPackage)
{
	HRESULT hr = NOERROR;

#ifdef DEBUG
	if (szSecurityPackage)
	{
		sprintf(g_szDebugStr, "CChatSocket::HrTryLogin - Enter with package=%s\n", szSecurityPackage);
		OutputDebugThreadIdString(g_szDebugStr);
	}
	else
		OutputDebugThreadIdString("CChatSocket::HrTryLogin - Enter anonymous\n");
#endif

	if (m_bCancelOperation)
	{
		OutputDebugThreadIdString("CChatSocket::HrTryLogin - Login Cancelled\n");
		return CC_S_LOGINCANCELLED;
	}
	
	if (FAILED(hr = m_cconn.HrLogin(m_szNickname, m_szUserName, m_szRealName, m_szUserPwd, (CHAR*) szSecurityPackage)))
	{
		if (E_FAIL == hr || (SEC_E_INSUFFICIENT_MEMORY <= hr && SEC_E_INCOMPLETE_MESSAGE >= hr))
			hr = CC_E_AUTHFAILED;
		return hr;
	}
	return NOERROR;
}


//=--------------------------------------------------------------------------=
// CChatSocket::bCheckRating
//=--------------------------------------------------------------------------=
//
// Parameters:
//	  NONE
//
// Output:
//    BOOL
//
// Notes:
//
BOOL CChatSocket::bCheckRating(LPCTSTR szChannelName)
{
	LPTSTR				szUrl			= NULL;
	CChatItems			*pCItems		= NULL;
	LPTSTR				szRatingTmp		= NULL;
	HANDLE				hObtainQuery	= NULL;
	CRatingQueryData*	prqData			= NULL;
	LPVOID				pRatingDetails	= NULL;
	BOOL				bAccess			= FALSE;
	DWORD				dwPropertyAccessTimeOut = 0L;

	OutputDebugThreadIdString("CChatSocket::bCheckRating - Enter\n");

	m_hrLastError = NOERROR;

	ASSERT(m_hwndParent, "m_hwndParent is NULL in CChatSocket::bCheckRating");
	ASSERT(szChannelName, "szChannelName is NULL in CChatSocket::bCheckRating");

	// RamuM
	// Load the Ratings DLL (if possible)
	ENSURE_LOADED(g_hinstRatings, c_tszRatingsDLL);

	// If Rating DLL is not found assume ratings are not enabled.

	if ((NULL != g_hinstRatings) && (S_OK == RatingEnabledQuery()))
	{
		prqData = new CRatingQueryData;

		if (!prqData)
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
		if (!prqData->m_hEvent)
		{
			delete prqData;
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}

		// The actual S_OK or S_FALSE comes through callback later
		// and the function will not fail if there is some info
		// i.e this function never returns allow / deny directly!

		// construct the szUrl for this server and channel
		if (!(szUrl = new TCHAR[lstrlen(m_szServerName) + lstrlen(szChannelName) + 16]))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
		lstrcpy(szUrl, g_szIrcPrefix);
		lstrcat(szUrl, g_szDblSlash);
		lstrcat(szUrl, m_szServerName);
		lstrcat(szUrl, g_szSlash);
		lstrcat(szUrl, szChannelName);

		m_hrLastError = RatingObtainQuery(szUrl, (DWORD) prqData,
										  RatingObtainQueryCallback, &hObtainQuery);
		if (FAILED(m_hrLastError))
		{	
			// Means access depends on actual rating of the URL 
			RatingObtainCancel(hObtainQuery);
			CloseHandle(prqData->m_hEvent);
			prqData->m_hEvent = NULL;	// Clear the event so that we won't wait later.		
		}

		// Get the PICS rating from the IRCX server
		if (bIsIrcXSocket())
		{
			// We cash the timeout value and overwrite it for this internal synchronous call
			dwPropertyAccessTimeOut = m_dwPropertyAccessTimeOut;
			m_dwPropertyAccessTimeOut = (DWORD) g_lDefaultPropertyAccessTimeOut;

			if (bQueryChannelProperty(TRUE /*bSyncAccess*/, g_rgwszChannelPropertyNames[cpnRating], szChannelName, &pCItems))
			{
				VARIANT vItem;

				// pCItems was correctly set
				ASSERT(pCItems, "pCItems is NULL in CChatSocket::bCheckRating");
				if (pCItems->bGetItem((BSTR&) g_rgwszChannelPropertyNames[cpnRating], &vItem))
				{
					ASSERT(vItem.bstrVal, "vItem.bstrVal is NULL in CChatSocket::bCheckRating");

					USES_CONVERSION;
					if (!(szRatingTmp = W2T(vItem.bstrVal)))
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}
				}
				else
				{
					if (CC_E_ITEMNOTAVAILABLE != pCItems->HrGetLastError())
					{
						m_hrLastError = pCItems->HrGetLastError();
						goto exit;
					}
					// else the channel is unrated and szRatingTmp stays NULL
				}
			}
			else
			{
				// Querying the channel ratings failed
				switch (m_hrLastError)
				{
					case CC_E_NOSUCHCHANNEL:	// Unexisting channels are considered unrated
						break;

					//case CC_E_TIMEOUT:
					//	m_hrLastError = CC_E_LOGINFAILED;	// wrong error code
					default:
						goto exit;
				}
			}
		}
		// else szRatingTmp stays NULL for IRC channels (<==> IRC channels are considered unrated)

		if (S_OK == (m_hrLastError = RatingCheckUserAccess(NULL, szUrl, szRatingTmp, NULL, 0, &pRatingDetails)))
		{
			// User can access according to local rating. Access may eventually be allowed.
			bAccess = TRUE;
		}
		else if (S_FALSE == m_hrLastError)
		{
			// User cannot access according to local rating.  May still be overridden by callback though.
			bAccess = FALSE;		
		}

		// We rely on the fact that RatingCheckUserAccess(...., NULL, ....) for
		// checking unrated site access will never fail.
//
//		// else rating is in error, consider page unrated , ???? Do something
//		else 
//			if (FAILED(m_hrLastError))
//			{
//				// verify that user has access to unrated sites
//
//				// This rating detail takes precedence
//				if (pRatingDetails)
//					RatingFreeDetails(pRatingDetails);
//
//				if (NOERROR == (m_hrLastError = RatingCheckUserAccess(NULL, szUrl, NULL, NULL, 0, &pRatingDetails)))
//					// User can access according to local rating. Access may eventually be allowed.
//					bAccess = TRUE;
//			}
//

		// Check to see if we're done looking for ratings, and if so, allow or
		// deny access appropriately.
		if (prqData->m_hEvent) // There might be overriding local settings, so wait for RatingObtainQuery to finish
		{
			// Can possibly be doing other things instead of idle waiting
			if (WAIT_FAILED != WaitForSingleObject(prqData->m_hEvent, INFINITE))
			{
				if (NOERROR == prqData->m_hr)
					bAccess = TRUE;
				else 
					if (SUCCEEDED(prqData->m_hr))
					{
						bAccess = FALSE;
						// This rating detail takes precedence
						if (pRatingDetails)
							RatingFreeDetails(pRatingDetails);
						pRatingDetails = prqData->m_pRatingDetails ;
						prqData->m_pRatingDetails = NULL;	// So that the object won't free it
					}
					// else use the access based on rating of the channel	
			}
		}	

		// We are done with prqData so free it
		ASSERT(prqData, "prqData is NULL in CChatSocket::bCheckRating");
		delete prqData;

		if (!bAccess)
		{
			m_hrLastError = RatingAccessDeniedDialog(m_hwndParent, NULL, NULL, pRatingDetails);
			RatingFreeDetails(pRatingDetails);
			if (S_FALSE == m_hrLastError)
				m_hrLastError = CC_E_RATINGBLOCK;	
		}
	}

exit:
	if (szUrl)
		delete [] szUrl;

	if (pCItems)
		ReleaseChatItems(pCItems);

	if (dwPropertyAccessTimeOut)
		m_dwPropertyAccessTimeOut = dwPropertyAccessTimeOut;

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bDisconnect
//=--------------------------------------------------------------------------=
// closes the socket so that the user gets disconnected - this function does 
// not release the socket object - this has to be done in the functions currently
// using the socket
//
// Parameters:
//    NONE
//
// Output:
//    BOOL
//
// Notes:
//	the function returns NOERROR if the login was successful or an HRESULT
//  caller has to lock m_pics
//
BOOL CChatSocket::bDisconnect(void)
{
	OutputDebugThreadIdString("CChatSocket::bDisconnect - Enter\n");

	switch (m_csState)
	{
		case csDisconnected:
		case csConnecting:
			m_hrLastError = CC_E_NOTCONNECTED;
			return FALSE;

		case csDisconnecting:
			m_hrLastError = CC_E_ALREADYDISCONNECTING;
			return FALSE;

		case csLogging:
			m_hrLastError = CC_E_NOTLOGGED;
			return FALSE;
		
		// case csConnected:
		// case csLogged:
			// we're fine
	}

	ASSERT(m_csState == csLogged || m_csState == csConnected, "m_csState != csLogged && m_csState != csConnected in CChatSocket::bDisonnect");

	HrPostNewConnectionState(csDisconnecting);

	return bCloseConnection(FALSE);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bCloseConnection
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bCloseConnection(BOOL bSync, BOOL bStopPosting)
{
	BOOL bRet;
	
#ifdef DEBUG
	sprintf(g_szDebugStr, "CChatSocket::bCloseConnection - Enter bSync=%d, bStopPosting=%d\n", bSync, bStopPosting);
	OutputDebugThreadIdString(g_szDebugStr);
#endif // DEBUG

	if (bStopPosting)
		m_cconn.SetStopPosting(TRUE);

	// if we still have open channels, we close them first
	if (m_lOpenChannels > 0)
	{
		OutputDebugThreadIdString("CChatSocket::bCloseConnection - Parting open channels...\n");
		bRet = m_listChannel.bDoActionOnList(g_nActionLeave);
		ASSERT(bRet, "bDoActionOnList(g_nActionLeave) failed in CChatSocket::bCloseConnection");

		if (!bSync)
			return bRet;
	}

	OutputDebugThreadIdString("CChatSocket::bCloseConnection - Calling HrCloseChatSocket\n");
	if (FAILED(m_hrLastError = HrCloseChatSocket(FALSE)))
		return FALSE;

	if (bSync)
	{
		OutputDebugThreadIdString("CChatSocket::bCloseConnection - Waiting for reader thread to die\n");
		m_hrLastError = HrWaitForThreadDead(&m_hMsgThread);
		HrPostNewConnectionState(csDisconnected);
		return SUCCEEDED(m_hrLastError);
	}
	else
	{
		if (!m_hMsgThread)
			return SUCCEEDED(m_hrLastError = HrPostNewConnectionState(csDisconnected));
		else 
			return TRUE;
	}
}


//=--------------------------------------------------------------------------=
// CChatSocket::HrWaitForThreadDead
//=--------------------------------------------------------------------------=
HRESULT CChatSocket::HrWaitForThreadDead(HANDLE *phThread)
{
	HRESULT hr = NOERROR;

	ASSERT(phThread, "phThread is NULL in CChatSocket::HrWaitForThreadDead");

	if (*phThread)
	{
		// wait for the msg thread to finish up.
		if (WAIT_TIMEOUT == ::WaitForSingleObject(*phThread, 8*g_dwBigTimeout))
		{
			ASSERT(FALSE, "Thread did not terminate in CChatSocket::HrWaitForThreadDead");
			hr = CC_E_TIMEOUT;
			// let's terminate the thread anyway
			::TerminateThread(*phThread, 0L);
			::CloseHandle(*phThread);
			*phThread = NULL;
		}
	}
	return hr;
}


//=--------------------------------------------------------------------------=
// CChatSocket::HrThreadProcSocketLoop
//=--------------------------------------------------------------------------=
HRESULT CChatSocket::HrThreadProcSocketLoop(void)
{
	HRESULT hr = NOERROR;
	BOOL	bWaitForLogin = FALSE;
	DWORD	dwIdentDThreadId;

	OutputDebugThreadIdString("CChatSocket::HrThreadProcSocketLoop - Enter\n");

	if (FAILED(hr = m_cconn.HrConnection(m_szServerName, m_uPort)) && !m_bCancelOperation)
		HrPostEvent(WM_U_SOCKET_HRESULT, NULL, (LPARAM) hr);

	if (FAILED(hr) || m_bCancelOperation)
	{
		m_bCancelOperation = FALSE;
		HrCloseChatSocket();
		goto exit;
	}

	ASSERT(!m_hIdentDThread, "m_hIdentDThread is NOT NULL in CChatSocket::HrThreadProcSocketLoop");
	// Launch the IdentD thread before checking if the server is an IRCX server
	m_bTryIdentD = TRUE;
	m_hIdentDThread = ::CreateThread(NULL,
									 0,
									 DwSocketThreadProcIdentDLoop,
									 this,
									 0,
									 &dwIdentDThreadId);

	// Let's try to figure out if this is an IRCX server
	if (FAILED(hr = hr = m_cconn.HrIdentifyServer(m_szServerName)) && !m_bCancelOperation)
		HrPostEvent(WM_U_SOCKET_HRESULT, NULL, (LPARAM) hr);

	if (FAILED(hr) || m_bCancelOperation)
	{
		m_bCancelOperation = FALSE;
		HrCloseChatSocket();
		goto exit;
	}

	// some server properties are now local
	m_rgbServerPropLocal[pnServerName]			   = \
	m_rgbServerPropLocal[pnServerIgnoredUsers]	   = \
	m_rgbServerPropLocal[pnServerMaxMessageLength] = \
	m_rgbServerPropLocal[pnServerAnonymousAllowed] = \
	m_rgbServerPropLocal[pnServerSecurityPackages] = TRUE;

	while (!m_cconn.bIsLoggedOn())
	{
		if (!m_bLoginAlso || bWaitForLogin)
		{
			// we don't want to auto login, so let's wait until the user wants us to login
			if (!m_bLoginAlso && !bWaitForLogin)
				HrPostNewConnectionState(csConnected);
			do
			{
				hr = m_cconn.HrIsDataWaiting(0, 400000);	// we only wait for 400000 microseconds
				switch (hr)
				{
					case CC_E_NOSOCKETDATA:
						// nothing's here to eat
						hr = NOERROR;
						break;
					case NOERROR:
						// we have some meat available
						hr = m_cconn.HrReaderThread(FALSE);
						break;
					//default:
						// we lost the socket
				}
			}
			while (SUCCEEDED(hr) && !m_bLoginAlso && !m_cconn.bIsLoggedOn());

			if (FAILED(hr))	
			{
				if (m_csState != csDisconnecting && !m_bCancelOperation)
					HrPostEvent(WM_U_SOCKET_HRESULT, NULL, (LPARAM) hr);
				goto exit;
			}
		}

		if (!m_cconn.bIsLoggedOn())
		{
			// we're ready to login
			m_bLoginAlso = FALSE;
			if (FAILED(hr = HrLogin()) && !m_bCancelOperation)
				HrPostEvent(WM_U_SOCKET_HRESULT, NULL, (LPARAM) hr);

			if (FAILED(hr) || m_bCancelOperation)
			{
				m_bCancelOperation = FALSE;
				HrCloseChatSocket();

				// Generate the OnConnectionState event and exit the thread
				goto exit;
			}
			else
				bWaitForLogin = TRUE;
		}
	}

	ASSERT(m_cconn.bIsLoggedOn(), "We're not yet logged on in CChatSocket::HrThreadProcSocketLoop");

	m_bTryIdentD = FALSE;	// This will stop the IdentD thread if it's still alive

	// we are definitely logged on at this point!
	if (FAILED(hr = m_cconn.HrReaderThread()) && m_csState != csDisconnecting)
	{
		if (bFatalConnectionError(hr) && m_csState != csDisconnecting && !m_cconn.bStoppedPosting())
			HrSendEvent(WM_U_CLOSECHANNELS, 0L, 0L);	// only close synchronously the channels if the control is not being destroyed

		HrPostEvent(WM_U_SOCKET_HRESULT, NULL, (LPARAM) hr);
		HrCloseChatSocket();
	}

exit:
	m_bTryIdentD = FALSE;	// This will stop the IdentD thread if it's still alive

	// Make sure the IdentD thread dies before this one
	HrWaitForThreadDead(&m_hIdentDThread);

	HrPostNewConnectionState(csDisconnected);

	// Exit this thread
	CloseHandle(m_hMsgThread);
	m_hMsgThread = NULL;
	OutputDebugThreadIdString("CChatSocket::HrThreadProcSocketLoop - Leave\n");
	ExitThread(hr);

	return hr;
}


//=--------------------------------------------------------------------------=
// CChatSocket::HrThreadProcIdentDLoop
//=--------------------------------------------------------------------------=
HRESULT CChatSocket::HrThreadProcIdentDLoop(void)
{
	HRESULT hr;

	OutputDebugThreadIdString("CChatSocket::HrThreadProcIdentDLoop - Enter\n");

	hr = m_cIdentD.HrCreateStreamSocket();
	ASSERT(SUCCEEDED(hr), "HrCreateStreamSocket failed in CChatSocket::HrThreadProcIdentDLoop");
	hr = m_cIdentD.HrBindToPortAndListen(113);
	ASSERT(SUCCEEDED(hr), "HrBindToPortAndListen failed in CChatSocket::HrThreadProcIdentDLoop");

	if (FAILED(hr))
		goto exit;

	// First try to accept a connection
	while (NOERROR != m_cIdentDAccept.HrIsSocketValid() && m_bTryIdentD)
	{
		// We haven't accepted a connection yet, let's see if the server is knocking at our door...
		hr = m_cIdentD.HrAccept(&m_cIdentDAccept);
		#ifdef DEBUG
			if (SUCCEEDED(hr))
				OutputDebugThreadIdString("CChatSocket::HrThreadProcIdentDLoop - Accepted a connection for IdentD\n");
		#endif // DEBUG
		if (FAILED(hr))
			Sleep(500);	// Sleep half a second
	}

	// We accepted a connection. Did we already send our ID?
	while (!m_cIdentDAccept.bAuthenticated() && m_bTryIdentD)
	{
		hr = m_cIdentDAccept.HrAuthenticate(m_szUserName);
		#ifdef DEBUG
			if (NOERROR == hr)
				OutputDebugThreadIdString("CChatSocket::HrThreadProcIdentDLoop - Authenticated with IdentD\n");
		#endif // DEBUG
		if (FAILED(hr))
			Sleep(500);	// Sleep half a second
	}

exit:
	m_cIdentD.Close();
	m_cIdentDAccept.Close();

	// Exit this thread
	CloseHandle(m_hIdentDThread);
	m_hIdentDThread = NULL;
	OutputDebugThreadIdString("CChatSocket::HrThreadProcIdentDLoop - Leave\n");
	ExitThread(hr);

	return hr;
}


//=--------------------------------------------------------------------------=
// CChatSocket::bFindChannel
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bFindChannel(SHORT nSearchId1, PVOID pvCriteria1, SHORT nSearchId2, PVOID pvCriteria2, CChatChannel **ppChannel)
{								
	OutputDebugThreadIdString("CChatSocket::bFindChannel - Enter\n");

	ASSERT(ppChannel, "ppChannel is NULL in CChatSocket::bFindChannel");

	*ppChannel = NULL;

	BOOL bRet = m_listChannel.bFindCellFromData(-1L,
												nSearchId1,
												pvCriteria1,
												nSearchId2,
												pvCriteria2,
												(PVOID*) ppChannel,
												NULL);
	return bRet;
}


//=--------------------------------------------------------------------------=
// CChatSocket::bCreateJoinChannel
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bCreateJoinChannel(CChatChannel* pChannel, LPTSTR szChannelKey, BOOL bCreate)
{
	BYTE				*pb;
	INT					cb;
	CChatChannel		*pChannelTmp = NULL;
	enumChannelState	chs	= chsOpen;

	OutputDebugThreadIdString("CChatSocket::bCreateJoinChannel - Enter\n");

	ASSERT(pChannel, "pChannel is NULL in CChatSocket::bCreateJoinChannel");

	if (!bCheckLogged())
		return FALSE;

	if (!pChannel->m_prChannel.szName)
	{
		m_hrLastError = CC_E_CHANNELBADNAME;
		return FALSE;
	}

	ASSERT(m_hMsgThread, "m_hMsgThread is NULL in CChatSocket::bCreateJoinChannel");

	// we are already connected and logged, we just want to enter a channel

	ASSERT(pChannel->m_chs == chsClosed, "Channel is not closed in CChatSocket::bCreateJoinChannel");

	// update the channel's IRC(X) mode
	pChannel->SetIrcXChannel(bIsIrcXSocket());

	// make sure that we are not in that IRC(X) channel already
	// we check this synchronously because IRC servers don't reply
	// with an error code when the client tries to join a channel
	// he's already in
	if (m_listChannel.bFindCellFromData(1L /*lPosition*/, 
										g_nSearchByState /*nSearchID1*/,
										(PVOID) &chs,
										g_nSearchByName /*nSearchID2*/,
										(PVOID) pChannel->m_prChannel.szName,
										(PVOID*) &pChannelTmp,
										NULL /*plPositionFound*/))
	{
		m_hrLastError = CC_E_ALREADYONCHANNEL;
		return FALSE;
	}

	// On IRC servers, we cannot CREATE channels, or JOIN an extended channel
	if (!bIsIrcXSocket() && (bCreate || g_chExtChnPfx == pChannel->m_prChannel.szName[0]))
	{
		m_hrLastError = CC_E_NOTIRCX;
		return FALSE;
	}

	if (bCreate && (pChannel->m_prChannel.lModes & cmClone ||
					pChannel->m_prChannel.lModes & cmRegistered ||
					pChannel->m_prChannel.lModes & cmService))
		{
			m_hrLastError = CC_E_CHANNELBADMODES;
			return FALSE;
		}

	// Make sure that there is no extended character in the channel name, when there is no % prefix
	if (g_chExtChnPfx != pChannel->m_prChannel.szName[0] && 
		bExtendedChannelName(pChannel->m_prChannel.szName, TRUE /*bAcceptUpperAnsi*/))
	{
		m_hrLastError = CC_E_CHANNELBADNAME;
		return FALSE;
	}

	HrPostNewChannelState(pChannel, chsOpening);

	if (!bCreate || !(pChannel->m_prChannel.lModes & cmCreateOnly))
		if (!bCheckRating(pChannel->m_prChannel.szName))
			goto exit;

	if (bCreate)	// CREATE channel operation
	{
		if (!m_cconn.bCreateChannel(pChannel->m_prChannel.szName,
									szChannelKey,
									pChannel->m_prChannel.lModes,
									pChannel->m_prChannel.lMaxMemberCount))
		{
			m_hrLastError = m_cconn.HrGetLastError();
			goto exit;
		}
	}
	else								
	{
		// JOIN channel operation
		// Reset the topic if necessary
		if (pChannel->m_prChannel.szTopic)
			*(pChannel->m_prChannel.szTopic) = g_chEOS;

		if (SUCCEEDED(m_hrLastError = HrMakeJoinMsg(&pb, &cb, pChannel->m_prChannel.szName, 
															  szChannelKey)))
			m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);

		if (FAILED(m_hrLastError))
			goto exit;
	}

	// Everything succeeded
	InterlockedIncrement(&m_lOpeningChannels);
	return TRUE;

exit:
	// Error occurred after posting Opening channel state
	HrPostNewChannelState(pChannel, chsClosed);
	return FALSE;
}


//=--------------------------------------------------------------------------=
// CChatSocket::bConnect
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bConnect(LPTSTR szServerName, BOOL bLoginAlso)
{
	DWORD	dwMsgThreadId;
	UINT	cch, uPort;

	OutputDebugThreadIdString("CChatSocket::bConnect - Enter\n");

	switch (m_csState)
	{
		case csDisconnected:
		case csConnected:
			break;
		case csConnecting:
			m_hrLastError = CC_E_ALREADYCONNECTING;
			return FALSE;
		case csLogging:
			m_hrLastError = CC_E_ALREADYLOGGING;
			return FALSE;
		case csLogged:
			m_hrLastError = CC_E_ALREADYLOGGED;
			return FALSE;
		case csDisconnecting:
			m_hrLastError = CC_E_ALREADYDISCONNECTING;
			return FALSE;
	}

	ASSERT(m_csState == csDisconnected || m_csState == csConnected, "m_csState != csDisconnected && m_csState != csConnected in CChatSocket::bConnect");

	// Parse the servername:port param
	if (FAILED(HrParseServerName((CHAR*) szServerName, &cch, &uPort)))
	{
		m_hrLastError = E_INVALIDARG;
		return FALSE;
	}
	szServerName[cch] = g_chEOS;

	// do we want to login or connect?
	if (m_csState == csDisconnected)
	{	
		ASSERT(!m_hMsgThread, "m_hMsgThread is NOT NULL in CChatSocket::bConnect");

		// we want to connect first
		// do we also want to login afterwards?
		m_bLoginAlso = bLoginAlso;

		// set the internal port number
		m_uPort = uPort;

		// set the internal m_szServerName value
		if (!bCopyStr(&m_szServerName, szServerName))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		
		// set the initial user modes value
		m_lUserModes = umNotClientIgnored|\
					   umNotAway|\
					   umNotInvisible|\
					   umNotSysop|\
					   umNotAdmin|\
					   umNoNotices|\
					   umNoWallops;

		m_bCancelOperation = FALSE;

		// Reset the m_bCancel flag in CConnection object
		m_cconn.SetCancel(FALSE);

		m_hMsgThread = ::CreateThread(NULL,
									  0,
									  DwSocketThreadProcSocketLoop,
									  this,
									  0,
									  &dwMsgThreadId);

		if (m_hMsgThread)
		{
			#ifdef DEBUG
				sprintf(g_szDebugStr, "CChatSocket::bConnect - Created thread %X\n", dwMsgThreadId);
				OutputDebugThreadIdString(g_szDebugStr);
			#endif // DEBUG
			HrPostNewConnectionState(csConnecting);
			return TRUE;
		}
		else
		{
			m_hrLastError = GetLastError();
			return FALSE;
		}
	}
	else
	{
		// we are already connected
		ASSERT(m_csState == csConnected, "m_csState != csConnected in CChatSocket::bConnect");

		if (!bLoginAlso)
		{
			// We are already connected and waiting for a Login operation
			m_hrLastError = CC_E_ALREADYCONNECTED;
			return FALSE;
		}

		// make sure that we are connected to the same server and port
		if (0 != lstrcmpi(szServerName, m_szServerName) ||
			uPort != m_uPort)
		{
			// the server names are not the same, argh!
			m_hrLastError = CC_E_DISCONNECTFIRST;
			return FALSE;
		}

		// we just want to login
		m_bLoginAlso = TRUE;	// This will trigger the login operation in the second thread
		return TRUE;
	}
}


//=--------------------------------------------------------------------------=
// CChatSocket::bCancelConnectLogin
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bCancelConnectLogin(void)
{
	OutputDebugThreadIdString("CChatSocket::bCancelConnectLogin - Enter\n");
	
	switch (m_csState)
	{
		case csDisconnected:
		case csConnected:
		case csLogged:
		case csDisconnecting:
			m_hrLastError = CC_E_NOOPINPROGRESS;
			return FALSE;

		case csConnecting:
		case csLogging:
			m_bCancelOperation = TRUE;

			if (!bCloseConnection(TRUE))
				goto failure;
			else
				return TRUE;
		
		default:
			ASSERT(FALSE, "Unexpected Connection State in CChatSocket::bCancelConnectLogin");
			m_hrLastError = E_FAIL;
	}
	
failure:
	m_bCancelOperation = FALSE;
	return FALSE;
}


BOOL CChatSocket::bSendPrivateMessage(LONG pmt, VARIANT *pvMessage, VARIANT *pvRecipientNicknames, VARIANT *pvTag)
{
	PBYTE	rgbMsg = NULL;
	DWORD	dwcb, cbLen = 0L;
    LONG	cRcp;
	LPTSTR	szRcpNicks = NULL, szTagTmp = NULL, szMessageTmp = NULL, szMessageToSend;
	BOOL	bPostProcess, bFreeMessageToSend = FALSE, bFreeMessageTmp = FALSE;
	SHORT	nCmd;

	m_hrLastError = NOERROR;	// No Error by default

	USES_CONVERSION;

	if (pvRecipientNicknames->vt == (VT_BYREF+VT_VARIANT))
		pvRecipientNicknames = pvRecipientNicknames->pvarVal;

	if (pvMessage->vt == (VT_BYREF+VT_VARIANT))
		pvMessage = pvMessage->pvarVal;

	if (pvTag->vt == (VT_BYREF+VT_VARIANT))
		pvTag = pvTag->pvarVal;

	// Make sure the pmt param is valid
	if (
		pmtNormal					!= pmt &&
	    pmtNotice					!= pmt &&
		pmtData						!= pmt &&
		pmtDataRequest				!= pmt &&
		pmtDataReply				!= pmt &&
		pmtData+pmtDataRaw			!= pmt &&
		pmtDataRequest+pmtDataRaw	!= pmt &&
		pmtDataReply+pmtDataRaw		!= pmt &&
		pmtVersion					!= pmt &&
		pmtLagTime					!= pmt &&
		pmtLocalTime				!= pmt &&
		pmtAction					!= pmt &&
		pmtSound					!= pmt &&
		pmtCTCP						!= pmt &&
		pmtVersion+pmtNotice		!= pmt &&
		pmtLagTime+pmtNotice		!= pmt &&
		pmtLocalTime+pmtNotice		!= pmt &&
		pmtAction+pmtNotice			!= pmt &&
		pmtSound+pmtNotice			!= pmt &&
		pmtCTCP+pmtNotice			!= pmt
	   )
	{
		m_hrLastError = CC_E_BADMSGTYPE;
		goto exit;
	}

	// make sure pvMessage != NULL
	if (!pvMessage)
	{
		m_hrLastError = CC_E_NULLMSG;
		goto exit;
	}

	// Construct the recipient list
	if (!bConstructRecipientList(pvRecipientNicknames, &szRcpNicks, &cRcp))
		goto exit;

	if (0L == cRcp)
	{
		// No specific nickname was specified
		m_hrLastError = CC_E_INVALIDRECIPIENTLIST;
		goto exit;
	}

	// Convert Tag to ANSI
    if (!pvTag ||
		!(
		  (pvTag->vt == VT_EMPTY) || 
		  (pvTag->vt == VT_NULL)  || 
		  (pvTag->vt == VT_ERROR) || 
		  (pvTag->vt == VT_BSTR)
		 )
		)
	{
		m_hrLastError = CC_E_BADTAG;
		goto exit;
	}
	if (pvTag->vt == VT_BSTR && pvTag->bstrVal && g_chEOS != *pvTag->bstrVal)
	{
		if (!(szTagTmp = W2T(pvTag->bstrVal)))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
	}
	if (
		(szTagTmp && !bIsPmtData(pmt)) ||
		(!szTagTmp && bIsPmtData(pmt))
	   )
	{
		m_hrLastError = CC_E_BADTAG;		// There is a tag but pmtData, pmtDataRequest, pmtDataReply are not set, or vice versa
		goto exit;
	}

	if (bIsPmtData(pmt))
	{
		nCmd = NCmdIdFromPmt(pmt); 
		// Data stream case
		if (!bConstructDataStream(pvMessage, &rgbMsg, &dwcb, (BOOL) (pmt & pmtDataRaw), &bPostProcess))
			goto exit;
	}
	else
	{
		// Non-data stream case

		// Convert message to ANSI if user wants to send a string
		if (pvMessage->vt == VT_BSTR && pvMessage->bstrVal)
		{
			// Might have to convert private message from ShiftJis to Jis, etc...
			LPTSTR szTmp;
			if (!(szTmp = W2T(pvMessage->bstrVal)) ||
				!bConvertString(FALSE, m_byteCharSet, szTmp, 0, &szMessageTmp, NULL, &bFreeMessageTmp))
			{
				m_hrLastError = E_OUTOFMEMORY;
				goto exit;
			}
		}

		switch (pmt & ~pmtNotice)
		{
			case pmtNormal:
				// If the message is a 0 byte string then don't send anything and throw an error
				if (!szMessageTmp || g_chEOS == szMessageTmp[0])
				{
					m_hrLastError = CC_E_NULLMSG;
					goto exit;
				}
				szMessageToSend = szMessageTmp;
				break;

			case pmtVersion:
			case pmtLagTime:
			case pmtLocalTime:
			case pmtAction:
			case pmtSound:
			case pmtCTCP:
				szMessageToSend = new TCHAR[cbLen = (szMessageTmp ? lstrlen(szMessageTmp) + 32 : 32)];
				if (!szMessageToSend)
				{
					m_hrLastError = E_OUTOFMEMORY;
					goto exit;
				}
				else
				{
					bFreeMessageToSend = TRUE;
					ZeroMemory(szMessageToSend, cbLen);
				}

				szMessageToSend[0] = g_byteCTCPBrace;
				switch (pmt & ~pmtNotice)
				{
					case pmtVersion:
						lstrcpy(szMessageToSend+1, m_rgszComic[IDS_VERSION-IDS_COMICSTART]);
						break;

					case pmtLagTime:
						lstrcpy(szMessageToSend+1, m_rgszComic[IDS_LAGTIME-IDS_COMICSTART]);
						break;
					
					case pmtLocalTime:
						lstrcpy(szMessageToSend+1, m_rgszComic[IDS_LOCALTIME-IDS_COMICSTART]);
						break;
					
					case pmtAction:
						lstrcpy(szMessageToSend+1, m_rgszComic[IDS_ACTION-IDS_COMICSTART]);
						break;
					
					case pmtSound:
						lstrcpy(szMessageToSend+1, m_rgszComic[IDS_SOUND-IDS_COMICSTART]);
						break;
					
					case pmtCTCP:
						if (!szMessageTmp || g_chEOS == szMessageTmp[0])
						{
							m_hrLastError = CC_E_NULLMSG;
							goto exit;
						}
						break;
				}
				if (szMessageTmp && *szMessageTmp)
				{
					if (!(pmt & pmtCTCP))
						lstrcat(szMessageToSend, g_szSpace);
					lstrcat(szMessageToSend, szMessageTmp);
				}
				szMessageToSend[lstrlen(szMessageToSend)] = g_byteCTCPBrace;
				break;

			default:
				m_hrLastError = CC_E_BADMSGTYPE;
				goto exit;
		}
	}

	// Finally send the message
	if (bIsPmtData(pmt))
		bSendData((LPCTSTR) szRcpNicks /*szTarget*/, NULL /*szRcpNicks*/, (LPCTSTR) szTagTmp, rgbMsg, dwcb, nCmd, bPostProcess);
	else
		bSendPrivateText(szRcpNicks, szMessageToSend, pmt & pmtNotice);

exit:
	if (rgbMsg)
		delete [] rgbMsg;

	if (szMessageToSend && bFreeMessageToSend)
		delete [] szMessageToSend;

	if (szMessageTmp && bFreeMessageTmp)
		delete [] szMessageTmp;

	if (szRcpNicks)
		delete [] szRcpNicks;

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bSendPrivateText
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bSendPrivateText(LPCTSTR szTarget, LPCTSTR szText, BOOL bNotice)
{
	BYTE	*pb;
	INT		cb;
	LPTSTR	szTmp;
	BOOL	bFreeTmp;

	// szTarget can be one nickname, or a comma separated list of nicknames

	OutputDebugThreadIdString("CChatSocket::bSendPrivateText - Enter\n");

	if (!bCheckLogged())
		return FALSE;

	// Might have to quote \r and \n
	if (!bLowLevelQuoting(g_chLLQuoteCTCP, TRUE /*bTreatAsByteArray*/, szText, &szTmp, &bFreeTmp))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (SUCCEEDED(m_hrLastError = HrMakeSendText(&pb, &cb, bNotice, FALSE /*bWhisper*/, (CHAR*) szTarget, NULL, (CHAR*) szTmp)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);

	if (bFreeTmp)
		delete [] szTmp;

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bSendData
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bSendData(LPCTSTR szTarget, LPCTSTR szRcpNicks, LPCTSTR szTag, BYTE *pbData, DWORD dwcb, SHORT nCmd, BOOL bPostProcess)
{
	BYTE	*pb;
	INT		cb;
	LPTSTR	szData = NULL;

	OutputDebugThreadIdString("CChatSocket::bSendData - Enter\n");

	ASSERT(szTarget, "szTarget is NULL in CChatSocket::bSendData");
	ASSERT(szTag, "szTag is NULL in CChatSocket::bSendData");
	ASSERT(pbData, "pbData is NULL in CChatSocket::bSendData");

	if (!bCheckLogged())
		return FALSE;

	if (bPostProcess)
	{
		// Post-process the data stream to turn it into a string
		if (!bDataToString(pbData, (UINT) dwcb, &szData, TRUE /* bAfterColon */))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
	}
	else
	{
		// Data comes from the user and has already been post processed
		if (!(szData = new CHAR[dwcb+1]))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		CopyMemory((PVOID) szData, pbData, dwcb);
		szData[dwcb] = g_chEOS;
	}

	ASSERT(szData, "szData is NULL in CChatSocket::bSendData");

	if (SUCCEEDED(m_hrLastError = HrMakeSendData(&pb, &cb, (CHAR*) szTarget, (CHAR*) szRcpNicks, (CHAR*) szTag, (CHAR*) szData, nCmd)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);

	delete [] szData;

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bSendProtocolText
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bSendProtocolText(LPCTSTR szTextMessage)
{
	OutputDebugThreadIdString("CChatSocket::bSendProtocolText - Enter\n");

	switch (m_csState)
	{
		case csDisconnected:
		case csConnecting:
			m_hrLastError = CC_E_NOTCONNECTED;
			return FALSE;

		case csLogging:
		case csConnected:
		case csLogged:
			break;

		case csDisconnecting:
			m_hrLastError = CC_E_ALREADYDISCONNECTING;
			return FALSE;

		default:
			ASSERT(FALSE, "Unexpected Connection State in CChatSocket::bSendProtocolText");
			m_hrLastError = E_FAIL;
			return FALSE;
	}

	return SUCCEEDED(m_hrLastError = m_cconn.HrSend((PVOID) szTextMessage, (DWORD) lstrlen(szTextMessage)));
}


BOOL CChatSocket::bConstructDataMessageFromString(VARIANT *pvDataMessage, LPTSTR szDataMessage, BOOL *pbDataRaw)
{
	ASSERT(pvDataMessage, "pvDataMessage is NULL in CChatSocket::bConstructDataMessageFromString");
	ASSERT(szDataMessage, "szDataMessage is NULL in CChatSocket::bConstructDataMessageFromString");
	ASSERT(pbDataRaw, "pbDataRaw is NULL in CChatSocket::bConstructDataMessageFromString");

	UINT		cbData = 0L;
	PBYTE		pbData = (PBYTE) szDataMessage;
	LPTSTR		szTmp  = szDataMessage;
	VARIANT		v;
    SAFEARRAY	*psa;
    SAFEARRAY	*psaSimple = NULL, *psaComplex = NULL;
	LONG		lIndice;

	// Check if this comes from a protocol OCX sibling or not
	// The two first DWORDs should be g_dwMagicData1 = 0x19FE1944 and g_dwMagicData2 = 0x11DE1944
	// Check if the data is at least 2 DWORDS long
	while (*szTmp != g_chEOS && szTmp-szDataMessage < 2*sizeof(DWORD))
		szTmp++;

	if (*szTmp == g_chEOS)
		goto noheader;

	if (*((DWORD*) szDataMessage) != g_dwMagicData1 ||
		*((DWORD*) (szDataMessage+sizeof(DWORD))) != g_dwMagicData2)
		goto noheader;

	// This is a data message from a protocol ocx, with a header
	*pbDataRaw = FALSE;

	// First we need to pre-process the string
	if (!bStringToData(szDataMessage, pbData, &cbData))
	{
		ASSERT(FALSE, "bStringToData failed in CChatSocket::bConstructDataMessageFromString");
		m_hrLastError = E_FAIL;	// This should never happen
		return FALSE;
	}

	// Interpret the pre-processed byte stream
	ASSERT(pbData, "pbData is NULL in CChatSocket::bConstructDataMessageFromString");

	// Skip the 2 magic DWORDs
	pbData += 2*sizeof(DWORD);

	// Read the Stream Type byte
	BYTE byteStreamType;

	byteStreamType = *((BYTE*) pbData);
	pbData += sizeof(BYTE);

	if (g_byteSingleton == byteStreamType)
	{
		// Variant is not an array but a singleton
		// Read the VT...
		pvDataMessage->vt = *pbData;
		pbData++;
		// ...and the value
		switch (pvDataMessage->vt)
		{
		case VT_UI1:
			pvDataMessage->bVal = *pbData;
			break;
		case VT_I2:
			pvDataMessage->iVal = *((short*) pbData);
			break;
		case VT_I4:
			pvDataMessage->lVal = *((long*) pbData);
			break;
		case VT_R4:
			pvDataMessage->fltVal = *((float*) pbData);
			break;
		case VT_R8:
			pvDataMessage->dblVal = *((double*) pbData);
			break;
		case VT_CY:
			pvDataMessage->cyVal = *((CY*) pbData);
			break;
		case VT_DATE:
			pvDataMessage->date = *((DATE*) pbData);
			break;
		case VT_BSTR:
		{
			WORD wLen = *((WORD*) pbData);
			pbData += sizeof(WORD);
			if (wLen > 0)
			{
				if (!(pvDataMessage->bstrVal = A2BSTR((LPTSTR) pbData)))
				{
					pvDataMessage->vt = 0;
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}
			}
			else
				pvDataMessage->bstrVal = SysAllocString(L"");
			break;
		}
		case VT_ERROR:
			pvDataMessage->scode = *((SCODE*) pbData);
			break;
		case VT_BOOL:
			pvDataMessage->boolVal = *((VARIANT_BOOL*) pbData);
			break;
		default:
			m_hrLastError = E_FAIL;
			return FALSE;
		}
	}
	else
	{
		// We deal with a simple or complex array
		VARTYPE		vtCommon = VT_VARIANT;

		// Read the array boundaries
		WORD wLBound, wUBound;

		wLBound = *((WORD*) pbData);
		pbData += sizeof(WORD);

		wUBound = *((WORD*) pbData);
		pbData += sizeof(WORD);

		// Number of elements is wUBound-wLBound+1

		// Read the common Vartype if this is a simple array
		if (g_byteSimpleArray == byteStreamType)
		{
			vtCommon = (VARTYPE) *pbData;
			pbData += sizeof(BYTE);
		}

		if (!(psa = SafeArrayCreateVector(vtCommon, wLBound, wUBound-wLBound+1)))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}

		pvDataMessage->vt = vtCommon+VT_ARRAY;
		pvDataMessage->parray = psa;

		VariantInit(&v);

		for (lIndice = wLBound; lIndice <= wUBound; lIndice++)
			SafeArrayPutElement(psa, &lIndice, (PVOID) &v);

		for (lIndice = wLBound; lIndice <= wUBound; lIndice++)
		{
			if (g_byteSimpleArray == byteStreamType)
				// All elements have the same VT
				v.vt = vtCommon;
			else
			{
				// Read the element specific VT
				v.vt = (VARTYPE) *pbData;
				pbData++;
			}
			// Read the value
			switch (v.vt)
			{
			case VT_UI1:
				v.bVal = *pbData;
				break;
			case VT_I2:
				v.iVal = *((short*) pbData);
				break;
			case VT_I4:
				v.lVal = *((long*) pbData);
				break;
			case VT_R4:
				v.fltVal = *((float*) pbData);
				break;
			case VT_R8:
				v.dblVal = *((double*) pbData);
				break;
			case VT_CY:
				v.cyVal = *((CY*) pbData);
				break;
			case VT_DATE:
				v.date = *((DATE*) pbData);
				break;
			case VT_BSTR:
			{
				WORD wLen = *((WORD*) pbData);
				pbData += sizeof(WORD);
				if (wLen > 0)
				{
					if (!(v.bstrVal = A2BSTR((LPTSTR) pbData)))
					{
						VariantClear(pvDataMessage);
						m_hrLastError = E_OUTOFMEMORY;
						return FALSE;
					}
				}
				else
					v.bstrVal = SysAllocString(L"");
				pbData += wLen;
				break;
			}
			case VT_ERROR:
				v.scode = *((SCODE*) pbData);
				break;
			case VT_BOOL:
				v.boolVal = *((VARIANT_BOOL*) pbData);
				break;
			default:
				VariantClear(pvDataMessage);
				m_hrLastError = E_FAIL;
				return FALSE;
			}

			if (g_byteSimpleArray == byteStreamType)
			{
				if (VT_BSTR == v.vt)
					m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) v.bstrVal);
				else
					m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) pbData);
			}
			else
				m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) &v);

			pbData += byteSizeOfVariant(v.vt);

			if (FAILED(m_hrLastError))
			{
				VariantClear(pvDataMessage);
				return FALSE;
			}

			VariantClear(&v);
		}
	}

	// Don't release pbData, we use the same memory as szDataMessage
	return TRUE;

noheader:
	// This is a data message without the magic header
	*pbDataRaw = TRUE;

	if (!(psa = SafeArrayCreateVector(VT_VARIANT, 0L, 3)))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	pvDataMessage->vt = VT_VARIANT+VT_ARRAY;
	pvDataMessage->parray = psa;
	
	VariantInit(&v);

	for (lIndice = 0L; lIndice <= 2L; lIndice++)
		SafeArrayPutElement(psa, &lIndice, (PVOID) &v);
	
	// DataMessage[0] is the string
	// DataMessage[1] is the byte or array of bytes
	// DataMessage[2] is the VARIANT or array of VARIANTs

	if (!(v.bstrVal = A2BSTR(szDataMessage)))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto exit;
	}
	v.vt = VT_BSTR;

	// Store the string first
	lIndice = 0L;
	if (FAILED(m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) &v)))
		goto exit;

	// Free the string
	VariantClear(&v);

	// Get the actual data from the string
	if (!bStringToData(szDataMessage, pbData, &cbData))
	{
		ASSERT(FALSE, "bStringToData failed in CChatSocket::bConstructDataMessageFromString");
		m_hrLastError = E_FAIL;	// This should never happen
		goto exit;
	}

	// Interpret the pre-processed byte stream
	ASSERT(pbData, "pbData is NULL in CChatSocket::bConstructDataMessageFromString");

	if (1 == cbData)
	{
		// We got a byte singleton
		v.vt = VT_UI1;
		v.bVal = *pbData;
		lIndice = 1L;
		if (FAILED(m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) &v)))
			goto exit;
		lIndice = 2L;
		if (FAILED(m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) &v)))
			goto exit;
	}
	else
	{
		// This is a byte array case
		if (!(psaSimple  = SafeArrayCreateVector(VT_UI1, 0L, cbData)) ||
			!(psaComplex = SafeArrayCreateVector(VT_VARIANT, 0L, cbData)))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}

		v.vt = VT_UI1;
		for (lIndice = 0L; lIndice < (LONG) cbData; lIndice++)
		{
			v.bVal = *pbData;

			if (FAILED(m_hrLastError = SafeArrayPutElement(psaSimple, &lIndice, (PVOID) pbData)))
				goto exit;

			if (FAILED(m_hrLastError = SafeArrayPutElement(psaComplex, &lIndice, (PVOID) &v)))
				goto exit;

			pbData++;
		}

		v.vt = VT_UI1+VT_ARRAY;
		v.parray = psaSimple;
		lIndice = 1L;
		if (FAILED(m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) &v)))
			goto exit;

		// Free the array of bytes
		VariantClear(&v);

		v.vt = VT_VARIANT+VT_ARRAY;
		v.parray = psaComplex;
		lIndice = 2L;
		if (FAILED(m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) &v)))
			goto exit;

		// Free the array of VARIANTs
		VariantClear(&v);
	}

	return TRUE;

exit:
	VariantClear(pvDataMessage);
	return FALSE;
}


BOOL CChatSocket::bConstructDataStream(VARIANT *pvMessage, BYTE **prgbMsg, DWORD *pdwcb, BOOL bDataRaw, BOOL *pbPostProcess)
{
	PBYTE		pbMsg;
    LONG		i, iUBMsg, iLBMsg;
	WORD		cbLeft, cbLen;
	LPTSTR		szMessageTmp;
	SAFEARRAY	*psa;
	VARTYPE		vt;

	ASSERT(pvMessage, "pvMessage is NULL in CChatSocket::bConstructDataStream");
	ASSERT(prgbMsg, "prgbMsg is NULL in CChatSocket::bConstructDataStream");
	ASSERT(pdwcb, "pdwcb is NULL in CChatSocket::bConstructDataStream");
	ASSERT(pbPostProcess, "pbPostProcess is NULL in CChatSocket::bConstructDataStream");

	m_hrLastError = NOERROR; 
	vt = pvMessage->vt;

	// By default, string needs to be post processed
	*pbPostProcess = TRUE;

	USES_CONVERSION;

	// Allocate buffer
	if (!(*prgbMsg = new BYTE[g_lMaxMsgLength]))
	{
    	m_hrLastError = E_OUTOFMEMORY;
    	goto exit;
	}
	cbLeft = (WORD) g_lMaxMsgLength;

    pbMsg = *prgbMsg;

    // Fill the message header only if this is not a raw data message
	if (!bDataRaw)
	{
		// Start with the to magic keys
		*(DWORD*) pbMsg = (DWORD) g_dwMagicData1;
		pbMsg += sizeof(DWORD);

		*(DWORD*) pbMsg = (DWORD) g_dwMagicData2;
		pbMsg += sizeof(DWORD);

		cbLeft -= 2*sizeof(DWORD);
	}

	// Treat the case where the message is not an array
	if (0 == (vt & VT_ARRAY))
	{
		// For raw data, it can only be one byte or a string
		if (bDataRaw && VT_UI1 != vt && VT_BSTR != vt)
		{
			m_hrLastError = CC_E_BADMSGTYPE;
			goto exit;
		}

		// We don't put boundaries information in this singleton case

		if (!bDataRaw)
		{
			// Put the StreamType byte first
			*pbMsg = (BYTE) g_byteSingleton;
			pbMsg += sizeof(BYTE);

			// Prefix the Variant with its type
			*pbMsg = (BYTE) vt;
			pbMsg += sizeof(BYTE);

			cbLeft -= 2 * sizeof(BYTE);	// Stream Type + Variant Type
		}

		switch (vt)
		{
		    case VT_UI1 :
				*pbMsg = pvMessage->bVal;
				break;

		    case VT_I2 :
				*(short*) pbMsg = pvMessage->iVal;
				break;

		    case VT_I4 :
				*(long*) pbMsg = pvMessage->lVal;
				break;

		    case VT_R4 :
				*(float*) pbMsg = pvMessage->fltVal;
				break;

		    case VT_R8 :
				*(double*) pbMsg = pvMessage->dblVal;
				break;

			case VT_BOOL:
				*(VARIANT_BOOL*) pbMsg = pvMessage->boolVal;
				break;
			
			case VT_ERROR:
				*(SCODE*) pbMsg = pvMessage->scode;
				break;
			
			case VT_CY:
				*(CY*) pbMsg = pvMessage->cyVal;
				break;
			
			case VT_DATE:
				*(DATE*) pbMsg = pvMessage->date;
				break;

			case VT_BSTR:
			{
				// Data is a BSTR - we first convert it into ANSI
				ASSERT(pvMessage->bstrVal, "!pvMessage->bstrVal in CChatSocket::bConstructDataStream");

				if (!(szMessageTmp = W2T(pvMessage->bstrVal)))
				{
					m_hrLastError = E_OUTOFMEMORY;
					goto exit;
				}

				cbLen = lstrlen(szMessageTmp) + (bDataRaw ? 0 : 1);

				if (bDataRaw)
				{
					// String comes from the user, it is already post processed
					*pbPostProcess = FALSE;
					// Well, actually let's make sure the user post processed the string correctly
					PBYTE pbDst = new BYTE[cbLen+1];
					UINT  cbDst;
					BOOL  bRet = bStringToData(szMessageTmp, pbDst, &cbDst);
					delete [] pbDst;
					if (!bRet)
					{
						m_hrLastError = E_INVALIDARG;
						goto exit;
					}
				}
				else
				{
					// We prefix the string with its length, \0 included or not / bDataRaw
					*((WORD*) pbMsg) = cbLen;
					pbMsg += sizeof(WORD);
					cbLeft -= sizeof(WORD);
				}

				if (cbLen > cbLeft)
				{
					m_hrLastError = CC_E_TOOMUCHDATA;
					goto exit;
				}

				// We finally append the string itself
				CopyMemory(pbMsg, szMessageTmp, cbLen);
				pbMsg += cbLen;
				cbLeft -= cbLen;
				break;
			}

			default:
				// Unexpected data type
				m_hrLastError = CC_E_BADMSGTYPE;
				goto exit;
		}
		pbMsg += byteSizeOfVariant(vt);
		cbLeft -= byteSizeOfVariant(vt);
		// Time to return
		goto exit;
	}

	// Treat the cases where we have an array of elements that can be VT_UI1, VT_I2, ... VT_BSTR, VT_VARIANT

	// For raw data, we only can deal with arrays of bytes or arrays of variants that are bytes
	if (bDataRaw && 
		vt != (VT_VARIANT+VT_ARRAY+VT_BYREF) && 
		vt != (VT_VARIANT+VT_ARRAY) && 
		vt != (VT_UI1+VT_ARRAY))
	{
		m_hrLastError = CC_E_BADMSGTYPE;
		goto exit;
	}
	// Still need to make sure that each individual element is a VT_UI1!
	
	if (vt & VT_BYREF)
	{
		psa = *pvMessage->pparray;
		vt -= VT_BYREF;
	}
	else
		psa = pvMessage->parray;

	if (FAILED(m_hrLastError = SafeArrayGetLBound(psa, 1, &iLBMsg)))
	    goto exit;

	if (FAILED(m_hrLastError = SafeArrayGetUBound(psa, 1, &iUBMsg)))
	    goto exit;

	if (iUBMsg > 32767 || iLBMsg > 32767)
	{
	    m_hrLastError = CC_E_TOOMUCHDATA;
	    goto exit;
	}

	if (!bDataRaw)
	{
		// Put the Stream Type first
		if (vt == (VT_VARIANT+VT_ARRAY))
			*pbMsg = (BYTE) g_byteComplexArray;
		else
			*pbMsg = (BYTE) g_byteSimpleArray;
		pbMsg += sizeof(BYTE);

		// Append Array boundaries
		*(WORD*) pbMsg = (WORD) iLBMsg;
		pbMsg += sizeof(WORD);

		*(WORD*) pbMsg = (WORD) iUBMsg;
		pbMsg += sizeof(WORD);

		cbLeft -= sizeof(BYTE) + 2*sizeof(WORD); // Stream Type + 2 Boundaries

		if (vt != (VT_VARIANT+VT_ARRAY))
		{
			// Put the elements' vartype info first
			*pbMsg = (BYTE) vt & ~VT_ARRAY;
			pbMsg += sizeof(BYTE);
			cbLeft -= sizeof(BYTE);
		}
	}

	for (i = iLBMsg; i <= iUBMsg; i++)
	{
		switch (vt & ~VT_ARRAY)
		{
			case VT_UI1:
			case VT_I2:
			case VT_I4:
			case VT_R4 :
			case VT_R8 :
			case VT_BOOL:
			case VT_ERROR:
			case VT_CY:
			case VT_DATE:
			{
				cbLen = byteSizeOfVariant(vt & ~VT_ARRAY);
				if (cbLeft < cbLen)
				{
					m_hrLastError = CC_E_TOOMUCHDATA;
					goto exit;
				}

				if (FAILED(m_hrLastError = SafeArrayGetElement(psa, &i, pbMsg)))
					goto exit;

				pbMsg += cbLen;
				cbLeft -= cbLen;
				break;
			}
			case VT_BSTR:
			{
				BSTR bstrElement;

				// Prefix the Variant with its type
				*pbMsg = (BYTE) VT_BSTR;
				pbMsg += sizeof(BYTE);
				cbLeft -= sizeof(BYTE);

				if (FAILED(m_hrLastError = SafeArrayGetElement(psa, &i, &bstrElement)))
					goto exit;

				if (!bstrElement)
				{
					*(WORD*) pbMsg = 0L;		// For an empty string
					pbMsg += sizeof(WORD);
					cbLeft -= sizeof(WORD);
				}
				else
				{
					if (!(szMessageTmp = W2T(bstrElement)))
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}

					SysFreeString(bstrElement);
					// We prefix the string with its length, \0 included
					*(WORD*) pbMsg = cbLen = lstrlen(szMessageTmp)+1;
					pbMsg += sizeof(WORD);
					cbLeft -= sizeof(WORD);
					// We finally append the string itself
					if (cbLen > cbLeft)
					{
						m_hrLastError = CC_E_TOOMUCHDATA;
						goto exit;
					}
					CopyMemory(pbMsg, szMessageTmp, cbLen);
					pbMsg += cbLen;
					cbLeft -= cbLen;
				}
				break;
			}
			case VT_VARIANT:
			{
				VARIANT vElement;

				VariantInit(&vElement);

				if (FAILED(m_hrLastError = SafeArrayGetElement(psa, &i, &vElement)))
					goto exit;

				if (bDataRaw)
				{
					// Make sure the element is a byte
					if (vElement.vt != VT_UI1)
					{
						m_hrLastError = CC_E_BADMSGTYPE;
						goto exit;
					}
				}
				else
				{
					// Prefix the Variant with its type
					*pbMsg = (BYTE) vElement.vt;
					pbMsg += sizeof(BYTE);
					cbLeft -= sizeof(BYTE);
				}

				cbLen = byteSizeOfVariant(vElement.vt);
				if (cbLeft < cbLen)
				{
					m_hrLastError = CC_E_TOOMUCHDATA;
					goto exit;
				}

				switch (vElement.vt)
				{
					case VT_UI1 :
						*pbMsg = vElement.bVal;
						break;

					case VT_I2 :
						*(short*) pbMsg = vElement.iVal;
						break;

					case VT_I4 :
						*(long*) pbMsg = vElement.lVal;
						break;

					case VT_R4 :
						*(float*) pbMsg = vElement.fltVal;
						break;

					case VT_R8 :
						*(double*) pbMsg = vElement.dblVal;
						break;

					case VT_BOOL:
						*(VARIANT_BOOL*) pbMsg = vElement.boolVal;
						break;
					
					case VT_ERROR:
						*(SCODE*) pbMsg = vElement.scode;
						break;
					
					case VT_CY:
						*(CY*) pbMsg = vElement.cyVal;
						break;
					
					case VT_DATE:
						*(DATE*) pbMsg = vElement.date;
						break;

					case VT_BSTR:
					{
						if (!vElement.bstrVal)
						{
							*(WORD*) pbMsg = 0L;		// 0 length for an empty string
							pbMsg += sizeof(WORD);
							cbLeft -= sizeof(WORD);
						}
						else
						{
							if (!(szMessageTmp = W2T(vElement.bstrVal)))
							{
								m_hrLastError = E_OUTOFMEMORY;
								goto exit;
							}

							SysFreeString(vElement.bstrVal);
							// We prefix the string with its length, \0 included
							*(WORD*) pbMsg = cbLen = lstrlen(szMessageTmp)+1;
							pbMsg += sizeof(WORD);
							cbLeft -= sizeof(WORD);
							// We finally append the string itself
							if (cbLen > cbLeft)
							{
								m_hrLastError = CC_E_TOOMUCHDATA;
								goto exit;
							}
							CopyMemory(pbMsg, szMessageTmp, cbLen);
						}
						break;
					}
					default:
					{
						// Unexpected data type
						m_hrLastError = CC_E_BADMSGTYPE;
						goto exit;
					}
					break;
				}
				pbMsg += cbLen;
				cbLeft -= cbLen;
				break;
			}
			default:
			{
				// Unexpected data type
				m_hrLastError = CC_E_BADMSGTYPE;
				goto exit;
			}
		}
	}

exit:
	if (FAILED(m_hrLastError))
	{
		if (*prgbMsg)
			delete [] *prgbMsg;
		*prgbMsg = NULL;
		*pdwcb = 0L;

		return FALSE;
	}
	else
	{
		*pdwcb = (DWORD) (g_lMaxMsgLength - cbLeft);
		return TRUE;
	}
}


//=--------------------------------------------------------------------------=
// CChatSocket::bConstructRecipientList
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bConstructRecipientList(VARIANT *pvRecipientNicknames, LPTSTR *pszRcpNicks, LONG *pcRcp)
{
    LONG		i, iLBDest, iUBDest, cRcp;
	LPTSTR		szRcpNickTmp, szRcpNicks = NULL;
    BSTR		bstrRecipientNickname = NULL;
	BOOL		bFreeRcpNick;
	SAFEARRAY	*psa;

	ASSERT(pszRcpNicks, "pszRcpNicks is NULL in CChatSocket::bConstructRecipientList");
	ASSERT(pcRcp, "plRcp is NULL in CChatSocket::bConstructRecipientList");

	USES_CONVERSION;

    // Make sure the destination list is valid.
	// It must be either a VT_BSTR, a VT_NULL, a VT_EMPTY, a VT_ERROR, a single dimensional VT_ARRAY of VT_BSTRs
	// or a single dimensional VT_ARRAY of VT_VARIANTS that are BSTRs
    if (!pvRecipientNicknames ||
		!(
		  (pvRecipientNicknames->vt == VT_EMPTY) || 
		  (pvRecipientNicknames->vt == VT_NULL)  || 
		  (pvRecipientNicknames->vt == VT_ERROR) || 
		  (pvRecipientNicknames->vt == VT_BSTR)	 || 
	      (pvRecipientNicknames->vt == (VT_BSTR | VT_ARRAY) && SafeArrayGetDim(pvRecipientNicknames->parray) == 1) ||
	      (pvRecipientNicknames->vt == (VT_VARIANT | VT_ARRAY) && SafeArrayGetDim(pvRecipientNicknames->parray) == 1) ||
	      (pvRecipientNicknames->vt == (VT_VARIANT | VT_ARRAY | VT_BYREF) && SafeArrayGetDim(*pvRecipientNicknames->pparray) == 1)
		 )
	   )
	{
		m_hrLastError = CC_E_INVALIDRECIPIENTLIST;
		goto exit;
    }
	
	// get the array boundaries
	switch (pvRecipientNicknames->vt)
	{
	case VT_EMPTY:
	case VT_NULL:
	case VT_ERROR:
		cRcp = 0L;	// target is whole channel
		break;

	case VT_BSTR:
		// if the BSTRs given is empty, then we consider that it's a normal message
		if (!pvRecipientNicknames->bstrVal || g_chEOS == *pvRecipientNicknames->bstrVal)
			cRcp = 0L;	// empty nickname - target is whole channel
		else
			cRcp = 1L;	// this is a uni-casting
		break;

	case VT_ARRAY+VT_BSTR:
	case VT_ARRAY+VT_VARIANT:
		// this is a multi-casting

		psa = pvRecipientNicknames->parray;
		// get the recipients array boundaries
		if (FAILED(m_hrLastError = SafeArrayGetLBound(psa, 1, &iLBDest)) ||
			FAILED(m_hrLastError = SafeArrayGetUBound(psa, 1, &iUBDest)))
			goto exit;

		cRcp = iUBDest - iLBDest + 1;
		break;

	case VT_BYREF+VT_ARRAY+VT_VARIANT:
		// this is a multi-casting

		psa = *pvRecipientNicknames->pparray;
		// get the recipients array boundaries
		if (FAILED(m_hrLastError = SafeArrayGetLBound(psa, 1, &iLBDest)) ||
			FAILED(m_hrLastError = SafeArrayGetUBound(psa, 1, &iUBDest)))
			goto exit;

		cRcp = iUBDest - iLBDest + 1;
		break;

	default:
		ASSERT(FALSE, "Unexpected variant type for RecipientNicknames in CChatSocket::bConstructRecipientList");
		m_hrLastError = CC_E_INVALIDRECIPIENTLIST;
		goto exit;
	}

	// Construct the recipient nicknames string
	if (pvRecipientNicknames->vt & VT_ARRAY)
	{
		LONG	cbRcpNicks, cbNick;
		VARIANT	vRcpNick;

		// Prepare the suite of nicknames
		ASSERT(pvRecipientNicknames->vt == (VT_ARRAY+VT_BSTR) || 
			   pvRecipientNicknames->vt == (VT_ARRAY+VT_VARIANT) ||
			   pvRecipientNicknames->vt == (VT_ARRAY+VT_VARIANT|VT_BYREF), "Bad Variant type in CChatSocket::bConstructRecipientList");

		szRcpNicks = new TCHAR[g_lMaxMsgLength];
		if (!szRcpNicks)
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}

		szRcpNicks[0] = g_chEOS;
		cbRcpNicks = 0L;
		cRcp = 0L;

		for (i = iLBDest; i < iUBDest; i++)
		{
			if (pvRecipientNicknames->vt == (VT_ARRAY+VT_BSTR))
			{
				if (FAILED(m_hrLastError = SafeArrayGetElement(psa, &i, &bstrRecipientNickname)))
					goto exit;
			}
			else
			{
				// Extract the bstrRecipientNickname from the variant element
				if (FAILED(m_hrLastError = SafeArrayGetElement(psa, &i, &vRcpNick)))
					goto exit;

				switch (vRcpNick.vt)
				{
					case VT_BSTR: 
						bstrRecipientNickname = vRcpNick.bstrVal;
						break;
					case VT_EMPTY:
						ASSERT(!bstrRecipientNickname, "bstrRecipientNickname is NOT NULL in CChatSocket::bConstructRecipientList");
						break;
					default:
						m_hrLastError = CC_E_INVALIDRECIPIENTLIST;
						goto exit;
				}
			}

			if (bstrRecipientNickname && *bstrRecipientNickname)
			{
				if (bIsIrcXSocket() && bExtendedWideNickname(bstrRecipientNickname))
				{
					// The nickname is extended on IRCX - Need to convert it to UTF8
					bConvertWideStringToUTF8(bstrRecipientNickname, 0, &szRcpNickTmp, NULL, TRUE /*bNickname*/);
					bFreeRcpNick = TRUE;
				}
				else
				{
					szRcpNickTmp = W2T(bstrRecipientNickname);
					bFreeRcpNick = FALSE;
				}

				if (!szRcpNickTmp)
				{
					m_hrLastError = E_OUTOFMEMORY;
					goto exit;
				}

				// At this point we decide not to check if the nickname is part fo the channel or not
				// The server will send us an asynchronous error if necessary
				cRcp++;

				// Add this user to our list
				cbNick = lstrlen(szRcpNickTmp);
				if (cbRcpNicks + cbNick + 1 >= g_lMaxMsgLength)
				{
					m_hrLastError = CC_E_TOOMUCHDATA;
					goto exit;
				}
				if (g_chEOS != szRcpNicks[0])
				{
					szRcpNicks[cbRcpNicks] = g_chComma;
					cbRcpNicks++;
				}
				lstrcpyn(szRcpNicks+cbRcpNicks, szRcpNickTmp, cbNick+1);
				cbRcpNicks += cbNick;
				if (bFreeRcpNick)
					delete [] szRcpNickTmp;
			}
			if (bstrRecipientNickname)
			{
				SysFreeString(bstrRecipientNickname);
				bstrRecipientNickname = NULL;
			}
		}

		// Make sure there is at least one recipient
		if (0L == cRcp)
		{
			m_hrLastError = CC_E_INVALIDRECIPIENTLIST;
			goto exit;
		}
	}
	else
		if (pvRecipientNicknames->vt == VT_BSTR && cRcp > 0)
		{
			if (bIsIrcXSocket() && bExtendedWideNickname(pvRecipientNicknames->bstrVal))
			{
				// The nickname is extended on IRCX - Need to convert it to UTF8
				if (!bConvertWideStringToUTF8(pvRecipientNicknames->bstrVal, 0, &szRcpNicks, NULL, TRUE /*bNickname*/))
				{
					m_hrLastError = E_OUTOFMEMORY;
					goto exit;
				}
			}
			else
			{
				szRcpNickTmp = W2T(pvRecipientNicknames->bstrVal);
				if (!szRcpNickTmp ||
					!bCopyStr(&szRcpNicks, szRcpNickTmp))
				{
					m_hrLastError = E_OUTOFMEMORY;
					goto exit;
				}
			}
		}

	*pszRcpNicks = szRcpNicks;
	*pcRcp = cRcp;
	return TRUE;

exit:
	if (szRcpNicks)
		delete [] szRcpNicks;

	if (bstrRecipientNickname)
		SysFreeString(bstrRecipientNickname);

	*pszRcpNicks = NULL;
	*pcRcp = 0L;
	return FALSE;
}


BOOL CChatSocket::bConstructRecipientsVariant(LPCTSTR szRcpNicks, VARIANT *pvRcpNicks, LONG *pcRcp)
{
	LPTSTR		szRcpNick, sz;
	LPWSTR		wszRcpNick;
	VARIANT		vRcp;
    SAFEARRAY	*psa = NULL;
	LONG		lIndice;
	BOOL		bEnd;

	ASSERT(szRcpNicks, "szRcpNicks is NULL in CChatSocket::bConstructRecipientsVariant");
	ASSERT(pvRcpNicks, "pvRcpNicks is NULL in CChatSocket::bConstructRecipientsVariant");

	VariantInit(pvRcpNicks);

	psa = SafeArrayCreateVector(VT_VARIANT, 0L, 64L);	// 64 should be quite enough!
	if (!psa)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	pvRcpNicks->vt = VT_VARIANT+VT_ARRAY;
	pvRcpNicks->parray = psa;

	VariantInit(&vRcp);

	// Initialize the array with empty variants
	for (lIndice = 0L; lIndice < 64L; lIndice++)
		SafeArrayPutElement(psa, &lIndice, (PVOID) &vRcp);

	szRcpNick = sz = (LPTSTR) szRcpNicks;
	lIndice = 0L;

	do
	{
		SkipTillChar(&sz, g_chComma);
		bEnd = *sz == g_chEOS;
		*sz = g_chEOS;
		
		// Might want to check if the member is in this channel!

		// If the nickname is in UTF8, we need to convert it
		if (g_chExtNckPfx == szRcpNick[0])
		{
			if (!bConvertUTF8StringToWide(szRcpNick, 0, &wszRcpNick, NULL, TRUE /*bNickname*/))
			{
				m_hrLastError = E_OUTOFMEMORY;
				goto exit;
			}
			vRcp.bstrVal = SysAllocString(wszRcpNick);
			delete [] wszRcpNick;
		}
		else
			vRcp.bstrVal = A2BSTR(szRcpNick);

		if (!vRcp.bstrVal)
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
		vRcp.vt = VT_BSTR;

		// Rebuild what we destroyed
		if (!bEnd)
		{
			*sz = g_chComma;
			szRcpNick = sz = sz+1;
		}

		if (FAILED(m_hrLastError = SafeArrayPutElement(psa, &lIndice, (PVOID) &vRcp)))
			goto exit;

		lIndice++;

		VariantClear(&vRcp);
	}
	while (!bEnd && lIndice < 64L);

	// Let's resize the safe array appropriately if necessary
	SAFEARRAYBOUND sabound;
	sabound.lLbound = 0L;
	sabound.cElements = lIndice;
	if (FAILED(m_hrLastError = SafeArrayRedim(psa, &sabound)))
		goto exit;

	if (pcRcp)
		*pcRcp = lIndice;

	return TRUE;

exit:
	if (pcRcp)
		*pcRcp = 0L;

	VariantClear(pvRcpNicks);

	return FALSE;
}


//=--------------------------------------------------------------------------=
// CChatSocket::bChangeNickname
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bChangeNickname(LPCWSTR wszNewNickname)
{
	BYTE	*pb;
	INT		cb;
	LPTSTR	szNewNickname;
	BOOL	bFreeNickname = FALSE;

	OutputDebugThreadIdString("CChatSocket::bChangeNickname - Enter\n");

	if (!bCheckLogged())
		return FALSE;

	if (!wszNewNickname || L'\0' == *wszNewNickname)
	{
		m_hrLastError = CC_E_BADNICKNAME;
		return FALSE;
	}

	if (bIsIrcXSocket() && bExtendedWideNickname(wszNewNickname))
	{
		// The new nickname is extended on IRCX - Need to convert it to UTF8
		bConvertWideStringToUTF8(wszNewNickname, 0, &szNewNickname, NULL, TRUE /*bNickname*/);
		bFreeNickname = TRUE;
	}
	else
	{
		USES_CONVERSION;
		szNewNickname = W2T(wszNewNickname);
	}

	if (!szNewNickname)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (SUCCEEDED(m_hrLastError = HrMakeNickMsg(&pb, &cb, (CHAR*) szNewNickname)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);

	if (bFreeNickname)
		delete [] szNewNickname;

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bGetSyncUserHost
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bGetSyncUserHost(PPRUSER pPrUser)
{
	OutputDebugThreadIdString("CChatSocket::bGetSyncUserHost - Enter\n");

	ASSERT(pPrUser, "pPrUser is NULL in CChatSocket::bGetSyncUserHost");
	ASSERT(pPrUser->szNickname, "pPrUser->szNickname is NULL in CChatSocket::bGetSyncUserHost");

	DWORD	dwPropertyAccessTimeOut = m_dwPropertyAccessTimeOut;
	PRQUERY	prQuery;
	BOOL	bRet;
	BYTE	*pb;
	INT		cb;

	prQuery.at = atUser;
	prQuery.bSyncAccess = TRUE;
	prQuery.uHeadReplyCode = RPL_USERHOST;
	prQuery.szChannelName = NULL;
	prQuery.pvData = (PVOID) pPrUser;

	// Send the request to the server
	if (SUCCEEDED(m_hrLastError = HrMakeUserHostMsg(&pb, &cb, (CHAR*) pPrUser->szNickname)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
	if (FAILED(m_hrLastError))
		return FALSE;

	// We cash the timeout value and overwrite it for this internal synchronous call
	m_dwPropertyAccessTimeOut = (DWORD) g_lDefaultPropertyAccessTimeOut;

	// Wait for the property to come!
	bRet = bWaitForProperty(atUser, (SHORT) pnUserModes, &prQuery);

	// Restore cashed value
	m_dwPropertyAccessTimeOut = dwPropertyAccessTimeOut;

	return bRet;
}

//=--------------------------------------------------------------------------=
// CChatSocket::bIsUserIgnored
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bIsUserIgnored(LPCTSTR szNickname, LPCTSTR szUserName, LPCTSTR szIPAddress, BOOL bCheckIfSysOpOrAdmin, BOOL *pbIgnored)
{
	LONG			lMaskCount;
	PPRUSERMATCH	pPrUserMatch;

	// OutputDebugThreadIdString("CChatSocket::bIsUserIgnored - Enter\n");

	ASSERT(pbIgnored, "pbIgnored is NULL in CChatSocket::bIsUserIgnored");

	*pbIgnored = FALSE;	// By default

	// If no cell in our list, return right away
	if (0L == (lMaskCount = m_listMask.DwGetCellCount()))
		return TRUE;

	// Need to go through the mask list and see if there is a match
	for (LONG lPosition = 1L; lPosition <= lMaskCount; lPosition++)
	{
		if (m_listMask.bGetCell(lPosition, (PVOID*) &pPrUserMatch))
		{
			// Got a mask cell - let's see if this one filters the incoming message
			if (bIsMatch(pPrUserMatch, szNickname, szUserName, szIPAddress))
			{
				// This user should be ignored
				if (bCheckIfSysOpOrAdmin)
				{
					// Check if he/she's a SysOp or Admin before returning
					PRUSER prUser;

					ZeroMemory(&prUser, sizeof(PRUSER));

					if (!bCopyStr(&(prUser.szNickname), szNickname))
					{
						m_hrLastError = E_OUTOFMEMORY;
						return FALSE;
					}

					if (bGetSyncUserHost(&prUser))
						// Got the user modes - Can't ignore SysOps or Admins
						*pbIgnored = !(prUser.lModes & umSysop) && !(prUser.lModes & umAdmin);
					else
						*pbIgnored = TRUE;
				}
				else
					*pbIgnored = TRUE;
				return TRUE;
			}
		}
	}
	return TRUE;
}


//=--------------------------------------------------------------------------=
// CChatSocket::bIgnoreUsers
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bIgnoreUsers(CChatItems *pCItems, BOOL bSet)
{
	LPTSTR szIdentMask = NULL;

	OutputDebugThreadIdString("CChatSocket::bIgnoreUsers - Enter\n");

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bIgnoreUsers");

	if (!bCheckLogged())
		return FALSE;

	// Get the identmask string from the chatitems object   <nickname>!<username>@<ipaddress>
	if (!bGetIdentMaskFromChatItems(pCItems, &szIdentMask))
		return FALSE;

	BOOL bRet = bIgnoreUsers(szIdentMask, bSet);

	delete [] szIdentMask;

	return bRet;
}


BOOL CChatSocket::bIgnoreUsers(LPTSTR szIdentMask, BOOL bSet)
{
	PRUSERMATCH		prUserMatch;
	PPRUSERMATCH	pPrUserMatch;

	OutputDebugThreadIdString("CChatSocket::bIgnoreUsers - Enter\n");

	ASSERT(szIdentMask, "szIdentMask is NULL in CChatSocket::bIgnoreUsers");

	// Check if there is already such a mask in our list
	ZeroMemory(&prUserMatch, sizeof(PRUSERMATCH));
	prUserMatch.szTheMask = szIdentMask;

	BOOL bExists = m_listMask.bFindCell((PVOID) &prUserMatch, (PVOID*) &pPrUserMatch);

	if (bSet)
	{
		// Want to create a new mask entry
		if (bExists)
			return TRUE;	// Don't need to do anything, the mask already exists in the list
		else
		{
			// Need to add the mask to the list
			if (!(pPrUserMatch = new PRUSERMATCH))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}

			LPTSTR szIdent = NULL;
			bCopyStr(&szIdent, szIdentMask);

			bGetUserMatchFromMask(szIdent, pPrUserMatch);
			if (!m_listMask.bAddFrontCell((PVOID) pPrUserMatch))
			{
				delete szIdent;
				delete pPrUserMatch;
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			return TRUE;
		}
	}
	else
	{
		if (bExists)
		{
			// Remove the entry from our mask list
			BOOL bRet = m_listMask.bRemoveCell((PVOID) pPrUserMatch);
			ASSERT(bRet, "m_listMask.bRemoveCell failed in CChatSocket::bIgnoreUsers");
			return TRUE;
		}
		else
			// Couldn't find an entry to remove!
			return TRUE;	// Let's not return an error and pretend everything's fine
	}
}


BOOL CChatSocket::bBuildIgnoredUsersArray(void)
{
	VARIANT			vItem;
	LONG			lPosition, lEntries;
	PPRUSERMATCH	pPrUserMatch;
	CChatItems		*pCItems = NULL;

	// Deallocate the old array
	if (m_prServer.psaIgnored)
	{
		SafeArrayDestroy(m_prServer.psaIgnored);
		m_prServer.psaIgnored = NULL;
	}

	if (0L == (lEntries = m_listMask.DwGetCellCount()))
		return TRUE;

	// Allocate the array
	if (!(m_prServer.psaIgnored = SafeArrayCreateVector(VT_VARIANT, 0L, lEntries)))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	// Initialize the array with empty variants
	VariantInit(&vItem);
	for (lPosition = 0L; lPosition < lEntries; lPosition++)
		SafeArrayPutElement(m_prServer.psaIgnored, &lPosition, (PVOID) &vItem);

	for (lPosition = 0L; lPosition < lEntries; lPosition++)
	{
		if (m_listMask.bGetCell(lPosition+1L, (PVOID*) &pPrUserMatch))
		{			
			ASSERT(pPrUserMatch, "pPrUserMatch is NULL in CChatSocket::bBuildIgnoredUsersArray");
			if (!bGetChatItemsFromIdentMask(&pCItems, pPrUserMatch->szTheMask, atUser))
				return FALSE;

			vItem.vt = VT_DISPATCH;
			vItem.pdispVal = pCItems->GetDispatch();

			ASSERT(vItem.pdispVal, "vItem.pdispVal is NULL in CChatSocket::bBuildIgnoredUsersArray");

			m_hrLastError = SafeArrayPutElement(m_prServer.psaIgnored, &lPosition, (PVOID) &vItem);
			if (FAILED(m_hrLastError))
			{
				ReleaseChatItems(pCItems);
				(IDispatch*)(vItem.pdispVal)->Release();
				VariantClear(&vItem);
				return FALSE;
			}
			(IDispatch*)(vItem.pdispVal)->Release();
			VariantClear(&vItem);
		}
	}
	return TRUE;
}


//=--------------------------------------------------------------------------=
// CChatSocket::bBanUsers
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bBanUsers(CChatItems *pCItems, BOOL bSet, LPCTSTR szReason, LONG lDuration)
{
	LPTSTR	szIdentMask = NULL;
	BYTE	*pb;
	INT		cb;

	OutputDebugThreadIdString("CChatSocket::bBanUsers - Enter\n");

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bBanUsers");

	if (!bCheckLogged())
		return FALSE;

	// Get the identmask string from the chatitems object   <nickname>!<username>@<ipaddress>
	if (!bGetIdentMaskFromChatItems(pCItems, &szIdentMask))
		return FALSE;

	ASSERT(szIdentMask, "szIdentMask is NULL in CChatSocket::bBanUsers");

	if (bIsIrcXSocket())
	{
		// On an IRCX server, we use the ACCESS command
		if (SUCCEEDED(m_hrLastError = HrMakeAccessMsg(&pb, &cb, bSet, szIdentMask, (CHAR*) szReason, lDuration)))
			m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
	}
	else
	{
		// On an IRC server, we use the KLINE command
		if (SUCCEEDED(m_hrLastError = HrMakeKLineMsg(&pb, &cb, bSet, szIdentMask, (CHAR*) szReason)))
			m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
	}

	delete [] szIdentMask;

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bKillTarget
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bKillTarget(LPCWSTR wszTarget, LPCTSTR szReason, BOOL bChannel)
{
	BYTE	*pb;
	INT		cb;
	LPTSTR	szTarget;
	BOOL	bFreeTarget = FALSE;

	OutputDebugThreadIdString("CChatSocket::bKillTarget - Enter\n");

	ASSERT(wszTarget, "wszTarget is NULL in CChatSocket::bKillTarget");
	// szReason can be NULL

	// wszTarget can be a nickname or channel name

	if (!bCheckLogged())
		return FALSE;

	USES_CONVERSION;

	if (bChannel)
	{
		if (!bIsIrcXSocket())
		{
			m_hrLastError = CC_E_NOTIRCX;
			return FALSE;
		}
		// Need to convert the channel name to UTF8 if it's extended
		if (bExtendedWideChannelName(wszTarget))
			if (L'%' == *wszTarget)
			{
				// The channel name is extended on IRCX - Need to convert it to UTF8
				bConvertWideStringToUTF8(wszTarget, 0, &szTarget, NULL, FALSE /*bNickname*/, TRUE /*bChannelName*/);
				bFreeTarget = TRUE;
			}
			else
				// Might have to convert from ShiftJis to Jis, etc...
				bConvertString(FALSE, m_byteCharSet, W2T(wszTarget), 0, &szTarget, NULL, &bFreeTarget);
	}
	else
	{
		// This is a nickname
		if (bIsIrcXSocket() && bExtendedWideNickname(wszTarget))
		{
			// The nickname is extended on IRCX - Need to convert it to UTF8
			bConvertWideStringToUTF8(wszTarget, 0, &szTarget, NULL, TRUE /*bNickname*/);
			bFreeTarget = TRUE;
		}
	}

	if (!bFreeTarget)
	{
		USES_CONVERSION;
		szTarget = W2T(wszTarget);
	}

	if (!szTarget)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (SUCCEEDED(m_hrLastError = HrMakeKillMsg(&pb, &cb, (CHAR*) szTarget, (CHAR*) szReason)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);

	if (bFreeTarget)
		delete [] szTarget;

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bSetAway
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bSetAway(LPCTSTR szAway)
{
	BYTE	*pb;
	INT		cb;

	OutputDebugThreadIdString("CChatSocket::bSetAway - Enter\n");

	if (!bCheckLogged())
		return FALSE;

	if (SUCCEEDED(m_hrLastError = HrMakeAwayMsg(&pb, &cb, (CHAR*) szAway)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);

	return SUCCEEDED(m_hrLastError);
}


//=--------------------------------------------------------------------------=
// CChatSocket::bSendInvitation
//=--------------------------------------------------------------------------=
BOOL CChatSocket::bSendInvitation(LPCWSTR wszNickname, LPCTSTR szChannelName)
{
	BYTE	*pb;
	INT		cb;
	LPTSTR	szNickname;
	BOOL	bFreeNickname = FALSE;

	OutputDebugThreadIdString("CChatSocket::bSendInvitation - Enter\n");

	ASSERT(wszNickname, "wszNickname is NULL in CChatSocket::bSendInvitation");
	ASSERT(szChannelName, "szChannelName is NULL in CChatSocket::bSendInvitation");

	if (!bCheckLogged())
		return FALSE;

	if (bIsIrcXSocket() && bExtendedWideNickname(wszNickname))
	{
		// The nickname is extended on IRCX - Need to convert it to UTF8
		bConvertWideStringToUTF8(wszNickname, 0, &szNickname, NULL, TRUE /*bNickname*/);
		bFreeNickname = TRUE;
	}
	else
	{
		USES_CONVERSION;
		szNickname = W2T(wszNickname);
	}

	if (!szNickname)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (SUCCEEDED(m_hrLastError = HrMakeInviteMsg(&pb, &cb, (CHAR*) szNickname, (CHAR*) szChannelName)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);

	if (bFreeNickname)
		delete [] szNickname;

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatSocket::bQueryChannelProperty(BOOL bSyncAccess, LPCWSTR wszChannelPropertyName, LPCTSTR szChannelName, CChatItems **ppCItems)
{
	PPRQUERY			pPrQuery = NULL;
	PPRCHANNEL			pPrChannel = NULL;
	BYTE				*pb;
	INT					cb;
	LONG				lOldProgressCount, *plProgressCount = NULL;

	OutputDebugThreadIdString("CChatSocket::bQueryChannelProperty - Enter\n");

	ASSERT(!bSyncAccess || ppCItems, "bSyncAccess && !ppCItems in CChatSocket::bQueryChannelProperty");
	ASSERT(szChannelName, "szChannelName is NULL in CChatSocket::bQueryChannelProperty");

	if (ppCItems)
		*ppCItems = NULL;

	if (!wszChannelPropertyName)
	{
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Make sure we are logged on to a server
	if (!bCheckLogged())
		return FALSE;

	// Make sure that the property name is known
	enumChannelPropertyName cpnProp = (enumChannelPropertyName) NGetPropertyIndex((SHORT) atChannel, wszChannelPropertyName);

	if (cpnProp < 0)
	{
		// this property is not a channel prop
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Make sure the channel name starts with a valid prefix
	if (g_chExtChnPfx != szChannelName[0] &&
		g_chGblChnPfx != szChannelName[0] &&
		g_chLclChnPfx != szChannelName[0])
	{
		m_hrLastError = CC_E_CHANNELBADNAME;
		return FALSE;
	}

	if (bSyncAccess)
	{
		// Create a CChatItems object via a virtual function that in fact creates a new CItemsObject object
		if (!bCreateChatItems(ppCItems))
			return FALSE;

		ASSERT(*ppCItems, "*ppCItems is NULL in CChatSocket::bQueryChannelProperty");
	}

	pPrQuery = new PRQUERY;
	pPrChannel = new PRCHANNEL;
	if (!pPrQuery || !pPrChannel)
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}

	pPrQuery->at = atChannel;
	pPrQuery->bSyncAccess = bSyncAccess;
	pPrQuery->uHeadReplyCode = 0;
	pPrQuery->pvData = (PVOID) pPrChannel;
	pPrQuery->szChannelName = NULL;
	if (!bCopyStr(&(pPrQuery->szChannelName), szChannelName))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}

	ZeroMemory(pPrChannel, sizeof(PRCHANNEL));

	if (!bCopyStr(&(pPrChannel->szName), szChannelName) ||
		!(pPrChannel->psaBannedList = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxBannedList)))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	pPrChannel->lBannedListCount= 0L;
	pPrChannel->lCloneListCount = 0L;
	pPrChannel->lMemberCount	= -1L;
	pPrChannel->lMaxMemberCount	= -1L;
	pPrChannel->lModes			= cmNone;
	pPrChannel->lLag			= -1L;
	pPrChannel->bFree			= TRUE;
	// lCreationTime, lOID and pChannel are NULL, bMicOnly is FALSE

	if (!m_listQuery.bInsertCell((LONG) m_listQuery.DwGetCellCount()+1L, (PVOID) pPrQuery))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}

	// Send the request to the server
	switch (cpnProp)
	{
		case cpnAccount:
		case cpnClientData:
		case cpnClientGuid:
		case cpnCreationTime:
		case cpnHostKey:
		case cpnLag:
		case cpnLanguage:
		case cpnObjectId:
		case cpnOnJoin:
		case cpnOnPart:
		case cpnOwnerKey:
		case cpnRating:
		case cpnServicePath:
		case cpnSubject:
			if (!bIsIrcXSocket())
			{
				m_hrLastError = CC_E_NOTIRCX;
				break;
			}
			pPrQuery->uHeadReplyCode = RPL_PROPLIST;
			if (SUCCEEDED(m_hrLastError = HrMakeGetPropMsg(&pb, &cb, (CHAR*) szChannelName, (CHAR*) g_rgszIrcxChannelPropertyNames[cpnProp])))
				m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
			if (SUCCEEDED(m_hrLastError))
			{
				lOldProgressCount = m_lPropInProgress;
				plProgressCount = &m_lPropInProgress;
				m_lPropInProgress++;
			}
			break;

		case cpnName:
			// This is a way to see if a channel exists or not!
		case cpnTopic:
		case cpnMemberCount:
			pPrQuery->uHeadReplyCode = RPL_LISTSTART;
			// we force a LIST instead of a LISTX
			if (SUCCEEDED(m_hrLastError = HrMakeListMsg(&pb, &cb, (CHAR*) szChannelName, FALSE)))
				m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
			if (SUCCEEDED(m_hrLastError))
			{
				lOldProgressCount = m_lListInProgress;
				plProgressCount = &m_lListInProgress;
				m_lListInProgress++;
			}
			break;

		case cpnBannedList:
			pPrQuery->uHeadReplyCode = RPL_BANLIST;
			if (SUCCEEDED(m_hrLastError = HrMakeGetBannedListMsg(&pb, &cb, (CHAR*) szChannelName)))
				m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
			if (SUCCEEDED(m_hrLastError))
			{
				lOldProgressCount = m_lBannedListInProgress;
				plProgressCount = &m_lBannedListInProgress;
				m_lBannedListInProgress++;
			}
			break;

		case cpnKeyword:
		case cpnMaxMemberCount:
		case cpnModes:
			pPrQuery->uHeadReplyCode = RPL_CHANNELMODEIS;
			if (SUCCEEDED(m_hrLastError = HrMakeGetModeMsg(&pb, &cb, (CHAR*) szChannelName)))
				m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
			if (SUCCEEDED(m_hrLastError))
			{
				lOldProgressCount = m_lModeIsInProgress;
				plProgressCount = &m_lModeIsInProgress;
				m_lModeIsInProgress++;
			}
			break;

		default:
		// cpnChannelAgeMax:
		// cpnChannelAgeMin:
		// cpnCloneList:
		// cpnLanguageOp:
		// cpnListCount:
		// cpnMemberCountMax:
		// cpnMemberCountMin:
		// cpnNameOp:
		// cpnRegistered:
		// cpnSubjectOp:
		// cpnTopicAgeMax:
		// cpnTopicAgeMin:
		// cpnTopicOp:
			m_hrLastError = CC_E_PROPNOTREADABLE;
			break;
	}
		
	if (FAILED(m_hrLastError))
		goto error;

	if (bSyncAccess)
	{
		// wait for the property to come!
		if (!bWaitForProperty(atChannel, (SHORT) cpnProp, pPrQuery))
			goto error;

		if (!bFillChatItemsFromChannel(*ppCItems, pPrChannel))
			goto error;

		BOOL bRet = m_listQuery.bRemoveCell((PVOID) pPrQuery);
		ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatSocket::bQueryChannelProperty");
	}

	return TRUE;

error:
	// Remove query cell from list
	if (pPrQuery)
		if (m_hrLastError == CC_E_TIMEOUT)
		{
			// We know wait for the answer asynchronously
			ASSERT(pPrQuery->bSyncAccess, "pPrQuery->bSyncAccess is FALSE in CChatSocket::bQueryChannelProperty");
			pPrQuery->bSyncAccess = FALSE;
		}
		else
		{
			if (pPrQuery->uHeadReplyCode != RPL_LISTSTART || !pPrQuery->bSyncAccess)
				if (plProgressCount)
					*plProgressCount = lOldProgressCount;

			if (!m_listQuery.bRemoveCell((PVOID) pPrQuery))
				delete pPrQuery;
		}

	if (ppCItems && *ppCItems)
	{
		ReleaseChatItems(*ppCItems);
		*ppCItems = NULL;
	}

	return FALSE;
}


BOOL CChatSocket::bChangeUserProperty(LPCWSTR wszUserPropertyName, VARIANT *pvUserProperty, LPCTSTR szUserNickname)
{
	CChatItems	*pCItems;
	LPTSTR		szIdentityTmp = NULL, szIdentMask = NULL;
	LONG		lOldValue, lValueIg, lValue = 0L;
	SHORT		cbLen;
	DWORD		dwPropertyAccessTimeOut = 0L;
	BOOL		bRet, bIsIgnored, bWantToIgnore, bWantToUnIgnore, bFireEvent = FALSE;

	OutputDebugThreadIdString("CChatSocket::bChangeUserProperty - Enter\n");

	ASSERT(pvUserProperty, "pvUserProperty is NULL in CChatSocket::bChangeUserProperty");

	USES_CONVERSION;

	// Check if we are logged or not
	if (!bCheckLogged())
		return FALSE;

	if (!wszUserPropertyName)
	{
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Make sure that the property name is known
	enumUserPropertyName upnProp = (enumUserPropertyName) NGetPropertyIndex((SHORT) atUser, wszUserPropertyName);

	if (upnProp < 0)
	{
		// this property is not a user prop
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	if (pvUserProperty->vt == (VT_BYREF+VT_VARIANT))
		pvUserProperty = pvUserProperty->pvarVal;

	switch (upnProp)
	{
		case pnUserModes:
		{
			if (!pvUserProperty || (pvUserProperty->vt != VT_I2 && pvUserProperty->vt != VT_I4))
				// the new prop variant can only be a SHORT or LONG for
				// the user mode (invisible/visible + ignore/don't ignore, etc flags)
			{
				m_hrLastError = E_INVALIDARG;
				return FALSE;
			}
			if (VT_I2 == pvUserProperty->vt)
				lValue = (LONG) pvUserProperty->iVal;
			else
			{
				lValue = pvUserProperty->lVal;
				ASSERT(VT_I4 == pvUserProperty->vt, "Unexpected variant type for new property in CChatSocket::bChangeUserProperty");
			}
			break;
		}
		case pnUserNickname:
		{
			// Target has to be the caller himself
			if (0 != lstrcmpi(m_szNickname, szUserNickname))
			{
				m_hrLastError = CC_E_PROPREADONLY;		// We only allow changing our own nickname
				return FALSE;
			}
			// Get the new nickname
			if (!pvUserProperty || (pvUserProperty->vt != VT_BSTR))
				// the new prop variant can only be a BSTR for the user nickname
			{
				m_hrLastError = E_INVALIDARG;
				return FALSE;
			}

			//if (!pvUserProperty->bstrVal || g_chEOS == *pvUserProperty->bstrVal)
			//{
			//	m_hrLastError = CC_E_BADNICKNAME;
			//	return FALSE;
			//}

			//LPTSTR szNicknameTmp;

			//if (!(szNicknameTmp = W2T(pvUserProperty->bstrVal)))
			//{
			//	m_hrLastError = E_OUTOFMEMORY;
			//	return FALSE;
			//}

			return bChangeNickname(pvUserProperty->bstrVal);
		}
		default:
			m_hrLastError = CC_E_PROPREADONLY;		// We only allow changing the UserModes property
			goto exit;
	}

	if (!bIsUserModesValid(lValue))
	{
		m_hrLastError = CC_E_USERBADMODES;		// Given UserModes is invalid
		goto exit;
	}

	// First take care of the local bits

	// Want to ignore the user or not?
	bWantToIgnore = lValue & umClientIgnored;
	bWantToUnIgnore = lValue & umNotClientIgnored;

	m_hrLastError = NOERROR;

	// Let's get the current UserModes and Identity of this user

	// We cash the timeout value and overwrite it for this internal synchronous call
	dwPropertyAccessTimeOut = m_dwPropertyAccessTimeOut;
	m_dwPropertyAccessTimeOut = (DWORD) g_lDefaultPropertyAccessTimeOut;

	if (bQueryUserProperty(TRUE, g_rgwszUserPropertyNames[pnUserModes], szUserNickname, &pCItems))
	{
		VARIANT vItem;

		// pCItems was correctly set
		ASSERT(pCItems, "pCItems is NULL in CChatSocket::bChangeUserProperty");
		if (pCItems->bGetItem((BSTR&) g_rgwszUserPropertyNames[pnUserModes], &vItem))
		{
			ASSERT(VT_I4 == vItem.vt, "VT_I4 != vItem.vt in CChatSocket::bChangeUserProperty");
			lOldValue = vItem.lVal;
		}
		else
		{
			// Extracting the user modes failed
			ReleaseChatItems(pCItems);
			goto exit;
		}
		if (pCItems->bGetItem((BSTR&) g_rgwszUserPropertyNames[pnUserIdentity], &vItem))
		{
			ASSERT(VT_BSTR == vItem.vt, "VT_BSTR != vItem.vt in CChatSocket::bChangeUserProperty");

			if (!(szIdentityTmp = W2T(vItem.bstrVal)))
			{
				m_hrLastError = E_OUTOFMEMORY;
				goto exit;
			}
			ReleaseChatItems(pCItems);
		}
		else
		{
			// Extracting the user identity failed
			ReleaseChatItems(pCItems);
			goto exit;
		}
	}
	else
		// Querying the user modes and identity failed
		goto exit;

	ASSERT(szIdentityTmp, "szIdentityTmp is NULL in CChatSocket::bChangeUserProperty");

	lValueIg = lOldValue;

	if (bWantToIgnore || bWantToUnIgnore)
	{
		bIsIgnored = lOldValue & umClientIgnored;

		// Construct szIdentMask
		if (!(szIdentMask = new TCHAR[(cbLen = lstrlen(szUserNickname))+lstrlen(szIdentityTmp)+2]))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
		lstrcpyn(szIdentMask, szUserNickname, cbLen+1);
		szIdentMask[cbLen] = '!';
		szIdentMask[cbLen+1] = g_chEOS;
		lstrcat(szIdentMask, szIdentityTmp);

		if (bWantToIgnore && !bIsIgnored)
		{
			if ((lOldValue & umSysop) || (lOldValue & umAdmin))
			{
				m_hrLastError = CC_E_CANTIGNORE;	// User can't ignore a SysOp or Admin
				goto exit;
			}
			// We need to add this user to our list
			bRet = bIgnoreUsers(szIdentMask, TRUE);
			ASSERT(bRet, "bIgnoreUsers failed in CChatSocket::bChangeUserProperty");

			lValueIg |= umClientIgnored;
			lValueIg &= ~umNotClientIgnored;

			// Need to fire the OnUserPropertyChanged event
			bFireEvent = TRUE;
		}

		if (bWantToUnIgnore && bIsIgnored)
		{
			// We need to remove this user from our list
			bRet = bIgnoreUsers(szIdentMask, FALSE);
			ASSERT(bRet, "bIgnoreUsers failed in CChatSocket::bChangeUserProperty");

			lValueIg &= ~umClientIgnored;
			lValueIg |= umNotClientIgnored;

			// Need to fire the OnUserPropertyChanged event
			bFireEvent = TRUE;
		}

		if (bFireEvent)
		{
			VARIANT vOldUserProperty, vNewUserProperty;
			vOldUserProperty.vt = vNewUserProperty.vt = VT_I4;
			vOldUserProperty.lVal = lOldValue;
			vNewUserProperty.lVal = lValueIg;
			ASSERT(bIsUserModesValid(lValueIg), "Invalid user modes in CChatSocket::bChangeUserProperty");
			bRet = bFireUserPropertyChanged(szUserNickname, m_szNickname, g_rgwszUserPropertyNames[pnUserModes], &vOldUserProperty, &vNewUserProperty);
			ASSERT(bRet, "bFireUserPropertyChanged failed in CChatSocket::bChangeUserProperty");
		}
	}

	// Take care of the remote bits now
	if (0L != (lValueIg ^ lValue))
	{
		BYTE	*pb;
		INT		cb;
		// There are some remote bits to change
		if (NOERROR == (m_hrLastError = HrMapToIRCAndMakeUserModeMsg(&pb, &cb, (CHAR*) szUserNickname, lValueIg, lValue)))
			m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
	}

exit:
	if (szIdentMask)
		delete [] szIdentMask;

	if (dwPropertyAccessTimeOut)
		m_dwPropertyAccessTimeOut = dwPropertyAccessTimeOut;

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatSocket::bQueryUserProperty(BOOL bSyncAccess, LPCWSTR wszUserPropertyName, LPCTSTR szUserNickname, CChatItems **ppCItems)
{
	BYTE		*pb;
	INT			cb;
	PPRQUERY	pPrQuery = NULL;
	PPRUSER		pPrUser  = NULL;

	OutputDebugThreadIdString("CChatSocket::bQueryUserProperty - Enter\n");

	ASSERT(!bSyncAccess || ppCItems, "bSyncAccess && !ppCItems in CChatSocket::bQueryUserProperty");
	ASSERT(szUserNickname, "szUserNickname is NULL in CChatSocket::bQueryUserProperty");

	if (ppCItems)
		*ppCItems = NULL;

	if (!wszUserPropertyName)
	{
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// The UserNickname can not contain wildcards in this case
	if (SzSubStr(szUserNickname, "?") || SzSubStr(szUserNickname, "*"))
	{
		m_hrLastError = CC_E_BADNICKNAME;
		return FALSE;
	}

	// Make sure that the property name is known
	enumUserPropertyName upnProp = (enumUserPropertyName) NGetPropertyIndex((SHORT) atUser, wszUserPropertyName);

	if (upnProp < 0)
	{
		// this property is not a user prop
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Check if we are connected or not
	if (!bCheckLogged())
		return FALSE;

	if (bSyncAccess)
	{
		// Create a CChatItems object via a virtual function that in fact creates a new CItemsObject object
		if (!bCreateChatItems(ppCItems))
			return FALSE;

		ASSERT(*ppCItems, "*ppCItems is NULL in CChatSocket::bQueryUserProperty");
	}

	pPrQuery = new PRQUERY;
	pPrUser = new PRUSER;
	if (!pPrQuery || !pPrUser)
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}

	ZeroMemory(pPrUser, sizeof(PRUSER));

	pPrQuery->at = atUser;
	pPrQuery->szChannelName = NULL;
	pPrQuery->bSyncAccess = bSyncAccess;
	pPrQuery->uHeadReplyCode = 0;
	pPrQuery->pvData = (PVOID) pPrUser;

	if (!bCopyStr(&(pPrUser->szNickname), szUserNickname))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	pPrUser->lIdleTime = -1L;
	pPrUser->lSignOnTime = -1L;
	pPrUser->lChannelCount = -1L;
	pPrUser->lChannelOwnerCount = -1L;
	pPrUser->lChannelHostCount = -1L;
	pPrUser->lChannelVoiceCount = -1L;

	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bQueryUserProperty");
	// Getting the info for this user?
	if (0 == lstrcmpi(m_szNickname, szUserNickname))
		pPrUser->lModes	= m_lUserModes;
	else
		pPrUser->lModes	= umNotSysop|\
						  umNotAdmin|\
						  umNotInvisible;	// by default, a user is not a sysop, and not an admin

	// The umClientIgnored or umNotClientIgnored flag is set once we got the server answers

	ASSERT(bIsUserModesValid(pPrUser->lModes), "Invalid user modes in CChatSocket::bQueryUserProperty");

	pPrUser->psaChannels = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxUserChannels);
	pPrUser->psaChannelsOwner = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxUserChannels);
	pPrUser->psaChannelsHost = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxUserChannels);
	pPrUser->psaChannelsVoice = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxUserChannels);
	if (!pPrUser->psaChannels || 
		!pPrUser->psaChannelsOwner || 
		!pPrUser->psaChannelsHost || 
		!pPrUser->psaChannelsVoice)
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}

	// Always add query cell at the end of the list since no prop can be local
	if (!m_listQuery.bInsertCell((LONG) m_listQuery.DwGetCellCount()+1, (PVOID) pPrQuery))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}

	// We assume here that all the properties come from the WHOIS command
	// Send the request to the server
	pPrUser->lChannelCount		= 0L;	// Only do this when doing a WHOIS command
	pPrUser->lChannelOwnerCount = 0L;	// Idem
	pPrUser->lChannelHostCount	= 0L;	// Idem
	pPrUser->lChannelVoiceCount	= 0L;	// Idem
	pPrQuery->uHeadReplyCode = RPL_WHOISUSER;
	if (SUCCEEDED(m_hrLastError = HrMakeWhoIsMsg(&pb, &cb, (CHAR*) szUserNickname)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
	
	if (FAILED(m_hrLastError))
		goto error;

	if (bSyncAccess)
	{
		// Wait for the property to come!
		if (!bWaitForProperty(atUser, (SHORT) upnProp, pPrQuery))
			goto error;

		if (!bFillChatItemsFromUser(*ppCItems, pPrUser))
			goto error;

		BOOL bRet = m_listQuery.bRemoveCell((PVOID) pPrQuery);
		ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatSocket::bQueryUserProperty");
	}
	else
		m_lWhoIsInProgress++;

	return TRUE;

error:
	if (m_hrLastError == CC_E_TIMEOUT)
	{
		// We know wait for the answer asynchronously
		ASSERT(pPrQuery->bSyncAccess, "pPrQuery->bSyncAccess is FALSE in CChatSocket::bQueryUserProperty");
		pPrQuery->bSyncAccess = FALSE;
		m_lWhoIsInProgress++;
	}
	else
	{
		// Remove query cell from list
		if (!m_listQuery.bRemoveCell((PVOID) pPrQuery))
		{
			if (pPrUser)
			{
				FreePrUserContent(pPrUser);
				delete pPrUser;
			}
			if (pPrQuery)
				delete pPrQuery;
		}
	}

	if (ppCItems && *ppCItems)
	{
		ReleaseChatItems(*ppCItems);
		*ppCItems = NULL;
	}

	return FALSE;
}


BOOL CChatSocket::bQueryServerProperty(BOOL bSyncAccess, LPCWSTR wszServerPropertyName, CChatItems **ppCItems)
{
	OutputDebugThreadIdString("CChatSocket::bQueryServerProperty - Enter\n");

	ASSERT(!bSyncAccess || ppCItems, "bSyncAccess && !ppCItems in CChatSocket::bQueryServerProperty");

	if (ppCItems)
		*ppCItems = NULL;

	if (!wszServerPropertyName)
	{
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Make sure that the property name is known
	enumServerPropertyName spnProp = (enumServerPropertyName) NGetPropertyIndex((SHORT) atServer, wszServerPropertyName);

	if (spnProp < 0)
	{
		// this property is not a server prop
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Check if we are connected or not
	switch (m_csState)
	{
		case csDisconnected:
		case csConnecting:
			m_hrLastError = CC_E_NOTCONNECTED;
			return FALSE;
		
		case csDisconnecting:
			m_hrLastError = CC_E_ALREADYDISCONNECTING;
			return FALSE;

		case csConnected:
		case csLogging:
			// Some property access require to be logged on
			if (spnProp != pnServerName &&
				spnProp != pnServerIgnoredUsers &&
				spnProp != pnServerMaxMessageLength &&
				spnProp != pnServerAnonymousAllowed &&
				spnProp != pnServerSecurityPackages)
			{
				m_hrLastError = CC_E_NOTLOGGED;
				return FALSE;
			}
			break;

		// case csLogged:
			// we're fine all the time
	}

	// Those server properties always need to be refreshed, and are never local
	m_prServer.lChannelCount = -1L;
	m_prServer.lNetInvisibleCount = -1L;
	m_prServer.lNetServerCount = -1L;
	m_prServer.lNetUserCount = -1L;
	m_prServer.lNodeServerCount = -1L;
	m_prServer.lNodeUserCount = -1L;
	m_prServer.lSysopCount = -1L;
	m_prServer.lUnknownConnectionCount = -1L;

	if (bSyncAccess)
	{
		// Create a CChatItems object via a virtual function that in fact creates a new CItemsObject object ;-)
		if (!bCreateChatItems(ppCItems))
			return FALSE;

		ASSERT(*ppCItems, "*ppCItems is NULL in CChatSocket::bQueryServerProperty");
	}

	// is the property already local or not?
	if (m_rgbServerPropLocal[spnProp])
	{
		if (bSyncAccess)
			// REGISB: release *ppCItems if bFillServerItems fails!
			return bFillServerItems(*ppCItems);
		else
			return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_SERVERPROP, 0L, 0L));
	}
	else
	{
		BYTE	*pb;
		INT		cb;

		// this property needs a client - server round trip
		switch (spnProp)
		{
			case pnServerInfo:
			{
				ASSERT(m_prServer.psaInfo == NULL, "m_prServer.psaInfo != NULL in CChatSocket::bQueryServerProperty");

				m_prServer.psaInfo = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxServerInfo);
				if (!m_prServer.psaInfo)
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}

				m_prServer.lInfoCount = 0L;

				// Send the request to the server
				if (SUCCEEDED(m_hrLastError = HrMakeServerInfoMsg(&pb, &cb)))
					m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
				
				if (FAILED(m_hrLastError))
					return FALSE;

				break;
			}

			case pnServerChannelCount:
			case pnServerNetInvisibleCount:
			case pnServerNetServerCount:
			case pnServerNetUserCount:
			case pnServerNodeServerCount:
			case pnServerNodeUserCount:
			case pnServerSysopCount:
			case pnServerUnknownConnectionCount:
			{
				// Send the request to the server
				if (SUCCEEDED(m_hrLastError = HrMakeLUsersMsg(&pb, &cb)))
					m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
				
				if (FAILED(m_hrLastError))
					return FALSE;

				break;
			}

			default:
			{
				ASSERT(FALSE, "Unexpected server property in CChatSocket::bQueryServerProperty");
				m_hrLastError = E_FAIL;
				return FALSE;
			}
		}

		if (bSyncAccess)
		{
			// Wait for the property to come!
			if (!bWaitForProperty(atServer, (SHORT) spnProp, NULL))
				return FALSE;

			// REGISB: release *ppCItems if bFillServerItems fails!
			return bFillServerItems(*ppCItems);
		}
	}

	return TRUE;
}


BOOL CChatSocket::bWaitForProperty(enumAssociatedType at, SHORT nPropIndex, PPRQUERY pPrQuery)
{
	OutputDebugThreadIdString("CChatSocket::bWaitForProperty - Enter\n");

	PPRIRCMSG	pPrIrcMsg		= NULL;
	DWORD		dwStartTime		= GetTickCount();
	BOOL		bStopWaiting	= FALSE;
	BOOL		bGotPropStart	= FALSE;
	BOOL		bGotWholeProp	= FALSE;
	BOOL		bRet, bRemoveMsg;
	MSG			msg;
	UINT		uCode;
	HRESULT		hr;

	m_hrLastError = NOERROR;

	// At the beginning there is no message in the tmp window queue
	ASSERT(!PeekMessage(&msg, m_hwndMessEx, WM_U_IRC_MSG, WM_U_IRC_MSG, PM_NOREMOVE), "Unexpected message in tmp window in CChatSocket::bWaitForProperty");

	// If the beginning of the property hasn't arrived after m_dwPropertyAccessTimeOut
	// milliseconds we stop the waiting
	do
	{
		if (PeekMessage(&msg, m_hwndMess, WM_U_IRC_MSG, WM_U_IRC_MSG, PM_REMOVE))
		{
			// We got a server message (WM_U_IRC_MSG)
			// wParam = pointer to PRIRCMSG
			// lParam = 0L
			pPrIrcMsg = (PPRIRCMSG) msg.wParam;
			bRemoveMsg = FALSE;

			ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bWaitForProperty");
			
	 		if (uCode = pPrIrcMsg->uCode)
			{
				// The message is a result or error code
				// Is it an Error or a Result code?
				if (bIsErrorCode(uCode))
				{
					// We got an error code. Do we care about it?
					switch (at)
					{
						case atServer:
						{
							switch (nPropIndex)
							{
								case pnServerInfo:
								{
									// We shouldn't get any error code related to the INFO command
									// So it has to be related to a prior request or a connection problem
									// REGISB: Stop waiting if the connection was lost
									break;
								}
								case pnServerChannelCount:
								case pnServerNetInvisibleCount:
								case pnServerNetServerCount:
								case pnServerNetUserCount:
								case pnServerNodeServerCount:
								case pnServerNodeUserCount:
								case pnServerSysopCount:
								case pnServerUnknownConnectionCount:
								{
									// We shouldn't get any error code related to the LUSERS command
									// So it has to be related to a prior request or a connection problem
									// REGISB: Stop waiting if the connection was lost
									break;
								}
								default:
								{
									ASSERT(FALSE, "Unexpected property index in CChatSocket::bWaitForProperty");
									m_hrLastError = E_FAIL;
									goto exit;
								}
							}
							break;
						}

						case atUser:
						{
							switch (uCode)
							{
								case ERR_NOSUCHSERVER:
								case ERR_NONICKNAMEGIVEN:
								{
									ASSERT(FALSE, "Unexpected error code in CChatSocket::bWaitForProperty");
									m_hrLastError = E_FAIL;
									goto exit;
								}
								case ERR_NOSUCHNICK:
								{
									if (pPrQuery->uHeadReplyCode == RPL_WHOISUSER ||
										pPrQuery->uHeadReplyCode == RPL_USERHOST)
									{
										m_hrLastError = CC_E_NOSUCHNICK;
										bStopWaiting = bRemoveMsg = TRUE;
									}
									break;
								}
								default:
								{
									ASSERT(FALSE, "Got an error in CChatSocket::bWaitForProperty");
									// REGISB: don't know what to do yet in this situation
								}
							}
							break;
						}

						case atMember:
						{
							ASSERT(FALSE, "Got an error in CChatSocket::bWaitForProperty");
							// REGISB: don't know what to do yet in this situation
							break;
						}

						case atChannel:
						{
							switch (uCode)
							{
								case ERR_CHANOPRIVSNEEDED:
								{
									if (pPrQuery->uHeadReplyCode == RPL_BANLIST)
									{
										m_hrLastError = CC_E_NOTHOST;
										bStopWaiting = bRemoveMsg = TRUE;
									}
									break;
								}
								case ERR_NEEDMOREPARAMS:
								{
									ASSERT(FALSE, "Unexpected error code in CChatSocket::bWaitForProperty");
									m_hrLastError = E_FAIL;
									goto exit;
								}
								case ERR_NOSUCHCHANNEL:
								{
									if (pPrQuery->uHeadReplyCode == RPL_BANLIST ||
										pPrQuery->uHeadReplyCode == RPL_LISTSTART ||
										pPrQuery->uHeadReplyCode == RPL_CHANNELMODEIS)
									{
										m_hrLastError = CC_E_NOSUCHCHANNEL;
										bStopWaiting = bRemoveMsg = TRUE;
									}
									break;
								}
								case ERR_NOSUCHOBJECT:
								{
									if (pPrQuery->uHeadReplyCode == RPL_PROPLIST)
									{
										m_hrLastError = CC_E_NOSUCHCHANNEL;
										bStopWaiting = bRemoveMsg = TRUE;
									}
									break;
								}
								case ERR_NOTONCHANNEL:
								{
									if (pPrQuery->uHeadReplyCode == RPL_BANLIST)
									{
										m_hrLastError = CC_E_NOTONCHANNEL;
										bStopWaiting = bRemoveMsg = TRUE;
									}
									break;
								}
								case ERR_SECURITY:
								{
									if (pPrQuery->uHeadReplyCode == RPL_PROPLIST)
									{
										m_hrLastError = CC_E_PERMISSIONDENIED;
										bStopWaiting = bRemoveMsg = TRUE;
									}
									break;
								}
								default:
								{
									ASSERT(FALSE, "Got an untreated error in CChatSocket::bWaitForProperty");
									// REGISB: don't know what to do yet in this situation
								}
							}
							break;
						}

						default:
						{
							ASSERT(FALSE, "Unexpected associated type in CChatSocket::bWaitForProperty");
							m_hrLastError = E_FAIL;
							goto exit;
						}
					}
				}
				else
				{
					// We got a reply code. Do we care about it?
					switch (at)
					{
						case atServer:
						{
							switch (nPropIndex)
							{
								case pnServerInfo:
								{
									switch (uCode)
									{
										case RPL_ENDOFINFO:
										{
											bStopWaiting = bGotWholeProp = TRUE;
											// fall through
										}
										case RPL_INFO:
										{
											bRemoveMsg = bGotPropStart = TRUE;
											bRet = bHandleServerInfo(pPrIrcMsg, uCode, FALSE /*bAsyncAccess*/);
											ASSERT(bRet, "bHandleServerInfo failed in CChatSocket::bWaitForProperty");
											break;
										}
									}
									break;
								}
								case pnServerChannelCount:
								case pnServerNetInvisibleCount:
								case pnServerNetServerCount:
								case pnServerNetUserCount:
								case pnServerNodeServerCount:
								case pnServerNodeUserCount:
								case pnServerSysopCount:
								case pnServerUnknownConnectionCount:
								{
									switch (uCode)
									{
										case RPL_LUSERME:
										{
											bStopWaiting = bGotWholeProp = TRUE;
											// fall through
										}
										case RPL_LUSERCLIENT:
										case RPL_LUSEROP:
										case RPL_LUSERUNKNOWN:
										case RPL_LUSERCHANNELS:
										{
											bRemoveMsg = bGotPropStart = TRUE;
											bRet = bHandleServerLUsers(pPrIrcMsg, uCode, FALSE /*bAsyncAccess*/);
											ASSERT(bRet, "bHandleServerLUsers failed in CChatSocket::bWaitForProperty");
											break;
										}
									}
									break;
								}
								default:
								{
									ASSERT(FALSE, "Unexpected property index in CChatSocket::bWaitForProperty");
									m_hrLastError = E_FAIL;
									goto exit;
								}
							}
							break;
						}

						case atUser:
						{
							switch (uCode)
							{
								case RPL_WHOISUSER:
								case RPL_WHOISSERVER:
								case RPL_WHOISOPERATOR:
								case RPL_WHOISIDLE:
								case RPL_WHOISCHANNELS:
								case RPL_ENDOFWHOIS:
								case RPL_AWAY:
								{
									if (pPrQuery->uHeadReplyCode == RPL_WHOISUSER)
									{
										if (bRet = bHandleWhoIs(pPrQuery, pPrIrcMsg, uCode))
										{
											bRemoveMsg = bGotPropStart = TRUE;
											if (RPL_ENDOFWHOIS == uCode)
											{
												bStopWaiting = bGotWholeProp = TRUE;
											}
										}
										else
											bStopWaiting = (m_hrLastError != CC_E_NOTQUERIEDOBJECT);
									}
									break;
								}
								case RPL_USERHOST:
								{
									if (pPrQuery->uHeadReplyCode == RPL_USERHOST)
									{
										if (bRet = bHandleUserHost(pPrQuery, pPrIrcMsg))
										{
											bRemoveMsg = bGotPropStart = bGotWholeProp = bStopWaiting = TRUE;
										}
										else
											bStopWaiting = (m_hrLastError != CC_E_NOTQUERIEDOBJECT);
									}
									break;
								}
							}
							break;
						}

						case atMember:
						{
							switch (uCode)
							{
								case RPL_USERHOST:
								{
									if (pPrQuery->uHeadReplyCode == RPL_USERHOST)
									{
										bRet = bHandleUserHost(pPrQuery, pPrIrcMsg);
										if (bRet)
										{
											bRemoveMsg = bGotPropStart = bGotWholeProp = bStopWaiting = TRUE;
										}
										else
											bStopWaiting = (m_hrLastError != CC_E_NOTQUERIEDOBJECT);
									}
									break;
								}
							}
							break;
						}

						case atChannel:
						{
							switch (uCode)
							{
								case RPL_LISTSTART:
								case RPL_LIST:
								case RPL_LISTEND:
									if (pPrQuery->uHeadReplyCode == RPL_LISTSTART)
									{
										bRet = bHandleListMsg(pPrQuery, pPrIrcMsg, uCode);
										if (bRet)
										{
											bRemoveMsg = bGotPropStart = TRUE;
											if (RPL_LISTEND == uCode)
											{
												bStopWaiting = bGotWholeProp = TRUE;
											}
										}
										else
										{											
											bRemoveMsg = bStopWaiting = (m_hrLastError != CC_E_NOTQUERIEDOBJECT);
										}
									}
									break;

								case RPL_CHANNELMODEIS:
									if (pPrQuery->uHeadReplyCode == RPL_CHANNELMODEIS)
									{
										bRet = bHandleChannelModeIs(pPrQuery, pPrIrcMsg);
										if (bRet)
										{
											bRemoveMsg = bGotPropStart = bGotWholeProp = bStopWaiting = TRUE;
										}
										else
											bStopWaiting = (m_hrLastError != CC_E_NOTQUERIEDOBJECT);
									}
									break;
			
								case RPL_BANLIST:
								case RPL_ENDOFBANLIST:
									if (pPrQuery->uHeadReplyCode == RPL_BANLIST)
									{
										bRet = bHandleBanList(pPrQuery, pPrIrcMsg, uCode);
										if (bRet)
										{
											bRemoveMsg = bGotPropStart = TRUE;
											if (RPL_ENDOFBANLIST == uCode)
											{
												bStopWaiting = bGotWholeProp = TRUE;
											}
										}
										else
											bStopWaiting = (m_hrLastError != CC_E_NOTQUERIEDOBJECT);
									}
									break;

								case RPL_PROPLIST:
								case RPL_PROPEND:
									if (pPrQuery->uHeadReplyCode == RPL_PROPLIST)
									{
										bRet = bHandlePropReply(pPrQuery, pPrIrcMsg, uCode);
										if (bRet)
										{
											bRemoveMsg = bGotPropStart = TRUE;
											if (RPL_PROPEND == uCode)
											{
												bStopWaiting = bGotWholeProp = TRUE;
											}
										}
										else
											bStopWaiting = (m_hrLastError != CC_E_NOTQUERIEDOBJECT);
									}
									break;
							}
							break;
						}

						default:
						{
							ASSERT(FALSE, "Unexpected associated type in CChatSocket::bWaitForProperty");
							m_hrLastError = E_FAIL;
							goto exit;
						}
					}
				}
			}
			if (bRemoveMsg)
				// We treated that message and don't want to keep it in the message queue
				m_cconn.FreeIrcMsg(pPrIrcMsg);
			else
			{
				// We don't quite need this message now, we post it to our tmp window (m_hwndMessEx)
				#ifdef DEBUG
					sprintf(g_szDebugStr, "CChatSocket::bWaitForProperty - Reposting IRC Msg with command '%s' to our tmp window\n", pPrIrcMsg->szCommand);
					OutputDebugString(g_szDebugStr);
				#endif // DEBUG
				m_hrLastError = HrPostEvent(WM_U_IRC_MSG, (WPARAM) pPrIrcMsg, 0L, TRUE);
			}
		}
		// else there is nothing in the main queue yet
		if ((!bGotPropStart && (GetTickCount() - dwStartTime > m_dwPropertyAccessTimeOut)) ||
			(GetTickCount() - dwStartTime > 2 * m_dwPropertyAccessTimeOut))
			bStopWaiting = TRUE;
	}
	while (!bStopWaiting);

	if (!bGotWholeProp && NOERROR == m_hrLastError)
		m_hrLastError = CC_E_TIMEOUT;

exit:
	// There might be some messages in the tmp window queue that we need to put in front of the main queue
	if (PeekMessage(&msg, m_hwndMessEx, WM_U_IRC_MSG, WM_U_IRC_MSG, PM_NOREMOVE))
	{
		// Stop the second thread, the one that reads the incoming server messages on the socket
		m_cconn.LockPosting();

		OutputDebugThreadIdString("CChatSocket::bWaitForProperty - Transfering tailing main msgs to tmp queue\n");
		// empty the main queue, put all the remaining messages into the tmp queue
		while (PeekMessage(&msg, m_hwndMess, WM_U_IRC_MSG, WM_U_IRC_MSG, PM_REMOVE))
		{
			#ifdef DEBUG
				sprintf(g_szDebugStr, "CChatSocket::bWaitForProperty - Reposting command '%s' to tmp queue\n", ((PPRIRCMSG) msg.wParam)->szCommand);
				OutputDebugString(g_szDebugStr);
			#endif // DEBUG
			hr = HrPostEvent(WM_U_IRC_MSG, (WPARAM) msg.wParam, (LPARAM) msg.lParam, TRUE);
			ASSERT(SUCCEEDED(hr), "HrPostEvent failed in CChatSocket::bWaitForProperty");
		}

		OutputDebugThreadIdString("CChatSocket::bWaitForProperty - Transfering all tmp msgs to main queue\n");
		// transfer all messages in the tmp queue to the main queue
		while (PeekMessage(&msg, m_hwndMessEx, WM_U_IRC_MSG, WM_U_IRC_MSG, PM_REMOVE))
		{
			#ifdef DEBUG
				sprintf(g_szDebugStr, "CChatSocket::bWaitForProperty - Reposting command '%s' to main queue\n", ((PPRIRCMSG) msg.wParam)->szCommand);
				OutputDebugString(g_szDebugStr);
			#endif // DEBUG
			hr = HrPostEvent(WM_U_IRC_MSG, (WPARAM) msg.wParam, (LPARAM) msg.lParam);
			ASSERT(SUCCEEDED(hr), "HrPostEvent failed in CChatSocket::bWaitForProperty");
		}

		// The second thread can post messages again
		m_cconn.UnlockPosting();
	}

	// At the end there is no message in the tmp window queue
	ASSERT(!PeekMessage(&msg, m_hwndMessEx, WM_U_IRC_MSG, WM_U_IRC_MSG, PM_NOREMOVE), "Unexpected message in tmp window in CChatSocket::bWaitForProperty");

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatSocket::bGetChatItemsFromIdentMask(CChatItems **ppCItems, LPTSTR szIdentMask, enumAssociatedType at)
{
	ASSERT(szIdentMask, "szIdentMask is NULL in CChatSocket::bGetChatItemsFromIdentMask");
	ASSERT(ppCItems, "ppCItems is NULL in CChatSocket::bGetChatItemsFromIdentMask");

	*ppCItems = NULL;

	if (!bCreateChatItems(ppCItems))
		return FALSE;

	ASSERT(*ppCItems, "*ppCItems is NULL in CChatSocket::bGetChatItemsFromIdentMask");

	// szIdentMask is of the form <nickname>!<username>@ipaddress

	// First set the AssociatedType of the ChatItems
	if (!(*ppCItems)->bSetAssociatedType(g_rgwszAssociatedTypes[at]))
	{
		m_hrLastError = (*ppCItems)->HrGetLastError();
		goto error;
	}

	// Figure out the three components of the mask
	PRUSERMATCH			prUserMatch;
	BOOL				bStartsWith, bEndsWith;
	TCHAR				ch;
	enumQueryOperator	qo;
	
	bGetUserMatchFromMask(szIdentMask, &prUserMatch);

	// Set the IPAddress property
	if (prUserMatch.cbIPAddress)
	{
		ASSERT(prUserMatch.szIPAddress, "prUserMatch.szIPAddress is NULL in CChatSocket::bGetChatItemsFromIdentMask");
		ch = prUserMatch.szIPAddress[prUserMatch.cbIPAddress];
		prUserMatch.szIPAddress[prUserMatch.cbIPAddress] = g_chEOS;

		bEndsWith = (prUserMatch.szIPAddress[0] == '*');
		if (bStartsWith = (prUserMatch.szIPAddress[prUserMatch.cbIPAddress-1] == '*'))
			prUserMatch.szIPAddress[prUserMatch.cbIPAddress-1] = g_chEOS;

		if (!(*ppCItems)->bSetItem(g_rgwszUserPropertyNames[pnUserIPAddress], prUserMatch.szIPAddress+(bEndsWith?1:0), 0L))
		{
			m_hrLastError = (*ppCItems)->HrGetLastError();
			goto error;
		}

		if (bStartsWith)
			prUserMatch.szIPAddress[prUserMatch.cbIPAddress-1] = '*';	// Fix what we broke
		prUserMatch.szIPAddress[prUserMatch.cbIPAddress] = ch;

		if (bEndsWith || bStartsWith)
		{
			if (bEndsWith && bStartsWith)
				qo = qoContains;
			else if (bEndsWith)
				qo = qoEndsWith;
			else
				qo = qoStartsWith;

			if (!(*ppCItems)->bSetItem(g_rgwszUserPropertyNames[pnUserIPAddressOp], g_rgszQueryOperators[qo], 0L))
			{
				m_hrLastError = (*ppCItems)->HrGetLastError();
				goto error;
			}
		}
	}

	// Set the UserName property
	if (prUserMatch.cbUserName)
	{
		ASSERT(prUserMatch.szUserName, "prUserMatch.szUserName is NULL in CChatSocket::bGetChatItemsFromIdentMask");
		ch = prUserMatch.szUserName[prUserMatch.cbUserName];
		prUserMatch.szUserName[prUserMatch.cbUserName] = g_chEOS;

		bEndsWith = (prUserMatch.szUserName[0] == '*');
		if (bStartsWith = (prUserMatch.szUserName[prUserMatch.cbUserName-1] == '*'))
			prUserMatch.szUserName[prUserMatch.cbUserName-1] = g_chEOS;

		if (!(*ppCItems)->bSetItem(g_rgwszUserPropertyNames[pnUserUserName], prUserMatch.szUserName+(bEndsWith?1:0), 0L))
		{
			m_hrLastError = (*ppCItems)->HrGetLastError();
			goto error;
		}

		if (bStartsWith)
			prUserMatch.szUserName[prUserMatch.cbUserName-1] = '*';	// Fix what we broke
		prUserMatch.szUserName[prUserMatch.cbUserName] = ch;

		if (bEndsWith || bStartsWith)
		{
			if (bEndsWith && bStartsWith)
				qo = qoContains;
			else if (bEndsWith)
				qo = qoEndsWith;
			else
				qo = qoStartsWith;

			if (!(*ppCItems)->bSetItem(g_rgwszUserPropertyNames[pnUserUserNameOp], g_rgszQueryOperators[qo], 0L))
			{
				m_hrLastError = (*ppCItems)->HrGetLastError();
				goto error;
			}
		}
	}

	// Set the Nickname property
	if (prUserMatch.cbNickname)
	{
		ASSERT(prUserMatch.szNickname, "prUserMatch.szNickname is NULL in CChatSocket::bGetChatItemsFromIdentMask");
		ch = prUserMatch.szNickname[prUserMatch.cbNickname];
		prUserMatch.szNickname[prUserMatch.cbNickname] = g_chEOS;

		bEndsWith = (prUserMatch.szNickname[0] == '*');
		if (bStartsWith = (prUserMatch.szNickname[prUserMatch.cbNickname-1] == '*'))
			prUserMatch.szNickname[prUserMatch.cbNickname-1] = g_chEOS;

		if (!(*ppCItems)->bSetItem(g_rgwszUserPropertyNames[pnUserNickname], prUserMatch.szNickname+(bEndsWith?1:0), 0L))
		{
			m_hrLastError = (*ppCItems)->HrGetLastError();
			goto error;
		}

		if (bStartsWith)
			prUserMatch.szNickname[prUserMatch.cbNickname-1] = '*';	// Fix what we broke
		prUserMatch.szNickname[prUserMatch.cbNickname] = ch;

		if (bEndsWith || bStartsWith)
		{
			if (bEndsWith && bStartsWith)
				qo = qoContains;
			else if (bEndsWith)
				qo = qoEndsWith;
			else
				qo = qoStartsWith;

			if (!(*ppCItems)->bSetItem(g_rgwszUserPropertyNames[pnUserNicknameOp], g_rgszQueryOperators[qo], 0L))
			{
				m_hrLastError = (*ppCItems)->HrGetLastError();
				goto error;
			}
		}
	}

	return TRUE;

error:
	ReleaseChatItems(*ppCItems);
	*ppCItems = NULL;
	return FALSE;
}


BOOL CChatSocket::bGetIdentMaskFromChatItems(CChatItems *pCItems, LPTSTR *pszIdentMask)
{
	enumMemberPropertyName mpnProp;
	BSTR		bstrAssociatedType;
	PPRITEM		pPrItem		= NULL;
	CItemLList	*plistItem	= NULL;
	LPTSTR		szNickname	= NULL;
	LPTSTR		szUserName	= NULL;
	LPTSTR		szIPAddress	= NULL;
	LPTSTR		szIdentMask, szValueTmp;
	SHORT		cbIdentMask = 0;
	VARIANT		vName;
	VARIANT		vValue;
	BOOL		bExtendedNickname = FALSE;

	ASSERT(pszIdentMask, "pszIdentMask is NULL in CChatSocket::bGetIdentMaskFromChatItems");

	*pszIdentMask = NULL;
	m_hrLastError = NOERROR;

	USES_CONVERSION;

	// Make sure the associated type is correct: Query
	if (!pCItems || 
		!(bstrAssociatedType = pCItems->BstrGetAssociatedType()) ||
		0 != WStricmp(bstrAssociatedType, g_rgwszAssociatedTypes[atQuery]))
	{
		m_hrLastError = E_INVALIDARG;
		return FALSE;
	}

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bGetIdentMaskFromChatItems");

	plistItem = pCItems->GetItemList();

	ASSERT(plistItem, "plistItem is NULL in CChatSocket::bGetIdentMaskFromChatItems");

	for (LONG lPos = 1L; lPos <= (LONG) plistItem->DwGetCellCount(); lPos++)
	{
		if (!plistItem->bGetCell(lPos, (PVOID*) &pPrItem))
		{
			m_hrLastError = E_FAIL;
			return FALSE;
		}
		ASSERT(pPrItem, "pPrItem is NULL in CChatSocket::bGetIdentMaskFromChatItems");
		
		vName = pPrItem->vName;
		vValue = pPrItem->vValue;

		// Is the property name a string?			// REGISB: could it possibly be something else?
		if (vName.vt != VT_BSTR || !vName.bstrVal)
		{
			m_hrLastError = CC_E_INVALIDITEMNAME;
			return FALSE;
		}
			
		// Member Properties	
		//pnMemberIdentity,
		//pnMemberIPAddress,
		//pnMemberIPAddressOp,
		//pnMemberModes,
		//pnMemberNickname,
		//pnMemberNicknameOp,
		//pnMemberObjectId,
		//pnMemberUserName,
		//pnMemberUserNameOp

		// Make sure that the property name is known
		mpnProp = (enumMemberPropertyName) NGetPropertyIndex((SHORT) atMember, vName.bstrVal);

		if (mpnProp < 0)
		{
			// this property is not a member prop
			m_hrLastError = CC_E_INVALIDITEMNAME;
			return FALSE;
		}
		
		// Check if value has right type
		switch (mpnProp)
		{
			case pnMemberIdentity:
			case pnMemberModes:
			case pnMemberObjectId:
				// We can not use those properties in that context
				m_hrLastError = CC_E_ITEMNAMENA;
				break;

			case pnMemberIPAddress:
			case pnMemberIPAddressOp:
			case pnMemberNickname:
			case pnMemberNicknameOp:
			case pnMemberUserName:
			case pnMemberUserNameOp:
				// Is the property value a string?
				if (vValue.vt != VT_BSTR)
				{
					m_hrLastError = CC_E_INVALIDITEMVALUE;
					return FALSE;
				}
				break;
		}

		// Construct the criteria strings
		if (vValue.bstrVal)
		{
			switch (mpnProp)
			{
				case pnMemberNickname:
				{
					if (bExtendedWideNickname(vValue.bstrVal) && bIsIrcXSocket())
					{
						// We deal with an extended nickname, need to convert it to UTF8
						bConvertWideStringToUTF8(vValue.bstrVal, 0, &szNickname, NULL, TRUE /*bNickname*/);
						bExtendedNickname = TRUE;
					}
					else
					{
						szValueTmp = W2T(vValue.bstrVal);
						bCopyStr(&szNickname, szValueTmp);
					}

					if (!szNickname)
					{
						m_hrLastError = E_OUTOFMEMORY;
						return FALSE;
					}

					cbIdentMask += lstrlen(szNickname);

					break;
				}

				case pnMemberIPAddress:
				case pnMemberUserName:
				{
					LPTSTR *pszValue = (mpnProp == pnMemberIPAddress ? &szIPAddress : &szUserName);

					szValueTmp = W2T(vValue.bstrVal);
					if (!szValueTmp || !bCopyStr(pszValue, szValueTmp))
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}

					cbIdentMask += lstrlen(*pszValue);

					break;
				}
			}
		}
	}

	// Take care of the string wildcards now
	for (lPos = 1L; lPos <= (LONG) plistItem->DwGetCellCount(); lPos++)
	{
		if (!plistItem->bGetCell(lPos, (PVOID*) &pPrItem))
		{
			m_hrLastError = E_FAIL;
			goto exit;
		}
		ASSERT(pPrItem, "pPrItem is NULL in CChatSocket::bGetIdentMaskFromChatItems");
		
		vName = pPrItem->vName;
		vValue = pPrItem->vValue;

		// Get the property index
		mpnProp = (enumMemberPropertyName) NGetPropertyIndex((SHORT) atMember, vName.bstrVal);
		ASSERT(mpnProp >= 0, "mpnProp < 0 in CChatSocket::bGetIdentMaskFromChatItems");
		
		// Construct identmask wildcards
		switch (mpnProp)
		{
			case pnMemberIPAddressOp:
			case pnMemberNicknameOp:
			case pnMemberUserNameOp:
			{
				if (vValue.bstrVal)
				{
					LPTSTR *pszValue = (mpnProp == pnMemberIPAddressOp ? &szIPAddress : (mpnProp == pnMemberNicknameOp ? &szNickname : &szUserName));
					// Make sure that the query operator is known
					enumQueryOperator qo = (enumQueryOperator) NGetPropertyIndex((SHORT) atQuery, vValue.bstrVal);
					if (qo < 0)
					{
						// this string is not a query operator
						m_hrLastError = CC_E_NOTOPERATOR;
						goto exit;
					}

					if (!bOperateStr(pszValue, (SHORT) qo, bExtendedNickname && pnMemberNicknameOp == mpnProp))
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}
				}
				else
				{
					m_hrLastError = CC_E_NOTOPERATOR;
					goto exit;
				}
				break;
			}
		}
	}

	if (!(*pszIdentMask = szIdentMask = new TCHAR[cbIdentMask+16]))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto exit;
	}
	szIdentMask[0] = g_chEOS;

	if (szNickname)
	{
		// The user has a nickname criteria
		lstrcat(szIdentMask, szNickname);
		lstrcat(szIdentMask, "!");
	}
	else
		lstrcat(szIdentMask, "*!");

	if (szUserName)
	{
		// The user has a username criteria
		lstrcat(szIdentMask, szUserName);
		lstrcat(szIdentMask, "@");
	}
	else
		lstrcat(szIdentMask, "*@");

	if (szIPAddress)
		// The user has a ipaddress criteria
		lstrcat(szIdentMask, szIPAddress);
	else
		lstrcat(szIdentMask, "*");

exit:
	if (szNickname)
		delete [] szNickname;
	if (szUserName)
		delete [] szUserName;
	if (szIPAddress)
		delete [] szIPAddress;

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatSocket::bGetChannelNameFromVariant(VARIANT *pvChannelName, LPTSTR *pszChannelName)
{
	ASSERT(pvChannelName, "pvChannelName is NULL in CChatSocket::bGetChannelNameFromVariant");
	ASSERT(pszChannelName, "pszChannelName is NULL in CChatSocket::bGetChannelNameFromVariant");

	if (pvChannelName->vt == (VT_BYREF+VT_VARIANT))
		pvChannelName = pvChannelName->pvarVal;

	if (!pvChannelName ||
		!((pvChannelName->vt == VT_BSTR)  ||
		  (pvChannelName->vt == VT_EMPTY) ||
		  (pvChannelName->vt == VT_NULL)  ||
		  (pvChannelName->vt == VT_ERROR)))
	{
		// the channelname can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CChatSocket::bGetChannelNameFromVariant - Invalid VARIANT for ChannelName param\n");
		m_hrLastError = CC_E_CHANNELBADNAME;
		return FALSE;
	}

	if ((pvChannelName->vt != VT_BSTR) && (m_lOpenChannels != 1L))
	{
		// there is not only one open channel
		m_hrLastError = CC_E_OPENNOTUNIQUE;
		return FALSE;
	}

	if (pvChannelName->vt == VT_BSTR && pvChannelName->bstrVal && *pvChannelName->bstrVal)
	{
		// the caller specified a channelname
		USES_CONVERSION;

		if (bIsIrcXSocket() && bExtendedWideChannelName(pvChannelName->bstrVal))
		{
			if (L'%' == *pvChannelName->bstrVal)
			{
				// We deal with an extended channel name, need to convert it to UTF8
				bConvertWideStringToUTF8(pvChannelName->bstrVal, 0, pszChannelName, NULL, FALSE /*bNickname*/, TRUE /*bChannelName*/);
			}
			else
			{
				// Might have to convert from ShiftJis to Jis, etc...
				LPTSTR	szConvertedChannelName;
				BOOL	bFreeConverted;
	
				bConvertString(FALSE, m_byteCharSet, W2T(pvChannelName->bstrVal), 0, &szConvertedChannelName, NULL, &bFreeConverted);

				if (bFreeConverted)
					*pszChannelName = szConvertedChannelName;
				else
					bCopyStr(pszChannelName, szConvertedChannelName);
			}
		}
		else
		{
			LPTSTR szChannelNameTmp = W2T(pvChannelName->bstrVal);
			if (szChannelNameTmp)
				bCopyStr(pszChannelName, szChannelNameTmp);
		}

		if (!*pszChannelName)
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
	}
	else
	{
		CChatChannel		*pChannel = NULL;
		enumChannelState	chs	= chsOpen;

		// we use the unique open channel name if the variant is empty
		ASSERT(m_lOpenChannels == 1L, "m_lOpenChannels != 1L in CChatSocket::bGetChannelNameFromVariant");
		if (!m_listChannel.bFindCellFromData(1L /*lPosition*/, 
											 g_nSearchByState /*nSearchID1*/,
											 (PVOID) &chs,
											 g_nSearchNone /*nSearchID2*/,
											 NULL,
											 (PVOID*) &pChannel,
											 NULL /*plPositionFound*/))
		{
			ASSERT(FALSE, "Could not find open channel in CChatSocket::bGetChannelNameFromVariant");
			m_hrLastError = E_FAIL;
			return FALSE;
		}
		ASSERT(pChannel, "pChannel is NULL in CChatSocket::bGetChannelNameFromVariant");
		ASSERT(pChannel->SzGetChannelName(), "pChannel->SzGetChannelName() is NULL in CChatSocket::bGetChannelNameFromVariant");
		// Channel name is already converted to UTF8 if necessary
		if (!bCopyStr(pszChannelName, pChannel->SzGetChannelName()))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
	}	

	return TRUE;
}


BOOL CChatSocket::bGetNicknameFromVariant(VARIANT *pvMemberNickname, LPTSTR *pszNickname)
{
	ASSERT(pvMemberNickname, "pvMemberNickname is NULL in CChatSocket::bGetNicknameFromVariant");
	ASSERT(pszNickname, "pszNickname is NULL in CChatSocket::bGetNicknameFromVariant");

	if (pvMemberNickname->vt == (VT_BYREF+VT_VARIANT))
		pvMemberNickname = pvMemberNickname->pvarVal;

	if (!pvMemberNickname || 
		!((pvMemberNickname->vt == VT_BSTR)  || 
		  (pvMemberNickname->vt == VT_EMPTY) || 
		  (pvMemberNickname->vt == VT_NULL)  || 
		  (pvMemberNickname->vt == VT_ERROR)))
	{
		// the nickname can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CChatSocket::bGetNicknameFromVariant - Invalid VARIANT for MemberNickname param\n");
		m_hrLastError = CC_E_BADNICKNAME;
		return FALSE;
	}

	if (pvMemberNickname->vt == VT_BSTR && pvMemberNickname->bstrVal && *pvMemberNickname->bstrVal)
	{
		// The caller specified a nickname
		// Is this an extended nickname?
		if (bIsIrcXSocket() && bExtendedWideNickname(pvMemberNickname->bstrVal))
		{
			// The new nickname is extended - need to convert it to UTF8
			if (!bConvertWideStringToUTF8(pvMemberNickname->bstrVal, 0, pszNickname, NULL, TRUE /*bNickname*/))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
		}
		else
		{
			USES_CONVERSION;
			LPTSTR szNicknameTmp = W2T(pvMemberNickname->bstrVal);
			if (!szNicknameTmp ||
				!bCopyStr(pszNickname, szNicknameTmp))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
		}
	}
	else
	{
		// we use this user's nickname if the variant is empty or missing
		ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bGetNicknameFromVariant");
		if (!bCopyStr(pszNickname, m_szNickname))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CChatSocket::bListUsers(CChatItems *pCItems)
{
	BSTR		bstrAssociatedType;
	PPRITEM		pPrItem			= NULL;
	CItemLList	*plistItem		= NULL;
	LPTSTR		*pszValue, szValueTmp;
	LPTSTR		szNickname		= NULL;
	LPTSTR		szIPAddress		= NULL;
	LPTSTR		szChannelName	= NULL;
	LPTSTR		szMask			= NULL;
	VARIANT		vName;
	VARIANT		vValue;
	BYTE		*pb;
	INT			cb;
	BOOL		bExtendedNickname = FALSE;
	PPRQUERY	pPrQuery = NULL;

	OutputDebugThreadIdString("CChatSocket::bListUsers - Enter\n");

	USES_CONVERSION;

	// Check if we are connected or not
	if (!bCheckLogged())
		return FALSE;

	if (!pCItems || 
		!(bstrAssociatedType = pCItems->BstrGetAssociatedType()) ||
		0 != WStricmp(bstrAssociatedType, g_rgwszAssociatedTypes[atQuery]))
	{
		m_hrLastError = E_INVALIDARG;
		return FALSE;
	}

	plistItem = pCItems->GetItemList();

	ASSERT(plistItem, "plistItem is NULL in CChatSocket::bListUsers");

	// We take care of the non-Op items first
	for (LONG lPos = 1L; lPos <= (LONG) plistItem->DwGetCellCount(); lPos++)
	{
		if (!plistItem->bGetCell(lPos, (PVOID*) &pPrItem))
		{
			m_hrLastError = E_FAIL;
			goto exit;
		}
		ASSERT(pPrItem, "pPrItem is NULL in CChatSocket::bListUsers");
		
		vName = pPrItem->vName;
		vValue = pPrItem->vValue;

		// Is the property name a string?
		if (vName.vt != VT_BSTR || !vName.bstrVal)	// REGISB: really necessary?
		{											// could it be something else?
			m_hrLastError = CC_E_INVALIDITEMNAME;
			goto exit;
		}
		
		// Make sure that the property name is known
		enumUserPropertyName upnProp = (enumUserPropertyName) NGetPropertyIndex((SHORT) atUser, vName.bstrVal);

		if (upnProp < 0)
		{
			// this property is not a user prop
			m_hrLastError = CC_E_INVALIDITEMNAME;
			goto exit;
		}
		
		// On IRC/IRC2 servers, only the pnUserNickname, pnUserNicknameOp, pnUserMask, pnUserMaskOp and pnUserChannels can be used, pfff.
		if (!bIsIrcXSocket() && 
			upnProp != pnUserNickname && 
			upnProp != pnUserNicknameOp && 
			upnProp != pnUserMask && 
			upnProp != pnUserMaskOp && 
			upnProp != pnUserChannels)
		{
			m_hrLastError = CC_E_NOTIRCX;
			goto exit;
		}

		// Check if value has right type
		switch (upnProp)
		{
			// So far we only deal with user nicknames, IP addresses, masks and channels
			case pnUserNickname:
			case pnUserNicknameOp:
			case pnUserMask:
			case pnUserMaskOp:
			case pnUserIPAddress:
			case pnUserIPAddressOp:
			case pnUserChannels:
				// Is the property value a string?
				if (vValue.vt != VT_BSTR)
				{
					m_hrLastError = CC_E_INVALIDITEMVALUE;
					goto exit;
				}
				break;
		}

		// Construct query for server
		if (vValue.bstrVal)
		{
			switch (upnProp)
			{
				case pnUserNickname:
				{
					if (bExtendedWideNickname(vValue.bstrVal) && bIsIrcXSocket())
					{
						// We deal with an extended nickname, need to convert it to UTF8
						bConvertWideStringToUTF8(vValue.bstrVal, 0, &szNickname, NULL, TRUE /*bNickname*/);
						bExtendedNickname = TRUE;
					}
					else
					{
						szValueTmp = W2T(vValue.bstrVal);
						bCopyStr(&szNickname, szValueTmp);
					}

					if (!szNickname)
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}

					break;
				}

				case pnUserMask:
				{
					if (bExtendedWideString(vValue.bstrVal) && bIsIrcXSocket())
					{
						// We deal with an extended string, need to convert it to UTF8
						bConvertWideStringToUTF8(vValue.bstrVal, 0, &szMask, NULL);
					}
					else
					{
						szValueTmp = W2T(vValue.bstrVal);
						bCopyStr(&szMask, szValueTmp);
					}

					if (!szMask)
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}

					break;
				}

				case pnUserIPAddress:
				{
					szValueTmp = W2T(vValue.bstrVal);
					if (!szValueTmp || !bCopyStr(&szIPAddress, szValueTmp))
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}

					break;
				}

				case pnUserChannels:
				{
					// Make sure the channel name starts with a valid character
					if (L'%' != *(vValue.bstrVal) &&
						L'#' != *(vValue.bstrVal) &&
						L'&' != *(vValue.bstrVal))
					{
						m_hrLastError = CC_E_CHANNELBADNAME;
						goto exit;
					}
					if (bIsIrcXSocket() && bExtendedWideChannelName(vValue.bstrVal))
					{
						if (L'%' == *(vValue.bstrVal))
							// We deal with an extended channel name, need to convert it to UTF8
							bConvertWideStringToUTF8(vValue.bstrVal, 0, &szChannelName, NULL, FALSE /*bNickname*/, TRUE /*bChannelName*/);
						else
						{
							// Might have to convert from ShiftJis to Jis, etc...
							LPTSTR	szConvertedChannelName;
							BOOL	bFreeConverted;
					
							bConvertString(FALSE, m_byteCharSet, W2T(vValue.bstrVal), 0, &szConvertedChannelName, NULL, &bFreeConverted);

							if (bFreeConverted)
								szChannelName = szConvertedChannelName;
							else
								bCopyStr(&szChannelName, szConvertedChannelName);
						}
					}
					else
					{
						szValueTmp = W2T(vValue.bstrVal);
						bCopyStr(&szChannelName, szValueTmp);
					}

					if (!szChannelName)
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}

					break;
				}
			}
		}
	}

	// We now take care of the Op items
	for (lPos = 1L; lPos <= (LONG) plistItem->DwGetCellCount(); lPos++)
	{
		if (!plistItem->bGetCell(lPos, (PVOID*) &pPrItem))
		{
			m_hrLastError = E_FAIL;
			goto exit;
		}
		ASSERT(pPrItem, "pPrItem is NULL in CChatSocket::bListUsers");
		
		vName = pPrItem->vName;
		vValue = pPrItem->vValue;

		// Get the user property name
		enumUserPropertyName upnProp = (enumUserPropertyName) NGetPropertyIndex((SHORT) atUser, vName.bstrVal);

		ASSERT(upnProp >= 0, "upnProp < 0 in CChatSocket::bListUsers");
		ASSERT(vName.vt == VT_BSTR, "vName.vt != VT_BSTR in CChatSocket::bListUsers");
		ASSERT(vValue.vt == VT_BSTR, "vValue.vt != VT_BSTR in CChatSocket::bListUsers");
	
		// Construct query for server
		switch (upnProp)
		{
			// So far we only deal with user nickname and IP address operators!
			case pnUserNicknameOp:
			case pnUserMaskOp:
			case pnUserIPAddressOp:
				if (vValue.bstrVal)
				{
					pszValue = (pnUserNicknameOp == upnProp) ? &szNickname : (pnUserIPAddressOp == upnProp) ? &szIPAddress : &szMask;

					// Make sure that the query operator is known
					enumQueryOperator qo = (enumQueryOperator) NGetPropertyIndex((SHORT) atQuery, vValue.bstrVal);
					if (qo < 0)
					{
						// this string is not a query operator
						m_hrLastError = CC_E_NOTOPERATOR;
						goto exit;
					}

					// This seems to work now on Kent's IRCX servers
					//if ((upnProp == pnUserIPAddressOp) && (qo != qoEndsWith))
					//{
					//	// Only the "EndsWith" operator can be use in this context
					//	// WHOIS *.fr
					//	m_hrLastError = CC_E_ITEMNAMENA;
					//	goto exit;
					//}

					if (!bOperateStr(pszValue, (SHORT) qo, bExtendedNickname && pnUserNicknameOp == upnProp))
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}

					// prefix the criteria with *!*@ if it's an IP Address one
					if (pnUserIPAddressOp == upnProp)
					{
						LPTSTR szIPA = new TCHAR[_tcslen(szIPAddress)+5];
						szIPA[0] = g_chEOS;
						_tcscpy(szIPA, "*!*@");
						_tcscat(szIPA, szIPAddress);
						delete [] szIPAddress;
						szIPAddress = szIPA;
					}
				}
				else
				{
					m_hrLastError = CC_E_NOTOPERATOR;
					goto exit;
				}
				break;
		}
	}

	if (!szNickname && !szMask && !szChannelName && !szIPAddress)
		// No criteria is specified, we list all the users
		if (bIsIrcXSocket())
			bCopyStr(&szNickname, "*");	// WHOIS * on IRCX servers
		else
			bCopyStr(&szMask, "*");		// WHO * on IRC servers

	if ((szNickname  && szChannelName)	||
		(szNickname  && szIPAddress)	||
		(szNickname  && szMask)			||
		(szIPAddress && szChannelName)	||
		(szIPAddress && szMask)	||
		(szMask && szChannelName))
	{
		m_hrLastError = E_INVALIDARG;
		goto exit;
	}

	if (szNickname || szIPAddress)
	{
		if (!(pPrQuery = new PRQUERY))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}

		pPrQuery->at = atQuery;
		pPrQuery->bSyncAccess = FALSE;
		pPrQuery->szChannelName = NULL;
	}

	if (szMask || szChannelName)
	{
		// WHO case
		if (SUCCEEDED(m_hrLastError = HrMakeWhoMsg(&pb, &cb, szMask ? szMask : szChannelName)))
			if (SUCCEEDED(m_hrLastError = m_cconn.HrSendAndDelete(pb, cb)))
				m_lWhoListingInProgress++;
	}
	else
	{
		// WHOIS case
		ASSERT(szNickname || szIPAddress, "!szNickname && !szIPAddress in CChatSocket::bListUsers");
		if (SUCCEEDED(m_hrLastError = HrMakeWhoIsMsg(&pb, &cb, (CHAR*) szNickname ? szNickname : szIPAddress)))
		{
			m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
			if (SUCCEEDED(m_hrLastError))
			{
				pPrQuery->uHeadReplyCode = RPL_WHOISUSER;
				pPrQuery->pvData = (PVOID) qtUserList;
				m_lWhoIsListingInProgress++;
			}
		}
	}
	if (SUCCEEDED(m_hrLastError))
		// FireBeginEnumeration asynchronously
		HrPostEvent(WM_U_BEGINENUMERATION, NULL, (LPARAM) etUsers);

	if (pPrQuery && !m_listQuery.bInsertCell((LONG) m_listQuery.DwGetCellCount()+1L, (PVOID) pPrQuery))
		m_hrLastError = E_OUTOFMEMORY;

exit:
	if (szNickname)
		delete [] szNickname;

	if (szMask)
		delete [] szMask;

	if (szIPAddress)
		delete [] szIPAddress;

	if (szChannelName)
		delete [] szChannelName;

	if (FAILED(m_hrLastError))
	{
		if (pPrQuery) delete [] pPrQuery;
		return FALSE;
	}
	else
		return TRUE;
}


BOOL CChatSocket::bListChannels(CChatItems *pCItems)
{
	BSTR		bstrAssociatedType;
	PPRITEM		pPrItem = NULL;
	CItemLList	*plistItem = NULL;
	TCHAR		szVal[32];
	LPTSTR		szName = NULL;
	LPTSTR		szTopic = NULL;
	LPTSTR		szLanguage = NULL;
	LPTSTR		szSubject = NULL;
	LPTSTR		szQuery = NULL;
	SHORT		cbQuery = 0;
	BOOL		bRegistered = FALSE, bUnregistered = FALSE;
	LONG		lListCount = -1L;
	LONG		lMemberCountMax = -1L;
	LONG		lMemberCountMin = -1L;
	LONG		lChannelAgeMax = -1L;
	LONG		lChannelAgeMin = -1L;
	LONG		lTopicAgeMax = -1L;
	LONG		lTopicAgeMin = -1L;
	VARIANT		vName;
	VARIANT		vValue;
	BYTE		*pb;
	INT			cb;

	OutputDebugThreadIdString("CChatSocket::bListChannels - Enter\n");

	// Check if we are connected or not
	if (!bCheckLogged())
		return FALSE;

	// Make sure that we won't expose unrated content when it's forbiden
	if (!bIsIrcXSocket() && !(m_bCanViewUnrated = bCanViewUnrated(m_hwndParent, TRUE /*bPromptOverride*/)))
	{
		m_hrLastError = CC_E_RATINGBLOCK;
		return FALSE;
	}

	if (!pCItems || 
		!(bstrAssociatedType = pCItems->BstrGetAssociatedType()) ||
		0 != WStricmp(bstrAssociatedType, g_rgwszAssociatedTypes[atQuery]))
	{
		m_hrLastError = E_INVALIDARG;
		return FALSE;
	}

	plistItem = pCItems->GetItemList();

	ASSERT(plistItem, "plistItem is NULL in CChatSocket::bListChannels");

	for (LONG lPos = 1L; lPos <= (LONG) plistItem->DwGetCellCount(); lPos++)
	{
		if (!plistItem->bGetCell(lPos, (PVOID*) &pPrItem))
		{
			m_hrLastError = E_FAIL;
			return FALSE;
		}
		ASSERT(pPrItem, "pPrItem is NULL in CChatSocket::bListChannels");
		
		vName = pPrItem->vName;
		vValue = pPrItem->vValue;

		// Is the property name a string?			// REGISB: could it possibly be something else?
		if (vName.vt != VT_BSTR || !vName.bstrVal)
		{
			m_hrLastError = CC_E_INVALIDITEMNAME;
			return FALSE;
		}
		
		// Make sure that the property name is known
		enumChannelPropertyName cpnProp = (enumChannelPropertyName) NGetPropertyIndex((SHORT) atChannel, vName.bstrVal);

		if (cpnProp < 0)
		{
			// this property is not a channel prop
			m_hrLastError = CC_E_INVALIDITEMNAME;
			return FALSE;
		}
		
		// On IRC2 servers, only the cpnName can be used, pfff.
		if (!bIsIrcXSocket() && cpnProp != cpnName)
		{
			m_hrLastError = CC_E_NOTIRCX;
			return FALSE;
		}

		// Check if value has right type
		switch (cpnProp)
		{
			case cpnName:
			case cpnTopic:
			case cpnLanguage:
			case cpnSubject:
			case cpnNameOp:
			case cpnTopicOp:
			case cpnLanguageOp:
			case cpnSubjectOp:
				// Is the property value a string?
				if (vValue.vt != VT_BSTR)
				{
					m_hrLastError = CC_E_INVALIDITEMVALUE;
					return FALSE;
				}
				break;
			case cpnChannelAgeMax:
			case cpnChannelAgeMin:
			case cpnListCount:
			case cpnMemberCount:
			case cpnMemberCountMax:
			case cpnMemberCountMin:
			case cpnTopicAgeMax:
			case cpnTopicAgeMin:
				// Is the property value a number?
				if (vValue.vt != VT_I4)
				{
					m_hrLastError = CC_E_INVALIDITEMVALUE;
					return FALSE;
				}
				break;
			case cpnRegistered:
				// Is the property a boolean?
				if (vValue.vt != VT_BOOL)
				{
					m_hrLastError = CC_E_INVALIDITEMVALUE;
					return FALSE;
				}
				break;
		}

		// Construct query for server
		switch (cpnProp)
		{
			case cpnName:
			case cpnTopic:
			case cpnLanguage:
			case cpnSubject:
				if (vValue.bstrVal)
				{
					LPTSTR *pszValue, szValueTmp;

					switch (cpnProp)
					{
						case cpnName:
							pszValue = &szName;
							break;
						case cpnTopic:
							pszValue = &szTopic;
							break;
						case cpnLanguage:
							pszValue = &szLanguage;
							break;
						case cpnSubject:
							pszValue = &szSubject;
					}

					USES_CONVERSION;

					if (cpnProp == cpnName && bIsIrcXSocket() && bExtendedWideChannelName(vValue.bstrVal))
						// We deal with an extended channel name fraction, need to convert it to UTF8
						bConvertWideStringToUTF8(vValue.bstrVal, 0, pszValue, NULL, FALSE /*bNickname*/, FALSE /*bChannelName*/, TRUE /*bPostProcess*/, TRUE /*bEscapeWildcards*/);
					else if (bIsIrcXSocket() && bExtendedWideString(vValue.bstrVal))
						// We deal with an extended string, need to convert it to UTF8
						bConvertWideStringToUTF8(vValue.bstrVal, 0, pszValue, NULL, FALSE /*bNickname*/, FALSE /*bChannelName*/, TRUE /*bPostProcess*/, TRUE /*bEscapeWildcards*/);
					else
						if (szValueTmp = W2T(vValue.bstrVal))
						{
							if (bIsIrcXSocket())
								bEncodeMask(szValueTmp, pszValue);
							else
								bCopyStr(pszValue, szValueTmp);
						}
					if (!*pszValue)
					{
						m_hrLastError = E_OUTOFMEMORY;
						return FALSE;
					}
					cbQuery += lstrlen(*pszValue);
				}
				break;
			case cpnListCount:
				lListCount = vValue.lVal;
				break;
			case cpnMemberCount:
				lMemberCountMax = vValue.lVal+1L;
				lMemberCountMin = vValue.lVal-1L;
				break;
			case cpnMemberCountMax:
				lMemberCountMax = vValue.lVal+1L;
				break;
			case cpnMemberCountMin:
				lMemberCountMin = vValue.lVal-1L;
				break;
			case cpnChannelAgeMax:
				lChannelAgeMax = vValue.lVal+1L;
				break;
			case cpnChannelAgeMin:
				lChannelAgeMin = vValue.lVal-1L;
				break;
			case cpnTopicAgeMax:
				lTopicAgeMax = vValue.lVal+1L;
				break;
			case cpnTopicAgeMin:
				lTopicAgeMin = vValue.lVal-1L;
				break;
			case cpnRegistered:
				if (vValue.boolVal)
					bRegistered = TRUE;
				else
					bUnregistered = TRUE;
				break;
		}
	}

	if (lChannelAgeMax > g_lAgeLimit ||
		lChannelAgeMin > g_lAgeLimit ||
		lTopicAgeMax > g_lAgeLimit ||
		lTopicAgeMin > g_lAgeLimit)
	{
		m_hrLastError = CC_E_INVALIDITEMVALUE;
		goto exit;
	}

	// Take care of the string wildcards now
	for (lPos = 1L; lPos <= (LONG) plistItem->DwGetCellCount(); lPos++)
	{
		if (!plistItem->bGetCell(lPos, (PVOID*) &pPrItem))
		{
			m_hrLastError = E_FAIL;
			goto exit;
		}
		ASSERT(pPrItem, "pPrItem is NULL in CChatSocket::bListChannels");
		
		vName = pPrItem->vName;
		vValue = pPrItem->vValue;

		// Get the property index
		enumChannelPropertyName cpnProp = (enumChannelPropertyName) NGetPropertyIndex((SHORT) atChannel, vName.bstrVal);
		ASSERT(cpnProp >= 0, "cpnProp < 0 in CChatSocket::bListChannels");
		
		// Construct query for server
		switch (cpnProp)
		{
			case cpnNameOp:
			case cpnTopicOp:
			case cpnLanguageOp:
			case cpnSubjectOp:
			{
				if (vValue.bstrVal)
				{
					LPTSTR *pszValue;

					switch (cpnProp)
					{
						case cpnNameOp:
							pszValue = &szName;
							break;
						case cpnTopicOp:
							pszValue = &szTopic;
							break;
						case cpnLanguageOp:
							pszValue = &szLanguage;
							break;
						case cpnSubjectOp:
							pszValue = &szSubject;
					}

					// Make sure that the query operator is known
					enumQueryOperator qo = (enumQueryOperator) NGetPropertyIndex((SHORT) atQuery, vValue.bstrVal);
					if (qo < 0)
					{
						// this string is not a query operator
						m_hrLastError = CC_E_NOTOPERATOR;
						goto exit;
					}

					if (!bOperateStr(pszValue, (SHORT) qo))
					{
						m_hrLastError = E_OUTOFMEMORY;
						goto exit;
					}
				}
				else
				{
					m_hrLastError = CC_E_NOTOPERATOR;
					goto exit;
				}
				break;
			}
		}
	}

	if (bIsIrcXSocket())
	{
		if (!(szQuery = new TCHAR[cbQuery+64]))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
		szQuery[0] = g_chEOS;

		if (szName)
		{
			lstrcat(szQuery, "N=");
			lstrcat(szQuery, szName);
		}
		if (szTopic)
		{
			lstrcat(szQuery, " T=");
			lstrcat(szQuery, szTopic);
		}
		if (szLanguage)
		{
			lstrcat(szQuery, " L=");
			lstrcat(szQuery, szLanguage);
		}
		if (szSubject)
		{
			lstrcat(szQuery, " S=");
			lstrcat(szQuery, szSubject);
		}
		if (lMemberCountMax >= 0L)
		{
			wsprintf(szVal, " <%ld", lMemberCountMax);
			lstrcat(szQuery, szVal);
		}
		if (lMemberCountMin >= 0L)
		{
			wsprintf(szVal, " >%ld", lMemberCountMin);
			lstrcat(szQuery, szVal);
		}
		if (lChannelAgeMax >= 0L)
		{
			wsprintf(szVal, " C<%ld", lChannelAgeMax);
			lstrcat(szQuery, szVal);
		}
		if (lChannelAgeMin >= 0L)
		{
			wsprintf(szVal, " C>%ld", lChannelAgeMin);
			lstrcat(szQuery, szVal);
		}
		if (lTopicAgeMax >= 0L)
		{
			wsprintf(szVal, " T<%ld", lTopicAgeMax);
			lstrcat(szQuery, szVal);
		}
		if (lTopicAgeMin >= 0L)
		{
			wsprintf(szVal, " T>%ld", lTopicAgeMin);
			lstrcat(szQuery, szVal);
		}
		if (bRegistered)
			lstrcat(szQuery, " R=1");
		if (bUnregistered)
			lstrcat(szQuery, " R=0");
		if (lListCount >= 0L)
		{
			wsprintf(szVal, " %ld", lListCount);
			lstrcat(szQuery, szVal);
		}

		m_hrLastError = HrMakeListMsg(&pb, &cb, szQuery, TRUE);
	}
	else
		m_hrLastError = HrMakeListMsg(&pb, &cb, szName, FALSE);

	if (SUCCEEDED(m_hrLastError))
	{
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
		if (SUCCEEDED(m_hrLastError))
			m_lChannelListingInProgress++;
	}

exit:
	if (szName)
		delete [] szName;
	if (szTopic)
		delete [] szTopic;
	if (szQuery)
		delete [] szQuery;

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatSocket::bFillServerItems(CChatItems *pCItems)
{
	TCHAR	szSecurity[2*g_nMaxLengthSmall];
	BOOL	bAnonAllowed;

	OutputDebugThreadIdString("CChatSocket::bFillServerItems - Enter\n");

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bFillServerItems");

	// First set the AssociatedType of the ChatItems
	if (!pCItems->bSetAssociatedType(g_rgwszAssociatedTypes[atServer]))
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	// Server properties
	// pnServerName = "Name"
	// pnServerSecurityPackages = "SecurityPackages"
	// pnServerAnonymousAllowed = "AnonymousAllowed"
	// pnServerInfo = "Info"
	// pnServerIgnoredUsers = "IgnoredUsers"
	// pnServerChannelCount = "ChannelCount"
	// pnServerNetInvisibleCount = "NetInvisibleCount"
	// pnServerNetServerCount = "NetServerCount"
	// pnServerNetUserCount = "NetUserCount"
	// pnServerNodeServerCount = "NodeServerCount"
	// pnServerNodeUserCount = "NodeUserCount"
	// pnServerSysopCount = "SysopCount"
	// pnServerUnknownConnectionCount = "UnknownConnectionCount"
	// pnServerMaxMessageLength = "MaxMessageLength"
	
	// Set the UnknownConnectionCount property
	if (-1L != m_prServer.lUnknownConnectionCount)
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerUnknownConnectionCount], NULL, m_prServer.lUnknownConnectionCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the SysopCount property
	if (-1L != m_prServer.lSysopCount)
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerSysopCount], NULL, m_prServer.lSysopCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the NodeUserCount property
	if (-1L != m_prServer.lNodeUserCount)
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerNodeUserCount], NULL, m_prServer.lNodeUserCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the NodeServerCount property
	if (-1L != m_prServer.lNodeServerCount)
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerNodeServerCount], NULL, m_prServer.lNodeServerCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the NetUserCount property
	if (-1L != m_prServer.lNetUserCount)
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerNetUserCount], NULL, m_prServer.lNetUserCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the NetServerCount property
	if (-1L != m_prServer.lNetServerCount)
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerNetServerCount], NULL, m_prServer.lNetServerCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the NetInvisibleCount property
	if (-1L != m_prServer.lNetInvisibleCount)
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerNetInvisibleCount], NULL, m_prServer.lNetInvisibleCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the ChannelCount property
	if (-1L != m_prServer.lChannelCount)
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerChannelCount], NULL, m_prServer.lChannelCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Info property
	if (m_rgbServerPropLocal[pnServerInfo])
	{
		ASSERT(m_prServer.psaInfo, "m_prServer.psaInfo is NULL in CChatSocket::bFillServerItems");
		
		// The server info is local
		VARIANT	vItem;

		VariantInit(&vItem);

		vItem.vt = (VT_VARIANT+VT_ARRAY);
		vItem.parray = m_prServer.psaInfo;

		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerInfo], &vItem))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	// Set the IgnoredUsers property
	if (bBuildIgnoredUsersArray())
	{
		VARIANT	vItem;

		VariantInit(&vItem);

		if (m_prServer.psaIgnored)
		{
			vItem.vt = (VT_VARIANT+VT_ARRAY);
			vItem.parray = m_prServer.psaIgnored;
		}

		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerIgnoredUsers], &vItem))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}
	else
		return FALSE;

	if (bGetSecurityPackages(szSecurity, 2*g_nMaxLengthSmall, &bAnonAllowed))
	{
		// Set the AnonymousAllowed property
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerAnonymousAllowed], bAnonAllowed))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

		// Set the SecurityPackages property
		if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerSecurityPackages], szSecurity, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	// Set the MaxMessageLength property
	if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerMaxMessageLength], NULL, g_lMaxMsgLength))
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	// Set the Name property
	if (!pCItems->bSetItem(g_rgwszServerPropertyNames[pnServerName], m_szServerName, 0L))
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	return TRUE;
}


BOOL CChatSocket::bFillChatItemsFromUser(CChatItems *pCItems, PPRUSER pPrUser)
{
	BOOL bRet;

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bFillChatItemsFromUser");
	ASSERT(pPrUser, "pPrUser is NULL in CChatSocket::bFillChatItemsFromUser");

	// First set the AssociatedType of the ChatItems
	if (!pCItems->bSetAssociatedType(g_rgwszAssociatedTypes[atUser]))
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	// User structure
	//LPTSTR		szNickname;		// Justin, can be UTF8 (like Régis) and needs a conversion to Unicode
	//LPTSTR		szRealName;		// Regis Brid de Schmolblutz
	//LPTSTR		szUserName;		// regisb
	//LPTSTR		szIPAddress;	// 157.55.65.36	(also called host)
	//LPTSTR		szServerName;	// KEEZER
	//LPTSTR		szServerInfo;	// Microsoft Internet Chat server 2.0
	//LPTSTR		szAway;			// Out with the dog
	//SAFEARRAY		*psaChannels;	// @#Channel1, +#ModeratedChannel2, #Newbies
	//LONG			lChannelCount;	// 3
	//LONG			lModes;			// Authenticated or not?
	//LONG			lIdleTime;		// 56 seconds
	//LONG			lSignOnTime;	// 89557456 seconds

	// Set the SignOnTime property
	if (-1L != pPrUser->lSignOnTime)
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserSignOnTime], NULL, pPrUser->lSignOnTime))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the IdleTime property
	if (-1L != pPrUser->lIdleTime)
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserIdleTime], NULL, pPrUser->lIdleTime))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Modes property
	if (umNone != pPrUser->lModes)
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserModes], NULL, pPrUser->lModes))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Channels property
	if (pPrUser->lChannelCount >= 0L)
	{
		VARIANT	vItem;

		VariantInit(&vItem);

		if (pPrUser->lChannelCount > 0L)
		{
			ASSERT(pPrUser->psaChannels, "pPrUser->psaChannels is NULL in CChatSocket::bFillChatItemsFromUser");
			vItem.vt = (VT_VARIANT+VT_ARRAY);
			vItem.parray = pPrUser->psaChannels;
		}

		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserChannels], &vItem))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	// Set the OwnerOfChannels property
	if (pPrUser->lChannelOwnerCount >= 0L)
	{
		VARIANT	vItem;

		VariantInit(&vItem);

		if (pPrUser->lChannelOwnerCount > 0L)
		{
			ASSERT(pPrUser->psaChannelsOwner, "pPrUser->psaChannelsOwner is NULL in CChatSocket::bFillChatItemsFromUser");
			vItem.vt = (VT_VARIANT+VT_ARRAY);
			vItem.parray = pPrUser->psaChannelsOwner;
		}

		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserOwnerOfChannels], &vItem))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	// Set the HostInChannels property
	if (pPrUser->lChannelHostCount >= 0L)
	{
		VARIANT	vItem;

		VariantInit(&vItem);

		if (pPrUser->lChannelHostCount > 0L)
		{
			ASSERT(pPrUser->psaChannelsHost, "pPrUser->psaChannelsHost is NULL in CChatSocket::bFillChatItemsFromUser");
			vItem.vt = (VT_VARIANT+VT_ARRAY);
			vItem.parray = pPrUser->psaChannelsHost;
		}

		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserHostInChannels], &vItem))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	// Set the VoiceInModeratedChannels property
	if (pPrUser->lChannelVoiceCount >= 0L)
	{
		VARIANT	vItem;

		VariantInit(&vItem);

		if (pPrUser->lChannelVoiceCount > 0L)
		{
			ASSERT(pPrUser->psaChannelsVoice, "pPrUser->psaChannelsVoice is NULL in CChatSocket::bFillChatItemsFromUser");
			vItem.vt = (VT_VARIANT+VT_ARRAY);
			vItem.parray = pPrUser->psaChannelsVoice;
		}

		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserVoiceInChannels], &vItem))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	// Set the Away property
	if (pPrUser->szAway)
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserAway], pPrUser->szAway, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the ServerInfo property
	if (pPrUser->szServerInfo)
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserServerInfo], pPrUser->szServerInfo, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the ServerName property
	if (pPrUser->szServerName)
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserServerName], pPrUser->szServerName, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the UserName, IPAddress and Identity properties
	if (pPrUser->szIPAddress)
	{
		ASSERT(pPrUser->szUserName, "pPrUser->szUserName is NULL in CChatSocket::bFillChatItemsFromUser");
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserIPAddress], pPrUser->szIPAddress, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserUserName], pPrUser->szUserName, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
		LPTSTR szIdentity = SzIdentityFromUserNameAndIPAddress(pPrUser->szUserName, pPrUser->szIPAddress);
		if (!szIdentity)
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserIdentity], szIdentity, 0L))
		{
			delete [] szIdentity;
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
		delete [] szIdentity;
	}

	// Set the RealName property
	if (pPrUser->szRealName)
		if (!pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserRealName], pPrUser->szRealName, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Nickname property
	if (g_chExtNckPfx == pPrUser->szNickname[0])
	{
		LPWSTR	wszWideNick;
		VARIANT	vItem;

		VariantInit(&vItem);
		vItem.vt = VT_BSTR;
		if (!bConvertUTF8StringToWide(pPrUser->szNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/) ||
			!(vItem.bstrVal = SysAllocString(wszWideNick)))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		delete [] wszWideNick;

		bRet = pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserNickname], &vItem);
		VariantClear(&vItem);
	}
	else
		bRet = pCItems->bSetItem(g_rgwszUserPropertyNames[pnUserNickname], pPrUser->szNickname, 0L);

	if (!bRet)
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	return TRUE;
}


BOOL CChatSocket::bFillChatItemsFromChannel(CChatItems *pCItems, PPRCHANNEL pPrChannel)
{
	LPTSTR	szTmp = NULL;
	BOOL	bFreeConverted = FALSE;

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bFillChatItemsFromChannel");
	ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bFillChatItemsFromChannel");

	// First set the AssociatedType of the ChatItems
	if (!pCItems->bSetAssociatedType(g_rgwszAssociatedTypes[atChannel]))
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	// Channel structure				Uknown or empty property when equal to
	//LPTSTR		szName				
	//LPTSTR		szTopic				NULL
	//LPTSTR		szKeyword			NULL or g_chEOS
	//LPTSTR		szHostKey			NULL or g_chEOS
	//LPTSTR		szOwnerKey			NULL or g_chEOS
	//LPTSTR		szRating			NULL or g_chEOS
	//LPTSTR		szClientData		NULL or g_chEOS
	//LPTSTR		szClientGuid		NULL or g_chEOS
	//LPTSTR		szLanguage			NULL or g_chEOS
	//LPTSTR		szOnJoin			NULL or g_chEOS
	//LPTSTR		szOnPart			NULL or g_chEOS
	//LPTSTR		szSubject			NULL or g_chEOS
	//LPTSTR		szServicePath		NULL or g_chEOS
	//LPTSTR		szAccount			NULL or g_chEOS
	//SAFEARRAY		*psaBannedList		lBannedListCount == 0L
	//SAFEARRAY		*psaCloneList		lCloneListCount == 0L
	//LONG			lMemberCount		< 0L
	//LONG			lMaxMemberCount		< 0L
	//LONG			lModes				cmNone
	//LONG			lOID				<= 0L
	//LONG			lLag				< 0L
	//LONG			lCreationTime		<= 0L


	// Set the ObjectId property
	if (pPrChannel->lOID > 0L)
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnObjectId], NULL, pPrChannel->lOID))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Lag property
	if (pPrChannel->lLag >= 0L)
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnLag], NULL, pPrChannel->lLag))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the CreationTime property
	if (pPrChannel->lCreationTime > 0L)
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnCreationTime], NULL, pPrChannel->lCreationTime))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Modes property
	if (pPrChannel->lModes != cmNone)
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnModes], NULL, pPrChannel->lModes))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the MaxMemberCount property
	if (pPrChannel->lMaxMemberCount >= 0L)
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnMaxMemberCount], NULL, pPrChannel->lMaxMemberCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the MemberCount property
	if (pPrChannel->lMemberCount >= 0L)
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnMemberCount], NULL, pPrChannel->lMemberCount))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the BannedList property
	if (pPrChannel->lBannedListCount > 0L)
	{	
		VARIANT	vItem;

		VariantInit(&vItem);

		ASSERT(pPrChannel->psaBannedList, "pPrChannel->psaBannedList is NULL in CChatSocket::bFillChatItemsFromChannel");
		vItem.vt = (VT_VARIANT+VT_ARRAY);
		vItem.parray = pPrChannel->psaBannedList;

		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnBannedList], &vItem))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	// Set the CloneList property
	if (pPrChannel->lCloneListCount > 0L)
	{	
		VARIANT	vItem;

		VariantInit(&vItem);

		ASSERT(pPrChannel->psaCloneList, "pPrChannel->psaCloneList is NULL in CChatSocket::bFillChatItemsFromChannel");
		vItem.vt = (VT_VARIANT+VT_ARRAY);
		vItem.parray = pPrChannel->psaCloneList;

		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnCloneList], &vItem))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	// Set the ClientData property
	if (pPrChannel->szClientData && g_chEOS != pPrChannel->szClientData[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnClientData], pPrChannel->szClientData, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the ClientGuid property
	if (pPrChannel->szClientGuid && g_chEOS != pPrChannel->szClientGuid[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnClientGuid], pPrChannel->szClientGuid, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Language property
	if (pPrChannel->szLanguage && g_chEOS != pPrChannel->szLanguage[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnLanguage], pPrChannel->szLanguage, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the OnJoin property
	if (pPrChannel->szOnJoin && g_chEOS != pPrChannel->szOnJoin[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnOnJoin], pPrChannel->szOnJoin, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the OnPart property
	if (pPrChannel->szOnPart && g_chEOS != pPrChannel->szOnPart[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnOnPart], pPrChannel->szOnPart, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Subject property
	if (pPrChannel->szSubject && g_chEOS != pPrChannel->szSubject[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnSubject], pPrChannel->szSubject, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the ServicePath property
	if (pPrChannel->szServicePath && g_chEOS != pPrChannel->szServicePath[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnServicePath], pPrChannel->szServicePath, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Account property
	if (pPrChannel->szAccount && g_chEOS != pPrChannel->szAccount[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnAccount], pPrChannel->szAccount, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Rating property
	if (pPrChannel->szRating && g_chEOS != pPrChannel->szRating[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnRating], pPrChannel->szRating, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the OwnerKey property
	if (pPrChannel->szOwnerKey && g_chEOS != pPrChannel->szOwnerKey[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnOwnerKey], pPrChannel->szOwnerKey, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the HostKey property
	if (pPrChannel->szHostKey && g_chEOS != pPrChannel->szHostKey[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnHostKey], pPrChannel->szHostKey, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Keyword property
	if (pPrChannel->szKeyword && g_chEOS != pPrChannel->szKeyword[0])
		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnKeyword], pPrChannel->szKeyword, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	USES_CONVERSION;

	// Set the ChannelTopic property
	if (pPrChannel->szTopic)
	{
		if (pPrChannel->szTopic[0] && g_chExtChnPfx == pPrChannel->szName[0] && !pPrChannel->bMicOnly)
		{
			// Need to convert UTF8 extended channel topic to Unicode
			LPWSTR wszChannelTopic;
			if (!bConvertUTF8StringToWide(pPrChannel->szTopic, 0 /*cchIn*/, &wszChannelTopic, NULL /*pcchOut*/, FALSE /*bNickname*/, FALSE /*bChannelName*/, FALSE /*bPostProcess*/))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			ASSERT(wszChannelTopic, "wszChannelTopic in NULL in CChatSocket::bFillChatItemsFromChannel");

			szTmp = W2T(wszChannelTopic);
			delete [] wszChannelTopic;
		}
		else
		{
			// Might have to convert topic from Jis to ShiftJis, etc...
			if (!bConvertString(TRUE, m_byteCharSet, pPrChannel->szTopic, 0, &szTmp, NULL, &bFreeConverted))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
		}

		if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnTopic], szTmp, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
	}

	if (bFreeConverted && szTmp)
	{
		delete [] szTmp;
		bFreeConverted = FALSE;
	}

	// Set the ChannelName property
	if (bIsIrcXSocket())
	{
		if (g_chExtChnPfx == pPrChannel->szName[0])
		{
			// Need to convert UTF8 extended channel name to Unicode
			LPWSTR wszChannelName;
			if (!bConvertUTF8StringToWide(pPrChannel->szName, 0 /*cchIn*/, &wszChannelName, NULL /*pcchOut*/, FALSE, TRUE))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			ASSERT(wszChannelName, "wszChannelName in NULL in CChatSocket::bFillChatItemsFromChannel");

			szTmp = W2T(wszChannelName);
			delete [] wszChannelName;
		}
		else
		{
			// Might have to convert channel name from Jis to ShiftJis, etc...
			if (!bConvertString(TRUE, m_byteCharSet, pPrChannel->szName, 0, &szTmp, NULL, &bFreeConverted))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
		}
	}
	else
		szTmp = pPrChannel->szName;

	if (!pCItems->bSetItem(g_rgwszChannelPropertyNames[cpnName], szTmp, 0L))
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	if (bFreeConverted && szTmp)
		delete [] szTmp;

	return TRUE;
}


BOOL CChatSocket::bFillChatItemsFromMember(CChatItems *pCItems, PPRMEMBER pPrMember)
{
	BOOL bRet;

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bFillChatItemsFromMember");
	ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bFillChatItemsFromMember");

	// First set the AssociatedType of the ChatItems
	if (!pCItems->bSetAssociatedType(g_rgwszAssociatedTypes[atMember]))
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	// Member structure
	//LPTSTR	szNickname;		// Justin
	//LPTSTR	szUserName;		// regisb
	//LPTSTR	szIPAddress;	// 157.55.65.36
	//LONG		lModes;
	//LONG		lOID;

	// Set the ObjectID property
	if (pPrMember->lOID)
		if (!pCItems->bSetItem(g_rgwszMemberPropertyNames[pnMemberObjectId], NULL, (LONG) pPrMember->lOID))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}

	// Set the Modes property
	if (pPrMember->lModes)
	{
		if (!pCItems->bSetItem(g_rgwszMemberPropertyNames[pnMemberModes], NULL, (LONG) pPrMember->lModes))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
		// The Away and Sysop flags are reset once we exposed them - this is just a snapshot!
		pPrMember->lModes &= ~mmSysop;
		pPrMember->lModes &= ~mmNotSysop;
		pPrMember->lModes &= ~mmAway;
		pPrMember->lModes &= ~mmNotAway;
	}

	// Set the UserName, IPAddress and Identity properties
	if (pPrMember->szIPAddress)
	{
		ASSERT(pPrMember->szUserName, "pPrMember->szUserName is NULL in CChatSocket::bFillChatItemsFromMember");
		if (!pCItems->bSetItem(g_rgwszMemberPropertyNames[pnMemberIPAddress], pPrMember->szIPAddress, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
		if (!pCItems->bSetItem(g_rgwszMemberPropertyNames[pnMemberUserName], pPrMember->szUserName, 0L))
		{
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
		LPTSTR szIdentity = SzIdentityFromUserNameAndIPAddress(pPrMember->szUserName, pPrMember->szIPAddress);
		if (!szIdentity)
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		if (!pCItems->bSetItem(g_rgwszMemberPropertyNames[pnMemberIdentity], szIdentity, 0L))
		{
			delete [] szIdentity;
			m_hrLastError = pCItems->HrGetLastError();
			return FALSE;
		}
		delete [] szIdentity;
	}

	// Set the Nickname property
	if (g_chExtNckPfx == pPrMember->szNickname[0])
	{
		LPWSTR	wszWideNick;
		VARIANT	vItem;

		VariantInit(&vItem);
		vItem.vt = VT_BSTR;
		if (!bConvertUTF8StringToWide(pPrMember->szNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/) ||
			!(vItem.bstrVal = SysAllocString(wszWideNick)))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		delete [] wszWideNick;

		bRet = pCItems->bSetItem(g_rgwszMemberPropertyNames[pnMemberNickname], &vItem);
		VariantClear(&vItem);
	}
	else
		bRet = pCItems->bSetItem(g_rgwszMemberPropertyNames[pnMemberNickname], pPrMember->szNickname, 0L);

	if (!bRet)
	{
		m_hrLastError = pCItems->HrGetLastError();
		return FALSE;
	}

	return TRUE;
}


BOOL CChatSocket::bLoadComicStrings(HINSTANCE hInst)
{
	for (WORD wIDS = IDS_COMICSTART; wIDS <= IDS_COMICEND; wIDS++)
		if (!::LoadString(hInst, wIDS, m_rgszComic[wIDS-IDS_COMICSTART], g_nMaxLengthSmall))
		{
			m_hrLastError = GetLastError();
			return FALSE;
		}
	return TRUE;
}


//=--------------------------------------------------------------------------=
// BOOL CChatSocket::bFilterTextMessage
//=--------------------------------------------------------------------------=
// filters the incoming text messages - 
//
// Parameters:
//    LPTSTR		- [in] the text message
//    enumMsgType*	- [out]	the type of text message
//    BOOL*			- [out] is the message comic chat specific?
//    LPTSTR*		- [out] where does the message body start?
//
// Output:
//    HRESULT		NOERROR if function was successful
//
// Notes:
//	1) "# Appears as <anystringhere>." should be entirely removed (via send, whisper, or private message).
//	2) "# GetInfo" should be removed (via whisper or private message).
//	3) "# Background <anystringhere>", should be removed (via send, whisper, or private message)
//	4) "(#<anything>) Chr" should be entirely removed (via send, whisper, or private message)
//	5) "(#<anything>) <message>" should be printed as <username> say: <message>.
//	6) "\0x01ACTION <message>\0x01" gets printed out as <username> does: <message>
//  7) "\0x01VERSION\0x01" 
//  8) "\0x01PING <time>\0x01"
//  9) "\0x01TIME\0x01"
// 10) "\0x01SOUND <sound file path> \0x01
// 11) "\0x01.....\0x01	==> CTCP message
// 11) Thoughts are of the form (#*M3*) <message>. Should be printed out as <username> thinks: <message>.
// 12) Comic action of the form (#*M5*) <message>. Don't need to prepend the username, since it's already in the message.  
//												   Just print <message> out as an action.
//
BOOL CChatSocket::bFilterTextMessage(LPTSTR szTextMessage, LONG pmt, enumMsgType *pMT, BOOL *pbComic, LPTSTR *pszMessageBody)
{
	LPTSTR	szTmp1 = NULL;
	LPTSTR	szTmp2 = NULL;

	ASSERT(szTextMessage, "szTextMessage is NULL in CChatSocket::bFilterTextMessage");
	ASSERT(pszMessageBody, "pszMessageBody is NULL in CChatSocket::bFilterTextMessage");
	ASSERT(pMT, "pMT is NULL in CChatSocket::bFilterTextMessage");
	ASSERT(pbComic, "pbComic is NULL in CChatSocket::bFilterTextMessage");

	*pszMessageBody = NULL;
	*pbComic = FALSE;	// by default

	if (SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_APPEARSAS-IDS_COMICSTART]) == szTextMessage)
	{
		*pMT = mtAppearsAs;
		*pbComic = TRUE;
		return TRUE;
	}

	if (SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_GETINFO-IDS_COMICSTART]) == szTextMessage)
	{
		*pMT = mtGetInfo;
		*pbComic = TRUE;
		return TRUE;
	}

	if (SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_BACKGROUND-IDS_COMICSTART]) == szTextMessage)
	{
		*pMT = mtBackground;
		*pbComic = TRUE;
		return TRUE;
	}

	if (SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_CCPREFIX-IDS_COMICSTART]) == szTextMessage)
	{
		szTmp1 = szTextMessage + lstrlen(szTextMessage) - lstrlen(m_rgszComic[IDS_CCSUFFIX1-IDS_COMICSTART]);
		if (SzSubStr(szTmp1, (LPCTSTR) m_rgszComic[IDS_CCSUFFIX1-IDS_COMICSTART]) == szTmp1)
		{
			*pMT = mtChr;
			*pbComic = TRUE;
			return TRUE;
		}

		if (szTmp1 = SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_M3-IDS_COMICSTART]))
		{
			if (szTmp2 = SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_CCSUFFIX2-IDS_COMICSTART]))
			{
				if (szTmp1 < szTmp2)
				{
					*pszMessageBody = szTmp2 + lstrlen(m_rgszComic[IDS_CCSUFFIX2-IDS_COMICSTART]);
					*pMT = mtThought;
					*pbComic = TRUE;
					return TRUE;
				}
			}
		}

		if (szTmp1 = SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_M5-IDS_COMICSTART]))
		{
			if (szTmp2 = SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_CCSUFFIX2-IDS_COMICSTART]))
			{
				if (szTmp1 < szTmp2)
				{
					*pszMessageBody = szTmp2 + lstrlen(m_rgszComic[IDS_CCSUFFIX2-IDS_COMICSTART]);
					*pMT = mtAction;
					*pbComic = TRUE;
					return TRUE;
				}
			}
		}

		if (szTmp1 = SzSubStr(szTextMessage, (LPCTSTR) m_rgszComic[IDS_CCSUFFIX2-IDS_COMICSTART]))
		{
			*pszMessageBody = szTmp1 + lstrlen(m_rgszComic[IDS_CCSUFFIX2-IDS_COMICSTART]);
			*pMT = mtNormal;
			*pbComic = TRUE;
			return TRUE;
		}
	}

	if (*szTextMessage == g_byteCTCPBrace && szTextMessage+1 && 
		*(szTextMessage + lstrlen(szTextMessage) - 1) == g_byteCTCPBrace)
	{
		LPTSTR	szVerb;
		INT		cbVerb;

		*(szTextMessage + lstrlen(szTextMessage) - 1) = g_chEOS;	// get rid of the trailing 0x01

		if (SzSubStr(szTextMessage+1, (LPCTSTR) m_rgszComic[IDS_ACTION-IDS_COMICSTART]) == szTextMessage+1)
		{
			cbVerb = lstrlen(m_rgszComic[IDS_ACTION-IDS_COMICSTART]);
			if (*(szTextMessage + cbVerb + 1) == g_chSpace)
			{
				*pszMessageBody = szTextMessage + cbVerb + 2;
				*pMT = mtAction;
				return TRUE;
			}
			else
				goto CTCP;
		}

		if (SzSubStr(szTextMessage+1, (LPCTSTR) m_rgszComic[IDS_SOUND-IDS_COMICSTART]) == szTextMessage+1)
		{
			cbVerb = lstrlen(m_rgszComic[IDS_SOUND-IDS_COMICSTART]);
			if (*(szTextMessage + cbVerb + 1) == g_chSpace)
			{
				*pszMessageBody = szTextMessage + cbVerb + 2;
				*pMT = mtSound;
				return TRUE;
			}
			else
				goto CTCP;
		}

		szVerb = SzSubStr(szTextMessage+1, (LPCTSTR) m_rgszComic[IDS_LAGTIME-IDS_COMICSTART]);
		if (szVerb == szTextMessage+1 || 
			(szVerb == szTextMessage+2 && *(szTextMessage+1) == '*'))
		{
			cbVerb = lstrlen(m_rgszComic[IDS_LAGTIME-IDS_COMICSTART]);
			if (*(szVerb + cbVerb) == g_chSpace || *(szVerb + cbVerb) == g_chEOS)
			{
				*pszMessageBody = szVerb + cbVerb + 1;
				*pMT = ((szVerb == szTextMessage+1) && !(pmt & pmtNotice)) ? mtLagTimeRequest : mtLagTimeReply;
				if (*(szTextMessage + lstrlen(szTextMessage) - 1) == g_byteCTCPBrace)
					*(szTextMessage + lstrlen(szTextMessage) - 1) = g_chEOS;	// get rid of the buggy trailing 0x01 from MsChat2.0
				return TRUE;
			}
			else
				goto CTCP;
		}

		szVerb = SzSubStr(szTextMessage+1, (LPCTSTR) m_rgszComic[IDS_LOCALTIME-IDS_COMICSTART]);
		if (szVerb == szTextMessage+1 ||
			(szVerb == szTextMessage+2 && *(szTextMessage+1) == '*'))
		{
			cbVerb = lstrlen(m_rgszComic[IDS_LOCALTIME-IDS_COMICSTART]);
			if (*(szVerb + cbVerb) == g_chSpace || *(szVerb + cbVerb) == g_chEOS)
			{
				*pszMessageBody = szVerb + cbVerb + 1;
				*pMT = ((szVerb == szTextMessage+1) && !(pmt & pmtNotice)) ? mtLocalTimeRequest : mtLocalTimeReply;
				return TRUE;
			}
			else
				goto CTCP;
		}

		szVerb = SzSubStr(szTextMessage+1, (LPCTSTR) m_rgszComic[IDS_VERSION-IDS_COMICSTART]);
		if (szVerb == szTextMessage+1 || 
			(szVerb == szTextMessage+2 && *(szTextMessage+1) == '*'))
		{
			cbVerb = lstrlen(m_rgszComic[IDS_VERSION-IDS_COMICSTART]);
			if (*(szVerb + cbVerb) == g_chSpace || *(szVerb + cbVerb) == g_chEOS)
			{
				*pszMessageBody = szVerb + cbVerb + 1;
				*pMT = mtVersion;
				return TRUE;
			}
			else
				goto CTCP;
		}

CTCP:
		// In all the other cases, we just say it's a CTCP message
		*pszMessageBody = szTextMessage + 1;
		*pMT = mtCTCP;
		return TRUE;
	}

	*pszMessageBody = szTextMessage;
	*pMT = mtNormal;
	return TRUE;
}


BOOL CChatSocket::bNotifyUserProperty(LPCTSTR szMemberInfo)
{
	LPTSTR	szNickname = (LPTSTR) szMemberInfo;
	PRUSER	prUser;
	BOOL	bRet;

	ASSERT(szMemberInfo, "szMemberInfo is NULL in CChatSocket::bNotifyUserProperty");
	
	if ((szNickname[0] == '.') || (szNickname[0] == '@') || (szNickname[0] == '+'))
		szNickname++;

	CChatItems* pCItems = NULL;
	
	if (!bCreateChatItems(&pCItems))
		return FALSE;

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bNotifyUserProperty");

	ZeroMemory(&prUser, sizeof(PRUSER));

	if (!bCopyStr(&(prUser.szNickname), szNickname))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}
	prUser.lIdleTime = -1L;
	prUser.lSignOnTime = -1L;
	prUser.lChannelCount = -1L;
	prUser.lChannelOwnerCount = -1L;
	prUser.lChannelHostCount = -1L;
	prUser.lChannelVoiceCount = -1L;

	bRet = bFillChatItemsFromUser(pCItems, &prUser);

	FreePrUserContent(&prUser);

	if (!bRet)
		goto error;

	FireUserProperty(pCItems);

	return TRUE;

error:
	ReleaseChatItems(pCItems);
	return FALSE;
}


BOOL CChatSocket::bNotifyMemberProperty(CChatChannel *pChannel, LPCTSTR szMemberInfo)
{
	PPRMEMBER	pPrMember;
	LPTSTR		szNickname = (LPTSTR) szMemberInfo;
	BOOL		bIsOwner = FALSE, bIsHost = FALSE, bHasVoice = FALSE;

	ASSERT(szMemberInfo, "szMemberInfo is NULL in CChatSocket::bNotifyMemberProperty");
	ASSERT(pChannel, "pChannel is NULL in CChatSocket::bNotifyMemberProperty");

	if ((bIsOwner = (szNickname[0] == '.')) || (bIsHost = (szNickname[0] == '@')) || (bHasVoice = (szNickname[0] == '+')))
		szNickname++;

	if (!pChannel->bGetMemberFromNickname(szNickname, &pPrMember))
	{
		ASSERT(FALSE, "Couldn't find member in channel in CChatSocket::bNotifyMemberProperty");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}
	ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bNotifyMemberProperty");

	if (bIsOwner)
	{
		pPrMember->lModes &= ~mmNotOwner;
		pPrMember->lModes |= mmOwner;
	}
	// REGISB: no else here?
	if (bIsHost)
	{
		pPrMember->lModes &= ~mmNotHost;
		pPrMember->lModes |= mmHost;
	}
	// REGISB: no else here?
	if (bHasVoice)
	{
		pPrMember->lModes &= ~mmNoVoice;
		pPrMember->lModes |= mmVoice;
	}
	else
	{
		pPrMember->lModes &= ~mmVoice;
		pPrMember->lModes |= mmNoVoice;
	}

	ASSERT(bIsMemberModesValid(pPrMember->lModes), "Invalid member modes in CChatSocket::bNotifyMemberProperty");

	CChatItems* pCItems = NULL;
	
	if (!bCreateChatItems(&pCItems))
		return FALSE;

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bNotifyMemberProperty");

	if (!bFillChatItemsFromMember(pCItems, pPrMember))
		return FALSE;

	FireMemberProperty(pChannel, pCItems);

	return TRUE;
}


BOOL CChatSocket::bHandleWho(PPRIRCMSG pPrIrcMsg, UINT uCode)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleWho");
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleWho");
	ASSERT(pPrIrcMsg->byteParams > 2, "pPrIrcMsg->byteParams <= 2 in CChatSocket::bHandleWho");

	// pPrIrcMsg->szParams[0] is the caller's nickname
	ASSERT(0 == lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname), "pPrIrcMsg->szParams[0] != m_szNickname in CChatSocket::bHandleWho");

	if (m_lWhoListingInProgress <= 0L)
	{
		// We don't expect a WHO reply
		m_hrLastError = CC_E_NOTQUERIEDOBJECT;
		return FALSE;
	}
	
	if (RPL_ENDOFWHO == uCode)
	{
		m_lWhoListingInProgress--;
		ASSERT(m_lWhoListingInProgress >= 0L, "m_lWhoListingInProgress < 0L in CChatSocket::bHandleWho");
		FireEndEnumeration(NULL, etUsers);
		return FALSE;
	}

	// This is a RPL_WHOREPLY case
	ASSERT(RPL_WHOREPLY == uCode, "RPL_WHOREPLY != uCode in CChatSocket::bHandleWho");

	// pPrIrcMsg->szParams[1] == <ChannelName>
	// pPrIrcMsg->szParams[2] == <UserName>
	// pPrIrcMsg->szParams[3] == <IPAddress>
	// pPrIrcMsg->szParams[4] == <ServerName>
	// pPrIrcMsg->szParams[5] == <Nickname>
	// pPrIrcMsg->szParams[6] == <H|G>[*|a][.|@|+][x]
	// pPrIrcMsg->szParams[7] == <HopCount> <RealName>

	if (pPrIrcMsg->byteParams < 8)
	{
		// We expect more parameters
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	PPRUSER	pPrUser = &m_prUser;
	LPTSTR			szTmp = pPrIrcMsg->szParams[6];
	SAFEARRAY		*psa;
	SAFEARRAYBOUND	sabound;
	VARIANT			vItem;
	CHAR			*sz, *szRealName;
	INT				cch;

	bCopyStr(&(pPrUser->szUserName), pPrIrcMsg->szParams[2]);
	bCopyStr(&(pPrUser->szIPAddress), pPrIrcMsg->szParams[3]);
	bCopyStr(&(pPrUser->szServerName), pPrIrcMsg->szParams[4]);
	bCopyStr(&(pPrUser->szNickname), pPrIrcMsg->szParams[5]);
	pPrUser->lIdleTime			= -1L;
	pPrUser->lSignOnTime		= -1L;
	pPrUser->lModes				= umNotSysop|umNotAdmin;
	pPrUser->lChannelCount		= 0;
	pPrUser->lChannelOwnerCount	= 0;
	pPrUser->lChannelHostCount	= 0;
	pPrUser->lChannelVoiceCount = 0;
	ASSERT(pPrUser->psaChannels, "pPrUser->psaChannels is NULL in CChatSocket::bHandleWho");
	ASSERT(pPrUser->psaChannelsOwner, "pPrUser->psaChannelsOwner is NULL in CChatSocket::bHandleWho");
	ASSERT(pPrUser->psaChannelsHost, "pPrUser->psaChannelsHost is NULL in CChatSocket::bHandleWho");
	ASSERT(pPrUser->psaChannelsVoice, "pPrUser->psaChannelsVoice is NULL in CChatSocket::bHandleWho");
	if (pPrUser->szRealName)
	{
		delete [] pPrUser->szRealName;
		pPrUser->szRealName = NULL;
	}
	if (pPrUser->szServerInfo)
	{
		delete [] pPrUser->szServerInfo;
		pPrUser->szServerInfo = NULL;
	}
	if (pPrUser->szAway)
	{
		delete [] pPrUser->szAway;
		pPrUser->szAway = NULL;
	}

	VariantInit(&vItem);
	sabound.cElements = 1L;
	sabound.lLbound = 0L;

	USES_CONVERSION;

	if ('*' != pPrIrcMsg->szParams[1][0])
	{
		// The user is at least in one channel
		if (bIsIrcXSocket())
		{
			if (g_chExtChnPfx == pPrIrcMsg->szParams[1][0])
			{
				// Need to convert UTF8 extended channel name to Unicode
				LPWSTR wszChannelName;
				if (!bConvertUTF8StringToWide(pPrIrcMsg->szParams[1], 0 /*cchIn*/, &wszChannelName, NULL /*pcchOut*/, FALSE, TRUE))
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}
				ASSERT(wszChannelName, "wszChannelName in NULL in CChatSocket::bHandleWho");

				vItem.bstrVal = SysAllocString(wszChannelName);
				delete [] wszChannelName;
			}
			else
			{
				LPTSTR	szTmpCN;
				BOOL	bFreeConverted;

				// Might have to convert from Jis to ShiftJis, etc...
				if (!bConvertString(TRUE, m_byteCharSet, pPrIrcMsg->szParams[1], 0, &szTmpCN, NULL, &bFreeConverted))
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}
				ASSERT(szTmpCN, "szTmpCN is NULL in CChatSocket::bHandleWho");
				vItem.bstrVal = A2BSTR(szTmpCN);
				if (bFreeConverted)
					delete [] szTmpCN;
			}
		}
		else
			vItem.bstrVal = A2BSTR(pPrIrcMsg->szParams[1]);

		if (!vItem.bstrVal)
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		vItem.vt = VT_BSTR;

		if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannels, &sabound)))
			goto exit;

		if (FAILED(m_hrLastError = SafeArrayPutElement(pPrUser->psaChannels, &(sabound.lLbound), (PVOID) &vItem)))
			goto exit;

		pPrUser->lChannelCount = 1L;
	}

	while (g_chEOS != *szTmp)
	{
		switch (*szTmp)
		{
		case 'H':
			pPrUser->lModes |= umNotAway;
			break;

		case 'G':
			pPrUser->lModes |= umAway;
			break;

		case '*':
			pPrUser->lModes |= umSysop;
			pPrUser->lModes &= ~umNotSysop;
			break;

		case 'a':
			pPrUser->lModes |= umAdmin;
			pPrUser->lModes &= ~umNotAdmin;
			break;

		case 'x':
			// User is in IRCX mode, we don't expose this.
			break;

		case '.':
		case '@':
		case '+':
			ASSERT(vItem.bstrVal, "vItem.bstrVal is NULL in CChatSocket::bHandleWho");
			if ('.' == *szTmp)
				psa = pPrUser->psaChannelsOwner;
			else if ('@' == *szTmp)
				psa = pPrUser->psaChannelsHost;
			else
				psa = pPrUser->psaChannelsVoice;

			if (FAILED(m_hrLastError = SafeArrayRedim(psa, &sabound)))
				goto exit;

			if (FAILED(m_hrLastError = SafeArrayPutElement(psa, &(sabound.lLbound), (PVOID) &vItem)))
				goto exit;

			if ('.' == *szTmp)
				pPrUser->lChannelOwnerCount = 1L;
			else if ('@' == *szTmp)
				pPrUser->lChannelHostCount = 1L;
			else
				pPrUser->lChannelVoiceCount = 1L;
			break;

		default:
			ASSERT(FALSE, "Unexpected character in pPrIrcMsg->szParams[6] of RPL_WHOREPLY in CChatSocket::bHandleWho");
			break;
		}
		szTmp++;
	}

	// We don't expose the Hopcount value to the caller.
	szRealName = pPrIrcMsg->szParams[7];
	bGetNextParam(&szRealName, &sz, &cch);
	if (szRealName)
		bCopyStr(&(pPrUser->szRealName), szRealName);

	ASSERT(bIsUserModesValid(pPrUser->lModes), "Invalid user modes in CChatSocket::bHandleWho");

exit:
	VariantClear(&vItem);

	if (SUCCEEDED(m_hrLastError))
		m_hrLastError = HrSendEvent(WM_U_USERPROP, (WPARAM) 0L, (LPARAM) pPrUser);
	return SUCCEEDED(m_hrLastError);
}


BOOL CChatSocket::bHandleWhoIs(PPRQUERY pPrQry, PPRIRCMSG pPrIrcMsg, UINT uCode)
{
	PPRQUERY			pPrQuery = pPrQry, pPrQueryU = NULL, pPrQueryUL = NULL;
	PPRUSER				pPrUser = NULL;
	enumAssociatedType	at;
	LONG				lPosU = 0L, lPosUL = 0L;

	static PPRQUERY		pPrLatestQuery = NULL;
	static PPRUSER		pPrLatestUser = NULL;
	static TCHAR		szLatestNickname[256] = "";
	static BOOL			bUserProp = TRUE;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleWhoIs");
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleWhoIs");

	// pPrQuery == NULL means asynchronous access

	// Three possible situations here:
	//	- the user did an asynchronous property access
	//	- the main thread is waiting for the answer during a synchronous access
	//	- this answer comes after a timeout and RPL_ENDOFWHOIS will fire the OnUserProperty event
	//  - the user is doing a user listing via the ListUsers method

	ASSERT(pPrIrcMsg->byteParams > 1, "pPrIrcMsg->byteParams <= 1 in CChatSocket::bHandleWhoIs");

	// pPrIrcMsg->szParams[0] is the caller's nickname
	ASSERT(0 == lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname), "pPrIrcMsg->szParams[0] != m_szNickname in CChatSocket::bHandleWhoIs");

	// pPrIrcMsg->szParams[1] is the nickname queried
	// Attention: this is wrong on MIC1.0 servers where pPrIrcMsg->szParams[1] is the caller's nickname!

	if (pPrQuery)
	{
		bUserProp = TRUE;
		pPrUser = (PPRUSER) pPrQuery->pvData;
		ASSERT(pPrUser, "pPrUser is NULL in CChatSocket::bHandleWhoIs");
		ASSERT(pPrUser->szNickname, "pPrUser->szNickname is NULL in CChatSocket::bHandleWhoIs");
		if (0 != lstrcmpi(pPrIrcMsg->szParams[1], pPrUser->szNickname))
		{
			// MIC 1.0 bug workaround
			if (bIsIrcXSocket() || 0 != lstrcmpi(szLatestNickname, pPrUser->szNickname))
			{
				m_hrLastError = CC_E_NOTQUERIEDOBJECT;
				return FALSE;
			}
			ASSERT(0 == lstrcmpi(pPrIrcMsg->szParams[0], pPrIrcMsg->szParams[1]), "Unexpected pPrIrcMsg->szParams[1] in CChatSocket::bHandleWhoIs");
		}
		// MIC 1.0 bug workaround
		else
			lstrcpy(szLatestNickname, pPrIrcMsg->szParams[1]);
	}
	else
	{
		// Has the nickname changed since the last WhoIs reply?
		if (0 == lstrcmp(pPrIrcMsg->szParams[1], szLatestNickname) || RPL_ENDOFWHOIS == uCode)
		{
			pPrQuery = pPrLatestQuery;
			pPrUser = pPrLatestUser;
		}
		else
		{
			// The user nickname has changed - we're dealing with a new guy!

			// We might need to expose the latest user property bag
			if (pPrLatestUser)
				// This has to be for a user listing
				bEndOfUser(pPrLatestQuery, pPrLatestUser, FALSE);

			// Update the latest nickname, query and user
			pPrLatestQuery = NULL;
			pPrLatestUser = NULL;

			if (m_lWhoIsInProgress > 0L)
			{
				// Might be an Asynchronous access - let's try to find the query cell in the list
				at = atUser;

				m_listQuery.bFindCellFromData(1L,
											  g_nSearchByAssociatedType,
											  (PVOID) &at,
											  g_nSearchByNickname,
											  pPrIrcMsg->szParams[1],
											  (PVOID*) &pPrQueryU,
											  &lPosU);
			}

			if (m_lWhoIsListingInProgress > 0L)
			{
				// Might be a user listing - let's try to find the query cell in the list
				at = atQuery;
				enumQueryType qt = qtUserList;

				m_listQuery.bFindCellFromData(1L,
											  g_nSearchByAssociatedType,
											  (PVOID) &at,
											  g_nSearchByQueryType,
											  (PVOID) &qt,
											  (PVOID*) &pPrQueryUL,
											  &lPosUL);
			}

			if (!pPrQueryU && !pPrQueryUL)
			{
				m_hrLastError = CC_E_NOTQUERIEDOBJECT;
				return FALSE;
			}

			if (pPrQueryU && (lPosU < lPosUL || 0L == lPosUL))
			{
				// User prop access case
				pPrUser = (PPRUSER) pPrQueryU->pvData;
				pPrQuery = pPrQueryU;
				bUserProp = TRUE;
			}
			else
			{
				ASSERT(pPrQueryUL, "pPrQueryUL is NULL in CChatSocket::bHandleWhoIs");
				// User listing case
				pPrUser = &m_prUser;
				pPrQuery = pPrQueryUL;
				bUserProp = FALSE;

				bCopyStr(&(pPrUser->szNickname), pPrIrcMsg->szParams[1]);
				pPrUser->lIdleTime			= -1L;
				pPrUser->lSignOnTime		= -1L;
				pPrUser->lModes				= (umNotSysop|umNotAdmin);	// By default a user is not an admin, nor a sysop
				pPrUser->lChannelCount		= 0L;
				pPrUser->lChannelOwnerCount	= 0L;
				pPrUser->lChannelHostCount	= 0L;
				pPrUser->lChannelVoiceCount = 0L;
				if (pPrUser->szRealName)
				{
					delete [] pPrUser->szRealName;
					pPrUser->szRealName = NULL;
				}
				if (pPrUser->szServerName)
				{
					delete [] pPrUser->szServerName;
					pPrUser->szServerName = NULL;
				}
				if (pPrUser->szServerInfo)
				{
					delete [] pPrUser->szServerInfo;
					pPrUser->szServerInfo = NULL;
				}
				if (pPrUser->szAway)
				{
					delete [] pPrUser->szAway;
					pPrUser->szAway = NULL;
				}
			}

			ASSERT(pPrUser, "pPrUser is NULL in CChatSocket::bHandleWhoIs");
			pPrLatestQuery = pPrQuery;
			pPrLatestUser = pPrUser;
			lstrcpy(szLatestNickname, pPrIrcMsg->szParams[1]);
		}

		if (!pPrUser)
			// This is for RPL_ENDOFWHOIS of a WhoIs that failed
			return TRUE;

		// We finally have our user structure
		ASSERT(pPrUser->szNickname, "pPrUser->szNickname is NULL in CChatSocket::bHandleWhoIs");
	}

	ASSERT(pPrQuery && RPL_WHOISUSER == pPrQuery->uHeadReplyCode, "RPL_WHOISUSER != pPrQuery->uHeadReplyCode in CChatSocket::bHandleWhoIs");

	switch (uCode)
	{
		case RPL_AWAY:
		{
			ASSERT(pPrIrcMsg->byteParams >= 3, "pPrIrcMsg->byteParams < 3 in CChatSocket::bHandleWhoIs - RPL_AWAY");
			if (!bCopyStr(&(pPrUser->szAway), pPrIrcMsg->szParams[2]))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			break;
		}
		case RPL_WHOISUSER:
		{
			ASSERT(pPrIrcMsg->byteParams >= 6, "pPrIrcMsg->byteParams < 6 in CChatSocket::bHandleWhoIs - RPL_WHOISUSER");
			
			LPCSTR	szUserName = pPrIrcMsg->szParams[2];
			BOOL	bAuthUser  = bIsIrcXSocket();

			while ('~' == szUserName[0] || '+' == szUserName[0])
			{
				if ('~' == szUserName[0] && bIsIrcXSocket())
					bAuthUser = FALSE;
				szUserName++;
			}

			pPrUser->lModes |= (bAuthUser ? umAuthUser : umNotAuthUser);

			ASSERT(bIsUserModesValid(pPrUser->lModes), "Invalid user modes in CChatSocket::bHandleWhoIs");

			if (!bCopyStr(&(pPrUser->szUserName), szUserName) ||
				!bCopyStr(&(pPrUser->szIPAddress), pPrIrcMsg->szParams[3]))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			if (pPrIrcMsg->szParams[5][0] != g_chEOS &&
				!bCopyStr(&(pPrUser->szRealName), pPrIrcMsg->szParams[5]))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			break;
		}
		
		case RPL_WHOISSERVER:
		{
			ASSERT(pPrIrcMsg->byteParams >= 4, "pPrIrcMsg->byteParams < 4 in CChatSocket::bHandleWhoIs - RPL_WHOISSERVER");
			if (!bCopyStr(&(pPrUser->szServerName), pPrIrcMsg->szParams[2]) ||
				!bCopyStr(&(pPrUser->szServerInfo), pPrIrcMsg->szParams[3]))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			break;
		}
		
		case RPL_WHOISOPERATOR:
		{
			ASSERT(pPrIrcMsg->byteParams >= 3, "pPrIrcMsg->byteParams < 3 in CChatSocket::bHandleWhoIs - RPL_WHOISOPERATOR");
			if ('o' == pPrIrcMsg->szParams[2][10])	// :is an IRC operator
			{
				pPrUser->lModes &= ~umNotSysop;
				pPrUser->lModes |= umSysop;
			}
			else
			{
				if ('a' == pPrIrcMsg->szParams[2][10])	// :is an IRC administrator
				{
					pPrUser->lModes &= ~umNotAdmin;
					pPrUser->lModes |= umAdmin;
				}
			}
			break;
		}
		
		case RPL_WHOISIDLE:
		{
			ASSERT(pPrIrcMsg->byteParams >= 4, "pPrIrcMsg->byteParams < 4 in CChatSocket::bHandleWhoIs - RPL_WHOISIDLE");
			// we always get the idle time
			pPrUser->lIdleTime = SzToL(pPrIrcMsg->szParams[2]);
			if (pPrIrcMsg->byteParams == 5)
				// we got the signon time too
				pPrUser->lSignOnTime = SzToL(pPrIrcMsg->szParams[3]);
			break;
		}
		
		case RPL_WHOISCHANNELS:
		{
			CHAR			*sz = pPrIrcMsg->szParams[2];
			CHAR			*szChannelName;
			SAFEARRAYBOUND	sabound;
			VARIANT			vItem;
			HRESULT			hr;
			INT				cch = 0;
			LONG			lUBound, lUBoundOwner, lUBoundHost, lUBoundVoice;
			BOOL			bOwner, bHost, bVoice;

			// get the current vector size
			ASSERT(pPrUser->psaChannels, "pPrUser->psaChannels is NULL in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
			ASSERT(pPrUser->psaChannelsOwner, "pPrUser->psaChannelsOwner is NULL in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
			ASSERT(pPrUser->psaChannelsHost, "pPrUser->psaChannelsHost is NULL in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
			ASSERT(pPrUser->psaChannelsVoice, "pPrUser->psaChannelsVoice is NULL in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
			hr = SafeArrayGetUBound(pPrUser->psaChannels, 1, &lUBound);
			ASSERT(SUCCEEDED(hr), "SafeArrayGetUBound failed in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
			hr = SafeArrayGetUBound(pPrUser->psaChannelsOwner, 1, &lUBoundOwner);
			ASSERT(SUCCEEDED(hr), "SafeArrayGetUBound failed in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
			hr = SafeArrayGetUBound(pPrUser->psaChannelsHost, 1, &lUBoundHost);
			ASSERT(SUCCEEDED(hr), "SafeArrayGetUBound failed in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
			hr = SafeArrayGetUBound(pPrUser->psaChannelsVoice, 1, &lUBoundVoice);
			ASSERT(SUCCEEDED(hr), "SafeArrayGetUBound failed in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");

			// channel names are space separated in pPrIrcMsg->szParams[2]
			ASSERT(pPrIrcMsg->byteParams >= 3, "pPrIrcMsg->byteParams < 3 in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
			
			while (bGetNextParam(&sz, &szChannelName, &cch))
			{
				if (cch)
				{
					if ((lUBound+1) == pPrUser->lChannelCount)
					{
						// need to expand the vector a bit
						lUBound += g_nMaxUserChannels;
						sabound.cElements = lUBound + 1L;
						sabound.lLbound = 0L;

						if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannels, &sabound)))
							return FALSE;
					}

					bOwner = szChannelName[0] == '.';
					bHost  = szChannelName[0] == '@';
					bVoice = szChannelName[0] == '+';

					if (bOwner || bHost || bVoice)
						szChannelName++;

					vItem.vt = VT_BSTR;
					if (bIsIrcXSocket())
					{
						if (g_chExtChnPfx == szChannelName[0])
						{
							// Need to convert UTF8 extended channel name to Unicode
							LPWSTR wszChannelName;
							if (!bConvertUTF8StringToWide(szChannelName, 0 /*cchIn*/, &wszChannelName, NULL /*pcchOut*/, FALSE, TRUE))
							{
								m_hrLastError = E_OUTOFMEMORY;
								return FALSE;
							}
							ASSERT(wszChannelName, "wszChannelName is NULL in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");

							vItem.bstrVal = SysAllocString(wszChannelName);
							delete [] wszChannelName;
						}
						else
						{
							LPTSTR	szTmpCN;
							BOOL	bFreeConverted;
							// Might have to convert from Jis to ShiftJis, etc...
							if (!bConvertString(TRUE, m_byteCharSet, szChannelName, 0, &szTmpCN, NULL, &bFreeConverted))
							{
								m_hrLastError = E_OUTOFMEMORY;
								return FALSE;
							}
							ASSERT(szTmpCN, "szTmpCN is NULL in CChatSocket::bHandleWhoIs - RPL_WHOISCHANNELS");
							vItem.bstrVal = A2BSTR(szTmpCN);
							if (bFreeConverted)
								delete [] szTmpCN;
						}
					}
					else
						vItem.bstrVal = A2BSTR(szChannelName);

					if (!vItem.bstrVal)
					{
						m_hrLastError = E_OUTOFMEMORY;
						return FALSE;
					}

					if (FAILED(m_hrLastError = SafeArrayPutElement(pPrUser->psaChannels, &(pPrUser->lChannelCount), (PVOID) &vItem)))
					{
						VariantClear(&vItem);
						return FALSE;
					}

					pPrUser->lChannelCount++;

					if (bOwner)
					{
						// the user is an owner of a channel
						if ((lUBoundOwner+1) == pPrUser->lChannelOwnerCount)
						{
							// need to expand the vector a bit
							lUBoundOwner += g_nMaxUserChannels;
							sabound.cElements = lUBoundOwner + 1L;
							sabound.lLbound = 0L;

							if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannelsOwner, &sabound)))
							{
								VariantClear(&vItem);
								return FALSE;
							}
						}
						
						if (FAILED(m_hrLastError = SafeArrayPutElement(pPrUser->psaChannelsOwner, &(pPrUser->lChannelOwnerCount), (PVOID) &vItem)))
						{
							VariantClear(&vItem);
							return FALSE;
						}

						pPrUser->lChannelOwnerCount++;
					}

					if (bHost)
					{
						// the user is a host in a channel
						if ((lUBoundHost+1) == pPrUser->lChannelHostCount)
						{
							// need to expand the vector a bit
							lUBoundHost += g_nMaxUserChannels;
							sabound.cElements = lUBoundHost + 1L;
							sabound.lLbound = 0L;

							if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannelsHost, &sabound)))
							{
								VariantClear(&vItem);
								return FALSE;
							}
						}
						
						if (FAILED(m_hrLastError = SafeArrayPutElement(pPrUser->psaChannelsHost, &(pPrUser->lChannelHostCount), (PVOID) &vItem)))
						{
							VariantClear(&vItem);
							return FALSE;
						}

						pPrUser->lChannelHostCount++;
					}

					if (bVoice)
					{
						// the user has voice in a channel
						if ((lUBoundVoice+1) == pPrUser->lChannelVoiceCount)
						{
							// need to expand the vector a bit
							lUBoundVoice += g_nMaxUserChannels;
							sabound.cElements = lUBoundVoice + 1L;
							sabound.lLbound = 0L;

							if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannelsVoice, &sabound)))
							{
								VariantClear(&vItem);
								return FALSE;
							}
						}
						
						if (FAILED(m_hrLastError = SafeArrayPutElement(pPrUser->psaChannelsVoice, &(pPrUser->lChannelVoiceCount), (PVOID) &vItem)))
						{
							VariantClear(&vItem);
							return FALSE;
						}

						pPrUser->lChannelVoiceCount++;
					}

					VariantClear(&vItem);
				}
			}
			break;
		}
		
		case RPL_ENDOFWHOIS:
		{
			szLatestNickname[0] = g_chEOS;
			pPrLatestQuery = NULL;
			pPrLatestUser = NULL;

			// The whole property is here, we can now expose it to the user
			return bEndOfUser(pPrQuery, pPrUser, bUserProp, TRUE);
		}
		
		default:
		{
			ASSERT(FALSE, "Unexpected reply code in CChatSocket::bHandleWhoIs");
			m_hrLastError = E_FAIL;
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CChatSocket::bEndOfUser(PPRQUERY pPrQuery, PPRUSER pPrUser, BOOL bUserProp, BOOL bRealEnd)
{
	// We have pPrUser->lChannelCount, pPrUser->lChannelOwnerCount, pPrUser->lChannelHostCount, pPrUser->lChannelVoiceCount VARIANTs (BSTRs) in our safearrays
	ASSERT(pPrUser,  "pPrUser in CChatSocket::bEndOfUser");
	ASSERT(pPrQuery, "pPrQuery in CChatSocket::bEndOfUser");
	ASSERT(pPrUser->psaChannels, "pPrUser->psaChannels is NULL in CChatSocket::bEndOfUser");
	ASSERT(pPrUser->psaChannelsOwner, "pPrUser->psaChannelsOwner is NULL in CChatSocket::bEndOfUser");
	ASSERT(pPrUser->psaChannelsHost, "pPrUser->psaChannelsHost is NULL in CChatSocket::bEndOfUser");
	ASSERT(pPrUser->psaChannelsVoice, "pPrUser->psaChannelsVoice is NULL in CChatSocket::bEndOfUser");

	if (bUserProp)
	{
		if (!pPrQuery->bSyncAccess)
		{
			m_lWhoIsInProgress--;
			ASSERT(m_lWhoIsInProgress >= 0L, "m_lWhoIsInProgress < 0L in CChatSocket::bEndOfUser");
		}
		ASSERT(RPL_WHOISUSER == pPrQuery->uHeadReplyCode, "RPL_WHOISUSER != pPrQuery->uHeadReplyCode in CChatSocket::bEndOfUser");
	}
	else
	{
		if (bRealEnd)
		{
			ASSERT(pPrUser == &m_prUser, "pPrUser != &m_prUser in CChatSocket::bEndOfUser");
			m_lWhoIsListingInProgress--;
			ASSERT(m_lWhoIsListingInProgress >= 0L, "m_lWhoIsListingInProgress < 0L in CChatSocket::bEndOfUser");

			// Delete the query cell from the query list
			BOOL bRet = m_listQuery.bRemoveCell((PVOID) pPrQuery);
			ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatSocket::bEndOfUser");
		}
	}

	// Check if the user is ignored or not
	BOOL bIgnored;
	// The WHOIS call tells us if the user is a SysOp|Admin or not, so bCheckIfSysOpOrAdmin == FALSE
	bIsUserIgnored(pPrUser->szNickname, pPrUser->szUserName, pPrUser->szIPAddress, FALSE /*bCheckIfSysOpOrAdmin*/, &bIgnored);
	// We don't ignore Sysops and Admins
	if (bIgnored && 0 == (pPrUser->lModes & umSysop) && 0 == (pPrUser->lModes & umAdmin))
	{
		pPrUser->lModes &= ~umNotClientIgnored;
		pPrUser->lModes |= umClientIgnored;
	}
	else
		pPrUser->lModes |= umNotClientIgnored;

	ASSERT(bIsUserModesValid(pPrUser->lModes), "Invalid user modes in CChatSocket::bEndOfUser");

	// Check if the user is the caller, and update m_lUserModes
	if (0 == lstrcmpi(m_szNickname, pPrUser->szNickname))
		m_lUserModes = pPrUser->lModes;

	SAFEARRAYBOUND sabound;
	sabound.lLbound = 0L;

	// Let's resize the safe arrays appropriately if necessary
	if (pPrUser->lChannelCount > 0L)
	{
		sabound.cElements = pPrUser->lChannelCount;
		if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannels, &sabound)))
			return FALSE;
	}
	if (pPrUser->lChannelOwnerCount > 0L)
	{
		sabound.cElements = pPrUser->lChannelOwnerCount;
		if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannelsOwner, &sabound)))
			return FALSE;
	}
	if (pPrUser->lChannelHostCount > 0L)
	{
		sabound.cElements = pPrUser->lChannelHostCount;
		if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannelsHost, &sabound)))
			return FALSE;
	}
	if (pPrUser->lChannelVoiceCount > 0L)
	{
		sabound.cElements = pPrUser->lChannelVoiceCount;
		if (FAILED(m_hrLastError = SafeArrayRedim(pPrUser->psaChannelsVoice, &sabound)))
			return FALSE;
	}
		
	if (bUserProp)
	{
		// This is a property access condition
		if (!pPrQuery->bSyncAccess)
			return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_USERPROP, (WPARAM) pPrQuery, (LPARAM) pPrQuery->pvData));
		else
			return TRUE;
	}
	else
	{
		// This is a user listing condition
		m_hrLastError = HrSendEvent(WM_U_USERPROP, (WPARAM) 0L, (LPARAM) pPrUser);
		if (bRealEnd)
			FireEndEnumeration(NULL, etUsers);
		return SUCCEEDED(m_hrLastError);
	}
}


BOOL CChatSocket::bHandleUserHost(PPRQUERY pPrQry, PPRIRCMSG pPrIrcMsg)
{
	PPRQUERY	pPrQuery  = pPrQry;
	PPRMEMBER	pPrMember = NULL;
	PPRUSER		pPrUser	  = NULL;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleUserHost");
	ASSERT(pPrIrcMsg->byteParams >= 2, "pPrIrcMsg->byteParams < 2 in CChatSocket::bHandleUserHost");
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleUserHost");

	// pPrIrcMsg->szParams[0] is the caller's nickname
	ASSERT(0 == lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname), "Unexpected param 0 in CChatSocket::bHandleUserHost");

	// Three possible situations here:
	//	- the user did an asynchronous member property access
	//	- the main thread is waiting for the answer during a synchronous access
	//	- this answer comes after a timeout and we'll fire the OnMemberProperty event

	// pPrIrcMsg->szParams[1] is: "<nickname> ['*'] '=' <'+'|'-'><'~'><username>'@'<ipaddress>"
	// where '*' => user is Sysop or Admin
	//       '+' => user isn't away
	//       '-' => user is away
	//       '~' => user isn't authenticated
	// or pPrIrcMsg->szParams[1] is "" which means that the nickname was not found
	if (pPrIrcMsg->szParams[1][0] == g_chEOS)
	{
		// nickname was not found
		m_hrLastError = CC_E_NOSUCHNICK;
		return FALSE;
	}

	CHAR	*szNickname = pPrIrcMsg->szParams[1];
	CHAR	*szUserName = NULL;
	CHAR	*szIPAddress= NULL;
	CHAR	ch;
	BOOL	bIsSysop	= FALSE;
	BOOL	bIsAway		= FALSE;
	BOOL	bIsAuth		= bIsIrcXSocket();
	LONG	*plModes;

	CHAR	*sz = szNickname;
	SkipTillChar(&sz, '=');

	ASSERT(*sz == '=', "Unexpected USERHOST answer in CChatSocket::bHandleUserHost");
	szUserName = sz+1;

	// Skip the trailing * and potential space
	sz--;
	while (*sz == '*' || *sz == ' ')
	{
		if (*sz == '*')
			bIsSysop = TRUE;
		sz--;
	}
	sz++;
	ch = *sz;	// might have to fix this if the message is unexpected
	*sz = g_chEOS;

	if (pPrQuery)
	{
		// Since this is a synchronous access, it can be for a member or a user
		if (atMember == pPrQuery->at)
		{
			pPrMember = (PPRMEMBER) pPrQuery->pvData;
			ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bHandleUserHost");
			ASSERT(pPrMember->szNickname, "pPrMember->szNickname is NULL in CChatSocket::bHandleUserHost");
			if (0 != lstrcmpi(szNickname, pPrMember->szNickname))
			{
				*sz = ch;
				m_hrLastError = CC_E_NOTQUERIEDOBJECT;
				return FALSE;
			}
		}
		else
		{
			ASSERT(atUser == pPrQuery->at, "atUser != pPrQuery->at in CChatSocket::bHandleUserHost");
			pPrUser = (PPRUSER) pPrQuery->pvData;
			ASSERT(pPrUser, "pPrUser is NULL in CChatSocket::bHandleUserHost");
			ASSERT(pPrUser->szNickname, "pPrUser->szNickname is NULL in CChatSocket::bHandleUserHost");
			if (0 != lstrcmpi(szNickname, pPrUser->szNickname))
			{
				*sz = ch;
				m_hrLastError = CC_E_NOTQUERIEDOBJECT;
				return FALSE;
			}
		}
	}
	else
	{
		// Asynchronous access - let's try to find the query cell in the list
		enumAssociatedType	at = atMember;	// Can only be for a mamber when asynchronous

		if (!m_listQuery.bFindCellFromData(1L,
										   g_nSearchByAssociatedType,
										   (PVOID) &at,
										   g_nSearchByNickname,
										   szNickname,
										   (PVOID*) &pPrQuery,
										   NULL))
		{
			*sz = ch;
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
		pPrMember = (PPRMEMBER) pPrQuery->pvData;
		ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bHandleUserHost");
		ASSERT(pPrMember->szNickname, "pPrMember->szNickname is NULL in CChatSocket::bHandleUserHost");
	}

	ASSERT(RPL_USERHOST == pPrQuery->uHeadReplyCode, "RPL_USERHOST != pPrQuery->uHeadReplyCode in CChatSocket::bEndOfUser");
	
	// Skip potential spaces
	while (*szUserName == ' ')
		szUserName++;

	while ('+' == szUserName[0] || '-' == szUserName[0] || '~' == szUserName[0])
	{
		// Is member away or not?
		if ('-' == szUserName[0])
			bIsAway = TRUE;

		if ('~' == szUserName[0] && bIsIrcXSocket())
			bIsAuth = FALSE;

		szUserName++;
	}

	sz = szUserName;
	SkipTillPrefixExtender(&sz);
	ASSERT(*sz == '@', "Unexpected USERHOST answer in CChatSocket::bHandleUserHost");
	*sz = g_chEOS;

	szIPAddress = sz+1;

	ASSERT(pPrMember || pPrUser, "pPrMember && pPrUser are NULL in CChatSocket::bHandleUserHost");
	ASSERT(!pPrMember || 0 == lstrcmpi(szNickname, pPrMember->szNickname), "Unexpected Nickname in CChatSocket::bHandleUserHost");
	ASSERT(!pPrUser || 0 == lstrcmpi(szNickname, pPrUser->szNickname), "Unexpected Nickname in CChatSocket::bHandleUserHost");

	if (pPrMember)
	{
		if (!bCopyStr(&(pPrMember->szUserName), szUserName) ||
			!bCopyStr(&(pPrMember->szIPAddress), szIPAddress))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		plModes = &(pPrMember->lModes);
	}
	else
	{
		if (!bCopyStr(&(pPrUser->szUserName), szUserName) ||
			!bCopyStr(&(pPrUser->szIPAddress), szIPAddress))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		plModes = &(pPrUser->lModes);
	}

	// Attention!  This assumes that:
	// umSysop == mmSysop
	// umNotSysop == mmNotSysop
	// umAway == mmAway
	// umNotAway == mmNotAway
	// umAuthUser == mmAuthMember
	// umNotAuthUser == mmNotAuthMember
	if (bIsSysop)
	{
		*plModes |= mmSysop;
		*plModes &= ~mmNotSysop;
	}
	else
	{
		*plModes &= ~mmSysop;
		*plModes |= mmNotSysop;
	}

	if (bIsAway)
	{
		*plModes |= mmAway;
		*plModes &= ~mmNotAway;
	}
	else
	{
		*plModes &= ~mmAway;
		*plModes |= mmNotAway;
	}

	if (bIsAuth)
	{
		*plModes |= mmAuthMember;
		*plModes &= ~mmNotAuthMember;
	}
	else
	{
		*plModes &= ~mmAuthMember;
		*plModes |= mmNotAuthMember;
	}

	ASSERT(!pPrMember || bIsMemberModesValid(pPrMember->lModes), "Invalid member modes in CChatSocket::bHandleUserHost");
	ASSERT(!pPrUser || bIsUserModesValid(pPrUser->lModes), "Invalid user modes in CChatSocket::bHandleUserHost");

	// The whole property is here, we can now expose it to the user
	if (!pPrQuery->bSyncAccess)
	{
		ASSERT(atMember == pPrQuery->at, "atMember != pPrQuery->at in CChatSocket::bHandleUserHost");
		return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_MEMBERPROP, (WPARAM) pPrQuery, (LPARAM) TRUE));
	}

	return TRUE;
}


BOOL CChatSocket::bHandleServerInfo(PPRIRCMSG pPrIrcMsg, UINT uCode, BOOL bAsyncAccess)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleServerInfo");

	if (!m_prServer.psaInfo || m_rgbServerPropLocal[pnServerInfo])
	{
		// user sent a raw INFO message to the server
		m_hrLastError = E_FAIL;
		return FALSE;	// this will trigger the OnProtocolMessage event
	}

	// Three possible situations here:
	//	- the user did an asynchronous property access
	//	- the main thread is waiting for the answer during a synchronous access
	//	- this answer comes after a timeout and we'll fire the OnServerProperty event
	switch (uCode)
	{
		case RPL_INFO:
		{
			// Fill in the safe array of VARIANTs (BSTRs)
			ASSERT(m_prServer.psaInfo, "m_prServer.psaInfo is NULL in CChatSocket::bHandleServerInfo");
			ASSERT(pPrIrcMsg->byteParams > 0, "pPrIrcMsg->byteParams == 0 in CChatSocket::bHandleServerInfo");
			ASSERT(m_prServer.lInfoCount < g_nMaxServerInfo, "m_prServer.lInfoCount >= g_nMaxServerInfo in CChatSocket::bHandleServerInfo");

			VARIANT vItem;

			vItem.vt = VT_BSTR;
			vItem.bstrVal = A2BSTR(pPrIrcMsg->szParams[pPrIrcMsg->byteParams - 1]);
			if (!vItem.bstrVal)
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}

			m_hrLastError = SafeArrayPutElement(m_prServer.psaInfo, &(m_prServer.lInfoCount), (PVOID) &vItem);

			VariantClear(&vItem);

			if (SUCCEEDED(m_hrLastError))
			{
				m_prServer.lInfoCount++;
				return TRUE;
			}
			else
				return FALSE;
		}

		case RPL_ENDOFINFO:
		{
			// The whole property is here, we can now expose it to the user
			// We have m_prServer.lInfoCount VARIANTs (BSTRs) in our safearray
			ASSERT(m_prServer.psaInfo, "m_prServer.psaInfo is NULL in CChatSocket::bHandleServerInfo");
			ASSERT(m_prServer.lInfoCount <= g_nMaxServerInfo, "m_prServer.lInfoCount > g_nMaxServerInfo in CChatSocket::bHandleServerInfo");

			// Let's resize the safe array appropriately
			SAFEARRAYBOUND sabound;
			
			sabound.cElements = m_prServer.lInfoCount;
			sabound.lLbound = 0L;

			if (FAILED(m_hrLastError = SafeArrayRedim(m_prServer.psaInfo, &sabound)))
				return FALSE;
			
			m_rgbServerPropLocal[pnServerInfo] = TRUE;

			if (bAsyncAccess)
				return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_SERVERPROP, 0L, 0L));
			break;
		}

		default:
		{
			ASSERT(FALSE, "Unexpected reply code in CChatSocket::bHandleServerInfo");
			m_hrLastError = E_FAIL;
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CChatSocket::bHandleServerLUsers(PPRIRCMSG pPrIrcMsg, UINT uCode, BOOL bAsyncAccess)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleServerLUsers");

	// Three possible situations here:
	//	- the user did an asynchronous property access
	//	- the main thread is waiting for the answer during a synchronous access
	//	- this answer comes after a timeout and we'll fire the OnServerProperty event
	switch (uCode)
	{
		case RPL_LUSERCLIENT:
		{
			ASSERT(pPrIrcMsg->byteParams >= 2, "pPrIrcMsg->byteParams < 2 in CChatSocket::bHandleServerLUsers - RPL_LUSERCLIENT");

			// We extract the lNetInvisibleCount, lNetServerCount and lNetUserCount properties from this message
			// "There are <lNetUserCount> users and <lNetInvisibleCount> invisible on <lNetServerCount> servers"
			LPTSTR szTmp = pPrIrcMsg->szParams[1];

			SkipTillDigit(&szTmp);
			m_prServer.lNetUserCount = SzToL(szTmp);
			SkipTillAlpha(&szTmp);
			SkipTillDigit(&szTmp);
			m_prServer.lNetInvisibleCount = SzToL(szTmp);
			SkipTillAlpha(&szTmp);
			SkipTillDigit(&szTmp);
			m_prServer.lNetServerCount = SzToL(szTmp);

			break;
		}

		case RPL_LUSEROP:
		{
			// We extract the lSysopCount property from this message
			ASSERT(pPrIrcMsg->byteParams >= 3, "pPrIrcMsg->byteParams < 3 in CChatSocket::bHandleServerLUsers - RPL_LUSEROP");

			m_prServer.lSysopCount = SzToL(pPrIrcMsg->szParams[1]);
			break;
		}

		case RPL_LUSERUNKNOWN:
		{
			// We extract the lUnknownConnectionCount property from this message
			ASSERT(pPrIrcMsg->byteParams >= 3, "pPrIrcMsg->byteParams < 3 in CChatSocket::bHandleServerLUsers - RPL_LUSERUNKNOWN");

			m_prServer.lUnknownConnectionCount = SzToL(pPrIrcMsg->szParams[1]);
			break;
		}

		case RPL_LUSERCHANNELS:
		{
			// We extract the lChannelCount property from this message
			ASSERT(pPrIrcMsg->byteParams >= 3, "pPrIrcMsg->byteParams < 3 in CChatSocket::bHandleServerLUsers - RPL_LUSERCHANNELS");

			m_prServer.lChannelCount = SzToL(pPrIrcMsg->szParams[1]);
			break;
		}

		case RPL_LUSERME:
		{
			// We extract the lNodeServerCount and lNodeUserCount properties from this message
			// "I have <lNodeUserCount> clients and <lNodeServerCount> servers"

			ASSERT(pPrIrcMsg->byteParams >= 2, "pPrIrcMsg->byteParams < 2 in CChatSocket::bHandleServerLUsers - RPL_LUSERME");

			LPTSTR szTmp = pPrIrcMsg->szParams[1];

			SkipTillDigit(&szTmp);
			m_prServer.lNodeUserCount = SzToL(szTmp);
			SkipTillAlpha(&szTmp);
			SkipTillDigit(&szTmp);
			m_prServer.lNodeServerCount = SzToL(szTmp);

			// The whole property is here, we can now expose it to the user
			if (bAsyncAccess)
				return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_SERVERPROP, 0L, 0L));
			break;
		}

		default:
		{
			ASSERT(FALSE, "Unexpected reply code in CChatSocket::bHandleServerLUsers");
			m_hrLastError = E_FAIL;
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CChatSocket::bHandleAway(PPRIRCMSG pPrIrcMsg, UINT uCode)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleAway");

	switch (uCode)
	{
		case RPL_AWAY:
		{
			// "<nickname> :<away message>"
			// pPrIrcMsg->szParams[1] = nickname
			// pPrIrcMsg->szParams[2] = away message
			ASSERT(pPrIrcMsg->byteParams >= 3, "pPrIrcMsg->byteParams < 3 in CChatSocket::bHandleAway - RPL_AWAY");

			// Fire the OnPrivateMessage event
			BOOL bRet = bFireTextMessage(NULL, pPrIrcMsg->szParams[1], NULL, pPrIrcMsg->szParams[2], 0L, pmtAway);
			ASSERT(bRet, "bFireTextMessage failed in CChatSocket::bHandleAway");
			break;
		}
		case RPL_UNAWAY:
		case RPL_NOWAWAY:
		{
			// ":You are no longer marked as being away"
			// or
			// ":You have been marked as being away"
			ASSERT(pPrIrcMsg->byteParams >= 2, "pPrIrcMsg->byteParams < 2 in CChatSocket::bHandleAway - RPL_UNAWAY || RPL_NOWAWAY");

			CChatChannel		*pChannel = NULL;
			enumChannelState	chs = chsOpen;
			PPRMEMBER			pPrMember = NULL;
			VARIANT				vOldProperty, vNewProperty;
			BOOL				bRet;

			VariantInit(&vOldProperty);
			VariantInit(&vNewProperty);

			vOldProperty.vt = vNewProperty.vt = VT_I4;

			vOldProperty.lVal = vNewProperty.lVal = m_lUserModes;

			if (RPL_UNAWAY == uCode)
			{
				vNewProperty.lVal &= ~umAway;
				vNewProperty.lVal |=  umNotAway;
			}
			else
			{
				vNewProperty.lVal |=  umAway;
				vNewProperty.lVal &= ~umNotAway;
			}

			// cash the new Modes value
			m_lUserModes = vNewProperty.lVal;

			ASSERT(bIsUserModesValid(m_lUserModes), "Invalid user modes in CChatSocket::bHandleAway");

			// Did anything actually change?
			if (0L != (vOldProperty.lVal ^ vNewProperty.lVal))
			{
				// Fire the OnUserPropertyChanged event
				ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleAway");
				bRet = bFireUserPropertyChanged(m_szNickname, NULL, g_rgwszUserPropertyNames[pnUserModes], &vOldProperty, &vNewProperty);
				ASSERT(bRet, "bFireUserPropertyChanged failed in CChatSocket::bHandleAway");
			}

			// Also fire the OnMemberPropertyChanged event for each open channel in the collection
			if (m_lOpenChannels > 0L)
			{
				// Go through our open channels in the collection and fire the event
				for (LONG lPosition = 1L; lPosition <= m_lOpenChannels; lPosition++)
				{
					if (m_listChannel.bFindCellFromData(lPosition,
														g_nSearchByState,
														(PVOID) &chs,
														g_nSearchNone,
														NULL,
														(PVOID*) &pChannel,
														NULL))
					{
						// Got an open channel
						ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleAway");
						ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleAway");

						// Get this user from the list
						if (pChannel->bGetMemberFromNickname(m_szNickname, &pPrMember))
						{
							// We found the user in there
							ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bHandleAway");

							vOldProperty.lVal = vNewProperty.lVal = pPrMember->lModes;

							if (RPL_UNAWAY == uCode)
							{
								vNewProperty.lVal &= ~mmAway;
								vNewProperty.lVal |=  mmNotAway;
							}
							else
							{
								vNewProperty.lVal |=  mmAway;
								vNewProperty.lVal &= ~mmNotAway;
							}

							// Did anything actually change?
							if (0L != (vOldProperty.lVal ^ vNewProperty.lVal))
							{
								// Fire the OnMemberPropertyChanged event
								bRet = bFireMemberPropertyChanged(pChannel, m_szNickname, NULL, g_rgwszMemberPropertyNames[pnMemberModes], &vOldProperty, &vNewProperty);
								ASSERT(bRet, "bFireMemberPropertyChanged failed in CChatSocket::bHandleAway");
							}
						}
					}
				}
			}

			VariantClear(&vOldProperty);
			VariantClear(&vNewProperty);
			break;
		}
		default:
		{
			ASSERT(FALSE, "Unexpected reply code in CChatSocket::bHandleAway");
			m_hrLastError = E_FAIL;
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CChatSocket::bHandleMessageOfTheDay(PPRIRCMSG pPrIrcMsg, UINT uCode)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleMessageOfTheDay");

	switch (uCode)
	{
		case RPL_MOTDSTART:
			FireBeginEnumeration(NULL, etMessageOfTheDay);
			break;

		case RPL_MOTD:
		case RPL_MOTD2:
			if (pPrIrcMsg->byteParams < 2)
			{
				ASSERT(FALSE, "Unexpected params in CChatSocket::bHandleMessageOfTheDay");
				m_hrLastError = CC_E_SERVER;
				return FALSE;
			}
			FireServerTextMessage(smtMessageOfTheDay, pPrIrcMsg->szParams[1]+1);
			break;

		case RPL_ENDOFMOTD:
			FireEndEnumeration(NULL, etMessageOfTheDay);
			break;

		default:
			ASSERT(FALSE, "Unexpected reply code in CChatSocket::bHandleMessageOfTheDay");
			m_hrLastError = E_FAIL;
			return FALSE;
	}
	return TRUE;
}


BOOL CChatSocket::bHandleCreateJoin(PPRIRCMSG pPrIrcMsg, SHORT nCmd)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleCreateJoin");

	if (pPrIrcMsg->szPrefix[0])
	{
		//
		// First we have to figure out whether this is an ADD member or an actual JOIN
		// received because of something this user did. In IRC, both are notified using 
		// the JOIN message
		// The differentiation is based on the Prefix. If the prefix matches the user's nick,
		// then the JOIN came because we joined the channel
		//
		CHAR* szNickname = pPrIrcMsg->szPrefix;
		SkipTillPrefixExtender(&szNickname);
		CHAR chPrefixExtender = *szNickname;
		*szNickname = g_chEOS;

		ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleCreateJoin");
		if (0 != ::lstrcmpi(m_szNickname, pPrIrcMsg->szPrefix))
		{
			// This is actually an ADD Member
			*szNickname = chPrefixExtender;
			return bHandleAddMember(pPrIrcMsg);
		}
	}

	// This user joined or created a channel
	OutputDebugThreadIdString("CChatSocket::bHandleCreateJoin - AddChannel Msg\n");

	BYTE				*pb;
	INT					cb;
	LPCTSTR				szChannelName = NULL;
	enumChannelState	chs = chsOpening;
	CChatChannel		*pChannel = NULL;

	// the channel name is stored in the first param
	if (pPrIrcMsg->byteParams == 0)
	{
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// find the channel cell in the channel list for this event
	// it is the cell with m_chs == chsOpening and name szParam[0]
	if (!m_listChannel.bFindCellFromData(1L /*lPosition*/, 
										 g_nSearchByName /*nSearchID1*/,
										 (PVOID) pPrIrcMsg->szParams[0],
										 g_nSearchByState /*nSearchID2*/,
										 (PVOID) &chs,
										 (PVOID*) &pChannel,
										 NULL /*plPositionFound*/))
	{
		// This channel must be a clone, try to find the channel with the original name
		if (!m_listChannel.bFindCellFromData(1L /*lPosition*/, 
											 g_nSearchByCloneName /*nSearchID1*/,
											 (PVOID) pPrIrcMsg->szParams[0],
											 g_nSearchByState /*nSearchID2*/,
											 (PVOID) &chs,
											 (PVOID*) &pChannel,
											 NULL /*plPositionFound*/))
		{
			m_hrLastError = CC_E_SERVER;
			return FALSE;
		}
	}

	ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleCreateJoin");
	ASSERT(pChannel->m_chs == chsOpening, "pChannel->m_chs != chsOpening in CChatSocket::bHandleCreateJoin");

	// save the final name of the channel (might be a clone)
	if (!bCopyStr(&(pChannel->m_prChannel.szName), pPrIrcMsg->szParams[0]))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	// update the channel object ID number
	if (cmdidCreate == nCmd && pPrIrcMsg->byteParams > 1)
		// Expected command: "CREATE #MyRoom 080000008"
		// No prefix
		// Channel name = pPrIrcMsg->szParams[0]
		// Channel object ID = pPrIrcMsg->szParams[1]
		pChannel->m_prChannel.lOID = HexSzToL(pPrIrcMsg->szParams[1]);

	// update the channel's status
	InterlockedDecrement(&m_lOpeningChannels);
	InterlockedIncrement(&m_lOpenChannels);

	ASSERT(pChannel->m_chs != chsOpen, "pChannel->m_chs == chsOpen in CChatSocket::bHandleCreateJoin");

	// Create a new query cell and add it at the end of the list
	PPRQUERY pPrQuery;
	
	if (pPrQuery = new PRQUERY)
		ZeroMemory(pPrQuery, sizeof(PRQUERY));
	if (!pPrQuery || 
		!bCopyStr(&(pPrQuery->szChannelName), pChannel->m_prChannel.szName) ||
		!m_listQuery.bInsertCell((LONG) m_listQuery.DwGetCellCount()+1, (PVOID) pPrQuery))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	pPrQuery->at = atChannel;
	pPrQuery->bSyncAccess = FALSE;
	pPrQuery->uHeadReplyCode = RPL_CHANNELMODEIS;
	pPrQuery->pvData = (PVOID) &(pChannel->m_prChannel);

	if (SUCCEEDED(m_hrLastError = HrMakeGetModeMsg(&pb, &cb, pChannel->m_prChannel.szName)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);
	if (SUCCEEDED(m_hrLastError))
		m_lModeIsInProgress++;

	// Expose the new state only after requesting the channel modes!
	FireChannelState(pChannel, pChannel->m_chs = chsOpen);

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatSocket::bHandleAddMember(PPRIRCMSG pPrIrcMsg)
{
	enumChannelState	chs = chsOpen;
	CChatItems*			pCItems = NULL;
	CChatChannel*		pChannel = NULL;
	PPRMEMBER			pPrMember = NULL;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleAddMember");
	
	if (pPrIrcMsg->byteParams == 0 || g_chEOS == pPrIrcMsg->szPrefix[0])
	{
		ASSERT(FALSE, "Unexpected prefix or param in CChatSocket::bHandleAddMember");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// Which channel is being joined?
	if (!m_listChannel.bFindCellFromData(1L /*lPosition*/, 
										 g_nSearchByName /*nSearchID1*/,
										 (PVOID) pPrIrcMsg->szParams[0],
										 g_nSearchByState /*nSearchID2*/,
										 (PVOID) &chs,
										 (PVOID*) &pChannel,
										 NULL /*plPositionFound*/))
	{
		m_hrLastError = E_FAIL;
		return FALSE;
	}
	
	// We got the channel 
	ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleAddMember");
	ASSERT(pChannel->m_chs == chsOpen, "pChannel->m_chs != chsOpen in CChatSocket::bHandleAddMember");

	// Need to add this member to the member list and fire the AddMember event
	if (!pChannel->bAddMember((LPCTSTR) pPrIrcMsg->szPrefix, &pPrMember))
	{
		m_hrLastError = pChannel->HrGetLastError();
		return FALSE;
	}

	ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bHandleAddMember");
	ASSERT(pChannel->bGotMemberList(), "Unexpected AddMember before getting the initial member list");

	// Create a CChatItems object via a virtual function that in fact creates a new CItemsObject object ;-)
	if (!bCreateChatItems(&pCItems))
		return FALSE;

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bHandleAddMember");

	// Fill in the CChatItems from the pPrMember here
	if (!bFillChatItemsFromMember(pCItems, pPrMember))
	{
		ReleaseChatItems(pCItems);
		return FALSE;
	}
	FireAddMember(pChannel, pPrMember->szNickname, pCItems);

	return TRUE;
}


BOOL CChatSocket::bHandlePing(PPRIRCMSG pPrIrcMsg)
{
	BYTE	*pb;
	INT		cb;
	CHAR	*szPong;
	
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandlePing");

	// Did the server send us a specific pong string? If so, return that
	if (pPrIrcMsg->byteParams == 0)
	{
		ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandlePing");
		szPong = m_szNickname;
	}
	else
		szPong = pPrIrcMsg->szParams[0];

	if (SUCCEEDED(m_hrLastError = HrMakePingOrPongMsg(&pb, &cb, FALSE, szPong)))
		m_hrLastError = m_cconn.HrSendAndDelete(pb, cb);

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatSocket::bHandlePart(PPRIRCMSG pPrIrcMsg)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandlePart");
	
	if (g_chEOS == pPrIrcMsg->szPrefix[0])
	{
		ASSERT(FALSE, "Empty prefix in CChatSocket::bHandlePart");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// The first part of the prefix has the nickname of the member who has parted
	CHAR *szNickname, *sz;
	sz = szNickname = pPrIrcMsg->szPrefix;
	SkipTillPrefixExtender(&sz);
	*sz = g_chEOS;

	// The first parameter is the name of the channel being exited.
	ASSERT(pPrIrcMsg->byteParams > 0, "Empty 1st param in CChatSocket::bHandlePart");

	// Find the CChatChannel for this channel
	CChatChannel*		pChannel = NULL;
	enumChannelState	chs = chsOpen;
	
	// find the first open channel with the given name
	m_listChannel.bFindCellFromData(1,
									g_nSearchByName,
									(PVOID) pPrIrcMsg->szParams[0],
									g_nSearchByState,
									(PVOID) &chs,
									(PVOID*) &pChannel,
									NULL);
	
	if (pChannel)
	{
		// We found the channel
		ASSERT(pChannel->bGotMemberList(), "Unexpected DelMember before treating initial member list");

		// Delete this member from the list of members of this channel
		// This will assure the integrity of the MemberCount prop in the OnDelMember event handler
		if (!pChannel->bDelMember(szNickname))
		{
			#ifdef DEBUG
				sprintf(g_szDebugStr, "CChatSocket::bHandlePart - Couldn't bDelMember parting member '%s'\n", szNickname);
				OutputDebugThreadIdString(g_szDebugStr);
				pChannel->DumpMembers();
			#endif
			m_hrLastError = pChannel->HrGetLastError();
			return FALSE;
		}

		// Fire the DelMember event
		FireDelMember(pChannel, szNickname);  // virtual function in CMsChatPr and CChatSock

		// Am I leaving by the way?
		ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandlePart");
		if (0 == lstrcmpi(szNickname, m_szNickname))
			// It's me! Let's update the channel's state and clean up the member list
			return bLeftChannel(pChannel);
		else
			return TRUE;
	}
	else
	{
		ASSERT(FALSE, "Couldn't find channel in CChatSocket::bHandlePart");
		m_hrLastError = E_FAIL;
		return FALSE;
	}
}


BOOL CChatSocket::bHandleNameReply(PPRIRCMSG pPrIrcMsg)
{
	LONG				lMode;
	BOOL				bQueryReply, bStartedList, bGotList = TRUE;
	PPRQUERY			pPrQuery = NULL;
	CChatItems*			pCItems = NULL;
	CChatChannel*		pChannel = NULL;
	enumChannelState	chs = chsOpen;
	enumAssociatedType	at = atQuery;
	enumQueryType		qt;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleNameReply");

	if (pPrIrcMsg->byteParams < 4)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleNameReply");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	//
	// The member list is always preceeded by information that tells us what kind
	// of CHANNEL this is!
	// First comes the NICK of the person this was sent to
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleNameReply");
	ASSERT(0 == lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname), "Unexpected param 0 in CChatSocket::bHandleNameReply");

	//
	// Get the channel's mode
	//
	switch (pPrIrcMsg->szParams[1][0])
	{
		default:
			ASSERT(FALSE, "Unexpected Channel Mode in CChatSocket::bHandleNameReply");
			m_hrLastError = CC_E_SERVER;
			return FALSE;
		
		case '=':
			lMode = cmPublic;
			break;

		case '*':
			lMode = cmPrivate;
			break;

		case '@':
			lMode = cmSecret;
			break;
	}

	// Now figure out what kind of channel this is...
	// Find the first open channel with the given name
	m_listChannel.bFindCellFromData(1,
									g_nSearchByName,
									(PVOID) pPrIrcMsg->szParams[2],
									g_nSearchByState,
									(PVOID) &chs,
									(PVOID*) &pChannel,
									NULL);

	// And also if this could result from a listing query
	bQueryReply = m_listQuery.bFindCellFromData(1,
												g_nSearchByAssociatedType,
												(PVOID) &at,
												g_nSearchByName,
												(PVOID) pPrIrcMsg->szParams[2],
												(PVOID*) &pPrQuery,
												NULL);

	if (bQueryReply)
	{
		ASSERT(pPrQuery, "pPrQuery is NULL in CChatSocket::bHandleNameReply");
		qt = (enumQueryType) (DWORD) pPrQuery->pvData;
	}

	if (pChannel)
	{
		bGotList	 = pChannel->bGotMemberList();
		bStartedList = pChannel->bStartedMemberList();

		if (!bStartedList && !bGotList)
		{
			ASSERT(!bQueryReply, "bQueryReply is TRUE in CChatSocket::bHandleNameReply");
			// Case of the beginning of the initial list
			pChannel->AddChannelModes(lMode);
			pChannel->SetStartedMemberList(TRUE);
			// Notify the user of the incoming initial member list
			FireBeginEnumeration(pChannel, etInitialMembers);
		}
		else
		{
			// Is this for a query reply?
			if (bQueryReply)
			{
				// Is this for a Member or User Listing?
				if (qt == qtNamesForUsers)
				{
					if (!m_bFiredUserChanBeginEnum)
					{
						FireBeginEnumeration(NULL, etUsers);
						m_bFiredUserChanBeginEnum = TRUE;
					}
					pChannel = NULL;	// We want users and not members
				}
				else
				{
					ASSERT(qt == qtNamesForMembers, "Unexpected QueryType in CChatSocket::bHandleNameReply");
					if (!bStartedList)
					{
						FireBeginEnumeration(pChannel, etMembers);	
						pChannel->SetStartedMemberList(TRUE);
					}
				}
			}
			else
			{
				// Could be the continuation of an initial list...
				if (!bStartedList || bGotList)
				{
					// Must come from a raw send text
					m_hrLastError = CC_E_NOTQUERIEDOBJECT;
					return FALSE;
				}
			}
		}
	}
	else
	{
		// Is this a listing reply?	
		if (bQueryReply)
		{
			ASSERT(qt == qtNamesForUsers, "Unexpected QueryType in CChatSocket::bHandleNameReply");
			// Might have to fire the OnBeginEnumeration event
			if (!m_bFiredUserChanBeginEnum)
			{
				FireBeginEnumeration(NULL, etUsers);
				m_bFiredUserChanBeginEnum = TRUE;
			}
		}
		else
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
	}

	
	//
	// And finally, Loop through and Add Member each member in the list
	//
	PPRMEMBER	pPrMember;
	CHAR		*sz = pPrIrcMsg->szParams[3];
	CHAR		*szNickname;
	BOOL		bRet = TRUE;
	INT			cch = 0;

	while (bGetNextParam(&sz, &szNickname, &cch) && bRet)
	{
		if (cch)
		{
			if (bGotList)
			{
				// This is for a NAMES command reply
				if (pChannel)
					bRet = bNotifyMemberProperty(pChannel, szNickname);
				else
					bRet = bNotifyUserProperty(szNickname);
			}
			else
			{
				ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleNameReply");
				if (!(bRet = pChannel->bAddMember(szNickname, &pPrMember)))
					m_hrLastError = pChannel->HrGetLastError();
				else
				{
					ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bHandleNameReply");
				
					// Create a CChatItems object via a virtual function that in fact creates a new CItemsObject object ;-)
					if (bRet = bCreateChatItems(&pCItems))
					{
						ASSERT(pCItems, "pCItems is NULL in CChatSocket::bHandleNameReply");
					
						// Fill in the CChatItems from the pPrMember
						if (bRet = bFillChatItemsFromMember(pCItems, pPrMember))
							FireAddMember(pChannel, pPrMember->szNickname, pCItems);
						else
							ReleaseChatItems(pCItems);
					}
				}
			}
		}
	}

	return bRet;
}


BOOL CChatSocket::bHandleEndOfNames(PPRIRCMSG pPrIrcMsg)
{
	BOOL				bQueryReply, bRet;
	PPRQUERY			pPrQuery = NULL;
	CChatChannel*		pChannel = NULL;
	enumChannelState	chs = chsOpen;
	enumAssociatedType	at = atQuery;
	enumQueryType		qt;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleEndOfNames");

	if (pPrIrcMsg->byteParams < 2)
	{
		ASSERT(FALSE, "Unexpected param 1 in CChatSocket::bHandleEndOfNames");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// Figure out what kind of channel this is...
	m_listChannel.bFindCellFromData(1,
									g_nSearchByName,
									(PVOID) pPrIrcMsg->szParams[1],
									g_nSearchByState,
									(PVOID) &chs,
									(PVOID*) &pChannel,
									NULL);

	// ...and also if this could result from a User or Member Listing
	bQueryReply = m_listQuery.bFindCellFromData(1,
												g_nSearchByAssociatedType,
												(PVOID) &at,
												g_nSearchByName,
												(PVOID) pPrIrcMsg->szParams[1],
												(PVOID*) &pPrQuery,
												NULL);

	if (bQueryReply)
	{
		ASSERT(pPrQuery, "pPrQuery is NULL in CChatSocket::bHandleEndOfNames");
		qt = (enumQueryType) (DWORD) pPrQuery->pvData;
	}

	if (pChannel)
	{
		// We are in that channel

		if (pChannel->bGotMemberList())
		{
			// Is this for a query reply?
			if (bQueryReply)
			{
				// Is this for a Member or User Listing?
				if (qt == qtNamesForUsers)
				{
					// If there is no such channel we have to fire the OnBeginEnumeration first
					if (!m_bFiredUserChanBeginEnum)
						FireBeginEnumeration(NULL, etUsers);

					// Fire the OnEndEnumeration event
					FireEndEnumeration(NULL, etUsers);
					m_bFiredUserChanBeginEnum = FALSE;
				}
				else
				{
					ASSERT(qt == qtNamesForMembers, "Unexpected QueryType in CChatSocket::bHandleEndOfNames");
					// Fire the OnEndEnumeration event
					pChannel->SetStartedMemberList(FALSE);
					FireEndEnumeration(pChannel, etMembers);
				}

				bRet = m_listQuery.bRemoveCell((PVOID) pPrQuery);
				ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatSocket::bHandleEndOfNames");
			}
			else
			{
				m_hrLastError = CC_E_NOTQUERIEDOBJECT;
				return FALSE;
			}
		}
		else
		{
			// We finally got the whole list of members in the channel

			pChannel->SetGotMemberList(TRUE);
			pChannel->SetStartedMemberList(FALSE);
			FireEndEnumeration(pChannel, etInitialMembers);	// virtual function
		}
	}
	else
	{
		// We are not in that channel

		// Is this for a query reply?
		if (bQueryReply)
		{
			ASSERT(qt == qtNamesForUsers, "Unexpected QueryType in CChatSocket::bHandleEndOfNames");
			// If there is no such channel we have to fire the OnBeginEnumeration first
			if (!m_bFiredUserChanBeginEnum)
				FireBeginEnumeration(NULL, etUsers);

			// Fire the OnEndEnumeration event
			FireEndEnumeration(NULL, etUsers);
			m_bFiredUserChanBeginEnum = FALSE;

			bRet = m_listQuery.bRemoveCell((PVOID) pPrQuery);
			ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatSocket::bHandleEndOfNames");
		}
		else
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
	}
	
	return TRUE;
}


BOOL CChatSocket::bHandleQuit(PPRIRCMSG pPrIrcMsg)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleQuit");
	
	// The QUIT message might be related to some other user:
	// ":TAFFY2!~cjking@198.110.237.XXX QUIT :Lost Connection 64"
	// Prefix:'TAFFY2!~cjking@198.110.237.XXX', Command:'QUIT', Param:'Lost Connection 64'
	// or
	// "toto!~nom@france QUIT :EOF from client"
	// Prefix:'toto!~nom@france', Command:'QUIT', Param:'EOF From client'
	  
	// Or, we are quitting -> make sure that we update the connection state
	if (!m_szNickname || 0 == lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname))
		return SUCCEEDED(m_hrLastError = HrPostNewConnectionState(csDisconnected));
	else
	{
		// Go through all the open channels and fire the OnDelMember event whenever the user is in there
		PPRMEMBER			pPrMember = NULL;
		CChatChannel		*pChannel = NULL;
		enumChannelState	chs = chsOpen;
		CHAR				*szNickname, *sz;

		// The first part of the prefix has the nickname of the user who has parted
		sz = szNickname = pPrIrcMsg->szPrefix;
		SkipTillPrefixExtender(&sz);
		*sz = g_chEOS;

		ASSERT(m_lOpenChannels > 0L, "m_lOpenChannels <= 0L in CChatSocket::bHandleQuit");
		for (LONG lPosition = 1L; lPosition <= m_lOpenChannels; lPosition++)
		{
			if (m_listChannel.bFindCellFromData(lPosition,
												g_nSearchByState,
												(PVOID) &chs,
												g_nSearchNone,
												NULL,
												(PVOID*) &pChannel,
												NULL))
			{
				// Got an open channel
				ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleQuit");

				ASSERT(pChannel->bGotMemberList(), "Unexpected Quit before receiving member list in CChatSocket::bHandleQuit");

				// Check if the parting User is part of it
				if (pChannel->bGetMemberFromNickname(szNickname, &pPrMember))
				{
					// We found the dude in here
					ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bHandleQuit");

					// Delete this member from the list of members of this channel
					if (!pChannel->bDelMember(szNickname))
					{
						#ifdef DEBUG
							sprintf(g_szDebugStr, "CChatSocket::bHandleQuit - Couldn't bDelMember parting member '%s'\n", szNickname);
							OutputDebugThreadIdString(g_szDebugStr);
							pChannel->DumpMembers();
						#endif
						m_hrLastError = pChannel->HrGetLastError();
						return FALSE;
					}

					// Announce the user's departure by firing the OnDelMember event
					FireDelMember(pChannel, szNickname);  // virtual function in CMsChatPr and CChatSock
				}
			}
		}
		return TRUE;
	}
}


BOOL CChatSocket::bHandleDataMsg(PPRIRCMSG pPrIrcMsg, SHORT nCmd)
{
	CChatChannel		*pChannel = NULL;
	enumChannelState	chs = chsOpen;
	LPTSTR				szSender, szUserName, szIPAddress;
	BOOL				bAuthUser, bRet;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleDataMsg");
	
	// The sender is in the prefix
	// Three formats:
	//  :SenderNick!UN@IP DATA #Channel MyNick Tag :Data		= Channel multicast
	//  :SenderNick!UN@IP DATA #Channel Tag :Data				= Channel broadcast
	//  :SenderNick!UN@IP DATA MyNick Tag :Data					= Private message

	if (pPrIrcMsg->byteParams < 3)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleDataMsg");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// The sender is in the prefix (it can be the server or a user identity)
	if (g_chEOS != pPrIrcMsg->szPrefix[0])
		ExtractNickUserIPAddress((LPTSTR) pPrIrcMsg->szPrefix, bIsIrcXSocket(), &szSender, &szUserName, &szIPAddress, &bAuthUser);
	else
		szSender = m_szServerName;

	BOOL bSenderIsServer = (0 == ::lstrcmpi(m_szServerName, pPrIrcMsg->szPrefix) || g_chEOS == pPrIrcMsg->szPrefix[0]);

	// Was the msg sent directly to us?
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleDataMsg");
	if (0 == ::lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname))
	{
		// This is a private message - Is the sender ignored?
		BOOL bIgnored;
		bIsUserIgnored(szSender, szUserName, szIPAddress, TRUE, &bIgnored);
		return bIgnored ? TRUE : bFireDataMessage(NULL,						// pChannel
												  szSender,					// szSenderNick
												  NULL,						// pvRcpNicks
												  pPrIrcMsg->szParams[1],	// szTag
												  pPrIrcMsg->szParams[2],	// szData
												  nCmd);
	}

	// Data Msg was sent to a channel. Find out if we are in such a channel
	// Find the first open channel with the given name
	m_listChannel.bFindCellFromData(1L,
									g_nSearchByName,
									(PVOID) pPrIrcMsg->szParams[0],
									g_nSearchByState,
									(PVOID) &chs,
									(PVOID*) &pChannel,
									NULL);
	if (pChannel)
	{
		// we found the channel
		PPRMEMBER	pPrMember = NULL;

		// make sure that the sender (if not server) is in this channel
		if (!bSenderIsServer && 
			!pChannel->bGetMemberFromNickname(szSender, &pPrMember))
		{
			m_hrLastError = E_FAIL;
			return FALSE;	// we just expose it as a protocol message
		}

		// check if the sender is ignored by this user using the pPrMember structure
		if (pPrMember && (pPrMember->lModes & mmClientIgnored))
			return TRUE;	// we just ignore this incoming message

		VARIANT	vRcpNicks;

		VariantInit(&vRcpNicks);

		if (4 == pPrIrcMsg->byteParams)
		{
			ASSERT(0 == ::lstrcmpi(pPrIrcMsg->szParams[1], m_szNickname), "Unexpected recipient in CChatSocket::bHandleDataMsg");
			ASSERT(m_bstrNickname, "m_bstrNickname is NULL in CChatSocket::bHandleDataMsg");
			
			vRcpNicks.vt = VT_BSTR;
			vRcpNicks.bstrVal = m_bstrNickname;

			// Fire the OnMessage event - Multicast case
			bRet = bFireDataMessage(pChannel,
									szSender,
									&vRcpNicks,
									pPrIrcMsg->szParams[2],
									pPrIrcMsg->szParams[3],
									nCmd);
		}
		else
			// Fire the OnMessage event - Broadcast case
			bRet = bFireDataMessage(pChannel,
									szSender,
									&vRcpNicks,
									pPrIrcMsg->szParams[1],
									pPrIrcMsg->szParams[2],
									nCmd);
		return bRet;
	}
	else
	{
		ASSERT(FALSE, "Couldn't find channel in CChatSocket::bHandleDataMsg");
		m_hrLastError = E_FAIL;
		return FALSE;
	}
}

	
BOOL CChatSocket::bHandleTextMsg(PPRIRCMSG pPrIrcMsg, SHORT nCmd)
{
	CChatChannel		*pChannel = NULL;
	enumChannelState	chs = chsOpen;
	LPTSTR				szSender, szUserName, szIPAddress;
	BOOL				bAuthUser;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleTextMsg");
	
	// The recipient is in pPrIrcMsg->szParams[0]
	// The sent text is in pPrIrcMsg->szParams[1]
	if (pPrIrcMsg->byteParams < 2)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleTextMsg");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// The sender is in the prefix (it can be the server name, or a channel name). If no prefix, the sender must be the server itself
	if (g_chEOS != pPrIrcMsg->szPrefix[0])
		ExtractNickUserIPAddress((LPTSTR) pPrIrcMsg->szPrefix, bIsIrcXSocket(), &szSender, &szUserName, &szIPAddress, &bAuthUser);
	else
		szSender = m_szServerName;

	BOOL bSenderIsServer = (0 == ::lstrcmpi(m_szServerName, pPrIrcMsg->szPrefix) || g_chEOS == pPrIrcMsg->szPrefix[0]);

	// Was the msg sent directly to us?
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleTextMsg");
	if (0 == ::lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname) ||
		0 == ::lstrcmpi(pPrIrcMsg->szParams[0], "AUTH") ||
		0 == ::lstrcmpi(pPrIrcMsg->szParams[0], "*"))	// This is a Sysop broadcast
	{
		if (bSenderIsServer)
		{
			// treat it as a direct msg from the server
			FireServerTextMessage((cmdidNotice == nCmd) ? smtNotice : smtNormal, pPrIrcMsg->szParams[1]);
			return TRUE;
		}
		else
		{
			// there is a specific sender - this is a private message
			// is the sender ignored?
			BOOL bIgnored;
			bIsUserIgnored(szSender, szUserName, szIPAddress, TRUE, &bIgnored);
			return bIgnored ? TRUE : bFireTextMessage(NULL, szSender, NULL, pPrIrcMsg->szParams[1], 0L, (cmdidNotice == nCmd) ? pmtNotice : pmtNormal);
		}
	}

	// Msg was sent to a channel. Find out if we are in such a channel
	// Find the first open channel with the given name
	m_listChannel.bFindCellFromData(1L,
									g_nSearchByName,
									(PVOID) pPrIrcMsg->szParams[0],
									g_nSearchByState,
									(PVOID) &chs,
									(PVOID*) &pChannel,
									NULL);
	
	if (pChannel)
	{
		// we found the channel
		PPRMEMBER	pPrMember = NULL;

		// make sure that the sender (if not server or channel) is in this channel
		if (!bSenderIsServer && 
			0 != lstrcmpi(szSender, pChannel->m_prChannel.szName) &&
			!pChannel->bGetMemberFromNickname(szSender, &pPrMember))
		{
			m_hrLastError = E_FAIL;
			return FALSE;	// we just expose it as a protocol message
		}

		// check if the sender (if not server) is ignored by this user using the pPrMember structure
		if (pPrMember && (pPrMember->lModes & mmClientIgnored))
			return TRUE;	// we just ignore this incoming message

		// fire the OnMessage event
		return bFireTextMessage(pChannel, szSender, NULL, pPrIrcMsg->szParams[1], (cmdidNotice == nCmd) ? msgtNotice : msgtNormal);
	}
	else
	{
		ASSERT(FALSE, "Couldn't find channel in CChatSocket::bHandleTextMsg");
		m_hrLastError = E_FAIL;
		return FALSE;
	}
}

	
BOOL CChatSocket::bHandleErrorMsg(PPRIRCMSG pPrIrcMsg)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleErrorMsg");
	ASSERT(g_chEOS == pPrIrcMsg->szPrefix[0], "g_chEOS != pPrIrcMsg->szPrefix[0] in CChatSocket::bHandleErrorMsg");
	
	// The error description is in pPrIrcMsg->szParams[0]
	if (pPrIrcMsg->byteParams != 1)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleErrorMsg");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	FireServerTextMessage(smtError, pPrIrcMsg->szParams[0]);
	return TRUE;
}


BOOL CChatSocket::bHandlePropChanged(PPRIRCMSG pPrIrcMsg)
{
	CHAR					*szChannel, *szPropName, **pszPropOldValue, *szPropNewValue, *szNickname, *sz;
	CChatChannel			*pChannel = NULL;
	enumChannelState		chs = chsOpen;
	enumChannelPropertyName	cpnProp;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandlePropChanged");

	if (pPrIrcMsg->byteParams < 3)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandlePropChanged");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// Prefix contains full name of user who changed the property MsChaTst!~regisb@157.55.102.164 or the server name
	if (0 != ::lstrcmpi(pPrIrcMsg->szPrefix, m_szServerName))
	{
		// So the first part of the prefix is the nickname of the modifier user
		sz = szNickname = pPrIrcMsg->szPrefix;
		SkipTillPrefixExtender(&sz);
		*sz = g_chEOS;
	}
	else
		// REGISB: Not sure if for this command the prefix can be the server name. Ask KentCe
		szNickname = NULL;

	// Channel name is in the first parameter pPrIrcMsg->szParams[0]
	// Property name is in the second parameter pPrIrcMsg->szParams[1]
	// Property value is in the third parameter pPrIrcMsg->szParams[2]

	szChannel		= pPrIrcMsg->szParams[0];
	szPropName		= pPrIrcMsg->szParams[1];
	szPropNewValue	= pPrIrcMsg->szParams[2];

	ASSERT(szChannel, "szChannel is NULL in CChatSocket::bHandlePropChanged");
	ASSERT(szPropName, "szPropName is NULL in CChatSocket::bHandlePropChanged");
	ASSERT(szPropNewValue, "szPropNewValue is NULL in CChatSocket::bHandlePropChanged");

	// Get the internal channel property code from the IRCX property name
	cpnProp = (enumChannelPropertyName) NGetChannelPropertyIndex(szPropName);
	if (-1 == cpnProp)
	{
		ASSERT(FALSE, "Unexpected channel property name in CChatSocket::bHandlePropChanged");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// Let's try to find this channel
	m_listChannel.bFindCellFromData(1,
									g_nSearchByName,
									(PVOID) szChannel,
									g_nSearchByState,
									(PVOID) &chs,
									(PVOID*) &pChannel,
									NULL);
	if (pChannel)
	{
		// we found the open channel and fire the OnChannelPropertyChanged event if the prop really changed
		VARIANT vOldChannelProperty, vNewChannelProperty;

		VariantInit(&vOldChannelProperty);
		VariantInit(&vNewChannelProperty);

		switch (cpnProp)
		{
			case cpnLag:
				vOldChannelProperty.vt = vNewChannelProperty.vt = VT_I4;
				vOldChannelProperty.lVal = pChannel->m_prChannel.lLag;
				vNewChannelProperty.lVal = SzToL(szPropNewValue);
				if (vOldChannelProperty.lVal != vNewChannelProperty.lVal)
				{
					// Update the internal variable
					pChannel->m_prChannel.lLag = vNewChannelProperty.lVal;
					return bFireChannelPropertyChanged(pChannel, szNickname, g_rgwszChannelPropertyNames[cpnProp], &vOldChannelProperty, &vNewChannelProperty);
				}
				return TRUE;

			default:
				vOldChannelProperty.vt = vNewChannelProperty.vt = VT_BSTR;
				if (cpnProp == cpnOnJoin || cpnProp == cpnOnPart)
					bLowLevelUnquoting(g_chLLQuoteIRCX, FALSE /*bTreatAsByteArray*/, szPropNewValue, szPropNewValue);	// "\n" --> '\n'
				vNewChannelProperty.bstrVal = A2BSTR(szPropNewValue);
				break;
		}

		switch (cpnProp)
		{
			case cpnAccount:
				pszPropOldValue = &pChannel->m_prChannel.szAccount;
				break;
			case cpnClientData:
				pszPropOldValue = &pChannel->m_prChannel.szClientData;
				break;
			case cpnClientGuid:
				pszPropOldValue = &pChannel->m_prChannel.szClientGuid;
				break;
			case cpnHostKey:
				pszPropOldValue = &pChannel->m_prChannel.szHostKey;
				break;
			case cpnLanguage:
				pszPropOldValue = &pChannel->m_prChannel.szLanguage;
				break;
			case cpnOnJoin:
				pszPropOldValue = &pChannel->m_prChannel.szOnJoin;
				break;
			case cpnOnPart:
				pszPropOldValue = &pChannel->m_prChannel.szOnPart;
				break;
			case cpnOwnerKey:
				pszPropOldValue = &pChannel->m_prChannel.szOwnerKey;
				break;
			case cpnRating:
				pszPropOldValue = &pChannel->m_prChannel.szRating;
				break;
			case cpnServicePath:
				pszPropOldValue = &pChannel->m_prChannel.szServicePath;
				break;
			case cpnSubject:
				pszPropOldValue = &pChannel->m_prChannel.szSubject;
				break;

			default:
				ASSERT(FALSE, "Unexpected property change in CChatSocket::bHandlePropChanged");
				m_hrLastError = CC_E_SERVER;
				return FALSE;
		}

		vOldChannelProperty.bstrVal = *pszPropOldValue ? A2BSTR(*pszPropOldValue) : SysAllocString(L"");

		if (VT_BSTR == vNewChannelProperty.vt && (!vNewChannelProperty.bstrVal || !vOldChannelProperty.bstrVal))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}

		// Has the property really changed?
		if ((!*pszPropOldValue && szPropNewValue[0] == g_chEOS) ||
			(*pszPropOldValue && 0 == lstrcmp(*pszPropOldValue, szPropNewValue)))
		{
			// Property is unchanged 
			VariantClear(&vOldChannelProperty);
			VariantClear(&vNewChannelProperty);
			return TRUE;
		}

		// Update the internal property for this channel
		if (!bCopyStr(pszPropOldValue, szPropNewValue))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}

		// Finally fire the OnChannelPropertyChanged event
		BOOL bRet = bFireChannelPropertyChanged(pChannel, szNickname, g_rgwszChannelPropertyNames[cpnProp], &vOldChannelProperty, &vNewChannelProperty);

		VariantClear(&vOldChannelProperty);
		VariantClear(&vNewChannelProperty);
		return bRet;
	}
	else
	{
		ASSERT(FALSE, "Couldn't find channel in CChatSocket::bHandlePropChanged");
		m_hrLastError = E_FAIL;
		return FALSE;
	}
}


BOOL CChatSocket::bHandlePropReply(PPRQUERY pPrQry, PPRIRCMSG pPrIrcMsg, UINT uCode)
{
	static PPRQUERY			pPrLatestQuery = NULL;
	PPRQUERY				pPrQuery = pPrQry;
	PPRCHANNEL				pPrChannel = NULL;
	CChatChannel			*pChannel = NULL;
	enumChannelState		chs = chsOpen;
	enumAssociatedType		at = atChannel;
	enumChannelPropertyName	cpnProp;
	LPTSTR					szChannelName, szPropName, szPropValue, *pszPropValue = NULL;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandlePropReply");

	if (m_lPropInProgress <= 0L)
	{
		m_hrLastError = CC_E_NOTQUERIEDOBJECT;
		return FALSE;
	}

	ASSERT(0 == lstrcmpi(m_szServerName, pPrIrcMsg->szPrefix), "Prefix != ServerName in CChatSocket::bHandlePropReply");
	ASSERT(0 == lstrcmpi(m_szNickname, pPrIrcMsg->szParams[0]), "Params[0] != Nickname in CChatSocket::bHandlePropReply");

	szChannelName = pPrIrcMsg->szParams[1];

	if (RPL_PROPEND == uCode)
	{
		m_lPropInProgress--;
		ASSERT(m_lPropInProgress >= 0L, "m_lPropInProgress < 0L in CChatSocket::bHandlePropReply - RPL_PROPEND");

		// Remove the pPrQuery object from the query list
		if (!pPrQuery)
			pPrQuery = pPrLatestQuery;

		ASSERT(!pPrLatestQuery || pPrQuery == pPrLatestQuery, "pPrLatestQuery && pPrQuery != pPrLatestQuery in CChatSocket::bHandlePropReply");
		pPrLatestQuery = NULL;

		if (!pPrQuery)
		{
			// This is an asynchronous query - Get the latest cell with type atChannel and channel name szChannelName
			if (!m_listQuery.bFindCellFromData(1L,
											   g_nSearchByAssociatedType,
											   (PVOID) &at,
											   g_nSearchByName,
											   szChannelName,
											   (PVOID*) &pPrQuery,
											   NULL))
			{
				ASSERT(FALSE, "Could not find query cell in CChatSocket::bHandlePropReply");
				m_hrLastError = E_FAIL;
				return FALSE;
			}
		}
		ASSERT(pPrQuery, "pPrQuery is NULL in CChatSocket::bHandlePropReply");

		// The whole property is here, we can expose it to the user
		if (!pPrQuery->bSyncAccess)
			return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_CHANNELPROP, (WPARAM) pPrQuery, (LPARAM) TRUE));

		return TRUE;
	}

	//	prefix:		Server name (KEEZER)
	//	command:	RPL_PROPLIST = 815
	//	params:		Object name (#Newbies)
	//				Property name (ONJOIN)
	//				Property value (Welcome to the #newbies channel!)

	// There should be at least 4 parameters
	if (pPrIrcMsg->byteParams < 4)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandlePropReply");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	szPropName		= pPrIrcMsg->szParams[2];
	szPropValue		= pPrIrcMsg->szParams[3];

	ASSERT(szChannelName, "szChannelName is NULL in CChatSocket::bHandlePropReply");
	ASSERT(szPropName, "szPropName is NULL in CChatSocket::bHandlePropReply");
	ASSERT(szPropValue, "szPropValue is NULL in CChatSocket::bHandlePropReply");

	if (pPrQuery)
	{
		pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
		ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bHandlePropReply");
		ASSERT(pPrChannel->szName, "pPrChannel->szName is NULL in CChatSocket::bHandlePropReply");
		if (0 != lstrcmpi(szChannelName, pPrChannel->szName))
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
	}
	else
	{
		// Asynchronous access - let's try to find the query cell in the list

		// We only expect PROP for a channel at this point - might have to compute 'at' from the property name
		if (!m_listQuery.bFindCellFromData(1L,
										   g_nSearchByAssociatedType,
										   (PVOID) &at,
										   g_nSearchByName,
										   szChannelName,
										   (PVOID*) &pPrQuery,
										   NULL))
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
		ASSERT(pPrQuery, "pPrQuery is NULL in CChatSocket::bHandlePropReply");
		pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
		ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bHandlePropReply");
		ASSERT(pPrChannel->szName, "pPrMember->szNickname is NULL in CChatSocket::bHandlePropReply");
	}

	ASSERT(RPL_PROPLIST == pPrQuery->uHeadReplyCode, "RPL_PROPLIST != pPrQuery->uHeadReplyCode in CChatSocket::bHandlePropReply");

	// pPrChannel can be a channel where we're not in

	// Get the internal channel property code from the IRCX property name
	cpnProp = (enumChannelPropertyName) NGetChannelPropertyIndex(szPropName);
	if (-1 == cpnProp)
	{
		ASSERT(FALSE, "Unexpected channel property name in CChatSocket::bHandlePropReply");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	switch (cpnProp)
	{
		case cpnCreationTime:
			pPrChannel->lCreationTime = SzToL(szPropValue);
			break;
		case cpnLag:
			pPrChannel->lLag = SzToL(szPropValue);
			break;
		case cpnObjectId:
			pPrChannel->lOID = HexSzToL(szPropValue);
			break;
		case cpnAccount:
			pszPropValue = &pPrChannel->szAccount;
			break;
		case cpnClientData:
			pszPropValue = &pPrChannel->szClientData;
			break;
		case cpnClientGuid:
			pszPropValue = &pPrChannel->szClientGuid;
			break;
		case cpnHostKey:
			pszPropValue = &pPrChannel->szHostKey;
			break;
		case cpnLanguage:
			pszPropValue = &pPrChannel->szLanguage;
			break;
		case cpnOnJoin:
			pszPropValue = &pPrChannel->szOnJoin;
			bLowLevelUnquoting(g_chLLQuoteIRCX, FALSE /*bTreatAsByteArray*/, szPropValue, szPropValue);	// "\n" --> '\n'
			break;
		case cpnOnPart:
			pszPropValue = &pPrChannel->szOnPart;
			bLowLevelUnquoting(g_chLLQuoteIRCX, FALSE /*bTreatAsByteArray*/, szPropValue, szPropValue);	// "\n" --> '\n'
			break;
		case cpnOwnerKey:
			pszPropValue = &pPrChannel->szOwnerKey;
			break;
		case cpnRating:
			pszPropValue = &pPrChannel->szRating;
			break;
		case cpnServicePath:
			pszPropValue = &pPrChannel->szServicePath;
			break;
		case cpnSubject:
			pszPropValue = &pPrChannel->szSubject;
			break;
		default:
			ASSERT(FALSE, "Unexpected property change in CChatSocket::bHandlePropReply");
			m_hrLastError = CC_E_SERVER;
			return FALSE;
	}

	if (pszPropValue)
	{
		if (!bCopyStr(pszPropValue, szPropValue))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
	}

	pPrLatestQuery = pPrQuery;

	return TRUE;
}


BOOL CChatSocket::bHandleTopic(PPRIRCMSG pPrIrcMsg)
{
	CHAR				*szChannel, *szTopic, *szNickname, *sz, *szOldTopic;
	CChatChannel		*pChannel = NULL;
	enumChannelState	chs = chsOpen;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleTopic");

	// Prefix contains full name of member who changed the topic MsChaTst!~regisb@157.55.102.164 or the server name
	if (0 != ::lstrcmpi(pPrIrcMsg->szPrefix, m_szServerName))
	{
		// So the first part of the prefix is the nickname of the member
		sz = szNickname = pPrIrcMsg->szPrefix;
		SkipTillPrefixExtender(&sz);
		*sz = g_chEOS;

		// Channel name is in the first parameter pPrIrcMsg->szParams[0]
		// Channel topic is in the second parameter pPrIrcMsg->szParams[1]
		if (pPrIrcMsg->byteParams < 2)
		{
			ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleTopic");
			m_hrLastError = CC_E_SERVER;
			return FALSE;
		}

		szChannel = pPrIrcMsg->szParams[0];
		szTopic	  = pPrIrcMsg->szParams[1];
	}
	else
	{
		szNickname = NULL;


		// Channel name is in the second parameter pPrIrcMsg->szParams[1]
		// Channel topic is in the third parameter pPrIrcMsg->szParams[2]
		if (pPrIrcMsg->byteParams < 3)
		{
			ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleTopic");
			m_hrLastError = CC_E_SERVER;
			return FALSE;
		}

		szChannel = pPrIrcMsg->szParams[1];
		szTopic	  = pPrIrcMsg->szParams[2];
	}

	ASSERT(szChannel, "szChannel is NULL in CChatSocket::bHandleTopic");
	ASSERT(szTopic, "szTopic is NULL in CChatSocket::bHandleTopic");

	// Let's try to find this channel
	m_listChannel.bFindCellFromData(1,
									g_nSearchByName,
									(PVOID) szChannel,
									g_nSearchByState,
									(PVOID) &chs,
									(PVOID*) &pChannel,
									NULL);
	if (pChannel)
	{
		szOldTopic = (LPTSTR) pChannel->SzGetChannelTopic();
		ASSERT(szOldTopic, "szOldTopic is NULL in CChatSocket::bHandleTopic");

		if (lstrcmp(szOldTopic, szTopic))
		{
			// we found the channel and fire the OnChannelPropertyChanged event
			VARIANT vOldChannelProperty, vNewChannelProperty;

			VariantInit(&vOldChannelProperty);
			VariantInit(&vNewChannelProperty);

			if (g_chExtChnPfx == szChannel[0] && !pChannel->bMicOnly())
			{
				// Need to convert UTF8 extended channel topic to Unicode
				LPWSTR	wszChannelTopic;
				
				if (g_chEOS != szTopic[0])
				{
					if (!bConvertUTF8StringToWide(szTopic, 0 /*cchIn*/, &wszChannelTopic, NULL /*pcchOut*/, FALSE /*bNickname*/, FALSE /*bChannelName*/, FALSE /*bPostProcess*/))
					{
						m_hrLastError = E_OUTOFMEMORY;
						return FALSE;
					}

					ASSERT(wszChannelTopic, "wszChannelTopic in NULL in CChatSocket::bHandleTopic");

					vNewChannelProperty.bstrVal = SysAllocString(wszChannelTopic);
					delete [] wszChannelTopic;
				}
				else
					vNewChannelProperty.bstrVal = SysAllocString(L"");

				if (g_chEOS != szOldTopic[0])
				{
					if (!bConvertUTF8StringToWide(szOldTopic, 0 /*cchIn*/, &wszChannelTopic, NULL /*pcchOut*/, FALSE /*bNickname*/, FALSE /*bChannelName*/, FALSE /*bPostProcess*/))
					{
						m_hrLastError = E_OUTOFMEMORY;
						return FALSE;
					}

					ASSERT(wszChannelTopic, "wszChannelTopic in NULL in CChatSocket::bHandleTopic");

					vOldChannelProperty.bstrVal = SysAllocString(wszChannelTopic);
					delete [] wszChannelTopic;
				}
				else
					vOldChannelProperty.bstrVal = SysAllocString(L"");
			}
			else
			{
				LPTSTR	szTmp;
				BOOL	bFreeConverted;

				// Might have to convert topic from Jis to ShiftJis, etc...
				if (!bConvertString(TRUE, GetCharSet(), szTopic, 0, &szTmp, NULL, &bFreeConverted))
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}

				vNewChannelProperty.bstrVal = A2BSTR(szTmp);

				if (bFreeConverted)
					delete [] szTmp;

				if (!bConvertString(TRUE, GetCharSet(), szOldTopic, 0, &szTmp, NULL, &bFreeConverted))
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}

				vOldChannelProperty.bstrVal = A2BSTR(szTmp);

				if (bFreeConverted)
					delete [] szTmp;
			}

			vOldChannelProperty.vt = vNewChannelProperty.vt = VT_BSTR;

			if (!vNewChannelProperty.bstrVal || !vOldChannelProperty.bstrVal)
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}

			// update the internal topic for this channel
			if (!pChannel->bSetChannelTopic(szTopic))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}

			BOOL bRet = bFireChannelPropertyChanged(pChannel, szNickname, g_rgwszChannelPropertyNames[cpnTopic], &vOldChannelProperty, &vNewChannelProperty);

			VariantClear(&vOldChannelProperty);
			VariantClear(&vNewChannelProperty);
			return bRet;
		}
		else
			// REGISB: Do we want to expose this command in an OnProtocolMessage event?
			return TRUE;	// Topic hasn't really changed
	}
	else
	{
		ASSERT(FALSE, "Couldn't find channel in CChatSocket::bHandleTopic");
		m_hrLastError = E_FAIL;
		return FALSE;
	}
}

	
BOOL CChatSocket::bHandleModeCommand(PPRIRCMSG pPrIrcMsg)
{
	LPTSTR				szChannelName, szModifierNickname, szModifiedNickname, szModes, sz;
	PPRMEMBER			pPrMember = NULL;
	CChatChannel*		pChannel = NULL;
	enumChannelState	chs = chsOpen;
	LONG				lModes = 0L, lOldModes = 0L;
	VARIANT				vOldProperty, vNewProperty;
	BOOL				bRet = TRUE;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleModeCommand");

	//	prefix:		Full source member identity (justin!regisb@157.55.102.164)
	//  command:	MODE
	//  params:		Channel name (#MsChatPr) or User nickname (prout)
	//				Mode (+o or +l, or -w)
	//				Target member nickname or new max member list, or key	(zorg or 100 or Paradis)

	// There should be at least 2 parameters
	if (pPrIrcMsg->byteParams < 2)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleModeCommand");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	VariantInit(&vOldProperty);
	VariantInit(&vNewProperty);

	szChannelName = szModifierNickname = szModifiedNickname = szModes = NULL;

	ASSERT(cmdidMode == NGetCmd(pPrIrcMsg->szCommand), "Command != MODE in CChatSocket::bHandleModeCommand");

	sz = szModifierNickname = pPrIrcMsg->szPrefix;
	SkipTillPrefixExtender(&sz);
	*sz = g_chEOS;

	szChannelName = pPrIrcMsg->szParams[0];	// this could also be a user nickname
	szModes = pPrIrcMsg->szParams[1];
	if (pPrIrcMsg->byteParams > 2)
		szModifiedNickname = pPrIrcMsg->szParams[2];	// this could also be the new max member count or a key

	ASSERT(szChannelName, "szChannelName is NULL in CChatSocket::bHandleModeCommand");
	ASSERT(szModes, "szModes is NULL in CChatSocket::bHandleModeCommand");

	// Get the channel object if any
	m_listChannel.bFindCellFromData(1,
									g_nSearchByName,
									(PVOID) szChannelName,
									g_nSearchByState,
									(PVOID) &chs,
									(PVOID*) &pChannel,
									NULL);
	
	if (!pChannel)
	{
		if (0 == lstrcmpi(szChannelName, m_szNickname))
		{
			// Our own User modes changed
			lOldModes = lModes = m_lUserModes;
			ApplyIRCToOurUserMode(szModes, &lModes);
			// Did anything actually change?
			if (0L != (lOldModes ^ lModes))
			{
				// Update the user's internal modes
				m_lUserModes = lModes;
				// Fire OnUserPropertyChanged(BSTR Nickname, BSTR UserPropertyName, VARIANT OldUserProperty, VARIANT NewUserProperty)
				vOldProperty.vt = vNewProperty.vt = VT_I4;

				vOldProperty.lVal = lOldModes;
				vNewProperty.lVal = lModes;
				bRet = bFireUserPropertyChanged(m_szNickname, NULL, g_rgwszUserPropertyNames[pnUserModes], &vOldProperty, &vNewProperty);
				ASSERT(bRet, "bFireUserPropertyChanged failed in CChatSocket::bHandleModeCommand");
				return bRet;
			}
			else
				return TRUE;	// Nothing has changed for the caller, waste of traffic!
		}
		else
		{
			// This is for another user mode change - We don't expect that yet
			OutputDebugThreadIdString("CChatSocket::bHandleModeCommand - User Mode change not forwarded\n");
			ASSERT(FALSE, "CChatSocket::bHandleModeCommand - User Mode change not forwarded");
			m_hrLastError = CC_E_SERVER;
			return FALSE;	// REGISB: still to be done?  Can this ever happen??
		}
	}

	// Is the mode applied to a channel or member?
	// Try to get the member given his nickname?
	if (szModifiedNickname && NULL == SzSubStr(szModes, "k"))
	{
		pChannel->bGetMemberFromNickname(szModifiedNickname, &pPrMember);
		ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleModeCommand");
		if (!pPrMember && 0 == lstrcmpi(szModifiedNickname, m_szNickname))
			// REGISB: My Modes are changing before my AddMember, we ignore this Mode change
			// - this is happening because because I'm a host of the channel I'm creating
			return TRUE;
	}
	
	if (pPrMember)
	{
		// Member mode change
		lOldModes = lModes = pPrMember->lModes;
		ApplyIRCToOurMemberMode(szModes, &lModes);
		// Did anything actually change?
		if (0L != (lOldModes ^ lModes))
		{
			// Update the member's internal modes
			pPrMember->lModes = lModes;
			// Fire OnMemberPropertyChanged(Channel* Channel, BSTR Nickname, BSTR MemberPropertyName, VARIANT OldMemberProperty, VARIANT NewMemberProperty)
			vOldProperty.vt = vNewProperty.vt = VT_I4;

			vOldProperty.lVal = lOldModes;
			vNewProperty.lVal = lModes;
			bRet = bFireMemberPropertyChanged(pChannel, szModifiedNickname, szModifierNickname, g_rgwszMemberPropertyNames[pnMemberModes], &vOldProperty, &vNewProperty);
		}
	}
	else
	{
		// Channel Mode
		//
		BOOL bMaxMemberCountChanged = FALSE;	// does the mode set the max member count?
		BOOL bKeywordSet = FALSE;
		BOOL bKeywordReset = FALSE;
		
		ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleModeCommand");

		lOldModes = lModes = pChannel->LGetChannelModes();
		ApplyIRCToOurChannelMode(szModes, &lModes, &bMaxMemberCountChanged, &bKeywordSet, &bKeywordReset, &(pChannel->m_prChannel.bMicOnly));
		// Do we need to update the max member count ?
		if (bMaxMemberCountChanged)
		{
			LONG lMaxMemberCount = 0L;

			if (szModifiedNickname)
			{
				lMaxMemberCount = SzToL(szModifiedNickname);
				ASSERT(lMaxMemberCount > 0L, "lMaxMemberCount == 0L in CChatSocket::bHandleModeCommand");
			}

			vOldProperty.vt = vNewProperty.vt = VT_I4;

			// Get the previous max member count
			vOldProperty.lVal = pChannel->LGetMaxMemberCount();

			// Update the internal variable
			pChannel->SetMaxMemberCount(lMaxMemberCount);

			// Notify user of the change
			// Fire OnChannelPropertyChanged(Channel* Channel, VARIANT ModifierNickname, BSTR ChannelPropertyName, VARIANT NewChannelProperty);
			vNewProperty.lVal = lMaxMemberCount;

			if (vOldProperty.lVal != vNewProperty.lVal)
				bRet = bFireChannelPropertyChanged(pChannel, szModifierNickname, g_rgwszChannelPropertyNames[cpnMaxMemberCount], &vOldProperty, &vNewProperty);
		}
		if (bKeywordSet)
		{
			if (!pChannel->m_prChannel.szKeyword || 0 != lstrcmp(pChannel->m_prChannel.szKeyword, szModifiedNickname))
			{
				vOldProperty.vt = vNewProperty.vt = VT_BSTR;

				vOldProperty.bstrVal = pChannel->m_prChannel.szKeyword ? A2BSTR(pChannel->m_prChannel.szKeyword) : SysAllocString(L"");
				vNewProperty.bstrVal = A2BSTR(szModifiedNickname);

				if (!vOldProperty.bstrVal || !vNewProperty.bstrVal)
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}

				if (!bCopyStr(&(pChannel->m_prChannel.szKeyword), szModifiedNickname))
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}

				// Fire the OnChannelPropertyChanged event
				bRet = bFireChannelPropertyChanged(pChannel, szModifierNickname, g_rgwszChannelPropertyNames[cpnKeyword], &vOldProperty, &vNewProperty);

				VariantClear(&vOldProperty);
				VariantClear(&vNewProperty);
			}
		}
		if (bKeywordReset)
		{
			vOldProperty.vt = vNewProperty.vt = VT_BSTR;

			vOldProperty.bstrVal = A2BSTR(szModifiedNickname);
			vNewProperty.bstrVal = SysAllocString(L"");

			if (!vOldProperty.bstrVal || !vNewProperty.bstrVal)
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}

			bCopyStr(&(pChannel->m_prChannel.szKeyword), g_szEmpty);

			// Fire the OnChannelPropertyChanged event
			bRet = bFireChannelPropertyChanged(pChannel, szModifierNickname, g_rgwszChannelPropertyNames[cpnKeyword], &vOldProperty, &vNewProperty);

			VariantClear(&vOldProperty);
			VariantClear(&vNewProperty);
		}

		if (0L != (lOldModes ^ lModes))
		{
			pChannel->SetChannelModes(lModes);
			// Notify user of the change
			// Fire OnChannelPropertyChanged(Channel* Channel, VARIANT ModifierNickname, BSTR ChannelPropertyName, VARIANT NewChannelProperty);
			vOldProperty.vt = vNewProperty.vt = VT_I4;
		
			vOldProperty.lVal = lOldModes;
			vNewProperty.lVal = lModes;
			if (!bFireChannelPropertyChanged(pChannel, szModifierNickname, g_rgwszChannelPropertyNames[cpnModes], &vOldProperty, &vNewProperty))
				return FALSE;
		}

		// If the moderated flag was set or reset we need to swap the speaker/speactator privileges
		//if ((lOldModes & cmModerated) != (lModes & cmModerated))
		//	bRet = pChannel->bSwapSpeakersSpectators();
	}

	return bRet;
}


BOOL CChatSocket::bHandleChannelModeIs(PPRQUERY pPrQry, PPRIRCMSG pPrIrcMsg)
{
	LPTSTR				szChannelName, szModes, szMaxMemberCount, szKeyword;
	PPRQUERY			pPrQuery = pPrQry;
	PPRCHANNEL			pPrChannel = NULL;
	CChatChannel		*pChannel = NULL;
	enumChannelState	chs = chsOpen;
	LONG				lModes = 0L;
	BOOL				bRet = TRUE, bKeywordSet = FALSE, bMaxMemberCountChanged = FALSE;	// does the mode set the max member count?

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleChannelModeIs");

	//	prefix:		Server name (KEEZER)
	//	command:	RPL_CHANNELMODEIS = 324
	//	params:		Member nickname (zorg)
	//				Channel name (#MsChatPr)
	//				Mode (+n)

	// There should be at least 3 parameters
	if (pPrIrcMsg->byteParams < 3)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleChannelModeIs");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	if (m_lModeIsInProgress <= 0L)
	{
		m_hrLastError = CC_E_NOTQUERIEDOBJECT;
		return FALSE;
	}

	ASSERT(0 == lstrcmpi(m_szServerName, pPrIrcMsg->szPrefix), "Prefix != ServerName in CChatSocket::bHandleChannelModeIs");
	ASSERT(0 == lstrcmpi(m_szNickname, pPrIrcMsg->szParams[0]), "Params[0] != Nickname in CChatSocket::bHandleChannelModeIs");

	szChannelName = pPrIrcMsg->szParams[1];
	szModes	= pPrIrcMsg->szParams[2];
	switch (pPrIrcMsg->byteParams)
	{
		case 4:
			szMaxMemberCount = szKeyword = pPrIrcMsg->szParams[3];
			break;

		case 5:
			szMaxMemberCount = pPrIrcMsg->szParams[3];
			szKeyword = pPrIrcMsg->szParams[4];
			break;

		default:
			szMaxMemberCount = szKeyword = NULL;
	}

	ASSERT(szChannelName, "szChannelName is NULL in CChatSocket::bHandleChannelModeIs");
	ASSERT(szModes, "szModes is NULL in CChatSocket::bHandleChannelModeIs");

	if (pPrQuery)
	{
		// This is for a synchronous property access
		pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
		ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bHandleChannelModeIs");
		ASSERT(pPrChannel->szName, "pPrChannel->szName is NULL in CChatSocket::bHandleChannelModeIs");
		if (0 != lstrcmpi(szChannelName, pPrChannel->szName))
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
	}
	else
	{
		// Asynchronous access - let's try to find the query cell in the list
		enumAssociatedType	at = atChannel;

		if (!m_listQuery.bFindCellFromData(1L,
										   g_nSearchByAssociatedType,
										   (PVOID) &at,
										   g_nSearchByName,
										   szChannelName,
										   (PVOID*) &pPrQuery,
										   NULL))
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
		pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
		ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bHandleChannelModeIs");
		ASSERT(pPrChannel->szName, "pPrMember->szNickname is NULL in CChatSocket::bHandleChannelModeIs");
	}

	ASSERT(pPrQuery->uHeadReplyCode == RPL_CHANNELMODEIS, "pPrQuery->uHeadReplyCode != RPL_CHANNELMODEIS in CChatSocket::bHandleChannelModeIs");

	// Get the channel object if any
	if (pPrChannel->pChannel)
	{
		m_listChannel.bFindCellFromData(1,
										g_nSearchByName,
										(PVOID) szChannelName,
										g_nSearchByState,
										(PVOID) &chs,
										(PVOID*) &pChannel,
										NULL);
		if (pChannel != (PVOID) pPrChannel->pChannel)
			pChannel = NULL;
	}

	ApplyIRCToOurChannelMode(szModes, &lModes, &bMaxMemberCountChanged, &bKeywordSet, NULL, &(pPrChannel->bMicOnly));
	// Do we need to update the max member count ?
	if (bMaxMemberCountChanged)
	{
		ASSERT(szMaxMemberCount, "szMaxMemberCount is NULL in CChatSocket::bHandleChannelModeIs");

		pPrChannel->lMaxMemberCount = SzToL(szMaxMemberCount);

		ASSERT(pPrChannel->lMaxMemberCount > 0L, "pPrChannel->lMaxMemberCount <= 0L in CChatSocket::bHandleChannelModeIs");
	}
	else
		// The server didn't specify a max member count, so it must be 0 for the default value.
		pPrChannel->lMaxMemberCount = 0L;

	// Do we need to update the keyword ?
	if (bKeywordSet)
	{
		ASSERT(szKeyword, "szKeyword is NULL in CChatSocket::bHandleChannelModeIs");
		bCopyStr(&pPrChannel->szKeyword, szKeyword);
	}
	else
	{
		// The server didn't specify a keyword, so it must be empty for the default value.
		if (pPrChannel->szKeyword)
			bCopyStr(&pPrChannel->szKeyword, g_szEmpty);
	}

	pPrChannel->lModes = lModes;

	// The whole property is here, we can now expose it to the user
	if (!pPrQuery->bSyncAccess)
		bRet = SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_CHANNELPROP, (WPARAM) pPrQuery, (LPARAM) TRUE));

	if (pChannel && !pChannel->bGotChannelModes())
		pChannel->SetGotChannelModes(TRUE);

	m_lModeIsInProgress--;
	ASSERT(m_lModeIsInProgress >= 0L, "m_lModeIsInProgress < 0L in CChatSocket::bHandleChannelModeIs");

	return bRet;
}


BOOL CChatSocket::bHandleBanList(PPRQUERY pPrQry, PPRIRCMSG pPrIrcMsg, UINT uCode)
{
	PPRQUERY	pPrQuery = pPrQry;
	PPRCHANNEL	pPrChannel  = NULL;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleBanList");

	// pPrQuery == NULL means asynchronous access

	// Three possible situations here:
	//	- the user did an asynchronous property access
	//	- the main thread is waiting for the answer during a synchronous access
	//	- this answer comes after a timeout and RPL_ENDOFBANLIST will fire the OnChannelProperty event

	ASSERT(pPrIrcMsg->byteParams > 1, "pPrIrcMsg->byteParams <= 1 in CChatSocket::bHandleBanList");

	// pPrIrcMsg->szParams[0] is the caller's nickname
	ASSERT(0 == lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname), "pPrIrcMsg->szParams[0] != m_szNickname in CChatSocket::bHandleBanList");

	if (m_lBannedListInProgress <= 0L)
	{
		m_hrLastError = CC_E_NOTQUERIEDOBJECT;
		return FALSE;
	}

	if (pPrQuery)
	{
		pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
		ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bHandleBanList");
		ASSERT(pPrChannel->szName, "pPrChannel->szName is NULL in CChatSocket::bHandleBanList");
		if (0 != lstrcmpi(pPrIrcMsg->szParams[1], pPrChannel->szName))
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
	}
	else
	{
		// Asynchronous access - let's try to find the query cell in the list
		enumAssociatedType	at = atChannel;

		if (!m_listQuery.bFindCellFromData(1L,
										   g_nSearchByAssociatedType,
										   (PVOID) &at,
										   g_nSearchByName,
										   pPrIrcMsg->szParams[1],
										   (PVOID*) &pPrQuery,
										   NULL))
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
		pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
		ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bHandleBanList");
		ASSERT(pPrChannel->szName, "pPrChannel->szName is NULL in CChatSocket::bHandleBanList");
	}

	ASSERT(RPL_BANLIST == pPrQuery->uHeadReplyCode, "RPL_BANLIST != pPrQuery->uHeadReplyCode in CChatSocket::bHandleBanList");

	switch (uCode)
	{
		case RPL_BANLIST:
		{
			SAFEARRAYBOUND	sabound;
			VARIANT			vItem;
			HRESULT			hr;
			INT				cch = 0;
			LONG			lUBound;
			CChatItems		*pCItems = NULL;

			// get the current vector size
			ASSERT(pPrIrcMsg->byteParams > 2, "pPrIrcMsg->byteParams <= 2 in CChatSocket::bHandleBanList");
			ASSERT(pPrChannel->psaBannedList, "pPrChannel->psaBannedList is NULL in CChatSocket::bHandleBanList - RPL_BANLIST");
			hr = SafeArrayGetUBound(pPrChannel->psaBannedList, 1, &lUBound);
			ASSERT(SUCCEEDED(hr), "SafeArrayGetUBound failed in CChatSocket::bHandleBanList - RPL_BANLIST");

			//if (pPrChannel->lBannedListCount < 0L)
			//	pPrChannel->lBannedListCount = 0L;

			if ((lUBound+1) == pPrChannel->lBannedListCount)
			{
				// need to expand the vector a bit
				lUBound += g_nMaxBannedList;
				sabound.cElements = lUBound + 1L;
				sabound.lLbound = 0L;

				if (FAILED(m_hrLastError = SafeArrayRedim(pPrChannel->psaBannedList, &sabound)))
					return FALSE;
			}

			if (!bGetChatItemsFromIdentMask(&pCItems, pPrIrcMsg->szParams[2], atMember))
				return FALSE;

			vItem.vt = VT_DISPATCH;
			vItem.pdispVal = pCItems->GetDispatch();

			ASSERT(vItem.pdispVal, "vItem.pdispVal is NULL in CChatSocket::bHandleBanList");

			if (FAILED(m_hrLastError = SafeArrayPutElement(pPrChannel->psaBannedList, &(pPrChannel->lBannedListCount), (PVOID) &vItem)))
			{
				ReleaseChatItems(pCItems);
				(IDispatch*)(vItem.pdispVal)->Release();
				VariantClear(&vItem);
				return FALSE;
			}

			pPrChannel->lBannedListCount++;
			(IDispatch*)(vItem.pdispVal)->Release();
			VariantClear(&vItem);
			break;
		}
		
		case RPL_ENDOFBANLIST:
		{
			// The whole property is here, we can now expose it to the user
			// We have pPrChannel->lBannedListCount VARIANTs (BSTRs) in our safearray
			ASSERT(pPrChannel->psaBannedList, "pPrChannel->psaBannedList is NULL in CChatSocket::bHandleBanList - RPL_ENDOFBANLIST");

			m_lBannedListInProgress--;
			ASSERT(m_lBannedListInProgress >= 0L, "m_lBannedListInProgress < 0L in CChatSocket::bHandleBanList - RPL_ENDOFBANLIST");

			SAFEARRAYBOUND sabound;
			sabound.lLbound = 0L;

			// Let's resize the safe arrays appropriately if necessary
			if (pPrChannel->lBannedListCount > 0L)
			{
				sabound.cElements = pPrChannel->lBannedListCount;

				if (FAILED(m_hrLastError = SafeArrayRedim(pPrChannel->psaBannedList, &sabound)))
					return FALSE;
			}
			//else
			//	pPrChannel->lBannedListCount = 0L;	// We know the list is empty

			if (!pPrQuery->bSyncAccess)
				return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_CHANNELPROP, (WPARAM) pPrQuery, (LPARAM) TRUE));
			break;
		}
		
		default:
		{
			ASSERT(FALSE, "Unexpected reply code in CChatSocket::bHandleBanList");
			m_hrLastError = E_FAIL;
			return FALSE;
		}
	}
	return TRUE;
}


BOOL CChatSocket::bHandleListMsg(PPRQUERY pPrQry, PPRIRCMSG pPrIrcMsg, UINT uCode)
{
	PPRQUERY		pPrQuery = pPrQry;
	PPRCHANNEL		pPrChannel = NULL;
	static PPRQUERY	pPrLatestQuery = NULL;
	static BOOL		bFireEndEnum = FALSE, bFireChannelProp = FALSE, bExpectingOneChannel = FALSE;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleListMsg");
	ASSERT(pPrIrcMsg->byteParams > 0, "pPrIrcMsg->byteParams == 0 in CChatSocket::bHandleListMsg");

	// pPrQuery == NULL means asynchronous access

	// Four possible situations here:
	//	- the user did an asynchronous property access
	//	- the main thread is waiting for the answer during a synchronous access
	//	- this answer comes after a timeout and RPL_LISTEND will fire the OnChannelProperty event
	//  - the user did a ListChannels call

	// pPrIrcMsg->szParams[0] is the caller's nickname
	ASSERT(0 == lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname), "pPrIrcMsg->szParams[0] != m_szNickname in CChatSocket::bHandleListMsg");

	if (m_lListInProgress <= 0L && m_lChannelListingInProgress <= 0L)
	{
		m_hrLastError = CC_E_NOTQUERIEDOBJECT;
		return FALSE;
	}

	if (RPL_LISTSTART == uCode || RPL_LISTXSTART == uCode)
	{
		if (!pPrQuery && 0L == m_listQuery.DwGetCellCount())
		{
			// Fire the OnBeginEnumeration event
			bFireEndEnum = TRUE;
			FireBeginEnumeration(NULL, etChannels);
		}
		else
		{
			// Channel property access situation
			ASSERT(RPL_LISTSTART == uCode, "RPL_LISTSTART != uCode in CChatSocket::bHandleListMsg");
			bExpectingOneChannel = TRUE;
		}
		return TRUE;
	}
		
	if (RPL_LISTEND == uCode || RPL_LISTXEND == uCode || RPL_LISTXTRUNC == uCode)
	{
		if (bFireChannelProp)
		{
			bFireChannelProp = FALSE;
			if (RPL_LISTEND != uCode)
				// This is a Channels Listing situation where we need to expose the last filled up channel
				if (FAILED(m_hrLastError = HrSendEvent(WM_U_CHANNELPROP2, (WPARAM) &m_prChannel, 0L)))
					return FALSE;
		}
		if (bFireEndEnum)
		{
			m_lChannelListingInProgress--;
			ASSERT(m_lChannelListingInProgress >= 0L, "m_lChannelListingInProgress < 0L in CChatSocket::bHandleListMsg - RPL_LIST[X][TRUNC]END");
			bFireEndEnum = FALSE;
			// Fire the OnEndEnumeration event
			FireEndEnumeration(NULL, RPL_LISTXTRUNC == uCode ? etChannelsTruncated : etChannels);
			return TRUE;
		}
		else
		{
			m_lListInProgress--;
			ASSERT(m_lListInProgress >= 0L, "m_lListInProgress < 0L in CChatSocket::bHandleListMsg - RPL_LISTEND");
			if (bExpectingOneChannel)
			{
				bExpectingOneChannel = FALSE; // Back to normal
				// We tried a channel property access, but didn't get any entry
				// which means that the channel does not exist
				if (pPrQuery)
				{
					// Access is synchronous
					m_hrLastError = CC_E_NOSUCHCHANNEL;
					return FALSE;
				}
				else
				{
					// We are in an Asynchronous Property Access situation - let's try to find the query cell in the list
					enumAssociatedType	at = atChannel;

					if (!m_listQuery.bFindCellFromData(1L,
													   g_nSearchByAssociatedType,
													   (PVOID) &at,
													   g_nSearchNone,
													   NULL,
													   (PVOID*) &pPrQuery,
													   NULL))
						// The query cell was already eaten by a 403 error message: No such channel
						return TRUE;
					pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
					ASSERT(RPL_LISTSTART == pPrQuery->uHeadReplyCode, "RPL_LISTSTART != pPrQuery->uHeadReplyCode in CChatSocket::bHandleListMsg");
					HrSendEvent(WM_U_SOCKET_HRESULT, (WPARAM) pPrChannel->szName, (LPARAM) CC_E_NOSUCHCHANNEL);
					// Need to delete the query cell from the list
					BOOL bRet = m_listQuery.bRemoveCell((PVOID) pPrQuery);
					ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatSocket::bHandleListMsg");
				}
			}
			return TRUE;
		}
	}

	// Convert the channel name if necessary
	ASSERT(uCode == RPL_LISTXPICS ||
		   '*' == pPrIrcMsg->szParams[1][0] ||	// on some IRC servers like irc.eskimo.com
		   '+' == pPrIrcMsg->szParams[1][0] ||	// on some IRC servers like irc.dal.net
		   g_chExtChnPfx == pPrIrcMsg->szParams[1][0] ||
		   g_chGblChnPfx == pPrIrcMsg->szParams[1][0] ||
		   g_chLclChnPfx == pPrIrcMsg->szParams[1][0], "szParams[1] is not a channel name in CChatSocket::bHandleListMsg");

	if (pPrQuery)
	{
		pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
		ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bHandleListMsg");
		ASSERT(pPrChannel->szName, "pPrChannel->szName is NULL in CChatSocket::bHandleListMsg");
		if (0 != lstrcmpi(pPrIrcMsg->szParams[1], pPrChannel->szName) && uCode != RPL_LISTXPICS)
		{
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
		}
		bExpectingOneChannel = FALSE;
	}
	else
	{
		if (bFireEndEnum)
		{
			// We are in a Channels Listing situation

			if (RPL_LISTXLIST == uCode && bFireChannelProp)
				// This is a Channels Listing situation where we need to expose the latest filled up channel
				if (FAILED(m_hrLastError = HrSendEvent(WM_U_CHANNELPROP2, (WPARAM) &m_prChannel, 0L)))
					return FALSE;

			#ifdef DEBUG
			if (RPL_LIST == uCode || RPL_LISTXLIST == uCode)
			{
				ASSERT(!m_prChannel.szName, "m_prChannel.szName NOT NULL in CChatSocket::bHandleListMsg");
				ASSERT(!m_prChannel.szTopic, "m_prChannel.szTopic NOT NULL in CChatSocket::bHandleListMsg");
				ASSERT(!m_prChannel.szKeyword, "m_prChannel.szKeyword NOT NULL in CChatSocket::bHandleListMsg");
				ASSERT(!m_prChannel.szHostKey, "m_prChannel.szHostKey NOT NULL in CChatSocket::bHandleListMsg");
				ASSERT(!m_prChannel.szOwnerKey, "m_prChannel.szOwnerKey NOT NULL in CChatSocket::bHandleListMsg");
				ASSERT(!m_prChannel.szRating, "m_prChannel.szRating NOT NULL in CChatSocket::bHandleListMsg");
				ASSERT(!m_prChannel.psaBannedList, "m_prChannel.psaBannedList NOT NULL in CChatSocket::bHandleListMsg");
				// REGISB: there is more ....
			}
			#endif // DEBUG

			bFireChannelProp = TRUE;

			pPrChannel = &m_prChannel;

			if (RPL_LIST == uCode || RPL_LISTXLIST == uCode)
			{
				m_prChannel.lBannedListCount= 0L;
				m_prChannel.lCloneListCount	= 0L;
				m_prChannel.lMemberCount	= -1L;
				m_prChannel.lMaxMemberCount	= -1L;
				m_prChannel.lLag			= -1L;
				m_prChannel.lModes			= cmNone;

				if (!bCopyStr(&(m_prChannel.szName), pPrIrcMsg->szParams[1]))
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}
			}
		}
		else
		{
			// We are in an Asynchronous Property Access situation - let's try to find the query cell in the list
			enumAssociatedType	at = atChannel;

			if (uCode == RPL_LISTXPICS)
				pPrQuery = pPrLatestQuery;
			else
			{
				if (!m_listQuery.bFindCellFromData(1L,
												   g_nSearchByAssociatedType,
												   (PVOID) &at,
												   g_nSearchByName,
												   pPrIrcMsg->szParams[1],
												   (PVOID*) &pPrQuery,
												   NULL))
				{
					m_hrLastError = CC_E_NOTQUERIEDOBJECT;
					return FALSE;
				}
				if (RPL_LISTXLIST == uCode)
					pPrLatestQuery = pPrQuery;
			}
			bExpectingOneChannel = FALSE;
			ASSERT(pPrQuery, "pPrQuery is NULL in CChatSocket::bHandleListMsg");
			pPrChannel = (PPRCHANNEL) pPrQuery->pvData;
			ASSERT(RPL_LISTSTART == pPrQuery->uHeadReplyCode, "RPL_LISTSTART != pPrQuery->uHeadReplyCode in CChatSocket::bHandleListMsg");
		}
	}

	ASSERT(pPrChannel, "pPrChannel is NULL in CChatSocket::bHandleListMsg");
	ASSERT(pPrChannel->szName, "pPrChannel->szName is NULL in CChatSocket::bHandleListMsg");


	switch (uCode)
	{
		case RPL_LIST:
			// MemberCount is in pPrIrcMsg->szParams[2]
			// Topic is in pPrIrcMsg->szParams[3]
			ASSERT(pPrIrcMsg->byteParams > 3, "pPrIrcMsg->byteParams <= 3 in CChatSocket::bHandleListMsg");
			pPrChannel->lMemberCount = SzToL(pPrIrcMsg->szParams[2]);

			// bFillChatItemsFromChannel will do the conversion if needed
			if (!bCopyStr(&(pPrChannel->szTopic), pPrIrcMsg->szParams[3]))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			break;

		case RPL_LISTXLIST:
			// Modes is in pPrIrcMsg->szParams[2]
			// MemberCount is in pPrIrcMsg->szParams[3]
			// MaxMemberCount is in pPrIrcMsg->szParams[4]
			// Topic is in pPrIrcMsg->szParams[5]
			ASSERT(pPrIrcMsg->byteParams > 5, "pPrIrcMsg->byteParams <= 5 in CChatSocket::bHandleListMsg");
			
			ApplyIRCToOurChannelMode(pPrIrcMsg->szParams[2], &(pPrChannel->lModes), NULL, NULL, NULL, &(pPrChannel->bMicOnly));
			
			pPrChannel->lMemberCount = SzToL(pPrIrcMsg->szParams[3]);
			pPrChannel->lMaxMemberCount = SzToL(pPrIrcMsg->szParams[4]);

			// bFillChatItemsFromChannel will do the conversion if needed
			if (!bCopyStr(&(pPrChannel->szTopic), pPrIrcMsg->szParams[5]))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			break;

		case RPL_LISTXPICS:
			// PICS Ratings are in pPrIrcMsg->szParams[1]
			if (!bCopyStr(&(pPrChannel->szRating), pPrIrcMsg->szParams[1]))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			break;

		default:
			ASSERT(FALSE, "Unexpected reply code value in CChatSocket::bHandleListMsg");
			m_hrLastError = CC_E_NOTQUERIEDOBJECT;
			return FALSE;
	}

	if (pPrQuery)
	{
		// This is a Channel Property Access situation
		if (!pPrQuery->bSyncAccess)
			return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_CHANNELPROP, (WPARAM) pPrQuery, (LPARAM) TRUE));
		else
			return TRUE;
	}
	else 
		if (RPL_LIST == uCode)
		{
			// This a Channels Listing situation
			ASSERT(bFireEndEnum, "bFireEndEnum is FALSE in CChatSocket::bHandleListMsg");
			return SUCCEEDED(m_hrLastError = HrSendEvent(WM_U_CHANNELPROP2, (WPARAM) pPrChannel, 0L));
		}
	return TRUE;
}


BOOL CChatSocket::bHandleKnock(PPRIRCMSG pPrIrcMsg)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleKnock");

	CChatChannel*		pChannel = NULL;
	enumChannelState	chs = chsOpen;
	BOOL				bRet = TRUE;
	PRUSER				prUser;
	CChatItems			*pCItems = NULL;
	CHAR				*szNickname, *szUserName, *szIPAddress;
	BOOL				bAuthUser;

	OutputDebugThreadIdString("CChatSocket::bHandleKick - Enter\n");

	// Need to fire the OnKnock(Channel* Channel, ChatItems* KnockerItems, long ErrorCode) event

	// pPrIrcMsg->szPrefix	  = Knocker identity
	// pPrIrcMsg->szParams[0] = Channel name
	// pPrIrcMsg->szParams[1] = Error code

	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleKnock");

	// The prefix contains the knocker, and there has to be at least 2 params
	if (pPrIrcMsg->byteParams < 2 || g_chEOS == pPrIrcMsg->szPrefix[0])
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleKnock");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	ZeroMemory(&prUser, sizeof(PRUSER));

	ExtractNickUserIPAddress((LPTSTR) pPrIrcMsg->szPrefix, bIsIrcXSocket(), &szNickname, &szUserName, &szIPAddress, &bAuthUser);

	ASSERT(szNickname,  "szNickname is NULL in CChatSocket::bHandleKnock");
	ASSERT(szUserName,  "szUserName is NULL in CChatSocket::bHandleKnock");
	ASSERT(szIPAddress, "szIPAddress is NULL in CChatSocket::bHandleKnock");

	if (!bCopyStr(&(prUser.szNickname),  szNickname) ||
		!bCopyStr(&(prUser.szUserName),  szUserName) ||
		!bCopyStr(&(prUser.szIPAddress), szIPAddress))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}
	prUser.lIdleTime = -1L;
	prUser.lSignOnTime = -1L;
	prUser.lChannelCount = -1L;
	prUser.lChannelOwnerCount = -1L;
	prUser.lChannelHostCount = -1L;
	prUser.lChannelVoiceCount = -1L;
	prUser.lModes += (bAuthUser ? umAuthUser : umNotAuthUser);

	if (!bCreateChatItems(&pCItems))
		goto error;

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bHandleKnock");

	if (!bFillChatItemsFromUser(pCItems, &prUser))
		goto error;

	// Find the channel in our open collection
	if (!m_listChannel.bFindCellFromData(1,
										 g_nSearchByName,
										 (PVOID) pPrIrcMsg->szParams[0],
										 g_nSearchByState,
										 (PVOID) &chs,
										 (PVOID*) &pChannel,
										 NULL))
	{
		m_hrLastError = E_FAIL;
		goto error;
	}

	ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleKnock");

	// Fire the OnKnock event
	FireKnock(pChannel, pCItems, SzToL(pPrIrcMsg->szParams[1]));
	return TRUE;

error:
	FreePrUserContent(&prUser);

	if (pCItems)
		ReleaseChatItems(pCItems);

	return FALSE;
}


BOOL CChatSocket::bHandleKick(PPRIRCMSG pPrIrcMsg)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleKick");

	CChatChannel*		pChannel = NULL;
	enumChannelState	chs = chsOpen;
	CHAR				*sz, *szChannel, *szKicked, *szSubChannel, *szSubKicked, *szKicker, *szReason;
	INT					cch;
	BOOL				bRet = TRUE;

	OutputDebugThreadIdString("CChatSocket::bHandleKick - Enter\n");

	// pPrIrcMsg->szParams[0] = Channel name being exited
	// pPrIrcMsg->szParams[1] = Kicked nickname
	// pPrIrcMsg->szParams[2] = Optional reason of kicking
	// pPrIrcMsg->szPrefix	  = Kicker full identity if any

	// So there should be at least 2 parameters
	if (pPrIrcMsg->byteParams < 2)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleKick");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}
	
	if (g_chEOS != pPrIrcMsg->szPrefix[0])
	{
		// Who is the kicker?
		szKicker = sz = pPrIrcMsg->szPrefix;
		SkipTillPrefixExtender(&sz);
		*sz = g_chEOS;
	}
	else
		szKicker = NULL;

	szChannel = pPrIrcMsg->szParams[0];
	szKicked  = pPrIrcMsg->szParams[1];
	szReason  = (pPrIrcMsg->byteParams > 2) ? pPrIrcMsg->szParams[2] : NULL;

	// NOTE: we might be getting multiple Kicks. So now we need to parse the channels
	// and members for multiple params
	while (bGetNextSubParam(&szChannel, &szSubChannel, &cch) &&	
		   bGetNextSubParam(&szKicked, &szSubKicked, &cch))
	{
		// Find the channel in our open collection
		if (!m_listChannel.bFindCellFromData(1,
											 g_nSearchByName,
											 (PVOID) szSubChannel,
											 g_nSearchByState,
											 (PVOID) &chs,
											 (PVOID*) &pChannel,
											 NULL))
		{
			m_hrLastError = E_FAIL;
			return FALSE;
		}

		ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleKick");
		ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleKick");

		// Delete this member from the list of members of this channel
		if (!pChannel->bDelMember(szSubKicked))
		{
			#ifdef DEBUG
				sprintf(g_szDebugStr, "CChatSocket::bHandleKick - Couldn't bDelMember parting member '%s'\n", szSubKicked);
				OutputDebugThreadIdString(g_szDebugStr);
				pChannel->DumpMembers();
			#endif
			m_hrLastError = pChannel->HrGetLastError();
			return FALSE;
		}

		// Fire OnMemberKicked(Channel* Channel, BSTR KickedNickname, BSTR KickerNickname, BSTR Reason)
		bRet = bFireMemberKicked(pChannel, szSubKicked, szKicker, szReason);

		// Also fire the DelMember event
		FireDelMember(pChannel, szSubKicked);
		
		// Am I leaving by the way?
		if (0 == lstrcmpi(szSubKicked, m_szNickname))
			// It's me :-( Let's update the channel's state and clean up the member list
			if (!bLeftChannel(pChannel))
				return FALSE;
	}

	return bRet;
}


BOOL CChatSocket::bHandleNick(PPRIRCMSG pPrIrcMsg)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleNick");

	CChatChannel*		pChannel = NULL;
	enumChannelState	chs = chsOpen;
	CHAR				*sz, *szOldNickname, *szNewNickname;
	PPRMEMBER			pPrMember = NULL;
	VARIANT				vOldProperty, vNewProperty;
	LPWSTR				wszOldNickname, wszNewNickname;

	// pPrIrcMsg->szPrefix		: full identity of user that is changing nickname
	// pPrIrcMsg->szParams[0]	: new nickname

	sz = szOldNickname = pPrIrcMsg->szPrefix;
	SkipTillPrefixExtender(&sz);
	*sz = g_chEOS;

	if (pPrIrcMsg->byteParams < 1)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleNick");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}
	szNewNickname = pPrIrcMsg->szParams[0];

	ASSERT(g_chEOS != szOldNickname[0], "g_chEOS == szOldNickname[0] is NULL in CChatSocket::bHandleNick");
	ASSERT(szNewNickname, "szNewNickname is NULL in CChatSocket::bHandleNick");
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleNick");

	// First check: is this a BOGUS nick message? This is to get around a bug in some IRC servers that send you
	// a NICK change message even if the nick hasn't really changed
	if (0 == lstrcmp(szOldNickname, szNewNickname))
		return TRUE;

	VariantInit(&vOldProperty);
	VariantInit(&vNewProperty);
	vNewProperty.bstrVal = NULL;

	// Are we changing our own nick?
	if (0 == lstrcmp(szOldNickname, m_szNickname))
	{
		// Save the nick the user is now using
		if (g_chExtNckPfx == szNewNickname[0])
		{
			if (!bConvertUTF8StringToWide(szNewNickname, 0, &wszNewNickname, NULL, TRUE /*bNickname*/) ||
				!bSetNickname(szNewNickname, szNewNickname, (BSTR) wszNewNickname))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			vNewProperty.bstrVal = SysAllocString(wszNewNickname);
			delete [] wszNewNickname;
		}
		else
		{
			vNewProperty.bstrVal = A2BSTR(szNewNickname);
			if (!vNewProperty.bstrVal ||
				!bSetNickname(szNewNickname, NULL, vNewProperty.bstrVal))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
		}
	}

	vOldProperty.vt = vNewProperty.vt = VT_BSTR;

	if (g_chExtNckPfx == szOldNickname[0])
	{
		if (!bConvertUTF8StringToWide(szOldNickname, 0, &wszOldNickname, NULL, TRUE /*bNickname*/))
		{
			m_hrLastError = E_OUTOFMEMORY;
			return FALSE;
		}
		vOldProperty.bstrVal = SysAllocString(wszOldNickname);
		delete [] wszOldNickname;
	}
	else
		vOldProperty.bstrVal = A2BSTR(szOldNickname);

	if (!vNewProperty.bstrVal)
	{
		if (g_chExtNckPfx == szNewNickname[0])
		{
			if (!bConvertUTF8StringToWide(szNewNickname, 0, &wszNewNickname, NULL, TRUE /*bNickname*/))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			vNewProperty.bstrVal = SysAllocString(wszNewNickname);
			delete [] wszNewNickname;
		}
		else
			vNewProperty.bstrVal = A2BSTR(szNewNickname);
	}

	if (!vOldProperty.bstrVal || !vNewProperty.bstrVal)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	// Fire the OnUserPropertyChanged event
	bFireUserPropertyChanged(szOldNickname, NULL, g_rgwszUserPropertyNames[pnUserNickname], &vOldProperty, &vNewProperty);

	if (m_lOpenChannels > 0L)
	{
		// Go through our open channels in the collection and see if this user in there
		for (LONG lPosition = 1L; lPosition <= m_lOpenChannels; lPosition++)
		{
			if (m_listChannel.bFindCellFromData(lPosition,
												g_nSearchByState,
												(PVOID) &chs,
												g_nSearchNone,
												NULL,
												(PVOID*) &pChannel,
												NULL))
			{
				// Got an open channel
				ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleNick");

				// Check if the OldNickname is part of it
				if (pChannel->bGetMemberFromNickname(szOldNickname, &pPrMember))
				{
					// We found the dude in here
					ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bHandleNick");

					// Remove the old member from the hash table...
					if (!pChannel->bDelMember(szOldNickname, FALSE))	// We don't want to free pPrMember!
					{
						#ifdef DEBUG
							sprintf(g_szDebugStr, "CChatSocket::bHandleNick - Couldn't remove renaming member '%s'\n", szOldNickname);
							OutputDebugThreadIdString(g_szDebugStr);
							pChannel->DumpMembers();
						#endif
						m_hrLastError = pChannel->HrGetLastError();
						return FALSE;
					}

					// Update the member's internal nickname
					if (!bCopyStr(&(pPrMember->szNickname), szNewNickname))
					{
						m_hrLastError = E_OUTOFMEMORY;
						return FALSE;
					}

					// ...and add the one with the new nickname
					if (!pChannel->bAddMember(pPrMember))	// Just replace it in the hash table
					{
						#ifdef DEBUG
							sprintf(g_szDebugStr, "CChatSocket::bHandleNick - Couldn't add renaming member '%s'\n", szNewNickname);
							OutputDebugThreadIdString(g_szDebugStr);
							pChannel->DumpMembers();
						#endif
						m_hrLastError = pChannel->HrGetLastError();
						return FALSE;
					}
					// Fire the OnMemberPropertyChanged event
					bFireMemberPropertyChanged(pChannel, szOldNickname, NULL, g_rgwszMemberPropertyNames[pnMemberNickname], &vOldProperty, &vNewProperty);
				}
			}
		}
	}

	VariantClear(&vOldProperty);
	VariantClear(&vNewProperty);

	return TRUE;
}


BOOL CChatSocket::bHandleInviteMsg(PPRIRCMSG pPrIrcMsg)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleInviteMsg");

	// Ignore the event if the user blocks unrated content
	if (!bCanViewUnrated(NULL /*hWnd*/, FALSE /*bPromptOverride*/))
		return TRUE;

	// Need to fire the OnInvitation(BSTR ChannelName, ChatItems* InviterItems) event

	// The prefix contains the inviter, and there has to be at least 2 params
	if (g_chEOS == pPrIrcMsg->szPrefix[0] || pPrIrcMsg->byteParams < 2)
	{
		ASSERT(FALSE, "Unexpected prefix or param number in CChatSocket::bHandleInviteMsg");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// The first param contains the invited nickname (which should be myself)
	// and the second param contains the channel name
	ASSERT(0 == lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname), "pPrIrcMsg->szParams[0] != m_szNickname in CChatSocket::bHandleInviteMsg");

	// pPrIrcMsg->szParams[1] has the channel name

	PRUSER		prUser;
	CChatItems	*pCItems = NULL;
	CHAR		*szNickname, *szUserName, *szIPAddress;
	BOOL		bAuthUser;

	ZeroMemory(&prUser, sizeof(PRUSER));

	ExtractNickUserIPAddress((LPTSTR) pPrIrcMsg->szPrefix, bIsIrcXSocket(), &szNickname, &szUserName, &szIPAddress, &bAuthUser);

	ASSERT(szNickname,  "szNickname is NULL in CChatSocket::bHandleInviteMsg");
	ASSERT(szUserName,  "szUserName is NULL in CChatSocket::bHandleInviteMsg");
	ASSERT(szIPAddress, "szIPAddress is NULL in CChatSocket::bHandleInviteMsg");

	if (!bCopyStr(&(prUser.szNickname),  szNickname) ||
		!bCopyStr(&(prUser.szUserName),  szUserName) ||
		!bCopyStr(&(prUser.szIPAddress), szIPAddress))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto error;
	}
	prUser.lIdleTime = -1L;
	prUser.lSignOnTime = -1L;
	prUser.lChannelCount = -1L;
	prUser.lChannelOwnerCount = -1L;
	prUser.lChannelHostCount = -1L;
	prUser.lChannelVoiceCount = -1L;
	prUser.lModes += (bAuthUser ? umAuthUser : umNotAuthUser);

	if (!bCreateChatItems(&pCItems))
		goto error;

	ASSERT(pCItems, "pCItems is NULL in CChatSocket::bHandleInviteMsg");

	if (!bFillChatItemsFromUser(pCItems, &prUser))
		goto error;

	// Fire the OnInvitation event - bFireInvitation takes care of the channel name conversion
	return bFireInvitation(pPrIrcMsg->szParams[1], pCItems);

error:
	FreePrUserContent(&prUser);

	if (pCItems)
		ReleaseChatItems(pCItems);

	return FALSE;
}


BOOL CChatSocket::bHandleClone(PPRIRCMSG pPrIrcMsg)
{
	HRESULT				hr;
	SAFEARRAYBOUND		sabound;
	VARIANT				vItem;
	CChatChannel		*pChannel = NULL;
	enumChannelState	chs = chsOpen;
	VARIANT				vOldProperty, vNewProperty;
	BOOL				bRet = FALSE;

	// We just got another sheep

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleClone");

	USES_CONVERSION;

	// We should have at least 2 params
	if (pPrIrcMsg->byteParams < 2)
	{
		ASSERT(FALSE, "Unexpected param number in CChatSocket::bHandleClone");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	// Channel clone name in pPrIrcMsg->szParams[0]

	// This channel name is a clone, try to find the channel with the original name
	if (!m_listChannel.bFindCellFromData(1L /*lPosition*/, 
										 g_nSearchByCloneName /*nSearchID1*/,
										 (PVOID) pPrIrcMsg->szParams[0],
										 g_nSearchByState /*nSearchID2*/,
										 (PVOID) &chs,
										 (PVOID*) &pChannel,
										 NULL /*plPositionFound*/))
	{
		m_hrLastError = CC_E_NOTQUERIEDOBJECT;	// Weird! We couldn't find the original channel
		return FALSE;							// This should not happen
	}

	ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleClone");
	ASSERT(pChannel->m_chs == chsOpen, "pChannel->m_chs != chsOpen in CChatSocket::bHandleClone");

	VariantInit(&vOldProperty);
	VariantInit(&vNewProperty);

	if (pChannel->m_prChannel.lCloneListCount > 0L)
	{
		VariantInit(&vItem);

		vItem.vt = (VT_VARIANT+VT_ARRAY);
		vItem.parray = pChannel->m_prChannel.psaCloneList;

		// Let's copy the old clone list into the vOldProperty variant
		if (FAILED(m_hrLastError = VariantCopy(&vOldProperty, &vItem)))
			return FALSE;

		// We need to add the clone to the CloneList property

		#ifdef DEBUG
			LONG lUBound;
			// Get the current vector size
			hr = SafeArrayGetUBound(pChannel->m_prChannel.psaCloneList, 1, &lUBound);
			ASSERT(SUCCEEDED(hr), "SafeArrayGetUBound failed in CChatSocket::bHandleClone");
			ASSERT(lUBound+1 == pChannel->m_prChannel.lCloneListCount, "lUBound+1 != pChannel->m_prChannel.lCloneListCount in CChatSocket::bHandleClone");
		#endif // DEBUG

		// Check if the new clone is already in the list
		LONG lIndex = 0L, lIndex1, lIndexFound = -1L;
		while (lIndex < pChannel->m_prChannel.lCloneListCount && lIndexFound < 0L)
		{
			if (FAILED(m_hrLastError = SafeArrayGetElement(pChannel->m_prChannel.psaCloneList, &lIndex, &vItem)))
				goto exit;
			ASSERT(VT_BSTR == vItem.vt, "VT_BSTR != vItem.vt in CChatSocket::bHandleClone");
			ASSERT(vItem.bstrVal, "vItem.bstrVal is NULL in CChatSocket::bHandleClone");
			if (!lstrcmpi(W2T(vItem.bstrVal), pPrIrcMsg->szParams[0]))
				lIndexFound = lIndex;
			else
				lIndex++;
			VariantClear(&vItem);
		}

		if (lIndexFound >= 0L)
		{
			// the clone already existed once upon the time on this server
			// need to relocate it
			for (lIndex = lIndexFound + 1L, lIndex1 = lIndexFound; lIndex < pChannel->m_prChannel.lCloneListCount; lIndex++, lIndex1++)
			{
				if (FAILED(m_hrLastError = SafeArrayGetElement(pChannel->m_prChannel.psaCloneList, &lIndex, &vItem)))
					goto exit;
				if (FAILED(m_hrLastError = SafeArrayPutElement(pChannel->m_prChannel.psaCloneList, &lIndex1, (PVOID) &vItem)))
					goto exit;
				VariantClear(&vItem);
			}
			pChannel->m_prChannel.lCloneListCount--;
		}
		else
		{
			// need to expand the vector a bit since this is a brand new clone
			sabound.cElements = pChannel->m_prChannel.lCloneListCount + 1L;
			sabound.lLbound = 0L;

			if (FAILED(m_hrLastError = SafeArrayRedim(pChannel->m_prChannel.psaCloneList, &sabound)))
				goto exit;
		}
	}
	else
	{
		// This is the first clone
		ASSERT(!pChannel->m_prChannel.psaCloneList, "pChannel->m_prChannel.psaCloneList is NOT NULL in CChatSocket::bHandleClone");
		if (!(pChannel->m_prChannel.psaCloneList = SafeArrayCreateVector(VT_VARIANT, 0L, 1)))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
	}

	vItem.vt = VT_BSTR;
	vItem.bstrVal = A2BSTR(pPrIrcMsg->szParams[0]);
	if (!vItem.bstrVal)
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto exit;
	}

	if (FAILED(m_hrLastError = SafeArrayPutElement(pChannel->m_prChannel.psaCloneList, &(pChannel->m_prChannel.lCloneListCount), (PVOID) &vItem)))
	{
		VariantClear(&vItem);
		goto exit;
	}

	pChannel->m_prChannel.lCloneListCount++;
	VariantClear(&vItem);

	// We now need to fire the OnChannelPropertyChanged event
	vItem.vt = (VT_VARIANT+VT_ARRAY);
	vItem.parray = pChannel->m_prChannel.psaCloneList;

	// Let's copy the new clone list into the vNewProperty variant
	if (FAILED(m_hrLastError = VariantCopy(&vNewProperty, &vItem)))
		goto exit;

	// And finally fire the event
	bRet = bFireChannelPropertyChanged(pChannel, NULL, g_rgwszChannelPropertyNames[cpnCloneList], &vOldProperty, &vNewProperty);

exit:
	VariantClear(&vOldProperty);
	VariantClear(&vNewProperty);

	return bRet;
}


BOOL CChatSocket::bHandleWhisper(PPRIRCMSG pPrIrcMsg)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleWhisper");

	// The prefix contains the sender, and we should have at least 3 params
	if (g_chEOS == pPrIrcMsg->szPrefix[0] || pPrIrcMsg->byteParams < 3)
	{
		ASSERT(FALSE, "Unexpected prefix or param number in CChatSocket::bHandleWhisper");
		m_hrLastError = CC_E_SERVER;
		return FALSE;
	}

	m_hrLastError = NOERROR;

	LPTSTR	szSenderNickname, sz;
	sz = szSenderNickname = pPrIrcMsg->szPrefix;
	SkipTillPrefixExtender(&sz);
	*sz = g_chEOS;

	PPRMEMBER			pPrMember;
	CChatChannel*		pChannel = NULL;
	enumChannelState	chs = chsOpen;

	// pPrIrcMsg->szParams[0] contains the channel name
	// pPrIrcMsg->szParams[1] contains the recipient nicknames
	// pPrIrcMsg->szParams[2] contains the incoming message

	// Find the channel in our open collection
	if (!m_listChannel.bFindCellFromData(1,
										g_nSearchByName,
										(PVOID) pPrIrcMsg->szParams[0],
										g_nSearchByState,
										(PVOID) &chs,
										(PVOID*) &pChannel,
										NULL))
	{
		ASSERT(FALSE, "Couldn't find open channel in CChatSocket::bHandleWhisper");
		m_hrLastError = E_FAIL;
		return FALSE;
	}

	ASSERT(pChannel, "pChannel is NULL in CChatSocket::bHandleWhisper");
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleWhisper");

	// Make sure the sender is in this channel - don't want to put the client in trouble
	if (!pChannel->bGetMemberFromNickname(szSenderNickname, &pPrMember))
	{
		ASSERT(FALSE, "Couldn't find whisper sender in channel in CChatSocket::bHandleWhisper");
		m_hrLastError = E_FAIL;
		return FALSE;
	}
	ASSERT(pPrMember, "pPrMember is NULL in CChatSocket::bHandleWhisper");

	// Make sure this member is not ignored in the channel
	if (pPrMember->lModes & mmClientIgnored)
		return TRUE;	// We just ignore the message in that case

	VARIANT	vRcpNicks;

	// Construct the recipients variant = VARIANT of VARIANTS of BSTRs
	if (!bConstructRecipientsVariant(pPrIrcMsg->szParams[1], &vRcpNicks, NULL))
		return FALSE;

	// Fire the OnMessage event finally
	bFireTextMessage(pChannel, szSenderNickname, &vRcpNicks, pPrIrcMsg->szParams[2], msgtWhisper);

	VariantClear(&vRcpNicks);

	return SUCCEEDED(m_hrLastError);
}
	

BOOL CChatSocket::bHandleServerWelcome(PPRIRCMSG pPrIrcMsg, UINT uCode)
{
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CChatSocket::bHandleServerWelcome");
	ASSERT(m_szNickname, "m_szNickname is NULL in CChatSocket::bHandleServerWelcome");
	ASSERT(m_bstrNickname, "m_bstrNickname is NULL in CChatSocket::bHandleServerWelcome");

	// Our real nickname might be truncated, let's check
	if (lstrcmpi(pPrIrcMsg->szParams[0], m_szNickname))
	{
		// Save the nick the user is now using
		if (g_chExtNckPfx == pPrIrcMsg->szParams[0][0])
		{
			LPWSTR wszNickname;
			if (!bConvertUTF8StringToWide(pPrIrcMsg->szParams[0], 0, &wszNickname, NULL, TRUE /*bNickname*/) ||
				!bSetNickname(pPrIrcMsg->szParams[0], pPrIrcMsg->szParams[0], (BSTR) wszNickname))
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
			delete [] wszNickname;
		}
		else
		{
			INT cbLen = lstrlen(pPrIrcMsg->szParams[0]);
			m_szNickname[cbLen] = g_chEOS;	// We truncate our nickname...
			m_bstrNickname[cbLen] = L'\0';  // ...and update the BSTR form too
		}
	}

	// The server name might not be the one we asked for either!
	// Ex:  comicadm-1.microsoft.com becomes comicsup.microsoft.com
	if (!bCopyStr(&m_szServerName, pPrIrcMsg->szPrefix))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (uCode == RPL_WELCOME)
	{
		m_cconn.SetLoggedOn(TRUE);
		
		// Notify the user that he's logged on
		::SendMessage(m_hwndMess, WM_U_CONNECTIONSTATE, (WPARAM) (m_csState = csLogged), 0L);
	}

	SHORT	nPrmsLen = 0;
	LPTSTR	szPrms = NULL;

	for (SHORT nPrm = 1; nPrm < pPrIrcMsg->byteParams; nPrm++)
		nPrmsLen += lstrlen(pPrIrcMsg->szParams[nPrm]) + 1;

	szPrms = new TCHAR[nPrmsLen+1];
	if (!szPrms)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	szPrms[0] = g_chEOS;

	for (nPrm = 1; nPrm < pPrIrcMsg->byteParams; nPrm++)
	{
		lstrcat(szPrms, pPrIrcMsg->szParams[nPrm]);
		lstrcat(szPrms, g_szSpace);
	}

	FireServerTextMessage(smtServerInfo, szPrms);

	delete [] szPrms;

	return TRUE;
}

