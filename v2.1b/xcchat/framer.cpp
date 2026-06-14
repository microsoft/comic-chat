/*
 * FRAMER.CPP
 * Document Objects Framer
 *
 * Sample to demonstrate in-place activation of a DocObject--also
 * activates normal embeddings in separate windows.
 *
 * Copyright (c)1995-1996 Microsoft Corporation, All Rights Reserved
 */
//MODIFIED FOR XCCHAT by Numbers & Co.

#include "stdafx.h"
#include "confcli.h"
#include "resource.h"

#define INITGUID
#include "framer.h"




#define TIMER_DELAYEDOPEN_NM		1
#define TIMER_DELAYEDOPEN_NM_DELAY	100		// .1 sec



// NetMeeting's cb handles (breaker breaker good buddy...)
#define CONF_CLASS		_T("MPWClass")			// main UI window
#define CONF_TRAYCLASS  _T("ConfHiddenWindow")	// tray icon window






CFrame::CFrame(HINSTANCE hInst, HINSTANCE hInstPrev, LPSTR pszCmdLine )    
    {
	ATLTRACE( "CFrame::CFrame\n" );

    m_cRef = 0;

    m_hInst=hInst;
    m_hWnd=NULL;
    m_hInstPrev=hInstPrev;

    m_hWndClient=NULL;

    m_pIStorage=NULL;
    m_dwIDCounter=0;

	m_hWndObj=NULL;

    m_pSite=NULL;
    SetRect(&m_bwIP, 0, 0, 0, 0);
    m_fInContextHelp=FALSE;
    m_pIOleIPActiveObject=NULL;

	// ActiveX container interfaces
	m_spAdviseSink = NULL;
	m_spClientSite = NULL;
	m_spDataAdviseHolder = NULL;
	m_spInPlaceSite = NULL;
	m_spOleAdviseHolder = NULL;

	m_LasthMenu = NULL;
	m_LasthOLEMenu = NULL;
	m_LasthWndObj = NULL;

	m_bShowMenu = TRUE;
	m_bShowToolbar = TRUE;

	m_bDelayedOpenNM = FALSE;
    }



CFrame::~CFrame(void)
    {
	ATLTRACE( "CFrame::~CFrame\n" );

	if( m_pSite != NULL )
		Close();				
	
	// ActiveX container interfaces
	ReleaseInterface( m_spAdviseSink );
	ReleaseInterface( m_spClientSite );
	ReleaseInterface( m_spDataAdviseHolder );
	ReleaseInterface( m_spInPlaceSite );
	ReleaseInterface( m_spOleAdviseHolder );


    //Frees the temp file.
    ReleaseInterface(m_pIStorage);
    
    }




BOOL CFrame::Init( LPUNKNOWN pObj,
				   HWND hParentWnd, 
				   IStorage *pStg,
				   int x, int y, int cx, int cy,
				   IAdviseSink *spAdviseSink,
				   IOleClientSite *spClientSite,
				   IDataAdviseHolder *spDataAdviseHolder,
				   IOleInPlaceSiteWindowless *spInPlaceSite,
				   IOleAdviseHolder *spOleAdviseHolder )
    {
	ATLTRACE( "CFrame::Init\n" );

    RECT                rc;


	if( pStg == NULL )
		{
		ATLTRACE( "pStg == NULL\n" );
		return FALSE;
		}


    if (NULL==m_hInstPrev)
        {
        if (!RegisterAllClasses())
            return FALSE;
        }


    m_hWnd = CreateWindow( SZCLASSFRAME, 
						   TEXT(""), 
						   WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS, 
						   x,y, 
						   cx,cy, 
						   hParentWnd, 
						   NULL, 
						   m_hInst, 
						   this );
        
    if (NULL==m_hWnd)
		{
		ATLTRACE( "can't create m_hWnd\n" );
        return FALSE;
		}

    GetClientRect(m_hWnd, &rc);

    m_hWndClient = CreateWindow( SZCLASSCLIENT, 
								 SZCLASSCLIENT, 
								 WS_CHILD | WS_CLIPCHILDREN | WS_VISIBLE | WS_CLIPSIBLINGS,
								 rc.left,rc.top, 
								 rc.right-rc.left,rc.bottom-rc.top, 
								 m_hWnd, 
								 NULL, 
								 m_hInst, 
								 this );


    if (NULL==m_hWndClient)
		{
		ATLTRACE( "can't create m_hWndClient\n" );
        return FALSE;
		}

	// get and hang on to ActiveX container interfaces
	m_spAdviseSink		 = spAdviseSink;
	if( m_spAdviseSink != NULL )
		m_spAdviseSink->AddRef();

	m_spClientSite		 = spClientSite;      
	if( m_spClientSite != NULL )
		m_spClientSite->AddRef();

	m_spDataAdviseHolder = spDataAdviseHolder;
	if( m_spDataAdviseHolder != NULL )
		m_spDataAdviseHolder->AddRef();

	m_spInPlaceSite		 = spInPlaceSite;
	if( m_spInPlaceSite != NULL )
		m_spInPlaceSite->AddRef();

	m_spOleAdviseHolder	 = spOleAdviseHolder;
	if( m_spOleAdviseHolder != NULL )
		m_spOleAdviseHolder->AddRef();


	m_pIStorage = pStg;	  // use container's storage
	m_pIStorage->AddRef(); // hang on to it


    PCHourglass     pHour;
    pHour=new CHourglass;            


    CreateObject( pObj, m_pIStorage );


    delete pHour;

    return TRUE;
    }




