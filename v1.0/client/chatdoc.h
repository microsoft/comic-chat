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
	CPtrList m_pages;
	CPtrList m_history;
	RECT m_bbox;
	char *m_comicsTitle;
	char m_fileType;		// current file type for saving and opening
	BOOL m_bEmbedded;
	BOOL m_bPrompt;
	BOOL m_bIconMembers;
	BOOL m_bEnableFileMenu;
	CMenu* m_pmenuAdmin;
	BOOL m_bLastMemberView;

	// System generated function
	CChatItem* GetEmbeddedItem()
		{ return (CChatItem*)COleServerDoc::GetEmbeddedItem(); }

// Operations
public:
	void AddLine(UINT id, const char *, UCHAR mode, CPtrArray *links = NULL);
	void ProcessLine(UINT id, const char *line, UCHAR mode, BOOL cooked, CPtrArray *links = NULL);
	void TallySpeech(UINT id);
	void ShowInfo(void *, const char *);				// show an info box for a character
	void InitMyDocument();
	void AddNewPage();
	void DestroyHistory();
	void ExecuteHistory(int mode);
	void DestroyPages();
	void ReadSampleConversation(const char *);
	BOOL ParseLocatorFile(FILE *fp);
	int FindFileType(const char *);
	BOOL DoSave(LPCTSTR lpszPathName, BOOL bReplace);
	static CChatDoc * GetDoc();
	COleIPFrameWnd* CreateInPlaceFrame(CWnd* pParentWnd);
	CString GetDesktopOrFavorites(BOOL bDesktop);
	void RegenerateView();
	BOOL CChatDoc::SaveDocDataToReg();
	BOOL CChatDoc::LoadDocDataFromReg();
	CWnd *GetOrigParent() { return m_pOrigParent; }
	CString GetPathname() {return m_strPathName; }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	virtual void DeleteContents();
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
	afx_msg void OnPanels3wide();
	afx_msg void OnUpdatePanels3wide(CCmdUI* pCmdUI);
	afx_msg void OnPanels1wide();
	afx_msg void OnUpdatePanels1wide(CCmdUI* pCmdUI);
	afx_msg void OnPanels2wide();
	afx_msg void OnPanels4wide();
	afx_msg void OnUpdatePanels4wide(CCmdUI* pCmdUI);
	afx_msg void OnPanels5wide();
	afx_msg void OnUpdatePanels5wide(CCmdUI* pCmdUI);
	afx_msg void OnEditCopy();
	afx_msg void OnEditCut();
	afx_msg void OnEditPaste();
	afx_msg void OnEditUndo();
	afx_msg void OnEditDelete();
	afx_msg void OnEditSelectAll();
	afx_msg void OnSendComicsData();
	afx_msg void OnUpdateSendComicsData(CCmdUI* pCmdUI);
	afx_msg void OnAppCharacterSel();
	afx_msg void OnActionsSay();
	afx_msg void OnActionsThink();
	afx_msg void OnActionsWhisper();
	afx_msg void OnOptionsProfile();
	afx_msg void OnUpdatePanels2wide(CCmdUI* pCmdUI);
	afx_msg void OnUpdateOptionsPromptforsave(CCmdUI* pCmdUI);
	afx_msg void OnOptionsPromptforsave();
	afx_msg void OnMemberGetinfo();
	afx_msg void OnMemberIgnore();
	afx_msg void OnUpdateMemberGetinfo(CCmdUI* pCmdUI);
	afx_msg void OnUpdateMemberIgnore(CCmdUI* pCmdUI);
	afx_msg void OnFileCreateshortcut();
	afx_msg void OnFavoritesAddtofavorites();
	afx_msg void OnFavoritesOpenfavorites();
	afx_msg void OnSendAction();
	afx_msg void OnUpdateDisconnect(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAppCharacterSel(CCmdUI* pCmdUI);
	afx_msg void OnViewComics();
	afx_msg void OnViewText();
	afx_msg void OnUpdateViewComics(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewText(CCmdUI* pCmdUI);
	afx_msg void OnAdministratorKick();
	afx_msg void OnAdministratorTopic();
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateChatroomList(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAdministratorKick(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrint(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileCreateshortcut(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFavoritesAddtofavorites(CCmdUI* pCmdUI);
	afx_msg void OnUpdateViewOptions(CCmdUI* pCmdUI);
	afx_msg void OnNewroom();
	afx_msg void OnUpdateNewroom(CCmdUI* pCmdUI);
	afx_msg void OnLeave();
	afx_msg void OnUpdateLeave(CCmdUI* pCmdUI);
	afx_msg void OnConnect();
	afx_msg void OnUpdateConnect(CCmdUI* pCmdUI);
	afx_msg void OnDisconnect();
	afx_msg void OnMakeAdmin();
	afx_msg void OnUpdateMakeAdmin(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRemoveAdmin(CCmdUI* pCmdUI);
	//}}AFX_MSG
	public:
	afx_msg void OnViewIcon();
	afx_msg void OnUpdateViewIcon(CCmdUI* pCmdUI);
	afx_msg void OnViewList();
	afx_msg void OnUpdateViewList(CCmdUI* pCmdUI);
	afx_msg void OnViewOptions();
	afx_msg void OnChatroomList();
	void InsertAdminMenu();
	void RemoveAdminMenu();

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// FILE TYPES
#define FT_CCC	1
#define FT_CCR	2
