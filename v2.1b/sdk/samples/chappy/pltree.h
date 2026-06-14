//****************************************************************************
//  Module:     Chappy.EXE
//  File:       PLTree.h : header file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************
#ifndef __PLTREE_H__
#define __PLTREE_H__

//////////////////////////////////////////////
// CPartListTree window

#define PART_LIST_MENU		1

class CPartListTree : public CTreeCtrl
{
// Construction
public:
	int GetCx();
	CPartListTree();

// Attributes
private:
	HTREEITEM m_iSelectedItem;

// Operations
public:
	inline HTREEITEM GetSelItem() { return m_iSelectedItem;};


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPartListTree)
	//}}AFX_VIRTUAL

// Implementation
public:
	HTREEITEM FindItemFromText(LPCSTR lpszItemText_p);
	void DeleteItemFromText(LPCSTR lpszItemText_p);
	virtual ~CPartListTree();

	// Generated message map functions
protected:
	//{{AFX_MSG(CPartListTree)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
private:
	int m_cx;
};

/////////////////////////////////////////////////////////////////////////////

#endif //__PLTREE_H__
