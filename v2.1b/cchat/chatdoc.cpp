// chatDoc.cpp : implementation of the CChatDoc class
//

#include "stdafx.h"
#include "chat.h"

#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatDoc.h"
#include "binditem.h"
#include "ChatItem.h"

#include "setupdlg.h"
#include "spltchat.h"
#include "chatview.h"
#include <winreg.h>
#include "bbox.h"
#include "traj.h"
#include "spline.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "balloon.h"
#include "backdrop.h"
#include "memblst.h"
#include "pageview.h"
#include "panel.h"
#include "ui.h"
#include "bodycam.h"
#include "saywnd.h"
#include "histent.h"
//#include "proppage.h"
#include "textcore.h"
#include "textview.h"
#include "format.h"
#include "tabbar.h"	// for tabbar

#include <tchar.h>
#include <mbstring.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;
void ChatPartChannel(CChatDoc *, BOOL hardDisconnect);

#ifndef _DEBUG
// generate smaller code in release build
#define RELEASE(lpUnk) _AfxRelease((LPUNKNOWN*)&lpUnk)
#else
// generate larger but typesafe code in debug build
#define RELEASE(lpUnk) do \
	{ if ((lpUnk) != NULL) { (lpUnk)->Release(); (lpUnk) = NULL; } } while (0)
#endif

/////////////////////////////////////////////////////////////////////////////
// CChatDoc

IMPLEMENT_DYNCREATE(CChatDoc, CDocObjectServerDoc)

BEGIN_MESSAGE_MAP(CChatDoc, CDocObjectServerDoc)
	//{{AFX_MSG_MAP(CChatDoc)
	ON_UPDATE_COMMAND_UI(ID_EDIT_UNDO, OnUpdateEditUndo)
	ON_UPDATE_COMMAND_UI(ID_EDIT_CUT, OnUpdateEditCut)
	ON_UPDATE_COMMAND_UI(ID_EDIT_COPY, OnUpdateEditCopy)
	ON_UPDATE_COMMAND_UI(ID_EDIT_PASTE, OnUpdateEditPaste)
	ON_UPDATE_COMMAND_UI(ID_EDIT_DELETE, OnUpdateEditDelete)
	ON_UPDATE_COMMAND_UI(ID_EDIT_SELECTALL, OnUpdateEditSelectAll)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectAll)
	ON_COMMAND(ID_ACTIONS_SAY, OnActionsSay)
	ON_COMMAND(ID_ACTIONS_THINK, OnActionsThink)
	ON_COMMAND(ID_ACTIONS_WHISPER, OnActionsWhisper)
	ON_COMMAND(ID_MEMBER_GETINFO, OnMemberGetinfo)
	ON_COMMAND(ID_MEMBER_IGNORE, OnMemberIgnore)
	ON_UPDATE_COMMAND_UI(ID_MEMBER_GETINFO, OnUpdateMemberGetinfo)
	ON_UPDATE_COMMAND_UI(ID_MEMBER_IGNORE, OnUpdateMemberIgnore)
	ON_COMMAND(ID_FILE_CREATESHORTCUT, OnFileCreateshortcut)
	ON_COMMAND(ID_FAVORITES_ADDTOFAVORITES, OnFavoritesAddtofavorites)
	ON_COMMAND(ID_SEND_ACTION, OnSendAction)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMICS, OnUpdateViewComics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEXT, OnUpdateViewText)
	ON_COMMAND(ID_ADMINISTRATOR_KICK, OnAdministratorKick)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_CREATESHORTCUT, OnUpdateFileCreateshortcut)
	ON_UPDATE_COMMAND_UI(ID_FAVORITES_ADDTOFAVORITES, OnUpdateFavoritesAddtofavorites)
	ON_UPDATE_COMMAND_UI(ID_SESSION_LEAVE, OnUpdateLeave)
	ON_COMMAND(ID_CHANNELPROPS, OnChannelprops)
	ON_COMMAND(ID_ADMIN_BGRNDSYNC, OnAdminBgrndsync)
	ON_UPDATE_COMMAND_UI(ID_ADMIN_BGRNDSYNC, OnUpdateAdminBgrndsync)
	ON_COMMAND(ID_ADMIN_BAN, OnAdminBan)
	ON_UPDATE_COMMAND_UI(ID_ADMIN_BAN, OnUpdateAdminBan)
	ON_UPDATE_COMMAND_UI(ID_CHANNELPROPS, OnUpdateChannelprops)
	ON_COMMAND(ID_MAKESPEAKER, OnMakespeaker)
	ON_COMMAND(ID_MAKESPECTATOR, OnMakespectator)
	ON_COMMAND(ID_MAKEADMIN, OnMakeadmin)
	ON_UPDATE_COMMAND_UI(ID_MAKEADMIN, OnUpdateMakeadmin)
	ON_UPDATE_COMMAND_UI(ID_MAKESPEAKER, OnUpdateMakespeaker)
	ON_UPDATE_COMMAND_UI(ID_MAKESPECTATOR, OnUpdateMakespectator)
	ON_COMMAND(ID_INVITE, OnInvite)
	ON_UPDATE_COMMAND_UI(ID_INVITE, OnUpdateInvite)
	ON_COMMAND(ID_GETIDENTITY, OnGetidentity)
	ON_UPDATE_COMMAND_UI(ID_GETIDENTITY, OnUpdateGetidentity)
	ON_COMMAND(ID_GET_VERSION, OnGetVersion)
	ON_COMMAND(ID_PLAY_SOUND, OnPlaySound)
	ON_COMMAND(ID_PING_USER, OnPingUser)
	ON_COMMAND(ID_GET_LOCALTIME, OnGetLocaltime)
	ON_COMMAND(ID_WHISPERBOX_MLIST, OnWhisperboxMlist)
	ON_UPDATE_COMMAND_UI(ID_ADMINISTRATOR_KICK, OnUpdate1SelectionNotSelf)
	ON_COMMAND(ID_SETFONT, OnSetfont)
	ON_COMMAND(ID_SETCOLOR, OnSetColor)
	ON_COMMAND(ID_SWITCHBOLD, OnSwitchBold)
	ON_COMMAND(ID_SWITCHITALIC, OnSwitchItalic)
	ON_COMMAND(ID_SWITCHUNDERLINED, OnSwitchUnderlined)
	ON_COMMAND(ID_SWITCHFIXEDPITCH, OnSwitchFixedPitch)
	ON_COMMAND(ID_SWITCHSYMBOL, OnSwitchSymbol)
	ON_UPDATE_COMMAND_UI_RANGE(ID_SETCOLOR, ID_SWITCHSYMBOL, OnUpdateFormat)
	ON_COMMAND(ID_SEND_EMAIL, OnSendEmail)
	ON_COMMAND(ID_VISIT_HOMEPAGE, OnVisitHomepage)
	ON_COMMAND(ID_START_NETMEETING, OnStartNetmeeting)
	ON_UPDATE_COMMAND_UI(ID_SEND_EMAIL, OnUpdateComicUserNotSelf)
	ON_UPDATE_COMMAND_UI(ID_START_NETMEETING, OnUpdateStartNetmeeting)
	ON_COMMAND(ID_SEND_FILE, OnSendFile)
	ON_COMMAND(ID_SESSION_LEAVE, OnLeave)
	ON_COMMAND(ID_VIEW_COMICS, OnViewComics)
	ON_COMMAND(ID_VIEW_TEXT, OnViewText)
	ON_COMMAND(ID_VIEW_ICON, OnViewIcon)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ICON, OnUpdateViewIcon)
	ON_COMMAND(ID_VIEW_LIST, OnViewList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIST, OnUpdateViewList)
	ON_UPDATE_COMMAND_UI(ID_GET_VERSION, OnUpdateGetidentity)
	ON_UPDATE_COMMAND_UI(ID_PING_USER, OnUpdateGetidentity)
	ON_UPDATE_COMMAND_UI(ID_GET_LOCALTIME, OnUpdateGetidentity)
	ON_UPDATE_COMMAND_UI(ID_WHISPERBOX_MLIST, OnUpdate1SelectionNotSelf)
	ON_UPDATE_COMMAND_UI(ID_VISIT_HOMEPAGE, OnUpdateMemberGetinfo)
	ON_UPDATE_COMMAND_UI(ID_SEND_FILE, OnUpdate1SelectionNotSelf)
	//}}AFX_MSG_MAP
	ON_COMMAND_RANGE(ID_MACRO_A0, ID_MACRO_A9, OnMacro)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatDoc construction/destruction

