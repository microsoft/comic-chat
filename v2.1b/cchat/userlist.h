// UserList.h : header file
//

class CUser {
public:
	CUser () { m_prettyNick = NULL; }
	~CUser () { if (m_prettyNick) free(m_prettyNick); }
	const char *GetPrettyNick() { if (m_prettyNick) return m_prettyNick; else return m_nick; }

	CString m_nick;
	CString m_ident;
	CString m_fullName;
	CString m_room;
	CString m_prettyRoom;
	char *m_prettyNick;
};

#define USERSEARCH_NICK	1
#define USERSEARCH_ID	2
#define USERSEARCH_ALL	0

class CUserListPersist {
public:
	CUserListPersist() {m_users = NULL; m_searchType = USERSEARCH_ALL;/* m_bSearchDescrs = FALSE;*/ m_nUsers = m_usersSize = 0; m_sortAscending = TRUE; m_sortColumn = 0; }
	~CUserListPersist();
	void MakeEmpty();
	void Reset() { MakeEmpty(); m_cachedServer = ""; m_strUserFilter = ""; /*m_bSearchDescrs = FALSE;*/m_searchType = USERSEARCH_ALL; m_sortAscending = TRUE; m_sortColumn = 0; m_searchTime = "";}
	void Sort();
	int AddUser(CUser *user);

	CString m_cachedServer;		// server for which this room list is cached
	CUser **m_users;
	int m_nUsers;
	int m_usersSize;
	BOOL m_sortAscending;  // pertains to how sorted in CUserList (not sort order here)
	BOOL m_sortColumn;
	CString m_strUserFilter;
//	CString m_strTopicFilter;
//	BOOL m_bSearchDescrs;
	int m_searchType;
	CString m_searchTime;
};


/////////////////////////////////////////////////////////////////////////////
// CUserListCtrl window

class CUserListCtrl : public CListCtrl
{
// Construction
public:
	CUserListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CUserListCtrl();
	CUser *CUserListCtrl::GetSelectedUser();
	const char *GetSelectedNickname();

	// Generated message map functions
protected:
	//{{AFX_MSG(CUserListCtrl)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CUserList dialog

class CUserList : public CCSDialog
{
// Construction
public:
	CUserList(CUserListPersist *, CWnd* pParent = NULL);
	~CUserList();

// Dialog Data
	//{{AFX_DATA(CUserList)
	enum { IDD = IDD_USERLIST };
	CButton	m_reset;
	CStatic	m_ctlCaption;
	CEdit	m_user;
	CUserListCtrl	m_userListCtrl;
	CString	m_strUser;
	//}}AFX_DATA

	CUserInfo *m_selUser;
	BOOL m_bResetHadFocus;		// set during OnResetList, with Reset button's focus state


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUserList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CUserList)
	virtual BOOL OnInitDialog();
	afx_msg void OnResetList();
	afx_msg void OnInviteFromList();
	afx_msg void OnItemchangedUserlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUsersearchAll();
	afx_msg void OnUsersearchIdentity();
	afx_msg void OnUsersearchNick();
	afx_msg void OnMessageFromList();
	afx_msg void OnJoinRoom();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void AddToUserList(int index);
	void AnnounceCount();
	void AnnounceTime() { GetDlgItem(IDC_SEARCH_TIME)->SetWindowText(m_persist->m_searchTime);}
	void Sort(BOOL bResetList = TRUE);
	void Load(BOOL bResetList = TRUE);
	CUserListPersist *m_persist;
};

/////////////////////////////////////////////////////////////////////////////

#define LAUNCH_WHISPERBOX	8181		// return value for CUserListDlg.DoModal, when we're launching whisperbox
