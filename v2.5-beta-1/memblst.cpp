// memblist.cpp : implementation file
//

#include "stdafx.h"
#include "saywnd.h"
#include "chat.h"
#include "userinfo.h"
#include "memblst.h"
#include "ui.h"
#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "textcore.h"
#include "textview.h"
#include "protsupp.h"


extern CChatApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMemberListCtrl

CMemberListCtrl::CMemberListCtrl()
{
}

CMemberListCtrl::~CMemberListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMemberListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMemberListCtrl)
	ON_WM_CHAR()
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemberListCtrl message handlers
void CMemberListCtrl::OnSize(UINT nType, int cx, int cy) 
{
	SetColumnWidth(0, cx);
	CListCtrl::OnSize(nType, cx, cy);
}


void CMemberListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (VK_TAB != nChar)
		ForwardToSayWnd(nChar);
}


void ForwardToSayWnd(UINT nChar)
{
	GetChatDoc()->SetFocusToSayWnd();

	BYTE vKey = VkKeyScan(nChar) & 0xff;
	keybd_event(vKey, 0, 0, 0);
}


/////////////////////////////////////////////////////////////////////////////
// CMemberList

IMPLEMENT_DYNCREATE(CMemberList, CFrameWnd)

CMemberList::CMemberList()
{
	m_pDoc = NULL;		// to be safe
}

CMemberList::~CMemberList()
{
}


BEGIN_MESSAGE_MAP(CMemberList, CFrameWnd)
	//{{AFX_MSG_MAP(CMemberList)
	ON_WM_CONTEXTMENU()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_NOTIFY(NM_KILLFOCUS,1,OnLVKillFocus)
	ON_NOTIFY(NM_DBLCLK,1,OnDblClick)
	ON_NOTIFY(LVN_KEYDOWN,1,OnKeyDown)
	ON_NOTIFY(LVN_GETDISPINFO, 1, OnGetdispinfo)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemberList message handlers

BOOL CMemberList::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	DWORD dwStyle = WS_CHILD | WS_VISIBLE | LVS_ICON | LVS_NOCOLUMNHEADER |
					   LVS_AUTOARRANGE | /*LVS_SORTASCENDING | */ LVS_SHOWSELALWAYS | LVS_SHAREIMAGELISTS;
	m_MemberListBox.Create(dwStyle, CRect(0, 0, 100, 100), this, 1);
	m_MemberListBox.SetFont(&theApp.m_fontGui);
	m_MemberListBox.SetBkColor(COLORREF(RGB(255,255,255)));
	m_MemberListBox.SetTextBkColor(COLORREF(RGB(255,255,255)));
	m_MemberListBox.SetImageList(&theApp.m_ImageList,LVSIL_NORMAL);
	m_MemberListBox.SetImageList(&theApp.m_StatusIcons, LVSIL_SMALL);
//	m_MemberListBox.SetImageList(&theApp.m_StatusIcons, LVSIL_STATE); // set in OnViewIcon
	m_MemberListBox.InsertColumn(0, "FOO", LVCFMT_LEFT, 100);
	UINT mask = m_MemberListBox.GetCallbackMask();;
	m_MemberListBox.SetCallbackMask(mask /*| LVIS_OVERLAYMASK*/ | LVIS_STATEIMAGEMASK);

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CMemberList::RecalcLayout(BOOL bNotify) 
{
	// TODO: Add your specialized code here and/or call the base class
	CRect rect;
	GetClientRect(&rect);
	m_MemberListBox.MoveWindow(0, 0, rect.right, rect.bottom, TRUE);
	CFrameWnd::RecalcLayout(bNotify);
}


void AddMacroMenu(CMenu &contextMenu)
{
	CString strLabel;
	CMenu	*pMenuViewMacros, *pMenuView, *pMenuBody, *pMenuMacros;

	if (!GetChatDoc())
		return;

	pMenuView = GetChatDoc()->GetMenu(VIEWMENUPOS);
	if (!pMenuView)
		return;

	// get macros sub menu (- 3 because of Automation & Options)
	pMenuViewMacros = pMenuView->GetSubMenu(pMenuView->GetMenuItemCount() - 3);
	if (!pMenuViewMacros)
		return;

	pMenuBody = contextMenu.GetSubMenu(0);
	if (!pMenuBody)
		return;

	pMenuMacros = pMenuBody->GetSubMenu((bCanViewUnrated() && GetChatDoc()->m_bComicView) ? MACROSUBMENUCOMIC : MACROSUBMENUTEXT); // NOTE: 7/6 is position of Macros submenu
	if (!pMenuMacros)
		return;

	while (pMenuMacros->RemoveMenu(0, MF_BYPOSITION));

	INT	nItems = pMenuViewMacros->GetMenuItemCount();
	for (INT nTmp = 0; nTmp < nItems; nTmp++)
	{
		INT nID = pMenuViewMacros->GetMenuItemID(nTmp);
		pMenuViewMacros->GetMenuString(nTmp, strLabel, MF_BYPOSITION);
		INT nState = pMenuViewMacros->GetMenuState(nTmp, MF_GRAYED | MF_BYPOSITION);
		INT nFlag = nID ? MF_STRING : MF_SEPARATOR;
		pMenuMacros->AppendMenu(nFlag, nID, strLabel);
		pMenuMacros->EnableMenuItem(nTmp, nState | MF_BYPOSITION);
	}
}


