//--------------------------------------------------------------------------------------------
//
//	Copyright (c) Microsoft Corporation, 1996
//
//	Description:
//
//		A Generic HASH Table.
//		Uses OPEN HASHING.
//
//	Authors:
//
//		Umesh Madan - Modified by Regis Brid
//
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
//
// INCLUDES.
//
//--------------------------------------------------------------------------------------------
#include "StdAfx.H"
#include "CUtil.H"
#include "CHash.H"

// for ASSERT and FAIL
SZTHISFILE
//--------------------------------------------------------------------------------------------
//
// MACROS.
//
//--------------------------------------------------------------------------------------------

#define AssertValid() ASSERT(m_dwBucketCount > 0L && m_pHashTable, "AssertValid failed")

//--------------------------------------------------------------------------------------------
//
// CLASSES.
//
//--------------------------------------------------------------------------------------------
CMemoryBlockChain::CMemoryBlockChain(void)
{
	m_pmbNext = NULL;
}


CMemoryBlockChain::~CMemoryBlockChain(void)
{
	if (m_pmbNext)
		FreeBlockAndChain();
}


PMEMBLOCK CMemoryBlockChain::PmbCreate(DWORD dwcb)
{
	ASSERT(dwcb, "dwcb is NULL in CMemoryBlockChain::PmbCreate");

	//
	// Allocate a new block. Allocate the byte count requested by the user plus enough
	// space for the Memory Block header.
	//
	PMEMBLOCK pmb = (PMEMBLOCK) new BYTE[sizeof(MEMBLOCK) + dwcb];
	if (!pmb)
	{
		ASSERT(FALSE, "Out of Memory in CMemoryBlockChain::PmbCreate");
		return NULL;
	}

	//
	// Chain the block in
	//
	pmb->m_pmbNext = m_pmbNext;
	m_pmbNext = pmb;

	return pmb;
}

//
// Free this block and all its linked blocks
//
void CMemoryBlockChain::FreeBlockAndChain(void)     
{
	PMEMBLOCK	pmb = m_pmbNext;
	PMEMBLOCK	pmbNext;

	while (pmb)
	{
		//
		// Save the link to the next block in the chain
		//
		pmbNext = pmb->m_pmbNext;
		//
		// delete this block
		//
		delete [] (BYTE*) pmb;
		//
		// Next in the chain
		//
		pmb = pmbNext;
	} 

	m_pmbNext = NULL;
}


CMemoryChain::CMemoryChain(DWORD dwBlockSize)
{
	//
	// The hash table contains linked lists of HASHCELLs. To enhance performance, memory
	// is allocated in BLOCKS. nBlockSize determines how many HASHCELLs we should allocate
	// at a time.
	ASSERT(dwBlockSize > 0L, "Unexpeced dwBlockSize in CMemoryChain::CMemoryChain");

	m_dwBlockSize	= dwBlockSize;	
	m_dwCellCount	= 0L;
	m_pFreeList		= NULL;
}


CMemoryChain::~CMemoryChain(void)
{
	CleanUp();
}


void CMemoryChain::CleanUp(void)
{
	m_dwCellCount	= 0L;
	m_pFreeList		= NULL;
	m_memBlocks.FreeBlockAndChain();
}


PHASHCELL CMemoryChain::PNewCell(void)
{
	PHASHCELL pCell;
	//
	// Do we have any free cells?
	//
	if (!m_pFreeList)
	{
		PMEMBLOCK pmb;
		PHASHCELL pCellLast;
		//
		// add another block. This will ZERO out everything
		//
		pmb = m_memBlocks.PmbCreate(m_dwBlockSize * sizeof(HASHCELL));
		if (!pmb)
			return NULL;

		//
		// Add new cells to free list.. chain them in.
		//
		pCell		= (PHASHCELL) pmb->PvData();
		pCellLast	= pCell + (m_dwBlockSize - 1);
		while (pCell <= pCellLast)
		{
			pCell->pcellNext = m_pFreeList;
			m_pFreeList = pCell;
			++pCell;
		}
	}

	ASSERT(m_pFreeList, "m_pFreeList is NULL in CMemoryChain::PNewCell"); 
	//
	// Take a cell from the front of the free list and use it
	//
	pCell				= m_pFreeList;
	m_pFreeList			= m_pFreeList->pcellNext;
	pCell->pcellNext	= NULL;
	pCell->pvContent	= NULL;
	pCell->dwKey		= 0L;

	++m_dwCellCount;

	ASSERT(m_dwCellCount > 0L, "m_dwCellCount == 0L in CMemoryChain::PNewCell");  // make sure we don't overflow
	
	return pCell;
}


