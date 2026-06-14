/*
	*****************************************************************
	*																*
	*	Module 			: CChan.cpp, MICChat Channel Class object	*
	*																*
	*	Author 			: RegisB, 12/17/96							*
	*																*
	*	Current Owner	: RegisB, ex-cchan.cpp from RamuM			*
	*																*
	*	Changes			:											*
	*																*
	*	To Dos			: UniCode Support							*
	*																*
	*****************************************************************
*/

#include "StdAfx.H"
#include "CChan.H"
#include "CCSock.H"
#include "IrcMsg.H"

// for ASSERT and FAIL
SZTHISFILE


CChatChannel::CChatChannel(void)
{
	OutputDebugThreadIdString("CChatChannel::CChatChannel - Enter\n");

	m_hrLastError				= NOERROR;

	m_pccsock					= NULL;

	m_chs						= chsClosed;

	ZeroMemory(&m_prChannel, sizeof(PRCHANNEL));
	m_prChannel.pChannel		= (PVOID) this;

	// Any Channel Associated info should be initialized
	ResetChannelInfo();

	// This will make sure that event empty topics are declared as known
	bSetChannelTopic(g_szEmpty);
}


CChatChannel::~CChatChannel(void)
{
	OutputDebugThreadIdString("CChatChannel::~CChatChannel - Enter\n");

	BOOL bRet;

	if (m_pccsock && m_pccsock->GetPQueryList())
	{
		// Remove potential query cells in m_pccsock->m_listQuery
		// that might still point to this m_prChannel
		PPRQUERY			pPrQuery = NULL;
		enumAssociatedType	at = atChannel;
		while (m_pccsock->GetPQueryList()->bFindCellFromData(1L,
															 g_nSearchByAssociatedType,
															 (PVOID) &at,
															 g_nSearchByQueryType,
															 (PVOID) &m_prChannel,
															 (PVOID*) &pPrQuery,
															 NULL))
		{
			ASSERT(pPrQuery, "pPrQuery is NULL in CChatChannel::~CChatChannel");
			ASSERT(pPrQuery->pvData == (PVOID) &m_prChannel, "pPrQuery->pvData != &m_prChannel in CChatChannel::~CChatChannel");
			bRet = m_pccsock->GetPQueryList()->bRemoveCell((PVOID) pPrQuery);
			ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatChannel::~CChatChannel");
		}
	}

	bRet = bEmptyMemberTable();
	ASSERT(bRet, "bEmptyMemberTable() failed in CChatChannel::~CChatChannel");

	FreePrChannelContent(&m_prChannel);

	OutputDebugThreadIdString("CChatChannel::~CChatChannel - Leave\n");
}


void CChatChannel::ResetChannelInfo(void)
{
	OutputDebugThreadIdString("CChatChannel::ResetChannelInfo - Enter\n");
	// Make sure this is the same info being reset.
	m_bIrcXChannel		= FALSE;
	m_bStartedMemList	= FALSE;
	m_bGotMemList		= FALSE;
	m_bGotChannelModes	= FALSE;
	m_bExitingChannel	= FALSE;

	if (m_prChannel.szKeyword)
	{
		delete [] m_prChannel.szKeyword;
		m_prChannel.szKeyword = NULL;
	}
	if (m_prChannel.szHostKey)
	{
		delete [] m_prChannel.szHostKey;
		m_prChannel.szHostKey = NULL;
	}
	if (m_prChannel.szOwnerKey)
	{
		delete [] m_prChannel.szOwnerKey;
		m_prChannel.szOwnerKey = NULL;
	}
	if (m_prChannel.szAccount)
	{
		delete [] m_prChannel.szAccount;
		m_prChannel.szAccount = NULL;
	}
	if (m_prChannel.szClientData)
	{
		delete [] m_prChannel.szClientData;
		m_prChannel.szClientData = NULL;
	}
	if (m_prChannel.szClientGuid)
	{
		delete [] m_prChannel.szClientGuid;
		m_prChannel.szClientGuid = NULL;
	}
	if (m_prChannel.szServicePath)
	{
		delete [] m_prChannel.szServicePath;
		m_prChannel.szServicePath = NULL;
	}
	if (m_prChannel.szOnJoin)
	{
		delete [] m_prChannel.szOnJoin;
		m_prChannel.szOnJoin = NULL;
	}
	if (m_prChannel.szOnPart)
	{
		delete [] m_prChannel.szOnPart;
		m_prChannel.szOnPart = NULL;
	}
	if (m_prChannel.szLanguage)
	{
		delete [] m_prChannel.szLanguage;
		m_prChannel.szLanguage = NULL;
	}
	if (m_prChannel.szSubject)
	{
		delete [] m_prChannel.szSubject;
		m_prChannel.szSubject = NULL;
	}

	if (m_prChannel.psaBannedList)
	{
		SafeArrayDestroy(m_prChannel.psaBannedList);
		m_prChannel.psaBannedList = NULL;
	}
	m_prChannel.lBannedListCount = 0L;

	if (m_prChannel.psaCloneList)
	{
		SafeArrayDestroy(m_prChannel.psaCloneList);
		m_prChannel.psaCloneList = NULL;
	}
	m_prChannel.lCloneListCount = 0L;

	m_prChannel.lModes			= cmNone;
	m_prChannel.lCreationTime	= 0L;
	m_prChannel.lMaxMemberCount	= -1L;	// MaxMemberCount initially unknown
	m_prChannel.lOID			= -1L;
	m_prChannel.lLag			= -1L;

	m_prChannel.bMicOnly		= FALSE;
}


BYTE CChatChannel::GetCharSet(void)
{
	return m_pccsock ? m_pccsock->GetCharSet() : 0; 
}


BOOL CChatChannel::bCheckOpen(void)
{
	switch (m_chs)
	{
		case chsClosed:
		case chsOpening:
			m_hrLastError = CC_E_NOTOPEN;
			return FALSE;

		case chsOpen:
			return TRUE;

		default:
			ASSERT(FALSE, "Unexpected m_chs in CChatChannel::bCheckOpen");
			m_hrLastError = E_FAIL;
			return FALSE;
	}
}


BOOL CChatChannel::bEmptyMemberTable(void)
{
	OutputDebugThreadIdString("CChatChannel::bEmptyMemberTable - Enter\n");

	BOOL bRet = m_hashMember.bRemoveAll();
	ASSERT(bRet, "bRemoveAll() failed in CChatChannel::bEmptyMemberTable");
	
	return bRet;
}


