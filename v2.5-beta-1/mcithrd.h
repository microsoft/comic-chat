#ifndef __MCITHRD_H__EDB4D47C_A324_11D1_B7EC_00C04FA3426D__
#define __MCITHRD_H__EDB4D47C_A324_11D1_B7EC_00C04FA3426D__

// ============================================================================
// A background-threaded MCI playback class, which allows you to implement
// asynchronous playback with minimal overhead to your main thread. 
// The primary use of this class would be in sndPlaySound-type functionality.
// The class supports both synchronous and asynchronous requests.
//
// Synchronous requests:
// 1) Call one of the request functions (Play, Stop). The function will
//    return an ID.
// 2) Call WaitForCompletion with the ID returned by the request function.
//    This will return only when the request function has completed.
// 
// Error reporting:
// In general, error codes for asynchronous calls are not implemented. For
// a synchronous call, error reporting is available, but only reliable
// if there is only one client thread. GetLastRequestStatus returns the
// result of the last request processed. If you only have one thread making
// requests, you can call WaitForCompletion after making a request, and then
// call GetLastRequestStatus, and you will be able to get the result of the
// request.
// 
// Before calling any functions, call the StartupThread function. This will
// ensure that the thread is available for use. It takes an argument
// pointing to a pointer to the thread itself - the pointer is cleaned up when
// the thread is destroyed. (The thread goes away when it hasn't played anything 
// or received any requests in a while.) The function also takes a pointer to a 
// CRITICAL_SECTION object that will be used to control access.
//
// Thread Destruction:
// The thread is designed to go away when idle for some time. To check for this,
// use the following technique.
//		1. Enter critical section passed to StartupThread.
//		2. Check pointer passed to StartupThread. If NULL, the thread has
//		   gone away, and you need to create another instance.
//		3. Call Lock to keep the thread from going away.
//		4. Leave the critical section.
//		5. Make requests.
//		6. Call Unlock.
// NOTE: When this thread is created, it is initially given a lock count of 1.
// Call UnLock after calling StartupThread to release it.

class CMciPlaybackThread;

class CMciPlaybackWnd : public CStatic
{
public:
	CMciPlaybackWnd() 
		{ m_pParentThread = NULL; }
	void SetParentThread (CMciPlaybackThread * pParentThread)
		{ m_pParentThread = pParentThread; }
protected:
	CMciPlaybackThread* m_pParentThread;
	afx_msg LRESULT OnMCINotify(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP();
};


class CMciPlaybackThread : public CWinThread
{
	DECLARE_DYNCREATE(CMciPlaybackThread)

public:
	CMciPlaybackThread();
	virtual ~CMciPlaybackThread();

	// Functions called on main thread.
	BOOL StartupThread(CMciPlaybackThread * * pPtrToSelf, LPCRITICAL_SECTION pCritSec);
	BOOL Cleanup();
	DWORD Play(UINT nDevice, LPCSTR pszElement, BOOL bLoop = FALSE);
	DWORD Stop();
	BOOL WaitForCompletion(DWORD dwRequestID, DWORD dwTimeout);
	BOOL GetLastRequestStatus();
	BOOL IsPlaying();
	void Lock();
	void UnLock();

	// Functions called on class's thread.
	BOOL OnPlayMsg(UINT nDevice, LPCSTR pszElement, BOOL bLoop);
	BOOL OnStopMsg();
	void OnMCINotify(UINT nMsg, MCIDEVICEID id);

protected:
	virtual BOOL InitInstance();
	virtual int Run();
	
	#if _MFC_VER < 0x420
	BOOL PostThreadMessage(UINT msg, WPARAM wParam = 0, LPARAM lParam = 0)
		{ return ::PostThreadMessage (m_nThreadID, msg, wParam, lParam); }
	#endif

	void FinishRequest(BOOL bStatus);

	CMciPlaybackThread** m_pPtrToSelf;
	LPCRITICAL_SECTION 	 m_pCritSec;
	DWORD				 m_dwLastAssignedRequestID;
	DWORD				 m_dwCurrentlyProcessedRequestID;
	BOOL				 m_bLastRequestStatus;
	CMciPlaybackWnd		 m_wnd;
	BOOL				 m_bReadyForRequests;
	int					 m_nIdleCount;
	BOOL				 m_bQuitting;
	MCIDEVICEID			 m_id;
	MCIDEVICEID			 m_idInternal;
	BOOL				 m_bPlaying;
	BOOL				 m_bLoop;
	int					 m_nLocks;
	int					 m_nPendingPlayback;

};

#define MCIM_PLAY WM_USER + 150
#define MCIM_STOP WM_USER + 151

#endif
