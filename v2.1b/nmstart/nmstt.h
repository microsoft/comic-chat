// NMStt.h : Declaration of the CNMStt

#ifndef __NMSTT_H_
#define __NMSTT_H_

#include "Resource.H"       // main symbols
#include "Debug.H"
#include "NMCnst.H"
#include "NMErr.H"
#include "NMEvents.H"

/////////////////////////////////////////////////////////////////////////////
// CNMStt
class ATL_NO_VTABLE CNMStt :
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CNMStt,&CLSID_NMStart>,
	public CComControl<CNMStt>,
	public IDispatchImpl<INMStt, &IID_INMStt, &LIBID_NMSTARTLib>,
	public IPersistStreamInitImpl<CNMStt>,
	public IOleControlImpl<CNMStt>,
	public IOleObjectImpl<CNMStt>,
	public IOleInPlaceActiveObjectImpl<CNMStt>,
	public IViewObjectExImpl<CNMStt>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CNMStt>,
	public IOleInPlaceObjectWindowlessImpl<CNMStt>,
	public IProvideClassInfo2Impl<&CLSID_NMStart, &DIID__INMSttEvent, &LIBID_NMSTARTLib, 1, 0>,
	public IObjectSafetyImpl<CNMStt>,
	public CProxy_INMSttEvent<CNMStt>
{
	// window proc of messaging window
	friend LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	CNMStt()
	{
		OutputDebugThreadIdString("CNMStt::CNMStt - Enter\n"); 

		m_hwndMess				= NULL;
		m_hThreadListen			= NULL;
		m_hThreadConnect		= NULL;
		m_bAcceptNetMeetingCalls= TRUE;
		m_bCtrlDestructed		= FALSE;
		for (SHORT n = 0; n < g_nMaxConnectCalls; n++)
			m_dwIPAddress[n] = 0L;
		bCreateMessagingWindow(&m_hwndMess, g_szMsgWndClass, (DWORD) this);
	}

	~CNMStt(void)
	{
		OutputDebugThreadIdString("CNMStt::~CNMStt - Enter\n");

		m_bCtrlDestructed = TRUE;

		WaitForThreadDead(&m_hThreadListen);
		WaitForThreadDead(&m_hThreadConnect);

		if (m_hwndMess)
			::DestroyWindow(m_hwndMess);

		OutputDebugThreadIdString("CNMStt::~CNMStt - Leave\n");
	}


DECLARE_REGISTRY_RESOURCEID(IDR_NMSTT)

BEGIN_COM_MAP(CNMStt) 
	COM_INTERFACE_ENTRY(IDispatch)
	COM_INTERFACE_ENTRY(INMStt)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject2, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL(IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY_IMPL(IOleControl)
	COM_INTERFACE_ENTRY_IMPL(IOleObject)
	COM_INTERFACE_ENTRY_IMPL(IPersistStreamInit)
	COM_INTERFACE_ENTRY_IMPL(IObjectSafety)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()

BEGIN_PROPERTY_MAP(CNMStt)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROPERTY_MAP()


BEGIN_CONNECTION_POINT_MAP(CNMStt)
	CONNECTION_POINT_ENTRY(DIID__INMSttEvent)
END_CONNECTION_POINT_MAP()


BEGIN_MSG_MAP(CNMStt)
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

// INMStt
	STDMETHOD(get_AcceptNetMeetingCalls)(/*[out, retval]*/ BOOL *pbAcceptNetMeetingCalls);
	STDMETHOD(put_AcceptNetMeetingCalls)(/*[in]*/ BOOL bAcceptNetMeetingCalls);
	STDMETHOD(get_NetMeetingInstalled)(/*[out, retval]*/ BOOL *pbNetMeetingInstalled);
	STDMETHOD(get_IPAddress)(/*[out, retval]*/ BSTR *pbstrIPAddress);

	STDMETHOD(OpenNetMeetingConference)(/*[in]*/ BSTR bstrIPAddress);
	STDMETHOD(InstallNetMeeting)(void);
	STDMETHOD(StartNetMeetingListen)(void);

public:
	HRESULT OnDraw(ATL_DRAWINFO& di);
	HRESULT	HrThreadConferenceListen(void);
	HRESULT HrThreadConferenceConnect(void);
	HRESULT HrThrowError(SCODE sc);
	UINT	Sc2ID(SCODE sc);
	BOOL	bCreateMessagingWindow(HWND *phwnd, LPCTSTR lpClassName, DWORD dwUserData);
	void	DisplayDesignIcon(HDC hdcDraw, DWORD dwDrawAspect, LPCRECTL prcBounds, WORD wIDB);
	void	WaitForThreadDead(HANDLE *phThread);
	void	FireOperationDone(DWORD dwResult, UINT uDispID);

	HWND	m_hwndMess;
	HANDLE	m_hThreadListen, m_hThreadConnect;
	BOOL	m_bAcceptNetMeetingCalls;
	BOOL	m_bCtrlDestructed;
	DWORD	m_dwIPAddress[g_nMaxConnectCalls];
};

#endif //__NMSTT_H_
