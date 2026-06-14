//****************************************************************************
//  Module:     Chappy.EXE
//  File:       REdit.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************
#if !defined(AFX_REDIT_H__5C77B63F_BC3A_11D0_AD48_00C04FC29DE5__INCLUDED_)
#define AFX_REDIT_H__5C77B63F_BC3A_11D0_AD48_00C04FC29DE5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000


/////////////////////////////////////////////////////////////////////////////
// CREdit window

class CREdit : public CRichEditCtrl
{
// Construction
public:
	CREdit();

// Attributes
public:
	void OnFocus		(BOOL bFocus_p);
	void TogleItalicText(BOOL bIsItalic_p);
	void SetTextColor	(COLORREF crColor_p);
	void TogleBoldText	(BOOL bIsBold_p = TRUE);
	void AddText		(CString &szTextIn_p, BOOL bBold_p = FALSE);
	void AddText		(LPCSTR szTextIn_p, BOOL bBold_p = FALSE);




// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CREdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CREdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CREdit)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_REDIT_H__5C77B63F_BC3A_11D0_AD48_00C04FC29DE5__INCLUDED_)
