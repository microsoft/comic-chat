//****************************************************************************
//  Module:     Chappy.EXE
//  File:       InputRE.cpp : implementation file
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
#include "InputRE.h"
#include "ChppyVw.h"
#include "Globals.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CInputREdit

CInputREdit::CInputREdit()
{
	m_bIsCtrlDown = FALSE;
}

CInputREdit::~CInputREdit()
{
}


BEGIN_MESSAGE_MAP(CInputREdit, CRichEditCtrl)
	//{{AFX_MSG_MAP(CInputREdit)
	ON_WM_KEYUP()
	ON_WM_KEYDOWN()
	ON_WM_RBUTTONDOWN()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CInputREdit message handlers

void CInputREdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE3 ("KEY_DOWN: %u %u %u\n",nChar, nRepCnt, nFlags);

	switch (nChar)
	{
		case CTRL_KEY:
			m_bIsCtrlDown = TRUE;
			break;

		case VK_RETURN:
			//-- RETURN == SEND
			//-- CTRL+RETURN == new line
			//catch the KEY_DOWN because KEY_UP it would not work with StickKeys turned on
			// series of events with StickKeys
			//		CTRL DOWN
			//			ENTER DOWN
			//		CTRL UP
			//		ENTER UP  <-- seams that we are not pressing CTRL
			if (m_bIsCtrlDown) break;

			TRACE0 ("-- ENTER -- \n");

			//Post the message in the chat room
			((CChappyView *)GetParent())->PostChatMessage(msgtNormal);

			//exit function without calling the parent OnKeyDown()
			break;
			
		default:
			break;
	}
	CRichEditCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}

void CInputREdit::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	TRACE3 ("KEY_UP: %u %u %u\n",nChar, nRepCnt, nFlags);

	switch (nChar)
	{
		case VK_RETURN:
			if (m_bIsCtrlDown) break;
			//clear the control
			SetSel(0,-1);
			Clear(); 
			break;

		case CTRL_KEY:
			m_bIsCtrlDown = FALSE;

		default:
			break;
	}
			
	CRichEditCtrl::OnKeyUp(nChar, nRepCnt, nFlags);
}

void CInputREdit::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPoint pt(point);
	ClientToScreen(&pt);
	PopMenu (INPUT_MENU, pt);

	
	CRichEditCtrl::OnRButtonDown(nFlags, point);
}


int CInputREdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CREdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	return 0;
}
