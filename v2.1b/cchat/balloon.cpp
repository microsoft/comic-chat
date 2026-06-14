#include "stdafx.h"
#include "chat.h"

#include "userinfo.h"
#include "chatprot.h"
#include "ircproto.h"
#include "ui.h"
#include "vector2d.h"
#include "traj.h"
#include "spline.h"
#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "balloon.h"
#include "backdrop.h"
#include "script.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "pageview.h"
#include "panel.h"
#include "format.h"
#include "ccommon.h"
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <tchar.h>
#include <winnls.h>

extern CChatApp theApp;

extern "C" void *GetMime();
extern "C" int	iBytesofChar(BYTE ch);
extern "C" BOOL FindSubStringForINTLThatFits(
								void	*,
								HDC		hdc,
								LPCTSTR lpsz,		// The string that we are trying to break up
								int		cbString,	// The length of the string
								DWORD*	prgdwFormatting,	// string formatting
								int		cFormats,			// number of elements in formatting array
								LPCTSTR	szFixedPitchName,	// Fixed Pitch font name
								LPCTSTR szSymbolName,		// Symbol font name
								int*	pcbFit,		// Pointer to location that holds the actual length of 
													// sub string that fits, broken at a space.
								BOOL*	pbHasBlankOrAlike,
								LPSIZE	lpSize,		// Send back the size of the substring that fits in this.
								int		nMaxExtent	// The size in which we are trying to fit in this string
);


#define XBOXDELTA			90
#define YBOXDELTA			50
#define MINROUTEWIDTH		300
#define BUBBLEHEIGHT		150
#define INTERBUBBLE			100
#define ENDBUBBLEWIDTH		400
#define VWAVEHEIGHT			70
#define VWAVEINTERVAL		300
#define HWAVEHEIGHT			70
#define HWAVEINTERVAL		300
#define MAXPTS				150
#define THRESH1				(-70)
#define THRESH2				70
#define XPERMUTE			50
#define YPERMUTE			10
#define XBORDER				100
#define YBORDER				40
#define TOPBORDER			(-20)
#define MAXLEFTSHIFT		0
#define MAXCENTERSHIFT		0 // 100
#define LARGEDELTA			350
#define SMALLDELTA			150
#define MINTAILHEIGHT		100
#define BORDERFUDGE			400		// Yum fudge!

// REGISB; not used anymore
//#define NODESCENDERS(x)		((x) == ANSI_CHARSET || (x) == GREEK_CHARSET || (x) == RUSSIAN_CHARSET \
//								/* || (x) == TURKISH_CHARSET */ || (x) == EASTEUROPE_CHARSET || (x) == BALTIC_CHARSET)
//#define FAREAST(x)			((x) == SHIFTJIS_CHARSET || (x) == HANGEUL_CHARSET || (x) == GB2312_CHARSET \
//							    || (x) == CHINESEBIG5_CHARSET || (x) == THAI_CHARSET)

//#define FAREAST_ADJUSTMENT	30
#define FAREAST_TOPOFFSET	50

typedef struct
{
	int start;
	int end;
	int x;
	int y;
} RANGE;

static const char	*szContinuationStr1 = "...";
static const char	*szContinuationStr2 = "...";
static POINT		mouseDownPt;
POINT				tpoint;

CPen CBWoodringNormal :: m_pen (PS_SOLID, 28, RGB(0,0,0));
CPen CBWoodringWhisper :: m_nimbusPen (PS_SOLID, 100, RGB(255, 255, 255));



void DrawPoints(CDC *pdc, POINT *p, int nPts)
{
	for (int i = 0; i < nPts; i++)
		DrawPoint(pdc, p[i]);
}

void Capitalize(char *str)
{
	DWORD lcid;
	unsigned char *cptr;
	switch (theApp.m_charSet) {
	   default:
			CharUpperBuff(str, _tcslen(str));   // internationalized
			return;		//
		case GREEK_CHARSET:
			lcid = MAKELCID(MAKELANGID(LANG_GREEK, SUBLANG_NEUTRAL), SORT_DEFAULT);
			cptr = (unsigned char *) str;
			while (*cptr) {
				if (*cptr == 220 || *cptr == 162) *cptr = 193;		// alpha w/ tonos -> ALPHA
				else if (*cptr == 221 || *cptr == 184) *cptr = 197; // epsilon w/ tonos -> EPSILON
				else if (*cptr == 222 || *cptr == 185) *cptr = 199; // eta w/ tonos -> ETA
				else if (*cptr == 223 || *cptr == 186) *cptr = 201; // iota w/ tonos -> IOTA
				else if (*cptr == 252 || *cptr == 188) *cptr = 207; // omicron w/ tonos -> OMICRON
				else if (*cptr == 253 || *cptr == 190) *cptr = 213; // upsilon w/ tonos -> UPSILON
				else if (*cptr == 254 || *cptr == 191) *cptr = 217; // omega w/ tonos -> OMEGA
				else if (*cptr == 242) *cptr = 211; // final sigma -> SIGMA
				else if (*cptr == 192) *cptr = 218; // iota w/ stuff -> IOTA W/ STUFF
				else if (*cptr == 224) *cptr = 219; // upsilon w/ stuff -> UPSILON W/ STUFF
				cptr++;
			}
			break;
		case RUSSIAN_CHARSET:
			lcid = MAKELCID(MAKELANGID(LANG_RUSSIAN, SUBLANG_NEUTRAL), SORT_DEFAULT);
			break;
		case TURKISH_CHARSET: {
			lcid = MAKELCID(MAKELANGID(LANG_TURKISH, SUBLANG_NEUTRAL), SORT_DEFAULT);
			cptr = (unsigned char *) str;
			while (*cptr) {
				if (*cptr == 253) *cptr = 73;
				else if (*cptr == 105) *cptr = 221;
				cptr++;
			}
			break;			
		}
	}
	char *src = strdup(str);
	int len = _tcslen(str)+1;
	VERIFY(LCMapString(lcid, LCMAP_UPPERCASE, src, len, str, len));
	free(src);
}


// return the first non-punctuation character after the stream of white space
// (or terminating null char)
char* GetNextStart(char *szString)
{
	while (my_isspace(*szString))
		szString = CharNext(szString);
	return szString;
}

// return the first whitespace char starting at szString
// (or the terminating null) after any initial run of whitespace
char* GetNextEnd(char *szString)
{
	while (*szString && my_isspace(*szString))
		szString = CharNext(szString);
	while (*szString && !my_isspace(*szString))
		szString = CharNext(szString);
	return szString;
}

BOOL UpcomingReturn(char *szString)
{
	while (*szString && my_isspace(*szString))
		if (*szString == '\n')
			return TRUE;
		else
			szString = CharNext(szString);

	return FALSE;
}


void ForceLineBreak(CDC *pdc, char *szString, CDWordArray *prgdwFormatting, int iMaxWidth, int &iLength, int &iWidth, int &iHeight)
{
	// TRACE("FORCELINEBREAK: MAXWIDTH = %d.\n", iMaxWidth);
	
	CSize	sizeExtent;
	int		iLastCharWidth;

	iWidth = iHeight = iLength = 0;
	
	while (TRUE)
	{
		iLastCharWidth = GetMime() ? iBytesofChar(szString[iLength]) : 1;
		iLength += iLastCharWidth;
		// REGISB: original: CSize sizeExtent = pdc->GetTextExtent(szString, iLength);
		sizeExtent = GetFormattedTextExtent(pdc, szString, iLength, prgdwFormatting);
		if (szString[iLength] && (sizeExtent.cx <= iMaxWidth))
		{
			iWidth = sizeExtent.cx;
			iHeight = max(iHeight, sizeExtent.cy);
		}
		else
		{
			ASSERT(iLength != 0);		// one character won't fit!!!!
			iLength -= iLastCharWidth;	// last one was OK
			return;
		}
	}
}


char* FindFurthestLineBreakIntl(CDC *pdc, int iMaxWidth, char *szString, CDWordArray *prgdwFormatting, int &iLength, int &iWidth, int &iHeight)
{
	BOOL	b;
	SIZE	size;
	int		nBytes = strlen(szString);
	CSize	sizeExtent = GetFormattedTextExtent(pdc, szString, nBytes, prgdwFormatting);

	if (sizeExtent.cx <= iMaxWidth)
	{
		iWidth  = sizeExtent.cx;
		iHeight = sizeExtent.cy;
		iLength = nBytes;
		return strchr(szString, g_chEOS); // return end of string
	}
	else
	{
		static short	nFixedPitchIndex = nGetSpecialFontIndex(TRUE);
		static short	nSymbolIndex = nGetSpecialFontIndex(FALSE);

		if (!FindSubStringForINTLThatFits(GetMime(),
										   pdc->m_hDC,
										   szString,
										   nBytes,
										   prgdwFormatting ? prgdwFormatting->GetData() : NULL,
										   prgdwFormatting ? prgdwFormatting->GetSize() : 0,
										   nFixedPitchIndex >= 0 ? FIXEDPITCHFACENAMES[nFixedPitchIndex] : NULL,
										   nSymbolIndex >= 0 ? SYMBOLFACENAMES[nSymbolIndex] : NULL,
										   &iLength,
										   &b,
										   &size,
										   iMaxWidth))
		{
			ForceLineBreak(pdc, szString, prgdwFormatting, iMaxWidth, iLength, iWidth, iHeight);
			return (szString + iLength);
		} 
		else
		{
			// REGISB added 10/15/97, make sure that fitting string does not end with spaces
			ASSERT(iLength > 0);
			char *szTmp = szString, *szFirstBreak = NULL;

			while (szTmp < szString+iLength)	// finding furthest character that starts a space sequence
			{
				if (my_isspace(*szTmp))
				{
					if (!szFirstBreak)
						szFirstBreak = szTmp;
				}
				else
					szFirstBreak = NULL;

				szTmp = CharNext(szTmp);
			}

			if (!szFirstBreak)
			{
				iWidth = size.cx;
				iHeight = size.cy;
				return szString + iLength;
			}
			else
			{
				sizeExtent = GetFormattedTextExtent(pdc, szString, szFirstBreak-szString, prgdwFormatting);
				iWidth  = sizeExtent.cx;
				iHeight = sizeExtent.cy;
				return szFirstBreak;
			}
		}
	}
}


