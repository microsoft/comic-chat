#include "stdafx.h"
#include <afxsock.h>
#include "resource.h"
#include "userinfo.h"
#include "chatprot.h"
#include "chat.h"
#include "setupdlg.h"
#include <mbstring.h>
#include "binddoc.h"
#include "chatDoc.h"
#include "histent.h"
#include "ui.h"
#include "ircproto.h"
#include "ccommon.h"
#include "format.h"
#include "setupdlg.h"
#include "actions.h"

#ifdef CB32SUPPORT
#include "icbcore.h"
#endif

#include "nmproto.h"

extern CChatApp theApp;

CIrcSocket serverConn;

extern CPtrList g_docs;
extern BOOL		g_bCanViewUnrated;			// cached during room list

BOOL bRatingsEnabled();
void IgnoreUser(const char *, const char *, BOOL, BOOL);
void ShowIdentity(const char *nick, const char *user, const char *host);


CRoomInfo *NewDefaultProto(CChatDoc *doc) {
	CIrcProto *proto = new CIrcProto;
	if (proto) {
		proto->m_pSock = &serverConn;
		proto->m_doc = doc;
	}
	return proto;
}


BOOL CommunicationInits() {
#ifdef CB32SUPPORT
	if (theApp.m_bDoCB32 && !CNmProto::Initialize()) return FALSE;
#endif CB32SUPPORT

	cui.m_pvIrcProto = NewDefaultProto(NULL);

	if (!AfxSocketInit()) {
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return SUCCEEDED(serverConn.HrInitAlloc(g_nDefaultIOBuff));
}


void CommunicationCleanup() {
#ifdef CB32SUPPORT
	if (theApp.m_bDoCB32) CNmProto::Uninitialize();
	else
#endif CB32SUPPORT

	if (GetIrcProto()) delete (GetIrcProto());
	cui.m_pvIrcProto = NULL;
}


void FixMICChannelName(CChatDoc *doc, CRoomInfo *pEnterRoom)
{
	ASSERT(pEnterRoom);

	void ChatSetChannel(const char *);
	doc->m_proto->m_strPrettyChannel = DecodeChan(doc->m_proto->m_strChannel, TRUE);
	ChatSetChannel(DecodeChan(pEnterRoom->m_strChannel, TRUE));	// successfully connected, so remember requested channel
	doc->SetLegalPath(doc->m_proto->m_strPrettyChannel);		// decoded true channel name
	doc->m_proto->SetConnectionStatus(doc->GetConnectionStatus());		// fix status bar string
}


void GetModeChars(DWORD dwFlags, char *szBuff) {
	// assumption: szBuff is large enough to hold mode string
	char *bptr = szBuff;
	if (dwFlags & CM_PRIVATE) *bptr++ = 'p';
	if (dwFlags & CM_HIDDEN) *bptr++ = 's';
	if (dwFlags & CM_INVITEONLY) *bptr++ = 'i';
	if (dwFlags & CM_TOPICHOST) *bptr++ = 't';
	if (dwFlags & CM_NOEXTERN) *bptr++ = 'n';
	if (dwFlags & CM_MODERATED) *bptr++ = 'm';
	if (dwFlags & CM_USERLIMIT) *bptr++ = 'l';
	if (dwFlags & CM_CHANNELKEY) *bptr++ = 'k';
	*bptr = '\0';
}


void ChatFillRoomList(CRoomList *prl)
{
	LPCTSTR			szQuery = prl->m_persist->m_strQuery;
	LPTSTR			szParam = NULL;
	enumCommandType	ct = ctMax;

	if (*szQuery)
	{
		// extract IRC/X command and parameter
		LPTSTR szStart = (LPTSTR) szQuery, szCommand = GetToken(szStart, &szStart, g_szEmpty);
		if (szCommand)
		{
			ASSERT(0 == stricmp(szCommand, "LIST") || 0 == stricmp(szCommand, "LISTX"));
			ct = (4 == strlen(szCommand)) ? ctList : ctListX;
			szParam = GetToken(szStart, &szStart, g_szEmpty);
		}
	}

	if (serverConn.m_bIrcXServer)
	{
		g_bCanViewUnrated = bCanViewUnrated();
		if (!*szQuery)
			ct = ctListX;
	}
	else
	{
		if (!*szQuery)
			ct = ctList;
	}

	ASSERT(ct != ctMax);
	VERIFY(GetIrcProto()->bExecuteQuery(qpRoomListDlg, ct, dtMax, NULL, szParam ? szParam : "", ""));
}


void ChatFillUserList(CUserList *pul)
{
	CString strUser, strChannel;

	if (!pul->m_persist->m_strQuery.IsEmpty())
	{
		// case where user sends /WHO <param>
		// pul->m_persist->m_strQuery is WHO <param>
		ASSERT(0 == pul->m_persist->m_strQuery.Left(3).CompareNoCase("WHO"));
		LPTSTR szUser, szNext, szStart = (LPTSTR) (LPCTSTR) pul->m_persist->m_strQuery;
		szUser = GetToken(szStart+3, &szNext, "");	// skip WHO command
		if (szUser)
			strUser = szUser;
	}
	else
	{
		if (pul->m_persist->m_searchType == USERSEARCH_ALL)
			strUser = "";
		else
			if (pul->m_persist->m_searchType == USERSEARCH_NICK || pul->m_persist->m_searchType == USERSEARCH_ID)
			{
				pul->m_user.GetWindowText(strUser);
				strUser.TrimLeft();
				pul->m_persist->m_strUserFilter = strUser;	// will be used to filter responses in AddToUserList
				if (strUser != "")
				{    // don't bother to create search args of the form **
					if (pul->m_persist->m_searchType == USERSEARCH_NICK && serverConn.m_bIrcXServer)
						strUser = CString("'*") + (EncodeNick(strUser, TRUE)+1);
					else
						strUser = "*" + strUser;
					strUser += "*";
				}
			}
			else
				if (pul->m_persist->m_searchType == USERSEARCH_ROOM)
				{
					if (!pul->m_persist->m_strEncRoom.IsEmpty())
						strChannel = pul->m_persist->m_strEncRoom;
					else
					{
						pul->m_ctlRoom.GetWindowText(strChannel);
						strChannel.TrimLeft();
						strChannel = EncodeChan(strChannel);  // strChannel has changed the room edit box -- override m_strEncRoom
					}
				}
	}

	VERIFY(GetIrcProto()->bExecuteQuery(qpUserListDlg, ctWho, dtMax, NULL, strChannel, strUser));
}


long GetMyIP() {
	SOCKADDR addr;
	SOCKADDR_IN *pAddr;
	int len = sizeof(SOCKADDR);
	if (!serverConn.GetSockName(&addr, &len)) return 0;
	pAddr = (SOCKADDR_IN *)(&addr);
	long l = ntohl(pAddr->sin_addr.S_un.S_addr);
	return (l);
}


// so we don't need to expose irc proto in protsupp.cpp
void SetVisibility(BOOL bVisible) {
	GetIrcProto()->SetVisibility(bVisible);
}


const char *EncodeNick(const char *szNick, BOOL bEscapeWildcards)
{
	static char	szEncoded[128];
	char*		szUtf8 = NULL;
	int			pCChOut, a;

	if (!(a = MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, szNick, -1, theApp.m_wszBuffer, theApp.m_nBufferSize)))
		goto error;
	if (!bConvertWideStringToUTF8(theApp.m_wszBuffer, 0, &szUtf8, &pCChOut, TRUE, FALSE, TRUE, bEscapeWildcards))
		goto error;
	if (!szUtf8)
		goto error;
	strcpy(szEncoded, szUtf8);
	delete [] szUtf8;
	return szEncoded;

error:
	ASSERT(0);
	strcpy(szEncoded, szNick);
	return szEncoded;
}


