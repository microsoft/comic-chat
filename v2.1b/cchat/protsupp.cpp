#include "stdafx.h"
#include "chat.h"
#include "binddoc.h"
#include "chatDoc.h"
#include "IrcProto.H"

#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "setupdlg.h"
#include "ui.h"
#include "memblst.h"
#include "histent.h"
#include <mmsystem.h>
#include "roomlist.h"
#include "userlist.h"
#include "admindlg.h"
#include "chanprop.h"
#include "bodycam.h"
#include "proppage.h"
#include "saywnd.h"
#include "format.h"
#include "tabbar.h"	// for tabbar
#include "ccommon.h"
#include "textcore.h"
#include "whisprbx.h"
#include <mbstring.h>

// To see if people are allowed a room list
#include "DlyLdDll.h"
#include "ratings.h"
#include <time.h>
#include <winnls.h>
#include <winsock.h>	// for inet_addr, etc... (NetMeeting reply)
#include <msconf.h>		// for NetMeeting calls
#include <sys/stat.h>	// for _stat
#include <process.h>	// for _beginthread


extern CChatApp theApp;
extern CUserInfo *puiSelf;
extern UINT MyAvatarID();
//extern CSetupPage setupDlg;
extern const char *GetMyName();
extern const char *GetMyServer();
extern void SetMyName(const char *charName);
extern void SetMyNameNick(const char *nick);
extern const char *GetMyRealName();
extern const char *GetMyCharacter();
extern const char *GetMyChannel();
extern const char *GetMyEmail();
extern const char *GetMyHomePage();
extern void ChatSetServer(const char *);
extern void ChatSetChannel(const char *);
extern void SetMyCharacter(char *);
BOOL RequestedChannelList(BOOL);
void ShowSay(CUserInfo *pui, const char *szText, CDWordArray *prgdwFormatting, BYTE bbCooked, BYTE byteMode = SM_SAY);
extern BOOL AddToWhisperBox(CUserInfo *pui, BYTE byteMode, const char *szMesg);
//extern DWORD WINAPI ConvertINetString(DWORD dwSrcEncoding, DWORD dwDstEncoding, LPCSTR lpSrcStr, LPINT lpnSrcSize, LPSTR lpDstStr, LPINT lpnDstSize);
BOOL bInitEnterInfo(const char *roomName, const char *password = NULL, BOOL bEncoode = FALSE);
BOOL bSwitchToRoom(const char *room, const char *password = NULL, BOOL bEncode = FALSE);
BOOL bCanViewUnrated(BOOL = FALSE), bNMInstalled();
const char *DecodeNick(const char *), *EncodeNick(const char *, BOOL = FALSE);
const char *DecodeChan(const char *, BOOL = FALSE), *EncodeChan(const char *);
const char *DecodeString(const char *szString, int iEncoding);
void ConfirmAway();
extern CChatDoc *LookupDoc(const char *);
extern CPtrList g_docs;

CMapStringToPtr *mapNickToPtr;
CPtrArray whisperees;
static CPtrList externalPuis;

static BOOL bSendComicsData = TRUE;
BOOL bCXPrompt = TRUE;
int iViewMode = VM_UNSPECIFIED;

static CString strRequestedNick;
static BOOL bInEnumeration = FALSE;

CRoomInfo enterInfo;
CRoomInfo *currentRoom;


BOOL ToggleSendComicsData() {
	bSendComicsData = ! bSendComicsData;
	// note: following line should really alert world, not just current room!
	if (bSendComicsData)
		GetChatDoc()->m_proto->ChatAnnounceNewAvatar(GetMyCharacter(), NULL);  // announce new av to world
	return bSendComicsData;
}

void SetSendComicsData(BOOL val) {
	bSendComicsData = val;
}

BOOL GetSendComicsData() {     // does not rewrite ini
	return (bSendComicsData);
}

void ChatSetCXPrompt(BOOL prompt) {
	bCXPrompt = prompt;
}


BOOL ChatInitialize()
{
#ifdef IRCLOG
	void StartFileInLoop();
	if (theApp.m_fileIn) {
		StartFileInLoop();
		return TRUE;
	}
#endif

	extern bCXKeepServer, bCXPrompt;

	if (!bCXKeepServer)
		InitializeServerConnection();

	bCXKeepServer = FALSE;
	bCXPrompt = TRUE;

	return TRUE;
}

void AdjustViewMode() {
	CChatDoc *doc = GetChatDoc();
	if (!doc) return;
	if (iViewMode == VM_TEXT || !theApp.m_bFoundArt)
		doc->OnViewText();
	else if (iViewMode == VM_COMICS)
		doc->OnViewComics();

	iViewMode = VM_UNSPECIFIED;
}


void SetArtDir(const char *artDir) {
	void ResetAvatarNames();
	BOOL ArtDirsOK();

	CString strNewBackDropDir = theApp.m_strBaseDir + "\\" + artDir;
	if (strNewBackDropDir == theApp.m_strBackDropDir) return; // both dirs are unchanged
	theApp.m_strBackDropDir = strNewBackDropDir;
	theApp.m_strAvatarDir = strNewBackDropDir;
	theApp.m_bFoundArt = ArtDirsOK();
	ResetAvatarNames();					// clear the cache of avatar names
}

#if 0

void ChatSetMemberCount(int count) {
//	memberCount = count;
	// Post string to StatusBar Pane 2
	CString mesg, strCount;
	if (count == 1) mesg.LoadString(ID_USER_SINGULAR);
	else mesg.LoadString(ID_USER_PLURAL);
	strCount.Format("%d", count);
	VERIFY(ReplaceToken(mesg, CString("%1"), strCount));
	ASSERT(AfxGetApp());
	((CChatApp*)AfxGetApp())->SetStatusPaneString(1, mesg);

	if (count == 0 && GetMembers())
		GetMembers()->CleanupImageList();  // might as well purge it.
}

#endif


void CIrcProto::TryNewNick(int msg_id, const char *showNick, BOOL registerNick, CString *newNick) {
	CNicknameDlg nickDlg;
	nickDlg.m_label.LoadString(msg_id);
	nickDlg.m_strNickname = showNick ? showNick : GetMyName();
	if (nickDlg.DoModal() == IDCANCEL) {
		if (GetConnectionStatus() == CX_CONNECTING) {
			theApp.OnDisconnect(); // force a disconnect
			void OfflineEditInits();
			OfflineEditInits();		// so the user can type text!
		}
		return;
	}

	if (nickDlg.m_strNickname.IsEmpty())
		nickDlg.m_strNickname.LoadString(IDS_DEFAULT_NICK);

	// If Personal page is open, we need to update it's nickname field too.
	CPersonalPage *page = GetPersonalPage();
	if (page && IsWindow(page->m_hWnd)) {
		page->m_strNickname = nickDlg.m_strNickname;
		page->UpdateData(FALSE);
	}

	if (newNick) (*newNick) = nickDlg.m_strNickname;

	if (registerNick) {
		int cxStatus = GetConnectionStatus();
		if (cxStatus != CX_DISCONNECTED)
			ChatSetNick(nickDlg.m_strNickname);
		else
			SetMyName(nickDlg.m_strNickname);  // since won't get a nick message back
	}
}

void CIrcProto::SetConnectionStatus(int iStat) {
	CString leftMesg;
	switch(iStat) {
	case CX_DISCONNECTED:
		m_sock->m_iConnected = iStat;
		break;
	case CX_INCHANNEL:
		m_sock->m_iConnected = CX_CONNECTED;
		m_bInRoom = TRUE;
		const char *GetMyServer();
		leftMesg.LoadString(ID_CONNECTED);
		VERIFY(ReplaceToken(leftMesg, CString("%1"), m_strPrettyChannel));
		VERIFY(ReplaceToken(leftMesg, CString("%2"), GetMyServer()));
		((CChatDoc *)m_doc)->SaveConnectStatus(leftMesg);
		break;
	case CX_CONNECTING:
		m_sock->m_iConnected = iStat;
		break;
	case CX_NOCHANNEL:
		m_sock->m_iConnected = CX_CONNECTED;
		m_bInRoom = FALSE;
		break;
	}
	UpdateStatus();
}

int CIrcProto::GetConnectionStatus() {
	if (m_sock->m_iConnected == CX_DISCONNECTED || m_sock->m_iConnected == CX_CONNECTING)
		return m_sock->m_iConnected;
	else if (m_bInRoom) return CX_INCHANNEL;
	else return CX_NOCHANNEL;
}

#define MAXTOKEN 256

char *GetToken(char *start, char **nextStart, const char *seps/* = ",.)"*/) {
	static char buff[MAXTOKEN];
	while (*start && (my_isspace(*start) || strchr(seps, *start))) start++;
	if (!*start) return NULL;
	char *endPtr = start;
	while (*endPtr && !my_isspace(*endPtr) && !strchr(seps, *endPtr)) endPtr++;
	int nchars = endPtr - start;
	nchars = min(nchars, sizeof(buff)-1); // don't overrun buff!
	ASSERT(nchars);
	strncpy(buff, start, nchars);
	buff[nchars] = '\0';
	*nextStart = endPtr;
	return buff;
}


void ChatEmptyMemberList(CChatDoc *doc = NULL) {
	if (!doc) doc = GetChatDoc();
	if (doc->m_memberList) { // can be called after window destroyed!
		((CMemberList *)doc->m_memberList)->m_MemberListBox.DeleteAllItems();		// empty out member pane
		doc->ResetStatus(FALSE, TRUE);
	}
}

void DestroyUserInfos(CChatDoc *doc) {
	CString nick;
	int avID, GetAvatarUpperBound();

	ChatEmptyMemberList(doc);			// required since this has refs to puis

	POSITION pos = doc->m_allChannelPuis.GetHeadPosition();
	int avUpper = GetAvatarUpperBound();
	while (pos) {
		CUserInfo *pui = (CUserInfo *) doc->m_allChannelPuis.GetNext(pos);
		if ((avID = pui->GetAvatarID()) && avID <= avUpper) { // if avID not 0
			CAvatarX *av = GetAvatar(avID);
			av->m_userInfo = NULL;   // erase avatar pointers to puis
			av->m_nSends = 0;		 // so can be reused
		}
		delete pui;
	}

	doc->m_allChannelPuis.RemoveAll();	// reclaim list storage
	doc->m_mapNickToPtr.RemoveAll();	// reclaim map storage

	if (doc->m_puiSelf == puiSelf) puiSelf = NULL;	// set global to be null if necessary (window current)
	doc->m_puiSelf = NULL;	
}

void DestroyExternalUserInfos() {
	POSITION pos = externalPuis.GetHeadPosition();	// now externals...
	while (pos) {
		CUserInfo *pui = (CUserInfo *) externalPuis.GetNext(pos);
		delete pui;
	}
}

void ForEachWord(char *line, void (*func)(char *, void *), void *clientData, char *sep) {
	char *word;
	while (word = GetToken(line, &line, sep)) {
		(*func)(word, clientData);
	}
}

void AddToMembersList(CUserInfo *pui, CChatDoc *doc = NULL) {
	int AddToImageList(CUserInfo* pui);
	void UpdateTitle(CChatDoc *);

	if (!doc)
		doc = GetChatDoc();
	ASSERT(doc);
	if (doc->GetConnectionStatus() != CX_INCHANNEL)
		return;  // don't add if not connected to room!

	ASSERT(pui);
	TRACE("Adding %s to memberlist\n", pui->GetScreenName());

	CMemberList*	membList = (CMemberList *)(doc->m_memberList);
	LV_ITEM			lv;
	int				pos = membList->GetSortPosition(pui);

	if (doc->m_bComicView)
		AddToImageList(pui);

	// Fix 4481
	lv.stateMask = LVIS_STATEIMAGEMASK;
	lv.state = INDEXTOSTATEIMAGEMASK(1);

	lv.iItem = pos;
	lv.iSubItem = 0;
	lv.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lv.pszText = LPSTR_TEXTCALLBACK; // should work, but doesn't allow sort!!!
	lv.iImage = I_IMAGECALLBACK;
	lv.lParam = (long) pui;
	membList->m_MemberListBox.InsertItem(&lv);

	if (!bInEnumeration)
	{
		if (doc->m_bComicView)
		{
			UpdateTitle(doc);
			if (doc->m_bIconMembers)
				membList->Sort();
		}
	}
	doc->ResetStatus(FALSE, TRUE);
}


void AssignArbitraryAvatar(CUserInfo *pui) {
 	char avatarstr[_MAX_FNAME];
	GetNextAvatarName(avatarstr);
	CAvatarX *av = GetAvatar3(avatarstr);  // pui just created, so no need to specify as 2nd arg
	pui->SetAvatarID(av->m_avatarID);
	av->m_userInfo = pui;
	TRACE("Mapping %s to %s.\n", pui->GetScreenName(), av->m_name);
}


CUserInfo *LookupPui(const char *nickname, CChatDoc *doc = NULL) inline {
	void *pui;
	if (STATUS_CHAR(*nickname)) nickname++;	// strip away status sign, if given
	if (!doc) {
		doc = GetChatDoc();
		if (!doc) return NULL;
	}
//	CMapStringToPtr *map = doc ? &doc->m_mapNickToPtr : mapNickToPtr;
	if (doc->m_mapNickToPtr.Lookup(nickname, pui) == 0) return NULL;
	else return (CUserInfo *)pui;
}

CUserInfo *ExternalPui(const char *nick, const char *szFullName, BOOL addIfNotThere = FALSE) {
	POSITION pos = externalPuis.GetHeadPosition();
	while (pos) {
		CUserInfo *pui = (CUserInfo *) externalPuis.GetNext(pos);
		if (stricmp(nick, pui->GetName()) == 0 && stricmp(szFullName, pui->GetFullName()) == 0)
			return pui;
	}
	if (addIfNotThere) {
		CUserInfo *pui = new CUserInfo(nick, szFullName);
		pui->SetExternal(TRUE);
		externalPuis.AddHead(pui);
		return pui;
	} else return NULL;
}



USHORT GetAvatarIDFromNickname(LPCTSTR nickname) {
	return LookupPui(nickname)->GetAvatarID();
}


CUserInfo *CIUserJoin(CUserInfo *pui) {	// assume for now that all joins represent totally new people
	CChatDoc *doc = GetChatDoc();
	BOOL comicMode = doc->m_bComicView;
	const char *nickname = pui->GetName();

	// see if it's us
	if (pui->IsSelf()) {
		ASSERT( puiSelf == NULL );
		if (comicMode) {
			pui->SetAvatarID(MyAvatarID());
			CAvatarX *av = MyAvatar();
			av->m_userInfo = pui;
//			doc->m_proto->ChatAnnounceNewAvatar(GetMyCharacter());
		}
		puiSelf = pui;
		doc->m_puiSelf = /*(void *)*/ pui;
		pui->ComicUser(TRUE);		// we know this to be true, since it's us.
		doc->UpdateAdminMenu();
	} else {
		if (comicMode) 
			AssignArbitraryAvatar(pui);
	}
	if ((doc->m_proto->m_dwModes & CM_MODERATED) && !pui->CheckFlag(UF_HASVOICE) && !pui->IsOperator())
		pui->SetFlag(UF_SPECTATOR, TRUE);  // really should be in pui creation but don't have doc ref there
	CUserInfo *pui3;
	if (pui3 = LookupPui(nickname)) TRACE("####### GOT ANOTHER INSTANCE OF %s (%x)\n", nickname, pui3);
	mapNickToPtr->SetAt(nickname, pui);
	doc->m_allChannelPuis.AddHead(pui);
	AddToMembersList(pui, doc);
	return pui;
}

int AddToImageList(CUserInfo* pui)
{
	// First get the CDib
	CAvatarX* pAv = GetAvatar(pui->GetAvatarID());
	CAvatarX *origAv = pAv->m_origID ? GetAvatar(pAv->m_origID) : pAv;

	if (origAv->m_iconIndex < 0) {
		CPose* pPose = GetPoseFromID(pAv->m_icon);
		CDIB* pDIB = pPose->m_drawing;

		//Turn DIB into HBITMAP
		HBITMAP hBmp = CreateDIBitmap(GetClientDC()->GetSafeHdc(),
										&(pDIB->GetBitmapInfoAddress()->bmiHeader),
										CBM_INIT,
										pDIB->GetBitsAddress(),
										pDIB->GetBitmapInfoAddress(),
										DIB_RGB_COLORS);
		CBitmap temp;
		CBitmap* pImageBmp = temp.FromHandle(hBmp);
									
		// Replace the next two lines with code to get correct avatar head
		origAv->m_iconIndex = theApp.m_ImageList.Add(pImageBmp,pImageBmp);
		TRACE("Allocating an icon (%d) for %s.\n", origAv->m_iconIndex, origAv->m_name);
		TRACE("Icon %d is handle %x\n", origAv->m_iconIndex, hBmp);
		VERIFY(DeleteObject(hBmp));
	} else TRACE("Icon already in stock (%d) for %s.\n", origAv->m_iconIndex, origAv->m_name);

	return origAv->m_iconIndex;
}

