// AdminDlgs.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "userinfo.h"
#include "chatprot.h"
#include "admindlg.h"
#include "resource.h"
#include "mschat.h"
#include "ircproto.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CKickDialog dialog


CKickDialog::CKickDialog(CWnd* pParent /*=NULL*/)
	: CCSDialog(CKickDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKickDialog)
	m_reason = _T("");
	m_strKick = _T("");
	m_bBanToo = FALSE;
	m_strBanPattern = _T("");
	//}}AFX_DATA_INIT
}


void CKickDialog::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKickDialog)
	DDX_Text(pDX, IDC_REASON, m_reason);
	DDV_MaxChars(pDX, m_reason, MAX_INPUTLEN);
	DDX_Text(pDX, IDC_KICKMSG, m_strKick);
	DDX_Check(pDX, IDC_BANTOO, m_bBanToo);
	DDX_Text(pDX, IDC_BANTOO_NAME, m_strBanPattern);
	//}}AFX_DATA_MAP
}


BOOL CKickDialog::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	
	GetDlgItem(IDC_BANTOO_NAME)->EnableWindow(m_bBanToo);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


const DWORD CKickDialog::m_nHelpIDs[] =
{
	IDC_REASON,			IDH_KICK_COMMENT,
	IDC_BANTOO,			IDH_ALSO_BAN,
	IDC_BANTOO_NAME,	IDH_ALSO_BAN,
	0, 0
};


BEGIN_MESSAGE_MAP(CKickDialog, CCSDialog)
	//{{AFX_MSG_MAP(CKickDialog)
	ON_BN_CLICKED(IDC_BANTOO, OnBantoo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKickDialog message handlers
/////////////////////////////////////////////////////////////////////////////
void CKickDialog::OnBantoo() 
{
	m_bBanToo = !m_bBanToo;
	GetDlgItem(IDC_BANTOO_NAME)->EnableWindow(m_bBanToo);
}



/////////////////////////////////////////////////////////////////////////////
// CBanDlg dialog

CBanDlg::CBanDlg(CWnd* pParent /*=NULL*/)
	: CCSDialog(CBanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBanDlg)
	m_strMesg = _T("");
	m_strBanPattern = _T("");
	m_szEncodedChannel = NULL;
	//}}AFX_DATA_INIT
}


void CBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBanDlg)
	DDX_Control(pDX, IDC_BANNED_USERS, m_ctlBans);
	DDX_Text(pDX, IDC_BANMESG, m_strMesg);
	DDX_CBString(pDX, IDC_BANNED_USERS, m_strBanPattern);
	//}}AFX_DATA_MAP
}


const DWORD CBanDlg::m_nHelpIDs[] =
{
	IDC_BANNED_USERS,	IDH_NICK_BAN,
	IDBAN,				IDH_BAN,
	IDUNBAN,			IDH_UNBAN,
	0, 0
};


