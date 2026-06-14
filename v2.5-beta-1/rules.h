//=--------------------------------------------------------------------------=
// Rules.H
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 01/16/98

#ifndef __RULES_H__

#include "defines.h"
#include "resource.h"
#include "ccomp.h"
#include "query.h"
#include "userlist.h"

const UINT		g_uErrVersion			= (CFileException::endOfFile+1);
const UINT		g_uErrFormat			= (g_uErrVersion+1);
const UINT		g_uErrRulesSkipped		= (g_uErrFormat+1);
const UINT		g_uErrOOM				= (g_uErrRulesSkipped+1);

const UINT		g_uErrFlooding			= 0;

const UINT		g_uMaxEventParams		= 3;	// maximum number of parameters for an event
const UINT		g_uMaxActionParams		= 3;	// maximum number of parameters for an action

const UINT		g_uMaxSetNameLength		= 19;
const UINT		g_uMaxParamLength		= 128;
const UINT		g_uMaxShortParamLength	= 14;
const UINT		g_uRuleFixedPrefix		= 17;
const UINT		g_uMaxSerializedRule	= g_uRuleFixedPrefix+(g_uMaxParamLength+1)*(g_uMaxEventParams+g_uMaxActionParams*(MAX_FORMATTINGPERBYTE+1))+9;

const UCHAR		g_uDefRuleFloodOcc		= 12;
const UCHAR		g_uDefRuleFloodInt		= 4;	// Max of 12 occurrences of same rule in 4 seconds to avoid fast loops

const TCHAR		g_szBeginParams[]		= "(";	// OnConnect(<%Me>, <%Any>)
const TCHAR		g_szEndParams[]			= ")";
const TCHAR		g_szBeginParam[]		= "<";
const TCHAR		g_szEndParam[]			= ">";
const TCHAR		g_szParamSeparator[]	= ", ";
const TCHAR		g_szContinuation[]		= "...";
const TCHAR		g_szRuleSetsSubKey[]	= _T("\\RuleSets");
const TCHAR		g_szRuleSetFlags[]		= _T("RuleSetFlags");
const TCHAR		g_szRuleSetsClass[]		= _T("Rule Sets Data");
const TCHAR		g_szRulesClass[]		= _T("Rules Data");
const TCHAR		g_szRuleSetFileExt[]	= _T("crs");	// Chat Rule Set file

const WORD		g_wDoNotDisplay			= 0x0001;
const WORD		g_wHighlight			= 0x0002;
const WORD		g_wReplace				= 0x0004;

const UINT		g_uRulesDaemonTimer		= 82;
const UINT		g_uRulesDaemonShortElapse	= 12;		// 12 seconds
const UINT		g_uRulesDaemonLongElapse	= 150;		// 2.5 minutes

const UINT		g_uDelayedRulesTimer	= 84;
const UINT		g_uDelayedRulesElapse	= 1;		// 1 second increment

const WORD		g_wVersion				= 0x0001;	// version info for rules serialization

const WORD		g_wActive				= 0x0001;
const WORD		g_wNoSubsequent			= 0x0002;
const WORD		g_wMatchCase			= 0x0004;
const WORD		g_wMatchWord			= 0x0008;

const WORD		g_wStopped				= 0x0040;
const WORD		g_wSortDescending		= 0x0080;

//const WORD	g_wGeneral				= 0x0010;
//const WORD	g_wModified				= 0x0020;

typedef enum
{
	eOnConnect,			// 0
	eOnDisconnect,		// 1
	eOnInvitation,		// 2
	eOnJoin,			// 3
	eOnKick,			// 4
	eOnLeave,			// 5
	eOnMessage,			// 6
	eOnNewHost,			// 7
	eOnNewRoom,			// 8
	eOnWhisper,			// 9
	eOnWhisperInRoom,	// 10
	eMax
} enumEvents;


typedef enum
{
	aBan,					// 0x00000001 0
	aBeep,					// 0x00000002 1
	aDoNotDisplay,			// 0x00000004 2
	aExecuteMacro,			// 0x00000008 3
	aGetIdentity,			// 0x00000010 4
	aGetLagTime,			// 0x00000020 5
	aGetLocalTime,			// 0x00000040 6
	aGetProfile,			// 0x00000080 7
	aGetVersion,			// 0x00000100 8
	aHighlightMessage,		// 0x00000200 9
	aIgnore,				// 0x00000400 10
	aInvite,				// 0x00000800 11
	aJoinRoom,				// 0x00001000 12
	aKick,					// 0x00002000 13
	aLeaveRoom,				// 0x00004000 14
	aMakeHost,				// 0x00008000 15
	aNotifyDialog,			// 0x00010000 16
	aPlaySound,				// 0x00020000 17
	aConnect,				// 0x00040000 18
	aReplaceMessage,		// 0x00080000 19
	aSendAction,			// 0x00100000 20
	aSendFileLine,			// 0x00200000 21
	aSendMessage,			// 0x00400000 22
	aSendSound,				// 0x00800000 23
	aSendThought,			// 0x01000000 24
	aSendWhisper,			// 0x02000000 25
	aSendWhisperInRoom,		// 0x04000000 26
	aWhisperFileLine,		// 0x08000000 27
	aDisconnect,			// 0x10000000 28
	aActivateRuleSet,		// 0x20000000 29
	aMax
} enumActions;


typedef enum
{
	ptActivate,			// 0x0001 - 0
	ptBeepCount,		// 0x0002 - 1
	ptHighlight,		// 0x0004 - 2
	ptLineNumber,		// 0x0008 - 3
	ptMacroName,		// 0x0010 - 4
	ptMessage,			// 0x0020 - 5
	ptNickname,			// 0x0040 - 6
	ptReason,			// 0x0080 - 7
	ptRoomName,			// 0x0100 - 8
	ptRuleSetName,		// 0x0200 - 9
	ptServerName,		// 0x0400 - 10
	ptSoundFileName,	// 0x0800 - 11
	ptTextFileName,		// 0x1000 - 12
	ptMax
} enumParamType;


