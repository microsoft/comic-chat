//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include "StdAfx.H"
#include "Connect.H"
#include "CCError.H"

// for ASSERT and FAIL
SZTHISFILE

//--------------------------------------------------------------------------------------------
//
// CConnection
// 
// Manages connections to chat servers.
// Wrapper that implements a socket based connection.
//
//--------------------------------------------------------------------------------------------

CConnection::CConnection(DWORD cbBufferMax)
{
	m_hSocket		= INVALID_SOCKET;
	m_bCancel		= FALSE;
	m_bInConnectCall= FALSE;
	m_pbBuf			= NULL;
	m_cbMax			= cbBufferMax;
}


CConnection::~CConnection(void)
{
	CloseSocket();
	CleanUp();
}


BOOL CConnection::bInitSocketDLL(void)
{
	WORD	wVer; 
	WSADATA wsaData; 
	INT		err; 
	
	wVer = MAKEWORD(1, 1);	// use Winsock 1.1 

	// Load the DLL
	err = WSAStartup(wVer, &wsaData);  
	if (err != 0) 
		{
		return FALSE;
		}

	// Make sure that the winsock dll indeed supports 1.1
	if (1 != LOBYTE(wsaData.wVersion) || 1 != HIBYTE(wsaData.wVersion)) 
		{ 
		WSACleanup(); 
		return FALSE;
		}

    return TRUE; 
}


void CConnection::FreeSocketDLL(void)
{
	WSACleanup();
}


//
// Open a connection the named server, and connect to the port 'uPort'
// Can block
//
HRESULT CConnection::HrConnection(CHAR* pszServer, UINT uPort)
{
	HRESULT hr = NOERROR;

	OutputDebugThreadIdString("CConnection::HrConnection - Enter\n");

	if (!pszServer)
	{
		ASSERT(FALSE, "pszServer is NULL in CConnection::HrConnection");
		return E_INVALIDARG;
	}

	struct hostent	*pHostEnt = NULL;
	ULONG			uAddr;

	if (SUCCEEDED(hr = HrIsSocketValid()) || bCancelPending())
		return hr;	// already connected or user wants to cancel connection

	// Load WinSock, etc
	if (FAILED(hr = HrInit() || bCancelPending()))
		goto error;

	//
	// resolve host name to an IP address
	// THIS COULD BLOCK
	//
	// Is it an address in IP format?
	uAddr = inet_addr(pszServer);
	if (INADDR_NONE == uAddr)
	{
		// It's not an internet address. resolve with DNS
		pHostEnt = ::gethostbyname(pszServer);
		if (NULL == pHostEnt)
		{
			hr = CC_E_HOSTNOTFOUND;
			goto error;
		}
	}

	if (bCancelPending())
		goto error;

	//  Create a STREAM socket.
	if (FAILED(hr = HrCreateStreamSocket()))
		goto error;

	if (bCancelPending())
		goto error;

	// connect
	if (FAILED(hr = HrConnectToServer(pHostEnt, uAddr, uPort)))
		goto error;

	return NOERROR;

error:	
	if (bCancelPending())
		hr = CC_E_SOCKETCLOSED;

	CloseSocket();
	CleanUp();
	return hr;
}


HRESULT CConnection::HrGetLocalHost(CHAR *szName, INT cch)
{
	OutputDebugThreadIdString("CConnection::HrGetLocalHost - Enter\n");

	ASSERT(szName && cch > 0, "!szName or cch <= 0 in CConnection::HrGetLocalHost");

	return (SOCKET_ERROR == ::gethostname(szName, cch)) ? E_FAIL : NOERROR;
}


HRESULT CConnection::HrClose(BOOL bCleanUp)
{
	OutputDebugThreadIdString("CConnection::HrClose - Enter\n");

	SetCancel(TRUE);	// in case we called HrClose BEFORE we actually created a win socket!

	// Close the socket
	CloseSocket();

	if (bCleanUp)	// This is to make sure that we don't delete the m_pbBuf 
		CleanUp();	// while it's still being used by the reading thread

	return NOERROR;
}


