#include "stdafx.h"
#include <time.h>
#include <stdlib.h>
#include "ccommon.h"
#include "chat.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "chatprot.h"
#include "ircproto.h"
#include "ui.h"
#include "actions.h"
#include "query.h"
#include "protsupp.h"
#include "status.h"
#include "notipage.h"
#include "whisprbx.h"
#include "sounddlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CChatApp		theApp;

extern void			GetBanString(const char *szUserName, const char *szHostName, CString& strBan);
extern BOOL			bFindAndPlaySound(const char *, BOOL, BOOL);
extern BOOL			bLegalToSend(BOOL bPrivMsg = FALSE);
extern char*		GetNextStart(char *szString);


BOOL bGetNextRange(LPTSTR *pszStr, UINT *puMin, UINT *puMax)
{
	ASSERT(pszStr, "pszStr is NULL in bGetNextRange");
	ASSERT(*pszStr, "*pszStr is NULL in bGetNextRange");
	ASSERT(puMin, "puMin is NULL in bGetNextRange");
	ASSERT(puMax, "puMax is NULL in bGetNextRange");

	LPTSTR	szTmp = *pszStr, szStart;
	TCHAR	chTmp;

	szTmp = GetNextStart(szTmp);	// read uMinLine
	if (!isdigit(*szTmp))
	{
		#ifdef DEBUG
			if (*szTmp)	TRACE("Bad file line interval in bGetNextRange\n");
		#endif
		goto exit;
	}
	szStart = szTmp;
	while (*szTmp && isdigit(*szTmp))
		szTmp++;
	chTmp = *szTmp;
	*szTmp = g_chEOS;
	*puMin = atoi(szStart);
	*szTmp = chTmp;
	szTmp = GetNextStart(szTmp);
	switch (*szTmp)
	{
	case g_chComma:
		szTmp++;
	case g_chEOS:
		*puMax = *puMin;		// just read one line
		break;
	case g_chDash:				// read uMaxLine
		szTmp++;
		szTmp = GetNextStart(szTmp);
		if (!isdigit(*szTmp))
		{
			TRACE("Bad file line interval in bGetNextRange\n");
			goto exit;
		}
		szStart = szTmp;
		while (*szTmp && isdigit(*szTmp))
			szTmp++;
		chTmp = *szTmp;
		*szTmp = g_chEOS;
		*puMax = atoi(szStart);
		*szTmp = chTmp;
		szTmp = GetNextStart(szTmp);
		if (*szTmp == g_chComma)
			szTmp++;
		break;
	default:
		TRACE("Bad file line interval in bGetNextRange\n");
		goto exit;
	}
	*pszStr = szTmp;

	if (*puMin > *puMax)
	{
		UINT uExchange = *puMin;
		*puMin = *puMax;
		*puMax = uExchange;
	}
	return TRUE;

exit:
	*puMin = *puMax = 0;
	return FALSE;
}


BOOL bInPtrArray(CPtrArray& rgpItems, PVOID pItem)
{
	INT iItems = rgpItems.GetSize();

	for (INT iIndex = 0; iIndex < iItems; iIndex++)
		if (pItem == rgpItems.GetAt(iIndex))
			return TRUE;
	return FALSE;
}


void TrimQuotes(CString& strIn)
{
	INT cbLen = strIn.GetLength();

	if (cbLen >= 2 && strIn[0] == '\"' && 
		OurMbsRChr(((LPCTSTR)strIn)+1, '\"') == ((LPCSTR) strIn)+cbLen-1)
		strIn = strIn.Mid(1, cbLen-2);
}


CString GetNextToken(CString& strTokens, CHAR chSep, BOOL bTrim)
{
	INT		iIndex;
	CString	strToken;

	while (!strTokens.IsEmpty())
	{
		if (bTrim)
			strTokens.TrimLeft();
		if ((iIndex = strTokens.Find(chSep)) > -1)
		{
			strToken = strTokens.Left(iIndex);
			if (bTrim)
				strToken.TrimRight();
			strTokens = strTokens.Mid(iIndex+1);
			if (!strToken.IsEmpty())
			{
				while ((bTrim && strTokens[0] == g_chSpace) || strTokens[0] == chSep)
					strTokens = strTokens.Mid(1);
				TrimQuotes(strToken);
				return strToken;
			}
		}
		else
		{
			if (bTrim)
				strTokens.TrimRight();
			strToken = strTokens;
			TrimQuotes(strToken);
			strTokens.Empty();
			return strToken;
		}
	}
	return strToken;
}


CString StrExtractNickname(CString strIdentity)
{
	CString strNickname;

	if (strIdentity.Find('!') != -1 && strIdentity.Find('@') != -1)
		// this is a full identity
		strNickname = strIdentity.Left(strIdentity.Find('!'));
	else
		strNickname = strIdentity;

	return strNickname;
}


CString StrExtractIdent(CString strIdentity)
{
	CString strIdent;

	if (strIdentity.Find('!') != -1 && strIdentity.Find('@') != -1)
		// this is a full identity
		strIdent = strIdentity.Mid(strIdentity.Find('!')+1);

	return strIdent;
}


