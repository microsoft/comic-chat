// UserList.h : header file
//
#ifndef __USERLIST_H__
#define __USERLIST_H__

#include "userinfo.h"

class CUser
{
public:
	CUser()		{ m_szPrettyNick = NULL; m_nRefCount = 1; m_wFlags = 0; }
	~CUser()	{ ASSERT(!m_nRefCount); if (m_szPrettyNick) free(m_szPrettyNick); }

	WORD		GetFlags()				{ return m_wFlags; }
	void		SetFlags(WORD wFlags)	{ m_wFlags = wFlags; }
	const char*	GetPrettyNick()			{ if (m_szPrettyNick) return m_szPrettyNick; else return m_strNickname; }
	void		AddRef()				{ m_nRefCount++; }
	void		Release();
	BOOL		operator==(const CUser& user);

	CString m_strNickname;
	CString m_strIdentity;
	CString m_strFullName;
	CString m_strRoom;
	CString m_strPrettyRoom;
	char*	m_szPrettyNick;
	short	m_nRefCount;
	WORD	m_wFlags;
};


#define USERSEARCH_ALL	0
#define USERSEARCH_NICK	1
#define USERSEARCH_ID	2
#define USERSEARCH_ROOM	3

class CUserListPersist {
public:
	CUserListPersist() {m_users = NULL; m_searchType = USERSEARCH_NICK; m_nUsers = m_usersSize = 0; m_sortAscending = TRUE; m_sortColumn = 0; }
	~CUserListPersist();
	void MakeEmpty();
	void Reset() { MakeEmpty(); m_cachedServer = ""; m_strUserFilter = ""; m_strRoomFilter = ""; m_strEncRoom = ""; m_searchType = USERSEARCH_NICK; m_sortAscending = TRUE; m_sortColumn = 0; m_searchTime = "";}
	void Sort();
	int AddUser(CUser *user);

	CString m_cachedServer;		// server for which this room list is cached
	CUser **m_users;
	int m_nUsers;
	int m_usersSize;
	BOOL m_sortAscending;  // pertains to how sorted in CUserList (not sort order here)
	BOOL m_sortColumn;
	CString m_strUserFilter;
	CString m_strRoomFilter;
	int m_searchType;
	CString m_searchTime;
	CString m_strQuery;
	CString m_strEncRoom;	// encoded room -- non empty indicates a member query from room list
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
	afx_msg void OnSetFocus(CWnd* pOldWnd);
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
	CEdit	m_ctlRoom;
	CButton	m_reset;
	CStatic	m_ctlCaption;
	CEdit	m_user;
	CUserListCtrl	m_userListCtrl;
	CString	m_strUser;
	CString	m_strRoom;
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
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL OnInitDialog();
	afx_msg void OnResetList();
	afx_msg void OnInviteFromList();
	afx_msg void OnItemchangedUserlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnUsersearchAll();
	afx_msg void OnUsersearchIdentity();
	afx_msg void OnUsersearchNick();
	afx_msg void OnMessageFromList();
	afx_msg void OnJoinRoom();
	afx_msg void OnUsersearchRoom();
	afx_msg void OnChangeRoomEdit();
	afx_msg void OnCloseDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	void AddToUserList(int index);
	void AnnounceCount();
	void AnnounceTime() { GetDlgItem(IDC_SEARCH_TIME)->SetWindowText(m_persist->m_searchTime);}
	void Sort(BOOL bResetList = TRUE);
	void Load(BOOL bResetList = TRUE);
	void ShowAndEnableControl(UINT nID, BOOL bShowAndEnable);
	CUserListPersist *m_persist;
};

/////////////////////////////////////////////////////////////////////////////

#define LAUNCH_WHISPERBOX	8181		// return value for CUserListDlg.DoModal, when we're launching whisperbox

#endif // __USERLIST_H__
