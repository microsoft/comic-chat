//****************************************************************************
//  Module:     Chappy.EXE
//  File:       InputRE.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef __INPUTRE_H__
#define __INPUTRE_H__

#include "REdit.h"

#define CTRL_KEY		17
#define INPUT_MENU		2

/////////////////////////////////////////////////////////////////////////////
// CInputREdit window

class CInputREdit : public CREdit
{
// Construction
public:
	CInputREdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInputREdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CInputREdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CInputREdit)
	afx_msg void OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	BOOL m_bIsCtrlDown;
};

/////////////////////////////////////////////////////////////////////////////

#endif //__INPUTRE_H__
