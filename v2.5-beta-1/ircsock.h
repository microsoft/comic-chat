#ifndef __IRCSOCK_H__
#define __IRCSOCK_H__

#include "UserInfo.H"
#include "ChatProt.H"
#include "CSSPI.H"
#include "Query.H"
#include "Resource.H"

///////////////////////////////////////////////////////////////////////////////
// Constants declaration
const short g_nDefaultIOBuff			= 512;	// By default, for IRC servers

#define	ENC_CHANNEL						0
#define ENC_DBCS						1
#define	ENC_UTF8						2

#define PT_NOTINIT						0
#define PT_WHOLESTRING					1		// for m_iType of CIrcPrint (PRINT TYPE)
#define PT_LASTSTRING					2
#define PT_NONE							3
#define PT_OFFSET						4

#define AT_COUNT						15
#define AT_NONE							0x00000000
#define AT_NICKNAME						0x00000001
#define AT_NICKMASK						0x00000002
#define AT_CHANNEL						0x00000004
#define AT_TOPIC						0x00000008
#define AT_REASON						0x00000010
#define AT_MESSAGE						0x00000020
#define AT_MAXMEMBER					0x00000040
#define AT_CHANNELFLAGS					0x00000080
#define AT_PASSWORD						0x00000100
#define AT_SERVER						0x00000200
#define AT_NETWORK						0x00000400
#define AT_SOUND						0x00000800
#define AT_USERFLAGS					0x00001000
#define AT_PROPNAME						0x00002000
#define AT_PROPVALUE					0x00004000
#define AT_OPTIONAL						0x08000000
#define AT_SPACEMULTIPLE				0x10000000
#define AT_COMMAMULTIPLE				0x20000000
#define AT_SHOWCOLON					0x40000000
#define AT_COLON						0x80000000

#define MAXARGS							10

#define MC_NONE							0
#define MC_HOSTLOST						1
#define MC_OWNERLOST					2

#define CMD_SHOWSTATUSWINDOW			0x01
#define CMD_MUSTBECONNECTED				0x02


// IRC Result Codes
const INT RPL_IRCSTART					= 1;

const INT RPL_WELCOME					= 1;
const INT RPL_YOURHOST					= 2;
const INT RPL_CREATED					= 3;
const INT RPL_MYINFO					= 4;
const INT RPL_FOOFORNOW					= 5;

const INT RPL_TRACELINK					= 200;
const INT RPL_TRACECONNECTING			= 201;
const INT RPL_TRACEHANDSHAKE			= 202;
const INT RPL_TRACEUNKNOWN				= 203;
const INT RPL_TRACEOPERATOR				= 204;
const INT RPL_TRACEUSER					= 205;
const INT RPL_TRACESERVER				= 206;
const INT RPL_TRACENEWTYPE				= 208;
const INT RPL_TRACELOG					= 261;

const INT RPL_STATSLINKINFO				= 211;
const INT RPL_STATSCOMMANDS				= 212;
const INT RPL_STATSCLINE				= 213;
const INT RPL_STATSNLINE				= 214;
const INT RPL_STATSILINE				= 215;
const INT RPL_STATSKLINE				= 216;
const INT RPL_STATSYLINE				= 218;
const INT RPL_ENDOFSTATS				= 219;
const INT RPL_STATSLLINE				= 241;
const INT RPL_STATSUPTIME				= 242;
const INT RPL_STATSOLINE				= 243;
const INT RPL_STATSHLINE				= 244;

const INT RPL_ADMINME					= 256;
const INT RPL_ADMINLOC1					= 257;
const INT RPL_ADMINLOC2					= 258;
const INT RPL_ADMINEMAIL				= 259;

const INT RPL_USERHOST					= 302;
const INT RPL_AWAY						= 301;
const INT RPL_UNAWAY					= 305;
const INT RPL_NOWAWAY					= 306;

const INT RPL_ISON						= 303;

const INT RPL_WHOISUSER					= 311;
const INT RPL_WHOISSERVER				= 312;
const INT RPL_WHOISOPERATOR				= 313;
const INT RPL_WHOISIDLE					= 317;
const INT RPL_ENDOFWHOIS				= 318;
const INT RPL_WHOISCHANNELS				= 319;
const INT RPL_WHOISIP					= 320;

const INT RPL_WHOWASUSER				= 314;
const INT RPL_ENDOFWHOWAS				= 369;

const INT RPL_LISTSTART					= 321;
const INT RPL_LIST						= 322;
const INT RPL_LISTEND					= 323;

const INT RPL_CHANNELMODEIS				= 324;

