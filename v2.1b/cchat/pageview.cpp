// PageView.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"

#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "dib.h"
#include "tabbar.h"
#include "mainfrm.h"

#include "ui.h"
#include "saywnd.h"
#include "traj.h"
#include "spline.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "bodycam.h"
#include "balloon.h"
#include "backdrop.h"
#include "PageView.h"
#include "panel.h"
#include "histent.h"
#include "memblst.h"
#include "format.h"

#include <math.h>
#include <winnls.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Convenience macros

#define pMainFrame	((CMainFrame *) theApp.m_pMainWnd)

/////////////////////////////////////////////////////////////////////////////
// The one and only CClientApp object

extern CChatApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CPageView

IMPLEMENT_DYNCREATE(CPageView, CScrollView)

CPageView::CPageView()
{
//	srand(1879);  GGOD
//	srand(1580);  // SAVE THIS FOR MAIN!
//	srand(2898);  // PRETTY GOOD! FOR MAIN
//	srand(56);	// ALSO GOOD
//	srand(45);   // ALSO GOOD
//	srand(35);  // best so far
//	srand(581);  // for video
//	srand(1111);
	m_retDib = NULL;	// allocated later in CreateRetainedPanel
	m_retSec = NULL;
	m_printRetDib = NULL;
	m_printRetSec = NULL;
	m_palette = NULL;
	m_bFirstTime = TRUE;	// the first time this is checked it will be the first time through
	m_bAtBottom = TRUE;
	m_bbox.SetRectEmpty(); // init view size
}

CPageView::~CPageView()
{
	FreeRetainedPanelS();
	FreeRetainedPanelP();

#ifdef NOGLOBPAL
	if (m_palette) 
		delete m_palette;
#endif NOGLOBPAL

	GetDocument()->m_view = NULL;		// gone!

	theApp.SaveToReg();
}


BEGIN_MESSAGE_MAP(CPageView, CScrollView)
	//{{AFX_MSG_MAP(CPageView)
	ON_WM_SETFOCUS()
	ON_WM_CHAR()
	ON_WM_MOUSEMOVE()
	ON_WM_CONTEXTMENU()
	ON_WM_LBUTTONDOWN()
	ON_COMMAND(ID_CANCEL_EDIT_SRVR, OnCancelEditSrvr)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	ON_WM_VSCROLL()
	ON_WM_RBUTTONDOWN()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPageView drawing

CPalette *CPageView::InstallPalette() 
{
#ifdef NOGLOBPAL
	LOGPALETTE *lp = (LOGPALETTE *) malloc(sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 255);

	lp->palVersion = 0x300;
	lp->palNumEntries = 256;
	for (int i = 0; i < 256; i++) {
		if (i != LINKINDEX) {
			lp->palPalEntry[i].peRed = i;
			lp->palPalEntry[i].peGreen = i;
			lp->palPalEntry[i].peBlue = i;
		} else {
			lp->palPalEntry[i].peRed = GetRValue(linkColor);
			lp->palPalEntry[i].peGreen = GetGValue(linkColor);
			lp->palPalEntry[i].peBlue = GetBValue(linkColor);
		}
		lp->palPalEntry[i].peFlags = 0;
	}

	ASSERT(!m_palette);  // why should we ever reinstall?
	m_palette = new CPalette();
	VERIFY(m_palette->CreatePalette(lp));
	free(lp);
#else NOGLOBPAL
	m_palette = &ghPalette;
#endif NOGLOBPAL
	return (m_palette);
}

void CreateRetainedPanel(CDC *pDC, HBITMAP *retSec, CDIB **retDib) {
	void CreateRetainedBitmap(CDC *, HBITMAP *, CDIB **, int, int);

	double inchesWide = (double) CUnitPanelPage::m_unitWidth / UNITSPERINCH;
	double inchesHigh = (double) CUnitPanelPage::m_unitHeight / UNITSPERINCH;

	CPoint dpi(UNITSPERINCH, -UNITSPERINCH); // rely on windows to do rounding here
	int oldMode = pDC->SetMapMode(MM_TWIPS);
	pDC->LPtoDP(&dpi);
	pDC->SetMapMode(oldMode);
	int width = (int) ceil(inchesWide * dpi.x);
	int height = (int) ceil(inchesHigh * dpi.y);
	CreateRetainedBitmap(pDC, retSec, retDib, width, height);
}

int PointsToTwips(int points) {
	 // CPageView::m_dpiX not cached yet :-( so compute real logPixelsY
	CClientDC dc(CWnd::GetDesktopWindow());
	dc.SetMapMode(MM_TWIPS);
	CPoint pt(UNITSPERINCH, -UNITSPERINCH);
	dc.LPtoDP(&pt);
	int logPixelsY = pt.y;
	int negHeight = (int) (-points * dc.GetDeviceCaps(LOGPIXELSY) / 72.0); // for some reason, this conversion requires false LOGPIXELSY, not real m_dpiY
	return ((int)((negHeight * 1440.0) / logPixelsY - 0.5));
}



BOOL bRET = FALSE;

void CPageView::OnDraw(CDC* pDC)
{
	CPalette *oldPal;
	BOOL bbox_overlap(RECT *, RECT *);
	CChatDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

//	CView::OnDraw(pDC);

	RECT rectPage, rectClip;
	pDC->GetClipBox(&rectClip);
	if (pDC->IsPrinting()) 
		AddPrintOffset(&rectClip, FALSE);
	else 
	{
		AddScrollOffset(&rectClip, FALSE, TRUE);
		// AddConversionDelta is necessary since we are switching back and forth between
		//   logical and physical scales, and otherwise we get an off by one refresh bug.
		AddConversionDelta(rectClip);
	}

	// Use our own palette, Moved above CreateRetainedPanel - RamuM
#ifdef NOGLOBPAL
	if (oldPal = pDC->SelectPalette(GetPalette(pDC), TRUE))
#else NOGLOBAPAL
	if (oldPal = pDC->SelectPalette(&ghPalette, TRUE))
#endif NOGLOBPAL
        pDC->RealizePalette();


	// Make sure a retained section is allocated, if we're drawing to the display.
	// This will only be done on the first Draw.
	// If we are printing, it was already allocated in OnBeginPrinting
	if (!GetRetSec(pDC) && !pDC->IsPrinting())
		CreateRetainedPanel(pDC, &m_retSec, &m_retDib);

	POSITION pagePos = pDoc->m_pages.GetHeadPosition();
	while (pagePos != NULL)	
	{							// for each page
		CPage *page = (CPage *) pDoc->m_pages.GetNext(pagePos);
		page->GetBBox(&rectPage);
		if (rectPage.top < rectClip.bottom)	// return if clip region appears before page
			break;
		if (bbox_overlap(&rectPage, &rectClip))
			page->Draw(this, pDC, NULL, &rectClip);
	}

	// Restore palette
	if (oldPal)
		pDC->SelectPalette(oldPal, TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CPageView diagnostics

#ifdef _DEBUG
void CPageView::AssertValid() const
{
	CScrollView::AssertValid();
}

void CPageView::Dump(CDumpContext& dc) const
{
	CScrollView::Dump(dc);
}

CChatDoc* CPageView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChatDoc)));
	return (CChatDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CPageView message handlers

void CPageView::OnSetFocus(CWnd* pOldWnd) 
{
}


BOOL CPageView::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style |= WS_CLIPSIBLINGS;
	cs.style &= ~WS_BORDER;
	
	return CScrollView::PreCreateWindow(cs);
}

