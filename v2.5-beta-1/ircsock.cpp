//=--------------------------------------------------------------------------=
// IrcSock.Cpp:		Implementation of CIrcSocket C++ class
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 02/19/98

#include "stdafx.h"
#include "chatprot.h"
#include "chat.h"
#include "chatdoc.h"
#include "ircsock.h"
#include "ircproto.h"
#include "histent.h"
#include "setupdlg.h"
#include "ui.h"
#include "ccommon.h"
#include "format.h"
#include "status.h"
#include "motd.h"
#include "avatar.h"

extern CChatApp theApp;
extern CPtrList g_docs;
extern BOOL		g_bCanViewUnrated;			// cached during room list

static CString		g_strBan;
static CStringArray g_arrayBans;

#define SET_CMD(sz)		sz, (sizeof(sz) - 1)
//
// This table contains all supported IRC commands. MUST BE SORTED
// and MUST match the order of ID_CMD in ircsock.h
//
PRIRCCMD g_rgIrcCmd[]=
{
	SET_CMD("ACCESS"),	0x03, 0x00,
	SET_CMD("ACTION"),	0x02, 0x00,
	SET_CMD("AUTH"),	0x03, 0x00,
	SET_CMD("AWAY"),	0x02, 0x00,
	SET_CMD("CLONE"),	0x03, 0x00,
	SET_CMD("CREATE"),	0x02, 0x00,
	SET_CMD("DATA"),	0x02, 0x00,
	SET_CMD("ERROR"),	0x02, 0x00,
	SET_CMD("INFO"),	0x03, 0x00,
	SET_CMD("INVITE"),	0x02, 0x02,
	SET_CMD("ISON"),	0x03, 0x01,
	SET_CMD("JOIN"),	0x02, 0x00,
	SET_CMD("KICK"),	0x02, 0x02,
	SET_CMD("KILL"),	0x02, 0x01,
	SET_CMD("KILLED"),	0x02, 0x00,
	SET_CMD("KLINE"),	0x03, 0x00,
	SET_CMD("KNOCK"),	0x03, 0x00,
	SET_CMD("LIST"),	0x02, 0x00,
	SET_CMD("LISTX"),	0x03, 0x00,
	SET_CMD("LUSERS"),	0x03, 0x00,
	SET_CMD("ME"),		0x02, 0x00,
	SET_CMD("MODE"),	0x02, 0x01,
	SET_CMD("MSG"),		0x02, 0x00,
	SET_CMD("NAMES"),	0x03, 0x00,
	SET_CMD("NICK"),	0x02, 0x01,
	SET_CMD("NOTICE"),	0x02, 0x00,
	SET_CMD("PART"),	0x02, 0x00,
	SET_CMD("PASS"),	0x02, 0x00,
	SET_CMD("PING"),	0x03, 0x00,
	SET_CMD("PONG"),	0x02, 0x00,
	SET_CMD("PRIVMSG"),	0x02, 0x00,
	SET_CMD("PROP"),	0x02, 0x00,
	SET_CMD("QUIT"),	0x02, 0x00,
	SET_CMD("QUOTE"),	0x03, 0x00,
	SET_CMD("RAW"),		0x03, 0x00,
	SET_CMD("REPLY"),	0x02, 0x00,
	SET_CMD("REQUEST"),	0x02, 0x00,
	SET_CMD("SERVER"),	0x01, 0x00,
	SET_CMD("SOUND"),	0x02, 0x00,
	SET_CMD("THINK"),	0x02, 0x00,
	SET_CMD("TOPIC"),	0x03, 0x01,
	SET_CMD("UNKLINE"),	0x03, 0x00,
	SET_CMD("USER"),	0x03, 0x00,
	SET_CMD("USERHOST"),0x03, 0x01,
	SET_CMD("WHISPER"),	0x02, 0x00,
	SET_CMD("WHO"),		0x02, 0x00,
	SET_CMD("WHOIS"),	0x03, 0x01
};

extern void ChatChangeAdmin(CChatDoc *doc, const char *szNickname, int sets, int unsets);
extern void IgnoreUser(const char *, const char *, BOOL, BOOL);
extern void ShowIdentity(const char *nick, const char *user, const char *host);


