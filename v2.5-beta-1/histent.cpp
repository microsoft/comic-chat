#include "stdafx.h"

#include "chat.h"
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "pageview.h"
#include "ui.h"
#include "histent.h"
#include "memblst.h"
#include "resource.h"
#include "textcore.h"
#include "textview.h"
#include "spltchat.h"
#include "chatview.h"
#include "format.h"
#include "ccommon.h"
#include "saywnd.h"
#include "protsupp.h"
#include "ircproto.h"

extern CChatApp theApp;

void BetweenTabs(CString&, CString&);
CDWordArray *IdentifyURLs(CDWordArray* , const char *);
void ProcessNick(CChatDoc *, const char *, const char *, BOOL = TRUE);
void ReinstallPui(CUserInfo *pui, const char *newNick);

SayEntry::~SayEntry()
{
	free((void *) m_mesg);
	free((void *) m_name);
	FreeAndNullFormatting(&m_prgdwFormatting);
}


SayEntry::SayEntry(CUserInfo *pui, const char *szMesg, CDWordArray *prgdwFormatting, char cHighlightType)
{
	m_pui			= pui;
	m_name			= strdup(pui->GetName());
	m_cHighlightType= cHighlightType;

	m_udi.m_chExpr	= pui->m_udi.m_chExpr;
	m_udi.m_chGest	= pui->m_udi.m_chGest;
	m_udi.m_chExprE	= pui->m_udi.m_chExprE;
	m_udi.m_chExprI	= pui->m_udi.m_chExprI;
	m_udi.m_chGestE	= pui->m_udi.m_chGestE;
	m_udi.m_chGestI	= pui->m_udi.m_chGestI;
	m_udi.m_uModes	= pui->m_udi.m_uModes;
	m_udi.m_bbCooked= pui->m_udi.m_bbCooked;
	m_udi.m_bbReq	= 1;	// for now, ignore req parameter... req;

	// REGISB: 11/13/97 new pui->m_udi in this function
	CopyArray(pui->m_udi.m_talkTos, m_udi.m_talkTos);

	m_mesg = strdup(szMesg);
	if (!prgdwFormatting)
	{
		// For incoming messages
		m_prgdwFormatting = new CDWordArray;
		m_mesg = SzControlLess((char*) m_mesg, m_prgdwFormatting);
	}
	else
	{
		// For outgoing messages
		if ((DWORD) -1 != (DWORD) prgdwFormatting)
			m_prgdwFormatting = CopyFormatting(prgdwFormatting);
		else
			m_prgdwFormatting = NULL;
	}
	m_prgdwFormatting = IdentifyURLs(m_prgdwFormatting, m_mesg);
}


void SayEntry::Execute(int, CChatDoc *)
{
	CopyArray(m_udi.m_talkTos, m_pui->m_udi.m_talkTos);

	if (!GetChatDoc()->m_bComicView)
	{
		GetTextView()->TextLine(m_pui, NULL /*szSenderNickname*/, NULL /*szReceiver*/, m_mesg, m_udi.m_uModes, m_udi.m_bbCooked, m_prgdwFormatting, m_cHighlightType);
		return;
	}

	CAvatarX *av = GetAvatar(m_pui->GetAvatarID());
	if (!av)
		return;  // should never happen

	if (m_udi.m_bbCooked)
	{
		if (!(av->m_flags & OTHERMAPPED))
			av->SetIndices(m_udi.m_chExpr, m_udi.m_chGest, m_udi.m_bbReq);
		else {
			void BytesToEmotion(CEmotion &em, BYTE emotion, BYTE intensity);
			CEmotion expr;
			CEmotion gest;
			BytesToEmotion(expr, m_udi.m_chExprE, m_udi.m_chExprI);
			BytesToEmotion(gest, m_udi.m_chGestE, m_udi.m_chGestI);
			av->SetEmotions(expr, gest);
		}
	}

	GetView()->GetDocument()->ProcessLine(m_pui->GetAvatarID(), m_mesg, m_udi.m_uModes, m_udi.m_bbCooked, m_prgdwFormatting);
}


void SayEntry::WriteSelf(CArchive &ar)
{
	CString strOut, strOtherArgs;
	
	FormatOtherArgs(strOtherArgs);

	BOOL QuoteReturns(const char **);

	const char	*szMesg = m_mesg;
	char		*szControlFull = NULL;

	// REGISB: 08/28/97 need to save the control full version of m_mesg
	if (m_prgdwFormatting)
		szControlFull = SzControlFull(szMesg, m_prgdwFormatting);
	if (szControlFull)
		szMesg = szControlFull;

	BOOL bNeedFree = QuoteReturns(&szMesg);

	strOut.Format("say\t%s\t%s\t%s\r\n", m_name, strOtherArgs, szMesg);
	
	if (bNeedFree)
		free((void *) szMesg);

	if (szControlFull)
		delete [] szControlFull;

	ar.WriteString(strOut);
}


void SayEntry::FormatOtherArgs(CString &str)
{
	str.Format("(G:%d %d %d E:%d %d %d R:%d M:%d", m_udi.m_chGest, m_udi.m_chGestE, m_udi.m_chGestI,
				m_udi.m_chExpr, m_udi.m_chExprE, m_udi.m_chExprI, m_udi.m_bbReq, BM2SM(m_udi.m_uModes));

	int iUpper = m_udi.m_talkTos.GetUpperBound();
	if (iUpper >= 0)
	{
		str += " T:";
		for (int i = 0; i <= iUpper; i++)
		{
			str += ((CUserInfo*) m_udi.m_talkTos[i])->GetName();
			if (i < iUpper) str += ",";
		}
	}
	str += ")";
}


// borrowed almost exactly from irc.cpp
void SayEntry::ReadOtherArgs(char *mesg, CChatDoc *doc) {
	// parse off initial parenthetical info
	char *end = strstr(mesg, ")");
	if (*mesg == '(') {
		char *start = strstr(mesg, SZGESTUREPREFIX);
		if (start && start < end) {
			start = start + strlen(SZGESTUREPREFIX);
			char *gestS = GetToken(start, &start);
			m_udi.m_chGest = atoi(gestS);
			gestS = GetToken(start, &start);
			m_udi.m_chGestE = atoi(gestS);
			gestS = GetToken(start, &start);
			m_udi.m_chGestI = atoi(gestS);
		}
		start = strstr(mesg, SZEXPRESSIONPREFIX);
		if (start && start < end) {
			start = start + strlen(SZEXPRESSIONPREFIX);
			char *exprS = GetToken(start, &start);
			m_udi.m_chExpr = atoi(exprS);
			exprS = GetToken(start, &start);
			m_udi.m_chExprE = atoi(exprS);
			exprS = GetToken(start, &start);
			m_udi.m_chExprI = atoi(exprS);
		}
		start = strstr(start, SZREQUESTEDPREFIX);
		if (start && start < end) {
			start = start + strlen(SZREQUESTEDPREFIX);
			char *reqS = GetToken(start, &start);
			m_udi.m_bbReq = atoi(reqS);
		}
		start = strstr(mesg, SZMODEPREFIX);
		if (start && start < end) {
			start = start + strlen(SZMODEPREFIX);
			char *modeS = GetToken(start, &start);
			m_udi.m_uModes = SM2BM(atoi(modeS));
		}
		start = strstr(mesg, SZCOOKEDPREFIX);
		if (start && start < end) {
			start = start + strlen(SZCOOKEDPREFIX);
			char *cookS = GetToken(start, &start);
			m_udi.m_bbCooked = atoi(cookS);
		}
		start = strstr(mesg, SZTALKTOPREFIX);
		if (start && start < end) {
			start = start + strlen(SZTALKTOPREFIX);
			m_pui->ClearTalkTos();
			while (TRUE) {
				char *name = GetToken(start, &start);
				if (!name) break;
				CUserInfo *pui = LookupPui(name, doc);
				if (pui)
					m_udi.m_talkTos.Add((DWORD) pui);
			}
		}
	}
}

