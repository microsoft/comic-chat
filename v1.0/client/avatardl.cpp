// avatardl.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "bodycam.h"
#include "chatprot.h"
#include "userinfo.h"
#include "histent.h"
#include "memblst.h"
#include "ui.h"

#include "avatardl.h"
#include <io.h>
#include <afxcmn.h>

// #include "render.h"
#include "ui.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The one and only CClientApp object

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAvatarDlg dialog


CAvatarDlg::CAvatarDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAvatarDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAvatarDlg)
	//}}AFX_DATA_INIT
}

void CAvatarDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAvatarDlg)
	DDX_Control(pDX, IDC_AVLIST, m_avBox);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAvatarDlg, CDialog)
	//{{AFX_MSG_MAP(CAvatarDlg)
	ON_LBN_DBLCLK(IDC_AVLIST, OnDblclkAvlist)
	ON_LBN_SELCHANGE(IDC_AVLIST, OnSelchangeAvlist)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CAvatarDlg message handlers

BOOL CAvatarDlg::OnInitDialog() 
{ 
	long hFind;
	struct _finddata_t fd;
   	CRect rAvatarPreview(cxPosAvatar, cyPosAvatar, (cxPosAvatar + cxAvatar),
   	                     (cyPosAvatar + cyAvatar));
	MapDialogRect(rAvatarPreview);

	CDialog::OnInitDialog();
  
    // create body cam pane
	VERIFY( m_wndCharSelBodyCam.Create( NULL, NULL, WS_CHILD | WS_VISIBLE, rAvatarPreview, this, 5 ) );
    m_wndCharSelBodyCam.m_talkToHeight = 0;					// no talktos
	m_wndCharSelBodyCam.RecalcRetainedBMP();
	m_wndCharSelBodyCam.m_forcedDelete = FALSE;
	cui.m_pvCharSelBodyCamWnd = &m_wndCharSelBodyCam;
	m_wndCharSelBodyCam.m_avatar = MyAvatar();

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
				char szExt[_MAX_EXT];
				char szFName[_MAX_FNAME];

				_splitpath( fd.name, NULL, NULL, szFName, szExt );
				CharUpperBuff(szFName, 1);
				m_avBox.AddString(szFName);
		   }

		} while( _findnext( hFind, &fd ) != -1 );
	}

	const char *prevName = MyAvatar()->m_name;
	if (prevName) {
		int index = m_avBox.FindStringExact(0, prevName);
		if (index != LB_ERR) m_avBox.SetCurSel(index);
	}
	
	return TRUE;
}

void CAvatarDlg::OnOK() 
{
	CAvatarX *modelAv = m_wndCharSelBodyCam.m_avatar;
	USHORT modelAvID = modelAv->m_avatarID;
	if (MyAvatarID() != modelAvID) {   // Did we choose a different avatar?
		void AddAndExecute(HistoryEntry *);
		extern CUserInfo *puiSelf;
		AddAndExecute(new ChangeAvatarEntry(puiSelf, modelAv->m_name));
	}

	cui.m_pvCharSelBodyCamWnd = NULL;

	CDialog::OnOK();
}

void CAvatarDlg::OnCancel() 
{
    cui.m_pvCharSelBodyCamWnd = NULL;
	CDialog::OnCancel();
}


void CAvatarDlg::HandleUserAvatarSel()
{
#if 0
    char szAvatarName[MAX_PATH];
	TV_ITEM TreeCtrlItem = {0};
	int iava;

//	HTREEITEM hTreeItem = m_pAvatarDirs.GetSelectedItem();

    TreeCtrlItem.cchTextMax = MAX_PATH;
    TreeCtrlItem.pszText = szAvatarName;
    TreeCtrlItem.mask = TVIF_TEXT | TVIF_PARAM | TVIF_HANDLE;
	TreeCtrlItem.hItem = hTreeItem; 
    m_pAvatarDirs.GetItem(&TreeCtrlItem);   

	iava = (int) TreeCtrlItem.lParam;

	// ignore folder selection
	if (iava == 0)
		return;
    
    if (iava < 0)
    {
        m_strAvaPick = m_strAvatarFiles[(-iava) - 1];
	    m_strAvaDirectory = m_strDirShared;
    }
    else
    {
        m_strAvaPick = m_strAvatarFiles[iava - 1];
        m_strAvaDirectory = m_strDirWorld;
    }

//    VERIFY( m_wndCharSelBodyCam.SetDib( m_strAvaPick, theApp.m_strAvatarPath, 
//                                        cImagesPerAvatar));
//    m_iAvaImage = GetCharSelBodyCam()->GetDib();
//    VERIFY( m_wndCharSelBodyCam.SetDibImage(m_iAvaImage));
#endif
}

#if 0
void CAvatarDlg::OnSelchangedAvatree(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	
	HandleUserAvatarSel();	

	*pResult = 0;
}
#endif

void CAvatarDlg::OnDblclkAvlist() 
{
	int index = m_avBox.GetCurSel();
	if (index != LB_ERR) {
		char newName[100];
		m_avBox.GetText(index, newName);
		if (stricmp(newName, m_wndCharSelBodyCam.m_avatar->m_name)) {
			m_wndCharSelBodyCam.m_avatar = GetAvatar2(newName);
			m_wndCharSelBodyCam.RefreshBody();
		}
	}	
	OnOK();	
}

//CAvatarX *GetAvatar2(const char *);
void RefreshBodyPreview();

void CAvatarDlg::OnSelchangeAvlist() 
{
	int index = m_avBox.GetCurSel();
	if (index != LB_ERR) {
		char newName[100];
		m_avBox.GetText(index, newName);
		if (stricmp(newName, m_wndCharSelBodyCam.m_avatar->m_name)) {
			m_wndCharSelBodyCam.m_avatar = GetAvatar2(newName);
			m_wndCharSelBodyCam.RefreshBody();
		}
	}	
}
