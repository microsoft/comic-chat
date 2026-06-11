// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// binddoc.cpp : implementation of the DocObject 
//	OLE server document class
//
// This is a part of the Microsoft Foundation Classes C++ library.
// Copyright (C) 1992-1995 Microsoft Corporation
// All rights reserved.
//
// This source code is only intended as a supplement to the
// Microsoft Foundation Classes Reference and related
// electronic documentation provided with the library.
// See these sources for detailed information regarding the
// Microsoft Foundation Classes product.

//BINDER
// CDocObjectServerDoc subclasses MFC's COleServerDoc to add binder-compatible
// OLE interfaces to the class.  The code here declares those extra
// interfaces, provides a rudimentary default command map, and manages
// any IOleDocumentViews active against this document instance.
//BINDER_END

#include "stdafx.h"
#include "binddoc.h"
#include "ui.h"			// for ChatView accessor

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDocObjectServerDoc

IMPLEMENT_DYNAMIC(CDocObjectServerDoc, COleServerDoc)

BEGIN_MESSAGE_MAP(CDocObjectServerDoc, COleServerDoc)
	//{{AFX_MSG_MAP(CDocObjectServerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BEGIN_INTERFACE_MAP(CDocObjectServerDoc, COleServerDoc)
   INTERFACE_PART(CDocObjectServerDoc, IID_IOleObject, DocOleObject)
   INTERFACE_PART(CDocObjectServerDoc, IID_IOleDocument, OleDocument)
   INTERFACE_PART(CDocObjectServerDoc, IID_IOleDocumentView, OleDocumentView)
   INTERFACE_PART(CDocObjectServerDoc, IID_IOleCommandTarget, OleCommandTarget)
   INTERFACE_PART(CDocObjectServerDoc, IID_IPrint, Print)
END_INTERFACE_MAP()

// root of OLECMD map can't use BEGIN_OLECMD_MAP macro
const AFX_DATADEF OLE_CMDMAP CDocObjectServerDoc::commandMap =
{
#ifdef _AFXDLL
	&CDocObjectServerDoc::_GetBaseCommandMap,
#else
	NULL,
#endif
	&CDocObjectServerDoc::_commandEntries[0]
};

#ifdef _AFXDLL
const OLE_CMDMAP* CDocObjectServerDoc::_GetBaseCommandMap()
{
	return NULL;
}
#endif

const OLE_CMDMAP* CDocObjectServerDoc::MyGetCommandMap() const
{
	return &CDocObjectServerDoc::commandMap;
}

const OLE_CMDMAP_ENTRY CDocObjectServerDoc::_commandEntries[] =
{
   { NULL, OLECMDID_SAVEAS, ID_FILE_SAVE_AS },  // ON_OLECMD()
   { NULL, 0, 0 }                               // END_OLECMD_MAP()   
};

CDocObjectServerDoc::CDocObjectServerDoc()
{
   // Initialize DocObject data
   m_pDocSite  = NULL;
   m_pViewSite = NULL;

   // All Binder-Compatible documents use Compound Files as their 
   // storage mechanism
   EnableCompoundFile(TRUE);
}

CDocObjectServerDoc::~CDocObjectServerDoc()
{
	if (m_pDocSite)
      m_pDocSite->Release();
}

void CDocObjectServerDoc::OnCloseDocument() 
{
	// Clean up pointer to document site, if any
	if (m_pDocSite)
	{
      m_pDocSite->Release();
      m_pDocSite = NULL;
	}	
	COleServerDoc::OnCloseDocument();
}

void CDocObjectServerDoc::ActivateDocObject()
{
   if (IsDocObject())
   {
      ASSERT(m_pDocSite != NULL);
      m_pDocSite->ActivateMe(NULL);
   }
}

#ifdef _DEBUG
void CDocObjectServerDoc::AssertValid() const
{
	COleServerDoc::AssertValid();
}

void CDocObjectServerDoc::Dump(CDumpContext& dc) const
{
	COleServerDoc::Dump(dc);
	dc << "m_pDocSite = " << m_pDocSite << "\n";
	dc << "m_pViewSite = " << m_pViewSite << "\n";
}
#endif //_DEBUG

#if 0
void TRACEFILE(char *mesg) {
	static FILE *fp = NULL;
	if (fp == NULL)
		VERIFY(fp = fopen("c:\\tmp\\trace.txt", "w"));
	fprintf(fp, "%s\r\n", mesg);
	fflush(fp);
}
#endif



