// ChCol.h : Declaration of the CChannelsCollection

#ifndef __CHANNELSCOLLECTION_H_
#define __CHANNELSCOLLECTION_H_

#include "Resource.H"       // main symbols
#include "CUtil.H"
#include "CCSock.H"


class CChatSocket;

/////////////////////////////////////////////////////////////////////////////
// CChannelsCollection
class ATL_NO_VTABLE CChannelsCollection : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CChannelsCollection, &CLSID_Channels>,
	public ISupportErrorInfo,
	public IDispatchImpl<IChannelsCollection, &IID_IChannelsCollection, &LIBID_MsChatPr, 2, 0>
{
public:
	CChannelsCollection()
	{
		OutputDebugThreadIdString("CChannelsCollection::CChannelsCollection - Enter\n");

		m_plistChannel	= NULL;
		m_pccsock		= NULL;
	}

	CChannelsCollection::~CChannelsCollection ()
	{
		OutputDebugThreadIdString("CChannelsCollection::~CChannelsCollection - Enter\n");
	}


DECLARE_REGISTRY_RESOURCEID(IDR_CHANNELSCOLLECTION)

BEGIN_COM_MAP(CChannelsCollection)
	COM_INTERFACE_ENTRY(IChannelsCollection)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IChannelsCollection
public:
	STDMETHOD(Remove)(/*[in]*/ VARIANT vIndex);
	STDMETHOD(Add)(/*[in,optional]*/ VARIANT vIndex, /*[in,optional]*/ VARIANT vKey, /*[out, retval]*/ IChannelObject** ppChannelObject);
	STDMETHOD(get_Count)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get__NewEnum)(/*[out, retval]*/ IUnknown* *pVal);
	STDMETHOD(get_Item)(/*[in,optional]*/ VARIANT vIndex, /*[out, retval]*/ IChannelObject* *pVal);

	CChanLList*		m_plistChannel;	// channels list
	CChatSocket*	m_pccsock;

private:
	// private function members
	HRESULT	HrThrowError(SCODE sc, LPCTSTR szChannelName = NULL, LPCTSTR szNickname = NULL);
};

#endif //__CHANNELSCOLLECTION_H_
