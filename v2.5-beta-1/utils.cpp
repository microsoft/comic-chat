#include "stdafx.h"
#include "utils.h"
#include "mcithrd.h"

// Gets the Windows media directory from the registry, if there is one.

BOOL 
GetWindowsMediaDirectory(
CString * pstrDirectory)
{
	pstrDirectory->Empty ();

	HKEY hkeyReg;

	// Only open with KEY_QUERY_VALUE access. This is really all we need, and
	// trying to open with too many permissions will fail if the user is not an
	// administrator, since we are opening a Local Machine key.
	if (FAILED(RegOpenKeyEx (HKEY_LOCAL_MACHINE, 
					"Software\\Microsoft\\Windows\\CurrentVersion",
					0, KEY_QUERY_VALUE, &hkeyReg)))
	{
		return FALSE;
	}

	DWORD dwSize = _MAX_PATH + 1;
	LPSTR pszDirectory = pstrDirectory->GetBuffer (dwSize);
	BOOL bRet = SUCCEEDED(RegQueryValueEx (hkeyReg, "MediaPath", NULL, NULL, 
								(LPBYTE)pszDirectory, &dwSize));
	CharLower (pszDirectory);
	pstrDirectory->ReleaseBuffer ();
	RegCloseKey (hkeyReg);

	return bRet;
}

// Returns the icon for a given filetype.

HICON 
GetFiletypeIcon(
LPCSTR pszFiletype, 
BOOL bSmallIcon)
{
	SHFILEINFO shfi;

	// Create a dummy filename of the given type. We can use the SHGFI_USEFILEATTRIBUTES
	// to treat this dummy filename as a real file.
	CString str;
	str.Format ("a.%s", pszFiletype);
	if (SHGetFileInfo (str, FILE_ATTRIBUTE_NORMAL, &shfi, sizeof(shfi), 
			SHGFI_ICON | (bSmallIcon ? SHGFI_SMALLICON : 0) | SHGFI_USEFILEATTRIBUTES) == 0)
	{
		return NULL;
	}
	return shfi.hIcon;
}

// Returns a DLL's version, if it supports DLLGetVersion

DWORD 
GetDllVersion(
HINSTANCE hinst, 
DWORD dwDefaultVersion)
{
	DLLGETVERSIONPROC pDllGetVersion = (DLLGETVERSIONPROC)GetProcAddress (hinst, TEXT("DllGetVersion"));
	if (pDllGetVersion)      
	{      
		DLLVERSIONINFO    dvi;      
		ZeroMemory (&dvi, sizeof(dvi));      
		dvi.cbSize = sizeof(dvi);   
		if (SUCCEEDED ((*pDllGetVersion)(&dvi)))
		{
			return DLLVER ((WORD)dvi.dwMajorVersion, (WORD)dvi.dwMinorVersion);
		}
	}
	return dwDefaultVersion;
}

// GetDllVersion, except by name instead of HINSTANCE. Returns 0 if the
// DLL can't be loaded.

DWORD 
GetDllVersion(
LPCSTR pszDLL, 
DWORD dwDefaultVersion)
{
	DWORD dwVer = 0;
	HINSTANCE hinst = ::LoadLibrary (pszDLL);
	if (hinst)
	{
		dwVer = GetDllVersion (hinst, dwDefaultVersion);
		::FreeLibrary (hinst);
	}
	return dwVer;
}

DWORD
GetComCtlVersion()
{
	static DWORD dwVer = (DWORD)-1L;

	// Cache calls to this function for speed.
	if (dwVer != (DWORD)-1L)
	{
		return dwVer;
	}

	dwVer = GetDllVersion ("comctl32.dll", COMCTLVER(4,0));
	ASSERT(dwVer != 0);
	return dwVer;
}

BOOL 
IsComCtlNewerThan(
DWORD dwVersion)
{
	return GetComCtlVersion () >= dwVersion;
}

// Adds a list element to the head of the list.

void
CListObject::AddToListHead(
CListObject* * pList)
{
	m_pNextInList = *pList;
	*pList = this;
}

// Adds a list element to the tail of the list.

void
CListObject::AddToListTail(
CListObject* * pList)
{
	while (*pList != NULL)
		pList = &((*pList)->m_pNextInList);
	*pList = this;
	m_pNextInList = NULL;
}

// Removes an element from the list

void
CListObject::RemoveFromList(
CListObject* * pList)
{
	while (*pList != this)
	{
		pList = &((*pList)->m_pNextInList);
		ASSERT(*pList != NULL);
	}
	*pList = m_pNextInList;
}

// Frees all elements in the list.

void
CListObject::FreeList()
{
	CListObject* pThis;
	CListObject* pNext = this;
	while ((pThis = pNext) != NULL)
	{
		pNext = pThis->m_pNextInList;
		delete pThis;
	}
}

// Gets the count of items in the list.

int
CListObject::GetListCount()
{
	int nCount = 0;
	CListObject* pThis = this;
	while (pThis)
	{
		nCount++;
		pThis = pThis->m_pNextInList;
	}
	return nCount;
}

// ===============================================================================
// CSimpleComboBox implementation

BEGIN_MESSAGE_MAP(CSimpleComboBox, CComboBox)
	//{{AFX_MSG_MAP(CSimpleComboBox)
	ON_WM_CTLCOLOR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

HBRUSH
CSimpleComboBox::OnCtlColor(
CDC* pDC,
CWnd* pWnd,
UINT nCtlColor)
{
	HBRUSH hbr;
	if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		UINT nFg, nBk;
		if (IsWindowEnabled ())
		{
			nFg = COLOR_WINDOWTEXT;
			nBk = COLOR_WINDOW;
		}
		else
		{
			nFg = COLOR_GRAYTEXT;
			nBk = COLOR_BTNFACE;
		}
		pDC->SetTextColor (::GetSysColor (nFg));
		pDC->SetBkColor (::GetSysColor (nBk));
		hbr = ::GetSysColorBrush (nBk);
	}
	else
	{
		hbr = CComboBox::OnCtlColor (pDC, pWnd, nCtlColor);
	}
	return hbr;
}

// Returns the current logged in username, or a default if there isn't one.

static void
GetCurrentUserName(
CString& strName)
{
	static BOOL bHaveUserName = FALSE;
	static char szUserName[128];

	if (!bHaveUserName)
	{
		szUserName[0] = '\0';
		HINSTANCE hinst = LoadLibrary ("mpr.dll");
		if (hinst)
		{
			DWORD (WINAPI * pfnWNetGetUser)(LPCTSTR, LPTSTR, LPDWORD);
			*(PVOID *)&pfnWNetGetUser = (PVOID)GetProcAddress (hinst, "WNetGetUserA");
			if (pfnWNetGetUser)
			{
				DWORD dwLen = sizeof(szUserName);
				DWORD dwRet = pfnWNetGetUser (NULL, szUserName, &dwLen);
				if (dwRet == NO_ERROR || dwRet == ERROR_MORE_DATA)
					szUserName[sizeof(szUserName) - 1] = '\0';
				else
					szUserName[0] = '\0';
			}
			FreeLibrary (hinst);
		}
		if (szUserName[0] == '\0')
			lstrcpy (szUserName, "GenericUser");
		bHaveUserName = TRUE;
	}

	strName = szUserName;
}



// Encode a data block with encryption. The size of the input buffer must
// be nDecryptedSize bytes, and the size of the output buffer must be 
// twice that of the input buffer plus 4 bytes.

