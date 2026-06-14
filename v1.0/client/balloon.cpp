#include "stdafx.h"
#include "chat.h"

#include "common.h"
#include "chatprot.h"
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
#include "panel.h"
#include "url.h"
#include <ctype.h>
#include <stdlib.h>
#include <math.h>
#include <tchar.h>

void DrawPoints(CDC *dc, POINT *p, int nPts) {
	for (int i = 0; i < nPts; i++)
		DrawPoint(dc, p[i]);
}


CPen CBWoodringNormal :: m_pen (PS_SOLID, 28, RGB(0,0,0));
CPen CBWoodringWhisper :: m_nimbusPen (PS_SOLID, 100, RGB(255, 255, 255));

void Capitalize(char *str) {
	CharUpperBuff(str, _tcslen(str));   // internationalized
}


// return the first non-punctuation character after the stream of white space
// (or terminating null char)
char *GetNextStart(char *str) {
	while (isspace(*str)) str++;
	return str;
}

// return the first whitespace char starting at str
// (or the terminating null) after any initial run of whitespace
char *GetNextEnd(char *str) {
	while (*str && isspace(*str)) str++;
	while (*str && !isspace(*str)) str++;
	return str;
}

void ForceLineBreak(CDC *dc, char *str, int maxWidth, int &length, int &width) {
	TRACE("FORCELINEBREAK: MAXWIDTH = %d.\n", maxWidth);
	width = length = 0;
	while (TRUE) {
		length++;
		CSize dwExtent = dc->GetTextExtent(str, length);
		if (str[length] && (dwExtent.cx <= maxWidth)) {
			width = dwExtent.cx;
		} else {
			ASSERT(length != 0);  // one character won't fit!!!!
			length--;			      // last one was OK
			return;
		}
	}
}

char *FindFurthestLineBreak(CDC *dc, int maxWidth, char *str, int &width) {
	char *lastEnd, *lineEnd = str;
	while (TRUE) {
		lastEnd = lineEnd;
		lineEnd = GetNextEnd(lineEnd);
		int thisLength = lineEnd - str;
		CSize dwExtent = dc->GetTextExtent(str, thisLength);
		if (dwExtent.cx <= maxWidth) {
			width = dwExtent.cx;
			if (!(*lineEnd))   // ran over end
				return lineEnd;
			continue;				// it fits -- try to fit more
		} else {					// doesn't fit
			if (lastEnd == str) {    // A line couldn't fit bbox constraint -- break it! (no hyphen)
				ForceLineBreak(dc, str, maxWidth, thisLength, width);
				lastEnd = str + thisLength;
			}
			return lastEnd;
		}
	}
}


#define MAXLINES	30

int BreakIntoLines(CDC *dc, int maxWidth, char *str, char *starts[], int lengths[], int widths[])
{
	int nLines = 0, thisLength = 0, lastLength = 0, lastWidth;
	char *lineEnd = str;
	while (TRUE) {
		lineEnd = GetNextEnd(lineEnd);
		lastLength = thisLength;
		thisLength = lineEnd - str;
		CSize dwExtent = dc->GetTextExtent(str, thisLength);
		if (dwExtent.cx <= maxWidth) {
			if (!(*lineEnd)) {  // ran over end
				starts[nLines] = str;
				lengths[nLines] = thisLength;
				widths[nLines++] = dwExtent.cx;
				return nLines;
			}
			lastWidth = dwExtent.cx;
			continue;				// it fits -- try to fit more
		} else {					// doesn't fit
			if (lastLength == 0)    // A line couldn't fit bbox constraint -- break it! (no hyphen)
				ForceLineBreak(dc, str, maxWidth, lastLength, lastWidth);
			starts[nLines] = str;
			lengths[nLines] = lastLength;
			widths[nLines++] = lastWidth;
			str = lineEnd = GetNextStart(str + lastLength);
			if (!(*str)) return nLines;
			if (nLines > MAXLINES) return 0;   // fail
		}
		thisLength = 0;
	}
}

extern CChatApp theApp;

