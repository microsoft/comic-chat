// WhisprBx.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "textcore.h"
#include "saywnd.h"
#include "userinfo.h"
#include "WhisprBx.h"
#include "chatprot.h"
#include "ui.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "format.h"
#include "setupdlg.h"
#include "protsupp.h"
#include <mmsystem.h>

extern CChatApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void InitializeTextCore(CTextCore *tc, BOOL bResetOld = FALSE, BOOL bResetSay = FALSE);

#define ID_RICHVIEW	5

/////////////////////////////////////////////////////////////////////////////
// CWhisperBox dialog


CWhisperBox::CWhisperBox(CWnd* pParent /*=NULL*/)
	: CDialog(CWhisperBox::IDD, pParent)
{
	m_currentIndex = -1;
	m_bPostCreate = FALSE;		// only store dimension/position info after create
	m_bInverted = FALSE;		// title bar not inverted to start
	//{{AFX_DATA_INIT(CWhisperBox)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CWhisperBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CWhisperBox)
	DDX_Control(pDX, IDC_IGNORE_WBOX, m_ignoreButton);
	DDX_Control(pDX, IDC_DELETE_TAB, m_deleteButton);
	DDX_Control(pDX, IDC_TAB1, m_tabCtrl);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CWhisperBox, CDialog)
	//{{AFX_MSG_MAP(CWhisperBox)
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab)
	ON_BN_CLICKED(IDC_DELETE_TAB, OnDeleteTab)
	ON_BN_CLICKED(IDC_IGNORE_WBOX, OnIgnoreWbox)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
	ON_NOTIFY(EN_LINK, ID_RICHVIEW, HandleLink)
	ON_WM_GETMINMAXINFO()
	ON_WM_ACTIVATE()
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CWhisperBox message handlers

