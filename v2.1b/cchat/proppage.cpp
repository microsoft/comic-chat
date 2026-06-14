// PropPage.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "setupdlg.h"
#include "ui.h"

#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "bodycam.h"
#include "userinfo.h"
#include "chatprot.h"
#include "memblst.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "histent.h"
#include "ui.h"
#include "traj.h"
#include "spline.h"
#include "backdrop.h"
#include "balloon.h"
#include "pageview.h"
#include "panel.h"
#include "textcore.h"
#include "textview.h"
#include "saywnd.h"
#include "ccommon.h"

#include <io.h>
#include <afxcmn.h>

#include "PropPage.h"
#include <winreg.h>

#include "dlylddll.h"
#include "ratings.h"
#include "txtfntdg.h"
#include "resource.h"
#include "mschat.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" void *GetMime();
extern "C" int	iBytesofChar(BYTE ch);

extern CChatApp theApp;
extern CPtrList g_docs;

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage property page

// IMPLEMENT_DYNCREATE(CSettingsPage, CCSPropertyPage)

CSettingsPage::CSettingsPage() : CCSPropertyPage(CSettingsPage::IDD)
{
	BOOL GetSendComicsData();
	//{{AFX_DATA_INIT(CSettingsPage)
	m_bSave = theApp.m_bPrompt;
	m_bComicsData = !GetSendComicsData();
	m_bAcceptWhispers = theApp.m_bAcceptWhispers;
	m_bShowArrivals = theApp.m_bShowArrivals;
	m_strSoundPath = theApp.m_soundPath;
	m_bAllowInvites = theApp.m_bAllowInvites;
	m_bAcceptNMCalls = theApp.m_bAcceptNMCalls;
	m_bPlaySounds = theApp.m_bPlaySounds;
	m_bShowIdentity = theApp.m_bShowIdentity;
	m_bAllowFileTX = theApp.m_bAllowFileTX;
	//}}AFX_DATA_INIT

	m_bUseRatings	= FALSE;
}

CSettingsPage::~CSettingsPage()
{
}

void CSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsPage)
	DDX_Control(pDX, IDC_ADVANCED_RATINGS_GROUPBOX, m_RatingsGroupBox);
	DDX_Control(pDX, IDC_RATINGS_ICON, m_RatingsIcon);
	DDX_Control(pDX, IDC_RATINGS_TEXT, m_RatingsText);
	DDX_Control(pDX, IDC_RATINGS_TURN_ON, m_RatingsOn);
	DDX_Control(pDX, IDC_ADVANCED_RATINGS_BUTTON, m_AdvancedRatings);
	DDX_Control(pDX, IDC_COMICSDATA, m_checkData);
	DDX_Control(pDX, IDC_SAVE, m_checkSave);
	DDX_Check(pDX, IDC_SAVE, m_bSave);
	DDX_Check(pDX, IDC_COMICSDATA, m_bComicsData);
	DDX_Check(pDX, IDC_ACCEPTWHISPERS, m_bAcceptWhispers);
	DDX_Check(pDX, IDC_SHOWARRIVALS, m_bShowArrivals);
	DDX_Text(pDX, IDC_SOUNDPATH, m_strSoundPath);
	DDX_Check(pDX, IDC_ALLOWINVITES, m_bAllowInvites);
	DDX_Check(pDX, IDC_NETMEETING_AUTOSTART, m_bAcceptNMCalls);
	DDX_Check(pDX, IDC_PLAYSOUNDS, m_bPlaySounds);
	DDX_Check(pDX, IDC_SHOWIDENTITY, m_bShowIdentity);
	DDX_Check(pDX, IDC_ALLOW_FILETX, m_bAllowFileTX);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CSettingsPage)
	ON_BN_CLICKED(IDC_COMICSDATA, OnComicsdata)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_ADVANCED_RATINGS_BUTTON, OnAdvancedRatingsButton)
	ON_BN_CLICKED(IDC_RATINGS_TURN_ON, OnRatingsTurnOn)
	ON_BN_CLICKED(IDC_ACCEPTWHISPERS, OnAcceptwhispers)
	ON_BN_CLICKED(IDC_SHOWARRIVALS, OnShowarrivals)
	ON_EN_CHANGE(IDC_SOUNDPATH, OnChangeSoundpath)
	ON_BN_CLICKED(IDC_ALLOWINVITES, OnAllowinvites)
	ON_BN_CLICKED(IDC_NETMEETING_AUTOSTART, OnAcceptNMCalls)
	ON_BN_CLICKED(IDC_PLAYSOUNDS, OnPlaysounds)
	ON_BN_CLICKED(IDC_SHOWIDENTITY, OnShowidentity)
	ON_BN_CLICKED(IDC_ALLOW_FILETX, OnAllowFiletx)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage message handlers
//extern void SetPanelsWide(int);
// extern void SetPanelsHigh(int);
//extern BOOL OKPanelWidth(int);
// extern BOOL OKPanelHeight(int);

BOOL CSettingsPage::OnSetActive() 
{
	if(GetChatDoc() && theApp.m_bEmbedded)
		m_checkSave.EnableWindow(FALSE);

	// RamuM
    // Load the Ratings DLL (if possible)
    if ( NULL == g_hinstRatings )
		g_hinstRatings = LoadLibrary(c_tszRatingsDLL);

    if ( NULL == g_hinstRatings )
	{
        m_AdvancedRatings.EnableWindow(FALSE);
        m_RatingsOn.EnableWindow(FALSE);
        m_RatingsText.EnableWindow(FALSE);
        m_RatingsIcon.EnableWindow(FALSE);
        m_RatingsGroupBox.EnableWindow(FALSE);
	}
	else
	{
		// if MSRATING.DLL not around, then don't do this call.  By not
		// doing this, it will keep the "Enable Ratings" text on the button 
		// but greyed off.
		if (g_hinstRatings)
			m_bUseRatings = RatingEnabledQuery() == S_OK; 

		m_strRatingsButtonText.LoadString(m_bUseRatings ? IDS_RATINGS_TURN_OFF : IDS_RATINGS_TURN_ON);
		SetDlgItemText(IDC_RATINGS_TURN_ON,m_strRatingsButtonText);
	}

	return CCSPropertyPage::OnSetActive();
}

#if 0
BOOL CSettingsPage::OnKillActive() 
{
	// disable ApplyNow button
//	SetModified(FALSE);
	return CCSPropertyPage::OnKillActive();
}
#endif

// manage the send comics data variable
void CSettingsPage::OnComicsdata() 
{
	m_bComicsData = !m_bComicsData;
	SetModified(TRUE);
}


// manage the save on close variable
void CSettingsPage::OnSave() 
{
	m_bSave = !m_bSave;
	SetModified(TRUE);
}

// manage the save on close variable
void CPersonalPage::OnSave() 
{
	m_bSave = !m_bSave;
	SetModified(TRUE);
}


void CSettingsPage::OnAcceptwhispers() 
{
	m_bAcceptWhispers = !m_bAcceptWhispers;
	SetModified(TRUE);
}

void CPersonalPage::OnAcceptwhispers() 
{
	m_bAcceptWhispers = !m_bAcceptWhispers;
	SetModified(TRUE);
}

void CSettingsPage::OnAllowinvites() 
{
	m_bAllowInvites = !m_bAllowInvites;
	SetModified(TRUE);
}

void CSettingsPage::OnAllowFiletx() 
{
	m_bAllowFileTX = !m_bAllowFileTX;
	SetModified(TRUE);
}

void CSettingsPage::OnChangeSoundpath() 
{
	SetModified(TRUE);
}