void UpdateViewsX(CView* pSender, LPARAM lHint = 0L, CObject* pHint = NULL, BOOL scrollToView = FALSE) {
	// GetView()->GetDocument()->UpdateAllViews(pSender, lHint, pHint); // there's got to be a more efficient accessor!
	// Won't do an UpdateAllViews now for efficiency, since CChatView is technically also a
	// view. Instead, send the message directly to our sender view.
	((CPageView *)pSender)->OnUpdate(pSender, lHint, pHint);
}


// Hack, to make retrieving ClientBitmap easier
CPalette* GetPalette(CDC *dc) {
	return(GetView()->GetPalette(dc));
}


#include <imm.h>

void CPageView::OnInitialUpdate() 
{
//	m_bbox.SetRectEmpty(); // reset view size
//	SetScrollSizes(MM_TWIPS, CSize(1, 1), CSize(0, 0), CSize(0,0));

	CScrollView::OnInitialUpdate();
	if (!m_bFirstTime)
		GetDocument()->m_client->PostMessage(WM_LOGINDLG,0,0);  // Not the first time activated so we can login from here
}

#if 0
// This function was implemented so that we could better regulate when the login dialog is 
// brought up.  For instance, bringing it up on OnInitialUpdate is too soon for IE3.0 the first time
// so it must be brought up in OnActivateView the first time only, and then afterwards in 
// OnInitialUpdate.
LRESULT CPageView::OnLoginDlg(WPARAM wParam, LPARAM lParam)
{
	CChatDoc* pDoc = GetDocument();
	if (pDoc->m_fileType == FT_CCR)
		VERIFY(ChatInitialize());   // initialize protocol connection
	return 0;
}
#endif

int d_cnt = 0;

void CPageView::OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint ) {
	if (pHint != NULL) {
		if (pHint->IsKindOf(RUNTIME_CLASS(CDamage))) {
			RECT logPanel;
			// The hint is that a stroke has been added (or changed).
			// So, invalidate its rectangle.
			CDamage *d = (CDamage *)pHint;
			logPanel = d->m_g;
			LPtoDP(&d->m_g);
			BOOL bNotTitlePanel = (d->m_g.left || d->m_g.top);
			m_bbox.UnionRect(&m_bbox, &d->m_g);							// update bbox
			if (!theApp.m_bNoRefresh) {
				AddScrollOffset(&d->m_g, TRUE, FALSE);
				InvalidateRect(&d->m_g, FALSE);							// invalidate damage
				if (bNotTitlePanel) UpdateScroll(&logPanel);
			}
			return;
		}
	}
	// We can't interpret the hint, so assume that anything might
	// have been updated.
	if (!theApp.m_bNoRefresh) Invalidate();
	return;
}

BOOL CPageView::AtBottom() {
	RECT r;
	int yMax = GetScrollLimit(SB_VERT);
	int curY = GetScrollPos(SB_VERT);
	GetClientRect(&r);
	// typically yMax == curY if you're scrolled to the bottom.  An exception to this is
	// if you've got a horizontal scroll bar and no vertical (Why?)  Then for some
	// reason, yMax > curY.  So we have a second term that tests for the situation
	// where there's no vertical scrollbar... if this is the case, you're always at the bottom.
	return (yMax == curY || (curY == 0 && r.bottom >= yMax));
}

void CPageView::UpdateScroll(RECT *logPanel, BOOL onlyAtBottom) {
//			TRACE("(%d) DAMAGE = %d %d to %d %d.\n", d_cnt++, d->m_g.left, d->m_g.bottom, d->m_g.right, d->m_g.top);
//			TRACE("UNION = %d %d to %d %d.\n", m_bbox.left, m_bbox.bottom, m_bbox.right, m_bbox.top);
			// Scroll extents must be in logical coords -- so map diagonal back
	BOOL doScroll = !onlyAtBottom || m_bAtBottom;
	CPoint extent(m_bbox.right, m_bbox.bottom);
	DPtoLP(&extent);
	CSize sizePage = CUnitPanelPage::GetScrollPage();
	SetScrollSizes(MM_TWIPS, CSize(extent.x, -extent.y),
				   sizePage, CSize(0,0));
//	TRACE("Extent = %d, %d.\n", extent.x, -extent.y);
//	CPoint pt = GetDeviceScrollPosition();
//	TRACE("DEVICE SCROLL = %d %d.\n", pt.x, pt.y);
//	CPoint pt = GetScrollPosition();
//	TRACE("LOGICAL SCROLL = %d %d.\n", pt.x, pt.y);
			
	// for now, scroll to bottom if requested only if at bottom
	if (logPanel && doScroll) {
		RECT clientRect;
		GetClientRect(&clientRect);
		DPtoLP(&clientRect);
//		TRACE("Logical clientRect = %d %d\n", clientRect.top, clientRect.bottom);
		CPoint ul(clientRect.left, clientRect.top);
		if (logPanel->top > clientRect.top)
			ul.y = logPanel->top;
		else if (logPanel->bottom < clientRect.bottom)
			ul.y -= clientRect.bottom - logPanel->bottom;
		if (logPanel->right > clientRect.right)
			ul.x += logPanel->right - clientRect.right;
		else if (logPanel->left < clientRect.left)
			ul.x = logPanel->left;
		if (ul.x != clientRect.left || ul.y != clientRect.top) {
			ScrollToPosition(ul);
//			UpdateWindow();
		}
	}
}



