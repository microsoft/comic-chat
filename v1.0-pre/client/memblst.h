// MemberList.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMemberList frame

class CMemberList : public CFrameWnd
{
	DECLARE_DYNCREATE(CMemberList)
protected:
	CMemberList();           // protected constructor used by dynamic creation

// Attributes
public:
	CListCtrl m_MemberListBox;
	CImageList m_ImageList;

// Operations
public:
	int GetSortPosition(const char *name);

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
	//}}AFX_MSG
	afx_msg void OnItemchangedList1(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLVKillFocus( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnDblClick(NMHDR* pNotifyStruct, LRESULT* result);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()
	int GetCurrentSelection();

};

/////////////////////////////////////////////////////////////////////////////
