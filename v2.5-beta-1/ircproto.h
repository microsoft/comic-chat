#ifndef __IRCPROTO_H__
#define __IRCPROTO_H__

#include "userinfo.h"
#include "chatprot.h"
#include "ircsock.h"
#include "userlist.h"
#include "roomlist.h"

class CIrcProto : public CRoomInfo {
public:
	CIrcProto() { m_bInRoom = FALSE; }

	CIrcSocket*		m_pSock;
	BOOL			m_bInRoom;
	CString			m_strClientData;

	virtual void	ChatPartChannel(CDocument *doc, BOOL hardDisconnect);
	virtual BOOL	bChatSendToChannel(const char *szAnnotations, const char *szMesg, char *szNMText = NULL, USHORT uModes = 0);
	virtual void	SendMessageText(char *szMesg);
	virtual BOOL	bChatSendPrivMesg(const char *szAddressee, const char *szAnnotations, const char *szMesg, char *szNMText = NULL, BOOL bAsNotice = FALSE, USHORT uModes = 0);
	virtual BOOL	ChatSetTopic(const char *szTopic);
	virtual BOOL	ChatKickUser(const char *szNickname, const char *szReason);
	virtual void	ChatKickUser(CUserInfo *pui);
	virtual void	ChatBanUser(CUserInfo *pui);
	virtual BOOL	ChatBanUser(const char *szBanPattern, BOOL bBan, const char *szEncodedChannel = NULL);
	virtual void	ChatGetIdentity(CUserInfo *pui, LPCTSTR szNickname = NULL);
	virtual BOOL	bChatShowMOTD();
	virtual BOOL	ChatSendInvitation(const char *szNickname);
	virtual BOOL	ChatChangeNick(const char *szNewNickname);
	virtual void	OnLogin();
	virtual void	ChatSetAway(BOOL bAway, const char *szMesg, CUserInfo *pui = NULL, BOOL bProtoNotify = TRUE);
	virtual void	SetConnectionStatus(int iStat);
	virtual int		GetConnectionStatus();
	virtual void	ChatJoinChannel(CRoomInfo &enterInfo);
	virtual void	ChatCreateChannel(CRoomInfo &enterInfo);
	virtual void	ChatJoinAux(CRoomInfo &enterInfo);
	virtual void	ChatCreateAux(CRoomInfo &enterInfo);
	virtual int		GetType() { return PC_IRC; }
	virtual void	ChatSetNick(const char *szNickname);
	virtual BOOL	bRegisterMode(char* szMesg);
	virtual BOOL	ProcessSlashCommand(char *szMesg, CDWordArray* prgdwFormatting, USHORT uModes, BOOL bInvokedByWhisperBox = FALSE);
	void			TryNewNick(int msg_id, const char *showNick = NULL, BOOL registerNick = TRUE, CString *newNick = NULL);
	BOOL			bChatSendToTarget(const char *szAddressee, const char *szAnnotations, const char *szMesg, USHORT uModes = 0, BOOL bAsNotice = FALSE);
	BOOL			SlashJoin(IRCPARSE *pParse);
	BOOL			SlashCreate(IRCPARSE *pParse);
	BOOL			SlashPart(IRCPARSE *pParse);
	BOOL			SlashNick(IRCPARSE *pParse);
	BOOL			SlashServer(IRCPARSE *pParse, char *szMesg);
	BOOL			SlashProp(IRCPARSE *pParse, char *szMesg);
	BOOL			SlashPrivMsg(IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting, BOOL bInvokedByWhisperBox);
	BOOL			SlashSound(IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting);
	BOOL			SlashMeOrThink(enumCmdId cmdid, IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting, USHORT uModes, BOOL bInvokedByWhisperBox);
	BOOL			SlashThink(IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting, BOOL bInvokedByWhisperBox);
	BOOL			SlashList(IRCPARSE *pParse, const char *szMesg);
	BOOL			SlashWho(const char *szMesg);
	BOOL			SlashAway(IRCPARSE *pParse, const char *szMesg, CDWordArray* prgdwFormatting);
	BOOL			SlashGeneric(enumCmdId cmdid, IRCPARSE *pParse, char *szMesg, CDWordArray* prgdwFormatting);
	BOOL			SlashMode(IRCPARSE *pParse);
	SYNTAX			GetSyntaxFromCmdId(enumCmdId cmdid, USHORT *puIndex = NULL);
	CString			StrSyntaxMessage(enumCmdId cmdid);
	CString			StrEncodeCommandParam(DWORD dwAt, INT *piEncoding, CHAR *szParam);
	BOOL			IsIRCX() { return m_pSock->m_bIrcXServer; }
	BOOL			ChatSetMode(DWORD newMode, DWORD newMaxUsers, const char *szNewPasswd);
	BOOL			bExecuteQuery(enumQueryPurpose qp, 
								  enumCommandType ct,
								  enumDataType dt,
								  PVOID pvData,
								  CString strChannelName,
								  CString strNicknameMask);
	BOOL			ChatSetClientData(const char *szClientData);
	void			HandleClientDataChange(LPCSTR pszNewClientData);
	int				EncodingType();
	const char*		EncodeString(const char *szString, int iEncoding = ENC_CHANNEL);
	virtual void	DoIgnoreUser(CUserInfo *pui, BOOL bIgnore, BOOL bAutoIgnore = FALSE, LPCTSTR szNickname = NULL);
	virtual void	SetVisibility(BOOL);
	virtual BOOL 	ChangeProperty(CUserInfo *puiSelf, LPCSTR pszProperty, LPCSTR pszValue);
};

