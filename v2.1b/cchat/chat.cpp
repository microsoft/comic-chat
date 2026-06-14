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

#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "bodycam.h"
#include "setupdlg.h"
#include "proppage.h"
#include "saywnd.h"
#include "roomlist.h"
#include "userlist.h"
#include "IrcProto.H"
#include "motd.h"
#include <winnls.h>
#include "chatver.h"
#include "format.h"

#include <direct.h>
#include <winreg.h>
#include <io.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" BYTE GetCorrectCharSet();

// proto for static version of AfxOleRegisterTypeLib
BOOL 
	CChat_AfxOleRegisterTypeLib( HINSTANCE hInstance, REFGUID tlid,
								 LPCTSTR pszFileName, LPCTSTR pszHelpDir=NULL);



/////////////////////////////////////////////////////////////////////////////
// Parameters

// REGISB 09/11/97 not used  #define szRichEditDLL			"RICHED32.DLL"

/////////////////////////////////////////////////////////////////////////////
// Convenience macros

#define IsSwitch(c) ((c)=='-' || (c)=='/')
//#define pMainFrame	((CMainFrame *) theApp.m_pMainWnd)

/////////////////////////////////////////////////////////////////////////////
// Statics with global scope

CString strHelpFile;
CUI cui;		// object constructed statically

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
	ON_COMMAND(ID_VIEW_OPTIONS, OnViewOptions)
	ON_UPDATE_COMMAND_UI(ID_VIEW_OPTIONS, OnUpdateViewOptions)
	ON_COMMAND(ID_USER_LIST, OnUserList)
	ON_UPDATE_COMMAND_UI(ID_USER_LIST, OnUpdateCanSearch)
	ON_COMMAND(ID_AWAY_TOGGLE, OnAwayToggle)
	ON_UPDATE_COMMAND_UI(ID_AWAY_TOGGLE, OnUpdateAwayToggle)
	ON_COMMAND(ID_VIEW_TABBAR, OnViewTabbar)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TABBAR, OnUpdateViewTabbar)
	ON_COMMAND(ID_MOTD, OnMotd)
	ON_UPDATE_COMMAND_UI(ID_MOTD, OnUpdateMotd)
	ON_UPDATE_COMMAND_UI(ID_ROOM_CREATEROOM, OnUpdateNewroom)
	ON_COMMAND(ID_CHATROOM_LIST, OnChatroomList)
	ON_UPDATE_COMMAND_UI(ID_CHATROOM_LIST, OnUpdateCanSearch)
	ON_COMMAND(ID_IRC_CHAT, OnIrcChat)
	ON_COMMAND(ID_HELP_FREESTUFF, OnHelpFreestuff)
	ON_COMMAND(ID_HELP_PRODUCTNEWS, OnHelpProductnews)
	ON_COMMAND(ID_HELP_FAQ, OnHelpFaq)
	ON_COMMAND(ID_HELP_ONLINESUPPORT, OnHelpOnlineSupport)
	ON_COMMAND(ID_HELP_BESTOFWEB, OnHelpBestofWeb)
	ON_COMMAND(ID_HELP_SEARCHTHEWEB, OnHelpSearchtheWeb)
	ON_COMMAND(ID_HELP_MSHOMEPAGE, OnHelpMsHomepage)
	ON_COMMAND(ID_HELP_RELEASENOTES, OnHelpReleaseNotes)
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_HELP, OnHelpTopics)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChatApp construction

