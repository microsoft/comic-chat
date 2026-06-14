// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "chat.h"

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

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateViewStatusBar)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT_PREVIEW, OnUpdateFilePrintPreview)
	//}}AFX_MSG_MAP
//	ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // connected/disconnected
//	ID_SEPARATOR,			// ringed by....
	ID_SEPARATOR			// # users
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
//	ID_NUMPRESENT,
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
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	cui.m_pvFrameWnd = this;					// save this away for later use


	// removed toolbar (ericg 7/3/96)
/*	if (!m_wndToolBar.Create(this) ||
	!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndToolBar.ShowWindow(SW_HIDE);*/
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

	UINT nID;
	UINT nStyle;
	int  cxWidth;

	m_wndStatusBar.GetPaneInfo(1,nID,nStyle,cxWidth);
	m_wndStatusBar.SetPaneInfo(1,nID,nStyle,75);

//	m_wndStatusBar.GetPaneInfo(1,nID,nStyle,cxWidth);
//	m_wndStatusBar.SetPaneInfo(1,nID,nStyle,375);

	
	// TODO: Remove this if you don't want tool tips or a resizeable toolbar
//	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() |
//		CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC);

	// TODO: Delete these three lines if you don't want the toolbar to
	//  be dockable
//	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
//	EnableDocking(CBRS_ALIGN_ANY);
//	DockControlBar(&m_wndToolBar);

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

	return CFrameWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
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


/*LRESULT CMainFrame::OnSetMessageString(WPARAM wParam, LPARAM lParam)
{
	if(wParam == AFX_IDS_IDLEMESSAGE)
	{
		CString strTemp = ((CChatApp*)AfxGetApp())->m_strStatusPane[0];
		((CChatApp*)AfxGetApp())->SetStatusPaneString(0, strTemp);
	}
	else
		((CChatApp*)AfxGetApp())->SetStatusPaneString(0, CString("Testing"));
	return wParam;
}*/
void CMainFrame::OnUpdateFilePrintPreview(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
}

