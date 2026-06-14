/*
	Microsoft Corp. (C) Copyright 1997
	Developed under contract by Numbers & Co.
----------------------------------------------------------------------------

        name:   XCChat - ActiveX CChat-document-container control

	 	file:	xcontrol.h

    comments:	Implements CXControl COM interface
     	
----------------------------------------------------------------------------
	Microsoft Corp. (C) Copyright 1997
	Developed under contract by Numbers & Co.
 */



#include "stdafx.h"
#include "xcchat.h"
#include "XControl.h"
#include "site.h"


#define INITGUID
#include <initguid.h>



// The following def makes XCChat pass IDispatch calls to CChat's IDispatch
// dspinterface. This should be def'd for normal operation but I comment it out
// for testing purposes sometimes. This breaks the vtbl part of CChat's dual
// IDispatch interface but it was the only way to get the html PARAM tag to work 
// with XCChat for its "command line" switches. The one alternative would be to 
// duplicate CChat's entire IDispatch interface and relay individual func
// calls to CChat but I thought that would be a maintenance nightmare and would
// destroy the generality of this control (other apps can be used by changing the
// guids below). So, I didn't do that. Since we only care about this working 
// in a web page anyway, having access to just the dspinterface of CChat's IDispatch 
// is an acceptable tradeoff. Why didn't I use aggregation?...because that completely 
// bypasses XCChat and the PARAM tag is useless (I was using aggregation at one point...). 
// Look at CXControl::Invoke below to see why I did it this way instead of aggregating. 
#define PASS_DISPATCH_INTERFACE_TO_DOC_OBJECT


// Modify if more dspinterface funcs are added to XCChat. Make sure they don't overlap
// CChat's ids (which are >= 0x60020000 right now).
#define LAST_XCCHAT_DSPID  4


DEFINE_GUID( CLSID_XCChat, 
// CChat
	0x241AF500, 0x8FB6, 0x11CF, 0xAD, 0xC5, 0x00, 0xAA, 0x00, 0xBA, 0xDF, 0x6F ); 

// testsrvr	- comment out PASS_DISPATCH_INTERFACE_TO_DOC_OBJECT
//	0x3B6B3023, 0x82DD, 0x11D1, 0x96, 0x4C, 0x00, 0xC0, 0x4F, 0xBB, 0xBB, 0x03 );

// excel worksheet - comment out PASS_DISPATCH_INTERFACE_TO_DOC_OBJECT
//	0x00030000, 0x0000, 0x0000, 0xC0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x46 ); 



const IID  IID_IXCChatDualDispatch = CLSID_XCChat;





// CXControl
CXControl::CXControl()
{
	ATLTRACE( "CXControl::CXControl\n" );

	m_pFR = NULL;
	m_pStorage = NULL;
	m_pPrivateStorage = NULL;
    m_pObj = NULL;
    m_pObjDispatch = NULL;
	m_pClassFactory = NULL;

	m_szCChatCmd[0] = '\0';
 
	m_bWindowOnly = TRUE; // MUST HAVE THIS FOR IE4 !!!!!!

	m_bShowToolbar = TRUE;
	m_bJoinNM = FALSE;
	m_bShowMenu = TRUE;
	m_bShowNM = FALSE;

	m_bDelayedOpenNM = FALSE;
	m_bOkToCreateDocObj = FALSE;
}







CXControl::~CXControl()
{
	ATLTRACE( "CXControl::~CXControl\n" );

	// CChat hasn't shutdown completely yet. We have to wait for that
	// or we blow up in IE4 later.
	if (m_pFR) 
	{
		m_pFR->WaitDestroyClientSite ();
		delete m_pFR;
	}
}





STDMETHODIMP CXControl::InterfaceSupportsErrorInfo(REFIID riid)
{
	ATLTRACE( "CXControl::InterfaceSupportsErrorInfo\n" );

	static const IID* arr[] = 
	{
		&IID_IXControl,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++) 
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}







STDMETHODIMP  CXControl::InPlaceDeactivate( void )
{
	ATLTRACE( "CXControl::InPlaceDeactivate\n" );

	HRESULT hr;

	if( m_pFR != NULL )
	{
		ATLTRACE( "starting to shutdown, setting CFrame's m_bIsClosing\n" );
		m_pFR->SetIsClosing();
	}

	DeleteGUIDSite();

	hr = IOleInPlaceObjectWindowlessImpl<CXControl>::InPlaceDeactivate();

	return( hr );

}