char* FindFurthestLineBreak(CDC *pdc, int iMaxWidth, char *szString, CDWordArray *prgdwFormatting, int &iWidth, int &iHeight)
{
	if (GetMime())
	{
		int iLength;
		return FindFurthestLineBreakIntl(pdc, iMaxWidth, szString, prgdwFormatting, iLength, iWidth, iHeight);
	}

	char *szLastEnd, *szLineEnd = szString;
	
	while (TRUE)
	{
		szLastEnd = szLineEnd;
		szLineEnd = GetNextEnd(szLineEnd);

		int		iThisLength = szLineEnd - szString;
		CSize	sizeExtent = GetFormattedTextExtent(pdc, szString, iThisLength, prgdwFormatting);

		if (sizeExtent.cx <= iMaxWidth)
		{
			iWidth  = sizeExtent.cx;
			iHeight = sizeExtent.cy;
			if (!(*szLineEnd))		// ran over end
				return szLineEnd;
			continue;				// it fits -- try to fit more
		}
		else
		{							// doesn't fit
			if (szLastEnd == szString)
			{						// A line couldn't fit bbox constraint -- break it! (no hyphen)
				ForceLineBreak(pdc, szString, prgdwFormatting, iMaxWidth, iThisLength, iWidth, iHeight);
				szLastEnd = szString + iThisLength;
			}
			return szLastEnd;
		}
	}
}


//int BreakIntoLinesIntl(CDC *pdc, int iMaxWidth, char *szString, CDWordArray *prgdwFormatting, char *rgszStarts[], int rgiLengths[], int rgiWidths[] /*, int rgiHeights[]*/)
//{
//	int iWidth, iHeight = 0, nLines = 0;
//
//	while (TRUE)
//	{
//		char *szRest = FindFurthestLineBreakIntl(pdc, iMaxWidth, szString, prgdwFormatting, iWidth, iHeight);
//		rgszStarts[nLines] = szString;
//		rgiWidths[nLines] = iWidth;
//		// rgiHeights[nLines] = iHeight;
//		int nBytes = szRest ? szRest - szString : strlen(szString);
//		rgiLengths[nLines++] = nBytes;
//		if (!szRest || !*szRest)
//			return nLines;
//		if (nLines > MAXLINES)
//			return MAXLINES;   // can't format more than MAXLINES for now
//		szString = GetNextStart(szRest);
//	}
//}


int BreakIntoLines(CDC *pdc, int iMaxWidth, char *szString, CDWordArray *prgdwFormatting, char *rgszStarts[], int rgiLengths[], int rgiWidths[] /*, int rgiHeights[]*/)
{
// REGISB 10/14/97: The function BreakIntoLinesIntl is totally bogus - it does not take into account the carriage returns 
//					that might break the string (and also does not give the correct prgdwFormatting to FindFurthestLineBreakIntl).
//                  Using the US BreakIntoLines seems to work well however.
//	if (GetMime())	
//		return BreakIntoLinesIntl(pdc, iMaxWidth, szString, prgdwFormatting, rgszStarts, rgiLengths, rgiWidths /*, rgiHeights*/);

	int				nRet = 0, nLines = 0, iThisLength = 0, iLastLength = 0, iLastWidth, iLastHeight;
	char*			szLineEnd = szString;
	char*			szStart = szString;
	CDWordArray*	prgdwPulledFormatting = NULL;

	while (TRUE)
	{
		szLineEnd = GetNextEnd(szLineEnd);
		iLastLength = iThisLength;
		iThisLength = szLineEnd - szString;
		if (prgdwPulledFormatting)
		{
			prgdwPulledFormatting->RemoveAll();
			delete prgdwPulledFormatting;
		}
		prgdwPulledFormatting = PullFormattingOffsets(prgdwFormatting, szString - szStart);
		CSize sizeExtent = GetFormattedTextExtent(pdc, szString, iThisLength, prgdwPulledFormatting);
		BOOL bFoundReturn = UpcomingReturn(szLineEnd);
		if (sizeExtent.cx <= iMaxWidth && !bFoundReturn)
		{
			if (!(*szLineEnd))
			{  // ran over end
				rgszStarts[nLines]  = szString;
				rgiLengths[nLines]  = iThisLength;
				// rgiHeights[nLines]  = max(rgiHeights[nLines], sizeExtent.cy - 0x4B);
				rgiWidths[nLines++] = sizeExtent.cx;
				nRet = nLines;
				goto exit;
			}
			iLastWidth = sizeExtent.cx;
			iLastHeight = sizeExtent.cy;
			continue;				// it fits -- try to fit more
		}
		else
		{					// doesn't fit
			if (iLastLength == 0 && sizeExtent.cx > iMaxWidth)    // A line couldn't fit bbox constraint -- break it! (no hyphen)
			{
				if (prgdwPulledFormatting)
				{
					prgdwPulledFormatting->RemoveAll();
					delete prgdwPulledFormatting;
				}
				prgdwPulledFormatting = PullFormattingOffsets(prgdwFormatting, szString - szStart);
				// REGISB 10/14/97 added if statement for INTL cases
				if (GetMime())
					FindFurthestLineBreakIntl(pdc, iMaxWidth, szString, prgdwPulledFormatting, iLastLength, iLastWidth, iLastHeight);
				else
					ForceLineBreak(pdc, szString, prgdwPulledFormatting, iMaxWidth, iLastLength, iLastWidth, iLastHeight);
			}
			else if (bFoundReturn && sizeExtent.cx <= iMaxWidth)
			{
				iLastLength = iThisLength;
				iLastWidth = sizeExtent.cx;
				iLastHeight = sizeExtent.cy;
			}

			rgszStarts[nLines]	 = szString;
			rgiLengths[nLines]	 = iLastLength;
			// rgiHeights[nLines] = iLastHeight - 0x4B;
			rgiWidths[nLines++]	 = iLastWidth;
			szString = szLineEnd = GetNextStart(szString + iLastLength);
			if (!(*szString))
			{
				nRet = nLines;
				goto exit;
			}
			if (nLines >= MAXLINES)
			{
				nRet = MAXLINES;   // 0 fail (can't format more than MAXLINES for now)
				goto exit;
			}
		}
		iThisLength = 0;
	}

exit:
	if (prgdwPulledFormatting)
	{
		prgdwPulledFormatting->RemoveAll();
		delete prgdwPulledFormatting;
	}
	return nRet;
}


double randfloat()
{
	return (((double) rand()) / RAND_MAX);
}


// x and y are in balloon space
void BreakSpline(CBalloon *balloon, CSpline *spline, int x, int y, double oFactor)
{
	POINT left, leftNearest, rightNearest;
	int leftKnotIndex, rightKnotIndex;
	int nCps = spline->nCps;

	int gapwidth = (int) ((80 + 0 /*(int)(randfloat() * 15)*/) * oFactor);
	left.x = x - gapwidth;
	left.y = y;

	leftNearest = spline->ClosestPoint(left, &leftKnotIndex);
	rightNearest = spline->WalkHorizontalDistance(leftNearest, leftKnotIndex, leftNearest.x + 2*gapwidth, rightKnotIndex);

	POINT *newCps = (POINT *) malloc ((nCps + 2) * sizeof(POINT));

	newCps[0] = rightNearest;
	for (int i = 1; i <= nCps; i++)
		newCps[i] = spline->cps[(rightKnotIndex+i-2+nCps)%nCps];
	int nCpsNew = nCps + 2 - (rightKnotIndex - leftKnotIndex + nCps)%nCps;
	newCps[nCpsNew-1] = leftNearest;

	free(spline->cps);
	spline->cps = newCps;
	spline->nCps = nCpsNew;
	free(spline->bezpts);
	spline->bezpts = NULL;
	spline->closed = FALSE;
	spline->ComputeBezpts();
}


