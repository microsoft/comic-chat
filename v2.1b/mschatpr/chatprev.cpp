//=--------------------------------------------------------------------------=
// ChatPrEv.Cpp
//=--------------------------------------------------------------------------=
// Copyright 1995-1996 Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=
//
#include "StdAfx.H"
#include "ChatPr.H"
#include "IrcMsg.H"
#include "CCError.H"
#include "CItmsObj.H"
#include "ChObj.H"

// for ASSERT and FAIL
//
SZTHISFILE

void CMsChatPr::TreatIrcMsg(PPRIRCMSG pPrIrcMsg, BOOL bFree)
{
	BOOL bEnableDefault = TRUE;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CMsChatPr::TreatIrcMsg");

	// If we are in Raw Mode, we first need to package the message and fire the OnProtocolMessage event
	// OnProtocolMessage(BSTR MessagePrefix, BSTR MessageCommand, VARIANT MessageParameters, BOOL *EnableDefault)
	if (!m_bProcessProtocolMessages)
		FireProtocolMessage(pPrIrcMsg, &bEnableDefault);

	if (bEnableDefault)
	{
		// Figure out what command it is...
		// If the command begins with a number, we know it is a Result code
 		if (0 == pPrIrcMsg->uCode)
		{
			// A real command
			HandleCommand(pPrIrcMsg);
		}
		else
		{
			// Result or Error Code
			if (bIsErrorCode(pPrIrcMsg->uCode))
				HandleErrors(pPrIrcMsg->uCode, pPrIrcMsg);
			else
				HandleResultCode(pPrIrcMsg->uCode, pPrIrcMsg);
		}
	}

	if (bFree)
		m_cconn.FreeIrcMsg(pPrIrcMsg);
}


void CMsChatPr::HandleCommand(PPRIRCMSG pPrIrcMsg)
{
	BOOL bRet = TRUE;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CMsChatPr::HandleCommand");
	//
	// Find the command
	//
	SHORT nCmd = NGetCmd(pPrIrcMsg->szCommand);
	if (-1 == nCmd)
	{
		// sometimes we get a totally empty message on bad IRC servers
		// so we just ignore them...
		if (g_chEOS == pPrIrcMsg->szCommand[0] && g_chEOS == pPrIrcMsg->szPrefix[0])
			return;

		BOOL bEnableDefault;
		//
		// Not a command we recognize
		// We fire the OnProtocolMessage event
		// ASSERT(FALSE, "Unrecognized command - Firing OnProtocolMessage event in CMsChatPr::HandleCommand");
		FireProtocolMessage(pPrIrcMsg, &bEnableDefault);
		return;
	}
	
	ASSERT(nCmd < cmdidMax, "iCmd >= cmdidMax");
	//
	// Call command handlers
	//	
	switch (nCmd)
	{
		default:				
			ASSERT(FALSE, "Unexpected nCmd in CMsChatPr::HandleCommand");
			break;

		case cmdidClone:
			bRet = bHandleClone(pPrIrcMsg);
			break;
		
		case cmdidCreate:
			bRet = bHandleCreateJoin(pPrIrcMsg, nCmd);
			break;
		
		case cmdidData:
		case cmdidRequest:
		case cmdidReply:
			bRet = bHandleDataMsg(pPrIrcMsg, nCmd);
			break;
		
		case cmdidError:
			bRet = bHandleErrorMsg(pPrIrcMsg);
			break;

		case cmdidInvite:
			bRet = bHandleInviteMsg(pPrIrcMsg);
			break;
				
		case cmdidJoin:
			bRet = bHandleCreateJoin(pPrIrcMsg, nCmd);
			break;

		case cmdidKick:
			bRet = bHandleKick(pPrIrcMsg);
			break;

		case cmdidKnock:
			bRet = bHandleKnock(pPrIrcMsg);
			break;

		case cmdidMode:
			bRet = bHandleModeCommand(pPrIrcMsg);
			break;

		case cmdidNick:
			bRet = bHandleNick(pPrIrcMsg);
			break;
		
		case cmdidNotice:
		case cmdidPrivMsg:
			bRet = bHandleTextMsg(pPrIrcMsg, nCmd);
			break;
		
		case cmdidPart:
			bRet = bHandlePart(pPrIrcMsg);
			break;

		case cmdidPing:
			bRet = bHandlePing(pPrIrcMsg);
			break;
		
		case cmdidProp:
			bRet = bHandlePropChanged(pPrIrcMsg);
			break;

		case cmdidTopic:
			bRet = bHandleTopic(pPrIrcMsg);
			break;	

		case cmdidQuit:
			bRet = bHandleQuit(pPrIrcMsg);
			break;

		case cmdidWhisper:
			bRet = bHandleWhisper(pPrIrcMsg);
			break;
	}

	if (!bRet)
		switch (m_hrLastError)
		{
			case CC_E_SERVER:
			case E_FAIL:
			{
				BOOL bEnableDefault;
				OutputDebugThreadIdString("CMsChatPr::HandleCommand - Unexpected message format - firing OnProtocolMessage event\n");
				FireProtocolMessage(pPrIrcMsg, &bEnableDefault);
				break;
			}
			default:
				FireConnectionError(NULL, NULL, m_hrLastError);
		}
}


