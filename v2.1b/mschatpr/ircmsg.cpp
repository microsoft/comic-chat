//--------------------------------------------------------------------------------------------
//
// INCLUDES
//
//--------------------------------------------------------------------------------------------
#include "StdAfx.H"
#include "IrcMsg.H"
#include "CCError.H"
#include "MsChatPr.H"

// for ASSERT and FAIL
SZTHISFILE

//--------------------------------------------------------------------------------------------
//
// TABLES
//
//--------------------------------------------------------------------------------------------

#define SET_CMD(sz)		sz, (sizeof(sz) - 1)
//
// This table contains all supported IRC commands. MUST BE SORTED
// and MUST match the order of ID_CMD in ircmsg.h
//
PRIRCCMD g_rgIrcCmd[]=
{
	SET_CMD("ACCESS"),
	SET_CMD("AUTH"),
	SET_CMD("AWAY"),
	SET_CMD("CLONE"),
	SET_CMD("CREATE"),
	SET_CMD("DATA"),
	SET_CMD("ERROR"),
	SET_CMD("INFO"),
	SET_CMD("INVITE"),
	SET_CMD("JOIN"),
	SET_CMD("KICK"),
	SET_CMD("KILL"),
	SET_CMD("KLINE"),
	SET_CMD("KNOCK"),
	SET_CMD("LIST"),
	SET_CMD("LISTX"),
	SET_CMD("LUSERS"),
	SET_CMD("MODE"),
	SET_CMD("NAMES"),
	SET_CMD("NICK"),
	SET_CMD("NOTICE"),
	SET_CMD("PART"),
	SET_CMD("PASS"),
	SET_CMD("PING"),
	SET_CMD("PONG"),
	SET_CMD("PRIVMSG"),
	SET_CMD("PROP"),
	SET_CMD("QUIT"),
	SET_CMD("REPLY"),
	SET_CMD("REQUEST"),
	SET_CMD("TOPIC"),
	SET_CMD("UNKLINE"),
	SET_CMD("USER"),
	SET_CMD("USERHOST"),
	SET_CMD("WHISPER"),
	SET_CMD("WHO"),
	SET_CMD("WHOIS"),
};

PRIRCCMD g_rgIrcFormat[] =
{
	SET_CMD("%s\r\n"),					// CMD
	SET_CMD("%s %s\r\n"),				// CMD PARAM
	SET_CMD("%s :%s\r\n"),				// CMD :PARAM
	SET_CMD("%s %s %s\r\n"),			// CMD PARAM PARAM
	SET_CMD("%s %s :%s\r\n"),			// CMD PARAM :PARAM
	SET_CMD("%s %s %s %s\r\n"),			// CMD PARAM PARAM PARAM
	SET_CMD("%s %s %s :%s\r\n"),		// CMD PARAM PARAM :PARAM
	SET_CMD("%s %s %s %s :%s\r\n"),		// CMD PARAM PARAM PARAM :PARAM
	SET_CMD("%s %s . . :%s\r\n"),		// CMD PARAM . . :PARAM
	SET_CMD("%s %s +%s\r\n"),			
	SET_CMD("%s %s -%s\r\n"),
	SET_CMD("%s %s +%s %s\r\n"),
	SET_CMD("%s %s -%s %s\r\n"),
};

enum ID_FORMAT
{
	ID_FORMAT_NOPARAM,
	ID_FORMAT_ONEPARAM,
	ID_FORMAT_COLONONEPARAM,
	ID_FORMAT_TWOPARAM,
	ID_FORMAT_COLONTWOPARAM,
	ID_FORMAT_THREEPARAM,
	ID_FORMAT_COLONTHREEPARAM,
	ID_FORMAT_COLONFOURPARAM,
	ID_FORMAT_USERPARAM,
	ID_FORMAT_SETMODE,
	ID_FORMAT_CLEARMODE,
	ID_FORMAT_SETUSERMODE,
	ID_FORMAT_CLEARUSERMODE,
	ID_FORMAT_MAX
};

const ID_FORMAT ID_FORMAT_NONE = ID_FORMAT_MAX;

//
// Table that maps IRC modes to OURs
//
PRMODEMAP g_rgPrChannelMode[] = 
{
	'a', cmAuthOnly,
	'd', cmCloneable,
	'e', cmClone,
	'f', cmNoFormat,
	'h', cmHidden,
	'i', cmInvite,
	'u', cmKnock,
	'm', cmModerated,
	'n', cmNoExtern,
	'p', cmPrivate,
	'r', cmRegistered,
	's', cmSecret,
	't', cmTopicop,
	'w', cmNoWhisper,
	'x', cmAuditorium,
	'z', cmService
};

const INT CCHANNELMODEMAP = sizeof(g_rgPrChannelMode)/sizeof(PRMODEMAP);

PRMODEMAP g_rgPrUserMode[] =
{
	'a', umAdmin,
	'a', umNotAdmin,
	'i', umInvisible,
	'i', umNotInvisible,
	's', umNotices,
	's', umNoNotices,
	'w', umWallops,
	'w', umNoWallops,
	'o', umSysop,
	'o', umNotSysop
};

const INT CUSERMODEMAP = sizeof(g_rgPrUserMode)/sizeof(PRMODEMAP);

PRMODEMAP g_rgPrMemberMode[] =
{
	'v', mmVoice,
	'v', mmNoVoice,
	'o', mmHost,
	'o', mmNotHost,
	'q', mmOwner,
	'q', mmNotOwner
};

const INT CMEMBERMODEMAP = sizeof(g_rgPrMemberMode)/sizeof(PRMODEMAP);


//--------------------------------------------------------------------------------------------
//
// PROTOTYPES
//
//--------------------------------------------------------------------------------------------
HRESULT HrMakeMsg(BYTE **ppb, INT *pcb, ID_FORMAT idFormat, INT cArgs, ...);

//--------------------------------------------------------------------------------------------
//
// FUNCTIONS AND MESSAGE CRACKERS
//
//--------------------------------------------------------------------------------------------
//
//  IRC MESSAGE CRACKERS
//
SHORT NGetCmd(CHAR* szCmd)
{
	ASSERT(szCmd, "szCmd is NULL in NGetCmd");
	//
	// binary search the command table
	//
	SHORT	nMiddle;
	SHORT	nStart, nEnd;	// search range
	SHORT	nRet;
	
	nStart	= 0;
	nEnd	= cmdidMax - 1;

	do
	{
		nMiddle = (nEnd - nStart)/2 + nStart;

		nRet = ::lstrcmpi(szCmd, g_rgIrcCmd[nMiddle].szCmd);
		if (0 == nRet) // a match
			return nMiddle;
		
		if (nStart == nEnd)
			break;

		if (-1 == nRet)
		{
			//
			// The cmd is less than
			//
			nEnd = nMiddle;
		}
		else
		{
			if (nMiddle != nStart)
				nStart = nMiddle;
			else
				nStart = nEnd;
		}
	}
	while (TRUE);
		
	return -1;	// not found
}