BOOL CConnection::bSetSocket(SOCKET hSocket)
{
	BOOL bRet = TRUE;
	
	OutputDebugThreadIdString("CConnection::bSetSocket - Enter\n");

	ASSERT(hSocket, "hSocket is NULL in CConnection::bSetSocket");

	Lock();
	if (INVALID_SOCKET == m_hSocket)
		m_hSocket = hSocket;
	else
	{
		ASSERT(FALSE, "INVALID_SOCKET != m_hSocket in CConnection::bSetSocket");
		bRet = FALSE;
	}
	Unlock();

	return bRet;
}


SOCKET CConnection::GetSocket(void)
{
	SOCKET	sock;

//	OutputDebugThreadIdString("CConnection::GetSocket - Enter\n");

	Lock();
	sock = m_hSocket;
	Unlock();

	return sock;
}


//
// Set the linger values for socket closes..
//
HRESULT CConnection::HrSetSocketLinger(void)
{
	SOCKET	hSock;
	LINGER  lingerInfo;

	OutputDebugThreadIdString("CConnection::HrSetSocketLinger - Enter\n");

	hSock = GetSocket();	

	lingerInfo.l_onoff = 1;
	lingerInfo.l_linger = CLINGERTIMEOUT;

	if (SOCKET_ERROR == ::setsockopt(hSock, 
									 SOL_SOCKET, 
 									 SO_LINGER, 
									 (char *)&lingerInfo,
									 sizeof(lingerInfo)))
		return CC_E_SOCKETERROR;
	else
		return NOERROR;
}


//
// protected.
// Initialize socket dlls . Multi-thread safe
//
HRESULT CConnection::HrInit(void)
{
	BYTE *pb = NULL;

	OutputDebugThreadIdString("CConnection::HrInit - Enter\n");

	// CleanUp any existing stuff
	CleanUp();	
	
	// Since the socket's data is being altered, prevent multi-thread fiddling
	Lock();
	
	pb = new BYTE[m_cbMax];
	if (!pb)
	{
		Unlock();
		return E_OUTOFMEMORY;
	}
	else
	{
		m_pbBuf	= pb;
		#ifdef DEBUG
			sprintf(g_szDebugStr, "New receiving buffer address: %X\n", m_pbBuf);
			OutputDebugThreadIdString(g_szDebugStr);
		#endif
		Unlock();
		return NOERROR;
	}
}


//
// protected. 
// NOTE: This should be called only from within HrConnect, because that protects 
// stuff from multi-thread fiddling.
//
HRESULT CConnection::HrCreateStreamSocket(void)
{
	OutputDebugThreadIdString("CConnection::HrCreateStreamSocket - Enter\n");

	// If already connected, return
	if (SUCCEEDED(HrIsSocketValid()))
		return NOERROR;
	
	//  Create a STREAM socket.
	SOCKET hSock = ::socket(PF_INET, SOCK_STREAM, 0);
	if (INVALID_SOCKET == hSock)
		return CC_E_SOCKETCREATE;

	// Save the socket
	return bSetSocket(hSock) ? NOERROR : CC_E_SOCKETERROR;
}


HRESULT CConnection::HrConnectToServer(struct hostent *pHostEnt, ULONG uAddr, UINT uPort)
{ 
	OutputDebugThreadIdString("CConnection::HrConnectToServer - Enter\n");

	if (FAILED(HrIsSocketValid()))
	{
		ASSERT(FALSE, "Invalid Socket in CConnection::HrConnectToServer");
		return CC_E_SOCKETERROR;
	}

	SOCKADDR_IN     sin;
	SOCKET			hSock;

	//  Fill in the address and port to connect to.
	::ZeroMemory(&sin, sizeof(sin));

	sin.sin_port = htons(uPort);
	if (pHostEnt && pHostEnt->h_addr_list)
	{
		sin.sin_family = pHostEnt->h_addrtype;
		::CopyMemory(&sin.sin_addr, *pHostEnt->h_addr_list, pHostEnt->h_length);
	}
	else
	{
		sin.sin_family = AF_INET;
		::CopyMemory(&sin.sin_addr, &uAddr, sizeof(ULONG));
	}

    //  Connect to the IP:Port address provided. This can BLOCK, so grab a copy of the socket
	//  This way, a seperate thread can cancel the connect
	hSock = GetSocket();

	SetInConnectCall(TRUE);
	OutputDebugThreadIdString("CConnection::HrConnectToServer - Calling ::connect\n");
    if (SOCKET_ERROR == ::connect(hSock, (const SOCKADDR*) &sin, sizeof(sin)))
	{
		OutputDebugThreadIdString("CConnection::HrConnectToServer - ::connect failed, calling CloseSocket\n");
		SetInConnectCall(FALSE);
		#ifdef DEBUG
			int err = WSAGetLastError();
		#endif // DEBUG
		CloseSocket();
        return CC_E_CANTCONNECT;
	}
	OutputDebugThreadIdString("CConnection::HrConnectToServer - ::connect succeeded - Leave\n");
	SetInConnectCall(FALSE);
	
	return NOERROR;
}


