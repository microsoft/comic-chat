/*
	*****************************************************************
	*																*
	*	Module 			: CToolbar.H  - Chat Toolbar Code			*
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

#ifndef _CTOOLBAR_H_
#define _CTOOLBAR_H_

#include <windows.h>
#include "Commctrl.H"

// Indices for Toolbar imagelists
#define IMLIST_DEFAULT          0
#define IMLIST_HOT              1
#define CIMLISTS                2

class CToolbar
{
public:
	CToolbar();
    ~CToolbar();

	HWND	GetHwnd()
				{ return m_hwnd; }

	HWND	GetHwndFrame()
				{ return m_hwndFrame; }	

	HRESULT	HrGetLastError()
				{ return m_hrLastError; }

	short	nGetWidth()
				{ return (short) m_size.cx; }
	short	nGetHeight()
				{ return (short) m_size.cy; }


	BOOL	bCreateToolbar(HWND hwndParent, DWORD dwExStyle, DWORD dwStyleFrame, DWORD dwStyleToolbar, UINT wToolbarID, UINT wBitmapID,
						   HINSTANCE hInstance, int dxButton, int dyButton, 
						   int dxBitmap, int dyBitmap);
	BOOL	DestroyWindow();
	BOOL	bMoveWindow(int X, int Y, int nWidth, int nHeight, BOOL bRepaint);
	BOOL	bShowWindow(int nCmdShow);
	BOOL	bEnableButton(UINT wID, BOOL bEnable);
	BOOL	bIsButtonEnabled(UINT wID);
	BOOL	bAddButton(TBBUTTON *ptbb);
	BOOL	bHideButton(UINT wID, BOOL bHide);
	HDWP	DeferWindowPos(HDWP hWinPosInfo,	int x, int y, int cx, int cy);
	HRESULT	HrLoadGlyphs(int cx, int idBmp);

	void	SetSize(short X, short Y, short BX, short BY);

protected:
	HWND		m_hwnd;
	HWND		m_hwndFrame;
	HRESULT		m_hrLastError;
	HINSTANCE	m_hInstance;

    HIMAGELIST	m_phiml[CIMLISTS];

	SIZE		m_size;

	short		m_nBorderX;
	short		m_nBorderY;

	short		m_nIconWidth;
	short		m_nIconHeight;
	short		m_nButtonWidth;
	short		m_nButtonHeight;
};

#endif // _CTOOLBAR_H_
