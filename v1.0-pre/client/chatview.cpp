// chatView.cpp : implementation of the CChatView class
//

#include "stdafx.h"
#include "chat.h"

#include "binddoc.h"
#include "chatDoc.h"
#include "spltchat.h"
#include "chatView.h"
#include "dib.h"
#include "script.h"
#include "userinfo.h"
#include "textview.h"
#include "pageview.h"
#include "dumbwnd.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "bodycam.h"
#include "saywnd.h"
#include "ui.h"
#include "memblst.h"
#include "avatardl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChatView

IMPLEMENT_DYNCREATE(CChatView, CView)

BEGIN_MESSAGE_MAP(CChatView, CView)
	//{{AFX_MSG_MAP(CChatView)
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_COMMAND(ID_AVATARCONTEXT_GETINFO, OnGetinfo)
	ON_COMMAND(ID_AVATARCONTEXT_IGNORE, OnIgnore)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatView construction/destruction

CChatView::CChatView()
{
	m_hpleft = (float)0.90;

	m_wndSplitter = NULL;	// splitters not yet instantiated
	m_wndLSplitter = NULL;
	m_wndRSplitter = NULL;
}

CChatView::~CChatView()
{
	if (m_wndSplitter) delete m_wndSplitter;
	if (m_wndLSplitter) delete m_wndLSplitter;
	if (m_wndRSplitter) delete m_wndRSplitter;
}

/////////////////////////////////////////////////////////////////////////////
// CChatView drawing - NONE!

void CChatView::OnDraw(CDC* pDC)
{
	CChatDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
}

/////////////////////////////////////////////////////////////////////////////
// CChatView printing -- just call CPageView's functions
//   -- could we just send a message to GetView()?

BOOL CChatView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return GetView()->OnPreparePrinting(pInfo);
}

void CChatView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	GetView()->OnBeginPrinting(pDC, pInfo);
}

void CChatView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	GetView()->OnEndPrinting(pDC, pInfo);
}

void CChatView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	GetView()->OnPrint(pDC, pInfo);
}


void CChatView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CView *innerView = (CView *) GetView();
	if (innerView) innerView->OnPrepareDC(pDC, pInfo);
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server support

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the server (not the container) causes the deactivation.
void CChatView::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}

/////////////////////////////////////////////////////////////////////////////
// CChatView diagnostics

#ifdef _DEBUG
void CChatView::AssertValid() const
{
	CView::AssertValid();
}

void CChatView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CChatDoc* CChatView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChatDoc)));
	return (CChatDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChatView message handlers

int CChatView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	cui.m_pvChatView = this;			// save for later ref

	ASSERT( GetParentFrame());
	// since we are the splitters parent the splitter
	// doesn't set the parent frames style, so we have to.
	GetParentFrame()->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);

	// Create the comic view
	if(GetChatApp()->m_bComicView)
		CreateComicView(FALSE);
	else
		CreateTextView(FALSE);

	return 0;
}


void CChatView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	RECT rect;
	GetClientRect(&rect);
	if ( m_wndSplitter && m_wndLSplitter && m_wndSplitter->m_hWnd && m_wndLSplitter->m_hWnd /* && m_wndRSplitter->m_hWnd */)
	{
		m_wndSplitter->SetWindowPos( NULL, 0, 0, 
									rect.right, 
									rect.bottom, 
									SWP_NOZORDER | SWP_NOACTIVATE );
		
	}
}

void CChatView::GetContext(CCreateContext* pContext)
{
	pContext->m_pNewViewClass=NULL;
	pContext->m_pCurrentDoc= GetDocument();
	pContext->m_pNewDocTemplate= GetDocument()?GetDocument()->GetDocTemplate():NULL;
	pContext->m_pLastView	= NULL;
	pContext->m_pCurrentFrame= GetParentFrame();
}