const char *DecodeNick(const char *szNick)
{
	static char	szEncoded[128];
	LPWSTR		wszBuff;
	int			pCChOut, a;

	if (*szNick != '\'')
		return szNick;
	if (!bConvertUTF8StringToWide(szNick, 0, &wszBuff, &pCChOut, TRUE, FALSE, TRUE))
		goto error;
	if (!wszBuff)
		goto error;
	if (!(a = WideCharToMultiByte(GetACP(), 0, wszBuff, -1, szEncoded, sizeof(szEncoded), NULL, NULL)))
		goto error;
	delete [] wszBuff;
	return szEncoded;

error:
	ASSERT(0);
	strcpy(szEncoded, szNick);
	return szEncoded;
}

const char *DecodeNickForScreen(const char *szNick)
{
	LPCSTR pszDecodedNick = DecodeNick (szNick);
	ASSERT(lstrlen (pszDecodedNick) < 128);
	WORD wTypeInfo[128];
	static char szBufOut[130];
	if (!GetStringTypeEx (GetUserDefaultLCID (), CT_CTYPE1, pszDecodedNick, lstrlen (pszDecodedNick), wTypeInfo))
		return pszDecodedNick;
   #if 0
	LPCSTR pszSrc = pszDecodedNick;
	LPSTR pszDest = szBufOut;
	LPWORD pwTypeInfo = wTypeInfo;
	while (*pszSrc)
	{
		if (*pwTypeInfo & (C1_CNTRL | C1_BLANK))
		{
			*pszDest = '_';
		}
		else
		{
			*pszDest = *pszSrc;
			if (IsDBCSLeadByte (*pszSrc))
				*(++pszDest) = *(++pszSrc);
		}
		pszSrc++;
		pszDest++;
		pwTypeInfo++;
	}
	*pszDest = '\0';
	return szBufOut;
   #else
   	LPCSTR pszSrc;
	LPWORD pwTypeInfo;
	for (pszSrc = pszDecodedNick, pwTypeInfo = wTypeInfo; 
		 *pszSrc; 
		 pszSrc = CharNext (pszSrc), pwTypeInfo++)
	{
		if (*pwTypeInfo & (C1_CNTRL | C1_BLANK))
		{
			wsprintf (szBufOut, "\"%s\"", pszDecodedNick);
			return szBufOut;
		}
	}
	return pszDecodedNick;
   #endif

}

#define US_CODEPAGE	1252

const char *DecodeChan(const char *szChannel, BOOL bForceDBCS) {
	int iPrefix = 0;
	char firstChar = *szChannel;

	if (!CHANNELPREFIX(firstChar))
		return szChannel;

	if (firstChar == '%') {
		LPWSTR wszBuff;
		int pCChOut, a;
		if (!bConvertUTF8StringToWide(szChannel, 0, &wszBuff, &pCChOut, FALSE, TRUE, TRUE)) goto error;
		if (!wszBuff) goto error;
		int codepage = bForceDBCS ? US_CODEPAGE : GetACP();
		if (!(a = WideCharToMultiByte(codepage, 0, wszBuff, -1, theApp.m_szBuffer, theApp.m_nBufferSize, NULL, NULL))) goto error;
		delete [] wszBuff;
		if (theApp.m_szBuffer[0] == '%' && theApp.m_szBuffer[1] != '\0') iPrefix = 2;
		szChannel = theApp.m_szBuffer;
	}
	if (firstChar == '#' || firstChar == '&' || bForceDBCS) {
		if (theApp.m_charSet == ANSI_CHARSET) return szChannel + iPrefix; // fast out
		char *szDup = strdup(szChannel);
		char *szInterChan = szDup;
		BOOL ConvertEncodingIn(LPSTR *);
		BOOL bNeedFree = ConvertEncodingIn(&szInterChan);
		strcpy(theApp.m_szBuffer, szInterChan);
		if (bNeedFree) delete [] szInterChan;
		free(szDup);
	}
	return theApp.m_szBuffer + iPrefix;

error:
//	ASSERT(0);
	strcpy(theApp.m_szBuffer, szChannel);
	return theApp.m_szBuffer;
}


const char *EncodeChan(const char *szChannel) {
	if (*szChannel == '#' || *szChannel == '&') {
		if (theApp.m_charSet == ANSI_CHARSET) return szChannel;
		char *szDup = strdup(szChannel);
		char *szInterChan = szDup;
		BOOL ConvertEncodingOut(LPSTR *);
		BOOL bNeedFree = ConvertEncodingOut(&szInterChan);
		strcpy(theApp.m_szBuffer, szInterChan);
		if (bNeedFree) delete [] szInterChan;
		free(szDup);
		return theApp.m_szBuffer;
	}
	else if (*szChannel) {
		int pCChOut, a;
		char *szUtf8 = NULL;
		if (!(a = MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, szChannel, -1, theApp.m_wszBuffer, theApp.m_nBufferSize))) goto error;

		if (!bConvertWideStringToUTF8(theApp.m_wszBuffer, 0, &szUtf8, &pCChOut, FALSE, TRUE, TRUE, FALSE)) goto error;
		if (!szUtf8) goto error;
		strcpy(theApp.m_szBuffer, szUtf8);
		delete [] szUtf8;
		return theApp.m_szBuffer;
	}

error:
	// ASSERT(0);
	strcpy(theApp.m_szBuffer, szChannel);
	return theApp.m_szBuffer;
}