SayEntry::SayEntry(CString &str, CChatDoc *doc, char cHighlightType) 
{
	CString strKeyword, strNick, strOtherArgs, strMesg, strPtr = str;

	BetweenTabs(strKeyword, strPtr);
	BetweenTabs(strNick, strPtr);
	BetweenTabs(strOtherArgs, strPtr);
	BetweenTabs(strMesg, strPtr);
	
	m_name = strdup(strNick);
	m_cHighlightType = cHighlightType;
	
	m_pui = LookupPui(strNick, doc);
	if (!m_pui)
	{
		extern CUserInfo *CIUserJoin(CUserInfo *);
		m_pui = CIUserJoin(new CUserInfo(strNick));	// got a say without a join -- fake it...
	}
	ReadOtherArgs((char *)((LPCSTR) strOtherArgs), doc);

	BOOL UnQuoteReturns(const char **);

	const char *szMesg = strMesg;
	
	BOOL bNeedFree = UnQuoteReturns(&szMesg);

	m_prgdwFormatting = new CDWordArray;
	m_mesg = strdup(SzControlLess((char*) szMesg, m_prgdwFormatting));
	m_prgdwFormatting = IdentifyURLs(m_prgdwFormatting, m_mesg);

	if (bNeedFree)
		free((void *) szMesg);
}

JoinEntry::~JoinEntry() {
	free(m_name);
	if (m_fullName) free(m_fullName);
}

void JoinEntry::Execute(int mode, CChatDoc *)
{
	extern CUserInfo *CIUserJoin(CUserInfo *);
	extern void AutoGreet(const char *);

	if (mode == HM_LIVE)
		CIUserJoin(m_pui);
	else
		ReinstallPui(m_pui, m_name);      // May need to restore old pui if someone wrote over it
	
	// display the darn thing
	if (!m_bTherePrior && theApp.m_bShowArrivals)
		if (!GetChatDoc()->m_bComicView) 
			GetTextView()->DisplayJoin(m_name, m_cHighlightType);

	if (mode == HM_LIVE && !theApp.m_bNoRefresh) {
		if (m_pui->IsSelf()) {
			extern const char *GetMyCharacter();
			extern const char *MyAvatarURL();
			// REGISB: Pbm: if channel is moderated we still send the '# Appears as' message eventhough we're a spectator
			GetChatDoc()->m_proto->ChatAnnounceNewAvatar(GetMyCharacter(), MyAvatarURL());
			if (theApp.m_bAway)
				GetChatDoc()->m_proto->ChatSetAway(TRUE, theApp.m_strAwayMessage, NULL, FALSE);
		} 
		else if (!m_bTherePrior) {
			AutoGreet(m_name);
			if (theApp.m_bAway)
				GetChatDoc()->m_proto->ChatSetAway(TRUE, theApp.m_strAwayMessage, m_pui, FALSE);
		}
	}
}

void JoinEntry::WriteSelf(CArchive &ar)
{
	CString		s;
	const char*	op = m_bTherePrior ? "ejoin" : "join";

	if (m_fullName)
		s.Format("%s\t%s\t%s\r\n", op, m_name, m_fullName);
	else
		s.Format("%s\t%s\r\n", op, m_name);
	ar.WriteString(s);
}

JoinEntry::JoinEntry(CUserInfo *pui, BOOL bPrior, char cHighlightType)
{
	CString strAttedNick;
	pui->GetAttedNick(strAttedNick, FALSE);
	m_name = strdup(strAttedNick);
	m_fullName = pui->GetFullName().IsEmpty ? NULL : strdup(pui->GetFullName());
	m_pui = pui;
	m_bTherePrior = bPrior;
	m_cHighlightType = cHighlightType;
}

JoinEntry::JoinEntry(CString &str, char cHighlightType)
{
	CString keyword, nick, fullName, ptr = str;
	BetweenTabs(keyword, ptr);
	m_bTherePrior = !strcmp(keyword, "ejoin"); 
	BetweenTabs(nick, ptr);
	m_name = strdup(nick);
	BetweenTabs(fullName, ptr);
	m_fullName = fullName.IsEmpty() ? NULL : strdup(fullName);
	m_pui = new CUserInfo(m_name, m_fullName);
	m_cHighlightType = cHighlightType;
}

PartEntry::~PartEntry() {
	free(m_name);
}

void PartEntry::Execute(int mode, CChatDoc *) {
	extern void CIUserPart(const char *);
	if (mode == HM_LIVE)
		CIUserPart(m_name);

	// display the darn thing
	if (theApp.m_bShowArrivals)
		if (!GetChatDoc()->m_bComicView) (GetTextView())->DisplayPart(m_name, m_cHighlightType);
}

void PartEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("part\t%s\r\n", m_name);
	ar.WriteString(s);
}

PartEntry::PartEntry(CString &str) {
	CString keyword, nick, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(nick, ptr);
	m_name = strdup(nick);
}

ChangeAvatarEntry::ChangeAvatarEntry(CUserInfo *pui, const char *avName, const char *avURL) {
	m_pui = pui;
	m_avName = strdup(avName);
	m_avURL = avURL != NULL ? strdup (avURL) : NULL;
	m_name = strdup(pui->GetName());
	 m_avID = 0;
} 

ChangeAvatarEntry::~ChangeAvatarEntry() {
	free(m_avName);
	free(m_avURL);
	free(m_name);
}

void ChangeAvatarEntry::Execute(int mode, CChatDoc * pDoc)
{
	void UpdateMemberListIcon(CUserInfo *);
	void UpdateTitle(CChatDoc * = NULL);

	if (!m_pui)
		m_pui = LookupPui(m_name);

	if (!pDoc->m_bComicView)
		return;  // This is a no-op unless in comics mode


	if (m_avID == 0)
	{
		m_avID = m_pui->GetAvatarID ();
		BOOL bSelectRandomIfNotFound = m_avURL == NULL || m_avID == 0;
		CAvatarX * pAv = GetAvatar3 (m_avName, m_pui, bSelectRandomIfNotFound);
		if (pAv)
			m_avID = pAv->m_avatarID;
	}


	if (m_pui == g_puiSelf)
		SetMyAvatar (m_avID);
	else
	{
		SetUserAvatarID (m_pui, m_avID);
		SetUserAvatarRealInfo (m_pui, m_avName, m_avURL, pDoc, mode == HM_LIVE);
	}

	if (mode == HM_LIVE)
	{
		m_avID = m_pui->GetAvatarID();
		UpdateMemberListIcon(m_pui);
		UpdateTitle();
	}

	// Do we need to save the URL anymore? Not if we already have the character.
	// That would just be a waste of space.
	if (m_pui->GetAvatarRealName () == NULL)
	{
		free (m_avURL);
		m_avURL = NULL;
	}
}

void ChangeAvatarEntry::WriteSelf(CArchive &ar) {
	CString s;
	// Use avatar's real name if we know it, otherwise what the user asked for.
	// The reasoning is we want to restore the same visuals that the user saw if possible.
	// May want to reconsider this, and just use m_avName.
	const char * avName;
	const char * avURL;
	if (m_avID) {
		CAvatarX * pAv = GetAvatar(m_avID);
		avName = pAv->OriginalName ();
		avURL = pAv->Url ();
	}
	else {
		avName = m_avName;
		avURL = m_avURL;
	}
	s.Format("changeavatar\t%s\t%s\t%s\r\n", m_name, avName, avURL ? avURL : "");
	ar.WriteString(s);
}

