// ChatPrPr.cpp : Implementation of CMsChatPr's Properties
#include "StdAfx.H"
#include "MsChatPr.H"
#include "ChatPr.H"
#include "CItmsObj.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CMsChatPr's properties

STDMETHODIMP CMsChatPr::get_ConnectionState(enumConnectionState *pcs)
{
	HRESULT hr = NOERROR;

	OutputDebugThreadIdString("CMsChatPr::get_ConnectionState - Enter\n");

    CHECK_POINTER(pcs);

    *pcs = (enumConnectionState) CsGetState();

    return NOERROR;
}


STDMETHODIMP CMsChatPr::get_ConnectionType(enumConnectionType *pct)
{
	HRESULT hr = NOERROR;

	OutputDebugThreadIdString("CMsChatPr::get_ConnectionType - Enter\n");

    CHECK_POINTER(pct);

	if (CsGetState() < csConnected)
		return HrThrowError(CPR_E_NOTCONNECTED);

	if (bIsIrcXSocket())
		*pct = ctIRCX;
	else
		*pct = ctIRC;

    return NOERROR;
}


STDMETHODIMP CMsChatPr::get_OpeningChannels(long *plOpeningChannels)
{
	HRESULT hr = NOERROR;

	OutputDebugThreadIdString("CMsChatPr::get_OpeningChannels - Enter\n");

    CHECK_POINTER(plOpeningChannels);

    *plOpeningChannels = LGetOpeningChannels();

    return NOERROR;
}


STDMETHODIMP CMsChatPr::get_PropertyAccessTimeOut(long *plPropertyAccessTimeOut)
{
	OutputDebugThreadIdString("CMsChatPr::get_PropertyAccessTimeOut - Enter\n");

    CHECK_POINTER(plPropertyAccessTimeOut);

	*plPropertyAccessTimeOut = m_dwPropertyAccessTimeOut;

    return NOERROR;
}


STDMETHODIMP CMsChatPr::put_PropertyAccessTimeOut(long lPropertyAccessTimeOut)
{
	OutputDebugThreadIdString("CMsChatPr::put_PropertyAccessTimeOut - Enter\n");

	if (lPropertyAccessTimeOut < 0L)
		return HrThrowError(CTL_E_INVALIDPROPERTYVALUE);

	m_dwPropertyAccessTimeOut = (DWORD) lPropertyAccessTimeOut;

	return NOERROR;
}


STDMETHODIMP CMsChatPr::get_ProcessProtocolMessages(BOOL *pbProcessProtocolMessages)
{
	OutputDebugThreadIdString("CMsChatPr::get_ProcessProtocolMessages - Enter\n");

    CHECK_POINTER(pbProcessProtocolMessages);

	*pbProcessProtocolMessages = m_bProcessProtocolMessages;

    return NOERROR;
}


STDMETHODIMP CMsChatPr::put_ProcessProtocolMessages(BOOL bProcessProtocolMessages)
{
	OutputDebugThreadIdString("CMsChatPr::put_ProcessProtocolMessages - Enter\n");

	m_bProcessProtocolMessages = bProcessProtocolMessages;

	return NOERROR;
}