int FindMemberListIndex(CUserInfo *pui, CChatDoc *doc = NULL) {
	LV_FINDINFO lvFind;
	lvFind.flags = LVFI_PARAM | LVFI_WRAP;
	lvFind.lParam = (long)pui;
	CMemberList *memb = doc ? (CMemberList *) doc->m_memberList : GetMembers();
	return (memb->m_MemberListBox.FindItem(&lvFind));
}

int RemoveMemberFromList(CUserInfo* pui) {
	int index = FindMemberListIndex(pui);
	GetMembers()->m_MemberListBox.DeleteItem(index);
	return index;
}


void CIUserPart(const char *nickname) {			// assume for now that parts represent parts from all channels
	void UpdateTitle(CChatDoc * = NULL);
	//	mapNickToPtr.RemoveKey(nickname);			// keep them there for now (facilitates garbage collection)
	CUserInfo *pui = LookupPui(nickname);
	if (!pui)
		return;
	pui->SetDeparted(TRUE);

//	ChatSetMemberCount(ChatGetMemberCount()-1);

	int index = FindMemberListIndex(pui);
	if (index != LB_ERR)
		GetMembers()->m_MemberListBox.DeleteItem(index);

	if (GetChatDoc()->m_bComicView)
		UpdateTitle();
	GetChatDoc()->ResetStatus(FALSE, TRUE);
	TRACE("%s has left the building\n", nickname);
}




void CopyArray(CDWordArray &source, CDWordArray &dest) inline {
	int nItems = source.GetUpperBound() + 1;
	dest.RemoveAll();
	for (int i = 0; i < nItems; i++) 
		if (source[i]) dest.Add(source[i]);
}

static CDWordArray tempTalkTos; // temporary storage during interim whispering to someone else

void AutoGreet(const char* szNick) {
	if (theApp.m_iGreetingType && puiSelf && puiSelf->IsOperator()) {
		CUserInfo *pui = LookupPui(szNick);
		if (!pui) return;

		// fill out message string variables
		CString strVar, strControlFull = theApp.m_strGreetingMesg;

		strVar.LoadString(IDS_USERVARIABLE);
		const char *screenName = pui->GetScreenName();
		if (stricmp(strVar, screenName)!= 0)
			while (ReplaceToken(strControlFull, strVar, screenName));

		strVar.LoadString(IDS_ROOMVARIABLE);
		if (stricmp(strVar, currentRoom->m_strPrettyChannel)!= 0)
			while (ReplaceToken(strControlFull, strVar, currentRoom->m_strPrettyChannel));

		// REGISB: 11/13/97 new m_udi in this function
		CopyArray(puiSelf->m_udi.m_talkTos, tempTalkTos);	// temporarily swap out our talktos array
		puiSelf->m_udi.m_talkTos.RemoveAll();
		puiSelf->m_udi.m_talkTos.Add((DWORD) pui);
		
		CDWordArray	rgdwFormatting;
		char		*szControlFull = strdup((LPCTSTR) strControlFull);
		char		*szControlLess = SzControlLess(szControlFull, &rgdwFormatting);

		// send the message
		switch (theApp.m_iGreetingType) {
		case AGT_SAY:
			ChatSendText(CString(szControlLess), SM_SAY, TRUE, &rgdwFormatting);
			break;
		case AGT_WHISPER:
			whisperees.RemoveAll();
			whisperees.Add(pui);
			ChatSendText(CString(szControlLess), SM_WHISPER, TRUE, &rgdwFormatting);
			break;
		}
		CopyArray(tempTalkTos, puiSelf->m_udi.m_talkTos);

		free(szControlFull);
	}
}

void CRoomInfo::ChatAnnounceNewAvatar(const char *szAvName, const char *szAddressee, BOOL bForce)
{
	if (GetConnectionStatus() != CX_INCHANNEL)
		return;

	if (bSendComicsData || bForce)
	{
		if (!szAvName || !*szAvName)
			szAvName = "NONE";		// make sure we send an Avatar Name
		sprintf(GetOutBuff(), "#%s%s.", APPEARSPREFIX, szAvName);

		if (!szAddressee)
			ChatSendToChannel(GetOutBuff() /*szAnnotations*/, NULL /*szMesg*/, NULL /*szNMText*/); // initially we send a spurious announcement prior to connection
		else
			ChatSendPrivMesg(szAddressee, GetOutBuff() /*szAnnotations*/, NULL /*szText*/);
	}
}


BOOL ProcessComment(CChatDoc *doc, CUserInfo *pui, char *szMesg, BYTE msgType) {
	// Low Level Unquoting for \r \n
	bLowLevelUnquoting(g_chLLQuoteCTCP, TRUE /*bTreatAsByteArray*/, szMesg, szMesg);

	ASSERT(*szMesg == '#');			// should have already been checked
	szMesg++;						// nuke the crosshatch
	CRoomInfo *proto = doc ? doc->m_proto : currentRoom; // XXX fix for multiprotocol!

	int iMatch = !strncmp(szMesg, APPEARSPREFIX, g_nAppearsAsLen);
	if (iMatch)
	{
		char *szVar = szMesg + g_nAppearsAsLen;
		char *szCharName = GetToken(szVar, &szVar);
		if (!szCharName)
			return FALSE;			// djk - BETA1 Fix
		if (proto)					// REGISB added 11/17/97 Fix 4503
		{
			ASSERT(pui);
			if (!pui->Ignored() && !pui->IsFlooding(doc))	// REGISB 11/18/97: we don't accept "# Appears As" of ignored users - is this OK??
			{
				if (!pui->IsComicUser())
				{
					if (msgType == MT_CHANNELSEND)
						proto->ChatAnnounceNewAvatar(GetMyCharacter(), pui->GetName(), TRUE /*bForce*/);  // Now send avatar info privately
					pui->ComicUser(TRUE);
				}
				AddAndExecute(new ChangeAvatarEntry(pui, szCharName), doc);
			}
		}
		return TRUE;
	}

	iMatch = !strncmp(szMesg, GETINFOPREFIX, strlen(GETINFOPREFIX));
	if (iMatch)
	{
		if (proto)					// REGISB added 11/17/97 Fix 4503
		{
			ASSERT(pui);
			if (!pui->Ignored() && !pui->IsFlooding(doc))
			{
				CString strProfile;

				TRACE("You've been probed by %s!\n", pui->GetScreenName());

				if (theApp.m_myProfile.IsEmpty())
					strProfile.LoadString(ID_DEFAULT_PROFILE);	// no profile
				else
					strProfile = theApp.m_myProfile;

				sprintf(GetOutBuff(), "#%s%s", HERESINFOPREFIX, strProfile);
				VERIFY(proto->ChatSendPrivMesg(pui->GetName(), NULL /*szAnnotations*/, GetOutBuff() /*szMesg*/, NULL /*szNMText*/, FALSE /*bAsNotice*/, SM_HERESINFO /*byteMode*/));
			}
		}
		return TRUE;
	}

	iMatch = !strncmp(szMesg, HERESINFOPREFIX, g_nHeresInfoLen);
	if (iMatch)
	{
		if (proto)					// REGISB added 11/17/97 Fix 4503
		{
			// Beta1 fix: verify that information was actually requested!
			ASSERT(pui);
			if (pui->IsRequestInfo(RF_PROFILE))
			{
				CString strFullMesg;
				char *strToEnd = szMesg + g_nHeresInfoLen;
				strFullMesg.LoadString(IDS_SHOWINFO_PREFIX);
				VERIFY(ReplaceToken(strFullMesg, CString("%1"), pui->GetScreenName()));
				VERIFY(ReplaceToken(strFullMesg, CString("%2"), strToEnd));
				AddAndExecute(new GetInfoEntry(pui, UnConst((LPCTSTR) strFullMesg)), doc);
				pui->DecrementRequestInfo(RF_PROFILE);
			}
			else
				pui->IsFlooding(doc);	// Count this unexpected message
		}
		return TRUE;
	}
	
	iMatch = !strncmp(szMesg, BACKGRNDPREFIX, strlen(BACKGRNDPREFIX));
	if (iMatch)
	{
		if (proto)					// REGISB added 11/17/97 Fix 4503
		{
			ASSERT(pui);
			if (!pui->Ignored() && !pui->IsFlooding(doc))
			{
				if (pui->IsOperator())
				{
					char *strToEnd = szMesg+strlen(BACKGRNDPREFIX);
					while (isspace(*strToEnd))
						strToEnd++;
					if (*strToEnd)
						AddAndExecute(new ChangeBackDropEntry((const char*) strToEnd), doc);
				}
			}
		}
		return TRUE;
	}

	return FALSE;
}


BYTE IndexToByte(BYTE byteIn)
{
	return byteIn + '0';
}


BYTE ByteToIndex(BYTE byteIn)
{
	return byteIn - '0';
}


#define GESTUREPREFIX		'G'
#define EXPRESSIONPREFIX	'E'
#define REQUESTEDPREFIX		'R'
#define MODEPREFIX			'M'
#define TALKTOPREFIX		'T'

void GetTalkTos(CChatDoc *doc, CUserInfo *talkerPui, char *str) {
	// REGISB: 11/13/97 new m_udi in this function
	talkerPui->m_udi.m_talkTos.RemoveAll();
	while (TRUE) {
		while (isspace(*str)) str++;
		if (*str == ')' || *str == '\0') return;
		char *name = GetToken(str, &str);
		if (!name) return;
		CUserInfo *pui = LookupPui(name, doc);
		if (pui) talkerPui->m_udi.m_talkTos.Add((DWORD) pui);
	}
}

void GetTalkTos(CChatDoc *doc, CDWordArray *talkTos, char *str) {
	while (TRUE) {
		while (isspace(*str)) str++;
		if (*str == '\0') return;
		char *name = GetToken(str, &str, ",");
		if (!name) return;
		CUserInfo *pui = LookupPui(name, doc);
		if (pui) talkTos->Add((DWORD) pui);
	}
}


char *PrepareTextAction(CUserInfo *pui, char *szMesg, CString &strNewMesg, BYTE &byteMode) {
	strNewMesg = pui->GetScreenName();
	strNewMesg += (szMesg + g_nActionLen);
	int iEndIndex = strNewMesg.Find(0x01);
	if (iEndIndex >= 0) strNewMesg = strNewMesg.Left(iEndIndex);
	byteMode = SM_ACTION;
	return (UnConst(strNewMesg));
}

char *PrepareComicsAction(CUserInfo *pui, char *szMesg, CString &strNewMesg) {
	strNewMesg = pui->GetScreenName();
	strNewMesg += " ";
	strNewMesg += szMesg;
	return (UnConst(strNewMesg));
}

void CRoomInfo::ReplyVersion(CUserInfo *pui)
{
	extern void GetVersionString(CString &);
	CString strVersion, strMode;

	GetVersionString(strVersion);
	if (GetChatDoc()->m_bComicView)
		strMode.LoadString(IDS_COMICS_MODE);
	else
		strMode.LoadString(IDS_TEXT_MODE);

	sprintf(GetOutBuff(), "%c%.*s %s %s%c", 0x01, g_nVersionLen - 1,
		    versionID+1, strVersion, strMode, 0x01);
	ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
}

void CRoomInfo::ReplyPing(CUserInfo *pui, CString mesg) {
	sprintf(GetOutBuff(), "%c%.*s %s%c", 0x01, g_nPingLen - 1, pingID+1, mesg, 0x1);
	ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
}

void CRoomInfo::ReplyTime(CUserInfo *pui) {
	char buff1[50], buff2[50];
	GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, buff1, sizeof(buff1)); 
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, buff2, sizeof(buff2));
	sprintf(GetOutBuff(), "%c%.*s %s, %s%c", 0x01, g_nTimeLen -1, timeID+1, buff1, buff2, 0x1);
	ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
}

void CRoomInfo::ReplyEmail(CUserInfo *pui) {
	sprintf(GetOutBuff(), "%c%.*s %s%c", 0x01, g_nEmailLen - 1, emailID+1, GetMyEmail(), 0x1);
	ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
}

void CRoomInfo::ReplyHomePage(CUserInfo *pui) {
	sprintf(GetOutBuff(), "%c%.*s %s%c", 0x01, g_nUrlLen - 1, urlID+1, GetMyHomePage(), 0x1);
	ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
}


void ShowVersion(CUserInfo *pui, CString strMesg)
{
	if (pui->IsRequestInfo(RF_VERSION))
	{
		CString strProcessedMesg;
		strProcessedMesg.LoadString(IDS_VERSION_PREFIX);
		VERIFY(ReplaceToken(strProcessedMesg, CString("%1"), pui->GetScreenName()));
		strMesg.TrimLeft();
		int iEnd = strMesg.Find((char) 0x01);
		if (iEnd >= 0)
			strMesg = strMesg.Left(iEnd);
		VERIFY(ReplaceToken(strProcessedMesg, CString("%2"), strMesg));
		AddAndExecute(new GetInfoEntry(pui, UnConst((LPCTSTR) strProcessedMesg)));
		pui->DecrementRequestInfo(RF_VERSION);
	}
}

void ShowPing(CUserInfo *pui, const char *mesg) {
	if (pui->CheckFlag(UF_REQUESTPING)) {
		time_t oldTime, newTime;
		if (sscanf(mesg, " %ld", &oldTime) != 1) return;
		time(&newTime);
		CString displayMsg, seconds;
		displayMsg.LoadString(IDS_PING_MESSAGE);
		VERIFY(ReplaceToken(displayMsg, CString("%1"), pui->GetScreenName()));
		seconds.Format("%d", newTime-oldTime);
		VERIFY(ReplaceToken(displayMsg, CString("%2"), seconds));
		AddAndExecute(new GetInfoEntry(pui, UnConst((LPCTSTR) displayMsg)));
		pui->SetFlag(UF_REQUESTPING, FALSE);
	}
}

void ShowTime(CUserInfo *pui, CString mesg) {
	if (pui->IsRequestInfo(RF_TIME)) {
		CString displayMsg, seconds;
		displayMsg.LoadString(IDS_LOCALTIME_MESSAGE);
		VERIFY(ReplaceToken(displayMsg, CString("%1"), pui->GetScreenName()));
		int end = mesg.Find((char)0x01);
		if (end >= 0) mesg = mesg.Left(end);
		VERIFY(ReplaceToken(displayMsg, CString("%2"), mesg));
		AddAndExecute(new GetInfoEntry(pui, UnConst((LPCTSTR) displayMsg)));
		pui->DecrementRequestInfo(RF_TIME);
	}
}

void ShowEmail(CUserInfo *pui, CString strAddress) {
	if (pui->IsRequestInfo(RF_EMAIL)) {
		int end = strAddress.Find((char)0x01);
		if (end >= 0) strAddress = strAddress.Left(end);
		strAddress.TrimLeft();
		if (strAddress.IsEmpty()) AfxMessageBox(IDS_NO_EMAIL_ADDRESS);
		else {
			CString command;
			command.Format("mailto:%s", strAddress);
			FLaunchBrowser(command);
		}
		pui->DecrementRequestInfo(RF_EMAIL);
	}
}

void ShowHomePage(CUserInfo *pui, CString strURL) {
	if (pui->IsRequestInfo(RF_HOMEPAGE)) {
		int end = strURL.Find((char)0x01);
		if (end >= 0) strURL = strURL.Left(end);
		strURL.TrimLeft();
		if (!strURL.IsEmpty()) {
			if (strnicmp(strURL, "http://", 7)) strURL = "http://" + strURL;  // make sure it's an http
			FLaunchBrowser(strURL);
		} else AfxMessageBox(IDS_NO_HOMEPAGE);
		pui->DecrementRequestInfo(RF_HOMEPAGE);
	}
}


