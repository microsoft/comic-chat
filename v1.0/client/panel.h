#include "stdafx.h"

class CDamage : public CObject {		// Acts as a hint to UpdateAllViews
	DECLARE_DYNAMIC(CDamage);
public:
	RECT m_g;							// damage bbox (d.m_g = damage, get it?)
};


class CEFrame : public CPanelElement {
public:
	short m_frameType;
};


class CPanel {
public:
	CPtrList m_elements;
	CPtrList m_bodies;
	unsigned int m_seed;
	BOOL m_hasBorder;
	CBackDrop m_backDrop;

	CPanel();
	CPanel(const CPanel &);
	virtual ~CPanel();
	virtual void Draw(CDC* dc, POINT *ul, RECT *rect) = 0;
	virtual void LayoutAvatars() = 0;
	virtual BOOL LayoutBalloons(char **) = 0;
	virtual BOOL LayoutBalloon(CBalloon *[], int nb, int index, RECT&) = 0;
	virtual RECT GetBalloonRect() = 0;
	CBody *FetchSpeaker(UINT id);
	BOOL ReplaceBody(UINT);
	virtual BOOL AvatarInPanel(UINT avID);
	virtual CPanel *Clone() = 0;

//	CPanel& operator=(const CPanel&);
};

class CUnitPanel : public CPanel {
public:
	static CPen m_borderPen;
	static int m_borderWidth;

	CUnitPanel() {}
	CUnitPanel(const CUnitPanel &p) : CPanel(p) {}
	void Draw(CDC* dc, POINT *ul, RECT *rect);
	void DrawBorder(CDC *dc, RECT *rect);
	void LayoutAvatars();
	virtual BOOL LayoutBalloons(char **);
	virtual BOOL LayoutBalloon(CBalloon *[], int nb, int index, RECT&);
	void RearrangeBalloons(CBalloon *balloons[], int nb, RECT& freeRect); 
	RECT GetBalloonRect();
	virtual CPanel *Clone() { return (new CUnitPanel(*this)); }
	void GetCloudEstimate(CBalloon *balloons[], int nb, int index, RECT& freeRect, RECT& brect);
	BOOL IsSpeaker(UINT avID);
	void AdjustArtToCoord(int fixedY, double zoomFactor);
};

class CPage {
public:
	short m_pageType;
	RECT m_boundary;
	CPtrList m_panels;
	RECT m_bbox;
	BOOL m_newPanel;

	CPage() { m_newPanel = TRUE; }
	virtual ~CPage();
	virtual BOOL AddPanel(CPanel *) = 0;
	virtual BOOL AddLine(UINT, const char *, UCHAR, CPtrArray *) = 0;  // void * is really CUserInfo *
	virtual void RefreshLastPanel() = 0;
	virtual void RefreshPanelN(int n) = 0;
	CPanel *RemoveLastPanel();
	virtual void AddTitle() = 0;
	virtual void UpdateTitle() = 0;
	virtual void ShowInfo(unsigned short avID, const char *info) = 0;
	virtual void GetBBox(RECT *) = 0;
	virtual void Draw(CDC*, POINT *, RECT *) = 0;
	virtual void StartNewPanel() { m_newPanel = TRUE; }
	virtual void PreparePrintDC(CDC *, CPrintInfo *, int pageNum) = 0;
	virtual int GetPhysicalPageCount(CPrintInfo *) = 0;
};

class CUnitPanelPage : public CPage {
public:
	static int panelsPerRow;
	static int panelsPerColumn;
	static int printPanelsPerRow;
	static int unitWidth;
	static int unitHeight;
	static int hInterstice;
	static int vInterstice;
	static CFont	m_fontTextBig;
	static CFont	m_fontBalloon;
	static CFont	m_fontWhisper;
	static CFont	m_fontMini;
	static CFont	m_fontTitle;
	static CFont	m_fontShout;
	static CFontInfo *m_fiTextBig;
	static CFontInfo *m_fiWNormal;
	static CFontInfo *m_fiWWhisper;
	static CFontInfo *m_fiMini;
	static CFontInfo *m_fiTitle;
	static CFontInfo *m_fiShout;

	int m_topY;
	int m_leftX;
	
	virtual BOOL AddPanel(CPanel *);
	virtual BOOL AddLine(UINT, const char *, UCHAR, CPtrArray *);
	BOOL AddReaction(int id);
	CBalloon *MakeBalloon(const char *mesgPtr, UCHAR mode, CPtrArray *links = NULL);
	virtual void RefreshLastPanel();
	virtual void RefreshPanelN(int n);
	virtual void AddTitle();
	virtual void UpdateTitle();
	virtual void ShowInfo(unsigned short avID, const char *info);
	virtual void AddStars(CUnitPanel *panel, int yTop);
	virtual void GetBBox(RECT *);
	virtual void Draw(CDC*, POINT *, RECT *);
	virtual void PreparePrintDC(CDC *, CPrintInfo *, int pageNum);
	virtual int GetPhysicalPageCount(CPrintInfo *);
	void PageSizeInPanels(CPrintInfo *, int &width, int &height);
	static CSize GetScrollPage();
	static int GetUnitPanelWidth()  { return unitWidth; }
	static int GetUnitPanelHeight() { return unitHeight; }
	static int GetUnitPanelsPerRow() { return panelsPerRow; }
	static void SetUnitPanelWidth(int width) { unitWidth = width; UpdateTitleFonts(); }
	static void SetUnitPanelHeight(int height) { unitHeight = height; }
	static void SetUnitPanelsPerRow(int n) { panelsPerRow = n; }
	static void SetPrintUnitPanelsPerRow(int n) { printPanelsPerRow = n; }
	static BOOL InitializeFonts();
	static BOOL UpdateTitleFonts();
	static void DestroyFonts();
};

#define MINUNITPANELWIDTH	2300
#define MINUNITPANELHEIGHT	MINUNITPANELWIDTH

