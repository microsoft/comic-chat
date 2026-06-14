//****************************************************************************
//  Module:     Chappy.EXE
//  File:       KickDlg.cpp : implementation file
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
#include "KickDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CKickDlg dialog


CKickDlg::CKickDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CKickDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CKickDlg)
	m_szReason = _T("");
	//}}AFX_DATA_INIT
}


void CKickDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CKickDlg)
	DDX_Text(pDX, IDC_REASON, m_szReason);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CKickDlg, CDialog)
	//{{AFX_MSG_MAP(CKickDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CKickDlg message handlers

int CKickDlg::DoModal() 
{
	//SetFocus();	
	return CDialog::DoModal();
}

BOOL CKickDlg::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL bRet = CDialog::Create(IDD, pParentWnd);
	ASSERT (bRet);
	SetFocus();	
	return bRet;
}