ChangeAvatarEntry::ChangeAvatarEntry(CString &str) {
	CString keyword, nick, avName, avURL, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(nick, ptr);
	m_name = strdup(nick);
	BetweenTabs(avName, ptr);
	BetweenTabs(avURL, ptr);
	m_pui = NULL;
	m_avID = GetAvatar3(avName, m_pui)->m_avatarID;	// Can create an avatar!!!
	m_avName = strdup(avName);
	m_avURL = avURL.GetLength () == 0 ? NULL : strdup(avURL);
}

GetInfoEntry::~GetInfoEntry() {
	free(m_name);
	free(m_info);
}

void GetInfoEntry::Execute(int, CChatDoc *)
{
	if (!m_pui) 
		m_pui = LookupPui(m_name);

	extern void ShowInfoX(void *, const char *, BOOL=FALSE, char=0);
	ShowInfoX((void*) m_pui, m_info);
}

void GetInfoEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("getinfo\t%s\t%s\r\n", m_name, m_info);
	ar.WriteString(s);
}

GetInfoEntry::GetInfoEntry(CString &str) {
	CString keyword, nick, info, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(nick, ptr);
	m_name = strdup(nick);
	BetweenTabs(info, ptr);
	m_info = strdup(info);
	m_pui = NULL;
}

void ComicCharacterEntry::Execute(int, CChatDoc *)
{
	if (!m_pui) 
		m_pui = LookupPui(m_name);

	// If this character is now available, don't show this message.
	LPCSTR pszRealName = m_pui->GetAvatarRealName ();
	if (pszRealName == NULL || *pszRealName == '\0') {
		return;
	}

	extern void ShowInfoX(void *, const char *, BOOL, char);
	CString str, strLink;
	str.LoadString (IDS_NO_CHAR_INFO);
	strLink.LoadString (IDS_NO_CHAR_HOTLINK);
	strLink = '\x18' + strLink + '\x18';
	ReplaceToken (str, "%1", DecodeNickForScreen (m_name));
	ReplaceToken (str, "%2", strLink);
	ShowInfoX ((void*) m_pui, str, TRUE, '\x18');
}

void ComicCharacterEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("comicchar\t%s\t%s\r\n", m_name, m_info);
	ar.WriteString(s);
}

ComicCharacterEntry::ComicCharacterEntry(CString &str) 
:
GetInfoEntry(str)
{
}

StartHistoryEntry::~StartHistoryEntry() {
	free(m_title);
	free((void *)m_avName);
	free(m_name);
}

void StartHistoryEntry::Execute(int mode, CChatDoc *)
{
	void SetMyNameNick(const char *);

	if (!GetChatDoc()->m_bArchived)
		SetMyNameNick(m_name);
	if (GetChatDoc()->m_bComicView)
	{
		GetChatDoc()->SetComicsTitle(m_title);
		SetMyAvatar(m_avName, FALSE);
	}
}

void StartHistoryEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("starthistory\t%s\t%s\t%s\r\n", m_name, m_avName, m_title);
	ar.WriteString(s);
}

StartHistoryEntry::StartHistoryEntry(const char *title, const char *avName, int rand) {
	char *GetRandomTitle();
	if (*title) m_title = strdup(title);
	else m_title = GetRandomTitle();
	m_avName = strdup(avName);
	m_name = strdup(GetMyNickName());  // name should really be passed in
	m_randStart = rand;
}

StartHistoryEntry::StartHistoryEntry(CString &str) {
	CString keyword, nick, avName, title, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(nick, ptr);
	BetweenTabs(avName, ptr);
	BetweenTabs(title, ptr);
	if (nick == "") nick.LoadString(IDS_DEFAULT_NICK);
	m_name = strdup(nick);
	m_avName = strdup(avName);
	m_title = strdup(title);
}


ChangeBackDropEntry::ChangeBackDropEntry(CString &str) {
	CString keyword, backName, backURL, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(backName, ptr);
	BetweenTabs(backURL, ptr);
	m_backName = strdup(backName);
	m_backURL = backURL.GetLength () == 0 ? NULL : strdup (backURL);
}

ChangeBackDropEntry::~ChangeBackDropEntry() {
	free(m_backName);
	free(m_backURL);
}

void ChangeBackDropEntry::Execute(int, CChatDoc *) {
	void SetBackDrop(const char *, const char *);
	if (GetChatDoc()->m_bComicView) SetBackDrop(m_backName, m_backURL);
}

void ChangeBackDropEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("backdrop\t%s\t%s\r\n", m_backName, m_backURL ? m_backURL : "");
	ar.WriteString(s);
}

// NEW STUFF


NickEntry::NickEntry(CString &str) {
	CString keyword, oldNick, newNick, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(oldNick, ptr);
	m_oldNick = strdup(oldNick);
	BetweenTabs(newNick, ptr);
	m_newNick = strdup(newNick);
}

NickEntry::~NickEntry() {
	free(m_oldNick);
	free(m_newNick);
}

void NickEntry::Execute(int mode, CChatDoc *) {
	if (mode == HM_LIVE)
		ProcessNick(GetChatDoc(), m_oldNick, m_newNick, TRUE);
	else ReinstallPui(LookupPui(m_oldNick), m_newNick);

	if (!GetChatDoc()->m_bComicView) (GetTextView()->DisplayNickChange(LookupPui(m_newNick), m_oldNick));
}

void NickEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("nick\t%s\t%s\r\n", m_oldNick, m_newNick);
	ar.WriteString(s);
}


void AddAndExecute(HistoryEntry *ent, CDocument *d) {
	CChatDoc *doc = (CChatDoc *) d;  // not passed that way for information hiding purposes
	if (!doc) doc = GetChatDoc();
	if (!doc) return;

	if (theApp.m_bPrompt && !theApp.m_bEmbedded)		// this will modify document
		doc->SetModifiedFlag(TRUE);

	doc->RegisterNewContent();

	doc->m_history.AddTail((void *) ent);

	CChatDoc *oldDoc = NULL;
	if (doc != GetChatDoc()) {    // set up state
		oldDoc = GetChatDoc();
		doc->LoadDocData();
	}

	ent->Execute(HM_LIVE, doc);

	if (oldDoc) oldDoc->LoadDocData();  // restore old state
}

void AddEntry(HistoryEntry *ent, CDocument *d) {
	((CChatDoc *)d)->m_history.AddTail((void *) ent);
	((CChatDoc *)d)->RegisterNewContent();
}

void AddEntry(CPtrList *lst, CDocument *d) {
	((CChatDoc *)d)->m_history.AddTail(lst);
	((CChatDoc *)d)->RegisterNewContent();
}



void AddToHistory(HistoryEntry *ent) {
	GetView()->GetDocument()->m_history.AddTail((void *) ent);
}

#define CONVERSATIONSTRING	"#CHATCONVERSATION"
void CChatDoc::ChatSaveConversation(CArchive &ar)
{
	CString ln;
	ln.Format("%s\r\n", CONVERSATIONSTRING);
	ar.WriteString(ln);

	POSITION pos = m_history.GetHeadPosition();
	while (pos)
	{
		HistoryEntry *entry = (HistoryEntry *) m_history.GetNext(pos);
		entry->WriteSelf(ar);
	}
}

const char *g_szUnInitChannelName = ": :";