void GetFilters(CFormatInfo& fInfo, RANGE l[], RANGE r[], int& nL, int& nR)
{
	nL = 0;
	nR = 0;
	l[nL].x = fInfo.m_rgiLeftX[0];
	r[nR].x = fInfo.m_rgiLeftX[0] + fInfo.m_rgiWidths[0];
	l[nL].start = r[nR].start = 0;

	for (int i = 1; i < fInfo.m_nLines; i++) {
		int thisLeft = fInfo.m_rgiLeftX[i];
		int thisRight = fInfo.m_rgiLeftX[i] + fInfo.m_rgiWidths[i];
		int leftDelta = thisLeft - l[nL].x;
		int rightDelta = thisRight - r[nR].x;
		if (leftDelta <= THRESH1) {  // Extends dramatically to left
			l[nL].end = i-1;
			l[++nL].start = i;
			l[nL].x = thisLeft;
		} else if (leftDelta <= 0) {	// Extends marginally to left
			l[nL].x = thisLeft;
		} else if (leftDelta >= THRESH2) {	// Indents dramatically to right
			// check following line
			int nextLeft = (i+1 < fInfo.m_nLines) ? fInfo.m_rgiLeftX[i+1] : thisLeft;
			if (nextLeft - l[nL].x >= THRESH2) {  // Following line also indents dram. to right
				l[nL].end = i-1;
				l[++nL].start = i;
				l[nL].x = min(thisLeft, nextLeft);
			}
		}

		if (rightDelta >= -THRESH1) { // Extends dramatically to right
			r[nR].end = i-1;
			r[++nR].start = i;
			r[nR].x = thisRight;
		} else if (rightDelta >= 0) {		// Extends marginally to right
			r[nR].x = thisRight;
		} else if (rightDelta <= -THRESH2) {  // Indents dramatically to left
			int nextRight = (i+1 < fInfo.m_nLines) ? fInfo.m_rgiLeftX[i+1] + fInfo.m_rgiWidths[i+1] : thisRight;
			if (nextRight - r[nR].x <= -THRESH2) { // Following line also indents dram. to left
				r[nR].end = i-1;
				r[++nR].start = i;
				r[nR].x = max(thisRight, nextRight);
			}
		}
	} // end for

	l[nL++].end = r[nR++].end = fInfo.m_nLines-1;
}


int PermuteFilters(CFontInfo& fontI, RANGE lFilters[], RANGE rFilters[], int nLFilters, int nRFilters)
{
	int baseY = 0;
	int lastX = LARGEINTEGER;
	for (int i = 0; i < nLFilters; i++) {
		lFilters[i].x -= XBORDER;
		if (i == 0)
			lFilters[i].y = baseY + TOPBORDER + YBORDER + fontI.m_topOffset;
		else if (lFilters[i].x < lastX)
			lFilters[i].y = baseY + YBORDER;
		else lFilters[i].y = baseY - YBORDER - fontI.m_baseAdd; // since font is offset vertically
		baseY -= (lFilters[i].end - lFilters[i].start + 1) * fontI.m_lineHeight;
		lastX = lFilters[i].x;
	}
	
	baseY = 0;
	lastX = -LARGEINTEGER;
	for (i = 0; i < nRFilters; i++) {
		rFilters[i].x += XBORDER;
		if (i == 0)
			rFilters[i].y = baseY + TOPBORDER + YBORDER + fontI.m_topOffset;
		else if (rFilters[i].x > lastX)
			rFilters[i].y = baseY + YBORDER;
		else rFilters[i].y = baseY - YBORDER - fontI.m_baseAdd;
		baseY -= (rFilters[i].end - rFilters[i].start + 1) * fontI.m_lineHeight;
		lastX = rFilters[i].x;
	}
	return baseY - TOPBORDER - YBORDER - fontI.m_baseAdd;
}


void AddWavies(POINT& pt1, POINT& pt2, POINT *pts, int& nPts, int waveDiam, int interval)
{
	double dist = point_dist(pt1, pt2);
	double nWaves = dist / interval;
	if (nWaves < 2) return;
	int iWaves = (int) nWaves;
	double waveLen = dist / iWaves;
	DPOINT unitVec = point_scalmult(1.0 / dist, point_sub(point_to_dpoint(pt2), point_to_dpoint(pt1)));
	POINT incVec = dpoint_to_point(point_scalmult(waveLen, unitVec));
	DPOINT normalVec;
	normalVec.x = unitVec.y;
	normalVec.y = -unitVec.x;
	POINT extraVec = dpoint_to_point(point_scalmult((double) waveDiam, normalVec));
	POINT thisBase = pt1;
	for (int i = 0; i < iWaves-1; i++) { // - 2 since we only add wavies if dist > 2 intervals
		thisBase = point_add(thisBase, incVec);
		if (!(i&0x1)) pts[nPts++] = point_add(thisBase, extraVec);
		else pts[nPts++] = thisBase;
	}
}


void Dock(RECT &rect)
{
	int delta = TOPBORDER + YBORDER + HWAVEHEIGHT;
	rect.top += delta;
	rect.bottom += delta;
}


void Dock(SRECT &rect)
{
	int delta = TOPBORDER + YBORDER + HWAVEHEIGHT;
	rect.Top += delta;
	rect.Bottom += delta;
}


CFontInfo::CFontInfo(CFont *pFont, COLORREF crDefaultForeColor, short nLeading, short nBaseAdd)
{
	TEXTMETRIC	tm;
	CDC*		pdc = GetClientDC();
	CFont*		pOldFont = pdc->SelectObject(pFont);
	VERIFY(pdc->GetTextMetrics(&tm));

	m_font = pFont;
	m_crDefaultForeColor = crDefaultForeColor;

	// Commented out, since we need some wide interline spacing for accented ANSI_CHARSET langs.

/*	if (!leading /*&& NODESCENDERS(tm.tmCharSet)) {
		m_leading = (short) -tm.tmDescent;
		m_baseAdd = (short)tm.tmDescent;
	} else {
*/
	m_leading = nLeading;
	m_baseAdd = nBaseAdd;
//	}

	m_leading += (short) tm.tmExternalLeading;
	m_baseAdd -= (short) tm.tmExternalLeading;

#if 0
	if (FAREAST(tm.tmCharSet)) {
		m_topOffset = FAREAST_TOPOFFSET;
//		m_leading += FAREAST_ADJUSTMENT;
//		m_baseAdd -= FAREAST_ADJUSTMENT;
	}
	else
		m_topOffset = 0;
#endif

	if (nLeading)
		m_topOffset = 0;
	else
		m_topOffset = FAREAST_TOPOFFSET;

	m_lineHeight = (short) (tm.tmHeight + m_leading);
	
	CSize sizeExtent = pdc->GetTextExtent(szContinuationStr1, strlen(szContinuationStr1));
	m_continuationWidth = (short) sizeExtent.cx;
	pdc->SelectObject(pOldFont);
}


// CArrow members
CArrow::CArrow(const CArrow &a)
{
	m_mid = a.m_mid;
	m_hi = a.m_hi;
	m_lo = a.m_lo;
}


// CPanelElement members
CPanelElement::CPanelElement(const CPanelElement& p)
{
	m_bbox = p.m_bbox;
}


void CPanelElement::GetBBox(RECT *r)
{
	r->left = m_bbox.Left;
	r->top = m_bbox.Top;
	r->right = m_bbox.Right;
	r->bottom = m_bbox.Bottom;
}


// CLabel members
CLabel::CLabel(const CLabel& c) : CPanelElement(c)
{
	CDWordArray* CopyFormatting(CDWordArray*);
	if (c.m_str)
		m_str = strdup(c.m_str);
	m_fontI = c.m_fontI;
	m_format = c.m_format;
	m_prgdwFormatting = CopyFormatting(c.m_prgdwFormatting);
}


int CLabel::BreakIntoLines(CFormatInfo &fInfo)
{
	CDC*	pdc = GetClientDC();
	CFont*	pOldFont = pdc->SelectObject(m_fontI->m_font);
	int		iDesiredWidth = m_bbox.Right - m_bbox.Left;

	fInfo.m_nLines = ::BreakIntoLines(pdc, iDesiredWidth, m_str, m_prgdwFormatting, fInfo.m_rgszStarts,
									  fInfo.m_rgiLengths, fInfo.m_rgiWidths /*, fInfo.m_rgiHeights */);

	fInfo.m_iMaxWidth = 0;
	for (int i = 0; i < fInfo.m_nLines; i++)			// find widest line
		if (fInfo.m_iMaxWidth < fInfo.m_rgiWidths[i])
			fInfo.m_iMaxWidth = fInfo.m_rgiWidths[i];

	fInfo.m_bbox.Top = m_bbox.Top;
	if (m_format & FT_LEFT_JUSTIFY)
	{
		fInfo.m_bbox.Left = m_bbox.Left;
		fInfo.m_bbox.Right = m_bbox.Left + fInfo.m_iMaxWidth;
	}
	else
	{	// CENTER
		fInfo.m_bbox.Left = (iDesiredWidth - fInfo.m_iMaxWidth)/2 + m_bbox.Left;
		fInfo.m_bbox.Right = fInfo.m_bbox.Left + fInfo.m_iMaxWidth;
	}

	fInfo.m_bbox.Bottom = (short)(fInfo.m_bbox.Top - fInfo.m_nLines * m_fontI->m_lineHeight - m_fontI->m_baseAdd);

	//fInfo.m_bbox.Bottom = (short) (fInfo.m_bbox.Top - m_fontI->m_baseAdd);
	//for (i = 0; i < fInfo.m_nLines; i++)
	//	fInfo.m_bbox.Bottom -= fInfo.m_rgiHeights[i];

	pdc->SelectObject(pOldFont);
	return fInfo.m_nLines;
}


int CLabel::AreaEstimate(int *piLen, int *piLineHeight)
{
	CDC*	pdc = GetClientDC();
	CFont*	pOldFont = pdc->SelectObject(m_fontI->m_font);
	CSize	sizeExtent = GetFormattedTextExtent(pdc, m_str, strlen(m_str), m_prgdwFormatting);

	pdc->SelectObject(pOldFont);
	*piLen = sizeExtent.cx;
	*piLineHeight = m_fontI->m_lineHeight;

	return ((int) (1.3 * sizeExtent.cx * (sizeExtent.cy + *piLineHeight)));
}


