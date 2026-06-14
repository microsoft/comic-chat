// setupdlg.h : header file
#ifndef __SETUPDLG_H__
#define __SETUPDLG_H__

//
// CChatFileDialog window
/////////////////////////////////////////////////////////////////////////////
const TCHAR	g_szCCCExt[] = "ccc";
const TCHAR g_szRTFExt[] = "rtf";

class CChatFileDialog : public CFileDialog
{
public:
	CChatFileDialog(BOOL bOpenFileDialog, 
					LPCTSTR lpszDefExt = NULL,
					LPCTSTR lpszFileName = NULL,
					DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
					LPCTSTR lpszFilter = NULL,
					CWnd* pParentWnd = NULL) 
				   : CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd) {};

	virtual	~CChatFileDialog() {};
	virtual void OnTypeChange();
};


//
// CFilterEdit window
/////////////////////////////////////////////////////////////////////////////
#define CTRL_V		0x16

enum FilterEditTypes
{
	FILTEREDIT_NOCHARS = 0x01,			// none of the characters in filter string.
	FILTEREDIT_NOSPC = 0x02,			// no spaces.
	FILTEREDIT_PRINTABLEONLY = 0x04,	// printable characters only 
};

class CFilterEdit : public CEdit
{
// Construction
public:
	CFilterEdit();

// Attributes
public:

// Operations
public:
	void SetFilter(DWORD dwFilterType, LPCSTR pszFilterString = NULL);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFilterEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual	~CFilterEdit() {};
	virtual BOOL CheckIfInvalid(UINT nChar);
	afx_msg BOOL OnChange();
	CString	m_strInvalid;
	BOOL	m_bPasting;
	DWORD   m_dwFilterType;
	UINT	m_nCurrLeadByte;
	CString m_strBeforeChange;
	DWORD   m_dwSelBeforeChange;
	// Generated message map functions
protected:
	//{{AFX_MSG(CFilterEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg LRESULT OnPaste(WPARAM wParam, LPARAM lParam);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


// CChanFilterEdit window
/////////////////////////////////////////////////////////////////////////////
class CChanFilterEdit : public CFilterEdit
{
// Construction
public:
	CChanFilterEdit();

// Implementation
public:
	virtual ~CChanFilterEdit() {};

	// virtual BOOL CheckIfInvalid(UINT nChar);	// REGISB 10/21/97 might be reused later again
};


/////////////////////////////////////////////////////////////////////////////
// CSetupPage dialog

enum Failed { CHATROOM, SERVER, CHANNEL};

class CSetupPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CSetupPage)

// Construction
public:
	CSetupPage();
	~CSetupPage();

	void InitVals();
	void UpdateOk();

	RECT m_rectOriginal;

	Failed nWhatFailed;

// Dialog Data
	//{{AFX_DATA(CSetupPage)
	enum { IDD = IDD_SETUPDIALOG };
	CChatServiceComboBox m_serverCtl;
	CChanFilterEdit	m_editChannel;
	CComboBox		m_ChatRooms;
	CString			m_strServer;
	CString			m_strService;
	CString			m_myChannel;
	CString			m_strChatRooms;
	int				m_radioConnect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSetupPage)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	virtual BOOL OnKillActive();
	virtual BOOL OnInitDialog();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSetupPage)
	virtual void OnCancel();
	afx_msg void OnSelchangeChatrooms();
	afx_msg void OnGetlist();
	afx_msg void OnConchan();
	afx_msg void OnNotConChan();
	afx_msg void OnChangeChannel();
	afx_msg void OnChangeServer();
	afx_msg void OnChangeServerTyped();
	afx_msg void OnConnectOnly();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	void EnableParentOK(BOOL bEnable);
	void OnConchanAux(BOOL);

	static const DWORD	m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() { return m_nHelpIDs; }
};

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// CNicknameDlg dialog

class CNicknameDlg : public CDialog
{
// Construction
public:
	CNicknameDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CNicknameDlg)
	enum { IDD = IDD_NICKNAME };
	CFilterEdit	m_editNick;
	CStatic		m_staticNick;
	CString		m_label;
	CString		m_strNickname;
	//}}AFX_DATA
	BOOL		m_bSpacesAllowed;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNicknameDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CNicknameDlg)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CChannelDlg dialog

class CChannelDlg : public CDialog
{
// Construction
public:
	CChannelDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChannelDlg)
	enum { IDD = IDD_CHANNEL };
	CFilterEdit		m_passwordCtrl;
	CButton			m_ok;
	CChanFilterEdit	m_channelCtrl;
	CString			m_strChannel;
	CString			m_strPassword;
	//}}AFX_DATA
	BOOL			m_bIsIRCX;


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannelDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChannelDlg)
	afx_msg void OnChangeChannel();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CPasswordDlg dialog

class CPasswordDlg : public CDialog
{
// Construction
public:
	CPasswordDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CPasswordDlg)
	enum { IDD = IDD_CHANPASSWORD };
	//CFilterEdit	m_passwordCtl;	// REGISB 12/02/97 not used
	CString		m_strPassword;
	CString		m_strMessage;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPasswordDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPasswordDlg)
		// NOTE: the ClassWizard will add member functions here
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern const char *GetMyName();
extern const char *GetMyScreenName();
extern const char *GetMyNickName();
extern const char *GetMyIdent();
extern const char *GetMyNickName();
extern const char *GetMyServer();
extern void GetMyServerDisplayName(CString &str);
extern const char *GetMyPhysicalServer();
extern const char *GetMyRealName();
extern const char *GetMyCharacter();
extern const char *GetMyChannel();
extern const char *GetMyHomePage();
extern const char *GetMyEmail();
extern const char *GetMyUserName();
extern void SetMyIdent(const char *szIdent);
extern void SetMyName(const char *szCharName);
extern void SetMyNameNick(const char *szNickname);
extern void SetMyRealName(const char *charName);
extern void SetMyCharacter(const char *charName);
extern void SetMyHomePage(const char *szHomePage);
extern void SetMyEmail(const char *szEmail);
extern void SetLimitedString(CString &strDest, const char *szSource, UINT uMaxBytes);


#endif // __SETUPDLG_H__
