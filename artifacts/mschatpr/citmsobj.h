// CItmsObj.h : Declaration of the CItemsObject

#ifndef __CHATITEMS_H_
#define __CHATITEMS_H_

#include "Resource.H"       // main symbols
#include "CItems.H"
#include "PrErr.H"

/////////////////////////////////////////////////////////////////////////////
// CItemsObject
class ATL_NO_VTABLE CItemsObject : 
	public CChatItems,
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CItemsObject, &CLSID_ChatItems>,
	public ISupportErrorInfo,
	public IDispatchImpl<IChatItems, &IID_IChatItems, &LIBID_MsChatPr, 2, 0>
{
public:
	CItemsObject()
	{
	//	OutputDebugThreadIdString("CItemsObject::CItemsObject - Enter\n");
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CHATITEMS)

BEGIN_COM_MAP(CItemsObject)
	COM_INTERFACE_ENTRY(IChatItems)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IChatItems
public:
	STDMETHOD(InvalidateItem)(/*[in]*/ BSTR ItemName);
	STDMETHOD(get_Item)(/*[in]*/ BSTR ItemName, /*[out, retval]*/ VARIANT *pvItem);
	STDMETHOD(put_Item)(/*[in]*/ BSTR ItemName, /*[in]*/ VARIANT vItem);
	STDMETHOD(get_ItemValid)(/*[in]*/ BSTR ItemName, /*[out, retval]*/ BOOL *pbItemValid);
	STDMETHOD(get_ValidItems)(/*[out, retval]*/ VARIANT *pvValidItems);
	STDMETHOD(get_AssociatedType)(/*[out, retval]*/ BSTR *pbstrAssociatedType);
	STDMETHOD(put_AssociatedType)(/*[in]*/ BSTR bstrAssociatedType);

private:
			HRESULT		HrThrowError(SCODE sc);

	virtual	PVOID		GetAutObject(void) { return (PVOID) this; }
	virtual	IDispatch	*GetDispatch(void);
};

#endif //__CHATITEMS_H_
