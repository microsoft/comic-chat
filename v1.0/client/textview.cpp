// TextView.cpp : implementation file
//

#include "stdafx.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "chatprot.h"
#include "saywnd.h"
#include "chat.h"
#include "ui.h"
#include "userinfo.h"
#include "TextView.h"
#include "resource.h"
#include "url.h"
#include <math.h>
#include <winnls.h>
#include "memblst.h"

#define IDC_RICHEDIT	501
// WARNING: RE: LINK defines...
// The following should be in v. 2.0 of richedit.h.  Since our build enviroment only has v. 1.0,
// I'm including these defines here.  Please fix as soon as the new .h is available!!!
#define CFM_LINK		0x00000020		/* Exchange hyperlink extension */
#define CFE_LINK		0x0020
#define ENM_LINK				0x04000000
#define EN_LINK					0x070b
#ifdef _WIN32
#	define	_WPAD	/##/
#else
#	define	_WPAD	WORD
#endif
typedef struct _enlink
{
    NMHDR nmhdr;
    UINT msg;
    _WPAD   _wPad1;
    WPARAM wParam;
    _WPAD   _wPad2;
    LPARAM lParam;
    CHARRANGE chrg;
} ENLINK;

// END WARNING

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_LOGINDLG WM_USER+1

/////////////////////////////////////////////////////////////////////////////
// CTextView

IMPLEMENT_DYNCREATE(CTextView, CView)

CTextView::CTextView()
{
	m_pRichEdit = NULL;
	m_bFirstTime = TRUE;	// the first time this is checked it will be the first time through
}

CTextView::~CTextView()
{
	delete m_pRichEdit;
	cui.m_pvClientView = NULL;			// no longer addressable
}


BEGIN_MESSAGE_MAP(CTextView, CView)
	//{{AFX_MSG_MAP(CTextView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_LOGINDLG, OnLoginDlg)
	ON_NOTIFY(EN_LINK, IDC_RICHEDIT, HandleLink)
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextView drawing

void CTextView::OnDraw(CDC* pDC)
{
	CDocument* pDoc = GetDocument();
	// TODO: add draw code here
}

/////////////////////////////////////////////////////////////////////////////
// CTextView diagnostics

#ifdef _DEBUG
void CTextView::AssertValid() const
{
	CView::AssertValid();
}

void CTextView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CTextView message handlers

int CTextView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pRichEdit = new CTextEdit;
	m_pRichEdit->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | ES_MULTILINE | ES_READONLY,
						CRect(0,0,0,0), this, IDC_RICHEDIT);
	m_pRichEdit->SetEventMask(ENM_LINK | ENM_MOUSEEVENTS /* | ENM_DROPFILES */);

	CHARFORMAT cf;
	m_pRichEdit->GetDefaultCharFormat(cf);
	CWindowDC richDC(m_pRichEdit);
	TEXTMETRIC tm;
	richDC.GetTextMetrics(&tm);
	m_lineHeight = tm.tmHeight + tm.tmExternalLeading;
	return 0;
}

void CTextView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	if(!m_bFirstTime)
		PostMessage(WM_LOGINDLG,0,0);	// Not the first time activated so we can login from here
}

