#include "stdafx.h"

#include "chat.h"
#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "backdrop.h"
#include "vector2d.h"
#include "userinfo.h"
#include "histent.h"
#include <io.h>

extern CChatApp theApp;

typedef struct {
	char *filename;
	unsigned short backID;
	short xdim;
	short ydim;
	short worldLeft;
	short worldTop;
	short worldRight;
	short worldBottom;
	short normHeight;
} BDFileRec;

CPtrArray backRecS;
CPtrArray backRecP;

static CStringArray strFiles;

void GetAllBackDropNames() {
// build file search strings
    CString strPattern;
	struct _finddata_t fd;
	long hFind;

    strFiles.RemoveAll();
	strPattern = theApp.GetBackDropDir();
	strPattern += "\\*.bmp";
	hFind = _findfirst( (char *) (const char *) strPattern, &fd );
	if( hFind != -1L )
	{
		do 
		{
			if (fd.attrib != _A_SUBDIR)
			{
				char szExt[_MAX_EXT];
				char szFName[_MAX_FNAME];
				
				_splitpath( fd.name, NULL, NULL, szFName, szExt );
				strFiles.Add(szFName);
		   }

		} while( _findnext( hFind, &fd ) != -1 );
	}
}


int SetBackDropAux(const char *backname) {  // backname must be a real backdrop file!
	BDFileRec *newRec = (BDFileRec *) malloc (sizeof (BDFileRec));
	newRec->filename = strdup(backname);
	newRec->xdim = newRec->ydim = 315;
	newRec->worldLeft = newRec->worldTop = 0;
	newRec->worldRight = 4860;
	newRec->worldBottom = -4860;
	newRec->normHeight = 100;
	newRec->backID = backRecS.Add(newRec);
	return newRec->backID;
}

int CBackDrop::m_defaultID = 0;

void SetBackDrop(const char *backname) {
	int backIndex = -1;
	int upper = backRecS.GetUpperBound();
	for (int i = 1; i <= upper; i++) {
		BDFileRec *rec = (BDFileRec *) backRecS[i];
		if (stricmp(backname, rec->filename) == 0) {
			backIndex = i;
			break;
		}
	}

	if (backIndex == -1) {
		CString filename;
		FILE *fp;
		filename.Format("%s\\%s.bmp", theApp.GetBackDropDir(), backname);
		if (fp = fopen(filename, "r")) {
			fclose(fp);		// the file is there
		} else {
			GetAllBackDropNames();
			backname = strFiles[0];
		}
		backIndex = SetBackDropAux(backname);
	}
	CBackDrop::SetDefaultID(backIndex);
	theApp.m_lastBackDrop = ((BDFileRec *) backRecS[backIndex])->filename;
}

void InitializeBackDrops() {
	backRecS.SetSize(0, 5);
	backRecS.Add(NULL);
	backRecP.SetSize(0, 5);
	backRecP.Add(NULL);
	void AddAndExecute(HistoryEntry *);
	AddAndExecute(new ChangeBackDropEntry((const char *) theApp.m_lastBackDrop));
}

const char *GetCurrentBackDropName() {
	return theApp.m_lastBackDrop;
}

#if 0
BDFileRec backRecS[] = {
	{NULL, 0},
	{"room8BS", 1, 315, 315, 0, 0, 4860, -4860, 100},
	{"room2a", 2, 315, 315, 0, 0, 4860, -4860, 100},
	{"pastor1a", 3, 315, 315, 0, 0, 4860, -4860, 100},
	{"pastor2a", 4, 315, 315, 0, 0, 4860, -4860, 100},
	{"black", 5, 315, 315, 0, 0, 4860, -4860, 100},
	{"fant1", 6, 315, 315, 0, 0, 4860, -4860, 100},
	{"fant2", 7, 315, 315, 0, 0, 4860, -4860, 100},
	{"fant3", 8, 315, 315, 0, 0, 4860, -4860, 100},
	{"ohio1", 9, 315, 315, 0, 0, 4860, -4860, 100},
	{"pastor3a", 10, 315, 315, 0, 0, 4860, -4860, 100},
	{"roomwll2", 11, 315, 315, 0, 0, 4860, -4860, 100},
	{"roomban2", 12, 315, 315, 0, 0, 4860, -4860, 100},
	{"roombana", 13, 315, 315, 0, 0, 4860, -4860, 100},
};

BDFileRec backRecP[] = {
	{NULL, 0},
	{"roomXB", 1, 4261, 4261, 0, 0, 4860, -4860, 100},
	{"room2b", 2, 2025, 2025, 0, 0, 4860, -4860, 100},
	{"pastor1b", 3, 5269, 5269, 0, 0, 4860, -4860, 100},
	{"pastor2b", 4, 3578, 3578, 0, 0, 4860, -4860, 100},
	{"black", 5, 315, 315, 0, 0, 4860, -4860, 100},
	{"fant1", 6, 315, 315, 0, 0, 4860, -4860, 100},
	{"fant2", 7, 315, 315, 0, 0, 4860, -4860, 100},
	{"fant3", 8, 315, 315, 0, 0, 4860, -4860, 100},
	{"ohio1", 9, 315, 315, 0, 0, 4860, -4860, 100},
	{"pastor3b", 10, 3661, 3661, 0, 0, 4860, -4860, 100},
	{"roomwll1", 11, 4537, 4537, 0, 0, 4860, -4860, 100},
	{"roomban1", 12, 4537, 4537, 0, 0, 4860, -4860, 100},
	{"roombanb", 13, 2143, 2143, 0, 0, 4860, -4860, 100},
};
#endif

