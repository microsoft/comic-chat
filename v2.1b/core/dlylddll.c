//*********************************************************************
//*                  Microsoft Windows                               **
//*            Copyright(c) Microsoft Corp., 1994-1996               **
//*********************************************************************
//
// DLYLDDLL.C - uses macros for delay loading of DLLs
//

#include <windows.h>
#include <assert.h>

#define ASSERT(h)	assert(h)

// coded copied from SHDOCVW's dllload.c file

 // No warnings when modifiers used on data
#pragma warning(disable:4229) 

#define ENSURE_LOADED(_hinst, _dll)   (_hinst ? TRUE : (BOOL)(_hinst = LoadLibrary(#_dll)))
#define DELAY_LOAD_ERR(_hinst, _dll, _ret, _fn, _args, _nargs, _err) \
_ret __stdcall _fn _args                \
{                                       \
    static _ret (CALLBACK* __stdcall _pfn##_fn) _args = NULL;   \
    if (!ENSURE_LOADED(_hinst, _dll))   \
    {                                   \
        ASSERT((BOOL)_hinst); \
        return (_ret)_err;                      \
    }                                   \
    if (_pfn##_fn == NULL)              \
    {                                   \
        *(FARPROC*)&(_pfn##_fn) = GetProcAddress(_hinst, #_fn); \
        ASSERT((BOOL)_pfn##_fn); \
        if (_pfn##_fn == NULL)		\
            return (_ret)_err;          \
    }                                   \
    return _pfn##_fn _nargs;            \
 }

#define DELAY_LOAD(_hinst, _dll, _ret, _fn, _args, _nargs) DELAY_LOAD_ERR(_hinst, _dll, _ret, _fn, _args, _nargs, 0)
#define DELAY_LOAD_HRESULT(_hinst, _dll, _fn, _args, _nargs) DELAY_LOAD_ERR(_hinst, _dll, HRESULT, _fn, _args, _nargs, E_FAIL)
#define DELAY_LOAD_SAFEARRAY(_hinst, _dll, _fn, _args, _nargs) DELAY_LOAD_ERR(_hinst, _dll, SAFEARRAY *, _fn, _args, _nargs, NULL)
#define DELAY_LOAD_UINT(_hinst, _dll, _fn, _args, _nargs) DELAY_LOAD_ERR(_hinst, _dll, UINT, _fn, _args, _nargs, 0)

#define DELAY_LOAD_VOID(_hinst, _dll, _fn, _args, _nargs) \
void __stdcall _fn _args                \
{                                       \
    static void (* __stdcall _pfn##_fn) _args = NULL;   \
    if (!ENSURE_LOADED(_hinst, _dll))   \
    {                                   \
        ASSERT((BOOL)_hinst); \
        return;                         \
    }                                   \
    if (_pfn##_fn == NULL)              \
    {                                   \
        *(FARPROC*)&(_pfn##_fn) = GetProcAddress(_hinst, #_fn); \
        ASSERT((BOOL)_pfn##_fn); \
        if (_pfn##_fn == NULL)		\
            return;                     \
    }                                   \
    _pfn##_fn _nargs;                   \
 }



// For private entrypoints exported by ordinal.
#define DELAY_LOAD_ORD_ERR(_hinst, _dll, _ret, _fn, _ord, _args, _nargs, _err) \
_ret __stdcall _fn _args                \
{                                       \
    static _ret (* __stdcall _pfn##_fn) _args = NULL;   \
    if (!ENSURE_LOADED(_hinst, _dll))   \
    {                                   \
        ASSERT((BOOL)_hinst); \
        return (_ret)_err;                      \
    }                                   \
    if (_pfn##_fn == NULL)              \
    {                                   \
        *(FARPROC*)&(_pfn##_fn) = GetProcAddress(_hinst, (LPSTR) _ord); \
        ASSERT((BOOL)_pfn##_fn); \
        if (_pfn##_fn == NULL)		\
            return (_ret)_err;          \
    }                                   \
    return _pfn##_fn _nargs;            \
 }

#define DELAY_LOAD_ORD(_hinst, _dll, _ret, _fn, _ord, _args, _nargs) DELAY_LOAD_ORD_ERR(_hinst, _dll, _ret, _fn, _ord, _args, _nargs, 0)


#define DELAY_LOAD_VOID_ORD(_hinst, _dll, _fn, _ord, _args, _nargs) \
void __stdcall _fn _args                \
{                                       \
    static void (* __stdcall _pfn##_fn) _args = NULL;   \
    if (!ENSURE_LOADED(_hinst, _dll))   \
    {                                   \
        ASSERT((BOOL)_hinst); \
        return;                         \
    }                                   \
    if (_pfn##_fn == NULL)              \
    {                                   \
        *(FARPROC*)&(_pfn##_fn) = GetProcAddress(_hinst, (LPSTR) _ord); \
        ASSERT((BOOL)_pfn##_fn); \
        if (_pfn##_fn == NULL)		\
            return;                     \
    }                                   \
    _pfn##_fn _nargs;                   \
 }


//--------- msrating.dll ----------------

HINSTANCE g_hinstRatings = NULL;
const TCHAR c_tszRatingsDLL[] = TEXT("MSRATING.DLL");

DELAY_LOAD(g_hinstRatings, MSRATING.DLL, HRESULT, RatingAccessDeniedDialog,
           (HWND hwndParent, LPCTSTR pszUsername, LPCTSTR pszContentDescription, LPVOID pRatingDetails),
           (hwndParent,pszUsername,pszContentDescription,pRatingDetails));

DELAY_LOAD(g_hinstRatings, MSRATING.DLL, HRESULT, RatingCheckUserAccess,
           (LPCTSTR pszUsername, LPCTSTR pszURL, LPCTSTR pszRatingInfo, LPBYTE pData, DWORD cbData, LPVOID *ppRatingDetails),
           (pszUsername, pszURL, pszRatingInfo, pData, cbData, ppRatingDetails));

DELAY_LOAD(g_hinstRatings, MSRATING.DLL, HRESULT, RatingEnable,
           (HWND hwndParent, LPCSTR pszUsername, BOOL fEnable),
           (hwndParent,pszUsername,fEnable));

DELAY_LOAD(g_hinstRatings, MSRATING.DLL, HRESULT, RatingEnabledQuery,
           (), ());

DELAY_LOAD(g_hinstRatings, MSRATING.DLL, HRESULT, RatingFreeDetails,
           (LPVOID pRatingDetails), 
	    (pRatingDetails));

DELAY_LOAD(g_hinstRatings, MSRATING.DLL, HRESULT, RatingObtainCancel,
           (HANDLE hRatingObtainQuery), 
	    (hRatingObtainQuery));

DELAY_LOAD(g_hinstRatings, MSRATING.DLL, HRESULT, RatingObtainQuery,
           (LPCTSTR pszTargetURL, DWORD dwUserData,void (*fCallback)(DWORD dwUserData, HRESULT hr, LPCTSTR pszRating, void *lpvRatingDetails), HANDLE *phRatingObtainQuery ), 
	    (pszTargetURL, dwUserData, fCallback, phRatingObtainQuery));

DELAY_LOAD(g_hinstRatings, MSRATING.DLL, HRESULT, RatingSetupUI,
           (HWND hDlg, LPCSTR pszUsername),
           (hDlg, pszUsername));



//--------- wininet.dll ----------------

HINSTANCE g_hinstWinInet = NULL;
const TCHAR c_tszWinInetDLL[] = TEXT("WININET.DLL");

DELAY_LOAD(g_hinstWinInet, WININET.DLL, BOOL, InternetCanonicalizeUrlA,
           (LPCTSTR lpszUrl, LPTSTR lpszBuffer, LPDWORD lpdwBufferLength, DWORD dwFlags),
           (lpszUrl, lpszBuffer, lpdwBufferLength, dwFlags));

DELAY_LOAD(g_hinstWinInet, WININET.DLL, BOOL, InternetCanonicalizeUrlW,
           (LPCTSTR lpszUrl, LPTSTR lpszBuffer, LPDWORD lpdwBufferLength, DWORD dwFlags),
           (lpszUrl, lpszBuffer, lpdwBufferLength, dwFlags));


//--------- msconf.dll ----------------

HINSTANCE g_hinstMSConf = NULL;
const TCHAR c_tszMSConfDLL[] = TEXT("MSCONF.DLL");

#include "..\inc\msconf.h"

DELAY_LOAD(g_hinstMSConf, MSCONF.DLL, DWORD, ConferenceConnectA,
           (HCONF *phConf, LPCONFADDRA lpConfAddr, LPCONFINFO lpConfInfo, LPCONFNOTIFY lpConfNotify),
           (phConf, lpConfAddr, lpConfInfo, lpConfNotify));


DELAY_LOAD(g_hinstMSConf, MSCONF.DLL, DWORD, ConferenceGetInfoA,
           (HCONF hConf, DWORD dwCode, LPVOID lpv),
           (hConf, dwCode, lpv));

DELAY_LOAD(g_hinstMSConf, MSCONF.DLL, DWORD, ConferenceListen,
		   (DWORD dwArg),
		   (dwArg));

