// bodycam.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CBodyCam window

class CBodyCam : public CWnd
{
	DECLARE_DYNCREATE(CBodyCam)
// Construction
public:
	CBodyCam();

	BOOL		m_forcedDelete;						// needs to be deleted in OnNCDestroy
	CAvatarX	*m_avatar;							// Avatar being controlled by widget

// Attributes
protected:
	BOOL		m_mouseDown;
	BOOL		m_bullDisabled;
	CEmotion	m_emotion;
	POINT		m_cursorPos;
	CPoint		m_bullsEye;
	short		m_bullRadius;
	short		m_bullSide;
	CDIB		m_icons[NEMOTIONS];
	RECT		m_bodyRect;
	static short	m_cursorRadius;
	static short	m_iconWidth;
	static short	m_iconHeight;
	CToolTipCtrl	m_toolTip;						// for emotion tooltips
	char			m_toolTipString[15];			// permanent string to hold emotion tooltips
	CPalette		*m_palette;
	HBITMAP		m_retSec;					// dib section for retained dib (for character display)
	CDIB		*m_retDib;					// retained dib (for character display)
	CString		*m_lastEmotionString;		// so we don't need to refresh status more than necessary
// Operations
public:
	inline void CacheBullSide(int winWidth);
	void DrawBullsEye(CDC *, RECT &);
	void DrawBullsEyeCons(CDC *dc, RECT &rect);
	void DrawCursor(POINT &, CDC * = NULL);
//	void EraseCursor(POINT &);
	void SetEmotion(CEmotion &emotion) { m_emotion = emotion; }
	CEmotion GetEmotionFromPoint(POINT &);
	POINT GetPointFromEmotion(CEmotion &);
	void UpdateEmotion(CEmotion &);
	void GetBodyRect(RECT &) inline;
	RECT DrawBody(CDC *, CBody *);
	void EraseRect(CDC *, RECT *);
	void GetBodyBox(CPose *pose, int &left, int &top, int &width, int &height);
	void RefreshBody();
	int OnToolHitTest( CPoint point, TOOLINFO* pTI ) const;
	RECT GetIconRect(int i) const;
	CPalette *GetPalette(CDC *) { if ( m_palette) return m_palette; else return (InstallPalette()); }  // for now, always return same palette
	CPalette *InstallPalette();
	void FreeRetainedPanel() {	if (m_retDib) delete m_retDib; if (m_retSec) ::DeleteObject(m_retSec); m_retDib = NULL; m_retSec = NULL; }
	void RecalcRetainedBMP();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBodyCam)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CBodyCam();

	// Generated message map functions
protected:
	//{{AFX_MSG(CBodyCam)
	afx_msg void OnPaint();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnBodycontextFreeze();
	afx_msg void OnBodycontextSendexpression();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnNcDestroy();
	afx_msg void OnUpdateBodycontextFreeze(CCmdUI* pCmdUI);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

extern CBodyCam *GetBodyCam();
