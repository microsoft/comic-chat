#include "stdafx.h"

#include "binddoc.h"
#include "chatdoc.h"
#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "script.h"
#include "pageview.h"
#include "ui.h"
#include "userinfo.h"
#include "histent.h"
#include "chatprot.h"
#include "memblst.h"
#include "resource.h"
#include "chat.h"
#include "textview.h"

extern CChatApp theApp;

void BetweenTabs(CString&, CString&);
CUserInfo *LookupPui(const char *);
CPtrArray *IdentifyURLs(const char *mesg);
void DestroyLinks(CPtrArray **m_links);
void ProcessNick(CUserInfo *, const char *, BOOL = TRUE);
void ReinstallPui(CUserInfo *pui, const char *newNick);
const char *GetMyName();
extern CUserInfo *puiSelf;

SayEntry::~SayEntry() {
	free((void *) m_mesg);
	free((void *) m_name);
	if (m_links) DestroyLinks(&m_links);
}

SayEntry::SayEntry(CUserInfo *pui, const char *mesg, BOOL cooked, BYTE mode, BYTE expr, BYTE gest, BYTE exprE, BYTE exprI,
				   BYTE gestE, BYTE gestI, BOOL req, CWordArray &talkTos) {
	m_pui = pui;
	m_name = strdup(pui->GetName());
	m_mesg = strdup(mesg);
	m_cooked = cooked;
	m_mode = mode;
	m_expr = expr;
	m_gest = gest;
	m_exprE = exprE;
	m_exprI = exprI;
	m_gestE = gestE;
	m_gestI = gestI;
	m_req = req;
	if (theApp.m_bComicView) {  // in the future we should encode this anyway in case they switch views
		int upper = talkTos.GetUpperBound();
		for (int i = 0; i <= upper; i++) {
			CAvatarX *av = GetAvatar(talkTos[i]);
			if (av) {
				CUserInfo *pui = (CUserInfo *) av->m_userInfo;
				if (pui && !pui->IsDeparted())	// only add them if they are there!	
					m_talkTos.Add(talkTos[i]);
			}
		}
	}
	m_links = IdentifyURLs(m_mesg);
}

void SayEntry::Execute(int) {
	if (!theApp.m_bComicView) {
		GetTextView()->TextLine(m_pui, m_mesg, m_mode, m_cooked, m_links);
		return;
	}

	CAvatarX *av = GetAvatar(m_pui->GetAvatarID());
	if (!av) return;  // should never happen

	if (m_cooked) {
		if (!(av->m_flags & OTHERMAPPED))
			av->SetIndices(m_expr, m_gest, m_req);
		else {
			void BytesToEmotion(CEmotion &em, BYTE emotion, BYTE intensity);
			CEmotion expr;
			CEmotion gest;
			BytesToEmotion(expr, m_exprE, m_exprI);
			BytesToEmotion(gest, m_gestE, m_gestI);
			av->SetEmotions(expr, gest);
		}
	}

	int upper = m_talkTos.GetUpperBound();
	av->ClearTalkTos();
	for (int i = 0; i <= upper; i++) {
			av->SelectTalkTo(m_talkTos[i], TRUE);
	}

	GetView()->GetDocument()->ProcessLine(m_pui->GetAvatarID(), m_mesg, m_mode, m_cooked, m_links);
}


void SayEntry::WriteSelf(CArchive &ar) {
	CString s;
	CString otherArgs;
	FormatOtherArgs(otherArgs);
	s.Format("say\t%s\t%s\t%s\r\n", m_name, otherArgs, m_mesg);
	ar.WriteString(s);
}

void SayEntry::FormatOtherArgs(CString &str) {
	str.Format("(G:%d %d %d E:%d %d %d R:%d M:%d", m_gest, m_gestE, m_gestI,
				m_expr, m_exprE, m_exprI, m_req, m_mode); 
	int upper = m_talkTos.GetUpperBound();
	if (upper >= 0) {
		str += " T:";
		for (int i = 0; i <= upper; i++) {
			CAvatarX *av = GetAvatar(m_talkTos[i]);
			str += ((CUserInfo *)av->m_userInfo)->GetName();
			str += ""; 
		}
	}
	str += ")";
}