const INT RPL_NOTOPIC					= 331;
const INT RPL_TOPIC						= 332;

const INT RPL_INVITING					= 341;

const INT RPL_WHOREPLY					= 352;
const INT RPL_ENDOFWHO					= 315;

const INT RPL_VERSION					= 351;

const INT RPL_NAMEREPLY					= 353;
const INT RPL_ENDOFNAMES				= 366;

const INT RPL_LINKS						= 364;
const INT RPL_ENDOFLINKS				= 365;

const INT RPL_BANLIST					= 367;
const INT RPL_ENDOFBANLIST				= 368;

const INT RPL_INFO						= 371;
const INT RPL_ENDOFINFO					= 374;

const INT RPL_MOTDSTART					= 375;
const INT RPL_MOTD						= 372;
const INT RPL_MOTD2						= 377;
const INT RPL_ENDOFMOTD					= 376;

const INT RPL_YOUREOPER					= 381;
const INT RPL_YOUREADMIN				= 386;

const INT RPL_TIME						= 391;

const INT RPL_UMODEIS					= 221;

const INT RPL_LUSERCLIENT				= 251;
const INT RPL_LUSEROP					= 252;
const INT RPL_LUSERUNKNOWN				= 253;
const INT RPL_LUSERCHANNELS				= 254;
const INT RPL_LUSERME					= 255;

const INT RPL_LOCALUSERS				= 265;
const INT RPL_GLOBALUSERS				= 266;

const INT RPL_IRCEND					= 399;


// IRC Standard error codes
const INT ERR_IRCSTART					= 401;

const INT ERR_NOSUCHSERVER				= 402;
const INT ERR_NEEDMOREPARAMS			= 461;
const INT ERR_NOTREGISTERED				= 451;
const INT ERR_ALREADYREGISTERED			= 462;
const INT ERR_TOOMANYTARGETS			= 407;
const INT ERR_NOORIGIN					= 409;
const INT ERR_UNKNOWNCOMMAND			= 421;
const INT ERR_NOMOTD					= 422;
const INT ERR_PASSWDMISMATCH			= 464;
const INT ERR_YOUREBANNEDCREEP			= 465;
const INT ERR_YOUWILLBEBANNED			= 466;
// Nicks
const INT ERR_NOSUCHNICK				= 401;	
const INT ERR_NONICKNAMEGIVEN			= 431;
const INT ERR_ERRONEUSNICKNAME			= 432;
const INT ERR_NICKNAMEINUSE				= 433; 
const INT ERR_NICKCOLLISION				= 436;
const INT ERR_NICKTOOFAST				= 438;
const INT ERR_NICKNOCHANGE				= 439;
// Channels
const INT ERR_NOSUCHCHANNEL				= 403;
const INT ERR_TOOMANYCHANNELS			= 405;
const INT ERR_CHANNELISFULL				= 471;
const INT ERR_INVITEONLYCHAN			= 473;
const INT ERR_BANNEDFROMCHAN			= 474;
const INT ERR_BADCHANNELKEY				= 475;
const INT ERR_USERONCHANNEL				= 443;
const INT ERR_KEYSET					= 467;
// Send
const INT ERR_CANNOTSENDTOCHAN			= 404;
const INT ERR_NORECIPIENT				= 411;
const INT ERR_USERNOTINCHANNEL			= 441;
const INT ERR_NOTONCHANNEL				= 442;
// Modes
const INT ERR_UNKNOWNMODE				= 472;
const INT ERR_NOPRIVILEGES				= 481;
const INT ERR_CHANOPRIVSNEEDED			= 482;
const INT ERR_CHANOWNPRIVNEEDED			= 485;

const INT ERR_UMODEUNKNOWNFLAG			= 501;
const INT ERR_USERSDONTMATCH			= 502;

const INT ERR_IRCEND					= 502;



// New IRCX replies
const INT RPL_IRCXSTART					= 800;

const INT RPL_IRCX						= 800;
const INT RPL_ACCESSADD					= 801;
const INT RPL_ACCESSDELETE				= 802;
const INT RPL_ACCESSSTART				= 803;
const INT RPL_ACCESSLIST				= 804;
const INT RPL_ACCESSEND					= 805;
const INT RPL_EVENTADD					= 806;
const INT RPL_EVENTDEL					= 807;
const INT RPL_EVENTSTART				= 808;
const INT RPL_EVENTLIST					= 809;
const INT RPL_EVENTEND					= 810;
const INT RPL_LISTXSTART				= 811;
const INT RPL_LISTXLIST					= 812;
const INT RPL_LISTXPICS					= 813;
const INT RPL_LISTXTRUNC				= 816;
const INT RPL_LISTXEND					= 817;

