// AvatarFiler.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AvatarFiler.h"

#include "pseudoAvatar.h"

#include "MainFrm.h"
#include "AvatarFilerDoc.h"
#include "AvatarFilerView.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerApp

BEGIN_MESSAGE_MAP(CAvatarFilerApp, CWinApp)
	//{{AFX_MSG_MAP(CAvatarFilerApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerApp construction

CAvatarFilerApp::CAvatarFilerApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAvatarFilerApp object

CAvatarFilerApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerApp initialization

BOOL CAvatarFilerApp::InitInstance()
{
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

	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views.

	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CAvatarFilerDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CAvatarFilerView));
	AddDocTemplate(pDocTemplate);

	if (__argc == 3) {
		return MyParseAndDispatch();
	} else {
		// Parse command line for standard shell commands, DDE, file open
		CCommandLineInfo cmdInfo;
		ParseCommandLine(cmdInfo);

		// Dispatch commands specified on the command line
		if (!ProcessShellCommand(cmdInfo))
			return FALSE;
	}

	return TRUE;
}

// djk - a quick hack to provide Jim Cambell with a commandline interface,
//       that his avatar editor can call.
//
BOOL CAvatarFilerApp::MyParseAndDispatch() {
	// Hide the window, open the document...
	m_nCmdShow = SW_HIDE;
	if (!OpenDocumentFile(__argv[1]))
		return FALSE;

	// cd to proper directory...
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];

	_splitpath(__argv[1], drive, dir, NULL, NULL);
	CString path(drive);
	path += dir;
	if (!path.IsEmpty()) 
		_chdir(path);


	// get the avatarfilerdoc object
	CWnd *main = AfxGetMainWnd();
	CAvatarFilerDoc *doc = (CAvatarFilerDoc *)(((CFrameWnd *) main)->GetActiveDocument());

	// now write self out ...
	doc->OnSaveDocument(__argv[2]);

	// now quit
	main->PostMessage(WM_COMMAND, ID_APP_EXIT, 0);

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
		// No message handlers
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void CAvatarFilerApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerApp commands
