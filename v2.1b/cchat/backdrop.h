class CBackDropArt {
public:
	CDIB *m_drawing;
	SRECT m_worldCoords;

	CBackDropArt() { m_drawing = NULL; }
	virtual ~CBackDropArt();
};

#define BF_NOZOOM	1

class CBackDrop : public CPanelElement {
public:
//	static int m_defaultID;
	CBackDrop() { m_backID = 0; m_mode = 0; m_bbox.Left = 0; m_bbox.Top = 0; m_bbox.Right = 4860; m_bbox.Bottom = -4860; }
	unsigned short m_backID;		// id for backdrop dib
	UCHAR m_mode;					// various flags

//	static void SetDefaultID(int id) { m_defaultID = id; }
	void Draw(CDC *dc, RECT *panelBox, RECT *dmgBox);
	virtual void Draw(CDC* dc, POINT* ul, RECT *rect) { ASSERT(0); }		// never to be called
};