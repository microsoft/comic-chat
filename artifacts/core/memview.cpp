/*
	*****************************************************************
	*																*
	*	Module 			: MemView.cpp, MemView for Microsoft Chat	*
	*																*
	*	Author 			: RegisB, 11/4/96							*
	*																*
	*	Current Owner	: RegisB									*
	*																*
	*	Changes			:											*
	*																*
	*	To Dos			:											*
	*																*
	*****************************************************************
*/


#include "MemView.H"
#include "CDebug.H"
#include "WinError.H"
#include "CCommon.H"
#include "Dib.H"

// for ASSERT and FAIL
//
SZTHISFILE


CMemView::CMemView()
{
	// Initialize the defaults;
	m_hwnd				= NULL;
	m_hInstance			= NULL;
	m_hFont				= NULL;
	m_bInternalCreation = FALSE;
	m_pfnIsEqual		= NULL;
	m_pfnDelete			= NULL;
	m_pfnCompare		= NULL;
	m_dwThisUser		= 0L;
}


CMemView::~CMemView()
{
	if (m_bInternalCreation)
	{
		::DestroyWindow(m_hwnd);
		m_hwnd = NULL;
	}

	if (m_hwnd)		// We don't own this window, but have it destroy it also
	{
		ASSERT(m_hwnd != NULL, "m_hwnd is valid in CMemView::~CMemView");
		::DestroyWindow(m_hwnd);
	}

	if (m_hFont)
		::DeleteObject(m_hFont);
}


BOOL CMemView::bCreateMemViewWindow(INT iWndID, TCHAR *szName, DWORD dwExStyle, DWORD dwStyle, 
									  INT x, INT y, INT nWidth, INT nHeight, 
									  HWND hwndParent, HINSTANCE hInstance)
{
	if (m_hwnd)
	{
		::SetLastError(ERROR_GEN_FAILURE);
		return (FALSE);
	}

	// Ensure that the common window Dll is loaded
	InitCommonControls();

	HWND hwnd = CreateWindowEx(dwExStyle,
							WC_LISTVIEW,						// List view class 
							szName, 
							dwStyle|MEM_VIEW_WND_FLAGS, 
							x, y, 
							nWidth, nHeight, 
							hwndParent, 
							(HMENU) iWndID, 
							hInstance, 
							NULL);

	if (hwnd)
	{
 		m_bInternalCreation = TRUE;
		return (AttachMemViewHWnd(hwnd, hInstance));
	}
	else
		return (FALSE);
}


BOOL CMemView::bResetMemViewImageList(BOOL bSmallImages)
{
	HIMAGELIST	hImageList;	// handle to image list

	ASSERT(m_hwnd, "m_hwnd <= 0 in CMemView::bResetMemViewImageList");

	// Try to get the image list already associated with the listview
	hImageList = ListView_GetImageList(m_hwnd, bSmallImages);
	if (hImageList)
	{
		ListView_SetImageList(m_hwnd, NULL, bSmallImages ? LVSIL_SMALL : LVSIL_NORMAL);
		return ImageList_Destroy(hImageList);
	}
	else
		return TRUE;
}


BOOL CMemView::bSetMemViewImageList(BOOL bSmallImages, INT cx, INT cy, INT cImageNum, INT cMaxImages, UINT uFirstResID)
{
	INT			iImage; 
	UINT		uFormat;
	HICON		hIcon;
	HIMAGELIST	hImageList;	// handle to image list

	ASSERT(cx > 0, "cx <= 0 in CMemView::bSetMemViewImageList");
	ASSERT(cy > 0, "cy <= 0 in CMemView::bSetMemViewImageList");
	ASSERT(cImageNum > 0, "cImageNum <= 0 in CMemView::bSetMemViewImageList");
	ASSERT(m_hInstance, "m_hInstance <= 0 in CMemView::bSetMemViewImageList");
	ASSERT(m_hwnd, "m_hwnd <= 0 in CMemView::bSetMemViewImageList");

	// Try to get the image list already associated with the listview
	hImageList = ListView_GetImageList(m_hwnd, bSmallImages);
	if (hImageList)
	{
		::SetLastError(ERROR_GEN_FAILURE);		// call bResetMemViewImageList first
		return FALSE;
	}

	// Create a new image list
	hImageList = ImageList_Create(cx, cy, ILC_COLOR,  cImageNum, cMaxImages-cImageNum);
	
	if (!hImageList)
		return FALSE;

	for (iImage = 0; iImage < cImageNum; iImage++)
	{
		if ((hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(uFirstResID+iImage))) == NULL)
			return FALSE;
		if (-1 == ImageList_AddIcon(hImageList, hIcon))
			return FALSE;
	}

	// Make sure that all of the icons are in the list
	if (ImageList_GetImageCount(hImageList) < cImageNum)
	{
		::SetLastError(ERROR_GEN_FAILURE);
		return FALSE;
	}

	// Associate list with listview window
	ListView_SetImageList(m_hwnd, hImageList, bSmallImages ? LVSIL_SMALL : LVSIL_NORMAL);

	return TRUE;
}


