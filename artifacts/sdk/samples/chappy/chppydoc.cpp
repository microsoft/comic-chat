//****************************************************************************
//  Module:     Chappy.EXE
//  File:       ChappyDoc.cpp : implementation of the CChappyDoc class
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

#include "ChppyDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChappyDoc

IMPLEMENT_DYNCREATE(CChappyDoc, CDocument)

BEGIN_MESSAGE_MAP(CChappyDoc, CDocument)
	//{{AFX_MSG_MAP(CChappyDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChappyDoc construction/destruction

CChappyDoc::CChappyDoc()
{
	// TODO: add one-time construction code here

}

CChappyDoc::~CChappyDoc()
{
}

BOOL CChappyDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CChappyDoc serialization

void CChappyDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
		TRACE ("Save !! \n");
	}
	else
	{
		// TODO: add loading code here
		TRACE ("Load !! \n");
	}
}

/////////////////////////////////////////////////////////////////////////////
// CChappyDoc diagnostics

#ifdef _DEBUG
void CChappyDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CChappyDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChappyDoc commands
