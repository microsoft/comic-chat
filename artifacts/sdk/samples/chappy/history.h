//****************************************************************************
//  Module:     Chappy.EXE
//  File:       History.H
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef __HISTORY_H__
#define __HISTORY_H__

#include "REdit.h"

/////////////////////////////////////////////////////////////////////////////
// CChatRoom window

#define HISTORY_MENU	0

class CHistory : public CREdit
{
// Construction
public:
	CHistory();

// Attributes
public:
	void UpdateMessageUI(LPCTSTR SenderNickname_p, long iMsgType_p,  CString szMessage_p);
	void OnFileSave();


// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHistory)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ClearAll();
	virtual ~CHistory();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHistory)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnSelectFont();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif //__HISTORY_H__
