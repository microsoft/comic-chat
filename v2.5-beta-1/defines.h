// defines.h : constants declarations
//
#ifndef __DEFINES_H__
#define __DEFINES_H__

#define SB_TOOLBAR				1	// Obsolete - use SB_TOOLBAR_* now
#define SB_STATUSBAR			2
#define SB_TOOLBAR_MAIN			4
#define SB_TOOLBAR_MEMBER		8
#define SB_TOOLBAR_TEXT			16
#define SB_TOOLBAR_ANY			(SB_TOOLBAR_MAIN|SB_TOOLBAR_MEMBER|SB_TOOLBAR_TEXT)
#define SB_TOOLBAR_OLDREAD		32

#define CHAT_TOOLBAR_WHOLE		((UINT)-1)
#define CHAT_TOOLBAR_MAIN		0
#define CHAT_TOOLBAR_MEMBER		1
#define CHAT_TOOLBAR_TEXT  		2

#define FLOOD_IGNORE			1

#define NMACROS					10
#define NHIGHLIGHTEDFONTS		8
#define NREGULARFONTS			10
#define NFONTS					(NHIGHLIGHTEDFONTS+NREGULARFONTS)

#define ST_ANONLOGIN			0
#define ST_IRCPASSWD			1
#define ST_SRVRPACKGS			2
#define ST_USERPACKGS			3

#define MAX_FORMATTINGPERBYTE	10		// ^kWX,YZ^i^u^f^b
#define MAX_INPUTLEN			350		// Used for regular + away + profile + greeting + macros messages + kick reasons
#define MAX_TOPICLEN			94		// Used for topics
#define MAX_ANNOTATIONS			256
#define MAX_COMMAND				128
#define MAX_TOKEN				201		// 200 is room size (used for parse.args and other)
#define MAX_NICK				51

#define MAX_NICKINPUT			23		// user cannot input a nickname longer than 23 bytes
#define MAX_REALNAMEINPUT		30		//											30 bytes
#define MAX_EMAILINPUT			100		//										   100 bytes
#define MAX_HOMEPAGEINPUT		200		//										   200 bytes
#define MAX_CHANNELPWD			40		// IRCX channel password limit is 31 bytes, but might be higher on other IRC servers
#define MAX_IRCCHANNAME			62		// max channel name length on IRC
#define MAX_IRCXCHANNAME		200		// max channel name length on IRCX


// host highlighting for text mode
#define HH_BOLD_HEADERS			1
#define HH_BOLD_MESSAGES		2

// on connect actions
#define CA_JOINROOM				0
#define CA_ROOMLIST				1
#define CA_NOACTION				2

#define SM_SAY					1
#define SM_WHISPER				2
#define SM_THINK				3
#define SM_SHOUT				4
#define SM_ACTION				5

#define BM_SAY					0x0001
#define BM_WHISPER				0x0002
#define BM_THINK				0x0004
#define BM_ACTION				0x0008
#define BM_SOUND				0x0010
#define BM_AWAY					0x0020
#define BM_HERESINFO			0x0040
#define BM_NOFORMAT				0x0080
#define BM_EXCHAN				0x0100

#define CGESTUREPREFIX			'G'
#define CEXPRESSIONPREFIX		'E'
#define CREQUESTEDPREFIX		'R'
#define CMODEPREFIX				'M'
#define CTALKTOPREFIX			'T'

#define SZGESTUREPREFIX			"G:"
#define SZEXPRESSIONPREFIX		"E:"
#define SZREQUESTEDPREFIX		"R:"
#define SZMODEPREFIX			"M:"
#define SZCOOKEDPREFIX			"C:"
#define SZTALKTOPREFIX			"T:"

#define CX_DISCONNECTED			0
#define CX_INCHANNEL			1
#define CX_CONNECTING			2
#define CX_NOCHANNEL			3
#define CX_CONNECTED			4

#define VM_UNSPECIFIED			0
#define VM_COMICS				1
#define VM_TEXT					2

#define UM_HOST					1
#define UM_SPEAKER				2
#define UM_SPECTATOR			3

// automatic greeting types
#define AGT_NONE				0
#define AGT_WHISPER				1
#define AGT_SAY					2

#define SC_OWNER				'.'
#define SC_HOST					'@'
#define SC_SPECTATOR			'>'
#define SC_HASVOICE				'+'

#define SS_HOST					"@"
#define SS_SPECTATOR			">"

#define LINKINDEX				252

#define UNITSPERINCH			1440		// we're using TWIMs
#define DEFAULTDELTA			100

#define STRETCHMODE				STRETCH_HALFTONE

