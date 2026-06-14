// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// chatDoc.cpp : implementation of the CChatDoc class
//

#include "stdafx.h"
#include "chat.h"

#include "binddoc.h"
#include "chatDoc.h"
#include "binditem.h"
#include "ChatItem.h"

#include "setupdlg.h"
#include "spltchat.h"
#include "chatview.h"
#include <winreg.h>
#include "memblst.h"
#include "bbox.h"
#include "traj.h"
#include "spline.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "balloon.h"
#include "backdrop.h"
#include "panel.h"
#include "userinfo.h"
#include "script.h"
#include "pageview.h"
#include "chatprot.h"
#include "ui.h"
#include "bodycam.h"
#include "avatardl.h"
#include "saywnd.h"
#include "profdlg.h"
#include "histent.h"
#include "roomlist.h"
#include "proppage.h"
#include "textview.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

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
	ON_COMMAND(ID_OPTIONS_PANELS_3WIDE, OnPanels3wide)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PANELS_3WIDE, OnUpdatePanels3wide)
	ON_COMMAND(ID_OPTIONS_PANELS_1WIDE, OnPanels1wide)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PANELS_1WIDE, OnUpdatePanels1wide)
	ON_COMMAND(ID_OPTIONS_PANELS_2WIDE, OnPanels2wide)
	ON_COMMAND(ID_OPTIONS_PANELS_4WIDE, OnPanels4wide)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PANELS_4WIDE, OnUpdatePanels4wide)
	ON_COMMAND(ID_OPTIONS_PANELS_5WIDE, OnPanels5wide)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PANELS_5WIDE, OnUpdatePanels5wide)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_CUT, OnEditCut)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_DELETE, OnEditDelete)
	ON_COMMAND(ID_EDIT_SELECTALL, OnEditSelectAll)
	ON_COMMAND(ID_SEND_COMICS_DATA, OnSendComicsData)
	ON_UPDATE_COMMAND_UI(ID_SEND_COMICS_DATA, OnUpdateSendComicsData)
	ON_COMMAND(ID_APP_CHARACTER_SEL, OnAppCharacterSel)
	ON_COMMAND(ID_ACTIONS_SAY, OnActionsSay)
	ON_COMMAND(ID_ACTIONS_THINK, OnActionsThink)
	ON_COMMAND(ID_ACTIONS_WHISPER, OnActionsWhisper)
	ON_COMMAND(ID_OPTIONS_PROFILE, OnOptionsProfile)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PANELS_2WIDE, OnUpdatePanels2wide)
	ON_UPDATE_COMMAND_UI(ID_OPTIONS_PROMPTFORSAVE, OnUpdateOptionsPromptforsave)
	ON_COMMAND(ID_OPTIONS_PROMPTFORSAVE, OnOptionsPromptforsave)
	ON_COMMAND(ID_MEMBER_GETINFO, OnMemberGetinfo)
	ON_COMMAND(ID_MEMBER_IGNORE, OnMemberIgnore)
	ON_UPDATE_COMMAND_UI(ID_MEMBER_GETINFO, OnUpdateMemberGetinfo)
	ON_UPDATE_COMMAND_UI(ID_MEMBER_IGNORE, OnUpdateMemberIgnore)
	ON_COMMAND(ID_FILE_CREATESHORTCUT, OnFileCreateshortcut)
	ON_COMMAND(ID_FAVORITES_ADDTOFAVORITES, OnFavoritesAddtofavorites)
	ON_COMMAND(ID_FAVORITES_OPENFAVORITES, OnFavoritesOpenfavorites)
	ON_COMMAND(ID_SEND_ACTION, OnSendAction)
	ON_UPDATE_COMMAND_UI(ID_FILE_DISCONNECT, OnUpdateDisconnect)
	ON_UPDATE_COMMAND_UI(ID_APP_CHARACTER_SEL, OnUpdateAppCharacterSel)
	ON_COMMAND(ID_VIEW_COMICS, OnViewComics)
	ON_COMMAND(ID_VIEW_TEXT, OnViewText)
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMICS, OnUpdateViewComics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEXT, OnUpdateViewText)
	ON_COMMAND(ID_ADMINISTRATOR_KICK, OnAdministratorKick)
	ON_COMMAND(ID_ADMINISTRATOR_TOPIC, OnAdministratorTopic)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_CHATROOM_LIST, OnUpdateChatroomList)
	ON_UPDATE_COMMAND_UI(ID_ADMINISTRATOR_KICK, OnUpdateAdministratorKick)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINT, OnUpdateFilePrint)
	ON_UPDATE_COMMAND_UI(ID_FILE_CREATESHORTCUT, OnUpdateFileCreateshortcut)
	ON_UPDATE_COMMAND_UI(ID_FAVORITES_ADDTOFAVORITES, OnUpdateFavoritesAddtofavorites)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIONS, OnUpdateViewOptions)
	ON_COMMAND(ID_SESSION_NEWROOM, OnNewroom)
	ON_UPDATE_COMMAND_UI(ID_SESSION_NEWROOM, OnUpdateNewroom)
	ON_COMMAND(ID_SESSION_LEAVE, OnLeave)
	ON_UPDATE_COMMAND_UI(ID_SESSION_LEAVE, OnUpdateLeave)
	ON_COMMAND(ID_SESSION_CONNECT, OnConnect)
	ON_UPDATE_COMMAND_UI(ID_SESSION_CONNECT, OnUpdateConnect)
	ON_COMMAND(ID_SESSION_DISCONNECT, OnDisconnect)
	ON_COMMAND(ID_MAKE_ADMIN, OnMakeAdmin)
	ON_UPDATE_COMMAND_UI(ID_MAKE_ADMIN, OnUpdateMakeAdmin)
	ON_UPDATE_COMMAND_UI(ID_REMOVE_ADMIN, OnUpdateRemoveAdmin)
	ON_COMMAND(ID_CHATROOM_LIST, OnChatroomList)
	ON_COMMAND(ID_VIEW_ICON, OnViewIcon)
	ON_UPDATE_COMMAND_UI(ID_VIEW_ICON, OnUpdateViewIcon)
	ON_COMMAND(ID_VIEW_LIST, OnViewList)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIST, OnUpdateViewList)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_COMMAND(ID_ADMINISTRATOR_TOPIC, OnAdministratorTopic)
	ON_UPDATE_COMMAND_UI(ID_SESSION_DISCONNECT, OnUpdateDisconnect)
	ON_COMMAND(ID_REMOVE_ADMIN, OnMakeAdmin)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatDoc construction/destruction

