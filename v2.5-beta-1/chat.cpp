// chat.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <afxpriv.h>
#include "chat.h"
#include "icchat.h"

//BINDER: include OLE interfaces for Binder compatibility
#include "ui.h"
#include "mfcbind.h"
#include "bindipfw.h"
#include "binddoc.h"
//BINDER_END

#include "userinfo.h"
#include "chatprot.h"
#include "chatDoc.h"
#include "TabBar.h"
#include "MainFrm.h"
#include "ChildFrm.h"
#include "IpFrame.h"
#include "spltchat.h"
#include "chatView.h"
#include "status.h"
#include "tabbar.h"
#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "bodycam.h"
#include "setupdlg.h"
#include "proppage.h"
#include "autopage.h"
#include "notipage.h"
#include "saywnd.h"
#include "roomlist.h"
#include "userlist.h"
#include "IrcProto.H"
#include "motd.h"
#include <winnls.h>
#include "chatver.h"
#include "format.h"
#include "actions.h"
#include "whisprbx.h"
#include <direct.h>
#include <winreg.h>
#include <io.h>

#include "backdrop.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" BYTE	GetCorrectCharSet();
CMenu* GetMainSubMenu(int n);

// proto for static version of AfxOleRegisterTypeLib
BOOL 
	CChat_AfxOleRegisterTypeLib( HINSTANCE hInstance, REFGUID tlid,
								 LPCTSTR pszFileName, LPCTSTR pszHelpDir=NULL);


struct FAVMONITOR
{
	HWND hwnd;
	LPCSTR pszDir;
	HANDLE hFinishEvent;
};


/////////////////////////////////////////////////////////////////////////////
// Convenience macros

#define IsSwitch(c) ((c)=='-' || (c)=='/')

/////////////////////////////////////////////////////////////////////////////
// Statics with global scope

CString strHelpFile;
CUI cui;		// object constructed statically


BOOL g_bOleShuttingDown = FALSE; // BOOL used by CChatApp::PreTranslateMessage and


/////////////////////////////////////////////////////////////////////////////
// CChatApp

BEGIN_MESSAGE_MAP(CChatApp, CWinApp)
	//{{AFX_MSG_MAP(CChatApp)
	ON_COMMAND(ID_HELP_TOPICS, OnHelpTopics)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_UPDATE_COMMAND_UI(ID_SESSION_CONNECT, OnUpdateSessionConnect)
	ON_COMMAND(ID_SESSION_CONNECT, OnSessionConnect)
	ON_COMMAND(ID_SESSION_NEWROOM, OnNewroom)
	ON_UPDATE_COMMAND_UI(ID_SESSION_NEWROOM, OnUpdateNewroom)
	ON_COMMAND(ID_ROOM_CREATEROOM, OnCreateroom)
	ON_COMMAND(ID_SESSION_DISCONNECT, OnDisconnect)
	ON_UPDATE_COMMAND_UI(ID_SESSION_DISCONNECT, OnUpdateDisconnect)
	ON_COMMAND(ID_FAVORITES_OPENFAVORITES, OnFavoritesOpenfavorites)
	ON_COMMAND(ID_VIEW_AUTOMATIONS, OnViewAutomations)
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_AUTOMATIONS, OnUpdateViewAutomations)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIONS, OnUpdateViewOptions)
	ON_COMMAND(ID_USER_LIST, OnUserList)
	ON_UPDATE_COMMAND_UI(ID_USER_LIST, OnUpdateCanSearch)
	ON_COMMAND(ID_AWAY_TOGGLE, OnAwayToggle)
	ON_UPDATE_COMMAND_UI(ID_AWAY_TOGGLE, OnUpdateAwayToggle)
	ON_COMMAND(ID_VIEW_TABBAR, OnViewTabbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TABBAR, OnUpdateViewTabbar)
	ON_COMMAND(ID_MOTD, OnMotd)
	ON_UPDATE_COMMAND_UI(ID_MOTD, OnUpdateMotd)
	ON_COMMAND(ID_IRC_CHAT, OnIrcChat)
	ON_COMMAND(ID_HELP_FREESTUFF, OnHelpFreestuff)
	ON_COMMAND(ID_HELP_PRODUCTNEWS, OnHelpProductnews)
	ON_COMMAND(ID_HELP_FAQ, OnHelpFaq)
	ON_COMMAND(ID_HELP_ONLINESUPPORT, OnHelpOnlineSupport)
	ON_COMMAND(ID_HELP_BESTOFWEB, OnHelpBestofWeb)
	ON_COMMAND(ID_HELP_SEARCHTHEWEB, OnHelpSearchtheWeb)
	ON_COMMAND(ID_HELP_MSHOMEPAGE, OnHelpMsHomepage)
	ON_COMMAND(ID_HELP_RELEASENOTES, OnHelpReleaseNotes)
	ON_COMMAND(ID_VIEW_STATUSWINDOW, OnViewStatuswindow)
	ON_COMMAND(ID_VIEW_LOGINNOTIFS, OnViewLoginNotifs)
	ON_UPDATE_COMMAND_UI(ID_VIEW_STATUSWINDOW, OnUpdateViewStatuswindow)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LOGINNOTIFS, OnUpdateViewLoginNotifs)
	ON_UPDATE_COMMAND_UI(ID_ROOM_CREATEROOM, OnUpdateNewroom)
	ON_COMMAND(ID_CHATROOM_LIST, OnChatroomList)
	ON_UPDATE_COMMAND_UI(ID_CHATROOM_LIST, OnUpdateCanSearch)
	ON_COMMAND(ID_DEFINE_MACRO, OnDefineMacro)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_HELP, OnHelpTopics)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
	ON_COMMAND_EX(ID_VIEW_TOOLBAR_MAIN, OnViewToolBar)
	ON_COMMAND_EX(ID_VIEW_TOOLBAR_MEMBER, OnViewToolBar)
	ON_COMMAND_EX(ID_VIEW_TOOLBAR_TEXT, OnViewToolBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_MAIN, OnUpdateViewToolBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_MEMBER, OnUpdateViewToolBar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR_TEXT, OnUpdateViewToolBar)
	ON_COMMAND(ID_CONNECT_ERROR, OnConnectError)
	ON_COMMAND(ID_CONNECT_CONNECTED, OnConnectConnected)
	ON_NOTIFY(TBN_DROPDOWN, AFX_IDW_TOOLBAR, OnToolBarDropDown)
	ON_COMMAND_RANGE(ID_FAVORITES, ID_FAVORITES_LAST, OnFavorite)
	ON_COMMAND(ID_TURN_OFF_SOUNDS, OnSoundsOff)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatApp construction
CChatApp::CChatApp()
{
	// Place all significant initialization in InitInstance
	m_bNoRefresh = FALSE;
	m_xFrame = m_yFrame = m_cxFrame = m_cyFrame = m_maxedFrame = 0;
	m_bComicView = FALSE;
	m_bShowMode = FALSE;
	m_bVIPMode = FALSE;
	m_bDisableMOTD = FALSE;
	m_textSpacing = TEXT_VIEW_BLANK_NEVER;
	m_bShowArrivals = TRUE;
	m_bAllowInvites = TRUE;
	m_bCfInitialized = FALSE;
	m_bCfHLInitialized = FALSE;
	m_iHostHighlight = HH_BOLD_HEADERS | HH_BOLD_MESSAGES;
	m_iGreetingType = 0;
	m_iShowBars = SB_TOOLBAR_ANY | SB_STATUSBAR;
//	m_bInSearch = FALSE;  -- now set to be FALSE in login
	m_charSet = ANSI_CHARSET;
	m_bPlaySounds = TRUE;
	m_bNoMIDI = FALSE;
	m_bAcceptNMCalls = TRUE;
	m_bShowIdentity = TRUE;
	m_bListRegistered = FALSE;
	SetRectEmpty (&m_rectWhisper);
	SetRectEmpty (&m_rectNotifs);
	m_bSaveViewMode = TRUE;
	m_bAllowFileTX = TRUE;
	m_uFloodFlags = FLOOD_IGNORE;
	m_uFloodCount = 8;
	m_uFloodInterval = 8;
	m_iAutoPage = -1;
	m_flags1 = ~0;
	m_flags0 = 0;
	m_pmenuAdmin = NULL;
	// m_pmenuMacro = NULL;
 	// m_bIPMacroDone = FALSE;
	m_iOnConnectAction = CA_JOINROOM;
	m_bLoadURL = FALSE;
	m_bIconMembers = TRUE;
	m_bPrompt = FALSE;			// do not prompt by default
	m_bAcceptWhispers = TRUE;
	m_bEmbedded = FALSE;
	m_bLoginNotifsShown = FALSE;
	m_bAway = FALSE;
	m_bDoCB32 = FALSE;
	m_bDoTest = FALSE;
	m_szGuiFaceName[0] = '\0';
	m_lfGuiPitchAndFamily = 0;
	m_strDefaultArtDir = "ComicArt";		// in case registry corrupted
	m_pNetRequestor = NULL;
	m_bAutoDownloadAvatars = FALSE;
	m_bAutoDownloadBackdrops = TRUE;
#ifdef IRCLOG
	m_fileIn = NULL;
#endif

	m_ImageList.Create(40, 40, ILC_COLOR, 5, 5);

	LOGFONT	logFontGUI;
	HFONT	hfontGUI = (HFONT) GetStockObject(DEFAULT_GUI_FONT);
	ASSERT(hfontGUI);
	GetObject(hfontGUI, sizeof(LOGFONT), (LPVOID) &logFontGUI);

	// REGISB added 12/11/97
	logFontGUI.lfCharSet = GetCorrectCharSet();
	MatchFont(logFontGUI);

	strcpy(m_szGuiFaceName, logFontGUI.lfFaceName);
	m_lfGuiPitchAndFamily = logFontGUI.lfPitchAndFamily;
	m_fontGui.CreateFontIndirect(&logFontGUI);

	// REGISB added 10/14/97
	SHORT nBufferSize = max(g_nDefaultIOBuff+1, (MAX_FORMATTINGPERBYTE+1)*MAX_INPUTLEN);
	m_szBuffer = new CHAR[nBufferSize];
	m_wszBuffer = new WCHAR[nBufferSize];
	*m_wszBuffer = *m_szBuffer = '\0';
	m_nBufferSize = nBufferSize;

	// REGISB added 11/07/97
	m_nMyIdentLength = 0;	// length of !<username>@<hostname> for myself


	g_bOleShuttingDown = FALSE; // BOOL used by CChatApp::PreTranslateMessage and

	m_pWndActiveDialog = NULL;
	m_pExitingDoc = NULL;

	m_dynaRules.SetEKPFunction((EVENT_KEY_PARAM_FN) bKeyEventParam);
	m_dynaRules.SetERPFunction((EVENT_RND_PARAM_FN) bRndEventParam);
	m_dynaRules.SetGEKPFunction((GET_EVENT_KEY_FN)  StrGetKeyEventParam);
	m_dynaRules.SetGAKPFunction((GET_ACTION_KEY_FN) StrGetKeyActionParam);
	m_dynaRules.SetExecuteActionFunction((EXECUTE_ACTION_FN) bExecuteAction);
	m_dynaRules.SetRuleFailureFunction((RULE_FAILURE_FN) bReportRuleFailure);
	m_dynaRules.SetDaemonQueryFunction((RULEDAEMON_QUERY_FN) bRuleDaemonQuery);
	m_dynaRules.SetRulesData(&m_rulesData);
	m_dynaRules.SetDelayedRules(&m_delayedRules);

	m_delayedRules.SetExecuteActionFunction((EXECUTE_ACTION_FN) bExecuteAction);

	m_dynaNotifs.SetDisplayNotificationsFunction((DISPLAY_NOTIFICATIONS_FN) bDisplayNotifications);
	m_dynaNotifs.SetSignalNewUpdateFunction((SIGNAL_NEW_UPDATE_FN) bSignalNewUpdate);
	m_dynaNotifs.SetDaemonQueryFunction((NOTIFDAEMON_QUERY_FN) bNotifDaemonQuery);

	m_enterInfos.Add((void*) &g_enterInfo);

	srand((unsigned) time(NULL));	// for rules: aSendFileLine & aWhisperFileLine actions

	#ifdef DEBUG
		g_nRulesRefCount = g_nDaemonsRefCount = 0;
	#endif
	
	m_pWndHiddenInThread = NULL;
	m_pbCoolBarState = NULL;

	m_SrvConnector.SetServiceList (&m_listChatServices);

	m_hNotificationThread = NULL;
	m_hShutdownEvent = NULL;
}