BOOL bKeyEventParam(CString& strParam, enumKeyEventParam kep)
{
	BOOL bRet = FALSE;

	switch (kep)
	{
	case kepMyActivatedRoom:
	case kepMyInactivatedRooms:
	{
		// strParam is an encoded channel name as well as strCurrentChannel
		CChatDoc* pDoc = LookupDoc(strParam);

		bRet = (currentRoom && strCurrentChannel == strParam) || (theApp.m_pExitingDoc == (CChatDoc*) pDoc && pDoc);
		if (kep == kepMyInactivatedRooms)
			bRet = pDoc && !bRet;
		break;
	}

	case kepAnyone:
	case kepAny:
		bRet = TRUE;
		break;

	case kepAnyOfMyRooms:
		bRet = (LookupDoc(strParam) != NULL);
		break;

	case kepMe:
	case kepAnyoneButMe:
	{
		// strParam is an encoded nickname or full identity, GetMyNickName() is encoded too
		CString strEncodedNickname = StrExtractNickname(strParam);
		bRet = (0 == strEncodedNickname.CompareNoCase(GetMyNickName()));
		if (kep == kepAnyoneButMe)
			bRet = !bRet;
		break;
	}

	default:
		ASSERT(FALSE);
	}
	return bRet;
}


BOOL bNetValid(CString strNetParam)
{
	CString strCurrentServer = GetMyServer();
	CString strService;
	UINT	cbCurSer, cbService;

	ASSERT(!strNetParam.IsEmpty());
	ASSERT(!strCurrentServer.IsEmpty());

	theApp.m_listChatServices.GetServiceNameFromDisplayName(strNetParam, strService);

	ASSERT(!strService.IsEmpty());

	if (0 == strService.CompareNoCase(strCurrentServer))
		return TRUE;

	if (strCurrentServer[0] != '/')
		// the current server is not part of any server group
		return FALSE;

	cbCurSer = strCurrentServer.GetLength();
	cbService = strService.GetLength();

	if (strService[0] == '/')
	{
		// strService is a server group
		// is strCurrentServer of form strService + "/server" ?
		if (cbCurSer > cbService && strCurrentServer.GetAt(cbService) == '/' && 0 == strService.CompareNoCase(strCurrentServer.Left(cbService)))
			return TRUE;
	}
	else
	{
		// strService is a single server
		// is strCurrentServer of form "//group/" + strService ?
		if (cbCurSer > cbService + 3 && strCurrentServer.GetAt(cbCurSer-cbService-1) == '/' && 0 == strService.CompareNoCase(strCurrentServer.Mid(cbCurSer-cbService)))
			return TRUE;
	}

	return FALSE;
}


BOOL bRndEventParam(CString& strValue, CString& strFilter, PPRUSERMATCH pPrUserMatch, WORD wFlags, enumParamType pt)
{
	BOOL bRet = FALSE;

	switch (pt)
	{
	case ptNickname:
	{
		// strFilter and pPrUSerMatch use decoded nicknames
		// strValue is encoded

		CString strDecodedNickname, strUserName, strHostName;
		INT		iUserStart, iHostStart;

		if ((iUserStart = strValue.Find('!')) != -1 && (iHostStart = strValue.Find('@')) != -1)
		{
			strDecodedNickname = DecodeNickForScreen(strValue.Left(iUserStart));
			strUserName = strValue.Mid(iUserStart+1, iHostStart-iUserStart-1);
			strHostName = strValue.Mid(iHostStart+1);
		}
		else
			strDecodedNickname = DecodeNickForScreen(strValue);

		if (pPrUserMatch && !strDecodedNickname.IsEmpty() && !strUserName.IsEmpty() && !strHostName.IsEmpty())
		{
			// need to use mask nick!user@host - first we try with the quoted nick
			if (!(bRet = bIsMatch(pPrUserMatch, strDecodedNickname, strUserName, strHostName)))
			{
				// no match with quoted nick - let's try without the quotes
				strDecodedNickname = DecodeNick(strValue.Left(iUserStart));
				bRet = bIsMatch(pPrUserMatch, strDecodedNickname, strUserName, strHostName);
			}
		}
		else
			bRet = (0 == strDecodedNickname.CompareNoCase((LPCTSTR) strFilter));
		break;
	}
	case ptMessage:
	{
		// strFilter needs to be part of strValue
		bRet = (NULL != StrFindSubString(strValue /*pszFindIn*/, 
										 strFilter /*pszFind*/, 
										 wFlags & g_wMatchWord /*bWholeWord*/, 
										 !(wFlags & g_wMatchCase /*bIgnoreCase*/)));
		break;
	}
	case ptRoomName:
	{
		// strFilter is a decoded channel name
		// strValue is an encoded channel name
		CString strEncodedFilter = EncodeChan(strFilter);
		bRet = (0 == strValue.CompareNoCase((LPCTSTR) strEncodedFilter));
		break;
	}
	case ptServerName:
	{
		bRet = bNetValid(strFilter);
		break;
	}
	default:
		ASSERT(FALSE);
	}

	return bRet;
}


