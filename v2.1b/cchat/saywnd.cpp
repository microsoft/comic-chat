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
#include <imm.h>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

BOOL WhisperInBox(CString &, CDWordArray *);

/////////////////////////////////////////////////////////////////////////////
// Parameters

// sizes for toolbar button image
#define cxToolBar		17
#define cyToolBar		17

// size for say bar
#define cxSayBar_say_IRC	120
#define cxSayBar_say_NM		96
#define cxSayBar_whisper	24


/////////////////////////////////////////////////////////////////////////////
// External statics

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// statics w/ local scope

static UINT BASED_CODE balloons_say[] =
{
	ID_ACTIONS_SAY,
	ID_ACTIONS_THINK,
	ID_ACTIONS_WHISPER,
	ID_SEND_ACTION,
	ID_PLAY_SOUND,
};

static UINT BASED_CODE balloons_whisper[] =
{
	ID_ACTIONS_WHISPER
};


/////////////////////////////////////////////////////////////////////////////
// CSayCtrl

IMPLEMENT_DYNCREATE(CSayCtrl, CRtfCtrl)

CSayCtrl::CSayCtrl()
{
	m_bWhisperSay = FALSE;
}


CSayCtrl::~CSayCtrl()
{
}


BOOL CSayCtrl::IsEmpty()
{
	// if no text, then empty
	if( SendMessage(WM_GETTEXTLENGTH, 0, 0) == 0 )
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
	SetWindowText("");
	BOOL bRet = bSetIndent(72);	// Set left indentation, 72 is magic number used in history text window too
	EmptyUndoBuffer();			// Fix 4409
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
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(EN_MAXTEXT, OnMaxtext)
	ON_WM_TIMER()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

#if 0
#define MAGICID 1
BOOL TrapSend() {
	char line[1000];
	const char *mesgPtr;
	UINT id;
	static FILE *fp = NULL;
	extern UINT MyAvatarID();
	void FindAttribution(const char *words, void *userInfo, const char **mesgPtr, UINT *id);
	void FindAddressees(UINT speakerID, const char *words, const char **mesgPtr);
	void FindPose(UINT speakerID, const char *words, const char **mesgPtr);
	int FindBalloonType(const char *words, const char **mesgPtr);

	if (MyAvatarID() != MAGICID) return FALSE;   // has to be susan, otherwise continue

	if (!fp) {
		CString path = theApp.GetBaseDir() + "\\transcript.txt";
		VERIFY(fp = fopen(path, "r"));
	}

	if (fgets(line, sizeof(line), fp)) {
		if (*line == '#') return TRUE;
		char *nl = strchr(line, '\n');			// null out new line
		if (nl) *nl = '\0';
		if (!(*line)) strcpy(line, "<Brk>");    // convert line breaks to panel breaks

		// do cooking locally...
		FindAttribution(line, NULL, &mesgPtr, &id);
		// temporarily change avatar
		SetMyAvatar(id);
		FindAddressees(id, mesgPtr, &mesgPtr);        // updates the avatar structure as a side effect
		FindPose(id, mesgPtr, &mesgPtr);
		int btype = FindBalloonType(mesgPtr, &mesgPtr);
		CString str(mesgPtr);
//		ChatPreSendText(str, id);
		GetView()->GetDocument()->TallySpeech(id);
		if (btype == SM_SAY || bType == SM_THINK || btype == SM_WHISPER)
			ChatSendText(str, bType);
		SetMyAvatar(MAGICID);

		if (!(*line)) TrapSend();
	} else return FALSE;						// after all reads finish, continue
	return TRUE;
}
#endif

BOOL LegalToSend(BOOL bPrivMsg = FALSE) {
	CRoomInfo *info = currentRoom ? currentRoom : GetDefaultProto();
	int cxStatus = info->GetConnectionStatus();
	if ((cxStatus == CX_NOCHANNEL && !bPrivMsg) || cxStatus == CX_CONNECTING) {
		AfxMessageBox(IDS_ILLEGAL_TO_SEND);
		return FALSE;
	} else return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CSayCtrl message handlers

#define TIMERID 1
#define TIMEOUT 30000


void CSayCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	BOOL GetSendComicsData(), bCanDance();
//	extern BOOL NullAvatar();			// saywnd doesn't know about avatars
//	if (NullAvatar()) return;			// body pose changes will crash null avatars

	if (nChar == CHAR_RETURN)
	{
		// if (TrapSend()) return;			// SIGGRAPH PIX

		// get contents, send them, then zap
		CString str;
		GetWindowText(str);
		BOOL isEmpty = str.IsEmpty(), bZap;

		if ((isEmpty || str[0] != '/') && !LegalToSend(m_bWhisperSay)) 
			return;

		if (isEmpty) {					//  transmit a signal that only character coming over
			if (!m_bWhisperSay && GetChatDoc() && GetChatDoc()->m_bComicView && GetSendComicsData() && bCanDance())
				str = "<Chr>";
			else {
				if (m_bWhisperSay) AfxMessageBox(IDS_SENDENABLED);
				return;					//  null sends prohibited in text mode, or whisper box
			}
		}

		CDWordArray* prgdwFormatting = m_pFont ? PRGDWGetFormatting(this, m_pFont, m_crTextColor) : NULL;

		bEmptyAndIndent();

		// Let UI object know
		if (m_bWhisperSay) 
			bZap = WhisperInBox(str, prgdwFormatting);
		else 
			bZap = ChatSendText(str, SM_SAY, TRUE, prgdwFormatting);

		//if (bZap)	// Attention! window may have been destroyed by a /part or /join command!
		//	bEmptyAndIndent();

		if (prgdwFormatting)
		{
			prgdwFormatting->RemoveAll();
			delete prgdwFormatting;
		}
		return;
	}

	BOOL bCtrlDown = GetKeyState(VK_CONTROL) & 0x8000;

	if (bCtrlDown)
	{
		if (nChar == CTRL_Q)
		{		// break the panel
			CString str("<Brk>");
			ChatSendText(str, SM_SAY);
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
		if (nChar == TAB)
		{
			if (m_bWhisperSay)
			{
				CWnd *wnd = GetParent()->GetParent();
				short state = GetKeyState(VK_SHIFT);
				int shifted = (state & 0x8000);
				wnd->SendMessage(WM_NEXTDLGCTL, shifted, 0);
			} 
			else if (!GetChatDoc()->m_bComicView)
				GetTextView()->m_pRichEdit->SetFocus();
			else
			{
				GetMembers()->m_MemberListBox.SetFocus();
				int index = GetMembers()->m_MemberListBox.GetNextItem(-1,LVNI_FOCUSED);
				if(index == -1)
					GetMembers()->m_MemberListBox.SetItemState(0,LVIS_FOCUSED, LVIS_FOCUSED);
			}
			return;
		}

		if (nChar == SHIFTTAB)
		{
			GetMembers()->m_MemberListBox.SetFocus();
			int index = GetMembers()->m_MemberListBox.GetNextItem(-1,LVNI_FOCUSED);
			if(index == -1)
				GetMembers()->m_MemberListBox.SetItemState(0,LVIS_FOCUSED, LVIS_FOCUSED);
			return;
		}
	}

	if (!bCtrlDown && _istspace(nChar) && LineLength() == 0) 
		return; // don't let people start out w/ spaces or tabs

	CRtfCtrl::OnChar(nChar, nRepCnt, nFlags);

	// Get the text, and do a PreSay (since it might affect the character's pose)
	CString str;
	GetWindowText(str);
	ChatPreSendText(str);
	// since a character was typed, start timer
	// if(!str.IsEmpty())
	//	m_timerid = SetTimer(TIMERID,TIMEOUT,NULL);
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


/////////////////////////////////////////////////////////////////////////////
// CSayWnd

IMPLEMENT_DYNCREATE(CSayWnd, CFrameWnd)

CSayWnd::CSayWnd()
{
	m_bWhisperSay	= FALSE;
	m_fontText		= NULL;

	SetToolBarInfo(FALSE);
}


CSayWnd::CSayWnd(BOOL bWhisperSay)
{
	m_bWhisperSay				= bWhisperSay;
	m_wndSayCtrl.m_bWhisperSay	= bWhisperSay;
	m_fontText					= NULL;

	SetToolBarInfo(bWhisperSay);
}


CSayWnd::~CSayWnd()
{
	if (m_fontText)
		delete m_fontText;
}


void CSayWnd::SetToolBarInfo(BOOL bWhisperSay)
{
	if (bWhisperSay)
	{
		m_balloons = balloons_whisper;
		m_cntBalloons = sizeof(balloons_whisper) / sizeof(UINT);
		m_cxSayBar = cxSayBar_whisper;
		m_IDB_BALLOONS = IDB_WHISPER_BAR;
	}
	else
	{
		m_balloons = balloons_say;
		m_cntBalloons = sizeof(balloons_say) / sizeof(UINT);
		m_cxSayBar = theApp.m_bDoCB32 ? cxSayBar_say_NM : cxSayBar_say_IRC;
		m_IDB_BALLOONS = IDB_SAY_BAR;
	}
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
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION1, OnUpdateActions)
	ON_WM_PAINT()
	ON_COMMAND(ID_ACTIONS_SAY, OnActionsSay)
	ON_COMMAND(ID_ACTIONS_THINK, OnActionsThink)
	ON_COMMAND(ID_ACTIONS_WHISPER, OnActionsWhisper)
	ON_COMMAND(ID_SEND_ACTION, OnSendAction)
	ON_COMMAND(ID_PLAY_SOUND, OnPlaySound)
	ON_WM_SETFOCUS()
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION2, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION3, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION4, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION5, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION6, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION7, OnUpdateActions)
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

	VERIFY( m_wndSayCtrl.Create(dwStyle, 
								CRect(CPoint(lpcs->x, lpcs->y), CSize(lpcs->cx, lpcs->cy)), 
								this, 
								ID_SAYCTRL));
	// Need to add the EN_SELCHANGE notification to the dwEventMask of the rich text control
	dwEventMask = (DWORD) m_wndSayCtrl.GetEventMask();
	m_wndSayCtrl.SetEventMask(dwEventMask | ENM_SELCHANGE | ENM_MOUSEEVENTS);

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
	if (!m_wndSayBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_NOALIGN, 1) ||
		!m_wndSayBar.LoadBitmap(m_IDB_BALLOONS) ||
		!m_wndSayBar.SetButtons(m_balloons, m_cntBalloons)) 
	{
		TRACE0("Failed to create balloons toolbar\n");
		return -1;      // fail to create
	}

	m_wndSayBar.SetSizes(sizeButton, sizeImage);

	// TODO: Remove this if you don't want Gest tips
	m_wndSayBar.SetBarStyle(m_wndSayBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY);

	// remove border
	m_wndSayBar.SetBarStyle(m_wndSayBar.GetBarStyle() & ~CBRS_BORDER_TOP );

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

void CSayWnd::OnUpdateActions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	
}

void CSayWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	CBrush brGray(RGB(192,192,192));
	RECT rect;

	GetClientRect(&rect);
	dc.FillRect( &rect, &brGray );
	
}

BOOL CSayWnd::IsEmpty()
{
	return m_wndSayCtrl.IsEmpty();
}

void CSayWnd::OnActionsWhisper() 
{
	//	m_wndSayCtrl.KillBotherTimer();
	CString			str;
	CDWordArray*	prgdwFormatting = NULL;
	BOOL			bZap;

	m_wndSayCtrl.ValidateIMEEntry();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if (!LegalToSend(m_bWhisperSay))
		return;

	if (m_bWhisperSay) {	// need to handle whisperbox case separately
		if (str.IsEmpty()) {
			AfxMessageBox(IDS_SENDENABLED);
			return;
		}

		prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

		m_wndSayCtrl.bEmptyAndIndent();
		bZap = WhisperInBox(str, prgdwFormatting);	// ... for historical reasons
	} else {
		extern CPtrArray whisperees;
		extern void GetSelectedPuis(CPtrArray &);

		GetSelectedPuis(whisperees);

		if (str.IsEmpty() || whisperees.GetUpperBound() < 0) {
			AfxMessageBox(IDS_WHISPERENABLED);
			return;
		}

		// Let UI object know
		prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

		m_wndSayCtrl.bEmptyAndIndent();
		bZap = ChatSendText(str, SM_WHISPER, TRUE, prgdwFormatting);
	}

	//if (bZap)
	//	m_wndSayCtrl.bEmptyAndIndent();

	if (prgdwFormatting)
	{
		prgdwFormatting->RemoveAll();
		delete prgdwFormatting;
	}
}