void CSettingsPage::OnOK() 
{
	BOOL ToggleSendComicsData();
	BOOL GetSendComicsData();	
	
	// verify SendComicsData and commit
	if(!m_bComicsData != GetSendComicsData())
		ToggleSendComicsData();

	// commit the save boolean
	theApp.m_bPrompt = m_bSave;

	extern CPtrList g_docs;
	POSITION pos = g_docs.GetHeadPosition();
	while (pos) {
		CChatDoc *doc = (CChatDoc *)g_docs.GetNext(pos);
		doc->SetModifiedFlag(m_bSave);
	}

	// commit the accept whispers boolean
	theApp.m_bAcceptWhispers = m_bAcceptWhispers;
	theApp.m_bAllowInvites = m_bAllowInvites;
	theApp.m_bAllowFileTX = m_bAllowFileTX;
	theApp.m_bShowArrivals = m_bShowArrivals;
	theApp.m_bPlaySounds = m_bPlaySounds;
	theApp.m_bAcceptNMCalls = m_bAcceptNMCalls;
	theApp.m_bShowIdentity = m_bShowIdentity;

	theApp.m_soundPath = m_strSoundPath;

	CCSPropertyPage::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CPersonalPage property page

// IMPLEMENT_DYNCREATE(CPersonalPage, CCSPropertyPage)


const char *GetMyName(), *GetMyEmail(), *GetMyHomePage(), *GetMyRealName();
void SetMyRealName(const char *charName);
void SetMyEmail(const char *email);
void SetMyHomePage(const char *homePage);

BOOL GetProfileString(CRtfCtrl &rtfProfile);

CPersonalPage::CPersonalPage(UINT id) : CCSPropertyPage(id /*CPersonalPage::IDD*/)
{
	//{{AFX_DATA_INIT(CPersonalPage)
	//}}AFX_DATA_INIT
	GetProfileString(m_rtfProfile);
	m_rtfProfile.m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_rtfProfile.DefineDefaultCharFormat();

	m_strNickname	= GetMyName();
	m_strRealName	= GetMyRealName();
	m_strEmail		= GetMyEmail();
	m_strHomePage	= GetMyHomePage();

	m_editHomePage.m_strInvalid = " ";
	m_editEmail.m_strInvalid = " ";

	m_bSave				= theApp.m_bPrompt;
	m_bAcceptWhispers	= theApp.m_bAcceptWhispers;
	m_bShowArrivals		= theApp.m_bShowArrivals;
}

CPersonalPage::~CPersonalPage()
{
}

void CPersonalPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPersonalPage)
	DDX_Control(pDX, IDC_REALNAME, m_editRealName);
	DDX_Control(pDX, IDC_HOMEPAGE, m_editHomePage);
	DDX_Control(pDX, IDC_EMAIL, m_editEmail);
	DDX_Control(pDX, IDC_NICKNAME, m_editNickname);
	DDX_Control(pDX, IDC_PROFILE_RICHEDIT, m_rtfProfile);
	DDX_Text(pDX, IDC_NICKNAME, m_strNickname);
	DDV_MaxChars(pDX, m_strNickname, 20);
	DDX_Text(pDX, IDC_REALNAME, m_strRealName);
	DDV_MaxChars(pDX, m_strRealName, 30);
	DDX_Text(pDX, IDC_EMAIL, m_strEmail);
	DDV_MaxChars(pDX, m_strEmail, 100);
	DDX_Text(pDX, IDC_HOMEPAGE, m_strHomePage);
	DDV_MaxChars(pDX, m_strHomePage, 200);
	DDX_Control(pDX, IDC_SAVE, m_checkSave);
	DDX_Check(pDX, IDC_SAVE, m_bSave);
	DDX_Check(pDX, IDC_ACCEPTWHISPERS, m_bAcceptWhispers);
	DDX_Check(pDX, IDC_SHOWARRIVALS, m_bShowArrivals);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPersonalPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CPersonalPage)
	ON_NOTIFY(EN_SELCHANGE, IDC_PROFILE_RICHEDIT, OnChangeProfile)
	ON_NOTIFY(EN_MSGFILTER, IDC_PROFILE_RICHEDIT, OnFilterProfile)
	ON_WM_CLOSE()
	ON_EN_CHANGE(IDC_NICKNAME, OnChangeEdit)
	ON_EN_CHANGE(IDC_REALNAME, OnChangeEdit)
	ON_EN_CHANGE(IDC_HOMEPAGE, OnChangeEdit)
	ON_EN_CHANGE(IDC_EMAIL, OnChangeEdit)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	ON_BN_CLICKED(IDC_ACCEPTWHISPERS, OnAcceptwhispers)
	ON_BN_CLICKED(IDC_SHOWARRIVALS, OnShowarrivals)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CPersonalPage::OnChangeProfile(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	SetModified(TRUE);

	*plResult = 0L;
}


void CPersonalPage::OnFilterProfile(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct);

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfProfile.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


const DWORD CPersonalPage::m_nHelpIDs[] =
{
	IDC_REALNAME,			IDH_REALNAME,
	IDC_NICKNAME,			IDH_NICKNAME,
	IDC_PROFILE_RICHEDIT,	IDH_PROFILE,
	IDC_EMAIL,				IDH_OPTIONS_PERSINFOTAB_EMAIL,
	IDC_HOMEPAGE,			IDH_OPTIONS_PERSINFOTAB_WEBPAGE,

	IDC_ACCEPTWHISPERS,		IDH_ACCEPT_WHISPERS,
	IDC_SHOWARRIVALS,		IDH_SHOW_ARRIVALS,
	IDC_SAVE,				IDH_PROMPT_TO_SAVE,
	0, 0
};

/////////////////////////////////////////////////////////////////////////////
// CPersonalPage message handlers


BOOL GetProfileString(CRtfCtrl &rtfProfile) 
{
	ASSERT(!rtfProfile.m_prgdwFormatting);

	if ("" != theApp.m_myProfile)
	{
		rtfProfile.m_prgdwFormatting = new CDWordArray;

		char*	szControlFull = strdup((LPCTSTR) theApp.m_myProfile);
		char*	szControlLess = SzControlLess(szControlFull, rtfProfile.m_prgdwFormatting);

		rtfProfile.m_strText = CString(szControlLess);
		free(szControlFull);

		return TRUE;
	}
	else 
	{
		rtfProfile.m_strText.LoadString(ID_DEFAULT_PROFILE);	// no profile
		return FALSE;
	}
}


void CPersonalPage::OnOK() 
{
	CString strControlFull = m_rtfProfile.m_strText;
	if (m_rtfProfile.m_prgdwFormatting)
	{
		char* szCtrlFull = SzControlFull((LPCTSTR) m_rtfProfile.m_strText, m_rtfProfile.m_prgdwFormatting);
		if (szCtrlFull)
		{
			strControlFull = CString(szCtrlFull);
			delete [] szCtrlFull;
		}
	}
	theApp.m_myProfile = strControlFull;

	if (m_strNickname.IsEmpty())
		m_strNickname.LoadString(IDS_DEFAULT_NICK);
	//	CString encoded_nick = EncodeNick(m_strNickname);
	//	CString decoded_nick = DecodeNick(encoded_nick);
	if (GetDefaultProto() && stricmp(m_strNickname, GetMyName()) != 0)
		GetDefaultProto()->ChatSetNick(m_strNickname);

	SetMyRealName(m_strRealName);
	SetMyEmail(m_strEmail);
	SetMyHomePage(m_strHomePage);

	if (theApp.m_bDoCB32)
	{
		// commit the accept whispers boolean
		theApp.m_bAcceptWhispers = m_bAcceptWhispers;
		theApp.m_bShowArrivals = m_bShowArrivals;
		
		// commit the save boolean
		if (theApp.m_bPrompt != m_bSave)
		{
			theApp.m_bPrompt = m_bSave;
			extern CPtrList g_docs;
			POSITION pos = g_docs.GetHeadPosition();
			while (pos)
			{
				CChatDoc *doc = (CChatDoc*) g_docs.GetNext(pos);
				doc->SetModifiedFlag(m_bSave);
			}
		}
	}

	// Update these properties with the system here.
	CCSPropertyPage::OnOK();
}


