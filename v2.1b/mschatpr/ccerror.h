/*
	*****************************************************************
	*																*
	*	Header 			: CCError.H, Chat Client Error Codes		*
	*																*
	*	Author 			: RegisB, 12/17/96							*
	*																*
	*	Current Owner	: RegisB									*
	*																*
	*****************************************************************
*/

#ifndef __CCERROR_H__
#define __CCERROR_H__

#define CHATCLIENT_ERROR(e)					MAKE_SCODE(SEVERITY_ERROR, FACILITY_ITF, 0x3000 + e)
#define CHATCLIENT_SUCCESS(s)				MAKE_SCODE(SEVERITY_SUCCESS, FACILITY_NULL, 0x3000 + s)

#ifdef __cplusplus

const HRESULT	CC_S_CONNECTCANCELLED		= CHATCLIENT_SUCCESS(0x0001);
const HRESULT	CC_S_LOGINCANCELLED			= CHATCLIENT_SUCCESS(0x0002);
const HRESULT	CC_S_CREATEJOINCANCELLED	= CHATCLIENT_SUCCESS(0x0003);

//
// Internal Errors
//
const HRESULT	CC_E_BUFFERTOOSMALL			= CHATCLIENT_ERROR(0x0001);
const HRESULT	CC_E_NOTQUERIEDOBJECT		= CHATCLIENT_ERROR(0x0002);

//
// Connection errors
//
const HRESULT	CC_E_ALREADYCONNECTING		= CHATCLIENT_ERROR(0x0020);
const HRESULT	CC_E_ALREADYCONNECTED		= CHATCLIENT_ERROR(0x0021);
const HRESULT	CC_E_ALREADYDISCONNECTING	= CHATCLIENT_ERROR(0x0022);
const HRESULT	CC_E_NOOPINPROGRESS			= CHATCLIENT_ERROR(0x0023);
const HRESULT	CC_E_DISCONNECTFIRST		= CHATCLIENT_ERROR(0x0024);
const HRESULT	CC_E_NOTCONNECTED			= CHATCLIENT_ERROR(0x0025);	// online operation attempted on an off-line socket
const HRESULT	CC_E_HOSTNOTFOUND			= CHATCLIENT_ERROR(0x0026);	// server not found by DNS
const HRESULT	CC_E_SOCKETCREATE			= CHATCLIENT_ERROR(0x0027);	// WinSock failure
const HRESULT	CC_E_CANTCONNECT			= CHATCLIENT_ERROR(0x0028);	// can't connect..server down?
const HRESULT	CC_E_NOSOCKETDATA			= CHATCLIENT_ERROR(0x0029);	
const HRESULT	CC_E_SOCKETERROR			= CHATCLIENT_ERROR(0x002a);	// generic WinSock error
const HRESULT	CC_E_LOSTCONNECTION			= CHATCLIENT_ERROR(0x002b);	// connection lost
const HRESULT	CC_E_SOCKETCLOSED			= CHATCLIENT_ERROR(0x002c);	// somebody closed the socket
const HRESULT	CC_E_NETWORKDOWN			= CHATCLIENT_ERROR(0x002d);	
const HRESULT	CC_E_HOSTDROPPEDCONNECTION	= CHATCLIENT_ERROR(0x002e);	// the host server dropped your connection
//
// Login errors
//
const HRESULT	CC_E_ALREADYLOGGING			= CHATCLIENT_ERROR(0x0030);
const HRESULT	CC_E_ALREADYLOGGED			= CHATCLIENT_ERROR(0x0031);
const HRESULT	CC_E_NOTLOGGED				= CHATCLIENT_ERROR(0x0032);
const HRESULT	CC_E_ALREADYREGISTERED		= CHATCLIENT_ERROR(0x0033);
const HRESULT	CC_E_NOTREGISTERED			= CHATCLIENT_ERROR(0x0034);
const HRESULT	CC_E_LOGINFAILED			= CHATCLIENT_ERROR(0x0035);
const HRESULT	CC_E_AUTHFAILED				= CHATCLIENT_ERROR(0x0036);
const HRESULT	CC_E_AUTHNOTAVAIL			= CHATCLIENT_ERROR(0x0037);	// SSPI authentication wasn't available.
const HRESULT	CC_E_AUTHENTICATEDONLY		= CHATCLIENT_ERROR(0x0038); // This server only allows authenticated logins
//
// Channel Errors
//
const HRESULT	CC_E_ALREADYOPEN			= CHATCLIENT_ERROR(0x0040);
const HRESULT	CC_E_ALREADYOPENING			= CHATCLIENT_ERROR(0x0041);
const HRESULT	CC_E_NOTOPEN				= CHATCLIENT_ERROR(0x0042);
const HRESULT	CC_E_OPENNOTUNIQUE			= CHATCLIENT_ERROR(0x0043);
const HRESULT	CC_E_CHANNELBADNAME			= CHATCLIENT_ERROR(0x0044);
const HRESULT	CC_E_CHANNELBADKEY			= CHATCLIENT_ERROR(0x0045);	// bad password on the channel
const HRESULT	CC_E_CHANNELBADMODES		= CHATCLIENT_ERROR(0x0046);	// bad password on the channel
const HRESULT	CC_E_NOTONCHANNEL			= CHATCLIENT_ERROR(0x0047);
const HRESULT	CC_E_NOSUCHCHANNEL			= CHATCLIENT_ERROR(0x0048);	// channel does not exist
const HRESULT	CC_E_CHANNELFULL			= CHATCLIENT_ERROR(0x0049);	// room is full
const HRESULT	CC_E_ALREADYONCHANNEL		= CHATCLIENT_ERROR(0x004a);	// you are joining a channel you are already on
const HRESULT	CC_E_TOOMANYCHANNELS		= CHATCLIENT_ERROR(0x004b);	// server has a limit on # of channels
																		// a user can be in at at time