CMapWordToPtr backMapS(10);		// maps backID to backDropArt (screen)
CMapWordToPtr backMapP(10);		// maps backID to backDropArt (printer)

CBackDropArt::~CBackDropArt() {
	if (m_drawing) delete m_drawing;
}

#define SCREENPRINT		1

// this assumes that m_poseID is also the avRec index
CBackDropArt *BackDropArtFromBackID(UINT backID, BOOL toScreen) {
	BDFileRec *rec = (SCREENPRINT || toScreen) ? (BDFileRec *) backRecS[backID] : (BDFileRec *) backRecP[backID];

	CBackDropArt *art = new CBackDropArt;
	CString buff;
	buff.Format("%s\\%s.bmp", theApp.GetBackDropDir(), rec->filename);
	art->m_drawing = new CDIB;
	VERIFY(art->m_drawing->Load((LPCTSTR) buff));
	art->m_worldCoords.Left = rec->worldLeft;
	art->m_worldCoords.Top = rec->worldTop;
	art->m_worldCoords.Right = rec->worldRight;
	art->m_worldCoords.Bottom = rec->worldBottom;
	return art;
}

// Almost identical to GetPoseFromID()

CBackDropArt *GetBackDropArtFromID(unsigned short backID, BOOL toScreen) {
	void *p;

	if (!backID) return NULL;				// No backDrop for ID 0

	CMapWordToPtr *map = (toScreen ? &backMapS : &backMapP);
	if (map->Lookup(backID, p)) {		// is it already there?
		CBackDropArt *pi = (CBackDropArt *) p;
		return pi;
	}
	TRACE("Allocating backdrop art id %d.\n", backID);

	// Nope, we need to construct the art, and register it
	CBackDropArt *art = BackDropArtFromBackID(backID, toScreen);
	map->SetAt(backID, art);
	return art;
}

#if 0   // to be used?
// Almost identical to FlushPoseFromID
void FlushBackDropFromID(unsigned short backID, BOOL toScreen = TRUE) {
	void *p;

	CMapWordToPtr *map = (toScreen ? &backMapS : &backMapP);
	if (map->Lookup(backID, p)) {		// is it really there?
		map->RemoveKey(backID);
		CBackDropArt *pi = (CBackDropArt *) p;
		delete pi;
	}
}
#endif

void FlushBackDropCache(BOOL useScreenCache = TRUE) {
	void *objPtr;
	unsigned short backID;

	CMapWordToPtr *map = (useScreenCache ? &backMapS : &backMapP);
	POSITION pos = map->GetStartPosition();
	while (pos) {
		  map->GetNextAssoc(pos, backID, objPtr);
		  CBackDropArt *art = (CBackDropArt *)objPtr;
		  delete art;
	}
	map->RemoveAll();	// would it be legal to do RemoveKey in the above loop?
}


void DestroyBackDropArt() {
	FlushBackDropCache(TRUE);
	FlushBackDropCache(FALSE);
	int upper = backRecS.GetUpperBound();
	for (int i = 1; i <= upper; i++) {
		BDFileRec *rec = (BDFileRec *) backRecS[i];
		if (rec) {
			free(rec->filename);
			delete rec;
		}
	}
	backRecS.SetSize(1);  // retain NULLs
	backRecP.SetSize(1);
}

void CBackDrop::Draw(CDC *dc, RECT *panelRect, RECT *) {
	CBackDropArt *art = GetBackDropArtFromID(m_backID, !dc->IsPrinting());
	if (art) {
		int panelWidth = panelRect->right - panelRect->left;
		int panelHeight = panelRect->bottom - panelRect->top;
		int bitWidth = art->m_drawing->GetWidth();
		int bitHeight = art->m_drawing->GetHeight();
		int srcLeft = ROUND(((double)m_bbox.Left / panelWidth) * bitWidth);
		int srcTop = ROUND(((double)m_bbox.Top / panelHeight) * bitHeight);
		int srcRight = ROUND(((double)m_bbox.Right / panelWidth) * bitWidth);
		int srcBottom = ROUND(((double)m_bbox.Bottom / panelHeight) * bitHeight);
		int srcWidth = srcRight - srcLeft;
		int srcHeight = srcBottom - srcTop;
//		TRACE("Backdrop draw params: IsPrinting= %d: %d %d %d %d %d %d %d %d\n",
//			dc->IsPrinting(), panelRect->left, panelRect->top, panelWidth, panelHeight, srcLeft, srcTop, srcWidth, srcHeight);
		art->m_drawing->Draw(dc, panelRect->left, panelRect->top, panelWidth, panelHeight,
							 srcLeft, srcTop, srcWidth, srcHeight,
							 SRCCOPY);
	} else {
		// just draw white background if we can't find the art
		dc->FillSolidRect(panelRect, RGB(255,255,255));
	}
}




