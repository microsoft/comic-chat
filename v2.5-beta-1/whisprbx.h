// WhisprBx.h : header file
//

#ifndef __WHISPRBX_H__
#define __WHISPRBX_H__

#include "saywnd.h"
#include "textcore.h"

class CWhisperLeaf {
public:
	CWhisperLeaf(CUserInfo *pui)
	{
		m_label = pui->GetScreenName();
		m_nick = pui->GetName();
		m_bModified = FALSE;
		m_bIgnore = pui->Ignored();
		m_id = pui->GetFullName();
	}

	~CWhisperLeaf();
	
	CString			m_label;
	CString			m_nick;
	CString			m_id;
	BOOL			m_bIgnore;
	BOOL			m_bModified;
	CRichEditCtrl*	m_richView;
	CTextCore*		m_richCore;
};

/////////////////////////////////////////////////////////////////////////////
// CWhisperBox dialog

class CWhisperBox : public CDialog
{

public:
	CWhisperBox(CWnd* pParent = NULL);   // standard constructor
	~CWhisperBox();
	void FreeLeaves();
	int AddTab(CUserInfo *);
	int GetTab(const char *szNick);
	void SwitchToTab(int iTabNum);
	void SetModified(int iTabNum, BOOL bValue); 
	virtual void OnOK();  // override to get carriage return;
	void SaveWhisperCoords();
	CRichEditCtrl* GetCurrentEdit();
	int			m_currentIndex;  // index of currently displayed leaf
	CPtrArray	m_leaves;
	CSayWnd*	m_sayWnd;
	RECT		m_richRect;
	BOOL		m_bPostCreate;
	BOOL		m_bInverted;		// title bar inverted, meaning unseen messages

// Dialog Data
	//{{AFX_DATA(CWhisperBox)
	enum { IDD = IDD_WHISPERBOX };
	CButton		m_ignoreButton;
	CButton		m_deleteButton;
	CTabCtrl	m_tabCtrl;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CWhisperBox)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CWhisperBox)
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangeTab(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDeleteTab();
	afx_msg void OnIgnoreWbox();
	afx_msg void OnClose();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	//}}AFX_MSG
	afx_msg void HandleLink ( NMHDR * pNotifyStruct, LRESULT * result );
	afx_msg void OnGetMinMaxInfo( MINMAXINFO FAR* lpMMI );
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint screenPoint);
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	DECLARE_MESSAGE_MAP()
};


CWhisperBox*	CreateWhisperBox();
void			WhisperBox(CUserInfo *pui, BOOL bGiveFocus = TRUE, BOOL bRestore = TRUE);
void			DestroyWhisperBox();
void			InitializeWhisperCores(BOOL bRestoreOld);
BOOL			bAddToWhisperBox(CUserInfo *pui, USHORT uModes, const char *szMesg);
BOOL			bWhisperInBox(CString strFilename, CString strMesg, CDWordArray *prgdwFormatting, USHORT uModes);


#endif // __WHISPRBX_H__