#define MAXLEFTSHIFT	0
#define MAXCENTERSHIFT	0 // 100

double randfloat() {
	return (((double) rand()) / RAND_MAX);
}

POINT UnitVector(POINT *pts, int nPts, int i1, int i2) {
	POINT rval;
	i1 = (i1 + nPts) % nPts;
	i2 = (i2 + nPts) % nPts;
	rval.x = pts[i2].x - pts[i1].x;
	rval.y = pts[i2].y - pts[i1].y;
	if (rval.x > 0) rval.x = 1;
	else if (rval.x < 0) rval.x = -1;
	if (rval.y > 0) rval.y = 1;
	else if (rval.y < 0) rval.y = -1;
	return rval;
}

inline int CrossProd(POINT &pt1, POINT &pt2) {
	return (pt1.x * pt2.y - pt1.y * pt2.x);
}

#define XPERMUTE	50
#define YPERMUTE	10
#define XBORDER		100
#define YBORDER		40
#define TOPBORDER	-20

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

CBalloon::CBalloon(const char *str, CFontInfo *fontInfo, CPtrArray *links) : CLabel(str, fontInfo, links) {
//	m_speaker = (CBody *) 10;
	m_fInfo = NULL;
	m_spline = NULL;
	m_traj = NULL;
	m_trueBox.Left = m_trueBox.Right = m_trueBox.Top = m_trueBox.Bottom = -1;  // for debugging -- indicates not set
}

CBalloon::~CBalloon() {
	if (m_fInfo) delete m_fInfo;
	if (m_spline) delete m_spline;
	if (m_traj) delete m_traj;
}


CBWoodringNormal::CBWoodringNormal(const char *str, CPtrArray *links) : CBalloon(str, CUnitPanelPage::m_fiWNormal, links) { Capitalize(m_str); }
CBWoodringWhisper::CBWoodringWhisper(const char *str, CPtrArray *links) : CBWoodringNormal(str, links) { m_fontI = CUnitPanelPage::m_fiWWhisper; /* reset to whisper font */ }
CBWoodringThink::CBWoodringThink(const char *str, CPtrArray *links) : CBWoodringNormal (str, links) {}
CBWoodringMini::CBWoodringMini(const char *str, CPtrArray *links) : CBWoodringNormal(str, links) { m_fontI = CUnitPanelPage::m_fiMini; }
CBWoodringBox::CBWoodringBox(const char *str, CPtrArray *links) : CBWoodringNormal(str, links) { m_format |= FT_LEFT_JUSTIFY; }

// x and y are in balloon space
void BreakSpline(CBalloon *balloon, CSpline *spline, int x, int y, double oFactor) {
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

#define LARGEDELTA	350
#define SMALLDELTA	150
#define MINTAILHEIGHT	100
POINT tpoint;

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
	int bottomStart = m_fInfo->leftX[m_fInfo->nLines-1];
	int bottomEnd = bottomStart + m_fInfo->widths[m_fInfo->nLines-1];
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


CArrow::CArrow(const CArrow &a) {
	m_mid = a.m_mid;
	m_hi = a.m_hi;
	m_lo = a.m_lo;
}

typedef struct {
	int start;
	int end;
	int x;
	int y;
} RANGE;

#define THRESH1	-70
#define THRESH2 70

void GetFilters(CFormatInfo& fInfo, RANGE l[], RANGE r[], int& nL, int& nR) {
	nL = 0;
	nR = 0;
	l[nL].x = fInfo.leftX[0];
	r[nR].x = fInfo.leftX[0] + fInfo.widths[0];
	l[nL].start = r[nR].start = 0;

	for (int i = 1; i < fInfo.nLines; i++) {
		int thisLeft = fInfo.leftX[i];
		int thisRight = fInfo.leftX[i] + fInfo.widths[i];
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
			int nextLeft = (i+1 < fInfo.nLines) ? fInfo.leftX[i+1] : thisLeft;
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
			int nextRight = (i+1 < fInfo.nLines) ? fInfo.leftX[i+1] + fInfo.widths[i+1] : thisRight;
			if (nextRight - r[nR].x <= -THRESH2) { // Following line also indents dram. to left
				r[nR].end = i-1;
				r[++nR].start = i;
				r[nR].x = max(thisRight, nextRight);
			}
		}
	} // end for

	l[nL++].end = r[nR++].end = fInfo.nLines-1;
}

