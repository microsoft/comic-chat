// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

//--------------------------------------------------------------------------------------------
//
//	Copyright (c) Microsoft Corporation, 1996
//
//	Description:
//
//		Microsoft Internet Chat
//		Routines and objects in this file maintain chat UI objects and their
//		properties.
//
//
//	Authors:
//		Umesh Madan 
//		URL recognition code based on original by David Fulmer. davidfu@microsoft.com
//
//--------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------

#include "stdafx.h"
#include "url.h"
#include "ui.h"
#if 0
//#include "icpch.h"
//#include "icwprop.h"
//#include "icmain.h"
#endif


//--------------------------------------------------------------------------------------------
//
// GLOBALS
//
//--------------------------------------------------------------------------------------------
TCHAR gszTerm[] = "\r\n\0";


//
// mapping to check for legal/illegal chars in URLs
//
const BYTE MPCH_FLEGALFORURL[] = 
{
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 0
	0x00, 0x02, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 16
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,

	0x02, 0x11, 0x02, 0x13,   0x01, 0x03, 0x01, 0x01,	// 32
	0x01, 0x11, 0x01, 0x01,   0x11, 0x01, 0x11, 0x03,

	0x01, 0x01, 0x01, 0x01,   0x01, 0x01, 0x01, 0x01,	// 48
	0x01, 0x01, 0x01, 0x01,   0x02, 0x01, 0x02, 0x13,


	0x01, 0x01, 0x01, 0x01,   0x01, 0x01, 0x01, 0x01,	// 64
	0x01, 0x01, 0x01, 0x01,   0x01, 0x01, 0x01, 0x01,

	0x01, 0x01, 0x01, 0x01,   0x01, 0x01, 0x01, 0x01,	// 80
	0x01, 0x01, 0x01, 0x02,   0x03, 0x02, 0x03, 0x01,

	0x03, 0x01, 0x01, 0x01,   0x01, 0x01, 0x01, 0x01,	// 96
	0x01, 0x01, 0x01, 0x01,   0x01, 0x01, 0x01, 0x01,

	0x01, 0x01, 0x01, 0x01,   0x01, 0x01, 0x01, 0x01,	// 112
	0x01, 0x01, 0x01, 0x02,   0x03, 0x02, 0x13, 0x00,


	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 128
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 144
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 160
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 176
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,


	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 192
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 208
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 224
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,

	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,	// 240
	0x00, 0x00, 0x00, 0x00,   0x00, 0x00, 0x00, 0x00,
};

//
// Universal URL seperator -> seperates protocol and the address of the resource
//
const TCHAR SZURLSEP[] = ":";
const TCHAR SZMICPROTOCOL[] = "mic";
//
// ordered by match probability
//
const TCHAR SZURLPREFIXS[] =
	"mic"	 "\0"
	"http"   "\0"
	"file"   "\0"
	"ftp"    "\0"
	"news"   "\0"
	"mailto" "\0"
	"https"  "\0"
	"gopher" "\0"
	"telnet" "\0"
	"nntp"   "\0"
	"wais"   "\0"
	"prospero\0\0";

#define CCHURLPREFIXMOST 28

const TCHAR REG_SZBROWSER[] = "Browser";	

const int	CCHMICPROTOCOL = 3;

COLORREF	linkColor = RGB(0, 0, 255);

#if 0

//--------------------------------------------------------------------------------------------
//
// FUNCTIONS
//
//--------------------------------------------------------------------------------------------
BOOL FHandleIfMicURL(HWND hWndParent,TCHAR *szURL)
{
	//
	// MIC URLS are of the form mic:\\ OR mic:
	//
	TCHAR	szProtocol[CCHMICPROTOCOL + 2];
	int		cch;
	//
	// Copy first 4 chars
	//			
	cch = CCHMICPROTOCOL + 1;
	::lstrcpyn(szProtocol,szURL,cch);
	szProtocol[cch] = '\0';
	//
	// Is it MIC
	//
	if (0 != ::lstrcmpi(SZMICPROTOCOL,szProtocol))
		{
		return FALSE;	// not MIC
		}
	
	return FJoinChannelFromURL(hWndParent,szURL);	
}

BOOL FMakeMicURL(CHAR *szServer,CHAR *szChannel,CHAR *szURL)
{
	Assert(szURL && szChannel && szServer);
	//
	// attach the server to the URL
	//	
	::wsprintf(szURL,"%s://%s/",SZMICPROTOCOL,szServer);
	// 
	// Copy the channel name.. encoding as necessary
	//
	TCHAR *pch;
	TCHAR *pchURL;
	CHAR  szSpecial[4];
	int	cch,i;
	
	pchURL = szURL + lstrlen(szURL);	
	pch = szChannel;

	while (*pch)
		{
		const BYTE fb = MPCH_FLEGALFORURL[*pch];
		//
		// Is this a special character?
		//
		if (0 != (fb & 0x02))
			{
			wsprintf(szSpecial,"%s%lx","%",(int)*pch);
			cch = lstrlen(szSpecial);
			i = 0;
			while (i < cch)
				{
				*pchURL++ = szSpecial[i++];
				}
			}		
		else
			{
			*pchURL++ = *pch;
			}
		pch++;
		}

	*pchURL = '\0';

	return TRUE;			
}

#endif

//
// Is the given STRING a URL? 
//
BOOL FIsURL(TCHAR *szURL,int *pISep,int *pcchURL)
{
	if (!szURL)
		{
		return FALSE;
		}
	
	int		cchURL;
	int		iSep;
	BOOL	fRet = FALSE;

	cchURL = lstrlen(szURL);
	if (cchURL <= 0)
		{
		return FALSE;
		}
	//
	// Find the color seperator
	//			
	for (iSep = 0;iSep < cchURL; ++iSep)
		{
		if (':' == szURL[iSep])
			{
			break;
			} 
		}
	if (iSep <= 0 || iSep >= CCHURLPREFIXMOST || ((iSep + 1) >= cchURL) )
		{
		//
		// URL starts with or does not have a COLON. OR, the stuff to the left of the colon
		// is just too big..unreasonable. OR ends with a colon. Bad URL
		//
		return FALSE;
		}
	//
	// Now make sure that the stuff to the left of the colon is a KNOWN prefix
	//
	TCHAR *pszURLPrefixs;
	int	isz;
	
	pszURLPrefixs	= (LPTSTR) SZURLPREFIXS;
	//
	// Replace colon with a '\0', to make comparison easy
	//
	szURL[iSep] = '\0';
	for (isz = 0; NULL != pszURLPrefixs[isz]; isz += (lstrlen(pszURLPrefixs + isz) + 1) )
		{
		//
		// URL protocol types are not case-sensitive
		//
		if (0 == lstrcmpi(szURL,&pszURLPrefixs[isz]))
			{
			szURL[iSep] = ':';
			goto LMatch; // got one
			}
		}
	
	goto LReturn;

LMatch:	
	//
	// Now look to the right of the colon
	//
	TCHAR	*pch;
	int		/*iStart,*/ iEnd,iLast;
	
	iEnd	= iSep;
	iLast	= iSep;
	pch		= &szURL[iLast];	
	for (pch; *pch; ++pch,++iEnd)
		{
		const BYTE fb = MPCH_FLEGALFORURL[*pch];

		if (!fb || !(fb & 0x01))
			{
			//
			// Anything set to NULL in the Map is an illegal char
			//
			return FALSE;
			}
		
		if (!(fb & 0x10))
			{
			//
			// Not a terminating char
			//
			iLast = iEnd + 1;
			}
		}
	
	if (iLast != iSep + 1)
		{
		fRet = TRUE;
		}

LReturn:
	
	if (pISep)
		{
		*pISep = iSep;
		}
	
	if (pcchURL)
		{		
		*pcchURL = cchURL;
		}
				
	szURL[iSep] = ':';	
				
	return fRet;	// valid URL
}

