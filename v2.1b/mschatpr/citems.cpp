//=--------------------------------------------------------------------------=
// CItems.Cpp	- CChatItems implementation
//=--------------------------------------------------------------------------=
// Copyright 1995-1997 Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=
//
#include "StdAfx.H"
#include "CItems.H"

// for ASSERT and FAIL
//
SZTHISFILE


//=--------------------------------------------------------------------------=
// CChatItems::CChatItems
//=--------------------------------------------------------------------------=
CChatItems::CChatItems(void)
{
	// OutputDebugThreadIdString("CChatItems::CChatItems - Enter\n");

	m_hrLastError			= NOERROR;
	m_bstrAssociatedType	= NULL;
}

//=--------------------------------------------------------------------------=
// CChatItems::~CChatItems
//=--------------------------------------------------------------------------=
//
// Notes:
//
CChatItems::~CChatItems (void)
{
	// OutputDebugThreadIdString("CChatItems::~CChatItems - Enter\n");

	// delete the items list
	m_listItem.bDeleteList();

	// delete the associated type
	if (m_bstrAssociatedType)
		SysFreeString(m_bstrAssociatedType);
}


BOOL CChatItems::bInvalidateItem(BSTR &bstrItemName)
{
	PRITEM prItem;
	
//	OutputDebugThreadIdString("CChatItems::bInvalidateItem\n");

	if (!bstrItemName)
	{
		// null item name passed
		m_hrLastError = CC_E_INVALIDITEMNAME;
		return FALSE;
	}

	prItem.vName.vt = VT_BSTR;
	prItem.vName.bstrVal = bstrItemName;

	// Try to find this item in our list and delete it
	if (!m_listItem.bRemoveCell((PVOID) &prItem))
	{
		// this item is not in our list
		m_hrLastError = CC_E_ITEMNOTAVAILABLE;
		return FALSE;
	}
	else
		return TRUE;
}


BOOL CChatItems::bGetAssociatedType(BSTR* pbstrAssociatedType)
{
//	OutputDebugThreadIdString("CChatItems::bGetAssociatedType - Enter\n");

    CHECK_POINTER(pbstrAssociatedType);

	if (m_bstrAssociatedType)
		*pbstrAssociatedType = SysAllocString(m_bstrAssociatedType);
	else
		*pbstrAssociatedType = SysAllocString(L"");

	return TRUE;
}


BOOL CChatItems::bSetAssociatedType(LPCWSTR wszAssociatedType)
{
//	OutputDebugThreadIdString("CChatItems::bSetAssociatedType - Enter\n");

	ASSERT(wszAssociatedType, "wszAssociatedType is NULL in CChatItems::bSetAssociatedType");

	if (m_bstrAssociatedType)
		SysFreeString(m_bstrAssociatedType);

	m_bstrAssociatedType = SysAllocString(wszAssociatedType);

	if (!m_bstrAssociatedType)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	else
		return TRUE;
}


BOOL CChatItems::bGetValidItems(VARIANT* pvValidItems)
{
//	OutputDebugThreadIdString("CChatItems::bGetValidItems - Enter\n");

    CHECK_POINTER(pvValidItems);

	if (!m_listItem.bDoActionOnList(g_nActionFillSafeArray))
	{
		m_hrLastError = E_FAIL;
		return FALSE;
	}

	*pvValidItems = m_listItem.GetValidItems();

	return TRUE;
}


BOOL CChatItems::bGetItemValid(BSTR &bstrItemName, BOOL* pbItemValid)
{
	PPRITEM pPrItem = NULL;
	PRITEM	prItem;

//	OutputDebugThreadIdString("CChatItems::bGetItemValid - Enter\n");

    ASSERT(bstrItemName, "bstrItemName is NULL in CChatItems::bGetItemValid");
    ASSERT(pbItemValid, "pbItemValid is NULL in CChatItems::bGetItemValid");

	prItem.vName.vt = VT_BSTR;
	prItem.vName.bstrVal = bstrItemName;

	*pbItemValid = m_listItem.bFindCell((PVOID) &prItem, (PVOID*) &pPrItem);

	return TRUE;
}


BOOL CChatItems::bGetItem(BSTR &bstrItemName, VARIANT* pvItem)
{
	PPRITEM pPrItem = NULL;
	PRITEM	prItem;

//	OutputDebugThreadIdString("CChatItems::bGetItem - Enter\n");

    ASSERT(bstrItemName, "bstrItemName is NULL in CChatItems::bGetItem");
    ASSERT(pvItem, "pvItem is NULL in CChatItems::bGetItem");

	prItem.vName.vt = VT_BSTR;
	prItem.vName.bstrVal = bstrItemName;

	VariantInit(pvItem);

	if (m_listItem.bFindCell((PVOID) &prItem, (PVOID*) &pPrItem))
	{
		ASSERT(pPrItem, "pPrItem is NULL in CChatItems::get_Item");
		return SUCCEEDED(m_hrLastError = VariantCopy(pvItem, &(pPrItem->vValue)));
	}
	else
	{
		m_hrLastError = CC_E_ITEMNOTAVAILABLE;
		return FALSE;
	}
}