void ShowMemberContext(int x, int y) {
	extern CUserInfo *mousedPui;
	extern void UpdateMemberContext(CMenu &);
	CMenu menu;
	
	if (mousedPui) {
	   #ifdef CB32SUPPORT
		if (g_puiSelf->IsOperator())
			menu.LoadMenu(IDR_MEMBERADMIN);
		else if (!theApp.m_bDoCB32)
			menu.LoadMenu(IDR_IRC_MEMBER);
		else 
			menu.LoadMenu(IDR_NM_MEMBER);
	   #else
		if (g_puiSelf->IsOperator())
			menu.LoadMenu(IDR_MEMBERADMIN);
		else
			menu.LoadMenu(IDR_IRC_MEMBER);
	   #endif // CB32SUPPORT
		GetChatDoc()->UpdateComicCharacterMenu (menu.GetSubMenu (0));
		AddMacroMenu(menu);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
										   x, y, AfxGetMainWnd()/*(CWnd *)cui.m_pvChatView*/);
	}
}

void CMemberList::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	int index = -1;
	extern CUserInfo *mousedPui;
	CMenu menu;
	POINT clientPoint = point;
	ScreenToClient(&clientPoint);  // moved above the first if/else block -djk

	if(point.x == -1 && point.y == -1) {  // then we must be invoking menu with Shift+F10
		// cant do this with multiple selections
		if(m_MemberListBox.GetSelectedCount() > 1) {
			MessageBeep(MB_OK);
			return;
		}
		CRect rect; // so lets set up our own point
		GetClientRect(&rect);
		point.x = (rect.right - rect.left)/2;
		point.y = (rect.bottom - rect.top)/2;
		ClientToScreen(&point);
		index = GetCurrentSelection();
	}
	else
		index = m_MemberListBox.HitTest(clientPoint);

	if (index == -1) {
		if (GetChatDoc()->m_bComicView) {
			menu.LoadMenu(IDR_MEMBERCONTEXT);
			menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
											   point.x, point.y, AfxGetMainWnd());
		}
		return;
	}

	mousedPui = (CUserInfo *) m_MemberListBox.GetItemData(index);
	ShowMemberContext(point.x, point.y);
}


#if 0
// Gets called when something is selected in the list view
void CMemberList::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
}
#endif

void CMemberList::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVNKeyDown = (LV_KEYDOWN*) pNMHDR;
	WORD		wKeyCode = pLVNKeyDown->wVKey;
	BOOL		bShifted = GetKeyState(VK_SHIFT) & 0x8000;

	if(wKeyCode == VK_TAB)
	{
		GetChatDoc ()->CycleFocus (CHATFOCUS_MEMBERLIST, bShifted);
		return;
	}
}

extern HWND hgPrevFocus;

void CMemberList::OnLVKillFocus( NMHDR * pNotifyStruct, LRESULT * result )
{
	hgPrevFocus = m_hWnd;
}

int CMemberList::GetCurrentSelection()
{
	int nCount = m_MemberListBox.GetItemCount();
	for(int index = 0; index < nCount; index++)
	{
		UINT state = m_MemberListBox.GetItemState(index, LVIS_SELECTED );
		if(state & LVIS_SELECTED)
			return index;
	}
	return -1;
}

void CMemberList::OnDblClick(NMHDR* pNotifyStruct, LRESULT* result)
{
	CUserInfo *pui;

	pui = ((CChatDoc *)m_pDoc)->GetSingleSelectedMember();
	if (pui) {
		if (pui->IsComicUser())
			currentRoom->ChatGetInfo(pui);
		else {
			CString mesg;
			mesg.LoadString(IDS_NOTCOMICSUSER);
			VERIFY(ReplaceToken(mesg, CString("%1"), pui->GetScreenName()));
			AfxMessageBox(mesg);
		}
	}
}

int GetSort(CUserInfo *pui) inline {	
	if (pui->IsOperator()) return 0;
	else if (pui->IsSpectator()) return 2;
	else return 1;
}

int CompareStringsWithoutQuotes(
LPCSTR pszString1,
LPCSTR pszString2)
{
	// Quick check, which may save time.

	if (pszString1[0] != '\"' && pszString2[0] != '\"')
		return stricmp (pszString1, pszString2);

	// Remove outside quotes, if there are any.
	int nLen1, nLen2;
	nLen1 = lstrlen (pszString1);
	nLen2 = lstrlen (pszString2);

	if (pszString1[0] == '\"')
	{
		ASSERT(pszString1[nLen1 - 1] == '\"');
		pszString1++;
		nLen1 -= 2;
	}
	if (pszString2[0] == '\"')
	{
		ASSERT(pszString2[nLen2 - 1] == '\"');
		pszString2++;
		nLen2 -= 2;
	}
	return _strnicmp (pszString1, pszString2, max (nLen1, nLen2));
}

