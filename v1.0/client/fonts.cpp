// *******************************************
// Comic Font Selection and Instantiation Code
//	  -- additional methods for CUnitPanelPage

#include "stdafx.h"
#include "chat.h"

#include "common.h"
#include "traj.h"
#include "spline.h"
#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "balloon.h"
#include "backdrop.h"
#include "panel.h"

// font picker parameters
#define nFontHeight				-14
#define nFontHeightBig			-24
#define nFontHeightBalloon		250
#define nFontHeightMini			210
#define nFontHeightTitle		800
#define nFontHeightShout		350
#define nFontWidth				0
#define nFontEscapement			0
#define nFontOrientation		0
#define fnFontWeight			FW_DONTCARE
#define fdwFontItalic			FALSE
#define fdwFontUnderline		FALSE
#define fdwFontStrikeOut		FALSE
#define fdwFontCharSet			ANSI_CHARSET
#define	fdwFontOutputPrecision	OUT_DEFAULT_PRECIS
#define fdwFontClipPrecision	CLIP_DEFAULT_PRECIS
#define fdwFontQuality			DEFAULT_QUALITY
#define fdwFontPitchAndFamily	VARIABLE_PITCH | FF_DONTCARE
#define titleFontWeight			FW_HEAVY
#define FontWeightWN			FW_BOLD

#define MSTITLE

//#ifdef MSTITLE
//#define titleFontFace			"Comic Sans MS"
//#else
//#define titleFontFace			"ComixHeavy"
//#endif

// Some necessary class static definitions
CFont	CUnitPanelPage::m_fontTextBig;
CFont	CUnitPanelPage::m_fontBalloon;
CFont	CUnitPanelPage::m_fontWhisper;
CFont	CUnitPanelPage::m_fontMini;
CFont	CUnitPanelPage::m_fontTitle;
CFont	CUnitPanelPage::m_fontShout;

CFontInfo *CUnitPanelPage::m_fiTextBig		= NULL;
CFontInfo *CUnitPanelPage::m_fiWNormal		= NULL;
CFontInfo *CUnitPanelPage::m_fiWWhisper		= NULL;
CFontInfo *CUnitPanelPage::m_fiMini			= NULL;
CFontInfo *CUnitPanelPage::m_fiTitle		= NULL;
CFontInfo *CUnitPanelPage::m_fiShout		= NULL;

static CString fontFace;	// initialized in InitializeFonts

// Should only be called once, but necessarily only after we can call GetClientDC()
BOOL CUnitPanelPage::InitializeFonts() {
	if (m_fontBalloon.m_hObject) return TRUE;		// fonts already initialized

	fontFace.LoadString(ID_COMIC_FONT_NAME);

	if( !m_fontBalloon.CreateFont(nFontHeightBalloon,
								nFontWidth,				
								nFontEscapement,
								nFontOrientation,
		//						FontWeightWN,
								fnFontWeight,
								fdwFontItalic,
								fdwFontUnderline,
								fdwFontStrikeOut,
								fdwFontCharSet,
								fdwFontOutputPrecision,
								fdwFontClipPrecision,
								fdwFontQuality,
								fdwFontPitchAndFamily,
								fontFace ) )
		return FALSE;

	if( !m_fontWhisper.CreateFont(nFontHeightBalloon,
								nFontWidth,				
								nFontEscapement,
								nFontOrientation,
								fnFontWeight,
								TRUE,						// italic
								fdwFontUnderline,
								fdwFontStrikeOut,
								fdwFontCharSet,
								fdwFontOutputPrecision,
								fdwFontClipPrecision,
								fdwFontQuality,
								fdwFontPitchAndFamily,
								fontFace ) )
		return FALSE;

	if( !m_fontMini.CreateFont(	nFontHeightMini,
								nFontWidth,				
								nFontEscapement,
								nFontOrientation,
								fnFontWeight,
								fdwFontItalic,
								fdwFontUnderline,
								fdwFontStrikeOut,
								fdwFontCharSet,
								fdwFontOutputPrecision,
								fdwFontClipPrecision,
								fdwFontQuality,
								fdwFontPitchAndFamily,
								fontFace ) )
		return FALSE;

	if (m_fiWNormal) delete m_fiWNormal;
	m_fiWNormal = new CFontInfo(&m_fontBalloon, -70, 60);
	if (m_fiWWhisper) delete m_fiWWhisper;
	m_fiWWhisper = new CFontInfo(&m_fontWhisper, -70, 60);
	if (m_fiMini) delete m_fiMini;
	m_fiMini = new CFontInfo(&m_fontMini, -50, 40);

	return UpdateTitleFonts();
}

// these are dependent on panel size
BOOL CUnitPanelPage::UpdateTitleFonts() {
	if (!m_fiWNormal) return FALSE;					// don't do this until InitializeFonts called once!

	m_fontTitle.DeleteObject();						// reclaim these so we can do the CreateFont again
	m_fontShout.DeleteObject();

	float reduction = (float)unitWidth / 4860;
	int fontHeight = (int) (nFontHeightTitle * reduction);
	fontHeight = max(fontHeight, (int) (1.2 * nFontHeightBalloon));
	if (!m_fontTitle.CreateFont(fontHeight,
								nFontWidth,				
								nFontEscapement,
								nFontOrientation,
								fnFontWeight,
								fdwFontItalic,
								fdwFontUnderline,
								fdwFontStrikeOut,
								fdwFontCharSet,
								fdwFontOutputPrecision,
								fdwFontClipPrecision,
								fdwFontQuality,
								fdwFontPitchAndFamily,
								fontFace ) )
		return FALSE;

	fontHeight = (int) (nFontHeightShout * reduction);
	fontHeight = max(fontHeight, nFontHeightBalloon);
	if (!m_fontShout.CreateFont(fontHeight,
								nFontWidth,				
								nFontEscapement,
								nFontOrientation,
								fnFontWeight,
								fdwFontItalic,
								fdwFontUnderline,
								fdwFontStrikeOut,
								fdwFontCharSet,
								fdwFontOutputPrecision,
								fdwFontClipPrecision,
								fdwFontQuality,
								fdwFontPitchAndFamily,
								fontFace ) )
		return FALSE;

	if (m_fiTitle) delete m_fiTitle;
	if (m_fiShout) delete m_fiShout;

#ifdef MSTITLE
	m_fiTitle = new CFontInfo(&m_fontTitle, (int)(-220 * reduction), (int)(120 * reduction));
#else
	m_fiTitle = new CFontInfo(&m_fontTitle, 0, 0);
#endif
	m_fiShout = new CFontInfo(&m_fontShout, 0, 0);

	return TRUE;
}


void CUnitPanelPage::DestroyFonts() {
	// first, Do DeleteObject on CFonts
	m_fontBalloon.DeleteObject();
	m_fontWhisper.DeleteObject();
	m_fontMini.DeleteObject();

	if (m_fiWNormal) {
		delete m_fiWNormal;
		m_fiWNormal = NULL;  // gratuitous now, but allows for multiple calls in the future
	}
	if (m_fiWWhisper) {
		delete m_fiWWhisper;
		m_fiWWhisper = NULL;
	}
	if (m_fiMini) {
		delete m_fiMini;
		m_fiMini = NULL;
	}
	if (m_fiTitle) {
		delete m_fiTitle;
		m_fiTitle = NULL;
	}
	if (m_fiShout) {
		delete m_fiShout;
		m_fiShout = NULL;
	}
}




			
