//=--------------------------------------------------------------------------=
// Notif.H
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 03/09/98

#ifndef __NOTIF_H__

#include "rules.h"

const UINT	g_uMaxNetArgLength			= 100;
const UINT	g_uMaxNotifKeyLength		= 20;
const UINT	g_uMaxNotifParamLength		= 32;
const UINT	g_uMaxSerializedNotif		= 12+3*g_uMaxNotifParamLength+g_uMaxNetArgLength;

const TCHAR g_szNotificationsSubKey[]	= _T("\\Notifications");
const TCHAR g_szNotificationFlags[]		= _T("NotificationFlags");
const TCHAR g_szNotificationsClass[]	= _T("Notifications Data");

const UCHAR	g_uNotifParamNum			= 4;
const UCHAR g_uNickname					= 0;
const UCHAR g_uUserName					= 1;
const UCHAR g_uHostName					= 2;
const UCHAR g_uNetName					= 3;

const UCHAR g_uAny						= 0;
const UCHAR g_uEquals					= 1;
const UCHAR g_uContains					= 2;
const UCHAR g_uStartsWith				= 3;
const UCHAR g_uEndsWith					= 4;

const UINT	g_uNotifsDaemonTimer		= 83;
const UINT	g_uNotifsDaemonNoElapse		= 0;		// immediat action!
const UINT	g_uNotifsDaemonShortElapse	= 10;		// 10 seconds
const UINT	g_uNotifsDaemonLongElapse	= 150;		// 2.5 minutes

const WORD	g_wVisible					= 0x0001;
const WORD	g_wConnected				= 0x0002;
const WORD	g_wNew						= 0x0004;
const WORD	g_wAltered					= 0x1000;

class CCNotif;
class CCDynaNotifs;

typedef BOOL	(__cdecl * DISPLAY_NOTIFICATIONS_FN)(CCDynaNotifs*);
typedef BOOL	(__cdecl * SIGNAL_NEW_UPDATE_FN)(CCDynaNotifs*);
typedef BOOL	(__cdecl * NOTIFDAEMON_QUERY_FN)(CCNotif*);
typedef BOOL	(__cdecl * NETVALID_FN)(CString);

class CCNotif
{
friend class CCDynaNotifs;

public:
	CCNotif();
	CCNotif(CCNotif* pNotif);
	virtual ~CCNotif();

	BOOL				operator==(const CCNotif& notif);

	void				AddRef();
	void				Release();
	void				CopyNotif(CCNotif* pNotif);

	void				Activate()					{ m_wFlags |= g_wActive; }
	void				Desactivate()				{ m_wFlags &= ~g_wActive; }
	BOOL				bActive()					{ return m_wFlags & g_wActive; }
	WORD				wGetFlags()					{ return m_wFlags; }
	void				SetFlags(WORD wFlags)		{ m_wFlags = wFlags; }

	BOOL				bDaemonNeeded();
	CCDaemonExt*		GetDaemonExt()				{ return m_pDaemonExt; }
	void				SetDaemonExt(CCDaemonExt* pDaemonExt) { m_pDaemonExt = pDaemonExt; }

	INT					Serialize(LPTSTR szBuff, INT cbBuffLen);
	INT					UnSerialize(LPBYTE pbBuff, INT cbBuffLen);

	BOOL				bValidateNotif(UINT uIndex, CString& strParam, UINT *puErrorIDS);
	BOOL				bUpdateDaemonExt(BOOL bResetUserLists);

	void				SetParam(UCHAR uParam, CString strParam)
													{ m_strParams[uParam] = strParam; }
	void				SetOperator(UCHAR uParam, UCHAR uOperator)
													{ m_uOperators[uParam] = uOperator; }

	CString				GetParam(UCHAR uParam)		{ return m_strParams[uParam]; }
	UCHAR				GetOperator(UCHAR uParam)	{ return m_uOperators[uParam]; }

	static NETVALID_FN	m_pfNetValid;

protected:
	CString				m_strParams[g_uNotifParamNum];
	UCHAR				m_uOperators[g_uNotifParamNum-1];

	SHORT				m_nRefCount;
	WORD				m_wFlags;

