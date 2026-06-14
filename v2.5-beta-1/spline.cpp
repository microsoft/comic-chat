#include "stdafx.h"
//#include "client.h"

#include "traj.h"
#include "spline.h"
#include "vector2d.h"

// int splineDebug = 1;

CSpline::CSpline(POINT cpArray[], int n, BOOL isClosed = FALSE) {
	ASSERT (n >= 2);
	nCps = n;
	cps = (POINT *) malloc (nCps * sizeof(POINT));
	for (int i = 0; i < nCps; i++)
		cps[i] = cpArray[i];
	bezpts = NULL;
	// for now, assume that it's closed only if endpoints are the same
	closed = isClosed;
}

CSpline::CSpline(const CSpline &s) {
	closed = s.closed;
	matrix = s.matrix;
	nCps = s.nCps;
	if (s.cps) {
		cps = (POINT *) malloc (nCps * sizeof(POINT));
		for (int i = 0; i < nCps; i++)
			cps[i] = s.cps[i];
	}
	// note: Bezpts are actually copied in the derived class constructor,
	//   since only then can we call KnotCount() [Called by BezierCount()].
	// for now, just null out bezpts
	bezpts = NULL;
}


CSpline::~CSpline() {
	if (nCps > 0)
		free(cps);
	if (bezpts)
		free(bezpts);
}

double CCardinal::defaultTension = 0.4;

CCardinal::CCardinal(POINT cpArray[], int n, BOOL isClosed = FALSE) :
	CSpline(cpArray, n, isClosed) {
		tension = defaultTension;
		SetMatrix(tension);
		ComputeBezpts();   // note: closed val must be set first
}

CCardinal::CCardinal(const CCardinal& c) :
	CSpline(c) {
	tension = c.tension;

	if (c.bezpts) {
		int nBezpts = BezierCount();
		bezpts = (POINT *) malloc (nBezpts * sizeof(POINT));
		for (int i = 0; i < nBezpts; i++)
			bezpts[i] = c.bezpts[i];
	}
}

double CBeta::defaultTension = 5.0;
double CBeta::defaultBias = 1.0;

CBeta::CBeta(POINT cpArray[], int n, BOOL isClosed = FALSE)
: CSpline(cpArray, n, isClosed) {
	tension = defaultTension;
	bias = defaultBias;
	SetMatrix(tension, bias);
	ComputeBezpts();
}

CBeta::CBeta(const CBeta& b)
: CSpline(b) {
	tension = b.tension;
	bias = b.bias;

	if (b.bezpts) {
		int nBezpts = BezierCount();
		bezpts = (POINT *) malloc (nBezpts * sizeof(POINT));
		for (int i = 0; i < nBezpts; i++)
			bezpts[i] = b.bezpts[i];
	}
}
	

CMapWordToPtr cardinalMatrixMap(10);
CMapStringToPtr betaMatrixMap(10);

void CCardinal::SetMatrix(double tension) {
	void *m;
	if (cardinalMatrixMap.Lookup((WORD)((float)tension), m)) {
		matrix = (MATRIX *) m;
		return;
	}
	matrix = (MATRIX *) malloc (sizeof MATRIX);
	(*matrix)[0][1] = 2.0 - tension;
	(*matrix)[0][2] = tension - 2.0;
	(*matrix)[1][0] = 2.0 * tension;
	(*matrix)[1][1] = tension - 3.0;
	(*matrix)[1][2] = 3.0 - 2.0 * tension;
	(*matrix)[3][1] = 1.0;
	(*matrix)[0][3] = (*matrix)[2][2] = tension;
	(*matrix)[0][0] = (*matrix)[1][3] = (*matrix)[2][0] = -tension;
	(*matrix)[2][1] = (*matrix)[2][3] = (*matrix)[3][0] = (*matrix)[3][2] = (*matrix)[3][3] = 0.0;
	cardinalMatrixMap.SetAt((WORD)((float)tension), matrix);
}

