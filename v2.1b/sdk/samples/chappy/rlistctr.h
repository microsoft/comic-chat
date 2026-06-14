//****************************************************************************
//  Module:     Chappy.EXE
//  File:       RListCtrl.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#if !defined(AFX_RLISTCTRL_H__3F0A490F_B78C_11D0_AD47_00C04FC29DE5__INCLUDED_)
#define AFX_RLISTCTRL_H__3F0A490F_B78C_11D0_AD47_00C04FC29DE5__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

 
/////////////////////////////////////////////////////////////////////////////
// CRoomListCtrl window

class CRoomListCtrl : public CListCtrl
{
// Construction
public:
	CRoomListCtrl();

// Attributes
private:
	long m_iSelectedItem;
	long m_iNumOfItems;
	long m_iNumOfColumns;

// Operations
public:
	inline long GetNumOfColumns() { return m_iNumOfColumns;};
	inline long GetSelectedItem() { return m_iSelectedItem;};
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	void AddColumn(CString &szColumnName_p);
	void ClearList();
	void AddSubItem (long iSubItemIndex_p, CString &szItem_p);
	void InsertNextItem (CString &szItem_p);
	
	virtual ~CRoomListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRoomListCtrl)
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RLISTCTRL_H__3F0A490F_B78C_11D0_AD47_00C04FC29DE5__INCLUDED_)
