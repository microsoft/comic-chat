// MyFontDg.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "textcore.h"
#include "txtfntdg.h"
#include "dlgs.h"
#include "tvres.h"
//#include "chatprot.h"

// THIS IS NECESSARY DUE TO AN OMISSION IN SOME commdlg.h VERSIONS.
// PLEASE REMOVE AS SOON AS POSSIBLE
#ifndef WM_CHOOSEFONT_SETLOGFONT
#define WM_CHOOSEFONT_GETLOGFONT      (WM_USER + 1)
#define WM_CHOOSEFONT_SETLOGFONT      (WM_USER + 101)
#endif
// END CHOOSEFONT DEFINES

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define ALL_MESSAGETYPE	255
extern CChatApp theApp;

static int mesgStarts[10];
static int mesgEnds[10];
static BYTE mesgTypes[10];
// For each item in the message type dropdown, mesgDropdown provides the index into the charformat array.
// Note that the first type, ALL_MESSAGETYPE, doesn't have a corresponding charformat.
//static BYTE mesgDropdown[] = {ALL_MESSAGETYPE, 2, 1, 4, 3, 6, 5, 7, 8, 0, 9 }; // first entry has no meaning
static BYTE mesgDropdown[] = {ALL_MESSAGETYPE, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 }; // first entry has no meaning
static int nLines;


/////////////////////////////////////////////////////////////////////////////
// CMyFontDialog

IMPLEMENT_DYNAMIC(CMyFontDialog, CFontDialog)

CMyFontDialog::CMyFontDialog(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd) : 
	CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
{
	m_bClosing		= FALSE;
	m_bFontCtlEvents= TRUE;
	m_hCursorHand	= NULL;
	m_bDeleteFont	= FALSE;
}


CMyFontDialog::~CMyFontDialog()
{
	if (m_bDeleteFont)
		m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CMyFontDialog, CFontDialog)
	//{{AFX_MSG_MAP(CMyFontDialog)
	ON_CBN_SELCHANGE(cmb1, OnFontChange)
	ON_WM_DESTROY()
	ON_CBN_SELCHANGE(cmb2, OnFontChange)
	ON_CBN_SELCHANGE(cmb3, OnFontChange)
	ON_CBN_SELCHANGE(cmb4, OnFontChange)
	ON_CBN_SELCHANGE(cmb5, OnFontChange)
	ON_CBN_SELCHANGE(IDC_MESSAGETYPE, OnChangeMessageType)
	ON_BN_CLICKED(chx1, OnStrikeoutChange)
	ON_BN_CLICKED(chx2, OnUnderlineChange)
	ON_WM_SETCURSOR()
	//}}AFX_MSG_MAP
	ON_NOTIFY(EN_SELCHANGE, 5, HandleSelection)
END_MESSAGE_MAP()


BOOL AddStringToCombo(const char *messageName, void *vpCombo) {
	CComboBox *combo = (CComboBox *) vpCombo;
	combo->AddString(messageName);
	return FALSE;
}