int PermuteFilters(CFontInfo& fontI, RANGE lFilters[], RANGE rFilters[], int nLFilters, int nRFilters) {
	int baseY = 0;
	int lastX = LARGEINTEGER;
	for (int i = 0; i < nLFilters; i++) {
		lFilters[i].x -= XBORDER;
		if (i == 0)
			lFilters[i].y = baseY + TOPBORDER + YBORDER;
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
			rFilters[i].y = baseY + TOPBORDER + YBORDER;
		if (rFilters[i].x > lastX)
			rFilters[i].y = baseY + YBORDER;
		else rFilters[i].y = baseY - YBORDER - fontI.m_baseAdd;
		baseY -= (rFilters[i].end - rFilters[i].start + 1) * fontI.m_lineHeight;
		lastX = rFilters[i].x;
	}
	return baseY - TOPBORDER - YBORDER - fontI.m_baseAdd;
}

void AddWavies(POINT& pt1, POINT& pt2, POINT *pts, int& nPts, int waveDiam, int interval) {
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

#define VWAVEHEIGHT		70
#define VWAVEINTERVAL	300
#define HWAVEHEIGHT		70
#define HWAVEINTERVAL	300

CSpline *CBWoodringNormal::CreateBalloonSpline(CFormatInfo& fInfo) {
	RANGE lFilters[20], rFilters[20];
	POINT pts[100], nextPoint, thisPoint;
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

	CBeta *spline = new CBeta(pts, nPts, TRUE);
	return spline;
}

CSpline *CBWoodringNormal::GetBalloonSpline() {
	CSpline *spline = m_spline->Clone();
	AddArrow(this, spline, *m_fInfo);
	return spline;
}

void CBWoodringNormal::SetBalloonTraj() {
	if (m_traj) delete m_traj;
	m_traj = new CTraj;
	CSpline *newSpline = m_spline->Clone();
	m_traj->AddSeg(newSpline);
	AddArrow(this, newSpline, *m_fInfo);
	m_traj->m_closed = TRUE;
}


void CBWoodringNormal::Draw(CDC* dc, POINT *ul, RECT *dmgArea) {
	CPen *oldPen = dc->SelectObject(&m_pen);
	// use comic font by default
	CFont *oldFont = dc->SelectObject(m_fontI->m_font);

	DrawPoint(dc, tpoint);

	dc->OffsetWindowOrg(-m_bbox.Left, -m_bbox.Top);
	if (!m_traj) SetBalloonTraj();
	m_traj->Draw(dc);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));
//	brush.CreateSolidBrush(RGB(	(int)(((double)rand() / RAND_MAX) * 255), (int)(((double)rand() / RAND_MAX) * 255), (int)(((double)rand() / RAND_MAX) * 255)));
	dc->SelectObject(brush);
	dc->StrokeAndFillPath();
//	DrawPoints(dc, outSpline->cps, outSpline->nCps);

	dc->SetBkMode(TRANSPARENT);
	DrawText(dc);

	dc->OffsetWindowOrg(m_bbox.Left, m_bbox.Top);
	dc->SelectObject(oldPen);
	dc->SelectObject(oldFont);
}


void CBWoodringWhisper::Draw(CDC* dc, POINT *ul, RECT *dmgArea) {
	CPen *oldPen = dc->SelectObject(&m_nimbusPen);
	// use comic font by default
	CFont *oldFont = dc->SelectObject(m_fontI->m_font);

	dc->OffsetWindowOrg(-m_bbox.Left, -m_bbox.Top);
	if (!m_traj) SetBalloonTraj();
	m_traj->Draw(dc);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));

	dc->SelectObject(brush);
	dc->StrokeAndFillPath();

	dc->SelectObject(&m_pen);
	m_traj->Dash(dc);

	dc->SetBkMode(TRANSPARENT);
	DrawText(dc);

	dc->OffsetWindowOrg(m_bbox.Left, m_bbox.Top);
	dc->SelectObject(oldPen);
	dc->SelectObject(oldFont);
}

