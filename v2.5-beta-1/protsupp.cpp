#include "stdafx.h"
#include "chat.h"
#include "binddoc.h"
#include "chatDoc.h"
#include "ircproto.h"
#include "textview.h"
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
#include "mainfrm.h"
#include "chatview.h"
#include "status.h"
#include "actions.h"
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

#include "sounddlg.h"

extern CChatApp theApp;
extern UINT MyAvatarID();
//extern CSetupPage setupDlg;
extern const char *GetMyName();
extern const char *GetMyScreenName();
extern const char *GetMyServer();
extern void SetMyName(const char *charName);
extern void SetMyNameNick(const char *nick);
extern const char *GetMyRealName();
extern const char *GetMyCharacter();
extern const char *GetMyChannel();
extern const char *GetMyEmail();
extern const char *GetMyHomePage();
extern void ChatSetChannel(const char *);
extern void SetMyCharacter(char *);
BOOL RequestedChannelList(BOOL);
//extern DWORD WINAPI ConvertINetString(DWORD dwSrcEncoding, DWORD dwDstEncoding, LPCSTR lpSrcStr, LPINT lpnSrcSize, LPSTR lpDstStr, LPINT lpnDstSize);
BOOL bNMInstalled();
extern CPtrList	g_docs;
extern BOOL bFindAndPlaySound(const char *, BOOL, BOOL);
extern void GetMyServerPrettyName(CString& str);

CNCSMapStringToPtr*	g_mapNickToPtr;
CPtrArray			g_rgpuiWhisperees;
static CPtrList		externalPuis;

static BOOL		g_bSendComicsData = TRUE;
SHORT			g_nCXKeepServer = 0;
BOOL			g_bCXPrompt = TRUE;
BOOL			g_bFreezeTabs = FALSE;
int				g_iViewMode = VM_UNSPECIFIED;

// static CString	g_strRequestedNick;  Not used
static BOOL		g_bInEnumeration = FALSE;

CRoomInfo		g_enterInfo;
CRoomInfo		*currentRoom;
BOOL			g_bEnterOnCreate = FALSE;

static char 	g_szLastBackdropName[512];

BOOL ToggleSendComicsData() {
	g_bSendComicsData = ! g_bSendComicsData;
	// note: following line should really alert world, not just current room!
	if (g_bSendComicsData && GetChatDoc() && GetChatDoc()->m_proto)
		GetChatDoc()->m_proto->ChatAnnounceNewAvatar(GetMyCharacter(), MyAvatarURL (), NULL);  // announce new av to world
	return g_bSendComicsData;
}

void SetSendComicsData(BOOL bSendComicsData) {
	g_bSendComicsData = bSendComicsData;
}

BOOL GetSendComicsData() {     // does not rewrite ini
	return g_bSendComicsData;
}

void ChatSetCXPrompt(BOOL bPrompt) {
	g_bCXPrompt = bPrompt;
}

BOOL GetCXPrompt() {
	return g_bCXPrompt;
}


#ifdef IRCLOG
void StartFileInLoop() {
	char szBuff[1000];

	while (fgets(szBuff, sizeof(szBuff), theApp.m_fileIn) != NULL) {
		if (*szBuff != '\n')
			serverConn.ProcessMessage(szBuff);
	}

	fclose(theApp.m_fileIn);
	theApp.m_fileIn = NULL;
}
#endif IRCLOG


BOOL ChatInitialize(SHORT *pnCXKeepServer, BOOL *pbCXPrompt)
{
	ASSERT(pnCXKeepServer);
	ASSERT(pbCXPrompt);
	ASSERT(*pnCXKeepServer >= 0);

	g_szLastBackdropName[0] = '\0';

#ifdef IRCLOG
	if (theApp.m_fileIn) {
		StartFileInLoop();
		return TRUE;
	}
#endif

	if (*pnCXKeepServer == 0)
		InitializeServerConnection(&g_enterInfo, pbCXPrompt);

	if (*pnCXKeepServer > 0)
		(*pnCXKeepServer)--;
	*pbCXPrompt = TRUE;

	return TRUE;
}

void AdjustViewMode() {
	CChatDoc *doc = GetChatDoc();
	if (!doc) return;
	if (g_iViewMode == VM_TEXT || !theApp.m_bFoundArt)
		doc->OnViewText();
	else if (g_iViewMode == VM_COMICS)
		doc->OnViewComics();

	g_iViewMode = VM_UNSPECIFIED;
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


void CIrcProto::TryNewNick(int msg_id, const char *showNick, BOOL registerNick, CString *newNick)
{
	CNicknameDlg nickDlg;
	nickDlg.m_label.LoadString(msg_id);
	nickDlg.m_strNickname = showNick ? showNick : GetMyName();
	// Spaces only allowed on IRCX servers.
	nickDlg.m_bSpacesAllowed = IsIRCX ();
	if (theApp.DoModalDlg(&nickDlg) == IDCANCEL) {
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
		m_pSock->m_iConnected = iStat;
		break;
	case CX_INCHANNEL:
	{
		CString str;
		GetMyServerPrettyName (str);
		m_pSock->m_iConnected = CX_CONNECTED;
		m_bInRoom = TRUE;
		leftMesg.LoadString(ID_CONNECTED);
		VERIFY(ReplaceToken(leftMesg, CString("%1"), m_strPrettyChannel));
		VERIFY(ReplaceToken(leftMesg, CString("%2"), str));
		((CChatDoc *)m_doc)->SaveConnectStatus(leftMesg);
		break;
	}
	case CX_CONNECTING:
		m_pSock->m_iConnected = iStat;
		break;
	case CX_NOCHANNEL:
		m_pSock->m_iConnected = CX_CONNECTED;
		m_bInRoom = FALSE;
		break;
	}
	UpdateStatus();
}


int CIrcProto::GetConnectionStatus()
{
	if (m_pSock->m_iConnected == CX_DISCONNECTED || m_pSock->m_iConnected == CX_CONNECTING)
		return m_pSock->m_iConnected;
	else
		if (m_bInRoom)
			return CX_INCHANNEL;
		else
			return CX_NOCHANNEL;
}


// GetToken, the new version, now allows different opening and closing separators.
char *GetToken2(char *szStart, char **pszNextStart, const char *szSepsBegin, const char *szSepsEnd, char **pszCurStart /* = NULL */)
{
	static char sszBuff[MAX_TOKEN];

	while (*szStart && (my_isspace(*szStart) || strchr(szSepsBegin, *szStart)))
		szStart++;

	if (pszCurStart)
		*pszCurStart = szStart;
	
	if (!*szStart)
		return NULL;

	char *szEndPtr = szStart;

	while (*szEndPtr && !my_isspace(*szEndPtr) && !strchr(szSepsEnd, *szEndPtr))
		szEndPtr++;

	int nChars = szEndPtr - szStart;

	nChars = min(nChars, sizeof(sszBuff)-1); // don't overrun buff!
	ASSERT(nChars);
	strncpy(sszBuff, szStart, nChars);
	sszBuff[nChars] = '\0';
	*pszNextStart = szEndPtr;
	return sszBuff;
}


char *GetToken1(char *szStart, char **pszNextStart, const char *szSeps, char **pszCurStart /* = NULL */, BOOL bSkipInitialSeps /* = TRUE */)
{
	static char sszBuff[MAX_TOKEN];

	while (*szStart && (my_isspace(*szStart) || (bSkipInitialSeps && strchr(szSeps, *szStart))))
		szStart++;

	if (pszCurStart)
		*pszCurStart = szStart;

	if (!*szStart)
		return NULL;

	char *szEndPtr = szStart;

	if (!bSkipInitialSeps && strchr(szSeps, *szEndPtr))
		szEndPtr++;

	while (*szEndPtr && !strchr(szSeps, *szEndPtr))
		szEndPtr++;
	
	int nChars = szEndPtr - szStart;

	nChars = min(nChars, sizeof(sszBuff)-1); // don't overrun buff!
	ASSERT(nChars);
	strncpy(sszBuff, szStart, nChars);
	sszBuff[nChars] = '\0';
	*pszNextStart = szEndPtr;
	return sszBuff;
}

char* GetToken(char *szStart, char **pszNextStart, const char *szSeps /* = ",.)" */, char **pszCurStart /* = NULL */)
{
	return GetToken2(szStart, pszNextStart, szSeps, szSeps, pszCurStart);
}


BOOL ReplaceToken(CString& str, const CString& strToken, const CString& strValue)
{
	CString strResult;
	int iIndex;

	// find the token in the string
	iIndex = str.Find(strToken);
	if( iIndex < 0 )
		return FALSE;

	// grab the first part
	strResult = str.Left(iIndex);

	// add the replacement
	strResult += strValue;

	// add the second part
	strResult += str.Mid(iIndex + strToken.GetLength());

	// put back in the right place
	str = strResult;

	return TRUE;
}


void ChatEmptyMemberList(CChatDoc *doc) {
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

	if (doc->m_puiSelf == g_puiSelf) g_puiSelf = NULL;	// set global to be null if necessary (window current)
	doc->m_puiSelf = NULL;	
}

void DestroyExternalUserInfos() {
	POSITION pos = externalPuis.GetHeadPosition();	// now externals...
	while (pos) {
		CUserInfo *pui = (CUserInfo *) externalPuis.GetNext(pos);
		delete pui;
	}
}


BOOL bForEachWord(char *szLine, BOOL (*pfn)(char *, void *, DWORD), void *pvClientData, DWORD dwClientData, char *szSep, BOOL bDoubleQuotes /*=FALSE*/)
{
	BOOL	bRet = FALSE;
	char	*szWord;
	char	szSepTmp[32];	// should be big enough

	if (bDoubleQuotes)
	{
		strcpy(szSepTmp, szSep);
		strcat(szSepTmp, "\"");
	}

	while (TRUE)
	{
		if (bDoubleQuotes && *szLine == '\"')
		{
			szWord = GetToken1(szLine, &szLine, szSepTmp, NULL, FALSE /*bSkipInitialSeps*/);
			if (*szLine == '\"')	// skip the terminating double quote
			{
				szLine++;
				strcat(szWord, "\"");
			}
		}
		else
			szWord = GetToken(szLine, &szLine, szSep);
		if (!szWord)
			break;
		bRet |= (*pfn)(szWord, pvClientData, dwClientData);
	}

	return bRet;
}


void AddToMembersList(CUserInfo *pui, CChatDoc *doc) {
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

	if (!g_bInEnumeration)
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
 	char szAvatarStr[_MAX_FNAME];
	GetNextAvatarName(szAvatarStr);
	CAvatarX *pAv = GetAvatar3(szAvatarStr);  // pui just created, so no need to specify as 2nd arg
	pui->SetAvatarID(pAv->m_avatarID);
	pAv->m_userInfo = pui;
	TRACE("Mapping %s to %s.\n", pui->GetScreenName(), pAv->m_name);
}


CUserInfo *LookupPui(const char *szNickname, CChatDoc *doc) inline
{
	void *pui;

	if (STATUS_CHAR(*szNickname))
		szNickname++;	// strip away status sign, if given

	if (!doc)
	{
		doc = GetChatDoc();
		if (!doc)
			return NULL;
	}

	if (doc->m_mapNickToPtr.Lookup(szNickname, pui) == 0)
		return NULL;
	else
		return (CUserInfo*) pui;
}


CUserInfo *ExternalPui(const char *szNickname, const char *szFullName, BOOL bAddIfNotThere)
{
	ASSERT(szNickname);

	CString		strFullName;
	CUserInfo*	pui;
	POSITION	pos = externalPuis.GetHeadPosition();
	while (pos)
	{
		pui = (CUserInfo*) externalPuis.GetNext(pos);
		strFullName = pui->GetFullName();

		// REGISB, 05/11/98:  added "|| strFullName.IsEmpty() || !*szFullName" because of bug #2540
		if (stricmp(szNickname, pui->GetName()) == 0 && 
			(
			 !szFullName ||
			 !*szFullName ||
			 strFullName.IsEmpty() ||
			 0 == stricmp(szFullName, strFullName)
			)
		   )
		{
			if (strFullName.IsEmpty() && szFullName && *szFullName)
				pui->SetFullName(szFullName);
			return pui;
		}
	}

	// REGISB, 05/11/98: note that RÉGIS and régis in UTF8 (alt0201 versus alt0233)
	// are two different users for our IRCX servers - don't treat them as identical users!!

	if (bAddIfNotThere)
	{
		pui = new CUserInfo(szNickname, szFullName);
		pui->SetExternal(TRUE);
		externalPuis.AddHead(pui);
		return pui;
	}
	else
		return NULL;
}



USHORT GetAvatarIDFromNickname(LPCTSTR szNickname) {
	return LookupPui(szNickname)->GetAvatarID();
}


CUserInfo *CIUserJoin(CUserInfo *pui)
{	// assume for now that all joins represent totally new people
	CChatDoc *doc = GetChatDoc();
	BOOL comicMode = doc->m_bComicView;
	const char *szNickname = pui->GetName();

	// see if it's us
	if (pui->IsSelf())
	{
		ASSERT(g_puiSelf == NULL);
		if (comicMode)
		{
			pui->SetAvatarID(MyAvatarID());
			CAvatarX *av = MyAvatar();
			av->m_userInfo = pui;
//			doc->m_proto->ChatAnnounceNewAvatar(GetMyCharacter(), MyAvatarURL ());
		}
		g_puiSelf = pui;
		doc->m_puiSelf = /*(void *)*/ pui;
		pui->ComicUser(TRUE);		// we know this to be true, since it's us.
		doc->UpdateAdminMenu();
	}
	else
	{
		if (comicMode) 
			AssignArbitraryAvatar(pui);
	}
	if ((doc->m_proto->m_dwModes & CM_MODERATED) && !pui->CheckFlag(UF_HASVOICE) && !pui->IsOperator())
		pui->SetFlag(UF_SPECTATOR, TRUE);  // really should be in pui creation but don't have doc ref there
	CUserInfo *pui3;
	if (pui3 = LookupPui(szNickname))
		TRACE("####### GOT ANOTHER INSTANCE OF %s (%x)\n", szNickname, pui3);
	g_mapNickToPtr->SetAt(szNickname, pui);
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
		CDIB * pDIB = pAv->GetIconPose()->GetDrawing ();
		if (pDIB == NULL) {
			return -1;
		}

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

int FindMemberListIndex(CUserInfo *pui, CChatDoc *doc) {
	LV_FINDINFO lvFind;
	lvFind.flags = LVFI_PARAM | LVFI_WRAP;
	lvFind.lParam = (long)pui;
	CMemberList *memb = doc ? (CMemberList *) doc->m_memberList : GetMembers();
	ASSERT(memb);
	return (memb->m_MemberListBox.FindItem(&lvFind));
}

int RemoveMemberFromList(CUserInfo* pui) {
	int index = FindMemberListIndex(pui);
	ASSERT(GetMembers());
	GetMembers()->m_MemberListBox.DeleteItem(index);
	return index;
}


void CIUserPart(const char *szNickname)
{	// assume for now that parts represent parts from all channels
	void UpdateTitle(CChatDoc * = NULL);
	//	g_mapNickToPtr.RemoveKey(szNickname);			// keep them there for now (facilitates garbage collection)
	CUserInfo *pui = LookupPui(szNickname);
	if (!pui)
		return;
	pui->SetDeparted(TRUE);

//	ChatSetMemberCount(ChatGetMemberCount()-1);

	int index = FindMemberListIndex(pui);
	if (index != LB_ERR)
	{
		ASSERT(GetMembers());
		GetMembers()->m_MemberListBox.DeleteItem(index);
	}

	if (GetChatDoc()->m_bComicView)
		UpdateTitle();
	GetChatDoc()->ResetStatus(FALSE, TRUE);
	TRACE("%s has left the building\n", szNickname);
}


void CopyPtrArrayToCWDordArray(CPtrArray &source, CDWordArray &dest) inline {
	int nItems = source.GetUpperBound() + 1;
	dest.SetSize(nItems);
	for (int i = 0; i < nItems; i++) dest[i] = (DWORD) source[i];
}


void CopyArray(CDWordArray &source, CDWordArray &dest) inline {
	int nItems = source.GetUpperBound() + 1;
	dest.SetSize(nItems);
	for (int i = 0; i < nItems; i++) dest[i] = source[i];
}


// if pui is NULL, it uses the single selected member in the doc (if possible)
BOOL ExpandVariables(CString &strMessage, CChatDoc *doc, CUserInfo *pui, BOOL bInvokedByRule)
{
	CString strVar;
	if (!strVar.LoadString(IDS_USERVARIABLE))
		return FALSE;

	if (!doc)
		doc = GetChatDoc();
	if (!doc)
		return FALSE;

	if (strstr(strMessage, strVar))
	{
		if (!pui)
		{
			if (bInvokedByRule)
				return FALSE;
			pui = doc->GetSingleSelectedMember();
		}
		if (!pui)
			return FALSE;  // substitution failed
		const char *szScreenName = pui->GetScreenName();
		if (!strstr(szScreenName, strVar))
			while (ReplaceToken(strMessage, strVar, szScreenName));
	}

	if (!strVar.LoadString(IDS_ROOMVARIABLE))
		return FALSE;
	if (!strstr(doc->m_proto->m_strPrettyChannel, strVar))
		while (ReplaceToken(strMessage, strVar, doc->m_proto->m_strPrettyChannel));

	return TRUE;
}


BOOL bReplaceMacroTokens(CString &strMessage, BOOL bIn)
{
	CString strBy;
	if (!strBy.LoadString(IDS_USERVARIABLE))
		return FALSE;

	if (bIn)
	{
		if (!strstr(strBy, szUserToken))
			while (ReplaceToken(strMessage, szUserToken, strBy));
	}
	else
	{
		if (!strstr(szUserToken, strBy))
			while (ReplaceToken(strMessage, strBy, szUserToken));
	}

	if (!strBy.LoadString(IDS_ROOMVARIABLE))
		return FALSE;

	if (bIn)
	{
		if (!strstr(strBy, szRoomToken))
			while (ReplaceToken(strMessage, szRoomToken, strBy));
	}
	else
	{
		if (!strstr(szRoomToken, strBy))
			while (ReplaceToken(strMessage, strBy, szRoomToken));
	}

	return TRUE;
}


void AutoGreet(const char* szNick)
{
	if (theApp.m_iGreetingType && g_puiSelf && g_puiSelf->IsOperator())
	{
		char		*szControlFull, *szControlLess;
		CDWordArray	rgdwFormatting;
		CUserInfo	*pui = LookupPui(szNick);
		if (!pui)
			return;

		// fill out message string variables
		CString strControlFull = theApp.m_strGreetingMesg;

		if (!ExpandVariables(strControlFull, GetChatDoc(), pui))   // XXX GetChatDoc() always accurate???
			return;

		g_puiSelf->m_udi.m_talkTos.RemoveAll();
		g_puiSelf->m_udi.m_talkTos.Add((DWORD) pui);
		
		if (!(szControlFull = strdup((LPCTSTR) strControlFull)))
			return;
		szControlLess = SzControlLess(szControlFull, &rgdwFormatting);

		// send the message
		switch (theApp.m_iGreetingType)
		{
		case AGT_SAY:
			bChatSendText(CString(szControlLess), BM_SAY, TRUE, &rgdwFormatting);
			break;
		case AGT_WHISPER:
			g_rgpuiWhisperees.RemoveAll();
			g_rgpuiWhisperees.Add(pui);
			bChatSendText(CString(szControlLess), BM_WHISPER, TRUE, &rgdwFormatting);
			break;
		}

		free(szControlFull);
	}
}


void 
CRoomInfo::ChatAnnounceNewAvatar(
const char *szAvName, 
const char *szURL,
const char *szAddressee, 
BOOL bForce)
{
	if (GetConnectionStatus() != CX_INCHANNEL)
		return;

	// REGISB 04/16: added IsIRCX because we use still use DATA even when g_bSendComicsData is FALSE
	if (IsIRCX() || g_bSendComicsData || bForce)
	{
		if (!szAvName || !*szAvName)
			szAvName = "NONE";		// make sure we send an Avatar Name
		if (szURL && *szURL != '\0')
			sprintf(GetOutBuff(), "#%s%s.%s", APPEARSPREFIX, szAvName, szURL);
		else
			sprintf(GetOutBuff(), "#%s%s", APPEARSPREFIX, szAvName);

		if (szAddressee) TRACE("Announcing self to %s\n", szAddressee);
		if (!szAddressee)
			bChatSendToChannel(GetOutBuff() /*szAnnotations*/, NULL /*szMesg*/, NULL /*szNMText*/); // initially we send a spurious announcement prior to connection
		else
			bChatSendPrivMesg(szAddressee, GetOutBuff() /*szAnnotations*/, NULL /*szText*/);
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
		szCharName = strdup (szCharName); // Copy it.
		if (!szCharName)
			return FALSE;			// djk - BETA1 Fix
		char *szCharURL = GetToken2(szVar, &szVar, ".,)", ",)");
		if (proto)					// REGISB added 11/17/97 Fix 4503
		{
			ASSERT(pui);
			if (!pui->Ignored() && !pui->IsFlooding())	// REGISB 11/18/97: we don't accept "# Appears As" of ignored users - is this OK??
			{
				if (!pui->IsComicUser())
				{
					if (msgType & MT_CHANNELSEND) {
						// Don't send URL in this reply-type announcement; just
						// send an extra '?' if we do have a URL - this way, the
						// other client will request the URL when they try to download.
						proto->ChatAnnounceNewAvatar(GetMyCharacter(), 
							MyAvatarURL () != NULL ? DEFERRED_URL_STRING : NULL,
							pui->GetName(), TRUE /*bForce*/);  // Now send avatar info privately
					}
					pui->ComicUser(TRUE);
				}
				// If we were waiting to get the URL for download purposes, 
				// do that directly.
				if (pui->NeedsDownload () && pui->GetAvatarRealName () &&
						!lstrcmpi (szCharName, pui->GetAvatarRealName ())) {
					// Avoid infinite looping.
					if (szCharURL[0] != '\0' && 
							(szCharURL[0] != DEFERRED_URL_CHAR || szCharURL[1] != '\0')) {
						SetUserAvatarRealInfo (pui, szCharName, szCharURL, doc);
					}
					else {
						pui->SetFlag (UF_AUTODOWNLOAD | UF_INTERACTIVEDOWNLOAD, FALSE);
					}
				}
				else {
					AddAndExecute(new ChangeAvatarEntry(pui, szCharName, szCharURL), doc);
				}
			}
		}
		free (szCharName);
		return TRUE;
	}

	iMatch = !strncmp(szMesg, GETINFOPREFIX, strlen(GETINFOPREFIX));
	if (iMatch)
	{
		if (proto)					// REGISB added 11/17/97 Fix 4503
		{
			ASSERT(pui);
			if (!pui->Ignored() && !pui->IsFlooding())
			{
				CString strProfile;

				TRACE("You've been probed by %s!\n", pui->GetScreenName());

				if (theApp.m_myProfile.IsEmpty())
					strProfile.LoadString(ID_DEFAULT_PROFILE);	// no profile
				else
					strProfile = theApp.m_myProfile;

				sprintf(GetOutBuff(), "#%s%s", HERESINFOPREFIX, strProfile);
				VERIFY(proto->bChatSendPrivMesg(pui->GetName(), NULL /*szAnnotations*/, GetOutBuff() /*szMesg*/, NULL /*szNMText*/, FALSE /*bAsNotice*/, BM_HERESINFO /*uModes*/));
			}
		}
		return TRUE;
	}

	iMatch = !strncmp(szMesg, REQUESTCHARPREFIX, g_nGetCharLen);
	if (iMatch) {

		if (proto) {
			ASSERT(pui);
			if (!pui->Ignored() && !pui->IsFlooding())
			{
				// Send the URL with this one.
				proto->ChatAnnounceNewAvatar(GetMyCharacter(), MyAvatarURL (),
					pui->GetName(), TRUE /*bForce*/);
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
				pui->IsFlooding();	// Count this unexpected message
		}
		return TRUE;
	}
	
	iMatch = !strncmp(szMesg, BACKGRNDPREFIX, strlen(BACKGRNDPREFIX));
	if (iMatch)
	{
		if (proto)					// REGISB added 11/17/97 Fix 4503
		{
			ASSERT(pui);
			if (!pui->Ignored() && !pui->IsFlooding())
			{
				if (pui->IsOperator())
				{
					char *strToEnd = szMesg+strlen(BACKGRNDPREFIX);
					while (isspace(*strToEnd))
						strToEnd++;
					if (*strToEnd && lstrcmpi (g_szLastBackdropName, strToEnd))
						AddAndExecute(new ChangeBackDropEntry((const char*) strToEnd, NULL), doc);
				}
			}
		}
		return TRUE;
	}

	// Support for the new background prefix. This is needed because the 
	// old background command is not very forward-compatible; it doesn't
	// do proper delimiter handling.
	iMatch = !strncmp(szMesg, NEWBACKGRNDPREFIX, strlen(NEWBACKGRNDPREFIX));
	if (iMatch)
	{
		if (proto)
		{
			ASSERT(pui);
			if (!pui->Ignored() && !pui->IsFlooding())
			{
				if (pui->IsOperator())
				{
					char *szVar = szMesg + strlen(NEWBACKGRNDPREFIX);
					char *szBackdropName = GetToken (szVar, &szVar, ",");
					szBackdropName = szBackdropName ? strdup (szBackdropName) : NULL; // Copy it.
					if (!szBackdropName)
						return FALSE;			// djk - BETA1 Fix
					if (*szBackdropName)
					{
						lstrcpy (g_szLastBackdropName, szBackdropName);
						LPSTR pszDot = (LPSTR)OurMbsChr (g_szLastBackdropName, '.');
						if (pszDot)
							*pszDot = '\0';
						char *szBackdropURL = GetToken2(szVar, &szVar, ",", ",)");
						AddAndExecute (new ChangeBackDropEntry (szBackdropName, szBackdropURL), doc);
					}
					free (szBackdropName);
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


USHORT SM2BM(BYTE byteMode)
{
	switch (byteMode)
	{
	case SM_WHISPER:
		return BM_WHISPER;
	case SM_THINK:
		return BM_THINK;
	case SM_ACTION:
		return BM_ACTION;
	default:
		return BM_SAY;
	}
}


BYTE BM2SM(USHORT uModes)
{
	if ((uModes & BM_ACTION) || (uModes & BM_SOUND))
		return SM_ACTION;

	if (uModes & BM_WHISPER)
		return SM_WHISPER;

	if (uModes & BM_THINK)
		return SM_THINK;

	return SM_SAY;
}


void GetTalkTos(CChatDoc *doc, CUserInfo *talkerPui, char *str)
{
	// REGISB: 11/13/97 new m_udi in this function
	talkerPui->m_udi.m_talkTos.RemoveAll();
	while (TRUE)
	{
		while (isspace(*str))
			str++;
		if (*str == ')' || *str == '\0')
			return;
		char *szName = GetToken(str, &str);
		if (!szName)
			return;
		CUserInfo *pui = LookupPui(szName, doc);
		if (pui)
			talkerPui->m_udi.m_talkTos.Add((DWORD) pui);
	}
}


void GetTalkTos(CChatDoc *doc, CDWordArray *talkTos, char *str)
{
	while (TRUE)
	{
		while (isspace(*str))
			str++;
		if (*str == '\0')
			return;
		char *szName = GetToken(str, &str, ",");
		if (!szName)
			return;
		CUserInfo *pui = LookupPui(szName, doc);
		if (pui)
			talkTos->Add((DWORD) pui);
	}
}


char *PrepareTextAction(CUserInfo *pui, char *szMesg, CString &strNewMesg, USHORT &uModes)
{
	strNewMesg = pui->GetScreenName();
	strNewMesg += (szMesg + g_nActionLen);
	int iEndIndex = strNewMesg.Find(0x01);
	if (iEndIndex >= 0)
		strNewMesg = strNewMesg.Left(iEndIndex);
	uModes &= ~BM_SAY;
	uModes |= BM_ACTION;
	return (UnConst(strNewMesg));
}


char *PrepareComicsAction(CUserInfo *pui, char *szMesg, CString &strNewMesg)
{
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
	VERIFY(bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE));
}

void CRoomInfo::ReplyPing(CUserInfo *pui, CString strMesg) {
	sprintf(GetOutBuff(), "%c%.*s %s%c", 0x01, g_nPingLen - 1, pingID+1, strMesg, 0x1);
	VERIFY(bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE));
}

void CRoomInfo::ReplyTime(CUserInfo *pui) {
	char buff1[50], buff2[50];
	GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, buff1, sizeof(buff1)); 
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, buff2, sizeof(buff2));
	sprintf(GetOutBuff(), "%c%.*s %s, %s%c", 0x01, g_nTimeLen -1, timeID+1, buff1, buff2, 0x1);
	VERIFY(bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE));
}

void CRoomInfo::ReplyEmail(CUserInfo *pui) {
	sprintf(GetOutBuff(), "%c%.*s %s%c", 0x01, g_nEmailLen - 1, emailID+1, GetMyEmail(), 0x1);
	VERIFY(bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE));
}

void CRoomInfo::ReplyHomePage(CUserInfo *pui) {
	sprintf(GetOutBuff(), "%c%.*s %s%c", 0x01, g_nUrlLen - 1, urlID+1, GetMyHomePage(), 0x1);
	VERIFY(bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE));
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

void ShowPing(CUserInfo *pui, const char *szMesg) {
	if (pui->CheckFlag(UF_REQUESTPING)) {
		time_t oldTime, newTime;
		if (sscanf(szMesg, " %ld", &oldTime) != 1) return;
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

void ShowTime(CUserInfo *pui, CString strMesg) {
	if (pui->IsRequestInfo(RF_TIME)) {
		CString displayMsg, seconds;
		displayMsg.LoadString(IDS_LOCALTIME_MESSAGE);
		VERIFY(ReplaceToken(displayMsg, CString("%1"), pui->GetScreenName()));
		int end = strMesg.Find((char)0x01);
		if (end >= 0) strMesg = strMesg.Left(end);
		VERIFY(ReplaceToken(displayMsg, CString("%2"), strMesg));
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
			CString strMesg;
			if (strnicmp(strAddr, "REFUSED", 7) == 0) strMesg.LoadString(IDS_NMCALL_REFUSED);
			else if (strnicmp(strAddr, "NOHAVE", 6) == 0) strMesg.LoadString(IDS_NMCALL_NOHAVE);
			else ASSERT(0);	// unexpected strAddr
			VERIFY(ReplaceToken(strMesg, CString("%1"), pui->GetScreenName()));
			AfxMessageBox(strMesg);
			pui->DecrementRequestInfo(RF_NETMEETING);
			return;
		}
	}

	// initiate the NetMeeting Connection
	const char *response = NULL;
	if (!theApp.m_bAcceptNMCalls || !bCanViewUnrated()) response = "REFUSED";
	else if (!bNMInstalled()) response = "NOHAVE";
	if (response)
	{
		sprintf(GetOutBuff(), "%.*s %s%c", g_nNetMeetLen, netMeetingID, response, 0x01);
		VERIFY(bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE));
	}
	else
	{
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
		VERIFY(bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE));
	}
}


char* PrepareSound(CUserInfo *pui, char *szMesg, CString &strNewMesg, USHORT &uModes)
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

	if (theApp.m_bPlaySounds)
		bFindAndPlaySound(szFile, FALSE, FALSE);	// ... and don't stop currently playing sounds

	if (*szEnd == '"')
		szEnd++;			// now end must be at space or end
	strNewMesg = pui->GetScreenName();
	strNewMesg += szEnd;
	int iEndIndex = strNewMesg.Find(0x01);
	if (iEndIndex >= 0)
		strNewMesg = strNewMesg.Left(iEndIndex);

	if (nResettingSequence(szEnd, szResetSeq))
		strNewMesg += CString(szResetSeq);

	strNewMesg += " (";
	strNewMesg += szFile;
	strNewMesg += ")";

	if (bNeedFree)
		free ((void *) szFile);
	
	uModes &= ~BM_SAY;
	uModes |= BM_ACTION;

	return UnConst(strNewMesg);
}

