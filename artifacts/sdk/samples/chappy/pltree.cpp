//****************************************************************************
//  Module:     Chappy.EXE
//  File:       PLTree.cpp : implementation file
//              
//
//  Copyright (c) Microsoft Corporation 1996-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#include "stdafx.h"
#include "Chappy.h"
#include "PLTree.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPartListTree

CPartListTree::CPartListTree()
{
	m_iSelectedItem = NULL;
}

CPartListTree::~CPartListTree()
{
}


BEGIN_MESSAGE_MAP(CPartListTree, CTreeCtrl)
	//{{AFX_MSG_MAP(CPartListTree)
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPartListTree message handlers
void CPartListTree::OnSize(UINT nType, int cx, int cy) 
{
	m_cx = cx + 8;	
	
	CTreeCtrl::OnSize(nType, cx, cy);
	
	//TRACE1("CPartListTree::OnSize = %u\n", m_cx);
	
}

int CPartListTree::GetCx()
{
	return m_cx;
}

void CPartListTree::DeleteItemFromText(LPCTSTR lpszItemText_p)
{
	HTREEITEM hItem = FindItemFromText(lpszItemText_p);
	//delete all children
	HTREEITEM hChildItem = NULL;
	while (hChildItem = GetChildItem(hItem))
	{
		DeleteItem (hChildItem);
	}

	DeleteItem (hItem);
}

HTREEITEM CPartListTree::FindItemFromText(LPCTSTR lpszItemText_p)
{

	HTREEITEM hItem = GetFirstVisibleItem();

	while (hItem)	
	{
		CString szText =  GetItemText(hItem);
		if (szText == lpszItemText_p) 
		{
			return hItem;
		}
		
		 hItem = GetNextVisibleItem(hItem);
	}
	return NULL;

}

void CPartListTree::OnLButtonDown(UINT nFlags, CPoint point) 
{
	UINT iFlags;	 
	m_iSelectedItem = HitTest(point, &iFlags);

	HTREEITEM htTemp = GetParentItem(m_iSelectedItem);
	m_iSelectedItem = (htTemp) ? htTemp : m_iSelectedItem;
	
	CTreeCtrl::OnLButtonDown(nFlags, point);
}

void CPartListTree::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CTreeCtrl::OnRButtonDown(nFlags, point);

	CPoint pt(point);
	ClientToScreen(&pt);
	PopMenu(PART_LIST_MENU, pt);
	
}

