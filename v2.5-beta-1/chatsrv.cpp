#include "stdafx.h"
#include "resource.h"
#include "defines.h"
#include "utils.h"
#include "chatsrv.h"
#include "chat.h"
#include "ircproto.h"
#include <afxpriv.h>

extern BOOL bForPath(const char *szPath, BOOL soundFunc(const char *, void *), void *pvData);


// =================================================================================
// Little wrappers around Winsock, the way MFC does it. MFC, oddly, does not
// let us use its implementation. Note that we don't use this right now, but we
// will automatically start using it should we link with the DLL version of MFC.

struct WINSOCKINSTANCESTRUCT
{
	~WINSOCKINSTANCESTRUCT()
		{ if (m_hinst) free (m_hinst); }
	u_short (PASCAL * htons)(u_short hostshort);
	struct hostent* (PASCAL * gethostbyname)(const char * name);
	unsigned long (PASCAL * inet_addr) (const char FAR * cp);
	HINSTANCE m_hinst;
};

WINSOCKINSTANCESTRUCT WinSockInstance = 
{
	htons,
	gethostbyname,
	inet_addr,
	NULL
};

// =================================================================================
// CChatServer implementation

CChatServer::CChatServer(
LPCSTR pszName,
PVOID  pvData,
UINT   nDataLen)
{
	m_pszName = strdup (pszName);
	SetDefaultSettings ();
	if (nDataLen > 0)
		ReadFromData (pvData, nDataLen);
	ZeroMemory (&m_sockaddr, sizeof(m_sockaddr));
}

CChatServer::~CChatServer()
{
	free (m_pszName);
	FreeSettings ();
}

// Read in settings for the server from given data. A chat server's data is 
// stored in the format given below. Each record is stored as a one byte 
// record type, followed by a variable number of bytes for the record data.
// If the record is not present, the default applies.
//		
//		RECORD				# BYTES		DATA DESCRIPTION	DEFAULT
//		datatypePort		2 	   		Port number			6667
//		datatypeAuthenticationType
//							1			Type (0-3)			0
//		datatypeUserName	encoded		User Name			none
//		datatypePassword	encoded		Password			none (prompt if needed)
//		datatypeSecurityPkg
//							string		Security packages	none

void 
CChatServer::ReadFromData(
PVOID pvData,
UINT nDataLen)
{
	// Set defaults.
	FreeSettings ();
	SetDefaultSettings ();

	PBYTE pbData = (PBYTE)pvData;
	BYTE byType;
	while (nDataLen-- > 0)
	{
		byType = *(pbData++);
		switch (byType)
		{
			case datatypePort:
				if (nDataLen >= 2)
				{
					m_nPort = *((PWORD)pbData);
				}
				pbData += 2;
				nDataLen -= 2;
				break;
			case datatypeAuthenticationType:
				if (nDataLen >= 1)
				{
					m_nAuthenticationType = *pbData;
				}
				pbData++;
				nDataLen--;
				break;

			case datatypeUserName:
			case datatypeUserPassword:
			{
				CString strData;
				int nEncryptionType = 0;
				if (nDataLen >= 1)
				{
					nEncryptionType = *pbData;
				}
				pbData++;
				nDataLen--;
				switch (nEncryptionType)
				{
					case 0:	// No encryption
					{
						LPCSTR pszStart = (LPCSTR)pbData;
						while (nDataLen > 0 && *pbData != '\0')
						{
							pbData++;
							nDataLen--;
						}
						if (pbData > (PBYTE)pszStart)
						{
							strData = CString (pszStart, ((LPCSTR)pbData) - pszStart);
						}
						pbData++;
						nDataLen--;
						break;
					}

					case 1:
					{
						BYTE bySrc[68], byDest[32];
						if (nDataLen >= 68)
						{
							memcpy (bySrc, pbData, 68);
							if (encDecodeData (byDest, bySrc, sizeof(byDest)))
							{
								strData = (LPCSTR)byDest;
							}
						}
						pbData += 68;
						nDataLen -= 68;
						break;
					}
				}

				if (!strData.IsEmpty ())
				{
					if (byType == datatypeUserName)
						m_pszUserName = strdup (strData);
					else
						m_pszPassword = strdup (strData);
				}
				break;
			}
			
			case datatypeRememberPassword:
			{
				if (nDataLen >= 1)
				{
					m_bRememberPassword = *pbData;
				}
				pbData++;
				nDataLen--;
			}

			case datatypeSecurityPkg:
			{
				LPCSTR pszStart = (LPCSTR)pbData;
				while (nDataLen > 0 && *pbData != '\0')
				{
					pbData++;
					nDataLen--;
				}
				if (pbData > (PBYTE)pszStart)
				{
					CString str = CString (pszStart, ((LPCSTR)pbData) - pszStart);
					m_pszSecurityPackages = strdup (str);
					pbData++;
					nDataLen--;
				}
				break;
			}
		}
	}
}

// Write settings to a data block. See documentation of format, above.

BOOL
CChatServer::WriteToData(
PVOID * ppvDataOut, 
UINT * pnDataLen)
{
	// Allocate a safe size buffer.

	PBYTE pbData = (PBYTE)malloc (512 + ((m_pszSecurityPackages != NULL) ? lstrlen (m_pszSecurityPackages) : 0));
	if (pbData == NULL)
	{
		return FALSE;
	}

	PBYTE pbWrite = pbData;
	if (m_nPort != 6667)
	{
		pbWrite[0] = (BYTE)datatypePort;
		*((PWORD)(&pbWrite[1])) = (WORD)m_nPort;
		pbWrite += 3;
	}
	if (m_nAuthenticationType != 0)
	{
		pbWrite[0] = (BYTE)datatypeAuthenticationType;
		pbWrite[1] = (BYTE)m_nAuthenticationType;
		pbWrite += 2;
	}
	if (m_nAuthenticationType == 1)
	{
		if (m_pszUserName != NULL && *m_pszUserName != '\0')
		{
			pbWrite[0] = (BYTE)datatypeUserName;
			pbWrite[1] = 0; // No encryption for now.
			lstrcpy ((LPSTR)(pbWrite + 2), m_pszUserName);
			pbWrite += 3 + lstrlen (m_pszUserName);
		}
		if (m_bRememberPassword && m_pszPassword != NULL && *m_pszPassword != '\0')
		{
			BYTE bySrc[32], byDest[68];
			lstrcpy ((LPSTR)bySrc, m_pszPassword);
			encEncodeData (byDest, bySrc, sizeof(bySrc));
			pbWrite[0] = (BYTE)datatypeUserPassword;
			pbWrite[1] = 1;
			memcpy (pbWrite + 2, byDest, sizeof(byDest));
			pbWrite += 2 + sizeof(byDest);
		}
		if (m_bRememberPassword)
		{
			pbWrite[0] = (BYTE)datatypeRememberPassword;
			pbWrite[1] = 1;
			pbWrite += 2;
		}
	}
	else if (m_nAuthenticationType == 3)
	{
		if (m_pszSecurityPackages != NULL && *m_pszSecurityPackages != '\0')
		{
			pbWrite[0] = (BYTE)datatypeSecurityPkg;
			lstrcpy ((LPSTR)(pbWrite + 1), m_pszSecurityPackages);
			pbWrite += 2 + lstrlen (m_pszSecurityPackages);
		}
	}

	*pnDataLen = pbWrite - pbData;
	if (*pnDataLen == 0)
	{
		free (pbData);
		*ppvDataOut = NULL;
	}
	else
	{
		*ppvDataOut = realloc (pbData, *pnDataLen);
	}
	return TRUE;
}

// Write settings to a registry entry. Wrapper around WriteToData.

BOOL
CChatServer::WriteToRegistry(
HKEY hkeyReg)
{
	PBYTE pbData;
	UINT nDataLen;
	BOOL bRet = WriteToData ((PVOID *)&pbData, &nDataLen);
	if (bRet)
	{
		if (nDataLen == 0)
			pbData = (PBYTE)"";
		bRet = RegSetValueEx (hkeyReg, m_pszName, 0, REG_BINARY, pbData, nDataLen) == ERROR_SUCCESS; 
		if (nDataLen != 0)
			free (pbData);
	}
	return bRet;
}


// Sets server settings to their defaults. In a function because it is used twice.

void 
CChatServer::SetDefaultSettings()
{
	m_pszUserName 		  = NULL;
	m_pszPassword 		  = NULL;
	m_pszSecurityPackages = NULL;
	m_nPort 			  = 6667;
	m_nAuthenticationType = authtypeNone;
	m_bRememberPassword   = FALSE;
}

// Sets server settings to their defaults. In a function because it is used twice.
// If you call this anywhere before the destructor, call SetDefaultSettings
// immediately, or there will be trouble.

void 
CChatServer::FreeSettings()
{
	free (m_pszUserName);
	free (m_pszPassword);
	free (m_pszSecurityPackages);
}

// Resolve the socket address of a server. Returns whether a socket was available.

BOOL
CChatServer::ResolveSocketAddress()
{
	if (m_sockaddr.sin_family == 0)
	{
		CString strServer;
		int nPort;	// Not used.
		TranslateServerNameToServerAndPort(m_pszName, &strServer, &nPort);

		ULONG ulAddr = WinSockInstance.inet_addr (strServer);
		if (ulAddr != INADDR_NONE)
		{
			m_sockaddr.sin_family = AF_INET;
			m_sockaddr.sin_addr.s_addr = ulAddr;
			m_sockaddr.sin_port = WinSockInstance.htons ((u_short)m_nPort);
		}
		else
		{
			LPHOSTENT lphostent;
			lphostent = WinSockInstance.gethostbyname (strServer);
			if (lphostent != NULL)
			{
				m_sockaddr.sin_family = AF_INET;
				m_sockaddr.sin_addr.s_addr = ((LPIN_ADDR)lphostent->h_addr)->s_addr;
				m_sockaddr.sin_port = WinSockInstance.htons ((u_short)m_nPort);
			}
			else
			{
				m_sockaddr.sin_family = -1;
			}
		}
	}
	return m_sockaddr.sin_family == AF_INET;
}

// =================================================================================
// CChatServerGroup implementation

CChatServerGroup::CChatServerGroup(
LPCSTR pszName)
{
	m_pszName = strdup (pszName);
	m_pszLastServer = NULL;
	m_bIsRead = FALSE;	// Don't read from registry until required to do so.
}

CChatServerGroup::~CChatServerGroup()
{
	free (m_pszName);
	free (m_pszLastServer);
}

// Lightweight call to check if the group contains a given server. This function
// does not read in all server entries from the registry, like FindServer does.

