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

#include "StdAfx.H"
#include "CDebug.H"
#include "CComp.H"

// for ASSERT and FAIL
//
SZTHISFILE

/* Original code
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
    BOOL	bNickname	= FALSE;
    LPTSTR	szUserName	= NULL;
    BOOL	bUserName	= FALSE;
    LPTSTR	szIPAddress	= NULL;
    BOOL	bIPAddress	= FALSE;
    BOOL	bPeriod		= FALSE;
    BOOL	bFoundWildCard;
    UINT	iMask;
    UINT	iLast;

	ASSERT(szIdentMask, "!szIdentMask in bGetUserMatchFromMask");
	ASSERT(pPrUserMatch, "!pPrUserMatch in bGetUserMatchFromMask");

    //  Initialize the returned data user match structure to default.  
	ZeroMemory(pPrUserMatch, sizeof(PRUSERMATCH));
    pPrUserMatch->bAll = TRUE;

	//  Set the whole mask first
	pPrUserMatch->szTheMask = szIdentMask;	// We use this field later to free the memory

    //  Scan the mask for specific characters to determine how to proceed.
    bFoundWildCard = FALSE;
    for (iMask = 0, iLast = 0; szIdentMask[iMask] != '\0'; iMask++)
    {
        if (szIdentMask[iMask] == '*' || szIdentMask[iMask] == '?')
            bFoundWildCard = TRUE;
        else if (szIdentMask[iMask] == '!')
        {
            szUserName = &szIdentMask[iMask + 1];
            pPrUserMatch->cbUserName = iMask - iLast;
            iLast = iMask + 1;
            bNickname = bFoundWildCard;
            bFoundWildCard = FALSE;
        }
        else if (szIdentMask[iMask] == '@')
        {
            szIPAddress = &szIdentMask[iMask + 1];
            pPrUserMatch->cbIPAddress = iMask - iLast;
            iLast = iMask + 1;
            bUserName = bFoundWildCard;
            bFoundWildCard = FALSE;
        }
        else if (szIdentMask[iMask] == '.')
            bPeriod = TRUE;
    }
	iMask--;

    //  Set the bFoundWildCard to the last field in the mask.
    if (szIPAddress || bPeriod)
        bIPAddress = bFoundWildCard;
    else if (szUserName)
        bUserName = bFoundWildCard;
    else
        bNickname = bFoundWildCard;

    //  Parse out the nickname from the mask.
    if (szUserName || (szIPAddress == NULL && !bPeriod))
    {
        pPrUserMatch->szNickname = szIdentMask; 
        pPrUserMatch->cbNickname = iMask - iLast;
        pPrUserMatch->bNickname  = bNickname;
    }

    //  Parse out the username from the mask.
    if (szUserName)
    {
        pPrUserMatch->szUserName = szUserName;
        pPrUserMatch->bUserName  = bUserName;
    }
    else if (szIPAddress)
    {
        pPrUserMatch->szUserName = szIdentMask;
        pPrUserMatch->cbUserName = iMask - iLast;
        pPrUserMatch->bUserName  = bUserName;
    }

    //  Parse out the IPAddress from the mask.
    if (szIPAddress)
    {
        pPrUserMatch->szIPAddress = szIPAddress;
        pPrUserMatch->bIPAddress = bIPAddress;
    }
    else if (bPeriod && szUserName == NULL)
    {
        pPrUserMatch->szIPAddress = szIdentMask;
        pPrUserMatch->cbIPAddress = iMask - iLast;
        pPrUserMatch->bIPAddress  = bIPAddress;
    }

	//  If the nickname field is null or '*' then null out the pointer to indicate any match
	if (pPrUserMatch->szNickname)
	{
		if (pPrUserMatch->cbNickname == 0 || (pPrUserMatch->szNickname[0] == '*' && pPrUserMatch->cbNickname == 1))
        {
			pPrUserMatch->szNickname = NULL;
			pPrUserMatch->cbNickname = 0;
        }
        else
            pPrUserMatch->bAll = FALSE;
	}

	//  If the username field is null or '*' then null out the pointer to indicate any match
	if (pPrUserMatch->szUserName)
	{
		if (pPrUserMatch->cbUserName == 0 || (pPrUserMatch->szUserName[0] == '*' && pPrUserMatch->cbUserName == 1))
        {
			pPrUserMatch->szUserName = NULL;
			pPrUserMatch->cbUserName = 0;
        }
        else
            pPrUserMatch->bAll = FALSE;
	}

	//  If the IPAddress field is null or '*' then null out the pointer to indicate any match
	if (pPrUserMatch->szIPAddress)
	{
		if (pPrUserMatch->cbIPAddress == 0 || (pPrUserMatch->szIPAddress[0] == '*' && pPrUserMatch->cbIPAddress == 1))
        {
			pPrUserMatch->szIPAddress = NULL;
			pPrUserMatch->cbIPAddress = 0;
        }
        else
            pPrUserMatch->bAll = FALSE;
	}

    return TRUE;
}
*/

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
    BOOL	bFoundWildCard;
    UINT	iMask;
    UINT	iLast;

	ASSERT(szIdentMask, "!szIdentMask in bGetUserMatchFromMask");
	ASSERT(pPrUserMatch, "!pPrUserMatch in bGetUserMatchFromMask");

    //  Initialize the returned data user match structure to default.  
	ZeroMemory(pPrUserMatch, sizeof(PRUSERMATCH));
    pPrUserMatch->bAll = TRUE;

	//  Set the whole mask first
	pPrUserMatch->szTheMask = pPrUserMatch->szNickname = szIdentMask;

    //  Scan the mask for specific characters to determine how to proceed.
    bFoundWildCard = FALSE;
    for (iMask = 0, iLast = 0; szIdentMask[iMask] != '\0'; iMask++)
    {
        if (szIdentMask[iMask] == '*' || szIdentMask[iMask] == '?')
            bFoundWildCard = TRUE;
        else if (szIdentMask[iMask] == '!')
        {
            pPrUserMatch->szUserName = &szIdentMask[iMask + 1];
            pPrUserMatch->cbNickname = iMask - iLast;
            iLast = iMask + 1;
            pPrUserMatch->bNickname = bFoundWildCard;
            bFoundWildCard = FALSE;
        }
        else if (szIdentMask[iMask] == '@')
        {
            pPrUserMatch->szIPAddress = &szIdentMask[iMask + 1];
            pPrUserMatch->cbUserName = iMask - iLast;
            iLast = iMask + 1;
            pPrUserMatch->bUserName = bFoundWildCard;
            bFoundWildCard = FALSE;
        }
    }

    //  Set the bFoundWildCard to the last field in the mask.
    pPrUserMatch->bIPAddress = bFoundWildCard;
    pPrUserMatch->cbIPAddress = iMask - iLast;

	//  If the nickname field is null or '*' then null out the pointer to indicate any match
	if (pPrUserMatch->szNickname)
	{
		if (pPrUserMatch->cbNickname == 0 || (pPrUserMatch->szNickname[0] == '*' && pPrUserMatch->cbNickname == 1))
        {
			pPrUserMatch->szNickname = NULL;
			pPrUserMatch->cbNickname = 0;
        }
        else
            pPrUserMatch->bAll = FALSE;
	}

	//  If the username field is null or '*' then null out the pointer to indicate any match
	if (pPrUserMatch->szUserName)
	{
		if (pPrUserMatch->cbUserName == 0 || (pPrUserMatch->szUserName[0] == '*' && pPrUserMatch->cbUserName == 1))
        {
			pPrUserMatch->szUserName = NULL;
			pPrUserMatch->cbUserName = 0;
        }
        else
            pPrUserMatch->bAll = FALSE;
	}

	//  If the IPAddress field is null or '*' then null out the pointer to indicate any match
	if (pPrUserMatch->szIPAddress)
	{
		if (pPrUserMatch->cbIPAddress == 0 || (pPrUserMatch->szIPAddress[0] == '*' && pPrUserMatch->cbIPAddress == 1))
        {
			pPrUserMatch->szIPAddress = NULL;
			pPrUserMatch->cbIPAddress = 0;
        }
        else
            pPrUserMatch->bAll = FALSE;
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
			if (CharUpper((LPTSTR) szMask[iMask]) != CharUpper((LPTSTR) szString[iString]))
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
			if (CharUpper((LPTSTR) szMask[cbMask - 1]) != CharUpper((LPTSTR) szString[cbString - 1]))
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
		if (CharUpper((LPTSTR) szMask[iMask + 1]) == CharUpper((LPTSTR) szString[iString]))
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
        if (!bIsMaskCompare(pPrUserMatch->szNickname, pPrUserMatch->cbNickname, szNickname, lstrlen(szNickname)))
            return FALSE;

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


/*
BOOL LibParseUserMatch(PCSTR pMask, UINT cMask, DATAUSERMATCH & Match)
{
    BOOL  fNick;
    PCSTR pUserid;
    BOOL  fUserid;
    PCSTR pHost;
    BOOL  fHost;
    BOOL  fPeriod;
    PCSTR pServer;
    BOOL  fServer;
    BOOL  fFoundMask;
    UINT  iMask;
    UINT  iLast;


    //
    //  Initialize the returned data user match structure to default.  
    //
	memset(&Match, 0, sizeof(Match));
    Match.fAll = TRUE;

	//
	//  If no mash string was provided then return the default of matching all.
	//
	if (pMask == NULL)
		return FALSE;

	//
	//  Clear local flag variables.
	//
    fNick = FALSE;
    pUserid = NULL;
    fUserid = FALSE;
    pHost = NULL;
    fHost = FALSE;
    fPeriod = FALSE;
    pServer = NULL;
    fServer = FALSE;

    //
    //  Scan the mask for specific characters to determine how to proceed.
    //
    fFoundMask = FALSE;
    for (iMask = 0, iLast = 0; iMask < cMask; iMask++)
    {
        if (pMask[iMask] == '*' || pMask[iMask] == '?')
        {
            fFoundMask = TRUE;
        }
        else if (pMask[iMask] == '!')
        {
            pUserid = &pMask[iMask + 1];
            //Match.cUserid = iMask - iLast;
            Match.cNick = iMask - iLast;
            iLast = iMask + 1;
            fNick = fFoundMask;
            fFoundMask = FALSE;
        }
        else if (pMask[iMask] == '@')
        {
            pHost = &pMask[iMask + 1];
            //Match.cHost = iMask - iLast;
            Match.cUserid = iMask - iLast;
            iLast = iMask + 1;
            fUserid = fFoundMask;
            fFoundMask = FALSE;
        }
        else if (pMask[iMask] == '$')
        {
            pServer = &pMask[iMask + 1];
            //Match.cServer = iMask - iLast;
            Match.cHost = iMask - iLast;
            iLast = iMask + 1;
            fHost = fFoundMask;
            fFoundMask = FALSE;
        }
        else if (pMask[iMask] == '.')
        {
            fPeriod = TRUE;
        }
    }

    //
    //  Set the fFoundMask to the last field in the mask.
    //
    if (pServer)
        fServer = fFoundMask;
    else if (pHost || fPeriod)
        fHost = fFoundMask;
    else if (pUserid)
        fUserid = fFoundMask;
    else
        fNick = fFoundMask;

    //
    //  Parse out the nick from the mask.
    //
    if (pUserid || (pHost == NULL && !fPeriod && pServer == NULL))
    {
        Match.pNick = pMask; 
        Match.cNick = iMask - iLast;
        Match.fNick = fNick;
    }

    //
    //  Parse out the userid from the mask.
    //
    if (pUserid)
    {
        Match.pUserid = pUserid;
        Match.fUserid = fUserid;
    }
    else if (pHost)
    {
        Match.pUserid = pMask;
        Match.cUserid = iMask - iLast;
        Match.fUserid = fUserid;
    }

    //
    //  Parse out the host from the mask.
    //
    if (pHost)
    {
        Match.pHost = pHost;
        Match.fHost = fHost;
    }
    else if (fPeriod && pUserid == NULL)
    {
        Match.pHost = pMask;
        Match.cHost = iMask - iLast;
        Match.fHost = fHost;
    }

    //
    //  Parse out the server from the mask.
    //
    if (pServer)
    {
        Match.pServer = pServer;
        Match.cServer = iMask - iLast;
        Match.fServer = fServer;
    }

	//
	//  If the nick field is null or '*' then null out the pointer to indicate any match.
	//
	if (Match.pNick)
	{
		if (Match.cNick == 0 || (Match.pNick[0] == '*' && Match.cNick == 1))
        {
			Match.pNick = NULL;
			Match.cNick = 0;
        }
        else
            Match.fAll = FALSE;
	}

	//
	//  If the userid field is null or '*' then null out the pointer to indicate any match.
	//
	if (Match.pUserid)
	{
		if (Match.cUserid == 0 || (Match.pUserid[0] == '*' && Match.cUserid == 1))
        {
			Match.pUserid = NULL;
			Match.cUserid = 0;
        }
        else
            Match.fAll = FALSE;
	}

	//
	//  If the host field is null or '*' then null out the pointer to indicate any match.
	//
	if (Match.pHost)
	{
		if (Match.cHost == 0 || (Match.pHost[0] == '*' && Match.cHost == 1))
        {
			Match.pHost = NULL;
			Match.cHost = 0;
        }
        else
            Match.fAll = FALSE;
	}

	//
	//  If the server field is null or '*' then null out the pointer to indicate any match.
	//
	if (Match.pServer)
	{
		if (Match.cServer == 0 || (Match.pServer[0] == '*' && Match.cServer == 1))
        {
			Match.pServer = NULL;
			Match.cServer = 0;
        }
        else
            Match.fAll = FALSE;
	}

    //
    //  If the host field was provided, then it must have at least one letter for
    //  non-sysops to use, else mark it as sysop only.
    //
    if (Match.pHost && Match.cHost)
    {
        for (UINT iHost = 0; iHost < Match.cHost; iHost++)
        {
            if (isalpha(Match.pHost[iHost]))
                break;
        }

        //
        //  If we scanned the entire host mask without finding a letter, mark for sysops.
        //
        if (Match.cHost == iHost)
            Match.fSysop = TRUE;
    }

    //
    //  If a specific nick was specified, then return true.
    //
    if (Match.pNick && Match.cNick && !Match.fNick)
        return TRUE;

    return FALSE;
}
*/