// doUpdate should be false, when the view is initially created in place, true otherwise
void CChatView::CreateComicView(BOOL doUpdate)
{
	CCreateContext Context;
	CCreateContext* pContext = &Context;
	GetContext(pContext);

	// instantiate the splitters
	ASSERT(!m_wndSplitter && !m_wndRSplitter && !m_wndLSplitter); // shouldn't be instantiated yet
	m_wndSplitter = new CSplitChatV;
	m_wndRSplitter = new CSplitChat;
	m_wndLSplitter = new CFixedSplitter;
	
	// first create the vertical splitter
	m_wndSplitter->CreateStatic( this ,1, 2);
	m_wndSplitter->SetColumnInfo( 0, 0, 0 );
	m_wndSplitter->SetColumnInfo( 1, 10, 0 );
		
	// next the right hand side horizontal splitter
	m_wndRSplitter->CreateStatic(m_wndSplitter,2, 1,WS_CHILD | WS_VISIBLE | WS_BORDER,
		m_wndSplitter->IdFromRowCol(0, 1));

	m_wndRSplitter->CreateView(0, 0,RUNTIME_CLASS(CMemberList), CSize(0, 0),
		pContext);

	m_wndRSplitter->CreateView(1, 0,RUNTIME_CLASS(CBodyCam), CSize(0, 10), pContext);


	// finally the left hand side horizontal splitter
	m_wndLSplitter->CreateStatic(m_wndSplitter, 2, 1,WS_CHILD | WS_VISIBLE | WS_BORDER,
		m_wndSplitter->IdFromRowCol(0, 0));


	m_wndLSplitter->CreateView(0,0,RUNTIME_CLASS( CPageView ), CSize(0,0), pContext);

	m_wndLSplitter->CreateView(1,0,RUNTIME_CLASS( CSayWnd ), CSize(0,40), pContext);

	cui.m_pvClientView = (void *) m_wndLSplitter->GetPane(0, 0);	// save windows for easy ref.
	cui.m_pvSayWnd = (void *) m_wndLSplitter->GetPane(1, 0);

	cui.m_pvBodyCamWnd = (void *) m_wndRSplitter->GetPane(1, 0);	// save for easy reference
	cui.m_pvMembersWnd = (void *) m_wndRSplitter->GetPane(0, 0);
	if(GetDocument()->m_bLastMemberView)
		GetDocument()->OnViewIcon();							//  members back to icons
	else
		GetDocument()->OnViewList();							// or members back to list
	
	if (doUpdate) {
		// Need to make comic view active, so it's the comic view's OnInitialUpdate that will be called...
		((CFrameWnd *)AfxGetMainWnd())->SetActiveView(GetView());

		// we should force a new OnInitialUpdate since we arent recreating the chatview
		((CFrameWnd*)AfxGetMainWnd())->InitialUpdateFrame(GetDocument(),TRUE);
		
		// we have to force a resize since we arent really recreating the chatview.
		CRect rect;
		GetClientRect(&rect);
		OnSize(SIZE_RESTORED,rect.right,rect.bottom);
	}
}

// doUpdate should be false, when the view is initially created in place, true otherwise
void CChatView::CreateTextView(BOOL doUpdate)
{
	CCreateContext Context;
	CCreateContext* pContext = &Context;
	GetContext(pContext);

	// instantiate the splitters
	ASSERT(!m_wndSplitter && !m_wndLSplitter); // shouldn't be instantiated yet
	m_wndSplitter = new CSplitChatV;
	m_wndLSplitter = new CFixedSplitter;

	// first create the vertical splitter
	m_wndSplitter->CreateStatic( this ,1, 2);
	m_wndSplitter->SetColumnInfo( 0, 0, 0 );
	m_wndSplitter->SetColumnInfo( 1, 10, 0 );
		
	m_wndSplitter->CreateView(0, 1,RUNTIME_CLASS(CMemberList), CSize(0, 0),
		pContext);
	cui.m_pvMembersWnd = (void *) m_wndSplitter->GetPane(0, 1);
	// finally the left hand side horizontal splitter
	m_wndLSplitter->CreateStatic(m_wndSplitter, 2, 1,WS_CHILD | WS_VISIBLE | WS_BORDER,
		m_wndSplitter->IdFromRowCol(0, 0));


	m_wndLSplitter->CreateView(0,0,RUNTIME_CLASS( CTextView ), CSize(200,200), pContext);

	m_wndLSplitter->CreateView(1,0,RUNTIME_CLASS( CSayWnd ), CSize(0,40), pContext);

	cui.m_pvClientView = (void *) m_wndLSplitter->GetPane(0, 0);	// save windows for easy ref.
	cui.m_pvSayWnd = (void *) m_wndLSplitter->GetPane(1, 0);

	GetDocument()->m_bLastMemberView = GetDocument()->m_bIconMembers;

	GetDocument()->OnViewList();							// make sure members not displayed as icons
	
	if (doUpdate) {
		// Need to make text view active, so it's the text view's OnInitialUpdate that will be called...
		((CFrameWnd *)AfxGetMainWnd())->SetActiveView(GetTextView());

		// we should force a new OnInitialUpdate since we arent recreating the chatview
		((CFrameWnd*)AfxGetMainWnd())->InitialUpdateFrame(GetDocument(),TRUE);
		
		// we have to force a resize since we arent really recreating the chatview.
		CRect rect;
		GetClientRect(&rect);
		OnSize(SIZE_RESTORED,rect.right,rect.bottom);
	}
}