HRESULT HrGetPrefix(CHAR** psz, CHAR** pszPrefix)
{
	ASSERT(psz && pszPrefix, "!psz || !pszPrefix in HrGetPrefix");

	CHAR*	sz;
	CHAR*	szPrefix = NULL;
	HRESULT hr = NOERROR;

	sz = *psz;
	//
	// Determine a prefix,if any
	//
	if (g_chColon != *sz)
		goto LReturn;

	//
	// Is there anything that follows?
	//
	++sz;
	if (bIsTermChar(*sz) || g_chSpace == *sz)
	{
		hr = CC_E_SERVER; // server is sending us garbage
		goto LReturn;
	}

	szPrefix = sz;
	SkipTillSpace(&sz);
	if (g_chSpace != *sz)
	{
		hr = CC_E_SERVER; // nothing valid after the prefix. getting garbage
		goto LReturn;
	}
	//
	// Null terminate it
	//
	*sz++ = g_chEOS;
	SkipSpaces(&sz);
	
LReturn:
	*pszPrefix = szPrefix;
	*psz = sz;

	return hr;
}

HRESULT HrGetCommand(CHAR** psz, CHAR** pszCmd)
{
	ASSERT(psz && pszCmd, "!psz || !pszCmd in HrGetCommand");

	CHAR*	sz;
	CHAR*	szCmd = NULL;
	HRESULT hr = NOERROR;

	sz = *psz;
 	//
	// Is there anything that follows?
	//
	if (bIsTermChar(*sz))
	{
		hr = CC_E_SERVER; // server is sending us garbage
		goto LReturn;
	}

	szCmd = sz;
	SkipTillSpace(&sz);
	if (g_chSpace != *sz)
	{
		if (*sz)
			hr = CC_E_SERVER; // nothing valid after the prefix. getting garbage
		else
			hr = NOERROR;
		goto LReturn;
	}
	//
	// Null terminate it
	//
	*sz++ = g_chEOS;
	SkipSpaces(&sz);
	
LReturn:
	*pszCmd = szCmd;
	*psz = sz;

	return hr;
}

//
// Given a string that is known to contain params, get the next param and skip the
// psz to past it..
//
BOOL bGetNextParam(CHAR** psz, CHAR** pszParam, INT* pcch)
{
	ASSERT(psz && pszParam, "!psz or !pszParam in bGetNextParam");

	CHAR *sz, *szParam;
	BOOL bRet = FALSE;

	sz = *psz;
	szParam = NULL;
	if (!*sz || bIsTermChar(*sz))
		goto LReturn;	// no more params

	szParam = sz;
	if (g_chColon == *sz)
	{
		// trailing param
		szParam += 1;
		SkipTillEnd(&sz);		
	}
	else
		SkipTillSpace(&sz);
	
	*pcch = (sz - szParam);
	if (*sz)
	{
		*sz = g_chEOS;	// null terminate it.
		++sz;
		SkipSpaces(&sz);
	}

	bRet = TRUE;

LReturn:
	
	*pszParam = szParam;
	*psz = sz;
		
	return bRet;
}

//
// Get Sub param.. a single parameter can be split into multiple.. by ','
// NOTE this assumes that you have actually got a VALID single param.. and
// NULL terminated. make sure you call bGetNextParam first.
//
BOOL bGetNextSubParam(CHAR **pszParam, CHAR **pszSubParam, INT *pcch)
{
	ASSERT(pszParam && pszSubParam && pcch, "One param NULL in bGetNextSubParam");

	CHAR *sz;

	*pszSubParam = *pszParam;
	sz = *pszParam;
	*pcch = 0;
	if (!*sz || bIsTermChar(*sz))
		return FALSE;

	//
	// Skip to the next ','
	//
	SkipTillChar(&sz, g_chComma);
	*pcch = (sz - *pszParam);
	if (*sz)
	{
		*sz = g_chEOS;
		++sz;
	}
	*pszParam = sz;	
	
	return TRUE;
}


void SkipTillChar(CHAR **psz, CHAR ch)
{
	ASSERT(psz, "psz is NULL in SkipTillChar");

	CHAR *sz = *psz;
	//
	// IRC strings are terminated by \n
	//
	while (*sz && !bIsTermChar(*sz) && ch != *sz)
		++sz;

	*psz = sz;
}


void SkipOverChar(CHAR **psz, CHAR ch)
{
	ASSERT(psz, "psz is NULL in SkipOverChar");

	CHAR *sz = *psz;
	//
	// IRC strings are terminated by \n
	//
	while (*sz && !bIsTermChar(*sz) && ch == *sz)
		++sz;

	*psz = sz;
}


void SkipTillDigit(CHAR **psz)
{
	ASSERT(psz, "psz is NULL in SkipTillDigit");

	CHAR *sz = *psz;

	while (*sz && !bIsDigit(*sz))
		++sz;

	*psz = sz;
}


void SkipTillAlpha(CHAR **psz)
{
	ASSERT(psz, "psz is NULL in SkipTillAlpha");

	CHAR *sz = *psz;

	while (*sz && bIsDigit(*sz))
		++sz;

	*psz = sz;
}


void SkipTillEnd(CHAR **psz)
{
	ASSERT(psz, "psz is NULL in SkipTillEnd");

	CHAR *sz = *psz;

	while (*sz && !bIsTermChar(*sz))
		++sz;
	
	*psz = sz;
}

void SkipTillPrefixExtender(CHAR **psz)
{
	ASSERT(psz, "psz is NULL in SkipTillPrefixExtender");

	CHAR *sz = *psz;

	while (*sz && g_chSpace != *sz && g_chExclamation != *sz && g_chAt != *sz)
		++sz;
	
	*psz = sz;
}


