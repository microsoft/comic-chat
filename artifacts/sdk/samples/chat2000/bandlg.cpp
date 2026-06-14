//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       BanDlg.cpp 
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#include "stdafx.h"
#include "Chat2000.h"
#include "BanDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CBanDlg dialog


CBanDlg::CBanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CBanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CBanDlg)
	m_Set = -1;
	m_Name = _T("");
	//}}AFX_DATA_INIT

	//the pointer to the list is NULL on costruction
	m_pListOfPart = NULL;
}


void CBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBanDlg)
	DDX_Radio(pDX, IDC_BANKICK, m_Set);
	DDX_CBString(pDX, IDC_NAME, m_Name);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBanDlg, CDialog)
	//{{AFX_MSG_MAP(CBanDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBanDlg message handlers

BOOL CBanDlg::OnInitDialog() 
{
	CComboBox *NameCombo = (CComboBox *) GetDlgItem(IDC_NAME);
	
	CString ParticipantName = GetParticipantAlias();
	while (!ParticipantName.IsEmpty())
	{
		int res = NameCombo->InsertString (-1,(LPCSTR)ParticipantName);
		ParticipantName = GetParticipantAlias();
	}
	
	UpdateData(FALSE); //put the data into the controls

	return 	CDialog::OnInitDialog();  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

CString CBanDlg::GetParticipantAlias()
{
	static POSITION CurrentPos = m_pListOfPart->GetHeadPosition();
	
	if (CurrentPos != NULL)
	{
		//return the alias, set the position to the next element of the list
		return (m_pListOfPart->GetNext(CurrentPos)).GetAlias();
	}
	else
	{
		//we reached the end of the list or the list is empty
		//    go back to the head
		CurrentPos = m_pListOfPart->GetHeadPosition();
		return _T("");
	}
}
