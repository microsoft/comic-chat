// ChildFrm.cpp : implementation of the CChildFrame class
//

#include "stdafx.h"
#include "chat.h"
#include "ChildFrm.h"
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "spltchat.h"
#include "chatview.h"
#include "ui.h"
#include "saywnd.h"
#include "tabbar.h"
#include "mainfrm.h"
#include "protsupp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CChildFrame

IMPLEMENT_DYNCREATE(CChildFrame, CMDIChildWnd)

BEGIN_MESSAGE_MAP(CChildFrame, CMDIChildWnd)
	//{{AFX_MSG_MAP(CChildFrame)
	ON_WM_MDIACTIVATE()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
	ON_WM_WINDOWPOSCHANGING()
	ON_WM_WINDOWPOSCHANGED()
	ON_WM_DESTROY()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChildFrame construction/destruction

CChildFrame::CChildFrame()
{
	m_bPositioned = FALSE;	
}

CChildFrame::~CChildFrame()
{
}

BOOL CChildFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
#ifdef CB32SUPPORT
	if (theApp.m_bDoCB32)
		cs.style &= ~WS_SYSMENU;
#endif CB32SUPPORT
	return CMDIChildWnd::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CChildFrame diagnostics

#ifdef _DEBUG
void CChildFrame::AssertValid() const
{
	CMDIChildWnd::AssertValid();
}

void CChildFrame::Dump(CDumpContext& dc) const
{
	CMDIChildWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChildFrame message handlers

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	CChatView *view = (CChatView *) GetActiveView();

	if (bActivate)
	{
		if (!view)
			return;	// on Doc obj close up (in binder), view can be NULL
		CChatDoc *pDoc = view->GetDocument();
		cui.m_pvFocusedDoc = pDoc;
		pDoc->LoadDocData();
		pDoc->UpdateAdminMenu();
		pDoc->UpdateComicCharacterMenu();
		pDoc->ResetStatus(TRUE, TRUE);
		TRACE("Activating MDI Frame for %s\n", pDoc->GetTitle());
		SetActiveTab(pDoc);
	}
	else
	{
		currentRoom = NULL;
		cui.m_pvChatDoc = NULL;
		theApp.SetStatusPaneString(1, "");
		// if (theApp.m_bEmbedded && view->GetDocument())
		//		view->GetDocument()->m_xOleInPlaceObject.UIDeactivate();
	}

	if (currentRoom)
		currentRoom->UpdateStatus();
	else
		GetDefaultProto()->UpdateStatus();
}


void UpdateVisibilityInfo() {
	extern CPtrList g_docs;

	POSITION pos = g_docs.GetHeadPosition();
	while (pos) {
		int area1, area2 = 0;
		RECT r1, r2, inCommon;

		CChatDoc *doc = (CChatDoc *) g_docs.GetNext(pos);
		CView *client = doc->m_client;
		if (client) {
			CFrameWnd *frame = client->GetParentFrame();
//			BOOL vis = GetWindowLong(frame->m_hWnd, GWL_STYLE) & WS_VISIBLE;
			if (frame->IsIconic() /* || !vis*/) doc->m_bObscured = TRUE;
			else {
				frame->GetWindowRect(&r1);
				area1 = (r1.bottom - r1.top) * (r1.right - r1.left);
				CWnd *wnd = frame;
				while ((wnd = wnd->GetNextWindow(GW_HWNDPREV)) != NULL) {
					wnd->GetWindowRect(&r2);
					IntersectRect(&inCommon, &r1, &r2);
					area2 += (inCommon.bottom - inCommon.top) * (inCommon.right - inCommon.left);
				}
				doc->SetObscured(2*area2 >= area1);
			}
//			if (doc->m_bObscured) TRACE("%s is obscured\n", doc->m_currentRoom.strChannel);
//			else TRACE("%s is NOT obscured\n", doc->m_currentRoom.strChannel);
		}
	}
}

void CChildFrame::OnWindowPosChanging( WINDOWPOS* lpwndpos ) {
	CChatDoc* pDoc = (CChatDoc*)GetActiveDocument ();
	DWORD dwFlagsAdd, dwFlagsRemove;
	if (pDoc && pDoc->m_bStatusView)
	{
		if (theApp.m_flags0 & F0_SHOWSTATUSWINDOW)
		{
			dwFlagsRemove = SWP_HIDEWINDOW;
		}
		else
		{
			dwFlagsRemove = SWP_SHOWWINDOW;
		}
		lpwndpos->flags &= ~dwFlagsRemove;
	}
	CMDIChildWnd::OnWindowPosChanging (lpwndpos);
}

void CChildFrame::OnWindowPosChanged( WINDOWPOS* lpwndpos ) {
	WINDOWPLACEMENT wp;
	VERIFY(GetWindowPlacement(&wp));

#ifdef _DEBUG
//begin test code
BOOL vis = GetWindowLong(m_hWnd, GWL_STYLE) & WS_VISIBLE;
BOOL vis2 = IsWindowVisible();
//end text code
#endif _DEBUG

	CMDIChildWnd::OnWindowPosChanged(lpwndpos);
	UpdateVisibilityInfo();
	if (wp.flags || !(lpwndpos->flags & SWP_FRAMECHANGED) || !(lpwndpos->flags & SWP_SHOWWINDOW))
		if (m_bPositioned && !theApp.m_pExitingDoc && !theApp.m_bDoCB32 && !IsIconic() && IsWindowVisible() && !theApp.m_bEmbedded)
			SetFlag(theApp.m_flags1, F1_MAXMDI, IsZoomed());
	UpdateMDITab(lpwndpos->flags);
}


