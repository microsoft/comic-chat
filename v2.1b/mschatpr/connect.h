#ifndef __CONNECT__
#define __CONNECT__

//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include "CUtil.H"
#include "WinSock.H"

//--------------------------------------------------------------------------------------------
//
// CONSTANTS
//
//--------------------------------------------------------------------------------------------
const INT CBBUFFERMOVE	 = 2048;					// 2 K 
const INT CLINGERTIMEOUT = 60;						// linger for 60 seconds max when closing the socket

//--------------------------------------------------------------------------------------------
//
// CLASSES
//
//--------------------------------------------------------------------------------------------

//
// Class to manage single connections to ANY server.
// Uses generic sockets to implement generic Connect, Read and Write functions.
//
class CConnection
{
//
// Interfaces
//
public:
	CConnection(DWORD cbBufferMax = 2048L);
	~CConnection(void);

	BOOL	bSetSocket(SOCKET hSocket);
	BOOL	bInitSocketDLL(void);
	void	FreeSocketDLL(void);

	void	SetCancel(BOOL bNewValue);
	void	SetInConnectCall(BOOL bNewValue);

	HRESULT HrInit(void);
	HRESULT HrConnection(CHAR* pszServer, UINT uPort);
	HRESULT HrClose(BOOL bCleanUp = TRUE);
	HRESULT HrIsSocketValid(void);
	HRESULT HrGetLocalHost(CHAR *szName, INT cch);
	HRESULT HrIsDataWaiting(DWORD dwTimeoutSec = 1L, DWORD dwTimeoutMicroSec = 0L);
	HRESULT HrSend(PVOID pvData, DWORD cb, INT flags = 0);
	HRESULT	HrReaderThread(BOOL bInfinite = TRUE);

	HRESULT HrCreateStreamSocket(void);

protected:
	
	void	Lock(void)
			{
				m_cs.Lock();
			}
		
	void	Unlock(void)
			{
				m_cs.Unlock();
			}

	void	CloseSocket(void);
	void	CleanUp(void);

	BOOL	bCancelPending(void);
	
	SOCKET	GetSocket(void);

	HRESULT HrSetSocketLinger(void);
	HRESULT HrConnectToServer(struct hostent *pHostEnt, ULONG uAddr, UINT uPort);
	HRESULT HrRecv(PVOID pvBuf, INT cbBuf, INT flags, INT *pcbRead);
	HRESULT	HrWinSockError(void);
			
	virtual HRESULT HrReceiveData(PVOID pvRecv, INT cbData, INT *pcbTaken) = 0;
//
// Data
//
protected:
	SOCKET				m_hSocket;
	BOOL				m_bCancel;
	BOOL				m_bInConnectCall;
						
	CLock				m_cs;

	BYTE				*m_pbBuf;		// buffer for socket to read into
	INT					m_cbMax;		// how data MAX
};

#endif // __CCONNECT__
