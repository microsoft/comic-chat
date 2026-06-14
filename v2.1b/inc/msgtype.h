#ifndef __MSGTYPE_H__

// Message type
typedef enum
{
	mtNormal		= 0,	// regular broadcast
	mtWhisper		= 1,	// whisper
	mtThought		= 2,	// ComicChat thought
	mtBroadcast		= 3,
	mtAction		= 4,	// IRC or ComicChat action
	mtPrivate,				// private message
	mtReply,				// reply to a private message

	mtJoin,					// member has joined the conversation
	mtLeave,				// member has left the conversation
	mtAppearsAs,			// ComicChat # Appears as message
	mtBackground,			// ComicChat # Background message
	mtChr,					// ComicChat (#<anything>) Chr message

	mtGetInfo,				// ComicChat # GetInfo message
	mtStatusChange,			// Member Status Change
	mtAliasChange,			// Member Alias Changed
	mtTopicChange,			// Room Topic Changed
	mtKicked,				// Member got kicked
	mtGetRealname,			// Reply of GetRealName for a member
	mtEndEnum				// The last enumeration, which gives us the count
} MSG_TYPE;

const MSG_TYPE mtBeginActions	= mtJoin;		// For use in text view so that 
												// header structures need not be allocated
const MSG_TYPE mtBeginInfo		= mtGetInfo;	// For use in displaying only the info

const MSG_TYPE mtURL			= mtEndEnum;	// For URL properties.


// Message from
typedef enum
{
	msHost,				// Host in the chat room
	msParticipant,		// Participant in the chat room
	msSpectator,		// Spectator in the chat room
	msRoom,				// From chat room
	msEndEnum			// The last enumeration, which gives us the count
} MEMBER_STATUS;


// Member Access Codes
const short g_nHost						= 0x01;			// used to change a member into a host
const short g_nParticipant				= 0x02;			// used to change a member into a participant
const short g_nSpectator				= 0x04;			// used to change a member into a spectator

const short g_nNoWhisper				= 0x08;			// used to accept/refuse whispers
const short g_nIgnored					= 0x10;			// used to ignore or not a member

const short g_nJoining					= 0x20;			// member is joining the channel
const short g_nLeaving					= 0x40;			// member is leaving the channel

const short g_nMe						= 0x80;			// member is me

const short g_nStatMin					= g_nHost;
const short g_nStatMax					= (g_nMe+g_nLeaving+g_nIgnored+g_nNoWhisper+g_nSpectator);


#define __MSGTYPE_H__
#endif __MSGTYPE_H__