void CMsChatPr::HandleErrors(UINT uCode, PPRIRCMSG pPrIrcMsg)
{
	HRESULT				hr;
	BOOL				bRet, bErrFound = TRUE;
	BOOL				bOpeningChannel = FALSE;
	CHAR				*szChannelName = NULL;
	CHAR				*szNickname = NULL;
	CChatChannel		*pChannel = NULL;
	PPRQUERY			pPrQuery = NULL;
	enumAssociatedType	at = atChannel;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CMsChatPr::HandleErrors");

	if (bIsIrcXSocket())
	{
		// We're on an IRCX server
		switch (uCode)
		{
			case ERR_CHANOWNPRIVNEEDED:
				// ":<ServerName> 485 <Nickname> <ChannelName> :You're not channel owner
				hr = CC_E_NOTOWNER;
				break;

			case ERR_CANTCHANGEUSERMODE:
				// ":<ServerName> 502 <Nickname> :Cant change mode for other users
				hr = CC_E_CANTCHANGEUSERMODE;
				break;

			case ERR_NOJOINDYNAMIC:
				// ":<ServerName> 552 <Nickname> <ChannelName> :Cannot join dynamic channels due to admin restriction
				hr = CC_E_NOJOINDYNAMIC;
				break;

			case ERR_NODYNAMICCHANNELS:
				// ":<ServerName> 553 <Nickname> <ChannelName> :Cannot create dynamic channels due to admin restriction
				hr = CC_E_NODYNAMICCHANNELS;
				break;

			case ERR_AUTHONLY:
				// ":<ServerName> 556 <Nickname> <ChannelName> :Only authenticated users may join channel
				hr = CC_E_AUTHONLY;
				break;

			case ERR_OVERFLOWABORT:
				// ":<ServerName> 557 <Nickname> :Command aborted to prevent output buffer overflow
				hr = CC_E_OVERFLOWABORT;
				break;

			case ERR_BADTAG:
				// ":<ServerName> 904 <Nickname> <Command> :Bad message tag
				hr = CC_E_BADTAG;
				break;

			case ERR_BADPROPERTY:
				// ":<ServerName> 905 <Nickname> <Object> :Bad property specified"
				hr = CC_E_BADPROPERTY;
				break;

			case ERR_BADVALUE:
				// ":<ServerName> 906 <Nickname> <Object> :Bad value specified
				hr = CC_E_BADVALUE;
				break;

			case ERR_SECURITY:
				// ":<ServerName> 908 <Nickname> :No permissions to perform command"
				// unfortunately the error message does not specify which object is 
				// involved 
			case ERR_NOACCESS:
				// ":<ServerName> 913 <Nickname> <*> :No access"
				hr = CC_E_PERMISSIONDENIED;
				break;

			case ERR_NOWHISPER:
				// ":<ServerName> 923 <Nickname> <ChannelName> :Does not permit whispers
				hr = CC_E_NOWHISPER;
				break;

			case ERR_NOSUCHOBJECT:
				// ":<ServerName> 924 <Nickname> <ObjectName> :No such object found"
				if ('#' == pPrIrcMsg->szParams[1][0] ||
					'&' == pPrIrcMsg->szParams[1][0] ||
					'%' == pPrIrcMsg->szParams[1][0])
					hr = CC_E_NOSUCHCHANNEL;
				else
					hr = CC_E_NOSUCHNICK;	// REGISB: don't know if this is wise. It could concern a member too 
				break;						// instead of a user!  Should we have a CC_E_NOSUCHOBJECT error code?

			case ERR_NOTSUPPORTED:
				// ":<ServerName> 927 <Nickname> <ObjectName> :Command not supported by object"
				hr = CC_E_UNKNOWNCOMMAND;
				break;

			case ERR_INTERNALERROR:
				// ":<ServerName> 999 <Nickname> :Unknown error code <ErrorCode>"
				hr = CC_E_SERVER;
				break;

			default:
				bErrFound = FALSE;
				break;
		}

		if (((ERR_SECURITY == uCode) || (CC_E_BADPROPERTY == hr) || (CC_E_NOSUCHCHANNEL == hr)) && m_lPropInProgress > 0L)
		{
			// This error could result from a channel prop query
			if (m_listQuery.bFindCellFromData(1L,
											  g_nSearchByAssociatedType,
											  (PVOID) &at,
											  ERR_SECURITY == uCode ? g_nSearchNone : g_nSearchByName,
											  ERR_SECURITY == uCode ? NULL : pPrIrcMsg->szParams[1],
											  (PVOID*) &pPrQuery,
											  NULL))
			{
				ASSERT(pPrQuery, "pPrQuery is NULL in CMsChatPr::HandleErrors");
				ASSERT(pPrQuery->uHeadReplyCode == RPL_PROPLIST, "pPrQuery->uHeadReplyCode != RPL_PROPLIST in CMsChatPr::HandleErrors");
				bRet = m_listQuery.bRemoveCell((PVOID) pPrQuery);
				ASSERT(bRet, "bRemoveCell failed in CMsChatPr::HandleErrors");
				m_lPropInProgress--;
				ASSERT(m_lPropInProgress >= 0L, "m_lPropInProgress < 0L in CMsChatPr::HandleErrors");
			}
		}
	}
	else
	{
		// We're on a IRC or MIC 1.0 server
		switch (uCode)
		{
			case ERR_CANNOTJOINMICONLY:
				//	":<ServerName> 900 <Nickname> %s :Cannot join MIC only channel with IRC client"
				hr = CC_E_NOJOINMICONLY;
				break;

			case ERR_CANNOTJOINFROMREMOTE:
				//	":<ServerName> 901 <Nickname> %s :Cannot join channel from remote server (+r)"
				hr = CC_E_NOJOINREMOTE;
				break;

			case ERR_CANNOTCREATEDYNAMIC:
				//	":<ServerName> 902 <Nickname> %s :Cannot create dynamic channels (admin)"
				hr = CC_E_NODYNAMICCHANNELS;
				break;

			case ERR_COMMANDNOTSUPPORTED:
				//	":<ServerName> 903 <Nickname> %s :Command not supported on this server"
				hr = CC_E_UNKNOWNCOMMAND;
				break;

			case ERR_ONLYAUTHCANJOIN:
				//	":<ServerName> 904 <Nickname> %s :Only authenticated users may join channel"
				hr = CC_E_AUTHONLY;
				break;

			case ERR_CANNOTCHANGENICK:
				//	":<ServerName> 905 <Nickname> :Nick changes are not permitted at this time, try again later"
				hr = CC_E_NICKNOCHANGE;
				break;

			case ERR_CANNOTMAKEHOST:
				//	":<ServerName> 906 <Nickname> %s :Cannot make host due to admin restriction"
				hr = CC_E_PERMISSIONDENIED;
				break;

			case ERR_CANNOTJOINDYNAMIC:
				//	":<ServerName> 907 <Nickname> %s :Cannot join dynamic channels due to admin restriction"
				hr = CC_E_NOJOINDYNAMIC;
				break;

			case ERR_UNKNOWNERROR:
				//	":<ServerName> 999 <Nickname> :Unknown error code %d"
				hr = CC_E_SERVER;
				break;

			default:
				bErrFound = FALSE;
				break;
		}
	}


	// IRC2 error codes
	switch (uCode)
	{	 
		default:
			if (!bErrFound)
			{
				// ASSERT(FALSE, "Unexpected IRC error in CMsChatPr::HandleErrors"); // REGISB: can actually easily occur when using SendProtocolMessage method
				BOOL bEnableDefault;
				OutputDebugThreadIdString("CMsChatPr::HandleErrors - Unexpected Error Code - Firing OnProtocolMessage event\n");
				FireProtocolMessage(pPrIrcMsg, &bEnableDefault);
				return;
			}
			break;
		
		case ERR_NICKTOOFAST:
			// <nick> :Nick change too fast. Please wait 1 seconds.
			hr = CC_E_NICKTOOFAST;
			break;

		case ERR_NICKNOCHANGE:
			// <nick> :Nick name changes not permitted.
			hr = CC_E_NICKNOCHANGE;
			break;

		case ERR_ERRONEUSNICKNAME:
			// <nick> :Erroneus nickname
			hr = CC_E_BADNICKNAME;
			break;

		case ERR_NICKNAMEINUSE:
			// <nick> :Nickname is already in use
			hr = CC_E_NICKNAMEINUSE;
			break;
		
		case ERR_NICKCOLLISION:
			// <nick> :Nickname collision KILL
			hr = CC_E_NICKCOLLISION;
			break;
		
		case ERR_NOSUCHNICK:
		{
			// <nick> :No such nick/channel

			// This error could result from a user prop query, or user listing
			PPRQUERY			pPrQueryU = NULL, pPrQueryUL = NULL;
			enumAssociatedType	atU = atUser, atUL = atQuery;
			enumQueryType		qt = qtUserList;
			LONG				lPosU = 0L, lPosUL = 0L;

			if (m_lWhoIsInProgress > 0L)
				m_listQuery.bFindCellFromData(1L,
											  g_nSearchByAssociatedType,
											  (PVOID) &atU,
											  g_nSearchByNickname,
											  pPrIrcMsg->szParams[1],
											  (PVOID*) &pPrQueryU,
											  &lPosU);

			if (m_lWhoIsListingInProgress > 0L)
				m_listQuery.bFindCellFromData(1L,
											  g_nSearchByAssociatedType,
											  (PVOID) &atUL,
											  g_nSearchByQueryType,
											  (PVOID) &qt,
											  (PVOID*) &pPrQueryUL,
											  &lPosUL);

			if (pPrQueryU && (lPosU < lPosUL || 0L == lPosUL))
			{
				bRet = m_listQuery.bRemoveCell((PVOID) pPrQueryU);
				ASSERT(bRet, "bRemoveCell failed in CMsChatPr::HandleErrors");

				m_lWhoIsInProgress--;
				ASSERT(m_lWhoIsInProgress >= 0L, "m_lWhoIsInProgress < 0L in CMsChatPr::HandleErrors");
			}

			if (pPrQueryUL && (lPosUL < lPosU || 0L == lPosU))
			{
				// Case of a ListUsers call that has no response
				bRet = m_listQuery.bRemoveCell((PVOID) pPrQueryUL);
				ASSERT(bRet, "bRemoveCell failed in CMsChatPr::HandleErrors");

				m_lWhoIsListingInProgress--;
				ASSERT(m_lWhoIsListingInProgress >= 0L, "m_lWhoIsListingInProgress < 0L in CMsChatPr::HandleErrors");
						
				FireEndEnumeration(NULL, etUsers);
				// We don't expose the error, the OnEndEnumeration event marks the end of the listing
				return;
			}

			hr = CC_E_NOSUCHNICK;
			break;
		}
		case ERR_USERNOTINCHANNEL:
			// <nick> <channel> :They aren't on that channel
			hr = CC_E_NOSUCHMEMBER;
			break;
				
		case ERR_INVITEONLYCHAN:
			// <channel> :Cannot join channel (+i)
			hr = CC_E_INVITEONLYCHANNEL;
			break;

		case ERR_CHANNELISFULL:
			// <channel> :Cannot join channel (+l)
			hr = CC_E_CHANNELFULL;
			break;

		case ERR_NOSUCHCHANNEL:
			// <channel> :No such channel
			hr = CC_E_NOSUCHCHANNEL;
			break;

		case ERR_BANNEDFROMCHAN:
			// <channel> :Cannot join channel (+b)
			hr = CC_E_BANNEDFROMCHANNEL;
			break;

		case ERR_PASSWDMISMATCH:
			// :Password incorrect"
			hr = CC_E_BADPASSWORD;
			break;

		case ERR_YOUREBANNEDCREEP:
			// :You are banned from this server"
			hr = CC_E_BANNEDFROMSERVER;
			break;

		case ERR_YOUWILLBEBANNED:
			hr = CC_E_WILLBEBANNED;
			break;

		case ERR_BADCHANNELKEY:
			// <channel> :Cannot join channel (+k)
			hr = CC_E_CHANNELBADKEY;
			break;

		case ERR_TOOMANYCHANNELS:
			// <channel> :You have joined too many channels
			hr = CC_E_TOOMANYCHANNELS;
			break;
		
		case ERR_NOTREGISTERED:
			// :You have not registered
			if (!bIsLoggedOn())	// Don't want to expose this error if we're not yet logged in
				return;			// It comes from the MODE ISIRCX\r\n\ command
			hr = CC_E_NOTREGISTERED;
			break;

		case ERR_ALREADYREGISTERED:
			// :You may not reregister
			hr = CC_E_ALREADYREGISTERED;
			break;
		
		case ERR_TOOMANYTARGETS:
			// <target> :Duplicate recipients. No message
		case ERR_NORECIPIENT:
			// :No recipient given (<command>)
			hr = CC_E_INVALIDRECIPIENTLIST;
			break;				
		
		case ERR_USERONCHANNEL:
			// <nick> <channel> :is already on channel
			hr = CC_E_ALREADYONCHANNEL;
			break;
		
		case ERR_CANNOTSENDTOCHAN:
			// <channel> :Cannot send to channel
			hr = CC_E_CANTSEND;
			break;

		case ERR_NOTONCHANNEL:
			// <channel> :You're not on that channel
			hr = CC_E_NOTONCHANNEL;
			break;

		case ERR_KEYSET:
			// <channel> :Channel key already set
			hr = CC_E_CHANNELKEYALREADYSET;
			break;

		case ERR_UNKNOWNCOMMAND:
			// <Command> :Unknown command"
			hr = CC_E_UNKNOWNCOMMAND;
			break;

		case ERR_UMODEUNKNOWNFLAG:
			// :Unkown MODE flag
		case ERR_UNKNOWNMODE:
			// <char> :is unknown mode char to me
			hr = CC_E_PROPMODE;
			break;
		
		case ERR_CHANOPRIVSNEEDED:
			// <channel> :You're not channel operator
			hr = CC_E_NOTHOST;
			break;

		case ERR_NOPRIVILEGES:
			// :Permission Denied - You're not an IRC operator
			hr = CC_E_NOTSYSOP;
			break;

		case ERR_NOMOTD:
			// :MOTD File is missing
			hr = CC_E_NOMOTD;
			break;
	}

	if (ERR_NOTONCHANNEL == uCode || ERR_NOSUCHCHANNEL == uCode)
	{
		// This error could result from an async channel prop query
		if (m_listQuery.bFindCellFromData(1L,
										  g_nSearchByAssociatedType,
										  (PVOID) &at,
										  g_nSearchByName,
										  pPrIrcMsg->szParams[1],
										  (PVOID*) &pPrQuery,
										  NULL))
		{
			ASSERT(pPrQuery, "pPrQuery is NULL in CMsChatPr::HandleErrors");
			switch (pPrQuery->uHeadReplyCode)
			{
			case RPL_LISTSTART:
				ASSERT(m_lListInProgress > 0L, "m_lListInProgress <= 0L in CMsChatPr::HandleErrors");
				break;

			case RPL_BANLIST:
				m_lBannedListInProgress--;
				ASSERT(m_lBannedListInProgress >= 0L, "m_lBannedListInProgress < 0L in CMsChatPr::HandleErrors");
				break;

			case RPL_CHANNELMODEIS:
				m_lModeIsInProgress--;
				ASSERT(m_lModeIsInProgress >= 0L, "m_lListInProgress < 0L in CMsChatPr::HandleErrors");
				break;
				default:
				ASSERT(FALSE, "Unexpected uHeadReplyCode in CMsChatPr::HandleErrors");
				break;
			}
			bRet = m_listQuery.bRemoveCell((PVOID) pPrQuery);
			ASSERT(bRet, "bRemoveCell failed in CMsChatPr::HandleErrors");
		}
	}

	// Set the szChannelName variable
	switch (uCode)
	{
		// IRC/IRCX common errors
		case ERR_USERONCHANNEL:
			// <nick> <channel> :is already on channel
		case ERR_USERNOTINCHANNEL:
			// <nick> <channel> :They aren't on that channel
			szChannelName = pPrIrcMsg->szParams[2];
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
		case ERR_CANNOTSENDTOCHAN:
			// <channel> :Cannot send to channel
		case ERR_NOTONCHANNEL:
			// <channel> :You're not on that channel
		case ERR_CHANOPRIVSNEEDED:
			// <channel> :You're not channel operator
		case ERR_KEYSET:
			// <channel> :Channel key already set
			szChannelName = pPrIrcMsg->szParams[1];
	}

	if (bIsIrcXSocket())
		switch (uCode)
		{
			// IRCX only errors
			case ERR_NOJOINDYNAMIC:
				// <Channel> :Cannot join dynamic channels due to admin restriction
			case ERR_NODYNAMICCHANNELS:
				// <Channel> :Cannot create dynamic channels due to admin restriction
			case ERR_AUTHONLY:
				// <Channel> :Only authenticated users may join channel
			case ERR_BADPROPERTY:
				// <Object> :Bad property specified
			case ERR_BADVALUE:
				// <Object> :Bad value specified
			case ERR_NOWHISPER:
				// <Channel> :Does not permit whispers
			case ERR_CHANOWNPRIVNEEDED:
				// <Channel> :You're not channel owner
				szChannelName = pPrIrcMsg->szParams[1];
				break;

			case ERR_NOSUCHOBJECT:
				// <ObjectName> :No such object found
				if (CC_E_NOSUCHCHANNEL == hr)
					szChannelName = pPrIrcMsg->szParams[1];
				break;
		}
	else
		switch (uCode)
		{
			// MIC 1.0 errors
			case ERR_CANNOTJOINMICONLY:
				// <channel> :Cannot join MIC only channel with IRC client
			case ERR_CANNOTJOINFROMREMOTE:
				// <channel> :Cannot join channel from remote server (+r)
				szChannelName = pPrIrcMsg->szParams[1];
		}

	switch (uCode)
	{
		// IRC/IRCX common errors
		case ERR_NICKNAMEINUSE:
			// MIC1.0 :<server> 433 <nick> :Nickname is already in use
			// IRCX   :<server> 433 <nick> <nick> :Nickname is already in use
			szNickname = pPrIrcMsg->szParams[pPrIrcMsg->byteParams - 2];
			break;

		case ERR_NICKCOLLISION:
			// <nick> :Nickname collision KILL
		case ERR_NOSUCHNICK:
			// <nick> :No such nick/channel
		case ERR_USERNOTINCHANNEL:
			// <nick> <channel> :They aren't on that channel
		case ERR_USERONCHANNEL:
			// <nick> <channel> :is already on channel
			szNickname = pPrIrcMsg->szParams[1];
			break;
	}

	if (bIsIrcXSocket())
		switch (uCode)
		{
			// IRCX only errors
			case ERR_NOSUCHOBJECT:
				// <ObjectName> :No such object found
				if (CC_E_NOSUCHNICK == hr)
					szNickname = pPrIrcMsg->szParams[1];
				break;
		}

	//
	// Figure out who the error was sent to. Error target is usually the second parameter in the string
	//
	#ifdef DEBUG
		sprintf(g_szDebugStr, "CMsChatPr::HandleErrors - Error: %ld, Param1: %s, Param2: %s\n", uCode, pPrIrcMsg->szParams[0], pPrIrcMsg->szParams[1]);
		OutputDebugThreadIdString(g_szDebugStr);
	#endif

	if (FAILED(hr))
	{	
		// is the specified channel name part of our collection?
		if (szChannelName)
		{
			enumChannelState	chs1 = chsOpening;
			enumChannelState	chs2 = chsOpen;

			// We're looking for an open or opening channel with that name
			if (bFindChannel(g_nSearchByName, (PVOID) szChannelName, g_nSearchByState, (PVOID) &chs1, &pChannel))
				bOpeningChannel = TRUE;
			else
				bFindChannel(g_nSearchByName, (PVOID) szChannelName, g_nSearchByState, (PVOID) &chs2, &pChannel);

			if (pChannel)
				FireChannelError(pChannel, szNickname, hr);
			else
				FireConnectionError(szChannelName, szNickname, hr);
		}
		else
			FireConnectionError(NULL, szNickname, hr);
	}

	if (bOpeningChannel)
	{
		ASSERT(pChannel, "pChannel is NULL in CMsChatPr::HandleErrors");
		switch (uCode)
		{	 
			case ERR_CANNOTJOINMICONLY:
			case ERR_CANNOTJOINFROMREMOTE:
			case ERR_CANNOTCREATEDYNAMIC:
			case ERR_ONLYAUTHCANJOIN:
			case ERR_CANNOTJOINDYNAMIC:
				if (bIsIrcXSocket())
					break;

			case ERR_NOJOINDYNAMIC:
			case ERR_NODYNAMICCHANNELS:
			case ERR_AUTHONLY:
			case ERR_INVITEONLYCHAN:
			case ERR_CHANNELISFULL:
			case ERR_NOSUCHCHANNEL:
			case ERR_BANNEDFROMCHAN:
			case ERR_BADCHANNELKEY:
			case ERR_TOOMANYCHANNELS:
			case ERR_USERONCHANNEL:	// could be a failed invitation or user tried to join a channel he's already in
				// the user failed to create/join a channel
				// need to update the channel's state
				// bSignalCreateJoinFailure(szChannelName);  // treat return value
				InterlockedDecrement(&m_lOpeningChannels);
				ASSERT(m_lOpeningChannels >= 0, "m_lOpeningChannels < 0 in CMsChatPr::HandleErrors");

				HrPostNewChannelState(pChannel, chsClosed);
		}
	}

	switch (uCode)
	{
		case ERR_ERRONEUSNICKNAME:
		case ERR_NICKNAMEINUSE:
			if (csLogging == CsGetState())
				// the user failed to login
				// need to update the connection state to Connected
				HrPostNewConnectionState(csConnected);
	}
}