// This function was implemented so that we could better regulate when the login dialog is 
// brought up.  For instance, bringing it up on OnInitialUpdate is too soon for IE3.0 the first time
// so it must be brought up in OnActivateView the first time only, and then afterwards in 
// OnInitialUpdate.
LRESULT CTextView::OnLoginDlg(WPARAM wParam, LPARAM lParam)
{
	CChatDoc* pDoc = (CChatDoc *)GetDocument();
	if (pDoc->m_fileType == FT_CCR)
		VERIFY(ChatInitialize());   // initialize protocol connection

	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CTextEdit

IMPLEMENT_DYNCREATE(CTextEdit, CRichEditCtrl)


CTextEdit::CTextEdit()
{
}

CTextEdit::~CTextEdit()
{
}


BEGIN_MESSAGE_MAP(CTextEdit, CRichEditCtrl)
	//{{AFX_MSG_MAP(CTextEdit)
	ON_WM_KILLFOCUS()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextEdit message handlers

void CTextView::PopulateLine(CUserInfo *pui, const char *line, UCHAR mode) {
	CString outString;
	int stringID = 0;

	switch (mode) {
	  case SM_SAY:
		stringID = ID_SAY_PREFIX;
		break;
	  case SM_WHISPER:
		stringID = ID_WHISPER_PREFIX;
		break;
	  case SM_THINK:
		stringID = ID_THINK_PREFIX;
		break;
	}
	
	if (stringID != 0) {
		outString.LoadString(stringID);
		VERIFY(ReplaceToken(outString, CString("%1"), pui->GetName()));
		ShowText(outString, FALSE, NULL, 0);
	}
}


void CTextView::ShowText(const char *text, BOOL newPgraph, CPtrArray *links, int scrollToEnd) {
	int cch;
	CHARRANGE cr;
	CRect rect;

#if 0
	CHARFORMAT cf;
	cf.dwMask = CFM_SIZE | /*CFM_UNDERLINE |*/ CFM_FACE;
//	cf.dwEffects = CFM_UNDERLINE;
	cf.yHeight = 225;
	strcpy(cf.szFaceName, "Comic Sans MS");
	m_pRichEdit->SetSelectionCharFormat(cf);
	m_pRichEdit->SetDefaultCharFormat(cf);
#endif

	if (scrollToEnd == -1)
		scrollToEnd = ScrolledToEnd();

//	TRACE("Current line count = %d.\n", m_pRichEdit->GetLineCount());
	if (newPgraph) NewParagraph();

	// get character count
	cch = /* (short) */ m_pRichEdit->GetWindowTextLength();

	// set selection to last character then replace it with new text
	cr.cpMin = cr.cpMax = cch;
	m_pRichEdit->SendMessage( EM_EXSETSEL, 0, (LPARAM) &cr );

	// handle other options
#if 0
	CharacterFormat( !!(eLogOptions & eLBold), crTextColor );
	Indent( !!(eLogOptions & eLIndent) );
	SuperScript( !!(eLogOptions & eLSuperScript) );
#endif
	m_pRichEdit->SendMessage( EM_REPLACESEL, 0, (LPARAM) text );

	if (links) RegisterTextLinks(links, cch);


	if (scrollToEnd) {
		// get window size (for scrolling)
		GetClientRect(&rect);
		int nLinesVisible = rect.Height() / m_lineHeight;
		int topLineActual = m_pRichEdit->GetFirstVisibleLine();
		int nLinesTotal = m_pRichEdit->GetLineCount();
		int topLineWanted = nLinesTotal - nLinesVisible;
		if (topLineWanted > topLineActual) {
			m_pRichEdit->LineScroll(topLineWanted - topLineActual);
//			TRACE("Scrolling %d lines.\n", topLineWanted - topLineActual);
		}

//		TRACE("At End: ScrollPos/ScrollLimit = %d / %d.\n", m_pRichEdit->GetScrollPos(SB_VERT), m_pRichEdit->GetScrollLimit(SB_VERT));
	}
#if 0
	CHARFORMAT cf;
	cf.dwMask = CFM_SIZE;
	m_pRichEdit->GetSelectionCharFormat(cf);
	CClientDC foo(m_pRichEdit);
	CSize cx(0, cf.yHeight);
	foo.LPtoDP(&cx);

	m_pRichEdit->GetDefaultCharFormat(cf);
	CSize cy(0, cf.yHeight);
	foo.LPtoDP(&cy);
	int n = foo.GetDeviceCaps(LOGPIXELSY);
	int lfHeight = -MulDiv(cf.yHeight, foo.GetDeviceCaps(LOGPIXELSY), 72);
#endif
	GetChatDoc()->m_bEnableFileMenu = TRUE;
	if(GetChatDoc()->m_bPrompt && !GetChatDoc()->m_bEmbedded)
		GetChatDoc()->SetModifiedFlag(TRUE);
}

void CTextView::TextLine(CUserInfo *pui, const char *line, UCHAR mode, BOOL cooked, CPtrArray *links) {
	if (mode == SM_SAY && !strcmp(line, "<Chr>")) return;   // don't draw change pose lines
	BOOL autoScroll = ScrolledToEnd();
	NewParagraph();
	PopulateLine(pui, line, mode);
	ShowText(line, FALSE, links, autoScroll);
}


void CTextView::NewParagraph()
{
	int cch;
	CHARRANGE cr;

	// get character count
	cch = /* (short) */ m_pRichEdit->GetWindowTextLength();

	if (cch <= 0) return;	// only allow new paragraph if no text present

	// set selection to last character then replace it with new text
	cr.cpMin = cr.cpMax = cch;

	m_pRichEdit->SendMessage( EM_EXSETSEL, 0, (LPARAM) &cr );
	m_pRichEdit->SendMessage( EM_REPLACESEL, 0, (LPARAM) (const char *) "\r\n");
}

void CTextView::ShowInfo(CUserInfo *pui, const char *info) {
	CString mesg;
	mesg.LoadString(IDS_SHOWINFO_PREFIX);
	ReplaceToken(mesg, CString("%1"), pui->GetName());
	mesg += info;
	ShowText(mesg);
}

void CTextView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	m_pRichEdit->MoveWindow(0, 0, cx, cy);
	
}

