// chat.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "chat.h"

//BINDER: include OLE interfaces for Binder compatibility
#include "ui.h"
#include "mfcbind.h"
#include "bindipfw.h"
#include "binddoc.h"
//BINDER_END

#include "MainFrm.h"
#include "IpFrame.h"
#include "chatDoc.h"
#include "spltchat.h"
#include "chatView.h"
#include "chatprot.h"
#include "version.h"

#include <direct.h>
#include <winreg.h>
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// Parameters

// font picker parameters
#define nFontHeight				-14
#define nFontWidth				0
#define nFontEscapement			0
#define nFontOrientation		0
#define fnFontWeight			FW_DONTCARE
#define fdwFontItalic			FALSE
#define fdwFontUnderline		FALSE
#define fdwFontStrikeOut		FALSE
#define fdwFontCharSet			ANSI_CHARSET
#define	fdwFontOutputPrecision	OUT_DEFAULT_PRECIS
#define fdwFontClipPrecision	CLIP_DEFAULT_PRECIS
#define fdwFontQuality			DEFAULT_QUALITY
#define fdwFontPitchAndFamily	VARIABLE_PITCH | FF_DONTCARE

#define szRichEditDLL			"RICHED32.DLL"

/////////////////////////////////////////////////////////////////////////////
// Convenience macros

#define IsSwitch(c) ((c)=='-' || (c)=='/')
#define pMainFrame	((CMainFrame *) theApp.m_pMainWnd)

/////////////////////////////////////////////////////////////////////////////
// Statics with global scope

CString strHelpFile;

/////////////////////////////////////////////////////////////////////////////
// CChatApp

BEGIN_MESSAGE_MAP(CChatApp, CWinApp)
	//{{AFX_MSG_MAP(CChatApp)
	ON_COMMAND(ID_HELP_TOPICS, OnHelpTopics)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
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
	m_pszName = NULL;				// not currently used - djk
	m_pszScene = NULL;				// not currently used - djk
	m_dwCID = 0;					// not currently used - djk
	m_bNoNetwork = FALSE;
	m_bNoRefresh = FALSE;
	m_bTestMode = FALSE;
	m_bRGBMode = FALSE;
	m_bNoFastMode = FALSE;
	m_xFrame = m_yFrame = m_cxFrame = m_cyFrame = m_maxedFrame = 0;
#ifdef _DEBUG
	m_bFPSDisplay = TRUE;
#else
	m_bFPSDisplay = FALSE;
#endif
	m_bComicView = TRUE;
	cui.m_pvChatApp = this;
}