CChatDoc::CChatDoc()
{
	// Use OLE compound files
//	EnableCompoundFile();
	m_comicsTitle = NULL;
	m_bEmbedded = FALSE;
	m_bPrompt = TRUE;
	theApp.m_strFavoritesDir = GetDesktopOrFavorites(FALSE);
	m_bRemember = TRUE;
	//changed this back to TRUE to be consistent with IE3.0
//  m_bRemember = FALSE;// mfc doesnt release its root storage so on exit it could try to close the 
	// file and if the disk isnt there anymore (A:), it will blue screen.  By turning off m_bRemember, 
	// it will release the storage each time.  A small tradeoff in performance, but works ok now.
	m_bIconMembers = m_bLastMemberView = TRUE;
	m_pmenuAdmin = NULL;
	m_bEnableFileMenu = FALSE;
	cui.m_pvChatDoc = this;

	LoadDocDataFromReg();
}

CChatDoc::~CChatDoc()
{
	DestroyPages();
	SaveDocDataToReg();

	// nuke admin menu if it exists
	if (m_pmenuAdmin) {
		m_pmenuAdmin->DestroyMenu();
		delete m_pmenuAdmin;
	}
}

void CChatDoc::InitMyDocument() {
	if (!theApp.m_bComicView) return;		// only has to do with Comics mode

	ASSERT(m_pages.IsEmpty());
	AddNewPage();

	if (m_comicsTitle) free(m_comicsTitle);
	char *GetRandomTitle();
	m_comicsTitle = GetRandomTitle();

	// Add title panel
	CPage *firstPage = (CPage *) (m_pages.GetHead());
	firstPage->AddTitle();		// give it a title
}

BOOL CChatDoc::OnNewDocument()
{
	if (!CDocObjectServerDoc::OnNewDocument())
		return FALSE;

	m_fileType = FT_CCR;		// so we trigger a connection dialogue

	InitMyDocument();

	return TRUE;
}

BOOL CChatDoc::SaveDocDataToReg() {

	// open the application's key
	HKEY	hKey = NULL;
	if (RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\Microsoft\\Microsoft Comic Chat", 
						0, "Application Global Data", REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,	NULL, &hKey, NULL) == ERROR_SUCCESS) {

		int i = 0;
		i =	m_bIconMembers;
		RegSetValueEx (hKey, "MemberListStyle", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		i = m_bPrompt;
		RegSetValueEx (hKey, "PromptForSave", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));
	}
	RegCloseKey (hKey);
	return TRUE;
}