CString	StrGetKeyEventParam(enumParamType pt)
{
	switch (pt)
	{
	case ptNickname:
		return CString(GetMyScreenName());  // GetMyName()
	case ptServerName:
		return CString(GetMyServer());
	case ptRoomName:
		if (currentRoom)
			return currentRoom->m_strPrettyChannel;
		else
			return CString("");
	default:
		ASSERT(FALSE);
		return CString("");
	}
}


CString	StrGetKeyActionParam(enumKeyActionParam kap, 
							 CString& strEventServer, 
							 CString& strEventIdentity, 
							 CString& strEventChannel,
							 CString& strEventRecipients,
							 CString& strEventCLMessage)
{
	// strEventIdentity, strEventChannel and strEventMessage are encoded strings

	CString	strRet;

	switch (kap)
	{
	case kapMyActivatedRoom:
		if (currentRoom)
			strRet = currentRoom->m_strPrettyChannel;
		break;
	
	case kapAll:
		strRet = CString(g_szAllLines);
		break;

	case kapEventMessage:
		strRet = strEventCLMessage;
		break;

	case kapEventNickname:
		strRet = DecodeNickForScreen(StrExtractNickname(strEventIdentity));	// was DecodeNick(...)
		break;

	case kapEventRecipients:
		strRet = strEventRecipients;
		break;

	case kapEventRoom:
		strRet = DecodeChan(strEventChannel);	// don't know if we need to set the MIC flag
		break;

	case kapEventServer:
		strRet = strEventServer;
		break;
	
	case kapRandom:
		strRet = CString(g_szRandomLine);
		break;

	case kapMe:
		strRet = GetMyScreenName(); // GetMyName();
		break;

	default:
		ASSERT(FALSE);
	}
	return strRet;
}


BOOL bBeep(CString& strBeepCount)
{
	INT		iBeepCount = atoi(strBeepCount);
	BOOL	bRet = TRUE;

	while (iBeepCount > 0)
	{
		MessageBeep(0xFFFFFFFF);
		iBeepCount--;
		if (iBeepCount > 0)	// might want to do this in another thread to avoid the synchronous pause.
			Sleep(100);
	}
	return bRet;
}


BOOL bInvite(CString& strNickname, CString& strChannel)
{
	if (0 == strNickname.CompareNoCase(GetMyNickName()))	// We don't want to invite ourselves
		return TRUE;

	CChatDoc*	pDoc = LookupDoc(EncodeChan(strChannel));	// Channel needs to be encoded

	if (pDoc)
	{
		ASSERT(pDoc->m_proto);
		pDoc->m_proto->ChatSendInvitation(strNickname);		// Nickname is encoded too
	}

	return TRUE;
}


BOOL bExecuteMacro(CString& strMacroName, CString& strEncodedChannelName, CUserInfo* pUI)
{
	for (INT iMacroNum = 0; iMacroNum < NMACROS; iMacroNum++)
		if (theApp.m_macros[iMacroNum].m_bDefined && 0 == strMacroName.Compare(theApp.m_macros[iMacroNum].m_strName))
		{
			theApp.m_macros[iMacroNum].Invoke(strEncodedChannelName.IsEmpty() ? NULL : (LPCTSTR) strEncodedChannelName, pUI, TRUE /*bInvokedByRule*/);
			break;
		}
	return TRUE;
}


BOOL bSendToChannel(CCActionContext* pActionCtx, USHORT uModes)
{
	CString			strChannelToSendTo		= pActionCtx->GetFinalActionParam(0);
	CString			strMessageToSend		= pActionCtx->GetFinalActionParam(1);
	CDWordArray*	prgdwFinalMsgFormatting = pActionCtx->GetFinalMsgFormatting();

	ASSERT(!strMessageToSend.IsEmpty());
	ASSERT(!strChannelToSendTo.IsEmpty());

	// strChannelToSendTo is decoded channel name
	// strMessageToSend is control less message
	g_rgpuiWhisperees.RemoveAll();

	BOOL bRet = bChatSendText(strMessageToSend, uModes, TRUE /*bEcho*/, prgdwFinalMsgFormatting, EncodeChan(strChannelToSendTo), TRUE /*bWhispereesFilled*/);
	ASSERT(bRet);

	return TRUE;
}


