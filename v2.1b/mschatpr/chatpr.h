// ChatPr.h : Declaration of the CMsChatPr

#ifndef __CHATPR_H__
#define __CHATPR_H__

#include "Resource.H"       // main symbols
#include "CCSock.H"
#include "CEvents.h"

/////////////////////////////////////////////////////////////////////////////
// CMsChatPr
class ATL_NO_VTABLE CMsChatPr :
	public CChatSocket, 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CMsChatPr, &CLSID_MsChatPr>,
	public CComControl<CMsChatPr>,
	public IDispatchImpl<IMsChatPr, &IID_IMsChatPr, &LIBID_MsChatPr, 2, 0>,
	public IProvideClassInfo2Impl<&CLSID_MsChatPr, &DIID__IMsChatPrEvent, &LIBID_MsChatPr, 2, 0>,
	public IPersistStreamInitImpl<CMsChatPr>,
	public IPersistStorageImpl<CMsChatPr>,
	public IOleControlImpl<CMsChatPr>,
	public IOleObjectImpl<CMsChatPr>,
	public IOleInPlaceActiveObjectImpl<CMsChatPr>,
	public IViewObjectExImpl<CMsChatPr>,
	public IOleInPlaceObjectWindowlessImpl<CMsChatPr>,
	public CProxy_IMsChatPrEvent<CMsChatPr>,
	public IConnectionPointContainerImpl<CMsChatPr>,
	public ISupportErrorInfo,
	public IObjectSafetyImpl<CMsChatPr>