void ShowAway(CUserInfo *pui, CString strAwayMsg, CChatDoc *doc)
{
	// strAwayMsg is a control full string

	CString	strMesg;
	int		iEnd = strAwayMsg.Find((char) 0x01);

	if (iEnd >= 0)
		strAwayMsg = strAwayMsg.Left(iEnd);	// get rid of closing 0x01

	BOOL bAway = !strAwayMsg.IsEmpty();

	if (!bAway)
	{
		strMesg.LoadString(IDS_BACKREPORT);
		VERIFY(ReplaceToken(strMesg, CString("%1"), pui->GetScreenName()));
	}
	else
	{
		strMesg.LoadString(IDS_AWAYREPORT);
		VERIFY(ReplaceToken(strMesg, CString("%1"), pui->GetScreenName()));
		VERIFY(ReplaceToken(strMesg, CString("%2"), strAwayMsg));
	}
	void DoUserAway(CChatDoc *doc, CUserInfo *pui, BOOL bAway);
	DoUserAway(doc, pui, bAway);
	AddAndExecute(new GetInfoEntry(pui, UnConst((LPCTSTR) strMesg)), doc);
}


void _cdecl ConfConnect(void *arg)
{
	HCONF    hConf;
	CONFINFO confInfo;
	CONFADDR confAddr;

	DWORD dwIp = (long) arg;
	// Check if we're in an existing conference
	ZeroMemory(&confInfo, sizeof(confInfo));
	confInfo.dwSize = sizeof(confInfo);
	if (CONFERR_SUCCESS == ConferenceGetInfo(NULL, CONF_ENUM_CONF, &confInfo))
	{
		// use existing conference
		hConf = confInfo.hConf;
	}
	else
	{
		// startup a new one
		hConf = NULL;
	}

	ZeroMemory(&confAddr, sizeof(confAddr));
	confAddr.dwSize = sizeof(confAddr);
	confAddr.dwAddrType = CONF_ADDR_IP;
	confAddr.dwIp = dwIp;

	confInfo.dwMediaType = CONF_MT_DATA | CONF_MT_AUDIO;

	/*return */ ConferenceConnect(&hConf, &confAddr, &confInfo, NULL);
}

void CRoomInfo::DoNetMeetingCX(CUserInfo *pui, CString strAddr) {
	static BOOL bNMRequestUp = FALSE;

	if (isalpha(strAddr[0])) {	// first, check if we're actually receiving a response!!!
		if (pui->IsRequestInfo(RF_NETMEETING)) {
			CString mesg;
			if (strnicmp(strAddr, "REFUSED", 7) == 0) mesg.LoadString(IDS_NMCALL_REFUSED);
			else if (strnicmp(strAddr, "NOHAVE", 6) == 0) mesg.LoadString(IDS_NMCALL_NOHAVE);
			else ASSERT(0);	// unexpected strAddr
			VERIFY(ReplaceToken(mesg, CString("%1"), pui->GetScreenName()));
			AfxMessageBox(mesg);
			pui->DecrementRequestInfo(RF_NETMEETING);
			return;
		}
	}

	// initiate the NetMeeting Connection
	const char *response = NULL;
	if (!theApp.m_bAcceptNMCalls || !bCanViewUnrated()) response = "REFUSED";
	else if (!bNMInstalled()) response = "NOHAVE";
	if (response) {
		sprintf(GetOutBuff(), "%.*s %s%c", g_nNetMeetLen, netMeetingID, response, 0x01);
		ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
	} else {
		if (bNMRequestUp) return;
		bNMRequestUp = TRUE;

		CString prompt;
		prompt.LoadString(IDS_NMPROMPT);
		VERIFY(ReplaceToken(prompt, CString("%1"), pui->GetScreenName()));
		if (AfxMessageBox(prompt, MB_YESNO) == IDYES) {
			int end = strAddr.Find((char)0x01);
			if (end >= 0) strAddr = strAddr.Left(end);
			TRACE("Doing a NetMeetingCX on %s\n", strAddr);
			long hostAddr = atol(strAddr); // ConfConnect expects in host ordering, which is what was sent	
			// ConfConnect can take a while (especially if NetMeeting setup required) so use separate thread.
			void _cdecl ConfConnect (void *);
			_beginthread(ConfConnect, 0, (void *) hostAddr);
		}
		bNMRequestUp = FALSE;
	}
}


void LaunchMicrosoftURL(UINT resourceID) {
	CString prefix, suffix;
	prefix.LoadString(IDS_URL_MSPREFIX);
	suffix.LoadString(resourceID);
	prefix += suffix;
	FLaunchBrowser(prefix);
}


void _cdecl ConfListenThread(void *) {
	ConferenceListen(0);
}

void CRoomInfo::ChatStartNetMeeting(CUserInfo *pui) {
	if (!bCanViewUnrated())
		AfxMessageBox(IDS_NM_BAD_RATINGS);
	else if (!bNMInstalled()) {
		if (AfxMessageBox(IDS_INSTALL_NETMEETING, MB_YESNO) == IDYES)
			LaunchMicrosoftURL(IDS_NETMEETING_URL);
	} else if (pui && !pui->IsDeparted()) {
		char hostname[100];
		if (gethostname(hostname, sizeof(hostname))) return;
		struct hostent *h2 = gethostbyname(hostname);
		if (!h2 || h2->h_length < 1 || !h2->h_addr_list[0]) return;
		long hostID = *((long *) h2->h_addr_list[0]);
		hostID = ntohl(hostID);
		// use separate thread for netmeeting, since it can take a bit of time to start up (especially if needs setup)
		_beginthread(ConfListenThread, 0, (void *) 0);
		pui->IncrementRequestInfo(RF_NETMEETING);		// so we put up rejection dialogs (Note: may stay set if no reply)
		sprintf(GetOutBuff(), "%c%.*s %lu%c", 0x01, g_nNetMeetLen - 1, netMeetingID+1, hostID, 0x1);
		ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
	}
}


char* PrepareSound(CUserInfo *pui, char *szMesg, CString &strNewMesg, BYTE &byteMode)
{
	char *szSound = szMesg + g_nSoundLen, *szEnd;

	while (my_isspace(*szSound))
		szSound++;

	if (!*szSound)
		return "";				// returning empty string cancels display

	if (*szSound == '"')
	{
		szEnd = strchr(++szSound, '"');
		if (!szEnd)
			return "";  // no matching quote
	}
	else
	{
		szEnd = strchr(szSound+1, ' ');
		if (!szEnd)
			szEnd = strchr(szSound, 0x01);
		if (!szEnd)
			szEnd = strchr(szSound, '\0');
	}

	CString		strFile(szSound, szEnd-szSound);
	const char*	szFile = strFile;
	char		szResetSeq[MAX_FORMATTINGPERBYTE];
	BOOL		bNeedFree = FALSE;
	if (*szSound != '"')
		bNeedFree = CTCPUnQuoteString(&szFile);
	TRACE("Got sound %s\n", szFile);
	
	BOOL FindAndPlaySound(const char*);

	if (theApp.m_bPlaySounds)
		FindAndPlaySound(szFile);

	if (*szEnd == '"')
		szEnd++;			// now end must be at space or end
	strNewMesg = pui->GetScreenName();
	strNewMesg += szEnd;
	int iEndIndex = strNewMesg.Find(0x01);
	if (iEndIndex >= 0)
		strNewMesg = strNewMesg.Left(iEndIndex);

	// tack on sound feedback
	//if (*szEnd > 0x01) // REGISB fix 3230
	//strNewMesg += " "; // ie, there is a message

	if (nResettingSequence(szEnd, szResetSeq))
		strNewMesg += CString(szResetSeq);

	strNewMesg += " (";
	strNewMesg += szFile;
	strNewMesg += ")";

	if (bNeedFree)
		free ((void *) szFile);
	
	byteMode = SM_ACTION;

	return UnConst(strNewMesg);
}

static BOOL foundMe;
void CheckForSelf(char *name, void *myName) {
	if (!stricmp(name, (const char *) myName)) foundMe = TRUE;
}

// Check to see if it's directed especially at us
void IdentifyWhispers(CChatDoc *doc, CUserInfo *pui, BYTE msgType, BYTE &byteMode, CDWordArray *talkTos) {
	if (msgType == MT_PRIVATEMSG) {
		byteMode = SM_WHISPER;
		// REGISB: 11/13/97 new m_udi in this function
		pui->m_udi.m_talkTos.RemoveAll();
		// if talkTos specified use it, otherwise only add self
		if (talkTos) {
			int upper = talkTos->GetUpperBound();
			for (int i = 0; i <= upper; i++)
				pui->m_udi.m_talkTos.Add(talkTos->GetAt(i));
		} else {
			CUserInfo *me = doc ? doc->m_puiSelf : ExternalPui(GetMyNickName(), "", TRUE);
			pui->m_udi.m_talkTos.Add((DWORD) me);
		}
	}
}

BOOL AcceptWhispers() {
	if (!theApp.m_bAcceptWhispers) return FALSE;
    // DJK - TODO - need to check channel whisper mode! (see below)
#if 0
	if (GetConnectionStatus() == CX_INCHANNEL) {
		// check to see if room can technically receive whispers.  Note this is
		// necessary, since private messages could still make it through
		DWORD dwMode;
		VERIFY(g_pChan->bGetChannelType(&dwMode));
		if (dwMode & CS_CHANNEL_NOWHISPER) return FALSE;
	}
#endif
	return TRUE;
}


void ProcessUDIData(CChatDoc *pDoc, CUserInfo *pui, char *szData) 
{
	ASSERT(szData && *szData);
	ASSERT(pui);

	char *szTmp = szData;

	pui->m_udi.Reset();

	pui->m_bbValidUDI = 0;

	if (theApp.m_bVIPMode && !pui->IsOperator())
		return;	// VIP's ignore messages from riffraff

	ASSERT(*szTmp == '#');

	szTmp++;

	if (*szTmp == GESTUREPREFIX)
	{
		szTmp++;
		if (*szTmp) pui->m_udi.m_chGest  = ByteToIndex(*szTmp++);
		if (*szTmp) pui->m_udi.m_chGestE = ByteToIndex(*szTmp++);
		if (*szTmp) pui->m_udi.m_chGestI = ByteToIndex(*szTmp++);
	}
	
	if (*szTmp == EXPRESSIONPREFIX)
	{
		szTmp++;
		if (*szTmp) pui->m_udi.m_chExpr  = ByteToIndex(*szTmp++);
		if (*szTmp) pui->m_udi.m_chExprE = ByteToIndex(*szTmp++);
		if (*szTmp) pui->m_udi.m_chExprI = ByteToIndex(*szTmp++);
	}
	
	if (*szTmp == REQUESTEDPREFIX)
	{
		szTmp++;
		pui->m_udi.m_bbReq = 1;
	}
	
	if (*szTmp == MODEPREFIX)
	{
		szTmp++;
		if (*szTmp) pui->m_udi.m_byteMode = ByteToIndex(*szTmp++);

		if (pui->m_udi.m_byteMode < SM_SAY ||
			pui->m_udi.m_byteMode > SM_ACTION ||
			pui->m_udi.m_byteMode == SM_SHOUT)
			pui->m_udi.m_byteMode = SM_SAY;   // ensure mode in band!
	}
	
	if (*szTmp == TALKTOPREFIX)
	{
		szTmp++;
		GetTalkTos(pDoc, &(pui->m_udi.m_talkTos), szTmp);
	}

	if (pui->m_udi.m_chGestI != -1 && pui->m_udi.m_chExprI != -1)
		pui->m_udi.m_bbCooked = 1;

	pui->m_bbValidUDI = 1;	// ready for next text message
}