BOOL CChatDoc::LoadDocDataFromReg() {

	DWORD cbData = 0;

	// open the key
	HKEY	hKey = NULL;
	if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\Microsoft\\Microsoft Comic Chat", 
						0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {

		cbData = sizeof(m_bIconMembers);
		RegQueryValueEx (hKey, "MemberListStyle", 0, NULL, (LPBYTE)&m_bIconMembers,
						&cbData);
		m_bLastMemberView = m_bIconMembers;

		cbData = sizeof(m_bPrompt);
		RegQueryValueEx (hKey, "PromptForSave", 0, NULL, (LPBYTE)&m_bPrompt,
						&cbData);
	}
	RegCloseKey (hKey);
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
	if (!ar.IsStoring()) InitMyDocument();

	if (m_fileType == FT_CCR) {
		if (ar.IsStoring())
		{
			extern BOOL ChatSaveLocator(CArchive &);
			ChatSaveLocator(ar);
		}
		else
		{
			extern BOOL ChatLoadLocator(CArchive &);
			ChatLoadLocator(ar);
		}
	} else {		// FT_CCC probably!
		if (ar.IsStoring())
		{
			extern void ChatSaveConversation(CArchive &);
			ChatSaveConversation(ar);
		}
		else
		{
			extern BOOL ChatLoadConversation(CArchive &);
			ChatLoadConversation(ar);
		}
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

void CChatDoc::AddLine(UINT id, const char *str, UCHAR mode, CPtrArray *links) {
//	AddToDialogHistory(id, str);					// first save in dialog history
	CPage *page = (CPage *) m_pages.GetTail();		// add a new panel to the current page
	if (!page->AddLine(id, str, mode, links)) {		//   ... first, adding a new page if needed
		AddNewPage();
		page = (CPage *) m_pages.GetHead();
		page->AddLine(id, str, mode, links);
	}
//	if(!m_bEmbedded && m_bPrompt)										// dont support prompt for save if embedded
//		SetModifiedFlag(TRUE);							// should set this when foreign line received too
}


void CChatDoc::AddNewPage() {
	CUnitPanelPage *firstPage = new CUnitPanelPage;		// create and add the initial page
	firstPage->m_topY = firstPage->m_leftX = 0;			// later calculate from existing pages
	m_pages.AddHead(firstPage);
}

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

void CChatDoc::ShowInfo(void *x_pui, const char *info) {   // simple proxy for current page.  really should check to see if fits on page
	CUserInfo *pui = (CUserInfo *) x_pui;
	if (theApp.m_bComicView) {
		CPage *page = (CPage *) m_pages.GetTail();
		page->ShowInfo(pui->GetAvatarID(), info);
	} else
		GetTextView()->ShowInfo(pui, info);
}

void FindAttribution(const char *words, void *userInfo, const char **mesgPtr, UINT *id) {
	USHORT ExtractAvatarID(void *);
	if (*id) {
		*mesgPtr = words;
		return;
	}

	const char *angle = strchr(words, '>');
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

void FindAddressees(UINT speakerID, const char *words, const char **mesgPtr) {
	UINT id, addressees[10], nAddressees = 0;
	const char *colon = strchr(words, ':');
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

void FindPose(UINT speakerID, const char *words, const char **mesgPtr) {
	UINT id, addressees[10], nAddressees = 0;
	const char *colon = strchr(words, '|');
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
	const char *colon = strchr(words, ';');
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


void CChatDoc::ProcessLine(UINT id, const char *line, UCHAR mode, BOOL cooked, CPtrArray *links) {
	const char *mesgPtr = line;

	if (!cooked) {
//		FindAttribution(line, NULL, &mesgPtr, &id);
//		FindAddressees(id, mesgPtr, &mesgPtr);        // updates the avatar structure as a side effect
		//		FindPose(id, mesgPtr, &mesgPtr);  // Note: ChatPreSendText will override this anyway
		CString str(mesgPtr);
		ChatPreSendText(str, id);
	}
	
	if (strcmp(mesgPtr, "<Brk>") != 0)
		TallySpeech(id);
	AddLine(id, mesgPtr, mode, links);
}

void CChatDoc::TallySpeech(UINT id) {
	CAvatarX *av = GetAvatar(id);
	av->m_nSends++;
	// Could update title, but don't bother for now
}

int CChatDoc::FindFileType(const char *pszPathName) {
	CString ext = CString(pszPathName).Right(4);
	if (lstrcmpi(ext, _T(".ccr"))==0) return FT_CCR;
	else return FT_CCC;
}

BOOL CChatDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
	m_fileType = FindFileType(lpszPathName);

	if (!COleServerDoc::OnOpenDocument(lpszPathName))
		return FALSE;
	
#if 0
	// Read the header type
	FILE *fp;
	char line[30], key[30];

	VERIFY(fp = fopen(lpszPathName, "r"));
	fgets(line, sizeof(line), fp);
	sscanf(line, "%s", key);
	if (strcmp(key, "#CHATLOCATOR") != 0) {
		AfxMessageBox(ID_ERR_NOT_CHATLOC);
		fclose(fp);
//		return FALSE;
	} else {
		ParseLocatorFile(fp);
	}
	fclose(fp);
#endif
	return TRUE;
}

BOOL CChatDoc::ParseLocatorFile(FILE *fp) {
	char buff[150], key[50], value[100];
	BOOL GetValue(const char *buff, char *value);
	extern void ChatSetServer(const char *);
	extern void ChatSetChannel(const char *);
	extern void ChatSetPort(UINT);

	while(fgets(buff, sizeof(buff), fp)) {
		sscanf(buff, "%s", key);
		if (!stricmp(key, "IRCSERVER:")) {
			if (GetValue(buff, value))
				ChatSetServer(value);
		} else if (!stricmp(key, "IRCCHANNEL:")) {
			if (GetValue(buff, value))
				ChatSetChannel(value);
		} else if (!stricmp(key, "IRCPORT:")) {
			if (GetValue(buff, value))
				ChatSetPort(atoi(value));
		} else {
			TRACE("Got an unknown key: %s.\n", key);
		}		
	}
	return TRUE;
}

BOOL CChatDoc::OnSaveDocument(LPCTSTR lpszPathName) 
{
	m_fileType = FindFileType(lpszPathName);

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

// warning: GetDoc() produces spurious results.  Use GetChatDoc().  -djk
CChatDoc *CChatDoc::GetDoc() {
	CFrameWnd * pFrame = (CFrameWnd *)(AfxGetApp()->m_pMainWnd);
	return pFrame ? (CChatDoc *) pFrame->GetActiveDocument() : NULL;
   }
 
void CChatDoc::DeleteContents() 
{
	DestroyPages();				// delete the pages

	if (!theApp.m_bComicView && GetTextView())
		GetTextView()->ClearTextView();

	DestroyHistory();		// delete last chat room's dialog history

	if (m_comicsTitle) {
		free(m_comicsTitle);
		m_comicsTitle = NULL;
	}

	extern BOOL bCXKeepServer;
	ChatPartChannel(!bCXKeepServer);

	void DestroyUserInfos();
	DestroyUserInfos();

	DestroyAvatars();		// clear the avatar and backdrop stores (housekeeping)

	void DestroyBackDropArt();
	DestroyBackDropArt();

	/*
	const char *GetMyCharacter();
	SetMyAvatar(GetMyCharacter());   // Always like to have a self-avatar set
*/
	extern UINT myAvatarID;
	myAvatarID = 0;
	extern void DetachBodyCamAvatar();
	DetachBodyCamAvatar();

	CDocObjectServerDoc::DeleteContents();
}

extern void SetPanelsWide(int);
// extern void SetPanelsHigh(int);
extern BOOL OKPanelWidth(int);
// extern BOOL OKPanelHeight(int);

void CChatDoc::OnPanels3wide() 
{
	SetPanelsWide(3);
}

void CChatDoc::OnUpdatePanels3wide(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(OKPanelWidth(3));
}

void CChatDoc::OnPanels1wide() 
{
	SetPanelsWide(1);	
}

void CChatDoc::OnUpdatePanels1wide(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(OKPanelWidth(1));	
}

void CChatDoc::OnPanels2wide() 
{
	SetPanelsWide(2);	
}

void CChatDoc::OnUpdatePanels2wide(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(OKPanelWidth(2));	
}

void CChatDoc::OnPanels4wide() 
{
	SetPanelsWide(4);
}

void CChatDoc::OnUpdatePanels4wide(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(OKPanelWidth(4));
}

void CChatDoc::OnPanels5wide() 
{
	SetPanelsWide(5);	
}

void CChatDoc::OnUpdatePanels5wide(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(OKPanelWidth(5));
}

/* Formerly also had functions like this...
void CChatDoc::OnPanels1high() 
{
	SetPanelsHigh(1);
}

void CChatDoc::OnUpdatePanels1high(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(OKPanelHeight(1));
	
}

*/


void CChatDoc::OnAppCharacterSel() 
{
    CAvatarDlg CharacterDlg;

	CharacterDlg.DoModal();
	GetView()->SetFocus();
}

void CChatDoc::OnEditUndo() 
{
	GetView()->GetFocus()->SendMessage(WM_UNDO, 0, 0);
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

	if 	((GetFocus() == hSayWnd  && !m_bEmbedded) || 
		(hgPrevFocus == hSayWnd && m_bEmbedded))
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
		(hgPrevFocus == GetSay()->GetSayEdit() && m_bEmbedded)))
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

	if (!theApp.m_bComicView)
		hTextWnd = GetTextView()->m_pRichEdit->m_hWnd;
	else
		hTextWnd = NULL;

	hSayWnd = GetSay()->GetSayEdit();

	VERIFY(hSayWnd);
	if 	((GetFocus() == hSayWnd  && !m_bEmbedded) || 
		(hgPrevFocus == hSayWnd && m_bEmbedded))
	{
		::SendMessage(hSayWnd, EM_GETSEL, (WPARAM)(LPDWORD)&lpdwStart, (LPARAM)(LPDWORD)&lpdwEnd);
	}
	else if ((GetFocus() == hTextWnd && !m_bEmbedded) || 
		(hgPrevFocus == hTextWnd && m_bEmbedded))
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
	pCmdUI->Enable((hFocus == GetSay()->GetSayEdit() && !m_bEmbedded)|| 
		(hgPrevFocus == GetSay()->GetSayEdit() && m_bEmbedded));
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
		(hgPrevFocus == GetSay()->GetSayEdit() && m_bEmbedded)))
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

	if (!theApp.m_bComicView)
		hTextWnd = GetTextView()->m_pRichEdit->m_hWnd;
	else
		hTextWnd = NULL;

	if((!GetSay()->IsEmpty() && (GetFocus() == GetSay()->GetSayEdit()) || 
					(hgPrevFocus == GetSay()->GetSayEdit() && m_bEmbedded)) ||
					(GetFocus() == hTextWnd && !m_bEmbedded) || 
						(hgPrevFocus == hTextWnd && m_bEmbedded))
	{
		pCmdUI->Enable(TRUE);
	}
	else
		pCmdUI->Enable(FALSE);

					
}

void CChatDoc::OnEditCut() 
{
	GetView()->GetFocus()->SendMessage(WM_CUT, 0, 0);
}

void CChatDoc::OnEditCopy() 
{
	GetView()->GetFocus()->SendMessage(WM_COPY, 0, 0);
}

void CChatDoc::OnEditPaste() 
{
	GetView()->GetFocus()->SendMessage(WM_PASTE, 0, 0);
}

void CChatDoc::OnEditDelete() 
{
	GetView()->GetFocus()->SendMessage(WM_CLEAR, 0, 0);
}

void CChatDoc::OnEditSelectAll() 
{
	GetView()->GetFocus()->SendMessage(EM_SETSEL, 0, (LPARAM)-1 );
}

void CChatDoc::OnSendComicsData() 
{
	BOOL ToggleSendComicsData();
	ToggleSendComicsData();
}

void CChatDoc::OnUpdateSendComicsData(CCmdUI* pCmdUI) 
{
	BOOL GetSendComicsData();
	pCmdUI->SetCheck(GetSendComicsData());
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
#endif

void CChatDoc::OnOptionsProfile() 
{
	CProfileDialog profileDlg;

	profileDlg.DoModal();
	GetView()->SetFocus(); 
}

void CChatDoc::ExecuteHistory(int mode) {
	BOOL oldRefresh = theApp.m_bNoRefresh;
	if (mode != HM_LIVE) theApp.m_bNoRefresh = TRUE;		// turn off refresh if not live

	POSITION pos = m_history.GetHeadPosition();
	while (pos) {
		HistoryEntry *entry = (HistoryEntry *) m_history.GetNext(pos);
		entry->Execute(mode);
	}

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

BOOL CChatDoc::DoSave(LPCTSTR lpszPathName, BOOL bReplace)
	// Save the document data to a file
	// lpszPathName = path name where to save document file
	// if lpszPathName is NULL then the user will be prompted (SaveAs)
	// note: lpszPathName can be different than 'm_strPathName'
	// if 'bReplace' is TRUE will change file name if successful (SaveAs)
	// if 'bReplace' is FALSE will not change path name (SaveCopyAs)
{
	CString newName = lpszPathName;
	if (newName.IsEmpty())
	{
		static char BASED_CODE szFilter[] = "Comic Chat Conversation (*.ccc)|*.ccc||";

		LONG lResult;
		CString strSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\CChat.exe";
		HKEY hExe;
		BYTE Data[256];
		DWORD cbData = 256;
		DWORD type;
	    char drive[_MAX_DRIVE];
	    char dir[_MAX_DIR];
	    char fname[_MAX_FNAME];
	    char ext[_MAX_EXT];

 
		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,LPCTSTR(strSubKey),0,KEY_QUERY_VALUE,&hExe);

		lResult = RegQueryValueEx(hExe,"",0,&type,Data,&cbData);

		_splitpath((char *)Data , drive, dir, fname, ext );

//		int index = strDefault.Find("Chat.EXE");
//		CString strFinal = strDefault.Left(index);

		CString strDir(dir);
		CString strFinal(drive);
		strFinal+= strDir + GetTitle();

		CFileDialog dlgFile( FALSE,"ccr",LPCTSTR(strFinal), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
			LPCTSTR(&szFilter));
	
		if(dlgFile.DoModal() == IDOK)
			newName = dlgFile.GetPathName();
		else
			return FALSE;
	}

	CWaitCursor wait;

	if (!OnSaveDocument(newName))
	{
		if (lpszPathName == NULL)
		{
			// be sure to delete the file
			TRY
			{
				CFile::Remove(newName);
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
	if (bReplace)
		SetPathName(newName);
	SetModifiedFlag(FALSE);
	return TRUE;        // success
}

COleIPFrameWnd* CChatDoc::CreateInPlaceFrame(CWnd* pParentWnd)
{
	m_bEmbedded = TRUE;
	return CDocObjectServerDoc::CreateInPlaceFrame(pParentWnd);
}

void CChatDoc::OnUpdateOptionsPromptforsave(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bPrompt);
	
}

void CChatDoc::OnOptionsPromptforsave() 
{
	if(m_bPrompt)
	{
		m_bPrompt = FALSE;
		SetModifiedFlag(FALSE);
	}
	else
		m_bPrompt = TRUE;
	
}


CUserInfo *GetSingleSelectedMember() {
	CUserInfo *LookupPui(const char *);
	if (GetMembers()->m_MemberListBox.GetSelectedCount() != 1) return NULL;
	int index = GetMembers()->m_MemberListBox.GetNextItem(-1,LVNI_SELECTED);
	CString nickname = GetMembers()->m_MemberListBox.GetItemText(index,0);
	return LookupPui(nickname);
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
		if (pui->Ignored()) pCmdUI->SetCheck(1);
		else pCmdUI->SetCheck(0);
	} else pCmdUI->Enable(FALSE);
}

void CChatDoc::OnUpdateMemberGetinfo(CCmdUI* pCmdUI) 
{
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui && pui->IsComicUser() && ChatGetConnectionStatus() == CX_INCHANNEL)
		pCmdUI->Enable(TRUE);
	else pCmdUI->Enable(FALSE);
}


void CChatDoc::OnFileCreateshortcut() 
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	// Get the document title to use as a link name
	CString strDocTitle = GetTitle();

	_splitpath(LPCTSTR(strDocTitle) , drive, dir, fname, ext );
	
	CString strfname = CString(fname);
	CString strTotalName = strfname + ".ccr";

	CString strDesktop = GetDesktopOrFavorites(TRUE);

//	if(strDocTitle.Right(4) != ".ccr")  //different versions of commdlg.dll will return a title
//		strDocTitle+=".ccr";			//with a extension.  have to check for that.

	CString strLink = strDesktop + "\\" + strTotalName;
	AfxMessageBox(IDS_SHORTCUT);
	DoSave(strLink,TRUE);
	if(!m_bEmbedded)// reset the modified flag to true again.
		SetModifiedFlag(TRUE);
}


void CChatDoc::OnFavoritesAddtofavorites() 
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];

	// Get the document title to use as a link name
	CString strDocTitle = GetTitle();

	_splitpath(LPCTSTR(strDocTitle) , drive, dir, fname, ext );
	
	CString strfname = CString(fname);
	CString strTotalName = strfname + ".ccr";

	CString strFavorites = GetDesktopOrFavorites(FALSE);
