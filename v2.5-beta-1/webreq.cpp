#include "stdafx.h"
#include "webreq.h"
#include "utils.h"

				 
// Turn this DEBUGMSG flag on to get thread debugging messages. Comment out
// the ifdef DEBUG to get retail mode thread debugging messages.
#ifdef DEBUG
//#define DEBUGMSG
#endif

#ifdef DEBUGMSG
#if defined(AFXWIN) && !defined(_CONSOLE)
// Can't use MFC TRACE macro...because it only works in debug mode, and
// we'd like the option of checking thread behavior in release mode too.
void WebReqDebugMsg(
LPCSTR pszFormat,
...)
{
	va_list args;
	char szDebugMsg[1024];

	va_start (args, pszFormat);
	wvsprintf (szDebugMsg, pszFormat, args);
	va_end (args);
	OutputDebugString (args);
}
#else
#define WebReqDebugMsg printf
#endif
#endif


// ============================================================================
// CInternetRequest implementation

// Default constructor. Can be used by derived classes who have manage
// the URL themselves.

CInternetRequest::CInternetRequest()
{
	m_pszURL = NULL;
	CommonConstruct ();		// Inline function for common initialization code.
}

// Explicit constructor. Makes a copy of the URL passed in. Use this constructor
// if you are using the base class directly rather than a derived class.

CInternetRequest::CInternetRequest(
LPCTSTR pszURL)
{
	ASSERT(pszURL != NULL);
	m_pszURL = strdup (pszURL);
	CommonConstruct ();		// Inline function for common initialization code.
}

// Destructor. IMPORTANT: If a derived class has allocated m_pszURL itself, 
// it should override the destructor and set m_pszURL to NULL. This will 
// prevent attempts to free the URL twice.

CInternetRequest::~CInternetRequest()
{
	free (m_pszURL);
	free (m_pszLocalFile);
	if (m_file) {
		fclose (m_file);
	}
}

// Delete method. The default simply calls the delete operator - derived 
// classes can override this to provide alternate functionality, such as 
// support for stack-created requests.

void
CInternetRequest::Delete()
{
	delete this;
}

// Adds the request to a queue. This function, and its companions below,
// RemoveFromList, DeleteList, GetListCount, GetListElem, together eliminate the 
// need to use MFC collections, saving a lot of overhead with minimal code 
// size increase.

void 
CInternetRequest::AddToListTail(
CInternetRequest** pRequestList)
{
	ASSERT(m_pNextInList == NULL);
	// Find the end of the list.
	while (*pRequestList != NULL) {
		pRequestList = &((*pRequestList)->m_pNextInList);
	}
	*pRequestList = this;
}

// Removes the request from the list. 

void
CInternetRequest::RemoveFromList(
CInternetRequest** pRequestList)
{
	// Find the request which links to this one.
	while (*pRequestList != NULL && *pRequestList != this) {
		pRequestList = &((*pRequestList)->m_pNextInList);
	}
	ASSERT (*pRequestList != NULL);
	*pRequestList = m_pNextInList;
	m_pNextInList = NULL;
}

// Removes the entire queue, deleting each element. Note that because this is
// called on a list version of a request, the "this" pointer can be NULL.

void
CInternetRequest::DeleteList()
{
	CInternetRequest* pThis = this;
	CInternetRequest* pNext;
	while (pThis) {
		pNext = pThis->m_pNextInList;
		pThis->Delete ();
		pThis = pNext;
	}
}

// Returns the number of requests in the queue. Note that because this is
// called on a list version of a request, the "this" pointer can be NULL.

int
CInternetRequest::GetListCount()
{
	int nCount = 0;
	CInternetRequest* pThis = this;
	while (pThis != NULL) {
		nCount++;
		pThis = pThis->m_pNextInList;
	}
	return nCount;
}

// Gets the nth request in the queue. Note that because this is
// called on a list version of a request, the "this" pointer can be NULL.