INT CMemView::iAddMemViewImage(BOOL bSmallImage, UINT uIconResID, UINT uOverlayIconResID1, UINT uOverlayIconResID2)
{
	HICON		hIcon = NULL;
	HIMAGELIST	hImageList = NULL;	// handle to image list

	ASSERT(m_hInstance, "m_hInstance <= 0 in CMemView::iAddMemViewImage");
	ASSERT(m_hwnd, "m_hwnd <= 0 in CMemView::iAddMemViewImage");

	if ((hImageList = ListView_GetImageList(m_hwnd, bSmallImage)) == NULL)
		return -1;

	if (!uOverlayIconResID1 && !uOverlayIconResID2)
	{
		if ((hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(uIconResID))) == NULL)
			return -1;
	}
	else
	{
		if ((hIcon = HGetIconFromResourceIDs(bSmallImage, uIconResID, uOverlayIconResID1, uOverlayIconResID2)) == NULL)
			return -1;
	}

	return ImageList_AddIcon(hImageList, hIcon);
}


INT CMemView::iReplaceMemViewImage(INT iImage, BOOL bSmallImage, UINT uIconResID, UINT uOverlayIconResID1, UINT uOverlayIconResID2)
{
	HICON		hIcon = NULL;
	HIMAGELIST	hImageList = NULL;	// handle to image list

	ASSERT(m_hInstance, "m_hInstance <= 0 in CMemView::iReplaceMemViewImage");
	ASSERT(m_hwnd, "m_hwnd <= 0 in CMemView::iReplaceMemViewImage");

	if ((hImageList = ListView_GetImageList(m_hwnd, bSmallImage)) == NULL)
		return -1;

	if (!uOverlayIconResID1 && !uOverlayIconResID2)
	{
		if ((hIcon = ::LoadIcon(m_hInstance, MAKEINTRESOURCE(uIconResID))) == NULL)
			return -1;
	}
	else
	{
		if ((hIcon = HGetIconFromResourceIDs(bSmallImage, uIconResID, uOverlayIconResID1, uOverlayIconResID2)) == NULL)
			return -1;
	}

	return ImageList_ReplaceIcon(hImageList, iImage, hIcon);
}


