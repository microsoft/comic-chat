// ChatPr.cpp : Implementation of CMsChatPr
#include "StdAfx.H"
#include "MsChatPr.H"
#include "ChatPr.H"
#include "CItmsObj.H"

// for ASSERT and FAIL
//
SZTHISFILE

//=--------------------------------------------------------------------------=
// MsgWndProc
//=--------------------------------------------------------------------------=
// Messaging window proc
//
LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg >= WM_U_MIN && uMsg <= WM_U_MAX)
	{
   		CMsChatPr	*pChatCtrl = (CMsChatPr*) GetWindowLong(hwnd, GWL_USERDATA);
   		SCODE		sc = S_OK;

		ASSERT(pChatCtrl, "pChatCtrl is NULL in MsgWndProc");
	
		switch (uMsg)
		{
			case WM_U_BEGINENUMERATION:
				// wParam = pointer to CChatChannel object
				// lParam = enumEnumType
				OutputDebugThreadIdString("WM_U_BEGINENUMERATION msg\n");
				pChatCtrl->FireBeginEnumeration((CChatChannel*) wParam, (enumEnumType) lParam);
				return 0L;

			case WM_U_CONNECTIONSTATE:
				// wParam = NewConnectionState
				// lParam = 0L
				OutputDebugThreadIdString("WM_U_CONNECTIONSTATE msg\n");
				pChatCtrl->FireConnectionState((enumConnectionState) wParam);
				return 0L;

			case WM_U_CHANNELSTATE:
				// wParam = pointer to CChatChannel object
				// lParam = chsNewChannelState
				OutputDebugThreadIdString("WM_U_CHANNELSTATE msg\n");
				pChatCtrl->FireChannelState((CChatChannel*) wParam, (enumChannelState) lParam);
				return 0L;

			case WM_U_CLOSECHANNELS:
				// wParam = 0L
				// lParam = 0L
				OutputDebugThreadIdString("WM_U_CLOSECHANNELS msg\n");
				pChatCtrl->bCloseAllChannels();
				return 0L;

			case WM_U_CHANNEL_HRESULT:
				// wParam = pointer to CChatChannel object
				// lParam = HRESULT error code
				OutputDebugThreadIdString("WM_U_CHANNEL_HRESULT msg\n");
				pChatCtrl->FireChannelError((CChatChannel*) wParam, NULL, (HRESULT) lParam);
				return 0L;

			case WM_U_SOCKET_HRESULT:
				// wParam = szChannelName or NULL
				// lParam = HRESULT error code
				OutputDebugThreadIdString("WM_U_SOCKET_HRESULT msg\n");
				pChatCtrl->FireConnectionError((LPCTSTR) wParam, NULL, (HRESULT) lParam);
				return 0L;

			case WM_U_IRC_MSG:
				// wParam = pointer to PRIRCMSG
				// lParam = shall we delete the PRIRCMSG from the pool?
				pChatCtrl->TreatIrcMsg((PPRIRCMSG) wParam, (BOOL) lParam);
				return 0L;

			case WM_U_SERVERPROP:
				// wParam = 0L
				// lParam = 0L
				pChatCtrl->ExposeServerItems();
				return 0L;

			case WM_U_USERPROP:
				// wParam = pointer to PRQUERY
				// lParam = pointer to PRUSER
				pChatCtrl->ExposeUserItems((PPRQUERY) wParam, (PPRUSER) lParam);
				return 0L;

			case WM_U_MEMBERPROP:
				// wParam = pointer to PRQUERY
				// lParam = TRUE if pPrQuery needs to be removed from query list
				//			FALSE if pPrQuery just needs to be freed
				pChatCtrl->ExposeMemberItems((PPRQUERY) wParam, (BOOL) lParam);
				return 0L;

			case WM_U_CHANNELPROP:
				// wParam = pointer to PRQUERY
				// lParam = TRUE if pPrQuery needs to be removed from query list
				//			FALSE if pPrQuery just needs to be freed
				pChatCtrl->ExposeChannelItems((PPRQUERY) wParam, (BOOL) lParam);
				return 0L;

			case WM_U_CHANNELPROP2:
				// wParam = pointer to PRCHANNEL
				// lParam = 0L
				pChatCtrl->ExposeChannelItems((PPRCHANNEL) wParam);
				return 0L;

			default:
				ASSERT(FALSE, "Unexpected uMsg in MsgWndProc");
		}
	}
		
    return (DefWindowProc(hwnd, uMsg, wParam, lParam));
}    


