// profdlg.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "profdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CProfileDialog dialog

CProfileDialog::CProfileDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CProfileDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CProfileDialog)
	m_profileString = _T("");
	//}}AFX_DATA_INIT
	BOOL GetProfileString(CString &);

	GetProfileString(m_profileString);
}


void CProfileDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CProfileDialog)
	DDX_Text(pDX, IDC_PROFILE_EDIT, m_profileString);
	DDV_MaxChars(pDX, m_profileString, 450);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CProfileDialog, CDialog)
	//{{AFX_MSG_MAP(CProfileDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CProfileDialog message handlers

void CProfileDialog::OnOK() 
{
	CDialog::OnOK();

	FILE *fp;
	CString path = theApp.GetBaseDir();
	path += "\\profile.txt";

	if (fp = fopen(path, "w")) {
		fwrite((LPCSTR) m_profileString, m_profileString.GetLength(), 1, fp);
		fclose(fp);
	} else {
		AfxMessageBox(ID_ERR_PROFILE_WRITE);
	}
}