/*
INT CMemView::iSetMemViewUserImage(BOOL bSmallImage, UINT uIconResID, UINT uIconMeResID)
{
	INT				iRetVal;
	UINT			uFormat;
	HICON			hIcon1;
	HICON			hIcon2;
	HIMAGELIST		hImageList;	// handle to image list
	PICONRESOURCE	pIconRes1 = NULL;
	PICONRESOURCE	pIconRes2 = NULL;

	ASSERT(m_hInstance, "m_hInstance <= 0 in CMemView::iSetMemViewUserImage");
	ASSERT(m_hwnd, "m_hwnd <= 0 in CMemView::iSetMemViewUserImage");

	if ((hImageList = ListView_GetImageList(m_hwnd, bSmallImage)) == NULL)
		return -1;

	pIconRes1 = GetIconDetailsFromInstance(uIconResID);
	if (!pIconRes1)
		return -1;

	pIconRes2 = GetIconDetailsFromInstance(uIconMeResID);
	if (!pIconRes2)
		goto exit;

	ASSERT(pIconRes1->uNumImages == pIconRes2->uNumImages, "pIconRes1->uNumImages != pIconRes2->uNumImages in CMemView::iSetMemViewUserImage");
	
	for (uFormat = 0; uFormat < pIconRes1->uNumImages; uFormat++)
		if (((pIconRes1->IconImages+uFormat)->uWidth == 16 && bSmallImage) ||
			((pIconRes1->IconImages+uFormat)->uWidth == 32 && !bSmallImage))
			break;

	if (!bTreatIconBits(pIconRes1->IconImages+uFormat, pIconRes2->IconImages+uFormat))
		goto exit;

	if ((hIcon1 = MakeIconFromResource(pIconRes1->IconImages+uFormat)) == NULL)
		goto exit;

	if (bSmallImage)
	{
		if (-1 == m_iSmallIconMe)
			// set my small icon for the first time
			iRetVal = m_iSmallIconMe = ImageList_AddIcon(hImageList, hIcon1);
		else
			iRetVal = m_iSmallIconMe = ImageList_ReplaceIcon(hImageList, m_iSmallIconMe, hIcon1);
	}
	else
	{
		if (-1 == m_iSmallIconMe)
			// set my large icon for the first time
			iRetVal = m_iLargeIconMe = ImageList_AddIcon(hImageList, hIcon1);
		else
			iRetVal = m_iLargeIconMe = ImageList_ReplaceIcon(hImageList, m_iLargeIconMe, hIcon1);
	}

exit:
	if (pIconRes1)
	{
		for(uFormat = 0; uFormat < pIconRes1->uNumImages; uFormat++)
			if (pIconRes1->IconImages[uFormat].pbBits)
				delete [] pIconRes1->IconImages[uFormat].pbBits;
		delete [] (PBYTE) pIconRes1;
	}

	if (pIconRes2)
	{
		for(uFormat = 0; uFormat < pIconRes2->uNumImages; uFormat++)
			if (pIconRes2->IconImages[uFormat].pbBits)
				delete [] pIconRes2->IconImages[uFormat].pbBits;
		delete [] (PBYTE) pIconRes2;
	}

	return iRetVal;
}
*/


HICON CMemView::HGetIconFromResourceIDs(BOOL bSmallImage, UINT uIconResID, UINT uOverlayIconResID1, UINT uOverlayIconResID2)
{
	UINT			uFormat;
	HICON			hIcon;
	PICONRESOURCE	pIconRes[3] = { NULL, NULL, NULL };

	ASSERT(m_hInstance, "m_hInstance <= 0 in CMemView::HGetIconFromResourceIDs");
	ASSERT(m_hwnd, "m_hwnd <= 0 in CMemView::HGetIconFromResourceIDs");

	if ((pIconRes[0] = GetIconDetailsFromInstance(uIconResID)) == NULL)
		return NULL;

	for (uFormat = 0; uFormat < pIconRes[0]->uNumImages; uFormat++)
		if (((pIconRes[0]->IconImages+uFormat)->uWidth == 16 && bSmallImage) ||
			((pIconRes[0]->IconImages+uFormat)->uWidth == 32 && !bSmallImage))
			break;

	if (uOverlayIconResID1)
	{
		if ((pIconRes[1] = GetIconDetailsFromInstance(uOverlayIconResID1)) == NULL)
			goto exit;

		ASSERT(pIconRes[0]->uNumImages == pIconRes[1]->uNumImages, "pIconRes[0]->uNumImages != pIconRes[1]->uNumImages in CMemView::HGetIconFromResourceIDs");
		
		if (!bTreatIconBits(pIconRes[0]->IconImages+uFormat, pIconRes[1]->IconImages+uFormat))
			goto exit;
	}

	if (uOverlayIconResID2)
	{
		if ((pIconRes[2] = GetIconDetailsFromInstance(uOverlayIconResID2)) == NULL)
			goto exit;

		ASSERT(pIconRes[0]->uNumImages == pIconRes[2]->uNumImages, "pIconRes[0]->uNumImages != pIconRes[2]->uNumImages in CMemView::HGetIconFromResourceIDs");
		
		if (!bTreatIconBits(pIconRes[0]->IconImages+uFormat, pIconRes[2]->IconImages+uFormat))
			goto exit;
	}

	hIcon = MakeIconFromResource(pIconRes[0]->IconImages+uFormat);

exit:
	for (short nCnt = 0; nCnt < 3; nCnt++)
		if (pIconRes[nCnt])
		{
			for(uFormat = 0; uFormat < pIconRes[nCnt]->uNumImages; uFormat++)
				if (pIconRes[nCnt]->IconImages[uFormat].pbBits)
					delete [] pIconRes[nCnt]->IconImages[uFormat].pbBits;
			delete [] (PBYTE) pIconRes[nCnt];
		}

	return hIcon;
}