int CLabel::WidestWord()
{
	CDC			*pdc = GetClientDC();
	CSize		sizeExtent;
	int			iMaxWidth = 0;
	char		*szEnd, *szStart = m_str;
	CDWordArray	*prgdwPulledFormatting = NULL;
	
	while (TRUE)
	{
		while (*szStart && !isprint(*szStart))			// szStart points to next printable character
			szStart = CharNext(szStart);
		if (!*szStart)
			break;
		szEnd = szStart;
		while(isprint(*szEnd))
			szEnd = CharNext(szEnd);
		// REGISB: original: sizeExtent = pdc->GetTextExtent(szStart, szEnd - szStart + 1);
		prgdwPulledFormatting = PullFormattingOffsets(m_prgdwFormatting, szStart - m_str);
		sizeExtent = GetFormattedTextExtent(pdc, szStart, szEnd - szStart + 1, prgdwPulledFormatting);
		if (prgdwPulledFormatting)
		{
			prgdwPulledFormatting->RemoveAll();
			delete prgdwPulledFormatting;
			prgdwPulledFormatting = NULL;
		}
		iMaxWidth = max(iMaxWidth, sizeExtent.cx);
		// now set up szStart for next iteraction
		if (!*szEnd)
			break;
		szStart = szEnd + 1;
	}

	return iMaxWidth;
}


void CLabel::ShiftLines(CFormatInfo &fInfo)
{
	int i, iShiftLimit, iShift;

	if (m_format & FT_LEFT_JUSTIFY)
	{
		for (i = 0; i < fInfo.m_nLines; i++)
		{
			iShiftLimit = fInfo.m_iMaxWidth - fInfo.m_rgiWidths[i];
			fInfo.m_rgiLeftX[i] = (int)(randfloat() * min(MAXLEFTSHIFT, iShiftLimit));
		}
	}
	else
	{
		for (i = 0; i < fInfo.m_nLines; i++)
		{
			iShiftLimit = (fInfo.m_iMaxWidth - fInfo.m_rgiWidths[i]) / 2;
			iShift = (int) ((randfloat() * 2.0 - 1.0) * min(MAXCENTERSHIFT, iShiftLimit));
			fInfo.m_rgiLeftX[i] = (((fInfo.m_bbox.Right - fInfo.m_bbox.Left)
								- fInfo.m_rgiWidths[i]) / 2) + iShift;
		}
	}
//	fInfo.m_bShifted = TRUE;
}


char* CLabel::SplitHeight(int iHeight, CDWordArray **pprgdwRestFormatting, char **pszURLStartInRest)
{
	ASSERT(pprgdwRestFormatting);

	int				rgiLengths[MAXLINES], rgiWidths[MAXLINES], /* rgiHeights[MAXLINES], */ iWidthD = 0, iHeightD = 0;
	char*			rgszStarts[MAXLINES];
	CClientDC*		pdc = GetClientDC();
	CDWordArray*	prgdwFormatting = NULL;

	CFont*			pOldFont = pdc->SelectObject(m_fontI->m_font);

	int				iDesiredWidth = m_bbox.Right - m_bbox.Left;
	int				iCumulHeight = 0, iMaxLines, nLines = ::BreakIntoLines(pdc, iDesiredWidth, m_str, m_prgdwFormatting, rgszStarts, rgiLengths, rgiWidths /*, rgiHeights */);

	*pprgdwRestFormatting = NULL;

	if (pszURLStartInRest)
		*pszURLStartInRest = NULL;

	iMaxLines = iHeight / m_fontI->m_lineHeight;

	//for (iMaxLines = 0; iMaxLines < MAXLINES; iMaxLines++)
	//{
	//	if (iCumulHeight + rgiHeights[iMaxLines] > iHeight)
	//		break;
	//	iCumulHeight += rgiHeights[iMaxLines];
	//}

	if (nLines < iMaxLines)
		return NULL;	// no rest, there is room enough...

	prgdwFormatting = PullFormattingOffsets(m_prgdwFormatting, rgszStarts[iMaxLines - 1] - m_str);

	char*			szEnd = FindFurthestLineBreak(pdc, m_bbox.Right - m_bbox.Left - m_fontI->m_continuationWidth,
												  rgszStarts[iMaxLines - 1], prgdwFormatting, iWidthD, iHeightD);
	int				nToCopy = szEnd - m_str;
	char*			szNewStr = (char*) malloc(nToCopy + strlen(szContinuationStr1) + 1);

	if (prgdwFormatting)
	{
		prgdwFormatting->RemoveAll();
		delete prgdwFormatting;
		prgdwFormatting = NULL;
	}

	strncpy(szNewStr, m_str, nToCopy);
	strcpy(szNewStr + nToCopy, szContinuationStr1);
	char *szRestStart = m_str + nToCopy;
	char *szRest = (char*) malloc(strlen(szRestStart) + strlen(szContinuationStr2) + 1);
	strcpy(szRest, szContinuationStr2);
	strcat(szRest, szRestStart);

	*pprgdwRestFormatting = PullFormattingOffsets(m_prgdwFormatting, szRestStart - m_str);

	free(m_str);
	m_str = szNewStr;

	// adjust m_prgdwFormatting if necessary
	if (m_prgdwFormatting)
	{
		m_prgdwFormatting = CutFormattingArray(m_prgdwFormatting, nToCopy);
		// add an element with wFormat = 0 for the szContinuationStr1
		if (m_prgdwFormatting)
		{
			//DWORD dwLastElement = m_prgdwFormatting->GetAt(m_prgdwFormatting->GetUpperBound());
			//if (LOWORD(dwLastElement) != 0)
			//	// continuation of formatting
			//	*pprgdwRestFormatting = InsertFormat(*pprgdwRestFormatting, TRUE, LOWORD(dwLastElement), 0);

			m_prgdwFormatting->Add(MAKELONG(0, nToCopy));
		}
	}

	PushFormattingOffsets(*pprgdwRestFormatting, strlen(szContinuationStr2));	// add offset of ...

	pdc->SelectObject(pOldFont);
	return szRest;
}


void CLabel::Draw(CDC *pdc, POINT *ul, RECT *dmgArea)
{
	int			rgiLengths[MAXLINES], rgiWidths[MAXLINES], /* rgiHeights[MAXLINES], */ rgiLeftX[MAXLINES];
	char*		rgszStarts[MAXLINES];

	CFont*		pOldFont = pdc->SelectObject(m_fontI->m_font);
	COLORREF	crOldColor = pdc->SetTextColor(m_fontI->m_crDefaultForeColor);

	int			iDesiredWidth = m_bbox.Right - m_bbox.Left;
	int			nLines = ::BreakIntoLines(pdc, iDesiredWidth, m_str, m_prgdwFormatting, rgszStarts, rgiLengths, rgiWidths /*, rgiHeights */);
	int			iBaseY = m_bbox.Top;

	pdc->SetBkMode(TRANSPARENT);

	for (int i = 0; i < nLines; i++)
	{
		if (m_format & FT_LEFT_JUSTIFY)
			rgiLeftX[i] = m_bbox.Left;
		else
			rgiLeftX[i] = m_bbox.Left + ((CUnitPanelPage::m_unitWidth - rgiWidths[i]) / 2); // Kludge - fix
	}

	if (m_prgdwFormatting && m_prgdwFormatting->GetSize())
		DrawFormattedText(pdc, rgszStarts, rgiLeftX, rgiLengths, /* rgiHeights, */ nLines, iBaseY);
	else
		for (int i = 0; i < nLines; i++)
		{
			pdc->TextOut(rgiLeftX[i],
						 iBaseY,
						 rgszStarts[i],
						 rgiLengths[i]);

			//iBaseY -= rgiHeights[i];
			iBaseY -= m_fontI->m_lineHeight;
		}

	pdc->SelectObject(pOldFont);
	pdc->SetTextColor(crOldColor);
}


void CLabel::DrawFormattedText(CDC *pdc, char* rgszStarts[], int rgiLeftX[], int rgiLengths[], /* int rgiHeights[], */ int nLines, int iStartTop, int *piURL)
{
	char*	szChunkStart = m_str;
	int		iChunkLen = 0, iLeftX, iFormatLen, iStringLen = strlen(m_str), iBaseY = iStartTop, iURL = -1;
	int		iFormattingIndex, iUpper = m_prgdwFormatting->GetUpperBound(), iCurInLineIndex = 0, iCurLineIndex = 0;
	DWORD	dwElement;
	WORD	wFormat, wOffset;
	BOOL	bInURL = FALSE, bURLHit = (piURL != NULL);

	ASSERT(iUpper >= 0);
 
	// number of lines to display: nLines

	// first there might be an initial chunk with no formatting
	iLeftX    = rgiLeftX[0];
	dwElement = m_prgdwFormatting->GetAt(0);
	wFormat   = LOWORD(dwElement);
	wOffset   = HIWORD(dwElement);

	if (wOffset > 0)
	{
		while (rgszStarts[iCurLineIndex] - m_str + iCurInLineIndex < wOffset && iCurLineIndex < nLines)
		{
			szChunkStart = rgszStarts[iCurLineIndex];
			iChunkLen = min(wOffset + m_str - rgszStarts[iCurLineIndex], rgiLengths[iCurLineIndex]);
			iLeftX += iDrawFormattedTextLine(pdc, iLeftX, iBaseY, szChunkStart, iChunkLen, 0, &bURLHit);
			if (iChunkLen == rgiLengths[iCurLineIndex])
			{
				//iBaseY -= rgiHeights[iCurLineIndex];
				iCurLineIndex++;
				iCurInLineIndex = 0;
				iLeftX = rgiLeftX[iCurLineIndex];
				iBaseY -= m_fontI->m_lineHeight;
			}
			else
				iCurInLineIndex = iChunkLen;
		}
	}

	for (iFormattingIndex = 0; iFormattingIndex <= iUpper; iFormattingIndex++)
	{
		dwElement = m_prgdwFormatting->GetAt(iFormattingIndex);
		wFormat = LOWORD(dwElement);
		wOffset = HIWORD(dwElement);

		if ((wFormat & wLink) && !bInURL)
			iURL++;
		bInURL = wFormat & wLink;
		
		if (iUpper > iFormattingIndex)
			iFormatLen = HIWORD(m_prgdwFormatting->GetAt(iFormattingIndex+1)) - wOffset;
		else
			iFormatLen = iStringLen - wOffset;

		while (szChunkStart + iChunkLen < m_str + wOffset + iFormatLen && iCurLineIndex < nLines)
		{
			szChunkStart = rgszStarts[iCurLineIndex] + iCurInLineIndex;
			iChunkLen = min(iFormatLen + wOffset + m_str - szChunkStart, rgiLengths[iCurLineIndex] - iCurInLineIndex);
			if (iChunkLen > 0)
			{
				bURLHit = (piURL != NULL);
				iLeftX += iDrawFormattedTextLine(pdc, iLeftX, iBaseY, szChunkStart, iChunkLen, wFormat, &bURLHit);
				iCurInLineIndex += iChunkLen;
				if (bURLHit)
				{
					*piURL = iURL;
					return;
				}
			}
			if (iCurInLineIndex >= rgiLengths[iCurLineIndex])
			{
				ASSERT(iCurInLineIndex == rgiLengths[iCurLineIndex]);
				//iBaseY -= rgiHeights[iCurLineIndex];
				iCurLineIndex++;
				iCurInLineIndex = 0;
				ASSERT(iCurLineIndex <= nLines);
				iLeftX = rgiLeftX[iCurLineIndex];
				iBaseY -= m_fontI->m_lineHeight;
			}
		}
	}
}