//
// protected. 
//
HRESULT CConnection::HrSend(PVOID pvData, DWORD cb, INT flags)
{
	HRESULT hr;
    INT		cbSent;

	OutputDebugThreadIdString("CConnection::HrSend - Enter\n");

	ASSERT(pvData, "pvData in NULL in CConnection::HrSend");

	if (FAILED(hr = HrIsSocketValid()))
		return hr;

	// send the data. This can BLOCK
	SOCKET	hSock;
	PCSTR	pstrSend;
	
	hSock	= GetSocket();

	// Loop till we've sent all the data
#ifdef DEBUG
	g_cs.Lock();
	OutputDebugString("     [");
#endif // DEBUG
	pstrSend = (PCSTR) pvData;
	while (cb > 0)	// keep sending till we've sent it all
	{
		cbSent = ::send(hSock, pstrSend, cb, flags);
#ifdef DEBUG
		CHAR szOutgoing[2048];
		if (cbSent != SOCKET_ERROR)
		{
			CopyMemory((PVOID) szOutgoing, (CONST VOID*) pstrSend, cbSent);
			szOutgoing[cbSent-2] = g_chEOS;
			OutputDebugString(szOutgoing);
		}
		else
		{
			CopyMemory((PVOID) szOutgoing, (CONST VOID*) pstrSend, cb);
			szOutgoing[cb] = g_chEOS;
			sprintf(g_szDebugStr, "Failed sending:'%s']\n", szOutgoing);
			OutputDebugString(g_szDebugStr);
		}
#endif // DEBUG
		if (SOCKET_ERROR == cbSent)
		{
   			CloseSocket();
			return CC_E_CANTSEND;
		}
		cb -= cbSent;
		pstrSend += cbSent;
	}
#ifdef DEBUG
	OutputDebugString("]\n");
	g_cs.Unlock();
#endif // DEBUG
	return NOERROR;
}


//
// protected. 
//
HRESULT CConnection::HrRecv(PVOID pvBuf, INT cbBuf, INT flags, INT *pcbRead)
{
	HRESULT hr;
	INT		cb;

	OutputDebugThreadIdString("CConnection::HrRecv - Enter\n");

	//ASSERT(pvBuf, "pvBuf is NULL in CConnection::HrRecv");
	ASSERT(pcbRead, "pcbRead is NULL in CConnection::HrRecv");

	*pcbRead = 0;

	if (FAILED(hr = HrIsSocketValid()))
		return hr;

	// read whatever is waiting on our socket. This could BLOCK
    cb = ::recv(m_hSocket, (CHAR*) pvBuf, cbBuf, flags);
	
	// Check for errors
	if (SOCKET_ERROR == cb)
	{
		HRESULT hr = HrWinSockError();
		CloseSocket();
		return hr;
	}

	// If cb is 0, the socket has been closed. Good time to clean up
    if (0 == cb)
	{ 
        CloseSocket();
		return CC_E_HOSTDROPPEDCONNECTION;
	}	
	
	*pcbRead = cb;

	return NOERROR;
}


//
// protected
//
HRESULT CConnection::HrIsSocketValid(void)
{
	HRESULT hr = NOERROR;

//	OutputDebugThreadIdString("CConnection::HrIsSocketValid - Enter\n");

	Lock();

	if (INVALID_SOCKET == m_hSocket)
		hr = CC_E_NOTCONNECTED;

	Unlock();

	return hr;
}


BOOL CConnection::bCancelPending(void)
{
	BOOL bRet;

	Lock();

	bRet = m_bCancel;

	Unlock();

	return bRet;
}


void CConnection::SetCancel(BOOL bNewValue)
{
	#ifdef DEBUG
		if (bNewValue)
			OutputDebugThreadIdString("CConnection::SetCancel - Enter (TRUE)\n");
		else
			OutputDebugThreadIdString("CConnection::SetCancel - Enter (FALSE)\n");
	#endif // DEBUG

	Lock();

	m_bCancel = bNewValue;

	Unlock();
}


