//****************************************************************************
//  Module:     Chat2000.EXE
//  File:       C2View.cpp : implementation of the CChat2000View class
//              
//
//  Copyright (c) Microsoft Corporation 1995-1997
//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF 
// ANY KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO 
// THE IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A 
// PARTICULAR PURPOSE.
//****************************************************************************

#include "stdafx.h"
#include "Chat2000.h"

#include "C2Doc.h"
#include "C2View.h"
#include "MainFrm.h"

#include "EnterDlg.h"
#include "KickBan.h"
#include "BanDlg.h"
#include "InfoDlg.h"
#include "TopicDlg.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CChat2000View

IMPLEMENT_DYNCREATE(CChat2000View, CView)

BEGIN_MESSAGE_MAP(CChat2000View, CView)
	//{{AFX_MSG_MAP(CChat2000View)
	ON_WM_CREATE()
	ON_COMMAND(IDM_CHAT_ENTERROOM, OnChatEnterRoom)
	ON_COMMAND(IDM_CHAT_EXITROOM, OnChatExitRoom)
	ON_UPDATE_COMMAND_UI(IDM_CHAT_ENTERROOM, OnUpdateChatEnterRoom)
	ON_UPDATE_COMMAND_UI(IDM_CHAT_EXITROOM, OnUpdateChatExitRoom)
	ON_WM_SIZE()
	ON_WM_PARENTNOTIFY()
	ON_COMMAND(ID_CHAT_ABOUTMSCHAT, OnChatAboutmschat)
	ON_COMMAND(ID_CHAT_KICKPARTICIPANT, OnChatKickParticipant)
	ON_UPDATE_COMMAND_UI(ID_CHAT_KICKPARTICIPANT, OnUpdateChatKickparticipant)
	ON_COMMAND(ID_CHAT_CLEARHISTORY, OnChatClearhistory)
	ON_COMMAND(ID_CHAT_BANPARTICIPANT, OnChatBanparticipant)
	ON_UPDATE_COMMAND_UI(ID_CHAT_BANPARTICIPANT, OnUpdateChatBanparticipant)
	ON_COMMAND(ID_CHAT_SHOWINFO, OnChatShowinfo)
	ON_COMMAND(ID_CHAT_SETROOMTOPIC, OnChatSetroomtopic)
	ON_UPDATE_COMMAND_UI(ID_CHAT_SETROOMTOPIC, OnUpdateChatSetroomtopic)
	//}}AFX_MSG_MAP
	// Standard printing commands
	//ON_COMMAND(ID_FILE_PRINT, CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_DIRECT, CView::OnFilePrint)
	//ON_COMMAND(ID_FILE_PRINT_PREVIEW, CView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CChat2000View construction/destruction

CChat2000View::CChat2000View()
{
	m_iXppli = m_iYppli = 0;
	m_posThisParticipant = NULL;
	m_bActive = FALSE;
}

CChat2000View::~CChat2000View()
{
}

BOOL CChat2000View::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs

	return CView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CChat2000View drawing

void CChat2000View::OnDraw(CDC* pDC)
{
	CChat2000Doc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);

	// TODO: add draw code for native data here
	//Call the CWnd method for Redrawing the OCX.
	m_ocxChat.RedrawWindow();
}

/////////////////////////////////////////////////////////////////////////////
// CChat2000View diagnostics

#ifdef _DEBUG
void CChat2000View::AssertValid() const
{
	CView::AssertValid();
}

