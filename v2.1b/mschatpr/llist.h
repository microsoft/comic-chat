//=--------------------------------------------------------------------------=
// llist.h:		CLList header file
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

#ifndef __LLIST_H__
#define __LLIST_H__

#include <windows.h>

typedef struct tagCELL
{
	PVOID		pvContent;
	tagCELL*	pcellNext;
} CELL;

class CLList
{
public:
	CLList(BOOL bProtected);
    CLList();
    ~CLList();

			BOOL	bProtect(void);
			void	Unprotect(void);
			BOOL	bAddFrontCell(PVOID pvNewFrontCell);
			BOOL	bInsertCell(LONG lPosition, PVOID pvNewCell);
			BOOL	bRemoveCell(PVOID pvCell);
			BOOL	bDeleteList(void);
			BOOL	bDoActionOnList(SHORT nActionID);
			BOOL	bFindCellFromData(LONG lPosition, SHORT nSearchID1, PVOID pvData1, SHORT nSearchID2, PVOID pvData2, PVOID *ppvCell, LONG* plPositionFound);
			BOOL	bFindCell(PVOID pvCell1, PVOID *ppvCell2);
			BOOL	bGetCell(LONG lPosition, PVOID *ppvCell);
			BOOL	bIsEmpty(void) { return m_pcellHead == NULL; }
			DWORD	DwGetCellCount(void);

			void	Lock(void);
			void	Unlock(void);

	virtual BOOL	bDoActionOnCell(SHORT nActionID, PVOID pvCell) { return FALSE; } 
	virtual BOOL	bVerifySearchCriteria(SHORT nSearchID, PVOID pvData, PVOID pvCell) { return FALSE; }
	virtual BOOL	bIsEqual(PVOID pvCell1, PVOID pvCell2) = 0;
	virtual BOOL	bDeleteCell(PVOID pvCell);

#ifdef DEBUG
			void	DisplayList(void);
#endif // DEBUG

protected:
	CRITICAL_SECTION*	m_pcs;				// critical section used only if m_bProtected == TRUE
	BOOL				m_bProtected;
	CELL*				m_pcellHead;
	DWORD				m_dwCellCount;
};

#endif // __LLIST_H__
