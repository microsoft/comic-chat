//=--------------------------------------------------------------------------=
// NotiPage.cpp : implementation file
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 03/16/98

#include "stdafx.h"
#include "chatprot.h"
#include "notipage.h"
#include "ccommon.h"
#include "cdebug.h"
#include "protsupp.h"
#include "whisprbx.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "actions.h"
#include "ui.h"
#include <mmsystem.h>

// for ASSERT and FAIL
//
SZTHISFILE

extern CChatApp theApp;

INT CALLBACK CompareNotifs(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CCNotif*	pNotif1 = (CCNotif*) lParam1;
	CCNotif*	pNotif2 = (CCNotif*) lParam2;
	UCHAR		uColumn = (lParamSort & 0xF0) >> 4;
	BOOL		bAscending = lParamSort & 0x0F;
	INT			iRet = 0;

	ASSERT(pNotif1, "pNotif1 is NULL in CompareNotifs");
	ASSERT(pNotif2, "pNotif2 is NULL in CompareNotifs");
	ASSERT(uColumn >= g_uNickname, "Unexpected uColumn value in CompareNotifs");
	ASSERT(uColumn <= g_uNetName, "Unexpected uColumn value in CompareNotifs");

	iRet = pNotif1->GetParam(uColumn).CompareNoCase(pNotif2->GetParam(uColumn));

	return bAscending ? iRet : -iRet;
}


INT CALLBACK CompareUsers(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
{
	CString strPrettyNick1, strPrettyNick2;
	CUser*	pUser1 = (CUser*) lParam1;
	CUser*	pUser2 = (CUser*) lParam2;
	UCHAR	uColumn = (lParamSort & 0xF0) >> 4;
	BOOL	bAscending = lParamSort & 0x0F;
	INT		iRet = 0;

	ASSERT(pUser1, "pUser1 is NULL in CompareUsers");
	ASSERT(pUser2, "pUser2 is NULL in CompareUsers");
	ASSERT(uColumn >= g_uNickname, "Unexpected uColumn value in CompareUsers");
	ASSERT(uColumn <= g_uNetName, "Unexpected uColumn value in CompareUsers");

	strPrettyNick1 = pUser1->GetPrettyNick();
	strPrettyNick2 = pUser2->GetPrettyNick();

	TrimQuotes(strPrettyNick1);
	TrimQuotes(strPrettyNick2);

	switch (uColumn)
	{
	case 0:	// nickname
		iRet = stricmp(strPrettyNick1, strPrettyNick2);
		break;
	case 1:	// identity
		iRet = pUser1->m_strIdentity.CompareNoCase(pUser2->m_strIdentity);
		break;
	case 2:
		iRet = pUser1->m_strFullName.CompareNoCase(pUser2->m_strFullName);
		break;
	case 3:
		iRet = pUser1->m_strPrettyRoom.CompareNoCase(pUser2->m_strPrettyRoom);
		break;
	default:
		ASSERT(FALSE, "Unexpected column in CompareUsers");
	}

	if (!iRet && uColumn != 0)
		iRet = stricmp(strPrettyNick1, strPrettyNick2);  // sort on name if descrs identical

	return bAscending ? iRet : -iRet;
}


/////////////////////////////////////////////////////////////////////////////
// CNotifsListCtrl
CNotifsListCtrl::CNotifsListCtrl()
{
	m_uSortColumn = 0;
	m_bSortAscending = FALSE;

	m_ilActiveStatus.Create(IDB_INACTIVE, 16, 1, RGB(0, 0, 255));
	CBitmap	bmp;
	if (bmp.LoadBitmap(IDB_ACTIVE))
	{
		m_ilActiveStatus.Add(&bmp, RGB(0, 0, 255));
		bmp.DeleteObject();
	}
}


CNotifsListCtrl::~CNotifsListCtrl() 
{
	m_ilActiveStatus.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CNotifsListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CNotifsListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


INT CNotifsListCtrl::iGetSelectedNotif(CCNotif **ppNotif)
{
	INT iIndex = GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);

	if (ppNotif)
		*ppNotif = NULL;

	if (iIndex < 0)
		return iIndex;

	if (ppNotif)
		*ppNotif = (CCNotif*) GetItemData(iIndex);

	return iIndex;
}


INT CNotifsListCtrl::iGetSortPosition(CCNotif* pNotif)
{
	ASSERT(pNotif, "pNotif is NULL in CNotifsListCtrl::iGetSortPosition");
	ASSERT(m_uSortColumn >= 0 && m_uSortColumn <= g_uNotifParamNum, "Unexpected m_uSortColumn in CNotifsListCtrl::iGetSortPosition");

	INT			iOrder, iNotifs = GetItemCount();
	CCNotif*	pNotif2;

	for (INT iItem = 0; iItem < iNotifs; iItem++)
	{
		pNotif2 = (CCNotif*) GetItemData(iItem);
		ASSERT(pNotif2, "pNotif2 is NULL in CNotifsListCtrl::iGetSortPosition");
		iOrder = pNotif->GetParam(m_uSortColumn).CompareNoCase(pNotif2->GetParam(m_uSortColumn));
		if ((iOrder <= 0 && m_bSortAscending) || (iOrder >= 0 && !m_bSortAscending))
			break;
	}
	return iItem;
}


void CNotifsListCtrl::SwitchActivation(INT iIndex)
{
	CCNotif*	pNotif = (CCNotif*) GetItemData(iIndex);
	ASSERT(pNotif, "pNotif is NULL in CNotifsListCtrl::SwitchActivation");
	if (pNotif->bActive())
		pNotif->Desactivate();
	else
		pNotif->Activate();
	RedrawItems(iIndex, iIndex);
	((CNotificationsPage*) GetParent())->SetModified(TRUE);
}


/////////////////////////////////////////////////////////////////////////////
// CNotifsListCtrl message handlers
void CNotifsListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO*	pDispInfo = (LV_DISPINFO*) pNMHDR;
	LV_ITEM*		pItem = &pDispInfo->item;
	ASSERT(pItem, "pItem is NULL in CNotifsListCtrl::OnGetdispinfo");

	CCNotif*		pNotif = (CCNotif*) pItem->lParam;
	ASSERT(pNotif, "pNotif is NULL in CNotifsListCtrl::OnGetdispinfo");

	if (pItem->mask & LVIF_TEXT)
	{
		UINT cchMax = pItem->cchTextMax - 1;

		if (pItem->iSubItem < g_uNotifParamNum-1 && g_uAny == pNotif->GetOperator(pItem->iSubItem))
			strcpy(pItem->pszText, g_szAnyReplacement);
		else
			switch (pItem->iSubItem)
			{
			case 0:
			case 1:
			case 2:
			case 3:
				strncpy(pItem->pszText, (LPCTSTR) pNotif->GetParam(pItem->iSubItem), cchMax);
				break;
			default:
				ASSERT(FALSE, "Unexpected pItem->iSubItem in CNotifsListCtrl::OnGetdispinfo");
			}

		pItem->pszText[cchMax] = g_chEOS;
	}

	if (pItem->mask & LVIF_IMAGE)
		pItem->iImage = (pNotif->bActive() ? 1 : 0);

	*pResult = 0;
}


