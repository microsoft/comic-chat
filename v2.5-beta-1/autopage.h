//=--------------------------------------------------------------------------=
// AutoPage.h : header file
//=--------------------------------------------------------------------------=
// Copyright  1998  Microsoft Corporation.  All Rights Reserved.
//=--------------------------------------------------------------------------=

// Created by RegisB on 01/20/98

#ifndef __AUTOPAGE_H__
#define __AUTOPAGE_H__

#include "chat.h"
#include "rtfcmb.h"
#include "sounddlg.h"
#include "resource.h"
#include "setupdlg.h"

/////////////////////////////////////////////////////////////////////////////
// Constants
const SHORT	g_nIconCount		= 2;
const SHORT	g_nInactiveIndex	= 0;
const SHORT	g_nActiveIndex		= 1;
const SHORT g_nIconMargin		= 2;
const SHORT	g_nIconWidth		= 16;
const SHORT	g_nIconHeight		= 15;

const INT	IDOVERWRITE			= 3;
const INT	IDRENAME			= 4;

/////////////////////////////////////////////////////////////////////////////
// CAutomationPage dialog

class CAutomationPage : public CCSPropertyPage
{
// Construction
public:
	CAutomationPage();
	~CAutomationPage() {};

	int		m_iGreetingType;
	CMacro	m_macros[NMACROS];
	BOOL	m_bOKing;
	BOOL	m_bSetActiveNeverCalled;

// Dialog Data
	//{{AFX_DATA(CAutomationPage)
	enum { IDD = IDD_AUTOMATION_PAGE };
	CRtfCtrl		m_rtfMacro;
	CRtfCtrl		m_rtfGreetingMesg;
	CEdit			m_macroNameCtl;
	CEdit			m_mesgCntCtl;
	CEdit			m_intervalCtl;
	CComboBox		m_keyCtl;
	CSpinButtonCtrl	m_spinMesgCnt;
	CSpinButtonCtrl	m_spinInterval;
	BOOL			m_bAutoIgnore;
	UINT			m_uMesgCnt;
	UINT			m_uInterval;
	CString			m_strMesgCnt;
	CString			m_strInterval;
	//}}AFX_DATA


// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CAutomationPage)
	public:
	virtual BOOL OnSetActive();
	virtual BOOL OnKillActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	void				OnFilterGreetingMesg(NMHDR *pNotifyStruct, LRESULT *plResult);
	void				OnFilterMacro(NMHDR *pNotifyStruct, LRESULT *plResult);
	static const  DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CAutomationPage)
	afx_msg void OnMesgCountChg();
	afx_msg void OnIntervalChg();
	afx_msg void OnAutoIgnore();
	afx_msg void OnNogreeting();
	afx_msg void OnSaygreeting();
	afx_msg void OnWhispergreeting();
	afx_msg void OnAddMacro();
	afx_msg void OnDeleteMacro();
	afx_msg void OnSelchangeKey();
	afx_msg void OnChangeGreetingMesg();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

class CRulesPage;

/////////////////////////////////////////////////////////////////////////////
// CRuleIcons class
class CRuleIcons
{
public:
	CRuleIcons();
	virtual	~CRuleIcons();

	HBITMAP		GetIcon(SHORT nIndex)	{ return m_hbmpIcon[nIndex]; }

protected:
	HBITMAP		m_hbmpIcon[g_nIconCount];
};


/*
/////////////////////////////////////////////////////////////////////////////
// CTabButton control
class CTabButton : public CButton
{
public:
	CTabButton() {};
	virtual ~CTabButton() {};

protected:
	//{{AFX_MSG(CTabButton)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CTabEdit control
class CTabEdit : public CEdit
{
public:
	CTabEdit() {};
	virtual ~CTabEdit() {};

protected:
	//{{AFX_MSG(CTabEdit)
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
*/


/////////////////////////////////////////////////////////////////////////////
// CRuleSetsListBox control
class CRuleSetsListBox : public CListBox
{

// Construction
public:
	CRuleSetsListBox() {}

// Operations

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRuleSetsListBox)
	virtual void	DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
	//}}AFX_VIRTUAL

// Implementation
	virtual			~CRuleSetsListBox() {};

	// Generated message map functions
protected:
	//{{AFX_MSG(CRuleSetsListBox)
	afx_msg void	OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void	OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	void			SwitchActivation(INT iIndex);

	DECLARE_MESSAGE_MAP()

	CRuleIcons	m_icons;
};