int CLabel::iDrawFormattedTextLine(CDC *pdc, int iLeftX, int iBaseY, char *szChunkStart, int iChunkLen, WORD wFormat, BOOL *pbURLHit)
{
	BOOL			bTransparency = FALSE;
	COLORREF		crOldColor;
	LOGFONT			logFont;
	CSize			size;
	CFont			font, *pOldFont = NULL;
	char			*szChunkTmp = NULL;
	static short	nFixedPitchIndex = nGetSpecialFontIndex(TRUE);
	static short	nSymbolIndex = nGetSpecialFontIndex(FALSE);
				
	ASSERT(pdc);

	if (wFormat & wLink)
		crOldColor = pdc->SetTextColor(linkColor);
	else
	{
		COLORREF crFGTmp = m_fontI->m_crDefaultForeColor; 
		if (wFormat & wForeground)
		{
			COLORREF crForeground = GetRBGColor((wFormat >> 4) & 0x000F);
			if (crForeground != GetSysColor(COLOR_WINDOW))
				crFGTmp = crForeground;

			if ((wFormat & wBackground) &&
				((wFormat >> 4) & 0x000F) == (wFormat & 0x000F))
			{
				// Sender wants transparency
				crFGTmp = crForeground;
				bTransparency = TRUE;
			}
		}
		crOldColor = pdc->SetTextColor(crFGTmp);
	}

	m_fontI->m_font->GetLogFont(&logFont);
	if (wFormat & wBold)
		logFont.lfWeight = 700;
	if (wFormat & wItalic)
		logFont.lfItalic = TRUE;
	if (wFormat & wUnderline)
		logFont.lfUnderline = TRUE;
	if (nFixedPitchIndex >= 0 && (wFormat & wFixedPitch))
	{
		_tcscpy(logFont.lfFaceName, FIXEDPITCHFACENAMES[nFixedPitchIndex]);
		logFont.lfPitchAndFamily &= ~VARIABLE_PITCH;
		logFont.lfPitchAndFamily |= FIXED_PITCH;
	}
	if (nSymbolIndex >= 0 && (wFormat & wSymbol))
	{
		_tcscpy(logFont.lfFaceName, SYMBOLFACENAMES[nSymbolIndex]);
		logFont.lfPitchAndFamily &= ~VARIABLE_PITCH;
		logFont.lfPitchAndFamily &= ~FIXED_PITCH;
		logFont.lfPitchAndFamily |= DEFAULT_PITCH;
	}

	font.CreateFontIndirect(&logFont);
	pOldFont = pdc->SelectObject(&font);

	if (bTransparency)
	{
		if (!(szChunkTmp = new char[iChunkLen+1]))
			return 0;
		FillMemory(szChunkTmp, iChunkLen, g_chTransparent);
		szChunkTmp[iChunkLen] = g_chEOS;

		size = pdc->GetTextExtent(szChunkTmp, iChunkLen);
	}
	else
		size = pdc->GetTextExtent(szChunkStart, iChunkLen);

	if (*pbURLHit)
	{
		// we want to check if URL was hit
		if (wFormat & wLink)
			*pbURLHit = bURLHit(iLeftX, iBaseY, size);
		else
			*pbURLHit = FALSE;
	}
	else
	{
		// we want to display the text
		pdc->TextOut(iLeftX, iBaseY - (m_fontI->m_lineHeight - size.cy) / 2, szChunkTmp ? szChunkTmp : szChunkStart, iChunkLen);
	}

	pdc->SetTextColor(crOldColor);
	if (pOldFont)
		pdc->SelectObject(pOldFont);

	if (szChunkTmp)
		delete [] szChunkTmp;

	return size.cx;
}


BOOL CLabel::bURLHit(int iLeftX, int iBaseY, CSize &size)
{
	RECT r;
	int iLeft = m_bbox.Left + iLeftX;
	int iTop = m_bbox.Top + iBaseY;
	SetRect(&r, iLeft, iTop, iLeft + size.cx, iTop - size.cy);
	return inside_bbox(&mouseDownPt, &r);
}


void CLabel::GetBBox(RECT *r)
{
	CFormatInfo fInfo;
	BreakIntoLines(fInfo);

	r->left = fInfo.m_bbox.Left;
	r->top = fInfo.m_bbox.Top;
	r->right = fInfo.m_bbox.Right;
	r->bottom = fInfo.m_bbox.Bottom;
}


// CBalloon members
CBalloon::CBalloon(const char *szText, CFontInfo *pFontInfo, CDWordArray *prgdwFormatting, const char *szURLStart) : CLabel(szText, pFontInfo, prgdwFormatting)
{
	m_fInfo			= NULL;
	m_spline		= NULL;
	m_traj			= NULL;
	m_prgszURLs		= NULL;
	m_trueBox.Left	= m_trueBox.Right = m_trueBox.Top = m_trueBox.Bottom = -1;  // for debugging -- indicates not set

	CreateURLArray(m_str, m_prgdwFormatting, szURLStart);
}


CBalloon::CBalloon(const CBalloon& b) : CLabel(b)
{
	if (b.m_fInfo) {
		m_fInfo = new CFormatInfo;
		*m_fInfo = *b.m_fInfo;
	} else m_fInfo = NULL;
	
	m_speaker	= NULL;	// b.m_speaker ? b.m_speaker->Clone() : NULL; //  // don't clone the speaker
	m_traj		= NULL;	// for now, don't clone traj
	m_prgszURLs	= NULL;	// REGISB: fix me?  is this constructor ever used??  YES IT IS!

	m_spline	= b.m_spline ? b.m_spline->Clone() : NULL;
	m_trueBox	= b.m_trueBox;
}


CBalloon::~CBalloon()
{
	if (m_fInfo)
		delete m_fInfo;
	if (m_spline)
		delete m_spline;
	if (m_traj)
		delete m_traj;
	if (m_prgszURLs)
	{
		for (int i = 0; i < MAX_URL_INTEXT; i++)
			if (m_prgszURLs[i])
				delete [] m_prgszURLs[i];
		delete [] m_prgszURLs;
	}
}


void CBalloon::GetBBox(RECT *r)
{
	// this isn't exactly right -- fix
	GetCloudBBox(r);
	r->bottom = min(r->bottom, m_speaker->m_bbox.Top + 200);  // include tail pt. (hack)
}


void CBalloon::DockAtTop(int height)
{
	int oldBBoxHeight = m_bbox.Top - m_bbox.Bottom;

	m_bbox.Top = height + TOPBORDER;
	m_bbox.Bottom = m_bbox.Top - oldBBoxHeight;
}


void CBalloon::DrawText(CDC *pdc)
{
	if (m_prgdwFormatting && m_prgdwFormatting->GetSize())
	{	// this is rare, so call a separate function when this occurs
		DrawFormattedText(pdc, m_fInfo->m_rgszStarts, m_fInfo->m_rgiLeftX, m_fInfo->m_rgiLengths, /* m_fInfo->m_rgiHeights, */ m_fInfo->m_nLines, 0);
		return;
	}

	int iBaseY = 0;
	for (int i = 0; i < m_fInfo->m_nLines; i++)
	{
		pdc->TextOut(m_fInfo->m_rgiLeftX[i],
					iBaseY,
					m_fInfo->m_rgszStarts[i],
					m_fInfo->m_rgiLengths[i]);
		//iBaseY -= m_fInfo->m_rgiHeights[i];
		iBaseY -= m_fontI->m_lineHeight;
	}
}


void CBalloon::OnLButtonDown(POINT &pt)
{
	if (m_prgdwFormatting && bURLPresent(m_prgdwFormatting))
	{
		int iURL = -1;
		mouseDownPt = pt;
		DrawFormattedText(GetClientDC(), m_fInfo->m_rgszStarts, m_fInfo->m_rgiLeftX, m_fInfo->m_rgiLengths, /* m_fInfo->m_rgiHeights, */ m_fInfo->m_nLines, 0, &iURL);
		if (iURL >= 0 && m_prgszURLs && m_prgszURLs[iURL])
			FLaunchBrowser(m_prgszURLs[iURL]);
	}
}


