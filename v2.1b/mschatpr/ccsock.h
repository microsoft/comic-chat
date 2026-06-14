/*
	*********************************************************************
	*																	*
	*	Module 			: CCSock.H, Chat Socket Class					*
	*																	*
	*	Author 			: RegisB, 12/03/96								*
	*																	*
	*	Current Owner	: RegisB										*
	*																	*
	*********************************************************************
*/


#ifndef __CCSOCK_H__
#define __CCSOCK_H__

// #define STRICT 1
#include <windows.h>
#include <tchar.h>
#include "Lock.H"
#include "CChan.H"
#include "CLList.H"
#include "ChCol.H"
#include "PrRes.H"
#include "CConn.H"
#include "CIdentD.H"
#include "CComp.H"
#include "CRating.H"


class CChatSocket
{
	// thread to connect and poll the socket messages
	friend DWORD __stdcall DwSocketThreadProcIdentDLoop(PVOID pvData);
	friend DWORD __stdcall DwSocketThreadProcSocketLoop(PVOID pvData);
	friend class CChatChannel;

public:
	CChatSocket(void);
	~CChatSocket(void);

	enumConnectionState	CsGetState(void)
										{ return m_csState; }

	LONG			LGetOpeningChannels(void) 
										{ return m_lOpeningChannels; }

	LONG*			PLPropCount(void)
										{ return &m_lPropInProgress; }

	LONG*			PLListCount(void)
										{ return &m_lListInProgress; }

	LONG*			PLBannedListCount(void)
										{ return &m_lBannedListInProgress; }

	LONG*			PLModeIsCount(void)
										{ return &m_lModeIsInProgress; }

	HRESULT			HrGetLastError(void)
										{ return m_hrLastError; }

	HWND			HwndGetMess(void)
										{ return m_hwndMess; }

	IDispatch*		GetPChannelsCollectionDispatch(void);	// return the channels collection IDispatch interface
	CChanLList*		GetPChanList(void)
										{ return &m_listChannel; }

	CQueryLList*	GetPQueryList(void)
										{ return &m_listQuery; }

	CChatConn*		GetPChatConn(void)	
										{ return &m_cconn; }

	BYTE			GetCharSet(void)
										{ return m_byteCharSet; }

	LPCTSTR			SzGetServerName(void)
										{ return m_szServerName; }
	LPCTSTR			SzGetRealName(void)
										{ return m_szRealName; }
	LPCTSTR			SzGetUserName(void)
										{ return m_szUserName; }
	LPCTSTR			SzGetNickname(void)
										{ return m_szNickname; }
	BSTR			BstrGetNickname(void)
										{ return m_bstrNickname; }
	LPCTSTR			SzGetUserPwd(void)
										{ return m_szUserPwd; }
	LPCTSTR			SzGetSecurity(void)
										{ return m_szSecurity; }

	void			SetMsgWnd(HWND hwndMess)
										{ m_cconn.SetMsgWnd(hwndMess); }

	BOOL			bCheckLogged(void);
	BOOL			bSetNickname(LPCTSTR cszNickname, LPCTSTR cszUTF8Nickname, BSTR bstrNickname);
	BOOL			bSetChannelName(LPCTSTR cszChannelName);
	BOOL			bSetChannelKeyword(LPCTSTR cszChannelKeyword);
	BOOL			bSetChannelHostKey(LPCTSTR cszChannelHostKey);
	BOOL			bSetChannelOwnerKey(LPCTSTR cszChannelOwnerKey);
	BOOL			bSetChannelTopic(LPCTSTR cszChannelTopic);

	BOOL			bIsIrcXSocket(void)	
										{ return m_cconn.bIsIrcXServer(); }
	BOOL			bIsConnected(void)
										{ return SUCCEEDED(m_cconn.HrIsSocketValid()); }
	BOOL			bIsLoggedOn(void) 
										{ return m_cconn.bIsLoggedOn(); }

	BOOL			bConnect(LPTSTR szServerName, BOOL bLoginAlso);
	BOOL			bDisconnect(void);
	BOOL			bCancelConnectLogin(void);
	BOOL			bCloseConnection(BOOL bSync, BOOL bStopPosting = FALSE);

	BOOL			bCreateJoinChannel(CChatChannel* pChannel, LPTSTR szChannelKey, BOOL bCreate);
	BOOL			bFindChannel(SHORT nSearchId1, PVOID pvCriteria1, SHORT nSearchId2, PVOID pvCriteria2, CChatChannel **ppChannel);

