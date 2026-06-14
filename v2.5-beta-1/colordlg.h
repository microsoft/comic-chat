// ColorDlg.h : header file
//

#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// CColorDlg dialog

const g_nColorWidth = 11;
const g_nColorHeight = 11;
const g_nMarginWidth = 10;
const g_nMarginHeight = 10;
const g_nIntervalWidth = 6;
const g_nIntervalHeight = 6;

const COLORREF clrTable[] = { RGB(0,0,0), RGB(128,0,0), RGB(0,128,0), RGB(128,128,0),
							  RGB(0,0,128), RGB(128,0,128), RGB(0,128,128), RGB(128,128,128),
							  RGB(192,192,192), RGB(255,0,0), RGB(0,255,0), RGB(255,255,0),
							  RGB(0,0,255), RGB(255,0,255), RGB(0,255,255), RGB(255,255,255) };

class CColorDlg;

class CColorStatic : public CStatic
{
// Construction
public:
	CColorStatic();

// Members
	CColorDlg*	m_pColorDlg;
	POINT		m_position;
	SHORT		m_iIndex;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorStatic)
	afx_msg void	OnMouseMove(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CColorEdit : public CEdit
{
// Construction
public:
	CColorEdit();

// Members
	CColorDlg*	m_pColorDlg;
	SHORT		m_iIndex;

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CColorEdit)
	afx_msg void	OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


class CColorDlg : public CDialog
{
// Construction
public:
	CColorDlg(LONG lInitialColor, CWnd* pParent = NULL);
	~CColorDlg();

// Dialog Data
	//{{AFX_DATA(CColorDlg)
	enum { IDD = IDD_CHOOSECOLOR };
	//}}AFX_DATA

// Implementation
public:
	void			SetCursorPos(const POINT &pt, const SHORT iIndex);
//	void			SetSelectionPos(SHORT nNewSelectionIndex);
//	SHORT			GetSelectedColorIndex(void)
//						{ return m_nSelectedColor; }
	BOOL			GetSelectedColorRGB(COLORREF *pcr);

	CColorStatic	m_color[16];
	SHORT			m_iIndex;
	CPoint			m_point;

protected:
	CStatic			m_cursor;
	CStatic			m_selection;
	CColorEdit		m_position[16];
	CBrush			m_brush;
	SHORT			m_nSelectedColor;

	virtual BOOL	CColorDlg::OnInitDialog();
	virtual void	OnOK();

	// Generated message map functions
	//{{AFX_MSG(CColorDlg)
	afx_msg HBRUSH	OnCtlColor(CDC*, CWnd*, UINT);
	afx_msg void	OnColorClick(UINT nID);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
