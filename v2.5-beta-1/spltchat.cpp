// spltchat.cpp : implementation of the CSplitChat class
//
/////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "chat.h"			// for theApp
#include "saywnd.h"
#include "spltchat.h"
#include "ui.h"
#include <afxpriv.h>

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// implementation for memberpane/bodycam splitter

BEGIN_MESSAGE_MAP(CSplitChat, CSplitterWnd)
	ON_WM_SIZE()
	ON_WM_CHAR()
END_MESSAGE_MAP()

CSplitChat::CSplitChat() : CSplitterWnd()
{
	m_nPctBottom =70;   // initial percentage of bodycam
	m_nViewsCreated = 0;
};

void CSplitChat::StopTracking( BOOL bAccept )
{
	// Must call base class first to finish splitter bar move.
	CSplitterWnd::StopTracking( bAccept );

	if (bAccept)
	{
		CRect rect;
		int cxBottom, cxBottomMin;


		// Get size of client rect.
		GetClientRect(&rect);
		GetRowInfo( 1, cxBottom, cxBottomMin );
		if (!rect.IsRectEmpty())
		{
		    // Compute the new percentage based on current location of client rect used by splitter.
			m_nPctBottom = cxBottom * 100 / (rect.bottom - rect.top); 
		}
	}
}

void CSplitChat::OnSize( UINT nType, int cx, int cy )
{
	if (m_nViewsCreated == 2)
	{
		int iNewPaneHeight;
		CRect rect;

		// OK, the parent window is telling us it has resized so we need to split the two rows up as defined
		// by the horizontal splitter percentage. This will keep the percentage constant.
		GetClientRect(&rect);

		iNewPaneHeight = (rect.Height() * (100 - m_nPctBottom)) / 100;
		SetRowInfo( 0, iNewPaneHeight, 10 );

		iNewPaneHeight = (rect.Height() * m_nPctBottom) / 100;
		SetRowInfo( 1, iNewPaneHeight, 10 );

//		RecalcLayout();  // subsequent OnSize does this for us
	}
	CSplitterWnd::OnSize( nType, cx, cy );
}

BOOL CSplitChat::CreateView( int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext )
{
	// Need to know when the views have been created so we don't try and set column info before hand in OnSize()
	// (called as a result of CreateStatic()).
	m_nViewsCreated++;

	// Just call the base class.
	return CSplitterWnd::CreateView( row, col, pViewClass, sizeInit, pContext );
}

void CSplitChat::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	LPARAM lparam;
	lparam = nFlags << 16;
	lparam |= (WORD)nRepCnt;

	ASSERT(GetSay());

	HWND hSayEdit = GetSay()->GetSayEdit();
	CSayCtrl* pSayCtrl = ((CSayCtrl*)FromHandle(hSayEdit));
	pSayCtrl->SendMessage(WM_CHAR,nChar,lparam);
	GetSay()->SetFocusToSayWnd();
}
//////////////////////////////////////////////////////////////////////////////////////////////
// Implementation of Vertical Splitter

BEGIN_MESSAGE_MAP(CSplitChatV, CSplitterWnd)
	ON_WM_SIZE()
	ON_WM_CHAR()
END_MESSAGE_MAP()

CSplitChatV::CSplitChatV() : CSplitterWnd()
{
	m_nPctLeft = (!theApp.m_bShowMode) ? 80 : 100; // initial left side size of 80% (100% for show)
	m_bViewsCreated = FALSE;
};

void CSplitChatV::StopTracking( BOOL bAccept )
{
	// Must call base class first to finish splitter bar move.
	CSplitterWnd::StopTracking( bAccept );

	if (bAccept)
	{
		CRect rect;
		int cxLeft, cxLeftMin;


		// Get size of client rect.
		GetClientRect(&rect);
		GetColumnInfo( 0, cxLeft, cxLeftMin );
		if (!rect.IsRectEmpty())
		{
		    // Compute the new percentage based on current location of client rect used by left splitter.
			m_nPctLeft = cxLeft * 100 / (rect.right - rect.left); 
		}
	}
}

