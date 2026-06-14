/*
 * ICLISITE.CPP
 * IOleClientSite for Document Objects CSite class
 *
 * Copyright (c)1995-1996 Microsoft Corporation, All Rights Reserved
 */
//MODIFIED FOR XCCHAT by Numbers & Co.


#include "stdafx.h"
#include "resource.h"

#include "framer.h"


/*
 * CImpIOleClientSite::CImpIOleClientSite
 * CImpIOleClientSite::~CImpIOleClientSite
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

CImpIOleClientSite::CImpIOleClientSite(PCSite pSite, LPUNKNOWN pUnkOuter)
    {
	ATLTRACE( "CImpIOleClientSite::CImpIOleClientSite\n" );

    m_cRef=0;
    m_pSite=pSite;
    m_pUnkOuter=pUnkOuter;
    return;
    }

CImpIOleClientSite::~CImpIOleClientSite(void)
    {
	ATLTRACE( "CImpIOleClientSite::~CImpIOleClientSite\n" );

    return;
    }




/*
 * CImpIOleClientSite::QueryInterface
 * CImpIOleClientSite::AddRef
 * CImpIOleClientSite::Release
 *
 * Purpose:
 *  IUnknown members for CImpIOleClientSite object.
 */

STDMETHODIMP CImpIOleClientSite::QueryInterface(REFIID riid, void **ppv)
    {
	ATLTRACE( "CImpIOleClientSite::QueryInterface\n" );

    return m_pUnkOuter->QueryInterface(riid, ppv);
    }


STDMETHODIMP_(ULONG) CImpIOleClientSite::AddRef(void)
    {
    ++m_cRef;
	ATLTRACE( "CImpIOleClientSite::AddRef(%d)\n", m_cRef );
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) CImpIOleClientSite::Release(void)
    {
    --m_cRef;
	ATLTRACE( "CImpIOleClientSite::Release(%d)\n", m_cRef );
    return m_pUnkOuter->Release();
    }




/*
 * CImpIOleClientSite::SaveObject
 *
 * Purpose:
 *  Requests that the container call OleSave for the object that
 *  lives here.  Typically this happens on server shutdown.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  HRESULT         Standard.
 */

STDMETHODIMP CImpIOleClientSite::SaveObject(void)
    {
	ATLTRACE( "CImpIOleClientSite::SaveObject\n" );

    m_pSite->Update();
    return NOERROR;
    }




/*
 * Unimplemented/trivial members
 *  GetMoniker
 *  GetContainer
 *  RequestNewObjectLayout
 *  OnShowWindow
 *  ShowObject
 */

STDMETHODIMP 
	CImpIOleClientSite::GetMoniker(DWORD dwAssign, DWORD dwWhich, LPMONIKER *ppmk)
    {
	ATLTRACE( "CImpIOleClientSite::GetMoniker\n" );

    return E_NOTIMPL;
    }

STDMETHODIMP CImpIOleClientSite::GetContainer(LPOLECONTAINER * ppContainer)
    {
	ATLTRACE( "CImpIOleClientSite::GetContainer\n" );

    return E_NOTIMPL;
    }

STDMETHODIMP CImpIOleClientSite::RequestNewObjectLayout(void)
    {
	ATLTRACE( "CImpIOleClientSite::RequestNewObjectLayout\n" );

    return E_NOTIMPL;
    }

STDMETHODIMP CImpIOleClientSite::OnShowWindow(BOOL fShow)
    {
	ATLTRACE( "CImpIOleClientSite::OnShowWindow\n" );

    return NOERROR;
    }

STDMETHODIMP CImpIOleClientSite::ShowObject(void)
    {
	ATLTRACE( "CImpIOleClientSite::ShowObject\n" );

    return NOERROR;
    }