void CBeta::SetMatrix(double tension, double bias) {
	void *m;
	char key[30];	// major hack - use string as key;
	sprintf(key, "%f*%f", tension, bias);
	if (betaMatrixMap.Lookup(key, m)) {
		matrix = (MATRIX *) m;
		return;
	}

	matrix = (MATRIX *) malloc (sizeof MATRIX);
	double b2 = bias * bias;
	double b3 = bias * b2;
	double d = 1.0 / (tension + (2.0 * b3) + (4.0 * (b2 + bias)) + 2.0);

	(*matrix)[0][0] = -2.0 * b3;
	(*matrix)[0][1] = 2.0 * (tension + b3 + b2 + bias);
	(*matrix)[0][2] = -2.0 * (tension + b2 + bias + 1.0);
	(*matrix)[1][0] = 6.0 * b3;
	(*matrix)[1][1] = -3.0 * (tension + (2.0 * (b3 + b2)));
	(*matrix)[1][2] = 3.0 * (tension + 2.0 * b2);
	(*matrix)[2][0] = -6.0 * b3;
	(*matrix)[2][1] = 6.0 * (b3 - bias);
	(*matrix)[2][2] = 6.0 * bias;
	(*matrix)[3][0] = 2.0 * b3;
	(*matrix)[3][1] = tension + (4.0 * (b2 + bias));
	(*matrix)[0][3] = (*matrix)[3][2] = 2.0;
	(*matrix)[1][3] = (*matrix)[2][3] = (*matrix)[3][3] = 0.0;
	
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			(*matrix)[i][j] *= d;

	betaMatrixMap.SetAt(key, matrix);   // key is dup'ed by SetAt -- no need to strdup here
}

void DestroySplineMatrixCaches() {
	WORD cardKey;
	CString betaKey;
	void *p;

	POSITION pos = cardinalMatrixMap.GetStartPosition();
	while (pos) {
		cardinalMatrixMap.GetNextAssoc(pos, cardKey, p);
		MATRIX *m = (MATRIX *) p;
		free(m);
		cardinalMatrixMap.RemoveKey(cardKey);
	}

	pos = betaMatrixMap.GetStartPosition();
	while (pos) {
		betaMatrixMap.GetNextAssoc(pos, betaKey, p);
		MATRIX *m = (MATRIX *) p;
		free(m);
		betaMatrixMap.RemoveKey(betaKey);
	}
}

void CSpline::ComputeBezpts() {
	int nKnots = KnotCount();
	ASSERT(nKnots >= 4);

	/* assuming knots >= 4, bezpts = 3(n-3)+1 */
	if (!bezpts)
		bezpts = (POINT *) malloc (BezierCount() * sizeof(POINT));

	// updates  the bezpts  a cubic spline.  assumes that bezpts and cubics arrays are in place.
//    if (splineDebug)
//		DEBUG("nKnots = %d, nr-cps = %d\n", nKnots, nCPS);
	int bezIndex = 1;
	POINT knot0 = GetKnot(0);
	POINT knot1 = GetKnot(1);
	POINT knot2 = GetKnot(2);
	POINT knot3 = GetKnot(3);
	POINT c0, c1, c2, c3;
	POINT b0, b1, b2, b3;
	for (int i = 0; 1; i++) {
//		if (splineDebug) DEBUG("Looping with i = %d.\n", i);
		CvertsToCubic(knot0, knot1, knot2, knot3, c0, c1, c2, c3);
		CubicToBezier(c0, c1, c2, c3, b0, b1, b2, b3);
		if (i == 0)
			bezpts[0] = b0;
		bezpts[bezIndex] = b1;
		bezpts[bezIndex+1] = b2;
		bezpts[bezIndex+2] = b3;
		if (i + 4 == nKnots) return;
		bezIndex += 3;
		knot0 = knot1;
		knot1 = knot2;
		knot2 = knot3;
		knot3 = GetKnot(i+4);
	}
}

// Note: k's are knots, c's are cubic coefficient pairs
void CSpline::CvertsToCubic(POINT &k0, POINT &k1, POINT &k2, POINT &k3,
							POINT &c0, POINT &c1, POINT &c2, POINT &c3) {
	c3.x = ROUND((*matrix)[0][0] * k0.x + (*matrix)[0][1] * k1.x + (*matrix)[0][2] * k2.x + (*matrix)[0][3] * k3.x);
	c3.y = ROUND((*matrix)[0][0] * k0.y + (*matrix)[0][1] * k1.y + (*matrix)[0][2] * k2.y + (*matrix)[0][3] * k3.y);
	c2.x = ROUND((*matrix)[1][0] * k0.x + (*matrix)[1][1] * k1.x + (*matrix)[1][2] * k2.x + (*matrix)[1][3] * k3.x);
	c2.y = ROUND((*matrix)[1][0] * k0.y + (*matrix)[1][1] * k1.y + (*matrix)[1][2] * k2.y + (*matrix)[1][3] * k3.y);
	c1.x = ROUND((*matrix)[2][0] * k0.x + (*matrix)[2][1] * k1.x + (*matrix)[2][2] * k2.x + (*matrix)[2][3] * k3.x);
	c1.y = ROUND((*matrix)[2][0] * k0.y + (*matrix)[2][1] * k1.y + (*matrix)[2][2] * k2.y + (*matrix)[2][3] * k3.y);
	c0.x = ROUND((*matrix)[3][0] * k0.x + (*matrix)[3][1] * k1.x + (*matrix)[3][2] * k2.x + (*matrix)[3][3] * k3.x);
	c0.y = ROUND((*matrix)[3][0] * k0.y + (*matrix)[3][1] * k1.y + (*matrix)[3][2] * k2.y + (*matrix)[3][3] * k3.y);
}