BOOL bIsChannelModesValid(LONG lModes)
{
	return (
			(lModes >= 0L) &&
			(0L == (lModes &  ~(cmPublic	|\
								cmPrivate	|\
								cmHidden	|\
								cmSecret	|\
								cmModerated	|\
								cmNoExtern	|\
								cmTopicop	|\
								cmInvite	|\
								cmKnock		|\
								cmNoWhisper	|\
								cmRegistered|\
								cmService	|\
								cmAuthOnly	|\
								cmCloneable	|\
								cmClone		|\
								cmAuditorium|\
								cmNoFormat	|\
								cmCreateOnly))) &&
			!((lModes & cmPrivate) && (lModes & cmSecret)) &&
			!((lModes & cmPrivate) && (lModes & cmHidden)) &&
			!((lModes & cmHidden) && (lModes & cmSecret))
		   );
}


BOOL bIsMemberModesValid(LONG lModes)
{
	if ((lModes < 0L) || 
		(0L != (lModes &  ~(mmClientIgnored		|\
							mmNotClientIgnored	|\
							mmAuthMember		|\
							mmNotAuthMember		|\
							mmAway				|\
							mmNotAway			|\
							mmSysop				|\
							mmNotSysop			|\
							mmOwner				|\
							mmNotOwner			|\
							mmHost				|\
							mmNotHost			|\
							mmVoice				|\
							mmNoVoice))))
		return FALSE;

	if (((lModes & mmClientIgnored) && (lModes & mmSysop))				||	// Can't ignore a sysop
		((lModes & mmClientIgnored) && (lModes & mmOwner))				||	// Can't ignore an owner
		((lModes & mmClientIgnored) && (lModes & mmHost))				||	// Can't ignore a host
		((lModes & mmClientIgnored) && (lModes & mmNotClientIgnored))	||
		((lModes & mmAuthMember) && (lModes & mmNotAuthMember))			||
		((lModes & mmAway) && (lModes & mmNotAway))						||
		((lModes & mmSysop) && (lModes & mmNotSysop))					||
		((lModes & mmOwner) && (lModes & mmNotOwner))					||
		((lModes & mmHost) && (lModes & mmNotHost))						||
		((lModes & mmVoice) && (lModes & mmNoVoice))					||
		((lModes & mmHost) && (lModes & mmOwner)))
		return FALSE;

	return TRUE;
}


BOOL bIsUserModesValid(LONG lModes)
{
	if ((lModes < 0L) || 
		(0L != (lModes &  ~(umClientIgnored		|\
							umNotClientIgnored	|\
							umInvisible			|\
							umNotInvisible		|\
							umAuthUser			|\
							umNotAuthUser		|\
							umAway				|\
							umNotAway			|\
							umSysop				|\
							umNotSysop			|\
							umNotices			|\
							umNoNotices			|\
							umWallops			|\
							umNoWallops			|\
							umAdmin				|\
							umNotAdmin))))
		return FALSE;

	if (((lModes & umClientIgnored) && (lModes & umSysop))				||	// Can't ignore Sysops
		((lModes & umClientIgnored) && (lModes & umAdmin))				||	// Can't ignore Admins
		((lModes & umClientIgnored) && (lModes & umNotClientIgnored))	||
		((lModes & umInvisible) && (lModes & umNotInvisible))			||
		((lModes & umAuthUser) && (lModes & umNotAuthUser))				||
		((lModes & umAway) && (lModes & umNotAway))						||
		((lModes & umSysop) && (lModes & umNotSysop))					||
		((lModes & umNotices) && (lModes & umNoNotices))				||
		((lModes & umWallops) && (lModes & umNoWallops))				||
		((lModes & umAdmin) && (lModes & umNotAdmin)))
		return FALSE;

	return TRUE;
}


void ExtractNickUserIPAddress(LPTSTR szUserIdent, BOOL bIrcX, LPTSTR *pszNickname, LPTSTR *pszUserName, LPTSTR *pszIPAddress, BOOL *pbAuthUser)
{
	ASSERT(szUserIdent, "szUserIdent is NULL in ExtractNickUserIPAddress");
	ASSERT(pszNickname, "pszNickname is NULL in ExtractNickUserIPAddress");
	ASSERT(pszUserName, "pszUserName is NULL in ExtractNickUserIPAddress");
	ASSERT(pszIPAddress, "pszIPAddress is NULL in ExtractNickUserIPAddress");
	ASSERT(pbAuthUser, "pbAuthUser is NULL in ExtractNickUserIPAddress");

	DWORD cLen = lstrlen(szUserIdent);

	// szUserIdent has format MsChaTst!~regisb@157.55.102.164   or  @MsChaTst, or .MsChaTst

	*pbAuthUser = FALSE;
	*pszIPAddress = NULL;
	*pszNickname = *pszUserName = szUserIdent;
	SkipTillPrefixExtender(pszUserName);
	if (*pszUserName && (DWORD) (*pszUserName-szUserIdent) < cLen)
	{
		(*pszUserName)[0] = g_chEOS;
		*pszIPAddress = ++(*pszUserName);

		SkipTillPrefixExtender(pszIPAddress);
		if (*pszIPAddress && (DWORD) (*pszIPAddress-szUserIdent) < cLen)
		{
			(*pszIPAddress)[0] = g_chEOS;
			(*pszIPAddress)++;
		}
		else
			*pszIPAddress = NULL;

		if (bIrcX)
			*pbAuthUser = TRUE;	// User is innocent until proven guilty
		while ('~' == (*pszUserName)[0] || '+' == (*pszUserName)[0])
		{
			if ('~' == (*pszUserName)[0] && bIrcX)
				*pbAuthUser = FALSE;
			(*pszUserName)++;
		}
	}
	else
		*pszUserName = NULL;
}


//
// IRC to INTERNAL MAPPINGS
//
//
LONG LModeFromIrcChar(CHAR ch)
{
	for (INT i = 0; i < CCHANNELMODEMAP; i++)
		if (g_rgPrChannelMode[i].ircMode == ch)
			return g_rgPrChannelMode[i].lMode;

	return 0L;
}


