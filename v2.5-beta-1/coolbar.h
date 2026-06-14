////////////////////////////////////////////////////////////////
// CCoolBar 1997 Microsoft Systems Journal. 
// If this program works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 5.0 on Windows 95

//////////////////
// CCoolBar encapsulates IE 4.0 common coolbar for MFC.
//
class CCoolBar : public CControlBar {
protected:
	DECLARE_DYNAMIC(CCoolBar)

public:
	CCoolBar();
	virtual ~CCoolBar();

	BOOL Create(CWnd* pParentWnd, DWORD dwStyle,
		DWORD dwAfxBarStyle = CBRS_ALIGN_TOP,
		UINT nID = AFX_IDW_TOOLBAR);

	// Message wrappers
	BOOL GetBarInfo(LPREBARINFO lp)
		{ ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage(RB_GETBARINFO, 0, (LPARAM)lp); }
	BOOL SetBarInfo(LPREBARINFO lp)
		{ ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage(RB_SETBARINFO, 0, (LPARAM)lp); }
	BOOL GetBandInfo(int iBand, LPREBARBANDINFO lp)
		{ ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage(RB_GETBANDINFO, iBand, (LPARAM)lp); }
	BOOL SetBandInfo(int iBand, LPREBARBANDINFO lp)
		{ ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage(RB_SETBANDINFO, iBand, (LPARAM)lp); }
	BOOL InsertBand(int iWhere, LPREBARBANDINFO lp)
		{ ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage(RB_INSERTBAND, (WPARAM)iWhere, (LPARAM)lp); }
	BOOL DeleteBand(int nWhich)
		{ ASSERT(::IsWindow(m_hWnd));
		  return (BOOL)SendMessage(RB_DELETEBAND, (WPARAM)nWhich); }
	int GetBandCount()
		{ ASSERT(::IsWindow(m_hWnd));
		  return (int)SendMessage(RB_GETBANDCOUNT); }
	int GetRowCount()
		{ ASSERT(::IsWindow(m_hWnd));
	     return (int)SendMessage(RB_GETROWCOUNT); }
	int GetRowHeight(int nWhich)
		{ ASSERT(::IsWindow(m_hWnd));
	     return (int)SendMessage(RB_GETROWHEIGHT, (WPARAM)nWhich); }

protected:
	// new virtual functions you must/can override
	virtual BOOL OnCreateBands() = 0; // return -1 if failed
	virtual void OnHeightChange(const CRect& rcNew);

	// CControlBar Overrides
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);

	// message handlers
	DECLARE_MESSAGE_MAP()
	afx_msg int  OnCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnPaint();
	afx_msg void OnHeigtChange(NMHDR* pNMHDR, LRESULT* pRes);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};

//////////////////
// Specialized CToolBar fixes display problems in MFC.
//
class CCoolToolBar : public CToolBar {
public:
	CCoolToolBar();
	virtual ~CCoolToolBar();
protected:
	DECLARE_DYNAMIC(CCoolToolBar)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	DECLARE_MESSAGE_MAP()
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT lpcs);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS*);
};

//////////////////
// Programmer-friendly REBARINFO initializes itself
//
class CRebarInfo : public REBARINFO {
public:
	CRebarInfo() {
		memset(this, 0, sizeof(REBARINFO));
		cbSize = sizeof(REBARINFO);
	}
};

//////////////////
// Programmer-friendly REBARBANDINFO initializes itself
//
class CRebarBandInfo : public REBARBANDINFO {
public:
	CRebarBandInfo();
};

//////////////////
// "Flat" style tool bar. Use instead of CToolBar in your CMainFrame
// or other window to create a tool bar with the flat look.
//
// CFlatToolBar fixes the display bug described in the article. It also has
// overridden load functions that modify the style to TBSTYLE_FLAT. If you
// don't create your toolbar by loading it from a resource, you should call
// ModifyStyle(0, TBSTYLE_FLAT) yourself.
//
class CFlatToolBar : public CToolBar {
public:
	BOOL LoadToolBar(LPCTSTR lpszResourceName);
	BOOL LoadToolBar(UINT nIDResource)
		{ return LoadToolBar(MAKEINTRESOURCE(nIDResource)); }
protected:
	DECLARE_DYNAMIC(CFlatToolBar)
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	DECLARE_MESSAGE_MAP()
	afx_msg void OnWindowPosChanging(LPWINDOWPOS lpWndPos);
	afx_msg void OnWindowPosChanged(LPWINDOWPOS lpWndPos);
};
