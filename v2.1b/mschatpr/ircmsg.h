//--------------------------------------------------------------------------------------------
//
//	Copyright (c) Microsoft Corporation, 1996 - 1997
//
//	Description:
//
//		Microsoft Internet ChatSockets.
//		
//		Constructs IRC PROTOCOL Messages
//
//	Authors:
//
//		Umesh Madan
//
//  Modified:	
//
//		Regis Brid, 01/29/97
//
//--------------------------------------------------------------------------------------------

#ifndef __IRCMSG__
#define __IRCMSG__

//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include "PrCnst.H"

//--------------------------------------------------------------------------------------------
//
// TABLES
//
//--------------------------------------------------------------------------------------------

typedef struct tagPRIRCCMD
{
	CHAR	*szCmd;		// the command
	INT		cb;			// and its length
} PRIRCCMD, *PPRIRCCMD;


//
// Ids of understood IRC commands
//
typedef enum
{
	cmdidAccess,
	cmdidAuth,
	cmdidAway,
	cmdidClone,
	cmdidCreate,
	cmdidData,
	cmdidError,
	cmdidInfo,
	cmdidInvite,
	cmdidJoin,
	cmdidKick,
	cmdidKill,
	cmdidKLine,
	cmdidKnock,
	cmdidList,
	cmdidListX,
	cmdidLUsers,
	cmdidMode,
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
	cmdidReply,
	cmdidRequest,
	cmdidTopic,
	cmdidUnKLine,
	cmdidUser,
	cmdidUserHost,
	cmdidWhisper,
	cmdidWho,
	cmdidWhoIs,
	cmdidMax	   // always one more - indicates how many cmds there are
} enumCmdId;


typedef struct tagMODEMAP
{
	CHAR	ircMode;
 	LONG	lMode;
} PRMODEMAP, *PPRMODEMAP;


//const	INT					RPL_FAMILYMAXSIZE	= 6;
//const	INT					CREPLYFAMILIES		= 9;  // == sizeof(g_rgPrReplyFamilies)/sizeof(PRRGUREPLYFAMILY);
//typedef UINT				PRRGUREPLYFAMILY[RPL_FAMILYMAXSIZE];
//extern	PRRGUREPLYFAMILY	g_rgPrReplyFamilies[];

//--------------------------------------------------------------------------------------------
//
// FUNCTIONS
//
//--------------------------------------------------------------------------------------------

//
// MESSAGE CRACKERS
//
SHORT	NGetCmd(CHAR *szCmd);

HRESULT HrParseServerName(CHAR *szServer, UINT *pcch, UINT *puPort);
HRESULT	HrGetPrefix(CHAR **psz, CHAR **pszPrefix);
HRESULT	HrGetCommand(CHAR **psz, CHAR **pszCmd);
HRESULT HrParseMessage(CHAR *sz, CHAR *szEnd, CHAR **pszPrefix, CHAR **pszCmd, CHAR **pszParams);

BOOL	bGetNextParam(CHAR **psz, CHAR **pszParam, INT *pcch);
BOOL	bGetNextSubParam(CHAR **pszParam, CHAR **pszSubParam, INT *pcch);

void	SkipTillChar(CHAR **psz, CHAR ch);
void	SkipOverChar(CHAR **psz, CHAR ch);
void	SkipTillDigit(CHAR **psz);
void	SkipTillAlpha(CHAR **psz);
void	SkipTillEnd(CHAR **psz);
inline	void	SkipTillSpace(CHAR **psz)
				{
						SkipTillChar(psz, g_chSpace);
				}
inline	void	SkipSpaces(CHAR **psz)
				{
						SkipOverChar(psz, g_chSpace);
				}
inline	BOOL	bIsTermChar(CHAR ch)
				{
					return (g_chLF == ch || g_chCR == ch);
				}