//	public IQuickActivateImpl<CMsChatPr>,	// IQuickActivate and 
//	public IPropertyNotifySinkCP<CMsChatPr>,// IPropertyNotifySinkCP have to come together
//	public IDataObjectImpl<CMsChatPr>,
//	public ISpecifyPropertyPagesImpl<CMsChatPr>
{
	// window proc of messaging window
	friend LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CMsChatPr(void)
	{
		OutputDebugThreadIdString("CMsChatPr::CMsChatPr - Enter\n");

		bConstruct();
	}

	~CMsChatPr(void)
	{
		OutputDebugThreadIdString("CMsChatPr::~CMsChatPr - Enter\n");
		
		Destruct();

		OutputDebugThreadIdString("CMsChatPr::~CMsChatPr - Leave\n");
	}

DECLARE_REGISTRY_RESOURCEID(IDR_MSCHATPR)

BEGIN_COM_MAP(CMsChatPr)
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(IMsChatPr)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject2, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL(IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY_IMPL(IOleControl)
	COM_INTERFACE_ENTRY_IMPL(IOleObject)
	COM_INTERFACE_ENTRY_IMPL(IPersistStreamInit)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
	COM_INTERFACE_ENTRY_IMPL(IPersistStorage)
	COM_INTERFACE_ENTRY_IMPL(IObjectSafety)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
//	COM_INTERFACE_ENTRY_IMPL(IQuickActivate)
//	COM_INTERFACE_ENTRY_IMPL(ISpecifyPropertyPages)
//	COM_INTERFACE_ENTRY_IMPL(IDataObject)
END_COM_MAP()


BEGIN_PROPERTY_MAP(CMsChatPr)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
//	PROP_PAGE(CLSID_StockColorPage)
END_PROPERTY_MAP()


BEGIN_CONNECTION_POINT_MAP(CMsChatPr)
//	CONNECTION_POINT_ENTRY(IID_IPropertyNotifySink) // when IPropertyNotifySinkCP is implemented
	CONNECTION_POINT_ENTRY(DIID__IMsChatPrEvent)
END_CONNECTION_POINT_MAP()


BEGIN_MSG_MAP(CMsChatPr)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
END_MSG_MAP()


// ISupportsErrorInfo
	STDMETHOD(InterfaceSupportsErrorInfo)(REFIID riid);

// IViewObjectEx
	STDMETHOD(GetViewStatus)(DWORD* pdwStatus)
	{
		ATLTRACE(_T("IViewObjectExImpl::GetViewStatus\n"));
		*pdwStatus = VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE;
		return S_OK;
	}

// IMsChatPr
public:
	STDMETHOD(SendProtocolMessage)(/*[in]*/ BSTR TextMessage);
	STDMETHOD(SendPrivateMessage)(/*[in]*/ long PrivateMessageType, /*[in]*/ VARIANT Message, /*[in]*/ VARIANT RecipientNicknames, /*[in,optional]*/ VARIANT DataMessageTag);
	STDMETHOD(SendInvitation)(/*[in]*/ BSTR Nickname, /*[in,optional]*/ VARIANT ChannelName);
	STDMETHOD(QueryServerProperty)(/*[in]*/ BSTR ServerPropertyName);
	STDMETHOD(SetAway)(/*[in]*/ BOOL Away, /*[in,optional]*/ VARIANT TextMessage);
	STDMETHOD(ChangeNickname)(/*[in]*/ BSTR NewNickname);
	STDMETHOD(KickUser)(/*[in]*/ BSTR Nickname, /*[in,optional]*/ VARIANT Reason);
	STDMETHOD(BanUsers)(/*[in]*/ ChatItems* UserItems, /*[in]*/ BOOL Set, /*[in,optional]*/ VARIANT Reason, /*[in,optional]*/ VARIANT Duration);
	STDMETHOD(IgnoreUsers)(/*[in]*/ ChatItems* UserItems, /*[in]*/ BOOL Set);
	STDMETHOD(ChangeUserProperty)(/*[in]*/ BSTR UserPropertyName, /*[in]*/ VARIANT NewUserProperty, /*[in,optional]*/ VARIANT Nickname);
	STDMETHOD(QueryUserProperty)(/*[in]*/ BSTR UserPropertyName, /*[in,optional]*/ VARIANT Nickname);
	STDMETHOD(ListUsers)(/*[in]*/ ChatItems* UserQueryItems);
	STDMETHOD(KillChannel)(/*[in]*/ BSTR ChannelName, /*[in,optional]*/ VARIANT Reason);
	STDMETHOD(QueryChannelProperty)(/*[in]*/ BSTR ChannelPropertyName, /*[in,optional]*/ VARIANT ChannelName);
	STDMETHOD(ListChannels)(/*[in]*/ ChatItems* ChannelQueryItems);
	STDMETHOD(CancelConnectLogin)();
	STDMETHOD(Disconnect)();
	STDMETHOD(Login)(/*[in]*/ BSTR ServerName, /*[in]*/ BSTR Nickname, /*[in,optional]*/ VARIANT UserName, /*[in,optional]*/ VARIANT RealName, /*[in,optional]*/ VARIANT Password, /*[in,optional]*/ VARIANT SecurityPackages);
	STDMETHOD(Connect)(/*[in]*/ BSTR ServerName);
	STDMETHOD(AboutBox)();
	STDMETHOD(get_ServerProperty)(/*[in]*/ BSTR ServerPropertyName, /*[out, retval]*/ IChatItems* *pVal);
	STDMETHOD(get_ChannelProperty)(/*[in]*/ BSTR ChannelPropertyName, /*[in,optional]*/ VARIANT ChannelName, /*[out, retval]*/ IChatItems* *pVal);
	STDMETHOD(get_UserProperty)(/*[in]*/ BSTR UserPropertyName, /*[in,optional]*/ VARIANT Nickname,/*[out, retval]*/ IChatItems* *pVal);
	STDMETHOD(get_Nickname)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ServerName)(/*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_MaxStringLength)(/*[in]*/ enumStringType StringType, /*[out, retval]*/ long *pVal);
	STDMETHOD(get_NewChatItems)(/*[out, retval]*/ IChatItems* *pVal);
	STDMETHOD(get_Channels)(/*[out, retval]*/ IDispatch* *pVal);
	STDMETHOD(get_ConvertedString)(/*[in]*/ enumConversionType ConversionType, /*[in]*/ enumConversionSource ConversionSource, /*[in]*/ BSTR Source, /*[in]*/ BOOL PostProcess, /*[out, retval]*/ BSTR *pVal);
	STDMETHOD(get_ProcessProtocolMessages)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_ProcessProtocolMessages)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_PropertyAccessTimeOut)(/*[out, retval]*/ long *pVal);
	STDMETHOD(put_PropertyAccessTimeOut)(/*[in]*/ long newVal);
	STDMETHOD(get_OpeningChannels)(/*[out, retval]*/ long *pVal);
	STDMETHOD(get_ConnectionType)(/*[out, retval]*/ enumConnectionType *pVal);
	STDMETHOD(get_ConnectionState)(/*[out, retval]*/ enumConnectionState *pVal);

	HRESULT OnDraw(ATL_DRAWINFO& di);