void CNotifsListCtrl::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*) pNMHDR;
	CNotificationsPage*	np = (CNotificationsPage*) GetParent();

	ASSERT(np, "np is NULL in CNotifsListCtrl::OnColumnClick");

	if (m_uSortColumn == pNMListView->iSubItem)			// toggle sort order
		m_bSortAscending = !m_bSortAscending;
	else
	{													// change sort column
		m_uSortColumn = (UCHAR) pNMListView->iSubItem;
		m_bSortAscending = TRUE;						// sort ascendingly first
	}
	
	SortItems((PFNLVCOMPARE) CompareNotifs, (m_uSortColumn << 4) + m_bSortAscending);

	np->SortNotifs(m_uSortColumn, m_bSortAscending);

	*pResult = 0;
}


void CNotifsListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT	uFlags;
	INT		iIndex = HitTest(point, &uFlags);

	if (iIndex >= 0 && (LVHT_ONITEMICON & uFlags))
		SwitchActivation(iIndex);

	CListCtrl::OnLButtonDown(nFlags, point);
}


void CNotifsListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (g_chSpace == nChar)
	{
		INT	iIndex = GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);
		if (iIndex >= 0)
			SwitchActivation(iIndex);
	}
	CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CNotifsListCtrl::bFill(CCDynaNotifs* pDynaNotifs)
{
	BOOL		bRet = TRUE;
	INT			iNotifs;
	CPtrArray*	pNotifsArray;

	ASSERT(pDynaNotifs, "pDynaNotifs is NULL in CNotifsListCtrl::bFill");

	DeleteAllItems();

	pNotifsArray = &(pDynaNotifs->GetNotifsArray());
	iNotifs = pNotifsArray->GetSize();

	for (INT iIndex = 0; iIndex < iNotifs && bRet; iIndex++)
		bRet = bAddNotif((CCNotif*) pNotifsArray->GetAt(iIndex), iIndex);

	ASSERT(bRet, "bRet is FALSE in CNotifsListCtrl::bFill");

	return bRet;
}


BOOL CNotifsListCtrl::bAddNotif(CCNotif* pNotif, INT iIndex /*=-1*/)
{
	ASSERT(pNotif, "pNotif is NULL in CNotifsListCtrl::bAddNotif");

	LV_ITEM item;
	BOOL	bRet;

	if (iIndex < 0)
		item.iItem = GetItemCount();
	else
		item.iItem = iIndex;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	item.pszText = LPSTR_TEXTCALLBACK;
	item.lParam = (LPARAM) pNotif;
	item.iImage = I_IMAGECALLBACK;
	bRet = (InsertItem(&item) != -1);

	for (INT i = 1; i < g_uNotifParamNum; i++)
	{
		item.iSubItem = i;
		item.pszText = LPSTR_TEXTCALLBACK;
		bRet &= (SetItem(&item) != -1);
	}

	ASSERT(bRet, "bRet is FALSE in CNotifsListCtrl::bAddNotif");

	return bRet;
}




/////////////////////////////////////////////////////////////////////////////
// CNotificationsPage property page

CNotificationsPage::CNotificationsPage() : CCSPropertyPage(CNotificationsPage::IDD)
{
	//{{AFX_DATA_INIT(CNotificationsPage)
	//}}AFX_DATA_INIT
	m_bNotifsColumnSet = FALSE;
	m_bActivateNew = TRUE;
	m_bFreezeButtons = FALSE;
}


CNotificationsPage::~CNotificationsPage()
{
}


void CNotificationsPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotificationsPage)
	if (pDX->m_bSaveAndValidate)
	{
		// Just do validation.
		CString str;
		for (UINT uIndex = 0; uIndex < g_uNotifParamNum-1; uIndex++)
		{
			DDX_Text(pDX, IDC_NICKARG+uIndex, str);
			DDV_MaxChars (pDX, str, g_uMaxNotifParamLength);
		}
		DDX_Text(pDX, IDC_CMBNETARG, str);
		DDV_MaxChars(pDX, str, g_uMaxNetArgLength);
	}
	else
	{
		DDX_Control(pDX, IDC_LSTNOTIFS, m_lstNotifs);
	
		for (UINT uIndex = 0; uIndex < g_uNotifParamNum-1; uIndex++)
			DDX_Control(pDX, IDC_CMBNICKOP+uIndex, m_cmbOperators[uIndex]);
	
		for (uIndex = 0; uIndex < g_uNotifParamNum-1; uIndex++)
		{
			DDX_Control(pDX, IDC_NICKARG+uIndex, m_editArgs[uIndex]);
			DDX_Text(pDX, IDC_NICKARG+uIndex, m_strArgs[uIndex]);
			DDV_MaxChars(pDX, m_strArgs[uIndex], g_uMaxNotifParamLength);
		}
	
		DDX_Text(pDX, IDC_CMBNETARG, m_strNetArg);
		DDV_MaxChars(pDX, m_strNetArg, g_uMaxNetArgLength);
	}
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CNotificationsPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CNotificationsPage)
	ON_BN_CLICKED(IDC_ADDNOTIF, OnAddNotifClick)
	ON_BN_CLICKED(IDC_MODIFYNOTIF, OnModifyNotifClick)
	ON_BN_CLICKED(IDC_DELETENOTIF, OnDeleteNotifClick)
	ON_CONTROL_RANGE(EN_CHANGE, IDC_NICKARG, IDC_HOSTARG, OnEditChange)
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_CMBNICKOP, IDC_CMBHOSTOP, OnComboChange)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LSTNOTIFS, OnNotifItemChanged)
	ON_CBN_EDITCHANGE(IDC_CMBNETARG, OnNetArgChange)
	ON_CBN_SELCHANGE(IDC_CMBNETARG, OnNetArgChange)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CNotificationsPage::UpdateButtonsStatus()
{
	if (m_bFreezeButtons)
		return;

	CString	strNetArg;
	BOOL	bInvalidParam = FALSE, bCanAdd = FALSE, bNotifSelected = m_lstNotifs.GetSelectedCount() > 0;

	for (UCHAR uIndex = g_uNickname; uIndex <= g_uHostName; uIndex++)
	{
		INT iSel = m_cmbOperators[uIndex].GetCurSel();
		if (g_uAny != iSel && CB_ERR != iSel && m_editArgs[uIndex].LineLength() > 0)
			bCanAdd = TRUE;
		if (g_uAny != iSel && CB_ERR != iSel && m_editArgs[uIndex].LineLength() == 0)
			bInvalidParam = TRUE;
		if (CB_ERR == iSel && m_editArgs[uIndex].LineLength() > 0)
			bInvalidParam = TRUE;
	}

	m_cmbNetArg.GetWindowText(strNetArg);
	
	bCanAdd &= !bInvalidParam && (!strNetArg.IsEmpty() || CB_ERR != m_cmbNetArg.GetCurSel());

	GetDlgItem(IDC_ADDNOTIF)->EnableWindow(bCanAdd);
	GetDlgItem(IDC_MODIFYNOTIF)->EnableWindow(bNotifSelected && bCanAdd);
	GetDlgItem(IDC_DELETENOTIF)->EnableWindow(bNotifSelected);
}


