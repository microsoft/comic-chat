// UserList.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "binddoc.h"
#include "chatDoc.h"
#include "userinfo.h"
#include "UserList.h"
#include "chatprot.h"
#include "actions.h"
#include <winnls.h>

#include "ui.h"
#include "resource.h"
#include "mschat.h"
#include "protsupp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXNICK	50

extern CChatApp theApp;
const char *DecodeNick(const char *);


void CUser::Release()
{
	ASSERT(m_nRefCount > 0);

	if (--m_nRefCount == 0)
		delete this;
}



/////////////////////////////////////////////////////////////////////////////
// CUserList dialog

CUserList::CUserList(CUserListPersist *persist, CWnd* pParent /*=NULL*/)
	: CCSDialog(CUserList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserList)
	//}}AFX_DATA_INIT

	m_persist = persist;
	m_strUser = persist->m_strUserFilter;
	m_strRoom = persist->m_strRoomFilter;
//	m_bSearchDescrs = m_persist->m_bSearchDescrs;
	m_selUser = NULL;
}

CUserList::~CUserList() {
	if (m_selUser) delete m_selUser;
}

void CUserList::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUserList)
	DDX_Control(pDX, IDC_ROOM_EDIT, m_ctlRoom);
	DDX_Control(pDX, IDC_RESET_LIST, m_reset);
	DDX_Control(pDX, IDC_ROOM_CAPTION, m_ctlCaption);
	DDX_Control(pDX, IDC_SEARCH_EDIT, m_user);
	DDX_Control(pDX, IDC_USERLIST, m_userListCtrl);
	DDX_Text(pDX, IDC_SEARCH_EDIT, m_strUser);
	DDV_MaxChars(pDX, m_strUser, 100);
	DDX_Text(pDX, IDC_ROOM_EDIT, m_strRoom);
	DDV_MaxChars(pDX, m_strRoom, MAX_IRCXCHANNAME);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUserList, CCSDialog)
	//{{AFX_MSG_MAP(CUserList)
	ON_BN_CLICKED(IDC_RESET_LIST, OnResetList)
	ON_BN_CLICKED(IDC_INVITE_FROM_LIST, OnInviteFromList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_USERLIST, OnItemchangedUserlist)
	ON_BN_CLICKED(IDC_USERSEARCH_ALL, OnUsersearchAll)
	ON_BN_CLICKED(IDC_USERSEARCH_IDENTITY, OnUsersearchIdentity)
	ON_BN_CLICKED(IDC_USERSEARCH_NICK, OnUsersearchNick)
	ON_BN_CLICKED(IDC_MESSAGE_FROM_LIST, OnMessageFromList)
	ON_BN_CLICKED(IDC_JOINROOM, OnJoinRoom)
	ON_BN_CLICKED(IDC_USERSEARCH_ROOM, OnUsersearchRoom)
	ON_BN_CLICKED(IDC_CLOSE_USERLIST, OnCloseDialog)
	ON_EN_CHANGE(IDC_ROOM_EDIT, OnChangeRoomEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserList message handlers

void CUserList::OnResetList() 
{
	char buff[50];
	void ChatFillUserList(CUserList *);

	m_persist->m_searchTime.LoadString(IDS_SEARCH_TIME);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, buff, sizeof(buff));
	ReplaceToken(m_persist->m_searchTime, CString("%1"), buff);

	m_userListCtrl.DeleteAllItems();
	m_persist->MakeEmpty();

	theApp.m_bInSearch = TRUE;
	m_bResetHadFocus = m_reset.GetState() & 0x08;
	m_reset.EnableWindow(FALSE);
	ChatFillUserList(this);	
}


