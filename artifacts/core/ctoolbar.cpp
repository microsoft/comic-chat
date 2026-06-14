/*
	*****************************************************************
	*																*
	*	Module 			: CToolbar.cpp  - Chat Toolbar Code			*
	*																*
	*	Author 			: RegisB, 08/26/96							*
	*																*
	*	Current Owner	: RegisB									*
	*																*
	*	Changes			:											*
	*																*
	*	To Dos			:											*
	*																*
	*****************************************************************
*/

//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

#include "CToolbar.H"
#include "CCommon.H"
#include "CDebug.H"

// for ASSERT and FAIL
SZTHISFILE


/////////////////////////////////////////////////////////////////////////////
// CToolbar::CToolbar - Constructor
CToolbar::CToolbar()
{
	m_hwnd		= NULL;
	m_hwndFrame = NULL;
	m_hInstance	= NULL;

    for (int i = 0; i < CIMLISTS; i++)
        m_phiml[i] = NULL;

	m_hrLastError = NOERROR;

	m_size.cx = m_size.cy = 0;
	m_nBorderX = m_nBorderY = 0;

	m_nIconWidth	= 0;
	m_nIconHeight	= 0;
	m_nButtonWidth	= 0;
	m_nButtonHeight	= 0;
}


/////////////////////////////////////////////////////////////////////////////
// CToolbar::~CToolbar - Destructor
CToolbar::~CToolbar()
{
	DestroyWindow();
}


BOOL CToolbar::DestroyWindow()
{
	BOOL bRet1 = TRUE;
	BOOL bRet2 = TRUE;

    for (int i = 0; i < CIMLISTS; i++)
    {
        if (m_phiml[i])
		{
            ImageList_Destroy(m_phiml[i]);
			m_phiml[i] = NULL;
		}
	}

	if (m_hwnd)
	{
		bRet1 = ::DestroyWindow(m_hwnd);
		m_hwnd = NULL;
	}
	if (m_hwndFrame)
	{
		bRet2 = ::DestroyWindow(m_hwndFrame);
		m_hwndFrame = NULL;
	}
	return bRet1 && bRet2;
}


HRESULT CToolbar::HrLoadGlyphs(int cx, int idBmp)
{
    // set uMsg and uFlags for first iteration of loop (default state)
    UINT uMsg = TB_SETIMAGELIST;
    UINT uFlags = LR_LOADMAP3DCOLORS | LR_CREATEDIBSECTION;
    HIMAGELIST himlOld;

    for (int i = 0; i < CIMLISTS; i++)
    {
        m_phiml[i] = ImageList_LoadImage(m_hInstance,
										 MAKEINTRESOURCE(idBmp + i), 
										 cx, 
										 0, 
										 CLR_NONE,
										 IMAGE_BITMAP, 
										 uFlags);
		if (!m_phiml[i])
		{
			ASSERT(FALSE, "ImageList_LoadImage failed in CToolbar::HrLoadGlyphs");
			return E_FAIL;
		}

        himlOld = (HIMAGELIST) ::SendMessage(m_hwnd, uMsg, 0, (LPARAM) m_phiml[i]);

        if (himlOld)
            ImageList_Destroy(himlOld);

        // set uMsg and uFlags for last iteration of loop (hot state)
        uMsg = TB_SETHOTIMAGELIST;
        uFlags = 0;
    }

	return NOERROR;
}


BOOL CToolbar::bCreateToolbar(HWND hwndParent, DWORD dwExStyle, DWORD dwStyleFrame, DWORD dwStyleToolbar, UINT wToolbarID, UINT wBitmapID,
							  HINSTANCE hInstance, int dxButton, int dyButton, 
							  int dxBitmap, int dyBitmap)
{
	LRESULT lr = NOERROR;

	OutputDebugThreadIdString("CToolbar::bCreateToolbar - Enter\n");

	ASSERT(!m_hwnd, "m_hwnd is NOT NULL in CToolbar::bCreateToolbar");
	ASSERT(!m_hwndFrame, "m_hwndFrame is NOT NULL in CToolbar::bCreateToolbar");
	ASSERT(hInstance, "hInstance is NULL in CToolbar::bCreateToolbar");

	m_hInstance = hInstance;

	m_hwnd = CreateWindowEx(WS_EX_TOOLWINDOW, TOOLBARCLASSNAME, NULL, dwStyleToolbar, 
							0, 0, 0, 0, hwndParent, (HMENU) wToolbarID, hInstance, NULL);
	if (!m_hwnd)
	{
		ASSERT(FALSE, "CreateWindowEx 1 failed in CToolbar::bCreateToolbar");
		m_hrLastError = GetLastError();
		return FALSE;
	}

	if (dwExStyle || dwStyleFrame)
	{
		m_hwndFrame = CreateWindowEx(dwExStyle, "EDIT", NULL, dwStyleFrame | WS_CHILD | WS_CLIPSIBLINGS, 
									 0, 0, 0, 0, hwndParent, NULL, hInstance, NULL);
		if (!m_hwndFrame)
		{
			ASSERT(FALSE, "CreateWindowEx 2 failed in CToolbar::bCreateToolbar");
			m_hrLastError = GetLastError();
			return FALSE;
		}

		::EnableWindow(m_hwndFrame, FALSE);
		if (!::SetWindowPos(m_hwndFrame, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE))
		{
			ASSERT(FALSE, "SetWindowPos 1 failed in CToolbar::bCreateToolbar");
			m_hrLastError = GetLastError();
			return FALSE;
		}
		if (!::SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE))
		{
			ASSERT(FALSE, "SetWindowPos 2 failed in CToolbar::bCreateToolbar");
			m_hrLastError = GetLastError();
			return FALSE;
		}
	}

	// this tells the toolbar what version we are
    ::SendMessage(m_hwnd, TB_BUTTONSTRUCTSIZE, (WPARAM) sizeof(TBBUTTON), 0L);

    lr = ::SendMessage(m_hwnd, TB_SETMAXTEXTROWS, (WPARAM) 1L, 0L);
    
	if (!::SendMessage(m_hwnd, TB_SETBITMAPSIZE, 0L, (LPARAM) MAKELONG(dxBitmap, dyBitmap)))
	{
		ASSERT(FALSE, "TB_SETBITMAPSIZE failed in CToolbar::bCreateToolbar");
		m_hrLastError = E_FAIL;
		return FALSE;
	}

    return NOERROR == (m_hrLastError = HrLoadGlyphs(dxBitmap, wBitmapID));
}