extern CIrcSocket serverConn;


#define APPEARSPREFIX		" Appears as "
#define GETINFOPREFIX		" GetInfo"
#define HERESINFOPREFIX		" HeresInfo: "
#define BACKGRNDPREFIX		" BDrop: "
#define NEWBACKGRNDPREFIX	" BDrop2: "
#define REQUESTCHARPREFIX	" GetCharInfo"
#define CCUDI1				"CCUDI1"

const char actionID[]		= {0x01, 'A', 'C', 'T', 'I', 'O', 'N'};
const char soundID[]		= {0x01, 'S', 'O', 'U', 'N', 'D'};
const char versionID[]		= {0x01, 'V', 'E', 'R', 'S', 'I', 'O', 'N'};
const char pingID[]			= {0x01, 'P', 'I', 'N', 'G'};
const char timeID[]			= {0x01, 'T', 'I', 'M', 'E'};
const char emailID[]		= {0x01, 'E', 'M', 'A', 'I', 'L'};
const char urlID[]			= {0x01, 'U', 'R', 'L'};
const char netMeetingID[]	= {0x01, 'N', 'E', 'T', 'M', 'E', 'E', 'T'};
const char awayID[]			= {0x01, 'A', 'W', 'A', 'Y'};
const char clientInfoID[]	= {0x01, 'C', 'L', 'I', 'E', 'N', 'T', 'I', 'N', 'F', 'O'};
const char fileDCCID[]		= {0x01, 'D', 'C', 'C'};
const char xvchatID[]		= {0x01, 'X', '-', 'V', 'C', 'H', 'A', 'T'};

const short	g_nActionLen	= 7;
const short g_nSoundLen		= 6;
const short g_nVersionLen	= 8;
const short g_nPingLen		= 5;
const short g_nTimeLen		= 5;
const short g_nEmailLen		= 6;
const short g_nUrlLen		= 4;
const short g_nNetMeetLen	= 8;
const short g_nAwayLen		= 5;
const short g_nClientInfoLen= 11;
const short g_nFileDCCLen	= 4;
const short g_nHeresInfoLen	= 12;
const short g_nAppearsAsLen	= 12;
const short g_nXVChatLen	= 8;
const short g_nGetCharLen   = 12;

const WORD	g_wIgnoreIdent		= 0x0001;
const WORD	g_wAutoIgnoreIdent	= 0x0002;

// Character used to send a deferred URL, telling the receiver to query us
// for the real URL when needed.
#define DEFERRED_URL_STRING "?"
#define DEFERRED_URL_CHAR   '?'

extern const char *DecodeString(const char *szString, int iEncoding);
extern const char *EncodeChan(const char *szChannel);
extern const char *DecodeChan(const char *szChannel, BOOL bForceDBCS = FALSE);
extern const char *EncodeNick(const char *szNick, BOOL bEscapeWildcards = FALSE);
extern const char *DecodeNick(const char *szNick);
extern const char *DecodeNickForScreen(const char *szNick);
extern void StartIdentD();
extern void StopIdentD();
extern void SetVisibility(BOOL bVisible);
extern long GetMyIP();
extern void ChatFillUserList(CUserList *pul);
extern void ChatFillRoomList(CRoomList *prl);
extern void GetModeChars(DWORD dwFlags, char *szBuff);
extern void FixMICChannelName(CChatDoc* doc, CRoomInfo* pEnterRoom);
extern void CommunicationCleanup();
extern BOOL CommunicationInits();
extern CRoomInfo *NewDefaultProto(CChatDoc *doc);

#include "protsupp.h"

#endif // __IRCPROTO_H__