SHORT NGetCmd(CHAR* szCmd)
{
	ASSERT(szCmd);
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


void ParseIt(const char *szMessage, PIRCPARSE pParse, BOOL bDoubleQuotes /*=FALSE*/)
{
	// parse prefix
	const char	*szStart = szMessage;
	char		*szBody, *szCurToken;
	char		szPrefixBuff[300];

	pParse->nick[0] = '\0';
	pParse->user[0] = '\0';
	pParse->machine[0] = '\0';
	pParse->lastString = NULL;
	pParse->uCode = 0;
	pParse->nArgs = 0;
	ZeroMemory(pParse->args, sizeof(CHAR*) * MAXARGS);
	ZeroMemory(pParse->nOffsets, sizeof(SHORT) * MAXARGS);

	if (*szMessage == ':')
	{	// there's a prefix
		szMessage++;					// don't include the colon
		pParse->bHasPrefix = TRUE;
		szBody = strchr(szMessage, ' ');
		ASSERT(szBody);					// messages must have a body
		int cbPrefixSize = szBody - szMessage;
		cbPrefixSize = min(cbPrefixSize, sizeof(szPrefixBuff)-1);
		strncpy(szPrefixBuff, szMessage, cbPrefixSize);
		szPrefixBuff[cbPrefixSize] = '\0';
		char *szStart = szPrefixBuff;
		char *szEnd = NULL;
		if (!CHANNELPREFIX(*szStart)) szEnd = strpbrk(szStart, "!@");	// parse snick (must be present)
		if (szEnd)
		{
			int nChars = szEnd - szStart;
			nChars = min(nChars, sizeof(pParse->nick)-1);
			strncpy(pParse->nick, szStart, nChars);
			pParse->nick[nChars] = '\0';
			if (*szEnd == '!')
			{	// parse user
				szStart = szEnd+1;
				szEnd = strchr(szStart, '@');
				if (szEnd)
				{
					int nChars = szEnd - szStart;
					nChars = min(nChars, sizeof(pParse->user)-1);
					strncpy(pParse->user, szStart, nChars);
					pParse->user[nChars] = '\0';
					nChars = sizeof(pParse->machine)-1;
					strncpy(pParse->machine, szEnd+1, nChars);			// nfield now pts to !, parse machine
					pParse->machine[nChars] = '\0';
				}
			}
		}
		else
			if (strlen(szPrefixBuff) < sizeof(pParse->nick))
				strcpy(pParse->nick, szPrefixBuff);
	}
	else
	{
		pParse->bHasPrefix = FALSE;
		szBody = UnConst(szMessage);
	}

	while (TRUE)
	{
		while (my_isspace(*szBody))
			szBody++;
		if (*szBody == ':')
		{
			szBody++;
end:		char *szEnd = strpbrk(szBody, "\r\n");
			if (!szEnd)
				szEnd = strchr(szBody, '\0');
			int cbLen = szEnd - szBody;
			if (pParse->lastString = (char*) malloc(cbLen+1))
			{
				strncpy(pParse->lastString, szBody, cbLen);
				pParse->lastString[cbLen] = '\0';
			}
			break;
		}
		char *szToken;
		if (bDoubleQuotes && *szBody == '\"')
		{
			szToken = GetToken1(szBody, &szBody, "\"\r\n", &szCurToken, FALSE /*bSkipInitialSeps*/);
			if (*szBody == '\"')	// skip the terminating double quote
			{
				szBody++;
				strcat(szToken, "\"");
			}
		}
		else
		{
			szToken = GetToken(szBody, &szBody, " \r\n", &szCurToken);
			if (!szToken)
				break;
		}
		pParse->nOffsets[pParse->nArgs] = szCurToken - szStart;
		pParse->args[pParse->nArgs++] = strdup(szToken);
		if (pParse->nArgs == MAXARGS)
		{
			TRACE("Too many parameters - Have to use lastString for remaining message.\n");
			goto end;
		}
	}

	// Now fill in uCode member
	if (pParse->args[0])
	{
		CHAR	ch = pParse->args[0][0];
		INT		i  = 0;
 		if (isdigit(ch))
		{
			// Result or Error Code
			do
			{
				pParse->uCode *= 10;
				pParse->uCode += (ch - '0');
				ch = pParse->args[0][++i];
			}
			while (isdigit(ch));
		}
	}
}


void FreeParse(PIRCPARSE pParse)
{
	if (pParse->lastString)
		free(pParse->lastString);
	for (SHORT i = 0; i < pParse->nArgs; i++)
		free(pParse->args[i]);
}


BOOL bSingleJoin(char *szAttedNick, void *pDoc, DWORD dwData)
{
	ASSERT(pDoc);
	CUserInfo* pui;
	AddAndExecute(new JoinEntry(pui = new CUserInfo(szAttedNick)), (CDocument *) pDoc);
	return pui == ((CChatDoc*) pDoc)->m_puiSelf;
}


void CSInString(char **pszString, const char *szChannelName = NULL, CChatDoc *doc = NULL) {
	ASSERT(*pszString);
	int iEncoding = (szChannelName && *szChannelName == '%') ? ENC_UTF8 : ENC_DBCS;
	if (doc && (doc->m_proto->m_dwModes & CM_MIC)) iEncoding = ENC_DBCS; // until MIC disappears
	if (!**pszString || (theApp.m_charSet == ANSI_CHARSET && iEncoding == ENC_DBCS)) return;
	char *szNewString = strdup(DecodeString(*pszString, iEncoding));
	free(*pszString);
	*pszString = szNewString;
}


void CSInPlace(char *szNick) {
	if (theApp.m_charSet == ANSI_CHARSET || *szNick == '\0') return;;
	char *szOldNick = strdup(szNick);
	CSInString(&szOldNick);
	strcpy(szNick, szOldNick);
	free(szOldNick);
}


void ParseChannelMode(CChatDoc *doc, const char *szFlags, const char *szArg2, const char *szArg3, CRoomInfo *pEnterRoom)
{
	ASSERT(doc);

	BOOL	bAdd = TRUE;
	DWORD	dwDelta = 0, addFlags = 0, subFlags = 0;

	while (*szFlags != '\0')
	{
		switch (*szFlags++)
		{
		case '+':
			bAdd = TRUE;
			dwDelta = 0;
			break;
		case '-':
			bAdd = FALSE;
			dwDelta = 0;
			break;
		case 'p':
			dwDelta |= CM_PRIVATE;
			break;
		case 's':
			dwDelta |= CM_HIDDEN;
			break;
		case 'i':
			dwDelta |= CM_INVITEONLY;
			break;
		case 't':
			dwDelta |= CM_TOPICHOST;
			break;
		case 'n':
			dwDelta |= CM_NOEXTERN;
			break;
		case 'm':
			dwDelta |= CM_MODERATED;
			break;
		case 'l':
			dwDelta |= CM_USERLIMIT;
			doc->m_proto->m_dwMaxUsers = bAdd ? atoi(szArg2) : 0;
			break;
		case 'k':
			dwDelta |= CM_CHANNELKEY;
			if (bAdd)
				doc->m_proto->m_strPassword = (szArg3 && *szArg3) ? szArg3 : szArg2;
			else
				doc->m_proto->m_strPassword = ""; // clear it out, since we check for changes in the value before setting
			break;
		case 'q':
		    if (bAdd)
				ChatChangeAdmin(doc, szArg2, UF_OWNER | UF_OPERATOR, 0);
			else
				ChatChangeAdmin(doc, szArg2, 0, UF_OWNER);
			break;
		case 'o':
			if (bAdd)
				ChatChangeAdmin(doc, szArg2, UF_OPERATOR, 0);
			else
				ChatChangeAdmin(doc, szArg2, 0, UF_OPERATOR);
			break;
		case 'v':
			if (bAdd)
				ChatChangeAdmin(doc, szArg2, UF_HASVOICE, 0);
			else
				ChatChangeAdmin(doc, szArg2, 0, UF_HASVOICE);
			break;
		case 'f':
			if (GetIrcProto()->IsIRCX())
			{   // +f means something else for non-IRCX servers
				dwDelta |= CM_NOFORMAT;
				if (bAdd)
				{
					theApp.m_bSaveViewMode = FALSE;
					doc->OnViewText();
				}
			}
			break;
		case 'y':
			dwDelta |= CM_MIC;
			if (bAdd && pEnterRoom)
				FixMICChannelName(doc, pEnterRoom);
			break;
		}
		if (bAdd)
			addFlags |= dwDelta;
		else
			subFlags |= dwDelta;
	}

	doc->m_proto->m_dwModes |= addFlags;
	doc->m_proto->m_dwModes &= ~subFlags;

	void UpdateSpectators(CChatDoc *doc, BOOL moderated);
	if ((addFlags | subFlags) & CM_MODERATED)
	{
		UpdateSpectators(doc, doc->m_proto->m_dwModes & CM_MODERATED);
		// REGISB 05/04/98 - Bug 2459 - code ready to be checked in if we decide to 
		// send a # Appears as when channel becomes non-moderated.
		// if (subFlags & CM_MODERATED)
		// 	doc->m_proto->ChatAnnounceNewAvatar(GetMyCharacter(), MyAvatarURL());  // announce avatar to room when the moderated flag goes away
	}
}


void GetBanString(const char *szUserName, const char *szHostName, CString& strBan) {
	ASSERT(szUserName);
	ASSERT(szHostName);
	if (!GetIrcProto()->IsIRCX() || *szUserName == '~')
		strBan.Format("*!*@%s", szHostName);  // not authenticated
	else
		strBan.Format("*!%s@%s", szUserName, szHostName);
}


CIrcSocket::CIrcSocket(void)
{
    // Initialize SSPI related attributes
	m_pFuncTbl		= NULL;
	m_hSecLib		= NULL;
	m_bCredential	= FALSE;
	m_bContext		= FALSE;

	m_pszUserName 	= NULL;
	m_pszPassword 	= NULL;

	m_szInput		= NULL;
	m_szOutput2		= NULL;
	m_szMessage		= NULL;
	m_nAuthenticationType = authtypeNone;	// No authentication by default
	Reset();
}


CIrcSocket::~CIrcSocket(void)
{
	if (m_szInput)
		delete [] m_szInput;

	if (m_szOutput2)
		delete [] m_szOutput2;

	if (m_szMessage)
		delete [] m_szMessage;

	// Release security packages in arrays
	m_rgszSvrSecuPack.RemoveAll();
	m_rgszUsrSecuPack.RemoveAll();

    // Release the SSPI resources if allocated.
	CloseSSPI();

	if (m_hSecLib)
		FreeLibrary(m_hSecLib);	// REGISB: UmeshM puts in Chatsock that this call produces a GPF on Win95

	free (m_pszUserName);
	free (m_pszPassword);
}


void CIrcSocket::CloseSSPI(void)
{
    // Release the SSPI resources if allocated.
    if (m_pFuncTbl)
	{
		if (m_bContext)
			(*(m_pFuncTbl->DeleteSecurityContext))(&m_hContext);
        if (m_bCredential)
		    (*(m_pFuncTbl->FreeCredentialHandle))(&m_hCredential);
	}
	m_bContext = FALSE;
	m_bCredential = FALSE;
}


void CIrcSocket::Reset(void)
{
	m_nSecuPackIndex= -1;
	m_bIrcXServer	= FALSE;
	m_bRegistered	= FALSE;
	m_bAnonAllowed	= FALSE;
	m_bAuthFailed	= FALSE;
	m_bJustSentModeIsIrcX = FALSE;
	m_rgszSvrSecuPack.RemoveAll();
	if (m_szInput)
		*m_szInput = '\0';
}


HRESULT CIrcSocket::HrInitAlloc(SHORT nMaxIOBuff)
{
	if (m_szInput)
	{
		delete [] m_szInput;
		m_szInput = NULL;
	}

	if (m_szOutput2)
	{
		delete [] m_szOutput2;
		m_szOutput2 = NULL;
	}

	if (m_szMessage)
	{
		delete [] m_szMessage;
		m_szMessage = NULL;
	}

	m_szInput   = new CHAR[nMaxIOBuff+1];
	m_szOutput2 = new CHAR[nMaxIOBuff+1];
	m_szMessage = new CHAR[nMaxIOBuff+1];

	if (m_szInput && m_szOutput2 && m_szMessage && cui.bAllocOutBuff((MAX_FORMATTINGPERBYTE+1)*MAX_INPUTLEN+MAX_COMMAND))
	{
		*m_szInput = *m_szOutput2 = *m_szMessage = g_chEOS;

		m_nMaxMsgLength	= nMaxIOBuff;
		return NOERROR;
	}
	else
	{
		m_nMaxMsgLength	= 0;
		return E_OUTOFMEMORY;
	}
}

BOOL 
CIrcSocket::PromptForPassword(
LPCSTR pszUserName,
BOOL bSaveInSettings)
{
	CChatServerGroup* pGroup = theApp.m_SrvConnector.GetConnectingServerGroup ();
	CChatServer* pServer = theApp.m_SrvConnector.GetConnectingServer ();
	BOOL bRememberPassword = pServer != NULL ? pServer->m_bRememberPassword : FALSE;
	CChatPasswordDialog dlg (GetMyPhysicalServer (), pszUserName, bRememberPassword);
	if (theApp.DoModalDlg (&dlg) == IDOK)
	{
		free (m_pszPassword);
		m_pszPassword = strdup (dlg.m_strPassword);
		if (bSaveInSettings && pServer != NULL && pGroup != NULL)
		{
			free (pServer->m_pszPassword);
			pServer->m_pszPassword = strdup (m_pszPassword);
			pServer->m_bRememberPassword = dlg.m_bRememberPassword;
			pServer->m_nAuthenticationType = authtypePlainText;
			HKEY hkeyGroup = CChatServiceList::GetRegistryKey (CHATSVC_HKEY_SRVGROUP, pGroup->m_pszName);
			if (hkeyGroup)
			{
				pServer->WriteToRegistry (hkeyGroup);
				CChatServiceList::ReleaseRegistryKey (hkeyGroup);
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


HRESULT CIrcSocket::HrModeIsIrcXFailure()
{
	HRESULT hr = NOERROR;

	if (m_bJustSentModeIsIrcX)
	{
		POSITION	pos;
		CCQuery*	pQuery = m_queries.FindQuery(ctModeIsIrcX, &pos);

		if (pQuery)
		{
			ASSERT(pos);
			m_queries.FreeRemoveAt(pos);
		}

		// Don't want to expose this error to the user, it comes from the MODE ISIRCX\r\n command on an IRC server
		ASSERT(m_bIrcXServer == FALSE);
		// Login Time, on IRC Server, anonymously
		hr = HrIrcLogin(FALSE, (CHAR*) GetMyName(), m_pszUserName, (CHAR*) GetMyRealName(), m_pszPassword);
		ASSERT(NOERROR == hr);
		m_bJustSentModeIsIrcX = FALSE;
		::AfxGetMainWnd()->KillTimer(ID_ISIRCXTIMEOUT);
	}
	return hr;
}


//
//	CHAR	*szNickname			IN *your nickname. Required.
//	CHAR	*szUserName			IN *user name to use. Required.
//	CHAR	*szRealName			IN *real name to use (description). Not Required.
//	CHAR	*szPassword			IN *password. Not Required.
//								On authenticated login, you will be prompted by the security
//								dll if szUserName or szPassword are NULL
//
HRESULT 
CIrcSocket::HrIrcLogin(
BOOL bIRCX, 
CHAR *szNickname, 
CHAR *szUserName, 
CHAR *szRealName, 
CHAR *szPassword, 
BOOL bPromptForPassword)
{
	ASSERT(szNickname);

	if (szUserName == NULL)
	{
		szUserName = (CHAR *)GetMyUserName ();
	}

	// Remove spaces.
	char szUser[64];
	LPSTR pszDest, pszSrc;
	for (pszDest = szUser, pszSrc = (szUserName && *szUserName) ? szUserName : szNickname; *pszSrc; pszSrc++)
	{
		if (IsDBCSLeadByte (*pszSrc))
		{
			*(pszDest++) = *(pszSrc++);
			*(pszDest++) = *pszSrc;
		}
		else if (*pszSrc != ' ')
		{
			*(pszDest++) = *pszSrc;
		}
	}
	*pszDest = '\0';

	if ((szPassword == NULL || *szPassword == '\0') && bPromptForPassword && 
			m_nAuthenticationType != authtypeNone)
	{
		PromptForPassword (szUser, TRUE);
		szPassword = m_pszPassword;
	}

	// need to add EncodeString calls...

	// Send the PASS and NICK command, then construct the USER command
	if (szPassword && *szPassword)
	{
		sprintf(GetOutBuff(), "PASS %s\r\n", szPassword);
		GetIrcProto()->SendMessageText(GetOutBuff());
	}

	GetIrcProto()->ChatChangeNick(szNickname);

	if (!m_bRegistered)
	{
		CHAR szMachineName[127];
		if (SOCKET_ERROR == gethostname(szMachineName, sizeof(szMachineName)))
			strcpy(szMachineName, g_szNoMachine);

		// Send the USER command
		sprintf(GetOutBuff(), "USER %s %s . :%s\r\n", szUser, szMachineName, szRealName);
		GetIrcProto()->SendMessageText(GetOutBuff());

		// From now on we are registered until we disconnect
		m_bRegistered = TRUE;
	}

	// If a username and password is specified, try to use it to gain operator privileges.
	if (!bIRCX && m_nAuthenticationType == authtypePlainText && 
		m_pszUserName && *m_pszUserName && m_pszPassword && *m_pszPassword)
	{
		HrIrcSetOper (m_pszUserName, m_pszPassword);
	}

	return NOERROR;
}


HRESULT CIrcSocket::HrIrcXLogin(BOOL bForceNextPackage)
{
	// Trying an authentication on an IRCX server
	const char *szPackAvail;
	static BOOL	bLoopPwd = FALSE;

	// If the user does not force an authentication and the server allows anynomous
	// logins, then we just do a regular IRC login
	if (m_bAnonAllowed && (m_nAuthenticationType == authtypeNone || m_nAuthenticationType == authtypePlainText))
		return HrIrcLogin(TRUE, (CHAR*) GetMyName(), m_pszUserName, (CHAR*) GetMyRealName(), m_pszPassword);

	if (bForceNextPackage || !bLoopPwd)
		// Go to the next security package
		m_nSecuPackIndex++;

	// The user wants to force an authentication
	if (m_nAuthenticationType == authtypeCustomPackages)
	{
		// We try the user specified packages in sequence
		if (m_nSecuPackIndex <= m_rgszUsrSecuPack.GetUpperBound())
		{
			LPCTSTR szPackToTry = (LPCTSTR) m_rgszUsrSecuPack.GetAt(m_nSecuPackIndex);
			if (stricmp(g_szAnon, szPackToTry))
			{
				// User wants to try a specific package
				// Is this package provided by the server?
				BOOL bAvailable = FALSE;
				for (SHORT nSecuPackIndex = 0; nSecuPackIndex <= m_rgszSvrSecuPack.GetUpperBound(); nSecuPackIndex++)
					if (!_tcsicmp(szPackToTry, (LPCTSTR) m_rgszSvrSecuPack.GetAt(nSecuPackIndex)))
					{
						bAvailable = TRUE;
						szPackAvail = (LPCTSTR) m_rgszSvrSecuPack.GetAt(nSecuPackIndex);
						break;
					}
				if (bAvailable)
				{
					bLoopPwd = !_tcsicmp(szPackAvail, g_szMSN) || !_tcsicmp(szPackAvail, g_szDPA);
					if (FAILED(HrAuthenticate((CHAR*) GetMyUserName(), NULL /* szPassword */, (CHAR*) szPackAvail)))
						// Try next package
						return HrIrcXLogin(TRUE /*bForceNextPackage*/);
					else
						return NOERROR;
				}
				else
					// Try next package
					return HrIrcXLogin(TRUE /*bForceNextPackage*/);
			}
			else
			{
				// User wants to try anonymous
				if (m_bAnonAllowed)
					return HrIrcLogin(TRUE, (CHAR*) GetMyName(), m_pszUserName, (CHAR*) GetMyRealName(), m_pszPassword);
				else
					// Try the next package
					return HrIrcXLogin(TRUE /*bForceNextPackage*/);
			}
		}
		else
		{
			// No more packages available
			goto failure;
		}
	}
	else
	{
		// We try all the server packages in sequence, and then anonymous if allowed
		if (m_nSecuPackIndex <= m_rgszSvrSecuPack.GetUpperBound())
		{
			szPackAvail = (LPCTSTR) m_rgszSvrSecuPack.GetAt(m_nSecuPackIndex);
			bLoopPwd = !_tcsicmp(szPackAvail, g_szMSN) || !_tcsicmp(szPackAvail, g_szDPA);
			if (FAILED(HrAuthenticate((CHAR*) GetMyUserName(), NULL /* szPassword */, (CHAR*) szPackAvail)))
				// Try the next package
				return HrIrcXLogin(TRUE /*bForceNextPackage*/);
			else
				return NOERROR;
		}
		else
		{
			// No more packages available
			// Try anonymous if allowed
			if (m_bAnonAllowed)
				return HrIrcLogin(TRUE, (CHAR*) GetMyName(), m_pszUserName, (CHAR*) GetMyRealName(), m_pszPassword);
			else
				goto failure;
		}
	}

failure:
	// Authentication failed, report error

	// Close the connection
	OnClose(0);

	// Display appropriate error message
	CString strMesg;
	strMesg.LoadString(ID_ERR_NOAUTH);
	AfxMessageBox(strMesg);

	// New connection dialog
	AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
	return NOERROR;
}


HRESULT CIrcSocket::HrIrcSetOper(CHAR *szUserName, CHAR *szPassword)
{
	if (szPassword == NULL || *szPassword == '\0')
	{
		if (!PromptForPassword (szUserName, FALSE))
			return S_FALSE;
		szPassword = m_pszPassword;
	}
	
	sprintf (GetOutBuff (), "OPER %s %s\r\n", szUserName, szPassword);
	GetIrcProto()->SendMessageText(GetOutBuff ());

	return NOERROR;
}


HRESULT CIrcSocket::HrAuthenticate(CHAR *szUserName, CHAR *szPassword, CHAR *szSecurityPackage)
{
    SEC_WINNT_AUTH_IDENTITY AuthData, *pAuthData;
    INIT_SECURITY_INTERFACE	addrProcISI;
    DWORD					dwSecStatus;
    DWORD					dwLifeTime;

    //  If this is the first authentication attempt then load the SECURITY.DLL and obtain
    //  the security entry point.
	if (!m_pFuncTbl)
	{
        OSVERSIONINFO VerInfo;

        //  Find out which security DLL to use, depending on whether we are on NT or Win95
        VerInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);

		if (!GetVersionEx(&VerInfo))
			return GetLastError();	// REGISB: Might want to have a custom error code

		//  Load the platform specific version of the security dll.
        if (VerInfo.dwPlatformId == VER_PLATFORM_WIN32_WINDOWS)
			m_hSecLib = ::LoadLibrary(_T("SECUR32.DLL"));
        else
			m_hSecLib = ::LoadLibrary(_T("SECURITY.DLL"));

		//  If the security component is not found then fail (this should never happen).
		if (!m_hSecLib)
			return GetLastError();	// REGISB: put correct error code here

		//  Retrieve the security entrypoint.
		addrProcISI = (INIT_SECURITY_INTERFACE) GetProcAddress(m_hSecLib, SECURITY_ENTRYPOINT);       
		if (!addrProcISI)
		{
			FreeLibrary(m_hSecLib);	// REGISB:  UmeshM puts in Chatsock that this call produces a GPF on Win95
			m_hSecLib = NULL;
			return E_FAIL;	// REGISB: put correct error code here
		}

		// Get the SSPI function table
		m_pFuncTbl = (*addrProcISI)();
		if (!m_pFuncTbl)
		{
			FreeLibrary(m_hSecLib); // REGISB:  UmeshM puts in Chatsock that this call produces a GPF on Win95
			m_hSecLib = NULL;
			return E_FAIL;	// REGISB: put correct error code here
		}
	}

    //  If credentials from a previous authentication is still allocated, release.
	CloseSSPI();

    //  If the caller provided a username and password, use them over the default.
    if (szUserName && szPassword)
    {
        AuthData.User = (PBYTE) szUserName;
        AuthData.UserLength = lstrlen(szUserName);
        AuthData.Password = (PBYTE) szPassword;
        AuthData.PasswordLength = lstrlen(szPassword);
        AuthData.Domain = NULL;
        AuthData.DomainLength = 0;
        AuthData.Flags = SEC_WINNT_AUTH_IDENTITY_ANSI;
        pAuthData = &AuthData;
    }
    else
        pAuthData = NULL;

    //  Get a credential handle for this client to use in future SSPI calls.
    //      SEC_E_SECPKG_NOT_FOUND  080090305  - SSPI package not installed.
    dwSecStatus = (*(m_pFuncTbl->AcquireCredentialsHandle)) (NULL, szSecurityPackage, 
                      SECPKG_CRED_OUTBOUND,  NULL, pAuthData, NULL, NULL, 
                      &m_hCredential, &dwLifeTime);
    if (dwSecStatus != NO_ERROR)
        return E_FAIL;	// REGISB: put correct error code here

    //  Remember that a valid credential is available.
	m_bCredential = TRUE;

    //  Send the initial authentication message.
	return HrGenerateAndSendAuthMsg(NULL, szSecurityPackage);
}


//+-------------------------------------------------------------------------------------------
//
//  Method:     HrGenerateAndSendAuthMsg(szBlob, szSecurityPackage)
//
//  Synopsis:   This function calls InitializeSecurityContext to generate 
//              an authentication message and sends it to the ICP server.  
//              It generates different authentication messages depending on 
//              whether there's security token from the server to be used 
//              as input message or not (i.e. whether szBlob is NULL).
//
//  Args:       [szBlob] -- Pointer to blob from the server.
//
//--------------------------------------------------------------------------------------------
HRESULT CIrcSocket::HrGenerateAndSendAuthMsg(CHAR *szBlob, CHAR *szSecurityPackage)
{
    PCtxtHandle		phCurrContext;
    SecBufferDesc	inSecDesc, outSecDesc;
    SecBuffer		inSecBuffer, outSecBuffer;
    PSecBufferDesc	pInSecDesc;
    ULONG			ulContextReq;
    ULONG			ulContextAttrib;
    DWORD			dwExpireTime;
    DWORD			dwStatus;
    BYTE			pbBuffer[4096];
	CHAR			*szStr;
	UINT			cbBlob;
	BYTE			*pb = NULL;
	INT				cb = 0;
	BOOL			bLoop = FALSE;

    if (szBlob == NULL)
    {
        // This is the first time this HrGenerateAndSendAuthMsg() is called, so the
        // message generated will be a NEGOTIATE_MSG.
		ASSERT(!m_bContext);
        phCurrContext = NULL;
        pInSecDesc = NULL;
    }
    else
    {
		if (!bStringToData(szBlob, (PBYTE) szBlob, &cbBlob))
			return E_FAIL;

        // Since we have received a CHALLENGE_MSG from the server, we should
        // generate a AUTHENTICATE_MSG to send to server.
        phCurrContext = &m_hContext;

        // Setup API's input security buffer to pass the client's negotiate
        // message to the SSPI.
        inSecDesc.ulVersion = 0;
        inSecDesc.cBuffers = 1;
        inSecDesc.pBuffers = &inSecBuffer;

        inSecBuffer.cbBuffer = cbBlob;
        inSecBuffer.BufferType = SECBUFFER_TOKEN;
        inSecBuffer.pvBuffer = (PVOID) szBlob;

        pInSecDesc = &inSecDesc;
    }

    // Setup API's output security buffer for receiving challenge message
    // from the SSPI.
    // Pass the client message buffer to SSPI via pvBuffer.

    outSecDesc.ulVersion = 0;
    outSecDesc.cBuffers = 1;
    outSecDesc.pBuffers = &outSecBuffer;

    outSecBuffer.cbBuffer = sizeof(pbBuffer);
    outSecBuffer.BufferType = SECBUFFER_TOKEN;
    outSecBuffer.pvBuffer = pbBuffer;

    ulContextReq = ISC_REQ_CONFIDENTIALITY | ISC_REG_USE_SESSION_KEY;

	// First try using an exisiting session key
	if (m_bAuthFailed)
	{
		ulContextReq |= ISC_REQ_PROMPT_FOR_CREDS;
		if (szBlob)
			m_bAuthFailed = FALSE;
		bLoop = TRUE;
	}

tryAgain:
    // Generate a negotiate/authenticate message to be sent to the server.
    dwStatus = (*(m_pFuncTbl->InitializeSecurityContext)) (
                                &m_hCredential,							// phCredential
                                phCurrContext,							// phContext
                                (CHAR*)GetMyPhysicalServer (),       	// pszTargetName
                                ulContextReq,							// fContextReq
                                0L,										// reserved1
                                SECURITY_NATIVE_DREP,					// TargetDataRep
                                pInSecDesc,								// pInput
                                0L,										// reserved2
                                &m_hContext,							// phNewContext
                                &outSecDesc,							// pOutput negotiate msg
                                &ulContextAttrib,						// pfContextAttribute
                                &dwExpireTime);							// ptsLifeTime

	if (dwStatus == SEC_E_NO_CREDENTIALS && !bLoop)
	{
		// No existing credentials. Prompt for credentials
		bLoop = TRUE;
		ulContextReq |= ISC_REQ_PROMPT_FOR_CREDS;
		goto tryAgain;
	}

	if (FAILED(dwStatus))
	{
		m_bAuthFailed = FALSE;
		return E_FAIL;
	}

	m_bContext = TRUE;

	if (!bDataToString((PBYTE)(outSecBuffer.pvBuffer), (UINT) outSecBuffer.cbBuffer, &szStr, TRUE))
		return E_OUTOFMEMORY;

	sprintf(GetOutBuff(), "AUTH %s %s :%s\r\n", szSecurityPackage, szBlob ? "S" : "I", szStr);
	GetIrcProto()->SendMessageText(GetOutBuff());

	delete [] szStr;

    return NOERROR;
}


void CIrcSocket::OnReceive(int nErrorCode) {
	// TRACE("Entering OnReceive (code = %d).\n", nErrorCode);

	if (!nErrorCode) {
		char *startPtr = strchr(m_szInput, '\0');
		int space = m_szInput + m_nMaxMsgLength - startPtr;
		int nRead = Receive(startPtr, space);
		if (SOCKET_ERROR == nRead)
		{
			TRACE("Receive failed with error: %d\n", GetLastError());
			return;
		}
		startPtr[nRead] = '\0';
		char *eoc = strchr(m_szInput, '\n');
		while (eoc) {
			eoc++;
			int comLen = eoc - m_szInput;
			strncpy(m_szMessage, m_szInput, comLen);
			m_szMessage[comLen] = '\0';

			// now move rest of message forward
			char *eob = strchr(m_szInput, '\0');
			int nRest = eob - eoc;
			strncpy(m_szInput, eoc, nRest);
			m_szInput[nRest] = '\0';

			TRACE("Got message: %.100s\n", m_szMessage);
			ProcessMessage(m_szMessage);   // handle the message (*After clearing it from the buffer!!!)
			eoc = strchr(m_szInput, '\n'); // must do this after process message, since code is reentrant (but single threaded)
		}
	}
	// TRACE("Leaving OnReceive.\n");
}


void CIrcSocket::OnConnect(int nErrorCode) {
	TRACE("Connecting (code = %d)...\n", nErrorCode);
	if (nErrorCode) {  // couldn't connect
		GetIrcProto()->SetConnectionStatus(CX_DISCONNECTED);
		CString strMesg;
		strMesg.LoadString(ID_ERR_CONNECT);
//		setupDlg.nWhatFailed = PORT;
		VERIFY(ReplaceToken(strMesg, CString("%1"), GetMyServer()));
		AfxMessageBox(strMesg);
		InitializeServerConnection(&g_enterInfo, &g_bCXPrompt);
		return;
	}
	// got a connection!
	// Is this an IRCX server?
	ASSERT(GetIrcProto());
	VERIFY(GetIrcProto()->bExecuteQuery(qpIsIrcX, ctModeIsIrcX, dtMax, NULL, "", ""));
	m_bJustSentModeIsIrcX = TRUE;
	::AfxGetMainWnd()->SetTimer(ID_ISIRCXTIMEOUT, ISIRCXTIMEOUT, NULL);
}


void CIrcSocket::OnClose(int nErrorCode)
{
	TRACE("Closing socket on error %d.\n", nErrorCode);
	
	// Closing during connection? Then resume trying to connect to other servers.
	if (theApp.m_SrvConnector.IsConnecting () && theApp.m_SrvConnector.GetNumServers () > 1)
	{
		ChatServerDisconnect (FALSE, TRUE);
		theApp.ResumeConnection ();
	}
	else
	{
		// REGISB: added 11/13/97 because user would be disconnected for flooding without any notification
		// 0   				The function executed successfully.
		// WSAENETDOWN		The Windows Sockets implementation detected that the network subsystem failed.
		// WSAECONNRESET	The connection was reset by the remote side.
		// WSAECONNABORTED	The connection was aborted due to timeout or other failure.
		ChatServerDisconnect(TRUE /*bCheckRules*/);
		AfxMessageBox(IDS_CONNECTION_DROPPED);
	}
}


void CIrcProto::OnLogin()
{
	CChatService* AddToServerList(const char *);

	StopIdentD();

	if (theApp.m_dynaRules.bDaemonNeeded())
		VERIFY(theApp.m_dynaRules.bStartRulesDaemon(g_uRulesDaemonShortElapse, TRUE /*bForceReset*/));

	if (theApp.m_dynaNotifs.bDaemonNeeded())
		VERIFY(theApp.m_dynaNotifs.bStartNotifsDaemon(g_uNotifsDaemonShortElapse, TRUE /*bForceReset*/));

	AddToServerList(GetMyServer());
	theApp.m_bInSearch = FALSE;			// can now search again (in case disconnected during last search
	SetVisibility(theApp.m_flags1 & F1_USERVISIBLE);

	int iAction;
	if (theApp.m_bLoadURL)
	{
		iAction = CA_JOINROOM;
		theApp.m_bLoadURL = FALSE;	// Don't do this on a reconnect.
	}
	else
		iAction = theApp.m_iOnConnectAction;
	switch (iAction)
	{
	case CA_JOINROOM:
		ChatJoinChannel(g_enterInfo);
		break;
	case CA_ROOMLIST:
		theApp.OnChatroomList();
		break;
	}
}


void CIrcSocket::ProcessMessage(char *szLine) {
	IRCPARSE	parse;
	CIrcPrint	ircPrint;
	CString		strLine;

#ifdef IRCLOG    // creates IRC message log **ONLY FOR LOCAL DEBUGGING**
	if (!theApp.m_fileIn) {
		static FILE *fp = NULL;
		if (!fp)
			fp = fopen("irc.txt", "w");
		if (fp) {
			fprintf(fp, "%s\n", szLine);
			fflush(fp);
		}
	}
#endif IRCLOG

	ParseIt(szLine, &parse);
	if (parse.nArgs <= 0) {
		ASSERT(0);
		ParseIt(szLine, &parse);
	}
	if (parse.nArgs <= 0) {
		ASSERT(0);		// should never happen?
		return;
	}

	if (0 == parse.uCode)
	{
		// A real command
		HandleCommand(strLine, szLine, &parse, &ircPrint);
	}
	else
	{
		// Result or Error Code
		if (bIsErrorCode(parse.uCode))
			HandleErrorCode(szLine, &parse, &ircPrint);
		else
			HandleResultCode(strLine, szLine, &parse, &ircPrint);
	}

	AddToStatus(ircPrint, szLine);

	FreeParse(&parse);
}


void CIrcSocket::HandleCommand(CString& strLine, char *szLine, PIRCPARSE pParse, CIrcPrint *pIrcPrint)
{
	ASSERT(pParse);
	ASSERT(pIrcPrint);

	SHORT	nCmd = NGetCmd(pParse->args[0]);
	if (-1 == nCmd)
	{
		ASSERT(FALSE);
		// Unknown IRC/X command
		return;
	}

	ASSERT(nCmd < cmdidMax);

	switch (nCmd)
	{
		default:
		{
			#ifdef DEBUG
				TRACE("Unexpected command: sender nick = %s, mach = %s, user = %s, command = %s, last string = %s\n", 
				pParse->nick ? pParse->nick : "", 
				pParse->machine ? pParse->machine : "", 
				pParse->user ? pParse->user : "", 
				pParse->args[0] ? pParse->args[0] : "",
				pParse->lastString ? pParse->lastString : "");
				ASSERT(FALSE);
			#endif // DEBUG
			break;
		}

		case cmdidReply:
		case cmdidRequest:
		{
			#ifdef DEBUG
				TRACE("Untreated command: sender nick = %s, mach = %s, user = %s, command = %s, last string = %s\n", 
				pParse->nick ? pParse->nick : "", 
				pParse->machine ? pParse->machine : "", 
				pParse->user ? pParse->user : "", 
				pParse->args[0] ? pParse->args[0] : "",
				pParse->lastString ? pParse->lastString : "");
			#endif // DEBUG
			break;
		}

		case cmdidAuth:
		{
			// We are in the middle of an authentication
			TRACE("Got an AUTH!\n");
			if (pParse->nArgs >= 3)
			{
				pIrcPrint->SetFormat(PT_NONE);	// don't want to display these in the Status Window
				if ('S' == pParse->args[2][0])
				{
					// AUTH NTLM S :blob...
					if (E_ABORT == HrGenerateAndSendAuthMsg(pParse->lastString, pParse->args[1]))
						// User cancelled the authentication with the current package.
						// Let's try the next one...
						HrIrcXLogin(TRUE /*bForceNextPackage*/);
				}
				else
				{
					// AUTH NTLM * REGISB@REDMOND 0
					ASSERT(!stricmp("*", pParse->args[2]));
					// We're finally authenticated
					m_bRegistered = TRUE;
					// This finalizes the login
					HRESULT hr = HrIrcLogin(TRUE, (CHAR*) GetMyName(), NULL /*szUserName*/, NULL /*szRealName*/, NULL /*szPassword*/, FALSE);
					ASSERT(NOERROR == hr);
				}
			}
			break;
		}

		case cmdidClone:
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 1, TRUE);
			break;
		}

		case cmdidCreate:
		{
			if (pParse->nArgs >= 3)
			{
				if (bProcessAddChannel(pParse->args[1], NewDefaultProto(NULL), &g_nCXKeepServer, &g_bCXPrompt))
				{
					ASSERT(currentRoom);
					strCurrentChannelTopic = "";
					dwCurrentChannelMode = 0;
					dwCurrentUserLimit = 0;

					CCQuery* pQuery = new CCQuery(qpInitialNames, ctNames, dtMax, NULL, strCurrentChannel, "", FALSE /*bCreatePrUserMatch*/);

					if (pQuery)
						m_queries.bAddQuery(pQuery);

					pQuery = new CCQuery(qpInitialTopic, ctTopic, dtMax, NULL, strCurrentChannel, "", FALSE /*bCreatePrUserMatch*/);

					if (pQuery)
						m_queries.bAddQuery(pQuery);

					ASSERT(GetIrcProto());
					VERIFY(GetIrcProto()->bExecuteQuery(qpInitialMode, ctGetChannelMode, dtMax, NULL, pParse->args[1], ""));
					VERIFY(GetIrcProto()->bExecuteQuery(qpInitialWho, ctWho, dtMax, NULL, pParse->args[1], ""));

					if (m_bIrcXServer)
						VERIFY(GetIrcProto()->bExecuteQuery(qpJoinBackUrl, ctPropGet, dtMax, NULL, pParse->args[1], ""));
				}
				pIrcPrint->SetFormat(PT_NONE);
			}
			break;
		}

		case cmdidData:
		{
			TRACE("Got a Data message! (snick = %s, mach = %s, user = %s)\n", pParse->nick, pParse->machine, pParse->user);

			pIrcPrint->SetFormat(PT_NONE);
			// CCUDI1 = Comic Chat User Display Info version 1
			if (pParse->lastString && 
				pParse->lastString[0] == '#' && 
				pParse->nArgs >= 3 && 
				!strcmp(pParse->args[2], CCUDI1) && 
				*pParse->nick && 
				*pParse->user)
			{
				CChatDoc*	doc = NULL;
				BYTE		msgType;

				if (CHANNELPREFIX(pParse->args[1][0]))
				{
					doc = LookupDoc(pParse->args[1]);
					msgType = MT_CHANNELSEND | MT_DATA;
				}
				else
					msgType = MT_PRIVATEMSG | MT_DATA;

				if ((msgType & MT_PRIVATEMSG) || doc)
				{
					CString strID;

					if (*pParse->machine)
						strID.Format("%s@%s", pParse->user, pParse->machine);

					// interpret "# Appears as" message as being sent to all rooms they are a member of
					if (!doc && strncmp(((LPCTSTR)pParse->lastString)+1, APPEARSPREFIX, g_nAppearsAsLen) == 0)
					{
						POSITION pos = g_docs.GetHeadPosition();
						while (pos)
						{
							doc = (CChatDoc*) g_docs.GetNext(pos);
							CUserInfo *pui = LookupPui(pParse->nick, doc);
							if (pui && !pui->IsDeparted())
								OnDataMsg(doc, pParse->nick, strID, pParse->lastString, msgType);
						}
					}
					else
						OnDataMsg(doc, pParse->nick, strID, pParse->lastString, msgType);
				}
			}
			break;
		}

		case cmdidError:
		{
			if (pParse->lastString) {
				CSInString(&pParse->lastString);
				// Only show this if we are not in a multi-server connect.
				if (!theApp.m_SrvConnector.IsConnecting () || !theApp.m_SrvConnector.GetNumServers () == 1)
				{
					if (strstr(pParse->lastString, "No IRC clients"))
						AfxMessageBox(IDS_MICONLY);
					else
						AfxMessageBox(pParse->lastString);	// print the message verbatim (localization problem!!!)
					AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
				}
				else
					if (m_bJustSentModeIsIrcX)
					{
						pIrcPrint->SetFormat(PT_NONE);
						HrModeIsIrcXFailure();
					}
			}
			break;
		}

		case cmdidInvite:
		{
			if (pParse->lastString) {
				CSInPlace(pParse->user);  // necessary ?
				CString ident(pParse->user);
				ident += "@";
				ident += pParse->machine;
				OnInvite(pParse->nick, ident, pParse->lastString);
				pIrcPrint->SetFormat(PT_NONE);	// handled via popup
			}
			break;
		}

		case cmdidJoin:
		{
			TRACE("Got a JOIN!\n");
			ASSERT(pParse->lastString);

			CSInPlace(pParse->user);
			CString strIdent(pParse->user);
			strIdent += "@";
			strIdent += pParse->machine;

			if (stricmp(pParse->nick, GetMyNickName()))
			{				// Don't send or register self
				enumActions	rgaIDs[2] = { (enumActions) 1, aHighlightMessage };
				CDocument*	pDoc = LookupDoc(pParse->lastString);
				if (pDoc)	// else we just left this channel
				{
					theApp.m_dynaRules.bMatchAndApplyRules(eOnJoin, (enumActions*) rgaIDs, NULL, CString(GetMyServer()), CString(pParse->nick)+"!"+strIdent, CString(pParse->lastString), CString(""));
					char cHighlightType = -1;
					if (theApp.m_dynaRules.GetFlags() & g_wHighlight)
						cHighlightType = theApp.m_dynaRules.GetFlags() >> 8;
					AddAndExecute(new JoinEntry(new CUserInfo(pParse->nick, strIdent), FALSE, cHighlightType), pDoc);
					theApp.m_dynaRules.bMatchAndApplyRules(eOnJoin, NULL, (enumActions*) rgaIDs, CString(GetMyServer()), CString(pParse->nick)+"!"+strIdent, CString(pParse->lastString), CString(""));
				}
			}
			else
			{
				// REGISB 11/18/97 added if statement for Fix 4449
				if (bProcessAddChannel(pParse->lastString, NewDefaultProto(NULL), &g_nCXKeepServer, &g_bCXPrompt))
				{
					ASSERT(currentRoom);
					strCurrentChannelTopic = "";
					dwCurrentChannelMode = 0;
					dwCurrentUserLimit = 0;

					// REGISB added 11/07/97
					if (!theApp.m_nMyIdentLength)
					{
						theApp.m_nMyIdentLength = strIdent.GetLength() + 2; // + 2 for the ! and ~ signs 
						SetMyIdent(strIdent);
					}

					CCQuery* pQuery = new CCQuery(qpInitialNames, ctNames, dtMax, NULL, strCurrentChannel, "", FALSE /*bCreatePrUserMatch*/);

					if (pQuery)
						m_queries.bAddQuery(pQuery);

					pQuery = new CCQuery(qpInitialTopic, ctTopic, dtMax, NULL, strCurrentChannel, "", FALSE /*bCreatePrUserMatch*/);

					if (pQuery)
						m_queries.bAddQuery(pQuery);

					ASSERT(GetIrcProto());
					VERIFY(GetIrcProto()->bExecuteQuery(qpInitialMode, ctGetChannelMode, dtMax, NULL, pParse->lastString, ""));
					VERIFY(GetIrcProto()->bExecuteQuery(qpInitialWho, ctWho, dtMax, NULL, pParse->lastString, ""));

					if (m_bIrcXServer)
						VERIFY(GetIrcProto()->bExecuteQuery(qpJoinBackUrl, ctPropGet, dtMax, NULL, pParse->lastString, ""));
				}
			}
			pIrcPrint->SetFormat(PT_NONE);
			break;
		}

		case cmdidKick:
		{
			if (pParse->lastString && pParse->nArgs >= 3)
			{
				CChatDoc*	pDoc = LookupDoc(pParse->args[1]);
				CSInString(&pParse->lastString, pParse->args[1], pDoc);
				OnKick(pDoc, pParse->nick, pParse->args[2], pParse->lastString);
			}
			pIrcPrint->SetFormat(PT_NONE);		   // kicks only appear in channel window
			break;
		}
		
		case cmdidKnock:
		{
			pIrcPrint->SetFormat(PT_WHOLESTRING, szLine, RGB(0,0,0), 0, TRUE);
			break;
		}

		case cmdidMode:
		{
			if (pParse->nArgs >= 3)	// Channel mode change
			{
				const char *szFlags, *szArg2 = "", *szArg3 = "";
				
				szFlags = pParse->args[2];
				if (pParse->nArgs >= 4)
					szArg2 = pParse->args[3];
				if (pParse->nArgs >= 5)
					szArg3 = pParse->args[4];

				CChatDoc *doc = LookupDoc(pParse->args[1]);
				if (doc)
					ParseChannelMode(doc, szFlags, szArg2, szArg3, NULL);

				// Get oldest queued ctSetChannelMode query object
				POSITION		pos;
				CCQuery*		pQuery = m_queries.FindQuery(ctSetChannelMode, &pos);
				char*			szTmp;
				static MODECACH	mcLost = { NULL, NULL, MC_NONE };

				if (pQuery)
				{
					switch (pQuery->GetQueryPurpose())
					{
					case qpComSetChannelMode:
						ASSERT(pos);
						ASSERT(0 == stricmp(pQuery->GetChannelName(), pParse->args[1]));
						m_queries.FreeRemoveAt(pos);
						pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 1, TRUE);
						if (NULL != (szTmp = strstr(szFlags, "-o")) || NULL != strstr(szFlags, "-q"))
						{
							ZeroMemory((PVOID) &mcLost, sizeof(MODECACH));
							strncpy(mcLost.szChannelName, pParse->args[1], MAX_TOKEN-1);
							strncpy(mcLost.szNickname, szArg2, MAX_NICK-1);
							mcLost.byteStatus = szTmp ? MC_HOSTLOST : MC_OWNERLOST;
						}
						else
							mcLost.byteStatus = MC_NONE;
						break;
					default:
						ASSERT(FALSE);
					}
				}
				else
				{
					if (MC_NONE != mcLost.byteStatus && 
						!strcmp(mcLost.szChannelName, pParse->args[1]) && 
						!strcmp(mcLost.szNickname, szArg2))
					{
						if ((MC_HOSTLOST == mcLost.byteStatus && strstr(szFlags, "+q")) ||
							(MC_OWNERLOST == mcLost.byteStatus && strstr(szFlags, "+o")) ||
							(MC_OWNERLOST == mcLost.byteStatus && strstr(szFlags, "-o")))
							pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 1, TRUE);
						else
							pIrcPrint->SetFormat(PT_NONE);

						mcLost.byteStatus = MC_NONE;
					}
					else
						pIrcPrint->SetFormat(PT_NONE);
				}
			}
			else
				if (pParse->nArgs == 2)	// User mode change
				{
					// Our visibility could change for example
					if (0 == stricmp(pParse->args[1], GetMyNickName()))
					{
						// User mode change is for us
		
						// Get oldest queued ctSetUserMode query object
						POSITION	pos;
						CCQuery*	pQuery = m_queries.FindQuery(ctSetUserMode, &pos);
						BOOL		bDisplay = TRUE;

						if (pQuery)
						{
							ASSERT(pos);
							switch (pQuery->GetQueryPurpose())
							{
								case qpSetInvisible:
								case qpSetVisible:
								case qpComSetUserMode:
								{
									// REGISB this code should be turned into a generic function like ApplyIRCToOurUserMode
									// of MsChatPr if we decide to treat all user modes: a i s w o
									ASSERT(0 == pQuery->GetNicknameMask().CompareNoCase(GetMyNickName()));
									BOOL	bSet, bInvisibility = FALSE, bRemoveCell = (qpComSetUserMode == pQuery->GetQueryPurpose());
									LPTSTR	szModes = pParse->lastString; 
									ASSERT(szModes);
									while (*szModes)
									{
										switch (*szModes)
										{
											case '-':
												bSet = FALSE;
												break;
											case '+':
												bSet = TRUE;
												break;
											case 'i':
												theApp.m_flags1 = bSet ? (theApp.m_flags1 & ~F1_USERVISIBLE) : (theApp.m_flags1 | F1_USERVISIBLE);
												if (qpComSetUserMode != pQuery->GetQueryPurpose())
													bDisplay = FALSE;
												bRemoveCell = TRUE;
												break;
										}
										szModes++;
									}
									if (bRemoveCell)
										m_queries.FreeRemoveAt(pos);
									break;
								}
								default:
									ASSERT(FALSE);
							}
						}
						if (bDisplay)
							pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 1, TRUE);
						else
							pIrcPrint->SetFormat(PT_NONE);
					}
				}
			break;
		}

		case cmdidNick:
		{
			pIrcPrint->SetFormat(PT_NONE);
			if (pParse->lastString)
			{
				BOOL		bSetName = FALSE, bDisplayNewNickInStatusWnd;
				POSITION	pos;

				// Is our own nickname changing?
				bDisplayNewNickInStatusWnd = (0 == strcmp(pParse->nick, GetMyNickName()));

				pos = g_docs.GetHeadPosition();
				while (pos)
				{
					CChatDoc *doc = (CChatDoc*) g_docs.GetNext(pos);
					if (doc->m_proto->GetConnectionStatus() == CX_INCHANNEL)
					{
						CUserInfo *pui = LookupPui(pParse->nick, doc);
						if (pui && !pui->IsDeparted())
						{
							AddAndExecute(new NickEntry(pParse->nick, pParse->lastString), doc);
							bSetName = TRUE;
						}
					}
				}

				if (!bSetName)
					SetMyNameNick(pParse->lastString);

				if (bDisplayNewNickInStatusWnd)
				{
					strLine.Format(IDS_NOWKNOWNAS, GetMyScreenName());
					pIrcPrint->SetFormat(PT_WHOLESTRING, strLine, RGB(0,0,255), 0, TRUE);
				}
			}
			break;
		}

		case cmdidNotice:
		case cmdidPrivMsg:
		{
			pIrcPrint->SetFormat(PT_NONE);    // don't print these in status window

			TRACE("Got a PrivMsg! (snick = %s, mach = %s, user = %s)\n", pParse->nick, pParse->machine, pParse->user);

			if (pParse->lastString && pParse->nArgs >= 2)
				if (*pParse->nick && *pParse->user)
				{
					CChatDoc	*doc = NULL;
					BYTE		msgType;

					msgType = (cmdidPrivMsg == nCmd) ? MT_PRVMSG : MT_NOTICE;

					if (CHANNELPREFIX(pParse->args[1][0]))
					{
						doc = LookupDoc(pParse->args[1]);
						msgType |= MT_CHANNELSEND;
					}
					else
						msgType |= MT_PRIVATEMSG;

					if ((msgType & MT_PRIVATEMSG) || doc)
					{
						CString strID;
						if (*pParse->user && *pParse->machine)
							strID.Format("%s@%s", pParse->user, pParse->machine);

						// interpret "# Appears as" message as being sent to all rooms they are a member of
						if (!doc && pParse->lastString[0] == '#' && strncmp(((LPCTSTR)pParse->lastString)+1, APPEARSPREFIX, g_nAppearsAsLen) == 0)
						{
							POSITION pos = g_docs.GetHeadPosition();
							while (pos)
							{
								doc = (CChatDoc *)g_docs.GetNext(pos);
								CUserInfo *pui = LookupPui(pParse->nick, doc);
								if (pui && !pui->IsDeparted())
									OnTextMsg(doc, pParse->nick, strID, pParse->lastString, msgType);
							}
						}
						else
						{
							CSInString(&pParse->lastString, pParse->args[1], doc);
							OnTextMsg(doc, pParse->nick, strID, pParse->lastString, msgType);
						}
					}
				}
				else
					if (!*pParse->nick && !*pParse->user)
						pIrcPrint->SetFormat(PT_LASTSTRING, szLine, RGB(128,0,128));
			break;
		}

		case cmdidPart:
		{
			TRACE("Got a PART!\n");
			CChatDoc *pDoc = LookupDoc(pParse->args[1]);
			if ((stricmp(pParse->nick, GetMyNickName()) == 0))
			{
				GotPartChannel(pDoc);
				theApp.m_pExitingDoc = NULL;
			}
			else 
				if (pDoc)
				{
					enumActions	rgaIDs[2] = { (enumActions) 1, aHighlightMessage };
					CString		strIdent = CString(pParse->nick)+"!"+pParse->user+"@"+pParse->machine;

					theApp.m_dynaRules.bMatchAndApplyRules(eOnLeave, (enumActions*) rgaIDs, NULL, CString(GetMyServer()), strIdent, CString(pParse->args[1]), CString(""));
					char cHighlightType = -1;
					if (theApp.m_dynaRules.GetFlags() & g_wHighlight)
						cHighlightType = theApp.m_dynaRules.GetFlags() >> 8;
					AddAndExecute(new PartEntry(pParse->nick, cHighlightType), pDoc);
					theApp.m_dynaRules.bMatchAndApplyRules(eOnLeave, NULL, (enumActions*) rgaIDs, CString(GetMyServer()), strIdent, CString(pParse->args[1]), CString(""));
				}
			pIrcPrint->SetFormat(PT_NONE);
			break;
		}

		case cmdidPing:
		{
			sprintf(GetOutBuff(), "PONG :%s\r\n", pParse->lastString ? pParse->lastString : "");
			TRACE("%s", GetOutBuff());
			Send(GetOutBuff(), strlen(GetOutBuff()));
			pIrcPrint->SetFormat(PT_NONE);
			break;
		}

		case cmdidPong:
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 1, TRUE);
			break;
		}

		case cmdidProp:
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 2);		// may be overridden
			if (pParse->nArgs == 3 && pParse->lastString)
			{
				if (CHANNELPREFIX(pParse->args[1][0]))
				{
					CChatDoc* doc = LookupDoc(pParse->args[1]);
					// Also verify that it's an IRCX channel
					if (doc != NULL && doc->m_proto->IsIRCX ())
					{
						// Look for the CLIENT property being set, and handle it.
						if (!lstrcmp (pParse->args[2], "CLIENT"))
						{
							// Get oldest queued ctPropSet query object
							POSITION	pos;
							CCQuery*	pQuery = m_queries.FindQuery(ctPropSet, &pos);
							
							if (pQuery && pQuery->GetQueryPurpose() == qpSetClient)
							{
								ASSERT(pos);
								ASSERT(0 == stricmp(pQuery->GetChannelName(), pParse->args[1]));
								m_queries.FreeRemoveAt(pos);
								pIrcPrint->SetFormat(PT_NONE);
							}

							((CIrcProto *)doc->m_proto)->HandleClientDataChange (pParse->lastString);
						}
						if (!lstrcmp (pParse->args[2], "TOPIC"))
						{
							// Channel topic changed via PROP command
							CSInString(&pParse->lastString, pParse->args[1], doc);
							if (doc->m_proto->m_prgdwTopicFormatting)
								doc->m_proto->m_prgdwTopicFormatting->RemoveAll();
							else
								doc->m_proto->m_prgdwTopicFormatting = new CDWordArray;
							doc->m_proto->m_strTopic = SzControlLess(pParse->lastString, doc->m_proto->m_prgdwTopicFormatting);
						}
					}
				}
			}
			break;
		}

		case cmdidKilled:
		{
			// user successfully killed another one
			pIrcPrint->SetFormat(PT_WHOLESTRING, szLine, RGB(0,0,255), 0, TRUE);
			break;
		}

		case cmdidKill:
		case cmdidQuit:		// collapse w/ PART? 
		{
			LPCTSTR		szQuittingNick = (nCmd == cmdidQuit) ? pParse->nick : pParse->args[1];
			POSITION	pos = g_docs.GetHeadPosition();

			while (pos)
			{
				CChatDoc *pDoc = (CChatDoc*) g_docs.GetNext(pos);
				if (pDoc->GetConnectionStatus() != CX_INCHANNEL)
					break;
				CUserInfo *pui = LookupPui(szQuittingNick, pDoc);
				if (pui && !pui->IsDeparted())
					AddAndExecute(new PartEntry(szQuittingNick), pDoc);
			}
			pIrcPrint->SetFormat(PT_NONE);	// let's not display QUIT or KILL messages in the Status Window
			break;
		}

		case cmdidTopic:
		{
			if (pParse->nArgs >= 2 && pParse->lastString)
			{
				CString		strCtrlLessTopic;
				CDWordArray	rgdwFormattingTmp;
				CChatDoc*	pDoc = LookupDoc(pParse->args[1]);

				CSInString(&pParse->lastString, pParse->args[1], pDoc);

				strCtrlLessTopic = SzControlLess(pParse->lastString, &rgdwFormattingTmp);

				if (pDoc)
				{
					if (pDoc->m_proto->m_prgdwTopicFormatting)
						FreeAndNullFormatting(&pDoc->m_proto->m_prgdwTopicFormatting);

					pDoc->m_proto->m_prgdwTopicFormatting = CopyFormatting(&rgdwFormattingTmp);
					pDoc->m_proto->m_strTopic = strCtrlLessTopic;
				}

				// Get oldest queued ctTopic query object
				POSITION	pos;
				CCQuery*	pQuery = m_queries.FindQuery(ctTopic, &pos);

				if (pQuery)
				{
					ASSERT(pos);
					switch (pQuery->GetQueryPurpose())
					{
						case qpSetTopic:
							ASSERT(0 == strcmp(pParse->args[1], pQuery->GetChannelName()));
							break;
						default:
							ASSERT(FALSE);
					}
					m_queries.FreeRemoveAt(pos);
					pIrcPrint->SetFormat(PT_NONE);
				}
				else
				{
					strLine = CString(pParse->args[1]) + " :" + strCtrlLessTopic;
					PushFormattingOffsets(&rgdwFormattingTmp, strlen(pParse->args[1]) + 2);
					pIrcPrint->SetFormat(PT_WHOLESTRING, strLine, RGB(0,0,128), 0, TRUE);
					AddToStatus(*pIrcPrint, strLine, &rgdwFormattingTmp);
					pIrcPrint->SetFormat(PT_NONE);			// Don't display this a second time
				}

				rgdwFormattingTmp.RemoveAll();
			}
			break;
		}

		case cmdidWhisper:
		{
			CChatDoc *doc = LookupDoc(pParse->args[1]);
			if (doc && *pParse->nick) {
				CDWordArray talkTos;
				// compute talktos array, just in case message isn't cooked...
				void GetTalkTos(CChatDoc *doc, CDWordArray *talkTos, char *str);
				GetTalkTos(doc, &talkTos, pParse->args[2]);
				CSInString(&pParse->lastString, pParse->args[1], doc);
				OnTextMsg(doc, pParse->nick, "X", pParse->lastString, MT_PRIVATEMSG | MT_WHISPER, &talkTos); // for now, treated similarly to Private Message
			}
			break;
		}
	}
}