typedef enum
{
	kepAny,					// 0x01 - 0
	kepAnyone,				// 0x02 - 1
	kepMe,					// 0x04 - 2
	kepAnyoneButMe,			// 0x08 - 3
	kepAnyOfMyRooms,		// 0x10 - 4
	kepMyActivatedRoom,		// 0x20 - 5
	kepMyInactivatedRooms,	// 0x40 - 6
	kepMax					// maximum number of keyword event parameters
} enumKeyEventParam;


typedef struct tagKEPSUBS
{
	enumKeyEventParam	kep;
	LPTSTR				szSubs;	// the substitute
} KEPSUBS, *PKEPSUBS;


typedef enum
{
	kapMyActivatedRoom,	// 0x0001 0
	kapAll,				// 0x0002 1
	kapEventMessage,	// 0x0004 2
	kapEventNickname,	// 0x0008 3
	kapEventRoom,		// 0x0010 4
	kapEventServer,		// 0x0020 5
	kapRandom,			// 0x0040 6
	kapYes,				// 0x0080 7
	kapNo,				// 0x0100 8
	kapEventRecipients,	// 0x0200 9
	kapMe,				// 0x0400 10
	kapMax				// maximum number of default action parameters
} enumKeyActionParam;


typedef struct tagKAPSUBS
{
	enumKeyActionParam	kap;
	LPTSTR				szSubs;	// the substitute
} KAPSUBS, *PKAPSUBS;


typedef enum
{
	itChannel,
	itUser,
	itMax
} enumItemTypes;


typedef enum
{
	etMinDelay,
	etMax
} enumExceptionTypes;


typedef struct tagRULEEXCEPTION
{
	enumExceptionTypes	ex;
	enumEvents			eID;
	enumActions			aID;
	DWORD				dwValue;
} RULEX, *PRULEX;


const UINT	g_uExceptionCount = 1;
const RULEX	g_rgex[] = {
	{ etMinDelay, eOnDisconnect, aConnect, 5L }
};


const UINT	g_rguEventParamNums[] = { 2, // OnConnect
									  2, // OnDisconnect
									  2, // OnInvitation
									  2, // OnJoin
									  2, // OnKick
									  2, // OnLeave
									  3, // OnMessage
									  2, // OnNewHost
									  1, // OnNewRoom
									  2, // OnWhisper
									  3  // OnWhisperInRoom
};


const BOOL	g_rgbEventNeedDaemon[] = { 1, // OnConnect
									   1, // OnDisconnect
									   0, // OnInvitation
									   0, // OnJoin
									   0, // OnKick
									   0, // OnLeave
									   0, // OnMessage
									   0, // OnNewHost
									   1, // OnNewRoom
									   0, // OnWhisper
									   0  // OnWhisperInRoom
};


const WORD g_rgwActionParamFlags[] = { 0x0010, // Ban
									   0x0011, // Beep
									   0x0000, // DoNotDisplay
									   0x0011, // ExecuteMacro
									   0x0010, // GetIdentity
									   0x0010, // GetLagTime
									   0x0011, // GetLocalTime
									   0x0010, // GetProfile
									   0x0010, // GetVersion
									   0x0001, // HighlightMessage
									   0x0010, // Ignore
									   0x0011, // Invite
									   0x0011, // JoinRoom
									   0x0011, // Kick
									   0x0010, // LeaveRoom
									   0x0010, // MakeHost
									   0x0011, // NotifyDialog
									   0x0011, // PlaySound
									   0x0012, // Connect
									   0x0001, // ReplaceMessage
									   0x0012, // SendAction
									   0x0013, // SendFileLine
									   0x0012, // SendMessage
									   0x0013, // SendSound
									   0x0012, // SendThought
									   0x0012, // SendWhisper
									   0x0013, // SendWhisperInRoom
									   0x0013, // WhisperFileLine
									   0x0010, // Disconnect
									   0x0012  // ActivateRuleSet
};


const enumParamType	g_rgptEventParamTypes[][g_uMaxEventParams] = 
{
	{ptNickname,	ptServerName,	ptMax},		// OnConnect
	{ptNickname,	ptServerName,	ptMax},		// OnDisconnect
	{ptNickname,	ptRoomName,		ptMax},		// OnInvitation
	{ptNickname,	ptRoomName,		ptMax},		// OnJoin
	{ptNickname,	ptRoomName,		ptMax},		// OnKick
	{ptNickname,	ptRoomName,		ptMax},		// OnLeave
	{ptNickname,	ptRoomName,		ptMessage},	// OnMessage
	{ptNickname,	ptRoomName,		ptMax},		// OnNewHost
	{ptRoomName,	ptMax,			ptMax},		// OnNewRoom
	{ptNickname,	ptMessage,		ptMax},		// OnWhisper
	{ptNickname,	ptRoomName,		ptMessage}	// OnWhisperInRoom
};


