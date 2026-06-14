//=--------------------------------------------------------------------------=
// Query.Cpp:		Implementation of rules C++ classes
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 02/09/98

#include "stdafx.h"
#include "query.h"
#include "chat.h"
#include "cdebug.h"
//#include "ccommon.h"

// for ASSERT and FAIL
//
SZTHISFILE

//extern CChatApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CCEvent::CCEvent - Constructor
CCQuery::CCQuery()
{
	m_qp			= qpMax;
	m_ct			= ctMax;
	m_dt			= dtMax;
	m_pvData		= NULL;
	m_pPrUserMatch	= NULL;
}


CCQuery::CCQuery(enumQueryPurpose qp, 
				 enumCommandType ct,
				 enumDataType dt,
				 PVOID pvData,
				 CString strChannelName,
				 CString strNicknameMask,
				 BOOL bCreatePrUserMatch)
{
	m_qp				= qp;
	m_ct				= ct;
	m_dt				= dt;
	m_pvData			= pvData;
	m_strChannelName	= strChannelName;
	m_strNicknameMask	= strNicknameMask;

	if (dtRule == dt)
	{
		ASSERT(pvData, "pvData is NULL in CCQuery::CCQuery");
		CCRule* pRule = (CCRule*) pvData;
		pRule->AddRef();
	}
	else
		if (dtNotif == dt)
		{
			ASSERT(m_pvData, "m_pvData is NULL in CCQuery::CCQuery");
			CCNotif* pNotif = (CCNotif*) m_pvData;
			pNotif->AddRef();
		}

	if (bCreatePrUserMatch)
	{
		ASSERT(!m_strNicknameMask.IsEmpty(), "m_strNicknameMask is empty in CCQuery::CCQuery");
		m_pPrUserMatch = new PRUSERMATCH;
		if (m_pPrUserMatch)
			// since the strNicknameMask is not going to change we don't make a copy to store into pPrUserMatch, 
			// we use the CString string instead.
			bGetUserMatchFromMask((LPTSTR) (LPCTSTR) m_strNicknameMask, m_pPrUserMatch);
	}
	else
		m_pPrUserMatch = NULL;
}


CCQuery::~CCQuery()
{
	if (m_pPrUserMatch)
		// szTheMask is actually the m_strNicknameMask pointer, we don't need to free it
		delete m_pPrUserMatch;

	if (dtRule == m_dt)
	{
		ASSERT(m_pvData, "m_pvData is NULL in CCQuery::~CCQuery");
		CCRule* pRule = (CCRule*) m_pvData;
		pRule->Release();
	}
	else
		if (dtNotif == m_dt)
		{
			ASSERT(m_pvData, "m_pvData is NULL in CCQuery::~CCQuery");
			CCNotif* pNotif = (CCNotif*) m_pvData;
			pNotif->Release();
		}
}


CQueryPtrList::~CQueryPtrList()
{
	FreeRemoveAll();
}


BOOL CQueryPtrList::bAddQuery(CCQuery* pQuery)
{
	ASSERT(pQuery, "pQuery is NULL in CQueryPtrList::bAddQuery");

	return AddTail((PVOID) pQuery) >= 0;
}


void CQueryPtrList::FreeRemoveAll()
{
	POSITION	pos;
	CCQuery*	pQuery;

    for (pos = GetHeadPosition(); pos != NULL; )
    {
		pQuery = (CCQuery*) GetNext(pos);
		delete pQuery;
	}

	CPtrList::RemoveAll();
}


void CQueryPtrList::FreeRemoveAt(POSITION pos)
{
	ASSERT(pos, "pos is NULL in CQueryPtrList::FreeRemoveAt");

	CCQuery*	pQuery = (CCQuery*) GetAt(pos);

	ASSERT(pQuery, "pQuery is NULL in CQueryPtrList::FreeRemoveAt");

	delete pQuery;

	CPtrList::RemoveAt(pos);
}


// Find oldest queued query of type ct
CCQuery* CQueryPtrList::FindQuery(enumCommandType ct, POSITION *pPos, LONG *plRank)
{
	POSITION	pos, posPrev = NULL;
	CCQuery*	pQuery = NULL;
	LONG		lRank = 1L;

	if (pPos)
		*pPos = NULL;

	if (plRank)
		*plRank = 0L;

    for (pos = GetHeadPosition(); pos != NULL; )
    {
		posPrev = pos;
		pQuery = (CCQuery*) GetNext(pos);
		if (pQuery->m_ct == ct)
		{
			if (pPos)
				*pPos = posPrev;
			if (plRank)
				*plRank = lRank;
			return pQuery;
		}
		lRank++;
	}
	return NULL;
}


