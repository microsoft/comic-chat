//=--------------------------------------------------------------------------=
// CLList.H		// Custom Linked List classes
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=
//
// class declarations for the Chat control.
//
#ifndef __CLLIST_H__

#include "LList.H"


const SHORT	g_nSearchNone				= 0;
const SHORT	g_nSearchByName				= 1;
const SHORT g_nSearchByState			= 2;
const SHORT g_nSearchByChatChannel		= 3;
const SHORT g_nSearchByKey				= 4;
const SHORT g_nSearchByCloneName		= 5;
const SHORT g_nSearchByNickname			= 6;
const SHORT g_nSearchByAssociatedType	= 7;
const SHORT	g_nSearchByQueryType		= 8;

const SHORT g_nActionLeave				= 0;
const SHORT g_nActionFillSafeArray		= 1;


//=--------------------------------------------------------------------------=
// Mask list class
//=--------------------------------------------------------------------------=
class CMaskLList : public CLList
{
public:

	CMaskLList(void) {};
	~CMaskLList(void) {};

	virtual BOOL		bIsEqual(PVOID pvCell1, PVOID pvCell2);
	virtual BOOL		bDeleteCell(PVOID pvCell);
};


//=--------------------------------------------------------------------------=
// ChatItem list class
//=--------------------------------------------------------------------------=
class CItemLList : public CLList
{
public:

	CItemLList(void);
	~CItemLList(void);

			BOOL		bDoActionOnList(SHORT nActionID);
			VARIANT&	GetValidItems(void) { 	m_bFreeSafeArray = FALSE;
												return m_vValidItems; }

	virtual BOOL		bDoActionOnCell(SHORT nActionID, PVOID pvCell);
	virtual BOOL		bVerifySearchCriteria(SHORT nSearchID, PVOID pvData, PVOID pvCell);
	virtual BOOL		bIsEqual(PVOID pvCell1, PVOID pvCell2);
	virtual BOOL		bDeleteCell(PVOID pvCell);

private:
	SAFEARRAY	*m_psa;
	BOOL		m_bFreeSafeArray;
	LONG		m_lSafeArrayIndex;
	VARIANT		m_vValidItems;
};


//=--------------------------------------------------------------------------=
// Query list class
//=--------------------------------------------------------------------------=
class CQueryLList : public CLList
{
public:
	CQueryLList(void)  {};
	~CQueryLList(void) {};

	virtual BOOL bVerifySearchCriteria(SHORT nSearchID, PVOID pvData, PVOID pvCell);
	virtual BOOL bIsEqual(PVOID pvCell1, PVOID pvCell2);
	virtual BOOL bDeleteCell(PVOID pvCell);
};


//=--------------------------------------------------------------------------=
// Channel list class
//=--------------------------------------------------------------------------=

class CChanLList : public CLList
{
public:
	CChanLList(void) { m_dwUserData = 0L; }
	~CChanLList(void) {};

	void	SetUserData(DWORD dwUserData) { m_dwUserData = dwUserData; }
	DWORD	GetUserData(void) { return m_dwUserData; }

	virtual BOOL bDoActionOnCell(SHORT nActionID, PVOID pvCell);
	virtual BOOL bVerifySearchCriteria(SHORT nSearchID, PVOID pvData, PVOID pvCell);
	virtual BOOL bIsEqual(PVOID pvCell1, PVOID pvCell2);
	virtual BOOL bDeleteCell(PVOID pvCell);

private:
	DWORD	m_dwUserData;
};

#define __CLLIST_H__
#endif // __CLLIST_H__
