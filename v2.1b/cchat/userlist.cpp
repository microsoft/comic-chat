// UserList.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "userinfo.h"
#include "UserList.h"
#include "chatprot.h"
#include <winnls.h>

#include "ui.h"
#include "resource.h"
#include "mschat.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define MAXNICK	50

extern CChatApp theApp;
const char *DecodeNick(const char *);

/////////////////////////////////////////////////////////////////////////////
// CUserList dialog

CUserList::CUserList(CUserListPersist *persist, CWnd* pParent /*=NULL*/)
	: CCSDialog(CUserList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUserList)
	m_strUser = _T("");
	//}}AFX_DATA_INIT

	m_persist = persist;
//	m_strTopic = persist->m_strTopicFilter;
	m_strUser = persist->m_strUserFilter;
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
	DDX_Control(pDX, IDC_RESET_LIST, m_reset);
	DDX_Control(pDX, IDC_ROOM_CAPTION, m_ctlCaption);
	DDX_Control(pDX, IDC_SEARCH_EDIT, m_user);
	DDX_Control(pDX, IDC_USERLIST, m_userListCtrl);
	DDX_Text(pDX, IDC_SEARCH_EDIT, m_strUser);
	DDV_MaxChars(pDX, m_strUser, 100);
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
	if (m_persist->m_cachedServer != GetMyServer()) {  // if not cached for this server...
		m_persist->Reset(); // clear out everything, including filters...
//		OnResetList();      // do an initial reset
		m_persist->m_cachedServer = GetMyServer();
	}
	
	Load(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUserList::AnnounceCount() {
	CString label;
	CString count;
	count.Format("%d", m_userListCtrl.GetItemCount());
	label.LoadString(IDS_NUM_USERS);
	VERIFY(ReplaceToken(label, CString("%1"), count));
	m_ctlCaption.SetWindowText(label);
}

static int sortColumn = 0;
static BOOL sortAscending = TRUE;

int __cdecl StringCompare2(const void *r1, const void *r2) {
	int rval;
	CUser *user1 = (* ((CUser **) r1));
	CUser *user2 = (* ((CUser **) r2));
	if (sortColumn == 0) {
		rval = stricmp(user1->GetPrettyNick(), user2->GetPrettyNick());
//		TRACE("%s vs %s = %d\n", room1->m_name, room2->m_name, rval);
	}
	else if (sortColumn == 1)
		rval = stricmp(user1->m_ident, user2->m_ident);
	else if (sortColumn == 2)
		rval = stricmp(user1->m_fullName, user2->m_fullName);
	else if (sortColumn == 3)
		rval = stricmp(user1->m_prettyRoom, user2->m_prettyRoom);
	if (!sortAscending) rval = -rval;
	if (!rval) rval = stricmp(user1->GetPrettyNick(), user2->GetPrettyNick());  // sort on name if descrs identical
	return rval;
}

void CUserList::Sort(BOOL bResetList) {
	m_persist->Sort();
	Load(bResetList);
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
	else ((CButton *)GetDlgItem(IDC_USERSEARCH_ALL))->SetCheck(1);

	BOOL showEdit = (m_persist->m_searchType != USERSEARCH_ALL);
	GetDlgItem(IDC_SEARCH_LABEL)->EnableWindow(showEdit);
	GetDlgItem(IDC_SEARCH_EDIT)->EnableWindow(showEdit);

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
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUserListCtrl message handlers

#define ROOMGROWNUM 2000

int CUserListPersist::AddUser(CUser *room) {
	if (!m_users) {
		m_usersSize = ROOMGROWNUM;
		m_users = (CUser **) malloc (sizeof (CUser *) * ROOMGROWNUM);
	} else if (m_nUsers >= m_usersSize) {
		TRACE("nUsers = %d, m_usersSize = %d, but allocating %d more.\n", m_nUsers, m_usersSize, ROOMGROWNUM);
		m_usersSize += ROOMGROWNUM;
		m_users = (CUser **) realloc (m_users, sizeof (CUser *) * m_usersSize);
	}
	int rval = m_nUsers;
	m_users[m_nUsers++] = room;
	return rval;
}

CUserListPersist::~CUserListPersist() {
	MakeEmpty();
}

void CUserListPersist::MakeEmpty() {
	if (!m_users) return;

	for (int i = 0; i < m_nUsers; i++)
		delete m_users[i];

	free(m_users);
	m_users = NULL;
	m_nUsers = m_usersSize = 0;
}

void CUserListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

	LV_ITEM *lv = &pDispInfo->item;
	CUserList *rl = (CUserList *) GetParent();
	CUser *user = (CUser *) rl->m_persist->m_users[lv->lParam];
//	TRACE("Doing callback on %s (lParam = %d).\n", room->m_name, lv->lParam);
	if (lv->iSubItem == 0) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, user->GetPrettyNick(), maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	} else if (lv->iSubItem == 1) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, user->m_ident, maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	} else if (lv->iSubItem == 2) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, user->m_fullName, maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	} else if (lv->iSubItem == 3) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, user->m_prettyRoom, maxChar);
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
	if (!currentRoom || currentRoom->GetConnectionStatus() != CX_INCHANNEL) {
		AfxMessageBox(IDS_OUTCHANNEL_INVITE);
	} else {
		extern const char *GetMyChannel();
		const char *nick = m_userListCtrl.GetSelectedNickname();
		if (!nick) return;
		currentRoom->ChatSendInvitation(UnConst(nick));
	}	
}

