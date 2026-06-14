// SoundDlg.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "SoundDlg.h"
#include "resource.h"
#include <io.h>
#include <mmsystem.h>
#include "saywnd.h"
#include "ui.h"
#include "format.h"

#include <tchar.h>
#include "resource.h"
#include "mschat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;
extern void FirstCharUpper(char *szName) inline;

/////////////////////////////////////////////////////////////////////////////
// CSoundDlg dialog


CSoundDlg::CSoundDlg(CWnd* pParent /*=NULL*/)
	: CCSDialog(CSoundDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundDlg)
	m_selectedSnd = _T("");
	//}}AFX_DATA_INIT
}

void CSoundDlg::GetSndSelection(CString &snd)
{
	int index = m_sndList.GetCurSel();

	if (index != LB_ERR)
	{
		m_sndList.GetText(index, snd);
		snd += ".wav";
	}
	else 
		snd = "";
}

void CSoundDlg::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundDlg)
	DDX_Control(pDX, IDC_SND_MESSAGE, m_rtfCtrl);
	DDX_Control(pDX, IDC_SOUNDLIST, m_sndList);
	DDX_LBString(pDX, IDC_SOUNDLIST, m_selectedSnd);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSoundDlg, CCSDialog)
	//{{AFX_MSG_MAP(CSoundDlg)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_LBN_DBLCLK(IDC_SOUNDLIST, OnDblclkSoundlist)
	ON_LBN_SELCHANGE(IDC_SOUNDLIST, OnSelChangeSoundlist)
	ON_NOTIFY(EN_MSGFILTER, IDC_SND_MESSAGE, OnSndMsgFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL AddToSoundList(const char *dir, void *data) {	
	long hFind;
	struct _finddata_t fd;
	CSoundDlg *sndDlg = (CSoundDlg *) data;
	CString strPattern(dir);

	strPattern += "\\*.wav";
	hFind = _findfirst( (char *) (const char *) strPattern, &fd );
	if( hFind != -1 ) {
		do {
			if (fd.attrib != _A_SUBDIR) {
//				char szExt[_MAX_EXT];
				char szFName[_MAX_FNAME];

				_splitpath( fd.name, NULL, NULL, szFName, NULL /*szExt*/ );
				// CharUpperBuff(szFName, 1);
				FirstCharUpper(szFName);			// REGISB 10/15/97
				sndDlg->m_sndList.AddString(szFName);
			}
		} while( _findnext( hFind, &fd ) != -1 );
	}
	return FALSE;  // so search continues
}


		
BOOL ForPath(const char *path, BOOL soundFunc(const char *, void *), void *data) {
	while (TRUE) {
		const char *endstr;
		while (my_isspace(*path)) path++;
		if (!*path) return FALSE;
		if (*path == '"') {
			endstr = _tcschr(path, '"');
			if (!endstr) break;
			CString entry(path+1, endstr-path-1);
			if (soundFunc(entry, data)) return TRUE;
			endstr = _tcschr(endstr, ';');
		} else {
			endstr = _tcschr(path, ';');
			if (!endstr) endstr = _tcschr(path, '\0');
			CString entry(path, endstr - path);
			entry.TrimRight();
			if (soundFunc(entry, data)) return TRUE;
		}
		if (!endstr || !*endstr) break;
		path = endstr+1;
	}
	return FALSE;
}


BOOL TryToPlaySound(const char *path, void *snd) {
	const char *filename = (const char *) snd;
	CString completeFile(path);

	completeFile += "\\";
	completeFile += filename;
	return(sndPlaySound(completeFile, SND_ASYNC | SND_NODEFAULT));
}


BOOL FindAndPlaySound(const char *snd) inline {
	return (ForPath(theApp.m_soundPath, TryToPlaySound, (void *)snd));
}


void SetSoundPath(const char *path, BOOL reset = FALSE) {
	if (reset) {
		char windowsPath[MAX_PATH];
		if (!GetWindowsDirectory(windowsPath, MAX_PATH)) {
			theApp.m_soundPath = "";
			return;
		}
		theApp.m_soundPath = windowsPath;
		theApp.m_soundPath += "\\";
		CString mediaDir;
		mediaDir.LoadString(IDS_SOUND_DIR);
		theApp.m_soundPath += mediaDir;
	} else
		theApp.m_soundPath = path;
}


/////////////////////////////////////////////////////////////////////////////
// CSoundDlg message handlers

BOOL CSoundDlg::OnInitDialog() 
{
	CCSDialog::OnInitDialog();

	ForPath(theApp.m_soundPath, AddToSoundList, this);
	
	m_rtfCtrl.UseDefaultCharFormat();
	m_rtfCtrl.bSetTextColor(m_rtfCtrl.m_crTextColor);
	m_rtfCtrl.LimitText(MAX_INPUTLEN);
	m_rtfCtrl.bSetWindowFormattedText(m_rtfCtrl.m_strText, m_rtfCtrl.m_prgdwFormatting);

	// Need to add the EN_MSGFILTER notification to the dwEventMask of the rich text control
	DWORD dwEventMask = (DWORD) m_rtfCtrl.GetEventMask();
	m_rtfCtrl.SetEventMask(dwEventMask | ENM_MOUSEEVENTS);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSoundDlg::OnOK()
{
	if (m_rtfCtrl.m_pFont)
	{
		if (m_rtfCtrl.m_prgdwFormatting)
		{
			m_rtfCtrl.m_prgdwFormatting->RemoveAll();
			delete m_rtfCtrl.m_prgdwFormatting;
		}
		m_rtfCtrl.m_prgdwFormatting = PRGDWGetFormatting(&m_rtfCtrl, m_rtfCtrl.m_pFont, GetSysColor(COLOR_WINDOWTEXT));
	}

	m_rtfCtrl.GetWindowText(m_rtfCtrl.m_strText); 

	CCSDialog::OnOK();
}

void CSoundDlg::OnTest() 
{
	CString snd;
	GetSndSelection(snd);
	FindAndPlaySound(snd);
}

void CSoundDlg::OnSelChangeSoundlist()
{
	BOOL bEnabled = m_sndList.GetCurSel() != LB_ERR;
	GetDlgItem(IDOK)->EnableWindow(bEnabled);
	GetDlgItem(IDC_TEST)->EnableWindow(bEnabled);
}

void CSoundDlg::OnDblclkSoundlist() 
{
	OnTest();
}

const DWORD CSoundDlg::m_nHelpIDs[] =
{
	IDC_SOUNDLIST,					IDH_SOUNDS,
	IDC_SND_MESSAGE,				IDH_SOUND_MSG,
	IDC_TEST,						IDH_TEST_SOUND,
	0, 0
};

void CSoundDlg::OnSndMsgFilter(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct);

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfCtrl.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}