CInternetRequest*
CInternetRequest::GetListElem(
int nElem)
{
	ASSERT(nElem >= 0);
	CInternetRequest* pThis = this;
	while (pThis != NULL) {
		if (nElem-- == 0) {
			break;
		}
		pThis = pThis->m_pNextInList;
	}
	return pThis;
}

// Default implementation of OnBeginDownload. Creates a tempfile and opens it.

BOOL 
CInternetRequest::OnBeginDownload()
{
	char szTempDir[_MAX_PATH];
	char szTempPath[_MAX_PATH];
	GetTempPath (sizeof(szTempDir), szTempDir);
	if (GetTempFileName (szTempDir, "WRQ", 0, szTempPath) == 0) {
		return FALSE;
	}

	free (m_pszLocalFile);
	m_pszLocalFile = strdup (szTempPath);
	if (m_pszLocalFile == NULL) {
		return FALSE;
	}

	m_file = fopen (m_pszLocalFile, "wb");
	return m_file != NULL;
}

// Default implementation of OnAddDownloadedData. Appends the data to the file.

BOOL 
CInternetRequest::OnAddDownloadedData(
PBYTE pbData, 
UINT  nSize)
{
	ASSERT(pbData);
	ASSERT(m_file != NULL);
	return fwrite (pbData, 1, nSize, m_file) == nSize;
}

// Default implementation of OnEndDownload. Closes the file.

BOOL
CInternetRequest::OnEndDownload()
{
	ASSERT(m_file != NULL);
	fclose (m_file);
	m_file = NULL;
	return TRUE;
}

// Default implementation of OnAbortDownload. Closes and deletes the file.

void
CInternetRequest::OnAbortDownload()
{
	ASSERT(m_file != NULL);
	fclose (m_file);
	DeleteFile (m_pszLocalFile);
	free (m_pszLocalFile);
	m_pszLocalFile = NULL;
}

// ============================================================================
// CChatFileRequest implementation

// Constructor.

CChatFileRequest::CChatFileRequest(
LPCTSTR pszName, 
LPCTSTR pszURL, 
BYTE 	byType,
BOOL    bIsInteractive)
:
CInternetRequest (pszURL)
{
	m_pszName 	= strdup (pszName);
	m_byType  	= byType | (bIsInteractive ? CHATFILE_ISINTERACTIVE : 0);
	m_nRetry    = 0;
}

CChatFileRequest::~CChatFileRequest()
{
	free (m_pszName);
}

// ============================================================================
// CInternetRequestor implementation

// Constructor.
// A requestor is constructed with an app name and a maximum number of threads.
// The app name is used as the "User client" in HTTP requests - it is stored
// only as a pointer, so it should be statically stored by the caller. The number
// of threads determines how many objects can be downloading at once. A thread
// is not created until it is needed; once a thread is created, it is not 
// destroyed until the requestor is shut down.

CInternetRequestor::CInternetRequestor(
LPCTSTR pszAppName,
UINT nMaxThreads)
{
	ASSERT(nMaxThreads > 0 && nMaxThreads <= NUM_REASONABLE_MAX_THREADS);
	
	m_pszAppName = pszAppName;
	m_nMaxThreads = nMaxThreads;
	m_nThreadsBusy = m_nThreadsAllocated = 0;
	m_bShuttingDown = FALSE;
	m_dwMaxReasonableFileLimit = 0;

	// Allocate per-thread information. Note that calloc already zeroes out the
	// memory it allocates.

	m_pThreadInfo = (ThreadInfo *)calloc (nMaxThreads, sizeof(ThreadInfo));

	// Initialize the critical section used for synchronization.
	::InitializeCriticalSection (&m_critsec);

	// Create a semaphore used to tell work threads that work is available.
	m_hWorkSemaphore = ::CreateSemaphore (NULL, 0, 65535, NULL);

	// Don't try to load the WinInet DLL until it is actually needed.
	m_hWinInet = WININET_NOT_YET_LOADED;
	m_hInternet = NULL;

	m_pWorkQueue = m_pOutbox = m_pBeingWorkedOn = NULL;
}

