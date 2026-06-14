// avatardl.h : header file
//

#include <afxcmn.h>


#define cxPosAvatar             126
#define cyPosAvatar             23
#define cxAvatar				93
#define cyAvatar				217

/////////////////////////////////////////////////////////////////////////////
// CAvatarDlg dialog

class CAvatarDlg : public CDialog
{
// Construction
public:
	CAvatarDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CAvatarDlg)
	enum { IDD = IDD_CHARSELBOX };
	CListBox	m_avBox;
	//}}AFX_DATA

// Storage for currently selected Avatar
    CString     m_strAvaDirectory;
	CString		m_strAvaPick;
   	int     	m_iAvaImage;

    CStringArray m_strAvatarFiles;

// Current avatar preview
    CBodyCam	m_wndCharSelBodyCam;

// shared and per-world directories
//	CString		m_strDirShared;
//	CString		m_strDirWorld;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvatarDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

    void HandleUserAvatarSel();

	// Generated message map functions
	//{{AFX_MSG(CAvatarDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnDblclkAvlist();
	afx_msg void OnSelchangeAvlist();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