void ProcessSay(CChatDoc *doc, CUserInfo *pui, char *szMesg, BYTE msgType, CDWordArray *talkTos) 
{
	ASSERT(pui);
	
	CRoomInfo*	proto = doc ? doc->m_proto : currentRoom; // XXX fix for multiprotocol!
	CString		strActionMesg;
	BOOL		bFloodChecked = FALSE;

	if (msgType == MT_PRIVATEMSG && !AcceptWhispers())
		goto exitCheckFlood;

	if (theApp.m_bVIPMode && !pui->IsOperator())
	{
		pui->m_bbValidUDI = 0;
		goto exitCheckFlood;   // VIP's ignore messages from riffraff
	}

	// CUserDisplayInfo	udi, *pudi;
	// pudi = &udi;
	
	// Low Level Unquoting for \r \n
	bLowLevelUnquoting(g_chLLQuoteCTCP, TRUE /*bTreatAsByteArray*/, szMesg, szMesg);

	// parse off initial parenthetical info
	if (!strncmp(szMesg, "(#", 2) && strstr(szMesg+2, ") ")) {
		char *szStart = szMesg + 2;
		if (*szStart == GESTUREPREFIX) {
			szStart++;
			if (*szStart) pui->m_udi.m_chGest  = ByteToIndex(*szStart++);
			if (*szStart) pui->m_udi.m_chGestE = ByteToIndex(*szStart++);
			if (*szStart) pui->m_udi.m_chGestI = ByteToIndex(*szStart++);
		}
		if (*szStart == EXPRESSIONPREFIX) {
			szStart++;
			if (*szStart) pui->m_udi.m_chExpr  = ByteToIndex(*szStart++);
			if (*szStart) pui->m_udi.m_chExprE = ByteToIndex(*szStart++);
			if (*szStart) pui->m_udi.m_chExprI = ByteToIndex(*szStart++);
		}
		if (*szStart == REQUESTEDPREFIX) {
			szStart++;
			pui->m_udi.m_bbReq = 1;
		}
		if (*szStart == MODEPREFIX) {
			szStart++;
			if (*szStart) pui->m_udi.m_byteMode = ByteToIndex(*szStart++);
			if (pui->m_udi.m_byteMode < SM_SAY || 
				pui->m_udi.m_byteMode > SM_ACTION || 
				pui->m_udi.m_byteMode == SM_SHOUT)
				pui->m_udi.m_byteMode = SM_SAY;   // ensure mode in band!
		}

		pui->m_udi.m_talkTos.RemoveAll();
		if (*szStart == TALKTOPREFIX) {
			szStart++;
			GetTalkTos(doc, pui, szStart);
		}

		szStart = strstr(szStart, ") ");
		if (szStart && pui->m_udi.m_chGestI != -1 && pui->m_udi.m_chExprI != -1)
		{
			pui->m_udi.m_bbCooked = 1;
			szMesg = szStart + 2;		// advance string to end of parenthetical annotation
			ASSERT(!pui->m_bbValidUDI);	// shouldn't get any embedded annotations if m_udi is valid
		}
	}
	else
		if (!pui->m_bbValidUDI)
			pui->m_udi.Reset();
		//if (pui->m_bbValidUDI)
		//{
		//	pudi = &(pui->m_udi);
		//
		//	int iUpper = pui->m_udi.m_talkTos.GetUpperBound();
		//	for (int i = 0; i <= iUpper; i++)
		//		pui->m_talkTos.Add(pui->m_udi.m_talkTos.GetAt(i));
		//}

	pui->m_bbValidUDI = 0;	// pui->m_udi no more valid for next incoming text

	//ASSERT(pudi);

	//if (pudi->m_byteMode == SM_ACTION)
	if (pui->m_udi.m_byteMode == SM_ACTION)
	{
		if (!pui->Ignored() && !pui->IsFlooding(doc))
		{
			szMesg = PrepareComicsAction(pui, szMesg, strActionMesg);
			bFloodChecked = TRUE;
		}
		else
			return;
	}
	else if (strncmp(szMesg, actionID, g_nActionLen) == 0)
	{
		//szMesg = PrepareTextAction(pui, szMesg, strActionMesg, pudi->m_byteMode);
		if (!pui->Ignored() && !pui->IsFlooding(doc))
		{
			szMesg = PrepareTextAction(pui, szMesg, strActionMesg, pui->m_udi.m_byteMode);
			bFloodChecked = TRUE;
		}
		else
			return;
	}
	else if (strncmp(szMesg, soundID, g_nSoundLen) == 0)
	{
		//szMesg = PrepareSound(pui, szMesg, strActionMesg, pudi->m_byteMode);
		if (!pui->Ignored() && !pui->IsFlooding(doc))
		{
			szMesg = PrepareSound(pui, szMesg, strActionMesg, pui->m_udi.m_byteMode);
			bFloodChecked = TRUE;
		}
		else
			return;
	}
	else if (strnicmp(szMesg, versionID, g_nVersionLen) == 0)
	{
		const char *szOffset = szMesg + g_nVersionLen;
		if (*szOffset == 0x01)
		{
			if (!pui->Ignored() && !pui->IsFlooding(doc))
				if (proto)	// REGISB: we only respond if in a room. Should change this limitation.
					proto->ReplyVersion(pui);
		}
		else
		{
			if (my_isspace(*szOffset))
				ShowVersion(pui, szOffset+1);
			else
				goto exitCheckFlood;
		}
		return;
	} 
	else if (strnicmp(szMesg, pingID, g_nPingLen) == 0)
	{
		const char *szOffset = szMesg + g_nPingLen;
		if (!pui->CheckFlag(UF_REQUESTPING))
		{
			if (!pui->Ignored() && !pui->IsFlooding(doc))
				if (proto)	// REGISB: we only respond if in a room. Should change this limitation.
					proto->ReplyPing(pui, szOffset+1);
		}
		else
		{
			if (my_isspace(*szOffset))
				ShowPing(pui, szOffset+1);
			else
				goto exitCheckFlood;
		}
		return;
	}
	else if (strnicmp(szMesg, timeID, g_nTimeLen) == 0)
	{
		const char *szOffset = szMesg + g_nTimeLen;
		if (*szOffset == 0x01)
		{
			if (!pui->Ignored() && !pui->IsFlooding(doc))
				if (proto)	// REGISB: we only respond if in a room. Should change this limitation.
					proto->ReplyTime(pui);
		}
		else
		{
			if (my_isspace(*szOffset))
				ShowTime(pui, szOffset+1);
			else
				goto exitCheckFlood;
		}
		return;
	}
	else if (strnicmp(szMesg, fileDCCID, g_nFileDCCLen) == 0)
	{
		char *szOffset = szMesg + g_nFileDCCLen;
		void ChatReceiveFile(CUserInfo *, char *);
		
		if (!pui->Ignored() && !pui->IsFlooding(doc))
			if (my_isspace(*szOffset))
				ChatReceiveFile (pui, szOffset+1);
		return;
	}
	else if (strnicmp(szMesg, emailID, g_nEmailLen) == 0)
	{
		const char *szOffset = szMesg + g_nEmailLen;
		if (*szOffset == 0x01)
		{
			if (!pui->Ignored() && !pui->IsFlooding(doc))
				if (proto)	// REGISB: we only respond if in a room. Should change this limitation.
					proto->ReplyEmail(pui);
		}
		else
		{
			if (my_isspace(*szOffset))
				ShowEmail(pui, szOffset+1);
			else
				goto exitCheckFlood;
		}
		return;
	}
	else if (strnicmp(szMesg, urlID, g_nUrlLen) == 0)
	{
		const char *szOffset = szMesg + g_nUrlLen;
		if (*szOffset == 0x01)
		{
			if (!pui->Ignored() && !pui->IsFlooding(doc))
				if (proto)	// REGISB: we only respond if in a room. Should change this limitation.
					proto->ReplyHomePage(pui);
		}
		else
		{
			if (my_isspace(*szOffset))
				ShowHomePage(pui, szOffset+1);
			else
				goto exitCheckFlood;
		}
		return;
	}
	else if (strnicmp(szMesg, netMeetingID, g_nNetMeetLen) == 0)
	{
		const char *szOffset = szMesg + g_nNetMeetLen;
		if (my_isspace(*szOffset))
		{
			if (!pui->Ignored() && !pui->IsFlooding(doc))
				if (proto)	// REGISB: we only respond if in a room. Should change this limitation.
					proto->DoNetMeetingCX(pui, szOffset+1);
		}
		else
			goto exitCheckFlood;
		return;
	}
	else if (strnicmp(szMesg, awayID, g_nAwayLen) == 0)
	{
		const char *szOffset = szMesg + g_nAwayLen;
		if (!pui->Ignored() && !pui->IsFlooding(doc))
			ShowAway(pui, szOffset+1, doc);
		return;
	}
	else if (strnicmp(szMesg, clientInfoID, g_nClientInfoLen) == 0)
	{
		const char *szOffset = szMesg + g_nClientInfoLen;
		if (*szOffset == 0x01)
		{
			if (!pui->Ignored() && !pui->IsFlooding(doc))
				if (proto)	// REGISB: we only respond if in a room. Should change this limitation.
				{
					sprintf(GetOutBuff(), "%.*s %s%c", g_nClientInfoLen, clientInfoID, "ACTION AWAY CLIENTINFO DCC EMAIL NETMEET PING SOUND TIME USERINFO URL VERSION", 0x1);
					proto->ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
				}
		}
		else
			goto exitCheckFlood;
		return;
	}
	else if (strnicmp(szMesg, xvchatID, g_nXVChatLen) == 0)
	{
		// ignore X-VCHAT CTCPs
		return;
	}
	else if (*szMesg == 0x01 && szMesg[1] == '*')
	{	// until NOTICE'ed
		if (strnicmp(szMesg+2, versionID+1, g_nVersionLen - 1) == 0) { 
			const char *szOffset = szMesg + g_nVersionLen + 1;
			if (my_isspace(*szOffset))
				ShowVersion(pui, szOffset+1);
			else
				goto exitCheckFlood;
			return;
		}
		if (strnicmp(szMesg+2, timeID+1, g_nTimeLen - 1) == 0) {
			const char *szOffset = szMesg + g_nTimeLen + 1;
			if (my_isspace(*szOffset))
				ShowTime(pui, szOffset+1);
			else
				goto exitCheckFlood;
			return;
		}
		if (strnicmp(szMesg+2, pingID+1, g_nPingLen - 1) == 0) {
			const char *szOffset = szMesg + g_nPingLen + 1;
			if (my_isspace(*szOffset))
				ShowPing(pui, szOffset+1);
			else
				goto exitCheckFlood;
			return;
		}
		if (strnicmp(szMesg+2, emailID+1, g_nEmailLen - 1) == 0) {
			const char *szOffset = szMesg + g_nEmailLen + 1;
			if (my_isspace(*szOffset))
				ShowEmail(pui, szOffset+1);
			else
				goto exitCheckFlood;
			return;
		}
		if (strnicmp(szMesg+2, urlID+1, g_nUrlLen - 1) == 0) {
			const char *szOffset = szMesg + g_nUrlLen + 1;
			if (my_isspace(*szOffset))
				ShowHomePage(pui, szOffset+1);
			else
				goto exitCheckFlood;
			return;
		}
		if (strnicmp(szMesg+2, netMeetingID+1, g_nNetMeetLen - 1) == 0) {
			const char *szOffset = szMesg + g_nNetMeetLen + 1;
			if (my_isspace(*szOffset) && proto)	// REGISB: we only respond if in a room. Should change this limitation.
				proto->DoNetMeetingCX(pui, szOffset+1);
			else
				goto exitCheckFlood;
			return;
		}
	} 
	else
		if (*szMesg == 0x01)
			goto exitCheckFlood;  // don't display or respond to other CTCP messages

	if (!bFloodChecked && (pui->Ignored() || pui->IsFlooding(doc)))
		return;

	// if the whisper info was present in the cooked fields, use it.  Otherwise need to set it.
	//if (!pudi->m_bbCooked || pui->m_talkTos.GetUpperBound() < 0) 
	//	IdentifyWhispers(doc, pui, msgType, pudi->m_byteMode, talkTos);	// old code when we had m_talkTos in CUserInfo

	if (!pui->m_udi.m_bbCooked || pui->m_udi.m_talkTos.GetUpperBound() < 0) 
		IdentifyWhispers(doc, pui, msgType, pui->m_udi.m_byteMode, talkTos);

	if (proto && (proto->m_dwModes & CM_NOFORMAT)) 
		//pudi->m_byteMode = SM_NOFORMAT;
		pui->m_udi.m_byteMode = SM_NOFORMAT;

	if (strlen(szMesg) > 0)
		//if (!AddToWhisperBox(pui, pudi->m_byteMode, szMesg))
		if (!AddToWhisperBox(pui, pui->m_udi.m_byteMode, szMesg))
			AddAndExecute(new SayEntry(pui, szMesg, NULL), doc);

	return;

exitCheckFlood:
	pui->IsFlooding(doc);
}


void OnKick(CChatDoc *pDoc, char *szKicker, char *szKickee, char *szMesg) {
	TRACE("%s kicked %s with message (%s).\n", szKicker, szKickee, szMesg);
	BOOL bMeKicked = !stricmp(szKickee, GetMyNickName());
	CUserInfo *kickeePui = LookupPui(szKickee, pDoc);
	CUserInfo *kickerPui = LookupPui(szKicker, pDoc);

	if (!kickeePui) {		
		ASSERT(0);
		return;
	}

	// XXX - kickerPui == NULL for DALNET which allows kicks from externals
	// And when a sysop kills channel, szKicker is "System"

	CString strBoxMsg;
	if (szMesg && *szMesg)
	{
		strBoxMsg.LoadString(ID_KICK_MESG);
		VERIFY(ReplaceToken(strBoxMsg, CString("%3"), szMesg));
	}
	else
		strBoxMsg.LoadString(ID_KICK_NO_MESG);
	VERIFY(ReplaceToken(strBoxMsg, CString("%1"), kickerPui ? kickerPui->GetScreenName() : szKicker));
	VERIFY(ReplaceToken(strBoxMsg, CString("%2"), kickeePui->GetScreenName()));

	if (kickerPui)
	{
		// REGISB: 11/13/97 new m_udi in this function
		kickerPui->m_udi.m_chGest = 
		kickerPui->m_udi.m_chExpr = 
		kickerPui->m_udi.m_chGestE = 
		kickerPui->m_udi.m_chGestI = 
		kickerPui->m_udi.m_chExprE = 
		kickerPui->m_udi.m_chExprI = 0;
		kickerPui->m_udi.m_bbCooked = 0;
		kickerPui->m_udi.m_bbReq = 1;
		kickerPui->m_udi.m_byteMode = SM_ACTION;
		kickerPui->m_udi.m_talkTos.RemoveAll();
		kickerPui->m_udi.m_talkTos.Add((DWORD)kickeePui);

		AddAndExecute(new SayEntry(kickerPui, strBoxMsg, NULL), pDoc);
	}

	AddAndExecute(new PartEntry(szKickee), pDoc);
	if (bMeKicked) {
		void GotPartChannel(CChatDoc *pDoc);
		GotPartChannel(pDoc);
		AfxMessageBox(strBoxMsg);
	}
}

void PreMemberNameChange(CChatDoc *doc, CUserInfo *pui, LV_ITEM &item) {
	if (doc->m_proto->GetConnectionStatus() != CX_INCHANNEL)
		return;															// only do this if memberlist populated
	int index = FindMemberListIndex(pui, doc);							// remove old entry from member list...
	item.mask = LVIF_TEXT | LVIF_IMAGE | LVIF_STATE | LVIF_PARAM;		// but first get some info on it...
	item.stateMask = LVIS_FOCUSED | LVIS_SELECTED | LVIS_STATEIMAGEMASK;// Fix 4481
	item.iItem = index;
	item.iSubItem = 0;
	char buff[100];
	item.pszText = buff;
	item.cchTextMax = sizeof(buff);
	CMemberListCtrl *membList = & ((CMemberList *)doc->m_memberList)->m_MemberListBox;
	VERIFY(membList->GetItem(&item));
	membList->DeleteItem(index);
}

void PostMemberNameChange(CChatDoc *doc, CUserInfo *pui, LV_ITEM &item) {
	if (doc->m_proto->GetConnectionStatus() != CX_INCHANNEL)
		return;  // only do this if memberlist populated
	// insert modified entry back into member list
	item.mask |= LVIF_TEXT;
	item.pszText = LPSTR_TEXTCALLBACK;
	item.iImage = I_IMAGECALLBACK;
	CMemberList *members = (CMemberList*) doc->m_memberList;
	item.iItem = members->GetSortPosition(pui);
	members->m_MemberListBox.InsertItem(&item);
	if (GetChatDoc()->m_bIconMembers)
		members->Sort();
}

void ReinstallPui(CUserInfo *pui, const char *newNick) {
	if (!pui) return;		// should never happen, but may if history list not initialized :-(
	mapNickToPtr->RemoveKey(pui->GetName());
	if (STATUS_CHAR(*newNick)) newNick++;	// remove the at sign if there is one
	pui->SetName(newNick); // XXXXX We need to reset print name!!!!!
	if (pui == puiSelf) SetMyNameNick(newNick);
	mapNickToPtr->SetAt(newNick, pui);
}
	
void ProcessNick(CChatDoc *doc, const char *oldNick, const char *newNick, BOOL updateMemberList) {
	CUserInfo *pui = LookupPui(oldNick, doc);
	if (!pui) {
		if (!stricmp(oldNick, GetMyName())) SetMyName(newNick);
		return;
	}
	if (strcmp(oldNick, newNick)) {		// ie, if they are different
		LV_ITEM item;
		TRACE("Changing old nick (%s) to %s.\n", oldNick, newNick);
		// remove from memberlist under old nickname...
		if (updateMemberList)
			PreMemberNameChange(doc, pui, item);

		// refile PUI
		ReinstallPui(pui, newNick);

		// add to memberlist under new nickname ...
		if (updateMemberList) {
			PostMemberNameChange(doc, pui, item);

			// might as well update title too...
			void UpdateTitle(CChatDoc * = NULL);
			if (GetChatDoc()->m_bComicView) UpdateTitle();
		}
	}
}

// much of this code similar to that in ProcessNick.  Combine?
void ChatChangeAdmin(CChatDoc *doc, const char *nick, int setModes, int unsetModes) {
	CUserInfo *pui = LookupPui(nick, doc);
	if (!pui) return;

	if (setModes & UF_OPERATOR) pui->Ignore(FALSE);	// can't ignore ops (do in case ignored before)

	LV_ITEM item;
	PreMemberNameChange(doc, pui, item);
	pui->SetFlag(setModes, TRUE);
	pui->SetFlag(unsetModes, FALSE);
	pui->SetFlag(UF_SPECTATOR, (doc->m_proto->m_dwModes & CM_MODERATED) && !pui->CheckFlag(UF_HASVOICE) && !pui->CheckFlag(UF_OPERATOR));
	PostMemberNameChange(doc, pui, item);

	if (pui == puiSelf)
		doc->UpdateAdminMenu();
}

void AddIgnore(const char *nickMask) {
	theApp.m_ignores.SetAt(nickMask, NULL);
}

void RemoveIgnore(const char *nickMask) {
	theApp.m_ignores.RemoveKey(nickMask);
}

BOOL IsIgnored(const char *nickMask) {
	void *dummy;
	return (theApp.m_ignores.Lookup(nickMask, dummy));
}

void UpdateIgnoreOnEntry(const char *room, const char *nick, const char *user, const char *host) {
	CChatDoc *doc = LookupDoc(room);
	if (doc) {
		CUserInfo *pui = LookupPui(nick, doc);
		if (pui) {
			CString ident(user);
			ident += "@";
			ident += host;
			pui->SetFullName(ident);
			if (!pui->IsSelf() && IsIgnored(ident)) {
				LV_ITEM item;
				PreMemberNameChange(doc, pui, item);
				pui->Ignore(TRUE);
				PostMemberNameChange(doc, pui, item);
			}
		}
	}
}

void IgnoreUser(const char *nick, const char *nickMask, BOOL bIgnore, BOOL bAutoIgnore) {
	if (bIgnore) {
		if (!IsIgnored(nickMask))
			AddIgnore(nickMask);  // prevent duplicate entries
	} else
		RemoveIgnore(nickMask);

	CString mesg;
	UINT mesgID = bAutoIgnore ? IDS_AUTOFLOOD_IGNORE : (bIgnore ? IDS_IGNORE_FEEDBACK : IDS_UNIGNORE_FEEDBACK);
	POSITION posRoom = g_docs.GetHeadPosition();					// For each room
	while (posRoom) {
		CChatDoc *doc = (CChatDoc *) g_docs.GetNext(posRoom);
		if (!doc->GetConnectionStatus() == CX_INCHANNEL)
			continue;
		POSITION posPui = doc->m_mapNickToPtr.GetStartPosition();
		while (posPui) {											// For each user
			void *objPtr;
			CString dummy;
			doc->m_mapNickToPtr.GetNextAssoc(posPui, dummy, objPtr);
			CUserInfo *pui = (CUserInfo *) objPtr;
			if (!pui->IsSelf() && !pui->IsDeparted() &&
				(pui->GetFullName().IsEmpty() || pui->MatchesNickMask(nickMask)) &&
				(!*nick || stricmp(pui->GetName(), nick) == 0)) {
				LV_ITEM item;
				PreMemberNameChange(doc, pui, item);
				pui->Ignore(bIgnore);
				PostMemberNameChange(doc, pui, item);
				mesg.LoadString(mesgID);
				VERIFY(ReplaceToken(mesg, CString("%1"), pui->GetScreenName()));
				AddAndExecute(new GetInfoEntry(pui, UnConst(mesg)), doc);
			}
		}
	}
	CWhisperBox *wbox = GetWhisperBox();
	if (wbox) {
		int upper = wbox->m_leaves.GetUpperBound();
		for (int i = 0; i <= upper; i++) {
			CWhisperLeaf *leaf = (CWhisperLeaf *) wbox->m_leaves[i];
			if (leaf->m_nick == nick || leaf->m_id == nickMask) {
				mesg.LoadString(mesgID);
				VERIFY(ReplaceToken(mesg, CString("%1"), DecodeNick(leaf->m_nick)));
				leaf->m_richCore->iDisplayInfo(NULL, 0, "", 0, mesg, 0, mtGetInfo, msParticipant, NULL, NULL, 0);
				leaf->m_bIgnore = bIgnore;
				((CButton *)wbox->GetDlgItem(IDC_IGNORE_WBOX))->SetCheck(bIgnore ? 1 : 0);
			}
		}
	}
	// now update ignore of externals with same name combo
	CUserInfo *pui = ExternalPui(nick, nickMask, FALSE);
	if (pui) pui->Ignore(bIgnore);
}

