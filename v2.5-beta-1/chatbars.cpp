#include "stdafx.h"
#include "resource.h"
#include "chat.h"
#include "chatbars.h"
#include "tabbar.h"
#include "mainfrm.h"

// Private definitions

#include <pshpack1.h>
#define CBBSV_NEWLINE 1

struct COOLBARBANDSAVE
{
	void SetFromRebarBandInfo(REBARBANDINFO * prbbi)
		{
			ASSERT ((prbbi->fMask & (RBBIM_ID|RBBIM_SIZE|RBBIM_STYLE)) == (RBBIM_ID|RBBIM_SIZE|RBBIM_STYLE));
			wID = (WORD)prbbi->wID;
			wLength = (WORD)prbbi->cx;
			byFlags = (prbbi->fStyle & RBBS_BREAK) ? CBBSV_NEWLINE : 0;
		}
	void SetHidden(WORD id)
		{ wID = id; wLength = (WORD)-1; byFlags = 0; }
	void SetBlank() 
		{ wID = 0; wLength = 0; byFlags = 0; }
	WORD wID;
	WORD wLength;
	BYTE byFlags;
};
typedef struct COOLBARBANDSAVE * PCOOLBARBANDSAVE;

#include <poppack.h>

CCoolBarEx::CCoolBarEx()
{
	m_pnIDToolBars = NULL;
	m_nLastShown = 0;
	m_bOldVersion = !IsComCtlNewerThan (COMCTLVER(4, 71));
}

CCoolBarEx::~CCoolBarEx()
{
	// Delete all toolbars we created.
	POSITION pos = m_mapToolBars.GetStartPosition ();
	WORD w;
	CCoolToolBarEx * pToolBar;
	while (pos)
	{
		m_mapToolBars.GetNextAssoc (pos, w, (PVOID&)pToolBar);
		delete pToolBar;
	}
	if (m_pnIDToolBars != NULL)
	{
		delete[] m_pnIDToolBars;
	}
}

BOOL 
CCoolBarEx::Create(
CWnd* pParentWnd,
UINT* pnIDToolBars,
PBOOL pbVisibility,
DWORD dwStyle,
UINT nID)
{
	m_pnIDToolBarsTemp = pnIDToolBars;
	m_pbVisibilityTemp = pbVisibility;
	return CCoolBar::Create (pParentWnd, 
			WS_BORDER|WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
			RBS_BANDBORDERS|RBS_VARHEIGHT|RBS_DBLCLKTOGGLE, 
			dwStyle, nID);
}

BEGIN_MESSAGE_MAP(CCoolBarEx, CCoolBar)
	ON_WM_DESTROY()
	ON_WM_SYSCOLORCHANGE()
END_MESSAGE_MAP()				  

// Called to create bands. Creates each child toolbar.

