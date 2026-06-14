// saywnd.cpp : implementation file
//

#include "stdafx.h"
#include "userinfo.h"
#include "memblst.h"
#include "chat.h"
#include "saywnd.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "textcore.h"
#include "textview.h"
#include "dib.h"
#include "pageview.h"
#include "ui.h"
#include "sounddlg.h"
#include "format.h"
#include "colordlg.h"
#include "protsupp.h"
#include "whisprbx.h"
#include <imm.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Parameters

// sizes for toolbar button image
#define cxToolBar		17
#define cyToolBar		17

// size for say bar
#define BUTTONSIZE		24			// includes margin... toolbar size = m_nButtons * BUTTONSIZE


/////////////////////////////////////////////////////////////////////////////
// External statics

extern CChatApp theApp;

#define WM_SAYSCROLLKEY		(WM_USER + 300)

/////////////////////////////////////////////////////////////////////////////
// statics w/ local scope
CAccelTable accelWhisper(IDR_WHISPERACCEL);

static UINT BASED_CODE balloons_say[] =
{
	ID_ACTIONS_SAY,
	ID_ACTIONS_THINK,
	ID_ACTIONS_WHISPER,
	ID_SEND_ACTION,
	ID_WHISPER_ACTION,
	ID_PLAY_SOUND,
	ID_WHISPER_SOUND
};


/////////////////////////////////////////////////////////////////////////////
// CSayCtrl

IMPLEMENT_DYNCREATE(CSayCtrl, CRtfCtrl)

CSayCtrl::CSayCtrl()
{
	m_bWhisperSay = FALSE;
	m_bRerouteMenuInit = TRUE;
	m_nPopupMenuIndex = 0;
	m_bEnChangeFreeze = FALSE;
}


BOOL CSayCtrl::IsEmpty()
{
	// if no text, then empty
	if (SendMessage(WM_GETTEXTLENGTH, 0, 0) == 0)
		return TRUE;

	// get text
	CString str;
	GetWindowText(str);

	// if all white space, then empty
	str.TrimRight();
	return str.IsEmpty();
}


BOOL CSayCtrl::bEmptyAndIndent()
{
	BOOL bPrev = m_bEnChangeFreeze;
	m_bEnChangeFreeze = TRUE;
	SetWindowText("");
	BOOL bRet = bSetIndent(72);	// Set left indentation, 72 is magic number used in history text window too
	EmptyUndoBuffer();			// Fix 4409
	m_bEnChangeFreeze = bPrev;
	return bRet;
}


void CSayCtrl::ValidateIMEEntry()
{
	HIMC hIMC = ImmGetContext(m_hWnd);
	if (hIMC)
		ImmNotifyIME(hIMC, NI_COMPOSITIONSTR, CPS_COMPLETE, 0);
	ImmReleaseContext(m_hWnd, hIMC);
}


BEGIN_MESSAGE_MAP(CSayCtrl, CRtfCtrl)
	//{{AFX_MSG_MAP(CSayCtrl)
	ON_WM_KEYDOWN()
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_MAXTEXT, OnMaxtext)
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	ON_MESSAGE_VOID(WM_PASTE, OnPaste)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL bLegalToSend(BOOL bPrivMsg = FALSE)
{
	CRoomInfo *pRoomInfo = currentRoom ? currentRoom : GetDefaultProto();
	ASSERT(pRoomInfo);
	int cxStatus = pRoomInfo->GetConnectionStatus();

	if ((cxStatus == CX_NOCHANNEL && !bPrivMsg) || cxStatus == CX_CONNECTING)
	{
		if (pRoomInfo->m_doc && ((CChatDoc*) pRoomInfo->m_doc)->m_bStatusView)
			AfxMessageBox(IDS_ILLEGAL_NOSLASH);
		else
			AfxMessageBox(IDS_ILLEGAL_TO_SEND);
		return FALSE;
	}
	else
	{
		if (bPrivMsg)
		{
			if (cxStatus == CX_DISCONNECTED || cxStatus == CX_CONNECTING)
			{
				AfxMessageBox(IDS_MUSTBE_CONNECTED);
				return FALSE;
			}
			else
				return TRUE;
		}
		else
			return TRUE;
	}
}



/////////////////////////////////////////////////////////////////////////////
// CSayCtrl message handlers

#define TIMERID 1
#define TIMEOUT 30000

void CSayCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	UINT nCmd;
	if (m_bWhisperSay && (nCmd = accelWhisper.Lookup (WM_KEYDOWN, nChar, nRepCnt, nFlags)) != 0)
		GetParent()->SendMessage (WM_COMMAND, nCmd);
	if (nChar == VK_PRIOR || nChar == VK_NEXT)
		GetParent()->SendMessage (WM_SAYSCROLLKEY, (WPARAM)nChar, MAKELPARAM((WORD)nRepCnt, (WORD)nFlags));
	else
		CRtfCtrl::OnKeyDown (nChar, nRepCnt, nFlags);
}


void CSayCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CString str;
	BOOL	bCanDance();

	if (nChar == CHAR_RETURN)
	{
		// get contents, send them, then zap
		GetWindowText(str);
		BOOL bZap, bIsEmpty = str.IsEmpty();
		BOOL bCommand = !bIsEmpty && str[0] == '/';

		if ((bIsEmpty || !bCommand) && !bLegalToSend(m_bWhisperSay)) 
			return;

		if (bIsEmpty)
		{	//  transmit a signal that only character coming over
			if (!m_bWhisperSay && GetChatDoc() && GetChatDoc()->m_bComicView && GetSendComicsData() && bCanDance())
				str = "<Chr>";
			else
			{
				if (m_bWhisperSay)
					AfxMessageBox(IDS_SENDENABLED);
				return;					//  null sends prohibited in text mode, or whisper box
			}
		}

		CDWordArray* prgdwFormatting = m_pFont ? PRGDWGetFormatting(this, m_pFont, m_crTextColor) : NULL;

		if (!bIsEmpty && m_pDosKey)
			m_pDosKey->bAppendEntry(str, prgdwFormatting);

		bEmptyAndIndent();

		// Let UI object know
		if (m_bWhisperSay) 
			bZap = bWhisperInBox("", str, prgdwFormatting, BM_WHISPER);
		else 
			bZap = bChatSendText(str, BM_SAY, TRUE, prgdwFormatting);

		//if (bZap)	// Attention! window may have been destroyed by a /part or /join command!
		//	bEmptyAndIndent();

		FreeAndNullFormatting(&prgdwFormatting);
		return;
	}

	BOOL bCtrlDown = GetKeyState(VK_CONTROL) & 0x8000;

	if (bCtrlDown)
	{
		if (nChar == CTRL_Q && !m_bWhisperSay)		// REGISB can never happen because Ctrl Q brings up options dialog
		{		// break the panel
			str = "<Brk>";
			bChatSendText(str, BM_SAY);
			TRACE("Manual break...\n");
			return;
		} 
	
		if (nChar == CTRL_L)
		{		// clear the line
			ValidateIMEEntry();
			bEmptyAndIndent();
			ChatPreSendText(CString(""));
			return;
		}
	}
	else
	{
		if (nChar == VK_TAB)
		{
			BOOL bShifted = GetKeyState(VK_SHIFT) & 0x8000;

			if (m_bWhisperSay)
			{
				CWnd *pWnd = GetParent()->GetParent();
				ASSERT(pWnd);
				pWnd->SendMessage(WM_NEXTDLGCTL, (WPARAM) bShifted, (LPARAM) FALSE);
			} 
			else
			{
				GetChatDoc ()->CycleFocus (CHATFOCUS_INPUTWND, bShifted);
			}
			return;
		}
	}

	if (!bCtrlDown && _istspace(nChar) && LineLength() == 0) 
		return; // don't let people start out w/ spaces or tabs

	CRtfCtrl::OnChar(nChar, nRepCnt, nFlags);

	// Get the text, and do a PreSay (since it might affect the character's pose)
	//GetWindowText(str);
	//ChatPreSendText(str);

	if (str.GetLength() <= 1)
		bSetIndent(72); // REGISB 01/02/97  might find a better solution
}


extern HWND hgPrevFocus;
void CSayCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	ValidateIMEEntry();

	CRichEditCtrl::OnKillFocus(pNewWnd);
	
	hgPrevFocus = m_hWnd;
	
}


void CSayCtrl::OnSetFocus(CWnd* pOldWnd) 
{
	ValidateIMEEntry();

	CRichEditCtrl::OnSetFocus(pOldWnd);
}


void CSayCtrl::OnMaxtext() 
{
	AfxMessageBox(IDS_TOOMUCHTEXT);
}

