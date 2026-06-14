// TextView.h : header file
//

/*
We are going to probably use the subclassed rich edit control instead of this view
but we will keep it here for awhile until we decide for sure.
... Probably we will keep it...
*/

/////////////////////////////////////////////////////////////////////////////
// CTextEdit window

class CTextEdit : public CRichEditCtrl
{
// Construction
public:
	CTextEdit();
	DECLARE_DYNCREATE(CTextEdit)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CTextEdit();

	// Generated message map functions
protected:
	//{{AFX_MSG(CTextEdit)
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CTextView view

class CTextView : public CView
{
protected:
	CTextView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTextView)

// Attributes
public:
	CTextEdit* m_pRichEdit;
	BOOL	m_bFirstTime;
	CFont*  m_pFooterFont;
	int		m_lineHeight;

// Operations
public:
	void PopulateLine(CUserInfo *pui, const char *line, UCHAR mode);
	void TextLine(CUserInfo *pui, const char *line, UCHAR mode, BOOL cooked = FALSE, CPtrArray *links = NULL);
	void NewParagraph();
	void ShowInfo(CUserInfo *pui, const char *info);
	void ShowText(const char *text, BOOL newPgraph = TRUE, CPtrArray *links = NULL, int scrollToEnd = -1);
	void ClearTextView();
	BOOL ScrolledToEnd();
	void RegisterTextLinks(CPtrArray *m_links, int cch);
	BOOL HandleLink(void *penlink_v);
	long PrintPage(CDC* pDC, long nIndexStart, long nIndexStop);
	void PrintFooter(CDC *pDC, CPrintInfo *pInfo);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTextView)
	public:
	virtual void OnInitialUpdate();
	protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTextView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTextView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	afx_msg LRESULT OnLoginDlg(WPARAM wParam, LPARAM lParam);
	afx_msg void HandleLink (NMHDR * pNotifyStruct, LRESULT * result );
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
