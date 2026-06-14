#ifndef __CHATUTILS_H__
#define __CHATUTILS_H__

// General utility functions, classes, etc.

#ifndef ON_MESSAGE_VOID
#define ON_MESSAGE_VOID(message, memberFxn) \
	{ message, 0, 0, 0, AfxSig_vv, \
		(AFX_PMSG)(AFX_PMSGW)(void (AFX_MSG_CALL CWnd::*)(void))memberFxn },
#endif

// Common dialog constants etc. not available in older version of commctl.h

#if !defined(_WIN32_IE) || (_WIN32_IE < 0x0300)

#define TCS_HOTTRACK 0x0040					// Tab control style - hot tracking

#define TB_SETEXTENDEDSTYLE (WM_USER + 84)	// Toolbar msg - set extended style
#define TBSTYLE_EX_DRAWDDARROWS 0x00000001	// Toolbar exstyle - support dropdown arrows

#define LVM_SETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 54)	// Listview msg - set extended style   
#define LVM_GETEXTENDEDLISTVIEWSTYLE (LVM_FIRST + 55)	// Listview msg - get extended style
#define LVS_EX_FULLROWSELECT    0x00000020 	// Listview exstyle - select full row

typedef struct _DllVersionInfo
{
    DWORD cbSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformID;
}DLLVERSIONINFO;
typedef HRESULT (CALLBACK* DLLGETVERSIONPROC)(DLLVERSIONINFO *);

#endif

#if !defined(_WIN32_IE) || (_WIN32_IE < 0x0400)

#define RB_IDTOINDEX (WM_USER + 16)			// Rebar msg - get bar index from ID
#define RB_SHOWBAND  (WM_USER + 35)			// Rebar msg - show/hide band.

#define RBS_DBLCLKTOGGLE		0x8000		// Rebar style - min/max on double click
#define RBBS_GRIPPERALWAYS 		0x80		// Rebar band style - always show gripper

#endif

// General functions.

#define DLLVER(maj, min) MAKELONG(min, maj)
DWORD GetDllVersion(HINSTANCE hinst, DWORD dwDefaultVersion);
DWORD GetDllVersion(LPCSTR pszDLL, DWORD dwDefaultVersion);

BOOL GetWindowsMediaDirectory(CString * pstrDirectory);
HICON GetFiletypeIcon(LPCSTR pszFiletype, BOOL bSmallIcon);
CString GetFileNameFromFileDialog(CFileDialog &fd);
LPCSTR StrFindSubString(LPCSTR pszFindIn, LPCSTR pszFind, BOOL bWholeWord, BOOL bIgnoreCase);
BOOL QueryNewPaletteCommon(CWnd* pWnd);
BOOL CreateUniqueFileName(LPCSTR pszPath, LPCSTR pszFileName, LPCSTR pszFileType, CString* pstrOut);

// Generic file enumeration stuff. Will enumerate files of one or more types over a 
// path given by "dir0; dir1; dir2" etc. For those acquainted with the old bForPath
// function, this is a generalization.
//
// Note on pszSubFilter:
// pszSubFilter provides a generic way of restricting the enumeration.
// Examples (assume pszTypes="wav\0mid\0" in each case):
// pszSubFilter=NULL		=> returns *.wav and *.mid
// pszSubFilter="*.*" 		=> returns *.wav and *.mid
// pszSubFilter="*.wav" 	=> returns *.wav
// pszSubFilter="a*.*" 		=> returns a*.wav and a*.mid
// pszSubFilter="*.dat"		=> returns nothing
// pszSubFilter="a*."		=> returns nothing
// pszSubFilter="a"		    => returns a*.wav and a*.mid
// pszSubFilter="a*"		=> returns a*.wav and a*.mid
// Notice that specifying a . in the subfilter restricts the enumeration to files
// with no filetype, while omitting the . assumes *.* suffix (files that start with the filter) 

struct FILEENUMSTRUCT
{
	FILEENUMSTRUCT() { ZeroMemory (this, sizeof(*this)); }
	LPCSTR pszTypes; // Doubly null terminated list e.g. "wav\0mid\0", must be lowercase
	void (CALLBACK * pfnAdd)(LPARAM lParam, LPCSTR pszPath, LPCSTR pszFile, int nFileType);
	LPARAM lParam; // User-defined
	BOOL bRecursive;
	LPCSTR pszSubFilter;

	// Internal members - do not modify.
	LPCSTR pszSubFilterInternal;
};
void EnumFiles(LPCSTR pszPath, FILEENUMSTRUCT * pfileenum);

// Common Control version stuff.
#define COMCTLVER(maj, min) DLLVER(maj, min)
DWORD GetComCtlVersion();
BOOL IsComCtlNewerThan(DWORD dwVersion);

