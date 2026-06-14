// TextView.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "saywnd.h"
#include "ui.h"
#include "textcore.h"
#include "TextView.h"
#include "resource.h"
#include <math.h>
#include <winnls.h>
#include "memblst.h"
#include "format.h"
#include "protsupp.h"

#define IDC_RICHEDIT	501
#define TIMEDATESEP		"   "

extern CChatApp theApp;
extern "C" void *GetMime();

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//CTextCore g_textCore;

/////////////////////////////////////////////////////////////////////////////
// CTextView

IMPLEMENT_DYNCREATE(CTextView, CView)

CTextView::CTextView()
{
	m_pFooterFont = NULL;
	m_pRichEdit = NULL;
	m_bFirstTime = TRUE;	// the first time this is checked it will be the first time through
	m_fontText = NULL;
}

CTextView::~CTextView()
{
	theApp.SaveToReg(TRUE /*bShort*/);

	m_textCore.DetachTextViewHWnd();
	delete m_pRichEdit;
	if (m_fontText) delete m_fontText;

	if( m_pDocument != NULL )
		GetDocument()->m_view = NULL;		// gone!
}


BEGIN_MESSAGE_MAP(CTextView, CView)
	//{{AFX_MSG_MAP(CTextView)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
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

void SetSpecificFont(CTextCore *tc, CHARFORMAT &cf, MSG_TYPE mType, BOOL bHeader) {
	if (mType < mtGetInfo) {  // iterate through all member types
		for (int i = 0; i < msEndEnum; i++)
			tc->bSetMessageFormat(&cf, mType, (MEMBER_STATUS)i, bHeader);
	} else {				  // iterate to end of message types
		for (int i = mtBeginInfo; i < mtEndEnum; i++)
			tc->bSetMessageFormat(&cf, (MSG_TYPE)i, (MEMBER_STATUS)0, bHeader);
	}
}

void SetBoldFont(CTextCore *tc, MSG_TYPE mType, BOOL bHeader) {
	CHARFORMAT *pCharFormat, cf;

	if (tc->bGetMessageFormat(&pCharFormat, mType, msHost, bHeader) && pCharFormat) {
		pCharFormat->dwEffects |= CFE_BOLD;
		pCharFormat->dwMask |= CFM_BOLD;
	} else {
		if (!tc->bGetDefaultMessageFormat(&pCharFormat, mType, bHeader) || !pCharFormat)
			return;
		cf = *pCharFormat;
		cf.dwEffects |= CFE_BOLD;
		cf.dwMask |= CFM_BOLD;
		pCharFormat = &cf;
	}
	tc->bSetMessageFormat(pCharFormat, mType, msHost, bHeader);	
}


void ResetSayFont(CHARFORMAT *pCF)
{
	if (pCF) 
	{
		BOOL bCHARFORMATToLOGFONT(CHARFORMAT *, DWORD, LOGFONT *);

		bCHARFORMATToLOGFONT(pCF, pCF->dwMask, &theApp.m_textFont);
		CSayWnd *swnd = GetSay();
		if (swnd && swnd->m_hWnd)
			swnd->SetFont(theApp.m_textFont, TRUE);
	}
}