BOOL CChatChannel::bGetMemberFromNickname(LPCTSTR szNickname, PPRMEMBER *ppPrMember)
{
//	OutputDebugThreadIdString("CChatChannel::bGetMemberFromNickname - Enter\n");
	PVOID	pvContent = NULL;

	BOOL bRet = m_hashMember.bGetDataFromKey((CHAR*) szNickname, NULL, &pvContent);

	if (ppPrMember)
		*ppPrMember = (PPRMEMBER) pvContent;

	return bRet;
}

  
BOOL CChatChannel::bQueryChannelProperty(BOOL bSyncAccess, LPCWSTR wszChannelPropertyName, CChatItems **ppCItems)
{
	PPRQUERY	pPrQuery = NULL;
	BOOL		bIsPropLocal = FALSE;
	BYTE		*pb;
	INT			cb;
	LONG		lOldProgressCount, *plProgressCount = NULL;

	OutputDebugThreadIdString("CChatChannel::bQueryChannelProperty - Enter\n");

	ASSERT(!bSyncAccess || ppCItems, "bSyncAccess && !ppCItems in CChatChannel::bQueryChannelProperty");
	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bQueryChannelProperty");

	if (ppCItems)
		*ppCItems = NULL;

	if (!wszChannelPropertyName)
	{
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Make sure that the property name is known
	enumChannelPropertyName cpnProp = (enumChannelPropertyName) NGetPropertyIndex((SHORT) atChannel, wszChannelPropertyName);

	if (cpnProp < 0)
	{
		// this property is not a channel prop
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Check if we are in the channel or not
	if (!bCheckOpen())
		return FALSE;

	switch (cpnProp)
	{
		case cpnBannedList:
        case cpnHostKey:
        case cpnKeyword:
        case cpnMaxMemberCount:
        case cpnModes:
        case cpnOwnerKey:
		case cpnRating:
			bIsPropLocal = FALSE;
			break;

		case cpnCloneList:
		case cpnName:
			bIsPropLocal = TRUE;
			break;

		case cpnTopic:
        case cpnMemberCount:
			bIsPropLocal = m_bGotMemList;
			break;

		case cpnAccount:
			bIsPropLocal = (m_prChannel.szAccount != NULL);
			break;
		case cpnClientGuid:
			bIsPropLocal = (m_prChannel.szClientGuid != NULL);
			break;
		case cpnClientData:
			bIsPropLocal = (m_prChannel.szClientData != NULL);
			break;
		case cpnLanguage:
			bIsPropLocal = (m_prChannel.szLanguage != NULL);
			break;
		case cpnOnJoin:
			bIsPropLocal = (m_prChannel.szOnJoin != NULL);
			break;
		case cpnOnPart:
			bIsPropLocal = (m_prChannel.szOnPart != NULL);
			break;
		case cpnServicePath:
			bIsPropLocal = (m_prChannel.szServicePath != NULL);
			break;
		case cpnSubject:
			bIsPropLocal = (m_prChannel.szSubject != NULL);
			break;
		case cpnLag:
			bIsPropLocal = (m_prChannel.lLag != -1L);
			break;
		case cpnCreationTime:
			bIsPropLocal = (m_prChannel.lCreationTime != 0L);
			break;
        case cpnObjectId:
			bIsPropLocal = (m_prChannel.lOID != -1L);
			break;

		default:
			// this property is not a channel prop
			m_hrLastError = CC_E_PROPNOTREADABLE;
			return FALSE;
	}

	if (bSyncAccess)
	{
		// Create a CChatItems object via a virtual function that in fact creates a new CItemsObject object
		if (!m_pccsock->bCreateChatItems(ppCItems))
		{
			m_hrLastError = m_pccsock->HrGetLastError();
			return FALSE;
		}

		ASSERT(*ppCItems, "*ppCItems is NULL in CChatChannel::bQueryChannelProperty");

		// Make sure that we expose the latest MemberCount value
		m_prChannel.lMemberCount = LGetMemberCount();
	}

	if (!bIsPropLocal || !bSyncAccess)
	{
		if (pPrQuery = new PRQUERY)
			ZeroMemory(pPrQuery, sizeof(PRQUERY));
		if (!pPrQuery || !bCopyStr(&(pPrQuery->szChannelName), m_prChannel.szName))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto error;
		}
		pPrQuery->at = atChannel;
		pPrQuery->bSyncAccess = bSyncAccess;
		pPrQuery->uHeadReplyCode = 0;
		pPrQuery->pvData = (PVOID) &m_prChannel;
	}

	if (bIsPropLocal)
	{
		if (bSyncAccess)
		{
			ASSERT(!pPrQuery, "pPrQuery is NOT NULL in CChatChannel::bQueryChannelProperty");
			if (!m_pccsock->bFillChatItemsFromChannel(*ppCItems, &m_prChannel))
			{
				m_hrLastError = m_pccsock->HrGetLastError();
				goto error;
			}
		}
		else
		{
			ASSERT(pPrQuery, "pPrQuery is NULL in CChatChannel::bQueryChannelProperty");
			return SUCCEEDED(m_hrLastError = m_pccsock->HrPostEvent(WM_U_CHANNELPROP, (WPARAM) pPrQuery, (LPARAM) FALSE));
		}
	}
	else
	{
		if (!m_pccsock->GetPQueryList()->bInsertCell((LONG) m_pccsock->GetPQueryList()->DwGetCellCount()+1, (PVOID) pPrQuery))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto error;
		}

		// Send the request to the server
		switch (cpnProp)
		{
			case cpnAccount:
			case cpnClientGuid:
			case cpnClientData:
			case cpnCreationTime:
			case cpnHostKey:
			case cpnLag:
			case cpnLanguage:
			case cpnObjectId:
			case cpnOnJoin:
			case cpnOnPart:
			case cpnOwnerKey:
			case cpnRating:
			case cpnServicePath:
			case cpnSubject:
				if (!m_bIrcXChannel)
				{
					m_hrLastError = CC_E_NOTIRCX;
					break;
				}
				pPrQuery->uHeadReplyCode = RPL_PROPLIST;
				if (SUCCEEDED(m_hrLastError = HrMakeGetPropMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) g_rgszIrcxChannelPropertyNames[cpnProp])))
					m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
				if (SUCCEEDED(m_hrLastError))
					plProgressCount = m_pccsock->PLPropCount();
				break;

			case cpnBannedList:
				if (m_prChannel.psaBannedList)
					SafeArrayDestroy(m_prChannel.psaBannedList);
				m_prChannel.lBannedListCount = 0L;
				m_prChannel.psaBannedList = SafeArrayCreateVector(VT_VARIANT, 0L, g_nMaxBannedList);
				if (!m_prChannel.psaBannedList)
				{
					m_hrLastError = E_OUTOFMEMORY;
					break;
				}
				pPrQuery->uHeadReplyCode = RPL_BANLIST;
				if (SUCCEEDED(m_hrLastError = HrMakeGetBannedListMsg(&pb, &cb, (CHAR*) m_prChannel.szName)))
					m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
				if (SUCCEEDED(m_hrLastError))
					plProgressCount = m_pccsock->PLBannedListCount();
				break;

			case cpnKeyword:
			case cpnMaxMemberCount:
			case cpnModes:
				pPrQuery->uHeadReplyCode = RPL_CHANNELMODEIS;
				if (SUCCEEDED(m_hrLastError = HrMakeGetModeMsg(&pb, &cb, (CHAR*) m_prChannel.szName)))
					m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
				if (SUCCEEDED(m_hrLastError))
					plProgressCount = m_pccsock->PLModeIsCount();
				break;

			case cpnTopic:
			case cpnMemberCount:																  // We don't want to do a LISTX here
				pPrQuery->uHeadReplyCode = RPL_LISTSTART;
				if (SUCCEEDED(m_hrLastError = HrMakeListMsg(&pb, &cb, (CHAR*) m_prChannel.szName, FALSE)))
					m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
				if (SUCCEEDED(m_hrLastError))
					plProgressCount = m_pccsock->PLListCount();
				break;

			default:
				ASSERT(FALSE, "Unexpected requested channel property in CChatChannel::bQueryChannelProperty");
				m_hrLastError = E_FAIL;
		}
		
		if (FAILED(m_hrLastError))
			goto error;

		lOldProgressCount = *plProgressCount;
		(*plProgressCount)++;

		if (bSyncAccess)
		{
			// wait for the property to come!
			if (!m_pccsock->bWaitForProperty(atChannel, (SHORT) cpnProp, pPrQuery))
			{
				m_hrLastError = m_pccsock->HrGetLastError();
				goto error;
			}

			if (!m_pccsock->bFillChatItemsFromChannel(*ppCItems, &m_prChannel))
			{
				m_hrLastError = m_pccsock->HrGetLastError();
				goto error;
			}

			BOOL bRet = m_pccsock->GetPQueryList()->bRemoveCell((PVOID) pPrQuery);
			ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatChannel::bQueryChannelProperty");
		}
	}
	return TRUE;

error:
	// Remove query cell from list
	if (pPrQuery)
		if (m_hrLastError == CC_E_TIMEOUT)
		{
			// We know wait for the answer asynchronously
			ASSERT(pPrQuery->bSyncAccess, "pPrQuery->bSyncAccess is FALSE in CChatChannel::bQueryChannelProperty");
			pPrQuery->bSyncAccess = FALSE;
		}
		else
		{
			if (!m_pccsock->GetPQueryList()->bRemoveCell((PVOID) pPrQuery))
				delete pPrQuery;
			if (plProgressCount)
				*plProgressCount = lOldProgressCount;
		}

	if (ppCItems && *ppCItems)
	{
		m_pccsock->ReleaseChatItems(*ppCItems);
		*ppCItems = NULL;
	}

	return FALSE;
}


