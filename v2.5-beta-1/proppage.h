// PropPage.h : header file
//
#include "rtfctrl.h"

#define cxPosAvatarPage             141  //162
#define cyPosAvatarPage             21   //20
#define cxAvatarPage				100  //90
#define cyAvatarPage				188  //170
/////////////////////////////////////////////////////////////////////////////
// CSettingsPage dialog

class CSettingsPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CSettingsPage)

// Construction
public:
	CSettingsPage();
	~CSettingsPage();

// Dialog Data
	//{{AFX_DATA(CSettingsPage)
	enum { IDD = IDD_SETTINGSPAGE };
	CButton	m_RatingsGroupBox;
	CStatic	m_RatingsIcon;
	CStatic	m_RatingsText;
	CButton	m_RatingsOn;
	CButton	m_AdvancedRatings;
	CButton	m_checkData;
	CButton	m_checkSave;
	BOOL	m_bSave;
	BOOL	m_bComicsData;
	BOOL	m_bAcceptWhispers;
	BOOL	m_bShowArrivals;
	CString	m_strSoundPath;
	BOOL	m_bAllowInvites;
	BOOL	m_bAcceptNMCalls;
	BOOL	m_bPlaySounds;
	BOOL	m_bShowIdentity;
	BOOL	m_bAllowFileTX;
	BOOL	m_bVisible;
	//}}AFX_DATA

	BOOL m_bUseRatings;
	CString m_strRatingsButtonText;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSettingsPage)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CSettingsPage)
	afx_msg void OnComicsdata();
	afx_msg void OnSave();
	afx_msg void OnAdvancedRatingsButton();
	afx_msg void OnRatingsTurnOn();
	afx_msg void OnAcceptwhispers();
	afx_msg void OnShowarrivals();
	afx_msg void OnChangeSoundpath();
	afx_msg void OnAllowinvites();
	afx_msg void OnAcceptNMCalls();
	afx_msg void OnPlaysounds();
	afx_msg void OnShowidentity();
	afx_msg void OnAllowFiletx();
	afx_msg void OnInvisible();
	afx_msg void OnBrowseForSoundDir();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CPersonalPage dialog

class CPersonalPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CPersonalPage)

// Construction
public:
	CPersonalPage(UINT);
	~CPersonalPage();

// Dialog Data
	//{{AFX_DATA(CPersonalPage)
	enum { IDD = IDD_PERSONALPAGE };
	CEdit			m_editRealName;
	CFilterEdit		m_editHomePage;
	CFilterEdit		m_editEmail;
	CFilterEdit		m_editNickname;
	CRtfCtrl		m_rtfProfile;
	CString			m_strNickname;
	CString			m_strRealName;
	CString			m_strEmail;
	CString			m_strHomePage;
	CButton			m_checkSave;
	// For CB32SUPPORT
	//BOOL			m_bSave;
	//BOOL			m_bAcceptWhispers;
	//BOOL			m_bShowArrivals;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CPersonalPage)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void				OnFilterProfile(NMHDR *pNotifyStruct, LRESULT *plResult);
	static const DWORD	m_nHelpIDs[];
	virtual const DWORD	*GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CPersonalPage)
	afx_msg void OnChangeEdit();
	afx_msg void OnSave();
	afx_msg void OnAcceptwhispers();
	afx_msg void OnShowarrivals();
	afx_msg void OnChangeProfile();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL	m_bOKing;
};
/////////////////////////////////////////////////////////////////////////////
// CCharacterPage dialog

class CCharacterPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CCharacterPage)

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
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CCharacterPage)
	afx_msg void OnSelchangeAvlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CBackgroundPage dialog

class CBackgroundPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CBackgroundPage)

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
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CBackgroundPage)
	afx_msg void OnSelchangeBacklist();
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
/////////////////////////////////////////////////////////////////////////////
// CTextFontPage dialog

#include "dlgs.h"

class CTextFontPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CTextFontPage)

// Construction
public:
	CTextFontPage(UINT);
	~CTextFontPage();

// Dialog Data
	//{{AFX_DATA(CTextFontPage)
	enum { IDD = IDD_TEXTFONTPAGE_IRC };
	BOOL	m_hostHdrsBold;
	BOOL	m_hostMsgsBold;
	BOOL	m_bHeaderSeparate;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CTextFontPage)
	public:
	virtual void OnOK();
	virtual BOOL OnSetActive();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CTextFontPage)
	afx_msg void OnSetfont();
	afx_msg void OnLinespaceAll();
	afx_msg void OnLinespaceDifferent();
	afx_msg void OnLinespaceNone();
	afx_msg void OnResetTextfonts();
	afx_msg void OnHostHdrsBold();
	afx_msg void OnHostMsgsBold();
	afx_msg void OnHeaderSeparate();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
	BYTE		m_spacing;
	BOOL		m_bCfInitialized;
	BOOL		m_bCfHLInitialized;
	CHARFORMAT	m_cfArray[NFONTS];
	BOOL		m_bCfChangesMade;
};

