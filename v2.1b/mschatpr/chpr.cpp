// ChPr.cpp : Implementation of CChannelObject's properties
#include "StdAfx.H"
#include "MsChatPr.H"
#include "ChObj.H"
#include "CCSock.H"
#include "CItmsObj.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CChannelObject's properties

STDMETHODIMP CChannelObject::get_Index(long * plIndex)
{
	CChatChannel*	pChannel = NULL;
	LONG			lPositionFound = 0L;

	OutputDebugThreadIdString("CChannelObject::get_Index - Enter\n");

    CHECK_POINTER(plIndex);

	*plIndex = 0;

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::get_Index");

	if (!m_pccsock->GetPChanList()->bFindCellFromData(1L, g_nSearchByChatChannel, (PVOID) this, g_nSearchNone, NULL, (PVOID*) &pChannel, &lPositionFound))
	{
		ASSERT(FALSE, "Could not find channel cell in CChannelObject::get_Index");
		return HrThrowError(CPR_E_FAILURE);
	}

	ASSERT(pChannel == this, "pChannel != this in CChannelObject::get_Index");

	*plIndex = (long) (m_pccsock->GetPChanList()->DwGetCellCount() - lPositionFound + 1L);

	ASSERT(*plIndex >= 1L, "*plIndex < 1L in CChannelObject::get_Index");

    return NOERROR;
}


STDMETHODIMP CChannelObject::get_ChannelState(enumChannelState * pchs)
{
    CHECK_POINTER(pchs);

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	*pchs = m_chs;

#ifdef DEBUG
	sprintf(g_szDebugStr, "CChannelObject::get_ChannelState - Returns %ld\n", m_chs);
	OutputDebugThreadIdString(g_szDebugStr);
#endif // DEBUG

	return NOERROR;
}


STDMETHODIMP CChannelObject::get_ChannelName(BSTR * pbstrChannelName)
{
	OutputDebugThreadIdString("CChannelObject::get_ChannelName - Enter\n");

    CHECK_POINTER(pbstrChannelName);

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (m_prChannel.szName)
	{
		if (m_pccsock->bIsIrcXSocket())
		{
			if (g_chExtChnPfx == m_prChannel.szName[0])
			{
				// Need to convert UTF8 extended channel name to Unicode
				LPWSTR wszChannelName;
				if (!bConvertUTF8StringToWide(m_prChannel.szName, 0 /*cchIn*/, &wszChannelName, NULL /*pcchOut*/, FALSE, TRUE))
					return HrThrowError(CTL_E_OUTOFMEMORY);

				ASSERT(wszChannelName, "wszChannelName in NULL in CChannelObject::get_ChannelName");

				*pbstrChannelName = SysAllocString(wszChannelName);
				delete [] wszChannelName;
			}
			else
			{
				// Might have to convert from Jis to ShiftJis, etc...
				LPTSTR	szConvertedChannelName;
				BOOL	bFreeConverted;
		
				if (!bConvertString(TRUE, m_pccsock->GetCharSet(), m_prChannel.szName, 0, &szConvertedChannelName, NULL, &bFreeConverted))
					return HrThrowError(CTL_E_OUTOFMEMORY);

				*pbstrChannelName = A2BSTR(szConvertedChannelName);

				if (bFreeConverted)
					delete [] szConvertedChannelName;
			}
		}
		else
			*pbstrChannelName = A2BSTR(m_prChannel.szName);
	}
	else
		*pbstrChannelName = SysAllocString(L"");

    return (*pbstrChannelName) ? NOERROR : HrThrowError(CTL_E_OUTOFMEMORY);
}