BOOL
CChatServerGroup::ContainsServer(
LPCSTR pszServer)
{
	if (m_bIsRead)
	{
		return FindServer (pszServer) != NULL;		// Can use heavyweight version
	}
	else
	{
		HKEY hkeyGroup = CChatServiceList::GetRegistryKey(CHATSVC_HKEY_SRVGROUP, m_pszName);
		if (!hkeyGroup)
		{
			return FALSE;
		}
		DWORD dwData;
		BOOL bFound = RegQueryValueEx (hkeyGroup, pszServer, NULL, NULL, NULL, &dwData) == ERROR_SUCCESS;
		CChatServiceList::ReleaseRegistryKey (hkeyGroup);
		return bFound;
	}
}

// Heavyweight call to find a server. Calls ReadFromRegistry if the entries are not read.

CChatServer*
CChatServerGroup::FindServer(
LPCSTR pszServer)
{
	if (!m_bIsRead && !ReadFromRegistry ())
	{
		return NULL;
	}

	CChatServer* pServer;
	for (pServer = m_listServers.GetHead (); pServer != NULL; pServer = m_listServers.GetNext (pServer))
	{
		if (!lstrcmp (pServer->m_pszName, pszServer))
		{
			break;
		}
	}
	return pServer;
}

// Reads all server entries from registry, on demand.

BOOL
CChatServerGroup::ReadFromRegistry()
{
	char szBuff[256], szBuff2[512];
	DWORD cbData, cbData2;

	if (m_bIsRead)
		return TRUE;

	BOOL bRet = TRUE;
	HKEY hkeyGroup = NULL;

	TRY
	{
		HKEY hkeyGroup = CChatServiceList::GetRegistryKey(CHATSVC_HKEY_SRVGROUP, m_pszName);
		if (!hkeyGroup)
		{
			::AfxThrowUserException ();
		}
	
		DWORD dwIndex = 0;
		for (dwIndex = 0, cbData = sizeof(szBuff), cbData2 = sizeof(szBuff2);
			 RegEnumValue (hkeyGroup, dwIndex, szBuff, &cbData, NULL, NULL, (PBYTE)szBuff2, &cbData2) == ERROR_SUCCESS;
			 dwIndex++, cbData = sizeof(szBuff), cbData2 = sizeof(szBuff2))
		{
			// Check first character. All reserved entries will have a . in this character.
			if (cbData == 0)
				continue;
			if (szBuff[0] == '.')
			{
				if (!lstrcmpi (szBuff, ".LastServer"))
				{
					m_pszLastServer = strdup (szBuff2);
				}
			}
			else
			{
				CChatServer * pServer = new CChatServer(szBuff, szBuff2, cbData2);
				m_listServers.AddHead (pServer);
			}
		} 
	}
	CATCH_ALL(e)
	{
		bRet = FALSE;
		return FALSE;
	}
	END_CATCH_ALL

	CChatServiceList::ReleaseRegistryKey(hkeyGroup);
	m_bIsRead = TRUE;
	return bRet;
}

// Create a server internally and in the registry.

CChatServer* 
CChatServerGroup::CreateServer(
LPCSTR pszName,
int	   nPort)
{
	if (!m_bIsRead && !ReadFromRegistry ())
	{
		::AfxThrowUserException ();
	}

	// This is for debugging only - keep this line out of release builds
	// to increase performance.
	ASSERT(FindServer (pszName) == NULL);

	CChatServer* pServer;
	pServer = new CChatServer(pszName);
	pServer->m_nPort = nPort;

	HKEY hkeyGroup = CChatServiceList::GetRegistryKey (CHATSVC_HKEY_SRVGROUP, m_pszName);
	if (!hkeyGroup)
	{
		delete pServer;
		::AfxThrowUserException ();
	}

	pServer->WriteToRegistry (hkeyGroup);

	CChatServiceList::ReleaseRegistryKey (hkeyGroup);
	m_listServers.AddTail (pServer);
	return pServer;
}

// Destroys a server.

BOOL
CChatServerGroup::DestroyServer(
CChatServer* pServer)
{
	LPCSTR pszGroupNameCompare = lstrcmpi (UNASSOCIATED_GROUP, m_pszName) ? m_pszName : NULL;

	HKEY hkeyGroup = CChatServiceList::GetRegistryKey (CHATSVC_HKEY_SRVGROUP, m_pszName);
	if (hkeyGroup)
	{
		m_listServers.Remove (pServer);
		RegDeleteValue (hkeyGroup, pServer->m_pszName);
		delete pServer;
	}
	return hkeyGroup != NULL;
}

// Enumerates server entries.

BOOL 
CChatServerGroup::EnumServers(
CChatServer* &pServer)
{
	if (pServer == NULL)
	{
		if (!m_bIsRead && !ReadFromRegistry ())
		{
			return FALSE;
		}
		pServer = m_listServers.GetHead ();
	}
	else
	{
		ASSERT(m_bIsRead);
		pServer = m_listServers.GetNext (pServer);
	}
	return pServer != NULL;
}

// Sets the last server accessed.

void 
CChatServerGroup::SetLastAccessedServer(
CChatServer* pServer)
{
	HKEY hkeyGroup = CChatServiceList::GetRegistryKey (CHATSVC_HKEY_SRVGROUP, m_pszName);
	if (hkeyGroup)
	{
		RegSetValueEx (hkeyGroup, ".LastServer", 0, 
			REG_SZ, (PBYTE)pServer->m_pszName, lstrlen (pServer->m_pszName) + 1);
		free (m_pszLastServer);
		m_pszLastServer = strdup (pServer->m_pszName);
	}
}

// Checks if the server group is empty. Alas, this could have been an inline 
// function were it not for the fact that server groups are loaded on demand.

BOOL 
CChatServerGroup::IsEmpty()
{
	return (!m_bIsRead && !ReadFromRegistry ()) || m_listServers.IsEmpty ();
}

// Returns the number of entries. Again, load server groups on demand.

int
CChatServerGroup::GetServerCount()
{
	if (!m_bIsRead)
		ReadFromRegistry ();
	return m_listServers.GetCount ();
}

// =================================================================================
// CChatService implementation

// Constructor that takes a service name, as kept in the registry.
// A service name can be one of the three formats:
//		//network/server
//		//network
//		server

CChatService::CChatService(
LPCSTR pszService)
{
	if (pszService[0] == '/' && pszService[1] == '/')
	{
		pszService += 2;
		LPCSTR pszSlash = OurMbsChr (pszService, '/');
		if (pszSlash != NULL)
		{
			CommonConstruct (CString (pszService, pszSlash - pszService), pszSlash + 1);
		}
		else
		{
			CommonConstruct (pszService, NULL);
		}
	}
	else
	{
		CommonConstruct (NULL, pszService);
	}
}

// Destructor

CChatService::~CChatService()
{
	free (m_pszGroup);
	free (m_pszServer);
}

// Common code for constructor.

void
CChatService::CommonConstruct(
LPCSTR pszGroup,
LPCSTR pszServer)
{
	m_pszGroup  = (pszGroup != NULL && *pszGroup != '\0' && lstrcmpi (pszGroup, UNASSOCIATED_GROUP)) 
		? strdup (pszGroup) : NULL;
	m_pszServer = (pszServer != NULL && *pszServer != '\0') ? strdup (pszServer) : NULL;
}

// Formats the group and server name as a service name that can be stored in the
// registry. See the constructor above for details on the service name.

void 
CChatService::FormatAsServiceName(
CString &strOut)
{
	LPCSTR pszFormat;
	LPCSTR pszGroup = m_pszGroup;
	LPCSTR pszServer = m_pszServer;
	if (pszGroup != NULL)
	{
		pszFormat = pszServer != NULL ? "//%s/%s" : "//%s";
	}
	else if (pszServer != NULL)
	{
		pszFormat = "%s%s";
		pszGroup = "";
	}
	else
	{
		pszFormat = "";
	}
	strOut.Format (pszFormat, pszGroup, pszServer);
}

// =================================================================================
// CChatServiceList implementation

LPCSTR CChatServiceList::sm_pszRegKeyName = szRootRegKeyName;
HKEY CChatServiceList::sm_hkeyCachedMain = NULL;

// Reads the list of available services from the registry. Includes support for
// backward compatibility.

BOOL 
CChatServiceList::ReadFromRegistry()
{
	HKEY hkey = NULL;
	HKEY hkeySvc = NULL;
	HKEY hkeyMain = NULL;
	char szBuff[(MAX_FORMATTINGPERBYTE+1)*MAX_INPUTLEN];
	DWORD cbData;
	CString strOldSettings;
	BOOL bReturn = TRUE;

	TRY
	{
		// Open the base registry key.
		if ((hkey = GetRegistryKey (CHATSVC_HKEY_PARENT)) == NULL)
		{
			::AfxThrowUserException ();
		}

		// First, check for old registry settings. If they exist, and have not
		// been migrated, read them in, and mark them as having been migrated.

		BOOL bServersMigrated;
		cbData = sizeof(bServersMigrated);
		if (RegQueryValueEx (hkey, "ServersMigrated", 0, 
				NULL, (LPBYTE)&bServersMigrated, &cbData) != ERROR_SUCCESS)
		{
			cbData = sizeof(szBuff);
			if (RegQueryValueEx (hkey, "ServerList", 0, NULL, (PBYTE)szBuff, &cbData) == ERROR_SUCCESS)
			{
				strOldSettings = szBuff;
			}
			else
			{
				strOldSettings.LoadString (IDS_DEFAULT_SERVERLIST);
			}

		}

		// Open subkey that contains the service list

		if ((hkeySvc = GetRegistryKey (CHATSVC_HKEY_SVCLIST)) == NULL)
		{
			::AfxThrowUserException ();
		}

		// Read in each entry, until we hit the last one.

		char szValueName[12];
		int i = 0;
		while (TRUE)
		{
			wsprintf (szValueName, "%d", i);

			cbData = sizeof(szBuff);
			if (RegQueryValueEx (hkeySvc, szValueName, 0, NULL, (PBYTE)szBuff, &cbData) 
					!= ERROR_SUCCESS || cbData == 0)
			{
				break;
			}

			CChatService* pSvc = new CChatService (szBuff);
			m_listServices.AddTail (pSvc);
			i++;
		}
		m_nOldReadCount = i;

		// Open the "Servers" main key. This key will already be cached, so there's
		// no need to do excessive testing of the return value.

		hkeyMain = GetRegistryKey (CHATSVC_HKEY_ROOT);
		ASSERT(hkeyMain != NULL);

		// Enumerate subkeys. Subkeys that don't start with '.' are added to
		// the list.

		DWORD dwIndex = 0;
		*szBuff = '\0';
		while (RegEnumKey (hkeyMain, dwIndex++, szBuff, sizeof(szBuff)) == ERROR_SUCCESS)
		{
			if (*szBuff != '\0' && *szBuff != '.')
			{
				CChatServerGroup * pGroup = new CChatServerGroup (szBuff);
				m_listSrvGroups.AddTail (pGroup);
			}
			*szBuff = '\0';
		}

		// Now go through all the servers in the old settings. Any that are not
		// in the new list will be added to the bottom of the new list.
		// bForPath will enumerate the entries, and call AddOldServer, which
		// will do the adding.

		if (!strOldSettings.IsEmpty ())
		{
			bForPath (strOldSettings, AddOldServer, this);
			WriteIfChanged ();
		}
	}
	CATCH_ALL(e)
	{
		bReturn = FALSE;
	}
	END_CATCH_ALL

	// Also may have to load in prepopulated servers list.

	char szPsFileName[256];
	cbData = sizeof(szPsFileName);
	lstrcpy (szPsFileName, "servers.cfg");
	RegQueryValueEx (hkey, "PrepopulatedServers", 0, NULL, (PBYTE)szPsFileName, &cbData);
	if (*szPsFileName != '\0' && lstrcmpi (szPsFileName, "none"))
	{
		CString strFile;
		strFile.Format ("%s\\%s", (LPCSTR)theApp.m_strBaseDir, szPsFileName);
		if (!ImportFromFile (strFile))
		{
			::AfxThrowUserException ();
		}
		lstrcpy (szPsFileName, "none");
		RegSetValueEx (hkey, "PrepopulatedServers", 0, REG_SZ, 
			(PBYTE)&szPsFileName, lstrlen (szPsFileName) + 1);
	}

	ReleaseRegistryKey (hkey);
	ReleaseRegistryKey (hkeySvc);
	ReleaseRegistryKey (hkeyMain);

	m_bSvcListModified = FALSE;
	return bReturn;
}