void CSayCtrl::OnPaste()
{
	Default ();

	// This is done to remove trailing lines if they are blank
	int nLines = SendMessage (EM_GETLINECOUNT);
	BOOL bContinue = TRUE;
	while (bContinue)
	{
		bContinue = FALSE;
		if (nLines > 1)
		{
			union
			{
				WORD wLen;
				char szLine[8];
			} LineData;
			LineData.wLen = sizeof(LineData);
			int nChars = SendMessage (EM_GETLINE, nLines - 1, (LPARAM)&LineData);
			if (nChars == 2 && LineData.szLine[0] == '\x0d' && LineData.szLine[1] == '\x0a')
			{
				CHARRANGE charrangeSave, charrange;
				SendMessage (EM_EXGETSEL, 0, (LPARAM)&charrangeSave);
				charrange.cpMin = SendMessage (EM_LINEINDEX, nLines - 1) - 1;
				if (charrange.cpMin != -1L)
				{
					charrange.cpMax = charrange.cpMin + 2;
					if ((long)SendMessage (EM_EXSETSEL, 0, (LPARAM)&charrange) > 0)
						SendMessage (WM_CLEAR);
					SendMessage (EM_EXSETSEL, 0, (LPARAM)&charrangeSave);
					// Continuing depends on whether all this is working or not 
					// (don't want to get into loops or something)
					bContinue = nLines > 2 && (int)SendMessage (EM_GETLINECOUNT) == nLines - 1;
					nLines--;
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// CSayWnd

IMPLEMENT_DYNCREATE(CSayWnd, CFrameWnd)

CSayWnd::CSayWnd()
{
	m_bWhisperSay	= FALSE;
	m_fontText		= NULL;

	SetToolBarInfo(FALSE, m_dwDefaultButtons);

	m_wndSayCtrl.SetDosKey(&theApp.m_doskeyMain);
}


CSayWnd::CSayWnd(BOOL bWhisperSay, DWORD dwButtons)
{
	m_bWhisperSay				= bWhisperSay;
	m_wndSayCtrl.m_bWhisperSay	= bWhisperSay;
	m_wndSayCtrl.SetDosKey(bWhisperSay ? &theApp.m_doskeyWhisper : &theApp.m_doskeyMain);
	m_fontText					= NULL;

	SetToolBarInfo(bWhisperSay, dwButtons);
}

DWORD CSayWnd::m_dwDefaultButtons = 0;


CSayWnd::~CSayWnd()
{
	if (m_fontText)
		delete m_fontText;
}


void CSayWnd::SetToolBarInfo(BOOL bWhisperSay, DWORD dwButtons)
{
	m_dwButtons = dwButtons;
	m_cntBalloons = 0;

#ifdef CB32SUPPORT
	if (theApp.m_bDoCB32)
		dwButtons &= ~SB_SOUND;	// CB32 does not support sound
#endif CB32SUPPORT

	for (int i = 0; i < SAYNBUTTONS; i++)
		if (dwButtons & (1 << i))
			m_cntBalloons++;

	m_cxSayBar = m_cntBalloons * BUTTONSIZE;
}


void CSayWnd::SetFormattingToolBarInfo(CToolBarCtrl *pTBCtrl, INT nBoldID, INT nItalicID, INT nUnderlineID, INT nFixedPitchID, INT nSymbolID)
{
	ASSERT(pTBCtrl);
	m_wndSayCtrl.SetToolBarData(pTBCtrl, ID_SWITCHBOLD, ID_SWITCHITALIC, ID_SWITCHUNDERLINED, 
								ID_SWITCHFIXEDPITCH, ID_SWITCHSYMBOL);

	m_wndSayCtrl.MatchButtonsToSelection();
}


BEGIN_MESSAGE_MAP(CSayWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CSayWnd)
	ON_NOTIFY(EN_SELCHANGE, ID_SAYCTRL, OnSaySelChanged)
	ON_NOTIFY(EN_MSGFILTER, ID_SAYCTRL, OnSayFilter)
	ON_EN_CHANGE(ID_SAYCTRL, OnSayEditChanged)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_COMMAND(ID_ACTIONS_SAY, OnActionsSay)
	ON_COMMAND(ID_ACTIONS_THINK, OnActionsThink)
	ON_COMMAND(ID_ACTIONS_WHISPER, OnActionsWhisper)
	ON_COMMAND(ID_SEND_ACTION, OnSendAction)
	ON_COMMAND(ID_PLAY_SOUND, OnPlaySound)
	ON_COMMAND(ID_WHISPER_ACTION, OnSendAction)
	ON_COMMAND(ID_WHISPER_SOUND, OnPlaySound)
	ON_MESSAGE(WM_SAYSCROLLKEY, OnScrollKey)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSayWnd message handlers
void CSayWnd::OnSaySelChanged(NMHDR* pNMHDR, LRESULT* pLResult)
{
	if (m_wndSayCtrl.m_pTBCtrl)
	{
		// The input selection has changed - let's get the CHARFORMAT of the current selection
		CHARFORMAT cf;
		DWORD dwMaskConsistent = m_wndSayCtrl.GetSelectionCharFormat(cf);
    
		if (m_wndSayCtrl.m_nBoldID > 0)
			if ((dwMaskConsistent & CFM_BOLD) && (cf.dwEffects & CFE_BOLD))
				// press Bold button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nBoldID, TRUE);
			else
				// release Bold button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nBoldID, FALSE);
			
		if (m_wndSayCtrl.m_nItalicID > 0)
			if ((dwMaskConsistent & CFM_ITALIC) && (cf.dwEffects & CFE_ITALIC))
				// press Italic button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nItalicID, TRUE);
			else
				// release Italic button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nItalicID, FALSE);
			
		if (m_wndSayCtrl.m_nUnderlineID > 0)
			if ((dwMaskConsistent & CFM_UNDERLINE) && (cf.dwEffects & CFE_UNDERLINE))
				// press Underline button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nUnderlineID, TRUE);
			else
				// release Underline button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nUnderlineID, FALSE);
			
		if (m_wndSayCtrl.m_nFixedPitchID > 0)
			if ((dwMaskConsistent & CFM_FACE) && (FFixedPitchFont(cf.szFaceName) >= 0))		// REGISB: (cf.bPitchAndFamily & FIXED_PITCH) is buggy solution
				// press FixedPitch button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nFixedPitchID, TRUE);
			else
				// release FixedPitch button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nFixedPitchID, FALSE);

		if (m_wndSayCtrl.m_nSymbolID > 0)
			if ((dwMaskConsistent & CFM_FACE) && (FSymbolFont(cf.szFaceName) >= 0))
				// press Symbol button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nSymbolID, TRUE);
			else
				// release Symbol button
				m_wndSayCtrl.m_pTBCtrl->CheckButton(m_wndSayCtrl.m_nSymbolID, FALSE);
	}

    *pLResult = 0;
}


void CSayWnd::OnSayFilter(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct);

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_wndSayCtrl.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


void CSayWnd::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

	if (nType == SIZE_RESTORED)
	{
		// don't do during initialization
		if (::IsWindow(m_wndSayCtrl.m_hWnd))
		{
			// position and size gesture and entry window
			if (m_cntBalloons)
				m_wndSayBar.SetWindowPos(NULL,
										 cx - m_cxSayBar-6,					// The 6 was added to move the grabspace of the toolbar
																			// under the sayctrl
										 m_bWhisperSay ? -2 : -3,			// same with the three
										 m_cxSayBar+12,						// previously 0
										 cyToolBar+12+(m_bWhisperSay?3:0),	// previously 0
										 SWP_NOZORDER );					// this used to also have SWP_NORESIZE | 

			m_wndSayCtrl.SetWindowPos(&wndTop,							// this was previously NULL
										0,
										0,
										cx - m_cxSayBar,
										cy,								// REGISB, 09/02/97: This was previously cyToolBar+6
										0);								// This was previously SWP_NOZORDER

			m_wndSayCtrl.InvalidateRect(NULL, TRUE);
		}
	}
}


BOOL CSayWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~WS_BORDER;
	return CWnd::PreCreateWindow(cs);
}


BOOL CSayWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	DWORD	dwStyle, dwEventMask;

	// create log window (don't worry about size)
	dwStyle = 	WS_VISIBLE 		| 
				WS_CLIPSIBLINGS	|
				ES_MULTILINE 	| 
				ES_AUTOVSCROLL  |
				ES_AUTOHSCROLL	|
				ES_NOHIDESEL;
	if (m_bWhisperSay)
		dwStyle |= WS_BORDER;  // REGISB: border only for whisper box since we use CRichEditCtl instead of CEdit

	VERIFY(m_wndSayCtrl.Create(dwStyle, 
							   CRect(CPoint(lpcs->x, lpcs->y), CSize(lpcs->cx, lpcs->cy)), 
							   this, 
							   ID_SAYCTRL));
	// Need to add the EN_SELCHANGE notification to the dwEventMask of the rich text control
	dwEventMask = (DWORD) m_wndSayCtrl.GetEventMask();
	m_wndSayCtrl.SetEventMask(dwEventMask | ENM_SELCHANGE | ENM_MOUSEEVENTS | ENM_CHANGE);

	// set font
	if (theApp.m_bComicView) 
		SetFont(theApp.m_comicsFont);
	else 
		SetFont(theApp.m_textFont);

	// Set color to default Windows Text color
	m_wndSayCtrl.bSetTextColor(GetSysColor(COLOR_WINDOWTEXT));

	// Set left indentation
	m_wndSayCtrl.bEmptyAndIndent();

	// set max chars
	// REGISB: this needs to be changed: need to limit in function of the server's limit!
	m_wndSayCtrl.LimitText(MAX_INPUTLEN);		// reasonable message size

	// force focus here
	m_wndSayCtrl.SetFocus();

	// create toolbars
	CSize sizeButton(cxToolBar+7, cyToolBar+6+(m_bWhisperSay?3:0));
	CSize sizeImage(cxToolBar, cyToolBar);

	// Create say/think/emote bar
	if (m_cntBalloons) {
		if (!m_wndSayBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_NOALIGN, 1) ||
			!m_wndSayBar.LoadBitmap(IDB_SAY_BAR) ||
			!m_wndSayBar.SetButtons(NULL, m_cntBalloons))  {
			TRACE0("Failed to create balloons toolbar\n");
			return -1;      // fail to create
		}

		//m_wndSayBar.ModifyStyle(0, TBSTYLE_FLAT); // make it flat
		m_wndSayBar.SetSizes(sizeButton, sizeImage);

		int buttonPos = 0;
		for (int i = 0; i < SAYNBUTTONS; i++) {
			if ((1 << i) & m_dwButtons)
				m_wndSayBar.SetButtonInfo(buttonPos++, balloons_say[i], TBBS_BUTTON, i);
		}

		// Allow tooltips
		m_wndSayBar.SetBarStyle(m_wndSayBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

		// remove border
		m_wndSayBar.SetBarStyle(m_wndSayBar.GetBarStyle() & ~CBRS_BORDER_TOP );
	}


	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);

	return CFrameWnd::OnCreateClient(lpcs, NULL);
}


BOOL CSayWnd::SetFont(LOGFONT &logFont, BOOL bMatchButtonsToSelection /* = FALSE */) 
{
	if (!::IsWindow(m_wndSayCtrl.m_hWnd))
		return FALSE;

	LOGFONT lf = logFont;

	if (FFixedPitchFont(lf.lfFaceName) >= 0 || FSymbolFont(lf.lfFaceName) >= 0 || (lf.lfPitchAndFamily & FIXED_PITCH))
	{
		// use the default GUI face name
		strcpy(lf.lfFaceName, theApp.m_szGuiFaceName);
		lf.lfPitchAndFamily = theApp.m_lfGuiPitchAndFamily;
	}
	else
		lf.lfPitchAndFamily &= ~FIXED_PITCH;

	lf.lfWeight		= FW_REGULAR;
	lf.lfStrikeOut	= lf.lfUnderline = lf.lfItalic = FALSE;
	lf.lfHeight		= nFontHeight;

	MatchFont(lf);
	strcpy(logFont.lfFaceName, lf.lfFaceName);

	// delete if already assigned
	CFont *pOldFont = m_fontText;

	// create fonts
	m_fontText = new CFont;
	if(!m_fontText->CreateFontIndirect(&lf))
		return FALSE;

	m_wndSayCtrl.SetFont(m_fontText);
	m_wndSayCtrl.m_pFont = m_fontText;

	if (bMatchButtonsToSelection)
		m_wndSayCtrl.MatchButtonsToSelection();

	// Retrieve charset.  If used GetLogFont, could be unspecified, so select and test
	// REGISB: 12/08/97: GetTextCharset returns 0x00 instead of correct value for some languages
	CDC *pdc = m_wndSayCtrl.GetDC();
	if (pdc)
	{
		if ((theApp.m_charSet = GetTextCharset(pdc->m_hDC)) == ANSI_CHARSET)
			theApp.m_charSet = lf.lfCharSet;
		ReleaseDC(pdc);
	}

	if (pOldFont) 
		delete pOldFont;

	return TRUE;
}


void CSayWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CBrush * pbrBtnFace = CBrush::FromHandle (::GetSysColorBrush (COLOR_BTNFACE));
	RECT rect;
	GetClientRect (&rect);
	dc.FillRect (&rect, pbrBtnFace);
}


