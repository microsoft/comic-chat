/*
	*****************************************************************
	*																*
	*	Module 			: CComp.cpp  - Chat Comparison Code			*
	*																*
	*	Author 			: RegisB, 05/01/97							*
	*																*
	*	Current Owner	: RegisB, originally from KentCe			*
	*																*
	*****************************************************************
*/

#include "CDebug.H"
#include "CComp.H"

// for ASSERT and FAIL
//
SZTHISFILE

//+-------------------------------------------------------------------------------------------
// Is string of type [?]*
// 03/12/98 - on MS IRCX servers, ???...?* is equivalent to *
//--------------------------------------------------------------------------------------------
BOOL bMatchAll(LPTSTR szString, UINT cbLen)
{
	ASSERT(szString, "szString is NULL in bMatchAll");
	ASSERT(cbLen, "cbLen is 0 in bMatchAll");

	LPTSTR	szTmp = szString;

	while ((szTmp-szString < cbLen) && *szTmp == '?')
		szTmp = CharNext(szTmp);

	return (szTmp-szString == cbLen-1) && *szTmp == '*';
}


//+-------------------------------------------------------------------------------------------
//
//  Routine:    bGetUserMatchFromMask(szIdentMask, pPrUserMatch)
//
//  Synopsis:   Parse the nickname!username@ipaddress mask into it's components.
//
//  Arguments:  [szIdentMask]	-- Pointer to mask to parse.
//              [pPrUserMatch]	-- Pointer to user match data structure to initialize.
//
//--------------------------------------------------------------------------------------------

BOOL bGetUserMatchFromMask(LPTSTR szIdentMask, PPRUSERMATCH pPrUserMatch)
{
    UINT	iMask;
    UINT	iLast;

	ASSERT(szIdentMask, "!szIdentMask in bGetUserMatchFromMask");
	ASSERT(pPrUserMatch, "!pPrUserMatch in bGetUserMatchFromMask");

    //  Initialize the returned data user match structure to default.  
	ZeroMemory(pPrUserMatch, sizeof(PRUSERMATCH));

	//  Set the whole mask first
	pPrUserMatch->szTheMask = pPrUserMatch->szNickname = pPrUserMatch->szUserName = szIdentMask;

    //  Scan the mask for specific characters to determine how to proceed.
    for (iMask = 0, iLast = 0; szIdentMask[iMask] != '\0'; iMask++)
    {
		if (szIdentMask[iMask] == '!')
        {
            pPrUserMatch->szUserName = &szIdentMask[iMask + 1];
            pPrUserMatch->cbNickname = iMask - iLast;
            iLast = iMask + 1;
        }
        else 
			if (szIdentMask[iMask] == '@')
			{
				pPrUserMatch->szIPAddress = &szIdentMask[iMask + 1];
				pPrUserMatch->cbUserName = iMask - iLast;
				iLast = iMask + 1;
			}
    }

	if (pPrUserMatch->szIPAddress)
		pPrUserMatch->cbIPAddress = iMask - iLast;
	else
		if (pPrUserMatch->szUserName && (pPrUserMatch->szUserName != pPrUserMatch->szNickname))
            pPrUserMatch->cbUserName = iMask - iLast;
		else
		{
			ASSERT(pPrUserMatch->szNickname, "pPrUserMatch->szNickname is NULL in bGetUserMatchFromMask");
            pPrUserMatch->cbNickname = iMask - iLast;
		}

	//  If the nickname field is null or [?]* then null out the pointer to indicate any match
	if (pPrUserMatch->szNickname)
	{
		if (pPrUserMatch->cbNickname == 0 || bMatchAll(pPrUserMatch->szNickname, pPrUserMatch->cbNickname))
        {
			pPrUserMatch->szNickname = NULL;
			pPrUserMatch->cbNickname = 0;
        }
	}

	//  If the username field is null or [?]* then null out the pointer to indicate any match
	if (pPrUserMatch->szUserName)
	{
		if (pPrUserMatch->cbUserName == 0 || bMatchAll(pPrUserMatch->szUserName, pPrUserMatch->cbUserName))
        {
			pPrUserMatch->szUserName = NULL;
			pPrUserMatch->cbUserName = 0;
        }
	}

	//  If the IPAddress field is null or [?]* then null out the pointer to indicate any match
	if (pPrUserMatch->szIPAddress)
	{
		if (pPrUserMatch->cbIPAddress == 0 || bMatchAll(pPrUserMatch->szIPAddress, pPrUserMatch->cbIPAddress))
        {
			pPrUserMatch->szIPAddress = NULL;
			pPrUserMatch->cbIPAddress = 0;
        }
	}

    return TRUE;
}

//+-------------------------------------------------------------------------------------------
//
//  Routine:    bIsMaskCompare(LPCTSTR szMask, UINT cbMask, LPCTSTR szString, UINT cbString)
//
//  Synopsis:   Compares a string with a mask to determine a successful match.
//
//  Arguments:  [szMask]   -- Pointer to mask string to compare with.
//              [cbMask]   -- Number of characters in the mask string.
//              [szString] -- Pointer to string to compare.
//              [cbString] -- Number of characters in the string.
//
//  Returns:    True if compared successfully.
//
//  Algorithm:  Standard '*'/'?' mask compare with scanning from both ends of the string
//              concurrently.
//
//--------------------------------------------------------------------------------------------

