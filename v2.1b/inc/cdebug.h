//=--------------------------------------------------------------------------=
// CDebug.H
//=--------------------------------------------------------------------------=
//
// contains the various macros and the like which are only useful in DEBUG
// builds
//
#ifndef __CDEBUG_H__

#include <windows.h>
#include <stdlib.h>
#include <tchar.h>
#include <stdio.h>
#include "Lock.H"

#undef ASSERT
#undef FAIL
//=---------------------------------------------------------------------------=
// all the things required to handle our ASSERT mechanism
//=---------------------------------------------------------------------------=
//
#ifdef DEBUG

// Function Prototypes
//
VOID DisplayAssert(LPSTR pszMsg, LPSTR pszAssert, LPSTR pszFile, UINT line);

// Macros
//
// *** Include this macro at the top of any source file using *ASSERT*() macros ***
//
#define SZTHISFILE	static char _szThisFile[] = __FILE__;


// our versions of the ASSERT and FAIL macros.
//
#define ASSERT(fTest, szMsg)                                \
    if (!(fTest))  {                                        \
        static char szMsgCode[] = szMsg;                    \
        static char szAssert[] = #fTest;                    \
        DisplayAssert(szMsgCode, szAssert, _szThisFile, __LINE__); \
    }

#define FAIL(szMsg)                                         \
        { static char szMsgCode[] = szMsg;                    \
        DisplayAssert(szMsgCode, "FAIL", _szThisFile, __LINE__); }



// macro that checks a pointer for validity on input
//
#define CHECK_POINTER(val) if (!(val) || IsBadWritePtr((void *)(val), sizeof(void *))) return E_POINTER

// RegisB added 04/09/96
const short		g_cbDebugStr = 600;
extern TCHAR	g_szDebugStr[g_cbDebugStr];
extern CLock	g_cs;

extern void Cop(short n);
extern void OutputDebugThreadIdString(char* szDebug);
extern void EnterCriticalSectionDebug(LPCRITICAL_SECTION lpCriticalSection);
extern void LeaveCriticalSectionDebug(LPCRITICAL_SECTION lpCriticalSection);
// End RegisB

#else  // DEBUG

#define SZTHISFILE
#define ASSERT(fTest, err)
#define FAIL(err)
#define CHECK_POINTER(val)

// RegisB added 04/09/96
#define Cop(n)
#define OutputDebugThreadIdString(szDebug)
#define EnterCriticalSectionDebug(lpCriticalSection) EnterCriticalSection(lpCriticalSection)
#define LeaveCriticalSectionDebug(lpCriticalSection) LeaveCriticalSection(lpCriticalSection)
// End RegisB

#endif	// DEBUG

#define __CDEBUG_H__
#endif // __CDEBUG_H__
