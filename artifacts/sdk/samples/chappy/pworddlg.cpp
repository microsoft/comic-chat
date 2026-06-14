//****************************************************************************
//  Module:     Chappy.EXE
//  File:       PWordDlg.cpp : implementation file
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
#include "PWordDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPWordDlg dialog


CPWordDlg::CPWordDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPWordDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPWordDlg)
	m_szPassword = _T("");
	//}}AFX_DATA_INIT
}


void CPWordDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPWordDlg)
	DDX_Text(pDX, IDC_PASSWORD, m_szPassword);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPWordDlg, CDialog)
	//{{AFX_MSG_MAP(CPWordDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPWordDlg message handlers
