// setupdialog.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "setupdlg.h"
#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "backdrop.h"
#include "traj.h"
#include "spline.h"
#include "balloon.h"
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "pageview.h"
#include "panel.h"
#include "io.h"
#include "ui.h"
#include "histent.h"
#include "helpids.h"
#include <winreg.h>

#include "resource.h"
#include "mschat.h"
#include "format.h"
#include "ccommon.h"
#include "dlgs.h"	// for edt1 definition

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;
extern void FirstCharUpper(char *szName) inline;
void SetArtDir(const char *);
void SetSoundPath(const char *, BOOL);
BOOL bInitEnterInfo(const char *roomName, const char *password = NULL, BOOL bEncode = FALSE);
CChatDoc *LookupDoc(const char *);


void SetDefaultAutoGreeting()
{
	CString strGreeting, strVar;

	strGreeting.LoadString(IDS_DEFAULTGREETING);
	strVar.LoadString(IDS_USERVARIABLE);

	VERIFY(ReplaceToken(strGreeting, CString("%1"), strVar));
	strVar.LoadString(IDS_ROOMVARIABLE);
	VERIFY(ReplaceToken(strGreeting, CString("%2"), strVar));

	theApp.m_strGreetingMesg = strGreeting;
}


void CChatApp::InitVals() inline {
	m_ircServer.LoadString(IDS_DEFAULT_SERVER);
	m_myRealName.LoadString(IDS_DEFAULT_REALNAME);
	m_myChannel.LoadString(IDS_DEFAULT_CHANNEL);
	m_myName.LoadString(IDS_DEFAULT_NICK);
	m_myNick = m_myName;
	m_strEmail = "";
	m_strHomePage = "";
	m_strFavoritesDir = GetDesktopOrFavorites(FALSE);
	SetSoundPath("", TRUE);
	SetDefaultAutoGreeting();
}


// returns string containing path to desktop (TRUE) or favorites (FALSE)
void CChatApp::InitFileTXDir() {
	CString path(m_strBaseDir);
	CString dir;
	dir.LoadString(IDS_DOWNLOAD_NAME);
	path += "\\";
	path += dir;
	SECURITY_ATTRIBUTES sa;
	sa.nLength = sizeof(sa);
	sa.lpSecurityDescriptor = NULL;
	sa.bInheritHandle = FALSE;
	CreateDirectory(path, &sa);  // will fail if dir already exists
	m_strFileTXDir = path;
}


int GetBarStatus() {
	// if app's embedded, there's a problem determining whether toolbar
	// was showing or not (no longer a window at this time), so don't change
	// persistant value.  Later, fix so that persistant val is stored.
	CToolBar *toolbar = GetToolBar();
	if (!toolbar || !::IsWindow(toolbar->m_hWnd)) return theApp.m_iShowBars;

	int status = 0;

	if (/*toolbar && */toolbar->IsVisible()) status |= SB_TOOLBAR;

	CStatusBar *statusbar = GetStatusBar();
	if (statusbar && statusbar->IsVisible()) status |= SB_STATUSBAR;

	return status;
}


