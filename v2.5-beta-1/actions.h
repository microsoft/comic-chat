//=--------------------------------------------------------------------------=
// Actions.H
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 01/28/98

#ifndef __ACTIONS_H__

#include "notif.h"
#include "notipage.h"

const TCHAR g_szAllLines[]			= _T("1-999999");
const TCHAR g_szRandomLine[]		= _T("RND");

extern BOOL		bGetNextRange(LPTSTR *pszStr, UINT *puMin, UINT *puMax);
extern BOOL		bInPtrArray(CPtrArray& rgpItems, PVOID pItem);
extern void		TrimQuotes(CString& strIn);
extern CString	GetNextToken(CString& strTokens, CHAR chSep, BOOL bTrim);
extern BOOL		bKeyEventParam(CString& strParam, enumKeyEventParam kep);
extern BOOL		bNetValid(CString strNetParam);
extern BOOL		bRndEventParam(CString& strValue, CString& strFilter, PPRUSERMATCH pPrUserMatch, WORD wFlags, enumParamType pt);
extern CString	StrGetKeyEventParam(enumParamType pt);
extern CString	StrGetKeyActionParam(enumKeyActionParam kap, CString& strEventServer, CString& strEventIdentity, CString& strEventChannel, CString& strEventRecipients, CString& strEventCLMessage);
extern BOOL		bExecuteAction(CCDynaRules* pDynaRules, CCRule* pRule, CCActionContext* pActionCtx);
extern BOOL		bRuleDaemonQuery(CCRule* pRule);
extern BOOL		bNotifDaemonQuery(CCNotif* pNotif);
extern BOOL		bDisplayNotifications(CCDynaNotifs* pDynaNotifs);
extern BOOL		bSignalNewUpdate(CCDynaNotifs* pDynaNotifs);
extern BOOL		bReportRuleFailure(CCRuleSet* pRuleSet, CCRule* pRule, UINT uErrorCode);
extern CNotificationUsers* CreateNotificationBox();
extern void		DestroyNotificationBox();

#define __ACTIONS_H__
#endif __ACTIONS_H__

