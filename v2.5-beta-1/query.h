//=--------------------------------------------------------------------------=
// Query.H
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 02/09/98

#ifndef __QUERY_H__

#include "ccomp.h"

typedef enum
{
	qpBanDlg,
	qpComSetChannelMode,
	qpComSetUserMode,
	qpCreatePics,
	qpGetIdent,
	qpIgnoreIdent,
	qpInitialLUsersMOTD,
	qpInitialMode,
	qpInitialNames,
	qpInitialTopic,
	qpInitialWho,
	qpIrcX,
	qpIsIrcX,
	qpJoinBackUrl,
	qpJoinPics,
	qpKickDlg,
	qpListMembers,
	qpLUsersMOTD,
	qpOnConnectEvent,
	qpOnDisconnectEvent,
	qpOnNewRoomEvent,
	qpOnNotification,
	qpRoomListDlg,
	qpSetClient,
	qpSetInvisible,
	qpSetTopic,
	qpSetVisible,
	qpUserListDlg,
	qpMax
} enumQueryPurpose;


typedef enum
{
	ctGetChannelMode,
	ctIrcX,
	ctList,
	ctListX,
	ctLUsersMOTD,
	ctModeIsIrcX,
	ctNames,
	ctPropGet,
	ctPropSet,
	ctSetChannelMode,
	ctSetUserMode,
	ctTopic,
	ctWho,
	ctWhoIs,
	ctMax
} enumCommandType;


typedef enum
{
	dtFlags,
	dtNotif,
	dtRule,
	dtUser,
	dtMax
} enumDataType;


class CQueryPtrList;


class CCQuery
{
friend class CQueryPtrList;

public:
	CCQuery();
	CCQuery(enumQueryPurpose qp, 
			enumCommandType ct,
			enumDataType dt,
			PVOID pvData,
			CString strChannelName,
			CString strNicknameMask,
			BOOL bCreatePrUserMatch);
	virtual ~CCQuery();

	void				SetQueryPurpose(enumQueryPurpose qp)		{ m_qp = qp; }
	void				SetCommandType(enumCommandType ct)			{ m_ct = ct; }
	void				SetDataType(enumDataType dt)				{ m_dt = dt; }
	void				SetData(PVOID pvData)						{ m_pvData = pvData; }
	void				SetChannelName(CString& strChannelName)		{ m_strChannelName = strChannelName; }
	void				SetNicknameMask(CString& strNicknameMask)	{ m_strNicknameMask = strNicknameMask; }

	enumQueryPurpose	GetQueryPurpose()	{ return m_qp; }
	enumCommandType		GetCommandType()	{ return m_ct; }
	enumDataType		GetDataType()		{ return m_dt; }
	PVOID				GetData()			{ return m_pvData; }
	CString				GetChannelName()	{ return m_strChannelName; }
	CString				GetNicknameMask()	{ return m_strNicknameMask; }
	PPRUSERMATCH		GetPrUserMatch()	{ return m_pPrUserMatch; }

protected:
	enumQueryPurpose	m_qp;
	enumCommandType		m_ct;
	enumDataType		m_dt;
	PVOID				m_pvData;
	CString				m_strChannelName;
	CString				m_strNicknameMask;
	PPRUSERMATCH		m_pPrUserMatch;
};


class CQueryPtrList : public CPtrList
{
public:
	CQueryPtrList() {};
	virtual ~CQueryPtrList();

	BOOL		bAddQuery(CCQuery* pQuery);
	void		FreeRemoveAll();
	void		FreeRemoveAt(POSITION pos);
	CCQuery*	FindQuery(enumCommandType ct, POSITION *pPos = NULL, LONG *plRank = NULL);
};


#define __QUERY_H__
#endif __QUERY_H__