// Destructor.
// The destructor for this class should never be called before calling ShutDown.
// ShutDown is what does proper cleanup - if this destructor is somehow called
// before calling ShutDown, the requestor will not properly clean itself up.
// To keep any worker thread from crashing, it will destroy the worker thread
// with TerminateThread, which is a very unstable function. Hence, deleting
// a requestor without calling ShutDown is HIGHLY discouraged, except under
// extreme cases.

CInternetRequestor::~CInternetRequestor()
{
	// This checks whether we were cleanly shut down or not. ASSERTing at this line
	// except on an extreme-case program shutdown is very bad! See the 
	// comments above!
	ASSERT(m_hInternet == NULL);

	// Terminate any remaining threads with extreme prejudice.

	if (m_pThreadInfo != NULL) {
		for (int i = m_nMaxThreads - 1; i >= 0; i--) {
			if (m_pThreadInfo[i].hThread != NULL) {
				::TerminateThread (m_pThreadInfo[i].hThread, 0);
			}
		}
	}

	// Clean up the critical section used to synchronize things.
	DeleteCriticalSection (&m_critsec);

	// Free the work semaphore.
	if (m_hWorkSemaphore != NULL) {
		::CloseHandle (m_hWorkSemaphore);
	}

	// Free the thread info.
	free (m_pThreadInfo);

	// Free all requests that we still have ownership of.
	m_pWorkQueue->DeleteList ();
	m_pOutbox->DeleteList ();
	m_pBeingWorkedOn->DeleteList ();
}

// Checks if Internet Requestor capability is available. If needed, this is 
// where it loads WinInet.

#define LOADPROCADDR(handle, fn) (*(PVOID *)&fn = (PVOID)::GetProcAddress ((handle), #fn))
#ifdef UNICODE
#define LOADPROCADDRT(handle, fn) (*(PVOID *)&fn = (PVOID)::GetProcAddress ((handle), #fn "W"))
#else
#define LOADPROCADDRT(handle, fn) (*(PVOID *)&fn = (PVOID)::GetProcAddress ((handle), #fn "A"))
#endif

BOOL
CInternetRequestor::IsAvailable()
{
	// This is the quick check for multiple calls to this function.
	if (m_hInternet != NULL) {
		return TRUE;
	}

	// All this stuff is only done on the first attempt. If the first
	// attempt fails, m_hWinInet gets set to NULL, and future calls to
	// this function also fail.
	
	if (m_hWinInet == WININET_NOT_YET_LOADED) {
		
		// Checks to see if everything was allocated OK.
		if (m_pThreadInfo == NULL || m_hWorkSemaphore == NULL) {
			m_hWinInet = NULL;
			return FALSE;
		}

		// Load WinInet.
		m_hWinInet = ::LoadLibrary ("WININET.DLL");
		if (m_hWinInet != NULL) {
			
			// Look up all needed functions.
			if (LOADPROCADDRT (m_hWinInet, InternetOpen) != NULL &&
					LOADPROCADDR (m_hWinInet, InternetCloseHandle) != NULL &&
					LOADPROCADDRT (m_hWinInet, InternetOpenUrl) != NULL &&
					LOADPROCADDR (m_hWinInet, InternetReadFile) != NULL) {

				// Check for other functions to see what version we are running.
				m_dwInetVersion = 3;
				if (::GetProcAddress (m_hWinInet, "IsHostInProxyBypassList") != NULL) {
					m_dwInetVersion = 4;
				}

				// Try to initialize the library.

				m_hInternet = InternetOpen (m_pszAppName, INTERNET_OPEN_TYPE_PRECONFIG,
									NULL, NULL, 0);
				if (m_hInternet != NULL) {
					// Success at last!
					return TRUE;
				}
			}

			// Error.
			::FreeLibrary (m_hWinInet);
			m_hWinInet = NULL;		// So we don't keep trying to reload.
		}
	}

	return FALSE;
}

// Shut down the requestor. If this function returns FALSE, the caller can
// call ShutDown again. Only a TRUE return value indicates that an absolutely
// harmless destruction of the requestor is possible.