void CMsChatPr::HandleResultCode(UINT uCode, PPRIRCMSG pPrIrcMsg)
{
	BOOL bRet;
	BOOL bEnableDefault;

	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CMsChatPr::HandleResultCode");

	switch(uCode)
	{
		default:
		{
			// We don't recognize this result code and simply fire the OnProtocolMessage event
			OutputDebugThreadIdString("CMsChatPr::HandleResultCode - Unrecognized result code - firing OnProtocolMessage event\n");
			FireProtocolMessage(pPrIrcMsg, &bEnableDefault);
			break;
		}

		case RPL_WELCOME:
		case RPL_YOURHOST:
		case RPL_CREATED:
		case RPL_MYINFO:
			bRet = bHandleServerWelcome(pPrIrcMsg, uCode);
			break;

		case RPL_LUSERCLIENT:
		case RPL_LUSEROP:
		case RPL_LUSERUNKNOWN:
		case RPL_LUSERCHANNELS:
		case RPL_LUSERME:
			bRet = bHandleServerLUsers(pPrIrcMsg, uCode);
			break;

		case RPL_INFO:
		case RPL_ENDOFINFO:
			bRet = bHandleServerInfo(pPrIrcMsg, uCode);
			break;

		case RPL_MOTDSTART:
		case RPL_MOTD:
		case RPL_MOTD2:
		case RPL_ENDOFMOTD:
			bRet = bHandleMessageOfTheDay(pPrIrcMsg, uCode);
			break;

		case RPL_LISTSTART:
		case RPL_LIST:
		case RPL_LISTEND:
			bRet = bHandleListMsg(NULL, pPrIrcMsg, uCode);
			break;
		
		case RPL_CHANNELMODEIS:
			bRet = bHandleChannelModeIs(NULL, pPrIrcMsg);
			break;
			
		case RPL_BANLIST:
		case RPL_ENDOFBANLIST:
			bRet = bHandleBanList(NULL, pPrIrcMsg, uCode);
			break;

		case RPL_TOPIC:
			bRet = bHandleTopic(pPrIrcMsg);
			break;
		
		case RPL_NAMEREPLY:
			bRet = bHandleNameReply(pPrIrcMsg);
			break;
		
		case RPL_ENDOFNAMES:	  
			bRet = bHandleEndOfNames(pPrIrcMsg);
			break;
		
		case RPL_WHOISUSER:
		case RPL_WHOISSERVER:
		case RPL_WHOISOPERATOR:
		case RPL_WHOISIDLE:
		case RPL_WHOISCHANNELS:
		case RPL_ENDOFWHOIS:
			bRet = bHandleWhoIs(NULL, pPrIrcMsg, uCode);
			break;

		case RPL_WHOREPLY:
		case RPL_ENDOFWHO:
			bRet = bHandleWho(pPrIrcMsg, uCode);
			break;

		case RPL_AWAY:
			if (m_lWhoIsInProgress > 0L)
				bRet = bHandleWhoIs(NULL, pPrIrcMsg, uCode);
			else
				bRet = bHandleAway(pPrIrcMsg, uCode);
			break;

		case RPL_UNAWAY:
		case RPL_NOWAWAY:
			bRet = bHandleAway(pPrIrcMsg, uCode);
			break;

		case RPL_USERHOST:
			bRet = bHandleUserHost(NULL, pPrIrcMsg);
			break;


		// IRCX replies start here
		case RPL_PROPLIST:
		case RPL_PROPEND:
			bRet = bHandlePropReply(NULL, pPrIrcMsg, uCode);
			break;

		case RPL_LISTXSTART:
		case RPL_LISTXLIST:
		case RPL_LISTXPICS:
		case RPL_LISTXTRUNC:
		case RPL_LISTXEND:
			bRet = bHandleListMsg(NULL, pPrIrcMsg, uCode);
			break;
	}

	if (!bRet)
		switch (m_hrLastError)
		{
			case CC_E_NOTQUERIEDOBJECT:
			case CC_E_SERVER:
			case E_FAIL:
			{
				BOOL bEnableDefault;
				OutputDebugThreadIdString("CMsChatPr::HandleResultCode - Unexpected message format - firing OnProtocolMessage event\n");
				FireProtocolMessage(pPrIrcMsg, &bEnableDefault);
				break;
			}
			default:
				FireConnectionError(NULL, NULL, m_hrLastError);
		}
}