BOOL CChatDoc::ChatLoadConversation(CArchive &ar)
{
	char key[50];
	CString strIn;

	m_bArchived = TRUE;

	ar.ReadString(strIn);
	if (strnicmp(strIn, CONVERSATIONSTRING, strlen(CONVERSATIONSTRING)) != 0)
	{
		AfxMessageBox(ID_ERR_NOT_CHATCONV);
		return FALSE;
	}

	BOOL oldRefresh = theApp.m_bNoRefresh;
	theApp.m_bNoRefresh = TRUE;

	while (ar.ReadString(strIn))
	{
		sscanf(strIn, "%s", key);
//		TRACE("strIn is %s.\n", strIn);
		if (!stricmp(key, "say"))
			AddAndExecute(new SayEntry(strIn, this), this);
		else if (!stricmp(key, "join") || !stricmp(key, "ejoin"))
			AddAndExecute(new JoinEntry(strIn), this);
		else if (!stricmp(key, "part"))
			AddAndExecute(new PartEntry(strIn), this);
		else if (!stricmp(key, "changeavatar"))
			AddAndExecute(new ChangeAvatarEntry(strIn), this);
		else if (!stricmp(key, "getinfo"))
			AddAndExecute(new GetInfoEntry(strIn), this);
		else if (!stricmp(key, "comicchar"))
			AddAndExecute(new ComicCharacterEntry(strIn), this);
		else if (!stricmp(key, "nick"))
			AddAndExecute(new NickEntry(strIn), this);
		else if (!stricmp(key, "backdrop"))
			AddAndExecute(new ChangeBackDropEntry(strIn), this);
		else if (!stricmp(key, "starthistory"))
			AddAndExecute(new StartHistoryEntry(strIn), this);
		else
		{
			// REGISB: don't know if this is a good idea. Let's talk about this - 
			//		   might be better to ignore for future releases??
			CString mesg;
			mesg.LoadString(ID_ERR_BAD_CONV_FIELD);
			VERIFY(ReplaceToken(mesg, CString("%1"), key));
			AfxMessageBox(mesg);
		}
	}

	if (!m_puiSelf)  // in case file is bad, or old format, or nick has changed
	{
		AddAndExecute(new JoinEntry(new CUserInfo(GetMyNickName())), this);  // pretend to join to set pui & g_puiSelf
		SetModifiedFlag (FALSE);
	}

	theApp.m_bNoRefresh = oldRefresh;
	if (m_bComicView)
		((CPageView *)m_view)->UpdateScroll();
	else
		// correct for bug in textview class where if it needs to scroll it will launch invisibly
		((CTextView*)m_view)->m_pRichEdit->ShowWindow(SW_SHOWNA);

	SetModifiedFlag(FALSE);  // set by AddAndExecutes, but incorrectly in this case
	m_proto->m_strChannel = g_szUnInitChannelName; // bookkeeping -- so it doesn't get reclaimed (illegal name)

	return TRUE;
}

void BetweenTabs(CString &str, CString &src) {
	const char *start = src;
	const char *tab = strchr(start, '\t');
	if (!tab) {
		str = src;
		src = "";
		return;
	}
	str = src.Left(tab-start);
	src = tab+1;
}

void UpdateMemberListIcon(CUserInfo *pui) 
{
	int AddToImageList(CUserInfo*), FindMemberListIndex(CUserInfo *, CChatDoc * = NULL);

	// new - djk
	AddToImageList(pui);		// need to add icon if necessary

	ASSERT(GetMembers());
	CListCtrl *mList = &(GetMembers()->m_MemberListBox);
	int iPos = FindMemberListIndex(pui);
//	ASSERT(iPos != -1);
//	if (iPos != -1) mList->Update(iPos);
	if (iPos != -1) mList->RedrawItems(iPos, iPos);
//	GetMembers()->Sort();
//	mList->UpdateWindow();
}


// Quotes strings according to the CTCP draft, Feb. 2, 1997.  Doesn't quote nulls since these
// are treated here as string terminators.
//
BOOL CTCPQuoteString(const char **pszStr)
{
	ASSERT(*pszStr);

	if (!strpbrk(*pszStr, " \n\r\001\\"))
		return FALSE;
	
	int			i;
	char		*szDst, *szTmpDst;
	const char	*szTmpSrcNC, *szTmpSrc = *pszStr;

	szTmpDst = szDst = (char*) malloc(strlen(*pszStr)*2+1);	// allocate string twice as long (safe) + 1 for null
	if (!szDst)
		return FALSE;

	while (*szTmpSrc != g_chEOS)
	{
		switch (*szTmpSrc)
		{
			case g_chLF:
				*szTmpDst++ = g_chLLQuoteIRCX;
				*szTmpDst++ = 'n';
				szTmpSrc++;
				break;

			case g_chCR:
				*szTmpDst++ = g_chLLQuoteIRCX;
				*szTmpDst++ = 'r';
				szTmpSrc++;
				break;

			case g_chSpace:
				*szTmpDst++ = g_chLLQuoteIRCX;
				*szTmpDst++ = g_chAtSign;
				szTmpSrc++;
				break;

			case '\001':
				*szTmpDst++ = g_chLLQuoteIRCX;
				*szTmpDst++ = '1';
				szTmpSrc++;
				break;

			default:
				if (*szTmpSrc == g_chLLQuoteIRCX)
					*szTmpDst++ = g_chLLQuoteIRCX;
				szTmpSrcNC = CharNext(szTmpSrc);
				for (i = 0; i < szTmpSrcNC - szTmpSrc; i++)
					*szTmpDst++ = *(szTmpSrc+i);
				szTmpSrc = szTmpSrcNC;
		}
	}
	*szTmpDst = g_chEOS;

	*pszStr = szDst;

	return TRUE;
}

// UnQuotes strings according to the CTCP draft, Feb. 2, 1997.  Doesn't unquote nulls since these
// are treated here as string terminators.
//
BOOL CTCPUnQuoteString(const char **pszStr)
{
	LPCTSTR	szReadNC, szRead = *pszStr;
	BOOL	bQuotedChar = FALSE;
	BOOL	bQuotedString = TRUE;
	UINT	i;

	ASSERT(*pszStr);

	// First check if the string seems to be quoted or not
	while (*szRead != g_chEOS)
	{
		if (g_chLLQuoteIRCX == *szRead)
		{
			switch (*(szRead+1))
			{
				case '1':
				case '@':
				case 'n':
				case 'r':
					bQuotedChar = TRUE;
					szRead++;
					break;

				default:
					if (g_chLLQuoteIRCX == *(szRead+1))
						szRead++;
					else
					{
						bQuotedString = FALSE;
						goto Unquote;
					}
			}
		}
		szRead = CharNext(szRead);
	}

Unquote:
	if (!bQuotedChar || !bQuotedString)
		return FALSE;

	szRead = *pszStr;

	LPTSTR szWrite, szDst;
	szDst = szWrite = (LPTSTR) malloc(strlen(*pszStr)+1);
	if (!szDst)
		return FALSE;
	
	while (*szRead != g_chEOS)
	{
		if (g_chLLQuoteIRCX == *szRead)
		{
			switch (*(szRead+1))
			{
				case 'n':
					*szWrite = g_chLF;
					break;
				case 'r':
					*szWrite = g_chCR;
					break;
				case '1':
					*szWrite = '\001';
					break;
				case '@':
					*szWrite = g_chSpace;
					break;
				default:
					if (g_chLLQuoteIRCX == *(szRead+1))
						*szWrite = g_chLLQuoteIRCX;
					else
					{
						ASSERT(FALSE);
						free(szDst);
						return FALSE;
					}
			}
			szRead++;
		}
		else
		{
			szReadNC = CharNext(szRead);
			for (i = 0; i < (UINT) (szReadNC - szRead); i++)
				*(szWrite+i) = *(szRead+i);
		}
		szWrite = CharNext(szWrite);
		szRead = CharNext(szRead);
	}

	*szWrite = g_chEOS;

	*pszStr = szDst;

	return TRUE;
}


