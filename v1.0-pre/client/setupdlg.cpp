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
#include "panel.h"
#include "chatprot.h"
#include "io.h"
#include "ui.h"
#include "userinfo.h"
#include "histent.h"
#include "helpids.h"
#include <winreg.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;
CSetupDialog setupDlg;			// we keep this around for the value of its vars
void AddAndExecute(HistoryEntry *);

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog


CSetupDialog::CSetupDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CSetupDialog::IDD, pParent)
{
	// a later LoadFromReg will likely restore defaults for these values
	//{{AFX_DATA_INIT(CSetupDialog)
	m_myName = _T("");
	m_ircServer = _T("comicsrv1.microsoft.com");
	m_myRealName = _T("Your Full Name");
	m_myChannel = _T("#Comic_Chat");
	m_ircPort = 6667;
	m_strChatRooms = _T("");
	m_bGetChannelList = FALSE;
	m_radioConnect = 0;
	//}}AFX_DATA_INIT
	m_myCharacterName = _T("");

	bExpanded = FALSE;
	nWhatFailed = CHATROOM;
}


void CSetupDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetupDialog)
	DDX_Control(pDX, IDC_CHANNEL, m_editChannel);
	DDX_Control(pDX, IDC_CHATROOMS, m_ChatRooms);
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Text(pDX, IDC_SERVER, m_ircServer);
	DDV_MaxChars(pDX, m_ircServer, 100);
	DDX_Text(pDX, IDC_CHANNEL, m_myChannel);
	DDV_MaxChars(pDX, m_myChannel, 40);
	DDX_Text(pDX, IDC_PORTNUM, m_ircPort);
	DDV_MinMaxUInt(pDX, m_ircPort, 1, 100000);
	DDX_CBString(pDX, IDC_CHATROOMS, m_strChatRooms);
	DDX_Radio(pDX, IDC_CONCHAN, m_radioConnect);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSetupDialog, CDialog)
	//{{AFX_MSG_MAP(CSetupDialog)
	ON_CBN_SELCHANGE(IDC_CHARACTER, OnSelchangeCharacter)
	ON_EN_CHANGE(IDC_NICKNAME, OnChangeNickname)
	ON_BN_CLICKED(IDC_SETUPHELP, OnSetuphelp)
	ON_CBN_SELCHANGE(IDC_CHATROOMS, OnSelchangeChatrooms)
	ON_BN_CLICKED(IDC_GETLIST, OnGetlist)
	ON_BN_CLICKED(IDC_CONCHAN, OnConchan)
	ON_BN_CLICKED(IDC_LISTCHAN, OnListchan)
	ON_EN_CHANGE(IDC_CHANNEL, OnChangeChannel)
	ON_EN_CHANGE(IDC_PORTNUM, OnChangePortnum)
	ON_EN_CHANGE(IDC_SERVER, OnChangeServer)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog message handlers


void CSetupDialog::OnOK() 
{
	CDialog::OnOK();
}

// Finds the text after the colon (on an ini line) and copies that into value.
// Strips off white space on either end
BOOL GetValue(const char *buff, char *value) {
	// assumes value is large enough to hold chars
	char *start = strchr(buff, ':');
	if (!start) return FALSE;
	start++;
	while (*start && isspace(*start)) start++;
	char *end = strchr(buff, '\0');
	while (isspace(*end) || !*end) end--;
	int len = end - start + 1;
	strncpy(value, start, len);
	value[len] = '\0';
	return TRUE;
}

#if 0
BOOL LoadFromIni() {
	return TRUE;
//	return setupDlg.LoadFromIni();			// others need not know about setupdlg
}
#endif

BOOL LoadFromReg() {
	return setupDlg.LoadFromReg();			// others need not know about setupdlg
}