BOOL bIsMaskCompare(LPCTSTR szMask, UINT cbMask, LPCTSTR szString, UINT cbString)
{
	UINT iMask, iLastMask;
	UINT iString, iLastChar;

	//  If the mask is empty, then assume any match.
	if (cbMask == 0)
		return TRUE;

	//  Reset the beginning of the mask/string indexes.
	iMask = 0;
	iString = 0;

    //  Reset the last mask seen index.
    iLastMask = (UINT) ~0;

	//  Compare the mask with the string.
	while (iMask != cbMask && iString != cbString)
	{
		//  If the first character of the mask is a constant then compare for it.
		if (szMask[iMask] != '*' && szMask[iMask] != '?')
		{
			if (CharUpper((LPTSTR) MAKELONG(szMask[iMask], 0)) != CharUpper((LPTSTR) MAKELONG(szString[iString], 0)))
            {
                if (iLastMask != (UINT) ~0)
                {
                    iMask = iLastMask;
                    iString = iLastChar + 1;
                    iLastChar = iString;
                    continue;
                }
                return FALSE;
            }

			iMask++;
			iString++;
			continue;
		}

		//  If the last character of the mask is a constant then compare for it.
		if (szMask[cbMask - 1] != '*' && szMask[cbMask - 1] != '?')
		{
			if (CharUpper((LPTSTR) MAKELONG(szMask[cbMask - 1], 0)) != CharUpper((LPTSTR) MAKELONG(szString[cbString - 1], 0)))
            {
                if (iLastMask != (UINT) ~0)
                {
                    iMask = iLastMask;
                    iString = iLastChar + 1;
                    iLastChar = iString;
                    continue;
                }
				return FALSE;
            }

			cbMask--;
			cbString--;
			continue;
		}

		//  If the first character of the remaining mask is a '?', match any character.
		if (szMask[iMask] == '?')
		{
			iMask++;
			iString++;
			continue;
		}

		//  If the last character of the remaining mask is a '?', match any character.
		if (szMask[cbMask - 1] == '?')
		{
			cbMask--;
			cbString--;
			continue;
		}

		//  The mask character is an '*', if only one character left then return successful.
		if (iMask + 1 == cbMask)
			return TRUE;

        //  If this is the first time we have seen this wild card then remember the location
        //  so we can backtrace on failure.
        if (iLastMask != iMask)
        {
            iLastMask = iMask;
            iLastChar = iString;
        }

		//  If the character after the '*' matches the first character in the substring, 
		//  then a match is found and can skip over the '*' mask character.
		if (CharUpper((LPTSTR) MAKELONG(szMask[iMask + 1], 0)) == CharUpper((LPTSTR) MAKELONG(szString[iString], 0)))
		{
			iMask += 2;
			iString++;
			continue;
		}

		//  No match to character following the '*', keep looking.
		iString++;
	}

	//  If the mask and string are both null, then a successful compare.
	if (iMask == cbMask && iString == cbString)
		return TRUE;

	//  If the mask contains a single '*' character and the string is empty then successful.
	if (iMask + 1 == cbMask && szMask[iMask] == '*' && iString == cbString)
		return TRUE;

	return FALSE;
}


//+-------------------------------------------------------------------------------------------
//
//  Method:  	bIsMatch(PPRUSERMATCH pPrUserMatch, LPCTSTR szNickname, LPCTSTR szUserName, LPCTSTR szIPAddress)
//
//  Synopsis:   Determines if the PRUSERMATCH data structure matches this user.
//
//  Arguments:  [pPrUserMatch] -- DataUserMatch data struction to perform compare with.
//              [szNickname]   -- User's nickname
//              [szUserName]   -- User's userid
//              [szIPAddress]  -- User's host
//
//  Returns:    True for a successful match, else false.
//
//	Notes:		BUGBUG: Take advantage of no mask character info to speed up the compare.
//
//--------------------------------------------------------------------------------------------
BOOL bIsMatch(PPRUSERMATCH pPrUserMatch, LPCTSTR szNickname, LPCTSTR szUserName, LPCTSTR szIPAddress)
{
    //  If the nickname is part of the mask then compare the nickname.
    if (pPrUserMatch->szNickname && pPrUserMatch->cbNickname)
	{
		ASSERT(pPrUserMatch->szNickname, "pPrUserMatch->szNickname is NULL in bIsMatch");
		ASSERT(szNickname, "szNickname is NULL in bIsMatch");
		UINT	cbNickTmp1 = pPrUserMatch->cbNickname;
		LPCTSTR szNickTmp1 = pPrUserMatch->szNickname;
		LPCTSTR szNickTmp2 = (pPrUserMatch->szNickname[0] != '\'' && szNickname[0] == '\'') ? szNickname+1 : szNickname;

		if (pPrUserMatch->szNickname[0] == '\'' && szNickname[0] != '\'')
		{
			szNickTmp1++;
			cbNickTmp1--;
		}

        if (!bIsMaskCompare(szNickTmp1, cbNickTmp1, szNickTmp2, lstrlen(szNickTmp2)))
            return FALSE;
	}

    //  If the username is part of the mask then compare the username.
    if (pPrUserMatch->szUserName && pPrUserMatch->cbUserName)
        if (!bIsMaskCompare(pPrUserMatch->szUserName, pPrUserMatch->cbUserName, szUserName, lstrlen(szUserName)))
            return FALSE;

    //  If the ipaddress is part of the mask then compare the ipaddress.
    if (pPrUserMatch->szIPAddress && pPrUserMatch->cbIPAddress)
        if (!bIsMaskCompare(pPrUserMatch->szIPAddress, pPrUserMatch->cbIPAddress, szIPAddress, lstrlen(szIPAddress)))
            return FALSE;

    return TRUE;
}
