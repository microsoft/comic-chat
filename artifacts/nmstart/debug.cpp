//	*****************************************************************
//	*																*
//	*	Module 			: Debug.cpp  								*
//	*																*
//	*	Author 			: RegisB, 07/14/97							*
//	*																*
//	*	Current Owner	: RegisB									*
//	*																*
//	*****************************************************************

#include "StdAfx.H"
#include "Debug.H"

// for ASSERT and FAIL
//
SZTHISFILE

#ifdef DEBUG
//
// contains various methods that will only really see any use in DEBUG builds
//

//=--------------------------------------------------------------------------=
// Private Constants
//---------------------------------------------------------------------------=
//
static const char szFormat[]  = "%s\nFile %s, Line %d";
static const char szFormat2[] = "%s\n%s\nFile %s, Line %d";

#define _SERVERNAME_ "ActiveX Framework"

static const char szTitle[]  = _SERVERNAME_ " Assertion  (Abort = UAE, Retry = INT 3, Ignore = Continue)";


//=--------------------------------------------------------------------------=
// Local functions
//=--------------------------------------------------------------------------=
int NEAR _IdMsgBox(LPSTR pszText, LPCSTR pszTitle, UINT mbFlags);

//=--------------------------------------------------------------------------=
// DisplayAssert
//=--------------------------------------------------------------------------=
// Display an assert message box with the given pszMsg, pszAssert, source
// file name, and line number. The resulting message box has Abort, Retry,
// Ignore buttons with Abort as the default.  Abort does a FatalAppExit;
// Retry does an int 3 then returns; Ignore just returns.
//
VOID DisplayAssert
(
    LPSTR	 pszMsg,
    LPSTR	 pszAssert,
    LPSTR	 pszFile,
    UINT	 line
)
{
    char	szMsg[250];
    LPSTR	lpszText;

    lpszText = pszMsg;		// Assume no file & line # info

    // If C file assert, where you've got a file name and a line #
    //
    if (pszFile) {

        // Then format the assert nicely
        //
        wsprintf(szMsg, szFormat, (pszMsg&&*pszMsg) ? pszMsg : pszAssert, pszFile, line);
        lpszText = szMsg;
    }

    // Put up a dialog box
    //
    switch (_IdMsgBox(lpszText, szTitle, MB_ICONHAND|MB_ABORTRETRYIGNORE|MB_SYSTEMMODAL)) {
        case IDABORT:
            FatalAppExit(0, lpszText);
            return;

        case IDRETRY:
            // call the win32 api to break us.
            //
            DebugBreak();
            return;
    }

    return;
}


//=---------------------------------------------------------------------------=
// Beefed-up version of WinMessageBox.
//=---------------------------------------------------------------------------=
//
int NEAR _IdMsgBox
(
    LPSTR	pszText,
    LPCSTR	pszTitle,
    UINT	mbFlags
)
{
    HWND hwndActive;
    int  id;

    hwndActive = GetActiveWindow();

    id = MessageBox(hwndActive, pszText, pszTitle, mbFlags);

    return id;
}


// Debug functions
TCHAR		g_szDebugStr[g_cbDebugStr];
CLock		g_cs;

void Cop(short n)
{
	_stprintf(g_szDebugStr, _T("Cop# %d Enter "), n);
	OutputDebugString(g_szDebugStr);
	HGLOBAL hCop;

	hCop = GlobalAlloc(GMEM_MOVEABLE, 4000);
	if (hCop)
		GlobalFree(hCop);
	OutputDebugString(_T("& Leave\n"));
}

void OutputDebugThreadIdString(char* szDebug)
{
	TCHAR szConcat[g_cbDebugStr];

	g_cs.Lock();

	_stprintf(szConcat, _T("%X: %s"), GetCurrentThreadId(), szDebug);
	OutputDebugString(szConcat);

	//Cop(0);
	
	g_cs.Unlock();

	return;
}

void EnterCriticalSectionDebug(LPCRITICAL_SECTION lpCriticalSection)
{
//	_stprintf(g_szDebugStr, _T("Entering CS %ld\n"), lpCriticalSection);
//	OutputDebugThreadIdString(g_szDebugStr);

	EnterCriticalSection(lpCriticalSection);
}

void LeaveCriticalSectionDebug(LPCRITICAL_SECTION lpCriticalSection)
{
//	_stprintf(g_szDebugStr, _T("Leaving CS %ld\n"), lpCriticalSection);
//	OutputDebugThreadIdString(g_szDebugStr);

	LeaveCriticalSection(lpCriticalSection);
}

#endif // DEBUG