STDMETHODIMP CMsChatPr::get_ConvertedString(enumConversionType ConversionType, enumConversionSource ConversionSource, BSTR Source, BOOL PostProcess, BSTR *pbstrDestination)
{
	OutputDebugThreadIdString("CMsChatPr::get_ConvertedString - Enter\n");

    CHECK_POINTER(pbstrDestination);

	// Make sure the ConversionType and ConversionSource params are valid
	if (ConversionType < cvtToUTF8 ||
		ConversionType > cvtToUnicode ||
		ConversionSource < cvsNormal ||
		ConversionSource > cvsChannelName ||
		!Source)
		return HrThrowError(CPR_E_INVALIDARG);

	if (cvsChannelName == ConversionSource)
	{
		// Source has to start with '#', '&', '%#' or '%&'
		if (
			(
			 Source[0] != L'%' &&
			 Source[0] != L'#' &&
			 Source[0] != L'&'
			) ||
			(
			 Source[0] == L'%' &&
			 Source[1] != L'#' &&
			 Source[1] != L'&'
			)
		   )
			return HrThrowError(CPR_E_CHANNELBADNAME);
	}

	if (cvtToUTF8 == ConversionType)
	{
		LPTSTR	szOutStr;
		if (!bConvertWideStringToUTF8((LPCWSTR) Source, 0 /*cchIn*/, &szOutStr, NULL /*pcchOut*/, cvsNickname == ConversionSource /*bNickname*/, cvsChannelName == ConversionSource /*bChannelName*/, PostProcess /*bPostProcess*/))
			goto oom;
		
		ASSERT(szOutStr, "szOutStr is NULL in CMsChatPr::get_ConvertedString");
		
		*pbstrDestination = A2BSTR(szOutStr);
		delete [] szOutStr;
	}
	else
	{
		ASSERT(cvtToUnicode == ConversionType, "Unexpected ConversionType param in CMsChatPr::get_ConvertedString");
		LPCTSTR	szInStr;
		LPWSTR	wszOutStr;

		USES_CONVERSION;
		if (!(szInStr = W2T(Source)))
			goto oom;

		if (cvsNickname == ConversionSource && g_chExtNckPfx != *szInStr)
			return HrThrowError(CPR_E_BADNICKNAME);

		if (!bConvertUTF8StringToWide(szInStr, 0 /*cchIn*/, &wszOutStr, NULL /*pcchOut*/, cvsNickname == ConversionSource /*bNickname*/, cvsChannelName == ConversionSource /*bChannelName*/, PostProcess /*bPostProcess*/))
			goto oom;

		ASSERT(wszOutStr, "wszOutStr is NULL in CMsChatPr::get_ConvertedString");

		*pbstrDestination = SysAllocString(wszOutStr);
		delete [] wszOutStr;
	}
	if (!*pbstrDestination)
		goto oom;

    return NOERROR;

oom:
    return HrThrowError(CTL_E_OUTOFMEMORY);
}


STDMETHODIMP CMsChatPr::get_Channels(IDispatch **ppdispChannels)
{
	OutputDebugThreadIdString("CMsChatPr::get_Channels - Enter\n");

    CHECK_POINTER(ppdispChannels);

	if (m_pccdisp)
	{
		m_pccdisp->AddRef();
		*ppdispChannels = m_pccdisp;
		return NOERROR;
	}
	else
	{
		*ppdispChannels = NULL;
		return E_NOINTERFACE;
	}
}


STDMETHODIMP CMsChatPr::get_NewChatItems(IChatItems **ppChatItems)
{
	OutputDebugThreadIdString("CMsChatPr::get_NewChatItems - Enter\n");

    CHECK_POINTER(ppChatItems);

	CChatItems		*pCItems = NULL;
	CItemsObject	*pcitms  = NULL;
	
	*ppChatItems = NULL;

	if (!bCreateChatItems(&pCItems))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::get_NewChatItems");

	pcitms = (CItemsObject*) pCItems->GetAutObject();
	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::get_NewChatItems");

	*ppChatItems = (IChatItems*) pcitms;

	return NOERROR;
}


STDMETHODIMP CMsChatPr::get_MaxStringLength(enumStringType StringType, long *plMaxStringLength)
{
	OutputDebugThreadIdString("CMsChatPr::get_MaxStringLength - Enter\n");

    CHECK_POINTER(plMaxStringLength);

	switch (StringType)
	{
		case stServerMessage:
			*plMaxStringLength = g_lMaxMsgLength;
			break;

		case stChannelAccount:
		case stChannelHostKey:
		case stChannelKeyword:
		case stChannelLanguage:
		case stChannelSubject:
			*plMaxStringLength = 31;
			break;

		case stChannelTopic:
			*plMaxStringLength = 160;
			break;

		case stChannelName:
			*plMaxStringLength = 200;
			break;

		case stChannelClientData:
		case stChannelOnJoin:
		case stChannelOnPart:
		case stChannelOwnerKey:
		case stChannelRating:
			*plMaxStringLength = 255;
			break;

		default:
			return HrThrowError(GetScodeFromHResult(E_INVALIDARG), NULL, NULL);
	}

    return NOERROR;
}


