// saywnd.cpp : implementation file
//

#include "stdafx.h"
#include "userinfo.h"
#include "textview.h"
#include "memblst.h"
#include "chat.h"
//#include "bothdlg.h"
#include "saywnd.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "dib.h"
#include "script.h"
#include "pageview.h"
#include "ui.h"

#include "chatprot.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Parameters

#define DOCKABLEGESTUREBAR
#define WHITESAYWINDOW

// sizes for toolbar button image
#define cxToolBar		17
#define cyToolBar		17

// size for say bar
#define cxSayBar		96  // reduced this from 106


/////////////////////////////////////////////////////////////////////////////
// External statics

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// statics w/ local scope

static UINT BASED_CODE balloons[] =
{
	ID_ACTIONS_SAY,
	ID_ACTIONS_THINK,
	ID_ACTIONS_WHISPER,
	ID_SEND_ACTION,
};


/////////////////////////////////////////////////////////////////////////////
// CSayCtrl

IMPLEMENT_DYNCREATE(CSayCtrl, CEdit)

CSayCtrl::CSayCtrl()
{
//	m_timerid = 0;
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

BEGIN_MESSAGE_MAP(CSayCtrl, CEdit)
	//{{AFX_MSG_MAP(CSayCtrl)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_WM_TIMER()
	ON_CONTROL_REFLECT(EN_MAXTEXT, OnMaxtext)
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
		if (btype == SM_SAY) cui.Say(str);
		else if (btype == SM_THINK) cui.Think(str);
		else if (btype == SM_WHISPER) cui.Emote(str);
		SetMyAvatar(MAGICID);

		if (!(*line)) TrapSend();
	} else return FALSE;						// after all reads finish, continue
	return TRUE;
}
#endif

