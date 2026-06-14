// ChCol.cpp : Implementation of CChannelsCollection
#include "StdAfx.H"
#include "MsChatPr.H"
#include "ChCol.H"
#include "ChObj.H"
#include "PrErr.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CChannelsCollection

STDMETHODIMP CChannelsCollection::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IChannelsCollection,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


HRESULT CChannelsCollection::HrThrowError(SCODE sc, LPCTSTR szChannelName, LPCTSTR szNickname)
{
	TCHAR szError[g_nMaxLengthError];

	ASSERT(sc != S_OK, "sc == S_OK in CChannelsCollection::HrThrowError");
	ASSERT(m_pccsock, "m_pccsock is NULL in CChannelsCollection::HrThrowError");

	Sc2Sz(sc, szError, szChannelName, szNickname, m_pccsock->bIsIrcXSocket(), m_pccsock->GetCharSet());
	
	return AtlReportError(CLSID_Channels, szError, IID_IChannelsCollection, ResultFromScode(sc));
}


STDMETHODIMP CChannelsCollection::get_Item(VARIANT vIndex, IChannelObject **ppChannelObject)
{
	SCODE			sc = S_OK;
	CChatChannel*	pChannel = NULL;
	VARIANT*		pvIndex	 = &vIndex;

	OutputDebugThreadIdString("CChannelsCollection::get_Item - Enter\n");

    CHECK_POINTER(ppChannelObject);

	*ppChannelObject = NULL;

	if (pvIndex->vt == (VT_BYREF+VT_VARIANT))
		pvIndex = pvIndex->pvarVal;

	if (!pvIndex)
		return HrThrowError(CPR_E_CHANOBJ_BADINDEX);

	switch (pvIndex->vt)
	{
		case VT_EMPTY:
		case VT_NULL:
		case VT_ERROR:
			// By default we get last item in the collection
			if (!m_plistChannel->bGetCell(1L, (PVOID*) &pChannel))
				sc = CPR_E_CHANOBJ_INDEXOOB;
			break;

		case VT_I2:
		case VT_I2+VT_BYREF:
		case VT_I4:
		case VT_I4+VT_BYREF:
		{
			LONG lIndex;
			
			if (pvIndex->vt == VT_I2+VT_BYREF || pvIndex->vt == VT_I2)
				lIndex = (pvIndex->vt == VT_I2) ? pvIndex->iVal : *(pvIndex->piVal);
			else
				lIndex = (pvIndex->vt == VT_I4) ? pvIndex->lVal : *(pvIndex->plVal);

			if (!m_plistChannel->bGetCell(m_plistChannel->DwGetCellCount() - lIndex + 1L, (PVOID*) &pChannel))
				sc = CPR_E_CHANOBJ_INDEXOOB;
			break;
		}
		case VT_BSTR:
		{
			USES_CONVERSION;
			LPTSTR szAnsiChannelKey = W2T(pvIndex->bstrVal);

			//MAKE_ANSIPTR_FROMWIDE(szAnsiChannelKey, pvIndex->bstrVal);

			if (!szAnsiChannelKey)
				sc = CTL_E_OUTOFMEMORY;
			else
				if (!m_plistChannel->bFindCellFromData(1, g_nSearchByKey, (PVOID) szAnsiChannelKey, g_nSearchNone, NULL, (PVOID*) &pChannel, NULL))
					sc = CPR_E_CHANOBJ_BADINDEX;
			break;
		}
		default:
			sc = CPR_E_CHANOBJ_BADINDEX;
	}

	if (S_OK != sc)
		return HrThrowError(sc);

	ASSERT(pChannel, "pChannel is NULL in CChannelsCollection::get_Item");

	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();
	if (pcchan)
	{
		((IChannelObject*) pcchan)->AddRef();
		*ppChannelObject = (IChannelObject*) pcchan;
	}

	return (*ppChannelObject) ? NOERROR : E_NOINTERFACE;
}


STDMETHODIMP CChannelsCollection::get__NewEnum(IUnknown **pVal)
{
	return HrThrowError(E_NOTIMPL);
}


STDMETHODIMP CChannelsCollection::get_Count(long *plCount)
{
    CHECK_POINTER(plCount);

	*plCount = m_plistChannel->DwGetCellCount();

    return NOERROR;
}