void CNotificationsPage::FillUpCombos()
{
	CString		strOperator;

	for (UINT uParam = 0; uParam < g_uNotifParamNum-1; uParam++)
	{
		for (UINT uIndex = g_uAny; uIndex <= g_uEndsWith; uIndex++)
		{
			strOperator.LoadString(IDS_OP_ANY + uIndex);
			ASSERT(!strOperator.IsEmpty(), "strOperator.IsEmpty() in CNotificationsPage::FillUpCombos");
			m_cmbOperators[uParam].AddString(strOperator);
		}
		m_cmbOperators[uParam].SetCurSel(g_uAny);
	}

	m_strNetArg.LoadString(IDS_KEY_EVENT_PARAM0 + (UINT) kepAny);
	ASSERT(!m_strNetArg.IsEmpty(), "m_strNetArg.IsEmpty() in CNotificationsPage::FillUpCombos");
	// first add %Any% keyword 
	m_cmbNetArg.AddString(m_strNetArg);
	// load server list.
	m_cmbNetArg.SetServiceList(&theApp.m_listChatServices);
	m_cmbNetArg.Fill();
	// select %Any% keyword
	m_cmbNetArg.SetCurSel(0);
}


void CNotificationsPage::FillUpParamsFromIdent(CString& strIdent)
{
	// extract user name and host name
	LPCTSTR szIdent = strIdent;
	LPCTSTR szUserNameEnd = OurMbsChr(szIdent, '@');

	CString strUserName, strHostName;

	ASSERT(szUserNameEnd, "szUserNameEnd is NULL in CNotificationsPage::FillUpParamsFromIdent");

	if (szUserNameEnd)
	{
		strUserName = strIdent.Left(szUserNameEnd-szIdent);
		strHostName = strIdent.Mid(szUserNameEnd-szIdent+1);
	}

	m_cmbOperators[g_uUserName].SetCurSel(g_uEquals);
	m_cmbOperators[g_uHostName].SetCurSel(g_uEquals);

	m_editArgs[g_uUserName].EnableWindow(TRUE);
	m_editArgs[g_uHostName].EnableWindow(TRUE);

	m_editArgs[g_uUserName].SetWindowText(strUserName);
	m_editArgs[g_uHostName].SetWindowText(strHostName);

	m_strArgs[g_uUserName] = strUserName;
	m_strArgs[g_uHostName] = strHostName;
}


void CNotificationsPage::SortNotifs(UCHAR uSortColumn, BOOL bSortAscending)
{
	WORD wFlags = m_pDynaCopy->GetFlags();
	wFlags &= 0x0FFF;	// erase current sortcolumn setting
	wFlags |= (uSortColumn << 12);
	if (bSortAscending)
		wFlags &= ~g_wSortDescending;
	else
		wFlags |= g_wSortDescending;
	m_pDynaCopy->SetFlags(wFlags);
	BOOL bRet = m_pDynaCopy->bSortNotifs();
	ASSERT(bRet, "bSortNotifs() failed in CNotificationsPage::SortNotifs");
}


/////////////////////////////////////////////////////////////////////////////
// CNotificationsPage message handlers

BOOL CNotificationsPage::OnSetActive() 
{
	// OutputDebugString("CNotificationsPage::OnSetActive - Enter\n");

	ASSERT(m_pDynaCopy, "m_pDynaCopy is NULL in CNotificationsPage::OnSetActive");

	if (!m_bNotifsColumnSet)
	{
		for (UINT uParam = 0; uParam < g_uNotifParamNum-1; uParam++)
			m_editArgs[uParam].SetFilter(FILTEREDIT_NOCHARS, "!@");

		m_cmbNetArg.ReplaceControl(this, IDC_CMBNETARG);
		m_cmbNetArg.SetFont(&theApp.m_fontGui);

		CString strLabel;
		CString strWidth;

		m_lstNotifs.SetImageList(&m_lstNotifs.m_ilActiveStatus, LVSIL_SMALL);
		m_lstNotifs.SetFont(&theApp.m_fontGui);

		strLabel.LoadString(IDS_NICKARG_LABEL);
		strWidth.LoadString(IDS_NICKARG_WIDTH);
		m_lstNotifs.InsertColumn(0, strLabel, LVCFMT_LEFT, atoi(strWidth));

		strLabel.LoadString(IDS_USERARG_LABEL);
		strWidth.LoadString(IDS_USERARG_WIDTH);
		m_lstNotifs.InsertColumn(1, strLabel, LVCFMT_LEFT, atoi(strWidth), 1);

		strLabel.LoadString(IDS_HOSTARG_LABEL);
		strWidth.LoadString(IDS_HOSTARG_WIDTH);
		m_lstNotifs.InsertColumn(2, strLabel, LVCFMT_LEFT, atoi(strWidth), 1);

		strLabel.LoadString(IDS_NETARG_LABEL);
		strWidth.LoadString(IDS_NETARG_WIDTH);
		m_lstNotifs.InsertColumn(3, strLabel, LVCFMT_LEFT, atoi(strWidth), 1);

		FillUpCombos();

		if (!m_pDynaCopy->GetStartUpIdent().IsEmpty())
		{
			FillUpParamsFromIdent(m_pDynaCopy->GetStartUpIdent());
			m_pDynaCopy->SetStartUpIdent("");
		}

		m_bNotifsColumnSet = TRUE;
	}

	m_lstNotifs.SetSortSettings(m_pDynaCopy->GetFlags() >> 12, !(m_pDynaCopy->GetFlags() & g_wSortDescending));
	m_lstNotifs.bFill(m_pDynaCopy);
	UpdateButtonsStatus();

	return CCSPropertyPage::OnSetActive();
}