int CMemberList::GetSortPosition(CUserInfo *pui) {
	int val, val1;
	int nItems = m_MemberListBox.GetItemCount();
	val = GetSort(pui);
	for (int i = 0; i < nItems; i++) {
		CUserInfo *pui1 = (CUserInfo *) (m_MemberListBox.GetItemData(i));
		val1 = GetSort(pui1);
		if (val < val1) return i;
		else if (val == val1 && CompareStringsWithoutQuotes (pui->GetScreenName(), pui1->GetScreenName()) < 0)
			return i;
	}

	return nItems;
}


CALLBACK SortFunc(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CUserInfo *pui1 = (CUserInfo *)lParam1;
	CUserInfo *pui2 = (CUserInfo *)lParam2;

	int val1 = GetSort(pui1);
	int val2 = GetSort(pui2);

	if (val1 < val2) return -1;
	else if (val2 > val1) return 1;
	else return (CompareStringsWithoutQuotes (pui1->GetScreenName(), pui2->GetScreenName()));
}


void GetSelectedPuis(CPtrArray &selections) {
	selections.RemoveAll();
	int nItems = 0;
	int index = -1;
	ASSERT(GetMembers());
	CListCtrl *members = &GetMembers()->m_MemberListBox;
	do {
		index = members->GetNextItem(index, LVNI_SELECTED);
		if (index == -1) break;
		CUserInfo *pui = (CUserInfo *) members->GetItemData(index);
		if (pui && pui != g_puiSelf) selections.Add(pui);
	} while (nItems++ < 10); // only handle first 10 (hack... revisit this. avoid overrunning IRC mesg limit)
}

void CMemberList::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	LV_ITEM *lv = &pDispInfo->item;
	CUserInfo *pui = (CUserInfo *) lv->lParam;

	ASSERT(lv->iSubItem == 0);

	if (lv->mask & LVIF_TEXT) {
		CString nick;
		if (theApp.m_bDoTest) pui->GetAttedNick(nick);
		else nick = pui->GetScreenName();
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, nick, maxChar);
		lv->pszText[maxChar] = '\0';		// insurance;
	}; // else ASSERT(0);

	if (lv->mask & LVIF_IMAGE) {
		// show icon -- first, if in large icon mode, retrieve its icon index...
		if (((CChatDoc*) m_pDoc)->m_bComicView && ((CChatDoc*) m_pDoc)->m_bIconMembers) {
			CChatDoc *doc = ((CChatDoc *)m_pDoc);
			CAvatarX* pAv = GetAvatar(pui->GetAvatarID());
			CAvatarX *origAv = pAv->m_origID ? GetAvatar(pAv->m_origID) : pAv;
			lv->iImage = origAv->m_iconIndex;
		} else {
			if (pui->Ignored()) lv->iImage = 3;
			else if (pui->CheckFlag(UF_AWAY)) lv->iImage = 4;
			else if (pui->IsOperator()) lv->iImage = 1;
			else if (pui->CheckFlag(UF_SPECTATOR)) lv->iImage = 2;
			else lv->iImage = 0;
		}
	}

	if (lv->mask & LVIF_STATE) {
		if (((CChatDoc*) m_pDoc)->m_bComicView && ((CChatDoc*) m_pDoc)->m_bIconMembers) {
			int state;
			if (pui->Ignored()) state = 4;
			else if (pui->CheckFlag(UF_AWAY)) state = 5;
			else if (pui->IsOperator()) state = 2;
			else if (pui->CheckFlag(UF_SPECTATOR)) state = 3;
			else state = 1;
	//		theApp.m_ImageList.SetOverlayImage(0, 1);
			lv->stateMask = LVIS_STATEIMAGEMASK;
	//		lv->state = INDEXTOOVERLAYMASK(1);
			lv->state = INDEXTOSTATEIMAGEMASK(state);
		}
	}

	*pResult = 0;
}


void CMemberList::Sort() {
	m_MemberListBox.SortItems(SortFunc, NULL);
}

void CMemberList::MakeVisible(CUserInfo *pui) {
	int FindMemberListIndex(CUserInfo *, CChatDoc * = NULL);
	int i = FindMemberListIndex(pui);
	if (i != -1) m_MemberListBox.EnsureVisible(i, FALSE);
}


// don't swallow chars for positioning -- windows standard, but people hate it.
void CMemberList::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	CFrameWnd::OnChar(nChar, nRepCnt, nFlags);
}


void UpdateSpectators(CChatDoc *doc, BOOL moderated) {
	CMemberListCtrl *members = &((CMemberList *)doc->m_memberList)->m_MemberListBox;
	int count = members->GetItemCount();
	for (int i = 0; i < count; i++) {
		CUserInfo *pui = (CUserInfo *) members->GetItemData(i);
		pui->SetFlag(UF_SPECTATOR, !pui->IsOperator() && moderated && !pui->CheckFlag(UF_HASVOICE));
	}
	if (count > 0) members->RedrawItems(0, i-1);
}

