// RoomList.h : header file
//

class CRoom {
public:
	CString m_name;
	int m_nUsers;
	CString m_descr;
};

class CRoomListPersist {
public:
	CRoomListPersist() {m_rooms = NULL; m_bSearchDescrs = FALSE; m_nRooms = m_roomsSize = 0; m_sortAscending = TRUE; m_sortColumn = 0; }
	~CRoomListPersist();
	void MakeEmpty();
	void Reset() { MakeEmpty(); m_cachedServer = ""; m_strUserFilter = ""; m_strTopicFilter = ""; m_bSearchDescrs = FALSE; m_sortAscending = TRUE; m_sortColumn = 0; }
	void SortRooms();
	int AddRoom(CRoom *room);

	CString m_cachedServer;		// server for which this room list is cached
	CRoom **m_rooms;
	int m_nRooms;
	int m_roomsSize;
	BOOL m_sortAscending;  // pertains to how sorted in CRoomList (not sort order here)
	BOOL m_sortColumn;
	CString m_strUserFilter;
	CString m_strTopicFilter;
	BOOL m_bSearchDescrs;
};

/////////////////////////////////////////////////////////////////////////////
// CRoomListCtrl window

class CRoomListCtrl : public CListCtrl
{
// Construction
public:
	CRoomListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CRoomListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CRoomListCtrl)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CRoomList dialog

class CRoomList : public CDialog
{
// Construction
public:
	CRoomList(CRoomListPersist *, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRoomList)
	enum { IDD = IDD_ROOMLIST };
	CButton	m_reset;
	CStatic	m_ctlCaption;
	CButton	m_ctrlSearchDescrs;
	CNicknameEdit	m_topicEdit;
	CNicknameEdit	m_user;
	CRoomListCtrl	m_roomList;
	CString	m_strTopic;
	BOOL	m_bSearchDescrs;
	CString	m_strUser;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CRoomList)
	afx_msg void OnResetList();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkRoomlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGoto();
	afx_msg void OnSortRooms();
	afx_msg void OnSearchDescrs();
	afx_msg void OnChangeTopicEdit();
	afx_msg void OnNewroom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void ClearRoomList() { m_roomList.DeleteAllItems(); }
	void AddToRoomList(int roomIndex);
	void SortRooms(BOOL bResetList = TRUE);
	void LoadRooms(BOOL bResetList = TRUE);
	void FilterByTopic(CString &);
	BOOL MatchesTopicFilter(CRoom *, const char *, BOOL);
	virtual int DoModal();  // override stores certain values back in m_persist
	void AnnounceCount();
	void SwitchToRoom(const char *room);

	CRoomListPersist *m_persist;
};