STDMETHODIMP CChannelObject::put_ChannelName(BSTR bstrChannelName)
{
	OutputDebugThreadIdString("CChannelObject::put_ChannelName - Enter\n");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (m_chs != chsClosed)
		return HrThrowError(CTL_E_SETNOTPERMITTED);

	if (!bstrChannelName) 
	{
		bCopyStr(&(m_prChannel.szName), g_szEmpty);
		return NOERROR;
	}

	USES_CONVERSION;
	// Check if user wants to set an extended channel and
	// convert + put the % prefix if needed
	if (bExtendedWideChannelName(bstrChannelName))
	{
		if (m_prChannel.szName)
		{
			delete [] m_prChannel.szName;
			m_prChannel.szName = NULL;
		}
		if (L'%' == bstrChannelName[0])
		{
			// Need to convert channel name to UTF8
			if (!bConvertWideStringToUTF8(bstrChannelName, 0, &m_prChannel.szName, NULL, FALSE /*bNickname*/, TRUE /*bChannelName*/))
				return HrThrowError(CTL_E_OUTOFMEMORY);
		}
		else
		{
			// Might have to convert from ShiftJis to Jis, etc...
			LPTSTR	szConvertedChannelName, szTmp;
			BOOL	bFreeConverted;
	
			if (!(szTmp = W2T(bstrChannelName)) ||
				!bConvertString(FALSE, m_pccsock->GetCharSet(), szTmp, 0, &szConvertedChannelName, NULL, &bFreeConverted) ||
				!bCopyChanStr(&(m_prChannel.szName), szConvertedChannelName))
				return HrThrowError(CTL_E_OUTOFMEMORY);

			if (bFreeConverted)
				delete [] szConvertedChannelName;
		}
	}
	else
	{
		LPTSTR szTmp;
		
		if (!(szTmp = W2T(bstrChannelName)) ||
			!bCopyChanStr(&(m_prChannel.szName), szTmp))
			return HrThrowError(CTL_E_OUTOFMEMORY);
	}

	return NOERROR;
}


STDMETHODIMP CChannelObject::get_ChannelTopic(BSTR * pbstrChannelTopic)
{
	OutputDebugThreadIdString("CChannelObject::get_ChannelTopic - Enter\n");

    CHECK_POINTER(pbstrChannelTopic);

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (m_bGotMemList || m_chs != chsOpen)
	{
		if (m_prChannel.szTopic && *m_prChannel.szTopic)
		{
			if (m_prChannel.szName && g_chExtChnPfx == m_prChannel.szName[0] && !m_prChannel.bMicOnly)
			{
				// Need to convert UTF8 extended channel topic to Unicode
				LPWSTR wszChannelTopic;
				if (!bConvertUTF8StringToWide(m_prChannel.szTopic, 0 /*cchIn*/, &wszChannelTopic, NULL /*pcchOut*/, FALSE /*bNickname*/, FALSE /*bChannelName*/, FALSE /*bPostProcess*/))
					return HrThrowError(CTL_E_OUTOFMEMORY);

				ASSERT(wszChannelTopic, "wszChannelTopic in NULL in CChannelObject::get_ChannelTopic");

				*pbstrChannelTopic = SysAllocString(wszChannelTopic);
				delete [] wszChannelTopic;
			}
			else
			{
				LPTSTR	szTmp;
				BOOL	bFreeConverted;

				// Might have to convert topic from Jis to ShiftJis, etc...
				if (!bConvertString(TRUE, GetCharSet(), m_prChannel.szTopic, 0, &szTmp, NULL, &bFreeConverted))
					return HrThrowError(CTL_E_OUTOFMEMORY);

				*pbstrChannelTopic = A2BSTR(szTmp);

				if (bFreeConverted)
					delete [] szTmp;
			}
		}
		else
			*pbstrChannelTopic = SysAllocString(L"");
	}
	else
	{
		CChatItems *pCItems	= NULL;

		// We cash the timeout value and overwrite it for this internal synchronous call
		DWORD dwPropertyAccessTimeOut = m_pccsock->m_dwPropertyAccessTimeOut;
		m_pccsock->m_dwPropertyAccessTimeOut = (DWORD) g_lDefaultPropertyAccessTimeOut;

		if (bQueryChannelProperty(TRUE /*bSyncAccess*/, g_rgwszChannelPropertyNames[cpnTopic], &pCItems))
		{
			VARIANT vItem;

			// Restore cashed value
			m_pccsock->m_dwPropertyAccessTimeOut = dwPropertyAccessTimeOut;

			// pCItems was correctly set
			ASSERT(pCItems, "pCItems is NULL in CChannelObject::get_ChannelTopic");
			if (pCItems->bGetItem((BSTR&) g_rgwszChannelPropertyNames[cpnTopic], &vItem))
			{
				ASSERT(vItem.bstrVal, "vItem.bstrVal is NULL in CChannelObject::get_ChannelTopic");
				*pbstrChannelTopic = SysAllocString(vItem.bstrVal);
				m_pccsock->ReleaseChatItems(pCItems);
			}
			else
			{
				// Querying the channel topic failed
				*pbstrChannelTopic = SysAllocString(L"");
				return NOERROR;
			}
		}
		else
		{
			// Restore cashed value
			m_pccsock->m_dwPropertyAccessTimeOut = dwPropertyAccessTimeOut;

			// Querying the channel topic failed
			return HrThrowError(GetScodeFromHResult(HrGetLastError()));
		}
	}

	return (*pbstrChannelTopic) ? NOERROR : HrThrowError(CTL_E_OUTOFMEMORY);
}