const HRESULT	CC_E_INVITEONLYCHANNEL		= CHATCLIENT_ERROR(0x004c);	// you were not invited to this channel
const HRESULT	CC_E_CHANNELKEYALREADYSET	= CHATCLIENT_ERROR(0x004d);

//
// Property Errors
//
const HRESULT	CC_E_BADPROPERTY			= CHATCLIENT_ERROR(0x0050);
const HRESULT	CC_E_BADVALUE				= CHATCLIENT_ERROR(0x0051);
const HRESULT	CC_E_PROPNOTREADABLE		= CHATCLIENT_ERROR(0x0052);
const HRESULT	CC_E_PROPREADONLY			= CHATCLIENT_ERROR(0x0053);
const HRESULT	CC_E_PROPNOTWRITEABLE		= CHATCLIENT_ERROR(0x0054);
//
// Member Errors
// 
const HRESULT	CC_E_NOSUCHMEMBER			= CHATCLIENT_ERROR(0x0058);
const HRESULT	CC_E_MEMBERBADMODES			= CHATCLIENT_ERROR(0x0059);
//
// User Errors
//
const HRESULT	CC_E_BADNICKNAME			= CHATCLIENT_ERROR(0x0060);	// bad nick name - illegal chars
const HRESULT	CC_E_BADPASSWORD			= CHATCLIENT_ERROR(0x0061);	// invalid or wrong password
const HRESULT	CC_E_NICKNAMEINUSE			= CHATCLIENT_ERROR(0x0062);	// somebody else is already using this alias
const HRESULT	CC_E_NICKCOLLISION			= CHATCLIENT_ERROR(0x0063);
const HRESULT	CC_E_NOSUCHNICK				= CHATCLIENT_ERROR(0x0064);
const HRESULT	CC_E_USERBADMODES			= CHATCLIENT_ERROR(0x0065);
const HRESULT	CC_E_NICKTOOFAST			= CHATCLIENT_ERROR(0x0066);
const HRESULT	CC_E_NICKNOCHANGE			= CHATCLIENT_ERROR(0x0067);

//
// Sending
//
const HRESULT	CC_E_TOOMUCHDATA			= CHATCLIENT_ERROR(0x0070);	// data size exceeds max buffer bounds
const HRESULT	CC_E_CANTSEND				= CHATCLIENT_ERROR(0x0071);	// trouble sending buffer.could be client or server
const HRESULT	CC_E_INVALIDRECIPIENTLIST	= CHATCLIENT_ERROR(0x0072);	// 
const HRESULT	CC_E_NULLMSG				= CHATCLIENT_ERROR(0x0073);
const HRESULT	CC_E_BADMSGTYPE				= CHATCLIENT_ERROR(0x0074);
const HRESULT	CC_E_NOWHISPER				= CHATCLIENT_ERROR(0x0075);
const HRESULT	CC_E_BADTAG					= CHATCLIENT_ERROR(0x0076);

