#ifndef __CHATSRV_H_EDB4D47B_A324_11D1_B7EC_00C04FA3426D__
#define __CHATSRV_H_EDB4D47B_A324_11D1_B7EC_00C04FA3426D__

// CHATSRV.H
// Classes that manage information about chat servers. Mainly encapsulates
// data kept in the registry.
// Some nomenclature (not necessarily what appears to the user, but what
// appears in the code):
//		Chat Server				a single server
//		Chat Server Group 		A group of servers defining a network    
//		Chat Service   			an individual server or a whole network - 
//								a point of service to which the user can 
//								connect. This is a single entry in the list
//								of connections shown to the user.
//		Chat Service List		a list of all available services, as shown to the user.


// A chat server's data is saved in the registry in the following format.

class CChatServer : protected CListObject
{
public:
	CChatServer(LPCSTR pszName, PVOID pvData = NULL, UINT nDataLen = 0);
	virtual ~CChatServer();

	BOOL WriteToRegistry(HKEY hkeyReg);
	void SetDefaultSettings();
	void FreeSettings();
	BOOL ResolveSocketAddress();

	LPSTR m_pszName;
	UINT  m_nPort;
	UINT  m_nAuthenticationType;
	LPSTR m_pszUserName;
	LPSTR m_pszPassword;
	LPSTR m_pszSecurityPackages;
	BOOL  m_bRememberPassword;
	SOCKADDR_IN m_sockaddr;

protected:
	enum AuthType
	{
		authtypeNone = 0,
		authtypePlainText = 1,
		authtypeServerPackages = 2,
		authtypeCustomPackages = 3,
	};
	enum DataType
	{
		datatypePort 					= 0x01,
		datatypeAuthenticationType 		= 0x02,
		datatypeUserName				= 0x03,
		datatypeUserPassword			= 0x04,
		datatypeSecurityPkg				= 0x05,
		datatypeRememberPassword		= 0x06,
	};

	void ReadFromData(PVOID pvData, UINT nDataLen);
	BOOL WriteToData(PVOID * ppvDataOut, UINT * pnDataLen);
};

DECLARE_LISTSUPPORT(CChatServerObList, CChatServer)


class CChatServerGroup : protected CListObject
{
public:
	CChatServerGroup(LPCSTR pszName);
	virtual ~CChatServerGroup();

	BOOL ContainsServer(LPCSTR pszServer);			// Lightweight call
	CChatServer* FindServer(LPCSTR pszServer);	 	// Heavyweight call
	CChatServer* CreateServer(LPCSTR pszName, int nPort = 6667);
	BOOL DestroyServer(CChatServer* pServer);
	BOOL IsEmpty();
	int GetServerCount();
	BOOL EnumServers(CChatServer* &pServer);
	void SetLastAccessedServer(CChatServer* pServer);

	LPSTR			m_pszName;
	LPSTR			m_pszLastServer;

protected:
	BOOL ReadFromRegistry();

	BOOL 			m_bIsRead;
	CChatServerObList m_listServers; 
};

DECLARE_LISTSUPPORT(CChatServerGroupObList, CChatServerGroup)


class CChatService : protected CListObject
{
public:
	CChatService(LPCSTR pszService);
	CChatService(LPCSTR pszGroup, LPCSTR pszServer)
		{ CommonConstruct (pszGroup, pszServer); }
	virtual ~CChatService();
	
	LPCSTR GetGroup()
		{ return m_pszGroup; }
	LPCSTR GetServer()
	    { return m_pszServer; }
	LPCSTR GetDisplayName()
		{ return (m_pszServer != NULL) ? m_pszServer : m_pszGroup; }
	void FormatAsServiceName(CString &strOut);

protected:
	void CommonConstruct(LPCSTR pszGroup, LPCSTR pszServer);
	LPSTR m_pszGroup;
	LPSTR m_pszServer;
};

DECLARE_LISTSUPPORT(CChatServiceObList, CChatService)


#define CHATSVC_HKEY_ROOT				0
#define CHATSVC_HKEY_SVCLIST			1
#define CHATSVC_HKEY_SRVGROUP			2
#define CHATSVC_HKEY_PARENT				3

class CChatServiceList
{
public:
	CChatServiceList() 
		{ m_nOldReadCount = 0; m_bSvcListModified = FALSE; }
	~CChatServiceList() 
		{}

