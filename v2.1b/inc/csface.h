//--------------------------------------------------------------------------------------------
//
//	Copyright (c) Microsoft Corporation, 1996-1997.  All Rights Reserved.
//
//	Description:
//
//		Microsoft Internet Chat Sockets.
//
//		Client Interfaces
//
//--------------------------------------------------------------------------------------------

#ifndef __CSFACE__
#define __CSFACE__

//--------------------------------------------------------------------------------------------
//
// INCLUDES.
//
//--------------------------------------------------------------------------------------------
#include <windows.h>
#include <objbase.h>
#include <MicMsg.h>
#include "cserror.h"
#include <pshpack4.h>

//--------------------------------------------------------------------------------------------
//
// MACROS.
//
//--------------------------------------------------------------------------------------------

#ifdef __CHATSOCKDLL

#define CHATSOCKAPI			__declspec(dllexport) HRESULT __stdcall
#define CHATSOCKAPI_(type)	__declspec(dllexport) type __stdcall	

#else

#define CHATSOCKAPI			__declspec(dllimport) HRESULT __stdcall
#define CHATSOCKAPI_(type)	__declspec(dllimport) type __stdcall	

#endif

//--------------------------------------------------------------------------------------------
//
// DECLARATIONS.
//
//--------------------------------------------------------------------------------------------
// 
// LIMITS - max/min on message lengths, channel name lengths, etc..
//
#ifdef __cplusplus
const int CS_CCBMAX_MIC				= (MIC_MAX_MESSAGE);
const int CS_CCBMAX_MIC_MSG			= (MICMSG_MAX_MSGBYTES);
const int CS_CCBMAX_IRC_MSG			= 510;

const int CS_CCHMAX_MIC_USERNAME	= MIC_MAX_USER_NAME_LENGTH;
const int CS_CCHMAX_MIC_PASS		= MIC_MAX_USER_PASSWORD_LENGTH;
const int CS_CCHMAX_IRC_USERNAME	= 63;
const int CS_CCHMAX_IRC_PASS		= MIC_MAX_USER_PASSWORD_LENGTH;
const int CS_CCHMAX_MIC_NICK		= MIC_MAX_USER_ALIAS_LENGTH;
const int CS_CCHMAX_IRC_NICK		= 9;	// IRC nicks are limited to 9 chars - ANSI
const int CS_CCHMAX_IRC_DESCRIPTION = 63;

const int CS_CCHMIN_MIC_CHANNEL		= MIC_MIN_CHANNEL_NAME_LENGTH_MIC; // applies to MIC ONLY Channels
const int CS_CCHMAX_MIC_CHANNEL		= MIC_MAX_CHANNEL_NAME_LENGTH_MIC; // applies to MIC ONLY Channels
const int CS_CCHMIN_IRC_CHANNEL		= 1; // applies to ALL NON-MIC only channels
const int CS_CCHMAX_IRC_CHANNEL		= 200; // applies to ALL NON-MIC only channels
const int CS_CCHMAX_MIC_TOPIC		= MIC_MAX_CHANNEL_TOPIC_LENGTH;
const int CS_CCHMAX_CHANNEL_PASS	= MIC_MAX_CHANNEL_KEYWORD_LENGTH; // if you have a password on your channel

const int CS_CCHMAX_QUERY_STRING	= 200; // max length of ANY string in ANY MIC QUERY
const int CS_CCHMAX_BANMASK_MIC		= MIC_MAX_CHANNEL_BANNED_LENGTH;
const int CS_CCHMAX_MIC_AWAYMSG		= MIC_MAX_USER_AWAY_LENGTH;
const int CS_CMAX_WHISPERMEMBERS	= MICMSG_MAX_WHISPERS;	// MIC ONLYcan whisper to these many 
															// members at a time.. max. Beyond
															// this, whispers become unweildy
															// and pointless.
const int CS_CMAX_MIC_PROPS			= MICMSG_MAX_PROPS;		// you can retrieve these many
															// props from the server simultaneously
															// 
const int CS_CMAX_MIC_TERMS			= MICMSG_MAX_TERMS/3;	// you can have these many terms in a
															// query at the most

#else

#define CS_CCBMAX_MIC				  (MIC_MAX_MESSAGE)
#define CS_CCBMAX_MIC_MSG			  (MIC_MAX_MESSAGE - sizeof(MICMSG_SENDMSG))
#define CS_CCBMAX_IRC_MSG			  (512)
#define CS_CCHMAX_MIC_USERNAME		  (MIC_MAX_USER_NAME_LENGTH)
#define CS_CCHMAX_MIC_PASS			  (MIC_MAX_USER_PASSWORD_LENGTH)
#define CS_CCHMAX_MIC_NICK			  (MIC_MAX_USER_ALIAS_LENGTH)
#define CS_CCHMAX_IRC_NICK			  (9)
#define CS_CCHMIN_MIC_CHANNEL		  (MIC_MIN_CHANNEL_NAME_LENGTH)
#define CS_CCHMAX_MIC_CHANNEL		  (MIC_MAX_CHANNEL_NAME_LENGTH)
#define CS_CCHMIN_IRC_CHANNEL		  (1)
#define CS_CCHMAX_IRC_CHANNEL		  (200)
#define CS_CCHMAX_MIC_TOPIC			  (MIC_MAX_CHANNEL_TOPIC_LENGTH)
#define CS_CCHMAX_CHANNEL_PASS		  (MIC_MAX_CHANNEL_KEYWORD_LENGTH)
#define CS_CCHMAX_QUERY_STRING		  (31)
#define	CS_CCHMAX_BANMASK_MIC		  (MIC_MAX_CHANNEL_BANNED_LENGTH)
#define CS_CCHMAX_MIC_AWAYMSG		  (MIC_MAX_USER_AWAY_LENGTH)
#define CS_CMAX_WHISPERMEMBERS		  (MICMSG_MAX_WHISPERS)
#define CS_CMAX_MIC_PROPS			  (MICMSG_MAX_PROPS)
#define CS_CMAX_MIC_TERMS			  (MICMSG_MAX_TERMS/3)

#endif
//
// Advance TYPE DECLARATIONS
//
DECLARE_INTERFACE(IChatSocketFactory);
DECLARE_INTERFACE(IChatSocket);
DECLARE_INTERFACE(ICSChannel);
DECLARE_INTERFACE(ICSMember);
DECLARE_INTERFACE(ICSQuery);
DECLARE_INTERFACE(ICSProperty);
DECLARE_INTERFACE(ICSPrivateMsg);
DECLARE_INTERFACE(ICSInvitation);

typedef	IChatSocketFactory	*PICS_FACTORY;
typedef	IChatSocket			*PICS;
typedef	ICSChannel			*PICS_CHANNEL;
typedef	ICSMember			*PICS_MEMBER;
typedef	ICSQuery			*PICS_QUERY;
typedef	ICSProperty			*PICS_PROPERTY;
typedef ICSPrivateMsg		*PICS_PRIVMSG;
typedef ICSInvitation		*PICS_INVITATION;


typedef		BYTE				CSMSG_TYPE;
typedef		BYTE				CSMSG_FLAG;
typedef		USHORT				CSPROP_TYPE;
typedef		USHORT				CSPROP_OP;		
typedef		BYTE				CSPROP_FLAG;
//
// Call back to return broadcast messages to callers
//
typedef HRESULT (CALLBACK* CSBROADCASTPROC)(PICS_CHANNEL pChannel,BYTE *pb,DWORD dwcb);
//
// Note: ALL structures are packed to 4 byte boundaries
//
//
// Passed to ChatSock when Connecting to a server
//
typedef struct ChatSocketConnectInfo
{
	DWORD	dwcb;				// IN *size of this structure
	BYTE	bType;				// IN *what sort of connection? eg. Anonymous. IRC - alway anonymous
	DWORD	dwUserMode;			// IN *log in with what mode? eg.No Whisper
	PVOID	pvUser;				// IN *user name to use. Required.
	PVOID	pvPass;				// IN *password. Required if pvUser is NOT NULL 
								// On authenticated login, you will be prompted by the security
								// dll if pvUser or pvPass are NULL
	PVOID	pvNick;				// IN *your nick name. Required.
	CHAR	*szSecurityPackage;	// IN *a null terminated string that specifies the security package
								// to use during authentication. Ignored for Anonymous connections
	CHAR	*szDescription;		// IN *IRC only - ignored on MIC sockets. 
								// Allows you to describe yourself - with a real name..
								// If NULL, pvUser is used as the description
	DWORD	dwReserved;			// Reserved for ChatSock use

} CS_CONNINFO, *PCS_CONNINFO;

//
// Passed to ChatSock when creating a channel
// structure. 
//
typedef struct ChatSocketChannelInfo
{
	DWORD	dwcb;				// IN *size of this structure
	DWORD	dwType;				// IN *channel type
	DWORD	dwFlags;			// IN *channel flags - IGNORED on IRC Sockets - further customize channels
	BYTE	bCreateFlags;		// IN *channel creation flags. How should this channel be created? IGNORED on IRC.
	DWORD	dwUserFlags;		// IN *customize how the server handles YOU in this channel. IGNORED on IRC
	PVOID	pvChannelName;		// IN *channel name
	PVOID	pvTopic;			// IN *topic name
	PVOID	pvPassword;			// IN *password for this channel (can be NULL)
	DWORD	dwcUserMax;			// IN *max # of users allowed in this channel. To use server default, set to 0
	DWORD	dwReserved;			// Reserved for ChatSock use

} CS_CINFO, *PCS_CINFO;

