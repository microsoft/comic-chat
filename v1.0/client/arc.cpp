#include "stdafx.h"

#include "traj.h"
#include "vector2d.h"
#include <math.h>

void ScanArcAux(CDC *dc, DPOINT& A, DPOINT& C, POINT& center, double radius, double angle) {
	DPOINT B;
	POINT bcps[3];
	double s = cos(angle/2);
	double tau = 4*s / (3*(s + 1));

	double divisor = (A.x*C.y - A.y*C.x) / (radius*radius);
	B.x = (C.y - A.y) / divisor;
	B.y = (A.x - C.x) / divisor;

	DPOINT tauTimesB = point_scalmult(tau, B);

	// note: base calculations assume that beziers are being placed around (0, 0).
	//		 The addition of center to the beziers offsets this.

	bcps[0].x = ROUND((1-tau)*A.x + tauTimesB.x) + center.x;
	bcps[0].y = ROUND((1-tau)*A.y + tauTimesB.y) + center.y;
	bcps[1].x = ROUND((1-tau)*C.x + tauTimesB.x) + center.x;
	bcps[1].y = ROUND((1-tau)*C.y + tauTimesB.y) + center.y;
	bcps[2] = point_add(dpoint_to_point(C), center);
	dc->PolyBezierTo(bcps, 3);
}

#define ARCSTEP PI/2

void ScanArc(CDC *dc, POINT& absCenter, POINT& start, POINT& end, BOOL ccw = TRUE) {
	DPOINT A = point_to_dpoint(point_sub(start, absCenter));
	DPOINT FinalC = point_to_dpoint(point_sub(end, absCenter));
	DPOINT C;
	double radius = point_magn(A);
	double trueAngle = angle_between_vecs(FinalC, A);
	if (ccw) trueAngle = -trueAngle;
	if (trueAngle <= 0) trueAngle += 2*PI;  // put trueAngle in [0, 2PI) range
	double nextEnd = vector_to_angle(A);
	double step = ccw ? ARCSTEP : -ARCSTEP;
	BOOL doExit = FALSE;

	while (TRUE) {
		if (trueAngle > ARCSTEP) {
			nextEnd += step;
			C = point_scalmult(radius, angle_to_vector(nextEnd));
		} else {
			doExit = TRUE;
			C = FinalC;
			step = trueAngle;
		}
		ScanArcAux(dc, A, C, absCenter, radius, step);
		if (doExit) break;
		A = C;
		trueAngle -= ARCSTEP;
	}
}
	
#if 0
void DrawArc(CDC *dc, POINT& start, POINT& end, int radius, BOOL downStroke) {
	// assumes that we're already at start
	POINT mid, endToMid, midToCenter, absCenter, diagonal, ul, lr;
	double endToMidDist, midToCenterDist;

	mid = point_scalmult(0.5, point_add(start, end)); // midway between centers

	endToMid = point_sub(mid, end);			// vector from end to mid
	endToMidDist = point_magn(endToMid);
	ASSERT(radius >= endToMidDist);			// otherwise no circle meets the criteria
	midToCenterDist = sqrt((double)(radius*radius - endToMidDist*endToMidDist));

	if (downStroke) {
		midToCenter.x = endToMid.y;
		midToCenter.y = -endToMid.x;
	} else {
		midToCenter.x = -endToMid.y;
		midToCenter.y = endToMid.x;
	}

	midToCenter = point_scalmult(midToCenterDist / point_magn(midToCenter), midToCenter);

	// translate vector quantities to absolute coordinates
	absCenter = point_add(point_add(end, endToMid), midToCenter);

	diagonal.x = radius;
	diagonal.y = -radius;
	lr = point_add(absCenter, diagonal);
	ul = point_sub(absCenter, diagonal);

	dc->Arc(ul.x, ul.y, lr.x, lr.y, start.x, start.y, end.x, end.y);	// finally draw it!
}
#endif

