/*
	Microsoft Corp. (C) Copyright 1997
	Developed under contract by Numbers & Co.
----------------------------------------------------------------------------

        name:   XCChat - ActiveX CChat-document-container control

	 	file:	xcontrol.h

    comments:	Defines CXControl COM interface to XCChat
     	
----------------------------------------------------------------------------
	Microsoft Corp. (C) Copyright 1997
	Developed under contract by Numbers & Co.
 */





#ifndef __XCONTROL_H_
#define __XCONTROL_H_

#include "resource.h"       // main symbols
#include "framer.h"


/////////////////////////////////////////////////////////////////////////////
// CXControl
class ATL_NO_VTABLE CXControl : 
	public CComObjectRootEx<CComSingleThreadModel>,
	public CComCoClass<CXControl, &CLSID_XControl>,
	public CComControl<CXControl>,
	public IDispatchImpl<IXControl, &IID_IXControl, &LIBID_XCCHATLib>,
	public IProvideClassInfo2Impl<&CLSID_XControl, NULL, &LIBID_XCCHATLib>,
	public IPersistStreamInitImpl<CXControl>,
	public IPersistStorageImpl<CXControl>,
	public IPersistPropertyBagImpl<CXControl>,
	public IQuickActivateImpl<CXControl>,
	public IOleControlImpl<CXControl>,
	public IOleObjectImpl<CXControl>,
	public IOleInPlaceActiveObjectImpl<CXControl>,
	public IViewObjectExImpl<CXControl>,
	public IOleInPlaceObjectWindowlessImpl<CXControl>,
	public IDataObjectImpl<CXControl>,
	public ISupportErrorInfo,
	public IConnectionPointContainerImpl<CXControl>,
	public ISpecifyPropertyPagesImpl<CXControl>
{
public:
	CXControl( void );
   ~CXControl( void );


DECLARE_REGISTRY_RESOURCEID(IDR_XCONTROL)

BEGIN_COM_MAP(CXControl)
	COM_INTERFACE_ENTRY(IXControl)
	COM_INTERFACE_ENTRY( IDispatch )
	COM_INTERFACE_ENTRY_IMPL(IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject2, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IViewObject, IViewObjectEx)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleInPlaceObject, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL_IID(IID_IOleWindow, IOleInPlaceObjectWindowless)
	COM_INTERFACE_ENTRY_IMPL(IOleInPlaceActiveObject)
	COM_INTERFACE_ENTRY_IMPL(IOleControl)
	COM_INTERFACE_ENTRY_IMPL(IOleObject)
	COM_INTERFACE_ENTRY_IMPL(IQuickActivate)
	COM_INTERFACE_ENTRY_IMPL(IPersistStorage)
	COM_INTERFACE_ENTRY_IMPL(IPersistStreamInit)
	COM_INTERFACE_ENTRY_IMPL(IPersistPropertyBag)
	COM_INTERFACE_ENTRY_IMPL(ISpecifyPropertyPages)
	COM_INTERFACE_ENTRY_IMPL(IDataObject)
	COM_INTERFACE_ENTRY(IProvideClassInfo)
	COM_INTERFACE_ENTRY(IProvideClassInfo2)
	COM_INTERFACE_ENTRY(ISupportErrorInfo)
	COM_INTERFACE_ENTRY_IMPL(IConnectionPointContainer)
END_COM_MAP()


BEGIN_PROPERTY_MAP(CXControl)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	PROP_ENTRY("ShowToolBar", 1, CLSID_XControl)
	PROP_ENTRY("JoinNetMeeting", 2, CLSID_XControl)
	PROP_ENTRY("ShowMenu", 3, CLSID_XControl)
	PROP_ENTRY("ShowNetMeeting", 4, CLSID_XControl)
END_PROPERTY_MAP()


BEGIN_CONNECTION_POINT_MAP(CXControl)
END_CONNECTION_POINT_MAP()


BEGIN_MSG_MAP(CXControl)
	MESSAGE_HANDLER(WM_PAINT, OnPaint)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	MESSAGE_HANDLER(WM_KILLFOCUS, OnKillFocus)
END_MSG_MAP()



// ISupportsErrorInfo
	STDMETHODIMP InterfaceSupportsErrorInfo( REFIID riid );

// IViewObjectEx
	STDMETHODIMP GetViewStatus( DWORD* pdwStatus )
	{
		ATLTRACE(_T("IViewObjectExImpl::GetViewStatus\n"));
		*pdwStatus = VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE;
		return S_OK;
	}

	STDMETHODIMP QueryHitPoint( DWORD dwAspect, LPCRECT pRectBounds, POINT ptlLoc, LONG lCloseHit, DWORD* pHitResult );


// IOleObject
	STDMETHODIMP Close( DWORD dwSaveOption );
	STDMETHODIMP DoVerb( LONG iVerb, LPMSG lpmsg, 
						 IOleClientSite* pActiveSite, LONG lindex, 
						 HWND hwndParent, LPCRECT lprcPosRect );



// IOleInPlaceObject
	STDMETHODIMP InPlaceDeactivate( void );
	STDMETHODIMP UIDeactivate( void );
	STDMETHODIMP SetObjectRects( LPCRECT prcPos, LPCRECT prcClip );

	
// IPersistStorage
	STDMETHODIMP InitNew( IStorage *pStg );


// IPersistStreamInit
	STDMETHODIMP InitNew( void );


// IPersistPropertyBag
	STDMETHODIMP Load( LPPROPERTYBAG pPropBag, LPERRORLOG pErrorLog );


// IDispatch
	STDMETHODIMP GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid );
	STDMETHODIMP GetTypeInfo( UINT itinfo, LCID lcid, ITypeInfo** pptinfo );
	STDMETHODIMP GetTypeInfoCount( UINT* pctinfo );
	STDMETHODIMP Invoke( DISPID dispidMember, REFIID riid, LCID lcid, 
						 WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, 
						 EXCEPINFO* pexcepinfo, UINT* puArgErr );



// IXControl
public:
	STDMETHOD(get_ShowNetMeeting)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_ShowNetMeeting)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_ShowMenu)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_ShowMenu)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_JoinNetMeeting)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_JoinNetMeeting)(/*[in]*/ BOOL newVal);
	STDMETHOD(get_ShowToolBar)(/*[out, retval]*/ BOOL *pVal);
	STDMETHOD(put_ShowToolBar)(/*[in]*/ BOOL newVal);
	HWND CreateControlWindow( HWND hWndParent, RECT &rcPos );
	HRESULT OnDraw(ATL_DRAWINFO& di);
	HRESULT ControlQueryInterface( const IID& iid, void** ppv );
	HRESULT FinalConstruct( void );
	void FinalRelease( void );



private:
	BOOL CreateGUIDSite( HWND hMainWnd, RECT *prcPos, HINSTANCE hInstance );
	void DeleteGUIDSite( void );
	BOOL LaunchForNM( void );

    CFrame *m_pFR;   
	IStorage *m_pStorage;
	IStorage *m_pPrivateStorage;
    LPUNKNOWN m_pObj;
    LPUNKNOWN m_pObjDispatch;

	IOleInPlaceFrame    *m_pFrame;
	IOleInPlaceUIWindow *m_pDoc;

	TCHAR m_szCChatCmd[2*MAX_PATH];

	BOOL m_bShowToolbar;
	BOOL m_bJoinNM;
	BOOL m_bShowMenu;
	BOOL m_bShowNM;

	BOOL m_bDelayedOpenNM;
	BOOL m_bOkToCreateDocObj;

};

#endif //__XCONTROL_H_