STDMETHODIMP CChannelObject::get_MemberCount(long * plMemberCount)
{
	OutputDebugThreadIdString("CChannelObject::get_MemberCount - Enter\n");

    CHECK_POINTER(plMemberCount);

	*plMemberCount = 0L;

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (m_chs != chsOpen)
		return HrThrowError(CPR_E_NOTOPEN);

	if (m_bGotMemList)
	{
		*plMemberCount = LGetMemberCount();
		ASSERT(*plMemberCount > 0L, "*plMemberCount == 0L in CChannelObject::get_MemberCount");
	}
	else
	{
		CChatItems *pCItems	= NULL;

		// We cash the timeout value and overwrite it for this internal synchronous call
		DWORD dwPropertyAccessTimeOut = m_pccsock->m_dwPropertyAccessTimeOut;
		m_pccsock->m_dwPropertyAccessTimeOut = (DWORD) g_lDefaultPropertyAccessTimeOut;

		if (bQueryChannelProperty(TRUE /*bSyncAccess*/, g_rgwszChannelPropertyNames[cpnMemberCount], &pCItems))
		{
			VARIANT vItem;

			// Restore cashed value
			m_pccsock->m_dwPropertyAccessTimeOut = dwPropertyAccessTimeOut;

			// pCItems was correctly set
			ASSERT(pCItems, "pCItems is NULL in CChannelObject::get_MemberCount");
			if (pCItems->bGetItem((BSTR&) g_rgwszChannelPropertyNames[cpnMemberCount], &vItem))
			{
				ASSERT(vItem.lVal > 0L, "vItem.lVal <= 0L in CChannelObject::get_MemberCount");
				*plMemberCount = vItem.lVal;
				m_pccsock->ReleaseChatItems(pCItems);
			}
			else
			{
				// Querying the channel member count failed
				m_pccsock->ReleaseChatItems(pCItems);
				return HrThrowError(GetScodeFromHResult(pCItems->HrGetLastError()));
			}
		}
		else
		{
			// Restore cashed value
			m_pccsock->m_dwPropertyAccessTimeOut = dwPropertyAccessTimeOut;

			// Querying the channel member count failed
			return HrThrowError(GetScodeFromHResult(HrGetLastError()));
		}
	}

	return NOERROR;
}