#if 0

int IHexToInteger(TCHAR ch)
{
	if ('0' <= ch || ch <= '9')
		{
		return ch - '0';
		}
	
	if ('a' <= ch || ch <= 'f')
		{
		return ((ch - 'a') + 10);
		}

	if ('A' <= ch || ch <= 'F')
		{
		return ((ch - 'A') + 10);
		}
	
	return -1;
}

BOOL FCopyURL(TCHAR *szURL,TCHAR **ppszCopy)
{
	Assert(ppszCopy);

	int iSep;
	int cchURL,cchCopy;
	BOOL fRet = FALSE;
	TCHAR *szCopy = NULL;

	if (!FIsURL(szURL,&iSep,&cchURL))
		{
		return FALSE;
		}
	cchCopy = cchURL;
	szCopy = new TCHAR[cchCopy + 1];
	if (!szCopy)
		{
		AssertGLE(FALSE);
		DoOOM();
		return FALSE;
		}
	//
	// Copy it, replacing special chars 
	//
	int i,iCopy;
	int	ich,ich2;

	for (i = 0,iCopy = 0; iCopy < cchCopy; ++iCopy)
		{
		if ('%' == szURL[i])
			{
			//
			// get the embedded Hex code and turn it into an integer
			// This is really lame, but bug free :-)
			//
			ich = 0;
			ich = IHexToInteger(szURL[++i]);
			if (ich > -1)
				{
				ich2 = IHexToInteger(szURL[++i]);
				if (ich2 > -1)
					{
					ich *= 16;
					ich += ich2;
					++i;
					}
				szCopy[iCopy++] = (TCHAR)ich;
				}
			}
		//
		// Copy copy
		//
		szCopy[iCopy] = szURL[i++];
		}
	
	fRet = TRUE;

	if (fRet)
		{
		*ppszCopy = szCopy;
		}
	else 
		{
		delete [] szCopy;
		}

	return fRet;
}


//--------------------------------------------------------------------------------------------
//
// CLASSES
//
//--------------------------------------------------------------------------------------------
//
// RichEdit OLE callbacks
//
CChatRichEditCallBack::CChatRichEditCallBack(LPUNKNOWN punkOuter,PFNDESTROYED pfndestroy) 
			: CTCRichEditCallback(punkOuter,pfndestroy)

{
} 

STDMETHODIMP CChatRichEditCallBack::GetClipboardData(CHARRANGE *lpchrg,DWORD reco,
														LPDATAOBJECT *lplpdataobj)
{
	switch(reco)
		{
		case RECO_COPY:
			
			Assert(m_pui);

			if (m_pui->FCopyAvailable())
				{
				return ResultFromScode(S_FALSE);
				}
			break;
			
		case RECO_CUT:
			return ResultFromScode(S_FALSE);
			
		case RECO_PASTE:
			Assert(FALSE);
			return ResultFromScode(S_FALSE);

		}

	return ResultFromScode(E_NOTIMPL);
}

STDMETHODIMP CChatRichEditCallBack::GetNewStorage(LPSTORAGE *lplpstg)
{
	Assert(m_pui);

	return m_pui->HrStorageGet(lplpstg);
}

void CChatRichEditCallBack::SetPUI(CUIRichEdit *pui)
{
	Assert(pui);
	Assert(NULL == m_pui);

	m_pui = pui;
}

//
// Chat Generic RichEdit object
//
CUIRichEdit::CUIRichEdit(int idName)
			:CChatChildWnd()

{
	m_hInstRichEdit	= NULL;

	m_hFile			= INVALID_HANDLE_VALUE;

	::ZeroMemory(&m_ccf,sizeof(COLORCHARFORMAT));
	m_ccf.cformat.cbSize = sizeof(CHARFORMAT);
	m_cBold			= 0;
	m_idName 		= idName;
	m_szName[0]		= '\0';
	m_fReadOnly		= TRUE;	
	m_fRestore		= FALSE;
	//
	// OLE objects
	//
	m_pREOle		= NULL;
	m_pStorage		= NULL;
	m_cdwItem		= 0;
	//m_puiDragDrop	= NULL;
	//
	// URLS
	//
	m_pszURLPrefixs	= (LPTSTR) SZURLPREFIXS;
	m_crLink		= RGB(0,0,255); // BLUE by default
	m_fFindURLS		= TRUE;
	m_szBrowserPath[0] = '\0';
	m_hCursor		= NULL;
	//
	// Subclass
	//
	m_wndProc		= NULL;
}

CUIRichEdit::~CUIRichEdit(void)
{
	if (INVALID_HANDLE_VALUE == m_hFile)
		{
		::CloseHandle(m_hFile);
		}	
	if (m_pREOle)
		{
		m_pREOle->Release();
		}	
	if(m_pStorage)
		{
		m_pStorage->Release();
		}
	if (m_hInstRichEdit)
		{
		::FreeLibrary(m_hInstRichEdit);
		}
	/*
	if (m_puiDragDrop)
		{
		m_puiDragDrop->Release();
		}
	*/
}

