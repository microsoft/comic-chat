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
#include "protsupp.h"
#include "whisprbx.h"
#include "ircproto.h"
#include <io.h>
#include <afxcmn.h>

#include "PropPage.h"
#include <winreg.h>

#include "dlylddll.h"
#include "ratings.h"
#include "txtfntdg.h"
#include "resource.h"
#include "mschat.h"

#include "sounddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern "C" void *GetMime();
extern "C" int	iBytesofChar(BYTE ch);

extern CChatApp theApp;
extern CPtrList g_docs;

// Function and helper callback to remove all duplicate elements from a path.

struct REMOVEDUPLICATES
{
	CStringArray arrDirs;
	BOOL bAnyDuplicates;
};

BOOL 
RemoveDuplicatesCallback(
const char *szDir, 
void *pvData) 
{
	REMOVEDUPLICATES * prmvdup = (REMOVEDUPLICATES *)pvData;
	for (int i = prmvdup->arrDirs.GetUpperBound (); i >= 0; i--)
	{
		if (!lstrcmpi (szDir, prmvdup->arrDirs[i]))
		{
			prmvdup->bAnyDuplicates = TRUE;
			return FALSE;
		}
	}
	prmvdup->arrDirs.Add (szDir);
	return FALSE;
}

void 
RemoveDuplicatesInPath(
CString &str)
{
	REMOVEDUPLICATES rmvdup;
	rmvdup.bAnyDuplicates = FALSE;
	extern BOOL bForPath(const char *szPath, BOOL soundFunc(const char *, void *), void *pvData);
	bForPath (str, RemoveDuplicatesCallback, &rmvdup);
	if (rmvdup.bAnyDuplicates)
	{
		int nSize = rmvdup.arrDirs.GetSize ();
		ASSERT(nSize > 0);
		LPSTR psz = str.GetBuffer (str.GetLength () + 1);
		for (int i = 0; i < nSize; i++)
		{
			lstrcpy (psz, rmvdup.arrDirs[i]);
			psz += rmvdup.arrDirs[i].GetLength ();
			if (i < nSize - 1)
				*(psz++) = ';';
		}
		*psz = '\0';
		str.ReleaseBuffer ();
	}
}

// Decodes a copyright string stored in an art file into one or two lines of text.
// The copyright string in the file can contain a copyright message, plus an optional
// author name. If the author name is specified, it should be delimited from the
// copyright message by \n (the actual text characters '\' and 'n' rather than a newline).
// This function will take such a string and produce one or two lines that can 
// be given directly to the edit control that shows authoring information.

static void 
DecodeCopyrightString(
LPCSTR pszCopyright,
CString* pstrMessageOut,
UINT nDefaultID)
{
	if (pszCopyright == NULL || *pszCopyright == '\0')
	{
		pstrMessageOut->LoadString (nDefaultID);
	}
	else 
	{
		CString strCopyright;
		LPCSTR pszAuthor = "";
		LPCSTR pszFound = OurMbsStr (pszCopyright, "\\n");
		if (pszFound != NULL)
		{
			strCopyright = CString (pszCopyright, (int)(pszFound - pszCopyright));
			pszAuthor = pszFound + 2;
		}
		else
		{
			strCopyright = pszCopyright;
		}
		ASSERT(pszAuthor != NULL);
		if (*pszAuthor)
		{
			CString strFormat;
			strFormat.LoadString (IDS_COPYRIGHT_PLUS_AUTHOR);
			pstrMessageOut->Format (strFormat, (LPCSTR)strCopyright, pszAuthor);
		}
		else
		{
			*pstrMessageOut = strCopyright;
		}
	}
}

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage property page

// IMPLEMENT_DYNCREATE(CSettingsPage, CCSPropertyPage)

