//=--------------------------------------------------------------------------=
// CLList.Cpp:		Implementation of custom linked list classes.
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

#include "StdAfx.H"
#include "CLList.H"
#include "CCSock.H"
#include "ChObj.H"

// for ASSERT and FAIL
SZTHISFILE


//////////////////////////////////////////////////////////////////////////////
// Mask List Virtual Functions
//////////////////////////////////////////////////////////////////////////////

//=--------------------------------------------------------------------------=
// CMaskLList::bIsEqual
//=--------------------------------------------------------------------------=
// checks if two cells are equal
//
// Parameters:
//    PVOID			- [in] first cell content
//    PVOID			- [in] second cell content
//
// Output:
//    BOOL			TRUE if cells are identical
//
// Notes:
//
BOOL CMaskLList::bIsEqual(PVOID pvCell1, PVOID pvCell2)
{
	PPRUSERMATCH pPrUserMatch1 = (PPRUSERMATCH) pvCell1;
	PPRUSERMATCH pPrUserMatch2 = (PPRUSERMATCH) pvCell2;

	if (pPrUserMatch1 == pPrUserMatch2)
		return TRUE;

	if (!pPrUserMatch1 || !pPrUserMatch2)
		return FALSE;

	ASSERT(pPrUserMatch1->szTheMask, "pPrUserMatch1->szTheMask is NULL in CMaskLList::bIsEqual");
	ASSERT(pPrUserMatch2->szTheMask, "pPrUserMatch2->szTheMask is NULL in CMaskLList::bIsEqual");

	return 0 == lstrcmp(pPrUserMatch1->szTheMask, pPrUserMatch2->szTheMask);
}


