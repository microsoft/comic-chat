// motd.h : header file
//

#include "rtfctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CMOTD dialog

class CMOTD : public CCSDialog
{
// Construction
public:
	CMOTD(CWnd* pParent = NULL);   // standard constructor

	CRichEditCtrl m_edit;
	CString m_strLUSER;
	CString m_strMOTD;

// Dialog Data
	//{{AFX_DATA(CMOTD)
	enum { IDD = IDD_MOTD };
	BOOL	m_bShowMOTD;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMOTD)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CMOTD)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CAwayDlg dialog
class CAwayDlg : public CDialog
{
// Construction
public:
	CAwayDlg(CWnd* pParent = NULL);   // standard constructor

	void OnChangeAwayMsg(NMHDR *pNotifyStruct, LRESULT *plResult);
	void OnFilterAwayMsg(NMHDR *pNotifyStruct, LRESULT *plResult);

// Dialog Data
	//{{AFX_DATA(CAwayDlg)
	enum { IDD = IDD_AWAYDLG };
	CRtfCtrl	m_rtfAwayMsg;
	//}}AFX_DATA

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAwayDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CAwayDlg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