//	if(strDocTitle.Right(4) != ".ccr")  //different versions of commdlg.dll will return a title
//		strDocTitle+=".ccr";			//with a extension.  have to check for that.
	CString strLink = strFavorites + "\\" + strTotalName;
	AfxMessageBox(IDS_FAVORITE);
	DoSave(strLink,TRUE);
	if(!m_bEmbedded)// reset the modified flag to true again.
		SetModifiedFlag(TRUE);
}


// returns string containing path to desktop (TRUE) or favorites (FALSE)
CString CChatDoc::GetDesktopOrFavorites(BOOL bDesktop)
{
	LONG lResult;
	CString strSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	HKEY hShellFolders;
	BYTE Data[256];
	DWORD cbData = 256;
	DWORD type;

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER,LPCTSTR(strSubKey),0,KEY_QUERY_VALUE,&hShellFolders);
	if(bDesktop)
		lResult = RegQueryValueEx(hShellFolders,"Desktop",0,&type,Data,&cbData);
	else
		lResult = RegQueryValueEx(hShellFolders,"Favorites",0,&type,Data,&cbData);

	return CString(Data);
}


void CChatDoc::OnFavoritesOpenfavorites() 
{
//	CString strCommandLine;
	CString strFavorites = GetDesktopOrFavorites(FALSE) +"\\*.ccr";
//	strCommandLine = "explorer " + strFavorites;
//	WinExec(strCommandLine,SW_SHOWNORMAL);*/
	static char BASED_CODE szFilter[] = "Comic Chat Room (*.ccr)|*.ccr||";

	CFileDialog dlgFile( TRUE,"ccr",strFavorites, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR(&szFilter));
	
	if(dlgFile.DoModal() == IDOK)
	{
		CString newName = dlgFile.GetFileName();
		AfxGetApp()->OpenDocumentFile(newName);
	}
}