BOOL bWhisperToUser(CIrcProto* pProto, CCActionContext* pActionCtx)
{
	CString			strDecodedNickname, strIdent, strFullName;
	CString			strUsersToSendTo		= pActionCtx->GetFinalActionParam(0);
	CString			strMessageToSend		= pActionCtx->GetFinalActionParam(1);
	CDWordArray*	prgdwFinalMsgFormatting = pActionCtx->GetFinalMsgFormatting();
	CUserInfo*		pUI = NULL;
	CChatDoc*		pDoc = NULL;
	LPCTSTR			szEncodedNick;
	BOOL			bRet;
	CPtrArray		rgpuis;

	// strUsersToSendTo are decoded nicknames
	// strMessageToSend is control less message

	ASSERT(!strMessageToSend.IsEmpty());
	ASSERT(!strUsersToSendTo.IsEmpty());

	while (!strUsersToSendTo.IsEmpty())
	{
		strFullName = GetNextToken(strUsersToSendTo, ';', TRUE);
		strDecodedNickname = StrExtractNickname(strFullName);
		strIdent = StrExtractIdent(strFullName);
		ASSERT(!strDecodedNickname.IsEmpty());

		if (pProto->IsIRCX() && bExtendedNickname(strDecodedNickname))
			szEncodedNick = EncodeNick(strDecodedNickname);
		else
			szEncodedNick = strDecodedNickname;
		
		ASSERT(szEncodedNick);

		pUI = PuiFromDocNickIdent(&pDoc, (char*) szEncodedNick, strIdent, TRUE /*bSkipObscuredChannels*/, TRUE /*bAddExternalIfNotThere*/);

		if (pUI && (!pDoc || pUI != pDoc->m_puiSelf) && !bInPtrArray(rgpuis, pUI))
		{
			rgpuis.Add(pUI);
	
			WhisperBox(pUI, FALSE /*bGiveFocus*/, FALSE /*bRestore*/);
			bRet = bWhisperInBox("", strMessageToSend, prgdwFinalMsgFormatting, BM_WHISPER);
			ASSERT(bRet);
		}
	}

	rgpuis.RemoveAll();

	return TRUE;
}


BOOL bWhisperToUserInChannel(CIrcProto* pProto, CCActionContext* pActionCtx)
{
	ASSERT(pActionCtx);
	ASSERT(pProto);

	CString			strDecodedNickname;
	CString			strIdents				= pActionCtx->GetFinalActionParam(0);
	CString			strChannelToSendTo		= pActionCtx->GetFinalActionParam(1);
	CString			strMessageToSend		= pActionCtx->GetFinalActionParam(2);
	CDWordArray*	prgdwFinalMsgFormatting = pActionCtx->GetFinalMsgFormatting();
	CUserInfo*		pUI = NULL;
	CChatDoc*		pDoc = NULL;
	CPtrArray		rgpuis;
	LPCTSTR			szEncodedNick;
	BOOL			bRet;

	// strChannelToSendTo is decode channel
	// strMessageToSend is control less message
	ASSERT(!strMessageToSend.IsEmpty());
	ASSERT(!strChannelToSendTo.IsEmpty());

	pDoc = LookupDoc(EncodeChan(strChannelToSendTo));
	if (!pDoc || pDoc->GetConnectionStatus() != CX_INCHANNEL)
		return TRUE;	// No such open channel

	if (!pDoc->m_bObscured)
		g_rgpuiWhisperees.RemoveAll();

	while (!strIdents.IsEmpty())
	{
		strDecodedNickname = StrExtractNickname(GetNextToken(strIdents, ';', TRUE));
		ASSERT(!strDecodedNickname.IsEmpty());

		if (pProto->IsIRCX() && bExtendedNickname(strDecodedNickname))
			szEncodedNick = EncodeNick(strDecodedNickname);
		else
			szEncodedNick = strDecodedNickname;

		pUI = LookupPui(szEncodedNick, pDoc);

		if (pUI && pUI != pDoc->m_puiSelf && !pUI->IsDeparted() && !bInPtrArray(rgpuis, pUI))
		{
			rgpuis.Add(pUI);
			if (pDoc->m_bObscured)
			{
				WhisperBox(pUI, FALSE /*bGiveFocus*/, FALSE /*bRestore*/);
				bRet = bWhisperInBox("", strMessageToSend, prgdwFinalMsgFormatting, BM_WHISPER);
				ASSERT(bRet);
			}
			else
				g_rgpuiWhisperees.Add(pUI);
		}
	}

	if (!pDoc->m_bObscured && g_rgpuiWhisperees.GetSize() > 0)
	{
		ASSERT(pDoc->m_proto);
		bRet = bChatSendText(strMessageToSend, BM_WHISPER, TRUE /*bEcho*/, prgdwFinalMsgFormatting, pDoc->m_proto->m_strChannel, TRUE /*bWhispereesFilled*/);
		ASSERT(bRet);
	}

	rgpuis.RemoveAll();

	return TRUE;
}


BOOL bSendSound(CCActionContext* pActionCtx)
{
	CString			strChannelToSendTo		= pActionCtx->GetFinalActionParam(0);
	CString			strMessageToSend		= pActionCtx->GetFinalActionParam(1);
	CString			strSoundToPlay			= pActionCtx->GetFinalActionParam(2);
	CDWordArray*	prgdwFinalMsgFormatting = pActionCtx->GetFinalMsgFormatting();

	// Default extension is .WAV
	if (!OurMbsChr(strSoundToPlay, '.'))
		strSoundToPlay += CString(".") + GetSupportedSoundTypes ()[0];

	return bChatSendSound(strSoundToPlay, strMessageToSend, prgdwFinalMsgFormatting, TRUE /*bEcho*/, BM_SAY, EncodeChan(strChannelToSendTo));
}