void CTextView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetSay()->SetFocusToSayWnd();
	CWnd::GetFocus()->SendMessage( WM_CHAR, nChar, nFlags);
}


void CTextView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if(m_bFirstTime)	// This is the first time the view is dispayed, so we must be initialized and 
	{					// time to show the dialog
		PostMessage(WM_LOGINDLG,0,0);
		m_bFirstTime = FALSE;
	}
}

void CTextView::ClearTextView() {
	m_pRichEdit->SetWindowText("");
}

BOOL CTextView::ScrolledToEnd() {
	int scrollPos = m_pRichEdit->GetScrollPos(SB_VERT);
	int scrollLimit = m_pRichEdit->GetScrollLimit(SB_VERT);
	//	TRACE("AT FIRST:: ScrollPos/ScrollLimit = %d / %d.\n", m_pRichEdit->GetScrollPos(SB_VERT), m_pRichEdit->GetScrollLimit(SB_VERT));
	// in Large Text mode, sometimes scrollPos can't be set all the way to scrollLimit, so add some fuzz
	return (scrollPos + m_lineHeight >= scrollLimit); 
}

void CTextView::RegisterTextLinks(CPtrArray *m_links, int cch) {
	if (m_links) {
		CHARFORMAT cf, oldCF;
		CHARRANGE	chrgSave;

		// Save the current selection and then hide it
		//
		m_pRichEdit->GetSel(chrgSave);
		m_pRichEdit->HideSelection(TRUE, FALSE);
		m_pRichEdit->GetDefaultCharFormat(oldCF);  // also save old sel format

		// And set up the character format that we will impose on any found URLs
		//
		cf.cbSize		= sizeof(CHARFORMAT);
		cf.dwMask		= CFM_LINK | CFM_UNDERLINE | CFM_COLOR;
		cf.dwEffects	= CFE_LINK | CFE_UNDERLINE;
		cf.crTextColor	= linkColor;

		int upper = m_links->GetUpperBound();
		for (int i = 0; i <= upper; i++) {
			CLink *link = (CLink *)((*m_links)[i]);

			m_pRichEdit->SetSel(cch + link->m_start, cch + link->m_start + link->m_length);
			m_pRichEdit->SetSelectionCharFormat(cf);
		}

		// Restoration
#if 0
		ft.chrgText.cpMax = GetTextLength();
		ft.chrgText.cpMin = ft.chrgText.cpMax;
		m_pRichEdit->SendMessage(EM_EXSETSEL,(LPARAM) &ft.chrgText);
		m_pRichEdit->SetDefaultCharFormat();
#endif
		m_pRichEdit->SetSel(chrgSave);
		m_pRichEdit->HideSelection(FALSE,FALSE);
		m_pRichEdit->SetDefaultCharFormat(oldCF);
	}
}


