//=--------------------------------------------------------------------------=
// Notif.Cpp:		Implementation of notifications C++ classes
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 03/09/98

#include "stdafx.h"
#include "notif.h"
#include "chat.h"
#include "actions.h"
#include "cdebug.h"
#include "ccommon.h"
#include "ui.h"

// for ASSERT and FAIL
//
SZTHISFILE

#ifdef DEBUG
SHORT g_nNotifsRefCount;
#endif

NETVALID_FN CCNotif::m_pfNetValid = (NETVALID_FN) bNetValid;

/////////////////////////////////////////////////////////////////////////////
// CCNotif::CCNotif - Constructor
CCNotif::CCNotif()
{
	m_nRefCount		= 1;
	#ifdef DEBUG
		g_nNotifsRefCount++;
	#endif

	m_pDaemonExt	= NULL;
	m_wFlags		= 0;
}


CCNotif::CCNotif(CCNotif* pNotif)
{
	ASSERT(pNotif, "pNotif is NULL in CCNotif::CCNotif");
	m_nRefCount = 1;
	#ifdef DEBUG
		g_nNotifsRefCount++;
	#endif

	m_wFlags		= pNotif->m_wFlags;

	if (m_pDaemonExt = pNotif->m_pDaemonExt)
		m_pDaemonExt->AddRef();

	CopyNotif(pNotif);
}