BOOL CPersonalPage::OnSetActive() 
{
	m_rtfProfile.LimitText(MAX_INPUTLEN);
	m_rtfProfile.UseDefaultCharFormat();
	m_rtfProfile.bSetTextColor(m_rtfProfile.m_crTextColor);
	m_rtfProfile.bSetWindowFormattedText(m_rtfProfile.m_strText, m_rtfProfile.m_prgdwFormatting);

	m_editRealName.SetFont(&theApp.m_fontGui);
	m_editHomePage.SetFont(&theApp.m_fontGui);
	m_editEmail.SetFont(&theApp.m_fontGui);
	m_editNickname.SetFont(&theApp.m_fontGui);

	// Need to add the EN_SELCHANGE notification to the dwEventMask of the rich text control
	DWORD dwEventMask = (DWORD) m_rtfProfile.GetEventMask();
	m_rtfProfile.SetEventMask(dwEventMask | ENM_SELCHANGE | ENM_MOUSEEVENTS);

	// We need to store away a pointer to the ge when it'spersonal pa open, so we can change
	// the nickname in this dialog when it's overridden by a new edit into the special nickname
	// correcting dialog.
	cui.m_pvPersonalPage = this;
	int iStat = GetDefaultProto()->GetConnectionStatus();
	GetDlgItem(IDC_REALNAME)->EnableWindow(!theApp.m_bDoCB32 && iStat == CX_DISCONNECTED);

	return CCSPropertyPage::OnSetActive();
}


BOOL CPersonalPage::OnKillActive()
{
	if (m_rtfProfile.m_prgdwFormatting)
	{
		m_rtfProfile.m_prgdwFormatting->RemoveAll();
		delete m_rtfProfile.m_prgdwFormatting;
	}
	m_rtfProfile.m_prgdwFormatting = PRGDWGetFormatting(&m_rtfProfile, m_rtfProfile.m_pFont, m_rtfProfile.m_crTextColor);

	m_rtfProfile.GetWindowText(m_rtfProfile.m_strText); 

	return CPropertyPage::OnKillActive();
}


void CPersonalPage::OnClose() 
{
	cui.m_pvPersonalPage = NULL;
	CCSPropertyPage::OnClose();
}


void CPersonalPage::OnChangeEdit() 
{
	SetModified(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CCharacterPage property page

//IMPLEMENT_DYNCREATE(CCharacterPage, CCSPropertyPage)

CCharacterPage::CCharacterPage() : CCSPropertyPage(CCharacterPage::IDD)
{
	//{{AFX_DATA_INIT(CCharacterPage)
	m_strList = "";
	//}}AFX_DATA_INIT
	const char *GetMyCharacter();

	m_strSel = GetMyCharacter();	// default
	if (GetChatDoc() && GetChatDoc()->m_bComicView) {
		CAvatarX *myAv = MyAvatar();
		if (myAv) m_strSel = myAv->OriginalName();  // the usual case
	}
}

CCharacterPage::~CCharacterPage()
{
}

void CCharacterPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacterPage)
	DDX_Control(pDX, IDC_AVLIST, m_avList);
	DDX_LBString(pDX, IDC_AVLIST, m_strList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCharacterPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CCharacterPage)
	ON_LBN_SELCHANGE(IDC_AVLIST, OnSelchangeAvlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const DWORD CCharacterPage::m_nHelpIDs[] =
{
	IDC_AVLIST,				IDH_CHAR_LIST,
	5,						IDH_CHAR_PRVW,
	0, 0
};

/////////////////////////////////////////////////////////////////////////////
// CCharacterPage message handlers

void FirstCharUpper(char *szName) inline {
	int nBytes = GetMime() ? iBytesofChar(szName[0]) : 1;
	CharUpperBuff(szName, nBytes);
}


BOOL CCharacterPage::OnSetActive() 
{
	long hFind;
	struct _finddata_t fd;
   	CRect rAvatarPreview(cxPosAvatarPage, cyPosAvatarPage, (cxPosAvatarPage + cxAvatarPage),
   	                     (cyPosAvatarPage + cyAvatarPage));
	MapDialogRect(rAvatarPreview);

	CCSPropertyPage::OnSetActive();

    // build file search strings
    CString strPattern;

    m_strAvatarFiles.RemoveAll();
	strPattern = theApp.GetAvatarDir();
	strPattern += "\\*.avb";
	hFind = _findfirst( (char *) (const char *) strPattern, &fd );
	if( hFind != -1 )
	{
		do 
		{
			if (fd.attrib != _A_SUBDIR)
			{
				char szFName[_MAX_FNAME];

				_splitpath( fd.name, NULL, NULL, szFName, NULL);
				FirstCharUpper(szFName);
				m_avList.AddString(szFName);
		   }

		} while( _findnext( hFind, &fd ) != -1 );
	}

	CAvatarX *av = GetAvatar2(m_strSel);
	if (!av) av = GetAvatar3("X");  // choose one sequentially
	m_strSel = av->OriginalName();
	int index = m_avList.FindStringExact(0, m_strSel);
	if (index != LB_ERR) m_avList.SetCurSel(index);
#if 0
	if (!m_strSel.IsEmpty()) {
		int index = m_avList.FindStringExact(0, m_strSel);
		if (index == LB_ERR) {
			char avName[100];
			void GetNextAvatarName(char *);
			GetNextAvatarName(avName);
			m_strSel = avName;
			index = m_avList.FindStringExact(0, m_strSel);
		}
		if (index != LB_ERR) m_avList.SetCurSel(index);
	}
#endif

	// create body cam pane
	VERIFY( m_wndCharSelBodyCam.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rAvatarPreview, this, 5 ) );
	m_wndCharSelBodyCam.RecalcRetainedBMP();
	m_wndCharSelBodyCam.m_forcedDelete = FALSE;
	cui.m_pvCharSelBodyCamWnd = &m_wndCharSelBodyCam;
	m_wndCharSelBodyCam.m_avatar = av;

	return TRUE;
}

void CCharacterPage::OnOK() 
{
	void SetMyCharacter(const char *);
	if (!GetChatDoc() || !GetChatDoc()->m_bComicView)
		SetMyCharacter(m_strSel);
	
	else {
		CAvatarX *modelAv = m_wndCharSelBodyCam.m_avatar;
		USHORT modelAvID = modelAv->m_avatarID;
		if (MyAvatarID() != modelAvID) {   // Did we choose a different avatar?
			extern CUserInfo *puiSelf;
			if (puiSelf)  // i.e., we're in channel, so this is safe
				AddAndExecute(new ChangeAvatarEntry(puiSelf, modelAv->m_name));
			else SetMyAvatar(modelAv->m_name);
		}
		m_wndCharSelBodyCam.DestroyWindow();
		cui.m_pvCharSelBodyCamWnd = NULL;
	}

	CCSPropertyPage::OnOK();
}

void CCharacterPage::OnCancel() 
{
	m_wndCharSelBodyCam.DestroyWindow();
    cui.m_pvCharSelBodyCamWnd = NULL;
	CCSPropertyPage::OnCancel();
}


void CCharacterPage::OnSelchangeAvlist() 
{
	int index = m_avList.GetCurSel();
	if (index != LB_ERR) {
		char newName[_MAX_FNAME];
		m_avList.GetText(index, newName);
		if (stricmp(newName, m_wndCharSelBodyCam.m_avatar->m_name)) {
			CAvatarX *newAv = GetAvatar2(newName);
			if (newAv) {
		        m_wndCharSelBodyCam.m_avatar = newAv;
				m_strSel = newName;
				m_wndCharSelBodyCam.RefreshBody();
			    SetModified(TRUE);
			} else {
				CString mesg;
				mesg.LoadString(IDS_INVALIDART);
				VERIFY(ReplaceToken(mesg, CString("%1"), newName));
				AfxMessageBox(mesg);
				int sel = m_avList.FindString(-1, m_wndCharSelBodyCam.m_avatar->OriginalName());
				m_avList.SetCurSel(sel);
			}
		}
	}	
}


BOOL CCharacterPage::OnKillActive() 
{
	m_wndCharSelBodyCam.DestroyWindow();
    cui.m_pvCharSelBodyCamWnd = NULL;
	m_avList.ResetContent();
	return CCSPropertyPage::OnKillActive();
}


/////////////////////////////////////////////////////////////////////////////
// CBackgroundPage property page

//IMPLEMENT_DYNCREATE(CBackgroundPage, CCSPropertyPage)

CBackgroundPage::CBackgroundPage() : CCSPropertyPage(CBackgroundPage::IDD)
{
	//{{AFX_DATA_INIT(CBackgroundPage)
	m_strBackground = _T("");
	//}}AFX_DATA_INIT
	const char *GetCurrentBackDropName();
	m_strCurrentSel = GetCurrentBackDropName();
	m_rect.left = m_rect.right = m_rect.top = m_rect.bottom = 0;
}

CBackgroundPage::~CBackgroundPage()
{
}

void CBackgroundPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackgroundPage)
	DDX_Control(pDX, IDC_BACKLIST, m_listBack);
	DDX_LBString(pDX, IDC_BACKLIST, m_strBackground);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackgroundPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CBackgroundPage)
	ON_LBN_SELCHANGE(IDC_BACKLIST, OnSelchangeBacklist)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const DWORD CBackgroundPage::m_nHelpIDs[] =
{
	IDC_BACKLIST,			IDH_BKG_LIST,
	0, 0
};