static BOOL foundMe;
void CheckForSelf(char *name, void *myName) {
	if (!stricmp(name, (const char *) myName)) foundMe = TRUE;
}

// Check to see if it's directed especially at us
void IdentifyWhispers(CChatDoc *doc, CUserInfo *pui, BYTE msgType, USHORT &uModes, CDWordArray *talkTos) {
	if (msgType & MT_PRIVATEMSG)
	{
		uModes &= ~BM_SAY;
		uModes |= BM_WHISPER;
		// REGISB: 11/13/97 new m_udi in this function
		pui->m_udi.m_talkTos.RemoveAll();
		// if talkTos specified use it, otherwise only add self
		if (talkTos) {
			int upper = talkTos->GetUpperBound();
			for (int i = 0; i <= upper; i++)
				pui->m_udi.m_talkTos.Add(talkTos->GetAt(i));
		}
		else {
			CUserInfo *pUIMe = doc ? doc->m_puiSelf : ExternalPui(GetMyNickName(), "", TRUE);
			ASSERT(pUIMe);
			pui->m_udi.m_talkTos.Add((DWORD) pUIMe);
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

	if (*szTmp == CGESTUREPREFIX)
	{
		szTmp++;
		if (*szTmp) pui->m_udi.m_chGest  = ByteToIndex(*szTmp++);
		if (*szTmp) pui->m_udi.m_chGestE = ByteToIndex(*szTmp++);
		if (*szTmp) pui->m_udi.m_chGestI = ByteToIndex(*szTmp++);
	}
	
	if (*szTmp == CEXPRESSIONPREFIX)
	{
		szTmp++;
		if (*szTmp) pui->m_udi.m_chExpr  = ByteToIndex(*szTmp++);
		if (*szTmp) pui->m_udi.m_chExprE = ByteToIndex(*szTmp++);
		if (*szTmp) pui->m_udi.m_chExprI = ByteToIndex(*szTmp++);
	}
	
	if (*szTmp == CREQUESTEDPREFIX)
	{
		szTmp++;
		pui->m_udi.m_bbReq = 1;
	}
	
	if (*szTmp == CMODEPREFIX)
	{
		szTmp++;
		if (*szTmp)
			pui->m_udi.m_uModes = SM2BM(ByteToIndex(*szTmp++));
	}
	
	if (*szTmp == CTALKTOPREFIX)
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

	if ((msgType & MT_PRIVATEMSG) && !AcceptWhispers())
		goto exitCheckFlood;

	if (theApp.m_bVIPMode && !pui->IsOperator())
	{
		pui->m_bbValidUDI = 0;
		goto exitCheckFlood;   // VIP's ignore messages from riffraff
	}

	// Low Level Unquoting for \r \n
	bLowLevelUnquoting(g_chLLQuoteCTCP, TRUE /*bTreatAsByteArray*/, szMesg, szMesg);

	// parse off initial parenthetical info
	if (!strncmp(szMesg, "(#", 2) && strstr(szMesg+2, ") ")) {
		char *szStart = szMesg + 2;
		if (*szStart == CGESTUREPREFIX) {
			szStart++;
			if (*szStart) pui->m_udi.m_chGest  = ByteToIndex(*szStart++);
			if (*szStart) pui->m_udi.m_chGestE = ByteToIndex(*szStart++);
			if (*szStart) pui->m_udi.m_chGestI = ByteToIndex(*szStart++);
		}
		if (*szStart == CEXPRESSIONPREFIX) {
			szStart++;
			if (*szStart) pui->m_udi.m_chExpr  = ByteToIndex(*szStart++);
			if (*szStart) pui->m_udi.m_chExprE = ByteToIndex(*szStart++);
			if (*szStart) pui->m_udi.m_chExprI = ByteToIndex(*szStart++);
		}
		if (*szStart == CREQUESTEDPREFIX) {
			szStart++;
			pui->m_udi.m_bbReq = 1;
		}
		if (*szStart == CMODEPREFIX) {
			szStart++;
			if (*szStart)
				pui->m_udi.m_uModes = SM2BM(ByteToIndex(*szStart++));
			if (msgType & MT_PRIVATEMSG)
			{
				pui->m_udi.m_uModes &= ~(BM_SAY|BM_THINK);	// anti-hacker line
				pui->m_udi.m_uModes |= BM_WHISPER;
			}
		}

		pui->m_udi.m_talkTos.RemoveAll();
		if (*szStart == CTALKTOPREFIX)
		{
			CChatDoc*	pDocTmp = doc;
			CUserInfo*	puiTmp = PuiFromDocNickIdent(&pDocTmp, (LPTSTR) (LPCTSTR) pui->GetName(), pui->GetFullName(), FALSE /*bSkipObscuredChannels*/, FALSE /*bAddExternalIfNotThere*/);

			szStart++;
			GetTalkTos(pDocTmp, pui, szStart);
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
		{
			pui->m_udi.Reset();
			if (msgType & MT_PRIVATEMSG)
			{
				pui->m_udi.m_uModes &= ~BM_SAY;
				pui->m_udi.m_uModes |= BM_WHISPER;
			}
		}

	pui->m_bbValidUDI = 0;	// pui->m_udi no more valid for next incoming text

	if (pui->m_udi.m_uModes & BM_ACTION)
	{
		if (!pui->Ignored() && !pui->IsFlooding())
		{
			if (strncmp(szMesg, actionID, g_nActionLen) == 0)
				szMesg = PrepareTextAction(pui, szMesg, strActionMesg, pui->m_udi.m_uModes);
			else
				szMesg = PrepareComicsAction(pui, szMesg, strActionMesg);
			bFloodChecked = TRUE;
		}
		else
			return;
	}
	else if (strncmp(szMesg, actionID, g_nActionLen) == 0)
	{
		if (!pui->Ignored() && !pui->IsFlooding())
		{
			szMesg = PrepareTextAction(pui, szMesg, strActionMesg, pui->m_udi.m_uModes);
			bFloodChecked = TRUE;
		}
		else
			return;
	}
	else if (strncmp(szMesg, soundID, g_nSoundLen) == 0)
	{
		if (!pui->Ignored() && !pui->IsFlooding())
		{
			szMesg = PrepareSound(pui, szMesg, strActionMesg, pui->m_udi.m_uModes);
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
			if (!(msgType & MT_NOTICE) && !pui->Ignored() && !pui->IsFlooding() && proto)
				// REGISB: we only respond if in a room. Should change this limitation.
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
		char *szOffset = szMesg + g_nPingLen;
		if (msgType & MT_PRVMSG)
		{
			if (!pui->Ignored() && !pui->IsFlooding() && proto)
			{
				// REGISB: we only respond if in a room. Should change this limitation.
				short cbLen = strlen(szOffset);
				if (szOffset[cbLen-1] == 0x01)
					szOffset[cbLen-1] = '\0';
				proto->ReplyPing(pui, szOffset+1);
			}
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
			if (!(msgType & MT_NOTICE) && !pui->Ignored() && !pui->IsFlooding() && proto)
				// REGISB: we only respond if in a room. Should change this limitation.
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
		
		if (!pui->Ignored() && !pui->IsFlooding())
			if (my_isspace(*szOffset))
				ChatReceiveFile (pui, szOffset+1);
		return;
	}
	else if (strnicmp(szMesg, emailID, g_nEmailLen) == 0)
	{
		const char *szOffset = szMesg + g_nEmailLen;
		if (*szOffset == 0x01)
		{
			if (!(msgType & MT_NOTICE) && !pui->Ignored() && !pui->IsFlooding() && proto)
				// REGISB: we only respond if in a room. Should change this limitation.
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
			if (!pui->Ignored() && !pui->IsFlooding() && proto)
				// REGISB: we only respond if in a room. Should change this limitation.
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
			if (!pui->Ignored() && !pui->IsFlooding() && proto)
				// REGISB: we only respond if in a room. Should change this limitation.
				proto->DoNetMeetingCX(pui, szOffset+1);
		}
		else
			goto exitCheckFlood;
		return;
	}
	else if (strnicmp(szMesg, awayID, g_nAwayLen) == 0)
	{
		const char *szOffset = szMesg + g_nAwayLen + 1;
		if (!pui->Ignored() && !pui->IsFlooding())
			ShowAway(pui, szOffset, doc);
		else
		{
			// at least update the pui's flags (UF_AWAY)
			CString	strAwayMsg = szOffset;
			int		iEnd = strAwayMsg.Find((char) 0x01);

			if (iEnd >= 0)
				strAwayMsg = strAwayMsg.Left(iEnd);	// get rid of closing 0x01

			pui->SetFlag(UF_AWAY, !strAwayMsg.IsEmpty());
		}
		return;
	}
	else if (strnicmp(szMesg, clientInfoID, g_nClientInfoLen) == 0)
	{
		const char *szOffset = szMesg + g_nClientInfoLen;
		if (*szOffset == 0x01)
		{
			if (!pui->Ignored() && !pui->IsFlooding() && proto)
			{
				// REGISB: we only respond if in a room. Should change this limitation.
				sprintf(GetOutBuff(), "%.*s %s%c", g_nClientInfoLen, clientInfoID, "ACTION AWAY CLIENTINFO DCC EMAIL NETMEET PING SOUND TIME USERINFO URL VERSION", 0x1);
				proto->bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff(), NULL, TRUE);
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

	if (!bFloodChecked && (pui->Ignored() || pui->IsFlooding()))
		return;

	if (!pui->m_udi.m_bbCooked || pui->m_udi.m_talkTos.GetUpperBound() < 0) 
		IdentifyWhispers(doc, pui, msgType, pui->m_udi.m_uModes, talkTos);

	if (proto && (proto->m_dwModes & CM_NOFORMAT)) 
		pui->m_udi.m_uModes |= BM_NOFORMAT;

	if (strlen(szMesg) > 0)
	{
		if (!bAddToWhisperBox(pui, pui->m_udi.m_uModes, szMesg))
		{
			enumActions rgaIDs[4] = { (enumActions) 3, aDoNotDisplay, aHighlightMessage, aReplaceMessage };
			CString strSenderIdent = pui->GetName();
			CString	strDecodedRecipients;

			GetAddressees(pui, "; ", strDecodedRecipients, FALSE /*bUseNick*/);

			if (!pui->GetFullName().IsEmpty())
				strSenderIdent += "!"+pui->GetFullName();
			
			ASSERT(doc);
			ASSERT(doc->m_proto);

			theApp.m_dynaRules.SetCachRecipients(strDecodedRecipients);
			theApp.m_dynaRules.bMatchAndApplyRules((msgType & MT_CHANNELSEND) ? eOnMessage : eOnWhisperInRoom, (enumActions*) rgaIDs, NULL, CString(GetMyServer()), strSenderIdent, CString(doc->m_proto->m_strChannel), CString(szMesg));
			if (!(theApp.m_dynaRules.GetFlags() & g_wDoNotDisplay))
			{
				const char *szTmp;
				if (theApp.m_dynaRules.GetFlags() & g_wReplace)
					szTmp = (const char*) theApp.m_dynaRules.GetCFFinalMessage();
				else
					szTmp = (const char*) szMesg;
				char cHighlightType = -1;
				if (theApp.m_dynaRules.GetFlags() & g_wHighlight)
					cHighlightType = theApp.m_dynaRules.GetFlags() >> 8;
				AddAndExecute(new SayEntry(pui, szTmp, NULL, cHighlightType), doc);
			}
			theApp.m_dynaRules.bMatchAndApplyRules((msgType & MT_CHANNELSEND) ? eOnMessage : eOnWhisperInRoom, NULL, (enumActions*) rgaIDs, CString(GetMyServer()), strSenderIdent, CString(doc->m_proto->m_strChannel), CString(szMesg));
			theApp.m_dynaRules.SetCachRecipients("");
		}
	}

	return;

exitCheckFlood:
	pui->IsFlooding();
}


void OnKick(CChatDoc *pDoc, char *szKicker, char *szKickee, char *szMesg)
{
	// szMesg is control full

	TRACE("%s kicked %s with message (%s).\n", szKicker, szKickee, szMesg);

	char		*szControlFull, *szControlLess;
	char		cHighlightType = -1;
	CString		strBoxMsg, strKickeeIdent = szKickee;
	BOOL		bMeKicked = !stricmp(szKickee, GetMyNickName());
	CDWordArray	rgdwFormatting;
	CUserInfo	*kickeePui = LookupPui(szKickee, pDoc);
	CUserInfo	*kickerPui = LookupPui(szKicker, pDoc);
	enumActions rgaIDs[2] = { (enumActions) 1, aHighlightMessage };

	if (!kickeePui)
	{
		ASSERT(0);
		return;
	}

	// XXX - kickerPui == NULL for DALNET which allows kicks from externals
	// And when a sysop kills channel, szKicker is "System"

	if (szMesg && *szMesg)
	{
		strBoxMsg.LoadString(ID_KICK_MESG);
		VERIFY(ReplaceToken(strBoxMsg, CString("%3"), szMesg));
	}
	else
		strBoxMsg.LoadString(ID_KICK_NO_MESG);
	VERIFY(ReplaceToken(strBoxMsg, CString("%1"), kickerPui ? kickerPui->GetScreenName() : szKicker));
	VERIFY(ReplaceToken(strBoxMsg, CString("%2"), kickeePui->GetScreenName()));

	if (!(szControlFull = strdup(strBoxMsg)))
		return;
	szControlLess = SzControlLess(szControlFull, &rgdwFormatting);
	if (rgdwFormatting.GetSize())
		rgdwFormatting.Add(MAKELONG(0 /*wFormat*/, lstrlen(szControlLess)-2, /*wOffset*/));	// -2 for trailing ".

	if (!kickeePui->GetFullName().IsEmpty())
		strKickeeIdent += "!"+kickeePui->GetFullName();

	theApp.m_dynaRules.bMatchAndApplyRules(eOnKick, (enumActions*) rgaIDs, NULL, CString(GetMyServer()), strKickeeIdent, CString(pDoc->m_proto->m_strChannel), CString(""));

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
		kickerPui->m_udi.m_uModes = BM_ACTION;
		kickerPui->m_udi.m_talkTos.RemoveAll();
		kickerPui->m_udi.m_talkTos.Add((DWORD)kickeePui);

		if (theApp.m_dynaRules.GetFlags() & g_wHighlight)
			cHighlightType = theApp.m_dynaRules.GetFlags() >> 8;

		AddAndExecute(new SayEntry(kickerPui, szControlLess, &rgdwFormatting, cHighlightType), pDoc);
	}

	AddAndExecute(new PartEntry(szKickee, cHighlightType), pDoc);
	if (bMeKicked)
	{
		GotPartChannel(pDoc);
		AfxMessageBox(szControlLess);
	}

	ASSERT(pDoc->m_proto);

	// REGISB Should rules be applied before showing dialog box?
	//		  Should there be an action "DoNotDisplayDialog" ?
	theApp.m_dynaRules.bMatchAndApplyRules(eOnKick, NULL, (enumActions*) rgaIDs, CString(GetMyServer()), strKickeeIdent, CString(pDoc->m_proto->m_strChannel), CString(""));

	rgdwFormatting.RemoveAll();
	free(szControlFull);
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
	g_mapNickToPtr->RemoveKey(pui->GetName());
	if (STATUS_CHAR(*newNick)) newNick++;	// remove the at sign if there is one
	pui->SetName(newNick); // XXXXX We need to reset print name!!!!!
	if (pui == g_puiSelf) SetMyNameNick(newNick);
	g_mapNickToPtr->SetAt(newNick, pui);
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
void ChatChangeAdmin(CChatDoc *doc, const char *szNickname, int setModes, int unsetModes) {
	CUserInfo *pui = LookupPui(szNickname, doc);
	if (!pui)
		return;

	// Fix 2659
	//if (setModes & UF_OPERATOR)
	//	pui->Ignore(FALSE);	// can't ignore ops (do in case ignored before)

	LV_ITEM item;
	PreMemberNameChange(doc, pui, item);
	pui->SetFlag(setModes, TRUE);
	pui->SetFlag(unsetModes, FALSE);
	pui->SetFlag(UF_SPECTATOR, (doc->m_proto->m_dwModes & CM_MODERATED) && !pui->CheckFlag(UF_HASVOICE) && !pui->CheckFlag(UF_OPERATOR));
	PostMemberNameChange(doc, pui, item);

	if (pui == g_puiSelf)
		doc->UpdateAdminMenu();

	if (setModes & UF_OPERATOR)
	{
		CString strNewHostIdent = szNickname;
		if (!pui->GetFullName().IsEmpty())
			strNewHostIdent += "!"+pui->GetFullName();
		ASSERT(doc->m_proto);
		theApp.m_dynaRules.bMatchAndApplyRules(eOnNewHost, NULL, NULL, CString(GetMyServer()), strNewHostIdent, CString(doc->m_proto->m_strChannel), CString(""));
	}
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


void IgnoreUser(const char *nick, const char *nickMask, BOOL bIgnore, BOOL bAutoIgnore)
{
	if (bIgnore)
	{
		if (!IsIgnored(nickMask))
			AddIgnore(nickMask);  // prevent duplicate entries
	}
	else
		RemoveIgnore(nickMask);

	CString strMesg;
	UINT mesgID = bAutoIgnore ? IDS_AUTOFLOOD_IGNORE : (bIgnore ? IDS_IGNORE_FEEDBACK : IDS_UNIGNORE_FEEDBACK);
	POSITION posRoom = g_docs.GetHeadPosition();					// For each room
	while (posRoom)
	{
		CChatDoc *doc = (CChatDoc *) g_docs.GetNext(posRoom);
		if (!doc->GetConnectionStatus() == CX_INCHANNEL)
			continue;
		POSITION posPui = doc->m_mapNickToPtr.GetStartPosition();
		while (posPui)
		{											// For each user
			void *objPtr;
			CString dummy;
			doc->m_mapNickToPtr.GetNextAssoc(posPui, dummy, objPtr);
			CUserInfo *pui = (CUserInfo *) objPtr;
			if (!pui->IsSelf() && !pui->IsDeparted() &&
				(pui->GetFullName().IsEmpty() || pui->MatchesNickMask(nickMask)) &&
				(!*nick || stricmp(pui->GetName(), nick) == 0))
			{
				LV_ITEM item;
				PreMemberNameChange(doc, pui, item);
				pui->Ignore(bIgnore);
				PostMemberNameChange(doc, pui, item);
				strMesg.LoadString(mesgID);
				VERIFY(ReplaceToken(strMesg, CString("%1"), pui->GetScreenName()));
				AddAndExecute(new GetInfoEntry(pui, UnConst(strMesg)), doc);
			}
		}
	}
	CWhisperBox *wbox = GetWhisperBox();
	if (wbox)
	{
		int upper = wbox->m_leaves.GetUpperBound();
		for (int i = 0; i <= upper; i++)
		{
			CWhisperLeaf *leaf = (CWhisperLeaf *) wbox->m_leaves[i];
			if (leaf->m_nick == nick || leaf->m_id == nickMask)
			{
				strMesg.LoadString(mesgID);
				VERIFY(ReplaceToken(strMesg, CString("%1"), DecodeNickForScreen(leaf->m_nick)));
				leaf->m_richCore->iDisplayInfo(NULL, 0, "", 0, strMesg, 0, mtGetInfo, msParticipant, NULL, -1, NULL, 0);
				leaf->m_bIgnore = bIgnore;
				((CButton *)wbox->GetDlgItem(IDC_IGNORE_WBOX))->SetCheck(bIgnore ? 1 : 0);
			}
		}
	}
	// now update ignore of externals with same name combo
	CUserInfo *pui = ExternalPui(nick, nickMask, FALSE);
	if (pui)
		pui->Ignore(bIgnore);
}


void DoUserAway(CChatDoc *doc, CUserInfo *pui, BOOL bAway)
{
	LV_ITEM item;
	if (doc)
		PreMemberNameChange(doc, pui, item);
	pui->SetFlag(UF_AWAY, bAway);
	if (doc)
		PostMemberNameChange(doc, pui, item);
}


SYNTAX CIrcProto::GetSyntaxFromCmdId(enumCmdId cmdid, USHORT *puIndex /*= NULL*/)
{
	USHORT uIndex = 0;

	while (uIndex < g_uSyntaxCount && g_rgSyntax[uIndex].cmdid != cmdid)
		uIndex++;

	ASSERT(uIndex < g_uSyntaxCount);

	if (puIndex)
		*puIndex = uIndex;

	return g_rgSyntax[uIndex];
}


BOOL CIrcProto::SlashGeneric(enumCmdId cmdid, IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting)
{
	ASSERT(cmdid < cmdidMax);
	ASSERT(pParse);

	if (pParse->nArgs < g_rgIrcCmd[cmdid].uMinArg+1)
	{
		AfxMessageBox(StrSyntaxMessage(cmdid));
		return FALSE;
	}

	SYNTAX		syntax = GetSyntaxFromCmdId(cmdid);
	INT			iArg, iEncoding = ENC_DBCS;
	LPTSTR		szParam, szWord, szCtrlFull = NULL;
	CString		strOutput = g_rgIrcCmd[cmdid].szCmd;
	CDWordArray	*prgdwFormattingTmp = NULL;

	for (iArg = 0; iArg < syntax.uArgNum && iArg < pParse->nArgs-1; iArg++)
	{
		if (syntax.dwArgType[iArg] & (AT_TOPIC|AT_MESSAGE|AT_REASON))
		{
			prgdwFormattingTmp = PullFormattingOffsets(prgdwFormatting, pParse->nOffsets[iArg+1]);
			if (prgdwFormattingTmp)
				szParam = szCtrlFull = SzControlFull(szMesg + pParse->nOffsets[iArg+1], prgdwFormattingTmp);
			else
				szParam = szMesg + pParse->nOffsets[iArg+1];
		}
		else
			if (syntax.dwArgType[iArg] & AT_SPACEMULTIPLE)
				szParam = szMesg + pParse->nOffsets[iArg+1];
			else
				szParam = pParse->args[iArg+1];
		if (*szParam)
		{
			strOutput += " ";
			if (syntax.dwArgType[iArg] & AT_COLON)
				strOutput += ":";
			if ((syntax.dwArgType[iArg] & AT_COMMAMULTIPLE) ||
				(syntax.dwArgType[iArg] & AT_SPACEMULTIPLE))
			{
				BOOL bFirstWord = TRUE;
				while (TRUE)
				{
					if (*szParam == '\"')
					{
						szWord = GetToken1(szParam, &szParam, "\",", NULL, FALSE /*bSkipInitialSeps*/);
						if (*szParam == '\"')	// skip the terminating double quote
						{
							szParam++;
							strcat(szWord, "\"");
						}
					}
					else
						szWord = GetToken(szParam, &szParam, ",");
					if (!szWord)
						break;
					if ((syntax.dwArgType[iArg] & AT_COMMAMULTIPLE) && *szParam == ',')
						szParam++;
					if ((syntax.dwArgType[iArg] & AT_SPACEMULTIPLE) && *szParam == ' ')
						szParam++;
					if (!bFirstWord)
						if (syntax.dwArgType[iArg] & AT_COMMAMULTIPLE)
							strOutput += ",";
						else
							strOutput += " ";
					strOutput += StrEncodeCommandParam(syntax.dwArgType[iArg], &iEncoding, szWord);
					bFirstWord = FALSE;
				}
			}
			else
				strOutput += StrEncodeCommandParam(syntax.dwArgType[iArg], &iEncoding, szParam);
		}
	}
	strOutput += "\r\n";
	SendMessageText((LPTSTR) (LPCTSTR) strOutput);

	if (szCtrlFull)
		delete [] szCtrlFull;

	if (prgdwFormattingTmp)
		FreeAndNullFormatting(&prgdwFormattingTmp);

	return TRUE;
}


BOOL CIrcProto::SlashMode(IRCPARSE *pParse)
{
	if (pParse->nArgs < 2 || pParse->nArgs > 7)
	{
		AfxMessageBox(StrSyntaxMessage(cmdidMode));
		return FALSE;
	}

	INT		iEncoding = ENC_DBCS;
	USHORT	uIndex;
	SYNTAX	syntax = GetSyntaxFromCmdId(cmdidMode, &uIndex);
	CString	strTarget = StrEncodeCommandParam(AT_CHANNEL|AT_NICKNAME, &iEncoding, pParse->args[1]);
	CString	strOutput = CString(g_rgIrcCmd[cmdidMode].szCmd) + " " + strTarget;

	if (CHANNELPREFIX(strTarget[0]))
	{
		if (pParse->nArgs < 7 && pParse->nArgs > 2)
		{
			BOOL bMaxMembersPresent = (NULL != strchr(pParse->args[2], 'l'));
			BOOL bNicknamePresent = (NULL != strchr(pParse->args[2], 'o') || NULL != strchr(pParse->args[2], 'q') || NULL != strchr(pParse->args[2], 'v'));
			BOOL bNickMaskPresent = (NULL != strchr(pParse->args[2], 'b'));
			BOOL bPasswordPresent = (NULL != strchr(pParse->args[2], 'k'));

			USHORT uIndexTmp = 2;

			if (bMaxMembersPresent)
				syntax.dwArgType[uIndexTmp++] = AT_MAXMEMBER;

			if (bNicknamePresent)
				syntax.dwArgType[uIndexTmp++] = AT_NICKNAME;

			if (bNickMaskPresent)
				syntax.dwArgType[uIndexTmp++] = AT_NICKMASK;

			if (bPasswordPresent)
				syntax.dwArgType[uIndexTmp++] = AT_PASSWORD;
		}
	}
	else
	{
		if (pParse->nArgs > 3)
		{
			AfxMessageBox(StrSyntaxMessage(cmdidMode));
			return FALSE;
		}
		syntax = g_rgSyntax[uIndex+1];
	}

	for (INT iArg = 1; iArg < syntax.uArgNum && iArg < pParse->nArgs-1; iArg++)
		strOutput += " " + StrEncodeCommandParam(syntax.dwArgType[iArg], &iEncoding, pParse->args[iArg+1]);
	
	// only register the user/channel mode settings, not the user/channel mode reading which results in a 221/324 reply.
	if (pParse->nArgs > 2)
		return bRegisterMode((LPTSTR) (LPCTSTR) strOutput.Mid(5));
	else
	{
		strOutput += "\r\n";
		SendMessageText((LPTSTR) (LPCTSTR) strOutput);
		return TRUE;
	}
}


BOOL CIrcProto::SlashProp(IRCPARSE *pParse, char *szMesg)
{
	if (pParse->nArgs < 2)
	{
		AfxMessageBox(StrSyntaxMessage(cmdidProp));
		return FALSE;
	}

	INT		iEncoding = ENC_DBCS;
	USHORT	uIndex;
	SYNTAX	syntax = GetSyntaxFromCmdId(cmdidProp, &uIndex);
	CString	strOutput = CString(g_rgIrcCmd[cmdidProp].szCmd);

	if (pParse->lastString)
		syntax = g_rgSyntax[uIndex+1];

	for (INT iArg = 0; iArg < syntax.uArgNum && iArg < pParse->nArgs-1; iArg++)
		strOutput += " " + StrEncodeCommandParam(syntax.dwArgType[iArg], &iEncoding, pParse->args[iArg+1]);
	
	if (pParse->lastString)
	{
		strOutput += " :";
		if (*pParse->lastString)
			strOutput += StrEncodeCommandParam(syntax.dwArgType[iArg], &iEncoding, pParse->lastString);
	}

	strOutput += "\r\n";
	SendMessageText((LPTSTR) (LPCTSTR) strOutput);

	return TRUE;
}


BOOL CIrcProto::SlashJoin(IRCPARSE *pParse)
{
	if (pParse->nArgs < 2 || pParse->nArgs > 3 || _mbschr((UCHAR *) pParse->args[1], ','))
	{
		AfxMessageBox(StrSyntaxMessage(cmdidJoin));
		return FALSE;
	}

	const char *szPwd = pParse->nArgs > 2 ? pParse->args[2] : NULL;

	g_bEnterOnCreate = FALSE;
	return bSwitchToRoom(pParse->args[1], szPwd, NULL /*szCreationModes*/, 0L /*dwMaxUsers*/, TRUE);
}


BOOL CIrcProto::SlashCreate(IRCPARSE *pParse)
{
	if (pParse->nArgs < 3 || pParse->nArgs > 5 || _mbschr((UCHAR *) pParse->args[1], ','))
	{
		AfxMessageBox(StrSyntaxMessage(cmdidCreate));
		return FALSE;
	}

	const char*	szModes = pParse->nArgs > 2 ? pParse->args[2] : NULL;
	const char*	szPwd = NULL;
	DWORD		dwMaxUsers = 0L;
	
	if (pParse->nArgs > 4)
	{
		szPwd = pParse->args[4];
		dwMaxUsers = atoi(pParse->args[3]);
	}
	else
		if (pParse->nArgs > 3)
		{
			const char *szTmp = pParse->args[3];

			while (*szTmp && isdigit(*szTmp))
				szTmp++;

			if (*szTmp)
				szPwd = pParse->args[3];
			else
				dwMaxUsers = atoi(pParse->args[3]);
		}

	g_bEnterOnCreate = TRUE;
	return bSwitchToRoom(pParse->args[1], szPwd, szModes, dwMaxUsers, TRUE /*bEncodeChan*/, FALSE /*bCreateRoomInfo*/, TRUE /*bCreateRoom*/);
}


BOOL CIrcProto::SlashPart(IRCPARSE *pParse)
{
	if (pParse->nArgs > 2)
	{
		AfxMessageBox(StrSyntaxMessage(cmdidPart));
		return FALSE;
	}

	CChatDoc *pDoc = (pParse->nArgs > 1) ? LookupDoc(EncodeChan(pParse->args[1])) : GetChatDoc();

	if (!pDoc || (pParse->nArgs == 1 && pDoc->m_bStatusView))
	{
		CString strMesg;
		if (pParse->nArgs == 1 && pDoc && pDoc->m_bStatusView)
			strMesg.LoadString(IDS_ROOMNEEDSFOCUS);
		else
			if (pParse->nArgs > 1)
			{
				strMesg.LoadString(IDS_NOTINROOM);
				VERIFY(ReplaceToken(strMesg, CString("%1"), pParse->args[1]));
			}
			else
				strMesg.LoadString(IDS_NOTINANYROOM);
		AfxMessageBox(strMesg);
		return FALSE;
	}

	pDoc->OnLeave();
	return TRUE;
}


BOOL CIrcProto::SlashNick(IRCPARSE *pParse)
{
	if (pParse->nArgs != 2)
	{
		AfxMessageBox(StrSyntaxMessage(cmdidNick));
		return FALSE;
	}
	CString strNick = pParse->args[1];
	TrimQuotes(strNick);
	ChatSetNick(strNick);
	return TRUE;
}


BOOL CIrcProto::SlashPrivMsg(IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting, BOOL bInvokedByWhisperBox)
{
	if (pParse->nArgs < 3)
	{
		AfxMessageBox(StrSyntaxMessage(cmdidPrivMsg));
		return FALSE;
	}

	STRFMT		strfmt;
	CDWordArray	*prgdwFormattingTmp = PullFormattingOffsets(prgdwFormatting, pParse->nOffsets[2]);

	strfmt.szString = szMesg+pParse->nOffsets[2];
	strfmt.prgdwFormatting = prgdwFormattingTmp;

	bForEachWord(pParse->args[1], bSendPrivMsg, (void*) &strfmt, (DWORD) bInvokedByWhisperBox, ",", TRUE /*bDoubleQuotes*/);

	if (prgdwFormattingTmp)
		FreeAndNullFormatting(&prgdwFormattingTmp);

	return TRUE;
}


BOOL CIrcProto::SlashSound(IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting)
{
	if (pParse->nArgs == 2 && !lstrcmpi (pParse->args[1], "off"))
	{
		sndPlaySound (NULL, SND_SYNC);
		sndPlayMidiSound (NULL, SND_SYNC);
		return TRUE;
	} 
	else if (pParse->nArgs < 3)
	{
		AfxMessageBox(StrSyntaxMessage(cmdidSound));
		return FALSE;
	}

	int			iEncoding = ENC_DBCS;
	CString		strReceiver = StrEncodeCommandParam(AT_CHANNEL|AT_NICKNAME, &iEncoding, pParse->args[1]);
	char		szFilename[MAX_TOKEN+4];	// +4 for .wav
	char		*szText = "";
	CDWordArray	*prgdwFormattingTmp = NULL;
	BOOL		bRet;
	CString		strFilename = pParse->args[2];
	
	TrimQuotes(strFilename);
	strcpy(szFilename, (LPCTSTR) strFilename);

	// Default extension is .WAV
	LPCSTR pszDot = OurMbsChr (szFilename, '.');
	if (pszDot == NULL)
	{
		strcat(szFilename, ".");
		strcat(szFilename, GetSupportedSoundTypes()[0]);
	}

	if (pParse->nArgs > 3)
	{
		prgdwFormattingTmp = PullFormattingOffsets(prgdwFormatting, pParse->nOffsets[3]);
		szText = szMesg+pParse->nOffsets[3];
	}

	if (CHANNELPREFIX(strReceiver[0]))
	{
		// receiver is channel name
		// are we in this channel?
		bRet = bChatSendSound(szFilename /*szSnd*/, szText /*szMesg*/, prgdwFormattingTmp, TRUE /*bEcho*/, BM_SAY /*uModes*/, strReceiver /*szEncodedChannelName*/);
	}
	else
	{
		// receiver is user nickname
		// REGISB: look up is case sensitive for UTF8 nicks so there won't be a match if the nickname is not spelled exactly like the one stored in m_mapNickToPtr
		CChatDoc*	pDoc = NULL;
		CUserInfo*	pUI = PuiFromDocNickIdent(&pDoc, (LPTSTR) (LPCTSTR) strReceiver, "", TRUE /*bSkipObscuredChannels*/, TRUE /*bAddExternalIfNotThere*/);

		if (pDoc && pDoc->GetConnectionStatus() == CX_INCHANNEL)
		{
			ASSERT(!pDoc->m_bObscured);
			ASSERT(pDoc->m_proto);
			g_rgpuiWhisperees.RemoveAll();
			g_rgpuiWhisperees.Add(pUI);
			bRet = bChatSendSound(szFilename /*szSnd*/, szText /*szMesg*/, prgdwFormattingTmp, TRUE /*bEcho*/, BM_WHISPER /*uModes*/, pDoc->m_proto->m_strChannel /*szEncodedChannelName*/);
		}
		else
			if (pUI)
			{
				WhisperBox(pUI, FALSE /*bGiveFocus*/);
				bRet = bWhisperInBox(szFilename, szText, prgdwFormattingTmp, BM_WHISPER | BM_SOUND);
			}
	}

	if (prgdwFormattingTmp)
		FreeAndNullFormatting(&prgdwFormattingTmp);

	ASSERT(bRet);

	return bRet;
}


BOOL CIrcProto::SlashMeOrThink(enumCmdId cmdid, IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting, USHORT uModes, BOOL bInvokedByWhisperBox)
{
	if (pParse->nArgs < 2)
	{
		AfxMessageBox(StrSyntaxMessage(cmdid));
		return FALSE;
	}

	USHORT		uModesTmp;
	BOOL		bRet;
	CDWordArray	*prgdwFormattingTmp;

	prgdwFormattingTmp = PullFormattingOffsets(prgdwFormatting, pParse->nOffsets[1]);

	if (bInvokedByWhisperBox)
	{
		if (cmdidMe == cmdid)
			uModesTmp = BM_WHISPER | BM_ACTION;
		else
			// thoughts are sent as regular whispers in a whisper box
			uModesTmp = BM_WHISPER;
		bRet = bWhisperInBox("", szMesg+pParse->nOffsets[1], prgdwFormattingTmp, uModesTmp);
	}
	else
	{
		if (cmdidMe == cmdid)
			uModesTmp = BM_ACTION | (uModes & BM_WHISPER);
		else
			uModesTmp = BM_THINK;
		bRet = bChatSendText(CString(szMesg+pParse->nOffsets[1]), uModesTmp, TRUE /*bEcho*/, prgdwFormattingTmp);
	}

	if (prgdwFormattingTmp)
		FreeAndNullFormatting(&prgdwFormattingTmp);

	return bRet;
}


BOOL CIrcProto::SlashList(IRCPARSE *pParse, const char *szMesg)
{
	void OnChatroomListAux(const char *);

	if (pParse->nArgs > 2)
	{
		AfxMessageBox(StrSyntaxMessage(cmdidList));
		return FALSE;
	}

	if (pParse->nArgs > 1)
	{
		const char *szParams = szMesg+pParse->nOffsets[1];

		if (OurMbsChr(pParse->args[1], ',') || !IsIRCX())
			sprintf(GetOutBuff(), "LIST %s\r\n", szParams);
		else
			sprintf(GetOutBuff(), "LISTX %s\r\n", szParams);
		OnChatroomListAux(GetOutBuff());
	}
	else
		OnChatroomListAux(NULL);
	return TRUE;
}


BOOL CIrcProto::SlashWho(const char *szMesg)
{
	ASSERT(*szMesg == '/');
	OnUserListAux(szMesg+1, NULL, NULL);  // *szMesg == '/'
	return TRUE;
}


BOOL CIrcProto::SlashServer(IRCPARSE *pParse, char *szMesg)
{
	if (pParse->nArgs < 2)
	{
		AfxMessageBox(StrSyntaxMessage(cmdidServer));
		return FALSE;
	}

	char	*szServer = szMesg;
	BOOL	bCXPrompt = GetCXPrompt();

	GetToken(szServer, &szServer, " ");		// Skip /SERVER command

	while (my_isspace(*szServer))			// Get network or server name
		szServer++;

	ChatSetCXPrompt(FALSE);			// silent reconnection
	ReconnectToServer(GetMyName(), szServer);
	ChatSetCXPrompt(bCXPrompt);		// restore flag
	return TRUE;
}


BOOL CIrcProto::SlashAway(IRCPARSE *pParse, const char* szMesg, CDWordArray* prgdwFormatting)
{
	if (pParse->nArgs > 1)
	{
		// Going away.
		// This need for setting globals somewhere is kinda silly. Oh well...
		theApp.m_bAway = TRUE;
		theApp.m_bAwayPrompt = TRUE;

		CDWordArray	*prgdwFormattingTmp = PullFormattingOffsets(prgdwFormatting, pParse->nOffsets[1]);
		char		*szCtrlFull, *szStart;
		
		szCtrlFull = szStart = (char*) szMesg+pParse->nOffsets[1];

		if (prgdwFormattingTmp)
		{
			if (!(szCtrlFull = SzControlFull(szStart, prgdwFormattingTmp)))
				return FALSE;
			FreeAndNullFormatting(&prgdwFormattingTmp);
		}

		ChatSetAway (TRUE, szCtrlFull);

		if (szCtrlFull != szStart)
			delete [] szCtrlFull;
	}
	else if (theApp.m_bAway)
	{
		// Coming back.
		theApp.m_bAway = FALSE;
		theApp.m_bAwayPrompt = FALSE;
		ChatSetAway (FALSE, "");
	}
	return TRUE;
}


CString CIrcProto::StrSyntaxMessage(enumCmdId cmdid)
{
	CString		strSyntax, strArg, strKey, strSpace;
	UINT		uArg, uIndex = 0, uAtIndex;
	DWORD		dwArgType;
	SYNTAX		syntax;
	BOOL		bFirstKey;

	strSyntax.LoadString(IDS_SYNTAXPREFIX);
	strSpace.LoadString(IDS_AT_SPACEMULTIPLE);

	while (uIndex < g_uSyntaxCount && g_rgSyntax[uIndex].cmdid != cmdid)
		uIndex++;

	while (uIndex < g_uSyntaxCount && g_rgSyntax[uIndex].cmdid == cmdid)
	{
		syntax = g_rgSyntax[uIndex];
		strSyntax += CString("\n") + g_rgIrcCmd[cmdid].szCmd;
		for (uArg = 0; uArg < syntax.uArgNum; uArg++)
		{
			strSyntax += " ";
			if (syntax.dwArgType[uArg] & AT_SHOWCOLON)
				strSyntax += ":";
			if (syntax.dwArgType[uArg] & AT_OPTIONAL)
				strSyntax += "[";
			strArg = "<";
			dwArgType = AT_NICKNAME;
			bFirstKey = TRUE;
			for (uAtIndex = 0; uAtIndex < AT_COUNT; uAtIndex++)
			{
				if (syntax.dwArgType[uArg] & dwArgType)
				{
					if (strKey.LoadString(IDS_AT_NICKNAME + uAtIndex))
					{
						if (!bFirstKey)
							strArg += "|";
						strArg += strKey;
						bFirstKey = FALSE;
					}
				}
				dwArgType *= 2;
			}
			strArg += ">";
			strSyntax += strArg;
			if (syntax.dwArgType[uArg] & AT_COMMAMULTIPLE)
				strSyntax += "{," + strArg + "}";
			if (syntax.dwArgType[uArg] & AT_SPACEMULTIPLE)
				strSyntax += "{<" + strSpace + ">" + strArg + "}";
			if (syntax.dwArgType[uArg] & AT_OPTIONAL)
				strSyntax += "]";
		}
		if (syntax.uIDSComment != 0)
		{
			CString strComment;
			if (strComment.LoadString(syntax.uIDSComment))
				strSyntax += CString("\n") + strComment;
		}

		uIndex++;
	}

	return strSyntax;
}


void ListMembers(const char *szRoom, const char *szPrettyRoom)
{
	// does this channel actually still exist?
	VERIFY(GetIrcProto()->bExecuteQuery(qpListMembers, ctTopic, dtMax, (PVOID) szPrettyRoom, szRoom, ""));
}


/* Removed because of security hole
BOOL CRoomInfo::bRegisterJoins(char* szMesg)
{
	char	*szDup = strdup(szMesg);

	if (!szDup)
		return FALSE;

	char	*szTmp;
	char	*szChannels = GetToken(szDup, &szTmp, " ");
	char	*szChannel = szChannels;

	ASSERT(szChannels);

	while (*szChannels)
	{
		while (*szChannels && *szChannels != ',')
			szChannels++;
		if (*szChannels == ',')
		{
			*szChannels = '\0';
			*szChannels++;
		}
		if (*szChannel)
		{
			CRoomInfo* pEnterRoom = new CRoomInfo;
			if (!pEnterRoom)
				return FALSE;
			theApp.AddRoomInfo(pEnterRoom);
			VERIFY(bInitEnterInfo(*pEnterRoom, szChannel, NULL /*szPassword/, NULL, 0L, FALSE /*bEncodeChan/));
		}
		szChannel = szChannels;
	}

	free(szDup);

	return TRUE;
}
*/


BOOL CRoomInfo::SlashRaw(char *szMesg, IRCPARSE *pParse)
{
	if (!strnicmp(szMesg, "JOIN ", 5) || !strnicmp(szMesg, "CREATE ", 7))
	{
		// bRegisterJoins(szMesg+4);	// Old security hole

		// Nice try, but we're smarter then that!
		AfxMessageBox(IDS_ERR_RAWJOINCREATE);
		return FALSE;
	}
	else if (!strnicmp(szMesg, "LIST ", 5) || !strnicmp(szMesg, "LISTX ", 6) ||
			 !stricmp(szMesg, "LIST") || !stricmp(szMesg, "LISTX"))
	{
		if (!(g_bCanViewUnrated = bCanViewUnrated(TRUE)))
			return FALSE;
	}
	else if (!strnicmp(szMesg, "MODE ", 5))
	{
		ASSERT(pParse);
		// only register the user/channel mode settings, not the user/channel mode reading which results in a 221/324 reply.
		if (pParse->nArgs > 2)
			return bRegisterMode(szMesg+5);
	}

	sprintf(GetOutBuff(), "%s\r\n", szMesg);
	SendMessageText(GetOutBuff());
	return TRUE;
}


BOOL CRoomInfo::ProcessSlashCommand(char *szMesg, CDWordArray* prgdwFormatting, USHORT uModes, BOOL bInvokedByWhisperBox)
{
	AfxMessageBox(IDS_NOSLASH_COMMANDS);	// REGISB not sure this can ever be called
	return FALSE;
}
	

BOOL CIrcProto::ProcessSlashCommand(char *szMesg, CDWordArray* prgdwFormatting, USHORT uModes, BOOL bInvokedByWhisperBox)
{
	BOOL		bForceShowStatusWindow = FALSE;
	SHORT		nCmd;
	IRCPARSE	parse;
	BOOL		bRetVal = FALSE;

	ParseIt(szMesg, &parse, TRUE /*bDoubleQuotes*/);

	ASSERT(parse.args[0][0] == '/');

	nCmd = NGetCmd(parse.args[0]+1);

	if (nCmd < 0 || (g_rgIrcCmd[nCmd].uFlags & CMD_MUSTBECONNECTED))
	{
		int iStatus = GetConnectionStatus();

		if (iStatus != CX_NOCHANNEL && iStatus != CX_INCHANNEL)
		{
			FreeParse(&parse);
			AfxMessageBox(IDS_MUSTBE_CONNECTED);
			return FALSE;
		}
	}

	switch (nCmd)
	{
	case cmdidAction:
		nCmd = (SHORT) cmdidMe;
	case cmdidMe:
	case cmdidThink:
		bRetVal = SlashMeOrThink((enumCmdId) nCmd, &parse, szMesg, prgdwFormatting, uModes, bInvokedByWhisperBox);
		break;
	case cmdidAway:
		bRetVal = SlashAway(&parse, szMesg, prgdwFormatting);
		bForceShowStatusWindow = (CX_NOCHANNEL == GetConnectionStatus());
		break;
	case cmdidCreate:
		bRetVal = SlashCreate(&parse);
		break;
	case cmdidJoin:
		bRetVal = SlashJoin(&parse);
		break;
	case cmdidList:
		bRetVal = SlashList(&parse, szMesg);
		break;
	case cmdidMode:
		bRetVal = SlashMode(&parse);
		break;
	case cmdidMsg:
	case cmdidPrivMsg:
		bRetVal = SlashPrivMsg(&parse, szMesg, prgdwFormatting, bInvokedByWhisperBox);
		break;
	case cmdidNick:
		bRetVal = SlashNick(&parse);
		bForceShowStatusWindow = (CX_NOCHANNEL == GetConnectionStatus());
		break;
	case cmdidPart:
		bRetVal = SlashPart(&parse);
		break;
	case cmdidProp:
		bRetVal = SlashProp(&parse, szMesg);
		break;
	case cmdidQuote:
	case cmdidRaw:
	{
		char* szTmp = strchr(szMesg, ' ');
		if (!szTmp)
			return TRUE;

		while (my_isspace(*szTmp))
			szTmp++;

		bRetVal = SlashRaw(szTmp, &parse);
		break;
	}
	case cmdidServer:
		bRetVal = SlashServer(&parse, szMesg);
		break;
	case cmdidSound:
		bRetVal = SlashSound(&parse, szMesg, prgdwFormatting);
		break;
	case cmdidWho:
		bRetVal = SlashWho(szMesg);
		break;
	case cmdidInvite:
	case cmdidIsOn:
	case cmdidKick:
	case cmdidKill:
	case cmdidNames:
	case cmdidTopic:
	case cmdidUserHost:
	case cmdidWhoIs:
		bRetVal = SlashGeneric((enumCmdId) nCmd, &parse, szMesg, prgdwFormatting);
		break;
	default:
		bRetVal = SlashRaw(szMesg+1, &parse);	// skip initial '/'
	}

	// show Status Window if needed
	CFrameWnd*		pStatusFrame = GetStatusView() ? GetStatusView()->GetParentFrame() : NULL;
	CMDIFrameWnd*	pWnd = (CMDIFrameWnd*) AfxGetMainWnd();

	if (pStatusFrame && pWnd &&
		pStatusFrame != pWnd->MDIGetActive() &&
		bRetVal &&
		(nCmd < 0 || (g_rgIrcCmd[nCmd].uFlags & CMD_SHOWSTATUSWINDOW) || bForceShowStatusWindow))
	{
		theApp.m_flags0 |= F0_SHOWSTATUSWINDOW;
		pStatusFrame->ActivateFrame(SW_SHOWNOACTIVATE);
		if (!theApp.m_bEmbedded)
			// Doesn't do it immediately, does it on a PostMessage.
			((CMainFrame*) pWnd)->AutoArrangeWindows();
	}

	FreeParse(&parse);
	return bRetVal;
}


void GetAddressees(CUserInfo *pui, const char *szSeparator, CString &str, BOOL bUseNick)
{
	// REGISB: 11/13/97 new m_udi in this function
	int iUpperBound = min(pui->m_udi.m_talkTos.GetUpperBound(), 4); // clip at first 4 (so don't overrun output buff)

	for (int i = 0; i <= iUpperBound; i++)
	{
		ULONG		p = pui->m_udi.m_talkTos[i];
		CUserInfo*	pui2 = (CUserInfo *)(pui->m_udi.m_talkTos[i]);
		const char*	szNickname = bUseNick ? pui2->GetName() : pui2->GetScreenName();
		str += szNickname;
		if (i != iUpperBound)
			str += szSeparator;
	}
}


void GetWhisperedAddressees(const char *szSeparator, CString &str)
{
	int iUpperBound = min(g_rgpuiWhisperees.GetUpperBound(), 4); // clip at first 4 (so don't overrun output buff)

	for (int i = 0; i <= iUpperBound; i++)
	{
		const char *szNickname = ((CUserInfo *) g_rgpuiWhisperees[i])->GetName();
		str+= szNickname;
		if (i != iUpperBound)
			str+= szSeparator;
	}
}


CString StrGetSoundAction(CString strSoundFile, CString strTextMessage, CDWordArray* prgdwFormatting)
{
	CString strSoundAction(GetMyScreenName());	//REGISB: was calling puiSelf->GetScreenName() instead
	strSoundAction += " ";
	if (!strTextMessage.IsEmpty())
	{
		PushFormattingOffsets(prgdwFormatting, strSoundAction.GetLength());
		strSoundAction += strTextMessage;
		if (prgdwFormatting)
			AddFormat(prgdwFormatting, MAKELONG(0, strSoundAction.GetLength()));
		strSoundAction += " ";  // space between strTextMessage and strSoundFile
	}
	strSoundAction += "(";
	strSoundAction += strSoundFile;
	strSoundAction += ")";

	return strSoundAction;
}


static BOOL bInsertAnnotations(CUserInfo *puiSelf, char *szBuff, USHORT uModes, BOOL bIncludeParenthesis)
{
	void		EmotionToBytes(CEmotion &em, BYTE &emotion, BYTE &intensity);

	CHAR		faceIndex, torsoIndex;
	BYTE		faceEmotion, faceIntensity, torsoEmotion, torsoIntensity;
	BYTE		bbRequested;
	CEmotion	face, torso;
    CAvatarX*	av = MyAvatar();

	if (av && puiSelf)
	{
		av->GetIndices(faceIndex, torsoIndex, bbRequested);
		av->GetEmotions(face, torso);
		BYTE faceIndexByte = IndexToByte(faceIndex);
		BYTE torsoIndexByte = IndexToByte(torsoIndex);
		BYTE modeByte = IndexToByte(BM2SM(uModes));
		EmotionToBytes(face, faceEmotion, faceIntensity);
		EmotionToBytes(torso, torsoEmotion, torsoIntensity);

		sprintf(szBuff, "%s#%c%c%c%c%c%c%c%c%s%c%c",
				bIncludeParenthesis ? "(" : "",
				CGESTUREPREFIX, torsoIndexByte, torsoEmotion, torsoIntensity,
				CEXPRESSIONPREFIX, faceIndexByte, faceEmotion, faceIntensity,
				bbRequested ? "R" : "",
				CMODEPREFIX, modeByte);
		// REGISB: 11/13/97 new m_udi in this function
		if ((uModes != BM_WHISPER && puiSelf->m_udi.m_talkTos.GetUpperBound() >= 0) ||
			(uModes == BM_WHISPER && g_rgpuiWhisperees.GetUpperBound() >= 0))
		{
			CString str = "T";
			if (uModes == BM_WHISPER)
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


void ProcessNonComicsMsg(CString &str, USHORT &uModes)
{
	CString strPrefix;
	if (uModes & BM_THINK)
	{
		strPrefix.LoadString(ID_THINK_PREFIX);
		VERIFY(ReplaceToken(strPrefix, CString("%1"), ""));
		strPrefix.TrimLeft();
		str = strPrefix + str;
		uModes &= ~BM_THINK;
		uModes |= BM_ACTION;   // send it as an action
	}
	if (uModes & BM_ACTION)
	{
		strPrefix = actionID;
		strPrefix += " ";
		str = strPrefix + str;
		str += "\001";
	}
}


BOOL bChatSendText(CString& str, USHORT uModes, BOOL bEcho, CDWordArray* prgdwFormatting, LPCTSTR szEncodedChannelName, BOOL bWhispereesFilled, BOOL bInvokedByWhisperBox)
{
	SHORT		nLenBefore = str.GetLength();
	BOOL		bFreeTmp = FALSE;
	CDWordArray	*prgdwFormattingTmp = prgdwFormatting;

	if (!(uModes & BM_WHISPER))
		ConfirmAway(str);  // give 'em a choice to come back

	// strip off \n?
	str.TrimRight();
	if (str.IsEmpty()) 
		return TRUE;

	INT			iStatus;
	SHORT		nLenAfter = str.GetLength();
	BOOL		bSuccess = TRUE;
	USHORT		uMyModes = uModes;
	BOOL		bComicView = FALSE;
	CUserInfo*	puiSelf = NULL;
	CRoomInfo*	proto = NULL;
	
	if (szEncodedChannelName)
	{
		if (*szEncodedChannelName)
		{
			CChatDoc* pDoc = LookupDoc(szEncodedChannelName);
			if (pDoc)
			{
				if (pDoc == (CChatDoc*) theApp.m_pExitingDoc || pDoc->GetConnectionStatus() != CX_INCHANNEL)
					return TRUE;
				proto = pDoc->m_proto;
			}
		}
		else
			if (str[0] == '/')
				proto = GetDefaultProto();
	}	
	else
		proto = currentRoom ? currentRoom : GetDefaultProto();

	if (!proto)
		return TRUE;

	if (str[0] == '/')
		return proto->ProcessSlashCommand(UnConst(str), prgdwFormatting, uModes, bInvokedByWhisperBox);

	if (proto->m_doc)
	{
		// REGISB: 03/27/98 - Bug fixes
		bComicView = ((CChatDoc*) proto->m_doc)->m_bComicView;
		puiSelf = ((CChatDoc*) proto->m_doc)->m_puiSelf;
	}

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
			FreeAndNullFormatting(&prgdwFormattingTmp);
		prgdwFormattingTmp = prgdwFormattingTmp2;
		bFreeTmp = TRUE;
	}

	CString	strTmp = str, strControlFull = str;

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

	if ((uMyModes & BM_ACTION) && bEcho)
	{	// save copies of the action w/ name prepended
		strTmp = CString(GetMyScreenName()) + CString(" ") + strTmp;
		if (prgdwFormattingTmp)
			PushFormattingOffsets(prgdwFormattingTmp, strlen(GetMyName()) + 1);
	}

	iStatus = proto->GetConnectionStatus();

	if (iStatus == CX_INCHANNEL || iStatus == CX_NOCHANNEL)	// only do this if connected
	{
		if (puiSelf)
			if (bWhispereesFilled)
				// copy my talktos from the global g_rgpuiWhisperees
				CopyPtrArrayToCWDordArray(g_rgpuiWhisperees, puiSelf->m_udi.m_talkTos);
			else
				// copy my talktos from member list to puiself
				MListTalkTosToPuiself(puiSelf);

		char szAnnotations[MAX_ANNOTATIONS] = "";
		if (bComicView && (g_bSendComicsData || (GetDefaultProto() && GetDefaultProto()->IsIRCX())))
		{
			BOOL bUseAnnotations = TRUE;
			if ((uModes & BM_WHISPER) && g_rgpuiWhisperees.GetSize() == 1)
			{
				CUserInfo *pui = (CUserInfo *) g_rgpuiWhisperees.GetAt(0);
				ASSERT(pui);
				bUseAnnotations = !pui->IsExternal();
			}
			if (bUseAnnotations)
				bInsertAnnotations(puiSelf, szAnnotations, uModes, GetDefaultProto() && !GetDefaultProto()->IsIRCX() /*bIncludeParenthesis*/);
		}
		if ((!g_bSendComicsData || !bComicView) && ((uModes & BM_ACTION) || (uModes & BM_THINK)))
			ProcessNonComicsMsg(strControlFull, uModes);
		
		// If rich formating was done (prgdwFormattingTmp!=NULL) then send along the original
		// unformated string for NM in case we are talking to them too. IRC stuff will ignore
		// this.
		if (!(uModes & BM_WHISPER))
		{
			if (!puiSelf)
			{
				bSuccess = FALSE;  // if haven't joined yet, don't say
				goto exit;
			}

			if (!(bSuccess = !puiSelf->CheckFlag(UF_SPECTATOR)))
				AfxMessageBox(IDS_SPECTATEONLY);
			else
				bSuccess = proto->bChatSendToChannel(szAnnotations,
												     (LPCTSTR) strControlFull,
												     (prgdwFormattingTmp ? (char*)(LPCTSTR) str : NULL),
												     uModes);
		}
		else
		{
			BOOL bJustToMe;
			bSuccess = proto->bSendWhispers(szAnnotations,
											(LPCTSTR) strControlFull,
											(prgdwFormattingTmp ? (char*)(LPCTSTR) str : NULL),
											uModes, &bJustToMe);
			if (bJustToMe)
				bEcho = FALSE;
		}
	}

	if ((proto->m_dwModes & CM_NOFORMAT) && uModes != BM_WHISPER)
		bEcho = FALSE;

	if (bSuccess && bEcho)
		ShowSay((CChatDoc*) proto->m_doc, puiSelf, strTmp, prgdwFormattingTmp, bComicView, uMyModes);	// don't receive PRIVMSGs sent by self, so do explicit show
	
exit:
	if (prgdwFormattingTmp && bFreeTmp)
		FreeAndNullFormatting(&prgdwFormattingTmp);

	return bSuccess;
}


BOOL bChatSendSound(const char *szSnd, const char *szMesg, CDWordArray *prgdwFormatting, BOOL bEcho, USHORT uModes, LPCTSTR szEncodedChannelName)
{
	CChatDoc*	pDoc = NULL;
	CUserInfo*	puiSelf = NULL;
	CRoomInfo*	proto = NULL;
	BOOL		bSuccess = TRUE;
	BOOL		bNeedFree = FALSE;
	BOOL		bExternalChannel = FALSE;
	INT			iStatus;
	char*		szControlFull = NULL;
	const char*	szQuotedSnd = szSnd;
	
	if (szEncodedChannelName)
	{
		ASSERT(*szEncodedChannelName);
		pDoc = LookupDoc(szEncodedChannelName);
		if (pDoc && pDoc != (CChatDoc*) theApp.m_pExitingDoc && pDoc->GetConnectionStatus() == CX_INCHANNEL)
			// internal channel case
			proto = pDoc->m_proto;
		else
		{
			// external channel case
			if (pDoc = LookupDoc(STATUS_WINDOW_NAME))
			{
				proto = pDoc->m_proto;

				ASSERT(pDoc->m_proto);
				ASSERT(!strcmp(proto->m_strChannel, STATUS_WINDOW_NAME));

				proto->m_strChannel = szEncodedChannelName;
				bExternalChannel = TRUE;
			}
		}
	}	
	else
	{
		proto = currentRoom ? currentRoom : GetDefaultProto();
		pDoc = (CChatDoc*) proto->m_doc;
	}

	if (!proto)
		return TRUE;

	if (pDoc)
		puiSelf = pDoc->m_puiSelf;

	if (uModes != BM_WHISPER)
		ConfirmAway();  // give 'em a choice to come back
	
	iStatus = proto->GetConnectionStatus();

	if (iStatus == CX_INCHANNEL || iStatus == CX_NOCHANNEL)	// only do this if connected
	{
		bNeedFree = CTCPQuoteString(&szQuotedSnd);
		if (prgdwFormatting)
			szControlFull = SzControlFull(szMesg, prgdwFormatting);

		sprintf(GetOutBuff(), "%.*s %s %s%c", g_nSoundLen, soundID, szQuotedSnd, szControlFull ? szControlFull : szMesg, 0x1);

		if (!(uModes & BM_WHISPER))
		{
			if (!puiSelf && !bExternalChannel)
			{
				bSuccess = FALSE;  // if haven't joined yet, don't say
				goto exit;
			}

			if (!(bSuccess = (puiSelf && !puiSelf->CheckFlag(UF_SPECTATOR)) || bExternalChannel))
				AfxMessageBox(IDS_SPECTATEONLY);
			else
				bSuccess = proto->bChatSendToChannel(NULL,					/*szAnnotations*/
													 UnConst(GetOutBuff()),	/*szMesg*/
													 NULL,					/*szNMText*/
													 BM_SOUND);				/*uModes*/		
		}
		else 
		{
			BOOL bJustToMe;

			if (puiSelf)
				CopyPtrArrayToCWDordArray(g_rgpuiWhisperees, puiSelf->m_udi.m_talkTos);
			bSuccess = proto->bSendWhispers(NULL,
											UnConst(GetOutBuff()),
											NULL,
											BM_SOUND, &bJustToMe);
			if (bJustToMe)
				bEcho = FALSE;
		}
	}

	if (bSuccess && theApp.m_bPlaySounds)
		// and provide aural feedback
		bFindAndPlaySound(szSnd, TRUE, FALSE);

	if (bSuccess && bEcho)
	{
		// and provide visual feedback (For historical reasons, need to tack on name)
		CString strSoundAction = StrGetSoundAction(szSnd, szMesg, prgdwFormatting);

		if (puiSelf)
			ShowSay(pDoc, puiSelf, strSoundAction, prgdwFormatting, pDoc->m_bComicView, BM_ACTION | uModes);	// uModes == BM_SAY or BM_WHISPER
		else
			// external channel case
			((CTextView*) pDoc->m_view)->TextLine(NULL /*pui*/, GetMyScreenName(), szEncodedChannelName, (char*) (const char*) strSoundAction, BM_EXCHAN | BM_ACTION, FALSE, NULL, -1);
	}

exit:
	if (bExternalChannel)
	{
		ASSERT(proto && proto == pDoc->m_proto);
		proto->m_strChannel = STATUS_WINDOW_NAME;
	}

	if (szControlFull)
		delete [] szControlFull;

	if (bNeedFree && szQuotedSnd)
		free ((void *) szQuotedSnd);

	return bSuccess;
}


void CRoomInfo::ChatGetInfo (CUserInfo *pui) {
	// Set a bit indicating that we requested this info,
	// ... so people can't nefariously send us info that we don't want to show.
	pui->IncrementRequestInfo(RF_PROFILE);
	sprintf(GetOutBuff(), "#%s", GETINFOPREFIX);
	// Send message requesting info directly to target
	bChatSendPrivMesg(pui->GetName(), GetOutBuff(), NULL);
}

void CRoomInfo::ChatGetAvatarInfo (CUserInfo *pui, BOOL bInteractive) {
	// Force the avatar to download when this info arrives.
	pui->SetFlag (bInteractive ? UF_INTERACTIVEDOWNLOAD : UF_AUTODOWNLOAD, TRUE);
	sprintf(GetOutBuff(), "#%s", REQUESTCHARPREFIX);
	// Send message requesting info directly to target
	bChatSendPrivMesg(pui->GetName(), GetOutBuff(), NULL);
}

void CRoomInfo::ChatSyncBackDrop(CChatDoc* pDoc, const char *szBackdrop, const char *szURL)
{
	if (*szBackdrop && GetConnectionStatus() == CX_INCHANNEL)
	{
		CString strMesg;
		
		strMesg.Format("#%s%s,%s", NEWBACKGRNDPREFIX, szBackdrop, szURL ? szURL : "");
		bChatSendToChannel(UnConst(strMesg), NULL, NULL);
	    // Send the old one for backwards compatibility. Always send this message
		// second - newer clients will recognize both messages, but see that the 
		// background name is the same, and ignore this message. NOTE: The old message
		// should not include the filetype of the backdrop in the backdrop name,
		// only the name itself.
		LPCSTR pszDot = OurMbsChr (szBackdrop, '.');
		int nChars = pszDot != NULL ? pszDot - szBackdrop : lstrlen (szBackdrop);
		strMesg.Format("#%s %s", BACKGRNDPREFIX, (LPCSTR)CString (szBackdrop, nChars));
		bChatSendToChannel(UnConst(strMesg), NULL, NULL);
		// Set the background property of the channel. It's ok if this fails.
		strMesg.Format("%s,%s", szBackdrop, szURL ? szURL : "");
		ChangeProperty(pDoc->m_puiSelf, "bk", strMesg);
	}
}

void CRoomInfo::OnPropertyChange(
LPCSTR pszProperty,
LPCSTR pszValue)
{
	if (!lstrcmpi (pszProperty, "bk") && pszValue != NULL) {
		// Background changed.
		char *szVar = (LPSTR)pszValue;
		char *szBackdropName = GetToken (szVar, &szVar, ",");
		szBackdropName = szBackdropName ? strdup (szBackdropName) : NULL; // Copy it.
		if (szBackdropName != NULL) {
			char *szBackdropURL = GetToken2(szVar, &szVar, ",", ",)");
			AddAndExecute (new ChangeBackDropEntry (szBackdropName, szBackdropURL), m_doc);
			free (szBackdropName);
		}
	}
}

BOOL CUserInfo::IsFlooding()
{
	if (!(theApp.m_uFloodFlags & FLOOD_IGNORE) || this == g_puiSelf)
		return FALSE;

	USHORT uNow = time(NULL) & 0xffff;
	USHORT uInterval = abs(uNow - m_uIntervalStart);

	if (uInterval > theApp.m_uFloodInterval)
	{
		m_uIntervalStart = uNow;
		m_uMsgCount = 1;
		return FALSE;
	}
	else
	{
		if (++m_uMsgCount < theApp.m_uFloodCount)
			return FALSE;

		// evil flooder
		GetDefaultProto()->DoIgnoreUser(this, TRUE, TRUE);
		return TRUE;
	}
}


BOOL CRoomInfo::bSendWhispers(const char *szAnnotations, const char *szMesg, char *szNMText /*= NULL*/, USHORT uModes /*= BM_WHISPER*/, BOOL *pbJustToMe /*= NULL*/)
{
	int iUpperbound = g_rgpuiWhisperees.GetUpperBound();

	if (pbJustToMe)
		*pbJustToMe = TRUE;

	for (int i = 0; i <= iUpperbound; i++)
	{
		CUserInfo *pui = (CUserInfo *) g_rgpuiWhisperees[i];
		ASSERT(pui);
		if (pbJustToMe && stricmp(GetMyNickName(), pui->GetName()))
			*pbJustToMe = FALSE;
		if (!bChatSendPrivMesg(pui->GetName(), szAnnotations, szMesg, szNMText, FALSE, uModes))
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


void AddToRoomList(CRoom *pRoom, BOOL bAddIt)
{
	CRoomList *prl = GetRoomList();
	if (prl && bAddIt)
	{
		pRoom->CalculateSortByte();
		int iRoomIndex = prl->m_persist->AddRoom(pRoom);
		prl->AddToRoomList(iRoomIndex);
		prl->AnnounceCount();
	}
	else
		delete pRoom;
}


void AddToUserList(CUser *pUser)
{
	const char *stristr(const char *str1, const char *str2);

	CUserList *ul = GetUserList();
	if (ul)
	{
		const char *searchstr = ul->m_persist->m_strUserFilter;
		if ((ul->m_persist->m_searchType == USERSEARCH_ALL) ||
			(ul->m_persist->m_searchType == USERSEARCH_NICK && stristr(pUser->GetPrettyNick(), searchstr)) ||
			(ul->m_persist->m_searchType == USERSEARCH_ID && stristr(pUser->m_strIdentity, searchstr)) ||
			(ul->m_persist->m_searchType == USERSEARCH_ROOM))
		{
			int userIndex = ul->m_persist->AddUser(pUser);
			ul->AddToUserList(userIndex);
			ul->AnnounceCount();
		}
		else
			pUser->Release();
	}
	else
		pUser->Release();
}


void ShowIdentity(const char *nick, const char *user, const char *host) {
	CString strMesg;
	CUserInfo *pui = LookupPui(nick);
	if (!pui) return;
	strMesg.LoadString(IDS_REPORT_IDENT2);
	VERIFY(ReplaceToken(strMesg, CString("%1"), pui->GetScreenName()));
	VERIFY(ReplaceToken(strMesg, CString("%2"), user));
	VERIFY(ReplaceToken(strMesg, CString("%3"), host));
	AddAndExecute(new GetInfoEntry(LookupPui(nick), UnConst((LPCTSTR) strMesg)));
}


void CRoomInfo::UpdateStatus() {	
	CString leftMesg;
	int iConn = GetConnectionStatus();
	if (iConn == CX_DISCONNECTED)
		leftMesg.LoadString(ID_DISCONNECTED);
	else if (iConn == CX_CONNECTING)
		leftMesg.LoadString(ID_CONNECTING); 
	else if (iConn == CX_NOCHANNEL) {
		if (currentRoom && currentRoom != this && this != GetDefaultProto()) return;  // only write new status if it's for current room!!!
		leftMesg.LoadString(ID_NOCHANNEL);
		CString str;
		GetMyServerPrettyName (str);
		VERIFY(ReplaceToken(leftMesg, CString("%1"), str));
	} else if (iConn = CX_INCHANNEL) {
		if (currentRoom && currentRoom != this) return;  // only write new status if it's for current room!!!
		leftMesg = ((CChatDoc *)m_doc)->m_strStatus;
	}

	if (!leftMesg.IsEmpty())
		theApp.SetStatusPaneString(0, leftMesg);
}


void CRoomInfo::ChatSetOperator(CUserInfo *pui, int mode) {
	if (mode == UM_HOST) {
		sprintf(GetOutBuff(), "MODE %s +o %s\r\n", m_strChannel, pui->GetName());
		SendMessageText(GetOutBuff());
	} else {
		if (pui->IsOperator()) {
			sprintf(GetOutBuff(), "MODE %s -o %s\r\n", m_strChannel, pui->GetName());
			SendMessageText(GetOutBuff());
		}

		if (mode == UM_SPEAKER) {
			BOOL bIsModerated = currentRoom->m_dwModes & CM_MODERATED;
			if (bIsModerated) {
				sprintf(GetOutBuff(), "MODE %s +v %s\r\n", m_strChannel, pui->GetName());
				SendMessageText(GetOutBuff());
			}
		}
		if (mode == UM_SPECTATOR) {
			sprintf(GetOutBuff(), "MODE %s -v %s\r\n", m_strChannel, pui->GetName());
			SendMessageText(GetOutBuff());
		}
	}
}		


void CRoomInfo::ChatGetVersion(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->IncrementRequestInfo(RF_VERSION);
		sprintf(GetOutBuff(), "%.*s%c", g_nVersionLen, versionID, 0x01);
		bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}

void CRoomInfo::ChatPingUser(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->SetFlag(UF_REQUESTPING, TRUE);
		time_t seconds;
		time(&seconds);
		sprintf(GetOutBuff(), "%.*s %ld%c", g_nPingLen, pingID, (long) seconds, 0x01);
		bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}

void CRoomInfo::ChatGetLocalTime(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->IncrementRequestInfo(RF_TIME);
		sprintf(GetOutBuff(), "%.*s%c", g_nTimeLen, timeID, 0x01);
		bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}

void CRoomInfo::ChatGetEmail(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->IncrementRequestInfo(RF_EMAIL);
		sprintf(GetOutBuff(), "%.*s%c", g_nEmailLen, emailID, 0x01);
		bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
	}
}

void CRoomInfo::ChatGetHomePage(CUserInfo *pui) {
	if (pui && !pui->IsDeparted()) {
		pui->IncrementRequestInfo(RF_HOMEPAGE);
		sprintf(GetOutBuff(), "%.*s%c", g_nUrlLen, urlID, 0x01);
		bChatSendPrivMesg(pui->GetName(), NULL, GetOutBuff());
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
		VERIFY(bChatSendPrivMesg(pui->GetName(),	/*szAddressee*/
								 NULL,				/*szAnnotations*/
								 GetOutBuff(),		/*szMesg*/
								 NULL,				/*szNMText*/
								 FALSE,				/*bAsNotice*/
								 BM_AWAY));			/*uModes*/
	else
	{
		ShowAway((CUserInfo*) doc->m_puiSelf, GetOutBuff()+g_nAwayLen+1, doc);
		VERIFY(bChatSendToChannel(NULL,				/*szAnnotations*/
								  GetOutBuff(),		/*szText*/
								  NULL,				/*szNMText*/
								  BM_AWAY));		/*uModes*/
	}
}


void CRoomInfo::DoKickDlg(const char *nick, const char *strBan)
{
	CKickDialog kickDlg;
	CString strDlg;
	strDlg.LoadString(IDS_KICKREASON);
	VERIFY(ReplaceToken(strDlg, CString("%1"), DecodeNick(nick)));
	kickDlg.m_strKick = strDlg;
	kickDlg.m_strBanPattern = strBan;
	if (theApp.DoModalDlg(&kickDlg) == IDOK)
	{
		if (kickDlg.m_bBanToo)
		{					// first ban (if requested)
			kickDlg.m_strBanPattern.TrimLeft();
			if (!kickDlg.m_strBanPattern.IsEmpty())
				ChatBanUser(UnConst(kickDlg.m_strBanPattern), TRUE);
		}
		kickDlg.m_reason.TrimLeft();
		kickDlg.m_reason.TrimRight();
		ChatKickUser(nick, UnConst(kickDlg.m_reason));  // then kick
	}
	GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}


void DoBanDlg(const char *szEncodedChannelName, const char *szBan, CStringArray &banArray)
{
	CBanDlg banDlg;
	banDlg.m_strBanPattern = szBan;
	banDlg.m_strMesg.LoadString(IDS_BANMESG);
	BOOL bMIC = FALSE;
	if (currentRoom && !strcmp(strCurrentChannel, szEncodedChannelName))
		bMIC = dwCurrentChannelMode & CM_MIC;
	else
		banDlg.m_szEncodedChannel = szEncodedChannelName;
	VERIFY(ReplaceToken(banDlg.m_strMesg, CString("%1"), DecodeChan(szEncodedChannelName, bMIC)));
	banDlg.m_banArray = &banArray;
	theApp.DoModalDlg(&banDlg);
	banArray.RemoveAll();			// don't keep strings around
	GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}


BOOL bDoInvite(char *szInvitee, void *pvProto, DWORD dwData)
{
	CRoomInfo*	pProto = (CRoomInfo*) pvProto;
	LPCTSTR		szEncodedNick;

	ASSERT(pProto);
	ASSERT(szInvitee);

	if (pProto->IsIRCX() && bExtendedNickname(szInvitee))
		szEncodedNick = EncodeNick(szInvitee);
	else
		szEncodedNick = szInvitee;
	
	ASSERT(szEncodedNick);

	return pProto->ChatSendInvitation(szEncodedNick);
}


BOOL bSendPrivMsg(char *szReceiver, void *pvStrFmt, DWORD dwInvokedByWhisperBox)
{
	ASSERT(szReceiver && *szReceiver);
	ASSERT(pvStrFmt);

	CChatDoc*		pDoc = NULL;
	CString			strReceiver = szReceiver;
	INT				iEncoding = ENC_DBCS;
	CHAR*			szText = ((PSTRFMT) pvStrFmt)->szString;
	CDWordArray*	prgdwFormatting = ((PSTRFMT) pvStrFmt)->prgdwFormatting;

	ASSERT(szText && *szText);

	if (GetIrcProto())
		strReceiver = GetIrcProto()->StrEncodeCommandParam(AT_CHANNEL|AT_NICKNAME, &iEncoding, (LPTSTR) (LPCTSTR) strReceiver);

	if (CHANNELPREFIX(strReceiver[0]))
	{
		// receiver is channel name
		// are we in this channel?
		pDoc = LookupDoc(strReceiver);
		if (pDoc && pDoc != (CChatDoc*) theApp.m_pExitingDoc && pDoc->GetConnectionStatus() == CX_INCHANNEL)
			// internal channel case
			bChatSendText(CString(szText), BM_SAY /*uModes*/, TRUE /*bEcho*/, prgdwFormatting, strReceiver /*szEncodedChannelName*/, FALSE /*bWhispereesFilled*/);
		else
		{
			// external channel case
			if (pDoc = LookupDoc(STATUS_WINDOW_NAME))
			{
				ASSERT(pDoc->m_proto);
				ASSERT(!strcmp(pDoc->m_proto->m_strChannel, STATUS_WINDOW_NAME));

				char* szCtrlFull = szText;
				if (prgdwFormatting)
				{
					szCtrlFull = SzControlFull(szText, prgdwFormatting);
					if (!szCtrlFull)
					{
						ASSERT(FALSE);
						return FALSE;
					}
				}

				pDoc->m_proto->m_strChannel = strReceiver;
				pDoc->m_proto->bChatSendToChannel(NULL,
												  szCtrlFull,
												  NULL,
												  BM_SAY);
				if (prgdwFormatting)
					delete [] szCtrlFull;

				pDoc->m_proto->m_strChannel = STATUS_WINDOW_NAME;
				((CTextView*) pDoc->m_view)->TextLine(NULL /*pui*/, GetMyScreenName(), strReceiver, szText, BM_EXCHAN, FALSE, prgdwFormatting, -1);
			}
		}
	}
	else
	{
		// receiver is user nickname
		// REGISB: look up is case sensitive for UTF8 nicks so there won't be a match if the nickname is not spelled exactly like the one stored in m_mapNickToPtr
		CUserInfo* pUI = PuiFromDocNickIdent(&pDoc, (LPTSTR) (LPCTSTR) strReceiver, "", TRUE /*bSkipObscuredChannels*/, TRUE /*bAddExternalIfNotThere*/);

		if (!dwInvokedByWhisperBox && pDoc && pDoc->GetConnectionStatus() == CX_INCHANNEL)
		{
			ASSERT(!pDoc->m_bObscured);
			ASSERT(pDoc->m_proto);
			g_rgpuiWhisperees.RemoveAll();
			g_rgpuiWhisperees.Add(pUI);
			VERIFY(bChatSendText(CString(szText), BM_WHISPER, TRUE /*bEcho*/, prgdwFormatting, pDoc->m_proto->m_strChannel, TRUE /*bWhispereesFilled*/));
		}
		else
			if (pUI)
			{
				WhisperBox(pUI, FALSE /*bGiveFocus*/);
				VERIFY(bWhisperInBox("", szText, prgdwFormatting, BM_WHISPER));
			}
	}

	return TRUE;
}


void CRoomInfo::ChatInvite() {
	if (GetConnectionStatus() == CX_INCHANNEL && g_puiSelf) {
		CInviteDlg inviteDlg;
		if (theApp.DoModalDlg(&inviteDlg) == IDOK)
			bForEachWord(UnConst(inviteDlg.m_strInvitees), bDoInvite, this, 0L, " ,\r\n");

		GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
	}
}

// starting with an empty memberlist, repopulates it, according to our map's info.
void RepopulateMemberList() {
	void *p;
	CString attedNick, nick;
	int GetAvatarUpperBound();

	// image list no longer destroyed on changing modes!!!
//	int avUpper = GetAvatarUpperBound();  // reset old icon indices -- the image list was destroyed
//	for (int i = 1; i <= avUpper; i++)
//		GetAvatar(i)->m_iconIndex = -1;

	ChatEmptyMemberList();
	POSITION pos = g_mapNickToPtr->GetStartPosition();
//	g_bInEnumeration = TRUE;				// no need to sort in AddToMembersList
	while (pos) {
		g_mapNickToPtr->GetNextAssoc(pos, nick, p);
		CUserInfo *pui = (CUserInfo *) p;
		if (!pui->IsDeparted())
			AddToMembersList(pui);
	}

	// REGISB: 11/13/97 new m_udi in this function
	if (g_puiSelf)
		g_puiSelf->m_udi.m_talkTos.RemoveAll();		// if any were selected, they aren't now.
//	g_bInEnumeration = FALSE;
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

void CIrcProto::ChatSetNick(const char *szNickname)
{
	CString strNewNick = szNickname;
	CString strOldNick = GetMyName();
	int iStatus = GetConnectionStatus();
	if (iStatus != CX_DISCONNECTED)
	{
		// we have to do this anyway if we're connecting and the names haven't changed,
		// since if we have a bad nick, we'll need to get back a bad nick response.
		// Otherwise, we know we don't have a bad nick, w/ other connect statuses.
		// NOTE: not true any more, since CX_CONNECTING means not yet logged in... 
		while (!ChatChangeNick(strNewNick))
		{
			TryNewNick(ID_ERR_BAD_NICK, strNewNick, FALSE, &strNewNick);
			if (stricmp(strNewNick, strOldNick) != 0) break;  // break if they entered a different nick
		}
		// REGISB 02/02/98 this g_strRequestedNick variable is not being used - commented out code
		// g_strRequestedNick = newNick;    // cache requested version in case we get a socket-based Nick Change Message
	} 
	else
		if (iStatus == CX_DISCONNECTED && strcmp(szNickname, strOldNick))
			AddAndExecute(new NickEntry(strOldNick, szNickname));  // do it right away (won't get a nick message back)
}


void InitializeChannelConnection(CRoomInfo& enterInfo, SHORT *pnCXKeepServer, BOOL *pbCXPrompt, BOOL bCreateRoom)
{
	ASSERT(pnCXKeepServer);
	ASSERT(pbCXPrompt);
	ASSERT(*pnCXKeepServer >= 0);

	if (*pnCXKeepServer > 0)
		(*pnCXKeepServer)--;						// reset to default value, since we've used it up
	if (*pbCXPrompt)
	{
		CChannelDlg dlg;
		dlg.m_bIsIRCX = GetDefaultProto () && GetDefaultProto ()->IsIRCX ();
		theApp.DoModalDlg(&dlg);
		VERIFY(bInitEnterInfo(enterInfo, dlg.m_strChannel, dlg.m_strPassword, NULL /*szCreationmodes*/, 0L /*dwMaxUsers*/, TRUE));
	}
	*pbCXPrompt = TRUE;							// only go without prompts once!

	if (bCreateRoom)
		GetIrcProto()->ChatCreateChannel(enterInfo);
	else
		GetIrcProto()->ChatJoinChannel(enterInfo);
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

	cprops.m_bIsIRCX = GetDefaultProto () && GetDefaultProto ()->IsIRCX ();
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
	
	if (theApp.DoModalDlg(&cprops) == IDOK) {
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


void ChatSwitchChannel(const char *szChannelName)
{
	CChannelDlg dlg;
	dlg.m_bIsIRCX = GetDefaultProto () != NULL && GetDefaultProto()->IsIRCX();
	if (szChannelName)  // fill it in if we are correcting an error
		dlg.m_strChannel = DecodeChan(szChannelName);					// don't know if it's MIC
	if (theApp.DoModalDlg(&dlg) == IDOK)
	{
		g_bEnterOnCreate = FALSE;
		// check if room already exists -- if so bring to foreground...
		bSwitchToRoom(dlg.m_strChannel, dlg.m_strPassword, NULL /*szCreationModes*/, 0L /*dwMaxUsers*/, TRUE);
	}
}


void ShowBadChannelName(const char *szChannelName)
{
	BOOL bOnCreate = g_bEnterOnCreate;
	CString strMesg;
	strMesg.LoadString(ID_ERR_BADCHANNELNAME);   // DONE 2
	VERIFY(ReplaceToken(strMesg, CString("%1"), DecodeChan(szChannelName)));  // don't know if it's MIC
	AfxMessageBox(strMesg);
	if (bOnCreate)
		ChatCreateRoom (g_enterInfo);
	else
		ChatSwitchChannel(szChannelName); // so we show the bad room name (which may be cleared by JOIN #A,#B)
}


BOOL bInitEnterInfo(CRoomInfo& enterInfo, const char *szChannelName, const char *szChannelPassword, const char *szCreationModes, DWORD dwMaxUsers, BOOL bEncodeChan)
{
	enterInfo.m_bSetMode = FALSE;
	enterInfo.m_dwModes = CM_NOEXTERN | CM_TOPICHOST;
	enterInfo.m_dwMaxUsers = dwMaxUsers;
	enterInfo.m_strPassword = szChannelPassword ? szChannelPassword : "";
	enterInfo.m_strCreationModes = szCreationModes ? szCreationModes : "";
	enterInfo.m_strChannel = bEncodeChan ? EncodeChan(szChannelName) : szChannelName;
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

void ChatCreateRoom(CRoomInfo& enterInfo)
{
	CChannelCreateDlg cprops;
	cprops.m_bIsIRCX = GetDefaultProto () && GetDefaultProto ()->IsIRCX ();

	// if we get rid of ChatRetryCreate, we can get rid of this InitEnterInfo, and following props being
	// set by consulting enterInfo.
	bInitEnterInfo(enterInfo, "", NULL, NULL, 0L, FALSE);

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

	if (theApp.DoModalDlg(&cprops) == IDOK)
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
		g_bEnterOnCreate = TRUE;
		if (enterInfo.m_strChannel.Find(g_chComma) == -1)
			bSwitchToRoom(NULL);
		else
			ShowBadChannelName((LPCTSTR) enterInfo.m_strChannel);
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


BOOL bProcessAddChannel(const char *szChanName, CRoomInfo *proto, SHORT *pnCXKeepServer, BOOL *pbCXPrompt) // szChanName is encoded
{
	ASSERT(pnCXKeepServer);
	ASSERT(pbCXPrompt);
	ASSERT(*pnCXKeepServer >= 0);

	CChatDoc *doc = LookupDoc(szChanName);
	if (!doc)
		doc = LookupDoc("");
	if (!doc)
	{
		(*pnCXKeepServer)++;
		*pbCXPrompt = FALSE;
		g_bFreezeTabs = TRUE;
		AfxGetMainWnd()->SendMessage(WM_COMMAND, ID_FILE_NEW, 0);
		g_bFreezeTabs = FALSE;
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

	currentRoom->m_strPrettyChannel = DecodeChan(szChanName);   // if it's a MIC room, we'll change this later

	doc->SetLegalPath(currentRoom->m_strPrettyChannel);		// decoded true channel name
	GetTabBar()->m_tabCtrl.RedrawWindow();

	CFrameWnd *frame = doc->m_client->GetParentFrame();
	if (frame)
		frame->ActivateFrame();

	AdjustViewMode();
	strCurrentChannel = szChanName; // should be up-to-date now...
	// REGISB find enterInfo from szChanName - might be a problem for clones!
	ChatSetChannel(DecodeChan(g_enterInfo.m_strChannel));	// successfully connected, so remember requested channel
	proto->SetConnectionStatus(CX_INCHANNEL);		// we are officially in a channel
	doc->InitHistory();

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
	if (g_puiSelf && GetIrcProto()->GetConnectionStatus() == CX_INCHANNEL)  // i.e., we're recording a history
		AddAndExecute(new NickEntry(oldNick, newNick));
	else SetMyNameNick(newNick);
}


CUserInfo* PuiFromDocNickIdent(CChatDoc **ppDoc, char *szNickname, const char *szUserIdent, BOOL bSkipObscuredChannels, BOOL bAddExternalIfNotThere)
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
	{	// try other visible rooms...
		POSITION pos = g_docs.GetHeadPosition();
		while (pos)
		{
			pDocTmp = (CChatDoc*) g_docs.GetNext(pos);
			if (pDocTmp->GetConnectionStatus() != CX_INCHANNEL || (pDocTmp->m_bObscured && bSkipObscuredChannels))
				continue;
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
		pui = ExternalPui(szNickname, szUserIdent, bAddExternalIfNotThere);

	return pui;
}


void OnTextMsg(CChatDoc *doc, char *szNickname, const char *szUserIdent, char *szMesg, BYTE msgType, CDWordArray *talkTos) {
//	if (!g_puiSelf) return; // Don't process privmsgs out of channel
	TRACE("Got a text message! (snick = %s)\n", szNickname);
	if (CHANNELPREFIX(*szNickname))
		return;  // don't process messages from entire channels

	CUserInfo	*pui = PuiFromDocNickIdent(&doc, szNickname, szUserIdent, TRUE /*bSkipObscuredChannels*/, TRUE /*bAddExternalIfNotThere*/);

	if (pui)		 // old code:	&& !pui->Ignored() && !pui->IsFlooding())
	{
		if (*szMesg != '#' || !ProcessComment(doc, pui, szMesg, msgType))
			ProcessSay(doc, pui, szMesg, msgType, talkTos);
	}
}


void OnDataMsg(CChatDoc *pDoc, char *szNickname, const char *szUserIdent, char *szData, BYTE msgType)
{
	TRACE("Got a data message! (snick = %s)\n", szNickname);

	ASSERT(szData);
	ASSERT(szNickname && *szNickname);

	if (CHANNELPREFIX(*szNickname))
		return;  // don't process messages from entire channels

	CUserInfo	*pui = PuiFromDocNickIdent(&pDoc, szNickname, szUserIdent, TRUE /*bSkipObscuredChannels*/, TRUE /*bAddExternalIfNotThere*/);

	if (pui)
		if (*(szData+1) == ' ')
		{
			// if (!pui->Ignored() && !pui->IsFlooding())
			ProcessComment(pDoc, pui, szData, msgType);
		}
		else
			// ASSERT(msgType & MT_CHANNELSEND);	// this kind of annotations can only be channel messages - Wrong?
			ProcessUDIData(pDoc, pui, szData);
}


void ProcessBeginEnumeration() {
	g_bInEnumeration = TRUE;
}


void ProcessEndEnumeration() {
	g_bInEnumeration = FALSE;

	if (!g_puiSelf) {			// if didn't find self, don't allow enter (add better fix post MSN2.0)
		CString strMesg;
		strMesg.LoadString(ID_ERR_CANTJOIN);
		VERIFY(ReplaceToken(strMesg, CString("%1"), strCurrentChannel));
		AfxMessageBox(strMesg);
		GetChatDoc()->m_proto->ChatPartChannel(GetChatDoc(), FALSE);
	}

	void UpdateTitle(CChatDoc * = NULL);
	if (GetChatDoc()->m_bComicView) {
		UpdateTitle();	// update title only after enum
		if (GetChatDoc()->m_bIconMembers)
		{
			ASSERT(GetMembers());
			GetMembers()->Sort();					// sort members only after enum
		}
	}

	GetChatDoc()->SetModifiedFlag(FALSE);   // starting converation
}



BOOL bPassesRatings(const char *szRating, BOOL bPromptOverride) {
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


BOOL bSwitchToRoom(const char *szNewRoom, const char *szPassword, const char *szCreationModes, DWORD dwMaxUsers, BOOL bEncodeChan, BOOL bCreateRoomInfo, BOOL bCreateRoom)
{
	CRoomInfo* pEnterRoom;

	ConfirmAway();

	if (bCreateRoomInfo)
	{
		ASSERT(szNewRoom && *szNewRoom);
		pEnterRoom = new CRoomInfo;
		if (!pEnterRoom)
			return FALSE;
		theApp.AddRoomInfo(pEnterRoom);
	}
	else
		pEnterRoom = &g_enterInfo;

	const char *szRoom = szNewRoom ? szNewRoom : g_enterInfo.m_strChannel;
	if (bEncodeChan)
		szRoom = EncodeChan(szRoom);

	if (strchr(szRoom, g_chComma))
	{
		ShowBadChannelName(szRoom);
		return FALSE;
	}

	CChatDoc *doc = LookupDoc(szRoom);

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
		theApp.m_pExitingDoc = NULL;
	}

	if (doc)
	{	// doc already live -- just activate
		CFrameWnd *frame = doc->m_client->GetParentFrame();
		if (frame)
			frame->ActivateFrame();
	}
	else
	{	// must create new doc and join room
		ASSERT(pEnterRoom);
		g_bCXPrompt = FALSE;
		if (szNewRoom) 
			VERIFY(bInitEnterInfo(*pEnterRoom, szRoom, szPassword, szCreationModes, dwMaxUsers, FALSE));  // don't reset if they passed in a NULL szNewRoom!
		InitializeChannelConnection(*pEnterRoom, &g_nCXKeepServer, &g_bCXPrompt, bCreateRoom);
	}

	return TRUE;
}


void OnInvite(const char *szSender, const char *szFullName, const char *szRoom)
{
	static BOOL bInInvite = FALSE;

	if (!theApp.m_bAllowInvites || !bCanViewUnrated()) 
		return;

	if (IsIgnored(szFullName)) return;   // do not allow invites from ignored folks

	if (bInInvite) 
		return;  // poor man's not-so-critical-section
	bInInvite = TRUE;

	// Issue here: when rule applies, should we still show the invitation dialog box??
	theApp.m_dynaRules.bMatchAndApplyRules(eOnInvitation, NULL, NULL, CString(GetMyServer()), CString(szSender)+"!"+szFullName, CString(szRoom), CString(""));

	CInvitationDlg invdlg;
	invdlg.m_strMessage.LoadString(ID_JOIN_OFFER);
	VERIFY(ReplaceToken(invdlg.m_strMessage, CString("%1"), DecodeNick(szSender)));
	VERIFY(ReplaceToken(invdlg.m_strMessage, CString("%2"), DecodeChan(szRoom)));		// don't know if MIC
	int rval = theApp.DoModalDlg(&invdlg);
	if (rval == IDYES)
	{
		g_bEnterOnCreate = FALSE;
		bSwitchToRoom(szRoom);
	}
	if (rval != IDCANCEL && invdlg.m_bIgnore)
		IgnoreUser(szSender, szFullName, TRUE, FALSE);

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


void OnBadChannelPassword(CRoomInfo& enterInfo)
{
	const short		nMaxChan = 20;
	short			nTmp = 0;
	CPasswordDlg	pwdDlg;
	CString			strChannelName = DecodeChan(enterInfo.m_strChannel);  // don't know if MIC
	LPTSTR			szChanTmp = strChannelName.GetBuffer(MAX_TOKEN);

	if (strlen(szChanTmp) > nMaxChan)
	{
		while (nTmp < nMaxChan)
		{
			szChanTmp = CharNext(szChanTmp);
			nTmp++;
		}
		*szChanTmp = '\0';
		strChannelName.ReleaseBuffer();
		strChannelName += "...";
	}
	else
		strChannelName.ReleaseBuffer();

	if (!enterInfo.m_strPassword.IsEmpty())
		AfxMessageBox(IDS_BAD_PASSWORD);

	pwdDlg.m_strMessage.LoadString(ID_PASSWORD_PROMPT);
	VERIFY(ReplaceToken(pwdDlg.m_strMessage, CString("%1"), strChannelName));
	pwdDlg.m_strPassword = enterInfo.m_strPassword;
	enterInfo.m_strPassword = "";		// so on canceled dialog, won't preserve password
	if (theApp.DoModalDlg(&pwdDlg) == IDOK)
	{
		g_bEnterOnCreate = FALSE;
		bSwitchToRoom(enterInfo.m_strChannel, pwdDlg.m_strPassword, NULL /*szCreationModes*/, 0L /*dwMaxUsers*/, FALSE /*bEncodeChan*/, TRUE /*bCreateRoomInfo*/);
	}
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
	if (doc->GetConnectionStatus() != CX_DISCONNECTED)
		doc->m_proto->SetConnectionStatus(CX_NOCHANNEL);  // GotPartChannel may be called after disconnection
	ChatEmptyMemberList(doc);
}


void OfflineEditInits()
{
	CChatDoc *pDoc = GetChatDoc();

	if (pDoc && !pDoc->m_bStatusView)
	{
		pDoc->InitHistory();

		// Set things up so user can still type in text and
		//  get comics.  Great for debugging too.
		AddAndExecute(new JoinEntry(new CUserInfo(GetMyName())));  // pretend to join to set pui & g_puiSelf
	}
}


void ChatServerDisconnect(BOOL bCheckRules, BOOL bResumeConnection)
{
	if (serverConn.m_hSocket != INVALID_SOCKET) 	// close last connection if necessary
		serverConn.Close();	

	if (!bResumeConnection)
	{
		if (::AfxGetMainWnd ())
			::AfxGetMainWnd ()->KillTimer (ID_CONNECT_TRY);
		theApp.m_SrvConnector.Cleanup ();
	
		StopIdentD();
	}

	CString strIdent;

	if (!theApp.m_myIdent.IsEmpty())
		strIdent = CString("!") + theApp.m_myIdent;

	#ifdef DEBUG
		if (serverConn.m_queries.GetCount())
			TRACE("!serverConn.m_queries still has %d cells!\n", serverConn.m_queries.GetCount());
	#endif // DEBUG
	serverConn.m_queries.FreeRemoveAll();

	theApp.m_bAway = FALSE;			// no longer away
	theApp.m_ignores.RemoveAll();	// clear the ignore list
	theApp.m_nMyIdentLength = 0;	// length of !<username>@<hostname> for myself
	theApp.m_myIdent = "";
	theApp.CleanRoomInfos();

	// Reset socket class attributes
	serverConn.Reset();

	// Forced part all existing rooms (need to clear memberlists to avoid bad pui ref later!)
	POSITION pos = g_docs.GetHeadPosition();
	while (pos)
	{
		CChatDoc *doc = (CChatDoc *) g_docs.GetNext(pos);
		if (doc->m_proto->GetType() == PC_IRC)
		{
			CIrcProto *proto = (CIrcProto *)(doc->m_proto);
			if (proto && proto->m_bInRoom) GotPartChannel(doc);
		}
	}

	INT iStatus = GetDefaultProto()->GetConnectionStatus();
	GetDefaultProto()->SetConnectionStatus(bResumeConnection ? CX_CONNECTING : CX_DISCONNECTED);

	if (bCheckRules && iStatus != CX_DISCONNECTED && iStatus != CX_CONNECTING)
	{
		theApp.m_dynaNotifs.bStopNotifsDaemon();
		theApp.m_dynaNotifs.bUpdateNotifsDaemonExt(TRUE);

		theApp.m_dynaRules.bStopRulesDaemon();
		theApp.m_dynaRules.bUpdateRuleSetsDaemonExt(TRUE);
		
		theApp.m_dynaRules.bMatchAndApplyRules(eOnDisconnect, NULL, NULL, CString(GetMyServer()), CString(GetMyNickName())+strIdent, CString(""), CString(""));
	}
}


BOOL bChatServerConnect(const char *szServer)
{
	ASSERT(CX_DISCONNECTED == GetIrcProto()->GetConnectionStatus());

	GetIrcProto()->SetConnectionStatus(CX_CONNECTING);
	((CFrameWnd*)AfxGetMainWnd())->UpdateWindow();
	void StartIdentD();
	StartIdentD();
	if (!theApp.m_SrvConnector.BeginConnectToService (szServer))
	{
		::AfxGetMainWnd ()->SendMessage (WM_COMMAND, ID_CONNECT_ERROR);
		return FALSE;
	}
	::AfxGetMainWnd ()->SetTimer (ID_CONNECT_TRY, 50, NULL);
	return TRUE;


#if 0
	GetIrcProto()->SetConnectionStatus(CX_CONNECTING);
	((CFrameWnd*)AfxGetMainWnd())->UpdateWindow();

	CString	strServer;
	int		iPort;
	char*	szColon = (char *) _mbschr((const UCHAR*) szServer, ':');

	if (szColon)
	{
		strServer = CString(szServer, szColon - szServer);
		iPort = atoi(szColon+1);
	}
	else
	{
		strServer = szServer;
		iPort = 6667;
#if 0
		// if port wasn't specified and user is with aol, trying to connect
		// to our servers, use port 7000 instead to avoid AOL'd port blocker
		if (strnicmp(szServer, "mschat", 6) == 0 || strnicmp(szServer, "comicsrv", 8) == 0) {
			char name[200];
			if (gethostname(name, sizeof(name)) == 0) { // ie, success
				int len = strlen(name);
				if (len > 7) {  // 7 == strlen("aol.com")
					char *partialName = name + len - 7;
					if (stricmp(partialName, "aol.com") == 0) port = 7000;
				}
			}
		}
#endif
	}

	VERIFY(serverConn.Create());
	int iRetVal = serverConn.Connect(strServer, iPort);
	if (iRetVal || GetLastError() == WSAEWOULDBLOCK)
	{
		StartIdentD();
		return TRUE;
	}
	else
	{
		TRACE("iRetVal = %u, lasterr = %u\n", iRetVal, GetLastError());
		GetIrcProto()->SetConnectionStatus(CX_DISCONNECTED);
		CString strMesg;
		strMesg.LoadString(ID_ERR_CONNECT);
		VERIFY(ReplaceToken(strMesg, CString("%1"), strServer));
		AfxMessageBox(strMesg);
		return FALSE;
	}
#endif
}


void InitializeServerConnection(CRoomInfo* pEnterInfo, BOOL* pbCXPrompt)
{
	BOOL bGoodChannelName = TRUE;

	if (!CChatDoc::CleanupExistingWindows())
		return;	// can't clean up old windows, so don't start new ones

	while (TRUE)
	{
		ChatServerDisconnect();	// first close last connection if necessary
		if (*pbCXPrompt)
		{
			int rval = DoConnectDialog();
			if (rval != IDOK)
			{	// on cancel or close, or anything except OK
				AdjustViewMode();
				OfflineEditInits();
				if (GetChatDoc())
					GetChatDoc()->SetModifiedFlag(FALSE);
				return;
			}
			ASSERT(pEnterInfo);
			bGoodChannelName = bInitEnterInfo(*pEnterInfo, theApp.m_myChannel, NULL, NULL, 0L, TRUE);
		}
			
		*pbCXPrompt = TRUE;	// only good for one non-prompt (could also be set to false in above DoModal)
		if (bGoodChannelName)
			if (bChatServerConnect(GetMyServer()))
				break;
	}
}


void ReconnectToServer(const char *szDecodedNickname, const char *szServerName)
{
	if (!CChatDoc::CleanupExistingWindows())
		return;	// can't clean up old windows, so don't start new ones

	SetMyName(szDecodedNickname);

	ChatServerDisconnect();	// first close last connection if necessary
	if (bInitEnterInfo(g_enterInfo, theApp.m_myChannel, NULL, NULL, 0L, TRUE))
	{
		CString	strService;

		theApp.m_listChatServices.GetServiceNameFromDisplayName(szServerName, strService);
		theApp.m_strConnectedService = strService;
		bChatServerConnect(strService);
	}
}


//void ChatSetPath(const char *szPath) {	REGISB not used
//	CChatDoc *doc = GetChatDoc();
//	// note: doc is null if we're closing up IE3.0 w/ Connect dialog open
//	if (doc) doc->SetPathName(szPath); 
//}


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
		return (theApp.DoModalDlg(&psOptions, TRUE));
	} else return (theApp.DoModalDlg(&psOptions, TRUE));
}


void ShowSay(CChatDoc *pDoc, CUserInfo *pui, const char *szText, CDWordArray *prgdwFormatting, BYTE bbCooked, USHORT uModes)
{
	CChatDoc*	pDocTmp = pDoc;

	if (!pDocTmp)
		pDocTmp = GetChatDoc();

	if (!pui || !pDocTmp)
		return;	// haven't made net connection yet!

	// don't show if ignored
	if (pui->Ignored())
		return;

	USHORT ExtractAvatarID(void*);

	pui->m_udi.m_bbCooked = bbCooked;
	pui->m_udi.m_uModes	= uModes;
	pui->m_udi.m_chExprE = 
	pui->m_udi.m_chGestE = 
	pui->m_udi.m_chExprI = 
	pui->m_udi.m_chGestI = 0;

	if (pDocTmp->m_bComicView)
	{
		ASSERT(bbCooked);
		CAvatarX *av = GetAvatar(ExtractAvatarID(pui));
		av->GetIndices(pui->m_udi.m_chExpr, pui->m_udi.m_chGest, pui->m_udi.m_bbReq);
		// for now, take the hit that SayEntry will just set expr, gest, and req again!
		AddAndExecute(new SayEntry(pui, szText, prgdwFormatting ? prgdwFormatting : (CDWordArray*) -1), pDocTmp);
	}
	else
	{
		pui->m_udi.m_bbReq = 0;
		AddAndExecute(new SayEntry(pui, szText, prgdwFormatting ? prgdwFormatting : (CDWordArray*) -1), pDocTmp);
	}
}

void AcknowledgeInvite(const char *szNickname, const char *szRoom) {
	CString strConfirmation;
	strConfirmation.LoadString(IDS_INVITE_CONF);
	ReplaceToken(strConfirmation, CString("%1"), szNickname);
	ReplaceToken(strConfirmation, CString("%2"), szRoom);
	AfxMessageBox(strConfirmation);
}


void ConfirmAway(LPCSTR pszConditionallyOnText)
{
	if (theApp.m_bAway && theApp.m_bAwayPrompt)
	{
		// Some commands don't trigger this prompt. /away is a prime example.
		if (pszConditionallyOnText)
		{
			if (*pszConditionallyOnText == '/')
			{
				CString str;
				LPCSTR pszSpace = OurMbsChr (pszConditionallyOnText, ' ');
				if (pszSpace)
					str = CString (pszConditionallyOnText, pszSpace - pszConditionallyOnText);
				else
					str	= pszConditionallyOnText;
				if (!str.CompareNoCase ("/AWAY"))
				{
					return;
				}
			}
		}

		if (AfxMessageBox(IDS_CONFIRMAWAY, MB_YESNO) == IDYES)
			theApp.OnAwayToggle();
		else
			theApp.m_bAwayPrompt = FALSE;  // they've seen it once
	}
}


CUser* CreateUserFromWhoReply(IRCPARSE *pParse)
{
	CUser *pUser = new CUser;
	if (!pUser)
		return NULL;
	pUser->m_strNickname = pParse->args[6];
	pUser->m_strIdentity = pParse->args[3];
	pUser->m_strIdentity += "@";
	pUser->m_strIdentity += pParse->args[4];
	pUser->m_strRoom = pParse->args[2];
	pUser->m_strPrettyRoom = DecodeChan(pParse->args[2]); // unfortunately, don't know if it's MIC
	if (pParse->args[6][0] == '\'')
		pUser->m_szPrettyNick = strdup(DecodeNickForScreen(pParse->args[6]));
	if (pParse->lastString)
	{
		void CSInString(char **pszString, const char *szChannelName = NULL, CChatDoc *doc = NULL);

		CSInString(&pParse->lastString);
		const char *szFullName = strchr(pParse->lastString, ' '); // jump over hopcount
		if (szFullName)
		{
			while (my_isspace(*szFullName))
				szFullName++;
			if (*szFullName)
				pUser->m_strFullName = szFullName;
		}
	}
	return pUser;
}

// The following functions deal with key strings. These strings take the form
// 			key1=value1;key2=value2;...
// A value can optionally be in quotes - these quotes are stripped out.
// A value itself can't have any quotes in it.
// Examples of key strings:
//				msg=hello;id=200
//				msg="hello; what is your name";id=200

// Find entries in a key string (see above for a description). If pszKey is NULL,
// just returns the first key-value pair (like an enumeration).
static BOOL 
FindInKeyString(
LPCSTR pszKeyString, 
LPCSTR pszKey, 
int	*  pnKeyPos, 
int *  pnValPos, 
int *  pnNextValPos)
{
	int nKeyLength = pszKey != NULL ? lstrlen (pszKey) : 0;

	int nPos = 0;
	BOOL bFound = FALSE;
	int nIncr;

	while (!bFound && *pszKeyString != '\0') {
		if (pszKey == NULL || (!strncmp (pszKeyString, pszKey, nKeyLength) &&
				pszKeyString[nKeyLength] == '='))
		{
			if (pszKey == NULL) {
				LPCSTR psz = OurMbsChr (pszKeyString, '=');
				if (psz == NULL) {
					return FALSE;
				}
				nKeyLength = (int)(psz - pszKeyString);
			}
			*pnKeyPos = nPos;
			*pnValPos = *pnKeyPos + nKeyLength + 1;
			bFound = TRUE;
		}
		// Skip to next one.
		LPCSTR psz = OurMbsChr (pszKeyString, '=');
		if (psz != NULL) {
			psz++;
			if (*psz == '\"') {
				psz = OurMbsChr (psz + 1, '\"');
				if (psz != NULL) {
					psz++;
				}
			} 
			if (psz != NULL && (psz = OurMbsChr (psz, ';')) != NULL) {
				nPos += (int)((psz + 1) - pszKeyString);
				pszKeyString = psz + 1;
				continue;
			}
		}
		
		int nLen = lstrlen (pszKeyString);
		nPos += nLen;
		pszKeyString += nLen;
	}

	if (bFound) {
		*pnNextValPos = (pszKeyString[nPos] == '\0') ? -1 : nPos;
	}
	return bFound;
}

// Makes changes to a key string (see above for a description)
// If pszValue is NULL or empty, the key is deleted
// If the change would make the string longer than nMaxSize, this function fails.

BOOL
ChangeKeyString(
CString &strKeyString, 
LPCSTR pszKey, 
LPCSTR pszValue,
int nMaxSize)
{
	ASSERT (pszKey != NULL && *pszKey != '\0');
	ASSERT (OurMbsPbrk (pszKey, "=;\"") == NULL);
	CString strNew = strKeyString;

	int nKeyPos, nValPos, nNextValPos;
	if (FindInKeyString (strNew, pszKey, &nKeyPos, &nValPos, &nNextValPos)) {
		if (nNextValPos == -1) {
			strNew = strNew.Left (nKeyPos - 1); // Take out trailing ; if there is one
		} 
		else {
			strNew = strNew.Left (nKeyPos) + strNew.Mid (nNextValPos);
		}
	}

	if (pszValue == NULL || *pszValue == '\0') {
		// Deletion has been done.
		return TRUE;
	}

	if (OurMbsChr (pszValue, '\"') != NULL) {
		return FALSE;
	}

	int nOrigLength = strNew.GetLength ();
	BOOL bNeedQuoting = OurMbsPbrk (pszValue, "=;\"") != NULL;
	int nAddedLength = lstrlen (pszKey) + lstrlen (pszValue) + 
							(bNeedQuoting ? 2 : 0) + 
							(nOrigLength > 0 ? 1 : 0); // 1 for the semicolon
	if (nOrigLength + nAddedLength > nMaxSize) {
		return FALSE;
	}

	CString strPair;
	if (bNeedQuoting) {
		strPair.Format ("%s=\"%s\"", pszKey, pszValue);
	} else {
		strPair.Format ("%s=%s", pszKey, pszValue);
	}
	if (nOrigLength > 0) {
		strPair = ';' + strPair;
	}

	strKeyString = strNew + strPair;
	return TRUE;
}

// Gets a value from a key string (see above for a description)
// If the key doesn't exist, returns FALSE.

BOOL
GetValueFromKeyString(
LPCSTR pszKeyString, 
LPCSTR pszKey, 
CString &strValueOut)
{
	if (pszKeyString == NULL) {
		return FALSE;
	}

	ASSERT (pszKey != NULL && *pszKey != '\0');
	ASSERT (OurMbsPbrk (pszKey, "=;\"") == NULL);

	int nKeyPos, nValPos, nNextValPos;
	if (!FindInKeyString (pszKeyString, pszKey, &nKeyPos, &nValPos, &nNextValPos)) {
		return FALSE;
	}

	if (nNextValPos == -1) {
		strValueOut = CString (pszKeyString + nValPos);
	} 
	else {
		strValueOut = CString (pszKeyString + nValPos, nNextValPos - nValPos);
	}

	// Note: For multi-byte character support, we check the last character of a 
	// string by using _mbsrchr.

	int nLeftTrim, nRightTrim;
	nLeftTrim  = 0;
	nRightTrim = strValueOut.GetLength ();

	// Trim trailing separators.
	while (nRightTrim > 0 && (LPCSTR)_mbsrchr ((const UCHAR *)(LPCSTR)strValueOut, ';') == 
		   ((LPCSTR)strValueOut) + nRightTrim - 1) {
		nRightTrim--;
	}

	// Trim quotes if they exist.
	if (nRightTrim >= 2 && strValueOut[0] == '\"' && 
			OurMbsRChr (strValueOut, '\"') == ((LPCSTR)strValueOut) + nRightTrim - 1) {
		nLeftTrim++;
		nRightTrim--;
	}

	strValueOut = strValueOut.Mid (nLeftTrim, nRightTrim - nLeftTrim);
	return TRUE;
}

// Enumerates entries in a key string.

BOOL 
EnumKeyString(
LPCSTR &pszKeyString, 
CString &strKey, 
CString &strValue)
{
	if (pszKeyString == NULL) {
		return FALSE;
	}

	int nKeyPos, nValPos, nNextValPos;
	if (!FindInKeyString (pszKeyString, NULL, &nKeyPos, &nValPos, &nNextValPos)) {
		return FALSE;
	}
	strKey = CString (pszKeyString, nValPos - nKeyPos - 1);
	if (!GetValueFromKeyString (pszKeyString, strKey, strValue)) {
		return FALSE;
	}
	pszKeyString = (nNextValPos == -1) ? NULL : pszKeyString + nNextValPos;
	return TRUE;
}