void CChatView::CleanUpBeforeChangeView()
{
	cui.m_pvClientView = NULL;		// no client view (temporarily)
	((CFrameWnd *)AfxGetMainWnd())->SetActiveView(NULL);  // active view must *always* be valid view or NULL
	CFrameWnd *origParent = (CFrameWnd *) (GetDocument()->GetOrigParent());
	if (origParent) origParent->SetActiveView(NULL);

	m_wndSplitter->DestroyWindow();

	delete m_wndSplitter;
	m_wndSplitter = NULL;
	delete m_wndLSplitter;
	m_wndLSplitter = NULL;
	if (m_wndRSplitter) {
		delete m_wndRSplitter;
		m_wndRSplitter = NULL;
	}
}
/////////////////////////////////////////////////////////////////////////////
// CFixedSplitter

//IMPLEMENT_DYNCREATE(CFixedSplitter, CSplitSay)

CFixedSplitter::CFixedSplitter()
{
}

CFixedSplitter::~CFixedSplitter()
{
}


BEGIN_MESSAGE_MAP(CFixedSplitter, CSplitSay)
	//{{AFX_MSG_MAP(CFixedSplitter)
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONDBLCLK()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
//CFixedSplitter message handlers

void CFixedSplitter::OnLButtonDown(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDown(nFlags, point);
}

void CFixedSplitter::OnLButtonUp(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonUp(nFlags, point);
}

void CFixedSplitter::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnMouseMove(nFlags, point);
}

void CFixedSplitter::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default
	
	CWnd::OnLButtonDblClk(nFlags, point);
}

//////////////////////////////////////////////////////////////////////////////

void OurMessageBox(const char *mesg) {
	CString title;
	title.LoadString(ID_MESSAGE_BOX_TITLE);
	CView *cv = GetChatView();
//	CWnd *wnd = AfxGetThread()->GetMainWnd();
//	wnd->MessageBox(mesg, title);
	HWND fwnd = GetForegroundWindow();
	CWnd *wnd = CWnd::FromHandle(fwnd);
	wnd->MessageBox(mesg, title, MB_SETFOREGROUND);
}

void OurMessageBox(UINT id) {
	CString mesg;
	mesg.LoadString(id);
	OurMessageBox(mesg);
}


void CChatView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	LPARAM lparam;
	lparam = nFlags << 16;
	lparam |= (WORD)nRepCnt;

	HWND hSayEdit = GetSay()->GetSayEdit();
	CSayCtrl* pSayCtrl = ((CSayCtrl*)FromHandle(hSayEdit));
	pSayCtrl->SendMessage(WM_CHAR,nChar,lparam);
	GetSay()->SetFocusToSayWnd();
}

extern CUserInfo *GetSingleSelectedMember();

void CChatView::OnGetinfo() 
{
	void ChatGetInfo(CUserInfo *);
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui) ChatGetInfo(pui);	
}

void CChatView::OnIgnore() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (!pui) return;
	BOOL ignore = !pui->Ignored();
	pui->Ignore(ignore);	
}

#if 0
void CChatView::OnRing() 
{
	void ChatRingUser(CUserInfo *);
	CUserInfo *pui = GetSingleSelectedMember();
	ChatRingUser(pui);
}
#endif