void SkipTillPrefixExtender(CHAR **psz);
void ExtractNickUserIPAddress(LPTSTR szUserIdent, BOOL bIrcX, LPTSTR *pszNickname, LPTSTR *pszUserName, LPTSTR *pszIPAddress, BOOL *pbAuthUser);

//
// MAPPERS
//
BOOL	bIsChannelModesValid(LONG lModes);
BOOL	bIsMemberModesValid(LONG lModes);
BOOL	bIsUserModesValid(LONG lModes);

INT		MapToIRCMode(enumAssociatedType at, LONG lMode, CHAR *szMode, INT cchMode);
void	ApplyIRCToOurChannelMode(CHAR *szMode, LONG *plMode, BOOL *pbCount, BOOL *pbKeywordSet, BOOL *pbKeywordReset, BOOL *pbMicOnly);
void	ApplyIRCToOurMemberMode(CHAR *szMode, LONG *plMode);
void	ApplyIRCToOurUserMode(CHAR *szMode, LONG *plMode);

HRESULT HrMapToIRCAndMakeChannelModeMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, LONG lOldModes, LONG lNewModes);
HRESULT HrMapToIRCAndMakeMemberModeMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szNickname, LONG lOldModes, LONG lNewModes);
HRESULT HrMapToIRCAndMakeUserModeMsg(BYTE **ppb, INT *pcb, CHAR *szNickname, LONG lOldModes, LONG lNewModes);

// Not used yet
// PPRMODEMAP PPrModeMap(INT *pcModes);

//
// MESSAGE MAKERS
//

// Functions used only once are inline
extern HRESULT HrMakeNickMsg(BYTE **ppb, INT *pcb, CHAR *szNickname);
extern HRESULT HrMakeUserMsg(BYTE **ppb, INT *pcb, CHAR *szUser, CHAR *szRealName);
extern HRESULT HrMakeCreateChannelMsg(BYTE **ppb, INT *pcb, CHAR *szChannel);
extern HRESULT HrMakeJoinMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szKeyword);
extern HRESULT HrMakeSetKeywordMsg(BYTE **ppb, INT *pcb, BOOL bSet, CHAR *szChannel, CHAR *szKeyword);
extern HRESULT HrMakeLogOffMsg(BYTE **ppb, INT *pcb);
extern HRESULT HrMakeLUsersMsg(BYTE **ppb, INT *pcb);
extern HRESULT HrMakeServerInfoMsg(BYTE **ppb, INT *pcb);
extern HRESULT HrMakeLeaveChannel(BYTE **ppb, INT *pcb, CHAR *szChannel);
extern HRESULT HrMakeKickMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szNickname, CHAR *szComment);
extern HRESULT HrMakePassMsg(BYTE **ppb, INT *pcb, CHAR *szPass);
extern HRESULT HrMakeSetChannelModeMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szMode, CHAR *szTarget);
extern HRESULT HrMakeCreateMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szModes, CHAR *szKeyword);
extern HRESULT HrMakeKLineMsg(BYTE **ppb, INT *pcb, BOOL bSet, CHAR *szIdentMask, CHAR *szReason);
extern HRESULT HrMakeAccessMsg(BYTE **ppb, INT *pcb, BOOL bSet, CHAR *szIdentMask, CHAR *szReason, LONG lDuration);
extern HRESULT HrMakeWhoMsg(BYTE **ppb, INT *pcb, CHAR *szUserMask);

