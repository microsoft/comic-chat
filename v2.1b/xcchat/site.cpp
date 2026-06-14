/*
 * SITE.CPP
 * Document Object Site Object
 *
 * Copyright (c)1995-1996 Microsoft Corporation, All Rights Reserved
 */
//MODIFIED FOR XCCHAT by Numbers & Co.


#include "stdafx.h"
#include "resource.h"

#include "framer.h"

/*
 * CSite::CSite
 * CSite::~CSite
 *
 * Constructor Parameters:
 *  dwID            DWORD identifer for this site.
 *  hWnd            HWND of the window associated with the site
 *  pFR             PCFrame to the parent structure.
 */

CSite::CSite(DWORD dwID, HWND hWnd, PCFrame pFR)
    {
	ATLTRACE( "CSite::CSite\n" );

    m_cRef=0;
    m_dwID=dwID;
    m_hWnd=hWnd;
    m_pFR=pFR;

    m_pIStorage=NULL;
	m_pIStream=NULL;

    m_pObj=NULL;
    
    m_pIOleObject=NULL;
    m_pIOleIPObject=NULL;
    m_pIOleDocView=NULL;
	m_pIViewObject = NULL;

    m_pImpIOleClientSite=NULL;
    m_pImpIAdviseSink=NULL;
    m_pImpIOleIPSite=NULL;
    m_pImpIOleDocumentSite=NULL;

    return;
    }


CSite::~CSite(void)
    {
	ATLTRACE( "CSite::~CSite\n" );

    //Object pointers cleaned up in Close.

    //We delete our own interfaces since we control them
    DeleteInterfaceImp(m_pImpIOleDocumentSite);
    DeleteInterfaceImp(m_pImpIOleIPSite);
    DeleteInterfaceImp(m_pImpIAdviseSink);
    DeleteInterfaceImp(m_pImpIOleClientSite);
    return;
    }




/*
 * CSite::QueryInterface
 * CSite::AddRef
 * CSite::Release
 *
 * Purpose:
 *  IUnknown members for CSite object.
 */

STDMETHODIMP CSite::QueryInterface(REFIID riid, void **ppv)
    {
	ATLTRACE( "CSite::QueryInterface\n" );

    *ppv=NULL;

    if (IID_IUnknown==riid)
		{
		ATLTRACE( "making IID_IUnknown\n" );
        *ppv=this;
		}

    if (IID_IOleClientSite==riid)
		{
		ATLTRACE( "making IID_IOleClientSite\n" );
        *ppv=m_pImpIOleClientSite;
		}

    if (IID_IAdviseSink==riid)
		{
		ATLTRACE( "making IID_IAdviseSink\n" );
        *ppv=m_pImpIAdviseSink;
		}

    if (IID_IOleWindow==riid || IID_IOleInPlaceSite==riid)
		{
		ATLTRACE( "making IID_IOleWindow or IID_IOleInPlaceSite\n" );
        *ppv=m_pImpIOleIPSite;
		}

    if (IID_IOleDocumentSite==riid)
		{
		ATLTRACE( "making IID_IOleDocumentSite\n" );
        *ppv=m_pImpIOleDocumentSite;
		}

    if (NULL!=*ppv)
        {
        ((LPUNKNOWN)*ppv)->AddRef();
        return NOERROR;
        }

	ATLTRACE( "E_NOINTERFACE\n" );
    return ResultFromScode(E_NOINTERFACE);
    }


STDMETHODIMP_(ULONG) CSite::AddRef(void)
    {
    ++m_cRef;
	ATLTRACE( "CSite::AddRef(%d)\n", m_cRef );
    return m_cRef;
    }

STDMETHODIMP_(ULONG) CSite::Release(void)
    {
    if (0!=--m_cRef)
		{
		ATLTRACE( "CSite::Release(%d)\n", m_cRef );
        return m_cRef;
		}

	ATLTRACE( "CSite::Release - deleteting object\n" );
    delete this;
    return 0;
    }






/*
 * CSite::Create
 *
 * Purpose:
 *  Asks the site to instantiate an object given a filename.
 *  This goes through OleCreateFromFile and will either create
 *  an embedded object or a package (embedded) object.  When
 *  activated, this will either launch the app and activate
 *  as a DocObject, launch the app and open the file, or fail
 *  because no app can open the file.
 *
 * Parameters:
 *  pszFile         LPTSTR of the file from which to create the object.
 *  pIStorage       IStorage * of the parent storage in which we're
 *                  to create an IStorage for the new object.
 *  dwID            DWORD identifier for this site.
 *
 * Return Value:
 *  BOOL            Result of the creation.
 */

BOOL CSite::Create( LPUNKNOWN pObj, LPSTORAGE pStg )
    {
	ATLTRACE( "CSite::Create\n" );

    HRESULT             hr=E_FAIL;

    //Create interface implementations
    m_pImpIOleClientSite=new CImpIOleClientSite(this, this);
    m_pImpIAdviseSink=new CImpIAdviseSink(this, this);
    m_pImpIOleIPSite=new CImpIOleInPlaceSite(this, this);
    m_pImpIOleDocumentSite=new CImpIOleDocumentSite(this, this);

	m_pIStorage = pStg;
	m_pIStorage->AddRef();

    if (NULL==m_pImpIOleClientSite || NULL==m_pImpIAdviseSink
        || NULL==m_pImpIOleIPSite || NULL==m_pImpIOleDocumentSite)
        return FALSE;

    return( ObjectInitialize( pObj ) );

    }