/////////////////////////////////////////////////////////////////////////////
// CMsChatPr

BOOL CMsChatPr::bConstruct(void)
{
	BOOL bRet;
		
	// By default timeout = 60000 milliseconds
	m_dwPropertyAccessTimeOut = (DWORD) g_lDefaultPropertyAccessTimeOut;
	// By default we process the protocol messages
	m_bProcessProtocolMessages = g_bDefaultProcessProtocolMessages;

	m_pcc				= NULL;
	m_pccdisp			= NULL;

	m_bCtrlDestructed	= FALSE;

	::LoadString(_Module.GetModuleInstance(), IDS_CTL_E_OUTOFMEMORY, g_szOOM, g_nMaxLengthError);

	bRet = bSetMessagingWindows();
	ASSERT(bRet, "Couldn't create messaging windows in CMsChatPr::bConstruct");

	bRet &= bLoadComicStrings(_Module.GetModuleInstance());
	ASSERT(bRet, "Couldn't load comic strings in CMsChatPr::bConstruct");

	// create a channel collection
	IChannelsCollection* picc = NULL;
	LPCLASSFACTORY pClassFactory = NULL;

	if (FAILED(m_hrLastError = AtlModuleGetClassObject(&_Module, CLSID_Channels, IID_IClassFactory, (LPVOID*) &pClassFactory)))
	{
		ASSERT(FALSE, "AtlModuleGetClassObject failed in CMsChatPr::bConstruct");
		return FALSE;
	}
	ASSERT(pClassFactory, "pClassFactory is NULL in CMsChatPr::bConstruct");

	m_hrLastError = pClassFactory->CreateInstance(NULL, IID_IDispatch, (LPVOID*) &m_pccdisp);
	pClassFactory->Release();

	if (FAILED(m_hrLastError))
	{
		ASSERT(FALSE, "CreateInstance failed in CMsChatPr::bConstruct");
		return FALSE;
	}
	ASSERT(m_pccdisp, "m_pccdisp is NULL in CMsChatPr::bConstruct");

	m_pccdisp->QueryInterface(IID_IChannelsCollection, (void**) &picc);
	ASSERT(picc, "picc is NULL in CMsChatPr::bConstruct");

	m_pcc = (CChannelsCollection*) picc;
	picc->Release();

	m_pcc->m_plistChannel = &m_listChannel;
	m_pcc->m_pccsock = (CChatSocket*) this;

	return bRet;
}


void CMsChatPr::Destruct(void)
{
	m_bCtrlDestructed = TRUE;

	// Close the channels and disconnect the socket if necessary
	bCloseConnection(TRUE /*bSync*/, TRUE /*bStopPosting*/);

	OutputDebugThreadIdString("CMsChatPr::Destruct - Socket is definitely closed\n");

	if (m_hwndMess)
		::DestroyWindow(m_hwndMess);

	if (m_hwndMessEx)
		::DestroyWindow(m_hwndMessEx);

	if (m_pccdisp)
		m_pccdisp->Release();
}


STDMETHODIMP CMsChatPr::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IMsChatPr,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


HRESULT CMsChatPr::OnDraw(ATL_DRAWINFO& di)
{
	DisplayDesignIcon(di.hdcDraw, di.dwDrawAspect, di.prcBounds, IDB_DESIGN_PRBITMAP);
	return S_OK;
}