void CSayWnd::OnActionsSay() {
//	m_wndSayCtrl.KillBotherTimer();
	
	CString str;

	m_wndSayCtrl.ValidateIMEEntry();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if(str.IsEmpty()) {
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!LegalToSend())
		return;

	CDWordArray* prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

	// zap
	m_wndSayCtrl.bEmptyAndIndent();
	
	// Let UI object know
	ChatSendText(str, SM_SAY, TRUE, prgdwFormatting);

	if (prgdwFormatting)
	{
		prgdwFormatting->RemoveAll();
		delete prgdwFormatting;
	}
}

void CSayWnd::OnActionsThink() 
{
//	m_wndSayCtrl.KillBotherTimer();

	CString str;

	m_wndSayCtrl.ValidateIMEEntry();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if(str.IsEmpty())
	{
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!LegalToSend())
		return;

	CDWordArray* prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

	// zap
	m_wndSayCtrl.bEmptyAndIndent();

	ChatSendText(str, SM_THINK, TRUE, prgdwFormatting);

	if (prgdwFormatting)
	{
		prgdwFormatting->RemoveAll();
		delete prgdwFormatting;
	}
}

void CSayWnd::OnPlaySound()
{
	if (!LegalToSend())
		return;

	CString			strSnd;
	CSoundDlg		sndDlg;

	m_wndSayCtrl.ValidateIMEEntry();
	m_wndSayCtrl.GetWindowText(sndDlg.m_rtfCtrl.m_strText);
	sndDlg.m_rtfCtrl.m_prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);
	sndDlg.m_rtfCtrl.m_pFont = m_fontText;
	sndDlg.m_rtfCtrl.m_crTextColor = m_wndSayCtrl.m_crTextColor;

	int rval = sndDlg.DoModal();
	
	m_wndSayCtrl.SetFocus();   // put focus in saywnd (important for IME support)
	
	if (rval != IDOK)
		return;
	
	strSnd = sndDlg.m_selectedSnd;
	
	if (strSnd.IsEmpty())
		return;

	strSnd += ".wav";

	// zap
	m_wndSayCtrl.bEmptyAndIndent();

	currentRoom->ChatSendSound(strSnd, sndDlg.m_rtfCtrl.m_strText, sndDlg.m_rtfCtrl.m_prgdwFormatting);
}



void CSayWnd::OnSendAction() 
{
//	m_wndSayCtrl.KillBotherTimer();

	CString str;

	m_wndSayCtrl.ValidateIMEEntry();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if (str.IsEmpty()) 
	{
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!LegalToSend()) 
		return;

	CDWordArray* prgdwFormatting = PRGDWGetFormatting(&m_wndSayCtrl, m_fontText, m_wndSayCtrl.m_crTextColor);

	// zap
	m_wndSayCtrl.bEmptyAndIndent();

	ChatSendText(str, SM_ACTION, TRUE, prgdwFormatting);

	if (prgdwFormatting)
	{
		prgdwFormatting->RemoveAll();
		delete prgdwFormatting;
	}
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