CChatApp::~CChatApp()
{
	// nuke admin menu if it exists
	if (m_pmenuAdmin) {
		m_pmenuAdmin->DestroyMenu();
		delete m_pmenuAdmin;
	}
	
	// REGISB 05/05/98 Macros menu is always visible now
	// if (m_pmenuMacro) {
	//	m_pmenuMacro->DestroyMenu();
	//	delete m_pmenuMacro;
	//}

	m_ImageList.DeleteImageList();
	m_StatusIcons.DeleteImageList();

	void CommunicationCleanup();
	CommunicationCleanup();

	if (m_szBuffer)
		delete [] m_szBuffer;
	if (m_wszBuffer)
		delete [] m_wszBuffer;

	m_fontGui.DeleteObject();

//	ASSERT(g_nRulesRefCount == 0);
//	ASSERT(g_nDaemonsRefCount == 0);

	if (m_pWndHiddenInThread)
	{
		delete m_pWndHiddenInThread;
	}

	free (m_pbCoolBarState);

	CleanRoomInfos();
}


HRESULT CChatApp::HrAllocBuffer(SHORT nMaxMsgLength)
{
	if (m_szBuffer)
		delete [] m_szBuffer;
	if (m_wszBuffer)
	{
		delete [] m_wszBuffer;
		m_wszBuffer = NULL;
	}

	m_nBufferSize = 0;

	SHORT nBufferSize = max(nMaxMsgLength+1, (MAX_FORMATTINGPERBYTE+1)*MAX_INPUTLEN);

	if (!(m_szBuffer = new CHAR[nBufferSize]))
		return E_OUTOFMEMORY;

	if (!(m_wszBuffer = new WCHAR[nBufferSize]))
		return E_OUTOFMEMORY;

	*m_wszBuffer = *m_szBuffer = '\0';

	m_nBufferSize = nBufferSize;

	return NOERROR;
}


void CChatApp::InitStatusIcons() {
	m_StatusIcons.Create(IDB_MEMBER, 16, 5, RGB(0, 0, 255));
}


extern "C" void SetMime(DWORD);

void CChatApp::InitializeFonts() {
	InitializeComicsFonts();

	m_textFont.lfHeight = nFontHeight;
	m_textFont.lfWidth = nFontWidth;
	m_textFont.lfEscapement = nFontEscapement;
	m_textFont.lfOrientation = nFontOrientation;
	m_textFont.lfWeight = fnFontWeight;
	m_textFont.lfItalic = fdwFontItalic;
	m_textFont.lfUnderline = fdwFontUnderline;
	m_textFont.lfStrikeOut = fdwFontStrikeOut;
	m_textFont.lfCharSet = m_comicsFont.lfCharSet; // don't need to compute twice
	m_textFont.lfOutPrecision = fdwFontOutputPrecision;
	m_textFont.lfClipPrecision = fdwFontClipPrecision;
	m_textFont.lfQuality = fdwFontQuality;
	m_textFont.lfPitchAndFamily = fdwFontPitchAndFamily;
	strcpy(m_textFont.lfFaceName, "");

	SetMime((DWORD)m_comicsFont.lfCharSet);
}

void CChatApp::InitializeComicsFonts() {
	CString strDefaultFontHeight, strWeight, strFace;

	int		PointsToTwips(int);		// heavy weight function -- just call once

	strDefaultFontHeight.LoadString(IDS_DFLT_COMICSPNTSIZE);
	m_iFontHeightBalloon = PointsToTwips(atoi(strDefaultFontHeight));

	m_comicsFont.lfHeight = m_iFontHeightBalloon;
	m_comicsFont.lfWidth = nFontWidth;
	m_comicsFont.lfEscapement = nFontEscapement;
	m_comicsFont.lfOrientation = nFontOrientation;
	strWeight.LoadString(IDS_COMICS_BOLD_DFLT);
	m_comicsFont.lfWeight = atoi(strWeight);
	m_comicsFont.lfItalic = fdwFontItalic;
	m_comicsFont.lfUnderline = fdwFontUnderline;
	m_comicsFont.lfStrikeOut = fdwFontStrikeOut;
	m_comicsFont.lfCharSet = GetCorrectCharSet();	// REGISB modified 12/09/97
	m_comicsFont.lfOutPrecision = fdwFontOutputPrecision;
	m_comicsFont.lfClipPrecision = fdwFontClipPrecision;
	m_comicsFont.lfQuality = fdwFontQuality;
	m_comicsFont.lfPitchAndFamily = fdwFontPitchAndFamily;
	strFace.LoadString(ID_COMIC_FONT_NAME);
	strcpy(m_comicsFont.lfFaceName, strFace);

	m_textColor = m_comicsColor = GetSysColor(COLOR_WINDOWTEXT);
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CChatApp object

CChatApp theApp;

// This identifier was generated to be statistically unique for your app.
// You may change it if you prefer to choose a specific identifier.

// {241AF500-8FB6-11CF-ADC5-00AA00BADF6F}
static const CLSID clsid =
{ 0x241af500, 0x8fb6, 0x11cf, { 0xad, 0xc5, 0x0, 0xaa, 0x0, 0xba, 0xdf, 0x6f } };


/////////////////////////////////////////////////////////////////////////////
// CChatApp initialization

BOOL CChatApp::InitInstance()
{
	int iarg;
	CString strTmp;

	/*
		There are three paths in NM that can launch CChat. Two of them use CreateProcess and
		the other uses ShellExecute. These two functions differ in how they handle command
		line params for __argv. CreateProcess makes the first param be __argv[0], ShellExecute
		makes it __argv[1]. If CChat is launched remotely or by DCL the __argv loop	below will 
		miss the /cb32 param since it is __argv[0] and the loop starts at __argv[1]. So, I changed 
		it to start at __argv[0].
	 */
	for (iarg=0; iarg < __argc; iarg++ ) {
		if (IsSwitch( __argv[iarg][0] ) ) {
			char *arg = __argv[iarg]+1;
#ifdef CB32SUPPORT
			if (!stricmp(arg, "cb32"))
				m_bDoCB32 = TRUE;
			else
#endif CB32SUPPORT
			if (!stricmp(arg, "test"))
				m_bDoTest = TRUE;
			else if (!stricmp(arg, "S"))
				m_bShowMode = TRUE;
			else if (!stricmp(arg, "V"))
				m_bVIPMode = TRUE;
#ifdef IRCLOG
			else if (*arg == 'F' || *arg == 'f') 
				m_fileIn = fopen(arg+1, "r");
#endif
			else if (!stricmp(arg, "A"))
				// force server Authentication packages in sequence
				serverConn.SetAuthentication (CIrcSocket::authtypeServerPackages);
			else if (('P' == *arg || 'p' == *arg) && ':' == *(arg+1) && '\0' != *(arg+2))
				serverConn.SetAuthentication (CIrcSocket::authtypeCustomPackages, NULL, NULL, arg + 2);
		}
	}

	if (!CommunicationInits()) return FALSE;

	// Initialize fonts...
	InitializeFonts();

	// Various initializations...
	void LoadEmotionStrings();
	LoadEmotionStrings();		// language specific strings from resources

	// Load emotion rules
	void InitializeEmotionRules();
	InitializeEmotionRules();

	m_lastBackDrop.LoadString(IDS_DEFAULT_BACKDROP);
	void InitializeBackDrops();
	InitializeBackDrops();

	InitStatusIcons();

	//	RamuM: LoadFromReg should have got it already
	//  ShankuN 05/01/98 - the base dir is needed during loading of registry 
	//  for default cases, so load it first.
	SetBaseDir(__argv[0]);

	// Load initial values from the registry
	LoadFromReg();

	// REGISB 04/28/98 - we also try to load the "General" & "Samples" rule sets from the resources now --> easy localization
	if (!(m_flags0 & F0_ALREADYRUN))
		m_dynaRules.bLoadRulesFromResource();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#if 0  // not necessary for win32
#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
	AfxMessageBox("Doing dynamic controls\n");
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
	AfxMessageBox("Doing static controls\n");
#endif
#endif

#if 0							// don't use MRU or preview
	LoadStdProfileSettings();  // Load standard INI file options (including MRU)
#endif


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CMultiDocTemplate* pDocTemplate;
	pDocTemplate = new CMultiDocTemplate(
#ifdef CB32SUPPORT
			m_bDoCB32 ? IDR_NM_MAIN : IDR_MAINFRAME,
#else
			IDR_MAINFRAME,
#endif CB32SUPPORT
			RUNTIME_CLASS(CChatDoc),
			RUNTIME_CLASS(CChildFrame),       // custom MDI child frame
			RUNTIME_CLASS(CChatView));
	pDocTemplate->SetServerInfo(
		IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE,
		RUNTIME_CLASS(CInPlaceFrame));
	AddDocTemplate(pDocTemplate);

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
	// 02/27/98 ShankuN - The third parameter to this function, labelled
	// bMultiInstance in MFC docs, actually means the opposite. Passing in FALSE
	// means that a single EXE *does* support multiple instances of the object,
	// while passing in TRUE means each instance must have its own EXE.
	// Strange, but true. We need the latter, because we have app-wide globals
	// that are unsafe for use with shared instances.
	m_server.ConnectTemplate(clsid, pDocTemplate, TRUE);
		// Note: SDI applications register server objects only if /Embedding
		//   or /Automation is present on the command line.

#if 0
	// debug code -- print out args
	char lne[400];
	lne[0] = '\0';
	for (iarg = 0; iarg < __argc; iarg++) {
		char *start = strchr(lne, '\0');
		sprintf(start, "%s ", __argv[iarg]);
	}
	AfxMessageBox(lne);
#endif

	//	RamuM: LoadFromReg should have got it already
	if (m_strBaseDir.IsEmpty())
		SetBaseDir(__argv[0]);

	strTmp.LoadString(IDS_HELP_FILE);
	strHelpFile = m_strBaseDir;
	strHelpFile += "\\";
	strHelpFile += strTmp;
	CWinApp::m_pszHelpFilePath = strHelpFile;

	// create main MDI Frame window
	CMainFrame* pMainFrame = new CMainFrame;
	if (!pMainFrame->LoadFrame(IDR_MAINFRAME))
		return FALSE;
	m_pMainWnd = pMainFrame;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);  // use our own

//	if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew) AfxMessageBox("FileNew");
//	else if (cmdInfo.m_nShellCommand == CCommandLineInfo::FileOpen) {AfxMessageBox("FileOpen"); AfxMessageBox(cmdInfo.m_strFileName);}


	// Check to see if launched as OLE server
	if (cmdInfo.m_bRunEmbedded || cmdInfo.m_bRunAutomated)
	{
		// Register all OLE server (factories) as running.  This enables the
		//  OLE libraries to create objects from other applications.
		COleTemplateServer::RegisterAll();

		// This window has to be created for embedded running.
		if (cmdInfo.m_bRunEmbedded && !CreateHiddenInThreadWnd ())
		{
			return FALSE;
		}

		// Application was run with /Embedding or /Automation.  Don't show the
		//  main window in this case.
		return TRUE;
	}

	// When a server application is launched stand-alone, it is a good idea
	//  to update the system registry in case it has been damaged.
	
	//BINDER:
	//    Binder objects have some special registry entries that
	//    MFC doesn't know about. Instead of calling 
	//    COleTemplateServer::UpdateRegistry, call our special
	//    registration function
	MfcBinderUpdateRegistry(pDocTemplate, OAT_INPLACE_SERVER);
	//END_BINDER

	// DUAL_SUPPORT_START
	// Make sure the type library is registered or dual interface won't work.
	CChat_AfxOleRegisterTypeLib( AfxGetInstanceHandle(), LIBID_iCChatLib, _T("icchat.tlb") );
	// DUAL_SUPPORT_END

	CreateStatusWnd(pDocTemplate);

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;

	// djk - commented this out -- was it a bug only for SDI windows???
//	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
//		m_nCmdShow = -1; // added this to fix mfc bug in ProcessShellCommand;
	SetPrinterResolution();

	// Set maximized if default (Note: need to do this after ProcessShellCommand, since that can change m_nCmdShow.)
	if (m_maxedFrame) m_nCmdShow = SW_SHOWMAXIMIZED;

	// The main window has been initialized, so show and update it.
	pMainFrame->ShowWindow(m_nCmdShow);
	pMainFrame->UpdateWindow();

	AddFavoritesToMenu (GetMainSubMenu (6), 2);

	m_hShutdownEvent = CreateEvent (NULL, TRUE, FALSE, NULL);
	if (m_hShutdownEvent)
	{
		FAVMONITOR * pfm = new FAVMONITOR;
		pfm->hwnd = ::AfxGetMainWnd ()->m_hWnd;
		pfm->pszDir = m_strFavoritesDir;
		pfm->hFinishEvent = m_hShutdownEvent;
		UINT nThreadAddr;
		m_hNotificationThread = (HANDLE)_beginthreadex (NULL, 0, FavoriteMonitorFunc, pfm, 0, &nThreadAddr);
	}

	return TRUE;
}

