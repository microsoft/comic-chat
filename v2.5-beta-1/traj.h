class CGraphicalObj {
public:
	virtual void Draw(CDC*) = 0;
};

typedef struct {
	BOOL inDash;
	int partialDist;
	int arrayIndex;
	int *dashArray;
	int nIndices;
	POINT lastPoint;
	CDC *dc;
} DASHINFO;

class CSeg : public CGraphicalObj {
public:
	virtual POINT SegLo() = 0;
	virtual void Dash(DASHINFO &) {}
	virtual ~CSeg() {}
};

class CLine : public CSeg {
public:
	POINT m_lo;
	POINT m_hi;
	CLine(POINT& lo, POINT& hi) { m_lo = lo; m_hi = hi; }
	virtual void Draw(CDC *);
	virtual POINT SegLo() { return (m_lo); }
	virtual void Dash(DASHINFO &);
};

class CArc : public CSeg {
public:
	POINT m_lo;
	POINT m_hi;
	int m_altitude;
	CArc(POINT& lo, POINT& hi, int alt) { m_lo = lo; m_hi = hi; m_altitude = alt; }
	virtual void Draw(CDC *);
	virtual POINT SegLo() { return(m_lo); }
	virtual void Dash(DASHINFO &);
};

class CTraj : public CGraphicalObj {
public:
	CPtrList m_segs;
	BOOL m_closed;

	CTraj() { m_closed = FALSE; }
	virtual ~CTraj();
	void AddSeg(CSeg *seg) { m_segs.AddTail(seg); }
	virtual void Draw(CDC *);
	virtual void Dash(CDC *);
};