void CNotificationsPage::OnOK()
{
	// OutputDebugString("CNotificationsPage::OnOK - Enter\n");

	ASSERT(m_pDynaCopy, "m_pDynaCopy is NULL in CNotificationsPage::OnOK");

	// save the changes made to the notifications...
	theApp.m_dynaNotifs = *m_pDynaCopy;

	// ...and update the notifs daemon
	if (theApp.m_dynaNotifs.bDaemonNeeded())
		theApp.m_dynaNotifs.bStartNotifsDaemon(g_uNotifsDaemonShortElapse, TRUE /*bForceReset*/);
	else
		theApp.m_dynaNotifs.bStopNotifsDaemon();

	theApp.m_dynaNotifs.bUpdateNotifsDaemonExt(FALSE);

	CCSPropertyPage::OnOK();
}


void CNotificationsPage::OnNotifItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;

	if (pNMListView->uNewState == (LVIS_SELECTED|LVIS_FOCUSED))
	{
		CCNotif* pNotif = (CCNotif*) m_lstNotifs.GetItemData(pNMListView->iItem);

		ASSERT(pNotif, "pNotif is NULL in CNotificationsPage::OnNotifItemChanged");

		for (UCHAR uParam = g_uNickname; uParam <= g_uHostName; uParam++)
		{
			UCHAR uOp = pNotif->GetOperator(uParam);
			m_editArgs[uParam].EnableWindow(uOp != g_uAny);
			m_editArgs[uParam].SetWindowText(pNotif->GetParam(uParam));
			m_cmbOperators[uParam].SetCurSel(uOp);
		}
		
		// fill network/server combo
		int iExistingItem = m_cmbNetArg.FindStringExact(-1 /*nIndexStart*/, pNotif->GetParam(g_uNetName));
		
		if (CB_ERR != iExistingItem)
			m_cmbNetArg.SetCurSel(iExistingItem);
		else
		{
			m_cmbNetArg.SetCurSel(-1);
			m_cmbNetArg.SetWindowText(pNotif->GetParam(g_uNetName));
		}
	}
	UpdateButtonsStatus();

	*pResult = 0;
}


void CNotificationsPage::OnEditChange(UINT nID)
{
	UpdateButtonsStatus();
}


void CNotificationsPage::OnNetArgChange() 
{
	UpdateButtonsStatus();
}


void CNotificationsPage::OnComboChange(UINT nID)
{
	CComboBox*	pCmb  = &(m_cmbOperators[nID - IDC_CMBNICKOP]);
	CEdit*		pEdit = &(m_editArgs[nID - IDC_CMBNICKOP]);
	INT			iIndex = pCmb->GetCurSel();

	if (iIndex == CB_ERR)
		return;

	if (iIndex == g_uAny)
		pEdit->SetWindowText("");
	pEdit->EnableWindow(iIndex != g_uAny);

	UpdateButtonsStatus();
}


