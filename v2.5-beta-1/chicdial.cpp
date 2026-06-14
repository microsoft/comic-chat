// chicdial.cpp : implementation file
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

#include "stdafx.h"
#include "chat.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCSDialog dialog

CCSDialog::CCSDialog(UINT nIDTemplate, CWnd* pParentWnd)
	: CDialog(nIDTemplate, pParentWnd)
{
}

CCSDialog::CCSDialog(LPCTSTR lpszTemplateName, CWnd* pParentWnd)
	: CDialog(lpszTemplateName, pParentWnd)
{
}

CCSDialog::CCSDialog() : CDialog()
{
}

int CCSDialog::DoModal() {
	::ReleaseCapture();
	return (CDialog::DoModal());
} 


BEGIN_MESSAGE_MAP(CCSDialog, CDialog)
	//{{AFX_MSG_MAP(CCSDialog)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_MESSAGE(WM_CONTEXTMENU, OnHelpContextMenu)
	ON_WM_QUERYNEWPALETTE()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCSDialog message handlers

LONG CCSDialog::OnHelp(UINT, LONG lParam)
{
	::WinHelp( (HWND)((LPHELPINFO)lParam)->hItemHandle, AfxGetApp()->m_pszHelpFilePath,
		HELP_WM_HELP, (DWORD)(LPVOID)GetHelpIDs());
	return 0;
}

LONG CCSDialog::OnHelpContextMenu(UINT wParam, LONG)
{
	::WinHelp((HWND)wParam, AfxGetApp()->m_pszHelpFilePath,
		HELP_CONTEXTMENU, (DWORD)(LPVOID)GetHelpIDs());
	return 0;
}

BOOL CCSDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	ModifyStyleEx(0, WS_EX_CONTEXTHELP);		
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

BOOL
CCSDialog::OnQueryNewPalette()
{
	return theApp.QueryNewPaletteCommon (this);
}

/////////////////////////////////////////////////////////////////////////////
// CCSPropertyPage

CCSPropertyPage::CCSPropertyPage(UINT nIDTemplate, UINT nIDCaption)
	: CPropertyPage(nIDTemplate, nIDCaption)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

CCSPropertyPage::CCSPropertyPage(LPCTSTR lpszTemplateName, 
	UINT nIDCaption) : CPropertyPage(lpszTemplateName, nIDCaption)
{
	m_psp.dwFlags &= ~PSP_HASHELP;
}

void
CCSPropertyPage::EnableButton(
UINT nID,
BOOL bEnable)
{
	HWND hwnd = ::GetDlgItem (m_hWnd, nID);
	ASSERT (hwnd);
	BOOL bWasEnabled = ::IsWindowEnabled (hwnd);
	if (bEnable == bWasEnabled)
		return;
	BOOL bFocusDisabling = !bEnable && ::GetFocus () == hwnd;
	if (bFocusDisabling)
	{
		// Need to do some weird stuff because of Windows bug affecting the scenario
		// where a button with focus is disabled.
		NextDlgCtrl ();
		HWND hwndFocus = ::GetFocus ();
		SendMessage (WM_NEXTDLGCTL, (WPARAM)::GetDlgItem (GetParent ()->m_hWnd, IDOK), TRUE);
		SendMessage (WM_NEXTDLGCTL, (WPARAM)hwndFocus, TRUE);
	}
	::EnableWindow (hwnd, bEnable);
}

void
CCSPropertyPage::EnableControl(
UINT nID,
BOOL bEnable)
{
	HWND hwnd = ::GetDlgItem (m_hWnd, nID);
	ASSERT (hwnd);
	BOOL bWasEnabled = ::IsWindowEnabled (hwnd);
	if (bEnable == bWasEnabled)
		return;
	::EnableWindow (hwnd, bEnable);
}