const char *DecodeString(const char *szString, int iEncoding) {
	if (iEncoding == ENC_DBCS) {
		if (theApp.m_charSet == ANSI_CHARSET) return szString;
		char *szDup = strdup(szString);
		char *szInterString = szDup;
		BOOL ConvertEncodingIn(LPSTR *);
		BOOL bNeedFree = ConvertEncodingIn(&szInterString);
		strcpy(theApp.m_szBuffer, szInterString);
		if (bNeedFree) delete [] szInterString;
		free(szDup);
		return theApp.m_szBuffer;
	} else {
		LPWSTR wszBuff;
		int pCChOut, a;
		if (!bConvertUTF8StringToWide(szString, 0, &wszBuff, &pCChOut, FALSE, FALSE, FALSE)) goto error;
		if (!wszBuff) goto error;
		if (!(a = WideCharToMultiByte(GetACP(), 0, wszBuff, -1, theApp.m_szBuffer, theApp.m_nBufferSize, NULL, NULL))) goto error;
		delete [] wszBuff;
//		if (theApp.m_szBuffer[0] == '%' && theApp.m_szBuffer[1] == '#') return theApp.m_szBuffer+2;
//		else
		return theApp.m_szBuffer;
	}

error:
	ASSERT(0);
	strcpy(theApp.m_szBuffer, szString);
	return theApp.m_szBuffer;
}


short nGetBreakingPoint(int iEncodingType, const char *szBody, short nBodyLen, short nMaxLength, WORD wFormatBegin, char *szFormatBegin, WORD *pwFormatEnd)
{
	ASSERT(pwFormatEnd);
	ASSERT(szBody);

	short	nFormatBeginLen = nFillFormatting(szFormatBegin, 0, wFormatBegin, *szBody);

	nMaxLength -= nFormatBeginLen;

	*pwFormatEnd = 0;

	if (nBodyLen <= nMaxLength)
		return nBodyLen;
	else
	{
		// here comes the tough one!

		const char	*szTmp = szBody;
		const char	*szFurthestSpaceStart = NULL, *szFurthestFormattingStart = NULL;
		const char	*szValidSpaceStart = szBody + (UINT) (nMaxLength * 0.8);
		BOOL		bInSpaces = FALSE;
		WORD		wLastFullFormat;

		// szTmp can point to a regular character or the starting point of a formatting sequence 

		do
		{
			switch (*szTmp)
			{
				case chCtlColor:
				case chCtlBold:
				case chCtlItalic:
				case chCtlFixedPitchFont:
				case chCtlUnderline:
				case chCtlSymbol:
					if (!szFurthestFormattingStart)
						szFurthestFormattingStart = szTmp;
					szTmp = SzSkipOneFormat(szTmp, &wFormatBegin);
					break;

				default:
					szFurthestFormattingStart = NULL;
					wLastFullFormat = wFormatBegin;
					if (my_isspace(*szTmp))
					{
						if (!bInSpaces)
						{
							*pwFormatEnd = wFormatBegin;
							szFurthestSpaceStart = szTmp;
							bInSpaces = TRUE;
						}
					}
					else
						bInSpaces = FALSE;
					szTmp = (ENC_DBCS == iEncodingType) ? CharNext(szTmp) : SzNextUTF8Char(szTmp);
			}
		}
		while (szTmp < szBody + nMaxLength - 2);

		if (szFurthestSpaceStart && szFurthestSpaceStart >= szValidSpaceStart)
			// we found a space character close enough to the end and will break there - last formatting is already set
			return szFurthestSpaceStart - szBody;
		else
		{
			*pwFormatEnd = wLastFullFormat;
			// no space at all in the big string
			if (szFurthestFormattingStart)
				// we are in the middle of a formatting sequence
				return szFurthestFormattingStart - szBody - 1;	// we don't want to include the last formatting part
			else
				// we cut wherever we stopped in the middle of the string because there is no space or formatting
				return szTmp - szBody;
		}
	}
}


void CIrcProto::SendMessageText(char *szMesg) {
	TRACE("Sending message: %s\n", szMesg);
	m_pSock->Send(szMesg, strlen(szMesg));
}


