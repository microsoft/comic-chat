// ChMe.cpp : Implementation of CChannelObject's methods
#include "StdAfx.H"
#include "MsChatPr.H"
#include "ChObj.H"
#include "IrcMsg.H"
#include "CCSock.H"
#include "CItmsObj.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CChannelObject's methods

STDMETHODIMP CChannelObject::CreateChannel(VARIANT ChannelName, VARIANT ChannelKeyword, VARIANT ChannelModes, VARIANT MaxMemberCount)
{
	LPTSTR		szTmp, szChannelKey = NULL;
	HRESULT		hr    = NOERROR;
	VARIANT*	pvCN  = &ChannelName;
	VARIANT*	pvCK  = &ChannelKeyword;
	VARIANT*	pvCM  = &ChannelModes;
	VARIANT*	pvMMC = &MaxMemberCount;

	OutputDebugThreadIdString("CChannelObject::CreateChannel\n");

	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::CreateChannel");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	// Make sure that the channel is closed
	switch (m_chs)
	{
		case chsClosed:
			break;

		case chsOpening:
			return HrThrowError(CPR_E_ALREADYOPENING);
		
		case chsOpen:
			return HrThrowError(CPR_E_ALREADYOPEN);
	}

	if (pvCN->vt == (VT_BYREF+VT_VARIANT))
		pvCN = pvCN->pvarVal;
	if (pvCK->vt == (VT_BYREF+VT_VARIANT))
		pvCK = pvCK->pvarVal;
	if (pvCM->vt == (VT_BYREF+VT_VARIANT))
		pvCM = pvCM->pvarVal;
	if (pvMMC->vt == (VT_BYREF+VT_VARIANT))
		pvMMC = pvMMC->pvarVal;

	if (!pvCN || !((pvCN->vt == VT_BSTR) || (pvCN->vt == VT_EMPTY) || (pvCN->vt == VT_NULL) || (pvCN->vt == VT_ERROR)))
	{
		// the channel name can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CChannelObject::CreateChannel - Invalid VARIANT for ChannelName param\n");
		return HrThrowError(CPR_E_CHANNELBADNAME);
	}

	if (!pvCK || !((pvCK->vt == VT_BSTR) || (pvCK->vt == VT_EMPTY) || (pvCK->vt == VT_NULL) || (pvCK->vt == VT_ERROR)))
	{
		// the channel keyword can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CChannelObject::CreateChannel - Invalid VARIANT for ChannelKeyword param\n");
		return HrThrowError(CPR_E_CHANNELBADKEY);
	}

	if (!pvCM || !((pvCM->vt == VT_I2) || (pvCM->vt == VT_I4) || (pvCM->vt == VT_EMPTY) || (pvCM->vt == VT_NULL) || (pvCM->vt == VT_ERROR)))
	{
		// the channel modes can only be empty, null, error, long or short
		OutputDebugThreadIdString("CChannelObject::CreateChannel - Invalid VARIANT for ChannelModes param\n");
		return HrThrowError(CPR_E_CHANNELBADMODES);
	}

	if (!pvMMC || !((pvMMC->vt == VT_I2) || (pvMMC->vt == VT_I4) || (pvMMC->vt == VT_EMPTY) || (pvMMC->vt == VT_NULL) || (pvMMC->vt == VT_ERROR)))
	{
		// the channel maxmembercount can only be empty, null, error, long or short
		OutputDebugThreadIdString("CChannelObject::CreateChannel - Invalid VARIANT for MaxMemberCount param\n");
		return HrThrowError(CPR_E_BADMAXMEMBERCOUNT);
	}

	USES_CONVERSION;

	if (pvCN->vt == VT_BSTR && pvCN->bstrVal && *pvCN->bstrVal)
	{
		// Check if user wants to join an extended channel and convert
		if (m_pccsock->bIsIrcXSocket())
		{
			if (L'%' == pvCN->bstrVal[0])
			{
				if (m_prChannel.szName)
					delete [] m_prChannel.szName;
				if (!bConvertWideStringToUTF8(pvCN->bstrVal, 0, &m_prChannel.szName, NULL, FALSE /*bNickname*/, TRUE /*bChannelName*/))
					return HrThrowError(CTL_E_OUTOFMEMORY);
			}
			else
			{
				LPTSTR	szConvertedChannelName;
				BOOL	bFreeConverted;
	
				if (!(szTmp = W2T(pvCN->bstrVal)) ||
					!bConvertString(FALSE, m_pccsock->GetCharSet(), szTmp, 0, &szConvertedChannelName, NULL, &bFreeConverted) ||
					!bCopyChanStr(&(m_prChannel.szName), szConvertedChannelName))
					return HrThrowError(CTL_E_OUTOFMEMORY);

				if (bFreeConverted)
					delete [] szConvertedChannelName;
			}
		}
		else
		{
			if (!(szTmp = W2T(pvCN->bstrVal)) ||
				!bCopyChanStr(&(m_prChannel.szName), szTmp))
				return HrThrowError(CTL_E_OUTOFMEMORY);
		}
	}
	//else we use the current ChannelName property content

	ASSERT(!m_prChannel.szKeyword, "m_prChannel.szKeyword is NOT NULL in CChannelObject::CreateChannel");
	if (VT_BSTR == pvCK->vt && pvCK->bstrVal && *pvCK->bstrVal)
		if (!(szChannelKey = W2T(pvCK->bstrVal)))
			return HrThrowError(CTL_E_OUTOFMEMORY);

	switch (pvCM->vt)
	{
	case VT_I2:
	case VT_I4:
		if (!bIsChannelModesValid((pvCM->vt == VT_I2) ? pvCM->iVal : pvCM->lVal))
			return HrThrowError(CPR_E_CHANNELBADMODES);
		m_prChannel.lModes = (pvCM->vt == VT_I2) ? pvCM->iVal : pvCM->lVal;
	//	break;
	//default:
	//	use the current property value
	}

	switch (pvMMC->vt)
	{
	case VT_I2:
	case VT_I4:
		if (!bMaxMemberCountValid((pvMMC->iVal == VT_I2) ? pvMMC->iVal : pvMMC->lVal))
			return HrThrowError(CPR_E_BADMAXMEMBERCOUNT);
		m_prChannel.lMaxMemberCount = (pvMMC->iVal == VT_I2) ? pvMMC->iVal : pvMMC->lVal;
		break;
	default:
		// use the default server max member count
		m_prChannel.lMaxMemberCount = -1L;
	}

	if (!m_pccsock->bCreateJoinChannel((CChatChannel*) this, szChannelKey, TRUE /*bCreate*/))
		return HrThrowError(GetScodeFromHResult(m_pccsock->HrGetLastError()), NULL, m_pccsock->SzGetNickname());

	return NOERROR;
}


