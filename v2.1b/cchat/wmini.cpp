#include "stdafx.h"
#include "client.h"

#include "chat.h"
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
#include <ctype.h>
#include <stdlib.h>
#include <math.h>

void DrawPoints(CDC *dc, POINT *p, int nPts) {
	for (int i = 0; i < nPts; i++)
		DrawPoint(dc, p[i]);
}


CPen CBWoodringNormal :: m_pen (PS_SOLID, 28, RGB(0,0,0)); // pen is black by default
CPen CBWoodringWhisper :: m_nimbusPen (PS_SOLID, 100, RGB(255, 255, 255));

#if 0
void DrawBalloon(CDC *dc, RECT *loc) {
//	static int i = 0;
//	if (i++ == 0) return;
	HENHMETAFILE hmf = GetEnhMetaFile("C:\\djk\\vchat\\client\\balloon.emf");
	dc->PlayMetaFile(hmf, loc);
	DeleteEnhMetaFile(hmf);
}
#endif


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

int BreakIntoLines(CDC *dc, int iMaxWidth, char *szString, char *rgszStarts[], int rgiLengths[], int rgiWidths[])
{
	int		nLines = 0, iThisLength = 0, iLastLength = 0, iLastWidth;
	char*	szLineEnd = szString;

	while (TRUE)
	{
		szLineEnd = GetNextEnd(szLineEnd);
		szLastLength = iThisLength;
		iThisLength = szLineEnd - szString;
		CSize dwExtent = dc->GetTextExtent(szString, iThisLength);
		if (dwExtent.cx <= iMaxWidth)
		{
			if (!(*szLineEnd))
			{	// ran over end
				rgszStarts[nLines] = szString;
				rgiLengths[nLines] = iThisLength;
				rgiWidths[nLines++] = dwExtent.cx;
				return nLines;
			}
			iLastWidth = dwExtent.cx;
			continue;				// it fits -- try to fit more
		}
		else
		{							// doesn't fit
			if (iLastLength == 0)
				return 0;	// A line couldn't fit bbox constraint -- fail for now (eventually hyphenate)
			rgszStarts[nLines] = szString;
			rgiLengths[nLines] = iLastLength;
			rgiWidths[nLines++] = iLastWidth;
			szString = iLineEnd = GetNextStart(szString + iLastLength);
			if (!(*szString))
				return nLines;
		}
		iThisLength = 0;
	}
}

extern CClientApp theApp;

#define MAXLEFTSHIFT	150
#define MAXCENTERSHIFT	0 // 100