const UINT	g_rguEventKeyParams[][g_uMaxEventParams] = 
{
	{0x04, 0x01, 0x00},	// OnConnect			Me						Any
	{0x04, 0x01, 0x00},	// OnDisconnect			Me						Any
	{0x08, 0x01, 0x00},	// OnInvitation			AnyoneButMe				Any
	{0x0E, 0x70, 0x00},	// OnJoin				Anyone+Me+AnyoneButMe	MyActivatedRoom+MyInactivatedRooms+AnyOfMyRooms
	{0x0E, 0x70, 0x00},	// OnKick				Anyone+Me+AnyoneButMe	MyActivatedRoom+MyInactivatedRooms+AnyOfMyRooms
	{0x0E, 0x70, 0x00},	// OnLeave				Anyone+Me+AnyoneButMe	MyActivatedRoom+MyInactivatedRooms+AnyOfMyRooms
	{0x08, 0x70, 0x01},	// OnMessage			AnyoneButMe				MyActivatedRoom+MyInactivatedRooms+AnyOfMyRooms	Any
	{0x0E, 0x70, 0x00},	// OnNewHost			Anyone+Me+AnyoneButMe	MyActivatedRoom+MyInactivatedRooms+AnyOfMyRooms		
	{0x00, 0x00, 0x00},	// OnNewRoom
	{0x08, 0x01, 0x00},	// OnWhisper			AnyoneButMe				Any
	{0x08, 0x70, 0x01}	// OnWhisperInRoom		AnyoneButMe				MyActivatedRoom+MyInactivatedRooms+AnyOfMyRooms	Any
};


const DWORD	g_rgdwEventEnabledActions[] = { 0x3FF31C0A, // OnConnect
											0x2FF7000A, // OnDisconnect
											0x3FF31C0A, // OnInvitation
											0x3FF3FFFB, // OnJoin
											0x3FF35E0B, // OnKick
											0x3FF35E0B, // OnLeave
											0x3FFBFFFF, // OnMessage
											0x3FF3F9FB, // OnNewHost
											0x3FF3100A, // OnNewRoom
											0x3FFB1E0E, // OnWhisper
											0x3FFBFFFF  // OnWhisperInRoom
};


// kapMyActivatedRoom, kapAll, kapRandom, kapYes, kapNo, and kapMe are added for all events (0x05C3)
const DWORD	g_rgdwEventExposedActionKeys[] = { 0x05EB, // OnConnect		kapEventNickname+kapEventServer
											   0x05EB, // OnDisconnect	kapEventNickname+kapEventServer
											   0x05DB, // OnInvitation	kapEventNickname+kapEventRoom
											   0x05DB, // OnJoin		kapEventNickname+kapEventRoom
											   0x05DB, // OnKick		kapEventNickname+kapEventRoom
											   0x05DB, // OnLeave		kapEventNickname+kapEventRoom
											   0x08DF, // OnMessage		kapEventNickname+kapEventRoom+kapEventMessage+kapEventRecipients
											   0x05DB, // OnNewHost		kapEventNickname+kapEventRoom
											   0x05D3, // OnNewRoom		kapEventRoom
											   0x05CF, // OnWhisper		kapEventNickname+kapEventMessage
											   0x08DF  // OnWhisperInRoom kapEventNickname+kapEventRoom+kapEventMessage+kapEventRecipients
};


const enumParamType	g_rgptActionParamTypes[][g_uMaxActionParams] = 
{
	{ptMax,				ptMax,			ptMax},				// Ban
	{ptBeepCount,		ptMax,			ptMax},				// Beep
	{ptMax,				ptMax,			ptMax},				// DoNotDisplay
	{ptMacroName,		ptMax,			ptMax},				// ExecuteMacro
	{ptMax,				ptMax,			ptMax},				// GetIdentity
	{ptMax,				ptMax,			ptMax},				// GetLagTime
	{ptNickname,		ptMax,			ptMax},				// GetLocalTime
	{ptMax,				ptMax,			ptMax},				// GetProfile
	{ptMax,				ptMax,			ptMax},				// GetVersion
	{ptHighlight,		ptMax,			ptMax},				// HighlightMessage
	{ptMax,				ptMax,			ptMax},				// Ignore
	{ptRoomName,		ptMax,			ptMax},				// Invite
	{ptRoomName,		ptMax,			ptMax},				// JoinRoom
	{ptReason,			ptMax,			ptMax},				// Kick
	{ptMax,				ptMax,			ptMax},				// LeaveRoom
	{ptMax,				ptMax,			ptMax},				// MakeHost
	{ptMessage,			ptMax,			ptMax},				// NotifyDialog
	{ptSoundFileName,	ptMax,			ptMax},				// PlaySound
	{ptNickname,		ptServerName,	ptMax},				// Connect
	{ptMessage,			ptMax,			ptMax},				// ReplaceMessage
	{ptRoomName,		ptMessage,		ptMax},				// SendAction
	{ptRoomName,		ptTextFileName,	ptLineNumber},		// SendFileLine
	{ptRoomName,		ptMessage,		ptMax},				// SendMessage
	{ptRoomName,		ptMessage,		ptSoundFileName},	// SendSound
	{ptRoomName,		ptMessage,		ptMax},				// SendThought
	{ptNickname,		ptMessage,		ptMax},				// SendWhisper
	{ptNickname,		ptRoomName,		ptMessage},			// SendWhisperInRoom
	{ptNickname,		ptTextFileName,	ptLineNumber},		// WhisperFileLine
	{ptMax,				ptMax,			ptMax},				// Disconnect
	{ptRuleSetName,		ptActivate,		ptMax}				// ActivateRuleSet
};