// Countof definition
#ifndef _countof
#define _countof(x) (sizeof(x) / sizeof((x)[0]))
#endif

// These macros eliminate the need for all sorts of casting

#define OurMbsChr(s,c) ((LPCSTR)_mbschr ((const UCHAR *)(LPCSTR)(s), c))
#define OurMbsRChr(s,c) ((LPCSTR)_mbsrchr ((const UCHAR *)(LPCSTR)(s), c))
#define OurMbsPbrk(s,s2) ((LPCSTR)_mbspbrk ((const UCHAR *)(LPCSTR)(s), (const UCHAR *)(LPCSTR)(s2)))
#define OurMbsStr(s,s2) ((LPCSTR)_mbsstr ((const UCHAR *)(LPCSTR)(s), (const UCHAR *)(LPCSTR)(s2)))

// sndPlaySound for MIDI files. Accepts the same API as sndPlaySound,
// although some flags are not supported (see implementation for details).

#define SND_SEMISYNC	0x0040

BOOL sndPlayMidiSound(LPCSTR pszSound, UINT fuSound);


// A simple class to do linked lists without using MFC overhead. Also 
// includes a template that provides a typesafe MFC CList-like class
// with little extra overhead.
// To work ideally, classes should derive from CListObject,
// and a separate list class should be derived from the CSimpleList template
// class. e.g.
//		class CMyObject : protected CListObject {....};
//		class CMyObjectList : public CNonMFCList<CMyObject> {};
// The overhead for this list support is five functions (plus some inline code),
// plus four bytes for each object and four additional bytes for each list.
// Note: if you derive from CListObject, the derived class must have a virtual
// destructor, if you want to use either the FreeList function or a CNonMFCList
// based on that class. This is because FreeList deletes elements itself.

class CListObject
{
public:
	CListObject() 
		{ }
	virtual ~CListObject()
		{ }
	void AddToListHead(CListObject* * pList);
	void AddToListTail(CListObject* * pList);
	void RemoveFromList(CListObject* * pList);
	void FreeList();
	int GetListCount();
	CListObject* GetNextInList() 
		{ return m_pNextInList; }
protected:
	CListObject* m_pNextInList;
};

template<class TYPE> class CNonMFCList
{
public:
	CNonMFCList()
		{ m_pList = NULL; }
	~CNonMFCList()
		{ m_pList->FreeList (); }
	inline void AddHead(TYPE* pItem)
		{ ((CListObject*)(PVOID)pItem)->AddToListHead (&m_pList); }
	void AddTail(TYPE* pItem)
		{ ((CListObject*)(PVOID)pItem)->AddToListTail (&m_pList); }
	void Remove(TYPE* pItem)
		{ ((CListObject*)(PVOID)pItem)->RemoveFromList (&m_pList); }
	TYPE* GetHead()
		{ return (TYPE*)(PVOID)m_pList; }
	TYPE* GetNext(TYPE* pOf)
		{ return (TYPE*)(PVOID)((CListObject*)(PVOID)pOf)->GetNextInList (); }
	BOOL IsEmpty()
		{ return m_pList == NULL; }
	int GetCount()
		{ return m_pList->GetListCount (); }

protected:
 	CListObject* m_pList;
};

#define DECLARE_LISTSUPPORT(listclass, objclass) \
	class listclass : public CNonMFCList<objclass> { };

// CSimpleComboBox - a very minimal class derived from CComboBox, to be used
// for CBS_SIMPLE type combo boxes. Why is this useful? Because simple combo boxes
// look pretty stupid when they're disabled, since list boxes are not grayed
// when disabled. This rectifies the problem with a simple WM_CTLCOLOR handler.

class CSimpleComboBox : public CComboBox
{
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

// Simple data encryption/decryption. The encrypted version of the data is always
// 2n+4 bytes long, where n is the length of the decrypted data.

void encEncodeData(PBYTE pbDataOut, PBYTE pbDataIn, UINT nDecryptedSize);
BOOL encDecodeData(PBYTE pbDataOut, PBYTE pbDataIn, UINT nDecryptedSize);

// A combo box that supports icons. To use this combo box, place a normal 
// combo box in your dialog, create a member variable of this class, and 
// call ReplaceControl to create the combo box, passing the parent window
// and the ID of the normal combo box. This will replace the normal one with
// an iconic one, and attach it to the member variable. Alternatively, you
// could call Create just as you would for a normal combo box, but make sure
// you pass the CBS_OWNERDRAWFIXED style, and set everything else up as a 
// dialog would (setting the correct font, etc.) Because of the added complexity,
// this method is not recommended.
// NOTE: You can't just put an ownerdraw combobox in your dialog template
// and attach it to a CIconicComboBox, because the WM_MEASUREITEM message
// never gets properly reflected.
// This combobox also supports drawing division markers between entries. To
// use this feature, override the ShouldDrawDivision member.

class CIconicComboBox : public CComboBox
{
public:
	CIconicComboBox();
	virtual ~CIconicComboBox();
	
