//--------------------------------------------------------------------------------------------
//
//	Copyright (c) Microsoft Corporation, 1996-1997.  All Rights Reserved.
//
//	Description:
//
//		Microsoft Internet ChatSockets.
//		ChatSocket Error and Status codes
//
//--------------------------------------------------------------------------------------------

#ifndef __CSERROR__
#define __CSERROR__

//--------------------------------------------------------------------------------------------
//
// ERRORS
//
//--------------------------------------------------------------------------------------------
//
// The Following Standard Win32 Errors are also returned
//
/*

	NOERROR

	E_NOTIMPL

	E_UNEXPECTED

	E_OUTOFMEMORY

	E_INVALIDARG

	E_FAIL

	S_FALSE

*/

#define CHATSOCK_ERROR(e)					  MAKE_SCODE(SEVERITY_ERROR, FACILITY_ITF, 0x6000 + e)

#ifdef __cplusplus

const HRESULT	CS_E_SYSTEM					= CHATSOCK_ERROR(0x0001);	// system error

//
// Generic errors
//

const HRESULT	CS_E_VERSION				= CHATSOCK_ERROR(0x0002);	// wrong version of chatsock
const HRESULT	CS_E_WAIT					= CHATSOCK_ERROR(0x0003);	// wait error
const HRESULT	CS_E_EVENT					= CHATSOCK_ERROR(0x0004);	// event error
const HRESULT	CS_E_SZTOOLONG				= CHATSOCK_ERROR(0x0005);	// string too long
const HRESULT	CS_E_EXITING				= CHATSOCK_ERROR(0x0006);	// dll in process of exiting
const HRESULT	CS_E_NOTANSI				= CHATSOCK_ERROR(0x0007);	// ANSI used instead of Unicode!
const HRESULT	CS_E_NOTUNICODE				= CHATSOCK_ERROR(0x0008);	// Unicode used instead of Ansi!
const HRESULT	CS_E_TOOMUCHDATA			= CHATSOCK_ERROR(0x0009);	// data size exceeds max buffer bounds
const HRESULT	CS_E_ILLEGAL_CHARS			= CHATSOCK_ERROR(0x000a);	// illegal chars in string
const HRESULT	CS_E_FIRSTCHAR				= CHATSOCK_ERROR(0x000b);	// first char error in IRC
//
// List management errors
//
const HRESULT	CS_E_ALREADYINLIST			= CHATSOCK_ERROR(0x000c);	// inserted element is already in list
const HRESULT	CS_E_NOTINLIST				= CHATSOCK_ERROR(0x000d);	// element not found
const HRESULT	CS_E_QUEEMPTY				= CHATSOCK_ERROR(0x000e);	// no msgs in wait q.
//
// Connection errors
//
const HRESULT	CS_E_NOTCONNECTED			= CHATSOCK_ERROR(0x000f);	// online operation attempted on an off-line socket
const HRESULT	CS_E_WINSOCKDLL				= CHATSOCK_ERROR(0x0010);	// could not load WinSock.dll
const HRESULT	CS_E_HOSTNOTFOUND			= CHATSOCK_ERROR(0x0011);	// server not found by DNS
const HRESULT	CS_E_SOCKETCREATE			= CHATSOCK_ERROR(0x0012);	// WinSock failure
const HRESULT	CS_E_CANTCONNECT			= CHATSOCK_ERROR(0x0013);	// can't connect..server down?
const HRESULT	CS_E_CANTSEND				= CHATSOCK_ERROR(0x0014);	// trouble sending buffer.could be client or server
const HRESULT	CS_E_TIMEOUT				= CHATSOCK_ERROR(0x0015);	// generic timeout
const HRESULT	CS_E_NODATA					= CHATSOCK_ERROR(0x0016);	
const HRESULT	CS_E_SOCKETERROR			= CHATSOCK_ERROR(0x0017);	// generic WinSock error
const HRESULT	CS_E_INVALIDSOCKET			= CHATSOCK_ERROR(0x0018);	// operation attempted on bad ICSSocket
const HRESULT	CS_E_LOSTCONNECTION			= CHATSOCK_ERROR(0x0019);	// connection lost
const HRESULT	CS_E_SOCKETCLOSED			= CHATSOCK_ERROR(0x001a);	// somebody closed the socket
const HRESULT	CS_E_NETWORKDOWN			= CHATSOCK_ERROR(0x001b);	
const HRESULT	CS_E_HOSTDROPPEDCONNECTION	= CHATSOCK_ERROR(0x001c);	// the host server dropped your connection
//
// Login errors
//
const HRESULT	CS_E_NOTLOGGEDIN			= CHATSOCK_ERROR(0x001d);	// chat operation attempted without logging into server
const HRESULT	CS_E_UNKNOWNUSER			= CHATSOCK_ERROR(0x001e);	// server does not know you
const HRESULT	CS_E_ALIASINUSE				= CHATSOCK_ERROR(0x001f);	// somebody else is already using this alias
const HRESULT	CS_E_ILLEGALUSER			= CHATSOCK_ERROR(0x0020);	// server does not like authenticating you
//
// Channel Errors
//
const HRESULT	CS_E_CHANNELCANCEL			= CHATSOCK_ERROR(0x0021);
const HRESULT	CS_E_CREATEFAIL				= CHATSOCK_ERROR(0x0022);	// channel create failed
const HRESULT	CS_E_JOINFAIL				= CHATSOCK_ERROR(0x0023);	// channel join failed
const HRESULT	CS_E_CANCELFAIL				= CHATSOCK_ERROR(0x0024);
const HRESULT	CS_E_CHANNELEXISTS			= CHATSOCK_ERROR(0x0025);	// channel already exists
const HRESULT	CS_E_CHANNELNOTFOUND		= CHATSOCK_ERROR(0x0026);	// attempt to join a bogus channel
const HRESULT	CS_E_CANTMAKEUNIQUECHANNEL	= CHATSOCK_ERROR(0x0027);	
const HRESULT	CS_E_CHANNELFULL			= CHATSOCK_ERROR(0x0028);	// room is full
const HRESULT	CS_E_ALREADYONCHANNEL		= CHATSOCK_ERROR(0x0029);	// you are joining a channel you are already on
const HRESULT	CS_E_CLOSE					= CHATSOCK_ERROR(0x002a); 	// error closing a channel
const HRESULT	CS_E_NOTINCHANNEL			= CHATSOCK_ERROR(0x002b);	// operation attempted on a channel you are not on
const HRESULT	CS_E_TOOMANYCHANNELS		= CHATSOCK_ERROR(0x002c); 	// server has a limit on # of channels
																		// a user can be in at at time