//
// Passed to ChatSock when Joining a Channel
//
typedef struct ChatSocketJoinChannelInfo
{
	DWORD	dwcb;				// IN *size of this structure
	PVOID	pvChannelName;		// IN *name of channel to join
	PVOID	pvPassword;			// IN *some channels are password protected. 
								// If password is not provided and channel is password 
								// protected, join will fail. You can however pass in NULL if you don't have a password
	DWORD	dwUserFlags;		// IN *customize how the server handles YOU in this channel.
	DWORD	dwReserved;

} CS_JOININFO, *PCS_JOININFO;

//
// Passed to Chatsock when sending a private msg
//
typedef struct ChatSocketPrivMsgInfo
{
	DWORD	dwcb;				// IN *size of this structure
	//
	// You may provide *EITHER* the user ID of the target user (if you have it) *OR* their
	// nickname. You cannot provide BOTH - because that generated ambiguity.
	// UserIDs are used in the MIC protocol - so you can ignore them for IRC.
	// If you don't want to provide dwUserID, set it to 0
	// If you don't have the pvNickTo - set it to NULL
	//
	DWORD	dwUserID;			// IN *MIC only. 0 if not available.
	PVOID	pvNickTo;			// IN *name of the user to send this msg to
	BOOL	fData;				// IN *are you sending data or text? Data is allowed on MIC only
	BYTE	*pbData;			// IN *data to send. if fData is FALSE, then this must be 
								// a null terminated string
	DWORD	dwcbData;			// IN *size of data..if 0, then pbData is assumed to be a
								// null terminated string 

} CS_PRIVMSGINFO,*PCS_PRIVMSGINFO;

//
// Passed to ChatSock when Inviting a user to a channel
//
typedef struct ChatSocketInvite
{
	DWORD	dwcb;				// IN *size of this structure
	//
	// You may provide *EITHER* the ID of the target user (if you have it) *OR* their
	// nickname. You cannot provide BOTH - because that generated ambiguity.
	// UserIDs are used in the MIC protocol - so you can ignore them for IRC.
	// If you don't want to provide dwUserID, set it to 0
	// If you don't have the pvNickTo - set it to NULL
	//
	DWORD	dwUserID;			// IN *MIC only. 0 if not available.
	PVOID	pvNickTo;			// IN *name of the user to send this msg to
	PVOID	pvChannel;			// IN *name of the channel to invite this user to
								// The channel does not have to currently exist
								// When calling ICSChannel::HrInvite, this parameter
								// is ignored
								//
		
} CS_INVITEINFO,*PCS_INVITEINFO;

//
// Used by YOU to pass data to ICSSocket.
// MIC ONLY - IRC can't do this.
//
typedef struct ChatSocketQueryTerm
{
	BOOL			fAndOr;		// IN* And/Or the results of this Term with the others. TRUE if And
	CSPROP_TYPE		csPropType;	// IN* The property type in question
	CSPROP_OP		csOp;		// IN* operators like == or <=..matches are done against what
								// you pass in pbData.
	BYTE			*pbData;	// IN* points to the data required by csPropType
	DWORD			dwcbData;	// size of pvData

} CS_QUERYTERM_MIC, *PCS_QUERYTERM_MIC;

//
// MIC ONLY - IRC can't do this
//
typedef struct ChatSocketQueryData
{
	DWORD				dwcb;		// IN* size of this structure
	BYTE				bcTerms;	// IN* how many terms in the query - must be <= CS_CMAX_MIC_TERMS
	BYTE				bcProps;	// IN* how many properties do we want server to return..
	CS_QUERYTERM_MIC	rgTerms[CS_CMAX_MIC_TERMS]; // IN* the actual query
	CSPROP_TYPE			rgProps[CS_CMAX_MIC_PROPS];	// IN* properties you want the server to return
												// after it makes a match based on rgTerms
} CS_QUERY_MIC, *PCS_QUERY_MIC;

// Use to pass info to ICSocket::HrListAllChannelsEx
//
typedef struct ChatSocketListChanData
{
	DWORD		dwcb;		// IN* size of this structure
	DWORD		dwcUserMin;	// IN* min # of users desired in a matched channel
	DWORD		dwcUserMax;	// IN* max # of users desired in a matched channel
	PVOID		pvChannel;	// IN* if you want to search on a channel name. 
	CSPROP_OP	csopName;	// IN* operator you want to use when matching pvChannelName against
							// the channel database
	DWORD		dwType;		// IN* and what type of channel should this be? 0 if you don't care
	DWORD		dwFlags;	// IN* and what flags.. 0 if you don't care

}CS_LISTCHANINFO,*PCS_LISTCHANINFO;

//
// Used to pass data to IChatSocket::HrKillUser
//
typedef struct ChatSocketKillInfo
{
	DWORD		dwcb;		// IN* size of this structure
	//
	// You may provide *EITHER* the ID of the target user (if you have it) *OR* their
	// nickname. You cannot provide BOTH - because that generated ambiguity.
	// UserIDs are used in the MIC protocol - so you can ignore them for IRC.
	// If you don't want to provide dwUserID, set it to 0
	// If you don't have the pvNick - set it to NULL
	//
	DWORD	dwUserID;			// IN *MIC only. 0 if not available.
	PVOID	pvNick;				// IN *name of the user to kill from the system. 
	PVOID	pvReason;			// IN *A reason for this user to be kicked
								//

}CS_KILLINFO,*PCS_KILLINFO;

//
// Used by IChatSocket::HrGetSecurityOptions to return data to you
//
typedef struct ChatSocketSecurity
{
	DWORD dwcb;						// size of this structure + that of rgch(below). Always >= sizeof(CS_SECURITY)
	int	cPackages;					// # of security packages supported by the server
									// you are currently logged into
	BOOL fAnonAllowed;				// does this server permit anonymous log ins?
	//rgch[cPackages]				// The remainder of the block is a series of 
									// contiguous null terminated CHAR strings. Each string
									// is the name of a security package
}CS_SECURITY,*PCS_SECURITY;


//
// Used by ICSProperty methods to return stuff to you.
//
typedef struct ChatSocketPropertyData
{
	BOOL			fAnsi;		// OUT* if the data is a string, is it ANSI
	BOOL			fString;	// OUT* is the property a null terminated string? If so dwcb will be 0
	CSPROP_TYPE		csPropType;	// OUT* the type of property data
	BYTE			*pbData;	// OUT* the data for this prop.. NULL terminated if fSting is TRUE
	DWORD			dwcb;		// OUT* the size of the data. 0 if fString is TRUE.

} CS_PROPDATA, *PCS_PROPDATA;

//
// Used by ICSPrivateMsg to return stuff to you
//
typedef struct ChatSocketPrivateMsg
{
	BOOL			fText;		// OUT* is the data TEXT?
	BOOL			fAnsi;		// OUT* if fText is TRUE, then is the text ANSI?
	BYTE			*pbData;	// OUT* pointer to the data in the  private msg. NULL terminated
								// if fText is true
	DWORD			dwcb;		// OUT* size of the data..
		
} CS_PRIVMSG,*PCS_PRIVMSG;

//---------------------------------
// MESSAGES
//---------------------------------
//
// Pointer to this is returned by ICSChannel::HrWaitForMsg.and ICSocket::HrWaitForMsg
// This is a header for a variable length msg structure.
// The msg is a block with this header - and csMsgType indicates how you should interpret
// the remainder of the msg
// To extract msgs correctly, use the macro MSGBASE_TO_MSG
// 
// When ChatSock posts a message, it provides you a pointer to a CS_MSGBASE.
// This is a header on a block of data whose size and content varies, based on CS_MSGBASE.csMsgType
// Once you have determined what csMsgType is, you know what sort of data follows CS_MSGBASE
// To correctly obtain that data, USE the MACRO	MSGBASE_TO_MSG
//
//
// NOTE: many messages come just with this header, as they don't need any other info
// These are:
//	CSMSG_TYPE_GOTMEMLIST,CSMSG_TYPE_LOGIN,CSMSG_TYPE_MODECHANNEL,CSMSG_TYPE_NEWTOPIC
typedef struct ChatSocketMsgBase
{
	CSMSG_TYPE	csMsgType;		// OUT *type of message
	DWORD		dwcb;			// OUT *size of the message
	CSMSG_FLAG	csMsgFlag;		// OUT *flags set to indicate Whisper or any other future 
								// modifiers. For a listing of flags, look for constants of
								// the type CSMSG_FLAG_*. NOTE: these are BIT wise flags, so
								// do an & or a |.
								// This should usually be set to CSMSG_FLAG_NONE

} CS_MSGBASE, *PCS_MSGBASE;

//
// Sent with csMsgType == CSMSG_TYPE_ADDCHANNEL or CSMSG_TYPE_DELCHANNEL 
//
typedef struct ChatSocketChannel
{
	PICS_CHANNEL	picsChannel;

} CS_MSGCHANNEL, *PCS_MSGCHANNEL;

//
// Sent with csMsgType == CSMSG_TYPE_ADDMEMBER or CSMSG_TYPE_DELMEMBER 
// or CSMSG_TYPE_MODEMEMBER
//
typedef struct ChatSocketMember
{
	PICS_MEMBER	picsMember;	// member to which this msg applies
							// if this value is NULL, then the msg was sent by the CHANNEL
							// This is especially true of Channel Services like Eliza.
	PICS_MEMBER picsMemSrc;	// IF NOT NULL -> then indicates the member who might have, due
							// to some action, caused this msg to be generated.
							// eg. picsMember is getting a DELMEMBER because picsMemSrc
							// kicked him/her.
	PVOID		pvReason;	// If this member was kicked, then this points to the reason (if any)
							// The reason is always is a NULL terminated string
	BOOL		fAnsi;		// is pvReason ANSI or Unicode

	DWORD		dwPrevMode;	// contains the previous mode for this member if their mode changed. Else set to 0.

} CS_MSGMEMBER, *PCS_MSGMEMBER;