BOOL CUIRichEdit::FCreate(HWND hWndParent,int idCtl,RECT *prc,BOOL fAutoURL,BOOL fAutoScroll)
{
	Assert(NULL == m_hInstRichEdit);	// already loaded?
	//
	// Make sure that RichEdit DLL is loaded
	//
	m_hInstRichEdit = LoadLibrary("RICHED32.DLL");
	if (NULL == m_hInstRichEdit)
		{
		AssertGLE(FALSE);
		return (FALSE);
		}

	DWORD dwStyle;

	m_fFindURLS = fAutoURL;
	dwStyle = fAutoScroll ? (ES_AUTOVSCROLL | ES_SUNKEN) : ES_DISABLENOSCROLL;
	if (!CChatChildWnd::FCreate(
								hWndParent,
								idCtl,
								prc,
								TEXT("RICHEDIT"),
									dwStyle
								|	ES_READONLY 
								|	ES_LEFT 
								|	ES_MULTILINE 
								|	ES_SAVESEL 
								|	WS_CHILD 
								|	WS_VISIBLE 							
								|	WS_VSCROLL,
								0,
								NULL
								)
						)
		{
		AssertGLE(FALSE);
		return FALSE;
		}
	//
	// Now Init everything
	//
	return FInitElement();	
}

//
// Set up the UI element
//
BOOL CUIRichEdit::FInitElement(void)
{
	BOOL	fRet = FALSE;

	if (!FInitFontColors())
		{
		return FALSE;
		}
	//
	// And notifications
	//
	SendMessage(EM_SETEVENTMASK,(LPARAM)(ENM_LINK | ENM_DROPFILES));
	//
	// Do the OLE thing. 
	//
	if (!FInitOLE())
		{
		return FALSE;
		}
	//
	// Create drag target
	//
	/*
	m_puiDragDrop = new CUIDragAndDrop((PVOID)this);
	if (NULL == m_puiDragDrop)
		{
		AssertGLE(FALSE);
		return FALSE;
		}
	//
	// Register OLE. NOTE: If this is NOT a READONLY RichEdit control, 
	//
	HRESULT	hr;

	hr = RegisterDragDrop(m_hWnd,m_puiDragDrop);
	if (FAILED(hr))
		{
		switch(GetScode(hr))
			{
			default:
				AssertGLE(FALSE);
				break;

			case DRAGDROP_E_INVALIDHWND:
				AssertSz(0,"DRAGDROP_E_INVALIDHWND");
				break;
			case DRAGDROP_E_ALREADYREGISTERED:
				AssertSz(0,"DRAGDROP_E_ALREADYREGISTERED");
				break;		
			}
		}
	*/
	//
	// Store a pointer to us
	//
	::SetWindowLong(m_hWnd,GWL_USERDATA,(LONG)this);
	//
	// And now subclass the control
	//
	SetWindowProc(RichEditUIWndProc);
	//
	// Nothing modified yet
	//
	SetClean();
	//
	// Load the Hand cursor
	//
	m_hCursor = ::LoadCursor(HInstance(m_hWnd),MAKEINTRESOURCE(IDC_CURSORHAND));
	if (!m_hCursor)
		{
		m_hCursor = ::LoadCursor(NULL,IDC_ARROW);
		}


	return TRUE;
}

BOOL CUIRichEdit::FInitFontColors(void)
{
	//
	//	Get default character format.
	//
	SendMessage(EM_GETCHARFORMAT,FALSE,(LPARAM)&m_ccf.cformat);
	//
	// Make sure its set up right
	//
	m_ccf.cformat.dwEffects = CFE_AUTOCOLOR;
	m_ccf.cformat.dwMask	= CFM_LINK | CFM_BOLD | CFM_COLOR | CFM_FACE | CFM_ITALIC | CFM_STRIKEOUT | CFM_UNDERLINE;
	SendMessage(EM_SETCHARFORMAT,(WPARAM)SCF_ALL,(LPARAM)&m_ccf.cformat);
	//
	// Set forecolor and backcolor to AUTO by default.
	//
	m_ccf.crefBak 	= ::ColorRefGet(0,FALSE);
	m_ccf.cref		= ::ColorRefGet(0);
	m_ccf.fModified	= CCF_COLORCHG;

	return TRUE;
}

void CUIRichEdit::SetWindowProc(WNDPROC wndProc)
{
	Assert(wndProc);
	m_wndProc = (WNDPROC)::SetWindowLong(m_hWnd,GWL_WNDPROC,(LONG)wndProc);
}

LRESULT CUIRichEdit::LrCallWindowProc(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	if (m_wndProc)
		{
		return ::CallWindowProc(m_wndProc,m_hWnd,uMsg,wParam,lParam);
		}

	return ::DefWindowProc(m_hWnd,uMsg,wParam,lParam);
}

BOOL CUIRichEdit::FInitOLE(void)
{
	Assert(NULL == m_pREOle);

	CChatRichEditCallBack	*pcRE;
	BOOL					fRet = FALSE;

	//
	// Alloc a new interface for richedit
	//
	pcRE = new CChatRichEditCallBack(NULL,NULL);
	if (NULL == pcRE)
		{
		DoOOM();
		AssertGLE(FALSE);
		return FALSE;
		}
	pcRE->AddRef();	// up the ref count, since we are passing a pointer to RichEdit
	//
	// Attach ourselves to the object
	//
	pcRE->SetPUI(this);
	//
	// Attach it to the RichEdit control
	//
	if (0 == SendMessage(EM_SETOLECALLBACK, (LPARAM)pcRE))
		{
		Assert(FALSE);
		goto LReturn;
		}
	//
	// Obtain OLE interfaces from RichEdit that we can use to do stuff
	//
	SendMessage(EM_GETOLEINTERFACE,(LPARAM)&m_pREOle);
	if (NULL == m_pREOle)
		{
		Assert(FALSE);
		goto LReturn;
		}
	fRet = TRUE;

LReturn:
	//
	// No longer need OUR copy of this object
	//
	pcRE->Release();

	return fRet;
}

BOOL CUIRichEdit::FSetColorCharFormat(COLORCHARFORMAT *pccf,BOOL fRedraw)
{
	Assert(NULL != pccf);

	::CopyMemory(&m_ccf,pccf,sizeof(COLORCHARFORMAT));

	if (fRedraw)
		{
		return FRedraw();
		}
	return TRUE;
}

BOOL CUIRichEdit::FRedraw(void)
{
	DWORD dwMask;

	dwMask = m_ccf.cformat.dwMask;
	m_ccf.cformat.dwMask = 0;
	if (m_ccf.fModified & CCF_FONTCHG)
		{
		m_ccf.cformat.dwMask |= CFM_FACE;
		}
	if (m_ccf.fModified & CCF_SIZECHG)
		{
		m_ccf.cformat.dwMask |= CFM_SIZE;
		}
	if (m_ccf.fModified & CCF_COLORCHG)
		{
		m_ccf.cformat.dwMask 	|= CFM_COLOR;
		m_ccf.cformat.dwEffects &= ~CFE_AUTOCOLOR;
		}
	//
	// Set the font, size and color
	//
	if (!::FSetRichTextFont(m_hWnd,&m_ccf.cformat)) 
		{
		m_ccf.cformat.dwMask = dwMask;
		return FALSE;
		}
	m_ccf.cformat.dwMask = dwMask;
	if (m_ccf.fModified & CCF_BAKCOLORCHG)
		{
		SendMessage(EM_SETBKGNDCOLOR,(LPARAM)m_ccf.crefBak);
		}
	//
	// No longer modified, as we just redrew..
	//
	m_ccf.fModified = FALSE;

	return TRUE;
}