void DrawRun(CDC *dc, CLabel *, int leftX, int baseY, int, int, char *start, int runLength, void *link) {
	COLORREF oldColor;
	if (link) oldColor = dc->SetTextColor(linkColor);
	dc->TextOut(leftX, baseY, start, runLength);
	if (link) dc->SetTextColor(oldColor);
}

void CBalloon::DrawText(CDC *dc) {
	if (m_links) {					// this is rare, so call a separate function when this occurs
		ForTextRuns(dc, DrawRun);
		return;
	}

	int baseY = 0;
	for (int i = 0; i < m_fInfo->nLines; i++) {
		dc->TextOut(m_fInfo->leftX[i],
					baseY,
					m_fInfo->starts[i],
					m_fInfo->lengths[i]);
		baseY -= m_fontI->m_lineHeight;
	}
}

static POINT mouseDownPt;

void FollowURL(CDC *, CLabel *label, int leftX, int baseY, int width, int height, char *, int, void *link) {
	if (link) {
		RECT r;
		int left = label->m_bbox.Left + leftX;
		int top = label->m_bbox.Top + baseY;
		SetRect(&r, left, top, left + width, top - height);
		if (inside_bbox(&mouseDownPt, &r)) {
			FLaunchBrowser(((CLink *)link)->m_url);
		}
	}
}

void CBalloon::OnLButtonDown(POINT &pt) {
	if (m_links) {
		mouseDownPt = pt;
		ForTextRuns(GetClientDC(), FollowURL);
	}
}


void CBalloon::ForTextRuns(CDC *dc, void (*runFunc)(CDC*, CLabel *, int, int, int, int, char *, int, void *)) {
	int baseY = 0;
	int upper = m_links->GetUpperBound();
	ASSERT(upper >= 0);
	int linkIndex = 0;
	CLink *link = (CLink *)(*m_links)[0];
	BOOL inLink = FALSE;
	int runLength, width;
	int dy = m_fontI->m_lineHeight;

	for (int i = 0; i < m_fInfo->nLines; i++) {
		char *start = m_fInfo->starts[i];
		int leftX = m_fInfo->leftX[i];
		char *end = start + m_fInfo->lengths[i];
		int startIndex = start - m_str;
		int endIndex = end - m_str;
		while (TRUE) {
			if (!inLink) {
				if (!link || link->m_start >= endIndex) {
					runLength = endIndex - startIndex;
					if (runLength) {
						width = dc->GetTextExtent(start, runLength).cx;
						runFunc(dc, this, leftX, baseY, width, dy, start, runLength, NULL);
					}
					break;
				} else {
					runLength = link->m_start - startIndex;
					if (runLength) {
						width = dc->GetTextExtent(start, runLength).cx;
						runFunc(dc, this, leftX, baseY, width, dy, start, runLength, NULL);
					}
					inLink = TRUE;
				}
			} else {  // inLink
				if (link->m_start + link->m_length > endIndex) {
					runLength = m_fInfo->lengths[i];
					if (runLength) {
						width = dc->GetTextExtent(start, runLength).cx;
						runFunc(dc, this, leftX, baseY, width, dy, start, runLength, link);
					}
					break;
				} else {
					runLength = (link->m_start + link->m_length) - startIndex;
					width = dc->GetTextExtent(start, runLength).cx;
					if (runLength) {
						width = dc->GetTextExtent(start, runLength).cx;
						runFunc(dc, this, leftX, baseY, width, dy, start, runLength, link);
					}
					inLink = FALSE;
					link = (++linkIndex > upper) ? NULL : (CLink *) (*m_links)[linkIndex];
				}
			}
			
			if (start >= end) break;
			if (runLength) leftX += width;
			startIndex += runLength;
			start = m_str + startIndex;
		}
		baseY -= dy;
	}
}



