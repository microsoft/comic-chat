//****************************************************************************
//  Module:     Chappy.EXE
//  File:       RTab.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#ifndef __RTAB_H__
#define __RTAB_H__

/////////////////////////////////////////////////////////////////////////////
// CRoomsTab window

#define ROOM_MENU		3

class CRoomsTab : public CTabCtrl
{
// Construction
public:
	CRoomsTab();

// Attributes
public:
	NMHDR m_nmhdr;
	CImageList	m_ImageList;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomsTab)
	//}}AFX_VIRTUAL

// Implementation
public:
	void ChangeImage(long iTab_p, long iImageIndex_p);
	void SetCurrentTab(long iTab_p);
	void RenameTab (long iTab_p, CString &szName_p);
	virtual ~CRoomsTab();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRoomsTab)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

#endif //__RTAB_H__