void CChatApp::ParseCommandLine(CCommandLineInfo& rCmdInfo)
{
	for (int i = 1; i < __argc; i++)
	{
#ifdef _UNICODE
		LPCTSTR pszParam = __wargv[i];
#else
		LPCTSTR pszParam = __argv[i];
#endif
		BOOL bFlag = FALSE;
		BOOL bLast = ((i + 1) == __argc);
		if (pszParam[0] == '-' || pszParam[0] == '/')
		{
			// remove flag specifier
			bFlag = TRUE;
			++pszParam;
		}
		rCmdInfo.ParseParam(pszParam, bFlag, bLast);
	}
}


void CChatApp::SetStatusPaneString(int index, const char *szPane)
{
	m_strStatusPane[index] = szPane;

	if (m_pActiveWnd != NULL)
	{
		CString strStatus;
		strStatus = m_strStatusPane[0]+"      "+m_strStatusPane[1];
		((CInPlaceFrame*)m_pActiveWnd)->SetStatusString(strStatus);
	}
	if (m_pMainWnd != NULL)
		((CMainFrame*)m_pMainWnd)->m_wndStatusBar.SetPaneText(index,szPane);
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Add a CBrush* to store the new background brush for the controls.
	CBrush* m_pBkBrush;

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

// Implementation
protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	//Instantiate and initialize background brush to white.
	m_pBkBrush=new CBrush(RGB(255,255,255));
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CChatApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	DoModalDlg (&aboutDlg);
}

/////////////////////////////////////////////////////////////////////////////
// Chat Help menu handler

void CChatApp::OnHelpTopics()
{
	AfxGetApp()->WinHelp(1, HELP_FINDER);
}

/////////////////////////////////////////////////////////////////////////////
// CChatApp commands

int CChatApp::ExitInstance() 
{
	if (m_hShutdownEvent)
	{
		SetEvent (m_hShutdownEvent);
		if (m_hNotificationThread)
		{
			WaitForSingleObject (m_hNotificationThread, INFINITE);
			CloseHandle (m_hNotificationThread);
		}
		CloseHandle (m_hShutdownEvent);
	}

	void DestroyEmotionRules();
	DestroyEmotionRules();				// Free the emotion spotting rules

	void CleanupComicsDataOnExit();
	CleanupComicsDataOnExit();

	DestroyWhisperBox();

	DestroyNotificationBox();

	void DestroyExternalUserInfos();
	DestroyExternalUserInfos();

	void CleanupFileProgressStore(BOOL);
	CleanupFileProgressStore(TRUE);

	// We can't just delete the net requestor, we have to shut it down properly.
	if (m_pNetRequestor != NULL) {
		m_pNetRequestor->ShutDown ();
		delete m_pNetRequestor;
		m_pNetRequestor = NULL;
	}

	SaveToReg(FALSE /*bShort*/);

	return CWinApp::ExitInstance();
}