/*
 * CFrame::RegisterAllClasses
 *
 * Purpose:
 *  Registers all classes used in this application.
 *
 * Return Value:
 *  BOOL            TRUE if registration succeeded, FALSE otherwise.
 */

BOOL CFrame::RegisterAllClasses(void)
    {
	ATLTRACE( "CFrame::RegisterAllClasses\n" );

    WNDCLASS        wc;

    //Field that are the same for all windows.
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.hInstance     = m_hInst;
    wc.cbClsExtra    = 0;

    //Register the Frame window
    wc.lpfnWndProc   = FrameWndProc;
    wc.cbWndExtra    = CBFRAMEWNDEXTRA;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = SZCLASSFRAME;

    if (!RegisterClass(&wc))
		{
		ATLTRACE( "1st RegisterClass failed with %d\n", GetLastError() );
		}


    //Register the do-nothing Client window
    wc.lpfnWndProc   = ClientWndProc;
    wc.cbWndExtra    = CBCLIENTWNDEXTRA;
    wc.hIcon         = NULL;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = NULL;
    wc.lpszMenuName  = NULL;
    wc.lpszClassName = SZCLASSCLIENT;

    if (!RegisterClass(&wc))
		{
		ATLTRACE( "2nd RegisterClass failed with %d\n", GetLastError() );
		}


    return TRUE;
    }




/*
 * CFrame::OnCommand
 *
 * Purpose:
 *  WM_COMMAND handler for the frame window so derivations can
 *  process their messages and then pass the standard commands (like
 *  file open and save) on to the base class.
 *
 * Parameters:
 *  hWnd            HWND of the frame window.
 *  wParam          WPARAM of the message.
 *  lParam          LPARAM of the message.
 *
 * Return Value:
 *  LRESULT         Return value for the message.
 */

LRESULT CFrame::OnCommand(HWND hWnd, WPARAM wParam, LPARAM lParam)
    {
	ATLTRACE( "CFrame::OnCommand\n" );

    return 0L;
    }





/*
 * CFrame::CreateObject
 *
 * Purpose:
 *  Creates a site and has it create an object based on a filename.
 *
 * Parameters:
 *  pszFile         LPTSTR pointing to the filename from which to
 *                  create the object.
 *
 * Return Value:
 *  BOOL            TRUE if successful, FALSE otherwise.
 */

BOOL CFrame::CreateObject( LPUNKNOWN pObj, LPSTORAGE pStg )
    {    
	ATLTRACE( "CFrame::CreateObject\n" );

    m_pSite=new CSite(++m_dwIDCounter, m_hWndClient, this);

    if (NULL==m_pSite)
        return FALSE;

    m_pSite->AddRef();  //So we can free with Release

    /*
     * Now tell the site to create an object in it using the filename
     * and the storage we opened.  The site will create a sub-storage
     * for the doc object's use.
     */
    if (!m_pSite->Create( pObj, pStg) )
        return FALSE;

    //We created the thing, now activate it with "Show"
    m_pSite->Activate(OLEIVERB_SHOW);

	//Force repaint to show "have object" message
	InvalidateRect(m_hWndClient, NULL, TRUE);
	UpdateWindow(m_hWndClient);
    return TRUE;        
    }






