#ifndef __BBOX_H__
#define __BBOX_H__

typedef struct  { // Same as SMALL_RECT, but want symbols to be visible
    SHORT Left;      
    SHORT Top;       
    SHORT Right;     
    SHORT Bottom;    
} SRECT;


void adjust_bbox(RECT *bbox, int delta);
void bbox_around_pt(RECT *bbox, POINT *pt, int delta);
void bbox_in_bbox (RECT *source, RECT *dest);
void include_pt_in_bbox(POINT *pt, RECT *bbox);
void include_pt_in_bbox(POINT *pt, SRECT *bbox);
BOOL inside_bbox (POINT *pt, RECT *bbox);
BOOL inside_bbox (POINT *pt, SRECT *bbox);
BOOL inside_bbox_tol (POINT *pt, RECT *bbox, int tol);
BOOL bbox_overlap (RECT *bbox1, RECT *bbox2);
BOOL bbox_within_bbox (RECT *bbox1, RECT *bbox2);
BOOL is_empty (RECT *bbox);
void make_empty (RECT *bbox);
void make_empty (SRECT *bbox);
BOOL bbox_intersect(RECT *bbox1, RECT *bbox2, RECT *result);
RECT SRECTToRECT(SRECT &);

#endif // __BBOX_H__