#define BUBBLEHEIGHT	150
#define INTERBUBBLE		100
#define ENDBUBBLEWIDTH	400

void CBWoodringThink::Draw(CDC* dc, POINT *ul, RECT *dmgArea) {
	CBWoodringNormal::Draw(dc, ul, dmgArea);   // will draw the cloud properly

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

	CPen *oldPen = dc->SelectObject(&m_pen);
	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));
	CBrush *oldBrush = dc->SelectObject(&brush);

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
		dc->Ellipse(&circRect);
		start = point_add(start, increment);
		widthAdjustment += widthDelta;
	}

	dc->SelectObject(oldPen);
	dc->SelectObject(oldBrush);
}

void CLabel::Draw(CDC *dc, POINT *ul, RECT *dmgArea) {
	int lengths[30], widths[30], leftText;
	char *starts[30];

	CFont *oldFont = dc->SelectObject(m_fontI->m_font);
	CSize dwExtent = dc->GetTextExtent(m_str, strlen(m_str));
	int desiredWidth = m_bbox.Right - m_bbox.Left;
	int nLines = ::BreakIntoLines(dc, desiredWidth, m_str, starts, lengths, widths);

	dc->SetBkMode(TRANSPARENT);

	int startTop = m_bbox.Top;
	for (int i = 0; i < nLines; i++) {
		if (m_format & FT_LEFT_JUSTIFY)
			leftText = m_bbox.Left;
		else
			leftText = m_bbox.Left + ((CUnitPanelPage::unitWidth - widths[i]) / 2); // Kludge - fix
		dc->TextOut(leftText,
					startTop,
					starts[i],
					lengths[i]);
		startTop -= m_fontI->m_lineHeight;
	}

	dc->SelectObject(oldFont);
}

void CLabel::GetBBox(RECT *r) {
	CFormatInfo fInfo;
	BreakIntoLines(fInfo);

	r->left = fInfo.m_bbox.Left;
	r->top = fInfo.m_bbox.Top;
	r->right = fInfo.m_bbox.Right;
	r->bottom = fInfo.m_bbox.Bottom;
}

void CBalloon::GetBBox(RECT *r) {
	// this isn't exactly right -- fix
	GetCloudBBox(r);
	r->bottom = min(r->bottom, m_speaker->m_bbox.Top + 200);  // include tail pt. (hack)
}

void CPanelElement::GetBBox(RECT *r) {
	r->left = m_bbox.Left;
	r->top = m_bbox.Top;
	r->right = m_bbox.Right;
	r->bottom = m_bbox.Bottom;
}

int CLabel::BreakIntoLines(CFormatInfo &fInfo) {
	CDC *dc = GetClientDC();
	CFont *oldFont = dc->SelectObject(m_fontI->m_font);
	CSize dwExtent = dc->GetTextExtent(m_str, strlen(m_str));
	int desiredWidth = m_bbox.Right - m_bbox.Left;
	fInfo.nLines = ::BreakIntoLines(dc, desiredWidth, m_str, fInfo.starts,
									fInfo.lengths, fInfo.widths);

	fInfo.maxWidth = 0;
	for (int i = 0; i < fInfo.nLines; i++)			// find widest line
		if (fInfo.maxWidth < fInfo.widths[i]) fInfo.maxWidth = fInfo.widths[i];

	fInfo.m_bbox.Top = m_bbox.Top;
	if (m_format & FT_LEFT_JUSTIFY) {
		fInfo.m_bbox.Left = m_bbox.Left;
		fInfo.m_bbox.Right = m_bbox.Left + fInfo.maxWidth;
	} else {	// CENTER
		fInfo.m_bbox.Left = (desiredWidth - fInfo.maxWidth)/2 + m_bbox.Left;
		fInfo.m_bbox.Right = fInfo.m_bbox.Left + fInfo.maxWidth;
	}

	fInfo.m_bbox.Bottom = (short)(fInfo.m_bbox.Top - fInfo.nLines * m_fontI->m_lineHeight - m_fontI->m_baseAdd);

	dc->SelectObject(oldFont);
	return fInfo.nLines;
}