BOOL 
CCoolBarEx::OnCreateBands()
{
	// Create and add toolbars. Strangely, this function, despite it's
	// apparent return value, has to return 0 for success and -1 for
	// failure.

	ASSERT (m_pnIDToolBarsTemp != NULL);
	UINT* pnIDToolBars = m_pnIDToolBarsTemp;
	PBOOL pbVisibility = m_pbVisibilityTemp;
	CCoolToolBarEx * pToolBar;
	TRY
	{
		while (*pnIDToolBars)
		{
			pToolBar = NULL;
			pToolBar = new CCoolToolBarEx;
			if (!pToolBar->Create (this, WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS|WS_CLIPCHILDREN|
					CBRS_TOOLTIPS|CBRS_FLYBY) || 
				!pToolBar->LoadToolBar (*pnIDToolBars))
			{
				::AfxThrowResourceException ();
			}
			pToolBar->ModifyStyle (0, TBSTYLE_FLAT);
			OnPrepareToolBar (*pnIDToolBars, pToolBar);
		   #ifdef DEBUG
		    PVOID pvToolBarPrev;
			ASSERT (!m_mapToolBars.Lookup (*pnIDToolBars, pvToolBarPrev));
		   #endif
			m_mapToolBars.SetAt (*pnIDToolBars, (PVOID)pToolBar);
			pToolBar->m_bVisible = *pbVisibility;
			pnIDToolBars++;
			pbVisibility++;
		}

		pToolBar = NULL;

		// Make a permanent copy of toolbar IDs in default order. This can
		// be used to restore default order.
		int nNumEntries = (int)(pnIDToolBars - m_pnIDToolBarsTemp);
		m_pnIDToolBars = new UINT[nNumEntries];
		memcpy (m_pnIDToolBars, m_pnIDToolBarsTemp, nNumEntries * sizeof(UINT));

		// Add the toolbars, in the default state.
		if (!AddToolBarBands ())
		{
			::AfxThrowUserException ();
		}
	}
	CATCH_ALL(e)
	{
		if (pToolBar != NULL)
		{
			delete pToolBar;
		}
		return (BOOL)-1;
	}
	END_CATCH_ALL

	m_pnIDToolBarsTemp = NULL; 		// This pointer was given to us temporarily
	m_pbVisibilityTemp = NULL;
	return (BOOL)0;
}

// Called immediately after each toolbar is created, before it is added to the coolbar.
// Override this to add/modify any items.

void 
CCoolBarEx::OnPrepareToolBar(
UINT nID,
CCoolToolBarEx * pToolBar)
{
}

void 
CCoolBarEx::OnDestroy()
{
	CCoolBar::OnDestroy ();
}

void 
CCoolBarEx::OnSysColorChange()
{
	CRebarBandInfo rbbi;
	rbbi.fMask = RBBIM_COLORS;
	rbbi.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbbi.clrBack = GetSysColor(COLOR_BTNFACE);

	int nCount = GetBandCount ();
	while (--nCount >= 0)
	{
		SetBandInfo (nCount, &rbbi);
	}

	CCoolBar::OnSysColorChange ();
}

// Adds a single toolbar bands to the end of the coolbar.

BOOL 
CCoolBarEx::AddSingleBand(
UINT nID,
CCoolToolBarEx* pToolBar,
int cx,
BOOL bBreakBefore)
{
	CSize szHorz, szVert;

	// Get minimum size of bands
	szHorz = pToolBar->CalcDynamicLayout (-1, 0);	// get min vert size
	szVert = pToolBar->CalcDynamicLayout (-1, LM_HORZ);	// get min vert size

	// For some odd reason, the old version of the coolbar and MFC don't work very well 
	// together. The first bar is always added at full size, and subsequent bars are
	// added at the minimum size. To get around this, we need to set the minimum bar 
	// width to the size before adding, and then change it back. (See RestoreBandMinSize)

	if (cx == -1)
	{
		cx = szVert.cx;
		bBreakBefore = FALSE;
	}
	else if (m_bOldVersion)
	{
		cx = szVert.cx;
	}

	CRebarBandInfo rbbi;
	rbbi.fMask = RBBIM_STYLE|RBBIM_CHILD|RBBIM_CHILDSIZE|RBBIM_COLORS|RBBIM_SIZE|RBBIM_ID;
	rbbi.fStyle = (bBreakBefore ? RBBS_BREAK : 0) | (pToolBar->m_bVisible ? 0 : RBBS_HIDDEN) |
		RBBS_CHILDEDGE | RBBS_GRIPPERALWAYS;
	rbbi.hwndChild = pToolBar->GetSafeHwnd ();
	rbbi.cxMinChild = m_bOldVersion ? cx : szHorz.cx - 6;
	rbbi.cyMinChild = szVert.cy;
	rbbi.cx = cx;
	rbbi.clrFore = GetSysColor(COLOR_BTNTEXT);
	rbbi.clrBack = GetSysColor(COLOR_BTNFACE);
	rbbi.wID = nID;

	return InsertBand(-1, &rbbi);
}

