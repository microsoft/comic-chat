//=--------------------------------------------------------------------------=
// AutoPage.cpp : implementation file
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 01/20/98

#include "stdafx.h"
#include "chat.h"
#include <io.h>
#include "autopage.h"
#include "format.h"
#include "ui.h"
#include "mschat.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "ccommon.h"
#include "cdebug.h"
#include "protsupp.h"
#include "whisprbx.h"


// for ASSERT and FAIL
//
SZTHISFILE

extern CChatApp theApp;


/////////////////////////////////////////////////////////////////////////////
// CAutomationPage property page

CAutomationPage::CAutomationPage() : CCSPropertyPage(CAutomationPage::IDD)
{
	ASSERT(!m_rtfGreetingMesg.m_prgdwFormatting, "m_rtfGreetingMesg.m_prgdwFormatting not NULL in CAutomationPage::CAutomationPage");
	//{{AFX_DATA_INIT(CAutomationPage)
	m_bOKing = FALSE;
	m_bSetActiveNeverCalled = TRUE;

	m_rtfGreetingMesg.m_prgdwFormatting = new CDWordArray;

	char*		szControlFull = strdup((LPCTSTR) theApp.m_strGreetingMesg);
	char*		szControlLess = SzControlLess(szControlFull, m_rtfGreetingMesg.m_prgdwFormatting);

	m_rtfGreetingMesg.m_strText = CString(szControlLess);
	m_rtfGreetingMesg.m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_rtfGreetingMesg.DefineDefaultCharFormat();
	m_iGreetingType = theApp.m_iGreetingType;
	free(szControlFull);

	m_rtfMacro.m_crTextColor = GetSysColor(COLOR_WINDOWTEXT);
	m_rtfMacro.DefineDefaultCharFormat();

	m_bAutoIgnore = ISTRUE(theApp.m_uFloodFlags & FLOOD_IGNORE);

	m_strMesgCnt.Format("%d", m_uMesgCnt = theApp.m_uFloodCount);
	m_strInterval.Format("%d", m_uInterval = theApp.m_uFloodInterval);
	//}}AFX_DATA_INIT
}


void CAutomationPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAutomationPage)
	DDX_Control(pDX, IDC_MACRORICHEDIT, m_rtfMacro);
	DDX_Control(pDX, IDC_MACRONAME, m_macroNameCtl);
	DDX_Control(pDX, IDC_KEY, m_keyCtl);
	DDX_Control(pDX, IDC_GREETINGMESG, m_rtfGreetingMesg);
	DDX_Control(pDX, IDC_MESGCOUNTSPIN, m_spinMesgCnt);
	DDX_Control(pDX, IDC_INTERVALSPIN, m_spinInterval);
	DDX_Control(pDX, IDC_MESGCOUNT, m_mesgCntCtl);
	DDX_Control(pDX, IDC_INTERVAL, m_intervalCtl);
	DDX_Text(pDX, IDC_MESGCOUNT, m_uMesgCnt);
	DDV_MinMaxUInt(pDX, m_uMesgCnt, 1, 255);
	DDX_Text(pDX, IDC_INTERVAL, m_uInterval);
	DDV_MinMaxUInt(pDX, m_uInterval, 1, 255);
	DDX_Check(pDX, IDC_AUTOIGNORE, m_bAutoIgnore);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAutomationPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CAutomationPage)
	ON_CONTROL(EN_CHANGE, IDC_GREETINGMESG, OnChangeGreetingMesg)
	ON_NOTIFY(EN_MSGFILTER, IDC_GREETINGMESG, OnFilterGreetingMesg)
	ON_NOTIFY(EN_MSGFILTER, IDC_MACRORICHEDIT, OnFilterMacro)
	ON_EN_CHANGE(IDC_MESGCOUNT, OnMesgCountChg)
	ON_EN_CHANGE(IDC_INTERVAL, OnIntervalChg)
	ON_BN_CLICKED(IDC_AUTOIGNORE, OnAutoIgnore)
	ON_BN_CLICKED(IDC_NOGREETING, OnNogreeting)
	ON_BN_CLICKED(IDC_SAYGREETING, OnSaygreeting)
	ON_BN_CLICKED(IDC_WHISPERGREETING, OnWhispergreeting)
	ON_BN_CLICKED(IDC_ADD_MACRO, OnAddMacro)
	ON_BN_CLICKED(IDC_DELETE_MACRO, OnDeleteMacro)
	ON_CBN_SELCHANGE(IDC_KEY, OnSelchangeKey)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAutomationPage message handlers

void CMacro::Invoke(
const char *szEncodedChannelName /*= NULL*/, 
CUserInfo *pui /*= NULL*/, 
BOOL bInvokedByRule /*= FALSE*/,
BOOL bInWhisperBox /*= FALSE*/)
{
	if (m_bDefined)
	{
		CDWordArray	rgdwFormatting, *prgdwFormatting;
		extern CChatDoc *LookupDoc(const char *);

		CString		strControlFull = m_strValue;
		CChatDoc*	doc = szEncodedChannelName ? LookupDoc(szEncodedChannelName) : NULL;

		if (!ExpandVariables(strControlFull, doc, pui, bInvokedByRule))
			return;

		char*	szControlFull = strdup((LPCTSTR) strControlFull);
		char*	szControlLess = SzControlLess(szControlFull, &rgdwFormatting);
		char*	szEnd;

		if (bInvokedByRule)
			g_rgpuiWhisperees.RemoveAll();

		while (szEnd = strchr(szControlLess, g_chLF))
		{
			*szEnd = g_chEOS;
			if (szEnd > szControlLess && *(szEnd-1) == g_chCR)
				*(szEnd-1) = g_chEOS;
			if (*szControlLess)
			{
				ChatPreSendText (CString(szControlLess));
				prgdwFormatting = PullFormattingOffsets(&rgdwFormatting, (SHORT) (szControlLess - szControlFull));
				prgdwFormatting = CutFormattingArray(prgdwFormatting, strlen(szControlLess));
				if (bInWhisperBox)
					bWhisperInBox("", CString(szControlLess), prgdwFormatting, BM_WHISPER);
				else
					bChatSendText(CString(szControlLess), BM_SAY, TRUE, prgdwFormatting, szEncodedChannelName, bInvokedByRule /*bWhispereesFilled*/);
				FreeAndNullFormatting(&prgdwFormatting);
			}
			szControlLess = szEnd+1;
		}
		if (*szControlLess)
		{
			ChatPreSendText (CString(szControlLess));
			prgdwFormatting = PullFormattingOffsets(&rgdwFormatting, (SHORT) (szControlLess - szControlFull));
			prgdwFormatting = CutFormattingArray(prgdwFormatting, strlen(szControlLess));
			if (bInWhisperBox)
				bWhisperInBox("", CString(szControlLess), prgdwFormatting, BM_WHISPER);
			else
				bChatSendText(CString(szControlLess), BM_SAY, TRUE, prgdwFormatting, szEncodedChannelName, bInvokedByRule /*bWhispereesFilled*/);
			FreeAndNullFormatting(&prgdwFormatting);
		}

		free(szControlFull);
		rgdwFormatting.RemoveAll();
	}
}


void CopyMacros(CMacro src[], CMacro dest[])
{
	for (int i = 0; i < NMACROS; i++)
	{
		dest[i].m_strValue = src[i].m_strValue;
		dest[i].m_strName  = src[i].m_strName;
		dest[i].m_bDefined = src[i].m_bDefined;
	}
}


void CAutomationPage::OnChangeGreetingMesg()
{
	if (m_bOKing)
		m_bOKing = FALSE;
	else
		SetModified(TRUE);
}


void CAutomationPage::OnFilterGreetingMesg(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct, "pNotifyStruct NULL in CAutomationPage::OnFilterGreetingMesg");

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfGreetingMesg.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


void CAutomationPage::OnFilterMacro(NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct, "pNotifyStruct NULL in CAutomationPage::OnFilterMacro");

	if((pNotifyStruct->code == EN_MSGFILTER) && (pMSGFILTER->msg == WM_RBUTTONDOWN))
		m_rtfMacro.ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


BOOL CAutomationPage::OnSetActive() 
{
#ifdef CB32SUPPORT
	if (theApp.m_bDoCB32)
	{
		m_iGreetingType = AGT_NONE;
		GetDlgItem(IDC_NOGREETING)->EnableWindow(FALSE);
		GetDlgItem(IDC_WHISPERGREETING)->EnableWindow(FALSE);
		GetDlgItem(IDC_SAYGREETING)->EnableWindow(FALSE);
	}
#endif CB32SUPPORT

	if (m_bSetActiveNeverCalled)
	{
		CheckRadioButton(IDC_NOGREETING, IDC_SAYGREETING,
					     IDC_NOGREETING + m_iGreetingType);

		m_rtfGreetingMesg.UseDefaultCharFormat();
		m_rtfGreetingMesg.bSetTextColor(m_rtfGreetingMesg.m_crTextColor);
		m_rtfGreetingMesg.bSetWindowFormattedText(m_rtfGreetingMesg.m_strText, m_rtfGreetingMesg.m_prgdwFormatting);
		m_rtfGreetingMesg.EnableWindow(m_iGreetingType);
		m_rtfGreetingMesg.LimitText(MAX_INPUTLEN);

		m_macroNameCtl.SetFont(&theApp.m_fontGui);

		// Need to add the EN_SELCHANGE & ENM_MOUSEEVENTS notifications to the dwEventMask of the rich text control
		DWORD dwEventMask = (DWORD) m_rtfGreetingMesg.GetEventMask();
		m_rtfGreetingMesg.SetEventMask(dwEventMask | ENM_CHANGE | ENM_MOUSEEVENTS);

		m_rtfMacro.UseDefaultCharFormat();
		m_rtfMacro.bSetTextColor(m_rtfMacro.m_crTextColor);
		m_rtfMacro.LimitText(MAX_INPUTLEN);
	
		// Need to add the ENM_MOUSEEVENTS notification to the dwEventMask of the rich text control
		dwEventMask = (DWORD) m_rtfMacro.GetEventMask();
		m_rtfMacro.SetEventMask(dwEventMask | ENM_MOUSEEVENTS);

		CopyMacros(theApp.m_macros, m_macros);

		m_keyCtl.SetCurSel(0);		// display first entry by default
		OnSelchangeKey();

		((CEdit*) GetDlgItem(IDC_MACRONAME))->SetLimitText(20);

		m_mesgCntCtl.EnableWindow(m_bAutoIgnore);
		m_intervalCtl.EnableWindow(m_bAutoIgnore);

		m_spinMesgCnt.EnableWindow(m_bAutoIgnore);
		m_spinInterval.EnableWindow(m_bAutoIgnore);

		m_spinMesgCnt.SetRange(1, 255);
		m_spinInterval.SetRange(1, 255);
		((CEdit*) GetDlgItem(IDC_MESGCOUNT))->SetLimitText(3);
		((CEdit*) GetDlgItem(IDC_INTERVAL))->SetLimitText(3);

		((CEdit*) GetDlgItem(IDC_MESGCOUNT))->GetWindowText(m_strMesgCnt);
		((CEdit*) GetDlgItem(IDC_INTERVAL))->GetWindowText(m_strInterval);

		m_bSetActiveNeverCalled = FALSE;
	}

	return CCSPropertyPage::OnSetActive();
}


void CAutomationPage::OnOK() 
{
	m_bOKing = TRUE;

	if (m_rtfGreetingMesg.m_prgdwFormatting)
	{
		m_rtfGreetingMesg.m_prgdwFormatting->RemoveAll();
		delete m_rtfGreetingMesg.m_prgdwFormatting;
	}
	m_rtfGreetingMesg.m_prgdwFormatting = PRGDWGetFormatting(&m_rtfGreetingMesg, m_rtfGreetingMesg.m_pFont, m_rtfGreetingMesg.m_crTextColor);

	m_rtfGreetingMesg.GetWindowText(m_rtfGreetingMesg.m_strText); 

	CString strControlFull = m_rtfGreetingMesg.m_strText;
	if (m_rtfGreetingMesg.m_prgdwFormatting)
	{
		char* szCtrlFull = SzControlFull((LPCTSTR) m_rtfGreetingMesg.m_strText, m_rtfGreetingMesg.m_prgdwFormatting);
		if (szCtrlFull)
		{
			strControlFull = CString(szCtrlFull);
			delete [] szCtrlFull;
		}
	}
	theApp.m_strGreetingMesg = strControlFull;

	theApp.m_iGreetingType = m_iGreetingType;

	CopyMacros(m_macros, theApp.m_macros);

	if (GetChatDoc())
		GetChatDoc()->UpdateMacroMenu();

	theApp.m_uFloodCount = m_uMesgCnt;
	theApp.m_uFloodInterval = m_uInterval;

	if (m_bAutoIgnore)
		theApp.m_uFloodFlags |= FLOOD_IGNORE;
	else
		theApp.m_uFloodFlags &= ~FLOOD_IGNORE;

	CCSPropertyPage::OnOK();
}


BOOL CAutomationPage::OnKillActive()
{
	m_bOKing = TRUE;

	return CPropertyPage::OnKillActive();
}


void CAutomationPage::OnNogreeting() 
{
	m_iGreetingType = AGT_NONE;
	m_rtfGreetingMesg.EnableWindow(FALSE);
	SetModified(TRUE);	
}


void CAutomationPage::OnSaygreeting() 
{
	m_iGreetingType = AGT_SAY;
	m_rtfGreetingMesg.EnableWindow(TRUE);
	SetModified(TRUE);		
}


void CAutomationPage::OnWhispergreeting() 
{
	m_iGreetingType = AGT_WHISPER;
	m_rtfGreetingMesg.EnableWindow(TRUE);
	SetModified(TRUE);		
}


void CAutomationPage::OnAddMacro() 
{
	CString strName, strTrimedName, strControlFull;

	int iMacroNum = m_keyCtl.GetCurSel();

	m_macroNameCtl.GetWindowText(strName);

	if (m_rtfMacro.m_prgdwFormatting)
	{
		m_rtfMacro.m_prgdwFormatting->RemoveAll();
		delete m_rtfMacro.m_prgdwFormatting;
	}
	m_rtfMacro.m_prgdwFormatting = PRGDWGetFormatting(&m_rtfMacro, m_rtfMacro.m_pFont, m_rtfMacro.m_crTextColor);

	m_rtfMacro.GetWindowText(m_rtfMacro.m_strText); 

	strControlFull = m_rtfMacro.m_strText;
	if (m_rtfMacro.m_prgdwFormatting)
	{
		char* szCtrlFull = SzControlFull((LPCTSTR) m_rtfMacro.m_strText, m_rtfMacro.m_prgdwFormatting);
		if (szCtrlFull)
		{
			strControlFull = CString(szCtrlFull);
			delete [] szCtrlFull;
		}
	}

	strTrimedName = strName;
	strTrimedName.TrimLeft();

	LPCTSTR szTmp = (LPCTSTR) strControlFull;

	while (*szTmp && my_isspace(*szTmp))
		szTmp++;

	if (iMacroNum < 0 || strTrimedName.IsEmpty() || !*szTmp)
	{
		AfxMessageBox(IDS_EMPTYMACRO);
		return;
	}

	m_macros[iMacroNum].m_strName  = strName;
	m_macros[iMacroNum].m_strValue = strControlFull;
	m_macros[iMacroNum].m_bDefined = TRUE;
	GetDlgItem(IDC_DELETE_MACRO)->EnableWindow(TRUE);

	SetModified(TRUE);
}


void CAutomationPage::OnDeleteMacro() 
{
	int iMacroNum = m_keyCtl.GetCurSel();
	if (iMacroNum < 0)
		return;

	m_macros[iMacroNum].m_strName = "";
	m_macros[iMacroNum].m_strValue = "";
	m_macros[iMacroNum].m_bDefined = FALSE;
	m_macroNameCtl.SetWindowText("");
	m_rtfMacro.SetWindowText("");	// update the char format to use to default??

	SetModified(TRUE);
}


void CAutomationPage::OnSelchangeKey() 
{
	int iMacroNum = m_keyCtl.GetCurSel();
	if (iMacroNum < 0)
		return;

	m_macroNameCtl.SetWindowText(m_macros[iMacroNum].m_strName);
	
	if (m_rtfMacro.m_prgdwFormatting)
		m_rtfMacro.m_prgdwFormatting->RemoveAll();
	else
		m_rtfMacro.m_prgdwFormatting = new CDWordArray;

	char*	szControlFull = strdup((LPCTSTR) m_macros[iMacroNum].m_strValue);
	char*	szControlLess = SzControlLess(szControlFull, m_rtfMacro.m_prgdwFormatting);

	m_rtfMacro.SetWindowText("");
	m_rtfMacro.bSetTextColor(m_rtfMacro.m_crTextColor);	// Reset to default color

	m_rtfMacro.m_strText = CString(szControlLess);
	free(szControlFull);

	if (!m_rtfMacro.m_strText.IsEmpty())
		m_rtfMacro.bSetWindowFormattedText(m_rtfMacro.m_strText, m_rtfMacro.m_prgdwFormatting);

	GetDlgItem(IDC_DELETE_MACRO)->EnableWindow(m_macros[iMacroNum].m_bDefined);
}


