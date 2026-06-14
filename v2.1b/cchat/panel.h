//#include "stdafx.h"

class CDamage : public CObject
{		// Acts as a hint to UpdateAllViews
	DECLARE_DYNAMIC(CDamage);
public:
	RECT m_g;							// damage bbox (d.m_g = damage, get it?)
};


class CEFrame : public CPanelElement
{
public:
	short m_frameType;
};


class CPanel
{
public:
	CPtrList		m_elements;
	CPtrList		m_bodies;
	unsigned int	m_seed;
	BOOL			m_hasBorder;
	CBackDrop		m_backDrop;

	CPanel();
	CPanel(const CPanel &);
	virtual ~CPanel();

	virtual void	Draw(CDC* dc, POINT *ul, RECT *rect) = 0;
	virtual void	LayoutAvatars() = 0;
	virtual BOOL	LayoutBalloons(char **, CDWordArray **, char **) = 0;
	virtual BOOL	LayoutBalloon(CBalloon *[], int nb, int index, RECT&) = 0;
	virtual RECT	GetBalloonRect() = 0;
	CBody*			FetchSpeaker(UINT uID);
	BOOL			ReplaceBody(UINT);
	virtual BOOL	AvatarInPanel(UINT avID);
	virtual CPanel*	Clone() = 0;

//	CPanel& operator=(const CPanel&);
};

class CUnitPanel : public CPanel
{
public:
	static CPen		m_borderPen;
	static int		m_borderWidth;

	CUnitPanel() {}
	CUnitPanel(const CUnitPanel &p) : CPanel(p) {}

	void			Draw(CDC* dc, POINT *ul, RECT *rect);
	void			DrawBorder(CDC *dc, RECT *rect);
	void			LayoutAvatars();
	virtual BOOL	LayoutBalloons(char **, CDWordArray **, char **);
	virtual BOOL	LayoutBalloon(CBalloon *[], int nb, int index, RECT&);
	void			RearrangeBalloons(CBalloon *balloons[], int nb, RECT& freeRect); 
	RECT			GetBalloonRect();
	virtual CPanel*	Clone()
						{ return (new CUnitPanel(*this)); }
	void			GetCloudEstimate(CBalloon *balloons[], int nb, int index, RECT& freeRect, RECT& brect);
	BOOL			IsSpeaker(CBody *);
	void			AdjustArtToCoord(int fixedY, double zoomFactor);
};

class CPage
{
public:
	short			m_pageType;
	RECT			m_boundary;
	CPtrList		m_panels;
	RECT			m_bbox;
	BOOL			m_newPanel;

	CPage() { m_newPanel = TRUE; }
	virtual ~CPage();

	CPanel*			RemoveLastPanel();
	virtual BOOL	AddPanel(CPanel *) = 0;
	virtual BOOL	AddLine(UINT, const char *, BYTE, CDWordArray *, const char *szURLStart = NULL) = 0;  // void * is really CUserInfo *
	virtual void	RefreshLastPanel() = 0;
	virtual void	RefreshPanelN(int n) = 0;
	virtual void	AddTitle(const char *) = 0;
	virtual void	UpdateTitle() = 0;
	virtual void	ShowInfo(USHORT avID, const char *szInfo) = 0;
	virtual void	GetBBox(RECT *) = 0;
	virtual void	Draw(CPageView *, CDC*, POINT *, RECT *) = 0;
	virtual void	StartNewPanel() { m_newPanel = TRUE; }
	virtual void	PreparePrintDC(CDC *, CPrintInfo *, int pageNum) = 0;
	virtual int		GetPhysicalPageCount(CPrintInfo *) = 0;
};

class CUnitPanelPage : public CPage
{
public:
	static int			m_panelsPerRow;
	static int			m_panelsPerColumn;
	static int			m_printPanelsPerRow;
	static int			m_unitWidth;
	static int			m_unitHeight;
	static int			m_hInterstice;
	static int			m_vInterstice;
	static CFont*		m_fontTextBig;
	static CFont*		m_fontBalloon;
	static CFont*		m_fontWhisper;
	static CFont*		m_fontMini;
	static CFont*		m_fontTitle;
	static CFont*		m_fontShout;
	static CFontInfo*	m_fiTextBig;
	static CFontInfo*	m_fiWNormal;
	static CFontInfo*	m_fiWWhisper;
	static CFontInfo*	m_fiMini;
	static CFontInfo*	m_fiTitle;
	static CFontInfo*	m_fiShout;
	static CPtrList		m_fonts;
	static CPtrList		m_fontInfos;

	int					m_topY;
	int					m_leftX;
	CChatDoc*			m_doc;
	
	CUnitPanelPage (CChatDoc *doc) 
						{ m_doc = doc; }

	virtual BOOL		AddPanel(CPanel *);
	virtual BOOL		AddLine(UINT, const char *, BYTE, CDWordArray *, const char *szURLStart = NULL);
	virtual void		RefreshLastPanel();
	virtual void		RefreshPanelN(int n);
	virtual void		AddTitle(const char *);
	virtual void		UpdateTitle();
	virtual void		ShowInfo(USHORT avID, const char *szInfo);
	virtual void		AddStars(CUnitPanel *panel, int yTop);
	virtual void		GetBBox(RECT *);
	virtual void		Draw(CPageView *, CDC*, POINT *, RECT *);
	virtual void		PreparePrintDC(CDC *, CPrintInfo *, int pageNum);
	virtual int			GetPhysicalPageCount(CPrintInfo *);
	BOOL				AddReaction(UINT uID);
	CBalloon*			MakeBalloon(const char *szMesg, BYTE byteMode, CDWordArray *prgdwFormatting = NULL, const char *szURLStart = NULL);
	void				PageSizeInPanels(CPrintInfo *, int &width, int &height);
	static CSize		GetScrollPage();
	static int			GetUnitPanelWidth()  { return m_unitWidth; }
	static int			GetUnitPanelHeight() { return m_unitHeight; }
	static int			GetUnitPanelsPerRow() { return m_panelsPerRow; }
	static void			SetUnitPanelWidth(int width) { m_unitWidth = width; UpdateTitleFonts(); }
	static void			SetUnitPanelHeight(int height) { m_unitHeight = height; }
	static void			SetUnitPanelsPerRow(int n) { m_panelsPerRow = n; }
	static void			SetPrintUnitPanelsPerRow(int n) { m_printPanelsPerRow = n; }
	static BOOL			SetFonts(LOGFONT &logFont, COLORREF crTextColor);
	static BOOL			UpdateTitleFonts();
	static void			DestroyFonts();
};

#define MINUNITPANELWIDTH	2300
#define MINUNITPANELHEIGHT	MINUNITPANELWIDTH

