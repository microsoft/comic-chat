//+-------------------------------------------------------------------------------------------
//
//  Copyright (C) Microsoft Corporation, 1996.  All Rights Reserved.
//
//  Project:    Microsoft Internet Chat (MIC)
//
//  Include:    MIC client <--> server protocol (MicMsg.h).
//
//  Notes:      See MicSpec.doc for documentation.
//
//--------------------------------------------------------------------------------------------

#if !defined(__MICMSG_H_)
#define __MICMSG_H_


//--------------------------------------------------------------------------------------------
//
//  MIC messages have a max limit of 2k.
//
//--------------------------------------------------------------------------------------------

const int MIC_MAX_MESSAGE   = 2048;


//--------------------------------------------------------------------------------------------
//
//  Define string message field limits.
//
//--------------------------------------------------------------------------------------------

const int MIC_MAX_COMMENT_LENGTH            =   63;
const int MIC_MAX_DNS_LENGTH                =   63;
const int MIC_MAX_REASON_LENGTH		        =	63;
const int MIC_MAX_STRING_LENGTH             = 1007;

const int MIC_MAX_BAN_NAME_LENGTH           =   63;
const int MIC_MAX_CLASS_NAME_LENGTH         =   63;
const int MIC_MAX_PORTAL_NAME_LENGTH        =   63;
const int MIC_MAX_SERVER_NAME_LENGTH        =   63;
const int MIC_MAX_SERVER_DNS_LENGTH         =   MIC_MAX_DNS_LENGTH;
const int MIC_MAX_SERVER_TITLE_LENGTH       =   63;
const int MIC_MAX_SERVER_ADMIN_LENGTH       =  119;
const int MIC_MAX_SERVER_MOTD_LENGTH        = 1007;
const int MIC_MAX_SERVER_PACKAGE_LENGTH     =   31;
const int MIC_MAX_USER_ALIAS_LENGTH         =   23;
const int MIC_MAX_USER_USERID_LENGTH        =   31;
const int MIC_MAX_USER_PASSWORD_LENGTH      =   31;
const int MIC_MAX_USER_IDENT_LENGTH         =  127;
const int MIC_MAX_USER_NAME_LENGTH          =   63;
const int MIC_MAX_USER_AWAY_LENGTH          =   63;
const int MIC_MIN_CHANNEL_NAME_LENGTH       =    1;
const int MIC_MAX_CHANNEL_NAME_LENGTH       =  200;
const int MIC_MIN_CHANNEL_NAME_LENGTH_MIC   =    1;
const int MIC_MAX_CHANNEL_NAME_LENGTH_MIC   =   63;
const int MIC_MAX_CHANNEL_ALIAS_LENGTH      = MIC_MAX_USER_ALIAS_LENGTH + 2;
const int MIC_MAX_CHANNEL_KEYWORD_LENGTH    =   31;
const int MIC_MAX_CHANNEL_TOPIC_LENGTH      =   95;
const int MIC_MAX_CHANNEL_SUBJECT_LENGTH    =   31;
const int MIC_MAX_CHANNEL_HOSTKEY_LENGTH    =   31;
const int MIC_MAX_CHANNEL_ACCOUNT_LENGTH    =   31;
const int MIC_MAX_CHANNEL_DATA_LENGTH       =  127;
const int MIC_MAX_CHANNEL_PICS_LENGTH       =  255;
const int MIC_MAX_CHANNEL_BANNED_LENGTH     = 1007;
const int MIC_MAX_CHANNEL_TEMPLATES         =   99;


//--------------------------------------------------------------------------------------------
//
//  Define mic message field limits.
//
//--------------------------------------------------------------------------------------------

const int MICMSG_MAX_WHISPERS   =   10; // No more than 10 whispers in micSendWhisper.
const int MICMSG_MAX_TERMS      =   15; // No more than 15 terms in a query.
const int MICMSG_MAX_PROPS      =   15; // No more than 15 properties in a mic message.
const int MICMSG_MAX_MSGBYTES   = 2000; // 2000 byte limit for SendMsg text.


//--------------------------------------------------------------------------------------------
//
//  Define other limits.
//
//--------------------------------------------------------------------------------------------

const int MIC_MAX_CHANNELS				= 10;
const int MIC_MIN_NSID					= 1;
const int MIC_MAX_NSID					= 255;
const int MIC_MIN_PING_DELAY			= 15;		// 15 seconds.
const int MIC_MAX_PING_DELAY			= 60*60;	// 1 hour.
const int MIC_MIN_INPUT_FLOOD			= 256;		// 256 bytes. 
const int MIC_MAX_INPUT_FLOOD			= 4096; 	// 4096 bytes. 
const int MIC_MIN_OUTPUT_SATURATION		= 4096;		// 4096 bytes.
const int MIC_MAX_OUTPUT_SATURATION		= 256*1024; // 256k bytes. 


//--------------------------------------------------------------------------------------------
//
//  Define class defines.
//
//--------------------------------------------------------------------------------------------

const int MIC_CLASS_DNS_DISABLE			= 0;
const int MIC_CLASS_DNS_ATTEMPT			= 1;
const int MIC_CLASS_DNS_REQUIRED		= 2;


//--------------------------------------------------------------------------------------------
//
//  Define class defaults.
//
//--------------------------------------------------------------------------------------------

const int MIC_CLASS_DNS_DEFAULT			= MIC_CLASS_DNS_DISABLE;
const int MIC_CLASS_CHANNELS_DEFAULT    = 10;
const int MIC_CLASS_PING_DEFAULT        = 90;
const int MIC_CLASS_INPUT_DEFAULT       = 1024;
const int MIC_CLASS_OUTPUT_DEFAULT      = 64 * 1024;


//--------------------------------------------------------------------------------------------
//
//  Define channel service limits.
//
//--------------------------------------------------------------------------------------------

const int MIC_MAX_SERVICE_PATH_LENGTH	= 260;
const int MIC_MAX_SERVICE_DATA_LENGTH	= 260;


