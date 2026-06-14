// RoomList.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "setupdlg.h"
#include "RoomList.h"
#include <winnls.h>
#include "resource.h"
#include "mschat.h"
#include "mbstring.h"
#include "ircproto.h"
#include "protsupp.h"

extern CChatApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoomList dialog


CRoomList::CRoomList(CRoomListPersist *persist, CWnd* pParent /*=NULL*/)
	: CCSDialog(CRoomList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRoomList)
	m_strTopic = _T("");
	m_bSearchDescrs = FALSE;
	m_minMembers = 0;
	m_maxMembers = 9999;
	m_bRegisteredOnly = FALSE;
	//}}AFX_DATA_INIT

	m_persist = persist;
}


void CRoomList::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoomList)
	DDX_Control(pDX, IDC_SPIN_MAX, m_spinMax);
	DDX_Control(pDX, IDC_SPIN_MIN, m_spinMin);
	DDX_Control(pDX, IDC_RESET_LIST, m_reset);
	DDX_Control(pDX, IDC_ROOM_CAPTION, m_ctlCaption);
	DDX_Control(pDX, IDC_SEARCH_DESCRS, m_ctrlSearchDescrs);
	DDX_Control(pDX, IDC_TOPIC_EDIT, m_topicEdit);
	DDX_Control(pDX, IDC_ROOMLIST, m_roomList);
	DDX_Text(pDX, IDC_TOPIC_EDIT, m_strTopic);
	DDX_Check(pDX, IDC_SEARCH_DESCRS, m_bSearchDescrs);
	DDX_Text(pDX, IDC_MIN_MEMBERS, m_minMembers);
	DDV_MinMaxUInt(pDX, m_minMembers, 0, 9999);
	DDX_Text(pDX, IDC_MAX_MEMBERS, m_maxMembers);
	DDV_MinMaxUInt(pDX, m_maxMembers, 0, 9999);
	DDX_Check(pDX, IDC_REGISTERED_ONLY, m_bRegisteredOnly);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRoomList, CCSDialog)
	//{{AFX_MSG_MAP(CRoomList)
	ON_BN_CLICKED(IDC_RESET_LIST, OnResetList)
	ON_NOTIFY(NM_DBLCLK, IDC_ROOMLIST, OnDblclkRoomlist)
	ON_BN_CLICKED(IDC_GOTO, OnGoto)
	ON_BN_CLICKED(IDC_SEARCH_DESCRS, OnSearchDescrs)
	ON_EN_CHANGE(IDC_TOPIC_EDIT, OnChangeTopicEdit)
	ON_BN_CLICKED(IDC_NEWROOM, OnCreateRoom)
	ON_EN_CHANGE(IDC_MIN_MEMBERS, OnChangeMinMembers)
	ON_EN_CHANGE(IDC_MAX_MEMBERS, OnChangeMaxMembers)
	ON_BN_CLICKED(IDC_REGISTERED_ONLY, OnRegisteredOnly)
	ON_BN_CLICKED(IDC_LISTMEMBERS, OnListmembers)
	ON_BN_CLICKED(IDC_CLOSE_ROOMLIST, OnCloseDialog)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomList message handlers

void CRoomList::OnResetList() 
{
	char szBuff[50];

	m_persist->m_searchTime.LoadString(IDS_SEARCH_TIME);
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, szBuff, sizeof(szBuff));
	ReplaceToken(m_persist->m_searchTime, CString("%1"), szBuff);

	m_roomList.DeleteAllItems();
	m_persist->MakeEmpty();
	theApp.m_bInSearch = TRUE;
	m_bResetHadFocus = m_reset.GetState() & 0x08;
	m_reset.EnableWindow(FALSE);
	GetDlgItem(IDC_GOTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_LISTMEMBERS)->EnableWindow(FALSE);
	ChatFillRoomList(this);
}

