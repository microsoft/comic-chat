// CItmsObj.cpp : Implementation of CItemsObject
#include "StdAfx.H"
#include "MsChatPr.H"
#include "CItmsObj.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CItemsObject

STDMETHODIMP CItemsObject::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IChatItems,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


IDispatch* CItemsObject::GetDispatch(void)
{
    void  *pvObjOut = NULL;

	if (SUCCEEDED(_InternalQueryInterface(IID_IDispatch, &pvObjOut)))
		return (IDispatch*) pvObjOut;
	else
		return NULL;
}


HRESULT	CItemsObject::HrThrowError(SCODE sc)
{
	TCHAR szError[g_nMaxLengthError];

	ASSERT(sc != S_OK, "sc == S_OK in CItemsObject::HrThrowError");
	
	Sc2Sz(sc, szError, NULL, NULL, FALSE, 0);

	return AtlReportError(CLSID_ChatItems, szError, IID_IChatItems, ResultFromScode(sc));
}
