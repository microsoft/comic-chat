// ChatItem.h : interface of the CChatItem class
//

class CChatItem : public CDocObjectServerItem
{
	DECLARE_DYNAMIC(CChatItem)

// Constructors
public:
	CChatItem(CChatDoc* pContainerDoc);

// Attributes
	CChatDoc* GetDocument() const
		{ return (CChatDoc*)CDocObjectServerItem::GetDocument(); }

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CChatItem)
	public:
	virtual BOOL OnDraw(CDC* pDC, CSize& rSize);
	virtual BOOL OnGetExtent(DVASPECT dwDrawAspect, CSize& rSize);
	//}}AFX_VIRTUAL

// Implementation
public:
	~CChatItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	virtual void Serialize(CArchive& ar);   // overridden for document i/o
};

/////////////////////////////////////////////////////////////////////////////