//
// Action functions
//
void CUIRichEdit::Copy(void)
{
	SendMessage(WM_COPY);
}

void CUIRichEdit::Cut(void)
{
	SendMessage(WM_CUT);
}

void CUIRichEdit::Paste(void)
{
	SendMessage(WM_PASTE);
}

void CUIRichEdit::SelectAll(void)
{
	if (m_hWnd)
		{
		CHARRANGE 	cr;
		LRESULT 	wChars;

		wChars 	 = GetTextLength();
		cr.cpMin = 0;
		cr.cpMax = wChars;
		SendMessage(EM_EXSETSEL,(LPARAM)&cr);
		}
}

void CUIRichEdit::Clear(void)
{
	if (m_hWnd)
		{
		SelectAll();
		SetWindowText(m_hWnd, "");
		FInitFontColors();
		}
}

void CUIRichEdit::Deselect(void)
{
	CHARRANGE 	cr;
	int 		wTextLen;

	wTextLen 	= GetTextLength();
	cr.cpMin 	= cr.cpMax = wTextLen;
	SendMessage(EM_EXSETSEL,(LPARAM)&cr);
}

char *CUIRichEdit::PSzGetSelection(void)
{
	CHARRANGE cr;
	//
	// Get the current selection
	//
	SendMessage(EM_EXGETSEL,(LPARAM)&cr);
	//
	// Allocate a buffer large enough for it
	//
	if (cr.cpMin >= cr.cpMax)
		{
		return NULL;
		}

	TCHAR *psz;

	psz = (TCHAR *) new TCHAR[cr.cpMax - cr.cpMin + 2];
	if (NULL == psz)
		{
		DoOOM();
		AssertGLE(FALSE);
		return NULL;
		}		   

	SendMessage(EM_GETSELTEXT,(LPARAM)psz);

	return psz;
}

BOOL CUIRichEdit::FSaveAs(char *pszFileName,int wFileType)
{
	if (INVALID_HANDLE_VALUE != m_hFile)
		{
		::CloseHandle(m_hFile);
		m_hFile = INVALID_HANDLE_VALUE;
		}

	m_iFileType = wFileType;
	SetDirty();

	return FSave(pszFileName);
}

BOOL CUIRichEdit::FSave(char *pszFileName)
{
	if (!FDirty())
		{
		return TRUE;
		}

	EDITSTREAM es;
	
	if (INVALID_HANDLE_VALUE == m_hFile)
		{
		if (NULL == pszFileName)
			{
			return FALSE;
			}
		m_hFile = ::CreateNormalFile(pszFileName);
		if (INVALID_HANDLE_VALUE == m_hFile)
			{
			DoAlert(IDS_ERRFILEOPEN, -1, metOK);
			return FALSE;
			}		
		}
	
	::ZeroMemory(&es, sizeof(EDITSTREAM));
	es.dwCookie		= (DWORD) this;
	es.pfnCallback 	= (EDITSTREAMCALLBACK)RichSaveHistory;

	::SetFilePointer((HANDLE)m_hFile,0, NULL, FILE_BEGIN);
	SendMessage(EM_STREAMOUT,(WPARAM)m_iFileType == TC_RTF ? SF_RTF : SF_TEXT, (LPARAM)&es);
	if (0 != es.dwError)
		{
		AssertGLE(FALSE);
		return FALSE;
		}
	::SetEndOfFile(m_hFile);
	
	if (!::FlushFileBuffers((HANDLE)m_hFile))
		{
		AssertGLE(FALSE);
		return FALSE;
		}
	//
	// Changes saved
	//
	SetClean();

	return TRUE;


}

DWORD CUIRichEdit::DwSaveHistory(
								LPBYTE 	pbBuff,
								LONG 	lcb,
								LONG 	*plcb
								)
{
	if (!::WriteFile(m_hFile,pbBuff,lcb,(unsigned long *)plcb, NULL))
		{
		DoAlert(IDS_SAVEFAILED, -1, metOK);
		return 1;
		}

	return 0;
}

//
// Write out data from rich text window to disk
//
DWORD CALLBACK RichSaveHistory(
							DWORD 	dwCookie,
							LPBYTE 	pbBuff,
							LONG 	lcb,
							LONG 	far *plcb
							)
{
	CUIRichEdit	*pui;

	pui = (CUIRichEdit *) dwCookie;
	if (pui)
		{
		return pui->DwSaveHistory(pbBuff,lcb,plcb);
		}
	
	return 0;
}

TCHAR *CUIRichEdit::PSzGetAllText(void)
{
	int		cch;
	TCHAR	*psz;

	cch = ::GetWindowTextLength(m_hWnd);
	if (0 == cch)
		{
		return NULL;
		}
	psz = new TCHAR[cch + 1];
	if (!psz)
		{
		AssertGLE(FALSE);
		return NULL;
		}
	//
	// Get the buffer
	//
	if (!FGetAllText(psz,cch))
		{
		AssertGLE(FALSE);
		delete [] psz;
		return NULL;
		}
		
	return psz;	
}

BOOL CUIRichEdit::FGetAllText(TCHAR *pszText,int cch)
{
	Assert(m_hWnd && pszText && cch);

	//
	// Get the buffer
	//
	return (::GetWindowText(m_hWnd,pszText,cch) > 0);
}

void CUIRichEdit::IndentText(DWORD dwIndent)
{
	PARAFORMAT 		pf;
	
	pf.cbSize 			= sizeof(PARAFORMAT);
	pf.dwMask 			= PFM_STARTINDENT;
	pf.dxStartIndent 	= dwIndent;
	
	SendMessage(EM_SETPARAFORMAT,(LPARAM)&pf);
}

void CUIRichEdit::SaveAndMoveSel(void)
{
	int 		wTextLen;
	CHARRANGE 	cr;

	m_fRestore = FALSE;
	//
	// Get current selection
	//	
	SendMessage(EM_EXGETSEL,(LPARAM)&m_charSav);
	//
	// This is a laborious way to determine if the selection should be restored.
	// We only do this IF the ENTIRE document is not selected.
	//
	wTextLen = SendMessage(WM_GETTEXTLENGTH);

	if (m_charSav.cpMin != m_charSav.cpMax || m_charSav.cpMin != wTextLen)
		{
		m_fRestore 	= TRUE;
		cr.cpMin 	= cr.cpMax = wTextLen;
		SendMessage(EM_EXSETSEL,(LPARAM)&cr);
		}
}

