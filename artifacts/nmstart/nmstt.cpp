// NMStt.cpp : Implementation of CNMStt
#include "StdAfx.H"
#include "NMStart.H"
#include "NMStt.H"
#include "Debug.H"
#include "DlyLdDll.H"
#include "MsConf.H"

// for ASSERT and FAIL
//
SZTHISFILE



DWORD __stdcall DwStartNetMeetingListen(PVOID pvData)
{
	CNMStt *pNMStt;
	
	ASSERT(pvData, "pvData is NULL in DwStartNetMeetingListen");

	pNMStt = (CNMStt*) pvData;

	return pNMStt->HrThreadConferenceListen();
}


DWORD __stdcall DwStartNetMeetingConnect(PVOID pvData)
{
	CNMStt *pNMStt;
	
	ASSERT(pvData, "pvData is NULL in DwStartNetMeetingConnect");

	pNMStt = (CNMStt*) pvData;

	return pNMStt->HrThreadConferenceConnect();
}


//=--------------------------------------------------------------------------=
// MsgWndProc
//=--------------------------------------------------------------------------=
// Messaging window proc
//
LRESULT CALLBACK MsgWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg >= WM_U_MIN && uMsg <= WM_U_MAX)
	{
   		CNMStt	*pNMStt = (CNMStt*) GetWindowLong(hwnd, GWL_USERDATA);

		ASSERT(pNMStt, "pNMStt is NULL in MsgWndProc");
	
		switch (uMsg)
		{
			case WM_U_OPERATIONDONE:
				// wParam = DWORD, result of ConferenceListen call
				// lParam = UINT, disp ID
				OutputDebugThreadIdString("WM_U_OPERATIONDONE msg\n");
				pNMStt->FireOperationDone(wParam, lParam);
				return 0L;

			default:
				ASSERT(FALSE, "Unexpected uMsg in MsgWndProc");
		}
	}
		
    return (DefWindowProc(hwnd, uMsg, wParam, lParam));
}    


/////////////////////////////////////////////////////////////////////////////
// CNMStt

STDMETHODIMP CNMStt::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_INMStt,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


BOOL CNMStt::bCreateMessagingWindow(HWND *phwnd, LPCTSTR lpClassName, DWORD dwUserData)
{
	ASSERT(phwnd, "phwnd is NULL in CNMStt::bCreateMessagingWindow");

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
			ASSERT(FALSE, "*phwnd is NULL in CNMStt::bCreateMessagingWindow");
			return FALSE;
		}

		if (dwUserData)
		{
			SetLastError(0);
			if (!::SetWindowLong(*phwnd, GWL_USERDATA, dwUserData) && GetLastError())
			{
				ASSERT(FALSE, "SetWindowLong failed in CNMStt::bCreateMessagingWindow");
				return FALSE;
			}
		}
	}
	return TRUE;
}


HRESULT CNMStt::OnDraw(ATL_DRAWINFO& di)
{
	DisplayDesignIcon(di.hdcDraw, di.dwDrawAspect, di.prcBounds, IDB_DESIGN_NMBITMAP);
	return NOERROR;
}


void CNMStt::DisplayDesignIcon(HDC hdcDraw, DWORD dwDrawAspect, LPCRECTL prcBounds, WORD wIDB)
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


HRESULT CNMStt::HrThrowError(SCODE sc)
{
	ASSERT(sc != S_OK, "sc == S_OK in CNMStt::HrThrowError");

	return AtlReportError(CLSID_NMStart, 
						  Sc2ID(sc), 
						  IID_INMStt, 
						  ResultFromScode(sc), 
						  _Module.GetModuleInstance());
}


UINT CNMStt::Sc2ID(SCODE sc)
{
	UINT	nID;

	switch (sc)
	{
	case CTL_E_OUTOFMEMORY:
		nID = IDS_CTL_E_OUTOFMEMORY;
		break;
	case CNM_E_INVALIDARG:
		nID = IDS_CNM_E_INVALIDARG;
		break;
	case CNM_E_TOOMANYOPENCALLS:
		nID = IDS_CNM_E_TOOMANYOPENCALLS;
		break;
	case CNM_E_LISTENSTARTED:
		nID = IDS_CNM_E_LISTENSTARTED;
		break;
	case CNM_E_NMNOTINSTALLED:
		nID = IDS_CNM_E_NMNOTINSTALLED;
		break;

	default:
		nID = IDS_CNM_E_FAILURE;
	}
	return nID;
}


void CNMStt::WaitForThreadDead(HANDLE *phThread)
{
	ASSERT(phThread, "phThread is NULL in CNMStt::WaitForThreadDead");

	if (*phThread)
		// wait for the thread to terminate.
		::WaitForSingleObject(*phThread, INFINITE);
}


void CNMStt::FireOperationDone(DWORD dwResult, UINT uDispID)
{
	if (!m_bCtrlDestructed)
		Fire_OnOperationDone(0L == dwResult, uDispID);
}


