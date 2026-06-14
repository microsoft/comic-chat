// BotherDlg.cpp : implementation file
//
#include "stdafx.h"
#include "chat.h"
#include "ui.h"
#include "BothDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBotherDlg dialog


CBotherDlg::CBotherDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBotherDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBotherDlg)
	m_bBother = GetChatApp()->m_bBother;
	//}}AFX_DATA_INIT
}


void CBotherDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBotherDlg)
	DDX_Check(pDX, IDC_BOTHER, m_bBother);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBotherDlg, CDialog)
	//{{AFX_MSG_MAP(CBotherDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////
// CBotherDlg message handlers

