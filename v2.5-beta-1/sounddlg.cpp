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

LPCSTR pszSupportedSoundTypes[] = { "wav", "mid", "rmi", NULL };
LPCSTR pszSupportedSoundTypeList = { "wav\0mid\0rmi\0" };
LPCSTR pszSupportedSoundTypesNoMIDI[] = { "wav", NULL };
LPCSTR pszSupportedSoundTypeListNoMIDI = { "wav\0" };

LPCSTR * GetSupportedSoundTypes()
{
	return theApp.m_bNoMIDI ? pszSupportedSoundTypesNoMIDI : pszSupportedSoundTypes;
}
LPCSTR GetSupportedSoundTypeList()
{
	return theApp.m_bNoMIDI ? pszSupportedSoundTypeListNoMIDI : pszSupportedSoundTypeList;
}

extern CChatApp theApp;

CImageList 	CSoundDlg::sm_imglist;
int 			CSoundDlg::sm_nImgListIcons[8];

CSoundList::CSoundList()
{
	m_pbBuffer = NULL;
	m_dwBufferSize = SNDLIST_INITIAL_BUFFER;
	m_nEntries = 0;
	m_pbPosition = NULL;
}

CSoundList::~CSoundList()
{
	ClearAll();
}

void 
CSoundList::ClearAll()
{
	free (m_pbBuffer);
	m_pbBuffer = m_pbPosition = NULL;
	m_nEntries = 0;
}

BOOL
CSoundList::AddName(
LPCSTR pszName,
int nIcon,
int nType)
{
	UINT nBytesNeeded = lstrlen (pszName) + 3;
	if (m_pbBuffer == NULL || m_pbBuffer + m_dwBufferSize < m_pbPosition + nBytesNeeded)
	{
		DWORD dwNewBufferSize;
		PBYTE pb;
		PBYTE pbPosition;
		if (m_pbBuffer == NULL)
		{
			// Use last allocated buffer size.
			dwNewBufferSize = m_dwBufferSize;
			pb = (PBYTE)malloc (dwNewBufferSize);
			pbPosition = pb;
		}
		else
		{
			dwNewBufferSize = 2 * m_dwBufferSize;
			pb = (LPBYTE)realloc (m_pbBuffer, dwNewBufferSize);
			pbPosition = (m_pbPosition - m_pbBuffer) + pb;
		}

		if (pb == NULL)
			return FALSE;
		m_pbBuffer = pb;
		m_dwBufferSize = dwNewBufferSize;
		m_pbPosition = pbPosition;
	}

	m_pbPosition[0] = (BYTE)nIcon;
	m_pbPosition[1] = (BYTE)nType;
	lstrcpy ((LPSTR)m_pbPosition + 2, pszName);
	m_pbPosition += nBytesNeeded;
	m_nEntries++;
	return TRUE;
}

void 
CSoundList::AddToListCtrl(
CListCtrl* pListCtrl)
{
	pListCtrl->DeleteAllItems ();
	
	// Compact the buffer.
	if (m_pbBuffer != NULL)
	{
		m_pbBuffer = (LPBYTE)realloc (m_pbBuffer, m_pbPosition - m_pbBuffer);
		ASSERT(m_pbBuffer != NULL);
	}

	if (m_pbBuffer == NULL || m_nEntries == 0)
		return;

	// Set the item count on the list control.

	pListCtrl->SetItemCount (m_nEntries);

	LPBYTE pbPosition = m_pbBuffer;
	for (int i = m_nEntries; i > 0; i--)
	{
		int nItem = pListCtrl->InsertItem (LVIF_PARAM|LVIF_TEXT|LVIF_IMAGE,
						0, (LPSTR)pbPosition + 2, 0, 0, pbPosition[0], (LPARAM)pbPosition[1]);
		if (nItem == -1)
			break;
		pListCtrl->SetItemData (nItem, (DWORD)pbPosition[1]);
		pbPosition += 3 + lstrlen ((LPCSTR)pbPosition + 2);
	}
}

LPCSTR 
CSoundList::GetItemName(
DWORD dwData)
{
	ASSERT(dwData && (LPVOID)dwData >= m_pbBuffer && (LPVOID)dwData < m_pbPosition);
	return (LPCSTR)dwData + 1;
}