BOOL CChatChannel::bChangeChannelProperty(LPCWSTR wszChannelPropertyName, VARIANT *pvChannelProperty)
{
	LPTSTR	szValue = NULL;
	LONG	lValue = 0L;
	BYTE	*pb;
	INT		cb;

	OutputDebugThreadIdString("CChatChannel::bChangeChannelProperty - Enter\n");

	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bChangeChannelProperty");
	ASSERT(pvChannelProperty, "pvChannelProperty is NULL in CChatChannel::bChangeChannelProperty");

	// Check if we are in the channel or not
	if (!bCheckOpen())
		return FALSE;

	if (!wszChannelPropertyName)
	{
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Make sure that the property name is known
	enumChannelPropertyName cpnProp = (enumChannelPropertyName) NGetPropertyIndex((SHORT) atChannel, wszChannelPropertyName);

	if (cpnProp < 0)
	{
		// this property is not a channel prop
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	if (pvChannelProperty->vt == (VT_BYREF+VT_VARIANT))
		pvChannelProperty = pvChannelProperty->pvarVal;

	if (!pvChannelProperty || (pvChannelProperty->vt != VT_BSTR && pvChannelProperty->vt != VT_I2 && pvChannelProperty->vt != VT_I4))
		// the new prop variant can only be a BSTR or SHORT or LONG
		return E_INVALIDARG;

	USES_CONVERSION;

	m_hrLastError = NOERROR;

	switch (pvChannelProperty->vt)
	{
		case VT_BSTR:
			if (pvChannelProperty->bstrVal)
			{
				LPTSTR	szAnsiValue = W2T(pvChannelProperty->bstrVal);

				// This might be a topic that needs to be converted to UTF8
				if (cpnTopic == cpnProp)	// REGISB: cpnSubject too?
				{
					if (g_chExtChnPfx == m_prChannel.szName[0] && !m_prChannel.bMicOnly)
					{
						if (!bConvertWideStringToUTF8(pvChannelProperty->bstrVal, 0, &szValue, NULL, FALSE /*bNickname*/, FALSE /*bChannelName*/, FALSE /*bPostProcess*/))
							m_hrLastError = E_OUTOFMEMORY;
					}
					else
					{
						BOOL	bFreeConverted;
						LPTSTR	szTmp;

						// Might have to convert from ShiftJis to Jis, etc...
						if (bConvertString(FALSE, GetCharSet(), szAnsiValue, 0, &szTmp, NULL, &bFreeConverted))
						{
							if (bFreeConverted)
								szValue = szTmp;
							else
								bCopyStr(&szValue, szTmp);
						}
						else
							m_hrLastError = E_OUTOFMEMORY;
					}
				}
				else
				{
					if (!bCopyStr(&szValue, szAnsiValue))
						m_hrLastError = E_OUTOFMEMORY;
				}
			}
			else
				bCopyStr(&szValue, g_szEmpty);
			break;

		case VT_I2:
			lValue = (LONG) pvChannelProperty->iVal;
			break;

		case VT_I4:
			lValue = pvChannelProperty->lVal;
			break;

		default:
			ASSERT(FALSE, "Unexpected variant type for new property in CChatChannel::bChangeChannelProperty");
			m_hrLastError = E_FAIL;
	}

	if (FAILED(m_hrLastError))
		return FALSE;

	// Send the request to the server
	switch (cpnProp)
	{
		case cpnName:
        case cpnMemberCount:
		case cpnBannedList:
		case cpnCloneList:
		case cpnCreationTime:
		case cpnObjectId:
			m_hrLastError = CC_E_PROPREADONLY;
			break;

		case cpnAccount:
		case cpnClientData:
		case cpnClientGuid:
		case cpnHostKey:
		case cpnLanguage:
		case cpnOwnerKey:
		case cpnOnJoin:
		case cpnOnPart:
		case cpnRating:
		case cpnServicePath:
		case cpnSubject:
		{
			BOOL	bFree = FALSE;
			LPTSTR	szEncodedValue = szValue;

			if (!m_bIrcXChannel)
			{
				m_hrLastError = CC_E_NOTIRCX;
				break;
			}
			if (szValue)
			{
				if (cpnProp == cpnOnJoin || cpnProp == cpnOnPart)
					// Low Level Encoding job to do...
					bLowLevelQuoting(g_chLLQuoteIRCX, FALSE /*bTreatAsByteArray*/, szValue, &szEncodedValue, &bFree, TRUE /*bRemoveCarriageReturns*/);

				if (SUCCEEDED(m_hrLastError = HrMakeSetPropMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) g_rgszIrcxChannelPropertyNames[cpnProp], szEncodedValue)))
					m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);

				if (bFree)
					delete [] szEncodedValue;
			}
			else
				m_hrLastError = E_INVALIDARG;
			break;
		}

		case cpnLag:
		{
			if (!m_bIrcXChannel)
			{
				m_hrLastError = CC_E_NOTIRCX;
				break;
			}
			if (szValue)
				m_hrLastError = E_INVALIDARG;
			else
			{
				if (lValue < 0L)
					m_hrLastError = E_INVALIDARG;
				else
				{
					CHAR szVal[32];
					wsprintf(szVal, "%ld", lValue);

					if (SUCCEEDED(m_hrLastError = HrMakeSetPropMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) g_rgszIrcxChannelPropertyNames[cpnProp], szVal)))
						m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
				}
			}
			break;
		}

		case cpnMaxMemberCount:
		{
			if (szValue)
				m_hrLastError = E_INVALIDARG;
			else
			{
				if (lValue < 0L)
					m_hrLastError = E_INVALIDARG;
				else
					if (SUCCEEDED(m_hrLastError = HrMakeChannelMaxMemberCountMsg(&pb, &cb, (CHAR*) m_prChannel.szName, lValue)))
						m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
			}
			break;
		}

		case cpnModes:
		{
			if (szValue)
				m_hrLastError = E_INVALIDARG;
			else
			{
				if (!bIsChannelModesValid(lValue))
					m_hrLastError = CC_E_CHANNELBADMODES;
				else
				{
					if (SUCCEEDED(m_hrLastError = HrMapToIRCAndMakeChannelModeMsg(&pb, &cb, (CHAR*) m_prChannel.szName, m_prChannel.lModes, lValue)) && (S_FALSE != m_hrLastError))
						m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
				}
			}
			break;
		}

		case cpnTopic:
		{
			if (szValue)
			{
				if (SUCCEEDED(m_hrLastError = HrMakeTopicMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) szValue)))
					m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
			}
			else
				m_hrLastError = E_INVALIDARG;
			break;
		}

		case cpnKeyword:
		{
			if (szValue)
			{																														  // Set	// Reset
				if (SUCCEEDED(m_hrLastError = HrMakeSetKeywordMsg(&pb, &cb, *szValue, (CHAR*) m_prChannel.szName, (CHAR*) (*szValue ? szValue : m_prChannel.szKeyword))))
					m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
			}
			else
				m_hrLastError = E_INVALIDARG;
			break;
		}

		default:
			m_hrLastError = CC_E_PROPNOTWRITEABLE;
	}

	if (szValue)
		delete [] szValue;

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatChannel::bChangeMemberProperty(LPCWSTR wszMemberPropertyName, VARIANT *pvMemberProperty, LPCTSTR szMemberNickname)
{
	PPRMEMBER	pPrMember = NULL;
//	LPTSTR		szValue = NULL;
	LONG		lValue = 0L;
	BYTE		*pb;
	INT			cb;

	//REGISB: This function can be simplified if no string property can be changed
	OutputDebugThreadIdString("CChatChannel::bChangeMemberProperty - Enter\n");

	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bChangeMemberProperty");
	ASSERT(pvMemberProperty, "pvMemberProperty is NULL in CChatChannel::bChangeMemberProperty");

	// Check if we are in the channel or not
	if (!bCheckOpen())
		return FALSE;

	// Get the member with that nickname
	if (!bGetMemberFromNickname(szMemberNickname, &pPrMember))
	{
		m_hrLastError = CC_E_NOSUCHMEMBER;
		return FALSE;
	}

	ASSERT(pPrMember, "pPrMember is NULL in CChatChannel::bChangeMemberProperty");
	ASSERT(0 == lstrcmpi(szMemberNickname, pPrMember->szNickname), "Unexpected nickname in CChatChannel::bChangeMemberProperty");

	if (!wszMemberPropertyName)
	{
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Make sure that the property name is known
	enumMemberPropertyName mpnProp = (enumMemberPropertyName) NGetPropertyIndex((SHORT) atMember, wszMemberPropertyName);

	if (mpnProp < 0)
	{
		// this property is not a member prop
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	if (pvMemberProperty->vt == (VT_BYREF+VT_VARIANT))
		pvMemberProperty = pvMemberProperty->pvarVal;

	//if (!pvMemberProperty || (pvMemberProperty->vt != VT_BSTR && pvMemberProperty->vt != VT_I2 && pvMemberProperty->vt != VT_I4))
	//	// the new prop variant can only be a BSTR or SHORT or LONG
	//	return E_INVALIDARG;

	if (!pvMemberProperty || (pvMemberProperty->vt != VT_I2 && pvMemberProperty->vt != VT_I4))
		// the new prop variant can only be a SHORT or LONG
		return E_INVALIDARG;

	m_hrLastError = NOERROR;
	switch (pvMemberProperty->vt)
	{
		//case VT_BSTR:
		//	if (pvMemberProperty->bstrVal)
		//	{
		//		MAKE_ANSIPTR_FROMWIDE(szAnsiValue, (LPWSTR) pvMemberProperty->bstrVal);
		//		if (!szAnsiValue ||
		//			!bCopyStr(&szValue, szAnsiValue))
		//			m_hrLastError = E_OUTOFMEMORY;
		//	}
		//	else
		//		bCopyStr(&szValue, g_szEmpty);
		//	break;

		case VT_I2:
			lValue = (LONG) pvMemberProperty->iVal;
			break;

		case VT_I4:
			lValue = pvMemberProperty->lVal;
			break;

		default:
			ASSERT(FALSE, "Unexpected variant type for new property in CChatChannel::bChangeMemberProperty");
			m_hrLastError = E_FAIL;
	}

	if (FAILED(m_hrLastError))
		return FALSE;

	// Send the request to the server
	switch (mpnProp)
	{
		case pnMemberNickname:
		case pnMemberObjectId:
		case pnMemberUserName:
		case pnMemberIPAddress:
		case pnMemberIdentity:
			m_hrLastError = CC_E_PROPREADONLY;
			break;

		case pnMemberModes:
		{
			//if (szValue)
			//	m_hrLastError = E_INVALIDARG;
			//else
			//{
				if (!bIsMemberModesValid(lValue))
					m_hrLastError = CC_E_MEMBERBADMODES;
				else
				{
					if (NOERROR == (m_hrLastError = HrMapToIRCAndMakeMemberModeMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) szMemberNickname, pPrMember->lModes, lValue & (mmOwner|mmNotOwner))))
						m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
					if (FAILED(m_hrLastError))
						break;

					if (NOERROR == (m_hrLastError = HrMapToIRCAndMakeMemberModeMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) szMemberNickname, pPrMember->lModes, lValue & (mmHost|mmNotHost))))
						m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
					if (FAILED(m_hrLastError))
						break;

					if (NOERROR == (m_hrLastError = HrMapToIRCAndMakeMemberModeMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) szMemberNickname, pPrMember->lModes, lValue & (mmVoice|mmNoVoice))))
						m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
					if (FAILED(m_hrLastError))
						break;

					// Take care of the local bits
					LONG lOldModes = pPrMember->lModes;
					if (lValue & mmClientIgnored)
					{
						// Make sure that the user is not trying to ignore a host, owner or sysop
						if ((pPrMember->lModes & mmHost) || (pPrMember->lModes & mmOwner))
						{
							// Member is a Host or Owner
							m_hrLastError = CC_E_CANTIGNORE;
							break;
						}
						else
						{
							// Is Member is Sysop or Admin?
							PRUSER prUser;

							ZeroMemory(&prUser, sizeof(PRUSER));

							if (!bCopyStr(&(prUser.szNickname), pPrMember->szNickname))
							{
								m_hrLastError = E_OUTOFMEMORY;
								break;
							}

							if (m_pccsock->bGetSyncUserHost(&prUser) &&
								((prUser.lModes & umSysop) || (prUser.lModes & umAdmin)))
							{
								// Member is a Sysop or Admin
								m_hrLastError = CC_E_CANTIGNORE;
								break;
							}
						}

						pPrMember->lModes |= mmClientIgnored;
						pPrMember->lModes &= ~mmNotClientIgnored;
					}
					if (lValue & mmNotClientIgnored)
					{
						pPrMember->lModes |= mmNotClientIgnored;
						pPrMember->lModes &= ~mmClientIgnored;
					}
					ASSERT(bIsMemberModesValid(pPrMember->lModes), "Invalid member mode in CChatChannel::bChangeMemberProperty");
					if (lOldModes != pPrMember->lModes)
					{
						VARIANT vOldMemberProperty, vNewMemberProperty;
						vOldMemberProperty.vt = vNewMemberProperty.vt = VT_I4;
						vOldMemberProperty.lVal = lOldModes;
						vNewMemberProperty.lVal = pPrMember->lModes;
						// We need to fire the OnMemberPropertyChanged event
						if (!m_pccsock->bFireMemberPropertyChanged(this, pPrMember->szNickname, m_pccsock->SzGetNickname(), g_rgwszMemberPropertyNames[pnMemberModes], &vOldMemberProperty, &vNewMemberProperty))
							m_hrLastError = m_pccsock->HrGetLastError();
					}
				}
			//}
			break;
		}

		default:
			m_hrLastError = CC_E_INVALIDITEMNAME;
	}

