//////////////////////////////////////////////////////////////////////////////
// CProxy_IMsChatPrEvent
template <class T>
class CProxy_IMsChatPrEvent : public IConnectionPointImpl<T, &DIID__IMsChatPrEvent, CComDynamicUnkArray>
{
public:
//methods:
//_IMsChatPrEvent : IDispatch
public:
	void Fire_OnConnectionState(LONG lNewConnectionState)
	{
		VARIANTARG* pvars = new VARIANTARG;
		VariantInit(pvars);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars->vt = VT_I4;
				pvars->lVal= lNewConnectionState;
				DISPPARAMS disp = { pvars, NULL, 1, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x6E, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete pvars;
	}
	
	void Fire_OnConnectionError(LONG lErrorCode, 
								LPCSTR szDescription, 
								VARIANT* pvChannelName)
	{
		VARIANTARG* pvars = new VARIANTARG[3];
		for (int i = 0; i < 3; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[2].vt = VT_I4;
				pvars[2].lVal= lErrorCode;
				pvars[1].vt = VT_BSTR;
				pvars[1].bstrVal= A2BSTR(szDescription);
				pvars[0].vt = VT_VARIANT|VT_BYREF;
				pvars[0].pvarVal = pvChannelName;
				DISPPARAMS disp = { pvars, NULL, 3, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x6F, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		VariantClear(&pvars[1]);
		delete [] pvars;
	}

	void Fire_OnChannelState(IDispatch* pdispChannel, 
							 LONG lNewChannelState)
	{
		VARIANTARG* pvars = new VARIANTARG[2];
		for (int i = 0; i < 2; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[1].vt = VT_DISPATCH;
				pvars[1].pdispVal = pdispChannel;
				pvars[0].vt = VT_I4;
				pvars[0].lVal= lNewChannelState;
				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x70, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete [] pvars;
	}

	void Fire_OnChannelError(IDispatch *pdispChannel, 
							 LONG lErrorCode, 
							 LPCSTR szDescription)
	{
		VARIANTARG* pvars = new VARIANTARG[3];
		for (int i = 0; i < 3; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[2].vt = VT_DISPATCH;
				pvars[2].pdispVal = pdispChannel;
				pvars[1].vt = VT_I4;
				pvars[1].lVal= lErrorCode;
				pvars[0].vt = VT_BSTR;
				pvars[0].bstrVal= A2BSTR(szDescription);
				DISPPARAMS disp = { pvars, NULL, 3, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x71, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		VariantClear(&pvars[0]);
		delete [] pvars;
	}

	void Fire_OnBeginEnumeration(IDispatch* pdispChannel, 
								 LONG lEnumType)
	{
		VARIANTARG* pvars = new VARIANTARG[2];
		for (int i = 0; i < 2; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[1].vt = VT_DISPATCH;
				pvars[1].pdispVal = pdispChannel;
				pvars[0].vt = VT_I4;
				pvars[0].lVal= lEnumType;
				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x72, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete [] pvars;
	}

	void Fire_OnEndEnumeration(IDispatch* pdispChannel, 
							   LONG lEnumType)
	{
		VARIANTARG* pvars = new VARIANTARG[2];
		for (int i = 0; i < 2; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[1].vt = VT_DISPATCH;
				pvars[1].pdispVal = pdispChannel;
				pvars[0].vt = VT_I4;
				pvars[0].lVal= lEnumType;
				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x73, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete [] pvars;
	}

	void Fire_OnAddMember(IDispatch* pdispChannel, 
						  LPCSTR szNickname, 
						  IDispatch* pdispMemberItems)
	{
		LPWSTR		wszWideNick;
		VARIANTARG* pvars = new VARIANTARG[3];
		for (int i = 0; i < 3; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[2].vt = VT_DISPATCH;
				pvars[2].pdispVal = pdispChannel;

				pvars[1].vt = VT_BSTR;
				if (g_chExtNckPfx == szNickname[0])
				{
					if (!bConvertUTF8StringToWide(szNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
						return;
					pvars[1].bstrVal = SysAllocString(wszWideNick);
					delete [] wszWideNick;
				}
				else
					pvars[1].bstrVal = A2BSTR(szNickname);

				pvars[0].vt = VT_DISPATCH;
				pvars[0].pdispVal= pdispMemberItems;
				DISPPARAMS disp = { pvars, NULL, 3, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x74, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		VariantClear(&pvars[1]);
		delete [] pvars;
	}

	void Fire_OnDelMember(IDispatch* pdispChannel, 
						  LPCSTR szNickname)
	{
		LPWSTR		wszWideNick;
		VARIANTARG* pvars = new VARIANTARG[2];
		for (int i = 0; i < 2; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[1].vt = VT_DISPATCH;
				pvars[1].pdispVal = pdispChannel;

				pvars[0].vt = VT_BSTR;
				if (g_chExtNckPfx == szNickname[0])
				{
					if (!bConvertUTF8StringToWide(szNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
						return;
					pvars[0].bstrVal = SysAllocString(wszWideNick);
					delete [] wszWideNick;
				}
				else
					pvars[0].bstrVal = A2BSTR(szNickname);

				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x75, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		VariantClear(&pvars[0]);
		delete [] pvars;
	}

	void Fire_OnUserProperty(IDispatch* pdispUserItems)
	{
		VARIANTARG* pvars = new VARIANTARG;
		VariantInit(pvars);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars->vt = VT_DISPATCH;
				pvars->pdispVal = pdispUserItems;
				DISPPARAMS disp = { pvars, NULL, 1, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x76, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete pvars;
	}

	void Fire_OnUserPropertyChanged(LPCSTR szModifiedNickname, 
									LPCSTR szModifierNickname, 
									LPCWSTR wszUserPropertyName,
									VARIANT* pvOldUserProperty, 
									VARIANT* pvNewUserProperty)
	{
		LPWSTR		wszWideNick;
		VARIANTARG	*pvars = new VARIANTARG[5];
		for (int i = 0; i < 5; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[4].vt = VT_BSTR;
				if (g_chExtNckPfx == szModifiedNickname[0])
				{
					if (!bConvertUTF8StringToWide(szModifiedNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
						return;
					pvars[4].bstrVal = SysAllocString(wszWideNick);
					delete [] wszWideNick;
				}
				else
					pvars[4].bstrVal = A2BSTR(szModifiedNickname);
				
				pvars[3].vt = VT_BSTR;
				if (szModifierNickname)
				{
					if (g_chExtNckPfx == szModifierNickname[0])
					{
						if (!bConvertUTF8StringToWide(szModifierNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
							return;
						pvars[3].bstrVal = SysAllocString(wszWideNick);
						delete [] wszWideNick;
					}
					else
						pvars[3].bstrVal = A2BSTR(szModifierNickname);
				}
				else
					pvars[3].bstrVal = SysAllocString(L"");

				pvars[2].vt = VT_BSTR;
				pvars[2].bstrVal = SysAllocString(wszUserPropertyName);

				pvars[1].vt = VT_VARIANT|VT_BYREF;
				pvars[1].pvarVal = pvOldUserProperty;

				pvars[0].vt = VT_VARIANT|VT_BYREF;
				pvars[0].pvarVal = pvNewUserProperty;

				if (!pvars[4].bstrVal ||
					!pvars[3].bstrVal ||
					!pvars[2].bstrVal)
					return;

				DISPPARAMS disp = { pvars, NULL, 5, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x77, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		for (i = 2; i < 5; i++)
			VariantClear(&pvars[i]);
		delete [] pvars;
	}

	void Fire_OnMemberProperty(IDispatch* pdispChannel, 
							   IDispatch* pdispMemberItems)
	{
		VARIANTARG* pvars = new VARIANTARG[2];
		for (int i = 0; i < 2; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[1].vt = VT_DISPATCH;
				pvars[1].pdispVal = pdispChannel;
				pvars[0].vt = VT_DISPATCH;
				pvars[0].pdispVal = pdispMemberItems;
				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x78, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete [] pvars;
	}

	void Fire_OnMemberPropertyChanged(IDispatch* pdispChannel, 
									  LPCSTR szModifiedNickname, 
									  LPCSTR szModifierNickname, 
									  LPCWSTR wszMemberPropertyName,
									  VARIANT* pvOldMemberProperty, 
									  VARIANT* pvNewMemberProperty)
	{
		LPWSTR		wszWideNick;
		VARIANTARG* pvars = new VARIANTARG[6];
		for (int i = 0; i < 6; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[5].vt = VT_DISPATCH;
				pvars[5].pdispVal = pdispChannel;

				pvars[4].vt = VT_BSTR;
				if (g_chExtNckPfx == szModifiedNickname[0])
				{
					if (!bConvertUTF8StringToWide(szModifiedNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
						return;
					pvars[4].bstrVal = SysAllocString(wszWideNick);
					delete [] wszWideNick;
				}
				else
					pvars[4].bstrVal = A2BSTR(szModifiedNickname);

				pvars[3].vt = VT_BSTR;
				if (szModifierNickname)
				{
					if (g_chExtNckPfx == szModifierNickname[0])
					{
						if (!bConvertUTF8StringToWide(szModifierNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
							return;
						pvars[3].bstrVal = SysAllocString(wszWideNick);
						delete [] wszWideNick;
					}
					else
						pvars[3].bstrVal = A2BSTR(szModifierNickname);
				}
				else
					pvars[3].bstrVal = SysAllocString(L"");

				pvars[2].vt = VT_BSTR;
				pvars[2].bstrVal = SysAllocString(wszMemberPropertyName);
				pvars[1].vt = VT_VARIANT|VT_BYREF;
				pvars[1].pvarVal = pvOldMemberProperty;
				pvars[0].vt = VT_VARIANT|VT_BYREF;
				pvars[0].pvarVal = pvNewMemberProperty;
				DISPPARAMS disp = { pvars, NULL, 6, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x79, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		for (i = 2; i < 5; i++)
			VariantClear(&pvars[i]);
		delete [] pvars;
	}

	void Fire_OnMemberKicked(IDispatch* pdispChannel, 
							 LPCSTR szKickedNickname, 
							 LPCSTR szKickerNickname, 
							 LPCSTR szReason)
	{
		LPWSTR		wszWideNick;
		VARIANTARG* pvars = new VARIANTARG[4];
		for (int i = 0; i < 4; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[3].vt = VT_DISPATCH;
				pvars[3].pdispVal = pdispChannel;

				pvars[2].vt = VT_BSTR;
				if (g_chExtNckPfx == szKickedNickname[0])
				{
					if (!bConvertUTF8StringToWide(szKickedNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
						return;
					pvars[2].bstrVal = SysAllocString(wszWideNick);
					delete [] wszWideNick;
				}
				else
					pvars[2].bstrVal = A2BSTR(szKickedNickname);
				
				pvars[1].vt = VT_BSTR;
				if (szKickerNickname)
				{
					if (g_chExtNckPfx == szKickerNickname[0])
					{
						if (!bConvertUTF8StringToWide(szKickerNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
							return;
						pvars[1].bstrVal = SysAllocString(wszWideNick);
						delete [] wszWideNick;
					}
					else
						pvars[1].bstrVal = A2BSTR(szKickerNickname);
				}
				else
					pvars[1].bstrVal = SysAllocString(L"");

				pvars[0].vt = VT_BSTR;
				pvars[0].bstrVal = szReason ? A2BSTR(szReason) : SysAllocString(L"");
				DISPPARAMS disp = { pvars, NULL, 4, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x7A, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		for (i = 0; i < 3; i++)
			VariantClear(&pvars[i]);
		delete [] pvars;
	}

	void Fire_OnChannelProperty(IDispatch* pdispChannel, 
								IDispatch* pdispChannelItems)
	{
		VARIANTARG* pvars = new VARIANTARG[2];
		for (int i = 0; i < 2; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[1].vt = VT_DISPATCH;
				pvars[1].pdispVal = pdispChannel;
				pvars[0].vt = VT_DISPATCH;
				pvars[0].pdispVal = pdispChannelItems;
				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x7B, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete [] pvars;
	}

	void Fire_OnChannelPropertyChanged(IDispatch* pdispChannel,
									   LPCSTR szModifierNickname,
									   LPCWSTR wszChannelPropertyName,
									   VARIANT* pvOldChannelProperty, 
									   VARIANT* pvNewChannelProperty)
	{
		LPWSTR		wszWideNick;
		VARIANTARG* pvars = new VARIANTARG[5];
		for (int i = 0; i < 5; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[4].vt = VT_DISPATCH;
				pvars[4].pdispVal = pdispChannel;

				pvars[3].vt = VT_BSTR;
				if (szModifierNickname)
				{
					if (g_chExtNckPfx == szModifierNickname[0])
					{
						if (!bConvertUTF8StringToWide(szModifierNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
							return;
						pvars[3].bstrVal = SysAllocString(wszWideNick);
						delete [] wszWideNick;
					}
					else
						pvars[3].bstrVal = A2BSTR(szModifierNickname);
				}
				else
					pvars[3].bstrVal = SysAllocString(L"");

				pvars[2].vt = VT_BSTR;
				pvars[2].bstrVal = SysAllocString(wszChannelPropertyName);
				
				pvars[1].vt = VT_VARIANT|VT_BYREF;
				pvars[1].pvarVal = pvOldChannelProperty;
				pvars[0].vt = VT_VARIANT|VT_BYREF;
				pvars[0].pvarVal = pvNewChannelProperty;
				
				DISPPARAMS disp = { pvars, NULL, 5, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x7C, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		for (i = 2; i < 4; i++)
			VariantClear(&pvars[i]);
		delete [] pvars;
	}

	void Fire_OnServerProperty(IDispatch* pdispServerItems)
	{
		VARIANTARG* pvars = new VARIANTARG;
		VariantInit(pvars);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars->vt = VT_DISPATCH;
				pvars->byref = pdispServerItems;
				DISPPARAMS disp = { pvars, NULL, 1, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x7D, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete pvars;
	}

	void Fire_OnInvitation(BSTR bstrChannelName, 
						   IDispatch* pdispInviterItems)
	{
		VARIANTARG* pvars = new VARIANTARG[2];
		for (int i = 0; i < 2; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[1].vt = VT_BSTR;
				pvars[1].bstrVal = bstrChannelName;
				pvars[0].vt = VT_DISPATCH;
				pvars[0].pdispVal = pdispInviterItems;
				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x7E, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete [] pvars;
	}

	void Fire_OnKnock(IDispatch* pdispChannel, 
					  IDispatch* pdispKnockerItems,
					  LONG lErrorCode)
	{
		VARIANTARG* pvars = new VARIANTARG[3];
		for (int i = 0; i < 3; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[2].vt = VT_DISPATCH;
				pvars[2].pdispVal = pdispChannel;
				pvars[1].vt = VT_DISPATCH;
				pvars[1].pdispVal = pdispKnockerItems;
				pvars[0].vt = VT_I4;
				pvars[0].lVal = lErrorCode;
				DISPPARAMS disp = { pvars, NULL, 3, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x7F, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete [] pvars;
	}

	void Fire_OnPrivateMessage(LPCSTR szSenderNickname,
							   LONG lPrivateMessageType,
							   VARIANT* pvMessage,
							   VARIANT* pvDataMessageTag)
	{
		LPWSTR		wszWideNick;
		VARIANTARG* pvars = new VARIANTARG[4];
		for (int i = 0; i < 4; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[3].vt = VT_BSTR;
				if (g_chExtNckPfx == szSenderNickname[0])
				{
					if (!bConvertUTF8StringToWide(szSenderNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
						return;
					pvars[3].bstrVal = SysAllocString(wszWideNick);
					delete [] wszWideNick;
				}
				else
					pvars[3].bstrVal = A2BSTR(szSenderNickname);

				pvars[2].vt = VT_I4;
				pvars[2].lVal = lPrivateMessageType;
				pvars[1].vt = VT_VARIANT|VT_BYREF;
				pvars[1].pvarVal = pvMessage;
				pvars[0].vt = VT_VARIANT|VT_BYREF;
				pvars[0].pvarVal = pvDataMessageTag;
				DISPPARAMS disp = { pvars, NULL, 4, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x80, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		VariantClear(&pvars[3]);
		delete [] pvars;
	}

	void Fire_OnServerTextMessage(LONG lServerMessageType,
								  LPCSTR szText)
	{
		VARIANTARG* pvars = new VARIANTARG[2];
		for (int i = 0; i < 2; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[1].vt = VT_I4;
				pvars[1].lVal = lServerMessageType;
				pvars[0].vt = VT_BSTR;
				pvars[0].bstrVal = A2BSTR(szText);
				DISPPARAMS disp = { pvars, NULL, 2, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x81, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		VariantClear(&pvars[0]);
		delete [] pvars;
	}

	void Fire_OnMessage(IDispatch* pdispChannel, 
						LPCSTR szSenderNickname, 
						LONG lMessageType,
						VARIANT* pvMessage,
						VARIANT* pvRecipientNicknames,
						VARIANT* pvDataMessageTag)
	{
		LPWSTR		wszWideNick;
		VARIANTARG* pvars = new VARIANTARG[6];
		for (int i = 0; i < 6; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[5].vt = VT_DISPATCH;
				pvars[5].pdispVal = pdispChannel;

				pvars[4].vt = VT_BSTR;
				if (g_chExtNckPfx == szSenderNickname[0])
				{
					if (!bConvertUTF8StringToWide(szSenderNickname, 0, &wszWideNick, NULL, TRUE /*bNickname*/))
						return;
					pvars[4].bstrVal = SysAllocString(wszWideNick);
					delete [] wszWideNick;
				}
				else
					pvars[4].bstrVal = A2BSTR(szSenderNickname);
				
				pvars[3].vt = VT_I4;
				pvars[3].lVal = lMessageType;
				pvars[2].vt = VT_VARIANT|VT_BYREF;
				pvars[2].pvarVal = pvMessage;
				pvars[1].vt = VT_VARIANT|VT_BYREF;
				pvars[1].pvarVal = pvRecipientNicknames;
				pvars[0].vt = VT_VARIANT|VT_BYREF;
				pvars[0].pvarVal = pvDataMessageTag;
				DISPPARAMS disp = { pvars, NULL, 6, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x82, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		VariantClear(&pvars[4]);
		delete [] pvars;
	}

	void Fire_OnProtocolMessage(LPCSTR szMessagePrefix, 
								LPCSTR szMessageCommand, 
								VARIANT* pvMessageParameters,
								OLE_ENABLEDEFAULTBOOL* pbEnableDefault)
	{
		VARIANTARG* pvars = new VARIANTARG[4];
		for (int i = 0; i < 4; i++)
			VariantInit(&pvars[i]);
		T* pT = (T*)this;
		pT->Lock();
		IUnknown** pp = m_vec.begin();
		while (pp < m_vec.end())
		{
			if (*pp != NULL)
			{
				pvars[3].vt = VT_BSTR;
				pvars[3].bstrVal = A2BSTR(szMessagePrefix);
				pvars[2].vt = VT_BSTR;
				pvars[2].bstrVal = A2BSTR(szMessageCommand);
				pvars[1].vt = VT_VARIANT|VT_BYREF;
				pvars[1].pvarVal = pvMessageParameters;
				pvars[0].vt = VT_BYREF|VT_BOOL;
				pvars[0].pboolVal = pbEnableDefault;
				DISPPARAMS disp = { pvars, NULL, 4, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(0x83, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		for (i = 2; i < 4; i++)
			VariantClear(&pvars[i]);
		delete [] pvars;
	}
};
