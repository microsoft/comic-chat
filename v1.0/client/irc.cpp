#include "stdafx.h"
#include <afxsock.h>
#include "ircsock.h"
#include "chat.h"

#include "binddoc.h"
#include "chatDoc.h"

								// so we can see if we're on the network or not
#include "userinfo.h"
#include "dib.h"
#include "chatprot.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "setupdlg.h"
#include "ui.h"
#include "memblst.h"
#include "histent.h"
#include <mmsystem.h>
#include "roomlist.h"
#include "admindlg.h"

#define IDP_SOCKETS_INIT_FAILED			104   // should be guaranteed unique and in resource file

extern CChatApp theApp;
extern CUserInfo *puiSelf;
extern UINT MyAvatarID();
extern CSetupDialog setupDlg;
extern const char *GetMyName();
extern const char *GetMyServer();
extern void SetMyName(const char *charName);
extern void ChatSetChannel(const char *chanName);
extern UINT GetMyPort();
extern const char *GetMyRealName();
extern const char *GetMyCharacter();
extern const char *GetMyChannel();
extern void ChatSetServer(const char *);
extern void ChatSetChannel(const char *);
extern void ChatSetPort(UINT);
extern void SetMyCharacter(char *);
extern void AddAndExecute(HistoryEntry *);

void ShowSay( CUserInfo *pui, const char *psz, BOOL cooked, UCHAR mode = SM_SAY);

static CIrcSocket serverConn;
CMapStringToPtr mapNickToPtr(10);
static char outBuff[513];			// enough?
static int	memberCount = 0;
CPtrArray whisperees;

// BOOL bSendPose = FALSE;
static BOOL bSendComicsData = TRUE;
BOOL bCXPrompt = TRUE;
static int iConnected = CX_DISCONNECTED;
static CString strCurrentChannel;

