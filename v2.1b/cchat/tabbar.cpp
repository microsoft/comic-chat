#include "stdafx.h"

#include "chat.h"
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"

#include "tabbar.h"
#include "resource.h"
#include "ui.h"

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CTabBar

CTabBar::CTabBar()
{
	m_currentDoc	= NULL;
	m_bDeleteFont	= FALSE;
	m_lLargestTab	= 64L;	// magic number

	LOGFONT	logFont;
	theApp.m_fontGui.GetLogFont(&logFont);

	CString strRoomTabFontWeight;
	strRoomTabFontWeight.LoadString(IDS_ROOMTAB_FONTWEIGHT);
	logFont.lfWeight = atoi(strRoomTabFontWeight);
	if (m_font.CreateFontIndirect(&logFont))
		m_bDeleteFont = TRUE;
}

CTabBar::~CTabBar()
{
	if (m_bDeleteFont)
		m_font.DeleteObject();
}


BEGIN_MESSAGE_MAP(CTabBar, CControlBar)
	//{{AFX_MSG_MAP(CTabBar)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_NOTIFY(TCN_SELCHANGE, IDC_TAB1, OnSelchangeTab)
	ON_WM_ERASEBKGND()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CTabBar message handlers

BOOL CTabBar::OnEraseBkgnd(CDC* pDC) 
{
	// For some unknown reason, pDC sometimes is a CClientDC that's invalid (doesn't
	// have a m_hWnd associated with it.  So we need to create our own.  Sigh. Investigate!
	// Seems to happen when we hide the tab control
//	CClientDC dc(this);
	DWORD clr = GetSysColor(COLOR_3DFACE);
	RECT rect;

	GetClientRect(&rect);
	pDC->FillSolidRect(&rect, clr);
	return TRUE;
}


// REGISB 10/24/97 This method is never called - removing
//CSize CTabBar::CalcFixedLayout(BOOL bStretch, BOOL bHorz) {
//	OutputDebugString("CTabBar::CalcFixedLayout\n");
//	CWnd *parent = GetParent();
//	RECT rect;
//	parent->GetClientRect(&rect);
//
//	return bHorz ? CSize(rect.right, 25) : CSize(20, rect.bottom);
//}


CSize CTabBar::CalcDynamicLayout(int nLength, DWORD dwMode)
{
	static int iLength = 0;

//#ifdef DEBUG
//	BOOL b = dwMode & LM_STRETCH;
//	BOOL c = dwMode & LM_HORZ;
//	BOOL d = dwMode & LM_MRUWIDTH;
//	BOOL e = dwMode & LM_HORZDOCK;
//	BOOL f = dwMode & LM_VERTDOCK;
//	BOOL g = dwMode & LM_LENGTHY;
//	BOOL h = dwMode & LM_COMMIT;
//	TRACE("CTabBar::CalcDynamicLayout dwMode=%d, nLength=%d, iLength=%d\n", dwMode, nLength, iLength);
//	TRACE("LM_STRETCH=%d, LM_HORZ=%d, LM_MRUWIDTH=%d, LM_HORZDOCK=%d, LM_VERTDOCK=%d, LM_LENGTHY=%d, LM_COMMIT= %d\n", b,c,d,e,f,g,h);
//#endif // DEBUG

	CWnd*	pParentWnd = GetDockingFrame();
	RECT	rect;

	ASSERT(pParentWnd);
	pParentWnd->GetWindowRect(&rect);

	if (dwMode == LM_HORZ)
		iLength = max(nLength, m_lLargestTab + 64L);
	else
		if (dwMode == LM_HORZ+LM_HORZDOCK && nLength == 0)
			iLength = max(iLength, m_lLargestTab + 64L);
		else
			if (dwMode == LM_HORZ+LM_HORZDOCK && nLength == -1)
			{
				iLength = iLength ? iLength : max(rect.right - rect.left - 100, m_lLargestTab + 64L);
				return CSize(rect.right - rect.left, 29);
			}

	return CSize(iLength, 29);
}


int CTabBar::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CControlBar::OnCreate(lpCreateStruct) == -1)
		return -1;

	RECT r;
	GetClientRect(&r);
	r.top = 5;
//	r.bottom = 15;
	
	m_tabCtrl.Create(WS_CHILD /*| WS_VISIBLE | TCS_BUTTONS*/, r, this, IDC_TAB1);

	if (m_bDeleteFont)
		m_tabCtrl.SetFont(&m_font, FALSE);

	m_images.Create(IDB_TABS, 16, 4, RGB(0, 255, 0));
	m_tabCtrl.SetImageList(&m_images);
	
	return 0;
}