void CSplitChatV::OnSize( UINT nType, int cx, int cy )
{
	if (m_bViewsCreated)
	{
		int iNewPaneWidth;
		CRect rect;

		// OK, the parent window is telling us it has resized so we need to split the two columns up as defined
		// by the left splitter percentage. This will keep the percentage constant.
		GetClientRect(&rect);

		iNewPaneWidth = (rect.Width() * m_nPctLeft) / 100;
		SetColumnInfo( 0, iNewPaneWidth, 10 );
		
		iNewPaneWidth = (rect.Width() * (100 - m_nPctLeft)) / 100;
		SetColumnInfo( 1, iNewPaneWidth, 10 );

//		RecalcLayout();  // subsequent OnSize does this for us

		CSplitterWnd::OnSize( nType, cx, cy );
	}
}

BOOL CSplitChatV::CreateStatic( CWnd* pParentWnd, int nRows, int nCols, 
							   DWORD dwStyle, UINT nID)
{

	// yeah, overriding this is a bad thing since it isnt virtual, but
	// we have to check if its been called before we do a SetColumnInfo in 
	// OnSize, so there is no way around it.
	BOOL bRet = CSplitterWnd::CreateStatic(pParentWnd,nRows,nCols,dwStyle,nID);
	m_bViewsCreated = TRUE;
	return bRet;

}


void CSplitChatV::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	LPARAM lparam;
	lparam = nFlags << 16;
	lparam |= (WORD)nRepCnt;

	ASSERT(GetSay());

	HWND hSayEdit = GetSay()->GetSayEdit();
	CSayCtrl* pSayCtrl = ((CSayCtrl*)FromHandle(hSayEdit));
	pSayCtrl->SendMessage(WM_CHAR,nChar,lparam);
	GetSay()->SetFocusToSayWnd();
}
  
/////////////////////////////////////////////////////////////////////////////
// implementation for pageview/saywnd splitter

// Initial window percentages/sizes.
#define NPIXELSSAYMIN	23

BEGIN_MESSAGE_MAP(CSplitSay, CSplitterWnd)
	ON_WM_SIZE()
	ON_WM_CHAR()
END_MESSAGE_MAP()

CSplitSay::CSplitSay() : CSplitterWnd()
{
	m_bViewsCreated = FALSE;
	m_bInitialSizingDone = FALSE;
	m_nPixelsSayMin = (!theApp.m_bShowMode) ? NPIXELSSAYMIN : 0;
};

void CSplitSay::StopTracking( BOOL bAccept )
{
	if (bAccept)
	{
		int cyClient, cyClientPrev, cySay, cyClientMin, cySayMin;

		// Before we call the base class, which will perform all standard MFC calculations
		// for the splitter bar move and call out overriden RecalcLayout, we will store the 
		// current pane heights so we can know what has changed.
		GetRowInfo( 0, cyClientPrev, cyClientMin);
		ASSERT(cyClientMin == 0);

		// Must call base class first to finish splitter bar move.
		CSplitterWnd::StopTracking( TRUE );

		// Only real requirement is to ensure say pane can't be reduced below it's minimum.
		GetRowInfo( 0, cyClient, cyClientMin);
		GetRowInfo( 1, cySay, cySayMin);
		ASSERT(cyClientMin == 0);
		ASSERT(cySayMin == 0);

		if (cySay < m_nPixelsSayMin)
		{
			// Reducing pane to zero will mean that base class StopTracking will expect
			// to remove border around say pane and add the border height to the client.
			if (cySay == 0) 
			{
				// Take the difference from the client pane.
				cyClient = cyClient - m_cyBorder - m_nPixelsSayMin;
			}
			else
			{
				// Take the difference from the client pane.
				cyClient = cyClient - (m_nPixelsSayMin - cySay);
			}
		}
		SetRowInfo(0, cyClient, 0);

		// Update pixel heights for registry. (not being used by Comic_chat right not)
		m_nPixelsClient = cyClient;

		// Redraw the splitter
		SayRecalcLayout();
	}
	else
	{
		// Must call base class first to finish splitter bar move.
		CSplitterWnd::StopTracking( FALSE );
	}
}