//
// User clicked on a URL. The parent window then calls this routine..so the click
// is handled.
//
void CTextView::HandleLink ( NMHDR * pNotifyStruct, LRESULT * result ) {
	BOOL		fShift = FALSE;
	TEXTRANGE	tr;
	TCHAR		szURL[MAX_PATH];
	HCURSOR		hCursor = NULL;
	BOOL		fRet = TRUE;
	ENLINK		*penlink = (ENLINK *) pNotifyStruct;



	if (WM_SETCURSOR == penlink->msg) {
		//
		// Set the cursor to a HAND
		//
#if 0
		SetCursor(m_hCursor);
#endif
		*result = 0;
		return;
	}

	//
	// Only respond to full clicks.. and if the control key is down, let them edit/copy it
	//
	if (WM_LBUTTONDOWN != penlink->msg || (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
		*result = 0;
		return;
		}
	#if 0
	//
	// if the path to the browser is NULL, or the user uses the shift key to invoke a browser
	// we throw up a common file dialog to find the browser
	//
	fShift = !!(GetAsyncKeyState(VK_SHIFT) & 0x8000);
	if (fShift)
		{
		if (!FFindBrowser(TRUE))
			{
			return FALSE;
			}
		}
#endif
	//
	// Must make sure that the URL isn't too long..if it is, we will just have to truncate it
	//
	tr.chrg.cpMin	= penlink->chrg.cpMin;
	tr.chrg.cpMax	= penlink->chrg.cpMax;
	if ((tr.chrg.cpMax - tr.chrg.cpMin) > (sizeof(szURL)- 1))
		{
		tr.chrg.cpMax	= (sizeof(szURL)- 1);
		}
	tr.lpstrText	= szURL; // let Richedit place the URL in the rest of the buffer
	//
	// Get the selection
	//	
	m_pRichEdit->SendMessage(EM_GETTEXTRANGE, 0, (LPARAM) &tr);
	//
	// Eliminate an \r and \n at the end
	//
	int cch;

	cch = ::lstrlen(szURL) - 1;
	while (cch >= 0)
		{
		switch(szURL[cch])
			{
			default:
				szURL[cch + 1] = '\0';
				goto LLaunch;
			
			case '\r':
			case '\n':
			case ' ':
				break;					
			}
		--cch;
		}

LLaunch:	
	//
	// Launch it
	//

#if 0
	//
	// MIC URLS are different and are NOT launched using a Web Browser
	//
	if (FHandleIfMicURL(m_hWnd,szURL))
		{
		return TRUE;
		}

	hCursor = ::SetCursor(::LoadCursor(NULL,IDC_APPSTARTING));
#endif
	if (fShift || !FLaunchBrowser(szURL)) {
//		fRet = FWinExecBrowser(szURL);
	}
	
#if 0
	::SetCursor(hCursor);
#endif

	*result = 0;
}


void CTextView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{

	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT(pInfo != NULL);
	ASSERT(pInfo->m_bContinuePrinting);

	long nStartIndex = 0;
	long nTextPrinted = 0;
	
	do
	{
		pDC->StartPage();
		// print as much as possible in the current page.
		nTextPrinted = PrintPage(pDC, nStartIndex, 0xFFFFFFFF);
		PrintFooter(pDC,pInfo);
		pDC->EndPage();
		if(nTextPrinted < m_pRichEdit->GetTextLength())
			nStartIndex = nTextPrinted;
		pInfo->m_nCurPage++;
	}while(nTextPrinted < m_pRichEdit->GetTextLength());

	pDC->EndDoc();


}

long CTextView::PrintPage(CDC* pDC, long nIndexStart, long nIndexStop)
	// worker function for laying out text in a rectangle.
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	FORMATRANGE fr;

	CFont *oldFont = pDC->SelectObject(m_pFooterFont);

   int         nHorizRes = GetDeviceCaps(pDC->m_hDC, HORZRES),
               nVertRes = GetDeviceCaps(pDC->m_hDC, VERTRES),
               nLogPixelsX = GetDeviceCaps(pDC->m_hDC, LOGPIXELSX),
               nLogPixelsY = GetDeviceCaps(pDC->m_hDC, LOGPIXELSY);

   // Ensure the printer DC is in MM_TEXT mode.
   SetMapMode ( pDC->m_hDC, MM_TEXT );

   // Rendering to the same DC we are measuring.
   fr.hdc = fr.hdcTarget = pDC->m_hDC;

   // Set up the page.
   fr.rcPage.left     = fr.rcPage.top = 0;
   fr.rcPage.right    = (nHorizRes/nLogPixelsX) * 1440;
   fr.rcPage.bottom   = (nVertRes/nLogPixelsY) * 1440;

   // Set up 1/2" margins all around.
   fr.rc.left   = fr.rcPage.left + 1440/2;  // 1440 TWIPS = 1 inch.
   fr.rc.top    = fr.rcPage.top + 1440/2;
   fr.rc.right  = fr.rcPage.right - 1440/2;
   fr.rc.bottom = fr.rcPage.bottom - 1440/4;

   // Default the range of text to print as the entire document.
   fr.chrg.cpMin = nIndexStart;
   fr.chrg.cpMax = nIndexStop;

   long lRes = m_pRichEdit->FormatRange(&fr,TRUE);

   	pDC->SelectObject(oldFont);


   return lRes;
}