BOOL CUserList::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	CString labelStr;
	CString widthStr;

	m_userListCtrl.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, 
		LVS_EX_FULLROWSELECT);

	m_userListCtrl.SetFont(&theApp.m_fontGui);
	m_user.SetFont(&theApp.m_fontGui);

	labelStr.LoadString(ID_UL_NICK_LABEL);
	widthStr.LoadString(ID_UL_NICK_WIDTH);
	m_userListCtrl.InsertColumn(0, labelStr, LVCFMT_LEFT, atoi(widthStr));

	labelStr.LoadString(ID_UL_IDENT_LABEL);
	widthStr.LoadString(ID_UL_IDENT_WIDTH);
	m_userListCtrl.InsertColumn(1, labelStr, LVCFMT_LEFT, atoi(widthStr),1);

	labelStr.LoadString(ID_UL_REALNAME_LABEL);
	widthStr.LoadString(ID_UL_REALNAME_WIDTH);
	m_userListCtrl.InsertColumn(2, labelStr, LVCFMT_LEFT, atoi(widthStr),2);

	labelStr.LoadString(ID_UL_ROOM_LABEL);
	widthStr.LoadString(ID_UL_ROOM_WIDTH);
	m_userListCtrl.InsertColumn(3, labelStr, LVCFMT_LEFT, atoi(widthStr),3);

	m_userListCtrl.SetItemCount(2000);  // on this order... Prepare for the deluge...


	extern const char *GetMyServer();
	if (!m_persist->m_strQuery.IsEmpty() || !m_persist->m_strEncRoom.IsEmpty())
		OnResetList();
	else if (m_persist->m_cachedServer != GetMyServer())    // if not cached for this server...
		m_persist->Reset(); // clear out everything, including filters...


	m_persist->m_cachedServer = GetMyServer();

	if (!m_persist->m_strQuery.IsEmpty()) {
		CString strTitle;
		GetWindowText(strTitle);
		strTitle += ": ";
		strTitle += m_persist->m_strQuery;
		strTitle.TrimRight(); //strTitle.Left(strTitle.Length()-2);
		SetWindowText(strTitle);
	}
	
	Load(FALSE);

	switch (m_persist->m_searchType)
	{
		case USERSEARCH_NICK:
		case USERSEARCH_ID: 
			GetDlgItem (IDC_SEARCH_EDIT)->SetFocus ();
			return FALSE;
		case USERSEARCH_ROOM: 
			GetDlgItem (IDC_ROOM_EDIT)->SetFocus ();
			return FALSE;
	}
	return TRUE;
}

void CUserList::AnnounceCount()
{
	CString strCount;
	strCount.Format(IDS_NUM_USERS, m_userListCtrl.GetItemCount());
	m_ctlCaption.SetWindowText(strCount);
}

static int sortColumn = 0;
static BOOL sortAscending = TRUE;

int __cdecl StringCompare2(const void *r1, const void *r2)
{
	int		rval;
	CUser*	pUser1 = (* ((CUser **) r1));
	CUser*	pUser2 = (* ((CUser **) r2));

	CString strPrettyNick1, strPrettyNick2;

	strPrettyNick1 = pUser1->GetPrettyNick();
	strPrettyNick2 = pUser2->GetPrettyNick();

	TrimQuotes(strPrettyNick1);
	TrimQuotes(strPrettyNick2);

	if (sortColumn == 0)
		rval = stricmp(strPrettyNick1, strPrettyNick2);
	else if (sortColumn == 1)
		rval = stricmp(pUser1->m_strIdentity, pUser2->m_strIdentity);
	else if (sortColumn == 2)
		rval = stricmp(pUser1->m_strFullName, pUser2->m_strFullName);
	else if (sortColumn == 3)
		rval = stricmp(pUser1->m_strPrettyRoom, pUser2->m_strPrettyRoom);

	if (!rval && sortColumn != 0)
		rval = stricmp(strPrettyNick1, strPrettyNick2);  // sort on name if descrs identical

	return sortAscending ? rval : -rval;
}

void CUserList::Sort(BOOL bResetList) {
	m_persist->Sort();
	Load(bResetList);
}

void CUserList::ShowAndEnableControl(
UINT nID,
BOOL bShowAndEnable)
{
	HWND hwnd = ::GetDlgItem (m_hWnd, nID);
	// Don't show in this version
	//::ShowWindow (hwnd, bShowAndEnable ? SW_SHOW : SW_HIDE);
	::EnableWindow (hwnd, bShowAndEnable);
}

