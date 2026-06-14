// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

// PageView.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPageView view

class CPageView : public CScrollView
{
protected:
	CPageView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CPageView)

// Attributes
protected:
	CRect				m_bbox;							// in client coords
	CFont				*m_footerFont;					// only allocated during printing
	CToolTipCtrl		m_toolTip;						// for avatar tooltips
	char				m_toolTipString[80];			// permanent string to hold avatar tooltips
	float				m_dpiConvx;						// for screen, physical dpi * m_dpiConv = logical dpi
	float				m_dpiConvy;						// NT allows for different x and y dpi's.
public:
	HBITMAP m_retSec;		// dib section for retained dib (for display)
	CDIB *m_retDib;			// retained dib (for display)
	HBITMAP m_printRetSec;	// dib section for retained dib (for printing)
	CDIB *m_printRetDib;	// retained dib (for printing)
	POINT m_printOffset;
	CScript m_script;		// dynamic script that this view will follow
	CPalette *m_palette;

	CChatDoc* GetDocument();
	BOOL bFirstTime;		// State variable to determine if this is the first time the view has been activated

// Operations
public:
	void PrintFooter(CDC *pDC, CPrintInfo *pInfo);
	HBITMAP GetRetSec(CDC *dc) { return (dc->IsPrinting() ? m_printRetSec : m_retSec); }
	CPalette *GetPalette(CDC *) { if ( m_palette) return m_palette; else return (InstallPalette()); }  // for now, always return same palette
	CPalette *InstallPalette();
	virtual int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;
	unsigned int FindAvatarUnderPoint(POINT);
	void *FindLabelUnderPoint(POINT point, POINT &panelPt);
	virtual void SetScrollSizes(int nMapMode, SIZE sizeTotal, const SIZE& sizePage, const SIZE& sizeLine);
	virtual void ScrollToPosition(CPoint pt);
	// routines to aid in 32 bit scrolling
	void InitializeDPI(CDC *dc);
	void LPtoDP(LPPOINT p) { p->x = (int) (p->x / m_dpiConvx); p->y = (int) (p->y / -m_dpiConvy); }
	void LPtoDP(RECT *r) { r->left = (int) (r->left / m_dpiConvx); r->right = (int) (r->right / m_dpiConvx); r->top = (int) (r->top / -m_dpiConvy); r->bottom = (int) (r->bottom / -m_dpiConvy); }
	void DPtoLP(LPPOINT p) { p->x = (int) (p->x * m_dpiConvx); p->y = (int) (p->y * -m_dpiConvy); } 
	void DPtoLP(RECT *r) { r->left = (int) (r->left * m_dpiConvx); r->right = (int) (r->right * m_dpiConvx); r->top = (int) (r->top * -m_dpiConvy); r->bottom = (int) (r->bottom * -m_dpiConvy); }
	void AddScrollOffset(RECT *, BOOL mapBack, BOOL logCoords);
	void AddScrollOffset(POINT *, BOOL mapBack, BOOL logCoords);
	void UpdateScroll(RECT *logPanel = NULL, BOOL onlyAtBottom = TRUE);
	void AddConversionDelta(RECT &r) { r.left -= (int) m_dpiConvx; r.right += (int) m_dpiConvx; r.bottom -= (int) m_dpiConvy; r.top += (int) m_dpiConvy; }
	void AddPrintOffset(RECT *rect, BOOL mapBack);
	void AddPrintOffset(POINT *, BOOL mapBack);
	int GetProspectivePanelWidth(int nWide);
	int GetProspectivePanelHeight(int nHigh);
	int FitPanelsWide();			// largest column count whose panels stay a comfortable size
	void SetPanelsWide(int nWide);
	void SetPanelsHigh(int nHigh);
	void FreeRetainedPanelS() { if (m_retDib) delete m_retDib; if (m_retSec) ::DeleteObject(m_retSec); m_retDib = NULL; m_retSec = NULL; }
	void FreeRetainedPanelP() { if (m_printRetDib) delete m_printRetDib; if (m_printRetSec) ::DeleteObject(m_printRetSec); m_printRetDib = NULL; m_printRetSec = NULL; }
	void ResetExistingPanels();
	BOOL AtBottom();
	BOOL m_bAutoFitting;			// re-entrancy guard for the panels-per-row auto-fit
	void VerifyScrollPosition(CPoint& pt);


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPageView)
	public:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	virtual void OnInitialUpdate();     // first time after construct
	virtual void OnUpdate( CView* pSender, LPARAM lHint, CObject* pHint );
	virtual void OnPrepareDC(CDC* pDC, CPrintInfo* pInfo = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView);
	public:  // printing functions called by CChatView's printing functions
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CPageView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
	//{{AFX_MSG(CPageView)
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnAvatarcontextTalkto();
	afx_msg void OnAvatarcontextTalktoalso();
	afx_msg void OnCancelEditSrvr();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnDestroy();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	//}}AFX_MSG
	afx_msg LRESULT OnLoginDlg(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnAutoFitPanels(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnMemberGetinfo();

};

#ifndef _DEBUG  // debug version in clienvw.cpp
inline CChatDoc* CPageView::GetDocument()
   { return (CChatDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
