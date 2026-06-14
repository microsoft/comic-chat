#ifndef __CCOMMON_H__
#define __CCOMMON_H__

#include <windows.h>
#include <tchar.h>
#include "richedit.h"
#include "fechrcnv.h"

// String length constants
const SHORT g_nMaxLengthSmall	= 31;	// small string buffers
const SHORT g_nMaxLength		= 255;	// large string buffers
const SHORT g_nMaxChanBuff		= 200;	// Max channel name length

// Various characters and strings
const TCHAR g_szAnon[]			= _T("ANON");
const TCHAR g_szMSN[]			= _T("MSN");
const TCHAR g_szDPA[]			= _T("DPA");
const TCHAR g_szNTLM[]			= _T("NTLM");
const TCHAR g_szNoMachine[]		= _T("NoMachine");
const TCHAR g_szEmpty[]			= _T("");
const TCHAR g_szLF[]			= _T("\n");
const TCHAR g_szCR[]			= _T("\r");
const TCHAR g_szSpace[]			= _T(" ");

const TCHAR g_chEOS				= _TCHAR('\0');
const TCHAR	g_chComma			= _TCHAR(',');
const TCHAR g_chSpace			= _TCHAR(' ');
const TCHAR g_chBell			= _TCHAR('\a');
const TCHAR g_chTab				= _TCHAR('\t');
const TCHAR g_chBackSlash		= _TCHAR('\\');
const TCHAR g_chLF				= _TCHAR('\n');
const TCHAR g_chCR				= _TCHAR('\r');
const TCHAR g_chExtNckPfx		= _TCHAR('\'');
const TCHAR g_chExtChnPfx		= _TCHAR('%');
const TCHAR g_chGblChnPfx		= _TCHAR('#');
const TCHAR g_chLclChnPfx		= _TCHAR('&');
const TCHAR g_chLLQuoteIRCX		= _TCHAR('\\');
const TCHAR g_chAtSign			= _TCHAR('@');
const TCHAR g_chLLQuoteCTCP		= 0x10;
const TCHAR g_chTransparent		= 0x01;

// External common functions
extern BOOL		bExtendedString(LPCTSTR szInStr);
extern BOOL		bExtendedWideString(LPCWSTR wszInStr);
extern BOOL		bExtendedChannelName(LPCTSTR szChannelName, BOOL bAcceptUpperAnsi = FALSE);
extern BOOL		bExtendedWideChannelName(LPCWSTR wszChannelName);
extern BOOL		bExtendedNickname(LPCTSTR szNickname);
extern BOOL		bExtendedWideNickname(LPCWSTR wszNickname);
extern BOOL		bConvertWideStringToUTF8(LPCWSTR wszInStr, INT cchIn, LPTSTR *pszOutStr, INT *pcchOut, BOOL bNickname = FALSE, BOOL bChannelName = FALSE, BOOL bPostProcess = TRUE, BOOL bEscapeWildcards = FALSE);
extern BOOL		bConvertUTF8StringToWide(LPCTSTR szInStr, INT cchIn, LPWSTR *pwszOutStr, INT *pcchOut, BOOL bNickname = FALSE, BOOL bChannelName = FALSE, BOOL bPostProcess = TRUE);
extern LPCTSTR	SzNextUTF8Char(LPCTSTR szInStr);

extern BOOL		bSB2DBKatakana(CHAR *szInStr, INT cchIn, CHAR **pszOutStr, INT *pcchOut, BOOL *pbFree);
extern BOOL		bConvertString(BOOL bIncomingText, BYTE byteCharSet, CHAR *szInStr, INT cchIn, CHAR **pszOutStr, INT *pcchOut, BOOL *pbFree);
extern BOOL		bDataToString(PBYTE pbSrc, UINT cbSrc, LPTSTR *pszDst, BOOL bAfterColon);
extern BOOL		bStringToData(LPTSTR szSrc, PBYTE pbDst, UINT *pcbDst);
extern BOOL		bLowLevelQuoting(TCHAR chQuotingChar, BOOL bTreatAsByteArray, LPCTSTR szSrc, LPTSTR *pszDst, BOOL *pbFree, BOOL bRemoveCarriageReturns = FALSE);
extern BOOL		bLowLevelUnquoting(TCHAR chQuotingChar, BOOL bTreatAsByteArray, LPCTSTR szSrc, LPTSTR szDst);

#endif //__CCOMMON_H__
