//****************************************************************************
//  Module:     Chappy.EXE
//  File:       Utility.cpp
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
#include "utility.h"

void PopMenu (long iSubMenu_p, CPoint pt)
{		
		CMenu menu;
		CMenu *submenu;
		//Load the menu.
		menu.LoadMenu(IDR_POPUP);
		//Get the pop-up menu.
		submenu = menu.GetSubMenu(iSubMenu_p);
		
		//Convert to screen coordinates.
		//Post the menu.
		submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
				pt.x,    pt.y,
				AfxGetApp()->m_pMainWnd,NULL);
}	