private:
	// private member functions
	BOOL	bCreateChatItems(CChatItems **ppCItems);
	void	ReleaseChatItems(CChatItems *pCItems);

	BOOL	bFireInvitation(LPCTSTR szChannelName, CChatItems* pCItems);
	BOOL	bFireChannelPropertyChanged(CChatChannel* pChannel, LPCTSTR szModifierNickname, LPCWSTR wszChannelPropertyName, VARIANT *pvOldChannelProperty, VARIANT *pvNewChannelProperty);
	BOOL	bFireMemberPropertyChanged(CChatChannel* pChannel, LPCTSTR szModifiedNickname, LPCTSTR szModifierNickname, LPCWSTR wszMemberPropertyName, VARIANT *pvOldMemberProperty, VARIANT *pvNewMemberProperty);
	BOOL	bFireMemberKicked(CChatChannel* pChannel, LPCTSTR szKickedNickname, LPCTSTR szKickerNickname, LPCTSTR szReason);
	BOOL	bFireUserPropertyChanged(LPCTSTR szModifiedNickname, LPCTSTR szModifierNickname, LPCWSTR wszUserPropertyName, VARIANT *pvOldUserProperty, VARIANT *pvNewUserProperty);
	BOOL	bFireDataMessage(CChatChannel* pChannel, LPCTSTR szFromNickname, VARIANT *pvRcpNicks, LPCTSTR szDataMessageTag, LPCTSTR szDataMessage, SHORT nCmd);
	BOOL	bFireTextMessage(CChatChannel* pChannel, LPCTSTR szFromNickname, VARIANT *pvRcpNicks, LPCTSTR szTextMessage, LONG msgt, LONG pmt = pmtNormal);
	void	FireKnock(CChatChannel* pChannel, CChatItems* pCItems, LONG lErrorCode);
	void	FireDelMember(CChatChannel* pChannel, LPCTSTR szNickname);
	void	FireAddMember(CChatChannel* pChannel, LPCTSTR szNickname, CChatItems* pCItems);
	void	FireBeginEnumeration(CChatChannel* pChannel, enumEnumType et);
	void	FireEndEnumeration(CChatChannel* pChannel, enumEnumType et);
	void	FireChannelState(CChatChannel* pChannel, enumChannelState chsNewChannelState);
	void	FireUserProperty(CChatItems *pCItems);
	void	FireServerProperty(CChatItems *pCItems);
	void	FireMemberProperty(CChatChannel* pChannel, CChatItems* pCItems);

	void	FireChannelProperty(CChatChannel* pChannel, CChatItems* pCItems);
	void	DisplayDesignIcon(HDC hdcDraw, LPCRECTL prcBounds, WORD wIDB);
	void	FireConnectionError(LPCTSTR szChannelName, LPCTSTR szNickname, HRESULT hr);
	void	FireChannelError(CChatChannel* pChannel, LPCTSTR szNickname, HRESULT hr);
	void	FireConnectionState(enumConnectionState csNewConnectionState);
	void	FireServerTextMessage(enumServerMessageType smt, LPCTSTR szText);
	void	FireProtocolMessage(PPRIRCMSG pPrIrcMsg, BOOL *pbEnableDefault);
	void	TreatIrcMsg(PPRIRCMSG pPrIrcMsg, BOOL bFree = TRUE);
	void	HandleCommand(PPRIRCMSG pPrIrcMsg);
	void	HandleErrors(UINT uCode, PPRIRCMSG pPrIrcMsg);
	void	HandleResultCode(UINT uCode, PPRIRCMSG pPrIrcMsg);
	void	ExposeChannelItems(PPRQUERY pPrQuery, BOOL bRemoveFromQueryList);
	void	ExposeChannelItems(PPRCHANNEL pPrChannel);
	void	ExposeMemberItems(PPRQUERY pPrQuery, BOOL bRemoveFromQueryList);
	void	ExposeUserItems(PPRQUERY pPrQuery, PPRUSER pPrUser);
	void	ExposeServerItems(void);

	BOOL	bCreateMessagingWindow(HWND *phwnd, LPCTSTR lpClassName, DWORD dwUserData);
	BOOL	bSetMessagingWindows(void);

	BOOL	bConstruct(void);
	void	Destruct(void);

	HRESULT	HrThrowError(SCODE sc, LPCTSTR szChannelName = NULL, LPCTSTR szNickname = NULL);

	void	DisplayDesignIcon(HDC hdcDraw, DWORD dwDrawAspect, LPCRECTL prcBounds, WORD wIDB);

	BOOL					m_bCtrlDestructed;
	BOOL					m_bProcessProtocolMessages;

	TCHAR					m_szOOM[g_nMaxLengthError];	// message used when OOM condition occurs

	IDispatch*				m_pccdisp;
	CChannelsCollection*	m_pcc;
};

#endif //__CHATPR_H__