void CMsChatPr::ExposeChannelItems(PPRQUERY pPrQuery, BOOL bRemoveFromQueryList)
{
	BOOL				bRet;
	PPRCHANNEL			pPrChannel = NULL;
	CChatItems			*pCItems  = NULL;
	CChatChannel		*pChannel = NULL;
	enumChannelState	chs = chsOpen;

	// OutputDebugThreadIdString("CMsChatPr::ExposeChannelItems - Enter\n");

	ASSERT(pPrQuery, "pPrQuery is NULL in CMsChatPr::ExposeChannelItems");

	pPrChannel = (PPRCHANNEL) pPrQuery->pvData;

	ASSERT(pPrChannel, "pPrChannel is NULL in CMsChatPr::ExposeChannelItems");
	ASSERT(pPrQuery->szChannelName, "pPrQuery->szChannelName is NULL in CMsChatPr::ExposeChannelItems");

	if (pPrChannel->pChannel)
	{
		// Check if this channel is still here and open
		if (!m_listChannel.bFindCellFromData(1L /*lPosition*/, 
											 g_nSearchByName /*nSearchID1*/,
											 (PVOID) pPrQuery->szChannelName,
											 g_nSearchByState /*nSearchID2*/,
											 (PVOID) &chs,
											 (PVOID*) &pChannel,
											 NULL /*plPositionFound*/))
		{
			OutputDebugThreadIdString("CMsChatPr::ExposeChannelItems - Channel no more valid\n");
			return;
		}
		if (pPrChannel->pChannel != (PVOID) pChannel)
			// this channel name is now linked to another channel object
			pChannel = NULL;
		else
			// Make sure we expose the very latest member count
			pPrChannel->lMemberCount = pChannel->LGetMemberCount();
	}

	if (!pPrChannel->pChannel && pPrChannel->szTopic && *pPrChannel->szTopic)
	{
		// This is for a channel the user is not in - so she/he might not have access right to the topic
		if (pPrChannel->szRating && g_chEOS != pPrChannel->szRating[0])
		{
			if (!bPassesRatings(pPrChannel->szRating))
			{
				// We don't expose rated content that doesn't pass the acceptance exam ;-)
				if (pPrChannel->szTopic)
				{
					delete [] pPrChannel->szTopic;
					pPrChannel->szTopic = NULL;
				}
			}
		}
		else
		{
			if (!(bCanViewUnrated(CChatSocket::m_hwndParent, TRUE /*bPromptOverride*/)))
			{
				if (pPrChannel->szTopic)
				{
					// We don't expose unrated content
					delete [] pPrChannel->szTopic;
					pPrChannel->szTopic = NULL;
				}
			}
		}
	}

	if (!bCreateChatItems(&pCItems))
		goto exit;

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::ExposeChannelItems");

	bRet = bFillChatItemsFromChannel(pCItems, pPrChannel);

	if (bRemoveFromQueryList)
	{
		// Remove query cell from list
		BOOL bDel = m_listQuery.bRemoveCell((PVOID) pPrQuery);
		ASSERT(bDel, "Couldn't remove query cell in CMsChatPr::ExposeChannelItems");
	}
	else
	{
		if (pPrQuery->szChannelName)
			delete [] pPrQuery->szChannelName;
		delete pPrQuery;
	}

	if (!bRet)
	{
		ReleaseChatItems(pCItems);
		goto exit;
	}

	FireChannelProperty(pChannel, pCItems);
	return;

exit:
	if (FAILED(m_hrLastError))
		if (pChannel)
			FireChannelError(pChannel, NULL, m_hrLastError);
		else
			FireConnectionError(NULL, NULL, m_hrLastError);
}


void CMsChatPr::ExposeChannelItems(PPRCHANNEL pPrChannel)
{
	BOOL		bRet;
	CChatItems	*pCItems  = NULL;

	// OutputDebugThreadIdString("CMsChatPr::ExposeChannelItems - Enter\n");

	ASSERT (pPrChannel, "pPrChannel is NULL in CMsChatPr::ExposeChannelItems");

	// First make sure that the content can be exposed
	if (pPrChannel->szRating && g_chEOS != pPrChannel->szRating[0])
	{
		if (!bPassesRatings(pPrChannel->szRating))
			return; // We don't expose rated content that doesn't pass the acceptance exam ;-)
	}
	else
	{
		if (!m_bCanViewUnrated)
			return;	// We don't expose unrated content
	}

	if (!bCreateChatItems(&pCItems))
		goto exit;

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::ExposeChannelItems");

	bRet = bFillChatItemsFromChannel(pCItems, pPrChannel);

	FreePrChannelContent(pPrChannel);

	if (!bRet)
	{
		ReleaseChatItems(pCItems);
		goto exit;
	}

	FireChannelProperty(NULL, pCItems);
	return;

exit:
	if (FAILED(m_hrLastError))
		FireConnectionError(NULL, NULL, m_hrLastError);
}