BEGIN_MESSAGE_MAP(CBanDlg, CCSDialog)
	//{{AFX_MSG_MAP(CBanDlg)
	ON_CBN_EDITCHANGE(IDC_BANNED_USERS, OnEditChange)
	ON_BN_CLICKED(IDBAN, OnBan)
	ON_BN_CLICKED(IDUNBAN, OnUnban)
	ON_CBN_SELCHANGE(IDC_BANNED_USERS, OnSelchange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBanDlg message handlers
/////////////////////////////////////////////////////////////////////////////
void CBanDlg::DoBan(BOOL bBan) 
{
	CString strBanPattern;
	GetDlgItem(IDC_BANNED_USERS)->GetWindowText(strBanPattern);
	strBanPattern.TrimLeft();
	if (!strBanPattern.IsEmpty())
	{
		if (currentRoom)
		{
			currentRoom->ChatBanUser(strBanPattern, bBan, m_szEncodedChannel);	// strBanPattern is decoded mask
			if (bBan)
				m_ctlBans.AddString(strBanPattern);
			else
			{
				int iIndex = m_ctlBans.FindStringExact(-1, strBanPattern);
				if (iIndex != LB_ERR)
					m_ctlBans.DeleteString(iIndex);
			}
		}
	}
}


void CBanDlg::OnBan() 
{
	DoBan(TRUE);
	GetDlgItem(IDUNBAN)->EnableWindow(TRUE);
	GotoDlgCtrl(GetDlgItem(IDBAN));
	NextDlgCtrl();
	GetDlgItem(IDBAN)->EnableWindow(FALSE);
}


void CBanDlg::OnUnban() 
{
	DoBan(FALSE);
	GetDlgItem(IDBAN)->EnableWindow(TRUE);
	GetDlgItem(IDUNBAN)->EnableWindow(FALSE);
	GotoDlgCtrl(GetDlgItem(IDC_BANNED_USERS));
}


void CBanDlg::OnEditChange()
{
	CString strBanPattern;
	GetDlgItem(IDC_BANNED_USERS)->GetWindowText(strBanPattern);
	strBanPattern.TrimLeft();
	if (strBanPattern.IsEmpty())
	{
		GetDlgItem(IDBAN)->EnableWindow(FALSE);
		GetDlgItem(IDUNBAN)->EnableWindow(FALSE);
		return;
	}
	int index = m_ctlBans.FindStringExact(-1, strBanPattern);
	BOOL found = (index > -1);
	GetDlgItem(IDBAN)->EnableWindow(!found);
	GetDlgItem(IDUNBAN)->EnableWindow(found);
}


BOOL CBanDlg::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	
	m_ctlBans.SetFont(&theApp.m_fontGui);

	int upper = m_banArray->GetUpperBound();
	for (int i = 0; i <= upper; i++)
		m_ctlBans.AddString(DecodeNick(m_banArray->GetAt(i)));
	
	OnEditChange();	// Do initial enabling of buttons
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void CBanDlg::OnSelchange() 
{
	GetDlgItem(IDBAN)->EnableWindow(FALSE);
	GetDlgItem(IDUNBAN)->EnableWindow(TRUE);	
}


// CInviteDlg dialog

CInviteDlg::CInviteDlg(CWnd* pParent /*=NULL*/)
	: CCSDialog(CInviteDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInviteDlg)
	m_strInvitees = _T("");
	//}}AFX_DATA_INIT
}


void CInviteDlg::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInviteDlg)
	DDX_Text(pDX, IDC_INVITEES, m_strInvitees);
	DDV_MaxChars(pDX, m_strInvitees, 255);
	//}}AFX_DATA_MAP
}


const DWORD CInviteDlg::m_nHelpIDs[] =
{
	IDC_INVITEES,		IDH_NICK_INVITE,
	0, 0
};


BEGIN_MESSAGE_MAP(CInviteDlg, CCSDialog)
	//{{AFX_MSG_MAP(CInviteDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInviteDlg message handlers


/////////////////////////////////////////////////////////////////////////////
// CInvitationDlg dialog


CInvitationDlg::CInvitationDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInvitationDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInvitationDlg)
	m_bIgnore = FALSE;
	m_strMessage = _T("");
	//}}AFX_DATA_INIT
}


void CInvitationDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInvitationDlg)
	DDX_Check(pDX, IDC_IGNORE, m_bIgnore);
	DDX_Text(pDX, IDC_MESSAGE, m_strMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInvitationDlg, CDialog)
	//{{AFX_MSG_MAP(CInvitationDlg)
	ON_BN_CLICKED(IDNO, OnNo)
	ON_BN_CLICKED(IDYES, OnYes)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInvitationDlg message handlers

void CInvitationDlg::OnNo() 
{
	UpdateData(TRUE);	// for m_bIgnore
	EndDialog(IDNO);	
}

void CInvitationDlg::OnYes() 
{
	UpdateData(TRUE);	// for m_bIgnore
	EndDialog(IDYES);
}

BOOL CInviteDlg::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	
	GetDlgItem(IDC_INVITEES)->SetFont(&theApp.m_fontGui);
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
