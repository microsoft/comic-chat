// chat.h : main header file for the CHAT application
//
#ifndef __MAINCHAT_H__
#define __MAINCHAT_H__

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols
#include "defines.h"
#include "rules.h"
#include "notif.h"
#include "webreq.h"
#include "doskey.h"
#include "utils.h"
#include "chatsrv.h"
#include "chatprot.h"

extern COLORREF	linkColor;
extern BOOL		g_bOleShuttingDown; // BOOL used by CChatApp::PreTranslateMessage and
									// CDocObjectServerDoc::XDocOleObject::Close

// Forward declarations
class CUserInfo;
class CChatDoc;


class CMacro {
public:
	BOOL	m_bDefined;
	CString	m_strName;
	CString	m_strValue;

    CMacro () { m_bDefined = FALSE; }

	void	UnSerialize(const char *szBuff);
	int		Serialize(char *szBuff, int iBuffLen);
	void	Invoke(const char *szEncodedChannelName = NULL, CUserInfo *pui = NULL, BOOL bInvokedByRule = FALSE, BOOL bInWhisperBox = FALSE);
};

class CHiddenInThreadWnd : public CWnd
{
// Attributes
public:
	void StartModal(CWnd* pForParent);
	void FinishModal(CWnd* pForParent);
protected:
	HWND GetContainer();
};

/////////////////////////////////////////////////////////////////////////////
// CChatApp:
// See chat.cpp for the implementation of this class
//

class CChatApp : public CWinApp
{
public:
	CMenu*			m_pmenuAdmin;
	// CMenu*		m_pmenuMacro;
	// BOOL			m_bIPMacroDone;    // indicates we've already added the macro menu to the ip frame

	BOOL			m_bComicView;
	BOOL			m_bFoundArt;
	BOOL			m_bEmbedded;
	INT				m_iShowBars;
	INT				m_iAutoPage;
	LOGFONT			m_comicsFont;
	LOGFONT			m_textFont;
	COLORREF		m_comicsColor;
	COLORREF		m_textColor;
	CFont			m_fontGui;
	CHAR			m_szGuiFaceName[LF_FACESIZE]; 
	BYTE			m_lfGuiPitchAndFamily;
	INT				m_iFontHeightBalloon;
	INT				m_textSpacing;
	CHARFORMAT		m_cfArray[NFONTS];
	BOOL			m_bCfInitialized;
	BOOL			m_bCfHLInitialized;
	BOOL			m_bShowArrivals;
	INT				m_iHostHighlight;
	BOOL			m_bAllowInvites;
	INT				m_iGreetingType;
	CString			m_strGreetingMesg;
	CMacro			m_macros[NMACROS];
	BOOL			m_bInSearch;
	BOOL			m_bPlaySounds;
	BOOL			m_bNoMIDI;
	BOOL			m_bAcceptNMCalls;
	BOOL			m_bShowIdentity;
	BOOL			m_bListRegistered;
	RECT			m_rectWhisper;
	RECT			m_rectNotifs;
	CChatServiceList m_listChatServices;
	CChatServiceConnector m_SrvConnector;
	CString			m_strConnectedService;
	CString			m_strConnectedServer;
	BOOL			m_bSaveViewMode;
	BOOL			m_bAllowFileTX;
	BYTE			m_charSet;
	UCHAR			m_uFloodInterval;
	UCHAR			m_uFloodCount;
	UCHAR			m_uFloodFlags;
	DWORD			m_flags1;
	DWORD			m_flags0;
	BOOL			m_bLoginNotifsShown;
	BOOL			m_bIconMembers;
	BOOL			m_bPrompt;
	BOOL			m_bAcceptWhispers;
	BOOL			m_bAway;
	BOOL			m_bAwayPrompt;
	BOOL			m_bDoCB32;
	BOOL			m_bDoTest;
	CString			m_strAwayMessage;
	CMapStringToPtr	m_ignores;