CSettingsPage::CSettingsPage() : CCSPropertyPage(CSettingsPage::IDD)
{
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
	m_bVisible = ISTRUE(theApp.m_flags1 & F1_USERVISIBLE);
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
	DDV_MaxChars(pDX, m_strSoundPath, MAX_PATH);
	DDX_Check(pDX, IDC_ALLOWINVITES, m_bAllowInvites);
	DDX_Check(pDX, IDC_NETMEETING_AUTOSTART, m_bAcceptNMCalls);
	DDX_Check(pDX, IDC_PLAYSOUNDS, m_bPlaySounds);
	DDX_Check(pDX, IDC_SHOWIDENTITY, m_bShowIdentity);
	DDX_Check(pDX, IDC_ALLOW_FILETX, m_bAllowFileTX);
	DDX_Check(pDX, IDC_INVISIBLE, m_bVisible);
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
	ON_BN_CLICKED(IDC_INVISIBLE, OnInvisible)
	ON_BN_CLICKED(IDC_SOUNDPATH_BROWSE, OnBrowseForSoundDir)
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


void CSettingsPage::OnAcceptwhispers() 
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


void CSettingsPage::OnInvisible() 
{
	m_bVisible = !m_bVisible;
	SetModified(TRUE);
}


void CSettingsPage::OnChangeSoundpath() 
{
	SetModified(TRUE);
}


void CSettingsPage::OnBrowseForSoundDir()
{
	CString strDefaultFolder;
	CString strTitle, strDescription, strFileDescr;
	GetWindowsMediaDirectory (&strDefaultFolder);
	strTitle.LoadString (IDS_BROWSE_SOUNDFOLDER);
	strDescription.LoadString (IDS_BROWSE_SOUNDFOLDER_DESC);
	strFileDescr.LoadString (IDS_BROWSE_SOUNDFILE_DESC);
	CBrowseFolderDialogEx bfdlg (strTitle, strDescription, strFileDescr, GetSupportedSoundTypeList (), NULL, strDefaultFolder, this);
	if (theApp.DoModalDlg (&bfdlg) == IDOK)
	{
		CString str, strOrig;
		GetDlgItemText (IDC_SOUNDPATH, str);
		strOrig = str;
		// Append a semicolon if there isn't one.
		if (!str.IsEmpty () && str.ReverseFind (';') != str.GetLength () - 1)
			str += ';';
		str	+= bfdlg.GetSelectedFolder ();
		RemoveDuplicatesInPath (str);
		if (str.CompareNoCase (strOrig))
		{
			SetDlgItemText (IDC_SOUNDPATH, str);
			SetModified (TRUE);
		}
	}
}


void CSettingsPage::OnOK() 
{
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

	theApp.m_bAcceptWhispers = m_bAcceptWhispers;
	theApp.m_bAllowInvites = m_bAllowInvites;
	theApp.m_bAllowFileTX = m_bAllowFileTX;
	theApp.m_bShowArrivals = m_bShowArrivals;
	theApp.m_bPlaySounds = m_bPlaySounds;
	theApp.m_bAcceptNMCalls = m_bAcceptNMCalls;
	theApp.m_bShowIdentity = m_bShowIdentity;
	if (ISTRUE(theApp.m_flags1 & F1_USERVISIBLE) != m_bVisible)
		SetVisibility(m_bVisible);

	RemoveDuplicatesInPath (m_strSoundPath);
	theApp.m_soundPath = m_strSoundPath;

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


void CSettingsPage::OnShowarrivals() 
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
// CPersonalPage property page

// IMPLEMENT_DYNCREATE(CPersonalPage, CCSPropertyPage)
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


CPersonalPage::CPersonalPage(UINT id) : CCSPropertyPage(id /*CPersonalPage::IDD*/)
{
	//{{AFX_DATA_INIT(CPersonalPage)
	//}}AFX_DATA_INIT
	GetProfileString(m_rtfProfile);
	m_rtfProfile.m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_rtfProfile.DefineDefaultCharFormat();

	m_bOKing		= FALSE;

	m_strNickname	= GetMyName();
	m_strRealName	= GetMyRealName();
	m_strEmail		= GetMyEmail();
	m_strHomePage	= GetMyHomePage();

	m_editHomePage.m_strInvalid = " ";
	m_editEmail.m_strInvalid = " ";

#ifdef CB32SUPPORT
	m_bSave				= theApp.m_bPrompt;
	m_bAcceptWhispers	= theApp.m_bAcceptWhispers;
	m_bShowArrivals		= theApp.m_bShowArrivals;
#endif CB32SUPPORT
}


CPersonalPage::~CPersonalPage()
{
}

extern void DDV_NonBlankNick(CDataExchange*, CString&);

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
	DDV_MaxChars(pDX, m_strNickname, MAX_NICKINPUT);
	DDV_NonBlankNick(pDX, m_strNickname);
	DDX_Text(pDX, IDC_REALNAME, m_strRealName);
	DDV_MaxChars(pDX, m_strRealName, MAX_REALNAMEINPUT);
	DDX_Text(pDX, IDC_EMAIL, m_strEmail);
	DDV_MaxChars(pDX, m_strEmail, MAX_EMAILINPUT);
	DDX_Text(pDX, IDC_HOMEPAGE, m_strHomePage);
	DDV_MaxChars(pDX, m_strHomePage, MAX_HOMEPAGEINPUT);
	DDX_Control(pDX, IDC_SAVE, m_checkSave);
	// For CB32SUPPORT
	//DDX_Check(pDX, IDC_SAVE, m_bSave);
	//DDX_Check(pDX, IDC_ACCEPTWHISPERS, m_bAcceptWhispers);
	//DDX_Check(pDX, IDC_SHOWARRIVALS, m_bShowArrivals);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPersonalPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CPersonalPage)
	ON_EN_CHANGE(IDC_REALNAME, OnChangeEdit)
	ON_EN_CHANGE(IDC_NICKNAME, OnChangeEdit)
	ON_EN_CHANGE(IDC_HOMEPAGE, OnChangeEdit)
	ON_EN_CHANGE(IDC_EMAIL, OnChangeEdit)
	ON_NOTIFY(EN_MSGFILTER, IDC_PROFILE_RICHEDIT, OnFilterProfile)
	ON_CONTROL(EN_CHANGE, IDC_PROFILE_RICHEDIT, OnChangeProfile)
	// For CB32SUPPORT
	// ON_BN_CLICKED(IDC_SAVE, OnSave)
	// ON_BN_CLICKED(IDC_ACCEPTWHISPERS, OnAcceptwhispers)
	// ON_BN_CLICKED(IDC_SHOWARRIVALS, OnShowarrivals)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


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

#ifdef CB32SUPPORT
// manage the save on close variable
void CPersonalPage::OnSave() 
{
	m_bSave = !m_bSave;
	SetModified(TRUE);
}


void CPersonalPage::OnShowarrivals() 
{
	m_bShowArrivals = !m_bShowArrivals;
	SetModified(TRUE);
}


void CPersonalPage::OnAcceptwhispers() 
{
	m_bAcceptWhispers = !m_bAcceptWhispers;
	SetModified(TRUE);
}
#endif CB32SUPPORT


void CPersonalPage::OnChangeProfile()
{
	if (!m_bOKing)
		SetModified(TRUE);
	else
		m_bOKing = FALSE;
}


void CPersonalPage::OnChangeEdit() 
{
	SetModified(TRUE);
}


void CPersonalPage::OnFilterProfile(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct);

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfProfile.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


void CPersonalPage::OnOK() 
{
	m_bOKing = TRUE;

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

#ifdef CB32SUPPORT
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
#endif CB32SUPPORT

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

	m_editNickname.SetFilter (FILTEREDIT_NOCHARS, ",");

	// Need to add the EN_SELCHANGE notification to the dwEventMask of the rich text control
	DWORD dwEventMask = (DWORD) m_rtfProfile.GetEventMask();
	m_rtfProfile.SetEventMask(dwEventMask | ENM_CHANGE | ENM_MOUSEEVENTS);

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
	
	cui.m_pvPersonalPage = NULL;

	return CPropertyPage::OnKillActive();
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

	// Figure out rectangle for avatar preview.

   	CRect rAvatarPreview;
	CWnd* pWnd = GetDlgItem (IDC_CHARACTER_PREVIEW);
	if (pWnd) {
		pWnd->GetWindowRect (rAvatarPreview);
		ScreenToClient (rAvatarPreview);
	}
	else {
		rAvatarPreview.SetRect (cxPosAvatarPage, cyPosAvatarPage, (cxPosAvatarPage + cxAvatarPage),
   	                     (cyPosAvatarPage + cyAvatarPage));
		MapDialogRect(rAvatarPreview);
	}

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
		_findclose (hFind);
	}

	CAvatarX *av = GetAvatar2(m_strSel);
	if (!av) av = GetAvatar3("X");  // choose one sequentially
	m_strSel = av->OriginalName();
	int index = m_avList.FindStringExact(0, m_strSel);
	if (index != LB_ERR) m_avList.SetCurSel(index);

	// create body cam pane. Double clicking on the body cam avatar image normally
	// brings up this dialog, so disable that.
	m_wndCharSelBodyCam.EnableDoubleClick (FALSE);
	VERIFY( m_wndCharSelBodyCam.Create( NULL, NULL, WS_CHILD | WS_VISIBLE | WS_TABSTOP, rAvatarPreview, this, 5 ) );
	m_wndCharSelBodyCam.RecalcRetainedBMP();
	m_wndCharSelBodyCam.m_forcedDelete = FALSE;
	cui.m_pvCharSelBodyCamWnd = &m_wndCharSelBodyCam;
	m_wndCharSelBodyCam.m_avatar = av;

	CString strCopyright;
	DecodeCopyrightString (av ? av->Copyright () : NULL, &strCopyright, IDS_AUTHOR_NONE);
	SetDlgItemText (IDC_AVATAR_COPYRIGHT, strCopyright);

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
			if (g_puiSelf)  // i.e., we're in channel, so this is safe
				AddAndExecute(new ChangeAvatarEntry(g_puiSelf, modelAv->m_name, modelAv->Url ()));
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
				CString strCopyright;
				DecodeCopyrightString (newAv ? newAv->Copyright () : NULL, &strCopyright, IDS_AUTHOR_NONE);
				SetDlgItemText (IDC_AVATAR_COPYRIGHT, strCopyright);
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

	// Backdrop files can be of more than one filetype, as indicated by the
	// pszBackdropTypes array. Go through the list of types, enumerating files
	// of each type, and add them to the listbox. The file type
	// is stored in the listbox extra data for the entry.

	int nEntry;
	for (int iType = 0; iType < NUMBACKDROPTYPES; iType++) {
		strPattern = theApp.GetBackDropDir();
		strPattern += BACKDROPTYPE_SEARCHMASK(iType);
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
					nEntry = m_listBack.AddString(szFName);
					if (nEntry != LB_ERR) {
						m_listBack.SetItemData (nEntry, iType);
					}
				}

			} while( _findnext( hFind, &fd ) != -1 );
			_findclose (hFind);
		}
	}

	// set selection to current background. The background name may have both a name
	// and a file type - we need to break this out.

	int nSelType;
	CString strSelName = m_strCurrentSel;
	int nNameLength = m_strCurrentSel.GetLength ();
	for (nSelType = 0; nSelType < NUMBACKDROPTYPES; nSelType++) {
		int nExtLength = lstrlen (BACKDROPTYPE_FILEEXT(nSelType));
		if (nNameLength > nExtLength && !lstrcmpi (((LPCSTR)m_strCurrentSel) + 
				nNameLength - nExtLength, BACKDROPTYPE_FILEEXT(nSelType))) {
			strSelName = strSelName.Left (nNameLength - nExtLength);
			break;
		}
	}

	// If nSelType == NUMBACKDROPTYPES here, then we can just select the first one 
	// that matches by name.

	if (!strSelName.IsEmpty ()) {
		int index = -1;
		int nPrevIndex = -1;
		int nType;
		while ((index = m_listBack.FindStringExact(nPrevIndex, strSelName)) > nPrevIndex)
		{
			nPrevIndex = index;
			nType = (int)m_listBack.GetItemData (index);
			if (nSelType == NUMBACKDROPTYPES || nType == nSelType) {
				m_listBack.SetCurSel (index);
				break;
			}
		}
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
	// Get the filename. The filetype is stored in the listbox extra data for the
	// entry.

	CString strPath, strFile;
	int index = m_listBack.GetCurSel();
	if (index == LB_ERR) {
		m_dib.Clear ();
		InvalidateRect (m_rect);
		return;
	}
	m_listBack.GetText(index,m_strCurrentSel);
	int nCurrentSelType = (int)m_listBack.GetItemData (index);
	ASSERT (nCurrentSelType < NUMBACKDROPTYPES);
	m_strCurrentSel += BACKDROPTYPE_FILEEXT(nCurrentSelType);
	strPath = theApp.GetBackDropDir();

	strFile.Format ("%s\\%s", (LPCSTR)strPath, (LPCSTR)m_strCurrentSel);

	// Clear the old stuff in the DIB.
	m_dib.Clear ();
	CAvatarFileStream stream (strFile);
	CChatBackdrop * pBackdrop = CChatBackdrop::LoadBackdrop (&stream);
	// Set the copyright message
	CString strCopyright;
	DecodeCopyrightString (pBackdrop ? pBackdrop->Copyright () : NULL, &strCopyright, IDS_AUTHOR_NONE_BK);
	SetDlgItemText (IDC_BACKGROUND_COPYRIGHT, strCopyright);
	if (pBackdrop != NULL && pBackdrop->m_pDIB != NULL) {
		// Transfer the dib information.
		m_dib.TransferFrom (pBackdrop->m_pDIB);
		delete pBackdrop;
	}
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

	if (m_dib.GetBitmapInfoAddress ())
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
	void SetBackDrop(const char *, const char *);
	int status = CX_DISCONNECTED;
	if (currentRoom) status = currentRoom->GetConnectionStatus();
	
	if (status == CX_INCHANNEL || status == CX_DISCONNECTED)
		AddAndExecute(new ChangeBackDropEntry((const char *) m_strCurrentSel, NULL));
	else 
		SetBackDrop(m_strCurrentSel, NULL);

	CCSPropertyPage::OnOK();
}


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
	m_bCfHLInitialized = theApp.m_bCfHLInitialized;
	
	if (m_bCfInitialized)
		for (int i = 0; i < NREGULARFONTS; i++)
			m_cfArray[i] = theApp.m_cfArray[i];

	if (m_bCfHLInitialized)
		for (int i = NREGULARFONTS; i < NFONTS; i++)
			m_cfArray[i] = theApp.m_cfArray[i];

	m_bCfChangesMade = FALSE;
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

	fDialog.m_bSetFromCf = m_bCfChangesMade;
	memcpy (fDialog.m_cfArray, m_cfArray, NFONTS);

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

	if (theApp.DoModalDlg(&fDialog) == IDOK)
	{
		//m_bCfInitialized = fDialog.m_bCfInitialized;
		//m_bCfHLInitialized = fDialog.m_bCfHLInitialized;
		//if (m_bCfInitialized)
		//	for (int i = 0; i < NREGULARFONTS; i++)
		//		m_cfArray[i] = fDialog.m_cfArray[i];
		//if (m_bCfHLInitialized)
		//	for (i = NREGULARFONTS; i < NFONTS; i++)
		//		m_cfArray[i] = fDialog.m_cfArray[i];

		m_bCfInitialized = m_bCfHLInitialized = TRUE;
		for (int i = 0; i < NFONTS; i++)
			m_cfArray[i] = fDialog.m_cfArray[i];

		m_bCfChangesMade = TRUE;
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

	if (theApp.DoModalDlg(&fDialog) == IDOK)
	{
		theApp.m_bCfInitialized = theApp.m_bCfHLInitialized = TRUE;
		for (int i = 0; i < NFONTS; i++)
			theApp.m_cfArray[i] = fDialog.m_cfArray[i];
		theApp.m_textColor = (theApp.m_cfArray[2].dwMask & CFM_COLOR) ? theApp.m_cfArray[2].crTextColor : GetSysColor(COLOR_WINDOWTEXT);
	}
	
	GetChatDoc()->SetFocusToSayWnd();   // put focus in saywnd (important for IME support)
	
	// reinitialize font choices in main text view (must be done after values above set)
	void InitializeTextCores(BOOL, BOOL = FALSE);

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
	BOOL bRestoreOld = !m_bCfInitialized;

	theApp.m_textSpacing = m_spacing;
	theApp.m_bCfInitialized = m_bCfInitialized;
	theApp.m_bCfHLInitialized = m_bCfHLInitialized;

	if (m_bCfInitialized)
	{
		for (int i = 0; i < NREGULARFONTS; i++) 
			theApp.m_cfArray[i] = m_cfArray[i];
		theApp.m_textColor = (theApp.m_cfArray[2].dwMask & CFM_COLOR) ? theApp.m_cfArray[2].crTextColor : GetSysColor(COLOR_WINDOWTEXT);
	}

	if (m_bCfHLInitialized)
		for (int i = NREGULARFONTS; i < NFONTS; i++) 
			theApp.m_cfArray[i] = m_cfArray[i];

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
	void InitializeTextCores(BOOL, BOOL = FALSE);

	InitializeTextCores(bRestoreOld, TRUE);
	InitializeWhisperCores(bRestoreOld);

	CCSPropertyPage::OnOK();
}


