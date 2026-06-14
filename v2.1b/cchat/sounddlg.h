// SoundDlg.h : header file
//

#include "rtfctrl.h"

/////////////////////////////////////////////////////////////////////////////
// CSoundDlg dialog

class CSoundDlg : public CCSDialog
{
// Construction
public:
	CSoundDlg(CWnd* pParent = NULL);   // standard constructor
	void GetSndSelection(CString &snd);

	CRtfCtrl	m_rtfCtrl;

// Dialog Data
	//{{AFX_DATA(CSoundDlg)
	enum { IDD = IDD_SOUND_DLG };
	CListBox	m_sndList;
	CString		m_selectedSnd;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSoundDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CSoundDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnTest();
	afx_msg void OnDblclkSoundlist();
	afx_msg void OnSelChangeSoundlist();
	afx_msg void OnSndMsgFilter(NMHDR *pNotifyStruct, LRESULT *plResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
