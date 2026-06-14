#ifndef __CCONN__
#define __CCONN__

//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include "Connect.H"
#include "PrCnst.H"
#include "CSSPI.H"

//--------------------------------------------------------------------------------------------
//
// CONSTANTS
//
//--------------------------------------------------------------------------------------------
const INT CBBUFFERMAX	= 16384;					// 8 K


//--------------------------------------------------------------------------------------------
//
// CLASSES
//
//--------------------------------------------------------------------------------------------

//
// Class to manage single connections to ANY server.
// Uses generic sockets to implement generic Connect, Read and Write functions.
//
class CChatConn : public CConnection
{
//
// Interfaces
//
public:
	CChatConn(DWORD cbBufferMax = CBBUFFERMAX);
	~CChatConn(void);

	void			FreeIrcMsg(PPRIRCMSG pPrIrcMsg);
	void			SetMsgWnd(HWND hwndMsg) { m_hwndMess = hwndMsg; }
	void			SetRegistered(BOOL bSet);
	void			SetStopPosting(BOOL bSet) { m_bStopPosting = bSet; }
	BOOL			bStoppedPosting(void) { return m_bStopPosting; }
	BOOL			bIsRegistered(void);
	void			SetLoggedOn(BOOL bSet);
	BOOL			bIsLoggedOn(void);
	BOOL			bIsIrcXServer(void) { return m_bIrcXServer; }

	HRESULT			HrConnection(CHAR *szServer, UINT uPort);
	HRESULT			HrIdentifyServer(CHAR *szServer);
	HRESULT			HrLogin(CHAR *szNickname, CHAR *szUserName, CHAR *szRealName, CHAR *szPassword, CHAR *szSecurityPackage);
	HRESULT			HrLogOff(void);
	HRESULT			HrGetSecurity(PPRSECURITY *ppPrSecurity);
	HRESULT			HrSendAndDelete(BYTE *pb, INT cb);
	HRESULT			HrGetLastError() { return m_hrLastError; }

	BOOL			bCreateChannel(CHAR *szName, CHAR *szKeyword, LONG lModes, LONG lMaxMemberCount);

	void			LockPosting(void) { m_bProtectedPosting = TRUE; m_csPosting.Lock(); }
	void			UnlockPosting(void) { m_csPosting.Unlock(); m_bProtectedPosting = FALSE; }

#ifdef DEBUG
	void			DumpIrcMsg(PPRIRCMSG pPrIrcMsg);
	BOOL			bStopPosting(void) { return m_bStopPosting; }
#endif // DEBUG

protected:
	BOOL			bInitIrcMsgPool(void);
	BOOL			bFreeIrcMsgPool(void);
	BOOL			bPostNewIrcMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);

	HRESULT			HrWaitForNextReplies(DWORD dwTimeOut, BOOL bGetAllReplies, PPRIRCMSG rgPrIrcMsg, UINT *pcPrIrcMsgCount);
	HRESULT			HrCopyNextWord(CHAR *szDest, CHAR **pszSrc, INT *pcLen);
	HRESULT			HrCopyNextParam(CHAR **pszDest, SHORT *pnParamLen, CHAR **pszSrc, INT *pcLen);
	HRESULT			HrFillInIrcMsg(PPRIRCMSG pPrIrcMsg, CHAR *szSrc);
	HRESULT			HrFillInSecurityPackages(PPRIRCMSG pPrIrcMsg);
	HRESULT			HrIsServerIrcX(CHAR *szServer);
	HRESULT			HrMakeLoginMsg(CHAR *szNickname, CHAR *szUserName, CHAR *szRealName, CHAR *szPassword, BYTE **ppb, INT *pcb);
	HRESULT			HrAuthenticate(CHAR *szUserName, CHAR *szPassword, CHAR *szSecurityPackage);

//	HRESULT			HrDecode(CHAR *szSrc, PBYTE pbDst, UINT *pcbDst);
//	HRESULT			HrEncode(PBYTE pbSrc, UINT cbSrc, PBYTE pbDst, BOOL bMessage = FALSE);
	HRESULT			HrGenerateAndSendAuthMsg(CHAR *szBlob, CHAR *szSecurityPackage);

	virtual HRESULT	HrReceiveData(PVOID pvRecv, INT cbData, INT *pcbTaken);

//
// Data
//
protected:
	CLock			m_csPosting;
	BOOL			m_bProtectedPosting;
	BOOL			m_bStopPosting;

	CLock			m_csData;

    CredHandle		m_hCredential;			// SSPI Security
    CtxtHandle		m_hContext;				// SSPI Security
	PSecurityFunctionTable m_pFuncTbl;		// SSPI Security
    HINSTANCE		m_hSecLib;				// SSPI Security
	BOOL			m_bCredential;			// SSPI Security

	BOOL			m_bLoggedOn;
	BOOL			m_bIrcXServer;
	BOOL			m_bRegistered;
	PPRSECURITY		m_pPrSecurity;
	
	LONG			m_lPoolIndex;
	LONG			m_lPoolFree;
	PRIRCMSG		m_rgPrIrcMsg[g_nIrcMsgPool];
	HWND			m_hwndMess;
	HRESULT			m_hrLastError;
};

#endif // __CCONN__
