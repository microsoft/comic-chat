// chatView.cpp : implementation of the CChatView class
//

#include "stdafx.h"
#include "chat.h"
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatDoc.h"
#include "spltchat.h"
#include "chatView.h"
#include "dib.h"
#include "textcore.h"
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
#include <imm.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

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
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
	ON_MESSAGE(WM_LOGINDLG, OnLoginDlg)
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
	CChatDoc *doc = GetDocument();
	doc->m_memberList = NULL;
	doc->m_client = NULL;
	doc->m_bodyCam = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// CChatView drawing - NONE!


void CChatView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	// Do not call CView::OnPaint() for painting messages
}



BOOL CChatView::OnEraseBkgnd(CDC* pDC) 
{
	return TRUE; //CView::OnEraseBkgnd(pDC);
}

/////////////////////////////////////////////////////////////////////////////
// CChatView printing -- just call CPageView's functions
//   -- could we just send a message to GetView()?

BOOL CChatView::OnPreparePrinting(CPrintInfo* pInfo)
{
	return ((CPageView *)GetPrimaryView())->OnPreparePrinting(pInfo);
}

void CChatView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	((CPageView *)GetPrimaryView())->OnBeginPrinting(pDC, pInfo);
}

void CChatView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	((CPageView *)GetPrimaryView())->OnEndPrinting(pDC, pInfo);
}

void CChatView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	((CPageView *)GetPrimaryView())->OnPrint(pDC, pInfo);
}


