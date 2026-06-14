// Created		: RegisB, 02/16/98
//
// RtfCmb.cpp	: implementation file

#include "stdafx.h"
#include "rtfcmb.h"
#include "autopage.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRtfCmb

IMPLEMENT_DYNCREATE(CRtfCmb, CComboBox)

CRtfCmb::CRtfCmb()
{
	m_bAttached	= FALSE;
	m_bRtfMode	= FALSE;
	m_pRtfCtrl	= NULL;
}


CRtfCmb::~CRtfCmb()
{
	if (m_pRtfCtrl)
	{
		ASSERT(m_bRtfMode);
		delete m_pRtfCtrl;
	}
}


BOOL CRtfCmb::bSetRtfMode(BOOL bRtfMode)
{
	if (bRtfMode == m_bRtfMode)
		return TRUE;

	if (bRtfMode)
	{
		// Switch to RTF mode
		ASSERT(!m_pRtfCtrl);
		if (!(m_pRtfCtrl = new CRtfCmbEdit))
			return FALSE;
		m_pRtfCtrl->SetParent(this);
		m_bRtfMode = TRUE;
		m_bAttached = FALSE;
		ModifyStyle(WS_TABSTOP, 0L);
		return TRUE;
	}
	else
	{
		// Switch to non-RTF mode
		if (m_pRtfCtrl)
		{
			delete m_pRtfCtrl;
			m_pRtfCtrl = NULL;
		}
		m_bRtfMode = FALSE;
		m_bAttached = FALSE;
		ModifyStyle(0L, WS_TABSTOP);
		return TRUE;
	}
}


BOOL CRtfCmb::bAttachRtfCtrl(UINT nID)
{
	RECT	rect1, rect2, rect3;

	if (!m_pRtfCtrl || m_bAttached)
		return TRUE;

	GetClientRect(&rect1);
	GetWindowRect(&rect2);
	GetParent()->GetWindowRect(&rect3);

	rect1.left	+= rect2.left - rect3.left;
	rect1.top	+= rect2.top - rect3.top - 19; // + 75;
	rect1.right	+= rect1.left - 23;
	rect1.bottom+= rect1.top - 5;

	if (m_bAttached = m_pRtfCtrl->Create(WS_CHILD | WS_TABSTOP | WS_VISIBLE | ES_AUTOHSCROLL | ES_NOHIDESEL, rect1, GetParent(), nID))
	{
		m_pRtfCtrl->DefineDefaultCharFormat();
		m_pRtfCtrl->UseDefaultCharFormat();
		m_pRtfCtrl->SetWindowPos(&wndTop, 0, 0, 0, 0, SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOSIZE);
		m_pRtfCtrl->SetEventMask(m_pRtfCtrl->GetEventMask() | ENM_MOUSEEVENTS);
	}

	return m_bAttached;
}


BEGIN_MESSAGE_MAP(CRtfCmb, CComboBox)
	//{{AFX_MSG_MAP(CRtfCmb)
	ON_CONTROL_REFLECT(CBN_DROPDOWN, OnDropDown)
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseUp)
	ON_CONTROL_REFLECT(CBN_EDITCHANGE, OnEditChange)
	ON_CONTROL_REFLECT(CBN_SELENDOK, OnSelEndOK)
	ON_WM_SHOWWINDOW()
//	ON_WM_LBUTTONUP()
//	ON_WM_KEYDOWN()
//	ON_WM_GETDLGCODE()
//	ON_MESSAGE(WM_U_TAB, OnTabOut)
//	ON_WM_ACTIVATE()
//	ON_WM_MOUSEACTIVATE()
//	ON_WM_PAINT()
//	ON_WM_CHAR()
//	ON_WM_SETFOCUS()
//	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRtfCmb message handlers

//void CRtfCmb::OnSetFocus(CWnd* pOldWnd)
//{
//	OutputDebugString("CRtfCmb::OnSetFocus - Enter\n");
//
//	CComboBox::OnSetFocus(pOldWnd);
//
//	if (m_bRtfMode)
//	{
//		ASSERT(m_pRtfCtrl);
//		m_pRtfCtrl->SetFocus();
//	}
//}

CWnd* CRtfCmb::RedirectFocus()
{
	if (m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		return m_pRtfCtrl->SetFocus();
	}
	else
		return NULL;
}


void CRtfCmb::OnDropDown()
{
	//OutputDebugString("CRtfCmb::OnDropDown - Enter\n");
	RedirectSelection();
}


void CRtfCmb::OnCloseUp()
{
	//OutputDebugString("CRtfCmb::OnCloseUp - Enter\n");
	RedirectFocus();
}


/*
void CRtfCmb::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
{
	if (nState == WA_INACTIVE)
		OutputDebugString("CRtfCmb::OnActivate - WA_INACTIVE\n");
	else
		if (nState == WA_ACTIVE)
			OutputDebugString("CRtfCmb::OnActivate - WA_ACTIVE\n");
		else
			OutputDebugString("CRtfCmb::OnActivate - WA_CLICKACTIVE\n");

	CComboBox::OnActivate(nState, pWndOther, bMinimized);
}


int	CRtfCmb::OnMouseActivate(CWnd* pDesktopWnd, UINT nHitTest, UINT message)
{
	OutputDebugString("CRtfCmb::OnMouseActivate - Enter\n");

	return CComboBox::OnMouseActivate(pDesktopWnd, nHitTest, message);
}
*/


void CRtfCmb::RedirectSelection()
{
	DWORD dwSel = CComboBox::GetEditSel();

	if (CB_ERR != dwSel && m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		m_pRtfCtrl->SetSel(LOWORD(dwSel), HIWORD(dwSel));
	}
}


