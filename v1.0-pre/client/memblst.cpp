// memblist.cpp : implementation file
//

#include "stdafx.h"
#include "saywnd.h"
#include "chat.h"
#include "memblst.h"
#include "ui.h"
#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "userinfo.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "textview.h"
#include "chatprot.h"


extern CChatApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMemberList

IMPLEMENT_DYNCREATE(CMemberList, CFrameWnd)

CMemberList::CMemberList()
{
}

CMemberList::~CMemberList()
{
	cui.m_pvMembersWnd = NULL;   // so nobody tries to clear the control!
}


BEGIN_MESSAGE_MAP(CMemberList, CFrameWnd)
	//{{AFX_MSG_MAP(CMemberList)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
	ON_WM_SIZE()
	ON_NOTIFY(LVN_ITEMCHANGED,1, OnItemchangedList1)
	ON_NOTIFY(NM_KILLFOCUS,1,OnLVKillFocus)
	ON_NOTIFY(NM_DBLCLK,1,OnDblClick)
	ON_NOTIFY(LVN_KEYDOWN,1,OnKeyDown)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMemberList message handlers

BOOL CMemberList::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class
	DWORD dwStyle = WS_CHILD | WS_VISIBLE /*| WS_VSCROLL | WS_HSCROLL| LVS_NOSCROLL |*/| LVS_ICON | 
	/*	LVS_SINGLESEL |*/ LVS_AUTOARRANGE /*| LVS_SORTASCENDING  */ | LVS_SHOWSELALWAYS;
	m_MemberListBox.Create(dwStyle, CRect(0, 0, 100, 100), this, 1);
	m_MemberListBox.SetBkColor(COLORREF(RGB(255,255,255)));
	m_MemberListBox.SetTextBkColor(COLORREF(RGB(255,255,255)));
	m_ImageList.Create(40,40,FALSE,5,5);
	m_MemberListBox.SetImageList(&m_ImageList,LVSIL_NORMAL);
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


void CMemberList::OnContextMenu(CWnd* pWnd, CPoint point) 
{
	int index = -1;
//	extern USHORT mousedAvatarID;
	CUserInfo *LookupPui(const char *);
	extern CUserInfo *mousedPui;
	extern USHORT GetAvatarIDFromNickname(LPCTSTR nickname);
	extern void UpdateIgnore(CMenu &);
	CMenu menu;
	POINT clientPoint = point;
	ScreenToClient(&clientPoint);  // moved above the first if/else block -djk

	if(point.x == -1 && point.y == -1)  // then we must be invoking menu with Shift+F10
	{
		// cant do this with multiple selections
		if(m_MemberListBox.GetSelectedCount() > 1)
		{
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

	if (index == -1)
	{
		if(GetChatApp()->m_bComicView)
		{
			menu.LoadMenu(IDR_MEMBERCONTEXT);
			menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
											   point.x, point.y, (CWnd *)cui.m_pvFrameWnd);
		}
		return;
	}
	CString nickname;
	nickname = m_MemberListBox.GetItemText(index, 0);
//	mousedAvatarID = GetAvatarIDFromNickname(nickname);
	mousedPui = LookupPui(nickname);

extern CUserInfo* puiSelf;
	
	if (mousedPui) {
		if(puiSelf->IsOperator())
			menu.LoadMenu(IDR_MEMBERADMIN);
		else
			menu.LoadMenu(IDR_AVATARCONTEXT);
		UpdateIgnore(menu);
		menu.GetSubMenu(0)->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RIGHTBUTTON,
										   point.x, point.y, (CWnd *)cui.m_pvChatView);
	}
}

// Gets called when something is selected in the list view
void CMemberList::OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!theApp.m_bComicView) {
		*pResult = 0;		// why?
		return;
	}

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	
	int index = pNMListView->iItem; // index of item sending notification
	UINT uChanged = pNMListView->uChanged; // what has changed?  Should be state
	UINT uNewState = pNMListView->uNewState; // what has it changed to?  Should be focus and selected
	UINT uOldState = pNMListView->uOldState;
	
	//Find out the avatar being selected in case we need it.
	CString nickname = m_MemberListBox.GetItemText(index, 0);
	CUserInfo *LookupPui(const char *);
	extern CUserInfo *mousedPui;
	mousedPui = LookupPui(nickname);
	if (!mousedPui) return;   // logged out?
	unsigned short avID = mousedPui->GetAvatarID();

//	extern USHORT mousedAvatarID;
	// First check if it has just been selected
	if((uChanged == LVIF_STATE) && (uNewState & LVIS_SELECTED) && ~(uOldState & LVIS_SELECTED))
	{
		if (theApp.m_bComicView) MyAvatar()->SelectTalkTo(avID, TRUE, FALSE);
//		mousedAvatarID = avID;
	}
	// Check if it has just been deselected
	else if((uChanged == LVIF_STATE) && ~(uNewState & LVIS_SELECTED) && (uOldState & LVIS_SELECTED)) {
		if (theApp.m_bComicView) MyAvatar()->DeselectTalkTo(avID, FALSE);
		mousedPui = NULL;
	}

	*pResult = 0;
}

void CMemberList::OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult)
{
	LV_KEYDOWN* pLVNKeyDown = (LV_KEYDOWN*)pNMHDR;
	WORD wKeyCode = pLVNKeyDown->wVKey;
	if(wKeyCode == TAB)
	{
		::SetFocus(GetSay()->GetSayEdit());
	}
	else if(wKeyCode == SHIFTTAB)
	{
		if(!GetChatApp()->m_bComicView)
			GetTextView()->SetFocus();
		else
			::SetFocus(GetSay()->GetSayEdit());
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
	void ChatGetInfo(CUserInfo *);
	CUserInfo *pui, *GetSingleSelectedMember();

	pui = GetSingleSelectedMember();
	if (pui) {
		if (pui->IsComicUser())
			ChatGetInfo(pui);
		else {
			CString mesg;
			mesg.LoadString(IDS_NOTCOMICSUSER);
			VERIFY(ReplaceToken(mesg, CString("%1"), pui->GetName()));
			AfxMessageBox(mesg);
		}
	}
}

int CMemberList::GetSortPosition(const char *name) {
	int nItems = m_MemberListBox.GetItemCount();
	for (int i = 0; i < nItems; i++) {
		CString label = m_MemberListBox.GetItemText(i, 0);
		if (stricmp(name, label) < 0) return i;
	}

	return nItems;
}
void GetSelectedPuis(CPtrArray &selections) {
	extern CUserInfo *LookupPui(const char *), *puiSelf;
	selections.RemoveAll();
	int nItems = 0;
	int index = -1;
	CListCtrl *members = &GetMembers()->m_MemberListBox;
	do {
		index = members->GetNextItem(index, LVNI_SELECTED);
		if (index == -1) break;
		CString label = members->GetItemText(index, 0);
		CUserInfo *pui = LookupPui(label);
		if (pui && pui != puiSelf) selections.Add(pui);
	} while (nItems++ < 10); // only handle first 10 (hack... revisit this. avoid overrunning IRC mesg limit)
}