void CSplitSay::OnSize( UINT nType, int cx, int cy )
{
//	if (m_pRowInfo == NULL) return;  // nothing to do... 

	if (m_bViewsCreated)
	{
		if (!m_bInitialSizingDone)
		{
			// Right we should only get here if the parent window is sizing for the first time.
			// Pixel figures should be correct (i.e. client pixels leave room for min
			// say height).
			CRect rect;
			GetClientRect(&rect);
			// need to account for minimun say wnd + 2 borders + splitter
			SetRowInfo( 0,rect.bottom-rect.top-m_nPixelsSayMin-m_cySplitter-m_cyBorder-m_cyBorder, 0);

			// And redraw the splitter.
			SayRecalcLayout();

			// Only implement this logic at start-up to implement registry stored figures. (not used by Comic Chat)
			m_bInitialSizingDone = TRUE;

			// Store size to determine if we're shrinking or growing next call.
			m_cy = cy;
		}
		else
		{
			// When parent is being sized, maintain height of say wnd.
			int cyClient, cySay, cyClientMin, cySayMin, iPixelsShrunk;

			GetRowInfo( 0, cyClient, cyClientMin);
			GetRowInfo( 1, cySay, cySayMin);
			ASSERT(cyClientMin == 0);
			ASSERT(cySayMin == 0);

			iPixelsShrunk = m_cy - cy;
			if (iPixelsShrunk > 0)
			{	
				// The height of client rect is shrinking, we must reduce height of panes in 
				// priority order. First client, then say.

				// Is there enough client height to take all the shrinkage?
				if (cyClient >= iPixelsShrunk) 
				{
					cyClient -= iPixelsShrunk;
				}
				SetRowInfo( 0, cyClient, 0);

				// Update pixel heights for registry. (not used for Comic Chat)
				m_nPixelsClient = cyClient;
			}
			else if (iPixelsShrunk < 0)
			{
				// The height of client rect is growing. We must expand the height of panes
				// in priority order. First say up to it's minimum (if it's below it's minimum)
				// then client.


				// Can say take all growth?.
				if ((m_nPixelsSayMin - cySay) >= (-iPixelsShrunk))
				{
					// Under most conditions these will be zero. However, when starting maximized
					// we display window initialy in it's restored state (hidden) and set the 
					// clients then, which can leave the say pane below minimum. In this
					// circumstance we should not zero the log and client panes, and in all
					// other circumstances they will be zero anyway!
					cyClient = 0;
				}
				else
				{
					// Don't grow say once it's at it's minimum.
					if (cySay < m_nPixelsSayMin)
					{
					// Client can take all (remaining) growth.
					cyClient += ((-iPixelsShrunk) - (m_nPixelsSayMin - cySay));

					// If the say pane is being expanded from zero it will mean that 
					// some of the total expanded height will be taken by the addition
					// of the gap around the say pane. This height must be taken from
					// the client.
						if (cySay == 0) 
						{
						// Take the difference from the client pane.
								cyClient -= m_cySplitterGap;
						}
					}
					else
						cyClient += -iPixelsShrunk;
				}
				SetRowInfo( 0, cyClient, 0);

				// Update pixel heights for registry. (not used for comic chat)
				m_nPixelsClient = cyClient;
			}
			// Update size to determine if we're shrinking or growing next call.
			m_cy = cy;
		}
		// Redraw the splitter.
		SayRecalcLayout();
	}
	else
	{
		// Call base class since we've nothing to do ourselves.
		CSplitterWnd::OnSize( nType, cx, cy );
	}
}

BOOL CSplitSay::CreateView( int row, int col, CRuntimeClass* pViewClass, SIZE sizeInit, CCreateContext* pContext )
{
	// Need to know when the views have been created so we don't try and set column info before hand in OnSize()
	// (called as a result of CreateStatic()).
	m_bViewsCreated = TRUE;

	// Just call the base class.
	return CSplitterWnd::CreateView( row, col, pViewClass, sizeInit, pContext );
}

