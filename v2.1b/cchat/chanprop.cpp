// chanprop.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "setupdlg.h"  // for CNicknameEdit
#include "chanprop.h"
#include "userinfo.h"
#include "IrcProto.H"
#include "format.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CChannelProp dialog


CChannelProp::CChannelProp(CWnd* pParent /*=NULL*/)
	: CCSDialog(CChannelProp::IDD, pParent)
{
	DoMyInits();
}

CChannelProp::CChannelProp(UINT nidTemplate, CWnd* pParent)
: CCSDialog(nidTemplate, pParent) 
{
	DoMyInits();
}

void CChannelProp::DoMyInits() {
	//{{AFX_DATA_INIT(CChannelProp)
	m_bAuditorium	= FALSE;
	m_bSecret		= FALSE;
	m_bInviteOnly	= FALSE;
	m_bModerated	= FALSE;
	m_bPrivate		= FALSE;
	m_bTopicAnyone	= FALSE;
	m_uMaxParticipants = 0;
	m_bSetMax		= FALSE;
	m_bNoWhispers	= FALSE;
	m_strPassword	= _T("");
	m_bSetPassword	= FALSE;
	//}}AFX_DATA_INIT
}


void CChannelProp::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelProp)
	// DDX_Check(pDX, IDC_AUDITORIUM, m_bAuditorium);
	DDX_Control(pDX, IDC_TOPIC_RICHEDIT, m_rtfTopic);

	DDX_Check(pDX, IDC_HIDDEN,			m_bSecret);
	DDX_Check(pDX, IDC_INVITEONLY,		m_bInviteOnly);
	DDX_Check(pDX, IDC_MODERATED,		m_bModerated);
	DDX_Check(pDX, IDC_PRIVATE,			m_bPrivate);
	DDX_Check(pDX, IDC_TOPICANYONE,		m_bTopicAnyone);
	DDX_Check(pDX, IDC_SETMAX,			m_bSetMax);
	DDX_Check(pDX, IDC_NOWHISPERS,		m_bNoWhispers);
	DDX_Check(pDX, IDC_PASSWORD_GIVEN,	m_bSetPassword);
	DDX_Text(pDX,  IDC_MAXPARTICIPANTS,	m_uMaxParticipants);
	DDX_Text(pDX,  IDC_PASSWORD,		m_strPassword);

	DDV_MaxChars(pDX, m_strPassword, 20);
	DDV_MinMaxUInt(pDX, m_uMaxParticipants, 0, 10000);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChannelProp, CCSDialog)
	//{{AFX_MSG_MAP(CChannelProp)
	ON_BN_CLICKED(IDC_SETMAX, OnSetmax)
	ON_BN_CLICKED(IDC_HIDDEN, OnHidden)
	ON_BN_CLICKED(IDC_PRIVATE, OnPrivate)
	ON_BN_CLICKED(IDC_PASSWORD_GIVEN, OnSetPassword)
	ON_NOTIFY(EN_MSGFILTER, IDC_TOPIC_RICHEDIT, OnTopicFilter)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELP, OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelProp message handlers

