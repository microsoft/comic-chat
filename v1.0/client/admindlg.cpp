// AdminDlgs.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "admindlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKickDialog dialog


CKickDialog::CKickDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CKickDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKickDialog)
	m_reason = _T("");
	m_strKick = _T("");
	//}}AFX_DATA_INIT
}


void CKickDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKickDialog)
	DDX_Control(pDX, IDC_KICKMSG, m_Kick);
	DDX_Text(pDX, IDC_REASON, m_reason);
	DDV_MaxChars(pDX, m_reason, 200);
	DDX_Text(pDX, IDC_KICKMSG, m_strKick);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKickDialog, CDialog)
	//{{AFX_MSG_MAP(CKickDialog)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKickDialog message handlers
/////////////////////////////////////////////////////////////////////////////
// CTopicDlg dialog


CTopicDlg::CTopicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTopicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTopicDlg)
	m_topic = _T("");
	//}}AFX_DATA_INIT
}


void CTopicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTopicDlg)
	DDX_Text(pDX, IDC_TOPIC, m_topic);
	DDV_MaxChars(pDX, m_topic, 256);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTopicDlg, CDialog)
	//{{AFX_MSG_MAP(CTopicDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopicDlg message handlers