// Only needed for old version of toolbar. See comments in AddSingleBand below. This function
// restores the proper minimum size of the bar.

void
CCoolBarEx::RestoreBandMinSize(
UINT nID, 
CCoolToolBarEx* pToolBar)
{
	return;

	// Dead code
	#if 0
	int nBand = FindBand (nID);
	if (nBand == -1)		// Not in band!
		return;
	CSize szHorz = pToolBar->CalcDynamicLayout (-1, 0);	// get min vert size
	CSize szVert = pToolBar->CalcDynamicLayout (-1, LM_HORZ);	// get min vert size
	CRebarBandInfo rbbi;
	rbbi.fMask = RBBIM_CHILDSIZE;
	rbbi.cxMinChild = szHorz.cx - 6;
	rbbi.cyMinChild = szVert.cy;
	SetBandInfo (nBand, &rbbi);
    #endif
}

// Adds toolbar bands to the coolbar. Called when the bar is created, as well as
// when it is configured through LoadStateFromBuffer.

BOOL 
CCoolBarEx::AddToolBarBands(
int   nCount,
PVOID pvState)	// really a PCOOLBARBANDSAVE, can be NULL.
{
	PCOOLBARBANDSAVE pcbbs = (PCOOLBARBANDSAVE)pvState;

	if (pcbbs == NULL)
	{
		nCount = m_mapToolBars.GetCount ();
	}

	int i;
	UINT nID;
	int cx;
	BOOL bBreakBefore;
	CCoolToolBarEx* pToolBar;

	m_arrBandsNotAdded.RemoveAll ();

	for (i = 0; i < nCount; i++)
	{
		nID = (pcbbs != NULL) ? pcbbs[i].wID : m_pnIDToolBars[i];
		if (!m_mapToolBars.Lookup (nID, (PVOID&)pToolBar))
		{
			return FALSE;
		}

		// If we're using the old version, and the toolbar is not to be shown, 
		// don't even add it.

		if (m_bOldVersion && !pToolBar->m_bVisible)
		{
			m_arrBandsNotAdded.Add (nID);
			continue;
		}

		if (pcbbs != NULL)
		{
			cx = (int)pcbbs[i].wLength;
			bBreakBefore = (pcbbs[i].byFlags & CBBSV_NEWLINE) != 0;
		}
		else
		{
			cx = -1;
			bBreakBefore = FALSE;
		}

		if (!AddSingleBand (nID, pToolBar, cx, bBreakBefore))
		{
			 return FALSE;
		}

		if (pToolBar->m_bVisible)
		{
			m_nLastShown = nID;
		}

	}

	// For old version, we need to restore the minsizes of each band.
	if (m_bOldVersion)
	{
		for (i = 0; i < nCount; i++)
		{
			nID = (pcbbs != NULL) ? pcbbs[i].wID : m_pnIDToolBars[i];
			VERIFY (m_mapToolBars.Lookup (nID, (PVOID&)pToolBar));
			RestoreBandMinSize (nID, pToolBar);
		}
	}
	return TRUE;
}

// Saves the state of the coolbar to a buffer.

