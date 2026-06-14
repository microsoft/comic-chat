// MyFontDg.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "textcore.h"
#include "txtfntdg.h"
#include "dlgs.h"
#include "tvres.h"
#include "setupdlg.h"

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

static int mesgStarts[NFONTS];
static int mesgEnds[NFONTS];
static BYTE mesgTypes[NFONTS];
// For each item in the message type dropdown, mesgDropdown provides the index into the charformat array.
// Note that the first type, ALL_MESSAGETYPE, doesn't have a corresponding charformat.
static BYTE mesgDropdown[] = {ALL_MESSAGETYPE, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17 }; // first entry has no meaning
static int nLines;


/////////////////////////////////////////////////////////////////////////////
// CMyFontDialog

IMPLEMENT_DYNAMIC(CMyFontDialog, CFontDialog)

CMyFontDialog::CMyFontDialog(LPLOGFONT lplfInitial, DWORD dwFlags, CDC* pdcPrinter, CWnd* pParentWnd) : 
	CWin4FontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
{
	m_bClosing		= FALSE;
	m_bFontCtlEvents= TRUE;
	m_hCursorHand	= NULL;
	m_bDeleteFont	= FALSE;
	m_bSettingUI 	= FALSE;
	m_bSetFromCf    = FALSE;
}


CMyFontDialog::~CMyFontDialog()
{
	if (m_bDeleteFont)
		m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CMyFontDialog, CWin4FontDialog)
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


BOOL AddStringToCombo(const char *messageName, void *vpCombo)
{
	CComboBox *combo = (CComboBox *) vpCombo;
	combo->AddString(messageName);
	return FALSE;
}