void CPageView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{

	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	if(m_bFirstTime && bActivate) // REGISB: added " && bActivate"
	{	// This is the first time the view is dispayed, so we must be initialized and 
		// time to show the dialog
		GetDocument()->m_client->PostMessage(WM_LOGINDLG,0,0);
		m_bFirstTime = FALSE;

		// if panel width never set (new user!) do something reasonable for current display
		if (CUnitPanelPage::m_unitWidth < MINUNITPANELWIDTH) SetPanelsWide(2);  // force reasonable panel size
	}

	if (bActivate) GetDocument()->SetFocusToSayWnd();
}		


void CPageView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
    // switch focus to say window and send it the character
	if (nChar < 11) GetBodyCam()->SendMessage(WM_CHAR, nChar, nFlags);
	else ForwardToSayWnd(nChar);
}

/////////////////////////////////////////////////////////////////////////////
// OLE Server support

// The following command handler provides the standard keyboard
//  user interface to cancel an in-place editing session.  Here,
//  the server (not the container) causes the deactivation.
void CPageView::OnCancelEditSrvr()
{
	GetDocument()->OnDeactivateUI(FALSE);
}


/////////////////////////////////////////////////////////////////////////////
// CPageView printing

#define FOOTERMARGIN		1000   // (printable) space to leave on either side of footer
#define FOOTERFONTHEIGHT	250	   // height of footer font (in TWIPS)
#define FOOTERHEIGHT		(FOOTERFONTHEIGHT * 3 / 2)	// vertical space reserved for footer
#define TIMEDATESEP			"   "

void CPageView::OnPrepareDC(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	ASSERT(m_nMapMode > 0);
	pDC->SetMapMode(m_nMapMode);
	CView::OnPrepareDC(pDC, pInfo);  // heh, for now just try CView
	
	if (pInfo) {		// i.e., printing...
		unsigned int physPages, pagesSoFar = 0, pagesBeforeThis = 0;
		CChatDoc* pDoc = GetDocument();
		pInfo->m_bContinuePrinting = FALSE;			// unless we find a page

		// Loop through the pages, find the page that needs to be printed on this
		//  physical page, and ask the page to set up the DC appropriately.
		//  We should eventually change this so we don't have to start from the first
		//  CPage w/ each OnPrepareDC.

		// set up drawing rect to entire page (in logical coordinates)
		// note that this isn't done by default until the second time OnPrepareDC is called
		// per page by the framework  :-(.
		pInfo->m_rectDraw.SetRect(0, 0, pDC->GetDeviceCaps(HORZRES), pDC->GetDeviceCaps(VERTRES));
		pDC->DPtoLP(&pInfo->m_rectDraw);
		pInfo->m_rectDraw.bottom += FOOTERHEIGHT;

		POSITION pos = pDoc->m_pages.GetHeadPosition();
		while (pos) {
			CPage *page = (CPage *) pDoc->m_pages.GetNext(pos);
			physPages = page->GetPhysicalPageCount(pInfo);
			pagesSoFar += physPages;
			if (pInfo->m_nCurPage <= pagesSoFar) {
				page->PreparePrintDC(pDC, pInfo, pInfo->m_nCurPage - pagesBeforeThis);
				pInfo->m_bContinuePrinting = TRUE;
				break;
			}
			pagesBeforeThis += physPages;
		}
	}
}

void CPageView::PrintFooter(CDC *pDC, CPrintInfo *pInfo) {
	char buff[60];
	SYSTEMTIME st;
	CRect *pbox = &pInfo->m_rectDraw;			// for easy reference
	int y = pbox->bottom + 1;					// the y at which to draw the footer												// +1 necessary so text not clipped (why?)

	CFont *oldFont = pDC->SelectObject(m_footerFont);

	// first output title
	int oldAlign = pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
	pDC->TextOut(pbox->left, y, "Microsoft Chat");

	// next output page num
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	CString strFormat;
	strFormat.LoadString(IDS_PAGEFOOTER);
	sprintf(buff, "%u", pInfo->m_nCurPage);  // should use the m_strPageDesc field
	VERIFY(ReplaceToken(strFormat, CString("%1"), buff));	
	pDC->TextOut((pbox->left + pbox->right)/2, y, strFormat);

	// finally output date/time
	GetLocalTime(&st);
	GetTimeFormat(LOCALE_USER_DEFAULT, NULL, &st, NULL, buff, sizeof(buff) - strlen(TIMEDATESEP));
	strcat(buff, TIMEDATESEP);
	int nchars = strlen(buff);
	GetDateFormat(LOCALE_USER_DEFAULT, NULL, &st, NULL, /*"MMM d, yy",*/ buff+nchars, sizeof(buff)-nchars);
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
	pDC->TextOut(pbox->right, y, buff);

	pDC->SetTextAlign(oldAlign);
	pDC->SelectObject(oldFont);
}


void CPageView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	OnDraw(pDC);

	// Give m_rectDraw the left and right border of the comics page, so footers 
	//   do not extend beyond the margins.
//	RECT clipBox;
//	pDC->GetClipBox(&clipBox);
//	TRACE("RectDraw first is left = %d, right = %d.\n", pInfo->m_rectDraw.left, pInfo->m_rectDraw.right);
//	pInfo->m_rectDraw.left = clipBox.left;
//	pInfo->m_rectDraw.right = clipBox.right;
//	TRACE("ClipBox:Left = %d, right = %d.\n", clipBox.left, clipBox.right);

	// We set a clip region in OnPrepareDC to make sure that only the desired panels were
	//   printed on a page.  Since we want to put the footer below these panels, it's
	//   necessary to clear the clip region.
	pInfo->m_rectDraw.left += FOOTERMARGIN;
	pInfo->m_rectDraw.right -= FOOTERMARGIN;
	// FOOTERHEIGHT was taken away in OnPreparePrintDC, so panels wouldn't collide w/ footer.
	//    Put it back here.
	pInfo->m_rectDraw.bottom -= FOOTERHEIGHT;
	pDC->SelectClipRgn(NULL, RGN_COPY);
	PrintFooter(pDC, pInfo);
}

void CPageView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CScrollView::OnBeginPrinting(pDC, pInfo);

	// Allocate the footer font (deallocated in OnEndPrinting)
	m_footerFont = new CFont();
	LOGFONT lf = theApp.m_comicsFont;
	lf.lfHeight = FOOTERFONTHEIGHT;
	lf.lfWeight = FW_REGULAR;
	VERIFY(m_footerFont->CreateFontIndirect(&lf));

	// Temporarily create a retained section and bitmap suitable for printer CDC
	if (!m_printRetSec)
		CreateRetainedPanel(pDC, &m_printRetSec, &m_printRetDib);	// make sure a retained section was allocated
}