/*
 * CFrame::Close
 *
 * Purpose:
 *  Handles File/Close by freeing the object and resetting the
 *  application state.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CFrame::Close(void)
    {    
	ATLTRACE( "CFrame::Close\n" );

	RECT	rc;

    if (NULL!=m_pSite)
        {
		CSite *pSite=m_pSite;        //Prevents reentry
		m_pSite=NULL;

        pSite->Close(FALSE);         //Frees the object
        pSite->Destroy(m_pIStorage); //Cleans up the storage
        pSite->Release();            //Frees the site        
        }
    
	ReleaseInterface( m_pIOleIPActiveObject );

    SetRect(&m_bwIP, 0, 0, 0, 0);	

	GetClientRect(m_hWnd, &rc);	
	ResizeClientWindow( rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top );	

	//Force repaint to remove "have object" message
	InvalidateRect(m_hWndClient, NULL, TRUE);
	UpdateWindow(m_hWndClient);
    
    return;        
    }


/*
 * CFrame::ResizeClientWindow
 *
 * Purpose:
 *	Resizes the client-area window according to current toolbar sizes
 *  and the frame window size.
 *
 * Parameters:
 *	x,y,cx,cy		UINT origin and dimensions of the window
 *
 * Return Value:
 *	None
 */

void CFrame::ResizeClientWindow(UINT x, UINT y, UINT cx, UINT cy)
	{
	ATLTRACE( "CFrame::ResizeClientWindow\n" );

	SetWindowPos(m_hWndClient, NULL, x,y, cx,cy, SWP_NOZORDER | SWP_NOACTIVATE);

	ATLTRACE( "site rect = %d,%d,width=%d,height=%d\n", x,y, cx,cy);

    //Tell the site to tell the object.
	if (NULL!=m_pSite)
		{
		ATLTRACE( "resizing site object\n" );
    	m_pSite->UpdateObjectRects();
		}

	return;
	}










void CFrame::ResetSharedMenu( void )
	{
	ATLTRACE( "CFrame::ResetSharedMenu\n" );

	RECT rcPos;
	RECT rcClip;
	IOleInPlaceFrame *pIIPFrame;
	IOleInPlaceUIWindow *pIIPUIWindow;
	OLEINPLACEFRAMEINFO Fi;
	HWND hwndIpf;
	
	m_pSite->Activate( OLEIVERB_UIACTIVATE );

	}
#if 0
void CFrame::ResetSharedMenu( void )
	{
	ATLTRACE( "CFrame::ResetSharedMenu\n" );

	RECT rcPos;
	RECT rcClip;
	IOleInPlaceFrame *pIIPFrame;
	IOleInPlaceUIWindow *pIIPUIWindow;
	OLEINPLACEFRAMEINFO Fi;
	HWND hwndIpf;

//#if 0
	m_spInPlaceSite->OnUIActivate();

	if( !m_bShowMenu )
		{
		ATLTRACE( "Menus are disabled, ignoring reset" );
		}

	if( m_LasthMenu != NULL )
		{
		ATLTRACE( "reseting menu bar to previous XCChat shared menu\n" );

		m_spInPlaceSite->GetWindowContext( &pIIPFrame, &pIIPUIWindow, &rcPos, &rcClip, &Fi );
		pIIPFrame->SetMenu( m_LasthMenu, m_LasthOLEMenu, GUIDWindow() );
		//pIIPFrame->GetWindow( &hwndIpf );
		//OleSetMenuDescriptor( m_LasthOLEMenu, hwndIpf, GUIDWindow(), 
		//					  pIIPFrame, GetIPActiveObject());
		}
//#endif

	}
#endif








void CFrame::RemoveSharedMenu( void )
	{
	ATLTRACE( "CFrame::RemoveSharedMenu\n" );

	RECT rcPos;
	RECT rcClip;
	IOleInPlaceFrame *pIIPFrame;
	IOleInPlaceUIWindow *pIIPUIWindow;
	OLEINPLACEFRAMEINFO Fi;

	if( !m_bShowMenu )
		{
		ATLTRACE( "Menus are disabled, ignoring remove" );
		return;
		}

    m_pSite->GetInPlaceObject()->UIDeactivate();

	}