STDMETHODIMP CChannelsCollection::Add(VARIANT vIndex, VARIANT vKey, IChannelObject **ppChannelObject)
{
	CChannelObject*	pcchan = NULL;
	CChatChannel*	pChannel = NULL;
	IDispatch*		pidisp = NULL;
	LPCLASSFACTORY	pClassFactory = NULL;
	VARIANT*		pvIndex	 = &vIndex;
	VARIANT*		pvKey	 = &vKey;
	HRESULT			hr = NOERROR;
	LONG			lIndex = 0L;

	OutputDebugThreadIdString("CChannelsCollection::Add - Enter\n");

    CHECK_POINTER(ppChannelObject);

	*ppChannelObject = NULL;

	if (pvIndex->vt == (VT_BYREF+VT_VARIANT))
		pvIndex = pvIndex->pvarVal;

	if (pvKey->vt == (VT_BYREF+VT_VARIANT))
		pvKey = pvKey->pvarVal;

	if (!pvIndex)
		return HrThrowError(CPR_E_CHANOBJ_BADINDEX);

	if (!pvKey)
		return HrThrowError(CPR_E_CHANOBJ_BADKEY);

	switch (pvIndex->vt)
	{
		case VT_I2:
		{
			lIndex = pvIndex->iVal;
			break;
		}
		case VT_I4:
		{
			lIndex = pvIndex->lVal;
			break;
		}
		case VT_EMPTY:
		case VT_NULL:
		case VT_ERROR:
		{
			// we want to add the item at the end of the collection
			lIndex = (LONG) (m_plistChannel->DwGetCellCount() + 1L);
			break;
		}
		default:
			return HrThrowError(CPR_E_CHANOBJ_BADINDEX);
	}

	if (lIndex < 1L || lIndex > (LONG) (m_plistChannel->DwGetCellCount() + 1L))
		return HrThrowError(CPR_E_CHANOBJ_INDEXOOB);

	if (FAILED(hr = AtlModuleGetClassObject(&_Module, CLSID_Channel, IID_IClassFactory, (LPVOID*) &pClassFactory)))
	{
		ASSERT(FALSE, "AtlModuleGetClassObject failed in CChannelsCollection::Add");
		return HrThrowError(GetScodeFromHResult(hr));
	}

	ASSERT(pClassFactory, "pClassFactory is NULL in CChannelsCollection::Add");

	hr = pClassFactory->CreateInstance(NULL, IID_IDispatch, (LPVOID*) &pidisp);
	pClassFactory->Release();

	if (FAILED(hr))
	{
		ASSERT(FALSE, "CreateInstance failed in CChannelsCollection::Add");
		return HrThrowError(GetScodeFromHResult(hr));
	}

	ASSERT(pidisp, "pidisp is NULL in CChannelsCollection::Add");

	hr = pidisp->QueryInterface(IID_IChannelObject, (void**) ppChannelObject);
	pidisp->Release();

	if (FAILED(hr))
	{
		ASSERT(FALSE, "QueryInterface(IID_IChannelObject) failed in CChannelsCollection::Add");
		return HrThrowError(GetScodeFromHResult(hr));
	}

	ASSERT(ppChannelObject, "ppChannelObject is NULL in CChannelsCollection::Add");
	
	pcchan = (CChannelObject*) *ppChannelObject;

	ASSERT(pcchan, "pcchan is NULL in CChannelsCollection::Add");

	((IChannelObject*) pcchan)->AddRef();

	switch (pvKey->vt)
	{
		case VT_BSTR:
		{
			if (pvKey->bstrVal)
			{
				USES_CONVERSION;
				LPTSTR szAnsiItemKey = W2T(pvKey->bstrVal);

				// Can pass stack mem to HrSetItemKey since it allocates memory and makes a copy
				if (FAILED(hr = pcchan->HrSetItemKey(szAnsiItemKey)))
				{
					((IChannelObject*) pcchan)->Release();
					*ppChannelObject = NULL;
					return HrThrowError(GetScodeFromHResult(hr));
				}
			}
			else
				pcchan->HrSetItemKey(NULL);
			break;
		}
		case VT_EMPTY:
		case VT_NULL:
		case VT_ERROR:
		{
			pcchan->HrSetItemKey(NULL);
			break;
		}
		default:
		{
			((IChannelObject*) pcchan)->Release();
			*ppChannelObject = NULL;
			return HrThrowError(CPR_E_CHANOBJ_BADKEY);
		}
	}

	// make sure that the given key is unique
	if (m_plistChannel->bFindCellFromData(1, g_nSearchByKey, (PVOID) pcchan->SzGetItemKey(), g_nSearchNone, NULL, (PVOID*) &pChannel, NULL))
	{
		((IChannelObject*) pcchan)->Release();
		*ppChannelObject = NULL;
		return HrThrowError(CPR_E_CHANOBJ_KEYNOTUNIQUE);
	}

	pcchan->m_bInCollection = TRUE;
	pcchan->SetChatSocket(m_pccsock);

	if (!m_plistChannel->bInsertCell(m_plistChannel->DwGetCellCount() + 2L - lIndex, (PVOID) (CChatChannel*) pcchan))
	{
		((IChannelObject*) pcchan)->Release();
		*ppChannelObject = NULL;
		return HrThrowError(CTL_E_OUTOFMEMORY);
	}

	return NOERROR;
}


