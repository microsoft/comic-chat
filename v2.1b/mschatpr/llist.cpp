//=--------------------------------------------------------------------------=
// LList.Cpp:		Implementation of the CLList class.
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

#include "StdAfx.H"
#include "CUtil.H"
#include "LList.H"
#include "CDebug.H"

// for ASSERT and FAIL
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CLList::CLList - Constructor
CLList::CLList(void)
{
	m_pcellHead		= NULL;
	m_dwCellCount	= 0L;
	m_bProtected	= FALSE;
	m_pcs			= NULL;
}

CLList::CLList(BOOL bProtected)
{
	m_pcellHead		= NULL;
	m_dwCellCount	= 0L;
	m_bProtected	= bProtected;
	m_pcs			= new CRITICAL_SECTION;

	if (m_pcs)
		InitializeCriticalSection(m_pcs);
}


/////////////////////////////////////////////////////////////////////////////
// CLList::~CLList - Destructor
CLList::~CLList(void)
{
	//OutputDebugThreadIdString("CLList::~CLList - Enter\n");

	bDeleteList();

	if (m_pcs)
	{
		DeleteCriticalSection(m_pcs);
		delete [] m_pcs;
		m_pcs = NULL;
	}

	//OutputDebugThreadIdString("CLList::~CLList - Leave\n");
}


void CLList::Lock(void)
{ 
	if (m_pcs) EnterCriticalSectionDebug(m_pcs);
}


void CLList::Unlock(void)
{
	if (m_pcs) LeaveCriticalSectionDebug(m_pcs);
}


BOOL CLList::bProtect(void)
{
	ASSERT(!m_pcs, "m_pcs is NOT NULL in CLList::bProtect");

	m_pcs = new CRITICAL_SECTION;

	if (m_pcs)
	{
		m_bProtected = TRUE;
		InitializeCriticalSection(m_pcs);
		return TRUE;
	}
	else
		return FALSE;
}