BOOL CWhisperBox::OnInitDialog() 
{
	CDialog::OnInitDialog();

	RECT rect;
	CWnd *base = GetDlgItem(IDC_SAYPOSITION);
	ASSERT(base);
	base->GetWindowRect(&rect);
	ScreenToClient(&rect);
	CRect rSay(rect.left+4, rect.top, rect.right-5, rect.bottom); // +4 hack to account for tabctrl margin :-(
	m_sayWnd = new CSayWnd(TRUE, SB_WHISPER | SB_WACTION | SB_WSOUND);
	VERIFY (m_sayWnd->Create(NULL, "", WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP, rSay, this));
	m_sayWnd->SetWindowPos(GetDlgItem(IDC_TAB1), 0, 0, 0, 0,			// place in proper tabstop location
						   SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CWhisperBox::SwitchToTab(int iTabNum)
{
	if (iTabNum == m_currentIndex) 
		return;

	//TRACE("SHOWING %d\n", tabNum);
	CWhisperLeaf *newLeaf = (CWhisperLeaf *)(m_leaves[iTabNum]);  // display new first so no flash
	newLeaf->m_richView->SetWindowPos(&wndTop, m_richRect.left, m_richRect.top, m_richRect.right - m_richRect.left,
					                  m_richRect.bottom - m_richRect.top, SWP_SHOWWINDOW);

	// hide current tab
	if (m_currentIndex >= 0) 
	{
		CWhisperLeaf *oldLeaf = (CWhisperLeaf*) m_leaves[m_currentIndex];
		oldLeaf->m_richView->ShowWindow(SW_HIDE);
	}

	//TRACE("HIDING %d\n", m_currentIndex);

	m_tabCtrl.SetCurSel(iTabNum);
	m_currentIndex = iTabNum;
	m_ignoreButton.SetCheck(newLeaf->m_bIgnore != 0);   // in case valid bool != 1
	SetModified(iTabNum, FALSE);
}


int CWhisperBox::AddTab(CUserInfo *pui)
{
	// insert tab alphabetically in place
	const int		HISTMINHEIGHT = 44;
	CWhisperLeaf*	leaf;
	const char*		szTabName = pui->GetScreenName();
	TC_ITEM			tc;

	tc.pszText = UnConst(szTabName);
	tc.mask = TCIF_TEXT;
	int iUpper = m_leaves.GetUpperBound();
	int iPlace = 0;
	for (int i = 0; i <= iUpper; i++)
	{
		leaf = (CWhisperLeaf*) m_leaves[i];
		if (leaf->m_label.CompareNoCase(szTabName) > 0)
			break;
		else
			iPlace++;
	}
	int iTab = m_tabCtrl.InsertItem(iPlace, &tc);
	if (iTab < 0)
		return iTab;   // couldn't add a tab
	
	// create new leaf, and fill it in
	CWhisperLeaf *newLeaf = new CWhisperLeaf(pui);
	m_leaves.InsertAt(iPlace, newLeaf);

	if (iPlace <= m_currentIndex)
		m_currentIndex++;

	RECT r;
	m_tabCtrl.GetWindowRect(&r);
	ScreenToClient(&r);
	m_tabCtrl.AdjustRect(FALSE, &r);
	m_richRect = r;

	newLeaf->m_richView = new CRichEditCtrl;
	VERIFY(newLeaf->m_richView->Create(WS_CHILD | WS_VSCROLL | WS_BORDER | TEXT_VIEW_WND_FLAGS, r, GetWhisperBox(), ID_RICHVIEW));
	newLeaf->m_richView->SetEventMask(ENM_LINK | ENM_MOUSEEVENTS);
	newLeaf->m_richCore = new CTextCore;
	VERIFY(SUCCEEDED(newLeaf->m_richCore->AttachTextViewHWnd(newLeaf->m_richView->m_hWnd, AfxGetInstanceHandle())));
	VERIFY(newLeaf->m_richCore->bSetTextViewBufferMaxSize(65536));
	InitializeTextCore(newLeaf->m_richCore);

	// need to refresh size of current history window
	if (m_currentIndex >= 0)
	{
		leaf = (CWhisperLeaf*) m_leaves[m_currentIndex];
		leaf->m_richView->SetWindowPos(&wndTop, m_richRect.left, m_richRect.top, m_richRect.right - m_richRect.left,
									   m_richRect.bottom - m_richRect.top, SWP_SHOWWINDOW);
	}

	// history window size might have become too small
	if (r.bottom - r.top < HISTMINHEIGHT)	// resize the whole dialog if the history window becomes too small
	{
		int iIncrease = HISTMINHEIGHT + r.top - r.bottom;
		GetWindowRect(&r);
		r.top -= iIncrease/2;
		r.bottom += iIncrease/2;
		MoveWindow(&r);
	}
	
	return iPlace;
}


CWhisperBox *CreateWhisperBox() 
{
	CWhisperBox *wbox = new CWhisperBox();
	cui.m_pvWhisperBox = wbox;
	// use desktop as parent, since we don't want this win to be always on top of main window
	CWnd *parent = CWnd::GetDesktopWindow();
	VERIFY(wbox->Create(IDD_WHISPERBOX, parent));
	wbox->m_bPostCreate = TRUE;
	HICON hIcon = theApp.LoadIcon(IDI_WHISPER);
	if (hIcon)
	{
		wbox->SetIcon(hIcon, TRUE);
		wbox->SetIcon(hIcon, FALSE);
	}
	if (!IsRectEmpty (&theApp.m_rectWhisper))
	{
		MakeRectVisibleOnScreen (&theApp.m_rectWhisper);
		wbox->MoveWindow (&theApp.m_rectWhisper);
	}
	wbox->ShowWindow(SW_SHOWNORMAL);
	return wbox;
}


void WhisperBox(CUserInfo *pui, BOOL bGiveFocus /* = TRUE */, BOOL bRestore /* = TRUE */)
{
	ASSERT(pui);
	CWhisperBox *pWBox;
	if ((pWBox = GetWhisperBox()) == NULL)
		pWBox = CreateWhisperBox();
	else
		if (bRestore)
			pWBox->ShowWindow(SW_RESTORE);  // show and un-iconify

	int iTabNum = pWBox->GetTab(pui->GetName());
	if (iTabNum >= 0) 
		pWBox->SwitchToTab(iTabNum);
	else
	{
		iTabNum = pWBox->AddTab(pui);
		if (iTabNum < 0)
			return;
		pWBox->SwitchToTab(iTabNum);	
	}

	if (bGiveFocus)
		// put focus there...
		pWBox->m_sayWnd->SetFocusToSayWnd();
}


void CWhisperBox::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	int sel = m_tabCtrl.GetCurSel();
	ASSERT(sel >= 0);
	SwitchToTab(sel);
	
	*pResult = 0;
}


CWhisperLeaf::~CWhisperLeaf()
{
	if (m_richCore)
	{
		m_richCore->DetachTextViewHWnd();
		delete m_richCore;
	}
	if (m_richView)
		delete m_richView;
}


CWhisperBox::~CWhisperBox()
{
	FreeLeaves();
}


void CWhisperBox::FreeLeaves()
{
	int iUpper = m_leaves.GetUpperBound();

	for (int i = 0; i <= iUpper; i++)
	{
		CWhisperLeaf *leaf = (CWhisperLeaf *) m_leaves[i];
		delete leaf;
	}
}


int CWhisperBox::GetTab(const char *nick)
{
	int iUpper = m_leaves.GetUpperBound();
	
	for (int i = 0; i <= iUpper; i++)
	{
		CWhisperLeaf *leaf = (CWhisperLeaf *) m_leaves[i];
		if (leaf->m_nick == nick) return i;
	}
	return -1;
}


void DestroyWhisperBox() 
{
	if (cui.m_pvWhisperBox) {
		delete (CWhisperBox *)(cui.m_pvWhisperBox);
		cui.m_pvWhisperBox = NULL;
	}
}


void CWhisperBox::SetModified(int iTabNum, BOOL bValue)
{
	BOOL bPlayedSound = FALSE;

	if (bValue)
	{
		if (!IsWindowVisible())
		{	// first, make sure window is visible
			ShowWindow(SW_SHOW);
			SwitchToTab(iTabNum);
		}
		if (IsIconic() || CWnd::GetForegroundWindow() != this)
		{
			if (!m_bInverted)
			{
				FlashWindow(TRUE);  // if iconic, flash the icon and make a sound
				m_bInverted = TRUE;
				sndPlaySound("Default sound", SND_ASYNC);
				bPlayedSound = TRUE;
			}
		}
	}

	if (iTabNum == m_currentIndex && bValue)
		return;		// current tab never set to be modified

	CWhisperLeaf *pLeaf = (CWhisperLeaf *) m_leaves[iTabNum];

	if (!pLeaf)
		return;

	if (pLeaf->m_bModified == bValue)
		return;		// already set

	if (pLeaf->m_bModified && !bValue)
	{
		TC_ITEM tc;
		tc.pszText = UnConst(pLeaf->m_label);
		tc.mask = TCIF_TEXT;
		m_tabCtrl.SetItem(iTabNum, &tc);
		pLeaf->m_bModified = FALSE;
	}
	else
		if (!pLeaf->m_bModified && bValue)
		{
			TC_ITEM tc;
			CString strLabel = pLeaf->m_label;
			strLabel += " *";
			tc.pszText = UnConst(strLabel);
			tc.mask = TCIF_TEXT;
			m_tabCtrl.SetItem(iTabNum, &tc);
			pLeaf->m_bModified = TRUE;
			if (!bPlayedSound)
				sndPlaySound("Default sound", SND_ASYNC);  // don't play twice
		}
}


BOOL bAddToWhisperBox(CUserInfo *pui, USHORT uModes, const char *szMesg)
{
	ASSERT(pui);

	// mesg is a control full string
	if (uModes & BM_WHISPER)
	{
		int			iTab = -1;
		CWhisperBox *wbox = GetWhisperBox();

		if (wbox)
			iTab = wbox->GetTab(pui->GetName());

		if (pui->IsExternal() || iTab >= 0)
		{
			enumActions	rgaIDs[4] = { (enumActions) 3, aDoNotDisplay, aHighlightMessage, aReplaceMessage };
			CChatDoc*	pDocTmp = NULL;
			CUserInfo*	puiTmp = PuiFromDocNickIdent(&pDocTmp, (LPTSTR) (LPCTSTR) pui->GetName(), pui->GetFullName(), FALSE /*bSkipObscuredChannels*/, FALSE /*bAddExternalIfNotThere*/);
			CString		strChannelName, strDecodedRecipients, strSenderIdent = pui->GetName();

			if (!pui->GetFullName().IsEmpty())
				strSenderIdent += "!"+pui->GetFullName();

			GetAddressees(pui, "; ", strDecodedRecipients, FALSE /*bUseNick*/);

			if (pDocTmp && pDocTmp->m_proto)
			{
				ASSERT(puiTmp);
				strChannelName = pDocTmp->m_proto->m_strChannel;
			}

			theApp.m_dynaRules.SetCachRecipients(strDecodedRecipients);
			theApp.m_dynaRules.bMatchAndApplyRules(puiTmp ? eOnWhisperInRoom : eOnWhisper, (enumActions*) rgaIDs, NULL, CString(GetMyServer()), strSenderIdent, strChannelName, CString(szMesg));
			if (!(theApp.m_dynaRules.GetFlags() & g_wDoNotDisplay))
			{
				if (iTab == -1)
				{
					if (!wbox)
					{
						wbox = CreateWhisperBox();
						if (!wbox)
							return TRUE;
					}
					iTab = wbox->AddTab(pui);
					if (iTab < 0)
						return TRUE;
					if (wbox->m_currentIndex == -1)
						wbox->SwitchToTab(iTab);
				}

				CWhisperLeaf	*leaf = (CWhisperLeaf *) wbox->m_leaves[iTab];
				CDWordArray		*prgdwFormatting = new CDWordArray;
				char			*szControlFull = strdup(szMesg);

				if (!prgdwFormatting || !szControlFull)
					return TRUE;

				char* szControlLess = SzControlLess(szControlFull, prgdwFormatting);

				wbox->SetModified(iTab, TRUE);

				strDecodedRecipients.Empty();
				GetAddressees(pui, ", ", strDecodedRecipients, FALSE /*bUseNick*/);

				char cHighlightType = -1;
				if (theApp.m_dynaRules.GetFlags() & g_wHighlight)
					cHighlightType = theApp.m_dynaRules.GetFlags() >> 8;
				
				leaf->m_id = pui->GetFullName();
				leaf->m_richCore->iDisplayMsgHeader(strlen(szControlLess),
													pui->GetScreenName(),
													0,
													strDecodedRecipients,
													0,
													mtWhisper,
													msParticipant,
													NULL,
													2*cHighlightType);
				if (uModes & BM_ACTION)
					leaf->m_richCore->iDisplayAction(NULL,					/*szFrom, */
													 0, 
													 szControlLess,			/*szAction, */
													 0, 
													 msParticipant,
													 TRUE,					/* bShowURLs */
													 DEFAULT_INDENT,		/* lIndent */
													 NULL,					/* pCharFormat */
													 2*cHighlightType+1,	/* iHighlightIndex */
													 prgdwFormatting ? prgdwFormatting->GetData() : NULL,
													 prgdwFormatting ? prgdwFormatting->GetSize() : 0);
				else
					leaf->m_richCore->iDisplayMsgText(szControlLess,
													  0,
													  mtWhisper,
													  msParticipant,
													  TRUE,					/* bShowURLs */
													  FALSE,				/* bInformFull */
													  FALSE,				/* bAppend */
													  DEFAULT_INDENT,
													  NULL,					/* pCharFormat */
													  2*cHighlightType+1,	/* iHighlightIndex */
													  prgdwFormatting ? prgdwFormatting->GetData() : NULL,
													  prgdwFormatting ? prgdwFormatting->GetSize() : 0);
				free(szControlFull);
				FreeAndNullFormatting(&prgdwFormatting);
			}
			theApp.m_dynaRules.bMatchAndApplyRules(puiTmp ? eOnWhisperInRoom : eOnWhisper, NULL, (enumActions*) rgaIDs, CString(GetMyServer()), strSenderIdent, strChannelName, CString(szMesg));
			theApp.m_dynaRules.SetCachRecipients("");
			return TRUE;
		}
	}
	return FALSE;
}


void CWhisperBox::OnDeleteTab() 
{
	ASSERT(m_currentIndex >= 0);
	CWhisperLeaf *leaf = (CWhisperLeaf*) m_leaves[m_currentIndex];
	delete leaf;
	m_leaves.RemoveAt(m_currentIndex);
	int nextTab = (m_currentIndex > 0) ? m_currentIndex - 1 : 0;
	m_tabCtrl.DeleteItem(m_currentIndex);
	m_currentIndex = -1;  // so it will be a switch
	if (m_tabCtrl.GetItemCount() == 0)
		ShowWindow(SW_HIDE);
	else
		SwitchToTab(nextTab);

	RECT r;
	m_tabCtrl.GetWindowRect(&r);
	ScreenToClient(&r);
	m_tabCtrl.AdjustRect(FALSE, &r);
	m_richRect = r;
	if (m_currentIndex >= 0)
	{
		leaf = (CWhisperLeaf*) m_leaves[m_currentIndex];
		leaf->m_richView->SetWindowPos(&wndTop, m_richRect.left, m_richRect.top, m_richRect.right - m_richRect.left,
									   m_richRect.bottom - m_richRect.top, SWP_SHOWWINDOW);
	}
}


BOOL bWhisperInBox(CString strFilename, CString strMesg, CDWordArray *prgdwFormatting, USHORT uModes)
{
	CWhisperBox		*wbox = GetWhisperBox();
	if (!wbox || wbox->m_currentIndex < 0)
		return FALSE;

	CWhisperLeaf	*leaf = (CWhisperLeaf *) wbox->m_leaves[wbox->m_currentIndex];
	CChatDoc		*pDoc = NULL;
	CUserInfo		*pui = PuiFromDocNickIdent(&pDoc, (LPTSTR) (LPCTSTR) leaf->m_nick, (LPTSTR) (LPCTSTR) leaf->m_id, FALSE /*bSkipObscuredChannels*/, FALSE /*bAddExternalIfNotThere*/);

	if (!pui || pui->IsDeparted())
		pui = ExternalPui(leaf->m_nick, leaf->m_id, TRUE);
	if (!pui)
		return FALSE;

	g_rgpuiWhisperees.SetSize(1);
	g_rgpuiWhisperees[0] = pui;
	
	BOOL bZap;
	
	// don't echo, since we'll display it ourselves in box
	if (uModes & BM_SOUND)
		bZap = bChatSendSound(strFilename, strMesg, prgdwFormatting, FALSE /*bEcho*/, uModes, NULL /*szEncodedChannelName*/);
	else
		bZap = bChatSendText(strMesg, uModes, FALSE /*bEcho*/, prgdwFormatting, NULL, TRUE /*bWhispereesFilled*/, TRUE /*bInvokedByWhisperBox*/);

	if (!strMesg.IsEmpty() && strMesg[0] == '/') 
		return bZap; // was a command

	// if whisper box is iconized (call triggered by rule) flag the current tab
	if (wbox->IsIconic() || CWnd::GetForegroundWindow() != wbox)
		wbox->SetModified(wbox->m_currentIndex /*iTabNum*/, TRUE /*bValue*/);

	// don't display outgoing messages that are sent to myself
	if (!stricmp(leaf->m_nick, GetMyNickName()))
		return bZap;

	// display message
	const char *GetMyName(), *szName;

	if (g_puiSelf) 
		szName = g_puiSelf->GetScreenName();
	else 
		szName = GetMyName();
	// note "szName = g_puiSelf ? g_puiSelf->GetScreenName() : GetMyName()" is broken (due to type differential?)
	leaf->m_richCore->iDisplayMsgHeader(strlen(strMesg), 
										szName, 
										0, 
										leaf->m_label, 
										0,
										mtWhisper, 
										msParticipant,
										NULL,
										-1);
	if ((uModes & BM_ACTION) || (uModes & BM_SOUND))
	{
		CString strAction;
		
		if (uModes & BM_SOUND)
			strAction = StrGetSoundAction(strFilename, strMesg, prgdwFormatting);
		else
		{
			if (prgdwFormatting)
				PushFormattingOffsets(prgdwFormatting, strlen(GetMyName()) + 1);
			strAction = CString(GetMyName()) + " " + strMesg;
		}

		leaf->m_richCore->iDisplayAction(NULL,					/*szFrom, */
										 0, 
										 strAction,				/*szAction, */
										 0, 
										 msParticipant,
										 TRUE,					/* bShowURLs */
										 DEFAULT_INDENT,		/* lIndent */
										 NULL,					/* pCharFormat */
										 -1,					/* iHighlightIndex */
										 prgdwFormatting ? prgdwFormatting->GetData() : NULL,
										 prgdwFormatting ? prgdwFormatting->GetSize() : 0);
	}
	else
		leaf->m_richCore->iDisplayMsgText(strMesg,
										  0, 
										  mtWhisper, 
										  msParticipant,
										  TRUE,		/* bShowURLs */
										  FALSE,	/* bInformFull */
										  FALSE,	/* bAppend */
										  DEFAULT_INDENT,
										  NULL,		/* pCharFormat */
										  -1,		/* iHighlightIndex */
										  prgdwFormatting ? prgdwFormatting->GetData() : NULL,
										  prgdwFormatting ? prgdwFormatting->GetSize() : 0);
	return bZap;
}


// even though there is no "OK" button, the CDialog class gives us an "OK" when
// the user hits return in the SayWndCtrl.  To work around that, we send a
// return to the SayWndCtrl whenever we get an "OnOK" call.  (Hack).
void CWhisperBox::OnOK() 
{
	m_sayWnd->SendReturn();
}


// simply iterate through the whisper richedits, and change their fonts accordingly
void InitializeWhisperCores(BOOL bRestoreOld)
{
	CWhisperBox *wbox = GetWhisperBox();
	if (!wbox)
		return;
	int iUpper = wbox->m_leaves.GetUpperBound();
	for (int i = 0; i <= iUpper; i++)
	{
		CWhisperLeaf *leaf = (CWhisperLeaf *) wbox->m_leaves[i];
		InitializeTextCore(leaf->m_richCore, bRestoreOld);
	}
}


void CWhisperBox::OnIgnoreWbox() 
{
	if (m_currentIndex < 0)
		return;
	CWhisperLeaf *leaf = (CWhisperLeaf*) m_leaves[m_currentIndex];
	CUserInfo *pui = LookupPui(leaf->m_nick);
	if (!pui || pui->IsDeparted())
		pui = ExternalPui(leaf->m_nick, leaf->m_id, TRUE);
	if (pui)
		GetDefaultProto()->DoIgnoreUser(pui, m_ignoreButton.GetCheck(), FALSE);
}

//
// User clicked on a URL. The parent window then calls this routine..so the click
// is handled.
//
void CWhisperBox::HandleLink ( NMHDR * pNotifyStruct, LRESULT * result ) {
	if (m_currentIndex >= 0) {
		CWhisperLeaf *leaf = (CWhisperLeaf*) m_leaves[m_currentIndex];
		*result = leaf->m_richCore->bHandleLink((ENLINK*) pNotifyStruct);
	}
	else
		*result = 0;
}

#define RIGHTMARGIN			 7
#define LEFTMARGIN			RIGHTMARGIN
#define TOPMARGIN			RIGHTMARGIN
#define BOTTOMMARGIN		 3
#define INTERBUTTON			 7
#define	SAYTOPFROMBOTTOM	40
#define TABBOTTOMFROMBOTTOM	46


void CWhisperBox::OnClose()
{
	CDialog::OnClose();

	if (GetChatDoc())
		GetChatDoc()->SetFocusToSayWnd();
}


void CWhisperBox::OnSize(UINT nType, int cx, int cy) 
{
	CDialog::OnSize(nType, cx, cy);

	RECT r1;
	CWnd *delButton = GetDlgItem(IDC_DELETE_TAB);

	if (!delButton || nType != SIZE_RESTORED) 
		return;
	delButton->GetWindowRect(&r1);

	CRect margin(RIGHTMARGIN, LEFTMARGIN, TOPMARGIN, BOTTOMMARGIN);
	MapDialogRect(&margin);
	CRect metrics(INTERBUTTON, SAYTOPFROMBOTTOM, 0, TABBOTTOMFROMBOTTOM);
	MapDialogRect(&metrics);

	int deleteWidth = r1.right - r1.left;
	int deleteHeight = r1.bottom - r1.top;
	int deleteX = cx - deleteWidth - margin.right;
	int deleteY = cy - deleteHeight - margin.bottom;

	m_deleteButton.SetWindowPos(NULL, deleteX, deleteY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	m_ignoreButton.GetWindowRect(&r1);

	int ignoreWidth = r1.right - r1.left;
	int ignoreHeight = r1.bottom - r1.top;
	int ignoreX = deleteX - INTERBUTTON - ignoreWidth;
	int ignoreY = deleteY + (deleteHeight - ignoreHeight)/2;  // center ignore checkbox vert. wrt. delete button

	m_ignoreButton.SetWindowPos(NULL, ignoreX, ignoreY, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	m_sayWnd->GetWindowRect(&r1);
	int sayHeight = r1.bottom - r1.top;
	///
	m_tabCtrl.GetWindowRect(&r1);

	m_tabCtrl.SetWindowPos(NULL, margin.left, margin.top,
							cx - margin.left - margin.right, cy - metrics.bottom, SWP_NOZORDER);
	
	m_tabCtrl.GetWindowRect(&r1);
	ScreenToClient(&r1);
	m_tabCtrl.AdjustRect(FALSE, &r1);
	m_richRect = r1;
	if (m_currentIndex >= 0) {
		CWhisperLeaf *currentLeaf = (CWhisperLeaf*) m_leaves[m_currentIndex];
		currentLeaf->m_richView->SetWindowPos(&wndTop, m_richRect.left, m_richRect.top, m_richRect.right - m_richRect.left,
											  m_richRect.bottom - m_richRect.top, SWP_SHOWWINDOW);
	}

	m_sayWnd->SetWindowPos(NULL, m_richRect.left, cy - metrics.top,
							m_richRect.right - m_richRect.left, sayHeight, SWP_NOZORDER);

	m_deleteButton.RedrawWindow();  // otherwise, damage can appear on the buttons
	m_ignoreButton.RedrawWindow();	//  ... but why doesn't UpdateWindow work?
	m_sayWnd->RedrawWindow();

	SaveWhisperCoords();
}

void CWhisperBox::OnMove(int x, int y) 
{
	CDialog::OnMove(x, y);
	
	SaveWhisperCoords();
}

void CWhisperBox::SaveWhisperCoords() {
	if (m_bPostCreate && !IsIconic())
		GetWindowRect(&theApp.m_rectWhisper);
}

void CWhisperBox::OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI ) {
	lpMMI->ptMinTrackSize.x = 267;
	lpMMI->ptMinTrackSize.y = 175;		// reasonable minimum size for whisperbox
}

void CWhisperBox::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState!= WA_INACTIVE && m_bInverted/* && !bMinimized*/) {
		FlashWindow(FALSE); // restore icon state on activation
		m_bInverted = FALSE;
	}
}