void CChat2000View::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CChat2000Doc* CChat2000View::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CChat2000Doc)));
	return (CChat2000Doc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CChat2000View message handlers

int CChat2000View::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	//enable OCX embeding
	AfxEnableControlContainer();

	//Get pixels per logical inch, used to calculate the size of the control
	CClientDC MyCDC(NULL);
	m_iXppli = MyCDC.GetDeviceCaps(LOGPIXELSX);
	m_iYppli = MyCDC.GetDeviceCaps(LOGPIXELSY);
	
	//Create the ChatOCX and initialize the UI
	RECT rect;

	rect.left	= 1;
	rect.top	= 1;
	rect.right	= 5000;//lpCreateStruct->cx;
	rect.bottom	= 5000;//lpCreateStruct->cy;
	
	if (!m_ocxChat.Create(NULL, WS_VISIBLE | WS_BORDER, rect, this, IDC_OCX_CHAT))
	{
		CString szFailMessage;
		szFailMessage.LoadString(IDS_FAIL_TO_LOAD_OCX);
		if (IDYES == AfxMessageBox (szFailMessage,MB_YESNO ) )
		{
			CString szURLtoChat;
			szURLtoChat.LoadString(IDS_URL_TO_CHAT);
			//start default web browser to load the chat OCX
			HINSTANCE hResults = ShellExecute (NULL, "open", szURLtoChat, "",NULL,SW_SHOWNORMAL);
			//if ((int) hResult <= 32) // error starting the browser
			//TODO handle the ShellExecute faillure
		}
		return -1;
	}
		
	//resize the OCX so that the UI looks proportionate
	m_ocxChat.SetWidth(MAP_PIX_TO_LOGHIM (1000,m_iXppli));
	m_ocxChat.SetHeight(MAP_PIX_TO_LOGHIM (1000,m_iYppli));
	m_ocxChat.SetUIOption(UI_OPT_ALL - UI_OPT_DISPLAY_ERRORMESSAGES - UI_OPT_ALLOW_RESIZING);

	//set the Status UI 
	UpdateStatusUI();

	return 0;
}

void CChat2000View::OnSize(UINT nType, int cx, int cy) 
{
	CView::OnSize(nType, cx, cy);
	
	//TRACE ("OnSize\n");

	if (m_ocxChat && cx && cy) //do not resize to 0
	{
		m_ocxChat.SetWidth(MAP_PIX_TO_LOGHIM (cx,m_iXppli));
		m_ocxChat.SetHeight(MAP_PIX_TO_LOGHIM (cy,m_iYppli));
	}
}


void CChat2000View::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	m_bActive = bActivate;
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);
	UpdateStatusUI();
}

void CChat2000View::OnParentNotify(UINT message, LPARAM lParam) 
{
	//to trap the WM_PARENTNOTIFY message we need to set the 'Message Filter' dropdown
	//in the 'advaced options' section on the 'Class Info' tab of the 'Class Wizard' to
	//Window'.  The View is originally set as 'Child Window'
	
	CView::OnParentNotify(message, lParam);

	//We might not have focus yet. We risk to pop up the wrong menu
	//To avoid that we check if the view got activated

	if ( (message == WM_RBUTTONDOWN) && (m_bActive) )
	{		
		CMenu menu;
		CMenu *submenu;
		//Load the menu.
		menu.LoadMenu(IDR_CHAT20TYPE);
		//Get the pop-up menu.
		submenu = menu.GetSubMenu(POPUPMENU);
		
		CPoint pt(LOWORD(lParam),HIWORD(lParam));
		//Convert to screen coordinates.
		ClientToScreen(&pt);
		//Post the menu.
		submenu->TrackPopupMenu(TPM_LEFTALIGN|TPM_RIGHTBUTTON,
				pt.x,    pt.y,
				AfxGetApp()->m_pMainWnd,NULL);
	}	
}

//-----------------------------------------------------------------------------
//Command handlers
void CChat2000View::OnChatEnterRoom() 
{
	ASSERT(m_ocxChat);

	CEnterDlg	dlgEnter;
		
	int iDlgRet = dlgEnter.DoModal();

	if (iDlgRet == IDOK)
	{

		TRACE1("Room URL: %s",(LPCTSTR) dlgEnter.m_szRoom);
		m_ocxChat.EnterRoom((LPCTSTR) dlgEnter.m_szRoom, dlgEnter.m_szPassword, dlgEnter.m_szNickName,_T("NTLM;ALL"), dlgEnter.m_iFlags, dlgEnter.m_iTypes);
	}
	else
	{
		if (iDlgRet != IDCANCEL)
		{
			AfxMessageBox ("Error Displaying the dialog: %i", iDlgRet);
		}
	}
	
}