void CBalloon::CreateURLArray(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart)
{
	ASSERT(szText);
	ASSERT(!m_prgszURLs);

	if (!bURLPresent(prgdwFormatting))
	{
		ASSERT(!szURLStart);
		return;
	}
	
	m_prgszURLs = new char*[MAX_URL_INTEXT];
	if (!m_prgszURLs)
		return;

	for (int i = 0; i < MAX_URL_INTEXT; i++)
		m_prgszURLs[i] = NULL;

	int			iUpper = prgdwFormatting->GetUpperBound();
	int			iFormatIndex, iURLCnt = -1;
	BOOL		bInURL = FALSE;
	WORD		wFormat;
	const char	*szURLBegin, *szURLEnd;

	for (iFormatIndex = 0; iFormatIndex <= iUpper; iFormatIndex++)
	{
		wFormat = LOWORD(prgdwFormatting->GetAt(iFormatIndex));
		if (!bInURL && (wFormat & wLink))
		{
			// reached a beginning of URL
			bInURL = TRUE;
			iURLCnt++;
			szURLBegin = szText + HIWORD(prgdwFormatting->GetAt(iFormatIndex));
		}
		else
		{
			if (bInURL && !(wFormat & wLink))
			{
				// reached end of URL
				bInURL = FALSE;
				szURLEnd = szText + HIWORD(prgdwFormatting->GetAt(iFormatIndex));
				if (iURLCnt < MAX_URL_INTEXT)
				{
					// need to add URL to URL array
					if (0 == iURLCnt && szURLStart)
					{
						m_prgszURLs[iURLCnt] = new char[strlen(szURLStart) + 1];
						if (!m_prgszURLs[iURLCnt])
							return;
						strcpy(m_prgszURLs[iURLCnt], szURLStart);
					}
					else
					{
						m_prgszURLs[iURLCnt] = new char[szURLEnd - szURLBegin + 1];
						if (!m_prgszURLs[iURLCnt])
							return;
						strncpy(m_prgszURLs[iURLCnt], szURLBegin, szURLEnd - szURLBegin);
						m_prgszURLs[iURLCnt][szURLEnd - szURLBegin] = '\0';
					}
				}
			}
		}
	}
	if (bInURL)
	{
		const char*	szTmp;

		if (0 == iURLCnt && szURLStart)
			szTmp = szURLStart;
		else
			szTmp = szURLBegin;

		m_prgszURLs[iURLCnt] = new char[strlen(szTmp) + 1];
		if (!m_prgszURLs[iURLCnt])
			return;
		strcpy(m_prgszURLs[iURLCnt], szTmp);
	}
}


void CBalloon::QueryRouteRgn(int OtherToX, int &leftAllowance, int &rightAllowance)
{
	int toX = m_speaker->m_arrowX;

	if (OtherToX > toX)
	{		// other balloon's toX is to right of this balloon's
		leftAllowance = max(toX, m_routeRgn.Left + MINROUTEWIDTH);
		rightAllowance = LARGEINTEGER;
	}
	else
	{					// other balloon's toX is to left of this balloon's
		leftAllowance = -LARGEINTEGER;
		rightAllowance = min(toX, m_routeRgn.Right - MINROUTEWIDTH);
	}
}


void CBalloon::SetRouteRgn(int OtherToX, int left, int right)
{
	SRECT *speakerBox = &(m_speaker->m_bbox);
	int toX = m_speaker->m_arrowX;

	if (OtherToX > toX)			// other balloon's toX is to right of this balloon's
		m_routeRgn.Right = min(m_routeRgn.Right, left);
	else
		m_routeRgn.Left = max(m_routeRgn.Left, right);
}


BOOL CBalloon::Overlap(CBalloon *b2)
{
	RECT cb1, cb2;
	GetCloudBBox(&cb1);
	b2->GetCloudBBox(&cb2);
	return (bbox_overlap(&cb1, &cb2));
}


BOOL CBalloon::SetBBox(int left, int bottom, int right, int top)
{
	if (m_bbox.Right - m_bbox.Left != right - left ||
		m_bbox.Top - m_bbox.Bottom != top - bottom) // just change origin
	{
		m_bbox.Left = 0;
		m_bbox.Right = (right - left) - 2*XBORDER;	// estimate
		m_bbox.Top = 0;

		// note: this ignores m_bbox, except to calculate width
		// it will fail if we can't set the bbox width this small for this balloon
		if (!ComputeInternals())
			return FALSE;

		// a reasonable bottom may not be provided.  calculate it based on other parameters.
		bottom = top + m_trueBox.Bottom - m_trueBox.Top;  // make a reasonable estimate
	}

	// Adjust bbox and origin accordingly
	m_bbox.Left = left - m_trueBox.Left;
	m_bbox.Right = right - m_trueBox.Left;
	m_bbox.Top = top - m_trueBox.Top;
	m_bbox.Bottom = bottom - m_trueBox.Top;
	return TRUE;
}


void CBalloon::InMyCoords(SRECT *bbox)
{
	bbox->Left -= m_bbox.Left;
	bbox->Right -= m_bbox.Left;
	bbox->Top -= m_bbox.Top;
	bbox->Bottom -= m_bbox.Top;
}


void CBalloon::ComputeCloudBBox()
{ // called only to compute it from scratch
	make_empty(&m_trueBox);
	for (int i = 0; i < m_spline->nCps; i++)
		include_pt_in_bbox(&m_spline->cps[i], &m_trueBox);
}


// provides true cloud bbox (not necessarily what was explicitly "Set")
void CBalloon::GetCloudBBox(RECT *r)
{
	r->left = m_trueBox.Left + m_bbox.Left;
	r->top = m_trueBox.Top + m_bbox.Top;
	r->right = m_trueBox.Right + m_bbox.Left;
	r->bottom = m_trueBox.Bottom + m_bbox.Top;
}


void CBalloon::GetCloudBBox(SRECT *r)
{
	r->Left = m_trueBox.Left + m_bbox.Left;
	r->Top = m_trueBox.Top + m_bbox.Top;
	r->Right = m_trueBox.Right + m_bbox.Left;
	r->Bottom = m_trueBox.Bottom + m_bbox.Top;
}


// CBWoodringNormal members
CBWoodringNormal::CBWoodringNormal(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart) : CBalloon(szText, CUnitPanelPage::m_fiWNormal, prgdwFormatting, szURLStart)
{
	Capitalize(m_str);
}

void CBWoodringNormal::AddArrow(CBalloon *balloon, CSpline *spline, CFormatInfo &fInfo) {
	POINT left, right, bottom, bottom2, top2;
	SRECT *routeRgn = &(balloon->m_routeRgn);

	bottom2.x = balloon->m_speaker->m_arrowX;
	bottom2.y = balloon->m_speaker->m_bbox.Top + 200;
	ASSERT(bottom2.x > 150);
	bottom.x = bottom2.x - balloon->m_bbox.Left;		// bottom is in balloon's coord system
	bottom.y = bottom2.y - balloon->m_bbox.Top;

	// for now, choose middle of routeRgn as hook break entry
//	_RPT2(_CRT_WARN, "route left = %d, route right = %d\n", routeRgn->Left, routeRgn->Right);
	SRECT cbbox;
	GetCloudBBox(&cbbox);		// this is an estimate, used for opening adjustment
	int xbreak = ((routeRgn->Left + routeRgn->Right) / 2) - balloon->m_bbox.Left;
	int bottomStart = m_fInfo->m_rgiLeftX[m_fInfo->m_nLines-1];
	int bottomEnd = bottomStart + m_fInfo->m_rgiWidths[m_fInfo->m_nLines-1];
#if 0
	POINT tpoint;
	tpoint.x = bottomStart;
	tpoint.y = fInfo.m_bbox.Bottom;
	extern CDC *pnlDC;
	DrawPoint(pnlDC, tpoint);
	tpoint.x = bottomEnd;
	DrawPoint(pnlDC, tpoint);
#endif

	if (xbreak < bottomStart && bottomStart + balloon->m_bbox.Left < routeRgn->Right - LARGEDELTA)
		xbreak = bottomStart + SMALLDELTA;
	else if (xbreak > bottomEnd && bottomEnd + balloon->m_bbox.Left > routeRgn->Left + LARGEDELTA)
		xbreak = bottomEnd - SMALLDELTA;

	top2.x = xbreak + balloon->m_bbox.Left;
	top2.y = cbbox.Bottom;

	if (top2.y - bottom2.y < MINTAILHEIGHT)	 {	// ensure tail is minimum height
		bottom2.y = top2.y - MINTAILHEIGHT;
		bottom.y = bottom2.y - balloon->m_bbox.Top;
	}

	double ang = vector_to_angle(point_sub(top2, bottom2));
//	TRACE("Top = (%d, %d), Bottom = (%d, %d), delta = (%d, %d).\n", top2.x, top2.y, bottom2.x, bottom2.y, top2.x-bottom2.x, top2.y-bottom2.y);
//	TRACE("Angle = %f. from vertical = %f. in degrees = %f\n", ang, ang - PI/2, (ang - PI/2) * 180/PI );
//	ASSERT(oFactor < 3.0);

	// if angle is too great, bring xbreak closer to char (limit angle to 45 degrees)
	if (fabs(ang) - PI/2.0 > PI/4.0) {
		if (ang > 3*PI/4.0)
			ang = 3*PI/4.0;
		else ang = PI/4.0;
		int heightDelta = top2.y - bottom2.y;
		xbreak = (int)(cos(ang) * heightDelta + bottom2.x - balloon->m_bbox.Left);
	}

	double oFactor = 1.0; // 1.0 / cos(ang - PI/2.0);

	BreakSpline(balloon, spline, xbreak, fInfo.m_bbox.Bottom, oFactor);
//	TRACE("oFactor = %f.\n", oFactor);

	left = spline->cps[spline->nCps - 1];
	right = spline->cps[0];
	top2.y = (left.y + right.y) / 2 + balloon->m_bbox.Top;
	top2.x = (left.x + right.x) / 2 + balloon->m_bbox.Left;

	// assume that m_lo is already current point
	// m_mid is in panel coordinates, while m_lo and m_mid are in balloon coordinates
	int tailLen = (int) point_dist(top2, bottom2);
	int alt = (int) (.05 * tailLen);
	int sign = bottom.x > left.x ? 1 : -1;
	CArc *arc = new CArc(left, bottom, sign*alt);
	m_traj->AddSeg(arc);
	arc = new CArc(bottom, right, -sign*alt);
	m_traj->AddSeg(arc);
}