BOOL bSendOrWhisperFileLine(CIrcProto* pProto, CCActionContext* pActionCtx, BOOL bWhisper)
{
	CStdioFile	file;
	CString		strMessageToSend, strDecodedNickname, strIdent, strFullName;
	CString		strUsersToSendToTmp, strUsersToSendTo = pActionCtx->GetFinalActionParam(0);
	CString		strTextFileName	= pActionCtx->GetFinalActionParam(1);
	LPTSTR		szFree, szLineNumber = strdup(pActionCtx->GetFinalActionParam(2));
	UINT		uCnt = 1, uMinLine, uMaxLine;
	LPCTSTR		szEncodedNick;
	CUserInfo*	pUI = NULL;
	CChatDoc*	pDoc;
	CPtrArray	rgpuis;
	BOOL		bRet;

	if (!szLineNumber)
		return FALSE;

	ASSERT(!strUsersToSendTo.IsEmpty());

	if (strTextFileName.GetLength() >= 2 && strTextFileName[1] != ':')
		// strTextFileName is not an absolute path
		strTextFileName	= theApp.m_strBaseDir + "\\" + strTextFileName;

	if (!file.Open(strTextFileName, CFile::modeRead | CFile::shareDenyWrite | CFile::typeText))
	{
		TRACE("Could not open text file in bSendOrWhisperFileLine\n");
		free(szLineNumber);
		return TRUE;
	}

	szFree = szLineNumber;	// to be freed later

	while (*szLineNumber)
	{
		if (!_tcscmp(szLineNumber, g_szRandomLine))
		{
			// need to find a random line
			*szLineNumber = 0;
			uCnt = 0;
			while (file.ReadString(strMessageToSend))
				uCnt++;
			uMinLine = uMaxLine = (UINT) (uCnt * (((float) rand())/(RAND_MAX+1)) + 1);
			uCnt = 999999; // force to seek to begin
		}
		else
			if (!bGetNextRange(&szLineNumber, &uMinLine, &uMaxLine))
			{
				TRACE("Bad file line interval in bSendOrWhisperFileLine\n");
				goto exit;
			}

		if (uCnt > uMinLine)
		{
			uCnt = 1;
			file.SeekToBegin();
		}
		while (uCnt < uMinLine && file.ReadString(strMessageToSend))
			uCnt++;
		if (uCnt < uMinLine)
			continue;
		while (uCnt <= uMaxLine && file.ReadString(strMessageToSend))
		{
			if (!strMessageToSend.IsEmpty())
			{
				if (bWhisper)
				{
					strUsersToSendToTmp = strUsersToSendTo;
					while (!strUsersToSendToTmp.IsEmpty())
					{
						strFullName = GetNextToken(strUsersToSendToTmp, ';', TRUE);
						strDecodedNickname = StrExtractNickname(strFullName);
						strIdent = StrExtractIdent(strFullName);
						ASSERT(!strDecodedNickname.IsEmpty());

						if (pProto->IsIRCX() && bExtendedNickname(strDecodedNickname))
							szEncodedNick = EncodeNick(strDecodedNickname);
						else
							szEncodedNick = strDecodedNickname;
						
						ASSERT(szEncodedNick);

						pDoc = NULL;
						pUI  = PuiFromDocNickIdent(&pDoc, (char*) szEncodedNick, strIdent, TRUE /*bSkipObscuredChannels*/, TRUE /*bAddExternalIfNotThere*/);

						if (pUI && (!pDoc || pUI != pDoc->m_puiSelf) && !bInPtrArray(rgpuis, pUI))
						{
							rgpuis.Add(pUI);

							if (!pDoc)
							{
								WhisperBox(pUI, FALSE /*bGiveFocus*/, FALSE /*bRestore*/);
								bRet = bWhisperInBox("", strMessageToSend, NULL, BM_WHISPER);
								ASSERT(bRet);
							}
							else
							{
								g_rgpuiWhisperees.SetSize(1);
								g_rgpuiWhisperees[0] = pUI;
								ASSERT(pDoc->m_proto);
								bRet = bChatSendText(strMessageToSend, BM_WHISPER, TRUE /*bEcho*/, NULL, pDoc->m_proto->m_strChannel, TRUE /*bWhispereesFilled*/);
								ASSERT(bRet);
							}
						}
					}
					rgpuis.RemoveAll();
				}
				else
				{
					bRet = bChatSendText(strMessageToSend, BM_SAY, TRUE /*bEcho*/, NULL, EncodeChan(pActionCtx->GetFinalActionParam(0)), TRUE /*bWhispereesFilled*/);
					ASSERT(bRet);
				}
			}
			uCnt++;
		}
	}

exit:
	file.Close();

	free(szFree);

	return TRUE;
}