void GetVersionString(CString &str) {
	str.LoadString(ID_SIMPLE_VERSION);

	// replace the placeholders with true version
	CString entry;
	VERIFY(ReplaceToken(str, CString("%1"), CString(VER_PRODUCTVERSION_STR)));
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CStatic *pwnd;
	CString str, strNum, strRegName, strRegCorp;
	CDC *dlgDC;
	char buff[200];
	DWORD cbData = 0;
	HKEY	hKey = NULL;
	BOOL	bFoundName = FALSE;

	// Set the background for the dialog to white to prevent flashing
	dlgDC=GetDC();
	dlgDC->SetBkColor(RGB(255,255,255));
	ReleaseDC(dlgDC);

	// Put TIKI on the bottom.
	VERIFY(pwnd = (CStatic *) GetDlgItem( IDC_TIKI ));
	pwnd->SetWindowPos( this, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	GetVersionString(str);
	VERIFY(pwnd = (CStatic *) GetDlgItem(IDC_VERSION));
	// put the text into the control
	pwnd->SetWindowText( str );
	// put control on top of the TIKI bitmap
	pwnd->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	// Read the registration information from the registry and insert it into the dialog.
	// The user registration info can be in one of several places. We go through each
	// one until we find something.

	static const HKEY hkeyRegLook[] = { HKEY_CURRENT_USER, HKEY_LOCAL_MACHINE, HKEY_LOCAL_MACHINE };
	static const LPCSTR pszRegLook[] = {
		"Software\\Microsoft\\MS Setup (ACME)\\User Info", 
		"Software\\Microsoft\\Windows NT\\CurrentVersion",
		"Software\\Microsoft\\Windows\\CurrentVersion",
	};
	static const LPCSTR pszRegLookUserName[] = { "DefName", "RegisteredOwner", "RegisteredOwner" };
	static const LPCSTR pszRegLookUserOrg[] = { "DefCompany", "RegisteredOrganization", "RegisteredOrganization" };

	for (int iReg = 0; !bFoundName && iReg < _countof(hkeyRegLook); iReg++)
	{
		if (RegOpenKeyEx (hkeyRegLook[iReg], pszRegLook[iReg], 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
		{
            *buff = '\0';
			cbData = sizeof(buff);
			RegQueryValueEx (hKey, pszRegLookUserName[iReg], 0, NULL, (unsigned char *)buff, &cbData);
			strRegName = buff;
			*buff = '\0';
			cbData = sizeof(buff);
			RegQueryValueEx (hKey, pszRegLookUserOrg[iReg], 0, NULL, (unsigned char *)buff, &cbData);
			strRegCorp = buff;
			if (!strRegName.IsEmpty () || !strRegCorp.IsEmpty ())
			{
				bFoundName = TRUE;
			}
		}
	} 
	if (bFoundName) {
		GetDlgItem(IDC_USER)->SetWindowText(strRegName);
		// put window on top of the TIKI bitmap
		GetDlgItem(IDC_USER)->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

		GetDlgItem(IDC_CORP)->SetWindowText(strRegCorp);
		// put window on top of the TIKI bitmap
		GetDlgItem(IDC_CORP)->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}

	CString strWarn;
	strWarn.LoadString(IDS_WARNING_TEXT);
	GetDlgItem(IDC_WARNING)->SetWindowText(strWarn);

	// put the rest of the dialog item windows on top of the TIKI bitmap
	GetDlgItem(IDC_WARNING)->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	GetDlgItem(IDC_COPY)->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	GetDlgItem(IDC_LICENSE)->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	GetDlgItem(IDOK)->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	return CDialog::OnInitDialog();
}

#if 0
void CDUpOne(const char *fullpath) {
	char buff[200];
	const char *start = fullpath, *penult = NULL, *ult = NULL;
	while (1) {
		char *next = strchr(start, '\\');
		if (next) {
			penult = ult;
			ult = next;
			start = next+1;
		} else break;
	}
	ASSERT(penult);
	int nchars = penult - fullpath;
	strncpy(buff, fullpath, nchars);
	buff[nchars] = '\0';
	_chdir(buff);
}
#endif

// sets the chatapp's base directory to be the base of fullpath
void CChatApp::SetBaseDir(const char *fullpath) {
	char buff[200];
	const char *start = fullpath, *ult = NULL;
	while (1) {
		char *next = strchr(start, '\\');
		if (next) {
			ult = next;
			start = next+1;
		} else break;
	}
	ASSERT(ult);
	int nchars = ult - fullpath;
	strncpy(buff, fullpath, nchars);
	buff[nchars] = '\0';
	m_strBaseDir = buff;
}



BOOL CChatApp::OnIdle(LONG lCount) 
{
//	SetStatusPaneString(0,m_strStatusPane[0]);
	if (GetDefaultProto()) GetDefaultProto()->OnIdle(lCount);

	// Check for completed file downloads.
	if (lCount == 1 && m_pNetRequestor != NULL && 
			m_pNetRequestor->IsOutBoxNonEmpty ()) {
		HandleDownloadedFiles ();
	}

	return CWinApp::OnIdle(lCount);
}



BOOL  CChatApp::PreTranslateMessage( MSG* pMsg )
// This is a debug hook for catching a really weird condtion that was occuring before
// I fixed the UIActivate... handshaking with XCChat. I left it in here just for 
// informations sake if it happens again. This function and the simular ones in 
// mainfrm.cpp can probably be removed at some point down the road.
//
// This is only active after CDocObjectServerDoc::XDocOleObject::Close has been called 
// for DEBUG builds. The rest of the time it just passes everything
{

#ifdef _DEBUG
#if 0
	// are we shuting down an embedded server?
	if( g_bOleShuttingDown )
	{
		CMainFrame *pMainWnd = (CMainFrame *)AfxGetMainWnd();
		HWND hwndActive;
		HWND hwndMDI;

		if( pMainWnd != NULL )
		{
			hwndMDI = pMainWnd->m_hWndMDIClient;
			if( hwndMDI != NULL )
			{
				hwndActive = (HWND)::SendMessage( hwndMDI, WM_MDIGETACTIVE, 0, NULL );
				if( hwndActive != NULL )
				{
					if( !IsWindow( hwndActive ) )
					{
					// bad news, man....
					TRACE1( "BAD MDI CLIENT ACTIVE WINDOW HANDLE %x\n", hwndActive );
					ASSERT( 0 );
					}
				}
			}
		}
	}
#endif
#endif

	// pass message on to MFC
	return( CWinApp::PreTranslateMessage( pMsg ) );
}




//Overrode default behavior to designate between .ccr and .ccc
void CChatApp::OnFileOpen() 
{
	CString strFinal, strFilter;
	strFilter.LoadString(IDS_CCC_FILTER);
	
	if (GetChatDoc()) strFinal = GetChatDoc()->GetPathname();

	if(strFinal.IsEmpty())
	{
		LONG lResult;
		CString strSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\App Paths\\CChat.exe";
		HKEY hExe;
		BYTE Data[256];
		DWORD cbData = 256;
		DWORD type;
	    char drive[_MAX_DRIVE];
	    char dir[_MAX_DIR];
	    char fname[_MAX_FNAME];
	    char ext[_MAX_EXT];

		lResult = RegOpenKeyEx(HKEY_LOCAL_MACHINE,LPCTSTR(strSubKey),0,KEY_QUERY_VALUE,&hExe);

		lResult = RegQueryValueEx(hExe,"",0,&type,Data,&cbData);

		_splitpath((char *)Data , drive, dir, fname, ext );

		CString strDir(dir);
		CString strFinal(drive);
		strFinal+= strDir;
	}

	//ToDo : Have the ".ccc" as a common string including the filter RamuM
	CFileDialog dlgFile( TRUE,"ccc",strFinal, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter);
	
	if(DoModalDlg (&dlgFile) == IDOK)
	{
		CString newName = dlgFile.GetPathName();
		OpenDocumentFile(newName);
	}
}

void CChatApp::SetPrinterResolution()
{
	PRINTDLG FAR * pPrintDlg = new PRINTDLG;
 
	// Get the current printer's settings
	if(AfxGetApp()->GetPrinterDeviceDefaults(pPrintDlg))
	{
		// Get pointer to the DEVMODE structure
		DEVMODE FAR *lpDevMode = (DEVMODE FAR*)::GlobalLock(pPrintDlg->hDevMode);
		if(lpDevMode)
		{
			lpDevMode->dmFields |= (DM_PRINTQUALITY | DM_YRESOLUTION);
        
		// If the printer initializes the dmYResolution member, the dmPrintQuality
		// member specifies the x-resolution of the printer, in dots per inch.              
			lpDevMode->dmPrintQuality = DMRES_MEDIUM;  // we could set this to be device dependent 150
//			lpDevMode->dmYResolution = 150;
		}
      
		// Unlock the pointers to the setting structures
		::GlobalUnlock(pPrintDlg->hDevMode);
	}

	// Clean up
	delete pPrintDlg;
}

CDocument* CChatApp::OpenDocumentFile(LPCTSTR lpszFileName)
{
	CDocument* pDoc = CWinApp::OpenDocumentFile(lpszFileName);
#if 0  // necessary (from EricG)
	if(pDoc == NULL)  //then we better ask for a new doc.
		if(AfxGetMainWnd())
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
#endif
	return pDoc;
}

// Intercept the OnCtlColor and OnDestroy messages for the controls in the dialog to force
// them to have a white background.

HBRUSH CAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	switch (nCtlColor){
	case CTLCOLOR_EDIT:
	case CTLCOLOR_MSGBOX:
	case CTLCOLOR_STATIC:
		pDC->SetTextColor(RGB(0,0,0));
		pDC->SetBkColor(RGB(255,255,255));
		return (HBRUSH) (m_pBkBrush->GetSafeHandle());
	default:
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
	}
}

void CAboutDlg::OnDestroy() 
{
	CDialog::OnDestroy();
	
	//Free the extra brush.
	delete m_pBkBrush;	
}

static const char *textPostFix =		"__text";
static const char *comicsPostFix =		"__comics";
static const char *passwdSeparator =	"___";

// url form: mic://server/room__postFix___password
// postFix and password are optional (and usually not used). postFix must precede password
void ParseRoomComponent(const char *roomURL, CString &strRoom, CString &password) {
	extern int g_iViewMode;
	char *end;

	if (end = strstr(roomURL, passwdSeparator)) {	// parse off password
		strRoom = CString(roomURL, end - roomURL);
		password = end + strlen(passwdSeparator);
	}

	if (end = strstr(roomURL, textPostFix)) {
		strRoom = CString(roomURL, end - roomURL);
		g_iViewMode = VM_TEXT;
		theApp.m_bSaveViewMode = FALSE;		// don't change registry view default!!!
	} else if (end = strstr(roomURL, comicsPostFix)) {
		strRoom = CString(roomURL, end - roomURL);
		g_iViewMode = VM_COMICS;
		theApp.m_bSaveViewMode = FALSE;		// don't change registry view default!!!
	}

	if (strRoom.IsEmpty()) strRoom = roomURL;		// common case
}

BOOL CChatApp::ProcessShellCommand(CCommandLineInfo &cmdLine) {
	BOOL FIsURL(TCHAR *szURL,int *pISep,int *pcchURL);
	CString url;

//	AfxMessageBox("ProcessingShellCommand...\n");
	// if it's a url, we set up the room params appropriately, and sub in a FileNew
	if (cmdLine.m_nShellCommand == CCommandLineInfo::FileOpen) {
//		AfxMessageBox("Got a FileOpen");
		const char *start = cmdLine.m_strFileName;
		// try stripping quotes (If launched from IE, URL in quotes)
		int len = strlen(cmdLine.m_strFileName);
		if (*start == '"' && start[len-1] == '"')
			url = CString(start+1, len-2);
		else url = start;

//		AfxMessageBox(url);

		if (strncmp(url, "mic://", 6)==0 ||  strncmp(url, "irc://", 6)==0) {	// Process URL if necessary (does url contain a url?)
			start = url;
			if (start) {
				void ChatSetServer(const char *);
				start += 6;  // size of mic:// or irc://
				const char *end = strchr(start, '/');
				if (end) {
					ChatSetServer(CString(start, end-start));
					end++;
					CString room, password;
					ParseRoomComponent(end, room, password);
					bInitEnterInfo(g_enterInfo, room, password, NULL, 0L, TRUE);	// REGISB Fix me if returns FALSE!
					if (GetChatDoc()) GetChatDoc()->m_fileType = FT_CCR;
					ChatSetCXPrompt(FALSE);
					m_bLoadURL = TRUE;
					cmdLine.m_nShellCommand = CCommandLineInfo::FileNew;
				}
			}
		} else {
			// convert to long filename (OK if was long)
			struct _finddata_t fdata;
			char drive[_MAX_DRIVE], dir[_MAX_DIR], fname[_MAX_FNAME], ext[_MAX_EXT], full[_MAX_PATH];
			_splitpath(url, drive, dir, fname, ext);  // note: url has cmdLine.m_strFileName, but guaranteed no surrounding quotes!!!
			long hFindRoom = _findfirst( (char *) (const char *) url, &fdata);
			if (hFindRoom != -1) {
				_makepath(full, drive, dir, fdata.name, NULL);
				cmdLine.m_strFileName = full;
				_findclose (hFindRoom);
			}
		}
	}
	return CWinApp::ProcessShellCommand(cmdLine);
}


void CChatApp::OnUpdateSessionConnect(CCmdUI* pCmdUI) 
{
	int status = GetIrcProto()->GetConnectionStatus();
	pCmdUI->Enable(status == CX_DISCONNECTED);
}

void CChatApp::OnSessionConnect() 
{
	extern CPtrList g_docs;
	if (g_docs.GetCount() < 2)
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
	else
		InitializeServerConnection(&g_enterInfo, &g_bCXPrompt);
}

void CChatApp::OnNewroom() 
{
	void ChatSwitchChannel(const char *);
	ChatSwitchChannel(NULL);
}

void CChatApp::OnUpdateNewroom(CCmdUI* pCmdUI) 
{
	int status = GetIrcProto()->GetConnectionStatus();
	pCmdUI->Enable(status == CX_INCHANNEL || status == CX_NOCHANNEL);
}

void CChatApp::OnCreateroom() 
{
	ChatCreateRoom(g_enterInfo);
}

void CChatApp::OnDisconnect() 
{
	ChatServerDisconnect(TRUE /*bCheckRules*/);
}

void CChatApp::OnUpdateDisconnect(CCmdUI* pCmdUI) 
{
	int status = GetIrcProto()->GetConnectionStatus();
	pCmdUI->Enable(status != CX_DISCONNECTED);
}

void CChatApp::OnFavoritesOpenfavorites() 
{
	BOOL DoEasterEgg();
	if (DoEasterEgg()) return;

	CString strFavorites = m_strFavoritesDir +"\\*.ccr";
	CString strFilter;
	strFilter.LoadString(IDS_CCR_FILTER);

	CFileDialog dlgFile( TRUE,"ccr",strFavorites, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		strFilter);
	
	if(DoModalDlg (&dlgFile) == IDOK) {
		CString newName = dlgFile.GetPathName();
		OpenDocumentFile(newName);
	}	
}

// returns string containing path to desktop (TRUE) or favorites (FALSE)
CString CChatApp::GetDesktopOrFavorites(BOOL bDesktop)
{
	LONG lResult;
	CString strSubKey = "Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders";
	HKEY hShellFolders;
	BYTE Data[MAX_PATH];
	DWORD cbData = sizeof(Data);
	DWORD type;

	lResult = RegOpenKeyEx (HKEY_CURRENT_USER,LPCTSTR(strSubKey),0,KEY_QUERY_VALUE,&hShellFolders);
	if(bDesktop)
		lResult = RegQueryValueEx (hShellFolders,"Desktop",0,&type,Data,&cbData);
	else {
		lResult = RegQueryValueEx (hShellFolders,"Favorites",0,&type,Data,&cbData);
		if (lResult != ERROR_SUCCESS) {   // No favorites.  Installed w/o IE?  Create Favorites under basedir
			CString dir;
			dir.LoadString (IDS_FAVORITES_NAME);
			CString path;
			path.Format ("%s\\%s", (LPCSTR)theApp.m_strBaseDir, (LPCSTR)dir);
			strcpy((char *)Data, path);
		}
		// If the favorites directory doesn't exist, create it.
		if (GetFileAttributes ((LPCSTR)Data) == (DWORD)-1L)
			CreateDirectory ((LPCSTR)Data, NULL);
	}

	return CString(Data);
}

void 
CChatApp::DoOptionsDialog(
BOOL bComicsMode,
UINT nInitialPageID)
{
	CCSPropertySheet	psOptions(IDS_OPTIONS);
	CSettingsPage		ppSettings;
	CComicsPropPage		ppComics;
	CCharacterPage		ppCharacter;
	CBackgroundPage		ppBackground;
   #ifdef CB32SUPPORT
	CTextFontPage		ppTextFont(theApp.m_bDoCB32 ? IDD_TEXTFONTPAGE_NM : IDD_TEXTFONTPAGE_IRC);
	CPersonalPage		ppPersonal(theApp.m_bDoCB32 ? IDD_PERSONALPAGE_NM : IDD_PERSONALPAGE_IRC);
   #else
	CTextFontPage		ppTextFont(IDD_TEXTFONTPAGE_IRC);
	CPersonalPage		ppPersonal(IDD_PERSONALPAGE_IRC);
   #endif CB32SUPPORT
	CServersPage		ppServers;

	psOptions.m_psh.dwFlags &= ~(PSH_HASHELP);  // deactivate help button

	psOptions.AddPage(&ppPersonal);

   #ifdef CB32SUPPORT
	if (!theApp.m_bDoCB32)
   #endif CB32SUPPORT
		psOptions.AddPage(&ppSettings);

	if (bComicsMode) {
		psOptions.AddPage(&ppComics);
		psOptions.AddPage(&ppCharacter);
		psOptions.AddPage(&ppBackground);
	} else {
		psOptions.AddPage(&ppTextFont);
	}

	psOptions.AddPage (&ppServers);

	if (nInitialPageID != 0)
	{
		int nPage = psOptions.GetPageIndexFromID (nInitialPageID);
		if (nPage != -1)
			psOptions.SetActivePage (nPage);
	}

	DoModalDlg (&psOptions, TRUE); // It's a property sheet not a dialog.
	if (GetChatDoc()) GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}

void CChatApp::OnViewOptions() 
{
	BOOL comicView = GetChatDoc() ? GetChatDoc()->m_bComicView : theApp.m_bComicView;
	DoOptionsDialog (comicView);
}

//Note that OnDefineMacro simply calls OnViewAutomations, since macros
// are on the first page.  If this changes, we'll need to parameterize this
// routine to take a start page number (call SetActivePage(n) before DoModal)

void CChatApp::OnViewAutomations() 
{
	CCSPropertySheet	psAutomations(IDS_AUTOMATIONS);
	CAutomationPage		apAutomation;
	CNotificationsPage	apNotifs;
	CRuleSetsPage		apRuleSets;
	CRulesPage			apRules;
	CCDynaRules			m_dynaRulesCopy;
	CCDynaNotifs		m_dynaNotifsCopy;

	psAutomations.m_psh.dwFlags &= ~(PSH_HASHELP);  // deactivate help button

	psAutomations.AddPage(&apAutomation);

	if (!theApp.m_bDoCB32)
	{
		VERIFY(theApp.m_rulesData.bInitAlloc());
		VERIFY(theApp.m_rulesData.bLoadStrings());

		m_dynaRulesCopy = theApp.m_dynaRules;
		m_dynaNotifsCopy = theApp.m_dynaNotifs;

		apNotifs.SetDynaNotifs(&m_dynaNotifsCopy);
		apRuleSets.SetDynaRules(&m_dynaRulesCopy);
		apRules.SetDynaRules(&m_dynaRulesCopy);

		psAutomations.AddPage(&apNotifs);
		psAutomations.AddPage(&apRuleSets);
		psAutomations.AddPage(&apRules);
	}

	if (m_iAutoPage >= 0 && m_iAutoPage < psAutomations.GetPageCount())
		psAutomations.SetActivePage(m_iAutoPage);
	m_iAutoPage = -2;
	DoModalDlg(&psAutomations, TRUE);
	m_iAutoPage = -1;

	theApp.m_dynaNotifs.SetStartUpIdent("");
	
	if (GetChatDoc())
		GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}


void CChatApp::OnUpdateViewAutomations(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!currentRoom || currentRoom->GetConnectionStatus() != CX_CONNECTING);
}


void CChatApp::OnUpdateViewOptions(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(!currentRoom || currentRoom->GetConnectionStatus() != CX_CONNECTING);
}


void CChatApp::OnUpdateCanSearch(CCmdUI* pCmdUI) 
{
	int iConnect = GetIrcProto()->GetConnectionStatus();
	pCmdUI->Enable(!theApp.m_bInSearch && (iConnect == CX_INCHANNEL || iConnect == CX_NOCHANNEL));
}

static CRoomListPersist roomPersist;

void OnChatroomListAux(const char *query = NULL) {
	if (!serverConn.m_bIrcXServer && !bCanViewUnrated(TRUE)) return;   // PICS test for non-IRCX servers

	if (query || !roomPersist.m_strQuery.IsEmpty()) roomPersist.m_cachedServer = ""; // force reset if this is a query or roomlist holds a query
	roomPersist.m_strQuery = query ? query : "";
	CRoomList roomDlg(&roomPersist);
	cui.m_pvRoomList = &roomDlg;		// cache this info for irc.cpp
	theApp.DoModalDlg (&roomDlg);
	cui.m_pvRoomList = NULL;
	if (GetChatDoc()) GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}

void CChatApp::OnChatroomList() 
{
	OnChatroomListAux();
}

static CUserListPersist userPersist;
#define LAUNCH_WHISPERBOX	8181

void OnUserListAux(const char *query = NULL, const char *encRoom = NULL, const char *prettyRoom = NULL)
{
	if (!bCanViewUnrated(TRUE))
		return;   // PICS test for all users

	if (query || !userPersist.m_strQuery.IsEmpty())
		roomPersist.m_cachedServer = ""; // force reset if this is a query or userlist holds a query

	userPersist.m_strQuery = query ? query : "";

	if (encRoom)
	{
		extern const char *DecodeChan(const char *, BOOL = FALSE);
		userPersist.m_strEncRoom = encRoom;
		userPersist.m_searchType = USERSEARCH_ROOM;
		userPersist.m_strRoomFilter = prettyRoom ? prettyRoom : "";
	}
	else
		userPersist.m_strEncRoom = "";

	CUserList userDlg(&userPersist);
	cui.m_pvUserList = &userDlg;

	if (theApp.DoModalDlg(&userDlg) == LAUNCH_WHISPERBOX)
	{
		if (userDlg.m_selUser)  // must be set, but check anyway
			WhisperBox(userDlg.m_selUser);
	}
	else
		if (GetChatDoc())
			GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)

	cui.m_pvUserList = NULL;

	// if this listing was triggered by a List Members... click in the room list dialog 
	// we need to re-enable the button
	if (cui.m_pvRoomList)
	{
		CRoomList*	pRoomList = (CRoomList*) cui.m_pvRoomList;
		CWnd*		pBtn1 = pRoomList->GetDlgItem(IDC_RESET_LIST);
		CWnd*		pBtn2 = pRoomList->GetDlgItem(IDC_LISTMEMBERS);
		
		if (pBtn1 && pBtn2)
		{
			pBtn2->EnableWindow(TRUE);
			pRoomList->GotoDlgCtrl(pBtn1);
			pRoomList->NextDlgCtrl();
		}
	}
}