void CPageView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TODO: Add your specialized code here and/or call the base class
	
	CScrollView::OnEndPrinting(pDC, pInfo);

	// Free up footer font
	delete m_footerFont;
	m_footerFont = NULL;

	// Free up CDC retained sections and bitmaps
	if (m_printRetDib) delete m_printRetDib;
	m_printRetDib = NULL;
	if (m_printRetSec) ::DeleteObject(m_printRetSec);
	m_printRetSec = 0;

	// Free up print caches
	void FlushBackDropCache(BOOL);
	FlushBackDropCache(FALSE);
}


BOOL CPageView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	int nPhysPages = 0;
	CChatDoc* pDoc = GetDocument();

#if 0
	POSITION pos = pDoc->m_pages.GetHeadPosition();
	while (pos) {
		CPage *page = (CPage *) pDoc->m_pages.GetNext(pos);
		nPhysPages += page->GetPhysicalPageCount(pInfo);
	}
	pInfo->SetMaxPage(nPhysPages);
#endif
	
	BOOL bRet = DoPreparePrinting (pInfo);	  // default preparation
//	pInfo->m_nNumPreviewPages = 2;		//Preview 2 pages at a time
	// Set this value after calling DoPreparePrinting to override
	// value read from .INI file
	return bRet;
}



// Need only be called once for each CPageView.  Sets the DPI Conversion necessary
// for scrolling.
void CPageView::InitializeDPI(CDC *dc) {
	POINT pt;
	pt.x = pt.y = UNITSPERINCH*10;
	dc->SetMapMode(MM_TWIPS);
	dc->LPtoDP(&pt);
	m_dpiConvx = ((FLOAT) UNITSPERINCH*10) / ((FLOAT) pt.x);
	m_dpiConvy = ((FLOAT) UNITSPERINCH*10) / ((FLOAT) -pt.y);

	CPoint pt2(UNITSPERINCH, -UNITSPERINCH); // rely on windows to do rounding here
	dc->LPtoDP(&pt2);
	m_dpiX = pt2.x;
	m_dpiY = pt2.y;
}



void CPageView::OnMouseMove(UINT nFlags, CPoint point) 
{
	CScrollView::OnMouseMove(nFlags, point);   // what does this do?
}

int CPageView::OnToolHitTest( CPoint point, TOOLINFO* pTI ) const {
#ifdef IDEDJK  // for djk's interactive compiles
	return -1;
#endif

	const char *mesg;

	// CWnd::OnToolHitTest expects the tool window to be a child of the window to
	//   which the ToolTipCtrl is attached.  Since that's not the case here, we'll
	//   override the OnToolHitTest method to return a TOOLINFO referring to the
	//   CPageView.  -djk
	pTI->hwnd = m_hWnd;
	pTI->uId = (UINT)m_hWnd;
	pTI->uFlags |= (TTF_IDISHWND|TTF_NOTBUTTON);
	pTI->lpszText = NULL;
	unsigned int avatarID = ((CPageView *) this)->FindAvatarUnderPoint(point); // how to properly deal with const this?
	if (avatarID) {
		CAvatarX *av = GetAvatar(avatarID);
		av->GetScreenName(&mesg);
		pTI->lpszText = strdup(mesg);  // system frees this in CWnd::FilterToolTipMessage()
		return avatarID;
	}
	else return -1;  // no hit
}

unsigned int CPageView::FindAvatarUnderPoint(POINT point) {
	// Find which character the mouse is over
	// For now, iterate over pages.  Eventually, we'll have a special list for visible pages
	// Give panels a bbox and fix this code!!! XXX
	RECT panelBBox;
	CChatDoc* pDoc = GetDocument();

	DPtoLP(&point);
	AddScrollOffset(&point, FALSE, TRUE);

	POSITION pagePos = pDoc->m_pages.GetHeadPosition();
	while (pagePos) {
		CPage *page = (CPage *) pDoc->m_pages.GetNext(pagePos);
		if (1) {   // eventually should be if (inside_bbox(&point, &page->m_bbox)) {
			POSITION panelPos = page->m_panels.GetHeadPosition();
			int pNum = 0;
			while (panelPos) {
				CPanel *panel = (CPanel *) page->m_panels.GetNext(panelPos);
				int rowNum = pNum / CUnitPanelPage::m_panelsPerRow;
				int colNum = pNum % CUnitPanelPage::m_panelsPerRow;
				int ulx = colNum * (CUnitPanelPage::m_unitWidth + CUnitPanelPage::m_vInterstice);
				int uly = -rowNum * (CUnitPanelPage::m_unitHeight + CUnitPanelPage::m_hInterstice);
				SetRect(&panelBBox, ulx, uly, ulx + CUnitPanelPage::m_unitWidth, uly - CUnitPanelPage::m_unitHeight);
				pNum++;
				if (inside_bbox(&point, &panelBBox)) {
					CPoint panelPt = point;			// put panelPt in panel coords
					panelPt.x -= panelBBox.left;
					panelPt.y -= panelBBox.top;
					POSITION bodyPos = panel->m_bodies.GetHeadPosition();
					while (bodyPos) {
						CBody *body = (CBody *) panel->m_bodies.GetNext(bodyPos);
						if (inside_bbox(&panelPt, &body->m_bbox))
							return(body->m_avatarID);
					}
				}
			}
		}
	}
	return 0;		// No bodies found
}