int CLabel::AreaEstimate(int *len, int *lineHeight) {
	CDC *dc = GetClientDC();
	CFont *oldFont = dc->SelectObject(m_fontI->m_font);
	CSize dwExtent = dc->GetTextExtent(m_str, strlen(m_str));
	dc->SelectObject(oldFont);
	*len = dwExtent.cx;
	*lineHeight = m_fontI->m_lineHeight;
	return ((int) (1.3 * dwExtent.cx * (dwExtent.cy + *lineHeight)));
}

int CLabel::WidestWord() {
	CDC *dc = GetClientDC();
	int maxWidth = 0;
	char *endPtr, *startPtr = m_str;

	while (TRUE) {
		while (*startPtr && !isprint(*startPtr))			// startPtr points to next printable character
			startPtr++;
		if (!*startPtr) break;
		endPtr = startPtr;
		while(isprint(*endPtr)) endPtr++;
		CSize dwExtent = dc->GetTextExtent(startPtr, endPtr - startPtr + 1);
		maxWidth = max(maxWidth, dwExtent.cx);
		// now set up startPtr for next iteraction
		if (!*endPtr) break;
		startPtr = endPtr+1;
	}

	return maxWidth;
}


void CLabel::ShiftLines(CFormatInfo &fInfo) {
	int i, shiftLimit, shift;

	if (m_format & FT_LEFT_JUSTIFY) {
		for (i = 0; i < fInfo.nLines; i++) {
			shiftLimit = fInfo.maxWidth - fInfo.widths[i];
			fInfo.leftX[i] = (int)(randfloat() * min(MAXLEFTSHIFT, shiftLimit));
		}
	} else {
		for (i = 0; i < fInfo.nLines; i++) {
			shiftLimit = (fInfo.maxWidth - fInfo.widths[i]) / 2;
			shift = (int) ((randfloat() * 2.0 - 1.0) * min(MAXCENTERSHIFT, shiftLimit));
			fInfo.leftX[i] = (((fInfo.m_bbox.Right - fInfo.m_bbox.Left)
								- fInfo.widths[i]) / 2) + shift;
		}
	}
	fInfo.m_shifted = TRUE;
}


void CBalloon::DockAtTop(int height) {
	int oldBBoxHeight = m_bbox.Top - m_bbox.Bottom;

	m_bbox.Top = height + TOPBORDER;
	m_bbox.Bottom = m_bbox.Top - oldBBoxHeight;
}

void Dock(RECT &rect) {
	int delta = TOPBORDER + YBORDER + HWAVEHEIGHT;
	rect.top += delta;
	rect.bottom += delta;
}

void Dock(SRECT &rect) {
	int delta = TOPBORDER + YBORDER + HWAVEHEIGHT;
	rect.Top += delta;
	rect.Bottom += delta;
}

BOOL CBalloon::Overlap(CBalloon *b2) {
	RECT cb1, cb2;
	GetCloudBBox(&cb1);
	b2->GetCloudBBox(&cb2);
	return (bbox_overlap(&cb1, &cb2));
}


void CBalloon::ComputeCloudBBox() { // called only to compute it from scratch
	make_empty(&m_trueBox);
	for (int i = 0; i < m_spline->nCps; i++)
		include_pt_in_bbox(&m_spline->cps[i], &m_trueBox);
}

// provides true cloud bbox (not necessarily what was explicitly "Set")
void CBalloon::GetCloudBBox(RECT *r) {
	r->left = m_trueBox.Left + m_bbox.Left;
	r->top = m_trueBox.Top + m_bbox.Top;
	r->right = m_trueBox.Right + m_bbox.Left;
	r->bottom = m_trueBox.Bottom + m_bbox.Top;
}

