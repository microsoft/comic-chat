// ColorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "ColorDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorDlg dialog

CColorDlg::CColorDlg(LONG lInitialColor, CWnd* pParent /*=NULL*/)
	: CDialog(CColorDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CColorDlg)
	//}}AFX_DATA_INIT
	m_iIndex = m_nSelectedColor = -1;
	m_point.x = m_point.y = 0;

	if (lInitialColor >= 0)
		for (SHORT n = 0; n < 16; n++) 
			if (clrTable[n] == (COLORREF) lInitialColor)
			{
				m_iIndex = m_nSelectedColor = n;
				break;
			}
}


CColorDlg::~CColorDlg()
{
	m_brush.DeleteObject();
}


BOOL CColorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();
	
	RECT rect;

	for (int i = 0; i < 8; i++)
	{
		rect.left = g_nMarginWidth + (g_nColorWidth + g_nIntervalWidth) * i;
		rect.top = g_nMarginHeight;
		rect.right = rect.left + g_nColorWidth;
		rect.bottom = rect.top + g_nColorHeight;

		m_position[i].Create(WS_CHILD|WS_CLIPSIBLINGS/*|WS_TABSTOP|(i==0?WS_GROUP:0)*/, rect, (CWnd*) this, IDC_COLOR1+i+20);
		m_position[i].m_pColorDlg = this;
		m_position[i].m_iIndex = i;

		m_color[i].Create("", SS_LEFT|SS_NOTIFY|WS_BORDER|WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, rect, (CWnd*) this, IDC_COLOR1+i);
		m_color[i].m_pColorDlg = this;
		m_color[i].m_position.x = rect.left;
		m_color[i].m_position.y = rect.top;
		m_color[i].m_iIndex = i;

		rect.top = g_nMarginHeight + g_nColorHeight + g_nIntervalHeight;
		rect.bottom = rect.top + g_nColorHeight;

		m_position[i+8].Create(WS_CHILD|WS_CLIPSIBLINGS, rect, (CWnd*) this, IDC_COLOR1+i+28);
		m_position[i+8].m_pColorDlg = this;
		m_position[i+8].m_iIndex = i+8;

		m_color[i+8].Create("", SS_LEFT|SS_NOTIFY|WS_BORDER|WS_CHILD|WS_VISIBLE|WS_CLIPSIBLINGS, rect, (CWnd*) this, IDC_COLOR1+i+8);
		m_color[i+8].m_pColorDlg = this;
		m_color[i+8].m_position.x = rect.left;
		m_color[i+8].m_position.y = rect.top;
		m_color[i+8].m_iIndex = i+8;
	}

	m_selection.CreateEx(WS_EX_STATICEDGE/*|WS_EX_TRANSPARENT*/, "Static", "", SS_NOTIFY|SS_LEFT|WS_CHILD|WS_CLIPSIBLINGS, 0, 0, g_nIntervalWidth+g_nColorWidth, g_nIntervalHeight+g_nColorHeight, this->m_hWnd, (HMENU) IDC_COLOR1+16);

	m_cursor.CreateEx(WS_EX_STATICEDGE/*|WS_EX_TRANSPARENT*/, "Static", "", SS_NOTIFY|SS_LEFT|WS_CHILD|WS_CLIPSIBLINGS, 0, 0, g_nIntervalWidth+g_nColorWidth, g_nIntervalHeight+g_nColorHeight, this->m_hWnd, (HMENU) IDC_COLOR1+17);

	if (m_nSelectedColor != -1)
		m_selection.SetWindowPos(&wndBottom, g_nMarginWidth - g_nIntervalWidth/2 + (m_nSelectedColor%8)*(g_nColorWidth+g_nIntervalWidth), m_nSelectedColor<8 ? g_nMarginHeight - g_nIntervalHeight/2 : g_nMarginHeight - g_nIntervalHeight/2 + g_nColorHeight + g_nIntervalHeight, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);

	SetFocus();

	return TRUE;
}


void CColorDlg::OnOK()
{
	// OutputDebugString("CColorDlg::OnOK\n");
	m_nSelectedColor = m_iIndex;
	CDialog::OnOK();
}