STDMETHODIMP CChannelObject::JoinChannel(VARIANT ChannelName, VARIANT ChannelKeyword)
{
	LPTSTR		szTmp, szChannelKey = NULL;
	HRESULT		hr    = NOERROR;
	VARIANT*	pvCN  = &ChannelName;
	VARIANT*	pvCK  = &ChannelKeyword;

	OutputDebugThreadIdString("CChannelObject::JoinChannel\n");

	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::JoinChannel");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	// Make sure that the channel is closed
	switch (m_chs)
	{
		case chsClosed:
			break;

		case chsOpening:
			return HrThrowError(CPR_E_ALREADYOPENING);
		
		case chsOpen:
			return HrThrowError(CPR_E_ALREADYOPEN);
	}

	if (pvCN->vt == (VT_BYREF+VT_VARIANT))
		pvCN = pvCN->pvarVal;
	if (pvCK->vt == (VT_BYREF+VT_VARIANT))
		pvCK = pvCK->pvarVal;

	if (!pvCN || !((pvCN->vt == VT_BSTR) || (pvCN->vt == VT_EMPTY) || (pvCN->vt == VT_NULL) || (pvCN->vt == VT_ERROR)))
	{
		// the channel name can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CChannelObject::JoinChannel - Invalid VARIANT for ChannelName param\n");
		return HrThrowError(CPR_E_CHANNELBADNAME);
	}

	if (!pvCK || !((pvCK->vt == VT_BSTR) || (pvCK->vt == VT_EMPTY) || (pvCK->vt == VT_NULL) || (pvCK->vt == VT_ERROR)))
	{
		// the channel key can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CChannelObject::JoinChannel - Invalid VARIANT for ChannelKey param\n");
		return HrThrowError(CPR_E_CHANNELBADKEY);
	}

	USES_CONVERSION;
	
	if (pvCN->vt == VT_BSTR && pvCN->bstrVal && *pvCN->bstrVal)
	{
		// Check if user wants to join an extended channel and convert
		if (m_pccsock->bIsIrcXSocket())
		{
			if (L'%' == pvCN->bstrVal[0])
			{
				if (m_prChannel.szName)
					delete [] m_prChannel.szName;
				if (!bConvertWideStringToUTF8(pvCN->bstrVal, 0, &m_prChannel.szName, NULL, FALSE /*bNickname*/, TRUE /*bChannelName*/))
					return HrThrowError(CTL_E_OUTOFMEMORY);
			}
			else
			{
				LPTSTR	szConvertedChannelName;
				BOOL	bFreeConverted;
		
				if (!(szTmp = W2T(pvCN->bstrVal)) ||
					!bConvertString(FALSE, m_pccsock->GetCharSet(), szTmp, 0, &szConvertedChannelName, NULL, &bFreeConverted) ||
					!bCopyChanStr(&(m_prChannel.szName), szConvertedChannelName))
					return HrThrowError(CTL_E_OUTOFMEMORY);

				if (bFreeConverted)
					delete [] szConvertedChannelName;
			}
		}
		else
		{
			if (!(szTmp = W2T(pvCN->bstrVal)) ||
				!bCopyChanStr(&(m_prChannel.szName), szTmp))
				return HrThrowError(CTL_E_OUTOFMEMORY);
		}
	}
	// else we use the current ChannelName property content

	ASSERT(!m_prChannel.szKeyword, "m_prChannel.szKeyword is NOT NULL in CChannelObject::JoinChannel");
	if (VT_BSTR == pvCK->vt && pvCK->bstrVal && *pvCK->bstrVal)
		if (!(szChannelKey = W2T(pvCK->bstrVal)))
			return HrThrowError(CTL_E_OUTOFMEMORY);

	if (!m_pccsock->bCreateJoinChannel((CChatChannel*) this, szChannelKey, FALSE /*bCreate*/))
		return HrThrowError(GetScodeFromHResult(m_pccsock->HrGetLastError()), NULL, m_pccsock->SzGetNickname());

	return NOERROR;
}