#if 0
BOOL CSetupDialog::LoadFromIni() {
	// for now, just use TEMP -- XXX fix!
	CString iniFile = getenv("TEMP");
	iniFile += "\\chat.ini";

	FILE *fp;
	if ((fp = fopen(iniFile, "r")) == NULL) return FALSE;

	char buff[200], key[50], value[200];
	while(fgets(buff, sizeof(buff), fp)) {
		sscanf(buff, "%s", key);
		if (!stricmp(key, "XFRAME:")) {
			if (GetValue(buff, value))
				theApp.m_xFrame = atoi(value);
		} else if (!strcmp(key, "YFRAME:")) {
			if (GetValue(buff, value))
				theApp.m_yFrame = atoi(value);
		} else if (!strcmp(key, "CXFRAME:")) {
			if (GetValue(buff, value)) 
				theApp.m_cxFrame = atoi(value);
		} else if (!strcmp(key, "CYFRAME:")) {
			if (GetValue(buff, value))
				theApp.m_cyFrame = atoi(value);
		} else if (!strcmp(key, "MAXIMIZED:")) {
			if (GetValue(buff, value))
				theApp.m_maxedFrame = atoi(value);
		} else if (!strcmp(key, "UPNLWIDTH:")) {
			if (GetValue(buff, value))
				CUnitPanelPage::SetUnitPanelWidth(atoi(value));
		} else if (!strcmp(key, "UPNLHEIGHT:")) {
			if (GetValue(buff, value))
				CUnitPanelPage::SetUnitPanelHeight(atoi(value));
		} else if (!strcmp(key, "UNITSWIDE:")) {
			if (GetValue(buff, value))
				CUnitPanelPage::SetUnitPanelsPerRow(atoi(value));
		} else if (!stricmp(key, "IRCSERVER:")) {
			if (GetValue(buff, value))
				m_ircServer = value;
		} else if (!stricmp(key, "IRCCHANNEL:")) {
			if (GetValue(buff, value))
				m_myChannel = value;
		} else if (!stricmp(key, "IRCPORT:")) {
			if (GetValue(buff, value))
				m_ircPort = atoi(value);
		} else if (!stricmp(key, "NAME:")) {
			if (GetValue(buff, value))
				m_myName = value;
		} else if (!stricmp(key, "REALNAME:")) {
			if (GetValue(buff, value))
				m_myRealName = value;
		} else if (!stricmp(key, "CHARACTER:")) {
			if (GetValue(buff, value))
				m_myCharacterName = value;
		} else if (!stricmp(key, "COMICSDATA:")) {
			if (GetValue(buff, value)) {
				void SetSendComicsData(BOOL);
				SetSendComicsData(atoi(value));
			}
		} else {
			TRACE("Got an unknown key: %s.\n", key);
		}		
	}
	fclose(fp);
	return TRUE;
}
#endif

