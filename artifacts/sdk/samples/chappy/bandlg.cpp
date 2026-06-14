//****************************************************************************
//  Module:     Chappy.EXE
//  File:       BanDlg.cpp : implementation file
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
	m_iDuration = 0;
	m_szReason = _T("");
	m_szValue = _T("");
	m_iOperation = -1;
	m_iProperty = -1;
	m_iBan = -1;
	//}}AFX_DATA_INIT
}


void CBanDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CBanDlg)
	DDX_Text(pDX, IDC_HOW_LONG, m_iDuration);
	DDV_MinMaxLong(pDX, m_iDuration, 1, 2147483647);
	DDX_Text(pDX, IDC_REASON, m_szReason);
	DDX_Text(pDX, IDC_VALUE, m_szValue);
	DDX_Radio(pDX, IDC_RB_NO_OP, m_iOperation);
	DDX_Radio(pDX, IDC_RB_IP, m_iProperty);
	DDX_Radio(pDX, IDC_RB_UNBAN, m_iBan);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CBanDlg, CDialog)
	//{{AFX_MSG_MAP(CBanDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CBanDlg message handlers