void CAutomationPage::OnAutoIgnore() 
{
	m_bAutoIgnore = !m_bAutoIgnore;

	m_mesgCntCtl.EnableWindow(m_bAutoIgnore);
	m_intervalCtl.EnableWindow(m_bAutoIgnore);
	
	m_spinMesgCnt.EnableWindow(m_bAutoIgnore);
	m_spinInterval.EnableWindow(m_bAutoIgnore);

	SetModified(TRUE);
}


void CAutomationPage::OnMesgCountChg()
{
	CEdit*	pEdit = (CEdit*) GetDlgItem(IDC_MESGCOUNT);
	CString strMC;
	UINT	uMC;

	ASSERT(pEdit, "pEdit NULL in CAutomationPage::OnMesgCountChg");

	pEdit->GetWindowText(strMC);
	uMC = atoi(strMC);

	if (uMC < 1 || uMC > 255)
	{
		ASSERT(!m_strMesgCnt.IsEmpty(), "m_strMesgCnt empty in CAutomationPage::OnMesgCountChg");
		pEdit->SetWindowText(m_strMesgCnt);
		pEdit->SetSel(0, -1);
	}
	else
		m_strMesgCnt = strMC;

	SetModified(TRUE);
}


void CAutomationPage::OnIntervalChg()
{
	CEdit*	pEdit = (CEdit*) GetDlgItem(IDC_INTERVAL);
	CString strI;
	UINT	uI;

	ASSERT(pEdit, "pEdit NULL in CAutomationPage::OnIntervalChg");

	pEdit->GetWindowText(strI);
	uI = atoi(strI);

	if (uI < 1 || uI > 255)
	{
		ASSERT(!m_strInterval.IsEmpty(), "m_strInterval empty in CAutomationPage::OnIntervalChg");
		pEdit->SetWindowText(m_strInterval);
		pEdit->SetSel(0, -1);
	}
	else
		m_strInterval = strI;

	SetModified(TRUE);
}


const DWORD CAutomationPage::m_nHelpIDs[] =
{
	IDC_NOGREETING,			IDH_AUTO_NONE,
	IDC_WHISPERGREETING,	IDH_AUTO_WHISPER,
	IDC_SAYGREETING,		IDH_AUTO_SAY,
	IDC_GREETINGMESG,		IDH_AUTO_TEXT,
	IDC_KEY,				IDH_MACRO_KEY,
	IDC_MACRONAME,			IDH_MACRO_NAME,
	IDC_MACRORICHEDIT,		IDH_MACRO_TEXT,
	IDC_ADD_MACRO,			IDH_MACRO_ADD,
	IDC_DELETE_MACRO,		IDH_MACRO_DELETE,
	IDC_AUTOIGNORE,			IDH_OPTIONS_AUTOMATION_FLOODERS,
	IDC_MESGCOUNT,			IDH_OPTIONS_AUTOMATION_MSGCOUNT,
	IDC_INTERVAL,			IDH_OPTIONS_AUTOMATION_INTERVAL,
	IDC_GROUP0,				IDH_GROUP_ID,
	IDC_GROUP1,				IDH_GROUP_ID,
	IDC_GROUP2,				IDH_GROUP_ID,
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CRuleSetsPage property page

CRuleSetsPage::CRuleSetsPage() : CCSPropertyPage(CRuleSetsPage::IDD)
{
	//{{AFX_DATA_INIT(CRuleSetsPage)
	//}}AFX_DATA_INIT
}


CRuleSetsPage::~CRuleSetsPage()
{
}


void CRuleSetsPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRuleSetsPage)
	DDX_Control(pDX, IDC_LSTRULESETS, m_lstSets);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRuleSetsPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CRuleSetsPage)
	ON_BN_CLICKED(IDC_CREATERULESET, OnCreateRuleSet)
	ON_BN_CLICKED(IDC_RENAMERULESET, OnRenameRuleSet)
	ON_BN_CLICKED(IDC_DELETERULESET, OnDeleteRuleSet)
	ON_BN_CLICKED(IDC_MOVERULESETUP, OnMoveUpRuleSet)
	ON_BN_CLICKED(IDC_MOVERULESETDOWN, OnMoveDownRuleSet)
	ON_BN_CLICKED(IDC_BTNLOADSET, OnLoadRuleSet)
	ON_BN_CLICKED(IDC_BTNSAVESET, OnSaveRuleSet)
	ON_LBN_SELCHANGE(IDC_LSTRULESETS, OnRuleSetItemChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CRuleSetsPage::bFillRuleSets()
{
	BOOL		bRet = TRUE;
	INT			iRuleSets, iIndex;
	CCRuleSet*	pRuleSet;
	CPtrArray*	pRuleSetsArray;

	m_lstSets.ResetContent();

	pRuleSetsArray = &(m_pDynaCopy->GetRuleSetsArray());
	iRuleSets = pRuleSetsArray->GetSize();

	for (INT iIndexSet = 0; iIndexSet < iRuleSets && bRet; iIndexSet++)
	{
		pRuleSet = (CCRuleSet*) pRuleSetsArray->GetAt(iIndexSet);
		ASSERT(pRuleSet, "pRuleSet is NULL in CRuleSetsPage::bFillRuleSets");
		ASSERT(!pRuleSet->GetName().IsEmpty(), "pRuleSet->GetName().IsEmpty() in CRuleSetsPage::bFillRuleSets");

		if ((iIndex = m_lstSets.AddString(pRuleSet->GetName())) >= 0)
		{
			m_lstSets.SetItemDataPtr(iIndex, (void*) pRuleSet);
			if ((0 == iIndex && NULL == m_pDynaCopy->GetSelectedRuleSet()) ||
				pRuleSet == m_pDynaCopy->GetSelectedRuleSet())
				SetCurRuleSet(pRuleSet, iIndex);
		}
		else
		{
			ASSERT(FALSE, "AddString failed in CRuleSetsPage::bFillRuleSets");
			bRet = FALSE;
		}
	}

	ASSERT(bRet, "bRet is FALSE in CRuleSetsPage::bFillRuleSets");

	return bRet;
}


void CRuleSetsPage::SetCurRuleSet(CCRuleSet* pRuleSet, INT iIndex)
{
	m_lstSets.SetCurSel(iIndex);
	if (pRuleSet)
		m_pDynaCopy->SetSelectedRuleSet(pRuleSet);
	else
		m_pDynaCopy->SetSelectedRuleSet((CCRuleSet*) m_lstSets.GetItemDataPtr(iIndex));
}


void CRuleSetsPage::UpdateButtonsStatus()
{
	BOOL	bMoveUp = FALSE, bMoveDown = FALSE;
	BOOL	bSetSelected = (m_pDynaCopy->GetSelectedRuleSet() != NULL);
	INT		iIndex;

	GetDlgItem(IDC_RENAMERULESET)->EnableWindow(bSetSelected);
	GetDlgItem(IDC_DELETERULESET)->EnableWindow(bSetSelected);
	GetDlgItem(IDC_BTNSAVESET)->EnableWindow(bSetSelected);

	if (bSetSelected)
	{
		iIndex = m_lstSets.GetCurSel();
		ASSERT(iIndex >= 0, "iIndex < 0 in CRuleSetsPage::UpdateButtonsStatus");
		bMoveUp = iIndex > 0;
		bMoveDown = iIndex < m_lstSets.GetCount()-1;
	}
	GetDlgItem(IDC_MOVERULESETUP)->EnableWindow(bMoveUp);
	GetDlgItem(IDC_MOVERULESETDOWN)->EnableWindow(bMoveDown);
}


/////////////////////////////////////////////////////////////////////////////
// CRuleSetsPage message handlers

BOOL CRuleSetsPage::OnSetActive() 
{
	// OutputDebugString("CRuleSetsPage::OnSetActive - Enter\n");

	ASSERT(m_pDynaCopy, "m_pDynaCopy is NULL in CRuleSetsPage::OnSetActive");

	bFillRuleSets();
	UpdateButtonsStatus();

	return CCSPropertyPage::OnSetActive();
}


void CRuleSetsPage::OnOK()
{
	// OutputDebugString("CRuleSetsPage::OnOK - Enter\n");

	ASSERT(m_pDynaCopy, "m_pDynaCopy is NULL in CRuleSetsPage::OnOK");

	// save the changes made to the rule sets...
	theApp.m_dynaRules = *m_pDynaCopy;

	// ...and update the rules daemon
	if (theApp.m_dynaRules.bDaemonNeeded())
		theApp.m_dynaRules.bStartRulesDaemon(g_uRulesDaemonShortElapse, TRUE /*bForceReset*/);
	else
		theApp.m_dynaRules.bStopRulesDaemon();

	theApp.m_dynaRules.bUpdateRuleSetsDaemonExt(FALSE);

	CCSPropertyPage::OnOK();
}


void CRuleSetsPage::OnCreateRuleSet()
{
	CCreateSet csDlg(&m_lstSets);

	INT	iIndex;

	if (IDOK == theApp.DoModalDlg(&csDlg))
	{
		CString strSetName = csDlg.m_strSetName;

		ASSERT(!strSetName.IsEmpty(), "strSetName.IsEmpty() in CRuleSetsPage::OnCreateRuleSet");

		// Make sure this set doesn't already exist
		ASSERT(LB_ERR == m_lstSets.FindStringExact(-1, (LPCTSTR) strSetName), "Rule Set name existing in CRuleSetsPage::OnCreateRuleSet");

		CCRuleSet*	pRuleSet = (CCRuleSet*) new CCRuleSet(m_pDynaCopy);

		if (!pRuleSet)
			return;

		pRuleSet->SetName(strSetName);
		pRuleSet->Activate();

		if ((iIndex = m_lstSets.AddString(strSetName)) >= 0)
		{
			m_lstSets.SetItemDataPtr(iIndex, (void*) pRuleSet);
			SetCurRuleSet(pRuleSet, iIndex);
			m_pDynaCopy->bAddRuleSet(pRuleSet);
			SetModified(TRUE);
		}
		else
		{
			ASSERT(FALSE, "AddString failed in CRuleSetsPage::OnCreateRuleSet");
			delete pRuleSet;
		}

		GotoDlgCtrl(&m_lstSets);
		UpdateButtonsStatus();
	}
	else
		GotoDlgCtrl(GetDlgItem(IDC_CREATERULESET));
}


void CRuleSetsPage::OnRenameRuleSet()
{
	CCRuleSet*	pRuleSet = m_pDynaCopy->GetSelectedRuleSet();
	INT			iIndex = m_lstSets.GetCurSel();

	ASSERT(iIndex >= 0, "iIndex < 0 in CRuleSetsPage::OnRenameRuleSet");
	ASSERT(pRuleSet, "pRuleSet is NULL in CRuleSetsPage::OnRenameRuleSet");
	ASSERT(pRuleSet == (CCRuleSet*) m_lstSets.GetItemDataPtr(iIndex), "pRuleSet != m_lstSets.GetItemDataPtr(iIndex) in CRuleSetsPage::OnRenameRuleSet");

	CRenameSet rsDlg(pRuleSet, &m_lstSets);

	if (IDRENAME == theApp.DoModalDlg(&rsDlg))
	{
		m_lstSets.DeleteString(iIndex);
		m_lstSets.InsertString(iIndex, (LPCTSTR) pRuleSet->GetName());
		m_lstSets.SetItemDataPtr(iIndex, (void*) pRuleSet);
		SetCurRuleSet(pRuleSet, iIndex);
		SetModified(TRUE);
	}
}


void CRuleSetsPage::OnDeleteRuleSet()
{
	CCRuleSet*	pRuleSet = m_pDynaCopy->GetSelectedRuleSet();
	INT			iIndex = m_lstSets.GetCurSel(), iCount;

	ASSERT(iIndex >= 0, "iIndex < 0 in CRuleSetsPage::OnDeleteRuleSet");
	ASSERT(pRuleSet, "pRuleSet is NULL in CRuleSetsPage::OnDeleteRuleSet");
	ASSERT(pRuleSet == (CCRuleSet*) m_lstSets.GetItemDataPtr(iIndex), "pRuleSet != m_lstSets.GetItemDataPtr(iIndex) in CRuleSetsPage::OnDeleteRuleSet");

	m_lstSets.DeleteString(iIndex);	// Remove rule set from the list box
	
	m_pDynaCopy->bRemoveRuleSet(NULL, iIndex);

	if ((iCount = m_lstSets.GetCount()) == 0)
	{
		m_pDynaCopy->SetSelectedRuleSet(NULL);
		GotoDlgCtrl(GetDlgItem(IDC_CREATERULESET));
	}
	else
		SetCurRuleSet(NULL, iIndex < iCount ? iIndex : iIndex - 1);

	UpdateButtonsStatus();
	SetModified(TRUE);
}


void CRuleSetsPage::OnMoveUpRuleSet()
{
	CCRuleSet*	pRuleSet = m_pDynaCopy->GetSelectedRuleSet();
	INT			iIndex = m_lstSets.GetCurSel();

	ASSERT(iIndex >= 1, "iIndex < 1 in CRuleSetsPage::OnMoveUpRuleSet");
	ASSERT(pRuleSet, "pRuleSet is NULL in CRuleSetsPage::OnMoveUpRuleSet");
	ASSERT(pRuleSet == (CCRuleSet*) m_lstSets.GetItemDataPtr(iIndex), "pRuleSet != m_lstSets.GetItemDataPtr(iIndex) in CRuleSetsPage::OnMoveUpRuleSet");

	m_pDynaCopy->bUpRuleSet(NULL, iIndex);
	m_lstSets.DeleteString(iIndex);
	m_lstSets.InsertString(iIndex - 1, pRuleSet->GetName());
	m_lstSets.SetItemDataPtr(iIndex - 1, (void*) pRuleSet);
	SetCurRuleSet(pRuleSet, iIndex - 1);

	UpdateButtonsStatus();

	UINT		uIDC = (iIndex == 1) ? IDC_MOVERULESETDOWN : IDC_MOVERULESETUP;
	GotoDlgCtrl(GetDlgItem(uIDC));

	SetModified(TRUE);
}


void CRuleSetsPage::OnMoveDownRuleSet()
{
	CCRuleSet*	pRuleSet = m_pDynaCopy->GetSelectedRuleSet();
	INT			iIndex = m_lstSets.GetCurSel();

	ASSERT(iIndex >= 0, "iIndex < 0 in CRuleSetsPage::OnMoveDownRuleSet");
	ASSERT(iIndex < m_lstSets.GetCount()-1, "iIndex >= m_lstSets.GetCount()-1 in CRuleSetsPage::OnMoveDownRuleSet");
	ASSERT(pRuleSet, "pRuleSet is NULL in CRuleSetsPage::OnMoveDownRuleSet");
	ASSERT(pRuleSet == (CCRuleSet*) m_lstSets.GetItemDataPtr(iIndex), "pRuleSet != m_lstSets.GetItemDataPtr(iIndex) in CRuleSetsPage::OnMoveDownRuleSet");

	m_pDynaCopy->bDownRuleSet(NULL, iIndex);
	m_lstSets.DeleteString(iIndex);
	m_lstSets.InsertString(iIndex + 1, pRuleSet->GetName());
	m_lstSets.SetItemDataPtr(iIndex + 1, (void*) pRuleSet);
	SetCurRuleSet(pRuleSet, iIndex + 1);
	
	UpdateButtonsStatus();

	UINT		uIDC = (iIndex == m_lstSets.GetCount()-2) ? IDC_MOVERULESETUP : IDC_MOVERULESETDOWN;
	GotoDlgCtrl(GetDlgItem(uIDC));

	SetModified(TRUE);
}


void CRuleSetsPage::OnLoadRuleSet()
{
	CCRuleSet*	pNewRuleSet = (CCRuleSet*) new CCRuleSet(m_pDynaCopy);
	UINT		uError, uIDS;
	INT			iIndex;

	if (!pNewRuleSet)
		return;

	if (!pNewRuleSet->bLoadFromFile(&uError, (CWnd*) this))
	{
		switch (uError)
		{
		case g_uErrVersion:
			uIDS = IDS_ERR_CRS_VERSION;
			break;
		case g_uErrFormat:
			uIDS = IDS_ERR_CRS_FORMAT;
			break;
		case CFileException::none:
			uIDS = 0;
			break;
		case CFileException::accessDenied:
			uIDS = IDS_ERR_CRS_ACCESSDENIED;
			break;
		case CFileException::lockViolation:
			uIDS = IDS_ERR_CRS_LOCKVIOLATION;
			break;
		default:
			uIDS = IDS_ERR_CRS_LOADGENERIC;
		}
		if (uIDS) 
			AfxMessageBox(uIDS);
		delete pNewRuleSet;
	}
	else
	{
		if (uError == g_uErrRulesSkipped)
			AfxMessageBox(IDS_ERR_CRS_SKIPPEDRULES);

		// Add new rule set to current rule set list
		// Make sure this set doesn't already exist
		if (LB_ERR == (iIndex = m_lstSets.FindStringExact(-1, (LPCTSTR) pNewRuleSet->GetName())))
			goto add;
		else
		{
			CSetNameConflict	sncDlg(pNewRuleSet->GetName(), &m_lstSets);

			switch (theApp.DoModalDlg(&sncDlg))
			{
			case IDCANCEL:
				delete pNewRuleSet;
				break;
			case IDOVERWRITE:
				m_lstSets.DeleteString(iIndex);	// Remove rule set from the list box
				m_pDynaCopy->bRemoveRuleSet(NULL, iIndex);
				goto add;
			case IDRENAME:
				pNewRuleSet->SetName(sncDlg.m_strSetName);
				goto add;
			}
		}
	}
	return;

add:
	ASSERT(pNewRuleSet, "pNewRuleSet is NULL in CRuleSetsPage::OnLoadRuleSet");
	if ((iIndex = m_lstSets.AddString(pNewRuleSet->GetName())) >= 0)
	{
		m_lstSets.SetItemDataPtr(iIndex, (void*) pNewRuleSet);
		SetCurRuleSet(pNewRuleSet, iIndex);
		m_pDynaCopy->bAddRuleSet(pNewRuleSet);
		SetModified(TRUE);
	}
	else
	{
		ASSERT(FALSE, "AddString failed in CRuleSetsPage::OnLoadRuleSet");
		delete pNewRuleSet;
	}
	UpdateButtonsStatus();
}


void CRuleSetsPage::OnSaveRuleSet()
{
	CCRuleSet*	pRuleSet = m_pDynaCopy->GetSelectedRuleSet();
	INT			iIndex = m_lstSets.GetCurSel();
	UINT		uError;

	ASSERT(iIndex >= 0, "iIndex < 0 in CRuleSetsPage::OnSaveRuleSet");
	ASSERT(iIndex < m_lstSets.GetCount(), "iIndex >= m_lstSets.GetCount()-1 in CRuleSetsPage::OnSaveRuleSet");
	ASSERT(pRuleSet, "pRuleSet is NULL in CRuleSetsPage::OnSaveRuleSet");
	ASSERT(pRuleSet == (CCRuleSet*) m_lstSets.GetItemDataPtr(iIndex), "pRuleSet != m_lstSets.GetItemDataPtr(iIndex) in CRuleSetsPage::OnSaveRuleSet");

	if (!pRuleSet->bSaveToFile(&uError, (CWnd*) this))
	{
		UINT	uIDS;

		switch (uError)
		{
		case CFileException::accessDenied:
			uIDS = IDS_ERR_CRS_ACCESSDENIED;
			break;
		case CFileException::lockViolation:
			uIDS = IDS_ERR_CRS_LOCKVIOLATION;
			break;
		case CFileException::diskFull:
			uIDS = IDS_ERR_CRS_DISKFULL;
			break;
		default:
			uIDS = IDS_ERR_CRS_SAVEGENERIC;
		}
		AfxMessageBox(uIDS);
	}
}


void CRuleSetsPage::OnRuleSetItemChanged()
{
	INT iIndex = m_lstSets.GetCurSel();

	if (LB_ERR == iIndex)
		m_pDynaCopy->SetSelectedRuleSet(NULL);
	else
	{
		void*	pvData = m_lstSets.GetItemDataPtr(iIndex);

		if ((void*) -1 != pvData)
		{
			CCRuleSet*	pRuleSet = (CCRuleSet*) pvData;

			m_pDynaCopy->SetSelectedRuleSet(pRuleSet);
		}
	}
	UpdateButtonsStatus();	// Called more often than needed
}


const DWORD CRuleSetsPage::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CRulesPage property page
CRulesPage::CRulesPage() : CCSPropertyPage(CRulesPage::IDD)
{
	//{{AFX_DATA_INIT(CRulesPage)
	//}}AFX_DATA_INIT
	m_bRulesColumnSet = FALSE;

	m_ilActiveStatus.Create(IDB_INACTIVE, 16, 1, RGB(0, 0, 255));
	CBitmap	bmp;
	if (bmp.LoadBitmap(IDB_ACTIVE))
	{
		m_ilActiveStatus.Add(&bmp, RGB(0, 0, 255));
		bmp.DeleteObject();
	}
	if (bmp.LoadBitmap(IDB_STOPPED))
	{
		m_ilActiveStatus.Add(&bmp, RGB(0, 0, 128));
		bmp.DeleteObject();
	}
}


CRulesPage::~CRulesPage()
{
	m_ilActiveStatus.DeleteImageList();
}


void CRulesPage::DoDataExchange(CDataExchange* pDX)
{
	CCSPropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRulesPage)
	DDX_Control(pDX, IDC_LSTRULES, m_lstRules);
	DDX_Control(pDX, IDC_LSTSETS, m_lstSets);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CRulesPage, CCSPropertyPage)
	//{{AFX_MSG_MAP(CRulesPage)
	ON_LBN_SELCHANGE(IDC_LSTSETS, OnRuleSetItemChanged)
	ON_BN_CLICKED(IDC_ADDRULE, OnAddRule)
	ON_BN_CLICKED(IDC_EDITRULE, OnEditRule)
	ON_BN_CLICKED(IDC_DELETERULE, OnDeleteRule)
	ON_BN_CLICKED(IDC_DUPLICATERULE, OnDuplicateRule)
	ON_BN_CLICKED(IDC_MOVEUP, OnMoveUpRule)
	ON_BN_CLICKED(IDC_MOVEDOWN, OnMoveDownRule)
	ON_BN_CLICKED(IDC_ADDTORULESETS, OnAddToRuleSets)
	ON_BN_CLICKED(IDC_ADVANCEDRULE, OnAdvancedRuleSettings)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_LSTRULES, OnRuleItemChanged)
	ON_NOTIFY(NM_DBLCLK, IDC_LSTRULES, OnDblclkRule)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CRulesPage::bFillRuleSets()
{
	BOOL		bRet = TRUE;
	INT			iRuleSets, iIndex;
	CCRuleSet*	pRuleSet;
	CPtrArray*	pRuleSetsArray;

	m_lstSets.ResetContent();

	pRuleSetsArray = &(m_pDynaCopy->GetRuleSetsArray());
	iRuleSets = pRuleSetsArray->GetSize();

	for (INT iIndexSet = 0; iIndexSet < iRuleSets && bRet; iIndexSet++)
	{
		pRuleSet = (CCRuleSet*) pRuleSetsArray->GetAt(iIndexSet);
		ASSERT(pRuleSet, "pRuleSet is NULL in CRulesPage::bFillRuleSets");
		ASSERT(!pRuleSet->GetName().IsEmpty(), "pRuleSet->GetName().IsEmpty() in CRulesPage::bFillRuleSets");

		if ((iIndex = m_lstSets.AddString(pRuleSet->GetName())) >= 0)
		{
			m_lstSets.SetItemDataPtr(iIndex, (void*) pRuleSet);
			if ((0 == iIndex && NULL == m_pDynaCopy->GetSelectedRuleSet()) ||
				pRuleSet == m_pDynaCopy->GetSelectedRuleSet())
				SetCurRuleSet(pRuleSet, iIndex);
		}
		else
		{
			ASSERT(FALSE, "AddString failed in CRulesPage::bFillRuleSets");
			bRet = FALSE;
		}
	}

	ASSERT(bRet, "bRet is FALSE in CRulesPage::bFillRuleSets");

	return bRet;
}