// Functions used used more than once -> not worth being inline functions (to minimize size)
HRESULT HrMakeKillMsg(BYTE **ppb, INT *pcb, CHAR *szTarget, CHAR *szReason);
HRESULT HrMakeModeMsg(BYTE **ppb, INT *pcb, BOOL bSet, CHAR *szTarget, CHAR *szMode);
HRESULT HrMakeGetBannedListMsg(BYTE **ppb, INT *pcb, CHAR *szTarget);
HRESULT HrMakeGetModeMsg(BYTE **ppb, INT *pcb, CHAR *szTarget);
HRESULT HrMakeChannelMaxMemberCountMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, LONG lMaxMemberCount);
HRESULT HrMakePingOrPongMsg(BYTE **ppb, INT *pcb, BOOL bPing, CHAR *szTarget);
HRESULT HrMakeSendText(BYTE **ppb, INT *pcb, BOOL bNotice, BOOL bWhisper, CHAR *szTarget, CHAR *szRcpNicks, CHAR *szText);
HRESULT HrMakeSendData(BYTE **ppb, INT *pcb, CHAR *szTarget, CHAR *szRcpNicks, CHAR *szTag, CHAR *szData, SHORT nCmd);
HRESULT HrMakeTopicMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szTopic);
HRESULT HrMakeListMsg(BYTE **ppb, INT *pcb, CHAR *szCriteria, BOOL bListX);
HRESULT HrMakeNamesMsg(BYTE **ppb, INT *pcb, CHAR *szChannel);
HRESULT HrMakeUserHostMsg(BYTE **ppb, INT *pcb, CHAR *szName);
HRESULT HrMakeWhoIsMsg(BYTE **ppb, INT *pcb, CHAR *szNickmask);
HRESULT HrMakeInviteMsg(BYTE **ppb, INT *pcb, CHAR *szNickname, CHAR *szChannel);
HRESULT HrMakeAwayMsg(BYTE **ppb, INT *pcb, CHAR *szAway);
HRESULT HrMakeAuthMsg(BYTE **ppb, INT *pcb, CHAR *szSecurityPackage, CHAR *szSeq, CHAR *szBlob);
HRESULT HrMakeSetPropMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szPropName, CHAR *szPropValue);
HRESULT HrMakeGetPropMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szPropName);

// Not used yet - Might be able to remove these ones
//HRESULT HrMakeRegisterPong(BYTE **ppb, INT *pcb, CHAR *szMsg);

//--------------------------------------------------------------------------------------------
//
// IRC Errors
//
//--------------------------------------------------------------------------------------------
// IRC Result Codes

const INT RPL_IRCSTART			= 1;

const INT RPL_WELCOME			= 1;
const INT RPL_YOURHOST			= 2;
const INT RPL_CREATED			= 3;
const INT RPL_MYINFO			= 4;

const INT RPL_USERHOST			= 302;
const INT RPL_AWAY				= 301;
const INT RPL_UNAWAY			= 305;
const INT RPL_NOWAWAY			= 306;

const INT RPL_WHOISUSER			= 311;
const INT RPL_WHOISSERVER		= 312;
const INT RPL_WHOISOPERATOR		= 313;
const INT RPL_WHOISIDLE			= 317;
const INT RPL_ENDOFWHOIS		= 318;
const INT RPL_WHOISCHANNELS		= 319;

const INT RPL_LISTSTART			= 321;
const INT RPL_LIST				= 322;
const INT RPL_LISTEND			= 323;

const INT RPL_CHANNELMODEIS		= 324;

const INT RPL_NOTOPIC			= 331;
const INT RPL_TOPIC				= 332;

const INT RPL_WHOREPLY			= 352;
const INT RPL_ENDOFWHO			= 315;

const INT RPL_NAMEREPLY			= 353;
const INT RPL_ENDOFNAMES		= 366;

const INT RPL_BANLIST			= 367;
const INT RPL_ENDOFBANLIST		= 368;

const INT RPL_INFO				= 371;
const INT RPL_ENDOFINFO			= 374;

const INT RPL_MOTDSTART			= 375;
const INT RPL_MOTD				= 372;
const INT RPL_MOTD2				= 377;
const INT RPL_ENDOFMOTD			= 376;

const INT RPL_UMODEIS			= 221;

const INT RPL_LUSERCLIENT		= 251;
const INT RPL_LUSEROP			= 252;
const INT RPL_LUSERUNKNOWN		= 253;
const INT RPL_LUSERCHANNELS		= 254;
const INT RPL_LUSERME			= 255;

const INT RPL_IRCEND			= 399;


// IRC Standard error codes