BOOL CChatItems::bSetItem(LPCWSTR wszItemName, VARIANT* pvItem)
{
	BOOL	bItemValid;
	PPRITEM pPrItem = NULL;
	
//	OutputDebugThreadIdString("CChatItems::bSetItem - Enter\n");

	ASSERT(wszItemName, "wszItemName is NULL in CChatItems::bSetItem");
	ASSERT(pvItem, "pvItem is NULL in CChatItems::bSetItem");

	// Items have to be uniquely set
	bGetItemValid((BSTR&) wszItemName, &bItemValid);
	if (bItemValid)
	{
		BOOL bRet = bInvalidateItem((BSTR&) wszItemName);
		ASSERT(bRet, "Could not invalidate item in CChatItems::bSetItem");
	}

	pPrItem = new PRITEM;
	if (!pPrItem)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	VariantInit(&(pPrItem->vName));
	VariantInit(&(pPrItem->vValue));

	pPrItem->vName.vt = VT_BSTR;
	pPrItem->vName.bstrVal = SysAllocString(wszItemName);
	if (!pPrItem->vName.bstrVal)
	{
		delete pPrItem;
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (FAILED(m_hrLastError = VariantCopy(&(pPrItem->vValue), pvItem)))
		return FALSE;

	if (!m_listItem.bAddFrontCell((PVOID) pPrItem))
	{
		SysFreeString(pPrItem->vName.bstrVal);
		VariantClear(&(pPrItem->vValue));
		delete pPrItem;
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	else
		return TRUE;
}


BOOL CChatItems::bSetItem(LPCWSTR wszItemName, BOOL bItemValue)
{
	BOOL	bItemValid;
	PPRITEM pPrItem = NULL;
	
//	OutputDebugThreadIdString("CChatItems::bSetItem - Enter\n");

	ASSERT(wszItemName, "wszItemName is NULL in CChatItems::bSetItem");

	// Items have to be uniquely set
	bGetItemValid((BSTR&) wszItemName, &bItemValid);
	if (bItemValid)
	{
		BOOL bRet = bInvalidateItem((BSTR&) wszItemName);
		ASSERT(bRet, "Could not invalidate item in CChatItems::bSetItem");
	}

	pPrItem = new PRITEM;
	if (!pPrItem)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	VariantInit(&(pPrItem->vName));
	VariantInit(&(pPrItem->vValue));

	pPrItem->vName.vt = VT_BSTR;
	pPrItem->vName.bstrVal = SysAllocString(wszItemName);

	pPrItem->vValue.vt = VT_BOOL;
	pPrItem->vValue.boolVal = (VARIANT_BOOL) bItemValue;
	
	if (!m_listItem.bAddFrontCell((PVOID) pPrItem))
	{
		SysFreeString(pPrItem->vName.bstrVal);
		delete pPrItem;
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	else
		return TRUE;
}


BOOL CChatItems::bSetItem(LPCWSTR wszItemName, LPCTSTR szItemValue, LONG lItemValue)
{
	BOOL	bItemValid;
	PPRITEM pPrItem = NULL;
	
//	OutputDebugThreadIdString("CChatItems::bSetItem - Enter\n");

	ASSERT(wszItemName, "wszItemName is NULL in CChatItems::bSetItem");

	// Items have to be uniquely set
	bGetItemValid((BSTR&) wszItemName, &bItemValid);
	if (bItemValid)
	{
		BOOL bRet = bInvalidateItem((BSTR&) wszItemName);
		ASSERT(bRet, "Could not invalidate item in CChatItems::bSetItem");
	}

	pPrItem = new PRITEM;
	if (!pPrItem)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	VariantInit(&(pPrItem->vName));
	VariantInit(&(pPrItem->vValue));

	pPrItem->vName.vt = VT_BSTR;
	pPrItem->vName.bstrVal = SysAllocString(wszItemName);

	if (szItemValue)
	{
		pPrItem->vValue.vt = VT_BSTR;
		pPrItem->vValue.bstrVal = A2BSTR(szItemValue);
	}
	else
	{
		pPrItem->vValue.vt = VT_I4;
		pPrItem->vValue.lVal = lItemValue;
	}
	
	if (!pPrItem->vName.bstrVal || (szItemValue && !pPrItem->vValue.bstrVal))
	{
		delete pPrItem;
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	if (!m_listItem.bAddFrontCell((PVOID) pPrItem))
	{
		SysFreeString(pPrItem->vName.bstrVal);
		if (szItemValue) SysFreeString(pPrItem->vValue.bstrVal);
		delete pPrItem;
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}
	else
		return TRUE;
}