void CChatView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	CPageView *innerView = (CPageView *)GetPrimaryView();
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
	
	if (ghPalette.m_hObject == NULL) {
#ifndef NOGLOBPAL

	// GdiSetBatchLimit(1);	// RamuM

	LOGPALETTE *lp = gpLogPal;

	lp->palVersion = 0x300;
	lp->palNumEntries = 256;

	// Assumes that the entire palette is full. RamuM

	//*** Just use the screen DC where we need it
	HDC hdc = ::GetDC (NULL);

#ifndef USE_HALFTONE_PALETTE

	CDIB	PalDib;
	VERIFY(PalDib.Load(IDR_PALETTE));
	BITMAPINFO *PalDibInfo =  PalDib.GetBitmapInfoAddress();
	ASSERT(PalDibInfo);

	//*** For SYSPAL_NOSTATIC, just copy the color table into
	//*** a PALETTEENTRY array and replace the first and last entries
	//*** with black and white
	if (GetSystemPaletteUse (hdc) == SYSPAL_NOSTATIC)
	{
		//*** Make sure the last entry is white
		//*** This may replace an entry in the array!
		lp->palPalEntry[255].peRed = 255;
		lp->palPalEntry[255].peGreen = 255;
		lp->palPalEntry[255].peBlue = 255;
		lp->palPalEntry[255].peFlags = 0;

		//*** And the first is black
		//*** This may replace an entry in the array!
		lp->palPalEntry[0].peRed = 0;
		lp->palPalEntry[0].peGreen = 0;
		lp->palPalEntry[0].peBlue = 0;
		lp->palPalEntry[0].peFlags = 0;

		// fill the remaining colors
		for (int i = 1; i < 255; i++) 
		{
			lp->palPalEntry[i].peRed = PalDibInfo->bmiColors[i].rgbRed;
			lp->palPalEntry[i].peGreen = PalDibInfo->bmiColors[i].rgbGreen;
			lp->palPalEntry[i].peBlue = PalDibInfo->bmiColors[i].rgbBlue;
			lp->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
		}

	}
	else
	//*** For SYSPAL_STATIC, get the twenty static colors into
	//*** the array, then fill in the empty spaces with the
	//*** given color table
	{
		int nStaticColors;
		int nUsableColors;
		int i;

		//*** Get the static colors from the system palette
		nStaticColors = GetDeviceCaps (hdc, NUMCOLORS);
		GetSystemPaletteEntries (hdc, 0, 256, lp->palPalEntry);

		//*** Set the peFlags of the lower static colors to zero
		nStaticColors = nStaticColors / 2;
		for (i=0; i<nStaticColors; i++)
			lp->palPalEntry[i].peFlags = 0;

		//*** Fill in the entries from the given color table
		nUsableColors = 256 - nStaticColors;
		for (; i<nUsableColors; i++)
		{
			lp->palPalEntry[i].peRed = PalDibInfo->bmiColors[i].rgbRed;
			lp->palPalEntry[i].peGreen = PalDibInfo->bmiColors[i].rgbGreen;
			lp->palPalEntry[i].peBlue = PalDibInfo->bmiColors[i].rgbBlue;
			lp->palPalEntry[i].peFlags = PC_NOCOLLAPSE;
		}

		//*** Set the peFlags of the upper static colors to zero
		for (i = 256 - nStaticColors; i<256; i++)
			lp->palPalEntry[i].peFlags = 0;
	}

	VERIFY(ghPalette.CreatePalette(lp));

#else USE_HALFTONE_PALETTE

	CDC cdc;

	cdc.Attach(hdc);

	VERIFY(ghPalette.CreateHalftonePalette(&cdc));
	// Work around to have proper palette entries on NT
	CPalette	*phOldPal = cdc.SelectPalette(&ghPalette, FALSE);
	cdc.RealizePalette();
	GetSystemPaletteEntries (hdc, 0, 256, lp->palPalEntry);

	if (phOldPal)
		cdc.SelectPalette(phOldPal, TRUE);

//	lp->palNumEntries = ghPalette.GetPaletteEntries(0,256,lp->palPalEntry);
//	lp->palNumEntries = 256;	// Hack
	cdc.Detach();
#endif USE_HALFTONE_PALETTE

	//*** Remember to release the DC!
	::ReleaseDC (NULL, hdc);
#endif //NOGLOBPAL
    }  // if ghPalette.m_hObject

	ASSERT( GetParentFrame());
	// since we are the splitters parent the splitter
	// doesn't set the parent frames style, so we have to.
	GetParentFrame()->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);

	// Create the comic view
	if(theApp.m_bComicView)
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
	CChatDoc *pDoc = GetDocument();
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

	pDoc->m_view = (CView *) m_wndLSplitter->GetPane(0, 0);	// save windows for easy ref.
	pDoc->m_sayWnd = m_wndLSplitter->GetPane(1, 0);
	pDoc->m_client = this;

	pDoc->m_bodyCam = m_wndRSplitter->GetPane(1, 0);	// save for easy reference
	pDoc->m_memberList = m_wndRSplitter->GetPane(0, 0);
	((CMemberList *)(pDoc->m_memberList))->m_pDoc = pDoc;  // memberlist wants to know its doc

	ImmAssociateContext(((CBodyCam *)GetDocument()->m_bodyCam)->m_hWnd, NULL);
	ImmAssociateContext(((CMemberList *)GetDocument()->m_memberList)->m_MemberListBox.m_hWnd, NULL);
	ImmAssociateContext(GetDocument()->m_view->m_hWnd, NULL);

	if (GetDocument()->m_bLastMemberView)
		pDoc->OnViewIcon();							//  members back to icons
	else
		pDoc->OnViewListAux();						// or members back to list
	
	if (doUpdate) {
		// Need to make comic view active, so it's the comic view's OnInitialUpdate that will be called...
		CView *childView = GetDocument()->m_view;
//		((CFrameWnd *)AfxGetMainWnd())->SetActiveView(childView);  -- causes MDI problems though!

		// we should force a new OnInitialUpdate since we arent recreating the chatview
		CFrameWnd *pFrame = childView->GetParentFrame();
		pFrame->InitialUpdateFrame(pDoc ,TRUE);
		
		// we have to force a resize since we arent really recreating the chatview.
		CRect rect;
		GetClientRect(&rect);
		OnSize(SIZE_RESTORED,rect.right,rect.bottom);
	}

	pDoc->m_bComicView = TRUE;

	// REGISB: added 08/29/97 - Setting toolbar info for CRtfCtrl object
	CToolBar*		pTB = (CToolBar*) cui.GetToolBarPv();
	CToolBarCtrl*	pTBCtrl;

	ASSERT(pTB);

	pTBCtrl = &(pTB->GetToolBarCtrl());

	ASSERT(pTBCtrl);

	((CSayWnd*) pDoc->m_sayWnd)->SetFormattingToolBarInfo(pTBCtrl, ID_SWITCHBOLD, ID_SWITCHITALIC, ID_SWITCHUNDERLINED, 
																   ID_SWITCHFIXEDPITCH, ID_SWITCHSYMBOL);
}

