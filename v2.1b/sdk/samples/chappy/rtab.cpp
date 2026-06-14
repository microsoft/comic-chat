//****************************************************************************
//  Module:     Chappy.EXE
//  File:       RTab.cpp : implementation file
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
#include "RTab.h"
#include "globals.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoomsTab

CRoomsTab::CRoomsTab()
{
}

CRoomsTab::~CRoomsTab()
{
}


BEGIN_MESSAGE_MAP(CRoomsTab, CTabCtrl)
	//{{AFX_MSG_MAP(CRoomsTab)
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomsTab message handlers

int CRoomsTab::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_ImageList.Create(16,16,TRUE,0,6);

	m_ImageList.Add (LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_EARTH)));
	m_ImageList.Add (LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_IMPORTANT)));
	m_ImageList.Add (LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_SAY)));
	m_ImageList.Add (LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_WHISPER)));
	m_ImageList.Add (LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_DO)));
	m_ImageList.Add (LoadIcon(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDI_ROOM_LIST)));

	SetImageList ( &m_ImageList);
	
	return 0;
}


void CRoomsTab::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CTabCtrl::OnRButtonDown(nFlags, point);

	
}

void CRoomsTab::RenameTab(long iTab_p, CString & szName_p)
{
	TC_ITEM tcItem;
	tcItem.mask = TCIF_TEXT;

	char szTabTitle[256];
	sprintf(szTabTitle, "%s",(LPCTSTR) szName_p);
	tcItem.pszText = szTabTitle;

	int ret = SetItem(iTab_p,&tcItem);
	ASSERT (ret);

}

void CRoomsTab::ChangeImage(long iTab_p, long iImageIndex_p)
{
	TC_ITEM tcItem;
	tcItem.mask = TCIF_IMAGE;

	tcItem.iImage = iImageIndex_p;

	int ret = SetItem(iTab_p,&tcItem);
	ASSERT (ret);

}

void CRoomsTab::SetCurrentTab(long iTab_p)
{
	SetCurSel (iTab_p);
	//since SetCurSel does not notify us that we changed the tab...
	
	m_nmhdr.hwndFrom	= GetSafeHwnd();
	m_nmhdr.idFrom		= IDC_MESSAGE_TAB; 
	m_nmhdr.code		= TCN_SELCHANGE; 

	SendMessage(WM_NOTIFY, IDC_MESSAGE_TAB, (LPARAM) &m_nmhdr);
}