//
// Protocol Errors
//
const HRESULT	CC_E_SERVER					= CHATCLIENT_ERROR(0x0080);	// general server error
const HRESULT	CC_E_NOTIRCX				= CHATCLIENT_ERROR(0x0081);	// NOT an IRCX function OR if a IRCX operation
const HRESULT	CC_E_PROPMODE				= CHATCLIENT_ERROR(0x0082);
const HRESULT	CC_E_NOMOTD					= CHATCLIENT_ERROR(0x0083);
const HRESULT	CC_E_UNKNOWNCOMMAND			= CHATCLIENT_ERROR(0x0084);

//
// Access Rights errors
//
const HRESULT	CC_E_NOTSYSOP				= CHATCLIENT_ERROR(0x0090);
const HRESULT	CC_E_NOTOWNER				= CHATCLIENT_ERROR(0x0091);	// you need owner privileges for this operation
const HRESULT	CC_E_NOTHOST				= CHATCLIENT_ERROR(0x0092);	// you need host privileges for this operation
const HRESULT	CC_E_RATINGBLOCK			= CHATCLIENT_ERROR(0x0093);	// PICS rating blocks content access
const HRESULT	CC_E_BANNEDFROMCHANNEL		= CHATCLIENT_ERROR(0x0094);	// you are banned from this channel
const HRESULT	CC_E_BANNEDFROMSERVER		= CHATCLIENT_ERROR(0x0095);	// you are banned from this server
const HRESULT	CC_E_WILLBEBANNED			= CHATCLIENT_ERROR(0x0096);	// you will be banned
const HRESULT	CC_E_PERMISSIONDENIED		= CHATCLIENT_ERROR(0x0097);	// you don't have the right privileges to perform this command
const HRESULT	CC_E_CANTIGNORE				= CHATCLIENT_ERROR(0x0098);	// you can't ignore a SysOp or Host
const HRESULT	CC_E_NOJOINMICONLY			= CHATCLIENT_ERROR(0x0099);	// you can't join a MIC only channel with a IRCX client
const HRESULT	CC_E_NOJOINREMOTE			= CHATCLIENT_ERROR(0x009a);	// you can't join this channel from a remote server
const HRESULT	CC_E_NOJOINDYNAMIC			= CHATCLIENT_ERROR(0x009b);	// you can't join dynamic channels due to admin restriction
const HRESULT	CC_E_NODYNAMICCHANNELS		= CHATCLIENT_ERROR(0x009c);	// you can't create dynamic channels due to admin restriction
const HRESULT	CC_E_AUTHONLY				= CHATCLIENT_ERROR(0x009d);	// only authenticated users may join this channel
const HRESULT	CC_E_OVERFLOWABORT			= CHATCLIENT_ERROR(0x009e);	// command is aborted to prevent output buffer overflow
const HRESULT	CC_E_CANTCHANGEUSERMODE		= CHATCLIENT_ERROR(0x009f);	// user could not change other user's mode

//
// ChatItems related errors
//
const HRESULT	CC_E_ITEMNOTAVAILABLE		= CHATCLIENT_ERROR(0x0100);
const HRESULT	CC_E_ITEMNAMENA				= CHATCLIENT_ERROR(0x0101);
const HRESULT	CC_E_INVALIDITEMNAME		= CHATCLIENT_ERROR(0x0102);
const HRESULT	CC_E_INVALIDITEMVALUE		= CHATCLIENT_ERROR(0x0103);
const HRESULT	CC_E_NOTOPERATOR			= CHATCLIENT_ERROR(0x0104);	// not a valid operator
const HRESULT	CC_E_TIMEOUT				= CHATCLIENT_ERROR(0x0105);	// generic timeout

#else																	  

#define	CC_E_ZORG							CHATCLIENT_ERROR(0x0001)

#define CC_S_CONNECTCANCELLED				CHATCLIENT_SUCCESS(0x0001)
#define CC_S_LOGINCANCELLED					CHATCLIENT_SUCCESS(0x0002)
#define CC_S_CREATEJOINCANCELLED			CHATCLIENT_SUCCESS(0x0003)

#endif

#endif // __CCERROR_H__