void CMemoryChain::FreeCell(PHASHCELL pCell)
{
	ASSERT(pCell, "pCell is NULL in CMemoryChain::FreeCell");
	//
	// Return the cell to the FREE pool
	//
	pCell->pcellNext	= m_pFreeList;
	m_pFreeList			= pCell;
	--m_dwCellCount;
}

//
// Stuff core to any hash table
//
CHashBase::CHashBase(DWORD dwBlockSize)
		:CMemoryChain(dwBlockSize)
{
	//
	// The hash table contains linked lists of HASHCELL. To enhance performance, memory
	// is allocated in BLOCKS. dwBlockSize determines how many HASHCELL we should allocate
	// at a time.
	//
	m_pHashTable		= NULL;
	m_dwBucketCount		= CDEFTABLE;  	
}


CHashBase::~CHashBase(void)
{
}


BOOL CHashBase::bDeleteHashTable(void)
{
	//
	// free existing hash table
	//
	if (!m_pHashTable)
		return TRUE;

	//
	// First go through the entire table and free any stored data
	//
	BOOL bRet = bFreeAllData();

	ASSERT(bRet, "bFreeAllData failed in CHashBase::bDeleteHashTable");

	delete [] m_pHashTable;
	m_pHashTable = NULL;

	return bRet;
}


BOOL CHashBase::bFreeAllData(void)
{
	if (!m_pHashTable)
		return TRUE;

	PHASHCELL	pCell;
	DWORD		dwBucket;
	BOOL		bRet = TRUE;

	for (dwBucket = 0L; dwBucket < m_dwBucketCount; ++dwBucket)
	{
		pCell = m_pHashTable[dwBucket];
		while (pCell)
		{
			//
			// Look at each cell and free up its stored data
			//
			bRet &= bDeleteUserData(pCell->pvContent);

			ASSERT(bRet, "bDeleteUserData failed in CHashBase::bFreeAllData");

			pCell = pCell->pcellNext;
		}
	}
	
	return bRet;
}


BOOL CHashBase::bInitHashTable(DWORD dwBuckets)
{
	ASSERT(dwBuckets, "dwBuckets is 0L in CHashBase::bInitHashTable");
	//
	// Delete any existing tables
	//
	bDeleteHashTable();
	//
	// Create a new table - of pointers to cell lists...
	//
	m_dwBucketCount = dwBuckets;
	m_pHashTable = new PHASHCELL[m_dwBucketCount];
	if (!m_pHashTable)
		return FALSE;

	::ZeroMemory(m_pHashTable, sizeof(PHASHCELL) * m_dwBucketCount);

	return TRUE;
}


BOOL CHashBase::bRemoveAll(void)
{
	BOOL bRet = bDeleteHashTable();
	CleanUp();

	return bRet;
}


//
// NOTES:
// The final hash index in our hash table is the modulo of the hash value divided by the table
// size. The widest dispersal of modulo results when the hash value and table size are 
// relatively prime - i.e. contain no common factors. The best way to assure this is to make
// the table size PRIME.
// Some prime Numbers: 17,31,37,107
//
CHashTableNumber::CHashTableNumber(DWORD dwBlockSize)
		:CHashBase(dwBlockSize)
{
}


CHashTableNumber::~CHashTableNumber(void)
{
}


//
// Given the key, pass it through the Hash function and locate the matching object in the
// hash table
//
PHASHCELL CHashTableNumber::PGetCellAt(DWORD dwKey, DWORD *pdwHash)
{
	if (!m_pHashTable)
		return NULL;

	AssertValid();

	DWORD dwHash = DwHashKey(dwKey) % m_dwBucketCount;
	if (pdwHash)
		*pdwHash = dwHash;

	//
	// Find it, and return if it exists
	//
	for (PHASHCELL pCell = m_pHashTable[dwHash]; pCell; pCell = pCell->pcellNext)
	{
		if (dwKey == pCell->dwKey)
			return pCell;
	}

	return NULL;
}