void CRulesPage::SetCurRuleSet(CCRuleSet* pRuleSet, INT iIndex)
{
	m_lstSets.SetCurSel(iIndex);
	if (pRuleSet)
		m_pDynaCopy->SetSelectedRuleSet(pRuleSet);
	else
		m_pDynaCopy->SetSelectedRuleSet((CCRuleSet*) m_lstSets.GetItemDataPtr(iIndex));
	m_lstRules.bFill(m_pDynaCopy);
}


void CRulesPage::UpdateButtonsStatus()
{
	BOOL	bMoveUp = FALSE, bMoveDown = FALSE, bRuleSelected = m_lstRules.GetSelectedCount() > 0;
	BOOL	bSetSelected = (m_pDynaCopy->GetSelectedRuleSet() != NULL);
	INT		iIndex;

	GetDlgItem(IDC_ADDRULE)->EnableWindow(bSetSelected);
	GetDlgItem(IDC_EDITRULE)->EnableWindow(bRuleSelected);
	GetDlgItem(IDC_DELETERULE)->EnableWindow(bRuleSelected);
	GetDlgItem(IDC_DUPLICATERULE)->EnableWindow(bRuleSelected);
	GetDlgItem(IDC_ADDTORULESETS)->EnableWindow(bRuleSelected && m_pDynaCopy->GetRuleSetsArray().GetSize() > 1);

	if (bRuleSelected)
	{
		iIndex = m_lstRules.iGetSelectedRule(NULL);
		ASSERT(iIndex >= 0, "iIndex < 0 in CRulesPage::UpdateButtonsStatus");
		bMoveUp = iIndex > 0;
		bMoveDown = iIndex < m_lstRules.GetItemCount()-1;
	}
	GetDlgItem(IDC_MOVEUP)->EnableWindow(bMoveUp);
	GetDlgItem(IDC_MOVEDOWN)->EnableWindow(bMoveDown);
}


/////////////////////////////////////////////////////////////////////////////
// CRulesPage message handlers

BOOL CRulesPage::OnSetActive() 
{
	// OutputDebugString("CRulesPage::OnSetActive - Enter\n");

	ASSERT(m_pDynaCopy, "m_pDynaCopy is NULL in CRulesPage::OnSetActive");

	if (!m_bRulesColumnSet)
	{
		CString strLabel;
		CString strWidth;

		m_lstRules.SetImageList(&m_ilActiveStatus, LVSIL_SMALL);
		m_lstRules.SetFont(&theApp.m_fontGui);

		strLabel.LoadString(IDS_EVENTS_LABEL);
		strWidth.LoadString(IDS_EVENTS_WIDTH);
		m_lstRules.InsertColumn(0, strLabel, LVCFMT_LEFT, atoi(strWidth));

		strLabel.LoadString(IDS_ACTIONS_LABEL);
		strWidth.LoadString(IDS_ACTIONS_WIDTH);
		m_lstRules.InsertColumn(1, strLabel, LVCFMT_LEFT, atoi(strWidth), 1);

		m_bRulesColumnSet = TRUE;
	}

	bFillRuleSets();
	m_lstRules.bFill(m_pDynaCopy);
	UpdateButtonsStatus();

	return CCSPropertyPage::OnSetActive();
}


void CRulesPage::OnOK()
{
	// OutputDebugString("CRulesPage::OnOK - Enter\n");

	ASSERT(m_pDynaCopy, "m_pDynaCopy is NULL in CRulesPage::OnOK");

	// save the changes made to the rules...
	theApp.m_dynaRules = *m_pDynaCopy;

	// ...and update the rules daemon
	if (theApp.m_dynaRules.bDaemonNeeded())
		theApp.m_dynaRules.bStartRulesDaemon(g_uRulesDaemonShortElapse, TRUE /*bForceReset*/);
	else
		theApp.m_dynaRules.bStopRulesDaemon();

	theApp.m_dynaRules.bUpdateRuleSetsDaemonExt(FALSE);

	CCSPropertyPage::OnOK();
}


void CRulesPage::OnRuleSetItemChanged()
{
	INT iIndex = m_lstSets.GetCurSel();

	if (LB_ERR == iIndex)
		m_pDynaCopy->SetSelectedRuleSet(NULL);
	else
	{
		void*	pvData = m_lstSets.GetItemDataPtr(iIndex);

		if ((void*) -1 != pvData)
			m_pDynaCopy->SetSelectedRuleSet((CCRuleSet*) pvData);
	}
	m_lstRules.bFill(m_pDynaCopy);

	UpdateButtonsStatus();	// Called more often than needed
}


void CRulesPage::OnAddRule()
{
	CEditRule	erDlg;
	CCRule*		pRule;
	CCRuleSet*	pRuleSet;

	if (!(pRuleSet = m_pDynaCopy->GetSelectedRuleSet()))
	{
		ASSERT(FALSE, "GetSelectedRuleSet failed in CRulesPage::OnAddRule");
		return;
	}

	if (!(pRule = new CCRule(m_pDynaCopy)))
	{
		ASSERT(FALSE, "OOM in CRulesPage::OnAddRule");
		return;
	}

	erDlg.UseRule(pRule, m_pDynaCopy);

	if (theApp.DoModalDlg(&erDlg) == IDOK)
	{
		// pRuleSet->SetModified();
		pRuleSet->bAddRule(pRule);
		m_lstRules.bAddRule(pRule);		// Add new rule at the end of the list
		pRule->Activate();				// New rule is born activated
		pRule->SetDelay(erDlg.GetDelay());
		m_lstRules.SetItemState(m_lstRules.GetItemCount()-1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);	// Select that new rule
		m_lstRules.EnsureVisible(m_lstRules.GetItemCount()-1, FALSE);
		SetModified(TRUE);
	}
	else
		pRule->Release();
}


void CRulesPage::OnEditRule()
{
	ASSERT(m_lstRules.GetSelectedCount() == 1, "m_lstRules.GetSelectedCount() != 1 in CRulesPage::OnEditRule");

	CCRule*	pRule;
	INT		iIndex = m_lstRules.iGetSelectedRule(&pRule);

	ASSERT(iIndex >= 0, "iIndex < 0 in CRulesPage::OnEditRule");
	ASSERT(pRule, "pRule is NULL in CRulesPage::OnEditRule");

	CEditRule	erDlg;
	CCRule*		pRuleCopy = (CCRule*) new CCRule(pRule, m_pDynaCopy);
	CCRuleSet*	pRuleSet;

	if (!(pRuleSet = m_pDynaCopy->GetSelectedRuleSet()))
	{
		ASSERT(FALSE, "GetSelectedRuleSet failed in CRulesPage::OnEditRule");
		return;
	}

	// REGISB: should i desactivate pRule while it is being changed?
	// pRule->Desactivate();

	if (!pRuleCopy)
	{
		ASSERT(FALSE, "OOM in CRulesPage::OnEditRule");
		return;
	}

	erDlg.UseRule(pRuleCopy, m_pDynaCopy);

	if (theApp.DoModalDlg(&erDlg) == IDOK)
	{
		// pRuleSet->SetModified();
		pRuleCopy->SetDelay(erDlg.GetDelay());
		pRule->CopyRule(pRuleCopy);
		m_lstRules.SetItemText(iIndex, 0, (LPTSTR) (LPCTSTR) pRule->StrGetEventDisplay());
		m_lstRules.SetItemText(iIndex, 1, (LPTSTR) (LPCTSTR) pRule->StrGetActionDisplay());
		SetModified(TRUE);
	}
	pRuleCopy->Release();
}


