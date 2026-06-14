// MsChatPr.cpp : Implementation of CMsChatPr's methods
#include "StdAfx.H"
#include "MsChatPr.H"
#include "ChatPr.H"
#include "CItmsObj.H"
#include "Version.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CMsChatPr's methods


STDMETHODIMP CMsChatPr::AboutBox()
{
	TCHAR	szABTitle[g_nMaxLengthTitle], szABBodyF[g_nMaxLengthBody], szABBody[g_nMaxLengthBody];
	LPTSTR	rgszArg[1];
	
	rgszArg[0] = (LPTSTR) VER_PRODUCTVERSION_STR;

	if (!::LoadString(_Module.GetModuleInstance(), IDS_PR_OCXNAME,   szABTitle, g_nMaxLengthTitle-1) ||
		!::LoadString(_Module.GetModuleInstance(), IDS_PR_ABOUTMESS, szABBodyF, g_nMaxLengthBody-1))
		return HrThrowError(GetScodeFromHResult(GetLastError()));

	// Replace %1 by VER_PRODUCTVERSION_STR (4.3.2037 etc...)

// REGISB 06/26/97. Don't understand why the two first versions fail:
//	if (!FormatMessage(FORMAT_MESSAGE_FROM_HMODULE|FORMAT_MESSAGE_ARGUMENT_ARRAY,
//						_Module.GetModuleInstance(),
//						IDS_PR_ABOUTMESS,
//						0L,
//						szABBody,
//						g_nMaxLengthTitle,
//						rgszArg))
//		return HrThrowError(GetScodeFromHResult(GetLastError()));

//	TCHAR *pszOut = NULL;
//	if (!FormatMessage(
//					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
//					FORMAT_MESSAGE_FROM_HMODULE | 
//					FORMAT_MESSAGE_ARGUMENT_ARRAY,
//					_Module.GetModuleInstance(),
//					IDS_PR_ABOUTMESS, 
//					0L,
//					(TCHAR*) &pszOut, 
//					0L,
//					rgszArg)
//					)
//		return HrThrowError(GetScodeFromHResult(GetLastError()));

	if (!FormatMessage(FORMAT_MESSAGE_FROM_STRING|FORMAT_MESSAGE_ARGUMENT_ARRAY,
					   szABBodyF,
					   0, 
					   0L,
					   szABBody, 
					   g_nMaxLengthBody,
					   rgszArg))
		return HrThrowError(GetScodeFromHResult(GetLastError()));

    ::MessageBox(NULL, szABBody, szABTitle, MB_OK | MB_TASKMODAL | MB_ICONINFORMATION);
	return NOERROR;
}


STDMETHODIMP CMsChatPr::Connect(BSTR ServerName)
{
	OutputDebugThreadIdString("CMsChatPr::Connect - Enter\n");

	LPTSTR szAnsiServerName;

	if (!ServerName)
		return HrThrowError(CPR_E_INVALIDARG);
		
	USES_CONVERSION;
	if (!(szAnsiServerName = W2T(ServerName)))
		return HrThrowError(CTL_E_OUTOFMEMORY);

	#ifdef DEBUG
		sprintf(g_szDebugStr, "CMsChatPr::Connect ServerName:%s\n", szAnsiServerName);
		OutputDebugThreadIdString(g_szDebugStr);
	#endif // DEBUG

	// bConnect duplicates the stack mem szAnsiServerName
	if (!bConnect(szAnsiServerName, FALSE /*bLoginAlso*/))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	
	return NOERROR;
}