	BOOL			bLeftChannel(CChatChannel* pChannel);
	BOOL			bCloseAllChannels(void);
	BOOL			bFatalConnectionError(HRESULT hr);

	BOOL			bSendPrivateMessage(THIS_ LONG pmt, VARIANT *pvMessage, VARIANT *pvRecipientNicknames, VARIANT *pvTag);
	BOOL			bSendPrivateText(LPCTSTR szTarget, LPCTSTR szText, BOOL bNotice = FALSE);
	BOOL			bSendData(LPCTSTR szTarget, LPCTSTR szRcpNicks, LPCTSTR szTag, BYTE *pbData, DWORD dwcb, SHORT nCmd, BOOL bPostProcess);
	BOOL			bSendProtocolText(LPCTSTR szTextMessage);
	BOOL			bConstructDataMessageFromString(VARIANT *pvDataMessage, LPTSTR szDataMessage, BOOL *pbDataRaw);
	BOOL			bConstructDataStream(VARIANT *pvMessage, BYTE **prgbMsg, DWORD *pdwcb, BOOL bDataRaw, BOOL *pbPostProcess);
	BOOL			bConstructRecipientList(VARIANT *pvRecipientNicknames, LPTSTR *pszRcpNicks, LONG *pcRcp);
	BOOL			bConstructRecipientsVariant(LPCTSTR szRcpNicks, VARIANT *pvRcpNicks, LONG *pcRcp);
	BOOL			bSendInvitation(LPCWSTR wszNickname, LPCTSTR szChannelName);
	BOOL			bKillTarget(LPCWSTR wszTarget, LPCTSTR szReason, BOOL bChannel);
	BOOL			bIgnoreUsers(CChatItems *pCItems, BOOL bSet);
	BOOL			bIgnoreUsers(LPTSTR szIdentMask, BOOL bSet);
	BOOL			bBanUsers(CChatItems *pCItems, BOOL bSet, LPCTSTR szReason, LONG lDuration);

	BOOL			bGetSecurityPackages(LPTSTR szSecurity, DWORD cbSecurity, BOOL* pbAnonymousAllowed);

	BOOL			bChangeNickname(LPCWSTR wszNewNickname);
	BOOL			bSetAway(LPCTSTR szAway);

	BOOL			bQueryChannelProperty(BOOL bSyncAccess, LPCWSTR wszChannelPropertyName, LPCTSTR szChannelName, CChatItems **ppCItems);
	BOOL			bChangeUserProperty(LPCWSTR wszUserPropertyName, VARIANT *pvUserProperty, LPCTSTR szUserNickname);
	BOOL			bQueryUserProperty(BOOL bSyncAccess, LPCWSTR wszUserPropertyName, LPCTSTR szUserNickname, CChatItems **ppCItems);
	BOOL			bQueryServerProperty(BOOL bSyncAccess, LPCWSTR wszServerPropertyName, CChatItems **ppCItems);

	BOOL			bLoadComicStrings(HINSTANCE hInst);
	BOOL			bFilterTextMessage(LPTSTR szTextMessage, LONG pmt, enumMsgType *pMT, BOOL *pbComic, LPTSTR *pszMessageBody);


