#ifndef __CIDENTD_H__
#define __CIDENTD_H__

//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include "Connect.H"
#include "PrCnst.H"

//--------------------------------------------------------------------------------------------
//
// CONSTANTS
//
//--------------------------------------------------------------------------------------------
const INT CBBUFFERMAXIDENT	= 512;					// 1/2 K


//--------------------------------------------------------------------------------------------
//
// CLASSES
//
//--------------------------------------------------------------------------------------------

//
// Class to manage single connections to ANY server.
// Uses generic sockets to implement generic Connect, Read and Write functions.
//
class CIdentD : public CConnection
{
// Interfaces
public:
	CIdentD(DWORD cbBufferMax = CBBUFFERMAXIDENT);
	~CIdentD(void);

	BOOL			bAuthenticated(void) { return m_bAuthenticated; }

	HRESULT			HrBindToPortAndListen(INT idPort);
	HRESULT			HrAccept(CIdentD *pcIdentD);
	HRESULT			HrAuthenticate(LPCTSTR szUserName);

	void			Close(void);

protected:
	virtual HRESULT	HrReceiveData(PVOID pvRecv, INT cbData, INT *pcbTaken);

// Data
protected:
	BOOL			m_bAuthenticated;
	LPCTSTR			m_szUserName;
};

#endif // __CIDENTD_H__
