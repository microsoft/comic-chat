/************************************************************************
*************************************************************************
************************** Splineutils.c ********************************
*************** This file contains spline class code. *******************
*************************************************************************
Author: David Kurlander (mainly code that I developed at Columbia U.)
*************************************************************************
************************************************************************/

#include "stdafx.h"    // necessary for precompiled header
#include "chat.h"

#include "traj.h"
#include "spline.h"
#include "vector2d.h"
#include <math.h>

/* split_bezier() - takes a bezier (b) and splits it into two new beziers
		    (right, left) which together form the first */
void split_bezier(BEZIER *b, BEZIER *left, BEZIER *right) {
	/* Algorithm from The Killer B's Intro to Spline in Computer Graphics.
	   Efficient implementation of:
		S0 = V0;
		S1 = (1/2) * (V0 + V1);
		S2 = (1/4) * (V0 + 2V1 + V2);
		S3 = (1/8) * (V0 + 3V1 + 3V2 + V3);
		T0 = (1/8) * (V0 + 3V1 + 3V2 + V3);
		T1 = (1/4) * (V1 + 2V2 + V3)
		T2 = (1/2) * (V2 + V3);
		T3 = V3;
				(V = original, S = left, T = right)
	*/
	DPOINT t;

	left->p0 = b->p0;
	left->p1 = point_scalmult(.5, point_add(b->p0, b->p1));
	t = point_scalmult(.5, point_add(b->p1, b->p2));
	left->p2 = point_scalmult(.5, point_add(left->p1, t));
	right->p3 = b->p3;
	right->p2 = point_scalmult(.5, point_add(b->p2, b->p3));
	right->p1 = point_scalmult(.5, point_add(t, right->p2));
	left->p3 = right->p0 =
		point_scalmult(.5, point_add(left->p2, right->p1));
}

BOOL inside_bbox_tol(DPOINT *pt, BOUNDBOX *bbox, double tol)
{
	if ((pt->x + tol < bbox->xmin) || (pt->x - tol >bbox->xmax)
		|| (pt->y + tol < bbox->ymin) || (pt->y - tol > bbox->ymax))
		return FALSE;
	else return TRUE;
}

/* flat_bezier() - returns true if the specified bezier is within epsilon
		   of being flat.  Algorithm is only an approximation.
		   Again, thanks to the folks at PARC */
double epsilon = 1.0;
flat_bezier(BEZIER *b) {
	double dx, dy, dxdy, dydx;
	DPOINT d1, d2, d;
	BOUNDBOX bbox;

	bbox.xmin = MIN(b->p0.x, b->p3.x);
	bbox.xmax = MAX(b->p0.x, b->p3.x);
	bbox.ymin = MIN(b->p0.y, b->p3.y);
	bbox.ymax = MAX(b->p0.y, b->p3.y);
	if (!inside_bbox_tol(&b->p1, &bbox, .5 * epsilon) ||
		!inside_bbox_tol(&b->p2, &bbox, .5 * epsilon))
		return FALSE;

	d1 = point_sub(b->p1, b->p0);
	d2 = point_sub(b->p2, b->p0);
	d =  point_sub(b->p3, b->p0);
	dx = ABS(d.x);
	dy = ABS(d.y);
	if (dx + dy < epsilon) return TRUE;
	if (dy < dx) {
		dydx = d.y/d.x;
		return (fabs(d2.y-(d2.x*dydx)) < epsilon &&
			fabs(d1.y-(d1.x*dydx)) < epsilon);
	} else {
		dxdy = d.x/d.y;
		return (fabs(d2.x-(d2.y*dxdy)) < epsilon &&
			fabs(d1.x-(d1.y*dxdy)) < epsilon);
	}
}


/* subdivide() traverses a bezier, calling the specified call-back procedure
	       at points roughly delta apart.  Returns when the call-back
	       proc returns TRUE. */
subdivide(BEZIER *bezier, int (*proc)(DPOINT *, void *), void *arg, double delta)
{
	double length, step, alpha;
	DPOINT pt;
	BEZIER right, left;

	if (flat_bezier(bezier)) {
	    length = point_dist(bezier->p0, bezier->p3);
	    if (length > SMALLNUMBER) {
		step = delta/length;
		for (alpha = 0.; alpha <= 1.0; alpha += step) {
			pt = point_add(point_scalmult(alpha, bezier->p3),
				       point_scalmult(1.0-alpha, bezier->p0));
			if ((*proc)(&pt, arg)) return TRUE;
		}
	    }
	    return ((*proc)(&bezier->p3, arg));
	}
	else {
		split_bezier(bezier, &left, &right);
		if (subdivide(&left, proc, arg, delta) ||
		    subdivide(&right, proc, arg, delta))
			return TRUE;
		else return FALSE;
	}
}


/* walk_path - walks an array of beziers, stopping every epsilon or so
		to run the call-back proc (proc) on its generic argument
		(arg).  if the call-back proc returns TRUE, the procedure
		terminates immediately.  */
walk_path(int n, BEZIER *beziers, int (*proc)(DPOINT *, void *), void *arg)
{
	int i;
	for (i = 0; i < n; i++)
		if (subdivide(&(beziers[i]), proc, arg, epsilon)) return TRUE;
	return FALSE;
}


#define TOL 2.0

cb_on_line(DPOINT *pt, DPOINT *testpt) {
	if (fabs(pt->x - testpt->x) + fabs(pt->y - testpt->y) <= TOL)
		return TRUE;
	return FALSE;
}


