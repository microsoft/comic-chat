#ifndef __CRATING_H__
#define __CRATING_H__

#include "Windows.H"
#include "Ratings.H"
#include "DlyLdDll.H"

// Class object for callback - used in rating checking
class CRatingQueryData
{
public:
	CRatingQueryData() 
	{ 
		m_hEvent = CreateEvent(NULL,FALSE,FALSE,NULL); 
		m_pRatingDetails = NULL;
		m_hr = NULL;
	}

	~CRatingQueryData()
	{ 
		if (NULL != m_hEvent) 
			CloseHandle(m_hEvent); 
		if (NULL != m_pRatingDetails)
			RatingFreeDetails(m_pRatingDetails);
	}

	HANDLE	m_hEvent;
	HRESULT m_hr;
	LPVOID	m_pRatingDetails;
};


// External common functions
extern void		RatingObtainQueryCallback(DWORD dwUserData, HRESULT hr, LPCTSTR pszRating, LPVOID lpvRatingDetails);
extern BOOL		bRatingsEnabled(void);
extern BOOL		bCanViewUnrated(HWND hWnd, BOOL bPromptOverride);
extern BOOL		bPassesRatings(LPCTSTR szRating);

#endif //__CRATING_H__