void CNotificationsPage::OnAddNotifClick()
{
	if (!UpdateData (TRUE))
	{
		// Data not validated!
		return;
	}

	INT			iIndex, iOp;
	CCNotif*	pNotif;
	CString		strParam;

	if (!(pNotif = new CCNotif()))
		return;

	for (UINT uParam = g_uNickname; uParam <= g_uHostName; uParam++)
	{
		m_editArgs[uParam].GetWindowText(strParam);
		if (CB_ERR == (iOp = m_cmbOperators[uParam].GetCurSel()))
			iOp = g_uAny;
		pNotif->SetOperator(uParam, (UCHAR) iOp);
		pNotif->SetParam(uParam, strParam);
	}

	m_cmbNetArg.GetWindowText(strParam);
	pNotif->SetParam(g_uNetName, strParam);

	if (m_pDynaCopy->bNotifExists(pNotif))
	{
		AfxMessageBox(IDS_ERR_NOTIFALREADYEXISTS);
		delete pNotif;
		return;
	}

	if (m_lstNotifs.bAddNotif(pNotif, iIndex = m_lstNotifs.iGetSortPosition(pNotif)))
	{
		if (!m_pDynaCopy->bAddNotif(pNotif, iIndex))
		{
			ASSERT(FALSE, "bAddNotif failed in CNotificationsPage::OnAddNotifClick");
			m_lstNotifs.DeleteItem(iIndex);
			delete pNotif;
		}
		else
		{
			if (m_bActivateNew)
				pNotif->Activate();				// New notification is born activated
			m_lstNotifs.SetItemState(iIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	// Select that new notification
			m_lstNotifs.EnsureVisible(iIndex, FALSE);
			SetModified(TRUE);
		}
	}
	else
	{
		ASSERT(FALSE, "bAddNotif failed in CNotificationsPage::OnAddNotifClick");
		delete pNotif;
	}
}


void CNotificationsPage::OnModifyNotifClick()
{
	if (!UpdateData (TRUE))
	{
		// Data not validated!
		return;
	}

	ASSERT(m_lstNotifs.GetSelectedCount() == 1, "m_lstNotifs.GetSelectedCount() != 1 in CNotificationsPage::OnModifyNotifClick");

	CCNotif*	pNotif;
	INT			iIndex = m_lstNotifs.iGetSelectedNotif(&pNotif);

	ASSERT(iIndex >= 0, "iIndex < 0 in CNotificationsPage::OnModifyNotifClick()");
	ASSERT(pNotif, "pNotif is NULL in CNotificationsPage::OnModifyNotifClick()");

	m_bActivateNew = pNotif->bActive();
	m_bFreezeButtons = TRUE;
	m_lstNotifs.DeleteItem(iIndex);			// Remove notif from the list control
	m_pDynaCopy->bRemoveNotif(NULL, iIndex);
	m_bFreezeButtons = FALSE;
 	OnAddNotifClick();
	m_bActivateNew = TRUE;
}


void CNotificationsPage::OnDeleteNotifClick()
{
	ASSERT(m_lstNotifs.GetSelectedCount() == 1, "m_lstNotifs.GetSelectedCount() != 1 in CNotificationsPage::OnDeleteNotifClick");

	CCNotif*	pNotif;
	INT			iIndex = m_lstNotifs.iGetSelectedNotif(&pNotif);

	ASSERT(iIndex >= 0, "iIndex < 0 in CNotificationsPage::OnDeleteNotifClick()");

	m_lstNotifs.DeleteItem(iIndex);			// Remove notif from the list control

	m_pDynaCopy->bRemoveNotif(NULL, iIndex);

	if (m_lstNotifs.GetItemCount() > 0)
		m_lstNotifs.SetItemState(iIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	else
		GotoDlgCtrl(GetDlgItem(IDC_ADDNOTIF));

	SetModified(TRUE);
}


const DWORD CNotificationsPage::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CNotificationUsers dialog

CNotificationUsers::CNotificationUsers(CWnd* pwndParent /*=NULL*/)
	: CCSDialog(CNotificationUsers::IDD, pwndParent)
{
	//{{AFX_DATA_INIT(CNotificationUsers)
	//}}AFX_DATA_INIT
	m_bInverted = FALSE;
	m_bPostCreate = FALSE;		// only store dimension/position info after create

	m_bSortAscending = FALSE;
	m_uSortColumn = 0;

	m_ImageList.Create(IDB_CONNECT, 16, 2, RGB(0, 0, 255));
	m_StateIcons.Create(IDB_OLDNEW, 16, 1, RGB(0, 0, 255));

	m_sizeDialog.cx = m_sizeDialog.cy = 0;
	m_sizeMinimal.cx = m_sizeMinimal.cy = 0;
}


CNotificationUsers::~CNotificationUsers()
{
	m_ImageList.DeleteImageList();
	m_StateIcons.DeleteImageList();
}


BEGIN_MESSAGE_MAP(CNotificationUsers, CCSDialog)
	//{{AFX_MSG_MAP(CNotificationUsers)
	ON_NOTIFY(LVN_GETDISPINFO, IDC_LSTNOTIFICATIONUSERS, OnGetdispinfo)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LSTNOTIFICATIONUSERS, OnItemChangedNotifUsers)
	ON_NOTIFY(LVN_COLUMNCLICK, IDC_LSTNOTIFICATIONUSERS, OnColumnClick)
	ON_BN_CLICKED(IDC_DEFINENOTIF, OnDefineNotif)
	ON_BN_CLICKED(IDC_NOTIFCLEAR, OnNotifClear)
	ON_BN_CLICKED(IDC_NOTIFINVITE, OnNotifInvite)
	ON_BN_CLICKED(IDC_NOTIFWHISPER, OnNotifWhisper)
	ON_BN_CLICKED(IDC_NOTIFJOIN, OnNotifJoin)
	ON_BN_CLICKED(IDC_NOTIFUPDATE, OnNotifUpdate)
	ON_BN_CLICKED(IDC_CLOSE_NOTIFUSERS, OnCloseDialog)
	ON_WM_SIZE()
	ON_WM_MOVE()
	ON_WM_CLOSE()
	ON_WM_SHOWWINDOW()
	ON_WM_ACTIVATE()
	ON_WM_GETMINMAXINFO()
	ON_NOTIFY(NM_SETFOCUS, IDC_LSTNOTIFICATIONUSERS, OnSetFocusUserList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CNotificationUsers::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CNotificationUsers)
	DDX_Control(pDX, IDC_LSTNOTIFICATIONUSERS, m_lstUsers);
	//}}AFX_DATA_MAP
}


BOOL CNotificationUsers::PreTranslateMessage(MSG* pMsg)
{
	// Trap the F5 key, for an instant refresh.
	if ((pMsg->message == WM_KEYDOWN || pMsg->message == WM_KEYUP) && pMsg->wParam == VK_F5)
	{
		if (pMsg->message == WM_KEYDOWN && LOWORD(pMsg->lParam) == 1) // Don't do it on repeat keystrokes
			PostMessage(WM_COMMAND, (WPARAM) MAKELONG(IDC_NOTIFUPDATE, BN_CLICKED), (LPARAM) GetDlgItem(IDC_NOTIFUPDATE)->m_hWnd);
		return TRUE;
	}
	else
		return CCSDialog::PreTranslateMessage (pMsg);
}


void CNotificationUsers::SaveNotifCoords()
{
	if (m_bPostCreate && !IsIconic())
		GetWindowRect(&theApp.m_rectNotifs);
}


void CNotificationUsers::RedirectFocus()
{
	if (theApp.m_pWndActiveDialog)
	{
		theApp.m_pWndActiveDialog->SetActiveWindow();
		return;
	}

	if (GetChatDoc())
		GetChatDoc()->SetFocusToSayWnd();
}


INT CNotificationUsers::iFindUserIndex(CUser* pUser, INT iLastItemsCount)
{
	ASSERT(pUser, "pUser is NULL in CNotificationUsers::iFindUserIndex");
	ASSERT(iLastItemsCount > 0, "iLastItemsCount <= 0 in CNotificationUsers::iFindUserIndex");

	CUser*	pUserTmp;
	INT		iIndex, iUserCount = m_lstUsers.GetItemCount();

	ASSERT(iUserCount - iLastItemsCount >= 0, "iUserCount - iLastItemsCount < 0 in CNotificationUsers::iFindUserIndex");

	for (iIndex = iUserCount - iLastItemsCount; iIndex < iUserCount; iIndex++)
	{
		pUserTmp = (CUser*) m_lstUsers.GetItemData(iIndex);
		ASSERT(pUserTmp, "pUserTmp is NULL in CNotificationUsers::iFindUserIndex");
		if (pUserTmp == pUser)
			return iIndex;
	}

	return -1;
}