BOOL CToolbar::bMoveWindow(int X, int Y, int nWidth, int nHeight, BOOL bRepaint)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CToolbar::bMoveWindow");

	if (m_hwndFrame)
		::MoveWindow(m_hwndFrame, X, Y, nWidth, nHeight, bRepaint);

	return ::MoveWindow(m_hwnd, X+m_nBorderX, Y+m_nBorderY, nWidth-2*m_nBorderX, nHeight-2*m_nBorderY, bRepaint);
}


BOOL CToolbar::bShowWindow(int nCmdShow)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CToolbar::bShowWindow");

	BOOL bRet;

	bRet = ::ShowWindow(m_hwnd, nCmdShow);

	if (m_hwndFrame)
	{
		bRet = ::ShowWindow(m_hwndFrame, nCmdShow);
		bRet = ::SetWindowPos(m_hwndFrame, HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ASSERT(bRet, "SetWindowPos");
		bRet = ::SetWindowPos(m_hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
		ASSERT(bRet, "SetWindowPos");
	}

	return TRUE;
}


HDWP CToolbar::DeferWindowPos(HDWP hWinPosInfo,	int x, int y, int cx, int cy)
{
	HDWP	hDWP = hWinPosInfo;

	if (m_hwndFrame)
		hDWP = ::DeferWindowPos(hDWP, m_hwndFrame, HWND_BOTTOM, x, y, cx, cy, 0);

	hDWP = ::DeferWindowPos(hDWP, m_hwnd, HWND_TOP, x+m_nBorderX, y+m_nBorderY, cx-2*m_nBorderX, cy-2*m_nBorderY, 0);

	return hDWP;
}


BOOL CToolbar::bEnableButton(UINT wID, BOOL bEnable)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CToolbar::bEnableButton");

	return ::SendMessage(m_hwnd, TB_ENABLEBUTTON, wID, MAKELONG(bEnable, 0));
}


BOOL CToolbar::bIsButtonEnabled(UINT wID)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CToolbar::bIsButtonEnabled");

	return ::SendMessage(m_hwnd, TB_ISBUTTONENABLED, wID, 0L);
}


BOOL CToolbar::bAddButton(TBBUTTON *ptbb)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CToolbar::bAddButton");
	ASSERT(m_hInstance, "m_hInstance is NULL in CToolbar::bAddButton");
	ASSERT(ptbb, "ptbb is NULL in CToolbar::bAddButton");

//  if (-1 == ::SendMessage(m_hwnd, TB_ADDSTRING, (WPARAM) m_hInstance, (LPARAM) MAKELONG(ptbb->idCommand, 0)))
//	{
//		ASSERT(FALSE, "TB_ADDSTRING failed in CToolbar::bAddButton");
//		m_hrLastError = E_FAIL;
//		return FALSE;
//	}

    return ::SendMessage(m_hwnd, TB_ADDBUTTONS,(WPARAM) 1L, (LPARAM) ptbb);
}


BOOL CToolbar::bHideButton(UINT wID, BOOL bHide)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CToolbar::bHideButton");

	return ::SendMessage(m_hwnd, TB_HIDEBUTTON, (WPARAM) wID, (LPARAM) bHide);
}


void CToolbar::SetSize(short X, short Y, short BX, short BY)
{
	m_size.cx = X+2*BX;
	m_size.cy = Y+2*BY;

	m_nBorderX = BX;
	m_nBorderY = BY;
}