	BOOL ReplaceControl(CWnd* pParentWnd, UINT nID, DWORD dwAdditionalStyles = 0);

protected:
	// Override these functions to provide icons, division markers, etc.
	virtual HICON GetIcon(UINT nIndex, LPCSTR pszString, DWORD dwItemData);
	virtual BOOL ShouldDrawDivision(UINT nIndex, LPCSTR pszString, DWORD dwItemData);
    
	afx_msg void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	afx_msg void MeasureItem(LPMEASUREITEMSTRUCT lpMeasureItemStruct);
	DECLARE_MESSAGE_MAP()
};

// Automatically loaded/unloaded icon. Simple class with almost all inline functions.

class CIcon
{
public:
	CIcon()
		{ m_hicon = NULL; }
	CIcon(UINT nIconID) 
		{ m_hicon = NULL; LoadIcon (nIconID); }
	~CIcon() 
		{ if (m_hicon) DestroyIcon (m_hicon); }
	operator HICON() 
		{ return m_hicon; }
	virtual BOOL Draw(CDC* pDC, int x, int y) 
		{ ASSERT (m_hicon); return pDC->DrawIcon (x, y, m_hicon); }
	virtual BOOL LoadIcon(UINT nIconID) 
		{ return m_hicon || (m_hicon = ::LoadIcon (::AfxGetInstanceHandle (), MAKEINTRESOURCE(nIconID))) != NULL; }
protected:
	HICON m_hicon;
};

class CSmallIcon : public CIcon
{
public:
	virtual BOOL Draw(CDC* pDC, int x, int y) 
		{ ASSERT (m_hicon); return ::DrawIconEx (pDC->m_hDC, x, y, m_hicon, 0, 0, 0, NULL, DI_NORMAL); }
	virtual BOOL LoadIcon(UINT nIconID) 
		{ return m_hicon || (m_hicon = (HICON)::LoadImage (::AfxGetInstanceHandle (), MAKEINTRESOURCE(nIconID), IMAGE_ICON, 16, 16, 0)) != NULL; }
};

// Browse For Folder dialog. Displays a dialog to browse for a folder. Only
// works with file system folders. 

class CBrowseFolderDialog : public CCommonDialog
{
	DECLARE_DYNAMIC(CBrowseFolderDialog)

public:
	CBrowseFolderDialog(LPCSTR pszTitle = NULL, 
						LPCSTR pszDescription = NULL,
						LPCSTR pszRoot = NULL,
						LPCSTR pszInitialFolder = NULL,
						UINT uFlags = BIF_RETURNONLYFSDIRS,
						CWnd* pParentWnd = NULL);
	virtual ~CBrowseFolderDialog();

	// Operations.
	virtual int DoModal();
	CString GetSelectedFolder() const
		{ return m_strSelectedFolder; }
	void SelectFolder(LPCSTR pszFolder);
	void SetStatusMessage(LPCSTR pszMessage);

	// Use this to directly access the BROWSEINFO structure.
	BROWSEINFO m_bi;

protected:
	// Overridables. Derived classes can override these. 
	virtual void OnInitialized();
	virtual void OnSelChanged(LPCSTR pszSelectedFolder);
	virtual BOOL OnInvalidChars(LPCSTR pszTypedText);

	// Utility functions.
	static BOOL ConvertIIDListToFolderName(LPITEMIDLIST piidlist, CString * pstrFolderOut);
    static BOOL ConvertFolderNameToIIDList(LPCSTR pszFolder, LPITEMIDLIST* ppiidlistOut);
    static int CALLBACK OurBrowseFolderCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData);
   
	CString    m_strTitle;
	CString    m_strSelectedFolder;
	char	   m_szDisplayName[_MAX_PATH];

	afx_msg BOOL OnHelpInfo(HELPINFO*);
	DECLARE_MESSAGE_MAP()
};

// Derivation of Browse For Folder dialog. Shows a directory list on the left, a list
// of files on the right, and contains an edit control for changing the path yourself.

class CBrowseFolderDialogEx : public CBrowseFolderDialog
{
public:
	CBrowseFolderDialogEx(LPCSTR pszTitle, 
						  LPCSTR pszDescription, 
						  LPCSTR pszFileDescr,
						  LPCSTR pszFileTypes,
						  LPCSTR pszRoot = NULL,
						  LPCSTR pszInitialFolder = NULL,
						  CWnd* pParentWnd = NULL);
	virtual ~CBrowseFolderDialogEx();

protected:
	virtual void OnInitialized();
	virtual void OnSelChanged(LPCSTR pszSelectedFolder);