// this ASSUMES that szMode is big enough. Allocating a buffer of 32 bytes should more than
// do it
//
INT MapToIRCMode(enumAssociatedType at, LONG lMode, CHAR *szMode, INT cchMode)
{															 
	ASSERT(szMode, "szMode is NULL in MapToIRCMode");

	INT i, cch;
	//
	// Go through the provided lMode..and build the IRC equivalent in szMode
	// return the length of the mode string..
	//
	switch (at)
	{
		case atUser:
			for (i = 0, cch = 0; cch < cchMode && i < CUSERMODEMAP; ++i)
			{
				if (0L != (lMode & g_rgPrUserMode[i].lMode))
					szMode[cch++] = g_rgPrUserMode[i].ircMode;
			}
			break;
		case atMember:
			for (i = 0, cch = 0; cch < cchMode && i < CMEMBERMODEMAP; ++i)
			{
				if (0L != (lMode & g_rgPrMemberMode[i].lMode))
					szMode[cch++] = g_rgPrMemberMode[i].ircMode;
			}
			break;
		case atChannel:
			for (i = 0, cch = 0; cch < cchMode && i < CCHANNELMODEMAP; ++i)
			{
				if (0L != (lMode & g_rgPrChannelMode[i].lMode))
					szMode[cch++] = g_rgPrChannelMode[i].ircMode;
			}
			break;
		default:
			ASSERT(FALSE, "Unexpected associated type in MapToIRCMode");
			break;
	}

	szMode[cch] = g_chEOS;

	return cch;
}


void ApplyIRCToOurChannelMode(CHAR *szMode, LONG *plMode, BOOL *pbCount, BOOL *pbKeywordSet, BOOL *pbKeywordReset, BOOL *pbMicOnly)
{
	ASSERT(szMode && plMode, "!szMode || !plMode in ApplyIRCToOurChannelMode");
	
	BOOL	bSet;
	CHAR	ch;
	LONG	lMode;
	
	if (pbCount)
		*pbCount = FALSE;

	if (pbKeywordSet)
		*pbKeywordSet = FALSE;
	if (pbKeywordReset)
		*pbKeywordReset = FALSE;
	if (pbMicOnly)
		*pbMicOnly = FALSE;

	ASSERT(bIsChannelModesValid(*plMode), "Initial *plModes not valid in ApplyIRCToOurChannelMode");

	while (TRUE)
	{
		ch = *szMode;
		if ('+' == ch || '-' == ch)
		{
			bSet = ('+' == ch);
			szMode++;
			ch = *szMode;
		}

		switch(ch)
		{
			case g_chEOS:
				return;

			case 'l':
				lMode = 0L;
				if (pbCount)
					*pbCount = TRUE;									
				break;

			case 'k':
				lMode = 0L;
				if (bSet && pbKeywordSet)
					*pbKeywordSet = TRUE;
				if (!bSet && pbKeywordReset)
					*pbKeywordReset = TRUE;
				break;

			case 'y':
				if (pbMicOnly)
					*pbMicOnly = bSet;
				break;

			default:
				lMode = LModeFromIrcChar(ch);
				break;
		}
		
		if (bSet)
			*plMode |= lMode;
		else
			*plMode &= ~lMode;

		++szMode;		
	}	

	ASSERT(bIsChannelModesValid(*plMode), "Final *plModes not valid in ApplyIRCToOurChannelMode");
}


void ApplyIRCToOurMemberMode(CHAR *szMode, LONG *plMode)
{
	ASSERT(szMode && plMode, "!szMode || !plMode in ApplyIRCToOurMemberMode");
	
	LONG	lAdd, lDel;
	BOOL	bSet;
	CHAR	ch;

	ASSERT(bIsMemberModesValid(*plMode), "Initial *plModes not valid in ApplyIRCToOurMemberMode");

	while (TRUE)
	{
		ch = *szMode;
		if ('+' == ch || '-' == ch)
		{
			bSet = ('+' == ch);
			szMode++;
			ch = *szMode;
		}

		lAdd = lDel = 0L;
		// o : host or not
		// v : voice or not
		// q : owner or not
		switch(ch)
		{
			case g_chEOS:
				ASSERT(bIsMemberModesValid(*plMode), "Final *plModes not valid in ApplyIRCToOurMemberMode");
				return;

			case 'q':
				if (bSet)
				{
					lAdd = mmOwner|mmNotClientIgnored|mmNotHost;
					lDel = mmNotOwner|mmHost;
				}
				else
				{
					lAdd = mmNotOwner;
					lDel = mmOwner;
				}
				break;

			case 'o':
				if (bSet)
				{
					lAdd = mmHost|mmNotClientIgnored|mmNotOwner;
					lDel = mmNotHost|mmOwner;
				}
				else
				{
					lAdd = mmNotHost;
					lDel = mmHost;
				}
				break;

			case 'v':
				if (bSet)
				{
					lAdd = mmVoice;
					lDel = mmNoVoice;
				}
				else
				{
					lAdd = mmNoVoice;
					lDel = mmVoice;
				}
				break;

			default:
				ASSERT(FALSE, "Unexpected member mode character in ApplyIRCToOurMemberMode");
		}

		*plMode |= lAdd;
		*plMode &= ~lDel;

		++szMode;
	}
}


void ApplyIRCToOurUserMode(CHAR *szMode, LONG *plMode)
{
	ASSERT(szMode && plMode, "!szMode || !plMode in ApplyIRCToOurUserMode");
	
	LONG	lAdd, lDel;
	BOOL	bSet;
	CHAR	ch;

	ASSERT(bIsUserModesValid(*plMode), "Initial *plModes not valid in ApplyIRCToOurUserMode");

	while (TRUE)
	{
		ch = *szMode;
		if ('+' == ch || '-' == ch)
		{
			bSet = ('+' == ch);
			szMode++;
			ch = *szMode;
		}

		lAdd = lDel = 0L;
		// a : administrator or not
		// i : invisible or not
		// s : receive server notices or not
		// w : receive wallops or not
		// o : sysop or not
		switch(ch)
		{
			case g_chEOS:
				ASSERT(bIsUserModesValid(*plMode), "Final *plModes not valid in ApplyIRCToOurUserMode");
				return;

			case 'a':
				if (bSet)
				{
					lAdd = umAdmin;
					lDel = umNotAdmin;
				}
				else
				{
					lAdd = umNotAdmin;
					lDel = umAdmin;
				}
				break;

			case 'i':
				if (bSet)
				{
					lAdd = umInvisible;
					lDel = umNotInvisible;
				}
				else
				{
					lAdd = umNotInvisible;
					lDel = umInvisible;
				}
				break;

			case 's':
				if (bSet)
				{
					lAdd = umNotices;
					lDel = umNoNotices;
				}
				else
				{
					lAdd = umNoNotices;
					lDel = umNotices;
				}
				break;

			case 'w':
				if (bSet)
				{
					lAdd = umWallops;
					lDel = umNoWallops;
				}
				else
				{
					lAdd = umNoWallops;
					lDel = umWallops;
				}
				break;

			case 'o':
				if (bSet)
				{
					lAdd = umSysop;
					lDel = umNotSysop;
				}
				else
				{
					lAdd = umNotSysop;
					lDel = umSysop;
				}
				break;

			default:
				ASSERT(FALSE, "Unexpected member mode character in ApplyIRCToOurMemberMode");
		}

		*plMode |= lAdd;
		*plMode &= ~lDel;

		++szMode;
	}
}