CPtrList g_docs;

CChatDoc::CChatDoc()
{
	// Use OLE compound files
//	EnableCompoundFile();
	m_comicsTitle = NULL;
	m_bRemember = TRUE;
	//changed this back to TRUE to be consistent with IE3.0
//  m_bRemember = FALSE;// mfc doesnt release its root storage so on exit it could try to close the 
	// file and if the disk isnt there anymore (A:), it will blue screen.  By turning off m_bRemember, 
	// it will release the storage each time.  A small tradeoff in performance, but works ok now.
	m_bIconMembers = m_bLastMemberView = theApp.m_bIconMembers;
	m_memberList = m_sayWnd = m_bodyCam = NULL;
	m_puiSelf = NULL;
	m_myAvatarID = 0;
	m_myBackDropID = 0;
	CRoomInfo *NewDefaultProto(CChatDoc *);
	m_proto = NewDefaultProto(this);
	m_bNewContent = FALSE;
	m_seed = rand();
	m_client = NULL;
	m_bObscured = FALSE;
	g_docs.AddHead(this);
}

CChatDoc::~CChatDoc()
{
	POSITION pos = g_docs.Find(this);
	if (pos)
		g_docs.RemoveAt(pos);

	if (g_docs.IsEmpty())
		cui.m_pvChatView = NULL;	// No current chat view

	if (m_proto)
		delete m_proto;

	DestroyPages();
}

void CChatDoc::InitMyDocument() {
	srand(m_seed);
	// Set up art dir to be default (unless overridden later)
	void SetArtDir(const char *);
	SetArtDir(theApp.m_strDefaultArtDir);

	if (!m_bComicView) return;		// only has to do with Comics mode

	ASSERT(m_pages.IsEmpty());
	AddNewPage();

	// Add title panel -- make sure to do this in proper document context!
	//   Could this be done instead from the StartHistoryEntry (more efficiently?)
	CChatDoc *oldDoc = NULL;
	if (this != GetChatDoc()) {    // set up state
		oldDoc = GetChatDoc();
		LoadDocData();
	}

	CPage *firstPage = (CPage *) (m_pages.GetHead());
	firstPage->AddTitle(GetComicsTitle());		// give it a title

	if (oldDoc) oldDoc->LoadDocData();  // restore old state
}



char *CChatDoc::GetComicsTitle() {
	char *GetRandomTitle();
	char *title = m_bComicView ? m_comicsTitle : "";
	if (!title) title = m_comicsTitle = GetRandomTitle();
	return title;
}


void CChatDoc::SetComicsTitle(char *title) {
	if (m_comicsTitle) free(m_comicsTitle);
	m_comicsTitle = strdup(title);
}


// to be called only after InitMyDocument, to change title
// note: this clears all panels and puts up the new title.  Would be better
//    to have a function that splices in the change, but this is ok for now
//    since it is only called before any non-title panels are generated
void CChatDoc::SetComicsTitle2(char *newTitle) {
	if (m_comicsTitle) 
		free(m_comicsTitle);
	m_comicsTitle = strdup(newTitle);
	if (m_bComicView)
		((CPageView *)m_view)->ResetExistingPanels(TRUE);
}

