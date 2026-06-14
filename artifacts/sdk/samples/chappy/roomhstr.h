//****************************************************************************
//  Module:     Chappy.EXE
//  File:       RoomHstr.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#if !defined(AFX_ROOMHSTR_H__5C77B642_BC3A_11D0_AD48_00C04FC29DE5__INCLUDED_)
#define AFX_ROOMHSTR_H__5C77B642_BC3A_11D0_AD48_00C04FC29DE5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "History.h"

/////////////////////////////////////////////////////////////////////////////
// CRoomHistory window

#define ROOM_HISTORY_MENU	4

class CRoomHistory : public CHistory
{
// Construction
public:
	CRoomHistory();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomHistory)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRoomHistory();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRoomHistory)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ROOMHSTR_H__5C77B642_BC3A_11D0_AD48_00C04FC29DE5__INCLUDED_)