void CMsChatPr::ExposeMemberItems(PPRQUERY pPrQuery, BOOL bRemoveFromQueryList)
{
	BOOL				bRet;
	PPRMEMBER			pPrMember = NULL;
	CChatItems			*pCItems  = NULL;
	CChatChannel		*pChannel = NULL;
	enumChannelState	chs = chsOpen;

	
	OutputDebugThreadIdString("CMsChatPr::ExposeMemberItems - Enter\n");

	ASSERT (pPrQuery, "pPrQuery is NULL in CMsChatPr::ExposeMemberItems");

	pPrMember = (PPRMEMBER) pPrQuery->pvData;

	ASSERT(pPrMember, "pPrMember is NULL in CMsChatPr::ExposeMemberItems");
	ASSERT(pPrQuery->szChannelName, "pPrQuery->szChannelName is NULL in CMsChatPr::ExposeMemberItems");

	// Check if this channel is still here and alive
	if (!m_listChannel.bFindCellFromData(1L /*lPosition*/, 
										 g_nSearchByName /*nSearchID1*/,
										 (PVOID) pPrQuery->szChannelName,
										 g_nSearchByState /*nSearchID2*/,
										 (PVOID) &chs,
										 (PVOID*) &pChannel,
										 NULL /*plPositionFound*/))
	{
		OutputDebugThreadIdString("CMsChatPr::ExposeMemberItems - Channel no more valid\n");
		return;
	}

	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::ExposeMemberItems");

	if (!bCreateChatItems(&pCItems))
		goto exit;

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::ExposeMemberItems");

	bRet = bFillChatItemsFromMember(pCItems, pPrMember);

	if (bRemoveFromQueryList)
		// Remove query cell from list
		m_listQuery.bRemoveCell((PVOID) pPrQuery);
	else
	{
		delete [] pPrQuery->szChannelName;
		delete pPrQuery;
	}

	if (!bRet)
	{
		ReleaseChatItems(pCItems);
		goto exit;
	}

	FireMemberProperty(pChannel, pCItems);
	return;

exit:
	if (FAILED(m_hrLastError))
		FireChannelError(pChannel, NULL, m_hrLastError);
}


void CMsChatPr::ExposeUserItems(PPRQUERY pPrQuery, PPRUSER pPrUser)
{
	BOOL		bRet, bRem;
	CChatItems	*pCItems = NULL;
	
	// OutputDebugThreadIdString("CMsChatPr::ExposeUserItems - Enter\n");

	ASSERT(pPrUser, "pPrUser are NULL in CMsChatPr::ExposeUserItems");

	if (!bCreateChatItems(&pCItems))
		goto exit;

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::ExposeUserItems");

	bRet = bFillChatItemsFromUser(pCItems, pPrUser);

	// Remove query cell from list
	if (pPrQuery)
	{
		bRem = m_listQuery.bRemoveCell((PVOID) pPrQuery);
		ASSERT(bRem, "m_listQuery.bRemoveCell failed in CMsChatPr::ExposeUserItems");
	}

	if (!bRet)
	{
		ReleaseChatItems(pCItems);
		goto exit;
	}

	FireUserProperty(pCItems);
	return;

exit:
	if (pPrQuery)
		m_listQuery.bRemoveCell((PVOID) pPrQuery);

	if (FAILED(m_hrLastError))
		FireConnectionError(NULL, NULL, m_hrLastError);
}


void CMsChatPr::ExposeServerItems(void)
{
	CChatItems	*pCItems = NULL;
	
	OutputDebugThreadIdString("CMsChatPr::ExposeServerItems - Enter\n");

	// Create a CChatItems object via a virtual function that in fact creates a new CItemsObject object
	if (!bCreateChatItems(&pCItems))
		goto exit;

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::ExposeServerItems");

	if (!bFillServerItems(pCItems))
	{
		ReleaseChatItems(pCItems);
		goto exit;
	}

	FireServerProperty(pCItems);
	return;

exit:
	if (FAILED(m_hrLastError))
		FireConnectionError(NULL, NULL, m_hrLastError);
}


void CMsChatPr::FireConnectionError(LPCTSTR szChannelName, LPCTSTR szNickname, HRESULT hr)
{
	TCHAR	szError[g_nMaxLengthError];
	VARIANT	vChannel;
	SCODE	sc;

	OutputDebugThreadIdString("CMsChatPr::FireConnectionError - Enter\n");

	if (m_bCtrlDestructed || S_OK == (sc = GetScodeFromHResult(hr)))
		return;

	VariantInit(&vChannel);

	USES_CONVERSION;

	if (szChannelName)
	{
		if (bIsIrcXSocket())
		{
			if (g_chExtChnPfx == *szChannelName)
			{
				// Extended channel name that needs to be converted from UTF8
				LPWSTR wszChannelName;
				if (bConvertUTF8StringToWide(szChannelName, 0 /*cchIn*/, &wszChannelName, NULL /*pcchOut*/, FALSE, TRUE))
				{
					ASSERT(wszChannelName, "wszChannelName in NULL in CMsChatPr::FireConnectionError");

					vChannel.vt = VT_BSTR;
					vChannel.bstrVal = SysAllocString(wszChannelName);
					delete [] wszChannelName;
				}
			}
			else
			{
				LPTSTR	szTmpCN;
				BOOL	bFreeConverted;
				// Might have to convert from Jis to ShiftJis, etc...
				if (bConvertString(TRUE, m_byteCharSet, (LPTSTR) szChannelName, 0, &szTmpCN, NULL, &bFreeConverted))
				{
					ASSERT(szTmpCN, "szTmpCN is NULL in CMsChatPr::FireConnectionError");
					vChannel.vt = VT_BSTR;
					vChannel.bstrVal = A2BSTR(szTmpCN);
					if (bFreeConverted)
						delete [] szTmpCN;
				}
			}
		}
		else
		{
			vChannel.vt = VT_BSTR;
			vChannel.bstrVal = A2BSTR(szChannelName);
		}
	}

	Sc2Sz(sc, szError, szChannelName, szNickname, bIsIrcXSocket(), GetCharSet());

	Fire_OnConnectionError(SCODE_CODE(sc), szError, &vChannel);

	VariantClear(&vChannel);
}


void CMsChatPr::FireChannelError(CChatChannel* pChannel, LPCTSTR szNickname, HRESULT hr)
{
	TCHAR	szError[g_nMaxLengthError];
	SCODE	sc;

	OutputDebugThreadIdString("CMsChatPr::FireChannelError - Enter\n");

	if (m_bCtrlDestructed || S_OK == (sc = GetScodeFromHResult(hr)))
		return;

	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::FireChannelError");

	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();

	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireChannelError");

	Sc2Sz(sc, szError, pcchan->SzGetChannelName(), szNickname, bIsIrcXSocket(), GetCharSet());

	// in run-time mode, fire OnChannelError event
	Fire_OnChannelError(pcchan, SCODE_CODE(sc), szError);
}


void CMsChatPr::FireConnectionState(enumConnectionState csNewConnectionState)
{
	OutputDebugThreadIdString("CMsChatPr::FireConnectionState - Enter\n");

	if (csDisconnected == csNewConnectionState)
	{
		for (SHORT nCnt = 0; nCnt < g_nServerPropertyNames; nCnt++)
			m_rgbServerPropLocal[nCnt] = FALSE;
		
		m_prServer.lChannelCount = -1L;
		m_prServer.lNetInvisibleCount = -1L;
		m_prServer.lNetServerCount = -1L;
		m_prServer.lNetUserCount = -1L;
		m_prServer.lNodeServerCount = -1L;
		m_prServer.lNodeUserCount = -1L;
		m_prServer.lSysopCount = -1L;
		m_prServer.lUnknownConnectionCount = -1L;

		if (m_prServer.psaInfo)
			SafeArrayDestroy(m_prServer.psaInfo);
		m_prServer.psaInfo = NULL;
		
		if (m_prServer.psaIgnored)
			SafeArrayDestroy(m_prServer.psaIgnored);
		m_prServer.psaIgnored = NULL;

		if (0L != m_listQuery.DwGetCellCount())
			OutputDebugThreadIdString("!! Query List Cell Count > 0 in CMsChatPr::FireConnectionState !!");

		BOOL bRet = m_listQuery.bDeleteList();
		ASSERT(bRet, "Query's bDeleteList() failed in CMsChatPr::FireConnectionState");

		m_lPropInProgress = 0L;
		m_lWhoIsInProgress = 0L;
		m_lListInProgress = 0L;
		m_lChannelListingInProgress = 0L;
		m_lMemberListingInProgress = 0L;
		m_lUserChanListingInProgress = 0L;
		m_lWhoIsListingInProgress = 0L;
		m_lWhoListingInProgress = 0L;
		m_lBannedListInProgress = 0L;
		m_lModeIsInProgress = 0L;

		m_szServerName[0] = g_chEOS;	// maybe do more than just that!
	}

	if (csConnected == csNewConnectionState || csDisconnected == csNewConnectionState)
		ResetLoginInfo();

	if (!m_bCtrlDestructed)
		Fire_OnConnectionState(csNewConnectionState);
}


void CMsChatPr::FireChannelState(CChatChannel* pChannel, enumChannelState chsNewChannelState)
{
	OutputDebugThreadIdString("CMsChatPr::FireChannelState - Enter\n");

	if (m_bCtrlDestructed) return;

	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::FireChannelState");

	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();

	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireChannelState");

	Fire_OnChannelState(pcchan, chsNewChannelState);
}