BOOL CRoomList::OnInitDialog() 
{
	extern const char *GetMyServer();
	if (m_persist->m_cachedServer != GetMyServer())  // if not cached for this server...
		m_persist->Reset(); // clear out everything, including filters...

	m_strTopic = m_persist->m_strTopicFilter;     // transfer info from m_persist
	m_bSearchDescrs = m_persist->m_bSearchDescrs;
	m_bRegisteredOnly = m_persist->m_bRegisteredOnly && serverConn.m_bIrcXServer;
	m_minMembers = m_persist->m_minMembers;
	m_maxMembers = m_persist->m_maxMembers;

	CCSDialog::OnInitDialog();

	m_roomList.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, 
		LVS_EX_FULLROWSELECT);

	CString labelStr;
	CString widthStr;

	m_roomList.SetFont(&theApp.m_fontGui);
	m_topicEdit.SetFont(&theApp.m_fontGui);

	labelStr.LoadString(ID_RL_ROOM_LABEL);
	widthStr.LoadString(ID_RL_ROOM_WIDTH);
	m_roomList.InsertColumn(0, labelStr, LVCFMT_LEFT, atoi(widthStr));

	labelStr.LoadString(ID_RL_NUSERS_LABEL);
	widthStr.LoadString(ID_RL_NUSERS_WIDTH);
	m_roomList.InsertColumn(1, labelStr, LVCFMT_LEFT, atoi(widthStr),1);

	labelStr.LoadString(ID_RL_DESCR_LABEL);
	widthStr.LoadString(ID_RL_DESCR_WIDTH);
	m_roomList.InsertColumn(2, labelStr, LVCFMT_LEFT, atoi(widthStr),2);

	m_roomList.SetItemCount(2000);  // on this order... Prepare for the deluge...

	if (m_persist->m_cachedServer != GetMyServer()) {  // if not cached for this server...
		OnResetList();      // do an initial reset
		m_persist->m_cachedServer = GetMyServer();
	} 
	else
		LoadRooms(FALSE);

	m_spinMin.SetRange(0, 9999);
	m_spinMax.SetRange(0, 9999);
	((CEdit *)GetDlgItem(IDC_MIN_MEMBERS))->SetLimitText(4);
	((CEdit *)GetDlgItem(IDC_MAX_MEMBERS))->SetLimitText(4);

	GetDlgItem(IDC_REGISTERED_ONLY)->EnableWindow(serverConn.m_bIrcXServer);

	if (!m_persist->m_strQuery.IsEmpty()) {
		CString strTitle;
		GetWindowText(strTitle);
		strTitle += ": ";
		strTitle += m_persist->m_strQuery;
		strTitle.TrimRight(); //strTitle.Left(strTitle.Length()-2);
		SetWindowText(strTitle);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRoomList::LoadRooms(BOOL bResetList) {
	if (bResetList)
		m_roomList.DeleteAllItems();

	for (int i = 0; i < m_persist->m_nRooms; i++)
		AddToRoomList(i);

	AnnounceCount();
	AnnounceTime();

	// REGISB added 11/17/97 
	GetDlgItem(IDC_GOTO)->EnableWindow(FALSE);
	GetDlgItem(IDC_LISTMEMBERS)->EnableWindow(FALSE);
}


/* inline */ char * UnConst(const char * cvs_arg) {  // thanks Andy
    union 
    {
        char *      vs;     
        const char * cvs;
    } cas;

    cas.cvs = cvs_arg;
    return cas.vs;

}

void CRoomList::AddToRoomList(int iRoomIndex) {
	if (!MatchesTopicFilter(m_persist->m_rooms[iRoomIndex], m_strTopic, m_bSearchDescrs))
		return;			// only add to list if meets topic filter

	LV_ITEM lv;
	lv.iItem = m_roomList.GetItemCount();  // add to end
	lv.iSubItem = 0;
	lv.mask = LVIF_TEXT | LVIF_PARAM;
	lv.pszText = /* UnConst((const char *) room->m_name) */ LPSTR_TEXTCALLBACK; // should work, but doesn't allow sort!!!
//	lv.state = lv.stateMask = 0;
	lv.lParam = iRoomIndex;
//	TRACE("Adding room %s at pos %d.\n", room->m_name, lv.lParam);
	VERIFY(m_roomList.InsertItem(&lv) != -1);

	lv.iSubItem = 1;
//	lv.mask = LVIF_TEXT | LVIF_PARAM;
//	char numBuff[10];
//	sprintf(numBuff, "%d", room->m_nUsers);
	lv.pszText = LPSTR_TEXTCALLBACK; // numBuff;
//	lv.state = lv.stateMask = 0;
	VERIFY(m_roomList.SetItem(&lv) != -1); // want to SetItem here since we already Inserted Item previously

	lv.iSubItem = 2;
//	lv.mask = LVIF_TEXT | LVIF_PARAM;
	lv.pszText = LPSTR_TEXTCALLBACK; //UnConst((const char *) room->m_descr);
//	lv.state = lv.stateMask = 0;
	VERIFY(m_roomList.SetItem(&lv) != -1);
}

#define ROOMGROWNUM 2000

int CRoomListPersist::AddRoom(CRoom *room) {
	if (!m_rooms) {
		m_roomsSize = ROOMGROWNUM;
		m_rooms = (CRoom **) malloc (sizeof (CRoom *) * ROOMGROWNUM);
	} else if (m_nRooms >= m_roomsSize) {
		TRACE("nRooms = %d, m_roomSize = %d, but allocating %d more.\n", m_nRooms, m_roomsSize, ROOMGROWNUM);
		m_roomsSize += ROOMGROWNUM;
		m_rooms = (CRoom **) realloc (m_rooms, sizeof (CRoom *) * m_roomsSize);
	}
	int rval = m_nRooms;
	m_rooms[m_nRooms++] = room;
	return rval;
}


static int sortColumn = 0;
static BOOL sortAscending = TRUE;

int CompareRoomNames(CRoom *room1, CRoom* room2) {
	if (room1->m_byteSort < room2->m_byteSort) return -1;
	else if (room1->m_byteSort > room2->m_byteSort) return 1;
	else return stricmp(room1->m_prettyName, room2->m_prettyName);
}

int __cdecl StringCompare(const void *r1, const void *r2) {
	int rval;
	CRoom *room1 = (* ((CRoom **) r1));
	CRoom *room2 = (* ((CRoom **) r2));
	if (sortColumn == 0) {
		rval = CompareRoomNames(room1, room2);
//		TRACE("%s vs %s = %d\n", room1->m_name, room2->m_name, rval);
	} else if (sortColumn == 2) {
		rval = stricmp(room1->m_descr, room2->m_descr);
	}
	if (!sortAscending) rval = -rval;
	if (!rval) rval = CompareRoomNames(room1, room2);  // sort on name if descrs identical
	return rval;
}

int __cdecl NumberCompare(const void *r1, const void *r2) {
	int rval;
	CRoom *room1 = (* ((CRoom **) r1));
	CRoom *room2 = (* ((CRoom **) r2));
	// got to be column 1
	ASSERT(sortColumn == 1);
	if (room1->m_nUsers < room2->m_nUsers) rval = -1;
	else if (room1->m_nUsers > room2->m_nUsers) rval = 1;
	else rval = 0;
	if (!sortAscending) rval = -rval;
	if (!rval) rval = stricmp(room1->m_prettyName, room2->m_prettyName);   // sort on room name if numbers equal
	return rval;
}


void CRoomList::SortRooms(BOOL bResetList) {
//	m_roomList.DeleteAllItems();
	m_persist->SortRooms();
	LoadRooms(bResetList);
}

void CRoomListPersist::SortRooms() {
	sortColumn = m_sortColumn;
	sortAscending = m_sortAscending;
	TRACE("Sort ascending = %d, col = %d.\n", m_sortColumn, m_sortAscending);

	switch (m_sortColumn) {
	  case 0:
	  case 2:
		qsort(m_rooms, m_nRooms, sizeof(CRoom *), StringCompare);
		break;
	  case 1:
		qsort(m_rooms, m_nRooms, sizeof(CRoom *), NumberCompare);
		break;
	}
}

void CRoomList::OnDblclkRoomlist(NMHDR* pNMHDR, LRESULT* pResult) 
{
	OnGoto();

	*pResult = 0;
}

CRoom *CRoomList::GetSelectedRoom() {
	// presumably we could get selection info from pnMHDR, but don't know how, so do this instead...
	if (m_roomList.GetSelectedCount() != 1) return NULL;
	int index = m_roomList.GetNextItem(-1, LVNI_FOCUSED);
	int roomNum = m_roomList.GetItemData(index);
	return ((CRoom *) m_persist->m_rooms[roomNum]);
}

void CRoomList::OnGoto() 
{
	CRoom *room = GetSelectedRoom();
	if (!room) return;
	EndDialog(0);
	g_bEnterOnCreate = FALSE;
	bSwitchToRoom(room->m_name);
}


void CRoomList::OnListmembers() 
{
	CRoom *pRoom = GetSelectedRoom();
	if (!pRoom)
		return;

	// Don't want to click this button while an earlier request is being executed
	GetDlgItem(IDC_LISTMEMBERS)->EnableWindow(FALSE);

	ListMembers(pRoom->m_name, pRoom->m_prettyName);
}


void CRoomList::OnCreateRoom() 
{
	EndDialog(0);
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_ROOM_CREATEROOM);
}


