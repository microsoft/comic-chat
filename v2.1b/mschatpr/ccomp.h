//=--------------------------------------------------------------------------=
// CComp.H
//=--------------------------------------------------------------------------=
// Copyright  1997  Microsoft Corporation.  All Rights Reserved.
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
    BOOL	bNickname;		// TRUE, wildcard(s) in nickname
    LPTSTR	szUserName;		// "regisb"
    UINT	cbUserName;		// 6
    BOOL	bUserName;		// FALSE, no wildcard in username
    LPTSTR	szIPAddress;	// "*.microsoft.com"
    UINT	cbIPAddress;	// 15
    BOOL	bIPAddress;		// TRUE
    BOOL	bAll;			// FALSE, does not match to everything
} PRUSERMATCH, *PPRUSERMATCH;

// External routines
extern BOOL bGetUserMatchFromMask(LPTSTR szIdentMask, PPRUSERMATCH pPrUserMatch);
extern BOOL bIsMaskCompare(LPCTSTR szMask, UINT cbMask, LPCTSTR szString, UINT cbString);
extern BOOL bIsMatch(PPRUSERMATCH pPrUserMatch, LPCTSTR szNickname, LPCTSTR szUserName, LPCTSTR szIPAddress);

typedef struct
{
    PCSTR   pNick;
    UINT    cNick;
    BOOL    fNick;
    PCSTR   pUserid;
    UINT    cUserid;
    BOOL    fUserid;
    PCSTR   pHost;
    UINT    cHost;
    BOOL    fHost;
    PCSTR   pServer;
    UINT    cServer;
    BOOL    fServer;
    BOOL    fAll;
    BOOL    fSysop;
} DATAUSERMATCH;

// BOOL LibParseUserMatch(PCSTR pMask, UINT cMask, DATAUSERMATCH & Match);

#define __CCOMP_H__
#endif __CCOMP_H__