//
// Sent with csMsgType == CSMSG_TYPE_TEXT_A,CSMSG_TYPE_TEXT_W,CSMSG_TYPE_DATA to the 
// channel message que
//
typedef struct ChatSocketMsg
{
	PICS_MEMBER		picsFrom;		// OUT *which member is it from?	If NULL, is from the channel
	BYTE			*pbData;		// OUT *the data we received. Can be NULL
	DWORD			dwcbData;		// OUT *how much data... If pbData is NULL, will be 0

} CS_MSG, *PCS_MSG;

//
// Sent with csMsgType == CSMSG_TYPE_WHISPERTEXT_A,CSMSG_TYPE_WHISPERTEXT_W,
// and CSMSG_TYPE_WHISPERDATA
//
typedef struct ChatSocketWhisper
{
	PCS_MSG			pcsMsg;			// OUT* the actual msg.
	PICS_MEMBER		*prgMember;		// OUT* ptr to an array of recipients
	DWORD			dwcMem;

} CS_MSGWHISPER, *PCS_MSGWHISPER;
//
// Sent with csMsgType == CSMSG_TYPE_ERROR. 
//
typedef struct ChatSocketError
{
	HRESULT	hr;		// OUT * error code	

} CS_ERROR, *PCS_ERROR;

//
// Sent with CSMSG_TYPE_PROPERTYDATA and CSMSG_TYPE_QUERYDATA
// Think of this is a database. Property Types are the fields. Each PICS_PROPERTY pointer
// points to the Record that has the data for those fields.
// The server could return multiple property records on every query, often in the same
// block of data. ChatSock parses this data and breaks it up on the record boundaries..
// and sends the data to you ONE record at a time. That is where the fLastRecord field
// comes in. 
// fLastRecord is TRUE if this is the last record in the data set
// fNestedParent is TRUE if:
//	Some property queries return NESTED data sets. eg. HrNames() or any MIC queries that
// include a member list. So with HrNames(), you will get a channel's information (with 
// fNestedParent == TRUE) and the all of the members on that channel..and then the next
// channel.. and so forth.
//
typedef struct ChatSocketProperty
{
	BOOL			fLastRecord;	// OUT* is this the last record?
	BOOL			fNestedParent;	// OUT* are subsequent properties "children" of this one
	PICS_PROPERTY	picsProperty;	// OUT* pointer to a property manipulation object. May be
									// NULL if fLastRecord or fNestedParent is True. 
									//
} CS_PROPERTY, *PCS_PROPERTY;

//
// Sent with CSMSG_TYPE_NEWNICK
//
typedef struct ChatSockNewNick
{
	CS_MSGMEMBER	csMsgMember;	// OUT* whose nick changed..
	PVOID			pvOldNick;		// OUT* the previous nick
	BOOL			fAnsi;			// OUT* was the old nick in Ansi or Unicode?

} CS_NEWNICK, *PCS_NEWNICK;
//
// Sent with CSMSG_TYPE_PRIVMSG 
//
typedef struct ChatSockPrivateMessage
{
	PICS_PRIVMSG	picsPrivMsg;	// OUT* ptr to an interface that lets you manipulate the msg
		
} CS_MSGPRIVATE,*PCS_MSGPRIVATE;

//
// Sent with CSMSG_TYPE_INVITE
//
typedef struct ChatSockInvite
{
	PICS_INVITATION	picsInvite;

} CS_MSGINVITE,*PCS_MSGINVITE;

//
// Sent with CSMSG_TYPE_SERVERMSG_*
// A message from the server. the msg is forwarded as is..
//
typedef struct ChatSockServerMsg
{
	BYTE	*pbData;
	DWORD	dwcb;

} CS_MSGSERVER,*PCS_MSGSERVER;

//
// pMsgBase is of type CS_MSGBASE
// type is the type of struct you want to cast it to. 
// So if you got pMsgBase->csMsgType == CSMSG_TYPE_TEXTA, you would do:
// MSGBASE_TO_MSG(pMsgBase,PCS_MSG)
// Similary, CSMSG_TYPE_ADDCHANNEL would produce MSGBASE_TO_MSG(pMsgBase,PCS_MSGCHANNEL)
//
#define MSGBASE_TO_MSG(pMsgBase,type)	(type) (pMsgBase + 1)

//
// Chat Socket Types
//
#ifdef __cplusplus

const BYTE	CS_INVALID		= 0;
const BYTE	CS_MIC			= 1;
const BYTE	CS_IRC			= 2;

#else

#define CS_INVALID	0
#define CS_MIC		1
#define CS_IRC  2

#endif

#define		CS_DEFAULT							(CS_MIC)
//
// Server Connect Authentication types
//
#ifdef __cplusplus

const BYTE	CS_CONNECT_INVALID		= 0;
const BYTE	CS_CONNECT_ANONYMOUS	= 1;
const BYTE	CS_CONNECT_AUTHENTICATE	= 2;

#else

#define CS_CONNECT_INVALID			0
#define CS_CONNECT_ANONYMOUS		1
#define CS_CONNECT_AUTHENTICATE		2

#endif
//
// USER MODES - The user's current mode on the server, and as seen by everybody else. 
// Use these when you LOG on. This will set your preferences for the session.
// So, if you turn on the NoWhisper flag when you login, you will NEVER receive whipsers,
// even if you are in a channel that permits Whispers.
// MIC ONLY MIC ONLY
//
#define CS_USER_NOWHISPER			(MIC_USERMODE_NOWHISPER)	// user hates whispers
#define CS_USER_INVISIBLE			(MIC_USERMODE_INVISIBLE)	// make the user invisible. 
#define CS_USER_NODATA				(MIC_USERMODE_NODATA)		// the user will not receive any data - text only
//
// These are now Read-Only, set by the server and hence no longer settable from
// the client. They exist purely for backward compatibility
//
#define CS_USER_SYSOP				MIC_USERMODE_SYSOP
#define CS_USER_AUTHUSER		    MIC_USERMODE_AUTHUSER
#define CS_USER_IRC					MIC_USERMODE_IRC
//
// MEMBER Modes - OTHER user's modes. the users you are actually chatting with
//
#define CS_MEMBER_NOWHISPER			(MIC_MEMBERMODE_NOWHISPER)	// member does not accept whispers
#define CS_MEMBER_HOST				(MIC_MEMBERMODE_HOST)		// member is a host
#define CS_MEMBER_SPEAKER			(MIC_MEMBERMODE_SPEAKER)	// member can speak in the chat
#define CS_MEMBER_SYSOP				(MIC_MEMBERMODE_SYSOP)		// member is a sysop
#define CS_MEMBER_AUTHUSER			(MIC_MEMBERMODE_AUTHUSER)	// member was registered (authenticated).
#define CS_MEMBER_SPECTATOR			(MIC_MEMBERMODE_NONE)		// member can only watch
#define CS_MEMBER_IRC				(MIC_MEMBERMODE_IRC)		// member is an IRC dude
#define	CS_MEMBER_NODATA			(MIC_MEMBERMODE_NODATA)		// member does not accept data messages

#define CS_MEMBER_DEFAULT			(CS_MEMBER_SPEAKER)
//
// Channel Types. NOTE - channel types can be OR combined. They set BITS. 
//
#define CS_CHANNEL_PREDEFINED		(MIC_CHANNELMODE_DEFAULT)
#define CS_CHANNEL_PUBLIC			(MIC_CHANNELMODE_PUBLIC)	// open to all authenticated users by default.(to allow EVERYBODY - also use ALLOWANON flag.
#define CS_CHANNEL_PROTECTED		(MIC_CHANNELMODE_PRIVATE)	// can't do finds on member, but can query other properties
#define	CS_CHANNEL_PRIVATE			(MIC_CHANNELMODE_SECRET)	// can't be seen by FIND at all
#define	CS_CHANNEL_INVITELIST		(MIC_CHANNELMODE_INVITE)	// enter by invitation only
#define	CS_CHANNEL_MODERATED		(MIC_CHANNELMODE_MODERATED)	// new members do not have speaking rights by default. host must give it
#define	CS_CHANNEL_TOPICOP			(MIC_CHANNELMODE_TOPICOP)	// topic can be changed only by Host
#define CS_CHANNEL_NOREMOTE			(MIC_CHANNELMODE_NOREMOTE)	// only allow users who logged into THIS server..to enter chat
#define CS_CHANNEL_NOEXTERNAL		(MIC_CHANNELMODE_NOEXTERN)	// users must be in a channel to send messages to it
//
// These channel modes can only be used on MIC Servers. They are MIC features, although
// some of them are revealed to IRC too. 
//
// MIC and IRC (only if the IRC server is also a MIC server)
//
#define CS_CHANNEL_NOWHISPER		(MIC_CHANNELMODE_NOWHISPER)		// no whispers or \msg allowed
#define CS_CHANNEL_AUDITORIUM		(MIC_CHANNELMODE_AUDITORIUM)	// all msgs from non-hosts go to hosts first
//
// MIC ONLY 
//
#define CS_CHANNEL_REALNAME			(MIC_CHANNELMODE_NOALIAS)	// use real names only in this channel.. no NICKs
#define CS_CHANNEL_NOREALNAME		(MIC_CHANNELMODE_NOREALNAME)// do not permit RealName lookups

#define CS_CHANNEL_NODATA			(MIC_CHANNELMODE_NODATA)	// channel does not permit data msgs