double randfloat() {
	return (((double) rand()) / RAND_MAX);
#if 0
	double r = (((double) rand()) / RAND_MAX);
	TRACE("RAND = %f.\n", r);
	return r;
#endif
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

CBalloon::CBalloon(const char *str, CFontInfo *fontInfo) : CLabel(str, fontInfo) {
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

extern CFontInfo *fontWNormal;
extern CFontInfo *fontWWhisper;

CBWoodringNormal::CBWoodringNormal(const char *str) : CBalloon(str, fontWNormal) { Capitalize(m_str); }
CBWoodringWhisper::CBWoodringWhisper(const char *str) : CBWoodringNormal(str) { m_fontI = fontWWhisper; /* reset to whisper font */ }
CBWoodringThink::CBWoodringThink(const char *str) : CBWoodringNormal (str) {}

// x and y are in balloon space
void BreakSpline(CBalloon *balloon, CSpline *spline, int x, int y, double oFactor) {
	POINT left, leftNearest, rightNearest;
	int leftKnotIndex, rightKnotIndex;
	int nCps = spline->nCps;

//	TRACE("Got an oFactor of %f.\n", oFactor);
	int gapwidth = (int) ((80 + 0 /*(int)(randfloat() * 15)*/) * oFactor);
//	TRACE("Gapwidth = %d.\n", gapwidth);
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
	TRACE("oFactor = %f.\n", oFactor);

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


void CBWoodringNormal::Draw(CDC* dc, POINT *ul, RECT *dmgArea)
{
	CPen *oldPen = dc->SelectObject(&m_pen);
	// use comic font by default
	CFont *oldFont = dc->SelectObject(m_fontI->m_font);

	DrawPoint(dc, tpoint);

	dc->OffsetWindowOrg(-m_bbox.Left, -m_bbox.Top);
	if (!m_traj)
		SetBalloonTraj();
	m_traj->Draw(dc);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));
	dc->SelectObject(brush);
	dc->StrokeAndFillPath();
	dc->SetBkMode(TRANSPARENT);
	int iBaseY = 0;
	for (int i = 0; i < m_fInfo->nLines; i++)
	{
		dc->TextOut(m_fInfo->leftX[i],
					iBaseY,
					m_fInfo->starts[i],
					m_fInfo->lengths[i]);
		iBaseY -= m_fontI->m_lineHeight;
	}

	dc->OffsetWindowOrg(m_bbox.Left, m_bbox.Top);
	dc->SelectObject(oldPen);
	dc->SelectObject(oldFont);
}


void CBWoodringWhisper::Draw(CDC* dc, POINT *ul, RECT *dmgArea)
{
	CPen *oldPen = dc->SelectObject(&m_nimbusPen);
	// use comic font by default
	CFont *oldFont = dc->SelectObject(m_fontI->m_font);

	dc->OffsetWindowOrg(-m_bbox.Left, -m_bbox.Top);
	if (!m_traj)
		SetBalloonTraj();
	m_traj->Draw(dc);

	CBrush brush;
	brush.CreateSolidBrush(RGB(255, 255, 255));

	dc->SelectObject(brush);
	dc->StrokeAndFillPath();

	dc->SelectObject(&m_pen);
	m_traj->Dash(dc);

	dc->SetBkMode(TRANSPARENT);
	int iBaseY = 0;
	for (int i = 0; i < m_fInfo->nLines; i++)
	{
		dc->TextOut(m_fInfo->leftX[i],
					iBaseY,
					m_fInfo->starts[i],
					m_fInfo->lengths[i]);
		iBaseY -= m_fontI->m_lineHeight;
	}

	dc->OffsetWindowOrg(m_bbox.Left, m_bbox.Top);
	dc->SelectObject(oldPen);
	dc->SelectObject(oldFont);
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





#if 0

// To be fast(er), uses manhattan distance rather than euclidean distance.  Good 'nuff.
void DashPath(CDC *dc, int dashArray[], int nArray) {
	BOOL inDash, closed = FALSE;
	VERIFY(dc->FlattenPath());
	int nPoints = dc->GetPath(NULL, NULL, 0);
	LPPOINT pts = (LPPOINT) malloc (nPoints * sizeof(POINT));
	LPBYTE types = (LPBYTE) malloc (nPoints * sizeof(BYTE));

	int oldMode = dc->SetMapMode(MM_TEXT);   // do to a bug w/ inverse mapping in GetPath, this seems to be necessary (?)
	dc->GetPath(pts, types, nPoints);
	if (types[nPoints-1] & PT_CLOSEFIGURE) {
		nPoints++;   // fool it into thinking there's an extra point
		closed = TRUE;
	}
	int partialDist, arrayIndex, nextDist, distLimit, thisType;
	POINT lastPoint, lastMove, thisPoint;      // will be set, since first type always a moveto (?)

	for (int i = 0; i < nPoints; i++) {
	  if (i < nPoints-1 || !closed) {
		thisPoint = pts[i];
		thisPoint.x &= 0xffff;
		thisPoint.y &= 0xffff;   // upper word is munged in win995
		thisType = types[i] & ~PT_CLOSEFIGURE;
	  } else {
		thisPoint = lastMove;
		thisType = PT_LINETO;
	  }
	  switch (thisType) {
		case PT_LINETO:
			while (TRUE) {
				nextDist = manhattan_dist(lastPoint, thisPoint);
				if (nextDist + partialDist < distLimit) break;   // remainder of seg completely in or out of a dash;
				// this seg straddled by a dash
				POINT deltaVec = point_sub(thisPoint, lastPoint);
				DPOINT deltaVecN;
				deltaVecN.x = (double) deltaVec.x / nextDist;
				deltaVecN.y = (double) deltaVec.y / nextDist;
				POINT interpedPoint = point_add(lastPoint, dpoint_to_point(point_scalmult((double)distLimit - partialDist, deltaVecN)));
				if (inDash) dc->LineTo(interpedPoint);
				else dc->MoveTo(interpedPoint);
				lastPoint = interpedPoint;
				inDash = !inDash;
				partialDist = 0;
				arrayIndex = (arrayIndex + 1) % nArray;
				distLimit = dashArray[arrayIndex];
			}
			partialDist = nextDist + partialDist;      // remainder of seg completely in or out of dash
			if (inDash) dc->LineTo(thisPoint);
			lastPoint = thisPoint;
			break;
		case PT_MOVETO:								  // first point should be a moveto -- will initialize these vars
			lastMove = lastPoint = thisPoint;
			inDash = TRUE;							 // start over w/ dash pattern
			partialDist = 0;
			arrayIndex = 0;
			distLimit = dashArray[0];
			dc->MoveTo(lastPoint);
			break;
		default:
			ASSERT(FALSE);		// flattened path should be only linetos and movetos
			break;
	  }
	}

	free(pts);					// cleanup
	free(types);
	dc->SetMapMode(oldMode);
}
#endif

  
// int dashArray[] = { 100, 100 };


#if 0
void CBWoodringNormal::DrawWhisperNimbus(CDC *dc, POINT *ul, RECT *dmgArea) {
	CPen *oldPen = dc->SelectObject(&m_whisperPen);
	dc->OffsetWindowOrg(-m_bbox.Left, -m_bbox.Top);
	if (!m_traj) SetBalloonTraj();
	m_traj->Draw(dc);

	dc->StrokePath();

	dc->OffsetWindowOrg(m_bbox.Left, m_bbox.Top);
	dc->SelectObject(oldPen);
}
#endif

void CLabel::Draw(CDC *dc, POINT *ul, RECT *dmgArea)
{
	int		rgiLengths[30], rgiWidths[30], iLeftText;
	char*	rgszStarts[30];

	CFont*	oldFont = dc->SelectObject(m_fontI->m_font);
	CSize	dwExtent = dc->GetTextExtent(m_str, strlen(m_str));
	int		iDesiredWidth = m_bbox.Right - m_bbox.Left;
	int		nLines = ::BreakIntoLines(dc, iDesiredWidth, m_str, rgszStarts, rgiLengths, rgiWidths);
	int		iStartTop = m_bbox.Top;

	dc->SetBkMode(TRANSPARENT);

	for (int i = 0; i < nLines; i++)
	{
		if (m_format & FT_LEFT_JUSTIFY)
			iLeftText = m_bbox.Left;
		else
			iLeftText = m_bbox.Left + ((CUnitPanelPage::unitWidth - rgiWidths[i]) / 2); // Kludge - fix
		dc->TextOut(iLeftText,
					iStartTop,
					rgszStarts[i],
					rgiLengths[i]);
		iStartTop -= m_fontI->m_lineHeight;
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

int CLabel::BreakIntoLines(CFormatInfo &fInfo)
{
	CDC*	dc = GetClientDC();
	CFont*	oldFont = dc->SelectObject(m_fontI->m_font);
	CSize	dwExtent = dc->GetTextExtent(m_str, strlen(m_str));
	int		iDesiredWidth = m_bbox.Right - m_bbox.Left;
	
	fInfo.m_nLines = ::BreakIntoLines(dc, iDesiredWidth, m_str, fInfo.m_rgszStarts,
									  fInfo.m_rgiLengths, fInfo.m_rgiWidths);

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

	dc->SelectObject(oldFont);
	return fInfo.m_nLines;
}

int CLabel::AreaEstimate(int* piLen, int* piLineHeight)
{
	CDC*	dc = GetClientDC();
	CFont*	oldFont = dc->SelectObject(m_fontI->m_font);
	CSize	dwExtent = dc->GetTextExtent(m_str, strlen(m_str));

	dc->SelectObject(oldFont);
	
	*piLen = dwExtent.cx;
	*piLineHeight = m_fontI->m_lineHeight;

	return ((int) (1.3 * dwExtent.cx * (dwExtent.cy + *piLineHeight)));
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
	fInfo.m_bShifted = TRUE;
}


void CBalloon::DockAtTop(int iHeight)
{
	int iOldBBoxHeight = m_bbox.Top - m_bbox.Bottom;

	m_bbox.Top = iHeight + TOPBORDER;
	m_bbox.Bottom = m_bbox.Top - iOldBBoxHeight;
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

BOOL CBalloon::Overlap(CBalloon *b2)
{
	RECT cb1, cb2;
	GetCloudBBox(&cb1);
	b2->GetCloudBBox(&cb2);
	return (bbox_overlap(&cb1, &cb2));
}


void CBalloon::ComputeCloudBBox()
{ // called only to compute it from scratch
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
	if (c.m_str) m_str = strdup(c.m_str);
	m_fontI = c.m_fontI;
	m_format = c.m_format;
}


CBalloon::CBalloon(const CBalloon& b)
: CLabel(b) {
	if (b.m_fInfo) {
		m_fInfo = new CFormatInfo;
		*m_fInfo = *b.m_fInfo;
	}
	m_speaker = b.m_speaker ? b.m_speaker->Clone() : NULL;
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

CFontInfo::CFontInfo(CFont *font, int leading, int baseAdd) {
	TEXTMETRIC tm;

	m_font = font;
	m_leading = (char) leading;
	m_baseAdd = (char) baseAdd;

	CDC *dc = GetClientDC();
	CFont *oldFont = dc->SelectObject(font);
	VERIFY( dc->GetTextMetrics(&tm) );
	m_lineHeight = (short)(tm.tmHeight + leading);
	dc->SelectObject(oldFont);
}








