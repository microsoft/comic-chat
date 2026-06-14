#ifndef __CHATPROT_H__
#define __CHATPROT_H__

#include "defines.h"
#include "userinfo.h"
#include "format.h"
#include "ircsock.h"

// For protocol and protosupp 
class CRoomInfo
{
public:
	CString			m_strChannel;			// Encoded channel name
	CString			m_strPrettyChannel;		// Decoded channel name
	CString			m_strPassword;
	CString			m_strTopic;
	CString			m_strCreationModes;
	CDWordArray*	m_prgdwTopicFormatting;
	DWORD			m_dwModes;
	DWORD			m_dwMaxUsers;
	BOOL			m_bSetMode;
	CDocument*		m_doc;

	CRoomInfo() 
	{ 
		m_strTopic = m_strPassword = m_strChannel = "";
		m_doc = NULL;
		m_prgdwTopicFormatting = NULL;
		m_dwModes = m_dwMaxUsers = 0L;
		m_bSetMode = FALSE;
	}

	~CRoomInfo() 
	{
		FreeAndNullFormatting(&m_prgdwTopicFormatting);
	}

	virtual void ChatPartChannel(CDocument *doc, BOOL bHardDisconnect) {}
	virtual void SendMessageText(char *szMesg) { ASSERT(0); }
	virtual BOOL bChatSendToChannel(const char *szAnnotations, const char *szMesg, char *szNMText = NULL, USHORT uModes = 0) 
		{ ASSERT(0); return FALSE; }
	virtual void ChatAnnounceNewAvatar(const char *szAvName, const char *szURL, const char *szAddressee = NULL, BOOL bForce = FALSE);
	virtual BOOL bChatSendPrivMesg(const char *szAddressee, const char *szAnnotations, const char *szMesg, char *szNMText = NULL, BOOL bAsNotice = FALSE, USHORT uModes = 0) 
		{ ASSERT(0); return FALSE; }
	virtual BOOL ChatSetTopic(const char *szTopic) { ASSERT(0); return FALSE; }
	virtual BOOL ChatKickUser(const char *szNickname, const char *szReason) { ASSERT(0); return FALSE; }
	virtual void ChatKickUser(CUserInfo *pui) { ASSERT(0); }
	virtual void ChatBanUser(CUserInfo *pui) { ASSERT(0); }
	virtual BOOL ChatBanUser(const char *szBanPattern, BOOL bBan, const char *szEncodedChannel = NULL) { ASSERT(0); return FALSE; }
	virtual void ChatGetIdentity(CUserInfo *pui, LPCTSTR szNickname = NULL) { ASSERT(0); }
	virtual BOOL bChatShowMOTD() { ASSERT(0); return FALSE; }
	virtual BOOL ChatSendInvitation(const char *szNickname) { ASSERT(0); return FALSE; }
	virtual BOOL ChatChangeNick(const char *szNewNick) { ASSERT(0); return FALSE; }
	virtual void ReplyVersion(CUserInfo *pui);
	virtual void ReplyPing(CUserInfo *pui, CString mesg);
	virtual void ReplyTime(CUserInfo *pui);
	virtual void ReplyEmail(CUserInfo *pui);
	virtual void ReplyHomePage(CUserInfo *pui);
	virtual void DoNetMeetingCX(CUserInfo *pui, CString strAddr);
	virtual void ChatStartNetMeeting(CUserInfo *pui);
	virtual void ChatGetVersion(CUserInfo *pui);
	virtual void ChatPingUser(CUserInfo *pui);
	virtual void ChatGetLocalTime(CUserInfo *pui);
	virtual void ChatGetEmail(CUserInfo *pui);
	virtual void ChatGetHomePage(CUserInfo *pui);
	// virtual BOOL bRegisterJoins(char* szMesg);
	virtual BOOL bRegisterMode(char* szMesg) { ASSERT(0); return FALSE; }
	virtual BOOL SlashRaw(char *szMesg, IRCPARSE *pParse);
	virtual BOOL ProcessSlashCommand(char *szMesg, CDWordArray* prgdwFormatting, USHORT uModes, BOOL bInvokedByWhisperBox = FALSE);
	virtual void ChatGetInfo(CUserInfo *pui);
	virtual void ChatGetAvatarInfo(CUserInfo *pui, BOOL bInteractive);
	virtual void ChatSyncBackDrop(CChatDoc* pDoc, const char *szBackdrop, const char *szURL);
	virtual BOOL bSendWhispers(const char *szAnnotations, const char *szMesg, char *szNMText = NULL, USHORT uModes = BM_WHISPER, BOOL *pbJustToMe = NULL);
	virtual void DoChannelDialog();
	virtual BOOL ChatSetMode(DWORD newMode, DWORD newMaxUsers, const char *szNewPasswd) { ASSERT(0); return FALSE; }
	virtual void DoKickDlg(const char *nick, const char *strBan);
	virtual void ChatSetOperator(CUserInfo *pui, int mode);
	virtual void ChatInvite();
	virtual void ChatSendFile(CUserInfo *pui); 
	virtual void OnLogin() { ASSERT(0); }
	virtual void ChatSetAway(BOOL bAway, const char *szMesg, CUserInfo *pui = NULL, BOOL bProtoNotify = TRUE);
	virtual void SetStatusString(CString &str) { ASSERT(0); }
	virtual void SetConnectionStatus(int iStat) { ASSERT(0); }
	virtual int GetConnectionStatus() { ASSERT(0); return 0; }
	virtual void UpdateStatus();
	virtual void ChatJoinChannel(CRoomInfo &enterInfo) { ASSERT(0); }
	virtual void ChatCreateChannel(CRoomInfo &enterInfo) { ASSERT(0); }
	virtual int GetType() { ASSERT(0); return 0; }
	virtual void ChatSetNick(const char *szNickname) { ASSERT(0); }
	virtual void OnIdle(LONG) {}
	virtual BOOL IsIRCX () { return FALSE; }
	virtual void DoIgnoreUser(CUserInfo *pui, BOOL bIgnore, BOOL bAutoIgnore = FALSE, LPCTSTR szNickname = NULL) { ASSERT(0); }
	virtual BOOL ChangeProperty(CUserInfo *puiSelf, LPCSTR pszProperty, LPCSTR pszValue) { ASSERT(0); return FALSE; }
	virtual void OnPropertyChange(LPCSTR pszProperty, LPCSTR pszValue);
};


extern CRoomInfo g_enterInfo, *currentRoom;

#define strCurrentChannel			(currentRoom->m_strChannel)
#define strCurrentChannelTopic		(currentRoom->m_strTopic)
#define strCurrentChannelKey		(currentRoom->m_strPassword)
#define dwCurrentChannelMode		(currentRoom->m_dwModes)
#define dwCurrentUserLimit			(currentRoom->m_dwMaxUsers)

#endif // __CHATPROT_H__