BOOL CNotificationUsers::bFillList(CCItemPtrArray* prgpNotifUsers, UINT uModifiedUsersCount)
{
	ASSERT(prgpNotifUsers, "prgpNotifUsers is NULL in CNotificationUsers::bFillList");

	INT		iIndexModif, iIndexFound, iNotifUsers = prgpNotifUsers->GetSize(), iInitialItemCount = m_lstUsers.GetItemCount();
	LV_ITEM item;
	BOOL	bRet = TRUE;

	ASSERT(iNotifUsers >= uModifiedUsersCount, "iNotifUsers < uModifiedUsersCount in CNotificationUsers::bFillList");

	for (iIndexModif = iNotifUsers - uModifiedUsersCount; iIndexModif < iNotifUsers; iIndexModif++)
	{
		CUser*	pUser = (CUser*) prgpNotifUsers->GetAt(iIndexModif);
		ASSERT(pUser, "pUser is NULL in CNotificationUsers::bFillList");
		ASSERT(pUser->GetFlags() & g_wVisible, "!(pUser->GetFlags() & g_wVisible) in CNotificationUsers::bFillList");
		ASSERT(pUser->GetFlags() & g_wNew, "!(pUser->GetFlags() & g_wNew) in CNotificationUsers::bFillList");
		ASSERT(pUser->GetFlags() & g_wAltered, "!(pUser->GetFlags() & g_wAltered) in CNotificationUsers::bFillList");

		WORD	wFlags = pUser->GetFlags();

		// remove the g_wAltered flag
		pUser->SetFlags(wFlags & ~g_wAltered);

		// if user is already present in the list, he's moved to the top
		if (iInitialItemCount > 0)
		{
			iIndexFound = iFindUserIndex(pUser, iInitialItemCount);
			if (iIndexFound >= 0)
			{
				m_lstUsers.DeleteItem(iIndexFound);
				iInitialItemCount--;
				ASSERT(iInitialItemCount >= 0, "iInitialItemCount < 0 in CNotificationUsers::bFillList"); 
			}
		}

		item.iItem		= 0;		// always add on top of list
		item.iSubItem	= 0;
		item.mask		= LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE | LVIF_STATE;
		item.pszText	= LPSTR_TEXTCALLBACK;
		item.iImage		= I_IMAGECALLBACK;
		item.stateMask	= LVIS_STATEIMAGEMASK;
		item.state		= INDEXTOSTATEIMAGEMASK(1);	// show the "new" state icon
		item.lParam		= (LPARAM) pUser;
		bRet = (m_lstUsers.InsertItem(&item) != -1);

		item.mask = LVIF_TEXT;
		for (INT i = 1; i < 5; i++)
		{
			item.iSubItem = i;
			item.pszText = LPSTR_TEXTCALLBACK;
			bRet &= (m_lstUsers.SetItem(&item) != -1);
		}

		ASSERT(bRet, "bRet is FALSE in CNotificationUsers::bFillList");
	}

	GetDlgItem(IDC_NOTIFCLEAR)->EnableWindow(m_lstUsers.GetItemCount() > 0);

	bUpdateCountLabel();
	bUpdateLastUpdateLabel();

	return bRet;
}


BOOL CNotificationUsers::bUpdateCountLabel()
{
	CString strCount;
	CWnd*	pWnd = GetDlgItem(IDC_NOTIFCOUNT);

	if (pWnd)
	{
		strCount.Format(IDS_NUM_USERS, m_lstUsers.GetItemCount());
		pWnd->SetWindowText(strCount);
		return TRUE;
	}
	return FALSE;
}


BOOL CNotificationUsers::bUpdateLastUpdateLabel()
{
	CHAR	szTimeBuff[64];
	CString strLastUpdate;
	CWnd*	pWnd = GetDlgItem(IDC_NOTIFTIME);

	if (pWnd)
	{
		GetTimeFormat(LOCALE_USER_DEFAULT, 0, NULL, NULL, szTimeBuff, sizeof(szTimeBuff));
		strLastUpdate.Format(IDS_NOTIF_TIMELABEL, szTimeBuff);
		pWnd->SetWindowText(strLastUpdate);
		return TRUE;
	}
	return FALSE;
}


BOOL CNotificationUsers::bSignalNewEntries()
{
	if (!IsWindowVisible())
		// first, make sure window is visible
		ShowWindow(SW_SHOW);

	if (IsIconic() || CWnd::GetForegroundWindow() != this)
	{
		if (!m_bInverted)
		{
			FlashWindow(TRUE);
			m_bInverted = TRUE;
		}
		sndPlaySound("Default sound", SND_ASYNC);
	}

	return TRUE;
}


BOOL CNotificationUsers::bSignalNewUpdate()
{
	m_lstUsers.DeleteAllItems();
	return bUpdateLastUpdateLabel();
}


CUser* CNotificationUsers::GetSelectedUser()
{
	CUser* pUser = NULL;

	if (m_lstUsers.GetSelectedCount() == 1)
	{
		INT iIndex = m_lstUsers.GetNextItem(-1, LVNI_SELECTED);
		ASSERT(iIndex >= 0, "iIndex < 0 in CNotificationUsers::GetSelectedUser");
		pUser = (CUser*) m_lstUsers.GetItemData(iIndex);
		ASSERT(pUser, "pUser is NULL in CNotificationUsers::GetSelectedUser");
	}

	return pUser;
}


const char* CNotificationUsers::GetSelectedNickname()
{
	CUser *pUser = GetSelectedUser();
	if (pUser)
		return pUser->m_strNickname;
	else
		return NULL;
}


