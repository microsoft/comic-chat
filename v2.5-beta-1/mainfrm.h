// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////


#include "chatbars.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();
	virtual ~CMainFrame();


	void OleShuttingDown()
	{m_bOleShuttingDown = TRUE;}



// Attributes
public:

// Operations
public:
	virtual void GetMessageString( UINT nID, CString& rMessage ) const;
	void AutoArrangeWindows();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual LRESULT WindowProc( UINT message, WPARAM wParam, LPARAM lParam );
	virtual BOOL PreTranslateMessage( MSG* pMsg );
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public:  // control bar embedded members
	CStatusBar  m_wndStatusBar;
	CChatToolBar m_wndToolBar;
	CTabBar	m_wndTabBar;

	static BOOL CALLBACK SendBroadcastProc(HWND hwnd, LPARAM lParam);
	void SendMessageToAllChildWindows(UINT nMsg, WPARAM wParam = 0, LPARAM lParam = 0);

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnViewStatusBar();
	afx_msg void OnUpdateViewStatusBar(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintPreview(CCmdUI* pCmdUI);
	afx_msg BOOL OnQueryNewPalette();
	afx_msg void OnPaletteChanged(CWnd* pFocusWnd);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg BOOL OnBarCheck(UINT nID);
	afx_msg BOOL OnMDIWindowCmd(UINT nID);
	afx_msg void OnWindowTileAuto();
	afx_msg void OnUpdateWindowTileAuto(CCmdUI* pCmdUI);
	afx_msg void OnSize(UINT type, int cx, int cy);
	afx_msg void OnSysColorChange();
	afx_msg void OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu);
	//}}AFX_MSG
//	afx_msg LRESULT OnSetMessageString(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()


private:
	BOOL m_bOleShuttingDown;

};

