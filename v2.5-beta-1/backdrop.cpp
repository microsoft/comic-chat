#include "stdafx.h"

#include "chat.h"
#include "dib.h"
#include "bbox.h"
#include "pe.h"
#include "backdrop.h"
#include "vector2d.h"
#include "userinfo.h"
#include "ui.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "histent.h"

#include <io.h>

// List of backdrop filetypes. To get the search mask use the BACKDROPTYPE_SEARCHMASK macro.
// To get the actual file extension (with the .) use the BACKDROPTYPE_FILEEXT macro.
char * pszBackdropTypes[] = { 
	"\\*.bmp",
	"\\*.bgb",
};
const int NUMBACKDROPTYPES = (sizeof(pszBackdropTypes) / sizeof(pszBackdropTypes[0]));

extern CChatApp theApp;

typedef struct {
	char *filename;
	char *pszRealName;
	char *pszURL;
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

int GetAllBackDropNames() {
// build file search strings
    CString strPattern;
	struct _finddata_t fd;
	long hFind;

    strFiles.RemoveAll();

	// Enumerate files of all supported types.

	int iType;
	for (iType = 0; iType < NUMBACKDROPTYPES; iType++) {
		strPattern = theApp.GetBackDropDir();
		strPattern += BACKDROPTYPE_SEARCHMASK(iType);
		hFind = _findfirst( (char *) (const char *) strPattern, &fd );
		if( hFind != -1L ) {
			do {
				if (fd.attrib != _A_SUBDIR) {
					// We are now adding the whole filename - SSN 2/4/98
					strFiles.Add(fd.name);
				   #if 0
					char szExt[_MAX_EXT];
					char szFName[_MAX_FNAME];
					_splitpath( fd.name, NULL, NULL, szFName, szExt );
					strFiles.Add(szFName);
				   #endif
				}
			} while( _findnext( hFind, &fd ) != -1 );
			_findclose (hFind);
		}
	}

	return (strFiles.GetUpperBound()+1);  // return number found
}


// backname must be a real backdrop file!
// pszRealName can be the actual backdrop requested - one we don't have.
int SetBackDropAux(const char *backname, const char * pszRealName) {

	BDFileRec *newRec = (BDFileRec *) malloc (sizeof (BDFileRec));
	newRec->filename = strdup(backname);
	newRec->pszRealName = pszRealName ? strdup(pszRealName) : NULL;
	newRec->pszURL = NULL; 	// Not set until the file is loaded
	newRec->xdim = newRec->ydim = 315;
	newRec->worldLeft = newRec->worldTop = 0;
	newRec->worldRight = 4860;
	newRec->worldBottom = -4860;
	newRec->normHeight = 100;
	newRec->backID = backRecS.Add(newRec);
	return newRec->backID;
}

//int CBackDrop::m_defaultID = 0;

void SetBackDrop(const char *backname, const char * backURL) {
	const char * pszRealBackName = NULL;
	int backIndex = -1;
	int upper = backRecS.GetUpperBound();
	for (int i = 1; i <= upper; i++) {
		BDFileRec *rec = (BDFileRec *) backRecS[i];
		if (stricmp(backname, rec->pszRealName ? rec->pszRealName : rec->filename) == 0) {
			backIndex = i;
			break;
		}
	}

	CString strBackNameOverride;

	if (backIndex == -1) {
		CString filename;
		// The backname name may now contain the filetype too.	SSN 2/5/98
		// If it doesn't look, look for each type.
		BOOL bExists = FALSE;
		if (!OurMbsChr (backname, '.'))
		{
			for (int iType = 0; iType < NUMBACKDROPTYPES; iType++)
			{
				filename.Format("%s\\%s%s", theApp.GetBackDropDir(), backname, BACKDROPTYPE_FILEEXT(iType));
				if (GetFileAttributes (filename) != (DWORD)-1L)
				{
					bExists = TRUE;
					strBackNameOverride.Format ("%s%s", backname, BACKDROPTYPE_FILEEXT(iType));
					backname = strBackNameOverride;
					break;
				}
			}
		}
		else
		{
			filename.Format("%s\\%s", theApp.GetBackDropDir(), backname);
			bExists = GetFileAttributes (filename) != (DWORD)-1L;
		}

		if (!bExists) {
			// If we have been given a URL, start downloading it now. We'll change
			// the backdrop when it arrives.
			extern BOOL g_bCanViewUnrated;
			if (backURL && g_bCanViewUnrated && (!theApp.m_bAutoDownloadBackdrops ||
					theApp.StartDownloadingBackdrop (backname, backURL))) {
				pszRealBackName = backname;
				backname = theApp.m_lastBackDrop;
			} 
			else {
				if (GetChatDoc()->GetBackDropID() != 0)	// don't change existing bground if can't find backname
					return;
				if (GetAllBackDropNames() == 0) return; // can't change existing bground if no backgrounds!
				backname = strFiles[0];
			}
		}

		backIndex = SetBackDropAux(backname, pszRealBackName);
	}
	if (GetChatDoc()) GetChatDoc()->SetBackDropID(backIndex);

	BDFileRec * rec = (BDFileRec *)backRecS[backIndex];
	theApp.m_lastBackDrop = rec->pszRealName ? rec->pszRealName : rec->filename;
}

void InitializeBackDrops() {
	if (backRecS.GetUpperBound() <= 0) {
		backRecS.SetSize(0, 5);
		backRecS.Add(NULL);
		backRecP.SetSize(0, 5);
		backRecP.Add(NULL);
	}
}

const char *GetCurrentBackDropName() {
	return theApp.m_lastBackDrop;
}

int GetCurrentBackDropID()
{
	if (theApp.m_lastBackDrop.IsEmpty ())
		return 0;

	int upper = backRecS.GetUpperBound();
	for (int i = 1; i <= upper; i++) {
		BDFileRec *rec = (BDFileRec *) backRecS[i];
		if (stricmp(theApp.m_lastBackDrop, rec->pszRealName ? rec->pszRealName : rec->filename) == 0) {
			return i;
			break;
		}
	}

	return 0;
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
	if (m_backdrop) delete m_backdrop;
}

#define SCREENPRINT		1

// this assumes that m_poseID is also the avRec index
CBackDropArt *BackDropArtFromBackID(UINT backID, BOOL toScreen) {
	BDFileRec *rec = (SCREENPRINT || toScreen) ? (BDFileRec *) backRecS[backID] : (BDFileRec *) backRecP[backID];

	CBackDropArt *art = new CBackDropArt;
	CString buff;
	// The filename now contains the type too. // SSN 2/5/98
	buff.Format("%s\\%s", theApp.GetBackDropDir(), rec->filename);

	CAvatarFileStream stream (buff);
	art->m_backdrop = CChatBackdrop::LoadBackdrop (&stream);
	if (art->m_backdrop != NULL) {
		art->m_worldCoords.Left = rec->worldLeft;
		art->m_worldCoords.Top = rec->worldTop;
		art->m_worldCoords.Right = rec->worldRight;
		art->m_worldCoords.Bottom = rec->worldBottom;
	}
	else {
		delete art;
		art = NULL;
	}
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
			free(rec->pszRealName);
			free(rec->pszURL);
			free(rec);
		}
	}