void InitializeTextCore(CTextCore *tc, BOOL bResetOld = FALSE, BOOL bResetSay = FALSE)
{
	if (bResetOld)
	{
		tc->bReSetDefaultMsgTypeProperties(TRUE);
		tc->bReSetDefaultHighlightFormats(TRUE);
	}

	tc->SetHeaderSeparate(theApp.m_flags1 & F1_HEADERSEPARATE);
	tc->bSetInsertBlank(theApp.m_textSpacing);
	tc->SetURLBrowser(theApp.m_bEmbedded); // isn't really a formatting property, but convenient here
	tc->SetDBCSSystem(GetMime() != NULL);

	if (!theApp.m_bCfInitialized)
	{	// fill the array of text formats
		// Make the default font size = 10
		CHARFORMAT *pCharFormat;
		if (tc->bGetTextViewDefaultFormat(&pCharFormat) && pCharFormat)
		{
			pCharFormat->dwMask |= CFM_SIZE;
			pCharFormat->yHeight = 10 * 20;  // 10 pt * 20 twips/pt = twips
			tc->bSetTextViewDefaultFormat(pCharFormat);	// Set the default font for text View Window
			if (bResetSay)
				ResetSayFont(pCharFormat);
		}
	} 
	else
	{
		SetSpecificFont(tc, theApp.m_cfArray[0], mtJoin,	FALSE);
		SetSpecificFont(tc, theApp.m_cfArray[1], mtNormal,	TRUE);
		SetSpecificFont(tc, theApp.m_cfArray[2], mtNormal,	FALSE);
		SetSpecificFont(tc, theApp.m_cfArray[3], mtWhisper, TRUE);
		SetSpecificFont(tc, theApp.m_cfArray[4], mtWhisper, FALSE);
		SetSpecificFont(tc, theApp.m_cfArray[5], mtThought, TRUE);
		SetSpecificFont(tc, theApp.m_cfArray[6], mtThought, FALSE);
		SetSpecificFont(tc, theApp.m_cfArray[7], mtAction,	FALSE);
		SetSpecificFont(tc, theApp.m_cfArray[8], mtGetInfo, FALSE);
		SetSpecificFont(tc, theApp.m_cfArray[9], mtLeave,	FALSE);

		if (bResetSay) 
			ResetSayFont(&theApp.m_cfArray[2]);
	}

	if (theApp.m_bCfHLInitialized)
		for (SHORT nIndex = 0; nIndex < g_nHighlightedFormats; nIndex++)
			tc->bSetHighlightFormat(&theApp.m_cfArray[10+nIndex], nIndex);

	// make host messages bold, if necessary
	for (int i = 0; i < mtBeginInfo; i++)
	{
		if (theApp.m_iHostHighlight & HH_BOLD_MESSAGES)
			SetBoldFont(tc, (MSG_TYPE)i, FALSE);
		if ((i < mtBeginActions) && (theApp.m_iHostHighlight & HH_BOLD_HEADERS))
			SetBoldFont(tc, (MSG_TYPE)i, TRUE);
	}
}

int CTextView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	m_pRichEdit = new CTextEdit;
	m_pRichEdit->Create(WS_CHILD | WS_VISIBLE | WS_VSCROLL | TEXT_VIEW_WND_FLAGS,
						CRect(0,0,0,0), this, IDC_RICHEDIT);
	m_pRichEdit->SetEventMask(ENM_LINK | ENM_MOUSEEVENTS /* | ENM_DROPFILES */);

	VERIFY(SUCCEEDED(m_textCore.AttachTextViewHWnd(m_pRichEdit->m_hWnd, AfxGetInstanceHandle()))); // attach to hwnd to text core handler
	VERIFY(m_textCore.bSetTextViewBufferMaxSize(256000)); // FMs want a large limit
	InitializeTextCore(&m_textCore, FALSE, TRUE);

	return 0;
}

void CTextView::OnInitialUpdate() 
{
	CView::OnInitialUpdate();

	if(!m_bFirstTime)
	{
		TRACE("CTextView::OnInitialUpdate - Posting WM_LOGINDLG\n");
		GetDocument()->m_client->PostMessage(WM_LOGINDLG,0,0);	// Not the first time activated so we can login from here
	}
}

#if 0
// This function was implemented so that we could better regulate when the login dialog is 
// brought up.  For instance, bringing it up on OnInitialUpdate is too soon for IE3.0 the first time
// so it must be brought up in OnActivateView the first time only, and then afterwards in 
// OnInitialUpdate.
LRESULT CTextView::OnLoginDlg(WPARAM wParam, LPARAM lParam)
{
	CChatDoc* pDoc = GetDocument();
	if (pDoc->m_fileType == FT_CCR)
		VERIFY(ChatInitialize());   // initialize protocol connection

	return 0;
}
#endif


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
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTextEdit message handlers