const UINT	g_rguActionKeyParams[][g_uMaxActionParams] = 
{
	{0x0000, 0x0000, 0x0000},		// Ban
	{0x0000, 0x0000, 0x0000},		// Beep
	{0x0000, 0x0000, 0x0000},		// DoNotDisplay
	{0x0000, 0x0000, 0x0000},		// ExecuteMacro
	{0x0000, 0x0000, 0x0000},		// GetIdentity
	{0x0000, 0x0000, 0x0000},		// GetLagTime
	{0x0408, 0x0000, 0x0000},		// GetLocalTime
	{0x0000, 0x0000, 0x0000},		// GetProfile
	{0x0000, 0x0000, 0x0000},		// GetVersion
	{0x0000, 0x0000, 0x0000},		// HighlightMessage
	{0x0000, 0x0000, 0x0000},		// Ignore
	{0x0011, 0x0000, 0x0000},		// Invite
	{0x0010, 0x0000, 0x0000},		// JoinRoom
	{0x0000, 0x0000, 0x0000},		// Kick
	{0x0000, 0x0000, 0x0000},		// LeaveRoom
	{0x0000, 0x0000, 0x0000},		// MakeHost
	{0x0004, 0x0000, 0x0000},		// NotifyDialog
	{0x0000, 0x0000, 0x0000},		// PlaySound
	{0x0008, 0x0020, 0x0000},		// Connect
	{0x0000, 0x0000, 0x0000},		// ReplaceMessage
	{0x0011, 0x0000, 0x0000},		// SendAction
	{0x0011, 0x0000, 0x0042},		// SendFileLine
	{0x0011, 0x0004, 0x0000},		// SendMessage
	{0x0011, 0x0004, 0x0000},		// SendSound
	{0x0011, 0x0000, 0x0000},		// SendThought
	{0x0008, 0x0004, 0x0000},		// SendWhisper
	{0x0208, 0x0011, 0x0004},		// SendWhisperInRoom
	{0x0008, 0x0000, 0x0042},		// WhisperFileLine
	{0x0000, 0x0000, 0x0000},		// Disconnect
	{0x0000, 0x0180, 0x0000}		// ActivateRuleSet
};


class CCRulesData;
class CCDynaRules;
class CCUserPtrArray;
class CCDaemonExt;
class CCRuleSet;
class CCRule;
class CCDynaNotifs;
class CCActionContext;
class CCDelayedRules;

typedef BOOL	(__cdecl * EVENT_KEY_PARAM_FN)(CString&, enumKeyEventParam);
typedef BOOL	(__cdecl * EVENT_RND_PARAM_FN)(CString&, CString&, PPRUSERMATCH, WORD, enumParamType);
typedef CString	(__cdecl * GET_EVENT_KEY_FN)(enumParamType);
typedef CString	(__cdecl * GET_ACTION_KEY_FN)(enumKeyActionParam, CString&, CString&, CString&, CString&, CString&);
typedef BOOL	(__cdecl * EXECUTE_ACTION_FN)(CCDynaRules*, CCRule*, CCActionContext*);
typedef BOOL	(__cdecl * RULE_FAILURE_FN)(CCRuleSet*, CCRule*, UINT);
typedef BOOL	(__cdecl * RULEDAEMON_QUERY_FN)(CCRule*);

/////////////////////////////////////////////////////////////////////////////
// Macros
#define RTFParam(ptActionParam, aID)	(aID != aNotifyDialog && ptActionParam == ptMessage)
#define ActionParamNum(iIndex)	((UINT) (g_rgwActionParamFlags[iIndex] & 0x000F))
#define ActionDelayOK(iIndex)	((BOOL) (g_rgwActionParamFlags[iIndex] & 0x00F0) ? TRUE : FALSE)


/////////////////////////////////////////////////////////////////////////////
// Static data
class CCEvent
{
friend class CCRulesData;
friend class CCDynaRules;
friend class CCRule;

public:
    CCEvent(const enumEvents eID,
			const UINT uIDS_LongDesc,
			const UINT uIDS_ShortDesc,
			const UINT rguIDS_ParamDesc[g_uMaxEventParams],
			const UINT uParamNum,
			const UINT rguKeyParam[g_uMaxEventParams],
			const enumParamType rgpt[g_uMaxEventParams],
			const DWORD dwActionKeysExposed,
			const DWORD dwEnabledActions,
			const BOOL bNeedDaemon);
    virtual ~CCEvent() {};

	enumEvents		GetID()						{ return m_eID; }
	CString&		GetLongDesc()				{ return m_strLongDesc; }
	CString&		GetParamDesc(UINT uIndex)	{ return m_rgstrParamDesc[uIndex]; }

	DWORD			GetEnabledActions()			{ return m_dwEnabledActions; }
	enumParamType	GetParamType(UINT uIndex)	{ return m_rgpt[uIndex]; }
	UINT			GetParamNum()				{ return m_uParamNum; }
	UINT			GetKeyParam(UINT uIndex)	{ return m_rguKeyParam[uIndex]; }
	DWORD			GetActionKeysExposed()		{ return m_dwActionKeysExposed; }

protected:
	// might want to remove the UINT IDSs and just keep the CStrings
	enumEvents		m_eID;									// 0 -> eMax-1
	UINT			m_uIDS_LongDesc;						// A user connects to your server
	UINT			m_uIDS_ShortDesc;						// OnConnect
	UINT			m_rguIDS_ParamDesc[g_uMaxEventParams];	// Nickname of the user that just connected. Use the keyword %Me for yourself or type a nickname.
	UINT			m_uParamNum;							// number of parameters for this event
	UINT			m_rguKeyParam[g_uMaxEventParams];		// possible keyword values for each event parameter (bit combination)
	enumParamType	m_rgpt[g_uMaxEventParams];				// Type of each event parameter
	DWORD			m_dwActionKeysExposed;					// Bit combination of exposed action keys (%EventNickname, %EventRoom, etc...)
	DWORD			m_dwEnabledActions;						// Bit combination of enabled actions
	CString			m_strLongDesc;							// Long description string of event: A user connects to your server
	CString			m_strShortDesc;							// Short description string of event: OnConnect
	CString			m_rgstrParamDesc[g_uMaxEventParams];	// Description strings of event parameters
	BOOL			m_bNeedDaemon;							// This event needs a daemon to check if it occurred periodically
};