const HRESULT	CS_E_INVITEONLYCHANNEL		= CHATSOCK_ERROR(0x002d);	// you were not invited to this channel
const HRESULT	CS_E_CHANNELBADPASS			= CHATSOCK_ERROR(0x002e);	// bad password on the channel
//
// Protocol Errors
//
const HRESULT	CS_E_BYTECOUNT				= CHATSOCK_ERROR(0x002f);	// header byte counts are off. Usually a hostile or outdated server
const HRESULT	CS_E_SERVER					= CHATSOCK_ERROR(0x0030);	// general server error
const HRESULT	CS_E_SECURITY				= CHATSOCK_ERROR(0x0031);	// server security error
const HRESULT	CS_E_SERVICE				= CHATSOCK_ERROR(0x0032);	// error in channel service
const HRESULT	CS_E_NOTMIC					= CHATSOCK_ERROR(0x0033);	// NOT a MIC function OR if a MIC operation
																		// was attempted on an IRC channel etc
//
// Access Rights errors
//
const HRESULT	CS_E_CALLERNOTHOST			= CHATSOCK_ERROR(0x0034);	// you need host priviledges for this operation
const HRESULT	CS_E_ISHOST					= CHATSOCK_ERROR(0x0035);	// 
const HRESULT	CS_E_NOTYOU					= CHATSOCK_ERROR(0x0036);	// if you try to set a mode on some other user
																		// .. and this is a mode you can only set on yourself