//--------------------------------------------------------------------------------------------
//
//  Define data types used in MIC messages.
//
//--------------------------------------------------------------------------------------------
                    
typedef BYTE        MIC_MSGFLAG;
typedef BYTE        MIC_TYPE;
typedef USHORT      MIC_ERROR;
typedef MIC_ERROR   MICERR;

typedef DWORD       MIC_ID;
typedef DWORD       MIC_MODE;
typedef USHORT      MIC_TERM;
typedef USHORT      MIC_PROP;
typedef BYTE        MIC_STATE;

typedef MIC_MODE    MIC_NETWORKMODE;

typedef MIC_MODE    MIC_CHANNELMODE;
typedef MIC_MODE    MIC_CHANNELFLAG;
typedef DWORD       MIC_CHANNELCREATE;
typedef DWORD       MIC_CHANNELOPEN;

typedef MIC_MODE    MIC_USERMODE;

typedef MIC_MODE    MIC_MEMBERMODE;


//--------------------------------------------------------------------------------------------
//
//  Define useful constants.
//
//--------------------------------------------------------------------------------------------

const MIC_ID MIC_ID_INVALID = 0;
const MIC_ID MIC_ID_NULL = 0;


//--------------------------------------------------------------------------------------------
//
//  Define Client <--> Server message types.
//
//--------------------------------------------------------------------------------------------

const MIC_TYPE MIC_MSGTYPE_NULL             	= 0x00;
const MIC_TYPE MIC_MSGTYPE_FIRSTCLIENT     		= 0x01;
const MIC_TYPE MIC_MSGTYPE_CONNECTSSPI			= 0x01;
const MIC_TYPE MIC_MSGTYPE_CONNECTUSER			= 0x02;
const MIC_TYPE MIC_MSGTYPE_KILLUSER     		= 0x03;
const MIC_TYPE MIC_MSGTYPE_CREATECHANNEL		= 0x04;
const MIC_TYPE MIC_MSGTYPE_OPENCHANNEL			= 0x05;
const MIC_TYPE MIC_MSGTYPE_LEAVECHANNEL			= 0x06;  
const MIC_TYPE MIC_MSGTYPE_CLOSE				= 0x07;
const MIC_TYPE MIC_MSGTYPE_SENDINVITE			= 0x08;
const MIC_TYPE MIC_MSGTYPE_SENDMSG				= 0x09;
const MIC_TYPE MIC_MSGTYPE_SENDUDP     			= 0x0A;
const MIC_TYPE MIC_MSGTYPE_SENDUSER				= 0x0B;
const MIC_TYPE MIC_MSGTYPE_SENDWHISPER			= 0x0C;
const MIC_TYPE MIC_MSGTYPE_RESERVED2            = 0x0D;  
const MIC_TYPE MIC_MSGTYPE_GETPROPERTY			= 0x0E;
const MIC_TYPE MIC_MSGTYPE_SETPROPERTY			= 0x0F;
const MIC_TYPE MIC_MSGTYPE_QUERY				= 0x10;
const MIC_TYPE MIC_MSGTYPE_RESERVED3			= 0x11;
const MIC_TYPE MIC_MSGTYPE_RESERVED4        	= 0x12;
const MIC_TYPE MIC_MSGTYPE_RESERVED5    		= 0x13;
const MIC_TYPE MIC_MSGTYPE_EVENTRESULT			= 0x14;
const MIC_TYPE MIC_MSGTYPE_EVENTSSPI			= 0x15;
const MIC_TYPE MIC_MSGTYPE_EVENTADDNETWORK		= 0x16;
const MIC_TYPE MIC_MSGTYPE_EVENTADDSERVER		= 0x17;
const MIC_TYPE MIC_MSGTYPE_EVENTADDUSER			= 0x18;
const MIC_TYPE MIC_MSGTYPE_EVENTADDCHANNEL		= 0x19;
const MIC_TYPE MIC_MSGTYPE_EVENTADDMEMBER		= 0x1A;
const MIC_TYPE MIC_MSGTYPE_RESERVED6			= 0x1B;
const MIC_TYPE MIC_MSGTYPE_EVENTDELSERVER		= 0x1C;
const MIC_TYPE MIC_MSGTYPE_EVENTDELUSER			= 0x1D;
const MIC_TYPE MIC_MSGTYPE_EVENTDELCHANNEL		= 0x1E;
const MIC_TYPE MIC_MSGTYPE_EVENTDELMEMBER		= 0x1F;
const MIC_TYPE MIC_MSGTYPE_EVENTINVITE			= 0x20;
const MIC_TYPE MIC_MSGTYPE_EVENTKNOCK           = 0x21;
const MIC_TYPE MIC_MSGTYPE_EVENTMSG				= 0x22;
const MIC_TYPE MIC_MSGTYPE_EVENTPROP			= 0x23;
const MIC_TYPE MIC_MSGTYPE_RESERVED7 			= 0x24;	 
const MIC_TYPE MIC_MSGTYPE_RESERVED8  		    = 0x25;
const MIC_TYPE MIC_MSGTYPE_EVENTUDP      		= 0x26;
const MIC_TYPE MIC_MSGTYPE_EVENTUSER			= 0x27;
const MIC_TYPE MIC_MSGTYPE_EVENTWHISPER			= 0x28;
const MIC_TYPE MIC_MSGTYPE_DATAMEMBERLIST		= 0x29;
const MIC_TYPE MIC_MSGTYPE_RESERVED9            = 0x2A; 
const MIC_TYPE MIC_MSGTYPE_DATAPROPERTY			= 0x2B;
const MIC_TYPE MIC_MSGTYPE_DATAQUERY			= 0x2C;
const MIC_TYPE MIC_MSGTYPE_RESERVED10        	= 0x2D;
const MIC_TYPE MIC_MSGTYPE_RESERVED11     		= 0x2E;
const MIC_TYPE MIC_MSGTYPE_RESERVED12     		= 0x2F;
const MIC_TYPE MIC_MSGTYPE_PING					= 0x30;
const MIC_TYPE MIC_MSGTYPE_PONG					= 0x31;
const MIC_TYPE MIC_MSGTYPE_RESERVED13			= 0x32;
const MIC_TYPE MIC_MSGTYPE_LASTCLIENT     		= 0x32;


