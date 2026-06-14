#ifndef __IRCPROTO_H__
#define __IRCPROTO_H__

#include "UserInfo.H"
#include "ChatProt.H"
#include "CSSPI.H"

const short g_nDefaultIOBuff = 512;	// By default, for IRC servers

class CIrcSocket : public CAsyncSocket {
public:
	CIrcSocket::CIrcSocket(void);
	CIrcSocket::~CIrcSocket(void);

	HRESULT			HrInitAlloc(SHORT nMaxIOBuff);
	HRESULT			HrIrcXLogin(BOOL bForceNextPackage);
	HRESULT			HrIrcLogin(CHAR *szNickname, CHAR *szUserName, CHAR *szRealName, CHAR *szPassword);
	HRESULT			HrAuthenticate(CHAR *szUserName, CHAR *szPassword, CHAR *szSecurityPackage);
	HRESULT			HrGenerateAndSendAuthMsg(CHAR *szBlob, CHAR *szSecurityPackage);
	void			Reset(void);
	void			CloseSSPI(void);

	virtual void	OnConnect(int nErrorCode);
	virtual void	OnClose(int nErrorCode);
	virtual void	OnReceive(int nErrorCode);
	virtual void	OnOutOfBandData(int nErrorCode) { TRACE("Out of Band socket on error %d.\n", nErrorCode); }
	virtual void	ProcessMessage(char *);

	CStringArray	m_rgszSvrSecuPack;
	CStringArray	m_rgszUsrSecuPack;
	BOOL			m_bAnonAllowed;
	BOOL			m_bSvrAuthSeq;
	BOOL			m_bUsrAuthSeq;
	BOOL			m_bIrcXServer;
	BOOL			m_bRegistered;
	BOOL			m_bJustSentModeIsIrcX;
	SHORT			m_nMaxMsgLength;
	SHORT			m_nSecuPackIndex;
	CHAR			*m_szInput;
	CHAR			*m_szOutput2;
	CHAR			*m_szMessage;

    CredHandle		m_hCredential;			// SSPI Security related
    CtxtHandle		m_hContext;				// attributes
	PSecurityFunctionTable m_pFuncTbl;
    HINSTANCE		m_hSecLib;
	BOOL			m_bCredential;
	BOOL			m_bContext;
	BOOL			m_bAuthFailed;
	INT				m_iConnected;
};

#define	ENC_CHANNEL		0
#define ENC_DBCS		1
#define	ENC_UTF8		2

class CIrcProto : public CRoomInfo {
public:
	CIrcProto() { m_bInRoom = FALSE; }

	CIrcSocket*		m_sock;
	BOOL			m_bInRoom;

	virtual void	ChatPartChannel(CDocument *doc, BOOL hardDisconnect);
	virtual BOOL	ChatSendToChannel(const char *szAnnotations, const char *szMesg, char *szNMText = NULL, BYTE byteMode = 0);
	virtual void	SendMessageText(char *szMesg);
	virtual BOOL	ChatSendPrivMesg(const char *szAddressee, const char *szAnnotations, const char *szMesg, char *szNMText = NULL, BOOL bAsNotice = FALSE, BYTE byteMode = 0);
	virtual BOOL	ChatSetTopic(const char *topic);
	virtual BOOL	ChatKickUser(const char *nick, const char *reason);
	virtual void	ChatKickUser(CUserInfo *pui);
	virtual void	ChatBanUser(CUserInfo *pui);
	virtual BOOL	ChatBanUser(const char *pattern, BOOL bBan);
	virtual void	ChatGetIdentity(CUserInfo *pui); 
	virtual void	ChatShowMOTD();
	virtual BOOL	ChatSendInvitation(const char *nick);
	virtual BOOL	ChatChangeNick(const char *newNick);
	virtual void	OnLogin();
	virtual void	ChatSetAway(BOOL bAway, const char *szMesg, CUserInfo *pui = NULL, BOOL bProtoNotify = TRUE);
	virtual void	SetConnectionStatus(int iStat);
	virtual int		GetConnectionStatus();
	virtual void	ChatJoinChannel();
	virtual void	ChatJoinAux(const char *);
	virtual int		GetType() { return PC_IRC; }
	virtual void	ChatSetNick(const char *nick);
	virtual BOOL	ProcessSlashCommand(char *mesg);
	void			TryNewNick(int msg_id, const char *showNick = NULL, BOOL registerNick = TRUE, CString *newNick = NULL);
	BOOL			bChatSendToTarget(const char *szAddressee, const char *szAnnotations, const char *szMesg, BYTE byteMode=0, BOOL bAsNotice=FALSE);
	BOOL			SlashJoin(IRCPARSE *parse);
	BOOL			SlashPart(IRCPARSE *parse);
	BOOL			SlashNick(IRCPARSE *parse);
	BOOL			IsIRCX() { return m_sock->m_bIrcXServer; }
	BOOL			ChatSetMode(DWORD newMode, DWORD newMaxUsers, const char *szNewPasswd);
	int				EncodingType();
	const char*		EncodeString(const char *szString, int iEncoding = ENC_CHANNEL);
	virtual void DoIgnoreUser(CUserInfo *pui, BOOL bIgnore, BOOL bAutoIgnore = FALSE);
};

extern CIrcSocket serverConn;

#define APPEARSPREFIX		" Appears as "
#define GETINFOPREFIX		" GetInfo"
#define HERESINFOPREFIX		" HeresInfo: "
#define BACKGRNDPREFIX		" BDrop: "
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

#endif // __IRCPROTO_H__
