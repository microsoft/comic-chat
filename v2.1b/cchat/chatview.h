// chatView.h : interface of the CChatView class
//
/////////////////////////////////////////////////////////////////////////////
class CFixedSplitter : public CSplitSay
{
//	DECLARE_DYNCREATE(CFixedSplitter)
public:
	CFixedSplitter();           // protected constructor used by dynamic creation

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFixedSplitter)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CFixedSplitter();

	// Generated message map functions
	//{{AFX_MSG(CFixedSplitter)
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

class CChatView : public CView
{
protected: // create from serialization only
	CChatView();
	DECLARE_DYNCREATE(CChatView)

// Attributes
public:
	CChatDoc* GetDocument();
	float m_hpleft;
	CSplitChatV *m_wndSplitter;
	CFixedSplitter *m_wndLSplitter;
	CSplitChat *m_wndRSplitter;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatView)
	public:
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnDraw(CDC* pDC) {}
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChatView();
	void CreateComicView(BOOL doUpdate = TRUE);
	void CreateTextView(BOOL doUpdate = TRUE);
	void GetContext(CCreateContext* pContext);
	void CleanUpBeforeChangeView();
	CView *GetPrimaryView() { return (GetDocument()->GetPrimaryView()); }
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:
	//{{AFX_MSG(CChatView)
	afx_msg void OnCancelEditSrvr();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnGetinfo();
	afx_msg void OnIgnore();
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	afx_msg LRESULT OnLoginDlg(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in chatView.cpp
inline CChatDoc* CChatView::GetDocument()
   { return (CChatDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
