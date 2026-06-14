//=--------------------------------------------------------------------------=
// CLList.H		// Custom Linked List classes
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=
//
// class declarations for the Chat control.
//
#ifndef __CLLIST_H__

#include "LList.H"


//=--------------------------------------------------------------------------=
// Member list class
//=--------------------------------------------------------------------------=
class CMemLList : public CLList
{
public:

	CMemLList() { m_dwUserData = 0L; }
	CMemLList(DWORD dwUserData) { m_dwUserData = dwUserData; }
	~CMemLList() {};

	void	SetUserData(DWORD dwUserData) { m_dwUserData = dwUserData; }
	DWORD	GetUserData() { return m_dwUserData; }

	virtual BOOL FDoActionOnCell(short nActionID, LPVOID pvCell);
	virtual BOOL FVerifySearchCriteria(short nSearchID, LPVOID pvData, LPVOID pvCell);
	virtual BOOL FIsEqual(LPVOID pvCell1, LPVOID pvCell2);
	virtual BOOL FDeleteCell(LPVOID pvCell);

private:
	DWORD m_dwUserData;
};


//=--------------------------------------------------------------------------=
// Message list class
//=--------------------------------------------------------------------------=
class CMsgLList : public CLList
{
public:

	CMsgLList() { m_dwUserData = 0L; }
	~CMsgLList() {};

	void	SetUserData(DWORD dwUserData) { m_dwUserData = dwUserData; }
	DWORD	GetUserData() { return m_dwUserData; }

	virtual BOOL FDoActionOnCell(short nActionID, LPVOID pvCell);
	virtual BOOL FIsEqual(LPVOID pvCell1, LPVOID pvCell2);
	virtual BOOL FDeleteCell(LPVOID pvCell);

private:
	DWORD m_dwUserData;
};

#define __CLLIST_H__
#endif // __CLLIST_H__