const DWORD CSetupPage::m_nHelpIDs[] =
{
	IDC_CHATROOMS,		IDH_FAVORITES,
	IDC_SERVER, 		IDH_SERVER,
	IDC_CONCHAN,		IDH_CHAT_ROOM,
	IDC_CHANNEL,		IDH_CHAT_ROOM,
	IDC_LISTCHAN,		IDH_SHOW_ALL,
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CSetupPage property page

//IMPLEMENT_DYNCREATE(CSetupPage, CCSPropertyPage)

CSetupPage::CSetupPage() : CCSPropertyPage(CSetupPage::IDD)
{
	// a later LoadFromReg will likely restore defaults for these values
	//{{AFX_DATA_INIT(CSetupPage)
	m_strChatRooms = _T("");
	m_radioConnect = 0;
	//}}AFX_DATA_INIT

	m_ircServer = theApp.m_ircServer;
	m_myChannel = theApp.m_myChannel;

	nWhatFailed = CHATROOM;
}

CSetupPage::~CSetupPage()
{
}

void CSetupPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupPage)
	DDX_Control(pDX, IDC_SERVER, m_serverCtl);
	DDX_Control(pDX, IDC_CHANNEL, m_editChannel);
	DDX_Control(pDX, IDC_CHATROOMS, m_ChatRooms);
	DDX_Text(pDX, IDC_SERVER, m_ircServer);
	DDV_MaxChars(pDX, m_ircServer, 100);
	DDX_Text(pDX, IDC_CHANNEL, m_myChannel);
	DDV_MaxChars(pDX, m_myChannel, 200);
	DDX_CBString(pDX, IDC_CHATROOMS, m_strChatRooms);
	DDX_Radio(pDX, IDC_CONCHAN, m_radioConnect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CSetupPage)
	ON_CBN_SELCHANGE(IDC_CHATROOMS, OnSelchangeChatrooms)
	ON_BN_CLICKED(IDC_CONCHAN, OnConchan)
	ON_BN_CLICKED(IDC_LISTCHAN, OnListchan)
	ON_EN_CHANGE(IDC_CHANNEL, OnChangeChannel)
	ON_CBN_EDITCHANGE(IDC_SERVER, OnChangeServer)
	ON_CBN_SELCHANGE(IDC_SERVER, OnChangeServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPage message handlers

// Finds the text after the colon (on an ini line) and copies that into value.
// Strips off white space on either end
BOOL GetValue(const char *szBuff, char *value, UINT uBuffSize) {
	// assumes value is large enough to hold chars
	char *start = strchr(szBuff, ':');
	if (!start) return FALSE;
	start++;
	while (*start && my_isspace(*start)) start++;
	char *end = strchr(szBuff, '\0');
	while (my_isspace(*end) || !*end) end--;
	UINT len = end - start + 1;
	if (len > uBuffSize-1) return FALSE;
	strncpy(value, start, len);
	value[len] = '\0';
	return TRUE;
}

int CMacro::Serialize(char *szBuff, int iBuffLen) {
	int iNameLen = strlen(m_strName);
	int iValLen = strlen(m_strValue);
	int iTotalLen = iNameLen + iValLen + 3;
	if (iBuffLen < iTotalLen) return 0;
	strcpy(szBuff, m_strName);
	strcpy(szBuff + iNameLen + 1, m_strValue);
	szBuff[iTotalLen - 1] = '\0';
	return iTotalLen;
}

void CMacro::UnSerialize(const char *szBuff) {
	m_bDefined = TRUE;
	m_strName = szBuff;
	const char *szBreakPtr = strchr(szBuff, '\0');
	if (szBreakPtr) m_strValue = szBreakPtr + 1;
}

void SaveMacros() {
	HKEY hKey = NULL;
	char szBuff[(MAX_FORMATTINGPERBYTE+1)*MAX_INPUTLEN];
	CString strMacroKey(szRootRegKeyName), strRegName;

	strMacroKey += "\\Macros";
	if (RegCreateKeyEx (HKEY_CURRENT_USER, strMacroKey, 
						0, "Macro Data", REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,	NULL, &hKey, NULL) == ERROR_SUCCESS) {
		for (int i = 0; i < NMACROS; i++) {
			strRegName.Format("%d", i);
			if (!theApp.m_macros[i].m_bDefined) RegDeleteValue(hKey, strRegName);
			else {
				int size = theApp.m_macros[i].Serialize(szBuff, sizeof(szBuff));
				if (size >= 0)
				   RegSetValueEx (hKey, strRegName, 0, REG_MULTI_SZ,
								  (const unsigned char *)(LPCTSTR)szBuff,
								  size);
			}
		}
	}
	RegCloseKey (hKey);
}

void LoadMacros() {
	HKEY hKey = NULL;
	char szBuff[(MAX_FORMATTINGPERBYTE+1)*MAX_INPUTLEN];
	CString strMacroKey(szRootRegKeyName), strRegName;

	strMacroKey += "\\Macros";
	if (RegOpenKeyEx (HKEY_CURRENT_USER, strMacroKey, 
					  0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {
		for (int i = 0; i < NMACROS; i++) {
			strRegName.Format("%d", i);
			ULONG cbData = sizeof(szBuff);
		    if (RegQueryValueEx (hKey, strRegName, 0, NULL, (LPBYTE)szBuff, &cbData) == ERROR_SUCCESS) {
				theApp.m_macros[i].UnSerialize(szBuff);
			}
		}
	}
	RegCloseKey (hKey);
}

void LoadServerFromReg(HKEY hKey) {
}

BOOL AddToList(const char *szServer, void *hKey) {
	CServer *server = new CServer(szServer);
	LoadServerFromReg((HKEY)hKey);
	theApp.m_servers.Add(server);
	return FALSE;	// keep adding
}

void LoadServerList(const char *servers, HKEY hKey = NULL) {
	if (servers) {
		HKEY hSubKey = NULL;
		if (hKey)
			RegOpenKeyEx (hKey, "ServEntries", 0, KEY_ALL_ACCESS, &hSubKey);
		BOOL ForPath(const char *path, BOOL soundFunc(const char *, void *), void *data);
		ForPath(servers, AddToList, hSubKey);
		if (hSubKey) RegCloseKey(hSubKey);
	} else {
		CString serverList;
		serverList.LoadString(IDS_DEFAULT_SERVERLIST);
		LoadServerList(serverList);
	}
}

CServer *GetServer(int i) inline {
	return ((CServer *) theApp.m_servers[i]);
}

void SaveServerList(HKEY hKey) {
	CString servers;
	int upper = theApp.m_servers.GetUpperBound();
	int len = 0;
	for (int i = 0; i <= upper; i++) {
		// limit size to maxpath for now
		CServer *serv = GetServer(i);
		int sublen = serv->m_strName.GetLength() + 1;  // be conservative and add semicolon
		if (len + sublen > MAX_PATH) break;
		servers += serv->m_strName;
		if (i < upper) servers += ";";
		len += sublen;
	}
	RegSetValueEx (hKey, "ServerList", 0, REG_SZ, 
						(const unsigned char *)(LPCTSTR)servers, 
						strlen(servers)+1);
}


BOOL CChatApp::LoadFromReg() {
	DWORD		cbData = 0;
	int			i = 0;
	char		szBuff[(MAX_FORMATTINGPERBYTE+1)*MAX_INPUTLEN];
	CDWordArray	rgdwFormatting;
	char		*szControlFull = NULL, *szControlLess = NULL;

	HKEY	hKeyLM = NULL;
	if (RegOpenKeyEx (HKEY_LOCAL_MACHINE, szRootRegKeyName, 
						0, KEY_READ, &hKeyLM) == ERROR_SUCCESS) 
	{
		// RamuM, get the ArtDir from HKLM Reg
		cbData = sizeof(szBuff);
		szBuff[0]= NULL;
		RegQueryValueEx (hKeyLM, szBaseDirValName, 0, NULL, (unsigned char *)szBuff, &cbData);
		if (szBuff[0]) 
			m_strBaseDir = szBuff;


		cbData = sizeof(szBuff);
		szBuff[0]= NULL;
		RegQueryValueEx (hKeyLM, szArtDirValName, 0, NULL, (unsigned char *)szBuff, &cbData);

		if (szBuff[0]) {
			m_strDefaultArtDir = szBuff;   // art dir that we will switch to if none requested
			SetArtDir(szBuff);					// this is used (unless overridden)
		}
	}

	RegCloseKey(hKeyLM);

	InitVals();

	// open the key
	HKEY	hKey = NULL;
	if (RegOpenKeyEx (HKEY_CURRENT_USER, szRootRegKeyName, 
						0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS)  {

		// if that succeeded, read values
		cbData = sizeof(m_xFrame);
		RegQueryValueEx (hKey, "XFrame", 0, NULL, (LPBYTE)&m_xFrame, 
						&cbData);

		cbData = sizeof(m_yFrame);
		RegQueryValueEx (hKey, "YFrame", 0, NULL, (LPBYTE)&m_yFrame, 
						&cbData);

		cbData = sizeof(m_cxFrame);
		RegQueryValueEx (hKey, "CXFrame", 0, NULL, (LPBYTE)&m_cxFrame, 
						&cbData);

		cbData = sizeof(m_cyFrame);
		RegQueryValueEx (hKey, "CYFrame", 0, NULL, (LPBYTE)&m_cyFrame, 
						&cbData);

		cbData = sizeof(m_maxedFrame);
		RegQueryValueEx (hKey, "Maximized", 0, NULL, (LPBYTE)&m_maxedFrame, 
						&cbData);

		cbData = sizeof(i);
		if (RegQueryValueEx (hKey, "UPNLWidth", 0, NULL, (LPBYTE)&i, &cbData) == ERROR_SUCCESS)
			CUnitPanelPage::SetUnitPanelWidth(i);

		cbData = sizeof(i);
		if (RegQueryValueEx (hKey, "UPNLHeight", 0, NULL, (LPBYTE)&i, &cbData) == ERROR_SUCCESS)
			CUnitPanelPage::SetUnitPanelHeight(i);

		cbData = sizeof(i);
		if (RegQueryValueEx (hKey, "UnitsWide", 0, NULL, (LPBYTE)&i, &cbData) == ERROR_SUCCESS)
			CUnitPanelPage::SetUnitPanelsPerRow(i);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "FavoritesDir", 0, NULL, (unsigned char *) szBuff, &cbData) == ERROR_SUCCESS)
			m_strFavoritesDir = szBuff;

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "LastFavorite", 0, NULL, (unsigned char *) szBuff, &cbData) == ERROR_SUCCESS)
			m_strChatRooms = szBuff;
		else m_strChatRooms = "";

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "FileTXDir", 0, NULL, (unsigned char *) szBuff, &cbData) == ERROR_SUCCESS)
			m_strFileTXDir = szBuff;

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "IRCServer", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_ircServer = szBuff;

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "IRCChannel", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_myChannel = szBuff;

		cbData = sizeof(szBuff);
		void SetMyName(const char *);
		if (RegQueryValueEx (hKey, "Name", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			SetMyName(szBuff);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "RealName", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_myRealName = szBuff;

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "Email", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_strEmail = szBuff;

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "HomePage", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_strHomePage = szBuff;

		*szBuff = NULL;		//RamuM
		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, szProfileValName, 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_myProfile = szBuff;

		*szBuff = NULL;
		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "AwayMsg", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_strAwayMessage = szBuff;

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "Character", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_myCharacterName = szBuff;

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "Backdrop", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_lastBackDrop = szBuff;

		cbData = sizeof(m_bComicView);
		RegQueryValueEx (hKey, "ShowComicView", 0, NULL, (LPBYTE)&m_bComicView, 
						&cbData);

		void SetSendComicsData(BOOL);
		cbData = sizeof(i);
		if (RegQueryValueEx (hKey, "ComicsData", 0, NULL, (LPBYTE)&i, &cbData) == ERROR_SUCCESS)
			SetSendComicsData(i);

		cbData = sizeof(m_bIconMembers);
		RegQueryValueEx (hKey, "MemberListStyle", 0, NULL, (LPBYTE)&m_bIconMembers,
						&cbData);

		cbData = sizeof(m_bPrompt);
		RegQueryValueEx (hKey, "PromptForSave", 0, NULL, (LPBYTE)&m_bPrompt,
						&cbData);

		cbData = sizeof(m_bAcceptWhispers);
		RegQueryValueEx (hKey, "AcceptWhispers", 0, NULL, (LPBYTE)&m_bAcceptWhispers,
						&cbData);
		
		cbData = sizeof(i);
		if (RegQueryValueEx (hKey, "FloodControl", 0, NULL, (LPBYTE)&i, &cbData) == ERROR_SUCCESS) {
			BYTE floodInterval = i & 0x000000ff;
			BYTE floodCount = (i & 0x0000ff00) >> 8;
			m_floodFlags = (i & 0x00ff0000) >> 16;
			if (floodInterval && floodCount) {
				m_floodCount = floodCount;
				m_floodInterval = floodInterval;
			}
		}

		// Load Font Information
		cbData = sizeof(m_comicsFont);
		RegQueryValueEx(hKey, "ComicsFont", 0, NULL, (LPBYTE)&m_comicsFont, &cbData);

		// REGISB added 08/28 for default Comics color
		cbData = sizeof(COLORREF);
		RegQueryValueEx(hKey, "ComicsColor", 0, NULL, (LPBYTE)&m_comicsColor, &cbData);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "SoundPath", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			SetSoundPath(szBuff, FALSE);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "AutoGreeting", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_strGreetingMesg = CString(szBuff);

		cbData = sizeof(m_iGreetingType);
		RegQueryValueEx (hKey, "AutoGreetType", 0, NULL, (LPBYTE)&m_iGreetingType,
						 &cbData);

		cbData = sizeof(m_cfArray);
		if (RegQueryValueEx (hKey, "TextFonts", 0, NULL, (LPBYTE)m_cfArray, &cbData) == ERROR_SUCCESS &&
			cbData == sizeof(m_cfArray))
		{
			m_bCfInitialized = TRUE;
			if (m_cfArray[2].dwMask & CFM_COLOR)
				m_textColor = m_cfArray[2].crTextColor;
		}

		cbData = sizeof(m_iHostHighlight);
		RegQueryValueEx (hKey, "HostHighlight", 0, NULL, (LPBYTE)&m_iHostHighlight, 
						&cbData);

		cbData = sizeof(m_textSpacing);
		RegQueryValueEx (hKey, "TextSpacing", 0, NULL, (LPBYTE)&m_textSpacing, 
						&cbData);

		cbData = sizeof(m_bShowArrivals);
		RegQueryValueEx (hKey, "ShowArrivals", 0, NULL, (LPBYTE)&m_bShowArrivals, 
						&cbData);

		cbData = sizeof(m_bAllowInvites);
		RegQueryValueEx (hKey, "AllowInvites", 0, NULL, (LPBYTE)&m_bAllowInvites, 
						&cbData);

		cbData = sizeof(m_bAllowFileTX);
		RegQueryValueEx (hKey, "AllowFileTXs", 0, NULL, (LPBYTE)&m_bAllowFileTX, 
						&cbData);

		cbData = sizeof(m_iShowBars);
		RegQueryValueEx (hKey, "ShowBars", 0, NULL, (LPBYTE)&m_iShowBars, 
						&cbData);

		cbData = sizeof(m_bPlaySounds);
		RegQueryValueEx (hKey, "PlaySounds", 0, NULL, (LPBYTE)&m_bPlaySounds, 
						&cbData);

		cbData = sizeof(m_bAcceptNMCalls);
		RegQueryValueEx (hKey, "AcceptNMCalls", 0, NULL, (LPBYTE)&m_bAcceptNMCalls, 
						&cbData);

		cbData = sizeof(m_bShowIdentity);
		RegQueryValueEx (hKey, "ShowIdentity", 0, NULL, (LPBYTE)&m_bShowIdentity, 
						&cbData);

		cbData = sizeof(m_bListRegistered);
		RegQueryValueEx (hKey, "ListRegistered", 0, NULL, (LPBYTE)&m_bListRegistered, 
						&cbData);

		cbData = sizeof(m_flags1);
		RegQueryValueEx (hKey, "Flags1", 0, NULL, (LPBYTE)&m_flags1,
						 &cbData);

		cbData = sizeof(m_whisperRect);
		RegQueryValueEx (hKey, "WhisperDims", 0, NULL, (LPBYTE)&m_whisperRect,
						 &cbData);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "ServerList", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			LoadServerList(szBuff, hKey);
		else if (strnicmp(m_ircServer, "mschat", 6)==0)
			LoadServerList(NULL); 
	} else {
		LoadServerList(NULL);			// otherwise, let's still fill in the list
	}

	RegCloseKey (hKey);

	if (m_myCharacterName == "") {
		char randName[_MAX_FNAME];
		void GetNextAvatarName(char *);
		GetNextAvatarName(randName);
		m_myCharacterName = randName;
	}

	if (m_strFileTXDir.IsEmpty() && !m_bDoCB32) InitFileTXDir();

	if (!m_bFoundArt) {			// force text mode if we don't have art
		m_bComicView = FALSE;
		m_bSaveViewMode = FALSE;
	}

	LoadMacros();

	return TRUE;
}