void CMsChatPr::FireBeginEnumeration(CChatChannel* pChannel, enumEnumType et)
{
	OutputDebugThreadIdString("CMsChatPr::FireBeginEnumeration - Enter\n");

	if (m_bCtrlDestructed) return;

	CChannelObject* pcchan = NULL;

	ASSERT(et >= etMessageOfTheDay && et <= etInitialMembers, "Unexpected enumEnumType value in CMsChatPr::FireBeginEnumeration");

	if (pChannel)
	{
		ASSERT(pChannel, "pChannel is NULL in CMsChatPr::FireBeginEnumeration");
		pcchan = (CChannelObject*) pChannel->GetAutObject();
		ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireBeginEnumeration");
	}

	Fire_OnBeginEnumeration(pcchan, et);
}


void CMsChatPr::FireEndEnumeration(CChatChannel* pChannel, enumEnumType et)
{
	OutputDebugThreadIdString("CMsChatPr::FireEndEnumeration - Enter\n");

	if (m_bCtrlDestructed) return;

	CChannelObject* pcchan = NULL;

	ASSERT(et >= etMessageOfTheDay && et <= etInitialMembers, "Unexpected enumEnumType value in CMsChatPr::FireEndEnumeration");

	if (pChannel)
	{
		ASSERT(pChannel, "pChannel is NULL in CMsChatPr::FireEndEnumeration");
		pcchan = (CChannelObject*) pChannel->GetAutObject();
		ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireEndEnumeration");
	}

	Fire_OnEndEnumeration(pcchan, et);
}


void CMsChatPr::FireServerTextMessage(enumServerMessageType smt, LPCTSTR szText)
{
	OutputDebugThreadIdString("CMsChatPr::FireServerTextMessage - Enter\n");

	if (m_bCtrlDestructed) return;

	ASSERT(szText, "szText is NULL in CMsChatPr::FireServerTextMessage");
	ASSERT(smt >= smtNormal && smt <= smtError, "Unexpected server message type in CMsChatPr::FireServerTextMessage");

	// Replace the user's extended nickname by it's Ansi form if needed in the szText
	if (m_szUTF8Nickname && 
		m_bstrNickname   &&
		SzSubStr(szText, m_szUTF8Nickname))
	{
		USES_CONVERSION;
		LPTSTR szConvertedText = SzReplaceStr(szText, m_szUTF8Nickname, W2T(m_bstrNickname));
		if (szConvertedText)
		{
			Fire_OnServerTextMessage(smt, szConvertedText);
			delete [] szConvertedText;
		}
	}
	else
		Fire_OnServerTextMessage(smt, szText);
}


void CMsChatPr::FireAddMember(CChatChannel* pChannel, LPCTSTR szNickname, CChatItems* pCItems)
{
	OutputDebugThreadIdString("CMsChatPr::FireAddMember - Enter\n");

	if (m_bCtrlDestructed) return;

	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::FireAddMember");
	ASSERT(szNickname, "szNickname is NULL in CMsChatPr::FireAddMember");
	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::FireAddMember");

	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();
	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireAddMember");

	CItemsObject* pcitms = (CItemsObject*) pCItems->GetAutObject();
	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::FireAddMember");

	Fire_OnAddMember(pcchan, szNickname, pcitms);
	((IChatItems*) pcitms)->Release();
}


void CMsChatPr::FireDelMember(CChatChannel* pChannel, LPCTSTR szNickname)
{
	OutputDebugThreadIdString("CMsChatPr::FireDelMember - Enter\n");

	if (m_bCtrlDestructed) return;

	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::FireDelMember");

	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();

	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireDelMember");

	Fire_OnDelMember(pcchan, szNickname);
}


BOOL CMsChatPr::bFireDataMessage(CChatChannel* pChannel, LPCTSTR szFromNickname, VARIANT *pvRcpNicks, LPCTSTR szDataMessageTag, LPCTSTR szDataMessage, SHORT nCmd)
{
	CChannelObject	*pcchan = NULL;
	VARIANT			vDataMessage;
	VARIANT			vDataMessageTag;
	BOOL			bDataRaw;

	if (m_bCtrlDestructed) return TRUE;

	OutputDebugThreadIdString("CMsChatPr::bFireDataMessage - Enter\n");

	m_hrLastError = NOERROR;

	if (pChannel)
	{
		pcchan = (CChannelObject*) pChannel->GetAutObject();
		ASSERT(pcchan, "pcchan is NULL in CMsChatPr::bFireDataMessage");
	}

	ASSERT(szFromNickname,			"szFromNickname is NULL in CMsChatPr::bFireDataMessage");
	ASSERT(!pChannel || pvRcpNicks, "pvRcpNicks is NULL in CMsChatPr::bFireDataMessage");
	ASSERT(szDataMessageTag,		"szDataMessageTag is NULL in CMsChatPr::bFireDataMessage");
	ASSERT(szDataMessage,			"szDataMessage is NULL in CMsChatPr::bFireDataMessage");

	VariantInit(&vDataMessage);
	VariantInit(&vDataMessageTag);

	if (!(vDataMessageTag.bstrVal = A2BSTR(szDataMessageTag)))
	{
		m_hrLastError = E_OUTOFMEMORY;
		goto exit;
	}
	vDataMessageTag.vt = VT_BSTR;

	if (!bConstructDataMessageFromString(&vDataMessage, (LPTSTR) szDataMessage, &bDataRaw))
		goto exit;

	if (pChannel)
	{
		LONG msgt = (nCmd == cmdidData) ? msgtData : (nCmd == cmdidRequest) ? msgtDataRequest : msgtDataReply;
		if (bDataRaw) msgt += msgtDataRaw;
		Fire_OnMessage(pcchan, szFromNickname, msgt, &vDataMessage, pvRcpNicks, &vDataMessageTag);
	}
	else
	{
		LONG pmt = (nCmd == cmdidData) ? pmtData : (nCmd == cmdidRequest) ? pmtDataRequest : pmtDataReply;
		if (bDataRaw) pmt += pmtDataRaw;
		Fire_OnPrivateMessage(szFromNickname, pmt, &vDataMessage, &vDataMessageTag);
	}

exit:
	VariantClear(&vDataMessage);
	VariantClear(&vDataMessageTag);

	return SUCCEEDED(m_hrLastError);
}