void CRoomList::OnCloseDialog()
{
	EndDialog(0);
}


/////////////////////////////////////////////////////////////////////////////
// CRoomListCtl

CRoomListCtrl::CRoomListCtrl()
{
	int i = 0;
}

CRoomListCtrl::~CRoomListCtrl()
{
}

CRoomListPersist::~CRoomListPersist() {
	MakeEmpty();
}

void CRoomListPersist::MakeEmpty() {
	if (!m_rooms) return;

	for (int i = 0; i < m_nRooms; i++)
		delete m_rooms[i];

	free(m_rooms);
	m_rooms = NULL;
	m_nRooms = m_roomsSize = 0;
}


BEGIN_MESSAGE_MAP(CRoomListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CRoomListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick)
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomListCtl message handlers

void CRoomListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	// TODO: Add your control notification handler code here
	LV_ITEM *lv = &pDispInfo->item;
	CRoomList *rl = (CRoomList *) GetParent();
	CRoom *room = (CRoom *) rl->m_persist->m_rooms[lv->lParam];
//	TRACE("Doing callback on %s (lParam = %d).\n", room->m_name, lv->lParam);
	if (lv->iSubItem == 0) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, room->m_prettyName, maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	} else if (lv->iSubItem == 1) {
		sprintf(lv->pszText, "%d", room->m_nUsers);
	} else if (lv->iSubItem == 2) {
		int maxChar = lv->cchTextMax - 1;
		strncpy(lv->pszText, room->m_descr, maxChar);
		lv->pszText[maxChar] = '\0';  // insurance
	}

	*pResult = 0;
}