void 
encEncodeData(
PBYTE pbDataOut, 
PBYTE pbDataIn, 
UINT nDecryptedSize)
{
	UINT nEncryptedSize = 2 * nDecryptedSize + sizeof(DWORD);

	ASSERT (pbDataOut != NULL && !IsBadWritePtr (pbDataOut, nEncryptedSize));
	ASSERT (pbDataIn != NULL && !IsBadReadPtr (pbDataIn, nDecryptedSize));

	CString strUserName;
	GetCurrentUserName (strUserName);
	UINT nUserNameLen = strUserName.GetLength ();

	ZeroMemory (pbDataOut, nEncryptedSize);
	PWORD pwDest = (PWORD)(pbDataOut + sizeof(DWORD));
	PBYTE pbSrc = pbDataIn;
	FILETIME ft;
	GetSystemTimeAsFileTime (&ft);
	DWORD dwTime = ft.dwLowDateTime;
	BYTE nLo = 0;
	WORD wPrev = 0;
	DWORD dwChecksum = 0;
	WORD w;
	for (UINT i = nDecryptedSize; i > 0; i--)
	{
		w = *(pbSrc++) ^ (WORD)(strUserName.GetAt (i % nUserNameLen));
		w = ((w & 0xcc) >> 2) | ((w & 0x33) << 8);
		w |= (LOWORD(dwTime) & 0xcccc);
		w ^= nLo ? LOWORD(dwTime) : HIWORD(dwTime);
		*(pwDest++) = wPrev = wPrev ^ w;
		dwChecksum += wPrev;
		nLo = nLo ^ 1;
		dwTime = dwTime + 3495364871;
	}
	*(LPDWORD)pbDataOut = dwTime ^ dwChecksum;
}

// Decode a data block with decryption. The size of the output buffer must
// be nDecryptedSize bytes, and the size of the input buffer must be 
// twice that of the output buffer plus 4 bytes. Returns TRUE if the
// data was decrypted successfully.

BOOL 
encDecodeData(
PBYTE pbDataOut, 
PBYTE pbDataIn, 
UINT nDecryptedSize)
{
	UINT nEncryptedSize = 2 * nDecryptedSize + sizeof(DWORD);
	
	ASSERT (pbDataOut != NULL && !IsBadWritePtr (pbDataOut, nDecryptedSize));
	ASSERT (pbDataIn != NULL && !IsBadReadPtr (pbDataIn, nEncryptedSize));

	CString strUserName;
	GetCurrentUserName (strUserName);
	UINT nUserNameLen = strUserName.GetLength ();

	PWORD pwSrc = (PWORD)(pbDataIn + sizeof(DWORD));
    DWORD dwChecksum = 0;
    WORD  w = 0;
    for (UINT i = 0; i < nDecryptedSize; i++)
    {
        w = w | pwSrc[i];
        dwChecksum += pwSrc[i];
    }
    if (!w)
    {
		// Impossible for the buffer to be completely zero.
        return FALSE;
    }
		
	// Calculate seed time again.
    DWORD dwTime;
    for (dwTime = *(PDWORD)pbDataIn ^ dwChecksum, i = 0;
         i < nDecryptedSize;
         dwTime = dwTime - 3495364871, i++);

	PBYTE pbDest = pbDataOut;
    BYTE nLo = 0;
    WORD  wPrev = 0;
	for (i = nDecryptedSize; i > 0; i--)
    {
        w = *pwSrc ^ wPrev;
        wPrev = *(pwSrc++);
        w ^= nLo ? LOWORD(dwTime) : HIWORD(dwTime);
        *(pbDest++) = (BYTE)((((w & 0x3300) >> 8) | ((w & 0x0033) << 2)) ^ 
								strUserName.GetAt (i % nUserNameLen));
        nLo = nLo ^ 1;
        dwTime = dwTime + 3495364871;
    }
	return TRUE;
}

// =================================================================================
// CIconicComboBox implementation.

// Dotted brush.

class CDottedBrush : public CBrush
{
public:
	BOOL Create();
};

BOOL
CDottedBrush::Create()
{
	short sBits[] = { 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55, 0xaa, 0x55 };
	if (m_hObject)
		return TRUE;
	CBitmap bm;
	if (!bm.CreateBitmap (8, 8, 1, 1, sBits))
		return FALSE;
	return CreatePatternBrush (&bm);
}

static CDottedBrush brushDotted;


CIconicComboBox::CIconicComboBox()
{
}

CIconicComboBox::~CIconicComboBox()
{
}

const int g_cxSmallIconSize = 16;
const int g_cySmallIconSize = 16;

// Create an iconic combobox on the parent window, and use it to 
// replace an existing combobox. The new combobox takes on all properties of the 
// original combobox, including position, size, style, extended style, and ID.
// It does NOT copy any existing items from the original combobox, so be sure
// to call this before adding items to the combo.
// Note: if the original combobox had a scrollbar, you need to specify 
// WS_VSCROLL in dwAdditionalStyles. This is because there is no API to find 
// out whether a combobox has a scrollbar or not (WS_HSCROLL and WS_VSCROLL are
// not preserved after creation).