//--------------------------------------------------------------------------------------------
//
//  Define message flags.
//
//--------------------------------------------------------------------------------------------

const MIC_MSGFLAG MIC_MSGFLAG_USRMASK 		= 0x07;

const MIC_MSGFLAG MIC_MSGFLAG_NONE  		= 0x00;
const MIC_MSGFLAG MIC_MSGFLAG_ANSI          = 0x01;
const MIC_MSGFLAG MIC_MSGFLAG_DATA          = 0x02;
const MIC_MSGFLAG MIC_MSGFLAG_INFO          = 0x04;
const MIC_MSGFLAG MIC_MSGFLAG_LAST  		= 0x80;

const MIC_MSGFLAG MIC_MSGFLAG_DATMASK 		= 0x03;
const MIC_MSGFLAG MIC_MSGFLAG_UNICODE 		= 0x00;

#define IsAnsiMsg(x) ((x) & MIC_MSGFLAG_ANSI)
#define IsUnicodeMsg(x) (((x) & MIC_MSGFLAG_DATMASK) == MIC_MSGFLAG_UNICODE)
#define IsDataMsg(x) ((x) & MIC_MSGFLAG_DATA)


//--------------------------------------------------------------------------------------------
//
//  Define MIC error codes.
//
//--------------------------------------------------------------------------------------------

const MICERR MICERR_NONE                    = 0x0000;
const MICERR MICERR_ALIASEXIST              = 0x0001;
const MICERR MICERR_ALIASNOTFOUND           = 0x0002;
const MICERR MICERR_ALREADYONCHANNEL        = 0x0003;
const MICERR MICERR_ALREADYREGISTERED       = 0x0004;
const MICERR MICERR_ANSIONLY                = 0x0005;
const MICERR MICERR_AUTHONLY                = 0x0006;
const MICERR MICERR_BADACCOUNT              = 0x0007;
const MICERR MICERR_BADCHANNELGUID          = 0x0008;
const MICERR MICERR_BADCHANNELNAME          = 0x0009;
const MICERR MICERR_BADKEYWORD              = 0x000A;
const MICERR MICERR_BADMESSAGE              = 0x000B;
const MICERR MICERR_BADPACKAGE              = 0x000C; 
const MICERR MICERR_BADPROPERTY             = 0x000D;
const MICERR MICERR_BADUSERALIAS            = 0x000E;
const MICERR MICERR_BADUSERNAME             = 0x000F;
const MICERR MICERR_BANNED                  = 0x0010;
const MICERR MICERR_CHANNELEXIST            = 0x0011;
const MICERR MICERR_CHANNELISFULL           = 0x0012;
const MICERR MICERR_CHANNELNOTFOUND         = 0x0013;
const MICERR MICERR_DNSLOOKUPFAILED         = 0x0014;
const MICERR MICERR_DROPCONNECTION          = 0x0015;
const MICERR MICERR_DUPLICATE               = 0x0016;
const MICERR MICERR_INTERNAL                = 0x0017;
const MICERR MICERR_INVITEONLY              = 0x0018;
const MICERR MICERR_KNOCK                   = 0x0019;
const MICERR MICERR_LOSTCONNECTION          = 0x001A;
const MICERR MICERR_NOACCESS                = 0x001B;
const MICERR MICERR_NOALIAS                 = 0x001C;
const MICERR MICERR_NOBROADCAST             = 0x001D;
const MICERR MICERR_NODATA                  = 0x001E;
const MICERR MICERR_NODYNAMICCHANNELS       = 0x001F;
const MICERR MICERR_NOEXTERNAL              = 0x0020;
const MICERR MICERR_NOGAMEMODE				= 0x0021;								
const MICERR MICERR_NOIMPERSONATION			= 0x0022;								
const MICERR MICERR_NOIRCCLIENTS            = 0x0023;
const MICERR MICERR_NOMATCHES               = 0x0024;
const MICERR MICERR_NONEWCONNECTIONS        = 0x0025;
const MICERR MICERR_NOREMOTEUSERS           = 0x0026;
const MICERR MICERR_NOSPEAKING              = 0x0027;
const MICERR MICERR_NOTSUPPORTED            = 0x0028;
const MICERR MICERR_NOWHISPER               = 0x0029;
const MICERR MICERR_NOTFOUND                = 0x002A;
const MICERR MICERR_NOTHANDLED              = 0x002B;
const MICERR MICERR_NOTHOST                 = 0x002C;
const MICERR MICERR_NOTMEMBER               = 0x002D;
const MICERR MICERR_NOTONCHANNEL            = 0x002E;
const MICERR MICERR_RESOURCE                = 0x002F;
const MICERR MICERR_ROUTING                 = 0x0030;
const MICERR MICERR_PENDING                 = 0x0031;
const MICERR MICERR_SECURITY                = 0x0032;
const MICERR MICERR_SERVERISFULL            = 0x0033;
const MICERR MICERR_SERVERNOTFOUND          = 0x0034;
const MICERR MICERR_SERVICE                 = 0x0035;
const MICERR MICERR_SHUTDOWN                = 0x0036;
const MICERR MICERR_TIMEOUT                 = 0x0037;
const MICERR MICERR_TOOMANYCHANNELS         = 0x0038;
const MICERR MICERR_UNKNOWNID               = 0x0039;
const MICERR MICERR_UNKNOWNTERM             = 0x003A;
const MICERR MICERR_UNKNOWNPROP             = 0x003B;
const MICERR MICERR_UNKNOWNUSER             = 0x003C;
const MICERR MICERR_USERONCHANNEL           = 0x003D;
const MICERR MICERR_USERNOTFOUND            = 0x003E;
const MICERR MICERR_USERNOTINCHANNEL        = 0x003F;
const MICERR MICERR_NOTMICCLIENT			= 0x0040;
const MICERR MICERR_NOTSYSOP    			= 0x0041;
const MICERR MICERR_MOREDATA    			= 0x0042;
const MICERR MICERR_BADPARAMETER            = 0x0043;
const MICERR MICERR_FLOODING                = 0x0044;
const MICERR MICERR_AWAY                    = 0x0045;
const MICERR MICERR_BADUSERUSERID           = 0x0046;
const MICERR MICERR_NOUNICODE               = 0x0047;
const MICERR MICERR_ADMINRESTRICTED         = 0x0048;
const MICERR MICERR_BADPROTOCOLVERSION      = 0x0049;