BOOL CCNotif::operator==(const CCNotif& notif)
{
	UCHAR uParam;

	for (uParam = g_uNickname; uParam <= g_uHostName; uParam++)
		if (m_uOperators[uParam] != notif.m_uOperators[uParam])
			return FALSE;

	for (uParam = g_uNickname; uParam <= g_uNetName; uParam++)
		if (m_strParams[uParam].CompareNoCase(notif.m_strParams[uParam]))
			return FALSE;

	if (m_strParams[g_uNetName].CompareNoCase(notif.m_strParams[g_uNetName]))
		return FALSE;

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CCNotif::CopyNotif
void CCNotif::CopyNotif(CCNotif* pNotif)
{
	// Copy of all parameters that can change in the EditNotif dialog
	ASSERT(pNotif, "pNotif is NULL in CCNotif::CopyNotif");

	m_wFlags		= pNotif->m_wFlags;

	for (UCHAR uParam = g_uNickname; uParam <= g_uHostName; uParam++)
	{
		m_strParams[uParam] = pNotif->m_strParams[uParam];
		m_uOperators[uParam] = pNotif->m_uOperators[uParam];
	}
	m_strParams[g_uNetName] = pNotif->m_strParams[g_uNetName];
}

	
/////////////////////////////////////////////////////////////////////////////
// CCNotif::CCNotif - Destructor
CCNotif::~CCNotif()
{
	if (m_pDaemonExt)
		m_pDaemonExt->Release();
}


void CCNotif::AddRef()
{
	m_nRefCount++;

	#ifdef DEBUG
		g_nNotifsRefCount++;
	#endif
}


void CCNotif::Release()
{
	#ifdef DEBUG
		ASSERT(g_nNotifsRefCount > 0, "m_nRefCount <= 0 in CCNotif::Release");
		g_nNotifsRefCount--;
	#endif

	ASSERT(m_nRefCount > 0, "m_nRefCount <= 0 in CCNotif::Release");

	if (--m_nRefCount == 0)
		delete this;	
}


INT	CCNotif::Serialize(LPTSTR szBuff, INT cbBuffLen)
{
	LPTSTR	szTmp;
	UINT	uIndex;
	UINT	rguParamLen[g_uNotifParamNum];

	ASSERT(szBuff, "szBuff is NULL in CCNotif::Serialize");

	// 1 byte for notif version
	// 2 bytes for notif status
	// 1 byte for number of operator+argument couples (gives more flexibility for the future)
	// 1 byte for number of single params
	// 3 (operator+arg)	couples
	// 1 single param
	INT		cbTotal = g_uNotifParamNum+4;
	CString strAny;

	for (uIndex = g_uNickname; uIndex <= g_uHostName; uIndex++)
		if (m_uOperators[uIndex] != g_uAny)
			cbTotal += (rguParamLen[uIndex] = m_strParams[uIndex].GetLength() + 1);  // +1 for terminating NULL
	
	strAny.LoadString(IDS_KEY_EVENT_PARAM0 + (UINT) kepAny);
	ASSERT(!strAny.IsEmpty(), "strAny.IsEmpty() in CCNotif::Serialize");

	if (0 == strAny.CompareNoCase(m_strParams[g_uNetName]))
		cbTotal += (rguParamLen[g_uNetName] = 1);	// we'll just write a 0x00
	else
		cbTotal += (rguParamLen[g_uNetName] = m_strParams[g_uNetName].GetLength() + 1);

	if (cbTotal > cbBuffLen)
	{
		ASSERT(FALSE, "buffer too small in CCNotif::Serialize");
		return -1;
	}

	szTmp = szBuff;

	*szTmp = g_wVersion;
	szTmp++;

	*(WORD*) szTmp = m_wFlags;
	szTmp += sizeof(WORD);

	*szTmp = g_uNotifParamNum-1;		// 3 (operator+arg) couples
	szTmp++;

	*szTmp = 1;							// 1 single string param
	szTmp++;

	for (uIndex = g_uNickname; uIndex <= g_uHostName; uIndex++)
	{
		*szTmp = m_uOperators[uIndex];
		szTmp++;
		if (m_uOperators[uIndex] != g_uAny)
		{
			strncpy(szTmp, m_strParams[uIndex], rguParamLen[uIndex]);
			szTmp += rguParamLen[uIndex];
		}
	}

	if (1 == rguParamLen[g_uNetName])
		*szTmp = 0;	// for the %Any% keyword - we don't want to write the localized version of %Any% into the reg database
	else
		strncpy(szTmp, m_strParams[g_uNetName], rguParamLen[g_uNetName]);
	szTmp += rguParamLen[g_uNetName];

	ASSERT(cbTotal == (szTmp-szBuff), "cbTotal != (szTmp-szBuff) in CCNotif::Serialize");
	return cbTotal;
}


INT	CCNotif::UnSerialize(LPBYTE pbBuff, INT cbBuffLen)
{
	ASSERT(pbBuff, "pbBuff is NULL in CCNotif::UnSerialize");

	UINT	uIndex;
	LPBYTE	pbTmp = pbBuff;
	INT		cbLeft = cbBuffLen, cbLen;

	if (cbLeft < 3 + sizeof(WORD))
		goto exit;

	if (g_wVersion != (WORD) *pbTmp)
		goto exit;

	pbTmp++;
	cbLeft--;

	m_wFlags = *((WORD*) pbTmp);
	if (m_wFlags & ~g_wActive)
		goto exit;

	pbTmp += sizeof(WORD);
	cbLeft -= sizeof(WORD);

	if (g_uNotifParamNum-1 != *pbTmp)
		goto exit;

	pbTmp++;
	cbLeft--;

	if (1 != *pbTmp)
		goto exit;

	pbTmp++;
	cbLeft--;

	for (uIndex = g_uNickname; uIndex <= g_uHostName; uIndex++)
	{
		if (cbLeft < 1)
			goto exit;

		m_uOperators[uIndex] = *pbTmp;

		pbTmp++;
		cbLeft--;

		if (m_uOperators[uIndex] > g_uEndsWith)
			goto exit;

		if (m_uOperators[uIndex] != g_uAny)
		{
			cbLen = 0;
			while (cbLen < cbLeft && cbLen < g_uMaxNotifParamLength && *(pbTmp+cbLen) != 0)
				cbLen++;
			if (*(pbTmp+cbLen) != 0)
				// no NULL terminating string or too long
				goto exit;
			m_strParams[uIndex] = (LPTSTR) pbTmp;
			pbTmp += cbLen+1;
			cbLeft -= cbLen+1;
		}
	}
	
	// read single param
	cbLen = 0;
	while (cbLen < cbLeft && cbLen < g_uMaxNetArgLength && *(pbTmp+cbLen) != 0)
		cbLen++;
	if (*(pbTmp+cbLen) != 0)
		// no NULL terminating string or too long
		goto exit;
	if (0 == cbLen)
	{
		CString strAny;
		strAny.LoadString(IDS_KEY_EVENT_PARAM0 + (UINT) kepAny);
		ASSERT(!strAny.IsEmpty(), "strAny.IsEmpty() in CCNotif::UnSerialize");
		m_strParams[g_uNetName] = strAny;
	}
	else
		m_strParams[g_uNetName] = (LPTSTR) pbTmp;
	cbLeft -= cbLen+1;

	ASSERT(!m_pDaemonExt, "m_pDaemonExt is NOT NULL in CCNotif::UnSerialize");

	bUpdateDaemonExt(TRUE);

	return cbBuffLen - cbLeft;

exit:
	m_wFlags = 0;
	return 0;
}


BOOL CCNotif::bUpdateDaemonExt(BOOL bResetItemLists)
{
	// Clean up the potential daemon lists
	if (bActive())
	{
		if (m_pDaemonExt)
		{
			if (bResetItemLists || m_pDaemonExt->m_bResetItemLists)
			{
				m_pDaemonExt->m_bResetItemLists = FALSE;
				bResetItemLists = TRUE;
				m_pDaemonExt->bCleanUpItemLists();
			}
		}
		else
		{
			m_pDaemonExt = new CCDaemonExt(itUser);
			bResetItemLists = TRUE;
		}

		if (m_pDaemonExt && bResetItemLists)
			if (!m_pDaemonExt->bAllocNewItemList(2))
			{
				TRACE("Could not allocate m_pDaemonExt's user list in CCNotif::bUpdateDaemonExt\n");
				m_pDaemonExt->Release();
				m_pDaemonExt = NULL;
				return FALSE;
			}
	}
	else
		if (m_pDaemonExt)
		{
			m_pDaemonExt->Release();
			m_pDaemonExt = NULL;
		}
	return TRUE;
}


BOOL CCNotif::bDaemonNeeded()
{
	if (!bActive())
		return FALSE;

	CString strAny;
	
	strAny.LoadString(IDS_KEY_EVENT_PARAM0 + (UINT) kepAny);
	ASSERT(!strAny.IsEmpty(), "strAny.IsEmpty() in CCNotif::bDaemonNeeded");

	if (0 == strAny.CompareNoCase(m_strParams[g_uNetName]))
		return TRUE;

	return m_pfNetValid(m_strParams[g_uNetName]);
}


/////////////////////////////////////////////////////////////////////////////
// CCDynaNotifs::CCDynaNotifs - Constructor
CCDynaNotifs::CCDynaNotifs()
{
	m_pfDisplayNotifications= NULL;
	m_pfSignalNewUpdate		= NULL;
	m_pfDaemonQuery			= NULL;
	m_bDaemonRunning		= FALSE;
	m_uWhosCount			= 0;
	m_uUpdateCount			= 0;
	m_uModifiedUsersCount	= 0;
	SetFlags(0);
}


/////////////////////////////////////////////////////////////////////////////
// CCDynaNotifs::CCDynaNotifs - Destructor
CCDynaNotifs::~CCDynaNotifs()
{
	CleanUpNotifsArray();
	bRemoveAllUsers();
}


/////////////////////////////////////////////////////////////////////////////
// CCDynaNotifs::operator=
const CCDynaNotifs& CCDynaNotifs::operator=(const CCDynaNotifs& dynaNotifs)
{
	CleanUpNotifsArray();

	CCNotif	*pNotif, *pNotifCopy;
	INT		iIndex, iNotifs = dynaNotifs.m_rgpNotifs.GetSize();

	for (iIndex = 0; iIndex < iNotifs; iIndex++)
	{
		pNotif = (CCNotif*) dynaNotifs.m_rgpNotifs.GetAt(iIndex);
		ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::operator=");
		if (pNotifCopy = new CCNotif(pNotif/*, this*/))
			m_rgpNotifs.Add((void*) pNotifCopy);
	}
	
	m_pfDisplayNotifications= dynaNotifs.m_pfDisplayNotifications;
	m_pfSignalNewUpdate		= dynaNotifs.m_pfSignalNewUpdate;
	m_pfDaemonQuery			= dynaNotifs.m_pfDaemonQuery;
	
	m_bDaemonRunning		= dynaNotifs.m_bDaemonRunning;
	m_wFlags				= dynaNotifs.m_wFlags;

	m_strStartUpIdent		= dynaNotifs.m_strStartUpIdent;

	// m_uUpdateCount + m_uWhosCount + m_uModifiedUsersCount + m_rgpNotifUsers not heritated
	return *this;
}


void CCDynaNotifs::CleanUpNotifsArray()
{
	CCNotif*	pNotif;
	INT			iNotifs, iIndex;

	if (iNotifs = m_rgpNotifs.GetSize())
	{
		for (iIndex = 0; iIndex < iNotifs; iIndex++)
		{
			pNotif = (CCNotif*) m_rgpNotifs.GetAt(iIndex);
			ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::CleanUpNotifsArray");
			pNotif->Release();
		}
		m_rgpNotifs.RemoveAll();
	}
}


BOOL CCDynaNotifs::bRemoveAllUsers()
{
	m_rgpNotifUsers.FreeRemoveAll();
	return TRUE;
}


BOOL CCDynaNotifs::bRemoveUsersWithoutFlag(WORD wFlag)
{
	INT		iIndex, iUsers = m_rgpNotifUsers.GetSize();
	CUser*	pUser;

	for (iIndex = 0; iIndex < iUsers; iIndex++)
	{
		pUser = (CUser*) m_rgpNotifUsers.GetAt(iIndex);
		ASSERT(pUser, "pUser is NULL in CCDynaNotifs::bRemoveUsersWithoutFlag");
		if (!(pUser->GetFlags() & wFlag))
		{
			m_rgpNotifUsers.RemoveAt(iIndex);
			pUser->Release();
			iUsers--;
		}
	}
	return TRUE;
}


BOOL CCDynaNotifs::bRemoveFlagsFromAllUsers(WORD wFlags)
{
	INT		iIndex, iUsers = m_rgpNotifUsers.GetSize();
	CUser*	pUser;

	for (iIndex = 0; iIndex < iUsers; iIndex++)
	{
		pUser = (CUser*) m_rgpNotifUsers.GetAt(iIndex);
		ASSERT(pUser, "pUser is NULL in CCDynaNotifs::bRemoveFlagsFromAllUsers");
		pUser->SetFlags(pUser->GetFlags() & ~wFlags);
	}
	return TRUE;
}


BOOL CCDynaNotifs::bUpdateNotifs()
{
	// start or queue new update
	if (0 == m_uWhosCount)
	{
		bUpdateNotifsDaemonExt(TRUE /*bResetItemLists*/);
		bRemoveAllUsers();
		if (bDaemonNeeded())
			OnNotifsDaemonTimer();
		return TRUE;
	}
	else
	{
		m_uUpdateCount++;
		return FALSE;
	}
}


BOOL CCDynaNotifs::bUpdateNotifsDaemonExt(BOOL bResetItemLists)
{
	CCNotif*	pNotif;
	INT			iIndex, iNotifs = m_rgpNotifs.GetSize();
	BOOL		bRet = TRUE;

	for (iIndex = 0; iIndex < iNotifs; iIndex++)
	{
		pNotif = (CCNotif*) m_rgpNotifs.GetAt(iIndex);
		ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::bUpdateDaemonExt");
		bRet &= pNotif->bUpdateDaemonExt(bResetItemLists);
	}

	return bRet;
}


BOOL CCDynaNotifs::bNotifExists(CCNotif* pNotif)
{
	ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::bNotifExists");

	CCNotif*	pNotifTmp;
	INT			iIndex, iNotifs = m_rgpNotifs.GetSize();

	for (iIndex = 0; iIndex < iNotifs; iIndex++)
	{
		pNotifTmp = (CCNotif*) m_rgpNotifs.GetAt(iIndex);
		ASSERT(pNotifTmp, "pNotifTmp is NULL in CCDynaNotifs::bNotifExists");
		if (*pNotifTmp == *pNotif)
			return TRUE;
	}
	return FALSE;
}


BOOL CCDynaNotifs::bAddNotif(CCNotif* pNotif, INT iIndex /* = -1 */)
{
	ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::bAddNotif");

	if (iIndex < 0)
		m_rgpNotifs.Add((void*) pNotif);				// Append notif at the end of the array
	else
		m_rgpNotifs.InsertAt(iIndex, (void*) pNotif);	// Insert in position iIndex
	return TRUE;
}


BOOL CCDynaNotifs::bRemoveNotif(CCNotif* pNotif, INT iIndex /* = -1 */)
{
	INT	iIndexTmp, iNotifs;

	ASSERT(pNotif || iIndex >= 0, "pNotif is NULL and iIndex < 0 in CCDynaNotifs::bRemoveNotif");

	if (iIndex < 0)
	{
		iIndexTmp = 0;
		iNotifs = m_rgpNotifs.GetSize();

		while (iIndexTmp < iNotifs && pNotif != (CCNotif*) m_rgpNotifs.GetAt(iIndexTmp))
			iIndexTmp++;
	
		if (iIndexTmp > iNotifs)
			return FALSE;
	}
	else
	{
		iIndexTmp = iIndex;
		ASSERT(iIndexTmp < m_rgpNotifs.GetSize(), "iIndexTmp >= m_rgpNotifs.GetSize() in CCDynaNotifs::bRemoveNotif");
		pNotif = (CCNotif*) m_rgpNotifs.GetAt(iIndexTmp);
		ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::bRemoveNotif");
	}

	pNotif->Desactivate();
	pNotif->Release();
	m_rgpNotifs.RemoveAt(iIndexTmp);

	return TRUE;
}


BOOL CCDynaNotifs::bSortNotifs()
{
	UCHAR	uSortColumn = m_wFlags >> 12;
	BOOL	bSortAscending = !(m_wFlags & g_wSortDescending);
	INT		iOrder, iNotif, iIndex, iNotifs = m_rgpNotifs.GetSize();
	DWORD	*pdwNotifs;
	CCNotif	*pNotif, *pNotif2;

	if (iNotifs)
	{
		if (!(pdwNotifs = new DWORD[iNotifs]))
			return FALSE;

		for (iNotif = 0; iNotif < iNotifs; iNotif++)
			pdwNotifs[iNotif] = (DWORD) m_rgpNotifs.GetAt(iNotif);

		m_rgpNotifs.RemoveAll();

		for (iNotif = 0; iNotif < iNotifs; iNotif++)
		{
			pNotif = (CCNotif*) pdwNotifs[iNotif];
			for (iIndex = 0; iIndex < iNotif; iIndex++)
			{
				pNotif2 = (CCNotif*) m_rgpNotifs.GetAt(iIndex);
				ASSERT(pNotif2, "pNotif2 is NULL in CCDynaNotifs::bSortNotifs");
				iOrder = pNotif->GetParam(uSortColumn).CompareNoCase(pNotif2->GetParam(uSortColumn));
				if ((iOrder <= 0 && bSortAscending) || (iOrder >= 0 && !bSortAscending))
					break;
			}
			m_rgpNotifs.InsertAt(iIndex, (void*) pNotif);
		}
		
		delete [] pdwNotifs;
	}

	return TRUE;
}


BOOL CCDynaNotifs::bSaveNotifsToReg()
{
	DWORD		dwValues, dwFlags;
	INT			iNotifs, iIndexNotif, cbNotif;
	HKEY		hKey = NULL;
	TCHAR		szBuff[g_uMaxSerializedNotif];
	CString		strNotifsKey(szRootRegKeyName), strRegName;
	CCNotif*	pNotif;

	strNotifsKey += g_szNotificationsSubKey;
	if (RegCreateKeyEx(HKEY_CURRENT_USER, strNotifsKey, 
					   0, (LPTSTR) g_szNotificationsClass, REG_OPTION_NON_VOLATILE,
					   KEY_ALL_ACCESS, NULL, &hKey, NULL) == ERROR_SUCCESS)
	{
		ASSERT(hKey, "hKey is NULL in CCDynaNotifs::bSaveNotifsToReg");
		iNotifs = m_rgpNotifs.GetSize();

		if (ERROR_SUCCESS == RegQueryInfoKey(hKey, 
											 NULL /*lpClass*/,
											 NULL /*lpcbClass*/,
											 NULL /*lpReserved*/,
											 NULL /*lpcSubKeys*/, 
											 NULL /*lpcbMaxSubKeyLen*/,
											 NULL /*lpcbMaxClassLen*/, 
											 &dwValues,
											 NULL /*lpcbMaxValueNameLen*/,
											 NULL /*lpcbMaxValueLen*/,
											 NULL /*lpcbSecurityDescriptor*/,
											 NULL /*lpftLastWriteTime*/))
			for (iIndexNotif = iNotifs; iIndexNotif < (INT) dwValues; iIndexNotif++)
			{
				strRegName.Format("%d", iIndexNotif);
				RegDeleteValue(hKey, strRegName);
			}

		dwFlags = MAKELONG(m_wFlags, g_wVersion);	// low + high
		if (ERROR_SUCCESS == RegSetValueEx(hKey, g_szNotificationFlags, 0, REG_DWORD, (CONST BYTE*) &dwFlags, sizeof(DWORD)))
			for (iIndexNotif = 0; iIndexNotif < iNotifs; iIndexNotif++)
			{
				strRegName.Format("%d", iIndexNotif);
				pNotif = (CCNotif*) m_rgpNotifs.GetAt(iIndexNotif);
				ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::bSaveNotifsToReg");
				if ((cbNotif = pNotif->Serialize(szBuff, g_uMaxSerializedNotif)) > 0)
					RegSetValueEx(hKey, strRegName, 0, REG_BINARY, (const unsigned char*)(LPCTSTR) szBuff, cbNotif);
			}
		RegCloseKey(hKey);
		return TRUE;
	}
	else
	{
		ASSERT(FALSE, "Couldn't open regkey for notifications in CCDynaNotifs::bSaveNotifsToReg");
		return FALSE;
	}
}


BOOL CCDynaNotifs::bLoadNotifsFromReg()
{
	HKEY		hKey = NULL;
	TCHAR		szValueName[g_uMaxNotifKeyLength];
	DWORD		dwFlags;
	BYTE		pbData[g_uMaxSerializedNotif];
	CString		strNotifsKey(szRootRegKeyName), strRegName;
	CCNotif*	pNotif = NULL;
	DWORD		dwIndex, dwType, cbValueName = g_uMaxNotifKeyLength, cbData = g_uMaxSerializedNotif;
	BOOL		bRet = FALSE;

	strNotifsKey += g_szNotificationsSubKey;
	if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER, strNotifsKey, 
									  0, KEY_ENUMERATE_SUB_KEYS | KEY_EXECUTE | KEY_QUERY_VALUE, &hKey))
	{
		dwIndex = 0;
		cbValueName = g_uMaxNotifKeyLength;
		while (ERROR_SUCCESS == RegEnumValue(hKey, dwIndex, szValueName, &cbValueName, NULL, &dwType, pbData, &cbData))
		{
			if (dwType == REG_DWORD && 0 == _tcscmp(g_szNotificationFlags, szValueName))
			{
				dwFlags = *((DWORD*) pbData);
				if ((HIWORD(dwFlags) != g_wVersion) /*|| (LOWORD(dwFlags) & ~g_wActive)*/)
					m_wFlags = 0;
				else
					m_wFlags = LOWORD(dwFlags);
			}
			else
				if (dwType == REG_BINARY)
				{
					if (!pNotif)
						if (!(pNotif = (CCNotif*) new CCNotif(/*this*/)))
							goto exit;
					if (cbData == pNotif->UnSerialize(pbData, cbData))
					{
						bAddNotif(pNotif);
						pNotif = NULL;
					}
				}
			dwIndex++;
			cbValueName = g_uMaxNotifKeyLength;
			cbData = g_uMaxSerializedNotif;
		}
		bRet = TRUE;
		goto exit;
	}
	return TRUE;

