//////////////////////////////////////////////////////////////////////////////
// CProxy_INMSttEvent
template <class T>
class CProxy_INMSttEvent : public IConnectionPointImpl<T, &DIID__INMSttEvent, CComDynamicUnkArray>
{
public:
//methods:
//_INMSttEvent : IDispatch
public:
	void Fire_OnOperationDone(BOOL bSuccess, UINT uDispID)
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
				pvars->vt = VT_BOOL;
				pvars->boolVal= bSuccess;
				DISPPARAMS disp = { pvars, NULL, 1, 0 };
				IDispatch* pDispatch = reinterpret_cast<IDispatch*>(*pp);
				pDispatch->Invoke(uDispID, IID_NULL, LOCALE_USER_DEFAULT, DISPATCH_METHOD, &disp, NULL, NULL, NULL);
			}
			pp++;
		}
		pT->Unlock();
		delete pvars;
	}
};