BOOL CMemView::AttachMemViewHWnd(HWND hwnd, HINSTANCE hInstance)
{
	ASSERT(hwnd,"hwnd is NULL in CMemView::AttachMemViewHWnd");
	ASSERT(hwnd,"hInstance is NULL in CMemView::AttachMemViewHWnd");

	if (m_hwnd)
	{
		::SetLastError(ERROR_GEN_FAILURE);
		return (FALSE);
	}

	m_hInstance = hInstance;

	if (m_hwnd=hwnd)
	{
		// ::SetWindowLong(m_hwnd, GWL_USERDATA, (long) this);

		if (m_hFont)
		{
			::SendMessage(m_hwnd, WM_SETFONT, (WPARAM) m_hFont, MAKELPARAM(TRUE, 0) /* fRedraw */);
		}
		else
		{
			::SendMessage(m_hwnd, WM_SETFONT, (WPARAM) ::GetStockObject(DEFAULT_GUI_FONT), MAKELPARAM(TRUE, 0) /* fRedraw */);
		}
		return (TRUE);
	}
	else
	{
		::SetLastError(ERROR_INVALID_PARAMETER);
		return (FALSE);
	}

}


HWND CMemView::DetachMemViewHWnd()
{
	if (!m_hwnd)
		return (NULL);
	
	if (m_bInternalCreation)
		m_bInternalCreation = FALSE;

	// TODO: Should the Font get deleted from the window?
//	if (NULL != m_hFont)
//		::DeleteObject(m_hFont);

	HWND hwnd = m_hwnd;
	m_hwnd = NULL;
	m_hInstance = NULL;

	return(hwnd);
}


BOOL CMemView::bClearMemView()
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bClearMemView");

	DWORD	dwItemData = 0L;
	BOOL	bDel;
	INT		iItem = 0;

	// empty the listview
	do
	{
		bDel = bDeleteMemViewItem(&dwItemData, iItem);
		iItem++;
		if (dwItemData && m_pfnDelete)
			(*m_pfnDelete)(dwItemData);
	}
	while(bDel);

	m_dwThisUser = 0L;

	return 0 == ListView_GetItemCount(m_hwnd);
}


BOOL CMemView::bFindMemViewItem(DWORD dwItemData, LPCTSTR szText, DWORD* pdwItemDataMatched)
{
	INT		cMem = 0;
	DWORD	dwTmp = 0L;

	ASSERT(pdwItemDataMatched, "pdwItemDataMatched is NULL in CMemView::bFindMemViewItem");
	ASSERT((dwItemData || szText) && !(dwItemData && szText), "Unexpected parameters in CMemView::bFindMemViewItem");

	*pdwItemDataMatched = 0L;

	if (szText)		// search by text
		*pdwItemDataMatched = dwGetItemDataFromText(szText);
	else			// search by member ID
	{
		cMem = iGetMemViewCount();

		if (cMem > 0)
		{		
			do
			{
				cMem--;
				dwTmp = dwGetItemDataFromIndex(cMem);
				if (dwTmp && m_pfnIsEqual && (*m_pfnIsEqual)(dwItemData, dwTmp))
					*pdwItemDataMatched = dwTmp;
			}
			while (!*pdwItemDataMatched && cMem > 0);
		}
	}

	return (0L != *pdwItemDataMatched);
}


INT CMemView::iGetMemViewCount()
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::iGetMemViewCount");

	return (INT) ListView_GetItemCount(m_hwnd);
}


