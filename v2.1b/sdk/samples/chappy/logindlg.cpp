//****************************************************************************
//  Module:     Chappy.EXE
//  File:       CLogInDlg.cpp : implementation file
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
#include "chappy.h"
#include "LogInDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CLogInDlg dialog


CLogInDlg::CLogInDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CLogInDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CLogInDlg)
	m_szPassword = _T("");
	m_szServerName = _T("");
	m_szNickname = _T("");
	m_bShowRoomList = FALSE;
	//}}AFX_DATA_INIT
}


void CLogInDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLogInDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_szPassword);
	DDX_CBString(pDX, IDC_SERVER, m_szServerName);
	DDX_Text(pDX, IDC_NICKNAME, m_szNickname);
	DDX_Check(pDX, IDC_ROOM_LIST_CHECK, m_bShowRoomList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLogInDlg, CDialog)
	//{{AFX_MSG_MAP(CLogInDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLogInDlg message handlers
