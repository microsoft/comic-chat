//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       InfoDlg.cpp : implementation file
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
#include "chat2000.h"
#include "InfoDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg dialog


CInfoDlg::CInfoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CInfoDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CInfoDlg)
	m_szMaxNumOfMembers = _T("");
	m_szNumberOfMembers = _T("");
	m_szAlias = _T("");
	m_szID = _T("");
	m_szPath = _T("");
	m_szRatings = _T("");
	m_szTopic = _T("");
	//}}AFX_DATA_INIT
}


void CInfoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CInfoDlg)
	DDX_Text(pDX, IDC_MAX_NUMBER_OF_MEMBERS, m_szMaxNumOfMembers);
	DDX_Text(pDX, IDC_NUMBER_OF_MEMBERS, m_szNumberOfMembers);
	DDX_Text(pDX, IDC_PARTICIPANT_ALIAS, m_szAlias);
	DDX_Text(pDX, IDC_PARTICIPANT_ID, m_szID);
	DDX_Text(pDX, IDC_PATH, m_szPath);
	DDX_Text(pDX, IDC_Ratings, m_szRatings);
	DDX_Text(pDX, IDC_TOPIC, m_szTopic);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CInfoDlg, CDialog)
	//{{AFX_MSG_MAP(CInfoDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInfoDlg message handlers