/////////////////////////////////////////////////////////////////////////////
// CBackgroundPage message handlers

BOOL CBackgroundPage::OnSetActive() 
{
    CString strPattern;
	long hFind;
	struct _finddata_t fd;

	strPattern = theApp.GetBackDropDir();
	strPattern += "\\*.bmp";
	hFind = _findfirst( (char *) (const char *) strPattern, &fd );
	if( hFind != -1 )
	{
		do 
		{
			if (fd.attrib != _A_SUBDIR)
			{
				char szFName[_MAX_FNAME];

				_splitpath( fd.name, NULL, NULL, szFName, NULL);
				FirstCharUpper(szFName);
				m_listBack.AddString(szFName);
		   }

		} while( _findnext( hFind, &fd ) != -1 );
	}

	// set selection to current background
	const char *prevName = m_strCurrentSel;
	if (prevName) {
		int index = m_listBack.FindStringExact(0, prevName);
		if (index != LB_ERR) m_listBack.SetCurSel(index);
	}
	
	// now get the position of the preview window...
	CRect tempRect;

	CWnd* pWnd = GetDlgItem(IDC_BACKPREV);
	pWnd->GetWindowRect(&tempRect);
	m_rect.left = tempRect.left;
	m_rect.top = tempRect.top;

	pWnd = GetDlgItem(IDC_PREVX);
	pWnd->GetWindowRect(&tempRect);
	m_rect.right = tempRect.right;
	pWnd = GetDlgItem(IDC_PREVY);

	pWnd->GetWindowRect(&tempRect);
	m_rect.bottom = tempRect.bottom;

	ScreenToClient(m_rect);
	PreviewSelection();
	
	return CCSPropertyPage::OnSetActive();
}

// load up the new background and repaint
void CBackgroundPage::PreviewSelection()
{
	CString strPath;
	int index = m_listBack.GetCurSel();
	m_listBack.GetText(index,m_strCurrentSel);
	strPath = theApp.GetBackDropDir();
	strPath += "\\" + m_strCurrentSel + ".bmp";
	m_dib.Load(LPSTR(LPCTSTR(strPath)));
	InvalidateRect(m_rect);
}


void CBackgroundPage::OnSelchangeBacklist() 
{
	SetModified(TRUE);
	PreviewSelection();
}

void CBackgroundPage::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	int iOldMode = dc.SetStretchBltMode(STRETCHMODE);
	CPalette *oldPal = dc.SelectPalette(&ghPalette, TRUE);

	m_dib.Draw(&dc,m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height());

	if (oldPal) dc.SelectPalette(oldPal, TRUE);  // cleanup
	dc.SetStretchBltMode(iOldMode);
}

BOOL CBackgroundPage::OnKillActive() 
{
	m_listBack.ResetContent();
	
	return CCSPropertyPage::OnKillActive();
}


void CBackgroundPage::OnOK() 
{
	// would also be nice to implement a GetCurrentBackground so I can 
	// set it when the dialog comes up.
	void SetBackDrop(const char *);
	int status = CX_DISCONNECTED;
	if (currentRoom) status = currentRoom->GetConnectionStatus();
	if (status == CX_INCHANNEL || status == CX_DISCONNECTED)
		AddAndExecute(new ChangeBackDropEntry((const char *) m_strCurrentSel));
	else SetBackDrop(m_strCurrentSel);

	CCSPropertyPage::OnOK();
}


void CSettingsPage::OnAdvancedRatingsButton() 
{
	RatingSetupUI(GetSafeHwnd(), (LPCSTR) NULL);        
}

void CSettingsPage::OnRatingsTurnOn() 
{
    if (SUCCEEDED(RatingEnable(GetSafeHwnd(), (LPCSTR)NULL, !m_bUseRatings))) 
    {
        m_bUseRatings = RatingEnabledQuery() == S_OK;
		m_strRatingsButtonText.LoadString(m_bUseRatings ? IDS_RATINGS_TURN_OFF : IDS_RATINGS_TURN_ON);
		SetDlgItemText(IDC_RATINGS_TURN_ON,m_strRatingsButtonText);
    }
}

const DWORD CSettingsPage::m_nHelpIDs[] =
{
	IDC_COMICSDATA,			IDH_NO_GRAPHX,
	IDC_ACCEPTWHISPERS,		IDH_ACCEPT_WHISPERS,
	IDC_ALLOWINVITES,		IDH_ACCEPT_INVITE,
	IDC_ALLOW_FILETX,		IDH_OPTIONS_SETTINGS_DCC,
	IDC_SHOWARRIVALS,		IDH_SHOW_ARRIVALS,
	IDC_SAVE,				IDH_PROMPT_TO_SAVE,
	IDC_SOUNDPATH,			IDH_SOUND_PATH,
	IDC_SHOWIDENTITY,		IDH_OPTIONS_SETTINGS_GETID,
	IDC_PLAYSOUNDS,			IDH_OPTIONS_SETTINGS_PLAYSOUNDS,
	IDC_NETMEETING_AUTOSTART, IDH_OPTIONS_SETTINGS_REC_NETMEET,
	IDC_RATINGS_TURN_ON,	IDH_SETTINGS_ENABLE_RATINGS,
	IDC_ADVANCED_RATINGS_BUTTON, IDH_SETTINGS_SETTINGS_BUTTON,
	IDC_GROUP0,				IDH_GROUP_ID,
	IDC_ADVANCED_RATINGS_GROUPBOX, IDH_SETTINGS_CONTENT_ADVISOR,
	IDC_RATINGS_TEXT,		IDH_SETTINGS_CONTENT_ADVISOR,
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CTextFontPage property page

//IMPLEMENT_DYNCREATE(CTextFontPage, CCSPropertyPage)

CTextFontPage::CTextFontPage(UINT id) : CCSPropertyPage(id)
{
	//{{AFX_DATA_INIT(CTextFontPage)
	m_bHeaderSeparate = ISTRUE(theApp.m_flags1 & F1_HEADERSEPARATE);
	//}}AFX_DATA_INIT

	m_spacing = theApp.m_textSpacing;
	m_hostHdrsBold = (theApp.m_iHostHighlight & HH_BOLD_HEADERS) != 0;	// != 0 necessary due to MFC idiocy
	m_hostMsgsBold = (theApp.m_iHostHighlight & HH_BOLD_MESSAGES) != 0;	// != 0 necessary due to MFC idiocy
	m_bCfInitialized = theApp.m_bCfInitialized;
	
	if (m_bCfInitialized) {
		for (int i = 0; i < 10; i++) {
			m_cfArray[i] = theApp.m_cfArray[i];
		}
	}
}

CTextFontPage::~CTextFontPage()
{
}

void CTextFontPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTextFontPage)
	DDX_Check(pDX, IDC_HOST_HDRS_BOLD, m_hostHdrsBold);
	DDX_Check(pDX, IDC_HOST_MSGS_BOLD, m_hostMsgsBold);
	DDX_Check(pDX, IDC_CHKHEADERSEPARATE, m_bHeaderSeparate);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTextFontPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CTextFontPage)
	ON_BN_CLICKED(ID_SETFONT, OnSetfont)
	ON_BN_CLICKED(IDC_LINESPACE_ALL, OnLinespaceAll)
	ON_BN_CLICKED(IDC_LINESPACE_DIFFERENT, OnLinespaceDifferent)
	ON_BN_CLICKED(IDC_LINESPACE_NONE, OnLinespaceNone)
	ON_BN_CLICKED(ID_RESET_TEXTFONTS, OnResetTextfonts)
	ON_BN_CLICKED(IDC_HOST_HDRS_BOLD, OnHostHdrsBold)
	ON_BN_CLICKED(IDC_HOST_MSGS_BOLD, OnHostMsgsBold)
	ON_BN_CLICKED(IDC_CHKHEADERSEPARATE, OnHeaderSeparate)
	//}}AFX_MSG_MAP