void CChatApp::OnUserList() 
{
	OnUserListAux();
}



void CChatApp::OnAwayToggle() 
{
	CAwayDlg away;
	BOOL bToggle = FALSE;

	if (!m_bAway)
	{
		if (m_strAwayMessage.IsEmpty())				   // use default string if never set
			m_strAwayMessage.LoadString(IDS_DFLTAWAYMSG);
		
		ASSERT(!away.m_rtfAwayMsg.m_prgdwFormatting);
		away.m_rtfAwayMsg.m_prgdwFormatting = new CDWordArray;

		char*	szControlFull = strdup((LPCTSTR) m_strAwayMessage);
		char*	szControlLess = SzControlLess(szControlFull, away.m_rtfAwayMsg.m_prgdwFormatting);

		away.m_rtfAwayMsg.m_strText = CString(szControlLess);
		free(szControlFull);
		
		away.m_rtfAwayMsg.m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
		away.m_rtfAwayMsg.DefineDefaultCharFormat();

		if (DoModalDlg(&away) == IDOK)
		{
			bToggle = TRUE;
			
			CString strControlFull = away.m_rtfAwayMsg.m_strText;  // can't be empty do to OK diabling
			if (away.m_rtfAwayMsg.m_prgdwFormatting)
			{
				char* szCtrlFull = SzControlFull((LPCTSTR) away.m_rtfAwayMsg.m_strText, away.m_rtfAwayMsg.m_prgdwFormatting);
				if (szCtrlFull)
				{
					strControlFull = CString(szCtrlFull);
					delete [] szCtrlFull;
				}
			}
			m_strAwayMessage = strControlFull;
		}
	}
	else
		bToggle = TRUE;

	if (bToggle)
	{
		m_bAway = !m_bAway;
		m_bAwayPrompt = m_bAway;	// we need to prompt on first user input
		
		GetDefaultProto()->ChatSetAway(m_bAway, m_strAwayMessage);
	}
}


void CChatApp::OnUpdateAwayToggle(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bAway);
	int iStatus = GetDefaultProto() ? GetDefaultProto()->GetConnectionStatus() : CX_DISCONNECTED;
	pCmdUI->Enable(iStatus == CX_NOCHANNEL || iStatus == CX_INCHANNEL);
}

void CChatApp::OnViewTabbar() 
{
	CControlBar* pBar = GetTabBar();
	if (pBar) {
		BOOL bShow = ((pBar->GetStyle() & WS_VISIBLE) == 0);
		GetFrame()->ShowControlBar(pBar, bShow, FALSE);
		theApp.m_flags1 = bShow ? (theApp.m_flags1 | F1_SHOWTABBAR) : (theApp.m_flags1 & ~F1_SHOWTABBAR);
	}

}

void CChatApp::OnUpdateViewTabbar(CCmdUI* pCmdUI) 
{
	CControlBar* pBar = GetTabBar();
	if (pBar) pCmdUI->SetCheck((pBar->GetStyle() & WS_VISIBLE) != 0);
}

BOOL 
CChatApp::OnViewToolBar(
UINT nID)
{
	UINT nWhich;
	switch (nID)
	{
		case ID_VIEW_TOOLBAR_MAIN:
			nWhich = CHAT_TOOLBAR_MAIN;
			break;
		case ID_VIEW_TOOLBAR_MEMBER:
			nWhich = CHAT_TOOLBAR_MEMBER;
			break;
		case ID_VIEW_TOOLBAR_TEXT:
			nWhich = CHAT_TOOLBAR_TEXT;
			break;
		default:
			return FALSE;
	}

	GetToolBar ()->ToggleBar (nWhich);
	return TRUE;
}

void 
CChatApp::OnUpdateViewToolBar(
CCmdUI* pCmdUI)
{
	UINT nFlag;
	switch (pCmdUI->m_nID)
	{
		case ID_VIEW_TOOLBAR_MAIN:
			nFlag = SB_TOOLBAR_MAIN;
			break;
		case ID_VIEW_TOOLBAR_MEMBER:
			nFlag = SB_TOOLBAR_MEMBER;
			break;
		case ID_VIEW_TOOLBAR_TEXT:
			nFlag = SB_TOOLBAR_TEXT;
			break;
		default:
			return;
	}

	pCmdUI->SetCheck ((m_iShowBars & nFlag) != 0);
}

void CChatApp::OnIrcChat() 
{
	if (AfxMessageBox(IDS_IRCWELCOME, MB_YESNO) == IDYES)
		ShellExecute(GetFrame()->m_hWnd, NULL, "cchat.exe", NULL, NULL, SW_SHOWNORMAL);	
}

extern void LaunchMicrosoftURL(UINT resourceID);

void CChatApp::OnHelpFreestuff() 
{
	LaunchMicrosoftURL(IDS_URL_FREESTUFF);	
}

void CChatApp::OnHelpProductnews() 
{
	LaunchMicrosoftURL(IDS_URL_PRODUCTNEWS);
}

void CChatApp::OnHelpFaq() 
{
	LaunchMicrosoftURL(IDS_URL_FAQ);	
}

void CChatApp::OnHelpOnlineSupport() 
{
	LaunchMicrosoftURL(IDS_URL_ONLINESUPPORT);
}

void CChatApp::OnHelpBestofWeb() 
{
	LaunchMicrosoftURL(IDS_URL_BESTOFWEB);	
}

void CChatApp::OnHelpSearchtheWeb() 
{
	LaunchMicrosoftURL(IDS_URL_SEARCHTHEWEB);	
}

void CChatApp::OnHelpMsHomepage() 
{
	LaunchMicrosoftURL(IDS_URL_MSHOMEPAGE);	
}

void CChatApp::OnHelpReleaseNotes()
{
	CString strReleaseNotes;
	char	*szTmp;
	
	strReleaseNotes.LoadString(IDS_TEXTFILES);
	strReleaseNotes = theApp.m_strBaseDir + "\\" + GetToken((LPTSTR) (LPCTSTR) strReleaseNotes, &szTmp) + szTxtExt;
	ShellExecute(GetFrame()->m_hWnd, NULL, strReleaseNotes, NULL, NULL, SW_SHOWNORMAL);	
}


// CLONED FROM MFC4.1 SOURCES SINCE THIS IS NOT IN THE MFC40 STATIC LIBS
BOOL 
	CChat_AfxOleRegisterTypeLib( HINSTANCE hInstance, REFGUID tlid,
								 LPCTSTR pszFileName, LPCTSTR pszHelpDir)
{
	USES_CONVERSION;

	BOOL bSuccess = FALSE;
	CString strPathName;
	::GetModuleFileName(hInstance, strPathName.GetBuffer(_MAX_PATH), _MAX_PATH);
	strPathName.ReleaseBuffer();

	// If a filename was specified, replace final component of path with it.
	if (pszFileName != NULL)
	{
		int iBackslash = strPathName.ReverseFind('\\');
		if (iBackslash != -1)
			strPathName = strPathName.Left(iBackslash+1);
		strPathName += pszFileName;
	}

	LPTYPELIB ptlib = NULL;
	if (SUCCEEDED(LoadTypeLib(T2COLE(strPathName), &ptlib)))
	{
		ASSERT_POINTER(ptlib, ITypeLib);

		LPTLIBATTR pAttr;
		GUID tlidActual = GUID_NULL;

		if (SUCCEEDED(ptlib->GetLibAttr(&pAttr)))
		{
			ASSERT_POINTER(pAttr, TLIBATTR);
			tlidActual = pAttr->guid;
			ptlib->ReleaseTLibAttr(pAttr);
		}

		// Check that the guid of the loaded type library matches
		// the tlid parameter.
		ASSERT(IsEqualGUID(tlid, tlidActual));

		if (IsEqualGUID(tlid, tlidActual))
		{
			// Register the type library.
			if (SUCCEEDED(RegisterTypeLib(ptlib,
					T2OLE((LPTSTR)(LPCTSTR)strPathName), T2OLE((LPTSTR)pszHelpDir))))
				bSuccess = TRUE;
		}

		ptlib->Release();
	}
	else
	{
		TRACE1("Warning: Could not load type library from %s\n", (LPCTSTR)strPathName);
	}


	return bSuccess;
}


