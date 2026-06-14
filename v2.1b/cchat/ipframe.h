// IpFrame.h : interface of the CInPlaceFrame class
//

class CInPlaceFrame : public CDocObjectIPFrameWnd
{
	DECLARE_DYNCREATE(CInPlaceFrame)
public:
	CInPlaceFrame();
	virtual ~CInPlaceFrame();

// Attributes
public:

// Operations
public:
	virtual void GetMessageString( UINT nID, CString& rMessage ) const;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInPlaceFrame)
	public:
	virtual BOOL OnCreateControlBars(CFrameWnd* pWndFrame, CFrameWnd* pWndDoc);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg BOOL OnQueryNewPalette();
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:
	void SetStatusString(CString& strStatus);

protected:
	CToolBar    m_wndToolBar;
	COleResizeBar   m_wndResizeBar;
	COleDropTarget m_dropTarget;
	CTabBar	m_wndTabBar;

// Generated message map functions
protected:
	//{{AFX_MSG(CInPlaceFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnHelpTopics();
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