//	if (szValue)
//		delete [] szValue;

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatChannel::bQueryMemberProperty(LPCTSTR szMemberNickname, enumMemberPropertyName mpnProp, LONG *plNumber, BSTR *pbstrString)
{
	PPRQUERY	pPrQuery = NULL;
	PPRMEMBER	pPrMember = NULL;
	BYTE		*pb;
	INT			cb;
	BOOL		bIsPropLocal = FALSE;
	BOOL		bRet;

	OutputDebugThreadIdString("CChatChannel::bQueryMemberProperty - Enter\n");

	ASSERT(szMemberNickname, "szMemberNickname is NULL in CChatChannel::bQueryMemberProperty");
	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bQueryMemberProperty");

	// Check if we are in the channel or not
	if (!bCheckOpen())
		return FALSE;

	// Get the member with that nickname
	if (!bGetMemberFromNickname(szMemberNickname, &pPrMember))
	{
		m_hrLastError = CC_E_NOSUCHMEMBER;
		return FALSE;
	}

	ASSERT(pPrMember, "pPrMember is NULL in CChatChannel::bQueryMemberProperty");
	ASSERT(0 == lstrcmpi(szMemberNickname, pPrMember->szNickname), "Unexpected nickname in CChatChannel::bQueryMemberProperty");

	// What prop do we want?
	switch (mpnProp)
	{
		case pnMemberModes:
			ASSERT(plNumber, "plNumber is NULL in CChatChannel::bQueryMemberProperty");
			*plNumber = 0L;
			bIsPropLocal = FALSE;	// Do a UserHost to get the away/sysop flags
			break;
		case pnMemberIdentity:
			ASSERT(pbstrString, "pbstrString is NULL in CChatChannel::bQueryMemberProperty");
			*pbstrString = NULL;
			bIsPropLocal = (pPrMember->szUserName && pPrMember->szIPAddress);
			break;
		default:
			ASSERT(FALSE, "Unexpected property name in CChatChannel::bQueryMemberProperty");
			m_hrLastError = E_FAIL;
			return FALSE;
	}

	if (bIsPropLocal)
	{
		// Property is already local
		switch (mpnProp)
		{
			case pnMemberIdentity:
			{
				// Identity = Username@IPAddress
				LPTSTR szIdentity = SzIdentityFromUserNameAndIPAddress(pPrMember->szUserName, pPrMember->szIPAddress);
				if (!szIdentity)
				{
					m_hrLastError = E_OUTOFMEMORY;
					return FALSE;
				}
				*pbstrString = A2BSTR(szIdentity);
				delete [] szIdentity;
				if (!*pbstrString)
					m_hrLastError = E_OUTOFMEMORY;
				return *pbstrString != NULL;
			}
			//case pnMemberModes:
			//{
			//	*plNumber = pPrMember->lModes;
			//	ASSERT(bIsMemberModesValid(*plNumber), "Invalid member modes in CChatChannel::bQueryMemberProperty");
			//	return TRUE;
			//}
		}
	}
	else
	{
		// Add query cell at the end of the list
		if (pPrQuery = new PRQUERY)
			ZeroMemory(pPrQuery, sizeof(PRQUERY));
		if (!pPrQuery || 
			!bCopyStr(&(pPrQuery->szChannelName), m_prChannel.szName))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto error;
		}
		pPrQuery->at = atMember;
		pPrQuery->bSyncAccess = TRUE;
		pPrQuery->uHeadReplyCode = 0;
		pPrQuery->pvData = (PVOID) pPrMember;

		if (!m_pccsock->GetPQueryList()->bInsertCell((LONG) m_pccsock->GetPQueryList()->DwGetCellCount()+1, (PVOID) pPrQuery))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto error;
		}

		// Send the request to the server
		pPrQuery->uHeadReplyCode = RPL_USERHOST;
		if (SUCCEEDED(m_hrLastError = HrMakeUserHostMsg(&pb, &cb, (CHAR*) szMemberNickname)))
			m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
		if (FAILED(m_hrLastError))
			goto error;

		// Wait for the property to come!
		if (!m_pccsock->bWaitForProperty(atMember, (SHORT) mpnProp, pPrQuery))
		{
			m_hrLastError = m_pccsock->HrGetLastError();
			goto error;
		}

		// Got the property
		switch (mpnProp)
		{
			case pnMemberIdentity:
			{
				LPTSTR szIdentity = SzIdentityFromUserNameAndIPAddress(pPrMember->szUserName, pPrMember->szIPAddress);
				if (!szIdentity)
				{
					m_hrLastError = E_OUTOFMEMORY;
					goto error;
				}
				*pbstrString = A2BSTR(szIdentity);
				delete [] szIdentity;
				if (!*pbstrString)
				{
					m_hrLastError = E_OUTOFMEMORY;
					goto error;
				}
				break;
			}
			case pnMemberModes:
			{
				*plNumber = pPrMember->lModes;
				ASSERT(bIsMemberModesValid(*plNumber), "Invalid member modes in CChatChannel::bQueryMemberProperty");
				// The Not/Away and Not/Sysop flags are reset once we exposed them - this is just a snapshot!
				pPrMember->lModes &= ~mmSysop;
				pPrMember->lModes &= ~mmNotSysop;
				pPrMember->lModes &= ~mmAway;
				pPrMember->lModes &= ~mmNotAway;
				ASSERT(bIsMemberModesValid(pPrMember->lModes), "Invalid member modes in CChatChannel::bQueryMemberProperty");
				break;
			}
		}

		bRet = m_pccsock->GetPQueryList()->bRemoveCell((PVOID) pPrQuery);
		ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatChannel::bQueryMemberProperty");
	}
	return TRUE;