	CString			m_myRealName;
	CString			m_myChannel;
	CString			m_strEmail;
	CString			m_strHomePage;
	CString			m_strChatRooms;
	CString			m_myName;			// pretty version of nick (in DBCS)
	CString			m_myNick;			// if IRCX, unicode nick, otherwise as above
	CString			m_myIdent;
	CString			m_myProfile;
	CString			m_myCharacterName;
	CString			m_strUserName;
	PBYTE  			m_pbCoolBarState;
	short			m_nMyIdentLength;
	int				m_iOnConnectAction;
	BOOL			m_bLoadURL;
	CWnd*			m_pWndActiveDialog;
	CDocument*		m_pExitingDoc;
	CCRulesData		m_rulesData;
	CCDynaRules		m_dynaRules;
	CCDelayedRules	m_delayedRules;
	CCDynaNotifs	m_dynaNotifs;
	CPtrArray		m_enterInfos;
	CDosKey			m_doskeyMain;
	CDosKey			m_doskeyWhisper;
	CImageList		m_ImageList;
	CImageList		m_StatusIcons;
	CMapStringToPtr m_mapDownloads;
	CInternetRequestor * m_pNetRequestor;

	HANDLE			m_hNotificationThread;
	HANDLE			m_hShutdownEvent;

	CHAR*			m_szBuffer;
	WCHAR*			m_wszBuffer;
	SHORT			m_nBufferSize;
#ifdef IRCLOG
	FILE *			m_fileIn;
#endif

public:
	CChatApp();
	~CChatApp();