exit:
	if (hKey)
		RegCloseKey(hKey);
	if (pNotif)
		pNotif->Release();
	return bRet;
}


BOOL CCDynaNotifs::bDaemonNeeded()
{
	CCNotif*	pNotif;
	INT			iIndex = 0, iNotifs = m_rgpNotifs.GetSize();

	while (iIndex < iNotifs)
	{
		pNotif = (CCNotif*) m_rgpNotifs.GetAt(iIndex);
		ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::bDaemonNeeded");
		if (pNotif->bDaemonNeeded())
			return TRUE;
		else
			iIndex++;
	}
	return FALSE;
}


BOOL CCDynaNotifs::bStartNotifsDaemon(UINT uNotifsDaemonElapse, BOOL bForceReset)
{
	if (m_bDaemonRunning && !bForceReset)
		return TRUE;

	ASSERT(GetFrame(), "GetFrame() return NULL in CCDynaNotifs::bStartNotifsDaemon");

	bStopNotifsDaemon();

	if (GetFrame())
		m_bDaemonRunning = (g_uNotifsDaemonTimer == GetFrame()->SetTimer(g_uNotifsDaemonTimer, 1000 * uNotifsDaemonElapse + 60, NULL));

	ASSERT(m_bDaemonRunning, "m_bDaemonRunning is FALSE in CCDynaNotifs::bStartNotifsDaemon()");

	return m_bDaemonRunning;
}