class CCAction
{
friend class CCRulesData;
friend class CCDynaRules;
friend class CCRule;

public:
    CCAction(const enumActions aID,
			 const UINT uIDS_LongDesc,
			 const UINT uIDS_ShortDesc,
			 const UINT rguIDS_ParamDesc[g_uMaxActionParams],
			 const UINT uParamNum,
			 const BOOL bDelayOK,
			 const UINT rguKeyParam[g_uMaxActionParams],
			 const enumParamType rgpt[g_uMaxActionParams]);
	virtual ~CCAction() {};

	enumActions		GetID()						{ return m_aID; }
	CString&		GetLongDesc()				{ return m_strLongDesc; }
	CString&		GetParamDesc(UINT uIndex)	{ return m_rgstrParamDesc[uIndex]; }
	enumParamType	GetParamType(UINT uIndex)	{ return m_rgpt[uIndex]; }
	UINT			GetParamNum()				{ return m_uParamNum; }
	UINT			GetKeyParam(UINT uIndex)	{ return m_rguKeyParam[uIndex]; }
	BOOL			GetDelayOK()				{ return m_bDelayOK; }

protected:
	// might want to remove the UINT IDSs and just keep the CStrings
	enumActions		m_aID;									// 0 -> aMax-1
	UINT			m_uIDS_LongDesc;						// Send a message to a channel
	UINT			m_uIDS_ShortDesc;						// SendMessage
	UINT			m_rguIDS_ParamDesc[g_uMaxActionParams];	// Name of the room to send a message to.
	UINT			m_uParamNum;							// number of parameters for this action
	UINT			m_rguKeyParam[g_uMaxActionParams];		// possible keyword values for each action parameter (bit combination)
	BOOL			m_bDelayOK;								// can this type of action be delayed?
	enumParamType	m_rgpt[g_uMaxActionParams];				// Type of each action parameter
	CString			m_strLongDesc;							// Long description string of action: Send a message to a channel
	CString			m_strShortDesc;							// Short description string of event: SendMessage
	CString			m_rgstrParamDesc[g_uMaxEventParams];	// Description strings of action parameters
};


class CCRulesData
{
public:
	CCRulesData();
	virtual ~CCRulesData();

	BOOL				bInitAlloc();		// mem allocation, returns FALSE if OOM
	BOOL				bLoadStrings();		// load event and action strings

	CCEvent*			GetEvent(UINT uIndex);
	CCAction*			GetAction(UINT uIndex);

	UINT				GetMissingEventParamError(enumParamType pt)
							{ return m_rguIDS_MissingEventParamError[(UINT) pt]; }

	UINT				GetMissingActionParamError(enumParamType pt)
							{ return m_rguIDS_MissingActionParamError[(UINT) pt]; }

	CString				GetKeyEventParam(enumKeyEventParam kep)
							{ return m_rgstrKeyEventParam[(UINT) kep]; }

	CString				GetKeyActionParam(enumKeyActionParam kap)
							{ return m_rgstrKeyActionParam[(UINT) kap]; }

	CString				GetEventsDesc()
							{ return m_strEventsDesc; }

	CString				GetActionsDesc()
							{ return m_strActionsDesc; }

	CString				StrFindAndReplaceKeyParams(CString strIn, BOOL bIncoming);

protected:
	UINT				m_rguIDS_MissingEventParamError[ptMax];
	UINT				m_rguIDS_MissingActionParamError[ptMax];
	CString				m_rgstrKeyEventParam[kepMax];
	CString				m_rgstrKeyActionParam[kapMax];
	CString				m_strEventsDesc;
	CString				m_strActionsDesc;

	CCEvent*			m_rgpEvents[eMax];	// array of events
	CCAction*			m_rgpActions[aMax];	// array of actions
	BOOL				m_bInitAlloc;		// was mem already allocated?
	BOOL				m_bStringsLoaded;	// were strings already loaded?
};


// Dynamic data
class CCChannel
{
friend class CCItemPtrArray;
friend class CCDaemonExt;

public:
	CCChannel() {};
	virtual ~CCChannel() {};

	BOOL	operator==(const CCChannel& channel);

protected:
	CString m_strChannelName;
};


class CCItemPtrArray : public CPtrArray
{
friend class CCDaemonExt;

public:
	CCItemPtrArray(enumItemTypes it = itUser) { m_it = it; };
	virtual ~CCItemPtrArray() { FreeRemoveAll(); }

	void FreeRemoveAll();

protected:
	SHORT			m_nCredits;
	enumItemTypes	m_it;
};


class CCDaemonExt
{
friend class CCRule;
friend class CCNotif;

public:
	CCDaemonExt(enumItemTypes it);
	virtual	~CCDaemonExt();

	enumItemTypes	GetIT() { return m_it; }

	BOOL			bAllocNewItemList(UINT uListCount);
	BOOL			bCleanUpItemLists();
	BOOL			bAddUserToCurrentList(CUser* pUser);
	BOOL			bAddChannelToCurrentList(CString& strChannelName);
	BOOL			bOnEndOfListing(CCDynaRules* pDynaRules, CCRule* pRule, enumQueryPurpose qp);
	BOOL			bOnEndOfListing(CCDynaNotifs* pDynaNotifs, CCNotif* pNotif);
	BOOL			bTreatNewItems(CCDynaRules* pDynaRules, CCRule* pRule, CCItemPtrArray* pPreviousItemList, CCItemPtrArray* pCurrentItemList);
	BOOL			bTreatNewItems(CCDynaNotifs* pDynaNotifs, CCNotif* pNotif, CCItemPtrArray* pPreviousItemList, CCItemPtrArray* pCurrentItemList);
	BOOL			bTreatOldItems(CCDynaRules* pDynaRules, CCRule* pRule, CCItemPtrArray* pPreviousItemList, CCItemPtrArray* pCurrentItemList);
	BOOL			bTreatOldItems(CCDynaNotifs* pDynaNotifs, CCNotif* pNotif, CCItemPtrArray* pPreviousItemList, CCItemPtrArray* pCurrentItemList);
	void			SetResetItemLists(BOOL bResetItemLists) { m_bResetItemLists = bResetItemLists; }
	void			AddRef();
	void			Release();

protected:
	CPtrList		m_itemLists;
	SHORT			m_nRefCount;
	BOOL			m_bResetItemLists;
	BOOL			m_bClearedItemLists;
	enumItemTypes	m_it;
};