//
// Restore selection
//
void CUIRichEdit::RestoreSel(void)
{
	if (m_fRestore)
		{
		SendMessage(EM_EXSETSEL,(LPARAM)&m_charSav);
		m_fRestore = FALSE;
		}
}

BOOL CUIRichEdit::FEndInView(void)
{
	LRESULT wChars;
	POINT 	pt;
	RECT	rc;
	
	wChars = SendMessage(WM_GETTEXTLENGTH);
	//
	// Get the location of the last character in the buffer
	//
	SendMessage(EM_POSFROMCHAR, (WPARAM)&pt, (LPARAM)wChars);
	if (!FGetClientRect(&rc))
		{
		return FALSE;
		}
	//
	// If the character is located higher than the last character, its visible..
	//
	return (pt.y <= rc.bottom);	
}

void CUIRichEdit::AutoScrollHistory(void)
{
	LRESULT wChars;
	POINT 	pt;
	RECT	rc;

	if (!FGetClientRect(&rc))
		{
		return;
		}

	if (rc.left >= rc.right || rc.top >= rc.bottom)
		{
		return;
		}	
	
	/*	
	//
	// how many characters in the buffer?
	//	
	wChars = SendMessage(WM_GETTEXTLENGTH);
	//
	// Get position of the last character. Characters are referred to by index
	// Scroll till the last character is actually visible.
	// We don't want to scroll down by pages.
	//
	
	while(1)
		{
		SendMessage(EM_POSFROMCHAR,(WPARAM)&pt,(LPARAM)wChars);		
		if (pt.y <= rc.bottom)
			{
			break;
			}
		SendMessage(EM_SCROLL,(WPARAM)SB_LINEDOWN);
		}
	*/
	
	int		cLines,cVis;
	LRESULT	chLast;
	POINT	posEnd;
	
	//
	// What is the last visible line?
	//	
	posEnd.x = rc.left;
	posEnd.y = rc.bottom;
	chLast = SendMessage(EM_CHARFROMPOS,(LPARAM)&posEnd);
	//
	// Is it beyond the current rect?
	//
	cVis = SendMessage(EM_LINEFROMCHAR,(WPARAM)chLast);
	cLines = SendMessage(EM_GETLINECOUNT) - 1;
	//
	// Jump to the End if necessary
	//
	if (cLines > cVis)
		{
		SendMessage(EM_LINESCROLL,(LPARAM)(cLines - cVis));
		}

}

void CUIRichEdit::InsertAllText(TCHAR *psz)
{
	//
	// Clear selection
	//
	Deselect();
	//
	// Insert into Msg Buffer
	//
	SelectAll();
	InsertText(psz);
}

//
// Insert msg text into a window.. Note, this does not set any text formatting, simply inserts text.
// Do all formatting before calling this.
//
BOOL CUIRichEdit::FInsertMsg(DWORD	dwMsgID,TCHAR *psz)
{
	TCHAR 		*pszOut = NULL;
	TCHAR 		*(rgsz[1]);
	
	rgsz[0] = psz;

	// 
	// Format msg to insert.
	//
	if (!FormatMessage(
					FORMAT_MESSAGE_ALLOCATE_BUFFER | 
					FORMAT_MESSAGE_FROM_HMODULE | 
					FORMAT_MESSAGE_ARGUMENT_ARRAY,
					NULL,
					dwMsgID, 
					0,(TCHAR *)&pszOut, 
					0,rgsz)
					)
		{
		AssertGLE(FALSE);
		return FALSE;
		}
	//
	// If all is well, insert it..and free
	//	
	if (pszOut)
		{
		InsertText(pszOut);
		LocalFree(pszOut);
		}
 	
	return TRUE;		
}

void CUIRichEdit::InsertText(TCHAR *psz)
{
	//
	// Do we need to autodetect URLS?
	//
	if (!m_fFindURLS)
		{
		SendMessage(EM_REPLACESEL,(LPARAM)(LPCSTR)psz);
		return;
		}
	
	LONG cpEnd,cpNewEnd;

	cpEnd = (LONG) GetTextLength();	// get current text length
	
	SendMessage(EM_REPLACESEL,(LPARAM)(LPCSTR)psz);

	cpNewEnd = (LONG) GetTextLength();	// get new text length
	//
	// And search it for URLS
	//
	RecognizeURLs(cpEnd,cpNewEnd);
}

//
// Action command status
//
BOOL CUIRichEdit::FCopyAvailable(void)
{
	if (m_hWnd)
		{
		CHARRANGE cr;
		
		SendMessage(EM_EXGETSEL,(LPARAM)&cr);
		return (cr.cpMin != cr.cpMax);
		}

	return FALSE;
}				

BOOL CUIRichEdit::FPasteAvailable(void)
{
	return SendMessage(EM_CANPASTE);
}

void  CUIRichEdit::SetCharFormat(BOOL fSet,BOOL fColor,BOOL fBold,CHARFORMAT *pcf)
{
	if (fBold)
		{
		if (fSet)
			{
			++m_cBold;
			}
		else
			{
			--m_cBold;
			if (m_cBold)
				{
				fBold = FALSE;	// ok, its not safe to clear this yet...
				}
			}
		}
	//
	// use default text format if user did not specify one..
	//
	if (pcf)
		{
		::SetCharFormat(m_hWnd,fSet,fColor,pcf->crTextColor,NULL,fBold);
		}
	else
		{
		::SetCharFormat(m_hWnd,fSet,FALSE,0,NULL,fBold);
		}
}

//
// Currently, we only accept 2 kinds of ShortCuts.
// .URL and .MCC
//
BYTE CUIRichEdit::FIsShortCut(LPSTR pszFile)
{
	Assert(pszFile);

	LPSTR				lptmp	= NULL;
	DWORD				dwcch	= 0;

	if('\0' == *pszFile)
		{
		return FALSE;
		}
	//
	// is this a calling card or URL file? That is the only kind we accept
	//
	if(4 <= (dwcch = ::lstrlen(pszFile)))
		{
		lptmp = pszFile + (dwcch - 4);
		if(0 == ::lstrcmpi(lptmp,".url"))
			{
			return 1;
			}
		}

	return 0;
}


//
// allocate storage for another object.
//