BOOL LegalToSend() {
	int ChatGetConnectionStatus(), cxStatus;
	cxStatus = ChatGetConnectionStatus();
	if (cxStatus == CX_NOCHANNEL || cxStatus == CX_CONNECTING) {
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
	BOOL GetSendComicsData();
//	extern BOOL NullAvatar();			// saywnd doesn't know about avatars
//	if (NullAvatar()) return;			// body pose changes will crash null avatars

	// always first kill the timer so it is set only if a character is typed
//	KillBotherTimer();

	if( nChar == CHAR_RETURN )
	{
//		if (TrapSend()) return;			// SIGGRAPH PIX

		CString str;

		if (!LegalToSend()) return;

		// get contents, send them, then zap
		GetWindowText(str);

		if (str.GetLength() == 0) {		//  transmit a signal that only character coming over
			if (theApp.m_bComicView && GetSendComicsData())
				str = "<Chr>";
			else return;				//  null sends prohibited in text mode
		}

		// Let UI object know
		cui.Say(str);

		// zap
		SetWindowText("");

		// set focus to view window
		GetView()->SetFocus();

	} else if (nChar == CTRL_Q) {		// break the panel
		CString str("<Brk>");
		cui.Say(str);
		TRACE("Manual break...\n");
		GetView()->SetFocus();
		return;
	} else if (nChar == CTRL_U) {		// clear the line
		SetWindowText("");
		GetView()->SetFocus();
		cui.PreSay(CString(""));
	}
	else if (nChar == TAB)
	{
			
		if(!GetChatApp()->m_bComicView)
		{
			GetTextView()->m_pRichEdit->SetFocus();
		}
		else
		{
			GetMembers()->m_MemberListBox.SetFocus();
			int index = GetMembers()->m_MemberListBox.GetNextItem(-1,LVNI_FOCUSED);
			if(index == -1)
				GetMembers()->m_MemberListBox.SetItemState(0,LVIS_FOCUSED, LVIS_FOCUSED);
		}

	}
	else if (nChar == SHIFTTAB)
	{
		GetMembers()->m_MemberListBox.SetFocus();
		int index = GetMembers()->m_MemberListBox.GetNextItem(-1,LVNI_FOCUSED);
		if(index == -1)
			GetMembers()->m_MemberListBox.SetItemState(0,LVIS_FOCUSED, LVIS_FOCUSED);
	}
	else {
		if (_istspace(nChar) && LineLength() == 0) return; // don't let people start out w/ spaces or tabs
		CEdit::OnChar(nChar, nRepCnt, nFlags);

		// Get the text, and do a PreSay (since it might affect the character's pose)
		CString str;
		GetWindowText(str);
		cui.PreSay(str);
		// since a character was typed, start timer
//		if(!str.IsEmpty())
//			m_timerid = SetTimer(TIMERID,TIMEOUT,NULL);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSayWnd

IMPLEMENT_DYNCREATE(CSayWnd, CFrameWnd)

CSayWnd::CSayWnd()
{
}

CSayWnd::~CSayWnd()
{
}


BEGIN_MESSAGE_MAP(CSayWnd, CFrameWnd)
	//{{AFX_MSG_MAP(CSayWnd)
	ON_WM_SIZE()
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION1, OnUpdateActions)
	ON_WM_PAINT()
	ON_COMMAND(ID_ACTIONS_EMOTE, OnActionsEmote)
	ON_COMMAND(ID_ACTIONS_SAY, OnActionsSay)
	ON_COMMAND(ID_ACTIONS_THINK, OnActionsThink)
	ON_COMMAND(ID_ACTIONS_WHISPER, OnActionsWhisper)
//	ON_UPDATE_COMMAND_UI(ID_ACTIONS_SAY, OnUpdateActionsSay)
//	ON_UPDATE_COMMAND_UI(ID_ACTIONS_WHISPER, OnUpdateActionsWhisper)
	ON_COMMAND(ID_SEND_ACTION, OnSendAction)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION2, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION3, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION4, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION5, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION6, OnUpdateActions)
	ON_UPDATE_COMMAND_UI(ID_ACTIONS_ACTION7, OnUpdateActions)
//	ON_UPDATE_COMMAND_UI(ID_ACTIONS_THINK, OnUpdateActionsSay)
//	ON_UPDATE_COMMAND_UI(ID_SEND_ACTION, OnUpdateActionsSay)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSayWnd message handlers

void CSayWnd::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

	if( nType == SIZE_RESTORED )
	{

		// don't do during initialization
		if( ::IsWindow( m_wndSayCtrl.m_hWnd ) )
		{
#ifndef DOCKABLEGESTUREBAR
			// position and size gesture and entry window
			m_wndSayCtrl.SetWindowPos(	NULL,
										0,
										0,
										cx - cxSayBar,
										cy - (cyToolBar + 14),
										SWP_NOZORDER );

			m_wndSayBar.SetWindowPos(	NULL,
										cx - cxSayBar,
										0,
										0,
										0,
										SWP_NOZORDER | SWP_NOSIZE );
			m_wndGestBar.SetWindowPos(	NULL,
										0,
										cy - (cyToolBar + 14),
										0,
										0,
										SWP_NOZORDER | SWP_NOSIZE );
#else
			// position and size gesture and entry window
			m_wndSayBar.SetWindowPos(	NULL,
										cx - cxSayBar-6,// The 6 was added to move the grabspace of the toolbar
														// under the sayctrl
										-3,				// same with the three
										cxSayBar+12,	// previously 0
										cyToolBar+12,	// previously 0
										SWP_NOZORDER );	// this used to also have SWP_NORESIZE | 

			m_wndSayCtrl.SetWindowPos(	&wndTop, // this was previously NULL
										0,
										0,
										cx - cxSayBar,
										cyToolBar+6, //  This was previously cy
										0);  // This was previously SWP_NOZORDER

#endif

			m_wndSayCtrl.InvalidateRect( NULL, TRUE );

		}
	}
}

void CSayWnd::SetFocusToSayWnd()
{
	m_wndSayCtrl.SetFocus();
}


BOOL CSayWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	cs.style &= ~WS_BORDER;
	return CWnd::PreCreateWindow(cs);
}

BOOL CSayWnd::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	DWORD dwStyle;

	// create log window (don't worry about size)
	dwStyle = 	WS_VISIBLE 		| 
				WS_BORDER		|
				WS_CLIPSIBLINGS	|
//				ES_MULTILINE 	| 
				ES_AUTOHSCROLL;

	VERIFY( m_wndSayCtrl.Create(dwStyle, 
								CRect( CPoint(lpcs->x, lpcs->y), CSize(lpcs->cx, lpcs->cy)), 
								this, 
								1 ) );

	// set font
	m_wndSayCtrl.SetFont( &theApp.m_fontText );

	// set max chars
	m_wndSayCtrl.SetLimitText(350);		// reasonable message size

	// force focus here
	m_wndSayCtrl.SetFocus();

	// create toolbars
	CSize sizeButton( cxToolBar+7, cyToolBar+6 );
	CSize sizeImage( cxToolBar, cyToolBar );