BOOL CIrcProto::bChatSendToTarget(const char *szAddressee, const char *szAnnotations, const char *szMesg, USHORT uModes /*=0*/, BOOL bAsNotice /*=FALSE*/)
{
	BOOL		bFreeTmp = FALSE;
	LPTSTR		szTmp = NULL;
	SHORT		nAnnotationsLen, nMesgLen, nTargetLen, nReceivingPrefixLen, nLen = 12; // 12 for IRC command PRIVMSG
	int			iEncodingType = 0;
	const char*	szTarget = szAddressee ? szAddressee : (const char*) m_strChannel;

	// szAddressee is NULL for channel messages

	if (szMesg)
	{
		iEncodingType = szAddressee ? ENC_DBCS : EncodingType();
		szMesg = EncodeString(szMesg, iEncodingType);

		// Might have to quote \r and \n
		bLowLevelQuoting(g_chLLQuoteCTCP, TRUE /*bTreatAsByteArray*/, szMesg, &szTmp, &bFreeTmp);
		szMesg = szTmp;
	}

	if (szAnnotations)
	{
		nAnnotationsLen = strlen(szAnnotations);
	}
	else
	{
		szAnnotations = "";
		nAnnotationsLen = 0;
	}

	if (szMesg)
	{
		nMesgLen = strlen(szMesg);
	}
	else
	{
		szMesg = "";
		nMesgLen = 0;
	}

	nTargetLen = strlen(szTarget);

	// on the receiving side, the prefix ":<nickname>!<username>@<hostname> " gets added, so we don't want the server
	// to cut the message to get room for this prefix, therefore:

	// 2 is for starting : and trailing space
	nReceivingPrefixLen = 2 + strlen(GetMyNickName());
	
	// for private messages: when hostname length is still unknown we use 32 by default
	if (theApp.m_nMyIdentLength)
		nReceivingPrefixLen += theApp.m_nMyIdentLength;
	else
		nReceivingPrefixLen += strlen(GetMyUserName()) + 32;

	nLen += nTargetLen + nAnnotationsLen + nMesgLen + nReceivingPrefixLen;	// final length of message on the receiving side

	if (nLen <= serverConn.m_nMaxMsgLength)
	{
		// message is short enough to be sent in one shot
		if (*szAnnotations && IsIRCX())
		{
			sprintf(serverConn.m_szOutput2, "DATA %s %s :%s\r\n", szTarget, CCUDI1, szAnnotations);
			SendMessageText(serverConn.m_szOutput2);

			if (*szMesg)
			{
				sprintf(serverConn.m_szOutput2, "%s %s :%s\r\n", (bAsNotice ? "NOTICE" : "PRIVMSG"),
																 szTarget, szMesg);
				SendMessageText(serverConn.m_szOutput2);
			}
		}
		else
		{
			sprintf(serverConn.m_szOutput2, "%s %s :%s%s\r\n", (bAsNotice ? "NOTICE" : "PRIVMSG"),
																szTarget, szAnnotations, szMesg);
			SendMessageText(serverConn.m_szOutput2);
		}
	}
	else
	{
		ASSERT(nMesgLen > 0);
		ASSERT(iEncodingType > 0);

		char		szPrefix[16];
		const char	*szBody;
		short		nBodyLen, nPrefixLen = 0, nSuffixLen = 1;	// 1 is for the terminating 0x01 by default
		short		nBreakingPoint;
		char		chBreakingChar, chSuffixChar;
		char		szFormatBegin[11];				// max length would be for ^kWX,YZ^b^u^f for example
		WORD		wFormatBegin = 0, wFormatEnd;
		BOOL		bOnlySendOneChunk;

		bOnlySendOneChunk = (iEncodingType == ENC_DBCS) && (GetACP() == 932);

		// multiple chunks case
		switch (uModes)
		{
		case BM_ACTION:
			// szMesg = 0x01ACTION <data>0x01

			// REGISB: could include verb thinks into prefix for think button.
			nPrefixLen = g_nActionLen + 1;
			break;
		case BM_SOUND:
			// szMesg = 0x01SOUND <filename> <data>0x01
			nPrefixLen = g_nSoundLen + 1;
			break;
		case BM_AWAY:
			// szMesg = 0x01AWAY <data>0x01
			nPrefixLen = g_nAwayLen + 1;
			break;
		case BM_HERESINFO:
			// szMesg = # HeresInfo: <data>
			nPrefixLen = g_nHeresInfoLen + 1;	// +1 for # sign
			nSuffixLen = 0;						// no terminating 0x01
			break;
		case BM_SAY:
		case BM_THINK:
		case BM_WHISPER:
			// szMesg = <data>   ==> nPrefixLen = 0
			nSuffixLen = 0;		// no terminating 0x01
			break;
		default:
			ASSERT(0);
		}

		szBody = szMesg + nPrefixLen;
		nBodyLen = nMesgLen - nPrefixLen;

		ASSERT(szBody);
		ASSERT(nBodyLen == (short) strlen(szBody));

		// first prepare prefix
		if (nPrefixLen)
			strncpy(szPrefix, szMesg, nPrefixLen);
		szPrefix[nPrefixLen] = '\0';

		do
		{
			// send another chunk and update szBody
			
			// bytes allowed in szBody term = serverConn.m_nMaxMsgLength - 12 - nChannelLen - nAnnotationsLen - nPrefixLen - nSuffixLen
			// 12 = "PRIVMSG  :\r\n"
			nBreakingPoint = nGetBreakingPoint(iEncodingType, szBody, nBodyLen, serverConn.m_nMaxMsgLength - nReceivingPrefixLen - 12 - nTargetLen - nAnnotationsLen - nPrefixLen - nSuffixLen, wFormatBegin, szFormatBegin, &wFormatEnd);

			chBreakingChar = szBody[nBreakingPoint+nSuffixLen];
			((char*)szBody)[nBreakingPoint+nSuffixLen] = '\0';

			if (nSuffixLen)
			{
				chSuffixChar = szBody[nBreakingPoint];
				((char*)szBody)[nBreakingPoint] = '\001';
			}

			if (*szAnnotations && IsIRCX())
			{
				sprintf(serverConn.m_szOutput2, "DATA %s %s :%s\r\n", szTarget, CCUDI1, szAnnotations);
				SendMessageText(serverConn.m_szOutput2);

				sprintf(serverConn.m_szOutput2, "%s %s :%s%s%s\r\n", 
						bAsNotice ? "NOTICE" : "PRIVMSG",
						szTarget, 
						szPrefix, 
						szFormatBegin, 
						szBody);
			}
			else
				sprintf(serverConn.m_szOutput2, "%s %s :%s%s%s%s\r\n", 
						bAsNotice ? "NOTICE" : "PRIVMSG",
						szTarget, 
						szAnnotations, 
						szPrefix, 
						szFormatBegin, 
						szBody);

			#ifdef DEBUG
				short nOutputLen = strlen(serverConn.m_szOutput2);
				ASSERT(nOutputLen <= serverConn.m_nMaxMsgLength);
			#endif // DEBUG

			SendMessageText(serverConn.m_szOutput2);

			((char*)szBody)[nBreakingPoint+nSuffixLen] = chBreakingChar;

			if (nSuffixLen)
				((char*)szBody)[nBreakingPoint] = chSuffixChar;

			// goto the beginning of the next chunk
			szBody += nBreakingPoint;
			nBodyLen -= nBreakingPoint;

			while (my_isspace(*szBody))	// we skip all spaces and tabs...
			{
				szBody++;				// all space type chars are single byte characters
				nBodyLen--;
			}

			wFormatBegin = wFormatEnd;	// assure formatting continuation

			ASSERT(nBodyLen == (short) strlen(szBody));

			if (BM_SOUND == uModes)
			{
				uModes = BM_ACTION;
				nPrefixLen = g_nActionLen + 1;
				strncpy(szPrefix, actionID, g_nActionLen);
				szPrefix[g_nActionLen] = ' ';
				szPrefix[g_nActionLen+1] = '\0';
			}
		}
		while (szBody && *szBody && !bOnlySendOneChunk);
	}

	if (bFreeTmp)
		delete [] szTmp;

	return TRUE;
}


BOOL CIrcProto::bChatSendPrivMesg(const char *szAddressee, const char *szAnnotations, const char *szMesg, char *szNMText, BOOL bAsNotice, USHORT uModes)
{
	return bChatSendToTarget(szAddressee, szAnnotations, szMesg, uModes, bAsNotice);
}


BOOL CIrcProto::bChatSendToChannel(const char *szAnnotations, const char *szMesg, char *szNMText /*=NULL*/, USHORT uModes /*=0*/)
{
	return bChatSendToTarget(NULL, szAnnotations, szMesg, uModes);
}


BOOL CIrcProto::ChatSetTopic(const char *szTopic)
{
	if (*szTopic)
		szTopic = EncodeString(szTopic);
	return bExecuteQuery(qpSetTopic, ctTopic, dtMax, (PVOID) szTopic, m_strChannel, "");
}