#if 0
void CFrame::RemoveSharedMenu( void )
	{
	ATLTRACE( "CFrame::RemoveSharedMenu\n" );

	RECT rcPos;
	RECT rcClip;
	IOleInPlaceFrame *pIIPFrame;
	IOleInPlaceUIWindow *pIIPUIWindow;
	OLEINPLACEFRAMEINFO Fi;

	if( !m_bShowMenu )
		{
		ATLTRACE( "Menus are disabled, ignoring remove" );
		return;
		}

	if( m_LasthMenu != NULL )
		{
		ATLTRACE( "reseting menu bar to previous XCChat shared menu\n" );

		m_spInPlaceSite->GetWindowContext( &pIIPFrame, &pIIPUIWindow, &rcPos, &rcClip, &Fi );
		pIIPFrame->RemoveMenus( m_LasthMenu );
		}
	}
#endif









IViewObject *
	CFrame::GetViewObject( void )
	{
	ATLTRACE( "CFrame::GetViewObject\n" );
	return( (m_pSite)?m_pSite->GetViewObject():NULL );
	}





IOleDocumentView  *
	CFrame::GetDocView( void )
	{
	ATLTRACE( "CFrame::GetDocView\n" );
	return( (m_pSite)?m_pSite->GetDocView():NULL );
	}





IOleInPlaceObject *
	CFrame::GetInPlaceObject( void )
	{
	ATLTRACE( "CFrame::GetInPlaceObject\n" );
	return( (m_pSite)?m_pSite->GetInPlaceObject():NULL );
	}



void
	CFrame::DelayedOpenNM( void )
	{
	ATLTRACE( "CFrame::DelayedOpenNM\n" );

	// set timer to try to open NM later
	m_bDelayedOpenNM = TRUE;
	SetTimer( m_hWnd, TIMER_DELAYEDOPEN_NM, TIMER_DELAYEDOPEN_NM_DELAY, NULL );

	}


/*
 * FrameWndProc
 *
 * Purpose:
 *  Frame window class procedure that allows a derivation of these
 *  classes to hook and process any messages desired.  Otherwise this
 *  handles standard commands as well as the status line and menus.
 */

LRESULT APIENTRY FrameWndProc( HWND hWnd, 
							   UINT iMsg, 
							   WPARAM wParam, 
							   LPARAM lParam )
    {
    PCFrame         pFR;
    RECT            rc;

    pFR=(PCFrame)GetWindowLong(hWnd, FRAMEWL_STRUCTURE);

    switch (iMsg)
        {
        case WM_NCCREATE:
			ATLTRACE( "FrameWndProc - WM_NCCREATE\n" );
            pFR=(PCFrame)((LPCREATESTRUCT)lParam)->lpCreateParams;
            SetWindowLong(hWnd, FRAMEWL_STRUCTURE, (LONG)pFR);
            return DefWindowProc(hWnd, iMsg, wParam, lParam);

        case WM_DESTROY:
			ATLTRACE( "FrameWndProc - WM_DESTROY\n" );
            break;

        case WM_CLOSE:                        
			ATLTRACE( "FrameWndProc - WM_CLOSE\n" );
            break;

        case WM_ERASEBKGND:
			ATLTRACE( "FrameWndProc - WM_ERASEBKGND\n" );
            //Client area window always manages painting
            return FALSE;

        case WM_SIZE:
			ATLTRACE( "FrameWndProc - WM_SIZE\n" );
            //Tell the in-place object about the new frame size
            GetClientRect(hWnd, &rc);

            if (NULL!=pFR->m_pIOleIPActiveObject)
                pFR->m_pIOleIPActiveObject->ResizeBorder(&rc, pFR, TRUE);
					
            /*
             * Resize the client, which is done in all cases since this window
			 * is the parent of the DocObject, plus we need to tell the 
			 * DocObject of the new size through IOleDocumentView::SetRect.
			 */
            rc.left  +=pFR->m_bwIP.left;
            rc.right -=pFR->m_bwIP.right;
            rc.top   +=pFR->m_bwIP.top;
            rc.bottom-=pFR->m_bwIP.bottom;            

			pFR->ResizeClientWindow( rc.left, rc.top, 
									 rc.right-rc.left, rc.bottom-rc.top );
			
            break;

        case WM_SETFOCUS:
			ATLTRACE( "FrameWndProc - WM_SETFOCUS\n" );
            if (NULL!=pFR->m_pIOleIPActiveObject)
                {
                HWND    hWndObj;

                pFR->m_pIOleIPActiveObject->GetWindow(&hWndObj);
				SetFocus(hWndObj);

				// set shared menu back up
				//if( pFR->m_LasthMenu != NULL )
				//	pFR->SetMenu( pFR->m_LasthMenu, pFR->m_LasthOLEMenu, pFR->m_LasthWndObj );
                }

            return TRUE;


		case WM_TIMER:		
			ATLTRACE( "FrameWndProc - WM_TIMER\n" );
			if( pFR->m_bDelayedOpenNM )
				{
				HWND hwndNM;

				// check if Conf is running
				if( (hwndNM = ::FindWindow( CONF_CLASS, NULL )) != NULL )
					{
					// open up conf's UI
					ATLTRACE( "Opening conf's UI\n" );
					::PostMessage( hwndNM, WM_SYSCOMMAND, SC_RESTORE, 0 );
					pFR->m_bDelayedOpenNM = FALSE; // only do this once
					}
				else
				if( (hwndNM = ::FindWindow( CONF_TRAYCLASS, NULL )) != NULL)
					{
					// open up conf's UI by poking the tray icon
					ATLTRACE( "Opening conf's UI from the tray icon\n" );
					::PostMessage( hwndNM, CM_OPEN_CONFROOM, 0, 0 );
					pFR->m_bDelayedOpenNM = FALSE; // only do this once
					}

				// do we need to keep trying?
				if( !pFR->m_bDelayedOpenNM )
					::KillTimer( hWnd, TIMER_DELAYEDOPEN_NM );
				//else - keep trying
				}
			else
				::KillTimer( hWnd, TIMER_DELAYEDOPEN_NM );

			break;


		case WM_COMMAND:
			pFR->OnCommand( hWnd, wParam, lParam);
			break;
			

        default:
            return DefWindowProc(hWnd, iMsg, wParam, lParam);
        }

    return 0L;
    }



