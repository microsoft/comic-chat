//****************************************************************************
//  Module:     Chappy.EXE
//  File:       Globals.H
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef __GLOBALS_H__
#define __GLOBALS_H__

#define IDC_MESSAGE_TAB			5001
#define IDC_INPUT_RICHEDIT		5002
#define IDC_PART_LIST_TREE		5003
#define IDC_SEND_TOOLBAR		5004
#define IDC_HISTORY_RICHEDIT	5005
#define IDC_ROOM_LIST			5006

//image list indexs
#define IMG_OFF			0
#define IMG_IMPORTANT	1
#define IMG_SAY			2
#define IMG_WHISPER		3
#define IMG_DO			4
#define IMG_ROOM_LIST	5

//|typedef /* [public][public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0001
//|    {	stServerMessage	= 1,
//|	stChannelAccount	= 2,
//|	stChannelClientData	= 3,
//|	stChannelHostKey	= 4,
//|	stChannelKeyword	= 5,
//|	stChannelLanguage	= 6,
//|	stChannelName	= 7,
//|	stChannelOnJoin	= 8,
//|	stChannelOnPart	= 9,
//|	stChannelOwnerKey	= 10,
//|	stChannelRating	= 11,
//|	stChannelTopic	= 12,
//|	stChannelURL	= 13
//|    }	enumStringType;
//|
//|typedef /* [public][public][public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0002
//|    {	etMessageOfTheDay	= 0,
//|	etUsers	= 0x1,
//|	etChannels	= 0x2,
//|	etChannelsTruncated	= 0x3,
//|	etMembers	= 0x4,
//|	etInitialMembers	= 0x5
//|    }	enumEnumType;
//|
//|typedef /* [public][public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0003
//|    {	ctIRC	= 0,
//|	ctIRCX	= 0x1
//|    }	enumConnectionType;
//|
//|typedef /* [public][public][public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0004
//|    {	csDisconnected	= 0,
//|	csConnecting	= 0x1,
//|	csConnected	= 0x2,
//|	csLogging	= 0x3,
//|	csLogged	= 0x4,
//|	csDisconnecting	= 0x5
//|    }	enumConnectionState;
//|
//|typedef /* [public][public][public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0005
//|    {	chsClosed	= 0,
//|	chsOpening	= 0x1,
//|	chsOpen	= 0x2
//|    }	enumChannelState;
//|
//|typedef /* [public][public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0006
//|    {	smtNormal	= 0,
//|	smtNotice	= 0x1,
//|	smtMessageOfTheDay	= 0x2,
//|	smtServerInfo	= 0x3,
//|	smtError	= 0x4
//|    }	enumServerMessageType;
//|
//|typedef /* [public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0007
//|    {	pmtNormal	= 0,
//|	pmtNotice	= 0x1,
//|	pmtData	= 0x2,
//|	pmtAway	= 0x4,
//|	pmtVersion	= 0x8,
//|	pmtLagTime	= 0x10,
//|	pmtLocalTime	= 0x20,
//|	pmtAction	= 0x40,
//|	pmtSound	= 0x80,
//|	pmtCTCP	= 0x100,
//|	pmtInfo	= 0x200
//|    }	enumPrivateMessageType;
//|
//|typedef /* [public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0008
//|    {	msgtNormal	= 0,
//|	msgtNotice	= 0x1,
//|	msgtData	= 0x2,
//|	msgtWhisper	= 0x4,
//|	msgtThought	= 0x8,
//|	msgtAction	= 0x10,
//|	msgtSound	= 0x20,
//|	msgtCTCP	= 0x40,
//|	msgtInfo	= 0x80
//|    }	enumMessageType;
//|
//|typedef /* [public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0009
//|    {	umNone	= 0,
//|	umClientIgnored	= 0x1,
//|	umNotClientIgnored	= 0x2,
//|	umWhisper	= 0x4,
//|	umNoWhisper	= 0x8,
//|	umAuthUser	= 0x10,
//|	umNotAuthUser	= 0x20,
//|	umAway	= 0x40,
//|	umNotAway	= 0x80,
//|	umSysop	= 0x100,
//|	umNotSysop	= 0x200,
//|	umAdmin	= 0x400,
//|	umNotAdmin	= 0x800,
//|	umInvisible	= 0x1000,
//|	umNotInvisible	= 0x2000,
//|	umNotices	= 0x4000,
//|	umNoNotices	= 0x8000,
//|	umWallops	= 0x10000,
//|	umNoWallops	= 0x20000
//|    }	enumUserMode;
//|
//|typedef /* [public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0010
//|    {	mmNone	= 0,
//|	mmClientIgnored	= 0x1,
//|	mmNotClientIgnored	= 0x2,
//|	mmWhisper	= 0x4,
//|	mmNoWhisper	= 0x8,
//|	mmAuthMember	= 0x10,
//|	mmNotAuthMember	= 0x20,
//|	mmAway	= 0x40,
//|	mmNotAway	= 0x80,
//|	mmSysop	= 0x100,
//|	mmNotSysop	= 0x200,
//|	mmOwner	= 0x400,
//|	mmNotOwner	= 0x800,
//|	mmHost	= 0x1000,
//|	mmNotHost	= 0x2000,
//|	mmVoice	= 0x4000,
//|	mmNoVoice	= 0x8000
//|    }	enumMemberMode;
//|
//|typedef /* [public] */ 
//|enum __MIDL___MIDL_itf_mschatx_0000_0011
//|    {	cmNone	= 0,
//|	cmPublic	= 0,
//|	cmPrivate	= 0x1,
//|	cmHidden	= 0x2,
//|	cmSecret	= 0x4,
//|	cmModerated	= 0x8,
//|	cmNoExtern	= 0x10,
//|	cmTopicop	= 0x20,
//|	cmInvite	= 0x40,
//|	cmKnock	= 0x80,
//|	cmNoWhisper	= 0x100,
//|	cmRegistered	= 0x200,
//|	cmService	= 0x400,
//|	cmAuthOnly	= 0x800,
//|	cmCloneable	= 0x1000,
//|	cmClone	= 0x2000,
//|	cmAuditorium	= 0x4000,
//|	cmCreateOnly	= 0x10000000
//|    }	enumChannelMode;
//|
//|