BOOL bExecuteAction(CCDynaRules* pDynaRules, CCRule* pRule, CCActionContext* pActionCtx)
{
	ASSERT(pActionCtx);

	BOOL		bRet = TRUE;
	CString		strEventNickname;
	CChatDoc*	pDoc = NULL;
	CIrcProto*	pProto = NULL;
	CUserInfo*	pUI = NULL;

	pProto = GetIrcProto();
	ASSERT(pProto);

	strEventNickname = StrExtractNickname(pActionCtx->GetCachedIdentity());	// Encoded event nickname

	switch (pActionCtx->GetActionID())
	{
	case aBan:
	case aGetLagTime:
	case aGetLocalTime:
	case aGetProfile:
	case aGetVersion:
	case aKick:
	case aLeaveRoom:
	case aMakeHost:
		pDoc = LookupDoc(pActionCtx->GetCachedChannel());		// Channel is encoded
		if (!pDoc)
			return TRUE;
	}

	switch (pActionCtx->GetActionID())
	{
	case aBan:
	case aGetLagTime:
	case aGetProfile:
	case aGetVersion:
	case aMakeHost:
		pUI = LookupPui((LPCTSTR) strEventNickname, pDoc);		// Nickname is encoded
		break;
	}

	switch (pActionCtx->GetActionID())
	{
	case aActivateRuleSet:
	{	// activation status might be overwritten if the RuleSets/Rules property sheet is currently open 
		CCDynaRules*		pDynaRulesTmp = pDynaRules ? pDynaRules : &theApp.m_dynaRules;
		CCRuleSet*			pRuleSet = pDynaRulesTmp->GetRuleSetFromName((LPCTSTR) pActionCtx->GetFinalActionParam(0));	// REGISB was GetActionParam(0)
		enumKeyActionParam	kap = pActionCtx->GetActionKeyParam(1);	// get kapYes or kapNo
		ASSERT(kap == kapYes || kap == kapNo);
		if (pRuleSet)
			if (kap == kapYes)
				pRuleSet->Activate();
			else
				pRuleSet->Desactivate();
		break;
	}
	case aBan:
		if (pDoc->m_puiSelf && pDoc->m_puiSelf->IsOperator() && 0 != strEventNickname.CompareNoCase(GetMyNickName()))
		{
			INT		iUserStart, iHostStart;

			if ((iUserStart = pActionCtx->GetCachedIdentity().Find('!')) != -1 &&
				(iHostStart = pActionCtx->GetCachedIdentity().Find('@')) != -1)
			{
				CString strBanPattern, strUserName, strHostName;

				strUserName = pActionCtx->GetCachedIdentity().Mid(iUserStart+1, iHostStart-iUserStart-1);
				strHostName = pActionCtx->GetCachedIdentity().Mid(iHostStart+1);
				GetBanString(strUserName, strHostName, strBanPattern);
				pDoc->m_proto->ChatBanUser(strBanPattern, TRUE /*bBan*/);
			}
			else
				if (pUI)
					pDoc->m_proto->ChatBanUser(pUI);
		}
		break;

	case aBeep:
		bRet = bBeep(pActionCtx->GetFinalActionParam(0));
		break;

	case aConnect:
		// Make sure we are disconnected...
		if (!GetDefaultProto() || GetDefaultProto()->GetConnectionStatus() == CX_DISCONNECTED)
		{
			BOOL bCXPrompt = GetCXPrompt();
			ChatSetCXPrompt(FALSE);			// silent reconnection
			ReconnectToServer(pActionCtx->GetFinalActionParam(0), pActionCtx->GetFinalActionParam(1));
			ChatSetCXPrompt(bCXPrompt);		// restore flag
		}
		break;

	case aDisconnect:
		ChatServerDisconnect(TRUE /*bCheckRules*/);
		break;

	case aDoNotDisplay:
		ASSERT(pDynaRules);
		pDynaRules->AddFlag(g_wDoNotDisplay);
		break;

	case aExecuteMacro:
	{
		CString		strIdent = StrExtractIdent(pActionCtx->GetCachedIdentity());
		CChatDoc*	pDoc = NULL;
		CUserInfo*	pUITmp = PuiFromDocNickIdent(&pDoc, (LPTSTR) (LPCTSTR) strEventNickname, (LPCTSTR) strIdent /*szUserIdent*/, FALSE /*bSkipObscuredChannels*/, FALSE /*bAddExternalIfNotThere*/);
		bRet = bExecuteMacro(pActionCtx->GetFinalActionParam(0), pActionCtx->GetCachedChannel(), pUITmp);
		break;
	}

	case aGetIdentity:
		// using encoded nickname
		pProto->ChatGetIdentity(NULL, (LPCTSTR) strEventNickname);
		break;

	case aGetLagTime:
		if (pUI)
			pProto->ChatPingUser(pUI);
		break;

	case aGetLocalTime:
	{
		LPCTSTR	szEncodedNick;
		CString strDecodedNickname = pActionCtx->GetFinalActionParam(0);

		if (pProto->IsIRCX() && bExtendedNickname(strDecodedNickname))
			szEncodedNick = EncodeNick(strDecodedNickname);
		else
			szEncodedNick = strDecodedNickname;
		
		ASSERT(szEncodedNick);

		if (pUI = LookupPui(szEncodedNick, pDoc))
			pProto->ChatGetLocalTime(pUI);
		break;
	}

	case aGetProfile:
		if (pUI && (eOnJoin == pActionCtx->GetEventID() || pUI->IsComicUser()))
			pProto->ChatGetInfo(pUI);
		break;

	case aGetVersion:
		if (pUI)
			pProto->ChatGetVersion(pUI);
		break;

	case aHighlightMessage:
		ASSERT(pRule);
		ASSERT(pDynaRules);
		pDynaRules->AddFlag(g_wHighlight);
		pDynaRules->AddFlag(pRule->iGetHighlightTypeIndex(pActionCtx->GetFinalActionParam(0)) << 8);
		break;

	case aIgnore:
		if (0 != strEventNickname.CompareNoCase(GetMyNickName()))
			// using encoded nickname
			pProto->DoIgnoreUser(pUI, 
								 TRUE /*bIgnore*/,
								 FALSE /*bAutoIgnore*/,
								 (LPCTSTR) strEventNickname);
		break;

	case aInvite:
		bRet = bInvite(strEventNickname, pActionCtx->GetFinalActionParam(0));
		break;

	case aJoinRoom:
		g_bEnterOnCreate = FALSE;
		bRet = bSwitchToRoom(pActionCtx->GetFinalActionParam(0), NULL, NULL, 0L, TRUE, TRUE /*bCreateRoomInfo*/);
		break;

	case aKick:
		if (pDoc->m_puiSelf && pDoc->m_puiSelf->IsOperator() && 0 != strEventNickname.CompareNoCase(GetMyNickName()))
			pDoc->m_proto->ChatKickUser(strEventNickname, pActionCtx->GetFinalActionParam(0));
		break;

	case aLeaveRoom:
		pDoc->OnLeave();
		break;

	case aMakeHost:
		if (pUI && pDoc->m_puiSelf && pDoc->m_puiSelf->IsOperator() && 0 != strEventNickname.CompareNoCase(GetMyNickName()))
			pDoc->m_proto->ChatSetOperator(pUI, UM_HOST);
		break;

	case aNotifyDialog:
		bRet = AfxMessageBox(pActionCtx->GetFinalActionParam(0), MB_OK|MB_ICONINFORMATION);
		break;

	case aPlaySound:
		bRet = bFindAndPlaySound(pActionCtx->GetFinalActionParam(0), FALSE /*bStopSound*/, FALSE /*bSemiSync*/);
		break;

	case aReplaceMessage:
		ASSERT(pRule);
		ASSERT(pDynaRules);
		bRet = pDynaRules->bReplaceMessage(pRule);
		break;

	case aSendAction:
		bRet = bSendToChannel(pActionCtx, BM_ACTION);
		break;

	case aSendFileLine:
		bRet = bSendOrWhisperFileLine(pProto, pActionCtx, FALSE /*bWhisper*/);
		break;

	case aSendMessage:
		bRet = bSendToChannel(pActionCtx, BM_SAY);
		break;

	case aSendSound:
		bRet = bSendSound(pActionCtx);
		break;

	case aSendThought:
		bRet = bSendToChannel(pActionCtx, BM_THINK);
		break;

	case aSendWhisper:
		bRet = bWhisperToUser(pProto, pActionCtx);
		break;

	case aSendWhisperInRoom:
		bRet = bWhisperToUserInChannel(pProto, pActionCtx);
		break;

	case aWhisperFileLine:
		bRet = bSendOrWhisperFileLine(pProto, pActionCtx, TRUE /*bWhisper*/);
		break;

	default:
		ASSERT(FALSE);
		bRet = FALSE;
	}

	return bRet;
}