STDMETHODIMP CChannelObject::LeaveChannel()
{
	OutputDebugThreadIdString("CChannelObject::LeaveChannel\n");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (!bLeaveChannel())
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	return NOERROR;
}


STDMETHODIMP CChannelObject::SendMessage(long MessageType, VARIANT Message, VARIANT RecipientNicknames, VARIANT DataMessageTag)
{
	OutputDebugThreadIdString("CChannelObject::SendMessage - Enter\n");
  
 	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::SendMessage");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (m_chs != chsOpen)
		return HrThrowError(CPR_E_NOTOPEN);

	return bSendMessage(MessageType, &Message, &RecipientNicknames, &DataMessageTag) ? NOERROR : HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CChannelObject::SendInvitation(BSTR Nickname)
{
	OutputDebugThreadIdString("CChannelObject::SendInvitation - Enter\n");

	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::SendInvitation");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (m_chs != chsOpen)
		return HrThrowError(CPR_E_NOTOPEN);

	if (!Nickname)
		return HrThrowError(CPR_E_BADNICKNAME);

	if (m_pccsock->bSendInvitation(Nickname, m_prChannel.szName))
		return TRUE;
	else
		return HrThrowError(GetScodeFromHResult(m_pccsock->HrGetLastError()));
}


STDMETHODIMP CChannelObject::ListMembers(ChatItems * MemberQueryItems)
{
	OutputDebugThreadIdString("CChannelObject::ListMembers - Enter\n");

	IChatItems		*pici = (IChatItems*) MemberQueryItems;
	CItemsObject	*pcitms = (CItemsObject*) pici;

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (bListMembers((CChatItems*) pcitms))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CChannelObject::QueryMemberProperty(BSTR MemberPropertyName, VARIANT MemberNickname)
{
	HRESULT hr = NOERROR;
	LPTSTR	szNickname = NULL;

	OutputDebugThreadIdString("CChannelObject::QueryMemberProperty - Enter\n");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	// Asynchronously access a member property
	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::QueryMemberProperty");
	if (!m_pccsock->bGetNicknameFromVariant(&MemberNickname, &szNickname))
		return HrThrowError(GetScodeFromHResult(m_pccsock->HrGetLastError()));

	ASSERT(szNickname, "szNickname is NULL in CChannelObject::QueryMemberProperty");
	if (!bQueryMemberProperty(FALSE, (LPCWSTR) MemberPropertyName, szNickname, NULL))
		hr = HrThrowError(GetScodeFromHResult(HrGetLastError()), NULL, szNickname);

	delete [] szNickname;

	return hr;
}


STDMETHODIMP CChannelObject::ChangeMemberProperty(BSTR MemberPropertyName, VARIANT NewMemberProperty, VARIANT MemberNickname)
{
	HRESULT	hr = NOERROR;
	LPTSTR	szNickname = NULL;

	OutputDebugThreadIdString("CChannelObject::ChangeMemberProperty - Enter\n");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	// Asynchronously change a Member property
	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::ChangeMemberProperty");
	if (!m_pccsock->bGetNicknameFromVariant(&MemberNickname, &szNickname))
		return HrThrowError(GetScodeFromHResult(m_pccsock->HrGetLastError()));

	ASSERT(szNickname, "szNickname is NULL in CChannelObject::ChangeMemberProperty");
	if (!bChangeMemberProperty((LPCWSTR) MemberPropertyName, &NewMemberProperty, szNickname))
	{
		if (E_INVALIDARG == (hr = HrGetLastError()))
			hr = HrThrowError(CTL_E_INVALIDPROPERTYVALUE);
		else
			hr = HrThrowError(GetScodeFromHResult(hr), NULL, szNickname);
	}

	delete [] szNickname;

	return hr;
}


STDMETHODIMP CChannelObject::BanMembers(ChatItems * MemberItems, BOOL Set)
{
	OutputDebugThreadIdString("CChannelObject::BanMembers\n");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	IChatItems		*pici = (IChatItems*) MemberItems;
	CItemsObject	*pcitms = (CItemsObject*) pici;

	if (bBanMembers((CChatItems*) pcitms, (BOOL) Set))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CChannelObject::KickMember(BSTR MemberNickname, VARIANT Reason)
{
	VARIANT	*pvReason  = &Reason;
	LPTSTR	szAnsiReason = NULL;	

	OutputDebugThreadIdString("CChannelObject::KickMember\n");

	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelObject::KickMember");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	if (m_chs != chsOpen)
		return HrThrowError(CPR_E_NOTOPEN);

	if (!MemberNickname)
		return HrThrowError(CPR_E_BADNICKNAME);

	if (pvReason->vt == (VT_BYREF+VT_VARIANT))
		pvReason = pvReason->pvarVal;

	if (!pvReason || !((pvReason->vt == VT_BSTR) || (pvReason->vt == VT_EMPTY) || (pvReason->vt == VT_NULL) || (pvReason->vt == VT_ERROR)))
		// the reason can only be empty, null, error or a BSTR
		return HrThrowError(CPR_E_BADREASON);

	USES_CONVERSION;

	if (pvReason->vt == VT_BSTR && pvReason->bstrVal && *pvReason->bstrVal)
	{
		if (!(szAnsiReason = W2T(pvReason->bstrVal)))
			return HrThrowError(CTL_E_OUTOFMEMORY);
	}

	if (bKickMember(MemberNickname, szAnsiReason))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()), NULL, W2T(MemberNickname));
}


STDMETHODIMP CChannelObject::QueryChannelProperty(BSTR ChannelPropertyName)
{
	OutputDebugThreadIdString("CChannelObject::QueryChannelProperty - Enter\n");

	ASSERT(ChannelPropertyName, "ChannelPropertyName is NULL in CChannelObject::QueryChannelProperty");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	// Asynchronously access a Channel property
	if (bQueryChannelProperty(FALSE, (LPCWSTR) ChannelPropertyName, NULL))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CChannelObject::ChangeChannelProperty(BSTR ChannelPropertyName, VARIANT NewChannelProperty)
{
	OutputDebugThreadIdString("CChannelObject::ChangeChannelProperty - Enter\n");

	ASSERT(ChannelPropertyName, "ChannelPropertyName is NULL in CChannelObject::ChangeChannelProperty");

	if (!m_bInCollection)
		return HrThrowError(CPR_E_CHANOBJ_DELETED);

	// Asynchronously change a Channel property
	if (bChangeChannelProperty((LPCWSTR) ChannelPropertyName, &NewChannelProperty))
		return NOERROR;
	else
	{
		if (E_INVALIDARG == HrGetLastError())
		{
			HrThrowError(CTL_E_INVALIDPROPERTYVALUE);
			return E_INVALIDARG;
		}
		else
			return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	}
}
