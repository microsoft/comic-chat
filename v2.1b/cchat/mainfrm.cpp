// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "chat.h"

#include "userinfo.h"	// required by chatprot.h
#include "chatprot.h"	// for tabbar (required by chatdoc)
#include "binddoc.h"	// for tabbar
#include "chatdoc.h"	// for tabbar
#include "tabbar.h"
#include "MainFrm.h"

#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "bodycam.h"
#include "ui.h"
#include <afxpriv.h>
#include "memblst.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CMDIFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CMDIFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	ON_WM_PALETTECHANGED()
	ON_WM_QUERYNEWPALETTE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // connected/disconnected
	ID_SEPARATOR			// # users
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	// TODO: add member initialization code here
//		_CrtSetBreakAlloc(399);
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CMDIFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	cui.m_pvFrameWnd = this;					// save this away for later use

	if (!m_wndToolBar.Create(this) || !m_wndToolBar.LoadToolBar(theApp.m_bDoCB32 ? IDR_NM_MAIN : IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	cui.m_pvToolBarWnd = &m_wndToolBar;
	if (!(theApp.m_iShowBars & SB_TOOLBAR)) m_wndToolBar.ShowWindow(SW_HIDE);

//  Create our own status bar so that we can configure the panes
	if (!m_wndStatusBar.Create(this) ||
//	if (!m_wndStatusBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,ID_CCHAT_STATUSBAR) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	cui.m_pvStatusBarWnd = &m_wndStatusBar;
	if (!(theApp.m_iShowBars & SB_STATUSBAR)) m_wndStatusBar.ShowWindow(SW_HIDE);

	m_wndStatusBar.SetFont(&theApp.m_fontGui);  // give status bar and mainframe correct font
//	SetFont(&theApp.m_fontGui);					// doesn't seem to work for title bar.  Why?

	UINT nID;
	UINT nStyle;
	int  cxWidth;

	m_wndStatusBar.GetPaneInfo(1,nID,nStyle,cxWidth);
	m_wndStatusBar.SetPaneInfo(1,nID,nStyle,75);

	
	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

	CString tabTitle;
	tabTitle.LoadString(IDS_TABTITLE);
	if (!m_wndTabBar.Create(NULL, tabTitle, WS_CHILD | WS_VISIBLE, CRect(0, 0, 100, 100), this, 81)) {
		TRACE0("Failed to create tabbar\n");
		return -1;      // fail to create
	}

	m_wndTabBar.SetBarStyle((m_wndTabBar.GetBarStyle() | CBRS_SIZE_DYNAMIC) & ~(CBRS_BORDER_TOP | CBRS_BORDER_BOTTOM | CBRS_BORDER_LEFT | CBRS_BORDER_RIGHT));
	m_wndTabBar.EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);
	DockControlBar(&m_wndTabBar, AFX_IDW_DOCKBAR_TOP);
	cui.m_pvTabBar = &m_wndTabBar;
	if (theApp.m_bDoCB32 || !(theApp.m_flags1 & F1_SHOWTABBAR)) m_wndTabBar.ShowWindow(SW_HIDE);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	if (theApp.m_cxFrame && theApp.m_cyFrame) {
		cs.x = theApp.m_xFrame;
		cs.y = theApp.m_yFrame;
		cs.cx = theApp.m_cxFrame;
		cs.cy = theApp.m_cyFrame;
		// note: maximization setting happens in CChatApp.InitInstance
	}

	return CMDIFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CMDIFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CMDIFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers


void CMainFrame::OnViewStatusBar() 
{
	m_wndStatusBar.ShowWindow((m_wndStatusBar.GetStyle() & WS_VISIBLE)==0);
	RecalcLayout();
}

void CMainFrame::OnUpdateViewStatusBar(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck((m_wndStatusBar.GetStyle() & WS_VISIBLE)!=0);
}


void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}


BOOL CMainFrame::OnQueryNewPalette() 
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
	return CMDIFrameWnd::OnQueryNewPalette();
}


void CMainFrame::OnPaletteChanged(CWnd* pFocusWnd) 
{
//	OutputDebugString("OnPaletteChange - Enter\n");

	CMDIFrameWnd::OnPaletteChanged(pFocusWnd);

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


void CMainFrame::GetMessageString( UINT nID, CString& rMessage ) const {
	if (nID == AFX_IDS_IDLEMESSAGE)
		rMessage = theApp.m_strStatusPane[0];
	else CMDIFrameWnd::GetMessageString(nID, rMessage);
}


void CMainFrame::OnTimer(UINT nIDEvent) 
{
	void OnEaster();
	if (nIDEvent == EASTER_TIMER) OnEaster();
	CMDIFrameWnd::OnTimer(nIDEvent);
}