BOOL CSayWnd::IsEmpty()
{
	return m_wndSayCtrl.IsEmpty();
}


void CSayWnd::OnActionsWhisper() 
{
	CString			str;
	CDWordArray*	prgdwFormatting = NULL;
	BOOL			bZap;
	CChatDoc*		pDoc = GetChatDoc();

	// Ctrl+W in the Status Window should have no effect
	if (pDoc && pDoc->m_bStatusView)
		return;

	m_wndSayCtrl.ValidateIMEEntry();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if (!bLegalToSend(m_bWhisperSay))
		return;

	if (m_bWhisperSay)
	{	// need to handle whisperbox case separately
		if (str.IsEmpty())
		{
			AfxMessageBox(IDS_SENDENABLED);
			return;
		}

		prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

		m_wndSayCtrl.bEmptyAndIndent();
		bZap = bWhisperInBox("", str, prgdwFormatting, BM_WHISPER);	// ... for historical reasons
	}
	else
	{
		if (pDoc && CX_DISCONNECTED == pDoc->GetConnectionStatus())
		{
			AfxMessageBox(IDS_MUSTBE_CONNECTED);
			return;
		}

		extern void GetSelectedPuis(CPtrArray &);
		GetSelectedPuis(g_rgpuiWhisperees);

		if (str.IsEmpty() || g_rgpuiWhisperees.GetUpperBound() < 0)
		{
			AfxMessageBox(IDS_WHISPERENABLED);
			return;
		}

		prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

		m_wndSayCtrl.bEmptyAndIndent();
		bZap = bChatSendText(str, BM_WHISPER, TRUE, prgdwFormatting);
	}

	if (m_wndSayCtrl.m_pDosKey)
		m_wndSayCtrl.m_pDosKey->bAppendEntry(str, prgdwFormatting);

	//if (bZap)
	//	m_wndSayCtrl.bEmptyAndIndent();

	FreeAndNullFormatting(&prgdwFormatting);
}