INT CMemView::iGetMemViewSelectedCount()
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::iGetMemViewSelectedCount");

	return ListView_GetSelectedCount(m_hwnd);
}


BOOL CMemView::bSetMemViewDefaultFont(LOGFONT *pLogFont)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bSetMemViewDefaultFont");

	HFONT hFont = ::CreateFontIndirect(pLogFont);

	if (hFont)
	{
		::SendMessage(m_hwnd, WM_SETFONT, (WPARAM) hFont, MAKELPARAM(TRUE, 0) /* fRedraw */);
		if (m_hFont)
			::DeleteObject(m_hFont);
		m_hFont = hFont;

		return (TRUE);
	}

	return (FALSE);
}


BOOL CMemView::bAddMemViewColumn(LPCTSTR szHeader, INT iSubItem, INT nWidth)
{
	LV_COLUMN	lvC;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bAddMemViewColumn");

	ZeroMemory(&lvC, sizeof(LV_COLUMN));

	lvC.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvC.fmt	= LVCFMT_LEFT;
	if (nWidth < 0)
	{
		RECT rcClient;
		GetClientRect(m_hwnd, &rcClient);
		lvC.cx = rcClient.right - rcClient.left;
	}
	else
		lvC.cx = nWidth;
	lvC.pszText	= (LPTSTR) szHeader;
	lvC.iSubItem= iSubItem;

	return iSubItem == ListView_InsertColumn(m_hwnd, iSubItem, &lvC);
}


INT	CMemView::iAddMemViewItem(LPCTSTR szAlias, INT iImage, DWORD dwItemData)
{
	LV_ITEM lvI;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::iAddMemViewItem");

	ZeroMemory(&lvI, sizeof(LV_ITEM));

	lvI.mask		= LVIF_TEXT | LVIF_IMAGE | LVIF_PARAM | LVIF_STATE;
	lvI.state		= 0;
	lvI.stateMask	= LVIS_SELECTED | LVIS_FOCUSED | LVIS_CUT;	// or should we put 0 here?
	lvI.pszText		= (LPTSTR) szAlias;
	lvI.iImage		= iImage;
	lvI.lParam		= dwItemData;

	INT iRetVal = (INT) ListView_InsertItem(m_hwnd, &lvI);

	if (::GetWindowLong(m_hwnd, GWL_STYLE) & LVS_ICON)
		ListView_SortItems(m_hwnd, m_pfnCompare, (LPARAM) this);

	return iRetVal;
}