void CConnection::SetInConnectCall(BOOL bNewValue)
{
	#ifdef DEBUG
		if (bNewValue)
			OutputDebugThreadIdString("CConnection::SetInConnectCall - Enter (TRUE)\n");
		else
			OutputDebugThreadIdString("CConnection::SetInConnectCall - Enter (FALSE)\n");
	#endif // DEBUG

	Lock();

	m_bInConnectCall = bNewValue;

	Unlock();
}


//
// protected:
// Close the connection, if we have one
//
void CConnection::CloseSocket(void)
{
	OutputDebugThreadIdString("CConnection::CloseSocket - Enter\n");

	Lock();

	if (INVALID_SOCKET != m_hSocket && !m_bInConnectCall)
	{
		HrSetSocketLinger();	// make sure we linger when we close

		OutputDebugThreadIdString("CConnection::CloseSocket - Calling ::closesocket\n");
        INT iErr = ::closesocket(m_hSocket);
		ASSERT(0 == iErr, "closesocket failed in CConnection::CloseSocket");
		m_hSocket = INVALID_SOCKET;
	}

	//	m_bCancel = FALSE;	// REGISB: bug fix 08/20/97

	Unlock();

	OutputDebugThreadIdString("CConnection::CloseSocket - Leave\n");
}


//
// protected
// Is there any data waiting for the socket?
// This will BLOCK depending on the value of timeout
//
HRESULT CConnection::HrIsDataWaiting(DWORD dwTimeoutSec, DWORD dwTimeoutMicroSec)
{
	HRESULT			hr;
	SOCKET			hSock;
	INT				iStatus;
	fd_set			fdRead;
	struct timeval	timeout = {dwTimeoutSec, dwTimeoutMicroSec}; // we just want to poll the socket

	OutputDebugThreadIdString("CConnection::HrIsDataWaiting - Enter\n");

	if (FAILED(hr = HrIsSocketValid()))
		return hr;

	FD_ZERO(&fdRead);

	hSock = GetSocket();
	FD_SET(hSock, &fdRead);	

	// this will return depending on timeout
	iStatus = ::select(0, &fdRead, NULL, NULL, &timeout);
	if (SOCKET_ERROR == iStatus)
		return CC_E_SOCKETERROR;

	if (FD_ISSET(hSock, &fdRead))
		return NOERROR;
	else
		return CC_E_NOSOCKETDATA;
}