void CChat2000View::OnChatExitRoom() 
{
	ASSERT(m_ocxChat);

	//exit or cancel
	int iState = m_ocxChat.GetState();
	ASSERT(iState != STATE_DISCONNECTED);

	if ( iState == STATE_CONNECTED)
	{
		m_ocxChat.ExitRoom();
	}
	else
	{
		m_ocxChat.CancelEntering();
	}
}

void CChat2000View::OnChatAboutmschat() 
{
	ASSERT(m_ocxChat);

	m_ocxChat.AboutBox();
}

void CChat2000View::OnChatKickParticipant() 
{
	ASSERT(m_ocxChat);

	CKickBanDlg	dlgKickBan;
	
	int iDlgRet = dlgKickBan.DoModal();

	if (iDlgRet == IDOK)
	{
		m_ocxChat.KickParticipant(-1,dlgKickBan.m_Comment);
	}
	else
	{
		if (iDlgRet != IDCANCEL)
		{
			AfxMessageBox ("Error Displaying the dialog: %i", iDlgRet);
		}
	}	
}

void CChat2000View::OnChatClearhistory() 
{
	ASSERT(m_ocxChat);
	m_ocxChat.ClearHistory();
}


void CChat2000View::OnChatBanparticipant() 
{
	ASSERT(m_ocxChat);

	CBanDlg	dlgBan;
	
	//set the control values
	dlgBan.m_Set = 0; //ban
	dlgBan.m_pListOfPart = &m_ListOfPart;

	int iDlgRet = dlgBan.DoModal();

	if ( (iDlgRet == IDOK) && (!dlgBan.m_Name.IsEmpty()) )
	{
		m_ocxChat.BanParticipant(dlgBan.m_Name,(dlgBan.m_Set==0));
	}
}

void CChat2000View::OnChatSetroomtopic() 
{
	ASSERT(m_ocxChat);

	CTopicDlg	dlgTopic;
	
	int iDlgRet = dlgTopic.DoModal();

	if ( iDlgRet == IDOK )
	{
		m_ocxChat.SetRoomTopic(dlgTopic.m_szTopic);
	}	
	
}

void CChat2000View::OnChatShowinfo() 
{
	ASSERT(m_ocxChat);

	CInfoDlg	dlgInfo;
	
	dlgInfo.m_szMaxNumOfMembers.Format("%u", m_ocxChat.GetMaxMembers());
	dlgInfo.m_szNumberOfMembers.Format("%u",m_ocxChat.GetMemberCount());
	dlgInfo.m_szAlias = m_ocxChat.GetThisParticipantAlias();
	dlgInfo.m_szID.Format("%u", m_ocxChat.GetThisParticipantID());
	dlgInfo.m_szPath = m_ocxChat.GetRoomPath();
	dlgInfo.m_szRatings = m_ocxChat.GetRating();
	dlgInfo.m_szTopic = m_ocxChat.GetRoomTopic();

	dlgInfo.DoModal();	
}


//-----------------------------------------------------------------------------
//UI Update functions
void CChat2000View::OnUpdateChatEnterRoom(CCmdUI* pCmdUI) 
{
	ASSERT(m_ocxChat);
	pCmdUI->Enable((m_ocxChat.GetState() == STATE_DISCONNECTED));
	
}

void CChat2000View::OnUpdateChatExitRoom(CCmdUI* pCmdUI) 
{
	ASSERT(m_ocxChat);
	short nState = m_ocxChat.GetState();

	//enable/disable the menu
	pCmdUI->Enable(nState != STATE_DISCONNECTED);

	if (nState != STATE_DISCONNECTED)
	{
		CString szNewMenuText;

		if (nState == STATE_CONNECTING)
		{
			szNewMenuText.LoadString(IDS_MNU_CANCEL_ENTERING);
		}
		else
		{
			szNewMenuText.LoadString(IDS_MNU_EXIT_ROOM);
		}

		pCmdUI->SetText(szNewMenuText);
	}
}