BOOL CHashTableNumber::bGetDataFromKey(DWORD dwKey, PVOID *ppvContent)
{
	ASSERT(ppvContent, "ppvContent is NULL in CHashTableNumber::bGetDataFromKey");
	AssertValid();

	PHASHCELL pCell = PGetCellAt(dwKey);
	if (!pCell)
		return FALSE;  // not in hash table

	*ppvContent = pCell->pvContent;

	return TRUE;
}


BOOL CHashTableNumber::bAdd(DWORD dwKey, PVOID pvContent)
{
	DWORD		dwHash;
	PHASHCELL	pCell;
	
	pCell = PGetCellAt(dwKey, &dwHash);
	if (!pCell)
	{
		if (!m_pHashTable)
			if (!bInitHashTable(m_dwBucketCount))
				return FALSE;

		//
		// it doesn't exist, add a new cell
		//
		pCell = PNewCell();
		if (!pCell)
			return FALSE;

		pCell->dwKey = dwKey;
		pCell->pvContent = pvContent;
		//
		// put into hash table
		//
		pCell->pcellNext = m_pHashTable[dwHash];
		m_pHashTable[dwHash] = pCell;
	}
	else
	{
		ASSERT(FALSE, "Unexpected hash key conflict in CHashTableNumber::bAdd");	// wow. Already exists! Bad
		return FALSE;
	}
	
	return TRUE;
}


BOOL CHashTableNumber::bRemove(DWORD dwKey, PVOID *ppvContent)
{
	AssertValid();

	if (!m_pHashTable)
		return FALSE;  // nothing in the table

	PHASHCELL *ppCell, pCell;

	ppCell = &m_pHashTable[DwHashKey(dwKey) % m_dwBucketCount];

	for (pCell = *ppCell; pCell; pCell = pCell->pcellNext)
	{
		if (dwKey == pCell->dwKey)
		{
			//
			// remove it
			//
			*ppCell = pCell->pcellNext;  // remove from list
			if (ppvContent)
				*ppvContent = pCell->pvContent;

			FreeCell(pCell);

			return TRUE;
		}
		ppCell = &(pCell->pcellNext);
	}

	return FALSE;  // not found
}

//
// NOTES:
// The final hash index in our hash table is the modulo of the hash value divided by the table
// size. The widest dispersal of modulo results when the hash value and table size are 
// relatively prime - i.e. contain no common factors. The best way to assure this is to make
// the table size PRIME.
// Some prime Numbers: 17,31,37,107
//
CHashTableString::CHashTableString(DWORD dwBlockSize)
		:CHashBase(dwBlockSize)
{
	for (INT i = 0; i < 256; ++i)
		m_szUpper[i] = (CHAR) i;
	
	::CharUpperBuff(&m_szUpper[0], 256);	// convert to upper case
}

CHashTableString::~CHashTableString(void)
{
}


DWORD CHashTableString::DwHashKey(TCHAR *szKey) const
{	
	ASSERT(szKey, "szKey is NULL in CHashTableString::DwHashKey");
	//
	// This works pretty well for most cases. .
	//
	DWORD	dwHash = 0L;
	CHAR	ch;
	
	for (INT c = 0; c < 4;++c)
	{
		ch = *szKey++;
		if (g_chEOS == ch)
			break;

		dwHash += ChUpper(ch);
	}	
	
	return dwHash;	
}


//
// Given the key, pass it through the Hash function and locate the matching object in the
// hash table
//
PHASHCELL CHashTableString::PGetCellAt(CHAR *szKey, CHAR *szParent, DWORD *pdwHash)
{
	ASSERT(szKey, "szKey is NULL in CHashTableString::PGetCellAt");

	if (!m_pHashTable)
		return NULL;

	AssertValid();

	DWORD dwHash = DwHashKey(szKey) % m_dwBucketCount;
	if (pdwHash)
		*pdwHash = dwHash;

	//
	// Find it, and return if it exists
	//
	for (PHASHCELL pCell = m_pHashTable[dwHash]; pCell; pCell = pCell->pcellNext)
	{
		ASSERT(pCell->dwKey, "Unexpected NULL key in CHashTableString::PGetCellAt");
		if (0 == ::lstrcmpi(szKey, (CHAR*) pCell->dwKey))
		{
			//
			// Key matches. Verify that so does the parent (if any)
			//
			if (!szParent || bCompareParent(pCell->pvContent, szParent))
				//
				// This is it
				//
				return pCell;
		}
	}

	return NULL;
}


