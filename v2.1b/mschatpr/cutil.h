#ifndef __CUTIL_H__
#define __CUTIL_H__

#include <windows.h>
#include <tchar.h>
#include "CCommon.H"
#include "CDebug.H"

const SHORT	g_byteCTCPBrace		= 0x01;

// String length constants
const SHORT g_nMaxLengthTitle	= 64;	// max length of message boxes title
const SHORT g_nMaxLengthBody	= 148;	// max length of message boxes body
const SHORT g_nMaxLengthError	= 256;	// max length of error messages

// Various characters and strings
const TCHAR	g_szDefaultSP[]		= _T("ANON");
const TCHAR g_szNoUser[]		= _T("NoUser");
const TCHAR g_szIrcPrefix[]		= _T("irc:");
const TCHAR g_szDblSlash[]		= _T("//");
const TCHAR g_szDblBackSlash[]	= _T("\\\\");
const TCHAR g_szSlash[]			= _T("/");
const TCHAR g_szBackSlash[]		= _T("\\");
const TCHAR g_szCRLF[]			= _T("\r\n");
const TCHAR g_szNameSeparator[]	= _T(", ");
const TCHAR g_szSColon[]		= _T(";");

const TCHAR g_chColon			= _TCHAR(':');
const TCHAR g_chSColon			= _TCHAR(';');
const TCHAR g_chSlash			= _TCHAR('/');
const TCHAR g_chStar			= _TCHAR('*');
const TCHAR g_chQMark			= _TCHAR('?');
const TCHAR g_chExclamation		= _TCHAR('!');
const TCHAR g_chAt				= _TCHAR('@');


// Custom Windows messages
// These codes are used by the messaging window of the main thread, which triggers the control events
const UINT WM_U_BEGINENUMERATION		= (WM_USER+1);
const UINT WM_U_CONNECTIONSTATE			= (WM_USER+2);
const UINT WM_U_CHANNELSTATE			= (WM_USER+3);
const UINT WM_U_CLOSECHANNELS			= (WM_USER+4);
const UINT WM_U_SOCKET_HRESULT			= (WM_USER+5);
const UINT WM_U_CHANNEL_HRESULT			= (WM_USER+6);
const UINT WM_U_IRC_MSG					= (WM_USER+7);
const UINT WM_U_SERVERPROP				= (WM_USER+8);
const UINT WM_U_CHANNELPROP				= (WM_USER+9);
const UINT WM_U_CHANNELPROP2			= (WM_USER+10);
const UINT WM_U_MEMBERPROP				= (WM_USER+11);
const UINT WM_U_USERPROP				= (WM_USER+12);

const UINT WM_U_MIN						= WM_U_BEGINENUMERATION;
const UINT WM_U_MAX						= WM_U_USERPROP;

const DWORD g_dwIsIrcXTimeout			= 60;		// seconds
const DWORD g_dwLoginTimeout			= 60;		// seconds
const DWORD g_dwBigTimeout				= 10000;	// milliseconds
const DWORD g_dwSlowDownPause			= 100;		// milliseconds
const DWORD g_dwMaxMsgQueue				= 100;

// Global variables
extern TCHAR	g_szOOM[g_nMaxLengthError];
extern LONG		g_lMaxMsgLength;

#ifdef DEBUG
// Track the object ref counts
extern LONG		g_lCollectionRefCount;
extern LONG		g_lChannelRefCount;
extern LONG		g_lChatItemsRefCount;
#endif // DEBUG

// Global macros
#define bIsDigit(ch)			((ch >= '0') && (ch <= '9'))

#define bIsPmtData(pmt)			((pmt == pmtData) || (pmt == pmtDataRequest) || (pmt == pmtDataReply) || \
								 (pmt == pmtData+pmtDataRaw) || (pmt == pmtDataRequest+pmtDataRaw) || (pmt == pmtDataReply+pmtDataRaw))

#define bIsMsgtData(msgt)		((msgt == msgtData) || (msgt == msgtDataRequest) || (msgt == msgtDataReply) || \
								 (msgt == msgtData+msgtDataRaw) || (msgt == msgtDataRequest+msgtDataRaw) || (msgt == msgtDataReply+msgtDataRaw))

#define NCmdIdFromPmt(pmt)		((pmt & pmtData) ? cmdidData : (pmt & pmtDataRequest) ? cmdidRequest : cmdidReply)

#define NCmdIdFromMsgt(msgt)	((msgt & msgtData) ? cmdidData : (msgt & msgtDataRequest) ? cmdidRequest : cmdidReply)


// External common functions
extern BYTE		byteSizeOfVariant(VARTYPE vt);
extern LONG		SzToL(LPCTSTR szStr);
extern LONG		HexSzToL(LPCTSTR szStr);
extern INT		WStricmp(LPCWSTR wszStr1, LPCWSTR wszStr2);
extern LPTSTR	SzSubStr(LPCTSTR szStr1, LPCTSTR szStr2);
extern LPTSTR	SzReplaceStr(LPCTSTR szInStr, LPCTSTR szOldStr, LPCTSTR szNewStr);
extern BOOL		bCopyStr(LPTSTR *pszDest, LPCTSTR szSrc);
extern BOOL		bCopyChanStr(LPTSTR *pszDest, LPCTSTR szSrc);
extern BOOL		bOperateStr(LPTSTR *pszDest, SHORT nOpType, BOOL bExtendedNickname = FALSE);
extern SHORT	NGetPropertyIndex(SHORT nPropType, LPCWSTR wszPropertyName);
extern SHORT	NGetChannelPropertyIndex(LPCTSTR szChannelPropertyName);
extern BOOL		bEncodeMask(LPTSTR szSrc, LPTSTR *pszDst);
extern WORD		Sc2Sz(SCODE sc, LPTSTR szError, LPCTSTR szChannelName, LPCTSTR szNickname, BOOL bIrcX, BYTE byteCharSet);
extern SCODE	GetScodeFromHResult(HRESULT hr);
extern LPTSTR	SzIdentityFromUserNameAndIPAddress(LPCTSTR szUserName, LPCTSTR szIPAddress);
#endif //__CUTIL_H__
