//=--------------------------------------------------------------------------=
// CLList.Cpp:		Implementation of custom linked list classes.
//=--------------------------------------------------------------------------=
// Copyright  1996  Microsoft Corporation.  All Rights Reserved.
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//=--------------------------------------------------------------------------=

#include "CCommon.H"
#include "CSFace.H"
#include "CLList.H"
#include "CDebug.H"

// for ASSERT and FAIL
SZTHISFILE


//=--------------------------------------------------------------------------=
// CMsgLList::FDoActionOnCell
//=--------------------------------------------------------------------------=
// executes an action on a cell of the list
//
// Parameters:
//    short			- [in] action ID
//    LPVOID		- [in] cell content
//
// Output:
//    BOOL			TRUE if action was successful
//
// Notes:
//
BOOL CMsgLList::FDoActionOnCell(short nActionID, LPVOID pvCell)
{
	return TRUE;
}


//=--------------------------------------------------------------------------=
// CMsgLList::FIsEqual
//=--------------------------------------------------------------------------=
// checks if two cells are equal
//
// Parameters:
//    LPVOID		- [in] first cell content
//    LPVOID		- [in] second cell content
//
// Output:
//    BOOL			TRUE if cells are identical
//
// Notes:
//
BOOL CMsgLList::FIsEqual(LPVOID pvCell1, LPVOID pvCell2)
{
	return pvCell1 == pvCell2;
}


//=--------------------------------------------------------------------------=
// CMsgLList::FDeleteCell
//=--------------------------------------------------------------------------=
// frees a cell
//
// Parameters:
//    LPVOID		- [in] cell content to free
//
// Output:
//    BOOL			TRUE if successfully released the cell content
//
// Notes:
//
BOOL CMsgLList::FDeleteCell(LPVOID pvCell)
{
	PCS_MSGBASE		pcsMsgBase = (PCS_MSGBASE) pvCell;

	ASSERT(pcsMsgBase, "pcsMsgBase is NULL in CMsgLList::FDeleteCell");

	//#ifdef DEBUG
	//	sprintf(g_szDebugStr, "Freeing message '%ld' in CMsgLList::FDeleteCell\n", (int) pcsMsgBase);
	//	OutputDebugThreadIdString(g_szDebugStr);
	//#endif

	return SUCCEEDED(::HrFreeMsg(pcsMsgBase));
}