HRESULT CUIRichEdit::HrStorageGet(LPSTORAGE *lplpstg)
{
	HRESULT hr = NOERROR;
	TCHAR	szItem[32];

	Assert(lplpstg);

	if(NULL == m_pStorage)
		{
		//
		// get storage for Rich Edit. Create a TEMP file.
		//
		if(FAILED( hr = StgCreateDocfile(
								NULL,
								STGM_CREATE|STGM_READWRITE|STGM_SHARE_EXCLUSIVE|STGM_DELETEONRELEASE,
								0,
								&m_pStorage
								)
			) )
			{
			Assert(FALSE);
			return hr;
			}
		}
	++m_cdwItem;
	//
	// A richedit object
	//
	wsprintf(szItem,"REOBJ%d",m_cdwItem);

	hr = m_pStorage->CreateStorage(
								(const OLECHAR *)szItem,
								STGM_READWRITE|STGM_SHARE_EXCLUSIVE,
								0,
								0,
								lplpstg
								);
	return hr;
}

BOOL CUIRichEdit::FGiveMsgToParent(UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return ::PostMessage(m_hWndParent,uMsg,wParam,lParam);
}

LRESULT CALLBACK RichEditUIWndProc(
							HWND	hWnd,
							UINT	uMsg,
							WPARAM	wParam,
							LPARAM	lParam
							)
{
	
	CUIRichEdit	*pcre;

	pcre = (CUIRichEdit *) GetWindowLong(hWnd,GWL_USERDATA);

	if (pcre)
		{
		switch(uMsg)
			{
			default:
				break;
			
			case WM_CHAR:
				switch(wParam)
					{
					default:
						break;

					case '\r':
						return pcre->FGiveMsgToParent(WM_COMMAND,MAKEWPARAM(IDC_SEND,0),0);
						break;
					
					case '\t':
						goto LTabKey;
					}
				break;

			case WM_COMMAND:
				switch (LOWORD(wParam))
					{
					default:
						break;
					
					case IDC_SEND:
					case IDC_WHISPER:
						return pcre->FGiveMsgToParent(WM_COMMAND,wParam,0);
					}
				break;

			case WM_KEYDOWN:
				switch(wParam)
					{
					default:
						break;

					case VK_F6:
LTabKey:
						int idCmd;

						if (GetAsyncKeyState(VK_SHIFT) & 0x8000)
							{
							idCmd = IDC_TABKEYSHIFT;
							}
						else
							{
							idCmd = IDC_TABKEY;
							}

						return pcre->FGiveMsgToParent(WM_COMMAND,MAKEWPARAM(idCmd,0),0);						

					case VK_UP:
					case VK_DOWN:
						pcre->SendMessage(EM_SCROLLCARET);
						break;
					}
				break;
			}
		return pcre->LrCallWindowProc(uMsg,wParam,lParam);
		}

	return ::DefWindowProc(hWnd,uMsg,wParam,lParam);
}


//
// When we get a drag and drop, we can get the object's file path.
// If this object happens to be an .mmc or a .url, we grab its data
// and send it to everybody!
//
BOOL CUIRichEdit::FDragDrop(ENDROPFILES *pedf)
{
	Assert(pedf);

	HDROP		hDrop;
	char		szFile[MAX_PATH];
	DWORD		dwcFiles = 0;
	DWORD		dwFile;
	HRESULT		hr;
	BYTE		bType;
	
	hDrop = (HDROP) pedf->hDrop;
	//
	// get the count of files
	//
	dwcFiles = DragQueryFile(hDrop,0XFFFFFFFF,NULL,0);
	//
	// We only accept ONE file at a time. FIX ME 
	//
	// Accept only if all given files are MSN PRIVATE CHAT TICKETS
	//
	for(dwFile = 0; dwFile < dwcFiles; ++dwFile)
		{		
		if(DragQueryFile(hDrop,dwFile,szFile,sizeof(szFile)) )
			{
			bType = FIsShortCut(szFile);
			if (0 == bType)
				{
				//
				// Not a drag drop we accept
				//
				MessageBeep(MB_ICONEXCLAMATION);
				}
			else
				{
				FAcceptDrag(szFile);
				}
			}					
		}
	return TRUE;
}

BOOL CUIRichEdit::FAcceptDrag(TCHAR *szFile)
{
	//
	// Open the file, grab the URL
	//
	TCHAR	szURL[MAX_PATH + 1];

	if (::GetPrivateProfileString("InternetShortcut","URL","",szURL,MAX_PATH,szFile))
		{
		InsertText(szURL);
		return TRUE;
		}
	
	return FALSE;
}


//
// Initialize the Browser path
//
BOOL CUIRichEdit::FFindBrowser(BOOL fReload)
{
	if (!fReload)
		{
		if ('\0' != m_szBrowserPath[0])
			{
			return TRUE;	// use what we have
			}
		//
		// Try to load it from the Registry
		//
		if (::FGetRegistrySz(
						HKEY_CURRENT_USER,
						REG_SZBROWSER,
						(TCHAR *)m_szBrowserPath,
						sizeof(m_szBrowserPath))
						)
			{
			return TRUE;
			}
		}

#if 0
	//
	// Get a new one
	//
	::lstrcpy(m_szBrowserPath,"*.exe");
	
	if (FGetBrowserPath(HInstance(m_hWnd),m_hWnd,m_szBrowserPath,sizeof(m_szBrowserPath)))
		{
		//
		// Save it to the registry
		//
		::SetRegistryRaw(
					HKEY_CURRENT_USER,
					REG_SZBROWSER,
					REG_SZ,
					(PVOID)m_szBrowserPath,
					lstrlen(m_szBrowserPath) + 1
					);
		return TRUE;
		}
#endif
	
	return FALSE;
}

BOOL CUIRichEdit::FWinExecBrowser(TCHAR *szURL)
{
	Assert(szURL);

	TCHAR	szCmd[2*MAX_PATH + 2];

LFindBrowser:
	if (!FFindBrowser())
		{
		return FALSE;
		}
	//
	// construct the command line
	//
	wsprintf(szCmd, "%s %s",m_szBrowserPath,szURL);	// start with the browser plus space...
	if (WinExec(szCmd, SW_SHOW) < 32)
		{
		// perhaps they moved or deleted their executable regardless
		// of the error, we will just browse for a path
		// 
		MessageBeep(MB_OK);
		m_szBrowserPath[0] = '\0';	// make them find one
		goto LFindBrowser;
		}
	
	return TRUE;
}

BOOL CUIRichEdit::FLaunchBrowser(TCHAR *szURL)
{
	Assert(szURL);
	
	UINT		uErr;	
	int			cch;
	//
	// First, we'll let ShellExecute do it's thing. Otherwise, we'll try using WinExec 
	// and the cached Browser path
	//
	uErr = (UINT) ShellExecute(m_hWnd,NULL,szURL,NULL,NULL,SW_SHOWNORMAL);
	return (uErr > 32);
}