error:
	// Remove query cell from list
	if (pPrQuery)
	{
		if (m_hrLastError == CC_E_TIMEOUT)
		{
			// We know wait for the answer asynchronously
			ASSERT(pPrQuery->bSyncAccess, "pPrQuery->bSyncAccess is FALSE in CChatChannel::bQueryMemberProperty");
			pPrQuery->bSyncAccess = FALSE;
		}
		else
		{
			if (!m_pccsock->GetPQueryList()->bRemoveCell((PVOID) pPrQuery))
			{
				if (pPrQuery->szChannelName)
					delete [] pPrQuery->szChannelName;
				delete pPrQuery;
			}
		}
	}
	return FALSE;
}


BOOL CChatChannel::bQueryMemberProperty(BOOL bSyncAccess, LPCWSTR wszMemberPropertyName, LPCTSTR szMemberNickname, CChatItems **ppCItems)
{
	PPRQUERY	pPrQuery = NULL;
	PPRMEMBER	pPrMember = NULL;
	BOOL		bIsPropLocal = FALSE;
	BYTE		*pb;
	INT			cb;
	BOOL		bRet;

	OutputDebugThreadIdString("CChatChannel::bQueryMemberProperty - Enter\n");

	ASSERT(!bSyncAccess || ppCItems, "bSyncAccess && !ppCItems in CChatChannel::bQueryMemberProperty");
	ASSERT(szMemberNickname, "szMemberNickname is NULL in CChatChannel::bQueryMemberProperty");
	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bQueryMemberProperty");

	if (ppCItems)
		*ppCItems = NULL;

	if (!wszMemberPropertyName)
	{
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Make sure that the property name is known
	enumMemberPropertyName mpnProp = (enumMemberPropertyName) NGetPropertyIndex((SHORT) atMember, wszMemberPropertyName);

	if (mpnProp < 0)
	{
		// this property is not a member prop
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	// Check if we are in the channel or not
	if (!bCheckOpen())
		return FALSE;

	// Get the member with that nickname
	if (!bGetMemberFromNickname(szMemberNickname, &pPrMember))
	{
		m_hrLastError = CC_E_NOSUCHMEMBER;
		return FALSE;
	}

	ASSERT(pPrMember, "pPrMember is NULL in CChatChannel::bQueryMemberProperty");
	ASSERT(0 == lstrcmpi(szMemberNickname, pPrMember->szNickname), "Unexpected nickname in CChatChannel::bQueryMemberProperty");

	// We assume here that all the properties come from the USERHOST command
	switch (mpnProp)
	{
		case pnMemberNickname:
			bIsPropLocal = TRUE;
			break;

		case pnMemberModes:
			bIsPropLocal = FALSE;
			break;

		case pnMemberUserName:
		case pnMemberIPAddress:
		case pnMemberIdentity:
			bIsPropLocal = (pPrMember->szUserName && pPrMember->szIPAddress);
			break;

		case pnMemberObjectId:
			bIsPropLocal = (pPrMember->lOID != 0L);
			break;

		default:
			// this property is not a member prop
			m_hrLastError = CC_E_INVALIDITEMNAME;
			return FALSE;
	}

	if (bSyncAccess)
	{
		// Create a CChatItems object via a virtual function that in fact creates a new CItemsObject object
		if (!m_pccsock->bCreateChatItems(ppCItems))
		{
			m_hrLastError = m_pccsock->HrGetLastError();
			return FALSE;
		}

		ASSERT(*ppCItems, "*ppCItems is NULL in CChatChannel::bQueryMemberProperty");
	}

	if (!bIsPropLocal || !bSyncAccess)
	{
		if (pPrQuery = new PRQUERY)
			ZeroMemory(pPrQuery, sizeof(PRQUERY));
		if (!pPrQuery || 
			!bCopyStr(&(pPrQuery->szChannelName), m_prChannel.szName))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto error;
		}
		pPrQuery->at = atMember;
		pPrQuery->bSyncAccess = bSyncAccess;
		pPrQuery->uHeadReplyCode = 0;
		pPrQuery->pvData = (PVOID) pPrMember;
	}

	if (bIsPropLocal)
	{
		if (bSyncAccess)
		{
			ASSERT(!pPrQuery, "pPrQuery is NOT NULL in CChatChannel::bQueryMemberProperty");
			if (!m_pccsock->bFillChatItemsFromMember(*ppCItems, pPrMember))
			{
				m_hrLastError = m_pccsock->HrGetLastError();
				goto error;
			}
		}
		else
		{
			ASSERT(pPrQuery, "pPrQuery is NULL in CChatChannel::bQueryMemberProperty");
			return SUCCEEDED(m_hrLastError = m_pccsock->HrPostEvent(WM_U_MEMBERPROP, (WPARAM) pPrQuery, (LPARAM) FALSE));
		}
	}
	else
	{
		// Add query cell at the end of the list
		if (!m_pccsock->GetPQueryList()->bInsertCell((LONG) m_pccsock->GetPQueryList()->DwGetCellCount()+1, (PVOID) pPrQuery))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto error;
		}

		// Send the request to the server
		pPrQuery->uHeadReplyCode = RPL_USERHOST;
		if (SUCCEEDED(m_hrLastError = HrMakeUserHostMsg(&pb, &cb, (CHAR*) szMemberNickname)))
			m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
		
		if (FAILED(m_hrLastError))
			goto error;

		if (bSyncAccess)
		{
			// wait for the property to come!
			if (!m_pccsock->bWaitForProperty(atMember, (SHORT) mpnProp, pPrQuery))
			{
				m_hrLastError = m_pccsock->HrGetLastError();
				goto error;
			}

			if (!m_pccsock->bFillChatItemsFromMember(*ppCItems, pPrMember))
			{
				m_hrLastError = m_pccsock->HrGetLastError();
				goto error;
			}

			bRet = m_pccsock->GetPQueryList()->bRemoveCell((PVOID) pPrQuery);
			ASSERT(bRet, "m_listQuery.bRemoveCell failed in CChatChannel::bQueryMemberProperty");
		}
	}
	return TRUE;

error:
	// Remove query cell from list
	if (pPrQuery)
	{
		if (m_hrLastError == CC_E_TIMEOUT)
		{
			// We know wait for the answer asynchronously
			ASSERT(pPrQuery->bSyncAccess, "pPrQuery->bSyncAccess is FALSE in CChatChannel::bQueryMemberProperty");
			pPrQuery->bSyncAccess = FALSE;
		}
		else
		{
			if (!m_pccsock->GetPQueryList()->bRemoveCell((PVOID) pPrQuery))
			{
				if (pPrQuery->szChannelName)
					delete [] pPrQuery->szChannelName;
				delete pPrQuery;
			}
		}
	}

	if (ppCItems && *ppCItems)
	{
		m_pccsock->ReleaseChatItems(*ppCItems);
		*ppCItems = NULL;
	}

	return FALSE;
}


