// *******************************************
// Comic Font Selection and Instantiation Code
//	  -- additional methods for CUnitPanelPage

#include "stdafx.h"
#include "chat.h"

#include "traj.h"
#include "spline.h"
#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "balloon.h"
#include "backdrop.h"
#include "userinfo.h"
#include "chatprot.h"
#include "ui.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "pageview.h"
#include "panel.h"

// Some necessary class static definitions
CFont	*CUnitPanelPage::m_fontBalloon		= NULL;
CFont	*CUnitPanelPage::m_fontWhisper		= NULL;
CFont	*CUnitPanelPage::m_fontTitle		= NULL;
CFont	*CUnitPanelPage::m_fontShout		= NULL;

CFontInfo *CUnitPanelPage::m_fiWNormal		= NULL;
CFontInfo *CUnitPanelPage::m_fiWWhisper		= NULL;
CFontInfo *CUnitPanelPage::m_fiTitle		= NULL;
CFontInfo *CUnitPanelPage::m_fiShout		= NULL;

CPtrList CUnitPanelPage::m_fonts;
CPtrList CUnitPanelPage::m_fontInfos;

extern CChatApp theApp;

BOOL CUnitPanelPage::SetFonts(LOGFONT &logFont, COLORREF crTextColor)
{
	m_fontBalloon = new CFont;
	m_fonts.AddHead(m_fontBalloon);

	if (!m_fontBalloon->CreateFontIndirect(&logFont))
		return FALSE;

	CHAR		szPhysFaceName[LF_FACESIZE];
	TEXTMETRIC	tm;
	CDC*		pDc = GetClientDC();
	CFont*		pOldFont = pDc->SelectObject(m_fontBalloon);

	VERIFY(pDc->GetTextMetrics(&tm));

	if (logFont.lfCharSet != DEFAULT_CHARSET && logFont.lfCharSet != tm.tmCharSet)
	{
		m_fontBalloon->DeleteObject();
		strcpy(logFont.lfFaceName, theApp.m_szGuiFaceName);
		if (!m_fontBalloon->CreateFontIndirect(&logFont))
			return FALSE;
		pDc->SelectObject(m_fontBalloon);
	}

	theApp.m_comicsFont = logFont;   // save it away for future reference...

	if (!pDc->GetTextFace(LF_FACESIZE, szPhysFaceName))
		szPhysFaceName[0] = '\0';

	if (pOldFont)
		pDc->SelectObject(pOldFont);

	m_fontWhisper = new CFont;
	m_fonts.AddHead(m_fontWhisper);
	LOGFONT lf2 = logFont;
	
	// Far Eastern fonts are difficult to read in italicized form - only italicize Western fonts
	lf2.lfItalic = logFont.lfItalic || (tm.tmCharSet < 3 || tm.tmCharSet == GREEK_CHARSET || tm.tmCharSet == TURKISH_CHARSET || tm.tmCharSet == RUSSIAN_CHARSET || tm.tmCharSet == BALTIC_CHARSET);

	if (!m_fontWhisper->CreateFontIndirect(&lf2))
		return FALSE;

	double reduction = abs(logFont.lfHeight) / 180.0;
	int doVKern = (strcmp(szPhysFaceName, "Comic Sans MS") == 0) ? 1 : 0;
	// this is a good compromise for the whole planet:
	// we introduce this vertical kernel for the English speaking users in order to avoid large spaces 
	// between lines in the balloons (and also above the first line in the balloon).
	// For other users (like French) the accents on the characters might touch other characters or the balloon,
	// but the text is still readable.
	m_fiWNormal = new CFontInfo(m_fontBalloon, crTextColor, (int)(-40 * reduction * doVKern), (int)(30 * reduction * doVKern));
	m_fontInfos.AddHead(m_fiWNormal);
	m_fiWWhisper = new CFontInfo(m_fontWhisper, crTextColor, (int)(-40 * reduction * doVKern), (int)(30 * reduction * doVKern));
	m_fontInfos.AddHead(m_fiWWhisper);

	return UpdateTitleFonts();
}

// these are dependent on panel size
BOOL CUnitPanelPage::UpdateTitleFonts() {
	if (!m_fiWNormal)
		return FALSE;					// don't do this until InitializeFonts called once!

	float	reduction = (float) m_unitWidth / 4860;
	int		fontHeight = (int) (nFontHeightTitle * reduction);

	fontHeight = min(fontHeight, (int) (1.2 * theApp.m_iFontHeightBalloon));

	m_fontTitle = new CFont;
	m_fonts.AddHead(m_fontTitle);

	LOGFONT	lf2 = theApp.m_comicsFont;
	lf2.lfHeight = fontHeight;
	if (!m_fontTitle->CreateFontIndirect(&lf2))
		return FALSE;

	CHAR	szPhysFaceName[LF_FACESIZE];
	CDC*	pDc = GetClientDC();
	CFont*	pOldFont = pDc->SelectObject(m_fontTitle);

	if (!pDc->GetTextFace(LF_FACESIZE, szPhysFaceName))
		szPhysFaceName[0] = '\0';

	if (pOldFont)
		pDc->SelectObject(pOldFont);

	int		doVKern = (strcmp(szPhysFaceName, "Comic Sans MS") == 0) ? 1 : 0;

	fontHeight = (int) (nFontHeightShout * reduction);
	fontHeight = min(fontHeight, theApp.m_iFontHeightBalloon);
	lf2.lfHeight = fontHeight;
	m_fontShout = new CFont;
	m_fonts.AddHead(m_fontShout);
	if (!m_fontShout->CreateFontIndirect(&lf2))
		return FALSE;

	m_fiTitle = new CFontInfo(m_fontTitle, theApp.m_comicsColor, (int)(-220 * reduction * doVKern), (int)(120 * reduction * doVKern));

	m_fiShout = new CFontInfo(m_fontShout, theApp.m_comicsColor, 0, 0);
	m_fontInfos.AddHead(m_fiTitle);
	m_fontInfos.AddHead(m_fiShout);
	return TRUE;
}


void CUnitPanelPage::DestroyFonts() {
	m_fontBalloon = NULL;
	m_fontWhisper = NULL;
	m_fontTitle = NULL;
	m_fontShout = NULL;

	m_fiWNormal = NULL;
	m_fiWWhisper = NULL;
	m_fiTitle = NULL;
	m_fiShout = NULL;

	POSITION pos = m_fonts.GetHeadPosition();
	while (pos) {
		CFont *font = (CFont *) m_fonts.GetNext(pos);
		delete font;
	}
	m_fonts.RemoveAll();

	pos = m_fontInfos.GetHeadPosition();
	while (pos) {
		CFontInfo *fontInfo = (CFontInfo *) m_fontInfos.GetNext(pos);
		delete fontInfo;
	}
	m_fontInfos.RemoveAll();
}




			