void CSayWnd::OnActionsSay()
{
	CString str;

	m_wndSayCtrl.ValidateIMEEntry();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if(str.IsEmpty())
	{
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!bLegalToSend())
		return;

	CDWordArray* prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

	if (m_wndSayCtrl.m_pDosKey)
		m_wndSayCtrl.m_pDosKey->bAppendEntry(str, prgdwFormatting);

	// zap
	m_wndSayCtrl.bEmptyAndIndent();
	
	// Let UI object know
	bChatSendText(str, BM_SAY, TRUE, prgdwFormatting);

	FreeAndNullFormatting(&prgdwFormatting);
}


void CSayWnd::OnActionsThink() 
{
	CString		str;
	CChatDoc*	pDoc = GetChatDoc();

	// Ctrl+T in the Status Window should have no effect
	if (pDoc && pDoc->m_bStatusView)
		return;

	m_wndSayCtrl.ValidateIMEEntry();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if(str.IsEmpty())
	{
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!bLegalToSend())
		return;

	CDWordArray* prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

	if (m_wndSayCtrl.m_pDosKey)
		m_wndSayCtrl.m_pDosKey->bAppendEntry(str, prgdwFormatting);

	// zap
	m_wndSayCtrl.bEmptyAndIndent();

	bChatSendText(str, BM_THINK, TRUE, prgdwFormatting);

	FreeAndNullFormatting(&prgdwFormatting);
}