// currently not enabled -- won't work, since disconnection tries to clear userinfos
#if 0
void CChatDoc::OnFileDisconnect() 
{
	void ChatPartChannel(BOOL);
	ChatPartChannel(TRUE);
}
#endif


void CChatDoc::OnUpdateAppCharacterSel(CCmdUI* pCmdUI) 
{
	if(MyAvatar())
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
	
}

void CChatDoc::OnViewComics() 
{
	if(GetChatApp()->m_bComicView)
		return;
	void MapNullAvatars();

	GetChatApp()->m_bComicView = TRUE;
	GetChatView()->CleanUpBeforeChangeView();
	m_fileType = FT_CCC;  // don't trigger connect dialogue
	GetChatView()->CreateComicView();
	MapNullAvatars();    // must be done before title panel created in ResetExistingPanels
	GetView()->ResetExistingPanels();
	RegenerateView();
	GetView()->PostMessage(WM_KEYDOWN,VK_END);
	GetView()->UpdateScroll();
}

void CChatDoc::OnViewText() 
{
	if(!GetChatApp()->m_bComicView)
		return;
	GetChatApp()->m_bComicView = FALSE;
	CChatView *chatView = GetChatView();
	chatView->CleanUpBeforeChangeView();
	m_fileType = FT_CCC;  // don't trigger connect dialogue
	chatView->CreateTextView();
	DestroyPages();			// delete the comics already created
	RegenerateView();
}