void CChat2000View::OnUpdateChatKickparticipant(CCmdUI* pCmdUI) 
{
	if (IsValidPos(m_posThisParticipant))
	{
		//enable the menu only if we are hosts
		pCmdUI->Enable( GetParticipantPos(m_posThisParticipant)->GetStatus() == PARTICIPANT_STATE_HOST);		
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}

}

void CChat2000View::OnUpdateChatBanparticipant(CCmdUI* pCmdUI) 
{

	if (IsValidPos(m_posThisParticipant))
	{
		//enable the menu only if we are hosts
		pCmdUI->Enable( GetParticipantPos(m_posThisParticipant)->GetStatus() == PARTICIPANT_STATE_HOST);		
	}
	else
	{
		pCmdUI->Enable(FALSE);
	}	
}

void CChat2000View::OnUpdateChatSetroomtopic(CCmdUI* pCmdUI) 
{
	ASSERT(m_ocxChat);
	short nState = m_ocxChat.GetState();

	pCmdUI->Enable(nState != STATE_DISCONNECTED);	
}



//-----------------------------------------------------------------------------
//Helper functions
void CChat2000View::UpdateStatusUI() 
{
	ASSERT(m_ocxChat);
	
	UpdateStatusUI(m_ocxChat.GetState());
	
}

void CChat2000View::UpdateStatusUI(short NewState_p) 
{
	ASSERT(m_ocxChat);

	CString szOut;
	szOut.LoadString(IDS_MDI_TITLE+NewState_p);
	
	//add the room info
	szOut +=  ((NewState_p != STATE_DISCONNECTED) ? (_T("[")+m_ocxChat.GetRoomPath()+_T("]")) : _T(""));
	
	if ( (NewState_p == STATE_CONNECTED) && (IsValidPos(m_posThisParticipant)) )
	{
		//add the user alias
		szOut = m_ocxChat.GetThisParticipantAlias() + _T(" ") + szOut;
	}	
	
	CMainFrame *MainFrame = (CMainFrame *)	GetParentFrame()->GetParentFrame();
	ASSERT (MainFrame); 
	MainFrame->m_wndStatusBar.SetPaneText(0,(LPCTSTR) szOut,TRUE);	
	MainFrame->GetActiveFrame()->SetWindowText((LPCTSTR) szOut);	
}

CString CChat2000View::GetHistory()
{
	ASSERT(m_ocxChat);
	//wrap this because it is used by the Doc class
	return m_ocxChat.GetHistory();
}

//-----------------------------------------------------------------------------
//Events from the OCX
//		since we created the OCX dinamically, the ClassWizard does not generate this code automatically
//		Added manually.  
//		An easy way to get this code is to create a project and insert the OCX in a dialog, let the Wizard generate the code,
//		Cut and paste in here and change the class references.

BEGIN_EVENTSINK_MAP(CChat2000View, CView)
    //{{AFX_EVENTSINK_MAP(CChat2000View)
	ON_EVENT(CChat2000View, IDC_OCX_CHAT, 111 /* OnStateChanged */, OnOnStateChangedOcxChat, VTS_I2)
	ON_EVENT(CChat2000View, IDC_OCX_CHAT, 106 /* OnParticipantStatusChanged */, OnOnParticipantStatusChangedOcxChat, VTS_I4 VTS_I2)
	ON_EVENT(CChat2000View, IDC_OCX_CHAT, 101 /* OnBeginEnumeration */, OnOnBeginEnumerationOcxChat, VTS_NONE)
	ON_EVENT(CChat2000View, IDC_OCX_CHAT, 102 /* OnEndEnumeration */, OnOnEndEnumerationOcxChat, VTS_NONE)
	ON_EVENT(CChat2000View, IDC_OCX_CHAT, 104 /* OnExitParticipant */, OnOnExitParticipantOcxChat, VTS_I4)
	ON_EVENT(CChat2000View, IDC_OCX_CHAT, 103 /* OnEnterParticipant */, OnOnEnterParticipantOcxChat, VTS_I4 VTS_BSTR VTS_I2)
	ON_EVENT(CChat2000View, IDC_OCX_CHAT, 114 /* OnError */, OnOnErrorOcxChat, VTS_I2 VTS_BSTR)
	//}}AFX_EVENTSINK_MAP
