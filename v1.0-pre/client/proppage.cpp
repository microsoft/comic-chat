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
#include "chatprot.h"
#include "userinfo.h"
#include "histent.h"
#include "memblst.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "ui.h"
#include "traj.h"
#include "spline.h"
#include "backdrop.h"
#include "balloon.h"
#include "panel.h"

#include <io.h>
#include <afxcmn.h>

#include "PropPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage property page

IMPLEMENT_DYNCREATE(CSettingsPage, CPropertyPage)

CSettingsPage::CSettingsPage() : CPropertyPage(CSettingsPage::IDD)
{
	BOOL GetSendComicsData();
	//{{AFX_DATA_INIT(CSettingsPage)
	m_bSave = GetChatDoc()->m_bPrompt;
	m_bComicsData = !GetSendComicsData();
	//}}AFX_DATA_INIT

	// managing a zero based listbox index vs number of panels
	m_nPanelsSel = CUnitPanelPage::panelsPerRow - 1;
//	m_nPanelsSel = CUnitPanelPage::panelsPerRow;

	m_bPanelClicked = FALSE;
}

CSettingsPage::~CSettingsPage()
{
}

void CSettingsPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSettingsPage)
	DDX_Control(pDX, IDC_PANELS, m_comboPanels);
	DDX_Control(pDX, IDC_COMICSDATA, m_checkData);
	DDX_Control(pDX, IDC_SAVE, m_checkSave);
	DDX_Check(pDX, IDC_SAVE, m_bSave);
	DDX_Check(pDX, IDC_COMICSDATA, m_bComicsData);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsPage, CPropertyPage)
	//{{AFX_MSG_MAP(CSettingsPage)
	ON_BN_CLICKED(IDC_COMICSDATA, OnComicsdata)
	ON_CBN_SELCHANGE(IDC_PANELS, OnSelchangePanels)
	ON_BN_CLICKED(IDC_SAVE, OnSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSettingsPage message handlers
extern void SetPanelsWide(int);
// extern void SetPanelsHigh(int);
extern BOOL OKPanelWidth(int);
// extern BOOL OKPanelHeight(int);

BOOL CSettingsPage::OnSetActive() 
{
	//	make sure it is set up properly for both comics view and textview
	if(GetChatApp()->m_bComicView)
	{
		// only enter the #panels options that the current size will allow
/*		if(OKPanelWidth(1))
			m_comboPanels.AddString("1 Panel Wide");
		if(OKPanelWidth(2))
			m_comboPanels.AddString("2 Panels Wide");
		if(OKPanelWidth(3))
			m_comboPanels.AddString("3 Panels Wide");
		if(OKPanelWidth(4))
			m_comboPanels.AddString("4 Panels Wide");
		if(OKPanelWidth(5))
			m_comboPanels.AddString("5 Panels Wide");

		m_comboPanels.SetCurSel(m_nPanelsSel-1);*/

		//This causes far too much confusion.  Especially when the
		// current number of panels isnt satisfied by OKPanelWidth
		// and a blank comes up in the combo box.  For now we will allow the user to make
		// it up to 4 panels wide if they want.
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

	}
	else
	{
		m_comboPanels.EnableWindow(FALSE);
		m_checkData.EnableWindow(FALSE);
	}

	if(GetChatDoc()->m_bEmbedded)
		m_checkSave.EnableWindow(FALSE);
	return CPropertyPage::OnSetActive();
}

BOOL CSettingsPage::OnKillActive() 
{
	// clear list box
	m_comboPanels.ResetContent();
	// disable ApplyNow button
	SetModified(FALSE);
	return CPropertyPage::OnKillActive();
}

// manage the send comics data variable
void CSettingsPage::OnComicsdata() 
{
	m_bComicsData = !m_bComicsData;
	SetModified(TRUE);
}

// manage the combobox for panel selection
void CSettingsPage::OnSelchangePanels() 
{
	m_nPanelsSel = m_comboPanels.GetCurSel();
	SetModified(TRUE);
	m_bPanelClicked = TRUE;
	
}

// manage the save on close variable
void CSettingsPage::OnSave() 
{
	m_bSave = !m_bSave;
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
	GetChatDoc()->m_bPrompt = m_bSave;
	if(!GetChatDoc()->m_bPrompt)
		GetChatDoc()->SetModifiedFlag(FALSE);

	// verify the panel selection and commit
//	if(m_nPanelsSel+1 != CUnitPanelPage::panelsPerRow)
	if(m_bPanelClicked)
		SetPanelsWide(m_nPanelsSel+1);

	m_bPanelClicked = FALSE;
	CPropertyPage::OnOK();
}
/////////////////////////////////////////////////////////////////////////////
// CPersonalPage property page

IMPLEMENT_DYNCREATE(CPersonalPage, CPropertyPage)


const char *GetMyName();
void SetMyName(const char *charName);
const char *GetMyRealName();
void SetMyRealName(const char *charName);
BOOL GetProfileString(CString &strProf);

CPersonalPage::CPersonalPage() : CPropertyPage(CPersonalPage::IDD)
{
	//{{AFX_DATA_INIT(CPersonalPage)
	m_strProfile = _T("");
	m_strNickname = _T("");
	m_strRealName = _T("");
	//}}AFX_DATA_INIT
	GetProfileString(m_strProfile);
	m_strNickname = GetMyName();
	m_strRealName = GetMyRealName();

}

CPersonalPage::~CPersonalPage()
{
}

void CPersonalPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPersonalPage)
	DDX_Control(pDX, IDC_REALNAME, m_editRealName);
	DDX_Control(pDX, IDC_NICKNAME, m_editNickname);
	DDX_Control(pDX, IDC_PROFILE_EDIT, m_editProfile);
	DDX_Text(pDX, IDC_PROFILE_EDIT, m_strProfile);
	DDX_Text(pDX, IDC_NICKNAME, m_strNickname);
	DDV_MaxChars(pDX, m_strNickname, 9);
	DDX_Text(pDX, IDC_REALNAME, m_strRealName);
	DDV_MaxChars(pDX, m_strRealName, 30);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPersonalPage, CPropertyPage)
	//{{AFX_MSG_MAP(CPersonalPage)
	ON_EN_CHANGE(IDC_PROFILE_EDIT, OnChangeProfileEdit)
	ON_EN_CHANGE(IDC_NICKNAME, OnChangeNickname)
	ON_EN_CHANGE(IDC_REALNAME, OnChangeRealname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPersonalPage message handlers

BOOL GetProfileString(CString &strProf) {
	FILE *fp;
	char buff[1000];
	int nread;

	CString path = GetChatApp()->GetBaseDir();
	path += "\\profile.txt";

	if (fp = fopen(path, "r")) {
		strProf = "";
		while (nread = fread(buff, 1, 1000, fp)) {
			buff[nread] = '\0';
			strProf += buff;
		}
		fclose(fp);
		strProf.TrimLeft();
		strProf.TrimRight();
		strProf = strProf.Left(450);    // for now.  Potential internat. problem...
		if (strProf == "")
			strProf.LoadString(ID_DEFAULT_PROFILE);  // empty profile
		return TRUE;
	} else {
		strProf.LoadString(ID_DEFAULT_PROFILE);		// no profile
		return FALSE;
	}
}

void CPersonalPage::OnOK() 
{
	
	FILE *fp;
	CString path = GetChatApp()->GetBaseDir();
	path += "\\profile.txt";

	if (fp = fopen(path, "w")) {
		fwrite((LPCSTR) m_strProfile, m_strProfile.GetLength(), 1, fp);
		fclose(fp);
	} else {
		AfxMessageBox(ID_ERR_PROFILE_WRITE);
	}

	ChatSetNick(m_strNickname);
	SetMyRealName(m_strRealName);

	// Update these properties with the system here.
	CPropertyPage::OnOK();
}

BOOL CPersonalPage::OnKillActive() 
{
	SetModified(FALSE);
	return CPropertyPage::OnKillActive();
}

void CPersonalPage::OnChangeProfileEdit() 
{
	SetModified(TRUE);
}

void CPersonalPage::OnChangeNickname() 
{
	SetModified(TRUE);
}

void CPersonalPage::OnChangeRealname() 
{
	SetModified(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// CCharacterPage property page

IMPLEMENT_DYNCREATE(CCharacterPage, CPropertyPage)

CCharacterPage::CCharacterPage() : CPropertyPage(CCharacterPage::IDD)
{
	//{{AFX_DATA_INIT(CCharacterPage)
	m_strList = "";
	//}}AFX_DATA_INIT
	if(GetChatApp()->m_bComicView) {
		CAvatarX *myAv = MyAvatar();
		if (myAv) m_strSel = myAv->m_name;  // the usual case
		else {
			char avName[100];
			void GetNextAvatarName(char *);
			GetNextAvatarName(avName);
			m_strSel = avName;
		}
	}
}

CCharacterPage::~CCharacterPage()
{
}

void CCharacterPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCharacterPage)
	DDX_Control(pDX, IDC_AVLIST, m_avList);
	DDX_LBString(pDX, IDC_AVLIST, m_strList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CCharacterPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCharacterPage)
	ON_LBN_SELCHANGE(IDC_AVLIST, OnSelchangeAvlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCharacterPage message handlers

BOOL CCharacterPage::OnSetActive() 
{
	long hFind;
	struct _finddata_t fd;
   	CRect rAvatarPreview(cxPosAvatarPage, cyPosAvatarPage, (cxPosAvatarPage + cxAvatarPage),
   	                     (cyPosAvatarPage + cyAvatarPage));
	MapDialogRect(rAvatarPreview);

	CPropertyPage::OnSetActive();
  
    // create body cam pane
	VERIFY( m_wndCharSelBodyCam.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rAvatarPreview, this, 5 ) );
    m_wndCharSelBodyCam.m_talkToHeight = 0;					// no talktos
	m_wndCharSelBodyCam.RecalcRetainedBMP();
	m_wndCharSelBodyCam.m_forcedDelete = FALSE;
	cui.m_pvCharSelBodyCamWnd = &m_wndCharSelBodyCam;
	m_wndCharSelBodyCam.m_avatar = GetAvatar2(m_strSel);//MyAvatar();

    // build file search strings
    CString strPattern;

    m_strAvatarFiles.RemoveAll();
	strPattern = GetChatApp()->GetAvatarDir();
	strPattern += "\\*.avb";
	hFind = _findfirst( (char *) (const char *) strPattern, &fd );
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
				m_avList.AddString(szFName);
		   }

		} while( _findnext( hFind, &fd ) != -1 );
	}

