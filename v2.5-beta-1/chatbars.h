#ifndef _CHATBARS_H_
#define _CHATBARS_H_

class CCoolToolBarEx : public CCoolToolBar
{
public:
	CCoolToolBarEx() 
		{ m_bVisible = TRUE; }
	void ModifyButtonStyle(UINT nID, DWORD dwAdd, DWORD dwRemove = 0);
	BOOL m_bVisible;
};

class CCoolBarEx : public CCoolBar
{
public:
	CCoolBarEx();
	virtual ~CCoolBarEx();
	BOOL Create(CWnd* pParentWnd, UINT* pnIDToolBars, PBOOL pbVisibility, DWORD dwStyle = CBRS_ALIGN_TOP|CBRS_BORDER_TOP, UINT nID=AFX_IDW_TOOLBAR);

	BOOL SaveStateToBuffer(PBYTE * pBufferOut, UINT * pOutBufferSize);
	BOOL LoadStateFromBuffer(PBYTE pBuffer);
	void ShowBar(UINT nBarID, BOOL bShow = TRUE);
	BOOL IsBarShown(UINT nBarID);
	CCoolToolBarEx* GetToolBarFromID(UINT nBarID);

protected:
	virtual BOOL OnCreateBands();
	virtual void OnPrepareToolBar(UINT nID, CCoolToolBarEx * pToolBar);
	BOOL AddToolBarBands(int nCount = 0, PVOID pvState = NULL);
	int FindBand(UINT nID);
	BOOL AddSingleBand(UINT nID, CCoolToolBarEx* pToolBar, int cx = -1, BOOL bBreakBefore = FALSE);
	void RestoreBandMinSize(UINT nID, CCoolToolBarEx* pToolBar);

	UINT* 		  m_pnIDToolBars;
	UINT* 		  m_pnIDToolBarsTemp;
	PBOOL		  m_pbVisibilityTemp;
	CMapWordToPtr m_mapToolBars;
	UINT		  m_nLastShown;
	CWordArray	  m_arrBandsNotAdded;
	BOOL		  m_bOldVersion;

	afx_msg void OnDestroy();
	afx_msg void OnSysColorChange();
	DECLARE_MESSAGE_MAP()
};

class CChatToolBar : public CCoolBarEx
{
public:
	BOOL Create(CWnd* pParentWnd, BOOL bDoCB32);
	void ToggleBar(UINT nWhich);
protected:
	virtual void OnPrepareToolBar(UINT nID, CCoolToolBarEx * pToolBar);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint pt);
	DECLARE_MESSAGE_MAP()
};

#endif