void DoUserAway(CChatDoc *doc, CUserInfo *pui, BOOL bAway) {
	LV_ITEM item;
	if (doc) PreMemberNameChange(doc, pui, item);
	pui->SetFlag(UF_AWAY, bAway);
	if (doc) PostMemberNameChange(doc, pui, item);
}


BOOL CIrcProto::SlashJoin(IRCPARSE *parse) {
	int status = GetConnectionStatus();
	if (status != CX_NOCHANNEL && status != CX_INCHANNEL) {
		AfxMessageBox(IDS_MUSTBE_CONNECTED);
		return FALSE;
	}

	if (parse->nArgs < 2 || parse->nArgs > 3 || _mbschr((UCHAR *)parse->args[1], ',')) {
		AfxMessageBox(IDS_JOIN_SYNTAX);
		return FALSE;
	}

	const char *key = parse->nArgs > 2 ? parse->args[2] : "";

	return bSwitchToRoom(parse->args[1], key, TRUE);
}

BOOL CIrcProto::SlashPart(IRCPARSE *parse) {
	int status = GetConnectionStatus();
	if (status != CX_NOCHANNEL && status != CX_INCHANNEL) {
		AfxMessageBox(IDS_MUSTBE_CONNECTED);
		return FALSE;
	}

	if (parse->nArgs > 2) {
		AfxMessageBox(IDS_PART_SYNTAX);
		return FALSE;
	}

	CChatDoc *doc = (parse->nArgs > 1) ? LookupDoc(parse->args[1]) : GetChatDoc();

	if (!doc) {
		CString mesg;
		if (parse->nArgs > 1) {
			mesg.LoadString(IDS_NOTINROOM);
			VERIFY(ReplaceToken(mesg, CString("%1"), parse->args[1]));
		} else mesg.LoadString(IDS_NOTINANYROOM);
		AfxMessageBox(mesg);
		return FALSE;
	}

	doc->OnLeave();
	return TRUE;
}

BOOL CIrcProto::SlashNick(IRCPARSE *parse) {
	if (parse->nArgs != 2) {
		AfxMessageBox(IDS_NICK_SYNTAX);
		return FALSE;
	}
	ChatSetNick(parse->args[1]);
	return TRUE;
}

BOOL CRoomInfo::SlashRaw(char *mesg) {
	int status = currentRoom->GetConnectionStatus();
	if (status != CX_NOCHANNEL && status != CX_INCHANNEL) {
		AfxMessageBox(IDS_MUSTBE_CONNECTED);
		return FALSE;
	}

	mesg = strchr(mesg, ' ');
	if (!mesg) return TRUE;
	while (my_isspace(*mesg)) mesg++;

	sprintf(GetOutBuff(), "%s\r\n", mesg);

	SendMessageText(GetOutBuff());
	return TRUE;
}

BOOL CRoomInfo::ProcessSlashCommand(char *mesg) {
	AfxMessageBox(IDS_NOSLASH_COMMANDS);
	return FALSE;
}
	

BOOL CIrcProto::ProcessSlashCommand(char *mesg) {
	void ParseIt(const char *message, IRCPARSE *parse), FreeParse(IRCPARSE *parse);
	IRCPARSE parse;
	BOOL rval = FALSE;

	ParseIt(mesg, &parse);

	if (!stricmp(parse.args[0], "/JOIN"))
		rval = SlashJoin(&parse);
	else if (!stricmp(parse.args[0], "/PART"))
		rval = SlashPart(&parse);
	else if (!stricmp(parse.args[0], "/NICK"))
		rval = SlashNick(&parse);
//	else if (!stricmp(parse.args[0], "/QUOTE") || !(stricmp(parse.args[0], "/RAWXX")))
//		rval = SlashRaw(mesg);

	else AfxMessageBox(IDS_NOSLASH_COMMANDS);

	FreeParse(&parse);
	return rval;
}


void GetAddressees(CUserInfo *pui, const char *separator, CString &s, BOOL bUseNick) {
	// REGISB: 11/13/97 new m_udi in this function
	int upperbound = pui->m_udi.m_talkTos.GetUpperBound();
	upperbound = min(upperbound, 4);    // clip at first 4 (so don't overrun output buff)
	for (int i = 0; i <= upperbound; i++) {
		ULONG p = pui->m_udi.m_talkTos[i];
		CUserInfo *pui2 = (CUserInfo *)(pui->m_udi.m_talkTos[i]);
		const char *nickname = bUseNick ? pui2->GetName() : pui2->GetScreenName();
		s += nickname;
		if (i != upperbound) s += separator;
	}
}


void GetWhisperedAddressees(const char *separator, CString &s) {
	int upperbound = whisperees.GetUpperBound();
	upperbound = min(upperbound, 4);   // clip at first 4 (so don't overrun output buff)
	for (int i = 0; i <= upperbound; i++) {
		const char *nickname = ((CUserInfo *) whisperees[i])->GetName();
		s+= nickname;
		if (i != upperbound) s+= separator;
	}
}

void EmotionToBytes(CEmotion &em, BYTE &emotion, BYTE &intensity);

static BOOL InsertAnnotations(char *szBuff, BYTE byteMode, BOOL bIncludeParenthesis)
{
	CHAR		faceIndex, torsoIndex;
	BYTE		faceEmotion, faceIntensity, torsoEmotion, torsoIntensity;
	BYTE		bbRequested;
	CEmotion	face, torso;
    CAvatarX*	av = MyAvatar();

	if (av && puiSelf) {
		av->GetIndices(faceIndex, torsoIndex, bbRequested);
		av->GetEmotions(face, torso);
		BYTE faceIndexByte = IndexToByte(faceIndex);
		BYTE torsoIndexByte = IndexToByte(torsoIndex);
		BYTE modeByte = IndexToByte(byteMode);
		EmotionToBytes(face, faceEmotion, faceIntensity);
		EmotionToBytes(torso, torsoEmotion, torsoIntensity);

		sprintf(szBuff, "%s#%c%c%c%c%c%c%c%c%s%c%c",
				bIncludeParenthesis ? "(" : "",
				GESTUREPREFIX, torsoIndexByte, torsoEmotion, torsoIntensity,
				EXPRESSIONPREFIX, faceIndexByte, faceEmotion, faceIntensity,
				bbRequested ? "R" : "",
				MODEPREFIX, modeByte);
		// REGISB: 11/13/97 new m_udi in this function
		if (puiSelf->m_udi.m_talkTos.GetUpperBound() >= 0) {
			CString str = "T";
			if (byteMode == SM_WHISPER)
				GetWhisperedAddressees(",", str);
			else
				GetAddressees(puiSelf, ",", str, TRUE);
			strcat(szBuff, str);
		}

		if (bIncludeParenthesis)
			strcat(szBuff, ") ");
	}
	return TRUE;
}

void ProcessNonComicsMsg(CString &str, BYTE &byteMode)
{
	if (byteMode == SM_THINK)
	{
		CString	strPrefix;
		strPrefix.LoadString(ID_THINK_PREFIX);
		VERIFY(ReplaceToken(strPrefix, CString("%1"), ""));
		strPrefix.TrimLeft();
		str = strPrefix + str;
		byteMode = SM_ACTION;   // send it as an action
	}
	if (byteMode == SM_ACTION)
	{
		CString strPrefix = actionID;
		strPrefix += " ";
		str = strPrefix + str;
		str += "\001";
	}
}


BOOL ChatSendText(CString& str, BYTE byteMode, BOOL bEcho, CDWordArray* prgdwFormatting)
{
	SHORT		nLenBefore = str.GetLength();
	BOOL		bFreeTmp = FALSE;
	CDWordArray	*prgdwFormattingTmp = prgdwFormatting;

	if (byteMode != SM_WHISPER)
		ConfirmAway();  // give 'em a choice ot come back

	// strip off \n?
	str.TrimRight();
	if (str.IsEmpty()) 
		return TRUE;

	INT			iStatus;
	SHORT		nLenAfter = str.GetLength();
	BOOL		bSuccess = TRUE;
	BYTE		myMode = byteMode;
	BOOL		bComicView = GetChatDoc() ? GetChatDoc()->m_bComicView : FALSE;
	CRoomInfo*	proto = currentRoom ? currentRoom : GetDefaultProto();

	if (str[0] == '/')
		return proto->ProcessSlashCommand(UnConst(str));

//	if (bComicView && MyAvatar() == NULL)
//		return FALSE;		// just make sure...

	if (nLenBefore - nLenAfter)
	{
		prgdwFormattingTmp = CopyFormatting(prgdwFormatting);
		prgdwFormattingTmp = CutFormattingArray(prgdwFormattingTmp, nLenAfter);
		bFreeTmp = TRUE;
		nLenBefore = nLenAfter;
	}

	str.TrimLeft();	// need to do this for FE language, DBCS string can have leading spaces
	nLenAfter = str.GetLength();

	if (nLenBefore - nLenAfter)
	{
		CDWordArray	*prgdwFormattingTmp2 = PullFormattingOffsets(prgdwFormattingTmp, nLenBefore - nLenAfter);
		if (prgdwFormattingTmp && bFreeTmp)
		{
			prgdwFormattingTmp->RemoveAll();
			delete prgdwFormattingTmp;
		}
		prgdwFormattingTmp = prgdwFormattingTmp2;
		bFreeTmp = TRUE;
	}

	CString	myStr = str, strControlFull = str;

	if (prgdwFormattingTmp)
	{
		char* szCtrlFull = SzControlFull((LPCTSTR) str, prgdwFormattingTmp);
		if (!szCtrlFull)
		{
			bSuccess = FALSE;
			goto exit;
		}
		strControlFull = CString(szCtrlFull);
		delete [] szCtrlFull;
	}

	if (myMode == SM_ACTION)
	{	// save copies of the action w/ name prepended
		myStr = CString(GetMyName()) + CString(" ") + myStr;
		if (prgdwFormattingTmp)
			PushFormattingOffsets(prgdwFormattingTmp, strlen(GetMyName()) + 1);
	}

	iStatus = proto->GetConnectionStatus();

	if (iStatus == CX_INCHANNEL || iStatus == CX_NOCHANNEL) {  // only do this if connected
		void MListTalkTosToPuiself();
		MListTalkTosToPuiself();	// copy my talktos from member list to puiself
		char szAnnotations[MAX_ANNOTATIONS] = "";
		if (bComicView && (bSendComicsData || (GetDefaultProto() && GetDefaultProto()->IsIRCX())))
			InsertAnnotations(szAnnotations, byteMode, bSendComicsData /*bIncludeParenthesis*/);
		if ((!bSendComicsData || !bComicView) && (byteMode == SM_ACTION || byteMode == SM_THINK))
			ProcessNonComicsMsg(strControlFull, byteMode);
//		if (bSendComicsData && bComicView && byteMode == SM_ACTION)
//			// if sending a cooked action, make sure to prepend the name first
//			strControlFull = CString(GetMyName()) + CString(" ") + strControlFull;
		
		// If rich formating was done (prgdwFormattingTmp!=NULL) then send along the original
		// unformated string for NM in case we are talking to them too. IRC stuff will ignore
		// this.
		if (byteMode != SM_WHISPER) {
			if (!puiSelf)
			{
				bSuccess = FALSE;  // if haven't joined yet, don't say
				goto exit;
			}

			if (!(bSuccess = !puiSelf->CheckFlag(UF_SPECTATOR)))
				AfxMessageBox(IDS_SPECTATEONLY);
			else
				bSuccess = currentRoom->ChatSendToChannel(szAnnotations,
														  (LPCTSTR) strControlFull,
														  (prgdwFormattingTmp ? (char*)(LPCTSTR) str : NULL),
														  byteMode);
		}
		else 
			bSuccess = proto->SendWhispers(szAnnotations,
										   (LPCTSTR) strControlFull,
										   (prgdwFormattingTmp ? (char*)(LPCTSTR) str : NULL));
	}

	if ((proto->m_dwModes & CM_NOFORMAT) && byteMode != SM_WHISPER)
		bEcho = FALSE;

	if (bSuccess && bEcho)
		ShowSay(puiSelf, myStr, prgdwFormattingTmp, bComicView, myMode);	// don't receive PRIVMSGs sent by self, so do explicit show
	
exit:
	if (prgdwFormattingTmp && bFreeTmp)
	{
		prgdwFormattingTmp->RemoveAll();
		delete prgdwFormattingTmp;
	}

	return bSuccess;
}


void CRoomInfo::ChatGetInfo (CUserInfo *pui) {
	// Set a bit indicating that we requested this info,
	// ... so people can't nefariously send us info that we don't want to show.
	pui->IncrementRequestInfo(RF_PROFILE);
	sprintf(GetOutBuff(), "#%s", GETINFOPREFIX);
	// Send message requesting info directly to target
	ChatSendPrivMesg(pui->GetName(), GetOutBuff(), NULL);
}

void CRoomInfo::ChatSyncBackDrop(const char *szBackdrop)
{
	if (*szBackdrop && GetConnectionStatus() == CX_INCHANNEL)
	{
		CString strMesg;
		strMesg.Format("#%s %s", BACKGRNDPREFIX, szBackdrop);
		ChatSendToChannel(UnConst(strMesg) /*szAnnotations*/, NULL /*szMesg*/, NULL /*szNMText*/);   // always ANSI
	}
}


BOOL CUserInfo::IsFlooding(void *doc)
{
	if (!(theApp.m_floodFlags & FLOOD_IGNORE) || this == puiSelf)
		return FALSE;

	USHORT now = time(NULL) & 0xffff;
	int interval = abs(now - m_intervalStart);

	if (interval > theApp.m_floodInterval)
	{
		m_intervalStart = now;
		m_msgCount = 1;
		return FALSE;
	}
	else
	{
		if (++m_msgCount < theApp.m_floodCount)
			return FALSE;

		// evil flooder
		GetDefaultProto()->DoIgnoreUser(this, TRUE, TRUE);
#if 0
		DoUserIgnore((CChatDoc *)doc, this, TRUE);
		CString floodMesg;
		floodMesg.LoadString(IDS_AUTOFLOOD_IGNORE);
		VERIFY(ReplaceToken(floodMesg, CString("%1"), GetScreenName()));
		if (doc)
			AddAndExecute(new GetInfoEntry(this, UnConst(floodMesg)), (CChatDoc *)doc);
#endif
		return TRUE;
	}
}


