//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       EnterDlg.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

/////////////////////////////////////////////////////////////////////////////
// CEnterDlg dialog

class CEnterDlg : public CDialog
{
// Construction
public:
	CEnterDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CEnterDlg)
	enum { IDD = IDD_CHAT_CONNECT };
	CString	m_szNickName;
	CString	m_szPassword;
	CString	m_szRoom;
	//}}AFX_DATA

	//we are going to handle the check button manually
	short	m_iFlags;
	short	m_iTypes;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEnterDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CEnterDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