BOOL
CInternetRequestor::ShutDown()
{
	// If we haven't done anything yet, the shutdown is really easy.
	if (m_hInternet == NULL && m_nThreadsAllocated == 0) {
		return FALSE;
	}

	// Signal everyone that we're shutting down.
	m_bShuttingDown = TRUE;

	// Make sure all threads are awake, by raising the semaphore by the max number.
	::ReleaseSemaphore (m_hWorkSemaphore, m_nMaxThreads, NULL);

	// Go through each thread, shutting down any handles that are open. The
	// WinInet docs say that if one thread is blocking on a call to WinInet
	// with a given handle, and another thread closes that handle, the 
	// blocking thread will unblock. So, this is the method we use to speed
	// up thread termination.

	int iThread;
	for (iThread = m_nMaxThreads - 1; iThread >= 0; iThread--) {
		Lock ();
		if (m_pThreadInfo[iThread].hFile) {
			InternetCloseHandle (m_pThreadInfo[iThread].hFile);
			m_pThreadInfo[iThread].hFile = NULL;
		}
		UnLock ();
	}

	// Close our internet handle. This should (according to docs) unblock 
	// threads who are blocking on InternetOpenURL. Note that on multiple
	// calls to ShutDown, the handle has already been closed after the first
	// call.

	if (m_hInternet != NULL) {
		InternetCloseHandle (m_hInternet);
		m_hInternet = NULL;
	}

	// Go through each thread, waiting for the thread itself to shut down. We don't
	// wait on all threads simultaneously - rather we wait on each thread for a 
	// certain time. Since all threads have commenced shutdown, later iterations
	// of this loop should have their wait call return immediately.

	BOOL bAnyFailedToExit = FALSE;
	
	for (iThread = m_nMaxThreads - 1; iThread >= 0; iThread--) {
		if (m_pThreadInfo[iThread].hThread != NULL) {
			if (::WaitForSingleObject (m_pThreadInfo[iThread].hThread, 
						REQUESTOR_THREAD_SHUTDOWN_WAIT) == WAIT_OBJECT_0) {
				// Close the handle and set it to NULL, so that the destructor
				// does not try and blow the thread away.
				ASSERT(m_pThreadInfo[iThread].hThread == NULL);
			} 
			else {
				bAnyFailedToExit = TRUE;
			}
		}
	}

	// If and only if all threads have shut down, do we unload WinInet.

	if (!bAnyFailedToExit) {
		::FreeLibrary (m_hWinInet);
		m_hWinInet = NULL;
	}

	return !bAnyFailedToExit;

}

// Adds a request to the work queue.

BOOL 
CInternetRequestor::AddToWorkQueue(
CInternetRequest* pRequest)
{
	ASSERT (pRequest != NULL);

	// Check if requestor is in working order.
	if (!IsAvailable ()) {
		return FALSE;
	}

	RequestorLock lock (this);
	
	// Add the entry.
	pRequest->AddToListTail (&m_pWorkQueue);
	pRequest->SetStatus (CInternetRequest::statusPending);

	if (m_nThreadsBusy + m_pWorkQueue->GetListCount () >= m_nThreadsAllocated) {
		
		// All allocated threads are busy. If we haven't run into the maximum,
		// we need to allocate another thread. If we have run into the maximum,
		// we just have to add the entry blindly, and wait till somebody
		// becomes free to use it.
		
		if (m_nThreadsAllocated < m_nMaxThreads) {

			ThreadInfo * pThreadInfo = &m_pThreadInfo[m_nThreadsAllocated];

			// Create the thread suspended, set up everything, and let the thread go.

			pThreadInfo->pRequestor = this;
			UINT nAddr;
			pThreadInfo->hThread = (HANDLE)_beginthreadex (NULL, 0, ThreadWorkerStatic, 
											pThreadInfo, CREATE_SUSPENDED, &nAddr);
			if (pThreadInfo->hThread == NULL) {
				return FALSE;
			}
			m_nThreadsAllocated++;
			::ResumeThread (pThreadInfo->hThread);
		}
		else {
			::ReleaseSemaphore (m_hWorkSemaphore, 1, NULL);
		}
	
	} 
	else {

		// A thread is available to process this request. Increment the semaphore
		// so that it wakes up and does the processing.
		::ReleaseSemaphore (m_hWorkSemaphore, 1, NULL);
	}
	return TRUE;
}