	BOOL ReadFromRegistry();
	BOOL WriteToRegistry();
	BOOL WriteIfChanged();
	CChatServerGroup* CreateGroup(LPCSTR pszName);
	BOOL DestroyGroup(CChatServerGroup* pGroup);
	BOOL EnumGroups(CChatServerGroup* &pGroup, BOOL &bUnassociatedGroup);
	BOOL EnumServices(CChatService* &pSvc);
	CChatServerGroup* FindGroup(LPCSTR pszName);
	CChatService* FindService(LPCSTR pszGroup, LPCSTR pszServer);
	void MoveServiceToTop(CChatService* pSvc);
	void DestroyService(CChatService* pSvc)
		{ m_listServices.Remove (pSvc); delete pSvc; }
	CChatService* CreateService(LPCSTR pszGroup, LPCSTR pszServer);
	static HKEY GetRegistryKey(DWORD dwRegKeyType = CHATSVC_HKEY_ROOT, LPCSTR pszSection = NULL);
	static void ReleaseRegistryKey(HKEY hkey);
	void GetServiceNameFromDisplayName(LPCSTR pszDisplayName, CString& strService);
	void RemoveReferences(LPCSTR pszGroup, LPCSTR pszServer);
	BOOL ImportFromFile(LPCSTR pszFile);

protected:
	BOOL GetGroupForOldServer(LPCSTR pszServer, CString& strGroupOut);
	static BOOL AddOldServer(LPCSTR pszServer, PVOID pvData);
	BOOL AddOldServer(LPCSTR pszServer);

	CChatServiceObList 		m_listServices;
	CChatServerGroupObList 	m_listSrvGroups;
	int						m_nOldReadCount;
	static HKEY				sm_hkeyCachedMain;
	static LPCSTR 	 		sm_pszRegKeyName;
	BOOL					m_bSvcListModified;
};

void TranslateServerNameToServerAndPort(LPCSTR pszServer, CString * pstrServer, int * pnPort);

// Chat Service UI class. Provides a layer on top of the Chat Service API that
// user interfaces can use. The main feature of going through this class is
// automatic undo/apply facilities.
// Instead of dealing directly with CChatServerGroup and CChatServer pointers,
// UI's deal with HCHATSRVGROUP and HCHATSERVER handles. When the UI calls for
// applying changes, call Apply and the changes will be reflected in the underlying
// service list.

typedef PVOID HCHATSRVGROUP;
typedef PVOID HCHATSERVER;

class CChatServiceUI
{
public:
	CChatServiceUI()
		{ m_pSvcList = NULL; m_bChangesMade = FALSE; }
	virtual ~CChatServiceUI();

	struct ServerProps
	{
		const ServerProps& operator =(const ServerProps &data);
		const ServerProps& operator =(const CChatServer &server);
		UINT  	m_nPort;
		UINT  	m_nAuthenticationType;
		CString m_strUserName;
		CString m_strPassword;
		CString m_strSecurityPackages;
		BOOL  	m_bRememberPassword;
		CChatServerGroup* m_pGroupIn;
	};

	void SetServiceList(CChatServiceList* pList)
		{ ASSERT (pList != NULL && m_pSvcList == NULL); m_pSvcList = pList; }
	HCHATSRVGROUP EnumGroups(POSITION &pos, BOOL &bUnassociatedGroup);
	HCHATSERVER EnumServersInGroup(HCHATSRVGROUP hGroup, POSITION &pos);
	LPCSTR GetGroupName(HCHATSRVGROUP hGroup);
	LPCSTR GetServerName(HCHATSERVER hServer);
	void GetServerProps(HCHATSERVER hServer, ServerProps& data);
	BOOL IsGroupEmpty(HCHATSRVGROUP hGroup);

	BOOL SetServerProps(HCHATSRVGROUP hGroup, HCHATSERVER hServer, ServerProps& data);
	HCHATSERVER AddServer(HCHATSRVGROUP hGroup, LPCSTR pszServer, int nPort);
	BOOL RemoveServer(HCHATSRVGROUP hGroup, HCHATSERVER hServer);
	HCHATSRVGROUP AddGroup(LPCSTR pszGroup);
	BOOL RemoveGroup(HCHATSRVGROUP hGroup);

	BOOL Apply();
	void Revert()
		{ Reset (); }

protected:
	class ObjArray : public CStringArray
	{
	public:
		ObjArray();
		UINT Add(CString &str);
		void Remove(UINT nID);
		UINT Find(LPCSTR psz);
	protected:
		int m_nFirstEmpty;
		int m_nNumEmpty;
		int m_nLastSearch;
	};

	void Reset(BOOL bOnDestruction = FALSE);
	BOOL TranslateServerName(LPCSTR pszName, CChatServerGroup * * pGroup, 
			CString *pstrServer, CChatServer * * pServer = NULL);