CUser *CUserListCtrl::GetSelectedUser() {
	if (GetSelectedCount() != 1) return NULL;
	int index = GetNextItem(-1,LVNI_SELECTED);
	ASSERT(index >= 0);
	int userNo = GetItemData(index);
	
	CUserList *rl = (CUserList *) GetParent();
	CUser *user = (CUser *) rl->m_persist->m_users[userNo];
	
	return user;
}

const char *CUserListCtrl::GetSelectedNickname() {
	CUser *user = GetSelectedUser();
	if (user) return user->m_nick;
	else return NULL;
}

void CUserList::OnItemchangedUserlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	BOOL bCanInvite();
	const char *selectedNick, *GetMyName();
	CUser *user = m_userListCtrl.GetSelectedUser();
	if (user) selectedNick = user->m_nick;
	else selectedNick = NULL;

	BOOL isOther = selectedNick && (strcmp(selectedNick, GetMyNickName()) != 0);
	GetDlgItem(IDC_INVITE_FROM_LIST)->EnableWindow(isOther && bCanInvite());
	GetDlgItem(IDC_MESSAGE_FROM_LIST)->EnableWindow(isOther);

	BOOL canJoin = FALSE;
	if (isOther) {
		if (CHANNELPREFIX(user->m_room[0]))
			canJoin = TRUE;
	}
	GetDlgItem(IDC_JOINROOM)->EnableWindow(canJoin);
	
	*pResult = 0;
}


void CUserList::OnUsersearchAll() 
{
	m_persist->m_searchType = USERSEARCH_ALL;
	GetDlgItem(IDC_SEARCH_LABEL)->EnableWindow(FALSE);
	GetDlgItem(IDC_SEARCH_EDIT)->EnableWindow(FALSE);
}

void CUserList::OnUsersearchIdentity() 
{
	m_persist->m_searchType = USERSEARCH_ID;
	GetDlgItem(IDC_SEARCH_LABEL)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEARCH_EDIT)->EnableWindow(TRUE);
}

void CUserList::OnUsersearchNick() 
{
	m_persist->m_searchType = USERSEARCH_NICK;
	GetDlgItem(IDC_SEARCH_LABEL)->EnableWindow(TRUE);
	GetDlgItem(IDC_SEARCH_EDIT)->EnableWindow(TRUE);
}

void CUserList::OnMessageFromList() 
{
	CUser *user = m_userListCtrl.GetSelectedUser();
	if (user) {
		const char *nick = user->m_nick;
		m_selUser = new CUserInfo(user->m_nick, user->m_ident);
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
	BOOL bSwitchToRoom(const char *newRoom, const char *password = NULL, BOOL bEncode = FALSE);

	CUser *user = m_userListCtrl.GetSelectedUser();
	if (!user)
		return;
	EndDialog(0);

	bSwitchToRoom(user->m_room);
}