#define GESTUREPREFIX "G:"
#define EXPRESSIONPREFIX "E:"
#define REQUESTEDPREFIX "R:"
#define MODEPREFIX "M:"
#define COOKEDPREFIX "C:"
#define TALKTOPREFIX "T:"

// borrowed almost exactly from irc.cpp
void SayEntry::ReadOtherArgs(char *mesg) {
	// parse off initial parenthetical info
	char *GetToken(char *, char **);
	char *end = strstr(mesg, ")");
	if (*mesg == '(') {
		char *start = strstr(mesg, GESTUREPREFIX);
		if (start && start < end) {
			start = start + strlen(GESTUREPREFIX);
			char *gestS = GetToken(start, &start);
			m_gest = atoi(gestS);
			gestS = GetToken(start, &start);
			m_gestE = atoi(gestS);
			gestS = GetToken(start, &start);
			m_gestI = atoi(gestS);
		}
		start = strstr(mesg, EXPRESSIONPREFIX);
		if (start && start < end) {
			start = start + strlen(EXPRESSIONPREFIX);
			char *exprS = GetToken(start, &start);
			m_expr = atoi(exprS);
			exprS = GetToken(start, &start);
			m_exprE = atoi(exprS);
			exprS = GetToken(start, &start);
			m_exprI = atoi(exprS);
		}
		start = strstr(start, REQUESTEDPREFIX);
		if (start && start < end) {
			start = start + strlen(REQUESTEDPREFIX);
			char *reqS = GetToken(start, &start);
			m_req = atoi(reqS);
		}
		start = strstr(mesg, MODEPREFIX);
		if (start && start < end) {
			start = start + strlen(MODEPREFIX);
			char *modeS = GetToken(start, &start);
			m_mode = atoi(modeS);
		}
		start = strstr(mesg, COOKEDPREFIX);
		if (start && start < end) {
			start = start + strlen(COOKEDPREFIX);
			char *cookS = GetToken(start, &start);
			m_cooked = atoi(cookS);
		}
		start = strstr(mesg, TALKTOPREFIX);
		if (start && start < end) {
			start = start + strlen(TALKTOPREFIX);
			CAvatarX *av = GetAvatar(m_pui->GetAvatarID());
			if (!av) return; // probably in text mode, so av doesn't make sense
			av->ClearTalkTos();
			while (TRUE) {
				char *name = GetToken(start, &start);
				if (!name) break;
				CUserInfo *pui = LookupPui(name);
				if (pui) {
					m_talkTos.Add(pui->GetAvatarID());
				}
			}
		}
	}
}

SayEntry::SayEntry(CString &str) {
	CString keyword, nick, otherArgs, mesg, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(nick, ptr);
	BetweenTabs(otherArgs, ptr);
	BetweenTabs(mesg, ptr);
	m_name = strdup(nick);
	m_pui = LookupPui(nick);
	if (!m_pui) {
		extern void CIUserJoin(const char *);
		CIUserJoin(nick);	// got a say without a join -- fake it...
		m_pui = LookupPui(nick);
	}
	ReadOtherArgs((char *)((LPCSTR) otherArgs));
	m_mesg = strdup(mesg);
	m_links = IdentifyURLs(mesg);
}

JoinEntry::~JoinEntry() {
	free(m_name);
}

void JoinEntry::Execute(int mode) {
	extern void CIUserJoin(const char *);
	if (mode == HM_LIVE) {
		CIUserJoin(m_name);
		m_pui = LookupPui(m_name);
	} else {
		ReinstallPui(m_pui, m_name);      // May need to restore old pui if someone wrote over it
	}
}

void JoinEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("join\t%s\r\n", m_name);
	ar.WriteString(s);
}

JoinEntry::JoinEntry(CString &str) {
	CString keyword, nick, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(nick, ptr);
	m_name = strdup(nick);
	m_pui = NULL;
}

PartEntry::~PartEntry() {
	free(m_name);
}