BOOL CTextView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	return CView::DoPreparePrinting(pInfo);
}

void CTextView::PrintFooter(CDC *pDC, CPrintInfo *pInfo) {
	char buff[60];
	SYSTEMTIME st;
	CRect *pbox = &pInfo->m_rectDraw;			// for easy reference
	int y = pbox->bottom + 1;					// the y at which to draw the footer												// +1 necessary so text not clipped (why?)
	
	CFont *oldFont = pDC->SelectObject(m_pFooterFont);

	// first output title
	int oldAlign = pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
	pDC->TextOut(pbox->left, y, "Microsoft Comic Chat");

	// next output page num
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	sprintf(buff, "Page %u", pInfo->m_nCurPage);	// should use the m_strPageDesc field
	pDC->TextOut((pbox->left + pbox->right)/2, y, buff);

	// finally output date/time
	GetLocalTime(&st);
	GetTimeFormat(LOCALE_USER_DEFAULT, NULL, &st, "h':'mm':'ss tt", buff, sizeof(buff));
	strcat(buff, "   ");
	int nchars = strlen(buff);
	GetDateFormat(LOCALE_USER_DEFAULT, NULL, &st, "MMM d, yy", buff+nchars, sizeof(buff)-nchars);
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
	pDC->TextOut(pbox->right, y, buff);

	pDC->SetTextAlign(oldAlign);
	pDC->SelectObject(oldFont);
}


void CTextView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnBeginPrinting(pDC, pInfo);

	m_pFooterFont = new CFont();
	VERIFY(m_pFooterFont->CreateFont(125, 0, 0, 0, FW_DONTCARE, FALSE, FALSE, FALSE,
								    ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
								    DEFAULT_QUALITY, VARIABLE_PITCH | FF_DONTCARE,
								    "Comic Sans MS"));

   m_pRichEdit->FormatRange(NULL,FALSE);
}

void CTextView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	CView::OnEndPrinting(pDC, pInfo);

	delete m_pFooterFont;
	m_pFooterFont = NULL;
    m_pRichEdit->FormatRange(NULL,FALSE);

}

BOOL CTextView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{

	NMHDR* pNMHDR = (NMHDR*)lParam;
	MSGFILTER* pMSGFILTER = (MSGFILTER*)lParam;
	if((pNMHDR->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
	{
		CMenu menu;
		POINT screenPoint;
		screenPoint.x = LOWORD(pMSGFILTER->lParam);
		screenPoint.y = HIWORD(pMSGFILTER->lParam);
		ClientToScreen(&screenPoint);
		if(screenPoint.x == -1 && screenPoint.y == -1)  // then we must be invoking menu with Shift+F10
		{
			CRect rect; // so lets set up our own point
			GetClientRect(&rect);
			screenPoint.x = (rect.right - rect.left)/2;
			screenPoint.y = (rect.bottom - rect.top)/2;
			ClientToScreen(&screenPoint);
		}
		menu.LoadMenu(IDR_VIEWCONTEXT);
//		((CFrameWnd*)AfxGetMainWnd())->SendMessage(WM_INITMENUPOPUP,WPARAM(menu.m_hMenu),0);
		if(GetChatApp()->m_bComicView)
			menu.GetSubMenu(0)->CheckMenuRadioItem(0,1,0,MF_BYPOSITION);
		else
			menu.GetSubMenu(0)->CheckMenuRadioItem(0,1,1,MF_BYPOSITION);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
											   screenPoint.x, screenPoint.y, (CWnd *)cui.m_pvChatView);
	}
	return CView::OnNotify(wParam, lParam, pResult);
}
	extern HWND hgPrevFocus;
void CTextView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
	
	hgPrevFocus = m_hWnd;
}

void CTextEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CRichEditCtrl::OnKillFocus(pNewWnd);
	
	hgPrevFocus = m_hWnd;
	
}

void CTextEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == TAB)
	{
			GetMembers()->m_MemberListBox.SetFocus();
			int index = GetMembers()->m_MemberListBox.GetNextItem(-1,LVNI_FOCUSED);
			if(index == -1)
				GetMembers()->m_MemberListBox.SetItemState(0,LVIS_FOCUSED, LVIS_FOCUSED);
	}
	else if(nChar == SHIFTTAB)
	{
		::SetFocus(GetSay()->GetSayEdit());
	}

	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