// Positive altitude bows the arc to the right of the vector from start to end.
// Assumes that current point is start.
void DrawArc2(CDC *dc, POINT& start, POINT& end, int altitude) {
	POINT mid, endToMid, midToCenter, absCenter;
	double endToMidDist, radius, midToCenterDist;

	if (altitude < 1 && altitude > -1) {
		dc->LineTo(end);
		return;
	}

	mid = point_scalmult(0.5, point_add(start, end));	// midway between centers
	endToMid = point_sub(mid, end);						// vector from end to mid
	endToMidDist = point_magn(endToMid);
	radius = (endToMidDist*endToMidDist + altitude*altitude)/(2*altitude);
	midToCenterDist = radius - altitude;	// note: radius, altitude, midToCenterDist are signed

	ASSERT(fabs(radius) >= fabs(endToMidDist));			// otherwise no circle meets the criteria

	midToCenter.x = endToMid.y;
	midToCenter.y = -endToMid.x;

	midToCenter = point_scalmult(midToCenterDist / point_magn(midToCenter), midToCenter);

	// translate vector quantities to absolute coordinates
	absCenter = point_add(point_add(end, endToMid), midToCenter);

	ScanArc(dc, absCenter, start, end, (altitude > 0));
}

void DashSeg(POINT &thisPoint, DASHINFO &d);
#define SAMPLESTEP .02  // in radians, the maximum allowed linear step along the arc to be dashed

// like DrawArc2, but creates arc-formed dashes
void DashArc2(DASHINFO &d, POINT& start, POINT& end, int altitude) {
	POINT mid, endToMid, midToCenter, absCenter;
	double endToMidDist, radius, midToCenterDist;

	if (altitude < 1 && altitude > -1) {
		DashSeg(end, d);								// *** just dash the arc as a straight seg
		return;
	}

	mid = point_scalmult(0.5, point_add(start, end));	// midway between centers
	endToMid = point_sub(mid, end);						// vector from end to mid
	endToMidDist = point_magn(endToMid);
	radius = (endToMidDist*endToMidDist + altitude*altitude)/(2*altitude);
	midToCenterDist = radius - altitude;	// note: radius, altitude, midToCenterDist are signed

	ASSERT(fabs(radius) >= fabs(endToMidDist));			// otherwise no circle meets the criteria

	midToCenter.x = endToMid.y;
	midToCenter.y = -endToMid.x;

	midToCenter = point_scalmult(midToCenterDist / point_magn(midToCenter), midToCenter);

	// translate vector quantities to absolute coordinates
	absCenter = point_add(point_add(end, endToMid), midToCenter);

	// here begin the major differences w.r.t. DrawArc2
	int ccw = (altitude > 0);

	// Now this code is borrowed from ScanArc... (A little modified)
	DPOINT A = point_to_dpoint(point_sub(start, absCenter));
	DPOINT FinalC = point_to_dpoint(point_sub(end, absCenter));
	DPOINT C;
	double trueAngle = angle_between_vecs(FinalC, A);
	if (ccw) trueAngle = -trueAngle;
	if (trueAngle <= 0) trueAngle += 2*PI;  // put trueAngle in [0, 2PI) range
	double nextEnd = vector_to_angle(A);
	double step = ccw ? SAMPLESTEP : -SAMPLESTEP;
	radius = fabs(radius);				   // radius was signed
	BOOL doExit = FALSE;

	while (TRUE) {
		if (trueAngle > SAMPLESTEP) {
			nextEnd += step;
			C = point_scalmult(radius, angle_to_vector(nextEnd));
		} else {
			doExit = TRUE;
			C = FinalC;
			step = trueAngle;
		}
		POINT M = point_add(dpoint_to_point(C), absCenter);
		DashSeg(point_add(dpoint_to_point(C), absCenter), d);
		if (doExit) break;
		A = C;
		trueAngle -= SAMPLESTEP;
	}
}

