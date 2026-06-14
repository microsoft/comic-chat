#include "avbfile.h"

class CChatBackdrop
{
public:
	CChatBackdrop()
		{ m_pDIB = NULL; m_pszOrigURL = m_pszNewURL = m_pszCopyright = NULL; }
	virtual ~CChatBackdrop()
		{ if (m_pDIB) delete m_pDIB; free (m_pszOrigURL); free (m_pszNewURL); free (m_pszCopyright); }
	static CChatBackdrop* LoadBackdrop(CAvatarStream * pStream);
	CAvatarDIB* GetDrawing()
		{ return this != NULL ? m_pDIB : NULL; }

	const char * Url()
		{ return m_pszNewURL ? m_pszNewURL : m_pszOrigURL; }
	const char * Copyright() 
		{ return m_pszCopyright; }
	virtual BOOL Load(CAvatarStream * pStream);
	BOOL LoadFromBmp(CAvatarStream * pStream);
	
	CAvatarDIB* m_pDIB;
	LPSTR		m_pszOrigURL;
	LPSTR		m_pszNewURL;
	LPSTR 		m_pszCopyright;
};

class CBackDropArt {
public:
	CChatBackdrop * m_backdrop;
	SRECT m_worldCoords;

	CBackDropArt() { m_backdrop = NULL; }
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


// List of backdrop filetypes. To get the search mask use the BACKDROPTYPE_SEARCHMASK macro.
// To get the actual file extension (with the .) use the BACKDROPTYPE_FILEEXT macro.
extern char * pszBackdropTypes[];
extern const int NUMBACKDROPTYPES;
#define BACKDROPTYPE_SEARCHMASK(nType) (pszBackdropTypes[nType])
#define BACKDROPTYPE_FILEEXT(nType) (pszBackdropTypes[nType] + 2)
const char * GetBackDropNameFromID(UINT nID);
const char * GetBackDropURLFromID(UINT nID);
int GetCurrentBackDropID();