void CTextView::TextLine(CUserInfo *puiSender, const char *szSenderNickname, const char *szReceiver, const char *szLine, USHORT uModes, BYTE bbCooked, 
						 CDWordArray *prgdwFormatting, char cHighlightType)
{
	if ((uModes & BM_SAY) && !strcmp(szLine, "<Chr>")) 
		return;   // don't draw change pose lines

	const char*		szFrom = szSenderNickname ? szSenderNickname : "";
	CString			strTo;
	MEMBER_STATUS	membStatus = msParticipant;
	MSG_TYPE		mesgType = mtEndEnum;   // just so we can check if we changed it

	if (puiSender)
	{
		szFrom = puiSender->GetScreenName();
		if (puiSender && puiSender->IsOperator())
			membStatus = msHost;
		else
			membStatus = msParticipant;
	}
	if (uModes == BM_SAY)
		mesgType = mtNormal;
	else if (uModes == BM_THINK)
		mesgType = mtThought;
	else if (uModes & BM_WHISPER)
	{
		mesgType = mtWhisper;
		GetAddressees(puiSender, ", ", strTo, FALSE);
	}
	else if (uModes & BM_EXCHAN)
	{
		ASSERT(szReceiver);
		mesgType = mtExChan;
		strTo = szReceiver;
	}

	if (mesgType != mtEndEnum)
	{
		int cbLen = strlen(szLine);
		m_textCore.iDisplayMsgHeader(cbLen,
									 szFrom,
									 0,
									 strTo,
									 0,
									 mesgType,
									 membStatus,
									 NULL,
									 2*cHighlightType);
		if (uModes & BM_ACTION)
			m_textCore.iDisplayAction(NULL,					/*szFrom, */
									  0, 
									  szLine,				/*szAction, */
									  0, 
									  membStatus,
									  TRUE,					/* bShowURLs */
									  DEFAULT_INDENT,		/* lIndent */
									  NULL,					/* pCharFormat */
									  2*cHighlightType+1,	/* iHighlightIndex */
									  prgdwFormatting ? prgdwFormatting->GetData() : NULL,
									  prgdwFormatting ? prgdwFormatting->GetSize() : 0);
		else
			m_textCore.iDisplayMsgText(szLine, 
									   cbLen,
									   mesgType,
									   membStatus,
									   TRUE,				/* bShowURLs */
									   FALSE,				/* bInformFull */
									   FALSE,				/* bAppend */
									   DEFAULT_INDENT,		/* lIndent */
									   NULL,				/* pCharFormat */
									   2*cHighlightType+1,	/* iHighlightIndex */
									   prgdwFormatting ? prgdwFormatting->GetData() : NULL,
									   prgdwFormatting ? prgdwFormatting->GetSize() : 0);
	} 
	else if (uModes & BM_ACTION)
	{
		m_textCore.iDisplayAction(NULL,					/*szFrom, */
								  0, 
								  szLine,				/*szAction, */
								  0, 
								  membStatus,
								  TRUE,					/* bShowURLs */
								  0,					/* lIndent */
								  NULL,					/* pCharFormat */
								  2*cHighlightType+1,	/* iHighlightIndex */
								  prgdwFormatting ? prgdwFormatting->GetData() : NULL,
								  prgdwFormatting ? prgdwFormatting->GetSize() : 0);
	} 
	else if (uModes & BM_NOFORMAT)
		m_textCore.iDisplayMsgText(szLine, 
								   0, 
								   mtNormal, 
								   msParticipant,
								   TRUE,				/* bShowURLs */
								   FALSE,				/* bInformFull */
								   FALSE,				/* bAppend */
								   DEFAULT_INDENT,
								   NULL,				/* pCharFormat */
								   2*cHighlightType+1,	/* iHighlightIndex */
								   prgdwFormatting ? prgdwFormatting->GetData() : NULL,
								   prgdwFormatting ? prgdwFormatting->GetSize() : 0);
}


void CTextView::DisplayPart(const char *szNick, char cHighlightType)
{
	CUserInfo*		pui = LookupPui(szNick);
	MEMBER_STATUS	membStatus = msParticipant;

	if (pui && pui->IsOperator())
		membStatus = msHost;
	if (pui)
		szNick = pui->GetQualifiedName();
	m_textCore.iDisplayMemberStatus(szNick, 0, mtLeave, membStatus, NULL, 2*cHighlightType+1);
}


void CTextView::DisplayJoin(const char *szNick, char cHighlightType)
{
	CUserInfo*		pui = LookupPui(szNick);
	MEMBER_STATUS	membStatus = msParticipant;

	if (pui)
	{
		if (pui->IsOperator())
			membStatus = msHost;
		szNick = pui->GetQualifiedName();
	}
	m_textCore.iDisplayMemberStatus(szNick, 0, mtJoin, membStatus, NULL, 2*cHighlightType+1);
}


void CTextView::DisplayNickChange(CUserInfo *pui, const char *szOldNick)
{
	MEMBER_STATUS membStatus = msParticipant;
	if (pui)
	{
		if (pui->IsOperator())
			membStatus = msHost;
		const char *DecodeNickForScreen(const char *);
		m_textCore.iDisplayInfo(DecodeNickForScreen(szOldNick), 0, UnConst(pui->GetScreenName()), 0,
								NULL, 0, mtAliasChange, membStatus);
	}
}