BOOL CMsChatPr::bFireTextMessage(CChatChannel* pChannel, LPCTSTR szFromNickname, VARIANT *pvRcpNicks, LPCTSTR szTextMessage, LONG msgt, LONG pmt)
{
	CChannelObject	*pcchan = NULL;
	LPTSTR			szMessageBody = NULL;
	BOOL			bComic;
	VARIANT			vRecipientNicknames;
	VARIANT			vTextMessage;
	VARIANT			vDataMessageTag;
	enumMsgType		mt;
	LONG			msgt2 = msgt;
	LONG			pmt2 = pmt;

	OutputDebugThreadIdString("CMsChatPr::bFireTextMessage - Enter\n");

	if (m_bCtrlDestructed) return TRUE;

	if (pChannel)
	{
		pcchan = (CChannelObject*) pChannel->GetAutObject();
		ASSERT(pcchan, "pcchan is NULL in CMsChatPr::bFireTextMessage");
	}

	ASSERT(szFromNickname, "szFromNickname is NULL in CMsChatPr::bFireTextMessage");

	m_hrLastError = NOERROR;
	VariantInit(&vRecipientNicknames);
	VariantInit(&vTextMessage);
	VariantInit(&vDataMessageTag);

	// Might have to unquote \020r and \020n
	bLowLevelUnquoting(g_chLLQuoteCTCP, TRUE /*bTreatAsByteArray*/, szTextMessage, (LPTSTR) szTextMessage);

	if (!bFilterTextMessage((LPTSTR) szTextMessage, pmt, &mt, &bComic, &szMessageBody))
		goto exit;

	switch (mt)
	{
		case mtAppearsAs:		// ComicChat # Appears as message
		case mtGetInfo:			// ComicChat # GetInfo message
		case mtBackground:		// ComicChat # Background message
		case mtChr:				// ComicChat (#<anything>) Chr message
			msgt2 |= msgtInfo;
			break;

		case mtNormal:			// regular broadcast or ComicChat (#<anything>) message, or whisper, or notice
			break;

		case mtThought:			// ComicChat thought
			msgt2 |= msgtThought;
			ASSERT(pChannel, "Unexpected private thought in CMsChatPr::bFireTextMessage"); 
			break;

		case mtAction:			// Regular or ComiChat action. For ComicChat actions,
			msgt2 |= msgtAction;	// the sender is removed to be compatible with regular actions
			pmt2  |= pmtAction;
			break;

		case mtSound:			// CTCP Sound
			msgt2 |= msgtSound;
			pmt2  |= pmtSound;
			break;

		case mtVersion:
			pmt2  |= pmtVersion;
			ASSERT(!pChannel, "Unexpected broadcast version request in CMsChatPr::bFireTextMessage"); 
			break;

		case mtLagTimeRequest:
		case mtLagTimeReply:
			pmt2  |= pmtLagTime;
			ASSERT(!pChannel, "Unexpected broadcast lagtime in CMsChatPr::bFireTextMessage"); 
			break;

		case mtLocalTimeRequest:
		case mtLocalTimeReply:
			pmt2  |= pmtLocalTime;
			ASSERT(!pChannel, "Unexpected broadcast localtime in CMsChatPr::bFireTextMessage"); 
			break;

		case mtCTCP:
			pmt2  |= pmtCTCP;
			msgt2 |= msgtCTCP;	// The trailing 0x01 has been removed
			break;

		default:
			ASSERT(FALSE, "Unexpected mt value in CMsChatPr::bFireTextMessage"); 
	}

	if (bComic)
	{
		TCHAR ch;

		// There is a ComicChat info to pass along
		vTextMessage.vt = VT_BSTR;
		if (szMessageBody)
		{
			ASSERT(szMessageBody > szTextMessage, "szMessageBody > szTextMessage in CMsChatPr::bFireTextMessage");
			ch = *(szMessageBody-1);
			*(szMessageBody-1) = g_chEOS;
		}
		
		vTextMessage.bstrVal = A2BSTR(szTextMessage);
		if (!vTextMessage.bstrVal)
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}

		if (szMessageBody)
			// Rebuild what we destroyed
			*(szMessageBody-1) = ch;

		if (pChannel)
			Fire_OnMessage(pcchan, 
						   szFromNickname,
						   msgt|msgtInfo,
						   &vTextMessage,
						   pvRcpNicks ? pvRcpNicks : &vRecipientNicknames,
						   &vDataMessageTag);
		else
			Fire_OnPrivateMessage(szFromNickname, 
								  pmt|pmtInfo, 
								  &vTextMessage,
								  &vDataMessageTag);

		VariantClear(&vTextMessage);
	}

	if (szMessageBody)
	{
		if (mtAction == mt && bComic)
		{
			// Need to skip the sender's nickname to be compatible with regular actions
			SkipTillChar(&szMessageBody, g_chSpace);
			szMessageBody++;
		}

		if (mtLagTimeRequest == mt)
			vTextMessage.bstrVal = A2BSTR(g_szEmpty);
		else
		{
			LPCTSTR szChannelName = pChannel ? pChannel->SzGetChannelName() : NULL;
			if (szChannelName && g_chExtChnPfx == szChannelName[0] && !pChannel->bMicOnly())
			{
				// Need to convert string from UTF8 to Unicode
				LPWSTR wszMessageBody;
				if (bConvertUTF8StringToWide(szMessageBody, 0, &wszMessageBody, NULL))
				{
					ASSERT(wszMessageBody, "wszMessageBody in NULL in CMsChatPr::bFireTextMessage");

					vTextMessage.bstrVal = SysAllocString(wszMessageBody);
					delete [] wszMessageBody;
				}
			}
			else
			{
				LPTSTR	szTmpMB;
				BOOL	bFreeConverted;
				// Might have to convert from Jis to ShiftJis, etc...
				if (bConvertString(TRUE, m_byteCharSet, szMessageBody, 0, &szTmpMB, NULL, &bFreeConverted))
				{
					ASSERT(szTmpMB, "szTmpMB is NULL in CMsChatPr::bFireTextMessage");
					vTextMessage.bstrVal = A2BSTR(szTmpMB);
					if (bFreeConverted)
						delete [] szTmpMB;
				}
			}
		}

		if (!vTextMessage.bstrVal)
		{
			m_hrLastError = E_OUTOFMEMORY;
			goto exit;
		}
		vTextMessage.vt = VT_BSTR;

		if (mtLagTimeRequest == mt ||
			mtLocalTimeRequest == mt)
		{
			// We need to reply right away
			LPTSTR	szReply = NULL;

			if (!(szReply = new TCHAR[lstrlen(szMessageBody) + 64]))
			{
				m_hrLastError = E_OUTOFMEMORY;
				goto exit;
			}

			if (mtLagTimeRequest == mt)
				// The sender is trying to get the lagtime between us
				// sprintf(szReply, "%c%s %s%c", g_byteCTCPBrace, m_rgszComic[IDS_LAGTIME-IDS_COMICSTART], szMessageBody, g_byteCTCPBrace);
				wsprintf(szReply, "%c%s %s%c", g_byteCTCPBrace, m_rgszComic[IDS_LAGTIME-IDS_COMICSTART], szMessageBody, g_byteCTCPBrace);
			else
			{
				// The sender wants our local time
				TCHAR szDate[50], szTime[50];
				INT	  iRet;
				iRet = GetDateFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, szDate, sizeof(szDate));
				ASSERT(iRet, "GetDateFormat failed in CMsChatPr::bFireTextMessage");
				iRet = GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, szTime, sizeof(szTime));
				ASSERT(iRet, "GetTimeFormat failed in CMsChatPr::bFireTextMessage");

				// sprintf(szReply, "%c%s %s, %s%c", g_byteCTCPBrace, m_rgszComic[IDS_LOCALTIME-IDS_COMICSTART], szDate, szTime, g_byteCTCPBrace);
				wsprintf(szReply, "%c%s %s, %s%c", g_byteCTCPBrace, m_rgszComic[IDS_LOCALTIME-IDS_COMICSTART], szDate, szTime, g_byteCTCPBrace);
			}

			if (!bSendPrivateText(szFromNickname, szReply, TRUE /*bNotice*/))
			{
				delete [] szReply;
				m_hrLastError = HrGetLastError();
				goto exit;
			}
			delete [] szReply;
		}

		if (pChannel)
			Fire_OnMessage(pcchan, 
						   szFromNickname,
						   msgt2,
						   &vTextMessage,
						   pvRcpNicks ? pvRcpNicks : &vRecipientNicknames,
						   &vDataMessageTag);
		else
			Fire_OnPrivateMessage(szFromNickname, 
								  pmt2, 
								  &vTextMessage,
								  &vDataMessageTag);
	}

exit:
	VariantClear(&vTextMessage);

	return SUCCEEDED(m_hrLastError);
}


BOOL CMsChatPr::bFireInvitation(LPCTSTR szChannelName, CChatItems *pCItems)
{
	BSTR bstrChannelName;

	OutputDebugThreadIdString("CMsChatPr::bFireInvitation - Enter\n");

	if (m_bCtrlDestructed) return TRUE;

	ASSERT(szChannelName, "szChannelName is NULL in CMsChatPr::bFireInvitation");

	if (bIsIrcXSocket())
	{
		if (g_chExtChnPfx == *szChannelName)
		{
			// Extended channel name that needs to be converted from UTF8
			LPWSTR wszChannelName;
			if (bConvertUTF8StringToWide(szChannelName, 0 /*cchIn*/, &wszChannelName, NULL /*pcchOut*/, FALSE, TRUE))
			{
				ASSERT(wszChannelName, "wszChannelName in NULL in CMsChatPr::bFireInvitation");

				bstrChannelName = SysAllocString(wszChannelName);
				delete [] wszChannelName;
			}
		}
		else
		{
			LPTSTR	szTmpCN;
			BOOL	bFreeConverted;
			// Might have to convert from Jis to ShiftJis, etc...
			if (bConvertString(TRUE, m_byteCharSet, (LPTSTR) szChannelName, 0, &szTmpCN, NULL, &bFreeConverted))
			{
				ASSERT(szTmpCN, "szTmpCN is NULL in CMsChatPr::bFireInvitation");
				bstrChannelName = A2BSTR(szTmpCN);
				if (bFreeConverted)
					delete [] szTmpCN;
			}
		}
	}
	else
		bstrChannelName = A2BSTR(szChannelName);

	if (!bstrChannelName)
	{
		m_hrLastError = E_OUTOFMEMORY;
		return FALSE;
	}

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::bFireInvitation");
	CItemsObject* pcitms = (CItemsObject*) pCItems->GetAutObject();
	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::bFireInvitation");

	Fire_OnInvitation(bstrChannelName, pcitms);

	((IChatItems*) pcitms)->Release();
	SysFreeString(bstrChannelName);

	return TRUE;
}


void CMsChatPr::FireKnock(CChatChannel* pChannel, CChatItems* pCItems, LONG lErrorCode)
{
	OutputDebugThreadIdString("CMsChatPr::FireKnock - Enter\n");

	if (m_bCtrlDestructed) return;	// REGISB: do we need to do the pcitms->Release() anyway?

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::FireKnock");
	CItemsObject* pcitms = (CItemsObject*) pCItems->GetAutObject();
	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::FireKnock");

	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::FireKnock");
	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();
	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireKnock");

	switch (lErrorCode)
	{
		case ERR_TOOMANYCHANNELS:
			lErrorCode = SCODE_CODE(CPR_E_TOOMANYCHANNELS);
			break;
		case ERR_CHANNELISFULL:
			lErrorCode = SCODE_CODE(CPR_E_CHANNELFULL);
			break;
		case ERR_INVITEONLYCHAN:
			lErrorCode = SCODE_CODE(CPR_E_INVITEONLYCHANNEL);
			break;
		case ERR_BANNEDFROMCHAN:
			lErrorCode = SCODE_CODE(CPR_E_BANNEDFROMCHANNEL);
			break;
		case ERR_BADCHANNELKEY:
			lErrorCode = SCODE_CODE(CPR_E_CHANNELBADKEY);
			break;
		case ERR_NOJOINDYNAMIC:
			lErrorCode = SCODE_CODE(CPR_E_NOJOINDYNAMIC);
			break;
		case ERR_AUTHONLY:
			lErrorCode = SCODE_CODE(CPR_E_AUTHONLY);
			break;

		default:
			ASSERT(FALSE, "Unexpected error code in CMsChatPr::FireKnock");
			lErrorCode = SCODE_CODE(CPR_E_FAILURE);
	}

	Fire_OnKnock(pcchan, pcitms, lErrorCode);
	((IChatItems*) pcitms)->Release();
}