//=--------------------------------------------------------------------------=
// CMaskLList::bDeleteCell
//=--------------------------------------------------------------------------=
// frees a cell
//
// Parameters:
//    PVOID			- [in] cell content to free
//
// Output:
//    BOOL			TRUE if successfully released the cell content
//
// Notes:
//
BOOL CMaskLList::bDeleteCell(PVOID pvCell)
{
	PPRUSERMATCH	pPrUserMatch = (PPRUSERMATCH) pvCell;

	ASSERT(pPrUserMatch, "pPrUserMatch is NULL in CMaskLList::bDeleteCell");

	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "Freeing pPrUserMatch '%X' in CMaskLList::bDeleteCell\n", (INT) pvCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif

	ASSERT(pPrUserMatch->szTheMask, "pPrUserMatch->szTheMask is NULL in CMaskLList::bDeleteCell");

	delete [] pPrUserMatch->szTheMask;
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// Channel List Virtual Functions
//////////////////////////////////////////////////////////////////////////////

//=--------------------------------------------------------------------------=
// CChanLList::bDoActionOnCell
//=--------------------------------------------------------------------------=
// executes an action on a cell of the list
//
// Parameters:
//    SHORT			- [in] action ID
//    PVOID			- [in] cell content
//
// Output:
//    BOOL			TRUE if action was successful
//
// Notes:
//
BOOL CChanLList::bDoActionOnCell(SHORT nActionID, PVOID pvCell)
{
	CChatChannel* pChannel = (CChatChannel*) pvCell;

	ASSERT(pChannel, "pChannel is NULL in CChanLList::bDoActionOnCell");

	#ifdef DEBUG
		sprintf(g_szDebugStr, "Action %d on '%ld' in CChanLList::bDoActionOnCell\n", nActionID, pChannel);
		OutputDebugThreadIdString(g_szDebugStr);
	#endif

	switch (nActionID)
	{
		case g_nActionLeave:
			switch (pChannel->ChsGetChannelState())
			{
			case chsClosed:
				return TRUE;

			case chsOpening:
				// REGISB: Humm, i don't really know what to do here!
				return pChannel->bLeaveChannel();	// this should be fine since we already sent the JOIN command

			case chsOpen:
				return pChannel->bLeaveChannel();

			default:
				ASSERT(FALSE, "Unexpected value for pchan->m_chs in CChanLList::bDoActionOnCell");
				return FALSE;
			}
			break;

		default:
			ASSERT(FALSE, "Unexpected value for nActionID in CChanLList::bDoActionOnCell");
			return FALSE;
	}

	return FALSE;
}


//=--------------------------------------------------------------------------=
// CChanLList::bIsEqual
//=--------------------------------------------------------------------------=
// checks if two cells are equal
//
// Parameters:
//    PVOID			- [in] first cell content
//    PVOID			- [in] second cell content
//
// Output:
//    BOOL			TRUE if cells are identical
//
// Notes:
//
BOOL CChanLList::bIsEqual(PVOID pvCell1, PVOID pvCell2)
{
	return pvCell1 == pvCell2;
}


//=--------------------------------------------------------------------------=
// CChanLList::bDeleteCell
//=--------------------------------------------------------------------------=
// frees a cell
//
// Parameters:
//    PVOID			- [in] cell content to free
//
// Output:
//    BOOL			TRUE if successfully released the cell content
//
// Notes:
//
BOOL CChanLList::bDeleteCell(PVOID pvCell)
{
	CChatChannel* pChannel = (CChatChannel*) pvCell;

	ASSERT(pChannel, "pChannel is NULL in CChanLList::bDeleteCell");

	#ifdef DEBUG
		sprintf(g_szDebugStr, "Deleting '%ld' in CChanLList::bDeleteCell\n", (INT) pChannel);
		OutputDebugThreadIdString(g_szDebugStr);
	#endif

	if (pChannel)
	{
		CChannelObject*	pcchan = (CChannelObject*) pChannel->GetAutObject();
		if (((IChannelObject*) pcchan)->Release() > 0)
			pChannel->SetChatSocket(NULL);
	}

	return TRUE;
}

//=--------------------------------------------------------------------------=
// CChanLList::bVerifySearchCriteria
//=--------------------------------------------------------------------------=
// verifies if a member has a given alias
//
// Parameters:
//    SHORT			- [in] search ID
//    PVOID			- [in] search criteria (alias)
//    PVOID			- [in] pointer to member
//
// Output:
//    BOOL			TRUE if search condition is verified
//
// Notes:
//
BOOL CChanLList::bVerifySearchCriteria(SHORT nSearchID, PVOID pvData, PVOID pvCell)
{
	ASSERT(pvCell, "pvCell is NULL in CChanLList::bVerifySearchCriteria");

	CChatChannel* pChannel = (CChatChannel*) pvCell;

	switch (nSearchID)
	{
		case g_nSearchNone:
		{
			return TRUE;
		}
		case g_nSearchByName:
		{
			ASSERT(pvData, "pvData is NULL in CChanLList::bVerifySearchCriteria");
			if (!pChannel->SzGetChannelName())
				return FALSE;
			return 0 == lstrcmpi(pChannel->SzGetChannelName(), (LPCTSTR) pvData);
		}
		case g_nSearchByState:
		{
			ASSERT(pvData, "pvData is NULL in CChanLList::bVerifySearchCriteria");
			enumChannelState*	pchs = (enumChannelState*) pvData;

			return pChannel->ChsGetChannelState() == *pchs;
		}
		case g_nSearchByChatChannel:
		{
			ASSERT(pvData, "pvData is NULL in CChanLList::bVerifySearchCriteria");
			return bIsEqual(pvData, pvCell);
		}
		case g_nSearchByKey:
		{
			LPCTSTR szItemKey = ((CChannelObject*) pChannel->GetAutObject())->SzGetItemKey();

			return szItemKey && (0 == lstrcmp(szItemKey, (LPCTSTR) pvData));
		}
		case g_nSearchByCloneName:
		{
			ASSERT(pvData, "pvData is NULL in CChanLList::bVerifySearchCriteria");
			if (!pChannel->SzGetChannelName())
				return FALSE;

			LPTSTR  szRealChannelName = (LPTSTR) pvData;					// Comic_Chat2
			LPCTSTR szInitialChannelName = pChannel->SzGetChannelName();	// comic_chat
			TCHAR	ch;
			INT		cLenI = lstrlen(szInitialChannelName);					// 10

			if (lstrlen(szRealChannelName) < cLenI)
				return FALSE;

			ch = szRealChannelName[cLenI];
			szRealChannelName[cLenI] = g_chEOS;
			if (0 != lstrcmpi(szRealChannelName, szInitialChannelName))
			{
				szRealChannelName[cLenI] = ch;
				return FALSE;
			}
			szRealChannelName[cLenI] = ch;

			while (szRealChannelName[cLenI] != g_chEOS)
			{
				if (!bIsDigit(szRealChannelName[cLenI]))
					return FALSE;
				cLenI++;
			}
			return TRUE;
		}
		default:
		{
			ASSERT(FALSE, "Unexpected nSearchID value in CChanLList::bVerifySearchCriteria");
			return FALSE;
		}
	}

	return FALSE;
}


//////////////////////////////////////////////////////////////////////////////
// Query List Virtual Functions
//////////////////////////////////////////////////////////////////////////////

//=--------------------------------------------------------------------------=
// CQueryLList::bIsEqual
//=--------------------------------------------------------------------------=
// checks if two cells are equal
//
// Parameters:
//    PVOID			- [in] first cell content
//    PVOID			- [in] second cell content
//
// Output:
//    BOOL			TRUE if cells are identical
//
// Notes:
//
BOOL CQueryLList::bIsEqual(PVOID pvCell1, PVOID pvCell2)
{
	PPRQUERY pPrQuery1 = (PPRQUERY) pvCell1;
	PPRQUERY pPrQuery2 = (PPRQUERY) pvCell2;

	if (pPrQuery1 == pPrQuery2)
		return TRUE;

	if (!pPrQuery1 || !pPrQuery2)
		return FALSE;

	if (pPrQuery1->at != pPrQuery2->at)
		return FALSE;

	if (pPrQuery1->bSyncAccess != pPrQuery2->bSyncAccess)
		return FALSE;
	
	return pPrQuery1->pvData == pPrQuery2->pvData;
}


//=--------------------------------------------------------------------------=
// CQueryLList::bVerifySearchCriteria
//=--------------------------------------------------------------------------=
// 
//
// Parameters:
//    SHORT			- [in] search ID
//    PVOID			- [in] search criteria
//    PVOID			- [in] pointer to member
//
// Output:
//    BOOL			TRUE if search condition is verified
//
// Notes:
//
BOOL CQueryLList::bVerifySearchCriteria(SHORT nSearchID, PVOID pvData, PVOID pvCell)
{
	ASSERT(pvCell, "pvCell is NULL in CQueryLList::bVerifySearchCriteria");

	PPRQUERY pPrQuery = (PPRQUERY) pvCell;

	switch (nSearchID)
	{
		case g_nSearchNone:
		{
			return TRUE;
		}
		case g_nSearchByAssociatedType:
		{
			ASSERT(pvData, "pvData is NULL in CQueryLList::bVerifySearchCriteria");

			enumAssociatedType*	pat = (enumAssociatedType*) pvData;

			return pPrQuery->at == *pat;
		}
		case g_nSearchByQueryType:
		{
			ASSERT(pvData, "pvData is NULL in CQueryLList::bVerifySearchCriteria");

			enumQueryType*	pqt = (enumQueryType*) pvData;

			return pPrQuery->pvData == (PVOID) *pqt;
		}
		case g_nSearchByName:
		{
			ASSERT(pvData, "pvData is NULL in CQueryLList::bVerifySearchCriteria");
			ASSERT(pPrQuery->szChannelName, "pPrQuery->szChannelName is NULL in CQueryLList::bVerifySearchCriteria");
			return 0 == lstrcmpi(pPrQuery->szChannelName, (LPCTSTR) pvData);
		}
		case g_nSearchByNickname:
		{
			ASSERT(pvData, "pvData is NULL in CQueryLList::bVerifySearchCriteria");
			switch (pPrQuery->at)
			{
				case atUser:
				{
					PPRUSER	pPrUser = (PPRUSER) pPrQuery->pvData;

					ASSERT(pvData, "pvData is NULL in CQueryLList::bVerifySearchCriteria");
					ASSERT(pPrUser, "pPrUser is NULL in CQueryLList::bVerifySearchCriteria");
					ASSERT(pPrUser->szNickname, "pPrUser->szNickname is NULL in CQueryLList::bVerifySearchCriteria");
					return 0 == lstrcmpi(pPrUser->szNickname, (LPCTSTR) pvData);
				}
				case atMember:
				{
					PPRMEMBER pPrMember = (PPRMEMBER) pPrQuery->pvData;

					ASSERT(pvData, "pvData is NULL in CQueryLList::bVerifySearchCriteria");
					ASSERT(pPrMember, "pPrMember is NULL in CQueryLList::bVerifySearchCriteria");
					ASSERT(pPrMember->szNickname, "pPrMember->szNickname is NULL in CQueryLList::bVerifySearchCriteria");
					return 0 == lstrcmpi(pPrMember->szNickname, (LPCTSTR) pvData);
				}
				default:
				{
					ASSERT(FALSE, "Unexpected associated type in CQueryLList::bVerifySearchCriteria");
					return FALSE;
				}
			}
		}
		default:
		{
			ASSERT(FALSE, "Unexpected nSearchID value in CQueryLList::bVerifySearchCriteria");
			return FALSE;
		}
	}
}


//=--------------------------------------------------------------------------=
// CQueryLList::bDeleteCell
//=--------------------------------------------------------------------------=
// frees a cell
//
// Parameters:
//    PVOID			- [in] cell content to free
//
// Output:
//    BOOL			TRUE if successfully released the cell content
//
// Notes:
//
BOOL CQueryLList::bDeleteCell(PVOID pvCell)
{
	PPRQUERY	pPrQuery = (PPRQUERY) pvCell;

	ASSERT(pPrQuery, "pPrQuery is NULL in CQueryLList::bDeleteCell");

	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "Freeing query '%X' in CQueryLList::bDeleteCell\n", (INT) pvCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif

	if (pPrQuery->szChannelName)
		delete [] pPrQuery->szChannelName;

	switch (pPrQuery->at)
	{
		case atUser:
		{
			PPRUSER	pPrUser = (PPRUSER) pPrQuery->pvData;

			ASSERT(pPrUser, "pPrUser is NULL in CQueryLList::bDeleteCell");
			FreePrUserContent(pPrUser);
			// free the PRUSER object itself
			delete pPrUser;
			pPrUser = NULL;
			break;
		}
		case atChannel:
		{
			PPRCHANNEL	pPrChannel = (PPRCHANNEL) pPrQuery->pvData;

			ASSERT(pPrChannel, "pPrChannel is NULL in CQueryLList::bDeleteCell");
			if (pPrChannel->bFree)
			{
				FreePrChannelContent(pPrChannel);
				// free the PRCHANNEL object itself
				delete pPrChannel;
				pPrChannel = NULL;
			}
			break;
		}
		case atMember:
		case atQuery:
		{
			// no work to do here ;-)
			break;
		}
		default:
		{
			ASSERT(FALSE, "Unexpected associated type in CQueryLList::bDeleteCell");
			return FALSE;
		}
	}
	return TRUE;
}


//////////////////////////////////////////////////////////////////////////////
// Chat Item List Virtual Functions
//////////////////////////////////////////////////////////////////////////////

CItemLList::CItemLList(void)
{
	m_lSafeArrayIndex	= 0L;
	m_psa				= NULL;
	m_bFreeSafeArray	= TRUE;

	VariantInit(&m_vValidItems);
}


CItemLList::~CItemLList(void)
{
	if (m_psa && m_bFreeSafeArray)
		SafeArrayDestroy(m_psa);
}


BOOL CItemLList::bDoActionOnList(SHORT nActionID)
{
	ASSERT(g_nActionFillSafeArray == nActionID, "Unexpected nActionID in CItemLList::bDoActionOnList");

	if (!m_dwCellCount)
	{
		m_vValidItems.vt = VT_EMPTY;
		m_vValidItems.lVal = 0L;
		return TRUE;
	}

	m_lSafeArrayIndex = 0L;
	
	if (m_psa && m_bFreeSafeArray)
	{
		SafeArrayDestroy(m_psa);
		m_psa = NULL;
	}

	m_psa = SafeArrayCreateVector(VT_VARIANT, 0L, m_dwCellCount);
	if (!m_psa)
		return FALSE;

	BOOL bRet = CLList::bDoActionOnList(nActionID);

	m_vValidItems.vt = VT_VARIANT|VT_ARRAY;
	m_vValidItems.parray = m_psa;

	return bRet;
}


//=--------------------------------------------------------------------------=
// CItemLList::bDoActionOnCell
//=--------------------------------------------------------------------------=
// executes an action on a cell of the list
//
// Parameters:
//    SHORT			- [in] action ID
//    PVOID			- [in] cell content
//
// Output:
//    BOOL			TRUE if action was successful
//
// Notes:
//
BOOL CItemLList::bDoActionOnCell(SHORT nActionID, PVOID pvCell)
{
	switch (nActionID)
	{
		case g_nActionFillSafeArray:
		{
			PPRITEM pPrItem = (PPRITEM) pvCell;

			ASSERT(m_psa, "m_psa is NULL in CItemLList::bDoActionOnCell");

			if (FAILED(SafeArrayPutElement(m_psa, &m_lSafeArrayIndex, (PVOID) &(pPrItem->vName))))
				return FALSE;

			m_lSafeArrayIndex++;
			break;
		}

		default:
		{
			ASSERT(FALSE, "Unexpected nActionID in CItemLList::bDoActionOnCell");
			return FALSE;
		}
	}

	return TRUE;
}


//=--------------------------------------------------------------------------=
// CItemLList::bIsEqual
//=--------------------------------------------------------------------------=
// checks if two cells are equal
//
// Parameters:
//    PVOID			- [in] first cell content
//    PVOID			- [in] second cell content
//
// Output:
//    BOOL			TRUE if cells are identical
//
// Notes:
//
BOOL CItemLList::bIsEqual(PVOID pvCell1, PVOID pvCell2)
{
	PPRITEM pPrItem1 = (PPRITEM) pvCell1;
	PPRITEM pPrItem2 = (PPRITEM) pvCell2;

	if (pPrItem1 == pPrItem2)
		return TRUE;

	if (!pPrItem1 || !pPrItem2)
		return FALSE;

	ASSERT(pPrItem1->vName.bstrVal, "pPrItem1->vName.bstrVal is NULL in CItemLList::bIsEqual");
	ASSERT(pPrItem2->vName.bstrVal, "pPrItem2->vName.bstrVal is NULL in CItemLList::bIsEqual");

	return 0 == WStricmp(pPrItem1->vName.bstrVal, pPrItem2->vName.bstrVal);
}


//=--------------------------------------------------------------------------=
// CItemLList::bVerifySearchCriteria
//=--------------------------------------------------------------------------=
// 
//
// Parameters:
//    SHORT			- [in] search ID
//    PVOID			- [in] search criteria
//    PVOID			- [in] pointer to chat item
//
// Output:
//    BOOL			TRUE if search condition is verified
//
// Notes:
//
BOOL CItemLList::bVerifySearchCriteria(SHORT nSearchID, PVOID pvData, PVOID pvCell)
{
	ASSERT(pvCell, "pvCell is NULL in CItemLList::bVerifySearchCriteria");

	PPRITEM pPrItem = (PPRITEM) pvCell;

	switch (nSearchID)
	{
		case g_nSearchNone:
		{
			return TRUE;
		}
		case g_nSearchByName:
		{
			ASSERT(pvData, "pvData is NULL in CItemLList::bVerifySearchCriteria");
			ASSERT(pPrItem->vName.bstrVal, "pPrItem->vName.bstrVal is NULL in CItemLList::bVerifySearchCriteria");
			return 0 == WStricmp(pPrItem->vName.bstrVal, (LPCWSTR) pvData);
		}
		default:
		{
			ASSERT(FALSE, "Unexpected nSearchID value in CItemLList::bVerifySearchCriteria");
			return FALSE;
		}
	}
}


//=--------------------------------------------------------------------------=
// CItemLList::bDeleteCell
//=--------------------------------------------------------------------------=
// frees a cell
//
// Parameters:
//    PVOID			- [in] cell content to free
//
// Output:
//    BOOL			TRUE if successfully released the cell content
//
// Notes:
//
BOOL CItemLList::bDeleteCell(PVOID pvCell)
{
	PPRITEM	pPrItem = (PPRITEM) pvCell;

	ASSERT(pPrItem, "pPrItem is NULL in CItemLList::bDeleteCell");

	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "Freeing chat item '%X' in CItemLList::bDeleteCell\n", (LONG) pvCell);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif

	VariantClear(&pPrItem->vName);
	VariantClear(&pPrItem->vValue);

	return TRUE;
}