void CRulesPage::OnDeleteRule()
{
	ASSERT(m_lstRules.GetSelectedCount() == 1, "m_lstRules.GetSelectedCount() != 1 in CRulesPage::OnDeleteRule");

	CCRuleSet*	pRuleSet;
	INT			iIndex = m_lstRules.iGetSelectedRule(NULL);

	ASSERT(iIndex >= 0, "iIndex < 0 in CRulesPage::OnDeleteRule");

	if (!(pRuleSet = m_pDynaCopy->GetSelectedRuleSet()))
	{
		ASSERT(FALSE, "GetSelectedRuleSet failed in CRulesPage::OnDeleteRule");
		return;
	}

	m_lstRules.DeleteItem(iIndex);			// Remove rule from the list control

	pRuleSet->bRemoveRule(NULL, iIndex);	// Remove rule from the array
	// pRuleSet->SetModified();
	SetModified(TRUE);

	if (m_lstRules.GetItemCount() > 0)
		m_lstRules.SetItemState(iIndex, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	else
		GotoDlgCtrl(GetDlgItem(IDC_ADDRULE));
}


void CRulesPage::OnDuplicateRule()
{
	ASSERT(m_lstRules.GetSelectedCount() == 1, "m_lstRules.GetSelectedCount() != 1 in CRulesPage::OnDuplicateRule");

	CCRule*		pRule;
	CCRuleSet*	pRuleSet;
	INT			iIndex = m_lstRules.iGetSelectedRule(NULL);

	ASSERT(iIndex >= 0, "iIndex < 0 in CRulesPage::OnDuplicateRule");

	if (!(pRuleSet = m_pDynaCopy->GetSelectedRuleSet()))
	{
		ASSERT(FALSE, "GetSelectedRuleSet failed in CRulesPage::OnDuplicateRule");
		return;
	}

	if (pRuleSet->bDuplicateRule(iIndex, &pRule))
	{
		ASSERT(pRule, "pRule is NULL in CRulesPage::OnDuplicateRule");
		// pRuleSet->SetModified();
		m_lstRules.bAddRule(pRule, iIndex + 1);
		SetModified(TRUE);
	}
}


void CRulesPage::OnMoveUpRule()
{
	ASSERT(m_lstRules.GetSelectedCount() == 1, "m_lstRules.GetSelectedCount() != 1 in CRulesPage::OnMoveUpRule");

	CCRuleSet*	pRuleSet;
	CCRule*		pRule;
	INT			iIndex = m_lstRules.iGetSelectedRule(&pRule);

	ASSERT(iIndex >= 1, "iIndex < 1 in CRulesPage::OnMoveUpRule");
	ASSERT(pRule, "pRule is NULL in CRulesPage::OnMoveUpRule");

	if (!(pRuleSet = m_pDynaCopy->GetSelectedRuleSet()))
	{
		ASSERT(FALSE, "GetSelectedRuleSet failed in CRulesPage::OnMoveUpRule");
		return;
	}

	m_lstRules.DeleteItem(iIndex);
	m_lstRules.bAddRule(pRule, iIndex - 1);
	m_lstRules.SetItemState(iIndex - 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	pRuleSet->bUpRule(NULL, iIndex);
	// pRuleSet->SetModified();

	UINT		uIDC = (iIndex == 1) ? IDC_MOVEDOWN : IDC_MOVEUP;
	GotoDlgCtrl(GetDlgItem(uIDC));

	SetModified(TRUE);
}


void CRulesPage::OnMoveDownRule()
{
	ASSERT(m_lstRules.GetSelectedCount() == 1, "m_lstRules.GetSelectedCount() != 1 in CRulesPage::OnMoveDownRule");

	CCRuleSet*	pRuleSet;
	CCRule*		pRule;
	INT			iIndex = m_lstRules.iGetSelectedRule(&pRule);

	ASSERT(iIndex >= 0, "iIndex < 0 in CRulesPage::OnMoveDownRule");
	ASSERT(iIndex < m_lstRules.GetItemCount()-1, "iIndex >= m_lstRules.GetItemCount()-1 in CRulesPage::OnMoveDownRule");
	ASSERT(pRule, "pRule is NULL in CRulesPage::OnMoveDownRule");

	if (!(pRuleSet = m_pDynaCopy->GetSelectedRuleSet()))
	{
		ASSERT(FALSE, "GetSelectedRuleSet failed in CRulesPage::OnMoveDownRule");
		return;
	}

	m_lstRules.DeleteItem(iIndex);
	m_lstRules.bAddRule(pRule, iIndex + 1);
	m_lstRules.SetItemState(iIndex + 1, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	pRuleSet->bDownRule(NULL, iIndex);
	// pRuleSet->SetModified();
	
	UINT		uIDC = (iIndex == m_lstRules.GetItemCount()-2) ? IDC_MOVEUP : IDC_MOVEDOWN;
	GotoDlgCtrl(GetDlgItem(uIDC));

	SetModified(TRUE);
}


void CRulesPage::OnAddToRuleSets()
{
	CCRule*		pRule;
	INT			iIndex = m_lstRules.iGetSelectedRule(&pRule);
	CCDynaRules	m_dynaCopy;

	ASSERT(iIndex >= 0, "iIndex < 0 in CRulesPage::OnAddToRuleSets");
	ASSERT(pRule, "pRule is NULL in CRulesPage::OnAddToRuleSets");

	m_dynaCopy = *m_pDynaCopy;

	ASSERT(m_dynaCopy.GetSelectedRuleSet(), "m_dynaCopy.GetSelectedRuleSet() is NULL in CRulesPage::OnAddToRuleSets");

	CAddToSets	atsDlg(&m_dynaCopy, m_dynaCopy.GetSelectedRuleSet(), pRule);
	
	if (theApp.DoModalDlg(&atsDlg) == IDOK && atsDlg.bRuleAdded())
	{
		*m_pDynaCopy = m_dynaCopy;

		bFillRuleSets();
		m_lstRules.bFill(m_pDynaCopy);
		UpdateButtonsStatus();
		SetModified(TRUE);
	}
}


void CRulesPage::OnAdvancedRuleSettings()
{
	ASSERT(m_pDynaCopy, "m_pDynaCopy is NULL in CRulesPage::OnAdvancedRuleSettings");

	CAdvancedRuleSettings arsDlg(m_pDynaCopy->GetFloodingOccurrences(), m_pDynaCopy->GetFloodingInterval());

	if (theApp.DoModalDlg(&arsDlg) == IDOK)
	{
		m_pDynaCopy->SetFloodParams(arsDlg.m_uInt, arsDlg.m_uOcc);
		SetModified(TRUE);
	}
}


void CRulesPage::OnRuleItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
	UpdateButtonsStatus();	// Called more often than needed
	*pResult = 0;
}


void CRulesPage::OnDblclkRule(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// same as hitting Edit Rule... button
	if (m_lstRules.GetSelectedCount() == 1)
		OnEditRule();
	*pResult = 0;
}


const DWORD CRulesPage::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CRuleIcons
CRuleIcons::CRuleIcons()
{
	// 0 - Inactive
	// 1 - Active
	for (SHORT nCnt = 0; nCnt < g_nIconCount; nCnt++)
		m_hbmpIcon[nCnt] = LoadBitmap(AfxGetResourceHandle(), MAKEINTRESOURCE(IDB_INACTIVE+nCnt));
}


CRuleIcons::~CRuleIcons()
{
	for (SHORT nCnt = 0; nCnt < g_nIconCount; nCnt++)
		if (m_hbmpIcon[nCnt])
			DeleteObject((HBITMAP) m_hbmpIcon[nCnt]);
}


/////////////////////////////////////////////////////////////////////////////
// CRuleSetsListBox

BEGIN_MESSAGE_MAP(CRuleSetsListBox, CListBox)
	//{{AFX_MSG_MAP(CRuleSetsListBox)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	// ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CRuleSetsListBox::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	ASSERT(lpDrawItemStruct, "lpDrawItemStruct is NULL in CRuleSetsListBox::DrawItem");

	DWORD		rgbSelOld;
	DWORD		rgbSelTextOld;
	DWORD		rgbSel;
	DWORD		rgbSelText;
	HDC			hdcBitmap;
	HBITMAP		hbmpOld;
	HBRUSH		hbrush;
	CCRuleSet*	pRuleSet;

	lpDrawItemStruct->rcItem.left += 2*g_nIconMargin+g_nIconWidth-1;

	// Rule Sets list box control
	if (lpDrawItemStruct->itemState & ODS_SELECTED)
	{
		rgbSelText = GetSysColor(COLOR_HIGHLIGHTTEXT);
		rgbSel = GetSysColor(COLOR_HIGHLIGHT);
	}
	else
	{
		rgbSelText = GetSysColor(COLOR_WINDOWTEXT);
		rgbSel = GetSysColor(COLOR_WINDOW);
	}
		
	hbrush = CreateSolidBrush(rgbSel);
	FillRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem), hbrush);
	
	if (lpDrawItemStruct->itemID != (UINT) -1)
	{
		pRuleSet = (CCRuleSet*) lpDrawItemStruct->itemData;
		if (!pRuleSet)
		{
			ASSERT(FALSE, "pRuleSet is NULL in CRuleSetsListBox::DrawItem");
			return;
		}

		hdcBitmap = CreateCompatibleDC(lpDrawItemStruct->hDC);
		hbmpOld = (HBITMAP) SelectObject(hdcBitmap, m_icons.GetIcon(pRuleSet->bActive() ? g_nActiveIndex : g_nInactiveIndex));
		BitBlt(lpDrawItemStruct->hDC, g_nIconMargin, lpDrawItemStruct->rcItem.top+1, g_nIconWidth, g_nIconHeight, hdcBitmap, 0, 0, SRCCOPY);

		rgbSelTextOld = SetTextColor(lpDrawItemStruct->hDC, rgbSelText);
		rgbSelOld = SetBkColor(lpDrawItemStruct->hDC, rgbSel);
		TextOut(lpDrawItemStruct->hDC, g_nIconWidth+2*g_nIconMargin, lpDrawItemStruct->rcItem.top+1, pRuleSet->GetName(), pRuleSet->GetName().GetLength());

		SelectObject(hdcBitmap, hbmpOld);
		DeleteDC(hdcBitmap);
	}

	if (lpDrawItemStruct->itemState & ODS_FOCUS)
		DrawFocusRect(lpDrawItemStruct->hDC, &(lpDrawItemStruct->rcItem));
		
	SetTextColor(lpDrawItemStruct->hDC, rgbSelTextOld);
	DeleteObject(hbrush);
	SetBkColor(lpDrawItemStruct->hDC, rgbSelOld);
}


void CRuleSetsListBox::SwitchActivation(INT iIndex)
{
	CCRuleSet* pRuleSet = (CCRuleSet*) GetItemDataPtr(iIndex);
	ASSERT((LONG) pRuleSet != -1, "pRuleSet == -1 in CRuleSetsListBox::SwitchActivation");
	if (pRuleSet->bActive())
		pRuleSet->Desactivate();
	else
		pRuleSet->Activate();
	// pRuleSet->SetModified();
	((CRuleSetsPage*) GetParent())->SetModified(TRUE);
	RECT rect;
	rect.left = g_nIconMargin;
	rect.right = 2*g_nIconMargin+g_nIconWidth;
	rect.top = (iIndex - GetTopIndex()) * GetItemHeight(0);
	rect.bottom = rect.top + GetItemHeight(0) - 1;
	InvalidateRect(&rect);
	UpdateWindow();
}


/////////////////////////////////////////////////////////////////////////////
// CRuleSetsListBox message handlers
void CRuleSetsListBox::OnLButtonDown(UINT nFlags, CPoint point)
{
	CListBox::OnLButtonDown(nFlags, point);

	if (point.x >= g_nIconMargin && point.x < 2*g_nIconMargin + g_nIconWidth-1)
	{
		INT iIndex = (INT) (point.y / GetItemHeight(0)) + GetTopIndex();
		if (iIndex < GetCount())
			SwitchActivation(iIndex);
	}
}


void CRuleSetsListBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (g_chSpace == nChar)
	{
		INT	iIndex = GetCurSel();
		if (iIndex != LB_ERR)
			SwitchActivation(iIndex);
	}
	CListBox::OnChar(nChar, nRepCnt, nFlags);
}


/*
/////////////////////////////////////////////////////////////////////////////
// CTabButton
BEGIN_MESSAGE_MAP(CTabButton, CButton)
	//{{AFX_MSG_MAP(CTabButton)
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


UINT CTabButton::OnGetDlgCode()
{
	return CButton::OnGetDlgCode() | DLGC_WANTTAB;
}


void CTabButton::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_TAB == nChar)
	{
		// OutputDebugString("CTabButton::OnKeyDown - Tab\n");
		CEditRule* per = (CEditRule*) GetParent();
		ASSERT(per, "per is NULL CTabButton::OnKeyDown");
		per->Tab(this, !(GetKeyState(VK_SHIFT) & 0x8000));
		return;
	}
	else
		CButton::OnKeyDown(nChar, nRepCnt, nFlags);
}


/////////////////////////////////////////////////////////////////////////////
// CTabEdit
BEGIN_MESSAGE_MAP(CTabEdit, CEdit)
	//{{AFX_MSG_MAP(CTabEdit)
	ON_WM_KEYDOWN()
	ON_WM_GETDLGCODE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


UINT CTabEdit::OnGetDlgCode()
{
	return CEdit::OnGetDlgCode() | DLGC_WANTTAB;
}


void CTabEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_TAB == nChar)
	{
		// OutputDebugString("CTabEdit::OnKeyDown - Tab\n");
		CEditRule* per = (CEditRule*) GetParent();
		ASSERT(per, "per is NULL CTabEdit::OnKeyDown");
		per->Tab(this, !(GetKeyState(VK_SHIFT) & 0x8000));
		MSG msg;
		while (PeekMessage(&msg, m_hWnd, WM_CHAR, WM_CHAR, PM_REMOVE));
		return;
	}
	else
		CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/


/////////////////////////////////////////////////////////////////////////////
// CRulesListCtrl

BEGIN_MESSAGE_MAP(CRulesListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CRulesListCtrl)
	ON_NOTIFY_REFLECT(LVN_GETDISPINFO, OnGetdispinfo)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	// ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


INT CRulesListCtrl::iGetSelectedRule(CCRule **ppRule)
{
	INT iIndex = GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);

	if (ppRule)
		*ppRule = NULL;

	if (iIndex < 0)
		return iIndex;

	if (ppRule)
		*ppRule = (CCRule*) GetItemData(iIndex);

	return iIndex;
}


void CRulesListCtrl::SwitchActivation(INT iIndex)
{
	CCRule*	pRule = (CCRule*) GetItemData(iIndex);
	ASSERT(pRule, "pRule is NULL in CRulesListCtrl::SwitchActivation");
	if (pRule->bStopped())
	{
		ASSERT(pRule->bActive(), "pRule not activated in CRulesListCtrl::SwitchActivation");
		pRule->SetFlags(pRule->wGetFlags() & ~g_wStopped);
	}
	else
		if (pRule->bActive())
			pRule->Desactivate();
		else
			pRule->Activate();
	ASSERT(m_pRuleSet, "m_pRuleSet is NULL in CRulesListCtrl::SwitchActivation");
	// m_pRuleSet->SetModified();
	((CRulesPage*) GetParent())->SetModified(TRUE);
	RedrawItems(iIndex, iIndex);
}


/////////////////////////////////////////////////////////////////////////////
// CRulesListCtrl message handlers
void CRulesListCtrl::OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult) 
{
	LV_DISPINFO*	pDispInfo = (LV_DISPINFO*) pNMHDR;
	LV_ITEM*		pItem = &pDispInfo->item;
	ASSERT(pItem, "pItem is NULL in CRulesListCtrl::OnGetdispinfo");

	CCRule*			pRule = (CCRule*) pItem->lParam;
	ASSERT(pRule, "pRule is NULL in CRulesListCtrl::OnGetdispinfo");

	if (pItem->mask & LVIF_TEXT)
	{
		UINT			cchMax = pItem->cchTextMax - 1;
		CString			strText;

		if (pItem->iSubItem == 0 || pItem->iSubItem == 1)
		{
			strText = pItem->iSubItem == 0 ? pRule->StrGetEventDisplay() : pRule->StrGetActionDisplay();
			strncpy(pItem->pszText, (LPCTSTR) strText, cchMax);
			pItem->pszText[cchMax] = g_chEOS;  // insurance
		}
	}

	if (pItem->mask & LVIF_IMAGE)
		pItem->iImage = (pRule->bActive() ? (pRule->bStopped() ? 2 : 1) : 0);

	*pResult = 0;
}


void CRulesListCtrl::OnLButtonDown(UINT nFlags, CPoint point)
{
	UINT	uFlags;
	INT		iIndex = HitTest(point, &uFlags);

	if (iIndex >= 0 && (LVHT_ONITEMICON & uFlags))
		SwitchActivation(iIndex);

	CListCtrl::OnLButtonDown(nFlags, point);
}


void CRulesListCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (g_chSpace == nChar)
	{
		INT	iIndex = GetNextItem(-1, LVNI_ALL|LVNI_SELECTED);
		if (iIndex >= 0)
			SwitchActivation(iIndex);
	}
	CListCtrl::OnChar(nChar, nRepCnt, nFlags);
}


BOOL CRulesListCtrl::bFill(CCDynaRules* pDynaRules)
{
	BOOL		bRet = TRUE;
	INT			iRules;
	CPtrArray*	pRulesArray;

	ASSERT(pDynaRules, "pDynaRules is NULL in CRulesListCtrl::bFill");

	DeleteAllItems();

	if (!(m_pRuleSet = pDynaRules->GetSelectedRuleSet()))
		return TRUE;	// No selected rule set

	pRulesArray = &(m_pRuleSet->GetRulesArray());
	iRules = pRulesArray->GetSize();

	for (INT iIndex = 0; iIndex < iRules && bRet; iIndex++)
		bRet = bAddRule((CCRule*) pRulesArray->GetAt(iIndex), iIndex);

	ASSERT(bRet, "bRet is FALSE in CRulesListCtrl::bFill");

	return bRet;
}


BOOL CRulesListCtrl::bAddRule(CCRule* pRule, INT iIndex /* = -1 */)
{
	ASSERT(pRule, "pRule is NULL in CRulesListCtrl::bAddRule");

	LV_ITEM item;
	BOOL	bRet;

	if (iIndex < 0)
		item.iItem = GetItemCount();
	else
		item.iItem = iIndex;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT | LVIF_PARAM | LVIF_IMAGE;
	item.pszText = LPSTR_TEXTCALLBACK;
	item.lParam = (LPARAM) pRule;
	item.iImage = I_IMAGECALLBACK;
	bRet = (InsertItem(&item) != -1);

	item.iSubItem = 1;
	item.pszText = LPSTR_TEXTCALLBACK;
	bRet &= (SetItem(&item) != -1);

	ASSERT(bRet, "bRet is FALSE in CRulesListCtrl::bAddRule");

	return bRet;
}



/////////////////////////////////////////////////////////////////////////////
// Used to fill the param combo with sound file names
void CALLBACK AddSoundToComboBox(LPARAM lParam, LPCSTR pszPath, LPCSTR pszFile, int nFileType)
{
	CSoundComboBox* pCmbSnd = (CSoundComboBox*) lParam;

	ASSERT(pCmbSnd, "pCmbSnd is NULL in AddSoundToComboBox");
	ASSERT(pszFile, "pszFile is NULL in AddSoundToComboBox");

	int iIndex = pCmbSnd->AddString(pszFile);

	if (iIndex >= 0)
		pCmbSnd->SetItemData(iIndex, (DWORD) nFileType);
}


/////////////////////////////////////////////////////////////////////////////
// Used to fill the param combo with text file names
void CALLBACK AddTextFileToComboBox(LPARAM lParam, LPCSTR pszPath, LPCSTR pszFile, int nFileType)
{
	CComboBox* pCmb = (CComboBox*) lParam;

	ASSERT(pCmb, "pCmb is NULL in AddTextFileToComboBox");
	ASSERT(pszFile, "pszFile is NULL in AddTextFileToComboBox");

	CString strRelPath = pszPath;
	CHAR	szAllFiles[128];
	CHAR	*szNextFiles, *szCurAllFiles = szAllFiles;

	::LoadString(AfxGetResourceHandle(), IDS_TEXTFILES, szAllFiles, 127);
	
	do
	{
		if (!stricmp(pszFile, GetToken(szCurAllFiles, &szNextFiles)))
			return;
		szCurAllFiles = szNextFiles;
	}
	while (*szCurAllFiles);

	strRelPath = strRelPath.Right(strRelPath.GetLength() - theApp.m_strBaseDir.GetLength() - 1);
	if (!strRelPath.IsEmpty())
		strRelPath += "\\";
	pCmb->AddString(strRelPath + pszFile + szTxtExt);
}


