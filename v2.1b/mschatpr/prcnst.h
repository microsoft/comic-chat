//=--------------------------------------------------------------------------=
// PrCnst.H
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=
//
// Declaration of constants and types used in the MSChat code
//
#ifndef __PRCNST_H__

#include "CUtil.H"
#include "MsChatPr.H"

//----------------------------------------------------------------------------
// Messaging window class name
const TCHAR szMSGWNDCLASS[]					= _T("MsChatPr_MsgWndClass");

const CHAR	g_szModeIsIrcX[]				= "MODE ISIRCX\r\n";
const CHAR	g_szIrcX[]						= "IRCX\r\n";

const LONG	g_lDefaultPropertyAccessTimeOut	= 60000L;	// timeout in milliseconds = 1 minute
const LONG	g_lDefaultMaxMsgLength			= 512L;
const BOOL	g_bDefaultProcessProtocolMessages=TRUE;		// we process the protocol messages by default
const UINT	g_uDefaultPort					= 6667;

const SHORT g_nMaxBannedList				= 8;
const SHORT	g_nMaxServerInfo				= 64;
const SHORT g_nMaxUserChannels				= 8;
const SHORT	g_nMaxPrefix					= 128;
const SHORT	g_nMaxCommand					= 128;
const SHORT g_nMaxParams					= 31;	// IRC2 allows up to 15 arguments, IRCX up to 31
const SHORT	g_nIrcMsgPool					= 200;

// LISTX ages limit
const LONG	g_lAgeLimit						= 0x4444444;

// Authentication magic data key
const DWORD g_dwMagicData1					= 0x27FE1723;	// header of data messages sent/received
const DWORD g_dwMagicData2					= 0x11DE1944;

// Data Stream Type
const BYTE	g_byteSingleton					= 0;	// data is a singleton
const BYTE	g_byteSimpleArray				= 1;	// data is a unfied array of elements
const BYTE	g_byteComplexArray				= 2;	// data is an array of VARIANTs

//----------------------------------------------------------------------------
// Used for persistent characteristics of control
//const WCHAR wszPropertyAccessTimeOut[]		= L"PropertyAccessTimeOut";
//const WCHAR wszProcessProtocolMessages[]	= L"ProcessProtocolMessages";

// Query types
typedef enum
{
	qtNamesForUsers,
	qtNamesForMembers,
	qtUserList
} enumQueryType;


const SHORT	g_nQueryOperators				= 3;

// Must be in the same order as g_rgwszQueryOperators
typedef enum
{
	qoContains,
	qoEndsWith,
	qoStartsWith
} enumQueryOperator;

// Must be in the same order as enumQueryOperator
const LPCWSTR g_rgwszQueryOperators[]=
{
	L"Contains",
	L"EndsWith",
	L"StartsWith"
};

// Must be in the same order as enumQueryOperator
const LPCSTR g_rgszQueryOperators[]=
{
	_T("Contains"),
	_T("EndsWith"),
	_T("StartsWith")
};


const SHORT	g_nAssociatedTypes				= 5;

// Must be in the same order as g_rgwszAssociatedTypes
typedef enum
{
	atUser,
	atMember,
	atChannel,
	atServer,
	atQuery
} enumAssociatedType;

// Must be in the same order as enumAssociatedType
const LPCWSTR g_rgwszAssociatedTypes[]=
{
	L"User",
	L"Member",
	L"Channel",
	L"Server",
	L"Query"
};


const SHORT g_nServerPropertyNames			= 14;

// Must be in the same order as g_rgwszServerPropertyNames
typedef enum
{
	pnServerAnonymousAllowed,
	pnServerChannelCount,
	pnServerIgnoredUsers,
	pnServerInfo,
	pnServerMaxMessageLength,
	pnServerName,
	pnServerNetInvisibleCount,
	pnServerNetServerCount,
	pnServerNetUserCount,
	pnServerNodeServerCount,
	pnServerNodeUserCount,
	pnServerSecurityPackages,
	pnServerSysopCount,
	pnServerUnknownConnectionCount
} enumServerPropertyName;

const LPCWSTR g_rgwszServerPropertyNames[]=
{
	L"AnonymousAllowed",
	L"ChannelCount",
	L"IgnoredUsers",
	L"Info",
	L"MaxMessageLength",
	L"Name",
	L"NetInvisibleCount",
	L"NetServerCount",
	L"NetUserCount",
	L"NodeServerCount",
	L"NodeUserCount",
	L"SecurityPackages",
	L"SysopCount",
	L"UnknownConnectionCount"
};


const SHORT g_nChannelPropertyNames 		= 34;