BOOL CWhisperBox::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (pMsg->wParam == VK_ESCAPE)
		{
			// Don't close whisperbox or kill current window on ESCAPE.  This would cause a problem if
			// the CSayCtrl passed the Escape on, it would kill the current frame (the CSayWnd).
			// Filtering out escapes cures two problems:
			//	1) The SayWnd disappearing on Escapes
			//	2) The WhisperBox disappearing on Escapes
			return TRUE;
		}
	}
	else if (pMsg->message == WM_SYSKEYDOWN)
	{
		if (pMsg->wParam >= '0' && pMsg->wParam <= '9')
		{
			if (m_currentIndex >= 0)
			{
				theApp.m_macros[pMsg->wParam - '0'].Invoke (NULL, NULL, FALSE, TRUE /*bInWhisperBox*/);
			}
			return TRUE;
		}
	}

	return CDialog::PreTranslateMessage(pMsg);
}

void 
CWhisperBox::OnContextMenu(
CWnd* pWnd, 
CPoint screenPoint)
{
	CMenu menu;
	if (screenPoint.x == -1 && screenPoint.y == -1) {  // then we must be invoking menu with Shift+F10
		CRect rect; // so lets set up our own point
		GetClientRect(&rect);
		screenPoint.x = (rect.right - rect.left)/2;
		screenPoint.y = (rect.bottom - rect.top)/2;
		ClientToScreen(&screenPoint);
	}
	menu.LoadMenu (IDR_STATUSVIEW);
	menu.DeleteMenu (ID_CLEAR_HISTORY, MF_BYCOMMAND);
	menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON,
					   				   screenPoint.x, screenPoint.y, AfxGetMainWnd());
}

BOOL 
CWhisperBox::OnNotify(
WPARAM wParam, 
LPARAM lParam, 
LRESULT* pResult) 
{
	NMHDR* pNMHDR = (NMHDR*)lParam;
	MSGFILTER* pMSGFILTER = (MSGFILTER*)lParam;
	if((pNMHDR->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONUP))
	{
		POINT screenPoint;
		screenPoint.x = LOWORD(pMSGFILTER->lParam);
		screenPoint.y = HIWORD(pMSGFILTER->lParam);
		(CWnd::FromHandle (pNMHDR->hwndFrom))->ClientToScreen(&screenPoint);
		SendMessage (WM_CONTEXTMENU, (WPARAM)pNMHDR->hwndFrom, MAKELPARAM(screenPoint.x, screenPoint.y));
	}
	return CDialog::OnNotify(wParam, lParam, pResult);
}

CRichEditCtrl* 
CWhisperBox::GetCurrentEdit()
{
	if (m_currentIndex < 0) 
		return NULL;
	CWhisperLeaf *pLeaf = (CWhisperLeaf*)m_leaves[m_currentIndex];
	ASSERT(pLeaf != NULL);
	return pLeaf->m_richView;
}