STDMETHODIMP CMsChatPr::Login(BSTR ServerName, BSTR Nickname, VARIANT UserName, VARIANT RealName, VARIANT Password, VARIANT SecurityPackages)
{
	LPTSTR		szAnsiServerName, szAnsiNickname = NULL, szUTF8Nickname = NULL, szTmp;
	SCODE		sc  = CTL_E_OUTOFMEMORY;
	HRESULT		hr	= NOERROR;
	VARIANT*	pvUN= &UserName;
	VARIANT*	pvRN= &RealName;
	VARIANT*	pvP = &Password;
	VARIANT*	pvSP= &SecurityPackages;
	TCHAR		szUserName[2*g_nMaxLengthSmall];
	DWORD		dwUserNameLen = 2*g_nMaxLengthSmall-1;

	OutputDebugThreadIdString("CMsChatPr::Login - Enter\n");

	USES_CONVERSION;

	if (bIsLoggedOn())
		return HrThrowError(CPR_E_ALREADYLOGGED);

	if (!ServerName || !Nickname)
	{
		sc = CPR_E_INVALIDARG;
		goto exit;
	}
		
	if (!(szAnsiServerName = W2T(ServerName)))
		goto exit;

	// Is the nickname provided extended?
	if (bExtendedWideNickname(Nickname))
		// Need to convert the nickname to UTF-8 string
		if (!bConvertWideStringToUTF8(Nickname, 0, &szUTF8Nickname, NULL, TRUE /*bNickname*/))
			goto exit;

	// Also store the Ansi string in case the server is an IRC one, or the nick is not extended
	if (!(szAnsiNickname = W2T(Nickname)))
		goto exit;

	if (pvUN->vt == (VT_BYREF+VT_VARIANT))
		pvUN = pvUN->pvarVal;

	if (pvRN->vt == (VT_BYREF+VT_VARIANT))
		pvRN = pvRN->pvarVal;

	if (pvP->vt == (VT_BYREF+VT_VARIANT))
		pvP = pvP->pvarVal;

	if (pvSP->vt == (VT_BYREF+VT_VARIANT))
		pvSP = pvSP->pvarVal;

	if (!pvUN || !((pvUN->vt == VT_BSTR) || (pvUN->vt == VT_EMPTY) || (pvUN->vt == VT_NULL) || (pvUN->vt == VT_ERROR)))
	{
		// the user name can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CMsChatPr::Login - Invalid VARIANT for UserName param\n");
		sc = CPR_E_BADUSERNAME;
		goto exit;
	}

	if (!pvRN || !((pvRN->vt == VT_BSTR) || (pvRN->vt == VT_EMPTY) || (pvRN->vt == VT_NULL) || (pvRN->vt == VT_ERROR)))
	{
		// the real name can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CMsChatPr::Login - Invalid VARIANT for RealName param\n");
		sc = CPR_E_BADREALNAME;
		goto exit;
	}

	if (!pvP || !((pvP->vt == VT_BSTR) || (pvP->vt == VT_EMPTY) || (pvP->vt == VT_NULL) || (pvP->vt == VT_ERROR)))
	{
		// the password can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CMsChatPr::Login - Invalid VARIANT for Password param\n");
		sc = CPR_E_BADPASSWORD;
		goto exit;
	}

	if (!pvSP || !((pvSP->vt == VT_BSTR) || (pvSP->vt == VT_EMPTY) || (pvSP->vt == VT_NULL) || (pvSP->vt == VT_ERROR)))
	{
		// the security packages can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CMsChatPr::Login - Invalid VARIANT for SecurityPackages param\n");
		sc = CPR_E_BADSECUPACK;
		goto exit;
	}

	ASSERT(!m_szUserName, "m_szUserName NOT NULL in CMsChatPr::Login");
	if (pvUN->vt == VT_BSTR)
	{
		if (pvUN->bstrVal && *pvUN->bstrVal)
		{
			// the caller specified a user name string
			if (!(szTmp = W2T(pvUN->bstrVal)) ||
				!bCopyStr(&m_szUserName, szTmp))
				goto exit;
		}
	}

	if (!m_szUserName)
	{
		// the user didn't specify a user name
		if (!::GetUserName(szUserName, &dwUserNameLen))
		{
			hr = GetLastError();
			if (ERROR_NOT_LOGGED_ON == hr)
				lstrcpy(szUserName, g_szNoUser);
			else
			{
				sc = GetScodeFromHResult(hr);
				goto exit;
			}
		}

		if (!bCopyStr(&m_szUserName, szUserName))
			goto exit;
	}

	ASSERT(!m_szRealName, "m_szRealName NOT NULL in CMsChatPr::Login");
	if (pvRN->vt == VT_BSTR)
	{
		if (pvRN->bstrVal && *pvRN->bstrVal)
		{
			// the caller specified a real name string
			if (!(szTmp = W2T(pvRN->bstrVal)) ||
				!bCopyStr(&m_szRealName, szTmp))
				goto exit;
		}
		//else m_szRealName is NULL
	}
	//else m_szRealName is NULL

	ASSERT(!m_szUserPwd, "m_szUserPwd NOT NULL in CMsChatPr::Login");
	if (pvP->vt == VT_BSTR)
	{
		if (pvP->bstrVal && *pvP->bstrVal)
		{
			// the caller specified a password string
			if (!(szTmp = W2T(pvP->bstrVal)) ||
				!bCopyStr(&m_szUserPwd, szTmp))
				goto exit;
		}
		//else m_szUserPwd is NULL
	}
	//else m_szUserPwd is NULL

	ASSERT(!m_szSecurity, "m_szSecurity NOT NULL in CMsChatPr::Login");
	if (pvSP->vt == VT_BSTR)
	{
		if (pvSP->bstrVal && *pvSP->bstrVal)
		{
			// the caller specified a security string
			if (!(szTmp = W2T(pvSP->bstrVal)) ||
				!bCopyStr(&m_szSecurity, szTmp))
				goto exit;
		} 
		else // security package is an empty string, we use the default value: ANON
			bCopyStr(&m_szSecurity, g_szDefaultSP);
	}
	else
		// no security string was specified, we use the default value: ANON
		bCopyStr(&m_szSecurity, g_szDefaultSP);

	// bSetNickname duplicates the stack mem szAnsiNickname
	if (!bSetNickname(szAnsiNickname, szUTF8Nickname, Nickname))
	{
		sc = GetScodeFromHResult(HrGetLastError());
		goto exit;
	}

	#ifdef DEBUG
		sprintf(g_szDebugStr, "CMsChatPr::Login ServerName:%s Nickname:%s UserName:%s RealName:%s Password:%s SecuPack:%s\n", szAnsiServerName, szAnsiNickname, SzGetUserName(), SzGetRealName() ? SzGetRealName() : "", SzGetUserPwd() ? SzGetUserPwd() : "", SzGetSecurity());
		OutputDebugThreadIdString(g_szDebugStr);
	#endif // DEBUG

	// bConnect duplicates the stack mem szAnsiServerName
	sc = bConnect(szAnsiServerName, TRUE /*bLoginAlso*/) ? S_OK : GetScodeFromHResult(HrGetLastError());

exit:
	return FAILED(sc) ? HrThrowError(sc) : NOERROR;
}