// Must be in the same order as g_rgwszChannelPropertyNames
typedef enum
{
	cpnAccount,
	cpnBannedList,
	cpnChannelAgeMax,
	cpnChannelAgeMin,
	cpnClientData,
	cpnClientGuid,
	cpnCloneList,
	cpnCreationTime,
	cpnHostKey,
	cpnKeyword,
	cpnLag,
	cpnLanguage,
	cpnLanguageOp,
	cpnListCount,
	cpnMaxMemberCount,
	cpnMemberCount,
	cpnMemberCountMax,
	cpnMemberCountMin,
	cpnModes,
	cpnName,
	cpnNameOp,
	cpnObjectId,
	cpnOnJoin,
	cpnOnPart,
	cpnOwnerKey,
	cpnRating,
	cpnRegistered,
	cpnServicePath,
	cpnSubject,
	cpnSubjectOp,
	cpnTopic,
	cpnTopicAgeMax,
	cpnTopicAgeMin,
	cpnTopicOp
} enumChannelPropertyName;

// Must be in the same order as enumChannelPropertyName
const LPCWSTR g_rgwszChannelPropertyNames[]=
{
	L"Account",
	L"BannedList",
	L"ChannelAgeMax",
	L"ChannelAgeMin",
	L"ClientData",
	L"ClientGuid",
	L"CloneList",
	L"CreationTime",
	L"HostKey",
	L"Keyword",
	L"Lag",
	L"Language",
	L"LanguageOp",
	L"ListCount",
	L"MaxMemberCount",
	L"MemberCount",
	L"MemberCountMax",
	L"MemberCountMin",
	L"Modes",
	L"Name",
	L"NameOp",
	L"ObjectId",
	L"OnJoin",
	L"OnPart",
	L"OwnerKey",
	L"Rating",
	L"Registered",
	L"ServicePath",
	L"Subject",
	L"SubjectOp",
	L"Topic",
	L"TopicAgeMax",
	L"TopicAgeMin",
	L"TopicOp"
};


// Must be in the same order as g_rgwszChannelPropertyNames
const LPCTSTR g_rgszIrcxChannelPropertyNames[]=
{
	_T("ACCOUNT"),
	_T(""),
	_T(""),
	_T(""),
	_T("CLIENT"),
	_T("CLIENTGUID"),
	_T(""),
	_T("CREATION"),
	_T("HOSTKEY"),
	_T("MEMBERKEY"),
	_T("LAG"),
	_T("LANGUAGE"),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T(""),
	_T("MODE"),
	_T("NAME"),
	_T(""),
	_T("OID"),
	_T("ONJOIN"),
	_T("ONPART"),
	_T("OWNERKEY"),
	_T("PICS"),
	_T(""),
	_T("SERVICEPATH"),
	_T("SUBJECT"),
	_T("TOPIC"),
	_T(""),
	_T(""),
	_T("")
};


const SHORT g_nMemberPropertyNames			= 9;

// Must be in the same order than g_rgwszMemberPropertyNames
typedef enum
{
	pnMemberIdentity,
	pnMemberIPAddress,
	pnMemberIPAddressOp,
	pnMemberModes,
	pnMemberNickname,
	pnMemberNicknameOp,
	pnMemberObjectId,
	pnMemberUserName,
	pnMemberUserNameOp
} enumMemberPropertyName;

const LPCWSTR g_rgwszMemberPropertyNames[]=
{
	L"Identity",
	L"IPAddress",
	L"IPAddressOp",
	L"Modes",
	L"Nickname",
	L"NicknameOp",
	L"ObjectId",
	L"UserName",
	L"UserNameOp"
};


const SHORT g_nUserPropertyNames			= 20;

// Must be in the same order as g_rgwszUserPropertyNames
typedef enum
{
	pnUserAway,
	pnUserChannels,
	pnUserHostInChannels,
	pnUserIdentity,
	pnUserIdleTime,
	pnUserIPAddress,
	pnUserIPAddressOp,
	pnUserMask,
	pnUserMaskOp,
	pnUserModes,
	pnUserNickname,
	pnUserNicknameOp,
	pnUserOwnerOfChannels,
	pnUserRealName,
	pnUserServerInfo,
	pnUserServerName,
	pnUserSignOnTime,
	pnUserUserName,
	pnUserUserNameOp,
	pnUserVoiceInChannels
} enumUserPropertyName;

const LPCWSTR g_rgwszUserPropertyNames[]=
{
	L"Away",
	L"Channels",
	L"HostInChannels",
	L"Identity",
	L"IdleTime",
	L"IPAddress",
	L"IPAddressOp",
	L"Mask",
	L"MaskOp",
	L"Modes",
	L"Nickname",
	L"NicknameOp",
	L"OwnerOfChannels",
	L"RealName",
	L"ServerInfo",
	L"ServerName",
	L"SignOnTime",
	L"UserName",
	L"UserNameOp",
	L"VoiceInChannels"
};


// Message type
typedef enum
{
	mtNormal,				// regular message for a channel
	mtWhisper,				// whisper
	mtThought,				// ComicChat thought
	mtBroadcast,
	mtAction,				// IRC or ComicChat action
	mtSound,				// CTCP Sound
	mtAppearsAs,			// ComicChat # Appears as message
	mtBackground,			// ComicChat # Background message
	mtChr,					// ComicChat (#<anything>) Chr message
	mtGetInfo,				// ComicChat # GetInfo message
	mtVersion,				// Version information request
	mtLagTimeRequest,		// Lag Time information request
	mtLagTimeReply,			// Lag Time information reply
	mtLocalTimeRequest,		// Local Time information request
	mtLocalTimeReply,		// Local Time information reply
	mtCTCP,					// For all other CTCP messages
	mtEndEnum				// The last enumeration, which gives us the count
} enumMsgType;