// REGISB: not used yet, might be able to remove it.
//PPRMODEMAP PPrModeMap(INT *pcModes)
//{
//	*pcModes = CCHANNELMODEMAP;
//	return &g_rgPrChannelMode[0];
//}


HRESULT HrParseServerName(CHAR *szServer, UINT *pcch, UINT *puPort)
{
	ASSERT(szServer && pcch, "!szServer || !pcch in HrParseServerName");
	
	CHAR *szSav;

	szSav = szServer;
	SkipTillChar(&szServer, g_chColon);
	if (*szServer)
		*puPort = (UINT) SzToL(szServer + 1);
	else
		// No port provided. Use default
		*puPort = g_uDefaultPort;
	
	*pcch = (UINT) (szServer - szSav);
	
	return (*pcch == 0 || *puPort == 0) ? E_INVALIDARG : NOERROR;
}

//
// IRC MESSAGE MAKERS
//
HRESULT HrMakeMsg(BYTE **ppb, INT *pcb, ID_FORMAT idFormat, INT cArgs, ...)
{
	INT		cbAlloc = 0;
	HRESULT hr = NOERROR;	
	PSTR	sz;
	va_list args;
	INT		i;
	INT		cbArgs = 0;

	*ppb = NULL;
	//
	// Get the arguments to be used in the message
	//
	va_start(args,cArgs);
	//
	// Figure out the size
	// how much does the command take.
	//
	cbAlloc = g_rgIrcFormat[idFormat].cb; 
	for (i = 0; i < cArgs; ++i)
	{
		sz = va_arg(args,PSTR);		
		cbArgs += lstrlen(sz);
	}	
	va_end(args);	
	//
	// Allocate the memory
	//
	cbAlloc += cbArgs;
	if (cbArgs > g_lMaxMsgLength)
		return CC_E_TOOMUCHDATA;

	*ppb = new BYTE[cbAlloc];
	if (!*ppb)
	{
		ASSERT(FALSE, "Out of Memory in HrMakeMsg");
		hr = E_OUTOFMEMORY;
		cbAlloc = 0;
		goto LReturn;
	}
	//
	// And make the msg
	//
	sz = (CHAR*) *ppb;
	va_start(args,cArgs);
	cbAlloc = wvsprintf(sz, g_rgIrcFormat[idFormat].szCmd, args);

	if (cbAlloc > g_lMaxMsgLength)
	{
		ASSERT(FALSE, "Too much data in HrMakeMsg");
		delete [] *ppb;
		*ppb = NULL;
		cbAlloc = 0;
		hr = CC_E_TOOMUCHDATA;
	}

LReturn:
	*pcb = cbAlloc;
	return hr;
}


HRESULT HrMapToIRCAndMakeMemberModeMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szNickname, LONG lOldModes, LONG lNewModes)
{
	CHAR	szModes[64];	//at most 31 mode chars, and at most 31 + or - to go with them..
							//.. so 64 chars should more than cover it :-)
	INT		ich = 0;
	BOOL	bFlagInOld, bFlagInNew, bNotFlagInOld, bNotFlagInNew;
	INT		cModes;
	CHAR	chOld = '*', chNew;

	ASSERT(szChannel, "szChannel is NULL in HrMapToIRCAndMakeMemberModeMsg");
	ASSERT(szNickname, "szNickname is NULL in HrMapToIRCAndMakeMemberModeMsg");
	ASSERT(bIsMemberModesValid(lOldModes), "lOldModes not valid in HrMapToIRCAndMakeMemberModeMsg");
	ASSERT(bIsMemberModesValid(lNewModes), "lNewModes not valid in HrMapToIRCAndMakeMemberModeMsg");

	// Map Our modes to IRC Member Modes
	cModes = CMEMBERMODEMAP / 2;
	while (cModes > 0)
	{
		--cModes;

		bFlagInOld = (g_rgPrMemberMode[2*cModes].lMode & lOldModes);
		bNotFlagInOld = (g_rgPrMemberMode[2*cModes+1].lMode & lOldModes);

		bFlagInNew = (g_rgPrMemberMode[2*cModes].lMode & lNewModes);
		bNotFlagInNew = (g_rgPrMemberMode[2*cModes+1].lMode & lNewModes);

		if ((bFlagInNew || bNotFlagInNew) && ((bFlagInNew != bFlagInOld) || (bNotFlagInNew != bNotFlagInOld)))
		{
			// The Flag or NotFlag is still specified and at least one of them has changed
			if (bFlagInNew)
			{
				ASSERT(!bNotFlagInNew, "Unexpected member modes in HrMapToIRCAndMakeMemberModeMsg");
				chNew = '+';
			}
			else
			{
				ASSERT(!bFlagInNew, "Unexpected member modes in HrMapToIRCAndMakeMemberModeMsg");
				chNew = '-';
			}

			if (chNew != chOld)
			{
				szModes[ich++] = chNew;
				chOld = chNew;
			}

			// Turn this mode bit into an IRC char
			szModes[ich++] = g_rgPrMemberMode[2*cModes].ircMode;
			szModes[ich] = g_chEOS;
		}
	}

	if (ich > 0)
		// Got the modes into an IRC string.. now construct the mode message
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_THREEPARAM,
						 4, 
						 g_rgIrcCmd[cmdidMode].szCmd,
						 szChannel,
						 szModes,
						 szNickname);

	// No changes were made to the member modes
	return S_FALSE;
}


