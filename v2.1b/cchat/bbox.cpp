#include "stdafx.h"
#include "bbox.h"
#include "vector2d.h"

void adjust_bbox(RECT *bbox, int delta) {
	bbox->left -= delta;
	bbox->bottom -= delta;
	bbox->right += delta;
	bbox->top += delta;
}

void bbox_around_pt(RECT *bbox, POINT *pt, int delta=0) {
	bbox->left = bbox->right = pt->x;
	bbox->top = bbox->bottom = pt->y;
	if (delta) adjust_bbox(bbox, delta);
}

void bbox_in_bbox (RECT *source, RECT *dest) {
	dest->left = min(source->left, dest->left);
	dest->bottom = min(source->bottom, dest->bottom);
	dest->right = max(source->right, dest->right);
	dest->top = max(source->top, dest->top);
}

void include_pt_in_bbox(POINT *pt, RECT *bbox) {
	bbox->left = min(pt->x, bbox->left);
	bbox->bottom = min(pt->y, bbox->bottom);
	bbox->right = max(pt->x, bbox->right);
	bbox->top = max(pt->y, bbox->top);
}

void include_pt_in_bbox(POINT *pt, SRECT *bbox) {
	bbox->Left = (short) min(pt->x, bbox->Left);
	bbox->Bottom = (short) min(pt->y, bbox->Bottom);
	bbox->Right = (short) max(pt->x, bbox->Right);
	bbox->Top = (short) max(pt->y, bbox->Top);
}

BOOL inside_bbox (POINT *pt, RECT *bbox) {
	return ((pt->x >= bbox->left) &&
			(pt->x <= bbox->right) &&
			(pt->y >= bbox->bottom) &&
			(pt->y <= bbox->top));
}

BOOL inside_bbox (POINT *pt, SRECT *bbox) {
	return ((pt->x >= bbox->Left) &&
			(pt->x <= bbox->Right) &&
			(pt->y >= bbox->Bottom) &&
			(pt->y <= bbox->Top));
}

BOOL inside_bbox_tol (POINT *pt, RECT *bbox, int tol) {
	return ((pt->x + tol >= bbox->left) &&
			(pt->x - tol <= bbox->right) &&
			(pt->y + tol >= bbox->bottom) &&
			(pt->y - tol <= bbox->top));
}

BOOL bbox_overlap (RECT *bbox1, RECT *bbox2) {
	return (!((bbox1->left > bbox2->right) ||
			  (bbox2->left > bbox1->right) ||
			  (bbox1->bottom > bbox2->top) ||
			  (bbox2->bottom > bbox1->top)));
}
			  

BOOL bbox_within_bbox (RECT *bbox1, RECT *bbox2) {
	POINT pt1, pt2;
	pt1.x = bbox1->right;
	pt1.y = bbox1->top;
	pt2.x = bbox1->left;
	pt2.y = bbox2->bottom;

	return (inside_bbox_tol(&pt1, bbox2, 0) && inside_bbox_tol(&pt2, bbox2, 0));
}

BOOL is_empty (RECT *bbox) {
	return ((bbox->left > bbox->right || bbox->bottom > bbox->top));
}

void make_empty (RECT *bbox) {
	bbox->left = bbox->bottom = LARGEINTEGER;
	bbox->right = bbox->top = -LARGEINTEGER;
}

void make_empty (SRECT *bbox) {
	bbox->Left = bbox->Bottom = LARGESHORT;
	bbox->Right = bbox->Top = -LARGESHORT;
}

BOOL bbox_intersect(RECT *bbox1, RECT *bbox2, RECT *result) {
	result->left = max(bbox1->left, bbox2->left);
	result->right = min(bbox1->right, bbox2->right);
	result->top = min(bbox1->top, bbox2->top);
	result->bottom = max(bbox1->bottom, bbox2->bottom);
	return (is_empty(result));
}

RECT SRECTToRECT(SRECT &s) {
	RECT r;
	r.left = s.Left;
	r.top = s.Top;
	r.right = s.Right;
	r.bottom = s.Bottom;
	return r;
}
