typedef double MATRIX[4][4];

class CSpline : public CSeg {
public:
	BOOL closed;
	MATRIX *matrix;
	POINT *bezpts;
	int nCps;
	POINT *cps;

	CSpline(POINT cpArray[], int n, BOOL isClosed);
	CSpline(const CSpline &);
	virtual ~CSpline();
	virtual int GetDups() = 0;	// number of duplicated control points needed to close
	void ComputeBezpts();
	virtual int KnotCount() = 0;
	int BezierCount() { return ((3 * KnotCount()) - 8); }
	POINT GetKnot(int index);
	void CvertsToCubic(POINT&, POINT&, POINT&, POINT&, POINT&, POINT&, POINT&, POINT&);
	void CubicToBezier(POINT&, POINT&, POINT&, POINT&, POINT&, POINT&, POINT&, POINT&);
	POINT ClosestPoint(POINT&, int *bezIndex);
	POINT WalkHorizontalDistance(POINT& fromPt, int fromKnotIndex, int goalX, int& foundKnotIndex);
	void Draw(CDC *);
	void Dash(DASHINFO &);
	virtual POINT SegLo();
	virtual CSpline *Clone() = 0;
};

class CCardinal : public CSpline {
	static double defaultTension;
	
public:
	double tension;

	CCardinal(const CCardinal &);
	CCardinal(POINT cpArray[], int n, BOOL isClosed);
	void SetMatrix(double tension);
	virtual int GetDups() { return 2; }
	virtual int KnotCount() { return (closed ? nCps + 3 : nCps + 2); }
	virtual CSpline *Clone() { return (new CCardinal(*this)); }
};

class CBeta : public CSpline {
	static double defaultTension;
	static double defaultBias;

public:
	double tension;
	double bias;

	CBeta(const CBeta &);
	CBeta(POINT cpArray[], int n, BOOL isClosed);
	void SetMatrix(double tension, double bias);
	virtual int GetDups() { return 3; }
	virtual int KnotCount() { return (closed ? nCps + 3 : nCps + 4); }
	virtual CSpline *Clone() { return (new CBeta(*this)); }
};