BOOL CIrcProto::ChatSetClientData(const char *szClientData) {
	if (IsIRCX ()) {
		ASSERT(szClientData);
		if (*szClientData) szClientData = EncodeString(szClientData);	// REGIS: why converting string??
		// sprintf(GetOutBuff(), "PROP %s CLIENT :%s\r\n", m_strChannel, szClientData);
		// SendMessageText(GetOutBuff());
		// return TRUE;

		// REGISB 04/03/98 need to Q the PROP setting because of the Status Window display
		return bExecuteQuery(qpSetClient, ctPropSet, dtMax, (PVOID) szClientData, m_strChannel, "");
	}
	else {
		return FALSE;
	}
}

void			
CIrcProto::HandleClientDataChange(
LPCSTR pszNewClientData)
{
	// We need to find out all properties that have been added, modified, or
	// removed. The best way to do this is to enumerate both the old and new
	// clientdata strings. For properties that are in the new string, and were 
	// different or didn't exist in the old one, we need to call OnPropertyChange.
	// For properties that were in the old string, and are not in the new one,
	// we need to call OnPropertyChange as well.

	LPCSTR pszPropStrings[2] = { pszNewClientData, m_strClientData };
	CString strKey;
	CString strValue, strOtherValue;
	LPCSTR psz;
	for (int iPass = 0; iPass < 2; iPass++) {
		psz = pszPropStrings[iPass];
		while (EnumKeyString (psz, strKey, strValue)) {
			if (!GetValueFromKeyString (pszPropStrings[1 - iPass], strKey, strOtherValue)) {
				// On first pass, this means the property has been added.
				// On second pass, this means the property has been removed.
				OnPropertyChange (strKey, (iPass == 1) ? (LPCSTR)NULL : (LPCSTR)strValue);
			} else if (strOtherValue != strValue && iPass == 0) {
				// The property has been modified. Only call once, on first pass.
				OnPropertyChange (strKey, strValue);
			}
		}
	}

	m_strClientData = pszNewClientData;
}

void CIrcProto::ChatPartChannel(CDocument *doc1, BOOL) {
	CChatDoc *doc = (CChatDoc*) doc1;
	if (m_bInRoom) {
		sprintf(GetOutBuff(), "PART %s\r\n", m_strChannel);  // exit gracefully
		SendMessageText(GetOutBuff());
	}

	if (GetConnectionStatus() == CX_INCHANNEL) {
		SetConnectionStatus(CX_NOCHANNEL);
		ASSERT(doc);
		ASSERT(doc->m_proto);
		if (!m_strChannel.IsEmpty() && !doc->m_bStatusView)
			theApp.m_dynaRules.bMatchAndApplyRules(eOnLeave, NULL, NULL, CString(GetMyServer()), theApp.m_myNick+"!"+theApp.m_myIdent, m_strChannel, CString(""));
	}
}


void CIrcProto::ChatJoinChannel(CRoomInfo &enterInfo)
{
	if (serverConn.m_bIrcXServer && bRatingsEnabled())
		VERIFY(bExecuteQuery(qpJoinPics, ctPropGet, dtMax, NULL, enterInfo.m_strChannel, ""));
	else
		if (g_bCanViewUnrated = bCanViewUnrated(TRUE))	// give 'em a chance to reset this value
			ChatJoinAux(enterInfo);
}


void CIrcProto::ChatCreateChannel(CRoomInfo &enterInfo)
{
	if (serverConn.m_bIrcXServer && bRatingsEnabled())
		VERIFY(bExecuteQuery(qpCreatePics, ctPropGet, dtMax, NULL, enterInfo.m_strChannel, ""));
	else
		if (g_bCanViewUnrated = bCanViewUnrated(TRUE))	// give 'em a chance to reset this value
			ChatCreateAux(enterInfo);
}


void CIrcProto::ChatJoinAux(CRoomInfo &enterInfo)
{
	ASSERT(!enterInfo.m_strChannel.IsEmpty());
	if (enterInfo.m_strPassword.IsEmpty())
		sprintf(GetOutBuff(), "JOIN %s\r\n", enterInfo.m_strChannel);
	else
	{
		int iEncoding = (enterInfo.m_strChannel[0] == '#' || enterInfo.m_strChannel[0] == '&') ? ENC_DBCS : ENC_UTF8;
		CString strPwd = EncodeString(enterInfo.m_strPassword, iEncoding);
		sprintf(GetOutBuff(), "JOIN %s %s\r\n", enterInfo.m_strChannel, strPwd);
	}

	SendMessageText(GetOutBuff());
}


void CIrcProto::ChatCreateAux(CRoomInfo &enterInfo)
{
	ASSERT(!enterInfo.m_strChannel.IsEmpty());

	CString strParams;

	if (!enterInfo.m_strCreationModes.IsEmpty())
		strParams += " " + enterInfo.m_strCreationModes;

	if (enterInfo.m_dwMaxUsers)
	{
		char szMaxUsers[16];
		sprintf(szMaxUsers, "%d", enterInfo.m_dwMaxUsers);
		strParams += " " + CString(szMaxUsers);
	}

	if (!enterInfo.m_strPassword.IsEmpty())
	{
		int iEncoding = (enterInfo.m_strChannel[0] == '#' || enterInfo.m_strChannel[0] == '&') ? ENC_DBCS : ENC_UTF8;
		CString strPwd = EncodeString(enterInfo.m_strPassword, iEncoding);
		strParams += " " + strPwd;
	}

	sprintf(GetOutBuff(), "CREATE %s%s\r\n", enterInfo.m_strChannel, strParams);
	SendMessageText(GetOutBuff());
}


BOOL CIrcProto::ChatKickUser(const char *szNickname, const char *szReason)
{
	if (szReason && *szReason)
		szReason = EncodeString(szReason);
	sprintf (GetOutBuff(), "KICK %s %s :%s\r\n", m_strChannel, szNickname, szReason ? szReason : "");
	SendMessageText(GetOutBuff());
	return TRUE;
}


BOOL CIrcProto::ChatBanUser(const char *szBanPattern, BOOL bBan, const char *szEncodedChannel /* = NULL */)
{
	const char *szFlag = bBan ? "+b" : "-b";

	LPCSTR szNickEnd = OurMbsChr(szBanPattern, '!');
	LPTSTR szNickname;

	if (szNickEnd)
	{
		LPCSTR szNickBegin = szBanPattern;

		while (*szNickBegin == '?' || *szNickBegin == '*')
			szNickBegin++;
		if (!(szNickname = new TCHAR[szNickEnd-szNickBegin+1]))
			return FALSE;
		strncpy(szNickname, szNickBegin, szNickEnd-szNickBegin);
		szNickname[szNickEnd-szNickBegin] = '\0';
	}
	else
		szNickname = (LPTSTR) szBanPattern;

	if (IsIRCX() && bExtendedNickname(szNickname))
		szBanPattern = EncodeNick(szBanPattern);
	sprintf(GetOutBuff(), "MODE %s %s %s\r\n", szEncodedChannel ? szEncodedChannel : m_strChannel, szFlag, szBanPattern);
	SendMessageText(GetOutBuff());

	if (szNickname != szBanPattern)
		delete [] szNickname;

	return TRUE;
}


