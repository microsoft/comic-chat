//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       EnterDlg.cpp : implementation file
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
#include "EnterDlg.h"

#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEnterDlg dialog


CEnterDlg::CEnterDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEnterDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CEnterDlg)
	m_szNickName = _T("");
	m_szPassword = _T("");
	m_szRoom = _T("");
	//}}AFX_DATA_INIT

	m_iFlags = 0x00000;
	m_iTypes = 0x00000;

}


void CEnterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterDlg)
	DDX_Text(pDX, IDC_EDIT_NICKNAME, m_szNickName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_ROOM, m_szRoom);
	//}}AFX_DATA_MAP

	int	iControl = 0;
	if (pDX->m_bSaveAndValidate)
	{
		//we are getting the data from the controls

		m_iFlags = 0; //clean the variable from old settings
		//get the checked flags 
		for (iControl = IDC_CREATE; iControl <= IDC_AUTENTICATED_TALK; iControl++)
		{
			m_iFlags |= ((CButton *)GetDlgItem(iControl))->GetCheck() << (iControl - IDC_CREATE);
		}

		m_iTypes = 0; //clean the variable from old settings
		//get the checked types
		for (iControl = IDC_ANY_USER; iControl <= IDC_SPECTATOR; iControl++)
		{
			m_iTypes |= ((CButton *)GetDlgItem(iControl))->GetCheck() << (iControl - IDC_ANY_USER);
		}
	}
	else
	{
		//we are putting the data in the controls
		short iRest	= m_iFlags;

		//set the checked flags 
		for (iControl = IDC_CREATE; iControl <= IDC_AUTENTICATED_TALK; iControl++)
		{
			((CButton *)GetDlgItem(iControl))->SetCheck(iRest&0x1);
			iRest >>= 1; //pop the last decimal
		}
		//set the checked types 
		iRest	= m_iTypes;
		for (iControl = IDC_ANY_USER; iControl <= IDC_SPECTATOR; iControl++)
		{
			((CButton *)GetDlgItem(iControl))->SetCheck(iRest&0x1);
			iRest >>= 1; //pop the last decimal
		}
	}
}


BEGIN_MESSAGE_MAP(CEnterDlg, CDialog)
	//{{AFX_MSG_MAP(CEnterDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEnterDlg message handlers

BOOL CEnterDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	//default values for the controls
	m_szRoom.LoadString(IDS_DEFAULT_CHAT_ROOM);
	m_szNickName.LoadString(IDS_DEFAULT_NICKNAME);
	m_iFlags = ENTER_FLAG_JOIN_CREATE;
	m_iTypes = ENTER_TYPE_ANY_USER;
	
	
	UpdateData(FALSE); //put the data into the controls
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}