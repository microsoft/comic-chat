//=--------------------------------------------------------------------------=
// CCHash.Cpp:		Implementation of custom hash table classes.
//=--------------------------------------------------------------------------=
// Copyright  1997  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

#include "StdAfx.H"
#include "CUtil.H"
#include "CCHash.H"
#include "PrCnst.H"

// for ASSERT and FAIL
SZTHISFILE


//////////////////////////////////////////////////////////////////////////////
// Member Hash Table Virtual Functions
//////////////////////////////////////////////////////////////////////////////

//=--------------------------------------------------------------------------=
// CHashMember::bDeleteUserData
//=--------------------------------------------------------------------------=
BOOL CHashMember::bDeleteUserData(PVOID pvContent)
{
	PPRMEMBER	pPrMember = (PPRMEMBER) pvContent;

	ASSERT(pPrMember, "pPrMember is NULL in CHashMember::bDeleteUserData");

	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "Freeing member '%ld' in CHashMember::bDeleteUserData\n", (int) pvContent);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif

	if (pPrMember->szNickname)
		delete [] pPrMember->szNickname;
	if (pPrMember->szUserName)
		delete [] pPrMember->szUserName;
	if (pPrMember->szIPAddress)
		delete [] pPrMember->szIPAddress;

	delete pPrMember;

	return TRUE;
}


#ifdef DEBUG
void CHashMember::DumpMemberList(void)
{
	if (!m_pHashTable)
	{
		OutputDebugThreadIdString("CHashMember::DumpMemberList - Member List is Empty\n");
		return;  // nothing in the table
	}

	PHASHCELL	pCell;
	PPRMEMBER	pPrMember;

	g_cs.Lock();

	for (DWORD dwBucket = 0L; dwBucket < m_dwBucketCount; dwBucket++)
	{
		if (m_pHashTable[dwBucket])
		{
			sprintf(g_szDebugStr, "Dumping bucket '%ld' ...\n", dwBucket);
			OutputDebugThreadIdString(g_szDebugStr);
		}
		for (pCell = m_pHashTable[dwBucket]; pCell; pCell = pCell->pcellNext)
		{
			ASSERT(pCell->dwKey, "Unexpected NULL key in CHashMember::DumpMemberList");
			pPrMember = (PPRMEMBER) pCell->pvContent;
			ASSERT(pPrMember, "pPrMember is NULL in CHashMember::DumpMemberList");
			ASSERT(0 == lstrcmpi((LPCTSTR) pCell->dwKey, pPrMember->szNickname), "Unexpected key in CHashMember::DumpMemberList");

			sprintf(g_szDebugStr, "Key:'%s' Nick:'%s' UserN:'%s' IP@:'%s'\n", 
					(LPCTSTR) pCell->dwKey,
					pPrMember->szNickname,
					pPrMember->szUserName ? pPrMember->szUserName : "",
					pPrMember->szIPAddress ? pPrMember->szIPAddress : "");
			OutputDebugThreadIdString(g_szDebugStr);
		}
	}

	g_cs.Unlock();
}
#endif // DEBUG