void CSpline::CubicToBezier(POINT &c0, POINT &c1, POINT &c2, POINT &c3,
							POINT &b0, POINT &b1, POINT &b2, POINT &b3) {
// "cubic" in most of this code means cubic spline.  However, the following
// function takes a real cubic, and returns the bezier points
	b0.x = c0.x;
	b0.y = c0.y;
 	b1.x = c0.x + ROUND((1.0 / 3.0) * c1.x);
	b1.y = c0.y + ROUND((1.0 / 3.0) * c1.y);
	b2.x = b1.x + ROUND((1.0 / 3.0) * (c1.x + c2.x));
	b2.y = b1.y + ROUND((1.0 / 3.0) * (c1.y + c2.y));
	b3.x = c0.x + c1.x + c2.x + c3.x;
	b3.y = c0.y + c1.y + c2.y + c3.y;
}

POINT CSpline::GetKnot(int index) {
	if (closed) {   // if it's a closed spline
		if (index == 0)
			return cps[nCps-1];
		else if (index == nCps + 1) // mod arith would be slower?
			return cps[0];
		else if (index == nCps + 2)
			return cps[1];
		else return cps[index-1];
	} else {
		int dups = GetDups(); // How many knots must be duplicated
		if (index < dups)
			return cps[0];
		else if (index >= nCps + dups - 2)
			return cps[nCps-1];
		else return cps[index-dups+1];
	}
}

POINT CSpline::ClosestPoint(POINT& toPt, int *knotIndex) {
	int dist, minDist = 10000000;
	int bezCount = BezierCount();
	POINT pos, minPos;
	void int_bezier_nearest_point(POINT*, POINT&, int *, POINT*);

	for (int i = 0; i < bezCount-1; i+=3) {
		int_bezier_nearest_point(bezpts+i, toPt, &dist, &pos);
		if (dist < minDist) {
			minDist = dist;
			minPos = pos;
			*knotIndex = (i / 3) + 2;
		}
	}

	return minPos;
}

POINT CSpline::WalkHorizontalDistance(POINT& fromPt, int fromKnotIndex, int goalX, int& foundKnotIndex) {
	BOOL walk_horizontal_dist(POINT *bezpts, int goalX, POINT &furthest);
	int bezCount = BezierCount();
	POINT furthest, lastFurthest;
	foundKnotIndex = -1;
	int index = (fromKnotIndex - 2)*3;   // rethink this

	furthest.x = furthest.y = 0;	// initialization not really necessary
	lastFurthest.x = lastFurthest.y = -100000;

	for (int i = 0; i < bezCount-1; i+=3) {
		if (index+3 > bezCount-1) index = 0;   // rethink this
		if (walk_horizontal_dist(bezpts+index, goalX, furthest)) {
			foundKnotIndex = index/3 + 2;   // rethink this
			return furthest;
		}

		// If we don't find a value near the goalX, save the rightmost anyway
		if (furthest.x > lastFurthest.x) {
			foundKnotIndex = index/3 + 2;	// rethink this
			lastFurthest = furthest;
		}
		index += 3;
	}

	ASSERT(foundKnotIndex > 0);
	return lastFurthest;				// ideally, this exit will never occur
}

void CSpline::Draw(CDC *dc) {
	dc->PolyBezierTo(bezpts+1, BezierCount()-1);
}

POINT CSpline::SegLo() {
	return bezpts[0];
}

void CSpline::Dash(DASHINFO &d) {
	void int_bezier_flatten(POINT *bezpts, int (*proc)(DPOINT *, void *), void *arg);
	int dash_sample(DPOINT *, void *);
	int bezCount = BezierCount();

	for (int i = 0; i < bezCount-1; i+=3) {
		int_bezier_flatten(bezpts+i, dash_sample, (void *) &d);
	}
}

int dash_sample(DPOINT *p, void *arg) {
	void DashSeg(POINT &thisPoint, DASHINFO &d);

	DashSeg(dpoint_to_point(*p), *((DASHINFO *)arg));
	return FALSE;  // so doesn't terminate early
}