void CRoomListCtrl::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;
	CRoomList *rl = (CRoomList*) GetParent();
	ASSERT(rl);
	CRoomListPersist *persist = rl->m_persist;

	if (persist->m_sortColumn == pNMListView->iSubItem) // toggle sortOrder
		persist->m_sortAscending = !persist->m_sortAscending;
	else {												  // change sortColumn
		persist->m_sortColumn = pNMListView->iSubItem;
		persist->m_sortAscending = TRUE;				  // sort ascendingly first
	}
	rl->SortRooms(TRUE);
	*pResult = 0;
}

void 
CRoomListCtrl::OnSetFocus(
CWnd* pOldWnd)
{
	// Set focus item if there isn't any.
	int iIndex = GetNextItem(-1, LVNI_FOCUSED);
	if (iIndex == -1)
		SetItemState(0, LVIS_FOCUSED, LVIS_FOCUSED);
	CListCtrl::OnSetFocus (pOldWnd);
}

#if 0
void CRoomList::OnSortRooms() {
	SortRooms(TRUE);
}
#endif

void CRoomList::FilterByTopic(CString &topic) {
	m_strTopic = topic;			  // hidden param to AddToRoomList
	LoadRooms(TRUE);
}

const char *stristr(const char *str1, const char *str2) {
	// inefficient, but simple to implement.  Should do in-place.
	const char *rval;

	char *low1 = strdup(str1);
	CharLowerBuff(low1, strlen(low1));
	char *low2 = strdup(str2);
	CharLowerBuff(low2, strlen(low2));
	char *match = (char *) _mbsstr((const UCHAR *) low1, (const UCHAR *)low2);
	if (!match) rval = NULL;
	else rval = match - low1 + str1;
	free(low1);
	free(low2);
	return rval;
}

BOOL CRoomList::MatchesTopicFilter(CRoom *r, const char *searchTopic, BOOL bSearchDescrs) {
	if (r->m_nUsers > m_maxMembers || r->m_nUsers < m_minMembers) return FALSE;
	if (m_bRegisteredOnly && !r->m_byteRegistered) return FALSE;
	if (m_strTopic.GetLength() == 0) return TRUE;
	if (stristr(r->m_prettyName, searchTopic)) return TRUE;
	if (bSearchDescrs && stristr(r->m_descr, searchTopic)) return TRUE;
	return FALSE;
}

