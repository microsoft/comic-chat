/*
 * IADVSINK.CPP
 * IAdviseSink for Document Objects CSite class
 *
 * Copyright (c)1995-1996 Microsoft Corporation, All Rights Reserved
 */
//MODIFIED FOR XCCHAT by Numbers & Co.


#include "stdafx.h"
#include "resource.h"

#include "framer.h"


/*
 * CImpIAdviseSink::CImpIAdviseSink
 * CImpIAdviseSink::~CImpIAdviseSink
 *
 * Parameters (Constructor):
 *  pSite           PCSite of the site we're in.
 *  pUnkOuter       LPUNKNOWN to which we delegate.
 */

CImpIAdviseSink::CImpIAdviseSink(PCSite pSite, LPUNKNOWN pUnkOuter)
    {
	ATLTRACE( "CImpIAdviseSink::CImpIAdviseSink\n" );

    m_cRef=0;
    m_pSite=pSite;
    m_pUnkOuter=pUnkOuter;
    return;
    }

CImpIAdviseSink::~CImpIAdviseSink(void)
    {
	ATLTRACE( "CImpIAdviseSink::~CImpIAdviseSink\n" );

    return;
    }




/*
 * CImpIAdviseSink::QueryInterface
 * CImpIAdviseSink::AddRef
 * CImpIAdviseSink::Release
 *
 * Purpose:
 *  IUnknown members for CImpIAdviseSink object.
 */

STDMETHODIMP CImpIAdviseSink::QueryInterface(REFIID riid, void **ppv)
    {
	ATLTRACE( "CImpIAdviseSink::QueryInterface\n" );

    return m_pUnkOuter->QueryInterface(riid, ppv);
    }


STDMETHODIMP_(ULONG) CImpIAdviseSink::AddRef(void)
    {
    ++m_cRef;
	ATLTRACE( "CImpIAdviseSink::AddRef(%d)\n", m_cRef );
    return m_pUnkOuter->AddRef();
    }

STDMETHODIMP_(ULONG) CImpIAdviseSink::Release(void)
    {
    --m_cRef;
	ATLTRACE( "CImpIAdviseSink::Release(%d)\n", m_cRef );
    return m_pUnkOuter->Release();
    }



/*
 * Unused members in CImpIAdviseSink
 *  OnDataChange
 *  OnViewChange
 *  OnRename
 *  OnSave
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnDataChange(LPFORMATETC pFEIn, LPSTGMEDIUM pSTM)
    {
	ATLTRACE( "CImpIAdviseSink::OnDataChange\n" );

    return;
    }

STDMETHODIMP_(void) CImpIAdviseSink::OnViewChange(DWORD dwAspect, LONG lindex)
    {    
	ATLTRACE( "CImpIAdviseSink::OnViewChange\n" );

    return;
    }

STDMETHODIMP_(void) CImpIAdviseSink::OnRename(LPMONIKER pmk)
    {
	ATLTRACE( "CImpIAdviseSink::OnRename\n" );

    return;
    }

STDMETHODIMP_(void) CImpIAdviseSink::OnSave(void)
    {
	ATLTRACE( "CImpIAdviseSink::OnSave\n" );

    return;
    }




/*
 * CImpIAdviseSink::OnClose
 *
 * Purpose:
 *  Informs the advise sink that the OLE object has closed and is
 *  no longer bound in any way.  We use this to do a File/Close
 *  to delete the object since we don't hold onto any.
 *
 * Parameters:
 *  None
 *
 * Return Value:
 *  None
 */

STDMETHODIMP_(void) CImpIAdviseSink::OnClose(void)
    {
	ATLTRACE( "CImpIAdviseSink::OnClose\n" );

    return;
    }








