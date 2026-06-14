//****************************************************************************
//  Module:     Chappy.EXE
//  File:       BanDlg.h
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#if !defined(AFX_BANDLG_H__B8AF51DF_DD08_11D0_AD50_00C04FC29DE5__INCLUDED_)
#define AFX_BANDLG_H__B8AF51DF_DD08_11D0_AD50_00C04FC29DE5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CBanDlg dialog

class CBanDlg : public CDialog
{
// Construction
public:
	CBanDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBanDlg)
	enum { IDD = IDD_BAN };
	long	m_iDuration;
	CString	m_szReason;
	CString	m_szValue;
	int		m_iOperation;
	int		m_iProperty;
	int		m_iBan;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CBanDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BANDLG_H__B8AF51DF_DD08_11D0_AD50_00C04FC29DE5__INCLUDED_)
