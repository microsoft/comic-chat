// RoomList.h : header file
//
#ifndef __ROOMLIST_H__
#define __ROOMLIST_H__

#include "setupdlg.h"

class CRoom {
public:
	CString m_name;
	CString m_prettyName;		// Decoded
	UINT m_nUsers;
	CString m_descr;
	BYTE m_byteRegistered;
	BYTE m_byteSort;

	void CalculateSortByte();
};

class CRoomListPersist {
public:
	CRoomListPersist() {m_rooms = NULL; m_bSearchDescrs = FALSE; m_nRooms = m_roomsSize = 0; m_sortAscending = TRUE; m_sortColumn = 0; }
	~CRoomListPersist();
	void MakeEmpty();
	void Reset() { MakeEmpty(); m_cachedServer = ""; m_strTopicFilter = ""; m_bSearchDescrs = FALSE; extern CChatApp theApp; m_bRegisteredOnly = theApp.m_bListRegistered; m_minMembers = 0; m_maxMembers = 9999; m_sortAscending = TRUE; m_sortColumn = 0; m_searchTime = "";}
	void SortRooms();
	int AddRoom(CRoom *room);

	CString m_cachedServer;		// server for which this room list is cached
	CRoom **m_rooms;
	int m_nRooms;
	int m_roomsSize;
	BOOL m_sortAscending;  // pertains to how sorted in CRoomList (not sort order here)
	BOOL m_sortColumn;
	CString m_strTopicFilter;
	BOOL m_bSearchDescrs;
	BOOL m_bRegisteredOnly;
	UINT m_maxMembers;
	UINT m_minMembers;
	CString m_searchTime;
	CString m_strQuery;
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
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CRoomList dialog

class CRoomList : public CCSDialog
{
// Construction
public:
	CRoomList(CRoomListPersist *, CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CRoomList)
	enum { IDD = IDD_ROOMLIST };
	CSpinButtonCtrl	m_spinMax;
	CSpinButtonCtrl	m_spinMin;
	CButton			m_reset;
	CStatic			m_ctlCaption;
	CButton			m_ctrlSearchDescrs;
	CFilterEdit		m_topicEdit;
	CRoomListCtrl	m_roomList;
	CString			m_strTopic;
	BOOL			m_bSearchDescrs;
	UINT			m_minMembers;
	UINT			m_maxMembers;
	BOOL			m_bRegisteredOnly;
	//}}AFX_DATA

	BOOL m_bResetHadFocus;		// set during OnResetList, with Reset button's focus state


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRoomList)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CRoomList)
	afx_msg void OnResetList();
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkRoomlist(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGoto();
	afx_msg void OnSearchDescrs();
	afx_msg void OnChangeTopicEdit();
	afx_msg void OnCreateRoom();
	afx_msg void OnChangeMinMembers();
	afx_msg void OnChangeMaxMembers();
	afx_msg void OnRegisteredOnly();
	afx_msg void OnListmembers();
	afx_msg void OnCloseDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	void ClearRoomList() { m_roomList.DeleteAllItems(); }
	void AddToRoomList(int iRoomIndex);
	void SortRooms(BOOL bResetList = TRUE);
	void LoadRooms(BOOL bResetList = TRUE);
	void FilterByTopic(CString &);
	BOOL MatchesTopicFilter(CRoom *, const char *, BOOL);
	virtual int DoModal();  // override stores certain values back in m_persist
	void AnnounceCount();
	void AnnounceTime() { GetDlgItem(IDC_SEARCH_TIME)->SetWindowText(m_persist->m_searchTime); }
	CRoom *GetSelectedRoom();

	CRoomListPersist *m_persist;
};


#endif // __ROOMLIST_H__