//--------------------------------------------------------------------------------------------
//
//  Define Chat Service Constants.
//
//--------------------------------------------------------------------------------------------

const MIC_MODE MIC_SERVICEMODE_SYSMASK      = 0x000002F7;

const MIC_MODE MIC_SERVICEMODE_NONE			= 0x00000000;   
const MIC_MODE MIC_SERVICEMODE_MICONLY   	= 0x00000001;
const MIC_MODE MIC_SERVICEMODE_DISABLENEW	= 0x00000002;   
const MIC_MODE MIC_SERVICEMODE_NOALIAS	    = 0x00000004;
const MIC_MODE MIC_SERVICEMODE_NOCHANHOST	= 0x00000010;
const MIC_MODE MIC_SERVICEMODE_NODYNAMIC 	= 0x00000020;
const MIC_MODE MIC_SERVICEMODE_SYSOPISHOST 	= 0x00000040;
const MIC_MODE MIC_SERVICEMODE_INTRANET 	= 0x00000080;
const MIC_MODE MIC_SERVICEMODE_INVISUSERS 	= 0x00000100;


//--------------------------------------------------------------------------------------------
//
//  Define Chat Class Constants.
//
//--------------------------------------------------------------------------------------------

const MIC_MODE MIC_CLASSMODE_SYSMASK        = 0x0000030F;

const MIC_MODE MIC_CLASSMODE_NONE		 	= 0x00000000;
const MIC_MODE MIC_CLASSMODE_DENYACCESS 	= 0x00000001;
const MIC_MODE MIC_CLASSMODE_NOCHANHOST		= 0x00000002;
const MIC_MODE MIC_CLASSMODE_NODYNAMIC 		= 0x00000004;
const MIC_MODE MIC_CLASSMODE_NOJOINDYNAMIC  = 0x00000008;
const MIC_MODE MIC_CLASSMODE_PERMITADMIN	= 0x00000100;
const MIC_MODE MIC_CLASSMODE_PERMITSYSOP	= 0x00000200;


//--------------------------------------------------------------------------------------------
//
//  Define Network Constants.
//
//--------------------------------------------------------------------------------------------

const MIC_MODE MIC_NETWORKMODE_REMOTE		= 0x80000000;   


//--------------------------------------------------------------------------------------------
//
//  Define Portal (server to server connects) Constants.
//
//--------------------------------------------------------------------------------------------

const MIC_MODE MIC_PORTALMODE_SYSMASK       = 0x00000003;   

const MIC_MODE MIC_PORTALMODE_RESERVED      = 0x00000001;   // TRUSTED 
const MIC_MODE MIC_PORTALMODE_UPLINK        = 0x00000002; 


//--------------------------------------------------------------------------------------------
//
//  Define Server Constants.
//
//--------------------------------------------------------------------------------------------

const MIC_MODE MIC_SERVERMODE_REMOTE		= 0x80000000;   


//--------------------------------------------------------------------------------------------
//
//  Define Channel Constants.
//
//--------------------------------------------------------------------------------------------

const MIC_MODE MIC_CHANNELMODE_USRMASK      = 0x0000FFFF;   
const MIC_MODE MIC_CHANNELMODE_SYSMASK      = 0x0000FFFF;   
const MIC_MODE MIC_CHANNELMODE_MICMASK      = 0x00000400;   

const MIC_MODE MIC_CHANNELMODE_NONE         = 0x00000000;   
const MIC_MODE MIC_CHANNELMODE_PUBLIC       = 0x00000000;   
const MIC_MODE MIC_CHANNELMODE_PRIVATE      = 0x00000001;   // +p
const MIC_MODE MIC_CHANNELMODE_RESERVED1    = 0x00000002;      
const MIC_MODE MIC_CHANNELMODE_SECRET       = 0x00000004;   // +s
const MIC_MODE MIC_CHANNELMODE_INVITE       = 0x00000008;   // +i
const MIC_MODE MIC_CHANNELMODE_MODERATED    = 0x00000010;   // +m
const MIC_MODE MIC_CHANNELMODE_NOEXTERN     = 0x00000020;   // +n
const MIC_MODE MIC_CHANNELMODE_TOPICOP      = 0x00000040;   // +t
const MIC_MODE MIC_CHANNELMODE_RESERVED2    = 0x00000080;   
const MIC_MODE MIC_CHANNELMODE_AUDITORIUM   = 0x00000100;   // +a
const MIC_MODE MIC_CHANNELMODE_KNOCK        = 0x00000200;   
const MIC_MODE MIC_CHANNELMODE_NOALIAS      = 0x00000400;
const MIC_MODE MIC_CHANNELMODE_NODATA       = 0x00000800;
const MIC_MODE MIC_CHANNELMODE_NOREALNAME   = 0x00001000;   
const MIC_MODE MIC_CHANNELMODE_NOREMOTE     = 0x00002000; 
const MIC_MODE MIC_CHANNELMODE_NOWHISPER    = 0x00004000;
const MIC_MODE MIC_CHANNELMODE_SYSOPISHOST  = 0x00008000;
const MIC_MODE MIC_CHANNELMODE_DEFAULT      = (MIC_CHANNELMODE_NOEXTERN |
											   MIC_CHANNELMODE_TOPICOP);

const MIC_MODE MIC_CHANNELFLAG_USRMASK      = 0x000003FF;   
const MIC_MODE MIC_CHANNELFLAG_SYSMASK      = 0xF000F7FF;
const MIC_MODE MIC_CHANNELFLAG_MICMASK      = 0x00000090;   