void *CPageView::FindLabelUnderPoint(POINT point, POINT &panelPt) {
	// Find which character the mouse is over
	// For now, iterate over pages.  Eventually, we'll have a special list for visible pages
	// Give panels a bbox and fix this code!!! XXX
	RECT panelBBox;
	CChatDoc* pDoc = GetDocument();

	DPtoLP(&point);
	AddScrollOffset(&point, FALSE, TRUE);

	POSITION pagePos = pDoc->m_pages.GetHeadPosition();
	while (pagePos) {
		CPage *page = (CPage *) pDoc->m_pages.GetNext(pagePos);
		if (1) {   // eventually should be if (inside_bbox(&point, &page->m_bbox)) {
			POSITION panelPos = page->m_panels.GetHeadPosition();
			int pNum = 0;
			while (panelPos) {
				CPanel *panel = (CPanel *) page->m_panels.GetNext(panelPos);
				int rowNum = pNum / CUnitPanelPage::m_panelsPerRow;
				int colNum = pNum % CUnitPanelPage::m_panelsPerRow;
				int ulx = colNum * (CUnitPanelPage::m_unitWidth + CUnitPanelPage::m_vInterstice);
				int uly = -rowNum * (CUnitPanelPage::m_unitHeight + CUnitPanelPage::m_hInterstice);
				SetRect(&panelBBox, ulx, uly, ulx + CUnitPanelPage::m_unitWidth, uly - CUnitPanelPage::m_unitHeight);
				pNum++;
				if (inside_bbox(&point, &panelBBox)) {
					panelPt = point;			// put panelPt in panel coords
					panelPt.x -= panelBBox.left;
					panelPt.y -= panelBBox.top;
					POSITION elPos = panel->m_elements.GetHeadPosition();
					while (elPos) {
						CLabel *label = (CLabel *) panel->m_elements.GetNext(elPos);
						if (inside_bbox(&panelPt, &label->m_bbox))
							return(label);
					}
				}
			}
		}
	}
	return 0;		// No bodies found
}


//USHORT mousedAvatarID;			// id of avatar for whom the context menu was launched
CUserInfo *mousedPui = NULL;		// last user for whom the context menu was launched
extern CUserInfo* puiSelf;
void CPageView::OnContextMenu(CWnd* pWnd, CPoint screenPoint) {
	CMenu menu;
	POINT clientPoint = screenPoint;
	ScreenToClient(&clientPoint);
	USHORT mousedAvatarID = FindAvatarUnderPoint(clientPoint);
	if (mousedAvatarID) {
		mousedPui = (CUserInfo *)(GetAvatar(mousedAvatarID)->m_userInfo);
		void ShowMemberContext(int x, int y);
		ShowMemberContext(screenPoint.x, screenPoint.y);
	} else {
		if (screenPoint.x == -1 && screenPoint.y == -1) {  // then we must be invoking menu with Shift+F10
			CRect rect; // so lets set up our own point
			GetClientRect(&rect);
			screenPoint.x = (rect.right - rect.left)/2;
			screenPoint.y = (rect.bottom - rect.top)/2;
			ClientToScreen(&screenPoint);
		}
		menu.LoadMenu(theApp.m_bDoCB32 ? IDR_VIEWCONTEXT_NM : IDR_VIEWCONTEXT);
		menu.GetSubMenu(0)->CheckMenuRadioItem(0, 1, GetDocument()->m_bComicView ? 0 : 1,
											   MF_BYPOSITION);
		UINT iRmPropEnable = (GetDocument()->GetConnectionStatus() == CX_INCHANNEL ? MF_ENABLED : MF_GRAYED);
		menu.GetSubMenu(0)->EnableMenuItem(ID_CHANNELPROPS, iRmPropEnable | MF_BYCOMMAND);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
											   screenPoint.x, screenPoint.y, (CWnd *)cui.m_pvChatView);
	}
}


void CPageView::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT avID = FindAvatarUnderPoint(point);
	if (!avID) {	// Second, try a label/URL
		POINT panelPt;
		CLabel *b = (CLabel *) FindLabelUnderPoint(point, panelPt);
		if (b && bURLPresent(b->m_prgdwFormatting))
		{
			b->OnLButtonDown(panelPt);
			return;
		}
	}

	if (puiSelf) {	//either select avatar (or unselect if not clicking over anything else)
		CUserInfo *puiThem = avID ? (CUserInfo *) GetAvatar(avID)->m_userInfo : NULL;
		puiSelf->SelectInMemberList(puiThem, TRUE, (nFlags & (MK_CONTROL | MK_SHIFT)));
		GetMembers()->MakeVisible(puiThem);
	}
}

void CPageView::OnRButtonDown(UINT nFlags, CPoint point) 
{
	// First do avatar selection, as in OnLButtonDown
	UINT avID = FindAvatarUnderPoint(point);

	if (puiSelf) {			//either select avatar (or unselect if not clicking over anything else)
		CUserInfo *puiThem = avID ? (CUserInfo *) GetAvatar(avID)->m_userInfo : NULL;
		puiSelf->SelectInMemberList(puiThem, TRUE, (nFlags & MK_SHIFT));
	}
	
	CScrollView::OnRButtonDown(nFlags, point);   // this will launch context menu?
}


#if 0
void CPageView::OnAvatarcontextTalkto() 
{
	if (theApp.m_bComicView)
		MyAvatar()->SelectTalkTo(mousedPui->GetAvatarID(), FALSE);
}


void CPageView::OnAvatarcontextTalktoalso() 
{
	if (theApp.m_bComicView)
		MyAvatar()->SelectTalkTo(mousedPui->GetAvatarID(), TRUE);
}
#endif

void StartNewPanel() {
	CPage *page = (CPage *) (GetView()->GetDocument()->m_pages.GetTail());
	page->StartNewPanel();
}

BOOL printBMP = FALSE;
#define OUTPATH "..\\..\\BMP Panels\\"
static int outDibCount = 0;

void PrintBMP() {
	char buff[80];
	CDIB *dib = GetView()->m_printRetDib;
	sprintf(buff, "%spanel%d.bmp", OUTPATH, outDibCount++);
	VERIFY(dib->Save(buff));
}

BOOL g_bNewedPanel = FALSE;

BOOL Establishing() {
	CPage *page1 = (CPage *) GetView()->GetDocument()->m_pages.GetHead();
	int count = page1->m_panels.GetCount();
	if (count <= 1 || (!g_bNewedPanel && count <= 2)) return TRUE;
	else return FALSE;
}

