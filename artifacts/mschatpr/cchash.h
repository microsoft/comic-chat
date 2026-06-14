//=--------------------------------------------------------------------------=
// CCHash.H		// Custom Hash Table classes
//=--------------------------------------------------------------------------=
// Copyright  1997  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=
//
// class declarations for the Chat control.
//
#ifndef __CCHASH_H__

#include "CHash.H"

//=--------------------------------------------------------------------------=
// CHashMember class
//=--------------------------------------------------------------------------=
class CHashMember : public CHashTableString
{
public:

	CHashMember(DWORD dwBlockSize = CDEFBLOCK) : CHashTableString(dwBlockSize) { m_hwndMess = NULL; }
	~CHashMember() {};

	virtual BOOL	bDeleteUserData(PVOID pvContent);

#ifdef DEBUG
	void			DumpMemberList(void);
#endif // DEBUG

private:

	HWND	m_hwndMess;
};


#define __CCHASH_H__
#endif // __CCHASH_H__