const INT RPL_PROPLIST					= 818;
const INT RPL_PROPEND					= 819;

const INT RPL_IRCXEND					= 899;



// New IRCX errors
const INT ERR_IRCXSTART1				= 503;

const INT ERR_NOJOINDYNAMIC				= 552;
const INT ERR_NODYNAMICCHANNELS			= 553;
const INT ERR_AUTHONLY					= 556;
const INT ERR_OVERFLOWABORT				= 557;

const INT ERR_IRCXEND1					= 557;

const INT ERR_IRCXSTART2				= 900;

const INT ERR_BADCOMMAND				= 900;
const INT ERR_TOOMANYARGUMENTS			= 901;
const INT ERR_BADFUNCTION				= 902;
const INT ERR_BADLEVEL					= 903;
const INT ERR_BADTAG					= 904;
const INT ERR_BADPROPERTY				= 905;
const INT ERR_BADVALUE					= 906;
const INT ERR_RESOURCE					= 907;
const INT ERR_SECURITY					= 908;
const INT ERR_ALREADYAUTHENTICATED		= 909;
const INT ERR_AUTHENTICATIONFAILED		= 910;
const INT ERR_AUTHENTICATIONSUSPENDED	= 911;
const INT ERR_UNKNOWNPACKAGE			= 912;
const INT ERR_NOACCESS					= 913;
const INT ERR_NOWHISPER					= 923;
const INT ERR_NOSUCHOBJECT				= 924;
const INT ERR_NOTSUPPORTED				= 925;
const INT ERR_CHANNELEXIST				= 926;

const INT ERR_INTERNALERROR				= 999;


// MIC 1.0 IRC2 Error code
const INT ERR_CANNOTJOINMICONLY			= 900; //	":%h 900 %n %s :Cannot join MIC only channel with IRC client",
const INT ERR_CANNOTJOINFROMREMOTE		= 901; //	":%h 901 %n %s :Cannot join channel from remote server (+r)",
const INT ERR_CANNOTCREATEDYNAMIC		= 902; //	":%h 902 %n %s :Cannot create dynamic channels (admin)",
const INT ERR_COMMANDNOTSUPPORTED		= 903; //	":%h 903 %n %s :Command not supported on this server",
const INT ERR_ONLYAUTHCANJOIN			= 904; //	":%h 904 %n %s :Only authenticated users may join channel",
const INT ERR_CANNOTCHANGENICK			= 905; //	":%h 905 %n :Nick changes are not permitted at this time, try again later",
const INT ERR_CANNOTMAKEHOST			= 906; //	":%h 906 %n %s :Cannot make host due to admin restriction",
const INT ERR_CANNOTJOINDYNAMIC			= 907; //	":%h 907 %n %s :Cannot join dynamic channels due to admin restriction",
const INT ERR_UNKNOWNERROR				= 999; //	":%h 999 %n :Unknown error code %d",

const INT ERR_IRCXEND2					= 999;


///////////////////////////////////////////////////////////////////////////////
// Macros declaration
#define bIsErrorCode(uCode)				((uCode >= ERR_IRCSTART && uCode <= ERR_IRCEND) || (uCode >= ERR_IRCXSTART1 && uCode <= ERR_IRCXEND1) || (uCode >= ERR_IRCXSTART2 && uCode <= ERR_IRCXEND2))
#define bIsReplyCode(uCode)				((uCode >= RPL_IRCSTART && uCode <= RPL_IRCEND) || (uCode >= RPL_IRCXSTART && uCode <= RPL_IRCXEND))


///////////////////////////////////////////////////////////////////////////////
// Structures declaration
typedef struct tagMODECACH
{
	CHAR	szChannelName[MAX_TOKEN];
	CHAR	szNickname[MAX_NICK];
	BYTE	byteStatus;
} MODECACH, *PMODECACH;


typedef struct tagPRIRCCMD
{
	CHAR	*szCmd;		// the command
	INT		cb;			// and its length
	UCHAR	uFlags;		// command flags: show Status Window | must be connected
	UCHAR	uMinArg;	// minimum number of arguments for this command
} PRIRCCMD, *PPRIRCCMD;


typedef struct tagPARSE
{
	BOOL	bHasPrefix;
	CHAR	nick[50];
	CHAR	user[50];
	CHAR	machine[50];
	UINT	uCode;
	SHORT	nArgs;
	CHAR	*args[MAXARGS];
	SHORT	nOffsets[MAXARGS];
	CHAR	*lastString;
} IRCPARSE, *PIRCPARSE;


