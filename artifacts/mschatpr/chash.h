//--------------------------------------------------------------------------------------------
//
//	Copyright (c) Microsoft Corporation, 1997
//
//	Description:
//
//		A Generic HASH Table.
//		Uses OPEN HASHING.
//
//	Authors:
//
//		Umesh Madan - Modified by Regis Brid
//
//--------------------------------------------------------------------------------------------

#ifndef __CHASH_H__
#define __CHASH_H__
//--------------------------------------------------------------------------------------------
//
// INCLUDES.
//
//--------------------------------------------------------------------------------------------
// #include "csinc.h"

//--------------------------------------------------------------------------------------------
//
// DECLARATIONS
//
//--------------------------------------------------------------------------------------------
class CMemoryBlockChain;
class CHashTableNumber;
class CHashTableString;

typedef CMemoryBlockChain	MEMBLOCK, *PMEMBLOCK;
typedef CHashTableNumber	HASHTABLE_DW, *PHASHTABLE_DW;
typedef CHashTableString	HASHTABLE_SZ, *PHASHTABLE_SZ;

//
// Linked list to do open hashing - where the key is a DWORD
//
typedef struct tagHASHCELL
{
	PVOID			pvContent;	// linked list's data
	DWORD			dwKey;		// the node's key
	tagHASHCELL*	pcellNext;	// the next key
} HASHCELL, *PHASHCELL;


//--------------------------------------------------------------------------------------------
//
// CONSTANTS
//
//--------------------------------------------------------------------------------------------
//
// Allocate memory 1 K at a time
//
const INT	CDEFBLOCK = (1024/sizeof(HASHCELL));

//
// NOTES:
// The final hash index in our hash table is the modulo of the hash value divided by the table
// size. The widest dispersal of modulo results when the hash value and table size are 
// relatively prime - i.e. contain no common factors. The best way to assure this is to make
// the table size PRIME.
// Some prime Numbers: 17,31,37,107
//
const INT	CDEFTABLE	= 107;
//--------------------------------------------------------------------------------------------
//
// CLASSES
//
//--------------------------------------------------------------------------------------------
//
// Manages chains of memory blocks
//
// A class to manages a chain of memory blocks allocated from the SYSTEM. 
// This class can manage a list of VARIED sized blocks and keeps them in a chain.
// To allocate a new block of memory, call PmbCreate.
//
class CMemoryBlockChain
{
//
// Interfaces
//
public:
	CMemoryBlockChain(void);
	~CMemoryBlockChain(void);

			PMEMBLOCK	PmbCreate(DWORD dwcb);
			void		FreeBlockAndChain(void);
			PVOID		PvData(void) 
							{ return this+1; } // the memory block hangs below the class header
//
// Data
//
public:
			PMEMBLOCK	m_pmbNext;	// this allows chaining of memory blocks
};


class CMemoryChain
{
//
// Interfaces
//
public:
	CMemoryChain(DWORD	dwBlockSize);
	~CMemoryChain(void);
	 
			DWORD		DwGetCellCount(void)
							{ return m_dwCellCount; }

			BOOL		bIsEmpty(void)
							{ return (0L == m_dwCellCount); }

			PHASHCELL	PNewCell(void);
					
			void		FreeCell(PHASHCELL pCell);
			void		CleanUp(void);
//
// Data
//
protected:
			MEMBLOCK	m_memBlocks;
			DWORD		m_dwBlockSize;
			PHASHCELL	m_pFreeList;
			DWORD		m_dwCellCount;
};

//
// Hash table base. Routines common to both string and integer hash tables
//
class CHashBase : public CMemoryChain
{
//
// Interfaces
//
public:
	CHashBase(DWORD dwBlockSize = CDEFBLOCK);
	~CHashBase(void);

			BOOL		bDeleteHashTable(void);
			BOOL		bInitHashTable(DWORD dwBucketCount = CDEFTABLE);
			BOOL		bRemoveAll(void);

protected:
			BOOL		bFreeAllData(void);
	virtual BOOL		bDeleteUserData(PVOID pvContent) { return TRUE;} 
//
// Data
//
protected:
			PHASHCELL	*m_pHashTable;
			DWORD		m_dwBucketCount;
};

//
// The number hash table - keys in this table are DWORDs
//
class CHashTableNumber : public CHashBase
{
//
// Interfaces
//
public:
	CHashTableNumber(DWORD dwBlockSize = CDEFBLOCK);
	~CHashTableNumber(void);

			BOOL		bGetDataFromKey(DWORD dwKey, PVOID *pvContent);
			BOOL		bAdd(DWORD dwKey, PVOID pvContent);
			BOOL		bRemove(DWORD dwKey, PVOID *ppvContent = NULL);

protected:
			DWORD		DwHashKey(DWORD dwID)
							{ return (dwID >> 4);	/* This works pretty well for most cases */ }
							
			PHASHCELL	PGetCellAt(DWORD dwKey, DWORD *pdwHash = NULL);			
};

//
// The string hash table - keys are string pointers, cast as DWORDs..
// when doing comparisons, we use lstrcmpi as opposed to ==
// CASE IN-SENSITIVE. 
// this is set up so you can sub group identical keys with a parent key..
// NOTE - this intentionally uses CHAR. All IRC transactions are in CHARs
//
class CHashTableString : public CHashBase
{
//
// Interfaces
//
public:
	CHashTableString(DWORD dwBlockSize = CDEFBLOCK);
	~CHashTableString(void);

			BOOL		bGetDataFromKey(CHAR *szKey, CHAR *szParent, PVOID *pvContent);
			BOOL		bAdd(CHAR *szKey, CHAR *szParent, PVOID pvContent);
			BOOL		bRemove(CHAR *szKey, CHAR *szParent, PVOID *ppvContent = NULL);
	virtual BOOL		bCompareParent(PVOID pv, CHAR *szParent)
							{ return TRUE; }
	
protected:
			DWORD		DwHashKey(CHAR *szKey) const;
			PHASHCELL	PGetCellAt(CHAR *szKey, CHAR *szParent, DWORD *pdwHash = NULL);			
			CHAR		ChUpper(CHAR ch) const
							{
								if (((UCHAR) ch) <= 255)
									return m_szUpper[(UCHAR) ch];
								else
									return ch;
							}

//
// Data
//
protected:
	//
	// Table that speeds conversion of CHARS to upper case
	//
			CHAR		m_szUpper[256];
};


#endif // __CHASH_H__