//	ON_NOTIFY(EN_LINK, 5, HandleLink)
END_MESSAGE_MAP()

const DWORD CTextFontPage::m_nHelpIDs[] =
{
	IDC_LINESPACE_ALL,		IDH_ALL_MSGS,
	IDC_LINESPACE_DIFFERENT,IDH_DIFF_MSGS,
	IDC_LINESPACE_NONE,		IDH_NO_BLANKS,
	IDC_CHKHEADERSEPARATE,	IDH_OPTIONS_TEXTVIEW_HEADERSMSGS,
	ID_SETFONT,				IDH_CHANGE_FONT,
	ID_RESET_TEXTFONTS,		IDH_RESET_FONT,
	IDC_HOST_HDRS_BOLD,		IDH_HOST_HEADERS,
	IDC_HOST_MSGS_BOLD,		IDH_HOST_BOLD,
	IDC_SPACE_GROUP,		IDH_GROUP_ID,
	IDC_GROUP1,				IDH_GROUP_ID,
	IDC_GROUP2,				IDH_GROUP_ID,
	0, 0
};

/////////////////////////////////////////////////////////////////////////////
// CTextFontPage message handlers

//extern CTextCore g_textCore;

void CTextFontPage::OnHeaderSeparate() 
{
	m_bHeaderSeparate = !m_bHeaderSeparate;
	SetModified(TRUE);
}

void CTextFontPage::OnSetfont() 
{
	CMyFontDialog	fDialog;
	CHOOSEFONT		*cf = &fDialog.m_cf;

	/*
	This would create some additional problems because MS Sans Serif, our default text font, is not a printable font,
	but printing seems to work anyway in text mode.
	// REGISB 11/14/97 We might want to show printable fonts in the dialog box
	CPrintDialog	dlg(FALSE);	// Don't show it!

	if (theApp.GetPrinterDeviceDefaults(&dlg.m_pd) && dlg.CreatePrinterDC())
	{
		cf->Flags |= (CF_BOTH | CF_SCALABLEONLY | CF_WYSIWYG);	// we want fonts that are both screen *and* printer fonts 
		cf->hDC = dlg.m_pd.hDC;
	}
	else
	{
		cf->Flags |= CF_SCREENFONTS;							// no default printer -> we just show the screen fonts.
		cf->Flags &= ~CF_PRINTERFONTS;
	}
	*/

	cf->lpTemplateName = MAKEINTRESOURCE(FORMATDLGORD31);
	cf->rgbColors = RGB(0, 0, 0);
	cf->Flags |= (CF_NOVERTFONTS | CF_ENABLETEMPLATE);

	if (fDialog.DoModal() == IDOK)
	{
		m_bCfInitialized = fDialog.m_bCfInitialized;
		if (m_bCfInitialized)
		{
			for (int i = 0; i < 10; i++)
				m_cfArray[i] = fDialog.m_cfArray[i];
		}
		SetModified(TRUE);
	}
// called from options dialog.  it will take care of focus restoration
}

// this bypasses the Apply and TextFontPage dependencies for direct invocation

void SetTextFont()
{
	CMyFontDialog	fDialog;
	CHOOSEFONT		*cf = &fDialog.m_cf;

	/*
	// REGISB 11/14/97 We might want to show printable fonts in the dialog box
	CPrintDialog	dlg(FALSE);	// Don't show it!

	if (theApp.GetPrinterDeviceDefaults(&dlg.m_pd) && dlg.CreatePrinterDC())
	{
		cf->Flags |= (CF_BOTH | CF_SCALABLEONLY | CF_WYSIWYG);	// we want fonts that are both screen *and* printer fonts 
		cf->hDC = dlg.m_pd.hDC;
	}
	else
	{
		cf->Flags |= CF_SCREENFONTS;							// no default printer -> we just show the screen fonts.
		cf->Flags &= ~CF_PRINTERFONTS;
	}
	*/

	cf->lpTemplateName = MAKEINTRESOURCE(FORMATDLGORD31);
	cf->rgbColors = RGB(0, 0, 0);
	cf->Flags |= (CF_NOVERTFONTS | CF_ENABLETEMPLATE);

	if (fDialog.DoModal() == IDOK)
	{
		theApp.m_bCfInitialized = TRUE;
		for (int i = 0; i < 10; i++)
			theApp.m_cfArray[i] = fDialog.m_cfArray[i];
		theApp.m_textColor = (theApp.m_cfArray[2].dwMask & CFM_COLOR) ? theApp.m_cfArray[2].crTextColor : GetSysColor(COLOR_WINDOWTEXT);
	}
	
	GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
	
	// reinitialize font choices in main text view (must be done after values above set)
	void InitializeTextCores(BOOL, BOOL = FALSE), InitializeWhisperCores(BOOL);

	InitializeTextCores(FALSE, TRUE);
	InitializeWhisperCores(FALSE);
}

void CTextFontPage::OnLinespaceAll() 
{
	m_spacing = TEXT_VIEW_BLANK_ALWAYS;
	SetModified(TRUE);
}

void CTextFontPage::OnLinespaceDifferent() 
{
	m_spacing = TEXT_VIEW_BLANK_DIFFTYPES;
	SetModified(TRUE);
}

void CTextFontPage::OnLinespaceNone() 
{
	m_spacing = TEXT_VIEW_BLANK_NEVER;
	SetModified(TRUE);
}


void CTextFontPage::OnOK() 
{
	BOOL restoreOld = !m_bCfInitialized;

	theApp.m_textSpacing = m_spacing;
	theApp.m_bCfInitialized = m_bCfInitialized;

	if (m_bCfInitialized)
	{
		for (int i = 0; i < 10; i++) 
			theApp.m_cfArray[i] = m_cfArray[i];
		theApp.m_textColor = (theApp.m_cfArray[2].dwMask & CFM_COLOR) ? theApp.m_cfArray[2].crTextColor : GetSysColor(COLOR_WINDOWTEXT);
	}

	theApp.m_iHostHighlight = 0;   // save host highlighting

	if (m_hostHdrsBold) 
		theApp.m_iHostHighlight |= HH_BOLD_HEADERS;
	if (m_hostMsgsBold) 
		theApp.m_iHostHighlight |= HH_BOLD_MESSAGES;

	// Update the m_flags1 flag
	if (m_bHeaderSeparate)
		theApp.m_flags1 |= F1_HEADERSEPARATE;
	else
		theApp.m_flags1 &= ~F1_HEADERSEPARATE;

	// reinitialize font choices in main text view (must be done after values above set)
	void InitializeTextCores(BOOL, BOOL = FALSE), InitializeWhisperCores(BOOL);

	InitializeTextCores(restoreOld, TRUE);
	InitializeWhisperCores(restoreOld);

	CCSPropertyPage::OnOK();
}



BOOL CTextFontPage::OnSetActive() 
{
	CheckRadioButton(IDC_LINESPACE_NONE, IDC_LINESPACE_ALL,
				     IDC_LINESPACE_NONE+m_spacing);
	
	return CCSPropertyPage::OnSetActive();
}

void CSettingsPage::OnShowarrivals() 
{
	m_bShowArrivals = !m_bShowArrivals;
	SetModified(TRUE);
}

void CPersonalPage::OnShowarrivals() 
{
	m_bShowArrivals = !m_bShowArrivals;
	SetModified(TRUE);
}


void CSettingsPage::OnAcceptNMCalls() 
{
	m_bAcceptNMCalls = !m_bAcceptNMCalls;
	SetModified(TRUE);
}

void CSettingsPage::OnPlaysounds() 
{
	m_bPlaySounds = !m_bPlaySounds;
	SetModified(TRUE);
}

void CSettingsPage::OnShowidentity() 
{
	m_bShowIdentity = !m_bShowIdentity;
	SetModified(TRUE);	
}


void CTextFontPage::OnResetTextfonts() 
{
	m_bCfInitialized = FALSE;
	SetModified(TRUE);
}

void CTextFontPage::OnHostHdrsBold() 
{
	SetModified(TRUE);	
}