void CSayWnd::OnPlaySound()
{
	CChatDoc*	pDoc = GetChatDoc();

	// Ctrl+H in the Status Window should have no effect
	if (pDoc && pDoc->m_bStatusView)
		return;

	if (!bLegalToSend(m_bWhisperSay))
		return;

	CString			strSnd;
	CSoundDlg		sndDlg(this);

	m_wndSayCtrl.ValidateIMEEntry();
	m_wndSayCtrl.GetWindowText(sndDlg.m_rtfCtrl.m_strText);
	sndDlg.m_rtfCtrl.m_prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);
	sndDlg.m_rtfCtrl.m_pFont = m_fontText;
	sndDlg.m_rtfCtrl.m_crTextColor = m_wndSayCtrl.m_crTextColor;

	int rval = theApp.DoModalDlg(&sndDlg);
	
	m_wndSayCtrl.SetFocus();   // put focus in saywnd (important for IME support)
	
	if (rval != IDOK)
		return;
	
	strSnd = sndDlg.m_selectedSnd;
	
	if (strSnd.IsEmpty())
		return;

	if (m_wndSayCtrl.m_pDosKey)
		m_wndSayCtrl.m_pDosKey->bAppendEntry(sndDlg.m_rtfCtrl.m_strText, sndDlg.m_rtfCtrl.m_prgdwFormatting);

	// zap
	m_wndSayCtrl.bEmptyAndIndent();

	if (m_bWhisperSay)
		bWhisperInBox(strSnd, sndDlg.m_rtfCtrl.m_strText, sndDlg.m_rtfCtrl.m_prgdwFormatting, BM_WHISPER | BM_SOUND);
	else
		bChatSendSound(strSnd, sndDlg.m_rtfCtrl.m_strText, sndDlg.m_rtfCtrl.m_prgdwFormatting, TRUE /*bEcho*/, BM_SAY, NULL);
}