BOOL CIrcProto::ChatSendInvitation(const char *szNickname)
{
	sprintf(GetOutBuff(), "INVITE %s %s\r\n", szNickname, (LPCTSTR) m_strChannel);
	SendMessageText(GetOutBuff());
	return TRUE;
}


BOOL CIrcProto::ChatChangeNick(const char *szNewNick)
{
	if (IsIRCX() && bExtendedNickname(szNewNick))
		szNewNick = EncodeNick(szNewNick);

	sprintf(GetOutBuff(), "NICK %s\r\n", szNewNick);
	SendMessageText(GetOutBuff());
	return TRUE;
}


void CIrcProto::ChatSetAway(BOOL bAway, const char *szMesg, CUserInfo *pui, BOOL bProtoNotify)
{
	// szMesg is a control full string

	if (!bProtoNotify)
	{  // simple notification case
		CRoomInfo::ChatSetAway(bAway, szMesg, pui, FALSE);
		return;
	}

	// otherwise, need to do whole thing
	if (bAway)
		sprintf(GetOutBuff(), "AWAY :%s\r\n", szMesg);		// !REGISB! 10/14/97 don't we need to encode the szMesg??
	else
		sprintf(GetOutBuff(), "AWAY\r\n");

	SendMessageText(GetOutBuff());

	// now loop and do this for all docs...
	POSITION pos = g_docs.GetHeadPosition();
	while (pos)
	{
		CChatDoc *doc = (CChatDoc*) g_docs.GetNext(pos);
		if (doc->m_proto->GetConnectionStatus() == CX_INCHANNEL)
			doc->m_proto->CRoomInfo::ChatSetAway(bAway, szMesg, NULL, FALSE);
	} 
}


BOOL CIrcProto::ChatSetMode(DWORD newMode, DWORD newMaxUsers, const char *szNewPasswd) {
	char szMaxUsers[7] = "";
	const char *szKey;
	DWORD newSets = newMode & ~dwCurrentChannelMode;
	DWORD newUnSets = dwCurrentChannelMode & ~newMode;

	if ((newMode & CM_USERLIMIT) && newMaxUsers != dwCurrentUserLimit) {
		newSets |= CM_USERLIMIT;
		sprintf(szMaxUsers, "%d", newMaxUsers);
	} else newSets &= ~CM_USERLIMIT;

	if (szNewPasswd && *szNewPasswd)
		szNewPasswd = EncodeString(szNewPasswd);

	if ((newMode & CM_CHANNELKEY) && strcmp(szNewPasswd, strCurrentChannelKey)) {
		newSets |= CM_CHANNELKEY;
		if (!strCurrentChannelKey.IsEmpty())
			newUnSets |= CM_CHANNELKEY;   // necessary to unset before we can set!
	} else newSets &= ~CM_CHANNELKEY;

	char modeBuff[20];
	GetModeChars(newUnSets, modeBuff);
	if (*modeBuff) {
		if (newUnSets & CM_CHANNELKEY) szKey = strCurrentChannelKey;
		else szKey = "";
		sprintf(GetOutBuff(), "MODE %s -%s %s\r\n", m_strChannel, modeBuff, szKey);
		SendMessageText(GetOutBuff());
	}
	GetModeChars(newSets, modeBuff);
	if (*modeBuff) {
		if (!(newSets & CM_CHANNELKEY)) szKey = "";
		else szKey = szNewPasswd;
		sprintf(GetOutBuff(), "MODE %s +%s %s %s\r\n", m_strChannel, modeBuff, szMaxUsers, szKey);
		SendMessageText(GetOutBuff());
	} 
	return TRUE;
}


void CIrcProto::ChatGetIdentity(CUserInfo *pui, LPCTSTR szNickname)
{
	if (!pui || pui->GetFullName().IsEmpty())
	{
		LPCTSTR szNick = pui ? ((LPCTSTR) pui->GetName()) : szNickname;
		ASSERT(szNick);
		VERIFY(bExecuteQuery(qpGetIdent, ctWhoIs, dtMax, NULL, "", szNick));
	}
	else
	{
		CString	strUser, strHost, strIdent = pui->GetFullName();
		int		iIndex = strIdent.Find('@');
		if (iIndex > -1)
		{
			strUser = strIdent.Left(iIndex);
			strHost = strIdent.Mid(iIndex+1);
		}
		else
			strUser = strIdent;
		ShowIdentity(pui->GetName (), strUser, strHost);
	}
}


void CIrcProto::ChatKickUser(CUserInfo *pui)
{
	VERIFY(bExecuteQuery(qpKickDlg, ctWhoIs, dtMax, NULL, m_strChannel, pui->GetName()));
}


void CIrcProto::ChatBanUser(CUserInfo *pui)
{
	if (pui)
		VERIFY(bExecuteQuery(qpBanDlg, ctWhoIs, dtMax, NULL, m_strChannel, pui->GetName()));
	else
	{
		sprintf(GetOutBuff(), "MODE %s +b\r\n", m_strChannel);
		SendMessageText(GetOutBuff());
	}
}


BOOL CIrcProto::bRegisterMode(char* szMesg)
{
	char	*szTmp;
	char	*szTarget = GetToken(szMesg, &szTmp, " ");

	if (CHANNELPREFIX(*szTarget))
		return bExecuteQuery(qpComSetChannelMode, ctSetChannelMode, dtMax, (PVOID) szTmp, szTarget, "");
	else
		return bExecuteQuery(qpComSetUserMode, ctSetUserMode, dtMax, (PVOID) szTmp, "", szTarget);
}


