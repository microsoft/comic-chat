//=--------------------------------------------------------------------------=
// Doskey.Cpp:		Implementation of 'doskey' feature
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 03/06/98

#include "stdafx.h"
#include "doskey.h"
#include "cdebug.h"

// for ASSERT and FAIL
//
SZTHISFILE


/////////////////////////////////////////////////////////////////////////////
// CDosKey::CDosKey - Constructor
CDosKey::CDosKey()
{
	m_rgstrEntries.SetSize(0, 8);
	m_rgpFormatting.SetSize(0, 8);
	m_iStartIndex =	-1;
	m_iEndIndex = m_iCurIndex = 0;
	m_uMaxSize = g_uDefDosKeySize;
	m_bEndReached = TRUE;
}


CDosKey::~CDosKey()
{
	ResetContent();
}


void CDosKey::SetMaxSize(UINT uMaxSize)
{
	ResetContent();
	m_uMaxSize = uMaxSize;
}


void CDosKey::ResetContent()
{
	m_rgstrEntries.RemoveAll();

	CDWordArray*	prgdwFormattingTmp;
	INT				iEntries = m_rgpFormatting.GetSize();

	for (INT iIndex = 0; iIndex < iEntries; iIndex++)
	{
		prgdwFormattingTmp = (CDWordArray*) m_rgpFormatting.GetAt(iIndex);
		FreeAndNullFormatting(&prgdwFormattingTmp);
	}

	m_iStartIndex =	-1;
	m_iEndIndex = m_iCurIndex = 0;
	m_bEndReached = TRUE;
}


void CDosKey::SeekToEnd()
{
	m_iCurIndex = m_iEndIndex;
	m_bEndReached = TRUE;
}


CString CDosKey::StrGetNextEntry(CDWordArray** pprgdwFormatting)
{
	ASSERT(pprgdwFormatting, "pprgdwFormatting is NULL in CDosKey::StrGetNextEntry");

	INT	iIndex = (m_iCurIndex + 1) % m_uMaxSize;

	if (m_bEndReached || iIndex == m_iEndIndex)
	{
		m_iCurIndex = m_iEndIndex;
		m_bEndReached = TRUE;
		*pprgdwFormatting = NULL;
		return "";
	}
	else
	{
		ASSERT(iIndex < m_rgstrEntries.GetSize(), "iIndex >= m_rgstrEntries.GetSize() in CDosKey::StrGetNextEntry");
		m_iCurIndex = iIndex;
		m_bEndReached = FALSE;
		*pprgdwFormatting = (CDWordArray*) m_rgpFormatting.GetAt(iIndex);
		return m_rgstrEntries.GetAt(iIndex);
	}
}


CString CDosKey::StrGetPrevEntry(CDWordArray** pprgdwFormatting)
{
	ASSERT(pprgdwFormatting, "pprgdwFormatting is NULL in CDosKey::StrGetPrevEntry");

	if (m_iCurIndex == m_iStartIndex && !m_bEndReached)
	{
		ASSERT(m_iStartIndex >= 0 && m_iStartIndex < m_rgstrEntries.GetSize(), "m_iStartIndex < 0 || m_iStartIndex >= m_rgstrEntries.GetSize() in CDosKey::StrGetPrevEntry");
		ASSERT(m_iStartIndex >= 0 && m_iStartIndex < m_rgpFormatting.GetSize(), "m_iStartIndex < 0 || m_iStartIndex >= m_rgpFormatting.GetSize() in CDosKey::StrGetPrevEntry");
		*pprgdwFormatting = (CDWordArray*) m_rgpFormatting.GetAt(m_iStartIndex);
		return m_rgstrEntries.GetAt(m_iStartIndex);
	}
	else
	{
		if (m_iStartIndex < 0)
		{
			*pprgdwFormatting = NULL;
			return "";
		}

		INT	iIndex = (m_iCurIndex - 1) % m_uMaxSize;

		m_iCurIndex = iIndex;
		m_bEndReached = FALSE;
		ASSERT(iIndex >= 0 && iIndex < m_rgstrEntries.GetSize(), "iIndex < 0 || iIndex >= m_rgstrEntries.GetSize() in CDosKey::StrGetPrevEntry");
		ASSERT(iIndex >= 0 && iIndex < m_rgpFormatting.GetSize(), "iIndex < 0 || iIndex >= m_rgpFormatting.GetSize() in CDosKey::StrGetPrevEntry");
		*pprgdwFormatting = (CDWordArray*) m_rgpFormatting.GetAt(iIndex);
		return m_rgstrEntries.GetAt(iIndex);
	}
}


BOOL CDosKey::bAppendEntry(CString strEntry, CDWordArray* prgdwFormatting)
{
	if (m_rgstrEntries.GetSize() < m_uMaxSize)
	{
		INT iNewEntry = m_rgstrEntries.Add(strEntry);
		ASSERT(m_iEndIndex == iNewEntry, "m_iEndIndex != iNewEntry in CDosKey::bAppendEntry");
		iNewEntry = m_rgpFormatting.Add((PVOID) CopyFormatting(prgdwFormatting));
		ASSERT(m_iEndIndex == iNewEntry, "m_iEndIndex != iNewEntry in CDosKey::bAppendEntry");
		if (m_iStartIndex < 0)
			m_iStartIndex = 0;
	}
	else
	{
		ASSERT(m_rgstrEntries.GetSize() == m_uMaxSize, "m_rgstrEntries.GetSize() != m_uMaxSize in CDosKey::bAppendEntry");
		ASSERT(m_iEndIndex >= 0, "m_iEndIndex < 0 in CDosKey::bAppendEntry");
		ASSERT(m_iEndIndex < m_uMaxSize, "m_iEndIndex >= m_uMaxSize in CDosKey::bAppendEntry");

		m_rgstrEntries.SetAt(m_iEndIndex, strEntry);
		
		CDWordArray* prgdwFormattingTmp = (CDWordArray*) m_rgpFormatting.GetAt(m_iEndIndex);
		FreeAndNullFormatting(&prgdwFormattingTmp);
		
		m_rgpFormatting.SetAt(m_iEndIndex, (PVOID) CopyFormatting(prgdwFormatting));

		m_iStartIndex = (m_iStartIndex + 1) % m_uMaxSize;
	}

	m_iEndIndex = (m_iEndIndex + 1) % m_uMaxSize;

	m_iCurIndex = m_iEndIndex;
	m_bEndReached = TRUE;

	return TRUE;
}