BOOL CChatDoc::OnNewDocument()
{
	if (!CDocObjectServerDoc::OnNewDocument())
		return FALSE;

	m_fileType = FT_CCR;		// so we trigger a connection dialogue

	InitMyDocument();

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CChatDoc server implementation

COleServerItem* CChatDoc::OnGetEmbeddedItem()
{
	// OnGetEmbeddedItem is called by the framework to get the CChatItem
	//  that is associated with the document.  It is only called when necessary.

	CChatItem* pItem = new CChatItem(this);
	ASSERT_VALID(pItem);
	return pItem;
}

/////////////////////////////////////////////////////////////////////////////
// CChatDoc serialization

void CChatDoc::Serialize(CArchive& ar)
{
	// InitMyDocument needs to be done after DeleteContents, but before FileIn.
	// Hence we have to put it here.
	if (!ar.IsStoring())
		InitMyDocument();

	if (m_fileType == FT_CCR)
	{
		if (ar.IsStoring())
			ChatSaveLocator(ar);
		else
			ChatLoadLocator(ar, TRUE, TRUE);
	}
	else
	{		// FT_CCC probably!
		if (ar.IsStoring())
			ChatSaveConversation(ar);
		else
			ChatLoadConversation(ar);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChatDoc diagnostics

#ifdef _DEBUG
void CChatDoc::AssertValid() const
{
	CDocObjectServerDoc::AssertValid();
}

void CChatDoc::Dump(CDumpContext& dc) const
{
	CDocObjectServerDoc::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChatDoc commands

void CChatDoc::AddLine(UINT uID, const char *szText, BYTE byteMode, CDWordArray *prgdwFormatting)
{
	// AddToDialogHistory(uID, szText);				// first save in dialog history

	CPage *page = (CPage*) m_pages.GetTail();		// add a new panel to the current page
	if (!page->AddLine(uID, szText, byteMode, prgdwFormatting)) //   ... first, adding a new page if needed
	{
		AddNewPage();
		page = (CPage*) m_pages.GetHead();
		if (page)
			page->AddLine(uID, szText, byteMode, prgdwFormatting);
	}
}


void CChatDoc::AddNewPage() {
	CUnitPanelPage *firstPage = new CUnitPanelPage(this);		// create and add the initial page
	firstPage->m_topY = firstPage->m_leftX = 0;					// later calculate from existing pages
	m_pages.AddHead(firstPage);
}

#if 0
void CChatDoc::ReadSampleConversation(const char *filename) {
	char line[1000];
	FILE *fp;

	if ((fp = fopen(filename, "r")) == NULL)
		return;

	while (fgets(line, sizeof(line), fp)) {
		if (*line == '#') break;
		char *nl = strchr(line, '\n');			// null out new line
		if (nl) *nl = '\0';
		if (!(*line)) strcpy(line, "<Brk>");    // convert line breaks to panel breaks
		ProcessLine(0, line, SM_SAY, FALSE);
//		AddLine(NULL, line, SM_SAY);
	}

	fclose(fp);
}
#endif


void CChatDoc::ShowInfo(void *x_pui, const char *szInfo)
{   // simple proxy for current page.  really should check to see if fits on page
	CUserInfo *pui = (CUserInfo *) x_pui;

	if (m_bComicView)
	{
		void AssignArbitraryAvatar(CUserInfo *);

		CPage *page = (CPage *) m_pages.GetTail();
		if (!pui->GetAvatarID())
			AssignArbitraryAvatar(pui); // in case this is an out of band ShowInfo (e.g., flooding from out of channel)
		page->ShowInfo(pui->GetAvatarID(), szInfo);
	}
	else
		GetTextView()->ShowInfo(pui, szInfo);
}


void FindAttribution(const char *words, void *userInfo, const char **mesgPtr, UINT *id) {
	USHORT ExtractAvatarID(void *);
	if (*id) {
		*mesgPtr = words;
		return;
	}

	char *angle = strchr(words, '>');
	if (!angle) {
		*mesgPtr = words;
		*id = userInfo ? ExtractAvatarID(userInfo) : MyAvatarID();		// necessary, since this file is unaware of CUserInfo type
		return;
	}

	*id = 0;
	const char *sptr = words;
	while (sptr < angle) {
		if (isdigit(*sptr))
			*id = 10 * *id + (*sptr - '0');
		else {
			*id = userInfo ? ExtractAvatarID(userInfo) : MyAvatarID();
			*mesgPtr = words;
			return;
		}
		sptr++;
	}
	*mesgPtr = angle + 1;
}

#if 0
void FindAddressees(UINT speakerID, const char *words, const char **mesgPtr) {
	UINT id, addressees[10], nAddressees = 0;
	char *colon = strchr(words, ':');
	if (!colon) {
		*mesgPtr = words;
		return;
	}

	// fill up addressee array
	const char *sptr = words;
	while (isspace(*sptr) && sptr < colon) sptr++;
	while (sptr < colon) {
		id = 0;
		while (isdigit(*sptr)) {
			id = 10 * id + (*sptr - '0');
			sptr++;
		}
		while (isspace(*sptr) && sptr < colon) sptr++;
		if (!isdigit(*sptr) && sptr != colon) {
			// bogus entry -- don't change talkto, and return string as part of message
			*mesgPtr = words;
			return;
		}
		addressees[nAddressees++] = id;
	}

	// adjust addressees of avatar
	CAvatarX *av = GetAvatar(speakerID);
	av->ClearTalkTos();
	for (UINT i = 0; i < nAddressees; i++)
		av->SelectTalkTo(addressees[i], TRUE);

	*mesgPtr = colon + 1;
}

#endif

void FindPose(UINT speakerID, const char *words, const char **mesgPtr) {
	UINT id, addressees[10], nAddressees = 0;
	char *colon = strchr(words, '|');
	if (!colon) {
		*mesgPtr = words;
		return;
	}

	// fill up addressee array
	const char *sptr = words;
	while (isspace(*sptr) && sptr < colon) sptr++;
	while (sptr < colon) {
		id = 0;
		while (isdigit(*sptr)) {
			id = 10 * id + (*sptr - '0');
			sptr++;
		}
		while (isspace(*sptr) && sptr < colon) sptr++;
		if (!isdigit(*sptr) && sptr != colon) {
			// bogus entry -- don't change talkto, and return string as part of message
			*mesgPtr = words;
			return;
		}
		addressees[nAddressees++] = id;
	}

	if (nAddressees != 2) {
		*mesgPtr = words;
		return;
	}

	// adjust addressees of avatar
	CBodyDouble *body = new CBodyDouble(speakerID);
	CAvatarComplex *av = (CAvatarComplex *) GetAvatar(speakerID);
	av->SetFace(body, addressees[0]);
	av->SetTorso(body, addressees[1]);
	av->UpdateBody(body);

	*mesgPtr = colon + 1;
}

int FindBalloonType(const char *words, const char **mesgPtr) {
	char *colon = strchr(words, ';');
	if (!colon || colon != words+1 || !strchr("WST", *words)) {
		*mesgPtr = words;
		return SM_SAY;
	}

	*mesgPtr = colon + 1;

	if (*words == 'S') return SM_SAY;
	else if (*words == 'W') return SM_WHISPER;
	else if (*words == 'T') return SM_THINK;
	else return SM_SAY;

}


void CChatDoc::ProcessLine(UINT uID, const char *szLine, BYTE byteMode, BYTE bbCooked, CDWordArray *prgdwFormatting)
{
	CString strMesg(szLine);

	if (!bbCooked)
		ChatPreSendText(strMesg, uID);

	if (byteMode == SM_ACTION)
	{
		int iSpacePos = strMesg.Find(' ');
		if (iSpacePos >= 0)
			strMesg = strMesg.Mid(iSpacePos);
		CUserInfo *pui = (CUserInfo *) GetAvatar(uID)->m_userInfo;
		strMesg = pui->GetScreenName() + strMesg;
	}
	
	if (strcmp(strMesg, "<Brk>") != 0)
		TallySpeech(uID);
	AddLine(uID, strMesg, byteMode, prgdwFormatting);
}


void CChatDoc::TallySpeech(UINT id) {
	CAvatarX *av = GetAvatar(id);
	av->m_nSends++;
	// Could update title, but don't bother for now
}

int CChatDoc::FindFileType(const char *pszPathName) {
	CString ext = CString(pszPathName).Right(4);
	if (lstrcmpi(ext, _T(".ccr"))==0) return FT_CCR;
	else if (lstrcmpi(ext, _T(".rtf"))== 0) return FT_RTF;
	else return FT_CCC;
}

BOOL CChatDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_fileType = FindFileType(lpszPathName);

	if (!COleServerDoc::OnOpenDocument(lpszPathName))
		if (!ParseLocatorFile(lpszPathName))  // try parsing an unstructured file
			return FALSE;

	return TRUE;
}


BOOL CChatDoc::ParseLocatorFile(LPCTSTR lpszPathName) {
	// now create a CFile...
	CFile f;

	if(!f.Open(lpszPathName, CFile::modeRead)) {
		TRACE("Unable to open %s", lpszPathName);		// no signal to user for now
		return FALSE;
	}

	// now create a CArchive from the CFile...
	CArchive ar(&f, CArchive::load);

	return (ChatLoadLocator(ar, TRUE, FALSE));
}

BOOL CChatDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_fileType = FindFileType(lpszPathName);
	if (m_fileType == FT_RTF) {
		if (!m_bComicView) {
			BOOL WriteRTF(CRichEditCtrl *, const char *);
			CTextView *tv = (CTextView *) m_view;
			BOOL success = WriteRTF(tv->m_pRichEdit, lpszPathName);
			if (!success) {
				CString mesg;
				mesg.LoadString(ID_ERR_SAVE);
				VERIFY(ReplaceToken(mesg, CString("%1"), lpszPathName));
				AfxMessageBox(mesg);
			}
			return success;
		} else {
			AfxMessageBox(ID_RTF_NO_COMICS);
			return TRUE; // pretend we saved it successfully, so won't delete old file w/ same name (if exists)
		}
	} else
		return CDocObjectServerDoc::OnSaveDocument(lpszPathName);
}

void CChatDoc::DestroyPages() {
	POSITION pos = m_pages.GetHeadPosition();
	while (pos) {
		CPage *page = (CPage *) m_pages.GetNext(pos);
		delete page;
	}
	m_pages.RemoveAll();		// important, since m_pages may be reused
}

 
void CChatDoc::DeleteContents() 
{
	DestroyPages();				// delete the pages

	if (!m_bComicView && m_view) 
		((CTextView *)m_view)->ClearTextView();

	//	This is an SDI remnant, possibly still used for in-place activation.
	//  DJK - Check and possibly remove.
	//  Note that we need to make sure that m_view->GetDocument() == this, since
	//  on termination via Windows shutdown, there GetDocument() returns NULL, causing
	//  havoc in ResetExistingPanels
	if (m_bComicView && m_view && m_view->GetDocument() == this)
		((CPageView *)m_view)->ResetExistingPanels(FALSE);

	DestroyHistory();		// delete last chat room's dialog history

	m_proto->ChatPartChannel(this, FALSE);

	void DestroyUserInfos(CChatDoc *);
	DestroyUserInfos(this);

	if (m_comicsTitle) {
		free(m_comicsTitle);
		m_comicsTitle = NULL;
	}

	extern CChatDoc *g_easterDoc;
	if (g_easterDoc == this) g_easterDoc = NULL;	// stop the easter egg

	CDocObjectServerDoc::DeleteContents();
}


void CChatDoc::OnEditUndo() 
{
	GetPrimaryView()->GetFocus()->SendMessage(WM_UNDO, 0, 0);
}

// Note that this following code is barely held together with a strand of glue
//  It seems that Good Ol' IE3.0, takes focus when you go to the menu, so to 
// update the menu state, you cant just check focus, you have to track what 
// previously had focus and go with that.  That, plus the rules we imposed
// ourseles (like text view being read only) inspired the follwoing spaghetti code.
extern HWND hgPrevFocus;
void CChatDoc::OnUpdateEditUndo(CCmdUI* pCmdUI) 
{
	HWND hSayWnd;
	hSayWnd = GetSay()->GetSayEdit();

	if 	((GetFocus() == hSayWnd  && !theApp.m_bEmbedded) || 
		(hgPrevFocus == hSayWnd && theApp.m_bEmbedded))
	{
		pCmdUI->Enable(::SendMessage (hSayWnd, EM_CANUNDO, 0, 0));
	}
	else
		pCmdUI->Enable(FALSE);
}
void CChatDoc::OnUpdateEditCut(CCmdUI* pCmdUI) 
{
	HWND hSayWnd;
	LPDWORD lpdwStart, lpdwEnd;

	hSayWnd = GetSay()->GetSayEdit();
	VERIFY(hSayWnd);
	::SendMessage(hSayWnd, EM_GETSEL, (WPARAM)(LPDWORD)&lpdwStart, (LPARAM)(LPDWORD)&lpdwEnd);
	if (lpdwStart != lpdwEnd &&
		(GetFocus() == GetSay()->GetSayEdit() || 
		(hgPrevFocus == GetSay()->GetSayEdit() && theApp.m_bEmbedded)))
	{
		// >0 chars selected in edit control
		pCmdUI->Enable();
	}
	else
	{
		// 0 chars selected in edit control
		pCmdUI->Enable(FALSE);
	}		
}

void CChatDoc::OnUpdateEditCopy(CCmdUI* pCmdUI) 
{
	HWND hSayWnd, hTextWnd;
	LPDWORD lpdwStart, lpdwEnd;

	if (!m_bComicView)
		hTextWnd = GetTextView()->m_pRichEdit->m_hWnd;
	else
		hTextWnd = NULL;

	hSayWnd = GetSay()->GetSayEdit();

	VERIFY(hSayWnd);
	if 	((GetFocus() == hSayWnd  && !theApp.m_bEmbedded) || 
		(hgPrevFocus == hSayWnd && theApp.m_bEmbedded))
	{
		::SendMessage(hSayWnd, EM_GETSEL, (WPARAM)(LPDWORD)&lpdwStart, (LPARAM)(LPDWORD)&lpdwEnd);
	}
	else if ((GetFocus() == hTextWnd && !theApp.m_bEmbedded) || 
		(hgPrevFocus == hTextWnd && theApp.m_bEmbedded))
	{
		::SendMessage(hTextWnd, EM_GETSEL, (WPARAM)(LPDWORD)&lpdwStart, (LPARAM)(LPDWORD)&lpdwEnd);
	}

	if(lpdwStart != lpdwEnd)
	{
		// >0 chars selected in edit control
		pCmdUI->Enable();
	}
	else
	{
		// 0 chars selected in edit control
		pCmdUI->Enable(FALSE);
	}		
}

void CChatDoc::OnUpdateEditPaste(CCmdUI* pCmdUI) 
{

	OpenClipboard(NULL);
	if(!IsClipboardFormatAvailable(CF_TEXT))
	{
		pCmdUI->Enable(FALSE);
		CloseClipboard();
		return;
	}
	CloseClipboard();
	HWND hFocus = GetFocus();
	pCmdUI->Enable((hFocus == GetSay()->GetSayEdit() && !theApp.m_bEmbedded)|| 
		(hgPrevFocus == GetSay()->GetSayEdit() && theApp.m_bEmbedded));
}

void CChatDoc::OnUpdateEditDelete(CCmdUI* pCmdUI) 
{
	HWND hSayWnd;
	LPDWORD lpdwStart, lpdwEnd;

	hSayWnd = GetSay()->GetSayEdit();
	VERIFY(hSayWnd);
	::SendMessage(hSayWnd, EM_GETSEL, (WPARAM)(LPDWORD)&lpdwStart, (LPARAM)(LPDWORD)&lpdwEnd);
	if (lpdwStart != lpdwEnd &&
		(GetFocus() == GetSay()->GetSayEdit() || 
		(hgPrevFocus == GetSay()->GetSayEdit() && theApp.m_bEmbedded)))
	{
		// >0 chars selected in edit control
		pCmdUI->Enable();
	}
	else
	{
		// 0 chars selected in edit control
		pCmdUI->Enable(FALSE);
	}		
}

void CChatDoc::OnUpdateEditSelectAll(CCmdUI* pCmdUI) 
{

	HWND hTextWnd;

	if (!m_bComicView)
		hTextWnd = GetTextView()->m_pRichEdit->m_hWnd;
	else
		hTextWnd = NULL;

	if((!GetSay()->IsEmpty() && (GetFocus() == GetSay()->GetSayEdit()) || 
					(hgPrevFocus == GetSay()->GetSayEdit() && theApp.m_bEmbedded)) ||
					(GetFocus() == hTextWnd && !theApp.m_bEmbedded) || 
						(hgPrevFocus == hTextWnd && theApp.m_bEmbedded))
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);

					
}

void CChatDoc::OnEditCut() 
{
	GetPrimaryView()->GetFocus()->SendMessage(WM_CUT, 0, 0);
}

void CChatDoc::OnEditCopy() 
{
	GetPrimaryView()->GetFocus()->SendMessage(WM_COPY, 0, 0);
}

void CChatDoc::OnEditPaste() 
{
	GetPrimaryView()->GetFocus()->SendMessage(WM_PASTE, 0, 0);
}

void CChatDoc::OnEditDelete() 
{
	GetPrimaryView()->GetFocus()->SendMessage(WM_CLEAR, 0, 0);
}

void CChatDoc::OnEditSelectAll() 
{
	GetPrimaryView()->GetFocus()->SendMessage(EM_SETSEL, 0, (LPARAM)-1 );
}


void CChatDoc::OnActionsSay() 
{
	GetSay()->OnActionsSay();
}

void CChatDoc::OnActionsThink() 
{
	GetSay()->OnActionsThink();
}

void CChatDoc::OnActionsWhisper() 
{
	GetSay()->OnActionsWhisper();	
}

void CChatDoc::OnSendAction() 
{
	GetSay()->OnSendAction();
}

void CChatDoc::OnPlaySound() 
{
	GetSay()->OnPlaySound();
}

/*void CChatDoc::OnUpdateActionsSay(CCmdUI* pCmdUI) 
{
	GetSay()->OnUpdateActionsSay(pCmdUI);	
}*/

/*{
	GetSay()->OnUpdateActionsWhisper(pCmdUI);
}*/

#if 0
void CChatDoc::OnSendRing() 
{
	void ChatRingUser(CUserInfo *pui);

	CAvatarX *av = MyAvatar();
	int upper = av->m_talkTo.GetUpperBound();
	for (int i = 0; i <= upper; i++)
		ChatRingUser((CUserInfo *) GetAvatar(av->m_talkTo[i])->m_userInfo);
}

void CChatDoc::OnUpdateSendRing(CCmdUI* pCmdUI) 
{
	int TalkToCount();
	pCmdUI->Enable(TalkToCount());
}


void CChatDoc::OnOptionsProfile() 
{
	CProfileDialog profileDlg;

	profileDlg.DoModal();
	GetView()->SetFocus(); 
}

#endif

void CChatDoc::ExecuteHistory(int mode) {
	BOOL oldRefresh = theApp.m_bNoRefresh;
	if (mode != HM_LIVE) theApp.m_bNoRefresh = TRUE;		// turn off refresh if not live

	CChatDoc *oldDoc = NULL;
	if (this != GetChatDoc()) {    // set up state
		oldDoc = GetChatDoc();
		LoadDocData();
	}

	POSITION pos = m_history.GetHeadPosition();
	while (pos) {
		HistoryEntry *entry = (HistoryEntry *) m_history.GetNext(pos);
		entry->Execute(mode);
	}

	if (oldDoc) oldDoc->LoadDocData();  // restore old state

	theApp.m_bNoRefresh = oldRefresh;
}

void CChatDoc::DestroyHistory() {
	POSITION pos = m_history.GetHeadPosition();
	while (pos) {
		HistoryEntry *entry = (HistoryEntry *) m_history.GetNext(pos);
		delete entry;
	}
	m_history.RemoveAll();	// important, since we may reuse history list later
}

void CChatDoc::InitHistory() {
	const char *GetMyCharacter();
	DestroyHistory();	// in case there was something already 
	AddAndExecute(new StartHistoryEntry(GetComicsTitle(), GetMyCharacter(), 0), this);
	AddAndExecute(new ChangeBackDropEntry((const char *)theApp.m_lastBackDrop), this);
}

BOOL CChatDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString strExt, strNewName = lpszPathName;
	if (strNewName.IsEmpty())
	{
		CString strFilter, rtfFilter;
		strFilter.LoadString(IDS_CCC_FILTER);

		if (!m_bComicView)  // prepend .rtf filter (text mode default)
		{
			rtfFilter.LoadString(IDS_RTF_FILTER);
			strFilter = rtfFilter + strFilter;
		}

		CString strFinal = m_strPathName;  // we don't want to call possibly buggy code path below if it can be avoided -djk

		if (strFinal.IsEmpty())
		{
			LONG	lResult;
			CString	strSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\CChat.exe";
			HKEY	hExe;
			BYTE	Data[256];
			DWORD	cbData = 256;
			DWORD	type;
			char	drive[_MAX_DRIVE];
			char	dir[_MAX_DIR];
			char	fname[_MAX_FNAME];
			char	ext[_MAX_EXT];

			lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,LPCTSTR(strSubKey), 0, KEY_QUERY_VALUE, &hExe);

			lResult = RegQueryValueEx(hExe, "", 0, &type, Data, &cbData);

			_splitpath((char*) Data , drive, dir, fname, ext);

			CString strDir(dir);
			strFinal = drive;
			strFinal+= strDir + GetTitle();   // this is potentially a bug.  GetTitle() might not return a legal file save name
											  // hopefully if m_strPathName is empty, there is no current file, so GetTitle will be legal.
		}

		// Add an extension if there is none
		strExt = strFinal.Right(3);
		if (_tcsicmp(g_szCCCExt, (LPCTSTR) strExt) && _tcsicmp(g_szRTFExt, (LPCTSTR) strExt))
			strFinal += CString(".") + (m_bComicView ? CString(g_szCCCExt) : CString(g_szRTFExt));

		CChatFileDialog dlgFile(FALSE, g_szCCCExt, LPCTSTR(strFinal), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_EXPLORER,
								strFilter);
	
		dlgFile.m_ofn.nFilterIndex = 1;
		if (!m_bComicView && !_tcsicmp(g_szCCCExt, (LPCTSTR) strFinal.Right(3)))
			dlgFile.m_ofn.nFilterIndex = 2;

		if(dlgFile.DoModal() == IDOK)
		{
			strNewName = dlgFile.GetPathName();
			strExt = dlgFile.GetFileExt();
			if (_tcsicmp(g_szCCCExt, (LPCTSTR) strExt) && _tcsicmp(g_szRTFExt, (LPCTSTR) strExt))
				strNewName += CString(".") + CString(g_szCCCExt);
		}
		else
			return FALSE;
	}

	CWaitCursor wait;

	if (!OnSaveDocument(strNewName))
	{
		if (lpszPathName == NULL)
		{
			// be sure to delete the file
			TRY
			{
				CFile::Remove(strNewName);
			}
			CATCH_ALL(e)
			{
				TRACE0("Warning: failed to delete file after failed SaveAs.\n");
//				DELETE_EXCEPTION(e);
			}
			END_CATCH_ALL
		}
		return FALSE;
	}

	// reset the title and change the document name
	if (bReplace) {
		CString oldTitle;
		extern const char *g_szUnInitChannelName;
		// keep title if doc was ever a live room
		BOOL bKeepTitle = (m_proto->m_strChannel != g_szUnInitChannelName);
		if (bKeepTitle) oldTitle = GetTitle();
		SetPathName(strNewName);
		if (bKeepTitle) SetTitle(oldTitle);
		if (!theApp.m_bEmbedded) SetModifiedFlag(FALSE);
	}
	return TRUE;        // success
}