END_EVENTSINK_MAP()

void CChat2000View::OnOnStateChangedOcxChat(short NewState) 
{	
	TRACE("CChat2000View::OnOnStateChangedOcxChat\n");
	UpdateStatusUI(NewState);	

	//remove all participants from the list
	if (NewState == STATE_DISCONNECTED)
	{
		DelAll();
	}
}

void CChat2000View::OnOnParticipantStatusChangedOcxChat(long ParticipantID, short Status) 
{
	TRACE("CChat2000View::OnOnParticipantStatusChangedOcxChat\n");
	
	ASSERT(ParticipantID);
	//set the participant status
	GetParticipantID((short)ParticipantID)->SetStatus (Status);
}

void CChat2000View::OnOnBeginEnumerationOcxChat() 
{
	TRACE("CChat2000View::OnOnBeginEnumerationOcxChat\n");
}

void CChat2000View::OnOnEndEnumerationOcxChat() 
{
	TRACE("CChat2000View::OnOnEndEnumerationOcxChat\n");

	//The OCX is done enumerating, now we can find out who we are
	m_posThisParticipant = FindParticipantID((short)m_ocxChat.GetThisParticipantID()); 

	//we entered a room, set the UI
	UpdateStatusUI();
}

void CChat2000View::OnOnExitParticipantOcxChat(long ParticipantID) 
{
	TRACE("CChat2000View::OnOnExitParticipantOcxChat\n");

	DelParticipantID((short)ParticipantID);
}

void CChat2000View::OnOnEnterParticipantOcxChat(long ParticipantID, LPCTSTR Alias, short Status) 
{
	TRACE("CChat2000View::OnOnEnterParticipantOcxChat\n");
	
	AddParticipant (ParticipantID, Alias, Status);
}

void CChat2000View::OnOnErrorOcxChat(short ErrorCode, LPCTSTR Description) 
{
	//catch those asyncronous errors
	AfxMessageBox(Description,MB_ICONSTOP | MB_OK);	
}

//-----------------------------------------------------------------------------
//participant list manager

inline void CChat2000View::AddParticipant	(CParticipant &Part_p)
{
	m_ListOfPart.AddTail(Part_p);
}

inline void CChat2000View::AddParticipant	(long ParticipantID, LPCTSTR Alias, short Status)
{
	CParticipant Part(ParticipantID, Alias, Status);
	m_ListOfPart.AddTail(Part);
}

inline void CChat2000View::DelParticipantID	(short iID_p)
{
	CParticipant Part(iID_p);
	m_ListOfPart.RemoveAt(m_ListOfPart.Find(Part));
	if (iID_p == m_ocxChat.GetThisParticipantID())
	{
		m_posThisParticipant = NULL;
	}
}

inline void CChat2000View::DelAll	()
{
	m_ListOfPart.RemoveAll();
	m_posThisParticipant = NULL;
}
 
inline POSITION CChat2000View::FindParticipantID	(short iID_p)
{
	CParticipant Part(iID_p);
	return m_ListOfPart.Find(Part);
}

inline CParticipant *CChat2000View::GetParticipantPos	(POSITION Pos_p)
{
	return &m_ListOfPart.GetAt(Pos_p);
}

inline CParticipant *CChat2000View::GetParticipantID	(short iID_p)
{
	return GetParticipantPos(FindParticipantID(iID_p));
}

inline BOOL CChat2000View::IsValidPos	(POSITION Pos_p)
{
	return (Pos_p != NULL);
	//we could check if the pos falls between the head and tail of the list
	//but we will rely on the fact that we keep a consistent list
}

inline CString CChat2000View::GetParticipantAlias()
{
	static POSITION CurrentPos = m_ListOfPart.GetHeadPosition();
	
	if (IsValidPos (CurrentPos))
	{
		//return the alias, set the position to the next element of the list
		return (m_ListOfPart.GetNext(CurrentPos)).GetAlias();
	}
	else
	{
		//we reached the end of the list or the list is empty
		//go back to the head
		CurrentPos = m_ListOfPart.GetHeadPosition();
		return "";
	}
}