BOOL CSetupDialog::LoadFromReg() {

	DWORD cbData = 0;
	int i = 0;
	char buff[200];

	// open the key
	HKEY	hKey = NULL;
	if (RegOpenKeyEx (HKEY_CURRENT_USER, "Software\\Microsoft\\Microsoft Comic Chat", 
						0, KEY_ALL_ACCESS, &hKey) == ERROR_SUCCESS) {

		// if that succeeded, read values
		cbData = sizeof(theApp.m_xFrame);
		RegQueryValueEx (hKey, "XFrame", 0, NULL, (LPBYTE)&theApp.m_xFrame, 
						&cbData);

		cbData = sizeof(theApp.m_yFrame);
		RegQueryValueEx (hKey, "YFrame", 0, NULL, (LPBYTE)&theApp.m_yFrame, 
						&cbData);

		cbData = sizeof(theApp.m_cxFrame);
		RegQueryValueEx (hKey, "CXFrame", 0, NULL, (LPBYTE)&theApp.m_cxFrame, 
						&cbData);

		cbData = sizeof(theApp.m_cyFrame);
		RegQueryValueEx (hKey, "CYFrame", 0, NULL, (LPBYTE)&theApp.m_cyFrame, 
						&cbData);

		cbData = sizeof(theApp.m_maxedFrame);
		RegQueryValueEx (hKey, "Maximized", 0, NULL, (LPBYTE)&theApp.m_maxedFrame, 
						&cbData);

		cbData = sizeof(i);
		RegQueryValueEx (hKey, "UPNLWidth", 0, NULL, (LPBYTE)&i, &cbData);
		CUnitPanelPage::SetUnitPanelWidth(i);

		cbData = sizeof(i);
		RegQueryValueEx (hKey, "UPNLHeight", 0, NULL, (LPBYTE)&i, &cbData);
		CUnitPanelPage::SetUnitPanelHeight(i);

		cbData = sizeof(i);
		RegQueryValueEx (hKey, "UnitsWide", 0, NULL, (LPBYTE)&i, &cbData);
		CUnitPanelPage::SetUnitPanelsPerRow(i);

		cbData = sizeof(buff);
		if (RegQueryValueEx (hKey, "LastFavorite", 0, NULL, (unsigned char *) buff, &cbData) == ERROR_SUCCESS)
			m_strChatRooms = buff;
		else m_strChatRooms = "";

		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "IRCServer", 0, NULL, (unsigned char *)buff, &cbData);
		m_ircServer = buff;

		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "IRCChannel", 0, NULL, (unsigned char *)buff, &cbData);
		m_myChannel = buff;

		cbData = sizeof(m_ircPort);
		RegQueryValueEx (hKey, "IRCPort", 0, NULL, (LPBYTE)&m_ircPort, &cbData);

		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "Name", 0, NULL, (unsigned char *)buff, &cbData);
		m_myName = buff;

		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "RealName", 0, NULL, (unsigned char *)buff, &cbData);
		m_myRealName = buff;

		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "Character", 0, NULL, (unsigned char *)buff, &cbData);
		m_myCharacterName = buff;

		cbData = sizeof(buff);
		RegQueryValueEx (hKey, "Backdrop", 0, NULL, (unsigned char *)buff, &cbData);
		if (buff[0]) theApp.m_lastBackDrop = buff;

		cbData = sizeof(theApp.m_bComicView);
		RegQueryValueEx (hKey, "ShowComicView", 0, NULL, (LPBYTE)&theApp.m_bComicView, 
						&cbData);

		cbData = sizeof(i);
		RegQueryValueEx (hKey, "ComicsData", 0, NULL, (LPBYTE)&i, &cbData);
		void SetSendComicsData(BOOL);
		SetSendComicsData(i);
	}
	RegCloseKey (hKey);
	return TRUE;
}

BOOL SaveToIni() {
	return TRUE;
	//	return setupDlg.SaveToIni();		// others need not know about setupDlg
}

BOOL SaveToReg() {
	return setupDlg.SaveToReg();
}

BOOL CSetupDialog::SaveToIni() {
	//  for now, just use TEMP -- XXX fix!
	CString iniFile = getenv("TEMP");
	iniFile += "\\chat.ini";

	FILE *fp;
	if ((fp = fopen(iniFile, "w")) == NULL) return FALSE;

	CWnd *frame = GetFrame();
	if (frame) {
		WINDOWPLACEMENT wp;
		frame->GetWindowPlacement(&wp);
		theApp.m_xFrame = wp.rcNormalPosition.left;
		theApp.m_yFrame = wp.rcNormalPosition.top;
		theApp.m_cxFrame = wp.rcNormalPosition.right - wp.rcNormalPosition.left + 1;
		theApp.m_cyFrame = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top + 1;
		theApp.m_maxedFrame = (wp.showCmd == SW_SHOWMAXIMIZED);
	}

	fprintf(fp, "XFRAME:\t%d\nYFRAME:\t%d\nCXFRAME:\t%d\nCYFRAME:\t%d\nMAXIMIZED:\t%d\n",
		    theApp.m_xFrame, theApp.m_yFrame, theApp.m_cxFrame, theApp.m_cyFrame,
			theApp.m_maxedFrame);
	fprintf(fp, "UPNLWIDTH:\t%d\nUPNLHEIGHT:\t%d\nUNITSWIDE:\t%d\n",
		    CUnitPanelPage::GetUnitPanelWidth(), CUnitPanelPage::GetUnitPanelHeight(),
		    CUnitPanelPage::GetUnitPanelsPerRow());
	fprintf(fp, "IRCSERVER:\t%s\n", m_ircServer);
	fprintf(fp, "IRCPORT:\t%d\n", m_ircPort);
	fprintf(fp, "IRCCHANNEL:\t%s\n", m_myChannel);
	fprintf(fp, "NAME:\t%s\n", m_myName);
	fprintf(fp, "REALNAME:\t%s\n", m_myRealName);
	fprintf(fp, "CHARACTER:\t%s\n", m_myCharacterName);
	BOOL GetSendComicsData();
	fprintf(fp, "COMICSDATA:\t%d\n", GetSendComicsData());

	fclose(fp);
	return TRUE;
}