//	const char *prevName = MyAvatar()->m_name;
//	if (prevName) {
//		int index = m_avList.FindStringExact(0, prevName);
//		if (index != LB_ERR) m_avList.SetCurSel(index);
	if (!m_strSel.IsEmpty()) {
		int index = m_avList.FindStringExact(0, m_strSel);
		if (index != LB_ERR) m_avList.SetCurSel(index);
	}
	return TRUE;
}

void CCharacterPage::OnOK() 
{
	CAvatarX *modelAv = m_wndCharSelBodyCam.m_avatar;
	USHORT modelAvID = modelAv->m_avatarID;
	if (MyAvatarID() != modelAvID) {   // Did we choose a different avatar?
		void AddAndExecute(HistoryEntry *);
		extern CUserInfo *puiSelf;
		if (puiSelf)  // i.e., we're in channel, so this is safe
			AddAndExecute(new ChangeAvatarEntry(puiSelf, modelAv->m_name));
		else SetMyAvatar(modelAv->m_name);
	}
	m_wndCharSelBodyCam.DestroyWindow();
	cui.m_pvCharSelBodyCamWnd = NULL;

	CPropertyPage::OnOK();
}

void CCharacterPage::OnCancel() 
{
	m_wndCharSelBodyCam.DestroyWindow();
    cui.m_pvCharSelBodyCamWnd = NULL;
	CPropertyPage::OnCancel();
}