/*
 * ClientWndProc
 *
 * Purpose:
 *  Client window class procedure that's only used to paint a
 *  message when we have a non-DocObject open.  Otherwise this
 *  is completely hidden.
 */

LRESULT APIENTRY ClientWndProc( HWND hWnd, 
								UINT iMsg, 
								WPARAM wParam, 
								LPARAM lParam )
    {
    PCFrame         pFR;

    pFR=(PCFrame)GetWindowLong(hWnd, CLIENTWL_STRUCTURE);

    switch (iMsg)
        {
        case WM_NCCREATE:
			ATLTRACE( "ClientWndProc - WM_NCCREATE\n" );

            pFR=(PCFrame)((LPCREATESTRUCT)lParam)->lpCreateParams;

            SetWindowLong(hWnd, CLIENTWL_STRUCTURE, (LONG)pFR);
            return DefWindowProc(hWnd, iMsg, wParam, lParam);

        case WM_CLOSE:                        
			ATLTRACE( "ClientWndProc - WM_CLOSE\n" );
            break;

        case WM_ERASEBKGND:
			ATLTRACE( "ClientWndProc - WM_ERASEBKGND\n" );
            return TRUE;

        case WM_MOVE:
        case WM_SIZE:
            break;

        default:
            return DefWindowProc(hWnd, iMsg, wParam, lParam);
        }

    return 0L;
    }











/*
 * IUnknown implementation
 */


/*
 * CFrame::QueryInterface
 * CFrame::AddRef
 * CFrame::Release
 */

STDMETHODIMP CFrame::QueryInterface(REFIID riid, void **ppv)
    {
	ATLTRACE( "CFrame::QueryInterface\n" );

    /*
     * We only know IOleInPlaceFrame and its base interfaces as well
     * as a bogus IOleCommandTarget to make PowerPoint happy.
	 */
    *ppv=NULL;

    if (IID_IUnknown==riid || IID_IOleInPlaceUIWindow==riid
        || IID_IOleWindow==riid || IID_IOleInPlaceFrame==riid)
		{
		ATLTRACE( "making IID_IUnknown or IID_IOleInPlaceUIWindow or IID_IOleWindow or IID_IOleInPlaceFrame\n" );
        *ppv=(IOleInPlaceFrame *)this;
		}

	if (IID_IOleCommandTarget==riid)
		{
		ATLTRACE( "making IID_IOleCommandTarget\n" );
        *ppv=(IOleCommandTarget *)this;
		}

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

	ATLTRACE( "E_NOINTERFACE\n" );
    return E_NOINTERFACE;
    }


STDMETHODIMP_(ULONG) CFrame::AddRef(void)
    {
    ++m_cRef;
	ATLTRACE( "CFrame::AddRef(%d)\n", m_cRef );
    return m_cRef;
    }

