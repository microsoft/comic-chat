//--------------------------------------------------------------------------------------------
//
//	Copyright (c) Microsoft Corporation, 1996-97
//
//	Description:
//
//		Microsoft Internet ChatSockets.
//
//		Simple Critical Section wrapper
//
//	Authors:
//
//		Umesh Madan 
//
//--------------------------------------------------------------------------------------------

#ifndef __LOCK_H__
#define __LOCK_H__

//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include <windows.h>

//--------------------------------------------------------------------------------------------
//
// CLASSES
//
//--------------------------------------------------------------------------------------------
class CLock
{
//
// Interfaces
//
public:
		CLock(void)
			{
				::InitializeCriticalSection(&m_cs);
			}

		~CLock(void)
			{
				::DeleteCriticalSection(&m_cs);
			}

	inline	void Lock(void)
			{
				::EnterCriticalSection(&m_cs);
			}

	inline	void Unlock(void)
			{
				::LeaveCriticalSection(&m_cs);
			}
//
// Data
//
protected:
				CRITICAL_SECTION	m_cs;
};

#endif // __LOCK_H__

