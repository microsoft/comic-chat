//=--------------------------------------------------------------------------=
// CComp.H
//=--------------------------------------------------------------------------=
// Copyright  1997 - 1998 Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=
//
// Declaration of constants and types for user matching code
//
#ifndef __CCOMP_H__

#include <windows.h>
#include <tchar.h>

// Break down of a nickname!username@ipaddress mask for user comparing.
typedef struct tagPRUSERMATCH
{
	LPTSTR	szTheMask;		// "?usti?!regisb@*.microsoft.com"
    LPTSTR	szNickname;		// "?usti?"
    UINT	cbNickname;		// 6
    LPTSTR	szUserName;		// "regisb"
    UINT	cbUserName;		// 6
    LPTSTR	szIPAddress;	// "*.microsoft.com"
    UINT	cbIPAddress;	// 15
} PRUSERMATCH, *PPRUSERMATCH;

// External routines
extern BOOL	bMatchAll(LPTSTR szString, UINT cbLen);
extern BOOL bGetUserMatchFromMask(LPTSTR szIdentMask, PPRUSERMATCH pPrUserMatch);
extern BOOL bIsMaskCompare(LPCTSTR szMask, UINT cbMask, LPCTSTR szString, UINT cbString);
extern BOOL bIsMatch(PPRUSERMATCH pPrUserMatch, LPCTSTR szNickname, LPCTSTR szUserName, LPCTSTR szIPAddress);

#define __CCOMP_H__
#endif __CCOMP_H__