class CCActionContext
{
public:
	CCActionContext();
	~CCActionContext();

	enumEvents			GetEventID()
							{ return m_eID; }
	enumActions			GetActionID()
							{ return m_aID; }
	enumKeyActionParam	GetActionKeyParam(UINT uIndex)
							{ return m_rgkap[uIndex]; }
	CString				GetFinalActionParam(UINT uIndex)
							{ return m_rgstrActionFinalParams[uIndex]; }
	CDWordArray*		GetFinalMsgFormatting()
							{ return m_prgdwFinalMsgFormatting; }
	CString				GetCachedIdentity()
							{ return m_strIdentityCach; }
	CString				GetCachedChannel()
							{ return m_strChannelCach; }
	UCHAR				GetDelay()
							{ return m_uDelay; }
	UCHAR				GetDecrementedDelay()
							{ return --m_uDelay; }

	BOOL				bInitActionContext(CCDynaRules* pDynaRules, CCRule* pRule);

protected:
	UCHAR				m_uDelay;

	// CCRule members
	enumEvents			m_eID;
	enumActions			m_aID;
	enumKeyActionParam	m_rgkap[g_uMaxActionParams];
	CString				m_rgstrActionFinalParams[g_uMaxActionParams];
	CDWordArray*		m_prgdwFinalMsgFormatting;

	// CCDynaRules members
	CString				m_strIdentityCach;
	CString				m_strChannelCach;
};


class CCRule
{
friend class CCDynaRules;

public:
	CCRule(CCDynaRules* pDynaRules);
	CCRule(CCRule* pRule, CCDynaRules* pDynaRules);
	virtual ~CCRule();

	void				AddRef();
	void				Release();
	void				CopyRule(CCRule* pRule);

	CString				StrGetEventDisplay();
	CString				StrGetActionDisplay();
	void				Activate()					{ m_wFlags |= g_wActive; }
	void				Desactivate()				{ m_wFlags &= ~g_wActive; }
	BOOL				bActive()					{ return m_wFlags & g_wActive; }
	BOOL				bStopped()					{ return m_wFlags & g_wStopped; }
	WORD				wGetFlags()					{ return m_wFlags; }
	void				SetFlags(WORD wFlags)		{ m_wFlags = wFlags; }
	void				SetDelay(UCHAR uDelay)		{ m_uDelay = uDelay; }
	UCHAR				GetDelay()					{ return m_uDelay; }

	void				SetDynaRules(CCDynaRules* pDynaRules) { m_pDynaRules = pDynaRules; }

	CCEvent*			GetEvent()					{ return m_pEvent; }
	CCAction*			GetAction()					{ return m_pAction; }
	void				SetEvent(CCEvent* pEvent)	{ m_pEvent = pEvent; }
	void				SetAction(CCAction* pAction){ m_pAction = pAction; }

	void				SetEventParam(UINT uIndex, CString& strParam);
	void				SetActionParam(UINT uIndex, CString& strParam) { m_rgstrActionParams[uIndex] = strParam; }

	void				SetEventKeyParam(UINT uIndex, enumKeyEventParam kep)  { m_rgkep[uIndex] = kep; }
	enumKeyEventParam	GetEventKeyParam(UINT uIndex)	{ return m_rgkep[uIndex]; }
	void				SetActionKeyParam(UINT uIndex, enumKeyActionParam kap) { m_rgkap[uIndex] = kap; }
	enumKeyActionParam	GetActionKeyParam(UINT uIndex)	{ return m_rgkap[uIndex]; }

	CString				GetEventParam(UINT uIndex)		{ return m_rgstrEventParams[uIndex]; }
	CString				GetActionParam(UINT uIndex)		{ return m_rgstrActionParams[uIndex]; }
	CString				GetFinalActionParam(UINT uIndex){ return m_rgstrActionFinalParams[uIndex]; }
	CDWordArray*		GetFinalMsgFormatting()			{ return m_prgdwFinalMsgFormatting; }
	CDWordArray*		GetMsgFormatting()				{ return m_prgdwMsgFormatting; }
	void				SetMsgFormatting(CDWordArray* prgdwMsgFormatting, BOOL bMakeCopy);

	CCDaemonExt*		GetDaemonExt()					{ return m_pDaemonExt; }
	void				SetDaemonExt(CCDaemonExt* pDaemonExt) { m_pDaemonExt = pDaemonExt; }
	void				InitRuleDaemon();

	INT					Serialize(LPTSTR szBuff, INT cbBuffLen);
	INT					UnSerialize(LPBYTE pbBuff, INT cbBuffLen);
	BOOL				bUnSerialize(LPCTSTR szRule);

	INT					iGetHighlightTypeIndex(CString strParam);
	BOOL				bValidateRuleEvent(UINT uIndex, CString& strParam, UINT *puErrorIDS);
	BOOL				bValidateRuleAction(UINT uIndex, CString& strParam, UINT *puErrorIDS);
	BOOL				bDaemonNeeded();
	BOOL				bUpdateDaemonExt(BOOL bResetUserLists, enumEvents eID);
	BOOL				bIsFlooding();

protected:
	CString				StrParamBeginning(CString& strParam);