const HRESULT	CS_E_NOTSPEAKER				= CHATSOCK_ERROR(0x0037);	// a spectator tried to speak
const HRESULT	CS_E_UNICODENOTALLOWED		= CHATSOCK_ERROR(0x0038);	// no Unicode allowed on this protocol
//
// Property/SetMode errors
//
const HRESULT	CS_E_PROPMODE				= CHATSOCK_ERROR(0x0039);
const HRESULT	CS_E_PROPERTY				= CHATSOCK_ERROR(0x003a);
const HRESULT	CS_E_NOMOREPROP				= CHATSOCK_ERROR(0x003b);
const HRESULT	CS_E_TOOMANYPROP			= CHATSOCK_ERROR(0x003c);	// HrGetProperty()..max exceeded..
const HRESULT	CS_E_TOOMANYTERMS			= CHATSOCK_ERROR(0x003d); 	// Queries have limits on the # of terms
																		// in them
const HRESULT	CS_E_NOTCHANNELPROP			= CHATSOCK_ERROR(0x003e);	// not a channel property
const HRESULT	CS_E_NOTMEMBERPROP			= CHATSOCK_ERROR(0x003f);	// not a member property 
const HRESULT	CS_E_NOTOPERATOR			= CHATSOCK_ERROR(0x0040);	// not a valid operator

const HRESULT	CS_E_PROPLOOKUP				= CHATSOCK_ERROR(0x0041);	// the server failed to look properties up	
const HRESULT	CS_E_NOMATCHES				= CHATSOCK_ERROR(0x0042);	// no matches on query

const HRESULT	CS_E_BANNED					= CHATSOCK_ERROR(0x0043);	// you be banned from server/channel

//
// Auth errors
//
const HRESULT	CS_E_AUTHNOTAVAIL			= CHATSOCK_ERROR(0x0044);	// SSPI authentication wasn't available.
const HRESULT	CS_E_INVALIDPASSWORD		= CHATSOCK_ERROR(0x0045);	// user gave invalid password

const HRESULT	CS_E_NOWHISPER				= CHATSOCK_ERROR(0x0046);	// whispers not allowed
const HRESULT	CS_E_BADCHANNELNAME			= CHATSOCK_ERROR(0x0047);
const HRESULT	CS_E_NICKCOLLISION			= CHATSOCK_ERROR(0x0048);
const HRESULT	CS_E_NOTMODERATED			= CHATSOCK_ERROR(0x0049);	//IRC does not permit some operations
																		// on non-moderated channels
const HRESULT	CS_E_INVALIDRECIPIENTLIST	= CHATSOCK_ERROR(0x004a);	// IRC
const HRESULT	CS_E_NOSUCHNICK				= CHATSOCK_ERROR(0x004b);	// you sent a msg to somebody who the server does not know
const HRESULT	CS_E_ALREADYLOGGEDIN		= CHATSOCK_ERROR(0x004c);	// already logged into this server
const HRESULT	CS_E_BADCHANNELGUID			= CHATSOCK_ERROR(0x004d);	// bad channel service guid
const HRESULT	CS_E_BADNICKNAME			= CHATSOCK_ERROR(0x004e);	// bad nick name - illegal chars
const HRESULT	CS_E_BADUSERNAME			= CHATSOCK_ERROR(0x0050);	// bad user name - illegal chars

const HRESULT	CS_E_IRCLIENTSNOTALLOWED	= CHATSOCK_ERROR(0x0051);  // server does not allow IRC clients

