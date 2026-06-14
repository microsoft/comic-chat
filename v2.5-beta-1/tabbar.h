#ifndef __TABBAR_H__
#define __TABBAR_H__

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CTabBar window

// Have to subclass this because the default WM_GETDLGCODE handler for
// this control does not let the Tab key get through.
class CTabBarTabCtrl : public CTabCtrl
{
protected:
    afx_msg UINT OnGetDlgCode();
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	DECLARE_MESSAGE_MAP()
};

class CTabBar : public CControlBar
{
// Construction
public:
	CTabBar();

// Attributes
public:
	CTabBarTabCtrl	m_tabCtrl;
	CImageList		m_images;

	BOOL			m_bDeleteFont;
	CFont			m_font;
	LONG			m_lLargestTab;

// Operations
public:
	virtual void OnUpdateCmdUI( CFrameWnd* pTarget, BOOL bDisableIfNoHndler ) {};
	// virtual CSize CalcFixedLayout( BOOL bStretch, BOOL bHorz );
	virtual CSize CalcDynamicLayout( int nLength, DWORD dwMode );
	void AddMDITab(const char *szChanName, CChatDoc *pDoc, BOOL bSelectIt = TRUE);
	void DelMDITab(int i);
	void GetTabString(int i, CString &name);
	int FindTabNum(CChatDoc *);
	CChatDoc *GetTabDoc(int index);
	void SetTabIcon(int tabNum, int iIcon);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTabBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTabBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTabBar)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnKeyDown(NMHDR* pNMHDR, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};

extern void SetActiveTab(CChatDoc *pDoc);

#endif // __TABBAR_H__