/////////////////////////////////////////////////////////////////////////////
// CComicsPropPage dialog

class CComicsPropPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CComicsPropPage)

// Construction
public:
	CComicsPropPage();
	~CComicsPropPage();

// Dialog Data
	//{{AFX_DATA(CComicsPropPage)
	enum { IDD = IDD_COMICS_VIEW };
	CComboBox	m_comboPanels;
	//}}AFX_DATA

	int		m_nPanelsSel;
	BOOL	m_bPanelClicked;
	BOOL	m_bShowComicRTF;
	BOOL	m_bAutoDownloadChars;
	BOOL	m_bAutoDownloadBackdrops;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CComicsPropPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CComicsPropPage)
	afx_msg void OnShowComicRTF();
	afx_msg void OnSetfont();
	afx_msg void OnSelchangePanels();
	afx_msg void OnResetfont();
	afx_msg void OnAutoDownloadChars();
	afx_msg void OnAutoDownloadBackdrops();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////
// CServersPage dialog

class CServersOnlyComboBox : public CIconicComboBox
{
public:
	virtual HICON GetIcon(UINT nIndex, LPCSTR pszString, DWORD dwItemData);
	virtual BOOL ShouldDrawDivision(UINT nIndex, LPCSTR pszString, DWORD dwItemData);
protected:
	CSmallIcon m_icon;
};

class CServersPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CServersPage)

// Construction
public:
	CServersPage();
	~CServersPage();

// Dialog Data
	//{{AFX_DATA(CServersPage)
	enum { IDD = IDD_SERVERSPAGE };
	CServersOnlyComboBox m_comboGroups;
	CSimpleComboBox m_comboServers;
	CString 		m_strCurrentGroup;
	CString			m_strCurrentServer;
	int				m_nPort;
	int				m_nSecurity;
	CString			m_strUserName;
	CString			m_strPassword;
	BOOL			m_bRememberPassword;
	CString			m_strSecurityPackages;
	int				m_nCurrSelGroup;
	int				m_nCurrSelServer;
	//}}AFX_DATA
	static CString  sm_strUnassociatedGroup;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CServersPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// The first list lists all the update hints. The second list lists all
	// the elements to be updated. Update hints are defined so that they use
	// bit values from the update element list to show what has to be updated.
	enum UIUpdateHint 
	{
		updateAll = 0xffff,
		updateChangeCurrSrvGroup = 0x000f,
		updateNewServerGroupText = 0x0002,
		updateAddSrvGroup = 0x000f,
		updateRemoveSrvGroup = 0x000f,
		updateChangeCurrServer = 0x000c,
		updateNewServerText = 0x0004,
		updateAddServer = 0x000e,
		updateRemoveServer = 0x000e,
		updateChangeAuthenticationType = 0x0008,
	};
	enum UIUpdateElement
	{
		updateelemEnableServerUI = 0x0001,
		updateelemAddRemoveGroup = 0x0002,
		updateelemAddRemoveServer = 0x0004,
		updateelemServerProps = 0x0008,
	};


	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}
	void SwitchGroup();
	void SwitchServer();
	void UpdateUIState(DWORD dwUpdateHint = updateAll);
	void GetServerPropsFromPage(CDataExchange* pDX);
	void SetServerPropsToPage(CDataExchange* pDX);
	BOOL AcceptServerSettings();
	void ChangeServerGroup(BOOL bKeystroke, BOOL bForce = FALSE);
	void ChangeServer(BOOL bKeystroke, BOOL bForce = FALSE);
	HCHATSRVGROUP GetCurrGroup() 
		{ return m_nCurrSelGroup == CB_ERR ? NULL : (HCHATSRVGROUP)m_comboGroups.GetItemDataPtr (m_nCurrSelGroup); }
	HCHATSERVER GetCurrServer() 
		{ return m_nCurrSelServer == CB_ERR ? NULL : (HCHATSERVER)m_comboServers.GetItemDataPtr (m_nCurrSelServer); }
	BOOL ValidatePortNumber();
	void ReloadSettings();

	CChatServiceList* m_pSvcList;
	CChatServiceUI m_ui;
	BOOL m_bServerPropChange;
	BOOL m_bSetActiveNeverCalled;
	BOOL m_bInDDX;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	//{{AFX_MSG(CServersPage)
	afx_msg void OnSelChangeServerGroup();
	afx_msg void OnSelChangeServer();
	afx_msg void OnEditChangeServerGroup();
	afx_msg void OnEditChangeServer();
	afx_msg void OnAddServer();
	afx_msg void OnRemoveServer();
	afx_msg void OnAddServerGroup();
	afx_msg void OnRemoveServerGroup();
	afx_msg void OnKillFocusPort();
	afx_msg void OnChangeAuthenticationType();
	afx_msg void OnChangeServerProp();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};
