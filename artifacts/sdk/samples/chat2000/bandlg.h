//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       BanDlg.h 
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#include "partic.h"

/////////////////////////////////////////////////////////////////////////////
// CBanDlg dialog

class CBanDlg : public CDialog
{
// Construction
public:
	CList <CParticipant, CParticipant&> *m_pListOfPart;

	CBanDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBanDlg)
	enum { IDD = IDD_BAN };
	int		m_Set;
	CString	m_Name;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	CString			GetParticipantAlias	();
	
	// Generated message map functions
	//{{AFX_MSG(CBanDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