//
// The thread that reads data off the socket
//
HRESULT CConnection::HrReaderThread(BOOL bInfinite)
{
	INT			cbRead;			// how much data was read.
	INT			cbTaken;		// how much data was taken by the owner chatsocket
	INT			cbData;			// how much "real" data
	INT			cbBuf;			// how much "real" space in the buffer
	BYTE		*pbBuf = NULL;	// pointer to where we want to place newly read data
	BYTE		*pbRecv = NULL;	// pointer we want to give to bReceiveData
	HRESULT		hr = NOERROR;

#ifdef DEBUG
	if (bInfinite)
		OutputDebugThreadIdString("CConnection::HrReaderThread - Enter: Infinite\n");
	else
		OutputDebugThreadIdString("CConnection::HrReaderThread - Enter: Non-Infinite\n");
#endif // DEBUG

	ASSERT(m_pbBuf, "m_pbBuf is NULL in CConnection::HrReaderThread");
	
	cbData	= 0;
	cbTaken = 0;
	cbRead	= 0;
	cbBuf	= m_cbMax;

	do
	{
		// Did the chat socket take less than we read? If so, save the remainder
		if (cbTaken >= cbData)
		{
			// Empty the buffer
LClearBuf:
			cbData	= 0;
LResetBuf:
			cbBuf	= m_cbMax - cbData;	// how much modifiable space do we have in the buffer?
			pbBuf	= m_pbBuf + cbData;	// actual beginning of the modifiable buffer
			pbRecv 	= m_pbBuf;	// points to the first byte in the buffer with readable data		
			goto LRead;
		}
		// NOTE: We do this to MINIMIZE MoveMemory calls. So basically the effective
		// buffer window moves down.. and we do a MoveMemory only when the remaining
		// buffer is smaller than a predefined threshold
		cbBuf	-=	cbRead;		// this much fewer modifiable bytes (since the last read)
		cbData	-=	cbTaken;	// how much of the buffer did the recipient keep? We have to
								// save the remainder.	
		pbRecv	+=	cbTaken;	// then next time we give data to the recipient, we need to
								// start with that data that we are saving for them..
		pbBuf	+=	cbRead;		// and since we are storing data, we better move the position
								// of the modifiable buffer
		//
		// Do we have less buffer left than the threshold we are willing to live with?
		// If so, move what data we have UP..
		if (cbBuf <= CBBUFFERMOVE)
		{
			// Move Buffer up, if we can
			if (cbData >= m_cbMax)
				goto LClearBuf;

			if (pbRecv != m_pbBuf)
			{
				::MoveMemory((PVOID) m_pbBuf, (PVOID) pbRecv, cbData);
				goto LResetBuf;
			}
		}
		// Read the socket
LRead:
		cbRead = cbTaken = 0;
		if (SUCCEEDED(hr = HrRecv((PVOID) pbBuf, cbBuf, 0, &cbRead)))
		{
			// Got some data. Give it to the parent chat socket
			// Because Sockets are STREAM, it is possible that msgs will get broken up
			// between reads. This is especially true of IRC and to some extent of MIC.
			// Because there is no guarantee, we give the parent chatsocket the entire contents
			// of the read, and it tells us how much of the block it actually "TOOK".
			// If the owner decided that it had not yet received the full contents of the 
			// msg it was expecting, it could set cbTaken appropriately.
			// NOTE: you cannot take bytes randomly from within the buffer. You must take
			// bytes directly off the top and sequentially
			//
			ASSERT(cbRead <= cbBuf, "cbRead > cbBuf in CConnection::HrReaderThread");
			cbData += cbRead;	// how much data do we have now..
			ASSERT(cbData <= m_cbMax, "cbData > m_cbMax in CConnection::HrReaderThread");

			if (FAILED(hr = HrReceiveData((PVOID) pbRecv, cbData, &cbTaken)))
				goto LExit;
		}
		else
			goto LExit;
	}
	while (bInfinite || cbData > cbTaken);

LExit:
	OutputDebugThreadIdString("CConnection::HrReaderThread - Leave\n");
	return hr;
}


//
// protected
// Clean up all memory usage
//
void CConnection::CleanUp(void)
{
	OutputDebugThreadIdString("CConnection::CleanUp - Enter\n");

	// REGISB: The reading thread has to be dead at this point

	// Protect data from multi-threads.. since data can get altered in here
	Lock();

	if (m_pbBuf)
	{
		delete [] m_pbBuf;
		m_pbBuf = NULL;
	}

	Unlock();
}


HRESULT CConnection::HrWinSockError(void)
{
	OutputDebugThreadIdString("CConnection::HrWinSockError - Enter\n");

	HRESULT	hr = CC_E_SOCKETERROR;

	INT idErr = WSAGetLastError();
	switch (idErr)
	{
		default:
			OutputDebugThreadIdString("CConnection::HrWinSockError - Check unexpected value\n");
			break;

#ifdef DEBUG
		case WSAEADDRINUSE:
			OutputDebugThreadIdString("CConnection::HrWinSockError - Check unexpected WSAEADDRINUSE\n");
			break;

		case WSAEFAULT:
			OutputDebugThreadIdString("CConnection::HrWinSockError - Check unexpected WSAEFAULT\n");
			break;
#endif // DEBUG

		case WSAEINTR:
			hr = CC_E_SOCKETCLOSED;
			break;
			
		case WSANOTINITIALISED:
			hr = CC_E_SOCKETERROR;
			break;
		
		case WSAENETDOWN:
			hr = CC_E_NETWORKDOWN;
			break;
		
		case WSAENETRESET:
			hr = CC_E_LOSTCONNECTION;
			break;

		case WSAENOTCONN:
			hr = CC_E_NOTCONNECTED;
			break;

		case WSAESHUTDOWN:
			hr = CC_E_SOCKETCLOSED;
			break;
		
		case WSAECONNRESET:
			hr = CC_E_HOSTDROPPEDCONNECTION;
			break;

		case WSAEWOULDBLOCK:
			hr = CC_E_SOCKETERROR;
			break;
	}
	return hr;
}
