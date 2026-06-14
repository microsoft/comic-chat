//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       C2Doc.cpp : implementation of the CChat2000Doc class
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#include "stdafx.h"
#include "Chat2000.h"

#include "C2Doc.h"
#include "C2View.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChat2000Doc

IMPLEMENT_DYNCREATE(CChat2000Doc, CDocument)

BEGIN_MESSAGE_MAP(CChat2000Doc, CDocument)
	//{{AFX_MSG_MAP(CChat2000Doc)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChat2000Doc construction/destruction

CChat2000Doc::CChat2000Doc()
{
	// TODO: add one-time construction code here

}

CChat2000Doc::~CChat2000Doc()
{
}

BOOL CChat2000Doc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChat2000Doc serialization

void CChat2000Doc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		POSITION pos = GetFirstViewPosition();
		//get the first (and only) view 
		CChat2000View *pView =(CChat2000View *) GetNextView( pos );
		ASSERT (pView);
		//write the ocx History
		ar << pView->GetHistory();
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChat2000Doc diagnostics

#ifdef _DEBUG
void CChat2000Doc::AssertValid() const
{
	CDocument::AssertValid();
}

void CChat2000Doc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG