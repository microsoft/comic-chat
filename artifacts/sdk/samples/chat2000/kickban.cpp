//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       CKickBanDlg.cpp : implementation file
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
#include "KickBan.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKickBanDlg dialog


CKickBanDlg::CKickBanDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKickBanDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKickBanDlg)
	m_Comment = _T("");
	//}}AFX_DATA_INIT
}


void CKickBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKickBanDlg)
	DDX_Text(pDX, IDC_COMMENT, m_Comment);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKickBanDlg, CDialog)
	//{{AFX_MSG_MAP(CKickBanDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKickBanDlg message handlers