void CMsChatPr::DisplayDesignIcon(HDC hdcDraw, DWORD dwDrawAspect, LPCRECTL prcBounds, WORD wIDB)
{
	INT         nSavedDC;
	HDC         hdcMem;
    HBITMAP		hbmpTile;
    HBITMAP		hbmpSave = NULL;
    BITMAP      bm;

    if (!(nSavedDC = SaveDC(hdcDraw)))
		goto exit;
    if (ERROR == IntersectClipRect(hdcDraw, prcBounds->left, prcBounds->top, prcBounds->right, prcBounds->bottom))
		goto exit;
    SetMapMode(hdcDraw, MM_TEXT);
	hdcMem = CreateCompatibleDC(hdcDraw);
    if (!hdcMem)
		goto exit;
	hbmpTile = LoadBitmap(_Module.GetModuleInstance(), MAKEINTRESOURCE(wIDB));
	if (!hbmpTile)
		goto exit;
	if (!GetObject(hbmpTile, sizeof(BITMAP), &bm))
		goto exit;
    if (!(hbmpSave = (HBITMAP) SelectObject(hdcMem, hbmpTile)))
		goto exit;
    BitBlt(hdcDraw, prcBounds->left, prcBounds->top, bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hbmpSave);
	hbmpSave = NULL;
    DeleteObject(hbmpTile);
    RestoreDC(hdcDraw, nSavedDC);

	/*
	SIZEL sizeInPix;
	SIZEL sizeInHiMetric;

	sizeInPix.cx = bm.bmWidth;
	sizeInPix.cy = bm.bmHeight;

	AtlPixelToHiMetric(&sizeInPix, &sizeInHiMetric);

	RECT rcPos;

	rcPos.left = 0;
	rcPos.top = 0;
	rcPos.right = sizeInPix.cx;
	rcPos.bottom = sizeInPix.cy;

    // Make sure that the control's size matches the one of the bitmap
    if (m_sizeExtent.cx != sizeInHiMetric.cx || m_sizeExtent.cy != sizeInHiMetric.cy)
		SetExtent(dwDrawAspect, &sizeInHiMetric);
	*/
	return;

exit:
	if (hbmpSave)
	{
		SelectObject(hdcMem, hbmpSave);
		DeleteObject(hbmpTile);
	}
	if (nSavedDC)
		RestoreDC(hdcDraw, nSavedDC);
}


HRESULT CMsChatPr::HrThrowError(SCODE sc, LPCTSTR szChannelName, LPCTSTR szNickname)
{
	TCHAR szError[g_nMaxLengthError];

	ASSERT(sc != S_OK, "sc == S_OK in CMsChatPr::HrThrowError");
	
	Sc2Sz(sc, szError, szChannelName, szNickname, bIsIrcXSocket(), GetCharSet());

	return AtlReportError(CLSID_MsChatPr, szError, IID_IMsChatPr, ResultFromScode(sc));
}