BOOL ToggleSendComicsData() {
	bSendComicsData = ! bSendComicsData;
	return(bSendComicsData);
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

int ChatGetMemberCount() {
	return memberCount;
}

void ChatSetMemberCount(int count) {
	memberCount = count;
	// Post string to StatusBar Pane 2
	CString mesg, strCount;
	if (count == 1) mesg.LoadString(ID_USER_SINGULAR);
	else mesg.LoadString(ID_USER_PLURAL);
	strCount.Format("%d", count);
	VERIFY(ReplaceToken(mesg, CString("%1"), strCount));
	ASSERT(AfxGetApp());
	((CChatApp*)AfxGetApp())->SetStatusPaneString(1, mesg);
}

void TryNewNick(int msg_id, const char *showNick = NULL) {
	CNicknameDlg nickDlg;
	nickDlg.m_label.LoadString(msg_id);
	nickDlg.m_strNickname = showNick ? showNick : GetMyName();
	BOOL gotOK = (nickDlg.DoModal() == IDOK);
	if (nickDlg.m_strNickname.IsEmpty())
		nickDlg.m_strNickname = "Anonymous";

	int cxStatus = ChatGetConnectionStatus();
	if (cxStatus != CX_DISCONNECTED)
		ChatSetNick(nickDlg.m_strNickname);
	if (cxStatus == CX_DISCONNECTED || cxStatus == CX_CONNECTING)
		SetMyName(nickDlg.m_strNickname);  // since won't get a nick message back
}

void ChatSetConnectionStatus(int iStat) inline {
	//Post string to StatusBar, Pane 1
	CString strStatus;

	switch(iStat)
	{
	case CX_DISCONNECTED:
		strStatus.LoadString(ID_DISCONNECTED);
		break;
	case CX_INCHANNEL:
		strStatus.LoadString(ID_CONNECTED);
		VERIFY(ReplaceToken(strStatus, CString("%1"), GetMyChannel()));
		break;
	case CX_CONNECTING:
		strStatus.LoadString(ID_CONNECTING);
		break;
	case CX_NOCHANNEL:
		strStatus.LoadString(ID_NOCHANNEL);
		break;
	}
	ASSERT(AfxGetApp());
	((CChatApp*)AfxGetApp())->SaveConnectStatus(strStatus);
	((CChatApp*)AfxGetApp())->SetStatusPaneString(0,strStatus);
	theApp.m_bNoNetwork = (iStat != CX_INCHANNEL);		// for now (added only for Kick support)
	iConnected = iStat;
}

int ChatGetConnectionStatus() {return iConnected;}

BOOL CommunicationInits() {
	if (!AfxSocketInit()) {
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}
	else return TRUE;
}

void InitializeServerConnection() {
	char *GetComicsTitle();
	void InitializeBackDrops();

	while (TRUE) {
		if (serverConn.m_hSocket != INVALID_SOCKET) {	// first close last connection if necessary
			serverConn.Close();		
		}

		if (*GetMyName() == '\0') TryNewNick(ID_ERR_NO_NICK);

		if (bCXPrompt) {
			int rval = setupDlg.DoModal();
			if (rval == IDCANCEL) {
				theApp.m_bNoNetwork = TRUE;
				ChatSetConnectionStatus(CX_DISCONNECTED);
				AddAndExecute(new StartHistoryEntry(GetComicsTitle(), GetMyCharacter(), 0));
				// Set things up so user can still type in text and
				//  get comics.  Great for debugging too.
				InitializeBackDrops();  // initializes and stores an initial history entr
				AddAndExecute(new JoinEntry(GetMyName()));  // pretend to join to set pui & puiSelf
				return;
			}
		}
		
		bCXPrompt = TRUE;	// only good for one non-prompt (could also be set to false in above DoModal)
		ChatSetConnectionStatus(CX_CONNECTING);
		((CFrameWnd*)AfxGetMainWnd())->UpdateWindow();
		VERIFY(serverConn.Create());
		if (serverConn.Connect(GetMyServer(), GetMyPort()) || GetLastError() == WSAEWOULDBLOCK) {
			theApp.m_bNoNetwork = FALSE;
//			AddAndExecute(new StartHistoryEntry(GetComicsTitle(), GetMyCharacter(), 0));
			// must initialize character (in case never enters channel), for character dialog...
			if (theApp.m_bComicView) SetMyAvatar(GetMyCharacter());
			InitializeBackDrops();  // initializes and stores an initial history entry
			return;
		}
		
		CString mesg;
		ChatSetConnectionStatus(CX_DISCONNECTED);
		mesg.LoadString(ID_ERR_WHERE_SERVER);
		VERIFY(ReplaceToken(mesg, CString("%1"), GetMyServer()));
		AfxMessageBox(mesg);
		setupDlg.nWhatFailed = SERVER;
	}
}



char *ParseMessage(char *message, char **prefix, char **command) {
	static char prefixBuff[100];
	static char commandBuff[20];

	// parse prefix
	char *body;
	if (*message == ':') {		// there's a prefix
		message++;				// don't include the colon
		body = strchr(message, ' ');
		ASSERT(body);			// messages must have a body
		int prefixSize = body - message;
		prefixSize = min(prefixSize, sizeof(prefixBuff)-1);   // BETA1 fix
		strncpy(prefixBuff, message, prefixSize);
		prefixBuff[prefixSize] = '\0';
		body++;					// move beyond space
	} else {
		*prefixBuff = '\0';
		body = message;
	}

	// parse command  (body now points to text after prefix)
	char *args = strpbrk(body, " \r\n");
	ASSERT(args);				// should at least end w/ \r\n if no args
	int commandSize = args - body;
	commandSize = min(commandSize, sizeof(commandBuff)-1);	// BETA1 fix
	strncpy(commandBuff, body, commandSize);
	commandBuff[commandSize] = '\0';

	*prefix = prefixBuff;
	*command = commandBuff;

	while (isspace(*args)) args++;  // return pointer to arguments (strip off whitespace)
	return args;
}

void ParsePrefix(char *prefix, char **snick, char **user, char **machine) {
	static char snickBuff[50], userBuff[50], machBuff[50];

	// initial setup
	snickBuff[0] = userBuff[0] = machBuff[0] = '\0';		// empty by default
	*snick = snickBuff;
	*user = userBuff;
	*machine = machBuff;

	char *nfield = strpbrk(prefix, "!@");					// parse snick (must be present)
	if (nfield) {
		int nchars = nfield - prefix;
		strncpy(snickBuff, prefix, nchars);
		snickBuff[nchars] = '\0';
	} else {
		strcpy(snickBuff, prefix);
		return;
	}

	if (*nfield == '!') {									// parse user
		prefix = nfield+1;
		nfield = strchr(prefix, '@');
		if (nfield) {
			int nchars = nfield - prefix;
			strncpy(userBuff, prefix, nchars);
			userBuff[nchars] = '\0';
		} else {
			strcpy(userBuff, prefix);
			return;
		}
	}

	strcpy(machBuff, nfield+1);								// nfield now pts to !, parse machine
}

// returns the "last string" (characters after the colon).  Assumes prefix has been stripped away
char *GetLastString(char *msg) {
	static char strBuff[513];				// large enough?
	char *colon = strchr(msg, ':');
	ASSERT(colon);							// got to be there, right?
	char *end = _tcspbrk(++colon, "\r\n");
	if (!end) end = strchr(colon, '\0');	// probably never happens w/ irc
	int nchars = end - colon;
	strncpy(strBuff, colon, nchars);
	strBuff[nchars] = '\0';
	return (strBuff);
}

char *GetToken(char *start, char **nextStart) {
	static char buff[100];
	while (*start && (isspace(*start) || strchr(",.)", *start))) start++;
	if (!*start) return NULL;
	char *endPtr = start;
	while (*endPtr && !isspace(*endPtr) && !strchr(",.)", *endPtr)) endPtr++;
	int nchars = endPtr - start;
	nchars = min(nchars, sizeof(buff)-1); // don't overrun buff! // djk - BETA1 fix!
	ASSERT(nchars);
	strncpy(buff, start, nchars);
	buff[nchars] = '\0';
	*nextStart = endPtr;
	return buff;
}

void DestroyUserInfos() {
	void *p;
	CString nick;
	int avID, GetAvatarUpperBound();

	POSITION pos = mapNickToPtr.GetStartPosition();
	int avUpper = GetAvatarUpperBound();
	while (pos) {
		mapNickToPtr.GetNextAssoc(pos, nick, p);
		CUserInfo *pui = (CUserInfo *) p;
		if (avID = pui->GetAvatarID() && avID <= avUpper) { // if avID not 0
			CAvatarX *av = GetAvatar(avID);
			av->m_userInfo = NULL;   // erase avatar pointers to puis
			av->m_nSends = 0;		 // so can be reused
		}
		delete pui;
	}

	mapNickToPtr.RemoveAll();	// reclaim map storage

	puiSelf = NULL;	
}

void ForEachWord(char *line, void (*func)(char *, void *), void *clientData) {
	char *word;
	while (word = GetToken(line, &line)) {
		(*func)(word, clientData);
	}
}

void AddToMembersList(const char *attedNick, CUserInfo *pui) {
	int AddToImageList(CUserInfo* pui);
	void UpdateTitle();

	CMemberList *membList = GetMembers();
	ChatSetMemberCount(ChatGetMemberCount()+1);
	int pos = membList->GetSortPosition(attedNick);
	if (theApp.m_bComicView) {
		int imageIndex = AddToImageList(pui);
		membList->m_MemberListBox.InsertItem(pos, attedNick, imageIndex);
		UpdateTitle();
	} else
		membList->m_MemberListBox.InsertItem(pos, attedNick);
}

void AssignArbitraryAvatar(CUserInfo *pui) {
 	char avatarstr[256];
	GetNextAvatarName(avatarstr);
	CAvatarX *av = GetAvatar3(avatarstr);  // pui just created, so no need to specify as 2nd arg
	pui->SetAvatarID(av->m_avatarID);
	av->m_userInfo = pui;
	TRACE("Mapping %s to %s.\n", pui->GetName(), av->m_name);
}


void CIUserJoin(const char *attedNick) {			// assume for now that all joins represent totally new people
	CUserInfo *pui = new CUserInfo;
	BOOL comicMode = theApp.m_bComicView;
	const char *nickname = attedNick;
	BOOL bAdmin = FALSE;

	if (*attedNick == '@') {
		nickname++;
		bAdmin = TRUE;
	}

	VERIFY(pui->Initialize(0, nickname, strlen(nickname)));
	if (bAdmin) pui->SetOperator(TRUE);

	// see if it's us
	if( !strcmp(nickname, GetMyName())) {
		ASSERT( puiSelf == NULL );
		if (comicMode) {
			pui->SetAvatarID(MyAvatarID());
			CAvatarX *av = MyAvatar();
			av->m_userInfo = pui;
		}
		puiSelf = pui;
		pui->ComicUser(TRUE);		// we know this to be true, since it's us.
		if (bAdmin)
			GetChatDoc()->InsertAdminMenu();

	} else {
		if (comicMode) 
			AssignArbitraryAvatar(pui);
	}
	mapNickToPtr.SetAt(nickname, pui);
	AddToMembersList(attedNick, pui);
}

int AddToImageList(CUserInfo* pui)
{
	// First get the CDib
	CAvatarX* pAv = GetAvatar(pui->GetAvatarID());
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
//	CBitmap ImageBmp;
//	ImageBmp.LoadBitmap(IDB_TESTBMP);
	int imageIndex = GetMembers()->m_ImageList.Add(pImageBmp,pImageBmp);
	return imageIndex;
}

int FindMemberListIndex(CUserInfo *pui) {
	LV_FINDINFO lvFind;
	lvFind.flags = LVFI_STRING | LVFI_WRAP;
	CString attedNick;
	pui->GetAttedNick(attedNick);
	lvFind.psz = attedNick;
	return (GetMembers()->m_MemberListBox.FindItem(&lvFind));
}

int RemoveMemberFromList(CUserInfo* pui) {
	int index = FindMemberListIndex(pui);
	GetMembers()->m_MemberListBox.DeleteItem(index);
	return index;
}


CUserInfo *LookupPui(const char *nickname) inline {
	void *pui;
	if (*nickname == '@') nickname++;			// strip away op sign, if given
	if (mapNickToPtr.Lookup(nickname, pui) == 0) return NULL;
	else return (CUserInfo *)pui;
}


USHORT GetAvatarIDFromNickname(LPCTSTR nickname) {
	return LookupPui(nickname)->GetAvatarID();
}

void SingleJoin(char *attedNick, void *) {
	AddAndExecute(new JoinEntry(attedNick));
}

void CIUserPart(const char *nickname) {			// assume for now that parts represent parts from all channels
	void UpdateTitle();
	//	mapNickToPtr.RemoveKey(nickname);			// keep them there for now (facilitates garbage collection)
	CUserInfo *pui = LookupPui(nickname);
	if (!pui) return;
	pui->SetDeparted(TRUE);

	ChatSetMemberCount(ChatGetMemberCount()-1);

	int index = FindMemberListIndex(pui);
	if (index != LB_ERR) GetMembers()->m_MemberListBox.DeleteItem(index);
	if (theApp.m_bComicView) UpdateTitle();
}

void ChatPartChannel(BOOL hardDisconnect, BOOL removeMembers) {
	if (strCurrentChannel != "") {
		sprintf(outBuff, "PART %s\r\n", strCurrentChannel);  // exit gracefully
		serverConn.Send(outBuff, strlen(outBuff));
		strCurrentChannel = "";
	}

	if (hardDisconnect) {
		if (serverConn.m_hSocket != INVALID_SOCKET) {	// close last connection if necessary
			serverConn.Close();		
		}
		ChatSetConnectionStatus(CX_DISCONNECTED);
	} else ChatSetConnectionStatus(CX_NOCHANNEL);

	if (GetMembers() && removeMembers) {
		GetMembers()->m_MemberListBox.DeleteAllItems();		// empty out member pane
		ChatSetMemberCount(0);
	}
	
	CChatDoc *doc = GetChatDoc();
	if (doc) doc->RemoveAdminMenu();  // So admin menu does not pile up
}

#define APPEARSPREFIX	" Appears as "
#define GETINFOPREFIX	" GetInfo"
#define HERESINFOPREFIX	" HeresInfo: "
#define RINGPREFIX		" RING"

void ChatAnnounceNewAvatar(const char *avName, const char *addressee) {
	if (bSendComicsData) {
		if (!addressee) addressee = GetMyChannel();
		sprintf(outBuff, "PRIVMSG %s :#%s%s.\r\n",
				addressee, APPEARSPREFIX, avName);
		serverConn.Send(outBuff, strlen(outBuff));
	}
}

void ProcessComment(CUserInfo *pui, char *mesg, char *rest) {					// BETA1 - adds rest arg
	ASSERT(*mesg == '#');			// should have already been checked
	mesg++;							// nuke the crosshatch
	int match = !strncmp(mesg, APPEARSPREFIX, strlen(APPEARSPREFIX));
	if (match) {
		char *var = mesg + strlen(APPEARSPREFIX);
		char *charName = GetToken(var, &var);
		if (!charName) return;					// djk - BETA1 Fix
		ASSERT(pui);
		pui->ComicUser(TRUE);
		AddAndExecute(new ChangeAvatarEntry(pui, charName));
		return;
	}
	match = !strncmp(mesg, GETINFOPREFIX, strlen(GETINFOPREFIX));
	if (match) {
		CString profStr;
		BOOL GetProfileString(CString &);

		TRACE("You've been probed!\n");
		GetProfileString(profStr);
		sprintf(outBuff, "PRIVMSG %s :#%s%s\r\n", pui->GetName(), HERESINFOPREFIX,
			    profStr);
		serverConn.Send(outBuff, strlen(outBuff));
		return;
	}
	match = !strncmp(mesg, HERESINFOPREFIX, strlen(HERESINFOPREFIX));
	if (match) {
		// Beta1 fix: verify that information was actually requested!
		if (pui->IsRequestInfo()) {
			char *strToEnd = mesg+strlen(HERESINFOPREFIX);
			AddAndExecute(new GetInfoEntry(pui, strToEnd));
			pui->SetRequestInfo(FALSE);
		}
		return;
	}
#if 0
	match = !strncmp(mesg, RINGPREFIX, strlen(RINGPREFIX));
	if (match) {  // BETA1 - this block checks to make sure ring not sent to entire channel
		char *firstPound = strchr(rest, '#');
		char *firstColon = strchr(rest, ':');
		if (!(firstPound && firstColon && firstPound < firstColon)) { // i.e., ring was not sent to entire channel
			void ChatRingReceived(CUserInfo *);
			ChatRingReceived(pui);
		}
		return;
	}
#endif
}

BYTE IndexToByte(BYTE in) {
	return (in + '0');
}

BYTE ByteToIndex(BYTE in) {
	return (in - '0');
}

#define GESTUREPREFIX		'G'
#define EXPRESSIONPREFIX	'E'
#define REQUESTEDPREFIX		'R'
#define MODEPREFIX			'M'
#define TALKTOPREFIX		'T'

void GetTalkTos(CWordArray &talkTos, char *str) {
	while (TRUE) {
		while (isspace(*str)) str++;
		if (*str == ')' || *str == '\0') return;
		char *name = GetToken(str, &str);
		if (!name) return;					// -djk BETA1 fix
		CUserInfo *pui = LookupPui(name);
		if (pui) talkTos.Add(pui->GetAvatarID());
	}
}

static char actionID[] = {0x01, 'A', 'C', 'T', 'I', 'O', 'N'};
#define ACTIONLENGTH	7

void PrepareAction(CUserInfo *pui, char *mesg, UCHAR &mode) {
	CString newMesg;
	newMesg = pui->GetName();
	newMesg += (mesg + ACTIONLENGTH);
	strcpy(mesg, newMesg);
	char *endOne = _tcschr(mesg, 0x01);
	if (endOne) *endOne = '\0';
	mode = SM_ACTION;
}

static BOOL foundMe;
void CheckForSelf(char *name, void *myName) {
	if (!stricmp(name, (const char *) myName)) foundMe = TRUE;
}

// Check to see if it's directed especially at us
void IdentifyWhispers(char *rest, UCHAR &mode, CWordArray &talkTos) {
	char *colon = strchr(rest, ':');
	if (!colon) return;		// no colon?  Bad protocol message!
	*colon = '\0';			// temporarily so we can search w/ ForEachWord
	foundMe = FALSE;
	ForEachWord(rest, CheckForSelf, (void *) GetMyName());
	if (foundMe) {
		mode = SM_WHISPER;
		if (theApp.m_bComicView)
			talkTos.Add(puiSelf->GetAvatarID());
	}
	*colon = ':';
}

void ProcessSay(CUserInfo *pui, char *mesg, char *rest) {
	// parse off initial parenthetical info
	int gest = -1, expr = -1, req = 0, seems_cooked = 0;
	int gestE = 0, gestI = 0, exprE = 0, exprI = 0;
	UCHAR mode = SM_SAY;
	CWordArray talkTos;
	if (!strncmp(mesg, "(#", 2)) {
		char *start = mesg + 2;
		if (*start == GESTUREPREFIX) {
			start++;
			if (*start) gest = ByteToIndex(*start++);
			if (*start) gestE = ByteToIndex(*start++);
			if (*start) gestI = ByteToIndex(*start++);
		}
		if (*start == EXPRESSIONPREFIX) {
			start++;
			if (*start) expr = ByteToIndex(*start++);
			if (*start) exprE = ByteToIndex(*start++);
			if (*start) exprI = ByteToIndex(*start++);
		}
		if (*start == REQUESTEDPREFIX) {
			start++;
			req = 1;
		}
		if (*start == MODEPREFIX) {
			start++;
			if (*start) mode = ByteToIndex(*start++);
			if (mode < SM_SAY || mode > SM_ACTION || mode == SM_SHOUT)
				mode = SM_SAY;   // ensure mode in band!  // djk - BETA1 fix!
		}
		if (*start == TALKTOPREFIX) {
			start++;
			GetTalkTos(talkTos, start);
		}

		start = strstr(start, ") ");
		if (start && gestI != -1 && exprI != -1) {
			seems_cooked = TRUE;
			mesg = start + 2;	// advance string to end of parenthetical annotation
		}
	}

	if (strncmp(mesg, actionID, ACTIONLENGTH) == 0) PrepareAction(pui, mesg, mode);
	if (!seems_cooked) IdentifyWhispers(rest, mode, talkTos);

	if (!(pui->Ignored()) && strlen(mesg) > 0)
		AddAndExecute(new SayEntry(pui, mesg, seems_cooked, mode, expr, gest,
								   exprE, exprI, gestE, gestI, req, talkTos));
}

void ProcessKick(char *kicker, char *args) {
	char chanbuff[90];
	char *channel = GetToken(args, &args);
	if (!channel) return;			// malformed message - djk BETA1 fix
	strcpy(chanbuff, channel);
	char *kickee = GetToken(args, &args);
	if (!kickee) return;			// malformed message - djk BETA1 fix
	char *mesg = GetLastString(args);
	TRACE("%s kicked %s off channel %s with message (%s).\n", kicker, kickee, channel, mesg);
	BOOL meKicked = !stricmp(kickee, GetMyName());
	CUserInfo *kickeePui = LookupPui(kickee);
	ASSERT(kickeePui);
	CUserInfo *kickerPui = LookupPui(kicker);
	ASSERT(kickerPui);
//	CAvatarX *kickerAv = GetAvatar(kickerPui->GetAvatarID());
	CWordArray talkTos;
	talkTos.Add(kickeePui->GetAvatarID());
	CString boxMsg;
	if (*mesg) boxMsg.LoadString(ID_KICK_MESG);
	else boxMsg.LoadString(ID_KICK_NO_MESG);
	VERIFY(ReplaceToken(boxMsg, CString("%1"), kicker));
	VERIFY(ReplaceToken(boxMsg, CString("%2"), kickee));
	if (*mesg) VERIFY(ReplaceToken(boxMsg, CString("%3"), mesg));
	AddAndExecute(new SayEntry(kickerPui, boxMsg, FALSE, SM_ACTION, 0, 0, 0, 0, 0, 0, TRUE, talkTos));
	AddAndExecute(new PartEntry(kickee));
	if (meKicked) {
		AfxMessageBox(boxMsg);
		ChatPartChannel(TRUE);
	}
}

void ProcessWhoIsChannels(char *args) {
	// assume for now that we can only do one whois at a time,
	// hence nick (arg1) is as expected
	CStringArray channels;
	char *rooms = GetLastString(args);
	while (TRUE) {
		char *channel = GetToken(rooms, &rooms);
		if (!channel || !*channel) break;
		if (*channel == '@') channel++;  // ignore admin indicator
		channels.Add(channel);
	}
	// now do a list only on these channels...
	int upper = channels.GetUpperBound();
	if (upper < 0) return;				// not a member of a single channel
	CString command = "LIST ";
	for (int i = 0; i <= upper; i++) {
		command += channels[i];
		if (i != upper) command += ",";
	}
	command += "\r\n";
	serverConn.Send(command, command.GetLength());
}

void PreMemberNameChange(CUserInfo *pui, LV_ITEM &item) {
	int index = FindMemberListIndex(pui);			// remove old entry from member list...
	item.mask = LVIF_IMAGE | LVIF_STATE;		// but first get some info on it...
	item.iItem = index;
	item.iSubItem = 0;
	VERIFY(GetMembers()->m_MemberListBox.GetItem(&item));
	GetMembers()->m_MemberListBox.DeleteItem(index);
}

void PostMemberNameChange(CUserInfo *pui, LV_ITEM &item) {
	CString attedNick;							// insert modified entry back into member list
	pui->GetAttedNick(attedNick);  // now atted nick of new name
	char *UnConst(const char *);
	item.mask |= LVIF_TEXT;
	item.pszText = UnConst(attedNick);
	item.iItem = GetMembers()->GetSortPosition(attedNick);
	GetMembers()->m_MemberListBox.InsertItem(&item);
}

void ReinstallPui(CUserInfo *pui, const char *newNick) {
	if (!pui) return;		// should never happen, but may if history list not initialized :-(
	mapNickToPtr.RemoveKey(pui->GetName());
	if (*newNick == '@') newNick++;			// remove the at sign if there is one
	pui->SetName(newNick);
	if (pui == puiSelf) SetMyName(newNick);
	mapNickToPtr.SetAt(newNick, pui);
}
	
void ProcessNick(CUserInfo *pui, const char *newNick, BOOL updateMemberList) {
	if (!pui) return;
	if (stricmp(pui->GetName(), newNick)) {		// ie, if they are different
		LV_ITEM item;
		TRACE("Changing old nick (%s) to %s.\n", pui->GetName(), newNick);
		// remove from memberlist under old nickname...
		if (updateMemberList) {
			PreMemberNameChange(pui, item);
		}

		// refile PUI
		ReinstallPui(pui, newNick);

		// add to memberlist under new nickname ...
		if (updateMemberList) {
			PostMemberNameChange(pui, item);

			// might as well update title too...
			void UpdateTitle();
			if (theApp.m_bComicView) UpdateTitle();
		}
	}
}

// much of this code similar to that in ProcessNick.  Combine?
void ChatChangeAdmin(const char *nick, BOOL makeAdmin) {
	CUserInfo *pui = LookupPui(nick);
	if (!pui) return;
	BOOL isOp = pui->IsOperator();
	if ((makeAdmin && isOp) || (!makeAdmin && !isOp))  // can't do equality test, since TRUE != 1 in all cases
		return;										 // nothing to change

	LV_ITEM item;
	PreMemberNameChange(pui, item);
	pui->SetOperator(makeAdmin);
	PostMemberNameChange(pui, item);

	if (pui == puiSelf) {
		CChatDoc *doc = GetChatDoc();			    // update admin menus appropriately
		if (makeAdmin) doc->InsertAdminMenu();
		else doc->RemoveAdminMenu();
	}
}

void ProcessMode(char *args) {
	if (!GetToken(args, &args)) return;  // channel name
	char switch_cmd[4], *switch_tmp = GetToken(args, &args);
	strncpy(switch_cmd, switch_tmp, sizeof(switch_cmd)-1);   // make a temporary copy
	if (switch_cmd && strlen(switch_cmd) == 2 && switch_cmd[1] == 'o') {
		const char *nick = GetToken(args, &args);
		ChatChangeAdmin(nick, switch_cmd[0] == '+');
	}
}


void CIrcSocket::ProcessMessage(char *line) {
	char *prefix, *command, *nick, *user, *machine;
	CString mesg;

	char *rest = ParseMessage(line, &prefix, &command);

	if (!strcmp(command, "PING")) {  // ignore sender for now
		sprintf(outBuff, "PONG %s\r\n", "djk2");
		TRACE("%s", outBuff);
		Send(outBuff, strlen(outBuff));
	}
	else if (!strcmp(command, "PRIVMSG")) {
		if (!puiSelf) return; // Don't process privmsgs out of channel
		ParsePrefix(prefix, &nick, &user, &machine);
		TRACE("Got a PrivMsg! (snick = %s, mach = %s, user = %s)\n", nick, machine, user);
		char *mesg = GetLastString(line+1);  // eventually use args.  Move past prefix colon
		void *p;
		if (!mapNickToPtr.Lookup(nick, p)) {   // should be there
			AddAndExecute(new JoinEntry(nick));
			mapNickToPtr.Lookup(nick, p);
			TRACE("Person not registered, yet speaks %s.\n", nick);
		}
		CUserInfo *pui = (CUserInfo *) p;
		if (*mesg != '#') ProcessSay(pui, mesg, rest);
		else ProcessComment(pui, mesg, rest);     // BETA1 fix -- adds rest arg
	}
	else if (!strcmp(command, "JOIN")) {
		TRACE("Got a JOIN!\n");
		ParsePrefix(prefix, &nick, &user, &machine);
		if (!(stricmp(nick, GetMyName()) == 0)) {				// Don't send or register self
			AddAndExecute(new JoinEntry(nick));				 // ignore joins of self
			ChatAnnounceNewAvatar(GetMyCharacter(), nick);  // Now send avatar info privately
		}
	}
	else if (!strcmp(command, "PART")) {
		TRACE("Got a PART!\n");
		ParsePrefix(prefix, &nick, &user, &machine);
		AddAndExecute(new PartEntry(nick));
	}
	else if (!strcmp(command, "QUIT")) {	// collapse w/ PART?
		TRACE("Got a QUIT!\n");
		ParsePrefix(prefix, &nick, &user, &machine);
		AddAndExecute(new PartEntry(nick));
	}
	else if (!strcmp(command, "353")) {     // Message saying who's already there. Register them.
		TRACE("Got a list of participants.\n");
		char *GetComicsTitle();
		void InitializeBackDrops();
		AddAndExecute(new StartHistoryEntry(GetComicsTitle(), GetMyCharacter(), 0));
		InitializeBackDrops();
		ChatSetConnectionStatus(CX_INCHANNEL);     // we are officially in a channel
		strCurrentChannel = GetMyChannel();		   // should be up-to-date now...
		ParsePrefix(prefix, &nick, &user, &machine);
		// assume for now it's a message for the one and only channel
		char *mesg = GetLastString(line+1);
		ForEachWord(mesg, SingleJoin, NULL);

		// Store the chanel name as the title...
		// drop the "#" and use channel name for doc title and file name
		const char *psz = strCurrentChannel;
		if ((*psz) && psz[1])	{ // handle str lengths of 0 and 1
			CChatDoc *doc = GetChatDoc();
			// note: doc is null if we're closing up IE3.0 w/ Connect dialog open
			if (doc) doc->SetPathName(psz+1); 
		}
	}
	else if (!strcmp(command, "NICK")) {		// change nickname
		ParsePrefix(prefix, &nick, &user, &machine);
		char *newNick = GetLastString(line+1);  // eventually use args.  Move past prefix colon
		if (puiSelf)  // i.e., we're recording a history
			AddAndExecute(new NickEntry(nick, newNick));
		else SetMyName(newNick);
	}
	else if (!strcmp(command, "433")) {		// Nickname already in use response
		char *badNick = GetToken(rest, &rest);
		TryNewNick(ID_ERR_DUPED_NICK, badNick);
	}
	else if (!strcmp(command, "431") || !strcmp(command, "432")) {  // no or bad nickname
		char *badNick = GetToken(rest, &rest);
		TryNewNick(ID_ERR_BAD_NICK, badNick);
	}
	else if (!strcmp(command, "001")) {		// We're logged in!  Join channel or show room list
		void OnLogin();
		OnLogin();
	}
	else if (!strcmp(command, "403")) {  // bad chat room name
		mesg.LoadString(ID_ERR_BAD_CHANNEL);
		VERIFY(ReplaceToken(mesg, CString("%1"), GetMyChannel()));
		AfxMessageBox(mesg);
		void ChatSwitchChannel();
		ChatSwitchChannel();
	}
	else if (!strcmp(command, "KICK")) {		// someone was kicked
		ParsePrefix(prefix, &nick, &user, &machine);
		ProcessKick(nick, rest);
	}
	else if (!strcmp(command, "322")) {
		void AddToRoomList(char *);
		AddToRoomList(rest);
	}
	else if (!strcmp(command, "319")) {
		ProcessWhoIsChannels(rest);
	}
	else if (!strcmp(command, "321")) {
		void StartRoomList();
		StartRoomList();
	}
	else if (!strcmp(command, "323")) {
		void EndRoomList();
		EndRoomList();
	}
	else if (!strcmp(command, "MODE")) {
		ProcessMode(rest);
	}
	else if (!strcmp(command, "471")) {
		CString mesg;
		mesg.LoadString(ID_ERR_CHANNELISFULL);
		VERIFY(ReplaceToken(mesg, CString("%1"), GetMyChannel()));
		AfxMessageBox(mesg);
	}
	else if (!strcmp(command, "473")) {
		CString mesg;
		mesg.LoadString(ID_ERR_INVITEONLY);
		VERIFY(ReplaceToken(mesg, CString("%1"), GetMyChannel()));
		AfxMessageBox(mesg);
	}
	else if (!strcmp(command, "474")) {
		CString mesg;
		mesg.LoadString(ID_ERR_BANNEDFROMCHAN);
		VERIFY(ReplaceToken(mesg, CString("%1"), GetMyChannel()));
		AfxMessageBox(mesg);
	}
	else if (!strcmp(command, "ERROR")) {
		char *mesg = GetLastString(line+1); // move beyond prefix colon
		AfxMessageBox(mesg);				// print the message verbatim (localization problem!!!)
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
	}
}

#define RECVSIZE	513

void CIrcSocket::OnReceive(int nErrorCode) {
	TRACE("Entering OnReceive (code = %d).\n", nErrorCode);
	static char recvBuff[RECVSIZE];

	if (!nErrorCode) {
		char *startPtr = strchr(recvBuff, '\0');
		int space = recvBuff + RECVSIZE - startPtr - 1;
		int nRead = Receive(startPtr, space);
		startPtr[nRead] = '\0';
		char *eoc = strchr(recvBuff, '\n');
		while (eoc) {
			eoc++;
			char message[RECVSIZE];
			int comLen = eoc - recvBuff;
			strncpy(message, recvBuff, comLen);
			message[comLen] = '\0';

			// now move rest of message forward
			char *eob = strchr(recvBuff, '\0');
			int nRest = eob - eoc;
			strncpy(recvBuff, eoc, nRest);
			recvBuff[nRest] = '\0';

			TRACE("Got message: %.100s\n", message);
			ProcessMessage(message);   // handle the message (*After clearing it from the buffer!!!)
			eoc = strchr(recvBuff, '\n'); // must do this after process message, since code is reentrant (but single threaded)
		}
	}
	TRACE("Leaving OnReceive.\n");
}


void CIrcSocket::OnConnect(int nErrorCode) {
	char buff[255];

	TRACE("Connecting (code = %d)...\n", nErrorCode);
	if (nErrorCode) {  // couldn't connect
		CString mesg;
		mesg.LoadString(ID_ERR_CONNECT);
		setupDlg.nWhatFailed = PORT;
		CString portNum;
		portNum.Format("%d", GetMyPort());
		VERIFY(ReplaceToken(mesg, CString("%1"), portNum));
		VERIFY(ReplaceToken(mesg, CString("%2"), GetMyServer()));
		AfxMessageBox(mesg);
		InitializeServerConnection();
		return;
	}
	// got a connection!
	sprintf(buff, "NICK %s\r\n", GetMyName());
	Send(buff, strlen(buff));
	CString strRealName = GetMyRealName();
	if(strRealName.IsEmpty())
		strRealName = "Anonymous";
	char user[60], machine[100];
	unsigned long nChars = sizeof(user);
	if (!GetUserName(user, &nChars) || nChars <= 1) strcpy(user, "NoUser");
	if (gethostname(machine, sizeof(machine))) strcpy(machine, "NoMachine");
	sprintf(buff, "USER %s %s %s :%s\r\n", user, machine, "myServer", strRealName);
	Send(buff, strlen(buff));
}

void CIrcSocket::OnClose(int nErrorCode) {
	TRACE("Closing socket on error %d.\n", nErrorCode);
	ChatSetConnectionStatus(CX_DISCONNECTED);
}

BOOL ChatTerminate() { return TRUE; }
BOOL ChatIdle() { return TRUE; }

void EmotionToBytes(CEmotion &em, BYTE &emotion, BYTE &intensity);

static BOOL InsertAnnotations(char *buff, UCHAR mode)
{
	void GetAddressees(CString &);
	UCHAR faceIndex, torsoIndex, requested, faceEmotion, faceIntensity, torsoEmotion, torsoIntensity;
	CEmotion face, torso;
    CAvatarX *av = MyAvatar();

	if(av)
	{
		av->GetIndices(faceIndex, torsoIndex, requested);
		av->GetEmotions(face, torso);
		BYTE faceIndexByte = IndexToByte(faceIndex);
		BYTE torsoIndexByte = IndexToByte(torsoIndex);
		BYTE modeByte = IndexToByte(mode);
		EmotionToBytes(face, faceEmotion, faceIntensity);
		EmotionToBytes(torso, torsoEmotion, torsoIntensity);

		sprintf(buff, "(#%c%c%c%c%c%c%c%c%s%c%c",
				GESTUREPREFIX, torsoIndexByte, torsoEmotion, torsoIntensity,
				EXPRESSIONPREFIX, faceIndexByte, faceEmotion, faceIntensity,
				requested ? "R" : "",
				MODEPREFIX, modeByte);
		if (av->m_talkTo.GetUpperBound() >= 0) {
			CString str="T";
			GetAddressees(str);
			strcat(buff, str);
		}

		strcat(buff, ") ");
	}
	return TRUE;
}

void ProcessNonComicsMsg(CString &str, UCHAR &mode) {
	if (mode == SM_THINK) {
		CString prefix;
		prefix.LoadString(ID_THINK_PREFIX);
		VERIFY(ReplaceToken(prefix, CString("%1"), ""));
		prefix.TrimLeft();
		str = prefix + str;
		mode = SM_ACTION;   // send it as an action
	}
	if (mode == SM_ACTION) {
		CString prefix = actionID;
		prefix += " ";
		str = prefix + str;
		str += "\001";
	}
}


BOOL ChatSendText(CString& str, UCHAR mode)
{
	void GetAddressees2(CString &);

	if (theApp.m_bComicView && MyAvatar() == NULL) return FALSE;		// just make sure...

	// strip off \n?
	str.TrimRight();
	CString myStr = str;
	UCHAR myMode = mode;

	if (myMode == SM_ACTION) {			// save copies of the action w/ name prepended
		CString prefix = GetMyName();
		prefix += " ";
		myStr = prefix + myStr;
	}

	if(ChatGetConnectionStatus() == CX_INCHANNEL) {  // only do this if there is a room to send to
		char annotations[100] = "";
		if (theApp.m_bComicView && bSendComicsData)
			InsertAnnotations(annotations, mode);
		CString addressee;
		if (mode != SM_WHISPER) addressee = GetMyChannel();
		else GetAddressees2(addressee);
		if ((!bSendComicsData || !theApp.m_bComicView) && (mode == SM_ACTION || mode == SM_THINK))
			ProcessNonComicsMsg(str, mode);
		if (bSendComicsData && theApp.m_bComicView && mode == SM_ACTION)
			str = myStr;   // if sending a cooked action, make sure to prepend the name first
		sprintf(outBuff, "PRIVMSG %s :%s%s\r\n", addressee, annotations, str);
		serverConn.Send(outBuff, strlen(outBuff));
	}

	ShowSay(puiSelf, myStr, theApp.m_bComicView, myMode);		// don't receive PRIVMSGs sent by self, so do explicit show
	return TRUE;
}

void ChatGetInfo (CUserInfo *pui) {
	// send message requesting info directly to target

	sprintf(outBuff, "PRIVMSG %s :#%s\r\n", pui->GetName(), GETINFOPREFIX);
	serverConn.Send(outBuff, strlen(outBuff));
	// BETA1 fix: set a bit indicating that we requested this info,
	// so people can't  nefariously send us info that we don't want to show.
	pui->SetRequestInfo(TRUE);
}

#if 0
void ChatRingUser (CUserInfo *pui) {
	sprintf(outBuff, "PRIVMSG %s :#%s\r\n", pui->GetName(), RINGPREFIX);
	serverConn.Send(outBuff, strlen(outBuff));
}


void ChatRingReceived (CUserInfo *pui) {
	// Should add ring notification to status bar or panel or something.  TBD -djk
	if (!pui->Ignored()) {
		MessageBeep(MB_ICONEXCLAMATION);
//		BOOL ok = PlaySound("ChatRing", NULL, SND_ALIAS);
		CString strRing;
		strRing.LoadString(ID_RING_MESG);
		VERIFY(ReplaceToken(strRing, CString("%1"), pui->GetName()));
//		ASSERT(AfxGetApp());
//		((CChatApp*)AfxGetApp())->SetStatusPaneString(1,strRing);
//		AfxMessageBox(strRing);
		GetMembers()->MessageBox(strRing, "Microsoft Comic Chat");
	}
}

#endif

void GetAddressees(CString &s) {
	CAvatarX *av = MyAvatar();
	int upperbound = av->m_talkTo.GetUpperBound();
	upperbound = min(upperbound, 4);    // clip at first 4 (so don't overrun output buff)
	for (int i = 0; i <= upperbound; i++) {
		CAvatarX *addressee = GetAvatar((UINT) av->m_talkTo[i]);
		const char *avName, *nickname;
		addressee->GetAvatarName(&avName, &nickname);
		s += nickname;
		if (i != upperbound) s += ",";
	}
}

void GetAddressees2(CString &s) {
	int upperbound = whisperees.GetUpperBound();
	for (int i = 0; i <= upperbound; i++) {
		CUserInfo *pui = (CUserInfo *) whisperees[i];
		s += pui->GetName();
		if (i != upperbound) s += ",";
	}
}

void ChatFillRoomList (CRoomList *rl) {
	CString users, command;
	rl->m_user.GetWindowText(users);
	users.TrimLeft();
	if (users.GetLength() == 0) {
		command = "LIST\r\n";
	} else {
		command.Format("WHOIS %s\r\n", users);
	}

	serverConn.Send(command, command.GetLength());
}

void StartRoomList() {
	CRoomList *rl = GetRoomList();
	if (rl) {
		rl->ClearRoomList();		// prepare for pending insertions
		rl->m_reset.EnableWindow(FALSE);
		// MakeEmpty already done in OnReset, but this prevents duplicates for multiple button presses
		rl->m_persist->MakeEmpty();
	}
}

void EndRoomList() {					// no-op for now
	CRoomList *rl = GetRoomList();
	if (rl) {
		rl->SortRooms(TRUE);
		rl->AnnounceCount();
		rl->m_reset.EnableWindow(TRUE);
	}
}

void AddToRoomList(char *line) {
	CRoomList *rl = GetRoomList();
	if (rl) {
		CRoomListPersist *pl = rl->m_persist;
		CRoom *room = new CRoom;
		GetToken(line, &line);	// read off nickname first
		char *token = GetToken(line, &line);
		if (token) room->m_name = token;
		token = GetToken(line, &line);
		if (token) room->m_nUsers = atoi(token);
		token = GetLastString(line);
		if (token) room->m_descr = token;
		int roomIndex = rl->m_persist->AddRoom(room);
		rl->AddToRoomList(roomIndex);
//		if (roomIndex % 50 == 49)  // show count regularly (helpful for large rooms)
		rl->AnnounceCount();
	}
}

void ChatKickUser(CUserInfo *pui) {
	CKickDialog kickDlg;
	CString strName = pui->GetName();
	CString strDlg;
	strDlg.LoadString(IDS_KICKREASON);
	VERIFY(ReplaceToken(strDlg, CString("%1"), strName));
	kickDlg.m_strKick = strDlg;
	//	kickDlg.m_Kick.SetWindowText(strDlg);
	if (kickDlg.DoModal() == IDOK) {
		CString command;
		command.Format("KICK %s %s :%s\r\n", GetMyChannel(), pui->GetName(),
						kickDlg.m_reason);
		serverConn.Send(command, command.GetLength());
	}
}

void ChatSetTopic() {
	CTopicDlg topicDlg;
	if (topicDlg.DoModal() == IDOK) {
		CString command;
		command.Format("TOPIC %s :%s\r\n", GetMyChannel(), topicDlg.m_topic);
		serverConn.Send(command, command.GetLength());
	}
}

// starting with an empty memberlist, repopulates it, according to our map's info.
void RepopulateMemberList() {
	void *p;
	CString attedNick, nick;
	extern CUserInfo *puiSelf;

	ChatSetMemberCount(0);				// AddToMembers will increment it
	POSITION pos = mapNickToPtr.GetStartPosition();
	while (pos) {
		mapNickToPtr.GetNextAssoc(pos, nick, p);
		CUserInfo *pui = (CUserInfo *) p;
		if (!pui->IsDeparted()) {
			pui->GetAttedNick(attedNick);
			AddToMembersList(attedNick, pui);
		}
	}
}

void MapNullAvatars() {
	void *p;
	CString nick;
	int GetAvatarUpperBound();

	POSITION pos = mapNickToPtr.GetStartPosition();
	int avUpper = GetAvatarUpperBound();
	while (pos) {
		mapNickToPtr.GetNextAssoc(pos, nick, p);
		CUserInfo *pui = (CUserInfo *) p;
		if (!pui->GetAvatarID()) { // null avID
			AssignArbitraryAvatar(pui);
		}
	}
}

void ChatSetNick(const char *nick) {
	int online = ChatGetConnectionStatus();
	CString oldNick = GetMyName();
	if (online != CX_DISCONNECTED) {
		if (stricmp(nick, oldNick) || online == CX_CONNECTING) {
			// we have to do this anyway if we're connecting and the names haven't changed,
			// since if we have a bad nick, we'll need to get back a bad nick response.
			// Otherwise, we know we don't have a bad nick, w/ other connect statuses.
			sprintf(outBuff, "NICK %s\r\n", nick);
			serverConn.Send(outBuff, strlen(outBuff));
		}
	} else 
		if (stricmp(nick, oldNick))
			AddAndExecute(new NickEntry(oldNick, nick));  // do it right away (won't get a nick message back)
}

void ChatJoinChannel() {
	sprintf(outBuff, "JOIN %s\r\n", GetMyChannel());
	serverConn.Send(outBuff, strlen(outBuff));
}


void OnLogin() {
	BOOL RequestedChannelList(BOOL);

	ChatSetConnectionStatus(CX_NOCHANNEL);

	if (RequestedChannelList(FALSE) || *GetMyChannel() == '\0') {
		GetChatDoc()->OnChatroomList();
	} else
		ChatJoinChannel();
}


BOOL bCXKeepServer = FALSE;
void InitializeChannelConnection() {
	bCXKeepServer = FALSE;						// reset to default value, since we've used it up
	if (bCXPrompt) {
		CChannelDlg dlg;
		dlg.DoModal();
		ChatSetChannel(dlg.m_strChannel);
	}
	bCXPrompt = TRUE;							// only go without prompts once!

	// must initialize character (in case never enters channel), for character dialog...
	if (theApp.m_bComicView) SetMyAvatar(GetMyCharacter());

	ChatJoinChannel();
}

void ChatSwitchChannel() {
	CChannelDlg dlg;
	int rval = dlg.DoModal();
	if (rval == IDOK) {
		bCXKeepServer = TRUE;
		bCXPrompt = FALSE;
		ChatSetChannel(dlg.m_strChannel);
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
	}
}

void ChatSetOperator(CUserInfo *pui, BOOL makeOp) {
	char switcher = makeOp ? '+' : '-';
	sprintf(outBuff, "MODE %s %co %s\r\n", GetMyChannel(), switcher, pui->GetName());
	serverConn.Send(outBuff, strlen(outBuff));
}