/////////////////////////////////////////////////////////////////////////////
// CAdvancedEventParams dialog

CAdvancedEventParams::CAdvancedEventParams(WORD wFlags, CWnd* pwndParent /*=NULL*/)
	: CCSDialog(CAdvancedEventParams::IDD, pwndParent)
{
	//{{AFX_DATA_INIT(CAdvancedEventParams)
	m_iMatchCase = (wFlags & g_wMatchCase) ? 1 : 0;
	m_iMatchWord = (wFlags & g_wMatchWord) ? 1 : 0;
	//}}AFX_DATA_INIT
}


void CAdvancedEventParams::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedEventParams)
	DDX_Check(pDX, IDC_CHKMATCHCASE, m_iMatchCase);
	DDX_Check(pDX, IDC_CHKMATCHWORD, m_iMatchWord);
	//}}AFX_DATA_MAP
}


const DWORD CAdvancedEventParams::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CAdvancedRuleSettings dialog

CAdvancedRuleSettings::CAdvancedRuleSettings(UCHAR uOcc, UCHAR uInt, CWnd* pwndParent /*=NULL*/)
	: CCSDialog(CAdvancedRuleSettings::IDD, pwndParent)
{
	//{{AFX_DATA_INIT(CAdvancedRuleSettings)
	m_uOcc = uOcc;
	m_uInt = uInt;
	//}}AFX_DATA_INIT
}


void CAdvancedRuleSettings::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAdvancedRuleSettings)
	DDX_Control(pDX, IDC_RULEOCCSPIN, m_spinOcc);
	DDX_Control(pDX, IDC_RULEINTSPIN, m_spinInt);
	DDX_Control(pDX, IDC_RULEOCC, m_editOcc);
	DDX_Control(pDX, IDC_RULEINT, m_editInt);
	DDX_Text(pDX, IDC_RULEOCC, m_uOcc);
	DDV_MinMaxUInt(pDX, m_uOcc, 1, 255);
	DDX_Text(pDX, IDC_RULEINT, m_uInt);
	DDV_MinMaxUInt(pDX, m_uInt, 1, 255);
	//}}AFX_DATA_MAP
}


BOOL CAdvancedRuleSettings::OnInitDialog() 
{
	CCSDialog::OnInitDialog();

	m_spinOcc.SetRange(1, 255);
	m_spinInt.SetRange(1, 255);
	((CEdit*) GetDlgItem(IDC_RULEOCC))->SetLimitText(3);
	((CEdit*) GetDlgItem(IDC_RULEINT))->SetLimitText(3);

	return FALSE;  // return TRUE unless you set the focus to a control
}


const DWORD CAdvancedRuleSettings::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CSoundComboBox control
CSoundComboBox::CSoundComboBox()
{
	m_bFilled  = FALSE;

	for (int i = 0; i < SOUNDTYPES; i++)
		m_hIcons[i] = GetFiletypeIcon(GetSupportedSoundTypes()[i], TRUE);
}


CSoundComboBox::~CSoundComboBox()
{
	for (int i = 0; i < SOUNDTYPES; i++)
		DestroyIcon(m_hIcons[i]);	// REGISB: Not sure this is needed since help file says only necessary for icons created with CreateIconIndirect function
}


/*
BEGIN_MESSAGE_MAP(CSoundComboBox, CIconicComboBox)
	//{{AFX_MSG_MAP(CSoundComboBox)
	ON_WM_GETDLGCODE()
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()
*/


HICON CSoundComboBox::GetIcon(UINT nIndex, LPCSTR pszString, DWORD dwItemData)
{
	ASSERT(dwItemData < SOUNDTYPES, "dwItemData out of range in CSoundComboBox::GetIcon");
	return m_hIcons[dwItemData];
}


/*
UINT CSoundComboBox::OnGetDlgCode()
{
	return CIconicComboBox::OnGetDlgCode() | DLGC_WANTTAB;
}


void CSoundComboBox::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_TAB == nChar)
	{
		// OutputDebugString("CSoundComboBox::OnKeyDown - Tab\n");
		CEditRule* per = (CEditRule*) GetParent();
		ASSERT(per, "per is NULL in CSoundComboBox::OnKeyDown");
		per->Tab(this, !(GetKeyState(VK_SHIFT) & 0x8000));
		return;
	}
	CIconicComboBox::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/


/////////////////////////////////////////////////////////////////////////////
// CEditRule dialog
CEditRule::CEditRule(CWnd* pwndParent /*=NULL*/)
	: CCSDialog(CEditRule::IDD, pwndParent)
{
	//{{AFX_DATA_INIT(CEditRule)
	//}}AFX_DATA_INIT
	m_nSoundFileType = 0;
	m_pRule = NULL;
	m_pDynaRules = NULL;
	m_uMinDelay = 0;

	for (UINT uCnt = 0; uCnt < (UINT) ptMax; uCnt++)
	{
		m_rgstrParamLabels[uCnt].LoadString(IDS_LBL_ACTIVATE + uCnt);
		m_prgdwActionParamFormatting[uCnt] = NULL;
	}
}


CEditRule::~CEditRule()
{
	for (UINT uCnt = 0; uCnt < (UINT) ptMax; uCnt++)
		FreeAndNullFormatting(&(m_prgdwActionParamFormatting[uCnt]));
}


BOOL CEditRule::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_TAB)
	{
		CWnd* pWnd = GetFocus();
		Tab(pWnd, !(GetKeyState(VK_SHIFT) & 0x8000));
		return TRUE;
	}
	else
		return CCSDialog::PreTranslateMessage(pMsg);
}


