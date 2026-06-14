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
	//}}AFX_MSG_MAP
	ON_WM_WINDOWPOSCHANGED()
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
	if (theApp.m_bDoCB32)
		cs.style &= ~WS_SYSMENU;
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

void CRoomInfo::UpdateStatus() {	
	CString leftMesg;
	int iConn = GetConnectionStatus();
	if (iConn == CX_DISCONNECTED)
		leftMesg.LoadString(ID_DISCONNECTED);
	else if (iConn == CX_CONNECTING)
		leftMesg.LoadString(ID_CONNECTING); 
	else if (iConn == CX_NOCHANNEL) {
		if (currentRoom && currentRoom != this && this != GetDefaultProto()) return;  // only write new status if it's for current room!!!
		leftMesg.LoadString(ID_NOCHANNEL);
		const char *GetMyServer();
		VERIFY(ReplaceToken(leftMesg, CString("%1"), GetMyServer()));
	} else if (iConn = CX_INCHANNEL) {
		if (currentRoom && currentRoom != this) return;  // only write new status if it's for current room!!!
		leftMesg = ((CChatDoc *)m_doc)->m_strStatus;
	}

	if (!leftMesg.IsEmpty())
		theApp.SetStatusPaneString(0, leftMesg);
}

void CChildFrame::OnMDIActivate(BOOL bActivate, CWnd* pActivateWnd, CWnd* pDeactivateWnd) 
{
	CMDIChildWnd::OnMDIActivate(bActivate, pActivateWnd, pDeactivateWnd);

	CChatView *view = (CChatView *) GetActiveView();

	if (bActivate) {
		if (!view) return;	// on Doc obj close up (in binder), view can be NULL
		CChatDoc *doc = view->GetDocument();
		cui.m_pvFocusedDoc = doc;
		doc->LoadDocData();
		doc->UpdateAdminMenu();
		doc->ResetStatus(TRUE, TRUE);
		TRACE("Activating MDI Frame for %s\n", doc->GetTitle());
		void SetActiveTab(CChatDoc *);
		/*if (*roomName) */ SetActiveTab(doc);
//		if (theApp.m_bEmbedded) doc->OnActivateView();
	} else {
		currentRoom = NULL;
		cui.m_pvChatDoc = NULL;
		theApp.SetStatusPaneString(1, "");
//		if (theApp.m_bEmbedded && view->GetDocument()) view->GetDocument()->m_xOleInPlaceObject.UIDeactivate();
	}

	if (currentRoom) currentRoom->UpdateStatus();
	else GetDefaultProto()->UpdateStatus();
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
			BOOL vis = GetWindowLong(frame->m_hWnd, GWL_STYLE) & WS_VISIBLE;
			if (frame->IsIconic() || !vis) doc->m_bObscured = TRUE;
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

void CChildFrame::OnWindowPosChanged( WINDOWPOS* lpwndpos ) {
	CMDIChildWnd::OnWindowPosChanged(lpwndpos);
	UpdateVisibilityInfo();
	if (m_bPositioned && !theApp.m_bDoCB32 && !IsIconic() && !theApp.m_bEmbedded)
		SetFlag(theApp.m_flags1, F1_MAXMDI, IsZoomed()); 
}


void CChildFrame::ActivateFrame(int nCmdShow) 
{
	if (! m_bPositioned) {
		if (theApp.m_bDoCB32 || (theApp.m_flags1 & F1_MAXMDI))
			MDIMaximize();
		else {
			// also maximize if 0 height.  This will happen when you accept an invitation with MainFrame closed
			// should really choose a reasonable size, but not necessarily maximize
			RECT r;
			GetClientRect(&r);
			if (r.bottom == 0) 
				MDIMaximize();
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