BOOL QuoteReturns(const char **str) {
	if (!strpbrk(*str, "\n\r\t\\"))
		return FALSE;
	
	char *copy = (char *) malloc (strlen(*str)*2+1);	// allocate string twice as long (safe) + 1 for null
	const char *sptr = *str;
	char *cptr = copy;
	while (*sptr) {
		switch (*sptr) {
		case '\n':
			*cptr++ = '\\';
			*cptr++ = 'n';
			break;
		case '\r':
			*cptr++ = '\\';
			*cptr++ = 'r';
			break;
		case '\\':
			*cptr++ = '\\';
			*cptr++ = '\\';
			break;
		case '\t':
			*cptr++ = '\\';
			*cptr++ = 't';
			break;
		default:
			*cptr++ = *sptr;
			break;
		}
		sptr++;
	}
	*cptr = '\0';
	*str = copy;
	return TRUE;
}

// UnQuotes strings according to the CTCP draft, Feb. 2, 1997.  Doesn't unquote nulls since these
// are treated here as string terminators.
//
BOOL UnQuoteReturns(const char **str) {
	char *escape = strchr(*str, '\\');
	if (!escape) return FALSE;
	
	char *copy = (char *) malloc (strlen(*str)+1);
	const char *start = *str;
	char *cptr = copy;
	while (escape) {
		int len = escape - start;
		strncpy(cptr, start, len);
		cptr += len;
		switch(*(escape+1)) {
		case 'n':
			*cptr++ = '\n';
			break;
		case 'r':
			*cptr++ = '\r';
			break;
		case '\\':
			*cptr++ = '\\';
			break;
		case 't':
			*cptr++ = '\t';
			break;
		default:				// badly quoted string
			ASSERT(0);
			*cptr++ = *escape;  // just include the back slash
			break;
		}
		start = escape+1;
		if (*start) start++;		// move beyond second escaped char
		escape = strchr(start, '\\');
	}
	// handle trailing string, post last escape
	strcpy(cptr, start);
	*str = copy;
	return TRUE;
}

#ifdef DECODE_HOLIDAY
static char *g_mesgs[] = {
	"h\tabab\tHUGH\tEASTER EGG: YOU'VE FOUND IT!!!",
	"b\tFIELD",
	"j\tdjk",
	"a\tdjk\ttiki",
	"s\tdjk\t\tHi there! Welcome to our Easter Egg.",
	"s\tdjk\t\tI'm David Kurlander, Creator of Microsoft Chat.",
	"d",
	"j\tregis",
	"a\tregis\tarmando",
	"s\tregis\t\tRegis Brid here, developer double-o-seven.",
	"d",
	"j\tteo",
	"a\tteo\ttongtyed",
	"s\tteo\t\tTeoman Smith is the name, MS Chat program management is my game.",
	"d",
	"j\tjanise",
	"a\tjanise\thugh",
	"s\tjanise\t\tI'm Janise Kieffer test lead.",
	"s\tjanise\t\tI certify this program is 100% bug free. LOL!",
	"d",
	"j\tnasa",
	"a\tnasa\tanna",
	"s\tnasa\t\tI'm another bug sleuth. My name is Nasa Koski",
	"d",
	"j\tseanh",
	"a\tseanh\tlance",
	"s\tseanh\t\tNo bugs made it past me, Sean Hilbert, bug destroyer",
	"d",
	"j\tchris",
	"a\tchris\tmike",
	"s\tchris\t\tI'm Chris Hind, and I kill bugs dead.",
	"d",
	"j\tval",
	"a\tval\tsusan",
	"s\tval\t\tMS Chat ships in 23 different languages.",
	"s\tval\t\tI'm Valerie Merrill, International PM",
	"d",
	"j\tren",
	"a\tren\tdan",
	"s\tren\t\tAnd I, Rene Fuller, speak those 23 languages",
	"d",
	"j\tseanm",
	"a\tseanm\tmargaret",
	"s\tseanm\t\tI'm Sean Moore and I supervise international testing.",
	"d",
	"j\tjimc",
	"a\tjimc\txeno",
	"s\tjimc\t\tJim Campbell here.  Download my character editor!",
	"d",
	"j\tjim",
	"a\tjim\tjordan",
	"s\tjim\t\tI'm Jim Woodring, comic artist of the fantastic and the absurd.",
	"s\tjim\t\tBuy my Jim and Frank books",
	"d",
	"s\tdjk\t\tFinally, we'd like to thank a few of our friends and coworkers...",
	"s\tteo\t\tBlake Irving and John Scarrow",
	"s\tregis\t\tBrian Neligan and Bryan Chee",
	"s\tjanise\t\tSusan McLean, Christos Talanoez, and B.J. Rollison",
	"s\tnasa\t\tMascha Kroenlein, Peng Yao, and Yuuki Hashimoto",
	"s\tseanh\t\tHolly Phillips and Steve Howland",
	"s\tchris\t\tCraig Harry and Kent Cedola",
	"s\tval\t\tRick Rashid, Dan Ling, and George Robertson",
	"s\tren\t\tEric George, Ross M. Brown, and Valerie Stowell",
	"s\tteo\t\tReingard Rieger, Susan Mykytiuk and Teruyuki Mihashi",
	"s\tjanise\t\tHeiko Oberleitner, Raymond Hung, and Toru Matsuda",
	"s\tseanm\t\tAideen Quin, Bianca Genari, and Brenda Walsh",
	"s\tjimc\t\tEduardo Rohde, Eirin O'Connell and Ellen O'Neill",
	"s\tjim\t\tEmma Skelly, Genevieve Kolakowski, and Igor Halama",
	"s\tdjk\t\tJan Rustenhoven, Jorge Estevez, and Lorraine O'Brien",
	"s\tregis\t\tMaeve McKenna, Michael Power, and Oriana Bucci",
	"s\tteo\t\tZita Harkin, Titan Chou, and Ya-Huei-Lana Peng",
	"s\tjanise\t\tChing-Hsien Chen, Susan Lee, and Hyo Kyoung Kim",
	"s\tnasa\t\tJi Eaon Bae, Wan Young Moon, and Yong-Zhong Wang",
	"s\tseanh\t\tJun-Christina Li, Hiroko Kuwakino, and Teri Kelsey",
	"s\tchris\t\tJohn Muller, Kevin Otnes, and Diane Stielstra",
	"s\tval\t\tYuji Yoneshige and David Kennedy",
	"s\tjimc\t\tDavid Salesin and Tim Skelly",
	"s\tdjk\t\tRachel Iwamoto and Scott Speyer",
	"s\tren\t\tYong-Sheng Yang, Rand Renfroe and Ramu Movva",
	"s\tjanise\t\tJean Saylor and David R. Williamson",
	"s\tseanm\t\tJohn Bowser and Mitch Smith",
	"s\tjimc\t\t...Mick Jagger too",
	"s\tjim\t\tAnd of course all our users.",
	"s\tteo\t\tThis easter egg has gone on far too long.",
	"s\tdjk\t\tNow it's time for us to watch you dance.",
	NULL
};
#endif DECODE_HOLIDAY