typedef struct tagPRITEM
{
	VARIANT	vName;		// Topic
	VARIANT	vValue;		// BSTR: Let's talk about Corvettes
} PRITEM, *PPRITEM;


typedef struct tagPRMEMBER
{
	LPTSTR	szNickname;		// Justin
	LPTSTR	szUserName;		// regisb
	LPTSTR	szIPAddress;	// 157.55.65.36
	LONG	lModes;
	LONG	lOID;
} PRMEMBER, *PPRMEMBER;


typedef struct tagPRUSER
{
	LPTSTR		szNickname;			// Justin
	LPTSTR		szRealName;			// Regis Brid de Schmolblutz
	LPTSTR		szUserName;			// regisb
	LPTSTR		szIPAddress;		// 157.55.65.36	(also called host)
	LPTSTR		szServerName;		// KEEZER
	LPTSTR		szServerInfo;		// Microsoft Internet Chat server 2.0
	LPTSTR		szAway;				// Out with the dog
	SAFEARRAY	*psaChannels;		// @#Channel1, +#ModeratedChannel2, #Newbies, .#ChannelProut
	SAFEARRAY	*psaChannelsOwner;	// .#ChannelProut
	SAFEARRAY	*psaChannelsHost;	// @#Channel1
	SAFEARRAY	*psaChannelsVoice;	// +#ModeratedChannel2
	LONG		lChannelCount;		// 3
	LONG		lChannelOwnerCount;	// 1
	LONG		lChannelHostCount;	// 1
	LONG		lChannelVoiceCount; // 1
	LONG		lModes;				// Authenticated or not?
	LONG		lIdleTime;			// 56 seconds
	LONG		lSignOnTime;		// 89557456 seconds
} PRUSER, *PPRUSER;


typedef struct tagPRCHANNEL
{
	LPTSTR		szName;
	LPTSTR		szTopic;
	LPTSTR		szKeyword;
	LPTSTR		szHostKey;
	LPTSTR		szOwnerKey;
	LPTSTR		szRating;
	LPTSTR		szLanguage;
	LPTSTR		szClientData;
	LPTSTR		szClientGuid;
	LPTSTR		szOnJoin;
	LPTSTR		szOnPart;
	LPTSTR		szAccount;
	LPTSTR		szServicePath;
	LPTSTR		szSubject;
	SAFEARRAY	*psaCloneList;
	SAFEARRAY	*psaBannedList;
	LONG		lCloneListCount;
	LONG		lBannedListCount;
	LONG		lMemberCount;
	LONG		lMaxMemberCount;
	LONG		lCreationTime;
	LONG		lLag;
	LONG		lModes;
	LONG		lOID;
	PVOID		pChannel;
	BOOL		bFree;
	BOOL		bMicOnly;
} PRCHANNEL, *PPRCHANNEL;


typedef struct tagPRSERVER
{
	SAFEARRAY	*psaInfo;				// Info strings for the Info property
	SAFEARRAY	*psaIgnored;			// ChatItems array of ignored users
	LONG		lInfoCount;
	LONG		lChannelCount;
	LONG		lNetInvisibleCount;
	LONG		lNetServerCount;
	LONG		lNetUserCount;
	LONG		lNodeServerCount;
	LONG		lNodeUserCount;
	LONG		lSysopCount;
	LONG		lUnknownConnectionCount;
} PRSERVER, *PPRSERVER;


typedef struct tagPRQUERY
{
	enumAssociatedType	at;				// can be atUser or atMember
	BOOL				bSyncAccess;
	UINT				uHeadReplyCode;
	LPTSTR				szChannelName;
	PVOID				pvData;			// can point to a PRUSER or PRMEMBER structure
} PRQUERY, *PPRQUERY;


typedef struct tagPRSECURITY
{
	DWORD	dwcb;						// size of this structure + that of rgch(below). Always >= sizeof(CS_SECURITY)
	INT		cPackages;					// # of security packages supported by the server
										// you are currently logged into
	BOOL	bAnonAllowed;				// does this server permit anonymous log ins?
										// The remainder of the block is a series of 
										// contiguous null terminated CHAR strings. Each string
										// is the name of a security package
} PRSECURITY, *PPRSECURITY;


typedef struct tagPRIRCMSG
{
	BOOL	bFree;
	BYTE	byteParams;
	CHAR	szPrefix[g_nMaxPrefix];
	CHAR	szCommand[g_nMaxCommand];
	UINT	uCode;
	CHAR	*szParams[g_nMaxParams];
	SHORT	nParamsLen[g_nMaxParams];
} PRIRCMSG, *PPRIRCMSG;

// External common functions
extern void		FreePrChannelContent(PPRCHANNEL pPrChannel);
extern void		FreePrMemberContent(PPRMEMBER pPrMember);
extern void		FreePrUserContent(PPRUSER pPrUser);

#define __PRCNST_H__
#endif __PRCNST_H__