CChatApp::~CChatApp()
{
	// Reclaim user info records
	void DestroyUserInfos();
	DestroyUserInfos();

	void DestroyEmotionRules();
	DestroyEmotionRules();				// Free the emotion spotting rules
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

	void initproc();
	initproc();

	// Various initializations...
	void LoadEmotionStrings();
	LoadEmotionStrings();		// language specific strings from resources

	// Load emotion rules
	void InitializeEmotionRules();
	InitializeEmotionRules();

	m_lastBackDrop.LoadString(IDS_DEFAULT_BACKDROP);

#if 0
	// Load the INI file
	BOOL LoadFromIni();
	LoadFromIni();
#endif

	// Load initial values from the registry
	BOOL LoadFromReg();
	LoadFromReg();

	// Set maximized if default
	if (m_maxedFrame) m_nCmdShow = SW_SHOWMAXIMIZED;

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	if (!CommunicationInits()) return FALSE;

	char *pszAvatar=NULL;   // why? probably not currently used

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	LoadStdProfileSettings();  // Load standard INI file options (including MRU)

	// load font
	CString szFontFile;
	szFontFile.LoadString(ID_COMP_FONT_FILE);
	AddFontResource(szFontFile);

	CString fontFace;
	fontFace.LoadString(ID_COMP_FONT_NAME);

	// create fonts
	if( !m_fontText.CreateFont(	nFontHeight,
								nFontWidth,				
								nFontEscapement,
								nFontOrientation,
								fnFontWeight,
								fdwFontItalic,
								fdwFontUnderline,
								fdwFontStrikeOut,
								fdwFontCharSet,
								fdwFontOutputPrecision,
								fdwFontClipPrecision,
								fdwFontQuality,
								fdwFontPitchAndFamily,
								fontFace ) )
	{
		return FALSE;
	}


	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CChatDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CChatView));
	pDocTemplate->SetServerInfo(
		IDR_SRVR_EMBEDDED, IDR_SRVR_INPLACE,
		RUNTIME_CLASS(CInPlaceFrame));
	AddDocTemplate(pDocTemplate);

	LoadLibrary(szRichEditDLL);

	// Connect the COleTemplateServer to the document template.
	//  The COleTemplateServer creates new documents on behalf
	//  of requesting OLE containers by using information
	//  specified in the document template.
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

	SetBaseDir(__argv[0]);

	for( iarg=1; iarg<__argc; iarg++ )
	{
		if( IsSwitch( __argv[iarg][0] ) )
			switch( toupper(__argv[iarg][1]) )
			{
				case 'C':
				{
					// specifies no network
					m_bNoNetwork = TRUE;

					break;
				}

				case 'N':
				{
					// specifies user's name and maybe face

					// check to see if name specificed
					if( (iarg+1)<__argc &&
						!IsSwitch( __argv[iarg+1][0]) )
					{
						m_pszName = __argv[iarg + 1];

						iarg += 1;
					}
					else
						AfxMessageBox(ID_ERR_BAD_ARGS);

					// check to see if face specified
					if( (iarg+1)<__argc &&
						!IsSwitch( __argv[iarg+1][0]) )
					{
						pszAvatar = __argv[iarg + 1];

						iarg += 1;
					}

					break;
				}

				case 'I':
				{
					// specifies conference ID
					if( (iarg+1) < __argc &&
						!IsSwitch( __argv[iarg+1][0] ) )
					{
						m_dwCID = (DWORD) atoi(__argv[iarg + 1]);
						iarg++;
					}
					else
						AfxMessageBox(ID_ERR_BAD_ARGS);
					break;
				}

				case 'T':
				{
					m_bTestMode = TRUE;
					break;
				}

				case 'Q':
				{
					if( __argv[iarg][2] == '\0' )
					{
						m_bNoSong = TRUE;
						m_bNoFX = TRUE;
					}
					else
					{
						char ch = toupper(__argv[iarg][2]);

						if( ch=='S' )
							m_bNoSong = TRUE;
						else if( ch=='F' )
							m_bNoFX = TRUE;
					}
					break;
				}

				case 'W':
				{
					// specifies world name
					if( (iarg+1)<__argc &&
						!IsSwitch( __argv[iarg+1][0]) )
					{
						m_pszScene = __argv[iarg + 1];

						iarg += 1;
					}
					else
					{
						AfxMessageBox(ID_ERR_BAD_ARGS);
					}
					break;
				}

				case '2':
				{
					// specifies 2D world
					if( __argv[iarg][2] == '\0' )
					{
						AfxMessageBox(ID_ERR_BAD_ARGS);
					}
					else
					{
						char ch = toupper(__argv[iarg][2]);

						if( ch=='D' )
						{
							m_b2DWorld = TRUE;
						}
						else 
						{
							AfxMessageBox(ID_ERR_BAD_ARGS);
						}
					}
					break;
				}

				case 'R':
					m_bRGBMode = TRUE;
					break;

				case 'F':
					m_bNoFastMode = TRUE;
					break;

				case 'S':
					m_bFPSDisplay = !m_bFPSDisplay;
					break;
			}
	}

	// compute file paths
	m_strBackDropDir = m_strBaseDir;
	m_strBackDropDir += "\\";
	m_strBackDropDir += BACKDROPDIR;

	m_strAvatarDir = m_strBaseDir;
	m_strAvatarDir += "\\";
	m_strAvatarDir += AVATARDIR;

	strTmp.LoadString(IDS_HELP_FILE);
	strHelpFile = m_strBaseDir;
	strHelpFile += "\\";
//	strHelpFile += CHATHELPDIR;
//	strHelpFile += "\\";
	strHelpFile += strTmp;
	CWinApp::m_pszHelpFilePath = strHelpFile;

	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

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

	// Dispatch commands specified on the command line
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	if(cmdInfo.m_nShellCommand == CCommandLineInfo::FileNew)
		m_nCmdShow = -1; // added this to fix mfc bug in ProcessShellCommand;
	SetPrinterResolution();

	return TRUE;
}