//	theApp.m_defaultBackDropID = 0;
	backRecS.SetSize(1);  // retain NULLs
	backRecP.SetSize(1);
}

void CBackDrop::Draw(CDC *dc, RECT *panelRect, RECT *) 
{
	CBackDropArt *art = GetBackDropArtFromID(m_backID, !dc->IsPrinting());
	CDIB* drawing = art != NULL ? art->m_backdrop->GetDrawing () : NULL;
	if (drawing) 
	{
#ifdef MEMDC_NOT_STRETCHED
		POINT point;
		GetBrushOrgEx(dc->GetSafeHdc(),&point);
		int iOldMode = dc->SetStretchBltMode(STRETCHMODE);
		SetBrushOrgEx(dc->GetSafeHdc(),point.x,point.y,&point);
#endif MEMDC_NOT_STRETCHED

		int panelWidth = panelRect->right - panelRect->left;
		int panelHeight = panelRect->bottom - panelRect->top;
		int bitWidth = drawing->GetWidth();
		int bitHeight = drawing->GetHeight();
		int srcLeft = ROUND(((double)m_bbox.Left / panelWidth) * bitWidth);
		int srcTop = ROUND(((double)m_bbox.Top / panelHeight) * bitHeight);
		int srcRight = ROUND(((double)m_bbox.Right / panelWidth) * bitWidth);
		int srcBottom = ROUND(((double)m_bbox.Bottom / panelHeight) * bitHeight);
		int srcWidth = srcRight - srcLeft;
		int srcHeight = srcBottom - srcTop;
//		TRACE("Backdrop draw params: IsPrinting= %d: %d %d %d %d %d %d %d %d\n",
//			dc->IsPrinting(), panelRect->left, panelRect->top, panelWidth, panelHeight, srcLeft, srcTop, srcWidth, srcHeight);
		drawing->Draw(dc, panelRect->left, panelRect->top, panelWidth, panelHeight,
				 srcLeft, srcTop, srcWidth, srcHeight,
				 SRCCOPY);
#ifdef MEMDC_NOT_STRETCHED
		GetBrushOrgEx(dc->GetSafeHdc(),&point);
		dc->SetStretchBltMode(iOldMode);
		SetBrushOrgEx(dc->GetSafeHdc(),point.x,point.y,&point);
#endif MEMDC_NOT_STRETCHED

	} else {
		// just draw white background if we can't find the art
		dc->FillSolidRect(panelRect, RGB(255,255,255));
	}
}

