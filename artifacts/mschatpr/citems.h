//=--------------------------------------------------------------------------=
// CItems.H		-	CChatItems class definition
//=--------------------------------------------------------------------------=
// Copyright 1995-1997 Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=
//
// class declaration for the MsChatPr ChatItems Object.
//
#ifndef __CITEMS_H__

#include "CLList.H"
#include "CCError.H"
#include "PrCnst.H"


//=--------------------------------------------------------------------------=
// CChatItems
//=--------------------------------------------------------------------------=
//
class CChatItems
{
  public:
					CChatItems(void);
					~CChatItems(void);

	HRESULT			HrGetLastError(void)
							{ return m_hrLastError; }
	BSTR			BstrGetAssociatedType(void)
							{ return m_bstrAssociatedType; }
	CItemLList*		GetItemList(void)
							{ return &m_listItem; }

    BOOL			bGetAssociatedType(BSTR* pbstrAssociatedType);
    BOOL			bSetAssociatedType(LPCWSTR wszAssociatedType);
    BOOL			bGetValidItems(VARIANT* pvValidItems);
    BOOL			bGetItemValid(BSTR &bstrItemName, BOOL* pbItemValid);
    BOOL			bGetItem(BSTR &bstrItemName, VARIANT* pvItem);
    BOOL			bSetItem(LPCWSTR wszItemName, VARIANT* pvItem);
	BOOL			bSetItem(LPCWSTR wszItemName, BOOL bItemValue);
	BOOL			bSetItem(LPCWSTR wszItemName, LPCTSTR szItemValue, LONG lItemValue);
    BOOL			bInvalidateItem(BSTR &bstrItemName);

	virtual	PVOID		GetAutObject(void) = 0;
	virtual	IDispatch	*GetDispatch(void) = 0;

  private:
	// private attributes
	CItemLList		m_listItem;
	BSTR			m_bstrAssociatedType;
	HRESULT			m_hrLastError;
};


#define __CITEMS_H__
#endif // __CITEMS_H__