const INT ERR_IRCSTART			= 401;

const INT ERR_NOSUCHSERVER		= 402;
const INT ERR_NEEDMOREPARAMS	= 461;
const INT ERR_NOTREGISTERED		= 451;
const INT ERR_ALREADYREGISTERED = 462;
const INT ERR_TOOMANYTARGETS	= 407;
const INT ERR_NOORIGIN			= 409;
const INT ERR_UNKNOWNCOMMAND	= 421;
const INT ERR_NOMOTD			= 422;
const INT ERR_PASSWDMISMATCH	= 464;
const INT ERR_YOUREBANNEDCREEP	= 465;
const INT ERR_YOUWILLBEBANNED	= 466;
// Nicks
const INT ERR_NOSUCHNICK		= 401;	
const INT ERR_NONICKNAMEGIVEN	= 431;
const INT ERR_ERRONEUSNICKNAME	= 432;
const INT ERR_NICKNAMEINUSE		= 433; 
const INT ERR_NICKCOLLISION		= 436;
const INT ERR_NICKTOOFAST		= 438;
const INT ERR_NICKNOCHANGE		= 439;
// Channels
const INT ERR_NOSUCHCHANNEL		= 403;
const INT ERR_TOOMANYCHANNELS	= 405;
const INT ERR_CHANNELISFULL		= 471;
const INT ERR_INVITEONLYCHAN	= 473;
const INT ERR_BANNEDFROMCHAN	= 474;
const INT ERR_BADCHANNELKEY		= 475;
const INT ERR_USERONCHANNEL		= 443;
const INT ERR_KEYSET			= 467;
// Send
const INT ERR_CANNOTSENDTOCHAN	= 404;
const INT ERR_NORECIPIENT		= 411;
const INT ERR_USERNOTINCHANNEL	= 441;
const INT ERR_NOTONCHANNEL		= 442;
// Modes
const INT ERR_UNKNOWNMODE		= 472;
const INT ERR_NOPRIVILEGES		= 481;
const INT ERR_CHANOPRIVSNEEDED	= 482;
const INT ERR_CHANOWNPRIVNEEDED	= 485;

const INT ERR_UMODEUNKNOWNFLAG	= 501;

const INT ERR_IRCEND			= 501;



// New IRCX replies
const INT RPL_IRCXSTART					= 800;

const INT RPL_IRCX						= 800;

const INT RPL_LISTXSTART				= 811;
const INT RPL_LISTXLIST					= 812;
const INT RPL_LISTXPICS					= 813;
const INT RPL_LISTXTRUNC				= 816;
const INT RPL_LISTXEND					= 817;

const INT RPL_PROPLIST					= 818;
const INT RPL_PROPEND					= 819;

const INT RPL_IRCXEND					= 899;



// New IRCX errors
const INT ERR_IRCXSTART1				= 502;

const INT ERR_CANTCHANGEUSERMODE		= 502;
const INT ERR_NOJOINDYNAMIC				= 552;
const INT ERR_NODYNAMICCHANNELS			= 553;
const INT ERR_AUTHONLY					= 556;
const INT ERR_OVERFLOWABORT				= 557;

const INT ERR_IRCXEND1					= 557;

const INT ERR_IRCXSTART2				= 900;

const INT ERR_BADTAG					= 904;
const INT ERR_BADPROPERTY				= 905;
const INT ERR_BADVALUE					= 906;
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


#define bIsErrorCode(uCode)				((uCode >= ERR_IRCSTART && uCode <= ERR_IRCEND) || (uCode >= ERR_IRCXSTART1 && uCode <= ERR_IRCXEND1) || (uCode >= ERR_IRCXSTART2 && uCode <= ERR_IRCXEND2))
#define bIsReplyCode(uCode)				((uCode >= RPL_IRCSTART && uCode <= RPL_IRCEND) || (uCode >= RPL_IRCXSTART && uCode <= RPL_IRCXEND))
#endif  //  __IRCMSG__