BOOL bRuleDaemonQuery(CCRule* pRule)
{
	CIrcProto* pProto = GetIrcProto();

	ASSERT(pProto);
	ASSERT(pRule);

	if (pProto->GetConnectionStatus() == CX_DISCONNECTED || 
		pProto->GetConnectionStatus() == CX_CONNECTING)
		return TRUE;

	ASSERT(pRule->bActive());

	switch (pRule->GetEvent()->GetID())
	{
		case eOnConnect:
			return pProto->bExecuteQuery(qpOnConnectEvent, ctWho, dtRule, pRule, "", pRule->GetEventParam(0));
		case eOnDisconnect:
			return pProto->bExecuteQuery(qpOnDisconnectEvent, ctWho, dtRule, pRule, "", pRule->GetEventParam(0));
		case eOnNewRoom:
			return pProto->bExecuteQuery(qpOnNewRoomEvent, pProto->IsIRCX() ? ctListX : ctList, dtRule, pRule, EncodeChan(pRule->GetEventParam(0)), "");
		default:
			ASSERT(FALSE);
			return FALSE;
	}

	return TRUE;
}


CString StrAddWildcards(CString strIn, UCHAR uOperator, BOOL bIsNick)
{
	switch (uOperator)
	{
	case g_uAny:
		return "*";
	case g_uEquals:
		return strIn;
	case g_uContains:
	{
		if (strIn[0] == '\'')
			return CString("'*") + strIn.Mid(1) + "*";
		else
			return CString("*") + strIn + "*";
	}
	case g_uStartsWith:
		return strIn + "*";
	case g_uEndsWith:
	{
		if (strIn[0] == '\'')
			return CString("'*") + strIn.Mid(1);
		else
			return CString("*") + strIn;
	}
	default:
		ASSERT(FALSE);
		return strIn;
	}
}


