//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include "StdAfx.H"
#include "CIdentD.H"
#include "CDebug.H"
#include "CCError.H"

// for ASSERT and FAIL
SZTHISFILE

//--------------------------------------------------------------------------------------------
//
// CIdentD
// 
//--------------------------------------------------------------------------------------------

CIdentD::CIdentD(DWORD cbBufferMax)
		  :CConnection(cbBufferMax)
{
	m_bAuthenticated= FALSE;
	m_szUserName	= NULL;
}


CIdentD::~CIdentD(void)
{
}

// Bind socket to a specific port
HRESULT CIdentD::HrBindToPortAndListen(INT idPort)
{
	HRESULT		hr;
	SOCKADDR_IN	sin;  
	SOCKET		hSock = GetSocket();
	u_long		argp = 1L;

	ASSERT(SUCCEEDED(HrIsSocketValid()), "Socket invalid in CIdentD::HrBindToPortAndListen");

	::ZeroMemory(&sin, sizeof(sin));

	sin.sin_family = PF_INET; 
	sin.sin_addr.s_addr = 0; 
    sin.sin_port = htons((u_short) idPort);

#ifdef DEBUG
    if (SOCKET_ERROR == ::bind(hSock, (LPSOCKADDR) &sin, sizeof(sin)))
	{
		hr = HrWinSockError();
		CloseSocket();
		return hr;
	}
	
	if (SOCKET_ERROR == ::listen(hSock, 1))
	{
		hr = HrWinSockError();
		CloseSocket();
		return hr;
	}

	if (SOCKET_ERROR == ::ioctlsocket(hSock, FIONBIO, &argp))
	{
		hr = HrWinSockError();
		CloseSocket();
		return hr;
	}

	return NOERROR;
#else
    if (SOCKET_ERROR == ::bind(hSock, (LPSOCKADDR) &sin, sizeof(sin)) ||
		SOCKET_ERROR == ::listen(hSock, 1) ||
		SOCKET_ERROR == ::ioctlsocket(hSock, FIONBIO, &argp))
	{
		hr = HrWinSockError();
		CloseSocket();
		return hr;
	}
	else
		return NOERROR;
#endif // DEBUG
}


HRESULT CIdentD::HrAccept(CIdentD *pcIdentD)
{
	struct sockaddr	addr;
	int				addrlen = sizeof(addr);
	SOCKET			hSock = GetSocket();
	SOCKET			hSockAccept;

	ASSERT(pcIdentD, "pcIdentD is NULL in CIdentD::HrAccept");

	if (FAILED(HrIsSocketValid()))
	{
		ASSERT(FALSE, "Invalid Socket in CIdentD::HrAccept");
		return CC_E_SOCKETERROR;
	}

	::ZeroMemory(&addr, sizeof(addr));

	hSockAccept = ::accept(hSock, &addr, &addrlen);

	if (INVALID_SOCKET == hSockAccept)
	{
		OutputDebugThreadIdString("CIdentD::HrAccept - Failed\n");
		return HrWinSockError();
	}
	else
	{
		BOOL bRet = pcIdentD->bSetSocket(hSockAccept);
		ASSERT(bRet, "bSetSocket failed in CIdentD::HrAccept");
		return pcIdentD->HrInit();
	}
}


HRESULT	CIdentD::HrAuthenticate(LPCTSTR szUserName)
{
	OutputDebugThreadIdString("CIdentD::HrAuthenticate - Enter\n");

	if (!m_szUserName)
		m_szUserName = szUserName;

	HRESULT	hr = HrIsDataWaiting(0, 200000);	// we only wait for 200000 microseconds
	switch (hr)
	{
		case CC_E_NOSOCKETDATA:
			// nothing's here to eat
			hr = S_FALSE;
			break;
		case NOERROR:
			// we have some meat available
			hr = HrReaderThread(FALSE);
			break;
		//default:
			// we lost the socket
	}
	return hr;
}


HRESULT CIdentD::HrReceiveData(PVOID pvRecv, INT cbData, INT *pcbTaken)
{
	CHAR	*szOutput;
	HRESULT	hr;

	OutputDebugThreadIdString("CIdentD::HrReceiveData - Enter\n");

	*pcbTaken = 0;

#ifdef DEBUG
	CHAR	szDebug[512];

	lstrcpyn(szDebug, (CHAR*) pvRecv, cbData+1);
	OutputDebugThreadIdString(szDebug);
#endif // DEBUG

	// The server must have sent us something like: "1778 , 6667"
	// We need to reply with "1778 , 6667 : USERID : UNIX : <UserName>\r\n"

	ASSERT(m_szUserName, "m_szUserName is NULL in CIdentD::HrReceiveData");

	CHAR *szEnd = SzSubStr((CHAR*) pvRecv, g_szCR);
	if (!szEnd)
		// no complete message available
		return NOERROR;

	// There is a complete message to eat
	*szEnd = g_chEOS;

	if (!(szOutput = new CHAR[cbData+lstrlen(m_szUserName)+32]))
		return E_OUTOFMEMORY;

	//sprintf(szOutput, "%s : USERID : UNIX : %s\r\n", (CHAR*) pvRecv, m_szUserName);
	wsprintf(szOutput, "%s : USERID : UNIX : %s\r\n", (CHAR*) pvRecv, m_szUserName);

	hr = HrSend((PVOID) szOutput, lstrlen(szOutput));

	if (SUCCEEDED(hr))
	{
		*pcbTaken = cbData;
		m_bAuthenticated = TRUE;
	}

	delete [] szOutput;

	return hr;
}


void CIdentD::Close(void)
{
	OutputDebugThreadIdString("CIdentD::Close - Enter\n");

    if (INVALID_SOCKET != m_hSocket)
	{
        INT iErr = ::closesocket(m_hSocket);
		ASSERT(0 == iErr, "closesocket failed in CIdentD::Close");
		m_hSocket = INVALID_SOCKET;
	}

	CleanUp();
	m_szUserName = NULL;
	m_bAuthenticated = FALSE;
}
