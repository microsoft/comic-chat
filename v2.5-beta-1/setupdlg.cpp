// setupdlg.cpp : implementation file
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
#include <dlgs.h>	// for edt1 definition
#include "chatbars.h"
#include "protsupp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;
extern void FirstCharUpper(char *szName) inline;
void SetArtDir(const char *);
void SetSoundPath(const char *, BOOL);
CChatDoc *LookupDoc(const char *);

void DDV_NonBlankNick(CDataExchange* pDX, CString &str)
{
	if (pDX->m_bSaveAndValidate)
	{
		str.TrimLeft ();
		str.TrimRight ();
		if (str.IsEmpty ())
		{
			::AfxMessageBox (IDS_BLANKNICK, MB_ICONEXCLAMATION | MB_OK);
			pDX->Fail ();
		}
	}
}


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
	CControlBar *toolbar = GetToolBar();
	if (!toolbar || !::IsWindow(toolbar->m_hWnd)) return theApp.m_iShowBars;

	// Toolbar status is kept in a live state. No need to re-query.
	int status = theApp.m_iShowBars & (SB_TOOLBAR_ANY | SB_TOOLBAR | SB_TOOLBAR_OLDREAD);

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
	//}}AFX_DATA_INIT

	m_strService = GetMyServer ();
	GetMyServerDisplayName (m_strServer);
	if (m_strServer.IsEmpty ())
	{
		CChatService* pSvc = NULL;
		if (theApp.m_listChatServices.EnumServices (pSvc))
		{
			m_strServer = pSvc->GetDisplayName ();
			pSvc->FormatAsServiceName (m_strService);
		}
	}

	m_myChannel = theApp.m_myChannel;
	m_radioConnect = theApp.m_iOnConnectAction;

	nWhatFailed = CHATROOM;

	
}

CSetupPage::~CSetupPage()
{
}

void CSetupPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupPage)
	DDX_Control(pDX, IDC_CHANNEL, m_editChannel);
	DDX_Control(pDX, IDC_CHATROOMS, m_ChatRooms);
	DDX_Text(pDX, IDC_SERVER, m_strServer);
	DDV_MaxChars(pDX, m_strServer, 100);
	DDX_Text(pDX, IDC_CHANNEL, m_myChannel);
	DDV_MaxChars(pDX, m_myChannel, MAX_IRCXCHANNAME);
	DDX_CBString(pDX, IDC_CHATROOMS, m_strChatRooms);
	DDX_Radio(pDX, IDC_CONCHAN, m_radioConnect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CSetupPage)
	ON_CBN_SELCHANGE(IDC_CHATROOMS, OnSelchangeChatrooms)
	ON_BN_CLICKED(IDC_CONCHAN, OnConchan)
	ON_BN_CLICKED(IDC_LISTCHAN, OnNotConChan)
	ON_BN_CLICKED(IDC_CONNECTONLY, OnNotConChan)
	ON_EN_CHANGE(IDC_CHANNEL, OnChangeChannel)
	ON_CBN_EDITCHANGE(IDC_SERVER, OnChangeServerTyped)
	ON_CBN_SELCHANGE(IDC_SERVER, OnChangeServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupPage message handlers

BOOL 
CSetupPage::OnInitDialog()
{
	m_serverCtl.ReplaceControl (this, IDC_SERVER);
	return CCSPropertyPage::OnInitDialog ();
}

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
		{
			// Make sure the directory doesn't have a trailing \, because no one expects it.
			int szBuffLen = lstrlen (szBuff);
			if (OurMbsRChr (szBuff, '\\') == szBuff + szBuffLen - 1)
				szBuff[szBuffLen - 1] = '\0';
			m_strBaseDir = szBuff;
		}


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
			m_strConnectedService = szBuff;

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "IRCChannel", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			m_myChannel = szBuff;

		cbData = sizeof(szBuff);
		void SetMyName(const char *);
		if (RegQueryValueEx (hKey, "Name", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
		{
			SetLimitedString(m_myName, szBuff, MAX_NICKINPUT);
			SetLimitedString(m_myNick, szBuff, MAX_NICKINPUT);
		}

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "RealName", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			SetLimitedString(m_myRealName, szBuff, MAX_REALNAMEINPUT);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "Email", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			SetLimitedString(m_strEmail, szBuff, MAX_EMAILINPUT);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, m_bDoCB32 ? "ToolBarState2" : "ToolBarState", 0, NULL, 
				(unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
		{
			PBYTE pbBuf = NULL;
			if (cbData == 0 || (pbBuf = (PBYTE)malloc (cbData)) != NULL)
			{
				if (cbData > 0)
					memcpy (pbBuf, szBuff, cbData);
				free (m_pbCoolBarState);
				m_pbCoolBarState = pbBuf;
			}
		}

		cbData = sizeof(szBuff);
		if (RegQueryValueEx (hKey, "HomePage", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			SetLimitedString(m_strHomePage, szBuff, MAX_HOMEPAGEINPUT);

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
		
		cbData = sizeof(m_bAutoDownloadAvatars);
		RegQueryValueEx (hKey, "AutoDownloadChars", 0, NULL, (LPBYTE)&m_bAutoDownloadAvatars,
						&cbData);

		cbData = sizeof(m_bAutoDownloadBackdrops);
		RegQueryValueEx (hKey, "AutoDownloadBackdrops", 0, NULL, (LPBYTE)&m_bAutoDownloadBackdrops,
						&cbData);

		cbData = sizeof(i);
		if (RegQueryValueEx (hKey, "FloodControl", 0, NULL, (LPBYTE)&i, &cbData) == ERROR_SUCCESS)
		{
			UCHAR uFloodInterval = i & 0x000000ff;
			UCHAR uFloodCount = (i & 0x0000ff00) >> 8;
			m_uFloodFlags = (i & 0x00ff0000) >> 16;
			if (uFloodInterval && uFloodCount)
			{
				m_uFloodCount = uFloodCount;
				m_uFloodInterval = uFloodInterval;
			}
		}

		// REGISB added 03/13/98
		cbData = sizeof(i);
		if (RegQueryValueEx (hKey, "RulesControl", 0, NULL, (LPBYTE) &i, &cbData) == ERROR_SUCCESS)
		{
			UCHAR uInt = i & 0x000000ff;
			UCHAR uOcc = (i & 0x0000ff00) >> 8;
			if (uInt && uOcc)
				m_dynaRules.SetFloodParams(uInt, uOcc);
		}

		// Load Font Information
		cbData = sizeof(m_comicsFont);
		RegQueryValueEx(hKey, "ComicsFont", 0, NULL, (LPBYTE)&m_comicsFont, &cbData);

		// REGISB added 08/28 for default Comics color
		cbData = sizeof(COLORREF);
		RegQueryValueEx(hKey, "ComicsColor", 0, NULL, (LPBYTE)&m_comicsColor, &cbData);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx(hKey, "SoundPath", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
			SetSoundPath(szBuff, FALSE);

		cbData = sizeof(szBuff);
		if (RegQueryValueEx(hKey, "AutoGreeting", 0, NULL, (unsigned char *)szBuff, &cbData) == ERROR_SUCCESS)
		{
			m_strGreetingMesg = CString(szBuff);
			bReplaceMacroTokens(m_strGreetingMesg, TRUE /*bIn*/);
		}

		cbData = sizeof(m_iGreetingType);
		RegQueryValueEx(hKey, "AutoGreetType", 0, NULL, (LPBYTE)&m_iGreetingType, &cbData);

		cbData = sizeof(CHARFORMAT) * NREGULARFONTS;
		if (RegQueryValueEx (hKey, "TextFonts", 0, NULL, (LPBYTE)m_cfArray, &cbData) == ERROR_SUCCESS &&
			cbData == sizeof(CHARFORMAT) * NREGULARFONTS)
		{
			m_bCfInitialized = TRUE;
			if (m_cfArray[2].dwMask & CFM_COLOR)
				m_textColor = m_cfArray[2].crTextColor;
		}

		cbData = sizeof(CHARFORMAT) * NHIGHLIGHTEDFONTS;
		if (RegQueryValueEx (hKey, "HighlightedTextFonts", 0, NULL, (LPBYTE) &m_cfArray[NREGULARFONTS], &cbData) == ERROR_SUCCESS &&
			cbData == sizeof(CHARFORMAT) * NHIGHLIGHTEDFONTS)
			m_bCfHLInitialized = TRUE;

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
		// Translation from old version to new.
		if ((m_iShowBars & (SB_TOOLBAR | SB_TOOLBAR_OLDREAD)) == SB_TOOLBAR)
		{
			m_iShowBars = m_iShowBars | (SB_TOOLBAR_ANY | SB_TOOLBAR_OLDREAD);
		}											    

		cbData = sizeof(m_bPlaySounds);
		RegQueryValueEx (hKey, "PlaySounds", 0, NULL, (LPBYTE)&m_bPlaySounds, 
						&cbData);

		cbData = sizeof(m_bNoMIDI);
		RegQueryValueEx (hKey, "NoMIDI", 0, NULL, (LPBYTE)&m_bNoMIDI,
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

		cbData = sizeof(m_flags0);
		RegQueryValueEx (hKey, "Flags0", 0, NULL, (LPBYTE)&m_flags0,
						 &cbData);

		cbData = sizeof(m_rectWhisper);
		RegQueryValueEx (hKey, "WhisperDims", 0, NULL, (LPBYTE) &m_rectWhisper,
						 &cbData);

		cbData = sizeof(m_rectNotifs);
		RegQueryValueEx (hKey, "NotifDims", 0, NULL, (LPBYTE) &m_rectNotifs,
						 &cbData);

		cbData = sizeof(m_iOnConnectAction);
		RegQueryValueEx (hKey, "OnConnect", 0, NULL, (LPBYTE)&m_iOnConnectAction,
						 &cbData);

	} else {
	}

	m_listChatServices.ReadFromRegistry ();

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

	m_dynaRules.bLoadRulesFromReg(/*TRUE*/);	// Load custom made rules from HKCU
	// m_dynaRules.bLoadRulesFromReg(FALSE);	// Load default rules "General" from HKLM if it's there

	m_dynaNotifs.bLoadNotifsFromReg();

	return TRUE;
}


BOOL CChatApp::SaveToReg(BOOL bShort)
{
	CString strControlFull;
	HKEY	hKey = NULL;

	// open the application's key
	if (RegCreateKeyEx (HKEY_CURRENT_USER, szRootRegKeyName, 
						0, "Application Global Data", REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,	NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		if (bShort)
		{
			// get current data
			CWnd *frame = GetFrame();
			if (frame)
			{
				WINDOWPLACEMENT wp;
				frame->GetWindowPlacement(&wp);
				m_xFrame = wp.rcNormalPosition.left;
				m_yFrame = wp.rcNormalPosition.top;
				m_cxFrame = wp.rcNormalPosition.right - wp.rcNormalPosition.left;
				m_cyFrame = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top;
				m_maxedFrame = (wp.showCmd == SW_SHOWMAXIMIZED);

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
				m_iShowBars = GetBarStatus();
				RegSetValueEx (hKey, "ShowBars", 0, REG_DWORD, (LPBYTE)&m_iShowBars,
								sizeof(m_iShowBars));

				CChatToolBar *toolbar = GetToolBar();
				free (m_pbCoolBarState);
				m_pbCoolBarState = NULL;
				UINT nBytesOut;
				if (toolbar->SaveStateToBuffer (&m_pbCoolBarState, &nBytesOut))
				{
					RegSetValueEx (hKey, m_bDoCB32 ? "ToolBarState2" : "ToolBarState", 0, REG_BINARY,
									(const unsigned char *)m_pbCoolBarState,
									nBytesOut);
				}
			}
			RegCloseKey (hKey);
			return TRUE;
		}

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
							(const unsigned char *)(LPCTSTR)m_strConnectedService,
							strlen(m_strConnectedService)+1);

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

			CString strReplaced = m_strGreetingMesg;
			bReplaceMacroTokens(strReplaced, FALSE /*bIn*/);
			RegSetValueEx (hKey, "AutoGreeting", 0, REG_SZ, (const unsigned char*) (LPCTSTR) strReplaced,
							strlen(strReplaced)+1);

			RegSetValueEx (hKey, "AutoGreetType", 0, REG_DWORD, (LPBYTE)&m_iGreetingType,
							sizeof(m_iGreetingType));

			RegSetValueEx (hKey, "OnConnect", 0, REG_DWORD, (LPBYTE)&m_iOnConnectAction,
							sizeof(m_iOnConnectAction));
		}

		m_listChatServices.WriteToRegistry ();

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

		BOOL bServersMigrated = TRUE;
		RegSetValueEx (hKey, "ServersMigrated", 0, REG_DWORD, 
			(PBYTE)&bServersMigrated, sizeof(bServersMigrated));

		if (m_bSaveViewMode)		// don't save if overridden by URL!
			RegSetValueEx (hKey, "ShowComicView", 0, REG_DWORD, (LPBYTE)&m_bComicView, 
							sizeof(m_bComicView));

		i = GetSendComicsData();
		RegSetValueEx (hKey, "ComicsData", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		RegSetValueEx (hKey, "MemberListStyle", 0, REG_DWORD, (LPBYTE)&m_bIconMembers, sizeof(m_bIconMembers));
		
		RegSetValueEx (hKey, "PromptForSave", 0, REG_DWORD, (LPBYTE)&m_bPrompt, sizeof(m_bPrompt));

		RegSetValueEx (hKey, "AcceptWhispers", 0, REG_DWORD, (LPBYTE)&m_bAcceptWhispers, sizeof(m_bAcceptWhispers));

		RegSetValueEx (hKey, "AutoDownloadChars", 0, REG_DWORD, (LPBYTE)&m_bAutoDownloadAvatars, sizeof(m_bAutoDownloadAvatars));

		RegSetValueEx (hKey, "AutoDownloadBackdrops", 0, REG_DWORD, (LPBYTE)&m_bAutoDownloadBackdrops, sizeof(m_bAutoDownloadBackdrops));

		// REGISB added 09/26/97
		i = m_uFloodInterval + (m_uFloodCount << 8) + (m_uFloodFlags << 16);
		RegSetValueEx (hKey, "FloodControl", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		// REGISB added 03/13/98
		i = m_dynaRules.GetFloodingInterval() + (m_dynaRules.GetFloodingOccurrences() << 8);
		RegSetValueEx (hKey, "RulesControl", 0, REG_DWORD, (LPBYTE) &i, sizeof(int));

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
			RegSetValueEx(hKey, "TextFonts", 0, REG_BINARY, (const unsigned char*) m_cfArray,
						  sizeof(CHARFORMAT) * NREGULARFONTS);
		else
			RegSetValueEx(hKey, "TextFonts", 0, REG_BINARY, (const unsigned char*) "", 0);  // zero length or non-existant entry means use defaults

		if (m_bCfHLInitialized)
			RegSetValueEx(hKey, "HighlightedTextFonts", 0, REG_BINARY, (const unsigned char*) &m_cfArray[NREGULARFONTS],
						  sizeof(CHARFORMAT) * NHIGHLIGHTEDFONTS);
		else
			RegSetValueEx(hKey, "HighlightedTextFonts", 0, REG_BINARY, (const unsigned char*) "", 0);  // zero length or non-existant entry means use defaults

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
		m_flags0 |= F0_ALREADYRUN;
		RegSetValueEx (hKey, "Flags0", 0, REG_DWORD, (LPBYTE)&m_flags0,
						sizeof(m_flags0));
		RegSetValueEx (hKey, "WhisperDims", 0, REG_BINARY, (LPBYTE) &m_rectWhisper,
						sizeof(m_rectWhisper));
		RegSetValueEx (hKey, "NotifDims", 0, REG_BINARY, (LPBYTE) &m_rectNotifs,
						sizeof(m_rectNotifs));
	}
	RegCloseKey (hKey);

	SaveMacros();

	m_dynaRules.bSaveRulesToReg();
	m_dynaNotifs.bSaveNotifsToReg();
	return TRUE;
}


BOOL CChatDoc::ChatSaveLocator(CArchive &f) {
	CString strTmp;

	f.WriteString("#CHATLOCATOR\r\n");
	strTmp.Format("IRCSERVER:\t%s\r\n", GetMyServer ());
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

BOOL CChatDoc::ChatLoadLocator(CArchive &f, BOOL bJoin, BOOL bDoException, SHORT *pnCXKeepServer)
{
	char		key[50], value[VBUFFLEN];
	CString		strBuff;
	extern int	g_iViewMode;

	ASSERT(pnCXKeepServer);
	ASSERT(*pnCXKeepServer >= 0);

	if (!ForwardToKey(f, LOCATORSTRING))
	{
		AfxMessageBox(ID_ERR_NOT_CHATLOC);
		return FALSE;
	}

	int iConn = GetDefaultProto()->GetConnectionStatus();
	BOOL onLine = (iConn == CX_INCHANNEL || iConn == CX_NOCHANNEL);

	g_iViewMode = VM_UNSPECIFIED;

	while(f.ReadString(strBuff))
	{
		if (sscanf(strBuff, "%49s", key) < 1)
			break;		// probably a null line
		if (!stricmp(key, "IRCSERVER:"))
		{
			if (GetValue(strBuff, value, VBUFFLEN))
			{
				if (stricmp (value, "localhost"))
				{
					CString strService;
					theApp.m_listChatServices.GetServiceNameFromDisplayName (value, strService);
					if (stricmp(strService, GetMyServer ()) != 0 || !onLine)
					{
						theApp.m_strConnectedService = strService;
						*pnCXKeepServer = 0;
					}
					else
						*pnCXKeepServer = 1;
				}
				else
					*pnCXKeepServer = 1;
			}
		}
		else
			if (!stricmp(key, "IRCCHANNEL:"))
			{
				if (GetValue(strBuff, value, VBUFFLEN))
				{
					VERIFY(bInitEnterInfo(g_enterInfo, value, NULL, NULL, 0L, TRUE));
					theApp.m_myChannel = value;
				}
			}
			else
				if (!stricmp(key, "CXPROMPT:")) 
				{
					if (GetValue(strBuff, value, VBUFFLEN));
						ChatSetCXPrompt(atoi(value));
				}
				else if (!stricmp(key, "CHARACTER:")) 
				{
					if (GetValue(strBuff, value, VBUFFLEN))
						theApp.m_myCharacterName = value;
				}
				else if (!stricmp(key, "BACKDROP:")) 
				{
					if (GetValue(strBuff, value, VBUFFLEN))
						theApp.m_lastBackDrop = strBuff;
				}
				else if (!stricmp(key, "COMICSDATA:")) 
				{
					if (GetValue(strBuff, value, VBUFFLEN))
					{
						void SetSendComicsData(BOOL);
						SetSendComicsData(atoi(value));
					}
				}
				else if (!stricmp(key, "TITLE:")) 
				{
					if (GetValue(strBuff, value, VBUFFLEN)) 
					{
						if (*value)
							GetChatDoc()->SetComicsTitle2(value);
					}
				}
				else if (!stricmp(key, "ARTDIR:")) 
				{
					if (GetValue(strBuff, value, VBUFFLEN)) 
					{
						if (*value)
							SetArtDir(value);
					}
				}
				else if (!stricmp(key, "VIEW:")) 
				{
					if (GetValue(strBuff, value, VBUFFLEN)) 
					{
						if (!stricmp(value, "Comics"))
							g_iViewMode = VM_COMICS;
						else
							if (!stricmp(value, "Text"))
								g_iViewMode = VM_TEXT;
					}
				}
	}

	if (bJoin && *pnCXKeepServer > 0)
	{
		g_bEnterOnCreate = FALSE;
		bSwitchToRoom(NULL);		// will move window to foreground if it's already there :-)
		if (LookupDoc(g_enterInfo.m_strChannel)) {
			if (bDoException)
				AfxThrowFileException(EX_DONTREPORT);
			else
				return FALSE;
		}
		(*pnCXKeepServer)++;			// need to set again since bSwitchToRoom (InitializeChannelConnection) will effectively unset it
	}
	
	return TRUE;
}

const char *GetMyName() {
	return ((LPCTSTR) theApp.m_myName);
}

const char *GetMyScreenName() {
	const char *DecodeNickForScreen(const char *);
	return DecodeNickForScreen (theApp.m_myName);
}

const char *GetMyNickName() {
	return ((LPCTSTR) theApp.m_myNick);
}

CChatService* AddToServerList(
LPCSTR pszService)
{
	CChatServiceList& SvcList = theApp.m_listChatServices;
	CChatService svcTemp (pszService);
	LPCSTR pszGroup = svcTemp.GetGroup ();
	LPCSTR pszServer = svcTemp.GetServer ();

	CChatService* pSvc = SvcList.FindService (pszGroup, pszServer);
	if (pSvc != NULL)
	{
		// Already in list, just move it to the front.
		SvcList.MoveServiceToTop (pSvc);
		SvcList.WriteIfChanged ();
		return pSvc;
	}

	// If there is any entry in the list that has the same server, delete that entry.
	if (pszServer != NULL)
	{
		pSvc = SvcList.FindService ((LPCSTR)-1L, pszServer);
		if (pSvc != NULL)
		{
			SvcList.DestroyService (pSvc);
		}
	}

	TRY
	{
		pSvc = SvcList.CreateService (pszGroup, pszServer);
	}
	CATCH_ALL(e)
	{
		return NULL;
	}
	END_CATCH_ALL
	SvcList.WriteIfChanged ();
	return pSvc;
}

const char *GetMyIdent() {
	return ((LPCTSTR) theApp.m_myIdent);
}

void SetMyIdent(const char *szIdent) {
	theApp.m_myIdent = szIdent;
}

void SetMyName(const char *szCharName)
{
	theApp.m_myName = szCharName;
	theApp.m_myNick = szCharName;
}


void SetLimitedString(CString &strDest, const char *szSource, UINT uMaxBytes)
{
	strDest = szSource;

	if (strDest.GetLength() > uMaxBytes)
	{
		if (IsDBCSLeadByte(szSource[uMaxBytes-1]))
		{
			LPTSTR	szPrev = CharPrev(szSource, szSource+uMaxBytes-1);
			if (szSource+uMaxBytes-1 == CharNext(szPrev))
				strDest = strDest.Left(uMaxBytes-1);
			else
				strDest = strDest.Left(uMaxBytes);
		}
		else
			strDest = strDest.Left(uMaxBytes);
	}
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
	return theApp.m_strConnectedService;
}

void GetMyServerPrettyName(CString& str) {
	CChatService svcTemp (theApp.m_strConnectedService);
	if (svcTemp.GetGroup ())
		str.Format ("%s (%s)", (LPCSTR)theApp.m_strConnectedServer, svcTemp.GetGroup ());
	else
		str.Format ("%s", (LPCSTR)theApp.m_strConnectedServer);
}

void GetMyServerDisplayName(CString& str) {
	CChatService svcTemp (theApp.m_strConnectedService);
	str	= svcTemp.GetDisplayName ();
}

const char *GetMyPhysicalServer() {
	return theApp.m_strConnectedServer;
}

const char *GetMyRealName() {
	if (theApp.m_myRealName.IsEmpty())
		theApp.m_myRealName.LoadString(IDS_DEFAULT_REALNAME);
	return ((LPCTSTR) theApp.m_myRealName);
}

void SetMyRealName(const char *szRealName)
{
	theApp.m_myRealName = szRealName;
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

void SetMyHomePage(const char *szHomePage)
{
	theApp.m_strHomePage = szHomePage;
}

void SetMyEmail(const char *szEmail)
{
	theApp.m_strEmail = szEmail;
}

void ChatSetChannel(const char *channelName) {
	theApp.m_myChannel = channelName;
}

// changes default -- does not attach
void ChatSetServer(const char *serverName) {
	CString str;
	theApp.m_listChatServices.GetServiceNameFromDisplayName (serverName, str);
	theApp.m_strConnectedService = str;
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

		_findclose (hFindRoom);

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

	// load server list.
	m_serverCtl.SetServiceList (&theApp.m_listChatServices);
	m_serverCtl.Fill (TRUE);

	if(m_radioConnect == 0)
		OnConchan();
	else
		OnNotConChan();

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

	UpdateOk();
	
	return CCSPropertyPage::OnSetActive();
}


// Update the ok button based on a non-empty channelname
void CSetupPage::UpdateOk()
{
	CString strTemp, strTemp2;
	BOOL bGoToChannel = IsDlgButtonChecked (IDC_CONCHAN);
	if (bGoToChannel)
		m_editChannel.GetWindowText(strTemp);
	m_serverCtl.GetWindowText(strTemp2);
	EnableParentOK((!bGoToChannel || !strTemp.IsEmpty()) && !strTemp2.IsEmpty() && strTemp2[0] != '.' && strTemp2[0] != '/');
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
	// default invalid characters
	SetFilter (FILTEREDIT_NOSPC | FILTEREDIT_NOCHARS, ",");
	m_bPasting	 = FALSE;
	m_nCurrLeadByte = 0;
}

void
CFilterEdit::SetFilter(
DWORD dwFilterType,
LPCSTR pszFilterString /* = NULL */)
{
	m_dwFilterType = dwFilterType;
	if ((dwFilterType & FILTEREDIT_NOCHARS) != 0)
	{
		ASSERT(pszFilterString);
		m_strInvalid = pszFilterString;
	}
}

// This will check the current character against a list of invalid characters
// to determine if we should let it into the nickname. nChar can be a two-byte
// DBCS character - in this case the second byte should be the lead character.
BOOL CFilterEdit::CheckIfInvalid(UINT nChar)
{
	if (nChar < 256 && (m_dwFilterType & FILTEREDIT_NOCHARS) != 0 && m_strInvalid.Find((TCHAR) nChar) >= 0)
		return TRUE;

	if ((m_dwFilterType & (FILTEREDIT_NOSPC | FILTEREDIT_PRINTABLEONLY)) != 0)
	{
		WORD wType[4];
		char sz[3];
		sz[0] = nChar < 256 ? (char)nChar : (char)((nChar >> 8) & 255);
		sz[1] = nChar < 256 ? 0 : (char)(nChar & 255);
		sz[2] = '\0';
		VERIFY (GetStringTypeEx (LOCALE_USER_DEFAULT, CT_CTYPE1, sz, -1, wType));
		if ((m_dwFilterType & FILTEREDIT_NOSPC) != 0 && (wType[0] & C1_SPACE) != 0)
			return TRUE;
		if ((m_dwFilterType & FILTEREDIT_PRINTABLEONLY) != 0 && (wType[0] & C1_CNTRL) != 0)
			return TRUE;
	}
	return FALSE;
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
	// At start of typing a character, save the position and string.
	if (m_nCurrLeadByte == 0)
	{
		GetWindowText (m_strBeforeChange);
		m_dwSelBeforeChange = GetSel ();
	}
	
	int nCurrLeadByte = m_nCurrLeadByte;
	if (m_nCurrLeadByte == 0 && IsDBCSLeadByte (nChar))
		m_nCurrLeadByte = nChar;
	else
		m_nCurrLeadByte = 0;

	BOOL bInvalid = FALSE;
	if (m_nCurrLeadByte == 0)
	{
		int nCharToCheck = nChar;
		if (nCurrLeadByte != 0)
		{
			nCharToCheck = nCurrLeadByte << 8 | nChar;
		}
		bInvalid = CheckIfInvalid (nCharToCheck);
	}

	// If the character is invalid, we can proceed in one of two ways. If this
	// is not a DBCS character, we can prevent the OnChar from getting through.
	// If this is a DBCS character, we must pass it through, and then reset the
	// text.

	if (bInvalid)
	{
		if (nCurrLeadByte != 0)
		{
			CEdit::OnChar (nChar, nRepCnt, nFlags);
		   	SetWindowText (m_strBeforeChange);
		   	SetSel (m_dwSelBeforeChange);
		}
		MessageBeep(0xFFFFFFFF);
	}
	else
		CEdit::OnChar (nChar, nRepCnt, nFlags);
}


BOOL CFilterEdit::OnChange()
{
	// OutputDebugString("OnChange\n");

	if (m_bPasting)
	{
		// OutputDebugString("OnChange while pasting\n");
		m_bPasting = FALSE;

		CString strTemp;
		GetWindowText (strTemp);
		LPCSTR psz;
		UINT nCharToCheck;
		for (psz = strTemp; *psz != '\0'; psz++)
		{
			if (IsDBCSLeadByte (*psz))
			{
				nCharToCheck = ((UINT)psz[0]) >> 8 | ((UINT)psz[1]);
				psz++;
			}
			else
				nCharToCheck = (UINT)psz[0];
			if (CheckIfInvalid (nCharToCheck))
				break;
		}

		if (*psz != '\0')
		{
			MessageBeep(0xFFFFFFFF);
			SetWindowText (m_strBeforeChange);
			SetSel (m_dwSelBeforeChange);
		}

		m_strBeforeChange.Empty ();
	}

	return FALSE;
}


LRESULT CFilterEdit::OnPaste(WPARAM wParam, LPARAM lParam)
{
	// OutputDebugString("OnPaste\n");
	m_bPasting = TRUE;	
	GetWindowText (m_strBeforeChange);
	m_dwSelBeforeChange = GetSel ();

	return DefWindowProc(WM_PASTE, wParam, lParam);
}


/////////////////////////////////////////////////////////////////////////////
// CChanFilterEdit

CChanFilterEdit::CChanFilterEdit()
{
	// Default invalid characters for IRC channel names  (space, comma, carriage return, linefeed, bell)
    SetFilter (FILTEREDIT_NOSPC | FILTEREDIT_NOCHARS, ",\r\n\a");
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

	CChatDoc::ChatLoadLocator(ar, FALSE, FALSE, &g_nCXKeepServer);
	GetMyServerDisplayName (m_strServer);
	m_strService = GetMyServer ();
	m_myChannel = theApp.m_myChannel;
	ChatSetCXPrompt(TRUE);				// we want to make sure to prompt next time through
	m_radioConnect = CA_JOINROOM;

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
	m_bSpacesAllowed = TRUE;
}


void CNicknameDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNicknameDlg)
	DDX_Control(pDX, IDC_NEWNICK, m_editNick);
	DDX_Control(pDX, IDC_STATICNICKNAME, m_staticNick);
	DDX_Text(pDX, IDC_STATICNICKNAME, m_label);
	DDX_Text(pDX, IDC_NEWNICK, m_strNickname);
	DDV_NonBlankNick(pDX, m_strNickname);
	DDV_MaxChars(pDX, m_strNickname, MAX_NICKINPUT);
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
	if (m_bSpacesAllowed)
		m_editNick.SetFilter (FILTEREDIT_NOCHARS, ",");
	
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

	UpdateOk ();
}

void CSetupPage::OnNotConChan() 
{
		GetDlgItem(IDC_CHANNEL)->EnableWindow(FALSE);
		UpdateOk ();
}


void CSetupPage::OnChangeChannel() 
{
	if (m_editChannel.m_hWnd)
		m_editChannel.OnChange ();
	UpdateOk();
	m_ChatRooms.SetCurSel(-1);
}


void CSetupPage::OnChangeServer() 
{
	int nSel = m_serverCtl.GetCurSel ();
	if (nSel != -1)
	{
		CChatService* pSvc = m_serverCtl.GetServiceAt (nSel);
		if (pSvc != NULL)
			pSvc->FormatAsServiceName (m_strService);
		else
			m_strService.Empty ();
		CString str;
		m_serverCtl.GetLBText (nSel, str);
		m_serverCtl.SetWindowText (str);
		UpdateOk();
		m_ChatRooms.SetCurSel(-1);
	}
}

void CSetupPage::OnChangeServerTyped() 
{
	UpdateOk();
	m_strService.Empty ();
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
	m_bIsIRCX = FALSE;
}


void CChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelDlg)
	DDX_Control(pDX, IDC_PASSWORD, m_passwordCtrl);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_NEWCHANNEL, m_channelCtrl);
	DDX_Text(pDX, IDC_NEWCHANNEL, m_strChannel);
	DDV_MaxChars(pDX, m_strChannel, m_bIsIRCX ? MAX_IRCXCHANNAME : MAX_IRCCHANNAME);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDV_MaxChars(pDX, m_strPassword, MAX_CHANNELPWD);
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
	if (m_channelCtrl.m_hWnd)
	{
		m_channelCtrl.OnChange ();
		CString strTemp;
		m_channelCtrl.GetWindowText(strTemp);
		if(strTemp.IsEmpty())
			m_ok.EnableWindow(FALSE);
		else m_ok.EnableWindow(TRUE);
	}
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


#define FOUND_BACKDROPS		1
#define FOUND_AVATARS		2
#define FOUND_ALL_ART		(FOUND_BACKDROPS | FOUND_AVATARS)

BOOL ArtDirsOK() {		// returns TRUE if ArtDirs contain at least one backdrop & character
	// Because backdrops can now be either .bmp or .bgb, we maintain what things we
	// found.
	struct _finddata_t fdRoom;
	UINT nFoundWhat = 0;
	CString pattern;

	pattern = theApp.m_strBackDropDir + "\\*.bmp";
	long hFindRoom = _findfirst( (char *) (const char *) pattern, &fdRoom );
	if( hFindRoom != -1) {
		nFoundWhat |= FOUND_BACKDROPS;
		_findclose (hFindRoom);
	}

	if ((nFoundWhat & FOUND_BACKDROPS) == 0) {
		pattern = theApp.m_strBackDropDir + "\\*.bgb";
		long hFindRoom = _findfirst( (char *) (const char *) pattern, &fdRoom );
		if( hFindRoom != -1) {
			nFoundWhat |= FOUND_BACKDROPS;
			_findclose (hFindRoom);
		}
	
	}

	pattern = theApp.m_strAvatarDir + "\\*.avb";
	hFindRoom = _findfirst( (char *) (const char *) pattern, &fdRoom );
	if( hFindRoom != -1) {
		nFoundWhat |= FOUND_AVATARS;
		_findclose (hFindRoom);
	}

	return nFoundWhat == FOUND_ALL_ART;
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
	DDV_MaxChars(pDX, m_strPassword, MAX_CHANNELPWD);
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


void CSetupPage::OnOK() 
{
	ChatSetServer (m_strService.IsEmpty () ? m_strServer : m_strService);
	theApp.m_myChannel = m_myChannel;
	// right now, CA_JOINROOM, CA_ROOMLIST, & CA_NOACTION are in the order of m_radioConnect
	// this may not always be true, in which case the following assignment needs to be changed
	theApp.m_iOnConnectAction = m_radioConnect;
	
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


