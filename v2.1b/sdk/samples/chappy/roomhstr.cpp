//****************************************************************************
//  Module:     Chappy.EXE
//  File:       RoomHstr.cpp : implementation file
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
#include "RoomHstr.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoomHistory

CRoomHistory::CRoomHistory()
{
}

CRoomHistory::~CRoomHistory()
{
}


BEGIN_MESSAGE_MAP(CRoomHistory, CRichEditCtrl)
	//{{AFX_MSG_MAP(CRoomHistory)
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomHistory message handlers

void CRoomHistory::OnRButtonDown(UINT nFlags, CPoint point) 
{
	{		
		CMenu menu;
		CMenu *submenu;
		//Load the menu.
		menu.LoadMenu(IDR_POPUP);
		//Get the pop-up menu.
		submenu = menu.GetSubMenu(ROOM_HISTORY_MENU);
		
		CPoint pt(point);
		//Convert to screen coordinates.
		ClientToScreen(&pt);
		//Post the menu.
		submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
				pt.x,    pt.y,
				AfxGetApp()->m_pMainWnd,NULL);
	}	
	
	CRichEditCtrl::OnRButtonDown(nFlags, point);
}

int CRoomHistory::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CHistory::OnCreate(lpCreateStruct) == -1)
		return -1;
		
	return 0;
}
