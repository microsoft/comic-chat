// MyFontDg.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CMyFontDialog dialog

class CMyFontDialog : public CWin4FontDialog
{
	DECLARE_DYNAMIC(CMyFontDialog)

public:
	CMyFontDialog(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#ifndef _AFX_NO_RICHEDIT_SUPPORT
	CMyFontDialog(const CHARFORMAT& charformat,
		DWORD dwFlags = CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL);
#endif
	virtual	~CMyFontDialog();

protected:
	//{{AFX_MSG(CMyFontDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnFontChange();
	virtual void OnOK();
	afx_msg void OnDestroy();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	int GetMessageType();
	BOOL SetupRichPreview();
	void HandleLink(NMHDR * pNotifyStruct, LRESULT * result);
	void OnChangeMessageType();
	void HandleSelection(NMHDR * pNotifyStruct, LRESULT * result);
	void GetUpperAndLower(SELCHANGE *lpSelChange, int &lower, int &upper);
	void AtomizeSelection(SELCHANGE *lpSelChange);
	void SyncFontControls(SELCHANGE *lpSelChange);
	void SyncMessageType(SELCHANGE *lpSelChange);
	void SetMessageType(int mType);
	DWORD GetCharFormatMask();
	void OnStrikeoutChange();
	void OnUnderlineChange();
	void UpdateFromCf();

	void GetFontFromUI(LPLOGFONT lplf);
	void SetFontToUI(LPLOGFONT lplf);
	static BOOL SupportsFontMessages();
	void GetCurrentFont(LPLOGFONT lplf) { GetFontFromUI (lplf); }

	BOOL			m_bClosing;
	BOOL			m_bFontCtlEvents;
	BOOL			m_bDeleteFont;
	CRichEditCtrl*	m_richPreview;
	CTextCore*		m_richCore;
	CFont			m_font;
	HCURSOR			m_hCursorHand;
	//BOOL			m_bCfInitialized;
	//BOOL			m_bCfHLInitialized;
	BOOL			m_bSetFromCf;
	CHARFORMAT		m_cfArray[NFONTS];
	BOOL			m_bSettingUI;
};