struct nearinfo {
	double dist;
	DPOINT given_pt, found_pt;
};

void spline_nearest_point(BEZIER beziers[], int n, DPOINT *given_pt, double *dist, DPOINT *found_pt)
{
	struct nearinfo neararg;
	BOOL cb_nearest(DPOINT *, void *);
	double sqrt();
//	SPLINEDATA *data;

	neararg.given_pt = *given_pt;
	neararg.dist = LARGENUMBER;

	walk_path(n, beziers, cb_nearest, (void *)&neararg);

	*dist = neararg.dist;		/* manhattan distance */
	*found_pt = neararg.found_pt;
}


void bezier_nearest_point(double bezpts[][2], double given_x, double given_y, double *dist,
					      double *found_x, double *found_y) {
    BEZIER b;
    DPOINT given_pt, found_pt;
    b.p0.x = bezpts[0][0];
    b.p0.y = bezpts[0][1];
    b.p1.x = bezpts[1][0];
    b.p1.y = bezpts[1][1];
    b.p2.x = bezpts[2][0];
    b.p2.y = bezpts[2][1];
    b.p3.x = bezpts[3][0];
    b.p3.y = bezpts[3][1];
    given_pt.x = given_x;
    given_pt.y = given_y;
    spline_nearest_point(&b, 1, &given_pt, dist, &found_pt);
    *found_x = found_pt.x;
    *found_y = found_pt.y;
}

/* An integer-based stub for the spline_nearest_point routine */
void int_bezier_nearest_point(POINT *bezpts, POINT& given, int *dist, POINT *found) {
	BEZIER b;
	DPOINT given_dpoint, found_dpoint;
	double d_dist;

	b.p0.x = bezpts[0].x;
	b.p0.y = bezpts[0].y;
	b.p1.x = bezpts[1].x;
	b.p1.y = bezpts[1].y;
	b.p2.x = bezpts[2].x;
	b.p2.y = bezpts[2].y;
	b.p3.x = bezpts[3].x;
	b.p3.y = bezpts[3].y;
	given_dpoint.x = given.x;
	given_dpoint.y = given.y;
	spline_nearest_point(&b, 1, &given_dpoint, &d_dist, &found_dpoint);
	*dist = (int)d_dist;
	found->x = (int)found_dpoint.x;  // should round
	found->y = (int)found_dpoint.y;
}

/* cb_nearest() - call-back proc for spline_nearest_point() */
BOOL cb_nearest(DPOINT *pt, void *arg)
{
	double thisdist, point_distsq();
	struct nearinfo *a = (struct nearinfo *) arg;

/*
	thisdist = point_distsq(*pt, a->given_pt);
	instead of this, to speed things up, let's use manhattan distance
*/
	thisdist = fabs(pt->x - a->given_pt.x) + fabs(pt->y - a->given_pt.y);
	if (thisdist < a->dist) {
		a->dist = thisdist;
		a->found_pt = *pt;
	}
	return FALSE;
}

/* flatten() traverses a bezier, flattening it into segments, and calls
   the specified callback on segment end points (not including the first)
 */
flatten(BEZIER *bezier, int (*proc)(DPOINT *, void *), void *arg)
{
	BEZIER right, left;

	if (flat_bezier(bezier)) {
		return ((*proc)(&bezier->p3, arg));
	}
	else {
		split_bezier(bezier, &left, &right);
		if (flatten(&left, proc, arg) ||
		    flatten(&right, proc, arg))
			return TRUE;
		else return FALSE;
	}
}


/* An integer-based stub for the spline_nearest_point routine */
void int_bezier_flatten(POINT *bezpts, int (*proc)(DPOINT *, void *), void *arg) {
	BEZIER b;

	b.p0.x = bezpts[0].x;
	b.p0.y = bezpts[0].y;
	b.p1.x = bezpts[1].x;
	b.p1.y = bezpts[1].y;
	b.p2.x = bezpts[2].x;
	b.p2.y = bezpts[2].y;
	b.p3.x = bezpts[3].x;
	b.p3.y = bezpts[3].y;

	flatten(&b, proc, arg);
}

BOOL walk_horizontal_dist(POINT *bezpts, int goalX, POINT &furthest) {
	BEZIER b;
	BOOL cb_beyond_deltaX(DPOINT *, void *);
	struct nearinfo neararg;

	b.p0.x = bezpts[0].x;
	b.p0.y = bezpts[0].y;
	b.p1.x = bezpts[1].x;
	b.p1.y = bezpts[1].y;
	b.p2.x = bezpts[2].x;
	b.p2.y = bezpts[2].y;
	b.p3.x = bezpts[3].x;
	b.p3.y = bezpts[3].y;

	// found_pt.x holds best X found so far -- start w/ number less than any in spline
	// found_pt.x and found_pt.y will be replaced w/ better values
	neararg.found_pt.x = -1000000;

	// given_pt.x holds goalX, rest irrelevant
	neararg.given_pt.x = goalX;

	BOOL found = walk_path(1, &b, cb_beyond_deltaX, (void *) &neararg);
	furthest = dpoint_to_point(neararg.found_pt);
	return found;
}


// fill in found_pt if better than the current stored point.
// if pt.x > given_pt.x, also return TRUE -- got the point!
BOOL cb_beyond_deltaX(DPOINT *pt, void *arg) {
	struct nearinfo *neararg = (struct nearinfo *) arg;
	if (pt->x > neararg->found_pt.x)
		neararg->found_pt = *pt;
	return (pt->x >= neararg->given_pt.x);
}