// doUpdate should be false, when the view is initially created in place, true otherwise
void CChatView::CreateTextView(BOOL doUpdate)
{
	CChatDoc *pDoc = GetDocument();
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
	pDoc->m_memberList = m_wndSplitter->GetPane(0, 1);
	((CMemberList *)(pDoc->m_memberList))->m_pDoc = pDoc;  // memberlist wants to know its doc
	// finally the left hand side horizontal splitter
	m_wndLSplitter->CreateStatic(m_wndSplitter, 2, 1,WS_CHILD | WS_VISIBLE | WS_BORDER,
		m_wndSplitter->IdFromRowCol(0, 0));


	m_wndLSplitter->CreateView(0,0,RUNTIME_CLASS( CTextView ), CSize(200,200), pContext);

	m_wndLSplitter->CreateView(1,0,RUNTIME_CLASS( CSayWnd ), CSize(0,40), pContext);

	pDoc->m_view = (CView *) m_wndLSplitter->GetPane(0, 0);	// save windows for easy ref.
	pDoc->m_sayWnd = m_wndLSplitter->GetPane(1, 0);
	pDoc->m_client = this;

	ImmAssociateContext(((CMemberList *)GetDocument()->m_memberList)->m_MemberListBox.m_hWnd, NULL);
//	ImmAssociateContext(GetView()->m_hWnd, NULL);

	GetDocument()->m_bLastMemberView = GetDocument()->m_bIconMembers;

	GetDocument()->OnViewListAux();							// make sure members not displayed as icons

	if (doUpdate) {
		// Need to make text view active, so it's the text view's OnInitialUpdate that will be called...
		CView *childView = GetTextView(); // testing...
//		((CFrameWnd *)AfxGetMainWnd())->SetActiveView(childView) -- causes MDI problems though;

		CFrameWnd *pFrame = childView->GetParentFrame();
		// we should force a new OnInitialUpdate since we arent recreating the chatview
		pFrame->InitialUpdateFrame(pDoc, TRUE);

		// we have to force a resize since we arent really recreating the chatview.
		CRect rect;
		GetClientRect(&rect);
		OnSize(SIZE_RESTORED,rect.right,rect.bottom);
	}

	GetDocument()->m_bComicView = FALSE;

	// REGISB: added 08/29/97 - Setting toolbar info for CRtfCtrl object
	CToolBar*		pTB = (CToolBar*) cui.GetToolBarPv();
	CToolBarCtrl*	pTBCtrl;

	ASSERT(pTB);

	pTBCtrl = &(pTB->GetToolBarCtrl());

	ASSERT(pTBCtrl);

	((CSayWnd*) pDoc->m_sayWnd)->SetFormattingToolBarInfo(pTBCtrl, ID_SWITCHBOLD, ID_SWITCHITALIC, ID_SWITCHUNDERLINED, 
																   ID_SWITCHFIXEDPITCH, ID_SWITCHSYMBOL);
}

void CChatView::CleanUpBeforeChangeView()
{
	CChatDoc *doc = GetDocument();
	CFrameWnd *f = ((CFrameWnd *)AfxGetMainWnd());
	CView *v = f->GetActiveView();

	doc->m_view = NULL;		// no client view (temporarily)
	((CFrameWnd *)AfxGetMainWnd())->SetActiveView(NULL);  // active view must *always* be valid view or NULL
	CFrameWnd *origParent = (CFrameWnd *) (GetDocument()->GetOrigParent());
	if (origParent) origParent->SetActiveView(NULL);

	m_wndSplitter->DestroyWindow();
	doc->m_sayWnd = NULL; // must be null on creation of textview (InitializeTextCore) xxx

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
	GetDocument()->SetFocusToSayWnd();
}

extern CUserInfo *GetSingleSelectedMember();

void CChatView::OnGetinfo() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui) GetDocument()->m_proto->ChatGetInfo(pui);	
}

void CChatView::OnIgnore() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (!pui) return;
	GetDefaultProto()->DoIgnoreUser(pui, !pui->Ignored(), FALSE);	
}

#if 0
void CChatView::OnRing() 
{
	void ChatRingUser(CUserInfo *);
	CUserInfo *pui = GetSingleSelectedMember();
	ChatRingUser(pui);
}
#endif


void CChatView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	if (bActivate) {
		GetDocument()->LoadDocData();
		// TRACE("Calling OnActivateView: doc = %s (%d) (%u)\n", currentRoom->strChannel, bActivate, pActivateView);
	}

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if (bActivate) GetDocument()->SetFocusToSayWnd();
}

LRESULT CChatView::OnLoginDlg(WPARAM wParam, LPARAM lParam)
{
	if (!theApp.m_bDoCB32) {
		if (GetDocument()->m_fileType == FT_CCR)
			VERIFY(ChatInitialize());   // initialize protocol connection
	} else {
		void GetNewNmProto();
		GetNewNmProto();
	}

	// Insert the macro menu -- really shouldn't be here, but necessary to do
	// this somewhat after InitMyDocument, or there is no effect
	if (!theApp.m_pmenuMacro) {
		GetDocument()->InsertMacroMenu();   // Insert and fill the macro menu
		GetDocument()->UpdateMacroMenu();
	}

	return 0;
}