COleIPFrameWnd* CChatDoc::CreateInPlaceFrame(CWnd* pParentWnd)
{
	theApp.m_bEmbedded = TRUE;
	ASSERT(m_view);
	if (!m_bComicView)	
		((CTextView*) m_view)->SetURLBrowser(TRUE);

	// The following hack deletes the macro menu already inserted in CMainFrame's
	// menu (called earlier from OnInitDocument) and inserts a macro menu in the IPFrame.
	// There is probably a better place for the following code, and it would be really
	// nice if we could have a single location where this work is done, for both out-of-place
	// and in-place activation.  The reason why we need m_bIPMacroDone, is that this code
	// could be called again if we leave an embedded chat in IE, and return to it.
	if (theApp.m_pmenuMacro && !theApp.m_bIPMacroDone) {		// hack-alert
		theApp.m_pmenuMacro->DestroyMenu();
		delete theApp.m_pmenuMacro;
		theApp.m_pmenuMacro = NULL;
		InsertMacroMenu();   // Insert and fill the macro menu
		UpdateMacroMenu();
		theApp.m_bIPMacroDone = TRUE;
	}

	COleIPFrameWnd* pFrameWnd = (COleIPFrameWnd*) CDocObjectServerDoc::CreateInPlaceFrame(pParentWnd);

	ASSERT(pFrameWnd);

	pFrameWnd->SetActiveView(m_view, TRUE);
	cui.m_pvFocusedDoc = this; // make sure that doc is current ...
	LoadDocData();			   // ... could be done in ActiveView, but also done in CChildFrame instead)

	return pFrameWnd;
}