STDMETHODIMP_(ULONG) CFrame::Release(void)
    {
    //Nothing special happening here--frame's life if user-controlled.
    --m_cRef;
	ATLTRACE( "CFrame::Release(%d)\n", m_cRef );
    return m_cRef;
    }


/*
 * IOleInPlaceFrame implementation
 */


/*
 * CFrame::GetWindow
 *
 * Purpose:
 *  Retrieves the handle of the window associated with the object
 *  on which this interface is implemented.
 *
 * Parameters:
 *  phWnd           HWND * in which to store the window handle.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, E_FAIL if there is no
 *                  window.
 */

STDMETHODIMP CFrame::GetWindow(HWND *phWnd)
    {
	ATLTRACE( "CFrame::GetWindow\n" );

    *phWnd=m_hWnd;
    return NOERROR;
    }




/*
 * CFrame::ContextSensitiveHelp
 *
 * Purpose:
 *  Instructs the object on which this interface is implemented to
 *  enter or leave a context-sensitive help mode.
 *
 * Parameters:
 *  fEnterMode      BOOL TRUE to enter the mode, FALSE otherwise.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CFrame::ContextSensitiveHelp(BOOL fEnterMode)
    {
	ATLTRACE( "CFrame::ContextSensitiveHelp\n" );

    /*
     * Don't bother if there is no active object since we don't do
     * context help on our own.
     */
    if (NULL==m_pIOleIPActiveObject)
        return NOERROR;

    //If the state changes, notify the active object.
    if (m_fInContextHelp!=fEnterMode)
        {
        m_fInContextHelp=fEnterMode;
        m_pIOleIPActiveObject->ContextSensitiveHelp(fEnterMode);
        }

    return NOERROR;
    }




/*
 * CFrame::GetBorder
 *
 * Purpose:
 *  Returns the rectangle in which the container is willing to
 *  negotiate about an object's adornments.
 *
 * Parameters:
 *  prcBorder       LPRECT in which to store the rectangle.
 *
 * Return Value:
 *  HRESULT         NOERROR if all is well, INPLACE_E_NOTOOLSPACE
 *                  if there is no negotiable space.
 */

STDMETHODIMP CFrame::GetBorder(LPRECT prcBorder)
    {
	ATLTRACE( "CFrame::GetBorder\n" );

    if (NULL==prcBorder)
        return E_INVALIDARG;

    //We return all the client area space
    GetClientRect(m_hWnd, prcBorder);
    return NOERROR;
    }




/*
 * CFrame::RequestBorderSpace
 *
 * Purpose:
 *  Asks the container if it can surrender the amount of space
 *  in pBW that the object would like for it's adornments.  The
 *  container does nothing but validate the spaces on this call.
 *
 * Parameters:
 *  pBW             LPCBORDERWIDTHS containing the requested space.
 *                  The values are the amount of space requested
 *                  from each side of the relevant window.
 *
 * Return Value:
 *  HRESULT         NOERROR if we can give up space,
 *                  INPLACE_E_NOTOOLSPACE otherwise.
 */

STDMETHODIMP CFrame::RequestBorderSpace(LPCBORDERWIDTHS pBW)
    {
	ATLTRACE( "CFrame::RequestBorderSpace\n" );

	if( m_bShowToolbar )
		{
		return( NOERROR );
		}
	else
		{
		ATLTRACE( "Toolbars are disabled\n" );
		return( INPLACE_E_NOTOOLSPACE );
		}
    }




/*
 * CFrame::SetBorderSpace
 *
 * Purpose:
 *  Called when the object now officially requests that the
 *  container surrender border space it previously allowed
 *  in RequestBorderSpace.  The container should resize windows
 *  appropriately to surrender this space.
 *
 * Parameters:
 *  pBW             LPCBORDERWIDTHS containing the amount of space
 *                  from each side of the relevant window that the
 *                  object is now reserving.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CFrame::SetBorderSpace(LPCBORDERWIDTHS pBW)
    {
	ATLTRACE( "CFrame::SetBorderSpace\n" );

    RECT            rc;

    /*
     * Since we have no tools, we can accept anything the object sends
     * and must therefore adjust the client-area window accordingly.
     */

    /*
     * If pBW is NULL, the object is not interested in tools, so we
     * don't have to do anything.  In either case we need to save
     * the toolspace allocations in order to resize the client window
     * correctly.
     */
    if (NULL==pBW)
        {
        SetRect(&m_bwIP, 0, 0, 0, 0);
        return NOERROR;
        }
    else
        {
        GetClientRect(m_hWnd, &rc);
        rc.left  +=pBW->left;
        rc.right -=pBW->right;
        rc.top   +=pBW->top;
        rc.bottom-=pBW->bottom;

        m_bwIP=*pBW;
        }

	ResizeClientWindow(rc.left, rc.top, rc.right-rc.left, rc.bottom-rc.top);
	
    return NOERROR;
    }