//
// Parse any incoming data for matches to standard URL formats.
// If found, insert those as CFE_LINK in the richedit control.
// When the user clicks on the link, we'll get notified and then take appropriate
// action with the URL
//
// cpStart & cpEnd indicate the range of text we are going to recognize URLs in
//
void CUIRichEdit::RecognizeURLs(LONG cpRangeStart,LONG cpRangeEnd)
{
	int			isz;
	LONG		cpMatch;
	LONG		cpSep;
	LONG		cpEnd;
	LPTSTR		pch;
	CHARRANGE	chrgSave;
	FINDTEXTEX	ft;
	TEXTRANGE	tr;
	CHARFORMAT	cf;
	TCHAR		szBuff[MAX_PATH]; 
	LPTSTR		pszT;
	LPARAM		lNotifSuppression;
	//
	// Prepare a few local variables for use
	//
	pszT			= szBuff;
	//
	// We are searching the given range for a Colon
	//
	ft.chrg.cpMin	= cpRangeStart;
	ft.chrg.cpMax	= cpRangeEnd;
	ft.lpstrText	= (LPSTR) SZURLSEP; 
	//
	// Richedit will return found string in this buffer
	//
	tr.lpstrText	= szBuff;
	//
	// And set up the character format that we will impose on any found URLs
	//
	cf.cbSize		= sizeof(CHARFORMAT);
	cf.dwMask		= CFM_LINK;
	cf.dwEffects	= 0;
	//
	// Save the current selection and then hide it
	//
	SendMessage(EM_EXGETSEL,(LPARAM) &chrgSave);
	SendMessage(EM_HIDESELECTION,(WPARAM)TRUE,(LPARAM)FALSE);
	//
	// And save and reset the Event notification mask to NOT send any right now
	//
	lNotifSuppression = SendMessage(EM_GETEVENTMASK);
	SendMessage(EM_SETEVENTMASK);
	//
	// Now loop through our range and mark URLS
	//
	for (;;)
	{
		LONG cpLast;

		//
		// find the colon
		//
		cpSep = SendMessage(EM_FINDTEXTEX,(LPARAM)&ft);
		if (cpSep <= 0)
			{
			//
			// No Colons found
			//
			break;
			}
		//
		// Set us up so we do the next find starting at the end of the current found range
		//
		cpEnd			= ft.chrgText.cpMax;
		ft.chrg.cpMin	= cpEnd;
		//
		// The colon could be embedded in ANY word. We need to first make sure that the word
		// we are embedded in is not a possible URL.
		// So, move LEFT till we find the beginning of this word.
		//
		cpMatch = SendMessage(EM_FINDWORDBREAK,(WPARAM)WB_MOVEWORDLEFT,(LPARAM)ft.chrgText.cpMin);
		if (cpMatch == cpSep)
			{
			//
			// Word starts with a colon. Ignore.
			//
			continue;
			}
		//
		// Is the colon stuck in the MIDDLE of a very large word? If so, skip over that word
		// We assume that URL prefixes (they come to the left of the colon) are no larger than
		// CCHURLPREFIXMOST
		//
		if (cpMatch < (cpSep - CCHURLPREFIXMOST))
			{
			ft.chrg.cpMin = SendMessage(EM_FINDWORDBREAK,(WPARAM)WB_MOVEWORDRIGHT,(LPARAM)cpSep);
			Assert(ft.chrg.cpMin > cpSep);
			continue;
			}
		//
		// OK, we have something that could be a URL protocol type. 
		// pull the text of the keyword out into szBuff to compare against our word list
		// Get the text from the beginning of the word till the Colon
		//
		tr.chrg.cpMin = cpMatch;
		tr.chrg.cpMax = cpSep;
		if (!SendMessage(EM_GETTEXTRANGE,(LPARAM) &tr))
			{
			goto LEnd;
			}
		
		//
		// compare to each word in our list
		//

		for (isz = 0; NULL != m_pszURLPrefixs[isz]; isz	+= (lstrlen(m_pszURLPrefixs + isz) +1) )
			{
			//
			// URL protocol types are not case-sensitive
			//
			if (0 == lstrcmpi(szBuff,&m_pszURLPrefixs[isz]))
				{
				goto LMatch; // got one
				}
			}
		continue;

LMatch:
		//
		// Now we have to step through the remainder of the URL and make sure its not
		// bogus
		//
		ft.chrgText.cpMin = cpMatch;
		cpLast = cpEnd; // assume that we will stop after the colon
		//
		// loop through chunks of the URL in steps of sizeof(szBuff) looking for a terminator
		// set cpLast to the last terminator byte that is legal according to us
		//
		for (;;)
			{
			//
			// Get the text to the right of the colon
			//
			tr.chrg.cpMin = cpEnd;
			tr.chrg.cpMax = cpEnd + sizeof(szBuff) - 1;
			if (!SendMessage(EM_GETTEXTRANGE,(LPARAM) &tr))
				{
				goto LEnd;
				}
			for (pch = szBuff; *pch; pch++, cpEnd++)
				{
				const BYTE fb = MPCH_FLEGALFORURL[*pch];

				if (!fb || !(fb & 0x01))
					{
					//
					// Anything set to NULL in the Map is an illegal char
					//
					goto LEnd;
					}
				
				if (!(fb & 0x10))
					{
					//
					// Not a terminating char
					//
					cpLast = cpEnd + 1;
					}
				}
			}

LEnd:
		//
		// Nothing beyond the : Implies something like http:
		// That is bogus
		if (cpLast == cpSep + 1) 
			{
			continue;            
			}
		//
		// select the entire URL including the http colon, mark it as linked, and change the 
		// charformat if appropriate
		//
		ft.chrgText.cpMax = cpLast; // extend the selction to just beyond the URL.
		SendMessage(EM_EXSETSEL,(LPARAM) &ft.chrgText);
		//
		// And underline it
		//
		cf.dwMask		= CFM_LINK | CFM_UNDERLINE | CFM_COLOR;
		cf.dwEffects	= CFE_LINK | CFE_UNDERLINE;
		cf.crTextColor	= m_crLink;
		SendMessage(EM_SETCHARFORMAT,(WPARAM)SCF_SELECTION,(LPARAM) &cf);
		//
		// no need to re-search through the URL
		// so, just advance past the last totally cool URL character
		ft.chrg.cpMin = cpLast + 1;
		}
	//
	// Restoration, Restoration..
	//
	ft.chrgText.cpMax = GetTextLength();
	ft.chrgText.cpMin = ft.chrgText.cpMax;
	SendMessage(EM_EXSETSEL,(LPARAM) &ft.chrgText);
	SetDefaultCharFormat();
	SendMessage(EM_EXSETSEL,(LPARAM) &chrgSave);
	SendMessage(EM_HIDESELECTION,(WPARAM)FALSE,(WPARAM)FALSE);
	SendMessage(EM_SETEVENTMASK,(WPARAM) 0,(LPARAM) lNotifSuppression);	
}