BOOL CChatChannel::bBanMembers(CChatItems *pCItems, BOOL bSet)
{
	LPTSTR	szIdentMask = NULL;
	BYTE	*pb;
	INT		cb;

	OutputDebugThreadIdString("CChatChannel::bBanMembers - Enter\n");

	// Check if we are in the channel or not
	if (!bCheckOpen())
		return FALSE;

	// Get the identmask string from the chatitems object   <nickname>!<username>@<ipaddress>
	if (!m_pccsock->bGetIdentMaskFromChatItems(pCItems, &szIdentMask))
	{
		m_hrLastError = m_pccsock->HrGetLastError();
		return FALSE;
	}

	ASSERT(szIdentMask, "szIdentMask is NULL in CChatChannel::bBanMembers");

	// Send un/ban to the server
	if (SUCCEEDED(m_hrLastError = HrMakeSetChannelModeMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) (bSet ? "+b":"-b"), (CHAR*) szIdentMask)))
		m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);

	delete [] szIdentMask;

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatChannel::bListMembers(CChatItems *pCItems)
{
//// For future releases //////////
//	enumMemberPropertyName mpnProp;	
//	BSTR		bstrAssociatedType;
//	PPRITEM		pPrItem		= NULL;
//	CItemLList	*plistItem	= NULL;
//	VARIANT		vName;
//	VARIANT		vValue;
//	So far, the IRCX does not support any sophisticated member listings
//	The CChatItems parameter is unused and there for later backwards compatibility

	BYTE *pb;
	INT	 cb;

	OutputDebugThreadIdString("CChatChannel::bListMembers - Enter\n");

	// Check if we are in the channel or not
	if (!bCheckOpen())
		return FALSE;

	/* REGISB 04/30/97: This code can be used later if the IRCX exposes exposes some complex querying
	if (!pCItems || 
		!(bstrAssociatedType = pCItems->BstrGetAssociatedType()) ||
		0 != _wcsicmp(bstrAssociatedType, (LPWSTR) g_rgwszAssociatedTypes[atQuery]))
	{
		m_hrLastError = E_INVALIDARG;
		return FALSE;
	}

	plistItem = pCItems->GetItemList();

	ASSERT(plistItem, "plistItem is NULL in CChatChannel::bListMembers");

	// We take care of the non-Op items first
	for (LONG lPos = 1L; lPos <= (LONG) plistItem->DwGetCellCount(); lPos++)
	{
		if (!plistItem->bGetCell(lPos, (PVOID*) &pPrItem))
		{
			m_hrLastError = E_FAIL;
			return FALSE;
		}
		ASSERT(pPrItem, "pPrItem is NULL in CChatChannel::bListMembers");
		
		vName = pPrItem->vName;
		vValue = pPrItem->vValue;

		// Is the property name a string?
		if (vName.vt != VT_BSTR || !vName.bstrVal)
		{
			m_hrLastError = CC_E_INVALIDITEMNAME;
			return FALSE;
		}
		
		// Make sure that the property name is known as a member property
		mpnProp = (enumMemberPropertyName) NGetPropertyIndex((SHORT) atMember, vName.bstrVal);

		if (mpnProp < 0)
		{
			// this property is not a member prop
			m_hrLastError = CC_E_INVALIDITEMNAME;
			return FALSE;
		}
		
		// On IRC2 servers, no listing criteria is possible at all, pfff.
		if (!m_bIrcXChannel)
		{
			m_hrLastError = CC_E_NOTIRCX;
			return FALSE;
		}

		// Check if value has right type
		switch (mpnProp)
		{
			// REGISB: we might be able to simplify this once IRCX is stable
			case pnMemberIdentity:
			case pnMemberIPAddress:
			case pnMemberIPAddressOp:
			case pnMemberNickname:
			case pnMemberNicknameOp:
			case pnMemberUserName:
			case pnMemberUserNameOp:
				// Is the property value a string?
				if (vValue.vt != VT_BSTR)
				{
					m_hrLastError = CC_E_INVALIDITEMVALUE;
					return FALSE;
				}
				break;

			case pnMemberModes:
			case pnMemberObjectId:
				// Is the property value a number?
				if (vValue.vt != VT_I2 && vValue.vt != VT_I4)
				{
					m_hrLastError = CC_E_INVALIDITEMVALUE;
					return FALSE;
				}
				break;
		}

		// Construct query for server
		// REGISB: Lots of work TBD here
	}

	// We now take care of the Op items
	for (lPos = 1L; lPos <= (LONG) plistItem->DwGetCellCount(); lPos++)
	{
		if (!plistItem->bGetCell(lPos, (PVOID*) &pPrItem))
		{
			m_hrLastError = E_FAIL;
			return FALSE;
		}
		ASSERT(pPrItem, "pPrItem is NULL in CChatChannel::bListMembers");
		
		vName = pPrItem->vName;
		vValue = pPrItem->vValue;

		// Get the member property name
		mpnProp = (enumMemberPropertyName) NGetPropertyIndex((SHORT) atMember, vName.bstrVal);

		ASSERT(mpnProp >= 0, "mpnProp < 0 in CChatChannel::bListMembers");
		ASSERT(vName.vt == VT_BSTR, "vName.vt != VT_BSTR in CChatChannel::bListMembers");
		ASSERT(vValue.vt == VT_BSTR, "vValue.vt != VT_BSTR in CChatChannel::bListMembers");
	
		// Construct query for server
		// REGISB: Lots of work TBD here
	}
	*/

	if (SUCCEEDED(m_hrLastError = HrMakeNamesMsg(&pb, &cb, (CHAR*) m_prChannel.szName)))
	{
		m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);
		if (SUCCEEDED(m_hrLastError))
		{
			PPRQUERY pPrQuery = new PRQUERY;
			if (!pPrQuery)
			{
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}

			pPrQuery->at = atQuery;
			pPrQuery->bSyncAccess = FALSE;
			pPrQuery->uHeadReplyCode = RPL_NAMEREPLY;
			pPrQuery->pvData = (PVOID) qtNamesForMembers;
			pPrQuery->szChannelName = NULL;
			if (!bCopyStr(&(pPrQuery->szChannelName), m_prChannel.szName) ||
				!m_pccsock->GetPQueryList()->bInsertCell((LONG) m_pccsock->GetPQueryList()->DwGetCellCount()+1, (PVOID) pPrQuery))
			{
				delete [] pPrQuery;
				m_hrLastError = E_OUTOFMEMORY;
				return FALSE;
			}
		}
	}

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatChannel::bAddMember(LPCTSTR szMemberInfo, PPRMEMBER* ppPrMember)
{
	PPRMEMBER	pPrMember = NULL;
	CHAR		*szNickname, *szUserName, *szIPAddress;
	BOOL		bIsOwner = FALSE, bIsHost = FALSE, bHasVoice = FALSE, bAuthMember;

	ASSERT(szMemberInfo, "szMemberInfo is NULL in CChatChannel::bAddMember");

	if (ppPrMember)
		*ppPrMember = NULL;

	szNickname = (CHAR*) szMemberInfo;

	if ((bIsOwner = (szNickname[0] == '.')) || (bIsHost = (szNickname[0] == '@')) || (bHasVoice = (szNickname[0] == '+')))
		szNickname++;

//	bIsSpectator = (m_prChannel.lModes & cmModerated) && !bIsSpeakerInModerated && !bIsHost && !bIsOwner;

	// szMemberInfo has format MsChaTst!~regisb@157.55.102.164   or  @MsChaTst, or .MsChaTst
	ExtractNickUserIPAddress((LPTSTR) szNickname, m_bIrcXChannel, &szNickname, &szUserName, &szIPAddress, &bAuthMember);

	pPrMember = new PRMEMBER;
	if (!pPrMember)
		goto error;

	pPrMember->szNickname			= NULL;
	pPrMember->szUserName			= NULL;
	pPrMember->szIPAddress			= NULL;
	pPrMember->lOID					= 0L;
	// pPrMember->bSpeakerInModerated	= bIsOwner || bIsHost || bIsSpeakerInModerated;

	if (!bCopyStr(&(pPrMember->szNickname), szNickname))
		goto error;

	if (szUserName)
		if (!bCopyStr(&(pPrMember->szUserName), szUserName))
			goto error;

	if (szIPAddress)
		if (!bCopyStr(&(pPrMember->szIPAddress), szIPAddress))
			goto error;

	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bAddMember");
	ASSERT(m_pccsock->SzGetNickname(), "m_pccsock->SzGetNickname() is NULL in CChatChannel::bAddMember");

	pPrMember->lModes = mmNotClientIgnored;

	if (bIsOwner)
		pPrMember->lModes |= (mmNotHost+mmOwner);
	else
	{
		pPrMember->lModes |= mmNotOwner;

		if (bIsHost)
			pPrMember->lModes |= mmHost;
		else
			pPrMember->lModes |= mmNotHost;
	}

	if (bHasVoice)
		pPrMember->lModes |= mmVoice;
	else
		pPrMember->lModes |= mmNoVoice; // Member has no voice by default

	if (bAuthMember)
		pPrMember->lModes |= mmAuthMember;
	else
		if (szUserName || !m_bIrcXChannel)
			pPrMember->lModes |= mmNotAuthMember;

	ASSERT(bIsMemberModesValid(pPrMember->lModes), "Invalid member modes in CChatChannel::bAddMember");

	if (m_hashMember.bAdd(pPrMember->szNickname, NULL, (PVOID) pPrMember))
	{
		if (ppPrMember)
			*ppPrMember = pPrMember;
		return TRUE;
	}

error:
	m_hrLastError = E_OUTOFMEMORY;
	return FALSE;
}