void CEditRule::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CEditRule)
	DDX_Control(pDX, IDC_CHKSUBRULES, m_chkSubRules);
	DDX_Control(pDX, IDC_BTNADVANCED, m_btnAdvanced);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);

	DDX_Control(pDX, IDC_CMBEVENTS, m_cmbEvents);
	DDX_Control(pDX, IDC_CMBACTIONS, m_cmbActions);
	DDX_Control(pDX, IDC_LBLPARAMDESC, m_lblParamDesc);

	for (UINT uIndex = 0; uIndex < g_uMaxEventParams; uIndex++)
	{
		DDX_Control(pDX, IDC_LBLEP0+2*uIndex, m_lblEventParams[uIndex]);
		DDX_Control(pDX, IDC_CMBEP0+2*uIndex, m_cmbEventParams[uIndex]);
	}
	for (uIndex = 0; uIndex < g_uMaxActionParams; uIndex++)
	{
		DDX_Control(pDX, IDC_LBLAP0+2*uIndex, m_lblActionParams[uIndex]);
		DDX_Control(pDX, IDC_CMBAP0+2*uIndex, m_cmbActionParams[uIndex]);
	}

	DDX_Control(pDX, IDC_DELAYSPIN, m_spinDelay);
	DDX_Control(pDX, IDC_RULEDELAY, m_editDelay);
	DDX_Text(pDX, IDC_RULEDELAY, m_uDelay);
	DDV_MinMaxUInt(pDX, m_uDelay, m_uMinDelay, 255);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CEditRule, CCSDialog)
	//{{AFX_MSG_MAP(CEditRule)
	ON_CBN_SELCHANGE(IDC_CMBEVENTS, OnEventChanged)
	ON_CBN_SELCHANGE(IDC_CMBACTIONS, OnActionChanged)
	ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_CMBEP0, IDC_CMBEP2, OnEventParamSetFocus)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, IDC_CMBEP0, IDC_CMBEP2, OnEventParamKillFocus)
	ON_CONTROL_RANGE(CBN_SETFOCUS, IDC_CMBAP0, IDC_CMBAP2, OnActionParamSetFocus)
	ON_CONTROL_RANGE(CBN_KILLFOCUS, IDC_CMBAP0, IDC_CMBAP2, OnActionParamKillFocus)
	ON_CONTROL_RANGE(EN_SETFOCUS, IDC_RTFAP0, IDC_RTFAP2, OnActionParamSetFocus)
	ON_CONTROL_RANGE(EN_KILLFOCUS, IDC_RTFAP0, IDC_RTFAP2, OnActionParamKillFocus)
	ON_CONTROL(CBN_SETFOCUS, IDC_CMBEPNS, OnEventNetParamSetFocus)
	ON_CONTROL(CBN_KILLFOCUS, IDC_CMBEPNS, OnKillFocusClear)
	ON_CONTROL(CBN_SETFOCUS, IDC_CMBAPNS, OnActionNetParamSetFocus)
	ON_CONTROL(CBN_KILLFOCUS, IDC_CMBAPNS, OnKillFocusClear)
	ON_CONTROL(CBN_SETFOCUS, IDC_CMBAPSD, OnActionSndParamSetFocus)
	ON_CONTROL(CBN_KILLFOCUS, IDC_CMBAPSD, OnKillFocusClear)
	ON_CONTROL(CBN_SETFOCUS, IDC_CMBEVENTS, OnEventsCmbSetFocus)
	ON_CONTROL(CBN_KILLFOCUS, IDC_CMBEVENTS, OnKillFocusClear)
	ON_CONTROL(CBN_SETFOCUS, IDC_CMBACTIONS, OnActionsCmbSetFocus)
	ON_CONTROL(CBN_KILLFOCUS, IDC_CMBACTIONS, OnKillFocusClear)
	ON_NOTIFY_RANGE(EN_MSGFILTER, IDC_RTFAP0, IDC_RTFAP2, OnActionParamFilter)
	ON_BN_CLICKED(IDC_CHKSUBRULES, OnFlagsCheckClick)
	ON_BN_CLICKED(IDC_BTNADVANCED, OnAdvancedClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CEditRule::Tab(CWnd* pWnd, BOOL bForward)
{
	ASSERT(pWnd, "pWnd is NULL in CEditRule::Tab");
	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::Tab");

	UINT uIndex, uActionParams = m_pRule->GetAction()->GetParamNum();

	if (pWnd == (CWnd*) &m_cmbActions)
	{
		if (bForward)
		{
			if (uActionParams > 0)
			{
				switch (m_pRule->GetAction()->GetParamType(0))
				{
					case ptSoundFileName:
						GotoDlgCtrl(&m_cmbActionSndParam);
						break;
					default:
						if (m_cmbActionParams[0].bGetRtfMode())
						{
							ASSERT(m_cmbActionParams[0].GetRtfCmbEdit(), "m_cmbActionParams[0].GetRtfCmbEdit() is NULL in CEditRule::Tab");
							GotoDlgCtrl(m_cmbActionParams[0].GetRtfCmbEdit());
						}
						else
							GotoDlgCtrl(&m_cmbActionParams[0]);
				}
			}
			else
			{
				if (m_editDelay.IsWindowEnabled())
					GotoDlgCtrl(&m_editDelay);
				else
					GotoDlgCtrl(&m_chkSubRules);
			}
		}
		else
		{
			if (m_btnAdvanced.IsWindowEnabled())
				GotoDlgCtrl(&m_btnAdvanced);
			else
				if (ptServerName == m_pRule->GetEvent()->GetParamType(m_pRule->GetEvent()->GetParamNum()-1))
					GotoDlgCtrl(&m_cmbEventNetParam);
				else
					GotoDlgCtrl(&m_cmbEventParams[m_pRule->GetEvent()->GetParamNum()-1]);
		}
		return;
	}

	if ((pWnd == (CWnd*) &m_editDelay) ||
		(!bForward && pWnd == (CWnd*) &m_chkSubRules && !m_editDelay.IsWindowEnabled()))
	{
		if (bForward)
			GotoDlgCtrl(&m_chkSubRules);
		else
			if (uActionParams > 0)
			{
				switch (m_pRule->GetAction()->GetParamType(uActionParams-1))
				{
					case ptServerName:
						GotoDlgCtrl(&m_cmbActionNetParam);
						break;
					case ptSoundFileName:
						GotoDlgCtrl(&m_cmbActionSndParam);
						break;
					default:
						if (m_cmbActionParams[uActionParams-1].bGetRtfMode())
						{
							ASSERT(m_cmbActionParams[uActionParams-1].GetRtfCmbEdit(), "m_cmbActionParams[uActionParams-1].GetRtfCmbEdit() is NULL in CEditRule::Tab");
							GotoDlgCtrl(m_cmbActionParams[uActionParams-1].GetRtfCmbEdit());
						}
						else
							GotoDlgCtrl(&m_cmbActionParams[uActionParams-1]);
				}
			}
			else
				GotoDlgCtrl(&m_cmbActions);
		return;
	}

	if (pWnd == (CWnd*) &m_chkSubRules)
	{
		if (bForward)
			NextDlgCtrl();
		else
			PrevDlgCtrl();
		return;
	}

	if (pWnd == (CWnd*) &m_btnCancel)
	{
		if (bForward)
			GotoDlgCtrl(&m_cmbEvents);
		else
			PrevDlgCtrl();
		return;
	}

	if (pWnd == (CWnd*) &m_cmbEvents)
	{
		if (bForward)
			GotoDlgCtrl(&m_cmbEventParams[0]);
		else
			GotoDlgCtrl(&m_btnCancel);
		return;
	}

	for (uIndex = 0; uIndex < uActionParams; uIndex++)
		if (pWnd->GetParent() == (CWnd*) &m_cmbActionParams[uIndex] || 
			pWnd == (CWnd*) m_cmbActionParams[uIndex].GetRtfCmbEdit())
			break;

	if (uIndex >= uActionParams && pWnd->GetParent() == (CWnd*) &m_cmbActionSndParam)
		for (uIndex = 0; uIndex < uActionParams; uIndex++)
			if (ptSoundFileName == m_pRule->GetAction()->GetParamType(uIndex))
				break;

	if (uIndex < uActionParams)
	{
		if (bForward)
		{
			if (uIndex < uActionParams-1)
			{
				switch (m_pRule->GetAction()->GetParamType(uIndex+1))
				{
					case ptServerName:
						GotoDlgCtrl(&m_cmbActionNetParam);
						break;
					case ptSoundFileName:
						GotoDlgCtrl(&m_cmbActionSndParam);
						break;
					default:
						if (m_cmbActionParams[uIndex+1].bGetRtfMode())
							GotoDlgCtrl(m_cmbActionParams[uIndex+1].GetRtfCmbEdit());
						else
							GotoDlgCtrl(&m_cmbActionParams[uIndex+1]);
				}
			}
			else
			{
				ASSERT(uIndex == uActionParams-1, "uIndex != uActionParams-1 in CEditRule::Tab");
				if (m_editDelay.IsWindowEnabled())
					GotoDlgCtrl(&m_editDelay);
				else
					GotoDlgCtrl(&m_chkSubRules);
			}
			return;
		}
		else
		{
			if (uIndex > 0)
			{
				if (m_cmbActionParams[uIndex-1].bGetRtfMode())
					GotoDlgCtrl(m_cmbActionParams[uIndex-1].GetRtfCmbEdit());
				else
					GotoDlgCtrl(&m_cmbActionParams[uIndex-1]);
			}
			else
			{
				ASSERT(uIndex == 0, "uIndex != 0 in CEditRule::Tab");
				GotoDlgCtrl(&m_cmbActions);
			}
			return;
		}
	}

	if (bForward)
		NextDlgCtrl();
	else
		PrevDlgCtrl();
}


void CEditRule::UseRule(CCRule* pRule, CCDynaRules* pDynaRules)
{
	ASSERT(pRule, "pRule is NULL in CEditRule::UseRule");
	ASSERT(pDynaRules, "pDynaRules is NULL in CEditRule::UseRule");

	m_pRule = pRule;
	m_pDynaRules = pDynaRules;
}


BOOL CEditRule::bFillActionsFromEvent(BOOL* pbActionChanged)
{
	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::bFillActionsFromEvent");
	ASSERT(m_pRule->GetEvent(), "m_pRule->m_pEvent is NULL in CEditRule::bFillActionsFromEvent");

	m_cmbActions.ResetContent();

	DWORD		dwActionMask = 1L;
	DWORD		dwEnabledActions = m_pRule->GetEvent()->GetEnabledActions();
	UINT		uIndex;
	INT			iIndex;
	BOOL		bActionSelected = FALSE;
	CCAction*	pAction;

	for (uIndex = 0; uIndex < (UINT) aMax; uIndex++)
	{
		if (dwEnabledActions & dwActionMask)
		{
			pAction = theApp.m_rulesData.GetAction(uIndex);
			if ((iIndex = m_cmbActions.AddString(pAction->GetLongDesc())) < 0)
			{
				m_cmbActions.ResetContent();
				ASSERT(FALSE, "AddString failed in CEditRule::bFillActionsFromEvent");
				return FALSE;
			}
			else
			{
				m_cmbActions.SetItemDataPtr(iIndex, pAction);
				if (m_pRule->GetAction() == pAction)
				{
					m_cmbActions.SetCurSel(iIndex);
					bActionSelected = TRUE;
				}
			}
		}
		dwActionMask <<= 1;
	}

	if (!bActionSelected)
	{
		m_cmbActions.SetCurSel(0);	// Select the first action in the combo
		m_pRule->SetAction((CCAction*) m_cmbActions.GetItemDataPtr(0));
	}

	if (pbActionChanged)
		*pbActionChanged = !bActionSelected;

	return TRUE;
}


void CEditRule::FillParamLabels(BOOL bEvents, BOOL bActions)
{
	UINT uIndex, uParamNum;
	BOOL bSDPresent, bNSPresent;

	if (bEvents)
	{
		ASSERT(m_pRule->GetEvent(), "m_pRule->GetEvent() is NULL in CEditRule::FillParamLabels");
		bNSPresent = FALSE;
		uParamNum = m_pRule->GetEvent()->GetParamNum();
		for (uIndex = 0; uIndex < uParamNum; uIndex++)
		{
			enumParamType ptEventParam = m_pRule->GetEvent()->GetParamType(uIndex);
			m_lblEventParams[uIndex].SetWindowText(m_rgstrParamLabels[(UINT) ptEventParam]);
			m_lblEventParams[uIndex].ShowWindow(SW_SHOWNOACTIVATE);
			if (ptServerName == ptEventParam)
			{
				m_cmbEventParams[uIndex].ShowWindow(SW_HIDE);
				m_cmbEventParams[uIndex].ResetContent();
				m_cmbEventNetParam.ShowWindow(SW_SHOWNOACTIVATE);
				CRect rec, recDropdown;
				m_cmbEventParams[uIndex].GetWindowRect(&rec);
				m_cmbEventParams[uIndex].GetDroppedControlRect(recDropdown);
				rec.bottom = recDropdown.bottom+4;	//+4 because the net cmb item height is bigger than usual
				m_cmbEventParams[uIndex].GetParent()->ScreenToClient(&rec);
				m_cmbEventNetParam.MoveWindow(&rec);
				bNSPresent = TRUE;
			}
			else
			{
				m_cmbEventParams[uIndex].ShowWindow(SW_SHOWNOACTIVATE);
				m_cmbEventParams[uIndex].ResetContent();
			}
		}

		for (uIndex = uParamNum; uIndex < g_uMaxEventParams; uIndex++)
		{
			m_lblEventParams[uIndex].ShowWindow(SW_HIDE);
			m_cmbEventParams[uIndex].ShowWindow(SW_HIDE);
			m_cmbEventParams[uIndex].ResetContent();
		}

		if (!bNSPresent)
			m_cmbEventNetParam.ShowWindow(SW_HIDE);
		m_cmbEventNetParam.ResetContent();

		GetDlgItem(IDC_LBLEP)->ShowWindow(uParamNum ? SW_SHOWNOACTIVATE : SW_HIDE);
	}

	if (bActions)
	{
		ASSERT(m_pRule->GetAction(), "m_pRule->GetAction() is NULL in CEditRule::FillParamLabels");
		bSDPresent = bNSPresent = FALSE;
		uParamNum = m_pRule->GetAction()->GetParamNum();
		for (uIndex = 0; uIndex < uParamNum; uIndex++)
		{
			enumParamType ptActionParam = m_pRule->GetAction()->GetParamType(uIndex);
			m_lblActionParams[uIndex].SetWindowText(m_rgstrParamLabels[(UINT) ptActionParam]);
			m_lblActionParams[uIndex].ShowWindow(SW_SHOWNOACTIVATE);
			if (ptServerName == ptActionParam || ptSoundFileName == ptActionParam)
			{
				CWnd* pWnd = (ptServerName == ptActionParam) ? (CWnd*) &m_cmbActionNetParam : (CWnd*) &m_cmbActionSndParam;
				CRect rec, recDropdown;

				m_cmbActionParams[uIndex].ShowWindow(SW_HIDE);
				m_cmbActionParams[uIndex].ResetContent();
				pWnd->ShowWindow(SW_SHOWNOACTIVATE);
				m_cmbActionParams[uIndex].GetWindowRect(&rec);
				m_cmbActionParams[uIndex].GetDroppedControlRect(recDropdown);
				rec.bottom = recDropdown.bottom+4;	//+4 because the net cmb item height is bigger than usual
				m_cmbActionParams[uIndex].GetParent()->ScreenToClient(&rec);
				pWnd->MoveWindow(&rec);
				bNSPresent = (ptServerName == ptActionParam);
				bSDPresent = !bNSPresent;
			}
			else
			{
				m_cmbActionParams[uIndex].ShowWindow(SW_SHOWNOACTIVATE);
				m_cmbActionParams[uIndex].ResetContent();
				m_cmbActionParams[uIndex].bSetRtfMode(RTFParam(ptActionParam, m_pRule->GetAction()->GetID()));
				m_cmbActionParams[uIndex].bAttachRtfCtrl(IDC_RTFAP0 + uIndex);
				m_cmbActionParams[uIndex].LimitText(g_uMaxParamLength);
			}
		}

		for (uIndex = uParamNum; uIndex < g_uMaxActionParams; uIndex++)
		{
			m_lblActionParams[uIndex].ShowWindow(SW_HIDE);
			m_cmbActionParams[uIndex].ShowWindow(SW_HIDE);
			m_cmbActionParams[uIndex].ResetContent();
		}

		if (!bNSPresent)
			m_cmbActionNetParam.ShowWindow(SW_HIDE);

		if (!bSDPresent)
			m_cmbActionSndParam.ShowWindow(SW_HIDE);
		
		m_cmbActionNetParam.ResetContent();
		// m_cmbActionSndParam.ResetContent();	// we don't refill the sound combo box everytime, once is enough!
		
		GetDlgItem(IDC_LBLAP)->ShowWindow(uParamNum ? SW_SHOWNOACTIVATE : SW_HIDE);
	}
}


BOOL CEditRule::bFillParamsFromRule(BOOL bEvents, BOOL bActions)
{
	CCEvent*	pEvent = m_pRule->GetEvent();
	CCAction*	pAction = m_pRule->GetAction();

	if (pEvent)
	{
		ASSERT(pAction, "pAction is NULL in CEditRule::bFillParamsFromRule");

		UINT	uIndex, uIndexKey, uKeyParam, uKeyParamBit;

		if (bEvents)
		{
			for (uIndex = 0; uIndex < pEvent->GetParamNum(); uIndex++)
			{
				enumParamType ptEventParam = pEvent->GetParamType(uIndex);
				if (ptServerName == ptEventParam)
					m_cmbEventNetParam.SetWindowText(m_rgstrEventParams[ptEventParam]);
				else
					m_cmbEventParams[uIndex].SetWindowText(m_rgstrEventParams[ptEventParam]);
				uKeyParamBit = 0x1;
				uKeyParam = pEvent->GetKeyParam(uIndex);
				for (uIndexKey = 0; uIndexKey < (UINT) kepMax; uIndexKey++)
				{
					if (uKeyParam & uKeyParamBit)
						if (ptServerName == ptEventParam)
							m_cmbEventNetParam.AddString(theApp.m_rulesData.GetKeyEventParam((enumKeyEventParam) uIndexKey));
						else
							m_cmbEventParams[uIndex].AddString(theApp.m_rulesData.GetKeyEventParam((enumKeyEventParam) uIndexKey));
					uKeyParamBit <<= 1;
				}

				if (ptServerName == ptEventParam)
					m_cmbEventNetParam.Fill();
			}
		}

		if (bActions)
		{
			DWORD dwActionKeysExposed = pEvent->GetActionKeysExposed();
			for (uIndex = 0; uIndex < pAction->GetParamNum(); uIndex++)
			{
				enumParamType ptActionParam = pAction->GetParamType(uIndex);
				if (RTFParam(ptActionParam, pAction->GetID()) && m_prgdwActionParamFormatting[ptActionParam])
				{
					ASSERT(m_cmbActionParams[uIndex].GetRtfCmbEdit(), "m_cmbActionParams[uIndex].GetRtfCmbEdit() is NULL in CEditRule::bFillParamsFromRule");
					m_cmbActionParams[uIndex].GetRtfCmbEdit()->bSetWindowFormattedText(m_rgstrActionParams[ptActionParam], m_prgdwActionParamFormatting[ptActionParam]);
					m_cmbActionParams[uIndex].GetRtfCmbEdit()->SetSel(0, 0);
				}
				else
					switch (ptActionParam)
					{
						case ptServerName:
							m_cmbActionNetParam.SetWindowText(m_rgstrActionParams[ptActionParam]);
							break;
						case ptSoundFileName:
							m_cmbActionSndParam.SetWindowText(m_rgstrActionParams[ptActionParam]);
							break;
						default:
							m_cmbActionParams[uIndex].SetWindowText(m_rgstrActionParams[ptActionParam]);
					}

				switch (ptActionParam)
				{
					case ptHighlight:
					{
						CString strType;

						for (UINT uHighlights = 1; uHighlights <= (NHIGHLIGHTEDFONTS/2); uHighlights++)
						{
							strType.Format(IDS_HIGHLIGHT_TYPE, uHighlights);
							m_cmbActionParams[uIndex].AddString(strType);
						}
						break;
					}
					case ptMacroName:
					{
						for (INT iMacroNum = 0; iMacroNum < NMACROS; iMacroNum++)
							if (theApp.m_macros[iMacroNum].m_bDefined)
								m_cmbActionParams[uIndex].AddString(theApp.m_macros[iMacroNum].m_strName);
						break;
					}
					case ptRuleSetName:
					{
						ASSERT(m_pDynaRules, "m_pDynaRules is NULL in CEditRule::bFillParamsFromRule");
						INT			iRuleSets = m_pDynaRules->GetRuleSetsArray().GetSize(), iIndex;
						CCRuleSet*	pRuleSet;

						for (iIndex = 0; iIndex < iRuleSets; iIndex++)
						{
							pRuleSet = (CCRuleSet*) m_pDynaRules->GetRuleSetsArray().GetAt(iIndex);
							ASSERT(pRuleSet, "pRuleSet is NULL in CEditRule::bFillParamsFromRule");
							m_cmbActionParams[uIndex].AddString(pRuleSet->GetName());
						}
						break;
					}
					case ptSoundFileName:
					{
						if (!m_cmbActionSndParam.GetFilled())
						{
							FILEENUMSTRUCT fileenum;
							fileenum.pfnAdd = AddSoundToComboBox;
							fileenum.lParam = (LPARAM) &m_cmbActionSndParam;
							fileenum.bRecursive = FALSE;
							EnumSounds(theApp.m_soundPath, &fileenum);
							m_cmbActionSndParam.SetFilled(TRUE);
						}
						break;
					}
					case ptTextFileName:
					{
						CHAR			szCurDir[_MAX_PATH];
						FILEENUMSTRUCT	fileenum;
						fileenum.pszTypes = "txt\0";
						fileenum.pfnAdd = AddTextFileToComboBox;
						fileenum.lParam = (LPARAM) &m_cmbActionParams[uIndex];
						fileenum.bRecursive = TRUE;
						EnumFiles(theApp.m_strBaseDir, &fileenum);
						break;
					}
					default:
					{
						uKeyParamBit = 0x1;
						uKeyParam = pAction->GetKeyParam(uIndex);
						for (uIndexKey = 0; uIndexKey < (UINT) kapMax; uIndexKey++)
						{
							if ((uKeyParam & uKeyParamBit) && (dwActionKeysExposed & uKeyParamBit))
							{
								if (ptServerName == ptActionParam)
									m_cmbActionNetParam.AddString(theApp.m_rulesData.GetKeyActionParam((enumKeyActionParam) uIndexKey));
								else
									m_cmbActionParams[uIndex].AddString(theApp.m_rulesData.GetKeyActionParam((enumKeyActionParam) uIndexKey));
							}
							uKeyParamBit <<= 1;
						}
					}

					if (ptServerName == ptActionParam)
						m_cmbActionNetParam.Fill();
				}
			}
			BOOL bDelayOK = pAction->GetDelayOK();

			m_editDelay.EnableWindow(bDelayOK);
			m_spinDelay.EnableWindow(bDelayOK);

			if (!bDelayOK)
			{
				m_uMinDelay = m_uDelay = 0;
				m_editDelay.SetWindowText("0");
			}
			else
			{
				// treat potential exception
				ASSERT(pAction, "pAction is NULL in CEditRule::bFillParamsFromRule");

				DWORD	dwCurrentRange = m_spinDelay.GetRange();
				BOOL	bException = FALSE;
				RULEX	ex;
				CString strDelay;

				m_editDelay.GetWindowText(strDelay);
				m_uDelay = atoi(strDelay);
				for (UINT uException = 0; uException < g_uExceptionCount; uException++)
				{
					ex = g_rgex[uException];
					if (ex.ex == etMinDelay && ex.eID == pEvent->GetID() && ex.aID == pAction->GetID())
					{
						if ((m_uMinDelay = ex.dwValue) > m_uDelay)
						{
							m_uDelay = ex.dwValue; 
							strDelay.Format("%d", m_uDelay);
							m_editDelay.SetWindowText(strDelay);
						}
						m_spinDelay.SetRange(m_uMinDelay, 255);
						bException = TRUE;
						break;
					}
				}
				if (!bException && 0 != HIWORD(dwCurrentRange))
				{
					m_spinDelay.SetRange(m_uMinDelay = 0, 255);
					bException = TRUE;
				}
				if (bException)
					UpdateData(FALSE /*bSaveAndValidate*/);
			}
		}
	}
	return TRUE;
}


BOOL CEditRule::bCorrectActionKeys()
{
	ASSERT(m_pRule->GetEvent(),  "m_pRule->GetEvent()  is NULL in CEditRule::bCorrectActionKeys");
	ASSERT(m_pRule->GetAction(), "m_pRule->GetAction() is NULL in CEditRule::bCorrectActionKeys");

	UINT	uIndex, uIndexKey, uKeyParam, uKeyParamBit;
	DWORD	dwActionKeysExposed = m_pRule->GetEvent()->GetActionKeysExposed();
	CString	strCurParam;

	for (uIndex = 0; uIndex < m_pRule->GetAction()->GetParamNum(); uIndex++)
	{
		switch (m_pRule->GetAction()->GetParamType(uIndex))
		{
		case ptHighlight:
		case ptMacroName:
		case ptRuleSetName:
		case ptSoundFileName:
		case ptServerName:
			break;

		default:
			if (!m_cmbActionParams[uIndex].bGetRtfMode())
				m_cmbActionParams[uIndex].GetWindowText(strCurParam);
			m_cmbActionParams[uIndex].ResetContent();
			if (!m_cmbActionParams[uIndex].bGetRtfMode())
				m_cmbActionParams[uIndex].SetWindowText(strCurParam);

			uKeyParamBit = 0x1;
			uKeyParam = m_pRule->GetAction()->GetKeyParam(uIndex);
			for (uIndexKey = 0; uIndexKey < (UINT) kapMax; uIndexKey++)
			{
				if ((uKeyParam & uKeyParamBit) && (dwActionKeysExposed & uKeyParamBit))
					m_cmbActionParams[uIndex].AddString(theApp.m_rulesData.GetKeyActionParam((enumKeyActionParam) uIndexKey));
				uKeyParamBit <<= 1;
			}
		}
	}

	return TRUE;
}


void CEditRule::SaveComboParams(BOOL bEvents, BOOL bActions)
{
	UINT iParam;

	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::SaveComboParams");

	if (bEvents)
	{
		ASSERT(m_pRule->GetEvent(), "m_pRule->GetEvent() is NULL in CEditRule::SaveComboParams");
		CCEvent* pEvent = (CCEvent*) m_pRule->GetEvent();
		for (iParam = 0; iParam < pEvent->GetParamNum(); iParam++)
		{
			enumParamType ptEventParam = pEvent->GetParamType(iParam);
			if (ptServerName == ptEventParam)
				m_cmbEventNetParam.GetWindowText(m_rgstrEventParams[ptEventParam]);
			else
				m_cmbEventParams[iParam].GetWindowText(m_rgstrEventParams[ptEventParam]);
		}
	}

	if (bActions)
	{
		ASSERT(m_pRule->GetAction(), "m_pRule->GetAction() is NULL in CEditRule::SaveComboParams");
		CCAction* pAction = (CCAction*) m_pRule->GetAction();
		for (iParam = 0; iParam < pAction->GetParamNum(); iParam++)
		{
			enumParamType ptActionParam = pAction->GetParamType(iParam);
			switch (ptActionParam)
			{
				case ptServerName:
					m_cmbActionNetParam.GetWindowText(m_rgstrActionParams[ptActionParam]);
					break;
				case ptSoundFileName:
					m_cmbActionSndParam.GetWindowText(m_rgstrActionParams[ptActionParam]);
					break;
				default:
					m_cmbActionParams[iParam].GetWindowText(m_rgstrActionParams[ptActionParam]);
			}
			if (RTFParam(ptActionParam, pAction->GetID()))
			{
				ASSERT(m_cmbActionParams[iParam].GetRtfCmbEdit(), "m_cmbActionParams[iParam].GetRtfCmbEdit() is NULL in CEditRule::SaveComboParams");
				FreeAndNullFormatting(&(m_prgdwActionParamFormatting[ptActionParam]));
				m_prgdwActionParamFormatting[ptActionParam] = PRGDWGetFormatting((CRichEditCtrl*) m_cmbActionParams[iParam].GetRtfCmbEdit(), m_cmbActionParams[iParam].GetRtfCmbEdit()->m_pFont, m_cmbActionParams[iParam].GetRtfCmbEdit()->m_crTextColor);
			}
		}
	}
}


void CEditRule::SaveRuleParams()
{
	ASSERT(m_pRule->GetEvent(), "m_pRule->GetEvent() is NULL in CEditRule::SaveRuleParams");
	ASSERT(m_pRule->GetAction(), "m_pRule->GetAction() is NULL in CEditRule::SaveRuleParams");

	CCEvent* pEvent = (CCEvent*) m_pRule->GetEvent();
	for (UINT iParam = 0; iParam < pEvent->GetParamNum(); iParam++)
		m_rgstrEventParams[pEvent->GetParamType(iParam)] = m_pRule->GetEventParam(iParam);

	CCAction* pAction = (CCAction*) m_pRule->GetAction();
	for (iParam = 0; iParam < pAction->GetParamNum(); iParam++)
	{
		enumParamType ptActionParam = pAction->GetParamType(iParam);
		m_rgstrActionParams[ptActionParam] = m_pRule->GetActionParam(iParam);
		if (ptActionParam == ptSoundFileName)
		{
			SHORT cbLen1 = m_rgstrActionParams[ptActionParam].GetLength(), cbLen2;

			LPCSTR * pSupportedSoundTypes = GetSupportedSoundTypes ();

			// Get rid of potential file extension
			for (SHORT i = 0; pSupportedSoundTypes[i]; i++)
			{
				cbLen2 = strlen(pSupportedSoundTypes[i]);
				if (cbLen1 > cbLen2 + 1 &&
					m_rgstrActionParams[ptActionParam].GetAt(cbLen1-cbLen2-1) == '.' &&
					!m_rgstrActionParams[ptActionParam].Right(cbLen2).CompareNoCase(pSupportedSoundTypes[i]))
				{
					m_rgstrActionParams[ptActionParam].SetAt(cbLen1-cbLen2-1, '\0');
					m_nSoundFileType = i;
				}
			}
		}
		else
			if (RTFParam(ptActionParam, pAction->GetID()))
			{
				ASSERT(!m_prgdwActionParamFormatting[ptActionParam], "m_prgdwActionParamFormatting[ptActionParam] is NOT NULL in CEditRule::SaveRuleParams");
				m_prgdwActionParamFormatting[ptActionParam] = CopyFormatting(m_pRule->GetMsgFormatting());
			}
	}
}


void CEditRule::UpdateAdvancedControls()
{
	ASSERT(m_pRule->GetEvent(), "m_pRule->GetEvent() is NULL in CEditRule::UpdateAdvancedControls");

	BOOL		bShow = FALSE;
	CCEvent*	pEvent = (CCEvent*) m_pRule->GetEvent();
	for (UINT iParam = 0; iParam < pEvent->GetParamNum(); iParam++)
		if (pEvent->GetParamType(iParam) == ptMessage)
		{
			bShow = TRUE;
			break;
		}

	m_btnAdvanced.ShowWindow(bShow ? SW_SHOWNOACTIVATE : SW_HIDE);
	m_btnAdvanced.EnableWindow(bShow);
}


/////////////////////////////////////////////////////////////////////////////
// CEditRule message handlers

BOOL CEditRule::OnInitDialog() 
{
	INT			iIndex;
	UINT		uIndex;
	CCEvent*	pEvent;

	CCSDialog::OnInitDialog();

	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnInitDialog");

	m_uDelay = m_pRule->GetDelay();
	CString strDelay;
	strDelay.Format("%d", m_uDelay);
	m_editDelay.SetWindowText(strDelay);
	m_spinDelay.SetRange(0, 255);
	m_editDelay.SetLimitText(3);

	m_cmbEventNetParam.ReplaceControl(this, IDC_CMBEPNS);
	m_cmbEventNetParam.SetFont(&theApp.m_fontGui);
	m_cmbEventNetParam.SetServiceList(&theApp.m_listChatServices);

	m_cmbActionNetParam.ReplaceControl(this, IDC_CMBAPNS);
	m_cmbActionNetParam.SetFont(&theApp.m_fontGui);
	m_cmbActionNetParam.SetServiceList(&theApp.m_listChatServices);

	m_cmbActionSndParam.ReplaceControl(this, IDC_CMBAPSD, WS_VSCROLL);
	m_cmbActionSndParam.SetFont(&theApp.m_fontGui);

	for (uIndex = 0; uIndex < g_uMaxEventParams; uIndex++)
		m_cmbEventParams[uIndex].LimitText(g_uMaxParamLength);
	for (uIndex = 0; uIndex < g_uMaxActionParams; uIndex++)
		m_cmbActionParams[uIndex].LimitText(g_uMaxParamLength);

	// Fill in the events combo box, in a sorted way
	for (uIndex = 0; uIndex < (UINT) eMax; uIndex++)
	{
		pEvent = theApp.m_rulesData.GetEvent(uIndex);
		if ((iIndex = m_cmbEvents.AddString(pEvent->GetLongDesc())) >= 0)
		{
			m_cmbEvents.SetItemDataPtr(iIndex, pEvent);
			if (pEvent == m_pRule->GetEvent())
				m_cmbEvents.SetCurSel(iIndex);
		}
		else
		{
			m_cmbEvents.ResetContent();
			return FALSE;
		}
	}

	if (!m_pRule->GetEvent())
	{
		m_cmbEvents.SetCurSel(0);	// Select first item for rule creation case
		m_pRule->SetEvent((CCEvent*) m_cmbEvents.GetItemDataPtr(0));
	}

	m_chkSubRules.SetCheck(m_pRule->wGetFlags() & g_wNoSubsequent);

	// Fill in the actions combo box.
	bFillActionsFromEvent(NULL);

	SaveRuleParams();

	// Fill the parameter labels
	FillParamLabels(TRUE /* bEvents */, TRUE /* bActions */);

	bFillParamsFromRule(TRUE /* bEvents */, TRUE /* bActions */);

	// Update matching check boxes
	UpdateAdvancedControls();

	GotoDlgCtrl(&m_cmbEvents);
	return FALSE;  // return TRUE unless you set the focus to a control
}


void CEditRule::OnOK()
{
	UINT	uIndex, uKeyParamBit, uErrorIDS;
	CString	strParam;

	CCEvent*	pEvent = m_pRule->GetEvent();
	CCAction*	pAction = m_pRule->GetAction();

	ASSERT(pEvent, "pEvent is NULL in CEditRule::OnOK");
	ASSERT(pAction, "pAction is NULL in CEditRule::OnOK");

	// Check for event param validity
	for (uIndex = 0; uIndex < pEvent->GetParamNum(); uIndex++)
	{
		if (ptServerName == pEvent->GetParamType(uIndex))
			m_cmbEventNetParam.GetWindowText(strParam);
		else
			m_cmbEventParams[uIndex].GetWindowText(strParam);
		if (strParam.IsEmpty())
		{
			AfxMessageBox(theApp.m_rulesData.GetMissingEventParamError(pEvent->GetParamType(uIndex)));
			return;
		}
		else
		{
			m_pRule->SetEventKeyParam(uIndex, kepMax);
			uKeyParamBit = 0x1;
			for (UINT uKep = kepAny; uKep < (UINT) kepMax; uKep++)
			{
				if ((pEvent->GetKeyParam(uIndex) & uKeyParamBit) && 0 == theApp.m_rulesData.GetKeyEventParam((enumKeyEventParam) uKep).CompareNoCase(strParam))
				{
					m_pRule->SetEventKeyParam(uIndex, (enumKeyEventParam) uKep);
					m_pRule->SetEventParam(uIndex, theApp.m_rulesData.GetKeyEventParam((enumKeyEventParam) uKep));
					break;
				}
				uKeyParamBit <<= 1;
			}
			if (kepMax == m_pRule->GetEventKeyParam(uIndex))
			{
				m_pRule->SetEventParam(uIndex, strParam);
				if (!m_pRule->bValidateRuleEvent(uIndex, strParam, &uErrorIDS))
				{
					ASSERT(uErrorIDS, "uErrorIDS is 0 in CEditRule::OnOK");
					AfxMessageBox(uErrorIDS);
					return;
				}
			}
		}
	}

	for (uIndex = 0; uIndex < pAction->GetParamNum(); uIndex++)
	{
		switch (pAction->GetParamType(uIndex))
		{
			case ptServerName:
				m_cmbActionNetParam.GetWindowText(strParam);
				break;
			case ptSoundFileName:
			{
				INT		iCurSel;
				SHORT	nType = m_nSoundFileType;

				// Set the current selection file type
				if (CB_ERR != (iCurSel = m_cmbActionSndParam.GetCurSel()))
					nType = m_cmbActionSndParam.GetItemData(iCurSel);

				ASSERT(nType < SOUNDTYPES, "nType out of range in CEditRule::OnOK");

				m_cmbActionSndParam.GetWindowText(strParam);

				strParam += '.';
				strParam += GetSupportedSoundTypes ()[nType];
				break;
			}
			default:
				m_cmbActionParams[uIndex].GetWindowText(strParam);
		}

		// Only the message accompanying a sound can be empty
		if (strParam.IsEmpty() && !(pAction->GetID() == aSendSound && pAction->GetParamType(uIndex) == ptMessage))
		{
			AfxMessageBox(theApp.m_rulesData.GetMissingActionParamError(pAction->GetParamType(uIndex)));
			return;
		}
		else
		{
			m_pRule->SetActionKeyParam(uIndex, kapMax);
			for (UINT uKap = 0; uKap < (UINT) kapMax; uKap++)
				if (0 == theApp.m_rulesData.GetKeyActionParam((enumKeyActionParam) uKap).CompareNoCase(strParam))
				{
					m_pRule->SetActionKeyParam(uIndex, (enumKeyActionParam) uKap);
					m_pRule->SetActionParam(uIndex, theApp.m_rulesData.GetKeyActionParam((enumKeyActionParam) uKap));
					break;
				}

			if (kapMax == m_pRule->GetActionKeyParam(uIndex))
			{
				m_pRule->SetActionParam(uIndex, strParam);
				if (!m_pRule->bValidateRuleAction(uIndex, strParam, &uErrorIDS))
				{
					ASSERT(uErrorIDS, "uErrorIDS is 0 in CEditRule::OnOK");
					AfxMessageBox(uErrorIDS);
					return;
				}
			}

			if (RTFParam(pAction->GetParamType(uIndex), pAction->GetID()))
			{
				ASSERT(m_cmbActionParams[uIndex].GetRtfCmbEdit(), "m_cmbActionParams[uIndex].GetRtfCmbEdit() is NULL in CEditRule::OnOK");
				CDWordArray* prgdwMsgFormatting = PRGDWGetFormatting((CRichEditCtrl*) m_cmbActionParams[uIndex].GetRtfCmbEdit(), m_cmbActionParams[uIndex].GetRtfCmbEdit()->m_pFont, m_cmbActionParams[uIndex].GetRtfCmbEdit()->m_crTextColor);
				m_pRule->SetMsgFormatting(prgdwMsgFormatting, FALSE /*bMakeCopy*/);
			}
		}
	}

	if (m_pRule->GetDaemonExt())
		m_pRule->GetDaemonExt()->SetResetItemLists(TRUE);

	CCSDialog::OnOK();
}


void CEditRule::OnEventChanged()
{
	UINT	uIndex = m_cmbEvents.GetCurSel();
	BOOL	bActionChanged;
		
	ASSERT(CB_ERR != uIndex, "uIndex == CB_ERR in CEditRule::OnEventChanged");
	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnEventChanged");

	SaveComboParams(TRUE /* bEvents */, TRUE /* bActions */);

	m_pRule->SetEvent((CCEvent*) m_cmbEvents.GetItemDataPtr(uIndex));

	UpdateAdvancedControls();

	// Correct Action if needed
	bFillActionsFromEvent(&bActionChanged);

	FillParamLabels(TRUE /* bEvents */, bActionChanged /* bActions */);

	bFillParamsFromRule(TRUE /* bEvents */, bActionChanged /*bActions */);

	if (!bActionChanged)
		bCorrectActionKeys();
}


void CEditRule::OnActionChanged()
{
	UINT	uIndex = m_cmbActions.GetCurSel();
		
	ASSERT(CB_ERR != uIndex, "uIndex == CB_ERR in CEditRule::OnActionChanged");
	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnActionChanged");

	SaveComboParams(FALSE /* bEvents */, TRUE /* bActions */);

	m_pRule->SetAction((CCAction*) m_cmbActions.GetItemDataPtr(uIndex));

	FillParamLabels(FALSE /* bEvents */, TRUE /* bActions */);

	bFillParamsFromRule(FALSE /* bEvents */, TRUE /*bActions */);
}


void CEditRule::OnEventParamSetFocus(UINT uID)
{
	INT iIndex = (uID - IDC_CMBEP0) / 2;

	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnEventParamSetFocus");
	ASSERT(m_pRule->GetEvent(), "m_pRule->GetEvent() is NULL in CEditRule::OnEventParamSetFocus");
	ASSERT(iIndex >= 0 && iIndex < m_pRule->GetEvent()->GetParamNum(), "iIndex out of range in CEditRule::OnEventParamSetFocus");

	m_lblParamDesc.SetWindowText(m_pRule->GetEvent()->GetParamDesc(iIndex));
	UpdateAdvancedControls();
}


void CEditRule::OnEventNetParamSetFocus()
{
	CCEvent* pEvent = m_pRule->GetEvent();

	ASSERT(pEvent, "pEvent is NULL in CEditRule::OnEventNetParamSetFocus");

	for (INT iIndex = 0; iIndex < pEvent->GetParamNum(); iIndex++)
		if (ptServerName == pEvent->GetParamType(iIndex))
			break;

	m_lblParamDesc.SetWindowText(pEvent->GetParamDesc(iIndex));
}


void CEditRule::OnEventParamKillFocus(UINT uID)
{
	INT iIndex = (uID - IDC_CMBEP0) / 2;

	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnEventParamKillFocus");
	ASSERT(m_pRule->GetEvent(), "m_pRule->GetEvent() is NULL in CEditRule::OnEventParamKillFocus");
	ASSERT(iIndex >= 0 && iIndex < m_pRule->GetEvent()->GetParamNum(), "iIndex out of range in CEditRule::OnEventParamKillFocus");

	m_lblParamDesc.SetWindowText("");
}


void CEditRule::OnActionParamSetFocus(UINT uID)
{
	// OutputDebugString("CEditRule::OnActionParamSetFocus - Enter\n");

	INT iIndex = (uID - IDC_CMBAP0) / 2;

	if (iIndex < 0 || iIndex >= m_pRule->GetAction()->GetParamNum())
		iIndex = uID - IDC_RTFAP0;

	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnActionParamSetFocus");
	ASSERT(m_pRule->GetAction(), "m_pRule->GetAction() is NULL in CEditRule::OnActionParamSetFocus");
	ASSERT(iIndex >= 0 && iIndex < m_pRule->GetAction()->GetParamNum(), "iIndex out of range in CEditRule::OnActionParamSetFocus");

	m_lblParamDesc.SetWindowText(m_pRule->GetAction()->GetParamDesc(iIndex));
}


void CEditRule::OnActionNetParamSetFocus()
{
	CCAction* pAction = m_pRule->GetAction();

	ASSERT(pAction, "pAction is NULL in CEditRule::OnActionNetParamSetFocus");

	for (INT iIndex = 0; iIndex < pAction->GetParamNum(); iIndex++)
		if (ptServerName == pAction->GetParamType(iIndex))
			break;

	m_lblParamDesc.SetWindowText(pAction->GetParamDesc(iIndex));
}


void CEditRule::OnActionSndParamSetFocus()
{
	CCAction* pAction = m_pRule->GetAction();

	ASSERT(pAction, "pAction is NULL in CEditRule::OnActionSndParamSetFocus");

	for (INT iIndex = 0; iIndex < pAction->GetParamNum(); iIndex++)
		if (ptSoundFileName == pAction->GetParamType(iIndex))
			break;

	m_lblParamDesc.SetWindowText(pAction->GetParamDesc(iIndex));
}


//void CEditRule::OnActionSndParamKillFocus()
//{
//	INT iCurSel;
//
//	// Set the current selection file type
//	if (CB_ERR != (iCurSel = m_cmbActionSndParam.GetCurSel()))
//		m_cmbActionSndParam.SetSelType(m_cmbActionSndParam.GetItemData(iCurSel));
//	else
//		m_cmbActionSndParam.SetSelType(0);
//
//	m_lblParamDesc.SetWindowText("");
//}


void CEditRule::OnActionParamKillFocus(UINT uID)
{
	BOOL	bTab = GetKeyState(VK_TAB) & 0x8000;
	HWND	hwnd = ::GetFocus();

	#ifdef DEBUG
		INT	iIndex = (uID - IDC_CMBAP0) / 2;

		if (iIndex < 0 || iIndex >= m_pRule->GetAction()->GetParamNum())
			iIndex = uID - IDC_RTFAP0;

		ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnActionParamKillFocus");
		ASSERT(m_pRule->GetAction(), "m_pRule->GetAction() is NULL in CEditRule::OnActionParamKillFocus");
		ASSERT(iIndex >= 0 && iIndex < m_pRule->GetAction()->GetParamNum(), "iIndex out of range in CEditRule::OnActionParamKillFocus");
	#endif // DEBUG

	if (bTab)
		for (INT iParam = 0; iParam < m_pRule->GetAction()->GetParamNum(); iParam++)
			if (m_cmbActionParams[iParam].GetRtfCmbEdit() && 
				m_cmbActionParams[iParam].GetRtfCmbEdit()->m_hWnd == hwnd)
				return;

	m_lblParamDesc.SetWindowText("");
}


void CEditRule::OnEventsCmbSetFocus()
{
	m_lblParamDesc.SetWindowText(theApp.m_rulesData.GetEventsDesc());
}


void CEditRule::OnActionsCmbSetFocus()
{
	m_lblParamDesc.SetWindowText(theApp.m_rulesData.GetActionsDesc());
}


void CEditRule::OnKillFocusClear()
{
	m_lblParamDesc.SetWindowText("");
}


void CEditRule::OnActionParamFilter(UINT uID, NMHDR *pNotifyStruct, LRESULT *plResult)
{
	MSGFILTER*	pMSGFILTER = (MSGFILTER*) pNotifyStruct;

	ASSERT(pNotifyStruct, "pNotifyStruct is NULL in CEditRule::OnActionParamFilter");

	if (pNotifyStruct->code == EN_MSGFILTER &&
		pMSGFILTER->msg == WM_RBUTTONDOWN &&
		m_cmbActionParams[uID - IDC_RTFAP0].GetRtfCmbEdit())
		m_cmbActionParams[uID - IDC_RTFAP0].GetRtfCmbEdit()->ShowFormattingPopUp(LOWORD(pMSGFILTER->lParam), HIWORD(pMSGFILTER->lParam));

	*plResult = 0L;
}


void CEditRule::OnFlagsCheckClick()
{
	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnFlagsCheckClick");

	WORD wFlags = m_pRule->wGetFlags();

	if (m_chkSubRules.GetCheck())
		wFlags |= g_wNoSubsequent;
	else
		wFlags &= ~g_wNoSubsequent;

	m_pRule->SetFlags(wFlags);
}


void CEditRule::OnAdvancedClick()
{
	ASSERT(m_pRule, "m_pRule is NULL in CEditRule::OnAdvancedClick");

	WORD					wOldFlags = m_pRule->wGetFlags();
	CAdvancedEventParams	aepDlg(wOldFlags & (g_wMatchCase | g_wMatchWord));

	if (theApp.DoModalDlg(&aepDlg) == IDOK)
	{
		wOldFlags &= ~(g_wMatchCase | g_wMatchWord);
		if (aepDlg.m_iMatchCase)
			wOldFlags |= g_wMatchCase;
		if (aepDlg.m_iMatchWord)
			wOldFlags |= g_wMatchWord;
		m_pRule->SetFlags(wOldFlags);
	}
}


/*
UINT CEditRule::OnGetDlgCode()
{
	return CCSDialog::OnGetDlgCode() | DLGC_WANTTAB;
}


void CEditRule::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	if (VK_TAB == nChar)
	{
		OutputDebugString("CEditRule::OnKeyDown - Tab\n");
		CCSDialog::OnKeyDown(nChar, nRepCnt, nFlags);
	}
	else
		CCSDialog::OnKeyDown(nChar, nRepCnt, nFlags);
}
*/

const DWORD CEditRule::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CAddToSets dialog

CAddToSets::CAddToSets(CCDynaRules* pDynaCopy, CCRuleSet* pRuleSet, CCRule* pRule, CWnd* pwndParent /*=NULL*/)
	: CCSDialog(CAddToSets::IDD, pwndParent)
{
	//{{AFX_DATA_INIT(CAddToSets)
	//}}AFX_DATA_INIT
	ASSERT(pDynaCopy, "pDynaCopy is NULL CAddToSets::CAddToSets");
	ASSERT(pRuleSet, "pRuleSet is NULL CAddToSets::CAddToSets");
	ASSERT(pRule, "pRule is NULL CAddToSets::CAddToSets");

	m_pRule		= pRule;
	m_pRuleSet	= pRuleSet;
	m_pDynaCopy	= pDynaCopy;
	m_bRuleAdded= FALSE;
}


//BEGIN_MESSAGE_MAP(CAddToSets, CCSDialog)
	//{{AFX_MSG_MAP(CAddToSets)
	//}}AFX_MSG_MAP
//END_MESSAGE_MAP()


void CAddToSets::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAddToSets)
	DDX_Control(pDX, IDC_ALSTRULES, m_lstRule);
	DDX_Control(pDX, IDC_ALSTSETS,  m_lstSets);
	//}}AFX_DATA_MAP
}