void CTextView::ShowInfo(CUserInfo *pui, const char *szInfo)
{
	MEMBER_STATUS membStatus = msParticipant;

	if (pui && pui->IsOperator())
		membStatus = msHost;
	else
		membStatus = msParticipant;

	// szInfo is a control full string
	CDWordArray* prgdwFormatting = new CDWordArray;

	char* szControlFull = strdup(szInfo);

	if (!prgdwFormatting || !szControlFull)
		return;

	char* szControlLess = SzControlLess(szControlFull, prgdwFormatting);

	m_textCore.iDisplayInfo(NULL,
							0,
							"",
							0,
							szControlLess,
							0,
							mtGetInfo,
							membStatus,
							NULL,
							-1,
							prgdwFormatting->GetData(),
							prgdwFormatting->GetSize());

	free(szControlFull);
	FreeAndNullFormatting(&prgdwFormatting);
}


void CTextView::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	// REGISB: obscure (very obscure) hack to fix bug 4552
	m_pRichEdit->MoveWindow(0, 0, cx, cy-1);
	m_pRichEdit->MoveWindow(0, 0, cx, cy);

	// Make the thing re-recognize its scroll position.
	SCROLLINFO si;
	if (m_pRichEdit->GetScrollInfo (SB_VERT, &si) && si.nMin < si.nMax)
	{
		m_pRichEdit->SendMessage (WM_VSCROLL, MAKELONG (SB_THUMBPOSITION, (WORD)(short)si.nPos), NULL);
	}
}


void CTextView::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	GetDocument()->SetFocusToSayWnd();
	CWnd::GetFocus()->SendMessage( WM_CHAR, nChar, nFlags);
}


void CTextView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if(m_bFirstTime)	// This is the first time the view is dispayed, so we must be initialized and 
	{					// time to show the dialog
		TRACE("CTextView::OnActivateView - Posting WM_LOGINDLG\n");
		GetDocument()->m_client->PostMessage(WM_LOGINDLG,0,0);
		m_bFirstTime = FALSE;
	}

	if (bActivate) GetDocument()->SetFocusToSayWnd();
}

void CTextView::ClearTextView() {
	m_textCore.dwClearTextViewBuffer(0);
}


//
// User clicked on a URL. The parent window then calls this routine..so the click
// is handled.
//
void CTextView::HandleLink ( NMHDR * pNotifyStruct, LRESULT * result ) {
	*result = m_textCore.bHandleLink((ENLINK *) pNotifyStruct);
}


BOOL CTextView::OnPreparePrinting(CPrintInfo* pInfo) 
{
	// TRACE("CTextView::OnPreparePrinting - Enter\n");

	return CView::DoPreparePrinting(pInfo);
}


void CTextView::OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TRACE("CTextView::OnBeginPrinting - Enter\n");

	if (m_pFooterFont = new CFont())
	{
		LOGFONT lf = theApp.m_textFont;
		lf.lfHeight = 45;
		lf.lfWeight = FW_REGULAR;
		VERIFY(m_pFooterFont->CreateFontIndirect(&lf));
	}

	ASSERT(pInfo);
	pInfo->SetMaxPage(lPrintPage(pDC, 0, FALSE));		// sets the number of pages in the document

	CView::OnBeginPrinting(pDC, pInfo);

    m_pRichEdit->FormatRange(NULL, FALSE);

//	DOCINFO	docinfo;

//	docinfo.cbSize		= sizeof(DOCINFO);
//	docinfo.lpszDocName = "<Channel Name>";	// not greater than 32 characters including the \0
//	docinfo.lpszOutput	= NULL;
//	docinfo.lpszDatatype= NULL;
//	docinfo.fwType		= 0L;

//	int iRet = pDC->StartDoc(&docinfo);
//	ASSERT(iRet > -1);
}


void CTextView::OnPrint(CDC* pDC, CPrintInfo* pInfo) 
{
	// TRACE("CTextView::OnPrint - Enter\n");

	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	ASSERT(pInfo != NULL);
	ASSERT(pInfo->m_bContinuePrinting);

//	int iRet = pDC->StartPage();
//	ASSERT(iRet > 0);
	
	// print as much as possible in the current page.
	lPrintPage(pDC, pInfo->m_nCurPage, TRUE);
	PrintFooter(pDC, pInfo);

//	iRet = pDC->EndPage();
//	ASSERT(iRet > 0);
}