void CMsChatPr::FireServerProperty(CChatItems *pCItems)
{
	OutputDebugThreadIdString("CMsChatPr::FireServerProperty - Enter\n");

	if (m_bCtrlDestructed) return;	// REGISB: do we need to do the pcitms->Release() anyway?

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::FireServerProperty");
	CItemsObject* pcitms = (CItemsObject*) pCItems->GetAutObject();
	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::FireServerProperty");

	Fire_OnServerProperty(pcitms);
	((IChatItems*) pcitms)->Release();
}


BOOL CMsChatPr::bFireChannelPropertyChanged(CChatChannel* pChannel, LPCTSTR szModifierNickname, LPCWSTR wszChannelPropertyName, VARIANT* pvOldChannelProperty, VARIANT* pvNewChannelProperty)
{
	OutputDebugThreadIdString("CMsChatPr::bFireChannelPropertyChanged - Enter\n");

	if (m_bCtrlDestructed) return TRUE;

	ASSERT(wszChannelPropertyName, "wszChannelPropertyName is NULL in CMsChatPr::bFireChannelPropertyChanged");
	ASSERT(pvOldChannelProperty, "pvOldChannelProperty is NULL in CMsChatPr::bFireChannelPropertyChanged");
	ASSERT(pvNewChannelProperty, "pvNewChannelProperty is NULL in CMsChatPr::bFireChannelPropertyChanged");
	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::bFireChannelPropertyChanged");

	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();
	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::bFireChannelPropertyChanged");

	Fire_OnChannelPropertyChanged(pcchan, 
								  szModifierNickname, 
								  wszChannelPropertyName,
								  pvOldChannelProperty,
								  pvNewChannelProperty);
	return TRUE;
}


void CMsChatPr::FireUserProperty(CChatItems* pCItems)
{
	//OutputDebugThreadIdString("CMsChatPr::FireUserProperty - Enter\n");

	if (m_bCtrlDestructed) return;

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::FireUserProperty");
	CItemsObject* pcitms = (CItemsObject*) pCItems->GetAutObject();
	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::FireUserProperty");

	Fire_OnUserProperty(pcitms);
	((IChatItems*) pcitms)->Release();
}


void CMsChatPr::FireChannelProperty(CChatChannel* pChannel, CChatItems* pCItems)
{
	CChannelObject* pcchan = NULL;

	// OutputDebugThreadIdString("CMsChatPr::FireChannelProperty - Enter\n");
	if (m_bCtrlDestructed) return;

	if (pChannel)
	{
		pcchan = (CChannelObject*) pChannel->GetAutObject();
		ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireChannelProperty");
	}

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::FireChannelProperty");
	CItemsObject* pcitms = (CItemsObject*) pCItems->GetAutObject();
	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::FireChannelProperty");

	Fire_OnChannelProperty(pcchan, pcitms);
	((IChatItems*) pcitms)->Release();
}


void CMsChatPr::FireMemberProperty(CChatChannel* pChannel, CChatItems* pCItems)
{
	OutputDebugThreadIdString("CMsChatPr::FireMemberProperty - Enter\n");

	if (m_bCtrlDestructed) return;

	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::FireMemberProperty");
	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();
	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::FireMemberProperty");

	ASSERT(pCItems, "pCItems is NULL in CMsChatPr::FireMemberProperty");
	CItemsObject* pcitms = (CItemsObject*) pCItems->GetAutObject();
	ASSERT(pcitms, "pcitms is NULL in CMsChatPr::FireMemberProperty");

	Fire_OnMemberProperty(pcchan, pcitms);
	((IChatItems*) pcitms)->Release();
}


BOOL CMsChatPr::bFireMemberPropertyChanged(CChatChannel* pChannel, LPCTSTR szModifiedNickname, LPCTSTR szModifierNickname, LPCWSTR wszMemberPropertyName, VARIANT *pvOldMemberProperty, VARIANT *pvNewMemberProperty)
{
	OutputDebugThreadIdString("CMsChatPr::bFireMemberPropertyChanged - Enter\n");

	if (m_bCtrlDestructed) return TRUE;

	ASSERT(wszMemberPropertyName, "wszMemberPropertyName is NULL in CMsChatPr::bFireMemberPropertyChanged");
	ASSERT(pvOldMemberProperty, "pvOldMemberProperty is NULL in CMsChatPr::bFireMemberPropertyChanged");
	ASSERT(pvNewMemberProperty, "pvNewMemberProperty is NULL in CMsChatPr::bFireMemberPropertyChanged");

	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::bFireMemberPropertyChanged");
	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();
	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::bFireMemberPropertyChanged");

	ASSERT(szModifiedNickname, "szModifiedNickname is NULL in CMsChatPr::bFireMemberPropertyChanged");

	Fire_OnMemberPropertyChanged(pcchan, 
								 szModifiedNickname,
								 szModifierNickname,
								 wszMemberPropertyName,
								 pvOldMemberProperty,
								 pvNewMemberProperty);
	return TRUE;
}


BOOL CMsChatPr::bFireMemberKicked(CChatChannel* pChannel, LPCTSTR szKickedNickname, LPCTSTR szKickerNickname, LPCTSTR szReason)
{
	OutputDebugThreadIdString("CMsChatPr::bFireMemberKicked - Enter\n");

	if (m_bCtrlDestructed) return TRUE;

	ASSERT(szKickedNickname, "szKickedNickname is NULL in CMsChatPr::bFireMemberKicked");
	ASSERT(pChannel, "pChannel is NULL in CMsChatPr::bFireMemberKicked");

	CChannelObject* pcchan = (CChannelObject*) pChannel->GetAutObject();
	ASSERT(pcchan, "pcchan is NULL in CMsChatPr::bFireMemberKicked");

	Fire_OnMemberKicked(pcchan, 
						szKickedNickname,
						szKickerNickname,
						szReason);
	return TRUE;
}


BOOL CMsChatPr::bFireUserPropertyChanged(LPCTSTR szModifiedNickname, LPCTSTR szModifierNickname, LPCWSTR wszUserPropertyName, VARIANT *pvOldUserProperty, VARIANT *pvNewUserProperty)
{
	OutputDebugThreadIdString("CMsChatPr::bFireUserPropertyChanged - Enter\n");

	if (m_bCtrlDestructed) return TRUE;

	ASSERT(wszUserPropertyName, "wszUserPropertyName is NULL in CMsChatPr::bFireUserPropertyChanged");
	ASSERT(pvOldUserProperty, "pvOldUserProperty is NULL in CMsChatPr::bFireUserPropertyChanged");
	ASSERT(pvNewUserProperty, "pvNewUserProperty is NULL in CMsChatPr::bFireUserPropertyChanged");
	ASSERT(szModifiedNickname, "szModifiedNickname is NULL in CMsChatPr::bFireUserPropertyChanged");

	Fire_OnUserPropertyChanged(szModifiedNickname,
							   szModifierNickname,
							   wszUserPropertyName,
							   pvOldUserProperty,
							   pvNewUserProperty);
	return TRUE;
}


void CMsChatPr::FireProtocolMessage(PPRIRCMSG pPrIrcMsg, BOOL *pbEnableDefault)
{
	HRESULT			hr = NOERROR;
	VARIANT			vParam;
    SAFEARRAY		*psa = NULL;
	LONG			lIndice;

	if (m_bCtrlDestructed) return;

	// OnProtocolMessage(BSTR MessagePrefix, BSTR MessageCommand, VARIANT MessageParameters)
	ASSERT(pPrIrcMsg, "pPrIrcMsg is NULL in CMsChatPr::FireProtocolMessage");
	ASSERT(pbEnableDefault, "pbEnableDefault is NULL in CMsChatPr::FireProtocolMessage");

	*pbEnableDefault = TRUE;

	if (!(psa = SafeArrayCreateVector(VT_VARIANT, 0L, pPrIrcMsg->byteParams)))
	{
		hr = E_OUTOFMEMORY;
		goto exit;
	}

	VariantInit(&vParam);

	for (lIndice = 0; lIndice < pPrIrcMsg->byteParams; lIndice++)
	{	 
		ASSERT(pPrIrcMsg->szParams[lIndice], "pPrIrcMsg->szParams[lIndice] is NULL in CMsChatPr::FireProtocolMessage");

		if (!(vParam.bstrVal = A2BSTR(pPrIrcMsg->szParams[lIndice])))
		{
			hr = E_OUTOFMEMORY;
			goto exit;
		}
		vParam.vt = VT_BSTR;

		if (FAILED(hr = SafeArrayPutElement(psa, &lIndice, (PVOID) &vParam)))
			goto exit;

		VariantClear(&vParam);
	}

	vParam.vt = VT_VARIANT|VT_ARRAY;
	vParam.parray = psa;

    Fire_OnProtocolMessage(pPrIrcMsg->szPrefix,
						   pPrIrcMsg->szCommand,
						   &vParam,
						   (OLE_ENABLEDEFAULTBOOL*) pbEnableDefault);

exit:
	if (psa)
		SafeArrayDestroy(psa);

	if (FAILED(hr))
		FireConnectionError(NULL, NULL, hr);
}

