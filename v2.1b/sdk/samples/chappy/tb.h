//****************************************************************************
//  Module:     Chappy.EXE
//  File:       TB.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef __TB_H__
#define __TB_H__
#include "FlatTB.h"

/////////////////////////////////////////////////////////////////////////////
// CTB window

class CTB : public CFlatToolbar
{
// Construction
public:
	CTB();

// Attributes
public:

// Operations
public:

//	BOOL OnToolTipNotify( UINT id, NMHDR * pNMHDR, LRESULT * pResult );
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTB)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTB();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTB)
	afx_msg void OnUpdateBtnSend(CCmdUI* pCmdUI);
	afx_msg void OnUpdateBtnWhisper(CCmdUI* pCmdUI);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif //__TB_H__