BOOL CAddToSets::bFillRuleSets()
{
	BOOL		bRet = TRUE;
	INT			iRuleSets, iIndex;
	CCRuleSet*	pRuleSet;
	CPtrArray*	pRuleSetsArray;

	pRuleSetsArray = &(m_pDynaCopy->GetRuleSetsArray());
	iRuleSets = pRuleSetsArray->GetSize();

	for (INT iIndexSet = 0; iIndexSet < iRuleSets && bRet; iIndexSet++)
	{
		pRuleSet = (CCRuleSet*) pRuleSetsArray->GetAt(iIndexSet);
		ASSERT(pRuleSet, "pRuleSet is NULL in CAddToSets::bFillRuleSets");
		ASSERT(!pRuleSet->GetName().IsEmpty(), "pRuleSet->GetName().IsEmpty() in CAddToSets::bFillRuleSets");

		if (pRuleSet != m_pRuleSet)
		{
			if ((iIndex = m_lstSets.AddString(pRuleSet->GetName())) >= 0)
				m_lstSets.SetItemDataPtr(iIndex, (void*) pRuleSet);
			else
			{
				ASSERT(FALSE, "AddString failed in CAddToSets::bFillRuleSets");
				bRet = FALSE;
			}
		}
	}

	ASSERT(bRet, "bRet is FALSE in CAddToSets::bFillRuleSets");

	return bRet;
}


