//****************************************************************************
//  Module:     Chappy.EXE
//  File:       History.cpp : implementation file
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
#include "History.h"
#include "Globals.h"
#include "Utility.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHistory

CHistory::CHistory()
{
}

CHistory::~CHistory()
{
	DestroyWindow();
}


BEGIN_MESSAGE_MAP(CHistory, CRichEditCtrl)
	//{{AFX_MSG_MAP(CHistory)
	ON_WM_RBUTTONDOWN()
	ON_COMMAND(IDM_SELECT_FONT, OnSelectFont)
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHistory message handlers

void CHistory::OnRButtonDown(UINT nFlags, CPoint point) 
{
	CPoint pt(point);
	ClientToScreen(&pt);
	PopMenu (HISTORY_MENU, pt);
	CRichEditCtrl::OnRButtonDown(nFlags, point);
}

void CHistory::OnSelectFont() 
{
	CFontDialog FontDlg;
	
	//GetDefaultCharFormat(FontDlg.m_cf);
	
	int ret  = FontDlg.DoModal();
	
	if (IDOK == ret)
	{
	//	SetDefaultCharFormat(FontDlg.m_cf);
	}

}

int CHistory::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CREdit::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	SetReadOnly();
	
	return 0;
}



void CHistory::UpdateMessageUI(LPCTSTR SenderNickname_p, long iMsgType_p,  CString szMessage_p)
{
	CString szMessage;
	CString szMsgType;
	szMsgType.LoadString(IDS_TEXT_MSG_TYPE + iMsgType_p);

	TogleItalicText(msgtWhisper == iMsgType_p);

	szMessage.Format("%s %s\n\t", SenderNickname_p, szMsgType);
	AddText (szMessage, TRUE);
	
	AddText (szMessage_p, FALSE);
	AddText ("\n\n", FALSE);
	LineScroll(3);
}

void CHistory::OnFileSave() 
{
	CFileDialog dlgSave (	FALSE, 
							"rtf", 
							"*.rtf", 
							OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
							"*.rtf");

	if ( IDOK == dlgSave.DoModal())
	{
		//save
		CFile fSaveFile;
		CFileException e;
		if (!fSaveFile.Open(dlgSave.GetPathName(), CFile::modeCreate | CFile::modeWrite , &e)) 
		{
			#ifdef _DEBUG
				afxDump << "File could not be opened " << e.m_cause << "\n";
			#endif
			return;
		}
		
		SetSel(0,-1);
		CString szText = GetSelText();
		int iTextLength = GetWindowTextLength();
		SetSel(iTextLength,iTextLength);
		
		int iBufferSize = szText.GetLength();
		LPCSTR lpBuffer = szText.LockBuffer();
		
		TRY
		{
			fSaveFile.Write(lpBuffer, iBufferSize);
		}
		CATCH( CFileException, e )
		{
			#ifdef _DEBUG
				afxDump << "File could not save " << e->m_cause << "\n";
			#endif
		}
		END_CATCH

		TRY
		{
			fSaveFile.Close();
		}
		CATCH( CFileException, e )
		{
			#ifdef _DEBUG
				afxDump << "File could be closed " << e->m_cause << "\n";
			#endif
		}
		END_CATCH

		szText.UnlockBuffer();

	}
}

void CHistory::ClearAll()
{
	SetReadOnly(FALSE);
	SetSel(0,-1);
	CRichEditCtrl::Clear(); 
	SetReadOnly(TRUE);
}