BOOL CChatChannel::bAddMember(PPRMEMBER pPrMember)
{
	ASSERT(pPrMember, "pPrMember is NULL in CChatChannel::bAddMember");
	ASSERT(pPrMember->szNickname, "pPrMember->szNickname is NULL in CChatChannel::bAddMember");
	ASSERT(bIsMemberModesValid(pPrMember->lModes), "Invalid member modes in CChatChannel::bAddMember");

	if (m_hashMember.bAdd(pPrMember->szNickname, NULL, (PVOID) pPrMember))
		return TRUE;
	else
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
}


BOOL CChatChannel::bDelMember(LPCTSTR szNickname, BOOL bFreeContent)
{
	ASSERT(szNickname, "szNickname is NULL in CChatChannel::bDelMember");

	PVOID	pvPrMember = NULL;

	if (!m_hashMember.bRemove((CHAR*) szNickname, NULL, &pvPrMember))
	{
		m_hrLastError = E_FAIL;
		return FALSE;
	}

	ASSERT(pvPrMember, "pvPrMember is NULL in CChatChannel::bDelMember");

	if (bFreeContent)
	{
		// we found the member, deleted it from the hash table, now we want to free its memory
		if (!m_hashMember.bDeleteUserData(pvPrMember))
		{
			m_hrLastError = E_FAIL;
			return FALSE;
		}
	}

	return TRUE;
}