STDMETHODIMP CChannelsCollection::Remove(VARIANT vIndex)
{
	CChannelObject* pcchan = NULL;
	CChatChannel*	pChannel = NULL;
	VARIANT*		pvIndex	 = &vIndex;
	HRESULT			hr = NOERROR;
	SCODE			sc = S_OK;
	LONG			lIndex = 0L;

	OutputDebugThreadIdString("CChannelsCollection::Remove - Enter\n");

	if (pvIndex->vt == (VT_BYREF+VT_VARIANT))
		pvIndex = pvIndex->pvarVal;

	if (!pvIndex)
		return HrThrowError(CPR_E_CHANOBJ_BADINDEX);

	switch (pvIndex->vt)
	{
		case VT_I2:
		case VT_I2+VT_BYREF:
		{
			lIndex = (pvIndex->vt == VT_I2) ? pvIndex->iVal : *(pvIndex->piVal);

			if (lIndex < 1L || lIndex > (LONG) m_plistChannel->DwGetCellCount())
				sc = CPR_E_CHANOBJ_INDEXOOB;
			else
			{
				if (!m_plistChannel->bGetCell(m_plistChannel->DwGetCellCount() - lIndex + 1L, (PVOID*) &pChannel))
					sc = CPR_E_CHANOBJ_INDEXOOB;
			}
			break;
		}
		case VT_I4:
		case VT_I4+VT_BYREF:
		{
			lIndex = (pvIndex->vt == VT_I4) ? pvIndex->lVal : *(pvIndex->plVal);

			if (lIndex < 1L || lIndex > (LONG) m_plistChannel->DwGetCellCount())
			{
				sc = CPR_E_CHANOBJ_INDEXOOB;
			}
			else
			{
				if (!m_plistChannel->bGetCell(m_plistChannel->DwGetCellCount() - lIndex + 1L, (PVOID*) &pChannel))
					sc = CPR_E_CHANOBJ_INDEXOOB;
			}
			break;
		}
		case VT_BSTR:
		{
			if (pvIndex->bstrVal)
			{
				USES_CONVERSION;
				LPTSTR szAnsiItemKey = W2T(pvIndex->bstrVal);

				if (!szAnsiItemKey)
					sc = CTL_E_OUTOFMEMORY;
				else
					if (!m_plistChannel->bFindCellFromData(1, g_nSearchByKey, (PVOID) szAnsiItemKey, g_nSearchNone, NULL, (PVOID*) &pChannel, NULL))
						sc = CPR_E_CHANOBJ_BADINDEX;
			}
			else
				sc = CPR_E_CHANOBJ_BADINDEX;
			break;
		}
		default:
			sc = CPR_E_CHANOBJ_BADINDEX;
	}

	if (FAILED(sc))
		return HrThrowError(sc);

	ASSERT(pChannel, "pChannel is NULL in CChannelsCollection::Remove");

	pcchan = (CChannelObject*) pChannel->GetAutObject();
	ASSERT(pcchan, "pcchan is NULL in CChannelsCollection::Remove");

	switch (pChannel->m_chs)
	{
		case chsClosed:
		{
			// that's cool
			// we just need to remove the channel from the list (which will do one Release call)

			pcchan->m_bInCollection = FALSE;

			if (!m_plistChannel->bRemoveCell((PVOID) pChannel))
			{
				ASSERT(FALSE, "Could not remove channel cell in CChannelsCollection::Remove");
				sc = CPR_E_FAILURE;
			}
			break;
		}
		case chsOpening:
		case chsOpen:
		{
			// The channel should be closed before removing it from the collection
			// Since leaving a channel is an asynchronous operation that involves firing events
			// we don't allow removing a non-closed channel
			sc = CPR_E_NOTCLOSED;
			break;
		}
		default:
		{
			ASSERT(FALSE, "Unexpected value for m_chs in CChannelsCollection::Remove");
			sc = CPR_E_FAILURE;
		}
	}

	return (SUCCEEDED(sc)) ? NOERROR : HrThrowError(sc);
}