#define DEFAULTPANELPERCOLUMN	3

//RamuM, RegKeys
#define szRootRegKeyName		"Software\\Microsoft\\Microsoft Comic Chat"
#define szProfileValName		"Profile"
#define szBaseDirValName		"BaseDir"
#define szArtDirValName			"ArtDir"
#define szTxtExt				".txt"

// REGISB added 04/08/98 - old resources replaced by %Nickname% and %Room%
#define szUserToken				"%name"
#define szRoomToken				"%room"

// Font info
#define CLIP_DFA_OVERRIDE		0x40				// Necessary for Korean FA Disabling

#define nFontHeight				-14
#define nFontHeightTitle		-576
#define nFontHeightShout		-252
#define nFontWidth				0
#define nFontEscapement			0
#define nFontOrientation		0
#define fnFontWeight			FW_REGULAR
#define fdwFontItalic			FALSE
#define fdwFontUnderline		FALSE
#define fdwFontStrikeOut		FALSE
#define	fdwFontOutputPrecision	OUT_DEFAULT_PRECIS
#define fdwFontClipPrecision	CLIP_DFA_OVERRIDE
#define fdwFontQuality			DEFAULT_QUALITY
#define fdwFontPitchAndFamily	VARIABLE_PITCH | FF_DONTCARE

// Internationalization - have _tcs functions do right thing (single executable has to handle multibyte)
#ifndef _MBCS
#define _MBCS
#endif
#ifndef _MB_MAP_DIRECT
#define _MB_MAP_DIRECT
#endif

#define my_isspace(c)			((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
#define ISTRUE(x)				((x) != 0)	// if argument is a logical true, makes it an official "TRUE".
#define STATUS_CHAR(c)			(((c)==SC_HOST) || ((c)==SC_SPECTATOR) || ((c) == SC_HASVOICE))
#define CHANNELPREFIX(c)		(((c) == '#') || ((c) == '%') || ((c) == '&'))

#define MT_CHANNELSEND			0x01	// message destination is the whole channel
#define MT_PRIVATEMSG			0x02	// message destination is a particular user or some channel members
#define MT_WHISPER				0x04	// message was sent via the IRCX WHISPER command
#define MT_PRVMSG				0x08	// message was sent via the IRC/X PRIVMSG command
#define MT_NOTICE				0x10	// message was sent via the IRC/X NOTICE command
#define MT_DATA					0x20	// message was sent via the IRCX DATA command
#define MT_DATAREQUEST			0x40	// message was sent via the IRCX REQUEST command
#define MT_DATAREPLY			0x80	// message was sent via the IRCX REPLY command

#define CM_PRIVATE				1
#define CM_HIDDEN				2
#define CM_INVITEONLY			4
#define CM_TOPICHOST			8
#define CM_NOEXTERN				16
#define CM_MODERATED			32
#define CM_USERLIMIT			64
#define CM_CHANNELKEY			128
#define CM_NOFORMAT				256
#define CM_MIC					512

#define PC_IRC					1
#define PC_NM					2

#define F1_SHOWMOTD				1
#define F1_MAXMDI				2
#define F1_RTFCOMIC				4
#define F1_HEADERSEPARATE		8
#define F1_SHOWTABBAR			16
#define F1_USERVISIBLE			32

#define F0_SHOWSTATUSWINDOW		1
#define F0_AUTOARRANGEWNDS		2
#define F0_AUTOARRANGEISVERT	4
#define F0_ALREADYRUN			8

#define EASTER_TIMER			81

// MODE ISIRCX timeout
#define ISIRCXTIMEOUT			50000	// 50 seconds

// Main Menu item positions - 0 based
#define VIEWMENUPOS				2
#define MEMBERMENUPOS			5
#define MACROSUBMENUTEXT		6
#define MACROSUBMENUCOMIC		7

// Focus window/component types
#define CHATFOCUS_COMICVIEW		1		// Main comics output pane
#define CHATFOCUS_TEXTVIEW 		2		// Main text output pane
#define CHATFOCUS_INPUTWND		3		// Input window 
#define CHATFOCUS_MEMBERLIST	4		// Member list window
#define CHATFOCUS_EMOTIONWND	5		// Emotion input window
#define CHATFOCUS_TABBAR		6		// Tab bar.
#define CHATFOCUS_OUTPUTWND		7		// "Smart" value - comic or text pane, depending on current view.


// User agent name for the net requestor
#define WEBREQ_USERAGENT_MSCHAT	"MS Chat"
#define STATUS_WINDOW_NAME		": Status"							 

#endif __DEFINES_H__