/*
 * CSite::ObjectInitialize
 * (Protected)
 *
 * Purpose:
 *  Performs operations necessary after creating an object or
 *  reloading one from storage.
 *
 * Parameters:
 *  pObj            LPUNKNOWN of the object in this tenant.
 *  pFE             LPFORMATETC describing the graphic here.
 *  dwData          DWORD extra data.  If pFE->dwAspect==
 *                  DVASPECT_ICON then this is the iconic metafile.
 *
 * Return Value:
 *  BOOL            TRUE if the function succeeded, FALSE otherwise.
 */

BOOL CSite::ObjectInitialize(LPUNKNOWN pObj)
    {
	ATLTRACE( "CSite::ObjectInitialize\n" );

    HRESULT         hr = E_FAIL;
    DWORD           dw;

    if (NULL==pObj)
        return FALSE;

    m_pObj=pObj;
	m_pObj->AddRef(); // hang on to this puppy

    //We need an IOleObject most of the time, so get one here.
    m_pIOleObject=NULL;
    hr=pObj->QueryInterface(IID_IOleObject, (void **)&m_pIOleObject);         
    if (FAILED(hr))
		{
		ATLTRACE( "can't make IID_IOleObject\n" );
        return FALSE;
		}

	m_pIViewObject = NULL;
    hr=m_pIOleObject->QueryInterface(IID_IViewObject, (void **)&m_pIViewObject);         
    if (FAILED(hr))
		{
		ATLTRACE( "can't make IID_IViewObject\n" );
        return FALSE;
		}


    //SetClientSite is critical for DocObjects
    m_pIOleObject->SetClientSite(m_pImpIOleClientSite);
    m_pIOleObject->Advise(m_pImpIAdviseSink, &dw);

    return TRUE;
    }


/*
 * CSite::Close
 *
 * Purpose:
 *  Possibly commits the storage, then releases it, afterwards
 *  frees alls the object pointers.
 *
 * Parameters:
 *  fCommit         BOOL indicating if we're to commit.
 *
 * Return Value:
 *  None
 */

void CSite::Close(BOOL fCommit)
    {
	ATLTRACE( "CSite::Close\n" );

    //OnInPlaceDeactivate releases this pointer.
    if (NULL!=m_pIOleIPObject)
        m_pIOleIPObject->InPlaceDeactivate();

    ReleaseInterface(m_pIOleDocView);
	ReleaseInterface( m_pIViewObject );

    if (NULL!=m_pIOleObject)
        {
        m_pIOleObject->Close(fCommit
            ? OLECLOSE_SAVEIFDIRTY : OLECLOSE_NOSAVE);
        ReleaseInterface(m_pIOleObject);
        }

    ReleaseInterface(m_pObj);
	ReleaseInterface(m_pIStream);
    ReleaseInterface(m_pIStorage);

    return;
    }




/*
 * CSite::Update
 *
 * Purpose:
 *  Forces a commit on the object's storage
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CSite::Update(void)
    {
	ATLTRACE( "CSite::Update\n" );

    LPPERSISTSTORAGE    pIPS;

    if (NULL == m_pIStorage)
        return;

    m_pObj->QueryInterface(IID_IPersistStorage, (void **)&pIPS);
    OleSave(pIPS, m_pIStorage, TRUE);
    pIPS->SaveCompleted(NULL);
    pIPS->Release();

    m_pIStorage->Commit(STGC_DEFAULT);
    return;
    }





/*
 * CSite::Destroy
 *
 * Purpose:
 *  Removes this storage from the parent storage.  The caller should
 *  eventually delete this CSite object to free the object herein.
 *  Nothing is committed when being destroyed.
 *
 * Parameters:
 *  pIStorage       IStorage * of the parent
 *
 * Return Value:
 *  None
 */

void CSite::Destroy(IStorage *pIStorage)
    {
	ATLTRACE( "CSite::Destroy\n" );

    if (NULL==pIStorage)
        return;

    if (NULL!=m_pObj)
        Close(FALSE);
    
    return;
    }





/*
 * CSite::Activate
 *
 * Purpose:
 *  Activates a verb on the object living in the site.
 *
 * Parameters:
 *  iVerb           LONG of the verb to execute.
 *
 * Return Value:
 *  None
 */

void CSite::Activate(LONG iVerb)
    {
	ATLTRACE( "CSite::Activate\n" );

    CHourglass *pHour;
    RECT        rc;
            
    pHour=new CHourglass;

    GetClientRect(m_hWnd, &rc);
    m_pIOleObject->DoVerb(iVerb, NULL, m_pImpIOleClientSite, 0, m_hWnd, &rc);

    delete pHour;
    return;
    }




/*
 * CSite::UpdateObjectRects
 *
 * Purpose:
 *  Informs the site that the client area window was resized and
 *  that the site needs to also tell the DocObject of the resize.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

void CSite::UpdateObjectRects(void)
    {
	ATLTRACE( "CSite::UpdateObjectRects\n" );

    RECT    rc;

    if (NULL==m_pIOleDocView)
        return;

    GetClientRect(m_hWnd, &rc);
    m_pIOleDocView->SetRect(&rc);
    return;
    }