///////////////////////////////////////////////////////////////////////////////
// Enumerations declaration
//
// Ids of understood IRC commands
typedef enum
{
	cmdidAccess,
	cmdidAction,
	cmdidAuth,
	cmdidAway,
	cmdidClone,
	cmdidCreate,
	cmdidData,
	cmdidError,
	cmdidInfo,
	cmdidInvite,
	cmdidIsOn,
	cmdidJoin,
	cmdidKick,
	cmdidKill,
	cmdidKilled,
	cmdidKLine,
	cmdidKnock,
	cmdidList,
	cmdidListX,
	cmdidLUsers,
	cmdidMe,
	cmdidMode,
	cmdidMsg,
	cmdidNames,
	cmdidNick,
	cmdidNotice,
	cmdidPart,
	cmdidPass,
	cmdidPing,
	cmdidPong,
	cmdidPrivMsg,
	cmdidProp,
	cmdidQuit,
	cmdidQuote,
	cmdidRaw,
	cmdidReply,
	cmdidRequest,
	cmdidServer,
	cmdidSound,
	cmdidThink,
	cmdidTopic,
	cmdidUnKLine,
	cmdidUser,
	cmdidUserHost,
	cmdidWhisper,
	cmdidWho,
	cmdidWhoIs,
	cmdidMax	   // always one more - indicates how many cmds there are
} enumCmdId;


typedef struct tagSYNTAX
{
	enumCmdId	cmdid;		// the command id - cmdidMax if reply code
	UINT		uIDSComment;// 0 if no additional comment
	UCHAR		uArgNum;	// number of arguments
	DWORD		dwArgType[6];// argument types
} SYNTAX, *PSYNTAX;


