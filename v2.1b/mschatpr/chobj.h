// ChObj.h : Declaration of the CChannelObject

#ifndef __CHANNELOBJECT_H_
#define __CHANNELOBJECT_H_

#include "Resource.H"       // main symbols
#include "CChan.H"
#include "PrErr.H"

// Macros
#define bMaxMemberCountValid(MaxMemberCount)	(MaxMemberCount >= 0)

class CChannelsCollection;

/////////////////////////////////////////////////////////////////////////////
// CChannelObject
class ATL_NO_VTABLE CChannelObject :
	public CChatChannel, 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CChannelObject, &CLSID_Channel>,
	public ISupportErrorInfo,
	public IDispatchImpl<IChannelObject, &IID_IChannelObject, &LIBID_MsChatPr, 2, 0>
{
	friend class CChannelsCollection;

public:
	CChannelObject()
	{
		OutputDebugThreadIdString("CChannelObject::CChannelObject - Enter\n");

		m_bInCollection	= FALSE;
		m_szItemKey		= NULL;
	}

	CChannelObject::~CChannelObject ()
	{
		OutputDebugThreadIdString("CChannelObject::~CChannelObject - Enter\n");

		if (m_szItemKey)
			delete [] m_szItemKey;
	}

DECLARE_REGISTRY_RESOURCEID(IDR_CHANNELOBJECT)

BEGIN_COM_MAP(CChannelObject)
	COM_INTERFACE_ENTRY(IChannelObject)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
END_COM_MAP()

// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IChannelObject
public:
	STDMETHOD(ChangeChannelProperty)(/*[in]*/ BSTR ChannelPropertyName, /*[in]*/ VARIANT NewChannelProperty);
	STDMETHOD(QueryChannelProperty)(/*[in]*/ BSTR ChannelPropertyName);
	STDMETHOD(KickMember)(/*[in]*/ BSTR MemberNickname, /*[in,optional]*/ VARIANT Reason);
	STDMETHOD(BanMembers)(/*[in]*/ ChatItems* MemberItems, /*[in]*/ BOOL Set);
	STDMETHOD(ChangeMemberProperty)(/*[in]*/ BSTR MemberPropertyName, /*[in]*/ VARIANT NewMemberProperty, /*[in,optional]*/ VARIANT MemberNickname);
	STDMETHOD(QueryMemberProperty)(/*[in]*/ BSTR MemberPropertyName, /*[in,optional]*/ VARIANT MemberNickname);
	STDMETHOD(ListMembers)(/*[in]*/ ChatItems* MemberQueryItems);
	STDMETHOD(SendInvitation)(/*[in]*/ BSTR Nickname);
	STDMETHOD(SendMessage)(/*[in]*/ long MessageType, /*[in]*/ VARIANT Message, /*[in,optional]*/ VARIANT RecipientNicknames, /*[in,optional]*/ VARIANT DataMessageTag);
	STDMETHOD(LeaveChannel)();
	STDMETHOD(JoinChannel)(/*[in,optional]*/ VARIANT ChannelName, /*[in,optional]*/ VARIANT ChannelKeyword);
	STDMETHOD(CreateChannel)(/*[in,optional]*/ VARIANT ChannelName, /*[in,optional]*/ VARIANT ChannelKeyword, /*[in,optional]*/ VARIANT ChannelModes, /*[in,optional]*/ VARIANT MaxMemberCount);
	STDMETHOD(get_MemberProperty)(/*[in]*/ BSTR MemberPropertyName, /*[in,optional]*/ VARIANT vMemberNickname, /*[out, retval]*/ IChatItems* *pVal);
	STDMETHOD(get_ChannelProperty)(/*[in]*/ BSTR ChannelPropertyName, /*[out, retval]*/ IChatItems* *pVal);
	STDMETHOD(get_MemberModes)(/*[in,optional]*/ VARIANT vMemberNickname, /*[out, retval]*/ long *pVal);
	STDMETHOD(get_MemberIdentity)(/*[in,optional]*/ VARIANT vMemberNickname, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_MemberCount)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ChannelTopic)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ChannelName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(put_ChannelName)(/*[in]*/ BSTR newVal);
	STDMETHOD(get_ChannelState)(/*[out, retval]*/ enumChannelState *pVal);
	STDMETHOD(get_Index)(/*[out, retval]*/ long *pVal);

	LPCTSTR	SzGetItemKey()
					{ return m_szItemKey; }
	HRESULT	HrSetItemKey(LPCTSTR szItemKey);

private:
	// private function members
	virtual	PVOID	GetAutObject() { return (PVOID) this; }

	HRESULT	HrThrowError(SCODE sc, LPCTSTR szChannelName = NULL, LPCTSTR szNickname = NULL);

	BOOL		m_bInCollection;					// Is this channel object in the collection?
	LPTSTR		m_szItemKey;						// Key of the collection item
};

#endif //__CHANNELOBJECT_H_
