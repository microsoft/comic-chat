// spltchat.h : interface of the CSplitChat class
//
/////////////////////////////////////////////////////////////////////////////

#ifndef __SPLTCHAT_H__
#define __SPLTCHAT_H__

class CSplitChat : public CSplitterWnd
{
public:
	CSplitChat();

	virtual BOOL CreateView( int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext );


protected:
	virtual void StopTracking(BOOL bAccept);

	int m_nPctBottom;
	int m_nViewsCreated;

	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////////////////////////
class CSplitChatV : public CSplitterWnd
{
public:
	CSplitChatV();

	virtual	BOOL CreateStatic( CWnd* pParentWnd, int nRows, int nCols, 
							   DWORD dwStyle = WS_CHILD | WS_VISIBLE, UINT nID = AFX_IDW_PANE_FIRST );
	virtual void AssertValid() {}	// override default check -- can have a 0 column, 0 row control

protected:
	virtual void StopTracking(BOOL bAccept);

	int m_nPctLeft;
	BOOL m_bViewsCreated;

	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
class CSplitSay : public CSplitterWnd
{
public:
	CSplitSay();

	virtual BOOL CreateView( int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext );
	BOOL ReadyToSize() { return (m_bViewsCreated); }

protected:

	virtual void RecalcLayout();
	virtual void SayRecalcLayout();

	virtual void StopTracking(BOOL bAccept);

	int m_nPixelsClient;

	int m_cy;

	int	m_nPixelsLogTracked;
	int m_nPixelsSayMin;

	BOOL m_bViewsCreated;
	BOOL m_bInitialSizingDone;

	afx_msg void OnSize( UINT nType, int cx, int cy );
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags); 
	DECLARE_MESSAGE_MAP()
};
#endif  // __SPLTCHAT_H__