void 
CCSPropertyPage::EnableControlRange(
UINT nIDFrom, 
UINT nIDTo, 
BOOL bEnable)
{
	ASSERT_VALID(this);
	HWND hwnd = ::GetDlgItem (m_hWnd, nIDFrom);
	ASSERT(hwnd != NULL);
	while (hwnd != NULL)
	{
		if (bEnable != ::IsWindowEnabled (hwnd))
			::EnableWindow (hwnd, bEnable);
		hwnd = ::GetDlgCtrlID (hwnd) == nIDTo ? NULL : ::GetNextWindow (hwnd, GW_HWNDNEXT);
	} 
}
BEGIN_MESSAGE_MAP(CCSPropertyPage, CPropertyPage)
	//{{AFX_MSG_MAP(CCSPropertyPage)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_MESSAGE(WM_CONTEXTMENU, OnHelpContextMenu)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CCSPropertyPage message handlers

LONG CCSPropertyPage::OnHelp(UINT, LONG lParam)
{
	::WinHelp( (HWND)((LPHELPINFO)lParam)->hItemHandle, AfxGetApp()->m_pszHelpFilePath,
		HELP_WM_HELP, (DWORD)(LPVOID)GetHelpIDs());
	return 0;
}

LONG CCSPropertyPage::OnHelpContextMenu(UINT wParam, LONG)
{
	::WinHelp((HWND)wParam, AfxGetApp()->m_pszHelpFilePath,
		HELP_CONTEXTMENU, (DWORD)(LPVOID)GetHelpIDs());
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
// CCSPropertySheet

BEGIN_MESSAGE_MAP(CCSPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CCSPropertySheet)
	ON_WM_NCCREATE()
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_HELP, OnHelp)
	ON_MESSAGE(WM_CONTEXTMENU, OnHelpContextMenu)
	ON_WM_QUERYNEWPALETTE()
END_MESSAGE_MAP()

CCSPropertySheet::CCSPropertySheet(UINT nIDCaption, CWnd *pParentWnd, 
	UINT iSelectPage) : CPropertySheet(nIDCaption, pParentWnd, iSelectPage)
{
}

CCSPropertySheet::CCSPropertySheet(LPCTSTR pszCaption, CWnd *pParentWnd, 
	UINT iSelectPage) : CPropertySheet(pszCaption, pParentWnd, iSelectPage)
{
}

int 
CCSPropertySheet::GetPageIndexFromID(
UINT nID)
{
	for (int i = m_pages.GetUpperBound (); i >= 0; i--)
	{
		CPropertyPage* pPage = GetPage (i);
		if (pPage->m_psp.pszTemplate == MAKEINTRESOURCE(nID))
			return i;
	}
	return -1;
}

/////////////////////////////////////////////////////////////////////////////
// CCSPropertySheet message handlers

LONG CCSPropertySheet::OnHelp(UINT wParam, LONG lParam)
{
	GetActivePage()->SendMessage(WM_HELP, wParam, lParam);
	return 0;
}

LONG CCSPropertySheet::OnHelpContextMenu(UINT wParam, LONG lParam)
{
	GetActivePage()->SendMessage(WM_CONTEXTMENU, wParam, lParam);
	return 0;
}

#if 0
BOOL CCSPropertySheet::PreCreateWindow(CREATESTRUCT& cs)
{
	cs.dwExStyle |= WS_EX_CONTEXTHELP;
	return CPropertySheet::PreCreateWindow(cs);
}
#endif

BOOL CCSPropertySheet::OnNcCreate(LPCREATESTRUCT lpCreateStruct) 
{
	lpCreateStruct->dwExStyle |= WS_EX_CONTEXTHELP;
	if (!CPropertySheet::OnNcCreate(lpCreateStruct))
		return FALSE;
	
	lpCreateStruct->dwExStyle |= WS_EX_CONTEXTHELP;
	ModifyStyleEx(0, WS_EX_CONTEXTHELP);
	
	return TRUE;
}

BOOL
CCSPropertySheet::OnQueryNewPalette()
{
	return theApp.QueryNewPaletteCommon (this);
}


