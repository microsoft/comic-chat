// motd.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "motd.h"
#include "format.h"
#include "ircproto.h"
#include "mschat.h"
#include "ui.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMOTD dialog


CMOTD::CMOTD(CWnd* pParent /*=NULL*/)
	: CCSDialog(CMOTD::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMOTD)
	m_bShowMOTD = FALSE;
	//}}AFX_DATA_INIT
}


void CMOTD::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMOTD)
	DDX_Check(pDX, IDC_SHOW_MOTD, m_bShowMOTD);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMOTD, CCSDialog)
	//{{AFX_MSG_MAP(CMOTD)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMOTD message handlers

void SetFlag (DWORD &dw, DWORD mask, BOOL value) {
	if (value) dw |= mask;
	else dw &= ~mask;
}

void ShowMOTD(const char *luser, const char *motd) {
	extern int g_iQuery;
	if (g_iQuery == QT_MOTD || (theApp.m_flags1 & F1_SHOWMOTD)) {
		CMOTD motdDlg;
		motdDlg.m_strLUSER = luser;
		motdDlg.m_strMOTD = motd;
		motdDlg.m_bShowMOTD = ISTRUE(theApp.m_flags1 & F1_SHOWMOTD);
		motdDlg.DoModal();
		SetFlag(theApp.m_flags1, F1_SHOWMOTD, motdDlg.m_bShowMOTD);
		g_iQuery = QT_NONE;
	}
}


BOOL CMOTD::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	
	RECT editDims;
	GetDlgItem(IDC_EDITPOS)->GetWindowRect(&editDims);
	ScreenToClient(&editDims);
	VERIFY(m_edit.Create(WS_CHILD | WS_VISIBLE | ES_MULTILINE | WS_VSCROLL | WS_BORDER | ES_READONLY, editDims, this, 5 ));
	CHARFORMAT cf;
	cf.cbSize = sizeof(cf);
	cf.dwMask = CFM_COLOR;
	cf.crTextColor = RGB(0,0,255);
	m_edit.SetSelectionCharFormat(cf);
	m_edit.ReplaceSel(m_strLUSER);
	if (!m_strLUSER.IsEmpty()) m_edit.ReplaceSel("\n");
	cf.crTextColor = RGB(0,0,0);
	m_edit.SetSelectionCharFormat(cf);
	m_edit.ReplaceSel(m_strMOTD);
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


const DWORD CMOTD::m_nHelpIDs[] =
{
	IDC_EDITPOS,			IDH_VIEW_MESSAGEOFDAY,
	IDC_SHOW_MOTD,			IDH_VIEW_MSGOFDAY_ONCONNECT,
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CAwayDlg dialog

CAwayDlg::CAwayDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAwayDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CAwayDlg)
	//}}AFX_DATA_INIT
}


void CAwayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAwayDlg)
	DDX_Control(pDX, IDC_AWAYMSG, m_rtfAwayMsg);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAwayDlg, CDialog)
	//{{AFX_MSG_MAP(CAwayDlg)
	ON_NOTIFY(EN_SELCHANGE, IDC_AWAYMSG, OnChangeAwayMsg)
	ON_NOTIFY(EN_MSGFILTER, IDC_AWAYMSG, OnFilterAwayMsg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAwayDlg message handlers

void CAwayDlg::OnChangeAwayMsg(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	CString strMesg;
	CWnd *msgWin = GetDlgItem(IDC_AWAYMSG);
	msgWin->GetWindowText(strMesg);
	strMesg.TrimLeft();
	GetDlgItem(IDOK)->EnableWindow(!strMesg.IsEmpty());

	*plResult = 0L;
}


void CAwayDlg::OnFilterAwayMsg(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct);

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfAwayMsg.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


BOOL CAwayDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	m_rtfAwayMsg.UseDefaultCharFormat();
	m_rtfAwayMsg.bSetTextColor(m_rtfAwayMsg.m_crTextColor);
	m_rtfAwayMsg.LimitText(MAX_INPUTLEN);
	m_rtfAwayMsg.bSetWindowFormattedText(m_rtfAwayMsg.m_strText, m_rtfAwayMsg.m_prgdwFormatting);

	// Need to add the EN_SELCHANGE notification to the dwEventMask of the rich text control
	DWORD dwEventMask = (DWORD) m_rtfAwayMsg.GetEventMask();
	m_rtfAwayMsg.SetEventMask(dwEventMask | ENM_SELCHANGE | ENM_MOUSEEVENTS);

	return TRUE;
}

void CAwayDlg::OnOK()
{
	if (m_rtfAwayMsg.m_pFont)
	{
		if (m_rtfAwayMsg.m_prgdwFormatting)
		{
			m_rtfAwayMsg.m_prgdwFormatting->RemoveAll();
			delete m_rtfAwayMsg.m_prgdwFormatting;
		}
		m_rtfAwayMsg.m_prgdwFormatting = PRGDWGetFormatting(&m_rtfAwayMsg, m_rtfAwayMsg.m_pFont, m_rtfAwayMsg.m_crTextColor);
	}

	m_rtfAwayMsg.GetWindowText(m_rtfAwayMsg.m_strText); 

	CDialog::OnOK();
}