void CPageView::SetScrollSizes(int nMapMode, SIZE sizeTotal,
	const SIZE& sizePage, const SIZE& sizeLine)
{
	ASSERT(sizeTotal.cx >= 0 && sizeTotal.cy >= 0);
	ASSERT(nMapMode > 0);
	ASSERT(nMapMode != MM_ISOTROPIC && nMapMode != MM_ANISOTROPIC);

	int nOldMapMode = m_nMapMode;
	m_nMapMode = nMapMode;
	m_totalLog = sizeTotal;

	//BLOCK: convert logical coordinate space to device coordinates
	{
		// total size
		m_totalDev = m_totalLog;
		LPtoDP((LPPOINT)&m_totalDev);
		m_pageDev = sizePage;
		LPtoDP((LPPOINT)&m_pageDev);
		m_lineDev = sizeLine;
		LPtoDP((LPPOINT)&m_lineDev);
		if (m_totalDev.cy < 0)
			m_totalDev.cy = -m_totalDev.cy;
		if (m_pageDev.cy < 0)
			m_pageDev.cy = -m_pageDev.cy;
		if (m_lineDev.cy < 0)
			m_lineDev.cy = -m_lineDev.cy;
	} // release DC here

	// now adjust device specific sizes
	ASSERT(m_totalDev.cx >= 0 && m_totalDev.cy >= 0);
	if (m_pageDev.cx == 0)
		m_pageDev.cx = m_totalDev.cx / 10;
	if (m_pageDev.cy == 0)
		m_pageDev.cy = m_totalDev.cy / 10;
	if (m_lineDev.cx == 0)
		m_lineDev.cx = m_pageDev.cx / 10;
	if (m_lineDev.cy == 0)
		m_lineDev.cy = m_pageDev.cy / 10;

	if (m_hWnd != NULL)
	{
		// window has been created, invalidate now
		UpdateBars();
		if (nOldMapMode != m_nMapMode)
			Invalidate(TRUE);
	}
}

void CPageView::ScrollToPosition(CPoint pt)    // logical coordinates
{
	ASSERT(m_nMapMode > 0);     // not allowed for shrink to fit
	if (m_nMapMode != MM_TEXT)
	{
		LPtoDP((LPPOINT)&pt);		// djk - no longer creates a dc. ClientView handles transformation
	}
	VerifyScrollPosition(pt);
	// now in device coordinates - limit if out of range
	ScrollToDevicePosition(pt);
}

void CPageView::AddScrollOffset(RECT *rect, BOOL mapBack, BOOL logCoords) {
	CPoint sPt = GetDeviceScrollPosition();
	if (logCoords) DPtoLP(&sPt);
	if (!mapBack) {
		rect->left += sPt.x;
		rect->right += sPt.x;
		rect->bottom += sPt.y;
		rect->top += sPt.y;
	} else {
		rect->left -= sPt.x;
		rect->right -= sPt.x;
		rect->bottom -= sPt.y;
		rect->top -= sPt.y;
	}
}

void CPageView::AddScrollOffset(POINT *pt, BOOL mapBack, BOOL logCoords) {
	CPoint sPt = GetDeviceScrollPosition();
	if (logCoords) DPtoLP(&sPt);
	if (!mapBack) {
		pt->x += sPt.x;
		pt->y += sPt.y;
	} else {
		pt->x -= sPt.x;
		pt->y -= sPt.y;
	}
}

void CPageView::AddPrintOffset(RECT *rect, BOOL mapBack) {
	if (!mapBack) {
		rect->left += m_printOffset.x;
		rect->right += m_printOffset.x;
		rect->top += m_printOffset.y;
		rect->bottom += m_printOffset.y;
	} else {
		rect->left -= m_printOffset.x;
		rect->right -= m_printOffset.x;
		rect->top -= m_printOffset.y;
		rect->bottom -= m_printOffset.y;
	}
}

void CPageView::AddPrintOffset(POINT *pt, BOOL mapBack) {
	if (!mapBack) {
		pt->x += m_printOffset.x;
		pt->y += m_printOffset.y;
	} else {
		pt->x -= m_printOffset.x;
		pt->y -= m_printOffset.y;
	}
}


POINT CPageView::AccountForScroll(POINT *loc, BOOL mapBack, BOOL logCoords, BOOL isPrinting) {
	POINT newPt = *loc;
	if (!isPrinting)
		AddScrollOffset(&newPt, mapBack, logCoords);
	else
		AddPrintOffset(&newPt, mapBack);
	return newPt;
}

void SetPrintOffset(int x, int y)
{
	CPageView *view = GetView();
	view->m_printOffset.x = x;
	view->m_printOffset.y = y;
}


void ShowInfoX(void *pui, const char *szInfo)
{
	GetView()->GetDocument()->ShowInfo(pui, szInfo);
}


int CPageView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CScrollView::OnCreate(lpCreateStruct) == -1)
		return -1;

	if (!GetClientDC()) {		// keep a private clientdc for text formatting
		CClientDC *dc = new CClientDC(GetDesktopWindow());
		dc->SetMapMode(MM_TWIPS);
		InitializeDPI(dc);		// must be done before SetScrollSizes
		cui.m_pvClientDC = dc;
		CUnitPanelPage::SetFonts(theApp.m_comicsFont, theApp.m_comicsColor);
	}
//	strcpy(m_toolTipString, "HOWDY!!!");
	VERIFY(m_toolTip.Create(this, TTS_ALWAYSTIP));
	VERIFY(m_toolTip.AddTool(this, m_toolTipString));
	EnableToolTips(TRUE);
	m_toolTip.Activate(TRUE);

	int GetAvatarUpperBound();
	if (GetAvatarUpperBound() == -1)
		InitializeAvatars();   // don't reinitialize, just initialize
	
	return 0;
}

void CPageView::OnDestroy() 
{
	CScrollView::OnDestroy();
}

void CleanupComicsDataOnExit() {
	DestroyAvatars();		// clear the avatar and backdrop stores (housekeeping)

	void DestroyBackDropArt();
	DestroyBackDropArt();
		
	CUnitPanelPage::DestroyFonts();

	// Now reclaim a few additionals.  Should we do this elsewhere?
	void DestroySplineMatrixCaches();
	DestroySplineMatrixCaches();
	
	if (GetClientDC()) {
		delete GetClientDC();
		cui.m_pvClientDC = NULL;	//  not necessary, but clean
	}
}


#if 0
void CPageView::OnAvatarcontextRing() 
{
	void ChatRingUser(CUserInfo *);
	ChatRingUser(mousedPui);	
}
#endif

