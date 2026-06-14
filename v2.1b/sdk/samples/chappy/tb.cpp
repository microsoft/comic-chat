//****************************************************************************
//  Module:     Chappy.EXE
//  File:       TB.cpp : implementation file
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
#include "TB.h"
#include "ChppyVw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTB

CTB::CTB()
{

}

CTB::~CTB()
{
}


BEGIN_MESSAGE_MAP(CTB, CFlatToolbar)
	//{{AFX_MSG_MAP(CTB)
	ON_UPDATE_COMMAND_UI(ID_BTN_SEND, OnUpdateBtnSend)
	ON_UPDATE_COMMAND_UI(ID_BTN_WHISPER, OnUpdateBtnWhisper)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/*	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipNotify)
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipNotify)
*/

/////////////////////////////////////////////////////////////////////////////
// CTB message handlers

void CTB::OnUpdateBtnSend(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	
}

void CTB::OnUpdateBtnWhisper(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(TRUE);
	
}