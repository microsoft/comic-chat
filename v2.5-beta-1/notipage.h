//=--------------------------------------------------------------------------=
// NotiPage.h : header file
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 03/16/98

#ifndef __NOTIPAGE_H__
#define __NOTIPAGE_H__

#include "chat.h"
#include "resource.h"
#include "notif.h"
#include "setupdlg.h"

/////////////////////////////////////////////////////////////////////////////
// Constants
const TCHAR	g_szAnyReplacement[]	= _T("*");
const UINT	g_uRightMargin			= 6;
const UINT	g_uLeftMargin			= g_uRightMargin;
const UINT	g_uTopMargin			= g_uRightMargin;
const UINT	g_uBottomListMargin		= 4;
const UINT	g_uBottomButtonMargin	= 4;
const UINT	g_uBottomLabelMargin	= 3;
const UINT	g_uInterButton			= 4;

/////////////////////////////////////////////////////////////////////////////
// CNotifsListCtrl control
class CNotifsListCtrl : public CListCtrl
{

// Construction
public:
	CNotifsListCtrl();

// Attributes
	CImageList	m_ilActiveStatus;

// Operations
	void		SetSortSettings(UCHAR uSortColumn, BOOL bSortAscending)
					{ m_uSortColumn = uSortColumn; m_bSortAscending = bSortAscending; }

	BOOL		bFill(CCDynaNotifs* pDynaNotifs);
	BOOL		bAddNotif(CCNotif* pNotif, INT iIndex = -1);
	INT			iGetSelectedNotif(CCNotif **ppNotif);
	INT			iGetSortPosition(CCNotif* pNotif);
	void		SwitchActivation(INT iIndex);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotifsListCtrl)
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CNotifsListCtrl();

	// Generated message map functions
protected:
	//{{AFX_MSG(CNotifsListCtrl)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	UCHAR	m_uSortColumn;
	BOOL	m_bSortAscending;
};


/////////////////////////////////////////////////////////////////////////////
// CNotificationsPage dialog

class CNotificationsPage : public CCSPropertyPage
{
// Construction
public:
	CNotificationsPage();
	~CNotificationsPage();

	void SetDynaNotifs(CCDynaNotifs* pDynaCopy) { m_pDynaCopy = pDynaCopy; }
	void SortNotifs(UCHAR uSortColumn, BOOL bSortAscending);

// Dialog Data
	//{{AFX_DATA(CNotificationsPage)
	enum { IDD = IDD_NOTIFICATIONS };
	CNotifsListCtrl			m_lstNotifs;
	CComboBox				m_cmbOperators[g_uNotifParamNum-1];
	CFilterEdit				m_editArgs[g_uNotifParamNum-1];
	CString					m_strArgs[g_uNotifParamNum-1];
	CString					m_strNetArg;
	CChatServiceComboBox	m_cmbNetArg;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CNotificationsPage)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	static const  DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	void				UpdateButtonsStatus();
	void				FillUpCombos();
	void				FillUpParamsFromIdent(CString& strIdent);

	// Generated message map functions
	//{{AFX_MSG(CNotificationsPage)
	afx_msg void OnAddNotifClick();
	afx_msg void OnModifyNotifClick();
	afx_msg void OnDeleteNotifClick();
	afx_msg void OnEditChange(UINT nID);
	afx_msg void OnComboChange(UINT nID);
	afx_msg void OnNetArgChange();
	afx_msg void OnNotifItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL				m_bNotifsColumnSet;
	BOOL				m_bActivateNew;
	BOOL				m_bFreezeButtons;
	CCDynaNotifs*		m_pDynaCopy;
};


/////////////////////////////////////////////////////////////////////////////
// CNotificationUsers dialog

class CNotificationUsers : public CCSDialog
{
// Construction
public:
	CNotificationUsers(CWnd* pwndParent = NULL);
	~CNotificationUsers();

	void	SetPostCreate(BOOL bNewVal)
				{ m_bPostCreate = bNewVal; }

	BOOL	bFillList(CCItemPtrArray* prgpNotifUsers, UINT uModifiedUsersCount);
	BOOL	bSignalNewEntries();
	BOOL	bSignalNewUpdate();

// Dialog Data
	//{{AFX_DATA(CNotificationUsers)
	enum { IDD = IDD_NOTIFICATIONUSERS };
	CListCtrl	m_lstUsers;
	//}}AFX_DATA

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CNotificationUsers)
	virtual BOOL OnInitDialog();
	virtual void OnCancel();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	BOOL		bUpdateCountLabel();
	BOOL		bUpdateLastUpdateLabel();
	void		RedirectFocus();
	void		SaveNotifCoords();
	void		UpdateButtons();
	INT			iFindUserIndex(CUser* pUser, INT iLastItemsCount);
	CUser*		GetSelectedUser();
	const char*	GetSelectedNickname();

	// Generated message map functions
	//{{AFX_MSG(CNotificationUsers)
	afx_msg void OnDefineNotif();
	afx_msg void OnNotifClear();
	afx_msg void OnNotifInvite();
	afx_msg void OnNotifWhisper();
	afx_msg void OnNotifJoin();
	afx_msg void OnNotifUpdate();
	afx_msg void OnCloseDialog();
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnItemChangedNotifUsers(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMove(int x, int y);
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnClose();
	afx_msg void OnSetFocusUserList(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	
	DECLARE_MESSAGE_MAP()

	BOOL		m_bInverted;
	BOOL		m_bPostCreate;
	BOOL		m_bSortAscending;
	UCHAR		m_uSortColumn;
	CSize		m_sizeDialog;
	CSize		m_sizeMinimal;

	CImageList	m_ImageList;
	CImageList	m_StateIcons;
};


#endif __NOTIPAGE_H__