BOOL CIrcProto::bExecuteQuery(enumQueryPurpose qp, 
							  enumCommandType ct,
							  enumDataType dt,
							  PVOID pvData,
							  CString strChannelName,
							  CString strNicknameMask)
{
	PPRUSERMATCH	pPrUserMatch;
	LPTSTR			szFilter = NULL;
	UINT			cbFilter = 0;
	CCQuery*		pQuery;

	// Make sure we are connected...
	if (GetConnectionStatus() == CX_DISCONNECTED)
		return FALSE;

	pQuery = new CCQuery(qp, ct, dt, pvData, strChannelName, strNicknameMask, !strNicknameMask.IsEmpty() && ctWho == ct /*bCreatePrUserMatch*/);
	if (!pQuery)
	{
		ASSERT(FALSE);
		return FALSE;
	}

	if (!m_pSock->m_queries.bAddQuery(pQuery))
	{
		ASSERT(FALSE);
		return FALSE;
	}

	switch (ct)
	{
	case ctWho:
		if (pPrUserMatch = pQuery->GetPrUserMatch())
		{
			if (pPrUserMatch->cbNickname)
			{
				szFilter = pPrUserMatch->szNickname;
				cbFilter = pPrUserMatch->cbNickname;
			}
			if (pPrUserMatch->cbUserName > cbFilter)
			{
				szFilter = pPrUserMatch->szUserName;
				cbFilter = pPrUserMatch->cbUserName;
			}
			if (pPrUserMatch->cbIPAddress > cbFilter)
			{
				szFilter = pPrUserMatch->szIPAddress;
				cbFilter = pPrUserMatch->cbIPAddress;
			}
		}
		if (szFilter)
		{
			LPTSTR szFilterTmp = strdup(szFilter);
			szFilterTmp[cbFilter] = g_chEOS;
			sprintf(GetOutBuff(), "WHO %s\r\n", szFilterTmp);
			free(szFilterTmp);
		}
		else
			if (strChannelName.IsEmpty())
				strcpy(GetOutBuff(), "WHO\r\n");
			else
				sprintf(GetOutBuff(), "WHO %s\r\n", strChannelName);
		break;

	case ctWhoIs:
		ASSERT(!strNicknameMask.IsEmpty());
		sprintf(GetOutBuff(), "WHOIS %s\r\n", (LPCTSTR) strNicknameMask);
		break;

	case ctTopic:
		switch (qp)
		{
		case qpSetTopic:
			sprintf(GetOutBuff(), "TOPIC %s :%s\r\n", strChannelName, (char*) pvData);
			break;
		case qpListMembers:
			sprintf(GetOutBuff(), "TOPIC %s\r\n", strChannelName);
			break;
		default:
			ASSERT(FALSE);
		}
		break;

	case ctList:
		if (strChannelName.IsEmpty())
			strcpy(GetOutBuff(), "LIST\r\n");
		else
			sprintf(GetOutBuff(), "LIST %s\r\n", (LPCTSTR) strChannelName);
		break;

	case ctListX:
		if (strChannelName.IsEmpty())
			strcpy(GetOutBuff(), "LISTX\r\n");
		else
			sprintf(GetOutBuff(), "LISTX N=%s\r\n", (LPCTSTR) strChannelName);
		break;

	case ctLUsersMOTD:
		strcpy(GetOutBuff(), "LUSERS\r\nMOTD\r\n");
		break;

	case ctIrcX:
		strcpy(GetOutBuff(), "IRCX\r\n");
		break;

	case ctModeIsIrcX:
		strcpy(GetOutBuff(), "MODE ISIRCX\r\n");
		break;

	case ctGetChannelMode:
		sprintf(GetOutBuff(), "MODE %s\r\n", (LPCTSTR) strChannelName);
		break;

	case ctSetChannelMode:
		sprintf(GetOutBuff(), "MODE %s%s\r\n", (LPCTSTR) strChannelName, (char*) pvData);
		break;

	case ctSetUserMode:
	{
		const char* szModes;

		switch (qp)
		{
		case qpSetVisible:
			ASSERT(!strNicknameMask.IsEmpty());
			szModes = "-i";
			break;
		case qpSetInvisible:
			ASSERT(!strNicknameMask.IsEmpty());
			szModes = "+i";
			break;
		case qpComSetUserMode:
			szModes = (const char*) pvData;
			break;
		default:
			ASSERT(FALSE);
		}

		sprintf(GetOutBuff(), "MODE %s %s\r\n", strNicknameMask, szModes);
		break;
	}

	case ctPropGet:
	{
		LPCTSTR szPropName;
		switch (qp)
		{
		case qpJoinPics:
		case qpCreatePics:
			szPropName = "PICS";
			break;
		case qpJoinBackUrl:
			szPropName = "CLIENT";
			break;
		default:
			ASSERT(FALSE);
		}
		ASSERT(!strChannelName.IsEmpty());
		sprintf(GetOutBuff(), "PROP %s %s\r\n", (LPCTSTR) strChannelName, szPropName);
		break;
	}

	case ctPropSet:
	{
		LPCTSTR szPropName;
		switch (qp)
		{
		case qpSetClient:
			szPropName = "CLIENT";
			break;
		default:
			ASSERT(FALSE);
		}
		ASSERT(!strChannelName.IsEmpty());
		sprintf(GetOutBuff(), "PROP %s %s :%s\r\n", (LPCTSTR) strChannelName, szPropName, (char*) pvData);
		break;
	}

	default:
		ASSERT(FALSE);
		return FALSE;
	}
	SendMessageText(GetOutBuff());

	return TRUE;
}


BOOL CIrcProto::bChatShowMOTD()
{
	return bExecuteQuery(qpLUsersMOTD, ctLUsersMOTD, dtMax, NULL, "", "");
}


void CIrcProto::DoIgnoreUser(CUserInfo *pui, BOOL bIgnore, BOOL bAutoIgnore, LPCTSTR szNickname)
{
	if (pui && (pui->Ignored() == bIgnore))
		return;   // no change!

	ASSERT(szNickname || pui);

	if (!pui || pui->GetFullName().IsEmpty())
	{
		CString strNick = pui ? pui->GetName() : CString(szNickname);
		WORD	wFlags = 0L;

		if (bIgnore)
			wFlags = g_wIgnoreIdent;
		if (bAutoIgnore)
			wFlags += g_wAutoIgnoreIdent;

		VERIFY(bExecuteQuery(qpIgnoreIdent, ctWhoIs, dtFlags, (PVOID) wFlags, "", strNick));
	}
	else
	{
		ASSERT(pui);
		IgnoreUser(pui->GetName(), pui->GetFullName(), bIgnore, bAutoIgnore);
	}
}


void CIrcProto::SetVisibility(BOOL bVisible) 
{
	VERIFY(bExecuteQuery(bVisible ? qpSetVisible : qpSetInvisible, ctSetUserMode, dtMax, NULL, "", GetMyNickName()));
}


inline int CIrcProto::EncodingType()
{
	const char *szChannel = (LPCTSTR) m_strChannel;
	return (szChannel[0] == '%' && !(m_dwModes & CM_MIC)) ? ENC_UTF8 : ENC_DBCS;
}