BOOL CCDynaNotifs::bStopNotifsDaemon()
{
	if (!m_bDaemonRunning)
		return TRUE;

	m_bDaemonRunning = FALSE;

	if (GetFrame())
		return GetFrame()->KillTimer(g_uNotifsDaemonTimer);

	return TRUE;
}


void CCDynaNotifs::OnNotifsDaemonTimer()
{
	CCNotif*	pNotif;
	INT			iIndex = 0, iNotifs = m_rgpNotifs.GetSize();
	BOOL		bPass;

	OutputDebugThreadIdString("CCDynaNotifs::OnNotifsDaemonTimer - Enter\n");

	// Switch to the long period
	ASSERT(m_bDaemonRunning, "m_bDaemonRunning is FALSE in CCDynaNotifs::OnNotifsDaemonTimer");
	bStartNotifsDaemon(g_uNotifsDaemonLongElapse, TRUE);

	if (0 == m_uWhosCount)	// we don't want to overload the server - natural previous WHOs request are not completed yet
	{
		while (iIndex < iNotifs)
		{
			pNotif = (CCNotif*) m_rgpNotifs.GetAt(iIndex);
			ASSERT(pNotif, "pNotif is NULL in CCDynaNotifs::OnNotifsDaemonTimer");
			if (pNotif->bDaemonNeeded() && pNotif->m_pDaemonExt)
			{
				bPass = TRUE;	// Once Network/Servers combo is functional, make sure the current server/network matches properly

				ASSERT(m_pfDaemonQuery, "m_pfDaemonQuery is NULL in CCDynaNotifs::OnNotifsDaemonTimer");
				if (bPass)
				{
					BOOL bRet = m_pfDaemonQuery(pNotif);
					ASSERT(bRet, "m_pfDaemonQuery call failed in CCDynaNotifs::OnNotifsDaemonTimer");
					if (bRet)
						m_uWhosCount++;
				}
			}
			iIndex++;
		}
	}
}


