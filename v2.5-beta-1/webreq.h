#ifndef __WEBREQ_H__EDB4D47A_A324_11D1_B7EC_00C04FA3426D__
#define __WEBREQ_H__EDB4D47A_A324_11D1_B7EC_00C04FA3426D__

// ============================================================================
// Classes for an "Internet Request" mechanism, a simple and compact way
// to download one or more files from the Internet. The caller creates a
// request and adds it to the work queue of the requestor. When the requestor has 
// finished downloaded it, it appears in the requestor's outbox.

// ============================================================================
// CLASS: CInternetRequest
// Class encapsulating a single request. Callers should create an object
// of this class (or an object of a derived class), and add it to the requestor's
// work queue. The base class only has support for a URL, a local filename, 
// and a status. Derived classes may add other information. 
// The request's status may be one of the following:
//		Free			The request is not associated with any requestor. This
//						is the initial state of the request (until it is added
//						to the requestor's work queue), and the state of the 
//						request	after it is removed from the requestor's outbox.
//		Pending			The request is in the requestor's work queue. Downloading
//						has not yet begun.
//		Downloading 	The request is being downloaded.
//		AbortPending	The request is being downloaded, but an abort is pending.   
//		Completed		The request has been downloaded, and is in the outbox.
//		Aborted			The request was aborted by the caller, and is in the outbox.
//		Failed			The request failed to download due to errors, and is in the
//						outbox.
// Ownership of a request depends on its status. Whenever the request's status is
// "Free", it is the caller's responsbility to delete the request. At all other
// times, it is the requestor's responsibility to delete the request. Because
// of the possible variety of request objects and their use, the Delete method
// of the request is used to delete the object; the destructor is not directly
// called. Note that all Delete calls are made on the primary thread - worker
// threads never delete the objects.
// OnBeginDownload, OnAddDownloadedData, OnEndDownload, and OnAbortDownload 
// are called by the requestor during downloading. The defaults for these 
// functions create a tempfile and write the contents to it.

class CInternetRequestor;

class CInternetRequest
{
public:
	CInternetRequest();
	CInternetRequest(LPCTSTR pszURL);
	virtual ~CInternetRequest();

	void Abort()
		{ m_status = statusAbortPending; }
	virtual void Delete();
	DWORD GetStatus()
		{ return m_status; }
	void SetRequestor(CInternetRequestor * pRequestor)
		{ m_pRequestor = pRequestor; }
	LPCTSTR GetURL()
		{ return m_pszURL; }
	LPCTSTR GetLocalFileName()
		{ return m_pszLocalFile; }

	// Called by requestor
	virtual BOOL OnBeginDownload();
	virtual BOOL OnAddDownloadedData(PBYTE pbData, UINT nSize);
	virtual BOOL OnEndDownload();
	virtual void OnAbortDownload();
	void SetStatus(DWORD dwStatus)
		{ m_status = dwStatus; }

	// Used in lists.
	void AddToListTail(CInternetRequest** pRequestList);
	void RemoveFromList(CInternetRequest** pRequestList);
	void DeleteList();
	int GetListCount();
	CInternetRequest* GetListElem(int nElem);
	
	enum Status
	{
		statusFree = 0,
		statusPending,
		statusDownloading,
		statusAbortPending,
		statusCompleted,
		statusAborted,
		statusFailed,
	};

protected:
	void CommonConstruct()
	    { m_status = statusFree; m_pRequestor = NULL; m_pszLocalFile = NULL; m_file = NULL; m_pNextInList = NULL; }

	LPTSTR				m_pszURL;
	LPTSTR				m_pszLocalFile;
	FILE*				m_file;
	DWORD				m_status;
	CInternetRequestor * m_pRequestor;
	CInternetRequest* 	m_pNextInList;			// Used when in a list.
};

// ============================================================================
// CLASS: CChatFileRequest
// A request for a chat file (avatar or backdrop). Derives from 
// CInternetRequest, because we need some additional data.

enum CHATFILETYPE
{
	chatfileAvatar = 0,
	chatfileBackdrop = 1,
};

#define CHATFILE_ISINTERACTIVE 		0x80

