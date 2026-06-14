#ifndef __CHATDOC_H__
#define __CHATDOC_H__

#include "chatprot.h"
#include "binddoc.h"

// chatDoc.h : interface of the CChatDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CChatItem;

class CChatDoc : public CDocObjectServerDoc
{
protected: // create from serialization only
	CChatDoc();
	DECLARE_DYNCREATE(CChatDoc)

// Attributes
public:
	CPtrList			m_pages;
	CPtrList			m_history;
	CRoomInfo*			m_proto;
	CWnd*				m_memberList;
	CWnd*				m_bodyCam;
	CWnd*				m_sayWnd;
	CView*				m_view;
	CView*				m_client;
	CNCSMapStringToPtr	m_mapNickToPtr;
	CPtrList			m_allChannelPuis;
	UINT				m_myAvatarID;
	UINT				m_myBackDropID;
	CUserInfo*			m_puiSelf;
	CString				m_strStatus;
	int					m_seed;
//	CString				m_tabName;

	RECT				m_bbox;
	char*				m_comicsTitle;
	char				m_fileType;		// current file type for saving and opening
	BOOL				m_bIconMembers;
	BOOL				m_bLastMemberView;
	BOOL				m_bComicView;
	BOOL				m_bStatusView;
	BOOL				m_bNewContent;
	BOOL				m_bObscured;
	BOOL				m_bArchived;

