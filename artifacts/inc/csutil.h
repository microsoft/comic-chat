/*
	*********************************************************************
	*																	*
	*	Module 			: CSUtil.h, ChatSock Utilities header			*
	*																	*
	*	Author 			: RamuM, 8/01/96								*
	*																	*
	*	Current Owner	: RamuM											*
	*																	*
	*	Changes			:												*
	*																	*
	*	To Dos			: 												*
	*																	*
	*********************************************************************
*/


#ifndef __CSUtil_H__
#define __CSUtil_H__

#define STRICT 1
#include <windows.h>
#include "csface.h"

extern RCP RcpParseRoomPath(LPCTSTR szRoomPath, LPTSTR szChatServer, LPTSTR szChatRoom);
extern HRESULT HrGetChatSocket(LPCTSTR cszURL, RCP* prcpSocketType, LPTSTR szServerName, LPTSTR szChannelName, PICS* ppics);
extern HRESULT HrGetChatChannelRating(PICS pics, RCP rcpSocketType, LPCTSTR szChannelName, LPTSTR szPicsLabel);

#endif // __CSUtil_H__