#ifndef DOCKABLEGESTUREBAR
	// Create gesture bar
	if (!m_wndGestBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_NOALIGN | CBRS_FLYBY, 1 ) ||
		!m_wndGestBar.LoadBitmap(IDB_GESTURES) ||
		!m_wndGestBar.SetButtons(gestures, sizeof(gestures)/sizeof(UINT)))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	m_wndGestBar.SetSizes( sizeButton, sizeImage );

	// TODO: Remove this if you don't want Gest tips
	m_wndGestBar.SetBarStyle(m_wndGestBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	// remove border
	m_wndGestBar.SetBarStyle(m_wndGestBar.GetBarStyle() & ~CBRS_BORDER_TOP );

#endif

	// Create say/think/emote bar
	if (!m_wndSayBar.Create(this, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_NOALIGN, 1 ) ||
		!m_wndSayBar.LoadBitmap(IDB_BALLOONS) ||
		!m_wndSayBar.SetButtons(balloons, sizeof(balloons)/sizeof(UINT)))
	{
		TRACE0("Failed to create balloons toolbar\n");
		return -1;      // fail to create
	}

	m_wndSayBar.SetSizes( sizeButton, sizeImage );

	// TODO: Remove this if you don't want Gest tips
	m_wndSayBar.SetBarStyle(m_wndSayBar.GetBarStyle() |
		CBRS_TOOLTIPS | CBRS_FLYBY);

	// remove border
	m_wndSayBar.SetBarStyle(m_wndSayBar.GetBarStyle() & ~CBRS_BORDER_TOP );

	EnableDocking(CBRS_ALIGN_TOP | CBRS_ALIGN_BOTTOM);

	return CFrameWnd::OnCreateClient(lpcs, NULL);
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
	CString str;
	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	extern CPtrArray whisperees;
	extern void GetSelectedPuis(CPtrArray &);

	GetSelectedPuis(whisperees);

	if (str.IsEmpty() || whisperees.GetUpperBound() < 0) {
		AfxMessageBox(IDS_WHISPERENABLED);
		return;
	}

	if (!LegalToSend()) return;

	// Let UI object know
	cui.Emote(str);		// for now, whisper is mislabeled as emote

	// zap
	m_wndSayCtrl.SetWindowText("");

	// set focus to view window
	GetPrimaryView()->SetFocus();
}

void CSayWnd::OnActionsSay() 
{

//	m_wndSayCtrl.KillBotherTimer();
	
	CString str;
	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if(str.IsEmpty()) {
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!LegalToSend()) return;

	// Let UI object know
	cui.Say(str);

	// zap
	m_wndSayCtrl.SetWindowText("");

	// set focus to view window
	GetPrimaryView()->SetFocus();
}

void CSayWnd::OnActionsThink() 
{
	
//	m_wndSayCtrl.KillBotherTimer();

	CString str;
	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if(str.IsEmpty()) {
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!LegalToSend()) return;

	// Let UI object know
	cui.Think(str);

	// zap
	m_wndSayCtrl.SetWindowText("");

	// set focus to view window
	GetPrimaryView()->SetFocus();
}

void CSayWnd::OnSendAction() 
{
	
//	m_wndSayCtrl.KillBotherTimer();

	CString str;
	const char *GetMyName();

	// get contents, send them, then zap
	m_wndSayCtrl.GetWindowText(str);

	if (str.IsEmpty()) {
		AfxMessageBox(IDS_SENDENABLED);
		return;
	}

	if (!LegalToSend()) return;

	// Let UI object know
//	CString composite = GetMyName();
//	composite += " ";
//	composite += str; 
	cui.Action(str);

	// zap
	m_wndSayCtrl.SetWindowText("");

	// set focus to view window
	GetPrimaryView()->SetFocus();
}

void CSayWnd::OnActionsEmote() 
{
	// TODO: Add your command handler code here
	
}

/*void CSayWnd::OnUpdateActionsSay(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TextEntered());
}*/


/*void CSayWnd::OnUpdateActionsWhisper(CCmdUI* pCmdUI) 
{
	int TalkToCount();
	pCmdUI->Enable(TextEntered() && TalkToCount());
}*/

extern HWND hgPrevFocus;
void CSayCtrl::OnKillFocus(CWnd* pNewWnd) 
{
	CEdit::OnKillFocus(pNewWnd);
	
	hgPrevFocus = m_hWnd;
	
}


/*void CSayCtrl::OnTimer(UINT nIDEvent) 
{
	// first turn off the timer
	KillBotherTimer();
	// if the user didnt say to not warn them again, then warn them
	BOOL temp = GetChatApp()->m_bBother;
	if(!GetChatApp()->m_bBother)
	{
		CBotherDlg dlg;
		dlg.DoModal();
		// Save the check box
		GetChatApp()->m_bBother = dlg.m_bBother;

	}
	
	CEdit::OnTimer(nIDEvent);
}*/

/*void CSayCtrl::KillBotherTimer()
{
	if(m_timerid !=0)
	{
		KillTimer(m_timerid);
		m_timerid = 0;
	}

}*/

void CSayCtrl::OnMaxtext() 
{
	AfxMessageBox(IDS_TOOMUCHTEXT);
	
}
