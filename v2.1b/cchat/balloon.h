#ifndef __BALLOON_H__
#define __BALLOON_H__

#include "format.h"

#define FT_LEFT_JUSTIFY	1
#define MAXLINES	10

extern CChatApp theApp;

class CFormatInfo
{
public:
	UCHAR	m_nLines;
	int		m_rgiLengths[MAXLINES];
	int		m_rgiWidths[MAXLINES];
	// int	m_rgiHeights[MAXLINES];
	int		m_iMaxWidth;
	char*	m_rgszStarts[MAXLINES];
	SRECT	m_bbox;
	int		m_rgiLeftX[MAXLINES];
//	BOOL	m_bShifted;

	CFormatInfo() 
	{
//		m_bShifted = FALSE; 
		//for (int i = 0; i < MAXLINES; i++)
		//	m_rgiHeights[i] = 0;
	}
};


class CArrow
{
public:
	POINT m_lo, m_hi, m_mid; // m_lo and m_hi in balloon coords, m_mid in panel coords

	CArrow() {}
	CArrow(const CArrow &);

	virtual void Draw(CDC *dc, int x, int y, RECT *rect) = 0;
	virtual void GetPoints(SRECT *, POINT& lo, POINT& mid, POINT& hi) = 0;
	virtual CArrow *Clone() = 0;
};


class CFontInfo
{
public:
	CFont*		m_font;
	COLORREF	m_crDefaultForeColor;
	short		m_leading;
	short		m_lineHeight;
	short		m_baseAdd;
	short		m_continuationWidth;
	short		m_topOffset;

	CFontInfo(CFont *pFont, COLORREF crDefaultForeColor, short nLeading, short nBaseAdd);
};

class CLabel : public CPanelElement
{
public:
	CFontInfo*		m_fontI;
	char*			m_str;
	UCHAR			m_format;
	CDWordArray*	m_prgdwFormatting;

	CLabel(const CLabel &);

	CLabel(const char* str, CFontInfo* fontInfo, CDWordArray* prgdwFormatting = NULL)
	{ 
		m_str = strdup(str);
		m_fontI = fontInfo;
		m_prgdwFormatting = (theApp.m_flags1 & F1_RTFCOMIC) ? CopyFormatting(prgdwFormatting) : CopyLinksFormatting(prgdwFormatting);
		m_format = (char) 0;
	}
	virtual ~CLabel() 
	{
		free(m_str);
		if (m_prgdwFormatting)
		{
			m_prgdwFormatting->RemoveAll();
			delete m_prgdwFormatting;
		}
	}  // font is reffed elsewhere

	BOOL			bURLHit(int iLeftX, int iBaseY, CSize &size);
	int				iDrawFormattedTextLine(CDC *pdc, int iLeftX, int iBaseY, char *szChunkStart, int iChunkLen, WORD wFormat, BOOL *pbURLHit);
	void			DrawFormattedText(CDC *pdc, char* rgszStarts[], int rgiLeftX[], int rgiLengths[], /*int rgiHeights[],*/ int nLines, int iStartTop, int *piURL = NULL);
	int				BreakIntoLines(CFormatInfo&);
	void			Draw(CDC *dc, POINT *ul, RECT *rect);
	void			ShiftLines(CFormatInfo&);
	virtual void	GetBBox(RECT *);
	virtual int		AreaEstimate(int *, int *);
	virtual int		WidestWord();
	virtual int		GetLeading() 
						{ return m_fontI->m_leading; }
	virtual char*	SplitHeight(int iHeight, CDWordArray **pprgdwRestFormatting, char **pszURLStartInRest = NULL);
	virtual void	OnLButtonDown(POINT &) {}
};


class CBalloon : public CLabel
{
public:
	CBody*			m_speaker;						// temporarily, balloon's utterer
	CSpline*		m_spline;
	CFormatInfo*	m_fInfo;
	SRECT			m_trueBox;						// this is the balloon's true bbox, relative to balloon's origin (m_bbox is really the Label box)
	SRECT			m_routeRgn;
	CTraj*			m_traj;
	char**			m_prgszURLs;

	CBalloon(const char *szText, CFontInfo *pFontInfo, CDWordArray *m_prgdwFormatting, const char *szURLStart);
	CBalloon(const CBalloon&);
	virtual ~CBalloon();
	virtual void	Draw(CDC* pdc, POINT *ul, RECT *rect) = 0;
	virtual void	DockAtTop(int height);
	void			CreateURLArray(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart);
	BOOL			Overlap(CBalloon *);
	void			GetCloudBBox(RECT *);
	void			GetCloudBBox(SRECT *);
	virtual BOOL	ComputeInternals() = 0;
	virtual void	ComputeCloudBBox();
	virtual BOOL	SetBBox(int left, int bottom, int right, int top);
	virtual void	GetBBox(RECT *);
	virtual void	InMyCoords(SRECT *);
	virtual void	QueryRouteRgn(int OtherToX, int &leftAllowance, int &rightAllowance);
	virtual void	SetRouteRgn(int OtherToX, int left, int right);
	virtual int		GetType() { return PE_BALLOON; }
	virtual char*	SplitHeight(int iHeight, CDWordArray **pprgdwRestFormatting, char **pszURLStartInRest = NULL) = 0;
	virtual void	DrawText(CDC *dc);
	virtual void	OnLButtonDown(POINT &);
	virtual CBalloon* Clone() = 0;
};

class CBWoodringNormal : public CBalloon
{
public:
	static CPen m_pen;

	CBWoodringNormal(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart);
	virtual void Draw(CDC *dc, POINT *ul, RECT *rect);
	virtual CSpline *CreateBalloonSpline(CFormatInfo& fInfo);
	CSpline *GetBalloonSpline();
	BOOL ComputeInternals();
	virtual CBalloon *Clone() { return (new CBWoodringNormal(*this)); }
	virtual void AddArrow(CBalloon *balloon, CSpline *spline, CFormatInfo &fInfo);
	virtual void SetBalloonTraj();
	virtual char *SplitHeight(int iHeight, CDWordArray **pprgdwRestFormatting, char **pszURLStartInRest = NULL);
};

class CBWoodringWhisper : public CBWoodringNormal
{
	static CPen m_nimbusPen;
public:
	CBWoodringWhisper(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart);
	virtual void Draw(CDC *dc, POINT *ul, RECT *rect);
	virtual CBalloon *Clone() { return (new CBWoodringWhisper(*this)); }
};

class CBWoodringThink : public CBWoodringNormal
{
public:
	CBWoodringThink(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart);
	virtual void Draw(CDC *dc, POINT *ul, RECT *rect);
	virtual CBalloon *Clone() { return (new CBWoodringThink(*this)); }
	virtual void AddArrow(CBalloon *, CSpline *, CFormatInfo &) {}
};

class CBWoodringBox : public CBWoodringNormal
{
public:
	CBWoodringBox(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart);
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

#if 0
class CBWoodringMini : public CBWoodringNormal
{
public:
	CBWoodringMini(const char *szText, CDWordArray *prgdwFormatting, const char *szURLStart);
	virtual CBalloon *Clone() { return (new CBWoodringMini(*this)); }
};
// in .cpp: CBWoodringMini::CBWoodringMini(const char *str, CDWordArray *prgdwFormatting) : CBWoodringNormal(str, prgdwFormatting) { m_fontI = CUnitPanelPage::m_fiMini; }
#endif

#endif // __BALLOON_H__