STDMETHODIMP CMsChatPr::Disconnect()
{
	HRESULT hr = NOERROR;

	OutputDebugThreadIdString("CMsChatPr::Disconnect\n");

	if (!bDisconnect())
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	
	return NOERROR;
}


STDMETHODIMP CMsChatPr::CancelConnectLogin()
{
	HRESULT hr = NOERROR;

	OutputDebugThreadIdString("CMsChatPr::CancelConnectLogin\n");

	if (!bCancelConnectLogin())
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	
	return NOERROR;
}


STDMETHODIMP CMsChatPr::ListChannels(ChatItems * ChannelQueryItems)
{
	OutputDebugThreadIdString("CMsChatPr::ListChannels - Enter\n");

	IChatItems		*pici = (IChatItems*) ChannelQueryItems;
	CItemsObject	*pcitms = (CItemsObject*) pici;

	if (bListChannels((CChatItems*) pcitms))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::QueryChannelProperty(BSTR ChannelPropertyName, VARIANT ChannelName)
{
	LPTSTR	szChannelName = NULL;

	// Asynchronously access a channel property
	OutputDebugThreadIdString("CMsChatPr::QueryChannelProperty - Enter\n");

	if (!ChannelPropertyName)
		return HrThrowError(CPR_E_INVALIDARG);

	// Check if the ChannelName param is OK, has to be unique open channel if empty
	if (!bGetChannelNameFromVariant(&ChannelName, &szChannelName))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	ASSERT(szChannelName, "szChannelName is NULL in CMsChatPr::QueryChannelProperty");

	BOOL bRet = bQueryChannelProperty(FALSE, (LPCWSTR) ChannelPropertyName, szChannelName, NULL);

	delete [] szChannelName;

	return bRet ? NOERROR : HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::KillChannel(BSTR ChannelName, VARIANT Reason)
{
	VARIANT *pvR = &Reason;
	LPTSTR	szAnsiReason = NULL;

	OutputDebugThreadIdString("CMsChatPr::KillChannel - Enter\n");

	if (!ChannelName)
		return HrThrowError(CPR_E_CHANNELBADNAME);

	if (pvR->vt == (VT_BYREF+VT_VARIANT))
		pvR = pvR->pvarVal;

	if (!pvR || !((pvR->vt == VT_BSTR) || (pvR->vt == VT_EMPTY) || (pvR->vt == VT_NULL) || (pvR->vt == VT_ERROR)))
	{
		// the reason can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CMsChatPr::KillChannel - Invalid VARIANT for Reason param\n");
		return HrThrowError(CPR_E_BADREASON);
	}

	USES_CONVERSION;

	if (pvR->vt == VT_BSTR && pvR->bstrVal && *pvR->bstrVal)
	{
		// the caller specified a non-null reason string
		if (!(szAnsiReason = W2T(pvR->bstrVal)))
			return HrThrowError(CTL_E_OUTOFMEMORY);
	}

	if (bKillTarget(ChannelName, szAnsiReason, TRUE))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()), W2T(ChannelName), NULL);
}