BOOL bNotifDaemonQuery(CCNotif* pNotif)
{
	CIrcProto*	pProto = GetIrcProto();
	CString		strNicknameMask, strDecodedNickname, strEncodedNickname;

	ASSERT(pProto);
	ASSERT(pNotif);

	if (pProto->GetConnectionStatus() == CX_DISCONNECTED || 
		pProto->GetConnectionStatus() == CX_CONNECTING)
		return TRUE;

	ASSERT(pNotif->bActive());

	strDecodedNickname = pNotif->GetParam(g_uNickname);
	TrimQuotes(strDecodedNickname);

	if (pProto->IsIRCX() && !strDecodedNickname.IsEmpty() && bExtendedNickname(strDecodedNickname))
		strEncodedNickname = EncodeNick(strDecodedNickname);
	else
		strEncodedNickname = strDecodedNickname;
		
	strNicknameMask = StrAddWildcards(strEncodedNickname, pNotif->GetOperator(g_uNickname), TRUE) + "!" +
					  StrAddWildcards(pNotif->GetParam(g_uUserName), pNotif->GetOperator(g_uUserName), FALSE) + "@" +
					  StrAddWildcards(pNotif->GetParam(g_uHostName), pNotif->GetOperator(g_uHostName), FALSE);

	return pProto->bExecuteQuery(qpOnNotification, ctWho, dtNotif, pNotif, "", strNicknameMask);
}


CNotificationUsers* CreateNotificationBox() 
{
	ASSERT(!cui.m_pvNotifBox);

	CNotificationUsers* pNotifBox = new CNotificationUsers();
	if (!pNotifBox)
		return FALSE;
	cui.m_pvNotifBox = (void*) pNotifBox;
	// use desktop as parent, since we don't want this win to be always on top of main window
	CWnd *pParentWnd = CWnd::GetDesktopWindow();
	if (!pNotifBox->Create(IDD_NOTIFICATIONUSERS, pParentWnd))
	{
		delete pNotifBox;
		return NULL;
	}
	HICON hIcon = theApp.LoadIcon(IDI_NOTIF);
	if (hIcon)
	{
		pNotifBox->SetIcon(hIcon, TRUE);
		pNotifBox->SetIcon(hIcon, FALSE);
	}
	pNotifBox->SetPostCreate(TRUE);
	if (!IsRectEmpty (&theApp.m_rectNotifs))
	{
		MakeRectVisibleOnScreen (&theApp.m_rectNotifs);
		pNotifBox->MoveWindow (&theApp.m_rectNotifs);
	}
	else
	{
		// The default size should actually be bigger than the dialog size in
		// the resource, because that size is the minimum size of the dialog.
		CRect rectSize;
		pNotifBox->GetWindowRect (&rectSize);
		pNotifBox->SetWindowPos (NULL, 0, 0, rectSize.Width () * 3 / 2, rectSize.Height () * 2,
			SWP_NOMOVE | SWP_NOACTIVATE | SWP_NOZORDER);
	}
	pNotifBox->ShowWindow(SW_SHOWNORMAL);
	return pNotifBox;
}


void DestroyNotificationBox() 
{
	if (cui.m_pvNotifBox)
	{
		delete (CNotificationUsers*) (cui.m_pvNotifBox);
		cui.m_pvNotifBox = NULL;
	}
}


BOOL bDisplayNotifications(CCDynaNotifs* pDynaNotifs)
{
	ASSERT(pDynaNotifs);

	CNotificationUsers*	pNotifBox;

	if (pDynaNotifs->GetModifiedUsersCount())
	{
		// Some new or gone user(s) to display
		if ((pNotifBox = GetNotifBox()) == NULL)
		{
			pNotifBox = CreateNotificationBox();
			pNotifBox->bFillList(pDynaNotifs->GetNotifUsersArray(), pDynaNotifs->GetModifiedUsersCount());
		}
		else
		{
			pNotifBox->bFillList(pDynaNotifs->GetNotifUsersArray(), pDynaNotifs->GetModifiedUsersCount());
			pNotifBox->bSignalNewEntries();
		}
	}

	pDynaNotifs->ResetModifiedUsersCount();

	return pNotifBox != NULL;
}


BOOL bSignalNewUpdate(CCDynaNotifs* pDynaNotifs)
{
	ASSERT(pDynaNotifs);

	CNotificationUsers*	pNotifBox;

	if ((pNotifBox = GetNotifBox()) == NULL)
		return FALSE;

	return pNotifBox->bSignalNewUpdate();
}


BOOL bReportRuleFailure(CCRuleSet* pRuleSet, CCRule* pRule, UINT uErrorCode)
{
	CString		strRuleDisplay, strReport;
	CIrcPrint	ircPrint;
	UINT		uIDS = uErrorCode + IDS_ERR_FLOODING;

	ASSERT(pRule);

	strRuleDisplay.Format(IDS_ERR_RULEDISPLAY, pRule->StrGetEventDisplay(), pRule->StrGetActionDisplay(), pRuleSet->GetName());
	strReport.Format(uIDS, strRuleDisplay);

	ircPrint.SetFormat(PT_NOTINIT, NULL, RGB(255,0,0), 0, TRUE);

	AddToStatus(ircPrint, strReport);
	
	return TRUE;
}


