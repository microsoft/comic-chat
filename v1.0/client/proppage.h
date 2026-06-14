// PropPage.h : header file
//
#define cxPosAvatarPage             141  //162
#define cyPosAvatarPage             21   //20
#define cxAvatarPage				100  //90
#define cyAvatarPage				188  //170
/////////////////////////////////////////////////////////////////////////////
// CSettingsPage dialog

class CSettingsPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CSettingsPage)

// Construction
public:
	CSettingsPage();
	~CSettingsPage();

// Dialog Data
	//{{AFX_DATA(CSettingsPage)
	enum { IDD = IDD_SETTINGSPAGE };
	CComboBox	m_comboPanels;
	CButton	m_checkData;
	CButton	m_checkSave;
	BOOL	m_bSave;
	BOOL	m_bComicsData;
	//}}AFX_DATA
	int m_nPanelsSel;
	BOOL m_bPanelClicked;


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSettingsPage)
	public:
	virtual BOOL OnKillActive();
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSettingsPage)
	afx_msg void OnComicsdata();
	afx_msg void OnSelchangePanels();
	afx_msg void OnSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPersonalPage dialog

class CPersonalPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CPersonalPage)

// Construction
public:
	CPersonalPage();
	~CPersonalPage();

// Dialog Data
	//{{AFX_DATA(CPersonalPage)
	enum { IDD = IDD_PERSONALPAGE };
	CEdit	m_editRealName;
	CNicknameEdit	m_editNickname;
	CEdit	m_editProfile;
	CString	m_strProfile;
	CString	m_strNickname;
	CString	m_strRealName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPersonalPage)
	public:
	virtual void OnOK();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CPersonalPage)
	afx_msg void OnChangeProfileEdit();
	afx_msg void OnChangeNickname();
	afx_msg void OnChangeRealname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CCharacterPage dialog

class CCharacterPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CCharacterPage)

// Construction
public:
	CCharacterPage();
	~CCharacterPage();

// Dialog Data
	//{{AFX_DATA(CCharacterPage)
	enum { IDD = IDD_CHARACTERPAGE };
	CListBox	m_avList;
	CString	m_strList;
	//}}AFX_DATA

	CString m_strSel;

// Storage for currently selected Avatar
    CString     m_strAvaDirectory;
	CString		m_strAvaPick;
   	int     	m_iAvaImage;

    CStringArray m_strAvatarFiles;

// Current avatar preview
    CBodyCam	m_wndCharSelBodyCam;
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CCharacterPage)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual void OnCancel();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CCharacterPage)
	afx_msg void OnSelchangeAvlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CBackgroundPage dialog

class CBackgroundPage : public CPropertyPage
{
	DECLARE_DYNCREATE(CBackgroundPage)

// Construction
public:
	CBackgroundPage();
	~CBackgroundPage();

// Dialog Data
	//{{AFX_DATA(CBackgroundPage)
	enum { IDD = IDD_BACKGROUNDPAGE };
	CListBox	m_listBack;
	CString	m_strBackground;
	//}}AFX_DATA
	CDIB m_dib;
	CString m_strCurrentSel;
	CRect m_rect;
	void PreviewSelection();

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CBackgroundPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CBackgroundPage)
	afx_msg void OnSelchangeBacklist();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