BOOL CSetupDialog::SaveToReg() {

	// get current data
	CWnd *frame = GetFrame();
	if (frame) {
		WINDOWPLACEMENT wp;
		frame->GetWindowPlacement(&wp);
		theApp.m_xFrame = wp.rcNormalPosition.left;
		theApp.m_yFrame = wp.rcNormalPosition.top;
		theApp.m_cxFrame = wp.rcNormalPosition.right - wp.rcNormalPosition.left + 1;
		theApp.m_cyFrame = wp.rcNormalPosition.bottom - wp.rcNormalPosition.top + 1;
		theApp.m_maxedFrame = (wp.showCmd == SW_SHOWMAXIMIZED);
	}

	// open the application's key
	HKEY	hKey = NULL;
	if (RegCreateKeyEx (HKEY_CURRENT_USER, "Software\\Microsoft\\Microsoft Comic Chat", 
						0, "Application Global Data", REG_OPTION_NON_VOLATILE,
						KEY_ALL_ACCESS,	NULL, &hKey, NULL) == ERROR_SUCCESS) {

		// write the values
		RegSetValueEx (hKey, "XFrame", 0, REG_DWORD, (LPBYTE)&theApp.m_xFrame, 
						sizeof(theApp.m_xFrame));

		RegSetValueEx (hKey, "YFrame", 0, REG_DWORD, (LPBYTE)&theApp.m_yFrame, 
						sizeof(theApp.m_yFrame));

		RegSetValueEx (hKey, "CXFrame", 0, REG_DWORD, (LPBYTE)&theApp.m_cxFrame, 
						sizeof(theApp.m_cxFrame));

		RegSetValueEx (hKey, "CYFrame", 0, REG_DWORD, (LPBYTE)&theApp.m_cyFrame, 
						sizeof(theApp.m_cyFrame));

		RegSetValueEx (hKey, "Maximized", 0, REG_DWORD, (LPBYTE)&theApp.m_maxedFrame, 
						sizeof(theApp.m_maxedFrame));
		int i = 0;
		i = CUnitPanelPage::GetUnitPanelWidth();
		RegSetValueEx (hKey, "UPNLWidth", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		i = CUnitPanelPage::GetUnitPanelHeight();
		RegSetValueEx (hKey, "UPNLHeight", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		i = CUnitPanelPage::GetUnitPanelsPerRow();
		RegSetValueEx (hKey, "UnitsWide", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));

		RegSetValueEx (hKey, "LastFavorite", 0, REG_SZ,
						(const unsigned char *)(LPCTSTR)m_strChatRooms,
						strlen(m_strChatRooms)+1);

		RegSetValueEx (hKey, "IRCServer", 0, REG_SZ, 
						(const unsigned char *)(LPCTSTR)m_ircServer, 
						strlen(m_ircServer)+1);

		RegSetValueEx (hKey, "IRCPort", 0, REG_DWORD, (LPBYTE)&m_ircPort, 
						sizeof(int));

		RegSetValueEx (hKey, "IRCChannel", 0, REG_SZ, 
						(const unsigned char *)(LPCTSTR)m_myChannel, 
						strlen(m_myChannel)+1);

		RegSetValueEx (hKey, "Name", 0, REG_SZ, 
						(const unsigned char *)(LPCTSTR)m_myName, 
						strlen(m_myName)+1);

		RegSetValueEx (hKey, "RealName", 0, REG_SZ, 
						(const unsigned char *)(LPCTSTR)m_myRealName, 
						strlen(m_myRealName)+1);

		RegSetValueEx (hKey, "Character", 0, REG_SZ, 
						(const unsigned char *)(LPCTSTR)m_myCharacterName, 
						strlen(m_myCharacterName)+1);

		RegSetValueEx (hKey, "Backdrop", 0, REG_SZ, 
						(const unsigned char *)(LPCTSTR)theApp.m_lastBackDrop, 
						strlen(theApp.m_lastBackDrop)+1);

		RegSetValueEx (hKey, "ShowComicView", 0, REG_DWORD, (LPBYTE)&theApp.m_bComicView, 
						sizeof(theApp.m_bComicView));

		BOOL GetSendComicsData();
		i = GetSendComicsData();
		RegSetValueEx (hKey, "ComicsData", 0, REG_DWORD, (LPBYTE)&i, sizeof(int));
	}
	RegCloseKey (hKey);
	return TRUE;
}

BOOL ChatSaveLocator(CArchive &f) {
	char ln[200];

	sprintf(ln, "#CHATLOCATOR\r\n");
	f.WriteString(ln);
	sprintf(ln, "IRCSERVER:\t%s\r\n", setupDlg.m_ircServer);
	f.WriteString(ln);
	sprintf(ln, "IRCPORT:\t%d\r\n", setupDlg.m_ircPort);
	f.WriteString(ln);
	sprintf(ln, "IRCCHANNEL:\t%s\r\n", setupDlg.m_myChannel);
	f.WriteString(ln);
	f.WriteString("CXPROMPT:\t0\r\n");		// for now, locators don't prompt

	return TRUE;
}

// doesn't work in the general case, but works with #ALPHA strings which is fine for now
BOOL ForwardToKey(CArchive &f, const char *key) {
	CString buff;
	int len = strlen(key);
	char c;
	const char *kptr = key;

	while (TRUE) {
		if (!*kptr) {  // we've matched totally -- advance to next line
			f.ReadString(buff);
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

BOOL ChatLoadLocator(CArchive &f) {
	char key[50], value[200];
	CString buff;

	if (!ForwardToKey(f, LOCATORSTRING)) {
		AfxMessageBox(ID_ERR_NOT_CHATLOC);
		return FALSE;
	}

	while(f.ReadString(buff)) {
		if (sscanf(buff, "%s", key) < 1) break;		// probably a null line
		if (!stricmp(key, "IRCSERVER:")) {
			if (GetValue(buff, value))
				setupDlg.m_ircServer = value;
		} else if (!stricmp(key, "IRCCHANNEL:")) {
			if (GetValue(buff, value))
				setupDlg.m_myChannel = value;
		} else if (!stricmp(key, "IRCPORT:")) {
			if (GetValue(buff, value))
				setupDlg.m_ircPort = atoi(value);
		} else if (!stricmp(key, "CXPROMPT:")) {
			if (GetValue(buff, value));
			void ChatSetCXPrompt(BOOL);
			ChatSetCXPrompt(atoi(value));
		} else if (!stricmp(key, "CHARACTER:")) {
			if (GetValue(buff, value))
				setupDlg.m_myCharacterName = value;
		} else if (!stricmp(key, "COMICSDATA:")) {
			if (GetValue(buff, value)) {
				void SetSendComicsData(BOOL);
				SetSendComicsData(atoi(value));
			}
		} else {
//			TRACE("Got an unknown key: %s.\n", key);  // this can happen now, due to extra docfile chars at end of file
		}		
	}
	return TRUE;
}

const char *GetMyName() {
	return ((LPCTSTR) setupDlg.m_myName);
}

void SetMyName(const char *charName) {
	setupDlg.m_myName = charName;
}

const char *GetMyServer() {
	return ((LPCTSTR) setupDlg.m_ircServer);
}

UINT GetMyPort() {
	return (setupDlg.m_ircPort);
}

const char *GetMyRealName() {
	return ((LPCTSTR) setupDlg.m_myRealName);
}

void SetMyRealName(const char *charName) {
	setupDlg.m_myRealName = charName;
}

const char *GetMyCharacter() {
	return ((LPCTSTR) setupDlg.m_myCharacterName);
}

const char *GetMyChannel() {
	return ((LPCTSTR) setupDlg.m_myChannel);
}

void SetMyCharacter(const char *charName) {
	setupDlg.m_myCharacterName = charName;
}

void ChatSetChannel(const char *channelName) {
	setupDlg.m_myChannel = channelName;
}

// changes default -- does not attach
void ChatSetServer(const char *serverName) {
	setupDlg.m_ircServer = serverName;
}

void ChatSetPort(UINT portNum) {
	setupDlg.m_ircPort = portNum;
}

BOOL RequestedChannelList(BOOL newVal) {
//	BOOL oldVal = setupDlg.m_bGetChannelList;
	BOOL oldVal = (BOOL)setupDlg.m_radioConnect;
//	setupDlg.m_bGetChannelList = newVal;
	setupDlg.m_radioConnect = newVal;
	return oldVal;
}


#define HEADX	266
#define HEADY	72
#define HEADW	55
#define HEADH	55

void CSetupDialog::DrawAvatarFeedback() {
#if 0
	CPaintDC dc(this);		// device context for painting
	char avName[20];
	CListBox *charList = charCtl();

	return;   /// XXX - fix rest of routine
	int curSel = charList->GetCurSel();
	charList->GetText(curSel, avName);
	CAvatarX *av = GetAvatar(avName);
	if (!av) return;
	CPose *pose = GetPoseFromID(av->m_icon);
	pose->m_drawing->Draw(&dc, HEADX, HEADY, HEADW, HEADH, SRCCOPY);
#endif
}


void CSetupDialog::OnSelchangeCharacter() 
{
	int curSel = setupDlg.m_myCharacter.GetCurSel();
	setupDlg.m_myCharacter.GetLBText(curSel, m_myCharacterName);
//	DrawAvatarFeedback();	// update the avatar thumbnail	
}

void CSetupDialog::OnCancel() 
{
	BOOL SetMyAvatar(const char *);
	void CIUserJoin(const char *);

/*	if(m_myName.IsEmpty())
		m_myName = "Anonymous";

	if (theApp.m_bComicView)
		SetMyAvatar(m_myCharacterName);	// Cancel means netless. Need to set up character anyway.*/

	AfxMessageBox(IDS_DISCONNECTMESSAGE);
	CDialog::OnCancel();
}

BOOL CSetupDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	// Load avatar possibilities (similar to avatardl.cpp)
	    // build file search strings
    CString strPattern;

//    m_strAvatarFiles.RemoveAll();
/*	strPattern = theApp.GetAvatarDir();
	strPattern += "\\*.avb";
	struct _finddata_t fd;
	long hFind = _findfirst( (char *) (const char *) strPattern, &fd );
	if( hFind != -1 )
	{
		do 
		{
			if (fd.attrib != _A_SUBDIR)
			{
				char szExt[_MAX_EXT];
				char szFName[_MAX_FNAME];

				_splitpath( fd.name, NULL, NULL, szFName, szExt );
				CharUpperBuff(szFName, 1);
				m_myCharacter.AddString(szFName);
		   }

		} while( _findnext( hFind, &fd ) != -1 );
	}*/

	if (m_myCharacterName == "") {
		char randName[50];
		void GetNextAvatarName(char *);
		GetNextAvatarName(randName);
		m_myCharacterName = randName;
	}

	if(m_myChannel.GetLength() > 40)
		m_myChannel = "";

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
				char szExt[_MAX_EXT];
				char szFName[_MAX_FNAME];

				_splitpath( fdRoom.name, NULL, NULL, szFName, szExt );
				CharUpperBuff(szFName, 1);
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
	
	// set up Character List Box appropriately
//	int curSel = m_myCharacter.FindStringExact(0, m_myCharacterName);
//	if (curSel == LB_ERR) {			// in case the string is unknown
//		curSel = 1;	
//		m_myCharacter.GetLBText(curSel, m_myCharacterName);  // store it away
//	}
//	m_myCharacter.SetCurSel(curSel);
//	OnChangeNickname();

	// removed expand button (ericg 7/3/97)
	// Size the dialog correctly:
/*	GetWindowRect(&m_rectOriginal);  // save the original size
	RECT rect = m_rectOriginal;
	RECT edgerect;
	GetDlgItem(IDC_EDGE)->GetWindowRect(&edgerect);
	rect.bottom = edgerect.bottom;
	MoveWindow(&rect);
	m_ExpandButton.SetWindowText("E&xpand");
	bExpanded = FALSE;*/

	m_ChatRooms.SetCurSel(-1);

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
	case PORT:
		GetDlgItem(IDC_PORTNUM)->SetFocus();
		((CEdit*)GetDlgItem(IDC_PORTNUM))->SetSel(0,-1);
		break;
	case CHANNEL:
		GetDlgItem(IDC_CHANNEL)->SetFocus();
		((CEdit*)GetDlgItem(IDC_CHANNEL))->SetSel(0,-1);
		break;
	}


	nWhatFailed = CHATROOM;
	
	return FALSE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

// Update the ok button based on a valid nickname
void CSetupDialog::UpdateOk(CString &nickname)
{
	if(nickname.IsEmpty())
		m_ok.EnableWindow(FALSE);
	else
		m_ok.EnableWindow(TRUE);
}

// If we are notified of a change in the nickname, we may have to update the ok button
void CSetupDialog::OnChangeNickname() 
{
	CString strTemp;
	m_editNickname.GetWindowText(strTemp);
	UpdateOk(strTemp);
}

	
/////////////////////////////////////////////////////////////////////////////
// CNicknameEdit

CNicknameEdit::CNicknameEdit()
{
	m_strInvalid = " ";  // our big list of invalid characters for nickname
}

CNicknameEdit::~CNicknameEdit()
{
}


BEGIN_MESSAGE_MAP(CNicknameEdit, CEdit)
	//{{AFX_MSG_MAP(CNicknameEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CNicknameEdit message handlers

// Validate character and then pass on or beep if invalid
void CNicknameEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if(CheckIfInvalid(nChar))
	{
		MessageBeep(0xFFFFFFFF);
		return;
	}
	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

// This will check the current character against a list of invalid characters
// to determine if we should let it into the nickname
BOOL CNicknameEdit::CheckIfInvalid(UINT nChar)
{
	int nlength = m_strInvalid.GetLength();
	CString strChar(nChar);

	for(int index=0;index<nlength;index++)
	{
		if(nChar == m_strInvalid.Mid(index,1))
			return TRUE;
	}
	return FALSE;
}


// removed expand button (ericg 7/3/97)
// When expanding the dialog for advanced users:
/*void CSetupDialog::OnExpand() 
{
	if(!bExpanded)
	{
		// Disable the expand button
		m_ExpandButton.SetWindowText("C&ollapse");
		// Enable the other controls
		GetDlgItem(IDC_SERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_PORTNUM)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHANNEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_REALNAME)->EnableWindow(TRUE);

		// make sure focus is set
		GetDlgItem(IDC_SERVER)->SetFocus();
		// and resize
		SetWindowPos(&wndTop,0,0,m_rectOriginal.right-m_rectOriginal.left,
			m_rectOriginal.bottom-m_rectOriginal.top,SWP_NOMOVE);
		bExpanded = TRUE;
	}
	else
	{
		m_ExpandButton.SetWindowText("E&xpand");
		// Enable the other controls
		GetDlgItem(IDC_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_PORTNUM)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHANNEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_REALNAME)->EnableWindow(FALSE);
		// make sure focus is set
		GetDlgItem(IDC_NICKNAME)->SetFocus();
		// and resize
		GetWindowRect(&m_rectOriginal);  // save the original size
		RECT rect = m_rectOriginal;
		RECT edgerect;
		GetDlgItem(IDC_EDGE)->GetWindowRect(&edgerect);
		rect.bottom = edgerect.bottom;
		SetWindowPos(&wndTop,0,0,rect.right-rect.left,
			rect.bottom-rect.top,SWP_NOMOVE);
		bExpanded = FALSE;
	}
}*/

void CSetupDialog::OnSetuphelp() 
{
	AfxGetApp()->WinHelp(IDR_CONNECTING);
}

void CSetupDialog::OnSelchangeChatrooms() 
{
	// need to load the relevant file...
	int curSel = setupDlg.m_ChatRooms.GetCurSel();
	if(curSel == -1)
		return;
	setupDlg.m_ChatRooms.GetLBText(curSel, m_strChatRooms);
	CString filename;
	filename.Format("%s\\%s.ccr", theApp.m_strFavoritesDir, m_strChatRooms);

	// now create a CFile...
	extern char* pFileName;
	CFile f;
	if(!f.Open(filename, CFile::modeRead)) {
		TRACE("Unable to open %s", filename);		// no signal to user for now
		return;
	}

	// now create a CArchive from the CFile...
	CArchive ar(&f, CArchive::load);

	ChatLoadLocator(ar);
	UpdateData(FALSE);		// update self
}


void CSetupDialog::OnGetlist() 
{

	if(!m_bGetChannelList)
	{
		GetDlgItem(IDC_STATICCHANNEL)->EnableWindow(FALSE);
		GetDlgItem(IDC_CHANNEL)->EnableWindow(FALSE);
	}
	else
	{
		GetDlgItem(IDC_STATICCHANNEL)->EnableWindow(TRUE);
		GetDlgItem(IDC_CHANNEL)->EnableWindow(TRUE);
	}
	m_bGetChannelList = !m_bGetChannelList;
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
	DDV_MaxChars(pDX, m_strNickname, 9);
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
	
	m_editNick.SetSel(0, -1);				// select contents for replacement
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSetupDialog::OnConchan() 
{
//	if(m_radioConnect == 0)
//	{
		CEdit* pEdit = (CEdit*)GetDlgItem(IDC_CHANNEL);
		pEdit->EnableWindow(TRUE);
		pEdit->SetFocus();
		pEdit->SetSel(0,-1);
		if(pEdit->LineLength() == 0)
			m_ok.EnableWindow(FALSE);
/*	}
	else
	{
		GetDlgItem(IDC_CHANNEL)->EnableWindow(FALSE);
	}*/
}

void CSetupDialog::OnListchan() 
{
/*	if(m_radioConnect == 1)
	{*/
		GetDlgItem(IDC_CHANNEL)->EnableWindow(FALSE);
		m_ok.EnableWindow(TRUE);
/*	}
	else
	{
		GetDlgItem(IDC_CHANNEL)->EnableWindow(TRUE);
	}*/
}

void CSetupDialog::OnChangeChannel() 
{
	CString strTemp;
	m_editChannel.GetWindowText(strTemp);
	UpdateOk(strTemp);
	m_ChatRooms.SetCurSel(-1);
}

void CSetupDialog::OnChangePortnum() 
{
	m_ChatRooms.SetCurSel(-1);
}

void CSetupDialog::OnChangeServer() 
{
	m_ChatRooms.SetCurSel(-1);
}
/////////////////////////////////////////////////////////////////////////////
// CChannelDlg dialog


CChannelDlg::CChannelDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CChannelDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CChannelDlg)
	m_strChannel = _T("");
	//}}AFX_DATA_INIT
}


void CChannelDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelDlg)
	DDX_Control(pDX, IDOK, m_ok);
	DDX_Control(pDX, IDC_NEWCHANNEL, m_channelCtrl);
	DDX_Text(pDX, IDC_NEWCHANNEL, m_strChannel);
	DDV_MaxChars(pDX, m_strChannel, 40);
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
	
	OnChangeChannel();  // enable OK only if characters filled in
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