BOOL CRoomInfo::ChatSendSound(const char *szSnd, const char *szMesg, CDWordArray *prgdwFormatting)
{
	if (!puiSelf) 
		return TRUE;

	ConfirmAway();
	
	if (puiSelf->CheckFlag(UF_SPECTATOR))
	{
		AfxMessageBox(IDS_SPECTATEONLY);
		return FALSE;
	}

	char		*szControlFull = NULL;
	const char	*szQuotedSnd = szSnd;

	BOOL bNeedFree = CTCPQuoteString(&szQuotedSnd);
	if (prgdwFormatting)
		szControlFull = SzControlFull(szMesg, prgdwFormatting);

	sprintf(GetOutBuff(), "%.*s %s %s%c", g_nSoundLen, soundID, szQuotedSnd, szControlFull ? szControlFull : szMesg, 0x1);

	if (szControlFull)
		delete [] szControlFull;

	if (GetConnectionStatus() == CX_INCHANNEL)
		ChatSendToChannel(NULL,						/*szAnnotations*/
						  UnConst(GetOutBuff()),	/*szMesg*/
						  NULL,						/*szNMText*/
						  SM_SOUND);				/*byteMode*/
	
	// and provide aural feedback
	BOOL FindAndPlaySound(const char *);
	if (theApp.m_bPlaySounds)
		FindAndPlaySound(szSnd);

	// and provide visual feedback (For historical reasons, need to tack on name)
	CString strComplete(puiSelf->GetScreenName());
	strComplete += " ";
	if (*szMesg)
	{
		PushFormattingOffsets(prgdwFormatting, strComplete.GetLength());
		strComplete += szMesg;
		if (prgdwFormatting)
			AddFormat(prgdwFormatting, MAKELONG(0, strComplete.GetLength()));
		strComplete += " ";  // space between mesg and sound string
	}
	strComplete += "(";
	strComplete += szSnd;
	strComplete += ")";

	ShowSay(puiSelf, strComplete, prgdwFormatting, ((CChatDoc*) m_doc)->m_bComicView, SM_ACTION);
	if (bNeedFree)
		free ((void *) szQuotedSnd);

	return TRUE;  /* zap */
}


BOOL CRoomInfo::SendWhispers(const char *szAnnotations, const char *szMesg, char *szNMText /*= NULL*/) {
	int iUpperbound = whisperees.GetUpperBound();
	for (int i = 0; i <= iUpperbound; i++) {
		CUserInfo *pui = (CUserInfo *) whisperees[i];
		if (!ChatSendPrivMesg(pui->GetName(), szAnnotations, szMesg, szNMText, FALSE, SM_WHISPER))
			return FALSE;
	}
	return TRUE;
}


BOOL bCanViewUnrated(BOOL bPromptOverride) {
	// RamuM - PICS rating stuff
	BOOL bAccess = TRUE;
	LPVOID pRatingDetails = NULL;

	// Load the Ratings DLL (if possible)
	ENSURE_LOADED(g_hinstRatings, c_tszRatingsDLL);

	// If Rating DLL is not found assume ratings are not enabled.

	if ((NULL != g_hinstRatings) && (S_OK == RatingEnabledQuery()))
	{
		if (S_OK != RatingCheckUserAccess(NULL, NULL, NULL, NULL, 0, &pRatingDetails))
		{
			HWND hwndParent;

			if (cui.GetChatViewPv())
				hwndParent = ((CWnd*) cui.GetChatViewPv())->m_hWnd;
			else
				hwndParent = ((CWnd*) cui.GetFramePv())->m_hWnd;

			// User cannot see Unrated sites, see if he can get supervisor override
			if (!bPromptOverride || (S_OK != RatingAccessDeniedDialog(hwndParent, NULL, NULL, pRatingDetails)))
				bAccess = FALSE;
		}

		RatingFreeDetails(pRatingDetails);
	}
	return bAccess;
}

BOOL bRatingsEnabled() {
	// Load the Ratings DLL (if possible)
	ENSURE_LOADED(g_hinstRatings, c_tszRatingsDLL);

	// If Rating DLL is not found assume ratings are not enabled.

	return ((NULL != g_hinstRatings) && (S_OK == RatingEnabledQuery()));
}


BOOL bNMInstalled() {
	return (ENSURE_LOADED(g_hinstMSConf, c_tszMSConfDLL));
}


int g_iQuery = QT_ROOMQUERY;
BOOL g_bCanViewUnrated;			// cached during room list


void StartRoomList() {
	CRoomList *rl = GetRoomList();
	if (rl) {
		rl->ClearRoomList();		// prepare for pending insertions
		rl->m_reset.EnableWindow(FALSE);
		// MakeEmpty already done in OnReset, but this prevents duplicates for multiple button presses
		rl->m_persist->MakeEmpty();
	}
}

void EndRoomList() {
	CRoomList *rl = GetRoomList();
	if (rl) {
		rl->SortRooms(TRUE);
		rl->AnnounceCount();
		rl->AnnounceTime();
		rl->m_reset.EnableWindow(TRUE);
		if (rl->m_bResetHadFocus) rl->m_reset.SetFocus();
	}
	theApp.m_bInSearch = FALSE;
}

void EndUserList() {
	CUserList *ul = GetUserList();
	if (ul) {
		ul->Sort(TRUE);
		ul->AnnounceCount();
		ul->m_reset.EnableWindow(TRUE);
		if (ul->m_bResetHadFocus) ul->m_reset.SetFocus();
	}
	theApp.m_bInSearch = FALSE;
}

void AddToRoomList(CRoom *room, BOOL bAddIt = TRUE) {
	CRoomList *rl = GetRoomList();
	if (rl && bAddIt) {
		room->CalculateSortByte();
		int roomIndex = rl->m_persist->AddRoom(room);
		rl->AddToRoomList(roomIndex);
		rl->AnnounceCount();
	} else
		delete room;
}

void AddToUserList(CUser *user) {
	const char *stristr(const char *str1, const char *str2);
	CUserList *ul = GetUserList();
	if (ul) {
		const char *searchstr = ul->m_persist->m_strUserFilter;
		if ((ul->m_persist->m_searchType == USERSEARCH_ALL ||
			(ul->m_persist->m_searchType == USERSEARCH_NICK && stristr(user->GetPrettyNick(), searchstr)) ||
			(ul->m_persist->m_searchType == USERSEARCH_ID && stristr(user->m_ident, searchstr)))) {
			int userIndex = ul->m_persist->AddUser(user);
			ul->AddToUserList(userIndex);
			ul->AnnounceCount();
		} else delete user;
	} else delete user;
}

#if 0
BOOL GetIdent(CUserInfo *pui, CString &strIdent) {
	BOOL GetIdent(const char *nick, CString &strIdent);
	return GetIdent(pui->GetName(), strIdent);
}
#endif

BOOL IsAuthenticated(CUserInfo *pui) {
	BOOL rval = FALSE;
//	PICS_MEMBER picsMember;
	// djk - TODO -- need to add authentification check
#if 0
	if (g_pChan->bGetMemberFromNicknameAnsi(pui->GetName(), &picsMember) && picsMember) {
		DWORD dwMode;
		if (SUCCEEDED(picsMember->HrGetMemberMode(&dwMode)) && (dwMode & CS_MEMBER_AUTHUSER))
			rval = TRUE;
		picsMember->Release();
	}
#endif
	return rval;   // OK, may mean fail too.
}

#if 0
void GetBanString(CUserInfo *pui, CString &strBanPattern) {
	if (pui && GetIdent(pui, strBanPattern)) {
		if (IsAuthenticated(pui))
			strBanPattern = "*!" + strBanPattern;  // authenticated, so trust user id
		else {
			int atIndex = strBanPattern.Find('@');
			strBanPattern = "*!*" + strBanPattern.Mid(atIndex);  // else ban entire host
		}

	} else strBanPattern = "";
}

#endif


