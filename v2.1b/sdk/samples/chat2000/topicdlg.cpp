//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       TopicDlg.cpp : implementation file
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
#include "TopicDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTopicDlg dialog


CTopicDlg::CTopicDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CTopicDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTopicDlg)
	m_szTopic = _T("");
	//}}AFX_DATA_INIT
}


void CTopicDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTopicDlg)
	DDX_Text(pDX, IDC_TOPIC, m_szTopic);
	DDV_MaxChars(pDX, m_szTopic, 95);  //room topic max size is 95 chars
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTopicDlg, CDialog)
	//{{AFX_MSG_MAP(CTopicDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTopicDlg message handlers