void PartEntry::Execute(int mode) {
	extern void CIUserPart(const char *);
	if (mode == HM_LIVE)
		CIUserPart(m_name);
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

ChangeAvatarEntry::ChangeAvatarEntry(CUserInfo *pui, const char *avName) {
	m_pui = pui;
	m_avName = strdup(avName);
	m_name = strdup(pui->GetName());
	if (theApp.m_bComicView)
		m_avID = GetAvatar3(avName, pui)->m_avatarID;  // Can load an avatar!!!
	else m_avID = 0;
} 

ChangeAvatarEntry::~ChangeAvatarEntry() {
	free(m_avName);
	free(m_name);
}

void ChangeAvatarEntry::Execute(int mode) {
	extern CUserInfo *puiSelf;
	void SetUserAvatarID(CUserInfo *pui, unsigned short avID);
	void UpdateMemberListIcon(CUserInfo *);
	void UpdateTitle();

	if (!theApp.m_bComicView) return;		// This is a no-op unless in comics mode

	if (!m_avID) {							// must have been created in text mode
		m_avID = GetAvatar3(m_avName, m_pui)->m_avatarID;
	}

	if (mode == HM_LIVE) {
		if (m_pui == puiSelf) {
			SetMyAvatar(m_avID);
		} else {
			SetUserAvatarID(m_pui, m_avID);
		}
		m_avID = m_pui->GetAvatarID();
		UpdateMemberListIcon(m_pui);
		UpdateTitle();
	} else {
		if (m_pui == puiSelf) {
			SetMyAvatar(m_avID);
		} else {
			SetUserAvatarID(m_pui, m_avID);
		}
	}
}

void ChangeAvatarEntry::WriteSelf(CArchive &ar) {
	CString s;
	// Use avatar's real name if we know it, otherwise what the user asked for.
	// The reasoning is we want to restore the same visuals that the user saw if possible.
	// May want to reconsider this, and just use m_avName.
	const char *avName = m_avID ? GetAvatar(m_avID)->OriginalName() : m_avName;
	s.Format("changeavatar\t%s\t%s\r\n", m_name, avName);
	ar.WriteString(s);
}

ChangeAvatarEntry::ChangeAvatarEntry(CString &str) {
	CString keyword, nick, avName, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(nick, ptr);
	m_name = strdup(nick);
	BetweenTabs(avName, ptr);
	m_pui = LookupPui(nick);
	m_avID = GetAvatar3(avName, m_pui)->m_avatarID;	// Can create an avatar!!!
	m_avName = strdup(avName);
}

GetInfoEntry::~GetInfoEntry() {
	free(m_name);
	free(m_info);
}

void GetInfoEntry::Execute(int) {
	extern void ShowInfoX(void *, const char *);
	ShowInfoX((void *)m_pui, m_info);
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
	m_pui = LookupPui(nick);
}

StartHistoryEntry::~StartHistoryEntry() {
	free(m_title);
	free((void *)m_avName);
	free(m_name);
}

void StartHistoryEntry::Execute(int) {
	void SetMyName(const char *);
	void SetComicsTitle(char *);

	SetMyName(m_name);
	if (theApp.m_bComicView) {
		SetComicsTitle(m_title);
		SetMyAvatar(m_avName);
	}
}

void StartHistoryEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("starthistory\t%s\t%s\t%s\r\n", m_name, m_avName, m_title);
	ar.WriteString(s);
}

StartHistoryEntry::StartHistoryEntry(const char *title, const char *avName, int rand) {
	extern const char *GetMyName();
	char *GetRandomTitle();
	if (*title) m_title = strdup(title);
	else m_title = GetRandomTitle();
	m_avName = strdup(avName);
	m_name = strdup(GetMyName());  // name should really be passed in
	m_randStart = rand;
}

StartHistoryEntry::StartHistoryEntry(CString &str) {
	CString keyword, nick, avName, title, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(nick, ptr);
	BetweenTabs(avName, ptr);
	BetweenTabs(title, ptr);
	if (nick == "") nick = "Anonymous";
	m_name = strdup(nick);
	m_avName = strdup(avName);
	m_title = strdup(title);
}

ChangeBackDropEntry::ChangeBackDropEntry(const char *backName) {
	m_backName = strdup(backName);
}