void CSayWnd::OnSendAction() 
{
	CString		str;
	CChatDoc*	pDoc = GetChatDoc();

	// Ctrl+J in the Status Window should have no effect
	if (pDoc && pDoc->m_bStatusView)
		return;

	m_wndSayCtrl.ValidateIMEEntry();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if (str.IsEmpty()) 
	{
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!bLegalToSend(m_bWhisperSay))
		return;

	CDWordArray* prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

	if (m_wndSayCtrl.m_pDosKey)
		m_wndSayCtrl.m_pDosKey->bAppendEntry(str, prgdwFormatting);

	// zap
	m_wndSayCtrl.bEmptyAndIndent();

	if (m_bWhisperSay)
		bWhisperInBox("", str, prgdwFormatting, BM_WHISPER | BM_ACTION);	// ... for historical reasons
	else
		bChatSendText(str, BM_ACTION, TRUE, prgdwFormatting);

	FreeAndNullFormatting(&prgdwFormatting);
}


void CSayWnd::OnSetFocus(CWnd* pOldWnd) 
{
	CFrameWnd::OnSetFocus(pOldWnd);

	// if focus set to outer saywnd (say it's a tab stop in whisperbox) reset it to inner CSayCtrl
	m_wndSayCtrl.SetFocus();
}


void CSayWnd::SwitchSelectionFormat(WORD wFormat)
{
	ASSERT(wFormat);

	m_wndSayCtrl.SwitchSelectionFormat(wFormat);
}

// WM_SAYSCROLLKEY is sent by the say RTF control, when the user hits page up
// or page down. This allows the parent to process this input.

LRESULT 
CSayWnd::OnScrollKey(
WPARAM wParam, 
LPARAM lParam)
{
	CWnd* pWndSendTo;

	if (m_bWhisperSay)
		pWndSendTo = ((CWhisperBox*)GetParent ())->GetCurrentEdit (); 
	else
		pWndSendTo = GetChatDoc ()->GetComponentWindow (CHATFOCUS_OUTPUTWND);
	if (pWndSendTo)
		pWndSendTo->SendMessage (WM_KEYDOWN, wParam, lParam);
	return 0;
}

void
CSayWnd::OnSayEditChanged()
{
	if (!m_wndSayCtrl.m_bEnChangeFreeze)
	{
		CString str;
		m_wndSayCtrl.GetWindowText (str);
		ChatPreSendText (str);
	}
}

int 
CSayToolBar::OnToolHitTest(
CPoint point, 
TOOLINFO* pTI) const
{
	int n = CToolBar::OnToolHitTest (point, pTI);
	//if (n != -1 && pTI != NULL)
	//{
	//	pTI->uFlags |= TTF_ALWAYSTIP;
	//}
	return n;
}
