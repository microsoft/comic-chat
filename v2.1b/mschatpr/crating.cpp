/*
	*****************************************************************
	*																*
	*	Module 			: CRating.cpp  - Chat Rating Code			*
	*																*
	*	Author 			: RegisB, 05/12/97							*
	*																*
	*	Current Owner	: RegisB									*
	*																*
	*****************************************************************
*/

#include "StdAfx.H"
#include "CRating.H"
#include "CDebug.H"

// for ASSERT and FAIL
//
SZTHISFILE

/////////////////////////////////////////////////////////////////////////////
// Callback function for RatingObtainQuery
void RatingObtainQueryCallback(DWORD dwUserData, HRESULT hr, LPCTSTR pszRating, LPVOID lpvRatingDetails)
{
	// Note that pszRating is ignored, we count on the ratings engine to have
	// called RatingCheckUserAccess for us and provide the HRESULT.
	// Should we free pszRating ????
	//
	CRatingQueryData *prqData = (CRatingQueryData *) dwUserData;

	if (prqData->m_hEvent)
	{
		prqData->m_hr = hr;
		prqData->m_pRatingDetails = lpvRatingDetails;
		SetEvent(prqData->m_hEvent);		// ?? Check Return Value
	}
	else
		RatingFreeDetails(lpvRatingDetails);
}


BOOL bRatingsEnabled(void)
{
	// Load the Ratings DLL (if possible)
	ENSURE_LOADED(g_hinstRatings, c_tszRatingsDLL);

	// If Rating DLL is not found assume ratings are not enabled.
	return ((NULL != g_hinstRatings) && (S_OK == RatingEnabledQuery()));
}


BOOL bCanViewUnrated(HWND hWnd, BOOL bPromptOverride)
{
	BOOL	bAccess = TRUE;
	LPVOID	pRatingDetails = NULL;

	// If ratings are not enabled, user can see anything
	if (!bRatingsEnabled())
		return TRUE;

	if (S_OK != RatingCheckUserAccess(NULL, NULL, NULL, NULL, 0, &pRatingDetails))
	{
		// User cannot see Unrated sites, see if he can get supervisor override
		if (!bPromptOverride || (S_OK != RatingAccessDeniedDialog(hWnd, NULL, NULL, pRatingDetails)))
			bAccess = FALSE;
	}

	if (pRatingDetails)
		RatingFreeDetails(pRatingDetails);

	return bAccess;
}


BOOL bPassesRatings(LPCTSTR szRating)
{
	ASSERT(szRating, "szRating is NULL in bPassesRatings");

	LPVOID	pRatingDetails = NULL;
	BOOL	bAccess = FALSE;

	if (S_OK == RatingCheckUserAccess(NULL, NULL, szRating, NULL, 0, &pRatingDetails))
		bAccess = TRUE;

	if (pRatingDetails) 
		RatingFreeDetails(pRatingDetails);

	return bAccess;
}