void CNotificationUsers::UpdateButtons()
{
	BOOL		bCanInvite();
	const char*	szSelectedNick = NULL;
	CUser*		pUser = GetSelectedUser();

	if (pUser)
		szSelectedNick = pUser->m_strNickname;

	BOOL		bInCurrentRoom = FALSE, bCanJoin = FALSE;
	BOOL		bIsOtherConnected = szSelectedNick && (strcmp(szSelectedNick, GetMyNickName()) != 0) && (pUser->GetFlags() & g_wConnected);
	
	// Make sure selected user is not in current room already
	CChatDoc*	pDoc = GetChatDoc();

	if (szSelectedNick && pDoc && pDoc->GetConnectionStatus() == CX_INCHANNEL) 
	{
		// try to interpret as utterance in current room
		CUserInfo* pui = LookupPui(szSelectedNick, pDoc);
		bInCurrentRoom = pui && !pui->IsDeparted();
	}

	if (bIsOtherConnected)
	{
		// is selected user in a channel?
		if (CHANNELPREFIX(pUser->m_strRoom[0]))
		{
			// make sure i'm not in that channel myself
			pDoc = LookupDoc(pUser->m_strRoom);
			bCanJoin = !pDoc || pDoc->GetConnectionStatus() != CX_INCHANNEL || pDoc != GetChatDoc();
		}
	}

	GetDlgItem(IDC_NOTIFINVITE)->EnableWindow(bIsOtherConnected && bCanInvite() && !bInCurrentRoom);
	GetDlgItem(IDC_NOTIFWHISPER)->EnableWindow(bIsOtherConnected);
	GetDlgItem(IDC_NOTIFJOIN)->EnableWindow(bCanJoin);
}
	

BOOL CNotificationUsers::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	
	CRect rectSizeMinimal;
	GetWindowRect (&rectSizeMinimal);
	m_sizeMinimal.cx = rectSizeMinimal.Width ();
	m_sizeMinimal.cy = rectSizeMinimal.Height ();

	CString strLabel;
	CString strWidth;

	m_lstUsers.SendMessage (LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, 
		LVS_EX_FULLROWSELECT);

	m_lstUsers.SetFont(&theApp.m_fontGui);

	strLabel.LoadString(ID_UL_NICK_LABEL);
	strWidth.LoadString(IDS_NOTIF_NICKWIDTH);
	m_lstUsers.InsertColumn(0, strLabel, LVCFMT_LEFT, atoi(strWidth));

	strLabel.LoadString(ID_UL_IDENT_LABEL);
	strWidth.LoadString(IDS_NOTIF_IDENTWIDTH);
	m_lstUsers.InsertColumn(1, strLabel, LVCFMT_LEFT, atoi(strWidth),1);

	strLabel.LoadString(ID_UL_REALNAME_LABEL);
	strWidth.LoadString(IDS_NOTIF_REALWIDTH);
	m_lstUsers.InsertColumn(2, strLabel, LVCFMT_LEFT, atoi(strWidth),2);

	strLabel.LoadString(ID_UL_ROOM_LABEL);
	strWidth.LoadString(IDS_NOTIF_ROOMWIDTH);
	m_lstUsers.InsertColumn(3, strLabel, LVCFMT_LEFT, atoi(strWidth),3);

	m_lstUsers.SetImageList(&m_ImageList, LVSIL_SMALL);
	m_lstUsers.SetImageList(&m_StateIcons, LVSIL_STATE);

	UINT uMask = m_lstUsers.GetCallbackMask();;
	m_lstUsers.SetCallbackMask(uMask | LVIS_STATEIMAGEMASK);
	
	bUpdateCountLabel();

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CNotificationUsers::OnCancel()
{
	OnCloseDialog();
}


void CNotificationUsers::OnCloseDialog()
{
	theApp.m_bLoginNotifsShown = FALSE;
	theApp.m_dynaNotifs.bRemoveFlagsFromAllUsers(g_wNew);
	ShowWindow(SW_HIDE);
	RedirectFocus();
}


void CNotificationUsers::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI)
{
	lpMMI->ptMinTrackSize.x = m_sizeMinimal.cx;
	lpMMI->ptMinTrackSize.y = m_sizeMinimal.cy;
}


void CNotificationUsers::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO*	pDispInfo = (LV_DISPINFO*) pNMHDR;
	LV_ITEM*		pItem = &pDispInfo->item;
	ASSERT(pItem, "pItem is NULL in CNotificationUsers::OnGetdispinfo");

	CUser*			pUser = (CUser*) pItem->lParam;
	ASSERT(pUser, "pUser is NULL in CNotificationUsers::OnGetdispinfo");

	if (pItem->mask & LVIF_TEXT)
	{
		UINT cchMax = pItem->cchTextMax - 1;

		switch (pItem->iSubItem)
		{
		case 0:
			strncpy(pItem->pszText, (LPCTSTR) pUser->GetPrettyNick(), cchMax);
			break;
		case 1:
			strncpy(pItem->pszText, (LPCTSTR) pUser->m_strIdentity, cchMax);
			break;
		case 2:
			strncpy(pItem->pszText, (LPCTSTR) pUser->m_strFullName, cchMax);
			break;
		case 3:
			strncpy(pItem->pszText, (LPCTSTR) pUser->m_strPrettyRoom, cchMax);
			break;
		default:
			ASSERT(FALSE, "Unexpected pItem->iSubItem in CNotificationUsers::OnGetdispinfo");
		}

		pItem->pszText[cchMax] = g_chEOS;
	}

	if (pItem->mask & LVIF_IMAGE)
	{
		pItem->iImage = (pUser->GetFlags() & g_wConnected) ? 0 : 1;
	}

	if (pItem->mask & LVIF_STATE)
	{
		pItem->state = INDEXTOSTATEIMAGEMASK((pUser->GetFlags() & g_wNew) ? 1 : 0);
	}

	*pResult = 0;
}


void CNotificationUsers::OnShowWindow(BOOL bShow, UINT nStatus)
{
	theApp.m_bLoginNotifsShown = bShow;

	CCSDialog::OnShowWindow(bShow, nStatus);

	if (bShow)
		GetDlgItem (IDC_LSTNOTIFICATIONUSERS)->SetFocus ();
}


void CNotificationUsers::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
{
	CCSDialog::OnActivate(nState, pWndOther, bMinimized);

	if (nState != WA_INACTIVE)
	{
		UpdateButtons();
		if (m_bInverted)
		{
			FlashWindow(FALSE);		// restore icon state on activation
			m_bInverted = FALSE;
		}
	}
}


void CNotificationUsers::OnClose()
{
	CCSDialog::OnClose();
	RedirectFocus();
}