BOOL CChannelProp::OnInitDialog() 
{
	CCSDialog::OnInitDialog();

	extern CUserInfo *puiSelf;
	
	// Enable / Disable controls
	BOOL bIsOperator = puiSelf->IsOperator();

	// Fill in the initial message
	CString strMesg;
	
	if (bIsOperator)
		strMesg.LoadString(IDS_CHANPROP_ADMIN);
	else if (m_bTopicAnyone)
		strMesg.LoadString(IDS_CHANPROP_TOPICONLY);
	else
		strMesg.LoadString(IDS_CHANPROP_NOTADMIN);

	GetDlgItem(IDC_CHANPROP_MESG)->SetWindowText(strMesg);

	// Enable / Disable controls
//	GetDlgItem(IDC_AUDITORIUM)->EnableWindow(bIsOperator && isIrcX);
	GetDlgItem(IDC_HIDDEN)->EnableWindow(bIsOperator);
	GetDlgItem(IDC_INVITEONLY)->EnableWindow(bIsOperator);
	GetDlgItem(IDC_MODERATED)->EnableWindow(bIsOperator);
	GetDlgItem(IDC_PRIVATE)->EnableWindow(bIsOperator);
	GetDlgItem(IDC_TOPICANYONE)->EnableWindow(bIsOperator);
	GetDlgItem(IDC_NOWHISPERS)->EnableWindow(bIsOperator && serverConn.m_bIrcXServer);

	GetDlgItem(IDC_SETMAX)->EnableWindow(bIsOperator);
	GetDlgItem(IDC_MAXPARTICIPANTS)->EnableWindow(m_bSetMax && bIsOperator);

	GetDlgItem(IDC_PASSWORD_GIVEN)->EnableWindow(bIsOperator);
	GetDlgItem(IDC_PASSWORD)->EnableWindow(bIsOperator && m_bSetPassword);

	m_rtfTopic.EnableWindow(bIsOperator || m_bTopicAnyone);

	m_rtfTopic.UseDefaultCharFormat();
	m_rtfTopic.LimitText(MAX_TOPICLEN);
	m_rtfTopic.SetAcceptMultiLine(FALSE);
	m_rtfTopic.bSetWindowFormattedText(m_rtfTopic.m_strText, m_rtfTopic.m_prgdwFormatting);

	GetDlgItem(IDC_PASSWORD)->SetFont(&theApp.m_fontGui);

	// Need to add the EN_MSGFILTER notification to the dwEventMask of the rich text control
	DWORD dwEventMask = (DWORD) m_rtfTopic.GetEventMask();
	m_rtfTopic.SetEventMask(dwEventMask | ENM_MOUSEEVENTS);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChannelProp::OnSetmax() 
{
	m_bSetMax = !m_bSetMax;
	GetDlgItem(IDC_MAXPARTICIPANTS)->EnableWindow(m_bSetMax);
}

void CChannelProp::OnOK()
{
	if (m_rtfTopic.m_pFont)
	{
		if (m_rtfTopic.m_prgdwFormatting)
		{
			m_rtfTopic.m_prgdwFormatting->RemoveAll();
			delete m_rtfTopic.m_prgdwFormatting;
		}
		m_rtfTopic.m_prgdwFormatting = PRGDWGetFormatting(&m_rtfTopic, m_rtfTopic.m_pFont, m_rtfTopic.m_crTextColor);
	}

	m_rtfTopic.GetWindowText(m_rtfTopic.m_strText); 

	CCSDialog::OnOK();
}

void CChannelProp::OnTopicFilter(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct);

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfTopic.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


//LONG CChannelProp::OnHelp(UINT, LONG lParam) {
//	::WinHelp((HWND)((LPHELPINFO)lParam)->hItemHandle, AfxGetApp()->m_pszHelpFilePath,
//		HELP_WM_HELP, (DWORD)(LPVOID)GetHelpIDs());
//	return 0;
//}

#include "mschat.h"
#include "resource.h"

const DWORD CChannelProp::m_nHelpIDs[] =
{
//	IDC_AUDITORIUM,		IDH_AUDITORIUM,
	IDC_TOPIC_RICHEDIT,	IDH_TOPIC,
	IDC_MODERATED,		IDH_MODERATED,
	IDC_TOPICANYONE,	IDH_SET_TOPIC,
	IDC_INVITEONLY,		IDH_INVITE,
	IDC_SETMAX,			IDH_SET_MAX,
	IDC_MAXPARTICIPANTS,IDH_SET_MAX,
	IDC_HIDDEN,			IDH_HIDDEN,
	IDC_PRIVATE,		IDH_PRIVATE,
//	IDC_NOWHISPERS,		IDH_NO_WHISPERS,
	IDC_CHANNELNAME,	IDH_CREATEROOM_CHATROOM_NAME,
	IDC_PASSWORD_GIVEN, IDH_CREATEROOM_OPTIONAL_PASSWORD,
	IDC_PASSWORD,		IDH_CREATEROOM_OPTIONAL_PASSWORD,
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CChannelCreateDlg dialog


CChannelCreateDlg::CChannelCreateDlg(CWnd* pParent /*=NULL*/)
	: CChannelProp(CChannelCreateDlg::IDD, pParent)
{
	CChannelProp::DoMyInits();
	//{{AFX_DATA_INIT(CChannelCreateDlg)
	m_strChannelName = _T("");
	//}}AFX_DATA_INIT
}


void CChannelCreateDlg::DoDataExchange(CDataExchange* pDX)
{
	CChannelProp::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CChannelCreateDlg)
	DDX_Control(pDX, IDC_CHANNELNAME, m_channelCtl);
	DDX_Text(pDX, IDC_CHANNELNAME, m_strChannelName);
	DDV_MaxChars(pDX, m_strChannelName, 200);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CChannelCreateDlg, CChannelProp)
	//{{AFX_MSG_MAP(CChannelCreateDlg)
	ON_EN_CHANGE(IDC_CHANNELNAME, OnChangeChannelname)
	ON_NOTIFY(EN_MSGFILTER, IDC_TOPIC_RICHEDIT, OnTopicFilter)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChannelCreateDlg message handlers

BOOL CChannelCreateDlg::OnInitDialog() 
{
	CCSDialog::OnInitDialog();    // CChannelProp does inits we don't want

//	GetDlgItem(IDC_AUDITORIUM)->EnableWindow(isIrcX);
	GetDlgItem(IDC_NOWHISPERS)->EnableWindow(serverConn.m_bIrcXServer);
	GetDlgItem(IDC_MAXPARTICIPANTS)->EnableWindow(m_bSetMax);
	GetDlgItem(IDC_PASSWORD)->EnableWindow(m_bSetPassword);
	GetDlgItem(IDOK)->EnableWindow(!m_strChannelName.IsEmpty());
	
	m_rtfTopic.UseDefaultCharFormat();
	m_rtfTopic.LimitText(MAX_TOPICLEN);
	m_rtfTopic.SetAcceptMultiLine(FALSE);

	m_channelCtl.SetFont(&theApp.m_fontGui);

	// Need to add the EN_MSGFILTER notification to the dwEventMask of the rich text control
	DWORD dwEventMask = (DWORD) m_rtfTopic.GetEventMask();
	m_rtfTopic.SetEventMask(dwEventMask | ENM_MOUSEEVENTS);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CChannelProp::OnSetPassword() 
{
	m_bSetPassword = !m_bSetPassword;
	GetDlgItem(IDC_PASSWORD)->EnableWindow(m_bSetPassword);
}

void CChannelCreateDlg::OnChangeChannelname() 
{
	CString strTemp;
	m_channelCtl.GetWindowText(strTemp);
	GetDlgItem(IDOK)->EnableWindow(!strTemp.IsEmpty());
}

void CChannelProp::OnHidden() 
{
	((CButton *)GetDlgItem(IDC_PRIVATE))->SetCheck(FALSE); // hidden & private mutually exclusive	
}

void CChannelProp::OnPrivate() 
{
	((CButton *)GetDlgItem(IDC_HIDDEN))->SetCheck(FALSE);	// hidden & private mutually exclusive	
}