HRESULT HrMapToIRCAndMakeUserModeMsg(BYTE **ppb, INT *pcb, CHAR *szNickname, LONG lOldModes, LONG lNewModes)
{
	CHAR	szModes[32];	//at most 15 mode chars, and at most 15 + or - to go with them..
							//.. so 32 chars should more than cover it :-)
	INT		ich = 0;
	BOOL	bFlagInOld, bFlagInNew, bNotFlagInOld, bNotFlagInNew;
	INT		cModes;
	CHAR	chOld = '*', chNew;

	ASSERT(szNickname, "szNickname is NULL in HrMapToIRCAndMakeUserModeMsg");
	ASSERT(bIsUserModesValid(lOldModes), "lOldModes not valid in HrMapToIRCAndMakeUserModeMsg");
	ASSERT(bIsUserModesValid(lNewModes), "lNewModes not valid in HrMapToIRCAndMakeUserModeMsg");

	// Map Our modes to IRC User Modes
	cModes = CUSERMODEMAP / 2;
	while (cModes > 0)
	{
		--cModes;

		bFlagInOld = (g_rgPrUserMode[2*cModes].lMode & lOldModes);
		bNotFlagInOld = (g_rgPrUserMode[2*cModes+1].lMode & lOldModes);

		bFlagInNew = (g_rgPrUserMode[2*cModes].lMode & lNewModes);
		bNotFlagInNew = (g_rgPrUserMode[2*cModes+1].lMode & lNewModes);

		if ((bFlagInNew || bNotFlagInNew) && ((bFlagInNew != bFlagInOld) || (bNotFlagInNew != bNotFlagInOld)))
		{
			// The Flag or NotFlag is still specified and at least one of them has changed
			if (bFlagInNew)
			{
				ASSERT(!bNotFlagInNew, "Unexpected user modes in HrMapToIRCAndMakeUserModeMsg");
				chNew = '+';
			}
			else
			{
				ASSERT(!bFlagInNew, "Unexpected member modes in HrMapToIRCAndMakeUserModeMsg");
				chNew = '-';
			}

			if (chNew != chOld)
			{
				szModes[ich++] = chNew;
				chOld = chNew;
			}

			// Turn this mode bit into an IRC char
			szModes[ich++] = g_rgPrUserMode[2*cModes].ircMode;
			szModes[ich] = g_chEOS;
		}
	}

	if (ich > 0)
		// Got the modes into an IRC string.. now construct the mode message
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_TWOPARAM,
						 3,
						 g_rgIrcCmd[cmdidMode].szCmd,
						 szNickname,
						 szModes);

	// No changes were made to the user modes
	return S_FALSE;
}


HRESULT HrMapToIRCAndMakeChannelModeMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, LONG lOldModes, LONG lNewModes)
{
	CHAR	szModes[64];	//at most 31 mode chars, and at most 31 + or - to go with them..
							//.. so 64 chars should more than cover it :-)
	INT		ich = 0;
	BOOL	bInOld, bInNew;
	BOOL	bInsertSet = TRUE;
	INT		cModes;
	CHAR	ch = '+';

	ASSERT(szChannel, "szChannel is NULL in HrMapToIRCAndMakeChannelModeMsg");
	ASSERT(bIsChannelModesValid(lOldModes), "lOldModes not valid in HrMapToIRCAndMakeChannelModeMsg");
	ASSERT(bIsChannelModesValid(lNewModes), "lNewModes not valid in HrMapToIRCAndMakeChannelModeMsg");

	// Map Our modes to IRC Channel Modes
	cModes = CCHANNELMODEMAP-1;
	while (cModes >= 0)
	{
		bInOld = (g_rgPrChannelMode[cModes].lMode & lOldModes);
		bInNew = (g_rgPrChannelMode[cModes].lMode & lNewModes);

		if (bInNew != bInOld)
		{
			// Since we are touching this flag, is it being turned on or off?
			if (bInNew)
			{
				if ('-' == ch)
					bInsertSet = TRUE; // make sure we insert a + in the mode string
				ch = '+';
			}
			else
			{
				if ('+' == ch)
					bInsertSet = TRUE;	// make sure we insert a - in the mode string
				ch = '-';
			}

			if (bInsertSet)
			{
				szModes[ich++] = ch;
				bInsertSet = FALSE;
			}

			// Turn this mode bit into an IRC char
			szModes[ich++] = g_rgPrChannelMode[cModes].ircMode;
			szModes[ich] = g_chEOS;
		}
		cModes--;
	}

	if (ich > 0)
		// Got the modes into an IRC string.. now construct the mode message
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_TWOPARAM,
						 3, 
						 g_rgIrcCmd[cmdidMode].szCmd,
						 szChannel,
						 &szModes[0]);
	
	// No channel mode changes were made
	return S_FALSE;
}


inline HRESULT HrMakeNickMsg(BYTE **ppb, INT *pcb, CHAR *szNickname)
{
	ASSERT(szNickname, "szNickname is NULL in HrMakeNickMsg");
	
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2,
					 g_rgIrcCmd[cmdidNick].szCmd,
					 szNickname);
}


inline HRESULT HrMakeUserMsg(BYTE **ppb, INT *pcb, CHAR *szUser, CHAR *szRealName)
{
	ASSERT(szUser && szRealName, "!szUser || !szRealName in HrMakeUserMsg");
	
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_USERPARAM,
					 3, 
					 g_rgIrcCmd[cmdidUser].szCmd,
					 szUser,
					 szRealName);
}

inline HRESULT HrMakeCreateChannelMsg(BYTE **ppb, INT *pcb, CHAR *szChannel)
{
	ASSERT(szChannel, "szChannel is NULL in HrMakeCreateChannelMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2, 
					 g_rgIrcCmd[cmdidJoin].szCmd,
					 szChannel);

}

inline HRESULT HrMakeJoinMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szKeyword)
{
	if (!szKeyword || !*szKeyword)
		return HrMakeCreateChannelMsg(ppb, pcb, szChannel);		

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_TWOPARAM,
					 3, 
					 g_rgIrcCmd[cmdidJoin].szCmd,
					 szChannel,
					 szKeyword);
}

inline HRESULT HrMakeSetKeywordMsg(BYTE **ppb, INT *pcb, BOOL bSet, CHAR *szChannel, CHAR *szKeyword)
{
	ASSERT(szChannel && szKeyword, "!szChannel || !szKeyword in HrMakeSetKeywordMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 bSet ? ID_FORMAT_SETUSERMODE : ID_FORMAT_CLEARUSERMODE,
					 4, 
					 g_rgIrcCmd[cmdidMode].szCmd,
					 szChannel,
					 "k",
					 szKeyword);
}

inline HRESULT HrMakeLogOffMsg(BYTE **ppb, INT *pcb)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_NOPARAM,
					 1, 
					 g_rgIrcCmd[cmdidQuit].szCmd);
}

inline HRESULT HrMakeLUsersMsg(BYTE **ppb, INT *pcb)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_NOPARAM,
					 1, 
					 g_rgIrcCmd[cmdidLUsers].szCmd);
}

inline HRESULT HrMakeServerInfoMsg(BYTE **ppb, INT *pcb)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_NOPARAM,
					 1, 
					 g_rgIrcCmd[cmdidInfo].szCmd);		
}