void CTextFontPage::OnHostMsgsBold() 
{
	SetModified(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CComicsPropPage property page

//IMPLEMENT_DYNCREATE(CComicsPropPage, CCSPropertyPage)

CComicsPropPage::CComicsPropPage() : CCSPropertyPage(CComicsPropPage::IDD)
{
	//{{AFX_DATA_INIT(CComicsPropPage)
		// NOTE: the ClassWizard will add member initialization here
	m_bShowComicRTF = ISTRUE(theApp.m_flags1 & F1_RTFCOMIC);
	//}}AFX_DATA_INIT

	// managing a zero based listbox index vs number of panels
	m_nPanelsSel = CUnitPanelPage::m_panelsPerRow - 1;

	m_bPanelClicked = FALSE;
}

CComicsPropPage::~CComicsPropPage()
{
}

void CComicsPropPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CComicsPropPage)
	DDX_Control(pDX, IDC_PANELS, m_comboPanels);
	DDX_Check(pDX, IDC_SHOWCOMICRTF, m_bShowComicRTF);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComicsPropPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CComicsPropPage)
	ON_BN_CLICKED(IDC_SHOWCOMICRTF, OnShowComicRTF)
	ON_BN_CLICKED(ID_SETFONT, OnSetfont)
	ON_CBN_SELCHANGE(IDC_PANELS, OnSelchangePanels)
	ON_BN_CLICKED(ID_RESET_TEXTFONTS, OnResetfont)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const DWORD CComicsPropPage::m_nHelpIDs[] =
{
	ID_SETFONT,				IDH_CHANGE_FONT,
	ID_RESET_TEXTFONTS,		IDH_RESET_FONT,
	IDC_PANELS,				IDH_LAYOUT,
	IDC_SHOWCOMICRTF,		IDH_OPTIONS_COMICSVIEW_RTF,
	IDC_GROUP0,				IDH_GROUP_ID,
	IDC_GROUP1,				IDH_GROUP_ID,
	0, 0
};

/////////////////////////////////////////////////////////////////////////////
// CComicsPropPage message handlers


void SetComicsFont() 
{
	CFontDialog	fDialog;
	CDC			dcPrinter;
	CHOOSEFONT	*cf = &fDialog.m_cf;
	LOGFONT		*lf = cf->lpLogFont;
	CHAR		szLogFaceName[LF_FACESIZE];
	int			logPixelsY = CPageView::m_dpiY;

	*lf = theApp.m_comicsFont;
	strcpy(szLogFaceName, lf->lfFaceName);

	HDC hDC = ::GetDC(NULL);
	if (hDC) {
		HFONT	hFont, hOldFont;
		CHAR	szPhysFaceName[LF_FACESIZE];

		hFont = ::CreateFontIndirect(lf);
		hOldFont = (HFONT) ::SelectObject(hDC, hFont);

		if (GetTextFace(hDC, LF_FACESIZE, szPhysFaceName))
			strcpy(lf->lfFaceName, szPhysFaceName);

		::SelectObject(hDC, hOldFont);
		::DeleteObject(hFont);

		::ReleaseDC(NULL, hDC);
	} 
//	lf->lfHeight = (int)(((abs(lf->lfHeight) * (float)logPixelsY) / 1440.) + 0.5);  // rounded should be OK
	lf->lfHeight = (int)((lf->lfHeight * logPixelsY) / 1440.0 - 0.5); // round negative number
//	lf->lfHeight = (int)((lf->lfHeight * 13.0) / 250.0 + 0.5); // rounded should be OK
	cf->nSizeMax = 18;
	cf->nSizeMin = 8;
	// cf->Flags &= ~CF_EFFECTS;	// REGISB 08/13, since we now want to support effects and colors

	// REGISB 11/14/97 We might want to show printable fonts in the dialog box
	CPrintDialog	dlg(FALSE);	// Don't show it!

	if (theApp.GetPrinterDeviceDefaults(&dlg.m_pd) && dlg.CreatePrinterDC())
	{
		cf->Flags |= (CF_BOTH | CF_SCALABLEONLY | CF_WYSIWYG);	// we want fonts that are both screen *and* printer fonts 
		cf->hDC = dlg.m_pd.hDC;
	}
	else
	{
		cf->Flags |= CF_SCREENFONTS;							// no default printer -> we just show the screen fonts.
		cf->Flags &= ~CF_PRINTERFONTS;
	}
	
	cf->Flags |= (CF_INITTOLOGFONTSTRUCT | CF_LIMITSIZE | CF_NOVERTFONTS);
	cf->rgbColors = theApp.m_comicsColor;

	if (fDialog.DoModal() == IDOK) {
		// convert from pixels to twips...
		lf->lfHeight = (int)((lf->lfHeight * 1440.0) / logPixelsY - 0.5);
//		lf->lfHeight = abs(lf->lfHeight) * 250 / 13
		lf->lfClipPrecision = CLIP_DFA_OVERRIDE;  // disable FA for our Korean Friends
		theApp.m_comicsColor = cf->rgbColors;
		CUnitPanelPage::SetFonts(*lf, cf->rgbColors);
		if (GetSay()) 
			GetSay()->SetFont(*lf, TRUE);
	}
	else
		strcpy(lf->lfFaceName, szLogFaceName);

	if (GetChatDoc()) 
		GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
}

void CComicsPropPage::OnSetfont() {
	SetComicsFont();
}


void CComicsPropPage::OnResetfont() 
{
	theApp.InitializeComicsFonts();		// this resets m_comicsFont LOGFONT
	CUnitPanelPage::SetFonts(theApp.m_comicsFont, theApp.m_comicsColor);
	if (GetSay())
		GetSay()->SetFont(theApp.m_comicsFont);
}



BOOL CComicsPropPage::OnSetActive() 
{
	// For now we will allow the user to make it up to 4 panels wide
	//    if they want.
	CString entry;
	entry.LoadString(IDS_1_WIDE);
	m_comboPanels.AddString(entry);
	entry.LoadString(IDS_2_WIDE);
	m_comboPanels.AddString(entry);
	entry.LoadString(IDS_3_WIDE);
	m_comboPanels.AddString(entry);
	entry.LoadString(IDS_4_WIDE);
	m_comboPanels.AddString(entry);
	m_comboPanels.SetCurSel(m_nPanelsSel);

	return CCSPropertyPage::OnSetActive();
}

void CComicsPropPage::OnSelchangePanels() 
{
// manage the combobox for panel selection
	m_nPanelsSel = m_comboPanels.GetCurSel();
	SetModified(TRUE);
	m_bPanelClicked = TRUE;
}	


void CComicsPropPage::OnOK() 
{
	// verify the panel selection and commit
	if (m_bPanelClicked)
	{
		POSITION	pos = g_docs.GetHeadPosition();
		int			nPanelWidth, nSmallestPanelWidth = 9999;
		CChatDoc*	doc;

		while (pos)
		{
			doc = (CChatDoc*) g_docs.GetNext(pos);
			if (doc->m_bComicView && !doc->m_bObscured)
			{
				if ((nPanelWidth = ((CPageView*) doc->m_view)->GetProspectivePanelWidth(m_nPanelsSel+1)) < nSmallestPanelWidth)
					nSmallestPanelWidth = nPanelWidth;
			}
		}
		pos = g_docs.GetHeadPosition();

		while (pos)
		{
			doc = (CChatDoc*) g_docs.GetNext(pos);
			if (doc->m_bComicView)
			{
				((CPageView*) doc->m_view)->SetPanelsWide(m_nPanelsSel+1, nSmallestPanelWidth);
			}
		}
		
		m_bPanelClicked = FALSE;
	}

	// Update the m_flags1 flag
	if (m_bShowComicRTF)
		theApp.m_flags1 |= F1_RTFCOMIC;
	else
		theApp.m_flags1 &= ~F1_RTFCOMIC;

	CCSPropertyPage::OnOK();
}


BOOL CComicsPropPage::OnKillActive() 
{
	// clear list box
	m_comboPanels.ResetContent();
	// disable ApplyNow button
	return CCSPropertyPage::OnKillActive();
}