const UINT		g_uSyntaxCount = 24;
const SYNTAX	g_rgSyntax[] = {
	{ cmdidCreate,	0, 4, { AT_CHANNEL, AT_CHANNELFLAGS, AT_MAXMEMBER|AT_OPTIONAL, AT_PASSWORD|AT_OPTIONAL, AT_NONE, AT_NONE } },
	{ cmdidInvite,	0, 2, { AT_NICKNAME, AT_CHANNEL, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidIsOn,	0, 1, { AT_NICKNAME|AT_SPACEMULTIPLE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidJoin,	0, 2, { AT_CHANNEL, AT_PASSWORD|AT_OPTIONAL, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidList,	0, 1, { AT_CHANNEL|AT_OPTIONAL|AT_COMMAMULTIPLE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidKick,	IDS_KICKMSG_SYNTAX, 3, { AT_CHANNEL, AT_NICKNAME, AT_REASON|AT_OPTIONAL|AT_COLON, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidKill,	IDS_KILLMSG_SYNTAX, 2, { AT_CHANNEL|AT_NICKNAME, AT_REASON|AT_OPTIONAL|AT_COLON, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidMe,		IDS_ME_SYNTAX, 1, { AT_MESSAGE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } }, 
	{ cmdidMsg,		0, 2, { AT_CHANNEL|AT_NICKNAME|AT_COMMAMULTIPLE, AT_MESSAGE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidMode,	0, 6, { AT_CHANNEL, AT_CHANNELFLAGS|AT_OPTIONAL, AT_MAXMEMBER|AT_OPTIONAL, AT_NICKNAME|AT_OPTIONAL, AT_NICKMASK|AT_OPTIONAL, AT_PASSWORD|AT_OPTIONAL } },
	{ cmdidMode,	0, 2, { AT_NICKNAME, AT_USERFLAGS|AT_OPTIONAL, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidNames,	0, 1, { AT_CHANNEL|AT_OPTIONAL|AT_COMMAMULTIPLE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidNick,	0, 1, { AT_NICKNAME, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidPart,	0, 1, { AT_CHANNEL|AT_OPTIONAL, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidPrivMsg,	IDS_PRIVMSG_SYNTAX, 2, { AT_CHANNEL|AT_NICKNAME|AT_COMMAMULTIPLE, AT_MESSAGE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidProp,	IDS_PROPGET_SYNTAX, 2, { AT_CHANNEL, AT_PROPNAME|AT_COMMAMULTIPLE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidProp,	IDS_PROPSET_SYNTAX, 3, { AT_CHANNEL, AT_PROPNAME, AT_PROPVALUE|AT_SHOWCOLON|AT_COLON|AT_OPTIONAL, AT_NONE } },
	{ cmdidServer,	0, 1, { AT_SERVER|AT_NETWORK, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidSound,	IDS_SOUND_SYNTAX, 3, { AT_CHANNEL|AT_NICKNAME, AT_SOUND, AT_MESSAGE|AT_OPTIONAL, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidThink,	0, 1, { AT_MESSAGE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidTopic,	IDS_SETTOPIC_SYNTAX, 2, { AT_CHANNEL, AT_COLON|AT_TOPIC, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidTopic,	IDS_GETTOPIC_SYNTAX, 1, { AT_CHANNEL, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidUserHost,0, 1, { AT_NICKNAME|AT_SPACEMULTIPLE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } },
	{ cmdidWhoIs,	0, 1, { AT_NICKMASK|AT_COMMAMULTIPLE, AT_NONE, AT_NONE, AT_NONE, AT_NONE, AT_NONE } }
};


///////////////////////////////////////////////////////////////////////////////
// Classes declaration
class CIrcPrint {
public:
	UINT		m_iType;
	const char*	m_szMessage;
	COLORREF	m_crTextColor;
	BYTE		m_offset;
	BOOL		m_bNewLine;

	CIrcPrint() { m_iType = PT_NOTINIT; m_crTextColor = RGB(0,0,0); m_bNewLine = FALSE; }

	void SetFormat(int type, const char *sz = NULL, COLORREF rgb = RGB(0,0,0), int offset = 0, BOOL bNewLine = FALSE) { m_iType = type; m_szMessage = sz; m_crTextColor = rgb; m_offset = (BYTE)offset; m_bNewLine = bNewLine;}
};


class CIrcSocket : public CAsyncSocket {
public:
	CIrcSocket::CIrcSocket(void);
	CIrcSocket::~CIrcSocket(void);

	HRESULT			HrInitAlloc(SHORT nMaxIOBuff);
	HRESULT			HrModeIsIrcXFailure();
	HRESULT			HrIrcXLogin(BOOL bForceNextPackage);
	HRESULT			HrIrcLogin(BOOL bIRCX, CHAR *szNickname, CHAR *szUserName, CHAR *szRealName, CHAR *szPassword, BOOL bPromptForPassword = TRUE);
	HRESULT			HrIrcSetOper(CHAR *szUserName, CHAR *szRealName);
	HRESULT			HrAuthenticate(CHAR *szUserName, CHAR *szPassword, CHAR *szSecurityPackage);
	HRESULT			HrGenerateAndSendAuthMsg(CHAR *szBlob, CHAR *szSecurityPackage);
	BOOL			PromptForPassword(LPCSTR pszUserName, BOOL bSaveInSettings);
	BOOL			bFreeModeCell(LPCTSTR szChannel, LPCTSTR szNickname);
	void			Reset(void);
	void			CloseSSPI(void);
	void 			SetAuthentication(UINT nType, LPCSTR pszUserName = NULL, LPCSTR pszPassword = NULL, LPCSTR pszCustomPkg = NULL);

	virtual void	OnConnect(int nErrorCode);
	virtual void	OnClose(int nErrorCode);
	virtual void	OnReceive(int nErrorCode);
	virtual void	OnOutOfBandData(int nErrorCode) { TRACE("Out of Band socket on error %d.\n", nErrorCode); }
	virtual void	ProcessMessage(char *);
	virtual void	HandleCommand(CString& strLine, char *szLine, PIRCPARSE pParse, CIrcPrint *pIrcPrint);
	virtual void	HandleResultCode(CString& strLine, char *szLine, PIRCPARSE pParse, CIrcPrint *pIrcPrint);
	virtual void	HandleErrorCode(char *szLine, PIRCPARSE pParse, CIrcPrint *pIrcPrint);

	CString			m_strMOTD;
	CString			m_strLUSER;

	CStringArray	m_rgszSvrSecuPack;
	CStringArray	m_rgszUsrSecuPack;
	LPSTR   		m_pszUserName;
	LPSTR    		m_pszPassword;
	BOOL			m_bAnonAllowed;
	UINT			m_nAuthenticationType;
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

	CQueryPtrList	m_queries;

	enum AuthType		// These should be the same as in chatsrv.h
	{
		authtypeNone = 0,
		authtypePlainText = 1,
		authtypeServerPackages = 2,
		authtypeCustomPackages = 3,
	};
};


extern PRIRCCMD	g_rgIrcCmd[];
extern void		ParseIt(const char *szMessage, PIRCPARSE pParse, BOOL bDoubleQuotes = FALSE);
extern void		FreeParse(PIRCPARSE pParse);
extern SHORT	NGetCmd(CHAR* szCmd);

#endif // __IRCSOCK_H__