// Leave this channel
BOOL CChatChannel::bLeaveChannel()
{
	OutputDebugThreadIdString("CChatChannel::bLeaveChannel - Enter\n");

	if (!bCheckOpen())
		return FALSE;

	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bLeaveChannel");
	ASSERT(m_pccsock->GetPChatConn(), "m_pccsock->GetPChatConn() is NULL in CChatChannel::bLeaveChannel");

	// Construct a leave channel msg
	BYTE	*pb;
	INT		cb;

	if (SUCCEEDED(m_hrLastError = HrMakeLeaveChannel(&pb, &cb, m_prChannel.szName)))
		m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatChannel::bSetChannelTopic(LPCTSTR szTopic)
{
	ASSERT(szTopic, "szTopic is NULL in CChatChannel::bSetChannelTopic");

	if (!bCopyStr(&(m_prChannel.szTopic), szTopic))
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	else
		return TRUE;
}


BOOL CChatChannel::bSendMessage(LONG msgt, VARIANT *pvMessage, VARIANT* pvRecipientNicknames, VARIANT* pvTag)
{
	PBYTE	rgbMsg = NULL;
	DWORD	dwcb;
    LONG	cRcp, cbLen;
	LPTSTR	szTagTmp = NULL, szRcpNicks = NULL;
	BOOL	bPostProcess;

	m_hrLastError = NOERROR;	// No Error by default

	USES_CONVERSION;

	if (pvRecipientNicknames && pvRecipientNicknames->vt == (VT_BYREF+VT_VARIANT))
		pvRecipientNicknames = pvRecipientNicknames->pvarVal;

	if (pvMessage && pvMessage->vt == (VT_BYREF+VT_VARIANT))
		pvMessage = pvMessage->pvarVal;

	if (pvTag && pvTag->vt == (VT_BYREF+VT_VARIANT))
		pvTag = pvTag->pvarVal;

	// make sure pvMessage != NULL
	if (!pvMessage)
	{
		m_hrLastError = E_INVALIDARG;
		goto exit;
	}

	// make sure msgt does not contain parasites
	if (msgtNormal					!= msgt &&
		msgtNotice					!= msgt &&
		msgtWhisper					!= msgt &&
		msgtData					!= msgt &&
		msgtDataRequest				!= msgt &&
		msgtDataReply				!= msgt &&
		msgtData+msgtDataRaw		!= msgt &&
		msgtDataRequest+msgtDataRaw	!= msgt &&
		msgtDataReply+msgtDataRaw	!= msgt &&
		msgtAction					!= msgt &&
		msgtSound					!= msgt &&
		msgtCTCP					!= msgt &&
		msgtAction+msgtNotice		!= msgt &&
		msgtSound+msgtNotice		!= msgt &&
		msgtCTCP+msgtNotice			!= msgt &&
		msgtAction+msgtWhisper		!= msgt &&
		msgtSound+msgtWhisper		!= msgt &&
		msgtCTCP+msgtWhisper		!= msgt)
	{
		m_hrLastError = CC_E_BADMSGTYPE;
		goto exit;
	}

	// Make sure this command is allowed on this server
	if (
		!m_bIrcXChannel && 
		(
		 (msgt & msgtData) ||
		 (msgt & msgtDataRequest) ||
		 (msgt & msgtDataReply) ||
		 (msgt & msgtWhisper)
		)
	   )
	{
		m_hrLastError = CC_E_NOTIRCX;
		goto exit;
	}

    if (!pvTag ||
		!(
		  (pvTag->vt == VT_EMPTY) || 
		  (pvTag->vt == VT_NULL)  || 
		  (pvTag->vt == VT_ERROR) || 
		  (pvTag->vt == VT_BSTR)
		 )
		)
	{
		m_hrLastError = CC_E_BADTAG;
		goto exit;
	}
	if (pvTag->vt == VT_BSTR && pvTag->bstrVal && g_chEOS != *pvTag->bstrVal)
	{
		if (!(szTagTmp = W2T(pvTag->bstrVal)))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
	}
	if (
		(szTagTmp && !bIsMsgtData(msgt)) ||		// There is a tag but mgstData, msgtDataRequest, msgtDataReply are not set
	    (!szTagTmp && bIsMsgtData(msgt))		// There is no tag but mgstData or msgtDataRequest or msgtDataReply is set
	   )	
	{
		m_hrLastError = CC_E_BADTAG;
		goto exit;
	}

	// Construct the recipient list
	if (!m_pccsock->bConstructRecipientList(pvRecipientNicknames, &szRcpNicks, &cRcp))
	{
		m_hrLastError = m_pccsock->HrGetLastError();
		goto exit;
	}

	if (0L == cRcp)
	{
		// Target is whole channel
		// Make sure that the msgt does not include the msgtWhisper flag
		if (msgt & msgtWhisper)
		{
			m_hrLastError = CC_E_BADMSGTYPE;
			goto exit;
		}
	}
	else
	{
		// Uni or Multi-casting case
		// Make sure that specific member targets are allowed
		if (!m_bIrcXChannel)
		{
			m_hrLastError = CC_E_INVALIDRECIPIENTLIST;
			goto exit;
		}
	}

    // If the message is a 0 byte string then don't send anything and throw an error
    if (pvMessage->vt == VT_BSTR && (!pvMessage->bstrVal || !*pvMessage->bstrVal))
	{
		m_hrLastError = CC_E_NULLMSG;
		goto exit;
	}

	// Take care of the BSTRs separately
	if ((VT_BSTR == pvMessage->vt) && !bIsMsgtData(msgt))
	{
		BOOL	bFreeMessage = FALSE, bFreeMessageToSend = FALSE, bFreeTmp;
		LPTSTR	szMessage, szMessageToSend = NULL, szTmp;
		
		if (g_chExtChnPfx == m_prChannel.szName[0] && !m_prChannel.bMicOnly)
		{
			// We are in an extended channel, we need to send UTF8 strings
			if (!bConvertWideStringToUTF8(pvMessage->bstrVal, 0, &szMessage, NULL, FALSE /*bNickname*/, FALSE /*bChannelName*/, FALSE /*bPostProcess*/))
			{
				m_hrLastError = E_OUTOFMEMORY;
				goto exit;
			}
			bFreeMessage = TRUE;
		}
		else
		{
			// Might have to convert channel message from ShiftJis to Jis, etc...
			LPTSTR	szTmp1;
			if (!(szTmp1 = W2T(pvMessage->bstrVal)) ||
				!bConvertString(FALSE, GetCharSet(), szTmp1, 0, &szMessage, NULL, &bFreeMessage))
			{
				m_hrLastError = E_OUTOFMEMORY;
				goto exit;
			}
		}

		// Might have to quote \r and \n
		if (!bLowLevelQuoting(g_chLLQuoteCTCP, TRUE /*bTreatAsByteArray*/, szMessage, &szTmp, &bFreeTmp))
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
		if (bFreeTmp)
		{
			if (bFreeMessage) delete [] szMessage;
			szMessage = szTmp;
			bFreeMessage = TRUE;
		}

		// Package the outgoing message in function of the msgt value
		switch (msgt & ~(msgtNotice|msgtWhisper))
		{
			case 0L:
				szMessageToSend = szMessage;
				break;

			case msgtAction:
			case msgtSound:
			case msgtCTCP:
				szMessageToSend = new TCHAR[cbLen = lstrlen(szMessage) + 32];
				if (!szMessageToSend)
				{
					m_hrLastError = E_OUTOFMEMORY;
					goto exit;
				}
				else
				{
					bFreeMessageToSend = TRUE;
					ZeroMemory(szMessageToSend, cbLen);
				}

				szMessageToSend[0] = g_byteCTCPBrace;
				
				switch (msgt & ~(msgtNotice|msgtWhisper))
				{
					case msgtAction:
						lstrcpy(szMessageToSend+1, m_pccsock->m_rgszComic[IDS_ACTION-IDS_COMICSTART]);
						lstrcat(szMessageToSend, g_szSpace);
						break;
					case msgtSound:
						lstrcpy(szMessageToSend+1, m_pccsock->m_rgszComic[IDS_SOUND-IDS_COMICSTART]);
						lstrcat(szMessageToSend, g_szSpace);
						break;
					case msgtCTCP:
						break;
				}

				lstrcat(szMessageToSend, szMessage);
				szMessageToSend[lstrlen(szMessageToSend)] = g_byteCTCPBrace;
				break;

			default:
				m_hrLastError = CC_E_BADMSGTYPE;
				goto exit;
		}

		// Send text message to the channel or recipients
		bSendText(szMessageToSend, szRcpNicks, msgt & msgtNotice, msgt & msgtWhisper);

		if (bFreeMessageToSend)
			delete [] szMessageToSend;

		if (bFreeMessage)
			delete [] szMessage;

		goto exit;
	}

	// Take care of the msgtData, msgtDataRequest, msgtDataReply cases
	ASSERT(bIsMsgtData(msgt), "Unexpected msgt in CChannelObject::SendMessage");

	// Construct the data stream
	if (!m_pccsock->bConstructDataStream(pvMessage, &rgbMsg, &dwcb, (BOOL) (msgt & msgtDataRaw), &bPostProcess))
	{
		m_hrLastError = m_pccsock->HrGetLastError();
		goto exit;
	}

    // Send the data message to the channel
	ASSERT(m_bIrcXChannel, "!m_bIrcXChannel in CChannelObject::SendMessage");
	ASSERT(rgbMsg, "rgbMsg is NULL in CChannelObject::SendMessage");
	ASSERT(dwcb, "dwcb == 0L in CChannelObject::SendMessage");

	if (!m_pccsock->bSendData((LPCTSTR) m_prChannel.szName, (LPCTSTR) szRcpNicks, (LPCTSTR) szTagTmp, rgbMsg, dwcb, NCmdIdFromMsgt(msgt), bPostProcess))
		m_hrLastError = m_pccsock->HrGetLastError();

exit:
	if (rgbMsg)
		delete [] rgbMsg;

	if (szRcpNicks)
		delete [] szRcpNicks;

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatChannel::bSendText(LPCTSTR szText, LPCTSTR szRcpNicks, BOOL bNotice, BOOL bWhisper)
{	
	BYTE	*pb;
	INT		cb;

	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bSendText");
	ASSERT(m_pccsock->GetPChatConn(), "m_pccsock->GetPChatConn() is NULL in CChatChannel::bSendText");
	ASSERT(m_chs == chsOpen, "m_chs != chsOpen in CChatChannel::bSendText");

	if (SUCCEEDED(m_hrLastError = HrMakeSendText(&pb, &cb, bNotice, bWhisper, m_prChannel.szName, (CHAR*) szRcpNicks, (CHAR*) szText)))
		m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);

	return SUCCEEDED(m_hrLastError);
}


BOOL CChatChannel::bKickMember(LPCWSTR wszKickedNickname, LPCTSTR szReason)
{
	BYTE	*pb;
	INT		cb;
	LPTSTR	szKickedNickname;
	BOOL	bFreeNickname = FALSE;

	ASSERT(m_pccsock, "m_pccsock is NULL in CChatChannel::bKickMember");
	ASSERT(m_pccsock->GetPChatConn(), "m_pccsock->GetPChatConn() is NULL in CChatChannel::bKickMember");
	ASSERT(m_chs == chsOpen, "m_chs != chsOpen in CChatChannel::bKickMember");

	// REGISB: we intentionally don't check if:
	//	1. the target is in our channel
	//  2. the kicker has the right privileges
	// the server will take care of those checks and will return the appropriate error

	if (m_bIrcXChannel && bExtendedWideNickname(wszKickedNickname))
	{
		// The nickname is extended on IRCX - Need to convert it to UTF8
		bConvertWideStringToUTF8(wszKickedNickname, 0, &szKickedNickname, NULL, TRUE /*bNickname*/);
		bFreeNickname = TRUE;
	}
	else
	{
		USES_CONVERSION;
		szKickedNickname = W2T(wszKickedNickname);
	}

	if (!szKickedNickname)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (SUCCEEDED(m_hrLastError = HrMakeKickMsg(&pb, &cb, (CHAR*) m_prChannel.szName, (CHAR*) szKickedNickname, (CHAR*) (szReason ? szReason : g_szEmpty))))
		m_hrLastError = m_pccsock->GetPChatConn()->HrSendAndDelete(pb, cb);

	if (bFreeNickname)
		delete [] szKickedNickname;

	return SUCCEEDED(m_hrLastError);
}