void CUserList::Load(BOOL bResetList) {
	if (bResetList) m_userListCtrl.DeleteAllItems();

	GetDlgItem(IDC_INVITE_FROM_LIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_MESSAGE_FROM_LIST)->EnableWindow(FALSE);
	GetDlgItem(IDC_JOINROOM)->EnableWindow(FALSE);

	if (m_persist->m_searchType == USERSEARCH_ID) 
		((CButton *)GetDlgItem(IDC_USERSEARCH_IDENTITY))->SetCheck(1);
	else if (m_persist->m_searchType == USERSEARCH_NICK)
		((CButton *)GetDlgItem(IDC_USERSEARCH_NICK))->SetCheck(1);
	else if (m_persist->m_searchType == USERSEARCH_ALL)
		((CButton *)GetDlgItem(IDC_USERSEARCH_ALL))->SetCheck(1);
	else if (m_persist->m_searchType == USERSEARCH_ROOM)
		((CButton *)GetDlgItem(IDC_USERSEARCH_ROOM))->SetCheck(1);

	BOOL showEdit1 = (m_persist->m_searchType == USERSEARCH_NICK) ||
					  (m_persist->m_searchType == USERSEARCH_ID);
	BOOL showEdit2 = m_persist->m_searchType == USERSEARCH_ROOM;
	BOOL bAllowChoice = m_persist->m_strQuery.IsEmpty();

	ShowAndEnableControl (IDC_SEARCH_LABEL, showEdit1 && bAllowChoice);
	ShowAndEnableControl (IDC_SEARCH_EDIT, showEdit1 && bAllowChoice);
	ShowAndEnableControl (IDC_ROOM_LABEL, showEdit2 && bAllowChoice);
	ShowAndEnableControl (IDC_ROOM_EDIT, showEdit2 && bAllowChoice);
	GetDlgItem(IDC_USERSEARCH_IDENTITY)->EnableWindow(bAllowChoice);
	GetDlgItem(IDC_USERSEARCH_NICK)->EnableWindow(bAllowChoice);
	GetDlgItem(IDC_USERSEARCH_ALL)->EnableWindow(bAllowChoice);
	GetDlgItem(IDC_USERSEARCH_ROOM)->EnableWindow(bAllowChoice);
	GetDlgItem(IDC_GROUP0)->EnableWindow(bAllowChoice);

	for (int i = 0; i < m_persist->m_nUsers; i++)
		AddToUserList(i);

	AnnounceCount();
	AnnounceTime();
}

void CUserListPersist::Sort() {
	sortColumn = m_sortColumn;
	sortAscending = m_sortAscending;
	TRACE("Sort ascending = %d, col = %d.\n", m_sortColumn, m_sortAscending);

	qsort(m_users, m_nUsers, sizeof(CUser *), StringCompare2);
}

void CUserList::AddToUserList(int index) {
//	if (!MatchesTopicFilter(m_persist->m_rooms[roomIndex], m_strTopic, m_bSearchDescrs))
//		return;			// only add to list if meets topic filter
	// djk - add back

	LV_ITEM lv;
	lv.iItem = m_userListCtrl.GetItemCount();  // add to end
	lv.iSubItem = 0;
	lv.mask = LVIF_TEXT | LVIF_PARAM;
	lv.pszText = /* UnConst((const char *) room->m_name) */ LPSTR_TEXTCALLBACK; // should work, but doesn't allow sort!!!
//	lv.state = lv.stateMask = 0;
	lv.lParam = index;
//	TRACE("Adding room %s at pos %d.\n", room->m_name, lv.lParam);
	VERIFY(m_userListCtrl.InsertItem(&lv) != -1);

	lv.iSubItem = 1;
	lv.pszText = LPSTR_TEXTCALLBACK; // numBuff;
	VERIFY(m_userListCtrl.SetItem(&lv) != -1); // want to SetItem here since we already Inserted Item previously

	lv.iSubItem = 2;
	lv.pszText = LPSTR_TEXTCALLBACK; // numBuff;
	VERIFY(m_userListCtrl.SetItem(&lv) != -1); // want to SetItem here since we already Inserted Item previously

	lv.iSubItem = 3;
	lv.pszText = LPSTR_TEXTCALLBACK; // numBuff;
	VERIFY(m_userListCtrl.SetItem(&lv) != -1); // want to SetItem here since we already Inserted Item previously
}

