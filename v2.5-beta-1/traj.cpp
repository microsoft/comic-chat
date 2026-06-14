#include "stdafx.h"

#include "traj.h"
#include "vector2d.h"

int dashArray[] = {100, 100};


// To be fast(er), uses manhattan distance rather than euclidean distance.  Good 'nuff.
void DashSeg(POINT &thisPoint, DASHINFO &d) {
	int nextDist;
	int distLimit = d.dashArray[d.arrayIndex];
	while (TRUE) {
		nextDist = manhattan_dist(d.lastPoint, thisPoint);
		if (nextDist + d.partialDist < d.dashArray[d.arrayIndex]) break;   // remainder of seg completely in or out of a dash;
		// this seg straddled by a dash
		POINT deltaVec = point_sub(thisPoint, d.lastPoint);
		DPOINT deltaVecN;
		deltaVecN.x = (double) deltaVec.x / nextDist;
		deltaVecN.y = (double) deltaVec.y / nextDist;
		POINT interpedPoint = point_add(d.lastPoint, dpoint_to_point(point_scalmult((double)distLimit - d.partialDist, deltaVecN)));
		if (d.inDash) (d.dc)->LineTo(interpedPoint);
		else (d.dc)->MoveTo(interpedPoint);
		d.lastPoint = interpedPoint;
		d.inDash = !(d.inDash);
		d.partialDist = 0;
		d.arrayIndex = (d.arrayIndex + 1) % d.nIndices;
		distLimit = d.dashArray[d.arrayIndex];
	}
	d.partialDist += nextDist;							// remainder of seg completely in or out of dash
	if (d.inDash) (d.dc)->LineTo(thisPoint);
	d.lastPoint = thisPoint;
}


CTraj::~CTraj() {
	POSITION pos = m_segs.GetHeadPosition();
	while (pos) {
		CSeg *seg = (CSeg *) m_segs.GetNext(pos);
		delete seg;
	}
}


void CTraj::Draw(CDC *dc) {
	BOOL firstSeg = TRUE;

	dc->BeginPath();
	POSITION pos = m_segs.GetHeadPosition();
	while (pos) {
		CSeg *seg = (CSeg *) m_segs.GetNext(pos);
		if (firstSeg) {
			dc->MoveTo(seg->SegLo());
			firstSeg = FALSE;
		}
		seg->Draw(dc);
	}
	if (m_closed) dc->CloseFigure();
	dc->EndPath();
}


void CTraj::Dash(CDC *dc) {
	BOOL firstSeg = TRUE;
	DASHINFO d;

	POSITION pos = m_segs.GetHeadPosition();
	while (pos) {
		CSeg *seg = (CSeg *) m_segs.GetNext(pos);
		if (firstSeg) {
			d.lastPoint = seg->SegLo();
			dc->MoveTo(d.lastPoint);
			firstSeg = FALSE;
			d.inDash = TRUE;
			d.partialDist = 0;
			d.arrayIndex = 0;
			d.dc = dc;
			d.nIndices = 2;
			d.dashArray = dashArray;
		}
		seg->Dash(d);
	}
}


void CLine::Draw(CDC *dc)
{
	dc->LineTo(m_hi);
}


void CLine::Dash(DASHINFO &d)
{
	DashSeg(m_hi, d);
}


void CArc::Draw(CDC *dc) {
	void DrawArc2(CDC *, POINT&, POINT&, int);
	DrawArc2(dc, m_lo, m_hi, m_altitude);
}


void CArc::Dash(DASHINFO &d) {
	void DashArc2(DASHINFO &, POINT&, POINT&, int);
	DashArc2(d, m_lo, m_hi, m_altitude);
}