BOOL 
CCoolBarEx::SaveStateToBuffer(
PBYTE * pBufferOut, 
UINT * pOutBufferSize)
{
	int nCount = m_mapToolBars.GetCount ();
	int nActualCount = GetBandCount ();
	ASSERT (nCount - nActualCount == m_arrBandsNotAdded.GetSize ());
	if (nCount == 0)
	{
		*pBufferOut = NULL;
		*pOutBufferSize = 0;
		return TRUE;
	}

	PCOOLBARBANDSAVE pcbbs = NULL;
	int nAllocSize = sizeof(COOLBARBANDSAVE) * (nCount + 1);
	TRY
	{
		pcbbs = (PCOOLBARBANDSAVE)malloc (nAllocSize);
		if (pcbbs == NULL)
		{
			::AfxThrowMemoryException (); 
		}
		
		// Get all toolbars that are added.
		for (int i = 0; i < nActualCount; i++)
		{
			CRebarBandInfo rbbi;
			rbbi.fMask = RBBIM_ID | RBBIM_SIZE | RBBIM_STYLE;
			if (!GetBandInfo (i, &rbbi))
			{
				::AfxThrowUserException ();
			}
			pcbbs[i].SetFromRebarBandInfo (&rbbi);
		}

		// Get all remaining toolbars, which are not in the coolbar 
		// (compatibility with older coolbars which don't use RBBS_HIDDEN properly)
		while (i < nCount)
		{
			pcbbs[i].SetHidden (m_arrBandsNotAdded[i - nActualCount]);
			i++;
		}

		pcbbs[i].SetBlank ();
	}
	CATCH_ALL(e)
	{
		free (pcbbs);
		return FALSE;

	}
	END_CATCH_ALL
	*pBufferOut = (PBYTE)pcbbs;
	*pOutBufferSize = nAllocSize;
	return TRUE;
}

// Loads the state of the coolbar from a buffer created with 
// SaveStateToBuffer. The coolbar must have already been initialized,
// and must already contain all the bars in it. Passing in an empty
// buffer will simply leave the coolbar as is.

BOOL 
CCoolBarEx::LoadStateFromBuffer(
PBYTE pBuffer)
{
	PCOOLBARBANDSAVE pcbbs = (PCOOLBARBANDSAVE)pBuffer;
	int nCount = 0;
	if (pcbbs != NULL)
	{
		while (pcbbs[nCount].wID != 0)
			nCount++;
	}

	if (nCount == 0)
	{
		// Nothing to do.
		return FALSE;
	}

	// Version 4.71 of the Common Controls DLL provides a RB_MOVEBAND message
	// that would be tres cool. However, to remain compatible with older versions,
	// we delete them, and add them back in.

	int nBandCount = GetBandCount ();
	while (nBandCount-- > 0)
	{
		DeleteBand (0);
	}

	return AddToolBarBands (nCount, pcbbs);
}

// Finds a band in the buffer based on its ID. Version 4.71 of the Common Control
// DLL provides a message for this, but for older versions, we must use a slower
// brute force technique.

int 
CCoolBarEx::FindBand(
UINT nID)
{
	if (!m_bOldVersion)
	{
		return (int)SendMessage (RB_IDTOINDEX, (WPARAM)nID);
	}
	else
	{
		int i = GetBandCount ();
		while (--i >= 0)
		{
			CRebarBandInfo rbbi;
			rbbi.fMask = RBBIM_ID;
			if (GetBandInfo (i, &rbbi) && rbbi.wID == nID)
			{
				break;
			}
		}
		return i;
	}
}

void 
CCoolBarEx::ShowBar(
UINT nBarID, 
BOOL bShow)
{
	CCoolToolBarEx * pBar;
	if (!m_mapToolBars.Lookup (nBarID, (PVOID&)pBar))
	{
		return;
	}

	if (pBar->m_bVisible == bShow)
		return;

	int nBand = FindBand (nBarID);
	if (nBand != -1 || (m_bOldVersion && bShow))
	{
		// On newer versions, use RB_SHOWBAND to show and hide the band. On older
		// versions, we actually need to add/remove the band from the bar.
		pBar->m_bVisible = bShow;
		if (!m_bOldVersion)
		{
			SendMessage (RB_SHOWBAND, (WPARAM)nBand, bShow);
		}
		else
		{
			if (bShow)
			{
				// Add it back.
				if (!AddSingleBand (nBarID, pBar))
					return;
				if (m_bOldVersion)
					RestoreBandMinSize (nBarID, pBar); 
				for (int iFind = m_arrBandsNotAdded.GetUpperBound (); iFind >= 0; iFind--)
				{
					if (m_arrBandsNotAdded[iFind] == nBarID)
					{
						m_arrBandsNotAdded.RemoveAt (iFind);
						break;
					}
				}
			}
			else
			{
				SendMessage (RB_DELETEBAND, (WPARAM)nBand);
				m_arrBandsNotAdded.Add (nBarID);
			}
		}

		// This fixes a weird bug in the rebar control. If you hide all bars,
		// or initially have one visible bar, then show another, the first bar
		// does not get the size handle. To fix this, we keep track of the last
		// bar shown, and manually show it again.

		if (bShow)
		{
			if (m_nLastShown != 0 && m_nLastShown != nBarID && GetBandCount () == 2)
			{
				ShowBar (m_nLastShown, TRUE);
			}		
			m_nLastShown = nBarID;
		}
		else if (m_nLastShown == nBarID)
		{
			m_nLastShown = 0;
		}
	}
}