	// System generated function
	CChatItem* GetEmbeddedItem()
		{ return (CChatItem*)COleServerDoc::GetEmbeddedItem(); }

// Operations
public:
	void AddLine(UINT uID, const char *szText, USHORT uModes, CDWordArray *prgdwFormatting = NULL);
	void ProcessLine(UINT uID, const char *szLine, USHORT uModes, BYTE bbCooked, CDWordArray *prgdwFormatting = NULL);
	void TallySpeech(UINT id);
	void ShowInfo(void *, const char *, BOOL, char);		// show an info box for a character
	void InitMyDocument();
	void AddNewPage();
	void DestroyHistory();
	void InitHistory();
	void ExecuteHistory(int mode);
	void DestroyPages();
	void ReadSampleConversation(const char *);
	BOOL ParseLocatorFile(LPCTSTR);
	int FindFileType(const char *);
	BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	COleIPFrameWnd* CreateInPlaceFrame(CWnd* pParentWnd);
	void RegenerateView();
	CWnd *GetOrigParent() { return m_pOrigParent; }
	CString GetPathname() {return m_strPathName; }
	CMenu *GetMenu(int index);
	void LoadDocData();
	void SetBackDropID(UINT id) {m_myBackDropID = id;}
	UINT GetBackDropID() { return m_myBackDropID; }
	static BOOL CleanupExistingWindows();
	char *GetComicsTitle();
	void SetComicsTitle(char *title);
	void SetComicsTitle2(char *newTitle);
	CView *GetPrimaryView() { return m_view; }
	void RegisterNewContent();
	void SetObscured(BOOL bObscured);
	void SetFocusToSayWnd();
	int GetConnectionStatus() { return (m_proto->GetConnectionStatus()); }
	BOOL SaveModified() { return CDocObjectServerDoc::SaveModified(); } // was private
	CUserInfo *GetSingleSelectedMember();
	CUserInfo *GetNextSelectedMember(int &index);
	int SelectedMemberCount();
	void OnNotifyNewAvatar(LPCTSTR pszAvatarName);
	void CycleFocus(UINT nCurrentFocus, BOOL bBackward);
	CWnd* GetComponentWindow(UINT nComponent);
	HWND GetFocusSayOrEdit(BOOL bSayOnly = FALSE);
	HWND GetFocusSay()
		{ return GetFocusSayOrEdit (TRUE); }
	void SaveShortcut(LPCSTR pszPathName);
	// BOOL bEnableMenu(UINT uMain, UINT uSub, BOOL bEnable);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
	virtual void OnCloseDocument();
	protected:
	virtual COleServerItem* OnGetEmbeddedItem();
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CChatDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CChatDoc)
	afx_msg void OnUpdateEditUndo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditCopy(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditPaste(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditDelete(CCmdUI* pCmdUI);
	afx_msg void OnUpdateEditSelectAll(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnEditDelete();
	afx_msg void OnEditSelectAll();
	afx_msg void OnActionsSay();
	afx_msg void OnActionsThink();
	afx_msg void OnActionsWhisper();
	afx_msg void OnMemberGetinfo();
	afx_msg void OnMemberIgnore();
	afx_msg void OnAddToNotifs();
	afx_msg void OnUpdateMemberGetinfo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMemberIgnore(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAddToNotifs(CCmdUI* pCmdUI);
	afx_msg void OnFileCreateshortcut();
	afx_msg void OnFavoritesAddtofavorites();
	afx_msg void OnSendAction();
	afx_msg void OnUpdateViewComics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewText(CCmdUI* pCmdUI);
	afx_msg void OnAdministratorKick();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAdministratorKick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileCreateshortcut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFavoritesAddtofavorites(CCmdUI* pCmdUI);
	afx_msg void OnUpdateLeave(CCmdUI* pCmdUI);
	afx_msg void OnChannelprops();
	afx_msg void OnAdminBgrndsync();
	afx_msg void OnUpdateAdminBgrndsync(CCmdUI* pCmdUI);
	afx_msg void OnAdminBan();
	afx_msg void OnUpdateAdminBan(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChannelprops(CCmdUI* pCmdUI);
	afx_msg void OnMakespeaker();
	afx_msg void OnMakespectator();
	afx_msg void OnMakeadmin();
	afx_msg void OnUpdateMakeadmin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMakespeaker(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMakespectator(CCmdUI* pCmdUI);
	afx_msg void OnInvite();
	afx_msg void OnUpdateInvite(CCmdUI* pCmdUI);
	afx_msg void OnGetidentity();
	afx_msg void OnUpdateGetidentity(CCmdUI* pCmdUI);
	afx_msg void OnGetVersion();
	afx_msg void OnPlaySound();
	afx_msg void OnPingUser();
	afx_msg void OnGetLocaltime();
	afx_msg void OnWhisperboxMlist();
	afx_msg void OnUpdate1SelectionNotSelf(CCmdUI* pCmdUI);
	afx_msg void OnSetfont();
	afx_msg void OnSetColor();
	afx_msg void OnSwitchBold();
	afx_msg void OnSwitchItalic();
	afx_msg void OnSwitchUnderlined();
	afx_msg void OnSwitchFixedPitch();
	afx_msg void OnSwitchSymbol();
	afx_msg void OnUpdateFormat(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOnLine(CCmdUI* pCmdUI);
	afx_msg void OnSendEmail();
	afx_msg void OnVisitHomepage();
	afx_msg void OnStartNetmeeting();
	afx_msg void OnUpdateComicUserNotSelf(CCmdUI* pCmdUI);
	afx_msg void OnUpdateStartNetmeeting(CCmdUI* pCmdUI);
	afx_msg void OnSendFile();
	afx_msg void OnNewroom();
	afx_msg void OnClearHistory();
	afx_msg void OnUpdateVisitHomepage(CCmdUI* pCmdUI);
	afx_msg void OnGetComicCharacter();
	afx_msg void OnUpdateGetComicCharacter(CCmdUI* pCmdUI);
	afx_msg void OnFileClose();
	//}}AFX_MSG
	void InsertAdminMenu();
	void RemoveAdminMenu();

	public:
	afx_msg void OnViewComics();
	afx_msg void OnViewText();
	afx_msg void OnViewIcon();
	afx_msg void OnMacro(UINT);
	afx_msg void OnUpdateMacro(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewIcon(CCmdUI* pCmdUI);
	afx_msg void OnViewList();
	afx_msg void OnUpdateViewList(CCmdUI* pCmdUI);
	afx_msg void OnViewAutomations();
	afx_msg void OnViewOptions();
	afx_msg void OnLeave();

	void OnViewListAux();
	void UpdateAdminMenu();
	// void InsertMacroMenu();
	// void EnableMacroMenus(BOOL bEnableMacros);
	void UpdateMacroMenu();
	void UpdateComicCharacterMenu(CMenu* pMenu = NULL);
	void SetLegalPath(const char *base, BOOL bAddToMRU = TRUE);
	virtual void SetTitle(LPCTSTR lpszTitle);
	void SaveConnectStatus(CString &strStatus);
	void ResetStatus(BOOL left = TRUE, BOOL right = FALSE);
	void ChatSaveConversation(CArchive &ar);
	BOOL ChatLoadConversation(CArchive &ar);
	BOOL ChatSaveLocator(CArchive &f);
	static BOOL ChatLoadLocator(CArchive &f, BOOL bJoin, BOOL bException, SHORT *pnCXKeepServer);
	virtual void ReportSaveLoadException(LPCTSTR lpszPathName, CException* e, BOOL bSaving, UINT nIDPDefault);
	virtual void OnFrameWindowActivate( BOOL bActivate );

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FILE TYPES
#define FT_CCC	1
#define FT_CCR	2
#define FT_RTF	3

#define EX_DONTREPORT	99999
#define WM_LOGINDLG (WM_USER+1)

extern CView	*GetPrimaryView();
extern CChatDoc	*LookupDoc(const char *channel);


#endif // __CHATDOC_H__