STDMETHODIMP CChannelObject::get_MemberIdentity(VARIANT vMemberNickname, BSTR * pbstrMemberIdentity)
{
	HRESULT	hr = NOERROR;
	LPTSTR	szNickname = NULL;

	OutputDebugThreadIdString("CChannelObject::get_MemberIdentity - Enter\n");

	// Synchronous access to a Member's Modes
	CHECK_POINTER(pbstrMemberIdentity);
	
	*pbstrMemberIdentity = NULL;

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::get_MemberIdentity");
	if (!m_pccsock->bGetNicknameFromVariant(&vMemberNickname, &szNickname))
		return HrThrowError(GetScodeFromHResult(m_pccsock->HrGetLastError()));

	ASSERT(szNickname, "szNickname is NULL in CChannelObject::get_MemberIdentity");
	if (!bQueryMemberProperty(szNickname, pnMemberIdentity, NULL, pbstrMemberIdentity))
		hr = HrThrowError(GetScodeFromHResult(HrGetLastError()), NULL, szNickname);

	delete [] szNickname;

	return hr;
}


STDMETHODIMP CChannelObject::get_MemberModes(VARIANT vMemberNickname, long * plMemberModes)
{
	HRESULT	hr = NOERROR;
	LPTSTR	szNickname = NULL;

	OutputDebugThreadIdString("CChannelObject::get_MemberModes - Enter\n");

	// Synchronous access to a Member's Modes
	CHECK_POINTER(plMemberModes);
	
	*plMemberModes = 0L;

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::get_MemberModes");
	if (!m_pccsock->bGetNicknameFromVariant(&vMemberNickname, &szNickname))
		return HrThrowError(GetScodeFromHResult(m_pccsock->HrGetLastError()));

	ASSERT(szNickname, "szNickname is NULL in CChannelObject::get_MemberModes");
	if (!bQueryMemberProperty(szNickname, pnMemberModes, plMemberModes, NULL))
		hr = HrThrowError(GetScodeFromHResult(HrGetLastError()), NULL, szNickname);

	delete [] szNickname;

	return hr;
}


STDMETHODIMP CChannelObject::get_ChannelProperty(BSTR ChannelPropertyName, IChatItems **ppChatItems)
{
	OutputDebugThreadIdString("CChannelObject::get_ChannelProperty - Enter\n");

	CChatItems		*pCItems= NULL;
	CItemsObject	*pcitms = NULL;

	// Synchronous access to a Channel property
    CHECK_POINTER(ppChatItems);
	
	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (bQueryChannelProperty(TRUE, (LPCWSTR) ChannelPropertyName, &pCItems))
	{
		ASSERT(pCItems, "pCItems is NULL in CChannelObject::get_ChannelProperty");
		pcitms = (CItemsObject*) pCItems->GetAutObject();
		ASSERT(pcitms, "pcitms is NULL in CChannelObject::get_ChannelProperty");
		*ppChatItems = (IChatItems*) pcitms;
		return NOERROR;
	}
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CChannelObject::get_MemberProperty(BSTR MemberPropertyName, VARIANT vMemberNickname, IChatItems **ppChatItems)
{
	LPTSTR			szNickname = NULL;
	CChatItems		*pCItems= NULL;
	CItemsObject	*pcitms = NULL;

	OutputDebugThreadIdString("CChannelObject::get_MemberProperty - Enter\n");

	// Synchronous access to a Member property
    CHECK_POINTER(ppChatItems);
	
	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::get_MemberProperty");
	if (!m_pccsock->bGetNicknameFromVariant(&vMemberNickname, &szNickname))
		return HrThrowError(GetScodeFromHResult(m_pccsock->HrGetLastError()));

	ASSERT(szNickname, "szNickname is NULL in CChannelObject::get_MemberProperty");
	if (bQueryMemberProperty(TRUE, (LPCWSTR) MemberPropertyName, szNickname, &pCItems))
	{
		delete [] szNickname;
		ASSERT(pCItems, "pCItems is NULL in CChannelObject::get_MemberProperty");
		pcitms = (CItemsObject*) pCItems->GetAutObject();
		ASSERT(pcitms, "pcitms is NULL in CChannelObject::get_MemberProperty");
		*ppChatItems = (IChatItems*) pcitms;
		return NOERROR;
	}
	else
	{
		HRESULT hr = HrThrowError(GetScodeFromHResult(HrGetLastError()), NULL, szNickname);
		delete [] szNickname;
		return hr;
	}
}