BOOL 
CCoolBarEx::IsBarShown(
UINT nBarID)
{
	CCoolToolBarEx * pBar;
	if (!m_mapToolBars.Lookup (nBarID, (PVOID&)pBar))
	{
		return FALSE;
	}
	return pBar->m_bVisible;
}

CCoolToolBarEx* 
CCoolBarEx::GetToolBarFromID(
UINT nBarID)
{
	CCoolToolBarEx* pToolBar = NULL;
	m_mapToolBars.Lookup (nBarID, (PVOID&)pToolBar);
	return pToolBar;
}

void 
CCoolToolBarEx::ModifyButtonStyle(
UINT  nID, 
DWORD dwAdd, 
DWORD dwRemove)
{
	int iButton = SendMessage (TB_COMMANDTOINDEX, nID);
	if (iButton != -1)
	{
		DWORD dwStyle = GetButtonStyle (iButton);
		dwStyle = (dwStyle & ~dwRemove) | dwAdd;
		SetButtonStyle (iButton, dwStyle);
	}
}

extern CChatApp theApp;

static UINT nIDs[] = { 0, 0, IDR_TEXTTOOLBAR, 0 };
static UINT nShowFlags[] = { SB_TOOLBAR_MAIN, SB_TOOLBAR_MEMBER, SB_TOOLBAR_TEXT, 0 };

BOOL 
CChatToolBar::Create(
CWnd* pParentWnd,
BOOL  bDoCB32)
{
	if (nIDs[0] == 0)
	{
#ifdef CB32SUPPORT
		nIDs[0] = bDoCB32 ? IDR_NM_MAIN : IDR_MAINFRAME;
		nIDs[1] = bDoCB32 ? IDR_NMUSERTOOLBAR : IDR_USERTOOLBAR;
#else
		nIDs[0] = IDR_MAINFRAME;
		nIDs[1] = IDR_USERTOOLBAR;
#endif

	}

	BOOL bVisibility[5];
	for (int i = 0; nIDs[i] != 0; i++)
	{
		bVisibility[i] = (theApp.m_iShowBars & nShowFlags[i]) != 0;
	}
	BOOL b = CCoolBarEx::Create (pParentWnd, nIDs, bVisibility);
	if (b)
	{
		// Load state from what's saved in registry.
		LoadStateFromBuffer (theApp.m_pbCoolBarState);
	}
	return b;
}