CUserInfo *GetSingleSelectedMember() {
	if (!GetMembers()) return NULL;
	CMemberListCtrl *box = &(GetMembers()->m_MemberListBox);
	if (box->GetSelectedCount() != 1) return NULL;
	int index = box->GetNextItem(-1, LVNI_SELECTED);
	if (index > -1) return (CUserInfo *) box->GetItemData(index);
	else return NULL;
}

extern CUserInfo *mousedPui;

void CChatDoc::OnMemberGetinfo() 
{
	GetChatView()->PostMessage(WM_COMMAND,ID_AVATARCONTEXT_GETINFO,0);
}

void CChatDoc::OnMemberIgnore() 
{
	GetChatView()->PostMessage(WM_COMMAND,ID_AVATARCONTEXT_IGNORE,0);
}

#if 0
void CChatDoc::OnMemberRing() 
{
	mousedPui = GetSingleSelectedMember();
	GetChatView()->PostMessage(WM_COMMAND,ID_AVATARCONTEXT_RING,0);
}
#endif


void CChatDoc::OnUpdateMemberIgnore(CCmdUI* pCmdUI) 
{
	extern CUserInfo *puiSelf;
	CUserInfo *pui = GetSingleSelectedMember();

	if (pui && pui != puiSelf) {
		pCmdUI->Enable(TRUE);
		if (pui->Ignored())
			pCmdUI->SetCheck(1);
		else
			pCmdUI->SetCheck(0);
	}
	else {
		pCmdUI->Enable(FALSE);
		pCmdUI->SetCheck(0);
	}
}

void CChatDoc::OnUpdateMemberGetinfo(CCmdUI* pCmdUI) 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui && pui->IsComicUser() && GetConnectionStatus() == CX_INCHANNEL)
		pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}


void CChatDoc::OnFileCreateshortcut() 
{
	char fname[_MAX_FNAME];

	// Get the document path to use as a link name
	CString strOKName = GetPathName();

	_splitpath(LPCTSTR(strOKName) , NULL, NULL, fname, NULL);
	
	CString strfname = CString(fname);
	CString strTotalName = strfname + ".ccr";

	CString strDesktop = theApp.GetDesktopOrFavorites(TRUE);

//	if(strDocTitle.Right(4) != ".ccr")  //different versions of commdlg.dll will return a title
//		strDocTitle+=".ccr";			//with a extension.  have to check for that.

	CString strLink = strDesktop + "\\" + strTotalName;
	AfxMessageBox(IDS_SHORTCUT);
	DoSave(strLink,FALSE);
}


void CChatDoc::OnFavoritesAddtofavorites() 
{
	char fname[_MAX_FNAME];

	// Get the document title to use as a link name
	CString strOKName = GetPathName();

	_splitpath(LPCTSTR(strOKName) , NULL, NULL, fname, NULL);
	
	CString strfname = CString(fname);
	CString strTotalName = strfname + ".ccr";

	CString strFavorites = theApp.m_strFavoritesDir;
//	if(strDocTitle.Right(4) != ".ccr")  //different versions of commdlg.dll will return a title
//		strDocTitle+=".ccr";			//with a extension.  have to check for that.
	CString strLink = strFavorites + "\\" + strTotalName;
	AfxMessageBox(IDS_FAVORITE);
	DoSave(strLink,FALSE);
}



void CChatDoc::OnViewComics() {
	if (m_bComicView) return;
	void MapNullAvatars(CChatDoc *);
	BOOL ArtDirsOK();
	void LaunchMicrosoftURL(UINT resourceID);

	theApp.m_bFoundArt = ArtDirsOK();		// In case they installed art while running chat
	if (!theApp.m_bFoundArt) {
		if (AfxMessageBox(IDS_NEEDART, MB_YESNO) == IDYES)
			LaunchMicrosoftURL(IDS_URL_FREESTUFF);
		return;
	}

	theApp.m_bComicView = m_bComicView = TRUE;
	GetChatView()->CleanUpBeforeChangeView();
	m_fileType = FT_CCC;  // don't trigger connect dialogue
	GetChatView()->CreateComicView();
	MapNullAvatars(this);    // must be done before title panel created in ResetExistingPanels
	CPageView *pv = (CPageView *)GetView();
	pv->ResetExistingPanels(TRUE);
	RegenerateView();
	pv->PostMessage(WM_KEYDOWN,VK_END);
	pv->UpdateScroll();
}

void CChatDoc::OnViewText() {
	if (!m_bComicView) return;

	theApp.m_bComicView = m_bComicView = FALSE;
	CChatView *chatView = GetChatView();
	chatView->CleanUpBeforeChangeView();
	m_fileType = FT_CCC;  // don't trigger connect dialogue
	chatView->CreateTextView();
	DestroyPages();			// delete the comics already created
	RegenerateView();
}



