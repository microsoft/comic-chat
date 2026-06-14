// KickDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CKickDialog dialog

class CKickDialog : public CCSDialog
{
// Construction
public:
	CKickDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CKickDialog)
	enum { IDD = IDD_KICK };
	CStatic	m_Kick;
	CString	m_reason;
	CString	m_strKick;
	BOOL	m_bBanToo;
	CString	m_strBanPattern;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CKickDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CKickDialog)
	virtual BOOL OnInitDialog();
	afx_msg void OnBantoo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CBanDlg dialog

class CBanDlg : public CCSDialog
{
// Construction
public:
	CBanDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CBanDlg)
	enum { IDD = IDD_BAN };
	CComboBox	m_ctlBans;
	CString	m_strMesg;
	CString	m_strBanPattern;
	//}}AFX_DATA
	CStringArray *m_banArray;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CBanDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CBanDlg)
	afx_msg void OnEditChange();
	afx_msg void OnBan();
	afx_msg void OnUnban();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchange();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	void DoBan(BOOL bBan);
};
/////////////////////////////////////////////////////////////////////////////
// CInviteDlg dialog

class CInviteDlg : public CCSDialog
{
// Construction
public:
	CInviteDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInviteDlg)
	enum { IDD = IDD_INVITE };
	CString	m_strInvitees;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInviteDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CInviteDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CInvitationDlg dialog

class CInvitationDlg : public CDialog
{
// Construction
public:
	CInvitationDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CInvitationDlg)
	enum { IDD = IDD_INVITATION };
	BOOL	m_bIgnore;
	CString	m_strMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CInvitationDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CInvitationDlg)
	afx_msg void OnNo();
	afx_msg void OnYes();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