// Called when a backdrop is downloaded. Finds and fixes up the entry in the 
// backdrop list.

void
NotifyDownloadedBackdrop(
LPCSTR pszBackdrop)
{
	// Two passes, one over printer array, one over screen.
	CPtrArray * pBackRecArray;
	for (int i = 0; i < 2; i++) {
		pBackRecArray = i == 1 ? &backRecP : &backRecS;
		for (int iRec = pBackRecArray->GetUpperBound (); iRec >= 0; iRec--) {
			BDFileRec * pRec = (BDFileRec *)pBackRecArray->GetAt (iRec);
			if (pRec && pRec->pszRealName && !lstrcmpi (pRec->pszRealName, pszBackdrop)) {
				// We have to replace this one. Throw out it's image.
				FlushBackDropFromID (iRec, i == 0);
				// Move the real name to the filename field, freeing the old filename.
				LPSTR pszOldFilename = (LPSTR)pRec->filename;
				pRec->filename = pRec->pszRealName;
				free (pszOldFilename);
				pRec->pszRealName = NULL;
			}
		}

	}
}

const char * 
GetBackDropNameFromID(
UINT nID)
{
	BDFileRec * pRec = (BDFileRec *)backRecS[nID];
	if (pRec == NULL) {
		return NULL;
	}

	return pRec->pszRealName != NULL ? pRec->pszRealName : pRec->filename;
}

const char * 
GetBackDropURLFromID(
UINT nID)
{
	BDFileRec * pRec = (BDFileRec *)backRecS[nID];
	if (pRec == NULL) {
		return NULL;
	}

	// If we don't have the file ourselves, we can't pass it on to others.
	if (pRec->pszRealName != NULL) {
		return NULL;
	}

	CBackDropArt * pArt = BackDropArtFromBackID (nID, TRUE);
	if (pArt == NULL || pArt->m_backdrop == NULL) {
		return NULL;
	}

	return pArt->m_backdrop->Url ();
}
