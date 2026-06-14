//=--------------------------------------------------------------------------=
// llist.h:		CLList header file
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=

#ifndef _LLIST_H_
#define _LLIST_H_

#include <windows.h>

typedef struct tagCELL
{
	LPVOID		pvContent;
	tagCELL*	pcellNext;
} CELL;

class CLList
{
public:
	CLList(BOOL bProtected);
    CLList();
    ~CLList();

	BOOL		FProtect();
	void		Unprotect();
	BOOL		FAddFrontCell(LPVOID pvNewFrontCell);
	BOOL		FRemoveCell(LPVOID pvCell);
	BOOL		FDeleteList();
	BOOL		FDoActionOnList(short nActionID);
	BOOL		FFindCellFromData(short nSearchID, LPVOID pvData, LPVOID *ppvCell);
	BOOL		FFindCell(LPVOID pvCell1, LPVOID *ppvCell2);
	BOOL		FGetCell(short nPosition, LPVOID *ppvCell);
	BOOL		FIsEmpty()			{ return m_pcellHead == NULL; }
	DWORD		DwGetCellCount()	{ return m_dwCellCount; }

	void		Lock();
	void		Unlock();

	virtual BOOL FDoActionOnCell(short nActionID, LPVOID pvCell) { return TRUE; } 
	virtual BOOL FVerifySearchCriteria(short nSearchID, LPVOID pvData, LPVOID pvCell);
	virtual BOOL FIsEqual(LPVOID pvCell1, LPVOID pvCell2);
	virtual BOOL FDeleteCell(LPVOID pvCell);

#ifdef DEBUG
	void	DisplayList();
#endif // DEBUG

protected:
	CRITICAL_SECTION*	m_pcs;				// critical section used only if m_bProtected == TRUE
	BOOL				m_bProtected;
	CELL*				m_pcellHead;
	DWORD				m_dwCellCount;
};

#endif // _LLIST_H_