const HRESULT	CS_E_NOTIRC					= CHATSOCK_ERROR(0x0052);
const HRESULT	CS_E_UNKNOWNSECURITYPACKAGE = CHATSOCK_ERROR(0x0053); // Tried to log in with unknown (server) security package
const HRESULT	CS_E_AUTHENTICATEDONLY		= CHATSOCK_ERROR(0x0054); // This server only allows authenticated logins
const HRESULT	CS_E_SERVERISFULL			= CHATSOCK_ERROR(0x0055); // server not letting any more users on

const HRESULT	CS_E_BROADCASTNOTAVAIL		= CHATSOCK_ERROR(0x0056); // broadcasting is currently not available
const HRESULT	CS_E_NOTSYSOP				= CHATSOCK_ERROR(0x0057);

const HRESULT	CS_E_AWAY					= CHATSOCK_ERROR(0x0058);
const HRESULT	CS_E_ADMINRESTRICTED		= CHATSOCK_ERROR(0x0059);
const HRESULT	CS_E_PROTOCOLVERSION		= CHATSOCK_ERROR(0x005a);
const HRESULT	CS_E_FLOODING				= CHATSOCK_ERROR(0x005b);

#else																	  

#define	CS_E_SYSTEM					CHATSOCK_ERROR(0x0001)	// system error

//
// Generic errors
//

#define	CS_E_VERSION				CHATSOCK_ERROR(0x0002)	// wrong version of chatsock
#define CS_E_WAIT					CHATSOCK_ERROR(0x0003)	// wait error
#define CS_E_EVENT					CHATSOCK_ERROR(0x0004)	// event error
#define CS_E_SZTOOLONG				CHATSOCK_ERROR(0x0005)	// string too long
#define CS_E_EXITING				CHATSOCK_ERROR(0x0006)	// dll in process of exiting
#define CS_E_NOTANSI				CHATSOCK_ERROR(0x0007)	// ANSI used instead of Unicode!
#define CS_E_NOTUNICODE				CHATSOCK_ERROR(0x0008)	// Unicode used instead of Ansi!
#define CS_E_TOOMUCHDATA			CHATSOCK_ERROR(0x0009)	// data size exceeds max buffer bounds
#define CS_E_ILLEGAL_CHARS			CHATSOCK_ERROR(0x000a)	// illegal chars in string
#define CS_E_FIRSTCHAR				CHATSOCK_ERROR(0x000b)	// first char error in IRC
//
// List management errors
//
#define CS_E_ALREADYINLIST			CHATSOCK_ERROR(0x000c)	// inserted element is already in list
#define CS_E_NOTINLIST				CHATSOCK_ERROR(0x000d)	// element not found
#define CS_E_QUEEMPTY				CHATSOCK_ERROR(0x000e)	// no msgs in wait q.
//
// Connection errors
//
#define CS_E_NOTCONNECTED			CHATSOCK_ERROR(0x000f)	// online operation attempted on an off-line socket
#define CS_E_WINSOCKDLL				CHATSOCK_ERROR(0x0010)	// could not load WinSock.dll
#define CS_E_HOSTNOTFOUND			CHATSOCK_ERROR(0x0011)	// server not found by DNS
#define CS_E_SOCKETCREATE			CHATSOCK_ERROR(0x0012)	// WinSock failure
#define CS_E_CANTCONNECT			CHATSOCK_ERROR(0x0013)	// can't connect..server down?
#define CS_E_CANTSEND				CHATSOCK_ERROR(0x0014)	// trouble sending buffer.. usually a WinSock problem
#define CS_E_TIMEOUT				CHATSOCK_ERROR(0x0015)	// generic timeout
#define CS_E_NODATA					CHATSOCK_ERROR(0x0016)	
#define CS_E_SOCKETERROR			CHATSOCK_ERROR(0x0017)	// generic WinSock error
#define CS_E_INVALIDSOCKET			CHATSOCK_ERROR(0x0018)	// operation attempted on bad ICSSocket
#define CS_E_LOSTCONNECTION			CHATSOCK_ERROR(0x0019)	// connection lost
#define CS_E_SOCKETCLOSED			CHATSOCK_ERROR(0x001a)	// somebody closed the socket
#define CS_E_NETWORKDOWN			CHATSOCK_ERROR(0x001b)	
#define CS_E_HOSTDROPPEDCONNECTION	CHATSOCK_ERROR(0x001c)	// the host server dropped your connection
//
// Login errors
//
#define CS_E_NOTLOGGEDIN			CHATSOCK_ERROR(0x001d)	// chat operation attempted without logging into server
#define CS_E_UNKNOWNUSER			CHATSOCK_ERROR(0x001e)	// server does not know you
#define CS_E_ALIASINUSE				CHATSOCK_ERROR(0x001f)	// somebody else is already using this alias
#define CS_E_ILLEGALUSER			CHATSOCK_ERROR(0x0020)	// server does not like authenticating you
//
// Channel Errors
//
#define CS_E_CHANNELCANCEL			CHATSOCK_ERROR(0x0021)
#define CS_E_CREATEFAIL				CHATSOCK_ERROR(0x0022)	// channel create failed
#define CS_E_JOINFAIL				CHATSOCK_ERROR(0x0023)	// channel join failed
#define CS_E_CANCELFAIL				CHATSOCK_ERROR(0x0024)
#define CS_E_CHANNELEXISTS			CHATSOCK_ERROR(0x0025)	// channel already exists
#define CS_E_CHANNELNOTFOUND		CHATSOCK_ERROR(0x0026)	// attempt to join a bogus channel
#define CS_E_CANTMAKEUNIQUECHANNEL	CHATSOCK_ERROR(0x0027)	
#define CS_E_CHANNELFULL			CHATSOCK_ERROR(0x0028)	// room is full
#define CS_E_ALREADYONCHANNEL		CHATSOCK_ERROR(0x0029)	// you are joining a channel you are already on
#define CS_E_CLOSE					CHATSOCK_ERROR(0x002a) 	// error closing a channel
#define CS_E_NOTINCHANNEL			CHATSOCK_ERROR(0x002b)	// operation attempted on a channel you are not on
#define CS_E_TOOMANYCHANNELS		CHATSOCK_ERROR(0x002c) 	// server has a limit on # of channels
																		// a user can be in at at time