BOOL CMyFontDialog::OnInitDialog() 
{
	CFontDialog::OnInitDialog();

	// fill the message type combo box
	CString messageTypes;
	VERIFY(messageTypes.LoadString(IDS_MESSAGETYPES));
	BOOL ForPath(const char *path, BOOL soundFunc(const char *, void *), void *data);
	ForPath(messageTypes, AddStringToCombo, (void *)GetDlgItem(IDC_MESSAGETYPE));
	
	// create Rich Preview window
	// set up richedit example
	CRect rTextPreview(5, 20, 256, 140);
    //	CRect rTextPreview(cxPosAvatarPage, cyPosAvatarPage, (cxPosAvatarPage + cxAvatarPage),
    //   	                     (cyPosAvatarPage + cyAvatarPage));
	MapDialogRect(rTextPreview);
	m_richPreview = new CRichEditCtrl;
	VERIFY(m_richPreview->Create(TEXT_VIEW_WND_FLAGS | WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_BORDER, rTextPreview, this, 5 ) );
	m_richCore = new CTextCore;
	VERIFY(SUCCEEDED(m_richCore->AttachTextViewHWnd(m_richPreview->m_hWnd, AfxGetInstanceHandle())));
	m_richCore->bSetTextViewBufferMaxSize(10000);
//	m_richCore->bSetAutoScroll(TEXT_VIEW_AUTOSCROLL_NEVER);
	SetupRichPreview();

	// request selection change notifications to parent
	LRESULT mask = m_richPreview->SendMessage(EM_GETEVENTMASK, 0, 0);
	mask |= ENM_SELCHANGE;
	m_richPreview->SendMessage(EM_SETEVENTMASK, 0, mask);

	if (!(m_hCursorHand = ::LoadCursor(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDC_CURSORHAND))))
		m_hCursorHand = ::LoadCursor(NULL, IDC_ARROW);

	// Get GUI font
	LOGFONT	logFont1, logFont2;
	HFONT	hfont = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	ASSERT(hfont);
	if (hfont)
	{
		CComboBox *pCombo = (CComboBox*) GetDlgItem(cmb1);	// face name
		ASSERT(pCombo);
		CFont *pFont = pCombo->GetFont();
		ASSERT(pFont);

		GetObject(hfont, sizeof(LOGFONT), (LPVOID) &logFont1);
		pFont->GetLogFont(&logFont2);
		strcpy(logFont1.lfFaceName, logFont2.lfFaceName);
		m_font.CreateFontIndirect(&logFont1);
		m_bDeleteFont = TRUE;

		pCombo->SetFont(&m_font, FALSE);

		pCombo = (CComboBox*) GetDlgItem(cmb4);	// color
		pCombo->SetFont(&m_font, FALSE);

		pCombo = (CComboBox*) GetDlgItem(cmb5);	// script
		pCombo->SetFont(&m_font, FALSE);
	}
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL CMyFontDialog::SetupRichPreview() {
	CString line, nick1, nick2;
	const char *GetMyName();
	void InitializeTextCore(CTextCore *, BOOL = FALSE, BOOL = FALSE);
	int *end = mesgEnds;
	BYTE *mType = mesgTypes;

	m_richCore->dwClearTextViewBuffer(0);
	InitializeTextCore(m_richCore);
	nick1 = GetMyName();
	if (nick1.IsEmpty()) nick1.LoadString(IDS_DEFAULT_NICK);
	nick2.LoadString(IDS_SAMPLE_NICK);

	*mType++ = 0;
	m_richCore->iDisplayMemberStatus(nick1, 0, mtJoin, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 1;
	line.LoadString(IDS_SAMPLE_SEND);
	int len = strlen(line);
	m_richCore->iDisplayMsgHeader(len, nick1, 0, "", 0, mtNormal, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 2;
	m_richCore->iDisplayMsgText(line, 0, mtNormal, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 3;
	line.LoadString(IDS_SAMPLE_WHISPER);
	len = strlen(line);
	m_richCore->iDisplayMsgHeader(len, nick1, 0, nick2, 0, mtWhisper, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 4;
	m_richCore->iDisplayMsgText(line, 0, mtWhisper, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 5;
	line.LoadString(IDS_SAMPLE_THOUGHT);
	m_richCore->iDisplayMsgHeader(len, nick1, 0, "", 0, mtThought, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 6;
	m_richCore->iDisplayMsgText(line, 0, mtThought, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 7;
	line.LoadString(IDS_SAMPLE_ACTION);
	m_richCore->iDisplayAction(nick1, 0, line, 0, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 8;
	m_richCore->iDisplayInfo(nick1, 0, nick2, 0, NULL, 0, mtAliasChange, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 9;
	m_richCore->iDisplayMemberStatus(nick2, 0, mtLeave, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	nLines = end - mesgEnds;

	mesgStarts[0] = 0;
	int nNewMessageDelta = (theApp.m_textSpacing == TEXT_VIEW_BLANK_NEVER) ? 1 : 2;
	int nHeaderBodyDelta = (theApp.m_flags1 & F1_HEADERSEPARATE) ? 1 : g_nHeaderTabLen;
	for (int i = 1; i < nLines; i++)
		if (i == 2 || i == 4 || i == 6)
			mesgStarts[i] = mesgEnds[i-1] + nHeaderBodyDelta;
		else
			mesgStarts[i] = mesgEnds[i-1] + nNewMessageDelta;

	m_richPreview->SetSel(0, 0);		// move to top (autoscrolls there)
	m_richPreview->ShowWindow(SW_SHOWNA);  // due to textview bug, we need to make it visible again

	return TRUE;
}



int CMyFontDialog::GetMessageType() {
	CComboBox *combo = (CComboBox *) GetDlgItem(IDC_MESSAGETYPE);
	int sel = combo->GetCurSel();
	return ((sel == CB_ERR) ? -1 : sel);
}

void CMyFontDialog::SetMessageType(int mType) {
	int entry = -1;
	for (int i = 0; i < sizeof(mesgDropdown) / sizeof(BYTE); i++) {
		if (mType == mesgDropdown[i]) {
			entry = i;
			break;
		}
	}
//	ASSERT(entry > -1);
	CComboBox *combo = (CComboBox *) GetDlgItem(IDC_MESSAGETYPE);
	combo->SetCurSel(entry);
}


static COLORREF clrMenu[] = { RGB(0,0,0), RGB(128,0,0), RGB(0,128,0), RGB(128,128,0),
							RGB(0,0,128), RGB(128,0,128), RGB(0,128,128), RGB(128,128,128),
							RGB(192,192,192), RGB(255,0,0), RGB(0,255,0), RGB(255,255,0),
							RGB(0,0,255), RGB(255,0,255), RGB(0,255,255), RGB(255,255,255)
};

void CMyFontDialog::OnFontChange() {
	extern BOOL bLOGFONTToCHARFORMAT(LOGFONT *pLogFont, COLORREF crColor, DWORD dwMask, CHARFORMAT *pCharFormat);
	CHARFORMAT cf;
	LOGFONT lf;
	
	TRACE("Calling OnFontChange\n");
	// dialog box needs to process message so we can do a get current font
	const MSG *msg = GetCurrentMessage();
	DefWindowProc(msg->message, msg->wParam, msg->lParam );
	if (!m_bFontCtlEvents) return;
	GetCurrentFont(&lf);

	DWORD mask = GetCharFormatMask();

	// now get color by looking at the color combo index
	COLORREF selClr = RGB(0, 0, 0);
	CComboBox *clrCombo = (CComboBox *) GetDlgItem(cmb4);
	if (clrCombo) {
		int clrIndex = clrCombo->GetCurSel();
		if (clrIndex >= 0) selClr = clrMenu[clrIndex];
	}

	// change color and font to charformat, and draw
	bLOGFONTToCHARFORMAT(&lf, selClr, mask, &cf);
	m_richPreview->SetSelectionCharFormat(cf);
}


void CMyFontDialog::OnStrikeoutChange() {
	CButton *button = (CButton *) GetDlgItem(chx1);
	button->SetCheck(! button->GetCheck());
	OnFontChange();
}

void CMyFontDialog::OnUnderlineChange() {
	CButton *button = (CButton *) GetDlgItem(chx2);
	button->SetCheck(! button->GetCheck());
	OnFontChange();
}


void CMyFontDialog::OnChangeMessageType() {
	TRACE("In OnChangeMessageType\n");
	CComboBox *msgCombo = (CComboBox *) GetDlgItem(IDC_MESSAGETYPE);
	int msgPos = msgCombo->GetCurSel();
	if (msgPos == 0) {
		m_richPreview->SetSel(mesgStarts[0], mesgEnds[nLines-1]);
	} else if (msgPos > 0) {
		// first find message type
		int mType = mesgDropdown[msgPos];
		// now map it to a message line
		int line = -1;
		for (int i = 0; i < nLines; i++) {
			if (mesgTypes[i] == mType) {
				line = i;
				break;
			}
		}
		if (line >= 0) {
			m_richPreview->SetSel(mesgStarts[line], mesgEnds[line]);
		}
	}
}

BOOL bCHARFORMATToLOGFONT(CHARFORMAT *pCharFormat, DWORD dwMask, LOGFONT *pLogFont) {
	ASSERT(pLogFont && pCharFormat);

	if (!pLogFont || !pCharFormat)
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	ZeroMemory(pLogFont, sizeof(LOGFONT));

	if ((pCharFormat->dwMask & CFM_BOLD) && (pCharFormat->dwEffects & CFE_BOLD))
		pLogFont->lfWeight = 700;

	if ((pCharFormat->dwMask & CFM_ITALIC) && (pCharFormat->dwEffects & CFE_ITALIC))
		pLogFont->lfItalic = TRUE;

	if ((pCharFormat->dwMask & CFM_UNDERLINE) && (pCharFormat->dwEffects & CFE_UNDERLINE))
		pLogFont->lfUnderline = TRUE;

	if ((pCharFormat->dwMask & CFM_STRIKEOUT) && (pCharFormat->dwEffects & CFE_STRIKEOUT))
		pLogFont->lfStrikeOut = TRUE;

	if (pCharFormat->dwMask & CFM_SIZE) {
		// convert from twips to pixels
		HDC hDC = ::GetDC(NULL);
		if (hDC) {
			pLogFont->lfHeight = pCharFormat->yHeight * GetDeviceCaps(hDC, LOGPIXELSY) / 1440;
			::ReleaseDC(NULL, hDC);
		} 
	}

	pLogFont->lfOutPrecision = 1; //OUT_DEFAULT_PRECIS;
	pLogFont->lfClipPrecision = CLIP_DFA_OVERRIDE;
	pLogFont->lfQuality = 1; //DEFAULT_QUALITY;
	pLogFont->lfCharSet = pCharFormat->bCharSet;
	pLogFont->lfPitchAndFamily = 34; //pCharFormat->bPitchAndFamily;

	if (pCharFormat->dwMask & CFM_FACE)
		_tcscpy(pLogFont->lfFaceName, pCharFormat->szFaceName);

	return TRUE;
}

void CMyFontDialog::HandleSelection (NMHDR *pNotifyStruct, LRESULT *result ) {
	*result = 0;
	if (m_bClosing) return;
	SELCHANGE *lpSelChange = (SELCHANGE *) pNotifyStruct;
	AtomizeSelection(lpSelChange);
	SyncFontControls(lpSelChange);
	SyncMessageType(lpSelChange);
}


void CMyFontDialog::AtomizeSelection(SELCHANGE *lpSelChange) {
	int lower, upper;
	GetUpperAndLower(lpSelChange, lower, upper);
	if (lower >= 0 && upper >= 0) {
		m_richPreview->SetSel(mesgStarts[lower], mesgEnds[upper]);
	}
}


void CMyFontDialog::SyncFontControls (SELCHANGE *lpSelChange)
{
	if (m_bClosing)
		return;  // don't do this when we're closing the dialog and retrieving charformat info

	CHARFORMAT	cf;
	LOGFONT		lf;
	DWORD		dwMaskConsistent = m_richPreview->GetSelectionCharFormat(cf);

	TRACE("Syncing font controls\n");
	bCHARFORMATToLOGFONT(&cf, cf.dwMask, &lf);
	m_bFontCtlEvents = FALSE;
//	SendMessage(WM_CHOOSEFONT_SETFLAGS, 0, (DWORD) CF_INITTOLOGFONTSTRUCT);
	SendMessage(WM_CHOOSEFONT_SETLOGFONT, 0, (DWORD)(&lf));
	SendMessage(WM_CHOOSEFONT_GETLOGFONT, 0, (DWORD)(&lf));
	TRACE("This font is %s\n", lf.lfFaceName);
//	GetCurrentFont(&lf);
	m_bFontCtlEvents = TRUE;

	CComboBox *faceCombo   = (CComboBox *) GetDlgItem(cmb1);
	CComboBox *styleCombo  = (CComboBox *) GetDlgItem(cmb2);
	CComboBox *pointCombo  = (CComboBox *) GetDlgItem(cmb3);
	CComboBox *clrCombo    = (CComboBox *) GetDlgItem(cmb4);
	CComboBox *scriptCombo = (CComboBox *) GetDlgItem(cmb5);
	CButton	*strikeButton  = (CButton *) GetDlgItem(chx1);
	CButton *underButton   = (CButton *) GetDlgItem(chx2);

	if (dwMaskConsistent & CFM_FACE)
		faceCombo->SelectString(-1, cf.szFaceName);
	else
		faceCombo->SetCurSel(-1);

	SendMessage(WM_CHOOSEFONT_GETLOGFONT, 0, (DWORD)(&lf));

	if (dwMaskConsistent & CFM_SIZE)
	{
		char numbuff[10];
		sprintf(numbuff, "%d", (int)(cf.yHeight/20.0 + 0.5));
		pointCombo->SelectString(-1, numbuff);
	}
	else
		pointCombo->SetCurSel(-1);

	if (!(dwMaskConsistent & CFM_ITALIC) || !(dwMaskConsistent & CFM_BOLD)) 
		styleCombo->SetCurSel(-1);

	if (!(dwMaskConsistent & CFM_CHARSET))
		scriptCombo->SetCurSel(-1);

	if (dwMaskConsistent & CFM_COLOR)
	{
		int clrIndex = -1;
		for (int i = 0; i < sizeof(clrMenu) / sizeof(COLORREF); i++)
		{
			if (clrMenu[i] == cf.crTextColor)
			{
				clrIndex = i;
				break;
			}
		}
		clrCombo->SetCurSel(clrIndex);
	}
	else
		clrCombo->SetCurSel(-1);

	if (dwMaskConsistent & CFM_STRIKEOUT)
		strikeButton->SetCheck((cf.dwEffects & CFE_STRIKEOUT) ? 1 : 0);
	else
		strikeButton->SetCheck(2);  // indeterminate

	if (dwMaskConsistent & CFM_UNDERLINE)
		underButton->SetCheck((cf.dwEffects & CFE_UNDERLINE) ? 1 : 0);
	else
		underButton->SetCheck(2);

//	PostMessage(WM_TEST, 0, 0);
//	LOGFONT lf;
//	SendMessage(WM_CHOOSEFONT_GETLOGFONT, 0, (DWORD)(&lf));
//	TRACE("This font is %s\n", lf.lfFaceName);
}


void CMyFontDialog::GetUpperAndLower(SELCHANGE *lpSelChange, int &lower, int &upper) {
	lower = upper = -1;
	for (int i = 0; i < nLines; i++) {
		if (lpSelChange->chrg.cpMax >= mesgStarts[i] && lpSelChange->chrg.cpMax <= mesgEnds[i])
			upper = i;
		if (lpSelChange->chrg.cpMin >= mesgStarts[i] && lpSelChange->chrg.cpMin <= mesgEnds[i])
			lower = i;
	}
}


void CMyFontDialog::SyncMessageType(SELCHANGE *lpSelChange) {
	int lower, upper;

	GetUpperAndLower(lpSelChange, lower, upper);

	CComboBox *mtypes = (CComboBox *)GetDlgItem(IDC_MESSAGETYPE);
	if (lower >= 0 && lower == upper) SetMessageType(mesgTypes[lower]);
	else if (lower == 0 && upper == nLines-1) SetMessageType(ALL_MESSAGETYPE);
	else SetMessageType(-1);
}


DWORD CMyFontDialog::GetCharFormatMask() {
	DWORD dwMask = 0;
	if (((CComboBox *) GetDlgItem(cmb1))->GetCurSel() >= 0)
		dwMask |= CFM_FACE;
	if (((CComboBox *) GetDlgItem(cmb2))->GetCurSel() >= 0)
		dwMask |= (CFM_ITALIC | CFM_BOLD);
	if (((CComboBox *) GetDlgItem(cmb3))->GetCurSel() >= 0)
		dwMask |= CFM_SIZE;
	if (((CComboBox *) GetDlgItem(cmb4))->GetCurSel() >= 0)
		dwMask |= CFM_COLOR;
	if (((CComboBox *) GetDlgItem(cmb5))->GetCurSel() >= 0)
		dwMask |= CFM_CHARSET;    // why isn't this in the docs? just the .h file
	if ((((CButton *) GetDlgItem(chx1))->GetState() & 0x3) != 2)
		dwMask |= CFM_STRIKEOUT;
	if ((((CButton *) GetDlgItem(chx2))->GetState() & 0x3) != 2)
		dwMask |= CFM_UNDERLINE;
	return dwMask;
}


void CMyFontDialog::OnOK() {
	m_bClosing = TRUE;
	for (int i = 0; i < nLines; i++) {
		CHARFORMAT ch;
		m_richPreview->SetSel(mesgStarts[i], mesgEnds[i]);
		m_richPreview->GetSelectionCharFormat(ch);
		m_cfArray[mesgTypes[i]] = ch;
	}
	m_bCfInitialized = TRUE;

	CFontDialog::OnOK();
}


void CMyFontDialog::OnDestroy() 
{
	CFontDialog::OnDestroy();
	
	m_richCore->DetachTextViewHWnd();
	delete m_richCore;
	delete m_richPreview;
	if (m_hCursorHand) ::DeleteObject(m_hCursorHand);
}


BOOL CMyFontDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (pWnd == m_richPreview && nHitTest == HTCLIENT) {
		::SetCursor(m_hCursorHand);
		return TRUE;
	}
	return CFontDialog::OnSetCursor(pWnd, nHitTest, message);
}