	static BOOL QueryNewPaletteCommon(CWnd* pWnd);
	int AddFavoritesToMenu(CMenu* pMenu, int nMenuOrigItems);
	static UINT __stdcall FavoriteMonitorFunc(LPVOID pParam);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	virtual BOOL PreTranslateMessage( MSG* pMsg );
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
	afx_msg void OnViewAutomations();
	afx_msg void OnViewOptions();
	afx_msg void OnUpdateViewAutomations(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewOptions(CCmdUI* pCmdUI);
	afx_msg void OnUserList();
	afx_msg void OnUpdateCanSearch(CCmdUI* pCmdUI);
	afx_msg void OnAwayToggle();
	afx_msg void OnUpdateAwayToggle(CCmdUI* pCmdUI);
	afx_msg void OnViewTabbar();
	afx_msg void OnUpdateViewTabbar(CCmdUI* pCmdUI);
	afx_msg BOOL OnViewToolBar(UINT nID);
	afx_msg void OnUpdateViewToolBar(CCmdUI* pCmdUI);
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
	afx_msg void OnViewStatuswindow();
	afx_msg void OnViewLoginNotifs();
	afx_msg void OnUpdateViewStatuswindow(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewLoginNotifs(CCmdUI* pCmdUI);
	afx_msg void OnDefineMacro();
	afx_msg void OnConnectError();
	afx_msg void OnConnectConnected();
	afx_msg void OnToolBarDropDown(NMHDR* pnmhdr, LRESULT *plr);
	//}}AFX_MSG
	afx_msg void OnChatroomList();
	afx_msg void OnFavorite(UINT nID);
	afx_msg void OnSoundsOff();
	DECLARE_MESSAGE_MAP()

public:
// Parsed command-line arguments


	BOOL	m_bNoRefresh;
	BOOL	m_bShowMode;
	BOOL	m_bVIPMode;
	BOOL	m_bDisableMOTD;
	int		m_xFrame;
	int		m_yFrame;
	int		m_cxFrame;
	int		m_cyFrame;
	int		m_maxedFrame;
	CString m_lastBackDrop;
	BOOL	m_bAutoDownloadAvatars;
	BOOL	m_bAutoDownloadBackdrops;
	CHiddenInThreadWnd*	m_pWndHiddenInThread;

// search paths
	CString m_strBaseDir;
	CString	m_strAvatarDir;
	CString m_strBackDropDir;
	CString m_strFavoritesDir;
	CString m_strFileTXDir;
	CStringArray m_arrFavorites;

	CString m_strDefaultArtDir;
	CString	m_soundPath;
	CString	m_strStatusPane[2];

// additional methods

public:
	//helper method for status bar strings
	void		SetStatusPaneString(int index, const char *szPane);
	void		SetPrinterResolution();
	CDocument*	OpenDocumentFile(LPCTSTR lpszFileName);
	BOOL		ProcessShellCommand(CCommandLineInfo &cmdLine);
	void		InitializeFonts();
	void		InitializeComicsFonts();
	CString		GetDesktopOrFavorites(BOOL bDesktop);
	HRESULT		HrAllocBuffer(SHORT nMaxMsgLength);
	BOOL		SaveToReg(BOOL bShort);
	virtual int DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt);
	BOOL		StartDownloadingAvatar(CUserInfo* pUserFrom, CChatDoc* pDocFrom, BOOL bInteractive);
	BOOL		StartDownloadingBackdrop(LPCSTR pszBackdrop, LPCSTR pszURL);
	void		HandleDownloadedFiles();
	int			DoModalDlg(CWnd* pDlgOrPropSheet, BOOL bIsPropSheet=FALSE);
	CWnd*		GetInThreadOwner(CWnd* pWndOf);
	BOOL		CreateHiddenInThreadWnd();
	CRoomInfo*	GetRoomInfoFromName(CString strChannelName, PINT piIndex = NULL, BOOL bCloneOK = FALSE, BOOL bNullOK = FALSE);
	INT			AddRoomInfo(CRoomInfo* pEnterInfo);
	void		RemoveRoomInfo(INT iIndex);
	void		CleanRoomInfos();
	void		ContinueConnection();
	void		ResumeConnection();
	void		CompleteConnection();
	void		IsIrcXTimeout();
	void		DoOptionsDialog(BOOL bComicsView, UINT nInitialPageID = 0);

private:
	void		SetBaseDir(const char *fullpath);
	void		InitStatusIcons();
	void		ParseCommandLine(CCommandLineInfo& rCmdInfo);  // use our own  -- old sometimes gets corrupted (which used?)
	BOOL		LoadFromReg();
	void		InitVals();
	void		InitFileTXDir();
	CFrameWnd*	CreateStatusWnd(CMultiDocTemplate *);

public:
	const char *GetBaseDir() { return ((LPCSTR) m_strBaseDir); }
	const char *GetAvatarDir() { return ((LPCSTR) m_strAvatarDir); }
	const char *GetBackDropDir() { return ((LPCSTR) m_strBackDropDir); }
};

extern CChatApp theApp;

BOOL ChatInitialize(SHORT *pnCXKeepServer, BOOL *pbCXPrompt);
void ChatPreSendText(CString & str, int avID = 0);
void SetMyAvatar(UINT avatarID, BOOL bBroadcast = TRUE);
BOOL SetMyAvatar(const char *avName, BOOL bBroadcast = TRUE);
void StartNewPanel();
void SetPrintOffset(int x, int y);
char *UnConst(const char *);
void ForwardToSayWnd(UINT);
BOOL FLaunchBrowser(const char *);
void DrawArc(CDC *dc, POINT& start, POINT& end, int radius, BOOL downStroke);
void DrawArc2(CDC *dc, POINT& start, POINT& end, int altitude);
void SetFlag (DWORD &dw, DWORD mask, BOOL value);
const char *GetMyNickName();
void SetMyNameNick(const char *);
void AvatarTransferError(LPCSTR pszAvatarName);
// new chat server abstraction
BOOL CommunicationInits();
BOOL CTCPQuoteString(const char **str);
BOOL CTCPUnQuoteString(const char **str);
void DrawPoint(CDC *, POINT, int delta = DEFAULTDELTA);
void OnUserListAux(const char *, const char *, const char *);

#endif __MAINCHAT_H__