BOOL CChatApp::SaveToReg() {
	CString strControlFull;

	// get current data
	CWnd *frame = GetFrame();
	if (frame) {
		WINDOWPLACEMENT wp;
		frame->GetWindowPlacement(&wp);
		m_xFrame = wp.rcNormalPosition.left;
		m_yFrame = wp.rcNormalPosition.top;
		m_cxFrame = wp.rcNormalPosition.right - wp.rcNormalPosition.left + 1;
		m_cyFrame = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top + 1;
		m_maxedFrame = (wp.showCmd == SW_SHOWMAXIMIZED);
	}

	// open the application's key
	HKEY	hKey = NULL;
	if (RegCreateKeyEx (HKEY_CURRENT_USER, szRootRegKeyName, 
						0, "Application Global Data", REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,	NULL, &hKey, NULL) == ERROR_SUCCESS) {

		// write the values
		RegSetValueEx (hKey, "XFrame", 0, REG_DWORD, (LPBYTE)&m_xFrame, 
						sizeof(m_xFrame));

		RegSetValueEx (hKey, "YFrame", 0, REG_DWORD, (LPBYTE)&m_yFrame, 
						sizeof(m_yFrame));

		RegSetValueEx (hKey, "CXFrame", 0, REG_DWORD, (LPBYTE)&m_cxFrame, 
						sizeof(m_cxFrame));

		RegSetValueEx (hKey, "CYFrame", 0, REG_DWORD, (LPBYTE)&m_cyFrame, 
						sizeof(m_cyFrame));

		RegSetValueEx (hKey, "Maximized", 0, REG_DWORD, (LPBYTE)&m_maxedFrame, 
						sizeof(m_maxedFrame));
		int i = 0;
		i = CUnitPanelPage::GetUnitPanelWidth();
		RegSetValueEx (hKey, "UPNLWidth", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		i = CUnitPanelPage::GetUnitPanelHeight();
		RegSetValueEx (hKey, "UPNLHeight", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		i = CUnitPanelPage::GetUnitPanelsPerRow();
		RegSetValueEx (hKey, "UnitsWide", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		if (!m_bDoCB32) {
			RegSetValueEx (hKey, "FavoritesDir", 0, REG_SZ,
							(const unsigned char *)(LPCTSTR)m_strFavoritesDir,
							strlen(m_strFavoritesDir)+1);

			RegSetValueEx (hKey, "LastFavorite", 0, REG_SZ,
							(const unsigned char *)(LPCTSTR)m_strChatRooms,
							strlen(m_strChatRooms)+1);

			RegSetValueEx (hKey, "FileTXDir", 0, REG_SZ,
							(const unsigned char *)(LPCTSTR)m_strFileTXDir,
							strlen(m_strFileTXDir)+1);


			RegSetValueEx (hKey, "IRCServer", 0, REG_SZ, 
							(const unsigned char *)(LPCTSTR)m_ircServer, 
							strlen(m_ircServer)+1);

			RegSetValueEx (hKey, "IRCChannel", 0, REG_SZ, 
							(const unsigned char *)(LPCTSTR)m_myChannel, 
							strlen(m_myChannel)+1);

			RegSetValueEx (hKey, "Name", 0, REG_SZ, 
							(const unsigned char *)(LPCTSTR)m_myName, 
							strlen(m_myName)+1);

			RegSetValueEx (hKey, "RealName", 0, REG_SZ, 
							(const unsigned char *)(LPCTSTR)m_myRealName, 
							strlen(m_myRealName)+1);

			RegSetValueEx (hKey, "Email", 0, REG_SZ, 
							(const unsigned char *)(LPCTSTR)m_strEmail, 
							strlen(m_strEmail)+1);

			RegSetValueEx (hKey, "AutoGreeting", 0, REG_SZ, (const unsigned char*) (LPCTSTR) m_strGreetingMesg,
							strlen(m_strGreetingMesg)+1);

			RegSetValueEx (hKey, "AutoGreetType", 0, REG_DWORD, (LPBYTE)&m_iGreetingType,
							sizeof(m_iGreetingType));
		}

		RegSetValueEx(hKey, "HomePage", 0, REG_SZ, 
					  (const unsigned char *)(LPCTSTR)m_strHomePage, 
					  strlen(m_strHomePage)+1);

		//RamuM - To Avoid possible bug if GetProfileString isn't called!
		if ("" != m_myProfile)
			RegSetValueEx(hKey, szProfileValName, 0, REG_SZ, 
						  (const unsigned char*) (LPCTSTR) m_myProfile, 
						  strlen(m_myProfile)+1);

		RegSetValueEx(hKey, "AwayMsg", 0, REG_SZ,
					  (const unsigned char*) (LPCTSTR) m_strAwayMessage, 
					  strlen(m_strAwayMessage)+1);

		if (m_bComicView) {  // see if we can remove the conditional...
			RegSetValueEx (hKey, "Character", 0, REG_SZ, 
							(const unsigned char *)(LPCTSTR)m_myCharacterName, 
							strlen(m_myCharacterName)+1);

			RegSetValueEx (hKey, "Backdrop", 0, REG_SZ, 
							(const unsigned char *)(LPCTSTR)m_lastBackDrop, 
							strlen(m_lastBackDrop)+1);
		}

		if (m_bSaveViewMode)		// don't save if overridden by URL!
			RegSetValueEx (hKey, "ShowComicView", 0, REG_DWORD, (LPBYTE)&m_bComicView, 
							sizeof(m_bComicView));

		BOOL GetSendComicsData();
		i = GetSendComicsData();
		RegSetValueEx (hKey, "ComicsData", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		RegSetValueEx (hKey, "MemberListStyle", 0, REG_DWORD, (LPBYTE)&m_bIconMembers, sizeof(m_bIconMembers));
		
		RegSetValueEx (hKey, "PromptForSave", 0, REG_DWORD, (LPBYTE)&m_bPrompt, sizeof(m_bPrompt));

		RegSetValueEx (hKey, "AcceptWhispers", 0, REG_DWORD, (LPBYTE)&m_bAcceptWhispers, sizeof(m_bAcceptWhispers));

		// REGISB added 09/26/97
		i = m_floodInterval + (m_floodCount << 8) + (m_floodFlags << 16);
		RegSetValueEx (hKey, "FloodControl", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		// Font Info
		RegSetValueEx (hKey, "ComicsFont", 0, REG_BINARY,
			           (const unsigned char *)&m_comicsFont, sizeof(m_comicsFont));

		// REGISB added 08/28/97
		RegSetValueEx (hKey, "ComicsColor", 0, REG_BINARY, (LPBYTE)&m_comicsColor,
						sizeof(COLORREF));

		RegSetValueEx (hKey, "SoundPath", 0, REG_SZ,
					   (const unsigned char *)(LPCTSTR)m_soundPath,
					   strlen(m_soundPath)+1);

		if (m_bCfInitialized)
			RegSetValueEx (hKey, "TextFonts", 0, REG_BINARY, (const unsigned char *)m_cfArray,
						   sizeof(m_cfArray));
		else
			RegSetValueEx (hKey, "TextFonts", 0, REG_BINARY, (const unsigned char *)"",
							0);  // zero length or non-existant entry means use defaults

		RegSetValueEx (hKey, "HostHighlight", 0, REG_DWORD, (LPBYTE)&m_iHostHighlight,
					    sizeof(m_iHostHighlight));
		RegSetValueEx (hKey, "TextSpacing", 0, REG_DWORD, (LPBYTE)&m_textSpacing,
						sizeof(m_textSpacing));
		RegSetValueEx (hKey, "ShowArrivals", 0, REG_DWORD, (LPBYTE)&m_bShowArrivals,
						sizeof(m_bShowArrivals));
		RegSetValueEx (hKey, "AllowInvites", 0, REG_DWORD, (LPBYTE)&m_bAllowInvites,
						sizeof(m_bAllowInvites));
		RegSetValueEx (hKey, "AllowFileTXs", 0, REG_DWORD, (LPBYTE)&m_bAllowFileTX,
						sizeof(m_bAllowFileTX));
		m_iShowBars = GetBarStatus();
		RegSetValueEx (hKey, "ShowBars", 0, REG_DWORD, (LPBYTE)&m_iShowBars,
						sizeof(m_iShowBars));
		RegSetValueEx (hKey, "PlaySounds", 0, REG_DWORD, (LPBYTE)&m_bPlaySounds,
						sizeof(m_bPlaySounds));
		RegSetValueEx (hKey, "AcceptNMCalls", 0, REG_DWORD, (LPBYTE)&m_bAcceptNMCalls,
						sizeof(m_bAcceptNMCalls));
		RegSetValueEx (hKey, "ShowIdentity", 0, REG_DWORD, (LPBYTE)&m_bShowIdentity,
						sizeof(m_bShowIdentity));
		RegSetValueEx (hKey, "ListRegistered", 0, REG_DWORD, (LPBYTE)&m_bListRegistered,
						sizeof(m_bListRegistered));
		RegSetValueEx (hKey, "Flags1", 0, REG_DWORD, (LPBYTE)&m_flags1,
						sizeof(m_flags1));
		RegSetValueEx (hKey, "WhisperDims", 0, REG_BINARY, (LPBYTE)&m_whisperRect,
						sizeof(m_whisperRect));
		SaveServerList(hKey);
	}
	RegCloseKey (hKey);

	SaveMacros();
	return TRUE;
}

BOOL CChatDoc::ChatSaveLocator(CArchive &f) {
	CString	strTmp;

	f.WriteString("#CHATLOCATOR\r\n");
	strTmp.Format("IRCSERVER:\t%s\r\n", theApp.m_ircServer);
	f.WriteString(strTmp);
	strTmp.Format("IRCCHANNEL:\t%s\r\n", m_proto->m_strPrettyChannel);
	f.WriteString(strTmp);
	f.WriteString("CXPROMPT:\t0\r\n");		// for now, locators don't prompt

	return TRUE;
}

// doesn't work in the general case, but works with #ALPHA strings which is fine for now
BOOL ForwardToKey(CArchive &f, const char *key) {
	CString strBuff;
	int len = strlen(key);
	char c;
	const char *kptr = key;

	while (TRUE) {
		if (!*kptr) {  // we've matched totally -- advance to next line
			f.ReadString(strBuff);
			return TRUE;
		}
		if (f.Read(&c, 1) != 1) return FALSE;
		if (c == *kptr)    // match so far
			kptr++;
		else kptr = key;
	}
	return FALSE;
}

#define LOCATORSTRING	"#CHATLOCATOR"
#define VBUFFLEN	_MAX_FNAME

BOOL CChatDoc::ChatLoadLocator(CArchive &f, BOOL bJoin, BOOL bDoException) {
	char key[50], value[VBUFFLEN];
	CString strBuff;
	extern int iViewMode;
	extern BOOL bCXKeepServer;
	BOOL bSwitchToRoom(const char *room, const char *password = NULL, BOOL bEncode = FALSE);

	if (!ForwardToKey(f, LOCATORSTRING)) {
		AfxMessageBox(ID_ERR_NOT_CHATLOC);
		return FALSE;
	}

	int iConn = GetDefaultProto()->GetConnectionStatus();
	BOOL onLine = (iConn == CX_INCHANNEL || iConn == CX_NOCHANNEL);

	iViewMode = VM_UNSPECIFIED;

	while(f.ReadString(strBuff)) {
		if (sscanf(strBuff, "%49s", key) < 1) break;		// probably a null line
		if (!stricmp(key, "IRCSERVER:")) {
			if (GetValue(strBuff, value, VBUFFLEN)) {
				if (stricmp(value, theApp.m_ircServer) != 0 || !onLine) {
					theApp.m_ircServer = value;
					bCXKeepServer = FALSE;
				} else bCXKeepServer = TRUE;
			}
		} else if (!stricmp(key, "IRCCHANNEL:")) {
			if (GetValue(strBuff, value, VBUFFLEN)) {
				VERIFY(bInitEnterInfo(value, NULL, TRUE));
				theApp.m_myChannel = value;
			}
		} else if (!stricmp(key, "CXPROMPT:")) {
			if (GetValue(strBuff, value, VBUFFLEN));
			void ChatSetCXPrompt(BOOL);
			ChatSetCXPrompt(atoi(value));
		} else if (!stricmp(key, "CHARACTER:")) {
			if (GetValue(strBuff, value, VBUFFLEN))
				theApp.m_myCharacterName = value;
		} else if (!stricmp(key, "BACKDROP:")) {
			if (GetValue(strBuff, value, VBUFFLEN))
				theApp.m_lastBackDrop = value;
		} else if (!stricmp(key, "COMICSDATA:")) {
			if (GetValue(strBuff, value, VBUFFLEN)) {
				void SetSendComicsData(BOOL);
				SetSendComicsData(atoi(value));
			}
		} else if (!stricmp(key, "TITLE:")) {
			if (GetValue(strBuff, value, VBUFFLEN)) {
				if (*value) GetChatDoc()->SetComicsTitle2(value);
			}
		} else if (!stricmp(key, "ARTDIR:")) {
			if (GetValue(strBuff, value, VBUFFLEN)) {
				if (*value) SetArtDir(value);
			}
		} else if (!stricmp(key, "VIEW:")) {
			if (GetValue(strBuff, value, VBUFFLEN)) {
				if (!stricmp(value, "Comics"))
					iViewMode = VM_COMICS;
				else if (!stricmp(value, "Text"))
					iViewMode = VM_TEXT;
			}
		} else {
//			TRACE("Got an unknown key: %s.\n", key);  // this can happen now, due to extra docfile chars at end of file
		}
	}

	if (bJoin && bCXKeepServer) {
		bSwitchToRoom(NULL);		// will move window to foreground if it's already there :-)
		if (LookupDoc(enterInfo.m_strChannel)) {
			if (bDoException)
				AfxThrowFileException(EX_DONTREPORT);
			else
				return FALSE;
		}
		bCXKeepServer = TRUE;	// need to reset again since bSwitchToRoom (InitializeChannelConnection) will effectively unset it
	}
	return TRUE;
}

const char *GetMyName() {
	return ((LPCTSTR) theApp.m_myName);
}

const char *GetMyNickName() {
	return ((LPCTSTR) theApp.m_myNick);
}

void SetMyName(const char *szCharName) {
	theApp.m_myName = szCharName;
	theApp.m_myNick = szCharName;
}

void SetMyNameNick(const char *szNickname) {
	ASSERT(szNickname);
	const char *DecodeNick(const char *);
	theApp.m_myNick = szNickname;
	// REGISB: 11/20/97 removed conditions for bug 4429 fix
	if (/* GetDefaultProto() && GetDefaultProto()->IsIRCX() && */ *szNickname == '\'')
		theApp.m_myName = DecodeNick(szNickname);
	else
		theApp.m_myName = szNickname;
}

const char *GetMyServer() {
	return ((LPCTSTR) theApp.m_ircServer);
}

const char *GetMyRealName() {
	if (theApp.m_myRealName.IsEmpty())
		theApp.m_myRealName.LoadString(IDS_DEFAULT_REALNAME);
	return ((LPCTSTR) theApp.m_myRealName);
}

void SetMyRealName(const char *charName) {
	theApp.m_myRealName = charName;
}

const char *GetMyCharacter() {
	return ((LPCTSTR) theApp.m_myCharacterName);
}

const char *GetMyChannel() {
	return ((LPCTSTR) theApp.m_myChannel);
}

const char *GetMyHomePage() {
	return ((LPCTSTR) theApp.m_strHomePage);
}

const char *GetMyEmail() {
	return ((LPCTSTR) theApp.m_strEmail);
}

const char *GetMyUserName() {
	int at = theApp.m_strEmail.Find('@');
	if (at > 0) {
		CString contents(theApp.m_strEmail, at);
		theApp.m_strUserName = contents;
	} else if (!theApp.m_strEmail.IsEmpty() && at != 0)
		theApp.m_strUserName = theApp.m_strEmail;
	else theApp.m_strUserName = GetMyName();
	return ((LPCTSTR) theApp.m_strUserName);
}

void SetMyCharacter(const char *charName) {
	theApp.m_myCharacterName = charName;
}

void SetMyHomePage(const char *szHomePage) {
	theApp.m_strHomePage = szHomePage;
}

void SetMyEmail(const char *szEmail) {
	theApp.m_strEmail = szEmail;
}

void ChatSetChannel(const char *channelName) {
	theApp.m_myChannel = channelName;
}

// changes default -- does not attach
void ChatSetServer(const char *serverName) {
	theApp.m_ircServer = serverName;
}


BOOL RequestedChannelList() {
	return (theApp.m_iOnConnectAction == CA_ROOMLIST);
}


#define HEADX	266
#define HEADY	72
#define HEADW	55
#define HEADH	55


void CSetupPage::OnCancel() 
{
	AfxMessageBox(IDS_DISCONNECTMESSAGE);
	CCSPropertyPage::OnCancel();
}



BOOL CSetupPage::OnSetActive() 
{
	m_serverCtl.SetFont(&theApp.m_fontGui);
	m_editChannel.SetFont(&theApp.m_fontGui);
	m_ChatRooms.SetFont(&theApp.m_fontGui);

    // Now, setup the chat room dropdown
	CString strRoom;
	strRoom = theApp.m_strFavoritesDir;
	strRoom += "\\*.ccr";
	struct _finddata_t fdRoom;
	long hFindRoom = _findfirst( (char *) (const char *) strRoom, &fdRoom );
	if( hFindRoom != -1 )
	{
		do 
		{
			if (fdRoom.attrib != _A_SUBDIR)
			{
				char szFName[_MAX_FNAME];

				_splitpath( fdRoom.name, NULL, NULL, szFName, NULL /*szExt*/ );
				// CharUpperBuff(szFName, 1);		// 1 doesn't work w/ NT & DBCS.
				FirstCharUpper(szFName);			// REGISB 10/15/97
				m_ChatRooms.AddString(szFName);
		   }

		} while( _findnext( hFindRoom, &fdRoom ) != -1 );

		int index = m_ChatRooms.FindStringExact(0, m_strChatRooms);
		if (index != LB_ERR) {
			m_ChatRooms.SetCurSel(index);
		} else {
			m_ChatRooms.SetCurSel(-1);
		}
	}
	else	// if there is no favorites, disable the dropdown
		m_ChatRooms.EnableWindow(FALSE);

	m_ChatRooms.SetCurSel(-1);

	// load server list from theApp.m_servers
	int upper = theApp.m_servers.GetUpperBound();
	for (int i = 0; i <= upper; i++)
		m_serverCtl.AddString(GetServer(i)->m_strName);

	if(m_radioConnect == 0)
		OnConchan();
	else
		OnListchan();

	switch(nWhatFailed)
	{
	case CHATROOM:
		GetDlgItem(IDC_CHATROOMS)->SetFocus();
		break;
	case SERVER:
		GetDlgItem(IDC_SERVER)->SetFocus();
		((CEdit*)GetDlgItem(IDC_SERVER))->SetSel(0,-1);
		break;
	case CHANNEL:
		GetDlgItem(IDC_CHANNEL)->SetFocus();
		((CEdit*)GetDlgItem(IDC_CHANNEL))->SetSel(0,-1);
		break;
	}


	nWhatFailed = CHATROOM;

	if (theApp.m_bComicView)
		GetDlgItem(IDC_COMICSWELCOME)->ShowWindow(SW_SHOWNA);

	
	return CCSPropertyPage::OnSetActive();
}


// Update the ok button based on a non-empty channelname
void CSetupPage::UpdateOk()
{
	CString strTemp, strTemp2;
	m_editChannel.GetWindowText(strTemp);
	m_serverCtl.GetWindowText(strTemp2);
	EnableParentOK(!strTemp.IsEmpty() && !strTemp2.IsEmpty());
}



void CSetupPage::EnableParentOK(BOOL bEnable) {
	CCSPropertyPage *parent = (CCSPropertyPage *) GetParent();
	CWnd *ok = parent->GetDlgItem(IDOK);
	ok->EnableWindow(bEnable);
}

	
/////////////////////////////////////////////////////////////////////////////
// CFilterEdit

CFilterEdit::CFilterEdit()
{
	m_strInvalid = " ,";  // default invalid characters
	m_bPasting	 = FALSE;
}


// This will check the current character against a list of invalid characters
// to determine if we should let it into the nickname
BOOL CFilterEdit::CheckIfInvalid(UINT nChar)
{
	return (m_strInvalid.Find((TCHAR) nChar) >= 0);
}


BEGIN_MESSAGE_MAP(CFilterEdit, CEdit)
	//{{AFX_MSG_MAP(CFilterEdit)
	ON_WM_CHAR()
	ON_MESSAGE(WM_PASTE, OnPaste)
	ON_CONTROL_REFLECT_EX(EN_CHANGE, OnChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CFilterEdit message handlers

// Validate character and then pass on or beep if invalid
void CFilterEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(CheckIfInvalid(nChar))
	{
		MessageBeep(0xFFFFFFFF);
		return;
	}

	//if ((GetKeyState(VK_CONTROL) & 0x8000) && (1 == nRepCnt) && (CTRL_V	== nChar))	// Ctrl V for pasting - bad solution
	//	m_bPasting = TRUE;
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CFilterEdit::OnChange()
{
	// OutputDebugString("OnChange\n");

	if (m_bPasting)
	{
		// OutputDebugString("OnChange while pasting\n");
		m_bPasting = FALSE;

		const int	nMaxPaste = 256;
		int			i = 0, j = 0, k;
		char		szTemp2[nMaxPaste];
		CString		strTemp1;
		int			nStartChar, nEndChar;

		GetSel(nStartChar, nEndChar);
		ASSERT(nStartChar == nEndChar);
		GetWindowText(strTemp1);
		k = strTemp1.GetLength();
		while (i < k && j < nMaxPaste - 1)
		{
			if(CheckIfInvalid(strTemp1[i]))
			{
				nStartChar--;
				ASSERT(nStartChar >= 0);
			}
			else
			{
				szTemp2[j++] = strTemp1[i];
			}
			i++;
		}
		szTemp2[j] = g_chEOS;
		SetWindowText((LPCTSTR) szTemp2);
		SetSel(nStartChar, nStartChar);
	}
	return FALSE;
}


LRESULT CFilterEdit::OnPaste(WPARAM wParam, LPARAM lParam)
{
	// OutputDebugString("OnPaste\n");
	m_bPasting = TRUE;	

	return DefWindowProc(WM_PASTE, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////
// CChanFilterEdit

CChanFilterEdit::CChanFilterEdit()
{
	m_strInvalid = " ,\r\n\a";  // default invalid characters for IRC channel names  (space, comma, carriage return, linefeed, bell)
}


// REGISB 10/21/97 might be reused later again
//BOOL CChanFilterEdit::CheckIfInvalid(UINT nChar)
//{
//	CString strCurChan;
//	GetWindowText(strCurChan);
//	
//	if (strCurChan.IsEmpty())
//		return CFilterEdit::CheckIfInvalid(nChar);
//
//	if (strCurChan[0] == '#' || strCurChan[0] == '&')
//		return CFilterEdit::CheckIfInvalid(nChar);
//	else
//		return FALSE;	// All characters are allowed for IRCX channels
//}


void CSetupPage::OnSelchangeChatrooms() 
{
	void ChatSetCXPrompt(BOOL);

	// need to load the relevant file...
	int curSel = m_ChatRooms.GetCurSel();
	if(curSel == -1)
		return;
	m_ChatRooms.GetLBText(curSel, theApp.m_strChatRooms);
	CString filename;
	filename.Format("%s\\%s.ccr", theApp.m_strFavoritesDir, theApp.m_strChatRooms);

	// now create a CFile...
	extern char* pFileName;
	CFile f;
	if(!f.Open(filename, CFile::modeRead)) {
		TRACE("Unable to open %s", filename);		// no signal to user for now
		return;
	}

	// now create a CArchive from the CFile...
	CArchive ar(&f, CArchive::load);

	CChatDoc::ChatLoadLocator(ar, FALSE, FALSE);
	m_ircServer = theApp.m_ircServer;	// only theApp gets updated by ChatLoadLocator
	m_myChannel = theApp.m_myChannel;
	ChatSetCXPrompt(TRUE);				// we want to make sure to prompt next time through

	UpdateData(FALSE);		// update self
	OnConchanAux(FALSE);    // OK/channel field enabled, depending on channelname
}


/////////////////////////////////////////////////////////////////////////////
// CNicknameDlg dialog


CNicknameDlg::CNicknameDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CNicknameDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CNicknameDlg)
	m_label = _T("");
	m_strNickname = _T("");
	//}}AFX_DATA_INIT
}


void CNicknameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNicknameDlg)
	DDX_Control(pDX, IDC_NEWNICK, m_editNick);
	DDX_Control(pDX, IDC_STATICNICKNAME, m_staticNick);
	DDX_Text(pDX, IDC_STATICNICKNAME, m_label);
	DDX_Text(pDX, IDC_NEWNICK, m_strNickname);
	DDV_MaxChars(pDX, m_strNickname, 20);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CNicknameDlg, CDialog)
	//{{AFX_MSG_MAP(CNicknameDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNicknameDlg message handlers

BOOL CNicknameDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_editNick.SetFont(&theApp.m_fontGui);
	m_editNick.SetSel(0, -1);				// select contents for replacement
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupPage::OnConchan() 
{
	OnConchanAux(TRUE);
}

void CSetupPage::OnConchanAux(BOOL bSetFocus) 
{
	CEdit* pEdit = (CEdit*)GetDlgItem(IDC_CHANNEL);
	pEdit->EnableWindow(TRUE);
	if (bSetFocus) {
		pEdit->SetFocus();
		pEdit->SetSel(0,-1);
	}

	EnableParentOK(pEdit->LineLength() > 0);
}

void CSetupPage::OnListchan() 
{
		GetDlgItem(IDC_CHANNEL)->EnableWindow(FALSE);
		EnableParentOK(TRUE);
}

void CSetupPage::OnChangeChannel() 
{
	UpdateOk();
	m_ChatRooms.SetCurSel(-1);
}


void CSetupPage::OnChangeServer() 
{
	UpdateOk();
	m_ChatRooms.SetCurSel(-1);
}
/////////////////////////////////////////////////////////////////////////////
// CChannelDlg dialog


CChannelDlg::CChannelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChannelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChannelDlg)
	m_strChannel = _T("");
	m_strPassword = _T("");
	//}}AFX_DATA_INIT
}


void CChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelDlg)
	DDX_Control(pDX, IDC_PASSWORD, m_passwordCtrl);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_NEWCHANNEL, m_channelCtrl);
	DDX_Text(pDX, IDC_NEWCHANNEL, m_strChannel);
	DDV_MaxChars(pDX, m_strChannel, 200);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 20);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChannelDlg, CDialog)
	//{{AFX_MSG_MAP(CChannelDlg)
	ON_EN_CHANGE(IDC_NEWCHANNEL, OnChangeChannel)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelDlg message handlers