// Default..
#define	CS_CHANNEL_DEFAULT			(CS_CHANNEL_PUBLIC)
//
// Channel FLAGS - You can specify some of these modifiers when you create the channel, but cannot
// change them later.. 
// The R/O ones - you can check with ICSChannel::DwFlags()
// MIC ONLY
//
#define CS_CHANNEL_FLAG_NONE		(MIC_CHANNELFLAG_NONE)
#define	CS_CHANNEL_FLAG_LOCAL		(MIC_CHANNELFLAG_LOCAL)		// channel not exposed to other servers on net
#define CS_CHANNEL_FLAG_MICONLY		(MIC_CHANNELFLAG_MICONLY)	// NO IRC - NO NO NO IRC
#define CS_CHANNEL_FLAG_UNICODE		(MIC_CHANNELFLAG_UNICODE)	// NO IRC - MUST BE MICONLY
#define CS_CHANNEL_FLAG_AUTHONLY	(MIC_CHANNELFLAG_AUTHJOIN)	// permit authenticated members only
#define CS_CHANNEL_FLAG_AUTHTALK	(MIC_CHANNELFLAG_AUTHTALK)	// permit only authenticated members to talk
#define	CS_CHANNEL_FLAG_TEMPLATE	(MIC_CHANNELFLAG_TEMPLATE)	// if all channels with this base name are full, auto create a new one
#define CS_CHANNEL_FLAG_ECHOSOURCE	(MIC_CHANNELFLAG_ECHOSOURCE) // server will echo a copy of everything you send back to you
#define CS_CHANNEL_FLAG_ROOM		(MIC_CHANNELFLAG_ROOMCHAT)	// R/O Multiuser dungeons, Elizas..
#define CS_CHANNEL_FLAG_FEED		(MIC_CHANNELFLAG_FEED)		// R/O news feeds, stock quotes..
#define	CS_CHANNEL_FLAG_PERSIST		(MIC_CHANNELFLAG_PERSISTENT) // R/O channel params defined by sys admin
#define CS_CHANNEL_FLAG_SERVICE		(MIC_CHANNELFLAG_SERVICE)	// R/O the channel is under the control of a channel service
//
// Channel Creation and Join flags
//
#define CS_CHANNEL_CREATE_JOIN		 (MIC_CHANNELOPEN_ALWAYS)		// server -if channel exists, join it,else create a new one
//
// Channel USER flags.
// Flags you specify will affect THE CALLER's status in this channel. eg. Setting CS_CHANNEL_USER_NOMEMLIST will
// indicate that the server should not send you the member list for the channel
// Ignored on IRC - on MIC servers, these help make games and auditoriums perform better
//
#define CS_CHANNEL_USER_NOMEMLIST	 (MIC_CHANNELOPEN_NOMEMLIST)	// server -don't send any member list to this user
//				   
// Inbound messages.
// Messages are sent to sockets or channels, depending on their nature
//
#ifdef __cplusplus

const CSMSG_TYPE	CSMSG_TYPE_NONE				= 0x00;
const CSMSG_TYPE	CSMSG_TYPE_ERROR			= 0x01;		// socket, channel	
const CSMSG_TYPE	CSMSG_TYPE_LOGIN			= 0x02;		// socket	
const CSMSG_TYPE	CSMSG_TYPE_TEXT_A			= 0x03;		// channel
const CSMSG_TYPE	CSMSG_TYPE_TEXT_W			= 0x04;		// channel
const CSMSG_TYPE	CSMSG_TYPE_DATA				= 0x05;		// channel
const CSMSG_TYPE	CSMSG_TYPE_ADDCHANNEL		= 0x06;		// socket
const CSMSG_TYPE	CSMSG_TYPE_ADDMEMBER		= 0x07;		// channel
const CSMSG_TYPE	CSMSG_TYPE_GOTMEMLIST		= 0x08;		// channel
const CSMSG_TYPE	CSMSG_TYPE_DELMEMBER		= 0x09;		// channel
const CSMSG_TYPE	CSMSG_TYPE_DELCHANNEL		= 0x0a;		// channel
const CSMSG_TYPE	CSMSG_TYPE_MODEMEMBER		= 0x0b;		// channel
const CSMSG_TYPE	CSMSG_TYPE_MODECHANNEL		= 0x0c;		// channel
const CSMSG_TYPE	CSMSG_TYPE_WHISPERTEXT_A	= 0x0d;		// channel
const CSMSG_TYPE	CSMSG_TYPE_WHISPERTEXT_W	= 0x0e;		// channel
const CSMSG_TYPE	CSMSG_TYPE_WHISPERDATA		= 0x0f;		// channel
const CSMSG_TYPE	CSMSG_TYPE_NEWTOPIC			= 0x10;		// channel
const CSMSG_TYPE	CSMSG_TYPE_PROPERTYDATA		= 0x11;		// socket,channel
const CSMSG_TYPE	CSMSG_TYPE_QUERYDATA		= 0x12;		// socket, channel
const CSMSG_TYPE	CSMSG_TYPE_PRIVATEMSG		= 0x13;		// socket
const CSMSG_TYPE	CSMSG_TYPE_NEWNICK			= 0x14;		// channel
const CSMSG_TYPE	CSMSG_TYPE_INVITE			= 0x15;		// socket
const CSMSG_TYPE	CSMSG_TYPE_SERVERMSG_TEXT_A	= 0x16;		// socket
const CSMSG_TYPE	CSMSG_TYPE_SERVERMSG_TEXT_W	= 0x17;		// socket

const CSMSG_TYPE	CSMSG_TYPE_LAST				= 0x17; // UPDATE ME WHEN MSGS ADDED!

#else

#define CSMSG_TYPE								0x00		// socket
#define CSMSG_TYPE_ERROR						0x01		// socket
#define CSMSG_TYPE_LOGIN						0x02		// channel
#define CSMSG_TYPE_TEXT_A						0x03		// channel
#define CSMSG_TYPE_TEXT_W						0x04		// channel
#define CSMSG_TYPE_DATA							0x05		// channel
#define CSMSG_TYPE_ADDCHANNEL					0x06		// socket
#define CSMSG_TYPE_ADDMEMBER					0x07		// channel
#define CSMSG_TYPE_GOTMEMLIST					0x08		// channel
#define CSMSG_TYPE_DELMEMBER					0x09		// channel
#define CSMSG_TYPE_DELCHANNEL					0x0a		// channel
#define CSMSG_TYPE_MODEMEMBER					0x0b		// channel
#define CSMSG_TYPE_MODECHANNEL					0x0c		// channel
#define CSMSG_TYPE_WHISPERTEXT_A				0x0d		// channel
#define CSMSG_TYPE_WHISPERTEXT_W				0x0e		// channel
#define CSMSG_TYPE_WHISPERDATA					0x0f		// channel
#define CSMSG_TYPE_NEWTOPIC						0x10		// channel
#define CSMSG_TYPE_PROPERTYDATA					0x11		// socket,channel
#define CSMSG_TYPE_QUERYDATA					0x12		// socket,channel
#define CSMSG_TYPE_PRIVATEMSG					0x13		// socket
#define CSMSG_TYPE_NEWNICK						0x14		// channel
#define CSMSG_TYPE_INVITE						0x15		// socket
#define CSMSG_TYPE_SERVERMSG_TEXT_A				0x16		// socket
#define CSMSG_TYPE_SERVERMSG_TEXT_W				0x17		// socket

#define CSMSG_TYPE_LAST							0x17

#endif	
//
// CSMSG_FLAGS. Modifiers on CSMSG_TYPE. This value is passed in CS_MSGBASE.bFlags
//

#ifdef __cplusplus

const CSMSG_FLAG	CSMSG_FLAG_NONE			= 0x00;
const CSMSG_FLAG	CSMSG_FLAG_WHISPER		= 0x01;

#else

#define CSMSG_FLAG_NONE			0x00
#define CSMSG_FLAG_WHISPER		0x01

#endif
//
// PROPERTIES and QUERIES. 
// NOTE: DUE TO THE NATURE OF THE IRC PROTOCOL AND SERVERS, generic "querying" is NOT supported
// under IRC. 
// Chatsock can therefore provide fewer methods that allow querying on IRC Sockets.
// Available methods are marked as such.
// 
// With MIC sockets, you can combine the following to retrieve multiple properties at a time.
//
// Use CSPROP_GENERIC_MODEMASK if you wish to query to see if specific bits in a matched
// object's modes are set. However, NEVER use it to as a return value that you want back.
// Use it only in queries you send up. In the first DWORD, specify the mode bits you are 
// interested in, and in the second, the mask..depending on whether you want to check for
// bits being set or not
// REPEAT - MIC ONLY
// 
// These can be used to query both for channels and members/users
#define CSPROP_GENERIC_MODE				(MIC_PROP_MODES)	// DWORD		
#define CSPROP_GENERIC_MODE2			(MIC_PROP_MODES2)	// Obsolete. User CSPROP_GENERIC_MODEMASK			
#define CSPROP_GENERIC_MODEMASK			(MIC_PROP_MODES2) 	// 2 DWORDS
// Every MIC channel,member and user is uniquely identified by its ID.
// If you specify an object's ID in a query, you can get the server to return
// properties for exactly that object
#define CSPROP_GENERIC_ID_MIC			(MIC_PROP_ID)					// DWORD
//
// These can be used to query only for members or users
//
// fix me: #define CSPROP_MEMBER_NAME				(MIC_PROP_MEMBER_ALIAS)			// string
#define CSPROP_MEMBER_NAME				(MIC_PROP_USER_ALIAS)			// string
#define CSPROP_MEMBER_REALNAME			(MIC_PROP_USER_IDENT)			// string
#define CSPROP_MEMBER_LOGINNAME			(MIC_PROP_USER_NAME)
#define CSPROP_MEMBER_IPADDRESS			(MIC_PROP_USER_IPADDR)			// dword
#define CSPROP_MEMBER_AWAY				(MIC_PROP_USER_AWAY)			// string

