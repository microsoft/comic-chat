// AvatarFilerView.h : interface of the CAvatarFilerView class
//
/////////////////////////////////////////////////////////////////////////////

class CAvatarFilerView : public CView
{
protected: // create from serialization only
	CAvatarFilerView();
	DECLARE_DYNCREATE(CAvatarFilerView)

// Attributes
public:
	CAvatarFilerDoc* GetDocument();

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAvatarFilerView)
	public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CAvatarFilerView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CAvatarFilerView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in AvatarFilerView.cpp
inline CAvatarFilerDoc* CAvatarFilerView::GetDocument()
   { return (CAvatarFilerDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////