INT CCDynaNotifs::iFindUserIndex(CUser* pUser)
{
	ASSERT(pUser, "pUser is NULL in CCDynaNotifs::iFindUserIndex");

	INT		iIndex, iUsers = m_rgpNotifUsers.GetSize();
	CUser*	pUserTmp;

	for (iIndex = 0; iIndex < iUsers; iIndex++)
	{
		pUserTmp = (CUser*) m_rgpNotifUsers.GetAt(iIndex);
		ASSERT(pUserTmp, "pUserTmp is NULL in CCDynaNotifs::iFindUserIndex");
		if (pUser->m_strNickname == pUserTmp->m_strNickname &&
			pUser->m_strIdentity == pUserTmp->m_strIdentity)
			return iIndex;
	}
	return -1;
}


BOOL CCDynaNotifs::bAddNotificationUser(CUser* pUser)
{
	ASSERT(pUser, "pUser is NULL in CCDynaNotifs::bAddNotificationUser");

	// Maybe find a match
	INT iIndex = iFindUserIndex(pUser);

	if (iIndex >= 0)
		return bModifyNotificationUser(pUser, g_wConnected /*wAddFlags*/, 0 /*wRemoveFlags*/, iIndex);

	if (m_rgpNotifUsers.Add((void*) pUser) >= 0)
	{
		pUser->SetFlags(g_wVisible | g_wConnected | g_wNew | g_wAltered);
		pUser->AddRef();
		m_uModifiedUsersCount++;
		return TRUE;
	}

	ASSERT(FALSE, "Couldn't add user in CCDynaNotifs::bAddNotificationUser");
	return FALSE;
}