const DWORD CUserList::m_nHelpIDs[] =
{
	IDC_USERSEARCH_ALL,				IDH_ALL_USER,
	IDC_USERSEARCH_NICK,			IDH_SEARCH_NICKNAME,
	IDC_USERSEARCH_IDENTITY,		IDH_IDENTITY,
	IDC_RESET_LIST,					IDH_USER_UPDATE,
	IDC_INVITE_FROM_LIST,			IDH_USER_INVITE,
	IDC_MESSAGE_FROM_LIST,			IDH_WHISPER_BOX,
	IDC_JOINROOM,					IDH_USERLIST_JOINUSER,
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CUserListCtrl

CUserListCtrl::CUserListCtrl()
{
}

CUserListCtrl::~CUserListCtrl()
{
}


BEGIN_MESSAGE_MAP(CUserListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CUserListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserListCtrl message handlers

#define USERGROWNUM 2000

int CUserListPersist::AddUser(CUser *user) {
	if (!m_users) {
		m_usersSize = USERGROWNUM;
		m_users = (CUser **) malloc (sizeof (CUser *) * USERGROWNUM);
	} else if (m_nUsers >= m_usersSize) {
		TRACE("nUsers = %d, m_usersSize = %d, but allocating %d more.\n", m_nUsers, m_usersSize, USERGROWNUM);
		m_usersSize += USERGROWNUM;
		m_users = (CUser **) realloc (m_users, sizeof (CUser *) * m_usersSize);
	}
	int rval = m_nUsers;
	m_users[m_nUsers++] = user;
	return rval;
}

CUserListPersist::~CUserListPersist() {
	MakeEmpty();
}

void CUserListPersist::MakeEmpty() {
	if (!m_users) return;

	for (int i = 0; i < m_nUsers; i++)
		m_users[i]->Release();

	free(m_users);
	m_users = NULL;
	m_nUsers = m_usersSize = 0;
}

void CUserListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	LV_ITEM *lv = &pDispInfo->item;
	CUserList *rl = (CUserList *) GetParent();
	CUser *pUser = (CUser *) rl->m_persist->m_users[lv->lParam];
	if (lv->iSubItem == 0) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, pUser->GetPrettyNick(), maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	} else if (lv->iSubItem == 1) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, pUser->m_strIdentity, maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	} else if (lv->iSubItem == 2) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, pUser->m_strFullName, maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	} else if (lv->iSubItem == 3) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, pUser->m_strPrettyRoom, maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	}

	*pResult = 0;
}

// essentially identical to CRoomListCtrl::OnColumnclick
void CUserListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CUserList *rl = (CUserList *) GetParent();
	CUserListPersist *persist = rl->m_persist;

	if (persist->m_sortColumn == pNMListView->iSubItem) // toggle sortOrder
		persist->m_sortAscending = !persist->m_sortAscending;
	else {												  // change sortColumn
		persist->m_sortColumn = pNMListView->iSubItem;
		persist->m_sortAscending = TRUE;				  // sort ascendingly first
	}
	rl->Sort(TRUE);
	*pResult = 0;
}


void CUserList::OnInviteFromList() 
{
	if (!currentRoom || currentRoom->GetConnectionStatus() != CX_INCHANNEL)
		AfxMessageBox(IDS_OUTCHANNEL_INVITE);
	else
	{
		const char *szNick = m_userListCtrl.GetSelectedNickname();
		if (!szNick)
			return;
		currentRoom->ChatSendInvitation(UnConst(szNick));
	}	
}

CUser *CUserListCtrl::GetSelectedUser()
{
	if (GetSelectedCount() != 1)
		return NULL;
	int iIndex = GetNextItem(-1,LVNI_SELECTED);
	ASSERT(iIndex >= 0);
	int iUserNo = GetItemData(iIndex);
	
	CUserList *rl = (CUserList *) GetParent();
	CUser *pUser = (CUser*) rl->m_persist->m_users[iUserNo];
	
	return pUser;
}

const char* CUserListCtrl::GetSelectedNickname()
{
	CUser *pUser = GetSelectedUser();
	if (pUser)
		return pUser->m_strNickname;
	else
		return NULL;
}

void 
CUserListCtrl::OnSetFocus(
CWnd* pOldWnd)
{
	// Set focus item if there isn't any.
	int iIndex = GetNextItem(-1, LVNI_FOCUSED);
	if (iIndex == -1)
		SetItemState(0, LVIS_FOCUSED, LVIS_FOCUSED);
	CListCtrl::OnSetFocus (pOldWnd);
}

void CUserList::OnItemchangedUserlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOL		bCanInvite();
	const char*	szSelectedNick;
	CUser*		pUser = m_userListCtrl.GetSelectedUser();

	if (pUser)
		szSelectedNick = pUser->m_strNickname;
	else
		szSelectedNick = NULL;

	BOOL bCanJoin = FALSE, bInCurrentRoom = FALSE;
	BOOL bIsOther = szSelectedNick && (strcmp(szSelectedNick, GetMyNickName()) != 0);

	// Make sure selected user is not in current room already
	CChatDoc*	pDoc = GetChatDoc();

	if (bIsOther && pDoc && pDoc->GetConnectionStatus() == CX_INCHANNEL) 
	{
		// try to interpret as utterance in current room
		CUserInfo* pui = LookupPui(szSelectedNick, pDoc);
		bInCurrentRoom = pui && !pui->IsDeparted();
	}

	GetDlgItem(IDC_INVITE_FROM_LIST)->EnableWindow(bIsOther && bCanInvite() && !bInCurrentRoom);
	GetDlgItem(IDC_MESSAGE_FROM_LIST)->EnableWindow(bIsOther);

	if (bIsOther)
	{
		// is selected user in a channel?
		if (CHANNELPREFIX(pUser->m_strRoom[0]))
		{
			// make sure i'm not in that channel myself
			pDoc = LookupDoc(pUser->m_strRoom);
			bCanJoin = !pDoc || pDoc->GetConnectionStatus() != CX_INCHANNEL || pDoc != GetChatDoc();
		}
	}
	GetDlgItem(IDC_JOINROOM)->EnableWindow(bCanJoin);
	
	*pResult = 0;
}