const MIC_MODE MIC_CHANNELFLAG_NONE         = 0x00000000;   
const MIC_MODE MIC_CHANNELFLAG_AUTHJOIN     = 0x00000001;   
const MIC_MODE MIC_CHANNELFLAG_AUTHTALK     = 0x00000002;   
const MIC_MODE MIC_CHANNELFLAG_FEED         = 0x00000004;   
const MIC_MODE MIC_CHANNELFLAG_LOCAL        = 0x00000008;   
const MIC_MODE MIC_CHANNELFLAG_MICONLY      = 0x00000010;	
const MIC_MODE MIC_CHANNELFLAG_ROOMCHAT     = 0x00000020;   
const MIC_MODE MIC_CHANNELFLAG_TEMPLATE     = 0x00000040;   
const MIC_MODE MIC_CHANNELFLAG_UNICODE      = 0x00000080;   
const MIC_MODE MIC_CHANNELFLAG_RESERVED     = 0x00000100;   // VIEWLOCAL
const MIC_MODE MIC_CHANNELFLAG_ECHOSOURCE   = 0x00000200;
const MIC_MODE MIC_CHANNELFLAG_CLONE        = 0x00000400;   // Clone of template channel.
const MIC_MODE MIC_CHANNELFLAG_PERSISTENT   = 0x00001000;   
const MIC_MODE MIC_CHANNELFLAG_AUTOSTART    = 0x00002000;  
const MIC_MODE MIC_CHANNELFLAG_SERVICE      = 0x00004000;   
const MIC_MODE MIC_CHANNELFLAG_IMPERSONATE  = 0x00008000;   
const MIC_MODE MIC_CHANNELFLAG_LOCALROUTE   = 0x10000000;   
const MIC_MODE MIC_CHANNELFLAG_SOURCEROUTE  = 0x20000000;   
const MIC_MODE MIC_CHANNELFLAG_GLOBALROUTE  = 0x40000000;   
const MIC_MODE MIC_CHANNELFLAG_DOMAINROUTE  = 0x80000000;   

const MIC_MODE MIC_CHANNELOPEN_USRMASK      = 0x00000003;   
const MIC_MODE MIC_CHANNELOPEN_SYSMASK      = 0x00000003;   

const MIC_MODE MIC_CHANNELOPEN_NONE         = 0x00000000;
const MIC_MODE MIC_CHANNELOPEN_ALWAYS       = 0x00000001;
const MIC_MODE MIC_CHANNELOPEN_LOCAL        = 0x00000002;
const MIC_MODE MIC_CHANNELOPEN_RESERVED1    = 0x00000004;   // FUTURE: NOHISTORY
const MIC_MODE MIC_CHANNELOPEN_RESERVED2    = 0x00000008;	// FUTURE: NOMEMLIST
const MIC_MODE MIC_CHANNELOPEN_RESERVED3    = 0x00000010;   // FUTURE: USEDEFAULTS
const MIC_MODE MIC_CHANNELOPEN_RESERVED4    = 0x00000100;	// FUTURE: NOCHAPROPS
const MIC_MODE MIC_CHANNELOPEN_RESERVED5    = 0x00000200;	// FUTURE: NOMEMPROPS
const MIC_MODE MIC_CHANNELOPEN_RESERVED6    = 0x00000400;	// FUTURE: NOMEMUPDATES


//--------------------------------------------------------------------------------------------
//
//  Define User Constants.
//
//--------------------------------------------------------------------------------------------

const DWORD MIC_USERMODE_LOGMASK			= 0x0000030F;   // Bits settable at logon.
const DWORD MIC_USERMODE_USRMASK			= 0x0000000F;   // Bits settable by the client.
const DWORD MIC_USERMODE_SYSMASK			= 0x811F070F;   

const DWORD MIC_USERMODE_NONE     			= 0x00000000;
const DWORD MIC_USERMODE_INVISIBLE			= 0x00000001;
const DWORD MIC_USERMODE_NODATA				= 0x00000002;
const DWORD MIC_USERMODE_NOTICES			= 0x00000004;
const DWORD MIC_USERMODE_NOWHISPER			= 0x00000008;
const DWORD MIC_USERMODE_EVENT  			= 0x00000100;  
const DWORD MIC_USERMODE_MICONLY			= 0x00000200;  
const DWORD MIC_USERMODE_AUTHUSER			= 0x00010000;  
const DWORD MIC_USERMODE_AUTHVISIBLE		= 0x00020000;  
const DWORD MIC_USERMODE_AWAY				= 0x00040000;  
const DWORD MIC_USERMODE_IRC				= 0x00080000;  
const DWORD MIC_USERMODE_SYSOP				= 0x00100000;  
const DWORD MIC_USERMODE_SIMPLE				= 0x01000000;  
const DWORD MIC_USERMODE_REMOTE				= 0x80000000;  


//--------------------------------------------------------------------------------------------
//
//  Define Member Constants.
//
//--------------------------------------------------------------------------------------------

const DWORD MIC_MEMBERMODE_HSTMASK			= 0x00000003;   // Bits a channel host can set.
const DWORD MIC_MEMBERMODE_USRMASK			= 0x00001103;   // Bits a member can set.
const DWORD MIC_MEMBERMODE_LVLMASK			= 0x00000003;
const DWORD MIC_MEMBERMODE_SYSMASK			= 0x80701103;   

const DWORD MIC_MEMBERMODE_NONE				= 0x00000000;
const DWORD MIC_MEMBERMODE_SPEAKER			= 0x00000001;
const DWORD MIC_MEMBERMODE_HOST				= 0x00000002;
const DWORD MIC_MEMBERMODE_NODATA			= 0x00000100;
const DWORD MIC_MEMBERMODE_RESERVED1        = 0x00000200;	// FUTURE: NOCHAPROPS
const DWORD MIC_MEMBERMODE_RESERVED2        = 0x00000400;	// FUTURE: NOMEMPROPS
const DWORD MIC_MEMBERMODE_RESERVED3        = 0x00000800;	// FUTURE: NOMEMUPDATES
const DWORD MIC_MEMBERMODE_NOWHISPER		= 0x00001000;	
const DWORD MIC_MEMBERMODE_AUTHUSER         = 0x00100000;   
const DWORD MIC_MEMBERMODE_IRC				= 0x00200000;   
const DWORD MIC_MEMBERMODE_SYSOP			= 0x00400000;   
const DWORD MIC_MEMBERMODE_SIMPLE			= 0x01000000;  
const DWORD MIC_MEMBERMODE_REMOTE			= 0x80000000;   

