//=--------------------------------------------------------------------------=
// DosKey.H
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 03/06/98

#ifndef __DOSKEY_H__

#include "format.h"

const UINT g_uDefDosKeySize = 64;

class CDosKey
{
public:
	CDosKey();
	virtual ~CDosKey();

	void			ResetContent();
	void			SetMaxSize(UINT uMaxSize);
	void			SeekToEnd();

	CString			StrGetNextEntry(CDWordArray** pprgdwFormatting);
	CString			StrGetPrevEntry(CDWordArray** pprgdwFormatting);

	BOOL			bAppendEntry(CString strEntry, CDWordArray* prgdwFormatting);

protected:
	CStringArray	m_rgstrEntries;
	CPtrArray		m_rgpFormatting;
	UINT			m_uMaxSize;			// max number of entries in the history
	INT				m_iStartIndex;		// index of first entry
	INT				m_iEndIndex;		// index of next added entry
	INT				m_iCurIndex;		// index of currently pointed entry
	BOOL			m_bEndReached;
};

#define __DOSKEY_H__
#endif __DOSKEY_H__