void CTabBar::OnSize(UINT nType, int cx, int cy) 
{
	// OutputDebugString("CTabBar::OnSize\n");
	CControlBar::OnSize(nType, cx, cy);
	RECT rect;
	m_tabCtrl.GetWindowRect(&rect);
	m_tabCtrl.SetWindowPos(NULL, 0, 0, cx, rect.bottom - rect.top, SWP_NOMOVE | SWP_NOZORDER);
}


void CTabBar::OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult) 
{
	//OutputDebugString("CTabBar::OnSelchangeTab\n");

	void ActivateWindow(CChatDoc *);

	int sel = m_tabCtrl.GetCurSel();
	ASSERT(sel >= 0);
	ActivateWindow(GetTabDoc(sel));

	*pResult = 0;
}


CChatDoc *CTabBar::GetTabDoc(int index) {
	TC_ITEM tcItem;
	tcItem.mask = TCIF_PARAM;
	m_tabCtrl.GetItem(index, &tcItem);
	return ((CChatDoc *)tcItem.lParam);
}


void CTabBar::AddMDITab(const char *szChanName, CChatDoc *doc) {
	//OutputDebugString("CTabBar::AddMDITab\n");
	RECT	rect;
	TC_ITEM tcItem;
	CString strTabName;
	int		iPlace = 0, nTabs = m_tabCtrl.GetItemCount();

	if (nTabs == 0)
		m_tabCtrl.ShowWindow(SW_SHOWNOACTIVATE); // keep invisible until tabs added due to refresh bug

	for (int i = 0; i < nTabs; i++)
	{
		GetTabString(i, strTabName);
		if (stricmp(szChanName, strTabName) < 0)
			break;
		else
			iPlace++;
	}

	tcItem.mask    = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
	tcItem.pszText = (char*) szChanName;
	tcItem.iImage  = 0; // XXX
	tcItem.lParam  = (long) doc;

	m_tabCtrl.InsertItem(iPlace, &tcItem);
	m_tabCtrl.SetCurSel(iPlace);
	m_tabCtrl.GetItemRect(iPlace, &rect);

	if ((rect.right - rect.left) > m_lLargestTab)
		m_lLargestTab = rect.right - rect.left;

	doc->m_bNewContent = FALSE;
	m_currentDoc = doc;
}


void SetActiveTab(CChatDoc *doc) {
	//OutputDebugString("SetActiveTab\n");
	CTabBar *tbar = GetTabBar();
	if (!tbar) return;
	int i = tbar->FindTabNum(doc);
	if (i > -1) {
		tbar->m_tabCtrl.SetCurSel(i);
		tbar->m_currentDoc = doc;
	}
}


void CTabBar::SetTabIcon(int tabNum, int iIcon) {
	//OutputDebugString("CTabBar::SetTabIcon\n");
	TC_ITEM tcItem;
	tcItem.mask = TCIF_IMAGE;
	tcItem.iImage = iIcon;
	m_tabCtrl.SetItem(tabNum, &tcItem);
}


void CTabBar::GetTabString(int i, CString &name) {
	//OutputDebugString("CTabBar::GetTabString\n");
	TC_ITEM tcItem;
	char textBuff[100];
	tcItem.mask = TCIF_TEXT;
	tcItem.cchTextMax = sizeof(textBuff);
	tcItem.pszText = textBuff;
	m_tabCtrl.GetItem(i, &tcItem);
	name = tcItem.pszText;
}


void CTabBar::DelMDITab(int iTab) {
	//OutputDebugString("CTabBar::DelMDITab\n");
	if (iTab > -1)
	{
		RECT rect;
	
		m_tabCtrl.GetItemRect(iTab, &rect);
		m_tabCtrl.DeleteItem(iTab);

		int nTabs = m_tabCtrl.GetItemCount();
		if (0 == nTabs)
		{
			m_tabCtrl.ShowWindow(SW_HIDE);	// keep invisible w/o tabs, due to refresh bug
			m_lLargestTab = 64L;
		}
		else
			if ((rect.right - rect.left) == m_lLargestTab)
			{
				// largest tab was just removed
				m_lLargestTab = 64L;
				for (int i = 0; i < nTabs; i++)
				{
					m_tabCtrl.GetItemRect(i, &rect);
					if ((rect.right - rect.left) > m_lLargestTab)
						m_lLargestTab = rect.right - rect.left;
				}
			}
	}
}


int CTabBar::FindTabNum(CChatDoc *doc) {
	//OutputDebugString("CTabBar::FindTabNum\n");
	int nTabs = m_tabCtrl.GetItemCount();
	for (int i = 0; i < nTabs; i++)
		if (doc == GetTabDoc(i)) return i;

	return -1;
}