void CColorDlg::SetCursorPos(const POINT &pt, const SHORT iIndex)
{
	// OutputDebugString("CColorDlg::SetCursorPos point\n");
	m_cursor.SetWindowPos(&wndBottom, pt.x - g_nIntervalWidth/2, pt.y - g_nIntervalHeight/2, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
	m_iIndex = iIndex;
}


//void CColorDlg::SetSelectionPos(SHORT nNewSelectionIndex)
//{
//	m_nSelectedColor = nNewSelectionIndex;
//	m_selection.SetWindowPos(&wndBottom, g_nMarginWidth - g_nIntervalWidth/2 + (m_nSelectedColor%8)*(g_nColorWidth+g_nIntervalWidth), m_nSelectedColor<8 ? g_nMarginHeight - g_nIntervalHeight/2 : g_nMarginHeight - g_nIntervalHeight/2 + g_nColorHeight + g_nIntervalHeight, 0, 0, SWP_SHOWWINDOW | SWP_NOSIZE);
//}


BOOL CColorDlg::GetSelectedColorRGB(COLORREF *pcr)
{
	ASSERT(pcr);

	if (m_nSelectedColor < 0)
		return FALSE;

	ASSERT(m_nSelectedColor < 16);

	*pcr = clrTable[m_nSelectedColor];
	return TRUE;
}


BEGIN_MESSAGE_MAP(CColorDlg, CDialog)
	//{{AFX_MSG_MAP(CColorDlg)
	ON_WM_CTLCOLOR()
	ON_CONTROL_RANGE(STN_CLICKED, IDC_COLOR1, IDC_COLOR16, OnColorClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorDlg message handlers
void CColorDlg::OnColorClick(UINT nID)
{
	// OutputDebugString("OnColorClick\n");
	// SetSelectionPos(nID - IDC_COLOR1);
	m_nSelectedColor = nID - IDC_COLOR1;
	EndDialog(nID);
}


HBRUSH CColorDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	if (CTLCOLOR_STATIC == nCtlColor)
	{
		// OutputDebugString("OnCtlColor\n");

		COLORREF cr = 0xFFFFFF;

		if (pWnd->m_hWnd == m_selection.m_hWnd)
			cr = 0xFFFFFF;
		else
			if (pWnd->m_hWnd == m_cursor.m_hWnd)
				cr = 0xC0C0C0;
			else
				for (SHORT n = 0; n < 16; n++)
					if (pWnd->m_hWnd == m_color[n].m_hWnd)
					{
						cr = clrTable[n];
						break;
					}

		m_brush.DeleteObject();
		m_brush.CreateSolidBrush(cr);

		return (HBRUSH) m_brush.GetSafeHandle();
	}
	else
		return CDialog::OnCtlColor(pDC, pWnd, nCtlColor);
}

/////////////////////////////////////////////////////////////////////////////
// CColorStatic static control

CColorStatic::CColorStatic() : CStatic()
{
	// OutputDebugString("CColorStatic\n");
	m_pColorDlg = NULL;
}

BEGIN_MESSAGE_MAP(CColorStatic, CStatic)
	//{{AFX_MSG_MAP(CColorStatic)
	ON_WM_MOUSEMOVE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorStatic message handlers
void CColorStatic::OnMouseMove(UINT nFlags, CPoint point)
{
	// OutputDebugString("OnMouseMove -- ColorStatic\n");
	ASSERT(m_pColorDlg);
	if (m_pColorDlg->m_point != point)
	{
		m_pColorDlg->m_point = point;
		m_pColorDlg->SetCursorPos(m_position, m_iIndex);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CColorEdit edit control

CColorEdit::CColorEdit() : CEdit()
{
	// OutputDebugString("CColorEdit\n");
	m_pColorDlg = NULL;
}

BEGIN_MESSAGE_MAP(CColorEdit, CEdit)
	//{{AFX_MSG_MAP(CColorEdit)
	ON_WM_KEYDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorEdit message handlers
void CColorEdit::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags)
{
	// OutputDebugString("CColorEdit::OnKeyDown\n");
	SHORT iNewIndex = 16;

	switch (nChar)
	{
	default:
		break;

	case VK_HOME:
		iNewIndex = 0;
		break;

	case VK_END:
		iNewIndex = 15;
		break;

	case VK_LEFT:
		switch (m_pColorDlg->m_iIndex)
		{
		case 0:
			iNewIndex = 7;
			break;
		case 8:
			iNewIndex = 15;
			break;
		default:
			iNewIndex = m_pColorDlg->m_iIndex - 1;
		}
		break;

	case VK_RIGHT:
		switch (m_pColorDlg->m_iIndex)
		{
		case 7:
			iNewIndex = 0;
			break;
		case 15:
			iNewIndex = 8;
			break;
		default:
			iNewIndex = m_pColorDlg->m_iIndex + 1;
		}
		break;

	case VK_UP:
	case VK_DOWN:
		iNewIndex = (m_pColorDlg->m_iIndex > 7) ? m_pColorDlg->m_iIndex - 8 : m_pColorDlg->m_iIndex + 8;
	}

	if (iNewIndex < 16)
		m_pColorDlg->SetCursorPos(m_pColorDlg->m_color[iNewIndex].m_position, iNewIndex);

	CEdit::OnKeyDown(nChar, nRepCnt, nFlags);
}