int CRoomList::DoModal() {
	int rval = CCSDialog::DoModal();
	m_persist->m_strTopicFilter = m_strTopic;
	m_persist->m_bSearchDescrs = m_bSearchDescrs;
	m_persist->m_bRegisteredOnly = m_bRegisteredOnly;
	m_persist->m_minMembers = m_minMembers;
	m_persist->m_maxMembers = m_maxMembers;
	return rval;
}


void CRoomList::OnSearchDescrs() 
{
	m_bSearchDescrs = m_ctrlSearchDescrs.GetCheck();
	if (m_strTopic != "") FilterByTopic(m_strTopic);  // don't refilter if topic empty
}

void CRoomList::AnnounceCount() {
	CString label;
	CString count;
	count.Format("%d", m_roomList.GetItemCount());
	label.LoadString(IDS_NUM_ROOMS);
	VERIFY(ReplaceToken(label, CString("%1"), count));
	m_ctlCaption.SetWindowText(label);
}

void CRoomList::OnChangeTopicEdit() 
{
	CString str;
	m_topicEdit.GetWindowText(str);
	FilterByTopic(str);
}

void CRoomListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;
	ASSERT(GetParent());
	BOOL bRoomSelected = (GetSelectedCount() == 1);
	CWnd *parent = GetParent();
	parent->GetDlgItem(IDC_GOTO)->EnableWindow(bRoomSelected);
	parent->GetDlgItem(IDC_LISTMEMBERS)->EnableWindow(bRoomSelected);
	
	*pResult = 0;
}

const DWORD CRoomList::m_nHelpIDs[] =
{
	IDC_TOPIC_EDIT,					IDH_SEARCH,
	IDC_SEARCH_DESCRS,				IDH_SEARCH_TOPICS,
	IDC_RESET_LIST,					IDH_UPDATE,
	IDC_GOTO,						IDH_GOTO,
	IDC_NEWROOM,					IDH_NEW_ROOM,
	IDC_REGISTERED_ONLY,			IDH_ROOMLIST_SHOW_ONLY_REG,
	IDC_MIN_MEMBERS,				IDH_ROOMLIST_MEMBERS_MIN,
	IDC_MAX_MEMBERS,				IDH_ROOMLIST_MEMBERS_MAX,
	0, 0
};


void CRoomList::OnChangeMinMembers() 
{
	if (!m_topicEdit.m_hWnd) return; // numeric edits get EN_CHANGE before CRoomList::InitDialog!

	// Make max >= min
	CString strMin;
	CEdit *minCtl = (CEdit *)GetDlgItem(IDC_MIN_MEMBERS);
	minCtl->GetWindowText(strMin);
	m_minMembers = atoi(strMin);
	if (m_minMembers > m_maxMembers) {
		CEdit *maxCtl = (CEdit *)GetDlgItem(IDC_MAX_MEMBERS);
		maxCtl->SetWindowText(strMin);
	}

	OnChangeTopicEdit();	// Misnomer -- this handles all filtering
}

void CRoomList::OnChangeMaxMembers() 
{
	if (!m_topicEdit.m_hWnd) return;  // numeric edits get EN_CHANGE before CRoomList::InitDialog!

	// Make min <= max
	CString strMax;
	CEdit *maxCtl = (CEdit *)GetDlgItem(IDC_MAX_MEMBERS);
	maxCtl->GetWindowText(strMax);
	m_maxMembers = atoi(strMax);
	if (m_minMembers > m_maxMembers) {
		CEdit *minCtl = (CEdit *)GetDlgItem(IDC_MIN_MEMBERS);
		minCtl->SetWindowText(strMax);
	}

	OnChangeTopicEdit();	// Misnomer -- this handles all filtering
}

void CRoomList::OnRegisteredOnly() 
{
	m_bRegisteredOnly = !m_bRegisteredOnly;
	theApp.m_bListRegistered = m_bRegisteredOnly;

	OnChangeTopicEdit();	// Misnomer -- this handles all filtering
}

void CRoom::CalculateSortByte() {
	int rval = 0;
	const char *name = m_prettyName;
	if (*name == '#' || *name == '&') {
		rval = 2;    // group IRC rooms after IRCX
		name++;
	}

	// if 1st  char is an ordinary char (non-lead) and non-alpha, put it at the end of its grouping
	UCHAR u = (UCHAR) *name;
	if ((u < 128 && !(('A' <= u && u <= 'Z') || ('a' <= u && u <= 'z'))))
		rval += 1;

	m_byteSort = (BYTE) rval;
}

BOOL 
CRoomList::PreTranslateMessage(
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

