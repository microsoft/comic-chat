// chat.h : main header file for the CHAT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


#define SB_TOOLBAR		1
#define SB_STATUSBAR	2

#define FLOOD_IGNORE	1

// MACRO definition

#define NMACROS	10

class CMacro {
public:
	BOOL m_bDefined;
	CString m_strName;
	CString m_strValue;

    CMacro () { m_bDefined = FALSE; }
	void UnSerialize(const char *buff);
	int Serialize(char *buff, int buffLen);
	void Invoke();
};


#define ST_ANONLOGIN	0
#define ST_IRCPASSWD	1
#define ST_SRVRPACKGS	2
#define ST_USERPACKGS	3

class CServer {
public:
	CString m_strName;
	BYTE m_byteSecurity;
	BYTE m_byteRemember;
	CString m_strUser;
	CString m_strPassword;
	CString m_strPackages;

	CServer(const char *szName) { m_strName = szName; m_byteSecurity = 0; m_byteRemember = FALSE; }
};


/////////////////////////////////////////////////////////////////////////////
// CChatApp:
// See chat.cpp for the implementation of this class
//

class CChatApp : public CWinApp
{
public:
	CMenu*			m_pmenuAdmin;
	CMenu*			m_pmenuMacro;
	BOOL			m_bIPMacroDone;    // indicates we've already added the macro menu to the ip frame

	BOOL			m_bComicView;
	BOOL			m_bFoundArt;
	BOOL			m_bEmbedded;
	INT				m_iShowBars;
	LOGFONT			m_comicsFont;
	LOGFONT			m_textFont;
	COLORREF		m_comicsColor;
	COLORREF		m_textColor;
	CFont			m_fontGui;
	CHAR			m_szGuiFaceName[LF_FACESIZE]; 
	BYTE			m_lfGuiPitchAndFamily;
	INT				m_iFontHeightBalloon;
	INT				m_textSpacing;
	CHARFORMAT		m_cfArray[10];
	BOOL			m_bCfInitialized;
	BOOL			m_bShowArrivals;
	INT				m_iHostHighlight;
	BOOL			m_bAllowInvites;
	INT				m_iGreetingType;
	CString			m_strGreetingMesg;
	CMacro			m_macros[NMACROS];
	BOOL			m_bInSearch;
	BOOL			m_bPlaySounds;
	BOOL			m_bAcceptNMCalls;
	BOOL			m_bShowIdentity;
	BOOL			m_bListRegistered;
	RECT			m_whisperRect;
	CPtrArray		m_servers;
	BOOL			m_bSaveViewMode;
	BOOL			m_bAllowFileTX;
	BYTE			m_charSet;
	BYTE			m_floodInterval;
	BYTE			m_floodCount;
	BYTE			m_floodFlags;
	DWORD			m_flags1;
	BOOL			m_bIconMembers;
	BOOL			m_bPrompt;
	BOOL			m_bAcceptWhispers;
	BOOL			m_bAway;
	BOOL			m_bAwayPrompt;
	BOOL			m_bDoCB32;
	BOOL			m_bDoTest;
	CString			m_strAwayMessage;
	CMapStringToPtr	m_ignores;

	CString			m_ircServer;		// formerly in setupdlg
	CString			m_myRealName;
	CString			m_myChannel;
	CString			m_strEmail;
	CString			m_strHomePage;
	CString			m_strChatRooms;
	CString			m_myName;			// pretty version of nick (in DBCS)
	CString			m_myNick;			// if IRCX, unicode nick, otherwise as above
	CString			m_myProfile;
	CString			m_myCharacterName;
	CString			m_strUserName;
	short			m_nMyIdentLength;
	int				m_iOnConnectAction;

	CImageList		m_ImageList;
	CImageList		m_StatusIcons;

	CHAR*			m_szBuffer;
	WCHAR*			m_wszBuffer;
	SHORT			m_nBufferSize;
#ifdef IRCLOG
	FILE *			m_fileIn;
#endif

public:
	CChatApp();
	~CChatApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation

	//{{AFX_MSG(CChatApp)
	afx_msg void OnHelpTopics();
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	afx_msg void OnUpdateSessionConnect(CCmdUI* pCmdUI);
	afx_msg void OnSessionConnect();
	afx_msg void OnNewroom();
	afx_msg void OnUpdateNewroom(CCmdUI* pCmdUI);
	afx_msg void OnCreateroom();
	afx_msg void OnDisconnect();
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnFavoritesOpenfavorites();
	afx_msg void OnViewOptions();
	afx_msg void OnUpdateViewOptions(CCmdUI* pCmdUI);
	afx_msg void OnUserList();
	afx_msg void OnUpdateCanSearch(CCmdUI* pCmdUI);
	afx_msg void OnAwayToggle();
	afx_msg void OnUpdateAwayToggle(CCmdUI* pCmdUI);
	afx_msg void OnViewTabbar();
	afx_msg void OnUpdateViewTabbar(CCmdUI* pCmdUI);
	afx_msg void OnMotd();
	afx_msg void OnUpdateMotd(CCmdUI* pCmdUI);
	afx_msg void OnIrcChat();
	afx_msg void OnHelpFreestuff();
	afx_msg void OnHelpProductnews();
	afx_msg void OnHelpFaq();
	afx_msg void OnHelpOnlineSupport();
	afx_msg void OnHelpBestofWeb();
	afx_msg void OnHelpSearchtheWeb();
	afx_msg void OnHelpMsHomepage();
	afx_msg void OnHelpReleaseNotes();
	//}}AFX_MSG
	afx_msg void OnChatroomList();
	DECLARE_MESSAGE_MAP()

public:
// Parsed command-line arguments


	BOOL	m_bNoRefresh;
	BOOL	m_bShowMode;
	BOOL	m_bVIPMode;
	int		m_xFrame;
	int		m_yFrame;
	int		m_cxFrame;
	int		m_cyFrame;
	int		m_maxedFrame;
	CString m_lastBackDrop;

// search paths
	CString m_strBaseDir;
	CString	m_strAvatarDir;
	CString m_strBackDropDir;
	CString m_strFavoritesDir;
	CString m_strFileTXDir;

	CString m_strDefaultArtDir;
	CString	m_soundPath;

// additional methods

public:
	//helper method for status bar strings
	void SetStatusPaneString(int index, const char *szPane);
	CString m_strStatusPane[2];
	void SetPrinterResolution();
	CDocument* OpenDocumentFile(LPCTSTR lpszFileName);
	BOOL ProcessShellCommand(CCommandLineInfo &cmdLine);
	void InitializeFonts();
	void InitializeComicsFonts();
	CString GetDesktopOrFavorites(BOOL bDesktop);
	HRESULT HrAllocBuffer(SHORT nMaxMsgLength);
	BOOL SaveToReg();
//	COleIPFrameWnd *GetInPlaceFrame() {return (m_bEmbedded ? m_pInPlaceFrame : NULL); }
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);

private:
	void SetBaseDir(const char *fullpath);
	void InitStatusIcons();
	void ParseCommandLine(CCommandLineInfo& rCmdInfo);  // use our own  -- old sometimes gets corrupted (which used?)
	BOOL LoadFromReg();
	void InitVals();
	void InitFileTXDir();

public:
	const char *GetBaseDir() { return ((LPCSTR) m_strBaseDir); }
	const char *GetAvatarDir() { return ((LPCSTR) m_strAvatarDir); }
	const char *GetBackDropDir() { return ((LPCSTR) m_strBackDropDir); }
};

// some defines

// REGISB added 10/14/97
#define MAX_FORMATTINGPERBYTE	10		// ^kWX,YZ^i^u^f^b
#define MAX_INPUTLEN			350		// Used for regular + away + profile + greeting + macros messages
#define MAX_TOPICLEN			94		// Used for topics
#define MAX_ANNOTATIONS			256
#define MAX_COMMAND				128

// host highlighting for text mode
#define HH_BOLD_HEADERS		1
#define HH_BOLD_MESSAGES	2

// on connect actions
#define CA_JOINROOM		0
#define CA_ROOMLIST		1
#define CA_NOACTION		2
/////////////////////////////////////////////////////////////////////////////

// Formerly in chatprot.h...

#define SM_SAY			1
#define SM_WHISPER		2
#define SM_THINK		3
#define SM_SHOUT		4
#define SM_ACTION		5
#define SM_SOUND		6
#define SM_AWAY			7
#define SM_HERESINFO	8
#define SM_NOFORMAT		100

#define CX_DISCONNECTED		0
#define CX_INCHANNEL		1
#define CX_CONNECTING		2
#define CX_NOCHANNEL		3
#define CX_CONNECTED		4

#define VM_UNSPECIFIED	0
#define VM_COMICS		1
#define VM_TEXT			2

#define UM_HOST			1
#define UM_SPEAKER		2
#define UM_SPECTATOR	3

// automatic greeting types
#define AGT_NONE		0
#define AGT_WHISPER		1
#define AGT_SAY			2

#define SC_OWNER		'.'
#define SC_HOST			'@'
#define SC_SPECTATOR	'>'
#define SC_HASVOICE		'+'

#define SS_HOST			"@"
#define SS_SPECTATOR	">"

extern COLORREF	linkColor;
#define LINKINDEX	252

BOOL ChatInitialize();
BOOL ChatSendText(CString& str, BYTE byteMode, BOOL bEcho = TRUE, CDWordArray* prgdwFormatting = NULL);
void ChatPreSendText(CString & str, int avID = 0);