#define CS_E_INVITEONLYCHANNEL		CHATSOCK_ERROR(0x002d)	// you were not invited to this channel
#define CS_E_CHANNELBADPASS			CHATSOCK_ERROR(0x002e)	// bad password on the channel
//
// Protocol Errors
//
#define CS_E_BYTECOUNT				CHATSOCK_ERROR(0x002f)	// header byte counts are off. Usually a hostile or outdated server
#define CS_E_SERVER					CHATSOCK_ERROR(0x0030)	// general server error
#define CS_E_SECURITY				CHATSOCK_ERROR(0x0031)	// server security error
#define CS_E_SERVICE				CHATSOCK_ERROR(0x0032)	// error in channel service
#define CS_E_NOTMIC					CHATSOCK_ERROR(0x0033)	// NOT a MIC function OR if a MIC operation
																		// was attempted on an IRC channel etc
//
// Access Rights errors
//
#define CS_E_CALLERNOTHOST			CHATSOCK_ERROR(0x0034)	// you need host priviledges for this operation
#define CS_E_ISHOST					CHATSOCK_ERROR(0x0035)	// 
#define CS_E_NOTYOU					CHATSOCK_ERROR(0x0036)	// if you try to set a mode on some other user
																		// .. and this is a mode you can only set on yourself