void CChatApp::OnMotd() 
{
	m_bDisableMOTD = GetIrcProto()->bChatShowMOTD();
}


void CChatApp::OnUpdateMotd(CCmdUI* pCmdUI) 
{
	int iStatus = GetIrcProto()->GetConnectionStatus();
	pCmdUI->Enable((iStatus == CX_INCHANNEL || iStatus == CX_NOCHANNEL) && !m_bDisableMOTD);
}


// Implementation of a hidden window that serves as the same-thread parent for
// dialogs when we are run embedded.

void
CHiddenInThreadWnd::StartModal(CWnd* pForParent)
{ 
	if (pForParent != NULL && pForParent == this)
	{
		HWND hwndContainer = GetContainer (); 
		if (hwndContainer) 
			::EnableWindow (hwndContainer, FALSE); 
	}
}

void
CHiddenInThreadWnd::FinishModal(CWnd* pForParent)
{ 
	if (pForParent != NULL && pForParent == this)
	{
		HWND hwndContainer = GetContainer (); 
		if (hwndContainer) 
			::EnableWindow (hwndContainer, TRUE); 
	}
}

HWND
CHiddenInThreadWnd::GetContainer()
{
	HWND hwnd= ::AfxGetMainWnd ()->GetSafeHwnd ();
	while (hwnd != NULL && (::GetWindowLong(hwnd, GWL_STYLE) & WS_CHILD))
		hwnd = ::GetParent(hwnd);
	return hwnd;
}

BOOL
CChatApp::CreateHiddenInThreadWnd()
{
	LPCTSTR pszClass = ::AfxRegisterWndClass (0);
	if (pszClass == NULL)
	{
		return FALSE;
	}

	TRY
	{
		m_pWndHiddenInThread = new CHiddenInThreadWnd;
		if (!m_pWndHiddenInThread->CreateEx (0, pszClass, "",
				WS_POPUP, -20000, -20000, 1, 1, NULL, NULL))
		{
			delete m_pWndHiddenInThread;
			m_pWndHiddenInThread = NULL;
			return FALSE;
		}
	}
	CATCH_ALL(e)
	{
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}


CRoomInfo* CChatApp::GetRoomInfoFromName(CString strChannelName, PINT piIndex, BOOL bCloneOK, BOOL bNullOK)
{
	INT			iRoomIndex, iRoomInfos = m_enterInfos.GetSize();
	CRoomInfo*	pEnterInfo;
	CString		strUpperChannelName, strUpperEnterRoom;

	for (iRoomIndex = iRoomInfos-1; iRoomIndex >= 0; iRoomIndex--)
	{
		pEnterInfo = (CRoomInfo*) m_enterInfos.GetAt(iRoomIndex);
		ASSERT(pEnterInfo);
		if (pEnterInfo->m_strChannel.CompareNoCase(strChannelName))
		{
			if (bCloneOK)
			{
				strUpperChannelName = strChannelName;
				strUpperEnterRoom = pEnterInfo->m_strChannel;
				strUpperChannelName.MakeUpper();
				strUpperEnterRoom.MakeUpper();
				if (0 == strUpperChannelName.Find(strUpperEnterRoom))
				{
					INT iIndex = strUpperEnterRoom.GetLength();
					INT iMax = strUpperChannelName.GetLength();
					while (iIndex < iMax && isdigit(strUpperChannelName[iIndex]))
						iIndex++;
					if (iIndex == iMax)
						// found a clone
						goto exit;
				}
			}
		}
		else
			goto exit;
	}

	pEnterInfo = NULL;

exit:
	if (!pEnterInfo)
	{
		if (bNullOK)
			iRoomIndex = -1;
		else
		{
			pEnterInfo = (CRoomInfo*) m_enterInfos.GetAt(iRoomIndex = 0);  // <==> &g_enterInfo;
			pEnterInfo->m_strChannel = strChannelName;
		}
	}

	if (piIndex)
		*piIndex = iRoomIndex;
	return pEnterInfo;
}


INT CChatApp::AddRoomInfo(CRoomInfo* pEnterInfo)
{
	ASSERT(pEnterInfo);

	return m_enterInfos.Add((void*) pEnterInfo);
}


void CChatApp::RemoveRoomInfo(INT iIndex)
{
	ASSERT(iIndex > 0);
	ASSERT(iIndex < m_enterInfos.GetSize());

	CRoomInfo*	pEnterInfo;
	
	pEnterInfo = (CRoomInfo*) m_enterInfos.GetAt(iIndex);
	ASSERT(pEnterInfo);
	delete pEnterInfo;

	m_enterInfos.RemoveAt(iIndex);
}


void CChatApp::CleanRoomInfos()
{
	INT			iRoomIndex, iRoomInfos = m_enterInfos.GetSize();
	CRoomInfo*	pEnterInfo;

	for (iRoomIndex = 1; iRoomIndex < iRoomInfos; iRoomIndex++)
	{
		pEnterInfo = (CRoomInfo*) m_enterInfos.GetAt(iRoomIndex);
		ASSERT(pEnterInfo);
		delete pEnterInfo;
	}
	m_enterInfos.SetSize(1);
}


// A kind of CWnd::GetSafeOwner, except this returns a popup window this is 
// in the same thread as the window passed in. This is required when running
// embedded, to keep the parent window from getting ownership of this window.

CWnd* 
CChatApp::GetInThreadOwner(CWnd* pWndOf)
{
	CWnd * pWndOwner;
	
	if (!m_pWndHiddenInThread)
	{
		// No special casing, just let MFC handle it.
		pWndOwner = pWndOf;
	}
	else if (pWndOf->GetSafeHwnd () == NULL)
	{
		pWndOwner = m_pWndHiddenInThread;
	}
	else
	{
		pWndOwner = CWnd::GetSafeOwner (pWndOf);
		HWND hwnd = pWndOwner->GetSafeHwnd ();
		if (hwnd != NULL && 
			::GetWindowThreadProcessId (hwnd, NULL) != ::GetWindowThreadProcessId (pWndOf->m_hWnd, NULL))
		{
			pWndOwner = m_pWndHiddenInThread;
		}
	}
	return pWndOwner;
}


// Just like the regular CApp::DoMessageBox, but passes in "GetActiveWindow" result
//    to GetSafeOwner, rather than NULL.  Passing in NULL results in the MainFrame
//    popping to the front when the AfxMessageBox is done, even if you are currently
//    using a whisperbox (which is a problem).
//
int CChatApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	ASSERT_VALID(this);

	HWND hwndFocus = m_bEmbedded ? ::GetFocus () : NULL;

	// disable windows for modal dialog
	EnableModeless(FALSE);
	HWND hWndTop;
	CWnd* pWnd = CWnd::GetSafeOwner(CWnd::GetActiveWindow(), &hWndTop);

	// set help context if possible
	DWORD* pdwContext = &m_dwPromptContext;
	if (pWnd != NULL)
	{
		// use app-level context or frame level context
		ASSERT_VALID(pWnd);
		CWnd* pMainWnd = pWnd->GetTopLevelParent();
		ASSERT_VALID(pMainWnd);
		if (pMainWnd->IsFrameWnd())
			pdwContext = &((CFrameWnd*)pMainWnd)->m_dwPromptContext;
	}

	ASSERT(pdwContext != NULL);
	DWORD dwOldPromptContext = *pdwContext;
	if (nIDPrompt != 0)
		*pdwContext = HID_BASE_PROMPT+nIDPrompt;

	// determine icon based on type specified
	if ((nType & MB_ICONMASK) == 0)
	{
		switch (nType & MB_TYPEMASK)
		{
		case MB_OK:
		case MB_OKCANCEL:
			nType |= MB_ICONEXCLAMATION;
			break;

		case MB_YESNO:
		case MB_YESNOCANCEL:
			nType |= MB_ICONEXCLAMATION;
			break;

		case MB_ABORTRETRYIGNORE:
		case MB_RETRYCANCEL:
			// No default icon for these types, since they are rarely used.
			// The caller should specify the icon.
			break;

#ifdef _MAC
		case MB_SAVEDONTSAVECANCEL:
			nType |= MB_ICONEXCLAMATION;
			break;
#endif
		}
	}

#ifdef _DEBUG
	if ((nType & MB_ICONMASK) == 0)
		TRACE0("Warning: no icon specified for message box.\n");
#endif

	_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
	CWnd* pUseParentWnd = GetInThreadOwner (pWnd); // Make sure parent is in same thread.
	int nResult =
		::MessageBox(pUseParentWnd->GetSafeHwnd(), lpszPrompt, m_pszAppName, nType);
	*pdwContext = dwOldPromptContext;

	// re-enable windows
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
	EnableModeless(TRUE);

	if (hwndFocus != NULL)
	{
		::SetFocus (hwndFocus);
	}
	return nResult;
}


void CChatApp::OnViewStatuswindow() 
{
	CStatusView*	pStatusView = (CStatusView*) GetStatusView();
	CChatDoc*		pStatusDoc  = pStatusView ? (CChatDoc*) pStatusView->GetDocument() : NULL;
	CFrameWnd*		pFrame = pStatusView ? GetStatusView()->GetParentFrame() : NULL;

	if (!pStatusDoc || !pFrame)
		return;

	theApp.m_flags0 ^= F0_SHOWSTATUSWINDOW;
	if (theApp.m_flags0 & F0_SHOWSTATUSWINDOW)
	{
		pFrame->ActivateFrame();
		SetActiveTab(pStatusDoc);
	}
	else
	{
		if (!theApp.m_bEmbedded)
		{
			CMDIFrameWnd *wnd = (CMDIFrameWnd*)AfxGetMainWnd();
			if (wnd->MDIGetActive () == pFrame)
				wnd->MDINext();
		}
		BOOL bVisible = pFrame->IsWindowVisible();
		pFrame->ShowWindow(SW_HIDE);
	}
	if (!theApp.m_bEmbedded)
		// Doesn't do it immediately, does it on a PostMessage.
		((CMainFrame*)::AfxGetMainWnd ())->AutoArrangeWindows ();

	// Refresh the window menu.
	CMDIFrameWnd *wnd = (CMDIFrameWnd*)AfxGetMainWnd();
	::SendMessage(wnd->m_hWndMDIClient, WM_MDIREFRESHMENU, 0, 0);
	wnd->DrawMenuBar ();
}


void CChatApp::OnUpdateViewStatuswindow(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(ISTRUE(theApp.m_flags0 & F0_SHOWSTATUSWINDOW));
}


void CChatApp::OnViewLoginNotifs() 
{
	m_bLoginNotifsShown = !m_bLoginNotifsShown;

	CNotificationUsers*	pNotifBox = GetNotifBox();

	if (m_bLoginNotifsShown)
	{
		if (pNotifBox)
			pNotifBox->ShowWindow(SW_SHOWNORMAL);
		else
			pNotifBox = CreateNotificationBox();
	}
	else
	{
		if (pNotifBox)
		{
			pNotifBox->ShowWindow(SW_HIDE);
			m_dynaNotifs.bRemoveFlagsFromAllUsers(g_wNew);
		}
	}
}


void CChatApp::OnUpdateViewLoginNotifs(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bLoginNotifsShown);
}


CFrameWnd *CChatApp::CreateStatusWnd(CMultiDocTemplate *pDocTemplate) {
	CChatDoc *statusDoc = (CChatDoc *) RUNTIME_CLASS(CChatDoc)->CreateObject();
	statusDoc->m_bStatusView = TRUE;
	CCreateContext context;
	context.m_pCurrentFrame = NULL;
	context.m_pCurrentDoc = statusDoc;
	context.m_pNewViewClass = RUNTIME_CLASS(CChatView);
	context.m_pNewDocTemplate = pDocTemplate;
	pDocTemplate->AddDocument(statusDoc);
	CFrameWnd* pFrame = (CFrameWnd*) RUNTIME_CLASS(CChildFrame)->CreateObject();
	if (pFrame == NULL) return NULL;
	ASSERT_KINDOF(CFrameWnd, pFrame);
	if (context.m_pNewViewClass == NULL)
		return NULL;

	// create new from resource
	if (!pFrame->LoadFrame(IDR_MAINFRAME,
			WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE,   // default frame styles
			NULL, &context))
		return NULL;


	// it worked !
	statusDoc->OnNewDocument();
	pFrame->InitialUpdateFrame(statusDoc, m_flags0 & F0_SHOWSTATUSWINDOW);
//	if (!(m_flags0 & F0_SHOWSTATUSWINDOW)) pFrame->ShowWindow(SW_HIDE);
	return pFrame;
}

