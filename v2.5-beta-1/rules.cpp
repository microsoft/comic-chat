//=--------------------------------------------------------------------------=
// Rules.Cpp:		Implementation of rules C++ classes
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 01/16/98

#include "stdafx.h"
#include "chat.h"
#include "cdebug.h"
#include "ccommon.h"
#include "format.h"
#include "ui.h"
#include "actions.h"
#include "protsupp.h"

// for ASSERT and FAIL
//
SZTHISFILE

#ifdef DEBUG
SHORT g_nRulesRefCount;
SHORT g_nDaemonsRefCount;
#endif

KEPSUBS g_rgKepSubs[]=
{
	kepMe,				_T("#Me#"),
	kepMyActivatedRoom, _T("#MyActivatedRoom#")
};


KAPSUBS g_rgKapSubs[]=
{
	kapMyActivatedRoom,	_T("#MyActivatedRoom#"),
	kapEventMessage,	_T("#EventMessage#"),
	kapEventNickname,	_T("#EventNickname#"),
	kapEventRoom,		_T("#EventRoom#"),
	kapEventServer,		_T("#EventServer#"),
	kapEventRecipients,	_T("#EventRecipients#"),
	kapMe,				_T("#Me#")
};


/////////////////////////////////////////////////////////////////////////////
// CCEvent::CCEvent - Constructor
CCEvent::CCEvent(const enumEvents eID,
				 const UINT uIDS_LongDesc,
				 const UINT uIDS_ShortDesc,
				 const UINT rguIDS_ParamDesc[g_uMaxEventParams],
				 const UINT uParamNum,
				 const UINT rguKeyParam[g_uMaxEventParams],
				 const enumParamType rgpt[g_uMaxEventParams],
				 const DWORD dwActionKeysExposed,
				 const DWORD dwEnabledActions,
				 const BOOL bNeedDaemon)
{
	m_eID					= eID;
	m_uIDS_LongDesc			= uIDS_LongDesc;
	m_uIDS_ShortDesc		= uIDS_ShortDesc;
	m_uParamNum				= uParamNum;
	m_dwActionKeysExposed	= dwActionKeysExposed;
	m_dwEnabledActions		= dwEnabledActions;
	m_bNeedDaemon			= bNeedDaemon;

	for (UINT uCnt = 0; uCnt < g_uMaxEventParams; uCnt++)
	{
		m_rguIDS_ParamDesc[uCnt]= rguIDS_ParamDesc[uCnt];
		m_rgpt[uCnt]			= rgpt[uCnt];
		m_rguKeyParam[uCnt]		= rguKeyParam[uCnt];
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCAction::CCAction - Constructor
CCAction::CCAction(const enumActions aID,
				   const UINT uIDS_LongDesc,
				   const UINT uIDS_ShortDesc,
				   const UINT rguIDS_ParamDesc[g_uMaxActionParams],
				   const UINT uParamNum,
				   const BOOL bDelayOK,
				   const UINT rguKeyParam[g_uMaxActionParams],
				   const enumParamType rgpt[g_uMaxActionParams])
{
	m_aID				= aID;
	m_uIDS_LongDesc		= uIDS_LongDesc;
	m_uIDS_ShortDesc	= uIDS_ShortDesc;
	m_uParamNum			= uParamNum;
	m_bDelayOK			= bDelayOK;

	for (UINT uCnt = 0; uCnt < g_uMaxActionParams; uCnt++)
	{
		m_rguIDS_ParamDesc[uCnt]= rguIDS_ParamDesc[uCnt];
		m_rgpt[uCnt]			= rgpt[uCnt];
		m_rguKeyParam[uCnt]		= rguKeyParam[uCnt];
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCRulesData::CCRulesData - Constructor
CCRulesData::CCRulesData()
{
	UINT uCnt;

	m_bInitAlloc = m_bStringsLoaded = FALSE;

	for (uCnt = 0; uCnt < ptMax; uCnt++)
	{
		m_rguIDS_MissingEventParamError[uCnt] = IDS_MISSING_EVENT_PARAM_ERROR0 + uCnt;
		m_rguIDS_MissingActionParamError[uCnt] = IDS_MISSING_ACTION_PARAM_ERROR0 + uCnt;
		//m_rgszMissingEventParamError[uCnt] = NULL;
		//m_rgszMissingActionParamError[uCnt] = NULL;
	}

	for (uCnt = 0; uCnt < eMax; uCnt++)
		m_rgpEvents[uCnt] = NULL;
	
	for (uCnt = 0; uCnt < aMax; uCnt++)
		m_rgpActions[uCnt] = NULL;
}


/////////////////////////////////////////////////////////////////////////////
// CCRulesData::CCRulesData - Destructor
CCRulesData::~CCRulesData()
{
	UINT uCnt;

	for (uCnt = 0; uCnt < eMax; uCnt++)
		if (m_rgpEvents[uCnt])
			delete m_rgpEvents[uCnt];
	
	for (uCnt = 0; uCnt < aMax; uCnt++)
		if (m_rgpActions[uCnt])
			delete m_rgpActions[uCnt];
}


/////////////////////////////////////////////////////////////////////////////
// CCRulesData::bInitAlloc - Mem allocator
BOOL CCRulesData::bInitAlloc()
{
	UINT uCnt, uCnt2;
	UINT rguIDS_ParamDesc[g_uMaxEventParams];

	if (m_bInitAlloc)
		return TRUE;

	for (uCnt = 0; uCnt < eMax; uCnt++)
	{
		for (uCnt2 = 0; uCnt2 < g_rguEventParamNums[uCnt]; uCnt2++)
			rguIDS_ParamDesc[uCnt2] = IDS_EVENT_PARAM_DESC0 + uCnt*g_uMaxEventParams + uCnt2;

		for (uCnt2 = g_rguEventParamNums[uCnt]; uCnt2 < g_uMaxEventParams; uCnt2++)
			rguIDS_ParamDesc[uCnt2] = 0;

		if (!(m_rgpEvents[uCnt] = new CCEvent((enumEvents) uCnt,
											 IDS_EVENT_LONG_DESC0 + uCnt,
											 IDS_EVENT_SHORT_DESC0 + uCnt,
											 rguIDS_ParamDesc,
											 g_rguEventParamNums[uCnt],
											 g_rguEventKeyParams[uCnt],
											 g_rgptEventParamTypes[uCnt],
											 g_rgdwEventExposedActionKeys[uCnt],
											 g_rgdwEventEnabledActions[uCnt],
											 g_rgbEventNeedDaemon[uCnt])))
			return FALSE;
	}
	
	for (uCnt = 0; uCnt < aMax; uCnt++)
	{
		for (uCnt2 = 0; uCnt2 < ActionParamNum(uCnt); uCnt2++)
			rguIDS_ParamDesc[uCnt2] = IDS_ACTION_PARAM_DESC0 + uCnt*g_uMaxActionParams + uCnt2;

		for (uCnt2 = ActionParamNum(uCnt); uCnt2 < g_uMaxActionParams; uCnt2++)
			rguIDS_ParamDesc[uCnt2] = 0;

		if (!(m_rgpActions[uCnt] = new CCAction((enumActions) uCnt,
											    IDS_ACTION_LONG_DESC0 + uCnt,
											    IDS_ACTION_SHORT_DESC0 + uCnt,
												rguIDS_ParamDesc,
												ActionParamNum(uCnt),
												ActionDelayOK(uCnt),
												g_rguActionKeyParams[uCnt],
												g_rgptActionParamTypes[uCnt])))
			return FALSE;
	}

	m_bInitAlloc = TRUE;
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCRulesData::bLoadStrings -
BOOL CCRulesData::bLoadStrings()
{
	UINT uCnt, uCnt2;

	ASSERT(m_bInitAlloc, "m_bInitAlloc is FALSE in CCRulesData::bLoadStrings");
	if (m_bStringsLoaded)
		return TRUE;

	for (uCnt = 0; uCnt < eMax; uCnt++)
	{
		ASSERT(m_rgpEvents[uCnt], "m_rgpEvents[uCnt] is NULL in CCRulesData::bLoadStrings");

		for (uCnt2 = 0; uCnt2 < m_rgpEvents[uCnt]->m_uParamNum; uCnt2++)
			if (!m_rgpEvents[uCnt]->m_rgstrParamDesc[uCnt2].LoadString(m_rgpEvents[uCnt]->m_rguIDS_ParamDesc[uCnt2]))
				return FALSE;

		if (!m_rgpEvents[uCnt]->m_strLongDesc.LoadString(m_rgpEvents[uCnt]->m_uIDS_LongDesc))
			return FALSE;

		if (!m_rgpEvents[uCnt]->m_strShortDesc.LoadString(m_rgpEvents[uCnt]->m_uIDS_ShortDesc))
			return FALSE;
	}

	for (uCnt = 0; uCnt < aMax; uCnt++)
	{
		ASSERT(m_rgpActions[uCnt], "m_rgpActions[uCnt] is NULL in CCRulesData::bLoadStrings");

		for (uCnt2 = 0; uCnt2 < m_rgpActions[uCnt]->m_uParamNum; uCnt2++)
			if (!m_rgpActions[uCnt]->m_rgstrParamDesc[uCnt2].LoadString(m_rgpActions[uCnt]->m_rguIDS_ParamDesc[uCnt2]))
				return FALSE;

		if (!m_rgpActions[uCnt]->m_strLongDesc.LoadString(m_rgpActions[uCnt]->m_uIDS_LongDesc))
			return FALSE;

		if (!m_rgpActions[uCnt]->m_strShortDesc.LoadString(m_rgpActions[uCnt]->m_uIDS_ShortDesc))
			return FALSE;
	}

	for (uCnt = 0; uCnt < kepMax; uCnt++)
		if (!m_rgstrKeyEventParam[uCnt].LoadString(IDS_KEY_EVENT_PARAM0+uCnt))	// Load %Me%, %Any%, %ActiveRoom%, etc...
			return FALSE;

	for (uCnt = 0; uCnt < kapMax; uCnt++)
		if (!m_rgstrKeyActionParam[uCnt].LoadString(IDS_KEY_ACTION_PARAM0+uCnt))	// Load %EventRoom%, etc...
			return FALSE;

	if (!m_strEventsDesc.LoadString(IDS_EVENTCMBDESC))
		return FALSE;

	if (!m_strActionsDesc.LoadString(IDS_ACTIONCMBDESC))
		return FALSE;

	m_bStringsLoaded = TRUE;
	return TRUE;
}


CCEvent* CCRulesData::GetEvent(UINT uIndex)
{
	ASSERT(m_bInitAlloc, "m_bInitAlloc is FALSE in CCRulesData::GetEvent");

	return m_rgpEvents[uIndex];
}


CCAction* CCRulesData::GetAction(UINT uIndex)
{
	ASSERT(m_bInitAlloc, "m_bInitAlloc is FALSE in CCRulesData::GetAction");

	return m_rgpActions[uIndex];
}


CString CCRulesData::StrFindAndReplaceKeyParams(CString strIn, BOOL bIncoming)
{
	UINT	uIndexKey, uIndexMax = sizeof(g_rgKepSubs)/sizeof(KEPSUBS);
	INT		iBegin, iBeginOri;
	CString	strUpperKeyParam, strUpperParam, strFrom, strTo = strIn;

	for (uIndexKey = 0; uIndexKey < (UINT) uIndexMax; uIndexKey++)
	{
		iBeginOri = 0;
		strFrom = strTo;
		strTo.Empty();
		strUpperParam = strFrom;
		strUpperParam.MakeUpper();
		strUpperKeyParam = bIncoming ? g_rgKepSubs[uIndexKey].szSubs : GetKeyEventParam(g_rgKepSubs[uIndexKey].kep);
		strUpperKeyParam.MakeUpper();
		while ((iBegin = strUpperParam.Find(strUpperKeyParam)) != -1)
		{
			strTo += strFrom.Mid(iBeginOri, iBegin);
			iBeginOri += iBegin + strUpperKeyParam.GetLength();
			strTo += bIncoming ? GetKeyEventParam(g_rgKepSubs[uIndexKey].kep) : g_rgKepSubs[uIndexKey].szSubs;
			strUpperParam = strUpperParam.Mid(iBeginOri);
		}
		strTo += strFrom.Mid(iBeginOri);
	}

	uIndexMax = sizeof(g_rgKapSubs)/sizeof(KAPSUBS);
	for (uIndexKey = 0; uIndexKey < (UINT) uIndexMax; uIndexKey++)
	{
		iBeginOri = 0;
		strFrom = strTo;
		strTo.Empty();
		strUpperParam = strFrom;
		strUpperParam.MakeUpper();
		strUpperKeyParam = bIncoming ? g_rgKapSubs[uIndexKey].szSubs : GetKeyActionParam(g_rgKapSubs[uIndexKey].kap);
		strUpperKeyParam.MakeUpper();
		while ((iBegin = strUpperParam.Find(strUpperKeyParam)) != -1)
		{
			strTo += strFrom.Mid(iBeginOri, iBegin);
			iBeginOri += iBegin + strUpperKeyParam.GetLength();
			strTo += bIncoming ? GetKeyActionParam(g_rgKapSubs[uIndexKey].kap) : g_rgKapSubs[uIndexKey].szSubs;
			strUpperParam = strUpperParam.Mid(iBeginOri);
		}
		strTo += strFrom.Mid(iBeginOri);
	}

	return strTo;
}


BOOL CCChannel::operator==(const CCChannel& channel)
{
	return m_strChannelName == channel.m_strChannelName;
}


void CCItemPtrArray::FreeRemoveAll()
{
	CCChannel*	pChannel;
	CUser*		pUser;
	UINT		uCnt, uItems = GetSize();

	for (uCnt = 0; uCnt < uItems; uCnt++)
	{
		switch (m_it)
		{
		case itUser:
			pUser = (CUser*) GetAt(uCnt);
			ASSERT(pUser, "pUser is NULL in CCItemPtrArray::FreeRemoveAll");
			pUser->Release();
			break;
		case itChannel:
			pChannel = (CCChannel*) GetAt(uCnt);
			ASSERT(pChannel, "pChannel is NULL in CCItemPtrArray::FreeRemoveAll");
			delete pChannel;
			break;
		default:
			ASSERT(FALSE, "Unexpected item type in CCItemPtrArray::FreeRemoveAll");
		}
	}

	CPtrArray::RemoveAll();
}


/////////////////////////////////////////////////////////////////////////////
// CCDaemonExt::CCDaemonExt - Constructor
CCDaemonExt::CCDaemonExt(enumItemTypes it)
{
	m_it				= it;
	m_nRefCount			= 1;
	m_bResetItemLists	= FALSE;
	m_bClearedItemLists	= FALSE;

	#ifdef DEBUG
		g_nDaemonsRefCount++;
	#endif
}


CCDaemonExt::~CCDaemonExt()
{
	bCleanUpItemLists();
}


void CCDaemonExt::AddRef()
{
	m_nRefCount++;

	#ifdef DEBUG
		g_nDaemonsRefCount++;
	#endif
}


void CCDaemonExt::Release()
{
	#ifdef DEBUG
		ASSERT(g_nDaemonsRefCount > 0, "g_nDaemonsRefCount <= 0 in CCDaemonExt::Release");
		g_nDaemonsRefCount--;
	#endif

	ASSERT(m_nRefCount > 0, "m_nRefCount <= 0 in CCDaemonExt::Release");

	if (--m_nRefCount == 0)
		delete this;
}


BOOL CCDaemonExt::bAllocNewItemList(UINT uListCount = 1)
{
	ASSERT(uListCount == 1 || uListCount == 2, "uListCount out of range in CCDaemonExt::bAllocNewItemList");

	BOOL bRet = TRUE;
	CCItemPtrArray* pNewItemList;

	for (UINT uCnt = 0; uCnt < uListCount && bRet; uCnt++)
	{
		ASSERT(m_it == itUser || m_it == itChannel, "Unexpected item type in CCDaemonExt::bAllocNewItemList");
		pNewItemList = (CCItemPtrArray*) new CCItemPtrArray(m_it);
		if (!pNewItemList)
			return FALSE;

		pNewItemList->SetSize(0 /*nNewSize*/, 10 /*nGrowBy*/);
		pNewItemList->m_nCredits = 2 / uListCount + uCnt;

		bRet &= m_itemLists.AddTail((void*) pNewItemList) > 0;
	}

	return bRet;
}


BOOL CCDaemonExt::bCleanUpItemLists()
{
	// OutputDebugThreadIdString("CCDaemonExt::bCleanUpItemLists - Enter\n");

	POSITION		pos;
	CCItemPtrArray*	pItemList;

    for (pos = m_itemLists.GetHeadPosition(); pos != NULL; )
    {
		pItemList = (CCItemPtrArray*) m_itemLists.GetNext(pos);
		ASSERT(pItemList, "pItemList is NULL in CCDaemonExt::bCleanUpItemLists");
		delete pItemList;	// does a call to FreeRemoveAll()
	}

	m_itemLists.RemoveAll();
	m_bClearedItemLists = TRUE;

	return TRUE;
}


BOOL CCDaemonExt::bAddChannelToCurrentList(CString& strChannelName)
{
	CCChannel* pChannel = new CCChannel;

	if (!pChannel)
		return FALSE;

	pChannel->m_strChannelName = strChannelName;

	// Current list is most recently add channel list in m_itemLists
	ASSERT(m_itemLists.GetCount() >= 2, "Unexpected channel list count in CCDaemonExt::bAddChannelToCurrentList");
	CCItemPtrArray	*pCurrentItemList = (CCItemPtrArray*) m_itemLists.GetTail();

	ASSERT(pCurrentItemList, "pCurrentItemList is NULL in CCDaemonExt::bAddChannelToCurrentList");
	return pCurrentItemList->Add((void*) pChannel) >= 0;
}


BOOL CCDaemonExt::bAddUserToCurrentList(CUser* pUser)
{	
	ASSERT(pUser, "pUser is NULL in CCDaemonExt::bAddUserToCurrentList");

	// Current list is most recently add user list in m_itemLists
	ASSERT(m_itemLists.GetCount() >= 2, "Unexpected user list count in CCDaemonExt::bAddUserToCurrentList");
	CCItemPtrArray	*pCurrentItemList = (CCItemPtrArray*) m_itemLists.GetTail();

	ASSERT(pCurrentItemList, "pCurrentItemList is NULL in CCDaemonExt::bAddUserToCurrentList");
	return pCurrentItemList->Add((void*) pUser) >= 0;
}


BOOL CCDaemonExt::bOnEndOfListing(CCDynaRules* pDynaRules, CCRule* pRule, enumQueryPurpose qp)
{
	ASSERT(pDynaRules, "pDynaRules is NULL in CCDaemonExt::bOnEndOfListing");
	ASSERT(pRule, "pRule is NULL in CCDaemonExt::bOnEndOfListing");
	
	BOOL			bRet;
	POSITION		posPrevious, posCurrent;
	CCItemPtrArray	*pPreviousItemList, *pCurrentItemList;

	switch (qp)
	{
		case qpOnConnectEvent:
		case qpOnDisconnectEvent:
		case qpOnNewRoomEvent:
		{
			ASSERT(m_itemLists.GetCount() >= 2, "Unexpected list count in CCDaemonExt::bOnEndOfListing");

			// pPreviousItemList and pCurrentItemList are the two most recent user lists
			posPrevious = posCurrent = m_itemLists.GetTailPosition();
			ASSERT(posCurrent, "posCurrent is NULL in CCDaemonExt::bOnEndOfListing");
			pCurrentItemList = (CCItemPtrArray*) m_itemLists.GetPrev(posPrevious);
			ASSERT(posPrevious, "posPrevious is NULL in CCDaemonExt::bOnEndOfListing");
			pPreviousItemList = (CCItemPtrArray*) m_itemLists.GetAt(posPrevious);

			ASSERT(pCurrentItemList, "pCurrentItemList is NULL in CCDaemonExt::bOnEndOfListing");
			ASSERT(pPreviousItemList, "pPreviousItemList is NULL in CCDaemonExt::bOnEndOfListing");

			ASSERT(pPreviousItemList->m_nCredits > 0, "pPreviousItemList->m_nCredits <= 0 in CCDaemonExt::bOnEndOfListing");
			ASSERT(pCurrentItemList->m_nCredits > 0, "pCurrentItemList->m_nCredits <= 0 in CCDaemonExt::bOnEndOfListing");

			// Prepare next iteration: Add a new list to the user list list
			if (!bAllocNewItemList())
				return FALSE;

			m_bClearedItemLists = FALSE;

			if (qpOnConnectEvent == qp || qpOnNewRoomEvent == qp)
				// Check which users are in the current list and not in the previous one
				bRet = bTreatNewItems(pDynaRules, pRule, pPreviousItemList, pCurrentItemList);
			else
				// Check which users are in the previous list and not in the current one
				bRet = bTreatOldItems(pDynaRules, pRule, pPreviousItemList, pCurrentItemList);

			ASSERT(bRet, "bTreat<New|Old>Items call failed in CCDaemonExt::bOnEndOfListing");

			// An action might have emptied the m_itemLists - Fix for #2542
			if (!m_bClearedItemLists)
			{
				// Empty and remove oldest user list
				pPreviousItemList->m_nCredits--;
				ASSERT(pPreviousItemList->m_nCredits >= 0, "pPreviousItemList->m_nCredits < 0 in CCDaemonExt::bOnEndOfListing");
				if (0 == pPreviousItemList->m_nCredits)
				{
					pPreviousItemList->FreeRemoveAll();
					m_itemLists.RemoveAt(posPrevious);
					delete pPreviousItemList;
				}

				pCurrentItemList->m_nCredits--;
				ASSERT(pCurrentItemList->m_nCredits >= 0, "pCurrentItemList->m_nCredits < 0 in CCDaemonExt::bOnEndOfListing");
				if (0 == pCurrentItemList->m_nCredits)
				{
					pCurrentItemList->FreeRemoveAll();
					m_itemLists.RemoveAt(posCurrent);
					delete pCurrentItemList;
				}
			}
			return bRet;
		}
		default:
			ASSERT(FALSE, "Unexpected qp value in CCDaemonExt::bOnEndOfListing");
	}
	return TRUE;
}


BOOL CCDaemonExt::bTreatNewItems(CCDynaRules* pDynaRules, CCRule* pRule, CCItemPtrArray* pPreviousItemList, CCItemPtrArray* pCurrentItemList)
{
	// Find and treat users that are in pCurrentItemList but not in pPreviousItemList
	ASSERT(m_it == itUser || m_it == itChannel, "Unexpected item type in CCDaemonExt::bTreatNewItems");
	ASSERT(pDynaRules, "pDynaRules is NULL in CCDaemonExt::bTreatNewItems");
	ASSERT(pRule, "pRule is NULL in CCDaemonExt::bTreatNewItems");

	ASSERT(pPreviousItemList, "pPreviousItemList is NULL in CCDaemonExt::bTreatNewItems");
	ASSERT(pCurrentItemList, "pCurrentItemList is NULL in CCDaemonExt::bTreatNewItems");

	BOOL		bFoundPrevious;

	UINT		uPreviousItems = pPreviousItemList->GetSize();
	UINT		uCurrentItems = pCurrentItemList->GetSize();

	UINT		uPreviousCnt, uCurrentCnt;

	PVOID		pvPreviousItem, pvCurrentItem;
	CUser		*pPreviousUser, *pCurrentUser = NULL;
	CCChannel	*pPreviousChannel, *pCurrentChannel = NULL;

	for (uCurrentCnt = 0; uCurrentCnt < uCurrentItems; uCurrentCnt++)
	{
		pvCurrentItem = pCurrentItemList->GetAt(uCurrentCnt);
		ASSERT(pvCurrentItem, "pvCurrentItem is NULL in CCDaemonExt::bTreatNewItems");

		if (itUser == m_it)
			pCurrentUser = (CUser*) pvCurrentItem;
		else
			pCurrentChannel = (CCChannel*) pvCurrentItem;

		bFoundPrevious = FALSE;
		for (uPreviousCnt = 0; uPreviousCnt < uPreviousItems; uPreviousCnt++)
		{
			pvPreviousItem = pPreviousItemList->GetAt(uPreviousCnt);
			ASSERT(pvPreviousItem, "pvPreviousItem is NULL in CCDaemonExt::bTreatNewItems");
			if (itUser == m_it)
			{
				pPreviousUser = (CUser*) pvPreviousItem;
				if (pCurrentUser->m_strIdentity == pPreviousUser->m_strIdentity)
				{
					bFoundPrevious = TRUE;
					break;
				}
			}
			else
			{
				pPreviousChannel = (CCChannel*) pvPreviousItem;
				if (*pCurrentChannel == *pPreviousChannel)
				{
					bFoundPrevious = TRUE;
					break;
				}
			}
		}
		if (!bFoundPrevious)
		{
			// we have a new item
			CString strCurrentServer = pDynaRules->m_pfGetKeyEventParam(ptServerName);
			CString strCurrentIdentity, strCurrentChannelTmp;
			
			if (m_it == itUser)
			{
				ASSERT(pCurrentUser, "pCurrentUser is NULL in CCDaemonExt::bTreatNewItems");
				strCurrentIdentity = pCurrentUser->m_strNickname + "!" + pCurrentUser->m_strIdentity;
			}
			else
			{
				ASSERT(pCurrentChannel, "pCurrentChannel is NULL in CCDaemonExt::bTreatNewItems");
				strCurrentChannelTmp = pCurrentChannel->m_strChannelName;
			}

			pDynaRules->SetCachVariables(pRule->GetEvent()->GetID(), 
										 strCurrentIdentity,
										 strCurrentServer,
										 strCurrentChannelTmp);

			OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDaemonExt::bTreatNewItems - Match event: ") + pRule->StrGetEventDisplay() + "\n"));
			OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDaemonExt::bTreatNewItems - Execute action: ") + pRule->StrGetActionDisplay() + "\n"));

			pDynaRules->bReplaceKeyActionParams(pRule);

			ASSERT(pDynaRules->m_pfExecuteAction, "m_pfExecuteAction is NULL in CCDaemonExt::bTreatNewItems");

			CCActionContext actCtx;

			actCtx.bInitActionContext(pDynaRules, pRule);
			pDynaRules->m_pfExecuteAction(pDynaRules, pRule, &actCtx);
		}
	}
	return TRUE;
}


BOOL CCDaemonExt::bTreatOldItems(CCDynaRules* pDynaRules, CCRule* pRule, CCItemPtrArray* pPreviousItemList, CCItemPtrArray* pCurrentItemList)
{
	// Find and treat users that are in pPreviousItemList but not in pCurrentItemList
	ASSERT(pDynaRules, "pDynaRules is NULL in CCDaemonExt::bTreatOldItems");
	ASSERT(pRule, "pRule is NULL in CCDaemonExt::bTreatOldItems");

	ASSERT(pPreviousItemList, "pPreviousItemList is NULL in CCDaemonExt::bTreatOldItems");
	ASSERT(pCurrentItemList, "pCurrentItemList is NULL in CCDaemonExt::bTreatOldItems");
	ASSERT(m_it == itUser, "Unexpected item type in CCDaemonExt::bTreatOldItems");	// for now there is no OnRoomClosed event

	BOOL	bFoundCurrent;

	UINT	uPreviousItems = pPreviousItemList->GetSize();
	UINT	uCurrentItems = pCurrentItemList->GetSize();

	UINT	uPreviousCnt, uCurrentCnt;

	CUser	*pPreviousUser, *pCurrentUser;

	for (uPreviousCnt = 0; uPreviousCnt < uPreviousItems; uPreviousCnt++)
	{
		pPreviousUser = (CUser*) pPreviousItemList->GetAt(uPreviousCnt);
		ASSERT(pPreviousUser, "pPreviousUser is NULL in CCDaemonExt::bTreatOldItems");
		bFoundCurrent = FALSE;
		for (uCurrentCnt = 0; uCurrentCnt < uCurrentItems; uCurrentCnt++)
		{
			pCurrentUser = (CUser*) pCurrentItemList->GetAt(uCurrentCnt);
			ASSERT(pCurrentUser, "pCurrentUser is NULL in CCDaemonExt::bTreatOldItems");
			if (pCurrentUser->m_strIdentity == pPreviousUser->m_strIdentity)
			{
				bFoundCurrent = TRUE;
				break;
			}
		}
		if (!bFoundCurrent)
		{
			// a user is gone
			CString strCurrentServer = pDynaRules->m_pfGetKeyEventParam(ptServerName);
			CString strPreviousIdentity = pPreviousUser->m_strNickname + "!" + pPreviousUser->m_strIdentity;

			pDynaRules->SetCachVariables(pRule->GetEvent()->GetID(), 
										 strPreviousIdentity,
										 strCurrentServer,
										 CString(""));

			OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDaemonExt::bTreatOldItems - Match event: ") + pRule->StrGetEventDisplay() + "\n"));
			OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDaemonExt::bTreatOldItems - Execute action: ") + pRule->StrGetActionDisplay() + "\n"));

			pDynaRules->bReplaceKeyActionParams(pRule);

			ASSERT(pDynaRules->m_pfExecuteAction, "m_pfExecuteAction is NULL in CCDaemonExt::bTreatOldItems");

			CCActionContext actCtx;

			actCtx.bInitActionContext(pDynaRules, pRule);
			pDynaRules->m_pfExecuteAction(pDynaRules, pRule, &actCtx);
		}
	}
	return TRUE;
}


BOOL CCDaemonExt::bOnEndOfListing(CCDynaNotifs* pDynaNotifs, CCNotif* pNotif)
{
	ASSERT(pDynaNotifs, "pDynaNotifs is NULL in CCDaemonExt::bOnEndOfListing");
	ASSERT(pNotif, "pNotif is NULL in CCDaemonExt::bOnEndOfListing");
	ASSERT(m_itemLists.GetCount() >= 2, "Unexpected list count in CCDaemonExt::bOnEndOfListing");

	BOOL			bRet;
	POSITION		posPrevious, posCurrent;
	CCItemPtrArray	*pPreviousItemList, *pCurrentItemList;

	pDynaNotifs->DecrementWhosCount();

	// pPreviousItemList and pCurrentItemList are the two most recent user lists
	posPrevious = posCurrent = m_itemLists.GetTailPosition();
	ASSERT(posCurrent, "posCurrent is NULL in CCDaemonExt::bOnEndOfListing");
	pCurrentItemList = (CCItemPtrArray*) m_itemLists.GetPrev(posPrevious);
	ASSERT(posPrevious, "posPrevious is NULL in CCDaemonExt::bOnEndOfListing");
	pPreviousItemList = (CCItemPtrArray*) m_itemLists.GetAt(posPrevious);

	ASSERT(pCurrentItemList, "pCurrentItemList is NULL in CCDaemonExt::bOnEndOfListing");
	ASSERT(pPreviousItemList, "pPreviousItemList is NULL in CCDaemonExt::bOnEndOfListing");

	ASSERT(pPreviousItemList->m_nCredits > 0, "pPreviousItemList->m_nCredits <= 0 in CCDaemonExt::bOnEndOfListing");
	ASSERT(pCurrentItemList->m_nCredits > 0, "pCurrentItemList->m_nCredits <= 0 in CCDaemonExt::bOnEndOfListing");

	// Prepare next iteration: Add a new list to the user list list
	if (!bAllocNewItemList())
		return FALSE;

	// Check which users are in the previous list and not in the current one -> gone users
	bRet = bTreatOldItems(pDynaNotifs, pNotif, pPreviousItemList, pCurrentItemList);
	ASSERT(bRet, "bTreatOldItems call failed in CCDaemonExt::bOnEndOfListing");

	// Check which users are in the current list and not in the previous one -. new users
	bRet = bTreatNewItems(pDynaNotifs, pNotif, pPreviousItemList, pCurrentItemList);
	ASSERT(bRet, "bTreatNewItems call failed in CCDaemonExt::bOnEndOfListing");

	// Empty and remove oldest user list
	pPreviousItemList->m_nCredits--;
	ASSERT(pPreviousItemList->m_nCredits >= 0, "pPreviousItemList->m_nCredits < 0 in CCDaemonExt::bOnEndOfListing");
	if (0 == pPreviousItemList->m_nCredits)
	{
		pPreviousItemList->FreeRemoveAll();
		m_itemLists.RemoveAt(posPrevious);
		delete pPreviousItemList;
	}

	pCurrentItemList->m_nCredits--;
	ASSERT(pCurrentItemList->m_nCredits >= 0, "pCurrentItemList->m_nCredits < 0 in CCDaemonExt::bOnEndOfListing");
	if (0 == pCurrentItemList->m_nCredits)
	{
		pCurrentItemList->FreeRemoveAll();
		m_itemLists.RemoveAt(posCurrent);
		delete pCurrentItemList;
	}

	if (0 == pDynaNotifs->GetWhosCount())
		// All WHOs are finished, let's display the notification dialog box
		pDynaNotifs->m_pfDisplayNotifications(pDynaNotifs);

	if (0 < pDynaNotifs->GetUpdateCount())
	{
		// There is a queued update request, let's start it
		pDynaNotifs->DecrementUpdateCount();
		if (pDynaNotifs->bUpdateNotifs())
			pDynaNotifs->m_pfSignalNewUpdate(pDynaNotifs);
	}

	return bRet;
}


BOOL CCDaemonExt::bTreatNewItems(CCDynaNotifs* pDynaNotifs, CCNotif* pNotif, CCItemPtrArray* pPreviousItemList, CCItemPtrArray* pCurrentItemList)
{
	// Find and treat users that are in pCurrentItemList but not in pPreviousItemList
	ASSERT(m_it == itUser, "Unexpected item type in CCDaemonExt::bTreatNewItems");
	ASSERT(pDynaNotifs, "pDynaNotifs is NULL in CCDaemonExt::bTreatNewItems");
	ASSERT(pNotif, "pNotif is NULL in CCDaemonExt::bTreatNewItems");

	ASSERT(pPreviousItemList, "pPreviousItemList is NULL in CCDaemonExt::bTreatNewItems");
	ASSERT(pCurrentItemList, "pCurrentItemList is NULL in CCDaemonExt::bTreatNewItems");

	BOOL		bFoundPrevious;
	UINT		uPreviousItems = pPreviousItemList->GetSize();
	UINT		uCurrentItems = pCurrentItemList->GetSize();
	UINT		uPreviousCnt, uCurrentCnt;
	CUser		*pPreviousUser, *pCurrentUser = NULL;

	for (uCurrentCnt = 0; uCurrentCnt < uCurrentItems; uCurrentCnt++)
	{
		pCurrentUser = (CUser*) pCurrentItemList->GetAt(uCurrentCnt);
		ASSERT(pCurrentUser, "pCurrentUser is NULL in CCDaemonExt::bTreatNewItems");

		bFoundPrevious = FALSE;
		for (uPreviousCnt = 0; uPreviousCnt < uPreviousItems; uPreviousCnt++)
		{
			pPreviousUser = (CUser*) pPreviousItemList->GetAt(uPreviousCnt);
			ASSERT(pPreviousUser, "pPreviousUser is NULL in CCDaemonExt::bTreatNewItems");
			if (pCurrentUser->m_strNickname == pPreviousUser->m_strNickname &&
				pCurrentUser->m_strIdentity == pPreviousUser->m_strIdentity &&
				pCurrentUser->m_strPrettyRoom == pPreviousUser->m_strPrettyRoom)
			{
				bFoundPrevious = TRUE;
				break;
			}
		}
		if (!bFoundPrevious)
		{
			// we have a new item
			// CString strCurrentServer = pDynaNotifs->m_pfGetKeyEventParam(ptServerName);
			CString strCurrentIdentity = pCurrentUser->m_strNickname + "!" + pCurrentUser->m_strIdentity;

			OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDaemonExt::bTreatNewItems - New user: ") + strCurrentIdentity + "\n"));

			pDynaNotifs->bAddNotificationUser(pCurrentUser);
		}
	}

	return TRUE;
}


BOOL CCDaemonExt::bTreatOldItems(CCDynaNotifs* pDynaNotifs, CCNotif* pNotif, CCItemPtrArray* pPreviousItemList, CCItemPtrArray* pCurrentItemList)
{
	// Find and treat users that are in pPreviousItemList but not in pCurrentItemList
	ASSERT(pDynaNotifs, "pDynaNotifs is NULL in CCDaemonExt::bTreatOldItems");
	ASSERT(pNotif, "pNotif is NULL in CCDaemonExt::bTreatOldItems");

	ASSERT(pPreviousItemList, "pPreviousItemList is NULL in CCDaemonExt::bTreatOldItems");
	ASSERT(pCurrentItemList, "pCurrentItemList is NULL in CCDaemonExt::bTreatOldItems");
	ASSERT(m_it == itUser, "Unexpected item type in CCDaemonExt::bTreatOldItems");

	BOOL	bFoundCurrent;

	UINT	uPreviousItems = pPreviousItemList->GetSize();
	UINT	uCurrentItems = pCurrentItemList->GetSize();

	UINT	uPreviousCnt, uCurrentCnt;

	CUser	*pPreviousUser, *pCurrentUser;

	for (uPreviousCnt = 0; uPreviousCnt < uPreviousItems; uPreviousCnt++)
	{
		pPreviousUser = (CUser*) pPreviousItemList->GetAt(uPreviousCnt);
		ASSERT(pPreviousUser, "pPreviousUser is NULL in CCDaemonExt::bTreatOldItems");
		bFoundCurrent = FALSE;
		for (uCurrentCnt = 0; uCurrentCnt < uCurrentItems; uCurrentCnt++)
		{
			pCurrentUser = (CUser*) pCurrentItemList->GetAt(uCurrentCnt);
			ASSERT(pCurrentUser, "pCurrentUser is NULL in CCDaemonExt::bTreatOldItems");
			if (pCurrentUser->m_strNickname == pPreviousUser->m_strNickname &&
				pCurrentUser->m_strIdentity == pPreviousUser->m_strIdentity &&
				pCurrentUser->m_strPrettyRoom == pPreviousUser->m_strPrettyRoom)
			{
				bFoundCurrent = TRUE;
				break;
			}
		}
		if (!bFoundCurrent)
		{
			// we have an old item
			// CString strCurrentServer = pDynaNotifs->m_pfGetKeyEventParam(ptServerName);
			CString strPreviousIdentity = pPreviousUser->m_strNickname + "!" + pPreviousUser->m_strIdentity;

			OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDaemonExt::bTreatOldItems - Old user: ") + strPreviousIdentity + "\n"));

			pDynaNotifs->bModifyNotificationUser(pPreviousUser, 0 /*wAddFlags*/, g_wConnected /*wRemoveFlags*/);
		}
	}
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCRule::CCRule - Constructor
CCRule::CCRule(CCDynaRules* pDynaRules)
{
	m_nRefCount					= 1;
	#ifdef DEBUG
		g_nRulesRefCount++;
	#endif

	m_pDynaRules				= pDynaRules;

	m_pDaemonExt				= NULL;
	m_prgdwMsgFormatting		= NULL;
	m_prgdwFinalMsgFormatting	= NULL;
	m_pEvent					= NULL;
	m_pAction					= NULL;	
	m_wFlags					= 0;
	m_uPeriodStart				= 0;
	m_uOccurrences				= 0;
	m_uDelay					= 0;

	for (UINT uCnt = 0; uCnt < g_uMaxEventParams; uCnt++)
		m_rgkep[uCnt] = kepMax;

	for (uCnt = 0; uCnt < g_uMaxActionParams; uCnt++)
		m_rgkap[uCnt] = kapMax;
}


CCRule::CCRule(CCRule* pRule, CCDynaRules* pDynaRules)
{
	ASSERT(pRule, "pRule is NULL in CCRule::CCRule");
	m_nRefCount = 1;
	#ifdef DEBUG
		g_nRulesRefCount++;
	#endif

	m_pDynaRules				= pDynaRules;
	m_prgdwMsgFormatting		= NULL;
	m_prgdwFinalMsgFormatting	= NULL;

	m_uPeriodStart				= pRule->m_uPeriodStart;
	m_uOccurrences				= pRule->m_uOccurrences;

	if (m_pDaemonExt = pRule->m_pDaemonExt)
		m_pDaemonExt->AddRef();

	CopyRule(pRule);
}


/////////////////////////////////////////////////////////////////////////////
// CCRule::CopyRule
void CCRule::CopyRule(CCRule* pRule)
{
	// Copy of all parameters that can change in the EditRule dialog
	ASSERT(pRule, "pRule is NULL in CCRule::CopyRule");

	m_pEvent	= pRule->m_pEvent;
	m_pAction	= pRule->m_pAction;	
	m_wFlags	= pRule->m_wFlags;
	m_uDelay	= pRule->m_uDelay;

	for (UINT uCnt = 0; uCnt < g_uMaxEventParams; uCnt++)
	{
		m_rgkep[uCnt] = pRule->m_rgkep[uCnt];
		SetEventParam(uCnt, pRule->m_rgstrEventParams[uCnt]);
	}

	for (uCnt = 0; uCnt < g_uMaxActionParams; uCnt++)
	{
		m_rgkap[uCnt] = pRule->m_rgkap[uCnt];
		m_rgstrActionParams[uCnt] = pRule->m_rgstrActionParams[uCnt];
	}

	FreeAndNullFormatting(&m_prgdwMsgFormatting);

	m_prgdwMsgFormatting = CopyFormatting(pRule->m_prgdwMsgFormatting);
}

	
/////////////////////////////////////////////////////////////////////////////
// CCRule::CCRule - Destructor
CCRule::~CCRule()
{
	FreeAndNullFormatting(&m_prgdwMsgFormatting);
	FreeAndNullFormatting(&m_prgdwFinalMsgFormatting);

	if (m_pDaemonExt)
		m_pDaemonExt->Release();
}


void CCRule::AddRef()
{
	m_nRefCount++;

	#ifdef DEBUG
		g_nRulesRefCount++;
	#endif
}


void CCRule::Release()
{
	#ifdef DEBUG
		ASSERT(g_nRulesRefCount > 0, "m_nRefCount <= 0 in CCRule::Release");
		g_nRulesRefCount--;
	#endif

	ASSERT(m_nRefCount > 0, "m_nRefCount <= 0 in CCRule::Release");

	if (--m_nRefCount == 0)
		delete this;	
}


void CCRule::SetMsgFormatting(CDWordArray* prgdwMsgFormatting, BOOL bMakeCopy)
{
	FreeAndNullFormatting(&m_prgdwMsgFormatting);

	if (bMakeCopy)
		m_prgdwMsgFormatting = CopyFormatting(prgdwMsgFormatting);
	else
		m_prgdwMsgFormatting = prgdwMsgFormatting;
}


void CCRule::SetEventParam(UINT uIndex, CString& strParam)
{
	m_rgstrEventParams[uIndex] = strParam;
	if (m_pEvent->m_rgpt[uIndex] == ptNickname && m_rgkep[uIndex] == kepMax)
		// need to update the m_prUserMatch member too
		bGetUserMatchFromMask((LPTSTR) (LPCTSTR) m_rgstrEventParams[uIndex], &m_prUserMatch);
}


CString CCRule::StrParamBeginning(CString& strParam)
{
	// We don't use GetLength because it returns the number of bytes not characters.
	if (_tcslen((LPCTSTR) strParam) > g_uMaxShortParamLength)
		return strParam.Left(g_uMaxShortParamLength - 3) + g_szContinuation;
	else
		return strParam;
}


CString CCRule::StrGetEventDisplay()
{
	CString strRet;
	UINT	uCnt;

	ASSERT(m_pEvent, "m_pEvent is NULL in CCRule::StrGetEventDisplay");

	strRet = m_pEvent->m_strShortDesc + g_szBeginParams;

	for (uCnt = 0; uCnt < m_pEvent->m_uParamNum; uCnt++)
	{
		if (uCnt > 0)
			strRet += g_szParamSeparator;
		strRet += g_szBeginParam + StrParamBeginning(m_rgstrEventParams[uCnt]) + g_szEndParam;
	}

	strRet += g_szEndParams;

	return strRet;
}


CString CCRule::StrGetActionDisplay()
{
	CString strRet;
	UINT	uCnt;

	ASSERT(m_pAction, "m_pAction is NULL in CCRule::StrGetActionDisplay");

	strRet = m_pAction->m_strShortDesc + g_szBeginParams;

	for (uCnt = 0; uCnt < m_pAction->m_uParamNum; uCnt++)
	{
		if (uCnt > 0)
			strRet += g_szParamSeparator;
		strRet += g_szBeginParam + StrParamBeginning(m_rgstrActionParams[uCnt]) + g_szEndParam;
	}

	strRet += g_szEndParams;

	return strRet;
}


void CCRule::InitRuleDaemon()
{
	if (m_pEvent && m_pEvent->m_bNeedDaemon)
	{
		enumItemTypes	it = itMax;
		switch (m_pEvent->m_eID)
		{
		case eOnConnect:
		case eOnDisconnect:
			it = itUser;
			break;
		case eOnNewRoom:
			it = itChannel;
			break;
		default:
			ASSERT(FALSE, "Unexpected event ID in CCRule::InitRuleDaemon");
		}
		m_pDaemonExt = new CCDaemonExt(it);
		if (m_pDaemonExt)
		{
			if (!m_pDaemonExt->bAllocNewItemList(2))
			{
				TRACE("Could not allocate m_pDaemonExt's user list in CCRule::InitRuleDaemon\n");
				ASSERT(m_pDaemonExt->m_nRefCount == 1, "m_pDaemonExt->m_nRefCount != 1 in CCRule::InitRuleDaemon");
				m_pDaemonExt->Release();
				m_pDaemonExt = NULL;
			}
		}
	}
	else
		m_pDaemonExt = NULL;
}


INT CCRule::Serialize(LPTSTR szBuff, INT cbBuffLen)
{
	LPTSTR	szTmp;
	UINT	uIndex;
	UINT	rguEventParamLen[g_uMaxEventParams], rguActionParamLen[g_uMaxActionParams];
	LPTSTR	szControlFull = NULL;
	CString rgstrEventParamsTmp[g_uMaxEventParams];
	CString rgstrActionParamsTmp[g_uMaxActionParams];

	ASSERT(szBuff, "szBuff is NULL in CCRule::Serialize");
	ASSERT(m_pDynaRules, "m_pDynaRules is NULL in CCRule::Serialize");
	ASSERT(m_pDynaRules->GetRulesData(), "m_pDynaRules->GetRulesData() is NULL in CCRule::Serialize");

	// 1 byte for rule version
	// 2 bytes for rule length in bytes
	// 2 bytes for rule status
	// 1 byte for rule delay
	// 2 bytes reserved for future
	// 2 bytes for event ID
	// 1 byte for event param number
	// up to 3 bytes for event key params
	// up to 3 strings for the event params
	// 1 byte for number of actions	(always 0x01 in this first version)
	// 2 bytes for action ID
	// 2 bytes for action status (always 0x0000 in this first version)
	// 1 byte for action param number
	// up to 3 bytes for action key params
	// up to 3 strings for the action params
	INT		cbTotal = g_uRuleFixedPrefix + m_pEvent->m_uParamNum + m_pAction->m_uParamNum;

	for (uIndex = 0; uIndex < m_pEvent->m_uParamNum; uIndex++)
	{
		if (m_rgkep[uIndex] == kepMax)
		{
			rgstrEventParamsTmp[uIndex] = m_pDynaRules->GetRulesData()->StrFindAndReplaceKeyParams(m_rgstrEventParams[uIndex], FALSE /*bIncoming*/);
			cbTotal += (rguEventParamLen[uIndex] = rgstrEventParamsTmp[uIndex].GetLength() + 1);  // +1 for terminating NULL
		}
	}

	for (uIndex = 0; uIndex < m_pAction->m_uParamNum; uIndex++)
	{
		if (m_rgkap[uIndex] == kapMax)
		{
			if (RTFParam(m_pAction->m_rgpt[uIndex], m_pAction->m_aID) && m_prgdwMsgFormatting)
			{
				ASSERT(!szControlFull, "szControlFull NOT NULL in CCRule::Serialize");
				szControlFull = SzControlFull((LPCTSTR) m_rgstrActionParams[uIndex], m_prgdwMsgFormatting);
				rgstrActionParamsTmp[uIndex] = m_pDynaRules->GetRulesData()->StrFindAndReplaceKeyParams(szControlFull, FALSE /*bIncoming*/);
			}
			else
				rgstrActionParamsTmp[uIndex] = m_pDynaRules->GetRulesData()->StrFindAndReplaceKeyParams(m_rgstrActionParams[uIndex], FALSE /*bIncoming*/);
			cbTotal += (rguActionParamLen[uIndex] = rgstrActionParamsTmp[uIndex].GetLength() + 1);  // +1 for terminating NULL
		}
	}

	if (cbTotal > cbBuffLen)
	{
		ASSERT(FALSE, "buffer too small in CCRule::Serialize");
		cbTotal = -1;
		goto exit;
	}

	szTmp = szBuff;

	// write versoin number
	*szTmp = (CHAR) g_wVersion;
	szTmp++;

	// write rule length
	*(WORD*) szTmp = cbTotal;
	szTmp += sizeof(WORD);

	// write rule status flags
	*(WORD*) szTmp = m_wFlags & ~g_wStopped;
	szTmp += sizeof(WORD);

	// write rule delay
	*szTmp = (CHAR) m_uDelay;
	szTmp++;

	// write reserved
	*(WORD*) szTmp = 0;		// for future releases
	szTmp += sizeof(WORD);

	// write event ID
	*(WORD*) szTmp = (WORD) m_pEvent->m_eID;
	szTmp += sizeof(WORD);

	// write event param number
	*szTmp = m_pEvent->m_uParamNum;
	szTmp++;

	// write event parameters
	for (uIndex = 0; uIndex < m_pEvent->m_uParamNum; uIndex++)
		if (m_rgkep[uIndex] != kepMax)
		{
			*szTmp = (TCHAR) m_rgkep[uIndex];
			szTmp++;
		}
		else
		{
			*szTmp = (TCHAR) 0xFF;
			szTmp++;
			strncpy(szTmp, rgstrEventParamsTmp[uIndex], rguEventParamLen[uIndex]);
			szTmp += rguEventParamLen[uIndex];
		}

	// write number of actions
	*szTmp = 0x01;	// for now always exactly one action per rule
	szTmp++;

	// write action ID
	*(WORD*) szTmp = (WORD) m_pAction->m_aID;
	szTmp += sizeof(WORD);

	// write action status
	*(WORD*) szTmp = 0x0000;
	szTmp += sizeof(WORD);

	// write action param number
	*szTmp = m_pAction->m_uParamNum;
	szTmp++;

	// write action parameters
	for (uIndex = 0; uIndex < m_pAction->m_uParamNum; uIndex++)
		if (m_rgkap[uIndex] != kapMax)
		{
			*szTmp = (TCHAR) m_rgkap[uIndex];
			szTmp++;
		}
		else
		{
			*szTmp = (TCHAR) 0xFF;
			szTmp++;
			strncpy(szTmp, rgstrActionParamsTmp[uIndex], rguActionParamLen[uIndex]);
			szTmp += rguActionParamLen[uIndex];
		}

	ASSERT(cbTotal == (szTmp-szBuff), "cbTotal != (szTmp-szBuff) in CCRule::Serialize");

exit:
	if (szControlFull)
		delete [] szControlFull;

	return cbTotal;
}


// return +N if successful (N length of rule)
//        -N if wrong version number or format (N length of rule)
INT CCRule::UnSerialize(LPBYTE pbBuff, INT cbBuffLen)
{
	ASSERT(pbBuff, "pbBuff is NULL in CCRule::UnSerialize");
	ASSERT(m_pDynaRules, "m_pDynaRules is NULL in CCRule::UnSerialize");

	CString	strTmp;
	RULEX	ex;
	UINT	uIndex, uEventParamNum, uActionParamNum, uException;
	LPBYTE	pbTmp = pbBuff;
	BYTE	byteVersion;
	INT		cbLeft = cbBuffLen, cbLen, cbRuleLength;

	if (cbLeft < g_uRuleFixedPrefix)
		goto exit;

	// read rule's version
	byteVersion = *pbTmp;
	pbTmp++;
	cbLeft--;

	// read rule's length
	cbRuleLength = *((WORD*) pbTmp);
	pbTmp += sizeof(WORD);
	cbLeft -= sizeof(WORD);

	// if wrong version, return rule length
	if (byteVersion != (BYTE) g_wVersion)
		goto exit;

	// read rule's flags
	m_wFlags = *((WORD*) pbTmp);
	// it's OK if more bits are set, we just ignore them
	m_wFlags &= (g_wActive | g_wNoSubsequent | g_wMatchCase | g_wMatchWord);

	pbTmp += sizeof(WORD);
	cbLeft -= sizeof(WORD);

	// read delay
	m_uDelay = (UCHAR) *pbTmp;
	pbTmp++;
	cbLeft--;

	// read reserved
	pbTmp += sizeof(WORD);	// skip reserved WORD
	cbLeft -= sizeof(WORD);

	// read event ID
	uIndex = *((WORD*) pbTmp);
	if (uIndex >= eMax)
		goto exit;

	m_pEvent = m_pDynaRules->GetRulesData()->GetEvent(uIndex);
	pbTmp += sizeof(WORD);
	cbLeft -= sizeof(WORD);

	ASSERT(m_pEvent, "m_pEvent is NULL in CCRule::UnSerialize");

	// read event param number
	uEventParamNum = (UINT) *pbTmp;
	if (m_pEvent->m_uParamNum != uEventParamNum)
		goto exit;
	pbTmp++;
	cbLeft--;

	// read event params
	for (uIndex = 0; uIndex < m_pEvent->m_uParamNum; uIndex++)
	{
		if (cbLeft < 1)
			goto exit;

		if (*pbTmp != 0xFF)
		{
			if (*pbTmp >= kepMax)
				goto exit;
			m_rgkep[uIndex] = (enumKeyEventParam) *pbTmp;
			SetEventParam(uIndex, m_pDynaRules->GetRulesData()->GetKeyEventParam(m_rgkep[uIndex]));
			pbTmp++;
			cbLeft--;
		}
		else
		{
			m_rgkep[uIndex] = kepMax;
			pbTmp++;
			cbLeft--;
			cbLen = 0;
			while (cbLen < cbLeft && *(pbTmp+cbLen) != 0)
				cbLen++;
			if (*(pbTmp+cbLen) != 0)
				// no NULL terminating string
				goto exit;
			strTmp = m_pDynaRules->GetRulesData()->StrFindAndReplaceKeyParams((LPTSTR) pbTmp, TRUE /*bIncoming*/);
			SetEventParam(uIndex, strTmp);
			pbTmp += cbLen+1;
			cbLeft -= cbLen+1;
		}
	}

	// make sure we won't read too far...
	if (cbLeft < 6)		// 6 = number of actions (1) + action ID (2) + action status (2) + action param number (1)
		goto exit;

	// read the number of actions
	if (0x01 != *pbTmp)	// we only deal with the rules that have one action
		goto exit;
	pbTmp++;
	cbLeft--;
	
	// read action ID
	uIndex = *((WORD*) pbTmp);
	if (uIndex >= aMax)
		goto exit;

	m_pAction = m_pDynaRules->GetRulesData()->GetAction(uIndex);
	pbTmp += sizeof(WORD);
	cbLeft -= sizeof(WORD);

	ASSERT(m_pAction, "m_pAction is NULL in CCRule::UnSerialize");

	// skip the unused action status
	pbTmp += sizeof(WORD);
	cbLeft -= sizeof(WORD);

	// read action param number
	uActionParamNum = (UINT) *pbTmp;
	if (m_pAction->m_uParamNum != uActionParamNum)
		goto exit;
	pbTmp++;
	cbLeft--;

	// read action params
	for (uIndex = 0; uIndex < m_pAction->m_uParamNum; uIndex++)
	{
		if (cbLeft < 1)
			goto exit;

		if (*pbTmp != 0xFF)
		{
			if (*pbTmp >= kapMax)
				goto exit;
			m_rgkap[uIndex] = (enumKeyActionParam) *pbTmp;
			m_rgstrActionParams[uIndex] = m_pDynaRules->GetRulesData()->GetKeyActionParam(m_rgkap[uIndex]);
			pbTmp++;
			cbLeft--;
		}
		else
		{
			m_rgkap[uIndex] = kapMax;
			pbTmp++;
			cbLeft--;
			cbLen = 0;
			while (cbLen < cbLeft && *(pbTmp+cbLen) != 0)
				cbLen++;
			if (*(pbTmp+cbLen) != 0)
				// no NULL terminating string
				goto exit;
			strTmp = m_pDynaRules->GetRulesData()->StrFindAndReplaceKeyParams((LPTSTR) pbTmp, TRUE /*bIncoming*/);
			if (RTFParam(m_pAction->m_rgpt[uIndex], m_pAction->m_aID))
			{
				ASSERT(!m_prgdwMsgFormatting, "m_prgdwMsgFormatting NOT NULL in CCRule::UnSerialize");
				m_prgdwMsgFormatting = new CDWordArray;

				char*	szControlFull = strdup((const char*) strTmp);
				char*	szControlLess = SzControlLess(szControlFull, m_prgdwMsgFormatting);

				if (strlen(szControlLess) > g_uMaxParamLength)
				{
					free(szControlFull);
					goto exit;
				}
	
				m_rgstrActionParams[uIndex] = CString(szControlLess);
				if (0 == m_prgdwMsgFormatting->GetSize())
					FreeAndNullFormatting(&m_prgdwMsgFormatting);
				free(szControlFull);
			}
			else
				m_rgstrActionParams[uIndex] = strTmp;
			pbTmp += cbLen+1;
			cbLeft -= cbLen+1;
		}
	}

	// make sure the delay is valid!
	for (uException = 0; uException < g_uExceptionCount; uException++)
	{
		ex = g_rgex[uException];
		if (ex.ex == etMinDelay && ex.eID == m_pEvent->GetID() && ex.aID == m_pAction->GetID())
		{
			if (ex.dwValue > m_uDelay)
				m_uDelay = ex.dwValue; 
			break;
		}
	}

	if (cbBuffLen - cbLeft == cbRuleLength)
	{
		InitRuleDaemon();
		return cbRuleLength;
	}

exit:
	m_wFlags = 0;
	m_uDelay = 0;
	m_pEvent = NULL;
	m_pAction = NULL;
	return -cbRuleLength;
}


BOOL CCRule::bUnSerialize(LPCTSTR szRule)
{
	ASSERT(szRule, "szRule is NULL in CCRule::bUnSerialize");
	LPTSTR	szToken, szTmp = (LPTSTR) szRule;
	INT		iToken;
	UINT	uParam;

	m_uDelay = 0;	// can't set the delay in the resource for now

	// Get the activation flag
	szToken = GetToken1(szTmp, &szTmp, "|");
	if (!szToken)
		goto exit;

	m_wFlags = atoi(szToken);
	if (m_wFlags & ~(g_wActive | g_wNoSubsequent | g_wMatchCase | g_wMatchWord))
		goto exit;

	// Get the event ID
	szToken = GetToken1(szTmp, &szTmp, "|");
	if (!szToken)
		goto exit;

	iToken = atoi(szToken);
	if (iToken >= eMax)
		goto exit;

	m_pEvent = m_pDynaRules->GetRulesData()->GetEvent(iToken);

	// Get the action ID
	szToken = GetToken1(szTmp, &szTmp, "|");
	if (!szToken)
		goto exit;

	iToken = atoi(szToken);
	if (iToken >= aMax)
		goto exit;

	m_pAction = m_pDynaRules->GetRulesData()->GetAction(iToken);

	if (!m_pEvent || !m_pAction)
		goto exit;

	for (uParam = 0; uParam < m_pEvent->m_uParamNum; uParam++)
	{
		// Get next event param
		szToken = GetToken1(szTmp, &szTmp, "|");
		if (!szToken)
			goto exit;

		if (*szToken == '$')
		{
			// this is a key event param
			iToken = atoi(szToken+1);
			if (iToken >= kepMax)
				goto exit;
			m_rgkep[uParam] = (enumKeyEventParam) iToken;
			SetEventParam(uParam, m_pDynaRules->GetRulesData()->GetKeyEventParam(m_rgkep[uParam]));
		}
		else
		{
			// this is a textual param
			m_rgkep[uParam] = kepMax;
			SetEventParam(uParam, m_pDynaRules->GetRulesData()->StrFindAndReplaceKeyParams(szToken, TRUE /*bIncoming*/));
		}
	}

	for (uParam = 0; uParam < m_pAction->m_uParamNum; uParam++)
	{
		// Get next action param
		szToken = GetToken1(szTmp, &szTmp, "|");
		if (!szToken)
			goto exit;

		if (*szToken == '$')
		{
			// this is a key action param
			iToken = atoi(szToken+1);
			if (iToken >= kapMax)
				goto exit;
			m_rgkap[uParam] = (enumKeyActionParam) iToken;
			m_rgstrActionParams[uParam] = m_pDynaRules->GetRulesData()->GetKeyActionParam(m_rgkap[uParam]);
		}
		else
		{
			// this is a textual param
			m_rgkap[uParam] = kapMax;
			m_rgstrActionParams[uParam] = m_pDynaRules->GetRulesData()->StrFindAndReplaceKeyParams(szToken, TRUE /*bIncoming*/);
		}
	}

	InitRuleDaemon();

	return TRUE;

exit:
	m_wFlags = 0;
	m_uDelay = 0;
	m_pEvent = NULL;
	m_pAction = NULL;
	return FALSE;
}


INT CCRule::iGetHighlightTypeIndex(CString strParam)
{
	CString strType;

	for (INT iHighlights = 0; iHighlights < (NHIGHLIGHTEDFONTS/2); iHighlights++)
	{
		strType.Format(IDS_HIGHLIGHT_TYPE, iHighlights+1);
		if (0 == strType.CompareNoCase(strParam))
			break;;
	}
	if (iHighlights >= (NHIGHLIGHTEDFONTS/2))
		return -1;
	else
		return iHighlights;
}


BOOL CCRule::bValidateRuleEvent(UINT uIndex, CString& strParam, UINT *puErrorIDS)
{
	ASSERT(m_pEvent, "m_pEvent is NULL in CCRule::bValidateRuleEvent");
	ASSERT(m_pAction, "m_pAction is NULL in CCRule::bValidateRuleEvent");

	switch (m_pEvent->m_eID)
	{
		case eOnConnect:
		case eOnDisconnect:
		{
			if (0 == uIndex)
			{
				// Nickname mask checking - it can't match to *!*@*
				PRUSERMATCH	prUserMatch;

				bGetUserMatchFromMask((LPTSTR) (LPCTSTR) strParam, &prUserMatch);

				if (!prUserMatch.cbNickname &&
					!prUserMatch.cbUserName &&
					!prUserMatch.cbIPAddress)
				{
					TRACE("Nickname mask matches *!*@* in CCRule::bValidateRuleEvent\n");
					if (puErrorIDS)
						*puErrorIDS = IDS_ERR_MATCHALL;
					return FALSE;
				}
			}
			break;
		}
	}

	if (puErrorIDS)
		*puErrorIDS = 0;

	return TRUE;
}


BOOL CCRule::bValidateRuleAction(UINT uIndex, CString& strParam, UINT *puErrorIDS)
{
	ASSERT(m_pEvent, "m_pEvent is NULL in CCRule::bValidateRuleAction");
	ASSERT(m_pAction, "m_pAction is NULL in CCRule::bValidateRuleAction");

	switch (m_pAction->m_aID)
	{
		case aSendFileLine:
		case aWhisperFileLine:
		{
			if (2 == uIndex)	// file line range checking
			{
				UINT	uMinLine, uMaxLine;
				LPTSTR	szLineNumber = (LPTSTR) (LPCTSTR) strParam;
				ASSERT(szLineNumber, "szLineNumber is NULL in CCRule::bValidateRuleAction");

				while (*szLineNumber)
				{
					if (!bGetNextRange(&szLineNumber, &uMinLine, &uMaxLine) || !uMinLine || !uMaxLine)
					{
						TRACE("Bad file line interval in CCRule::bValidateRuleAction\n");
						if (puErrorIDS)
							*puErrorIDS = IDS_ERR_FILELINERANGE;
						return FALSE;
					}
				}
			}
			break;
		}
		case aActivateRuleSet:
		{
			if (1 == uIndex)	// activate boolean
			{
				TRACE("Bad boolean value for activation operation in CCRule::bValidateRuleAction\n");
				if (puErrorIDS)
					*puErrorIDS = IDS_ERR_NOBOOLEAN;
				return FALSE;
			}
			break;
		}
		case aHighlightMessage:
		{
			ASSERT(0 == uIndex, "0 != uIndex in CCRule::bValidateRuleAction");
			if (iGetHighlightTypeIndex(strParam) < 0)
			{
				TRACE("Bad highlighting format type in CCRule::bValidateRuleAction\n");
				if (puErrorIDS)
					*puErrorIDS = IDS_ERR_NOHIGHLIGHT;
				return FALSE;
			}
		}
	}

	if (puErrorIDS)
		*puErrorIDS = 0;

	return TRUE;
}


BOOL CCRule::bDaemonNeeded()
{
	ASSERT(m_pEvent, "m_pEvent is NULL in CCRule::bDaemonNeeded");
	ASSERT(m_pAction, "m_pAction is NULL in CCRule::bDaemonNeeded");

	if (bActive() && !bStopped() && m_pEvent->m_bNeedDaemon)
	{
		switch (m_pEvent->m_eID)
		{
		case eOnConnect:
		case eOnDisconnect:
			if (m_rgkep[0] != kepMe)	// first parameter is Nickname, it needs to be != %Me% to start the daemon
				return TRUE;
			break;
		case eOnNewRoom:
			return TRUE;
		default:
			ASSERT(FALSE, "Unexpected event ID in CCRule::bDaemonNeeded");
		}
	}
	return FALSE;
}


BOOL CCRule::bUpdateDaemonExt(BOOL bResetItemLists, enumEvents eID)
{
	// Clean up the potential daemon lists
	if (bDaemonNeeded())
	{
		if (m_pDaemonExt)
		{
			if (bResetItemLists || m_pDaemonExt->m_bResetItemLists)
			{
				m_pDaemonExt->m_bResetItemLists = FALSE;
				bResetItemLists = TRUE;
				m_pDaemonExt->bCleanUpItemLists();
			}
		}
		else
		{
			enumItemTypes	it = itMax;
			switch (eID)
			{
			case eOnConnect:
			case eOnDisconnect:
				it = itUser;
				break;
			case eOnNewRoom:
				it = itChannel;
				break;
			default:
				ASSERT(FALSE, "Unexpected event ID in CCRule::bUpdateDaemonExt");
			}
			m_pDaemonExt = new CCDaemonExt(it);
			bResetItemLists = TRUE;
		}

		if (m_pDaemonExt && bResetItemLists)
			if (!m_pDaemonExt->bAllocNewItemList(2))
			{
				TRACE("Could not allocate m_pDaemonExt's user list in CCRule::bUpdateDaemonExt\n");
				m_pDaemonExt->Release();
				m_pDaemonExt = NULL;
				return FALSE;
			}
	}
	else
		if (m_pDaemonExt)
		{
			m_pDaemonExt->Release();
			m_pDaemonExt = NULL;
		}
	return TRUE;
}


BOOL CCRule::bIsFlooding()
{
	USHORT	uNow = time(NULL) & 0xFFFF;
	USHORT	uInterval = abs(uNow - m_uPeriodStart);

	ASSERT(m_pDynaRules, "m_pDynaRules is NULL in CCRule::bIsFlooding");

	if (uInterval > (USHORT) m_pDynaRules->GetFloodingInterval())
	{
		m_uPeriodStart = uNow;
		m_uOccurrences = 1;
		return FALSE;
	}
	else
		return !(++m_uOccurrences <= m_pDynaRules->GetFloodingOccurrences());
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::CCRuleSet - Constructor
CCRuleSet::CCRuleSet(CCDynaRules* pDynaRules)
{
	m_pDynaRules	= pDynaRules;
	m_wFlags		= 0;
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::CCRuleSet(CCRuleSet*)
CCRuleSet::CCRuleSet(CCRuleSet* pRuleSet, CCDynaRules* pDynaRules)
{
	ASSERT(pRuleSet, "pRuleSet is NULL in CCRuleSet::CCRuleSet");

	CCRule	*pRule, *pRuleCopy;
	INT		iRules, iIndex;

	if (iRules = pRuleSet->m_rgpRules.GetSize())
	{
		for (iIndex = 0; iIndex < iRules; iIndex++)
		{
			pRule = (CCRule*) pRuleSet->m_rgpRules.GetAt(iIndex);
			ASSERT(pRule, "pRule is NULL in CCRuleSet::CCRuleSet");
			if (pRuleCopy = new CCRule(pRule, pDynaRules))
				m_rgpRules.Add((void*) pRuleCopy);
		}
	}

	m_pDynaRules	= pDynaRules;
	m_strSetName	= pRuleSet->m_strSetName;
	m_wFlags		= pRuleSet->m_wFlags;
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::CCRuleSet - Destructor
CCRuleSet::~CCRuleSet()
{
	CleanUpRulesArray();
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::CleanUpRulesArray
void CCRuleSet::CleanUpRulesArray()
{
	CCRule*	pRule;
	INT		iRules, iIndex;

	if (iRules = m_rgpRules.GetSize())
	{
		for (iIndex = 0; iIndex < iRules; iIndex++)
		{
			pRule = (CCRule*) m_rgpRules.GetAt(iIndex);
			ASSERT(pRule, "pRule is NULL in CCRuleSet::CleanUpRulesArray");
			pRule->Desactivate();
			pRule->Release();
		}
		m_rgpRules.RemoveAll();
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::bUpdateRulesDaemonExt
BOOL CCRuleSet::bUpdateRulesDaemonExt(BOOL bResetItemLists)
{
	CCRule*	pRule;
	INT		iRules, iIndex;
	BOOL	bRet = TRUE;

	if (iRules = m_rgpRules.GetSize())
	{
		for (iIndex = 0; iIndex < iRules; iIndex++)
		{
			pRule = (CCRule*) m_rgpRules.GetAt(iIndex);
			ASSERT(pRule, "pRule is NULL in CCRuleSet::bUpdateRulesDaemonExt");
			bRet &= pRule->bUpdateDaemonExt(bResetItemLists, pRule->GetEvent()->GetID());
		}
	}
	return bRet;
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::bAddRule - Adds a rule at the end of the rules array
BOOL CCRuleSet::bAddRule(CCRule* pRule, INT iIndex /* = -1 */)
{
	ASSERT(pRule, "pRule is NULL in CCRuleSet::bAddRule");

	if (iIndex < 0)
		m_rgpRules.Add((void*) pRule);				// Append rule at the end of the array
	else
		m_rgpRules.InsertAt(iIndex, (void*) pRule);	// Insert in position iIndex
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::bRemoveRule - Removes a rule from the rules array
BOOL CCRuleSet::bRemoveRule(CCRule* pRule, INT iIndex /* = -1 */)
{
	INT	iIndexTmp, iRules;

	ASSERT(pRule || iIndex >= 0, "pRule is NULL and iIndex < 0 in CCRuleSet::bRemoveRule");

	if (iIndex < 0)
	{
		iIndexTmp = 0;
		iRules = m_rgpRules.GetSize();

		while (iIndexTmp < iRules && pRule != (CCRule*) m_rgpRules.GetAt(iIndexTmp))
			iIndexTmp++;
	
		if (iIndexTmp > iRules)
			return FALSE;
	}
	else
	{
		iIndexTmp = iIndex;
		ASSERT(iIndexTmp < m_rgpRules.GetSize(), "iIndexTmp >= m_rgpRules.GetSize() in CCRuleSet::bRemoveRule");
		pRule = (CCRule*) m_rgpRules.GetAt(iIndexTmp);
		ASSERT(pRule, "pRule is NULL in CCRuleSet::bRemoveRule");
	}

	pRule->Desactivate();
	pRule->Release();
	m_rgpRules.RemoveAt(iIndexTmp);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::bDuplicateRule - Puts the duplicated rule right after the original one
BOOL CCRuleSet::bDuplicateRule(INT iIndex, CCRule** ppRule)
{
	CCRule	*pRule, *pOriRule;

	ASSERT(iIndex >= 0, "iIndex < 0 in CCRuleSet::bDuplicateRule");
	ASSERT(ppRule, "ppRule is NULL in CCRuleSet::bDuplicateRule");

	*ppRule = NULL;

	pOriRule = (CCRule*) m_rgpRules.GetAt(iIndex);

	ASSERT(pOriRule, "pOriRule is NULL in CCRuleSet::bDuplicateRule");

	if (!(pRule = new CCRule(m_pDynaRules)))
		return FALSE;

	if (bAddRule(pRule, iIndex+1))
	{
		pRule->CopyRule(pOriRule);
		*ppRule = pRule;
		return TRUE;
	}
	else
	{
		pRule->Release();
		return FALSE;
	}
}


BOOL CCRuleSet::bUpRule(CCRule* pRule, INT iIndex /* = -1 */)
{
	INT		iIndexTmp, iRules;
	void	*pCurrent, *pPrevious = NULL;

	ASSERT(pRule || iIndex >= 0, "pRule is NULL and iIndex < 0 in CCRuleSet::bUpRule");

	if (iIndex < 0)
	{
		iIndexTmp = 0;
		iRules = m_rgpRules.GetSize();

		while (iIndexTmp < iRules && pRule != (CCRule*) (pCurrent = m_rgpRules.GetAt(iIndexTmp)))
		{
			pPrevious = pCurrent;
			iIndexTmp++;
		}
		
		if (iIndexTmp > iRules)
			return FALSE;
	}
	else if (iIndex > 0)
	{
		iIndexTmp = iIndex;
		pPrevious = m_rgpRules.GetAt(iIndexTmp-1);
		pCurrent = m_rgpRules.GetAt(iIndexTmp);
	}
	
	if (pPrevious)
	{
		m_rgpRules.SetAt(iIndexTmp-1, pCurrent);
		m_rgpRules.SetAt(iIndexTmp, pPrevious);
	}

	return TRUE;
}


BOOL CCRuleSet::bDownRule(CCRule* pRule, INT iIndex /* = -1 */)
{
	INT		iIndexTmp, iRules;
	void	*pCurrent;

	ASSERT(pRule || iIndex >= 0, "pRule is NULL and iIndex < 0 in CCRuleSet::bDownRule");

	iRules = m_rgpRules.GetSize();

	if (iIndex < 0)
	{
		iIndexTmp = 0;
		while (iIndexTmp < iRules && pRule != (CCRule*) (pCurrent = m_rgpRules.GetAt(iIndexTmp)))
			iIndexTmp++;
		
		if (iIndexTmp > iRules)
			return FALSE;
	}
	else
	{
		iIndexTmp = iIndex;
		pCurrent = m_rgpRules.GetAt(iIndexTmp);
	}
	
	if (iIndexTmp < iRules-1)
	{
		m_rgpRules.SetAt(iIndexTmp, m_rgpRules.GetAt(iIndexTmp+1));
		m_rgpRules.SetAt(iIndexTmp+1, pCurrent);
	}

	return TRUE;
}


BOOL CCRuleSet::bDaemonNeeded()
{
	CCRule*	pRule;
	INT		iRules, iIndex = 0;

	iRules = m_rgpRules.GetSize();

	while (iIndex < iRules)
	{
		pRule = (CCRule*) m_rgpRules.GetAt(iIndex);
		ASSERT(pRule, "pRule is NULL in CCRuleSet::bDaemonNeeded");
		if (pRule->bDaemonNeeded())
			return TRUE;
		else
			iIndex++;
	}
	return FALSE;
}


BOOL CCRuleSet::bSaveToFile(UINT* puError, CWnd* pParentWnd)
{
	CFile				file;
	CFileException		fe;
	CString				strFilter;
	INT					iIndex;
	TCHAR				szFilename[_MAX_PATH];
	OPENFILENAME		ofn;
	AFX_EXCEPTION_LINK	_afxExceptionLink;

	ASSERT(puError, "puError is NULL in CCRuleSet::bSaveToFile");
	ASSERT(pParentWnd, "pParentWnd is NULL in CCRuleSet::bSaveToFile");

	strcpy(szFilename, (LPCTSTR) m_strSetName);
	strFilter.LoadString(IDS_CRS_FILTER);
	iIndex = strFilter.GetLength() - 1;
	while (iIndex >= 0)
	{
		if (strFilter[iIndex] == '|')
			strFilter.SetAt(iIndex, g_chEOS);
		iIndex--;
	}

	*puError = CFileException::none;

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize	= sizeof(OPENFILENAME);
	ofn.hwndOwner	= pParentWnd->m_hWnd;
	ofn.lpstrFilter	= (LPCTSTR) strFilter;
	ofn.lpstrFile	= (LPTSTR) szFilename;
	ofn.nMaxFile	= _MAX_PATH;
	ofn.Flags		= OFN_OVERWRITEPROMPT | OFN_PATHMUSTEXIST | OFN_SHAREAWARE | OFN_HIDEREADONLY;
	ofn.lpstrDefExt	= (LPCTSTR) g_szRuleSetFileExt;

	if (!GetSaveFileName(&ofn))
		// currently ignoring error case where _MAX_PATH characters is too small
		return TRUE;

	TRACE("CCRuleSet::bSaveToFile - Trying to save rule set into file %s\n", szFilename);

	if (!file.Open((LPCTSTR) szFilename, CFile::shareExclusive | CFile::modeCreate | CFile::modeWrite, &fe))
	{
		TRACE("File could not be opened in CCRuleSet::bSaveToFile: %s\n", fe.m_cause);
		*puError = fe.m_cause;
		return FALSE;
	}

	try
	{
		// File was successfully opened for writing
		// First: write header
		DWORD dwHeader = MAKELONG(m_wFlags & g_wActive, g_wVersion);	// low + high
		file.Write((const void*) &dwHeader, sizeof(DWORD));
		dwHeader = 0L;	// reserved for future releases
		file.Write((const void*) &dwHeader, sizeof(DWORD));

		// Second: write set name
		file.Write((const void*) m_strSetName, m_strSetName.GetLength()+1);
		
		// Third: write rules
		CCRule*		pRule;
		INT			cbRule, iIndex, iRules = m_rgpRules.GetSize();
		TCHAR		szBuff[g_uMaxSerializedRule];

		for (iIndex = 0; iIndex < iRules; iIndex++)
		{
			pRule = (CCRule*) m_rgpRules.GetAt(iIndex);
			ASSERT(pRule, "pRule is NULL in CCRuleSet::bSaveToFile");
			
			if ((cbRule = pRule->Serialize(szBuff, g_uMaxSerializedRule)) > 0)
				file.Write((const void*) szBuff, cbRule);

			ASSERT(cbRule, "Serialize failed in CCRuleSet::bSaveToFile");
		}

		file.Flush();
		file.Close();
	}
	catch(CFileException* pfe)
	{
		_afxExceptionLink.m_pException = pfe;
		TRACE("File exception thrown in CCRuleSet::bSaveToFile: %s\n", pfe->m_cause);
		*puError = pfe->m_cause;
		return FALSE;
	}

	return TRUE;
}


BOOL CCRuleSet::bLoadFromFile(UINT* puError, CWnd* pParentWnd)
{
	CFile				file;
	CFileException		fe;
	CString				strFilter;
	INT					iIndex;
	TCHAR				szFilename[_MAX_PATH] = _T("");
	OPENFILENAME		ofn;
	AFX_EXCEPTION_LINK	_afxExceptionLink;
	BOOL				bRulesSkipped = FALSE;

	ASSERT(puError, "puError is NULL in CCRuleSet::bLoadFromFile");
	ASSERT(pParentWnd, "pParentWnd is NULL in CCRuleSet::bLoadFromFile");

	*puError = CFileException::none;

	strFilter.LoadString(IDS_CRS_FILTER);
	iIndex = strFilter.GetLength() - 1;
	while (iIndex >= 0)
	{
		if (strFilter[iIndex] == '|')
			strFilter.SetAt(iIndex, g_chEOS);
		iIndex--;
	}

	ZeroMemory(&ofn, sizeof(OPENFILENAME));

	ofn.lStructSize	= sizeof(OPENFILENAME);
	ofn.hwndOwner	= pParentWnd->m_hWnd;
	ofn.lpstrFilter	= (LPCTSTR) strFilter;
	ofn.lpstrFile	= (LPTSTR) szFilename;
	ofn.nMaxFile	= _MAX_PATH;
	ofn.Flags		= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_SHAREAWARE | OFN_HIDEREADONLY;
	ofn.lpstrDefExt	= (LPCTSTR) g_szRuleSetFileExt;

	if (!GetOpenFileName(&ofn))
		return FALSE;

	TRACE("CCRuleSet::bLoadFromFile - Trying to load rule set from file %s\n", szFilename);

	if (!file.Open((LPCTSTR) szFilename, CFile::shareExclusive | CFile::modeRead, &fe))
	{
		TRACE("File could not be opened in CCRuleSet::bLoadFromFile: %s\n", fe.m_cause);
		*puError = fe.m_cause;
		return FALSE;
	}

	try
	{
		// File was successfully opened for reading
		// First: read header
		DWORD	dwHeader;
		UINT	uRead, uToRead, uJustRead;

		*puError = g_uErrFormat;

		if ((uRead = file.Read((void*) &dwHeader, sizeof(DWORD))) != sizeof(DWORD))
			goto error;

		if (HIWORD(dwHeader) != g_wVersion)
		{
			*puError = g_uErrVersion;
			goto error;
		}

		// g_wActive is the only flag accepted - all others are just ignored
		m_wFlags = LOWORD(dwHeader) & g_wActive;

		// Skip Reserved DWORD - whatever its value is
		if ((uRead = file.Read((void*) &dwHeader, sizeof(DWORD))) != sizeof(DWORD))
			goto error;

		CCRule*		pRule;
		INT			cbRule, cbLen = 0;
		TCHAR		szBuff[g_uMaxSerializedRule+1];
		LPTSTR		szRead;
		BOOL		bEndReached;

		// Second: read set name
		szRead = (LPTSTR) szBuff;

		uRead = file.Read((void*) szRead, g_uMaxSetNameLength+1);
		while (cbLen < uRead && szRead[cbLen] != g_chEOS)
			cbLen++;
		if (szRead[cbLen] != g_chEOS)
			goto error;

		m_strSetName = CString(szRead);
		ASSERT(m_strSetName.GetLength() <= g_uMaxSetNameLength, "m_strSetName.GetLength() > g_uMaxSetNameLength in CCRuleSet::bLoadFromFile");

		uRead -= cbLen+1;
		CopyMemory((PVOID) szBuff, (CONST VOID*) (szBuff+cbLen+1), (DWORD) uRead);
		szRead = szBuff+uRead;
	
		// Third: read rules
		uToRead = g_uMaxSerializedRule - uRead;
		uRead += (uJustRead = file.Read((void*) szRead, uToRead));
		bEndReached = (uJustRead != uToRead);

		while(uRead > 0)
		{
			if (!(pRule = (CCRule*) new CCRule(m_pDynaRules)))
			{
				*puError = g_uErrOOM;
				goto error;
			}

			if ((cbRule = pRule->UnSerialize((LPBYTE) szBuff, uRead)) > 0)
			{			
				bAddRule(pRule);
				uRead -= cbRule;
				CopyMemory((PVOID) szBuff, (CONST VOID*) (szBuff+cbRule), (DWORD) uRead);
				szRead = szBuff+uRead;
			}
			else
			{
				bRulesSkipped = TRUE;
				cbRule = -cbRule;
				pRule->Release();
				// Couldn't read the rule properly - file could be corrupted or rule has higher version number
				// Skip this rule and try to read the next ones...
				while (cbRule > uRead && !bEndReached)
				{
					cbRule -= uRead;
					uRead = file.Read((void*) szBuff, g_uMaxSerializedRule);
					bEndReached = (uRead != g_uMaxSerializedRule);
				}
				if (cbRule <= uRead)
				{
					uRead -= cbRule;
					CopyMemory((PVOID) szBuff, (CONST VOID*) (szBuff+cbRule), (DWORD) uRead);
					szRead = szBuff+uRead;
				}
				else
					goto error;	// coudn't jump cbRule bytes - file is corrupted
			}

			if (!bEndReached)
			{
				uToRead = g_uMaxSerializedRule - uRead;
				uRead += (uJustRead = file.Read((void*) szRead, uToRead));
				bEndReached = (uJustRead != uToRead);
			}
		}

		file.Close();
	}
	catch (CFileException* pfe)
	{ 
		_afxExceptionLink.m_pException = pfe;
		TRACE("File exception thrown in CCRuleSet::bLoadFromFile: %s\n", pfe->m_cause);
		*puError = pfe->m_cause;
		CleanUpRulesArray();
		return FALSE;
	}

	*puError = bRulesSkipped ? g_uErrRulesSkipped : CFileException::none;
	return TRUE;

error:
	CleanUpRulesArray();
	file.Close();
	return FALSE;
}


/////////////////////////////////////////////////////////////////////////////
// CCDynaRules::CCDynaRules - Constructor
CCDynaRules::CCDynaRules()
{
	m_eIDCach				= eMax;
	m_pRulesData			= NULL;
	m_pDelayedRules			= NULL;
	m_pSelectedRuleSet		= NULL;
	m_pfEventKeyParam		= NULL;
	m_pfEventRndParam		= NULL;
	m_pfGetKeyEventParam	= NULL;
	m_pfGetKeyActionParam	= NULL;
	m_pfExecuteAction		= NULL;
	m_pfRuleFailure			= NULL;
	m_pfDaemonQuery			= NULL;
	m_prgdwMsgFormattingCach= NULL;
	m_paApprovedIDsCach		= NULL;
	m_paRejectedIDsCach		= NULL;
	m_bDaemonRunning		= FALSE;

	m_uFloodInterval		= g_uDefRuleFloodInt;
	m_uFloodOccurrences		= g_uDefRuleFloodOcc;

	ResetFlags();
}


/////////////////////////////////////////////////////////////////////////////
// CCDynaRules::CCDynaRules - Destructor
CCDynaRules::~CCDynaRules()
{
	CleanUpRuleSetsArray();

	FreeAndNullFormatting(&m_prgdwMsgFormattingCach);
}


/////////////////////////////////////////////////////////////////////////////
// CCDynaRules::operator=
const CCDynaRules& CCDynaRules::operator=(const CCDynaRules& dynaRules)
{
	CleanUpRuleSetsArray();

	CCRuleSet	*pRuleSet, *pRuleSetCopy;
	INT			iRuleSets, iIndex;

	if (iRuleSets = dynaRules.m_rgpRuleSets.GetSize())
	{
		for (iIndex = 0; iIndex < iRuleSets; iIndex++)
		{
			pRuleSet = (CCRuleSet*) dynaRules.m_rgpRuleSets.GetAt(iIndex);
			ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::operator=");
			if (pRuleSetCopy = new CCRuleSet(pRuleSet, this))
				m_rgpRuleSets.Add((void*) pRuleSetCopy);
			if (dynaRules.m_pSelectedRuleSet == pRuleSet)
				m_pSelectedRuleSet = pRuleSetCopy;
		}
	}

	FreeAndNullFormatting(&m_prgdwMsgFormattingCach);

	m_bDaemonRunning			= dynaRules.m_bDaemonRunning;
	
	m_pfEventKeyParam			= dynaRules.m_pfEventKeyParam;
	m_pfEventRndParam			= dynaRules.m_pfEventRndParam;
	m_pfGetKeyEventParam		= dynaRules.m_pfGetKeyEventParam;
	m_pfGetKeyActionParam		= dynaRules.m_pfGetKeyActionParam;
	m_pfExecuteAction			= dynaRules.m_pfExecuteAction;
	m_pfRuleFailure				= dynaRules.m_pfRuleFailure;
	m_pfDaemonQuery				= dynaRules.m_pfDaemonQuery;
	
	m_pRulesData				= dynaRules.m_pRulesData;
	m_pDelayedRules				= dynaRules.m_pDelayedRules;

	m_eIDCach					= dynaRules.m_eIDCach;
	m_strIdentityCach			= dynaRules.m_strIdentityCach;
	m_strServerCach				= dynaRules.m_strServerCach;
	m_strChannelCach			= dynaRules.m_strChannelCach;
	m_strRecipientsCach			= dynaRules.m_strRecipientsCach;
	m_strCLMessageCach			= dynaRules.m_strCLMessageCach;
	m_strCFMessageCach			= dynaRules.m_strCFMessageCach;
	m_prgdwMsgFormattingCach	= CopyFormatting(dynaRules.m_prgdwMsgFormattingCach);
	m_paApprovedIDsCach			= NULL;
	m_paRejectedIDsCach			= NULL;
	m_wFlags					= dynaRules.m_wFlags;

	m_strCFFinalMessage			= dynaRules.m_strCFFinalMessage;

	m_uFloodInterval			= dynaRules.m_uFloodInterval;
	m_uFloodOccurrences			= dynaRules.m_uFloodOccurrences;

	return *this;
}


void CCDynaRules::SetCachVariables(enumEvents eID, CString& strIdentity, CString& strServer, CString& strChannel)
{
	OutputDebugThreadIdString("CCDynaRules::SetCachVariables - Enter\n");
	m_eIDCach			= eID;
	m_paApprovedIDsCach	= NULL;
	m_paRejectedIDsCach	= NULL;
	m_strIdentityCach	= strIdentity;
	m_strServerCach		= strServer;
	m_strChannelCach	= strChannel;
	m_strRecipientsCach	= "";
	m_strCFMessageCach	= "";
	m_strCLMessageCach	= "";
	FreeAndNullFormatting(&m_prgdwMsgFormattingCach);
}


void CCDynaRules::CleanUpRuleSetsArray()
{
	CCRuleSet*	pRuleSet;
	INT			iRuleSets, iIndex;

	if (iRuleSets = m_rgpRuleSets.GetSize())
	{
		for (iIndex = 0; iIndex < iRuleSets; iIndex++)
		{
			pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iIndex);
			ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::CleanUpRuleSetsArray");
			delete pRuleSet;	// calls CleanUpRulesArray()
		}
		m_rgpRuleSets.RemoveAll();
	}
}


BOOL CCDynaRules::bUpdateRuleSetsDaemonExt(BOOL bResetItemLists)
{
	CCRuleSet*	pRuleSet;
	INT			iRuleSets, iIndex;
	BOOL		bRet = TRUE;

	if (iRuleSets = m_rgpRuleSets.GetSize())
	{
		for (iIndex = 0; iIndex < iRuleSets; iIndex++)
		{
			pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iIndex);
			ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::bUpdateRuleSetsDaemonExt");
			bRet &= pRuleSet->bUpdateRulesDaemonExt(bResetItemLists);
		}
	}
	return bRet;
}


BOOL CCDynaRules::bReplaceKeyEventParams(CString& strEventParam)
{
	UINT			uIndexKey;
	INT				iBegin, iBeginOri;
	CString			strUpperKeyParam, strUpperEventParam, strFrom, strTo = strEventParam;
	enumParamType	pt;

	for (uIndexKey = kepAny; uIndexKey < (UINT) kepMax; uIndexKey++)
	{
		switch ((enumKeyEventParam) uIndexKey)
		{
		case kepMe:
		case kepMyActivatedRoom:
			pt = (enumKeyEventParam) uIndexKey == kepMe ? ptNickname : ptRoomName;
			iBeginOri = 0;
			strFrom = strTo;
			strTo.Empty();
			strUpperEventParam = strFrom;
			strUpperEventParam.MakeUpper();
			strUpperKeyParam = m_pRulesData->GetKeyEventParam((enumKeyEventParam) uIndexKey);
			strUpperKeyParam.MakeUpper();
			while ((iBegin = strUpperEventParam.Find(strUpperKeyParam)) != -1)
			{
				strTo += strFrom.Mid(iBeginOri, iBegin);
				iBeginOri += iBegin + strUpperKeyParam.GetLength();
				strTo += m_pfGetKeyEventParam(pt);
				strUpperEventParam = strUpperEventParam.Mid(iBeginOri);
			}
			strTo += strFrom.Mid(iBeginOri);
		}
	}
	strEventParam = strTo;

	return TRUE;
}


BOOL CCDynaRules::bReplaceKeyActionParams(CCRule* pRule /*, CString& strEventServer, CString& strEventIdentity, CString& strEventChannel, CString& strEventMessage*/)
{
	// fill in m_rgstrActionFinalParams array by replacing action keywords in m_rgstrActionParams
	ASSERT(pRule, "pRule in NULL in CCDynaRules::bReplaceKeyActionParams");
	ASSERT(m_pfGetKeyEventParam, "m_pfGetKeyEventParam is NULL in CCDynaRules::bReplaceKeyActionParams");
	ASSERT(m_pfGetKeyActionParam, "m_pfGetKeyActionParam is NULL in CCDynaRules::bReplaceKeyActionParams");
	ASSERT(m_pRulesData, "m_pRulesData is NULL in CCDynaRules::bReplaceKeyActionParams");

	BOOL	bNeedControlFull;
	UINT	uIndexParam, uIndexKey;
	INT		iBegin, iBeginOri;
	CString	strUpperKeyParam, strUpperActionParam, strFrom, strTo;

	FreeAndNullFormatting(&pRule->m_prgdwFinalMsgFormatting);

	// replace kep and kap values
	for (uIndexParam = 0; uIndexParam < pRule->m_pAction->m_uParamNum; uIndexParam++)
	{
		iBeginOri = 0;
		if (RTFParam(pRule->m_pAction->m_rgpt[uIndexParam], pRule->m_pAction->m_aID) && pRule->m_prgdwMsgFormatting)
		{
			LPTSTR szControlFull = SzControlFull((LPCTSTR) pRule->m_rgstrActionParams[uIndexParam], pRule->m_prgdwMsgFormatting);
			strFrom = CString(szControlFull);
			delete [] szControlFull;
		}
		else
			strFrom = pRule->m_rgstrActionParams[uIndexParam];
		strTo.Empty();
		strUpperActionParam = strFrom;
		strUpperActionParam.MakeUpper();
		strUpperKeyParam = m_pRulesData->GetKeyEventParam(kepMe);
		strUpperKeyParam.MakeUpper();
		while ((iBegin = strUpperActionParam.Find(strUpperKeyParam)) != -1)
		{
			strTo += strFrom.Mid(iBeginOri, iBegin);
			iBeginOri += iBegin + strUpperKeyParam.GetLength();
			strTo += m_pfGetKeyEventParam(ptNickname);
			strUpperActionParam = strUpperActionParam.Mid(iBeginOri);
		}
		strTo += strFrom.Mid(iBeginOri);
	
		for (uIndexKey = 0; uIndexKey < (UINT) kapMax; uIndexKey++)
		{
			if ((enumKeyActionParam) uIndexKey == kapYes || (enumKeyActionParam) uIndexKey == kapNo)
				continue;	// skip Yes and No keywords

			strFrom = strTo;
			bNeedControlFull = (uIndexKey == (UINT) kapEventMessage) && 
							   (pRule->m_pAction->m_rgpt[uIndexParam] == ptMessage) && 
							   (pRule->m_pAction->m_aID != aNotifyDialog);
			if (bNeedControlFull)
			{
				CDWordArray	rgdwMsgFormatting;

				char*	szControlFull = strdup((LPCTSTR) strTo);
				char*	szControlLess = SzControlLess(szControlFull, &rgdwMsgFormatting);

				char*	szReplaced = SzReplaceFormattedString(m_pRulesData->GetKeyActionParam(kapEventMessage), 
															  m_strCLMessageCach, 
															  szControlLess,
															  m_prgdwMsgFormattingCach,
															  &rgdwMsgFormatting,
															  0 /*uFlags*/);	// no match case or match whole word

				strTo = CString(szReplaced);

				delete [] szReplaced;
				free(szControlFull);
				rgdwMsgFormatting.RemoveAll();
			}
			else
			{
				iBeginOri = 0;
				strTo.Empty();
				strUpperActionParam = strFrom;
				strUpperActionParam.MakeUpper();
				strUpperKeyParam = m_pRulesData->GetKeyActionParam((enumKeyActionParam) uIndexKey);
				strUpperKeyParam.MakeUpper();
				while ((iBegin = strUpperActionParam.Find(strUpperKeyParam)) != -1)
				{
					strTo += strFrom.Mid(iBeginOri, iBegin);
					iBeginOri += iBegin + strUpperKeyParam.GetLength();
					strTo += m_pfGetKeyActionParam((enumKeyActionParam) uIndexKey, m_strServerCach, m_strIdentityCach, m_strChannelCach, m_strRecipientsCach, m_strCLMessageCach);
					strUpperActionParam = strUpperActionParam.Mid(iBeginOri);
				}
				strTo += strFrom.Mid(iBeginOri);
			}
		}

		if (RTFParam(pRule->m_pAction->m_rgpt[uIndexParam], pRule->m_pAction->m_aID))
		{
			// need to create control less string and final formatting array
			if (!pRule->m_prgdwFinalMsgFormatting)
				pRule->m_prgdwFinalMsgFormatting = new CDWordArray;

			char*	szControlFull = strdup((LPCTSTR) strTo);
			char*	szControlLess = SzControlLess(szControlFull, pRule->m_prgdwFinalMsgFormatting);

			pRule->m_rgstrActionFinalParams[uIndexParam] = CString(szControlLess);
			free(szControlFull);
		}
		else
			pRule->m_rgstrActionFinalParams[uIndexParam] = strTo;
	}

	return TRUE;
}


BOOL CCDynaRules::bMatchAndApplyRules(enumEvents eID, enumActions* paApprovedIDs, enumActions* paRejectedIDs, CString& strServer, CString& strIdentity, CString& strChannel, CString& strMessage)
{
	CCRuleSet*	pRuleSet;
	CCRule*		pRule = NULL;
	INT			iRule, iRuleSet = -1;

	ASSERT(m_pfRuleFailure, "m_pfRuleFailure is NULL in CCDynaRules::bMatchAndApplyRules");
	ASSERT(m_pfExecuteAction, "m_pfExecuteAction is NULL in CCDynaRules::bMatchAndApplyRules");
	ResetFlags();

	if ((iRule = iGetFirstMatchingRule(&iRuleSet, eID, paApprovedIDs, paRejectedIDs, strServer, strIdentity, strChannel, strMessage, &pRule)) >= 0)
	{
		ASSERT(pRule, "pRule is NULL in CCDynaRules::bMatchAndApplyRules");
		OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDynaRules::bMatchAndApplyRules - Match event: ") + pRule->StrGetEventDisplay() + "\n"));

		if (pRule->bIsFlooding())
		{
			OutputDebugThreadIdString("CCDynaRules::bMatchAndApplyRules - Rule Flooding...\n");
			pRule->SetFlags(pRule->wGetFlags() | g_wStopped);
			pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iRuleSet);
			ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::bMatchAndApplyRules");
			m_pfRuleFailure(pRuleSet, pRule, g_uErrFlooding);
		}
		else
		{
			OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDynaRules::bMatchAndApplyRules - Execute action: ") + pRule->StrGetActionDisplay() + "\n"));
			bReplaceKeyActionParams(pRule /*, strServer, strIdentity, strChannel, strMessage*/);

			if (!pRule->GetDelay())
			{
				// immediate execution
				CCActionContext actCtx;

				actCtx.bInitActionContext(this, pRule);
				m_pfExecuteAction(this, pRule, &actCtx);
			}
			else
			{
				CCActionContext* pActionCtx;

				if (pActionCtx = new CCActionContext)
				{
					pActionCtx->bInitActionContext(this, pRule);
					ASSERT(m_pDelayedRules, "m_pDelayedRules is NULL in CCDynaRules::bMatchAndApplyRules");
					if (!m_pDelayedRules->bAddActionCtx(pActionCtx))
						delete pActionCtx;
				}
			}
		}

		while (!(pRule->m_wFlags & g_wNoSubsequent) && (iRule = iGetNextMatchingRule(&iRuleSet, iRule, &pRule)) >= 0)
		{
			ASSERT(pRule, "pRule is NULL in CCDynaRules::bMatchAndApplyRules");
			OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDynaRules::bMatchAndApplyRules - Match event: ") + pRule->StrGetEventDisplay() + "\n"));

			if (pRule->bIsFlooding())
			{
				OutputDebugThreadIdString("CCDynaRules::bMatchAndApplyRules - Rule Flooding...\n");
				pRule->SetFlags(pRule->wGetFlags() | g_wStopped);
				pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iRuleSet);
				ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::bMatchAndApplyRules");
				m_pfRuleFailure(pRuleSet, pRule, g_uErrFlooding);
			}
			else
			{
				OutputDebugThreadIdString((LPTSTR) (LPCTSTR) (CString("CCDynaRules::bMatchAndApplyRules - Execute action: ") + pRule->StrGetActionDisplay() + "\n"));
				bReplaceKeyActionParams(pRule /*, strServer, strIdentity, strChannel, strMessage*/);

				if (!pRule->GetDelay())
				{
					// immediate execution
					CCActionContext actCtx;

					actCtx.bInitActionContext(this, pRule);
					m_pfExecuteAction(this, pRule, &actCtx);
				}
				else
				{
					CCActionContext* pActionCtx;

					if (pActionCtx = new CCActionContext)
					{
						pActionCtx->bInitActionContext(this, pRule);
						ASSERT(m_pDelayedRules, "m_pDelayedRules is NULL in CCDynaRules::bMatchAndApplyRules");
						if (!m_pDelayedRules->bAddActionCtx(pActionCtx))
							delete pActionCtx;
					}
				}
			}
		}
	}
	return TRUE;
}


INT CCDynaRules::iGetFirstMatchingRule(PINT piRuleSet, enumEvents eID, enumActions* paApprovedIDs, enumActions* paRejectedIDs, CString& strServer, CString& strIdentity, CString& strChannel, CString& strMessage, CCRule** ppRule)
{
	ASSERT(piRuleSet, "piRuleSet is NULL in CCDynaRules::iGetFirstMatchingRule");

	CCRule*		pRule = NULL;
	CCRuleSet*	pRuleSet;
	BOOL		bStop = FALSE;
	INT			iRuleSets, iIndexRuleSet = 0, iRules, iIndexRule;

	OutputDebugThreadIdString("CCDynaRules::iGetFirstMatchingRule - Enter\n");
	m_eIDCach			= eID;
	m_paApprovedIDsCach	= paApprovedIDs;
	m_paRejectedIDsCach	= paRejectedIDs;
	m_strIdentityCach	= strIdentity;
	m_strServerCach		= strServer;
	m_strChannelCach	= strChannel;
	m_strCLMessageCach	= m_strCFMessageCach = strMessage;

	if (!strMessage.IsEmpty())
	{
		if (!m_prgdwMsgFormattingCach)
			m_prgdwMsgFormattingCach = new CDWordArray;

		char*	szControlFull = strdup((LPCTSTR) m_strCFMessageCach);
		char*	szControlLess = SzControlLess(szControlFull, m_prgdwMsgFormattingCach);

		m_strCLMessageCach = CString(szControlLess);
		free(szControlFull);
	}

	if (ppRule)
		*ppRule = NULL;

	iRuleSets = m_rgpRuleSets.GetSize();

	while (iIndexRuleSet < iRuleSets)
	{
		pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iIndexRuleSet);
		ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::iGetFirstMatchingRule");

		if (pRuleSet->bActive())
		{
			iRules = pRuleSet->m_rgpRules.GetSize();
			iIndexRule = 0;

			while (iIndexRule < iRules && !bStop)
			{
				pRule = (CCRule*) pRuleSet->m_rgpRules.GetAt(iIndexRule);
				ASSERT(pRule, "pRule is NULL in CCDynaRules::iGetFirstMatchingRule");
				if (bMatchingRule(pRule))
				{
					if (bRuleFilteredOut(pRule))
					{
						iIndexRule++;
						bStop = (pRule->m_wFlags & g_wNoSubsequent);
					}
					else
					{
						if (ppRule)
							*ppRule = pRule;
						*piRuleSet = iIndexRuleSet;
						return iIndexRule;
					}
				}
				else
					iIndexRule++;
			}
		}
		iIndexRuleSet++;
	}

	*piRuleSet = -1;
	return -1;
}


INT CCDynaRules::iGetNextMatchingRule(PINT piPreviousRuleSet, INT iPreviousRule, CCRule** ppRule)
{
	ASSERT(piPreviousRuleSet, "piPreviousRuleSet is NULL in CCDynaRules::iGetNextMatchingRule");

	CCRule*		pRule;
	CCRuleSet*	pRuleSet;
	BOOL		bStop = FALSE;
	INT			iRules, iRuleSets, iIndexRuleSet = *piPreviousRuleSet, iIndexRule = iPreviousRule+1;

	if (ppRule)
		*ppRule = NULL;

	iRuleSets = m_rgpRuleSets.GetSize();
	ASSERT(iIndexRuleSet < iRuleSets, "iIndexRuleSet >= iRuleSets in CCDynaRules::iGetNextMatchingRule");

	while (iIndexRuleSet < iRuleSets)
	{
		pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iIndexRuleSet);
		ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::iGetNextMatchingRule");

		if (pRuleSet->bActive())
		{
			iRules = pRuleSet->m_rgpRules.GetSize();

			while (iIndexRule < iRules && !bStop)
			{
				pRule = (CCRule*) pRuleSet->m_rgpRules.GetAt(iIndexRule);
				ASSERT(pRule, "pRule is NULL in CCDynaRules::iGetNextMatchingRule");
				if (bMatchingRule(pRule))
				{
					if (bRuleFilteredOut(pRule))
					{
						iIndexRule++;
						bStop = (pRule->m_wFlags & g_wNoSubsequent);
					}
					else
					{
						if (ppRule)
							*ppRule = pRule;
						*piPreviousRuleSet = iIndexRuleSet;
						return iIndexRule;
					}
				}
				else
					iIndexRule++;
			}
		}
		iIndexRule = 0;
		iIndexRuleSet++;
	}
	return -1;
}


BOOL CCDynaRules::bInActionIDs(enumActions* paActions, enumActions aID)
{
	if (!paActions)
		return FALSE;

	UINT uIDs = (UINT) paActions[0];

	for (UINT uIndex = 1; uIndex <= uIDs; uIndex++)
		if (aID == paActions[uIndex])
			return TRUE;

	return FALSE;
}


BOOL CCDynaRules::bRuleFilteredOut(CCRule* pRule)
{
	if (m_paApprovedIDsCach && !bInActionIDs(m_paApprovedIDsCach, pRule->m_pAction->m_aID))
		return TRUE;

	if (m_paRejectedIDsCach && bInActionIDs(m_paRejectedIDsCach, pRule->m_pAction->m_aID))
		return TRUE;

	return FALSE;
}


BOOL CCDynaRules::bMatchingRule(CCRule* pRule)
{
	UINT			uIndex;
	CString*		pstrTmp = NULL;
	CString			strEventParam;
	BOOL			bPass = TRUE;
	PPRUSERMATCH	pPrUserMatch = NULL;

	ASSERT(pRule, "pRule is NULL in CCDynaRules::bMatchingRule");
	ASSERT(pRule->m_pEvent, "pRule->pEvent is NULL in CCDynaRules::bMatchingRule");
	ASSERT(pRule->m_pAction, "pRule->pAction is NULL in CCDynaRules::bMatchingRule");

	if (m_eIDCach != pRule->m_pEvent->m_eID || !pRule->bActive() || pRule->bStopped())
		return FALSE;

	for (uIndex = 0; uIndex < g_rguEventParamNums[pRule->m_pEvent->m_eID] && bPass; uIndex++)
	{
		switch (g_rgptEventParamTypes[pRule->m_pEvent->m_eID][uIndex])
		{
			case ptRoomName:
				pstrTmp = &m_strChannelCach;
				break;
			case ptMessage:
				pstrTmp = &m_strCLMessageCach;
				break;
			case ptNickname:
				pstrTmp = &m_strIdentityCach;
				pPrUserMatch = &(pRule->m_prUserMatch);
				break;
			case ptServerName:
				pstrTmp = &m_strServerCach;
				break;
			default:
				ASSERT(FALSE, "Unexpected parameter type in CCDynaRules::bMatchingRule");
				return FALSE;
		}

		ASSERT(pstrTmp, "pstrTmp is NULL in CCDynaRules::bMatchingRule");

		if (pRule->m_rgkep[uIndex] != kepMax)
		{
			// rule uses a keyword for this parameter
			ASSERT(m_pfEventKeyParam, "m_pfEventKeyParam is NULL in CCDynaRules::bMatchingRule");
			bPass = m_pfEventKeyParam(*pstrTmp, pRule->m_rgkep[uIndex]);
		}
		else
		{
			// rule uses specific textual value for this parameter
			ASSERT(!pRule->m_rgstrEventParams[uIndex].IsEmpty(), "pRule->m_rgstrEventParams[uIndex] is empty in CCDynaRules::bMatchingRule");
			ASSERT(m_pfEventRndParam, "m_pfEventRndParam is NULL in CCDynaRules::bMatchingRule");

			strEventParam = pRule->m_rgstrEventParams[uIndex];
			// some event key params might have to be replaced by their values
			if (ptMessage == g_rgptEventParamTypes[pRule->m_pEvent->m_eID][uIndex])
				bReplaceKeyEventParams(strEventParam);

			bPass = m_pfEventRndParam(*pstrTmp, strEventParam, pPrUserMatch, pRule->m_wFlags, g_rgptEventParamTypes[pRule->m_pEvent->m_eID][uIndex]);
		}
	}

	return bPass;
}


CCRuleSet* CCDynaRules::GetRuleSetFromName(LPCTSTR szSetName)
{
	CCRuleSet*	pRuleSet;
	INT			iRuleSet = m_rgpRuleSets.GetSize() - 1;

	ASSERT(szSetName, "szSetName is NULL in CCDynaRules::GetRuleSetFromName");

	while (iRuleSet >= 0)
	{
		pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iRuleSet);
		ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::GetRuleSetFromName");
		if (0 == pRuleSet->GetName().CompareNoCase(szSetName))
			return pRuleSet;
		iRuleSet--;
	}
	return NULL;
}


BOOL CCDynaRules::bSaveRulesToReg(/*BOOL bToHKCU /*=TRUE*/)
{
	TCHAR		szValueName[g_uMaxSetNameLength];
	TCHAR		szClassName[16];
	FILETIME	ftLastWriteTime;
	DWORD		dwFlags, cbValueName = g_uMaxSetNameLength, cbClassName = 16;
	INT			iRules, iRuleSets, iIndexRule, iIndexRuleSet, cbRule;
	HKEY		hKeySet = NULL, hKey = NULL;
	TCHAR		szBuff[g_uMaxSerializedRule];
	CString		strRulesKey(szRootRegKeyName), strRegName;
	CCRuleSet*	pRuleSet;
	CCRule*		pRule;

	strRulesKey += g_szRuleSetsSubKey;
	//if (RegCreateKeyEx(bToHKCU ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, strRulesKey, 
	//				   0, (LPTSTR) g_szRuleSetsClass, REG_OPTION_NON_VOLATILE,
	//				   KEY_ALL_ACCESS, NULL, &hKeySet, NULL) == ERROR_SUCCESS)
	if (RegCreateKeyEx(HKEY_CURRENT_USER, strRulesKey, 
					   0, (LPTSTR) g_szRuleSetsClass, REG_OPTION_NON_VOLATILE,
					   KEY_ALL_ACCESS, NULL, &hKeySet, NULL) == ERROR_SUCCESS)
	{
		// if (bToHKCU)
			while (ERROR_SUCCESS == RegEnumKeyEx(hKeySet, 0L, szValueName, &cbValueName, NULL, szClassName, &cbClassName, &ftLastWriteTime))
			{
				RegDeleteKey(hKeySet, szValueName);
				cbValueName = g_uMaxSetNameLength;
				cbClassName = 16;
			}

		iRuleSets = m_rgpRuleSets.GetSize();
		for (iIndexRuleSet = 0; iIndexRuleSet < iRuleSets; iIndexRuleSet++)
		{
			pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iIndexRuleSet);
			ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::bSaveRulesToReg");
			ASSERT(!pRuleSet->m_strSetName.IsEmpty(), "pRuleSet->m_strSetName.IsEmpty() in CCDynaRules::bSaveRulesToReg");

			// if ((bToHKCU && !(pRuleSet->wGetFlags() & g_wGeneral)) || (!bToHKCU && (pRuleSet->wGetFlags() & g_wGeneral)))
				if (RegCreateKeyEx(hKeySet, pRuleSet->m_strSetName,
								   0, (LPTSTR) g_szRulesClass, REG_OPTION_NON_VOLATILE,
								   KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS)
				{
					ASSERT(hKey, "hKey is NULL in CCDynaRules::bSaveRulesToReg");
					iRules = pRuleSet->m_rgpRules.GetSize();

					dwFlags = MAKELONG(pRuleSet->m_wFlags & g_wActive, g_wVersion);	// low + high
					if (ERROR_SUCCESS == RegSetValueEx(hKey, g_szRuleSetFlags, 0, REG_DWORD, (CONST BYTE*) &dwFlags, sizeof(DWORD)))
						for (iIndexRule = 0; iIndexRule < iRules; iIndexRule++)
						{
							strRegName.Format("%d", iIndexRule);
							pRule = (CCRule*) pRuleSet->m_rgpRules.GetAt(iIndexRule);
							ASSERT(pRule, "pRule is NULL in CCDynaRules::bSaveRulesToReg");
							if ((cbRule = pRule->Serialize(szBuff, g_uMaxSerializedRule)) > 0)
								RegSetValueEx(hKey, strRegName, 0, REG_BINARY, (const unsigned char*)(LPCTSTR) szBuff, cbRule);
						}
					RegCloseKey(hKey);
				}
		}

		RegCloseKey(hKeySet);
		return TRUE;
	}
	else
	{
		ASSERT(FALSE, "Couldn't open regkey for rule sets in CCDynaRules::bSaveRulesToReg");
		return FALSE;
	}
}


BOOL CCDynaRules::bLoadRulesFromReg(/*BOOL bFromHKCU /*=TRUE*/)
{
	FILETIME	ftLastWriteTime;
	HKEY		hKeySet = NULL, hKey = NULL;
	TCHAR		szValueName[g_uMaxSetNameLength];
	TCHAR		szClassName[16];
	DWORD		dwFlags;
	BYTE		pbData[g_uMaxSerializedRule];
	CString		strRulesKey(szRootRegKeyName), strRegName, strSetName;
	CCRule*		pRule = NULL;
	CCRuleSet*	pRuleSet = NULL;
	DWORD		dwIndex, dwIndexSet = 0L, dwType, cbValueName = g_uMaxSetNameLength, cbClassName = 16, cbData = g_uMaxSerializedRule;
	BOOL		bRet = FALSE;

	ASSERT(m_pRulesData, "m_pRulesData is NULL in CCDynaRules::bLoadRulesFromReg");

	strRulesKey += g_szRuleSetsSubKey;
	//if (ERROR_SUCCESS == RegOpenKeyEx(bFromHKCU ? HKEY_CURRENT_USER : HKEY_LOCAL_MACHINE, strRulesKey, 
	//								  0, KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE, &hKeySet))
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, strRulesKey, 
									  0, KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE, &hKeySet))
	{
		BOOL b = m_pRulesData->bInitAlloc();
		ASSERT(b, "bInitAlloc failed in CCDynaRules::bLoadRulesFromReg");
		b = m_pRulesData->bLoadStrings();
		ASSERT(b, "bLoadStrings failed in CCDynaRules::bLoadRulesFromReg");

		while (ERROR_SUCCESS == RegEnumKeyEx(hKeySet, dwIndexSet, szValueName, &cbValueName, NULL, szClassName, &cbClassName, &ftLastWriteTime))
		{
			if (ERROR_SUCCESS == RegOpenKeyEx(hKeySet, szValueName, 0, KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE, &hKey))
			{
				if (!GetRuleSetFromName((LPCTSTR) szValueName))
				{
					ASSERT(!pRuleSet, "pRuleSet is NOT NULL in CCDynaRules::bLoadRulesFromReg");
					pRuleSet = (CCRuleSet*) new CCRuleSet(this);
					if (!pRuleSet)
						goto exit;
					pRuleSet->m_strSetName = CString(szValueName);
					dwIndex = 0;
					cbValueName = g_uMaxSetNameLength;
					while (ERROR_SUCCESS == RegEnumValue(hKey, dwIndex, szValueName, &cbValueName, NULL, &dwType, pbData, &cbData))
					{
						if (dwType == REG_DWORD && 0 == _tcscmp(g_szRuleSetFlags, szValueName))
						{
							dwFlags = *((DWORD*) pbData);
							if (HIWORD(dwFlags) != g_wVersion)
							{
								delete pRuleSet;
								pRuleSet = NULL;
								break;
							}
							else
							{
								// it's OK if LOWORD(dwFlags) has more bits set than g_wActive, we just ignore them
								pRuleSet->m_wFlags = LOWORD(dwFlags) & g_wActive;
								//if (!bFromHKCU)
								//	pRuleSet->m_wFlags |= g_wGeneral;	// rule sets read from HKLM are general
							}
						}
						else
							if (dwType == REG_BINARY)
							{
								if (!pRule)
								{
									pRule = (CCRule*) new CCRule(this);
									if (!pRule)
										goto exit;
								}
								if (pRule->UnSerialize(pbData, cbData) > 0)
								{
									pRuleSet->bAddRule(pRule);
									pRule = NULL;
								}
							}
						dwIndex++;
						cbValueName = g_uMaxSetNameLength;
						cbData = g_uMaxSerializedRule;
					}
					if (pRuleSet)
					{
						bAddRuleSet(pRuleSet);
						pRuleSet = NULL;
					}
				}
			}
			if (hKey)
			{
				RegCloseKey(hKey);
				hKey = NULL;
			}
			dwIndexSet++;
			cbValueName = g_uMaxSetNameLength;
			cbClassName = 16;
		}

		bRet = TRUE;
		goto exit;
	}
	return TRUE;

exit:
	if (hKey)
		RegCloseKey(hKey);
	if (hKeySet)
		RegCloseKey(hKeySet);
	if (pRule)
		pRule->Release();
	if (pRuleSet)
		delete pRuleSet;
	return bRet;
}


BOOL CCDynaRules::bLoadRulesFromResource()
{
	LPTSTR		szToken, szTmp;
	WORD		wFlags;
	BOOL		bRet;
	CString		strRuleSet, strRule;
	CCRuleSet	*pRuleSet;
	CCRule		*pRule;

	bRet = m_pRulesData->bInitAlloc();
	ASSERT(bRet, "bInitAlloc failed in CCDynaRules::bLoadRulesFromResource");
	bRet = m_pRulesData->bLoadStrings();
	ASSERT(bRet, "bLoadStrings failed in CCDynaRules::bLoadRulesFromResource");

	for (UINT uIDS = IDS_SAMPLES_RULESET; uIDS <= IDS_GENERAL_RULESET; uIDS++)
	{
		strRuleSet.LoadString(uIDS);

		// Get the rule set activation flag
		szTmp = (LPTSTR) (LPCTSTR) strRuleSet;
		if (!(szToken = GetToken1(szTmp, &szTmp, "|")))
		{
			ASSERT(FALSE, "Unexpected rule set format in CCDynaRules::bLoadRulesFromResource");
			return FALSE;
		}

		wFlags = atoi(szToken);
		if (wFlags & ~g_wActive)
		{
			ASSERT(FALSE, "Unexpected rule set format in CCDynaRules::bLoadRulesFromResource");
			return FALSE;
		}

		// skip |
		szTmp++;

		if (GetRuleSetFromName(szTmp))
			continue;	// that ruleset already exists

		pRuleSet = (CCRuleSet*) new CCRuleSet(this);
		if (!pRuleSet)
			return FALSE;
						
		pRuleSet->m_strSetName = szTmp;
		pRuleSet->m_wFlags = wFlags;

		// pRuleSet->m_wFlags = g_wActive | g_wGeneral;	// rule sets read from resources are active & general

		if (IDS_SAMPLES_RULESET == uIDS)
			for (UINT uRule = 0; uRule < 7; uRule++)		// we have 7 rules in the resources
			{
				if (!strRule.LoadString(IDS_SAMPLES_RULE1+uRule))
					continue;

				pRule = (CCRule*) new CCRule(this);
				if (!pRule)
					goto exit;

				if (!pRule->bUnSerialize(strRule) || !pRuleSet->bAddRule(pRule))
					pRule->Release();
			}

		bRet &= bAddRuleSet(pRuleSet);
	}

	ASSERT(bRet, "CCDynaRules::bLoadRulesFromResource failed");
	return bRet; 

exit:
	ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::bLoadRulesFromResource");
	ASSERT(!pRule, "pRule is NOT NULL in CCDynaRules::bLoadRulesFromResource");
	delete pRuleSet;
	return FALSE;
}


BOOL CCDynaRules::bReplaceMessage(CCRule* pRule)
{
	CString strCLReplaceWhat;
	CString strCLReplaceBy;	// might want to add a rgdwReplaceByFormatting
	UINT	uFlags = 0;	

	ASSERT(pRule, "pRule is NULL in CCDynaRules::bReplaceMessage");
	ASSERT(pRule->m_pEvent, "pRule->m_pEvent is NULL in CCDynaRules::bReplaceMessage");
	ASSERT(pRule->m_pAction, "pRule->m_pAction is NULL in CCDynaRules::bReplaceMessage");

	for (UINT uIndex = 0; uIndex < pRule->m_pEvent->m_uParamNum; uIndex++)
		if (pRule->m_pEvent->m_rgpt[uIndex] == ptMessage)
			break;
	ASSERT(uIndex < pRule->m_pEvent->m_uParamNum, "uIndex >= pRule->m_pEvent->m_uParamNum in CCDynaRules::bReplaceMessage");

	if (pRule->m_rgkep[uIndex] == kepMax)
		strCLReplaceWhat = pRule->m_rgstrEventParams[uIndex];
	else
	{
		ASSERT(pRule->m_rgkep[uIndex] == kepAny, "pRule->m_rgkep[uIndex] != kepAny in CCDynaRules::bReplaceMessage");
		strCLReplaceWhat = m_strCLMessageCach;
	}

	for (uIndex = 0; uIndex < pRule->m_pAction->m_uParamNum; uIndex++)
		if (pRule->m_pAction->m_rgpt[uIndex] == ptMessage)
			break;

	ASSERT(uIndex < pRule->m_pAction->m_uParamNum, "uIndex >= pRule->m_pAction->m_uParamNum in CCDynaRules::bReplaceMessage");

	strCLReplaceBy = pRule->m_rgstrActionFinalParams[uIndex];

	if (pRule->m_wFlags & g_wMatchCase)
		uFlags = FR_MATCHCASE;
	if (pRule->m_wFlags & g_wMatchWord)
		uFlags += FR_WHOLEWORD;

	char* szReplaced = SzReplaceFormattedString(strCLReplaceWhat, 
												strCLReplaceBy, 
												m_strCLMessageCach, 
												pRule->m_prgdwFinalMsgFormatting /*prgdwReplaceByFormatting*/, 
												m_prgdwMsgFormattingCach /*prgdwReplaceInFormatting*/,
												uFlags);

	m_strCFFinalMessage = CString(szReplaced);
	delete [] szReplaced;

	AddFlag(g_wReplace);
	return TRUE;
}


BOOL CCDynaRules::bStartRulesDaemon(UINT uRulesDaemonElapse, BOOL bForceReset)
{
	if (m_bDaemonRunning && !bForceReset)
		return TRUE;

	ASSERT(GetFrame(), "GetFrame() return NULL in CCDynaRules::bStartRulesDaemon");

	bStopRulesDaemon();

	if (GetFrame())
		m_bDaemonRunning = (g_uRulesDaemonTimer == GetFrame()->SetTimer(g_uRulesDaemonTimer, 1000 * uRulesDaemonElapse, NULL));

	ASSERT(m_bDaemonRunning, "m_bDaemonRunning is FALSE in CCDynaRules::bStartRulesDaemon()");

	return m_bDaemonRunning;
}


BOOL CCDynaRules::bStopRulesDaemon()
{
	if (!m_bDaemonRunning)
		return TRUE;

	m_bDaemonRunning = FALSE;

	if (GetFrame())
		return GetFrame()->KillTimer(g_uRulesDaemonTimer);

	return TRUE;
}


BOOL CCDynaRules::bDaemonNeeded()
{
	CCRuleSet*	pRuleSet;
	INT			iRuleSets, iIndex = 0;

	iRuleSets = m_rgpRuleSets.GetSize();

	while (iIndex < iRuleSets)
	{
		pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iIndex);
		ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::bDaemonNeeded");
		if (pRuleSet->bDaemonNeeded())
			return TRUE;
		else
			iIndex++;
	}
	return FALSE;
}


void CCDynaRules::OnRulesDaemonTimer()
{
	OutputDebugThreadIdString("CCDynaRules::OnRulesDaemonTimer - Enter\n");

	CCRuleSet*	pRuleSet;
	CCRule*		pRule;
	INT			iRuleSets, iRules, iIndexRuleSet = 0, iIndexRule;
	BOOL		bPass = TRUE;

	// Switch to the long period
	ASSERT(m_bDaemonRunning, "m_bDaemonRunning is FALSE in CCDynaRules::OnRulesDaemonTimer");
	bStartRulesDaemon(g_uRulesDaemonLongElapse, TRUE);

	iRuleSets = m_rgpRuleSets.GetSize();

	while (iIndexRuleSet < iRuleSets)
	{
		pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iIndexRuleSet);
		ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::OnRulesDaemonTimer");

		if (pRuleSet->bActive())
		{
			iRules = pRuleSet->m_rgpRules.GetSize();
			iIndexRule = 0;

			while (iIndexRule < iRules)
			{
				pRule = (CCRule*) pRuleSet->m_rgpRules.GetAt(iIndexRule);
				ASSERT(pRule, "pRule is NULL in CCDynaRules::OnRulesDaemonTimer");
				if (pRule->bActive() && !pRule->bStopped() && pRule->bDaemonNeeded() && pRule->m_pDaemonExt)
				{
					if (pRule->bIsFlooding())
					{
						OutputDebugThreadIdString("CCDynaRules::OnRulesDaemonTimer - Rule Flooding...\n");
						pRule->SetFlags(pRule->wGetFlags() | g_wStopped);
						m_pfRuleFailure(pRuleSet, pRule, g_uErrFlooding);
					}
					else
					{
						if (pRule->m_pDaemonExt->GetIT() == itUser)
						{
							// eID == eOnConnect || eID == eOnDisconnect
							ASSERT(m_pfGetKeyEventParam, "m_pfGetKeyEventParam is NULL in CCDynaRules::OnRulesDaemonTimer");
							CString strCurrentServer = m_pfGetKeyEventParam(ptServerName);
							if (pRule->m_rgkep[1] != kepMax)	// checking if server parameter matches with current server
							{
								// rule uses a keyword for server parameter
								ASSERT(m_pfEventKeyParam, "m_pfEventKeyParam is NULL in CCDynaRules::OnRulesDaemonTimer");
								bPass = m_pfEventKeyParam(strCurrentServer, pRule->m_rgkep[1]);
							}
							else
							{
								// rule uses specific textual value for the server name parameter
								ASSERT(!pRule->m_rgstrEventParams[1].IsEmpty(), "pRule->m_rgstrEventParams[uIndex] is empty in CCDynaRules::OnRulesDaemonTimer");
								ASSERT(m_pfEventRndParam, "m_pfEventRndParam is NULL in CCDynaRules::OnRulesDaemonTimer");
								bPass = m_pfEventRndParam(strCurrentServer, pRule->m_rgstrEventParams[1], NULL, pRule->m_wFlags, ptServerName);
							}
						}

						ASSERT(m_pfDaemonQuery, "m_pfDaemonQuery is NULL in CCDynaRules::OnRulesDaemonTimer");
						if (bPass)
						{
							BOOL bRet = m_pfDaemonQuery(pRule);
							ASSERT(bRet, "m_pfDaemonQuery call failed in CCDynaRules::OnRulesDaemonTimer");
						}
					}
				}
				iIndexRule++;
			}
		}
		iIndexRuleSet++;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CCDynaRules::bAddRuleSet - Adds a rule set at the end of the rule sets array
BOOL CCDynaRules::bAddRuleSet(CCRuleSet* pRuleSet, INT iIndex /* = -1 */)
{
	ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::bAddRuleSet");

	if (iIndex < 0)
		m_rgpRuleSets.Add((void*) pRuleSet);				// Append rule set at the end of the array
	else
		m_rgpRuleSets.InsertAt(iIndex, (void*) pRuleSet);	// Insert in position iIndex
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCRuleSet::bRemoveRuleSet - Removes a rule set from the rule sets array
BOOL CCDynaRules::bRemoveRuleSet(CCRuleSet* pRuleSet, INT iIndex /* = -1 */)
{
	INT	iIndexTmp, iRuleSets; // , iIndexRule, iRules;
	// CCRule*	pRule;

	ASSERT(pRuleSet || iIndex >= 0, "pRuleSet is NULL and iIndex < 0 in CCDynaRules::bRemoveRuleSet");

	if (iIndex < 0)
	{
		iIndexTmp = 0;
		iRuleSets = m_rgpRuleSets.GetSize();

		while (iIndexTmp < iRuleSets && pRuleSet != (CCRuleSet*) m_rgpRuleSets.GetAt(iIndexTmp))
			iIndexTmp++;
	
		if (iIndexTmp > iRuleSets)
			return FALSE;
	}
	else
	{
		iIndexTmp = iIndex;
		ASSERT(iIndexTmp < m_rgpRuleSets.GetSize(), "iIndexTmp >= m_rgpRuleSets.GetSize() in CCDynaRules::bRemoveRuleSet");
		pRuleSet = (CCRuleSet*) m_rgpRuleSets.GetAt(iIndexTmp);
		ASSERT(pRuleSet, "pRuleSet is NULL in CCDynaRules::bRemoveRuleSet");
	}

	//iRules = pRuleSet->m_rgpRules.GetSize();

	//for (iIndexRule = 0; iIndexRule < iRules; iIndexRule++)
	//{
	//	pRule = (CCRule*) pRuleSet->m_rgpRules.GetAt(iIndex);
	//	ASSERT(pRule, "pRule is NULL in CCDynaRules::bRemoveRuleSet");
	//	pRule->Desactivate();
	//	pRule->Release();
	//}

	delete pRuleSet;	// Calls CleanUpRulesArray
	m_rgpRuleSets.RemoveAt(iIndexTmp);

	return TRUE;
}


BOOL CCDynaRules::bUpRuleSet(CCRuleSet* pRuleSet, INT iIndex /* = -1 */)
{
	INT		iIndexTmp, iRuleSets;
	void	*pCurrent, *pPrevious = NULL;

	ASSERT(pRuleSet || iIndex >= 0, "pRuleSet is NULL and iIndex < 0 in CCDynaRules::bUpRuleSet");

	if (iIndex < 0)
	{
		iIndexTmp = 0;
		iRuleSets = m_rgpRuleSets.GetSize();

		while (iIndexTmp < iRuleSets && pRuleSet != (CCRuleSet*) (pCurrent = m_rgpRuleSets.GetAt(iIndexTmp)))
		{
			pPrevious = pCurrent;
			iIndexTmp++;
		}
		
		if (iIndexTmp > iRuleSets)
			return FALSE;
	}
	else if (iIndex > 0)
	{
		iIndexTmp = iIndex;
		pPrevious = m_rgpRuleSets.GetAt(iIndexTmp-1);
		pCurrent = m_rgpRuleSets.GetAt(iIndexTmp);
	}
	
	if (pPrevious)
	{
		m_rgpRuleSets.SetAt(iIndexTmp-1, pCurrent);
		m_rgpRuleSets.SetAt(iIndexTmp, pPrevious);
	}

	return TRUE;
}


BOOL CCDynaRules::bDownRuleSet(CCRuleSet* pRuleSet, INT iIndex /* = -1 */)
{
	INT		iIndexTmp, iRuleSets;
	void	*pCurrent;

	ASSERT(pRuleSet || iIndex >= 0, "pRuleSet is NULL and iIndex < 0 in CCDynaRules::bDownRuleSet");

	iRuleSets = m_rgpRuleSets.GetSize();

	if (iIndex < 0)
	{
		iIndexTmp = 0;
		while (iIndexTmp < iRuleSets && pRuleSet != (CCRuleSet*) (pCurrent = m_rgpRuleSets.GetAt(iIndexTmp)))
			iIndexTmp++;
		
		if (iIndexTmp > iRuleSets)
			return FALSE;
	}
	else
	{
		iIndexTmp = iIndex;
		pCurrent = m_rgpRuleSets.GetAt(iIndexTmp);
	}
	
	if (iIndexTmp < iRuleSets-1)
	{
		m_rgpRuleSets.SetAt(iIndexTmp, m_rgpRuleSets.GetAt(iIndexTmp+1));
		m_rgpRuleSets.SetAt(iIndexTmp+1, pCurrent);
	}

	return TRUE;
}


CCActionContext::CCActionContext()
{
	m_eID = eMax;
	m_aID = aMax;
	m_prgdwFinalMsgFormatting = NULL;
	for (UINT uIndex = 0; uIndex < g_uMaxActionParams; uIndex++)
		m_rgkap[uIndex] = kapMax;
}


CCActionContext::~CCActionContext() 
{
	FreeAndNullFormatting(&m_prgdwFinalMsgFormatting);
}


BOOL CCActionContext::bInitActionContext(CCDynaRules* pDynaRules, CCRule* pRule)
{
	ASSERT(pDynaRules, "pDynaRules is NULL in CCActionContext::bInitActionContext");
	ASSERT(pRule, "pRule is NULL in CCActionContext::bInitActionContext");
	ASSERT(pRule->GetAction(), "pRule->GetAction() is NULL in CCActionContext::bInitActionContext");

	UINT uIndex;

	m_uDelay = pRule->GetDelay();

	// CCRule members
	m_eID = pRule->GetEvent()->GetID();
	m_aID = pRule->GetAction()->GetID();

	for (uIndex = 0; uIndex < pRule->GetAction()->GetParamNum(); uIndex++)
	{
		m_rgkap[uIndex] = pRule->GetActionKeyParam(uIndex);
		m_rgstrActionFinalParams[uIndex] = pRule->GetFinalActionParam(uIndex);
	}

	m_prgdwFinalMsgFormatting = CopyFormatting(pRule->GetFinalMsgFormatting());

	// CCDynaRules members
	m_strIdentityCach = pDynaRules->GetCachedIdentity();
	m_strChannelCach = pDynaRules->GetCachedChannel();
	return TRUE;
}


CCDelayedRules::CCDelayedRules()
{
	m_pfExecuteAction = NULL;
	m_bTimerRunning = FALSE;
}


CCDelayedRules::~CCDelayedRules()
{
	FreeRemoveAll();
}


BOOL CCDelayedRules::bAddActionCtx(CCActionContext* pActionCtx)
{
	ASSERT(pActionCtx, "pActionCtx is NULL in CCDelayedRules::bAddActionCtx");

	if (m_plActionCtx.AddHead((PVOID) pActionCtx))
	{
		bStartTimer();
		return TRUE;
	}
	else
	{
		ASSERT(FALSE, "CCDelayedRules::bAddActionCtx failed.");
		return FALSE;
	}
}


BOOL CCDelayedRules::bExecuteActions()
{
	CCActionContext*	pActionCtx;
	POSITION			pos, prevPos;

	for (pos = m_plActionCtx.GetHeadPosition(); pos; )
    {
		prevPos = pos;
		pActionCtx = (CCActionContext*) m_plActionCtx.GetNext(pos);
		ASSERT(pActionCtx, "pActionCtx is NULL in CCDelayedRules::bExecuteActions");
		ASSERT(pActionCtx->GetDelay() > 0, "pActionCtx->GetDelay() == 0 in CCDelayedRules::bExecuteActions");
		if (!pActionCtx->GetDecrementedDelay())
		{
			ASSERT(prevPos, "prevPos is NULL in CCDelayedRules::bExecuteActions");
			m_plActionCtx.RemoveAt(prevPos);	// removing cell first because the action could be blocking like a DisplayNotification
			// and we only want to execute the action once!
			ASSERT(m_pfExecuteAction, "m_pfExecuteAction is NULL in CCDelayedRules::bExecuteActions");
			// it's time to execute this action!
			m_pfExecuteAction(NULL /*pDynaRules*/, NULL /*pRule*/, pActionCtx);
			// remove this cell from the list - the job is done
			delete pActionCtx;
		}
	}

	if (m_plActionCtx.GetCount() == 0)
		bStopTimer();

	return TRUE;
}


BOOL CCDelayedRules::bStartTimer()
{
	if (m_bTimerRunning)
		return TRUE;

	ASSERT(GetFrame(), "GetFrame() return NULL in CCDelayedRules::bStartTimer");

	if (GetFrame())
		m_bTimerRunning = (g_uDelayedRulesTimer == GetFrame()->SetTimer(g_uDelayedRulesTimer, 1000 * g_uDelayedRulesElapse, NULL));

	ASSERT(m_bTimerRunning, "m_bTimerRunning is FALSE in CCDelayedRules::bStartTimer()");

	return m_bTimerRunning;
}


BOOL CCDelayedRules::bStopTimer()
{
	if (!m_bTimerRunning)
		return TRUE;

	m_bTimerRunning = FALSE;

	if (GetFrame())
		return GetFrame()->KillTimer(g_uDelayedRulesTimer);

	return TRUE;
}


void CCDelayedRules::FreeRemoveAll()
{
	CCActionContext*	pActionCtx;
	POSITION			pos;

	for (pos = m_plActionCtx.GetHeadPosition(); pos; )
    {
		pActionCtx = (CCActionContext*) m_plActionCtx.GetNext(pos);
		ASSERT(pActionCtx, "pActionCtx is NULL in CCDelayedRules::FreeRemoveAll");
		delete pActionCtx;
	}
	
	m_plActionCtx.RemoveAll();
	bStopTimer();
}