	CCDaemonExt*		m_pDaemonExt;
};


class CCDynaNotifs
{
friend class CCDaemonExt;

public:
	CCDynaNotifs();
	virtual ~CCDynaNotifs();

	const CCDynaNotifs&	operator=(const CCDynaNotifs& dynaNotifs);

	CPtrArray&			GetNotifsArray()
							{ return m_rgpNotifs; }

	CCItemPtrArray*		GetNotifUsersArray()
							{ return &m_rgpNotifUsers; }

	void				ResetModifiedUsersCount()
							{ m_uModifiedUsersCount = 0; }

	UINT				GetModifiedUsersCount()
							{ return m_uModifiedUsersCount; }

	WORD				GetFlags()
							{ return m_wFlags; }

	void				SetFlags(WORD wFlags)
							{ m_wFlags = wFlags; }

	void				AddFlag(WORD wFlag)
							{ m_wFlags |= wFlag; }

	CString&			GetStartUpIdent()
							{ return m_strStartUpIdent; }

	void				SetStartUpIdent(CString strStartUpIdent)
							{ m_strStartUpIdent = strStartUpIdent; }

	void				DecrementUpdateCount()
							{ m_uUpdateCount--; }

	UINT				GetUpdateCount()
							{ return m_uUpdateCount; }

	void				DecrementWhosCount()
							{ m_uWhosCount--; }

	UINT				GetWhosCount()
							{ return m_uWhosCount; }

	void				SetDisplayNotificationsFunction(DISPLAY_NOTIFICATIONS_FN pfDisplayNotifications)
							{ m_pfDisplayNotifications = pfDisplayNotifications; }

	void				SetSignalNewUpdateFunction(SIGNAL_NEW_UPDATE_FN pfSignalNewUpdate)
							{ m_pfSignalNewUpdate = pfSignalNewUpdate; }

	void				SetDaemonQueryFunction(NOTIFDAEMON_QUERY_FN pfDaemonQuery)
							{ m_pfDaemonQuery = pfDaemonQuery; }

	CString				StrGetOperatorDisplay(UCHAR uOperator);

	INT					iFindUserIndex(CUser* pUser);
	BOOL				bNotifExists(CCNotif* pNotif);
	BOOL				bAddNotif(CCNotif* pNotif, INT iIndex = -1);
	BOOL				bRemoveNotif(CCNotif* pNotif, INT iIndex = -1);
	BOOL				bSortNotifs();
	BOOL				bRemoveAllUsers();
	BOOL				bRemoveUsersWithoutFlag(WORD wFlag);
	BOOL				bRemoveFlagsFromAllUsers(WORD wFlags);
	BOOL				bUpdateNotifs();

	BOOL				bSaveNotifsToReg();
	BOOL				bLoadNotifsFromReg();

	BOOL				bUpdateNotifsDaemonExt(BOOL bResetUserLists);
	BOOL				bStartNotifsDaemon(UINT uNotifsDaemonElapse, BOOL bForceReset);
	BOOL				bStopNotifsDaemon();
	BOOL				bDaemonNeeded();
	BOOL				bAddNotificationUser(CUser* pUser);
	BOOL				bModifyNotificationUser(CUser* pUser, WORD wAddFlags, WORD wRemoveFlags, INT iIndex = -1);
	void				OnNotifsDaemonTimer();

protected:
	void				CleanUpNotifsArray();

	BOOL				m_bDaemonRunning;
	WORD				m_wFlags;
	UINT				m_uModifiedUsersCount;
	UINT				m_uWhosCount;
	UINT				m_uUpdateCount;
	CPtrArray			m_rgpNotifs;
	CCItemPtrArray		m_rgpNotifUsers;
	CString				m_strStartUpIdent;

	DISPLAY_NOTIFICATIONS_FN	m_pfDisplayNotifications;
	SIGNAL_NEW_UPDATE_FN		m_pfSignalNewUpdate;
	NOTIFDAEMON_QUERY_FN		m_pfDaemonQuery;
};


#ifdef DEBUG
extern SHORT g_nNotificationsRefCount;
#endif

#define __NOTIF_H__
#endif __NOTIF_H__