BOOL CTextFontPage::OnSetActive() 
{
	CheckRadioButton(IDC_LINESPACE_NONE, IDC_LINESPACE_ALL,
				     IDC_LINESPACE_NONE+m_spacing);
	
	return CCSPropertyPage::OnSetActive();
}


void CTextFontPage::OnResetTextfonts() 
{
	m_bCfInitialized = m_bCfHLInitialized = FALSE;
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
	m_bAutoDownloadChars = theApp.m_bAutoDownloadAvatars;
	m_bAutoDownloadBackdrops = theApp.m_bAutoDownloadBackdrops;
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
	DDX_Check(pDX, IDC_AUTODOWNLOAD_CHARS, m_bAutoDownloadChars);
	DDX_Check(pDX, IDC_AUTODOWNLOAD_BACKDROPS, m_bAutoDownloadBackdrops);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CComicsPropPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CComicsPropPage)
	ON_BN_CLICKED(IDC_SHOWCOMICRTF, OnShowComicRTF)
	ON_BN_CLICKED(ID_SETFONT, OnSetfont)
	ON_CBN_SELCHANGE(IDC_PANELS, OnSelchangePanels)
	ON_BN_CLICKED(ID_RESET_TEXTFONTS, OnResetfont)
	ON_BN_CLICKED(IDC_AUTODOWNLOAD_CHARS, OnAutoDownloadChars)
	ON_BN_CLICKED(IDC_AUTODOWNLOAD_BACKDROPS, OnAutoDownloadBackdrops)
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
	CWin4FontDialog	fDialog;
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

	if (theApp.DoModalDlg(&fDialog) == IDOK) {
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

	// Update the auto download flags.
	theApp.m_bAutoDownloadAvatars = m_bAutoDownloadChars;
	theApp.m_bAutoDownloadBackdrops = m_bAutoDownloadBackdrops;
	// Should we start downloading ones we don't have at this time??

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


void CComicsPropPage::OnAutoDownloadChars()
{
	m_bAutoDownloadChars = !m_bAutoDownloadChars;
	SetModified(TRUE);
}


void CComicsPropPage::OnAutoDownloadBackdrops()
{
	m_bAutoDownloadBackdrops = !m_bAutoDownloadBackdrops;
	SetModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CServersPage property page

//IMPLEMENT_DYNCREATE(CServersPage, CCSPropertyPage)

// Special value used to indicate that the variable doesn't hold a valid selection state yet.
// This is distinct from the variable holding CB_ERR, indicating a state of no selection.
#define CB_NOCURRSEL ((UINT)-2L)

CString CServersPage::sm_strUnassociatedGroup;

// Constructor

CServersPage::CServersPage() : CCSPropertyPage(CServersPage::IDD)
{
	//{{AFX_DATA_INIT(CServersPage)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pSvcList = &theApp.m_listChatServices;
	m_ui.SetServiceList (m_pSvcList);
	sm_strUnassociatedGroup.LoadString (IDS_UNASSOCIATED_GROUP);
	m_nPort = 6667;
	m_nSecurity = 0;
	m_bRememberPassword = FALSE;
	m_nCurrSelServer = m_nCurrSelGroup = CB_NOCURRSEL;
	m_bServerPropChange = FALSE;
	m_bSetActiveNeverCalled = TRUE;
	m_bInDDX = FALSE;

	// First time in the dialog, choose the first group.
	CChatServerGroup* pGroup = NULL;
	BOOL bUnassociatedGroup;
	if (m_pSvcList->EnumGroups (pGroup, bUnassociatedGroup))
	{
		m_strCurrentGroup = bUnassociatedGroup ? sm_strUnassociatedGroup : pGroup->m_pszName;
	}
}

CServersPage::~CServersPage()
{
}

void CServersPage::DoDataExchange(CDataExchange* pDX)
{
	m_bInDDX = TRUE;
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CServersPage)
	DDX_Control (pDX, IDC_SERVERLIST, m_comboServers);
	//}}AFX_DATA_MAP

	if (!pDX->m_bSaveAndValidate)
	{
		// Set the group combo, and make the dialog switch to it.

		int nSel = m_comboGroups.FindStringExact (-1, m_strCurrentGroup);
		m_comboGroups.SetCurSel (nSel);
		if (nSel == CB_ERR)
			m_comboGroups.SetWindowText (m_strCurrentGroup);
		m_nCurrSelGroup = nSel;
		SwitchGroup ();

		// Set the server combo, and make the dialog switch to it.

		nSel = m_comboServers.FindStringExact (-1, m_strCurrentServer);
		m_comboServers.SetCurSel (nSel);
		if (nSel == CB_ERR)
			m_comboServers.SetWindowText (m_strCurrentServer);
		m_nCurrSelServer = nSel;

		// Show the data for the current server
		SetServerPropsToPage (pDX);

		UpdateUIState ();
	}
	m_bInDDX = FALSE;
}


BEGIN_MESSAGE_MAP(CServersPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CServersPage)
	ON_CBN_SELCHANGE(IDC_SRVGROUP_COMBO, OnSelChangeServerGroup)
	ON_CBN_SELCHANGE(IDC_SERVERLIST, OnSelChangeServer)
	ON_CBN_EDITCHANGE(IDC_SRVGROUP_COMBO, OnEditChangeServerGroup)
	ON_CBN_EDITCHANGE(IDC_SERVERLIST, OnEditChangeServer)
	ON_BN_CLICKED(IDC_SERVER_ADD, OnAddServer)
	ON_BN_CLICKED(IDC_SERVER_REMOVE, OnRemoveServer)
	ON_BN_CLICKED(IDC_SRVGROUP_ADD, OnAddServerGroup)
	ON_BN_CLICKED(IDC_SRVGROUP_REMOVE, OnRemoveServerGroup)
	ON_BN_CLICKED(IDC_SRVCONNECT_NOPWD, OnChangeAuthenticationType)
	ON_BN_CLICKED(IDC_SRVCONNECT_PWD, OnChangeAuthenticationType)
	//ON_BN_CLICKED(IDC_SRVCONNECT_SRVPKG, OnChangeAuthenticationType)
	//ON_BN_CLICKED(IDC_SRVCONNECT_PKG, OnChangeAuthenticationType)
	ON_EN_KILLFOCUS(IDC_SERVER_PORT, OnKillFocusPort)
	ON_EN_CHANGE(IDC_SERVER_PORT, OnChangeServerProp)
	ON_EN_CHANGE(IDC_SRV_PASSWORD, OnChangeServerProp)
	ON_EN_CHANGE(IDC_SRV_USERNAME, OnChangeServerProp)
	//ON_EN_CHANGE(IDC_SRV_PACKAGES, OnChangeServerProp)
	ON_BN_CLICKED(IDC_SRV_REMEMBER_PWD, OnChangeServerProp)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

const DWORD CServersPage::m_nHelpIDs[] =
{
	0, 0
};

/////////////////////////////////////////////////////////////////////////////
// CServersPage message handlers

// Reasonable minimum/maximum values for IRC server port numbers.
#define IRCPORT_MIN 	6000
#define IRCPORT_MAX		7000

// Called when the page is activated. Refills the server group combobox,
// and allows default handling to select all the right settings again.

BOOL CServersPage::OnSetActive() 
{
	m_bSetActiveNeverCalled = FALSE;

	ReloadSettings ();

	((CSpinButtonCtrl*)GetDlgItem (IDC_SERVER_PORT_NW))->SetRange (IRCPORT_MIN, IRCPORT_MAX);
	((CEdit*)GetDlgItem (IDC_SRV_USERNAME))->SetLimitText (30);
	((CEdit*)GetDlgItem (IDC_SRV_PASSWORD))->SetLimitText (30);
	((CComboBox*)GetDlgItem (IDC_SRVGROUP_COMBO))->LimitText (40);
	((CComboBox*)GetDlgItem (IDC_SERVERLIST))->LimitText (64);

	return CCSPropertyPage::OnSetActive();
}

void CServersPage::OnOK() 
{
	if (AcceptServerSettings ())
	{
		m_ui.Apply ();
		ReloadSettings ();
		UpdateData (FALSE);
		CCSPropertyPage::OnOK();
	}
}

// Reloads values in combo boxes, etc.

void 
CServersPage::ReloadSettings()
{
	m_comboGroups.ResetContent ();
	m_comboServers.ResetContent ();

	// Fill in list of server groups (networks).
	CComboBox* pCombo = (CComboBox*)GetDlgItem (IDC_SRVGROUP_COMBO);
	int i;
	BOOL bUnassociatedCreated = FALSE;
	BOOL bUnassociatedGroup;
	POSITION pos = NULL;
	HCHATSRVGROUP hGroup;
	while ((hGroup = m_ui.EnumGroups (pos, bUnassociatedGroup)) != NULL)
	{
		if (bUnassociatedGroup)
		{
			i = pCombo->AddString (sm_strUnassociatedGroup);
			bUnassociatedCreated = TRUE;
		}
		else
		{
			i = pCombo->AddString (m_ui.GetGroupName (hGroup));
		}
		pCombo->SetItemDataPtr (i, hGroup);
	}

	if (!bUnassociatedCreated)
	{
		// Create the <none> group if it doesn't exist.
		hGroup = m_ui.AddGroup (UNASSOCIATED_GROUP);
		if (hGroup != NULL)
		{
			i = pCombo->AddString (sm_strUnassociatedGroup);
			pCombo->SetItemDataPtr (i, hGroup);
		}
	}
}

// Called when the page is deactivated. Clears combo boxes, so that they
// may be properly refilled when the page is reactivated.

BOOL CServersPage::OnKillActive() 
{
	AcceptServerSettings ();
	// disable ApplyNow button
	return CCSPropertyPage::OnKillActive();
}

// Update UI states of controls based on current settings. The dwUpdateHint parameter
// allows updating selective groups of settings.

void 
CServersPage::UpdateUIState(
DWORD dwUpdateHint)
{
	if (dwUpdateHint == 0)
		return;

	int nSelGroup, nSelServer;
	BOOL bAllowAdd, bAllowRemove;
	HCHATSRVGROUP hCurrentGroup;

	ASSERT (m_nCurrSelGroup != CB_NOCURRSEL);
	ASSERT (m_nCurrSelServer != CB_NOCURRSEL);
	nSelGroup = m_nCurrSelGroup;
	hCurrentGroup = GetCurrGroup ();
	nSelServer = m_nCurrSelServer;

	if ((dwUpdateHint & updateelemEnableServerUI) != 0)
	{
		EnableControlRange (IDC_SERVERS_LABEL, IDC_SERVERLIST, nSelGroup != CB_ERR);
	}

	if ((dwUpdateHint & updateelemAddRemoveGroup) != 0)
	{
		if (nSelGroup != CB_ERR)
		{
			bAllowAdd = FALSE;
			bAllowRemove = lstrcmpi (m_strCurrentGroup, sm_strUnassociatedGroup);
		}
		else 
		{
			bAllowAdd = !m_strCurrentGroup.IsEmpty () && 
						 m_strCurrentGroup[0] != '.' && // . is a special keychar
						m_strCurrentGroup[0] != '{' && 	// So is {
						m_strCurrentGroup.FindOneOf ("/\\") == -1 &&
						lstrcmpi (m_strCurrentGroup, sm_strUnassociatedGroup) &&
						m_comboGroups.FindStringExact (-1, m_strCurrentGroup) == CB_ERR;
			bAllowRemove = FALSE;
		}
		EnableButton (IDC_SRVGROUP_ADD, bAllowAdd);
		EnableButton (IDC_SRVGROUP_REMOVE, bAllowRemove);
	}

	if ((dwUpdateHint & updateelemAddRemoveServer) != 0)
	{
		if (nSelGroup == CB_ERR)
		{
			bAllowAdd = bAllowRemove = FALSE;
		}
		else
		{
			if (nSelServer == CB_ERR)
			{
				bAllowAdd = !m_strCurrentServer.IsEmpty () &&
							m_strCurrentServer[0] != '.' &&
							m_strCurrentServer.FindOneOf ("/\\") == -1 &&
							m_comboServers.FindStringExact (-1, m_strCurrentServer) == CB_ERR;
				bAllowRemove = FALSE;
			}
			else
			{
				bAllowAdd = FALSE;
				bAllowRemove = TRUE;
			}
		}
		EnableButton (IDC_SERVER_ADD, bAllowAdd);
		EnableButton (IDC_SERVER_REMOVE, bAllowRemove);
	}

	if ((dwUpdateHint & updateelemServerProps) != 0)
	{
		if (nSelServer == CB_ERR)
		{
			//EnableControlRange (IDC_SERVER_SECURITY, IDC_SRV_PACKAGES, FALSE);
			EnableControlRange (IDC_SERVER_SECURITY, IDC_SRV_REMEMBER_PWD, FALSE);
		}
		else
		{
			EnableControlRange (IDC_SERVER_SECURITY, IDC_SRVCONNECT_PKG, TRUE);
			EnableControlRange (IDC_SRV_USERNAME_LABEL, IDC_SRV_REMEMBER_PWD, m_nSecurity == 1);
			//EnableControl (IDC_SRV_PACKAGES, m_nSecurity == 3);
		}
	}
}

// Called when user selects another entry in the Server Group combobox.

void
CServersPage::OnSelChangeServerGroup()
{
	AcceptServerSettings ();
	ChangeServerGroup (FALSE);
}

// Called when user selects another entry in the Server combobox.

void
CServersPage::OnSelChangeServer()
{
	AcceptServerSettings ();
	ChangeServer (FALSE);
}

// Called when the user types text into the Server Group combobox.

void
CServersPage::OnEditChangeServerGroup()
{
	AcceptServerSettings ();
	ChangeServerGroup (TRUE);
}

// Called when the user types text into the Server combobox.

void
CServersPage::OnEditChangeServer()
{
	AcceptServerSettings ();
	ChangeServer (TRUE);
}

// Common handler for changes made to the Server Group combobox. 

void
CServersPage::ChangeServerGroup(
BOOL bKeystroke,
BOOL bForce)
{
	// When the user types the first keystroke, we get a CBN_EDITCHANGE before
	// the combobox has set the selection to CB_ERR. So, disregard the current
	// selection when called for keystrokes.
	int nSel = bKeystroke ? CB_ERR : m_comboGroups.GetCurSel ();
	if (nSel == CB_ERR)
	{
		// Keyboard completion code - if you type the text of a combo member,
		// it is automatically selected.
		m_comboGroups.GetWindowText (m_strCurrentGroup);
		m_strCurrentGroup.TrimLeft ();
		m_strCurrentGroup.TrimRight ();
		int n = m_comboGroups.FindStringExact (-1, m_strCurrentGroup);
		if (n != CB_ERR)
		{
			// Keyboard completion code - if you type the text of a combo member,
			// it is automatically selected.
			DWORD dwSel = m_comboGroups.GetEditSel ();
			nSel = n;
			m_comboGroups.SetCurSel (n);
			m_comboGroups.SetEditSel ((int)LOWORD(dwSel), (int)HIWORD(dwSel));
			// Combo box keyboard handler automatically returns the selection to -1,
			// and thus doesn't show the selection. Avoid this by posting message again.
			m_comboGroups.PostMessage (CB_SETCURSEL, n, 0);
			m_comboGroups.PostMessage (CB_SETEDITSEL, 0, (LPARAM)dwSel);
		}
	}
	else
		m_comboGroups.GetLBText (nSel, m_strCurrentGroup);
	if (bForce || nSel != m_nCurrSelGroup)
	{
		m_nCurrSelGroup = nSel;
		m_strCurrentServer = "";
		SwitchGroup ();
		SwitchServer ();
		UpdateUIState (updateChangeCurrSrvGroup);
	}
	else
	{
		// Minimal update when typing a name
		UpdateUIState (updateNewServerGroupText);
	}
}

// Common handler for changes made to the Server combobox. 

void
CServersPage::ChangeServer(
BOOL bKeystroke,
BOOL bForce)
{
	// When the user types the first keystroke, we get a CBN_EDITCHANGE before
	// the combobox has set the selection to CB_ERR. So, disregard the current
	// selection when called for keystrokes.
	int nSel = bKeystroke ? CB_ERR : m_comboServers.GetCurSel ();
	if (nSel == CB_ERR)
	{
		// Keyboard completion code - if you type the text of a combo member,
		// it is automatically selected.
		m_comboServers.GetWindowText (m_strCurrentServer);
		m_strCurrentServer.TrimLeft ();
		m_strCurrentServer.TrimRight ();
		int n = m_comboServers.FindStringExact (-1, m_strCurrentServer);
		if (n != CB_ERR)
		{
			DWORD dwSel = m_comboServers.GetEditSel ();
			nSel = n;
			m_comboServers.SetCurSel (n);
			m_comboServers.SetEditSel ((int)LOWORD(dwSel), (int)HIWORD(dwSel));
			// Combo box keyboard handler automatically returns the selection to -1,
			// and thus doesn't show the selection. Avoid this by posting message again.
			m_comboServers.PostMessage (CB_SETCURSEL, n, 0);
			m_comboServers.PostMessage (CB_SETEDITSEL, 0, (LPARAM)dwSel);
		}
	}
	else
		m_comboServers.GetLBText (nSel, m_strCurrentServer);
	if (bForce || nSel != m_nCurrSelServer)
	{
		m_nCurrSelServer = nSel;
		SwitchServer ();
		UpdateUIState (updateChangeCurrServer);
	}
	else
	{
		// Minimal update when typing a name
		UpdateUIState (updateNewServerText);
	}
}

// Called to make the dialog switch to the currently selected group.

void 
CServersPage::SwitchGroup()
{
	ASSERT (m_nCurrSelGroup != CB_NOCURRSEL);
	int nSel = m_nCurrSelGroup;
	HCHATSRVGROUP hGroup = GetCurrGroup ();
	m_comboServers.ResetContent ();
	if (hGroup != NULL)
	{
		HCHATSERVER hServer;
		POSITION pos = NULL;
		int i;
		while ((hServer = m_ui.EnumServersInGroup (hGroup, pos)) != NULL)
		{
			i = m_comboServers.AddString (m_ui.GetServerName (hServer));
			m_comboServers.SetItemDataPtr (i, hServer);
		}
	}
	m_nCurrSelServer = CB_ERR;
}

// Called to make the dialog switch to the currently selected server. This is
// the function that gets the server's properties and copies them locally where
// they can be modified.

void 
CServersPage::SwitchServer()
{
	ASSERT (m_nCurrSelServer != CB_NOCURRSEL);
	int nSel = m_nCurrSelServer;

	if (nSel != CB_ERR)
	{
		HCHATSERVER hServer = GetCurrServer ();
		
		CChatServiceUI::ServerProps data;
		m_ui.GetServerProps (hServer, data);
		m_nPort = data.m_nPort;
		m_nSecurity = data.m_nAuthenticationType;
		m_strUserName = data.m_strUserName;
		m_strPassword = data.m_strPassword;
		m_bRememberPassword = data.m_bRememberPassword;
		m_strSecurityPackages = data.m_strSecurityPackages;
		if (m_nSecurity > 1)
			m_nSecurity = 0;
	}
	else
	{
		m_nPort = 6667;
		m_nSecurity = 0;
		m_strUserName.Empty ();
		m_strPassword.Empty ();
		m_bRememberPassword = FALSE;
		m_strSecurityPackages.Empty ();
	}

	CDataExchange dx (this, FALSE);
	SetServerPropsToPage (&dx);
}

// Sets server data into the controls on the page.

void
CServersPage::SetServerPropsToPage(
CDataExchange* pDX)
{
	ASSERT(!pDX->m_bSaveAndValidate);

	DDX_Radio(pDX, IDC_SRVCONNECT_NOPWD, m_nSecurity);
	CString strUserName, strPassword, strPackages;
	BOOL bRememberPassword;
	if (m_nSecurity == 1)
	{
		strUserName = m_strUserName;
		strPassword = m_strPassword;
		bRememberPassword = m_bRememberPassword;
	}
	else if (m_nSecurity == 3)
	{
		strPackages = m_strSecurityPackages;
		bRememberPassword = FALSE;
	}
	BOOL bPrevDDX = m_bInDDX;
	m_bInDDX = TRUE;
	DDX_Text(pDX, IDC_SRV_USERNAME, strUserName);
	DDX_Text(pDX, IDC_SRV_PASSWORD, strPassword);
	//DDX_Text(pDX, IDC_SRV_PACKAGES, strPackages);
	DDX_Text(pDX, IDC_SERVER_PORT, m_nPort);
	DDX_Check(pDX, IDC_SRV_REMEMBER_PWD, bRememberPassword);
	m_bInDDX = bPrevDDX;
}

// Gets server data from the controls on the page.

void
CServersPage::GetServerPropsFromPage(
CDataExchange* pDX)
{
	ASSERT(pDX->m_bSaveAndValidate);

	DDX_Radio(pDX, IDC_SRVCONNECT_NOPWD, m_nSecurity);
	if (m_nSecurity == 1)
	{
		DDX_Text(pDX, IDC_SRV_USERNAME, m_strUserName);
		DDX_Text(pDX, IDC_SRV_PASSWORD, m_strPassword);
		DDX_Check(pDX, IDC_SRV_REMEMBER_PWD, m_bRememberPassword);
	}
	else
	{
		m_strUserName.Empty ();
		m_strPassword.Empty ();
		m_bRememberPassword = FALSE;
	}
	
	if (m_nSecurity == 3)
	{
		//DDX_Text(pDX, IDC_SRV_PACKAGES, m_strSecurityPackages);
	}
	else
	{
		m_strSecurityPackages.Empty ();
	}

	DDX_Text(pDX, IDC_SERVER_PORT, m_nPort);
}

// Accepts the currently entered settings.

BOOL
CServersPage::AcceptServerSettings()
{
	if (!m_bServerPropChange)
		return TRUE;

	BOOL bError = FALSE;

	HCHATSERVER hServer = GetCurrServer ();
	HCHATSRVGROUP hGroup = GetCurrGroup ();
	if (hServer != NULL && hGroup != NULL)
	{
		if (!ValidatePortNumber ())
			bError = TRUE;

		CDataExchange dx (this, TRUE);
		GetServerPropsFromPage (&dx);

		CChatServiceUI::ServerProps data;
		data.m_nPort = m_nPort;
		data.m_nAuthenticationType = m_nSecurity;
		data.m_strUserName = m_strUserName;
		data.m_strPassword = m_strPassword;
		data.m_bRememberPassword = m_bRememberPassword;
		data.m_strSecurityPackages = m_strSecurityPackages;
		m_ui.SetServerProps (hGroup, hServer, data);
		m_bServerPropChange = FALSE;
	}
	return !bError;
}

void
CServersPage::OnAddServer()
{
	// The user is allowed to type a server name with a : in it, we 
	// automatically make that the port. This allows a user to create two entries for
	// a single server, say "irc.msn.com" and "irc.msn.com:6668". 

	ASSERT(m_nCurrSelGroup != CB_NOCURRSEL);
	HCHATSRVGROUP hGroup = GetCurrGroup ();
	if (hGroup != NULL)
	{
		CString strServer;
		int nPort;
		TranslateServerNameToServerAndPort (m_strCurrentServer, &strServer, &nPort);
		HCHATSERVER hServer = m_ui.AddServer (hGroup, m_strCurrentServer, nPort);
		if (hServer != NULL)
		{
			int nSel = m_comboServers.AddString (m_strCurrentServer);
			m_comboServers.SetItemDataPtr (nSel, hServer);
			m_comboServers.SetCurSel (nSel);
			ChangeServer (FALSE, TRUE);
			UpdateUIState (updateAddServer);
			m_comboServers.SetFocus ();
			//NextDlgCtrl ();
			SetModified(TRUE);
		}
	}
}

void
CServersPage::OnRemoveServer()
{
	ASSERT(m_nCurrSelGroup != CB_NOCURRSEL);
	ASSERT(m_nCurrSelServer != CB_NOCURRSEL);
	HCHATSRVGROUP hGroup = GetCurrGroup ();
	HCHATSERVER hServer = GetCurrServer ();
	if (hGroup != NULL && hServer != NULL)
	{
		if (m_ui.RemoveServer (hGroup, hServer))
		{
			m_comboServers.DeleteString (m_nCurrSelServer);
			m_comboServers.SetCurSel (m_nCurrSelServer < m_comboServers.GetCount () ? m_nCurrSelServer : m_nCurrSelServer - 1);
			ChangeServer (FALSE, TRUE);
		    UpdateUIState (updateRemoveServer);
			m_comboServers.SetFocus ();
			//NextDlgCtrl ();
			SetModified(TRUE);
		}
	}
}

void
CServersPage::OnAddServerGroup()
{
	ASSERT(m_nCurrSelGroup == CB_ERR);
	HCHATSRVGROUP hGroup = m_ui.AddGroup (m_strCurrentGroup);
	if (hGroup != NULL)
	{
		int nSel = m_comboGroups.AddString (m_strCurrentGroup);
		m_comboGroups.SetItemDataPtr (nSel, hGroup);
		m_comboGroups.SetCurSel (nSel);
		ChangeServerGroup (FALSE, TRUE);
		UpdateUIState (updateAddSrvGroup);
		m_comboServers.SetFocus ();
		//NextDlgCtrl ();
		SetModified(TRUE);
	}
}

void
CServersPage::OnRemoveServerGroup()
{
	ASSERT(m_nCurrSelGroup != CB_NOCURRSEL);
	HCHATSRVGROUP hGroup = GetCurrGroup ();
	if (hGroup != NULL)
	{
		if (!m_ui.IsGroupEmpty (hGroup))
		{
			CString strWarning;
			strWarning.LoadString (IDS_DELETEGROUP_WARNING);
			VERIFY(ReplaceToken(strWarning, CString("%1"), m_strCurrentGroup));
			if (AfxMessageBox (strWarning, MB_YESNO | MB_ICONWARNING) != IDYES)
				return;
		}

		if (m_ui.RemoveGroup (hGroup))
		{
			m_comboGroups.DeleteString (m_nCurrSelGroup);
			m_comboGroups.SetCurSel (m_nCurrSelGroup < m_comboGroups.GetCount () ? m_nCurrSelGroup : m_nCurrSelGroup - 1);
			ChangeServerGroup (FALSE, TRUE);
			UpdateUIState (updateRemoveSrvGroup);
			m_comboGroups.SetFocus ();
			//NextDlgCtrl ();
			SetModified(TRUE);
		}
	}
}

void 
CServersPage::OnChangeAuthenticationType()
{
	CDataExchange dx (this, TRUE);
	DDX_Radio (&dx, IDC_SRVCONNECT_NOPWD, m_nSecurity);
	UpdateUIState (updateChangeAuthenticationType);
	m_bServerPropChange = TRUE;
	SetModified (TRUE);
}

void
CServersPage::OnKillFocusPort()
{
	ValidatePortNumber ();
}

BOOL
CServersPage::ValidatePortNumber()
{
	BOOL bTranslated = FALSE;
	UINT nValue = GetDlgItemInt (IDC_SERVER_PORT, &bTranslated, FALSE);
	if (!bTranslated || nValue < IRCPORT_MIN || nValue > IRCPORT_MAX)
	{
		AfxMessageBox (IDS_PORTNUM_ERROR, MB_OK | MB_ICONEXCLAMATION);
		SetDlgItemInt (IDC_SERVER_PORT, m_nPort, FALSE);
		GetDlgItem (IDC_SERVER_PORT)->SetFocus ();
		//NextDlgCtrl ();
		return FALSE;
	}
	return TRUE;
}

void
CServersPage::OnChangeServerProp()
{
	if (!m_bSetActiveNeverCalled && !m_bInDDX)
	{
		m_bServerPropChange = TRUE;
		SetModified (TRUE);
	}
}

BOOL 
CServersPage::OnInitDialog()
{
	m_comboGroups.ReplaceControl (this, IDC_SRVGROUP_COMBO);
	return CCSPropertyPage::OnInitDialog ();
}

HICON CServersOnlyComboBox::GetIcon(
UINT nIndex, 
LPCSTR pszString, 
DWORD dwItemData)
{
	if (!lstrcmpi (pszString, CServersPage::sm_strUnassociatedGroup))
		return NULL;
	else
	{
		m_icon.LoadIcon (IDI_CONNECT_NET);
		return (HICON)m_icon;
	}
}

BOOL 
CServersOnlyComboBox::ShouldDrawDivision(
UINT nIndex, 
LPCSTR pszString, 
DWORD dwItemData)
{
	return FALSE;
}