void CComicsPropPage::OnShowComicRTF() 
{
	m_bShowComicRTF = !m_bShowComicRTF;
	SetModified(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CAutomationPage property page

//IMPLEMENT_DYNCREATE(CAutomationPage, CCSPropertyPage)

CAutomationPage::CAutomationPage() : CCSPropertyPage(CAutomationPage::IDD)
{
	//{{AFX_DATA_INIT(CAutomationPage)
	ASSERT(!m_rtfGreetingMesg.m_prgdwFormatting);

	m_rtfGreetingMesg.m_prgdwFormatting = new CDWordArray;

	char*		szControlFull = strdup((LPCTSTR) theApp.m_strGreetingMesg);
	char*		szControlLess = SzControlLess(szControlFull, m_rtfGreetingMesg.m_prgdwFormatting);

	m_rtfGreetingMesg.m_strText = CString(szControlLess);
	m_rtfGreetingMesg.m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_rtfGreetingMesg.DefineDefaultCharFormat();
	m_iGreetingType = theApp.m_iGreetingType;
	free(szControlFull);

	m_rtfMacro.m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_rtfMacro.DefineDefaultCharFormat();

	m_bAutoIgnore = ISTRUE(theApp.m_floodFlags & FLOOD_IGNORE);

	m_strMesgCnt.Format("%d", m_uMesgCnt = theApp.m_floodCount);
	m_strInterval.Format("%d", m_uInterval = theApp.m_floodInterval);
	//}}AFX_DATA_INIT
}

CAutomationPage::~CAutomationPage()
{
}

void CAutomationPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutomationPage)
	DDX_Control(pDX, IDC_MACRORICHEDIT, m_rtfMacro);
	DDX_Control(pDX, IDC_MACRONAME, m_macroNameCtl);
	DDX_Control(pDX, IDC_KEY, m_keyCtl);
	DDX_Control(pDX, IDC_GREETINGMESG, m_rtfGreetingMesg);
	DDX_Control(pDX, IDC_MESGCOUNTSPIN, m_spinMesgCnt);
	DDX_Control(pDX, IDC_INTERVALSPIN, m_spinInterval);
	DDX_Control(pDX, IDC_MESGCOUNT, m_mesgCntCtl);
	DDX_Control(pDX, IDC_INTERVAL, m_intervalCtl);
	DDX_Text(pDX, IDC_MESGCOUNT, m_uMesgCnt);
	DDV_MinMaxUInt(pDX, m_uMesgCnt, 1, 255);
	DDX_Text(pDX, IDC_INTERVAL, m_uInterval);
	DDV_MinMaxUInt(pDX, m_uInterval, 1, 255);
	DDX_Check(pDX, IDC_AUTOIGNORE, m_bAutoIgnore);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutomationPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CAutomationPage)
	ON_NOTIFY(EN_SELCHANGE, IDC_GREETINGMESG, OnChangeGreetingMesg)
	ON_NOTIFY(EN_MSGFILTER, IDC_GREETINGMESG, OnFilterGreetingMesg)
	ON_NOTIFY(EN_MSGFILTER, IDC_MACRORICHEDIT, OnFilterMacro)
	ON_EN_CHANGE(IDC_MESGCOUNT, OnMesgCountChg)
	ON_EN_CHANGE(IDC_INTERVAL, OnIntervalChg)
	ON_BN_CLICKED(IDC_AUTOIGNORE, OnAutoIgnore)
	ON_BN_CLICKED(IDC_NOGREETING, OnNogreeting)
	ON_BN_CLICKED(IDC_SAYGREETING, OnSaygreeting)
	ON_BN_CLICKED(IDC_WHISPERGREETING, OnWhispergreeting)
	ON_BN_CLICKED(IDC_ADD_MACRO, OnAddMacro)
	ON_BN_CLICKED(IDC_DELETE_MACRO, OnDeleteMacro)
	ON_CBN_SELCHANGE(IDC_KEY, OnSelchangeKey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutomationPage message handlers

void CopyMacros(CMacro src[], CMacro dest[]) {
	for (int i = 0; i < NMACROS; i++) {
		dest[i].m_strValue = src[i].m_strValue;
		dest[i].m_strName = src[i].m_strName;
		dest[i].m_bDefined = src[i].m_bDefined;
	}
}


void CAutomationPage::OnChangeGreetingMesg(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	SetModified(TRUE);

	*plResult = 0L;
}


void CAutomationPage::OnFilterGreetingMesg(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct);

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfGreetingMesg.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


void CAutomationPage::OnFilterMacro(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct);

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfMacro.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


BOOL CAutomationPage::OnSetActive() 
{
	if (theApp.m_bDoCB32)
	{
		m_iGreetingType = AGT_NONE;
		GetDlgItem(IDC_NOGREETING)->EnableWindow(FALSE);
		GetDlgItem(IDC_WHISPERGREETING)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAYGREETING)->EnableWindow(FALSE);
	}

	CheckRadioButton(IDC_NOGREETING, IDC_SAYGREETING,
				     IDC_NOGREETING + m_iGreetingType);

	m_rtfGreetingMesg.UseDefaultCharFormat();
	m_rtfGreetingMesg.bSetTextColor(m_rtfGreetingMesg.m_crTextColor);
	m_rtfGreetingMesg.bSetWindowFormattedText(m_rtfGreetingMesg.m_strText, m_rtfGreetingMesg.m_prgdwFormatting);
	m_rtfGreetingMesg.EnableWindow(m_iGreetingType);
	m_rtfGreetingMesg.LimitText(MAX_INPUTLEN);

	m_macroNameCtl.SetFont(&theApp.m_fontGui);

	// Need to add the EN_SELCHANGE & ENM_MOUSEEVENTS notifications to the dwEventMask of the rich text control
	DWORD dwEventMask = (DWORD) m_rtfGreetingMesg.GetEventMask();
	m_rtfGreetingMesg.SetEventMask(dwEventMask | ENM_SELCHANGE | ENM_MOUSEEVENTS);

	m_rtfMacro.UseDefaultCharFormat();
	m_rtfMacro.bSetTextColor(m_rtfMacro.m_crTextColor);
	m_rtfMacro.LimitText(MAX_INPUTLEN);
	
	// Need to add the ENM_MOUSEEVENTS notification to the dwEventMask of the rich text control
	dwEventMask = (DWORD) m_rtfMacro.GetEventMask();
	m_rtfMacro.SetEventMask(dwEventMask | ENM_MOUSEEVENTS);

	CopyMacros(theApp.m_macros, m_macros);

	m_keyCtl.SetCurSel(0);		// display first entry by default
	OnSelchangeKey();

	((CEdit*) GetDlgItem(IDC_MACRONAME))->SetLimitText(20);

	m_mesgCntCtl.EnableWindow(m_bAutoIgnore);
	m_intervalCtl.EnableWindow(m_bAutoIgnore);

	m_spinMesgCnt.EnableWindow(m_bAutoIgnore);
	m_spinInterval.EnableWindow(m_bAutoIgnore);

	m_spinMesgCnt.SetRange(1, 255);
	m_spinInterval.SetRange(1, 255);
	((CEdit*) GetDlgItem(IDC_MESGCOUNT))->SetLimitText(3);
	((CEdit*) GetDlgItem(IDC_INTERVAL))->SetLimitText(3);

	((CEdit*) GetDlgItem(IDC_MESGCOUNT))->GetWindowText(m_strMesgCnt);
	((CEdit*) GetDlgItem(IDC_INTERVAL))->GetWindowText(m_strInterval);

	return CCSPropertyPage::OnSetActive();
}


void CAutomationPage::OnOK() 
{
	CString strControlFull = m_rtfGreetingMesg.m_strText;
	if (m_rtfGreetingMesg.m_prgdwFormatting)
	{
		char* szCtrlFull = SzControlFull((LPCTSTR) m_rtfGreetingMesg.m_strText, m_rtfGreetingMesg.m_prgdwFormatting);
		if (szCtrlFull)
		{
			strControlFull = CString(szCtrlFull);
			delete [] szCtrlFull;
		}
	}
	theApp.m_strGreetingMesg = strControlFull;

	theApp.m_iGreetingType = m_iGreetingType;

	CopyMacros(m_macros, theApp.m_macros);
	if (GetChatDoc())
		GetChatDoc()->UpdateMacroMenu();  // XXX?

	theApp.m_floodCount = m_uMesgCnt;
	theApp.m_floodInterval = m_uInterval;

	if (m_bAutoIgnore)
		theApp.m_floodFlags |= FLOOD_IGNORE;
	else
		theApp.m_floodFlags &= ~FLOOD_IGNORE;

	CCSPropertyPage::OnOK();
}