void UpdateIgnore(CMenu &menu) {
	extern CUserInfo *puiSelf, *GetSingleSelectedMember();
	extern BOOL bNMInstalled();

	CUserInfo *pui = GetSingleSelectedMember();
	int status = currentRoom->GetConnectionStatus();

	int flag = (pui && pui->Ignored()) ? MF_CHECKED : MF_UNCHECKED;
	menu.CheckMenuItem(ID_AVATARCONTEXT_IGNORE, flag | MF_BYCOMMAND);

	flag = (pui && pui != puiSelf) ? MF_ENABLED : MF_GRAYED; // enable on ignored
	menu.EnableMenuItem(ID_AVATARCONTEXT_IGNORE, flag | MF_BYCOMMAND);

	BOOL enabled = (status == CX_INCHANNEL) && pui && pui->IsComicUser();
	flag = enabled ? MF_ENABLED : MF_GRAYED; // enable on get info
	menu.EnableMenuItem(ID_AVATARCONTEXT_GETINFO, flag | MF_BYCOMMAND);
	menu.EnableMenuItem(ID_VISIT_HOMEPAGE, flag | MF_BYCOMMAND);

	enabled = enabled && pui != puiSelf;
	flag = enabled ? MF_ENABLED : MF_GRAYED;
	menu.EnableMenuItem(ID_SEND_EMAIL, flag | MF_BYCOMMAND);
	enabled = !bNMInstalled() || enabled;
	flag = enabled ? MF_ENABLED : MF_GRAYED;
	menu.EnableMenuItem(ID_START_NETMEETING, flag | MF_BYCOMMAND);

	enabled = (status == CX_INCHANNEL) && pui;
	flag = enabled ? MF_ENABLED : MF_GRAYED;
	menu.EnableMenuItem(ID_GETIDENTITY, flag | MF_BYCOMMAND);
	menu.EnableMenuItem(ID_GET_VERSION, flag | MF_BYCOMMAND);
	menu.EnableMenuItem(ID_PING_USER, flag | MF_BYCOMMAND);
	menu.EnableMenuItem(ID_GET_LOCALTIME, flag | MF_BYCOMMAND);
	
	enabled = (status == CX_INCHANNEL) && pui && (pui != puiSelf);
	flag = enabled ? MF_ENABLED : MF_GRAYED;
	menu.EnableMenuItem(ID_WHISPERBOX_MLIST, flag | MF_BYCOMMAND);
	menu.EnableMenuItem(ID_SEND_FILE, flag | MF_BYCOMMAND);

	if (puiSelf && puiSelf->IsOperator()) {
		// do op item enabling
		BOOL preReqs = (pui && pui != puiSelf && (status == CX_INCHANNEL));
		flag = preReqs ? MF_ENABLED : MF_GRAYED;
		menu.EnableMenuItem(ID_ADMINISTRATOR_KICK, flag | MF_BYCOMMAND);

		flag = preReqs ? MF_ENABLED : MF_GRAYED;
		menu.EnableMenuItem(ID_ADMIN_BAN, flag | MF_BYCOMMAND);
		
		flag = preReqs ? MF_ENABLED : MF_GRAYED;
		menu.EnableMenuItem(ID_MAKEADMIN, flag | MF_BYCOMMAND);
	
		flag = preReqs ? MF_ENABLED : MF_GRAYED;
		menu.EnableMenuItem(ID_MAKESPEAKER, flag | MF_BYCOMMAND);

		BOOL isModerated = GetChatDoc()->m_proto->m_dwModes & CM_MODERATED;
		flag = (preReqs && isModerated) ? MF_ENABLED : MF_GRAYED;
		menu.EnableMenuItem(ID_MAKESPECTATOR, flag | MF_BYCOMMAND);

		if (pui) {
			UINT id = 0;
			if (pui->IsOperator()) id = ID_MAKEADMIN;
			else if (pui->IsSpeaker()) id = ID_MAKESPEAKER;
			else if (pui->IsSpectator()) id = ID_MAKESPECTATOR;
			if (id)
				menu.CheckMenuRadioItem(ID_MAKEADMIN, ID_MAKESPECTATOR, id, MF_BYCOMMAND);
		}


	}
}

#if 0   
//-- not too useful, since framework doesn't call On_Command_UI handler before context menu goes up
void CPageView::OnUpdateIgnore(CCmdUI* pCmdUI) 
{
	CAvatarX *mousedAv = GetAvatar(mousedAvatarID);
	BOOL checked = ((CUserInfo *)mousedAv->m_userInfo)->Ignored();
	pCmdUI->SetCheck(checked);	
}
#endif

#if 0
void CPageView::OnIgnore() 
{
	BOOL ignore = !mousedPui->Ignored();
	mousedPui->Ignore(ignore);
}
#endif

#define SCROLLWIDTH			16

#if 0
void SetPanelsWide(int nWide) inline {
	if (GetChatDoc() && GetChatDoc()->m_bComicView)  // bug here: doesn't work if no chatdoc! XXX
		GetView()->SetPanelsWide(nWide);
}
#endif


void CPageView::SetPanelsWide(int nWide, int nForcedPanelWidth)
{			// allows hiding of CPageView from CChatDoc
	int nGoalPanelWidth = nForcedPanelWidth ? nForcedPanelWidth : GetProspectivePanelWidth(nWide);
	nGoalPanelWidth = max(nGoalPanelWidth, MINUNITPANELWIDTH);
	CUnitPanelPage::SetUnitPanelWidth(nGoalPanelWidth);
	CUnitPanelPage::SetUnitPanelHeight(nGoalPanelWidth);
	CUnitPanelPage::SetUnitPanelsPerRow(nWide);

	FreeRetainedPanelS();
	CreateRetainedPanel(GetClientDC(), &m_retSec, &m_retDib);

	ResetExistingPanels(TRUE);
	GetDocument()->ExecuteHistory(HM_RELOAD);

	ScrollToBottom();
}

void CPageView::ScrollToBottom() {
	// fool it into scrolling to bottom
	CRect r(m_bbox);
	DPtoLP(r);
	r.left = r.right - CUnitPanelPage::m_unitWidth;
	r.top = r.bottom + CUnitPanelPage::m_unitHeight;
	UpdateScroll(&r, FALSE);
}

#if 0
BOOL OKPanelWidth(int nWide) {
	int panelWidth = GetView()->GetProspectivePanelWidth(nWide);
	return (panelWidth >= MINUNITPANELWIDTH);
}
#endif

int CPageView::GetProspectivePanelWidth(int nWide) {
	RECT r;
	GetClientRect(&r);
	int xWidth = (int) ceil((r.right - SCROLLWIDTH) * m_dpiConvx);
	int goalPanelWidth = (xWidth + CUnitPanelPage::m_vInterstice*(1-nWide)) / nWide;

	// now reduce goalPanelWidth so that an integral number of panels fit height-wise
	int yHeight = (int) ceil(r.bottom * m_dpiConvy);
	int nHigh = (int) ceil ((double)(yHeight + CUnitPanelPage::m_hInterstice) /
						         ((double)(goalPanelWidth + CUnitPanelPage::m_hInterstice)));
	nHigh = max(nHigh, 1);  // must be at least 1 high
	int goalPanelHeight = (yHeight + CUnitPanelPage::m_hInterstice*(1-nHigh)) / nHigh;

	goalPanelWidth = min(goalPanelWidth, goalPanelHeight);
	return goalPanelWidth;
}