#define CSPROP_USER_NAME				(MIC_PROP_USER_ALIAS)			// string
#define CSPROP_USER_REALNAME			(MIC_PROP_USER_NAME)			// string
#define CSPROP_USER_IDENT				(MIC_PROP_USER_IDENT)			// string
#define CSPROP_USER_IPADDRESS			(MIC_PROP_USER_IPADDR)			// dword
#define CSPROP_USER_AWAY				(MIC_PROP_USER_AWAY)			// string
//
// These can be used to query only for channels
// 
#define CSPROP_CHANNEL_NAME				(MIC_PROP_CHANNEL_NAME)			// string
#define CSPROP_CHANNEL_PASSWORD			(MIC_PROP_CHANNEL_KEYWORD)		// string
#define CSPROP_CHANNEL_TOPIC			(MIC_PROP_CHANNEL_TOPIC)		// string
#define CSPROP_CHANNEL_USERMAX			(MIC_PROP_CHANNEL_LIMIT)		// dword
#define CSPROP_CHANNEL_CUSER			(MIC_PROP_CHANNEL_MEMBERS)		// dword
#define CSPROP_CHANNEL_SUBJECT_MIC		(MIC_PROP_CHANNEL_SUBJECT)		// string 
#define CSPROP_CHANNEL_CHANNELGUID_MIC	(MIC_PROP_CHANNEL_CLIENTGUID)	// string 
#define CSPROP_CHANNEL_SERVICEGUID_MIC	(MIC_PROP_CHANNEL_SERVICEGUID)	// string 
#define CSPROP_CHANNEL_FLAGS			(MIC_PROP_CHANNEL_FLAGS)		// DWORD
#define CSPROP_CHANNEL_FLAGSMASK		(MIC_PROP_CHANNEL_FLAGS2)		// 2 DWORDS
#define CSPROP_CHANNEL_RATING_PICS		(MIC_PROP_CHANNEL_PICS)			// string
#define CSPROP_CHANNEL_BANNEDLIST		(MIC_PROP_CHANNEL_BANNED)		// string
#define CSPROP_CHANNEL_DATA				(MIC_PROP_CHANNEL_DATA)			//
//
// Use to build queries that combine above property types
// You may also use these when calling the HrList* functions
//
#define CSPROP_QUERY_CONTAINS			(MIC_TERM_CONTAINS)
#define CSPROP_QUERY_EQUAL				(MIC_TERM_EQUAL)
#define CSPROP_QUERY_NOTEQUAL			(MIC_TERM_NOTEQUAL)
#define CSPROP_QUERY_GREATER			(MIC_TERM_GREATER)
#define CSPROP_QUERY_GREATEREQUAL		(MIC_TERM_GREATEREQUAL)
#define CSPROP_QUERY_LESS				(MIC_TERM_LESS)
#define CSPROP_QUERY_LESSEQUAL			(MIC_TERM_LESSEQUAL)
#define CSPROP_QUERY_AND_MIC			(MIC_TERM_AND)
#define CSPROP_QUERY_OR_MIC				(MIC_TERM_OR)
#define CSPROP_QUERY_WILDCARD_MIC		(MIC_TERM_MASK)	  // MIC ONLY
//
// When you call Query wrappers such as ICSChatSocket::HrListAllChannels and HrListChannelsMask
// Chatsock sends a CSMSG_TYPE_QUERYDATA to you, with a predefined set of properties.
// You call  ICSProperty::HrGetProperty to retrieve the properties. 
// The following INDICES describe the ORDER of the properties in each msg.
// 
// HrListAllChannels		All		(returns all 6 below)
// HrListChannelsMaskA		1-4
// HrQueryChannelProperties	All
//

#ifdef __cplusplus

const int	CSINDEX_PROP_CHANNEL_NAME		= 1;	// always the first property 
const int	CSINDEX_PROP_CHANNEL_CUSER		= 2;	// user count. Always the 2nd property
const int	CSINDEX_PROP_TOPIC				= 3;	// always the 3rd property
const int	CSINDEX_PROP_CHANNEL_MODE		= 4;	// and 4'th
const int	CSINDEX_PROP_ID					= 5;	// Available only on MIC Sockets
const int	CSINDEX_PROP_CHANNEL_MAXUSER	= 6;	// Available only on MIC Sockets
//
// And these are the Indexes of the properties returned when you call:
// 1) HrListAllMembers 2)HrListAllUsers
const int	CSINDEX_PROP_MEMBER_NAME	= 1;
const int	CSINDEX_PROP_MEMBER_MODE	= 2;

#else

#define CSINDEX_PROP_CHANNEL_NAME		1	// always the first property 
#define CSINDEX_PROP_CHANNEL_CUSER		2	// user count. Always the 2nd property
#define CSINDEX_PROP_TOPIC				3	// always the 3rd property
#define CSINDEX_PROP_CHANNEL_MODE		4	// and 4'th
#define CSINDEX_PROP_ID					5	// Available only on MIC Sockets
#define CSINDEX_PROP_CHANNEL_MAXUSER	6	// Available only on MIC Sockets
//
// And these are the Indices of the properties returned when you call:
// 1) HrListAllMembers 2)HrListAllUsers
#define CSINDEX_PROP_MEMBER_NAME		1
#define CSINDEX_PROP_MEMBER_MODE		2
//
// Indices of properties returned when you call:
// ICSocket::HrGetRealName or ICMember::HrGetRealName
//
#define CSINDEX_PROP_REALNAME_REALNAME	1
#define CSINDEX_PROP_REALNAME_NAME		2

#endif

//--------------------------------------------------------------------------------------------
//
// GUIDS.
//
//--------------------------------------------------------------------------------------------
//
// CLSID for the ChatSock dll
//
DEFINE_GUID(CLSID_CHATSOCK,0xFD6E3680,0x75F1,0x11CF,0x82,0xDD,0x00,0xAA,0x00,0xB8,0x97,0x4E);
//
// Use this to obtain a Socket Factory
//
DEFINE_GUID(IID_ICSFACTORY,0xEDE45140,0xBC81,0x11CF,0xB5,0x12,0x00,0xA0,0x24,0xA6,0x52,0xFA);
//
// Use this to obtain a MIC socket interface
//
DEFINE_GUID(IID_IMIC,0x83605F80,0x75F3,0x11CF,0x82,0xDD,0x00,0xAA,0x00,0xB8,0x97,0x4E);
//
// Use this to obtain an IRC socket interface
//
DEFINE_GUID(IID_IIRC,0x8E9619A0,0x75F3,0x11CF,0x82,0xDD,0x00,0xAA,0x00,0xB8,0x97,0x4E);
//
// Use this to indicate Version numbers where required
//
DEFINE_GUID(IID_CHATSOCKVER1,0xFA395610,0x7C9D,0x11CF,0x82,0xE9,0x00,0xAA,0x00,0xB8,0x97,0x4E);

//--------------------------------------------------------------------------------------------
//
// FUNCTIONS.
//
//--------------------------------------------------------------------------------------------

#ifdef __cplusplus
extern "C" {
#endif
//
// If you KNOW what protocol you wish to use.. or if you know that the server you are connecting
// to speaks MIC/IRC.. then you call this function to obtain a chat socket interace.
// Use riid to specify IID_IMIC or IID_IIRC.
//
CHATSOCKAPI HrCreateChatSocket(REFIID riidVersion, REFIID riid, PICS *ppics);
//
// If you do NOT know what protocol you should use against a server.. i.e...all you have is
// a server name that you pulled from a Web Page or something.. then call this function to
// obtain a chat socket factory interface. You can then use the factory interface to obtain
// a socket.The factory will figure out what protocol should
// be used against this server and return you an appropriate socket pointer.
//
CHATSOCKAPI HrCreateChatSocketFactory(REFIID riidVersion, PICS_FACTORY *ppicsFactory);
//
// Always call this to Free any messages you receive from ChatSock
// 
CHATSOCKAPI HrFreeMsg(PCS_MSGBASE pMsg); 

//
// These are useful routines that help you verify your parameters up front.. instead of
// waiting to call methods and then getting E_INVALIDARG back. Use these to check stuff
// users entered in Dialog Box edit boxes etc..
// These routines will return detailed errors - Check cserror.h for details.
//
CHATSOCKAPI HrVerifyNickA(CHAR *szNick);
CHATSOCKAPI HrVerifyNickW(WCHAR *wszNick);
CHATSOCKAPI HrVerifyCreateChannelNameA(CHAR *szChannelName, BOOL fMicOnly, BOOL fLocal);
CHATSOCKAPI HrVerifyCreateChannelNameW(WCHAR *wszChannelName, BOOL fMicOnly, BOOL fLocal);

#ifdef __cplusplus
}
#endif
//
// OLE EXPORTS
//
STDAPI DllGetClassObject(REFCLSID rclsid,REFIID riid,LPVOID *ppv);
STDAPI DllCanUnloadNow(void);
STDAPI DllRegisterServer(void) ;
STDAPI DllUnregisterServer(void);

//--------------------------------------------------------------------------------------------
//
// INTERFACES: Definitions.
//
//--------------------------------------------------------------------------------------------
//
// When you ask the ChatDLL to connect to a server, it is often possible you do NOT know if
// the server is IRC or MIC. This is especially the case if you pick the server name etc off
// an HTML tag/Web page.. all the tag will says is that X(could be a URL)is a link to a Chat on a
// said server. It is then that you must use the socket factory to connect. The ChatSock DLL
// will automatically figure out what kind of server this is and then return an
// appropriate IChatSocket interface. Call IChatSocketFactory::HrMakeSocket
//
 // DO NOT use this factory if you already know what protocol you want to use. Call
// HrCreateChatSocket directly.. that will be FASTER
//

#undef INTERFACE
#define INTERFACE IChatSocketFactory