CChatApp::CChatApp()
{
	// Place all significant initialization in InitInstance
	m_bNoRefresh = FALSE;
	m_xFrame = m_yFrame = m_cxFrame = m_cyFrame = m_maxedFrame = 0;
	m_bComicView = TRUE;
	m_bShowMode = FALSE;
	m_bVIPMode = FALSE;
#define TEXT_VIEW_BLANK_ALWAYS 2   // can't include textcore.h for obscure reasons...
	m_textSpacing = TEXT_VIEW_BLANK_ALWAYS;
	m_bShowArrivals = TRUE;
	m_bAllowInvites = TRUE;
	m_bCfInitialized = FALSE;
	m_iHostHighlight = HH_BOLD_HEADERS | HH_BOLD_MESSAGES;
	m_iGreetingType = 0;
	m_iShowBars = SB_TOOLBAR | SB_STATUSBAR;
//	m_bInSearch = FALSE;  -- now set to be FALSE in login
	m_charSet = ANSI_CHARSET;
	m_bPlaySounds = TRUE;
	m_bAcceptNMCalls = TRUE;
	m_bShowIdentity = TRUE;
	m_bListRegistered = FALSE;
	m_whisperRect.left = m_whisperRect.right = 0;   // sign it hasn't been init'ed
	m_bSaveViewMode = TRUE;
	m_bAllowFileTX = TRUE;
	m_floodFlags = FLOOD_IGNORE;
	m_floodCount = 4;
	m_floodInterval = 5;
	m_flags1 = ~0;
	m_pmenuAdmin = NULL;
	m_pmenuMacro = NULL;
	m_bIPMacroDone = FALSE;
	m_iOnConnectAction = CA_JOINROOM;
	m_bIconMembers = TRUE;
	m_bPrompt = FALSE;			// do not prompt by default
	m_bAcceptWhispers = TRUE;
	m_bEmbedded = FALSE;
	m_bAway = FALSE;
	m_bDoCB32 = FALSE;
	m_bDoTest = FALSE;
	m_szGuiFaceName[0] = '\0';
	m_lfGuiPitchAndFamily = 0;
	m_strDefaultArtDir = "ComicArt";		// in case registry corrupted
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
}