// Writes the list of available services to the registry. Only the service list values
// are written - server group/server entries are maintained "on the fly", and don't
// need to be written here.

BOOL
CChatServiceList::WriteToRegistry()
{
	HKEY hkeySvc;
	if ((hkeySvc = GetRegistryKey (CHATSVC_HKEY_SVCLIST)) == NULL)
	{
		return FALSE;
	}

	int i;
	CChatService* pSvc;
	char szValueName[12];
	CString str;
	for (i = 0, pSvc = m_listServices.GetHead (); pSvc != NULL; pSvc = m_listServices.GetNext (pSvc), i++)
	{
		wsprintf (szValueName, "%d", i);
		pSvc->FormatAsServiceName (str);
		RegSetValueEx (hkeySvc, szValueName, 0, REG_SZ, (const BYTE *)(LPCSTR)str, str.GetLength () + 1);
	}

	// If the number of entries we are writing out is less than how many there were
	// before, delete the rest.

	int nOldReadCount = m_nOldReadCount;
	m_nOldReadCount = i;

	while (i < nOldReadCount)
	{
		wsprintf (szValueName, "%d", i);
		RegDeleteValue (hkeySvc, szValueName);
		i++;
	}

	ReleaseRegistryKey (hkeySvc);
	m_bSvcListModified = FALSE;
	return TRUE;
}

// Writes to registry if modified.

BOOL
CChatServiceList::WriteIfChanged()
{
	if (m_bSvcListModified)
		return WriteToRegistry ();
	else
		return FALSE;
}

// Returns a registry key, either for the "Servers" main directory, a server group's
// subkey, or the service list subkey. The main directory's key is kept cached, 
// but subkeys are not cached. The caller should call ReleaseRegistryKey to release
// the key - this ensures uniform implementation-independent behaviour on the caller's
// side, regardless of caching implemented in here.
// ADDITION: Also put in support for parent key (Comic Chat main registry key) with
// CHATSVC_HKEY_PARENT.

HKEY 
CChatServiceList::GetRegistryKey(
DWORD dwRegKeyType, 
LPCSTR pszSection)
{
	char szSection[256];
	HKEY hkeyParent;
	HKEY * phkeyCache;
	HKEY hkeyOut;

	switch (dwRegKeyType)
	{
		case CHATSVC_HKEY_ROOT:
			if (sm_hkeyCachedMain != NULL)
			{
				return sm_hkeyCachedMain;
			}
			hkeyParent = HKEY_CURRENT_USER;
			wsprintf (szSection, "%s\\%s", sm_pszRegKeyName, g_szServicesRegName);
			pszSection = szSection;
			phkeyCache = &sm_hkeyCachedMain;
			break;
		case CHATSVC_HKEY_SVCLIST:
			pszSection = g_szServicesList;
			// Fallthru
		case CHATSVC_HKEY_SRVGROUP:
			hkeyParent = GetRegistryKey (); // Get the root key.
			phkeyCache = &hkeyOut; // Cause a dead store
			break;
		case CHATSVC_HKEY_PARENT:
			hkeyParent = HKEY_CURRENT_USER;
			pszSection = sm_pszRegKeyName;
			phkeyCache = &hkeyOut; // Cause a dead store
			break;
		default:
			ASSERT(FALSE);
			return NULL;
	}

	if (hkeyParent != NULL)
	{
		DWORD dwDisposition;
		if (RegCreateKeyEx (hkeyParent, pszSection, 0, NULL, 
				REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL,
				&hkeyOut, &dwDisposition) != ERROR_SUCCESS)
			hkeyOut = NULL;
	}
	else
		hkeyOut = NULL;

	*phkeyCache = hkeyOut;
	return hkeyOut;
}

// Release the registry key. A cached key is not freed, an uncached one is.
// NULL keys are also allowed, to simplify caller's code.

void 
CChatServiceList::ReleaseRegistryKey(
HKEY hkey)
{
	if (hkey != NULL && hkey != sm_hkeyCachedMain)
	{
		RegCloseKey (hkey);
	}
}													   

// Returns the default group for a server. This function performs mappings from
// server names to groups. Return value specifies whether the server is already
// in this group or not.

BOOL
CChatServiceList::GetGroupForOldServer(
LPCSTR 	 pszServer, 
CString& strGroupOut)
{
	static LPSTR pszPrefixes[] = { NULL, "chat", NULL };
	static LPSTR pszSuffixes[] = { "microsoft.com", "msn.com", "msn.com" };
	static UINT nGroups[] = { IDS_PREDEFGROUP_MS, IDS_PREDEFGROUP_MSN, IDS_PREDEFGROUP_MS };

	LPCSTR pszColon = OurMbsChr (pszServer, ':');
	CString strServer (pszServer, pszColon != NULL ? pszColon - pszServer : lstrlen (pszServer));

	// First check if the server is already in a group. If so, just return that
	// group.

	CChatServerGroup* pGroup;
	for (pGroup = m_listSrvGroups.GetHead (); pGroup != NULL; pGroup = m_listSrvGroups.GetNext (pGroup))
	{
		if (pGroup->ContainsServer (pszServer))
		{
			strGroupOut = pGroup->m_pszName;
			return TRUE;
		}
		else if (pszColon != NULL && pGroup->ContainsServer (strServer))
		{
			strGroupOut = pGroup->m_pszName;
			return FALSE;
		}
	}

	// Determine the default group by matching server name prefix and suffix.

	int nLen = strServer.GetLength ();
	int nOffset;
	for (int i = 0; i < _countof(pszPrefixes); i++)
	{
		// This is really one big if statement, broken up for clarity.
		// The loop breaks out if the server name starts with the prefix and ends
		// with the suffix given below.

		if (pszPrefixes[i] != NULL)
		{
			if (strncmp (pszPrefixes[i], strServer, lstrlen (pszPrefixes[i])))
				continue;
		}

		if (pszSuffixes[i] != NULL)
		{
	   		nOffset = nLen - lstrlen (pszSuffixes[i]);
			if (nOffset < 0 || strcmp (pszSuffixes[i], ((LPCSTR)strServer) + nOffset))
				continue;
		}

		break;
	}

	if (i < _countof(pszPrefixes))
	{
		 strGroupOut.LoadString (nGroups[i]);
	}
	else
	{
		strGroupOut = UNASSOCIATED_GROUP;
	}
	return FALSE;
}

// Translates a server name (either "server" or "server:port" to a server and port)

void 
TranslateServerNameToServerAndPort(
LPCSTR pszServer, 
CString * pstrServer, 
int * pnPort)
{
	LPCSTR pszColon = OurMbsChr (pszServer, ':');
	if (pszColon != NULL)
	{
		*pstrServer = CString (pszServer, pszColon - pszServer);
		*pnPort = atoi (pszColon + 1);
		if (*pnPort < 6000 || *pnPort > 7000)
			*pnPort = 6667;
	}
	else
	{
		*pstrServer = pszServer;
		*pnPort = 6667;
	}
}

// Add a server from old settings. The server is added unless it is already in a group.

BOOL 
CChatServiceList::AddOldServer(
LPCSTR pszServer)
{
	ASSERT (pszServer != NULL);
	if (*pszServer == '\0')
		return FALSE;

	// From server name, extract server and port.

	CString strServer;
	int nPort;
	TranslateServerNameToServerAndPort (pszServer, &strServer, &nPort);

	// Check through list of services to see if there is already an entry with the
	// given server name.

	CChatService * pSvc;
	for (pSvc = m_listServices.GetHead (); pSvc != NULL; pSvc = m_listServices.GetNext (pSvc))
	{
		if (pSvc->GetServer () != NULL && !lstrcmpi (pSvc->GetServer (), pszServer))
		{
			break;
		}
	}

	if (pSvc != NULL)
		return FALSE;		// No need to add this entry.

	CString strGroupToAddTo;
	BOOL bAlreadyInGroup = GetGroupForOldServer (pszServer, strGroupToAddTo);

	if (!bAlreadyInGroup)
	{
		// Create the group if not already there.
		CChatServerGroup* pGroup = CreateGroup (strGroupToAddTo);

		CChatServer* pServer = pGroup->CreateServer (pszServer, nPort);
	}

	// If we are not adding to a real group, also add the server to the service list.

	if (!lstrcmp (strGroupToAddTo, UNASSOCIATED_GROUP))
	{
		pSvc = new CChatService (strGroupToAddTo, pszServer);
		m_listServices.AddTail (pSvc);
		m_bSvcListModified = TRUE;
	}

	return FALSE;
}

// Removes all references to a given group/server combination. The group name can 
// be NULL, in which case it refers to the unassociated group. The server name can
// be NULL, in which case all servers in the group are removed.

void 
CChatServiceList::RemoveReferences(
LPCSTR pszGroup, 
LPCSTR pszServer)
{
	if (pszGroup != NULL && !lstrcmpi (pszGroup, UNASSOCIATED_GROUP))
	{
		pszGroup = NULL;
	}

	// Destroy all service entries that refer to this server.

	CChatService * pSvc, * pNextSvc;
	for (pSvc = m_listServices.GetHead (); pSvc != NULL; pSvc = pNextSvc)
	{
		pNextSvc = m_listServices.GetNext (pSvc);
		if (((pSvc->GetGroup () == pszGroup) || 
					(pszGroup != NULL && !lstrcmpi (pSvc->GetGroup (), pszGroup))) &&
			(pszServer == NULL || !lstrcmpi (pSvc->GetServer (), pszServer)))
		{
			m_listServices.Remove (pSvc);
			m_bSvcListModified = TRUE;
			delete pSvc;
		}
	}

}