static CRoomListPersist roomPersist;

void CChatDoc::OnChatroomList() 
{
	CRoomList roomDlg(&roomPersist);
	cui.m_pvRoomList = &roomDlg;		// cache this info for irc.cpp
	roomDlg.DoModal();
	cui.m_pvRoomList = NULL;
}

void CChatDoc::OnUpdateViewComics(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(GetChatApp()->m_bComicView);
}

void CChatDoc::OnUpdateViewText(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(!(GetChatApp()->m_bComicView));
}


void CChatDoc::OnViewList() 
{
	HWND hWnd = GetMembers()->m_MemberListBox.m_hWnd;
	LONG lStyle = GetWindowLong(hWnd,GWL_STYLE);
	lStyle &= ~LVS_ICON;
	lStyle |= LVS_LIST;
	SetWindowLong(hWnd,GWL_STYLE,lStyle);
	m_bIconMembers = FALSE;
}

void CChatDoc::OnViewIcon() 
{
	HWND hWnd = GetMembers()->m_MemberListBox.m_hWnd;
	LONG lStyle = GetWindowLong(hWnd,GWL_STYLE);
	lStyle &= ~LVS_LIST;
	lStyle |= LVS_ICON;
	SetWindowLong(hWnd,GWL_STYLE,lStyle);
	m_bIconMembers = TRUE;
}