BOOL CCDynaNotifs::bModifyNotificationUser(CUser* pUser, WORD wAddFlags, WORD wRemoveFlags, INT iIndex)
{
	ASSERT(pUser, "pUser is NULL in CCDynaNotifs::bModifyNotificationUser");

	// Find a match
	INT iIndexTmp;
		
	if (iIndex < 0)
	{
		iIndexTmp = iFindUserIndex(pUser);

		if (iIndexTmp < 0)
			return TRUE;
	}
	else
		iIndexTmp = iIndex;

	ASSERT(iIndexTmp < m_rgpNotifUsers.GetSize(), "Unexpected iIndexTmp value in CCDynaNotifs::bModifyNotificationUser");

	// Get the stored user
	CUser*	pUserTmp = (CUser*) m_rgpNotifUsers.GetAt(iIndexTmp);
	ASSERT(pUserTmp, "pUserTmp is NULL in CCDynaNotifs::bModifyNotificationUser");

	// Update the user flags
	WORD	wFlags = pUserTmp->GetFlags();
	BOOL	bAlreadyAltered = (wFlags & g_wAltered);

	wFlags |= wAddFlags | g_wVisible | g_wNew | g_wAltered;
	wFlags &= ~wRemoveFlags;

	pUserTmp->SetFlags(wFlags);

	if (wFlags & g_wConnected)
	{
		pUserTmp->m_strPrettyRoom = pUser->m_strPrettyRoom;
		pUserTmp->m_strRoom = pUser->m_strRoom;
	}
	else
	{
		pUserTmp->m_strPrettyRoom = pUserTmp->m_strRoom = "";
	}

	// Put this user to the end of the array
	m_rgpNotifUsers.RemoveAt(iIndexTmp);

	if (m_rgpNotifUsers.Add((void*) pUserTmp) >= 0)
	{
		if (!bAlreadyAltered)
			m_uModifiedUsersCount++;
		return TRUE;
	}
	else
	{
		ASSERT(FALSE, "Couldn't move user in CCDynaNotifs::bModifyNotificationUser");
		return FALSE;
	}
}

