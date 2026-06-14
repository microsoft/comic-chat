// setupdialog.h : header file
//
// CNicknameEdit window
/////////////////////////////////////////////////////////////////////////////
class CNicknameEdit : public CEdit
{
// Construction
public:
	CNicknameEdit();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNicknameEdit)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CNicknameEdit();
	BOOL CheckIfInvalid(UINT nChar);
	CString m_strInvalid;
	// Generated message map functions
protected:
	//{{AFX_MSG(CNicknameEdit)
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// CSetupDialog dialog

enum Failed { CHATROOM, SERVER, PORT, CHANNEL};


class CSetupDialog : public CDialog
{
// Construction
public:
	CSetupDialog(CWnd* pParent = NULL);   // standard constructor
//	virtual BOOL LoadFromIni();
	virtual BOOL LoadFromReg();
	virtual BOOL SaveToIni();
	virtual BOOL SaveToReg();
	virtual void DrawAvatarFeedback();
	void CSetupDialog::UpdateOk(CString &nickname);

	CString m_myCharacterName;
	RECT m_rectOriginal;
	BOOL bExpanded;

	Failed nWhatFailed;

// Dialog Data
	//{{AFX_DATA(CSetupDialog)
	enum { IDD = IDD_SETUPDIALOG };
	CNicknameEdit	m_editChannel;
	CButton	m_ExpandButton;
	CComboBox	m_ChatRooms;
	CNicknameEdit	m_editNickname;
	CButton	m_ok;
	CComboBox	m_myCharacter;
	CString	m_myName;
	CString	m_ircServer;
	CString	m_myRealName;
	CString	m_myChannel;
	UINT	m_ircPort;
	CString	m_strChatRooms;
	BOOL	m_bGetChannelList;
	int		m_radioConnect;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetupDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
	private:
//		CListBox* charCtl() { return (CListBox *) GetDlgItem(IDC_CHARACTER); }
protected:

	// Generated message map functions
	//{{AFX_MSG(CSetupDialog)
	virtual void OnOK();
	afx_msg void OnSelchangeCharacter();
	virtual void OnCancel();
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeNickname();
	afx_msg void OnSetuphelp();
	afx_msg void OnSelchangeChatrooms();
	afx_msg void OnGetlist();
	afx_msg void OnConchan();
	afx_msg void OnListchan();
	afx_msg void OnChangeChannel();
	afx_msg void OnChangePortnum();
	afx_msg void OnChangeServer();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
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
	CNicknameEdit	m_editNick;
	CStatic	m_staticNick;
	CString	m_label;
	CString	m_strNickname;
	//}}AFX_DATA


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
	CButton	m_ok;
	CNicknameEdit	m_channelCtrl;
	CString	m_strChannel;
	//}}AFX_DATA


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