// Checks how many requests are waiting in the outbox.

int 
CInternetRequestor::GetOutBoxCount()
{
	RequestorLock lock (this);
	return m_pOutbox->GetListCount ();
}

// Looks at an entry from the outbox, optionally removing it from the outbox permanently.
// This optional behavior is roughly similar to the PM_REMOVE flag of the Windows
// PeekMessage call.

CInternetRequest* 
CInternetRequestor::PeekInOutBox(
int	 nEntry,
BOOL bRemove)
{
	RequestorLock lock (this);
	ASSERT (nEntry >= 0);
	CInternetRequest* pRequest = m_pOutbox->GetListElem (nEntry);
	if (pRequest != NULL && bRemove) {
		pRequest->SetStatus (CInternetRequest::statusFree);
		pRequest->RemoveFromList (&m_pOutbox);
	}
	return pRequest;
}

// The static function called by AfxBeginThread. Simply calls the non-static version.

UINT
CInternetRequestor::ThreadWorkerStatic(
PVOID pThreadInfo)
{
	ASSERT(pThreadInfo != NULL);
	ASSERT(((ThreadInfo *)pThreadInfo)->pRequestor != NULL);

	::SetThreadPriority (::GetCurrentThread (), THREAD_PRIORITY_IDLE);
	((ThreadInfo *)pThreadInfo)->pRequestor->ThreadWorker ((ThreadInfo *)pThreadInfo);
	return 0;
}

// Thread worker function. This is where all the work takes place. This function
// loops endlessly till told to quit. The class sets the m_bShuttingDown variable
// at shutdown - this function periodically checks it to see if we should quit,
// using the CHECK_EXIT_CASES macro.

#define CHECK_EXIT_CASES() \
	if (m_bShuttingDown) goto $ShutDown; \
	if (pRequest->GetStatus () == CInternetRequest::statusAbortPending) goto $RequestAborted