ChangeBackDropEntry::ChangeBackDropEntry(CString &str) {
	CString keyword, backName, ptr = str;
	BetweenTabs(keyword, ptr);
	BetweenTabs(backName, ptr);
	m_backName = strdup(backName);
}

ChangeBackDropEntry::~ChangeBackDropEntry() {
	free(m_backName);
}

void ChangeBackDropEntry::Execute(int) {
	void SetBackDrop(const char *);
	SetBackDrop(m_backName);
}

void ChangeBackDropEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("backdrop\t%s\r\n", m_backName);
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

void NickEntry::Execute(int mode) {
	if (mode == HM_LIVE)
		ProcessNick(LookupPui(m_oldNick), m_newNick, TRUE);
	else ReinstallPui(LookupPui(m_oldNick), m_newNick);
}

void NickEntry::WriteSelf(CArchive &ar) {
	CString s;
	s.Format("nick\t%s\t%s\r\n", m_oldNick, m_newNick);
	ar.WriteString(s);
}


void AddAndExecute(HistoryEntry *ent) {
	GetView()->GetDocument()->m_history.AddTail((void *) ent);
	ent->Execute(HM_LIVE);
}

void AddToHistory(HistoryEntry *ent) {
	GetView()->GetDocument()->m_history.AddTail((void *) ent);
}

#define CONVERSATIONSTRING	"#CHATCONVERSATION"
void ChatSaveConversation(CArchive &ar) {
	CString ln;
	ln.Format("%s\r\n", CONVERSATIONSTRING);
	ar.WriteString(ln);

	CPtrList *hist = &(GetView()->GetDocument()->m_history);
	POSITION pos = hist->GetHeadPosition();
	while (pos) {
		HistoryEntry *entry = (HistoryEntry *)hist->GetNext(pos);
		entry->WriteSelf(ar);
	}
}

BOOL ChatLoadConversation(CArchive &ar) {
	char key[50];
	CString buff;

	ar.ReadString(buff);
	if (strnicmp(buff, CONVERSATIONSTRING, strlen(CONVERSATIONSTRING)) != 0) {
		AfxMessageBox(ID_ERR_NOT_CHATCONV);
		return FALSE;
	}

	BOOL oldRefresh = theApp.m_bNoRefresh;
	theApp.m_bNoRefresh = TRUE;

	while (ar.ReadString(buff)) {
		sscanf(buff, "%s", key);
//		TRACE("BUFF is %s.\n", buff);
		if (!stricmp(key, "say"))
			AddAndExecute(new SayEntry(buff));
		else if (!stricmp(key, "join"))
			AddAndExecute(new JoinEntry(buff));
		else if (!stricmp(key, "part"))
			AddAndExecute(new PartEntry(buff));
		else if (!stricmp(key, "changeavatar"))
			AddAndExecute(new ChangeAvatarEntry(buff));
		else if (!stricmp(key, "getinfo"))
			AddAndExecute(new GetInfoEntry(buff));
		else if (!stricmp(key, "nick"))
			AddAndExecute(new NickEntry(buff));
		else if (!stricmp(key, "backdrop"))
			AddAndExecute(new ChangeBackDropEntry(buff));
		else if (!stricmp(key, "starthistory"))
			AddAndExecute(new StartHistoryEntry(buff));
		else {
			CString mesg;
			mesg.LoadString(ID_ERR_BAD_CONV_FIELD);
			VERIFY(ReplaceToken(mesg, CString("%1"), key));
			AfxMessageBox(mesg);
		}
	}

	if (!puiSelf)  // in case file is bad, or old format
		AddAndExecute(new JoinEntry(GetMyName()));  // pretend to join to set pui & puiSelf

	theApp.m_bNoRefresh = oldRefresh;
	if (theApp.m_bComicView) GetView()->UpdateScroll();

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

void UpdateMemberListIcon(CUserInfo *pui) {
	int AddToImageList(CUserInfo* pui);
	int RemoveMemberFromList(CUserInfo* pui);

	int listIndex = RemoveMemberFromList(pui);
	int imageIndex = AddToImageList(pui);

	CString attedNick;
	pui->GetAttedNick(attedNick);

	GetMembers()->m_MemberListBox.InsertItem(listIndex, attedNick, imageIndex);
}