#ifndef DECODE_HOLIDAY
static char *g_mesgs [] = {
	"\202\343\213\210\213\210\343\242\277\255\242\343\257\253\271\276\257\270\312\257\255\255\320\312\263\245\277\315\274\257\312\254\245\277\244\256\312\243\276\313\313\313",
	"\210\343\254\243\257\246\256",
	"\200\343\216\200\201",
	"\213\343\216\200\201\343\236\203\201\203",
	"\231\343\216\200\201\343\343\242\203\312\236\202\217\230\217\313\312\275\217\206\211\205\207\217\312\236\205\312\205\237\230\312\257\213\231\236\217\230\312\257\215\215\304",
	"\231\343\216\200\201\343\343\243\315\207\312\256\213\234\203\216\312\241\237\230\206\213\204\216\217\230\306\312\251\230\217\213\236\205\230\312\205\214\312\247\203\211\230\205\231\205\214\236\312\251\202\213\236\304",
	"\216",
	"\200\343\230\217\215\203\231",
	"\213\343\230\217\215\203\231\343\213\230\207\213\204\216\205",
	"\231\343\230\217\215\203\231\343\343\270\217\215\203\231\312\250\230\203\216\312\202\217\230\217\306\312\216\217\234\217\206\205\232\217\230\312\216\205\237\210\206\217\307\205\307\231\217\234\217\204\304",
	"\216",
	"\200\343\236\217\205",
	"\213\343\236\217\205\343\236\205\204\215\236\223\217\216",
	"\231\343\236\217\205\343\343\276\217\205\207\213\204\312\271\207\203\236\202\312\203\231\312\236\202\217\312\204\213\207\217\306\312\247\271\312\251\202\213\236\312\232\230\205\215\230\213\207\312\207\213\204\213\215\217\207\217\204\236\312\203\231\312\207\223\312\215\213\207\217\304",
	"\216",
	"\200\343\200\213\204\203\231\217",
	"\213\343\200\213\204\203\231\217\343\202\237\215\202",
	"\231\343\200\213\204\203\231\217\343\343\243\315\207\312\240\213\204\203\231\217\312\241\203\217\214\214\217\230\312\236\217\231\236\312\206\217\213\216\304",
	"\231\343\200\213\204\203\231\217\343\343\243\312\211\217\230\236\203\214\223\312\236\202\203\231\312\232\230\205\215\230\213\207\312\203\231\312\333\332\332\317\312\210\237\215\312\214\230\217\217\304\312\246\245\246\313",
	"\216",
	"\200\343\204\213\231\213",
	"\213\343\204\213\231\213\343\213\204\204\213",
	"\231\343\204\213\231\213\343\343\243\315\207\312\213\204\205\236\202\217\230\312\210\237\215\312\231\206\217\237\236\202\304\312\247\223\312\204\213\207\217\312\203\231\312\244\213\231\213\312\241\205\231\201\203",
	"\216",
	"\200\343\231\217\213\204\202",
	"\213\343\231\217\213\204\202\343\206\213\204\211\217",
	"\231\343\231\217\213\204\202\343\343\244\205\312\210\237\215\231\312\207\213\216\217\312\203\236\312\232\213\231\236\312\207\217\306\312\271\217\213\204\312\242\203\206\210\217\230\236\306\312\210\237\215\312\216\217\231\236\230\205\223\217\230",
	"\216",
	"\200\343\211\202\230\203\231",
	"\213\343\211\202\230\203\231\343\207\203\201\217",
	"\231\343\211\202\230\203\231\343\343\243\315\207\312\251\202\230\203\231\312\242\203\204\216\306\312\213\204\216\312\243\312\201\203\206\206\312\210\237\215\231\312\216\217\213\216\304",
	"\216",
	"\200\343\234\213\206",
	"\213\343\234\213\206\343\231\237\231\213\204",
	"\231\343\234\213\206\343\343\247\271\312\251\202\213\236\312\231\202\203\232\231\312\203\204\312\330\331\312\216\203\214\214\217\230\217\204\236\312\206\213\204\215\237\213\215\217\231\304",
	"\231\343\234\213\206\343\343\243\315\207\312\274\213\206\217\230\203\217\312\247\217\230\230\203\206\206\306\312\243\204\236\217\230\204\213\236\203\205\204\213\206\312\272\247",
	"\216",
	"\200\343\230\217\204",
	"\213\343\230\217\204\343\216\213\204",
	"\231\343\230\217\204\343\343\253\204\216\312\243\306\312\270\217\204\217\312\254\237\206\206\217\230\306\312\231\232\217\213\201\312\236\202\205\231\217\312\330\331\312\206\213\204\215\237\213\215\217\231",
	"\216",
	"\200\343\231\217\213\204\207",
	"\213\343\231\217\213\204\207\343\207\213\230\215\213\230\217\236",
	"\231\343\231\217\213\204\207\343\343\243\315\207\312\271\217\213\204\312\247\205\205\230\217\312\213\204\216\312\243\312\231\237\232\217\230\234\203\231\217\312\203\204\236\217\230\204\213\236\203\205\204\213\206\312\236\217\231\236\203\204\215\304",
	"\216",
	"\200\343\200\203\207\211",
	"\213\343\200\203\207\211\343\222\217\204\205",
	"\231\343\200\203\207\211\343\343\240\203\207\312\251\213\207\232\210\217\206\206\312\202\217\230\217\304\312\312\256\205\235\204\206\205\213\216\312\207\223\312\211\202\213\230\213\211\236\217\230\312\217\216\203\236\205\230\313",
	"\216",
	"\200\343\200\203\207",
	"\213\343\200\203\207\343\200\205\230\216\213\204",
	"\231\343\200\203\207\343\343\243\315\207\312\240\203\207\312\275\205\205\216\230\203\204\215\306\312\211\205\207\203\211\312\213\230\236\203\231\236\312\205\214\312\236\202\217\312\214\213\204\236\213\231\236\203\211\312\213\204\216\312\236\202\217\312\213\210\231\237\230\216\304",
	"\231\343\200\203\207\343\343\250\237\223\312\207\223\312\240\203\207\312\213\204\216\312\254\230\213\204\201\312\210\205\205\201\231",
	"\216",
	"\231\343\216\200\201\343\343\254\203\204\213\206\206\223\306\312\235\217\315\216\312\206\203\201\217\312\236\205\312\236\202\213\204\201\312\213\312\214\217\235\312\205\214\312\205\237\230\312\214\230\203\217\204\216\231\312\213\204\216\312\211\205\235\205\230\201\217\230\231\304\304\304",
	"\231\343\236\217\205\343\343\250\206\213\201\217\312\243\230\234\203\204\215\312\213\204\216\312\240\205\202\204\312\271\211\213\230\230\205\235",
	"\231\343\230\217\215\203\231\343\343\250\230\203\213\204\312\244\217\206\203\215\213\204\312\213\204\216\312\250\230\223\213\204\312\251\202\217\217",
	"\231\343\200\213\204\203\231\217\343\343\271\237\231\213\204\312\247\211\246\217\213\204\306\312\251\202\230\203\231\236\205\231\312\276\213\206\213\204\205\217\220\306\312\213\204\216\312\250\304\240\304\312\270\205\206\206\203\231\205\204",
	"\231\343\204\213\231\213\343\343\247\213\231\211\202\213\312\241\230\205\217\204\206\217\203\204\306\312\272\217\204\215\312\263\213\205\306\312\213\204\216\312\263\237\237\201\203\312\242\213\231\202\203\207\205\236\205",
	"\231\343\231\217\213\204\202\343\343\242\205\206\206\223\312\272\202\203\206\206\203\232\231\312\213\204\216\312\271\236\217\234\217\312\242\205\235\206\213\204\216",
	"\231\343\211\202\230\203\231\343\343\251\230\213\203\215\312\242\213\230\230\223\312\213\204\216\312\241\217\204\236\312\251\217\216\205\206\213",
	"\231\343\234\213\206\343\343\270\203\211\201\312\270\213\231\202\203\216\306\312\256\213\204\312\246\203\204\215\306\312\213\204\216\312\255\217\205\230\215\217\312\270\205\210\217\230\236\231\205\204",
	"\231\343\230\217\204\343\343\257\230\203\211\312\255\217\205\230\215\217\306\312\270\205\231\231\312\247\304\312\250\230\205\235\204\306\312\213\204\216\312\274\213\206\217\230\203\217\312\271\236\205\235\217\206\206",
	"\231\343\236\217\205\343\343\270\217\203\204\215\213\230\216\312\270\203\217\215\217\230\306\312\271\237\231\213\204\312\247\223\201\223\236\203\237\201\312\213\204\216\312\276\217\230\237\223\237\201\203\312\247\203\202\213\231\202\203",
	"\231\343\200\213\204\203\231\217\343\343\242\217\203\201\205\312\245\210\217\230\206\217\203\236\204\217\230\306\312\270\213\223\207\205\204\216\312\242\237\204\215\306\312\213\204\216\312\276\205\230\237\312\247\213\236\231\237\216\213",
	"\231\343\231\217\213\204\207\343\343\253\203\216\217\217\204\312\273\237\203\204\306\312\250\203\213\204\211\213\312\255\217\204\213\230\203\306\312\213\204\216\312\250\230\217\204\216\213\312\275\213\206\231\202",
	"\231\343\200\203\207\211\343\343\257\216\237\213\230\216\205\312\270\205\202\216\217\306\312\257\203\230\203\204\312\245\315\251\205\204\204\217\206\206\312\213\204\216\312\257\206\206\217\204\312\245\315\244\217\203\206\206",
	"\231\343\200\203\207\343\343\257\207\207\213\312\271\201\217\206\206\223\306\312\255\217\204\217\234\203\217\234\217\312\241\205\206\213\201\205\235\231\201\203\306\312\213\204\216\312\243\215\205\230\312\242\213\206\213\207\213",
	"\231\343\216\200\201\343\343\240\213\204\312\270\237\231\236\217\204\202\205\234\217\204\306\312\240\205\230\215\217\312\257\231\236\217\234\217\220\306\312\213\204\216\312\246\205\230\230\213\203\204\217\312\245\315\250\230\203\217\204",
	"\231\343\230\217\215\203\231\343\343\247\213\217\234\217\312\247\211\241\217\204\204\213\306\312\247\203\211\202\213\217\206\312\272\205\235\217\230\306\312\213\204\216\312\245\230\203\213\204\213\312\250\237\211\211\203",
	"\231\343\236\217\205\343\343\260\203\236\213\312\242\213\230\201\203\204\306\312\276\203\236\213\204\312\251\202\205\237\306\312\213\204\216\312\263\213\307\242\237\217\203\307\246\213\204\213\312\272\217\204\215",
	"\231\343\200\213\204\203\231\217\343\343\251\202\203\204\215\307\242\231\203\217\204\312\251\202\217\204\306\312\271\237\231\213\204\312\246\217\217\306\312\213\204\216\312\242\223\205\312\241\223\205\237\204\215\312\241\203\207",
	"\231\343\204\213\231\213\343\343\240\203\312\257\213\205\204\312\250\213\217\306\312\275\213\204\312\263\205\237\204\215\312\247\205\205\204\306\312\213\204\216\312\263\205\204\215\307\260\202\205\204\215\312\275\213\204\215",
	"\231\343\231\217\213\204\202\343\343\240\237\204\307\251\202\230\203\231\236\203\204\213\312\246\203\306\312\242\203\230\205\201\205\312\241\237\235\213\201\203\204\205\306\312\213\204\216\312\276\217\230\203\312\241\217\206\231\217\223",
	"\231\343\211\202\230\203\231\343\343\240\205\202\204\312\247\237\206\206\217\230\306\312\241\217\234\203\204\312\245\236\204\217\231\306\312\213\204\216\312\256\203\213\204\217\312\271\236\203\217\206\231\236\230\213",
	"\231\343\234\213\206\343\343\263\237\200\203\312\263\205\204\217\231\202\203\215\217\312\213\204\216\312\256\213\234\203\216\312\241\217\204\204\217\216\223",
	"\231\343\200\203\207\211\343\343\256\213\234\203\216\312\271\213\206\217\231\203\204\312\213\204\216\312\276\203\207\312\271\201\217\206\206\223",
	"\231\343\216\200\201\343\343\270\213\211\202\217\206\312\243\235\213\207\205\236\205\312\213\204\216\312\271\211\205\236\236\312\271\232\217\223\217\230",
	"\231\343\230\217\204\343\343\263\205\204\215\307\271\202\217\204\215\312\263\213\204\215\306\312\270\213\204\216\312\270\217\204\214\230\205\217\312\213\204\216\312\270\213\207\237\312\247\205\234\234\213",
	"\231\343\200\213\204\203\231\217\343\343\240\217\213\204\312\271\213\223\206\205\230\312\213\204\216\312\256\213\234\203\216\312\270\304\312\275\203\206\206\203\213\207\231\205\204",
	"\231\343\231\217\213\204\207\343\343\240\205\202\204\312\250\205\235\231\217\230\312\213\204\216\312\247\203\236\211\202\312\271\207\203\236\202",
	"\231\343\200\203\207\211\343\343\304\304\304\247\203\211\201\312\240\213\215\215\217\230\312\236\205\205",
	"\231\343\200\203\207\343\343\253\204\216\312\205\214\312\211\205\237\230\231\217\312\213\206\206\312\205\237\230\312\237\231\217\230\231\304",
	"\231\343\236\217\205\343\343\276\202\203\231\312\217\213\231\236\217\230\312\217\215\215\312\202\213\231\312\215\205\204\217\312\205\204\312\214\213\230\312\236\205\205\312\206\205\204\215\304",
	"\231\343\216\200\201\343\343\244\205\235\312\203\236\315\231\312\236\203\207\217\312\214\205\230\312\237\231\312\236\205\312\235\213\236\211\202\312\223\205\237\312\216\213\204\211\217\304",
	NULL
};
#endif !def DECODE_HOLIDAY