STDMETHODIMP CXControl::UIDeactivate( void )
{
	ATLTRACE( "CXControl::UIDeactivate\n" );

	HRESULT hr = S_OK;

	// remove doc object's shared menu
	if( m_pFR != NULL )
	{
		ATLTRACE( "removing doc object's shared menu\n" );
		m_pFR->RemoveSharedMenu();
	}

	hr = IOleInPlaceObjectWindowlessImpl<CXControl>::UIDeactivate();

	::InvalidateRect( m_hWndCD, NULL, TRUE );
	::UpdateWindow( m_hWndCD );

	return( hr );

}







STDMETHODIMP CXControl::Close( DWORD dwSaveOption )
{
	ATLTRACE( "CXControl::Close\n" );

	HRESULT hr;

	hr = IOleObjectImpl<CXControl>::Close( dwSaveOption );

	return( hr );

}





STDMETHODIMP CXControl::InitNew( IStorage *pStg )
{
	ATLTRACE( "CXControl<IPersistStorage>::InitNew(IStorage)\n" );

	HRESULT hr;

	m_pStorage = pStg;

	// see if we should join NetMeeting
	if( m_bJoinNM )
	{
		if( (m_bOkToCreateDocObj = LaunchForNM()) == FALSE )
		{
			// somethings broke, just show CChat background bmp
			return( S_OK );
		}
	}
	else
		m_bOkToCreateDocObj = TRUE;


	ATLTRACE( "making doc object\n" );
	hr = OleCreate( CLSID_XCChat, 
					IID_IUnknown, 
					OLERENDER_NONE,
					NULL,
					NULL,
					m_pStorage,
					(void **)&m_pObj );


	if( m_pObj != NULL )
	{
		hr = OleRun(m_pObj); // if already running for NM from above then no problem
		if( FAILED( hr ) )
		{
			ATLTRACE( "can't run object (hr = %d)\n", hr );
			goto bail_out;
		}
		else
		{
			ATLTRACE( "running object (hr = %d)\n", hr );

			m_pObj->QueryInterface( IID_IClassFactory, (void **)&m_pClassFactory );

			if( m_pClassFactory != NULL )
			{
				ATLTRACE( "locking running object\n" );
				//m_pClassFactory->LockServer( TRUE );
			}
			else
			{
				ATLTRACE( "can't lock running object\n" );
				goto bail_out;
			}
		}

		

#ifdef PASS_DISPATCH_INTERFACE_TO_DOC_OBJECT
		ATLTRACE( "making doc object IDispatch\n" );
		hr = m_pObj->QueryInterface( IID_IXCChatDualDispatch, (void **)&m_pObjDispatch );

		if( m_pObjDispatch == NULL )
		{
			ATLTRACE( "can't make doc object IDispatch\n" );
		}
		else
		{
			ATLTRACE( "made doc object IDispatch\n" );
		}
#else
		ATLTRACE( "NOT making doc object IDispatch for now\n" );
#endif

	}

bail_out:
	// ignore any errors so ComicChat background bmp will come up regardless
	return( S_OK ); 

}// CXControl::InitNew





STDMETHODIMP CXControl::InitNew( void )
{
	ATLTRACE( "CXControl<IPersistStreamInit>::InitNew(void)\n" );


	if( m_pStorage != NULL )
	{
		// IPersistStorage::InitNew( IStorage *pStg ) already called, we're done
		return( S_OK );
	}

	// IPersistStorage::InitNew( IStorage *pStg ) has not been called, have to make our own IStorage. 
	// Get rid of any old ones first.
	if( m_pPrivateStorage != NULL )
	{
		m_pPrivateStorage->Release();
		m_pPrivateStorage = NULL;
		m_pStorage = NULL;
	}

	// Make a new one.
    if( FAILED( StgCreateDocfile( NULL, 
								  STGM_TRANSACTED | STGM_READWRITE | STGM_SHARE_EXCLUSIVE | 
									STGM_CREATE| STGM_DELETEONRELEASE,
								  0, &m_pPrivateStorage ) ) )
		return( E_OUTOFMEMORY );

	// Set the IPersistStorage::InitNew ptr to this one so that rest of XCChat doesn't 
	// know the difference. m_pStorage isn't released by XCChat but m_pPrivateStorage
	// is at shutdown (and above).
	InitNew( m_pPrivateStorage );
	
	return( S_OK );

}