void CIrcSocket::HandleResultCode(CString &strLine, char *szLine, PIRCPARSE pParse, CIrcPrint *pIrcPrint)
{
	static CRoom	*sRoom;
	static BOOL		sbAddIt;

	ASSERT(pParse);
	ASSERT(pIrcPrint);
	ASSERT(pParse->uCode);

	switch (pParse->uCode)
	{
		default:
		{
			#ifdef DEBUG
				TRACE("Untreated reply: sender nick = %s, mach = %s, user = %s, reply = %s, last string = %s\n", 
				pParse->nick ? pParse->nick : "", 
				pParse->machine ? pParse->machine : "", 
				pParse->user ? pParse->user : "", 
				pParse->args[0] ? pParse->args[0] : "",
				pParse->lastString ? pParse->lastString : "");
			#endif // DEBUG
			break;
		}

		case RPL_WELCOME:		// 001
		{
			// Woohoo! We're in!
			theApp.CompleteConnection ();

			if (pParse->nArgs >= 2)				// We're logged in!  Join channel or show room list, or do nothing
				SetMyNameNick(pParse->args[1]);	// It's what the server thinks..
			pIrcPrint->SetFormat(PT_LASTSTRING, szLine, RGB(255,0,0));
			AddToStatus(*pIrcPrint, szLine);		// We need to do this before OnLogin displays the RoomList dialog box
			pIrcPrint->SetFormat(PT_NONE);		// Don't display this a second time

			GetIrcProto()->SetConnectionStatus(CX_NOCHANNEL);
			theApp.m_dynaRules.bMatchAndApplyRules(eOnConnect, NULL, NULL, CString(GetMyServer()), CString(pParse->args[1]), CString(""), CString(""));
			if (GetIrcProto()->GetConnectionStatus() != CX_DISCONNECTED)	// rules might have disconnected us
			{
				CCQuery* pQuery = new CCQuery(qpInitialLUsersMOTD, ctLUsersMOTD, dtMax, NULL, "", "", FALSE /*bCreatePrUserMatch*/);
				if (pQuery)
					m_queries.bAddQuery(pQuery);

				GetIrcProto()->OnLogin();
			}
			break;
		}

		case RPL_YOURHOST:		// 002
		case RPL_CREATED:		// 003
		{
			pIrcPrint->SetFormat(PT_LASTSTRING, szLine, RGB(255,0,0));
			break;
		}

		case RPL_MYINFO:		// 004
		case RPL_FOOFORNOW:		// 005
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(255,0,0), 3, TRUE);
			break;
		}

		case RPL_TRACELINK:		// 200
		case RPL_TRACECONNECTING://201
		case RPL_TRACEHANDSHAKE:// 202
		case RPL_TRACEUNKNOWN:	// 203
		case RPL_TRACEOPERATOR:	// 204
		case RPL_TRACEUSER:		// 205
		case RPL_TRACESERVER:	// 206
		case RPL_TRACENEWTYPE:	// 208
		case RPL_TRACELOG:		// 261

		case RPL_STATSLINKINFO:	// 211
		case RPL_STATSCOMMANDS:	// 212
		case RPL_STATSCLINE:	// 213
		case RPL_STATSNLINE:	// 214
		case RPL_STATSILINE:	// 215
		case RPL_STATSKLINE:	// 216
		case RPL_STATSYLINE:	// 218
		case RPL_ENDOFSTATS:	// 219
		case RPL_STATSLLINE:	// 241
		case RPL_STATSUPTIME:	// 242
		case RPL_STATSOLINE:	// 243
		case RPL_STATSHLINE:	// 244

		case RPL_ADMINME:		// 256
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 3, RPL_ENDOFSTATS == pParse->uCode);
			break;
		}

		case RPL_ADMINLOC1:		// 257
		case RPL_ADMINLOC2:		// 258
		case RPL_ADMINEMAIL:	// 259
		{
			pIrcPrint->SetFormat(PT_LASTSTRING, szLine, RGB(0,0,0), 0, FALSE);
			break;
		}

		case RPL_UMODEIS:		// 221
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 2, TRUE);
			break;

		case RPL_LUSERCLIENT:	// 251
		case RPL_LUSEROP:		// 252
		case RPL_LUSERUNKNOWN:	// 253
		case RPL_LUSERCHANNELS:	// 254
		case RPL_LUSERME:		// 255
		case RPL_LOCALUSERS:	// 265
		case RPL_GLOBALUSERS:	// 266
		{
			CSInString(&pParse->lastString);
			const char *szLUser = pParse->lastString;
			if (szLUser)
			{
				if (pParse->nArgs >= 3)
				{	// add first arg as string prefix...  Necessary for commands 252-254
					strLine = pParse->args[2];
					strLine += " ";
				}
				strLine += szLUser;

				CCQuery* pQuery = m_queries.FindQuery(ctLUsersMOTD, NULL);

				if (pQuery)
				{
					m_strLUSER += strLine;
					m_strLUSER += "\n";
				}

				if (pQuery && pQuery->GetQueryPurpose() == qpLUsersMOTD)
					pIrcPrint->SetFormat(PT_NONE);
				else
					pIrcPrint->SetFormat(PT_WHOLESTRING, strLine, RGB(0,0,255), 0, RPL_GLOBALUSERS == pParse->uCode);
			}
			break;
		}

		case RPL_USERHOST:		// 302
		{
			strLine.Format(IDS_USERHOST_PREFIX, pParse->lastString);
			pIrcPrint->SetFormat(PT_WHOLESTRING, strLine, RGB(128,0,128), 0, TRUE);
			break;
		}

		case RPL_ISON:			// 303
		{
			strLine.Format(IDS_ISON_PREFIX, pParse->lastString);
			pIrcPrint->SetFormat(PT_WHOLESTRING, strLine, RGB(0,0,255), 0, TRUE);
			break;
		}

		case RPL_AWAY:			// 301
		{
			if (pParse->lastString && pParse->nArgs > 2)
			{
				strLine.LoadString(IDS_AWAYREPORT);
				VERIFY(ReplaceToken(strLine, CString("%1"), DecodeNickForScreen(pParse->args[2])));
				VERIFY(ReplaceToken(strLine, CString("%2"), pParse->lastString));

				pIrcPrint->SetFormat(PT_WHOLESTRING, strLine, RGB(0,128,128), 0, TRUE);
			}
			break;
		}

		case RPL_UNAWAY:		// 305
		case RPL_NOWAWAY:		// 306
		{
			pIrcPrint->SetFormat(PT_LASTSTRING, szLine, RGB(0,128,128), 0, TRUE);
			break;
		}

		case RPL_WHOISUSER:		// 311
		{
			if (pParse->nArgs >= 5)
			{
				CSInString(&pParse->args[3]);	// user name

				// Get oldest queued ctWhoIs query object
				CCQuery*	pQuery = m_queries.FindQuery(ctWhoIs, NULL);

				// REGISB: pQuery can be NULL in user sends /RAW WHOIS KingArthur
				if (pQuery)
				{
					ASSERT(0 == stricmp(pQuery->GetNicknameMask(), pParse->args[2]));
					switch (pQuery->GetQueryPurpose())
					{
						case qpBanDlg:
						case qpKickDlg:
						{
							CString strBan;
							GetBanString(pParse->args[3], pParse->args[4], strBan);
							
							ASSERT(currentRoom);
							if (qpKickDlg == pQuery->GetQueryPurpose())
								currentRoom->DoKickDlg(pParse->args[2], strBan);
							else
							{
								g_strBan = strBan;
								sprintf(GetOutBuff(), "MODE %s +b\r\n", pQuery->GetChannelName());
								currentRoom->SendMessageText(GetOutBuff());
							}
							break;
						}
						case qpGetIdent:
						{
							ShowIdentity(pParse->args[2], pParse->args[3], pParse->args[4]);
							break;
						}
						case qpIgnoreIdent:
						{
							CString	strFullName(pParse->args[3]);
							strFullName += "@";
							strFullName += pParse->args[4];
							IgnoreUser(pParse->args[2], strFullName, ((WORD) pQuery->GetData()) & g_wIgnoreIdent, ((WORD) pQuery->GetData()) & g_wAutoIgnoreIdent);
							break;
						}
						default:
							ASSERT(FALSE);
					}
					pIrcPrint->SetFormat(PT_NONE);
				}
				else
					pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,128), 3);
			}
			break;
		}

		case RPL_WHOISSERVER:	// 312
		case RPL_WHOISOPERATOR:	// 313
		case RPL_WHOISIDLE:		// 317
		case RPL_WHOISCHANNELS:	// 319
		case RPL_WHOISIP:		// 320
		{
			// Get oldest queued ctWhoIs query object
			if (m_queries.FindQuery(ctWhoIs, NULL))
				pIrcPrint->SetFormat(PT_NONE);
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,128), 3);
			break;
		}

		case RPL_ENDOFWHOIS:	// 318
		{
			if (pParse->nArgs >= 3)
			{
				// Get oldest queued ctWhoIs query object
				POSITION	pos;
				CCQuery*	pQuery = m_queries.FindQuery(ctWhoIs, &pos);
				
				// REGISB: pQuery can be NULL in user sends /RAW WHOIS KingArthur
				if (pQuery)
				{
					ASSERT(pos);
					ASSERT(0 == stricmp(pQuery->GetNicknameMask(), pParse->args[2]));
					m_queries.FreeRemoveAt(pos);
					pIrcPrint->SetFormat(PT_NONE);
				}
				else
					pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,128), 3, TRUE);
			}
			break;
		}

		case RPL_WHOWASUSER:	// 314
		case RPL_ENDOFWHOWAS:	// 369
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,128), 3, pParse->uCode == RPL_ENDOFWHOWAS);
			break;
		}

		case RPL_LINKS:			// 364
		case RPL_ENDOFLINKS:	// 365
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 3, pParse->uCode == RPL_ENDOFLINKS);
			break;
		}

		case RPL_CHANNELMODEIS:	// 324
		{
			if (pParse->nArgs >= 4)
			{
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 3);		// may be overridden
	
				CChatDoc *doc = LookupDoc(pParse->args[2]);
				if (doc)
				{
					CRoomInfo	*pEnterInfo;
					INT			iRoomInfo;
					const char	*szArg2 = "", *szArg3 = "";
					if (pParse->nArgs >= 5)
						szArg2 = pParse->args[4];
					if (pParse->nArgs >= 6)
						szArg3 = pParse->args[5];
					ASSERT(doc->m_proto);
					doc->m_proto->m_strPassword = "";
					doc->m_proto->m_dwModes = 0;		// next ParseChannelMode is absolute, not relative
					pEnterInfo = theApp.GetRoomInfoFromName((LPCTSTR) pParse->args[2], &iRoomInfo, NULL != strchr(pParse->args[3], 'e') /*bCloneOK*/);
					ParseChannelMode(doc, pParse->args[3], szArg2, szArg3, pEnterInfo);
								
					if (currentRoom && pEnterInfo->m_bSetMode && !stricmp(pParse->args[2], currentRoom->m_strChannel))
					{	// set modes if requested on channel creation
						ASSERT(pEnterInfo == &g_enterInfo);
						doc->m_proto->ChatSetMode(pEnterInfo->m_dwModes, pEnterInfo->m_dwMaxUsers, pEnterInfo->m_strPassword);
						if (!pEnterInfo->m_strTopic.IsEmpty())
						{
							CString strControlFull = pEnterInfo->m_strTopic;
							if (pEnterInfo->m_prgdwTopicFormatting)
							{
								char* szCtrlFull = SzControlFull((LPCTSTR) pEnterInfo->m_strTopic, pEnterInfo->m_prgdwTopicFormatting);
								if (szCtrlFull)
								{
									strControlFull = CString(szCtrlFull);
									delete [] szCtrlFull;
								}
							}
							doc->m_proto->ChatSetTopic(strControlFull);
						}
					}

					if (iRoomInfo > 0)
						theApp.RemoveRoomInfo(iRoomInfo);
					else
						bInitEnterInfo(*pEnterInfo, "", NULL, NULL, 0L, FALSE);		// we've already taken care of the connecting sequence
				}

				// Get oldest queued ctGetChannelMode query object
				POSITION	pos;
				CCQuery*	pQuery = m_queries.FindQuery(ctGetChannelMode, &pos);

				if (pQuery)
				{
					ASSERT(pos);
					switch (pQuery->GetQueryPurpose())
					{
						case qpInitialMode:
							ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
							break;
						default:
							ASSERT(FALSE);
					}
					m_queries.FreeRemoveAt(pos);
					pIrcPrint->SetFormat(PT_NONE);
				}
			}
			break;
		}

		case RPL_NOTOPIC:		// 331
		{
			CCQuery*	pQuery;
			POSITION	pos;

			if (pQuery = m_queries.FindQuery(ctTopic, &pos))
			{
				ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
				ASSERT(qpListMembers == pQuery->GetQueryPurpose());
				pIrcPrint->SetFormat(PT_NONE);
				CString strEncodedChannel = pQuery->GetChannelName();
				CString strPrettyChannel  = pQuery->GetData() ? (LPTSTR) pQuery->GetData() : "";
				ASSERT(pos);
				m_queries.FreeRemoveAt(pos);
				OnUserListAux(NULL, strEncodedChannel, strPrettyChannel);
			}
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,128), 3, TRUE);
			break;
		}

		case RPL_TOPIC:			// 332
		{
			if (pParse->nArgs >= 3 && pParse->lastString)
			{
				CString		strEncodedChannel;
				CString		strPrettyChannel;
				CString		strCtrlLessTopic;
				CDWordArray	rgdwFormattingTmp;
				CChatDoc*	pDoc = LookupDoc(pParse->args[2]);
				CCQuery*	pQuery = NULL;
				POSITION	pos = NULL;
				BOOL		bListMembers = FALSE;

				CSInString(&pParse->lastString, pParse->args[2], pDoc);

				strCtrlLessTopic = SzControlLess(pParse->lastString, &rgdwFormattingTmp);

				if (pDoc)
				{
					if (pDoc->m_proto->m_prgdwTopicFormatting)
						FreeAndNullFormatting(&pDoc->m_proto->m_prgdwTopicFormatting);

					pDoc->m_proto->m_prgdwTopicFormatting = CopyFormatting(&rgdwFormattingTmp);
					pDoc->m_proto->m_strTopic = strCtrlLessTopic;

					// Get oldest queued ctTopic query object
					pQuery = m_queries.FindQuery(ctTopic, &pos);
				}

				if (pDoc && pQuery)
				{
					ASSERT(pos);
					switch (pQuery->GetQueryPurpose())
					{
						case qpInitialTopic:
							ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
							break;
						case qpListMembers:
							ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
							bListMembers = TRUE;
							break;
						default:
							ASSERT(FALSE);
					}
					pIrcPrint->SetFormat(PT_NONE);
				}
				else
				{
					if (pQuery = m_queries.FindQuery(ctTopic, &pos))
					{
						ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
						ASSERT(qpListMembers == pQuery->GetQueryPurpose());
						bListMembers = TRUE;
					}
					else
					{
						strLine = CString(pParse->args[2]) + " :" + strCtrlLessTopic;
						PushFormattingOffsets(&rgdwFormattingTmp, strlen(pParse->args[2]) + 2);
						pIrcPrint->SetFormat(PT_WHOLESTRING, strLine, RGB(0,0,128), 0, TRUE);
						AddToStatus(*pIrcPrint, strLine, &rgdwFormattingTmp);
						// Don't display this a second time
					}
					pIrcPrint->SetFormat(PT_NONE);	
				}

				rgdwFormattingTmp.RemoveAll();

				if (pQuery && pos)
				{
					if (bListMembers)
					{
						strEncodedChannel = pQuery->GetChannelName();
						strPrettyChannel  = pQuery->GetData() ? (LPTSTR) pQuery->GetData() : "";
					}
					m_queries.FreeRemoveAt(pos);
				}

				if (bListMembers)
					OnUserListAux(NULL, strEncodedChannel, strPrettyChannel);
			}
			break;
		}

		case RPL_INVITING:		// 341
		{
			if (pParse->nArgs >= 4)
			{
				AcknowledgeInvite(DecodeNick(pParse->args[2]), DecodeChan(pParse->args[3]));  // don't know if it's MIC
				pIrcPrint->SetFormat(PT_NONE);
			}
			break;
		}

		case RPL_LISTSTART:		// 321
		case RPL_LISTXSTART:	// 811
		{
			enumCommandType	ct = (pParse->uCode == RPL_LISTSTART) ? ctList : ctListX;
			CCQuery*		pQuery = m_queries.FindQuery(ct, NULL);

			if (pQuery)
			{
				switch (pQuery->GetQueryPurpose())
				{
					case qpOnNewRoomEvent:
						break;
					case qpRoomListDlg:
					{
						sRoom = NULL;
						g_bCanViewUnrated = bCanViewUnrated();
						StartRoomList();
						break;
					}
					default:
						ASSERT(FALSE);
				}
				pIrcPrint->SetFormat(PT_NONE);
			}
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(128,0,128), 3);
			break;
		}

		case RPL_LIST:			// 322
		{
			if (pParse->nArgs >= 4 && pParse->lastString)
			{
				// Get oldest queued ctList query object
				CCQuery*	pQuery = m_queries.FindQuery(ctList, NULL);

				if (pQuery)
				{
					switch (pQuery->GetQueryPurpose())
					{
						case qpOnNewRoomEvent:
						{
							CCRule* pRule = (CCRule*) pQuery->GetData();
							ASSERT(pRule);
							if (pRule->bActive() && !pRule->bStopped())
							{
								CCDaemonExt* pDaemonExt = pRule->GetDaemonExt();
								ASSERT(pDaemonExt);
								pDaemonExt->bAddChannelToCurrentList(CString(pParse->args[2]));
							}
							break;
						}
						case qpRoomListDlg:
						{
							CRoom *pRoom = new CRoom;
							if (pRoom)
							{
								CSInString(&pParse->lastString);
								// Some IRC servers return * as the room name when the
								// room is private.
								if (!pParse->args[2] || pParse->args[2][0] != '*' || 
										pParse->args[2][1] != '\0')
								{
									pRoom->m_name = pParse->args[2];
									pRoom->m_prettyName = DecodeChan(pParse->args[2]);
									pRoom->m_nUsers = atoi(pParse->args[3]);
									pRoom->m_descr = pParse->lastString;
									pRoom->m_byteRegistered = FALSE;
									AddToRoomList(pRoom);  // PICS test done in OnChatRoomList
								}
							}
							break;
						}
						default:
							ASSERT(FALSE);
					}
					pIrcPrint->SetFormat(PT_NONE);
				}
				else
					pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(128,0,128), 3);
			}
			break;
		}

		case RPL_LISTXLIST:		// 812
		{
			// Get oldest queued ctListX query object
			CCQuery*	pQuery = m_queries.FindQuery(ctListX, NULL);

			if (pQuery)
			{
				switch (pQuery->GetQueryPurpose())
				{
					case qpOnNewRoomEvent:
					{
						CCRule* pRule = (CCRule*) pQuery->GetData();
						ASSERT(pRule);
						if (pRule->bActive() && !pRule->bStopped())
						{
							CCDaemonExt* pDaemonExt = pRule->GetDaemonExt();
							ASSERT(pDaemonExt);
							pDaemonExt->bAddChannelToCurrentList(CString(pParse->args[2]));
						}
						break;
					}
					case qpRoomListDlg:
					{
						if (sRoom)
						{
							AddToRoomList(sRoom, sbAddIt);
							sRoom = NULL;
						}
						if (pParse->nArgs >= 6 && pParse->lastString)
						{
							if (sRoom = new CRoom)
							{
								const char *szRoomName = pParse->args[2];
								BOOL bMIC = (strchr(pParse->args[3], 'y') != NULL);
								CSInString(&pParse->lastString, bMIC ? NULL : szRoomName);
								sRoom->m_name = szRoomName;
								sRoom->m_prettyName = DecodeChan(szRoomName, bMIC);
								sRoom->m_nUsers = atoi(pParse->args[4]);
								sRoom->m_descr = SzControlLess(pParse->lastString, NULL);
								sRoom->m_byteRegistered = (strchr(pParse->args[3], 'r') != NULL);
								sbAddIt = g_bCanViewUnrated;  // default
							}
						}
						break;
					}
					default:
						ASSERT(FALSE);
				}
				pIrcPrint->SetFormat(PT_NONE);
			}
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(128,0,128), 3);
			break;
		}

		case RPL_LISTXPICS:		// 813
		{
			sbAddIt = bPassesRatings(pParse->lastString);	// PICS string (server-based.  should already be in windows charset.)

			if (m_queries.FindQuery(ctListX, NULL))
				pIrcPrint->SetFormat(PT_NONE);
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(128,0,128), 3);
			break;
		}

		case RPL_LISTEND:		// 323
		case RPL_LISTXTRUNC:	// 816
		case RPL_LISTXEND:		// 817
		{
			// Get oldest queued ctList query object
			POSITION	pos;
			CCQuery*	pQuery = m_queries.FindQuery(pParse->uCode == RPL_LISTEND ? ctList : ctListX, &pos);

			if (pQuery)
			{
				ASSERT(pos);
				m_queries.RemoveAt(pos);
				switch (pQuery->GetQueryPurpose())
				{
					case qpOnNewRoomEvent:
					{
						CCRule*	pRule = (CCRule*) pQuery->GetData();
						ASSERT(pRule);
						if (pRule->bActive() && !pRule->bStopped())
						{
							CCDaemonExt* pDaemonExt = pRule->GetDaemonExt();
							ASSERT(pDaemonExt);
							VERIFY(pDaemonExt->bOnEndOfListing(&(theApp.m_dynaRules), pRule, pQuery->GetQueryPurpose()));
						}
						break;
					}
					case qpRoomListDlg:
					{
						if (sRoom)
						{
							AddToRoomList(sRoom, sbAddIt);
							sRoom = NULL;
						}
						EndRoomList();
						break;
					}
					default:
						ASSERT(FALSE);
				}
				delete pQuery;
				pIrcPrint->SetFormat(PT_NONE);
			}
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(128,0,128), 3, TRUE);
			break;
		}

		case RPL_NAMEREPLY:		// 353
		{
			// Get oldest queued ctNames query object
			POSITION	pos;
			CCQuery*	pQuery;

			// We might not have gotten a RPL_TOPIC (332) reply and need to remove the qpInitialTopic cell
			if (pQuery = m_queries.FindQuery(ctTopic, &pos))
			{
				ASSERT(pos);
				switch (pQuery->GetQueryPurpose())
				{
					case qpInitialTopic:
						ASSERT(0 == strcmp(pParse->args[3], pQuery->GetChannelName()));
						break;
					default:
						ASSERT(FALSE);
				}
				m_queries.FreeRemoveAt(pos);
			}

			if (pQuery = m_queries.FindQuery(ctNames, NULL))
			{
				switch (pQuery->GetQueryPurpose())
				{
					case qpInitialNames:
					{
						CChatDoc* pDoc = pParse->nArgs >= 4 ? LookupDoc(pParse->args[3]) : NULL;
						if (pParse->lastString && pDoc)
							if (bForEachWord(pParse->lastString, bSingleJoin, pDoc, 0L, " "))
								theApp.m_dynaRules.bMatchAndApplyRules(eOnJoin, NULL, NULL, CString(GetMyServer()), CString(GetMyNickName())+"!"+GetMyIdent(), pDoc->m_proto->m_strChannel, CString(""));
						break;
					}
					default:
						ASSERT(FALSE);
				}
				pIrcPrint->SetFormat(PT_NONE);
			}
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(128,128,0), 4);
			break;
		}

		case RPL_ENDOFNAMES:	// 366
		{
			// Get oldest queued ctNames query object
			POSITION	pos;
			CCQuery*	pQuery = m_queries.FindQuery(ctNames, &pos);

			if (pQuery)
			{
				ASSERT(pos);
				switch (pQuery->GetQueryPurpose())
				{
					case qpInitialNames:
						ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
						break;
					default:
						ASSERT(FALSE);
				}
				m_queries.FreeRemoveAt(pos);
				pIrcPrint->SetFormat(PT_NONE);
			}
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(128,128,0), 3, TRUE);
			break;
		}

		case RPL_WHOREPLY:		// 352
		{
			if (pParse->nArgs >= 8)
			{
				// Get oldest queued ctWho query object
				CCQuery*	pQuery = m_queries.FindQuery(ctWho, NULL);

				if (pQuery)
				{
					switch (pQuery->GetQueryPurpose())
					{
						case qpOnConnectEvent:
						case qpOnDisconnectEvent:
						case qpOnNotification:
						{
							// Make sure this server answer matches to the query's pPrUserMatch filter
							ASSERT(pQuery->GetPrUserMatch());
							if (bIsMatch(pQuery->GetPrUserMatch(), pParse->args[6], pParse->args[3], pParse->args[4]))
							{
								CCRule*		pRule = NULL;
								CCNotif*	pNotif = NULL;

								if (qpOnNotification == pQuery->GetQueryPurpose())
								{
									pNotif = (CCNotif*) pQuery->GetData();
									ASSERT(pNotif);
								}
								else
								{
									pRule = (CCRule*) pQuery->GetData();
									ASSERT(pRule);
								}

								if ((pRule && pRule->bActive() && !pRule->bStopped()) ||
									(pNotif && pNotif->bActive()))
								{
									CCDaemonExt* pDaemonExt = pRule ? pRule->GetDaemonExt() : pNotif->GetDaemonExt();
									ASSERT(pDaemonExt);
									CUser* pUser = CreateUserFromWhoReply(pParse);
									if (pUser)
									{
										if (!pDaemonExt->bAddUserToCurrentList(pUser))
											pUser->Release();
									}
								}
							}
							break;
						}

						case qpInitialWho:
						case qpUserListDlg:
						{
							CSInString(&pParse->args[3]);

							if (qpInitialWho == pQuery->GetQueryPurpose())
								UpdateIgnoreOnEntry(pParse->args[2], pParse->args[6], pParse->args[3], pParse->args[4]);
							else
							{
								CUser* pUser = CreateUserFromWhoReply(pParse);
								if (pUser)
									AddToUserList(pUser);
							}
							break;
						}

						default:
							ASSERT(FALSE);
					}
					pIrcPrint->SetFormat(PT_NONE);
				}
				else
					pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,128,128), 3);
			}
			break;
		}

		case RPL_ENDOFWHO:		// 315
		{
			// Get oldest queued ctWho query object
			POSITION	pos;
			CCQuery*	pQuery = m_queries.FindQuery(ctWho, &pos);

			if (pQuery)
			{
				ASSERT(pos);
				m_queries.RemoveAt(pos);
				switch (pQuery->GetQueryPurpose())
				{
					case qpOnConnectEvent:
					case qpOnDisconnectEvent:
					case qpOnNotification:
					{
						CCRule*		pRule = NULL;
						CCNotif*	pNotif = NULL;

						if (qpOnNotification == pQuery->GetQueryPurpose())
						{
							pNotif = (CCNotif*) pQuery->GetData();
							ASSERT(pNotif);
						}
						else
						{
							pRule = (CCRule*) pQuery->GetData();
							ASSERT(pRule);
						}

						if ((pRule && pRule->bActive() && !pRule->bStopped()) ||
							(pNotif && pNotif->bActive()))
						{
							CCDaemonExt* pDaemonExt = pRule ? pRule->GetDaemonExt() : pNotif->GetDaemonExt();
							ASSERT(pDaemonExt);
							if (pRule)
								VERIFY(pDaemonExt->bOnEndOfListing(&(theApp.m_dynaRules), pRule, pQuery->GetQueryPurpose()));
							else
								VERIFY(pDaemonExt->bOnEndOfListing(&(theApp.m_dynaNotifs), pNotif));
						}
						break;
					}
					case qpInitialWho:
					case qpUserListDlg:
					{
						if (qpUserListDlg == pQuery->GetQueryPurpose())
							EndUserList();
						break;
					}
					default:
						ASSERT(FALSE);
				}
				delete pQuery;
				pIrcPrint->SetFormat(PT_NONE);
			}
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,128,128), 3, TRUE);
			break;
		}

		case RPL_INFO:			// 371
		case RPL_ENDOFINFO:		// 374
		case RPL_VERSION:		// 351
		case RPL_TIME:			// 391
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(128,0,0), 3, pParse->uCode != RPL_INFO);
			break;
		}
		
		case RPL_BANLIST:		// 367
		{
			// <channel> <banid>
			if (pParse->nArgs >= 4)
			{
				g_arrayBans.Add(DecodeNick(pParse->args[3]));
				pIrcPrint->SetFormat(PT_NONE);
			}
			break;
		}

		case RPL_ENDOFBANLIST:	// 368
		{
			// <channel> :End of channel ban list
			
			// Get oldest queued ctSetChannelMode query object
			POSITION	pos;
			CCQuery*	pQuery = m_queries.FindQuery(ctSetChannelMode, &pos);

			if (pQuery)
			{
				ASSERT(qpComSetChannelMode == pQuery->GetQueryPurpose());
				ASSERT(0 == stricmp(pQuery->GetChannelName(), pParse->args[2]));
				ASSERT(pos);
				m_queries.FreeRemoveAt(pos);
			}

			pIrcPrint->SetFormat(PT_NONE);
			DoBanDlg(pParse->args[2], g_strBan, g_arrayBans);
			g_strBan = "";
			g_arrayBans.RemoveAll();
			break;
		}

		case RPL_MOTDSTART:		// 375
		{
			pIrcPrint->SetFormat(PT_NONE);
			break;
		}

		case RPL_MOTD:			// 372
		case RPL_MOTD2:			// 377
		{
			// 377 used by irc.sprynet.com
			const char *szMOTD = pParse->lastString;

			if (szMOTD)
			{
				if (strncmp(szMOTD, "- ", 2) == 0)
					szMOTD += 2;
				if (strcmp(szMOTD, "-") == 0)
					szMOTD++;
				m_strMOTD += szMOTD;
				m_strMOTD += "\r\n";
				strLine = szMOTD;
				strLine += "\r";
			}

			CCQuery* pQuery = m_queries.FindQuery(ctLUsersMOTD, NULL);
			if (pQuery && pQuery->GetQueryPurpose() == qpLUsersMOTD)
				pIrcPrint->SetFormat(PT_NONE);
			else
				pIrcPrint->SetFormat(PT_WHOLESTRING, strLine, RGB(0,128,0));
			break;
		}

		case RPL_ENDOFMOTD:		// 376
		{
			// Get oldest queued ctLUsersMOTD query object
			POSITION	pos;
			CCQuery*	pQuery = m_queries.FindQuery(ctLUsersMOTD, &pos);
			BOOL		bNewLine = (!pQuery || pQuery->GetQueryPurpose() == qpInitialLUsersMOTD);

			if (pQuery)
			{
				if ((pQuery->GetQueryPurpose() == qpLUsersMOTD || (theApp.m_flags1 & F1_SHOWMOTD)) && (!m_strMOTD.IsEmpty() || !m_strLUSER.IsEmpty()))
					ShowMOTD(m_strLUSER, m_strMOTD);

				ASSERT(pos);
				m_queries.FreeRemoveAt(pos);
			}

			m_strMOTD = "";
			m_strLUSER = "";
			pIrcPrint->SetFormat(PT_NONE, "", RGB(0,128,0), 0, bNewLine);
			theApp.m_bDisableMOTD = FALSE;
			break;
		}

		case RPL_YOUREOPER:		// 381
		case RPL_YOUREADMIN:	// 386
		{
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,255), 2, TRUE);
			break;
		}

		case RPL_IRCX:			// 800
		{
			// Prefix		 = "keezer"
			// pParse->args[0] = "800"
			// pParse->args[1] = "*"						// no nickname specified yet
			// pParse->args[2] = "0|1"					// we asked to turn into the IRCX mode or not
			// pParse->args[3] = "0"						// IRCX version number
			// pParse->args[4] = "NTLM,ANON"				// security packages available
			// pParse->args[5] = "512"					// max message length
			// pParse->args[6] = "*"

			// ASSERT('*' == pParse->args[1][0]);
			POSITION	pos;
			CCQuery*	pQuery = m_queries.FindQuery(ctModeIsIrcX, &pos);

			if (!pQuery)
				pQuery = m_queries.FindQuery(ctIrcX, &pos);

			if (pQuery)
			{
				ASSERT(pos);
				m_queries.FreeRemoveAt(pos);

				// is this the first instance of 800 or the second?
				if ('0' == pParse->args[2][0])
				{
					// first instance, we're still in IRC mode
					m_bIrcXServer = TRUE;

					m_bJustSentModeIsIrcX = FALSE;

					// Create the SvrSecuPack string array
					// Are there any secu packages?
					if (pParse->nArgs >= 7)
					{
						BOOL bEnd;
						CHAR *szHeadTmp, *szTmp;
						szHeadTmp = szTmp = pParse->args[4];
						ASSERT('\0' != *szHeadTmp);
						do
						{
							if ((',' == *szTmp) || (bEnd = ('\0' == *szTmp)))
							{
								*szTmp = '\0';
								if (stricmp(g_szAnon, szHeadTmp))
									m_rgszSvrSecuPack.Add(szHeadTmp);
								else
									m_bAnonAllowed = TRUE;
								if (!bEnd) 
									szHeadTmp = ++szTmp;
							}
							else
								szTmp++;
						}
						while (!bEnd);
					}

					// Read the max message length
					SHORT nMaxMsgLength = atoi(pParse->args[pParse->nArgs-2]);

					// Is the max length bigger than our current buffers?
					if (m_nMaxMsgLength < nMaxMsgLength)
					{
						HrInitAlloc(nMaxMsgLength);
						theApp.HrAllocBuffer(nMaxMsgLength);
					}

					// REGISB: revisit since HrInitAlloc might return OOM

					// finally switch to IRCX mode
					GetIrcProto()->bExecuteQuery(qpIrcX, ctIrcX, dtMax, NULL, "", "");
				}
				else
				{
					// second instance, we're already in IRCX mode
					// Login Time
					HrIrcXLogin(TRUE /*bForceNextPackage*/);
				}
				pIrcPrint->SetFormat(PT_NONE);
			}
			else
				pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 3, TRUE);
			break;
		}

		case RPL_ACCESSADD:		// 801
		case RPL_ACCESSDELETE:	// 802
		case RPL_ACCESSSTART:	// 803
		case RPL_ACCESSLIST:	// 804
		case RPL_ACCESSEND:		// 805
		case RPL_EVENTADD:		// 806
		case RPL_EVENTDEL:		// 807
		case RPL_EVENTSTART:	// 808
		case RPL_EVENTLIST:		// 809
		case RPL_EVENTEND:		// 810
			pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 3, pParse->uCode != RPL_ACCESSLIST && pParse->uCode == RPL_EVENTLIST);
			break;

		case RPL_PROPLIST:		// 818
		{
			if (pParse->nArgs >= 4)
			{
				// Get oldest queued ctPropGet query object
				CCQuery*	pQuery = m_queries.FindQuery(ctPropGet, NULL);

				if (pQuery)
				{
					ASSERT(GetIrcProto());
					switch (pQuery->GetQueryPurpose())
					{
						case qpJoinPics:
						case qpCreatePics:
						{
							pQuery->SetQueryPurpose(qpMax);
							ASSERT(0 == lstrcmpi(pParse->args[3], "PICS"));
							if (bPassesRatings(pParse->lastString, TRUE))
							{
								CRoomInfo* pEnterInfo = theApp.GetRoomInfoFromName((LPCTSTR) pParse->args[2]);
								if (qpJoinPics == pQuery->GetQueryPurpose())
									GetIrcProto()->ChatJoinAux(*pEnterInfo);
								else
									GetIrcProto()->ChatCreateAux(*pEnterInfo);
							}
							// REGISB FIX ME ?? test it!!
							// else
							//	display error message PICS ratings don't allow you to step into
							break;
						}
						case qpJoinBackUrl:
						{
							ASSERT(0 == lstrcmpi(pParse->args[3], "CLIENT"));
							GetIrcProto()->HandleClientDataChange (pParse->lastString);
							break;
						}
						default:
							ASSERT(FALSE);
					}
					pIrcPrint->SetFormat(PT_NONE);
				}
				else
					pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(0,0,0), 3, TRUE);
			}
			break;
		}

		case RPL_PROPEND:		// 819
		{
			if (pParse->nArgs >= 3)
			{
				// Get oldest queued ctPropGet query object
				POSITION	pos;
				CCQuery*	pQuery = m_queries.FindQuery(ctPropGet, &pos);

				if (pQuery)
				{
					ASSERT(GetIrcProto());
					ASSERT(pos);
					m_queries.RemoveAt(pos);
					switch (pQuery->GetQueryPurpose())
					{
						case qpJoinPics:
						case qpCreatePics:
						{
							if (bCanViewUnrated(TRUE))
							{
								CRoomInfo* pEnterInfo = theApp.GetRoomInfoFromName((LPCTSTR) pParse->args[2]);
								if (qpJoinPics == pQuery->GetQueryPurpose())
									GetIrcProto()->ChatJoinAux(*pEnterInfo);
								else
									GetIrcProto()->ChatCreateAux(*pEnterInfo);
							}
							break;
						}
						case qpJoinBackUrl:
						case qpMax:
							break;
						default:
							ASSERT(FALSE);
					}
					delete pQuery;
				}
				pIrcPrint->SetFormat(PT_NONE);
			}
			break;
		}
	}
}