class CChatFileRequest : public CInternetRequest
{
public:
	CChatFileRequest(LPCTSTR pszName, LPCTSTR pszURL, BYTE byType, BOOL bIsInteractive = FALSE);
	virtual ~CChatFileRequest();
	LPCTSTR GetName() 
		{ return m_pszName; }
	BYTE GetType()
	    { return m_byType & ~CHATFILE_ISINTERACTIVE; }
	BOOL IsInteractive()
	    { return m_byType & CHATFILE_ISINTERACTIVE; }
	void SetInteractive(BOOL b = TRUE)
		{ m_byType = GetType () | (b ? CHATFILE_ISINTERACTIVE : 0); }
	void IncrementRetryCount()
		{ m_nRetry++; }
	UINT GetRetryCount()
		{ return m_nRetry; }

protected:
	LPTSTR m_pszName;
	BYTE   m_byType;
	UINT   m_nRetry;
};



// Some defines.

// Should not be specifying more than this many worker threads.
#define NUM_REASONABLE_MAX_THREADS		16
// Have not attempted to load WinInet yet.
#define WININET_NOT_YET_LOADED			((HINSTANCE)-1L)
// Size of Internet buffer.
#define REQUEST_DOWNLOAD_BUFSIZE		2048
// Thread shutdown wait time
#define REQUESTOR_THREAD_SHUTDOWN_WAIT	5000

// ============================================================================
// CLASS: CInternetRequestor
// The Internet Requestor class. 

class CInternetRequestor
{
public:
	CInternetRequestor(LPCTSTR pszAppName, UINT nMaxThreads = 2);
	~CInternetRequestor();

	BOOL IsAvailable();
	BOOL ShutDown();
	void SetMaxReasonableFileLimit(DWORD dwFileLimit)
		{ m_dwMaxReasonableFileLimit = dwFileLimit; }

	BOOL AddToWorkQueue(CInternetRequest* pRequest);
	BOOL IsOutBoxNonEmpty() 
		{ return m_pOutbox != NULL; }
	int GetOutBoxCount();
	CInternetRequest* PeekInOutBox(int nEntry = 0, BOOL bRemove = TRUE);

	void Lock()
		{  EnterCriticalSection (&m_critsec); }
	void UnLock()
		{  LeaveCriticalSection (&m_critsec); }

protected:
	struct ThreadInfo
	{
		HANDLE 				 hThread;
		CInternetRequestor * pRequestor;
		HANDLE				 hFile;
	};

	LPCTSTR   	m_pszAppName;
	UINT		m_nMaxThreads;
	ThreadInfo* m_pThreadInfo;
	CRITICAL_SECTION m_critsec;
	HANDLE		m_hWorkSemaphore;
	UINT		m_nThreadsAllocated;
	UINT		m_nThreadsBusy;
	BOOL		m_bShuttingDown;
	DWORD		m_dwMaxReasonableFileLimit;

	HINSTANCE	m_hWinInet;
	DWORD		m_dwInetVersion; 		// At least this version, could be more recent.
	HINTERNET 	m_hInternet;

	CInternetRequest* 	m_pWorkQueue;
	CInternetRequest* 	m_pOutbox;
	CInternetRequest* 	m_pBeingWorkedOn;

	void ThreadWorker(ThreadInfo* pThreadInfo);
	static UINT CALLBACK ThreadWorkerStatic(PVOID pThreadInfo);


	// Class that can beused to lock and unlock the requestor easily,
	// but takes up very little code (all inline) or memory (1 pointer).
	// Similar to MFC's CCriticalSection.
	class RequestorLock
	{
	public:
		RequestorLock(CInternetRequestor* pRequestor)
			{ m_pRequestor = pRequestor; pRequestor->Lock (); }
		~RequestorLock()
			{ m_pRequestor->UnLock (); }
	protected:
		CInternetRequestor* m_pRequestor;
	};

	// Needed WININET functions
    HINTERNET (WINAPI *InternetOpen)(LPCSTR, DWORD, LPCSTR, LPCSTR, DWORD);
	BOOL (WINAPI *InternetCloseHandle)(HINTERNET);
    HINTERNET (WINAPI *InternetOpenUrl)(HINTERNET, LPCSTR, LPCSTR, DWORD, DWORD, DWORD);
	BOOL (WINAPI *InternetReadFile)(HINTERNET, LPVOID, DWORD, LPDWORD);

};
															 

#endif //! __WEBREQ_H__EDB4D47A_A324_11D1_B7EC_00C04FA3426D__