	void AdjustControlPosition (UINT nID, int cx, int cy);
	void GenerateFileList();
	BOOL HandleFileOK();
	static void CALLBACK OnEnumFileAdd(LPARAM lParam, LPCSTR pszPath, LPCSTR pszFile, int nFileType);
	void EnumFileAdd(LPCSTR pszFile, int nFileType);
	
	struct TYPEINFO
	{
		LPCSTR pszType;
		HICON hicon;
		int nImgID;
	};

	enum IDs
	{
		IDC_FILEPATH_EDIT = 100,
		IDC_FILELIST_CTRL = 101,
	};
	CImageList m_imglist;
	CEdit m_wndPath;
	CStatic m_wndFileDescr;
	CListCtrl m_wndFileList;
	CString m_strFileDescr;
	TYPEINFO* m_pTypeInfo;
	int m_nTypeCount;
	CString m_strPathInListCtrl;
	BOOL m_bTypingInEdit;

	virtual void OnOK();
	afx_msg void OnChangeFilePathEdit();
	DECLARE_MESSAGE_MAP()
};

// Font dialog derivative. Supports WM_HELP message in MFC 4.1 and earlier.
// These versions of MFC automatically translate WM_HELP messages to old style
// context menu messages for all dialogs, including the common font dialog.
// We need to override this behavior, and call Default(), which will display
// the right help string.

class CWin4FontDialog : public CFontDialog
{
public:
	CWin4FontDialog(LPLOGFONT lplfInitial = NULL,
		DWORD dwFlags = CF_EFFECTS | CF_SCREENFONTS,
		CDC* pdcPrinter = NULL,
		CWnd* pParentWnd = NULL) : CFontDialog(lplfInitial, dwFlags, pdcPrinter, pParentWnd)
		{ };

protected:
	afx_msg BOOL OnHelpInfo(HELPINFO*);
	DECLARE_MESSAGE_MAP()
};

// Self-cleanup storage class. Can hold one or more entries of a given type. The class
// uses malloc and free to create the entries, so don't use this class with types that
// have constructors/destructors.
// Doesn't have all the functionality of, say, CArray, but this class is really tiny.

template<class TYPE> class CObjectPtr
{
public:
	CObjectPtr()
		{ m_pPtr = NULL; }
	~CObjectPtr()
		{ free (m_pPtr); }
	BOOL Alloc(UINT nCount);
	operator TYPE*()
		{ ASSERT(m_pPtr != NULL); return m_pPtr; }
	TYPE& operator[](UINT nIndex)
		{ ASSERT(m_pPtr != NULL && nIndex < m_nCountAlloc); return m_pPtr[m_nCountAlloc]; }
protected:
 	TYPE* m_pPtr;
	UINT  m_nCountAlloc;
};

template<class TYPE>
inline BOOL 
CObjectPtr<TYPE>::Alloc(UINT nCount)
{
	ASSERT(nCount > 0);
	if (nCount <= m_nCountAlloc)
		return TRUE;
	TYPE* pPtr = (m_pPtr != NULL) ? (TYPE*)realloc (m_pPtr, nCount * sizeof(TYPE))
								  : (TYPE*)malloc (nCount * sizeof(TYPE));
	if (pPtr)
		m_pPtr = pPtr;
	return pPtr != NULL;
}


class CNCSMapStringToPtr : public CMapStringToPtr
{
public:
	BOOL Lookup(LPCTSTR key, void*& rValue) const;
	void SetAt(LPCTSTR key, void* newValue);
	BOOL RemoveKey(LPCTSTR key);
};

// Generic resizing stuff.

#define RESIZECTL_ALIGNLEFT			1
#define RESIZECTL_ALIGNTOP			2
#define RESIZECTL_ALIGNRIGHT		4
#define RESIZECTL_ALIGNBOTTOM		8
#define RESIZECTL_STRETCHHORZ		(RESIZECTL_ALIGNLEFT | RESIZECTL_ALIGNRIGHT)
#define RESIZECTL_STRETCHVERT		(RESIZECTL_ALIGNTOP | RESIZECTL_ALIGNBOTTOM)

struct RESIZEABLEDLGCTL
{
	UINT nID;
	UINT nFlags;
};

BOOL AdjustResizeableDlgCtls(CWnd* pWnd, RESIZEABLEDLGCTL* pCtls, UINT nNumCtls, CSize& sizeOld, CSize& sizeNew);

// Multiple monitor support.

#ifndef SM_CMONITORS

typedef struct tagMONITORINFO
{
	DWORD cbSize;
	RECT rcMonitor;
	RECT rcWork;
	DWORD dwFlags;
} MONITORINFO, *LPMONITORINFO;

#define MONITOR_DEFAULTTONEAREST 0x2

#endif

void MakeRectVisibleOnScreen(LPRECT prc);

#endif
