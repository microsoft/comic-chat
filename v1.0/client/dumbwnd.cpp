// DumbWnd.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "DumbWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDumbWnd

IMPLEMENT_DYNCREATE(CDumbWnd, CWnd)

CDumbWnd::CDumbWnd()
{
}

CDumbWnd::~CDumbWnd()
{
}


BEGIN_MESSAGE_MAP(CDumbWnd, CWnd)
	//{{AFX_MSG_MAP(CDumbWnd)
	ON_WM_PAINT()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CDumbWnd message handlers

void CDumbWnd::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	
	dc.FillSolidRect(0, 0, 1000, 1000, RGB(255, 0, 0));
	
	// Do not call CWnd::OnPaint() for painting messages
}