HRESULT CXControl::OnDraw(ATL_DRAWINFO& di)
{
	ATLTRACE( "CXControl::OnDraw\n" );

	TCHAR   szBoilerPlate[256];
	RECT&   rc = *(RECT*)di.prcBounds;
	BOOL    bOk = FALSE;
	HDC     memhdc = NULL;
	HBITMAP hbmp = NULL;
	BITMAP	bmpinfo;
	POINT   bmppt;
	int     nWidth, nHeight;

	// erase entire background
	Rectangle(di.hdcDraw, rc.left, rc.top, rc.right, rc.bottom);

	// draw pretty background tiki
	memhdc = CreateCompatibleDC( di.hdcDraw );
	if( memhdc != NULL )
	{
		hbmp = LoadBitmap( _Module.GetModuleInstance(), MAKEINTRESOURCE( IDB_TIKI ) );
		if( hbmp != NULL )
		{
			// Setup bitmap
			SelectObject( memhdc, hbmp );
			GetObject( (HGDIOBJ)hbmp, sizeof(bmpinfo), &bmpinfo );

			// find coords for centering bmp
			nWidth = rc.right - rc.left - 2; // allow for border
			bmppt.x = nWidth/2 - bmpinfo.bmWidth/2;
			if( bmppt.x < 1 )
				bmppt.x = 1;

			nHeight = rc.bottom - rc.top - 2; // allow for border
			bmppt.y = nHeight/2 - bmpinfo.bmHeight/2;
			if( bmppt.y < 1 )
				bmppt.y = 1;

			bmppt.x += rc.left;
			bmppt.y += rc.top;

			// have to clip since we are drawing directly onto client window
			nWidth = min( nWidth, bmpinfo.bmWidth );
			nHeight = min( nHeight, bmpinfo.bmHeight );

			// splat
			BitBlt( di.hdcDraw, bmppt.x, bmppt.y, nWidth, nHeight, 
					memhdc, 0,0, SRCCOPY );

			bOk = TRUE;
		}
	}



	if( !bOk )
	{
		// couldn't draw bmp for some reason, use text for backup plan
		LoadString( _Module.GetModuleInstance(), IDS_BOILERPLATE, szBoilerPlate, sizeof szBoilerPlate );
		DrawText(di.hdcDraw, szBoilerPlate, -1, &rc, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
	}

	// clean up
	if( memhdc != NULL )
		DeleteDC( memhdc );

	if( hbmp != NULL )
		DeleteObject( hbmp );


// Experiments in doc object painting. Probably don't need this anymore but I 
// want to keep it around for a while
#if 0
	//IViewObject *pViewObj = ((m_pFR)?m_pFR->GetViewObject():NULL);
	//if( pViewObj != NULL )
	//	OleDraw( pViewObj, DVASPECT_CONTENT, di.hdcDraw, &rc );
	if( m_pFR != NULL )
	{
		//HWND hwnd = m_pFR->GUIDWindow();
		HWND hwnd = m_pFR->Window();
		if( hwnd != NULL )
		{
			::ShowWindow( hwnd, SW_SHOW );
			::InvalidateRect( hwnd, NULL, TRUE );
			::UpdateWindow( hwnd );
		}
	}
#endif

	return S_OK;
}





HWND CXControl::CreateControlWindow( HWND hWndParent, RECT &rcPos )
{
	HWND hWnd = NULL;

	ATLTRACE( "CXControl::CreateControlWindow\n" );
	ATLTRACE( "rect = %d,%d,%d,%d\n", rcPos.left,rcPos.top,rcPos.right,rcPos.bottom );

	if( CreateGUIDSite( hWndParent, (RECT *)&rcPos, _Module.GetModuleInstance() ) )
	{
		hWnd = m_pFR->Window();
		return( m_hWndCD = hWnd );  // make ATL use our window directly by setting m_hWndCD (which is NULL 
									// at this point and will ASSERT later since we didn't 
									// call CComControl::CreateControlWindow)
	}
	else
	{
		ATLTRACE( "can't create control\n" );
		return( CComControl<CXControl>::CreateControlWindow( hWndParent, rcPos ) );
	}

}









HRESULT 
CXControl::ControlQueryInterface( const IID& iid, void** ppv )
{
	ATLTRACE( "CXControl::ControlQueryInterface\n" );

	return( CComControl<CXControl>::ControlQueryInterface( iid, ppv ) );

}










HRESULT 
CXControl::FinalConstruct( void )
{
	ATLTRACE( "CXControl::FinalConstruct\n" );

	return( S_OK );
}











void 
CXControl::FinalRelease( void )
{
	ATLTRACE( "CXControl::FinalRelease\n" );

}









STDMETHODIMP 
CXControl::SetObjectRects( LPCRECT prcPos, LPCRECT prcClip )
{
	ATLTRACE( "CXControl::SetObjectRects\n" );

	HRESULT hr = S_OK;

	ATLTRACE( "prcPos = %d,%d,%d,%d\n", prcPos->left,prcPos->top,prcPos->right,prcPos->bottom );
	ATLTRACE( "prcClip = %d,%d,%d,%d\n", prcClip->left,prcClip->top,prcClip->right,prcClip->bottom );

	hr = IOleInPlaceObjectWindowlessImpl<CXControl>::SetObjectRects( prcPos, prcClip );

	return( hr );
}









STDMETHODIMP 
CXControl::DoVerb( LONG iVerb, LPMSG lpmsg, 
						 IOleClientSite* pActiveSite, LONG lindex, 
						 HWND hwndParent, LPCRECT lprcPosRect )
{
	ATLTRACE( "CXControl::DoVerb(iVerb = %d)\n", iVerb );

	HRESULT hr;

	hr = IOleObjectImpl<CXControl>::DoVerb( iVerb, lpmsg, 
											pActiveSite, lindex, 
											hwndParent, lprcPosRect );


	if( iVerb == OLEIVERB_UIACTIVATE )
	{
		// set doc object's shared menu
		if( m_pFR != NULL )
		{
			ATLTRACE( "setting doc object's shared menu for UIACTIVATE\n" );
			m_pFR->ResetSharedMenu();
		}
	}
	else if( (!m_bUIActive)&&(iVerb == OLEIVERB_INPLACEACTIVATE) )
	{
		// Special case to make this work with IE4. IE4 only sends OLEIVERB_INPLACEACTIVATE
		// to turn on this control. This causes the control to NOT go UIActive. I don't know
		// if this is an IE4 problem or an ATL problem at this point. You cannot override
		// CComControl::DoesVerbUIActivate so ATL won't let you say that OLEIVERB_INPLACEACTIVATE 
		// UIActivates too. So, I'm doing a fiddle here to make it happen.
		ATLTRACE( "forcing UIACTIVATE\n" );
		SetControlFocus( TRUE );

		// set doc object's shared menu
		if( m_pFR != NULL )
		{
			ATLTRACE( "setting doc object's shared menu for INPLACEACTIVATE\n" );
			m_pFR->ResetSharedMenu();
		}

		::InvalidateRect( m_hWndCD, NULL, TRUE );
		::UpdateWindow( m_hWndCD );
	}


	return( hr );

}








STDMETHODIMP 
CXControl::QueryHitPoint( DWORD dwAspect, LPCRECT pRectBounds, POINT ptlLoc, LONG lCloseHit, DWORD* pHitResult )
{
	//ATLTRACE( "CXControl::QueryHitPoint\n" );

	HRESULT hr;

	hr = IViewObjectExImpl<CXControl>::QueryHitPoint( dwAspect, pRectBounds, ptlLoc, lCloseHit, pHitResult );

	if( (!m_bUIActive)&&(*pHitResult == HITRESULT_HIT)&&(GetAsyncKeyState( VK_LBUTTON ) < 0) )
	{
		ATLTRACE( "forcing UIACTIVATE\n" );
		SetControlFocus( TRUE );

		// set doc object's shared menu
		if( m_pFR != NULL )
		{
			ATLTRACE( "setting doc object's shared menu\n" );
			::PostMessage( m_pFR->Window(), FRAMERWND_SETUIACTIVE, 0, 0 );
		}

		::InvalidateRect( m_hWndCD, NULL, TRUE );
		::UpdateWindow( m_hWndCD );
	}


	return( hr );

}










STDMETHODIMP 
CXControl::Load( LPPROPERTYBAG pPropBag, LPERRORLOG pErrorLog )
{
	ATLTRACE( "CXControl::Load\n" );

	HRESULT hr;

	hr = IPersistPropertyBagImpl<CXControl>::Load( pPropBag, pErrorLog );

	// If IPersistPropertyBag::Load is called, IPersistStorage::InitNew() won't 
	// be called, so we have to call it ourselves.
	if( m_pFR == NULL )
	{
		ATLTRACE( "Calling InitNew()\n" );
		InitNew();
	}

	return( hr );

}










STDMETHODIMP 
CXControl::GetIDsOfNames( REFIID riid, LPOLESTR* rgszNames, UINT cNames, LCID lcid, DISPID* rgdispid )
{
	ATLTRACE( "CXControl::GetIDsOfNames\n" );

	HRESULT hr;

	// relay IDispatch stuff to doc object if we have one, otherwise eat it ourselves
	if( m_pObjDispatch != NULL )
		hr = ((IDispatch *)m_pObjDispatch)->
				GetIDsOfNames( riid, rgszNames, cNames, lcid, rgdispid );
	else
		hr = IDispatchImpl<IXControl, &IID_IXControl, &LIBID_XCCHATLib>::
				GetIDsOfNames( riid, rgszNames, cNames, lcid, rgdispid );

	return( hr );
}




STDMETHODIMP 
CXControl::GetTypeInfo( UINT itinfo, LCID lcid, ITypeInfo** pptinfo )
{
	ATLTRACE( "CXControl::GetTypeInfo\n" );

	HRESULT hr;

	// relay IDispatch stuff to doc object if we have one, otherwise eat it ourselves
	if( m_pObjDispatch != NULL )
		hr = ((IDispatch *)m_pObjDispatch)->
				GetTypeInfo( itinfo, lcid, pptinfo );
	else
		hr = IDispatchImpl<IXControl, &IID_IXControl, &LIBID_XCCHATLib>::
				GetTypeInfo( itinfo, lcid, pptinfo );

	return( hr );
}




STDMETHODIMP 
CXControl::GetTypeInfoCount( UINT* pctinfo )
{
	ATLTRACE( "CXControl::GetTypeInfoCount\n" );

	HRESULT hr;

	// relay IDispatch stuff to doc object if we have one, otherwise eat it ourselves
	if( m_pObjDispatch != NULL )
		hr = ((IDispatch *)m_pObjDispatch)->
				GetTypeInfoCount( pctinfo );
	else
		hr = IDispatchImpl<IXControl, &IID_IXControl, &LIBID_XCCHATLib>::
				GetTypeInfoCount( pctinfo );

	return( hr );

}




STDMETHODIMP 
CXControl::Invoke( DISPID dispidMember, REFIID riid, LCID lcid, 
					   WORD wFlags, DISPPARAMS* pdispparams, VARIANT* pvarResult, 
					   EXCEPINFO* pexcepinfo, UINT* puArgErr )
{
	ATLTRACE( "CXControl::Invoke\n" );

	HRESULT hr;

	// if not an XCChat disp id, send it to doc object....
	if( (m_pObjDispatch != NULL)&&(dispidMember > LAST_XCCHAT_DSPID) )
		hr = ((IDispatch *)m_pObjDispatch)->
				Invoke( dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr );
	else
	{
		// ...otherwise, redirect it to us so html PARAM property tag will work with
		// IPersistPropertyBag interface independantly of doc object (IDispatch shell game :).
		hr = IDispatchImpl<IXControl, &IID_IXControl, &LIBID_XCCHATLib>::
				Invoke( dispidMember, riid, lcid, wFlags, pdispparams, pvarResult, pexcepinfo, puArgErr );
	}
	
	return( hr );

}










BOOL 
	CXControl::CreateGUIDSite( HWND hMainWnd, RECT *prcPos, HINSTANCE hInstance )
{
	ATLTRACE( "CXControl::CreateGUIDSite\n" );


	if( !m_bOkToCreateDocObj )
		return( FALSE );


	// Relaunch CChat if we launched it with /cb32 before. IE4 may not destroy this control 
	// when surfing to another page depending on caching. BUT, CChat.exe is unloaded. So, when 
	// the page is revisited Ole will relaunch CChat without the /cb32 command line param if 
	// we were talking to NM and the conference is broken. This may result in extra spurrious 
	// launches of CChat but Cbcore will cause them to shut down immediately. 
	if( m_szCChatCmd[0] != '\0' )
	{
		ATLTRACE( "Relaunching CChat Server\n" );
		WinExec( m_szCChatCmd, SW_SHOWDEFAULT );
	}

    if( m_pFR != NULL )
	{
		// we only need to do this once
        return( TRUE );
	}

    // Attempt to allocate and initialize the application
    m_pFR = new CFrame( hInstance, NULL, NULL );

    if( m_pFR == NULL )
        return( FALSE );
    
    // Initialize pFR
	m_pFR->ShowMenu( m_bShowMenu );
	m_pFR->ShowToolbar( m_bShowToolbar );
    if( m_pFR->Init( m_pObj, 
					 hMainWnd,
					 m_pStorage,
					 prcPos->left, prcPos->top, 
					 prcPos->right - prcPos->left, prcPos->bottom - prcPos->top,
					 m_spInPlaceSite ) )
	{
		// everythings cooking, see if we need to open NM's tray icon
		if( m_bDelayedOpenNM )
		{
			// open up NM later after it has a chance to create its windows
			m_pFR->DelayedOpenNM();

			// try it only once
			m_bDelayedOpenNM = FALSE;
		}

		return( TRUE );
	}

	// somethings broken
	DeleteGUIDSite();
	return( FALSE );
}






void 
CXControl::DeleteGUIDSite( void )
{
	ATLTRACE( "CXControl::DeleteGUIDSite\n" );

	ReleaseInterface( m_pObjDispatch );

	try
	{
		if( m_pFR != NULL )
		{
			// Don't actually delete the frame object yet - it is needed
			// for cleanup.
			m_pFR->PreDelete ();
		}

		if( m_pClassFactory != NULL )
		{
			// Unlock server before releasing pointer
			//m_pClassFactory->LockServer( FALSE );

			// safe to let go now
			m_pClassFactory->Release();
			m_pClassFactory = NULL;
		}

		ReleaseInterface( m_pObj );

		if( m_pPrivateStorage != NULL )
		{
			m_pPrivateStorage->Release();
			m_pPrivateStorage = NULL;
			m_pStorage = NULL;
		}
	}
	catch( ... )
	{
		// This might happen when closing IE4 while XCChat/CChat
		// is visible on a web page. Since everybodies shutting down, 
		// we don't care...	The fix in CSite::WaitForServerShutdown() probably
		// prevents this from happening now.
		ATLTRACE( "*** SHUTDOWN ERROR *** ignoring it\n" );
	}
}









BOOL
CXControl::LaunchForNM( void )
{
	ATLTRACE( "CXControl::LaunchForNM\n" );

	// conf is running, see if cbcore is about
	HKEY hkeyCbcore = NULL;
	HKEY hkeyCChatCmd = NULL;
	DWORD dwType;
	DWORD dwBuf;
	UINT  uErr;
	TCHAR title[256];
	TCHAR fmt[256];
	TCHAR msg[256];
	BOOL  bOk = FALSE;


	::LoadString( _Module.GetModuleInstance(), IDS_MSGTITLE, title, sizeof( title ) );

	ATLTRACE( "Conf is running, checking for Cbcore\n" );
	RegOpenKey( HKEY_CLASSES_ROOT, 
				_T("CLSID\\{C7047720-CABE-11d0-A041-444553540000}"), 
				&hkeyCbcore );

	if( hkeyCbcore != NULL )
	{
		// cbcore is available, get CChat's ActiveX server command line
		ATLTRACE( "Cbcore is installed\n" );
		RegOpenKey( HKEY_LOCAL_MACHINE, 
					_T("SOFTWARE\\Classes\\CLSID\\{241AF500-8FB6-11CF-ADC5-00AA00BADF6F}\\LocalServer32"), 
					&hkeyCChatCmd );

		if( hkeyCChatCmd != NULL )
		{
			dwBuf = sizeof( m_szCChatCmd );
			if( SUCCEEDED( RegQueryValueEx( hkeyCChatCmd, NULL, NULL, &dwType, (LPBYTE)m_szCChatCmd, &dwBuf ) ) &&
				(dwType == REG_SZ) )
			{
				// Got CChat's command line, add /cb32 and launch it as a server
				lstrcat( m_szCChatCmd, _T(" -Embedding /cb32") ); 
				if( (uErr = WinExec( m_szCChatCmd, SW_SHOWDEFAULT )) > 31 )
				{
					ATLTRACE( "Launching CChat as a NetMeeting ActiveX Chat Server\n" );

					if( m_bShowNM )
						m_bDelayedOpenNM = TRUE;

					bOk = TRUE;
				}
				else
				{
					ATLTRACE( "FAILED TO LAUNCH CChat as a NetMeeting ActiveX Chat Server\n" );
					::LoadString( _Module.GetModuleInstance(), IDS_CANTJOINNM, fmt, sizeof( fmt ) );
					wsprintf( msg, fmt, uErr );
					::MessageBox( NULL, msg, title, MB_OK );

					m_szCChatCmd[0] = '\0'; // zap so CreateGUIDSite() doesn't try to relaunch
				}
			}
			else
			{
				ATLTRACE( "CChat isn't installed properly\n" );
				::LoadString( _Module.GetModuleInstance(), IDS_CCHATBROKEN, msg, sizeof( msg ) );
				::MessageBox( NULL, msg, title, MB_OK );
			}

			RegCloseKey( hkeyCChatCmd );
		}
		else
		{
			ATLTRACE( "Can't find CChat\n" );
			::LoadString( _Module.GetModuleInstance(), IDS_CANTFINDCCHAT, msg, sizeof( msg ) );
			::MessageBox( NULL, msg, title, MB_OK );
		}

		RegCloseKey( hkeyCbcore );
	}
	else
	{
		ATLTRACE( "Can't find Cbcore\n" );
		::LoadString( _Module.GetModuleInstance(), IDS_CANTFINDCBCORE, msg, sizeof( msg ) );
		::MessageBox( NULL, msg, title, MB_OK );
	}

	return( bOk );

}









STDMETHODIMP CXControl::get_ShowToolBar(BOOL * pVal)
{
	ATLTRACE( "CXControl::get_ShowToolBar\n" );

	*pVal = m_bShowToolbar;

	return S_OK;
}

STDMETHODIMP CXControl::put_ShowToolBar(BOOL newVal)
{
	ATLTRACE( "CXControl::put_ShowToolBar\n" );

	m_bShowToolbar = newVal;

	if( m_pFR != NULL )
		m_pFR->ShowToolbar( m_bShowToolbar );

	return S_OK;
}

STDMETHODIMP CXControl::get_JoinNetMeeting(BOOL * pVal)
{
	ATLTRACE( "CXControl::get_JoinNetMeeting\n" );

	*pVal = m_bJoinNM;

	return S_OK;
}

STDMETHODIMP CXControl::put_JoinNetMeeting(BOOL newVal)
{
	ATLTRACE( "CXControl::put_JoinNetMeeting\n" );

	m_bJoinNM = newVal;

	return S_OK;
}

STDMETHODIMP CXControl::get_ShowMenu(BOOL * pVal)
{
	ATLTRACE( "CXControl::get_ShowMenu\n" );

	*pVal = m_bShowMenu;

	return S_OK;
}

STDMETHODIMP CXControl::put_ShowMenu(BOOL newVal)
{
	ATLTRACE( "CXControl::put_ShowMenu\n" );

	m_bShowMenu = newVal;

	if( m_pFR != NULL )
		m_pFR->ShowMenu( m_bShowMenu );

	return S_OK;
}

STDMETHODIMP CXControl::get_ShowNetMeeting(BOOL * pVal)
{
	ATLTRACE( "CXControl::get_ShowNetMeeting\n" );

	*pVal = m_bShowNM;

	return S_OK;
}

STDMETHODIMP CXControl::put_ShowNetMeeting(BOOL newVal)
{
	ATLTRACE( "CXControl::put_ShowNetMeeting\n" );

	m_bShowNM = newVal;

	return S_OK;
}