/////////////////////////////////////////////////////////////////////////////
// CRulesListCtrl control
class CRulesListCtrl : public CListCtrl
{

// Construction
public:
	CRulesListCtrl() { m_pRuleSet = NULL; }

// Attributes

// Operations
	BOOL	bFill(CCDynaRules* pDynaRules);
	BOOL	bAddRule(CCRule* pRule, INT iIndex = -1);
	INT		iGetSelectedRule(CCRule **ppRule);
	void	SwitchActivation(INT iIndex);

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRulesListCtrl)
	//}}AFX_VIRTUAL

// Implementation
	virtual ~CRulesListCtrl() {};

	// Generated message map functions
protected:
	//{{AFX_MSG(CRulesListCtrl)
	afx_msg void OnGetdispinfo(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

	CCRuleSet*	m_pRuleSet;
};


/////////////////////////////////////////////////////////////////////////////
// CAdvancedEventParams dialog

class CAdvancedEventParams : public CCSDialog
{
// Construction
public:
	CAdvancedEventParams(WORD wFlags, CWnd* pwndParent = NULL);
	~CAdvancedEventParams() {};

// Dialog Data
	//{{AFX_DATA(CAdvancedEventParams)
	enum { IDD = IDD_ADVANCEDEVENTPARAMS };
	INT	m_iMatchCase;
	INT	m_iMatchWord;
	//}}AFX_DATA

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvancedEventParams)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CAdvancedEventParams)
	//}}AFX_MSG
//	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CAdvancedRuleSettings dialog
class CAdvancedRuleSettings : public CCSDialog
{
// Construction
public:
	CAdvancedRuleSettings(UCHAR uOcc, UCHAR uInt, CWnd* pwndParent = NULL);
	~CAdvancedRuleSettings() {};

// Dialog Data
	//{{AFX_DATA(CAdvancedRuleSettings)
	enum { IDD = IDD_ADVANCEDRULESETTINGS };
	CSpinButtonCtrl	m_spinOcc;
	CSpinButtonCtrl	m_spinInt;
	CEdit			m_editOcc;
	CEdit			m_editInt;
	UCHAR			m_uOcc;
	UCHAR			m_uInt;
	//}}AFX_DATA

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAdvancedRuleSettings)
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}
};


/////////////////////////////////////////////////////////////////////////////
// CSoundComboBox control
class CSoundComboBox : public CIconicComboBox
{
public:
	CSoundComboBox();
	virtual ~CSoundComboBox();

	void			SetFilled(BOOL bFilled)	{ m_bFilled = bFilled; }
	BOOL			GetFilled() { return m_bFilled; }

protected:
	virtual HICON	GetIcon(UINT nIndex, LPCSTR pszString, DWORD dwItemData);
	virtual BOOL	ShouldDrawDivision(UINT nIndex, LPCSTR pszString, DWORD dwItemData) { return FALSE; }

	// Generated message map functions
	//{{AFX_MSG(CSoundComboBox)
//	afx_msg UINT	OnGetDlgCode();
//	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG

//	DECLARE_MESSAGE_MAP()

	// Private attributes
	HICON			m_hIcons[SOUNDTYPES];
	BOOL			m_bFilled;
};


/////////////////////////////////////////////////////////////////////////////
// CEditRule dialog
class CEditRule : public CCSDialog
{
// Construction
public:
	CEditRule(CWnd* pwndParent = NULL);
	~CEditRule();

// Dialog Data
	//{{AFX_DATA(CEditRule)
	enum { IDD = IDD_EDITRULE };
	CButton					m_btnAdvanced;
	//CTabButton				m_chkSubRules;
	CButton					m_chkSubRules;
	CRtfCmb					m_cmbEvents;
	CComboBox				m_cmbEventParams[g_uMaxEventParams];
	CRtfCmb					m_cmbActions;
	CRtfCmb					m_cmbActionParams[g_uMaxActionParams];
	CStatic					m_lblEventParams[g_uMaxEventParams];
	CStatic					m_lblActionParams[g_uMaxActionParams];
	CStatic					m_lblParamDesc;
	CChatServiceComboBox	m_cmbEventNetParam;
	CChatServiceComboBox	m_cmbActionNetParam;
	CSoundComboBox			m_cmbActionSndParam;
	CSpinButtonCtrl			m_spinDelay;
	//CTabEdit				m_editDelay;
	CEdit					m_editDelay;
	//CTabButton			m_btnCancel;
	CButton					m_btnCancel;
	UCHAR					m_uMinDelay;
	UCHAR					m_uDelay;
	//}}AFX_DATA

