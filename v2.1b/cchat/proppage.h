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
	BOOL			m_bSave;
	BOOL			m_bAcceptWhispers;
	BOOL			m_bShowArrivals;
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
	void				OnChangeProfile(NMHDR *pNotifyStruct, LRESULT *plResult);
	void				OnFilterProfile(NMHDR *pNotifyStruct, LRESULT *plResult);
	static const DWORD	m_nHelpIDs[];
	virtual const DWORD	*GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CPersonalPage)
	afx_msg void OnChangeEdit();
	afx_msg void OnClose();
	afx_msg void OnSave();
	afx_msg void OnAcceptwhispers();
	afx_msg void OnShowarrivals();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

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
	CHARFORMAT	m_cfArray[10];
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
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};


/////////////////////////////////////////////////////////////////////////////
// CAutomationPage dialog

class CAutomationPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CAutomationPage)

// Construction
public:
	CAutomationPage();
	~CAutomationPage();

	int		m_iGreetingType;
	CMacro	m_macros[NMACROS];

// Dialog Data
	//{{AFX_DATA(CAutomationPage)
	enum { IDD = IDD_AUTOMATION_PAGE };
	CRtfCtrl		m_rtfMacro;
	CRtfCtrl		m_rtfGreetingMesg;
	CEdit			m_macroNameCtl;
	CEdit			m_mesgCntCtl;
	CEdit			m_intervalCtl;
	CComboBox		m_keyCtl;
	CSpinButtonCtrl	m_spinMesgCnt;
	CSpinButtonCtrl	m_spinInterval;
	BOOL			m_bAutoIgnore;
	UINT			m_uMesgCnt;
	UINT			m_uInterval;
	CString			m_strMesgCnt;
	CString			m_strInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAutomationPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void				OnChangeGreetingMesg(NMHDR *pNotifyStruct, LRESULT *plResult);
	void				OnFilterGreetingMesg(NMHDR *pNotifyStruct, LRESULT *plResult);
	void				OnFilterMacro(NMHDR *pNotifyStruct, LRESULT *plResult);
	static const  DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CAutomationPage)
	afx_msg void OnMesgCountChg();
	afx_msg void OnIntervalChg();
	afx_msg void OnAutoIgnore();
	afx_msg void OnNogreeting();
	afx_msg void OnSaygreeting();
	afx_msg void OnWhispergreeting();
	afx_msg void OnAddMacro();
	afx_msg void OnDeleteMacro();
	afx_msg void OnSelchangeKey();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
