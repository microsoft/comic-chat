/*
	*****************************************************************
	*																*
	*	Module 			: MemView.h, Member View for Microsoft Chat	*
	*																*
	*	Author 			: RegisB, 11/4/96							*
	*																*
	*	Current Owner	: RegisB									*
	*																*
	*	Changes			:											*
	*																*
	*	To Dos			:											*
	*																*
	*****************************************************************
*/


#ifndef __MEMVIEW_H__

#include <windows.h>
#include <tchar.h>
#include <commctrl.h>
#include "icons.h"

// The hwnd being passed to Attach must use these constants.
#define MEM_VIEW_WND_FLAGS			(LVS_SHOWSELALWAYS|LVS_SORTASCENDING|LVS_AUTOARRANGE) // Style Flags needed for MemView

typedef BOOL (CALLBACK *PFNLVISEQUAL)(DWORD, DWORD);
typedef BOOL (CALLBACK *PFNLVDELETE)(DWORD);

// CMemView object to handle all chat member list requests.
class CMemView
{

public:
	CMemView();
    ~CMemView();

	// creates the Member View Window
	BOOL	bCreateMemViewWindow(INT iWndID, TCHAR *szName, 
								 DWORD dwExStyle, DWORD dwStyle, 
								 INT x, INT y, INT nWidth, INT nHeight, 
								 HWND hwndParent, HINSTANCE hInstance);
	BOOL	AttachMemViewHWnd(HWND hwnd, HINSTANCE hInstance); 	// Set the hwnd for Member View
	HWND	GetMemViewHWnd() 
				{ return m_hwnd; }	// Get the current hwnd of Member View
	HWND	DetachMemViewHWnd() ;				// Detaches the hwnd associated with this MemView Object

	//To manipulate the current MemView list
	BOOL	bResetMemViewImageList(BOOL bSmallImages);
	BOOL	bClearMemView();		// Empties the Member View list

	BOOL	bGetMemViewItemFromPoint(POINT pt, DWORD* pdwItemData, INT* piItem, INT* piSubItem);
	DWORD	dwGetItemDataFromText(LPCTSTR szText);
	DWORD	dwGetItemDataFromIndex(INT iItem);
	DWORD	dwGetMemViewThisUser()
				{ return m_dwThisUser; }
	INT		iGetIndexFromItemData(DWORD dwItemData);

	BOOL	bGetMemViewItemText(LPTSTR szText, DWORD cbText, DWORD dwItemData, INT iItem, INT iSubItem);

	INT		iGetMemViewCount();		// Returns the number of items in the listview
	INT		iGetMemViewSelectedCount();
	INT		iGetMemViewSelection(DWORD* prgdwItemData, INT* prgiIndex, INT cArraySize);

	BOOL	bSetMemViewIsEqualProc(PFNLVISEQUAL pfnIsEqual)
				{ m_pfnIsEqual = pfnIsEqual; return TRUE; }
	BOOL	bSetMemViewDeleteProc(PFNLVDELETE pfnDelete)
				{ m_pfnDelete = pfnDelete; return TRUE; }
	BOOL	bSetMemViewCompareProc(PFNLVCOMPARE pfnCompare)
				{ m_pfnCompare = pfnCompare; return TRUE; }

	BOOL	bSetMemViewThisUser(DWORD dwThisUser)
				{ m_dwThisUser = dwThisUser; return TRUE; }
	
	BOOL	bSetMemViewMode(DWORD dwDesactivateStyle, DWORD dwActivateStyle);

	BOOL	bSetMemViewDefaultFont(LOGFONT *pLogFont);	// Set the default font for text Member Window
	BOOL	bSetMemViewImageList(BOOL bSmallImages, INT cx, INT cy, INT cImageNum, INT cMaxImages, UINT uFirstResID);
	BOOL	bSetMemViewItemText(LPCTSTR szText, DWORD dwItemData, INT iItem, INT iSubItem);
	BOOL	bSetMemViewItemImage(INT iImage, DWORD dwItemData, INT iItem);
	BOOL	bSetMemViewItemState(DWORD dwItemData, INT iItem, UINT uState);

	INT		iAddMemViewItem(LPCTSTR szAlias, INT iImage, DWORD dwItemData);
	INT		iAddMemViewImage(BOOL bSmallImage, UINT uIconResID, UINT uOverlayIconResID1 = 0, UINT uOverlayIconResID2 = 0);
	INT		iReplaceMemViewImage(INT iImage, BOOL bSmallImage, UINT uIconResID, UINT uOverlayIconResID1 = 0, UINT uOverlayIconResID2 = 0);
	BOOL	bAddMemViewColumn(LPCTSTR szHeader, INT iSubItem, INT nWidth);

	BOOL	bDeleteMemViewItem(DWORD* pdwItemData, INT iItem);

	//INT		iSetMemViewUserImage(BOOL bSmallImage, UINT uIconResID, UINT uIconMeResID);

	BOOL	bFindMemViewItem(DWORD dwItemData, LPCTSTR szText, DWORD* pdwItemDataMatched);

protected:
	HICON			GetIconFromInstance(UINT uResID);
	HICON			MakeIconFromResource(PICONIMAGE pIcon);
	PICONRESOURCE	GetIconDetailsFromInstance(UINT uResID);
	BOOL			bAdjustIconImagePointers(PICONIMAGE pImage);
	BOOL			bTreatIconBits(PICONIMAGE pIcon1, PICONIMAGE pIcon2);
	HICON			HGetIconFromResourceIDs(BOOL bSmallImage, UINT uIconResID, UINT uOverlayIconResID1, UINT uOverlayIconResID2);

private:
	HWND			m_hwnd;				// handle to history window
	HINSTANCE		m_hInstance;		// handle to the current instance, to load resources
	HFONT			m_hFont;			// font used in the text boxes

	BOOL			m_bInternalCreation;// this object created the listview window

	DWORD			m_dwThisUser;		// dwItemData of this user
	PFNLVISEQUAL	m_pfnIsEqual;		// Are two members equal?
	PFNLVDELETE		m_pfnDelete;		// Free a member
	PFNLVCOMPARE	m_pfnCompare;		// Compare two members
};

#define __MEMVIEW_H__
#endif __MEMVIEW_H__