DECLARE_INTERFACE_(IChatSocketFactory, IClassFactory)
{
	// IUnknown:
	STDMETHOD(QueryInterface)		(THIS_ REFIID riid, LPVOID *ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)		(THIS) PURE;
	STDMETHOD_(ULONG, Release)		(THIS) PURE;
	// IClassFactory
	//
	// If you already know whether you want an IRC or MIC socket..then use the standard 
	// Factory CreateInstance
	STDMETHOD(CreateInstance)		(THIS_ IUnknown *pUnkOuter,REFIID riid,LPVOID *ppvObj) PURE;
	STDMETHOD(LockServer)			(THIS_ BOOL fLock) PURE;
	// IChatSocketFactory:
	// If you don't know what protocol to speak, use HrMakeSocket.. and it will figure
	// out what to speak.. and return an appropriate ppics
	//
	STDMETHOD(HrMakeSocket)			(THIS_ CHAR *szServer, PICS *ppics) PURE;
	STDMETHOD(HrCancelMakeSocket)	(THIS) PURE;
};

//
// CHATSOCKET logic has a simple hierachy.
//
// IChatSocket
//		|
//		'-----ICSChannel (multiple..)
//				|
//				'-------ICSMember
//							|
//							'----------Your Data

// First you create a Socket.
// The DLL returns an IChatSocket interface to you.
// You can then use this interface to Connect to your server, login to your sever(not the same
// as connect), to create/join channels. 
// Just like WinSock, ONE ChatSock connects to ONE chat server. 
// However, under a ChatSock, you can create multiple Channels.
// But if you wanted to create channels on TWO servers, you would need to create 2 chat sockets.
//
// Each ChatSocket sends you, the user, several notification and other messages. To grab these
// .. you should call IChatSocket::HrWaitForMsg(). This call will put your calling thread
// to SLEEP until a notification becomes available OR if the socket is ready to shutdown.
// Among the sorts of messages you get are:CSMSG_TYPE_LOGIN (successful login),CSMSG_TYPE_ADDCHANNEL
// ( a new channel came on line).. and CSMSG_TYPE_ERROR.
//
// NOTE: MIC and IRC are inherently asynchronous in how they return success/error codes etc
// to a caller. The simple safe "I call a function, it blocks till it succeeds" methodology
// cannot work. That is why the DLL needs message qs.
//
// Once you have a socket, you can call say IChatSocket::HrCreateChannel.. and pass it a 
// struct populated with various permitted params for channel creation. ChatSock will verify
// all parameters (such as max string lengths) before it does anything. It will then dispatch
// an appropriate msg to the server and RETURN. At some point, the server will send ChatSock
// an answer (success/failure) -and other details. ChatSock will parse this info and stick it
// on the Socket's Msg Q - using CS_MSG_TYPE_ADDCHANNEL. 
// NOTE: if you successively create channels (a) (b) and (c), you will get notifications from
// ChatSock in EXACTLY that order.. (a) first, (b) next and so on..
//
// The CS_MSG_TYPE_ADDCHANNEL msg will contain a pointer to ICSChannel 
// NOTE: ALL you need to store is this interface. ChatSocket will store the obj internally..
// etc etc.	 PICS_CHANNEL is ALL you need. 
// NOTE: Always call HrFreeMsg() after you are done using a msg.
//
// Now, each channel has a Msg Q associated with it. These are message intended for THIS
// channel only. Use ICSChannel::HrWaitForMsg to get messages. Always call HrFreeMsg() after 
// you are done using a message.
// The server will send ChatSock a list of members in the channel. The DLL will INTERNALLY
// store and manage these lists. Each time it gets a new member on the channel, it will place
// an CSMSG_TYPE_ADDMEMBER msg on the msg q. This msg will also contain a pointer to a
// ICSMember interface. THIS INTERFACE is all you need. The ChatSocket DLL maintains internal
// tables that manage the actual member object. 
//
// The concept of the interface is critical here. So for example, say somebody sends text to
// a particular member X (note: on IRC, like MIC, you can send private msgs directly to people,
// independant of their channel). ChatSock will quickly figure out who the user is and what
// channel the user is on. It will then place a CSMSG_TYPE_TEXT_A (if the msg was ANSI) on
// that channel's msg q. Included IS A POINTER TO ICSMEMBER -- the Member this message was
// FROM! ALL you have to do to DISPLAY the actual text is: call ICSMember::HrGetName()
// to get the user's name etc etc.. and then putting the text on screen. They key is, ChatSock
// ALWAYS figures out the whos and whats for you. You don't need to do any work.
//
// Similarly, if you wished to send ANSI text to the entire channel, you give your text to 
// ICSChannel::HrSendTextA.. (using the ICSChannel interface that ChatSock sent you when it
// created this channel). 
// But if you wanted to send ANSI text ONLY to a user "Jello", you give your text to
// ICSMember::HrSendTextA.. (using the ICSMember interface that ChatSock had sent you when it
// added this member).
// 
// ChatSock will send notification msgs when members leave, or channels close. It will include
// an appropriate interface pointer.. which you can then use to do comparisons as you delete
// stuff from your own data structures.
//
// So to write a simple chat client, all you need is a rich edit control and a list box control.
// You can stick the ICSMember pointers	straight into the list box's itemData. You do not need
// to maintain a user list, or for that matter, any major data structures. You don't need to
// know anything about WinSock, or MIC or IRC. If you do need to store additional information
// for a member or a channel, use the handy ICSChannel::HrSetChannelData or ICSMember::HrSetMemberData
// functions to store a pointer to your data DIRECTLY in chatsock's member tables 
//