BOOL CAddToSets::bAddToSets()
{
	INT iIndex, iSets = m_lstSets.GetCount(), iSelected, *prgIndex;
	CCRuleSet*	pRuleSet;
	CCRule*		pRule;
	BOOL		bRet = FALSE;

	ASSERT(m_pRule, "m_pRule is NULL in CAddToSets::bAddToSets");

	if (!iSets || !(prgIndex = new INT[iSets]))
		return FALSE;

	iSelected = m_lstSets.GetSelItems(iSets, prgIndex);
	for (iIndex = 0; iIndex < iSelected; iIndex++)
	{
		ASSERT(prgIndex[iIndex] >= 0 && prgIndex[iIndex] < iSets, "prgIndex[iIndex] out of range in CAddToSets::bAddToSets");
		pRuleSet = (CCRuleSet*) m_lstSets.GetItemDataPtr(prgIndex[iIndex]);
		ASSERT((LONG) pRuleSet != -1, "pRuleSet == -1 in CAddToSets::bAddToSets");
		ASSERT(!pRuleSet->GetName().IsEmpty(), "pRuleSet->GetName().IsEmpty() in CAddToSets::bAddToSets");

		if (pRule = new CCRule(m_pRule, pRuleSet->GetDynaRules()))
		{
			bRet |= pRuleSet->bAddRule(pRule);
			ASSERT(bRet, "bAddRule failed in CAddToSets::bAddToSets");
			// pRuleSet->SetModified();
		}
	}

	delete [] prgIndex;

	return bRet;
}


BOOL CAddToSets::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	
	CString strLabel;
	CString strWidth;

	m_lstRule.SetFont(&theApp.m_fontGui);

	strLabel.LoadString(IDS_EVENTS_LABEL);
	strWidth.LoadString(IDS_EVENTS_WIDTH);
	m_lstRule.InsertColumn(0, strLabel, LVCFMT_LEFT, atoi(strWidth));

	strLabel.LoadString(IDS_ACTIONS_LABEL);
	strWidth.LoadString(IDS_ACTIONS_WIDTH);
	m_lstRule.InsertColumn(1, strLabel, LVCFMT_LEFT, atoi(strWidth), 1);

	m_strEventDisplay = m_pRule->StrGetEventDisplay();
	m_strActionDisplay = m_pRule->StrGetActionDisplay();

	LV_ITEM item;

	item.iItem = 0;
	item.iSubItem = 0;
	item.mask = LVIF_TEXT;
	item.pszText = (LPTSTR) (LPCTSTR) m_strEventDisplay;
	m_lstRule.InsertItem(&item);

	item.iSubItem = 1;
	item.pszText = (LPTSTR) (LPCTSTR) m_strActionDisplay;
	m_lstRule.SetItem(&item);

	bFillRuleSets();
	GotoDlgCtrl(&m_lstSets);

	return FALSE;  // return TRUE unless you set the focus to a control
}


void CAddToSets::OnOK()
{
	m_bRuleAdded = bAddToSets();

	CCSDialog::OnOK();
}


const DWORD CAddToSets::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CSetNameConflict dialog

CSetNameConflict::CSetNameConflict(CString strSetName, CRuleSetsListBox* plstSets, CWnd* pwndParent /*=NULL*/)
	: CCSDialog(CSetNameConflict::IDD, pwndParent)
{
	//{{AFX_DATA_INIT(CSetNameConflict)
	//}}AFX_DATA_INIT
	ASSERT(!strSetName.IsEmpty(), "strSetName.IsEmpty() in CSetNameConflict::CSetNameConflict");
	ASSERT(plstSets, "plstSets is NULL in CSetNameConflict::CSetNameConflict");

	m_editSetName.SetFilter(FILTEREDIT_NOCHARS, "/\\:*?\"<>|");
	m_strSetName = strSetName;
	m_plstSets = plstSets;
}

BEGIN_MESSAGE_MAP(CSetNameConflict, CCSDialog)
	//{{AFX_MSG_MAP(CSetNameConflict)
	ON_BN_CLICKED(IDC_BTNOVERWRITERULESET, OnOverwriteRuleSetClick)
	ON_BN_CLICKED(IDC_BTNRENAMELOADEDRULESET, OnRenameRuleSetClick)
	ON_EN_CHANGE(IDC_RENAMEDRULESET, OnRenamedRuleSetChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CSetNameConflict::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSetNameConflict)
	DDX_Control(pDX, IDC_RENAMEDRULESET, m_editSetName);
	DDX_Text(pDX, IDC_RENAMEDRULESET, m_strSetName);
	DDV_MaxChars(pDX, m_strSetName, g_uMaxSetNameLength);
	//}}AFX_DATA_MAP
}


BOOL CSetNameConflict::OnInitDialog() 
{
	CCSDialog::OnInitDialog();
	
	CString strConflict, strFmt;
	strFmt.LoadString(IDS_ERR_CRS_CONFLICT);
	strConflict.Format(strFmt, m_strSetName);

	GetDlgItem(IDC_RULESETCONFLICT)->SetWindowText(strConflict);

	return TRUE;  // return TRUE unless you set the focus to a control
}


void CSetNameConflict::OnOverwriteRuleSetClick()
{
	EndDialog(IDOVERWRITE);
}


void CSetNameConflict::OnRenameRuleSetClick()
{
	CString	strSetName;
	GetDlgItem(IDC_RENAMEDRULESET)->GetWindowText(strSetName);
	strSetName.TrimLeft();
	strSetName.TrimRight();
	ASSERT(!strSetName.IsEmpty(), "strSetName.IsEmpty() in CSetNameConflict::OnRenameRuleSetClick");
	if (LB_ERR != m_plstSets->FindStringExact(-1, (LPCTSTR) strSetName))
	{
		CString strConflict, strFmt;
		strFmt.LoadString(IDS_ERR_CRS_CONFLICT2);
		strConflict.Format(strFmt, strSetName);
		AfxMessageBox (strConflict, MB_OK | MB_ICONEXCLAMATION);
		return;
	}
	m_strSetName = strSetName;

	EndDialog(IDRENAME);
}


void CSetNameConflict::OnRenamedRuleSetChanged()
{
	CString	strSetName;
	GetDlgItem(IDC_RENAMEDRULESET)->GetWindowText(strSetName);
	strSetName.TrimLeft();
	strSetName.TrimRight();
	GetDlgItem(IDC_BTNRENAMELOADEDRULESET)->EnableWindow(!strSetName.IsEmpty());
}


const DWORD CSetNameConflict::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CCreateSet dialog
CCreateSet::CCreateSet(CRuleSetsListBox* plstSets, CWnd* pwndParent /*=NULL*/)
	: CCSDialog(CCreateSet::IDD, pwndParent)
{
	//{{AFX_DATA_INIT(CCreateSet)
	//}}AFX_DATA_INIT
	ASSERT(plstSets, "plstSets is NULL in CCreateSet::CCreateSet");

	m_editSetName.SetFilter(FILTEREDIT_NOCHARS, "/\\:*?\"<>|");
	m_plstSets = plstSets;
}


BEGIN_MESSAGE_MAP(CCreateSet, CCSDialog)
	//{{AFX_DATA_INIT(CCreateSet)
	ON_EN_CHANGE(IDC_CREATEDSET, OnCreatedRuleSetChanged)
	//}}AFX_DATA_INIT
END_MESSAGE_MAP()


void CCreateSet::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCreateSet)
	DDX_Control(pDX, IDC_CREATEDSET, m_editSetName);
	DDX_Text(pDX, IDC_CREATEDSET, m_strSetName);
	DDV_MaxChars(pDX, m_strSetName, g_uMaxSetNameLength);
	//}}AFX_DATA_MAP
}


void CCreateSet::OnCreatedRuleSetChanged()
{
	CString	strSetName;
	GetDlgItem(IDC_CREATEDSET)->GetWindowText(strSetName);
	strSetName.TrimLeft();
	strSetName.TrimRight();
	GetDlgItem(IDOK)->EnableWindow(!strSetName.IsEmpty());
}


void CCreateSet::OnOK()
{
	CString	strSetName;
	GetDlgItem(IDC_CREATEDSET)->GetWindowText(strSetName);
	strSetName.TrimLeft();
	strSetName.TrimRight();
	ASSERT(!strSetName.IsEmpty(), "strSetName.IsEmpty() in CCreateSet::OnOK");
	if (LB_ERR != m_plstSets->FindStringExact(-1, (LPCTSTR) strSetName))
	{
		CString strConflict, strFmt;
		strFmt.LoadString(IDS_ERR_CRS_CONFLICT2);
		strConflict.Format(strFmt, strSetName);
		AfxMessageBox (strConflict, MB_OK | MB_ICONEXCLAMATION);
		GotoDlgCtrl(GetDlgItem(IDC_CREATEDSET));
		return;
	}
	GetDlgItem(IDC_CREATEDSET)->SetWindowText(strSetName);
	CCSDialog::OnOK();
}


const DWORD CCreateSet::m_nHelpIDs[] =
{
	0, 0
};


/////////////////////////////////////////////////////////////////////////////
// CRenameSet dialog
CRenameSet::CRenameSet(CCRuleSet* pRuleSet, CRuleSetsListBox* plstSets, CWnd* pwndParent /*=NULL*/)
	: CCSDialog(CRenameSet::IDD, pwndParent)
{
	//{{AFX_DATA_INIT(CRenameSet)
	//}}AFX_DATA_INIT
	ASSERT(pRuleSet, "pRuleSet is NULL in CRenameSet::CRenameSet");
	ASSERT(plstSets, "plstSets is NULL in CRenameSet::CRenameSet");

	m_editSetName.SetFilter(FILTEREDIT_NOCHARS, "/\\:*?\"<>|");
	m_strSetName = pRuleSet->GetName();
	m_plstSets = plstSets;
	m_pRuleSet = pRuleSet;
}


BEGIN_MESSAGE_MAP(CRenameSet, CCSDialog)
	//{{AFX_MSG_MAP(CRenameSet)
	ON_EN_CHANGE(IDC_RENAMEDSET, OnRenamedRuleSetChanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void CRenameSet::DoDataExchange(CDataExchange* pDX)
{
	CCSDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CRenameSet)
	DDX_Control(pDX, IDC_RENAMEDSET, m_editSetName);
	DDX_Text(pDX, IDC_RENAMEDSET, m_strSetName);
	DDV_MaxChars(pDX, m_strSetName, g_uMaxSetNameLength);
	//}}AFX_DATA_MAP
}


void CRenameSet::OnOK()
{
	CString	strSetName;
	GetDlgItem(IDC_RENAMEDSET)->GetWindowText(strSetName);
	strSetName.TrimLeft();
	strSetName.TrimRight();
	if (LB_ERR != m_plstSets->FindStringExact(-1, (LPCTSTR) strSetName) && strSetName.CompareNoCase(m_strSetName))
	{
		CString strConflict, strFmt;
		strFmt.LoadString(IDS_ERR_CRS_CONFLICT2);
		strConflict.Format(strFmt, strSetName);
		AfxMessageBox (strConflict, MB_OK | MB_ICONEXCLAMATION);
		GotoDlgCtrl(GetDlgItem(IDC_RENAMEDSET));
		return;
	}

	if (strSetName.Compare(m_strSetName))
	{
		m_pRuleSet->SetName(strSetName);
		// m_pRuleSet->SetModified();
		EndDialog(IDRENAME);
	}
	else
		CCSDialog::OnOK();
}


void CRenameSet::OnRenamedRuleSetChanged()
{
	CString	strSetName;
	GetDlgItem(IDC_RENAMEDSET)->GetWindowText(strSetName);
	strSetName.TrimLeft();
	strSetName.TrimRight();
	GetDlgItem(IDOK)->EnableWindow(!strSetName.IsEmpty());
}


const DWORD CRenameSet::m_nHelpIDs[] =
{
	0, 0
};