void CNotificationUsers::OnSize(UINT nType, int cx, int cy) 
{
	CCSDialog::OnSize(nType, cx, cy);

	if (nType == SIZE_MINIMIZED)
	{
		theApp.m_dynaNotifs.bRemoveFlagsFromAllUsers(g_wNew);
		return;
	}

	if (nType != SIZE_RESTORED) 
		return;

	if (m_sizeDialog.cx != 0 || m_sizeDialog.cy != 0)
	{
		static RESIZEABLEDLGCTL Ctls[] = {
			{ IDC_LSTNOTIFICATIONUSERS, 	RESIZECTL_STRETCHHORZ | RESIZECTL_STRETCHVERT },
			{ IDC_NOTIFWHISPER, 			RESIZECTL_ALIGNRIGHT | RESIZECTL_ALIGNBOTTOM },
			{ IDC_NOTIFINVITE,  			RESIZECTL_ALIGNRIGHT | RESIZECTL_ALIGNBOTTOM },
			{ IDC_NOTIFJOIN,    			RESIZECTL_ALIGNRIGHT | RESIZECTL_ALIGNBOTTOM },
			{ IDC_NOTIFUPDATE,  			RESIZECTL_ALIGNRIGHT | RESIZECTL_ALIGNBOTTOM },
			{ IDC_NOTIFCLEAR,   			RESIZECTL_ALIGNRIGHT | RESIZECTL_ALIGNBOTTOM },
			{ IDC_CLOSE_NOTIFUSERS,			RESIZECTL_ALIGNRIGHT | RESIZECTL_ALIGNBOTTOM },
			{ IDC_NOTIFCOUNT,      			RESIZECTL_ALIGNBOTTOM },
			{ IDC_NOTIFTIME,       			RESIZECTL_ALIGNRIGHT | RESIZECTL_ALIGNBOTTOM },
			{ IDC_DEFINENOTIF,     			RESIZECTL_ALIGNBOTTOM },
		};

		AdjustResizeableDlgCtls (this, Ctls, _countof(Ctls), m_sizeDialog, CSize (cx, cy));
	}
	m_sizeDialog.cx = cx;
	m_sizeDialog.cy = cy;
	SaveNotifCoords();
}


void CNotificationUsers::OnMove(int x, int y) 
{
	CCSDialog::OnMove(x, y);
	SaveNotifCoords();
}


void CNotificationUsers::OnItemChangedNotifUsers(NMHDR* pNMHDR, LRESULT* pResult) 
{
	UpdateButtons();
	*pResult = 0;
}


void CNotificationUsers::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW*		pNMListView = (NM_LISTVIEW*) pNMHDR;

	if (m_uSortColumn == pNMListView->iSubItem)			// toggle sort order
		m_bSortAscending = !m_bSortAscending;
	else
	{													// change sort column
		m_uSortColumn = (UCHAR) pNMListView->iSubItem;
		m_bSortAscending = TRUE;						// sort ascendingly first
	}
	
	m_lstUsers.SortItems((PFNLVCOMPARE) CompareUsers, (m_uSortColumn << 4) + m_bSortAscending);

	*pResult = 0;
}


void CNotificationUsers::OnNotifInvite()
{
	if (!currentRoom || currentRoom->GetConnectionStatus() != CX_INCHANNEL)
		AfxMessageBox(IDS_OUTCHANNEL_INVITE);
	else
	{
		const char *szNick = GetSelectedNickname();
		if (!szNick)
		{
			ASSERT(FALSE, "!szNick in CNotificationUsers::OnNotifInvite");
			return;
		}
		currentRoom->ChatSendInvitation(UnConst(szNick));
	}	
}


void CNotificationUsers::OnNotifWhisper()
{
	CUser*	pUser = GetSelectedUser();

	if (!pUser)
	{
		ASSERT(FALSE, "!pUser in CNotificationUsers::OnNotifWhisper");
		return;
	}

	ASSERT(pUser->GetFlags() & g_wConnected, "!(pUser->GetFlags() & g_wConnected) in CNotificationUsers::OnNotifWhisper");
	
	CUserInfo* pui = new CUserInfo(pUser->m_strNickname, pUser->m_strIdentity);
	if (pui)
	{
		WhisperBox(pui);
		delete pui;
	}
}


void CNotificationUsers::OnNotifJoin()
{
	CUser *pUser = GetSelectedUser();

	if (!pUser)
	{
		ASSERT(FALSE, "!pUser in CNotificationUsers::OnNotifJoin");
		return;
	}

	ASSERT(pUser->GetFlags() & g_wConnected, "!(pUser->GetFlags() & g_wConnected) in CNotificationUsers::OnNotifJoin");

	g_bEnterOnCreate = FALSE;
	bSwitchToRoom(pUser->m_strRoom);

	UpdateButtons();
}


void CNotificationUsers::OnNotifClear()
{
	theApp.m_dynaNotifs.bRemoveFlagsFromAllUsers(g_wVisible);
	theApp.m_dynaNotifs.bRemoveUsersWithoutFlag(g_wConnected);
	m_lstUsers.DeleteAllItems();
	GetDlgItem(IDC_NOTIFCLEAR)->EnableWindow(FALSE);
	bUpdateCountLabel();
	UpdateButtons();
}


void CNotificationUsers::OnDefineNotif()
{
	ShowWindow(SW_MINIMIZE);
	if (theApp.m_iAutoPage == -1)
	{
		theApp.m_iAutoPage = 1;	// Give focus to the second tab = Logon Notifications
		AfxGetMainWnd()->PostMessage(WM_COMMAND, ID_VIEW_AUTOMATIONS, 0);
	}
}


/*
REGISB: 05/01/98 - Old behavior 
void CNotificationUsers::OnNotifUpdate()
{
	if (theApp.m_dynaNotifs.bDaemonNeeded())
		theApp.m_dynaNotifs.bStartNotifsDaemon(g_uNotifsDaemonNoElapse, TRUE);
	bUpdateLastUpdateLabel();
}
*/


void CNotificationUsers::OnNotifUpdate()
{
	if (theApp.m_dynaNotifs.bUpdateNotifs())
	{
	 	m_lstUsers.DeleteAllItems();
		UpdateButtons();
		bUpdateCountLabel();
		bUpdateLastUpdateLabel();
	}
}


const DWORD CNotificationUsers::m_nHelpIDs[] =
{
	0, 0
};

void 
CNotificationUsers::OnSetFocusUserList(
NMHDR* pNMHDR, 
LRESULT* pResult)
{
	// Set focus item if there isn't any.
	int iIndex = m_lstUsers.GetNextItem(-1, LVNI_FOCUSED);
	if (iIndex == -1)
		m_lstUsers.SetItemState(0, LVIS_FOCUSED, LVIS_FOCUSED);
}