void CSplitSay::RecalcLayout()
{
	// This override is designed to be called only via the base classes (e.g. CSplitterWnd::StopTracking). 
	// It will not perform any drawing operations but will simply recalculate the heights of the panes
	// for use in further calculations.

	if (m_pRowInfo == NULL) return;

	CRect rectInside;
	GetInsideRect(rectInside);

	// Layout rows (restrict to possible sizes).

	CSplitterWnd::CRowColInfo* pInfoArray = m_pRowInfo;
	int nMax = m_nRows;
	int nSize = rectInside.Height();
	int nSizeSplitter = m_cySplitterGap;
	
	ASSERT(pInfoArray != NULL);
	ASSERT(nMax > 0);
	ASSERT(nSizeSplitter > 0);
																		  
	CSplitterWnd::CRowColInfo* pInfo;
	int i;

	if (nSize < 0)
		nSize = 0;  // if really too small, layout as zero size

	// start with ideal sizes
	for (i = 0, pInfo = pInfoArray; i < nMax-1; i++, pInfo++)
	{
		if (pInfo->nIdealSize < pInfo->nMinSize)
			pInfo->nIdealSize = 0;      // too small to see
		pInfo->nCurSize = pInfo->nIdealSize;
	}
	pInfo->nCurSize = INT_MAX;  // last row/column takes the rest

	for (i = 0, pInfo = pInfoArray; i < nMax; i++, pInfo++)
	{
		ASSERT(nSize >= 0);
		if (nSize == 0)
		{
			// no more room (set pane to be invisible)
			pInfo->nCurSize = 0;
			continue;       // don't worry about splitters
		}
		else if (nSize < pInfo->nMinSize && i != 0)
		{
			// additional panes below the recommended minimum size
			//   aren't shown and the size goes to the previous pane
			pInfo->nCurSize = 0;

			// previous pane already has room for splitter + border
			//   add remaining size and remove the extra border
			ASSERT(m_cxBorder == m_cyBorder);
			(pInfo-1)->nCurSize += nSize + m_cxBorder;
			nSize = 0;
		}
		else
		{
			// otherwise we can add the second pane
			ASSERT(nSize > 0);
			if (pInfo->nCurSize == 0)
			{
				// too small to see
				if (i != 0)
					pInfo->nCurSize = 0;
			}
			else if (nSize < pInfo->nCurSize)
			{
				// this row/col won't fit completely - make as small as possible
				pInfo->nCurSize = nSize;
				nSize = 0;
			}
			else
			{
				// can fit everything
				nSize -= pInfo->nCurSize;
			}
		}

		// see if we should add a splitter
		ASSERT(nSize >= 0);
		if (i != nMax - 1)
		{
			// should have a splitter
			if (nSize > nSizeSplitter)
			{
				nSize -= nSizeSplitter; // leave room for splitter + border
				ASSERT(nSize > 0);
			}
			else
			{
				// not enough room - add left over less splitter size
				ASSERT(m_cxBorder == m_cyBorder);
				pInfo->nCurSize += nSize;
				if (pInfo->nCurSize > (nSizeSplitter - m_cxBorder))
					pInfo->nCurSize -= (nSizeSplitter - m_cyBorder);
				nSize = 0;
			}
		}
	}
	ASSERT(nSize == 0); // all space should be allocated*/
}

void CSplitSay::SayRecalcLayout()
{
	// Just call base class.
	CSplitterWnd::RecalcLayout();
}

void CSplitSay::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	LPARAM lparam;
	lparam = nFlags << 16;
	lparam |= (WORD)nRepCnt;

	ASSERT(GetSay());

	HWND hSayEdit = GetSay()->GetSayEdit();
	CSayCtrl* pSayCtrl = ((CSayCtrl*)FromHandle(hSayEdit));
	pSayCtrl->SendMessage(WM_CHAR,nChar,lparam);
	GetSay()->SetFocusToSayWnd();
}