	CCEvent*			m_pEvent;	// rule's event type
	CCAction*			m_pAction;	// rule's action type

	// Context of the event - these are the filters of the event
	CString				m_rgstrEventParams[g_uMaxEventParams];
	enumKeyEventParam	m_rgkep[g_uMaxEventParams];

	// Action parameters
	CString				m_rgstrActionParams[g_uMaxActionParams];
	enumKeyActionParam	m_rgkap[g_uMaxActionParams];
	CDWordArray*		m_prgdwMsgFormatting;

	CString				m_rgstrActionFinalParams[g_uMaxActionParams];
	CDWordArray*		m_prgdwFinalMsgFormatting;

	PRUSERMATCH			m_prUserMatch;

	SHORT				m_nRefCount;
	WORD				m_wFlags;
	UCHAR				m_uDelay;
	USHORT				m_uPeriodStart;		// start of current flood interval
	UCHAR				m_uOccurrences;		// number of utterances within that period (moving average)

	CCDynaRules*		m_pDynaRules;
	CCDaemonExt*		m_pDaemonExt;
};


class CCRuleSet
{
friend class CCDynaRules;

public:
	CCRuleSet(CCDynaRules* pDynaRules);
	CCRuleSet(CCRuleSet* pRuleSet, CCDynaRules* pDynaRules);
	virtual ~CCRuleSet();

	void				SetName(CString strSetName)	{ m_strSetName = strSetName; }
	CString				GetName()					{ return m_strSetName; }
	void				Activate()					{ m_wFlags |= g_wActive; }
	void				Desactivate()				{ m_wFlags &= ~g_wActive; }
	// void				SetModified()				{ m_wFlags |= g_wModified; m_wFlags &= ~g_wGeneral; }
	BOOL				bActive()					{ return m_wFlags & g_wActive; }
	WORD				wGetFlags()					{ return m_wFlags; }
	void				SetFlags(WORD wFlags)		{ m_wFlags = wFlags; }

	void				SetDynaRules(CCDynaRules* pDynaRules) { m_pDynaRules = pDynaRules; }
	CCDynaRules*		GetDynaRules()				{ return m_pDynaRules; }

	CPtrArray&			GetRulesArray()				{ return m_rgpRules; }
	BOOL				bUpdateRulesDaemonExt(BOOL bResetUserLists);

	BOOL				bAddRule(CCRule* pRule, INT iIndex = -1);
	BOOL				bRemoveRule(CCRule* pRule, INT iIndex = -1);
	BOOL				bDuplicateRule(INT iIndex, CCRule** ppRule);
	BOOL				bUpRule(CCRule* pRule, INT iIndex = -1);
	BOOL				bDownRule(CCRule* pRule, INT iIndex = -1);
	BOOL				bSaveToFile(UINT* puError, CWnd* pParentWnd);
	BOOL				bLoadFromFile(UINT* puError, CWnd* pParentWnd);

	BOOL				bDaemonNeeded();

protected:
	void				CleanUpRulesArray();

	CCDynaRules*		m_pDynaRules;
	CPtrArray			m_rgpRules;
	WORD				m_wFlags;
	CString				m_strSetName;
};


class CCDynaRules
{
friend class CCDaemonExt;

public:
	CCDynaRules();
	virtual ~CCDynaRules();

	const CCDynaRules&	operator=(const CCDynaRules& dynaRules);

	CCRulesData*		GetRulesData()
							{ return m_pRulesData; }

	CCRuleSet*			GetSelectedRuleSet()
							{ return m_pSelectedRuleSet; }

	CCRuleSet*			GetRuleSetFromName(LPCTSTR szSetName);

	CPtrArray&			GetRuleSetsArray()
							{ return m_rgpRuleSets; }

	CString				GetCachedIdentity()
							{ return m_strIdentityCach; }

	CString				GetCachedServer()
							{ return m_strServerCach; }

	CString				GetCachedChannel()
							{ return m_strChannelCach; }

	CString				GetCachedRecipients()
							{ return m_strRecipientsCach; }

	CString				GetCachedCFMesage()
							{ return m_strCFMessageCach; }

	CString				GetCachedCLMesage()
							{ return m_strCLMessageCach; }

	WORD				GetFlags()
							{ return m_wFlags; }

	UCHAR				GetFloodingInterval()
							{ return m_uFloodInterval; }

	UCHAR				GetFloodingOccurrences()
							{ return m_uFloodOccurrences; }

	void				SetFloodParams(UCHAR uFloodInterval, UCHAR uFloodOccurrences)
							{ 
							  m_uFloodInterval = uFloodInterval;
							  m_uFloodOccurrences = uFloodOccurrences;
							}

	void				SetCachVariables(enumEvents eID, CString& strIdentity, CString& strServer, CString& strChannel);
	void				SetCachRecipients(CString strRecipients) 
							{ m_strRecipientsCach = strRecipients; }

	void				ResetFlags()
							{ m_wFlags = 0; }

	void				AddFlag(WORD wFlag)
							{ m_wFlags |= wFlag; }

	void				SetRulesData(CCRulesData* pRulesData)
							{ m_pRulesData = pRulesData; }

	void				SetDelayedRules(CCDelayedRules* pDelayedRules)
							{ m_pDelayedRules = pDelayedRules; }

	void				SetSelectedRuleSet(CCRuleSet* pSelectedRuleSet)
							{ m_pSelectedRuleSet = pSelectedRuleSet; }

	void				SetCFFinalMessage(CString strCFFinalMessage) 
							{ m_strCFFinalMessage = strCFFinalMessage; }

