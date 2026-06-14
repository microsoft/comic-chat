// AvatarFilerView.cpp : implementation of the CAvatarFilerView class
//

#include "stdafx.h"
#include "AvatarFiler.h"

#include "pseudoAvatar.h"

#include "AvatarFilerDoc.h"
#include "AvatarFilerView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerView

IMPLEMENT_DYNCREATE(CAvatarFilerView, CView)

BEGIN_MESSAGE_MAP(CAvatarFilerView, CView)
	//{{AFX_MSG_MAP(CAvatarFilerView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerView construction/destruction

CAvatarFilerView::CAvatarFilerView()
{
	// TODO: add construction code here

}

CAvatarFilerView::~CAvatarFilerView()
{
}

BOOL CAvatarFilerView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerView drawing

void CAvatarFilerView::OnDraw(CDC* pDC)
{
	CAvatarFilerDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
}

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerView printing

BOOL CAvatarFilerView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}

void CAvatarFilerView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add extra initialization before printing
}

void CAvatarFilerView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
	// TODO: add cleanup after printing
}

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerView diagnostics

#ifdef _DEBUG
void CAvatarFilerView::AssertValid() const
{
	CView::AssertValid();
}

void CAvatarFilerView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CAvatarFilerDoc* CAvatarFilerView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CAvatarFilerDoc)));
	return (CAvatarFilerDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CAvatarFilerView message handlers