void CUserList::OnUsersearchAll() 
{
	m_persist->m_searchType = USERSEARCH_ALL;
	ShowAndEnableControl (IDC_SEARCH_LABEL, FALSE);
	ShowAndEnableControl (IDC_SEARCH_EDIT, FALSE);
	ShowAndEnableControl (IDC_ROOM_LABEL, FALSE);
	ShowAndEnableControl (IDC_ROOM_EDIT, FALSE);
}

void CUserList::OnUsersearchIdentity() 
{
	m_persist->m_searchType = USERSEARCH_ID;
	ShowAndEnableControl (IDC_SEARCH_LABEL, TRUE);
	ShowAndEnableControl (IDC_SEARCH_EDIT, TRUE);
	ShowAndEnableControl (IDC_ROOM_LABEL, FALSE);
	ShowAndEnableControl (IDC_ROOM_EDIT, FALSE);
}

void CUserList::OnUsersearchNick() 
{
	m_persist->m_searchType = USERSEARCH_NICK;
	ShowAndEnableControl (IDC_SEARCH_LABEL, TRUE);
	ShowAndEnableControl (IDC_SEARCH_EDIT, TRUE);
	ShowAndEnableControl (IDC_ROOM_LABEL, FALSE);
	ShowAndEnableControl (IDC_ROOM_EDIT, FALSE);
}


void CUserList::OnUsersearchRoom() 
{
	m_persist->m_searchType = USERSEARCH_ROOM;
	ShowAndEnableControl (IDC_SEARCH_LABEL, FALSE);
	ShowAndEnableControl (IDC_SEARCH_EDIT, FALSE);
	ShowAndEnableControl (IDC_ROOM_LABEL, TRUE);
	ShowAndEnableControl (IDC_ROOM_EDIT, TRUE);
}

void CUserList::OnMessageFromList() 
{
	CUser *pUser = m_userListCtrl.GetSelectedUser();
	
	if (pUser)
	{
		ASSERT(!m_selUser);
		m_selUser = new CUserInfo(pUser->m_strNickname, pUser->m_strIdentity);
	}

	EndDialog(LAUNCH_WHISPERBOX);   // since whisperbox is not modal, but userlist is, we need to do this
	// if we launch the whisperbox before the dialog closed, then there's the modal / non-modal inconsistency,
	// since a non-modal would be launched by a modal.
	// Also, we had a problem with the whisperbox being moved under the app when the first DBCS characters
	// were typed into the whisperbox IME, unless we closed the user list first.  So now we launch
	// the whisperbox from CChatDoc::OnUserList.
}


void CUserList::OnJoinRoom() 
{
	CUser *pUser = m_userListCtrl.GetSelectedUser();
	if (!pUser)
		return;

	EndDialog(0);

	g_bEnterOnCreate = FALSE;
	bSwitchToRoom(pUser->m_strRoom);
}


void CUserList::OnCloseDialog()
{
	EndDialog(0);
}


void CUserList::OnChangeRoomEdit() 
{
	m_persist->m_strEncRoom = "";		// overriding encoded form set via roomlist	
}


BOOL 
CUserList::PreTranslateMessage(
MSG* pMsg)
{
	// Trap the F5 key, for an instant refresh.
	if ((pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) && pMsg->wParam == VK_F5)
	{
		if (pMsg->message == WM_KEYDOWN && LOWORD(pMsg->lParam) == 1) // Don't do it on repeat keystrokes
		{
			PostMessage (WM_COMMAND, (WPARAM)MAKELONG(IDC_RESET_LIST, BN_CLICKED), (LPARAM)m_reset.m_hWnd);
		}
		return TRUE;
	}
	else
		return CCSDialog::PreTranslateMessage (pMsg);
}