BOOL
CIconicComboBox::ReplaceControl(
CWnd* pParentWnd,
UINT  nID,
DWORD dwAdditionalStyles)
{
	ASSERT_VALID(pParentWnd);
	CComboBox* pOrigCombo = (CComboBox*)pParentWnd->GetDlgItem (nID);
	ASSERT(pOrigCombo != NULL);
   
   #ifdef DEBUG
    // Verify that the window is, indeed, a combobox.
	char szClass[32];
	::GetClassName (pOrigCombo->m_hWnd, szClass, sizeof(szClass));
	ASSERT(!lstrcmpi (szClass, "combobox"));
   #endif
   
	DWORD dwOrigStyle, dwOrigStyleEx;
	CRect rectOrig;

	dwOrigStyle   = pOrigCombo->GetStyle ();
	dwOrigStyleEx = pOrigCombo->GetExStyle ();
	pOrigCombo->GetWindowRect (&rectOrig);
	// If the combo box is a dropdown or dropdown list, we need to get it's
	// dropdown rect, and combine the two.
	if ((dwOrigStyle & CBS_DROPDOWN) != 0)
	{
		CRect rectDropdown;
		pOrigCombo->GetDroppedControlRect (rectDropdown);
		rectOrig.bottom = rectDropdown.bottom;
	}
	pParentWnd->ScreenToClient (&rectOrig);

	ASSERT ((dwAdditionalStyles & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE | CBS_HASSTRINGS)) == 0);
	ASSERT ((dwOrigStyle & (CBS_OWNERDRAWFIXED | CBS_OWNERDRAWVARIABLE)) == 0);
	dwOrigStyle |= CBS_OWNERDRAWFIXED | CBS_HASSTRINGS | dwAdditionalStyles;

	if (!Create (dwOrigStyle, rectOrig, pParentWnd, nID))
	{
		return FALSE;
	}

	DWORD dwNewStyleEx = GetExStyle ();
	if (dwNewStyleEx != dwOrigStyleEx)
		ModifyStyleEx (dwNewStyleEx & ~dwOrigStyleEx, dwOrigStyleEx & ~dwNewStyleEx, 0);
	
	SetWindowPos (pOrigCombo, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
	pOrigCombo->DestroyWindow ();

	// Font and item height stuff.

	int nEditHeight = GetItemHeight (-1);
	int nItemHeight;
	CFont * pfont = pParentWnd->GetFont ();
	if (pfont != NULL)
	{
		SetFont (pfont);
		nEditHeight = GetItemHeight (-1);
	   
	   #ifdef DEBUG
		LOGFONT lf;
		pfont->GetLogFont (&lf);
	   #endif
	   	CDC* pDC;
		if ((pDC = GetDC ()) != NULL)
		{
			TEXTMETRIC tm;
			CFont* pfontPrev = pDC->SelectObject (pfont);
			pDC->GetTextMetrics (&tm);
			pDC->SelectObject (pfontPrev);
			nEditHeight = tm.tmHeight + tm.tmExternalLeading + 2;
			ReleaseDC (pDC);
		}

		BOOL bDropDownList = (dwOrigStyle & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST;
		nItemHeight = max (g_cySmallIconSize + 2, nEditHeight);
		SetItemHeight (0, nItemHeight);
		SetItemHeight (-1, bDropDownList ? nItemHeight : nEditHeight);
	}

	return TRUE;
}

BEGIN_MESSAGE_MAP(CIconicComboBox, CComboBox)
	ON_WM_MEASUREITEM_REFLECT()
	ON_WM_DRAWITEM_REFLECT()
END_MESSAGE_MAP()

// WM_MEASUREITEM reflection.

void 
CIconicComboBox::MeasureItem(
LPMEASUREITEMSTRUCT lpMeasureItemStruct)
{
	if (lpMeasureItemStruct->itemHeight < 17)
		lpMeasureItemStruct->itemHeight = 17;
}

// WM_DRAWITEM reflection.

void 
CIconicComboBox::DrawItem(
LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	CRect rectIcon (&lpDrawItemStruct->rcItem);
	CRect rectText (&lpDrawItemStruct->rcItem);
	rectIcon.right = rectIcon.left + g_cxSmallIconSize + 2;
	rectText.left += g_cxSmallIconSize + 2;
	CRect rectTextAlpha (rectText);
	rectTextAlpha.left += 2;

	if ((lpDrawItemStruct->itemAction & (ODA_DRAWENTIRE | ODA_SELECT)) != 0)
	{
		if (lpDrawItemStruct->itemID == -1)
		{
			::FillRect (lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, 
				::GetSysColorBrush (IsWindowEnabled () ? COLOR_WINDOW : COLOR_BTNFACE));
		}
		else
		{
			CString strText;
			GetLBText (lpDrawItemStruct->itemID, strText);
			DWORD dwData = lpDrawItemStruct->itemData;
			HICON hicon = GetIcon (lpDrawItemStruct->itemID, strText, dwData);
			UINT nBkColor, nTextColor;
			CDC* pDC = CDC::FromHandle (lpDrawItemStruct->hDC);
			if (!IsWindowEnabled ())
			{
				nBkColor = COLOR_BTNFACE;
				nTextColor = COLOR_GRAYTEXT;
			}
			else if (lpDrawItemStruct->itemState & ODS_SELECTED)
			{		
				nBkColor = COLOR_HIGHLIGHT;
				nTextColor = COLOR_HIGHLIGHTTEXT;
			}
			else
			{
				nBkColor = COLOR_WINDOW;
				nTextColor = COLOR_WINDOWTEXT;
			}

			::FillRect (pDC->m_hDC, rectIcon, ::GetSysColorBrush (COLOR_WINDOW));
			if (hicon != NULL)
			{
				::DrawIconEx (pDC->m_hDC, 
					rectIcon.left, (rectIcon.top + rectIcon.bottom - g_cySmallIconSize) / 2, 
					hicon,
					0, 0,
					0, 
					::GetSysColorBrush (COLOR_WINDOW),
					DI_NORMAL);
			}

			::FillRect (pDC->m_hDC, rectText, ::GetSysColorBrush (nBkColor));
			COLORREF clrrefBk = pDC->SetBkColor (::GetSysColor (nBkColor));
			COLORREF clrrefText = pDC->SetTextColor (::GetSysColor (nTextColor));
			pDC->DrawText (strText, -1, rectTextAlpha, 
				DT_LEFT | DT_NOPREFIX | DT_VCENTER | DT_SINGLELINE);
			if ((lpDrawItemStruct->itemState & ODS_COMBOBOXEDIT) == 0 &&
					ShouldDrawDivision (lpDrawItemStruct->itemID, strText, dwData) &&
					brushDotted.Create ())
			{
				pDC->SetBkColor (::GetSysColor (COLOR_WINDOW));
				pDC->SetTextColor (::GetSysColor (COLOR_WINDOWTEXT));
				pDC->FillRect (CRect (rectIcon.left, rectIcon.bottom - 1, rectText.right, rectIcon.bottom), 
							   &brushDotted);
			}
			pDC->SetBkColor (::GetSysColor (nBkColor));
			pDC->SetTextColor (::GetSysColor (nTextColor));
		}
	}
	if (lpDrawItemStruct->itemAction == ODA_FOCUS || 
			(lpDrawItemStruct->itemState & ODS_FOCUS) != 0)
	{
		::DrawFocusRect (lpDrawItemStruct->hDC, rectText);
	}
}

HICON 
CIconicComboBox::GetIcon(
UINT   nIndex, 
LPCSTR pszString, 
DWORD  dwItemData)
{
	return (HICON)dwItemData;
}

BOOL
CIconicComboBox::ShouldDrawDivision(
UINT   nIndex, 
LPCSTR pszString, 
DWORD  dwItemData)
{
	return FALSE;
}

// =================================================================================
// CBrowseFolderDialog implementation.

// Small inline class to handle getting the Shell's IMalloc interface when needed.

class SHMALLOC
{
public:
	SHMALLOC()
		{ m_pMalloc = NULL; }
	~SHMALLOC()
		{ if (m_pMalloc) m_pMalloc->Release (); }
	LPMALLOC GetMalloc()
		{ if (!m_pMalloc) SHGetMalloc (&m_pMalloc); return m_pMalloc; }
private:
	LPMALLOC m_pMalloc;
} g_SHMalloc;
inline LPMALLOC GetShellMalloc() { return g_SHMalloc.GetMalloc (); }

IMPLEMENT_DYNAMIC(CBrowseFolderDialog, CDialog)

CBrowseFolderDialog::CBrowseFolderDialog(
LPCSTR pszTitle /*= NULL*/, 
LPCSTR pszDescription /*= NULL*/, 
LPCSTR pszRoot /*= NULL*/,
LPCSTR pszInitialFolder /*= NULL*/,
UINT uFlags /*= BIF_RETURNONLYFSDIRS*/,
CWnd* pParentWnd /*= NULL*/)
:
CCommonDialog(pParentWnd)
{
	ZeroMemory (&m_bi, sizeof(m_bi));
	ZeroMemory (&m_szDisplayName, sizeof(m_szDisplayName));
	if (pszRoot != NULL)
	{
	    ConvertFolderNameToIIDList (pszRoot, (LPITEMIDLIST *)&m_bi.pidlRoot);
	}
	if (pszInitialFolder != NULL)
		m_strSelectedFolder = pszInitialFolder;
	if (pszTitle != NULL)
		m_strTitle = pszTitle;
	m_bi.pszDisplayName = m_szDisplayName;
	m_bi.lpszTitle = pszDescription;
	m_bi.ulFlags = uFlags | BIF_RETURNONLYFSDIRS;
	m_bi.lpfn = OurBrowseFolderCallbackProc;
}

CBrowseFolderDialog::~CBrowseFolderDialog()
{
	if (m_bi.pidlRoot != NULL)
	{
		GetShellMalloc()->Free ((PVOID)m_bi.pidlRoot);
	}
}

int 
CBrowseFolderDialog::DoModal()
{
	ASSERT_VALID(this);
	ASSERT(m_bi.lpfn == OurBrowseFolderCallbackProc);
	m_bi.hwndOwner = PreModal();
	LPITEMIDLIST piidlist = ::SHBrowseForFolder (&m_bi);
	PostModal();

	if (piidlist != NULL)
	{
		BOOL b = ConvertIIDListToFolderName (piidlist, &m_strSelectedFolder);
		GetShellMalloc ()->Free (piidlist);
		return b ? IDOK : -1;
	}
	else
	{
		return IDCANCEL;
	}
}

// Converts an LPITEMIDLIST to a folder name.

BOOL 
CBrowseFolderDialog::ConvertIIDListToFolderName(
LPITEMIDLIST piidlist, 
CString * pstrFolderOut)
{
	ASSERT (piidlist != NULL);
	ASSERT (pstrFolderOut != NULL);
	LPSTR psz = pstrFolderOut->GetBuffer (_MAX_PATH + 1);
	BOOL bRet = SHGetPathFromIDList (piidlist, psz);
	pstrFolderOut->ReleaseBuffer ();
	return bRet;
}

// Converts a folder name to an LPITEMIDLIST. The list must be freed.

BOOL 
CBrowseFolderDialog::ConvertFolderNameToIIDList(
LPCSTR pszFolder,
LPITEMIDLIST *ppiidlistOut)
{
	USES_CONVERSION;

	ASSERT (pszFolder != NULL);
	ASSERT (ppiidlistOut != NULL);
	
	// Make sure it exists, the old way.

	if (::GetFileAttributes (pszFolder) == (DWORD)-1L)
		return FALSE;

	// Get the desktop folder, and get the ITEMIDLIST by calling ParseDisplayName on it.
	LPSHELLFOLDER pDesktopFolder;
	if (FAILED (SHGetDesktopFolder (&pDesktopFolder)))
		return FALSE;
	ULONG chEaten;
    HRESULT hr = pDesktopFolder->ParseDisplayName(NULL, NULL, T2OLE(pszFolder), 
							&chEaten, ppiidlistOut, NULL);
	pDesktopFolder->Release ();

	return SUCCEEDED(hr);
}

BEGIN_MESSAGE_MAP(CBrowseFolderDialog, CCommonDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

// These are not defined until SDK bits that support COMCTL32 version 4.71.

#ifndef BFFM_VALIDATEFAILED
#define BFFM_VALIDATEFAILEDA	3 
#define BFFM_VALIDATEFAILEDW	4 
#endif

// Selects the folder given by the name.

void 
CBrowseFolderDialog::SelectFolder(
LPCSTR pszFolder)
{
	ASSERT_VALID(this);
	ASSERT(pszFolder != NULL);
	SendMessage (BFFM_SETSELECTION, (WPARAM)TRUE, (LPARAM)pszFolder);
}

// Sets the message in the status area.

void 
CBrowseFolderDialog::SetStatusMessage(
LPCSTR pszMessage)
{
	ASSERT_VALID(this);
	ASSERT(pszMessage != NULL);
	SendMessage (BFFM_SETSTATUSTEXT, 0, (LPARAM)pszMessage);
}

// Called when the dialog is initialized. The base class implementation MUST
// be called if you want to set a title for the dialog or initially select a 
// folder.

void 
CBrowseFolderDialog::OnInitialized()
{
	if (!m_strTitle.IsEmpty ())
		SetWindowText (m_strTitle);
	if (!m_strSelectedFolder.IsEmpty ())
		SelectFolder (m_strSelectedFolder);
}

// Called when the user selects a different folder/selection. The folder is NULL
// if the user selects something that is not a file system folder.

void 
CBrowseFolderDialog::OnSelChanged(
LPCSTR pszSelectedFolder)
{
}

// Called when the user types something in the edit box that is not a valid folder,
// and presses OK. You will only get this callback with Common Control Version 4.71
// or higher. Return TRUE to keep the dialog open, FALSE to dismiss it.

BOOL 
CBrowseFolderDialog::OnInvalidChars(
LPCSTR pszTypedText)
{
	return TRUE;
}

// Callback procedure used for the dialog. Delegates callback messages to overridable
// handlers.

int CALLBACK
CBrowseFolderDialog::OurBrowseFolderCallbackProc(
HWND hwnd,
UINT uMsg,
LPARAM lParam,
LPARAM lpData)
{
    CBrowseFolderDialog* pDlg = DYNAMIC_DOWNCAST(CBrowseFolderDialog, CWnd::FromHandlePermanent(hwnd));
	int nRet = 0;
    if (pDlg != NULL)
	{
		switch (uMsg)
		{
			case BFFM_INITIALIZED:
				pDlg->OnInitialized ();
				break;
			case BFFM_SELCHANGED:
			{
				CString strFolder;
				if (lParam == 0 || !ConvertIIDListToFolderName ((LPITEMIDLIST)lParam, &strFolder))
					pDlg->OnSelChanged (NULL);
				else
					pDlg->OnSelChanged (strFolder);
				break;
			}
			case BFFM_VALIDATEFAILEDA:
				nRet = (int)pDlg->OnInvalidChars ((LPCSTR)lParam);
				break;
			case BFFM_VALIDATEFAILEDW:
			{
				USES_CONVERSION;
				nRet = (int)pDlg->OnInvalidChars (W2CA((LPCWSTR)lParam));
				break;
			}
		}
	}
	else 
	{
		nRet = (uMsg == BFFM_VALIDATEFAILEDA || uMsg == BFFM_VALIDATEFAILEDW) ? 1 : 0;
	}
	return nRet;
}

BOOL
CBrowseFolderDialog::OnHelpInfo(
HELPINFO*)
{
	return Default ();
}

// =================================================================================
// CBrowseFolderDialogEx implementation.

CBrowseFolderDialogEx::CBrowseFolderDialogEx(
LPCSTR pszTitle, 
LPCSTR pszDescription, 
LPCSTR pszFileDescr,
LPCSTR pszFileTypes,
LPCSTR pszRoot /* = NULL */,
LPCSTR pszInitialFolder /* = NULL*/,
CWnd* pParentWnd /*= NULL*/)
:
CBrowseFolderDialog (pszTitle, pszDescription, pszRoot, pszInitialFolder, 0, pParentWnd)
{
	if (pszFileDescr != NULL)
		m_strFileDescr = pszFileDescr;

	ASSERT (pszFileTypes != NULL);
	int n;
	LPCSTR psz;
	for (n = 0, psz = pszFileTypes; *psz != '\0'; n++, psz += lstrlen (psz) + 1);
	ASSERT (n > 0);
	
	m_nTypeCount = n;
	if ((m_pTypeInfo = (TYPEINFO *)malloc (n * sizeof(*m_pTypeInfo))) != NULL)
	{
		for (n = 0, psz = pszFileTypes; *psz != '\0'; n++, psz += lstrlen (psz) + 1)
		{
			m_pTypeInfo[n].pszType = psz;
			m_pTypeInfo[n].hicon = NULL;
			m_pTypeInfo[n].nImgID = -1;
		}
	}

	m_bTypingInEdit = FALSE;
}

CBrowseFolderDialogEx::~CBrowseFolderDialogEx()
{
	// Free icons.

	if (m_pTypeInfo != NULL)
	{
		for (int i = m_nTypeCount - 1; i >= 0; i--)
		{
			if (m_pTypeInfo[i].hicon != NULL)
				DestroyIcon (m_pTypeInfo[i].hicon);
		}
		free (m_pTypeInfo);
	}
}

BEGIN_MESSAGE_MAP(CBrowseFolderDialogEx, CBrowseFolderDialog)
	ON_EN_CHANGE(IDC_FILEPATH_EDIT, OnChangeFilePathEdit)
END_MESSAGE_MAP()

// Handles a click on an OK button. The OnOK handler for this class calls this function -
// however, if you derive from this class, you must call this function before any other
// processing. If this function returns FALSE, you should not process the message.
// This is used to handle the case where the user types a pathname and presses OK or Enter -
// the dialog changes to that folder rather than closing.

BOOL
CBrowseFolderDialogEx::HandleFileOK()
{
	if (!m_bTypingInEdit)
		return TRUE;
		
	CString strTypedFolder;
	m_wndPath.GetWindowText (strTypedFolder);

	LPITEMIDLIST piidlist;
	if (strTypedFolder.GetLength () > 0 && ConvertFolderNameToIIDList (strTypedFolder, &piidlist))
	{
		SendMessage (BFFM_SETSELECTION, (WPARAM)FALSE, (LPARAM)piidlist);
		GetShellMalloc ()->Free (piidlist);
		m_bTypingInEdit = FALSE;
	}
	else
	{
		MessageBeep ((UINT)-1L);
	}
	return FALSE;
}



// Modifies the dialog on initialization.

void
CBrowseFolderDialogEx::OnInitialized()
{
	// Find the treeview control.

	CWnd* pTreeView = NULL;
	HWND hwnd;
	char szClass[64];
	for (hwnd = ::GetWindow (m_hWnd, GW_CHILD); hwnd; hwnd = ::GetWindow (hwnd, GW_HWNDNEXT))
	{
		if (::GetClassName (hwnd, szClass, sizeof(szClass)) < 64 &&
				!lstrcmp (szClass, "SysTreeView32"))
		{
			pTreeView = CWnd::FromHandle (hwnd);
			break;
		}
	}
	if (pTreeView != NULL || m_pTypeInfo != NULL)
	{
		// Let's modify the dialog.
	
		pTreeView->ModifyStyle (0, TVS_SHOWSELALWAYS);

		CFont* pfont = GetFont ();

		CRect rectDlg;
		GetWindowRect (&rectDlg);
		
		CRect rectAdditional (0, 12, 8, 20);
		MapDialogRect (&rectAdditional);
		int cxBorder = rectAdditional.right / 2;
		int cyBorder = rectAdditional.Height () / 2;
		int cyEditHeight = rectAdditional.top;
	
		int cxAdjust = rectDlg.Width () / 2;
		int cyAdjust = rectAdditional.bottom;
		SetWindowPos (NULL, 
			0, 0, 
			rectDlg.Width () + cxAdjust, rectDlg.Height () + cyAdjust,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	
		AdjustControlPosition (IDOK, cxAdjust, cyAdjust);
		AdjustControlPosition (IDCANCEL, cxAdjust, cyAdjust);
	
	   	CRect rectTreeView;
		pTreeView->GetWindowRect (rectTreeView);
		ScreenToClient (rectTreeView);

		// Create an edit control.
		if (m_wndPath.CreateEx (WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY, "Edit",
								"", WS_CHILD | WS_VISIBLE | WS_TABSTOP | ES_AUTOHSCROLL,
								rectTreeView.left, rectTreeView.bottom + cyBorder,
								rectTreeView.Width (), cyEditHeight,
								m_hWnd, (HMENU)IDC_FILEPATH_EDIT))
		{
			m_wndPath.SetFont (pfont);
		}


		int cxFileWidth = cxAdjust - cxBorder;


		// Create a static control.
		CWnd* pWndDescription = GetWindow (GW_CHILD);
		CRect rectDescription;
		pWndDescription->GetWindowRect (rectDescription);
		ScreenToClient (rectDescription);

		if (m_wndFileDescr.CreateEx (WS_EX_NOPARENTNOTIFY, "Static",
									 m_strFileDescr, WS_CHILD | WS_VISIBLE | SS_LEFT, 
									 rectTreeView.right + cxBorder, rectDescription.top,
									 cxFileWidth, cyEditHeight, 
									 m_hWnd, (HMENU)-1L))
		{
		   	m_wndFileDescr.SetFont (pfont);
		}

		// Create a listview control.
		if (m_wndFileList.CreateEx (WS_EX_CLIENTEDGE | WS_EX_NOPARENTNOTIFY, "SysListView32",
							  "", WS_CHILD | WS_VISIBLE | WS_VSCROLL | 
							  LVS_ALIGNLEFT | LVS_REPORT | LVS_SORTASCENDING | LVS_NOCOLUMNHEADER, 
							  rectTreeView.right + cxBorder, rectTreeView.top,
							  cxFileWidth, rectTreeView.Height () + cyBorder + cyEditHeight,
							  m_hWnd, (HMENU)IDC_FILELIST_CTRL))
		{
			m_wndFileList.InsertColumn (0, "");
			if (m_imglist.Create (16, 16, TRUE, 1, 2))
			{
				m_wndFileList.SetImageList (&m_imglist, LVSIL_SMALL);
			}
		   	m_wndFileList.SetFont (pfont);
		}

		m_strPathInListCtrl.Empty ();
	}
	m_bTypingInEdit = FALSE;
	CBrowseFolderDialog::OnInitialized ();
}

// Handle a selection change, by updating the file list.

void
CBrowseFolderDialogEx::OnSelChanged(
LPCSTR pszSelectedFolder)
{
	if (m_wndPath.m_hWnd == NULL || m_wndFileList.m_hWnd == NULL)
		return;

	if (pszSelectedFolder != NULL)
	{
		m_wndPath.SetWindowText (pszSelectedFolder);
		if (m_strPathInListCtrl.CompareNoCase (pszSelectedFolder))
		{
			m_strPathInListCtrl = pszSelectedFolder;
			GenerateFileList ();
		}
	}
	else
	{
		m_wndPath.SetWindowText ("");
		m_strPathInListCtrl.Empty ();
		m_wndFileList.DeleteAllItems ();
	}

	m_bTypingInEdit = FALSE;
}

void CALLBACK 
CBrowseFolderDialogEx::OnEnumFileAdd(
LPARAM lParam, 
LPCSTR pszPath, 
LPCSTR pszFile, 
int nFileType)
{
	((CBrowseFolderDialogEx *)lParam)->EnumFileAdd (pszFile, nFileType);
}

void 
CBrowseFolderDialogEx::EnumFileAdd(
LPCSTR pszFile, 
int nFileType)
{
	// Make sure the icon is added to the image list.
	if (m_imglist.m_hImageList)
	{
		if (m_pTypeInfo[nFileType].hicon == NULL)
		{
			m_pTypeInfo[nFileType].hicon = GetFiletypeIcon (m_pTypeInfo[nFileType].pszType, TRUE);
			if (m_pTypeInfo[nFileType].hicon != NULL)
			{
				m_pTypeInfo[nFileType].nImgID = m_imglist.Add (m_pTypeInfo[nFileType].hicon);
			}
		}
	}

	m_wndFileList.InsertItem (0, pszFile, m_pTypeInfo[nFileType].nImgID);
}

// Generates list of files for current folder in the file list control.

void 
CBrowseFolderDialogEx::GenerateFileList()
{
	m_wndFileList.DeleteAllItems ();
	FILEENUMSTRUCT fileenum;
	fileenum.pszTypes = m_pTypeInfo[0].pszType;
	fileenum.pfnAdd = OnEnumFileAdd;
	fileenum.lParam = (LPARAM)this;
	fileenum.bRecursive = FALSE;
	EnumFiles (m_strPathInListCtrl, &fileenum);

	CRect rectClient;
	m_wndFileList.GetClientRect (rectClient);
	m_wndFileList.SetColumnWidth (0, rectClient.Width () - 1);
}

// Utility function to adjust the position of a control.

void
CBrowseFolderDialogEx::AdjustControlPosition(
UINT nID, 
int  cx, 
int  cy)
{
	CWnd* pWnd = GetDlgItem (nID);
	ASSERT (pWnd != NULL);
	CRect rectControl;
	pWnd->GetWindowRect (rectControl);
	ScreenToClient (rectControl);
	rectControl.OffsetRect (cx, cy);
	pWnd->SetWindowPos (NULL, rectControl.left, rectControl.top, 0, 0, 
		SWP_NOSIZE | SWP_NOACTIVATE | SWP_NOZORDER);
}

void 
CBrowseFolderDialogEx::OnChangeFilePathEdit()
{
	m_bTypingInEdit = TRUE;
}

void 
CBrowseFolderDialogEx::OnOK()
{
	if (HandleFileOK ())
	{
		CBrowseFolderDialog::OnOK ();
	}
}

// sndPlayMidiSound - sndPlaySound functionality for MIDI sounds. 
// Notes on sndPlaySound flags:
//		SND_ASYNC		Fully supported.
// 		SND_LOOP		Fully supported.
//		SND_MEMORY		Unsupported.
// 		SND_NODEFAULT	Always assumed - there is no default MIDI sound.
//		SND_NOSTOP		Partially supported. Even if this flag is not
//						specified, sndPlayMidiSound cannot play a MIDI
//						sound if a MIDI sound is already being played
//						by another application or by a function other
//						than sndPlayMidiSound.
//		SND_SYNC		Fully supported.
// Additional flags supported:
//		SND_SEMISYNC	Synchronizes to playback start, but not playback
//						completion. This causes the function to not return
//						until the sound has started playing. Useful for
//						very long files.


static CMciPlaybackThread * pPlaybackThread = NULL;
static CRITICAL_SECTION critsec;
static BOOL bCritsecInitialized;

// Auto cleanup.
class MIDISYSTEM
{
public:
	~MIDISYSTEM();
};
MIDISYSTEM::~MIDISYSTEM()
{
	if (bCritsecInitialized)
	{
		EnterCriticalSection (&critsec);
		if (pPlaybackThread != NULL)
			pPlaybackThread->Lock ();
		LeaveCriticalSection (&critsec);

		if (pPlaybackThread != NULL)
		{
			HANDLE h;
			BOOL bDup = DuplicateHandle (::GetCurrentProcess (), 
							pPlaybackThread->m_hThread,
							::GetCurrentProcess (),
							&h,
							0,
							FALSE,
							DUPLICATE_SAME_ACCESS);
			pPlaybackThread->Cleanup ();
			if (bDup)
			{
				WaitForSingleObject (h, 10000);
				CloseHandle (h);
			}
		}
	}
}
static MIDISYSTEM Midi;

BOOL
sndPlayMidiSound(
LPCSTR pszSound,
UINT fuSound)
{
	if (pszSound != NULL && GetFileAttributes (pszSound) == (DWORD)-1L)
		return FALSE;

	if (!bCritsecInitialized)
	{
		InitializeCriticalSection (&critsec);
		bCritsecInitialized = TRUE;
	}

	EnterCriticalSection (&critsec);
	if (pszSound == NULL && pPlaybackThread == NULL)
	{
		LeaveCriticalSection (&critsec);
		return TRUE;
	}
	BOOL bCreateNewThread = pPlaybackThread == NULL;
	if (!bCreateNewThread)
		pPlaybackThread->Lock ();
	LeaveCriticalSection (&critsec);
				 
	if (bCreateNewThread)
	{
		pPlaybackThread = (CMciPlaybackThread *)AfxBeginThread (RUNTIME_CLASS(CMciPlaybackThread),
													THREAD_PRIORITY_NORMAL, 0,
													CREATE_SUSPENDED);
		if (pPlaybackThread == NULL)
			return FALSE;
		if (!pPlaybackThread->StartupThread (&pPlaybackThread, &critsec))
		{
			delete pPlaybackThread;
			pPlaybackThread = NULL;
			return FALSE;
		}
	}

	BOOL bRet = FALSE;
	DWORD dwID = 0;
	BOOL bSynchronous = FALSE;
	BOOL bSemiSynchronous = FALSE;

	// OK, at this point we have a locked background thread.

	if ((fuSound & SND_NOSTOP) != 0 && pPlaybackThread->IsPlaying ())
		goto Done;

	bSynchronous = /*pszSound == NULL ||*/ (fuSound & (SND_ASYNC | SND_SEMISYNC)) == 0;
	bSemiSynchronous = (fuSound & SND_SEMISYNC) != 0;

	// Close any existing sound.
	dwID = pPlaybackThread->Stop ();
	if ((bSynchronous || bSemiSynchronous) && !pPlaybackThread->WaitForCompletion (dwID, 10000))
	{
		goto Done;
	}

	if (pszSound == NULL)
	{
		bRet = TRUE;
		goto Done;
	}

	dwID = pPlaybackThread->Play (MCI_DEVTYPE_SEQUENCER, pszSound, (fuSound & SND_LOOP) != 0);
	if (dwID == 0)
		goto Done;

	if (bSynchronous)
	{
		while (pPlaybackThread->IsPlaying ());
	}
	else if (bSemiSynchronous)
	{
		if (!pPlaybackThread->WaitForCompletion (dwID, 10000))
			goto Done;
	}
	
	bRet = TRUE;

Done:
	pPlaybackThread->UnLock ();
	return bRet;
}



// File enumeration stuff.

extern void FirstCharUpper(char *szName) inline;

BOOL bEnumFile(
const char *szDir, 
void *pvData) 
{	
	FILEENUMSTRUCT * pfileenum = (FILEENUMSTRUCT *)pvData;
	
	HANDLE hFind;
	WIN32_FIND_DATA fd;

	short cbLen = lstrlen (szDir);
	char szPattern[_MAX_PATH];
	lstrcpyn (szPattern, szDir, cbLen+1);
	if (OurMbsRChr (szDir, '\\') != szDir + cbLen - 1)
	{
		lstrcat (szPattern, "\\");
		cbLen++;
	}
	lstrcpy (szPattern + cbLen, pfileenum->pszSubFilterInternal);

	char szFName[_MAX_FNAME];
	char szExt[_MAX_EXT];
	int iType;
	LPCSTR pszType;

	hFind = FindFirstFile (szPattern, &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		do 
		{
			if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
			{
				_splitpath(fd.cFileName, NULL, NULL, szFName, szExt);
				if (szExt[0] == 0 || szExt[1] == 1)
					continue;
				CharLower (szExt + 1);
				for (iType = 0, pszType = pfileenum->pszTypes; 
					 *pszType;
					 iType++, pszType += lstrlen (pszType) + 1)
				{
					if (!lstrcmp (szExt + 1, pszType))
					{
						FirstCharUpper(szFName);			// REGISB 10/15/97
						pfileenum->pfnAdd (pfileenum->lParam, szDir, szFName, iType);
					}
				}
			}
		} while(FindNextFile (hFind, &fd));
		FindClose (hFind);
	}

	if (pfileenum->bRecursive)
	{
		lstrcpy (szPattern + cbLen, "*.*");
		char szRecDir[_MAX_PATH+_MAX_FNAME];	// safe
		lstrcpyn(szRecDir, szPattern, cbLen+1);
		LPSTR pszSubDir = szRecDir + cbLen;
		hFind = FindFirstFile(szPattern, &fd);
		if (hFind != INVALID_HANDLE_VALUE)
		{
			do 
			{
				if ((fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) && fd.cFileName[0] != '.')
				{
					lstrcpy(pszSubDir, fd.cFileName);
					bEnumFile(szRecDir, pvData);
				}
			} while(FindNextFile(hFind, &fd));
			FindClose(hFind);
		}
	}

	return FALSE;  // so search continues
}

void EnumFiles(
LPCSTR pszPath, 
FILEENUMSTRUCT * pfileenum)
{
	// Translate search options
	CString strNameFilter;
	if (pfileenum->pszSubFilter != NULL)
	{
		// Remove whitespaces and junk.
		strNameFilter = pfileenum->pszSubFilter;
		strNameFilter.TrimLeft ();
		strNameFilter.TrimRight ();
		int nSlash = strNameFilter.ReverseFind ('\\');
		if (nSlash != -1)
			strNameFilter = strNameFilter.Mid (nSlash + 1);
	}
	if (strNameFilter.IsEmpty ())
	{
		pfileenum->pszSubFilterInternal = "*.*";
	}
	else
	{
		// No dot implies *.*
		int nDot = strNameFilter.Find ('.');
		if (nDot == -1)
		{
			int nStar = strNameFilter.ReverseFind ('*');
			if (nStar != -1 && nStar == strNameFilter.GetLength () - 1)
				strNameFilter += ".*";
			else
				strNameFilter += "*.*";
		}
		pfileenum->pszSubFilterInternal = strNameFilter;
	}

	extern BOOL bForPath(const char *szPath, BOOL soundFunc(const char *, void *), void *pvData);
	bForPath (pszPath, bEnumFile, pfileenum);
}

// Needed to work with normal VC 4.1 libraries

#ifdef VC41BUILD
extern "C" char *  __cdecl _strdup(const char * psz)
{
	if (psz == NULL)
		return NULL;
	int nChars = lstrlen (psz) + 1;
	LPSTR pszCopy;
	if ((pszCopy = (LPSTR)malloc (nChars)) != NULL)
	{
		lstrcpy (pszCopy, psz);
	}
	return pszCopy;
}
#endif

// Replaces CFileDialog::GetFileName, which does not work where filename.ext is more
// than 63 characters, because of a bug in MFC.

CString 
GetFileNameFromFileDialog(
CFileDialog &fd)
{
	ASSERT(fd.m_hWnd == NULL);		// Just call GetFileName while in dialog.

	if (fd.m_ofn.lpstrFile[0] == '\0' || fd.m_ofn.nFileOffset == 0)
		return CString ("");
	else
	{
		return CString (fd.m_ofn.lpstrFile + fd.m_ofn.nFileOffset);
	}
}

BEGIN_MESSAGE_MAP(CWin4FontDialog, CFontDialog)
	ON_WM_HELPINFO()
END_MESSAGE_MAP()

BOOL
CWin4FontDialog::OnHelpInfo(
HELPINFO*)
{
	return Default ();
}

// StrFindSubString - Finds a substring in a string, with some options.

class CStringInfo : protected CObjectPtr<WORD>
{
public:
	BOOL CompileForString(LPCSTR pszString, int nStringLength, BOOL bWholeWord);
	BOOL IsDelimiter(int nBytePos)
		{
			return (m_pPtr[m_nStringLength + m_pPtr[nBytePos]] & 
				(C1_SPACE | C1_BLANK | C1_CNTRL | C1_PUNCT)) != 0;
		}
	BOOL IsCharStart(int nBytePos)
		{
			return nBytePos == 0 || m_pPtr[nBytePos - 1] < m_pPtr[nBytePos];
		}
protected:
	UINT m_nStringLength;
};

CStringInfo::CompileForString(
LPCSTR pszString, 
int nStringLength, 
BOOL bWholeWord)
{
	m_nStringLength = nStringLength; 

	// We need information about the string to look at. The buffer will hold
	// one set of WORDs which have character break information, and one set
	// of WORDS which may contain word break information, if needed.
	if (!Alloc (2 * nStringLength))
		return NULL;
	ZeroMemory (m_pPtr, 2 * nStringLength * sizeof(WORD));
	
	// Character breaks.
	UINT nByte, nChar;
	for (nByte = 0, nChar = 0; pszString[nByte] != '\0'; nByte++, nChar++)
	{
		m_pPtr[nByte] = nChar;
		if (IsDBCSLeadByte (pszString[nByte]))
			m_pPtr[++nByte] = nChar;
	}

	if (bWholeWord)
	{
		if (!GetStringTypeEx (GetUserDefaultLCID (), CT_CTYPE1, 
				pszString, nStringLength, m_pPtr + nStringLength))
			return FALSE;
	}

	return TRUE;
}

static CStringInfo StringInfo;


LPCSTR 
StrFindSubString(
LPCSTR pszFindIn, 
LPCSTR pszFind, 
BOOL bWholeWord, 
BOOL bIgnoreCase)
{
    if (pszFindIn == NULL || pszFind == NULL || *pszFindIn == '\0' || *pszFind == '\0')
    {
        return NULL;
    }

    int nFindInLength = lstrlen (pszFindIn);
    int nFindLength = lstrlen (pszFind);
    if (nFindInLength < nFindLength)
    {
        return NULL;
    }

	if (nFindInLength == nFindLength)
	{
		int n = bIgnoreCase ? lstrcmpi (pszFind, pszFindIn) : lstrcmp (pszFind, pszFindIn);
		return n == 0 ? pszFindIn : NULL;
	}

	// This is a true substring search (string to find is smaller than string to look for)

    if (bIgnoreCase)
    {
        CString strFindIn = pszFindIn;
        CString strFind = pszFind;
        strFindIn.MakeLower ();
        strFind.MakeLower ();
        return StrFindSubString (strFindIn, strFind, bWholeWord, FALSE);
    }

	if (!StringInfo.CompileForString (pszFindIn, nFindInLength, bWholeWord))
		return NULL;

    const BYTE * pbFindIn = (const BYTE *)pszFindIn;
    const BYTE * pbFind = (const BYTE *)pszFind;

    // Precompute distance table for all but the last character.
    int arr[256];
    memset (arr, 0, sizeof(arr));
    for (nFindLength = 0; pbFind[nFindLength + 1] != '\0'; nFindLength++)
    {
        arr[pbFind[nFindLength]] = nFindLength + 1;
    }
    nFindLength++;

    const BYTE * pbMatch = pbFindIn;
    int i;
    int nPos = 0;
    while (nPos + nFindLength <= nFindInLength)
    {
		// Skip over multi-byte trailers.
		if (!StringInfo.IsCharStart (nPos))
		{
			nPos++;
			pbMatch++;
			continue;
		}

        // For whole word search, check if it starts and ends on a word delimiter.
        if (bWholeWord &&
                ((nPos > 0 && !StringInfo.IsDelimiter (nPos - 1)) ||
                (pbMatch[nFindLength] != 0 && !StringInfo.IsDelimiter (nPos + nFindLength))))
        {
            nPos++;
            pbMatch++;
            continue;
        }

        // Do the R-to-L comparison
        for (i = nFindLength - 1; i >= 0; i--)
        {
            if (pbMatch[i] != pbFind[i])
            {
                break;
            }
        }

        if (i == -1)
        {
            return (LPCSTR)pbMatch;
        }
        else
        {
            // Skip
			if (arr[pbMatch[i]] == 0)
			{
				nPos += nFindLength;
				pbMatch += nFindLength;
			}
			else if (i > arr[pbMatch[i]] - 1)
			{
				nPos += i - arr[pbMatch[i]] + 1;
				pbMatch += i - arr[pbMatch[i]] + 1;
			}
			else
			{
				nPos += i + 1;
				pbMatch += i + 1;
			}
        }
    }

    return NULL;
}

// Create a unique filename, based on a path, a filename, and a filetype (without the .).
// Ideal for creating names for new shortcuts, because it creates filenames like
// "Shortcut.fil" and "Shortcut (2).fil", etc.


BOOL 
CreateUniqueFileName(
LPCSTR   pszPath, 
LPCSTR   pszFileName, 
LPCSTR 	 pszFileType, 
CString* pstrOut)
{
	char szFile[_MAX_PATH];
	LPSTR pszFileSuffix;
	int i = 1;
	int nLen = wsprintf (szFile, "%s\\%s.%s", pszPath, pszFileName, pszFileType);
	if (GetFileAttributes (szFile) != (DWORD)-1L)
	{
		// The file exists. Try and create an alternate version using a (n) prefix.
		LPSTR pszFileTrailer = szFile + nLen - lstrlen (pszFileType) - 1;
		i++;
		do
		{
			wsprintf (pszFileTrailer, " (%d).%s", i, pszFileType);
		} while (GetFileAttributes (szFile) != (DWORD)-1L && ++i < 500);
	}

	*pstrOut = szFile;
	return i < 500;
}


// CNCSMapStringToPtr class implementation (Non Case Sensitive) - used because the MFC CMapStringToPtr class is 
// case sensitive which is not appropriate for regular IRC nicknames
// This class uses the lowercase nicks as the keys instead of the nicks
BOOL CNCSMapStringToPtr::Lookup(LPCTSTR key, void*& rValue) const
{
	ASSERT(key);

	if (key[0] == '\'')
		return CMapStringToPtr::Lookup(key, rValue);
	
	CString strLowerKey = key;
	strLowerKey.MakeLower();
	return CMapStringToPtr::Lookup(strLowerKey, rValue);
}


void CNCSMapStringToPtr::SetAt(LPCTSTR key, void* newValue)
{
	ASSERT(key);

	if (key[0] == '\'')
	{
		CMapStringToPtr::SetAt(key, newValue);
		return;
	}

	CString strLowerKey = key;
	strLowerKey.MakeLower();
	CMapStringToPtr::SetAt(strLowerKey, newValue);
}


BOOL CNCSMapStringToPtr::RemoveKey(LPCTSTR key)
{
	ASSERT(key);

	if (key[0] == '\'')
		return CMapStringToPtr::RemoveKey(key);

	CString strLowerKey = key;
	strLowerKey.MakeLower();
	return CMapStringToPtr::RemoveKey(strLowerKey);
}

// Adjust positions/sizes of controls in a resizeable dialog.

BOOL
AdjustResizeableDlgCtls(
CWnd* pWnd,
RESIZEABLEDLGCTL* pCtls, 
UINT nNumCtls, 
CSize& sizeOld, 
CSize& sizeNew)
{
	ASSERT_VALID(pWnd);
	ASSERT(pCtls != NULL && nNumCtls > 0);

	int cx = sizeNew.cx - sizeOld.cx;
	int cy = sizeNew.cy - sizeOld.cy;

	if (cx == 0 && cy == 0)
		return TRUE;

	HDWP hdwp = BeginDeferWindowPos (nNumCtls);
	if (!hdwp)
		return FALSE;

	UINT iCtl;
	HWND hwndCtl;
	CRect rectCtl;
	UINT nFlags;
	CRect rect;
	for (iCtl = 0; iCtl < nNumCtls; iCtl++)
	{
		nFlags = SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE;
		hwndCtl = GetDlgItem (pWnd->m_hWnd, pCtls[iCtl].nID);
		ASSERT(hwndCtl != NULL);
		GetWindowRect (hwndCtl, &rect);
		pWnd->ScreenToClient (&rect);
		if (cx != 0 && (pCtls[iCtl].nFlags & RESIZECTL_ALIGNRIGHT) != 0)
		{
			if ((pCtls[iCtl].nFlags & RESIZECTL_ALIGNLEFT) != 0)
			{
				// Stretch fully.
				rect.right += cx;
				nFlags &= ~SWP_NOSIZE;
			}
			else
			{
				// Keep with right edge.
				rect.left += cx;
				rect.right += cx;
				nFlags &= ~SWP_NOMOVE;
			}
		}
		if (cy != 0 && (pCtls[iCtl].nFlags & RESIZECTL_ALIGNBOTTOM) != 0)
		{
			if ((pCtls[iCtl].nFlags & RESIZECTL_ALIGNTOP) != 0)
			{
				// Stretch fully.
				rect.bottom += cy;
				nFlags &= ~SWP_NOSIZE;
			}
			else
			{
				// Keep with bottom edge.
				rect.top += cy;
				rect.bottom += cy;
				nFlags &= ~SWP_NOMOVE;
			}
		}

		if ((nFlags & (SWP_NOSIZE | SWP_NOMOVE)) != (SWP_NOSIZE | SWP_NOMOVE))
		{
			hdwp = DeferWindowPos (hdwp, hwndCtl, NULL, rect.left, rect.top,
						rect.Width (), rect.Height (), nFlags);
			if (!hdwp)
				return FALSE;
		}
	}

	EndDeferWindowPos (hdwp);
	return TRUE;
}

// Multiple monitor support - modifies a rect so that it is visible on the screen.

BOOL g_bMonitorFunxLoaded = FALSE;
HANDLE (WINAPI* g_pMonitorFromRect)(LPCRECT, DWORD);
HANDLE (WINAPI* g_pGetMonitorInfo)(HANDLE, LPMONITORINFO);

void 
MakeRectVisibleOnScreen(
LPRECT prc)
{
	// First check if it is indeed visible.
	HDC hdc;
	hdc = GetDC (NULL);
	BOOL bVisible = RectVisible (hdc, prc);
	ReleaseDC (NULL, hdc);

	if (bVisible)
		return;


	if (!g_bMonitorFunxLoaded)
	{
		// Try to load monitor funx from User32 - this will provide multimon support.
		g_bMonitorFunxLoaded = TRUE;
		HINSTANCE hUser32 = GetModuleHandle ("USER32");
		if (hUser32 != NULL)
		{
			*(FARPROC*)&g_pMonitorFromRect = GetProcAddress (hUser32, "MonitorFromRect");
			*(FARPROC*)&g_pGetMonitorInfo = GetProcAddress (hUser32, "GetMonitorInfoA");
		}
	}

	RECT rcWork;
	if (g_pMonitorFromRect != NULL && g_pGetMonitorInfo != NULL)
	{
		HANDLE hMonitor = g_pMonitorFromRect (prc, MONITOR_DEFAULTTONEAREST);
		ASSERT(hMonitor != NULL);
		MONITORINFO mi;
		mi.cbSize = sizeof(mi);
		g_pGetMonitorInfo (hMonitor, &mi);
		rcWork = mi.rcWork;
	}
	else
	{
		SystemParametersInfo (SPI_GETWORKAREA, 0, &rcWork, 0);
	}

	int cxOffset, cyOffset;
	if (prc->right < rcWork.left)
		cxOffset = rcWork.left - prc->left;
	else if (prc->left > rcWork.right)
	{
		if (prc->right - prc->left >= rcWork.right - rcWork.left)
			cxOffset = -prc->left;
		else
			cxOffset = rcWork.right - prc->right;
	}
	else 
		cxOffset = 0;
	if (prc->bottom < rcWork.top)
		cyOffset = rcWork.top - prc->top;
	else if (prc->top > rcWork.bottom)
	{
		if (prc->bottom - prc->top >= rcWork.bottom - rcWork.top)
			cyOffset = -prc->top;
		else
			cyOffset = rcWork.bottom - prc->bottom;
	}
	else 
		cyOffset = 0;
	OffsetRect (prc, cxOffset, cyOffset);
}