// We're overriding these two routines (originally defined in olesrv1.cpp), so that the
//   top level view is placed in the inplace frame, not the furthest leaf
//   w/id = AFX_IDW_PANE_FIRST, which seems bizarre to me.  Since we have splitter windows
//   in the primary view, one of these split panes will register as the "view" to put
//   in the inplace frame.  Really, we want the CChatView, which will be (we know) the
//   first view.

COleIPFrameWnd* CDocObjectServerDoc::CreateInPlaceFrame(CWnd* pParentWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pParentWnd);

	// get runtime class from the doc template
	CDocTemplate* pTemplate = GetDocTemplate();
	ASSERT_VALID(pTemplate);

	// use existing view if possible
	CWnd* pViewParent = NULL;
	CView* pView = NULL;
	CFrameWnd* pFrame = GetFirstFrame();
	if (pFrame != NULL)
	{
		pView = (CView*) cui.GetChatViewPv();
		if (pView != NULL)
		{
			ASSERT_KINDOF(CView, pView);
			pViewParent = pView->GetParent();
			m_dwOrigStyle = pView->GetStyle();
			m_dwOrigStyleEx = pView->GetExStyle();
		}
	}

	// create the frame from the template
	COleIPFrameWnd* pFrameWnd = (COleIPFrameWnd*)
		pTemplate->CreateOleFrame(pParentWnd, this, pView == NULL);
	if (pFrameWnd == NULL)
		return NULL;

	// connect the view to the frame window, if necessary
	if (pView != NULL)
	{
		ConnectView(pFrameWnd, pView);
		pView->ModifyStyleEx(WS_EX_CLIENTEDGE, 0, SWP_DRAWFRAME);
	}

	// remember original parent window for deactivate
	m_pOrigParent = pViewParent;

	// send OnInitialUpdate if new view was created
	if (pView == NULL)
		pTemplate->InitialUpdateFrame(pFrameWnd, this, FALSE);

	// verify the type
	ASSERT_VALID(pFrameWnd);
	ASSERT_KINDOF(COleIPFrameWnd, pFrameWnd);
	return pFrameWnd;
}

void CDocObjectServerDoc::DestroyInPlaceFrame(COleIPFrameWnd* pFrameWnd)
{
	ASSERT_VALID(this);
	ASSERT_VALID(pFrameWnd);

	// connect view to original, if existing view was used
	if (m_pOrigParent != NULL)
	{
		// Used to do a GetDescendent -- now gets that ChatView parent window
		// explicitly.

		//		CView* pView = (CView*)pFrameWnd->GetDescendantWindow(
		//			AFX_IDW_PANE_FIRST, TRUE);
		CView* pView = (CView*) cui.GetChatViewPv();
		ASSERT_VALID(pView);

		// leaving the focus on an MDI child or one of its child windows
		// causes Windows to get confused when the child window is
		// destroyed, not to mention the fact that the focus will be
		// out of sync with activation.
		if (::GetFocus() == pView->m_hWnd)
		{
			// move focus to somewhere safe
			HWND hWnd = ::GetParent(pFrameWnd->m_hWnd);
			if (hWnd != NULL)
				::SetFocus(hWnd);

			// check again
			if (::GetFocus() == pView->m_hWnd)
				SetFocus(NULL); // last ditch effort
		}

		ConnectView(m_pOrigParent, pView);
		m_pOrigParent = NULL;

		// remove any scrollbars added because of in-place activation
		if ((m_dwOrigStyle & (WS_HSCROLL|WS_VSCROLL)) == 0 &&
			(pView->GetStyle() & (WS_HSCROLL|WS_VSCROLL)) != 0)
		{
			::SetScrollRange(pView->m_hWnd, SB_HORZ, 0, 0, TRUE);
			::SetScrollRange(pView->m_hWnd, SB_VERT, 0, 0, TRUE);
		}

		// restore old 3D style
		pView->ModifyStyleEx(0, m_dwOrigStyleEx & WS_EX_CLIENTEDGE,
			SWP_DRAWFRAME);

		// force recalc layout on splitter window
		CSplitterWnd* pSplitter = CView::GetParentSplitter(pView, TRUE);
		if (pSplitter != NULL)
			pSplitter->RecalcLayout();
	}

	// no active view or document during destroy
	pFrameWnd->SetActiveView(NULL);

	// destroy in-place frame window
	pFrameWnd->DestroyWindow();
}