void CCharacterPage::OnSelchangeAvlist() 
{
	int index = m_avList.GetCurSel();
	if (index != LB_ERR) {
		char newName[100];
		m_avList.GetText(index, newName);
		m_strSel = newName;
		if (stricmp(newName, m_wndCharSelBodyCam.m_avatar->m_name)) {
			m_wndCharSelBodyCam.m_avatar = GetAvatar2(newName);
			m_wndCharSelBodyCam.RefreshBody();
			SetModified(TRUE);
		}
	}	
}

BOOL CCharacterPage::OnKillActive() 
{
	m_wndCharSelBodyCam.DestroyWindow();
    cui.m_pvCharSelBodyCamWnd = NULL;
	m_avList.ResetContent();
	SetModified(FALSE);
	return CPropertyPage::OnKillActive();
}
/////////////////////////////////////////////////////////////////////////////
// CBackgroundPage property page

IMPLEMENT_DYNCREATE(CBackgroundPage, CPropertyPage)

CBackgroundPage::CBackgroundPage() : CPropertyPage(CBackgroundPage::IDD)
{
	//{{AFX_DATA_INIT(CBackgroundPage)
	m_strBackground = _T("");
	//}}AFX_DATA_INIT
	m_strCurrentSel = "";
	m_rect.left = m_rect.right = m_rect.top = m_rect.bottom = 0;
}