// Static callback to add a server from old settings. Just calls the non-static version.

BOOL 
CChatServiceList::AddOldServer(
LPCSTR pszServer, 
PVOID pvData)
{
	CChatServiceList* pSvcList = (CChatServiceList*)pvData;
	return pSvcList->AddOldServer (pszServer);
}

// Gets a service name from a display name.

void
CChatServiceList::GetServiceNameFromDisplayName(
LPCSTR pszDisplayName, 
CString& strService)
{
	// Is it really a display name?

	if (pszDisplayName[0] == '/')
	{
		strService = pszDisplayName;
		return;
	}

	CChatService* pService = NULL;
	while (EnumServices (pService))
	{
		if (!lstrcmpi (pService->GetDisplayName (), pszDisplayName))
		{
			pService->FormatAsServiceName (strService);
			return;
		}
	}

	CChatServerGroup * pGroup = FindGroup (pszDisplayName);
	if (pGroup != NULL)
	{
		strService.Format ("//%s", pszDisplayName);
		return;
	}

	CString strGroup;
	GetGroupForOldServer (pszDisplayName, strGroup);
	if (!lstrcmp (strGroup, UNASSOCIATED_GROUP))
	{
		strService = pszDisplayName;
	}
	else
	{
		strService.Format ("//%s/%s", strGroup, pszDisplayName);
	}
	return;
}

// Create a server group entry internally and in the registry, if one doesn't already
// exist.

CChatServerGroup* 
CChatServiceList::CreateGroup(
LPCSTR pszName)
{
	CChatServerGroup* pGroup;
	CChatService* pSvc;
	BOOL bIsUnassociated = !lstrcmpi (pszName, UNASSOCIATED_GROUP);

	if ((pGroup = FindGroup (pszName)) != NULL)
	{
		return pGroup;
	}

	pGroup = new CChatServerGroup (pszName);
	if (!bIsUnassociated)
		pSvc = new CChatService (pszName, NULL);
	
	// This will create the key.
	HKEY hkey = GetRegistryKey (CHATSVC_HKEY_SRVGROUP, pszName);
	ReleaseRegistryKey (hkey);
	if (!hkey)
	{
		delete pGroup;
		if (!bIsUnassociated)
			delete pSvc;
		::AfxThrowUserException ();
	}

	m_listSrvGroups.AddTail (pGroup);
	if (!bIsUnassociated)
	{
		m_listServices.AddTail (pSvc);
		m_bSvcListModified = TRUE;
	}
	return pGroup;
}

// Creates a service entry, adding it to the head.

CChatService* 
CChatServiceList::CreateService(
LPCSTR pszGroup, 
LPCSTR pszServer)
{
	CChatService * pSvc = new CChatService (pszGroup, pszServer);
	m_listServices.AddHead (pSvc);
	m_bSvcListModified = TRUE;
	return pSvc;
}

// Finds a service by its group/server name

CChatService* 
CChatServiceList::FindService(
LPCSTR pszGroup,
LPCSTR pszServer)
{
	CChatService * pSvc;
	LPCSTR pszServerCompare, pszGroupCompare;
	for (pSvc = m_listServices.GetHead (); pSvc != NULL; pSvc = m_listServices.GetNext (pSvc))
	{
		pszGroupCompare = pSvc->GetGroup ();
		pszServerCompare = pSvc->GetServer ();
		if ((pszGroup == (LPCSTR)-1L || pszGroupCompare == pszGroup || !lstrcmpi (pszGroupCompare, pszGroup)) &&
				(pszServer == (LPCSTR)-1L || pszServerCompare == pszServer || !lstrcmpi (pszServerCompare, pszServer)))
		{
			break;
		}
	}
	return pSvc;
}

// Moves a service to the top of the list.

void
CChatServiceList::MoveServiceToTop(
CChatService* pSvc)
{
	m_listServices.Remove (pSvc);
	m_listServices.AddHead (pSvc);
	m_bSvcListModified = TRUE;
}

// Destroys a server group.

BOOL 
CChatServiceList::DestroyGroup(
CChatServerGroup* pGroup)
{
	HKEY hkeyMain = CChatServiceList::GetRegistryKey (CHATSVC_HKEY_ROOT);
	if (!hkeyMain)
		return FALSE;

	RegDeleteKey (hkeyMain, pGroup->m_pszName);
	m_listSrvGroups.Remove (pGroup);
	delete pGroup;
	CChatServiceList::ReleaseRegistryKey (hkeyMain);
	return TRUE;
}

// Finds a server group entry of a given name

CChatServerGroup*
CChatServiceList::FindGroup(
LPCSTR pszName)
{
	CChatServerGroup * pGroup;
	for (pGroup = m_listSrvGroups.GetHead (); pGroup != NULL; pGroup = m_listSrvGroups.GetNext (pGroup))
	{
		if (!lstrcmpi (pGroup->m_pszName, pszName))
		{
			break;
		}
	}
	return pGroup;
}

// Enumerates server group entries. Call this with a pGroup set to NULL the first time, and
// stop looping when EnumGroups returns FALSE. The bUnassociatedGroup variable is set to
// TRUE when the group being returned is the special "Unassociated" group. Callers can
// use this to substitute their own name for the group (i.e. something locale-dependent).

