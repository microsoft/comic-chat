#include "stdafx.h"
#include "mcithrd.h"


// Private structure used for play messages
struct PLAYMSG
{
	UINT nDevice;
	BOOL bLoop;
	char szElement[1];
};

CMciPlaybackThread::CMciPlaybackThread()
{
	m_pPtrToSelf = NULL;
	m_pCritSec = NULL;
	m_dwLastAssignedRequestID = 0;
	m_dwCurrentlyProcessedRequestID = 0;
	m_bLastRequestStatus = FALSE;
	m_bReadyForRequests = FALSE;
	m_nIdleCount = 0;
	m_bQuitting = FALSE;
	m_idInternal = m_id = (MCIDEVICEID)-1;
	m_bPlaying = FALSE;
	m_nLocks = 1;
	m_nPendingPlayback = 0;
}

CMciPlaybackThread::~CMciPlaybackThread()
{
}

IMPLEMENT_DYNCREATE(CMciPlaybackThread, CWinThread);

BOOL
CMciPlaybackThread::StartupThread(
CMciPlaybackThread * * pPtrToSelf, 
LPCRITICAL_SECTION pCritSec)
{
	ASSERT(pPtrToSelf != NULL);
	ASSERT(pCritSec != NULL);
	m_pPtrToSelf = pPtrToSelf;
	m_pCritSec = pCritSec;
	ResumeThread ();
	while (*pPtrToSelf != NULL && !m_bReadyForRequests)
	{
		::Sleep (10);
	}
	return *pPtrToSelf != NULL;
}

// Cleans up and deletes the thread. Can be called on either the client thread
// or on the class's own thread.

BOOL
CMciPlaybackThread::Cleanup()
{
	BOOL bInThread = GetCurrentThreadId () == m_nThreadID;
	if (bInThread && m_id != (MCIDEVICEID)-1)
		return FALSE; 	// Can't clean up right now.

	// Make sure we are stopped.
	if (bInThread)
	{
		OnStopMsg ();
	}
	else
	{
		PostThreadMessage (MCIM_STOP);
		while (IsPlaying ())
		{
			::Sleep (10);
		}
	}

	::EnterCriticalSection (m_pCritSec);
	*m_pPtrToSelf = NULL;
	::LeaveCriticalSection (m_pCritSec);
	m_bQuitting = TRUE;
	PostThreadMessage (WM_QUIT, 0, 0);
	return TRUE;
}

// Play an element. Fails if something else is playing when this request is processed.
// Must be called on the client thread.

DWORD 
CMciPlaybackThread::Play(
UINT   nDevice,
LPCSTR pszElement,
BOOL   bLoop)
{
	if (m_bQuitting)
		return 0;

	ASSERT(GetCurrentThreadId () != m_nThreadID);
	PLAYMSG * pPlayMsg = (PLAYMSG *)malloc (sizeof(PLAYMSG) + lstrlen (pszElement));
	if (pPlayMsg == NULL)
		return 0;
	
	::EnterCriticalSection (m_pCritSec);
	m_nPendingPlayback++;
	::LeaveCriticalSection (m_pCritSec);

	pPlayMsg->nDevice = nDevice;
	pPlayMsg->bLoop = bLoop;
	lstrcpy (pPlayMsg->szElement, pszElement);
	PostThreadMessage (MCIM_PLAY, 0, (LPARAM)pPlayMsg);
	return ++m_dwLastAssignedRequestID;
}

// Play an element. Fails if something else is playing when this request is processed.
// Must be called on the client thread.

DWORD 
CMciPlaybackThread::Stop()
{
	if (m_bQuitting)
		return 0;

	::EnterCriticalSection (m_pCritSec);
	m_nPendingPlayback = 0;
	::LeaveCriticalSection (m_pCritSec);

	ASSERT(GetCurrentThreadId () != m_nThreadID);
	PostThreadMessage (MCIM_STOP);
	DWORD dwRet = ++m_dwLastAssignedRequestID;
	return dwRet;
}

// Waits for the completion of a particular request. The timeout setting is the same
// as WaitForSingleObject. Must be called on the client thread.

