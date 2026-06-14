// filesend.h : header file
//

/////////////////////////////////////////////////////////////////////////////

// CFileProgress dialog

class CFileProgress : public CDialog
{
// Construction
public:
	CFileProgress(CWnd* pParent = NULL);   // standard constructor
	~CFileProgress();

// Dialog Data
	//{{AFX_DATA(CFileProgress)
	enum { IDD = IDD_FILE_TRANSFER };
	CProgressCtrl	m_fileProgress;
	CString	m_bytesSent;
	CString	m_bytesTotal;
	CString	m_strStatus;
	CString	m_strXferredLabel;
	//}}AFX_DATA
	int m_iBytesTotal;
	CString m_strFileName;
	CString m_strOtherGuy;
	BOOL m_bSending;
	void *m_fileTX;			// really circular ref to FILETXINFO


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CFileProgress)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CFileProgress)
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
	afx_msg LRESULT OnStatChange(WPARAM wParam, LPARAM lParam);
};

typedef struct {
	HWND progHwnd;
	CFileProgress *progDlg;
	long hostAddr;		// only set on receive
	short port;
	unsigned long txThread;
	char pathName[MAX_PATH];
	int fileSize;		// only set on receive
	BOOL bCancel;
} FILETXINFO;