HRESULT CNMStt::HrThreadConferenceListen(void)
{
	OutputDebugThreadIdString("CNMStt::HrThreadConferenceListen - Enter\n");

	DWORD dwRet = ConferenceListen(0);

	ASSERT(m_hwndMess, "m_hwndMess is NULL in CNMStt::HrThreadConferenceListen");

	CloseHandle(m_hThreadListen);
	m_hThreadListen = NULL;

	::PostMessage(m_hwndMess, WM_U_OPERATIONDONE, dwRet, 0x0A);

	OutputDebugThreadIdString("CNMStt::HrThreadConferenceListen - Leave\n");

	ExitThread(0L);

	return NOERROR;
}


HRESULT CNMStt::HrThreadConferenceConnect(void)
{
	HCONF    hConf;
	CONFINFO confInfo;
	CONFADDR confAddr;
	SHORT	 n;

	OutputDebugThreadIdString("CNMStt::HrThreadConferenceConnect - Enter\n");

	do 
	{
		for (n = 0; n < g_nMaxConnectCalls; n++)
			if (m_dwIPAddress[n] != 0L) break;

		if (n != g_nMaxConnectCalls)
		{
			// Check if we're in an existing conference
			ZeroMemory(&confInfo, sizeof(confInfo));
			confInfo.dwSize = sizeof(confInfo);
			if (CONFERR_SUCCESS == ConferenceGetInfo(NULL, CONF_ENUM_CONF, &confInfo))
				// use existing conference
				hConf = confInfo.hConf;
			else
				// startup a new one
				hConf = NULL;

			ZeroMemory(&confAddr, sizeof(confAddr));
			confAddr.dwSize = sizeof(confAddr);
			confAddr.dwAddrType = CONF_ADDR_IP;
			confAddr.dwIp = m_dwIPAddress[n];

			confInfo.dwMediaType = CONF_MT_DATA | CONF_MT_AUDIO;

			DWORD dwRet = ConferenceConnect(&hConf, &confAddr, &confInfo, NULL);

			::PostMessage(m_hwndMess, WM_U_OPERATIONDONE, dwRet, 0x0B);

			m_dwIPAddress[n] = 0L;
		}
	}
	while (n != g_nMaxConnectCalls);

	CloseHandle(m_hThreadConnect);
	m_hThreadConnect = NULL;

	OutputDebugThreadIdString("CNMStt::HrThreadConferenceConnect - Leave\n");

	ExitThread(0L);

	return NOERROR;
}

	
/////////////////////////////////////////////////////////////////////////////
// CNMStart's properties

STDMETHODIMP CNMStt::get_AcceptNetMeetingCalls(BOOL *pbAcceptNetMeetingCalls)
{
    CHECK_POINTER(pbAcceptNetMeetingCalls);

    *pbAcceptNetMeetingCalls = m_bAcceptNetMeetingCalls;

    return NOERROR;
}


STDMETHODIMP CNMStt::put_AcceptNetMeetingCalls(BOOL bAcceptNetMeetingCalls)
{
    m_bAcceptNetMeetingCalls = bAcceptNetMeetingCalls;

    return NOERROR;
}


STDMETHODIMP CNMStt::get_NetMeetingInstalled(BOOL *pbNetMeetingInstalled)
{
    CHECK_POINTER(pbNetMeetingInstalled);

    *pbNetMeetingInstalled = ENSURE_LOADED(g_hinstMSConf, c_tszMSConfDLL);

    return NOERROR;
}


STDMETHODIMP CNMStt::get_IPAddress(BSTR *pbstrIPAddress)
{
    CHECK_POINTER(pbstrIPAddress);

	HRESULT			hr = E_FAIL;
	WSADATA			wsaData; 
	char			szHostName[100];
	long			lHostID;
	struct hostent	*h2;

	// Load the DLL
	if (0 != WSAStartup(MAKEWORD(1, 1), &wsaData))
		goto exit;

	// Make sure that the winsock dll indeed supports 1.1
	if (1 != LOBYTE(wsaData.wVersion) || 1 != HIBYTE(wsaData.wVersion)) 
		goto exit;

	if (gethostname(szHostName, sizeof(szHostName)))
		goto exit;

	h2 = gethostbyname(szHostName);
	if (!h2 || h2->h_length < 1 || !h2->h_addr_list[0])
		goto exit;

	lHostID = *((long *) h2->h_addr_list[0]);
	lHostID = ntohl(lHostID);

	_ltoa(lHostID, szHostName, 10);

	*pbstrIPAddress = A2BSTR(szHostName);
	hr = NOERROR;

exit:
	WSACleanup();

	if (FAILED(hr))
		return HrThrowError(CNM_E_FAILURE);
	else
		return *pbstrIPAddress ? NOERROR : HrThrowError(CTL_E_OUTOFMEMORY);
}


/////////////////////////////////////////////////////////////////////////////
// CNMStart's methods