	void	UseRule(CCRule* pRule, CCDynaRules* pDynaRules);
	void	Tab(CWnd* pWnd, BOOL bForward);
	UCHAR	GetDelay()	{ return m_uDelay; }

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CEditRule)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	BOOL	bFillActionsFromEvent(BOOL* pbActionChanged);
	void	FillParamLabels(BOOL bEvents, BOOL bActions);
	BOOL	bFillParamsFromRule(BOOL bEvents, BOOL bActions);
	BOOL	bCorrectActionKeys();
	void	SaveComboParams(BOOL bEvents, BOOL bActions);
	void	SaveRuleParams();
	void	UpdateAdvancedControls();

	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CEditRule)
	afx_msg void OnEventChanged();
	afx_msg void OnActionChanged();
	afx_msg void OnEventParamSetFocus(UINT uID);
	afx_msg void OnEventParamKillFocus(UINT uID);
	afx_msg void OnActionParamSetFocus(UINT uID);
	afx_msg void OnActionParamKillFocus(UINT uID);
	afx_msg void OnEventNetParamSetFocus();
	afx_msg void OnActionNetParamSetFocus();
	afx_msg void OnActionSndParamSetFocus();
	afx_msg void OnEventsCmbSetFocus();
	afx_msg void OnActionsCmbSetFocus();
	afx_msg void OnKillFocusClear();
	afx_msg void OnActionParamFilter(UINT uID, NMHDR *pNotifyStruct, LRESULT *plResult);
	afx_msg void OnFlagsCheckClick();
	afx_msg void OnAdvancedClick();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CCRule*			m_pRule;
	CCDynaRules*	m_pDynaRules;
	CString			m_rgstrParamLabels[ptMax];
	CString			m_rgstrEventParams[ptMax];
	CString			m_rgstrActionParams[ptMax];
	CDWordArray*	m_prgdwActionParamFormatting[ptMax];
	SHORT			m_nSoundFileType;
};


/////////////////////////////////////////////////////////////////////////////
// CAddToSets dialog

class CAddToSets : public CCSDialog
{
// Construction
public:
	CAddToSets(CCDynaRules* pDynaCopy, CCRuleSet* pRuleSet, CCRule* pRule, CWnd* pwndParent = NULL);
	~CAddToSets() {};
	
	BOOL		bRuleAdded()
					{ return m_bRuleAdded; }

// Dialog Data
	//{{AFX_DATA(CAddToSets)
	enum { IDD = IDD_ADDTOSETS };
	CListCtrl	m_lstRule;
	CListBox	m_lstSets;
	//}}AFX_DATA

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAddToSets)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD	m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	BOOL			bFillRuleSets();
	BOOL			bAddToSets();

	// Generated message map functions
	//{{AFX_MSG(CAddToSets)
	//}}AFX_MSG
	// DECLARE_MESSAGE_MAP()

	CCDynaRules*	m_pDynaCopy;
	CCRuleSet*		m_pRuleSet;
	CCRule*			m_pRule;
	CString			m_strEventDisplay;
	CString			m_strActionDisplay;
	BOOL			m_bRuleAdded;
};


/////////////////////////////////////////////////////////////////////////////
// CSetNameConflict dialog

class CSetNameConflict : public CCSDialog
{
// Construction
public:
	CSetNameConflict(CString strSetName, CRuleSetsListBox* plstSets, CWnd* pwndParent = NULL);
	~CSetNameConflict() {};

// Dialog Data
	//{{AFX_DATA(CSetNameConflict)
	enum { IDD = IDD_RENAMELOADEDSET };
	CFilterEdit			m_editSetName;
	CString				m_strSetName;
	CRuleSetsListBox*	m_plstSets;
	//}}AFX_DATA

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSetNameConflict)
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CSetNameConflict)
	afx_msg void	OnOverwriteRuleSetClick();
	afx_msg void	OnRenameRuleSetClick();
	afx_msg void	OnRenamedRuleSetChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CCreateSet dialog