void SetMyAvatar(UINT avatarID, BOOL bBroadcast = TRUE);
BOOL SetMyAvatar(const char *avName, BOOL bBroadcast = TRUE);
void StartNewPanel();
void SetPrintOffset(int x, int y);
char *UnConst(const char *);
char *GetToken(char *start, char **nextStart, const char *seps = ",.)");
void ForwardToSayWnd(UINT);
BOOL FLaunchBrowser(const char *);
BOOL ReplaceToken( CString& str, const CString& strToken, const CString& strValue );
void DrawArc(CDC *dc, POINT& start, POINT& end, int radius, BOOL downStroke);
void DrawArc2(CDC *dc, POINT& start, POINT& end, int altitude);
void SetFlag (DWORD &dw, DWORD mask, BOOL value);
const char *GetMyNickName();
void SetMyNameNick(const char *);

// new chat server abstraction
BOOL CommunicationInits();
void InitializeServerConnection();
void InitializeChannelConnection();
BOOL CTCPQuoteString(const char **str);
BOOL CTCPUnQuoteString(const char **str);

#define UNITSPERINCH	1440		// we're using TWIMs
#define DEFAULTDELTA 100
void DrawPoint(CDC *, POINT, int delta = DEFAULTDELTA);

#define STRETCHMODE	STRETCH_HALFTONE

//RamuM, RegKeys
#define szRootRegKeyName	"Software\\Microsoft\\Microsoft Comic Chat"
#define szProfileValName	"Profile"
#define szBaseDirValName	"BaseDir"
#define szArtDirValName		"ArtDir"

// Font info
#define CLIP_DFA_OVERRIDE		0x40				// Necessary for Korean FA Disabling

#define nFontHeight				-14
#define nFontHeightBig			-24
//#define nFontHeightBalloon		-180
#define nFontHeightMini			-150
#define nFontHeightTitle		-576
#define nFontHeightShout		-252
#define nFontWidth				0
#define nFontEscapement			0
#define nFontOrientation		0
#define nFontWeightBalloon		FW_BOLD
#define fnFontWeight			FW_REGULAR
#define fdwFontItalic			FALSE
#define fdwFontUnderline		FALSE
#define fdwFontStrikeOut		FALSE
#define fdwFontCharSet			DEFAULT_CHARSET    // necessary for globalization
#define	fdwFontOutputPrecision	OUT_DEFAULT_PRECIS
#define fdwFontClipPrecision	CLIP_DFA_OVERRIDE
#define fdwFontQuality			DEFAULT_QUALITY
#define fdwFontPitchAndFamily	VARIABLE_PITCH | FF_DONTCARE
#define titleFontWeight			FW_HEAVY

// Internationalization - have _tcs functions do right thing (single executable has to handle multibyte)
#ifndef _MBCS
#define _MBCS
#endif
#ifndef _MB_MAP_DIRECT
#define _MB_MAP_DIRECT
#endif

#define my_isspace(c) ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'))
#define ISTRUE(x) ((x) != 0)	// if argument is a logical true, makes it an official "TRUE".
#define STATUS_CHAR(c)	(((c)==SC_HOST) || ((c)==SC_SPECTATOR) || ((c) == SC_HASVOICE))
#define CHANNELPREFIX(c) (((c) == '#') || ((c) == '%') || ((c) == '&'))

#define MAXARGS	10

typedef struct {
	BOOL bHasPrefix;
	char nick[50];
	char user[50];
	char machine[50];
//	char other[70];
	int nArgs;
	char *args[MAXARGS];
	char *lastString;
} IRCPARSE;


#define MT_CHANNELSEND		1
#define MT_PRIVATEMSG		2
#define MT_WHISPER			3

#define QT_NONE			0
#define QT_ROOMQUERY	1
#define QT_USERQUERY	2
#define QT_IDENT		3
#define QT_KICKDLG		4
#define QT_BANDLG		5
#define QT_MOTD			6
#define QT_JOINPICS		7
#define QT_IGNOREIDENT	8
#define QT_INITIALWHO	9

#define CM_PRIVATE		1
#define CM_HIDDEN		2
#define CM_INVITEONLY	4
#define CM_TOPICHOST	8
#define CM_NOEXTERN		16
#define CM_MODERATED	32
#define CM_USERLIMIT	64
#define CM_CHANNELKEY	128
#define CM_NOFORMAT		256
#define CM_MIC			512

#define PC_IRC			1
#define PC_NM			2

#define F1_SHOWMOTD			1
#define F1_MAXMDI			2
#define F1_RTFCOMIC			4
#define F1_HEADERSEPARATE	8
#define F1_SHOWTABBAR		16

#define EASTER_TIMER	81