void CIrcSocket::HandleErrorCode(char *szLine, PIRCPARSE pParse, CIrcPrint *pIrcPrint)
{
	CString		strMesg;
	BOOL		bDisplayErrorInStatusWindow = FALSE;
	CHAR*		szChannelName = NULL;
	INT			iRoomIndex = -1;
	CRoomInfo*	pEnterInfo = NULL;

	ASSERT(pParse);
	ASSERT(pIrcPrint);
	ASSERT(pParse->uCode);

	switch (pParse->uCode)
	{
		default:
		{
			#ifdef DEBUG
				TRACE("Untreated error: sender nick = %s, mach = %s, user = %s, error = %s, last string = %s\n", 
				pParse->nick ? pParse->nick : "", 
				pParse->machine ? pParse->machine : "", 
				pParse->user ? pParse->user : "", 
				pParse->args[0] ? pParse->args[0] : "",
				pParse->lastString ? pParse->lastString : "");
			#endif // DEBUG
			bDisplayErrorInStatusWindow = TRUE;
			break;
		}

		case ERR_NOSUCHNICK:
		{
			//:chloe1 401 <mynick> <nick|channel> :No such nick/channel
			if (CHANNELPREFIX(pParse->args[2][0]))
			{
				// No such channel
				strMesg.Format(IDS_ERR_NOSUCHCHANNEL, DecodeChan(pParse->args[2]));	// don't know if it's MIC
			}
			else
			{
				// No such nickname
				strMesg.Format(IDS_ERR_NOSUCHNICK, DecodeNick(pParse->args[2]));
				bFreeModeCell(NULL, pParse->args[2]);
			}
			AfxMessageBox(strMesg);
			break;
		}

		case ERR_NOSUCHCHANNEL:		// 403
		{
			if (pEnterInfo = theApp.GetRoomInfoFromName((LPCTSTR) pParse->args[2], &iRoomIndex, FALSE /*bCloneOK*/, TRUE /*bNullOK*/))
				ShowBadChannelName(pParse->nArgs > 2 ? pParse->args[2] : "");
			else
			{
				POSITION	pos;
				CCQuery*	pQuery = m_queries.FindQuery(ctTopic, &pos);

				iRoomIndex = 0;

				if (pQuery && qpListMembers == pQuery->GetQueryPurpose())
				{
					ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
					ASSERT(pos);
					m_queries.FreeRemoveAt(pos);

					strMesg.Format(IDS_ERR_NOSUCHCHANNELANYMORE, DecodeChan(pParse->args[2]));	// don't know if it's MIC

					// we need to re-enable the button
					if (cui.m_pvRoomList)
					{
						CRoomList*	pRoomList = (CRoomList*) cui.m_pvRoomList;
						CWnd*		pBtn1 = pRoomList->GetDlgItem(IDC_RESET_LIST);
						CWnd*		pBtn2 = pRoomList->GetDlgItem(IDC_LISTMEMBERS);
						
						if (pBtn1 && pBtn2)
						{
							pBtn2->EnableWindow(TRUE);
							pRoomList->GotoDlgCtrl(pBtn1);
							pRoomList->NextDlgCtrl();
						}
					}
				}
				else
				{
					strMesg.Format(IDS_ERR_NOSUCHCHANNEL, DecodeChan(pParse->args[2]));	// don't know if it's MIC
					bFreeModeCell(pParse->args[2], pParse->args[2]);
				}
				AfxMessageBox(strMesg);
			}
			break;
		}

		case ERR_TOOMANYCHANNELS:	// 405
		{
			AfxMessageBox(IDS_ERR_TOOMANYCHANNELS);
			break;
		}

		case ERR_NOMOTD:			// 422
		{
			ASSERT(m_strMOTD.IsEmpty());
			
			strMesg.LoadString(IDS_ERR_NOMOTD);
			pIrcPrint->SetFormat(PT_WHOLESTRING, strMesg, RGB(0,0,255), 0, TRUE);
			AddToStatus(*pIrcPrint, strMesg, NULL);

			// Get oldest queued ctLUsersMOTD query object
			POSITION pos;
			CCQuery* pQuery = m_queries.FindQuery(ctLUsersMOTD, &pos);

			if (pQuery)
			{
				if ((pQuery->GetQueryPurpose() == qpLUsersMOTD || (theApp.m_flags1 & F1_SHOWMOTD)) && !m_strLUSER.IsEmpty())
					ShowMOTD(m_strLUSER, "");

				ASSERT(pos);
				m_queries.FreeRemoveAt(pos);
			}
			m_strLUSER = "";
			theApp.m_bDisableMOTD = FALSE;

			break;
		}

		case ERR_NONICKNAMEGIVEN:	// 431
		case ERR_ERRONEUSNICKNAME:	// 432
		case ERR_NICKNAMEINUSE:		// 433
		{
			int		iIndex = (ERR_NICKNAMEINUSE == pParse->uCode) ? 2 : 1;
			char*	szBadNick = pParse->nArgs >= (iIndex+1) ? pParse->args[iIndex] : "";
			GetIrcProto()->TryNewNick((ERR_NICKNAMEINUSE == pParse->uCode) ? ID_ERR_DUPED_NICK : ID_ERR_BAD_NICK, m_bIrcXServer ? DecodeNick(szBadNick) : szBadNick);
			break;
		}

		case ERR_NICKCOLLISION:		// 436
		{
			AfxMessageBox(IDS_ERR_NICKCOLLISION);
			break;
		}

		case ERR_NICKTOOFAST:		// 438
		{
			AfxMessageBox(IDS_ERR_NICKTOOFAST);
			break;
		}

		case ERR_NICKNOCHANGE:		// 439
		{
			AfxMessageBox(IDS_ERR_NICKNOCHANGE);
			break;
		}

		case ERR_NOTONCHANNEL:		// 442
		{
			// <channel> :You're not on that channel
			CCQuery*	pQuery;
			POSITION	pos;

			if (pQuery = m_queries.FindQuery(ctTopic, &pos))
			{
				// happens on irc.dal.net for example when trying to get the topic of a channel
				// in order to list its members.
				ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
				ASSERT(qpListMembers == pQuery->GetQueryPurpose());
				CString strEncodedChannel = pQuery->GetChannelName();
				CString strPrettyChannel  = pQuery->GetData() ? (LPTSTR) pQuery->GetData() : "";
				ASSERT(pos);
				m_queries.FreeRemoveAt(pos);
				OnUserListAux(NULL, strEncodedChannel, strPrettyChannel);
			}
			else
			{
				bFreeModeCell(pParse->args[2], NULL);
				bDisplayErrorInStatusWindow = TRUE;
			}
			break;
		}

		case ERR_NOTREGISTERED:		// 451
		{
			// :You have not registered
			HrModeIsIrcXFailure();
			break;
		}

		case ERR_NEEDMOREPARAMS:	// 461
		{
			// <command> :Not enough parameters
			bFreeModeCell(NULL, NULL);
			bDisplayErrorInStatusWindow = TRUE;
			break;
		}

		case ERR_PASSWDMISMATCH: 	// 464
		{
			// Wrong password for plaintext, for oper keyword.
			HRESULT hr = HrIrcSetOper (m_pszUserName, NULL);
			break;
		}

		case ERR_YOUREBANNEDCREEP:	// 465
		{
			AfxMessageBox(IDS_ERR_YOUREBANNEDCREEP);
			break;
		}

		case ERR_YOUWILLBEBANNED:	// 466
		{
			AfxMessageBox(IDS_ERR_YOUWILLBEBANNED);
			break;
		}

		case ERR_KEYSET:			// 467
		{
			// <channel> :Channel key already set
			bFreeModeCell(pParse->args[2], NULL);
			bDisplayErrorInStatusWindow = TRUE;
			break;
		}

		case ERR_CHANNELISFULL:		// 471
		{
			strMesg.Format(ID_ERR_CHANNELISFULL, DecodeChan(pParse->args[2]));	// don't know if it's MIC
			AfxMessageBox(strMesg);
			break;
		}

		case ERR_UNKNOWNMODE:		// 472
		{
			// <char> :is unknown mode char to me
			bFreeModeCell(NULL, NULL);
			bDisplayErrorInStatusWindow = TRUE;
			break;
		}

		case ERR_INVITEONLYCHAN:	// 473
		{
			strMesg.Format(ID_ERR_INVITEONLY, DecodeChan(pParse->args[2]));	// don't know if it's MIC
			AfxMessageBox(strMesg);
			break;
		}

		case ERR_BANNEDFROMCHAN:	// 474
		{
			strMesg.Format(ID_ERR_BANNEDFROMCHAN, DecodeChan(pParse->args[2]));	// don't know if it's MIC
			AfxMessageBox(strMesg);
			break;
		}

		case ERR_BADCHANNELKEY:		// 475
		{
			pEnterInfo = theApp.GetRoomInfoFromName((LPCTSTR) pParse->args[2], &iRoomIndex);
			OnBadChannelPassword(*pEnterInfo);
			break;
		}

		case ERR_CHANOPRIVSNEEDED:	// 482
		{
			// <channel> :You're not channel operator
			if (!bFreeModeCell(pParse->args[2], NULL))
			{
				// Does this come from a TOPIC command that failed?
				// Get oldest queued ctTopic query object
				POSITION	pos;
				CCQuery*	pQuery = m_queries.FindQuery(ctTopic, &pos);

				if (pQuery)
				{
					// this should not happen because UI does not allow this situation
					ASSERT(pos);
					switch (pQuery->GetQueryPurpose())
					{
						case qpSetTopic:
							ASSERT(0 == strcmp(pParse->args[2], pQuery->GetChannelName()));
							break;
						default:
							ASSERT(FALSE);
					}
					m_queries.FreeRemoveAt(pos);
				}
			}
			bDisplayErrorInStatusWindow = TRUE;
			break;
		}

		case ERR_UMODEUNKNOWNFLAG:	// 501
			// :Unknown MODE flag
		case ERR_USERSDONTMATCH:	// 502
			// :Can't change mode for other users
		{
			bFreeModeCell(NULL, "");
			bDisplayErrorInStatusWindow = TRUE;
			break;
		}
		
		case ERR_NOJOINDYNAMIC:		// 552
		{
			AfxMessageBox(IDS_ERR_NOJOINDYNAMIC);
			break;
		}

		case ERR_NODYNAMICCHANNELS:	// 553
		{
			AfxMessageBox(IDS_ERR_NODYNAMICCHANNELS);
			break;
		}

		case ERR_AUTHONLY:			// 556
		{
			AfxMessageBox(IDS_ERR_AUTHONLY);
			break;
		}

		case ERR_CANNOTCREATEDYNAMIC:	// or ERR_BADFUNCTION 902
		{
			if (!m_bIrcXServer)
			{
				// ERR_CANNOTCREATEDYNAMIC case
				AfxMessageBox(IDS_ERR_NODYNAMICCHANNELS);
			}
			// else ERR_BADFUNCTION case not treated
			break;
		}

		case ERR_ONLYAUTHCANJOIN:	// or ERR_BADTAG 904
		{
			if (!m_bIrcXServer)
			{
				// ERR_ONLYAUTHCANJOIN case
				AfxMessageBox(IDS_ERR_AUTHONLY);
			}
			// else ERR_BADTAG case not treated
			break;
		}

		case ERR_CANNOTCHANGENICK:	// or ERR_BADPROPERTY 905
		{
			if (!m_bIrcXServer)
			{
				// ERR_CANNOTCHANGENICK case
				AfxMessageBox(IDS_ERR_NICKNOCHANGE);
			}
			// else ERR_BADPROPERTY case not treated
			break;
		}

		case ERR_CANNOTJOINDYNAMIC:		// or ERR_RESOURCE 907
		{
			if (!m_bIrcXServer)
			{
				// ERR_CANNOTJOINDYNAMIC case
				AfxMessageBox(IDS_ERR_NOJOINDYNAMIC);
			}
			// else ERR_RESOURCE case not treated
			break;
		}

		case ERR_AUTHENTICATIONFAILED:	// 910
		{
			// ":<servername> 910 * <secupackage> : Authentication failed
			// User account information provided was wrong, let's try again...
			// Display appropriate error message
			AfxMessageBox(ID_ERR_BADUSERINFO);
			m_bAuthFailed = TRUE;
			HrIrcXLogin(FALSE /*bForceNextPackage*/);
			break;
		}

		case ERR_UNKNOWNPACKAGE:	// 912
		{
			// ":<servername> 912 * <secupackage> : Unsupported authentication package
			// This error should theoretically never occur, just here to be on the safe side
			// Try next security package, or fail
			HrIrcXLogin(TRUE /*bForceNextPackage*/);
			break;
		}

		case ERR_NOSUCHOBJECT:		// 924
		{
			// Get oldest queued ctPropGet query object
			POSITION	pos;
			CCQuery*	pQuery = m_queries.FindQuery(ctPropGet, &pos);

			if (pQuery)
			{
				ASSERT(GetIrcProto());
				switch (pQuery->GetQueryPurpose())
				{
					case qpJoinPics:
					case qpCreatePics:
					{	// prop test on non-existant room
						if (!pQuery->GetChannelName().Compare(pParse->args[2]) && bCanViewUnrated(TRUE))
						{
							pEnterInfo = theApp.GetRoomInfoFromName((LPCTSTR) pParse->args[2]);
							if (qpJoinPics == pQuery->GetQueryPurpose())
								GetIrcProto()->ChatJoinAux(*pEnterInfo);
							else
								GetIrcProto()->ChatCreateAux(*pEnterInfo);
						}
						ASSERT(pos);
						m_queries.FreeRemoveAt(pos);
						break;
					}
				}
			}
			else
				bDisplayErrorInStatusWindow = TRUE;
			break;
		}
	}

	// Set the szChannelName variable
	switch (pParse->uCode)
	{
		// IRC/IRCX common errors
		case ERR_USERONCHANNEL:
			// <nick> <channel> :is already on channel
			szChannelName = pParse->args[3];
			break;
				
		case ERR_NOSUCHNICK:
			// <nick/channel> :No such nick/channel
			szChannelName = pParse->args[2];
			break;

		case ERR_INVITEONLYCHAN:
			// <channel> :Cannot join channel (+i)
		case ERR_CHANNELISFULL:
			// <channel> :Cannot join channel (+l)
		case ERR_NOSUCHCHANNEL:
			// <channel> :No such channel
		case ERR_BANNEDFROMCHAN:
			// <channel> :Cannot join channel (+b)
		case ERR_BADCHANNELKEY:
			// <channel> :Cannot join channel (+k)
		case ERR_TOOMANYCHANNELS:
			// <channel> :You have joined too many channels
			szChannelName = pParse->args[2];
	}

	if (m_bIrcXServer)
		switch (pParse->uCode)
		{
			// IRCX only errors
			case ERR_NOJOINDYNAMIC:
				// <Channel> :Cannot join dynamic channels due to admin restriction
			case ERR_NODYNAMICCHANNELS:
				// <Channel> :Cannot create dynamic channels due to admin restriction
			case ERR_AUTHONLY:
				// <Channel> :Only authenticated users may join channel
			case ERR_CHANNELEXIST:
				// <Channel> :Channel already exists.
				szChannelName = pParse->args[2];
				break;

			case ERR_NOACCESS:
				// <*|ChannelName> :No access
				if (_tcscmp("*", pParse->args[2]))
					szChannelName = pParse->args[2];
				break;
		}
	else
		switch (pParse->uCode)
		{
			// MIC 1.0 errors
			case ERR_CANNOTJOINMICONLY:
				// <channel> :Cannot join MIC only channel with IRC client
			case ERR_CANNOTJOINFROMREMOTE:
				// <channel> :Cannot join channel from remote server (+r)
			case ERR_CANNOTCREATEDYNAMIC:
				// <channel> :Cannot create dynamic channels (admin)
			case ERR_ONLYAUTHCANJOIN:
				// <channel> :Only authenticated users may join channel
			case ERR_CANNOTJOINDYNAMIC:
				// <channel> :Cannot join dynamic channels due to admin restriction"
				szChannelName = pParse->args[2];
		}

	if (szChannelName)
	{
		// Try to find the opening channel in our list
		if (-1 == iRoomIndex)
			pEnterInfo = theApp.GetRoomInfoFromName((LPCTSTR) szChannelName, &iRoomIndex, FALSE /*bCloneOK*/);

		if (pEnterInfo && iRoomIndex > 0)
			theApp.RemoveRoomInfo(iRoomIndex);
	}

	if (bDisplayErrorInStatusWindow)
		pIrcPrint->SetFormat(PT_OFFSET, szLine, RGB(255,0,0), 3, TRUE);	
	else
		pIrcPrint->SetFormat(PT_NONE);
}