void CChannelDlg::OnChangeChannel() 
{
	CString strTemp;
	m_channelCtrl.GetWindowText(strTemp);
	if(strTemp.IsEmpty())
		m_ok.EnableWindow(FALSE);
	else m_ok.EnableWindow(TRUE);
}

BOOL CChannelDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_passwordCtrl.SetFont(&theApp.m_fontGui);
	m_channelCtrl.SetFont(&theApp.m_fontGui);

	OnChangeChannel();  // enable OK only if characters filled in
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}



BOOL ArtDirsOK() {		// returns TRUE if ArtDirs contain at least one backdrop & character
	struct _finddata_t fdRoom;
	CString pattern = theApp.m_strBackDropDir + "\\*.bmp";
	long hFindRoom = _findfirst( (char *) (const char *) pattern, &fdRoom );
	if( hFindRoom == -1) return FALSE;

	pattern = theApp.m_strAvatarDir + "\\*.avb";
	hFindRoom = _findfirst( (char *) (const char *) pattern, &fdRoom );
	if( hFindRoom == -1) return FALSE;

	return TRUE;
}
/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog


CPasswordDlg::CPasswordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPasswordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPasswordDlg)
	m_strPassword = _T("");
	m_strMessage = _T("");
	//}}AFX_DATA_INIT
}


void CPasswordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPasswordDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, 20);
	DDX_Text(pDX, IDC_PASSWORD_MESG, m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPasswordDlg, CDialog)
	//{{AFX_MSG_MAP(CPasswordDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg message handlers


void AddToServerList(const char *server) {
	int upper = theApp.m_servers.GetUpperBound();
	if (upper >= 0 && GetServer(0)->m_strName == server) return;   // common case

	CServer *newServer = NULL;

	for (int i = 0; i <= upper; i++)
		if (GetServer(i)->m_strName == server) {
			newServer = GetServer(i);
			theApp.m_servers.RemoveAt(i);
			break;
	}

	if (!newServer) newServer = new CServer(server);
	theApp.m_servers.InsertAt(0, newServer);

	while ((i = theApp.m_servers.GetUpperBound()) > 19) {
		delete GetServer(i);
		theApp.m_servers.RemoveAt(i);
	}
}

void CSetupPage::OnOK() 
{
	theApp.m_ircServer = m_ircServer;
	theApp.m_myChannel = m_myChannel;
	theApp.m_iOnConnectAction = m_radioConnect == 0 ? CA_JOINROOM : CA_ROOMLIST;
	
	CCSPropertyPage::OnOK();
}

BOOL CSetupPage::OnKillActive() 
{
	CString save1, save2;
	m_serverCtl.GetWindowText(save1);
	m_ChatRooms.GetWindowText(save2);
	m_serverCtl.ResetContent(); // clear list box but not edit ...
	m_ChatRooms.ResetContent(); // so current choice is not forgotten
	m_serverCtl.SetWindowText(save1);
	m_ChatRooms.SetWindowText(save2);
	
	return CCSPropertyPage::OnKillActive();
}


///////////////////////////////////////////////////////////////////////////////
// CChatFileDialog member functions
///////////////////////////////////////////////////////////////////////////////
void CChatFileDialog::OnTypeChange()
{
	CString strExt;
	switch (m_ofn.nFilterIndex)
	{
	case 1:
		strExt = g_szRTFExt;
		break;
	case 2:
		strExt = g_szCCCExt;
		break;
	default:
		ASSERT(FALSE);
	}

	CString strNewText = GetFileTitle() + "." + strExt;

	SetControlText(edt1, (LPCTSTR) strNewText);
}
