// chanprop.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CChannelProp dialog

#include "rtfctrl.h"

class CChannelProp : public CCSDialog
{
// Construction
public:
	CChannelProp(CWnd* pParent = NULL);   // standard constructor
	CChannelProp(UINT nidTemplate, CWnd* pParent);  // special constructor for subclassing
	void DoMyInits();

// Dialog Data
	//{{AFX_DATA(CChannelProp)
	enum { IDD = IDD_CHANNELPROP };
	BOOL		m_bAuditorium;
	BOOL		m_bSecret;
	BOOL		m_bInviteOnly;
	BOOL		m_bModerated;
	BOOL		m_bPrivate;
	CRtfCtrl	m_rtfTopic;
	BOOL		m_bTopicAnyone;
	UINT		m_uMaxParticipants;
	BOOL		m_bSetMax;
	BOOL		m_bNoWhispers;
	CString		m_strPassword;
	BOOL		m_bSetPassword;
	//CFilterEdit	m_passwordCtl;	// REGISB 12/02/97 not used
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannelProp)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CChannelProp)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnSetmax();
	afx_msg void OnHidden();
	afx_msg void OnPrivate();
	afx_msg void OnSetPassword();
	afx_msg void OnTopicFilter(NMHDR *pNotifyStruct, LRESULT *plResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
/////////////////////////////////////////////////////////////////////////////
// CChannelCreateDlg dialog

class CChannelCreateDlg : public CChannelProp
{
// Construction
public:
	CChannelCreateDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CChannelCreateDlg)
	enum { IDD = IDD_CHANNELCREATE };
	CChanFilterEdit	m_channelCtl;
	CString			m_strChannelName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChannelCreateDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CChannelCreateDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnChangeChannelname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