BOOL CIrcSocket::bFreeModeCell(LPCTSTR szChannel, LPCTSTR szNickname)
{
	// is there a queued ctSetChannelMode or ctSetUserMode cell with query purpose == qpComSetChannelMode or qpComSetUserMode?

	// Get oldest queued ctSetUserMode and ctSetChannelMode query object
	POSITION	pos1, pos2;
	LONG		lRank1, lRank2;
	CCQuery*	pQuery1 = NULL;
	CCQuery*	pQuery2 = NULL;

	if (szNickname || (!szNickname && !szChannel))
		pQuery1 = m_queries.FindQuery(ctSetUserMode, &pos1, &lRank1);

	if (szChannel || (!szNickname && !szChannel))
		pQuery2 = m_queries.FindQuery(ctSetChannelMode, &pos2, &lRank2);
		
	if (!pQuery1 || qpComSetUserMode != pQuery1->GetQueryPurpose())
		lRank1 = 0L;

	if (!pQuery2 || qpComSetChannelMode != pQuery2->GetQueryPurpose())
		lRank2 = 0L;
			
	if (lRank1 && (!lRank2 || lRank1 < lRank2))
	{
		ASSERT(pos1);
		m_queries.FreeRemoveAt(pos1);
		return TRUE;
	}
	if (lRank2 && (!lRank1 || lRank2 < lRank1))
	{
		ASSERT(pos2);
		m_queries.FreeRemoveAt(pos2);
		return TRUE;
	}
	return FALSE;
}