static char **g_nextLine = NULL;
extern CChatDoc *g_easterDoc = NULL;

#ifdef DECODE_HOLIDAY
void DoEncode() {
	FILE *fp;
	VERIFY(fp = fopen("strings.txt", "w"));
	fprintf(fp, "static char *g_mesgs [] = {\n");
	g_nextLine = g_mesgs;
	while (*g_nextLine) {
		fprintf(fp, "\t\"");
		char *bptr = *g_nextLine;
		while (*bptr != '\0') {
			unsigned char c = (*bptr ^ 0xea) & 0xff;
			fprintf(fp, "\\%03o", c);
			bptr++;
		}
		fprintf(fp, "\"");
		g_nextLine++;
		fprintf(fp, ",\n");
	}
	fprintf(fp, "\tNULL\n};");
	fclose(fp);
}
#endif DECODE_HOLIDAY

void DecodeChars(const char *encoded, CString &str) {
	char *decoded = strdup(encoded);
	char *bptr = decoded;
	while (*bptr) {
		*bptr = (*bptr ^ 0xea) & 0xff;
		bptr++;
	}
	str = decoded;
	free(decoded);
}

BOOL TextEntryTest() {
	// key test...
	if (!((GetKeyState(VK_SHIFT)&0x8000) && (GetKeyState(VK_CONTROL)&0x8000)))
		return FALSE;

	char buff[30];
	CChatDoc *doc = GetChatDoc();
	if (!doc) return FALSE;
	CSayWnd *swnd = (CSayWnd *) doc->m_sayWnd;
	HWND hWnd = swnd->GetSayEdit();
	GetWindowText(hWnd, buff, sizeof(buff));
	buff[sizeof(buff)-1] = '\0';  // careful
	return (strcmp(buff, "CanThereBMore?") == 0);
}