	CString				GetCFFinalMessage()
							{ return m_strCFFinalMessage; }

	void				SetEKPFunction(EVENT_KEY_PARAM_FN pfEventKeyParam)
							{ m_pfEventKeyParam = pfEventKeyParam; }

	void				SetERPFunction(EVENT_RND_PARAM_FN pfEventRndParam)
							{ m_pfEventRndParam = pfEventRndParam; }

	void				SetGEKPFunction(GET_EVENT_KEY_FN pfGetKeyEventParam)
							{ m_pfGetKeyEventParam = pfGetKeyEventParam; }

	void				SetGAKPFunction(GET_ACTION_KEY_FN pfGetKeyActionParam)
							{ m_pfGetKeyActionParam = pfGetKeyActionParam; }

	void				SetExecuteActionFunction(EXECUTE_ACTION_FN pfExecuteAction)
							{ m_pfExecuteAction = pfExecuteAction; }

	void				SetRuleFailureFunction(RULE_FAILURE_FN pfRuleFailure)
							{ m_pfRuleFailure = pfRuleFailure; }

	void				SetDaemonQueryFunction(RULEDAEMON_QUERY_FN pfDaemonQuery)
							{ m_pfDaemonQuery = pfDaemonQuery; }

	BOOL				bAddRuleSet(CCRuleSet* pRuleSet, INT iIndex = -1);
	BOOL				bRemoveRuleSet(CCRuleSet* pRuleSet, INT iIndex = -1);
	BOOL				bUpRuleSet(CCRuleSet* pRuleSet, INT iIndex = -1);
	BOOL				bDownRuleSet(CCRuleSet* pRuleSet, INT iIndex = -1);

	BOOL				bReplaceMessage(CCRule*	pRule);
	BOOL				bReplaceKeyActionParams(CCRule* pRule /*, CString& strEventServer, CString& strEventIdentity, CString& strEventChannel, CString& strEventMessage*/);
	BOOL				bReplaceKeyEventParams(CString& strEventParam);
	BOOL				bMatchAndApplyRules(enumEvents eID, enumActions* paApprovedIDs, enumActions* paRejectedIDs, CString& strServer, CString& strIdentity, CString& strChannel, CString& strMessage);
	BOOL				bInActionIDs(enumActions* paActions, enumActions aID);
	INT					iGetFirstMatchingRule(PINT piRuleSet, enumEvents eID, enumActions* paApprovedIDs, enumActions* paRejectedIDs, CString& strServer, CString& strIdentity, CString& strChannel, CString& strMessage, CCRule** ppRule = NULL);
	INT					iGetNextMatchingRule(PINT piPreviousRuleSet, INT iPreviousRule, CCRule** ppRule = NULL);

	BOOL				bSaveRulesToReg(/*BOOL bToHKCU = TRUE*/);
	BOOL				bLoadRulesFromReg(/*BOOL bFromHKCU = TRUE*/);
	BOOL				bLoadRulesFromResource();

	BOOL				bUpdateRuleSetsDaemonExt(BOOL bResetUserLists);
	BOOL				bDaemonNeeded();
	BOOL				bStartRulesDaemon(UINT uRulesDaemonElapse, BOOL bForceReset);
	BOOL				bStopRulesDaemon();
	void				OnRulesDaemonTimer();

protected:
	BOOL				bRuleFilteredOut(CCRule* pRule);
	BOOL				bMatchingRule(CCRule* pRule);
	void				CleanUpRuleSetsArray();

	CCRulesData*		m_pRulesData;
	CCDelayedRules*		m_pDelayedRules;
	WORD				m_wFlags;
	CPtrArray			m_rgpRuleSets;
	CCRuleSet*			m_pSelectedRuleSet;
	enumEvents			m_eIDCach;
	enumActions*		m_paApprovedIDsCach;
	enumActions*		m_paRejectedIDsCach;
	CString				m_strIdentityCach;
	CString				m_strServerCach;
	CString				m_strChannelCach;
	CString				m_strRecipientsCach;
	CString				m_strCFMessageCach;
	CString				m_strCLMessageCach;
	CDWordArray*		m_prgdwMsgFormattingCach;

	CString				m_strCFFinalMessage;

	EVENT_KEY_PARAM_FN	m_pfEventKeyParam;
	EVENT_RND_PARAM_FN	m_pfEventRndParam;
	GET_EVENT_KEY_FN	m_pfGetKeyEventParam;
	GET_ACTION_KEY_FN	m_pfGetKeyActionParam;
	EXECUTE_ACTION_FN	m_pfExecuteAction;
	RULE_FAILURE_FN		m_pfRuleFailure;
	RULEDAEMON_QUERY_FN	m_pfDaemonQuery;

	BOOL				m_bDaemonRunning;
	UCHAR				m_uFloodInterval;
	UCHAR				m_uFloodOccurrences;
};


class CCDelayedRules
{
public:
	CCDelayedRules();
	~CCDelayedRules();

	void				SetExecuteActionFunction(EXECUTE_ACTION_FN pfExecuteAction)
							{ m_pfExecuteAction = pfExecuteAction; }

	BOOL				bAddActionCtx(CCActionContext* pActionCtx);
	BOOL				bExecuteActions();
	BOOL				bStartTimer();
	BOOL				bStopTimer();
	void				FreeRemoveAll();

protected:
	EXECUTE_ACTION_FN	m_pfExecuteAction;
	BOOL				m_bTimerRunning;
	CPtrList			m_plActionCtx;
};


#ifdef DEBUG
extern SHORT g_nRulesRefCount;
extern SHORT g_nDaemonsRefCount;
#endif

#define __RULES_H__
#endif __RULES_H__