STDMETHODIMP CMsChatPr::ListUsers(ChatItems * UserQueryItems)
{
	OutputDebugThreadIdString("CMsChatPr::ListUsers - Enter\n");

	IChatItems		*pici = (IChatItems*) UserQueryItems;
	CItemsObject	*pcitms = (CItemsObject*) pici;

	if (bListUsers((CChatItems*) pcitms))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::QueryUserProperty(BSTR UserPropertyName, VARIANT Nickname)
{
	LPTSTR	szNickname = NULL;
	BOOL	bRet;

	OutputDebugThreadIdString("CMsChatPr::QueryUserProperty - Enter\n");

	if (!UserPropertyName)
		return HrThrowError(CPR_E_INVALIDARG);

	// Asynchronously access a user property
	if (!bGetNicknameFromVariant(&Nickname, &szNickname))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	ASSERT(szNickname, "szNickname is NULL in CChannelObject::QueryUserProperty");
	bRet = bQueryUserProperty(FALSE, (LPCWSTR) UserPropertyName, szNickname, NULL);

	delete [] szNickname;

	return bRet ? NOERROR : HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::ChangeUserProperty(BSTR UserPropertyName, VARIANT NewUserProperty, VARIANT Nickname)
{
	LPTSTR	szNickname = NULL;
	BOOL	bRet;

	OutputDebugThreadIdString("CMsChatPr::ChangeUserProperty - Enter\n");

	if (!UserPropertyName)
		return HrThrowError(CPR_E_INVALIDARG);

	// Asynchronously/Synchronously (client ignored flag) modify a user property
	if (!bGetNicknameFromVariant(&Nickname, &szNickname))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	ASSERT(szNickname, "szNickname is NULL in CChannelObject::ChangeUserProperty");
	bRet = bChangeUserProperty((LPCWSTR) UserPropertyName, &NewUserProperty, szNickname);

	delete [] szNickname;

	return bRet ? NOERROR : HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::IgnoreUsers(ChatItems * UserItems, BOOL Set)
{
	OutputDebugThreadIdString("CMsChatPr::IgnoreUsers - Enter\n");

	ASSERT(UserItems, "UserItems is NULL in CMsChatPr::IgnoreUsers");

	IChatItems		*pici = (IChatItems*) UserItems;
	CItemsObject	*pcitms = (CItemsObject*) pici;

	if (bIgnoreUsers((CChatItems*) pcitms, (BOOL) Set))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::BanUsers(ChatItems * UserItems, BOOL Set, VARIANT Reason, VARIANT Duration)
{
	OutputDebugThreadIdString("CMsChatPr::BanUsers - Enter\n");

	ASSERT(UserItems, "UserItems is NULL in CMsChatPr::BanUsers");

	IChatItems		*pici = (IChatItems*) UserItems;
	CItemsObject	*pcitms = (CItemsObject*) pici;
	VARIANT			*pvR = &Reason;
	VARIANT			*pvD = &Duration;
	LONG			lDuration;
	BOOL			bRet;

	if (pvR->vt == (VT_BYREF+VT_VARIANT))
		pvR = pvR->pvarVal;

	if (!pvR || !((pvR->vt == VT_BSTR) || (pvR->vt == VT_EMPTY) || (pvR->vt == VT_NULL) || (pvR->vt == VT_ERROR)))
	{
		// the reason can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CMsChatPr::BanUsers - Invalid VARIANT for Reason param\n");
		return HrThrowError(CPR_E_BADREASON);
	}

	if (pvD->vt == (VT_BYREF+VT_VARIANT))
		pvD = pvD->pvarVal;

	if (!pvD || !((pvD->vt == VT_I2) || (pvD->vt == VT_I4) || (pvD->vt == VT_EMPTY) || (pvD->vt == VT_NULL) || (pvD->vt == VT_ERROR)))
	{
		// the duration can only be empty, null, error or a integer or long
		OutputDebugThreadIdString("CMsChatPr::BanUsers - Invalid VARIANT for Duration param\n");
		return HrThrowError(CPR_E_INVALIDARG);
	}
	switch (pvD->vt)
	{
		case VT_I2:
			lDuration = (LONG) pvD->iVal;
			break;
		case VT_I4:
			lDuration = pvD->lVal;
			break;
		default:
			lDuration = -1L;
	}

	if (pvR->vt == VT_BSTR && pvR->bstrVal && *pvR->bstrVal)
	{
		USES_CONVERSION;
		LPTSTR szAnsiReason; 
		if (!(szAnsiReason = W2T(pvR->bstrVal)))
			return HrThrowError(CTL_E_OUTOFMEMORY);

		bRet = bBanUsers((CChatItems*) pcitms, (BOOL) Set, szAnsiReason, lDuration);
	}
	else
		bRet = bBanUsers((CChatItems*) pcitms, (BOOL) Set, NULL, lDuration);

	return bRet ? NOERROR : HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::KickUser(BSTR Nickname, VARIANT Reason)
{
	VARIANT	*pvR = &Reason;
	LPTSTR	szAnsiReason = NULL;

	OutputDebugThreadIdString("CMsChatPr::KickUser - Enter\n");

	if (!Nickname)
		return HrThrowError(CPR_E_INVALIDARG);

	if (pvR->vt == (VT_BYREF+VT_VARIANT))
		pvR = pvR->pvarVal;

	if (!pvR || !((pvR->vt == VT_BSTR) || (pvR->vt == VT_EMPTY) || (pvR->vt == VT_NULL) || (pvR->vt == VT_ERROR)))
	{
		// the reason can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CMsChatPr::KickUser - Invalid VARIANT for Reason param\n");
		return HrThrowError(CPR_E_BADREASON);
	}

	USES_CONVERSION;

	if (pvR->vt == VT_BSTR && pvR->bstrVal && *pvR->bstrVal)
	{
		if (!(szAnsiReason = W2T(pvR->bstrVal)))
			return HrThrowError(CTL_E_OUTOFMEMORY);
	}
	if (bKillTarget(Nickname, szAnsiReason, FALSE))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::ChangeNickname(BSTR NewNickname)
{
	OutputDebugThreadIdString("CMsChatPr::ChangeNickname\n");

	// NewNickname == NULL is taken care of in bChangeNickname

	if (bChangeNickname(NewNickname))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::SetAway(BOOL Away, VARIANT TextMessage)
{
	BOOL	bRet;
	VARIANT *pvTM = &TextMessage;

	OutputDebugThreadIdString("CMsChatPr::SetAway\n");

	if (pvTM->vt == (VT_BYREF+VT_VARIANT))
		pvTM = pvTM->pvarVal;

	if (!pvTM || 
		!((pvTM->vt == VT_BSTR) || (pvTM->vt == VT_EMPTY) || (pvTM->vt == VT_NULL) || (pvTM->vt == VT_ERROR)) ||
		(Away && (pvTM->vt != VT_BSTR)))
	{
		// the text message can only be empty, null, error or a BSTR
		OutputDebugThreadIdString("CMsChatPr::SetAway - Invalid VARIANT for TextMessage param\n");
		return HrThrowError(CPR_E_INVALIDAWAYMESSAGE);
	}

	if (!Away)
		bRet = bSetAway(NULL);
	else
	{
		ASSERT(pvTM->vt == VT_BSTR, "pvTM->vt != VT_BSTR in CMsChatPr::SetAway");
		if (pvTM->bstrVal && *pvTM->bstrVal)
		{
			// the caller specified a text message
			USES_CONVERSION;
			LPTSTR szAnsiTM; 
			if (!(szAnsiTM = W2T(pvTM->bstrVal)))
				return HrThrowError(CTL_E_OUTOFMEMORY);

			bRet = bSetAway(szAnsiTM);
		}
		else
			bRet = bSetAway(g_szEmpty);
	}

	return bRet ? NOERROR : HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::QueryServerProperty(BSTR ServerPropertyName)
{
	OutputDebugThreadIdString("CMsChatPr::QueryServerProperty - Enter\n");

	ASSERT(ServerPropertyName, "ServerPropertyName is NULL in CMsChatPr::QueryServerProperty");

	// Asynchronously access a server property
	// ServerPropertyName == NULL is taken care of in bQueryServerProperty
	if (bQueryServerProperty(FALSE, (LPCWSTR) ServerPropertyName, NULL))
		return NOERROR;
	else
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::SendInvitation(BSTR Nickname, VARIANT ChannelName)
{
	LPTSTR szChannelName = NULL;

	OutputDebugThreadIdString("CMsChatPr::SendInvitation - Enter\n");

	if (!Nickname)
		return HrThrowError(CPR_E_BADNICKNAME);

	// ChannelName can be empty if there is only one open channel in the collection
	if (!bGetChannelNameFromVariant(&ChannelName, &szChannelName))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	ASSERT(szChannelName, "szChannelName is NULL in CMsChatPr::SendInvitation");

	BOOL bRet = bSendInvitation(Nickname, szChannelName);

	delete [] szChannelName;

	return bRet ? NOERROR : HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::SendPrivateMessage(long PrivateMessageType, VARIANT Message, VARIANT RecipientNicknames, VARIANT DataMessageTag)
{
	OutputDebugThreadIdString("CMsChatPr::SendPrivateMessage - Enter\n");

	return bSendPrivateMessage(PrivateMessageType, &Message, &RecipientNicknames, &DataMessageTag) ? NOERROR : HrThrowError(GetScodeFromHResult(HrGetLastError()));
}


STDMETHODIMP CMsChatPr::SendProtocolMessage(BSTR TextMessage)
{
	OutputDebugThreadIdString("CMsChatPr::SendProtocolMessage - Enter\n");

	if (!TextMessage)
		return HrThrowError(CPR_E_INVALIDARG);

	USES_CONVERSION;
	LPTSTR szAnsiTextMessage; 
	if (!(szAnsiTextMessage = W2T(TextMessage)))
		return HrThrowError(CTL_E_OUTOFMEMORY);

	if (!bSendProtocolText(szAnsiTextMessage))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));

	return NOERROR;
}

