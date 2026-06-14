// profile.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CProfileDialog dialog

class CProfileDialog : public CDialog
{
// Construction
public:
	CProfileDialog(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CProfileDialog)
	enum { IDD = IDD_PROFILE };
	CString	m_profileString;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CProfileDialog)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CProfileDialog)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