STDMETHODIMP CNMStt::OpenNetMeetingConference(BSTR bstrIPAddress)
{
	if (!ENSURE_LOADED(g_hinstMSConf, c_tszMSConfDLL))
		return HrThrowError(CNM_E_NMNOTINSTALLED);

	if (!bstrIPAddress || '\0' == *bstrIPAddress)
		return HrThrowError(CNM_E_INVALIDARG);

	USES_CONVERSION;

	LPTSTR szIPAddress = W2T(bstrIPAddress);

	if (!szIPAddress)
		return HrThrowError(CTL_E_OUTOFMEMORY);

	// Get free index
	for (SHORT n = 0; n < g_nMaxConnectCalls; n++)
		if (m_dwIPAddress[n] == 0L) break;

	if (n == g_nMaxConnectCalls)
		return HrThrowError(CNM_E_TOOMANYOPENCALLS);

	m_dwIPAddress[n] = (DWORD) atol(szIPAddress); // ConfConnect expects in host ordering, which is what was sent

	DWORD	dwThreadId;

	// ConfConnect can take a while (especially if NetMeeting setup required) so use separate thread.
	if (!m_hThreadConnect)
	{
		m_hThreadConnect = ::CreateThread(NULL,
										  0,
										  DwStartNetMeetingConnect,
										  this,
										  0,
										  &dwThreadId);
		if (m_hThreadConnect)
			return NOERROR;
		else
			return HrThrowError(CNM_E_FAILURE);
	}
	else 
		return NOERROR;
}


STDMETHODIMP CNMStt::InstallNetMeeting(void)
{
	const TCHAR szFilePrefix[]	= "TOTO";
	const TCHAR szFileSuffix[]	= ".HTM";
	const short MAX_HTMBUFF		= 32;

	HCURSOR	hCursor	= NULL;
	TCHAR	szExePath[MAX_PATH] = _T("");
	UINT	uExe = 0, uNumber = 1, uWinDirLen = 0;
	HANDLE	hFile = NULL;
	TCHAR	szURL[MAX_PATH];
	TCHAR	szNumber[MAX_HTMBUFF];
	TCHAR	szWinDir[MAX_PATH];
	TCHAR	szHtmTmp[MAX_PATH+MAX_HTMBUFF];

	if (!::LoadString(_Module.GetModuleInstance(), IDS_NETMEETING_URL, szURL, MAX_PATH))
		goto exit;

	hCursor = ::SetCursor(::LoadCursor(NULL, IDC_APPSTARTING));

	if (0 == (uWinDirLen = GetWindowsDirectory((LPTSTR) szWinDir, MAX_PATH)))
		goto exit;

	if ('\\' != szWinDir[uWinDirLen-1])
		_tcscat(szWinDir, "\\");

	do
	{
		_tcscpy(szHtmTmp, szWinDir);
		_tcscat(szHtmTmp, szFilePrefix);
		_itot(uNumber, szNumber, 10);
		_tcscat(szHtmTmp, szNumber);
		_tcscat(szHtmTmp, szFileSuffix);

		uNumber *= 2;

		hFile = CreateFile(szHtmTmp, 
						  0L /*dwDesiredAccess*/, 
						  0L /*dwShareMode*/,
						  NULL /*lpSecurityAttributes*/,
						  CREATE_NEW /*dwCreationDistribution*/,
						  FILE_ATTRIBUTE_TEMPORARY /*dwFlagsAndAttributes*/,
						  NULL /*hTemplateFile*/);
	}
	while (INVALID_HANDLE_VALUE == hFile && uNumber <= 1024);

	if (INVALID_HANDLE_VALUE != hFile)
	{
		uExe = (UINT) FindExecutable(szHtmTmp, szWinDir, szExePath);
		CloseHandle(hFile);
		DeleteFile(szHtmTmp);
	}

	if (uExe > 32)
		uExe = (UINT) ShellExecute(NULL, NULL, szExePath, szURL, NULL, SW_SHOWNORMAL);
	else
		uExe = (UINT) ShellExecute(NULL, NULL, szURL, NULL, NULL, SW_SHOWNORMAL);

	::SetCursor(hCursor);

exit:
	return (uExe > 32) ? NOERROR : HrThrowError(CNM_E_FAILURE);
}


STDMETHODIMP CNMStt::StartNetMeetingListen(void)
{
	DWORD dwThreadId;

	if (m_hThreadListen)
		return HrThrowError(CNM_E_LISTENSTARTED);

	if (!ENSURE_LOADED(g_hinstMSConf, c_tszMSConfDLL))
		return HrThrowError(CNM_E_NMNOTINSTALLED);
	else
	{
		m_hThreadListen = ::CreateThread(NULL,
										 0,
										 DwStartNetMeetingListen,
										 this,
										 0,
										 &dwThreadId);
		if (m_hThreadListen)
			return NOERROR;
		else
			return HrThrowError(CNM_E_FAILURE);
	}
}