void
CInternetRequestor::ThreadWorker(
CInternetRequestor::ThreadInfo* pThreadInfo)
{
	CInternetRequest * pRequest;
	BOOL bStartedDownload;
	BYTE byDownloadBuffer[REQUEST_DOWNLOAD_BUFSIZE];
	DWORD dwTotalBytesRead;
	DWORD dwBytesRead;
	DWORD dwStatus;

   #ifdef DEBUGMSG
	WebReqDebugMsg ("Thread %lu started\n", GetCurrentThreadId ());
   #endif
	while (TRUE) {

		pRequest = NULL;
		pThreadInfo->hFile = NULL;
		bStartedDownload = FALSE;
		dwStatus = 0;

		while (!m_bShuttingDown) {

			// Always check first to see if there is an entry. The first time
			// the requestor creates this thread, it does NOT increment the semaphore.
			// This avoids needless calls to WaitForSingleObject.
			Lock ();
			if (m_pWorkQueue != NULL) {
				pRequest = m_pWorkQueue;
				pRequest->RemoveFromList (&m_pWorkQueue);
				pRequest->AddToListTail (&m_pBeingWorkedOn);
			}
			m_nThreadsBusy++;
			UnLock ();		// Important to unlock before sleeping!

			if (pRequest != NULL) {
				break;
			}
			else {
			   #ifdef DEBUGMSG
				WebReqDebugMsg ("Thread %lu blocking\n", GetCurrentThreadId ());
			   #endif
				::WaitForSingleObject (m_hWorkSemaphore, INFINITE);
			}
		};

		CHECK_EXIT_CASES();
	   #ifdef DEBUGMSG
		WebReqDebugMsg ("Thread %lu takes URL %s\n", GetCurrentThreadId (), pRequest->GetURL ());
	   #endif

		// OK, so we now have a request. Start downloading it. On any error,
		// or if the status of the request changes to AbortPending, then quit,
		// and put the item in the outbox.

		::SetThreadPriority (::GetCurrentThread (), THREAD_PRIORITY_NORMAL);
		pRequest->SetStatus (CInternetRequest::statusDownloading);

		DWORD dwFlags;
		if (m_dwInetVersion == 3)
			dwFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_COOKIES;
		else
			dwFlags = INTERNET_FLAG_KEEP_CONNECTION | INTERNET_FLAG_NO_CACHE_WRITE |
							INTERNET_FLAG_NO_UI | INTERNET_FLAG_NO_COOKIES;
		pThreadInfo->hFile = InternetOpenUrl (m_hInternet, pRequest->GetURL (), 
					"Accept: */*", -1L, dwFlags, 0);
		if (pThreadInfo->hFile == NULL) {
			#ifdef _DEBUG
			DWORD dwError = GetLastError ();
			#endif
			goto $RequestError;
		}

		CHECK_EXIT_CASES();

		// Tell the request that we are starting to download.
		if (!pRequest->OnBeginDownload ()) {
			goto $RequestError;
		}
		bStartedDownload = TRUE;

		CHECK_EXIT_CASES();
		
		// Loop, getting some data from the Internet and adding it to the request.

		dwTotalBytesRead = 0;
		do {

			if (!InternetReadFile (pThreadInfo->hFile, 
						byDownloadBuffer, sizeof(byDownloadBuffer), 
						&dwBytesRead)) {
				goto $RequestError;
			}

			CHECK_EXIT_CASES();
	
			if (dwBytesRead > 0 && !pRequest->OnAddDownloadedData (byDownloadBuffer, dwBytesRead)) {
				goto $RequestError;
			}

			// Check to see if caller has specified a maximum reasonable
			// size for a file.
			dwTotalBytesRead += dwBytesRead;
			if (m_dwMaxReasonableFileLimit != 0 && dwTotalBytesRead > m_dwMaxReasonableFileLimit) {
				goto $RequestError;
			}

			CHECK_EXIT_CASES();
		} while (dwBytesRead != 0);

		if (!pRequest->OnEndDownload ()) {
			goto $RequestError;
		}
		bStartedDownload = FALSE;

		// Succeeded. Set the status, then fall through all the cleanup code.
		dwStatus = CInternetRequest::statusCompleted;
	 
	   $RequestError:
		if (dwStatus == 0) {
			dwStatus = CInternetRequest::statusFailed;
		}

	   $ShutDown:
	   $RequestAborted:
		if (dwStatus == 0) {
			dwStatus = CInternetRequest::statusAborted;
		}

		if (pRequest != NULL) {
			if (bStartedDownload) {
				pRequest->OnAbortDownload ();
			}
			Lock ();
			pRequest->RemoveFromList (&m_pBeingWorkedOn);
			pRequest->AddToListTail (&m_pOutbox);
			pRequest->SetStatus (dwStatus);
			m_nThreadsBusy--;
			UnLock ();
		}

		// We need to lock the requestor when closing an hFile, because
		// the Requestor's ShutDown method will lock the requestor and close
		// all open handles to speed up thread exit.
		Lock ();
		if (pThreadInfo->hFile != NULL) {
			InternetCloseHandle (pThreadInfo->hFile);
			pThreadInfo->hFile = NULL;
		}
		UnLock ();

	   #ifdef DEBUGMSG
		if (pRequest != NULL) {
			WebReqDebugMsg ("Thread %lu finishes URL %s\n", GetCurrentThreadId (), pRequest->GetURL ());
		}
	   #endif

		if (m_bShuttingDown) {
			break;
		}

		::SetThreadPriority (::GetCurrentThread (), THREAD_PRIORITY_IDLE);
	}

   #ifdef DEBUGMSG
	WebReqDebugMsg ("Thread %lu closing down\n", GetCurrentThreadId ());
   #endif
   
	// We close the handle and set it to NULL ourselves, to give either
	// ourselves or the parent thread the option of shutting us down.
	::CloseHandle (pThreadInfo->hThread);
	pThreadInfo->hThread = NULL;
	_endthreadex (0);
}
