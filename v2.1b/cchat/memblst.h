// MemberList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CMemberListCtrl window

class CMemberListCtrl : public CListCtrl
{
// Construction
public:
	CMemberListCtrl();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemberListCtrl)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMemberListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CMemberListCtrl)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CMemberList frame

class CMemberList : public CFrameWnd
{
	DECLARE_DYNCREATE(CMemberList)
protected:
	CMemberList();           // protected constructor used by dynamic creation

// Attributes
public:
	CMemberListCtrl m_MemberListBox;
	void *m_pDoc;

// Operations
public:
	int GetSortPosition(CUserInfo *pui);
//	void CleanupImageList();
	void Sort();
	void MakeVisible(CUserInfo *);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMemberList)
	public:
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CMemberList();

	// Generated message map functions
	//{{AFX_MSG(CMemberList)
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	afx_msg void OnLVKillFocus( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnDblClick(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
	int GetCurrentSelection();

};

extern CMemberList *GetMembers();