/*
 * CFrame::SetActiveObject
 *
 * Purpose:
 *  Provides the container with the object's IOleInPlaceActiveObject
 *  pointer
 *
 * Parameters:
 *  pIIPActiveObj   LPOLEINPLACEACTIVEOBJECT of interest.
 *  pszObj          LPCOLESTR naming the object.  Not used.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */


STDMETHODIMP CFrame::SetActiveObject
    (LPOLEINPLACEACTIVEOBJECT pIIPActiveObj, LPCOLESTR pszObj)
    {
	ATLTRACE( "CFrame::SetActiveObject\n" );

    if (NULL!=m_pIOleIPActiveObject)
		{
        m_pIOleIPActiveObject->Release();
		m_hWndObj = NULL;
		}

    //NULLs m_pIOleIPActiveObject if pIIPActiveObj is NULL
    m_pIOleIPActiveObject=pIIPActiveObj;

    if (NULL!=m_pIOleIPActiveObject)
		{
        m_pIOleIPActiveObject->AddRef();
		m_pIOleIPActiveObject->GetWindow(&m_hWndObj);
		}

    return NOERROR;
    }




/*
 * CFrame::InsertMenus
 *
 * Purpose:
 *  Instructs the container to place its in-place menu items where
 *  necessary in the given menu and to fill in elements 0, 2, and 4
 *  of the OLEMENUGROUPWIDTHS array to indicate how many top-level
 *  items are in each group.
 *
 * Parameters:
 *  hMenu           HMENU in which to add popups.
 *  pMGW            LPOLEMENUGROUPWIDTHS in which to store the
 *                  width of each container menu group.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CFrame::InsertMenus(HMENU hMenu, LPOLEMENUGROUPWIDTHS pMGW)
    {    
	ATLTRACE( "CFrame::InsertMenus\n" );

	RECT rcPos;
	RECT rcClip;
	IOleInPlaceFrame *pIIPFrame;
	IOleInPlaceUIWindow *pIIPUIWindow;
	OLEINPLACEFRAMEINFO Fi;

	m_spInPlaceSite->GetWindowContext( &pIIPFrame, &pIIPUIWindow, &rcPos, &rcClip, &Fi );
	return( pIIPFrame->InsertMenus( hMenu, pMGW ) );
    }




/*
 * CFrame::SetMenu
 *
 * Purpose:
 *  Instructs the container to replace whatever menu it's currently
 *  using with the given menu and to call OleSetMenuDescritor so OLE
 *  knows to whom to dispatch messages.
 *
 * Parameters:
 *  hMenu           HMENU to show.
 *  hOLEMenu        HOLEMENU to the menu descriptor.
 *  hWndObj         HWND of the active object to which messages are
 *                  dispatched.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CFrame::SetMenu(HMENU hMenu, HOLEMENU hOLEMenu, HWND hWndObj)
    {
	ATLTRACE( "CFrame::SetMenu\n" );

	RECT rcPos;
	RECT rcClip;
	IOleInPlaceFrame *pIIPFrame;
	IOleInPlaceUIWindow *pIIPUIWindow;
	OLEINPLACEFRAMEINFO Fi;

	if( !m_bShowMenu )
		{
		ATLTRACE( "Menus are disabled, ignoring set" );
		return( S_OK );
		}

	if( hMenu != NULL )
		{
		m_LasthMenu = hMenu;
		m_LasthOLEMenu = hOLEMenu;
		m_LasthWndObj = hWndObj;
		}

	m_spInPlaceSite->GetWindowContext( &pIIPFrame, &pIIPUIWindow, &rcPos, &rcClip, &Fi );
	return( pIIPFrame->SetMenu( hMenu, hOLEMenu, hWndObj ) );
    }




/*
 * CFrame::RemoveMenus
 *
 * Purpose:
 *  Asks the container to remove any menus it put into hMenu in
 *  InsertMenus.
 *
 * Parameters:
 *  hMenu           HMENU from which to remove the container's
 *                  items.
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

#if 0
STDMETHODIMP CFrame::RemoveMenus(HMENU hMenu)
    {
	ATLTRACE( "CFrame::RemoveMenus\n" );

	if( hMenu == m_LasthMenu )
		{
		m_LasthMenu = NULL;
		m_LasthOLEMenu = NULL;
		m_LasthWndObj = NULL;
		}

	return( S_OK );

    }
#endif
//#if 0
STDMETHODIMP CFrame::RemoveMenus(HMENU hMenu)
    {
	ATLTRACE( "CFrame::RemoveMenus\n" );

	RECT rcPos;
	RECT rcClip;
	IOleInPlaceFrame *pIIPFrame;
	IOleInPlaceUIWindow *pIIPUIWindow;
	OLEINPLACEFRAMEINFO Fi;

	if( !m_bShowMenu )
		{
		ATLTRACE( "Menus are disabled, ignoring remove" );
		return( S_OK );
		}

	if( hMenu == NULL )
		return( S_OK );

	m_spInPlaceSite->GetWindowContext( &pIIPFrame, &pIIPUIWindow, &rcPos, &rcClip, &Fi );
	return( pIIPFrame->RemoveMenus( hMenu ) );
    }
//#endif


/*
 * CFrame::SetStatusText
 *
 * Purpose:
 *  Asks the container to place some text in a status line, if one
 *  exists.  If the container does not have a status line it
 *  should return E_FAIL here in which case the object could
 *  display its own.
 *
 * Parameters:
 *  pszText         LPCOLESTR to display.
 *
 * Return Value:
 *  HRESULT         NOERROR if successful, S_TRUNCATED if not all
 *                  of the text could be displayed, or E_FAIL if
 *                  the container has no status line.
 */