STDMETHODIMP CMsChatPr::get_ServerName(BSTR *pbstrServerName)
{
	OutputDebugThreadIdString("CMsChatPr::get_ServerName - Enter\n");

    CHECK_POINTER(pbstrServerName);

	*pbstrServerName = SzGetServerName() ? A2BSTR(SzGetServerName()) : SysAllocString(L"");

    return (*pbstrServerName) ? NOERROR : HrThrowError(CTL_E_OUTOFMEMORY);
}


STDMETHODIMP CMsChatPr::get_Nickname(BSTR *pbstrMemberNickname)
{
	OutputDebugThreadIdString("CMsChatPr::get_MemberNickname - Enter\n");

    CHECK_POINTER(pbstrMemberNickname);

	*pbstrMemberNickname = BstrGetNickname() ? SysAllocString(BstrGetNickname()) : SysAllocString(L"");

    return (*pbstrMemberNickname) ? NOERROR : HrThrowError(CTL_E_OUTOFMEMORY);
}


STDMETHODIMP CMsChatPr::get_UserProperty(BSTR UserPropertyName, VARIANT Nickname, IChatItems **ppChatItems)
{
	OutputDebugThreadIdString("CMsChatPr::get_UserProperty - Enter\n");

	LPTSTR			szNickname = NULL;
	CChatItems		*pCItems= NULL;
	CItemsObject	*pcitms = NULL;
	VARIANT			*pvN	= &Nickname;

	// Synchronous access to a user property
    CHECK_POINTER(ppChatItems);
	
	if (!bGetNicknameFromVariant(&Nickname, &szNickname))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	ASSERT(szNickname, "szNickname is NULL in CMsChatPr::get_UserProperty");
	if (bQueryUserProperty(TRUE, (LPCWSTR) UserPropertyName, szNickname, &pCItems))
	{
		delete [] szNickname;
		ASSERT(pCItems, "pCItems is NULL in CMsChatPr::get_UserProperty");
		pcitms = (CItemsObject*) pCItems->GetAutObject();
		ASSERT(pcitms, "pcitms is NULL in CMsChatPr::get_UserProperty");
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


STDMETHODIMP CMsChatPr::get_ChannelProperty(BSTR ChannelPropertyName, VARIANT ChannelName, IChatItems **ppChatItems)
{
	OutputDebugThreadIdString("CMsChatPr::get_ChannelProperty - Enter\n");

	LPTSTR			szChannelName = NULL;
	CChatItems		*pCItems= NULL;
	CItemsObject	*pcitms = NULL;

	// Synchronous access to a channel property
    CHECK_POINTER(ppChatItems);
	
	if (!bGetChannelNameFromVariant(&ChannelName, &szChannelName))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	ASSERT(szChannelName, "szChannelName is NULL in CMsChatPr::get_ChannelProperty");
	if (bQueryChannelProperty(TRUE, (LPCWSTR) ChannelPropertyName, szChannelName, &pCItems))
	{
		delete [] szChannelName;
		ASSERT(pCItems, "pCItems is NULL in CMsChatPr::get_ChannelProperty");
		pcitms = (CItemsObject*) pCItems->GetAutObject();
		ASSERT(pcitms, "pcitms is NULL in CMsChatPr::get_ChannelProperty");
		*ppChatItems = (IChatItems*) pcitms;
		return NOERROR;
	}
	else
	{
		HRESULT hr = HrThrowError(GetScodeFromHResult(HrGetLastError()), szChannelName, NULL);
		delete [] szChannelName;
		return hr;
	}
}


STDMETHODIMP CMsChatPr::get_ServerProperty(BSTR ServerPropertyName, IChatItems **ppChatItems)
{
	OutputDebugThreadIdString("CMsChatPr::get_ServerProperty - Enter\n");

	CChatItems		*pCItems = NULL;
	CItemsObject	*pcitms = NULL;

	// Synchronous access to a server property
    CHECK_POINTER(ppChatItems);
	
	if (bQueryServerProperty(TRUE, (LPCWSTR) ServerPropertyName, &pCItems))
	{
		ASSERT(pCItems, "pCItems is NULL in CMsChatPr::get_ServerProperty");
		pcitms = (CItemsObject*) pCItems->GetAutObject();
		ASSERT(pcitms, "pcitms is NULL in CMsChatPr::get_ServerProperty");
		*ppChatItems = (IChatItems*) pcitms;
		return NOERROR;
	}
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}
