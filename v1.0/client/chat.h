// chat.h : main header file for the CHAT application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// Parameters

#define AVATARDIR			"ComicArt\\Avatars"
#define BACKDROPDIR			"ComicArt\\Backdrop"
//#define CHATHELPDIR			"Help"


/////////////////////////////////////////////////////////////////////////////
// CChatApp:
// See chat.cpp for the implementation of this class
//

class CChatApp : public CWinApp
{
public:
	CFont	m_fontText;
	BOOL m_bComicView;
	BOOL m_bBother;

public:
	CChatApp();
	~CChatApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	virtual BOOL OnIdle(LONG lCount);
	//}}AFX_VIRTUAL

// Implementation
	COleTemplateServer m_server;
		// Server object for document creation

	//{{AFX_MSG(CChatApp)
	afx_msg void OnHelpTopics();
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
// Parsed command-line arguments

	char *	m_pszName;
	char *	m_pszScene;
	DWORD	m_dwCID;
	BOOL 	m_bTestMode;
	BOOL 	m_bNoNetwork;
	BOOL	m_bNoRefresh;
	BOOL	m_bNoSong;
	BOOL	m_bNoFX;
	BOOL	m_b2DWorld;
	BOOL	m_bRGBMode;
	BOOL	m_bNoFastMode;
	BOOL	m_bFPSDisplay;
	int		m_xFrame;
	int		m_yFrame;
	int		m_cxFrame;
	int		m_cyFrame;
	int		m_maxedFrame;
	CString m_lastBackDrop;

// search paths
	CString m_strBaseDir;
	CString	m_strAvatarDir;
	CString m_strBackDropDir;
	CString m_strFavoritesDir;

// additional methods

public:
	//helper method for status bar strings
	void SetStatusPaneString(int index, CString &strPane);
	void SaveConnectStatus(CString &strStatus);
	void ResetStatus();
	CString m_strStatusPane[2];
	CString m_strStatus;
	void SetPrinterResolution();
	CDocument* OpenDocumentFile(LPCTSTR lpszFileName);

private:
	void SetBaseDir(const char *fullpath);
public:
	const char *GetBaseDir() { return ((LPCSTR) m_strBaseDir); }
	const char *GetAvatarDir() { return ((LPCSTR) m_strAvatarDir); }
	const char *GetBackDropDir() { return ((LPCSTR) m_strBackDropDir); }
};

// some defines
#define	MSTITLE			// use the MS Comic Sans title font -- not the special purpose title font

/////////////////////////////////////////////////////////////////////////////
