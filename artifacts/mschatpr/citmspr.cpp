// CItmsPr.cpp : Implementation of CItemsObject
#include "StdAfx.H"
#include "MsChatPr.H"
#include "CItmsObj.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CItemsObject Properties

STDMETHODIMP CItemsObject::get_AssociatedType(BSTR * pbstrAssociatedType)
{
//	OutputDebugThreadIdString("CItemsObject::get_AssociatedType - Enter\n");

    CHECK_POINTER(pbstrAssociatedType);

	if (!bGetAssociatedType(pbstrAssociatedType))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	else
		return NOERROR;
}

STDMETHODIMP CItemsObject::put_AssociatedType(BSTR bstrAssociatedType)
{
//	OutputDebugThreadIdString("CItemsObject::put_AssociatedType - Enter\n");

	if (!bstrAssociatedType)
		return HrThrowError(CPR_E_INVALIDASSOCIATEDTYPE);

	if (!bSetAssociatedType(bstrAssociatedType))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	else
		return NOERROR;
}

STDMETHODIMP CItemsObject::get_ValidItems(VARIANT * pvValidItems)
{
//	OutputDebugThreadIdString("CItemsObject::get_ValidItems - Enter\n");

    CHECK_POINTER(pvValidItems);

	if (!bGetValidItems(pvValidItems))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	else
		return NOERROR;
}

STDMETHODIMP CItemsObject::get_ItemValid(BSTR ItemName, BOOL * pbItemValid)
{
//	OutputDebugThreadIdString("CItemsObject::get_ItemValid - Enter\n");

    CHECK_POINTER(pbItemValid);

	if (!ItemName)
		return HrThrowError(CPR_E_INVALIDITEMNAME);

	if (!bGetItemValid(ItemName, pbItemValid))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	else
		return NOERROR;
}

STDMETHODIMP CItemsObject::get_Item(BSTR ItemName, VARIANT * pvItem)
{
//	OutputDebugThreadIdString("CItemsObject::put_Item - Enter\n");

    CHECK_POINTER(pvItem);

	if (!ItemName)
		return HrThrowError(CPR_E_INVALIDITEMNAME);

	if (!bGetItem(ItemName, pvItem))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	else
		return NOERROR;
}

STDMETHODIMP CItemsObject::put_Item(BSTR ItemName, VARIANT vItem)
{
//	OutputDebugThreadIdString("CItemsObject::put_Item - Enter\n");

	if (!ItemName)
		return HrThrowError(CPR_E_INVALIDITEMNAME);

	if (!bSetItem((LPCWSTR) ItemName, &vItem))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	else
		return NOERROR;
}