const char *CIrcProto::EncodeString(const char *szString, int iEncoding) {
	if (iEncoding == ENC_CHANNEL)
		iEncoding = EncodingType();

	if (iEncoding == ENC_DBCS) {
		if (theApp.m_charSet == ANSI_CHARSET) return szString;
		char *szDup = strdup(szString);
		char *szInterString = szDup;
		BOOL ConvertEncodingOut(LPSTR *);
		BOOL bNeedFree = ConvertEncodingOut(&szInterString);
		strcpy(theApp.m_szBuffer, szInterString);
		if (bNeedFree) delete [] szInterString;
		free(szDup);
		return theApp.m_szBuffer;
	} else {
		int pCChOut, a;
		char *szUtf8 = NULL;
		if (!(a = MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, szString, -1, theApp.m_wszBuffer, theApp.m_nBufferSize))) goto error;

		if (!bConvertWideStringToUTF8(theApp.m_wszBuffer, 0, &szUtf8, &pCChOut, FALSE, FALSE, FALSE, FALSE)) goto error;
		if (!szUtf8) goto error;
		ASSERT(strlen(szUtf8) < theApp.m_nBufferSize);
		strcpy(theApp.m_szBuffer, szUtf8);
		delete [] szUtf8;
		return theApp.m_szBuffer;
	}

error:
	ASSERT(0);
	strcpy(theApp.m_szBuffer, szString);
	return theApp.m_szBuffer;
}


CString CIrcProto::StrEncodeCommandParam(DWORD dwAt, INT *piEncoding, CHAR *szParam)
{
	ASSERT(szParam);
	ASSERT(piEncoding);

	DWORD		dwAtTmp = dwAt & (AT_CHANNEL|AT_NICKNAME|AT_NICKMASK|AT_REASON|AT_PASSWORD|AT_TOPIC|AT_MESSAGE|AT_PROPVALUE);
	const char*	szEncodedChannelName = NULL;

	if (!dwAtTmp)
		return szParam;

	if ((dwAtTmp & AT_CHANNEL) && (dwAtTmp & AT_NICKNAME))
	{
		if (szParam[0] == '%' || szParam[0] == '#' || szParam[0] == '&')
			// definitely a channel name
			dwAtTmp &= ~AT_NICKNAME;
		else
		{
			// don't know yet if it's a channel name or nickname
			szEncodedChannelName = EncodeChan(szParam);
			CChatDoc* pDoc = LookupDoc(szEncodedChannelName);
			if (pDoc)
				// it must be a channel since we are in a channel with that name
				dwAtTmp &= ~AT_NICKNAME;
			else
			{
				// let consider this is a nickname and not a channel name - we don't check if this nick exists on purpose
				dwAtTmp &= ~AT_CHANNEL;
				szEncodedChannelName = NULL;
			}
		}
	}

	if (szEncodedChannelName)
	{
		// we already have our encoded answer
		ASSERT(dwAtTmp == AT_CHANNEL);
		*piEncoding = ENC_UTF8;
		return szEncodedChannelName;
	}

	switch (dwAtTmp)
	{
	case AT_CHANNEL:
		*piEncoding = (szParam[0] == '&' || szParam[0] == '#') ? ENC_DBCS : ENC_UTF8;
		return EncodeChan(szParam);

	case AT_NICKNAME|AT_NICKMASK:
	case AT_NICKNAME:
	case AT_NICKMASK:
	{
		LPSTR	szNickPortion = szParam;
		CString strNick = szParam;
		TrimQuotes(strNick);

		if (dwAtTmp & AT_NICKMASK)
		{
			LPCSTR szNickEnd = OurMbsChr(szParam, '!');

			if (szNickEnd)
			{
				LPCSTR szNickBegin = szParam;

				while (*szNickBegin == '?' || *szNickBegin == '*')
					szNickBegin++;
				if (!(szNickPortion = new CHAR[szNickEnd-szNickBegin+1]))
					return strNick;
				strncpy(szNickPortion, szNickBegin, szNickEnd-szNickBegin);
				szNickPortion[szNickEnd-szNickBegin] = '\0';
			}
		}

		if (IsIRCX() && bExtendedNickname(szNickPortion))
			strNick = EncodeNick(strNick);

		if (szNickPortion != szParam)
			delete [] szNickPortion;

		return strNick;
	}
	default:
		// AT_REASON | AT_TOPIC | AT_PASSWORD | AT_MESSAGE | AT_PROPVALUE
		return EncodeString(szParam, *piEncoding);
	}
}


BOOL CIrcProto::ChangeProperty(
CUserInfo *puiSelf, 
LPCSTR pszProperty, 
LPCSTR pszValue)
{
	// IRC implementation is limited to owners of IRCX rooms.
	if (!IsIRCX () || !puiSelf || !puiSelf->IsOwner ()) {
		return FALSE;
	}

	if (!ChangeKeyString (m_strClientData, pszProperty, pszValue, 255)) {
		return FALSE;
	}

	return ChatSetClientData (m_strClientData);
}

class CIdentdSocket : public CAsyncSocket {
public:
	virtual void OnAccept(int nErrorCode);
//	virtual void OnConnect(int nErrorCode) { ASSERT(0);TRACE("GOT HERE!!!!\n"); }
//	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode) { TRACE("IDENTD Out of Band socket on error %d.\n", nErrorCode); }
};

static CIdentdSocket g_identd, g_ident0;

void StartIdentD() {
	#if 0
	VERIFY(g_identd.Create(113));
	int rval = g_identd.Listen(1);
	ASSERT(rval || GetLastError() == WSAEWOULDBLOCK);
	#endif
	// More timid version, does not assert in debug builds, and is better
	// at error checking.
	if (g_identd.Create (113))
	{
		int rval = g_identd.Listen (1);
		if (rval == 0 && GetLastError () != WSAEWOULDBLOCK)
		{
			g_identd.Close ();
		}
	}
}

void StopIdentD() {
	if (g_identd.m_hSocket != INVALID_SOCKET) 	// close last connection if necessary
		g_identd.Close();
	if (g_ident0.m_hSocket != INVALID_SOCKET) 	// close last connection if necessary
		g_ident0.Close();
}

void CIdentdSocket::OnAccept(int nErrorCode) {
	int rval = Accept(g_ident0);
	ASSERT(rval || GetLastError() == WSAEWOULDBLOCK);
}

void CIdentdSocket::OnReceive(int nErrorCode) {
	char buff[200];
	if (!nErrorCode) {
		int nRead = Receive(buff, sizeof(buff)-1);
		buff[nRead] = '\0';
		char *eoc = strchr(buff, '\r');
		if (eoc) *eoc = '\0';
		sprintf(GetOutBuff(), "%s : USERID : UNIX : %s\r\n", buff, GetMyUserName());
		Send(GetOutBuff(), strlen(GetOutBuff()));
	}
}