BOOL CMemView::bSetMemViewItemText(LPCTSTR szText, DWORD dwItemData, INT iItem, INT iSubItem)
{
	INT	iIndex = 0;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bSetMemViewItemText");

	if (iItem < 0)
	{
		if (-1 == (iIndex = iGetIndexFromItemData(dwItemData)))
		{
			::SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
	}
	else
		iIndex = iItem;

	ListView_SetItemText(m_hwnd, iIndex, iSubItem, (LPTSTR) szText);
	return TRUE;
}

	
BOOL CMemView::bSetMemViewItemImage(INT iImage, DWORD dwItemData, INT iItem)
{
	LV_ITEM	lvI;
	INT		iIndex;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bSetMemViewItemImage");

	if (iItem < 0)
	{
		if (-1 == (iIndex = iGetIndexFromItemData(dwItemData)))
		{
			::SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
	}
	else
		iIndex = iItem;

	ZeroMemory(&lvI, sizeof(LV_ITEM));

	lvI.mask	= LVIF_IMAGE;
	lvI.iItem	= iIndex;
	lvI.iImage	= iImage;


	return ListView_SetItem(m_hwnd, &lvI);
}


BOOL CMemView::bDeleteMemViewItem(DWORD* pdwItemData, INT iItem)
{
	INT		iIndex = 0;
	DWORD	dwItemData = 0L;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bDeleteMemViewItem");

	if (iItem < 0)
	{
		ASSERT(pdwItemData, "pdwItemData is NULL in CMemView::bDeleteMemViewItem");
		dwItemData = *pdwItemData;
		if (-1 == (iIndex = iGetIndexFromItemData(dwItemData)))
		{
			::SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
	}
	else
	{
		iIndex = iItem;
		dwItemData = dwGetItemDataFromIndex(iIndex);
		if (pdwItemData)
			*pdwItemData = dwItemData;
	}

	if (dwItemData && m_pfnDelete)
		(*m_pfnDelete)(dwItemData);

	if (dwItemData == m_dwThisUser)
		m_dwThisUser = 0L;

	return ListView_DeleteItem(m_hwnd, iIndex);
}


INT CMemView::iGetIndexFromItemData(DWORD dwItemData)
{
	LV_FINDINFO	lvFi;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::iGetIndexFromItemData");

	ZeroMemory(&lvFi, sizeof(LV_FINDINFO));
	
	lvFi.flags = LVFI_PARAM;
	lvFi.psz = NULL;
	lvFi.lParam = dwItemData;

	return ListView_FindItem(m_hwnd, -1, &lvFi);
}


DWORD CMemView::dwGetItemDataFromIndex(INT iIndex)
{
	LV_ITEM lvI;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::dwGetItemDataFromIndex");

	ZeroMemory(&lvI, sizeof(LV_ITEM));

	lvI.mask	= LVIF_PARAM;
	lvI.iItem	= iIndex;

	if (ListView_GetItem(m_hwnd, &lvI))
		return lvI.lParam;
	else
		return 0L;
}


BOOL CMemView::bGetMemViewItemText(LPTSTR szText, DWORD cbText, DWORD dwItemData, INT iItem, INT iSubItem)
{
	LV_ITEM	lvI;
	INT		iIndex;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bGetMemViewItemText");
	ASSERT(szText, "szText is NULL in CMemView::bGetMemViewItemText");
	ASSERT(iSubItem >= 0, "iSubItem < 0 in CMemView::bGetMemViewItemText");

	if (iItem < 0)
	{
		if (-1 == (iIndex = iGetIndexFromItemData(dwItemData)))
		{
			::SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
	}
	else
		iIndex = iItem;

	ZeroMemory(&lvI, sizeof(LV_ITEM));

	lvI.mask		= LVIF_TEXT;
	lvI.iItem		= iIndex;
	lvI.iSubItem	= iSubItem;
	lvI.pszText		= szText;
	lvI.cchTextMax	= cbText;

	return ListView_GetItem(m_hwnd, &lvI);
}


BOOL CMemView::bGetMemViewItemFromPoint(POINT pt, DWORD* pdwItemData, INT* piItem, INT* piSubItem)
{
	INT				iIndex;
	LV_HITTESTINFO	lvH;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bGetMemViewItemFromPoint");
	ASSERT(pdwItemData || piItem || piSubItem, "!pdeItemData && !piItem && !piSubItem in CMemView::bGetMemViewItemFromPoint");

	if (pdwItemData)
		*pdwItemData = 0L;

	if (piItem)
		*piItem = 0;

	if (piSubItem)
		*piSubItem = 0;

	ZeroMemory(&lvH, sizeof(LV_HITTESTINFO));

	lvH.pt = pt;

	iIndex = ListView_HitTest(m_hwnd, &lvH);

	if (-1 == iIndex)
		return FALSE;
	else
	{
		if (pdwItemData)
			*pdwItemData = dwGetItemDataFromIndex(iIndex);
		if (piItem)
			*piItem = iIndex;
		if (piSubItem)
			*piSubItem = lvH.iSubItem;
		return TRUE;
	}
}


DWORD CMemView::dwGetItemDataFromText(LPCTSTR szText)
{
	INT			iIndex;
	LV_FINDINFO	lvFi;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::dwGetItemDataFromText");

	ZeroMemory(&lvFi, sizeof(LV_FINDINFO));
	
	lvFi.flags = LVFI_STRING;
	lvFi.psz = szText;

	iIndex = ListView_FindItem(m_hwnd, -1, &lvFi);
	if (-1 == iIndex)
		return 0L;
	else
		return dwGetItemDataFromIndex(iIndex);
}


BOOL CMemView::bSetMemViewItemState(DWORD dwItemData, INT iItem, UINT uState)
{
	INT	iIndex = 0;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bSetMemViewItemState");

	if (iItem < 0)
	{
		if (-1 == (iIndex = iGetIndexFromItemData(dwItemData)))
		{
			::SetLastError(ERROR_INVALID_PARAMETER);
			return FALSE;
		}
	}
	else
		iIndex = iItem;

	ListView_SetItemState(m_hwnd, iIndex, uState, LVIS_SELECTED | LVIS_FOCUSED | LVIS_CUT);

	return TRUE;
}


BOOL CMemView::bSetMemViewMode(DWORD dwDesactivateStyle, DWORD dwActivateStyle) 
// dwActivateStyle can include LVS_ICON, LVS_SMALLICON, LVS_LIST, or LVS_REPORT
{
	DWORD dwStyle = 0L;

	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bSetMemViewMode");

	if (0 == (dwStyle = ::GetWindowLong(m_hwnd, GWL_STYLE)))
		return FALSE;

	dwStyle &= ~dwDesactivateStyle;

	dwStyle |= dwActivateStyle;

	return 0 != ::SetWindowLong(m_hwnd, GWL_STYLE, dwStyle);
}


INT CMemView::iGetMemViewSelection(DWORD* prgdwItemData, INT* prgiIndex, INT cArraySize)
{
	ASSERT(m_hwnd, "m_hwnd is NULL in CMemView::bGetMemViewSelection");
	ASSERT((prgdwItemData || prgiIndex) && !(prgdwItemData && prgiIndex), "Unexpected array pointers in CMemView::bGetMemViewSelection");

	INT	iIndex	= -1;
	INT	cSel	= 0;

	do
	{
		iIndex = ListView_GetNextItem(m_hwnd, iIndex, LVNI_ALL | LVNI_SELECTED);
		if (iIndex != -1)
		{
			if (cArraySize == cSel)
				return -1;

			if (prgiIndex)
				prgiIndex[cSel] = iIndex;
			else
				prgdwItemData[cSel] = dwGetItemDataFromIndex(iIndex);

			cSel++;
		}
	}
	while (iIndex != -1);

	return cSel;
}


HICON CMemView::GetIconFromInstance(UINT uResID)
{
    HICON	hIcon = NULL;
    HRSRC	hRsrc = NULL;
    HGLOBAL	hGlobal = NULL;
    LPVOID	pvRes = NULL;
    int    	nID;

    // Find the group icon
    if((hRsrc = FindResource(m_hInstance, (LPCTSTR) MAKELPARAM(uResID, 0), RT_GROUP_ICON)) == NULL)
        return NULL;
    if((hGlobal = LoadResource(m_hInstance, hRsrc)) == NULL)
        return NULL;
    if((pvRes = LockResource(hGlobal)) == NULL)
        return NULL;

    // Find this particular image
    nID = LookupIconIdFromDirectory((PBYTE) pvRes, TRUE);
    if((hRsrc = FindResource(m_hInstance, MAKEINTRESOURCE(nID), RT_ICON)) == NULL)
        return NULL;
    if((hGlobal = LoadResource(m_hInstance, hRsrc)) == NULL)
        return NULL;
    if((pvRes = LockResource(hGlobal)) == NULL)
        return NULL;
    // Let the OS make us an icon
    hIcon = CreateIconFromResource((PBYTE) pvRes, SizeofResource(m_hInstance, hRsrc), TRUE, 0x00030000);
    return hIcon;
}


PICONRESOURCE CMemView::GetIconDetailsFromInstance(UINT uResID)
{
    PICONRESOURCE	pIR = NULL;
    HRSRC			hRsrc = NULL;
    HGLOBAL			hGlobal = NULL;
    PMEMICONDIR		pIcon = NULL;
    UINT			i;

	// Find the group icon resource
    if ((hRsrc = FindResource(m_hInstance, MAKEINTRESOURCE(uResID), RT_GROUP_ICON)) == NULL)
        return NULL;

	if ((hGlobal = LoadResource(m_hInstance, hRsrc)) == NULL)
		return NULL;

	if((pIcon = (PMEMICONDIR) LockResource(hGlobal)) == NULL)
        return NULL;

	// Allocate enough memory for the images
    if((pIR = (PICONRESOURCE) new BYTE[sizeof(ICONRESOURCE) + ((pIcon->idCount-1) * sizeof(ICONIMAGE))]) == NULL)
		return NULL;

	// Fill in local struct members
    pIR->uNumImages = pIcon->idCount;
    
    for(i = 0; i < pIR->uNumImages; i++)
		pIR->IconImages[i].pbBits = NULL;

	// Loop through the images
    for(i = 0; i < pIR->uNumImages; i++)
    {
        // Get the individual image
        if((hRsrc = FindResource(m_hInstance, MAKEINTRESOURCE(pIcon->idEntries[i].nID), RT_ICON)) == NULL)
			goto error;

		if((hGlobal = LoadResource(m_hInstance, hRsrc)) == NULL)
			goto error;

		// Store a copy of the resource locally
        pIR->IconImages[i].dwNumBytes = SizeofResource(m_hInstance, hRsrc);
        if ((pIR->IconImages[i].pbBits = new BYTE[pIR->IconImages[i].dwNumBytes]) == NULL)
			goto error;

		CopyMemory((PVOID) pIR->IconImages[i].pbBits, LockResource(hGlobal), pIR->IconImages[i].dwNumBytes);
        // Adjust internal pointers
        if(!bAdjustIconImagePointers(&(pIR->IconImages[i])))
			goto error;
    }
    return pIR;

error:
    for(i = 0; i < pIR->uNumImages; i++)
		if (pIR->IconImages[i].pbBits)
			delete [] pIR->IconImages[i].pbBits;
	delete [] (PBYTE) pIR;
	return NULL;
}


BOOL CMemView::bAdjustIconImagePointers(PICONIMAGE pImage)
{
    // Sanity check
    if(!pImage)
        return FALSE;

    // BITMAPINFO is at beginning of bits
    pImage->pbi = (LPBITMAPINFO) pImage->pbBits;
    // Width - simple enough
    pImage->uWidth = pImage->pbi->bmiHeader.biWidth;
    // Icons are stored in funky format where height is doubled - account for it
    pImage->uHeight = (pImage->pbi->bmiHeader.biHeight)/2;
    // How many colors?
    pImage->uColors = pImage->pbi->bmiHeader.biPlanes * pImage->pbi->bmiHeader.biBitCount;
    // XOR bits follow the header and color table
    pImage->pbXOR = (PBYTE) FindDIBBits((LPSTR) pImage->pbi);
    // AND bits follow the XOR bits
    pImage->pbAND = pImage->pbXOR + (pImage->uHeight*BytesPerLine((LPBITMAPINFOHEADER)(pImage->pbi)));

    return TRUE;
}


HICON CMemView::MakeIconFromResource(PICONIMAGE pIcon)
{
    HICON hIcon = NULL;

    // Sanity Checks
    if (!pIcon)
        return NULL;

    if (!pIcon->pbBits)
        return NULL;

    // Let the OS do the real work :)
    hIcon = CreateIconFromResourceEx(pIcon->pbBits, pIcon->dwNumBytes, TRUE, 0x00030000, 
            (*(LPBITMAPINFOHEADER)(pIcon->pbBits)).biWidth, (*(LPBITMAPINFOHEADER)(pIcon->pbBits)).biHeight/2, 0 );
    
    // It failed, odds are good we're on NT so try the non-Ex way
    if (!hIcon)
    {
        // We would break on NT if we try with a 16bpp image
        if (pIcon->pbi->bmiHeader.biBitCount != 16)
            hIcon = CreateIconFromResource(pIcon->pbBits, pIcon->dwNumBytes, TRUE, 0x00030000);
    }
    return hIcon;
}


BOOL CMemView::bTreatIconBits(PICONIMAGE pIcon1, PICONIMAGE pIcon2)
{
	DWORD *pdwBits1, dwBits2;

	short nNumDWords = pIcon1->uWidth * pIcon1->uHeight * pIcon1->uColors / 32;
	
	for (short cCnt = 0; cCnt < nNumDWords; cCnt++)
	{
		pdwBits1  =   (DWORD*) ((PBYTE) pIcon1->pbXOR+4*cCnt);
		dwBits2   = *((DWORD*) ((PBYTE) pIcon2->pbXOR+4*cCnt));
		*pdwBits1 &= ~dwBits2;
	}
	return TRUE;
}
