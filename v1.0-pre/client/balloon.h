#define FT_LEFT_JUSTIFY	1

class CFormatInfo {
public:
	UCHAR nLines;
	int lengths[30];
	int widths[30];
	int maxWidth;
	char *starts[30];
	SRECT m_bbox;
	int leftX[30];
	BOOL m_shifted;

	CFormatInfo() { m_shifted = FALSE; }
};


class CArrow {
public:
	POINT m_lo, m_hi, m_mid; // m_lo and m_hi in balloon coords, m_mid in panel coords
	CArrow() {}
	CArrow(const CArrow &);
	virtual void Draw(CDC *dc, int x, int y, RECT *rect) = 0;
	virtual void GetPoints(SRECT *, POINT& lo, POINT& mid, POINT& hi) = 0;
	virtual CArrow *Clone() = 0;
};


class CFontInfo {
public:
	CFont *m_font;
	short m_leading;
	short m_lineHeight;
	short m_baseAdd;

	CFontInfo(CFont *font, short leading, short baseAdd);
};

class CLabel : public CPanelElement {
public:
	CFontInfo *m_fontI;
	char *m_str;
	UCHAR m_format;
	CPtrArray *m_links;

	CLabel(const CLabel &);

	CLabel(const char *str, CFontInfo *fontInfo, CPtrArray *links = NULL) { 
		extern CPtrArray *CopyLinks(CPtrArray *);
		m_str = strdup(str);
		m_fontI = fontInfo;
		m_links = CopyLinks(links);
		m_format = (char)0;
	}
	virtual ~CLabel() { free(m_str); void DestroyLinks(CPtrArray **); if (m_links) DestroyLinks(&m_links); }  // font is reffed elsewhere
	void Draw(CDC *dc, POINT *ul, RECT *rect);
	virtual void GetBBox(RECT *);
	int BreakIntoLines(CFormatInfo&);
	void ShiftLines(CFormatInfo&);
	virtual int AreaEstimate(int *, int *);
	virtual int WidestWord();
	virtual int GetLeading() { return m_fontI->m_leading; }
	virtual char *SplitHeight(int height);
	virtual void OnLButtonDown(POINT &) {}
};


class CBalloon : public CLabel {
public:
	CBody *m_speaker;						// temporarily, balloon's utterer
	CSpline *m_spline;
	CFormatInfo *m_fInfo;
	SRECT m_trueBox;						// this is the balloon's true bbox, relative to balloon's origin (m_bbox is really the Label box)
	SRECT m_routeRgn;
	CTraj *m_traj;

	CBalloon(const char *str, CFontInfo *font, CPtrArray *m_links);
	CBalloon(const CBalloon&);
	virtual ~CBalloon();
	virtual void Draw(CDC* dc, POINT *ul, RECT *rect) = 0;
	virtual void DockAtTop(int height);
	BOOL Overlap(CBalloon *);
	void GetCloudBBox(RECT *);
	void GetCloudBBox(SRECT *);
	virtual BOOL ComputeInternals() = 0;
	virtual void ComputeCloudBBox();
	virtual BOOL SetBBox(int left, int bottom, int right, int top);
	virtual void GetBBox(RECT *);
	virtual void InMyCoords(SRECT *);
	virtual CBalloon *Clone() = 0;
	virtual void QueryRouteRgn(int OtherToX, int &leftAllowance, int &rightAllowance);
	virtual void SetRouteRgn(int OtherToX, int left, int right);
	virtual int GetType() { return PE_BALLOON; }
	virtual char *SplitHeight(int height) = 0;
	virtual void DrawText(CDC *dc);
	virtual void ForTextRuns(CDC *dc, void (*runFunc)(CDC*, CLabel *, int, int, int, int, char *, int, void *));
	virtual void OnLButtonDown(POINT &);
};

class CBWoodringNormal : public CBalloon {
public:
	static CPen m_pen;

	CBWoodringNormal(const char *str, CPtrArray *links);
	virtual void Draw(CDC *dc, POINT *ul, RECT *rect);
	virtual CSpline *CreateBalloonSpline(CFormatInfo& fInfo);
	CSpline *GetBalloonSpline();
	BOOL ComputeInternals();
	virtual CBalloon *Clone() { return (new CBWoodringNormal(*this)); }
	virtual void AddArrow(CBalloon *balloon, CSpline *spline, CFormatInfo &fInfo);
	virtual void SetBalloonTraj();
	virtual char *SplitHeight(int height);
};

class CBWoodringWhisper : public CBWoodringNormal {
	static CPen m_nimbusPen;
public:
	CBWoodringWhisper(const char *str, CPtrArray *links);
	virtual void Draw(CDC *dc, POINT *ul, RECT *rect);
	virtual CBalloon *Clone() { return (new CBWoodringWhisper(*this)); }
};

class CBWoodringThink : public CBWoodringNormal {
public:
	CBWoodringThink(const char *str, CPtrArray *links);
	virtual void Draw(CDC *dc, POINT *ul, RECT *rect);
	virtual CBalloon *Clone() { return (new CBWoodringThink(*this)); }
	virtual void AddArrow(CBalloon *, CSpline *, CFormatInfo &) {}
};

class CBWoodringMini : public CBWoodringNormal {
public:
	CBWoodringMini(const char *str, CPtrArray *links);
	virtual CBalloon *Clone() { return (new CBWoodringMini(*this)); }
};

class CBWoodringBox : public CBWoodringNormal {
public:
	CBWoodringBox(const char *str, CPtrArray *links);
	virtual CSpline *CreateBalloonSpline(CFormatInfo& fInfo) { return NULL; }
	virtual void AddArrow(CBalloon *balloon, CSpline *spline, CFormatInfo &fInfo) {}
	virtual void SetBalloonTraj();
	virtual void ComputeCloudBBox();
	virtual CBalloon *Clone() { return (new CBWoodringBox(*this)); }
	virtual void GetBBox(RECT *);
	virtual void QueryRouteRgn(int OtherToX, int &leftAllowance, int &rightAllowance); // effectively a no-op
	virtual void SetRouteRgn(int OtherToX, int left, int right) {} // no-op
	virtual int GetType() { return (PE_BALLOON | PE_BOX); }
};

