//****************************************************************************
//  Module:     Chappy.EXE
//  File:       REdit.cpp : implementation file
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
#include "REdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CREdit

CREdit::CREdit()
{
}

CREdit::~CREdit()
{
}


BEGIN_MESSAGE_MAP(CREdit, CRichEditCtrl)
	//{{AFX_MSG_MAP(CREdit)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// OnFocus(BOOL bFocus_p)
//  bFocus_p = TRUE we received focus
//	bFocus_p = FALSE we lost focus
void CREdit::OnFocus(BOOL bFocus_p)
{
	if (bFocus_p)
	{
		//we received focus
		//show the window
		ShowWindow(SW_SHOW);
	}
	else
	{
		//we lost focus
		//hide the window
		ShowWindow(SW_HIDE);
	}
}


void CREdit::AddText(LPCSTR szTextIn_p, BOOL bBold_p)
{
	TogleBoldText(bBold_p);
	//m_ctrlHistoryWnd.SetSel(0,-1);
	int iTextLength = GetWindowTextLength();
	SetSel(iTextLength,iTextLength);
	ReplaceSel(szTextIn_p);
	//m_ctrlHistoryWnd.LineScroll(1);

}

void CREdit::AddText(CString &szTextIn_p, BOOL bBold_p)
{
	//m_ctrlHistoryWnd.SetSel(0,-1);
	AddText((LPCSTR) szTextIn_p, bBold_p);
}

void CREdit::TogleBoldText(BOOL bIsBold_p)
{
	CHARFORMAT cf;
	
	cf.cbSize = sizeof (CHARFORMAT); 
	cf.dwMask=CFM_BOLD;
	cf.dwEffects = (bIsBold_p) ? CFE_BOLD : CFE_AUTOCOLOR;
	//cf.yHeight;
	//cf.yOffset;
	//cf.crTextColor=RGB(250,0,0);
	//cf.bCharSet;
	//cf.bPitchAndFamily;
	//sprintf(cf.szFaceName, "Arial");

	SetSelectionCharFormat(cf);
}

void CREdit::TogleItalicText(BOOL bIsItalic_p)
{
	CHARFORMAT cf;
	
	cf.cbSize = sizeof (CHARFORMAT); 
	cf.dwMask=CFM_ITALIC;
	cf.dwEffects = (bIsItalic_p) ? CFE_ITALIC : CFE_AUTOCOLOR;

	SetSelectionCharFormat(cf);

}

void CREdit::SetTextColor(COLORREF crColor_p)
{
	CHARFORMAT cf;
	
	cf.cbSize = sizeof (CHARFORMAT); 
	cf.dwMask = CFM_COLOR;
	cf.crTextColor=crColor_p;

	SetSelectionCharFormat(cf);
}

int CREdit::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CRichEditCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//set the default font
	CHARFORMAT cf;
	
	cf.cbSize = sizeof (CHARFORMAT); 
	cf.dwMask = CFM_FACE ;
	//cf.dwEffects;
	//cf.yHeight;
	//cf.yOffset;
	//cf.crTextColor=RGB(250,0,0);
	//cf.bCharSet;
	//cf.bPitchAndFamily;
	sprintf(cf.szFaceName, "Arial");

	SetDefaultCharFormat(cf);
	
	return 0;
}
