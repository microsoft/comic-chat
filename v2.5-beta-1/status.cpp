// status.cpp : implementation file
//

#include "stdafx.h"
#include "chat.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "userinfo.h"
#include "textcore.h"
#include "textview.h"
#include "status.h"
#include "ui.h"

#define IDC_RICHEDIT	501

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CStatusView

IMPLEMENT_DYNCREATE(CStatusView, CTextView)

CStatusView::CStatusView()
{
}

CStatusView::~CStatusView()
{
	cui.m_pvStatusView = NULL;   // Should no longer reference it!
}


BEGIN_MESSAGE_MAP(CStatusView, CTextView)
	//{{AFX_MSG_MAP(CStatusView)
	ON_WM_SHOWWINDOW()
	ON_WM_CREATE()
	ON_WM_CONTEXTMENU()
	ON_UPDATE_COMMAND_UI(ID_VIEW_COMICS, OnUpdateViewComics)
	ON_UPDATE_COMMAND_UI(ID_VIEW_TEXT, OnUpdateViewText)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CStatusView diagnostics

#ifdef _DEBUG
void CStatusView::AssertValid() const
{
	CTextView::AssertValid();
}

void CStatusView::Dump(CDumpContext& dc) const
{
	CTextView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CStatusView message handlers


void AddToStatus(CIrcPrint &ircPrint, const char *szLine, CDWordArray *prgdwFormatting)
{
	int i;
	static BOOL		sbNewLine = FALSE;
	CStatusView*	pStatus = (CStatusView*) GetStatusView();

	ASSERT(ircPrint.m_iType == PT_WHOLESTRING || !prgdwFormatting);

	if (pStatus)
	{
		switch (ircPrint.m_iType)
		{
		case PT_NOTINIT:
			break;
		case PT_LASTSTRING:
			if (*ircPrint.m_szMessage)
				ircPrint.m_szMessage++;
			szLine = strchr(ircPrint.m_szMessage, ':');
			if (szLine)
				szLine++;
			break;
		case PT_OFFSET:
			if (!(szLine = ircPrint.m_szMessage))
			{
				ASSERT(FALSE);
				return;
			}
			for (i = 0; i < ircPrint.m_offset; i++)
			{
				szLine = strchr(szLine, ' ');
				if (!szLine)
					return;
				else
					szLine++;	// move beyond the space
			}
			break;
		case PT_NONE:
			sbNewLine |= ircPrint.m_bNewLine;	// REGISB: won't be able to append empty lines with this solution
			return;								// but we shouldn't have to do that

			//if (!ircPrint.m_bNewLine)
			//	return;
			//ircPrint.m_bNewLine = FALSE;  // just displaying the line adds a newline
			//szLine = " "; // won't add empty message
			//break;
		case PT_WHOLESTRING:
			szLine = ircPrint.m_szMessage;
			break;
		}

		CHARFORMAT cf;
		ZeroMemory(&cf, sizeof(cf));
		cf.cbSize = sizeof(cf);
		cf.crTextColor = ircPrint.m_crTextColor;
		cf.dwMask = CFM_COLOR;

		int cbLen;
		char *szEnd = strchr(szLine, '\r');
		if (szEnd)
			cbLen = szEnd - szLine;
		else
			cbLen = strlen(szLine);
		CString strPretty(szLine, cbLen);
		if (sbNewLine)
		{
			strPretty = "\n" + strPretty;
			cbLen++;
			if (prgdwFormatting)
				PushFormattingOffsets(prgdwFormatting, 1);
		}
		sbNewLine = ircPrint.m_bNewLine;
		pStatus->m_textCore.iDisplayInfo(NULL,
										 0,
										 "",
										 0,
										 strPretty,
										 cbLen,
										 mtGetInfo,
										 msParticipant,
										 &cf,
										 -1,
										 prgdwFormatting ? prgdwFormatting->GetData() : NULL,
										 prgdwFormatting ? prgdwFormatting->GetSize() : 0);
		pStatus->GetDocument()->RegisterNewContent();
	}
}

void CStatusView::OnInitialUpdate() 
{
	// don't call textview's oninitialupdate -- don't want login dialog
	CView::OnInitialUpdate();
}


void CStatusView::OnActivateView(BOOL bActivate, CView* pActivateView, CView* pDeactiveView) 
{
	// don't call textview's onactivateview -- don't want login dialog
	CView::OnActivateView(bActivate, pActivateView, pDeactiveView);

	if (bActivate) {
		m_pRichEdit->ShowWindow(SW_NORMAL);	// fixes a bug in TextView (otherwise it can be stuck, inactive)
		GetDocument()->SetFocusToSayWnd();
	}
}

int CStatusView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTextView::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	m_textCore.bSetInsertBlank(0);

	return 0;
}

void CStatusView::OnUpdateViewComics(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(FALSE);
}

void CStatusView::OnUpdateViewText(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(FALSE);
	pCmdUI->SetCheck(TRUE);
}


int 
CStatusView::LoadContextMenu(
CMenu& menu)
{
	menu.LoadMenu (IDR_STATUSVIEW);
	return 0;
}
