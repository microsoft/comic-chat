/* Basic Types */

typedef struct {		/* 2D Point */
	double x, y;
	} DPOINT;

typedef struct {		/* bounding box */
	double xmin;
	double xmax;
	double ymin;
	double ymax;
	} BOUNDBOX;


typedef struct {		/* BEZIER */
	DPOINT p0;
	DPOINT p1;
	DPOINT p2;
	DPOINT p3;
	} BEZIER;

#define TRUE	1
#define FALSE	0

#define LARGENUMBER	1.e24
#define SMALLNUMBER	1.e-24
#define LARGEINTEGER 100000000
#define LARGESHORT	31000

/* Commonly used MACROS */
#define MAX(a,b) ((a) > (b) ? (a) : (b))
#define MIN(a,b) ((a) < (b) ? (a) : (b))
#define ABS(a) ((a) >= 0 ? (a) : -(a))
#define ROUND(fp) ((int)(fp + 0.5))

#define PI 3.14159

DPOINT point_add(DPOINT, DPOINT);
DPOINT point_sub(DPOINT, DPOINT);
DPOINT point_scalmult(double, DPOINT);
double point_dist(DPOINT, DPOINT);
double point_magn(DPOINT);
DPOINT point_norm(DPOINT);
DPOINT point_to_dpoint(POINT);

POINT point_add(POINT, POINT);
POINT point_sub(POINT, POINT);
POINT point_scalmult(double, POINT);
double point_dist(POINT, POINT);
double point_magn(POINT);
POINT point_norm(POINT pt);
int manhattan_dist(POINT, POINT);
POINT dpoint_to_point(DPOINT);

double angle_between_vecs(DPOINT, DPOINT);
double vector_to_angle (DPOINT);
double vector_to_angle (POINT);
DPOINT angle_to_vector (double angle);
double subtract_angles (double angle1, double angle2);