void CRtfCmb::OnEditChange()
{
	//OutputDebugString("CRtfCmb::OnEditChange - Enter\n");
	if (m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		CString strText;
		CComboBox::GetWindowText(strText);
		m_pRtfCtrl->SetWindowText(strText);
	}
}


void CRtfCmb::OnSelEndOK()
{
	// OutputDebugString("CRtfCmb::OnSelEndOK - Enter\n");
	if (m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		CString strText;
		int iIndex = CComboBox::GetCurSel();
		if (CB_ERR != iIndex)
		{
			GetLBText(iIndex, strText);
			m_pRtfCtrl->SetWindowText(strText);
			m_pRtfCtrl->SetSel(0, strText.GetLength());
			m_pRtfCtrl->UseDefaultCharFormat(TRUE /*bUpdateSelection*/);
			RedirectFocus();
		}
	}
}


void CRtfCmb::SetWindowText(LPCTSTR szString)
{
	if (m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		m_pRtfCtrl->SetWindowText(szString);
	}
	else
		CComboBox::SetWindowText(szString);
}


int	CRtfCmb::GetWindowText(LPTSTR szStringBuf, int nMaxCount) const
{
	if (m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		return m_pRtfCtrl->GetWindowText(szStringBuf, nMaxCount);
	}
	else
		return CComboBox::GetWindowText(szStringBuf, nMaxCount);
}


void CRtfCmb::GetWindowText(CString& rString) const
{
	if (m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		m_pRtfCtrl->GetWindowText(rString);
	}
	else
		CComboBox::GetWindowText(rString);
}


BOOL CRtfCmb::LimitText(int nMaxChars)
{
	if (m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		m_pRtfCtrl->LimitText(nMaxChars);
	}

	return CComboBox::LimitText(nMaxChars);
}


void CRtfCmb::OnShowWindow(BOOL bShow, UINT nStatus)
{
	CComboBox::OnShowWindow(bShow, nStatus);

	if (m_bRtfMode)
	{
		ASSERT(m_pRtfCtrl);
		m_pRtfCtrl->ShowWindow(bShow ? SW_SHOW : SW_HIDE);
	}
}


//void CRtfCmb::OnLButtonUp(UINT nFlags, CPoint point)
//{
//	if (!nFlags && -1 == point.x && -1 == point.y && (GetKeyState(VK_TAB) & 0x8000))
//	{
//		// OutputDebugString("CRtfCmb::OnLButtonUp - Tab\n");
//		CEditRule* per = (CEditRule*) GetParent();
//		ASSERT(per);
//		per->Tab(this, !(GetKeyState(VK_SHIFT) & 0x8000));
//		return;
//	}
//	CComboBox::OnLButtonUp(nFlags, point);
//}


/*
UINT CRtfCmb::OnGetDlgCode()
{
	return CComboBox::OnGetDlgCode() | DLGC_WANTTAB;
}


void CRtfCmb::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_TAB == nChar)
	{
		// OutputDebugString("CRtfCmb::OnKeyDown - Tab\n");
		CEditRule* per = (CEditRule*) GetParent();
		ASSERT(per);
		per->Tab(this, !(GetKeyState(VK_SHIFT) & 0x8000));
		return;
	}
	CComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/


/*
LONG CRtfCmb::OnTabOut(UINT u, LONG l)
{
	BOOL bVKTab = GetKeyState(VK_TAB) & 0x8000;
	TRACE("CRtfCmb::OnTabOut u=%d, l=%d, TabKeyDown=%d\n", u, l, bVKTab);
	return 0L;
}
*/

/////////////////////////////////////////////////////////////////////////////
// CRtfCmb message handlers
IMPLEMENT_DYNCREATE(CRtfCmbEdit, CRtfCtrl)

BEGIN_MESSAGE_MAP(CRtfCmbEdit, CRtfCtrl)
	//{{AFX_MSG_MAP(CRtfCmbEdit)
	ON_WM_CHAR()
	ON_WM_SETFOCUS()
	ON_WM_KILLFOCUS()
//	ON_WM_GETDLGCODE()
//	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/*
UINT CRtfCmbEdit::OnGetDlgCode()
{
	return CRtfCtrl::OnGetDlgCode() | DLGC_WANTTAB;
}


void CRtfCmbEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (nChar == VK_TAB && m_pParent)
	{
		// OutputDebugString("CRtfCmbEdit::OnKeyDown - Tab\n");
		CEditRule* per = (CEditRule*) m_pParent->GetParent();
		ASSERT(per);
		per->Tab(this, !(GetKeyState(VK_SHIFT) & 0x8000));
		return;
	}
	else
		CRtfCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/


void CRtfCmbEdit::OnSetFocus(CWnd* pOldWnd)
{
	// OutputDebugString("CRtfCmbEdit::OnSetFocus - Enter\n");
	
	if (!m_bColorWnd)
		SetSel(0, GetTextLength());

	CRtfCtrl::OnSetFocus(pOldWnd);
}


void CRtfCmbEdit::OnKillFocus(CWnd* pNewWnd)
{
	// OutputDebugString("CRtfCmbEdit::OnKillFocus - Enter\n");

	CRtfCtrl::OnKillFocus(pNewWnd);

	if (m_bSelectAll)
	{
		SetSel(0, 0);
		SetCursor(LoadCursor(NULL, IDC_ARROW));
	}
}


void CRtfCmbEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// OutputDebugString("CRtfCmbEdit::OnChar - Enter\n");
	CRtfCtrl::OnChar(nChar, nRepCnt, nFlags);

	CString strText;
	CRtfCtrl::GetWindowText(strText);
	if (m_pParent)
		m_pParent->SetWindowText(strText);
}