void CChildFrame::UpdateMDITab(UINT uFlags)
{
	CChatDoc*	pDoc = (CChatDoc*)GetActiveDocument ();
	CTabBar*	pTB = GetTabBar ();

	if (g_bFreezeTabs || !pDoc || !pTB || (uFlags & (SWP_SHOWWINDOW | SWP_HIDEWINDOW | SWP_FRAMECHANGED)) == 0)
		return;

	BOOL bVisible;
	if (uFlags & SWP_SHOWWINDOW)
		bVisible = TRUE;
	else if (uFlags & SWP_HIDEWINDOW)
		bVisible = FALSE;
	else if (pDoc->m_bStatusView)
		bVisible = (theApp.m_flags0 & F0_SHOWSTATUSWINDOW) != 0;
	else
		return;

	if (bVisible)
	{
		int i = pTB->FindTabNum(pDoc);
		if (i == -1)
		{
			CString QuoteAmpersands(const char *unquoted);
			CString strTitle = pDoc->GetTitle();
			CString strTabName = QuoteAmpersands(strTitle);
			pTB->AddMDITab(strTabName, pDoc, !(uFlags & SWP_NOACTIVATE));
			#ifdef DEBUG
				if (pDoc->m_bStatusView)
					TRACE("SHOWING STATUS TAB\n");
			#endif // DEBUG
		}
	}
	else
	{
		int i = pTB->FindTabNum(pDoc);
		if (i >= 0)
		{
			pTB->DelMDITab(i);
			#ifdef DEBUG
				if (pDoc->m_bStatusView)
					TRACE("HIDING STATUS TAB\n");
			#endif // DEBUG
		}
	}
}


void CChildFrame::ActivateFrame(int nCmdShow) 
{
	if (!m_bPositioned)
	{
		CChatView*	pView = (CChatView*) GetActiveView();
		CChatDoc*	pDoc = pView ? pView->GetDocument() : NULL;

		if (!pDoc || !pDoc->m_bStatusView || nCmdShow != SW_SHOWNOACTIVATE)
		{
			if (theApp.m_bDoCB32 || (theApp.m_flags1 & F1_MAXMDI))
				MDIMaximize();
			else
			{
				// also maximize if 0 height.  This will happen when you accept an invitation with MainFrame closed
				// should really choose a reasonable size, but not necessarily maximize
				RECT r;
				GetClientRect(&r);
				if (r.bottom == 0) 
					MDIMaximize();
			}
		}
		
		if (pDoc && !pDoc->m_bStatusView)
		{
			// On activation of other views, verify that the status view is still
			// hidden.
			CFrameWnd*	pFrame = GetStatusView () ? GetStatusView()->GetParentFrame() : NULL;
			if (pFrame && pFrame->IsWindowVisible () && 
				(theApp.m_flags0 & F0_SHOWSTATUSWINDOW) == 0)
				pFrame->ShowWindow (SW_HIDE);
		}

		m_bPositioned = TRUE;
	}
	
	CMDIChildWnd::ActivateFrame(nCmdShow);
}

void CChildFrame::GetMessageString( UINT nID, CString& rMessage ) const {
	if (nID == AFX_IDS_IDLEMESSAGE)
		rMessage = theApp.m_strStatusPane[0];
	else CMDIChildWnd::GetMessageString(nID, rMessage);
}

void CChildFrame::OnClose() 
{
	CChatDoc *doc = (CChatDoc *) GetActiveDocument();
	if (doc->m_bStatusView)
	{
		ASSERT(theApp.m_flags0 & F0_SHOWSTATUSWINDOW);
		theApp.OnViewStatuswindow();		// don't really close... just hide status window
	}
	else
	{
		CMDIChildWnd::OnClose();
	}
}

int 
CChildFrame::OnCreate(
LPCREATESTRUCT lpCreateStruct)
{
	int nRet = CMDIChildWnd::OnCreate (lpCreateStruct);
	if (nRet != -1 && !theApp.m_bEmbedded)
	{
		// Doesn't do it immediately, does it on a PostMessage.
		((CMainFrame*)::AfxGetMainWnd ())->AutoArrangeWindows ();
	}
	return nRet;
}

void
CChildFrame::OnDestroy()
{
	CMDIChildWnd::OnDestroy ();
	if (!theApp.m_bEmbedded)
	{
		// Doesn't do it immediately, does it on a PostMessage.
		((CMainFrame*)::AfxGetMainWnd ())->AutoArrangeWindows ();
	}
}

void 
CChildFrame::OnUpdateFrameMenu(
BOOL bActive, 
CWnd* pActivateWnd,
HMENU hMenuAlt)
{
	CMDIChildWnd::OnUpdateFrameMenu (bActive, pActivateWnd, hMenuAlt);
	::AfxGetMainWnd ()->SendMessage (WM_COMMAND, ID_FAVORITES_LAST);
}
