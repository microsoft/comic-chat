/*
	Microsoft Corp. (C) Copyright 1997
	Developed under contract by Numbers & Co.
----------------------------------------------------------------------------

        name:   XCChat - ActiveX CChat-document-container control

	 	file:	xcchat.cpp

    comments:	Implements DLL exports
     	
----------------------------------------------------------------------------
	Microsoft Corp. (C) Copyright 1997
	Developed under contract by Numbers & Co.
 */


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f xcchatps.mk in the project directory.

#include "stdafx.h"
#include "resource.h"
#include <string.h>
#include "initguid.h"
#include "xcchat.h"

#include "XControl.h"

DEFINE_GUID( CLSID_XCChat, 
	0x241AF500, 0x8FB6, 0x11CF, 0xAD, 0xC5, 0x00, 0xAA, 0x00, 0xBA, 0xDF, 0x6F ); 


CComModule _Module;

BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_XControl, CXControl)
END_OBJECT_MAP()

/////////////////////////////////////////////////////////////////////////////
// DLL Entry Point

extern "C"
BOOL WINAPI DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID /*lpReserved*/)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		_Module.Init(ObjectMap, hInstance);
		DisableThreadLibraryCalls(hInstance);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
		_Module.Term();
	return TRUE;    // ok
}

/////////////////////////////////////////////////////////////////////////////
// Used to determine whether the DLL can be unloaded by OLE

STDAPI DllCanUnloadNow(void)
{
	return (_Module.GetLockCount()==0) ? S_OK : S_FALSE;
}

/////////////////////////////////////////////////////////////////////////////
// Returns a class factory to create an object of the requested type

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, LPVOID* ppv)
{
	return _Module.GetClassObject(rclsid, riid, ppv);
}

/////////////////////////////////////////////////////////////////////////////
// DllRegisterServer - Adds entries to the system registry

BOOL CChatRegisterTypeLib(
LPCSTR pszTypeLib)
{
	USES_CONVERSION;

	char szKeyName[128];
	lstrcpy (szKeyName, "CLSID\\");
	LPOLESTR pszCLSID;
	if (FAILED(StringFromCLSID (CLSID_XCChat, &pszCLSID))) 
	{
		return FALSE;
	}
	lstrcat (szKeyName, W2A(pszCLSID));
	CoTaskMemFree (pszCLSID);
	lstrcat (szKeyName, "\\LocalServer32");

	char szFile[_MAX_PATH];
	long lSize = sizeof(szFile);
	if (RegQueryValue (HKEY_CLASSES_ROOT, szKeyName, szFile, &lSize) != ERROR_SUCCESS) 
	{
		return FALSE;
	}

	LPSTR pszSlash = strrchr (szFile, '\\');
	if (!pszSlash) 
	{
		return FALSE;
	}

	lstrcpy (pszSlash + 1, pszTypeLib);

	// Make sure the file exists.
	if (GetFileAttributes (szFile) == (DWORD)-1L)
	{
		return FALSE;
	}

	BOOL bSuccess = FALSE;
	LPTYPELIB ptlib = NULL;
	if (SUCCEEDED(LoadTypeLib(T2COLE(szFile), &ptlib)))
	{
		LPTLIBATTR pAttr;
		GUID tlidActual = GUID_NULL;

		if (SUCCEEDED(ptlib->GetLibAttr(&pAttr)))
		{
			tlidActual = pAttr->guid;
			ptlib->ReleaseTLibAttr(pAttr);
		}

		// Register the type library.
		if (SUCCEEDED(RegisterTypeLib(ptlib, T2OLE(szFile), NULL)))
			bSuccess = TRUE;

		ptlib->Release();
	}

	return bSuccess;
}

STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	HRESULT hr = _Module.RegisterServer(TRUE);
	if (SUCCEEDED(hr))
	{
		// If possible, register CChat's type library.
		CChatRegisterTypeLib ("icchat.tlb");
	}
	return hr;
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer();
	return S_OK;
}