	BOOL m_bChangesMade;
	ObjArray m_arrGroupsAdded;
	ObjArray m_arrGroupsRemoved;
	ObjArray m_arrServersAdded;
	ObjArray m_arrServersRemoved;
	CTypedPtrMap<CMapPtrToPtr, HCHATSERVER, ServerProps *> m_mapServerPropsChanged;
	CChatServiceList* m_pSvcList;
};


// Chat Service Connector class. This is what connects to a service, managing
// multiple server simultaneous connections, etc.

class CChatServiceConnector
{
public:
	CChatServiceConnector();
	~CChatServiceConnector();
	void SetServiceList(CChatServiceList* pSvcList)
		{ m_pSvcList = pSvcList; }

	BOOL IsConnecting()
		{ return m_pConnections != NULL; }
	int GetNumServers()
		{ return m_nServers; }
	BOOL BeginConnectToService(LPCSTR pszSvc);
	int AssignSocket(int nSocket);
	int GetNumSockets() 
		{ return m_pSockets != NULL ? m_nSockets : 0; }
	void Cleanup(BOOL bCleanupServerList = TRUE);
	CChatServerGroup* GetConnectingServerGroup()
		{ return m_pConnectingGroup; }
	CChatServer* GetConnectingServer()
		{ return m_pConnectingServer; }

	CString 		m_strSvc;
protected:
	BOOL AssignSocketToNewServer(int nSocket);
	static UINT ResolverThreadProc(PVOID pvParam);

	enum SrvConnectionStatus
	{
		srvconnUnresolved = 0,
		srvconnNotAttempted = 1,
		srvconnAttempting = 2,
		srvconnFailed = 3,
		srvconnConnected = 4,
	};
	struct SrvConnection
	{
		CChatServer* pServer;
		BYTE	     byStatus;
	};
	class Socket : public CAsyncSocket
	{
	public:
		Socket(CChatServiceConnector* pParent, int nID);
		BOOL Connect(int nServer);
		virtual void OnConnect(int nErrorCode);
		void ReattachTo(CAsyncSocket* pOtherSocket);
		CChatServiceConnector*	m_pParent;
		int						m_nID;
		int						m_nServer;
		BOOL					m_bConnecting;
	};
	struct ThreadData
	{
		CChatServiceConnector * pThis;
		BOOL					bTerminate;
	};

	CChatServiceList* m_pSvcList;
	SrvConnection*    m_pConnections;
	Socket * * 		  m_pSockets;
	int				  m_nServers;
	int				  m_nSockets;
	int				  m_nActiveSockets;
	CChatServerGroup* m_pConnectingGroup;
	CChatServer*	  m_pConnectingServer;
	HANDLE			  m_hThread;
	ThreadData*		  m_pCurThreadData;
	CRITICAL_SECTION  m_critsec;
	CMapPtrToWord	  m_mapDeleteableSockets;


	friend class Socket;
};

// Combo box that holds list of available services

class CChatServiceComboBox : public CIconicComboBox
{
public:
	CChatServiceComboBox()
		{ m_pSvcList = NULL; }
	BOOL ReplaceControl(CWnd* pParentWnd, UINT nID)
		{ return CIconicComboBox::ReplaceControl (pParentWnd, nID, WS_VSCROLL); }
	void SetServiceList(CChatServiceList* pSvcList)
		{ m_pSvcList = pSvcList; }
	void Fill(BOOL bNonEmptyGroupsOnly = FALSE);
	CChatService* GetServiceAt(int n);

	enum Flags
	{
		flagtypeEntry = 0x03,
		flagUserEntry = 0x00,
		flagServerEntry = 0x01,
		flagGroupEntry = 0x02,
	};

protected:
	virtual HICON GetIcon(UINT nIndex, LPCSTR pszString, DWORD dwItemData);
	virtual BOOL ShouldDrawDivision(UINT nIndex, LPCSTR pszString, DWORD dwItemData);
	CChatServiceList* m_pSvcList;
};

// Password dialog

class CChatPasswordDialog : public CCSDialog
{
public:
	CChatPasswordDialog(LPCSTR pszServerName, LPCSTR pszUserName, BOOL bRememberPassword, CWnd* pParentWnd = NULL);

	enum { IDD = IDD_PASSWORD };
	CString m_strPassword;
	BOOL m_bRememberPassword;

	virtual BOOL OnInitDialog();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	CString m_strServerName;
	CString m_strUserName;
};

const TCHAR g_szServicesRegName[] = _T("Servers");
const TCHAR g_szServicesList[] = _T(".SvcList");
const TCHAR g_szGroupUnassociated[] = _T("{Unassociated}");
#define UNASSOCIATED_GROUP g_szGroupUnassociated

#endif //ndef __CHATSRV_H_EDB4D47B_A324_11D1_B7EC_00C04FA3426D__
