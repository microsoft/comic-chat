//=--------------------------------------------------------------------------=
// LList.Cpp:		Implementation of the CLList class.
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=

#include "CCommon.H"
#include "LList.H"
#include "CDebug.H"

// for ASSERT and FAIL
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CLList::CLList - Constructor
CLList::CLList()
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
CLList::~CLList()
{
	OutputDebugThreadIdString("CLList::~CLList - Enter\n");

	FDeleteList();

	if (m_pcs)
	{
		DeleteCriticalSection(m_pcs);
		delete [] m_pcs;
		m_pcs = NULL;
	}

	OutputDebugThreadIdString("CLList::~CLList - Leave\n");
}


void CLList::Lock()
{ 
	if (m_pcs) EnterCriticalSectionDebug(m_pcs);
}


void CLList::Unlock()
{
	if (m_pcs) LeaveCriticalSectionDebug(m_pcs);
}


BOOL CLList::FProtect()
{
	ASSERT(!m_pcs, "m_pcs is NOT NULL in CLList::FProtect");

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


void CLList::Unprotect()
{
	ASSERT(m_pcs, "m_pcs is NULL in CLList::FUnprotect");

	if (m_pcs)
	{
		DeleteCriticalSection(m_pcs);
		delete [] m_pcs;
		m_pcs = NULL;
	}
	m_bProtected = FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::FAddFrontCell - Destructor
//
//	returns TRUE if successful 
//          FALSE if OOM or pvNewFrontCell is NULL or protection error
BOOL CLList::FAddFrontCell(LPVOID pvNewFrontCell)
{
	CELL *pcellNew;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::FAddFrontCell");
		return FALSE;
	}

	if (!pvNewFrontCell)
	{
		ASSERT(FALSE, "!pvNewFrontCell in CLList::FAddFrontCell");
		return FALSE;
	}

	//Lock();
	//OutputDebugThreadIdString("CLList::FAddFrontCell\n");
	//DisplayList();

	pcellNew = new CELL;
	if (!pcellNew)
	{
		ASSERT(FALSE, "!pcellNew in CLList::FAddFrontCell");
		return FALSE;
	}

	pcellNew->pvContent = pvNewFrontCell;

	Lock();

	pcellNew->pcellNext = m_pcellHead;
	m_pcellHead = pcellNew;

	m_dwCellCount++;

	Unlock();

	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "CLList::FAddFrontCell - Successfully added cell content: %ld\n", pvNewFrontCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif // DEBUG

	//DisplayList();
	//Unlock();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::FRemoveCell - removes a cell fron the linked list
BOOL CLList::FRemoveCell(LPVOID pvCell)
{
	CELL *pcellCur, *pcellPrev;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::FRemoveCell");
		return FALSE;
	}

	if (!pvCell)
	{
		ASSERT(FALSE, "!pvCell in CLList::FRemoveCell");
		return FALSE;
	}

	//Lock();
	//OutputDebugThreadIdString("CLList::FRemoveCell\n");
	//DisplayList();

	pcellPrev = NULL;

	Lock();

	pcellCur = m_pcellHead;

	while (pcellCur && !FIsEqual(pcellCur->pvContent, pvCell))
	{
		pcellPrev = pcellCur;
		pcellCur = pcellCur->pcellNext;
	}

	if (!pcellCur || !FDeleteCell(pcellCur->pvContent))
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
	//	sprintf(g_szDebugStr, "CLList::FRemoveCell - Successfully removed cell content: %ld\n", pvCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif // DEBUG

	//DisplayList();
	//Unlock();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::FDeleteList - delete the whole list
BOOL CLList::FDeleteList()
{
	CELL *pcellCur, *pcellPrev;

	OutputDebugThreadIdString("CLList::FDeleteList - Enter\n");

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::FDeleteList");
		return FALSE;
	}

	if (!m_pcellHead)
		return TRUE;
	
	Lock();

	pcellCur = m_pcellHead;
	while (pcellCur)
	{
		if (!FDeleteCell(pcellCur->pvContent))
		{
			Unlock();
			ASSERT(FALSE, "Could not delete cell content in CLList::FDeleteList");
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
// CLList::FDoAction - 
BOOL CLList::FDoActionOnList(short nActionID)
{
	BOOL fActionItem = TRUE;
	CELL *pcellCur;
	
	OutputDebugThreadIdString("CLList::FDoActionOnList\n");

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::FDoActionOnList");
		return FALSE;
	}

	Lock();

	pcellCur = m_pcellHead;
	while (pcellCur && fActionItem)
	{
		fActionItem = FDoActionOnCell(nActionID, pcellCur->pvContent);
		pcellCur = pcellCur->pcellNext;
	}

	Unlock();

	return fActionItem;
}


/////////////////////////////////////////////////////////////////////////////
// CLList::FIsEqual - virtual function to determine if two cells are identical
BOOL CLList::FIsEqual(LPVOID pvCell1, LPVOID pvCell2)
{
	return (pvCell1 == pvCell2);
}


/////////////////////////////////////////////////////////////////////////////
// CLList::FVerifySearchCriteria - virtual function to determine if a cell 
// verifies a search criteria
BOOL CLList::FVerifySearchCriteria(short nSearchID, LPVOID pvData, LPVOID pvCell)
{
	ASSERT(FALSE, "Derived class needs to implement this virtual function.");
	return FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// CLList::FDeleteCell - virtual function to destroy a single cell
BOOL CLList::FDeleteCell(LPVOID pvCell)
{
	if (pvCell)
	{
		delete pvCell;
		pvCell = NULL;
	}
	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "CLList::FDeleteCell - Successfully deleted cell: %ld\n", pvCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif // DEBUG
	return TRUE;
}


BOOL CLList::FFindCellFromData(short nSearchID, LPVOID pvData, LPVOID *ppvCell)
{
	CELL *pcellCur;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::FFindCellFromData");
		return FALSE;
	}

	if (!ppvCell)
	{
		ASSERT(FALSE, "!ppvCell in CLList::FFindCellFromData");
		return FALSE;
	}

	Lock();

	pcellCur = m_pcellHead;

	while (pcellCur && !FVerifySearchCriteria(nSearchID, pvData, pcellCur->pvContent))
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


BOOL CLList::FFindCell(LPVOID pvCell1, LPVOID *ppvCell2)
{
	CELL *pcellCur;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::FFindCell");
		return FALSE;
	}

	if (!pvCell1 || !ppvCell2)
	{
		ASSERT(FALSE, "!pvCell1 || !ppvCell2 in CLList::FFindCell");
		return FALSE;
	}

	Lock();

	pcellCur = m_pcellHead;

	while (pcellCur && !FIsEqual(pcellCur->pvContent, pvCell1))
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


BOOL CLList::FGetCell(short nPosition, LPVOID *ppvCell)
{
	CELL *pcellCur;

	if (m_bProtected && !m_pcs)
	{
		ASSERT(FALSE, "m_bProtected && !m_pcs in CLList::FGetCell");
		return FALSE;
	}

	if (!ppvCell)
	{
		ASSERT(FALSE, "!ppvCell in CLList::FGetCell");
		return FALSE;
	}

	*ppvCell = NULL;

	Lock();

	pcellCur = m_pcellHead;

	for (short nCnt = 1; nCnt < nPosition; nCnt++)
		if (pcellCur)
			pcellCur = pcellCur->pcellNext;
		else
			break;

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
void CLList::DisplayList()
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