void CChatApp::SaveConnectStatus(CString &strStatus)
{
	m_strStatus = strStatus;
}

void CChatApp::ResetStatus()
{
	SetStatusPaneString(0,m_strStatus);
}
void CChatApp::SetStatusPaneString(int index, CString &strPane)
{
	m_strStatusPane[index] = strPane;
	
	if (m_pActiveWnd != NULL)
	{
		CString strStatus;
		strStatus = m_strStatusPane[0]+"  --  "+m_strStatusPane[1];
		((CInPlaceFrame*)m_pActiveWnd)->SetStatusString(strStatus);
	}
	if (m_pMainWnd != NULL)
		((CMainFrame*)m_pMainWnd)->m_wndStatusBar.SetPaneText(index,strPane);
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
	ChatTerminate();

	// free font resource
	CString szFontFile;
	szFontFile.LoadString(ID_COMP_FONT_FILE);
	RemoveFontResource( szFontFile );
	
	// Must clean up animator and all animations.
//	delete pAnimator;

	return CWinApp::ExitInstance();
}

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CStatic *pwnd;
	CString str;
	CString strNum;
	CDC *dlgDC;
	char buff[200];
	DWORD cbData = 0;
	HKEY	hKey = NULL;

	// Set the background for the dialog to white to prevent flashing
	dlgDC=GetDC();
	dlgDC->SetBkColor(RGB(255,255,255));
	ReleaseDC(dlgDC);

	// Put TIKI on the bottom.
	pwnd = (CStatic *) GetDlgItem( IDC_TIKI );
	ASSERT( pwnd != NULL);
	pwnd->SetWindowPos( this, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
  
	// Grab the Version string.
	pwnd = (CStatic *) GetDlgItem( IDC_VERSION );
	ASSERT( pwnd != NULL );

	// get the text from the Version string
	pwnd->GetWindowText(str);

	// replace the placeholders with version values
	strNum.Format("%02d", 1 ); //should be rmj instead of 1
	VERIFY( ReplaceToken( str, CString("%1"), strNum ) );
	strNum.Format("%02d", rmm );
	VERIFY( ReplaceToken( str, CString("%2"), strNum ) );
	strNum.Format("%02d", 0 );
	VERIFY( ReplaceToken( str, CString("%3"), strNum ) );
	strNum.Format("%04d", rup);
	VERIFY( ReplaceToken( str, CString("%4"), strNum ) );

	// put the text back in the control
	pwnd->SetWindowText( str );
	// put control on top of the TIKI bitmap
	pwnd->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );

	
	// read the registration information from the registry and insert it into the dialog
	if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\Microsoft\\MS Setup (ACME)\\User Info", 
						0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) 
	{
		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "DefName", 0, NULL, (unsigned char *)buff, &cbData);
		CString strRegName = buff;
		GetDlgItem(IDC_USER)->SetWindowText(strRegName);
		// put window on top of the TIKI bitmap
		GetDlgItem(IDC_USER)->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
		// Clear the registry buffer.  In case the next RegQueryValueEx fails, we don't
		// want to put the DefName in twice.
		strcpy(buff,"\0");
		RegQueryValueEx (hKey, "DefCompany", 0, NULL, (unsigned char *)buff, &cbData);
		CString strRegCorp = buff;
		GetDlgItem(IDC_CORP)->SetWindowText(strRegCorp);

		// put window on top of the TIKI bitmap
		GetDlgItem(IDC_CORP)->SetWindowPos( GetDlgItem( IDC_TIKI ), 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE );
	}

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
	SetStatusPaneString(0,m_strStatusPane[0]);
	return CWinApp::OnIdle(lCount);
}


//Overrode default behavior to designate between .ccr and .ccc
void CChatApp::OnFileOpen() 
{
	static char BASED_CODE szFilter[] = "Comic Chat Conversation (*.ccc)|*.ccc||";

	CString strFinal = GetChatDoc()->GetPathname();

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

	CFileDialog dlgFile( TRUE,"ccr",strFinal, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR(&szFilter));
	
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
	if(pDoc == NULL)  //then we better ask for a new doc.
		if(AfxGetMainWnd())
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
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
