#if !defined(AFX_STATUS_H__FF12C0C3_8744_11D1_A334_00A024A6318E__INCLUDED_)
#define AFX_STATUS_H__FF12C0C3_8744_11D1_A334_00A024A6318E__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000
// status.h : header file
//

#include "textview.h"
#include "ircsock.h"

extern void AddToStatus(CIrcPrint &ircPrint, const char *szLine, CDWordArray *prgdwFormatting = NULL);

/////////////////////////////////////////////////////////////////////////////
// CStatusView view

class CStatusView : public CTextView
{
protected:
	CStatusView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CStatusView)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CStatusView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CStatusView();
	virtual int LoadContextMenu(CMenu& menu);
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CStatusView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnUpdateViewComics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewText(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUS_H__FF12C0C3_8744_11D1_A334_00A024A6318E__INCLUDED_)