void CLList::Unprotect(void)
{
	ASSERT(m_pcs, "m_pcs is NULL in CLList::bUnprotect");

	if (m_pcs)
	{
		DeleteCriticalSection(m_pcs);
		delete [] m_pcs;
		m_pcs = NULL;
	}
	m_bProtected = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::DwGetCellCount
//
//	returns number of cells in the linked list
DWORD CLList::DwGetCellCount(void)
{
	DWORD dwCellCount = 0L;

	Lock();

	dwCellCount = m_dwCellCount;

	Unlock();

	return dwCellCount;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::bAddFrontCell
//
//	returns TRUE if successful 
//          FALSE if OOM or pvNewFrontCell is NULL or protection error
BOOL CLList::bAddFrontCell(PVOID pvNewFrontCell)
{
	CELL *pcellNew;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::bAddFrontCell");
		return FALSE;
	}

	if (!pvNewFrontCell)
	{
		ASSERT(FALSE, "!pvNewFrontCell in CLList::bAddFrontCell");
		return FALSE;
	}

	//Lock();
	//OutputDebugThreadIdString("CLList::bAddFrontCell\n");
	//DisplayList();

	pcellNew = new CELL;
	if (!pcellNew)
	{
		ASSERT(FALSE, "!pcellNew in CLList::bAddFrontCell");
		return FALSE;
	}

	pcellNew->pvContent = pvNewFrontCell;

	Lock();

	pcellNew->pcellNext = m_pcellHead;
	m_pcellHead = pcellNew;

	m_dwCellCount++;

	Unlock();

	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "CLList::bAddFrontCell - Successfully added cell content: %ld\n", pvNewFrontCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif // DEBUG

	//DisplayList();
	//Unlock();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::bInsertCell - adds a cell inside the list at position lPosition
BOOL CLList::bInsertCell(LONG lPosition, PVOID pvNewCell)
{
	CELL *pcellNew;
	CELL *pcellCur, *pcellPrev = NULL;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::bInsertCell");
		return FALSE;
	}

	if (!pvNewCell)
	{
		ASSERT(FALSE, "!pvNewCell in CLList::bInsertCell");
		return FALSE;
	}

	pcellNew = new CELL;
	if (!pcellNew)
	{
		ASSERT(FALSE, "!pcellNew in CLList::bInsertCell");
		return FALSE;
	}

	pcellNew->pvContent = pvNewCell;

	Lock();

	pcellCur = m_pcellHead;

	for (LONG lCnt = 1L; lCnt < lPosition; lCnt++)
		if (pcellCur)
		{
			pcellPrev = pcellCur;
			pcellCur = pcellCur->pcellNext;
		}
		else
			break;

	pcellNew->pcellNext = pcellCur;

	if (pcellPrev)
		pcellPrev->pcellNext = pcellNew;
	else
		m_pcellHead = pcellNew;

	m_dwCellCount++;

	Unlock();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::bRemoveCell - removes a cell fron the linked list
BOOL CLList::bRemoveCell(PVOID pvCell)
{
	CELL *pcellCur, *pcellPrev;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::bRemoveCell");
		return FALSE;
	}

	if (!pvCell)
	{
		ASSERT(FALSE, "!pvCell in CLList::bRemoveCell");
		return FALSE;
	}

	//Lock();
	//OutputDebugThreadIdString("CLList::bRemoveCell\n");
	//DisplayList();

	pcellPrev = NULL;

	Lock();

	pcellCur = m_pcellHead;

	while (pcellCur && !bIsEqual(pcellCur->pvContent, pvCell))
	{
		pcellPrev = pcellCur;
		pcellCur = pcellCur->pcellNext;
	}

	if (!pcellCur || !bDeleteCell(pcellCur->pvContent))
	{
		Unlock();
		return FALSE;
	}

	// reset value of head
	if (!pcellPrev)
		// deleting head of list m_pcellHead == pcellCur
		m_pcellHead = pcellCur->pcellNext;
	else
		// deleting middle cell
		pcellPrev->pcellNext = pcellCur->pcellNext;

	m_dwCellCount--;

	Unlock();

	// delete the cell itself
	delete pcellCur;
	pcellCur = NULL;

	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "CLList::bRemoveCell - Successfully removed cell content: %ld\n", pvCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif // DEBUG

	//DisplayList();
	//Unlock();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::bDeleteList - delete the whole list
BOOL CLList::bDeleteList(void)
{
	CELL *pcellCur, *pcellPrev;

	// OutputDebugThreadIdString("CLList::bDeleteList - Enter\n");

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::bDeleteList");
		return FALSE;
	}

	if (!m_pcellHead)
		return TRUE;
	
	Lock();

	pcellCur = m_pcellHead;
	while (pcellCur)
	{
		if (!bDeleteCell(pcellCur->pvContent))
		{
			Unlock();
			ASSERT(FALSE, "Could not delete cell content in CLList::bDeleteList");
			return FALSE;
		}
		pcellPrev = pcellCur;
		pcellCur = pcellCur->pcellNext;
		delete pcellPrev;
		pcellPrev = NULL;
	}

	m_pcellHead = NULL;
	m_dwCellCount = 0L;

	Unlock();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CLList::bDoActionOnList - 
BOOL CLList::bDoActionOnList(SHORT nActionID)
{
	BOOL bActionItem = TRUE;
	CELL *pcellCur;
	
	// OutputDebugThreadIdString("CLList::bDoActionOnList\n");

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::bDoActionOnList");
		return FALSE;
	}

	Lock();

	pcellCur = m_pcellHead;
	while (pcellCur && bActionItem)
	{
		bActionItem = bDoActionOnCell(nActionID, pcellCur->pvContent);
		pcellCur = pcellCur->pcellNext;
	}

	Unlock();

	return bActionItem;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::bDeleteCell - virtual function to destroy a single cell
BOOL CLList::bDeleteCell(PVOID pvCell)
{
	if (pvCell)
	{
		delete pvCell;
		pvCell = NULL;
	}
	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "CLList::bDeleteCell - Successfully deleted cell: %ld\n", pvCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif // DEBUG
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::bFindCellFromData - 
BOOL CLList::bFindCellFromData(LONG lPosition, 
							   SHORT nSearchID1, PVOID pvData1, 
							   SHORT nSearchID2, PVOID pvData2, 
							   PVOID *ppvCell, 
							   LONG* plPositionFound)
{
	LONG	lCount = 0L;
	LONG	lPositionFound = 1L;
	CELL	*pcellCur = NULL, *pcellGood = NULL;

	if (!ppvCell)
	{
		ASSERT(FALSE, "!ppvCell in CLList::bFindCellFromData");
		return FALSE;
	}

	*ppvCell = NULL;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::bFindCellFromData");
		return FALSE;
	}

	if (plPositionFound)
		*plPositionFound = 0L;

	Lock();

	pcellCur = m_pcellHead;

	if (lPosition > 0L)
	{
		// find Nth match
		while (lCount < lPosition)
		{
			while (pcellCur && (!bVerifySearchCriteria(nSearchID1, pvData1, pcellCur->pvContent) || 
								!bVerifySearchCriteria(nSearchID2, pvData2, pcellCur->pvContent)))
			{
				pcellCur = pcellCur->pcellNext;
				lPositionFound++;
			}
			pcellGood = pcellCur;
			lCount++;
			if (pcellCur && lCount < lPosition)
			{
				pcellCur = pcellCur->pcellNext;
				lPositionFound++;
			}
		}
	}
	else
	{
		LONG lCnt = 1L;
		// find last match
		while (pcellCur)
		{
			if (bVerifySearchCriteria(nSearchID1, pvData1, pcellCur->pvContent) &&
				bVerifySearchCriteria(nSearchID2, pvData2, pcellCur->pvContent))
			{
				pcellGood = pcellCur;
				lPositionFound = lCnt;
			}
			pcellCur = pcellCur->pcellNext;
			lCnt++;
		}
	}

	if (!pcellGood)
	{
		Unlock();
		return FALSE;
	}

	*ppvCell = pcellGood->pvContent;
	if (plPositionFound)
		*plPositionFound = lPositionFound;

	Unlock();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::bFindCell - 
BOOL CLList::bFindCell(PVOID pvCell1, PVOID *ppvCell2)
{
	CELL *pcellCur;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::bFindCell");
		return FALSE;
	}

	if (!pvCell1 || !ppvCell2)
	{
		ASSERT(FALSE, "!pvCell1 || !ppvCell2 in CLList::bFindCell");
		return FALSE;
	}

	Lock();

	pcellCur = m_pcellHead;

	while (pcellCur && !bIsEqual(pcellCur->pvContent, pvCell1))
		pcellCur = pcellCur->pcellNext;

	if (!pcellCur)
	{
		Unlock();
		return FALSE;
	}

	*ppvCell2 = pcellCur->pvContent;
	Unlock();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::bGetCell - 
BOOL CLList::bGetCell(LONG lPosition, PVOID *ppvCell)
{
	CELL *pcellCur;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::bGetCell");
		return FALSE;
	}

	if (!ppvCell)
	{
		ASSERT(FALSE, "!ppvCell in CLList::bGetCell");
		return FALSE;
	}

	*ppvCell = NULL;

	Lock();

	pcellCur = m_pcellHead;

	if (lPosition > 0L)
	{
		for (LONG lCnt = 1L; lCnt < lPosition; lCnt++)
			if (pcellCur)
				pcellCur = pcellCur->pcellNext;
			else
				break;
	}
	else
		// get the last cell in the list
		while (pcellCur && pcellCur->pcellNext)
			pcellCur = pcellCur->pcellNext;

	if (!pcellCur)
	{
		Unlock();
		return FALSE;
	}

	*ppvCell = pcellCur->pvContent;

	Unlock();
	return TRUE;
}


#ifdef DEBUG
/////////////////////////////////////////////////////////////////////////////
// CLList::DisplayList - 
void CLList::DisplayList(void)
{
	CELL *pcellCur;
	
	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::DisplayList");
		return;
	}

	Lock();

	if (!m_pcellHead)
	{
		OutputDebugThreadIdString("CLList::DisplayList - List is empty\n");
		Unlock();
		return;
	}

	OutputDebugThreadIdString("CLList::DisplayList - List contains: ");
	pcellCur = m_pcellHead;

	while (pcellCur)
	{
		sprintf(g_szDebugStr, " %ld", pcellCur->pvContent);
		OutputDebugString(g_szDebugStr);
		pcellCur = pcellCur->pcellNext;
	}

	OutputDebugString("\n");

	Unlock();

	return;
}
#endif // DEBUG