int 
CSoundList::GetItemIconIndex(
DWORD dwData)
{
	ASSERT(dwData && (LPVOID)dwData >= m_pbBuffer && (LPVOID)dwData < m_pbPosition);
	return (int)*(LPBYTE)dwData;
}

int 
CSoundList::GetItemFileType(
DWORD dwData)
{
	ASSERT(dwData && (LPVOID)dwData >= m_pbBuffer && (LPVOID)dwData < m_pbPosition);
	return (int)(((LPBYTE)dwData)[1]);
}

/////////////////////////////////////////////////////////////////////////////
// CSoundDlg dialog

CString CSoundDlg::sm_strMatchString;

CSoundDlg::CSoundDlg(CWnd* pParent /*=NULL*/)
	: CCSDialog(CSoundDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSoundDlg)
	m_selectedSnd = _T("");
	//}}AFX_DATA_INIT

	// Create image list.

	if (!sm_imglist.m_hImageList)
	{
		ZeroMemory (sm_nImgListIcons, sizeof(sm_nImgListIcons));
		if (sm_imglist.Create (16, 16, TRUE, 1, 4))
		{
			LPCSTR * pSoundTypes = GetSupportedSoundTypes ();
			for (int i = 0; pSoundTypes[i]; i++)
			{
				HICON hicon = GetFiletypeIcon (pSoundTypes[i], TRUE);
				if (hicon)
				{
					sm_nImgListIcons[i] = sm_imglist.Add (hicon);
					DestroyIcon (hicon);
				}
			}
		}
	}

	m_bStartedSounds = FALSE;
	m_dwLastFilterEdit = 0;
}

void CSoundDlg::GetSndSelection(CString &strSound, BOOL bNameOnly)
{
	int iIndex = m_sndList.GetNextItem (-1, LVNI_ALL | LVNI_SELECTED);

	if (iIndex != -1)
	{
		strSound = m_sndList.GetItemText(iIndex, 0);
		if (!bNameOnly)
		{
			int nType = (int)m_sndList.GetItemData (iIndex);
			strSound += '.';
			strSound += GetSupportedSoundTypes ()[nType];
		}
	}
	else 
		strSound.Empty ();
}

void CSoundDlg::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSoundDlg)
	DDX_Control(pDX, IDC_SND_MESSAGE, m_rtfCtrl);
	DDX_Control(pDX, IDC_SOUNDLIST, m_sndList);
	//}}AFX_DATA_MAP
	if (pDX->m_bSaveAndValidate)
	{
		GetSndSelection (m_selectedSnd);
	}
	else
	{
		DDX_Text(pDX, IDC_SOUND_MATCH, sm_strMatchString);
	}
}