#undef INTERFACE
#define INTERFACE IChatSocket
DECLARE_INTERFACE_(IChatSocket, IUnknown)
{
	// IUnknown:
	STDMETHOD(QueryInterface)		(THIS_ REFIID riid, LPVOID *ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)		(THIS) PURE;
	STDMETHOD_(ULONG, Release)		(THIS) PURE;

	// IChatSocket:	
	STDMETHOD(HrConnect)			(THIS_ CHAR *szServer) PURE;
	STDMETHOD(HrCloseSocket)		(THIS) PURE;
	STDMETHOD(HrIsConnected)		(THIS) PURE;
	STDMETHOD(HrIsMicSocket)		(THIS) PURE;
	STDMETHOD(HrGetServerName)		(THIS_ CHAR **pszName) PURE; // what server are we connected to?
	
	STDMETHOD(HrGetSecurityInfo)	(THIS_ PCS_SECURITY *ppcsSecurity) PURE; // MIC ONLY

	STDMETHOD(HrLoginA)				(THIS_ PCS_CONNINFO pcsConnInfo) PURE;
	STDMETHOD(HrLoginW)				(THIS_ PCS_CONNINFO pcsConnInfo) PURE;	// MIC ONLY
	STDMETHOD(HrLogOff)				(THIS) PURE;
	STDMETHOD(HrIsLoggedOn)			(THIS) PURE;
	// IRC - works like /join
	STDMETHOD(HrCreateChannelA)		(THIS_ PCS_CINFO pcInfo) PURE;
 	STDMETHOD(HrCreateChannelW)		(THIS_ PCS_CINFO pcInfo) PURE;			// MIC ONLY
	// IRC - works like /join
	STDMETHOD(HrJoinChannelA)		(THIS_ PCS_JOININFO pjInfo) PURE;
	STDMETHOD(HrJoinChannelW)		(THIS_ PCS_JOININFO pjInfo) PURE;		// MIC ONLY
	// IRC -works like /privmsg
	STDMETHOD(HrSendPrivMsgA)		(THIS_ PCS_PRIVMSGINFO pmInfo) PURE;
	STDMETHOD(HrSendPrivMsgW)		(THIS_ PCS_PRIVMSGINFO pmInfo) PURE;
	// IRC -works like /INVITE
	STDMETHOD(HrSendInviteA)		(THIS_ PCS_INVITEINFO piInfo) PURE;
	STDMETHOD(HrSendInviteW)		(THIS_ PCS_INVITEINFO piInfo) PURE;
	// IRC - works like /away
	STDMETHOD(HrSetAwayA)			(THIS_ BOOL fAway, CHAR *szAwayMsg) PURE;	
	STDMETHOD(HrSetAwayW)			(THIS_ BOOL fAway, WCHAR *wszAwayMsg) PURE;	// MIC ONLY

	STDMETHOD(HrChangeNickA)		(CHAR *szNick) PURE;
	STDMETHOD(HrChangeNickW)		(WCHAR *wszNick) PURE;
	//
	// Advanced QUERY METHODS THAT ONLY WORK FOR MIC
	// These are available ONLY ON MIC SOCKETS
	//
	// Use to query for info on channel properties
	STDMETHOD(HrQueryForChannelsA)	(THIS_ PCS_QUERY_MIC pcsQuery) PURE;	 // MIC ONLY
	STDMETHOD(HrQueryForChannelsW)	(THIS_ PCS_QUERY_MIC pcsQuery) PURE;	 // MIC ONLY
	// Use to query for info on the members of a channel with ID dwIDChannel
	STDMETHOD(HrQueryForMembersA)	(THIS_ DWORD dwIDChannel,PCS_QUERY_MIC pcsQuery) PURE;	// MIC
	STDMETHOD(HrQueryForMembersW)	(THIS_ DWORD dwIDChannel,PCS_QUERY_MIC pcsQuery) PURE;	// MIC
	// Use to query on Users - i.e. anybody OUTSIDE the scope of a channel 
	STDMETHOD(HrQueryForUsersA)		(THIS_ PCS_QUERY_MIC pcsQuery) PURE;	 // MIC ONLY
	STDMETHOD(HrQueryForUsersW)		(THIS_ PCS_QUERY_MIC pcsQuery) PURE;	 // MIC ONLY
	//
	// The remainder of these methods, with the exception of those marked as such,
	// are available on both MIC and IRC sockets.
	// On MIC Sockets, these wrap the HrQuery methods above, allowing the MIC server to
	// do a lot of the hard work. For IRC servers, whose querying capabilities are limited,
	// Chatsock does the work of matching data to the search criteria you specified.
	// This method is a wrapper on HrQueryForChannels
	// Returns the channel name, topic, type and user count of a channels. For MIC sockets,
	// also returns the id and max user count allowed.
	STDMETHOD(HrListAllChannelsA)	(THIS_									
									 DWORD dwcUserMin,
									 DWORD dwcUserMax,
									 CHAR *szName,
									 CSPROP_OP csopName) PURE; 

	STDMETHOD(HrListAllChannelsW)	(THIS_
									 DWORD dwcUserMin,
									 DWORD dwcUserMax,
									 WCHAR *wszName,
									 CSPROP_OP csopName) PURE;	// MIC only
	// Does HrListAllChannels AND searches on additional paramters, such as the channel type
	STDMETHOD(HrListAllChannelsExA)	(THIS_ PCS_LISTCHANINFO pclInfo) PURE;
	STDMETHOD(HrListAllChannelsExW)	(THIS_ PCS_LISTCHANINFO pclInfo) PURE; // MIC only
	// returns the name and mode of all members on the channel whose name that matches szChannel
	STDMETHOD(HrListAllMembersFromNameA)	(THIS_ CHAR *szChannel) PURE;	
	STDMETHOD(HrListAllMembersFromNameW)	(THIS_ WCHAR *wszChannel) PURE; // MIC ONLY
	// Returns the name and mode of all members in the channel with ID dwIDChannel. On MIC
	// sockets, this is much faster than calling HrListAllMembersFromName
	STDMETHOD(HrListAllMembersA)	(THIS_ DWORD dwIDChannel) PURE;	//	MIC ONLY MIC ONLY
	STDMETHOD(HrListAllMembersW)	(THIS_ DWORD dwIDChannel) PURE;	//	MIC ONLY MIC ONLY
	// returns the name and mode of all users who match szName and the specified operator
	STDMETHOD(HrListAllUsersA)		(THIS_ CHAR *szName, CSPROP_OP csopName) PURE;	  
	STDMETHOD(HrListAllUsersW)		(THIS_ WCHAR *wszName, CSPROP_OP csopName) PURE;  // MIC ONLY
	// Returns the name of all users on the system who match the mask szUser
	STDMETHOD(HrListUsersMaskA)		(THIS_ CHAR *szUser) PURE;
	STDMETHOD(HrListUsersMaskW)		(THIS_ WCHAR *wszUser) PURE;
	// Returns the name,type,topic and user count of the channel that matches szChannel
	// If szChannel is NULL, returns the above for all channels
	STDMETHOD(HrQueryChannelPropsFromNameA)	(THIS_ CHAR *szChannel) PURE;	
	STDMETHOD(HrQueryChannelPropsFromNameW)	(THIS_ WCHAR *wszChannel) PURE;	// MIC ONLY
	// Returns the channel name, topic, mode, user count, id and max user count allowed
	// for the channel with the ID dwIDChannel. For MIC Sockets, this works faster than
	// calling HrQueryChannelPropsFromName
	STDMETHOD(HrQueryChannelPropertiesA)(THIS_ DWORD dwIDChannel) PURE; // MIC ONLY MIC ONLY
	STDMETHOD(HrQueryChannelPropertiesW)(THIS_ DWORD dwIDChannel) PURE; // MIC ONLY MIC ONLY	
	// Returns a user's Real Name
	STDMETHOD(HrGetRealNameA)		(THIS_ CHAR *szNickName) PURE;
	STDMETHOD(HrGetRealNameW)		(THIS_ WCHAR *wszNickName) PURE;	// MIC ONLY
	//
	// Sysops and higher can use this to eject users or channels from the chat network
	//
	STDMETHOD(HrKillUserA)			(THIS_ PCS_KILLINFO pkInfo) PURE;
	STDMETHOD(HrKillUserW)			(THIS_ PCS_KILLINFO pkInfo) PURE;		// MIC ONLY
	//
	// Store socket specific data here
	//
	STDMETHOD(HrSetSocketData)		(THIS_ PVOID pvData) PURE;	// you can store a pointer to YOUR data directly												// in the channel
	STDMETHOD(HrGetSocketData)		(THIS_ PVOID *ppvData) PURE;

	//
	// The Socket message que. Wait for msgs from the socket
	//
	STDMETHOD(HrWaitForMsg)			(THIS_ PCS_MSGBASE *ppcsMsg, DWORD dwTimeOut) PURE;
	STDMETHOD(HrWaitTillMsgType)	(THIS_ CSMSG_TYPE csmsgType,PCS_MSGBASE *ppcsMsg, DWORD dwTimeOut) PURE;
	// MIC only:
	STDMETHOD(HrKillChannelA)		(THIS_ DWORD dwIDChannel,CHAR *szReason) PURE;
	STDMETHOD(HrKillChannelW)		(THIS_ DWORD dwIDChannel,WCHAR *wszReason) PURE;		
};

//
// A Chat Channel
// Use this interface to Manipulate Chat Channel
//
#undef INTERFACE
#define INTERFACE ICSChannel
DECLARE_INTERFACE_(ICSChannel, IUnknown)
{
	// IUnknown:
	STDMETHOD(QueryInterface)		(THIS_ REFIID riid, LPVOID *ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)		(THIS) PURE;
	STDMETHOD_(ULONG, Release)		(THIS) PURE;

	// ICSChannel:
	//
	// Informational
	//
	STDMETHOD(HrGetName)			(THIS_ BYTE **ppbName, BOOL *pfAnsi) PURE;	// the channel name
	STDMETHOD(HrGetTopic)			(THIS_ BYTE **ppbTopic, BOOL *pfAnsi) PURE;	// the channel topic
	STDMETHOD(HrGetServerName)		(THIS_ CHAR **pszName) PURE;	// the channel's parent server
	STDMETHOD(HrGetMe)				(THIS_ PICS_MEMBER *ppicsMember) PURE;	// get a pointer to YOU, the client that is running
	STDMETHOD(HrGetMemberFromID)	(THIS_ DWORD dwID,PICS_MEMBER *ppicsMember) PURE; // MIC only - retrieve a member from
																					  // the chatsock internal Table, given
																					  // a member's id
	STDMETHOD(HrGetMemberFromNameA)	(THIS_ CHAR *szName,PICS_MEMBER *ppicsMember) PURE; 
	STDMETHOD(HrGetMemberFromNameW)	(THIS_ WCHAR *wszName,PICS_MEMBER *ppicsMember) PURE; // MIC only

	STDMETHOD(HrGetChannelSocket)	(THIS_ PICS *ppics) PURE; //get a pointer to the socket this channel is on
	//
	// Modes and Types
	//
	STDMETHOD(HrGetUserCount)		(THIS_ DWORD *pdwc) PURE;		// how many users in the channel
	STDMETHOD(HrGetType)			(THIS_ DWORD *pdwType) PURE;
	STDMETHOD(HrGetFlags)			(THIS_ DWORD *pdwFlags) PURE;
	// 
	// dwMask contains the channel type bits you want to modify
	// dwType contains the values (0 or 1) that each of these bits should be set to
	//	
	STDMETHOD(HrModifyType)			(THIS_ DWORD dwType,DWORD dwMask) PURE;
	// Wrappers on HrModifyType
	STDMETHOD(HrMakePublic)			(THIS) PURE;
	STDMETHOD(HrMakeProtected)		(THIS) PURE;
	STDMETHOD(HrMakePrivate)		(THIS) PURE;
	//
	// set the upper limit for users in this channel	
	//
	STDMETHOD(HrSetUserMax)			(THIS_ DWORD dwcUserMax) PURE;
	STDMETHOD(HrGetUserMax)			(THIS_ DWORD *pdwcUserMax) PURE;
	//
	// Ratings and Language - work only for channels on MIC servers
	//
	STDMETHOD(HrGetRating)			(THIS_ BYTE **ppbRating,BOOL *pfAnsi) PURE;		

	STDMETHOD(HrSetChannelData)		(THIS_ PVOID pvData) PURE;	// you can store a pointer to YOUR data directly
												// in the channel
	STDMETHOD(HrGetChannelData)		(THIS_ PVOID *ppvData) PURE;

	STDMETHOD(HrIsMicOnlyChannel)	(THIS) PURE; // MIC only or IRC style channel?
	STDMETHOD(HrIsUnicodeChannel)	(THIS) PURE; // Is UNICODE channel?
	//											    
	// Sending data/text to the entire channel
	//
	STDMETHOD(HrSendData)			(THIS_ PVOID pvData, DWORD dwcbData) PURE;
	STDMETHOD(HrSendDataList)		(THIS_ PVOID pvData, DWORD dwcbData, PICS_MEMBER *prgpicsMem, DWORD dwcMem) PURE;
	//
	// Send text to the entire channel
	//
	STDMETHOD(HrSendTextA)			(THIS_ CHAR *szText) PURE;	 
	STDMETHOD(HrSendTextW)			(THIS_ WCHAR *wszText) PURE;
	//
	// Send text to a list of members. 
	// To send to ONE member only..use the ICSMember interface
	//
	STDMETHOD(HrSendTextListA)		(THIS_ CHAR *szText, PICS_MEMBER *prgpicsMem, DWORD dwcMem) PURE;
	STDMETHOD(HrSendTextListW)		(THIS_ WCHAR *wszText, PICS_MEMBER *prgpicsMem, DWORD dwcMem) PURE;
	//
	// Invites
	//
	STDMETHOD(HrSendInviteA)		(THIS_ PCS_INVITEINFO piInfo) PURE;
	STDMETHOD(HrSendInviteW)		(THIS_ PCS_INVITEINFO piInfo) PURE;
	//
	// Bans
	//
	STDMETHOD(HrBanMemberMaskA)		(THIS_ CHAR *szMember,BOOL fSet) PURE;
	STDMETHOD(HrBanMemberMaskW)		(THIS_ WCHAR *wszMember,BOOL fSet) PURE;
	//
	// Change the Topic
	//
	STDMETHOD(HrSetTopicA)			(THIS_ CHAR *szTopic) PURE;
	STDMETHOD(HrSetTopicW)			(THIS_ WCHAR *wszTopic) PURE;
	// Whispers..	
	STDMETHOD(HrSetNoWhisper)		(THIS_ BOOL fOn) PURE;
	//
	// Broadcast support
	//
	STDMETHOD(HrInitBroadcast)		(THIS_ CSBROADCASTPROC pfnBroadcastCallBack) PURE;
	STDMETHOD(HrSendBroadcast)		(THIS_ BYTE *pbData,DWORD dwcb) PURE;
	// Retrieve properties in a BLOCK from the server. MIC ONLY
	// You specify the properties you want back.
	//	
	STDMETHOD(HrGetPropertiesA)		(THIS_ CSPROP_TYPE *prgcsPropType, DWORD dwcProp) PURE;
	STDMETHOD(HrGetPropertiesW)		(THIS_ CSPROP_TYPE *prgcsPropType, DWORD dwcProp) PURE;	
	//
	// Channel status
	//
	STDMETHOD(HrLeave)				(THIS_ BOOL fForce) PURE;		// YOU/Me leave the channel.. 
	//
	// Channel message que
	//
	STDMETHOD(HrWaitForMsg)			(THIS_ PCS_MSGBASE *ppcsMsg, DWORD dwTimeOut) PURE;
	STDMETHOD(HrWaitTillMsgType)	(THIS_ CSMSG_TYPE csmsgType,PCS_MSGBASE *ppcsMsg, DWORD dwTimeOut) PURE;
};

