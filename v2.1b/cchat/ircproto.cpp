#include "stdafx.h"
#include <afxsock.h>
#include "resource.h"
#include "userinfo.h"
#include "chatprot.h"
#include "chat.h"
#include "setupdlg.h"
#include "roomlist.h"
#include "userlist.h"
#include <mbstring.h>
#include "binddoc.h"
#include "chatDoc.h"
#include "histent.h"
#include "ui.h"
#include "IrcProto.H"
#include "CCommon.H"
#include "Format.H"

#include "icbcore.h"

#include "nmproto.h"

extern int g_iQuery;
extern CChatApp theApp;

CIrcSocket serverConn;

static CString strMOTD;
static CString strLUSER;
static CString g_strBan;
static CStringArray g_arrayBans;

extern CPtrList g_docs;
extern BOOL g_bCanViewUnrated;			// cached during room list

extern BOOL bInitEnterInfo(const char *roomName, const char *password = NULL, BOOL bEncode = FALSE);
extern CChatDoc *LookupDoc(const char *);
extern CUserInfo *LookupPui(const char *nickname, CChatDoc *doc = NULL);
void ChatChangeAdmin(CChatDoc *doc, const char *nick, int sets, int unsets);
void AddToRoomList(CRoom *, BOOL = TRUE);
extern BOOL bCanViewUnrated(BOOL = FALSE);
extern BOOL bPassesRatings(const char *szRating, BOOL = FALSE);
BOOL bRatingsEnabled();
char* SzControlLess(char *, CDWordArray * = NULL);
const char *DecodeNick(const char *), *DecodeChan(const char *, BOOL = FALSE), *EncodeNick(const char *, BOOL = FALSE);
const char *DecodeString(const char *szString, int iEncoding);
void OnTextMsg(CChatDoc *, char *nick, const char *id, char *mesg, BYTE msgType = MT_CHANNELSEND, CDWordArray *talkTos = NULL);
void OnDataMsg(CChatDoc *pDoc, char *szNickname, const char *szUserIdent, char *szData, BYTE msgType = MT_CHANNELSEND);

CRoomInfo *NewDefaultProto(CChatDoc *doc) {
	CIrcProto *proto = new CIrcProto;
	if (proto) {
		proto->m_sock = &serverConn;
		proto->m_doc = doc;
	}
	return proto;
}

BOOL CommunicationInits() {
	if (theApp.m_bDoCB32 && !CNmProto::Initialize()) return FALSE;
	cui.m_pvIrcProto = NewDefaultProto(NULL);

	if (!AfxSocketInit()) {
		AfxMessageBox(IDP_SOCKETS_INIT_FAILED);
		return FALSE;
	}

	return SUCCEEDED(serverConn.HrInitAlloc(g_nDefaultIOBuff));
}

void CommunicationCleanup() {
	if (theApp.m_bDoCB32) CNmProto::Uninitialize();
	else if (GetIrcProto()) delete (GetIrcProto());
}