void CChatDoc::OnUpdateViewComics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!(m_proto->m_dwModes & CM_NOFORMAT));
	pCmdUI->SetRadio(m_bComicView);
}

void CChatDoc::OnUpdateViewText(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(!m_bComicView);
}


void CChatDoc::OnViewListAux() 
{
	HWND hWnd = ((CMemberList *)m_memberList)->m_MemberListBox.m_hWnd;
	LONG lStyle = GetWindowLong(hWnd,GWL_STYLE);
	lStyle &= ~LVS_ICON;
	lStyle |= LVS_REPORT;
	SetWindowLong(hWnd,GWL_STYLE,lStyle);
	((CMemberList *)m_memberList)->m_MemberListBox.SetImageList(NULL, LVSIL_STATE);
	m_bIconMembers = FALSE;
}

void CChatDoc::OnViewList() {
	OnViewListAux();
	theApp.m_bIconMembers = FALSE;
}

void CChatDoc::OnViewIcon() 
{
	HWND hWnd = ((CMemberList *)m_memberList)->m_MemberListBox.m_hWnd;
	LONG lStyle = GetWindowLong(hWnd,GWL_STYLE);
	lStyle &= ~LVS_REPORT;
	lStyle |= LVS_ICON;
	SetWindowLong(hWnd,GWL_STYLE,lStyle);
	((CMemberList *)m_memberList)->m_MemberListBox.SetImageList(&theApp.m_StatusIcons, LVSIL_STATE);
	theApp.m_bIconMembers = m_bIconMembers = TRUE;
}

void CChatDoc::OnMacro(UINT nID)
{
	int mNum = nID - ID_MACRO_A0;

	if (mNum < 0 || mNum >= NMACROS)
		return;

	theApp.m_macros[mNum].Invoke();
	TRACE("Got macro: %d\n", nID);
}

void CChatDoc::OnUpdateViewIcon(CCmdUI* pCmdUI) 
{
	if (m_bComicView) {
		pCmdUI->Enable(TRUE);
		pCmdUI->SetRadio(m_bIconMembers);
	}
	else {
		pCmdUI->Enable(FALSE);
		pCmdUI->SetRadio(FALSE);
	}

}

void CChatDoc::OnUpdateViewList(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(!m_bIconMembers);	
}


CMenu *CChatDoc::GetMenu(int index) {
	// first get the main menu
	CWnd* pWnd = AfxGetMainWnd();
	CMenu* pMenu;
	CMenu* pSub;
	HMENU hMenu;
	if (theApp.m_bEmbedded) {
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT_VALID(pTemplate);
		hMenu =  pTemplate->m_hMenuInPlaceServer;
		pMenu = new CMenu;
		pMenu->Attach(hMenu);
		pSub = pMenu->GetSubMenu(index-1);
		pMenu->Detach();
		delete pMenu;
	} else {
		pMenu = pWnd->GetMenu();
		if (!pMenu) return NULL; //possibility on exit
		if (pMenu->GetSubMenu(0)->GetMenuItemID(0) == SC_RESTORE) index++;  // account for system menu when maximized (HACK!!!)
		pSub = pMenu->GetSubMenu(index);
	}

	return pSub;
}

#define MEMBERMENUPOS	5

void CChatDoc::InsertAdminMenu() 
{
	CWnd* pWnd = AfxGetMainWnd();
	CMenu* pSub = GetMenu(MEMBERMENUPOS);
	if (!pSub) return;

	// get a menu object representing the menu to insert
	theApp.m_pmenuAdmin = new CMenu;
	theApp.m_pmenuAdmin->LoadMenu(IDR_ADMIN);

	CMenu* pAdminSub = theApp.m_pmenuAdmin->GetSubMenu(0);

	//and insert
	int nItems = pSub->GetMenuItemCount();				      // insert at bottom
	pSub->InsertMenu(nItems,MF_BYPOSITION | MF_SEPARATOR);  // separator first
	CString label;
	label.LoadString(IDS_ADMINMENU_LABEL);
	BOOL bResult = pSub->InsertMenu(nItems+1,MF_BYPOSITION | MF_POPUP,(UINT)pAdminSub->m_hMenu, label);
	
	// make sure to redraw the menubar
	pWnd->DrawMenuBar();
}

void CChatDoc::RemoveAdminMenu()
{
	// first get the main menu
	if(theApp.m_pmenuAdmin) {
		CWnd* pWnd = AfxGetMainWnd();
		if (!pWnd) return;				// on exit, this may be true
		CMenu* pSub = GetMenu(MEMBERMENUPOS);

		if (!pSub) return;

		//remove the administrator menu
		int nItems = pSub->GetMenuItemCount();
		pSub->RemoveMenu(nItems-1,MF_BYPOSITION);
		pSub->RemoveMenu(nItems-2,MF_BYPOSITION);
		pWnd->DrawMenuBar();
		//clean up
		theApp.m_pmenuAdmin->DestroyMenu();
		delete theApp.m_pmenuAdmin;
		theApp.m_pmenuAdmin = NULL;
	}
}

void CChatDoc::UpdateAdminMenu() {
	extern CUserInfo *puiSelf;
	if (!puiSelf) return;
	if (theApp.m_pmenuAdmin) {
		if (!puiSelf->IsOperator())
			RemoveAdminMenu();		
	} else {
		if (puiSelf->IsOperator())
			InsertAdminMenu();
	}
}

void CChatDoc::InsertMacroMenu() {
	// first get the main menu
	CWnd* pWnd = AfxGetMainWnd();
	CMenu* pSub = GetMenu(2);
	if (!pSub) return;

	// get a menu object representing the menu to insert
	theApp.m_pmenuMacro = new CMenu;
	theApp.m_pmenuMacro->CreatePopupMenu();
//	m_pmenuMacro->LoadMenu(IDR_ADMIN);

//	CMenu* pAdminSub = m_pmenuMacro->GetSubMenu(0);

	//and insert
	int nItems = pSub->GetMenuItemCount();				      // insert at bottom - 1
	CString label;
	label.LoadString(IDS_MACROMENU_LABEL);
	BOOL bResult = pSub->InsertMenu(nItems-1,MF_BYPOSITION | MF_POPUP,(UINT)theApp.m_pmenuMacro->m_hMenu, label);
	
	// make sure to redraw the menubar
	pWnd->DrawMenuBar();
}

void CChatDoc::UpdateMacroMenu() {
	BOOL bEmpty = TRUE;

	if (!theApp.m_pmenuMacro) return;
	while (theApp.m_pmenuMacro->RemoveMenu(0, MF_BYPOSITION));
	for (int i = 0; i < NMACROS; i++) {
		if (theApp.m_macros[i].m_bDefined) {
			CString name;
			name.Format("%s\tAlt+%d", theApp.m_macros[i].m_strName, i);
			theApp.m_pmenuMacro->AppendMenu(MF_STRING, ID_MACRO_A0+i, name);
			bEmpty = FALSE;
		}
	}

	// Handle macros menu disabling/enabling
	CMenu *pMenuView = GetMenu(2);
	if (!pMenuView) return;
	int nItems = pMenuView->GetMenuItemCount();				      // menu at bottom - 1
	VERIFY(pMenuView->EnableMenuItem(nItems-2, MF_BYPOSITION | (bEmpty ? MF_GRAYED : MF_ENABLED)) != -1);
}


void CChatDoc::OnAdministratorKick() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui) m_proto->ChatKickUser(pui);
}

void CChatDoc::OnUpdate1SelectionNotSelf(CCmdUI* pCmdUI) 
{
	extern CUserInfo *puiSelf;
	CUserInfo *pui = GetSingleSelectedMember();

	pCmdUI->Enable(pui && pui != puiSelf);
}

void CChatDoc::OnUpdateComicUserNotSelf(CCmdUI* pCmdUI) 
{
	extern CUserInfo *puiSelf;
	CUserInfo *pui = GetSingleSelectedMember();

	pCmdUI->Enable(pui && pui != puiSelf && pui->IsComicUser());
}

void CChatDoc::OnAdminBan() 
{
	void ChatBanUser(CUserInfo *);
	CUserInfo *pui = GetSingleSelectedMember();
	m_proto->ChatBanUser(pui);	
}

