// SoundDlg.h : header file
//

#include "rtfctrl.h"

#define SOUNDTYPE_WAV 0
#define SOUNDTYPE_MID 1
#define SOUNDTYPE_RMI 2

#define SOUNDTYPES    3	// 3 different types in total

/////////////////////////////////////////////////////////////////////////////
// CSoundList - maintains a list of sounds generated during enumeration.
// 		The sound names and types are stored here, and then used when
// 		the list control calls us back.

class CSoundList
{
public:
	CSoundList();
	~CSoundList();
	void ClearAll();
	BOOL AddName(LPCSTR pszName, int nIcon, int nType);
	void AddToListCtrl(CListCtrl* pListCtrl);
	LPCSTR GetItemName(DWORD dwData);
	int GetItemIconIndex(DWORD dwData);
	int GetItemFileType(DWORD dwData);
protected:
	LPBYTE m_pbBuffer;
	DWORD m_dwBufferSize;
	UINT m_nEntries;
	LPBYTE m_pbPosition;
};
#define SNDLIST_INITIAL_BUFFER 8192

/////////////////////////////////////////////////////////////////////////////
// CSoundDlg dialog

class CSoundDlg : public CCSDialog
{
// Construction
public:
	CSoundDlg(CWnd* pParent = NULL);   // standard constructor
	void GetSndSelection(CString &snd, BOOL bNameOnly = FALSE);

	CRtfCtrl	m_rtfCtrl;

// Dialog Data
	//{{AFX_DATA(CSoundDlg)
	enum { IDD = IDD_SOUND_DLG };
	CListCtrl	m_sndList;
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

	void StopSounds(BOOL bForce = FALSE);
	void RescanFilter(BOOL bForce = FALSE);
	
	static CImageList   sm_imglist;
	static int 			sm_nImgListIcons[8];
	BOOL		m_bStartedSounds;
	DWORD		m_dwLastFilterEdit;
	static CString sm_strMatchString;
	CSoundList  m_soundEnumList;

	// Generated message map functions
	//{{AFX_MSG(CSoundDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void OnCancel();
	afx_msg void OnTest();
	afx_msg void OnDblclkSoundlist(NMHDR *pNotifyStruct, LRESULT *plResult);
	afx_msg void OnSelChangeSoundlist(NMHDR *pNotifyStruct, LRESULT *plResult);
	afx_msg void OnSndMsgFilter(NMHDR *pNotifyStruct, LRESULT *plResult);
	afx_msg void OnTimer(UINT nTimerID);
	afx_msg void OnEditChangeFilter();
	afx_msg void OnKillFocusFilter();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


extern LPCSTR * GetSupportedSoundTypes();
extern LPCSTR GetSupportedSoundTypeList();
extern void	  EnumSounds(LPCSTR pszPath, FILEENUMSTRUCT *pfileenum) inline;
