#ifndef __CHATPROT_H__
#define __CHATPROT_H__

#include "UserInfo.H"

// For protocol and protosupp 
class CRoomInfo
{
public:
	CString			m_strChannel;			// Encoded channel name
	CString			m_strPrettyChannel;		// Decoded channel name
	CString			m_strPassword;
	CString			m_strTopic;
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
		if (m_prgdwTopicFormatting)
		{
			m_prgdwTopicFormatting->RemoveAll();
			delete m_prgdwTopicFormatting;
		}
	}

	virtual void ChatPartChannel(CDocument *doc, BOOL hardDisconnect) {}
	virtual void SendMessageText(char *szMesg) { ASSERT(0); }
	virtual BOOL ChatSendToChannel(const char *szAnnotations, const char *szMesg, char *szNMText = NULL, BYTE byteMode = 0) 
		{ ASSERT(0); return FALSE; }
	virtual void ChatAnnounceNewAvatar(const char *szAvName, const char *szAddressee = NULL, BOOL bForce = FALSE);
	virtual BOOL ChatSendPrivMesg(const char *szAddressee, const char *szAnnotations, const char *szMesg, char *szNMText = NULL, BOOL bAsNotice = FALSE, BYTE byteMode = 0) 
		{ ASSERT(0); return FALSE; }
	virtual BOOL ChatSetTopic(const char *topic) { ASSERT(0); return FALSE; }
	virtual BOOL ChatKickUser(const char *nick, const char *reason) { ASSERT(0); return FALSE; }
	virtual void ChatKickUser(CUserInfo *pui) { ASSERT(0); }
	virtual void ChatBanUser(CUserInfo *pui) { ASSERT(0); }
	virtual BOOL ChatBanUser(const char *pattern, BOOL bBan) { ASSERT(0); return FALSE; }
	virtual void ChatGetIdentity(CUserInfo *pui) { ASSERT(0); }
	virtual void ChatShowMOTD() { ASSERT(0); }
	virtual BOOL ChatSendInvitation(const char *nick) { ASSERT(0); return FALSE; }
	virtual BOOL ChatChangeNick(const char *newNick) { ASSERT(0); return FALSE; }
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
	virtual BOOL SlashRaw(char *mesg);
	virtual BOOL ProcessSlashCommand(char *mesg);
	virtual void ChatGetInfo(CUserInfo *pui);
	virtual void ChatSyncBackDrop(const char *backdrop);
	virtual BOOL ChatSendSound(const char *snd, const char *mesg, CDWordArray *prgdwFormatting);
	virtual BOOL SendWhispers(const char *szAnnotations, const char *szMesg, char *szNMText = NULL);
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
	virtual void ChatJoinChannel() { ASSERT(0); }
	virtual int GetType() { ASSERT(0); return 0; }
	virtual void ChatSetNick(const char *nick) { ASSERT(0); }
	virtual void OnIdle(LONG) {}
	virtual BOOL IsIRCX () { return FALSE; }
	virtual void DoIgnoreUser(CUserInfo *pui, BOOL bIgnore, BOOL bAutoIgnore = FALSE) { ASSERT(0); }
};


extern CRoomInfo enterInfo, *currentRoom;

#define strCurrentChannel			(currentRoom->m_strChannel)
#define strCurrentChannelTopic		(currentRoom->m_strTopic)
#define strCurrentChannelKey		(currentRoom->m_strPassword)
#define dwCurrentChannelMode		(currentRoom->m_dwModes)
#define dwCurrentUserLimit			(currentRoom->m_dwMaxUsers)

#endif // __CHATPROT_H__