	BOOL			bHandleAddMember(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleAway(PPRIRCMSG pPrIrcMsg, UINT uCode);
	BOOL			bHandleBanList(PPRQUERY pPrQuery, PPRIRCMSG pPrIrcMsg, UINT uCode);
	BOOL			bHandleChannelModeIs(PPRQUERY pPrQuery, PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleClone(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleCreateJoin(PPRIRCMSG pPrIrcMsg, SHORT nCmd);
	BOOL			bHandleDataMsg(PPRIRCMSG pPrIrcMsg, SHORT nCmd);
	BOOL			bHandleEndOfNames(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleErrorMsg(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleInviteMsg(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleKick(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleKnock(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleListMsg(PPRQUERY pPrQuery, PPRIRCMSG pPrIrcMsg, UINT uCode);
	BOOL			bHandleMessageOfTheDay(PPRIRCMSG pPrIrcMsg, UINT uCode);
	BOOL			bHandleModeCommand(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleNameReply(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleNick(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandlePart(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandlePing(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandlePropChanged(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandlePropReply(PPRQUERY pPrQuery, PPRIRCMSG pPrIrcMsg, UINT uCode);
	BOOL			bHandleServerInfo(PPRIRCMSG pPrIrcMsg, UINT uCode, BOOL bAsyncAccess = TRUE);
	BOOL			bHandleServerLUsers(PPRIRCMSG pPrIrcMsg, UINT uCode, BOOL bAsyncAccess = TRUE);
	BOOL			bHandleServerWelcome(PPRIRCMSG pPrIrcMsg, UINT uCode);
	BOOL			bHandleTextMsg(PPRIRCMSG pPrIrcMsg, SHORT nCmd);
	BOOL			bHandleTopic(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleUserHost(PPRQUERY pPrQuery, PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleQuit(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleWhisper(PPRIRCMSG pPrIrcMsg);
	BOOL			bHandleWho(PPRIRCMSG pPrIrcMsg, UINT uCode);
	BOOL			bHandleWhoIs(PPRQUERY pPrQuery, PPRIRCMSG pPrIrcMsg, UINT uCode);

	HRESULT			HrPostNewConnectionState(enumConnectionState csNewState);
	HRESULT			HrPostNewChannelState(CChatChannel* pChannel, enumChannelState chsNewState);
	HRESULT			HrPostEvent(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL bPostToEx = FALSE);
	HRESULT			HrSendEvent(UINT uMsg, WPARAM wParam, LPARAM lParam);

	BOOL			bWaitForProperty(enumAssociatedType at, SHORT nPropIndex, PPRQUERY pPrQuery);
	BOOL			bFillServerItems(CChatItems *pCItems);
	BOOL			bFillChatItemsFromUser(CChatItems *pCItems, PPRUSER pPrUser);
	BOOL			bFillChatItemsFromMember(CChatItems *pCItems, PPRMEMBER pPrMember);
	BOOL			bFillChatItemsFromChannel(CChatItems *pCItems, PPRCHANNEL pPrChannel);
	BOOL			bGetNicknameFromVariant(VARIANT *pvMemberNickname, LPTSTR *pszNickname);
	BOOL			bGetChannelNameFromVariant(VARIANT *pvChannelName, LPTSTR *pszChannelName);
	BOOL			bGetChatItemsFromIdentMask(CChatItems **ppCItems, LPTSTR szIdentMask, enumAssociatedType at);
	BOOL			bGetIdentMaskFromChatItems(CChatItems *pCItems, LPTSTR *pszIdentMask);

	BOOL			bListChannels(CChatItems *pCItems);
	BOOL			bListUsers(CChatItems *pCItems);
	BOOL			bEndOfUser(PPRQUERY pPrQuery, PPRUSER pPrUser, BOOL bUserProp, BOOL bRealEnd = FALSE);

	void			ResetLoginInfo(void);

	virtual BOOL	bCreateChatItems(CChatItems **ppCItems) = 0;
	virtual void	ReleaseChatItems(CChatItems *pcitems) = 0;


private:
	BOOL			bNotifyMemberProperty(CChatChannel *pChannel, LPCTSTR szMemberInfo);
	BOOL			bNotifyUserProperty(LPCTSTR szMemberInfo);
	BOOL			bGetSyncUserHost(PPRUSER pPrUser);
	BOOL			bIsUserIgnored(LPCTSTR szNickname, LPCTSTR szUserName, LPCTSTR szIPAddress, BOOL bCheckIfSysOpOrAdmin, BOOL *pbIgnored);
	BOOL			bBuildIgnoredUsersArray(void);

	HRESULT			HrLogin(void);
	BOOL			bCheckRating(LPCTSTR cszChannelName);
	HRESULT			HrCloseChatSocket(BOOL bCleanUp = TRUE);

	HRESULT			HrDisplayErrorMessage(HRESULT hr);
	HRESULT			HrIterateLogin(PPRSECURITY pPrSecurity, HRESULT hrLatest);
	HRESULT			HrTryLogin(LPCTSTR szSecurityPackage);
	HRESULT			HrWaitForThreadDead(HANDLE *phThread);

	HRESULT			HrThreadProcIdentDLoop(void);
	HRESULT			HrThreadProcSocketLoop(void);

	virtual void	FireAddMember(CChatChannel* pChannel, LPCTSTR szNickname, CChatItems* pCItems) = 0;
	virtual void	FireBeginEnumeration(CChatChannel* pChannel, enumEnumType et) = 0;
	virtual void	FireChannelState(CChatChannel* pChannel, enumChannelState chsNewChannelState) = 0;
	virtual void	FireDelMember(CChatChannel* pChannel, LPCTSTR szNickname) = 0;
	virtual void	FireEndEnumeration(CChatChannel* pChannel, enumEnumType et) = 0;
	virtual void	FireMemberProperty(CChatChannel* pChannel, CChatItems* pCItems) = 0;
	virtual void	FireServerProperty(CChatItems *pCItems) = 0;
	virtual void	FireServerTextMessage(enumServerMessageType smt, LPCTSTR szText) = 0;
	virtual void	FireUserProperty(CChatItems *pCItems) = 0;
	virtual void	FireKnock(CChatChannel* pChannel, CChatItems* pCItems, LONG lErrorCode) = 0;
	virtual BOOL	bFireChannelPropertyChanged(CChatChannel* pChannel, LPCTSTR szModifierNickname, LPCWSTR wszChannelPropertyName, VARIANT *pvOldChannelProperty, VARIANT *pvNewChannelProperty) = 0;
	virtual	BOOL	bFireDataMessage(CChatChannel* pChannel, LPCTSTR szFromNickname, VARIANT *pvRcpNicks, LPCTSTR szDataMessageTag, LPCTSTR szDataMessage, SHORT nCmd) = 0;
	virtual BOOL	bFireTextMessage(CChatChannel* pChannel, LPCTSTR szFromNickname, VARIANT *pvRcpNicks, LPCTSTR szTextMessage, LONG msgt, LONG pmt = pmtNormal) = 0;
	virtual BOOL	bFireInvitation(LPCTSTR szChannelName, CChatItems* pCItems) = 0;
	virtual BOOL	bFireMemberKicked(CChatChannel* pChannel, LPCTSTR szKickedNickname, LPCTSTR szKickerNickname, LPCTSTR szReason) = 0;
	virtual BOOL	bFireMemberPropertyChanged(CChatChannel* pChannel, LPCTSTR szModifiedNickname, LPCTSTR szModifierNickname, LPCWSTR wszMemberPropertyName, VARIANT *pvOldMemberProperty, VARIANT *pvNewMemberProperty) = 0;
	virtual BOOL	bFireUserPropertyChanged(LPCTSTR szModifiedNickname, LPCTSTR szModifierNickname, LPCWSTR wszUserPropertyName, VARIANT *pvOldUserProperty, VARIANT *pvNewUserProperty) = 0;

//
// Data
//
public:
	HWND					m_hwndMess;				// messaging window for client notifications
	HWND					m_hwndMessEx;			// messaging window used during synchronous accesses
	HWND					m_hwndParent;			// parent window for dialog boxes
	TCHAR					m_rgszComic[IDS_COMICEND-IDS_COMICSTART+1][g_nMaxLengthSmall];	// comic strings
	DWORD					m_dwPropertyAccessTimeOut;

protected:
	UINT					m_uPort;			// socket port
	LPTSTR					m_szServerName;		// stores the server name
	LPTSTR					m_szSecurity;		// really needed ?

	LPTSTR					m_szNickname;
	LPTSTR					m_szUTF8Nickname;
	BSTR					m_bstrNickname;

	LPTSTR					m_szUserName;
	LPTSTR					m_szRealName;
	LPTSTR					m_szUserPwd;
	LONG					m_lUserModes;

	BOOL					m_bTryIdentD;
	BOOL					m_bLoginAlso;			// do we want to automatically login after connecting?
	BOOL					m_bCancelOperation;		// TRUE if the user tries to cancel the connection or login operation
	BOOL					m_bFiredUserChanBeginEnum;
	BOOL					m_bCanViewUnrated;

	enumConnectionState		m_csState;

	HRESULT					m_hrLastError;		// To keep track of last Function Call Error

	CMaskLList				m_listMask;			// UserMatch Masks
	CQueryLList				m_listQuery;
	CChanLList				m_listChannel;		// channels list
	CChatConn				m_cconn;			// connection socket object
	CIdentD					m_cIdentD;			// for identd authentication
	CIdentD					m_cIdentDAccept;	// for identd authentication

	HANDLE					m_hMsgThread;
	HANDLE					m_hIdentDThread;

	BOOL					m_rgbServerPropLocal[g_nServerPropertyNames];

	PRSERVER				m_prServer;			// for server info query (INFO, LUSERS)
	PRUSER					m_prUser;			// for user listings
	PRCHANNEL				m_prChannel;		// for channel listings

	LONG					m_lPropInProgress;
	LONG					m_lWhoIsInProgress;
	LONG					m_lListInProgress;
	LONG					m_lChannelListingInProgress;
	LONG					m_lMemberListingInProgress;
	LONG					m_lUserChanListingInProgress;
	LONG					m_lWhoIsListingInProgress;
	LONG					m_lWhoListingInProgress;
	LONG					m_lBannedListInProgress;
	LONG					m_lModeIsInProgress;
	LONG					m_lOpeningChannels;
	LONG					m_lOpenChannels;

	BYTE					m_byteCharSet;
};

#endif // __CCSOCK_H__