//
// User clicked on a URL. The parent window then calls this routine..so the click
// is handled.
//
BOOL CUIRichEdit::FHandleLink(ENLINK *penlink)
{
	BOOL		fShift;
	TEXTRANGE	tr;
	TCHAR		szURL[MAX_PATH];
	HCURSOR		hCursor;
	BOOL		fRet = TRUE;

	if (WM_SETCURSOR == penlink->msg)
		{
		//
		// Set the cursor to a HAND
		//
		SetCursor(m_hCursor);
		return TRUE;
		}
	//
	// Only respond to full clicks.. and if the control key is down, let them edit/copy it
	//
	if (WM_LBUTTONDOWN != penlink->msg || (GetAsyncKeyState(VK_CONTROL) & 0x8000))
		{
		return FALSE;
		}	
	//
	// if the path to the browser is NULL, or the user uses the shift key to invoke a browser
	// we throw up a common file dialog to find the browser
	//
	fShift = !!(GetAsyncKeyState(VK_SHIFT) & 0x8000);
	if (fShift)
		{
		if (!FFindBrowser(TRUE))
			{
			return FALSE;
			}
		}
	//
	// Must make sure that the URL isn't too long..if it is, we will just have to truncate it
	//
	tr.chrg.cpMin	= penlink->chrg.cpMin;
	tr.chrg.cpMax	= penlink->chrg.cpMax;
	if ((tr.chrg.cpMax - tr.chrg.cpMin) > (sizeof(szURL)- 1))
		{
		tr.chrg.cpMax	= (sizeof(szURL)- 1);
		}
	tr.lpstrText	= szURL; // let Richedit place the URL in the rest of the buffer
	//
	// Get the selection
	//	
	SendMessage(EM_GETTEXTRANGE,(LPARAM) &tr);
	//
	// Eliminate an \r and \n at the end
	//
	int cch;

	cch = ::lstrlen(szURL) - 1;
	while (cch >= 0)
		{
		switch(szURL[cch])
			{
			default:
				szURL[cch + 1] = '\0';
				goto LLaunch;
			
			case '\r':
			case '\n':
			case ' ':
				break;					
			}
		--cch;
		}

LLaunch:	
	//
	// Launch it
	//

	//
	// MIC URLS are different and are NOT launched using a Web Browser
	//
	if (FHandleIfMicURL(m_hWnd,szURL))
		{
		return TRUE;
		}

	hCursor = ::SetCursor(::LoadCursor(NULL,IDC_APPSTARTING));
	if (fShift || !FLaunchBrowser(szURL))
		{
		fRet = FWinExecBrowser(szURL);
		}
	
	::SetCursor(hCursor);

	return fRet;
}
#endif

inline BOOL IsURLChar(const UCHAR pch) {
	const BYTE fb = MPCH_FLEGALFORURL[pch];

	if (!fb || !(fb & 0x01)) return FALSE;
	else return TRUE;
}

// This routine searches backwards in a string, which in general is not OK for internationalization.
// However, based on the fact that URLs are ANSI, this should produce fine results
const char *FindPreceedingWord(const char *start, const char *colon) {
	const char *sptr = colon;
	while (sptr >= start) {
		if (IsURLChar((const UCHAR) *sptr)) sptr--;
		else return sptr+1;
	}
	return start;
}

inline const char *FindWordBreak(const char *start) {
	const char *wordBreak;
	if (wordBreak = strchr(start, ' ')) return wordBreak;
	else return strchr(start, '\0');
}

#if 0
const char *FindURLEnd(const char *colon) {
	const char *end = colon;
	while (IsURLChar(*end)) end++;
	while (ispunct(*--end) && end > colon);  // backup past trailing punctuation
	return end+1;  // return character after url
}
#endif

inline const char *FindURLEnd(const char *start, const char *end) {
	end--; // get to last character of URL (end is exclusive)
	while (ispunct(*end) && *end != '/' && end > start) end--;
	return end+1;  // return character after url (url is a min of 1 char long)
}


void DestroyLinks(CPtrArray **links) {
	if (!*links) return;
	int nLinks = (*links)->GetUpperBound();
	for (int i = 0; i <= nLinks; i++) {
		CLink *link = (CLink *) ((**links)[i]);
		delete link;
	}
	delete (*links);
	*links = NULL;
}

CPtrArray *CopyLinks(CPtrArray *links) {
	if (!links) return NULL;
	CPtrArray *newLinks = new CPtrArray;
	int upper = links->GetUpperBound();
	for (int i = 0; i <= upper; i++) {
		CLink *unit = (CLink *) (*links)[i];
		CLink *newUnit = new CLink(*unit);
		newLinks->Add(newUnit);
	}
	return newLinks;
}


CPtrArray *AddLink(CPtrArray *links, const char *url, int start, int length) {
	if (!links) links = new CPtrArray;
	CLink *unit = new CLink(start, length, url);
	links->Add(unit);
	return links;
}


CPtrArray *IdentifyURLs(const char *mesg) {
	CPtrArray *links = NULL;
	const char *start = mesg;

	while (TRUE) {
		const char *colon = strchr(start, ':');
		if (colon) {
			const char *word = FindPreceedingWord(mesg, colon);
			const char *wordEnd = FindWordBreak(colon);
			const char *trueEnd = FindURLEnd(word, wordEnd);  // ignores irrelevant terminating punctuation

			int len = trueEnd-word;
			char *possibleURL = (char *) malloc (sizeof(TCHAR) * (len+1));
			strncpy(possibleURL, word, len);
			possibleURL[len] = '\0';

			if (FIsURL(possibleURL, NULL, NULL)) {
				links = AddLink(links, possibleURL, word - mesg, trueEnd - word);
			}
			free(possibleURL);
			start = wordEnd;
		} else break;
	}
	return links;
}

BOOL FLaunchBrowser(const TCHAR *szURL)
{
	ASSERT(szURL);

	//
	// First, we'll let ShellExecute do it's thing. Otherwise, we'll try using WinExec 
	// and the cached Browser path
	//
	UINT uErr = (UINT) ShellExecute(GetPrimaryView()->GetSafeHwnd(),NULL,szURL,NULL,NULL,SW_SHOWNORMAL);
	return (uErr > 32);
}