STDMETHODIMP CFrame::SetStatusText(LPCOLESTR pszText)
    {
	ATLTRACE( "CFrame::SetStatusText\n" );

	RECT rcPos;
	RECT rcClip;
	IOleInPlaceFrame *pIIPFrame;
	IOleInPlaceUIWindow *pIIPUIWindow;
	OLEINPLACEFRAMEINFO Fi;

	m_spInPlaceSite->GetWindowContext( &pIIPFrame, &pIIPUIWindow, &rcPos, &rcClip, &Fi );
	return( pIIPFrame->SetStatusText( pszText ) );
    }



/*
 * CFrame::EnableModeless
 *
 * Purpose:
 *  Instructs the container to show or hide any modeless popup
 *  windows that it may be using.
 *
 * Parameters:
 *  fEnable         BOOL indicating to enable/show the windows
 *                  (TRUE) or to hide them (FALSE).
 *
 * Return Value:
 *  HRESULT         NOERROR
 */

STDMETHODIMP CFrame::EnableModeless(BOOL fEnable)
    {
	ATLTRACE( "CFrame::EnableModeless\n" );

    return NOERROR;
    }




/*
 * CFrame::TranslateAccelerator
 *
 * Purpose:
 *  When dealing with an in-place object from an EXE server, this
 *  is called to give the container a chance to process accelerators
 *  after the server has looked at the message.
 *
 * Parameters:
 *  pMSG            LPMSG for the container to examine.
 *  wID             WORD the identifier in the container's
 *                  accelerator table (from IOleInPlaceSite
 *                  ::GetWindowContext) for this message (OLE does
 *                  some translation before calling).
 *
 * Return Value:
 *  HRESULT         NOERROR if the keystroke was used,
 *                  S_FALSE otherwise.
 */

STDMETHODIMP CFrame::TranslateAccelerator(LPMSG pMSG, WORD wID)
    {
	ATLTRACE( "CFrame::TranslateAccelerator\n" );

    return S_FALSE;
    }


/*
 * IOleCommandTarget methods, provided to make PowerPoint happy
 * with this frame.
 */

STDMETHODIMP CFrame::QueryStatus(const GUID *pguidCmdGroup, ULONG cCmds
    , OLECMD *prgCmds, OLECMDTEXT *pCmdText)
	{
	ATLTRACE( "CFrame::QueryStatus\n" );

	return OLECMDERR_E_UNKNOWNGROUP;
	}
        
STDMETHODIMP CFrame::Exec(const GUID *pguidCmdGroup, DWORD nCmdID
    , DWORD nCmdexecopt, VARIANTARG *pvaIn, VARIANTARG *pvaOut)
	{
	ATLTRACE( "CFrame::Exec\n" );

	return OLECMDERR_E_UNKNOWNGROUP;
	}
    