inline HRESULT HrMakeLeaveChannel(BYTE **ppb, INT *pcb, CHAR *szChannel)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2, 
					 g_rgIrcCmd[cmdidPart].szCmd,
					 szChannel);
}
 
inline HRESULT HrMakeKickMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szNickname, CHAR *szComment)
{
	ASSERT(szNickname && szComment, "!szNickname || !szComment in HrMakeKickMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_COLONTHREEPARAM,
					 4,
					 g_rgIrcCmd[cmdidKick].szCmd,
					 szChannel,
					 szNickname,
					 szComment);
}

inline HRESULT HrMakePassMsg(BYTE **ppb, INT *pcb, CHAR *szPass)
{
	ASSERT(szPass, "szPass is NULL in HrMakePassMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2, 
					 g_rgIrcCmd[cmdidPass].szCmd,
					 szPass);	

}

inline HRESULT HrMakeSetChannelModeMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szMode, CHAR *szTarget)
{
	ASSERT(szChannel && szMode && szTarget, "szChannel || szMode || szTarget is NULL in HrMakeSetChannelModeMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_THREEPARAM,
					 4,
					 g_rgIrcCmd[cmdidMode].szCmd,
					 szChannel,
					 szMode,
					 szTarget);
}

inline HRESULT HrMakeCreateMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szModes, CHAR *szKeyword)
{
	if (szKeyword)
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_THREEPARAM,
						 4, 
						 g_rgIrcCmd[cmdidCreate].szCmd,
						 szChannel,
						 szModes,
						 szKeyword);

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_TWOPARAM,
					 3, 
					 g_rgIrcCmd[cmdidCreate].szCmd,
					 szChannel,
					 szModes);
}

inline HRESULT HrMakeKLineMsg(BYTE **ppb, INT *pcb, BOOL bSet, CHAR *szIdentMask, CHAR *szReason)
{
	ASSERT(szIdentMask, "!szIdentMask in HrMakeKLineMsg");

	if (bSet)
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_COLONTWOPARAM,
						 3,
						 g_rgIrcCmd[cmdidKLine].szCmd,
						 szIdentMask,
						 szReason ? szReason : g_szEmpty);
	else
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_ONEPARAM,
						 2,
						 g_rgIrcCmd[cmdidUnKLine].szCmd,
						 szIdentMask);
}

inline HRESULT HrMakeAccessMsg(BYTE **ppb, INT *pcb, BOOL bSet, CHAR *szIdentMask, CHAR *szReason, LONG lDuration)
{
	ASSERT(szIdentMask, "!szIdentMask in HrMakeAccessMsg");

	if (bSet)
	{
		if (lDuration >= 0L)
		{
			CHAR szDuration[32];
			wsprintf(szDuration, "%ld", lDuration);
			return HrMakeMsg(ppb,
							 pcb,
							 ID_FORMAT_COLONFOURPARAM,
							 5,
							 g_rgIrcCmd[cmdidAccess].szCmd,
							 "* ADD DENY",
							 szIdentMask,
							 szDuration,
							 szReason ? szReason : g_szEmpty);
		}
		else
			return HrMakeMsg(ppb,
							 pcb,
							 ID_FORMAT_COLONTHREEPARAM,
							 4,
							 g_rgIrcCmd[cmdidAccess].szCmd,
							 "* ADD DENY",
							 szIdentMask,
							 szReason ? szReason : g_szEmpty);
	}
	else
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_TWOPARAM,
						 3,
						 g_rgIrcCmd[cmdidAccess].szCmd,
						 "* DELETE DENY",
						 szIdentMask);
}

inline HRESULT HrMakeWhoMsg(BYTE **ppb, INT *pcb, CHAR *szUserMask)
{
	if (szUserMask)
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_ONEPARAM,
						 2,
						 g_rgIrcCmd[cmdidWho].szCmd,
						 szUserMask);

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_NOPARAM,
					 1,
					 g_rgIrcCmd[cmdidWho].szCmd);
}


// None - inline functions
HRESULT HrMakeKillMsg(BYTE **ppb, INT *pcb, CHAR *szTarget, CHAR *szReason)
{
	if (szReason)
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_COLONTWOPARAM,
						 3,
						 g_rgIrcCmd[cmdidKill].szCmd,
						 szTarget,
						 szReason);

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2, 
					 g_rgIrcCmd[cmdidKill].szCmd,
					 szTarget);	
}

HRESULT HrMakeModeMsg(BYTE **ppb, INT *pcb, BOOL bSet, CHAR *szTarget, CHAR *szMode)
{
	ASSERT(szTarget && szMode, "!szTarget || !szMode in HrMakeModeMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 bSet ? ID_FORMAT_SETMODE : ID_FORMAT_CLEARMODE,
					 3, 
					 g_rgIrcCmd[cmdidMode].szCmd,
					 szTarget,
					 szMode);
}

HRESULT HrMakeGetBannedListMsg(BYTE **ppb, INT *pcb, CHAR *szTarget)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_TWOPARAM,
					 3,
					 g_rgIrcCmd[cmdidMode].szCmd,
					 szTarget,
					 "+b");
}

HRESULT HrMakeGetModeMsg(BYTE **ppb, INT *pcb, CHAR *szTarget)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2, 
					 g_rgIrcCmd[cmdidMode].szCmd,
					 szTarget);
}

HRESULT HrMakeChannelMaxMemberCountMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, LONG lMaxMemberCount)
{
	ASSERT(szChannel, "szChannel is NULL in HrMakeChannelUserMaxMsg");
	
	CHAR szMaxMemberCount[32];
	
	wsprintf(szMaxMemberCount, "%ld", lMaxMemberCount);

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_SETUSERMODE,	// this works for <MODE szChannel +l lMaxMemberCount>
					 4, 
					 g_rgIrcCmd[cmdidMode].szCmd,
					 szChannel,
					 "l",
					 szMaxMemberCount);
}


HRESULT HrMakePingOrPongMsg(BYTE **ppb, INT *pcb, BOOL bPing, CHAR *szTarget)
{
	ASSERT(szTarget, "!szTarget in HrMakePingOrPongMsg");
		
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2, 
					 bPing ? g_rgIrcCmd[cmdidPing].szCmd : g_rgIrcCmd[cmdidPong].szCmd,
					 szTarget);
}