BEGIN_MESSAGE_MAP(CSoundDlg, CCSDialog)
	//{{AFX_MSG_MAP(CSoundDlg)
	ON_BN_CLICKED(IDC_TEST, OnTest)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_SOUNDLIST, OnSelChangeSoundlist)
	ON_NOTIFY(NM_DBLCLK, IDC_SOUNDLIST, OnDblclkSoundlist)
	ON_NOTIFY(EN_MSGFILTER, IDC_SND_MESSAGE, OnSndMsgFilter)
	ON_WM_TIMER()
	ON_EN_CHANGE(IDC_SOUND_MATCH, OnEditChangeFilter)
	ON_EN_KILLFOCUS(IDC_SOUND_MATCH, OnKillFocusFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void MaybeAddSlash(CString& str)
{
	int nLen = str.GetLength ();
	if (nLen == 0 || str.ReverseFind ('\\') != nLen - 1)
	{
		str	+= '\\';
	}
}

struct PLAYSOUNDSTRUCT
{
	LPCSTR pszSound;
	int nType;
	BOOL bStopSound;
	BOOL bSemiSync;
};

BOOL bTryToPlaySound(const char *szPath, void *szSound) {
	PLAYSOUNDSTRUCT * pPlaySound = (PLAYSOUNDSTRUCT *)szSound;
	const char *szFilename = pPlaySound->pszSound;
	CString strCompleteFile(szPath);
	// 03/09/98 ShankuN - don't add slash for paths like C:\ (Bugfix #1279)
	MaybeAddSlash (strCompleteFile);
	strCompleteFile += szFilename;
	UINT flags = SND_ASYNC | SND_NODEFAULT;
	if (!pPlaySound->bStopSound)
		flags |= SND_NOSTOP;
	BOOL bRet;
	if (GetFileAttributes (strCompleteFile) != (DWORD)-1L)
	{
		switch (pPlaySound->nType)
		{
			case SOUNDTYPE_WAV:
				bRet = sndPlaySound (strCompleteFile, flags);
				break;
			case SOUNDTYPE_MID:
			case SOUNDTYPE_RMI:
				if (pPlaySound->bSemiSync)
				{
					flags |= SND_SEMISYNC;
					flags &= ~SND_ASYNC;
				}
				bRet = sndPlayMidiSound (strCompleteFile, flags);
				break;
			default:
				// Not supported, abort.
				bRet = TRUE;
				break;
		}
	}
	else
	{
		bRet = FALSE;
	}
	return bRet;
}

BOOL bForPath(const char *szPath, BOOL soundFunc(const char *, void *), void *pvData) {
	while (TRUE) {
		const char *szEndStr;
		while (my_isspace(*szPath)) szPath++;
		if (!*szPath) return FALSE;
		if (*szPath == '"') {
			szEndStr = _tcschr(szPath, '"');
			if (!szEndStr) break;
			CString strEntry(szPath+1, szEndStr-szPath-1);
			if (soundFunc(strEntry, pvData)) return TRUE;
			szEndStr = _tcschr(szEndStr, ';');
		} else {
			szEndStr = _tcschr(szPath, ';');
			if (!szEndStr) szEndStr = _tcschr(szPath, '\0');
			CString strEntry(szPath, szEndStr - szPath);
			strEntry.TrimRight();
			if (soundFunc(strEntry, pvData)) return TRUE;
		}
		if (!szEndStr || !*szEndStr) break;
		szPath = szEndStr+1;
	}
	return FALSE;
}

inline void EnumSounds(
LPCSTR pszPath, 
FILEENUMSTRUCT * pfileenum)
{
	pfileenum->pszTypes = GetSupportedSoundTypeList ();
	EnumFiles (pszPath, pfileenum);
}


BOOL bFindAndPlaySound(const char *szSound, BOOL bStopSound, BOOL bSemiSync)  {
	if (OurMbsStr (szSound, "..") || OurMbsPbrk (szSound, "\\:"))
		return FALSE;

	PLAYSOUNDSTRUCT playsound;
	playsound.pszSound = szSound;
	char szExt[_MAX_EXT];
	LPSTR pszExt;
	_splitpath(szSound, NULL, NULL, NULL, szExt);
	pszExt = *szExt ? szExt + 1 : szExt;
	LPCSTR* pSupportedSoundTypes = GetSupportedSoundTypes ();
	for (int i = 0; pSupportedSoundTypes[i] != NULL; i++)
	{
		if (!lstrcmpi (pszExt, pSupportedSoundTypes[i]))
			break;
	}
	if (pSupportedSoundTypes[i] == NULL)
		return FALSE;
	playsound.nType = i;

	playsound.bStopSound = bStopSound;
	playsound.bSemiSync = bSemiSync;
	return bForPath(theApp.m_soundPath, bTryToPlaySound, &playsound);
}


void SetSoundPath(const char *szPath, BOOL bReset = FALSE) {
	if (bReset) {
		GetWindowsMediaDirectory (&theApp.m_soundPath);
	} else
		theApp.m_soundPath = szPath;
}


/////////////////////////////////////////////////////////////////////////////
// CSoundDlg message handlers


struct SOUNDDLGUI
{
	CSoundList* pSoundList;
	int * pnImages;
};

void CALLBACK
AddSoundToSoundList(
LPARAM lParam,
LPCSTR pszPath, 
LPCSTR pszFile, 
int	   nFileType)
{
	SOUNDDLGUI * psdui = (SOUNDDLGUI*)lParam;
	psdui->pSoundList->AddName (pszFile, psdui->pnImages[nFileType], nFileType);
}

BOOL CSoundDlg::OnInitDialog() 
{
	CCSDialog::OnInitDialog();

	m_sndList.InsertColumn (0, "");
	if (sm_imglist.m_hImageList)
	{
		m_sndList.SetImageList (&sm_imglist, LVSIL_SMALL);
	}

	RescanFilter (TRUE);

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
	// Stop sounds if we started em
	StopSounds ();

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

void CSoundDlg::OnCancel()
{
	// Stop sounds if we started em
	StopSounds ();

	CCSDialog::OnCancel ();
}

void CSoundDlg::OnTest() 
{
	CString strSound;
	GetSndSelection(strSound);
	if (!strSound.IsEmpty ())
	{
		CWaitCursor cur;
		StopSounds (TRUE);
		bFindAndPlaySound(strSound, TRUE, TRUE);
		m_bStartedSounds = TRUE;
	}
}

void CSoundDlg::OnSelChangeSoundlist(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	// NOTE: This function is also called internally, with pNotifyStruct and plResult
	// being NULL. So be sure to handle this if accessing either!
	BOOL bEnabled = m_sndList.GetNextItem (-1, LVNI_ALL | LVNI_SELECTED) != -1;
	GetDlgItem(IDOK)->EnableWindow(bEnabled);
	GetDlgItem(IDC_TEST)->EnableWindow(bEnabled);
}

void CSoundDlg::OnDblclkSoundlist(NMHDR *pNotifyStruct, LRESULT *plResult)
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

void 
CSoundDlg::StopSounds(
BOOL bForce)
{
	if (bForce || m_bStartedSounds)
	{
		sndPlaySound (NULL, SND_SYNC);
		sndPlayMidiSound (NULL, SND_SYNC);
	}
}

void 
CSoundDlg::RescanFilter(
BOOL bForce)
{
	KillTimer (100);
	m_dwLastFilterEdit = 0;

	// Get the match string.

	CString strMatch;
	GetDlgItemText (IDC_SOUND_MATCH, strMatch);
	if (!bForce && !strMatch.CompareNoCase (sm_strMatchString))
	{
		// No change!
		return;
	}
	sm_strMatchString = strMatch;

	// Get previous selection.
	CString strSelectedSnd;
	GetSndSelection (strSelectedSnd, TRUE /* Name only */);

	DWORD dwTime = timeGetTime ();
	m_sndList.SetRedraw (FALSE);
	m_soundEnumList.ClearAll ();

	CWaitCursor cur;
	SOUNDDLGUI sdui;
	FILEENUMSTRUCT fileenum;
	sdui.pSoundList = &m_soundEnumList;
	sdui.pnImages = sm_nImgListIcons;
	fileenum.pfnAdd = AddSoundToSoundList;
	fileenum.lParam = (LPARAM)&sdui;
	fileenum.bRecursive = FALSE;
	fileenum.pszSubFilter = sm_strMatchString;
	EnumSounds (theApp.m_soundPath, &fileenum); 

	m_soundEnumList.AddToListCtrl (&m_sndList);
	CRect rectClient;
	m_sndList.GetClientRect (rectClient);
	m_sndList.SetColumnWidth (0, rectClient.Width () - 1);

	m_sndList.SetRedraw (TRUE);
	m_sndList.Invalidate ();
	TRACE ("Adding sounds took %lu milliseconds\n", timeGetTime () - dwTime);

	// Set the previous selection back, if possible.

	int nFind;
	if (!strSelectedSnd.IsEmpty ())
	{
		LV_FINDINFO find;
		find.flags = LVFI_STRING;
		find.psz = strSelectedSnd;
		nFind = m_sndList.FindItem (&find);
	}
	else
		nFind = -1;
	if (nFind != -1)
	{
		m_sndList.SetItemState (nFind, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	}
	else 
	{
		// Select the first one by default.
		m_sndList.SetItemState (0, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
		if (!strSelectedSnd.IsEmpty ())
		{
			// Selection changed as a result of the filter change!
			OnSelChangeSoundlist(NULL, NULL);
		}
	}
}

void 
CSoundDlg::OnTimer(
UINT nTimerID)
{
	if (m_dwLastFilterEdit == 0)
		return;

	DWORD dwTime = timeGetTime ();
	if (dwTime < m_dwLastFilterEdit || dwTime > m_dwLastFilterEdit + 1000)
	{
		RescanFilter ();
	}
}

void 
CSoundDlg::OnEditChangeFilter()
{
	if (m_dwLastFilterEdit == 0)
	{
		SetTimer (100, 500, NULL);
	}
	m_dwLastFilterEdit = timeGetTime ();
}

void 
CSoundDlg::OnKillFocusFilter()
{
	if (m_dwLastFilterEdit != 0)
	{
		RescanFilter ();
	}
}