void CChatDoc::OnUpdateViewIcon(CCmdUI* pCmdUI) 
{
	if(GetChatApp()->m_bComicView)
	{
		pCmdUI->Enable(TRUE);
		pCmdUI->SetRadio(m_bIconMembers);
	}
	else
		pCmdUI->Enable(FALSE);

}

void CChatDoc::OnUpdateViewList(CCmdUI* pCmdUI) 
{
	pCmdUI->SetRadio(!m_bIconMembers);	
}

void CChatDoc::OnViewOptions() 
{
	CPropertySheet		psOptions(IDS_OPTIONS);
	CSettingsPage		ppSettings;
	CPersonalPage		ppPersonal;
	CCharacterPage		ppCharacter;
	CBackgroundPage		ppBackground;

	psOptions.AddPage(&ppPersonal);
	psOptions.AddPage(&ppSettings);

	if(GetChatApp()->m_bComicView)
	{
		psOptions.AddPage(&ppCharacter);
		psOptions.AddPage(&ppBackground);
	}

	psOptions.DoModal();

}

void CChatDoc::InsertAdminMenu() 
{
	
	// first get the main menu
	CWnd* pWnd = AfxGetMainWnd();
	CMenu* pMenu;
	CMenu* pSub;
	HMENU hMenu;
	if(m_bEmbedded)
	{
		CDocTemplate* pTemplate = GetDocTemplate();
		ASSERT_VALID(pTemplate);
		hMenu =  pTemplate->m_hMenuInPlaceServer;
		pMenu = new CMenu;
		pMenu->Attach(hMenu);
		pSub = pMenu->GetSubMenu(3);
	}
	else
	{
			pMenu = pWnd->GetMenu();
			pSub = pMenu->GetSubMenu(4);
	}

		//get a menu object representing the menu to insert
	m_pmenuAdmin = new CMenu;
	m_pmenuAdmin->LoadMenu(IDR_ADMIN);
	//make sure you are only looking at its sub menu
	//CMenu* pSub = m_pmenuAdmin->GetSubMenu(3);

	CMenu* pAdminSub = m_pmenuAdmin->GetSubMenu(0);
	//and insert
	//BOOL bResult = pMenu->InsertMenu(3,MF_BYPOSITION | MF_POPUP,(UINT)pSub->m_hMenu,"A&dministrator");
	pSub->InsertMenu(2,MF_BYPOSITION | MF_SEPARATOR);	
	BOOL bResult = pSub->InsertMenu(3,MF_BYPOSITION | MF_POPUP,(UINT)pAdminSub->m_hMenu,"A&dministrator");
	// make sure to redraw the menubar
	pWnd->DrawMenuBar();
	if(m_bEmbedded)
	{
		pMenu->Detach();
		delete pMenu;
	}
}