BOOL DoEasterEgg() {
	CChatDoc *LookupDoc(const char *);
	void OnEaster(), ChatSetChannel(const char *);
	CRoomInfo *NewDefaultProto(CChatDoc *);

#ifdef DECODE_HOLIDAY
	DoEncode(); return TRUE;
#endif
	if (!TextEntryTest()) return FALSE;
	int iStatus = GetDefaultProto()->GetConnectionStatus();
	if (!theApp.m_bFoundArt) return FALSE;
	CString strChanName, strSavedChan = GetMyChannel();
	DecodeChars("\251\270\257\256\243\276\271", strChanName);
	bProcessAddChannel(strChanName, NewDefaultProto(NULL), &g_nCXKeepServer, &g_bCXPrompt);
	ChatSetChannel(strSavedChan);  // restore value destroyed by bProcessAddChannel
	g_easterDoc = LookupDoc(strChanName);
	if (g_easterDoc) {
		g_easterDoc->OnViewComics();
		if (iStatus == CX_DISCONNECTED || iStatus == CX_CONNECTING) g_easterDoc->m_proto->SetConnectionStatus(CX_DISCONNECTED);
		else if (iStatus == CX_INCHANNEL || iStatus == CX_NOCHANNEL) g_easterDoc->m_proto->SetConnectionStatus(CX_NOCHANNEL);
		g_nextLine = g_mesgs;
		OnEaster();
	}
	return TRUE;
}


void OnEaster() {	
	char key[50];
	static CUserInfo *lastPui = NULL;
	void Dance(CChatDoc *, CUserInfo *);
	CChatDoc *doc = g_easterDoc;

	if (GetFrame()) GetFrame()->KillTimer(EASTER_TIMER);
	if (!doc) return; // doc was reclaimed
	while (*g_nextLine) {
		CString strIn;
		DecodeChars(*g_nextLine, strIn);
//		CString strIn = *g_nextLine;
		sscanf(strIn, "%s", key);
		if (!stricmp(key, "s")) {
			SayEntry *sayEntry = new SayEntry(strIn, doc);
			AddAndExecute(sayEntry, doc);
			lastPui = sayEntry->m_pui;
			if (GetFrame()) GetFrame()->SetTimer(EASTER_TIMER, 2000, NULL);
			g_nextLine++;
			return;
		} else if (!stricmp(key, "j")/* || !stricmp(key, "ejoin")*/)
			AddAndExecute(new JoinEntry(strIn), doc);
//		else if (!stricmp(key, "part"))
//			AddAndExecute(new PartEntry(strIn), doc);
		else if (!stricmp(key, "a"))
			AddAndExecute(new ChangeAvatarEntry(strIn), doc);
//		else if (!stricmp(key, "getinfo"))
//			AddAndExecute(new GetInfoEntry(strIn), doc);
//		else if (!stricmp(key, "nick"))
//			AddAndExecute(new NickEntry(strIn), doc);
		else if (!stricmp(key, "b"))
			AddAndExecute(new ChangeBackDropEntry(strIn), doc);
		else if (!stricmp(key, "h")) {
			StartHistoryEntry *histEnt = new StartHistoryEntry(strIn);
			doc->SetComicsTitle2(histEnt->m_title);
//			AddAndExecute(histEnt, doc);
			delete histEnt;
			if (!g_puiSelf)  // in case file is bad, or old format
				AddAndExecute(new JoinEntry(new CUserInfo(GetMyNickName())), doc);  // pretend to join to set pui & g_puiSelf
		}
		else if (!stricmp(key, "d")) {
			AddAndExecute(new SayEntry(lastPui, "Watch me dance", (CDWordArray *) -1), doc);
			Dance(doc, lastPui);
		}
		g_nextLine++;
	}
		
	// *g_nextLine must be null now

	Dance(doc, g_puiSelf);
	Dance(doc, g_puiSelf);

	if (!doc->m_bComicView)
		((CTextView*)doc->m_view)->m_pRichEdit->ShowWindow(SW_SHOWNA);

	doc->SetModifiedFlag(FALSE);  // set by AddAndExecutes, but incorrectly in this case
}


void Dance(CChatDoc *doc, CUserInfo *pui) {
	void SequentialExpression(CUserInfo *, int);
	for (int i = 0; i < 50; i++) {
		int avID = pui->GetAvatarID();
		CAvatarX *av;
		if (avID && (av = GetAvatar(avID))) {
			av->SetSequential(pui, i);
			AddAndExecute(new SayEntry(pui, "<Chr>", (CDWordArray *)-1), doc);
			doc->m_view->UpdateWindow();
//			Sleep(10);
		}
	}
}




void CChatDoc::OnClearHistory() 
{
	// ShankuN - no longer do this.
	//if (!SaveModified()) return;  // give user a chance to save, if option set

	if (AfxMessageBox (IDS_CLEARHISTORY_PROMPT, MB_ICONQUESTION | MB_YESNO) != IDYES)
		return;

	DestroyPages();				// delete the pages

	if (m_bComicView)
		((CPageView *)GetView())->ResetExistingPanels(TRUE);
	else if (m_view) 
		((CTextView *)m_view)->ClearTextView();

	m_view->RedrawWindow();  // do immediate refresh so that people don't see pause

	// Note: we collect all avatar entries of active participants, and append them to the end.  Perhaps
	//  we should instead save info on the requested avatar in the pui, and then simply create a
	//  changeavatar entry for each.  This would be simpler, but would require a new CString in each CUserInfo.
	//
	// 02/24/98 ShankuN - changed around so we don't save duplicates.
	CPtrList avEnts;		// collect list of changeavatar entries (must save)
	POSITION pos = m_history.GetTailPosition();
	POSITION pos2;
	while (pos) {
		POSITION prevPos = pos;
		HistoryEntry *entry = (HistoryEntry *) m_history.GetPrev (pos);
		if (entry->GetType() == HT_AVATARENTRY) {
			ChangeAvatarEntry *avEnt = (ChangeAvatarEntry *) entry;
			if (!avEnt->m_pui->IsDeparted()) {
				pos2 = avEnts.GetHeadPosition ();
				while (pos2) {
					if (((ChangeAvatarEntry *)avEnts.GetNext (pos2))->m_pui == avEnt->m_pui)
						break;
				}
				if (!pos2) {
					m_history.RemoveAt(prevPos);
					avEnts.AddTail(entry);
				}
			}
		}
	}
	
	DestroyHistory();		// delete last chat room's dialog history
	InitHistory();

	// add ejoins for existing members...
	void AddEntry(HistoryEntry *, CDocument *), AddEntry(CPtrList *, CDocument *);
	pos = m_allChannelPuis.GetHeadPosition();
	while (pos) {
		CUserInfo *pui = (CUserInfo *) m_allChannelPuis.GetNext(pos);
		if (!pui->IsDeparted())
			AddEntry(new JoinEntry(pui), this);
	}

	AddEntry(&avEnts, this);	// add change avatar entries
}