BOOL 
CMciPlaybackThread::WaitForCompletion(
DWORD dwRequestID,
DWORD dwTimeout)
{
	if (m_bQuitting)
		return TRUE;

	ASSERT(GetCurrentThreadId () != m_nThreadID);
	if (dwRequestID < m_dwLastAssignedRequestID)
		return FALSE;

	DWORD dwTime = timeGetTime ();
	DWORD dwLastRequest;
	while (TRUE)
	{
		::EnterCriticalSection (m_pCritSec);
		dwLastRequest = m_dwCurrentlyProcessedRequestID;
		::LeaveCriticalSection (m_pCritSec);
		if (dwLastRequest >= dwRequestID)
			return TRUE;
		if (dwTimeout != (DWORD)-1L && timeGetTime () - dwTime >= dwTimeout)
			break;
		::Sleep (10);
	} 
	return FALSE;
}

// Gets the status of the last completed request.

BOOL 
CMciPlaybackThread::GetLastRequestStatus()
{
	if (m_bQuitting)
		return FALSE;

	::EnterCriticalSection (m_pCritSec);
	BOOL bStatus = m_bLastRequestStatus;
	::LeaveCriticalSection (m_pCritSec);
	return bStatus;
}

// Checks if the thread is playing something, or has something to be played in the queue.

BOOL
CMciPlaybackThread::IsPlaying()
{
	::EnterCriticalSection (m_pCritSec);
	BOOL bRet = m_id != (MCIDEVICEID)-1 || m_nPendingPlayback > 0;
	::LeaveCriticalSection (m_pCritSec);
	return bRet;
}

// Lock/Unlock functions to prevent thread from self-termination.

void 
CMciPlaybackThread::Lock()
{
	::EnterCriticalSection (m_pCritSec);
	m_nLocks++;
	::LeaveCriticalSection (m_pCritSec);
}
	
void 
CMciPlaybackThread::UnLock()
{
	::EnterCriticalSection (m_pCritSec);
	m_nLocks--;
	::LeaveCriticalSection (m_pCritSec);
}

// CWinThread::InitInstance override

BOOL 
CMciPlaybackThread::InitInstance()
{
	// The thread must have been suspended, and StartupThread called.
	ASSERT(m_pPtrToSelf != NULL);

	// Create a window to process requests.
	m_wnd.SetParentThread (this);
	if (!m_wnd.CreateEx (0, "static", "", WS_POPUP, 0, 0, 0, 0, NULL, NULL))
	{
		return FALSE;
	}

	// Set a timer on the window.

	//m_wnd.SetTimer (1, 30000, NULL);

	m_pMainWnd = &m_wnd;
	m_bReadyForRequests = TRUE;
	m_nIdleCount = 0;
	return TRUE;
}

// CWinThread::Run override

int 
CMciPlaybackThread::Run()
{
	while (::GetMessage (&m_msgCur, NULL, 0, 0) > 0)
	{
		if (m_bQuitting)
		{
			break;
		}

		PreTranslateMessage (&m_msgCur);
		if (m_msgCur.message == MCIM_PLAY)
		{
			m_nIdleCount = 0;

			// If there are any other MCIM_PLAY or MCIM_STOP requests down the line,
			// ignore this one!
			MSG msg;
			if (::PeekMessage (&msg, NULL, MCIM_PLAY, MCIM_STOP, PM_NOREMOVE))
			{
				::EnterCriticalSection (m_pCritSec);
				if (m_nPendingPlayback > 0)
					m_nPendingPlayback--;
				::LeaveCriticalSection (m_pCritSec);
				FinishRequest (TRUE);
				free ((LPVOID)m_msgCur.lParam);
				continue;
			}
			PLAYMSG * pPlayMsg = (PLAYMSG*)m_msgCur.lParam;
			FinishRequest (OnPlayMsg (pPlayMsg->nDevice, pPlayMsg->szElement, pPlayMsg->bLoop));
			free (pPlayMsg);
		}
		else if (m_msgCur.message == MCIM_STOP)
		{
			m_nIdleCount = 0;

			// If there are any other MCIM_STOP requests down the line,
			// ignore this one!
			MSG msg;
			if (::PeekMessage (&msg, NULL, MCIM_STOP, MCIM_STOP, PM_NOREMOVE))
			{
				FinishRequest (TRUE);
				continue;
			}
			FinishRequest (OnStopMsg ());
		}
		else
			::DispatchMessage (&m_msgCur);
	}

	// Make sure the sound is closed.
	OnStopMsg ();

	return 0;
}

