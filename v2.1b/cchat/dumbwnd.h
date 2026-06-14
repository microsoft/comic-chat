// DumbWnd.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDumbWnd window

class CDumbWnd : public CWnd
{
	DECLARE_DYNCREATE(CDumbWnd)
// Construction
public:
	CDumbWnd();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDumbWnd)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CDumbWnd();

	// Generated message map functions
protected:
	//{{AFX_MSG(CDumbWnd)
	afx_msg void OnPaint();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