BOOL CHashTableString::bGetDataFromKey(CHAR *szKey, CHAR *szParent, PVOID *ppvContent)
{
	ASSERT(szKey, "szKey is NULL in CHashTableString::bGetDataFromKey");

	if (!m_pHashTable)
		return FALSE;

	ASSERT(ppvContent, "ppvContent is NULL in CHashTableString::bGetDataFromKey");
	AssertValid();

	PHASHCELL pCell = PGetCellAt(szKey, szParent);
	if (!pCell)
		return FALSE;  // not in hash table

	*ppvContent = pCell->pvContent;

	return TRUE;
}


BOOL CHashTableString::bAdd(CHAR *szKey, CHAR *szParent, PVOID pvContent)
{
	DWORD dwHash;
	
	ASSERT(szKey, "szKey is NULL in CHashTableString::bAdd");

	PHASHCELL pCell = PGetCellAt(szKey, szParent, &dwHash);
	if (!pCell)
	{
		if (!m_pHashTable)
		{
			if (!bInitHashTable(m_dwBucketCount))
				return FALSE;
			dwHash = DwHashKey(szKey) % m_dwBucketCount;
		}

		//
		// it doesn't exist, add a new cell
		//
		pCell = PNewCell();
		if (!pCell)
			return FALSE;

		pCell->dwKey = (DWORD) szKey;
		pCell->pvContent = pvContent;
		//
		// put into hash table
		//
		pCell->pcellNext = m_pHashTable[dwHash];
		m_pHashTable[dwHash] = pCell;

		#ifdef DEBUG
			sprintf(g_szDebugStr, "CHashTableString::bAdd - Added key '%s' in bucket '%ld'\n", (LPCTSTR) pCell->dwKey, dwHash);
			OutputDebugThreadIdString(g_szDebugStr);
		#endif // DEBUG
	}
	else
	{
		ASSERT(FALSE, "Unexpected hash key conflict in CHashTableString::bAdd");	// Humm, this key already exists
		return FALSE;
	}
	
	return TRUE;
}


BOOL CHashTableString::bRemove(CHAR *szKey, CHAR *szParent, PVOID *ppvContent)
{
	AssertValid();
	ASSERT(szKey, "szKey is NULL in CHashTableString::bRemove");

	if (!m_pHashTable)
		return FALSE;  // nothing in the table

	PHASHCELL *ppCell, pCell;

	ppCell = &m_pHashTable[DwHashKey(szKey) % m_dwBucketCount];

	for (pCell = *ppCell; pCell; pCell = pCell->pcellNext)
	{
		ASSERT(pCell->dwKey, "Unexpected NULL key in CHashTableString::bRemove");
		#ifdef DEBUG
			sprintf(g_szDebugStr, "bRemove - Comparing '%s'\n", (CHAR*) pCell->dwKey);
			OutputDebugThreadIdString(g_szDebugStr);
		#endif // DEBUG
		if (0 == ::lstrcmpi((CHAR*) pCell->dwKey, szKey))
		{
			if (!szParent || bCompareParent(pCell->pvContent, szParent))
			{
				// remove it
				*ppCell = pCell->pcellNext;  // remove from list
				if (ppvContent)
					*ppvContent = pCell->pvContent;

				FreeCell(pCell);

				return TRUE;
			}
		}
		ppCell = &(pCell->pcellNext);
	}

	#ifdef DEBUG
		sprintf(g_szDebugStr, "CHashTableString::bRemove - Could not find key '%s' in bucket '%ld'\n", (LPCTSTR) szKey, DwHashKey(szKey) % m_dwBucketCount);
		OutputDebugThreadIdString(g_szDebugStr);
	#endif // DEBUG

	return FALSE;  // not found
}