const DWORD MIC_MEMBERMODE_DEFAULT			= MIC_MEMBERMODE_SPEAKER;


//--------------------------------------------------------------------------------------------
//
//  Property Constants.  
//
//--------------------------------------------------------------------------------------------

//
//  Define Property Operations
//
const MIC_PROP MIC_PROP_OP_MASK               = (0x03 << 14);
const MIC_PROP MIC_PROP_OP_SET                = (0x00 << 14);
const MIC_PROP MIC_PROP_OP_ADD                = (0x01 << 14);
const MIC_PROP MIC_PROP_OP_DEL                = (0x02 << 14);

//
//  Define Property Types
//
const MIC_PROP MIC_PROP_TYPE_MASK             = (0x07 << 11);
const MIC_PROP MIC_PROP_TYPE_1                = (0x00 << 11);
const MIC_PROP MIC_PROP_TYPE_2                = (0x01 << 11);
const MIC_PROP MIC_PROP_TYPE_4                = (0x02 << 11);
const MIC_PROP MIC_PROP_TYPE_8                = (0x03 << 11);
const MIC_PROP MIC_PROP_TYPE_LEN1             = (0x04 << 11);
const MIC_PROP MIC_PROP_TYPE_LEN2             = (0x05 << 11);
const MIC_PROP MIC_PROP_TYPE_STR1             = (0x06 << 11);
const MIC_PROP MIC_PROP_TYPE_STR2             = (0x07 << 11);

//
//  Define Property Groups
//
const MIC_PROP MIC_PROP_GROUP_MASK            = (0x0F << 7);
const MIC_PROP MIC_PROP_GROUP_GENERIC         = (0x00 << 7);
const MIC_PROP MIC_PROP_GROUP_DOMAIN          = (0x01 << 7);
const MIC_PROP MIC_PROP_GROUP_NETWORK         = (0x02 << 7);
const MIC_PROP MIC_PROP_GROUP_CLASSES         = (0x03 << 7);
const MIC_PROP MIC_PROP_GROUP_PORTAL          = (0x04 << 7);
const MIC_PROP MIC_PROP_GROUP_SERVER          = (0x05 << 7);
const MIC_PROP MIC_PROP_GROUP_USER            = (0x06 << 7);
const MIC_PROP MIC_PROP_GROUP_SERVICE         = (0x07 << 7);
const MIC_PROP MIC_PROP_GROUP_CHANNEL         = (0x08 << 7);
const MIC_PROP MIC_PROP_GROUP_MEMBER          = (0x09 << 7);
const MIC_PROP MIC_PROP_GROUP_SPECIAL         = (0x0F << 7);

//
//  Define Property Groups
//
const MIC_PROP MIC_PROP_BASE_MASK             = 0x007F;

//
//  Define Generic Property Constants.
//
const MIC_PROP MIC_PROP_ID                    = 0x00 | MIC_PROP_OP_SET | MIC_PROP_GROUP_GENERIC | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_CTIME                 = 0x01 | MIC_PROP_OP_SET | MIC_PROP_GROUP_GENERIC | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_MODES                 = 0x02 | MIC_PROP_OP_SET | MIC_PROP_GROUP_GENERIC | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_MODES2                = 0x02 | MIC_PROP_OP_SET | MIC_PROP_GROUP_GENERIC | MIC_PROP_TYPE_8;
const MIC_PROP MIC_PROP_SERVER                = 0x03 | MIC_PROP_OP_SET | MIC_PROP_GROUP_GENERIC | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_SYSTEM                = 0x04 | MIC_PROP_OP_SET | MIC_PROP_GROUP_GENERIC | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_USER1                 = 0x05 | MIC_PROP_OP_SET | MIC_PROP_GROUP_GENERIC | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_LAST                  = 0x05;

//
//  Define Network Property Constants.
//
const MIC_PROP MIC_PROP_NETWORK_ID            = 0x00 | MIC_PROP_OP_SET | MIC_PROP_GROUP_NETWORK | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_NETWORK_LAST          = 0x00;


//
//  Define Server Property Constants.
//
const MIC_PROP MIC_PROP_SERVER_ID             = 0x00 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_SERVER_HOPS           = 0x01 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_1;  
const MIC_PROP MIC_PROP_SERVER_UPLINK         = 0x02 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_1;  
const MIC_PROP MIC_PROP_SERVER_ADMIN          = 0x03 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_SERVER_DNS            = 0x04 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_SERVER_TITLE          = 0x05 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_SERVER_IPADDR         = 0x06 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_4;  
const MIC_PROP MIC_PROP_SERVER_IPPORT         = 0x07 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_2;  
const MIC_PROP MIC_PROP_SERVER_MOTD           = 0x08 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_STR2;  
const MIC_PROP MIC_PROP_SERVER_INFO           = 0x09 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_STR2;  
const MIC_PROP MIC_PROP_SERVER_TIME           = 0x0A | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_8;  
const MIC_PROP MIC_PROP_SERVER_VERSION        = 0x0B | MIC_PROP_OP_SET | MIC_PROP_GROUP_SERVER  | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_SERVER_LAST           = 0x0B;

//
//  Define Server Property Structures.
//
typedef struct _MIC_SERVER_TIME
{
	DWORD	Time;						
    SHORT	TimeZone;
    SHORT	DstFlag;
} MIC_SERVER_TIME, * PMIC_SERVER_TIME;