void CBalloon::GetCloudBBox(SRECT *r) {
	r->Left = m_trueBox.Left + m_bbox.Left;
	r->Top = m_trueBox.Top + m_bbox.Top;
	r->Right = m_trueBox.Right + m_bbox.Left;
	r->Bottom = m_trueBox.Bottom + m_bbox.Top;
}

int nspline = -1;

BOOL CBWoodringNormal::ComputeInternals() {
	if (!m_fInfo) m_fInfo = new CFormatInfo;
	nspline++;
	if (!BreakIntoLines(*m_fInfo)) return FALSE;
	ShiftLines(*m_fInfo);
	if (m_spline) delete m_spline;
	m_spline = CreateBalloonSpline(*m_fInfo);
	ComputeCloudBBox();
	return TRUE;
}

BOOL CBalloon::SetBBox(int left, int bottom, int right, int top) {
	if (m_bbox.Right - m_bbox.Left != right - left ||
		m_bbox.Top - m_bbox.Bottom != top - bottom) // just change origin
	{
		m_bbox.Left = 0;
		m_bbox.Right = (right - left) - 2*XBORDER;	// estimate
		m_bbox.Top = 0;

		// note: this ignores m_bbox, except to calculate width
		// it will fail if we can't set the bbox width this small for this balloon
		if (!ComputeInternals()) return FALSE;

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

void CBalloon::InMyCoords(SRECT *bbox) {
	bbox->Left -= m_bbox.Left;
	bbox->Right -= m_bbox.Left;
	bbox->Top -= m_bbox.Top;
	bbox->Bottom -= m_bbox.Top;
}


CPanelElement::CPanelElement(const CPanelElement& p) {
	m_bbox = p.m_bbox;
}

CLabel::CLabel(const CLabel& c)
: CPanelElement(c) {
	CPtrArray *CopyLinks(CPtrArray *);
	if (c.m_str) m_str = strdup(c.m_str);
	m_fontI = c.m_fontI;
	m_format = c.m_format;
	m_links = CopyLinks(c.m_links);
}


CBalloon::CBalloon(const CBalloon& b)
: CLabel(b) {
	if (b.m_fInfo) {
		m_fInfo = new CFormatInfo;
		*m_fInfo = *b.m_fInfo;
	}
	m_speaker = NULL;  // b.m_speaker ? b.m_speaker->Clone() : NULL; //  // don't clone the speaker
	m_spline = b.m_spline ? b.m_spline->Clone() : NULL;
	m_traj = NULL;  // for now, don't clone traj
	m_trueBox = b.m_trueBox;
}

#define MINROUTEWIDTH 300

void CBalloon::QueryRouteRgn(int OtherToX, int &leftAllowance, int &rightAllowance) {
	int toX = m_speaker->m_arrowX;

	if (OtherToX > toX) {		// other balloon's toX is to right of this balloon's
		leftAllowance = max(toX, m_routeRgn.Left + MINROUTEWIDTH);
		rightAllowance = LARGEINTEGER;
	} else {					// other balloon's toX is to left of this balloon's
		leftAllowance = -LARGEINTEGER;
		rightAllowance = min(toX, m_routeRgn.Right - MINROUTEWIDTH);
	}
}

void CBalloon::SetRouteRgn(int OtherToX, int left, int right) {
	SRECT *speakerBox = &(m_speaker->m_bbox);
	int toX = m_speaker->m_arrowX;

	if (OtherToX > toX)			// other balloon's toX is to right of this balloon's
		m_routeRgn.Right = min(m_routeRgn.Right, left);
	else
		m_routeRgn.Left = max(m_routeRgn.Left, right);
}

CFontInfo::CFontInfo(CFont *font, short leading, short baseAdd) {
	TEXTMETRIC tm;

	m_font = font;
	m_leading = leading;
	m_baseAdd = baseAdd;

	CDC *dc = GetClientDC();
	CFont *oldFont = dc->SelectObject(font);
	VERIFY( dc->GetTextMetrics(&tm) );
	m_lineHeight = (short)(tm.tmHeight + leading);
	dc->SelectObject(oldFont);
}


#define BORDERFUDGE	400		// Yum fudge!
static int continuationStr1Len = 150;  // for now
static const char *continuationStr1 = "...";
static const char *continuationStr2 = "...";

char *CBWoodringNormal::SplitHeight(int height) {
	int width;  // dummy param
	int maxLines = (height - BORDERFUDGE) / m_fontI->m_lineHeight;
	if (maxLines >= m_fInfo->nLines) return NULL;

	// OK, we really have to do the split...
	m_fInfo->nLines = maxLines;
	char *end = FindFurthestLineBreak(GetClientDC(), m_fInfo->m_bbox.Right - m_fInfo->m_bbox.Left - continuationStr1Len,
								      m_fInfo->starts[maxLines - 1], width);
	int nToCopy = end - m_str;
	char *newStr = (char *) malloc (nToCopy + strlen(continuationStr1) + 1);
	strncpy(newStr, m_str, nToCopy);
	strcpy(newStr+nToCopy, continuationStr1);
	char *restStart = m_str + nToCopy;
	char *rest = (char *) malloc (strlen(restStart) + strlen(continuationStr2) + 1);
	strcpy(rest, continuationStr2);
	strcat(rest, restStart);
	free(m_str);
	m_str = newStr;

	// recompute m_fInfo...  Keep in mind that m_bbox has been offset by trueBox
	//    since original call to SetBBox, so we must compensate.  Also, value isn't
	//    really recomputed unless bbox dimensions change, so hack this.
	//    (This is a major hack -- should be made more elegant.)
	m_bbox.Left--;
	SetBBox(m_bbox.Left + m_trueBox.Left + 1, m_bbox.Bottom + m_trueBox.Top,
			m_bbox.Right + m_trueBox.Left, m_bbox.Top + m_trueBox.Top);
	return rest;
}

char *CLabel::SplitHeight(int height) {
	int lengths[30], widths[30], width;
	char *starts[30];
	CClientDC *dc = GetClientDC();

	CFont *oldFont = dc->SelectObject(m_fontI->m_font);
	CSize dwExtent = dc->GetTextExtent(m_str, strlen(m_str));

	int desiredWidth = m_bbox.Right - m_bbox.Left;
	int nLines = ::BreakIntoLines(dc, desiredWidth, m_str, starts, lengths, widths);

	int maxLines = height / m_fontI->m_lineHeight;
	if (nLines < maxLines) return NULL;

	char *end = FindFurthestLineBreak(dc, m_bbox.Right - m_bbox.Left - continuationStr1Len,
								      starts[maxLines - 1], width);
	int nToCopy = end - m_str;
	char *newStr = (char *) malloc (nToCopy + strlen(continuationStr1) + 1);
	strncpy(newStr, m_str, nToCopy);
	strcpy(newStr+nToCopy, continuationStr1);
	char *restStart = m_str + nToCopy;
	char *rest = (char *) malloc (strlen(restStart) + strlen(continuationStr2) + 1);
	strcpy(rest, continuationStr2);
	strcat(rest, restStart);
	free(m_str);
	m_str = newStr;

	dc->SelectObject(oldFont);
	return rest;
}

#define XBOXDELTA 90
#define YBOXDELTA 50

void CBWoodringBox::SetBalloonTraj() {
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


void CBWoodringBox::GetBBox(RECT *r) {
	GetCloudBBox(r);
} 

void CBWoodringBox::ComputeCloudBBox() {
	m_trueBox.Left = m_fInfo->m_bbox.Left - XBOXDELTA;
	m_trueBox.Right = m_fInfo->m_bbox.Right + XBOXDELTA;
	m_trueBox.Bottom = m_fInfo->m_bbox.Bottom - YBOXDELTA;
	m_trueBox.Top = m_fInfo->m_bbox.Top + YBOXDELTA;
}

void CBWoodringBox::QueryRouteRgn(int OtherToX, int &leftAllowance, int &rightAllowance) {
	rightAllowance = LARGEINTEGER;
	leftAllowance = -LARGEINTEGER;
}
