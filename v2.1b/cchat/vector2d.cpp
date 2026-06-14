#include "stdafx.h"

#include <stdio.h>
#include <math.h>
#include "vector2d.h"

// FIRST FOR DPOINTS.

DPOINT point_sub(DPOINT pt1, DPOINT pt2)
{
	DPOINT newpt;
	newpt.x = pt1.x - pt2.x;
	newpt.y = pt1.y - pt2.y;
	return newpt;
}

DPOINT point_add(DPOINT pt1, DPOINT pt2)
{
	DPOINT newpt;
	newpt.x = pt1.x + pt2.x;
	newpt.y = pt1.y + pt2.y;
	return newpt;
}

DPOINT point_scalmult(double scalar, DPOINT pt)
{
	pt.x *= scalar;
	pt.y *= scalar;
	return pt;
}

double point_dot(DPOINT pt1, DPOINT pt2)
{
	return (pt1.x * pt2.x + pt1.y * pt2.y);
}

double point_dist(DPOINT pt1, DPOINT pt2)
{
	double diffx = pt1.x - pt2.x;
	double diffy = pt1.y - pt2.y;
	return (sqrt (diffx * diffx + diffy * diffy));
}

double point_distsq(DPOINT pt1, DPOINT pt2)
{
	double diffx = pt1.x - pt2.x;
	double diffy = pt1.y - pt2.y;
	return (diffx * diffx + diffy * diffy);
}

double point_magn(DPOINT pt)
{
	return (sqrt (pt.x * pt.x + pt.y * pt.y));
}

DPOINT point_norm(DPOINT pt)
{
	double magn;

	if ((magn = point_magn(pt)) < SMALLNUMBER) {
		TRACE("Can't normalize the unit vector.\n");
		pt.x = pt.y = 0.0;
		return pt;
	}
	return (point_scalmult (1./magn, pt));
}

double vector_to_angle (DPOINT vec)
{
	if (fabs(vec.x) < SMALLNUMBER && fabs(vec.y) < SMALLNUMBER) {
		TRACE("vector_to_angle: vector too small.\n");
		return (0.0);
	}
	else return (atan2(vec.y, vec.x));
}

// NOW FOR REGULAR POINTS

POINT point_sub(POINT pt1, POINT pt2)
{
	POINT newpt;
	newpt.x = pt1.x - pt2.x;
	newpt.y = pt1.y - pt2.y;
	return newpt;
}

POINT point_add(POINT pt1, POINT pt2)
{
	POINT newpt;
	newpt.x = pt1.x + pt2.x;
	newpt.y = pt1.y + pt2.y;
	return newpt;
}

POINT point_scalmult(double scalar, POINT pt)
{
	pt.x = (int)(pt.x * scalar); // should round
	pt.y = (int)(pt.y * scalar); // should round
	return pt;
}

double point_dot(POINT pt1, POINT pt2)
{
	return (pt1.x * pt2.x + pt1.y * pt2.y);
}

double point_dist(POINT pt1, POINT pt2)
{
	double diffx = pt1.x - pt2.x;
	double diffy = pt1.y - pt2.y;
	return (sqrt (diffx * diffx + diffy * diffy));
}

double point_distsq(POINT pt1, POINT pt2)
{
	double diffx = pt1.x - pt2.x;
	double diffy = pt1.y - pt2.y;
	return (diffx * diffx + diffy * diffy);
}

int manhattan_dist(POINT pt1, POINT pt2)
{
	return( abs(pt1.x - pt2.x) + abs(pt1.y - pt2.y));
}

double point_magn(POINT pt)
{
	return (sqrt ((double)(pt.x * pt.x + pt.y * pt.y)));
}

POINT point_norm(POINT pt)
{
	double magn;

	if ((magn = point_magn(pt)) < SMALLNUMBER) {
		TRACE("Can't normalize the unit vector.\n");
		pt.x = pt.y = 0;
		return pt;
	}
	return (point_scalmult (1./magn, pt));
}

double vector_to_angle (POINT vec)
{
	if (abs(vec.x) < SMALLNUMBER && abs(vec.y) < SMALLNUMBER) {
		TRACE("vector_to_angle: vector too small.\n");
		return (0.0);
	}
	else return (atan2((double) vec.y, (double) vec.x));
}

DPOINT point_to_dpoint(POINT pt) {
	DPOINT dpt;
	dpt.x = pt.x;
	dpt.y = pt.y;
	return dpt;
}

POINT dpoint_to_point(DPOINT dpt) {
	POINT pt;
	pt.x = ROUND(dpt.x);
	pt.y = ROUND(dpt.y);
	return pt;
}

// SOME ANGLE ROUTINES

double degrees_to_rads (double degrees) {
	return (degrees * (PI / 180.0));
}

DPOINT angle_to_vector (double angle) {
	DPOINT vec;
	vec.x = cos(angle);
	vec.y = sin(angle);
	return vec;
}


// converts to an angle in (-180, 180]
double value_to_angle (double value) {
	if (value > -PI && value <= PI) return value;
	double temp = value / (2*PI);
	temp = (temp - (int)temp) * 2*PI;  // just retain fractional component
	if (temp > PI) return(temp - 2*PI);
	else if (temp <= -PI) return(temp + 2*PI);
	else return(temp);
}

double add_angles (double angle1, double angle2) {
	return value_to_angle(angle1 + angle2);
}

double subtract_angles (double angle1, double angle2) {
	return value_to_angle(angle1 - angle2);
}

double angle_between_vecs (DPOINT vec1, DPOINT vec2) {
	return (subtract_angles(vector_to_angle(vec2), vector_to_angle(vec1)));
}

/*
; converts to an angle in (-180, 180]     :-) => last paren for matching!
(defun value-to-angle (value)
  (cond ((and (> value -180) (<= value 180)) value)
	(t (let ((temp (mod value 360)))
	     (if (<= temp 180.0) temp (- temp 360))))))
*/