BOOL CAutomationPage::OnKillActive()
{
	if (m_rtfGreetingMesg.m_prgdwFormatting)
	{
		m_rtfGreetingMesg.m_prgdwFormatting->RemoveAll();
		delete m_rtfGreetingMesg.m_prgdwFormatting;
	}
	m_rtfGreetingMesg.m_prgdwFormatting = PRGDWGetFormatting(&m_rtfGreetingMesg, m_rtfGreetingMesg.m_pFont, m_rtfGreetingMesg.m_crTextColor);

	m_rtfGreetingMesg.GetWindowText(m_rtfGreetingMesg.m_strText); 

	return CPropertyPage::OnKillActive();
}


void CAutomationPage::OnNogreeting() 
{
	m_iGreetingType = AGT_NONE;
	m_rtfGreetingMesg.EnableWindow(FALSE);
	SetModified(TRUE);	
}

void CAutomationPage::OnSaygreeting() 
{
	m_iGreetingType = AGT_SAY;
	m_rtfGreetingMesg.EnableWindow(TRUE);
	SetModified(TRUE);		
}

void CAutomationPage::OnWhispergreeting() 
{
	m_iGreetingType = AGT_WHISPER;
	m_rtfGreetingMesg.EnableWindow(TRUE);
	SetModified(TRUE);		
}


void CAutomationPage::OnAddMacro() 
{
	CString strName, strControlFull;

	int iMacroNum = m_keyCtl.GetCurSel();

	m_macroNameCtl.GetWindowText(strName);

	if (m_rtfMacro.m_prgdwFormatting)
	{
		m_rtfMacro.m_prgdwFormatting->RemoveAll();
		delete m_rtfMacro.m_prgdwFormatting;
	}
	m_rtfMacro.m_prgdwFormatting = PRGDWGetFormatting(&m_rtfMacro, m_rtfMacro.m_pFont, m_rtfMacro.m_crTextColor);

	m_rtfMacro.GetWindowText(m_rtfMacro.m_strText); 

	strControlFull = m_rtfMacro.m_strText;
	if (m_rtfMacro.m_prgdwFormatting)
	{
		char* szCtrlFull = SzControlFull((LPCTSTR) m_rtfMacro.m_strText, m_rtfMacro.m_prgdwFormatting);
		if (szCtrlFull)
		{
			strControlFull = CString(szCtrlFull);
			delete [] szCtrlFull;
		}
	}

	if (iMacroNum < 0 || strName.IsEmpty() || strControlFull.IsEmpty())
	{
		AfxMessageBox(IDS_EMPTYMACRO);
		return;
	}

	m_macros[iMacroNum].m_strName  = strName;
	m_macros[iMacroNum].m_strValue = strControlFull;
	m_macros[iMacroNum].m_bDefined = TRUE;
	GetDlgItem(IDC_DELETE_MACRO)->EnableWindow(TRUE);

	SetModified(TRUE);
}


void CAutomationPage::OnDeleteMacro() 
{
	int iMacroNum = m_keyCtl.GetCurSel();
	if (iMacroNum < 0)
		return;

	m_macros[iMacroNum].m_strName = "";
	m_macros[iMacroNum].m_strValue = "";
	m_macros[iMacroNum].m_bDefined = FALSE;
	m_macroNameCtl.SetWindowText("");
	m_rtfMacro.SetWindowText("");	// update the char format to use to default??

	SetModified(TRUE);
}


void CAutomationPage::OnSelchangeKey() 
{
	int iMacroNum = m_keyCtl.GetCurSel();
	if (iMacroNum < 0)
		return;

	m_macroNameCtl.SetWindowText(m_macros[iMacroNum].m_strName);
	
	if (m_rtfMacro.m_prgdwFormatting)
		m_rtfMacro.m_prgdwFormatting->RemoveAll();
	else
		m_rtfMacro.m_prgdwFormatting = new CDWordArray;

	char*	szControlFull = strdup((LPCTSTR) m_macros[iMacroNum].m_strValue);
	char*	szControlLess = SzControlLess(szControlFull, m_rtfMacro.m_prgdwFormatting);

	m_rtfMacro.SetWindowText("");
	m_rtfMacro.bSetTextColor(m_rtfMacro.m_crTextColor);	// Reset to default color

	m_rtfMacro.m_strText = CString(szControlLess);
	free(szControlFull);

	if (!m_rtfMacro.m_strText.IsEmpty())
		m_rtfMacro.bSetWindowFormattedText(m_rtfMacro.m_strText, m_rtfMacro.m_prgdwFormatting);

	GetDlgItem(IDC_DELETE_MACRO)->EnableWindow(m_macros[iMacroNum].m_bDefined);
}


void CAutomationPage::OnAutoIgnore() 
{
	m_bAutoIgnore = !m_bAutoIgnore;

	m_mesgCntCtl.EnableWindow(m_bAutoIgnore);
	m_intervalCtl.EnableWindow(m_bAutoIgnore);
	
	m_spinMesgCnt.EnableWindow(m_bAutoIgnore);
	m_spinInterval.EnableWindow(m_bAutoIgnore);

	SetModified(TRUE);
}

void CAutomationPage::OnMesgCountChg()
{
	CEdit*	pEdit = (CEdit*) GetDlgItem(IDC_MESGCOUNT);
	CString strMC;
	UINT	uMC;

	ASSERT(pEdit);

	pEdit->GetWindowText(strMC);
	uMC = atoi(strMC);

	if (uMC < 1 || uMC > 255)
	{
		ASSERT(!m_strMesgCnt.IsEmpty());
		pEdit->SetWindowText(m_strMesgCnt);
		pEdit->SetSel(0, -1);
	}
	else
		m_strMesgCnt = strMC;

	SetModified(TRUE);
}


void CAutomationPage::OnIntervalChg()
{
	CEdit*	pEdit = (CEdit*) GetDlgItem(IDC_INTERVAL);
	CString strI;
	UINT	uI;

	ASSERT(pEdit);

	pEdit->GetWindowText(strI);
	uI = atoi(strI);

	if (uI < 1 || uI > 255)
	{
		ASSERT(!m_strInterval.IsEmpty());
		pEdit->SetWindowText(m_strInterval);
		pEdit->SetSel(0, -1);
	}
	else
		m_strInterval = strI;

	SetModified(TRUE);
}


const DWORD CAutomationPage::m_nHelpIDs[] =
{
	IDC_NOGREETING,			IDH_AUTO_NONE,
	IDC_WHISPERGREETING,	IDH_AUTO_WHISPER,
	IDC_SAYGREETING,		IDH_AUTO_SAY,
	IDC_GREETINGMESG,		IDH_AUTO_TEXT,
	IDC_KEY,				IDH_MACRO_KEY,
	IDC_MACRONAME,			IDH_MACRO_NAME,
	IDC_MACRORICHEDIT,		IDH_MACRO_TEXT,
	IDC_ADD_MACRO,			IDH_MACRO_ADD,
	IDC_DELETE_MACRO,		IDH_MACRO_DELETE,
	IDC_AUTOIGNORE,			IDH_OPTIONS_AUTOMATION_FLOODERS,
	IDC_MESGCOUNT,			IDH_OPTIONS_AUTOMATION_MSGCOUNT,
	IDC_INTERVAL,			IDH_OPTIONS_AUTOMATION_INTERVAL,
	IDC_GROUP0,				IDH_GROUP_ID,
	IDC_GROUP1,				IDH_GROUP_ID,
	IDC_GROUP2,				IDH_GROUP_ID,
	0, 0
};


void CMacro::Invoke()
{
	if (m_bDefined) {
		CDWordArray	rgdwFormatting;

		char*	szControlFull = strdup((LPCTSTR) m_strValue);
		char*	szControlLess = SzControlLess(szControlFull, &rgdwFormatting);

		ChatSendText(CString(szControlLess), SM_SAY, TRUE, &rgdwFormatting);

		free(szControlFull);
		rgdwFormatting.RemoveAll();
	}
}
