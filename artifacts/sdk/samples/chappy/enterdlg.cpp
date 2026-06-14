//****************************************************************************
//  Module:     Chappy.EXE
//  File:       EnterDlg.cpp : implementation file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************


#include "stdafx.h"
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
	//m_szNickName = _T("");
	m_szPassword = _T("");
	m_szRoom = _T("");
	//}}AFX_DATA_INIT
}


void CEnterDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEnterDlg)
	//DDX_Text(pDX, IDC_EDIT_NICKNAME, m_szNickName);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_szPassword);
	DDX_Text(pDX, IDC_EDIT_ROOM, m_szRoom);
	//}}AFX_DATA_MAP
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
	
	UpdateData(FALSE); //put the data into the controls
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}