void CChatDoc::OnUpdateAdminBan(CCmdUI* pCmdUI) 
{
	extern CUserInfo *puiSelf;
	CUserInfo *pui = NULL;
	int nSel = 2;
	if (GetMembers()) {
		CMemberListCtrl *box = &(GetMembers()->m_MemberListBox);
		nSel = box->GetSelectedCount();
		if (nSel == 1) pui = GetSingleSelectedMember();
	}

	pCmdUI->Enable(nSel < 2 && (!pui || pui != puiSelf)); // only if 1 or none selected, and self isn't selected
}

BOOL bCanInvite() {
	CChatDoc *doc = GetChatDoc();
	return (doc && doc->GetConnectionStatus() == CX_INCHANNEL && doc->m_puiSelf &&
				 (doc->m_puiSelf->IsOperator() || !(doc->m_proto->m_dwModes & CM_INVITEONLY)));
}

void CChatDoc::OnInvite() 
{
	m_proto->ChatInvite();
}

void CChatDoc::OnUpdateInvite(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(bCanInvite());
}



void CChatDoc::RegenerateView() {
	void RepopulateMemberList();
	GetPrimaryView()->SetRedraw(FALSE);
	ExecuteHistory(HM_RELOAD);  	// now repopulate panel with text info
	GetPrimaryView()->SetRedraw(TRUE);
	GetPrimaryView()->Invalidate(FALSE);
	RepopulateMemberList();
}

void CChatDoc::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);   // always can save
	
}

void CChatDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);	// always can save as
}


void CChatDoc::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);   // always can print
}

void CChatDoc::OnUpdateFileCreateshortcut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetConnectionStatus() == CX_INCHANNEL);
}

void CChatDoc::OnUpdateFavoritesAddtofavorites(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetConnectionStatus() == CX_INCHANNEL);
}


void CChatDoc::OnLeave() 
{
	OnFileClose();	
}

void CChatDoc::OnUpdateLeave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetConnectionStatus() == CX_INCHANNEL);
}

BOOL CChatDoc::CleanupExistingWindows() {
	CChatDoc *reuseDoc = NULL;
	POSITION pos = g_docs.GetHeadPosition();
	while (pos) {
		CChatDoc *doc = (CChatDoc *) g_docs.GetNext(pos);
		if (doc->m_proto->GetType() != PC_IRC) continue;   // for now, close only IRC windows
		if (!reuseDoc)
			reuseDoc = doc;
		else {
			if (!doc->SaveModified())
				return FALSE;
			else doc->OnCloseDocument();
		}
	}

	if (reuseDoc) {
		if (!reuseDoc->SaveModified())
			return FALSE;
		reuseDoc->OnNewDocument();
		if (reuseDoc->m_proto)
			reuseDoc->m_proto->m_strChannel = "";  // so it can be reclaimed
	}
	return TRUE;
}


void CChatDoc::OnMakeadmin() // toggles admin status
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui && (GetConnectionStatus() == CX_INCHANNEL))
		m_proto->ChatSetOperator(pui, UM_HOST);
}

void CChatDoc::OnMakespeaker() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui && (GetConnectionStatus() == CX_INCHANNEL))
		m_proto->ChatSetOperator(pui, UM_SPEAKER);
}


void CChatDoc::OnMakespectator() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui && (GetConnectionStatus() == CX_INCHANNEL))
		m_proto->ChatSetOperator(pui, UM_SPECTATOR);	
}

extern CUserInfo *puiSelf;

void CChatDoc::OnUpdateMakeadmin(CCmdUI* pCmdUI) 
{
	CUserInfo *pui = GetSingleSelectedMember();
	BOOL enabled = pui && pui != puiSelf && puiSelf->IsOperator() && (GetConnectionStatus() == CX_INCHANNEL);
	pCmdUI->Enable(enabled);
	pCmdUI->SetRadio(pui && pui->IsOperator());
}

void CChatDoc::OnUpdateMakespeaker(CCmdUI* pCmdUI) 
{
	CUserInfo *pui = GetSingleSelectedMember();
	BOOL enabled = pui && pui != puiSelf && puiSelf->IsOperator() && (GetConnectionStatus() == CX_INCHANNEL);
	pCmdUI->Enable(enabled);
	pCmdUI->SetRadio(pui && pui->IsSpeaker() && !pui->IsOperator()); // note speaker and operator not disjoint	
}

void CChatDoc::OnUpdateMakespectator(CCmdUI* pCmdUI) 
{
	CUserInfo *pui = GetSingleSelectedMember();
	BOOL isModerated = m_proto->m_dwModes & CM_MODERATED;
	BOOL enabled = pui && pui != puiSelf && puiSelf->IsOperator() && (GetConnectionStatus() == CX_INCHANNEL) && isModerated;
	pCmdUI->Enable(enabled);
	pCmdUI->SetRadio(pui && pui->IsSpectator());	
}


void CChatDoc::OnChannelprops() 
{
	m_proto->DoChannelDialog();
}

void CChatDoc::OnUpdateChannelprops(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(GetConnectionStatus() == CX_INCHANNEL);
}

void CChatDoc::OnAdminBgrndsync() 
{
	if (m_bComicView) 
		m_proto->ChatSyncBackDrop(theApp.m_lastBackDrop);
}

void CChatDoc::OnUpdateAdminBgrndsync(CCmdUI* pCmdUI) 
{
	// only if user is in comics mode	
	pCmdUI->Enable(GetConnectionStatus() == CX_INCHANNEL && m_bComicView);
}


void CChatDoc::OnGetidentity() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	m_proto->ChatGetIdentity(pui);
}

void CChatDoc::OnUpdateGetidentity(CCmdUI* pCmdUI) 
{
	CUserInfo *pui = GetSingleSelectedMember();
	pCmdUI->Enable(pui && !pui->IsDeparted() && GetConnectionStatus() == CX_INCHANNEL);
}

void CChatDoc::OnGetVersion() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	m_proto->ChatGetVersion(pui);
}


void CChatDoc::OnPingUser() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	m_proto->ChatPingUser(pui);
}



void CChatDoc::OnGetLocaltime() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	m_proto->ChatGetLocalTime(pui);
}



void CChatDoc::OnWhisperboxMlist() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	void WhisperBox(CUserInfo *);
	WhisperBox(pui);
}

void CChatDoc::SetLegalPath(const char *roomName, BOOL bAddToMRU) {
	// this is a wrapper for CDocument::SetPathName, necessary since that routine will
	// only work properly if there is a legal filename in the proposed basename.
	// So, here we check for the illegal chars (in roomName), replace them with %'s for
	// SetPathName, and then if a substitution was necessary, changes the title back to
	// the originally proposed roomName w/ the illegal filename characters.  This workaround
	// is required since the doc title should have the name of the room, yet this might
	// not be legal for the doc's true roomName.
	char *illegalChar;
	BOOL subbed = FALSE;
	char *fileName = strdup(roomName);

//	while (illegalChar = _tcspbrk(fileName, "\\/:*?\"<>|")) {
	while (illegalChar = (char *) _mbspbrk((UCHAR *)fileName, (UCHAR *)"\\/:*?\"<>|.")) {
		*illegalChar = '%';  // substitute in a %
		subbed = TRUE;
	}

	SetPathName(fileName, bAddToMRU);
	if (subbed) SetTitle(roomName);
	free(fileName);
}


void CChatDoc::OnSetfont() 
{
	void SetComicsFont(), SetTextFont();

	if (m_bComicView)
		SetComicsFont();
	else
		SetTextFont();
}


void CChatDoc::OnSetColor()
{
	OutputDebugString("OnSetColor\n");
	GetSay()->SwitchSelectionFormat(wForeground);
}


void CChatDoc::OnSwitchBold()
{
	OutputDebugString("OnSwitchBold\n");
	GetSay()->SwitchSelectionFormat(wBold);
}


void CChatDoc::OnSwitchItalic()
{
	OutputDebugString("OnSwitchItalic\n");
	GetSay()->SwitchSelectionFormat(wItalic);
}


void CChatDoc::OnSwitchUnderlined()
{
	OutputDebugString("OnSwitchUnderlined\n");
	GetSay()->SwitchSelectionFormat(wUnderline);
}


void CChatDoc::OnSwitchFixedPitch()
{
	OutputDebugString("OnSwitchFixedPitch\n");
	GetSay()->SwitchSelectionFormat(wFixedPitch);
}


void CChatDoc::OnSwitchSymbol()
{
	OutputDebugString("OnSwitchSymbol\n");
	GetSay()->SwitchSelectionFormat(wSymbol);
}