// Handles a play request.

BOOL
CMciPlaybackThread::OnPlayMsg(
UINT nDevice,
LPCSTR pszElement,
BOOL bLoop)
{
	if (m_id != (MCIDEVICEID)-1)
	{
		return FALSE;
	}

	m_bPlaying = FALSE;
	MCI_OPEN_PARMS openparms;
	openparms.dwCallback = (DWORD)m_wnd.m_hWnd;
	openparms.lpstrDeviceType = (LPSTR)nDevice;
	openparms.lpstrElementName = pszElement;
	openparms.lpstrAlias = NULL;
	m_bLoop = bLoop;
	BOOL bSuccess = (mciSendCommand (0, MCI_OPEN, MCI_OPEN_ELEMENT | MCI_NOTIFY, (DWORD)&openparms)) == 0;
	::EnterCriticalSection (m_pCritSec);
    if (m_nPendingPlayback > 0)
		m_nPendingPlayback--;
	if (bSuccess)
		m_idInternal = m_id = openparms.wDeviceID;
	::LeaveCriticalSection (m_pCritSec);
	return bSuccess;
}

// Handles a stop request.

BOOL
CMciPlaybackThread::OnStopMsg()
{
	if (m_idInternal == (MCIDEVICEID)-1)
		return TRUE;	// Redundant call.

	MCI_GENERIC_PARMS closeparms;
	closeparms.dwCallback = 0;
	m_idInternal = (MCIDEVICEID)-1;
	mciSendCommand (m_id, MCI_CLOSE, MCI_WAIT, (DWORD)&closeparms);

	// Drain the queue of any MCI messages.
	MSG msg;
	while (::PeekMessage (&msg, NULL, MM_MCINOTIFY, MM_MCINOTIFY, PM_REMOVE));
	::EnterCriticalSection (m_pCritSec);
	m_id = (MCIDEVICEID)-1;
	::LeaveCriticalSection (m_pCritSec);
	return TRUE;
}

void
CMciPlaybackThread::OnMCINotify(
UINT 		nMsg,
MCIDEVICEID id)
{
	if (m_idInternal == (MCIDEVICEID)-1)
		return;

	switch (nMsg)
	{
		case MCI_NOTIFY_SUCCESSFUL:
			if (!m_bPlaying || m_bLoop)
			{
				// Are there any stop requests pending? If so, don't play.
				MSG msg;
				if (m_bQuitting || ::PeekMessage (&msg, NULL, MCIM_STOP, MCIM_STOP, PM_NOREMOVE))
					return;
				m_bPlaying = TRUE;
				MCI_PLAY_PARMS playparms;
				playparms.dwCallback = (DWORD)m_wnd.m_hWnd;
				playparms.dwFrom = playparms.dwTo = 0;
				if (mciSendCommand (m_id, MCI_PLAY, MCI_NOTIFY, (DWORD)&playparms) != 0)
				{
					::EnterCriticalSection (m_pCritSec);
					m_bLastRequestStatus = FALSE;
					::LeaveCriticalSection (m_pCritSec);
					m_bPlaying = FALSE;
					OnStopMsg ();
				}
				break;
			}
			// FALLTHRU
		default:
			OnStopMsg ();
			break;
	}
}

// Called internally to finish a request.

void 
CMciPlaybackThread::FinishRequest(
BOOL bStatus)
{
	::EnterCriticalSection (m_pCritSec);
	m_dwCurrentlyProcessedRequestID++;
	m_bLastRequestStatus = bStatus;
	::LeaveCriticalSection (m_pCritSec);
}

BEGIN_MESSAGE_MAP(CMciPlaybackWnd, CStatic)
	ON_MESSAGE(MM_MCINOTIFY, OnMCINotify)
END_MESSAGE_MAP()

LRESULT
CMciPlaybackWnd::OnMCINotify(
WPARAM wParam, 
LPARAM lParam)
{
	m_pParentThread->OnMCINotify (wParam, lParam);
	return 0;
}