char* CBWoodringNormal::SplitHeight(int iHeight, CDWordArray **pprgdwRestFormatting, char **pszURLStartInRest)
{
	ASSERT(pprgdwRestFormatting);
	ASSERT(pszURLStartInRest);

	CDWordArray*	prgdwFormatting = NULL;
	CClientDC*		pdc = GetClientDC();
	CFont*			pOldFont = NULL;
	int				iMaxLines, iCumulHeight = 0, iWidthD = 0, iHeightD = 0;  // dummy params

	*pprgdwRestFormatting = NULL;
	*pszURLStartInRest = NULL;

	iMaxLines = (iHeight - BORDERFUDGE) / m_fontI->m_lineHeight;

	
	//for (iMaxLines = 0; iMaxLines < MAXLINES; iMaxLines++)
	//{
	//	if (iCumulHeight + m_fInfo->m_rgiHeights[iMaxLines] > iHeight - BORDERFUDGE)
	//		break;
	//	iCumulHeight += m_fInfo->m_rgiHeights[iMaxLines];
	//}

	if (iMaxLines >= m_fInfo->m_nLines)
		return NULL;

	// OK, we really have to do the split...
	m_fInfo->m_nLines = iMaxLines;

	pOldFont = pdc->SelectObject(m_fontI->m_font);

	prgdwFormatting = PullFormattingOffsets(m_prgdwFormatting, m_fInfo->m_rgszStarts[iMaxLines - 1] - m_str);

	char *szEnd = FindFurthestLineBreak(pdc, m_fInfo->m_bbox.Right - m_fInfo->m_bbox.Left - m_fontI->m_continuationWidth,
										m_fInfo->m_rgszStarts[iMaxLines - 1], prgdwFormatting, iWidthD, iHeightD);
	if (prgdwFormatting)
	{
		prgdwFormatting->RemoveAll();
		delete prgdwFormatting;
		prgdwFormatting = NULL;
	}

	int		nToCopy = szEnd - m_str;
	int contStr1Len = strlen(szContinuationStr1);

	// ensure that a line contains more than just continuation characters
	if (nToCopy <= contStr1Len && strncmp(m_str, szContinuationStr1, contStr1Len) == 0 && m_str[contStr1Len]) {
		nToCopy = contStr1Len+1;
		szEnd = m_str + nToCopy;
	}
		
	char	*szNewStr = (char*) malloc(nToCopy + contStr1Len + 1);
	strncpy(szNewStr, m_str, nToCopy);
	strcpy(szNewStr+nToCopy, szContinuationStr1);
	char	*szRestStart = GetNextStart(m_str + nToCopy);
	char	*szRest = (char*) malloc(strlen(szRestStart) + strlen(szContinuationStr2) + 1);
	strcpy(szRest, szContinuationStr2);
	strcat(szRest, szRestStart);

	*pprgdwRestFormatting = PullFormattingOffsets(m_prgdwFormatting, szRestStart - m_str);

	free(m_str);
	m_str = szNewStr;

	// adjust m_prgdwFormatting if necessary
	if (m_prgdwFormatting)
	{
		DWORD	dwLastElement;
		BOOL	bFirstRestCharNotURL;
		// is the first character of szRest starting a non-URL substring?
		m_prgdwFormatting = CutFormattingArray(m_prgdwFormatting, nToCopy+1);
		if (m_prgdwFormatting)
		{
			dwLastElement = m_prgdwFormatting->GetAt(m_prgdwFormatting->GetUpperBound());
			bFirstRestCharNotURL = !(LOWORD(dwLastElement) & wLink) && HIWORD(dwLastElement) == nToCopy;
		}

		m_prgdwFormatting = CutFormattingArray(m_prgdwFormatting, nToCopy);

		if (m_prgdwFormatting)
		{
			dwLastElement = m_prgdwFormatting->GetAt(m_prgdwFormatting->GetUpperBound());
			if ((LOWORD(dwLastElement) & wLink) && !bFirstRestCharNotURL)
			{
				// a URL is being broken, need to hand off the URL start to next balloon
				// get the correct URL
				ASSERT(m_prgszURLs);

				DWORD	dwLastURLStart, dwFirstURLEnd, dwElement;
				BOOL	bInURL = FALSE;
				char	*szURL, *szTmp;

				if (!(szURL = (char*) new char[serverConn.m_nMaxMsgLength]))	// more than enough
					return NULL;
				ZeroMemory(szURL, serverConn.m_nMaxMsgLength);

				for (int iFormatIndex = 0; iFormatIndex <= m_prgdwFormatting->GetUpperBound(); iFormatIndex++)
				{
					dwElement = m_prgdwFormatting->GetAt(iFormatIndex);
					if (!bInURL && (LOWORD(dwElement) & wLink))
					{
						bInURL = TRUE;
						dwLastURLStart = dwElement;
					}
					else
						if (bInURL && !(LOWORD(dwElement) & wLink))
							bInURL = FALSE;
				}

				strcpy(szURL, m_str + HIWORD(dwLastURLStart));
				szURL[strlen(szURL) - strlen(szContinuationStr1)] = '\0';

				if (*pprgdwRestFormatting)
				{
					// try to find the end of the URL 
					for (iFormatIndex = 0; iFormatIndex <= (*pprgdwRestFormatting)->GetUpperBound(); iFormatIndex++)
					{
						dwElement = (*pprgdwRestFormatting)->GetAt(iFormatIndex);
						if (!(LOWORD(dwElement) & wLink))
						{
							dwFirstURLEnd = dwElement;
							break;
						}
					}
					if (iFormatIndex != 0)
					{
						strncat(szURL, szRest + strlen(szContinuationStr2), HIWORD(dwFirstURLEnd));

						for (int i = 0; i < MAX_URL_INTEXT; i++)
							if (m_prgszURLs[i])
							{
								if (szTmp = strdup(m_prgszURLs[i]))
								{
									Capitalize(szTmp);
									if (strstr(szTmp, szURL))
									{
										free(szTmp);
										break;
									}
									free(szTmp);
								}
							}
						ASSERT(i < MAX_URL_INTEXT && m_prgszURLs[i]);
						*pszURLStartInRest = new char[strlen(m_prgszURLs[i])+1];
						if (*pszURLStartInRest)
							strcpy(*pszURLStartInRest, m_prgszURLs[i]);
					}
				}
				ASSERT(szURL);
				delete [] szURL;
			}

			// add an element with wFormat = 0 for the szContinuationStr1
			m_prgdwFormatting->Add(MAKELONG(0, nToCopy));
		}
	}

	PushFormattingOffsets(*pprgdwRestFormatting, strlen(szContinuationStr2));	// add offset of ...

	// recompute m_fInfo...  Keep in mind that m_bbox has been offset by trueBox
	//    since original call to SetBBox, so we must compensate.  Also, value isn't
	//    really recomputed unless bbox dimensions change, so hack this.
	//    (This is a major hack -- should be made more elegant.)
	m_bbox.Left--;
	SetBBox(m_bbox.Left + m_trueBox.Left + 1, m_bbox.Bottom + m_trueBox.Top,
			m_bbox.Right + m_trueBox.Left, m_bbox.Top + m_trueBox.Top);

	pdc->SelectObject(pOldFont);
	return szRest;
}


CSpline* CBWoodringNormal::CreateBalloonSpline(CFormatInfo& fInfo)
{
	RANGE lFilters[20], rFilters[20];
	POINT pts[MAXPTS], nextPoint, thisPoint;
	int nLFilters, nRFilters, nPts = 0, finalY, lastY;

	GetFilters(fInfo, lFilters, rFilters, nLFilters, nRFilters);
	lastY = finalY = PermuteFilters(*m_fontI, lFilters, rFilters, nLFilters, nRFilters);
	// fill pts vector w/ corners of tightly-binding text box
	for (int i = 0; i < nLFilters; i++) {
		thisPoint.x = nextPoint.x = lFilters[i].x;
		thisPoint.y = lFilters[i].y;
		if (i > 0) AddWavies(pts[nPts-1], thisPoint, pts, nPts, HWAVEHEIGHT, HWAVEINTERVAL);
		pts[nPts++] = thisPoint;
		nextPoint.y = (i == nLFilters-1) ? finalY : lFilters[i+1].y;
		AddWavies(pts[nPts-1], nextPoint, pts, nPts, VWAVEHEIGHT, VWAVEINTERVAL);
		pts[nPts++] = nextPoint;
	}

	for (i = nRFilters-1; i >= 0; i--) {
		thisPoint.x = nextPoint.x = rFilters[i].x;
		thisPoint.y = lastY;
		AddWavies(pts[nPts-1], thisPoint, pts, nPts, HWAVEHEIGHT, HWAVEINTERVAL);
		pts[nPts++] = thisPoint;
		nextPoint.x = thisPoint.x;
		lastY = nextPoint.y = rFilters[i].y;
		AddWavies(pts[nPts-1], nextPoint, pts, nPts, VWAVEHEIGHT, VWAVEINTERVAL);
		pts[nPts++] = nextPoint;
	}

	AddWavies(pts[nPts-1], pts[0], pts, nPts, HWAVEHEIGHT, HWAVEINTERVAL);

	ASSERT(nPts <= MAXPTS);
	CBeta *spline = new CBeta(pts, nPts, TRUE);
	return spline;
}