void CTextView::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo) 
{
	// TRACE("CTextView::OnEndPrinting - Enter\n");

	ASSERT(pDC);
	ASSERT(pInfo);
//	int iRet = pDC->EndDoc();
//	ASSERT(iRet > 0);

	CView::OnEndPrinting(pDC, pInfo);

	if (m_pFooterFont)
	{
		delete m_pFooterFont;
		m_pFooterFont = NULL;
	}

    m_pRichEdit->FormatRange(NULL, FALSE);

}


long CTextView::lPrintPage(CDC* pDC, UINT nCurPage, BOOL bDisplay)
	// worker function for laying out text in a rectangle.
{
	ASSERT_VALID(this);
	ASSERT_VALID(pDC);
	FORMATRANGE fr;

	CFont *pOldFont = pDC->SelectObject(m_pFooterFont);

	long	lRet;
	int		nHorizRes = GetDeviceCaps(pDC->m_hDC, HORZRES),
			nVertRes = GetDeviceCaps(pDC->m_hDC, VERTRES),
			nLogPixelsX = GetDeviceCaps(pDC->m_hDC, LOGPIXELSX),
			nLogPixelsY = GetDeviceCaps(pDC->m_hDC, LOGPIXELSY);

	// Ensure the printer DC is in MM_TEXT mode.
	SetMapMode(pDC->m_hDC, MM_TEXT);

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

	LONG lTextPrinted = 0L;
		
	lRet = 0L;
	fr.chrg.cpMin =  0L;
	fr.chrg.cpMax = -1L;
		
	if (bDisplay)
	{
		// first jump to the nCurPage th page
		for (UINT nTmpPage = 1; nTmpPage < nCurPage; nTmpPage++)
		{
			lTextPrinted = m_pRichEdit->FormatRange(&fr, FALSE);
			fr.chrg.cpMin = lTextPrinted;
		}
		// then print nCurPage th page
		lRet = m_pRichEdit->FormatRange(&fr, TRUE);
	}
	else
	{
		do
		{
			lTextPrinted = m_pRichEdit->FormatRange(&fr, FALSE);
			lRet++;
			fr.chrg.cpMin = lTextPrinted;
		}
		while (lTextPrinted < m_pRichEdit->GetTextLength());
	}

	pDC->SelectObject(pOldFont);

	return lRet;
}


void CTextView::PrintFooter(CDC *pDC, CPrintInfo *pInfo)
{
	// TRACE("CTextView::PrintFooter - Enter\n");
	ASSERT(pInfo);

	char		szBuff[64];
	SYSTEMTIME	st;
	CRect*		pbox = &pInfo->m_rectDraw;			// for easy reference
	CFont*		pOldFont = pDC->SelectObject(m_pFooterFont);
	int			iOldAlign;
	CString		strAppTitle, strFormat;

	pbox->left  += 50;	// left margin
	pbox->right -= 50;	// right margin
	pbox->bottom-= 50;	// bottom margin
	
	// first output title
	iOldAlign = pDC->SetTextAlign(TA_LEFT | TA_BOTTOM);
	strAppTitle.LoadString(AFX_IDS_APP_TITLE);
	pDC->TextOut(pbox->left, pbox->bottom, strAppTitle);

	// next output page num
	pDC->SetTextAlign(TA_CENTER | TA_BOTTOM);
	strFormat.LoadString(IDS_PAGEFOOTER);
	sprintf(szBuff, "%u", pInfo->m_nCurPage);  // should use the m_strPageDesc field
	VERIFY(ReplaceToken(strFormat, CString("%1"), szBuff));	
	pDC->TextOut((pbox->left + pbox->right)/2, pbox->bottom, strFormat);

	// finally output date/time
	GetLocalTime(&st);
	GetTimeFormat(LOCALE_USER_DEFAULT, NULL, &st, NULL, szBuff, sizeof(szBuff) - strlen(TIMEDATESEP));
	strcat(szBuff, TIMEDATESEP);
	int nChars = strlen(szBuff);
	GetDateFormat(LOCALE_USER_DEFAULT, NULL, &st, NULL, /*"MMM d, yy",*/ szBuff+nChars, sizeof(szBuff)-nChars);
	pDC->SetTextAlign(TA_RIGHT | TA_BOTTOM);
	pDC->TextOut(pbox->right, pbox->bottom, szBuff);

	pDC->SetTextAlign(iOldAlign);
	pDC->SelectObject(pOldFont);
}