void 
CChatToolBar::OnPrepareToolBar(
UINT nID,
CCoolToolBarEx * pToolBar)
{
	switch (nID)
	{
		case IDR_NM_MAIN:
		case IDR_MAINFRAME:
			// Give the Favorites menu a dropdown, if available.
			pToolBar->SendMessage (TB_SETEXTENDEDSTYLE, 0, TBSTYLE_EX_DRAWDDARROWS);
			pToolBar->ModifyButtonStyle (ID_FAVORITES_OPENFAVORITES, TBSTYLE_DROPDOWN);
			pToolBar->ModifyButtonStyle (ID_VIEW_COMICS, TBSTYLE_CHECKGROUP|TBSTYLE_CHECK);
			pToolBar->ModifyButtonStyle (ID_VIEW_TEXT, TBSTYLE_CHECK);
			pToolBar->ModifyButtonStyle (ID_CHATROOM_LIST, TBSTYLE_GROUP);
			break;

		case IDR_NMUSERTOOLBAR:
		case IDR_USERTOOLBAR:
			pToolBar->ModifyButtonStyle (ID_AWAY_TOGGLE, TBSTYLE_CHECK);
			break;

		case IDR_TEXTTOOLBAR:
			pToolBar->ModifyButtonStyle (ID_SWITCHBOLD, TBSTYLE_CHECK);
			pToolBar->ModifyButtonStyle (ID_SWITCHITALIC, TBSTYLE_CHECK);
			pToolBar->ModifyButtonStyle (ID_SWITCHUNDERLINED, TBSTYLE_CHECK);
			pToolBar->ModifyButtonStyle (ID_SWITCHFIXEDPITCH, TBSTYLE_CHECK);
			pToolBar->ModifyButtonStyle (ID_SWITCHSYMBOL, TBSTYLE_CHECK);
			break;
	}
}

BEGIN_MESSAGE_MAP(CChatToolBar, CCoolBarEx)
	ON_WM_CONTEXTMENU()
END_MESSAGE_MAP()

void
CChatToolBar::ToggleBar(
UINT nWhich)
{
	UINT nWhatsShowing = theApp.m_iShowBars & SB_TOOLBAR_ANY;
	UINT nFlags = nWhich == CHAT_TOOLBAR_WHOLE ? SB_TOOLBAR_ANY : nShowFlags[nWhich];
	BOOL bShowing = (nWhatsShowing & nFlags) != 0;
	UINT nNewShowing;
	if (nWhich == CHAT_TOOLBAR_WHOLE)
	{
		nNewShowing = bShowing ? 0 : SB_TOOLBAR_ANY;
	}
	else
	{
		nNewShowing = nWhatsShowing ^ nFlags;
	}

	// Show or hide an individual bar.
	if (nWhich != CHAT_TOOLBAR_WHOLE)
	{
		ShowBar (nIDs[nWhich], !bShowing);
	}

	// May have to show or hide the whole coolbar.
	BOOL bDoWholeBar = nWhich == CHAT_TOOLBAR_WHOLE || nWhatsShowing == 0 || nNewShowing == 0;
	if (bDoWholeBar)
	{
		// May need a little work to make the tabbar look right.
		CFrameWnd* pMainWnd = (CFrameWnd*)::AfxGetMainWnd ();
		if (pMainWnd->IsKindOf (RUNTIME_CLASS(CMainFrame)))
		{
			CControlBar * pDockBar = pMainWnd->GetControlBar (AFX_IDW_DOCKBAR_TOP);
			if (pDockBar != NULL)
			{
				DWORD dwStyle = pDockBar->GetBarStyle ();
				if (nNewShowing != 0) // Need to show it.
				{
					dwStyle &= ~CBRS_BORDER_ANY;
				}
				else
				{
					dwStyle |= CBRS_BORDER_TOP;
				}
				pDockBar->SetBarStyle (dwStyle);
			}
		}
		pMainWnd->ShowControlBar (this, nNewShowing != 0, FALSE);
	}

	theApp.m_iShowBars = (theApp.m_iShowBars & ~SB_TOOLBAR_ANY) | nNewShowing;
}

void 
CChatToolBar::OnContextMenu(
CWnd* pWnd, 
CPoint pt)
{
	if (pt.x == -1 && pt.y == -1)
	{
		GetCursorPos (&pt);
	}
	CMenu menu;
	if (menu.LoadMenu (IDR_TOOLBARCONTEXT))
		menu.GetSubMenu (0)->TrackPopupMenu (TPM_LEFTALIGN | TPM_RIGHTBUTTON,
			pt.x, pt.y, ::AfxGetMainWnd ());
}
