// IpFrame.cpp : implementation of the CInPlaceFrame class
//

#include "stdafx.h"
#include "chat.h"

#include "userinfo.h"	// required by chatprot.h
#include "chatprot.h"	// for tabbar (required by chatdoc)
#include "binddoc.h"	// for tabbar
#include "chatdoc.h"	// for tabbar
#include "tabbar.h"

#include "bindipfw.h"
#include "IpFrame.h"
#include "ui.h"


//#include "userinfo.h"
//#include "memblst.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

//BINDER:
//	An MFC application's in-place frame window is normally a subclass
//  of COleIPFrameWnd.  To be Binder-compatible, we steal code from
//  CDocObjectIPFrameWnd instead.
//BINDER_END

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame

IMPLEMENT_DYNCREATE(CInPlaceFrame, CDocObjectIPFrameWnd)

BEGIN_MESSAGE_MAP(CInPlaceFrame, CDocObjectIPFrameWnd)
	//{{AFX_MSG_MAP(CInPlaceFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_HELP_TOPICS, OnHelpTopics)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame construction/destruction

CInPlaceFrame::CInPlaceFrame()
{
}

CInPlaceFrame::~CInPlaceFrame()
{
}

int CInPlaceFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CDocObjectIPFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// CResizeBar implements in-place resizing.
	if (!m_wndResizeBar.Create(this))
	{
		TRACE0("Failed to create resize bar\n");
		return -1;      // fail to create
	}

	// By default, it is a good idea to register a drop-target that does
	//  nothing with your frame window.  This prevents drops from
	//  "falling through" to a container that supports drag-drop.
	m_dropTarget.Register(this);

	return 0;
}

// OnCreateControlBars is called by the framework to create control bars on the
//  container application's windows.  pWndFrame is the top level frame window of
//  the container and is always non-NULL.  pWndDoc is the doc level frame window
//  and will be NULL when the container is an SDI application.  A server
//  application can place MFC control bars on either window.
BOOL CInPlaceFrame::OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc)
{

	// Set owner to this window, so messages are delivered to correct app
	m_wndToolBar.SetOwner(this);

	// Create toolbar on client's frame window
	if (!m_wndToolBar.Create(pWndFrame, theApp.m_bDoCB32))
	{
		TRACE0("Failed to create toolbar\n");
		return FALSE;
	}

	cui.m_pvToolBarWnd = &m_wndToolBar;
	if (!(theApp.m_iShowBars & SB_TOOLBAR_ANY)) m_wndToolBar.ShowWindow(SW_HIDE);

	if ((m_wndToolBar.GetStyle () & WS_VISIBLE) != 0)
	{
		CControlBar * pBar = GetControlBar (AFX_IDW_DOCKBAR_TOP);
		if (pBar != NULL)
		{
			pBar->SetBarStyle (pBar->GetBarStyle () & ~CBRS_BORDER_ANY);
		}
	}

	return TRUE;
}

BOOL CInPlaceFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CDocObjectIPFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame diagnostics

#ifdef _DEBUG
void CInPlaceFrame::AssertValid() const
{
	CDocObjectIPFrameWnd::AssertValid();
}

void CInPlaceFrame::Dump(CDumpContext& dc) const
{
	CDocObjectIPFrameWnd::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CInPlaceFrame commands

void CInPlaceFrame::OnAppAbout() 
{
	((CChatApp*)AfxGetApp())->OnAppAbout();
}

void CInPlaceFrame::SetStatusString(CString& strStatus)
{
	USES_CONVERSION;
	if(m_lpFrame)
		m_lpFrame->SetStatusText(T2COLE(LPCTSTR(strStatus)));
}

void CInPlaceFrame::OnHelpTopics() 
{
	AfxGetApp()->WinHelp(0, HELP_CONTENTS);
}

BOOL CInPlaceFrame::OnQueryNewPalette() 
{
	// TODO: Add your message handler code here and/or call default
//	OutputDebugString("OnQueryNewPalette - Enter\n");

#ifndef NOGLOBPAL
//	if (ghPalette)
	{
//		OutputDebugString("OnQueryNewPalette - Doing SelectPalette\n");

		CDC *pdc = GetDC();

		UINT		uiRealized = 0;

		CPalette	*phOldPal = pdc->SelectPalette(&ghPalette, FALSE);
//		m_fPalRealized = TRUE;
		if (uiRealized = pdc->RealizePalette())
		{
//			OutputDebugString("OnQueryNewPalette - New Colors realized\n");
			InvalidateRect(NULL,TRUE);
		}


		if (phOldPal)
			pdc->SelectPalette(phOldPal, TRUE);

		ReleaseDC(pdc);

		return(uiRealized);
	}
#endif NOGLOBPAL
	return CFrameWnd::OnQueryNewPalette();
}


void CInPlaceFrame::OnPaletteChanged(CWnd* pFocusWnd) 
{
//	OutputDebugString("OnPaletteChange - Enter\n");

	CFrameWnd::OnPaletteChanged(pFocusWnd);

	// TODO: Add your message handler code here	
#ifndef NOGLOBPAL
	if (pFocusWnd == this)
			return;

//	if (ghPalette)
	{
//		OutputDebugString("OnPaletteChange - Doing SelectPalette\n");

		CDC *pdc = GetDC();

		UINT		uiRealized = 0;

		CPalette	*phOldPal = pdc->SelectPalette(&ghPalette, TRUE);
//		m_fPalRealized = TRUE;
		if (uiRealized = pdc->RealizePalette())
		{
//			OutputDebugString("OnPaletteChange - InValidating\n");
			InvalidateRect(NULL,TRUE);
		}
		if (phOldPal)
			pdc->SelectPalette(phOldPal, TRUE);

		ReleaseDC(pdc);
	}	
#endif NOGLOBPAL
}

// Mirrors CMainFrame::GetMessageString (but calls different parent's method)

void CInPlaceFrame::GetMessageString( UINT nID, CString& rMessage ) const {
	if (nID == AFX_IDS_IDLEMESSAGE)
		rMessage = theApp.m_strStatusPane[0];
	else CDocObjectIPFrameWnd::GetMessageString(nID, rMessage);
}

