// ChObj.cpp : Implementation of CChannelObject
#include "StdAfx.H"
#include "MsChatPr.H"
#include "ChObj.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// CChannelObject

STDMETHODIMP CChannelObject::InterfaceSupportsErrorInfo(REFIID riid)
{
	static const IID* arr[] = 
	{
		&IID_IChannelObject,
	};
	for (int i=0;i<sizeof(arr)/sizeof(arr[0]);i++)
	{
		if (InlineIsEqualGUID(*arr[i],riid))
			return S_OK;
	}
	return S_FALSE;
}


HRESULT CChannelObject::HrThrowError(SCODE sc, LPCTSTR szChannelName, LPCTSTR szNickname)
{
	TCHAR szError[g_nMaxLengthError];

	ASSERT(sc != S_OK, "sc == S_OK in CChannelObject::HrThrowError");

	Sc2Sz(sc, szError, szChannelName ? szChannelName : m_prChannel.szName, szNickname, bIsIrcXChannel(), GetCharSet());

	return AtlReportError(CLSID_Channel, szError, IID_IChannelObject, ResultFromScode(sc));
}


HRESULT	CChannelObject::HrSetItemKey(LPCTSTR szItemKey)
{
	if (m_szItemKey)
		delete [] m_szItemKey;

	if (szItemKey)
	{
		DWORD cbLen = 0L;
		m_szItemKey = new TCHAR[(cbLen = lstrlen(szItemKey)) + 1];
		if (!m_szItemKey)
			return E_OUTOFMEMORY;
		lstrcpyn(m_szItemKey, szItemKey, cbLen+1);
	}
	else
		m_szItemKey = NULL;

	return NOERROR;
}

