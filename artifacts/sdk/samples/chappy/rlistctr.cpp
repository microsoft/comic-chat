//****************************************************************************
//  Module:     Chappy.EXE
//  File:       RListCtrl.cpp : implementation file 
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
#include "chappy.h"
#include "RListCtr.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#include "ChppyVw.h"

#define	ROOM_LIST_MENU	5

/////////////////////////////////////////////////////////////////////////////
// CRoomListCtrl

CRoomListCtrl::CRoomListCtrl()
{
	m_iSelectedItem = -1;
	m_iNumOfItems	= -1;
	m_iNumOfColumns = 0;
}

CRoomListCtrl::~CRoomListCtrl()
{
}


BEGIN_MESSAGE_MAP(CRoomListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CRoomListCtrl)
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomListCtrl message handlers

void CRoomListCtrl::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CListCtrl::OnRButtonDown(nFlags, point);

	CPoint pt(point);
	ClientToScreen(&pt);
	PopMenu(ROOM_LIST_MENU, pt);

}

void CRoomListCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_iSelectedItem = HitTest(point);
	
	CListCtrl::OnLButtonDown(nFlags, point);
}

void CRoomListCtrl::InsertNextItem(CString &szItem_p)
{
	m_iNumOfItems++;
	m_iNumOfItems = InsertItem(m_iNumOfItems, szItem_p);
	ASSERT (-1 != m_iNumOfItems);
}

void CRoomListCtrl::AddSubItem(long iSubItemIndex_p, CString &szItem_p)
{
	LV_ITEM ItemData;
	ItemData.mask		= LVIF_TEXT;
	ItemData.iItem		= m_iNumOfItems;
	ItemData.iSubItem	= iSubItemIndex_p;
	ItemData.pszText	= (LPSTR) (LPCSTR) szItem_p;
	BOOL ret = SetItem (&ItemData);
	ASSERT (ret) ;
}

void CRoomListCtrl::ClearList()
{
	DeleteAllItems();
	m_iNumOfItems = -1;

	for (; m_iNumOfColumns >= 0; m_iNumOfColumns--)
	{
		DeleteColumn(m_iNumOfColumns);
	}
	m_iNumOfColumns = 0;
}

void CRoomListCtrl::AddColumn(CString & szColumnName_p)
{
	//set the colums
	LV_COLUMN ColData;
	ColData.mask	= LVCF_TEXT | LVCF_FMT | LVCF_WIDTH;
	
	ColData.fmt		= LVCFMT_LEFT;
	ColData.cx		= 200;
	ColData.pszText	= (LPSTR) (LPCSTR) szColumnName_p;

	
	int iCol =InsertColumn(m_iNumOfColumns, &ColData);
	ASSERT (m_iNumOfColumns == iCol); 
	m_iNumOfColumns++;
	
}