void CIrcProto::SendMessageText(char *szMesg) {
	// TRACE("Sending message: %s\n", szMesg);
	m_sock->Send(szMesg, strlen(szMesg));

#if 0
	if (theApp.m_charSet == ANSI_CHARSET)		// fast path
		m_sock->Send(szMesg, strlen(szMesg));
	else {
		BOOL ConvertEncodingOut(LPSTR *pszStr);
		BOOL bMustFree = ConvertEncodingOut(&szMesg);
		m_sock->Send(szMesg, strlen(szMesg));
		if (bMustFree) delete [] szMesg;
	}
#endif
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


BOOL CIrcProto::bChatSendToTarget(const char *szAddressee, const char *szAnnotations, const char *szMesg, BYTE byteMode /*=0*/, BOOL bAsNotice /*=FALSE*/)
{
	BOOL		GetSendComicsData();

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
	{
		const char *GetMyUserName();
		nReceivingPrefixLen += strlen(GetMyUserName()) + 32;
	}

	nLen += nTargetLen + nAnnotationsLen + nMesgLen + nReceivingPrefixLen;	// final length of message on the receiving side

	if (nLen <= serverConn.m_nMaxMsgLength)
	{
		// message is short enough to be sent in one shot
		if (*szAnnotations && !GetSendComicsData() && IsIRCX())
		{
			sprintf(serverConn.m_szOutput2, "DATA %s %s :%s\r\n", szTarget, CCUDI1, szAnnotations);
			SendMessageText(serverConn.m_szOutput2);

			sprintf(serverConn.m_szOutput2, "%s %s :%s\r\n", (bAsNotice ? "NOTICE" : "PRIVMSG"),
															 szTarget, szMesg);
		}
		else
			sprintf(serverConn.m_szOutput2, "%s %s :%s%s\r\n", (bAsNotice ? "NOTICE" : "PRIVMSG"),
																szTarget, szAnnotations, szMesg);
		SendMessageText(serverConn.m_szOutput2);
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
		switch (byteMode)
		{
		case SM_ACTION:
			// szMesg = 0x01ACTION <data>0x01

			// REGISB: could include verb thinks into prefix for think button.
			nPrefixLen = g_nActionLen + 1;
			break;
		case SM_SOUND:
			// szMesg = 0x01SOUND <filename> <data>0x01
			nPrefixLen = g_nSoundLen + 1;
			//szTmp = szMesg + g_nSoundLen + 1;
			// sound filename is part of the prefix, we need to include it in each chunk
			//szTmp = strchr(szTmp, ' ');
			//ASSERT(szTmp);
			//nPrefixLen = szTmp - szMesg + 1;	// +1 for the trailing space
			break;
		case SM_AWAY:
			// szMesg = 0x01AWAY <data>0x01
			nPrefixLen = g_nAwayLen + 1;
			break;
		case SM_HERESINFO:
			// szMesg = # HeresInfo: <data>
			nPrefixLen = g_nHeresInfoLen + 1;	// +1 for # sign
			nSuffixLen = 0;						// no terminating 0x01
			break;
		case SM_SAY:
		case SM_THINK:
		case SM_WHISPER:
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

			if (*szAnnotations && !GetSendComicsData() && IsIRCX())
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

			if (SM_SOUND == byteMode)
			{
				byteMode = SM_ACTION;
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


BOOL CIrcProto::ChatSendPrivMesg(const char *szAddressee, const char *szAnnotations, const char *szMesg, char *szNMText, BOOL bAsNotice, BYTE byteMode)
{
	return bChatSendToTarget(szAddressee, szAnnotations, szMesg, byteMode, bAsNotice);
}


BOOL CIrcProto::ChatSendToChannel(const char *szAnnotations, const char *szMesg, char *szNMText /*=NULL*/, BYTE byteMode /*=0*/)
{
	return bChatSendToTarget(NULL, szAnnotations, szMesg, byteMode);
}


BOOL CIrcProto::ChatSetTopic(const char *szTopic) {
	if (*szTopic) szTopic = EncodeString(szTopic);
	sprintf(GetOutBuff(), "TOPIC %s :%s\r\n", m_strChannel, szTopic);
	SendMessageText(GetOutBuff());
	return TRUE;
}


void CIrcProto::ChatPartChannel(CDocument *doc1, BOOL hardDisconnect) {
	CChatDoc *doc = (CChatDoc *)doc1;
	if (m_bInRoom) {
		sprintf(GetOutBuff(), "PART %s\r\n", m_strChannel);  // exit gracefully
		SendMessageText(GetOutBuff());
	}

	if (hardDisconnect) {
		void ChatServerDisconnect(), GotPartChannel(CChatDoc *);
		ChatServerDisconnect();
		GotPartChannel(doc);
	} else if (GetConnectionStatus() == CX_INCHANNEL)
		SetConnectionStatus(CX_NOCHANNEL);
}

void CIrcProto::ChatJoinChannel()
{
	CString realChannelName;
	BOOL bIsIRCX = serverConn.m_bIrcXServer;

//	if (*((LPCTSTR) enterInfo.m_strChannel) != '#' && *((LPCTSTR) enterInfo.m_strChannel) != '&' && !bIsIRCX)
//		realChannelName = "#" + enterInfo.m_strChannel;
//	else 
	realChannelName = enterInfo.m_strChannel;

	if (bIsIRCX && bRatingsEnabled()) {
		sprintf(GetOutBuff(), "PROP %s PICS\r\n", realChannelName);
		SendMessageText(GetOutBuff());
		g_iQuery = QT_JOINPICS;
	} else {
		g_bCanViewUnrated = bCanViewUnrated(TRUE);	// give 'em a chance to reset this value
		if (g_bCanViewUnrated)
			ChatJoinAux(realChannelName);
	}
}

void CIrcProto::ChatJoinAux(const char *szChannelName) {
	if (enterInfo.m_strPassword.IsEmpty())
		sprintf(GetOutBuff(), "JOIN %s\r\n", szChannelName);
	else {
		int iEncoding = (*szChannelName == '#' || *szChannelName == '&') ? ENC_DBCS : ENC_UTF8;
		CString strPwd = EncodeString(enterInfo.m_strPassword, iEncoding);
		sprintf(GetOutBuff(), "JOIN %s %s\r\n", szChannelName, strPwd);
	}

	SendMessageText(GetOutBuff());
}

BOOL CIrcProto::ChatKickUser(const char *nick, const char *reason) {
	reason = EncodeString(reason);
	sprintf (GetOutBuff(), "KICK %s %s :%s\r\n", m_strChannel, nick, reason);
	SendMessageText(GetOutBuff());
	return TRUE;
}

BOOL CIrcProto::ChatBanUser(const char *pattern, BOOL bBan) {
	const char *flag = bBan ? "+b" : "-b";
	sprintf(GetOutBuff(), "MODE %s %s %s\r\n", m_strChannel, flag, pattern);
	// Need to do Decoding!!!
	SendMessageText(GetOutBuff());
	return TRUE;
}

BOOL CIrcProto::ChatSendInvitation(const char *nick) {
	sprintf(GetOutBuff(), "INVITE %s %s\r\n", nick, m_strChannel);
	SendMessageText(GetOutBuff());
	return TRUE;
}

BOOL CIrcProto::ChatChangeNick(const char *szNewNick) {
	BOOL bExtendedNickname(LPCTSTR szNickname);
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

void CRoomInfo::ChatSetOperator(CUserInfo *pui, int mode) {
	if (mode == UM_HOST) {
		sprintf(GetOutBuff(), "MODE %s +o %s\r\n", m_strChannel, pui->GetName());
		SendMessageText(GetOutBuff());
	} else {
		if (pui->IsOperator()) {
			sprintf(GetOutBuff(), "MODE %s -o %s\r\n", m_strChannel, pui->GetName());
			SendMessageText(GetOutBuff());
		}

		if (mode == UM_SPEAKER) {
			BOOL bIsModerated = currentRoom->m_dwModes & CM_MODERATED;
			if (bIsModerated) {
				sprintf(GetOutBuff(), "MODE %s +v %s\r\n", m_strChannel, pui->GetName());
				SendMessageText(GetOutBuff());
			}
		}
		if (mode == UM_SPECTATOR) {
			sprintf(GetOutBuff(), "MODE %s -v %s\r\n", m_strChannel, pui->GetName());
			SendMessageText(GetOutBuff());
		}
	}
}		


CIrcSocket::CIrcSocket(void)
{
    // Initialize SSPI related attributes
	m_pFuncTbl		= NULL;
	m_hSecLib		= NULL;
	m_bCredential	= FALSE;
	m_bContext		= FALSE;

	m_szInput		= NULL;
	m_szOutput2		= NULL;
	m_szMessage		= NULL;
	m_bSvrAuthSeq	= FALSE;		// Don't force server authentication sequence by default
	m_bUsrAuthSeq	= FALSE;		// Don't force user defined authentication sequence by default
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


//
//	CHAR	*szNickname			IN *your nickname. Required.
//	CHAR	*szUserName			IN *user name to use. Required.
//	CHAR	*szRealName			IN *real name to use (description). Not Required.
//	CHAR	*szPassword			IN *password. Not Required.
//								On authenticated login, you will be prompted by the security
//								dll if szUserName or szPassword are NULL
//
HRESULT CIrcSocket::HrIrcLogin(CHAR *szNickname, CHAR *szUserName, CHAR *szRealName, CHAR *szPassword)
{
	ASSERT(szNickname);

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
		sprintf(GetOutBuff(), "USER %s %s . :%s\r\n", szUserName ? szUserName : szNickname, szMachineName, szRealName);
		GetIrcProto()->SendMessageText(GetOutBuff());

		// From now on we are registered until we disconnect
		m_bRegistered = TRUE;
	}

	return NOERROR;
}


HRESULT CIrcSocket::HrIrcXLogin(BOOL bForceNextPackage)
{
	// Trying an authentication on an IRCX server
	const char	*GetMyName(), *GetMyUserName(), *GetMyRealName(), *szPackAvail;
	static BOOL	bLoopPwd = FALSE;

	// If the user does not force an authentication and the server allows anynomous
	// logins, then we just do a regular IRC login
	if (m_bAnonAllowed && !m_bSvrAuthSeq && !m_bUsrAuthSeq)
		return HrIrcLogin((CHAR*) GetMyName(), (CHAR*) GetMyUserName(), (CHAR*) GetMyRealName(), NULL /* szPassword */);

	if (bForceNextPackage || !bLoopPwd)
		// Go to the next security package
		m_nSecuPackIndex++;

	// The user wants to force an authentication
	if (m_bUsrAuthSeq)
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
					return HrIrcLogin((CHAR*) GetMyName(), (CHAR*) GetMyUserName(), (CHAR*) GetMyRealName(), NULL /* szPassword */);
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
				return HrIrcLogin((CHAR*) GetMyName(), (CHAR*) GetMyUserName(), (CHAR*) GetMyRealName(), NULL /* szPassword */);
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
                                (CHAR*) (LPCTSTR) theApp.m_ircServer,	// pszTargetName
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
		char *startPtr = strchr(serverConn.m_szInput, '\0');
		int space = serverConn.m_szInput + serverConn.m_nMaxMsgLength - startPtr;
		int nRead = Receive(startPtr, space);
		if (SOCKET_ERROR == nRead)
		{
			TRACE("Receive failed with error: %d\n", GetLastError());
			return;
		}
		startPtr[nRead] = '\0';
		char *eoc = strchr(serverConn.m_szInput, '\n');
		while (eoc) {
			eoc++;
			int comLen = eoc - serverConn.m_szInput;
			strncpy(serverConn.m_szMessage, serverConn.m_szInput, comLen);
			serverConn.m_szMessage[comLen] = '\0';

			// now move rest of message forward
			char *eob = strchr(serverConn.m_szInput, '\0');
			int nRest = eob - eoc;
			strncpy(serverConn.m_szInput, eoc, nRest);
			serverConn.m_szInput[nRest] = '\0';

			TRACE("Got message: %.100s\n", serverConn.m_szMessage);
			ProcessMessage(serverConn.m_szMessage);   // handle the message (*After clearing it from the buffer!!!)
			eoc = strchr(serverConn.m_szInput, '\n'); // must do this after process message, since code is reentrant (but single threaded)
		}
	}
	// TRACE("Leaving OnReceive.\n");
}


void CIrcSocket::OnConnect(int nErrorCode) {
	const char *GetMyServer(), *GetMyRealName(), *GetMyUserName();

	TRACE("Connecting (code = %d)...\n", nErrorCode);
	if (nErrorCode) {  // couldn't connect
		GetIrcProto()->SetConnectionStatus(CX_DISCONNECTED);
		CString mesg;
		mesg.LoadString(ID_ERR_CONNECT);
//		setupDlg.nWhatFailed = PORT;
		VERIFY(ReplaceToken(mesg, CString("%1"), GetMyServer()));
		AfxMessageBox(mesg);
		InitializeServerConnection();
		return;
	}
	// got a connection!
	// Is this an IRCX server?
	CRoomInfo *proto = GetIrcProto();
	proto->SendMessageText("MODE ISIRCX\r\n");
	m_bJustSentModeIsIrcX = TRUE;
}


void CIrcSocket::OnClose(int nErrorCode) {
	void ChatServerDisconnect();
	TRACE("Closing socket on error %d.\n", nErrorCode);
	ChatServerDisconnect();
	GetIrcProto()->SetConnectionStatus(CX_DISCONNECTED);
	
	// REGISB: added 11/13/97 because user would be disconnected for flooding without any notification
	// 0   				The function executed successfully.
	// WSAENETDOWN		The Windows Sockets implementation detected that the network subsystem failed.
	// WSAECONNRESET	The connection was reset by the remote side.
	// WSAECONNABORTED	The connection was aborted due to timeout or other failure.
	AfxMessageBox(IDS_CONNECTION_DROPPED);
}


void CIrcProto::OnLogin() {
	BOOL RequestedChannelList();
	void AddToServerList(const char *), StopIdentD();
	const char *GetMyChannel(), *GetMyServer();

	SetConnectionStatus(CX_NOCHANNEL);
	AddToServerList(GetMyServer());
	theApp.m_bInSearch = FALSE;			// can now search again (in case disconnected during last search

	if (RequestedChannelList() || *GetMyChannel() == '\0')
		theApp.OnChatroomList();
	else
		ChatJoinChannel();

	StopIdentD();
}


void SingleJoin(char *attedNick, void *doc) {
	AddAndExecute(new JoinEntry(new CUserInfo(attedNick)), (CDocument *) doc);
}

#ifdef IRCLOG
void StartFileInLoop() {
	char buff[1000];

	while (fgets(buff, sizeof(buff), theApp.m_fileIn) != NULL) {
		if (*buff != '\n')
			serverConn.ProcessMessage(buff);
	}

	fclose(theApp.m_fileIn);
	theApp.m_fileIn = NULL;
}
#endif IRCLOG


BOOL ChatServerConnect(const char *server) {
	GetIrcProto()->SetConnectionStatus(CX_CONNECTING);
	((CFrameWnd*)AfxGetMainWnd())->UpdateWindow();

	CString strServer;
	int port;
	char *colon = (char *) _mbschr((const UCHAR *)server, ':');
	if (colon) {
		strServer = CString(server, colon - server);
		port = atoi(colon+1);
	} else {
		strServer = server;
		port = 6667;

#if 0
		// if port wasn't specified and user is with aol, trying to connect
		// to our servers, use port 7000 instead to avoid AOL'd port blocker
		if (strnicmp(server, "mschat", 6) == 0 || strnicmp(server, "comicsrv", 8) == 0) {
			char name[200];
			if (gethostname(name, sizeof(name)) == 0) { // ie, success
				int len = strlen(name);
				if (len > 7) {  // 7 == strlen("aol.com")
					char *partialName = name + len - 7;
					if (stricmp(partialName, "aol.com") == 0) port = 7000;
				}
			}
		}
#endif

	}
	VERIFY(serverConn.Create());
	int rval = serverConn.Connect(strServer, port);
	if (rval || GetLastError() == WSAEWOULDBLOCK) {
		void StartIdentD();
		StartIdentD();
		return TRUE;
	} else {
		TRACE("rval = %u, lasterr = %u\n", rval, GetLastError());
		GetIrcProto()->SetConnectionStatus(CX_DISCONNECTED);
		CString mesg;
		mesg.LoadString(ID_ERR_CONNECT);
		VERIFY(ReplaceToken(mesg, CString("%1"), strServer));
		AfxMessageBox(mesg);
		return FALSE;
	}
}


void ChatServerDisconnect() {
	if (serverConn.m_hSocket != INVALID_SOCKET) 	// close last connection if necessary
		serverConn.Close();	
	
	// Close the port 113 for IdentD authentication
	void StopIdentD();
	StopIdentD();

	theApp.m_bAway = FALSE;  // no longer away
	theApp.m_ignores.RemoveAll();  // clear the ignore list

	// Reset socket class attributes
	serverConn.Reset();

	// Forced part all existing rooms (need to clear memberlists to avoid bad pui ref later!)
	void GotPartChannel(CChatDoc *);
	POSITION pos = g_docs.GetHeadPosition();
	while (pos) {
		CChatDoc *doc = (CChatDoc *) g_docs.GetNext(pos);
		if (doc->m_proto->GetType() == PC_IRC) {
			CIrcProto *proto = (CIrcProto *)(doc->m_proto);
			if (proto && proto->m_bInRoom) GotPartChannel(doc);
		}
	}
}

static BOOL bArg1, bArg2;

void ParseIt(const char *message, IRCPARSE *parse) {
	// parse prefix
	char *body;
	char prefixBuff[300];
	parse->nick[0] = '\0';
	parse->user[0] = '\0';
	parse->machine[0] = '\0';
//	parse->other[0] = '\0';
	parse->lastString = NULL;
	parse->nArgs = 0;
	ZeroMemory(parse->args, sizeof(char *) * MAXARGS);

	if (*message == ':') {		// there's a prefix
		message++;				// don't include the colon
		parse->bHasPrefix = TRUE;
		body = strchr(message, ' ');
		ASSERT(body);			// messages must have a body
		int prefixSize = body - message;
		prefixSize = min(prefixSize, sizeof(prefixBuff)-1);
		strncpy(prefixBuff, message, prefixSize);
		prefixBuff[prefixSize] = '\0';
		char *pStart = prefixBuff;
		char *pEnd = NULL;
		if (!CHANNELPREFIX(*pStart)) pEnd = strpbrk(pStart, "!@");	// parse snick (must be present)
		if (pEnd) {
			int nchars = pEnd - pStart;
			nchars = min(nchars, sizeof(parse->nick)-1);
			strncpy(parse->nick, pStart, nchars);
			parse->nick[nchars] = '\0';
			if (*pEnd == '!') {									// parse user
				pStart = pEnd+1;
				pEnd = strchr(pStart, '@');
				if (pEnd) {
					int nchars = pEnd - pStart;
					nchars = min(nchars, sizeof(parse->user)-1);
					strncpy(parse->user, pStart, nchars);
					parse->user[nchars] = '\0';
					nchars = sizeof(parse->machine)-1;
					strncpy(parse->machine, pEnd+1, nchars);	// nfield now pts to !, parse machine
					parse->machine[nchars] = '\0';
				}
			}
		} else if (strlen(prefixBuff) < sizeof(parse->nick))
			strcpy(parse->nick, prefixBuff);
	} else {
		parse->bHasPrefix = FALSE;
		body = UnConst(message);
	}

	while (TRUE) {
		while (my_isspace(*body)) body++;
		if (*body == ':') {
			body++;
			char *end = strpbrk(body, "\r\n");
			if (!end) end = strchr(body, '\0');
			int len = end - body;
			parse->lastString = (char *) malloc (sizeof(char) * (len+1));
			strncpy(parse->lastString, body, len);
			parse->lastString[len] = '\0';
			break;
		}
		char *token = GetToken(body, &body, " \r\n");
		if (!token) break;
		parse->args[parse->nArgs++] = strdup(token);
		if (parse->nArgs == MAXARGS) {
			ASSERT(0);
			break;
		}
	}
}


void FreeParse(IRCPARSE *parse) {
	if (parse->lastString) free(parse->lastString);
	for (int i = 0; i < parse->nArgs; i++)
		free(parse->args[i]);
}

void CSInString(char **szString, const char *szChannelName = NULL, CChatDoc *doc = NULL) {
	int iEncoding = (szChannelName && *szChannelName == '%') ? ENC_UTF8 : ENC_DBCS;
	if (doc && (doc->m_proto->m_dwModes & CM_MIC)) iEncoding = ENC_DBCS; // until MIC disappears
	if (!**szString || (theApp.m_charSet == ANSI_CHARSET && iEncoding == ENC_DBCS)) return;
	char *szNewString = strdup(DecodeString(*szString, iEncoding));
	free(*szString);
	*szString = szNewString;
}


void CSInPlace(char *szNick) {
	if (theApp.m_charSet == ANSI_CHARSET || *szNick == '\0') return;;
	char *szOldNick = strdup(szNick);
	CSInString(&szOldNick);
	strcpy(szNick, szOldNick);
	free(szOldNick);
}


void UpdateSpectators(CChatDoc *doc, BOOL moderated) {
	void ChatEmptyMemberList(CChatDoc *doc), *p;
	void AddToMembersList(CUserInfo *pui, CChatDoc *doc = NULL);
	CString nick;

	ChatEmptyMemberList(doc);
	POSITION pos = doc->m_mapNickToPtr.GetStartPosition();
	while (pos) {
		doc->m_mapNickToPtr.GetNextAssoc(pos, nick, p);
		CUserInfo *pui = (CUserInfo *) p;
		if (!pui->IsDeparted()) {
			pui->SetFlag(UF_SPECTATOR, !pui->IsOperator() && moderated && !pui->CheckFlag(UF_HASVOICE));
			AddToMembersList(pui, doc);
		}
	}
}

void FixMICChannelName(CChatDoc *doc) {
	void ChatSetChannel(const char *);
	doc->m_proto->m_strPrettyChannel = DecodeChan(doc->m_proto->m_strChannel, TRUE);
	ChatSetChannel(DecodeChan(enterInfo.m_strChannel, TRUE));	// successfully connected, so remember requested channel
	doc->SetLegalPath(doc->m_proto->m_strPrettyChannel);		// decoded true channel name
	doc->m_proto->SetConnectionStatus(doc->GetConnectionStatus());		// fix status bar string
}


void ParseChannelMode(CChatDoc *doc, const char *flags, const char *arg2, const char *arg3) {
	BOOL bAdd = TRUE;
	DWORD dwDelta = 0, addFlags = 0, subFlags = 0;
	while (*flags != '\0') {
		switch (*flags++) {
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
			doc->m_proto->m_dwMaxUsers = bAdd ? atoi(arg2) : 0;
			break;
		case 'k':
			dwDelta |= CM_CHANNELKEY;
			if (bAdd) doc->m_proto->m_strPassword = (arg3 && *arg3) ? arg3 : arg2;
			else doc->m_proto->m_strPassword = ""; // clear it out, since we check for changes in the value before setting
			break;
		case 'o':
		case 'q':
			if (bAdd) ChatChangeAdmin(doc, arg2, UF_OPERATOR, 0);
			else ChatChangeAdmin(doc, arg2, 0, UF_OPERATOR);
			break;
		case 'v':
			if (bAdd) ChatChangeAdmin(doc, arg2, UF_HASVOICE, 0);
			else ChatChangeAdmin(doc, arg2, 0, UF_HASVOICE);
			break;
		case 'f':
			dwDelta |= CM_NOFORMAT;
			if (bAdd) {
				theApp.m_bSaveViewMode = FALSE;
				doc->OnViewText();
			}
			break;
		case 'y':
			dwDelta |= CM_MIC;
			if (bAdd) FixMICChannelName(doc);
			break;
		}
		if (bAdd) addFlags |= dwDelta;
		else subFlags |= dwDelta;
	}

	doc->m_proto->m_dwModes |= addFlags;
	doc->m_proto->m_dwModes &= ~subFlags;

	if ((addFlags | subFlags) & CM_MODERATED) UpdateSpectators(doc, doc->m_proto->m_dwModes & CM_MODERATED);
}


void GetModeChars(DWORD dwFlags, char *buff) {
	// assumption: buff is large enough to hold mode string
	char *bptr = buff;
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


BOOL CIrcProto::ChatSetMode(DWORD newMode, DWORD newMaxUsers, const char *szNewPasswd) {
	char szMaxUsers[7] = "";
	const char *szKey;
	DWORD newSets = newMode & ~dwCurrentChannelMode;
	DWORD newUnSets = dwCurrentChannelMode & ~newMode;

	if ((newMode & CM_USERLIMIT) && newMaxUsers != dwCurrentUserLimit) {
		newSets |= CM_USERLIMIT;
		sprintf(szMaxUsers, "%d", newMaxUsers);
	} else newSets &= ~CM_USERLIMIT;

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


void CIrcProto::ChatGetIdentity(CUserInfo *pui) {
	sprintf(GetOutBuff(), "WHOIS %s\r\n", pui->GetName());
	SendMessageText(GetOutBuff());
	g_iQuery = QT_IDENT;
}


void CIrcProto::ChatKickUser(CUserInfo *pui) {
	sprintf(GetOutBuff(), "WHOIS %s\r\n", pui->GetName());
	SendMessageText(GetOutBuff());
	g_iQuery = QT_KICKDLG;
}


void CIrcProto::ChatBanUser(CUserInfo *pui) {
	g_iQuery = QT_BANDLG;
	if (pui) {
		sprintf(GetOutBuff(), "WHOIS %s\r\n", pui->GetName());
		SendMessageText(GetOutBuff());
	} else {
		sprintf(GetOutBuff(), "MODE %s +b\r\n", m_strChannel);
		SendMessageText(GetOutBuff());
	}
}


void GetBanString(const char *user, const char *host, CString& strBan) {
	if (!GetIrcProto()->IsIRCX() || !user || *user == '~')  // don't think user can really be NULL -- but be paranoid
		strBan.Format("*!*@%s", host);  // not authenticated
	else
		strBan.Format("*!%s@%s", user, host);
}

void ChatFillRoomList (CRoomList *rl) {
	CString strUsers;
	rl->m_user.GetWindowText(strUsers);
	strUsers.TrimLeft();
	g_iQuery = QT_ROOMQUERY;
	const char *users = strUsers;
	if (strlen(users) == 0) {
		if (!serverConn.m_bIrcXServer)
			sprintf(GetOutBuff(), "LIST\r\n");
		else {
			g_bCanViewUnrated = bCanViewUnrated();
			sprintf(GetOutBuff(), "LISTX\r\n");
		}
	} //else {
//		sprintf(GetOutBuff(), "WHOIS %s\r\n", users);
//	}

	GetIrcProto()->SendMessageText(GetOutBuff());
}


void ChatFillUserList (CUserList *ul) {
	CString user;

	ul->m_user.GetWindowText(user);
	user.TrimLeft();
	ul->m_persist->m_strUserFilter = user;	// will be used to filter responses in AddToUserList

	if (ul->m_persist->m_searchType == USERSEARCH_ALL)
		user = "";
	else if (user != "") {    // don't bother to create search args of the form **
		if (ul->m_persist->m_searchType == USERSEARCH_NICK && serverConn.m_bIrcXServer)
			user = CString("'*") + (EncodeNick(user, TRUE)+1);
		else user = "*" + user;
		user += "*";
	}

	g_iQuery = QT_USERQUERY;

	sprintf(GetOutBuff(), "WHO %s\r\n", user);
	GetIrcProto()->SendMessageText(GetOutBuff());	
}


void CIrcProto::ChatShowMOTD() {
	g_iQuery = QT_MOTD;		// force the show, even if theApp.m_bShowMOTD is turned off
	sprintf(GetOutBuff(), "LUSERS\r\nMOTD\r\n");
	SendMessageText(GetOutBuff());
}

//#define IS_CHANNEL_CHAR(c) // moved to nmproto.h

void CIrcSocket::ProcessMessage(char *line) {
	CString mesg;
	void SetMyNameNick(const char *);
	IRCPARSE parse;
	static CRoom *sRoom;
	static BOOL bAddIt;

#ifdef IRCLOG    // creates IRC message log **ONLY FOR LOCAL DEBUGGING**
	if (!theApp.m_fileIn) {
		static FILE *fp = NULL;
		if (!fp)
			fp = fopen("irc.txt", "w");
		if (fp) {
			fprintf(fp, "%s\n", line);
			fflush(fp);
		}
	}
#endif IRCLOG

	ParseIt(line, &parse);
	if (parse.nArgs <= 0) {
		ASSERT(0);
		ParseIt(line, &parse);
	}

	if (parse.nArgs <= 0) {
		ASSERT(0);		// should never happen?
		return;
	}

	char *command = parse.args[0];

	if (!strcmp(command, "PING")) {  // ignore sender for now   // DONE 2
		sprintf(GetOutBuff(), "PONG :%s\r\n", parse.lastString ? parse.lastString : "");
		TRACE("%s", GetOutBuff());
		Send(GetOutBuff(), strlen(GetOutBuff()));
	}
	else if (!strcmp(command, "PRIVMSG") || !strcmp(command, "NOTICE")) {  // TODO - charset conversion
		TRACE("Got a PrivMsg! (snick = %s, mach = %s, user = %s)\n", parse.nick, parse.machine, parse.user);
		if (parse.lastString && parse.nArgs >= 2 && *parse.nick && *parse.user) {
			CChatDoc *doc = NULL;
			BYTE msgType;
			if (CHANNELPREFIX(parse.args[1][0])) {
				doc = LookupDoc(parse.args[1]);
				msgType = MT_CHANNELSEND;
			} else msgType = MT_PRIVATEMSG;
			if (msgType == MT_PRIVATEMSG || doc) {
				CString strID;
				if (*parse.user && *parse.machine) strID.Format("%s@%s", parse.user, parse.machine);

				// interpret "# Appears as" message as being sent to all rooms they are a member of
				if (!doc && parse.lastString[0] == '#' && strncmp(((LPCTSTR)parse.lastString)+1, APPEARSPREFIX, g_nAppearsAsLen) == 0) {
					POSITION pos = g_docs.GetHeadPosition();
					while (pos) {
						doc = (CChatDoc *)g_docs.GetNext(pos);
						CUserInfo *pui = LookupPui(parse.nick, doc);
						if (pui && !pui->IsDeparted())
							OnTextMsg(doc, parse.nick, strID, parse.lastString, msgType);
					}
				} else {
					CSInString(&parse.lastString, parse.args[1], doc);
					OnTextMsg(doc, parse.nick, strID, parse.lastString, msgType);
				}
			}
		}
	}
	else if (!strcmp(command, "DATA"))
	{
		TRACE("Got a Data message! (snick = %s, mach = %s, user = %s)\n", parse.nick, parse.machine, parse.user);
		// CCUDI1 = Comic Chat User Display Info version 1
		if (parse.lastString && 
			parse.lastString[0] == '#' && 
			parse.nArgs >= 3 && 
			!strcmp(parse.args[2], CCUDI1) && 
			*parse.nick && 
			*parse.user)
		{
			CChatDoc*	doc = NULL;
			BYTE		msgType;

			if (CHANNELPREFIX(parse.args[1][0]))
			{
				doc = LookupDoc(parse.args[1]);
				msgType = MT_CHANNELSEND;
			}
			else
				msgType = MT_PRIVATEMSG;

			if (msgType == MT_PRIVATEMSG || doc)
			{
				CString strID;

				if (*parse.machine)
					strID.Format("%s@%s", parse.user, parse.machine);

				// interpret "# Appears as" message as being sent to all rooms they are a member of
				if (!doc && strncmp(((LPCTSTR)parse.lastString)+1, APPEARSPREFIX, g_nAppearsAsLen) == 0)
				{
					POSITION pos = g_docs.GetHeadPosition();
					while (pos)
					{
						doc = (CChatDoc*) g_docs.GetNext(pos);
						CUserInfo *pui = LookupPui(parse.nick, doc);
						if (pui && !pui->IsDeparted())
							OnDataMsg(doc, parse.nick, strID, parse.lastString, msgType);
					}
				}
				else
					OnDataMsg(doc, parse.nick, strID, parse.lastString, msgType);
			}
		}
	}
	else if (!strcmp(command, "WHISPER")) {
		CChatDoc *doc = LookupDoc(parse.args[1]);
		if (doc && *parse.nick) {
			CDWordArray talkTos;
			// compute talktos array, just in case message isn't cooked...
			void GetTalkTos(CChatDoc *doc, CDWordArray *talkTos, char *str);
			GetTalkTos(doc, &talkTos, parse.args[2]);
			CSInString(&parse.lastString, parse.args[1], doc);
			OnTextMsg(doc, parse.nick, "X", parse.lastString, MT_PRIVATEMSG, &talkTos); // for now, treated similarly to Private Message
		}
	}
	else if (!strcmp(command, "JOIN")) {   // DONE 2
		TRACE("Got a JOIN!\n");
		ASSERT(parse.lastString);
		if (!(stricmp(parse.nick, GetMyNickName()) == 0)) {				// Don't send or register self
			CSInPlace(parse.user);
			CString ident(parse.user);
			ident += "@";
			ident += parse.machine;
			CDocument *doc = LookupDoc(parse.lastString);
			AddAndExecute(new JoinEntry(new CUserInfo(parse.nick, ident), FALSE), doc);
		} else {
			BOOL bProcessAddChannel(const char *channel, CRoomInfo *);
			// REGISB 11/18/97 added if statement for Fix 4449
			if (bProcessAddChannel(parse.lastString, NewDefaultProto(NULL)))
			{
				ASSERT(currentRoom);
				strCurrentChannelTopic = "";
				dwCurrentChannelMode = 0;
				dwCurrentUserLimit = 0;

				// REGISB added 11/07/97
				if (!theApp.m_nMyIdentLength)
					theApp.m_nMyIdentLength = strlen(parse.user) + strlen(parse.machine) + 2; // + 2 for the ! and @ signs 

				sprintf(GetOutBuff(), "MODE %s\r\n", parse.lastString);
				currentRoom->SendMessageText(GetOutBuff());
				g_iQuery = QT_INITIALWHO;
				sprintf(GetOutBuff(), "WHO %s\r\n", parse.lastString);
				currentRoom->SendMessageText(GetOutBuff());
			}
		}
	}
	else if (!strcmp(command, "PART")) {  // Done 2
		TRACE("Got a PART!\n");
		CChatDoc *doc = LookupDoc(parse.args[1]);
		if ((stricmp(parse.nick, GetMyNickName()) == 0)) {
			void GotPartChannel(CChatDoc *);
			GotPartChannel(doc);
		} else
			AddAndExecute(new PartEntry(parse.nick), doc);
	}
	else if (!strcmp(command, "QUIT")) {	// collapse w/ PART?   // Done 2
		POSITION pos = g_docs.GetHeadPosition();
		while (pos) {
			CChatDoc *doc = (CChatDoc *) g_docs.GetNext(pos);
			if (doc->GetConnectionStatus() != CX_INCHANNEL) break;
			CUserInfo *pui = LookupPui(parse.nick, doc);
			if (pui && !pui->IsDeparted())
				AddAndExecute(new PartEntry(parse.nick), doc);
		}
	}
	else if (!strcmp(command, "353")) {     // Message saying who's already there. Register them.
		CChatDoc *doc = parse.nArgs >= 4 ? LookupDoc(parse.args[3]) : NULL;
		void ForEachWord(char *line, void (*func)(char *, void *), void *clientData, char *sep);
		if (parse.lastString && doc)
			ForEachWord(parse.lastString, SingleJoin, doc, " ");  // Done 2
	}
	else if (!strcmp(command, "NICK")) {		// change nickname
		if (parse.lastString) {
			BOOL setName = FALSE;
			POSITION pos = g_docs.GetHeadPosition();
			while (pos) {
				CChatDoc *doc = (CChatDoc *) g_docs.GetNext(pos);
				if (doc->m_proto->GetConnectionStatus() == CX_INCHANNEL) {
					CUserInfo *pui = LookupPui(parse.nick, doc);
					if (pui && !pui->IsDeparted()) {
						AddAndExecute(new NickEntry(parse.nick, parse.lastString), doc);
						setName = TRUE;
					}
				}
			}
			if (!setName) SetMyNameNick(parse.lastString);
		}
	}
	else if (!strcmp(command, "433")) {		// Nickname already in use response
		char *szBadNick = parse.nArgs >= 3 ? parse.args[2] : "";	// DONE 2
		(GetIrcProto())->TryNewNick(ID_ERR_DUPED_NICK, m_bIrcXServer ? DecodeNick(szBadNick) : szBadNick);
	}
	else if (!strcmp(command, "431") || !strcmp(command, "432")) {  // no or bad nickname
		char *szBadNick = parse.nArgs >= 2 ? parse.args[1] : "";		// DONE 2
		(GetIrcProto())->TryNewNick(ID_ERR_BAD_NICK, m_bIrcXServer ? DecodeNick(szBadNick) : szBadNick);
	}
	else if (!strcmp(command, "905")) {		// DONE 9
		AfxMessageBox(IDS_ERR_NICKNOTNOW);
	}
	else if (!strcmp(command, "001")) {		// We're logged in!  Join channel or show room list
		if (parse.nArgs >= 2)				// DONE 2
			SetMyNameNick(parse.args[1]);  // It's what the server thinks..
		GetIrcProto()->OnLogin();
	}
	else if (!strcmp(command, "403")) {  // bad chat room name
		void ShowBadChannelName(const char *roomName);
		ShowBadChannelName(parse.nArgs > 2 ? parse.args[2] : "");
	}
	else if (!strcmp(command, "AUTH")) {
		// We are in the middle of an authentication
		TRACE("Got an AUTH!\n");
		if ('S' == parse.args[2][0])
		{
			// AUTH NTLM S :blob...
			if (E_ABORT == HrGenerateAndSendAuthMsg(parse.lastString, parse.args[1]))
				// User cancelled the authentication with the current package.
				// Let's try the next one...
				HrIrcXLogin(TRUE /*bForceNextPackage*/);
		}
		else
		{
			// AUTH NTLM * REGISB@REDMOND 0
			ASSERT(!stricmp("*", parse.args[2]));
			// We're finally authenticated
			serverConn.m_bRegistered = TRUE;
			// This finalizes the login
			const char *GetMyName();
			HRESULT hr = serverConn.HrIrcLogin((CHAR*) GetMyName(), NULL /*szUserName*/, NULL /*szRealName*/, NULL /*szPassword*/);
			ASSERT(NOERROR == hr);
		}
	}
	else if (!strcmp(command, "KICK")) {		// someone was kicked
		void OnKick(CChatDoc *, char *kicker, char *kickee, char *mesg);  // DONE 2
		if (parse.lastString && parse.nArgs >= 3) {
			CChatDoc *doc = LookupDoc(parse.args[1]);
			CSInString(&parse.lastString, parse.args[1], doc);
			OnKick(doc, parse.nick, parse.args[2], SzControlLess(parse.lastString));
		}
	}
	else if (!strcmp(command, "322")) { // IRC list entry   // DONE 2
		if (parse.nArgs >= 4 && parse.lastString) {
			CRoom *room = new CRoom;
			CSInString(&parse.lastString);
			room->m_name = parse.args[2];
			room->m_prettyName = DecodeChan(parse.args[2]);
			room->m_nUsers = atoi(parse.args[3]);
			room->m_descr = parse.lastString;
			room->m_byteRegistered = FALSE;
			AddToRoomList(room);  // PICS test done in OnChatRoomList
		}
	}
	else if (!strcmp(command, "812")) {  // IRCX list entry
		if (sRoom) AddToRoomList(sRoom, bAddIt);
		sRoom = NULL;
		if (parse.nArgs >= 6 && parse.lastString) {
			sRoom = new CRoom;
			const char *roomName = parse.args[2];
			BOOL bMIC = (strchr(parse.args[3], 'y') != NULL);
			CSInString(&parse.lastString, bMIC ? NULL : roomName);
//			if (roomName[0] == '%' && (roomName[1] == '#' || roomName[1] == '&'))
//				roomName += 2;
			sRoom->m_name = roomName;
			sRoom->m_prettyName = DecodeChan(roomName, bMIC);
			sRoom->m_nUsers = atoi(parse.args[4]);
			sRoom->m_descr = SzControlLess(parse.lastString);
			sRoom->m_byteRegistered = (strchr(parse.args[3], 'r') != NULL);
			bAddIt = g_bCanViewUnrated;  // default
		}
	}
	else if (!strcmp(command, "813")) {		// PICS string (server-based.  should already be in windows charset.)
			bAddIt = bPassesRatings(parse.lastString);  // DONE 9
	}
	else if (!strcmp(command, "818")) {			// DONE 9 - PICS is server stored
		if (g_iQuery == QT_JOINPICS) {
			g_iQuery = QT_NONE;
			if (bPassesRatings(parse.lastString, TRUE))
				GetIrcProto()->ChatJoinAux(parse.args[2]);
		}
	}
	else if (!strcmp(command, "819")) {		// end of props
		if (g_iQuery == QT_JOINPICS) {		// DONE 9
			g_iQuery = QT_NONE;
			if (bCanViewUnrated(TRUE))
				GetIrcProto()->ChatJoinAux(parse.args[2]);
		}
	}
	else if (!strcmp(command, "924")) {		// prop test on non-existant room
		if (g_iQuery == QT_JOINPICS) {		// DONE 9
			g_iQuery = QT_NONE;
			if (bCanViewUnrated(TRUE))
				GetIrcProto()->ChatJoinAux(parse.args[2]);
		}
	}
	else if (!strcmp(command, "321") || !strcmp(command, "811")) {  // start of LIST or LISTX -- do same thing
		sRoom = NULL;				// DONE 9
		g_bCanViewUnrated = bCanViewUnrated();
		void StartRoomList();
		StartRoomList();
	}
	else if (!strcmp(command, "323") || !strcmp(command, "817") || !strcmp(command, "816")) {  // end of LIST or LISTX or LISTX truncation -- do same thing
		if (sRoom) AddToRoomList(sRoom, bAddIt);    // DONE 9
		sRoom = NULL;
		void EndRoomList();
		EndRoomList();
	}
	else if (!strcmp(command, "352")) {	 // DONE 2
		if (parse.nArgs >= 8) {
			if (g_iQuery == QT_INITIALWHO) {
				void UpdateIgnoreOnEntry(const char *, const char *, const char *, const char *);
				CSInString(&parse.args[3]);
				UpdateIgnoreOnEntry(parse.args[2], parse.args[6], parse.args[3], parse.args[4]);
			} else {
				CSInString(&parse.args[3]);  // user
				CUser *user = new CUser;
				user->m_nick = parse.args[6];
				user->m_ident = parse.args[3];
				user->m_ident += "@";
				user->m_ident += parse.args[4];
				user->m_room = parse.args[2];
				user->m_prettyRoom = DecodeChan(parse.args[2]); // unfortunately, don't know if it's MIC
				if (parse.args[6][0] == '\'')
					user->m_prettyNick = strdup(DecodeNick(parse.args[6]));
				if (parse.lastString) {
					CSInString(&parse.lastString);
					const char *fullName = strchr(parse.lastString, ' '); // jump over hopcount
					if (fullName) {
						while (my_isspace(*fullName)) fullName++;
						if (*fullName) user->m_fullName = fullName;
					}
				}
				void AddToUserList(CUser *);
				AddToUserList(user);
			}
		}
	}
	else if (!strcmp(command, "315")) {		// DONE 9
		void EndUserList();
		if (g_iQuery == QT_INITIALWHO)
			g_iQuery = QT_NONE;
		else EndUserList();
	}
	else if (!strcmp(command, "471")) {		// DONE 2
		mesg.LoadString(ID_ERR_CHANNELISFULL);
		VERIFY(ReplaceToken(mesg, CString("%1"), DecodeChan(parse.args[2])));   // don't know if it's MIC
		AfxMessageBox(mesg);
	}
	else if (!strcmp(command, "473")) {		// DONE 2
		mesg.LoadString(ID_ERR_INVITEONLY);
		VERIFY(ReplaceToken(mesg, CString("%1"), DecodeChan(enterInfo.m_strChannel)));  // don't know if it's MIC
		AfxMessageBox(mesg);
	}
	else if (!strcmp(command, "474")) {		// DONE 2
		mesg.LoadString(ID_ERR_BANNEDFROMCHAN);
		VERIFY(ReplaceToken(mesg, CString("%1"), DecodeChan(enterInfo.m_strChannel)));  // don't know if it's MIC
		AfxMessageBox(mesg);
	}
	else if (!strcmp(command, "ERROR")) {    // DONE 2
		if (parse.lastString) {
			CSInString(&parse.lastString);
			if (strstr(parse.lastString, "No IRC clients"))
				AfxMessageBox(IDS_MICONLY);
			else
				AfxMessageBox(parse.lastString);	// print the message verbatim (localization problem!!!)
			AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_FILE_NEW, 0);
		}
	}
//	else if (!strcmp(command, "375")) {   // start MOTD is ignored
//	}
	else if (!strcmp(command, "372") || !strcmp(command, "377")) { // 377 used by irc.sprynet.com
//		CSInString(&parse.lastString);    // Server stored string -- no conversion needed
		const char *motd = parse.lastString;  // DONE 9
		if (motd) {
			if (strncmp(motd, "- ", 2) == 0) motd += 2;
			if (strcmp(motd, "-") == 0) motd++;
			strMOTD += motd;
			strMOTD += "\r\n";
		}
	}
	else if (!strcmp(command, "251") || !strcmp(command, "252")
		|| !strcmp(command, "253") || !strcmp(command, "254") || !strcmp(command, "255")) {  
		CSInString(&parse.lastString);			// DONE 9 - Server stored string -- no conversion needed
		const char *luser = parse.lastString;
		if (luser) {
			if (parse.nArgs >= 3) {			// add first arg as string prefix...  Necessary for commands 252-254
				strLUSER += parse.args[2];
				strLUSER += " ";
			}
			strLUSER += luser;
			strLUSER += "\n";
		}
	}
	else if (!strcmp(command, "376")) {	// end MOTD			// DONE 9
		void ShowMOTD (const char *, const char *);
		if (!strMOTD.IsEmpty() || !strLUSER.IsEmpty())
			ShowMOTD(strLUSER, strMOTD);
		strMOTD = "";
		strLUSER = "";
	}
	else if (!strcmp(command, "311")) {  // RPL_WHOISUSER
		if (parse.nArgs >= 5) {			// DONE 2
			CSInString(&parse.args[3]);	// user
			void ShowIdentity(const char *nick, const char *user, const char *host);
			if (g_iQuery == QT_IDENT) ShowIdentity(parse.args[2], parse.args[3], parse.args[4]);
			else if (g_iQuery == QT_IGNOREIDENT) {
				void IgnoreUser(const char *, const char *, BOOL, BOOL);
				CString fullName(parse.args[3]);
				fullName += "@";
				fullName += parse.args[4];
				IgnoreUser(parse.args[2], fullName, bArg1, bArg2);
			} else {
				void DoBanDlg(const char *);
				CString strBan;
				GetBanString(parse.args[3], parse.args[4], strBan);
				if (g_iQuery == QT_KICKDLG) currentRoom->DoKickDlg(parse.args[2], strBan);
				if (g_iQuery == QT_BANDLG) {
					g_strBan = strBan;
					sprintf(GetOutBuff(), "MODE %s +b\r\n", strCurrentChannel);
					currentRoom->SendMessageText(GetOutBuff());
				}
			}
		}
	}
	else if (!strcmp(command, "332")) {					// DONE 2
		if (parse.nArgs >= 3 && parse.lastString) {
			CChatDoc *doc = LookupDoc(parse.args[2]);
			CSInString(&parse.lastString, parse.args[2], doc);
			if (doc) {
				if (doc->m_proto->m_prgdwTopicFormatting)
					doc->m_proto->m_prgdwTopicFormatting->RemoveAll();
				else
					doc->m_proto->m_prgdwTopicFormatting = new CDWordArray;

				doc->m_proto->m_strTopic = SzControlLess(parse.lastString, doc->m_proto->m_prgdwTopicFormatting);
			}
		}
	}
	else if (!strcmp(command, "TOPIC")) {				// DONE 0.5
		if (parse.nArgs >= 2 && parse.lastString) {
			CChatDoc *doc = LookupDoc(parse.args[1]);
			CSInString(&parse.lastString, parse.args[1], doc);
			if (doc) {
				if (doc->m_proto->m_prgdwTopicFormatting)
					doc->m_proto->m_prgdwTopicFormatting->RemoveAll();
				else
					doc->m_proto->m_prgdwTopicFormatting = new CDWordArray;

				doc->m_proto->m_strTopic = SzControlLess(parse.lastString, doc->m_proto->m_prgdwTopicFormatting);
			}
		}
	}
	else if (!strcmp(command, "324")) {
		if (parse.nArgs >= 4 && !strcmp(parse.args[2], currentRoom->m_strChannel)) {
			const char *flags, *arg2 = "", *arg3 = "";
			flags = parse.args[3];
			if (parse.nArgs >= 5)
				arg2 = parse.args[4];
			if (parse.nArgs >= 6)
				arg3 = parse.args[5];
			dwCurrentChannelMode = 0;			// next parse is absolute, not relative
			strCurrentChannelKey = "";
			CChatDoc *doc = LookupDoc(parse.args[2]);
			ParseChannelMode(doc, flags, arg2, arg3);
						
			if (enterInfo.m_bSetMode)
			{	// set modes if requested on channel creation
				doc->m_proto->ChatSetMode(enterInfo.m_dwModes, enterInfo.m_dwMaxUsers, enterInfo.m_strPassword);
				if (!enterInfo.m_strTopic.IsEmpty())
				{
					CString strControlFull = enterInfo.m_strTopic;
					if (enterInfo.m_prgdwTopicFormatting)
					{
						char* szCtrlFull = SzControlFull((LPCTSTR) enterInfo.m_strTopic, enterInfo.m_prgdwTopicFormatting);
						if (szCtrlFull)
						{
							strControlFull = CString(szCtrlFull);
							delete [] szCtrlFull;
						}
					}
					doc->m_proto->ChatSetTopic(strControlFull);
				}
			}
			bInitEnterInfo("");		// we've already taken care of the connecting sequence
		}
	}
	else if (!strcmp(command, "MODE")) {   // DONE 2
		if (parse.nArgs >= 3) {
			CChatDoc *doc = LookupDoc(parse.args[1]);
			if (doc) {
				const char *flags, *arg2 = "", *arg3 = "";
				flags = parse.args[2];
				if (parse.nArgs >= 4) {
					CSInString(&parse.args[3]);
					arg2 = parse.args[3];
				}
				if (parse.nArgs >= 5) {
					CSInString(&parse.args[4]);
					arg3 = parse.args[4];
				}
				ParseChannelMode(doc, flags, arg2, arg3);
			}
		}
	}
	else if (!strcmp(command, "INVITE")) {		// DONE 2
		if (parse.lastString) {
			void OnInvite(const char *inviter, const char *fullName, const char *room);
			CSInPlace(parse.user);  // necessary ?
			CString ident(parse.user);
			ident += "@";
			ident += parse.machine;
			OnInvite(parse.nick, ident, parse.lastString);
		}
	}
	else if (!strcmp(command, "910")) {
		// ":<servername> 910 * <secupackage> : Authentication failed
		// User account information provided was wrong, let's try again...
		// Display appropriate error message
		CString strMesg;
		strMesg.LoadString(ID_ERR_BADUSERINFO);
		AfxMessageBox(strMesg);
		m_bAuthFailed = TRUE;
		HrIrcXLogin(FALSE /*bForceNextPackage*/);
	}
	else if (!strcmp(command, "912")) {
		// ":<servername> 912 * <secupackage> : Unsupported authentication package
		// This error should theoretically never occur, just here to be on the safe side
		// Try next security package, or fail
		HrIrcXLogin(TRUE /*bForceNextPackage*/);
	}
	else if (!strcmp(command, "475")) {   // DONE 2
		void OnBadChannelPassword();
		enterInfo.m_strChannel = parse.args[2]; // in case multiple , sep'ed joins
		OnBadChannelPassword();
	}
	else if (!strcmp(command, "451")) {   // DONE 2
		// :You have not registered
		if (m_bJustSentModeIsIrcX)
		{
			// Don't want to expose this error to the user, it comes from the MODE ISIRCX\r\n command on an IRC server
			ASSERT(serverConn.m_bIrcXServer == FALSE);
			// Login Time, on IRC Server, anonymously
			const char *GetMyName(), *GetMyUserName(), *GetMyRealName();
			HRESULT hr = serverConn.HrIrcLogin((CHAR*) GetMyName(), (CHAR*) GetMyUserName(), (CHAR*) GetMyRealName(), NULL /* szPassword */);
			ASSERT(NOERROR == hr);
			m_bJustSentModeIsIrcX = FALSE;
		}
	}
	else if (!strcmp(command, "405")) {				// DONE 9
		AfxMessageBox(IDS_ERR_TOOMANYCHANNELS);
	}
	else if (!strcmp(command, "367")) {
		if (parse.nArgs >= 4)
			g_arrayBans.Add(parse.args[3]);
	}
	else if (!strcmp(command, "368")) {
		void DoBanDlg(const char *, CStringArray &);
		DoBanDlg(g_strBan, g_arrayBans);
		g_strBan = "";
		g_arrayBans.RemoveAll();
	}
	else if (!strcmp(command, "341")) {			// DONE 2
		void AcknowledgeInvite(const char *, const char *);
		if (parse.nArgs >= 4)
			AcknowledgeInvite(DecodeNick(parse.args[2]), DecodeChan(parse.args[3]));  // don't know if it's MIC
	}
	else if (!strcmp(command, "800")) {
		// Prefix		 = "keezer"
		// parse.args[0] = "800"
		// parse.args[1] = "*"						// no nickname specified yet
		// parse.args[2] = "0|1"					// we asked to turn into the IRCX mode or not
		// parse.args[3] = "0"						// IRCX version number
		// parse.args[4] = "NTLM,ANON"				// security packages available
		// parse.args[5] = "512"					// max message length
		// parse.args[6] = "*"

		// ASSERT('*' == parse.args[1][0]);

		// is this the first instance of 800 or the second?
		if ('0' == parse.args[2][0])
		{
			// first instance, we're still in IRC mode
			serverConn.m_bIrcXServer = TRUE;

			m_bJustSentModeIsIrcX = FALSE;

			// Create the SvrSecuPack string array
			// Are there any secu packages?
			if (parse.nArgs >= 7)
			{
				BOOL bEnd;
				CHAR *szHeadTmp, *szTmp;
				szHeadTmp = szTmp = parse.args[4];
				ASSERT('\0' != *szHeadTmp);
				do
				{
					if ((',' == *szTmp) || (bEnd = ('\0' == *szTmp)))
					{
						*szTmp = '\0';
						if (stricmp(g_szAnon, szHeadTmp))
							serverConn.m_rgszSvrSecuPack.Add(szHeadTmp);
						else
							serverConn.m_bAnonAllowed = TRUE;
						if (!bEnd) 
							szHeadTmp = ++szTmp;
					}
					else
						szTmp++;
				}
				while (!bEnd);
			}

			// Read the max message length
			SHORT nMaxMsgLength = atoi(parse.args[parse.nArgs-2]);

			// Is the max length bigger than our current buffers?
			if (serverConn.m_nMaxMsgLength < nMaxMsgLength)
			{
				serverConn.HrInitAlloc(nMaxMsgLength);
				theApp.HrAllocBuffer(nMaxMsgLength);
			}

			// REGISB: revisit since HrInitAlloc might return OOM

			// finally switch to IRCX mode
			GetIrcProto()->SendMessageText("IRCX\r\n");
		}
		else
		{
			// second instance, we're already in IRCX mode
			// Login Time
			HrIrcXLogin(TRUE /*bForceNextPackage*/);
		}	
	}
	#ifdef DEBUG
	else
		TRACE("Untreated message: sender nick = %s, mach = %s, user = %s, command = %s, last string = %s\n", 
				parse.nick ? parse.nick : "", 
				parse.machine ? parse.machine : "", 
				parse.user ? parse.user : "", 
				command,
				parse.lastString ? parse.lastString : "");
	#endif // DEBUG
	FreeParse(&parse);
}

void CIrcProto::DoIgnoreUser(CUserInfo *pui, BOOL bIgnore, BOOL bAutoIgnore) {
	void IgnoreUser(const char *, const char *, BOOL, BOOL);
	ASSERT(pui);
	if (pui->GetFullName().IsEmpty()) {
		sprintf(GetOutBuff(), "WHOIS %s\r\n", pui->GetName());
		SendMessageText(GetOutBuff());
		bArg1 = bIgnore;
		bArg2 = bAutoIgnore;
		g_iQuery = QT_IGNOREIDENT;
	}
	else
		IgnoreUser(pui->GetName(), pui->GetFullName(), bIgnore, bAutoIgnore);

	// REGISB: added 11/17/97
	// user's status needs to be updated too in case s/he isn't present in any of our rooms
	// and IgnoreUser does not do the job
	// djk -- not necessary anymore, since this is done in IgnoreUser
	//pui->Ignore(bIgnore);
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
	VERIFY(g_identd.Create(113));
	int rval = g_identd.Listen(1);
	ASSERT(rval || GetLastError() == WSAEWOULDBLOCK);
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
	const char *GetMyUserName();
	if (!nErrorCode) {
		int nRead = Receive(buff, sizeof(buff)-1);
		buff[nRead] = '\0';
		char *eoc = strchr(buff, '\r');
		if (eoc) *eoc = '\0';
		sprintf(GetOutBuff(), "%s : USERID : UNIX : %s\r\n", buff, GetMyUserName());
		Send(GetOutBuff(), strlen(GetOutBuff()));
	}
}


const char *EncodeNick(const char *szNick, BOOL bEscapeWildcards) {
	static char szEncoded[128];
	char *szUtf8 = NULL;
	int pCChOut, a;
	if (!(a = MultiByteToWideChar(GetACP(), MB_PRECOMPOSED, szNick, -1, theApp.m_wszBuffer, theApp.m_nBufferSize))) goto error;

	if (!bConvertWideStringToUTF8(theApp.m_wszBuffer, 0, &szUtf8, &pCChOut, TRUE, FALSE, TRUE, bEscapeWildcards)) goto error;
	if (!szUtf8) goto error;
	strcpy(szEncoded, szUtf8);
	delete [] szUtf8;
	return szEncoded;

error:
	ASSERT(0);
	strcpy(szEncoded, szNick);
	return szEncoded;
}

const char *DecodeNick(const char *szNick) {
	static char szEncoded[128];
	LPWSTR wszBuff;
	int pCChOut, a;
	if (*szNick != '\'') return szNick;
	if (!bConvertUTF8StringToWide(szNick, 0, &wszBuff, &pCChOut, TRUE, FALSE, TRUE)) goto error;
	if (!wszBuff) goto error;

	if (!(a = WideCharToMultiByte(GetACP(), 0, wszBuff, -1, szEncoded, sizeof(szEncoded), NULL, NULL))) goto error;
	delete [] wszBuff;
	return szEncoded;

error:
	ASSERT(0);
	strcpy(szEncoded, szNick);
	return szEncoded;
}

#define US_CODEPAGE	1252

const char *DecodeChan(const char *szChannel, BOOL bForceDBCS) {
	int iPrefix = 0;
	char firstChar = *szChannel;
	if (firstChar != '%' && firstChar != '#' && firstChar != '&') return szChannel;

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
		strcpy(theApp.m_szBuffer, szUtf8);
		delete [] szUtf8;
		return theApp.m_szBuffer;
	}

error:
	ASSERT(0);
	strcpy(theApp.m_szBuffer, szString);
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

long GetMyIP() {
	SOCKADDR addr;
	SOCKADDR_IN *pAddr;
	int len = sizeof(SOCKADDR);
	if (!serverConn.GetSockName(&addr, &len)) return 0;
	pAddr = (SOCKADDR_IN *)(&addr);
	long l = ntohl(pAddr->sin_addr.S_un.S_addr);
	return (l);
}