BOOL 
CChatServiceList::EnumGroups(
CChatServerGroup* &pGroup, 
BOOL &bUnassociatedGroup)
{
	pGroup = pGroup == NULL ? m_listSrvGroups.GetHead () : m_listSrvGroups.GetNext (pGroup);
	if (pGroup != NULL)
	{
		bUnassociatedGroup = !lstrcmpi (pGroup->m_pszName, UNASSOCIATED_GROUP);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Enumerate service entries, in the order they appear in the list.

BOOL
CChatServiceList::EnumServices(
CChatService* &pSvc)
{
	pSvc = pSvc == NULL ? m_listServices.GetHead () : m_listServices.GetNext (pSvc);
	return pSvc != NULL;
}

// Import a list from a file, creating any entries that don't already exist.

BOOL 
CChatServiceList::ImportFromFile(
LPCSTR pszFile)
{
	if (GetFileAttributes (pszFile) == (DWORD)-1L)
		return TRUE;

	BOOL bRet = TRUE;
	LPSTR pszGroups;
	LPSTR pszServers;
	LPSTR pszCurGroup;
	LPSTR pszCurServer;
	CChatServerGroup* pGroup;
	CChatServer* pServer;

	TRY
	{
		pszGroups = (LPSTR)malloc (4096); 
		pszServers = (LPSTR)malloc (16384);
		if (pszGroups == NULL || pszServers == NULL)
			::AfxThrowUserException ();
	
		GetPrivateProfileSectionNames (pszGroups, 4096, pszFile);
	
		for (pszCurGroup = pszGroups; *pszCurGroup != '\0'; pszCurGroup += lstrlen (pszCurGroup) + 1)
		{
			if (!lstrcmpi (pszCurGroup, UNASSOCIATED_GROUP) ||
					pszCurGroup[0] == '.' || 
					pszCurGroup[0] == '{' || 
                    OurMbsPbrk (pszCurGroup, "/\\") != NULL ||
					lstrlen (pszCurGroup) > 100)
				continue;
	
			if ((pGroup = FindGroup (pszCurGroup)) == NULL)
				pGroup = CreateGroup (pszCurGroup);
			ASSERT(pGroup != NULL);
	
			GetPrivateProfileString (pszCurGroup, NULL, "", pszServers, 16384, pszFile);
			for (pszCurServer = pszServers; *pszCurServer;  pszCurServer += lstrlen (pszCurServer) + 1)
			{
				if (pszCurServer[0] == '.' || OurMbsPbrk (pszCurServer, "/\\") != NULL)
					continue;
				if (pGroup->FindServer (pszCurServer) == NULL)
				{
					CString strServer;
					int nPort;
					TranslateServerNameToServerAndPort(pszCurServer, &strServer, &nPort);
					pGroup->CreateServer (strServer, nPort);
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		bRet = FALSE;
	}
	END_CATCH_ALL

Cleanup:

	free (pszGroups);
	free (pszServers);
	return bRet;

}

// =================================================================================
// CChatServiceConnector implementation

CChatServiceConnector::CChatServiceConnector()
{
	m_pSvcList 			= NULL;
	m_pConnections	 	= NULL;
	m_pSockets 			= NULL;
	m_pConnectingServer	= NULL;
	m_pConnectingGroup 	= NULL;
	m_hThread 		    = NULL;
	m_pCurThreadData    = NULL;
	InitializeCriticalSection (&m_critsec);
}

CChatServiceConnector::~CChatServiceConnector()
{
	Cleanup ();
	POSITION pos = m_mapDeleteableSockets.GetStartPosition ();
	Socket* pSocket;
	WORD w;
	while (pos)
	{
		m_mapDeleteableSockets.GetNextAssoc (pos, (PVOID &)pSocket, w);
		delete pSocket;
	}
}

BOOL
CChatServiceConnector::BeginConnectToService(
LPCSTR pszSvc)
{
	// Avoid re-entry.

	if (m_pSvcList == NULL || m_pSockets != NULL)
	{
		return FALSE;
	}

	int i;

	// May be a retry of the same service. This is caused when we connect to
	// a server, but then cannot finish logging in with it. In this case,
	// proceed with the old server list.

	if (pszSvc == NULL)
	{
		ASSERT (m_pConnections != NULL);
		EnterCriticalSection (&m_critsec);
		for (i = 0; i < m_nServers; i++)
		{
			if (m_pConnections[i].byStatus == srvconnNotAttempted || 
					m_pConnections[i].byStatus == srvconnUnresolved)
				break;
		}
		LeaveCriticalSection (&m_critsec);
		if (i == m_nServers)
		{
			// No servers left to try!
			return FALSE;
		}
	}
	else
	{
		Cleanup ();	// Make sure old server list is gone.

		m_strSvc = pszSvc;
		CChatService svcTemp (pszSvc);
	
		// Build the array of servers to which we should try to connect.
	
		CChatServerGroup* pGroup;
		CChatServer*      pServer;
		if (svcTemp.GetServer () != NULL)
		{
			// Connect to a specific server.
			LPCSTR pszGroup = svcTemp.GetGroup () != NULL ? svcTemp.GetGroup () : UNASSOCIATED_GROUP;
			pGroup = m_pSvcList->FindGroup (pszGroup);
			if (pGroup == NULL)
			{
				TRY
				{
					pGroup = m_pSvcList->CreateGroup (pszGroup);
				}
				CATCH_ALL(e)
				{
				}
				END_CATCH_ALL
				if (pGroup == NULL)
				{
					return FALSE;
				}
			}
			m_pConnectingGroup = pGroup;
			pServer = pGroup->FindServer (svcTemp.GetServer ());
			if (pServer == NULL)
			{
				CString strServer;
				int nPort;
				TranslateServerNameToServerAndPort(svcTemp.GetServer (), &strServer, &nPort);
				pServer = pGroup->CreateServer (svcTemp.GetServer (), nPort);
				if (pServer == NULL)
				{
					return FALSE;
				}
			}
			m_pConnections = (SrvConnection *)malloc (sizeof(*m_pConnections));
			if (m_pConnections == NULL)
				return FALSE;
			m_pConnections[0].pServer = pServer;
			m_pConnections[0].byStatus = srvconnUnresolved;
			m_nServers = 1;
		}
		else
		{
			pGroup = m_pSvcList->FindGroup (svcTemp.GetGroup ());
			m_pConnectingGroup = pGroup;
			if (pGroup == NULL)
				return FALSE;
			m_nServers = pGroup->GetServerCount ();
			if (m_nServers == 0)
				return FALSE;
			m_pConnections = (SrvConnection *)malloc (sizeof(*m_pConnections) * m_nServers);
			if (m_pConnections == NULL)
				return FALSE;
			ZeroMemory (m_pConnections, sizeof(*m_pConnections) * m_nServers);
	
			// Go through the list of servers, adding each to a random place in
			// the array. Make sure the last accessed server is placed in the first
			// slot.
			pServer = NULL;
			int iOrigSlot, iSlot;
			srand ((UINT)GetTickCount ());
			while (pGroup->EnumServers (pServer))
			{
				if (m_nServers == 1 /*|| (m_pConnections[0].pServer == NULL && !lstrcmpi (pServer->m_pszName, pGroup->m_pszLastServer))*/)
				{
					m_pConnections[0].pServer = pServer;
				}
				else
				{
					iOrigSlot = iSlot = rand () % (m_nServers - 1) + 1;
					while (m_pConnections[iSlot].pServer != NULL)
					{
						iSlot++;
						if (iSlot == m_nServers)
							iSlot = 1;
						if (iSlot == iOrigSlot) // Can only happen if out of slots and invalid "last server"
						{
							iSlot = 0;
							ASSERT (m_pConnections[0].pServer == NULL);
							break;
						}
					}
					m_pConnections[iSlot].pServer = pServer;
				}
			}
		}

		// Create the thread which will resolve DNS addresses.

		m_pCurThreadData = (ThreadData*)malloc (sizeof(*m_pCurThreadData));
		if (m_pCurThreadData == NULL)
		{
			Cleanup ();
			return FALSE;
		}
		m_pCurThreadData->pThis = this;
		m_pCurThreadData->bTerminate = FALSE;
		CWinThread* pThread = AfxBeginThread (ResolverThreadProc, m_pCurThreadData,
									THREAD_PRIORITY_NORMAL, 0, CREATE_SUSPENDED);
		if (pThread == NULL)
		{
			free (m_pCurThreadData);
			m_pCurThreadData = NULL;
			Cleanup ();
			return FALSE;
		}

		DuplicateHandle (GetCurrentProcess (), pThread->m_hThread,
   						 GetCurrentProcess (), &m_hThread,
						 0, FALSE, DUPLICATE_SAME_ACCESS);

		pThread->m_bAutoDelete = TRUE;
		ResumeThread (m_hThread);
	}

	// Create a place to hold a small number of socket objects, but no more than the 
	// number of server.

	m_nSockets = min (5, m_nServers);
	m_pSockets = (Socket * *)malloc (sizeof(Socket *) * m_nSockets);
	if (m_pSockets == NULL)
	{
		Cleanup ();
		return FALSE;
	}
	ZeroMemory (m_pSockets, sizeof(Socket *) * m_nSockets);

	// Create the sockets.

	TRY
	{
		for (i = 0; i < m_nSockets; i++)
		{
			m_pSockets[i] = new Socket (this, i);
			if (!m_pSockets[i]->Create ())
			{
				::AfxThrowUserException ();
			}
		}
	}
	CATCH_ALL(e)
	{
		Cleanup ();
		return FALSE;
	}
	END_CATCH_ALL

	m_nActiveSockets = 0;

	return TRUE;
}

// Called periodically to assign the socket to the next available server.
// A return value of 0 indicates failure, 1 indicates success, and -1 indicates
// that the socket can't be assigned right now, and should be retried later.

int
CChatServiceConnector::AssignSocket(
int nSocket)
{
	// Make sure that we are in mid-connection, and that the socket is
	// not already busy.

	if (m_pSockets == NULL || nSocket >= m_nSockets || m_pSockets[nSocket]->m_bConnecting)
		return 1; // Acts like a success, so that the caller can blissfully continue.

	if (!AssignSocketToNewServer (nSocket))
	{
		Cleanup ();
		return 0;
	}

	// It's possible that the socket wasn't actually assigned to a server, because
	// no servers are currently available. 

	return m_pSockets[nSocket]->m_bConnecting ? 1 : -1;
}

BOOL
CChatServiceConnector::AssignSocketToNewServer(
int nSocket)
{
	if (m_pConnections == NULL)
		return FALSE;

	// Find next available connection.

	int nCurServer;
	BOOL bAnyUnresolved = FALSE;
	EnterCriticalSection (&m_critsec);
	for (nCurServer = 0; nCurServer < m_nServers; nCurServer++)
	{
		if (m_pConnections[nCurServer].byStatus == srvconnNotAttempted)
		{
			break;
		}
		else if (m_pConnections[nCurServer].byStatus == srvconnUnresolved)
		{
			bAnyUnresolved = TRUE;
		}
	}
	LeaveCriticalSection (&m_critsec);

	if (nCurServer == m_nServers)
	{
		// No more servers. If there are no more active sockets left,
		// return a FALSE here so that an error message is shown.
		// Otherwise, just silently return TRUE, so it can be tried later.
		return m_nActiveSockets != 0 || bAnyUnresolved;
	}

	// Sanity check, should never happen, but let's be safe.
	if (m_pSockets == NULL || nSocket >= m_nSockets || m_pSockets[nSocket] == NULL)
	{
		ASSERT (FALSE);
		return FALSE;
	}

	if (!m_pSockets[nSocket]->Connect (nCurServer))
	{
		// Try the next server.
		EnterCriticalSection (&m_critsec);
		m_pConnections[nCurServer].byStatus = srvconnFailed;
		LeaveCriticalSection (&m_critsec);
		return AssignSocketToNewServer (nSocket);
	}
	EnterCriticalSection (&m_critsec);
	m_pConnections[nCurServer].byStatus = srvconnAttempting;
	LeaveCriticalSection (&m_critsec);
	m_nActiveSockets++;
	return TRUE;
}

void
CChatServiceConnector::Cleanup(
BOOL bCleanupServerList)
{
	if (m_pSockets != NULL)
	{
		int i;
		for (i = 0; i < m_nSockets; i++)
		{
			if (m_pSockets[i] != NULL)
			{
				// We can't delete the socket here because of rogue
				// OnConnect messages that can possibly come through.
				// So, we just tell the socket to go away when connected.
				if (m_pSockets[i]->m_hSocket != INVALID_SOCKET && m_pSockets[i]->m_bConnecting)
				{
					m_mapDeleteableSockets.SetAt (m_pSockets[i], 0);
					m_pSockets[i]->m_nID = -1;
				}
				else
				{
					delete m_pSockets[i];
				}
			}
		}
		free (m_pSockets);
		m_pSockets = NULL;
	}

	if (bCleanupServerList)
	{
		if (m_hThread != NULL)
		{
			// If the thread has exited by itself, it will have freed the
			// thread data passed to it, and set m_pCurThreadData to NULL.
			// If it is still running, we can set bTerminate in the thread
			// data to TRUE. Then, the thread will exit, free the thread
			// data, but NOT set m_pCurThreadData to NULL. This lets 
			// orphaned threads go away at their own leisure.
			EnterCriticalSection (&m_critsec);
			if (m_pCurThreadData != NULL)
			{
				// Thread has not finished yet, tell it to finish.
				m_pCurThreadData->bTerminate = TRUE;
			}
			LeaveCriticalSection (&m_critsec);
			m_pCurThreadData = NULL;

			#if 0  // Why make the user wait when she doesn't have to, huh?
			// Wait for a while for the thread to finish. No big deal if it
			// doesn't, since the thread is now orphaned.
			HCURSOR hcurPrev = ::SetCursor (::LoadCursor (NULL, IDC_WAIT));
			WaitForSingleObject (m_hThread, 5000);
			::SetCursor (hcurPrev);
			#endif

			CloseHandle (m_hThread);
			m_hThread = NULL;
		}

		if (m_pConnections != NULL)
		{
			free (m_pConnections);
			m_pConnections = NULL;
		}
	}
}

CChatServiceConnector::Socket::Socket(
CChatServiceConnector* pParent,
int					   nID)
{
	m_pParent     = pParent;
	m_nID         = nID;
	m_nServer 	  = -1;
	m_bConnecting = FALSE;
}

BOOL
CChatServiceConnector::Socket::Connect(
int			 nServer)
{
	m_nServer = nServer;

	CString strServer;
	int		nPort;
	CChatServer* pServer = m_pParent->m_pConnections[nServer].pServer;
	TranslateServerNameToServerAndPort (pServer->m_pszName, &strServer, &nPort);
	nPort = pServer->m_nPort;	// Ignore port in name.
	TRACE("Socket %d (%d) handling server %s, port %d\n", m_nID, m_hSocket, (LPCSTR)strServer, nPort);
	BOOL b = CAsyncSocket::Connect ((const SOCKADDR*)&pServer->m_sockaddr, sizeof(pServer->m_sockaddr)) || GetLastError () == WSAEWOULDBLOCK;
	if (b)
		m_bConnecting = TRUE;
	return b;
}

void 
CChatServiceConnector::Socket::OnConnect(
int nErrorCode)
{
	if (m_nID == -1)
	{
		// This socket should be closed.
		TRACE ("Closing socket handle %x\n", m_hSocket);
		m_pParent->m_mapDeleteableSockets.RemoveKey ((PVOID)this);
		LINGER lingerInfo;
		lingerInfo.l_onoff = 1;
		lingerInfo.l_linger = 0;
		SetSockOpt (SO_LINGER, &lingerInfo, sizeof(lingerInfo));
		delete this;
		return;
	}

	m_pParent->m_nActiveSockets--;
	m_bConnecting = FALSE;
	if (nErrorCode == 0)
	{
		TRACE("Socket %d connected\n", m_nID);
		CChatServer* pServer = m_pParent->m_pConnections[m_nServer].pServer;

		::AfxGetMainWnd ()->SendMessage (WM_COMMAND, ID_CONNECT_CONNECTED);

		EnterCriticalSection (&m_pParent->m_critsec);
		m_pParent->m_pConnections[m_nServer].byStatus = srvconnConnected;
		LeaveCriticalSection (&m_pParent->m_critsec);
		m_pParent->m_pConnectingServer = pServer;

		// The connection succeeded.
		serverConn.SetAuthentication (pServer->m_nAuthenticationType, 
			pServer->m_pszUserName, pServer->m_pszPassword, pServer->m_pszSecurityPackages);

		// Attach the socket to the global socket we use. When we stop using 
		// a global socket, we can eliminate this code.
		ReattachTo (&serverConn);

		// Call the global socket's OnConnect to finish the work.
		serverConn.OnConnect (0);

		// Clean up the connector. IMPORTANT: When this call returns, the this handle will be
		// invalid!
		m_pParent->Cleanup (FALSE);
	}
	else
	{
		TRACE("Socket %d returned failure, trying next server\n", m_nID);

		EnterCriticalSection (&m_pParent->m_critsec);
		m_pParent->m_pConnections[m_nServer].byStatus = srvconnFailed;
		LeaveCriticalSection (&m_pParent->m_critsec);

	}
}

// Function that reattaches current socket handle to another MFC CAsyncSocket,
// freeing ourselves of the responsibility for this socket. This is not a
// very simple thing, for the following reasons:
// 1. Detach and Attach won't work, because Detach calls into WinSock and sets
// 	  the notification window to NULL, so we may miss events in the process.
// 2. DetachHandle and AttachHandle won't work, because if the socket is the
//    last one around, DetachHandle destroys the notification window, letting
//	  AttachHandle create another, so we'd lose ALL subsequent events.
// So the solution is to sort of duplicate the AttachHandle/DetachHandle code
// without doing anything of the nasty sort described above.

// AAARGH - The Alpha compiles use MFC 4.0 libraries with MFC 4.1 includes.
// So do this stuff in a different way.

#ifdef _M_ALPHA

class _AFX_SOCK_THREAD_STATE : public CNoTrackObject
{
public:
	// WinSock specific thread state
	HWND m_hSocketWindow;
	CMapPtrToPtr m_mapSocketHandle;
	CMapPtrToPtr m_mapDeadSockets;
	CPtrList m_listSocketNotifications;

	virtual ~_AFX_SOCK_THREAD_STATE();
};

EXTERN_THREAD_LOCAL(_AFX_SOCK_THREAD_STATE, _afxSockThreadState)

#endif

void 
CChatServiceConnector::Socket::ReattachTo(
CAsyncSocket* pOtherSocket)
{
	SOCKET hSocket = m_hSocket;
	ASSERT(hSocket != INVALID_SOCKET);
	ASSERT(pOtherSocket->m_hSocket == INVALID_SOCKET);
	
	#ifdef _M_ALPHA
	_AFX_SOCK_THREAD_STATE* pState = _afxSockThreadState;
	#else
	AFX_MODULE_THREAD_STATE* pState = AfxGetModuleThreadState ();
	#endif
	
	pState->m_mapSocketHandle.SetAt ((void*)hSocket, pOtherSocket);
	m_hSocket = INVALID_SOCKET;
	pOtherSocket->m_hSocket = hSocket;
}

// Thread procedure to resolve DNS names in the background.

UINT 
CChatServiceConnector::ResolverThreadProc(
PVOID pvParam)
{
	ThreadData* pData = (ThreadData*)pvParam;
	CChatServiceConnector * pThis = pData->pThis;
	CChatServer svrCopy ("");

	// First, go through every connection, marking all servers whose DNS addresses
	// we already have as being successful. Servers with DNS addresses marked as
	// errors are reset.

	int nConnections;
	int i;

	EnterCriticalSection (&pThis->m_critsec);
	if (pData->bTerminate)
		goto AbnormalTermination;
	nConnections = pThis->m_nServers;
	for (i = 0; i < nConnections; i++)
	{
		if (pThis->m_pConnections[i].pServer->m_sockaddr.sin_family == AF_INET)
			pThis->m_pConnections[i].byStatus = srvconnNotAttempted;
		else if (pThis->m_pConnections[i].pServer->m_sockaddr.sin_family == -1)
			pThis->m_pConnections[i].pServer->m_sockaddr.sin_family = 0;
	}
	LeaveCriticalSection (&pThis->m_critsec);

	// Now, go through every connection again, resolving addresses of servers
	// which we haven't handled. We have to do this very carefully, because the
	// parent object could tell us to go away at any time. We do this by making
	// a copy of the server we are resolving. (Well, not exactly - we only copy
	// the name and port, since we don't need anything else).

	CChatServer * pServer;
	BOOL bNeedResolving;
	BOOL bOK;
	for (i = 0; i < nConnections; i++)
	{
		// Make a copy of the server.
		EnterCriticalSection (&pThis->m_critsec);
		if (pData->bTerminate)
			goto AbnormalTermination;
		bNeedResolving = pThis->m_pConnections[i].byStatus == srvconnUnresolved;
		if (bNeedResolving)
		{
			pServer = pThis->m_pConnections[i].pServer;
			free (svrCopy.m_pszName);
			svrCopy.m_pszName = strdup (pServer->m_pszName);
			svrCopy.m_nPort = pServer->m_nPort;
			svrCopy.m_sockaddr.sin_family = 0;
		}
		LeaveCriticalSection (&pThis->m_critsec);

		if (bNeedResolving)
		{
			// Resolve the DNS.
			TRACE("Resolving Socket Address for %s\n", svrCopy.m_pszName);
			bOK = svrCopy.ResolveSocketAddress ();
			
			EnterCriticalSection (&pThis->m_critsec);
			if (pData->bTerminate)
				goto AbnormalTermination;

			// Set the server's status depending on the result code.
			if (bOK)
			{
				TRACE0("Address resolution succeeded\n");
				pServer->m_sockaddr = svrCopy.m_sockaddr;
				pThis->m_pConnections[i].byStatus = srvconnNotAttempted;
			}
			else
			{
				TRACE0("Address resolution failed\n");
				pThis->m_pConnections[i].byStatus = srvconnFailed;
			}
			LeaveCriticalSection (&pThis->m_critsec);
		}
	}

	// We're all done. Now, enter a critical section and fall through to 
	// the termination code.

	EnterCriticalSection (&pThis->m_critsec);

AbnormalTermination:
	// We are being told by the parent to go away quietly. Note that this piece
	// of code is always entered while we own the critical section.

	if (!pData->bTerminate)
	{
		// We haven't been orphaned yet, so set the variable in the parent.
		pThis->m_pCurThreadData = NULL;
	}
	free (pData);
	LeaveCriticalSection (&pThis->m_critsec);
	TRACE0("Resolver thread exiting\n");
	return 0;
}

// =================================================================================
// CChatServiceComboBox implementation

CSmallIcon iconConnections[2];
UINT nConnectionIconIDs[] = { IDI_CONNECT_SRV, IDI_CONNECT_NET };

HICON 
CChatServiceComboBox::GetIcon(
UINT   nIndex, 
LPCSTR pszString, 
DWORD  dwItemData)
{
	int nWhichIcon;

	if (dwItemData == 0)
		return NULL;
	else if (((CChatService*)dwItemData)->GetServer () == NULL)
		nWhichIcon = 1;
	else
		nWhichIcon = 0;
	iconConnections[nWhichIcon].LoadIcon (nConnectionIconIDs[nWhichIcon]);
	return (HICON)iconConnections[nWhichIcon];
}

BOOL 
CChatServiceComboBox::ShouldDrawDivision(
UINT   nIndex, 
LPCSTR pszString, 
DWORD  dwItemData)
{
	// Draw a division between user specified types and groups, 
	// and between groups and servers.

	int nCount = GetCount ();
	if ((int)nIndex == nCount - 1)
		return FALSE;
	
	CChatService* pSvc = (CChatService*)dwItemData;
	CChatService* pSvcNext = (CChatService*)GetItemData (nIndex + 1);
	BOOL bIsServer = pSvc != NULL && pSvc->GetServer () != NULL;
	BOOL bIsServerNext = pSvcNext != NULL && pSvcNext->GetServer () != NULL;

	return bIsServer != bIsServerNext;
}

void 
CChatServiceComboBox::Fill(
BOOL bNonEmptyGroupsOnly)
{
	// Does not reset the content of the combo box - you can add other items 
	// and then call this to add the available services.

	int nBaseCount = GetCount ();

	ASSERT (m_pSvcList != NULL);
	int nNumGroups = 0;
	BOOL bAnyServers = FALSE;
	CChatService* pSvc = NULL;
	int nIndex;
	while (m_pSvcList->EnumServices (pSvc))
	{
		if (pSvc->GetServer () == NULL)
		{
			if (bNonEmptyGroupsOnly)
			{
				CChatServerGroup* pGroup = m_pSvcList->FindGroup (pSvc->GetGroup ());
				if (pGroup == NULL || pGroup->IsEmpty ())
					continue;
			}
			nIndex = bAnyServers ? nNumGroups + nBaseCount : -1;
			nNumGroups++;
		}
		else
		{
			nIndex = -1;
			bAnyServers = TRUE;
		}

		nIndex = InsertString (nIndex, pSvc->GetDisplayName ());
		SetItemData (nIndex, (DWORD)pSvc);
	}
}

CChatService* 
CChatServiceComboBox::GetServiceAt(
int n)
{
	DWORD dwData = GetItemData (n);
	if (dwData == (DWORD)CB_ERR)
		dwData = 0;
	return (CChatService*)dwData;
}

// =================================================================================
// CChatPasswordDialog implementation

const DWORD CChatPasswordDialog::m_nHelpIDs[] =
{
	0, 0
};

/////////////////////////////////////////////////////////////////////////////
CChatPasswordDialog::CChatPasswordDialog(
LPCSTR pszServerName, 
LPCSTR pszUserName, 
BOOL bRememberPassword,
CWnd* pParentWnd)
:
CCSDialog(CChatPasswordDialog::IDD, pParentWnd)
{
	m_strServerName = pszServerName;
	m_strUserName = pszUserName;
	m_bRememberPassword = bRememberPassword;
}

void
CChatPasswordDialog::DoDataExchange(
CDataExchange* pDX)
{
	DDX_Text (pDX, IDC_SERVERNAME, m_strServerName);
	DDX_Text (pDX, IDC_USERNAME, m_strUserName);
	DDX_Text (pDX, IDC_PASSWORD, m_strPassword);
	DDX_Check (pDX, IDC_REMEMBER_PASSWORD, m_bRememberPassword);
	CCSDialog::DoDataExchange (pDX);
}

BOOL 
CChatPasswordDialog::OnInitDialog()
{
	CCSDialog::OnInitDialog ();
	GetDlgItem (IDC_PASSWORD)->SetFocus ();
	return FALSE;
}

// =================================================================================
// CChatServiceUI implementation

CChatServiceUI::~CChatServiceUI()
{
	Reset (TRUE);
}

// Enumerate server groups. 

HCHATSRVGROUP 
CChatServiceUI::EnumGroups(
POSITION &pos,
BOOL &bUnassociatedGroup)
{
	// First return items in the added groups array.

	if (HIWORD(pos) == 0)
	{
		if (m_bChangesMade)
		{
			int i = (int)pos;
			int nArrSize = m_arrGroupsAdded.GetSize ();
			while (i < nArrSize)
			{
				if (!m_arrGroupsAdded[i].IsEmpty ())
					break;
				i++;
			}
			if (i < nArrSize)
			{
				bUnassociatedGroup = FALSE;
				pos = (POSITION)(i + 1);
				return (HCHATSRVGROUP)pos;
			}
		}
		pos = NULL;
	}

	// Now go through each group already in the service list. Only return
	// groups that are not in the added or removed groups array.
	// If a group has been added, it is in the added groups array.
	// If a group has been removed, it is in the removed groups array.
	// If a group has been removed and subsequently readded, it is in both arrays.

	CChatServerGroup * pGroup = (CChatServerGroup*)pos;
	do
	{
		if (!m_pSvcList->EnumGroups (pGroup, bUnassociatedGroup))
			return NULL;
	} while (!bUnassociatedGroup && m_bChangesMade &&
				(m_arrGroupsAdded.Find (pGroup->m_pszName) ||
					m_arrGroupsRemoved.Find (pGroup->m_pszName)));
	pos = (POSITION)pGroup;
	return (HCHATSRVGROUP)pGroup;
}

// Enumerate servers in a group. Once you start an enumeration, you must either finish
// it, or call EnumServersInGroup with hGroup set to NULL and the last value of pos -
// this is because this enumeration allocates resources that must be freed.

struct ENUMSERVER
{
	CString strGroupName;
	int		nNameLen;
	int     nArray;
	CChatServer* pServer;
};

HCHATSERVER 
CChatServiceUI::EnumServersInGroup(
HCHATSRVGROUP hGroup, 
POSITION &pos)
{
	ENUMSERVER* pEnum;

	if (hGroup == NULL)
	{
		ASSERT(pos != NULL);
		delete (ENUMSERVER*)pos;
		return NULL;
	}

	if (pos == NULL)
	{
		pEnum = new ENUMSERVER;
		pEnum->strGroupName.Format ("//%s/", GetGroupName (hGroup));
		pEnum->nNameLen = pEnum->strGroupName.GetLength (); 
		pEnum->nArray = m_bChangesMade ? 0 : -1;
		pEnum->pServer = NULL;
		pos = (POSITION)pEnum;
	}
	else
	{
		pEnum = (ENUMSERVER*)pos;
	}

	// Go through items in the added servers array.

	if (pEnum->nArray >= 0)
	{
		int nArrSize = m_arrServersAdded.GetSize ();
		while (pEnum->nArray < nArrSize)
		{
			if (!strncmp (m_arrServersAdded[pEnum->nArray], pEnum->strGroupName, pEnum->nNameLen))
				break;
			pEnum->nArray++;
		}
		if (pEnum->nArray < nArrSize)
		{
			pEnum->nArray++;
			return (HCHATSERVER)pEnum->nArray;
		}
		pEnum->nArray = -1;
	}

	// If this group isn't already in the service list, that's all, there are no more
	// entries.

	if (!HIWORD(hGroup))
	{
		delete pEnum;
		return NULL;
	}

	// Go through servers in group, skipping over items in the added or removed
	// servers array.

	CChatServerGroup * pGroup = (CChatServerGroup*)hGroup;
	CString strServer;
	do
	{
		if (!pGroup->EnumServers (pEnum->pServer))
		{
			delete pEnum;
			return NULL;
		}
		strServer.Format ("//%s/%s", (LPCSTR)pEnum->strGroupName, pEnum->pServer->m_pszName);
	} while (m_bChangesMade && (m_arrServersAdded.Find (strServer) || 
									m_arrServersRemoved.Find (strServer)));
	
	return (HCHATSERVER)pEnum->pServer;
}

// Checks if a group is empty.

BOOL 
CChatServiceUI::IsGroupEmpty(
HCHATSRVGROUP hGroup)
{
	HCHATSERVER hServer;
	POSITION pos = NULL;
	hServer = EnumServersInGroup (hGroup, pos);
	if (hServer != NULL)
	{
		// Abort the enumeration.
		EnumServersInGroup (NULL, pos);
	}
	return hServer == NULL;
}

// Get the name of a group.

LPCSTR 
CChatServiceUI::GetGroupName(
HCHATSRVGROUP hGroup)
{
	ASSERT(hGroup != NULL);
	return (HIWORD(hGroup) == 0) ? (LPCSTR)m_arrGroupsAdded[((int)hGroup) - 1] 
								 : ((CChatServerGroup*)hGroup)->m_pszName;
}

// Get the name of a server.

LPCSTR 
CChatServiceUI::GetServerName(
HCHATSERVER hServer)
{
	ASSERT(hServer != NULL);
	if (HIWORD(hServer) == 0) 
	{
		LPCSTR psz = (LPCSTR)m_arrServersAdded[((int)hServer) - 1];
		psz = OurMbsChr (psz + 2, '/');
		ASSERT(psz);
		return psz + 1;
	}
	else
		return ((CChatServer*)hServer)->m_pszName;
}

// Gets the server properties as a struct.

void 
CChatServiceUI::GetServerProps(
HCHATSERVER hServer, 
ServerProps& data)
{
	ServerProps * pData;
	if (m_bChangesMade && m_mapServerPropsChanged.Lookup (hServer, pData))
	{
		data = *pData;
		return;
	}

	// Data hasn't been changed, so it better be in the original!

	ASSERT(HIWORD(hServer));
	data = *(CChatServer *)hServer;
}

// Sets the server properties. Really just creates an entry for that server, so that
// applying changes will set the data.

BOOL
CChatServiceUI::SetServerProps(
HCHATSRVGROUP hGroup,
HCHATSERVER hServer, 
ServerProps& data)
{
	ServerProps * pData = NULL;
	if (!m_bChangesMade || !m_mapServerPropsChanged.Lookup (hServer, pData))
	{
		TRY
		{
			pData = new ServerProps;
			m_mapServerPropsChanged.SetAt (hServer, pData);
		}
		CATCH_ALL(e)
		{
			if (pData)
				delete pData;
			return FALSE;
		}
		END_CATCH_ALL
	}
	*pData = data;
	pData->m_pGroupIn = HIWORD(hGroup) ? (CChatServerGroup*)hGroup : NULL;
	m_bChangesMade = TRUE;
	return TRUE;
}

// Add a server. It is assumed the server does not exist - it is up to the caller
// to verify this.

HCHATSERVER
CChatServiceUI::AddServer(
HCHATSRVGROUP hGroup, 
LPCSTR pszServer, 
int nPort)
{
	LPCSTR pszGroupName = GetGroupName (hGroup);
	CString strName;
	strName.Format ("//%s/%s", pszGroupName, pszServer);
	UINT nPos = 0;
	
	ServerProps data;
	data.m_nPort = nPort;
	data.m_nAuthenticationType = 0;
	data.m_bRememberPassword = FALSE;

	TRY
	{
		nPos = m_arrServersAdded.Add (strName);
		ASSERT(nPos > 0);
		if (!SetServerProps (hGroup, (HCHATSERVER)nPos, data))
		{
			::AfxThrowUserException ();
		}
	}
	CATCH_ALL(e)
	{
		if (nPos > 0)
			m_arrServersAdded.Remove (nPos);
	}
	END_CATCH_ALL

	m_bChangesMade = TRUE;
	return (HCHATSERVER)nPos;
}

// Remove a server. The server is assumed to exist in the group - it is up to the caller
// to verify this.

BOOL 
CChatServiceUI::RemoveServer(
HCHATSRVGROUP hGroup, 
HCHATSERVER hServer)
{
	CString strName;
	if (HIWORD(hServer))
		strName.Format ("//%s/%s", GetGroupName (hGroup), ((CChatServer*)hServer)->m_pszName);
	else
	{
		strName = m_arrServersAdded[((int)hServer) - 1];
		// Server is in added servers list, so remove it.
		m_arrServersAdded.Remove ((UINT)hServer);
	}

	// Changes to server properties are no longer useful.
	ServerProps * pData;
	if (m_mapServerPropsChanged.Lookup (hServer, pData))
	{
		delete pData;
		m_mapServerPropsChanged.RemoveKey (hServer);
	}

	// Only add the server to the removed servers array if it's not there already.
	if (!m_arrServersRemoved.Find (strName))
	{
		m_arrServersRemoved.Add (strName);
		m_bChangesMade = TRUE;
	}

	return TRUE;
}

// Add a server group. It is assumed the server group does not exist - it is up to the caller
// to verify this.

HCHATSRVGROUP 
CChatServiceUI::AddGroup(
LPCSTR pszGroup)
{
	UINT nPos;
	TRY
	{
		nPos = m_arrGroupsAdded.Add (CString (pszGroup));
		ASSERT(nPos > 0);
	}
	CATCH_ALL(e)
	{
		return NULL;
	}
	END_CATCH_ALL

	m_bChangesMade = TRUE;
	return (HCHATSRVGROUP)nPos;
}

// Remove a server group. The server group is assumed to exist - it is up to the caller
// to verify this.

BOOL 
CChatServiceUI::RemoveGroup(
HCHATSRVGROUP hGroup)
{
	CString strGroupName (GetGroupName (hGroup));

	// Remove all server property change items.
	if (!m_mapServerPropsChanged.IsEmpty ())
	{
		HCHATSERVER hServer;
		POSITION pos = NULL;
		ServerProps* pData;
		while ((hServer = EnumServersInGroup (hGroup, pos)) != NULL)
		{
			if (m_mapServerPropsChanged.Lookup (hServer, pData))
			{
				delete pData;
				m_mapServerPropsChanged.RemoveKey (hServer);
			}
		}
	}

	// Remove all server entries in added/removed servers.

	if (m_bChangesMade)
	{
		CString strPrefix;
		strPrefix.Format ("//%s/", (LPCSTR)strGroupName);
		int nCmpLen = strPrefix.GetLength ();
		ObjArray* pArray[2] = { &m_arrServersAdded, &m_arrServersRemoved };
		for (int iArray = 0; iArray < _countof(pArray); iArray++)
		{
			int nCount = pArray[iArray]->GetSize ();
			for (int i = 0; i < nCount; i++)
			{
				if (!strncmp (pArray[iArray]->GetAt (i), strPrefix, nCmpLen))
				{
					pArray[iArray]->Remove (i + 1);
					i--;
					nCount--;
				}
			}
		}
	}

	if (HIWORD(hGroup) == 0)
	{
		// The group is in the added groups array. Remove it.
		m_arrGroupsAdded.Remove ((UINT)hGroup);
	}

	// Only add the group to the removed groups array if it's not there already.
	if (!m_arrGroupsRemoved.Find (strGroupName))
	{
		m_arrGroupsRemoved.Add (strGroupName);
		m_bChangesMade = TRUE;
	}
	return TRUE;
}

// Applies all changes.

BOOL
CChatServiceUI::Apply()
{
	if (!m_bChangesMade)
		return TRUE;

	int i;
	int nCount;
	BOOL bRet = TRUE;

	TRY
	{
		CChatServerGroup* pGroup;
		CChatServer* pServer;
		ServerProps* pProps;
	
		// Destroy "removed groups".
	
		nCount = m_arrGroupsRemoved.GetSize ();
		for (i = 0; i < nCount; i++)
		{
			if (m_arrGroupsRemoved[i].IsEmpty ())
				continue;
			pGroup = m_pSvcList->FindGroup (m_arrGroupsRemoved[i]);
			if (pGroup != NULL)
			{
				m_pSvcList->RemoveReferences (pGroup->m_pszName, NULL);
				if (!m_pSvcList->DestroyGroup (pGroup))
					::AfxThrowUserException ();
			}
		}
	
		// Create "added groups".
	
		nCount = m_arrGroupsAdded.GetSize ();
		for (i = 0; i < nCount; i++)
		{
			if (m_arrGroupsAdded[i].IsEmpty ())
				continue;
			if (!m_pSvcList->CreateGroup (m_arrGroupsAdded[i]) &&
					!m_pSvcList->FindGroup (m_arrGroupsAdded[i]))
				::AfxThrowUserException ();
		}
	
		// Destroy "removed servers".
	
		nCount = m_arrServersRemoved.GetSize ();
		for (i = 0; i < nCount; i++)
		{
			if (m_arrServersRemoved[i].IsEmpty ())
				continue;
			if (TranslateServerName (m_arrServersRemoved[i], &pGroup, NULL, &pServer))
			{
				m_pSvcList->RemoveReferences (pGroup->m_pszName, pServer->m_pszName);
				if (!pGroup->DestroyServer (pServer))
					::AfxThrowUserException ();
			}
		}
	
		// Create "added servers".
		
		CString strServer;
		nCount = m_arrServersAdded.GetSize ();
		for (i = 0; i < nCount; i++)
		{
			if (m_arrServersAdded[i].IsEmpty ())
				continue;
			if (!m_mapServerPropsChanged.Lookup ((HCHATSERVER)(i + 1), pProps))
				continue;
			if (TranslateServerName (m_arrServersAdded[i], &pGroup, &strServer))
			{
				pProps->m_pGroupIn = pGroup;
				pServer = pGroup->CreateServer (strServer, 6667);
				if (pServer != NULL)
				{
					// If the group is <none>, add it to the service list.

					if (!lstrcmp (pGroup->m_pszName, UNASSOCIATED_GROUP))
					{
						m_pSvcList->CreateService (NULL, strServer);
					}
					m_mapServerPropsChanged.RemoveKey ((HCHATSERVER)(i + 1));
					m_mapServerPropsChanged.SetAt ((HCHATSERVER)pServer, pProps);
				}
				else if (!pGroup->FindServer (strServer))
					::AfxThrowUserException ();
			}
		}

		// Save all changes to server properties.

		HCHATSERVER hServer;
		POSITION pos = m_mapServerPropsChanged.GetStartPosition ();
		while (pos)
		{
			m_mapServerPropsChanged.GetNextAssoc (pos, hServer, pProps);
			if (HIWORD(hServer))		// A real server
			{
				pServer = (CChatServer*)hServer;
				if (pProps->m_pGroupIn == NULL)
				{
					ASSERT(FALSE);
				}
				else
				{
					pServer->FreeSettings ();
					pServer->m_nPort = pProps->m_nPort;
					pServer->m_nAuthenticationType = pProps->m_nAuthenticationType;
					pServer->m_pszUserName = pProps->m_strUserName.IsEmpty () ? NULL : strdup (pProps->m_strUserName);
					pServer->m_pszPassword = pProps->m_strPassword.IsEmpty () ? NULL : strdup (pProps->m_strPassword);
					pServer->m_bRememberPassword = pProps->m_bRememberPassword;
					pServer->m_pszSecurityPackages = pProps->m_strSecurityPackages.IsEmpty () 
												? NULL : strdup (pProps->m_strSecurityPackages);
					HKEY hkeyGroup = CChatServiceList::GetRegistryKey (CHATSVC_HKEY_SRVGROUP, pProps->m_pGroupIn->m_pszName);
					if (!hkeyGroup)
						::AfxThrowUserException ();
					pServer->WriteToRegistry (hkeyGroup);
					CChatServiceList::ReleaseRegistryKey (hkeyGroup);
				}
			}
		}
	}
	CATCH_ALL(e)
	{
		ASSERT(FALSE);
		bRet = FALSE;
	}
	END_CATCH_ALL
	Reset ();
	m_pSvcList->WriteIfChanged ();
	return bRet;
}


// Resets the structure back, after an Apply or before destruction.

void 
CChatServiceUI::Reset(
BOOL bOnDestruction)
{
	// There's no need to do this stuff in the destructor, since the normal
	// destructors do it for us.
	if (!bOnDestruction)
	{
		m_arrGroupsAdded.RemoveAll ();
		m_arrGroupsRemoved.RemoveAll ();
		m_arrServersAdded.RemoveAll ();
		m_arrServersRemoved.RemoveAll ();
		m_bChangesMade = FALSE;
	}

	POSITION pos;
	pos = m_mapServerPropsChanged.GetStartPosition ();
	HCHATSERVER hServer;
	ServerProps* pData;
	while (pos != NULL)
	{
		m_mapServerPropsChanged.GetNextAssoc (pos, hServer, pData);
		delete pData;
		m_mapServerPropsChanged.RemoveKey (hServer);
	}
}

// Translates //xxxxx/xxxxx server name to group and server name, and server if the 
// optional pServer parameter is provided. Returns TRUE if the group exists and either
// pServer is NULL or the server exists.

BOOL 
CChatServiceUI::TranslateServerName(
LPCSTR pszName, 
CChatServerGroup * * pGroup, 
CString *pstrServer,
CChatServer* * pServer)
{
	ASSERT (pszName[0] == '/' && pszName[1] == '/');
	LPCSTR pszFind = OurMbsChr (pszName + 2, '/');
	ASSERT (pszFind != NULL);
	CString strGroup (pszName + 2, pszFind - pszName - 2);
	*pGroup = m_pSvcList->FindGroup (strGroup);
	if (*pGroup == NULL)
		return FALSE;
	if (pstrServer != NULL)
		*pstrServer = (pszFind + 1);
	if (pServer != NULL)
	{
		*pServer = (*pGroup)->FindServer (pszFind + 1);
		if (*pServer == NULL)
			return FALSE;
	}
	return TRUE;
}

// Assignment operator to copy server properties structure

const CChatServiceUI::ServerProps&
CChatServiceUI::ServerProps::operator =(
const CChatServiceUI::ServerProps &data)
{
	if (&data != this)
	{
		m_nPort 				= data.m_nPort;
		m_nAuthenticationType 	= data.m_nAuthenticationType;
		m_strUserName 			= data.m_strUserName;
		m_strPassword 			= data.m_strPassword;
		m_strSecurityPackages 	= data.m_strSecurityPackages;
		m_bRememberPassword 	= data.m_bRememberPassword;
	}
	return *this;
}

// Assignment operator to create server properties structure from a server.

const CChatServiceUI::ServerProps&
CChatServiceUI::ServerProps::operator =(
const CChatServer& Server)
{
	m_nPort 				= Server.m_nPort;
	m_nAuthenticationType 	= Server.m_nAuthenticationType;
	m_strUserName 			= Server.m_pszUserName ? Server.m_pszUserName : "";
	m_strPassword 			= Server.m_pszPassword ? Server.m_pszPassword : "";
	m_strSecurityPackages 	= Server.m_pszSecurityPackages ? Server.m_pszSecurityPackages : "";
	m_bRememberPassword 	= Server.m_bRememberPassword;
	return *this;
}

// Functions for a string-base "object array". Once you add something to the array,
// you get back an ID, and can forever use that ID to find the object. This is implemented
// by blanking entries on delete, and then looking for these blank entries when an
// object is added.

CChatServiceUI::ObjArray::ObjArray()
{
	m_nFirstEmpty = 0;
	m_nNumEmpty = 0;
	m_nLastSearch = -1;
}

UINT 
CChatServiceUI::ObjArray::Add(
CString &str)
{
	ASSERT(!str.IsEmpty ());
	int n;
	if (m_nNumEmpty > 0)
	{
		// Use the empty slot. and then find the next one.
		n = m_nFirstEmpty;
		SetAt (n, str);
		if (--m_nNumEmpty > 0)
		{
			int nNextEmpty = n;
			int nLast = GetSize ();
			while (nNextEmpty < nLast && !GetAt (nNextEmpty).IsEmpty ())
				nNextEmpty++;
			ASSERT(nNextEmpty < nLast);
			m_nFirstEmpty = nNextEmpty;
		}
	}
	else
	{
		CStringArray::Add (str);
		n = GetUpperBound ();
	}
	return n + 1;
}

void 
CChatServiceUI::ObjArray::Remove(
UINT nID)
{
	ASSERT(nID > 0);
	int nIndex = nID - 1;
	SetAt (nIndex, "");
	if (m_nNumEmpty++ == 0 || nIndex < m_nFirstEmpty)
		m_nFirstEmpty = nIndex;
}

UINT 
CChatServiceUI::ObjArray::Find(
LPCSTR psz)
{
	ASSERT(psz != NULL && *psz != '\0');

	// The last searched value is cached, for some speed improvement.
	if (m_nLastSearch != -1 && !lstrcmpi (psz, GetAt (m_nLastSearch)))
		return m_nLastSearch + 1;

	for (int i = GetUpperBound (); i >= 0; i--)
	{
		if (!lstrcmpi (psz, GetAt (i)))
		{
			m_nLastSearch = i;
			return i + 1;
		}
	}
	
	return 0;
}