CBackgroundPage::~CBackgroundPage()
{
}

void CBackgroundPage::DoDataExchange(CDataExchange* pDX)
{
	CPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBackgroundPage)
	DDX_Control(pDX, IDC_BACKLIST, m_listBack);
	DDX_LBString(pDX, IDC_BACKLIST, m_strBackground);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBackgroundPage, CPropertyPage)
	//{{AFX_MSG_MAP(CBackgroundPage)
	ON_LBN_SELCHANGE(IDC_BACKLIST, OnSelchangeBacklist)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBackgroundPage message handlers

BOOL CBackgroundPage::OnSetActive() 
{
    CString strPattern;
	long hFind;
	struct _finddata_t fd;

	strPattern = GetChatApp()->GetBackDropDir();
	strPattern += "\\*.bmp";
	hFind = _findfirst( (char *) (const char *) strPattern, &fd );
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
				m_listBack.AddString(szFName);
		   }

		} while( _findnext( hFind, &fd ) != -1 );
	}

	// set selection to current background
	const char *GetCurrentBackDropName();
	const char *prevName = GetCurrentBackDropName();
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
	
	return CPropertyPage::OnSetActive();
}

// load up the new background and repaint
void CBackgroundPage::PreviewSelection()
{
	CString strPath;
	int index = m_listBack.GetCurSel();
	m_listBack.GetText(index,m_strCurrentSel);
	strPath = GetChatApp()->GetBackDropDir();
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
	m_dib.Draw(&dc,m_rect.left,m_rect.top,m_rect.Width(),m_rect.Height());
}

BOOL CBackgroundPage::OnKillActive() 
{
	m_listBack.ResetContent();
	SetModified(FALSE);
	
	return CPropertyPage::OnKillActive();
}


void CBackgroundPage::OnOK() 
{
	// would also be nice to implement a GetCurrentBackground so I can 
	// set it when the dialog comes up.
	void AddAndExecute(HistoryEntry *);
	void SetBackDrop(const char *);
	int status = ChatGetConnectionStatus();
	if (status == CX_INCHANNEL || status == CX_DISCONNECTED)
		AddAndExecute(new ChangeBackDropEntry((const char *) m_strCurrentSel));
	else SetBackDrop(m_strCurrentSel);

	CPropertyPage::OnOK();
}