//=--------------------------------------------------------------------------=
// CMsChatPr::bCreateMessagingWindow
//=--------------------------------------------------------------------------=
// creates the messaging window
//
// Parameters:
//    LPCTSTR			- [in] messaging window class name
//    DWORD				- [in] 'this' pointer
//
// Output:
//    BOOL
//
// Notes:
//    
BOOL CMsChatPr::bCreateMessagingWindow(HWND *phwnd, LPCTSTR lpClassName, DWORD dwUserData)
{
	ASSERT(phwnd, "phwnd is NULL in CMsChatPr::bCreateMessagingWindow");

	if (!*phwnd)
	{
	    WNDCLASS	wndclass;
		BOOL		bRet;
		HRESULT		hr;

		ZeroMemory(&wndclass, sizeof(WNDCLASS));
		wndclass.lpfnWndProc = MsgWndProc;
		wndclass.cbWndExtra = sizeof(DWORD);
		wndclass.hInstance = _Module.GetModuleInstance();
		wndclass.lpszClassName = lpClassName;
		bRet = RegisterClass(&wndclass);
		
		if (!bRet && ((hr = GetLastError()) != ERROR_CLASS_ALREADY_EXISTS && hr != NOERROR))
			return FALSE;

		*phwnd = ::CreateWindow(lpClassName, _T(""), 0, 0, 0, 0, 0, 0, NULL, _Module.GetModuleInstance(), NULL);

		if (!*phwnd)
		{
			ASSERT(FALSE, "*phwnd is NULL in CMsChatPr::bCreateMessagingWindow");
			return FALSE;
		}

		if (dwUserData)
		{
			SetLastError(0);
			if (!::SetWindowLong(*phwnd, GWL_USERDATA, dwUserData) && GetLastError())
			{
				ASSERT(FALSE, "SetWindowLong failed in CMsChatPr::bCreateMessagingWindow");
				return FALSE;
			}
		}
	}
	return TRUE;
}


BOOL CMsChatPr::bSetMessagingWindows(void)
{
	if (!bCreateMessagingWindow(&m_hwndMess,   szMSGWNDCLASS, (DWORD) this) || 
		!bCreateMessagingWindow(&m_hwndMessEx, szMSGWNDCLASS, 0L))
		return HrThrowError(GetScodeFromHResult(GetLastError()));

	ASSERT(m_hwndMess,   "m_hwndMess   is NULL in CMsChatPr::bSetMessagingWindows");
	ASSERT(m_hwndMessEx, "m_hwndMessEx is NULL in CMsChatPr::bSetMessagingWindows");

	CChatSocket::m_hwndParent = m_hwndMess;

	SetMsgWnd(m_hwndMess);

	return TRUE;
}


BOOL CMsChatPr::bCreateChatItems(CChatItems** ppCItems)
{
	ASSERT(ppCItems, "ppCItems is NULL in CMsChatPr::bCreateChatItems");

	// create a chatitems object
	LPCLASSFACTORY	pClassFactory = NULL;
	CItemsObject*	pcitms = NULL;
	IChatItems*		pici = NULL;
	IDispatch*		pidisp = NULL;

	*ppCItems = NULL;

	if (FAILED(m_hrLastError = AtlModuleGetClassObject(&_Module, CLSID_ChatItems, IID_IClassFactory, (LPVOID*) &pClassFactory)))
	{
		ASSERT(FALSE, "AtlModuleGetClassObject failed in CMsChatPr::bCreateChatItems");
		return FALSE;
	}

	ASSERT(pClassFactory, "pClassFactory is NULL in CMsChatPr::bCreateChatItems");

	m_hrLastError = pClassFactory->CreateInstance(NULL, IID_IDispatch, (LPVOID*) &pidisp);
	pClassFactory->Release();

	if (FAILED(m_hrLastError))
	{
		ASSERT(FALSE, "CreateInstance failed in CMsChatPr::bCreateChatItems");
		return FALSE;
	}

	ASSERT(pidisp, "pidisp is NULL in CMsChatPr::bCreateChatItems");

	pidisp->QueryInterface(IID_IChatItems, (void**) &pici);
	pidisp->Release();

	if (!pici)
	{
		ASSERT(FALSE, "pici is NULL in CMsChatPr::bCreateChatItems");
		m_hrLastError = E_FAIL;
		return FALSE;
	}

	pcitms = (CItemsObject*) pici;

	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::bCreateChatItems");

	*ppCItems = (CChatItems*) pcitms;
	
	return TRUE;
}


void CMsChatPr::ReleaseChatItems(CChatItems *pCItems)
{
	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::ReleaseChatItems");

	CItemsObject* pcitms = (CItemsObject*) pCItems->GetAutObject();

	((IChatItems*) pcitms)->Release();
}