//
//  Define User Property Constants.
//
const MIC_PROP MIC_PROP_USER_ID               = 0x00 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_USER_HACCT            = 0x01 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_8; 
const MIC_PROP MIC_PROP_USER_ALIAS            = 0x02 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_USER_IDENT            = 0x03 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_USER_NAME             = 0x04 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_USER_IPADDR           = 0x05 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_USER_IPPORT           = 0x06 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_2;
const MIC_PROP MIC_PROP_USER_AWAY             = 0x07 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_USER_PACKAGE          = 0x08 | MIC_PROP_OP_SET | MIC_PROP_GROUP_USER    | MIC_PROP_TYPE_STR1;
const MIC_PROP MIC_PROP_USER_LAST             = 0x08;


//
//  Define Channel Property Constants.
//
const MIC_PROP MIC_PROP_CHANNEL_ID            = 0x00 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_CHANNEL_NAME          = 0x01 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;
const MIC_PROP MIC_PROP_CHANNEL_KEYWORD       = 0x02 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_KEYWORD_ADD   = 0x02 | MIC_PROP_OP_ADD | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_KEYWORD_DEL   = 0x02 | MIC_PROP_OP_DEL | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_TOPIC         = 0x03 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_SUBJECT       = 0x04 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_CLIENTGUID    = 0x05 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_LEN1;  
const MIC_PROP MIC_PROP_CHANNEL_SERVICEGUID   = 0x06 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_LEN1;  
const MIC_PROP MIC_PROP_CHANNEL_FLAGS         = 0x07 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_4;  
const MIC_PROP MIC_PROP_CHANNEL_FLAGS2        = 0x07 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_8;  
const MIC_PROP MIC_PROP_CHANNEL_LIMIT         = 0x08 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_4;  
const MIC_PROP MIC_PROP_CHANNEL_RESERVED      = 0x09 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_4;  
const MIC_PROP MIC_PROP_CHANNEL_ACCOUNT       = 0x0A | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;
const MIC_PROP MIC_PROP_CHANNEL_MEMBERS       = 0x0B | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_4;  
const MIC_PROP MIC_PROP_CHANNEL_PICS          = 0x0C | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_RESERVED1     = 0x0D | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_IDLE          = 0x0E | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_4;
const MIC_PROP MIC_PROP_CHANNEL_DATA          = 0x0F | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;
const MIC_PROP MIC_PROP_CHANNEL_BANNED        = 0x10 | MIC_PROP_OP_SET | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_BANNED_ADD    = 0x10 | MIC_PROP_OP_ADD | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_BANNED_DEL    = 0x10 | MIC_PROP_OP_DEL | MIC_PROP_GROUP_CHANNEL | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_CHANNEL_LAST          = 0x10;


//
//  Define Channel Property Constants.
//
const MIC_PROP MIC_PROP_MEMBER_UDPPORT        = 0x00 | MIC_PROP_OP_SET | MIC_PROP_GROUP_MEMBER | MIC_PROP_TYPE_2;
const MIC_PROP MIC_PROP_MEMBER_ALIAS          = 0x01 | MIC_PROP_OP_SET | MIC_PROP_GROUP_MEMBER | MIC_PROP_TYPE_STR1;  
const MIC_PROP MIC_PROP_MEMBER_LAST           = 0x01;


//
//  Define Query Term Constants.
//
const MIC_PROP MIC_TERM_CONTAINS              = 0x00 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_MASK                  = 0x01 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_EQUAL                 = 0x02 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_NOTEQUAL              = 0x03 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_GREATER               = 0x04 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_GREATEREQUAL          = 0x05 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_LESS                  = 0x06 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_LESSEQUAL             = 0x07 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_AND                   = 0x08 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_OR                    = 0x09 | MIC_PROP_OP_SET | MIC_PROP_GROUP_SPECIAL | MIC_PROP_TYPE_2;
const MIC_PROP MIC_TERM_LAST                  = 0x09;


//--------------------------------------------------------------------------------------------
//
//  Query Constants.
//
//--------------------------------------------------------------------------------------------

//
//  Group Identfiers.
//
const MIC_ID MIC_ID_DOMAIN                  = 0xFFFF0001;
const MIC_ID MIC_ID_NETWORKS                = 0xFFFF0002;
const MIC_ID MIC_ID_CLASSES                 = 0xFFFF0003;
const MIC_ID MIC_ID_PORTALS                 = 0xFFFF0004;
const MIC_ID MIC_ID_SERVERS                 = 0xFFFF0005;
const MIC_ID MIC_ID_USERS                   = 0xFFFF0006;
const MIC_ID MIC_ID_CHANNELS                = 0xFFFF0007;
const MIC_ID MIC_ID_SERVICES                = 0xFFFF0008;


//--------------------------------------------------------------------------------------------
//
//  Message data structures.
//
//--------------------------------------------------------------------------------------------

#pragma pack(1)

typedef struct
{
    BYTE   Type;
    BYTE   Flags;
    USHORT Length;
} MICMSG_HEADER;
                    
typedef struct
{
    BYTE   Type;
    BYTE   Flags;
    USHORT Length;
} MICMSG_CLIENT;
                    
typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    BYTE            Protocol;
    BYTE            Version;
	BYTE			Reserved;
    BYTE            Sequence;
    MIC_USERMODE    Modes;
	BYTE			PackageLength;
    BYTE            AliasLength;
    BYTE            NameLength;
  //CHAR			PackageName[];
  //CHAR			Alias[];
  //CHAR			Name[];
  //BYTE			DataSSPI[];
} MICMSG_CONNECTSSPI;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    BYTE            Protocol;
    BYTE            Version;
	BYTE			Reserved1;
    BYTE            DescriptionLength;			// User provided text, normally 'first last' name.
    MIC_USERMODE    Modes;
    BYTE            AliasLength;
    BYTE            PasswordLength;
    BYTE            NameLength;					// Userid.
  //TCHAR			Alias[AliasLength];
  //TCHAR           Password[PasswordLength];
  //TCHAR           Name[NameLength];
  //TCHAR           Description[UseridLength];
} MICMSG_CONNECTUSER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Uid;
	BYTE			AliasLength;
	BYTE			ReasonLength;
  //TCHAR			Alias[AliasLength];
  //TCHAR           Reason[ReasonLength];
} MICMSG_KILLUSER;   

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_CHANNELMODE ChannelMode;
    MIC_CHANNELFLAG ChannelFlags;
    MIC_CHANNELOPEN ChannelOpen;
    ULONG           Limit;
	USHORT          UdpPort;
    BYTE            NameLength;
    BYTE            KeywordLength;
    BYTE            TopicLength;
    BYTE            SubjectLength;
    BYTE            Reserved;
	BYTE			ClientGuidLength; 	
	BYTE			ServiceGuidLength;
	BYTE			AccountLength;
} MICMSG_CREATECHANNEL;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_CHANNELOPEN ChannelOpen;
	USHORT          UdpPort;
    BYTE            NameLength;
    BYTE            KeywordLength;
} MICMSG_OPENCHANNEL;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Cid;			
  //TCHAR           Reason[#];
} MICMSG_LEAVECHANNEL;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Oid;			
  //TCHAR           Reason[#];
} MICMSG_CLOSE;   

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Oid;
  //BYTE            Message[#];
} MICMSG_SENDMSG;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Cid;
	MIC_ID			Mid;
	DWORD			Cookie;
  //BYTE            Message[#];
} MICMSG_SENDUDP;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Uid;
	BYTE			AliasLength;
  //TCHAR			Alias[AliasLength];
  //BYTE            Message[#];
} MICMSG_SENDUSER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    BYTE            Count;
  //MIC_ID          MemberId[Count];
  //BYTE            Message[#];
} MICMSG_SENDWHISPER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Uid;
	BYTE			AliasLength;
	BYTE			ChannelLength;
  //TCHAR			Alias[AliasLength];
  //TCHAR           ChannelName[ChannelLength];
} MICMSG_SENDINVITE;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Oid;
  //MIC_PROP		Props[#];
} MICMSG_GETPROPERTY;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Oid;
    BYTE            PropCount;			
  //MIC_PROP		Props[PropCount];
  //BYTE			PropData[0];
} MICMSG_SETPROPERTY;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Oid;
    BYTE            TermCount;
    BYTE            PropCount;
	USHORT			MaxRecords; 
} MICMSG_QUERY;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
	DWORD           Oid;
    MICERR          Result;
    BYTE            Message;
} MICMSG_EVENTRESULT;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    BYTE            Version;
    BYTE            Sequence;
} MICMSG_EVENTSSPI;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Nid;
	BYTE			NameLength;
    BYTE            TitleLength;
  //TCHAR			Name[NameLength];
  //TCHAR			Title[TitleLength];
} MICMSG_EVENTADDNETWORK;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Nid;
    MIC_ID          Sid;
	BYTE			NameLength;
    BYTE            TitleLength;
  //TCHAR			Name[NameLength];
  //TCHAR			Title[TitleLength];
} MICMSG_EVENTADDSERVER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Nid;
    MIC_ID          Sid;
    MIC_ID          Uid;
    MIC_USERMODE    Modes;
} MICMSG_EVENTADDUSER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Cid;
    MIC_CHANNELMODE Modes;
    MIC_CHANNELFLAG ChannelFlags;
	DWORD			Cookie;
	DWORD           UDPAddress;
	USHORT			UDPPort;
    USHORT          Reserved;
    BYTE            NameLength;
    BYTE            TopicLength;
    BYTE            PicsLength;
} MICMSG_EVENTADDCHANNEL;


typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Cid;
    MIC_ID          Mid;
    MIC_MEMBERMODE  Modes;
	BYTE            AliasLength;
} MICMSG_EVENTADDMEMBER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Sid;
} MICMSG_EVENTDELSERVER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Uid;
    MIC_ID          Kid;
    MICERR          Error;		
  //TCHAR			Reason;
} MICMSG_EVENTDELUSER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Cid;
} MICMSG_EVENTDELCHANNEL;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Mid;
    MIC_ID          Kid;
  //BYTE            Reason[#];
} MICMSG_EVENTDELMEMBER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          From;
    MIC_ID          To;
  //BYTE            Message[#];
} MICMSG_EVENTMSG;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Cid;
	MIC_ID			Mid;
	DWORD			Reserved;
  //BYTE            Message[#];
} MICMSG_EVENTUDP;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          From;
	MIC_ID			To;
    BYTE            AliasLength;
  //TCHAR           Alias[AliasLength];
  //BYTE            Message[#];
} MICMSG_EVENTUSER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          From;
    BYTE            Count;
  //MIC_ID          MemberId[Count];
  //BYTE            Message[#];
} MICMSG_EVENTWHISPER;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          From;
    BYTE            AliasLength;
  //CHAR            Alias[AliasLength];
  //CHAR            ChannelName[#];
} MICMSG_EVENTINVITE;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Cid;
    MIC_ID          Uid;
} MICMSG_EVENTKNOCK; 

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          From;
    MIC_ID          To;
    BYTE            PropCount;
  //MIC_PROP		Props[PropCount];
  //BYTE			PropData[0];
} MICMSG_EVENTPROP;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;  
	MIC_ID          Cid;
  //struct
  //{
  //  MICMSG_MEMBERITEM Member;
  //} [#]  
} MICMSG_DATAMEMBERLIST;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Id;
    BYTE            PropCount;
  //MIC_PROP		Props[PropCount];
  //BYTE			PropData[0];
} MICMSG_DATAPROPERTY;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
    MIC_ID          Id;
    BYTE            PropCount;
  //MIC_PROP		Props[PropCount];
  //BYTE			PropData[0];
} MICMSG_DATAQUERY;

typedef struct
{
    MIC_ID          Mid;
    MIC_MEMBERMODE  Modes;  
    BYTE            Flags;
    BYTE            AliasLength;
  //BYTE            Alias[#];
} MICMSG_MEMBERITEM;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
} MICMSG_PING;

typedef struct
{
    BYTE            Type;
    BYTE            Flags;
    USHORT          Length;
} MICMSG_PONG;

#pragma pack()

#endif
