/*
	*****************************************************************
	*																*
	*	Module 			: CChan.h, MsChatPr Channel Class object	*
	*																*
	*	Author 			: RegisB, 12/17/96							*
	*																*
	*	Current Owner	: RegisB, ex-CChan.h from RamuM				*
	*																*
	*	Changes			:											*
	*																*
	*	To Dos			: UniCode Support							*
	*																*
	*****************************************************************
*/


#ifndef __CCHAN_H__
#define __CCHAN_H__

// #define STRICT 1
#include <windows.h>
#include <tchar.h>
#include "CCError.H"
#include "CUtil.H"
#include "CCHash.H"
#include "CItems.H"
#include "MsChatPr.H"
#include "PrCnst.H"


class CChatSocket;
class CChanLList;
class CChannelsCollection;

class CChatChannel
{
	friend class CChatSocket;
	friend class CChanLList;
	friend class CChannelsCollection;

public:

	CChatChannel(void);
	~CChatChannel(void);

	void			SetStartedMemberList(BOOL bStartedMemList)
							{ m_bStartedMemList = bStartedMemList; }

	void			SetGotMemberList(BOOL bGotMemList)
							{ m_bGotMemList = bGotMemList; }

	void			SetGotChannelModes(BOOL bGotChannelModes)
							{ m_bGotChannelModes = bGotChannelModes; }

	void			SetIrcXChannel(BOOL bIrcXChannel)
							{ m_bIrcXChannel = bIrcXChannel; }

	void			SetChatSocket(CChatSocket* pccsock)
							{ m_pccsock = pccsock; }

	enumChannelState	ChsGetChannelState(void)
							{ return m_chs; }

	HRESULT			HrGetLastError(void)	
							{ return m_hrLastError; }

	BYTE			GetCharSet(void);

	BOOL			bStartedMemberList(void)
							{ return m_bStartedMemList; }

	BOOL			bGotMemberList(void)
							{ return m_bGotMemList; }

	BOOL			bGotChannelModes(void)
							{ return m_bGotChannelModes; }

	// Channel service connection related
	BOOL			bLeaveChannel(void);

	// Channel Properties
	LPCTSTR			SzGetChannelName(void)	// this channel's name	
							{ return m_prChannel.szName; }

	
	LPCTSTR			SzGetChannelTopic(void)
							{ return m_prChannel.szTopic; }

	BOOL			bSetChannelTopic(LPCTSTR szTopic);

	LONG			LGetChannelModes(void)
							{ return m_prChannel.lModes; }

	BOOL			bMicOnly(void)
							{ return m_prChannel.bMicOnly; }

	void			SetChannelModes(LONG lModes)
							{ m_prChannel.lModes = lModes; }

	void			AddChannelModes(LONG lModes)
							{ m_prChannel.lModes |= lModes; }

	BOOL			bIsIrcXChannel(void) { return (m_bIrcXChannel); }

	LONG			LGetMemberCount(void)
							{ return (LONG) m_hashMember.DwGetCellCount(); }
	
	void			SetMaxMemberCount(LONG lMaxMemberCount = 0L)
							{ m_prChannel.lMaxMemberCount = lMaxMemberCount; }

	LONG			LGetMaxMemberCount(void)
							{ return m_prChannel.lMaxMemberCount; }

	BOOL			bSendMessage(LONG msgt, VARIANT *pvMessage, VARIANT* pvRecipientNicknames, VARIANT* pvTag);
	BOOL			bSendText(LPCTSTR szText, LPCTSTR szRcpNicks = NULL, BOOL bNotice = FALSE, BOOL bWhisper = FALSE);
	BOOL			bKickMember(LPCWSTR wszKickedNickname, LPCTSTR szReason);

	BOOL			bAddMember(LPCTSTR szMemberInfo, PPRMEMBER* ppPrMember);
	BOOL			bAddMember(PPRMEMBER pPrMember);
	BOOL			bDelMember(LPCTSTR szNickname, BOOL bFreeContent = TRUE);
	BOOL			bCheckOpen(void);
	BOOL			bEmptyMemberTable(void);
	BOOL			bGetMemberFromNickname(LPCTSTR szNickname, PPRMEMBER *ppPrMember);
	BOOL			bQueryMemberProperty(LPCTSTR szMemberNickname, enumMemberPropertyName mpnProp, LONG *plNumber, BSTR *pbstrString);
	BOOL			bQueryMemberProperty(BOOL bSyncAccess, LPCWSTR wszMemberPropertyName, LPCTSTR szMemberNickname, CChatItems **ppCItems);
	BOOL			bChangeMemberProperty(LPCWSTR wszMemberPropertyName, VARIANT *pvMemberProperty, LPCTSTR szMemberNickname);
	BOOL			bQueryChannelProperty(BOOL bSyncAccess, LPCWSTR wszChannelPropertyName, CChatItems **ppCItems);
	BOOL			bChangeChannelProperty(LPCWSTR wszChannelPropertyName, VARIANT *pvChannelProperty);
	BOOL			bBanMembers(CChatItems* pCItems, BOOL bSet);
	BOOL			bListMembers(CChatItems *pCItems);

	virtual	PVOID	GetAutObject() = 0;

#ifdef DEBUG
	void			DumpMembers(void)
							{ m_hashMember.DumpMemberList(); }
#endif // DEBUG

private:

// Private Functions
	void			ResetChannelInfo(void);


// Data
protected:
	CChatSocket*		m_pccsock;
	CHashMember			m_hashMember;

	// These Should be set by CCSock when creating the Channel Object.
	BOOL				m_bIrcXChannel;	// If the Channel is IRCX

	HRESULT				m_hrLastError;	// To keep track of last Function Call Error

	BOOL				m_bStartedMemList;
	BOOL				m_bGotMemList;
	BOOL				m_bGotChannelModes;
	BOOL				m_bExitingChannel;	// TRUE if user is currently exiting the channel


	enumChannelState	m_chs;											// current channel state
	PRCHANNEL			m_prChannel;
};


#endif __CCHAN_H__