void CChatApp::OnDefineMacro() 
{
	OnViewAutomations();
}

// Maximum reasonable limit for size of files to be downloaded
#define WEBREQ_MAXFILESIZE (2 * 1024 * 1024)

// Start downloading an avatar. If the avatar is already being downloaded,
// it does not try again.

BOOL
CChatApp::StartDownloadingAvatar(
CUserInfo* pUserFrom,
CChatDoc * pDocFrom,
BOOL	   bInteractive)
{
	LPCTSTR pszAvatar = pUserFrom->GetAvatarRealName ();
	LPCTSTR pszURL = pUserFrom->GetAvatarRealURL ();

	CChatFileRequest * pRequest = NULL;	

	// Do we have a valid name and URL? 
	if (pszAvatar == NULL || *pszAvatar == '\0' || pszURL == NULL) {
		return FALSE;
	}

	// If the URL is a deferred URL, reget the character info, and defer the
	// get for later.
	if (pszURL[0] == DEFERRED_URL_CHAR && pszURL[1] == '\0') {
		pDocFrom->m_proto->ChatGetAvatarInfo (pUserFrom, bInteractive);
		return FALSE;
	}

	CString strLookup = pszAvatar;
	strLookup += ".avb";

	// Maybe the avatar is already being loaded.
	if (m_mapDownloads.Lookup (strLookup, (PVOID&)pRequest)) {
		// If the download wasn't interactive before, but now is, make the
		// request reflect that.
		if (bInteractive) {
			pRequest->SetInteractive ();
		}
		CString strStatusMsg;
		strStatusMsg.LoadString (IDS_DLSTAT_REPEAT_AV);
		VERIFY(ReplaceToken (strStatusMsg, CString ("%1"), strLookup));
		SetStatusPaneString (0, strStatusMsg);
		return TRUE;
	}

	CString strStatusMsg;
	strStatusMsg.LoadString (IDS_DLSTAT_DOWNLOADING_AV);
	VERIFY(ReplaceToken (strStatusMsg, CString ("%1"), strLookup));
	SetStatusPaneString (0, strStatusMsg);

	TRY
	{
		// May have to create the requestor.
		if (m_pNetRequestor == NULL) {
			m_pNetRequestor = new CInternetRequestor (WEBREQ_USERAGENT_MSCHAT);
			m_pNetRequestor->SetMaxReasonableFileLimit (WEBREQ_MAXFILESIZE);
		}

		// Create the request.
		pRequest = new CChatFileRequest (pszAvatar, pszURL, chatfileAvatar, bInteractive);
		m_mapDownloads.SetAt (strLookup, (PVOID)pRequest);

		// Add the request to the requestor's work queue. This will fail if
		// WinInet is not available.
		if (!m_pNetRequestor->AddToWorkQueue (pRequest)) {
			::AfxThrowUserException ();
		}
	}
	CATCH_ALL(e)
	{
		if (pRequest != NULL) {
			delete pRequest;
		}
		if (bInteractive) {
			::AvatarTransferError (pszAvatar);
		}
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}

// Start downloading a backdrop. If the backdrop is already being downloaded,
// it does not try again.

BOOL
CChatApp::StartDownloadingBackdrop(
LPCSTR pszBackdrop,
LPCSTR pszURL)
{
	CChatFileRequest * pRequest = NULL;	

	// Do we have a valid name and URL? 
	if (pszBackdrop == NULL || *pszBackdrop == '\0' || pszURL == NULL) {
		return FALSE;
	}

	// Maybe the backdrop is already being loaded.
	PVOID pv;
	if (m_mapDownloads.Lookup (pszBackdrop, pv)) {
		CString strStatusMsg;
		strStatusMsg.LoadString (IDS_DLSTAT_REPEAT_AV);
		VERIFY(ReplaceToken (strStatusMsg, CString ("%1"), pszBackdrop));
		SetStatusPaneString (0, strStatusMsg);
		return TRUE;
	}

	CString strStatusMsg;
	strStatusMsg.LoadString (IDS_DLSTAT_DOWNLOADING_BK);
	VERIFY(ReplaceToken (strStatusMsg, CString ("%1"), pszBackdrop));
	SetStatusPaneString (0, strStatusMsg);

	TRY
	{
		// May have to create the requestor.
		if (m_pNetRequestor == NULL) {
			m_pNetRequestor = new CInternetRequestor ("MS Chat");
			m_pNetRequestor->SetMaxReasonableFileLimit (WEBREQ_MAXFILESIZE);
		}

		// Create the request.
		pRequest = new CChatFileRequest (pszBackdrop, pszURL, chatfileBackdrop);
		m_mapDownloads.SetAt (pszBackdrop, (PVOID)pRequest);

		// Add the request to the requestor's work queue. This will fail if
		// WinInet is not available.
		if (!m_pNetRequestor->AddToWorkQueue (pRequest)) {
			::AfxThrowUserException ();
		}
	}
	CATCH_ALL(e)
	{
		if (pRequest != NULL) {
			delete pRequest;
		}
		return FALSE;
	}
	END_CATCH_ALL
	return TRUE;
}

// Handle one or more files whose downloading has finished. The status of the file
// may be "completed", or "aborted" or "failed".

void
CChatApp::HandleDownloadedFiles()
{
	ASSERT(m_pNetRequestor != NULL);
	CChatFileRequest * pRequest;
	BOOL bDeleteOrigFileIfFailed;
	LPCTSTR pszFileExt;
	LPCTSTR pszURLInFile;
	CString strURLInFile;
	CString strNewFileName;
	BOOL bProceed;

	while ((pRequest = (CChatFileRequest *)m_pNetRequestor->PeekInOutBox (0, FALSE)) != NULL) {

		bDeleteOrigFileIfFailed = FALSE;

		// Need to check the status while the request is in the outbox.
		// After we remove it, the status changes to statusFree.
		DWORD dwStatus = pRequest->GetStatus ();

		switch (pRequest->GetType ())
		{
			case chatfileAvatar:
				pszFileExt = ".avb";
				break;
			case chatfileBackdrop:
				pszFileExt = "";	// Backdrop filenames include the extension!
				break;
		}

		// Now remove the request from the outbox. The debug version is just
		// a safer bit of code that verifies that the outbox shows proper 
		// queue-like behavior.
       #ifdef _DEBUG
		CChatFileRequest * pRequestRemoved = (CChatFileRequest *)m_pNetRequestor->PeekInOutBox (0, TRUE);
		ASSERT(pRequestRemoved == pRequest);
	    pRequest = pRequestRemoved;
       #else
		m_pNetRequestor->PeekInOutBox (0, TRUE);
       #endif
	   
		// If the request failed, and hasn't been retried, add it back to the queue.

		if (dwStatus == CInternetRequest::statusFailed && 
				pRequest->GetRetryCount () < 1) {
			pRequest->IncrementRetryCount ();
			if (m_pNetRequestor->AddToWorkQueue (pRequest)) {
				continue;
			}
		}

		// Can only proceed if the file was downloaded successfully.
		bProceed = dwStatus == CInternetRequest::statusCompleted;
		if (bProceed) {
			// If download was successful, there is now a temp file, and we
			// have to delete it if there is a problem.
			bDeleteOrigFileIfFailed = TRUE;
		}

		// Next step is to check if it is a valid file.
		if (bProceed) {
			bProceed = FALSE;
			CAvatarFileStream file (pRequest->GetLocalFileName ());
			switch (pRequest->GetType ()) {
				case chatfileAvatar:
				{
					CAvatarX * pAv = CAvatarX::LoadAvatar (&file);
					if (pAv != NULL) {
						bProceed = TRUE;
						pszURLInFile = pAv->Url ();
						strURLInFile = pszURLInFile ? pszURLInFile : "";
						delete pAv;
					}
					break;
				}
				case chatfileBackdrop:
				{
					CChatBackdrop * pBackdrop = CChatBackdrop::LoadBackdrop (&file);
					if (pBackdrop != NULL) {
						bProceed = TRUE;
						pszURLInFile = pBackdrop->Url ();
						strURLInFile = pszURLInFile ? pszURLInFile : "";
						delete pBackdrop;
					}
					break;
				}
			}
		}

		// Security check - the URL in the file should correspond to the URL
		// we just downloaded - if not, someone is spoofing a URL.

		if (bProceed && lstrcmpi (strURLInFile, pRequest->GetURL ()) != 0) {
			ASSERT(FALSE);
			bProceed = FALSE;
		}

		// Next step is to formulate a local filename, based on the original filename,
		if (bProceed) {
			strNewFileName.Format ("%s\\%s%s", GetAvatarDir (), 
				pRequest->GetName (), pszFileExt);

			// Quick check to see if we're over the max path. We can only proceed
			// if the filename fits.
			if (strNewFileName.GetLength () >= MAX_PATH) {
				bProceed = FALSE;
			}
		}

		// Move the file.
		if (bProceed) {
			// Delete the old file, just in case there is one. (There shouldn't really be).
			::DeleteFile (strNewFileName);
			bProceed = ::MoveFile (pRequest->GetLocalFileName (), strNewFileName);
		}

		// For backdrops, we need to update all backdrop entries.
		if (bProceed && pRequest->GetType () == chatfileBackdrop) {
			extern void NotifyDownloadedBackdrop(LPCSTR);
			NotifyDownloadedBackdrop (pRequest->GetName ());
		}

		// Install the file for all that needs it. We do this by telling each
		// chat document that the file is available. 

		if (bProceed) {
			extern CPtrList g_docs;
			POSITION pos = g_docs.GetHeadPosition();
			while (pos) {
				CChatDoc *pDoc = (CChatDoc *)g_docs.GetNext (pos);
				if (pRequest->GetType () == chatfileAvatar) {
					pDoc->OnNotifyNewAvatar (pRequest->GetName ());
				}
			}
		}

		// Cleanup. Delete the file if needed, remove the entry from the map of
		// files being downloaded, and delete the request.

		if (!bProceed && bDeleteOrigFileIfFailed) {
			::DeleteFile (pRequest->GetLocalFileName ());
		}
		m_mapDownloads.RemoveKey (CString (pRequest->GetName ()) + pszFileExt);

		if (!bProceed && pRequest->IsInteractive ()) {
			::AvatarTransferError (pRequest->GetName ());
		}
		delete pRequest;
	}

	if (GetChatDoc ())
		GetChatDoc ()->ResetStatus ();
} 

// Weird classes to enable setting m_pParentWnd member, which is (strangely)
// protected and has (again, strangely) no accessor functions.

class CAccessParentDialog : public CDialog
{
public:
	void SetNewParent(CWnd * pParentWnd) { m_pParentWnd = pParentWnd; }
	CWnd* GetParent() { return m_pParentWnd; }
};
class CAccessParentPropertySheet : public CPropertySheet
{
public:
	void SetNewParent(CWnd * pParentWnd) { m_pParentWnd = pParentWnd; }
	CWnd* GetParent() { return m_pParentWnd; }
};

// This function is needed because of XCCHAT - because we are being contained in
// a child window, the proper control does not get focus back after a DoModal call.

int
CChatApp::DoModalDlg(
CWnd* pDlgOrPropSheet, 
BOOL bIsPropSheet)
{
	HWND hwndFocus;
	int nRet;
	CWnd* pParentWnd;
	if (m_bEmbedded)
	{
		hwndFocus = ::GetFocus ();
		// Make sure parent window is in thread, by changing it.
		CWnd* pOldParentWnd = bIsPropSheet ? 
			((CAccessParentPropertySheet *)pDlgOrPropSheet)->GetParent () :
			((CAccessParentDialog *)pDlgOrPropSheet)->GetParent ();
		pParentWnd = GetInThreadOwner (pOldParentWnd);
		if (pParentWnd != pParentWnd)
		{
			if (bIsPropSheet)
				((CAccessParentPropertySheet *)pDlgOrPropSheet)->SetNewParent (pParentWnd);
			else
				((CAccessParentDialog *)pDlgOrPropSheet)->SetNewParent (pParentWnd);
		}
	}
	else
	{
		hwndFocus = NULL;
		pParentWnd = NULL;
	}

	// Do the dialog.
	m_pWndActiveDialog = pDlgOrPropSheet;

	m_pWndHiddenInThread->StartModal (pParentWnd); // This is a no-op when not embedded.
	nRet = bIsPropSheet ? ((CPropertySheet *)pDlgOrPropSheet)->DoModal () : 
			       		  ((CDialog *)pDlgOrPropSheet)->DoModal ();
	m_pWndHiddenInThread->FinishModal (pParentWnd); // This is a no-op when not embedded.

	m_pWndActiveDialog = NULL;

	// Restore focus
	if (hwndFocus != NULL)
		::SetFocus (hwndFocus);
	return nRet;
}

void
CChatApp::OnConnectError()
{
	::AfxGetMainWnd ()->KillTimer (ID_CONNECT_TRY);
	GetIrcProto()->SetConnectionStatus(CX_DISCONNECTED);
	CString strMesg;
	strMesg.LoadString(ID_ERR_CONNECT);
	CChatService svcTemp (m_strConnectedService);
	VERIFY(ReplaceToken(strMesg, CString("%1"), svcTemp.GetDisplayName ()));
	AfxMessageBox(strMesg);
	InitializeServerConnection(&g_enterInfo, &g_bCXPrompt);
}

void 
CChatApp::OnConnectConnected()
{
	::AfxGetMainWnd ()->KillTimer (ID_CONNECT_TRY);
	::AfxGetMainWnd ()->KillTimer (ID_ISIRCXTIMEOUT);
}

void
CChatApp::ContinueConnection()
{
	int nNumSockets = m_SrvConnector.GetNumSockets ();
	int nRet = 1;
	for (int i = 0; nRet == 1 && i < nNumSockets; i++)
	{
		nRet = m_SrvConnector.AssignSocket (i);
		// A return code of 0 means error, which breaks out of the loop
		// and does the Connect Error stuff below. A code of 1 means
		// success and continues the loop. A code of -1 means that 
		// no servers are available (their addresses are still being
		// resolved), which breaks out of the loop and returns quietly.
	}
	if (nRet == 0)
	{
		OnConnectError ();
	}
}

void
CChatApp::ResumeConnection()
{
	if (m_SrvConnector.BeginConnectToService (NULL))
		::AfxGetMainWnd ()->SetTimer (ID_CONNECT_TRY, 50, NULL);
	else
		::AfxGetMainWnd ()->PostMessage (WM_COMMAND, ID_CONNECT_ERROR);
}

void
CChatApp::CompleteConnection()
{
	m_SrvConnector.Cleanup ();
	m_strConnectedService = m_SrvConnector.m_strSvc;
	m_strConnectedServer = m_SrvConnector.GetConnectingServer ()->m_pszName;
	m_SrvConnector.GetConnectingServerGroup ()->SetLastAccessedServer (m_SrvConnector.GetConnectingServer ());
}

void
CChatApp::IsIrcXTimeout()
{
	serverConn.HrModeIsIrcXFailure();
}


void
AvatarTransferError(
LPCSTR pszAvatarName)
{
	CString strMesg, strFmt;
	strFmt.LoadString (IDS_TRANSFER_AVATAR_ERROR);
	strMesg.Format (strFmt, pszAvatarName);
	AfxMessageBox (strMesg, MB_OK | MB_ICONEXCLAMATION);
}

CMenu*
GetMainSubMenu(
int n)
{
	if (theApp.m_bEmbedded)
	{
		CChatDoc* pDoc = GetChatDoc ();
		return pDoc != NULL ? pDoc->GetMenu (n) : NULL;
	}
	else
	{
		CMenu * pMenu = AfxGetMainWnd ()->GetMenu ();
		if (pMenu != NULL && (pMenu->GetSubMenu (0) == NULL || pMenu->GetSubMenu(0)->GetMenuItemID(0) == SC_RESTORE)) n++; // Skip system menu.
		return pMenu != NULL ? pMenu->GetSubMenu (n) : NULL;
	}
}


void 
CChatApp::OnToolBarDropDown(
NMHDR* pnmhdr,
LRESULT *plr)
{
	NMTOOLBAR* pnmtb = (NMTOOLBAR*)pnmhdr;
	CMenu* pMenu;
	switch (pnmtb->iItem)
	{
		case ID_FAVORITES_OPENFAVORITES:
			pMenu = GetMainSubMenu (6);
			break;
		default:
			pMenu = NULL;
			break;
	}

	if (pMenu)
	{
		CToolBar* pToolBar = (CToolBar *)CWnd::FromHandlePermanent (pnmtb->hdr.hwndFrom);
		if (pToolBar != NULL)
		{
			CRect rect;
			pToolBar->SendMessage (TB_GETRECT, pnmtb->iItem, (LPARAM)&rect);
			pToolBar->ClientToScreen (rect);
			pMenu->TrackPopupMenu (TPM_LEFTALIGN|TPM_LEFTBUTTON|TPM_VERTICAL,
				rect.left, rect.bottom, AfxGetMainWnd (), &rect);
		}
	}
}

// Add favorites from a single directory. This function is recursive.

extern void FirstCharUpper(char *szName) inline;

int
AddFavoritesFromDirectory(
HMENU hmenu,
int* pnCurrentID,
LPCSTR pszDirectory,
CStringArray * pArray)
{
	int nOrigCurrentID = *pnCurrentID;

	WIN32_FIND_DATA fd;
	HANDLE hFind;
	char pszSearch[_MAX_PATH];
	char pszSearchSubDir[_MAX_PATH];
	int nDirectoryLen = lstrlen (pszDirectory);

	// Do subdirectories.
	lstrcpy (pszSearch, pszDirectory);
	lstrcpy (pszSearch + nDirectoryLen, "\\*.*");
	lstrcpy (pszSearchSubDir, pszDirectory);
	lstrcpy (pszSearchSubDir + nDirectoryLen, "\\");
	nDirectoryLen++;
	if ((hFind = FindFirstFile (pszSearch, &fd)) != INVALID_HANDLE_VALUE)
	{
		HMENU hmenuSub = NULL;
		do
		{	
			if (fd.cFileName[0] != '.' && (fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0)
			{
				// Create a submenu.
				if (hmenuSub == NULL)
				{
					hmenuSub = ::CreatePopupMenu ();
				}
				if (hmenuSub != NULL)
				{
					lstrcpy (pszSearchSubDir + nDirectoryLen, fd.cFileName);
					if (AddFavoritesFromDirectory (hmenuSub, pnCurrentID, pszSearchSubDir, pArray) > 0)
					{
						::AppendMenu (hmenu, MF_POPUP, (UINT)hmenuSub, fd.cFileName);
						hmenuSub = NULL;
					}
					else
					{
						// Keep the popup, which is empty, to try on the next one.
					}
				}
			}
		} while (FindNextFile (hFind, &fd));
		FindClose (hFind);
		if (hmenuSub != NULL)
			::DestroyMenu (hmenuSub);
	}

	// Do files.
	lstrcpy (pszSearch + nDirectoryLen, "*.ccr");
	if ((hFind = FindFirstFile (pszSearch, &fd)) != INVALID_HANDLE_VALUE)
	{
		do
		{	
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				if (pArray != NULL)
				{
					lstrcpy (pszSearchSubDir + nDirectoryLen, fd.cFileName);
					pArray->Add (pszSearchSubDir);
				}

				char szFName[_MAX_FNAME];
				_splitpath(fd.cFileName, NULL, NULL, szFName, NULL /*szExt*/);
				FirstCharUpper(szFName);
				::AppendMenu (hmenu, MF_STRING, *pnCurrentID, szFName);
				*pnCurrentID += 1;
			}
		} while (FindNextFile (hFind, &fd));
		FindClose (hFind);
	}

	return *pnCurrentID - nOrigCurrentID;
}

// Add favorites to the Favorites menu.

int
CChatApp::AddFavoritesToMenu(
CMenu* pMenu, 
int nMenuOrigItems)
{
	if (pMenu == NULL)
		return 0;

	// Delete old items. Don't use MFC, to get speed.

	HMENU hmenu = pMenu->m_hMenu;
	int nCount = pMenu->GetMenuItemCount ();
	while (nCount-- > nMenuOrigItems)
	{
		HMENU hSubMenu = ::GetSubMenu (hmenu, nMenuOrigItems);
		::RemoveMenu (hmenu, nMenuOrigItems, MF_BYPOSITION);
		if (hSubMenu)
			::DestroyMenu (hSubMenu);
	}

	m_arrFavorites.RemoveAll ();
	m_arrFavorites.SetSize (0, 2);

	int nCurrentID = ID_FAVORITES;
	int nAdded = AddFavoritesFromDirectory (hmenu, &nCurrentID, m_strFavoritesDir, &m_arrFavorites);
	if (nAdded > 0)
	{
		::InsertMenu (hmenu, nMenuOrigItems, MF_BYPOSITION | MF_SEPARATOR, 0, NULL);
		nAdded++;
	}

	return nAdded;
}

void 
CChatApp::OnFavorite(
UINT nID)
{
	if (nID == ID_FAVORITES_LAST)
	{
		// Refresh the list.
		AddFavoritesToMenu (GetMainSubMenu (6), 2);
	}

	int nIndex = (int)nID - ID_FAVORITES;
	if (nIndex >= 0 && nIndex < m_arrFavorites.GetSize ())
	{
		OpenDocumentFile(m_arrFavorites[nIndex]);
	}
}

UINT 
CChatApp::FavoriteMonitorFunc(
LPVOID pParam)
{
	FAVMONITOR* pMonitor = (FAVMONITOR*)pParam;

	HANDLE hWait[2];
	char szPath[_MAX_PATH];
	HWND hwnd;

	lstrcpy (szPath, pMonitor->pszDir);
	hwnd = pMonitor->hwnd;
	hWait[0] = pMonitor->hFinishEvent;
	delete pMonitor;

	::SetThreadPriority (::GetCurrentThread (), THREAD_PRIORITY_IDLE);

    HANDLE hChange = ::FindFirstChangeNotification (szPath, 
							TRUE, FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_DIR_NAME);
    if (hChange == INVALID_HANDLE_VALUE)        
		return 0;

	BOOL bContinue = TRUE;
	do
	{
		hWait[1] = hChange;
		switch (::WaitForMultipleObjects (2, hWait, FALSE, INFINITE))
		{
			case WAIT_OBJECT_0:
			default:
				// Main thread exiting.
				bContinue = FALSE;
				break;
			case WAIT_OBJECT_0 + 1:
				// Something changed.
				::PostMessage (hwnd, WM_COMMAND, ID_FAVORITES_LAST, 0);
				if (!::FindNextChangeNotification (hChange))
					bContinue = FALSE;
				break;
		}
	} while (bContinue);
	::FindCloseChangeNotification (hChange);
	return 0;
}

// Common code to handle a WM_QUERYNEWPALETTE for any window in our app.

BOOL
CChatApp::QueryNewPaletteCommon(
CWnd* pWnd)
{
	CClientDC dc (pWnd);

	// Are we even in palette mode??
	if ((dc.GetDeviceCaps (RASTERCAPS) & RC_PALETTE) == 0)
		return FALSE;

	UINT		uiRealized = 0;
	CPalette	*phOldPal = dc.SelectPalette(&ghPalette, FALSE);
	if (uiRealized = dc.RealizePalette())
	{
		pWnd->InvalidateRect(NULL,TRUE);
	}
    if (phOldPal)
		dc.SelectPalette(phOldPal, TRUE);
	return uiRealized;
}

void
CChatApp::OnSoundsOff()
{
	sndPlaySound (NULL, SND_SYNC);
	sndPlayMidiSound (NULL, SND_SYNC);
}