CChatApp::~CChatApp()
{
	// nuke admin menu if it exists
	if (m_pmenuAdmin) {
		m_pmenuAdmin->DestroyMenu();
		delete m_pmenuAdmin;
	}
	
	if (m_pmenuMacro) {
		m_pmenuMacro->DestroyMenu();
		delete m_pmenuMacro;
	}

	m_ImageList.DeleteImageList();
	m_StatusIcons.DeleteImageList();

	// free servers
	int upper = m_servers.GetUpperBound();
	for (int i = 0; i <= upper; i++) {
		CServer *server = (CServer *) m_servers[i];
		delete server;
	}

	if (m_szBuffer)
		delete [] m_szBuffer;
	if (m_wszBuffer)
		delete [] m_wszBuffer;

	m_fontGui.DeleteObject();
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
			if (!stricmp(arg, "cb32"))
				m_bDoCB32 = TRUE;
			else if (!stricmp(arg, "test"))
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
				serverConn.m_bSvrAuthSeq = TRUE;
			else if (('P' == *arg || 'p' == *arg) && ':' == *(arg+1) && '\0' != *(arg+2))
			{
				// force user defined authentication Packages in sequence
				// read the packages
				serverConn.m_bUsrAuthSeq = TRUE;

				BOOL bEnd;
				CHAR *szHeadTmp, *szTmp;
				szHeadTmp = szTmp = arg+2;
				ASSERT('\0' != *szHeadTmp);
				do
				{
					if ((',' == *szTmp) || (bEnd = ('\0' == *szTmp)))
					{
						*szTmp = '\0';
						serverConn.m_rgszUsrSecuPack.Add(szHeadTmp);
						if (!bEnd) 
							szHeadTmp = ++szTmp;
					}
					else
						szTmp++;
				}
				while (!bEnd);
			}
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

	// Load initial values from the registry
	LoadFromReg();

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
			m_bDoCB32 ? IDR_NM_MAIN : IDR_MAINFRAME,
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
	m_server.ConnectTemplate(clsid, pDocTemplate, FALSE);
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
	aboutDlg.DoModal();
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
	void DestroyEmotionRules();
	DestroyEmotionRules();				// Free the emotion spotting rules

	void CleanupComicsDataOnExit();
	CleanupComicsDataOnExit();

	void DestroyWhisperBox();
	DestroyWhisperBox();

	void DestroyExternalUserInfos();
	DestroyExternalUserInfos();

	void CleanupFileProgressStore(BOOL);
	CleanupFileProgressStore(TRUE);

	void CommunicationCleanup();
	CommunicationCleanup();

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

	
	// read the registration information from the registry and insert it into the dialog
	if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\Microsoft\\MS Setup (ACME)\\User Info", 
						0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
		*buff = '\0';
		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "DefName", 0, NULL, (unsigned char *)buff, &cbData);
		strRegName = buff;
		*buff = '\0';
		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "DefCompany", 0, NULL, (unsigned char *)buff, &cbData);
		strRegCorp = buff;
		bFoundName = TRUE;
	} else if ((RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows\\CurrentVersion",
							 0, KEY_READ, &hKey) == ERROR_SUCCESS) ||
			   (RegOpenKeyEx (HKEY_LOCAL_MACHINE, "Software\\Microsoft\\Windows NT\\CurrentVersion",
			                 0, KEY_READ, &hKey) == ERROR_SUCCESS)) {
		*buff = '\0';
		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "RegisteredOwner", 0, NULL, (unsigned char *)buff, &cbData);
		strRegName = buff;
		*buff = '\0';
		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "RegisteredOrganization", 0, NULL, (unsigned char *)buff, &cbData);
		strRegCorp = buff;
		bFoundName = TRUE;
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
	return CWinApp::OnIdle(lCount);
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
	
	if(dlgFile.DoModal() == IDOK)
	{
		CString newName = dlgFile.GetFileName();
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
	extern int iViewMode;
	char *end;

	if (end = strstr(roomURL, passwdSeparator)) {	// parse off password
		strRoom = CString(roomURL, end - roomURL);
		password = end + strlen(passwdSeparator);
	}

	if (end = strstr(roomURL, textPostFix)) {
		strRoom = CString(roomURL, end - roomURL);
		iViewMode = VM_TEXT;
		theApp.m_bSaveViewMode = FALSE;		// don't change registry view default!!!
	} else if (end = strstr(roomURL, comicsPostFix)) {
		strRoom = CString(roomURL, end - roomURL);
		iViewMode = VM_COMICS;
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
				void ChatSetServer(const char *), ChatSetCXPrompt(BOOL prompt);
				BOOL bInitEnterInfo(const char *roomName, const char *password = NULL, BOOL bEncode = TRUE);
				start += 6;  // size of mic:// or irc://
				const char *end = strchr(start, '/');
				if (end) {
					ChatSetServer(CString(start, end-start));
					end++;
					CString room, password;
					ParseRoomComponent(end, room, password);
					bInitEnterInfo(room, password, TRUE);	// REGISB Fix me if returns FALSE!
					if (GetChatDoc()) GetChatDoc()->m_fileType = FT_CCR;
					ChatSetCXPrompt(FALSE);
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
	if (g_docs.IsEmpty())
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
	else
		InitializeServerConnection();
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
	void ChatCreateRoom();
	ChatCreateRoom();	
}

void CChatApp::OnDisconnect() 
{
	void ChatServerDisconnect();
	ChatServerDisconnect();
	GetIrcProto()->SetConnectionStatus(CX_DISCONNECTED);	
	AfxMessageBox(IDS_DISCONNECTMESSAGE2);
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
	
	if(dlgFile.DoModal() == IDOK) {
		CString newName = dlgFile.GetFileName();
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

	lResult = RegOpenKeyEx(HKEY_CURRENT_USER,LPCTSTR(strSubKey),0,KEY_QUERY_VALUE,&hShellFolders);
	if(bDesktop)
		lResult = RegQueryValueEx(hShellFolders,"Desktop",0,&type,Data,&cbData);
	else {
		lResult = RegQueryValueEx(hShellFolders,"Favorites",0,&type,Data,&cbData);
		if (lResult != ERROR_SUCCESS) {   // No favorites.  Installed w/o IE?  Create Favorites under basedir
			CString path(theApp.m_strBaseDir);
			CString dir;
			dir.LoadString(IDS_FAVORITES_NAME);
			path += "\\";
			path += dir;
			SECURITY_ATTRIBUTES sa;
			sa.nLength = sizeof(sa);
			sa.lpSecurityDescriptor = NULL;
			sa.bInheritHandle = FALSE;
			CreateDirectory(path, &sa);  // will fail if dir already exists
			strcpy((char *)Data, path);
		}
	}

	return CString(Data);
}

void CChatApp::OnViewOptions() 
{
	CCSPropertySheet	psOptions(IDS_OPTIONS);
	CSettingsPage		ppSettings;
	CPersonalPage		ppPersonal(theApp.m_bDoCB32 ? IDD_PERSONALPAGE_NM : IDD_PERSONALPAGE_IRC);
	CComicsPropPage		ppComics;
	CCharacterPage		ppCharacter;
	CBackgroundPage		ppBackground;
	CTextFontPage		ppTextFont(theApp.m_bDoCB32 ? IDD_TEXTFONTPAGE_NM : IDD_TEXTFONTPAGE_IRC);
	CAutomationPage		ppAutomation;

	psOptions.m_psh.dwFlags &= ~(PSH_HASHELP);  // deactivate help button

	psOptions.AddPage(&ppPersonal);
	if (!theApp.m_bDoCB32) psOptions.AddPage(&ppSettings);

	BOOL comicView = GetChatDoc() ? GetChatDoc()->m_bComicView : theApp.m_bComicView;

	if (comicView) {
		psOptions.AddPage(&ppComics);
		psOptions.AddPage(&ppCharacter);
		psOptions.AddPage(&ppBackground);
	} else {
		psOptions.AddPage(&ppTextFont);
	}

	psOptions.AddPage(&ppAutomation);

	psOptions.DoModal();
	if (GetChatDoc()) GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
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

extern BOOL bCanViewUnrated(BOOL = FALSE);
static CRoomListPersist roomPersist;

void CChatApp::OnChatroomList() 
{
	if (!serverConn.m_bIrcXServer && !bCanViewUnrated(TRUE)) return;   // PICS test for non-IRCX servers

	CRoomList roomDlg(&roomPersist);
	cui.m_pvRoomList = &roomDlg;		// cache this info for irc.cpp
	roomDlg.DoModal();
	cui.m_pvRoomList = NULL;
	if (GetChatDoc()) GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}

static CUserListPersist userPersist;
#define LAUNCH_WHISPERBOX	8181
void CChatApp::OnUserList() 
{
	if (!bCanViewUnrated(TRUE)) return;   // PICS test for all users

	CUserList userDlg(&userPersist);
	cui.m_pvUserList = &userDlg;

	if (userDlg.DoModal() == LAUNCH_WHISPERBOX) {
		if (userDlg.m_selUser) {  // must be set, but check anyway
			void WhisperBox(CUserInfo *);
			WhisperBox(userDlg.m_selUser);
		}
	} else if (GetChatDoc()) GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)

	cui.m_pvUserList = NULL;
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

		if (away.DoModal() == IDOK)
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
	CString rnotes = theApp.m_strBaseDir + "\\readme.txt";
	ShellExecute(GetFrame()->m_hWnd, NULL, rnotes, NULL, NULL, SW_SHOWNORMAL);	
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
	GetIrcProto()->ChatShowMOTD();
}

void CChatApp::OnUpdateMotd(CCmdUI* pCmdUI) 
{
	int iStatus = GetIrcProto()->GetConnectionStatus();
	pCmdUI->Enable(iStatus == CX_INCHANNEL || iStatus == CX_NOCHANNEL);
}



// Just like the regular CApp::DoMessageBox, but passes in "GetActiveWindow" result
//    to GetSafeOwner, rather than NULL.  Passing in NULL results in the MainFrame
//    popping to the front when the AfxMessageBox is done, even if you are currently
//    using a whisperbox (which is a problem).
//
int CChatApp::DoMessageBox(LPCTSTR lpszPrompt, UINT nType, UINT nIDPrompt)
{
	ASSERT_VALID(this);

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
	int nResult =
		::MessageBox(pWnd->GetSafeHwnd(), lpszPrompt, m_pszAppName, nType);
	*pdwContext = dwOldPromptContext;

	// re-enable windows
	if (hWndTop != NULL)
		::EnableWindow(hWndTop, TRUE);
	EnableModeless(TRUE);

	return nResult;
}