/*
void SetPanelsHigh(int nHigh) inline {
	GetView()->SetPanelsHigh(nHigh);
}

void CPageView::SetPanelsHigh(int nHigh) {
	int retval = AfxMessageBox(ID_KEEP_PANELS, MB_YESNOCANCEL);
	if (retval == IDCANCEL) return;

	int goalPanelHeight = GetProspectivePanelHeight(nHigh);
	CUnitPanelPage::SetUnitPanelWidth(goalPanelHeight);
	CUnitPanelPage::SetUnitPanelHeight(goalPanelHeight);

	// now, perform an inverse calculation from that in GetProspectivePanelWidth -- given a
	// panelWidth, calculate the n that fits completely widthwise
	RECT r;
	GetClientRect(&r);
	int xWidth = (int) ceil((r.right - SCROLLWIDTH) * m_dpiConvx);
	int goalNWide = (xWidth + CUnitPanelPage::m_vInterstice)
					 / (CUnitPanelPage::m_unitHeight + CUnitPanelPage::m_vInterstice);
	CUnitPanelPage::SetUnitPanelsPerRow(max(1, goalNWide));

	FreeRetainedPanelS();
	CreateRetainedPanel(GetClientDC(), &m_retSec, &m_retDib);

	ResetExistingPanels();
	if (retval == IDYES)
		GetDocument()->ExecuteHistory(HM_RELOAD);
}

BOOL OKPanelHeight(int nHigh) {
	int panelHeight = GetView()->GetProspectivePanelHeight(nHigh);
	return (panelHeight >= MINUNITPANELHEIGHT);
}


int CPageView::GetProspectivePanelHeight(int nHigh) {
	RECT r;
	int panelHeight = CUnitPanelPage::m_unitHeight;
	GetClientRect(&r);
	int yHeight = (int) ceil(r.bottom * m_dpiConvy);
	int goalPanelHeight = (yHeight + CUnitPanelPage::m_hInterstice*(1-nHigh)) / nHigh;
	ASSERT(panelHeight >= MINUNITPANELHEIGHT);
	return goalPanelHeight;
}
*/

// Similar to OnInitialUpdate, but doesn't reset connection, but Deletes existing pages
void CPageView::ResetExistingPanels(BOOL bAddPage) {
	CChatDoc *pDoc = GetDocument();

	m_bbox.SetRectEmpty(); // reset view size
	pDoc->DestroyPages();
	SetScrollSizes(MM_TWIPS, CSize(1, 1), CSize(0, 0), CSize(0,0));
	if (bAddPage) {
		pDoc->AddNewPage();
		CPage *firstPage = (CPage *) (pDoc->m_pages.GetHead());
		firstPage->AddTitle(pDoc->GetComicsTitle());  // add title
	}
	Invalidate();			// clear this view in its entirety
}


void UpdateTitle(CChatDoc *doc) {
	if (!doc) doc = GetView()->GetDocument();
	if (doc->m_pages.IsEmpty()) return;
	CPage *p = (CPage *) doc->m_pages.GetHead();
	p->UpdateTitle();
}


HWND hgPrevFocus;  // global variable to contain hwnd of last window with focus

void CPageView::OnKillFocus(CWnd* pNewWnd) 
{
	CScrollView::OnKillFocus(pNewWnd);
	
	hgPrevFocus = m_hWnd;
	
}


void CPageView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CPoint pt(0,0);
	CRect rect;
	switch(nChar)
	{
	case VK_HOME:
		VerifyScrollPosition(pt);
		ScrollToDevicePosition(pt);
		UpdateWindow();
		break;
	case VK_END:
		GetClientRect(&rect);
		if (m_totalDev.cy > rect.bottom) { // only scroll down if there's a scrollbar!
			pt.y = GetScrollLimit(SB_VERT);
			VerifyScrollPosition(pt);
			ScrollToDevicePosition(pt);
			UpdateWindow();
		}
		break;
	case VK_NEXT:
		pt = GetDeviceScrollPosition();
		GetClientRect(&rect);
		pt.y += rect.Height();
		VerifyScrollPosition(pt);
		ScrollToDevicePosition(pt);
		UpdateWindow();
		break;
	case VK_PRIOR:
		pt = GetDeviceScrollPosition();
		GetClientRect(&rect);
		pt.y -= rect.Height();
		VerifyScrollPosition(pt);
		ScrollToDevicePosition(pt);
		UpdateWindow();
		break;
	}

	CScrollView::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CPageView::VerifyScrollPosition(CPoint& pt)
{
	int xMax = GetScrollLimit(SB_HORZ);
	int yMax = GetScrollLimit(SB_VERT);
	if (pt.x < 0)
		pt.x = 0;
	else if (pt.x > xMax)
		pt.x = xMax;
	if (pt.y < 0)
		pt.y = 0;
	else if (pt.y > yMax)
		pt.y = yMax;
}

void CPageView::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
	if (nSBCode == SB_THUMBTRACK || nSBCode == SB_THUMBPOSITION) {

        // since we are tracking via the thumb, we have to get the 32-bit value
		// a little differently
		SCROLLINFO info;
        info.cbSize = sizeof(SCROLLINFO);
        info.fMask = SIF_TRACKPOS;
		GetScrollInfo(SB_VERT, &info);

        nPos = info.nTrackPos;
	}
	m_bAtBottom = AtBottom(); // recache value
	CScrollView::OnVScroll(nSBCode, nPos, pScrollBar);
}

#if 0
void CPageView::OnWindowPosChanging(WINDOWPOS *pos) {
	CScrollView::OnWindowPosChanging(pos);
}
#endif

float CPageView::m_dpiConvx;						// for screen, physical dpi * m_dpiConv = logical dpi
float CPageView::m_dpiConvy;
int	CPageView::m_dpiX;
int CPageView::m_dpiY;


void CPageView::OnSize(UINT nType, int cx, int cy) 
{
	if (m_bAtBottom) ScrollToBottom();		// So that we don't lose autoscrolling on sizing

	CScrollView::OnSize(nType, cx, cy);
	
//	if (m_bAtBottom) ScrollToBottom();		// So that we don't lose autoscrolling on sizing	
}