/****************************************
 * Generated header for library: MsChatPr
 * at Thu Aug 14 10:35:49 1997
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 


typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0001
    {	stServerMessage	= 1,
	stChannelAccount	= 2,
	stChannelClientData	= 3,
	stChannelHostKey	= 4,
	stChannelKeyword	= 5,
	stChannelLanguage	= 6,
	stChannelName	= 7,
	stChannelOnJoin	= 8,
	stChannelOnPart	= 9,
	stChannelOwnerKey	= 10,
	stChannelRating	= 11,
	stChannelSubject	= 12,
	stChannelTopic	= 13
    }	enumStringType;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0002
    {	etMessageOfTheDay	= 0,
	etUsers	= 0x1,
	etChannels	= 0x2,
	etChannelsTruncated	= 0x3,
	etMembers	= 0x4,
	etInitialMembers	= 0x5
    }	enumEnumType;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0003
    {	ctIRC	= 0,
	ctIRCX	= 0x1
    }	enumConnectionType;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0004
    {	cvtToUTF8	= 0,
	cvtToUnicode	= 0x1
    }	enumConversionType;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0005
    {	cvsNormal	= 0,
	cvsNickname	= 0x1,
	cvsChannelName	= 0x2
    }	enumConversionSource;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0006
    {	csDisconnected	= 0,
	csConnecting	= 0x1,
	csConnected	= 0x2,
	csLogging	= 0x3,
	csLogged	= 0x4,
	csDisconnecting	= 0x5
    }	enumConnectionState;

typedef /* [public][public][public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0007
    {	chsClosed	= 0,
	chsOpening	= 0x1,
	chsOpen	= 0x2
    }	enumChannelState;

typedef /* [public][public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0008
    {	smtNormal	= 0,
	smtNotice	= 0x1,
	smtMessageOfTheDay	= 0x2,
	smtServerInfo	= 0x3,
	smtError	= 0x4
    }	enumServerMessageType;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0009
    {	pmtNormal	= 0,
	pmtNotice	= 0x1,
	pmtData	= 0x2,
	pmtDataRequest	= 0x4,
	pmtDataReply	= 0x8,
	pmtDataRaw	= 0x10,
	pmtAway	= 0x20,
	pmtVersion	= 0x40,
	pmtLagTime	= 0x80,
	pmtLocalTime	= 0x100,
	pmtAction	= 0x200,
	pmtSound	= 0x400,
	pmtCTCP	= 0x800,
	pmtInfo	= 0x1000
    }	enumPrivateMessageType;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0010
    {	msgtNormal	= 0,
	msgtNotice	= 0x1,
	msgtData	= 0x2,
	msgtDataRequest	= 0x4,
	msgtDataReply	= 0x8,
	msgtDataRaw	= 0x10,
	msgtWhisper	= 0x20,
	msgtThought	= 0x40,
	msgtAction	= 0x80,
	msgtSound	= 0x100,
	msgtCTCP	= 0x200,
	msgtInfo	= 0x400
    }	enumMessageType;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0011
    {	umNone	= 0,
	umClientIgnored	= 0x1,
	umNotClientIgnored	= 0x2,
	umAuthUser	= 0x4,
	umNotAuthUser	= 0x8,
	umAway	= 0x10,
	umNotAway	= 0x20,
	umSysop	= 0x40,
	umNotSysop	= 0x80,
	umAdmin	= 0x100,
	umNotAdmin	= 0x200,
	umInvisible	= 0x400,
	umNotInvisible	= 0x800,
	umNotices	= 0x1000,
	umNoNotices	= 0x2000,
	umWallops	= 0x4000,
	umNoWallops	= 0x8000
    }	enumUserMode;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0012
    {	mmNone	= 0,
	mmClientIgnored	= 0x1,
	mmNotClientIgnored	= 0x2,
	mmAuthMember	= 0x4,
	mmNotAuthMember	= 0x8,
	mmAway	= 0x10,
	mmNotAway	= 0x20,
	mmSysop	= 0x40,
	mmNotSysop	= 0x80,
	mmOwner	= 0x100,
	mmNotOwner	= 0x200,
	mmHost	= 0x400,
	mmNotHost	= 0x800,
	mmVoice	= 0x1000,
	mmNoVoice	= 0x2000
    }	enumMemberMode;

typedef /* [public] */ 
enum __MIDL___MIDL_itf_mschatpr_0000_0013
    {	cmNone	= 0,
	cmPublic	= 0,
	cmPrivate	= 0x1,
	cmHidden	= 0x2,
	cmSecret	= 0x4,
	cmModerated	= 0x8,
	cmNoExtern	= 0x10,
	cmTopicop	= 0x20,
	cmInvite	= 0x40,
	cmKnock	= 0x80,
	cmNoWhisper	= 0x100,
	cmRegistered	= 0x200,
	cmService	= 0x400,
	cmAuthOnly	= 0x800,
	cmCloneable	= 0x1000,
	cmClone	= 0x2000,
	cmAuditorium	= 0x4000,
	cmNoFormat	= 0x8000,
	cmCreateOnly	= 0x10000000
    }	enumChannelMode;

#endif //__GLOBALS_H__