HRESULT HrMakeSendText(BYTE **ppb, INT *pcb, BOOL bNotice, BOOL bWhisper, CHAR *szTarget, CHAR *szRcpNicks, CHAR *szText)
{
	ASSERT(szTarget && szText, "!szTarget || !szText in HrMakeSendText");
	
	if (bWhisper)
	{
		// This is a whisper
		ASSERT(!bNotice, "bNotice is TRUE in whisper case in HrMakeSendText");
		ASSERT(szRcpNicks, "szRcpNicks is NULL in whisper case in HrMakeSendText");
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_COLONTHREEPARAM,
						 4,
						 g_rgIrcCmd[cmdidWhisper].szCmd,
						 szTarget,		// Channel name
						 szRcpNicks,	// Recipient Nicknames
						 szText);
	}
	else
	{
		// This is a private message or normal channel message
		if (szRcpNicks)
			return HrMakeMsg(ppb,
							 pcb,
							 ID_FORMAT_COLONTHREEPARAM,
							 4,
							 bNotice ? g_rgIrcCmd[cmdidNotice].szCmd : g_rgIrcCmd[cmdidPrivMsg].szCmd,
							 szTarget,		// Channel name
							 szRcpNicks,	// Recipient Nicknames
							 szText);
		else
			return HrMakeMsg(ppb,
							 pcb,
							 ID_FORMAT_COLONTWOPARAM,
							 3,
							 bNotice ? g_rgIrcCmd[cmdidNotice].szCmd : g_rgIrcCmd[cmdidPrivMsg].szCmd,
							 szTarget, // Channel name or user for private message
							 szText);
	}
}


HRESULT HrMakeSendData(BYTE **ppb, INT *pcb, CHAR *szTarget, CHAR *szRcpNicks, CHAR *szTag, CHAR *szData, SHORT nCmd)
{
	ASSERT(szTarget && szData && szTag, "!szTarget || !szdata || !szTag in HrMakeSendData");
	ASSERT(cmdidData == nCmd || cmdidReply == nCmd || cmdidRequest == nCmd, "Unexpected nCmd in HrMakeSendData");

	if (szRcpNicks)
	{
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_COLONFOURPARAM,
						 5,
						 g_rgIrcCmd[nCmd].szCmd,
						 szTarget,		// Channel name
						 szRcpNicks,	// Recipient Nicknames
						 szTag,
						 szData);
	}
	else
	{
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_COLONTHREEPARAM,
						 4,
						 g_rgIrcCmd[nCmd].szCmd,
						 szTarget, // Channel name or User nickname for private message
						 szTag,
						 szData);
	}
}


HRESULT HrMakeTopicMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szTopic)
{
	ASSERT(szTopic && szChannel, "!szTopic || !szChannel in HrMakeTopicMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_COLONTWOPARAM,
					 3, 
					 g_rgIrcCmd[cmdidTopic].szCmd,
					 szChannel,
					 szTopic);
}

HRESULT HrMakeListMsg(BYTE **ppb, INT *pcb, CHAR *szCriteria, BOOL bListX)
{
	if (szCriteria)
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_ONEPARAM,
						 2, 
						 g_rgIrcCmd[bListX ? cmdidListX : cmdidList].szCmd,
						 szCriteria);

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_NOPARAM,
					 1, 
					 g_rgIrcCmd[bListX ? cmdidListX : cmdidList].szCmd);
}

HRESULT HrMakeNamesMsg(BYTE **ppb,INT *pcb,CHAR *szChannel)
{
	if (szChannel)
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_ONEPARAM,
						 2, 
						 g_rgIrcCmd[cmdidNames].szCmd,
						 szChannel);

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_NOPARAM,
					 1, 
					 g_rgIrcCmd[cmdidNames].szCmd);

}

HRESULT HrMakeUserHostMsg(BYTE **ppb, INT *pcb, CHAR *szName)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2,
					 g_rgIrcCmd[cmdidUserHost].szCmd,
					 szName);	
}
 
HRESULT HrMakeWhoIsMsg(BYTE **ppb, INT *pcb, CHAR *szNickmask)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_ONEPARAM,
					 2,
					 g_rgIrcCmd[cmdidWhoIs].szCmd,
					 szNickmask);
}

HRESULT HrMakeInviteMsg(BYTE **ppb, INT *pcb, CHAR *szNickname, CHAR *szChannel)
{
	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_TWOPARAM,
					 3,
					 g_rgIrcCmd[cmdidInvite].szCmd,
					 szNickname,
					 szChannel);
}

HRESULT HrMakeAwayMsg(BYTE **ppb, INT *pcb, CHAR *szAway)
{
	if (szAway)
		return HrMakeMsg(ppb,
						 pcb,
						 ID_FORMAT_COLONONEPARAM,
						 2,
						 g_rgIrcCmd[cmdidAway].szCmd,
						 szAway);

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_NOPARAM,
					 1, 
					 g_rgIrcCmd[cmdidAway].szCmd);	
}


HRESULT HrMakeAuthMsg(BYTE **ppb, INT *pcb, CHAR *szSecurityPackage, CHAR *szSeq, CHAR *szBlob)
{
	ASSERT(szSecurityPackage && szSeq && szBlob, "!szSecurityPackage || !szSeq || !szBlob in HrMakeAuthMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_COLONTHREEPARAM,
					 4,
					 g_rgIrcCmd[cmdidAuth].szCmd,
					 szSecurityPackage,
					 szSeq,
					 szBlob);
}


HRESULT HrMakeSetPropMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szPropName, CHAR *szPropValue)
{
	ASSERT(szChannel && szPropName && szPropValue, "!szChannel || !szPropName || !szPropValue in HrMakeSetPropMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_COLONTHREEPARAM,
					 4,
					 g_rgIrcCmd[cmdidProp].szCmd,
					 szChannel,
					 szPropName,
					 szPropValue);
}


HRESULT HrMakeGetPropMsg(BYTE **ppb, INT *pcb, CHAR *szChannel, CHAR *szPropName)
{
	ASSERT(szChannel && szPropName, "!szChannel || !szPropName in HrMakeGetPropMsg");

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_TWOPARAM,
					 3,
					 g_rgIrcCmd[cmdidProp].szCmd,
					 szChannel,
					 szPropName);
}


/*
HRESULT HrMakeRegisterPong(BYTE **ppb, INT *pcb, CHAR *szMsg)
{
	CHAR *szSend;
	//
	// Skip through till the first /. Then send back whatever was contained
	// in the message
	szSend = szMsg;	
	SkipTillChar(&szSend, g_chSlash);	
	++szSend;

	return HrMakeMsg(ppb,
					 pcb,
					 ID_FORMAT_NOPARAM,
					 1,
					 szSend);
}
*/