void CChatDoc::RemoveAdminMenu()
{
	// first get the main menu
	if(m_pmenuAdmin)
	{
		CWnd* pWnd = AfxGetMainWnd();
		if (!pWnd) return;				// on exit, this may be true
		CMenu* pMenu;
		CMenu* pSub;
		HMENU hMenu;
		if(m_bEmbedded)
		{
			CDocTemplate* pTemplate = GetDocTemplate();
			ASSERT_VALID(pTemplate);
			hMenu =  pTemplate->m_hMenuInPlaceServer;
			pMenu = new CMenu;
			pMenu->Attach(hMenu);
			pSub = pMenu->GetSubMenu(3);
		}
		else
		{
				pMenu = pWnd->GetMenu();
				pSub = pMenu->GetSubMenu(4);
		}
		//remove the administrator menu
		pSub->RemoveMenu(3,MF_BYPOSITION);
		pSub->RemoveMenu(2,MF_BYPOSITION);
		pWnd->DrawMenuBar();
		//clean up
		m_pmenuAdmin->DestroyMenu();
		delete m_pmenuAdmin;
		m_pmenuAdmin = NULL;
		if(m_bEmbedded)
		{
			pMenu->Detach();
			delete pMenu;
		}
	}
}


void CChatDoc::OnAdministratorKick() 
{
	void ChatKickUser(CUserInfo *);
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui) ChatKickUser(pui);
}

void CChatDoc::OnAdministratorTopic() 
{
	void ChatSetTopic();
	ChatSetTopic();
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
	pCmdUI->Enable(m_bEnableFileMenu);
	
}

void CChatDoc::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bEnableFileMenu);
	
}

void CChatDoc::OnUpdateChatroomList(CCmdUI* pCmdUI) 
{
	int iConnect = ChatGetConnectionStatus();
	pCmdUI->Enable(iConnect == CX_INCHANNEL || iConnect == CX_NOCHANNEL);
}



void CChatDoc::OnUpdateAdministratorKick(CCmdUI* pCmdUI) 
{
	extern CUserInfo *puiSelf;
	CUserInfo *pui = GetSingleSelectedMember();

	if (pui && pui != puiSelf) 
		pCmdUI->Enable(TRUE);
	else
		pCmdUI->Enable(FALSE);
}

void CChatDoc::OnUpdateFilePrint(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bEnableFileMenu);
}

void CChatDoc::OnUpdateFileCreateshortcut(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ChatGetConnectionStatus() == CX_INCHANNEL);
}

void CChatDoc::OnUpdateFavoritesAddtofavorites(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ChatGetConnectionStatus() == CX_INCHANNEL);
}

void CChatDoc::OnUpdateViewOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ChatGetConnectionStatus() != CX_CONNECTING);
	
}

void CChatDoc::OnNewroom() 
{
	void ChatSwitchChannel();
	ChatSwitchChannel();	
}

void CChatDoc::OnUpdateNewroom(CCmdUI* pCmdUI) 
{
	int status = ChatGetConnectionStatus();
	pCmdUI->Enable(status == CX_INCHANNEL || status == CX_NOCHANNEL);
}

void CChatDoc::OnLeave() 
{
	ChatPartChannel(FALSE);	
}

void CChatDoc::OnUpdateLeave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(ChatGetConnectionStatus() == CX_INCHANNEL);
}

void CChatDoc::OnConnect() 
{
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
}

void CChatDoc::OnUpdateConnect(CCmdUI* pCmdUI) 
{
	int status = ChatGetConnectionStatus();
	pCmdUI->Enable(status == CX_DISCONNECTED);
	
}

void CChatDoc::OnDisconnect() 
{
	ChatPartChannel(TRUE, FALSE);
	AfxMessageBox(IDS_DISCONNECTMESSAGE2);
}

void CChatDoc::OnUpdateDisconnect(CCmdUI* pCmdUI) 
{
	int status = ChatGetConnectionStatus();
	pCmdUI->Enable(status != CX_DISCONNECTED);
}

void CChatDoc::OnMakeAdmin() // toggles admin status
{
	void ChatSetOperator(CUserInfo *pui, BOOL);
	CUserInfo *pui = GetSingleSelectedMember();
	if (pui && (ChatGetConnectionStatus() == CX_INCHANNEL))
		ChatSetOperator(pui, !pui->IsOperator());
}

void CChatDoc::OnUpdateMakeAdmin(CCmdUI* pCmdUI) 
{
	CUserInfo *pui = GetSingleSelectedMember();
	BOOL enabled = pui && !pui->IsOperator() && (ChatGetConnectionStatus() == CX_INCHANNEL);
	pCmdUI->Enable(enabled);
}

void CChatDoc::OnUpdateRemoveAdmin(CCmdUI* pCmdUI) 
{
	extern CUserInfo *puiSelf;
	CUserInfo *pui = GetSingleSelectedMember();
	BOOL enabled = pui && pui->IsOperator() && pui != puiSelf &&
		(ChatGetConnectionStatus() == CX_INCHANNEL);
	pCmdUI->Enable(enabled);

}