BOOL CMyFontDialog::OnInitDialog() 
{
	ModifyStyleEx (0, WS_EX_CONTEXTHELP);
	ModifyStyle (0, DS_CONTEXTHELP);

	CWin4FontDialog::OnInitDialog();

	// fill the message type combo box
	CString strMessageTypes, strMessageTypes2;
	VERIFY(strMessageTypes.LoadString(IDS_MESSAGETYPES));
	VERIFY(strMessageTypes2.LoadString(IDS_MESSAGETYPES2));
	strMessageTypes += strMessageTypes2;
	BOOL bForPath(const char *szPath, BOOL soundFunc(const char *, void *), void *pvData);
	bForPath(strMessageTypes, AddStringToCombo, (void *)GetDlgItem(IDC_MESSAGETYPE));
	
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
	if (m_bSetFromCf)
	{
		UpdateFromCf ();
	}

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

BOOL CMyFontDialog::SetupRichPreview()
{
	CString		strEntry, strNick1, strNick2, strTmp;
	void		InitializeTextCore(CTextCore *, BOOL = FALSE, BOOL = FALSE);
	int			cbLen;
	int*		end = mesgEnds;
	BYTE*		mType = mesgTypes;

	m_richCore->dwClearTextViewBuffer(0);
	InitializeTextCore(m_richCore);
	strNick1 = GetMyName();
	if (strNick1.IsEmpty()) strNick1.LoadString(IDS_DEFAULT_NICK);
	strNick2.LoadString(IDS_SAMPLE_NICK);

	*mType++ = 0;
	m_richCore->iDisplayMemberStatus(strNick1, 0, mtJoin, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 1;
	strEntry.LoadString(IDS_SAMPLE_SEND);
	cbLen = strlen(strEntry);
	m_richCore->iDisplayMsgHeader(cbLen, strNick1, 0, "", 0, mtNormal, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 2;
	m_richCore->iDisplayMsgText(strEntry, 0, mtNormal, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 3;
	strEntry.LoadString(IDS_SAMPLE_WHISPER);
	cbLen = strlen(strEntry);
	m_richCore->iDisplayMsgHeader(cbLen, strNick1, 0, strNick2, 0, mtWhisper, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 4;
	m_richCore->iDisplayMsgText(strEntry, 0, mtWhisper, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 5;
	strEntry.LoadString(IDS_SAMPLE_THOUGHT);
	m_richCore->iDisplayMsgHeader(cbLen, strNick1, 0, "", 0, mtThought, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 6;
	m_richCore->iDisplayMsgText(strEntry, 0, mtThought, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 7;
	strEntry.LoadString(IDS_SAMPLE_ACTION);
	m_richCore->iDisplayAction(strNick1, 0, strEntry, 0, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 8;
	m_richCore->iDisplayInfo(strNick1, 0, strNick2, 0, NULL, 0, mtAliasChange, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	*mType++ = 9;
	m_richCore->iDisplayMemberStatus(strNick2, 0, mtLeave, msParticipant);
	*end++ = m_richPreview->GetTextLength();
	
	for (UINT uHighlights = 0; uHighlights < (NHIGHLIGHTEDFONTS/2); uHighlights++)
	{
		*mType++ = 10+uHighlights*2;
		strEntry.Format(IDS_SAMPLE_HIGHLIGHT, uHighlights+1);
		cbLen = strlen(strEntry);
		m_richCore->iDisplayMsgHeader(cbLen,
									  strNick2,
									  0,
									  "",
									  0,
									  mtNormal,
									  msParticipant,
									  NULL,
									  2*uHighlights);
		*end++ = m_richPreview->GetTextLength();

		*mType++ = 11+uHighlights*2;
		m_richCore->iDisplayMsgText(strEntry,			// szText
									0,					// dwTextLen
									mtNormal,			// MsgType
									msParticipant,		// MembFrom
									FALSE,				// bShowURLs
									FALSE,				// bInformFull
									FALSE,				// bAppend
									DEFAULT_INDENT,		// lIndent
									NULL,				// pCharFormat
									2*uHighlights+1);	// iHighlightIndex
		*end++ = m_richPreview->GetTextLength();
	}

	nLines = end - mesgEnds;

	mesgStarts[0] = 0;
	int nNewMessageDelta = (theApp.m_textSpacing == TEXT_VIEW_BLANK_NEVER) ? 1 : 2;
	int nHeaderBodyDelta = (theApp.m_flags1 & F1_HEADERSEPARATE) ? 1 : g_nHeaderTabLen;
	for (int i = 1; i < nLines; i++)
		switch (i)
		{
		case 2:
		case 4:
		case 6:
		case 11:
		case 13:
		case 15:
		case 17:
			mesgStarts[i] = mesgEnds[i-1] + nHeaderBodyDelta;
			break;
		default:
			mesgStarts[i] = mesgEnds[i-1] + nNewMessageDelta;
		}
	m_richPreview->SetSel(0, 0);			// move to top (autoscrolls there)
	m_richPreview->ShowWindow(SW_SHOWNA);	// due to textview bug, we need to make it visible again

	return TRUE;
}

void
CMyFontDialog::UpdateFromCf()
{
	m_bClosing = TRUE;
	for (int i = 0; i < nLines; i++)
	{
		m_richPreview->SetSel(mesgStarts[i], mesgEnds[i]);
		m_richPreview->SetSelectionCharFormat(m_cfArray[mesgTypes[i]]);
	}
	// This has caused the control to scroll. Scroll back to top, and clear the selection.
	m_richPreview->SetSel (0, 1);
	m_richPreview->Clear ();
	m_bClosing = FALSE;
}

int CMyFontDialog::GetMessageType()
{
	CComboBox *combo = (CComboBox *) GetDlgItem(IDC_MESSAGETYPE);
	int sel = combo->GetCurSel();
	return ((sel == CB_ERR) ? -1 : sel);
}


void CMyFontDialog::SetMessageType(int mType)
{
	int entry = -1;
	for (int i = 0; i < sizeof(mesgDropdown) / sizeof(BYTE); i++)
		if (mType == mesgDropdown[i])
		{
			entry = i;
			break;
		}

	CComboBox *combo = (CComboBox *) GetDlgItem(IDC_MESSAGETYPE);
	combo->SetCurSel(entry);
}


void CMyFontDialog::OnFontChange()
{
	TRACE("Calling OnFontChange\n");
	// dialog box needs to process message so we can do a get current font
	const MSG *msg = GetCurrentMessage();
	DefWindowProc(msg->message, msg->wParam, msg->lParam );
	if (!m_bFontCtlEvents || m_bSettingUI) return;

	CHARFORMAT cf;
	LOGFONT lf;

	GetCurrentFont(&lf);

	DWORD mask = GetCharFormatMask();

	// now get color by looking at the color combo index
	COLORREF selClr = RGB(0, 0, 0);
	CComboBox *clrCombo = (CComboBox *) GetDlgItem(cmb4);
	if (clrCombo) {
		int clrIndex = clrCombo->GetCurSel();
		if (clrIndex >= 0) selClr = clrCombo->GetItemData (clrIndex);
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


void CMyFontDialog::OnChangeMessageType()
{
	TRACE("In OnChangeMessageType\n");
	CComboBox*	msgCombo = (CComboBox *) GetDlgItem(IDC_MESSAGETYPE);
	int			msgPos = msgCombo->GetCurSel();

	if (msgPos == 0)
		m_richPreview->SetSel(mesgStarts[0], mesgEnds[nLines-1]);
	else
		if (msgPos > 0)
		{
			// first find message type
			int mType = mesgDropdown[msgPos];
			// now map it to a message line
			int line = -1;
			for (int i = 0; i < nLines; i++)
				if (mesgTypes[i] == mType)
				{
					line = i;
					break;
				}
			if (line >= 0)
				m_richPreview->SetSel(mesgStarts[line], mesgEnds[line]);
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


void CMyFontDialog::HandleSelection (NMHDR *pNotifyStruct, LRESULT *result)
{
	*result = 0;
	if (m_bClosing)
		return;
	SELCHANGE *lpSelChange = (SELCHANGE *) pNotifyStruct;
	AtomizeSelection(lpSelChange);
	SyncFontControls(lpSelChange);
	SyncMessageType(lpSelChange);
}


void CMyFontDialog::AtomizeSelection(SELCHANGE *lpSelChange)
{
	int lower, upper;
	GetUpperAndLower(lpSelChange, lower, upper);
	if (lower >= 0 && upper >= 0)
		m_richPreview->SetSel(mesgStarts[lower], mesgEnds[upper]);
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
	SetFontToUI (&lf);
	GetFontFromUI (&lf);
	TRACE("This font is %s\n", lf.lfFaceName);
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

	GetFontFromUI (&lf);

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
		for (int i = clrCombo->GetCount () - 1; i >= 0; i--)
		{
			if (clrCombo->GetItemData (i) == cf.crTextColor)
			{
				break;
			}
		}
		clrCombo->SetCurSel(i);
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
}


void CMyFontDialog::GetUpperAndLower(SELCHANGE *lpSelChange, int &lower, int &upper)
{
	lower = upper = -1;
	for (int i = 0; i < nLines; i++)
	{
		if (lpSelChange->chrg.cpMax >= mesgStarts[i] && lpSelChange->chrg.cpMax <= mesgEnds[i])
			upper = i;
		if (lpSelChange->chrg.cpMin >= mesgStarts[i] && lpSelChange->chrg.cpMin <= mesgEnds[i])
			lower = i;
	}
}


void CMyFontDialog::SyncMessageType(SELCHANGE *lpSelChange)
{
	int lower, upper;

	GetUpperAndLower(lpSelChange, lower, upper);

	CComboBox *mtypes = (CComboBox *)GetDlgItem(IDC_MESSAGETYPE);
	if (lower >= 0 && lower == upper)
		SetMessageType(mesgTypes[lower]);
	else
		if (lower == 0 && upper == nLines-1)
			SetMessageType(ALL_MESSAGETYPE);
		else
			SetMessageType(-1);
}


DWORD CMyFontDialog::GetCharFormatMask()
{
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


void CMyFontDialog::OnOK()
{
	m_bClosing = TRUE;
	for (int i = 0; i < nLines; i++)
	{
		CHARFORMAT ch;
		m_richPreview->SetSel(mesgStarts[i], mesgEnds[i]);
		m_richPreview->GetSelectionCharFormat(ch);
		m_cfArray[mesgTypes[i]] = ch;
	}
	// m_bCfInitialized = m_bCfHLInitialized = TRUE;

	CWin4FontDialog::OnOK();
}


void CMyFontDialog::OnDestroy() 
{
	CWin4FontDialog::OnDestroy();
	
	m_richCore->DetachTextViewHWnd();
	delete m_richCore;
	delete m_richPreview;
	if (m_hCursorHand)
		::DeleteObject(m_hCursorHand);
}


BOOL CMyFontDialog::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message) 
{
	if (pWnd == m_richPreview && nHitTest == HTCLIENT)
	{
		::SetCursor(m_hCursorHand);
		return TRUE;
	}
	return CWin4FontDialog::OnSetCursor(pWnd, nHitTest, message);
}

// These are added for Win95/WinNT compatibility. Win95's font dialog supports
// messages that are not in WinNT's font dialog. If WinNT ever adds them, we
// can re-enable the code.

BOOL CMyFontDialog::SupportsFontMessages()
{
	// NOTE: Even if the platform does not support the messages, do not return
	// FALSE if the code to manually extract information from the dialog does not
	// work on the platform.

	static BOOL bSupports = (BOOL)-1;
	if (bSupports == (BOOL)-1)
	{
		OSVERSIONINFO verinfo;
		verinfo.dwOSVersionInfoSize = sizeof(verinfo);
		GetVersionEx (&verinfo);
		bSupports = verinfo.dwPlatformId != VER_PLATFORM_WIN32_NT;
	}
	return bSupports;
}

void CopyLogFontWToA(
LPLOGFONTA lplfDest,
LPLOGFONTW lplfSrc)
{
	memcpy (lplfDest, lplfSrc, offsetof(LOGFONTW, lfFaceName));
	wcstombs (lplfDest->lfFaceName, lplfSrc->lfFaceName, sizeof(lplfDest->lfFaceName));
}

void CMyFontDialog::GetFontFromUI(
LPLOGFONT lplf)
{
	if (SupportsFontMessages ())
	{
		CWin4FontDialog::GetCurrentFont (lplf);
	}
	else if (GetSafeHwnd () == NULL)
	{
		*lplf = m_lf;
	}
	else
	{
		char szBuf[64];

		ZeroMemory (lplf, sizeof(*lplf));
		CComboBox * pCombo;
		int nSel;

		// Typeface
		pCombo = (CComboBox *)GetDlgItem (cmb1);
		pCombo->GetWindowText (lplf->lfFaceName, sizeof(lplf->lfFaceName));

		// Font style. The font style combo extra data contains a pointer to a structure,
		// whose first member is a pointer to the LOGFONT structure.
		pCombo = (CComboBox *)GetDlgItem (cmb2);
		nSel = pCombo->GetCurSel ();
		if (nSel != CB_ERR)
		{
			LPLOGFONTW* ppFont = (LPLOGFONTW*)pCombo->GetItemData (nSel);
			if (!IsBadReadPtr (ppFont, sizeof(ppFont)) && !IsBadReadPtr (*ppFont, sizeof(*ppFont)))
			{
				CopyLogFontWToA (lplf, *ppFont);
			}
		}

		// Point size
		pCombo = (CComboBox *)GetDlgItem (cmb3);
		szBuf[0] = '\0';
		pCombo->GetWindowText (szBuf, sizeof(szBuf));
		if (szBuf[0] != '\0')
		{
			CWindowDC dc (this);
			lplf->lfHeight = MulDiv (atoi (szBuf), dc.GetDeviceCaps(LOGPIXELSY), 72);
		}

		// Strikeout/underline
		if (IsDlgButtonChecked (chx1) == 1)
		{
			lplf->lfStrikeOut = 0xff;
		}
		if (IsDlgButtonChecked (chx2) == 1)
		{
			lplf->lfUnderline = 0xff;
		}
		
		// Color is handled elsewhere.
		//

		// Character set. The combo extra data contains a pointer to a structure, 
		// whose second member is the charset.
		pCombo = (CComboBox *)GetDlgItem (cmb5);
		nSel = pCombo->GetCurSel ();
		if (nSel != CB_ERR)
		{
			LPDWORD pdw = (LPDWORD)pCombo->GetItemData (nSel);
			if (!IsBadReadPtr (pdw, 2 * sizeof(DWORD)))
			{
				lplf->lfCharSet = (BYTE)pdw[1];
			}
		}

		m_lf = *lplf;
	}
}

void CMyFontDialog::SetFontToUI(
LPLOGFONT lplf)
{
	m_bSettingUI = TRUE;
	if (SupportsFontMessages ())
	{
		SendMessage(WM_CHOOSEFONT_SETLOGFONT, 0, (DWORD)lplf);
	}
	else
	{
		CComboBox* pCombo;
		int		   i;

		// Set the typeface.
		pCombo = (CComboBox *)GetDlgItem (cmb1);
		pCombo->SelectString (-1, lplf->lfFaceName);
		SendMessage (WM_COMMAND, MAKELONG(cmb1, CBN_SELCHANGE), (LPARAM)(pCombo->m_hWnd));

		// Set the font style. We have to walk items, looking for the right styles.
		pCombo = (CComboBox *)GetDlgItem (cmb2);
		for (i = pCombo->GetCount () - 1; i > 0; i--)
		{
			LPLOGFONTW* ppFont = (LPLOGFONTW*)pCombo->GetItemData (i);
			if (!IsBadReadPtr (ppFont, sizeof(ppFont)) && 
				!IsBadReadPtr (*ppFont, sizeof(*ppFont)) &&
				(*ppFont)->lfWeight == lplf->lfWeight &&
				(*ppFont)->lfItalic == lplf->lfItalic)
			{
				break;
			}
		}
		pCombo->SetCurSel (i);

		// Set the size.
		char szSize[24];
		int nHeight = (lplf->lfHeight < 0) ? -lplf->lfHeight : lplf->lfHeight; 
		CWindowDC dc (this);
		wsprintf (szSize, "%u", (UINT)MulDiv (nHeight, 72, dc.GetDeviceCaps (LOGPIXELSY)));
		pCombo = (CComboBox *)GetDlgItem (cmb3);
		pCombo->SelectString (-1, szSize);
		pCombo->SetWindowText (szSize);

		// Select strikeout and underline.

		CheckDlgButton (chx1, lplf->lfStrikeOut != 0 ? 1 : 0);
		CheckDlgButton (chx2, lplf->lfUnderline != 0 ? 1 : 0);

		// Character set.
		pCombo = (CComboBox *)GetDlgItem (cmb5);
		for (i = pCombo->GetCount () - 1; i > 0; i--)
		{
			LPDWORD pdw = (LPDWORD)pCombo->GetItemData (i);
			if (!IsBadReadPtr (pdw, 2 * sizeof(DWORD)) && 
				(BYTE)(pdw[1]) == lplf->lfCharSet)
			{
				break;
			}
		}
		pCombo->SetCurSel (i);
		SendMessage (WM_COMMAND, MAKELONG(cmb5, CBN_SELCHANGE), (LPARAM)(pCombo->m_hWnd));
	}
	m_lf = *lplf;
	m_bSettingUI = FALSE;
}