#if 0
	if (pui && !pui->IsDeparted()) {
		CString strIdent, user, host;
		if (!GetIdent(pui, strIdent)) {
			mesg.LoadString(IDS_UNAVAILABLE_IDENT);
			VERIFY(ReplaceToken(mesg, CString("%1"), pui->GetName()));
			AfxMessageBox(mesg, MB_ICONINFORMATION);  // error, just quit operation
			return;
		}
		const char *start = strIdent;
		const char *atSign = strchr(strIdent, '@');
		if (atSign) {
			int iSplice = atSign - start;
			user = strIdent.Left(iSplice);
			host = strIdent.Mid(iSplice+1);
		} else {
			user = strIdent;
			host = "?";
		}
#endif

void ShowIdentity(const char *nick, const char *user, const char *host) {
	CString mesg;
	CUserInfo *pui = LookupPui(nick);
	if (!pui) return;
	mesg.LoadString(IDS_REPORT_IDENT2);
	VERIFY(ReplaceToken(mesg, CString("%1"), pui->GetScreenName()));
	VERIFY(ReplaceToken(mesg, CString("%2"), user));
	VERIFY(ReplaceToken(mesg, CString("%3"), host));
	AddAndExecute(new GetInfoEntry(LookupPui(nick), UnConst((LPCTSTR) mesg)));
}



void CRoomInfo::ChatGetVersion(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->IncrementRequestInfo(RF_VERSION);
		sprintf(GetOutBuff(), "%.*s%c", g_nVersionLen, versionID, 0x01);
		ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}

void CRoomInfo::ChatPingUser(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->SetFlag(UF_REQUESTPING, TRUE);
		time_t seconds;
		time(&seconds);
		sprintf(GetOutBuff(), "%.*s %ld%c", g_nPingLen, pingID, (long) seconds, 0x01);
		ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}

void CRoomInfo::ChatGetLocalTime(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->IncrementRequestInfo(RF_TIME);
		sprintf(GetOutBuff(), "%.*s%c", g_nTimeLen, timeID, 0x01);
		ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}

void CRoomInfo::ChatGetEmail(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->IncrementRequestInfo(RF_EMAIL);
		sprintf(GetOutBuff(), "%.*s%c", g_nEmailLen, emailID, 0x01);
		ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}

void CRoomInfo::ChatGetHomePage(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->IncrementRequestInfo(RF_HOMEPAGE);
		sprintf(GetOutBuff(), "%.*s%c", g_nUrlLen, urlID, 0x01);
		ChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}


void CRoomInfo::ChatSetAway(BOOL bAway, const char *szMesg, CUserInfo *pui, BOOL)
{
	// szMesg is a control full string

	CChatDoc *doc = (CChatDoc *) m_doc;

	if (bAway)
		sprintf(GetOutBuff(), "%.*s %s%c", g_nAwayLen, awayID, szMesg, 0x01);
	else
		sprintf(GetOutBuff(), "%.*s%c", g_nAwayLen, awayID, 0x01);

	if (pui)
		VERIFY(ChatSendPrivMesg(pui->GetName(),		/*szAddressee*/
								NULL,				/*szAnnotations*/
								GetOutBuff(),		/*szMesg*/
								NULL,				/*szNMText*/
								FALSE,				/*bAsNotice*/
								SM_AWAY));			/*byteMode*/
	else
	{
		ShowAway((CUserInfo*) doc->m_puiSelf, GetOutBuff()+g_nAwayLen+1, doc);
		VERIFY(ChatSendToChannel(NULL,				/*szAnnotations*/
								 GetOutBuff(),		/*szText*/
								 NULL,				/*szNMText*/
								 SM_AWAY));			/*byteMode*/
	}
}


void CRoomInfo::DoKickDlg(const char *nick, const char *strBan) {
	CKickDialog kickDlg;
	CString strDlg;
	strDlg.LoadString(IDS_KICKREASON);
	VERIFY(ReplaceToken(strDlg, CString("%1"), DecodeNick(nick)));
	kickDlg.m_strKick = strDlg;
	kickDlg.m_strBanPattern = strBan;
	if (kickDlg.DoModal() == IDOK) {
		if (kickDlg.m_bBanToo) {					// first ban (if requested)
			kickDlg.m_strBanPattern.TrimLeft();
			if (!kickDlg.m_strBanPattern.IsEmpty())
				ChatBanUser(UnConst(kickDlg.m_strBanPattern), TRUE);
		}
		ChatKickUser(nick, UnConst(kickDlg.m_reason));  // then kick
	}
	GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}

void DoBanDlg(const char *strBan, CStringArray &banArray) {
	CBanDlg banDlg;
	banDlg.m_strBanPattern = strBan;
	banDlg.m_strMesg.LoadString(IDS_BANMESG);
	BOOL bMIC = dwCurrentChannelMode & CM_MIC;
	VERIFY(ReplaceToken(banDlg.m_strMesg, CString("%1"), DecodeChan(strCurrentChannel, bMIC)));
	banDlg.m_banArray = &banArray;
	banDlg.DoModal();
	banArray.RemoveAll();			// don't keep strings around
	GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}

void DoInvite(char *invitee, void *proto1) {
	CRoomInfo *proto = (CRoomInfo *) proto1;
	proto->ChatSendInvitation(EncodeNick(invitee));
}


void CRoomInfo::ChatInvite() {
	if (GetConnectionStatus() == CX_INCHANNEL && puiSelf) {
		CInviteDlg inviteDlg;
		if (inviteDlg.DoModal() == IDOK)
			ForEachWord(UnConst(inviteDlg.m_strInvitees), DoInvite, this, " ,\r\n");

		GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
	}
}

// starting with an empty memberlist, repopulates it, according to our map's info.
void RepopulateMemberList() {
	void *p;
	CString attedNick, nick;
	extern CUserInfo *puiSelf;
	int GetAvatarUpperBound();

	// image list no longer destroyed on changing modes!!!
//	int avUpper = GetAvatarUpperBound();  // reset old icon indices -- the image list was destroyed
//	for (int i = 1; i <= avUpper; i++)
//		GetAvatar(i)->m_iconIndex = -1;

	ChatEmptyMemberList();
	POSITION pos = mapNickToPtr->GetStartPosition();
//	bInEnumeration = TRUE;				// no need to sort in AddToMembersList
	while (pos) {
		mapNickToPtr->GetNextAssoc(pos, nick, p);
		CUserInfo *pui = (CUserInfo *) p;
		if (!pui->IsDeparted())
			AddToMembersList(pui);
	}

	// REGISB: 11/13/97 new m_udi in this function
	if (puiSelf)
		puiSelf->m_udi.m_talkTos.RemoveAll();		// if any were selected, they aren't now.
//	bInEnumeration = FALSE;
}

void MapNullAvatars(CChatDoc *doc) {
	CString nick;

	POSITION pos = doc->m_allChannelPuis.GetHeadPosition();
	while (pos) {
		CUserInfo *pui = (CUserInfo *) doc->m_allChannelPuis.GetNext(pos);
		if (!pui->GetAvatarID()) { // null avID
			AssignArbitraryAvatar(pui);
		}
	}
}

void CIrcProto::ChatSetNick(const char *nick) {
	CString newNick = nick;
	CString oldNick = GetMyName();
	int online = GetConnectionStatus();
	if (online != CX_DISCONNECTED) {
		// we have to do this anyway if we're connecting and the names haven't changed,
		// since if we have a bad nick, we'll need to get back a bad nick response.
		// Otherwise, we know we don't have a bad nick, w/ other connect statuses.
		// NOTE: not true any more, since CX_CONNECTING means not yet logged in... 
		while (!ChatChangeNick(newNick)) {
			TryNewNick(ID_ERR_BAD_NICK, newNick, FALSE, &newNick);
			if (stricmp(newNick, oldNick) != 0) break;  // break if they entered a different nick
		}
		strRequestedNick = newNick;    // cache requested version in case we get a socket-based Nick Change Message
	} else if (online == CX_DISCONNECTED && strcmp(nick, oldNick))
		AddAndExecute(new NickEntry(oldNick, nick));  // do it right away (won't get a nick message back)
}


BOOL bCXKeepServer = FALSE;
void InitializeChannelConnection()
{
	bCXKeepServer = FALSE;						// reset to default value, since we've used it up
	if (bCXPrompt) {
		CChannelDlg dlg;
		dlg.DoModal();
		VERIFY(bInitEnterInfo(dlg.m_strChannel, dlg.m_strPassword, TRUE));
	}
	bCXPrompt = TRUE;							// only go without prompts once!

	GetIrcProto()->ChatJoinChannel();
}

void AdjustFlags(DWORD oldMode, BOOL newVal, UINT field, DWORD *setMask, DWORD *unsetMask)
{
	if (ISTRUE(oldMode & field) != newVal)
	{
		if (newVal) 
			*setMask |= field;
		else
			*unsetMask |= field;
	}
}


void CRoomInfo::DoChannelDialog()
{
	CChannelProp	cprops;
	extern CString	strCurrentChannelTopic;

	cprops.m_rtfTopic.DefineDefaultCharFormat();
	cprops.m_rtfTopic.m_strText = strCurrentChannelTopic;
	cprops.m_rtfTopic.m_prgdwFormatting = CopyFormatting(currentRoom->m_prgdwTopicFormatting);

	if (dwCurrentChannelMode & CM_USERLIMIT) {
		cprops.m_uMaxParticipants = dwCurrentUserLimit;
		cprops.m_bSetMax = (dwCurrentUserLimit > 0);
	}

	if (dwCurrentChannelMode & CM_CHANNELKEY) {
		int iEncoding = (strCurrentChannel[0] == '%' && !(dwCurrentChannelMode & CM_MIC)) ? ENC_UTF8 : ENC_DBCS;
		cprops.m_strPassword = DecodeString(strCurrentChannelKey, iEncoding);
		cprops.m_bSetPassword = !strCurrentChannelKey.IsEmpty();
	}

//	cprops.m_bAuditorium = ISTRUE(dwCurrentChannelMode & CS_CHANNEL_AUDITORIUM) && IsIrcX;
//	cprops.m_bNoWhispers = ISTRUE(dwCurrentChannelMode & CS_CHANNEL_NOWHISPER);
	cprops.m_bSecret = ISTRUE(dwCurrentChannelMode & CM_HIDDEN);
	cprops.m_bPrivate = ISTRUE(dwCurrentChannelMode & CM_PRIVATE);
	cprops.m_bInviteOnly = ISTRUE(dwCurrentChannelMode & CM_INVITEONLY);
	cprops.m_bModerated = ISTRUE(dwCurrentChannelMode & CM_MODERATED);
	cprops.m_bTopicAnyone = !ISTRUE(dwCurrentChannelMode & CM_TOPICHOST);
	
	if (cprops.DoModal() == IDOK) {
		if (strcmp((LPCTSTR) cprops.m_rtfTopic.m_strText, strCurrentChannelTopic) ||
			!bFormattingsEqual(cprops.m_rtfTopic.m_prgdwFormatting, currentRoom->m_prgdwTopicFormatting))
		{
			char* szControlFull = cprops.m_rtfTopic.m_prgdwFormatting ? SzControlFull(cprops.m_rtfTopic.m_strText, cprops.m_rtfTopic.m_prgdwFormatting) : NULL;

			ChatSetTopic(szControlFull ? szControlFull : cprops.m_rtfTopic.m_strText);

			if (szControlFull)
				delete [] szControlFull;
		}

		DWORD newMode = CM_NOEXTERN, newMaxUsers = 0;

		if (cprops.m_bSetMax) {
			newMaxUsers = cprops.m_uMaxParticipants;
			if (newMaxUsers > 0)
				newMode |= CM_USERLIMIT;
		}

		if (cprops.m_bSecret) newMode |= CM_HIDDEN;
		if (cprops.m_bPrivate) newMode |= CM_PRIVATE;
		if (cprops.m_bInviteOnly) newMode |= CM_INVITEONLY;
		if (cprops.m_bModerated) newMode |= CM_MODERATED;
		if (!cprops.m_bTopicAnyone) newMode |= CM_TOPICHOST;
		if (cprops.m_bSetPassword) newMode |= CM_CHANNELKEY;

		ChatSetMode(newMode, newMaxUsers, cprops.m_strPassword);
	}
	GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}


void ChatSwitchChannel(const char *roomName) {
	CChannelDlg dlg;
	if (roomName)  // fill it in if we are correcting an error
		dlg.m_strChannel = DecodeChan(roomName);					// don't know if it's MIC
	if (dlg.DoModal() == IDOK) {
		// check if room already exists -- if so bring to foreground...
		bSwitchToRoom(dlg.m_strChannel, dlg.m_strPassword, TRUE);
	}
}

void ShowBadChannelName(const char *roomName) {
	CString mesg;
	mesg.LoadString(ID_ERR_BADCHANNELNAME);   // DONE 2
	VERIFY(ReplaceToken(mesg, CString("%1"), DecodeChan(roomName)));  // don't know if it's MIC
	AfxMessageBox(mesg);
	ChatSwitchChannel(roomName); // so we show the bad room name (which may be cleared by JOIN #A,#B)
}


BOOL bInitEnterInfo(const char *channel, const char *password, BOOL bEncode)
{
	enterInfo.m_bSetMode = FALSE;
	enterInfo.m_dwModes = CM_NOEXTERN | CM_TOPICHOST;
	enterInfo.m_dwMaxUsers = 0L;
	enterInfo.m_strPassword = password ? password : "";
	enterInfo.m_strChannel = bEncode ? EncodeChan(channel) : channel;
	enterInfo.m_strTopic = "";
	if (enterInfo.m_prgdwTopicFormatting)
	{
		enterInfo.m_prgdwTopicFormatting->RemoveAll();
		delete enterInfo.m_prgdwTopicFormatting;
		enterInfo.m_prgdwTopicFormatting = NULL;
	}

	if (enterInfo.m_strChannel.Find(g_chComma) > -1)
	{
		ShowBadChannelName((LPCTSTR) enterInfo.m_strChannel);
		return FALSE;
	}
	else 
		return TRUE;
}

void ChatCreateRoom()
{
	CChannelCreateDlg cprops;

	// if we get rid of ChatRetryCreate, we can get rid of this InitEnterInfo, and following props being
	// set by consulting enterInfo.
	bInitEnterInfo("");

	// initialize
//	cprops.m_bNoWhispers = ISTRUE(enterInfo.channelType & CS_CHANNEL_NOWHISPER);
//	cprops.m_bAuditorium = ISTRUE(enterInfo.channelType & CS_CHANNEL_AUDITORIUM);

	cprops.m_bSecret = ISTRUE(enterInfo.m_dwModes & CM_HIDDEN);
	cprops.m_bPrivate = ISTRUE(enterInfo.m_dwModes & CM_PRIVATE);
	cprops.m_bInviteOnly = ISTRUE(enterInfo.m_dwModes & CM_INVITEONLY);
	cprops.m_bModerated = ISTRUE(enterInfo.m_dwModes & CM_MODERATED);
	cprops.m_bTopicAnyone = !ISTRUE(enterInfo.m_dwModes & CM_TOPICHOST);
	cprops.m_bSetMax = ISTRUE(enterInfo.m_dwMaxUsers);
	cprops.m_uMaxParticipants = enterInfo.m_dwMaxUsers;
	cprops.m_bSetPassword = (!enterInfo.m_strPassword.IsEmpty() && (enterInfo.m_dwModes & CM_CHANNELKEY));
	cprops.m_strPassword = enterInfo.m_strPassword;
	cprops.m_strChannelName = DecodeChan(enterInfo.m_strChannel);
	cprops.m_rtfTopic.m_strText = enterInfo.m_strTopic;
	if (cprops.m_rtfTopic.m_prgdwFormatting)
	{
		cprops.m_rtfTopic.m_prgdwFormatting->RemoveAll();
		delete cprops.m_rtfTopic.m_prgdwFormatting;
	}
	cprops.m_rtfTopic.m_prgdwFormatting = CopyFormatting(enterInfo.m_prgdwTopicFormatting);
	cprops.m_rtfTopic.DefineDefaultCharFormat();

	if (cprops.DoModal() == IDOK)
	{
		enterInfo.m_dwModes = CM_NOEXTERN;
//		if (cprops.m_bAuditorium) enterInfo.channelType |= CS_CHANNEL_AUDITORIUM;
//		if (cprops.m_bNoWhispers) enterInfo.channelType |= CS_CHANNEL_NOWHISPER;

		if (cprops.m_bSecret) enterInfo.m_dwModes |= CM_HIDDEN;
		if (cprops.m_bPrivate) enterInfo.m_dwModes |= CM_PRIVATE;
		if (cprops.m_bInviteOnly) enterInfo.m_dwModes |= CM_INVITEONLY;
		if (cprops.m_bModerated) enterInfo.m_dwModes |= CM_MODERATED;
		if (!cprops.m_bTopicAnyone) enterInfo.m_dwModes |= CM_TOPICHOST;
		if (cprops.m_bSetMax)
		{
			enterInfo.m_dwMaxUsers = cprops.m_uMaxParticipants;
			enterInfo.m_dwModes |= CM_USERLIMIT;
		}
		if (cprops.m_bSetPassword)
		{
			enterInfo.m_strPassword = cprops.m_strPassword;
			enterInfo.m_dwModes |= CM_CHANNELKEY;
		}
		enterInfo.m_strTopic = cprops.m_rtfTopic.m_strText;
		if (enterInfo.m_prgdwTopicFormatting)
		{
			enterInfo.m_prgdwTopicFormatting->RemoveAll();
			delete enterInfo.m_prgdwTopicFormatting;
		}
		enterInfo.m_prgdwTopicFormatting = CopyFormatting(cprops.m_rtfTopic.m_prgdwFormatting);
		enterInfo.m_bSetMode = TRUE;

		enterInfo.m_strChannel = EncodeChan(cprops.m_strChannelName);
		if (enterInfo.m_strChannel.Find(g_chComma) == -1)
			bSwitchToRoom(NULL);
		else
			ShowBadChannelName((LPCTSTR) enterInfo.m_strChannel);
			// enterInfo.m_strChannel = cprops.m_strChannelName; // for next try
	}
}

#if 0
void ChatRetryCreateRoom ()
{
	if (AfxMessageBox(IDS_ROOM_EXISTS, MB_YESNO) == IDYES)
		bSwitchToRoom(enterInfo.m_strChannel);
	else
		ChatCreateRoom();
}
#endif

BOOL bFreezeTabs = FALSE;

BOOL bProcessAddChannel(const char *szChanName, CRoomInfo *proto) // szChanName is encoded
{
	CChatDoc*	doc = LookupDoc(szChanName);
	if (!doc)
		doc = LookupDoc("");
	if (!doc)
	{
		bCXKeepServer = TRUE;
		bCXPrompt = FALSE;
		bFreezeTabs = TRUE;
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW, 0);
		bFreezeTabs = FALSE;
		doc = LookupDoc("");
		// REGISB Fix 4449
		if (!doc)
		{
			// New Document creation failed (because of low HD space for example)
			// -> need to leave the channel the user just joined
			ASSERT(proto);
			sprintf(GetOutBuff(), "PART %s\r\n", szChanName);  // exit gracefully
			proto->SendMessageText(GetOutBuff());
			delete proto;
			proto = NULL;

			CString strMesg;
			strMesg.LoadString(ID_ERR_GENERICCHAN);
			VERIFY(ReplaceToken(strMesg, CString("%1"), DecodeChan(szChanName)));
			AfxMessageBox(strMesg);

			return FALSE;
		}
	}

	ASSERT(doc);
	if (doc->m_proto)
	{
		delete doc->m_proto;
		currentRoom = doc->m_proto = proto;
		proto->m_doc = doc;
	}

	CFrameWnd *frame = doc->m_client->GetParentFrame();
	if (frame)
		frame->ActivateFrame();

	AdjustViewMode();
	strCurrentChannel = szChanName; // should be up-to-date now...
	currentRoom->m_strPrettyChannel = DecodeChan(szChanName);   // if it's a MIC room, we'll change this later
	ChatSetChannel(DecodeChan(enterInfo.m_strChannel));	// successfully connected, so remember requested channel
	proto->SetConnectionStatus(CX_INCHANNEL);		// we are officially in a channel
	doc->InitHistory();

	doc->SetLegalPath(currentRoom->m_strPrettyChannel);		// decoded true channel name

	return TRUE;
}

void ActivateWindow(CChatDoc *doc) {
	CFrameWnd *frame = doc->m_client->GetParentFrame();
	if (frame) frame->ActivateFrame();
}


void OnPart(char *nick) {
	AddAndExecute(new PartEntry(nick));
}

void OnNewNick(char *oldNick, char *newNick) {
	if (puiSelf && GetIrcProto()->GetConnectionStatus() == CX_INCHANNEL)  // i.e., we're recording a history
		AddAndExecute(new NickEntry(oldNick, newNick));
	else SetMyNameNick(newNick);
}


CUserInfo* PuiFromDocNickIdent(CChatDoc **ppDoc, char *szNickname, const char *szUserIdent)
{
	CChatDoc	*pDocTmp;
	CUserInfo	*pui = NULL, *puiTmp;

	ASSERT(ppDoc);

	if (*ppDoc)
	{
		pui = LookupPui(szNickname, *ppDoc);
		if (pui && pui->IsDeparted())
			pui = NULL;
	}
	else
	{
		pDocTmp = GetChatDoc();

		if (pDocTmp && pDocTmp->GetConnectionStatus() == CX_INCHANNEL) 
		{
			// try to interpret as utterance in current room
			pui = LookupPui(szNickname, pDocTmp);
			if (pui && pui->IsDeparted())
				pui = NULL;
			if (pui)
				*ppDoc = pDocTmp;
		}
	}

	if (!pui)
	{  // try other visible rooms...
		POSITION pos = g_docs.GetHeadPosition();
		while (pos)
		{
			pDocTmp = (CChatDoc*) g_docs.GetNext(pos);
			if (pDocTmp->GetConnectionStatus() != CX_INCHANNEL || pDocTmp->m_bObscured)
				break;	// shouldn't this be a continue; ??
			puiTmp = LookupPui(szNickname, pDocTmp);
			if (puiTmp && !puiTmp->IsDeparted())
			{
				pui = puiTmp;
				*ppDoc = pDocTmp;
				break;
			}
		}
	}

	if (!pui)
	{
		TRACE("Got an external message from: %s\n", szNickname);
		pui = ExternalPui(szNickname, szUserIdent, TRUE);
	}

	return pui;
}


void OnTextMsg(CChatDoc *doc, char *szNickname, const char *szUserIdent, char *szMesg, BYTE msgType = MT_CHANNELSEND, CDWordArray *talkTos = NULL) {
//	if (!puiSelf) return; // Don't process privmsgs out of channel
	TRACE("Got a text message! (snick = %s)\n", szNickname);
	if (CHANNELPREFIX(*szNickname))
		return;  // don't process messages from entire channels

	CUserInfo	*pui = PuiFromDocNickIdent(&doc, szNickname, szUserIdent);

	if (pui)		 // old code:	&& !pui->Ignored() && !pui->IsFlooding(doc))
	{
		if (*szMesg != '#' || !ProcessComment(doc, pui, szMesg, msgType))
			ProcessSay(doc, pui, szMesg, msgType, talkTos);
	}
}


void OnDataMsg(CChatDoc *pDoc, char *szNickname, const char *szUserIdent, char *szData, BYTE msgType = MT_CHANNELSEND)
{
	TRACE("Got a data message! (snick = %s)\n", szNickname);

	ASSERT(szData);
	ASSERT(szNickname && *szNickname);

	if (CHANNELPREFIX(*szNickname))
		return;  // don't process messages from entire channels

	CUserInfo	*pui = PuiFromDocNickIdent(&pDoc, szNickname, szUserIdent);

	if (pui)
		if (*(szData+1) == ' ')
		{
			// if (!pui->Ignored() && !pui->IsFlooding(pDoc))
			ProcessComment(pDoc, pui, szData, msgType);
		}
		else
		{
			ASSERT(msgType == MT_CHANNELSEND);	// this kind of annotations can only be channel messages
			ProcessUDIData(pDoc, pui, szData);
		}
}


void ProcessBeginEnumeration() {
	bInEnumeration = TRUE;
}


void ProcessEndEnumeration() {
	bInEnumeration = FALSE;

	if (!puiSelf) {			// if didn't find self, don't allow enter (add better fix post MSN2.0)
		CString mesg;
		mesg.LoadString(ID_ERR_CANTJOIN);
		VERIFY(ReplaceToken(mesg, CString("%1"), strCurrentChannel));
		AfxMessageBox(mesg);
		GetChatDoc()->m_proto->ChatPartChannel(GetChatDoc(), FALSE);
	}

	void UpdateTitle(CChatDoc * = NULL);
	if (GetChatDoc()->m_bComicView) {
		UpdateTitle();	// update title only after enum
		if (GetChatDoc()->m_bIconMembers) GetMembers()->Sort();					// sort members only after enum
	}

	GetChatDoc()->SetModifiedFlag(FALSE);   // starting converation
}



BOOL bPassesRatings(const char *szRating, BOOL bPromptOverride = FALSE) {
	if (!*szRating && g_bCanViewUnrated) return TRUE;  // if it has no PICS string and we can view unrated, return TRUE

	LPVOID pRatingDetails = NULL;
	BOOL bAccess = FALSE;
	if (S_OK == RatingCheckUserAccess(NULL, NULL, szRating, NULL, 0, &pRatingDetails))
		bAccess = TRUE;

	// User cannot see Unrated sites, see if he can get supervisor override
	else if (bPromptOverride && (S_OK == RatingAccessDeniedDialog(NULL, NULL, NULL, pRatingDetails)))
		bAccess = TRUE;

	if (pRatingDetails) 
		RatingFreeDetails(pRatingDetails);
	return bAccess;
}


void EndQuery() {
	if (g_iQuery == QT_ROOMQUERY)
		EndRoomList();
	else if (g_iQuery == QT_USERQUERY)
		EndUserList();
}