class CCreateSet : public CCSDialog
{
// Construction
public:
	CCreateSet(CRuleSetsListBox* plstSets, CWnd* pwndParent = NULL);
	~CCreateSet() {};

// Dialog Data
	//{{AFX_DATA(CCreateSet)
	enum { IDD = IDD_CREATESET };
	CFilterEdit			m_editSetName;
	CString				m_strSetName;
	CRuleSetsListBox*	m_plstSets;
	//}}AFX_DATA

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCreateSet)
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CCreateSet)
	afx_msg void	OnCreatedRuleSetChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////
// CRenameSet dialog

class CRenameSet : public CCSDialog
{
// Construction
public:
	CRenameSet(CCRuleSet* pRuleSet, CRuleSetsListBox* plstSets, CWnd* pwndParent = NULL);
	~CRenameSet() {};

// Dialog Data
	//{{AFX_DATA(CRenameSet)
	enum { IDD = IDD_RENAMESET };
	CFilterEdit			m_editSetName;
	CString				m_strSetName;
	CRuleSetsListBox*	m_plstSets;
	//}}AFX_DATA

protected:
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRenameSet)
	virtual void OnOK();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	static const DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CRenameSet)
	afx_msg void	OnRenamedRuleSetChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CCRuleSet*	m_pRuleSet;
};


/////////////////////////////////////////////////////////////////////////////
// CRuleSetsPage dialog

class CRuleSetsPage : public CCSPropertyPage
{
// Construction
public:
	CRuleSetsPage();
	~CRuleSetsPage();

	void SetDynaRules(CCDynaRules* pDynaCopy) { m_pDynaCopy = pDynaCopy; }

// Dialog Data
	//{{AFX_DATA(CRuleSetsPage)
	enum { IDD = IDD_RULESETSPAGE };
	CRuleSetsListBox	m_lstSets;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRuleSetsPage)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	void				UpdateButtonsStatus();
	void				SetCurRuleSet(CCRuleSet* pRuleSet, INT iIndex);
	BOOL				bFillRuleSets();

	static const  DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CRuleSetsPage)
	afx_msg void OnCreateRuleSet();
	afx_msg void OnRenameRuleSet();
	afx_msg void OnDeleteRuleSet();
	afx_msg void OnMoveUpRuleSet();
	afx_msg void OnMoveDownRuleSet();
	afx_msg void OnLoadRuleSet();
	afx_msg void OnSaveRuleSet();
	afx_msg void OnRuleSetItemChanged();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	CCDynaRules*		m_pDynaCopy;
};


/////////////////////////////////////////////////////////////////////////////
// CRulesPage dialog

class CRulesPage : public CCSPropertyPage
{
//	DECLARE_DYNCREATE(CRulesPage)

// Construction
public:
	CRulesPage();
	~CRulesPage();

	void SetDynaRules(CCDynaRules* pDynaCopy) { m_pDynaCopy = pDynaCopy; }

// Dialog Data
	//{{AFX_DATA(CRulesPage)
	enum { IDD = IDD_RULESPAGE };
	CRulesListCtrl	m_lstRules;
	CListBox		m_lstSets;
	//}}AFX_DATA

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CRulesPage)
	public:
	virtual BOOL OnSetActive();
	virtual void OnOK();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL


// Implementation
protected:
	void				UpdateButtonsStatus();
	void				SetCurRuleSet(CCRuleSet* pRuleSet, INT iIndex);
	BOOL				bFillRuleSets();

	static const  DWORD m_nHelpIDs[];
	virtual const DWORD *GetHelpIDs() {return m_nHelpIDs;}

	// Generated message map functions
	//{{AFX_MSG(CRulesPage)
	afx_msg void OnRuleSetItemChanged();
	afx_msg void OnAddRule();
	afx_msg void OnEditRule();
	afx_msg void OnDeleteRule();
	afx_msg void OnDuplicateRule();
	afx_msg void OnMoveUpRule();
	afx_msg void OnMoveDownRule();
	afx_msg void OnAddToRuleSets();
	afx_msg void OnAdvancedRuleSettings();
	afx_msg void OnRuleItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDblclkRule(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	BOOL			m_bRulesColumnSet;
	CCDynaRules*	m_pDynaCopy;
	CImageList		m_ilActiveStatus;
};

#endif __AUTOPAGE_H__