#define CS_E_NOTSPEAKER				CHATSOCK_ERROR(0x0037)	// a spectator tried to speak
#define CS_E_UNICODENOTALLOWED		CHATSOCK_ERROR(0x0038)	// no Unicode allowed on this protocol
//
// Property/SetMode errors
//
#define CS_E_PROPMODE				CHATSOCK_ERROR(0x0039)
#define CS_E_PROPERTY				CHATSOCK_ERROR(0x003a)
#define CS_E_NOMOREPROP				CHATSOCK_ERROR(0x003b)
#define CS_E_TOOMANYPROP			CHATSOCK_ERROR(0x003c)	// HrGetProperty()..max exceeded..
#define CS_E_TOOMANYTERMS			CHATSOCK_ERROR(0x003d) 	// Queries have limits on the # of terms
																		// in them
#define CS_E_NOTCHANNELPROP			CHATSOCK_ERROR(0x003e)	// not a channel property
#define CS_E_NOTMEMBERPROP			CHATSOCK_ERROR(0x003f)	// not a member property 
#define CS_E_NOTOPERATOR			CHATSOCK_ERROR(0x0040)	// not a valid operator

#define CS_E_PROPLOOKUP				CHATSOCK_ERROR(0x0041)	// the server failed to look properties up	
#define CS_E_NOMATCHES				CHATSOCK_ERROR(0x0042)	// no matches on query

#define CS_E_BANNED					CHATSOCK_ERROR(0x0043)	// you be banned from server/channel

//
// Auth errors
//
#define CS_E_AUTHNOTAVAIL			CHATSOCK_ERROR(0x0044)	// SSPI authentication wasn't available.
#define CS_E_INVALIDPASSWORD		CHATSOCK_ERROR(0x0045)	// user gave invalid password
//
// FIX ME - these errors need to be re-numbered during their next cleanup.
// We don't want to force people to rebuild their clients this close to a beta
//
#define CS_E_NOWHISPER				CHATSOCK_ERROR(0x0046)	// whispers not allowed
#define CS_E_BADCHANNELNAME			CHATSOCK_ERROR(0x0047)
#define CS_E_NICKCOLLISION			CHATSOCK_ERROR(0x0048)
#define CS_E_NOTMODERATED			CHATSOCK_ERROR(0x0049) //IRC does not permit some operations
																	// on non-moderated channels
#define CS_E_INVALIDRECIPIENTLIST	CHATSOCK_ERROR(0x004a)	// IRC
#define CS_E_NOSUCHNICK				CHATSOCK_ERROR(0x004b)	// you sent a msg to somebody who the server does not know
#define CS_E_ALREADYLOGGEDIN		CHATSOCK_ERROR(0x004c)
#define CS_E_BADCHANNELGUID			CHATSOCK_ERROR(0x004d)
#define CS_E_BADNICKNAME			CHATSOCK_ERROR(0x004e)	// bad nick name - illegal chars
#define CS_E_BADUSERNAME			CHATSOCK_ERROR(0x0050)	// bad user name - illegal chars
#define CS_E_IRCLIENTSNOTALLOWED	CHATSOCK_ERROR(0x0051) // server does not allow IRC clients
#define CS_E_NOTIRC					CHATSOCK_ERROR(0x0052)
#define CS_E_UNKNOWNSECURITYPACKAGE CHATSOCK_ERROR(0x0053) // Tried to log in with unknown (server) security package
#define CS_E_AUTHENTICATEDONLY		CHATSOCK_ERROR(0x0054) // This server only allows authenticated logins
#define CS_E_SERVERISFULL			CHATSOCK_ERROR(0x0055)
#define CS_E_BROADCASTNOTAVAIL		CHATSOCK_ERROR(0x0056)

#define CS_E_NOTSYSOP				CHATSOCK_ERROR(0x0057)

#define CS_E_AWAY					CHATSOCK_ERROR(0x0058)
#define CS_E_ADMINRESTRICTED		CHATSOCK_ERROR(0x0059)
#define CS_E_PROTOCOLVERSION		CHATSOCK_ERROR(0x005a)
#define CS_E_FLOODING				CHATSOCK_ERROR(0x005b)

#endif

#endif


