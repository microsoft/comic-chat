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
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
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
	CFont	*m_fontText;
	CTextCore m_textCore;

// Operations
public:
	void TextLine(CUserInfo *pui, const char *szLine, BYTE byteMode, BYTE bbCooked = 0, CDWordArray *prgdwFormatting = NULL);
	void ShowInfo(CUserInfo *pui, const char *szInfo);
	void ClearTextView();
	void DisplayPart(const char *);
	void DisplayJoin(const char *);
	void DisplayNickChange(CUserInfo *pui, const char *szOldNick);
	BOOL HandleLink(void *penlink_v);
	long lPrintPage(CDC* pDC, UINT nCurPage, BOOL bDisplay);
	void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
	// void SetFormat(CHARFORMAT *cf);  // REGISB not used
	void SetURLBrowser(BOOL bNewBrowser);
	CChatDoc* GetDocument();

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

//#ifndef _DEBUG  // debug version in chatView.cpp
inline CChatDoc* CTextView::GetDocument()
   { return (CChatDoc*)m_pDocument; }
//#endif

/////////////////////////////////////////////////////////////////////////////

extern CTextView *GetTextView();