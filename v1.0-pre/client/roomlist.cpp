// RoomList.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "setupdlg.h"
#include "RoomList.h"
#include "chatprot.h"


extern CChatApp theApp;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRoomList dialog


CRoomList::CRoomList(CRoomListPersist *persist, CWnd* pParent /*=NULL*/)
	: CDialog(CRoomList::IDD, pParent)
{
	//{{AFX_DATA_INIT(CRoomList)
	m_strTopic = _T("");
	m_bSearchDescrs = FALSE;
	m_strUser = _T("");
	//}}AFX_DATA_INIT

	m_persist = persist;
	m_strTopic = persist->m_strTopicFilter;
	m_strUser = persist->m_strUserFilter;
	m_bSearchDescrs = m_persist->m_bSearchDescrs;
}


void CRoomList::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRoomList)
	DDX_Control(pDX, IDC_RESET_LIST, m_reset);
	DDX_Control(pDX, IDC_ROOM_CAPTION, m_ctlCaption);
	DDX_Control(pDX, IDC_SEARCH_DESCRS, m_ctrlSearchDescrs);
	DDX_Control(pDX, IDC_TOPIC_EDIT, m_topicEdit);
	DDX_Control(pDX, IDC_USER_KEY, m_user);
	DDX_Control(pDX, IDC_ROOMLIST, m_roomList);
	DDX_Text(pDX, IDC_TOPIC_EDIT, m_strTopic);
	DDX_Check(pDX, IDC_SEARCH_DESCRS, m_bSearchDescrs);
	DDX_Text(pDX, IDC_USER_KEY, m_strUser);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRoomList, CDialog)
	//{{AFX_MSG_MAP(CRoomList)
	ON_BN_CLICKED(IDC_RESET_LIST, OnResetList)
	ON_NOTIFY(NM_DBLCLK, IDC_ROOMLIST, OnDblclkRoomlist)
	ON_BN_CLICKED(IDC_GOTO, OnGoto)
	ON_BN_CLICKED(IDC_SORT_ROOMS, OnSortRooms)
	ON_BN_CLICKED(IDC_SEARCH_DESCRS, OnSearchDescrs)
	ON_EN_CHANGE(IDC_TOPIC_EDIT, OnChangeTopicEdit)
	ON_BN_CLICKED(IDC_NEWROOM, OnNewroom)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRoomList message handlers

void CRoomList::OnResetList() 
{
	void ChatFillRoomList(CRoomList *);
	m_roomList.DeleteAllItems();
	m_persist->MakeEmpty();
	ChatFillRoomList(this);
}

BOOL CRoomList::OnInitDialog() 
{
	CDialog::OnInitDialog();
	CString labelStr;
	CString widthStr;

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

	extern const char *GetMyServer();
	if (m_persist->m_cachedServer != GetMyServer()) {  // if not cached for this server...
		m_persist->Reset(); // clear out everything, including filters...
		OnResetList();      // do an initial reset
		m_persist->m_cachedServer = GetMyServer();
	} else LoadRooms(FALSE);

	GetDlgItem(IDC_GOTO)->EnableWindow(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CRoomList::LoadRooms(BOOL bResetList) {
	if (bResetList) m_roomList.DeleteAllItems();
	for (int i = 0; i < m_persist->m_nRooms; i++) {
		AddToRoomList(i);
	}
	AnnounceCount();
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

void CRoomList::AddToRoomList(int roomIndex) {
	if (!MatchesTopicFilter(m_persist->m_rooms[roomIndex], m_strTopic, m_bSearchDescrs))
		return;			// only add to list if meets topic filter

	LV_ITEM lv;
	lv.iItem = m_roomList.GetItemCount();  // add to end
	lv.iSubItem = 0;
	lv.mask = LVIF_TEXT | LVIF_PARAM;
	lv.pszText = /* UnConst((const char *) room->m_name) */ LPSTR_TEXTCALLBACK; // should work, but doesn't allow sort!!!
//	lv.state = lv.stateMask = 0;
	lv.lParam = roomIndex;
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

int StringCompare(const void *r1, const void *r2) {
	int rval;
	CRoom *room1 = (* ((CRoom **) r1));
	CRoom *room2 = (* ((CRoom **) r2));
	if (sortColumn == 0) {
		rval = stricmp(room1->m_name, room2->m_name);
//		TRACE("%s vs %s = %d\n", room1->m_name, room2->m_name, rval);
	} else if (sortColumn == 2) {
		rval = stricmp(room1->m_descr, room2->m_descr);
	}
	if (!sortAscending) rval = -rval;
	if (!rval) rval = stricmp(room1->m_name, room2->m_name);  // sort on name if descrs identical
	return rval;
}

int NumberCompare(const void *r1, const void *r2) {
	int rval;
	CRoom *room1 = (* ((CRoom **) r1));
	CRoom *room2 = (* ((CRoom **) r2));
	// got to be column 1
	ASSERT(sortColumn == 1);
	if (room1->m_nUsers < room2->m_nUsers) rval = -1;
	else if (room1->m_nUsers > room2->m_nUsers) rval = 1;
	else rval = 0;
	if (!sortAscending) rval = -rval;
	if (!rval) rval = stricmp(room1->m_name, room2->m_name);		// sort on room name if numbers equal
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

void CRoomList::OnGoto() 
{
	// presumably we could get selection info from pnMHDR, but don't know how, so do this instead...
	if (m_roomList.GetSelectedCount() != 1) return;
	int index = m_roomList.GetNextItem(-1, LVNI_FOCUSED);
	CString newRoom = m_roomList.GetItemText(index,0);
	EndDialog(0);
	SwitchToRoom(newRoom);
}

void CRoomList::OnNewroom() 
{
//	CChannelDlg dlg;
//	if (dlg.DoModal() == IDOK)
//		SwitchToRoom(dlg.m_strChannel);
	EndDialog(0);
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_SESSION_NEWROOM);
}

void CRoomList::SwitchToRoom(const char *newRoom) {
	extern void ChatSetChannel(const char *);
	ChatSetChannel(newRoom);
	extern BOOL bCXPrompt;
	extern BOOL bCXKeepServer;
	bCXPrompt = FALSE;
	bCXKeepServer = TRUE;
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
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
		strncpy(lv->pszText, room->m_name, maxChar);
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
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	CRoomList *rl = (CRoomList *) GetParent();
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


void CRoomList::OnSortRooms() {
	SortRooms(TRUE);
}

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
	char *match = strstr(low1, low2);
	if (!match) rval = NULL;
	else rval = match - low1 + str1;
	free(low1);
	free(low2);
	return rval;
}

BOOL CRoomList::MatchesTopicFilter(CRoom *r, const char *searchTopic, BOOL bSearchDescrs) {
	if (m_strTopic.GetLength() == 0) return TRUE;
	if (stristr(r->m_name, searchTopic)) return TRUE;
	if (bSearchDescrs && stristr(r->m_descr, searchTopic)) return TRUE;
	return FALSE;
}

int CRoomList::DoModal() {
	int rval = CDialog::DoModal();
	m_persist->m_strUserFilter = m_strUser;
	m_persist->m_strTopicFilter = m_strTopic;
	m_persist->m_bSearchDescrs = m_bSearchDescrs;
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
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	GetParent()->GetDlgItem(IDC_GOTO)->EnableWindow(TRUE);
	
	*pResult = 0;
}
