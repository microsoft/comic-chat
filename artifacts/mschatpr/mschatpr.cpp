// MsChatPr.cpp : Implementation of DLL Exports.


// Note: Proxy/Stub Information
//		To build a separate proxy/stub DLL, 
//		run nmake -f MsChatPrps.mk in the project directory.

#include "StdAfx.H"
#include "Resource.H"
#include "InitGuid.H"
#include "MsChatPr.H"
#include "MsChatPr_i.C"
#include "ChatPr.H"
#include "ChCol.H"
#include "ChObj.H"
#include "CItmsObj.H"
#include "CUtil.H"

// Global Module object
CComModule _Module;

// Global string for out of memory conditions
TCHAR	g_szOOM[g_nMaxLengthError];

// Global value for max message length
LONG	g_lMaxMsgLength;


BEGIN_OBJECT_MAP(ObjectMap)
	OBJECT_ENTRY(CLSID_MsChatPr, CMsChatPr)
	OBJECT_ENTRY(CLSID_Channels, CChannelsCollection)
	OBJECT_ENTRY(CLSID_Channel, CChannelObject)
	OBJECT_ENTRY(CLSID_ChatItems, CItemsObject)
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

STDAPI DllRegisterServer(void)
{
	// registers object, typelib and all interfaces in typelib
	return _Module.RegisterServer(TRUE);
}

/////////////////////////////////////////////////////////////////////////////
// DllUnregisterServer - Removes entries from the system registry

STDAPI DllUnregisterServer(void)
{
	_Module.UnregisterServer();

	// REGISB: Added 07/01/97
	// Unregister the typelib and all interfaces
	UnRegisterTypeLib(LIBID_MsChatPr, 2, 0, MAKELANGID(LANG_NEUTRAL, SUBLANG_NEUTRAL), SYS_WIN32);

	return S_OK;
}
