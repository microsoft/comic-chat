//****************************************************************************
//  Module:     Chappy.EXE
//  File:       PWordDlg.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#if !defined(AFX_PWORDDLG_H__723CE485_C028_11D0_AD49_00C04FC29DE5__INCLUDED_)
#define AFX_PWORDDLG_H__723CE485_C028_11D0_AD49_00C04FC29DE5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CPWordDlg dialog

class CPWordDlg : public CDialog
{
// Construction
public:
	CPWordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPWordDlg)
	enum { IDD = IDD_PASSWORD };
	CString	m_szPassword;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPWordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPWordDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PWORDDLG_H__723CE485_C028_11D0_AD49_00C04FC29DE5__INCLUDED_)