void 			
CIrcSocket::SetAuthentication(
UINT   nType, 
LPCSTR pszUserName,
LPCSTR pszPassword, 
LPCSTR pszCustomPkg)
{
	m_rgszUsrSecuPack.RemoveAll ();
	m_nAuthenticationType = nType;
	m_bAnonAllowed = FALSE;
	free (m_pszUserName);
	free (m_pszPassword);
	m_pszUserName = pszUserName != NULL ? strdup (pszUserName) : NULL;
	m_pszPassword = pszPassword != NULL ? strdup (pszPassword) : NULL;
	if (pszCustomPkg != NULL && m_nAuthenticationType == authtypeCustomPackages)
	{
		BOOL bEnd;
		CString str (pszCustomPkg);
		LPSTR szHeadTmp = str.GetBuffer (str.GetLength ());
		LPSTR szTmp = szHeadTmp;
		ASSERT('\0' != *szHeadTmp);
		do
		{
			if ((',' == *szTmp) || (bEnd = ('\0' == *szTmp)))
			{
				*szTmp = '\0';
				m_rgszUsrSecuPack.Add(szHeadTmp);
				if (!bEnd) 
					szHeadTmp = ++szTmp;
			}
			else
			{
				szTmp++;
			}
		} while (!bEnd);
		str.ReleaseBuffer ();
	}
}
