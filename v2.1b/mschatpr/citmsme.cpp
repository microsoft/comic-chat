// CItmsMe.cpp : Implementation of CItemsObject
#include "stdafx.h"
#include "MsChatPr.h"
#include "CItmsObj.h"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CItemsObject Methods

STDMETHODIMP CItemsObject::InvalidateItem(BSTR ItemName)
{
//	OutputDebugThreadIdString("CItemsObject::InvalidateItem\n");

	if (!bInvalidateItem(ItemName))
		return HrThrowError(GetScodeFromHResult(HrGetLastError()));
	else
		return NOERROR;
}
