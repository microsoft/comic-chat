// AvatarFilerDoc.h : interface of the CAvatarFilerDoc class
//
/////////////////////////////////////////////////////////////////////////////

class CAvatarFilerDoc : public CDocument
{
protected: // create from serialization only
	CAvatarFilerDoc();
	DECLARE_DYNCREATE(CAvatarFilerDoc)

// Attributes
public:
	CAvatarX *m_avatar;				// the one avatar that has been read in

// Operations
public:
	void LoadBinaryAvatar(FILE *fp, int avtype, int version);	// for binary avatars
	void LoadFaces(FILE *fp, int version, CAvatarComplex *av);
	void LoadTorsos(FILE *fp, int version, CAvatarComplex *av);
	void LoadBodies(FILE *fp, int version, CAvatarSimple *av);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvatarFilerDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual BOOL OnSaveDocument(LPCTSTR lpszPathName);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAvatarFilerDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	CString m_strPath;

// Generated message map functions
protected:
	//{{AFX_MSG(CAvatarFilerDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