CSpline* CBWoodringNormal::GetBalloonSpline()
{
	CSpline *spline = m_spline->Clone();
	AddArrow(this, spline, *m_fInfo);
	return spline;
}


void CBWoodringNormal::SetBalloonTraj()
{
	if (m_traj)
		delete m_traj;

	m_traj = new CTraj;

	ASSERT(m_spline);
	ASSERT(m_traj);
	ASSERT(m_fInfo);

	CSpline *newSpline = m_spline->Clone();
	m_traj->AddSeg(newSpline);
	AddArrow(this, newSpline, *m_fInfo);
	m_traj->m_closed = TRUE;
}


BOOL CBWoodringNormal::ComputeInternals()
{
	if (!m_fInfo)
		m_fInfo = new CFormatInfo;
	if (!BreakIntoLines(*m_fInfo))
		return FALSE;
	ShiftLines(*m_fInfo);
	if (m_spline)
		delete m_spline;
	m_spline = CreateBalloonSpline(*m_fInfo);
	ComputeCloudBBox();
	return TRUE;
}


void CBWoodringNormal::Draw(CDC* pdc, POINT *ul, RECT *dmgArea)
{
	CPen*		pOldPen = pdc->SelectObject(&m_pen);
	// use comic font by default
	CFont*		pOldFont = pdc->SelectObject(m_fontI->m_font);
	COLORREF	crOldColor = pdc->SetTextColor(m_fontI->m_crDefaultForeColor);

	DrawPoint(pdc, tpoint);

	pdc->OffsetWindowOrg(-m_bbox.Left, -m_bbox.Top);
	if (!m_traj)
		SetBalloonTraj();
	m_traj->Draw(pdc);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));
	CBrush *pOldBrush = pdc->SelectObject(&brush);
	pdc->StrokeAndFillPath();

	pdc->SetBkMode(TRANSPARENT);
	DrawText(pdc);

	pdc->OffsetWindowOrg(m_bbox.Left, m_bbox.Top);
	pdc->SelectObject(pOldBrush);
	pdc->SelectObject(pOldPen);
	pdc->SelectObject(pOldFont);
	pdc->SetTextColor(crOldColor);
}


// CBWoodringWhisper members
CBWoodringWhisper::CBWoodringWhisper(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart) : CBWoodringNormal(szText, prgdwFormatting, szURLStart)
{
	m_fontI = CUnitPanelPage::m_fiWWhisper; /* reset to whisper font */ 
}


void CBWoodringWhisper::Draw(CDC* pdc, POINT *ul, RECT *dmgArea)
{
	CPen		*pOldPen = pdc->SelectObject(&m_nimbusPen);
	// use comic font by default
	CFont		*pOldFont = pdc->SelectObject(m_fontI->m_font);
	COLORREF	crOldColor = pdc->SetTextColor(m_fontI->m_crDefaultForeColor);

	pdc->OffsetWindowOrg(-m_bbox.Left, -m_bbox.Top);
	if (!m_traj) SetBalloonTraj();
	m_traj->Draw(pdc);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));
	CBrush *pOldBrush = pdc->SelectObject(&brush);
	pdc->StrokeAndFillPath();

	pdc->SelectObject(&m_pen);
	m_traj->Dash(pdc);

	pdc->SetBkMode(TRANSPARENT);
	DrawText(pdc);

	pdc->OffsetWindowOrg(m_bbox.Left, m_bbox.Top);
	pdc->SelectObject(pOldBrush);
	pdc->SelectObject(pOldPen);
	pdc->SelectObject(pOldFont);
	pdc->SetTextColor(crOldColor);
}


// CBWoodringThink members
CBWoodringThink::CBWoodringThink(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart) : CBWoodringNormal (szText, prgdwFormatting, szURLStart)
{
}

void CBWoodringThink::Draw(CDC* pdc, POINT *ul, RECT *dmgArea)
{
	CBWoodringNormal::Draw(pdc, ul, dmgArea);   // will draw the cloud properly

	// for now, choose middle of routeRgn as bubble entry point
	POINT bubbleEntry, bubbleTail;
	bubbleEntry.x = (m_routeRgn.Left + m_routeRgn.Right) / 2;
	bubbleEntry.y = m_fInfo->m_bbox.Bottom + m_bbox.Top;   // adding m_bbox.Top puts it in panel coords
	bubbleTail.x = m_speaker->m_arrowX;
	bubbleTail.y = m_speaker->m_bbox.Top + 200;  // for now

	int deltaY = bubbleEntry.y - bubbleTail.y;
	if (deltaY < 0) return;		// entry below tail?  eventually should assert an error
	int nBubbles = (deltaY + INTERBUBBLE) / (BUBBLEHEIGHT + INTERBUBBLE);
	if (nBubbles < 0) return;

	CPen	*pOldPen = pdc->SelectObject(&m_pen);
	CBrush	brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));
	CBrush	*pOldBrush = pdc->SelectObject(&brush);

	// bubbles should be spaced vertically across allowed height
	int bubbleSpacing = (nBubbles > 1) ? (deltaY - BUBBLEHEIGHT*nBubbles) / (nBubbles - 1) : 0;

	DPOINT deltaVec = point_to_dpoint(point_sub(bubbleEntry, bubbleTail));
	DPOINT deltaVecNorm = point_norm(deltaVec);
	POINT start = point_add(bubbleTail, dpoint_to_point(point_scalmult(BUBBLEHEIGHT/2.0, deltaVecNorm)));
	POINT increment = dpoint_to_point(point_scalmult((double)BUBBLEHEIGHT + bubbleSpacing, deltaVecNorm));
	RECT circRect;
	int widthDelta = (nBubbles > 1) ? (ENDBUBBLEWIDTH - BUBBLEHEIGHT) / (2*(nBubbles - 1)) : 0;
	int widthAdjustment = 0;
	for (int i = 0; i < nBubbles; i++) {
		bbox_around_pt(&circRect, &start, BUBBLEHEIGHT / 2);
		circRect.left -= widthAdjustment;
		circRect.right += widthAdjustment;
		pdc->Ellipse(&circRect);
		start = point_add(start, increment);
		widthAdjustment += widthDelta;
	}

	pdc->SelectObject(pOldPen);
	pdc->SelectObject(pOldBrush);
}


// CBWoodringBox members
CBWoodringBox::CBWoodringBox(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart) : CBWoodringNormal(szText, prgdwFormatting, szURLStart)
{
	m_format |= FT_LEFT_JUSTIFY;
}


void CBWoodringBox::SetBalloonTraj()
{
	if (m_traj) delete m_traj;
	m_traj = new CTraj;
	SRECT *fbbox = &m_fInfo->m_bbox;
	CPoint pt1(fbbox->Left - XBOXDELTA, fbbox->Bottom - YBOXDELTA);
	CPoint pt2(pt1.x, fbbox->Top + YBOXDELTA);
	CPoint pt3(fbbox->Right + XBOXDELTA, pt2.y);
	CPoint pt4(pt3.x, pt1.y);

	m_traj->AddSeg(new CLine(pt1, pt2));
	m_traj->AddSeg(new CLine(pt2, pt3));
	m_traj->AddSeg(new CLine(pt3, pt4));
	m_traj->AddSeg(new CLine(pt4, pt1));
	m_traj->m_closed = TRUE;
}


void CBWoodringBox::GetBBox(RECT *r)
{
	GetCloudBBox(r);
} 


void CBWoodringBox::ComputeCloudBBox()
{
	m_trueBox.Left = m_fInfo->m_bbox.Left - XBOXDELTA;
	m_trueBox.Right = m_fInfo->m_bbox.Right + XBOXDELTA;
	m_trueBox.Bottom = m_fInfo->m_bbox.Bottom - YBOXDELTA;
	m_trueBox.Top = m_fInfo->m_bbox.Top + YBOXDELTA;
}


void CBWoodringBox::QueryRouteRgn(int OtherToX, int &leftAllowance, int &rightAllowance)
{
	rightAllowance = LARGEINTEGER;
	leftAllowance = -LARGEINTEGER;
}


#if 0
POINT PermutePt(POINT &pt, POINT &thisVec, int cpSign) {
	int xSign = 0, ySign = 0;
	POINT rval;

	if (cpSign > 0) {
		xSign = thisVec.x | thisVec.y;
		ySign = (thisVec.x < 0 || thisVec.y > 0) ? 1 : -1;
	} else if (cpSign < 0) {
		ySign = -(thisVec.x | thisVec.y);
		xSign = (thisVec.x < 0 || thisVec.y > 0) ? 1 : -1;
	}
	
	rval.x = pt.x + xSign * ((long)(XPERMUTE * randfloat()) + XBORDER);
	rval.y = pt.y + ySign * ((long)(YPERMUTE * randfloat()) + YBORDER);
	return rval;
}
#endif