void CChatDoc::OnUpdateFormat(CCmdUI* pCmdUI)
{
	ASSERT(pCmdUI);
	ASSERT(GetSay());

	WORD wFormat = GetSay()->wGetConsistentFormats();

	switch (pCmdUI->m_nID)
	{
	case ID_SETCOLOR:
		// REGISB: ask Janise &| David if color should be checked when none default color is selected
		break;
	case ID_SWITCHBOLD:
		pCmdUI->SetCheck((wFormat & wBold) ? 1 : 0);
		break;
	case ID_SWITCHITALIC:
		pCmdUI->SetCheck((wFormat & wItalic) ? 1 : 0);
		break;
	case ID_SWITCHUNDERLINED:
		pCmdUI->SetCheck((wFormat & wUnderline) ? 1 : 0);
		break;
	case ID_SWITCHFIXEDPITCH:
		pCmdUI->SetCheck((wFormat & wFixedPitch) ? 1 : 0);
		break;
	case ID_SWITCHSYMBOL:
		pCmdUI->SetCheck((wFormat & wSymbol) ? 1 : 0);
	}
}


void CChatDoc::OnSendEmail() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui) m_proto->ChatGetEmail(pui);
}

void CChatDoc::OnVisitHomepage() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui) m_proto->ChatGetHomePage(pui);
}

void CChatDoc::OnStartNetmeeting() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	m_proto->ChatStartNetMeeting(pui);
}


void CChatDoc::OnUpdateStartNetmeeting(CCmdUI* pCmdUI) 
{
	BOOL bNMInstalled();
	extern CUserInfo *puiSelf;
	CUserInfo *pui = GetSingleSelectedMember();

	pCmdUI->Enable(!bNMInstalled() || (pui && pui != puiSelf && pui->IsComicUser()));	
}


void CChatDoc::OnSendFile() 
{
	CUserInfo *pui = GetSingleSelectedMember();
	m_proto->ChatSendFile(pui);
}


#if 0
// necessary fix for embedded app???
void CChatDoc::OnCloseDocument() 
{
	ChatPartChannel(this, TRUE);			// Force a chatsock cleanup before panes are closed in DeleteContents
	CDocObjectServerDoc::OnCloseDocument();
}
#endif

CMemberList *GetMembers() {
	CChatDoc *pDoc = GetChatDoc();
	ASSERT(pDoc);
	return ((CMemberList *)pDoc->m_memberList);
}

CBodyCam *GetBodyCam() {
	CChatDoc *pDoc = GetChatDoc();
	ASSERT(pDoc);
	return ((CBodyCam *)pDoc->m_bodyCam);
}

CSayWnd *GetSay() {
	CChatDoc *pDoc = GetChatDoc();
	return (pDoc ? (CSayWnd *)pDoc->m_sayWnd : NULL);
}

CTextView *GetTextView() {
	CChatDoc *pDoc = GetChatDoc();
	return pDoc ? (CTextView*) pDoc->m_view : NULL;
}

CView *GetPrimaryView() {
	CChatDoc *pDoc = GetChatDoc();
	ASSERT(pDoc);
	return (pDoc->m_view);
}


CPageView *GetView() {
	CChatDoc *pDoc = GetChatDoc();
	ASSERT(pDoc);
	return ((CPageView *)pDoc->m_view);
}

CChatDoc *LookupDoc(const char *channel) {
	POSITION pos = g_docs.GetHeadPosition();
	while (pos) {
		CChatDoc *doc = (CChatDoc *) g_docs.GetNext(pos);
		if (stricmp(doc->m_proto->m_strChannel, channel) == 0)
			return doc;
	}
	return NULL;
}



void CChatDoc::LoadDocData() {
	extern CUserInfo *puiSelf;
	extern CMapStringToPtr *mapNickToPtr;
	cui.m_pvChatDoc = this;
	cui.m_pvChatView = m_client;
	currentRoom = m_proto;
	puiSelf = (CUserInfo *) m_puiSelf;
	mapNickToPtr = &m_mapNickToPtr;
}



void CChatDoc::SaveConnectStatus(CString &strStatus)
{
	m_strStatus = strStatus;
}

void CChatDoc::ResetStatus(BOOL left, BOOL right)
{
	// First make sure that this is the Active Document
	if (GetFocusedDoc() != this) return;

	// It is, so update the status bar
	if (left) theApp.SetStatusPaneString(0,m_strStatus);
	if (right) {
		CString mesg, strCount;
		CMemberList *membList = (CMemberList *)m_memberList;
		CMemberListCtrl *membCtrl = &membList->m_MemberListBox;
		int count = membCtrl->GetItemCount();
		if (count == 1) mesg.LoadString(ID_USER_SINGULAR);
		else mesg.LoadString(ID_USER_PLURAL);
		strCount.Format("%d", count);
		VERIFY(ReplaceToken(mesg, CString("%1"), strCount));
		theApp.SetStatusPaneString(1, mesg);
	}
}

// Really should move virtual CDocObjectServerDoc::CreateInPlaceFrame(CWnd* pParentWnd)
// to this file.  This is a workaround.
CView *ExtractChatView(CDocObjectServerDoc *doc) {
	CChatDoc *d = (CChatDoc *) doc;
	return d->m_client;
}

CString QuoteAmpersands(const char *unquoted) {
	CString quoted = "";
	UCHAR *nextAmp;
	UCHAR *old = (UCHAR *)unquoted;

	while (nextAmp = _mbschr((UCHAR *)old, '&')) {
		CString inner((LPCTSTR) old, nextAmp - old);
		quoted += inner;
		quoted += "&&";
		old = nextAmp + 1;
	}
	quoted += old;
	return quoted;
}

void CChatDoc::SetTitle(LPCTSTR lpszTitle) {
	CDocObjectServerDoc::SetTitle(lpszTitle);

	extern BOOL bFreezeTabs;
	if (bFreezeTabs) return;
	CTabBar *tb = GetTabBar();

	if (!tb) return;

	CString strNewName = QuoteAmpersands(lpszTitle);
	int i = tb->FindTabNum(this);
	if (i >= 0) {
		tb->DelMDITab(i);
		tb->AddMDITab(strNewName, this);
	} else tb->AddMDITab(strNewName, this);

//	m_tabName = strNewName;
}


void CChatDoc::OnCloseDocument() 
{
	CTabBar *tb;
	if (tb = GetTabBar()) 
		tb->DelMDITab(tb->FindTabNum(this));
	

	if (theApp.m_bEmbedded)
		m_proto->ChatPartChannel(this, FALSE); // leave the channel, but leave doc open (EMBEDDED)

	// MUST close doc so that IOleClientSite, IOleDocumentSite and IOleInPlaceSite 
	// objects are released properly. If server shuts down then thats as it should be...
	CDocObjectServerDoc::OnCloseDocument();
}



void CChatDoc::ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault) {
	if (!(e->IsKindOf(RUNTIME_CLASS(CFileException)) && ((CFileException *)e)->m_cause == EX_DONTREPORT))
		CDocObjectServerDoc::ReportSaveLoadException(lpszPathName, e, bSaving, nIDPDefault);
}

void CChatDoc::RegisterNewContent() {
	if (!m_bNewContent && GetTabBar()) {
		CTabBar *tb = GetTabBar();
		if (m_bObscured) {
			int tabNum = tb->FindTabNum(this);
			tb->SetTabIcon(tabNum, 1);
			m_bNewContent = TRUE;
		}
	}
}

void CChatDoc::SetObscured(BOOL bObscured) {
	if (bObscured == m_bObscured) return;
	if (m_bNewContent && !bObscured) {
		CTabBar *tb = GetTabBar();
		if (tb) {
			int tabNum = tb->FindTabNum(this);
			tb->SetTabIcon(tabNum, 0);
		}
		m_bNewContent = FALSE;
	}
	m_bObscured = bObscured;
}

void CChatDoc::SetFocusToSayWnd() {
	if (m_sayWnd)
		((CSayWnd *)m_sayWnd)->SetFocusToSayWnd();
}

// want to make sure to reset the status info when the frame is active
// note that if the app is embedded, m_pActiveWnd may still be NULL in 
// CChatApp::SetStatusPaneString, even if the app is embedded!
//
void CChatDoc::OnFrameWindowActivate( BOOL bActivate ) {
	CDocObjectServerDoc::OnFrameWindowActivate(bActivate);
	if (bActivate) ResetStatus(TRUE, FALSE);
}