//
// A Chat Member
//
#undef INTERFACE
#define INTERFACE ICSMember
DECLARE_INTERFACE_(ICSMember, IUnknown)
{
	// IUnknown:
	STDMETHOD(QueryInterface)		(THIS_ REFIID riid, LPVOID *ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)		(THIS) PURE;
	STDMETHOD_(ULONG, Release)		(THIS) PURE;

	// ICSMember:
	//
	// Informational
	//
	STDMETHOD(HrGetName)			(THIS_ BYTE **ppbName, BOOL *pfAnsi) PURE;	// get a user's nickname
	STDMETHOD(HrGetMemberChannel)	(THIS_ PICS_CHANNEL *ppicsChannel) PURE; //get a pointer to the channel this member is on
	//
	// You can call GetMember mode and parse the mode bits yourself, OR you can
	// call the wrapper functions I have defined: eg. HrIsMemberHost
	//
	STDMETHOD(HrGetMemberMode)		(THIS_ DWORD *pdwMode) PURE;
	STDMETHOD(HrGetNameAndMode)		(THIS_ BYTE **ppbName,BOOL *pfAnsi,DWORD *pdwMode) PURE;

	STDMETHOD(HrIsMemberIgnored)	(THIS) PURE;
	STDMETHOD(HrIsMemberHost)		(THIS) PURE;
	STDMETHOD(HrIsMemberSpeaker)	(THIS) PURE;
	STDMETHOD(HrIsMemberSpectator)	(THIS) PURE;
	STDMETHOD(HrIsMemberSysOp)		(THIS) PURE;
	STDMETHOD(HrIsMemberAnonymous)	(THIS) PURE;
	STDMETHOD(HrIsMemberMICUser)	(THIS) PURE;			
	STDMETHOD(HrDoesMemberTakeWhisper)(THIS) PURE;
	//
	// Sending Data/Text to THIS member
	//
	STDMETHOD(HrSendData)			(THIS_ PVOID pvData, DWORD dwcbData) PURE;
	STDMETHOD(HrSendTextA)			(THIS_ CHAR *szText) PURE;
	STDMETHOD(HrSendTextW)			(THIS_ WCHAR *wszText) PURE;
	//
	// These are properties that ANY member can set. 
	//
	STDMETHOD(HrSetIgnoreMember)	(THIS_ BOOL fIgnore) PURE;
	//
	// These only a host can set. If caller is not a host, these will return an error
	//
	STDMETHOD(HrMakeSpectator)		(THIS) PURE;
	STDMETHOD(HrMakeSpeaker)		(THIS) PURE;
	STDMETHOD(HrMakeHost)			(THIS) PURE;	// beware. If you make someone a host, they can 
													// promptly make you a spectator! This is required 
													// however, for IRC compatibility
	//
	// Close this user - a host may use this to kick someone
	// You may provide a reason. If no reason, pass in NULL
	//
	STDMETHOD(HrCloseA)				(THIS_ CHAR *szReason) PURE; 
	STDMETHOD(HrCloseW)				(THIS_ WCHAR *wszReason) PURE; // MIC only

	STDMETHOD(HrSetNoWhisper)		(THIS_ BOOL fOn) PURE;
		
	STDMETHOD(HrSetMemberData)		(THIS_ PVOID pvData) PURE;	// you can store a pointer to YOUR data directly
																// in the member
	STDMETHOD(HrGetMemberData)		(THIS_ PVOID *ppvData) PURE;	
	//
	// The following methods return results asynchronously.. i.e. on the channel's msg q
	// The results are passed in the msg CSMSG_TYPE_PROPERTYDATA

	STDMETHOD(HrGetRealNameA)		(THIS) PURE;
	STDMETHOD(HrGetRealNameW)		(THIS) PURE;
	//
	// pass in an array of props to retrieve
	//
	STDMETHOD(HrGetPropertiesA)		(THIS_ CSPROP_TYPE *prgcsPropType, DWORD dwcProp) PURE;
	STDMETHOD(HrGetPropertiesW)		(THIS_ CSPROP_TYPE *prgcsPropType, DWORD dwcProp) PURE;	
};

//
// Results to Queries and Property Retrievals are always returned through this interface
// 
#undef INTERFACE
#define INTERFACE ICSProperty
DECLARE_INTERFACE_(ICSProperty, IUnknown)
{
	// IUnknown:
	STDMETHOD(QueryInterface)		(THIS_ REFIID riid, LPVOID *ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)		(THIS) PURE;
	STDMETHOD_(ULONG, Release)		(THIS) PURE;

	// ICSProperty:
	//
	// Property manipulation
	//
	STDMETHOD(HrGetCount)			(THIS_ DWORD *pdwcProp) PURE;
	//
	// dwIndex is ONE BASED. Index starts at ONE
	//
	STDMETHOD(HrGetProperty)		(THIS_ PCS_PROPDATA pPropData, DWORD dwIndex) PURE;
	//
	// Since clients will want to cache/save this pointer.. for finds and other purposes,
	// you can stick a pointer to your own data in here..
	//
	STDMETHOD(HrSetPrivateData)		(THIS_ PVOID pvData) PURE;	// you can store a pointer to YOUR data directly
													// in the property record
	STDMETHOD(HrGetPrivateData)		(THIS_ PVOID *ppvData) PURE;
};

//
// PRIVATE MESSAGES
//
#undef INTERFACE
#define INTERFACE ICSPrivateMsg
DECLARE_INTERFACE_(ICSPrivateMsg, IUnknown)
{
	// IUnknown:
	STDMETHOD(QueryInterface)		(THIS_ REFIID riid, LPVOID *ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)		(THIS) PURE;
	STDMETHOD_(ULONG, Release)		(THIS) PURE;
	
	// ICSPrivateMsg:
	STDMETHOD(HrGetMsg)				(THIS_ PCS_PRIVMSG pcsPrivMsg) PURE;
	STDMETHOD(HrGetMsgSender)		(THIS_ BYTE **ppbName, BOOL *pfAnsi) PURE;

	STDMETHOD(HrReplyTextA)			(THIS_ CHAR *szReply) PURE;
	STDMETHOD(HrReplyTextW)			(THIS_ WCHAR *wszReply) PURE;
	STDMETHOD(HrReplyData)			(THIS_ BYTE *pbData,DWORD dwcbData) PURE;

	STDMETHOD(HrForwardA)			(THIS_ CHAR **rgszName, DWORD dwcName) PURE;
	STDMETHOD(HrForwardW)			(THIS_ WCHAR **rgwszName, DWORD dwcName) PURE;
	STDMETHOD(HrForward)			(THIS_ DWORD *rgdwID, DWORD dwcID) PURE;
};

//
// INVITES
//
#undef INTERFACE
#define INTERFACE ICSInvitation
DECLARE_INTERFACE_(ICSInvitation, IUnknown)
{
	// IUnknown:
	STDMETHOD(QueryInterface)		(THIS_ REFIID riid, LPVOID *ppvObj) PURE;
	STDMETHOD_(ULONG, AddRef)		(THIS) PURE;
	STDMETHOD_(ULONG, Release)		(THIS) PURE;
	// ICSInvitation
	STDMETHOD(HrGetSender)			(THIS_ BYTE **ppbName, BOOL *pfAnsi) PURE;
	STDMETHOD(HrGetChannelName)		(THIS_ BYTE **ppbName, BOOL *pfAnsi) PURE;
	STDMETHOD(HrJoinChannel)		(THIS) PURE;

	STDMETHOD(HrGetSenderRealName)	(THIS) PURE;
};

#include <poppack.h>

#endif 