BOOL CTextView::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	MSGFILTER* pMSGFILTER = (MSGFILTER*)lParam;
	// ShankuN 3/6/98 - Changed the handler to mouse button up (to be consistent
	// with Windows UI) and moved actual code to WM_CONTEXTMENU handler, to provide
	// cleaner subclassing ability, etc.
	if((pNMHDR->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONUP))
	{
		POINT screenPoint;
		screenPoint.x = LOWORD(pMSGFILTER->lParam);
		screenPoint.y = HIWORD(pMSGFILTER->lParam);
		(CWnd::FromHandle (pNMHDR->hwndFrom))->ClientToScreen(&screenPoint);
		SendMessage (WM_CONTEXTMENU, (WPARAM)pNMHDR->hwndFrom, MAKELPARAM(screenPoint.x, screenPoint.y));
	}
	return CView::OnNotify(wParam, lParam, pResult);
}

extern HWND hgPrevFocus;
void CTextView::OnKillFocus(CWnd* pNewWnd) 
{
	CView::OnKillFocus(pNewWnd);
	
	hgPrevFocus = m_hWnd;
}

void CTextView::OnContextMenu(CWnd* pWnd, CPoint screenPoint)
{
	CMenu menu;
	if (screenPoint.x == -1 && screenPoint.y == -1) {  // then we must be invoking menu with Shift+F10
		CRect rect; // so lets set up our own point
		GetClientRect(&rect);
		screenPoint.x = (rect.right - rect.left)/2;
		screenPoint.y = (rect.bottom - rect.top)/2;
		ClientToScreen(&screenPoint);
	}

	int nPopup = LoadContextMenu (menu);
	menu.GetSubMenu(nPopup)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
										   screenPoint.x, screenPoint.y, AfxGetMainWnd());
}


void CTextEdit::OnKillFocus(CWnd* pNewWnd) 
{
	CRichEditCtrl::OnKillFocus(pNewWnd);
	
	hgPrevFocus = m_hWnd;
	
}

void CTextEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(nChar == VK_TAB)
	{
		BOOL bShifted = GetKeyState(VK_SHIFT) & 0x8000;
		GetChatDoc ()->CycleFocus (CHATFOCUS_TEXTVIEW, bShifted);
		return;
	}

	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}


// RTF File Writing

DWORD CALLBACK WriteRTFBuffer(DWORD dwCookie, LPBYTE pbBuff, LONG cb, LONG FAR *pcb) { 
	*pcb = fwrite(pbBuff, sizeof(char), cb, (FILE *) dwCookie);
	return 0;
}


BOOL WriteRTF(CRichEditCtrl *pRichEd, const char *fileName) {
	FILE *fpFileOut;

	if ((fpFileOut = fopen(fileName, "w"))) {
		EDITSTREAM es;
		es.dwCookie = (DWORD) fpFileOut;  // pass fpFileOut as cookie
		es.pfnCallback = WriteRTFBuffer;
		es.dwError = 0;
		long nChars = pRichEd->StreamOut(SF_RTF, es);
		fclose(fpFileOut);
		return (es.dwError == 0);
	} else return FALSE;
}

void CTextView::SetURLBrowser(BOOL bNewBrowser) {
	m_textCore.SetURLBrowser(bNewBrowser);	// abstracted here for info hiding
}


void CTextEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (VK_TAB != nChar)
		ForwardToSayWnd(nChar);
	
//	CRichEditCtrl::OnChar(nChar, nRepCnt, nFlags);
}

void InitializeTextCores(BOOL bResetOld, BOOL bResetSay)
{
	void InitializeTextCore(CTextCore *, BOOL = FALSE, BOOL = FALSE);
	extern CPtrList g_docs;

	POSITION pos = g_docs.GetHeadPosition();
	while (pos) {
		CChatDoc *doc = (CChatDoc *) g_docs.GetNext(pos);
		if (!doc->m_bComicView && !doc->m_bStatusView) {  // skip status views for now
			CTextView *tv = (CTextView *) doc->m_view;
			if (tv) 
				InitializeTextCore(&tv->m_textCore, bResetOld, bResetSay);
		}
	}
}

// Loads the appropriate menu. Overriden by subclasses. Returns popup index to use.

int 
CTextView::LoadContextMenu(
CMenu& menu)
{
#ifdef CB32SUPPORT
	menu.LoadMenu(theApp.m_bDoCB32 ? IDR_VIEWCONTEXT_NM : IDR_VIEWCONTEXT);
#else
	menu.LoadMenu(IDR_VIEWCONTEXT);
#endif CB32SUPPORT
	return 0;
}
