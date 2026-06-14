//****************************************************************************
//  Module:     Chappy.EXE
//  File:       CLogInDlg.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#if !defined(AFX_CLogInDlg_H__1D7CE41F_B542_11D0_AD41_00C04FC29DE5__INCLUDED_)
#define AFX_CLogInDlg_H__1D7CE41F_B542_11D0_AD41_00C04FC29DE5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CLogInDlg dialog

class CLogInDlg : public CDialog
{
// Construction
public:
	CLogInDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLogInDlg)
	enum { IDD = IDD_LOG_IN_DLG };
	CString	m_szPassword;
	CString	m_szServerName;
	CString	m_szNickname;
	BOOL	m_bShowRoomList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLogInDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLogInDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CLogInDlg_H__1D7CE41F_B542_11D0_AD41_00C04FC29DE5__INCLUDED_)