#if 0

static CRoom *newRoom = NULL;
static CUser *newUser = NULL;
static BOOL bAddIt;

BOOL FParseQueryData(PCS_PROPERTY pcsProp)
{
	BOOL fRet = TRUE;
	BOOL FOnPropString(CSPROP_TYPE csType,CHAR *sz);
	BOOL FOnPropBuffer(CSPROP_TYPE csType,BYTE *pbBuffer,DWORD dwcb);
	_ASSERT(pcsProp);

	DWORD	dwc;
	//
	// Make sure we have a property
	//
	if (pcsProp->picsProperty) {

		if (g_iQuery == QT_ROOMQUERY) {
			newRoom = new CRoom;
			bAddIt = TRUE;				// will be set to false if can't pass PICS test
		}

		else if (g_iQuery == QT_USERQUERY)
			newUser = new CUser;

		//
		// How many properties
		//	
		if (FAILED((pcsProp->picsProperty)->HrGetCount(&dwc))) {
			_ASSERT(0);
			return FALSE;
		}
		
		DWORD		dwi;
		CS_PROPDATA	cspd;
		
		for (dwi = 1; dwi <= dwc && fRet; ++dwi) {
			//
			// Retrieve the property
			//
			if (SUCCEEDED((pcsProp->picsProperty)->HrGetProperty(&cspd,dwi))) {
				if (!cspd.pbData || !cspd.fAnsi) // we don't do Unicode yet
					continue;
				if (cspd.fString)
					fRet = FOnPropString(cspd.csPropType,(CHAR *)cspd.pbData);
				else
					fRet = FOnPropBuffer(cspd.csPropType,cspd.pbData,cspd.dwcb);
			}		
		}

		if (g_iQuery == QT_ROOMQUERY) AddToRoomList(newRoom, bAddIt);
		else if (g_iQuery == QT_USERQUERY) AddToUserList(newUser);
	}

	if (pcsProp->fLastRecord) EndQuery();
		
	return fRet;
}

//
// WE did a query - such as list - Chatsock returns data asynchronously
// This is parsed by ESocket..and it calls FOnPropString if the data is a string
// Look at the property type and display the data appropriately
// Note: ESocket ignores non-ansi data
// To add granularity..look up more property types in <csface.h>,. the Chatsock header
//									
BOOL FOnPropString(CSPROP_TYPE csType,CHAR *sz)	// chatsock is returning query data
{
	_ASSERT(sz);
	BOOL bMustFree = ConvertEncodingIn(&sz);

	switch (csType) {
		default:
			TRACE("\tProperty %lx:\t%s",sz); 
			return TRUE;
		
		case CSPROP_MEMBER_NAME:
			newUser->m_nick = sz;
			break;
											                
		case CSPROP_MEMBER_REALNAME:
			newUser->m_ident = sz;
			break;
		
		case CSPROP_CHANNEL_NAME:
			newRoom->m_name = sz;
			break;
		
		case CSPROP_CHANNEL_TOPIC:
			newRoom->m_descr = sz;
			break;
			
		case CSPROP_CHANNEL_RATING_PICS:
			bAddIt = bPassesRatings(sz);
			break;
		}

	if (bMustFree)
		delete [] sz;
	return TRUE;
}

//
// WE did a query - such as list - Chatsock returns data asynchronously
// This is parsed by ESocket..and it calls FOnPropBuffer if the data is a buffer(not a string) - usually a DWORD
// Look at the property type and display the data appropriately
// Note: ESocket ignores non-ansi data
// To add granularity..look up more property types in <csface.h>,. the Chatsock header
//
BOOL FOnPropBuffer(CSPROP_TYPE csType,BYTE *pbBuffer,DWORD dwcb)
{
	_ASSERT(pbBuffer);

	switch (csType)
		{
		default:
			break;
		
		// DWORD		
//		case CSPROP_GENERIC_MODE:
//			::printf("\tMode:\t%lx\n",*((DWORD *)pbBuffer));  Not currently used for rooms
//			break;
		
		// DWORD		
		case CSPROP_CHANNEL_CUSER:
			newRoom->m_nUsers = *((DWORD *)pbBuffer);
			break;

		//DWORD
	    case CSPROP_CHANNEL_FLAGS:
			newRoom->m_bRegistered = (*((DWORD *)pbBuffer)) & CS_CHANNEL_FLAG_PERSIST;
			break;
		}
	
	return TRUE;	
}

#endif

BOOL bSwitchToRoom(const char *newRoom, const char *password, BOOL bEncode) {
	ConfirmAway();

	const char *room = newRoom ? newRoom : enterInfo.m_strChannel;
	if (bEncode)
		room = EncodeChan(room);

	if (strchr(room, g_chComma))
	{
		ShowBadChannelName(room);
		return FALSE;
	}

	CChatDoc *doc = LookupDoc(room);

	if (theApp.m_bEmbedded && currentRoom && currentRoom->m_strChannel != "") {
		// in case of embedded document, exit old room first
		if (!currentRoom->m_doc->SaveModified()) 
			return TRUE;
		currentRoom->m_doc->DeleteContents();
		((CChatDoc*) currentRoom->m_doc)->InitMyDocument();
		currentRoom->m_strChannel = "";  // make it reclaimable
		doc = NULL;
	}

	// if the document exists, but isn't connected, close it up and start a new one... (e.g., if was kicked out)
	if (doc && doc->GetConnectionStatus() != CX_INCHANNEL) {
		if (!doc->SaveModified())
			return TRUE;
		doc->OnCloseDocument();
		doc = NULL;
	}

	if (doc) {	// doc already live -- just activate
		CFrameWnd *frame = doc->m_client->GetParentFrame();
		if (frame)
			frame->ActivateFrame();
	} else {	// must create new doc and join room
		bCXPrompt = FALSE;
		if (newRoom) 
			VERIFY(bInitEnterInfo(room, password));  // don't reset if they passed in a NULL newRoom!
		InitializeChannelConnection();
	}

	return TRUE;
}


void OnInvite(const char *sender, const char *fullName, const char *room)
{
	static BOOL bInInvite = FALSE;

	if (!theApp.m_bAllowInvites || !bCanViewUnrated()) 
		return;

	if (IsIgnored(fullName)) return;   // do not allow invites from ignored folks

	if (bInInvite) 
		return;  // poor man's not-so-critical-section
	bInInvite = TRUE;

	CInvitationDlg invdlg;
	invdlg.m_strMessage.LoadString(ID_JOIN_OFFER);
	VERIFY(ReplaceToken(invdlg.m_strMessage, CString("%1"), DecodeNick(sender)));
	VERIFY(ReplaceToken(invdlg.m_strMessage, CString("%2"), DecodeChan(room)));		// don't know if MIC
	int rval = invdlg.DoModal();
	if (rval == IDYES)
		bSwitchToRoom(room);
	if (rval != IDCANCEL && invdlg.m_bIgnore)
		IgnoreUser(sender, fullName, TRUE, FALSE);

	bInInvite = FALSE;
}


#define JPN_SHIFT_JIS		932L
#define JPN_JIS				50220L
#define CP_EastEurUnicode	1250
#define CP_EastEurISO88592	28592

// REGISB 12/05/97 Russian conversion removed
//#define CP_RussianUnicode	1251
//#define CP_RussianKOI8	20866


BOOL ConvertEncodingIn(LPSTR *pszStr)
{
	extern int OurJIS_to_ShiftJIS (UCHAR *pJIS, int JIS_len, UCHAR **ppSJIS,  int ShiftJIS_len=0);
	
	if (theApp.m_charSet == ANSI_CHARSET) 
		return FALSE;  // fast path for common case

	switch (theApp.m_charSet)
	{
		case SHIFTJIS_CHARSET:
		{
			if (OurJIS_to_ShiftJIS ((UCHAR*) *pszStr,  -1, (UCHAR**) pszStr) > 0) 
				return TRUE;
			break;
		}
		case EASTEUROPE_CHARSET:
		{
			INT		cchWideStr, cchIn = _tcslen(*pszStr);
			WCHAR	*szWideStr = (WCHAR*) new WCHAR[cchIn+1];
			CHAR	*szOutStr = NULL;

			if (!szWideStr)
				return FALSE;

			if (!(cchWideStr = MultiByteToWideChar(CP_EastEurISO88592, 0L, *pszStr, cchIn+1, szWideStr, cchIn+1)))
			{
				delete [] szWideStr;
				return FALSE;
			}

			szOutStr = (CHAR*) new CHAR[2*cchWideStr+1];

			if (!szOutStr)
				return FALSE;

			if (!WideCharToMultiByte(CP_EastEurUnicode, 0L, szWideStr, cchWideStr, szOutStr, 2*cchWideStr+1, NULL, NULL))
			{
				delete [] szWideStr;
				delete [] szOutStr;
				return FALSE;
			}

			delete [] szWideStr;

			*pszStr = szOutStr;
			return TRUE;
		}
	}

	return FALSE;  // default -- not translated
}


BOOL ConvertEncodingOut(LPSTR *pszStr) 
{
	extern int OurShiftJIS_to_JIS (UCHAR *pShiftJIS,  int ShiftJIS_len, UCHAR **ppJIS,  int JIS_len=0);

	if (theApp.m_charSet == ANSI_CHARSET) 
		return FALSE;  // fast path for common case

	switch (theApp.m_charSet) 
	{
		case SHIFTJIS_CHARSET:
		{
			BOOL	bFree1 = FALSE, bFree2;
			LPSTR	szTmp;

			bSB2DBKatakana(*pszStr, 0, &szTmp, NULL, &bFree1);

			bFree2 = OurShiftJIS_to_JIS ((UCHAR*) szTmp,  -1, (UCHAR**) pszStr) > 0;

			if (bFree1 && bFree2)
				delete [] szTmp;

			if (bFree1 || bFree2) 
				return TRUE;
			break;
		}
		case EASTEUROPE_CHARSET:
		{
			INT		cchWideStr, cchIn = _tcslen(*pszStr);
			WCHAR	*szWideStr = (WCHAR*) new WCHAR[cchIn+1];
			CHAR	*szOutStr = NULL;

			if (!szWideStr)
				return FALSE;

			if (!(cchWideStr = MultiByteToWideChar(CP_EastEurUnicode, 0L, *pszStr, cchIn+1, szWideStr, cchIn+1)))
			{
				delete [] szWideStr;
				return FALSE;
			}

			szOutStr = (CHAR*) new CHAR[2*cchWideStr+1];

			if (!szOutStr)
				return FALSE;

			if (!WideCharToMultiByte(CP_EastEurISO88592, 0L, szWideStr, cchWideStr, szOutStr, 2*cchWideStr+1, NULL, NULL))
			{
				delete [] szWideStr;
				delete [] szOutStr;
				return FALSE;
			}

			delete [] szWideStr;

			*pszStr = szOutStr;
			return TRUE;
		}
	}

	return FALSE;  // default -- not translated
}


void OnBadChannelPassword() {
	CString QuoteAmpersands(const char *unquoted);
	CPasswordDlg pwdDlg;

	if (!enterInfo.m_strPassword.IsEmpty())
		AfxMessageBox(IDS_BAD_PASSWORD);

	pwdDlg.m_strMessage.LoadString(ID_PASSWORD_PROMPT);
	VERIFY(ReplaceToken(pwdDlg.m_strMessage, CString("%1"), QuoteAmpersands(DecodeChan(enterInfo.m_strChannel))));  // don't know if MIC
	pwdDlg.m_strPassword = enterInfo.m_strPassword;
	enterInfo.m_strPassword = "";		// so on canceled dialog, won't preserve password
	if (pwdDlg.DoModal() == IDOK)
		bSwitchToRoom(enterInfo.m_strChannel, pwdDlg.m_strPassword);
}

// Place limits on dancing.  As side effect: time of last dance stored in static lastDance.
// Returns true if OK to dance.
BOOL bCanDance() {
	static time_t lastDance = 0;
	time_t currentTime = time(NULL);
	if (abs(lastDance - currentTime) > 2) {
		lastDance = currentTime;
		return TRUE;
	} else return FALSE;
}

void GotPartChannel(CChatDoc *doc) {
	if (!doc) return;
	doc->m_proto->SetConnectionStatus(CX_NOCHANNEL);
	ChatEmptyMemberList(doc);
}

void OfflineEditInits() {
	GetChatDoc()->InitHistory();

	// Set things up so user can still type in text and
	//  get comics.  Great for debugging too.
	AddAndExecute(new JoinEntry(new CUserInfo(GetMyName())));  // pretend to join to set pui & puiSelf
}


void InitializeServerConnection() {
	BOOL ChatServerConnect(const char *);
	void ChatServerDisconnect();
	int DoConnectDialog();

	BOOL bGoodChannelName = TRUE;

	if (!CChatDoc::CleanupExistingWindows()) return;	// can't clean up old windows, so don't start new ones
//	if (*GetMyName() == '\0') TryNewNick(ID_ERR_NO_NICK);

	while (TRUE) {
		ChatServerDisconnect();	// first close last connection if necessary
		if (bCXPrompt) {
			int rval = DoConnectDialog();
			if (rval != IDOK) {		// on cancel or close, or anything except OK
				AdjustViewMode();
				OfflineEditInits();
				GetChatDoc()->SetModifiedFlag(FALSE);
				return;
			}
			bGoodChannelName = bInitEnterInfo(theApp.m_myChannel, NULL, TRUE);
		}
			
		bCXPrompt = TRUE;	// only good for one non-prompt (could also be set to false in above DoModal)
		if (bGoodChannelName)
			if (ChatServerConnect(GetMyServer())) break;
	}
}


void ChatSetPath(const char *path) {
	CChatDoc *doc = GetChatDoc();
	// note: doc is null if we're closing up IE3.0 w/ Connect dialog open
	if (doc) doc->SetPathName(path); 
}

int DoConnectDialog() {
	CCSPropertySheet	psOptions(IDS_CONNECTION);
	CSetupPage			ppSetup;
	CPersonalPage		ppPersonal(IDD_PERSONALPAGE_IRC);

	psOptions.m_psh.dwFlags &= ~(PSH_HASHELP);  // deactivate help button

	psOptions.AddPage(&ppSetup);
	psOptions.AddPage(&ppPersonal);

	if (theApp.m_bComicView) {
		CCharacterPage		ppCharacter;
		CBackgroundPage		ppBackground;
		psOptions.AddPage(&ppCharacter);
		psOptions.AddPage(&ppBackground);
		return (psOptions.DoModal());
	} else return (psOptions.DoModal());
}


void ShowSay(CUserInfo *pui, const char *szText, CDWordArray *prgdwFormatting, BYTE bbCooked, BYTE byteMode)
{
	if (!pui)
		return;	// haven't made net connection yet!

	// don't show if ignored
	if (pui->Ignored())
		return;

	USHORT ExtractAvatarID(void*);

	pui->m_udi.m_bbCooked = bbCooked;
	pui->m_udi.m_byteMode = byteMode;
	pui->m_udi.m_chExprE = 
	pui->m_udi.m_chGestE = 
	pui->m_udi.m_chExprI = 
	pui->m_udi.m_chGestI = 0;

	if (GetChatDoc()->m_bComicView)
	{
		ASSERT(bbCooked);
		CAvatarX *av = GetAvatar(ExtractAvatarID(pui));
		av->GetIndices(pui->m_udi.m_chExpr, pui->m_udi.m_chGest, pui->m_udi.m_bbReq);
		// for now, take the hit that SayEntry will just set expr, gest, and req again!
		AddAndExecute(new SayEntry(pui, szText, prgdwFormatting ? prgdwFormatting : (CDWordArray*) -1));
	}
	else
	{
		pui->m_udi.m_bbReq = 0;
		AddAndExecute(new SayEntry(pui, szText, prgdwFormatting ? prgdwFormatting : (CDWordArray*) -1));
	}
}

void AcknowledgeInvite(const char *nick, const char *room) {
	CString confirmation;
	confirmation.LoadString(IDS_INVITE_CONF);
	ReplaceToken(confirmation, CString("%1"), nick);
	ReplaceToken(confirmation, CString("%2"), room);
	AfxMessageBox(confirmation);
}

void ConfirmAway() {
	if (theApp.m_bAway && theApp.m_bAwayPrompt) {
		if (AfxMessageBox(IDS_CONFIRMAWAY, MB_YESNO) == IDYES)
			theApp.OnAwayToggle();
		else theApp.m_bAwayPrompt = FALSE;  // they've seen it once
	}
}
