#include "stdafx.h"

#include "dib.h"
#include "cache.h"

// ****************************************************************************
// CACHE IMPLEMENTATION

BOOL CCache::Lookup(ushort key, void *obj) {
	void *p
	if (m_map->Lookup(key, p)) {				// is in cache
		CCacheElement *dl = (CCacheElement *) p;
		Promote(dl);
		return(dl->obj);
	} else return NULL;
}

BOOL CCache::Promote(CCacheElement *dl) inline {
	RemoveFromList(dl);
	AddToTail(dl);
}

void CCache::RemoveFromList(CCacheElement *dl) inline {
	dl->prev->next = dl->next;		// remove from list
	dl->next->prev = dl->prev;
}

void CCache::AddToTail(CCacheElement *dl) inline {
	dl->m_tail->prev->next = dl;
	dl->m_tail->prev = dl;
}

void CCache::SetAt(USHORT key


// ****************************************************************************
// BACKDROP CACHE CODE

static CCache backCacheS;
static CCache bacPCacheP;

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

CBackDropArt::~CBackDropArt() {
	if (m_drawing) delete m_drawing;
}

#define SCREENPRINT		1

// this assumes that m_poseID is also the avRec index
CBackDropArt *BackDropArtFromBackID(UINT backID, BOOL toScreen) {
	BDFileRec *rec = (SCREENPRINT || toScreen) ? backRecS + backID : backRecP + backID;

	CBackDropArt *art = new CBackDropArt;
	char buff[100];
	sprintf(buff, "%s%s.bmp", BDBMPPATH, rec->filename);
	art->m_drawing = new CDIB;
	VERIFY(art->m_drawing->Load(buff));
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

	CCache *cache = (toScreen ? &backCacheS : &backCacheP);
	if (cache->Lookup(backID, p)) {
		CBackDropArt *pi = (CBackDropArt *) p;
		return pi;
	}
	TRACE("Allocating backdrop art id %d.\n", backID);

	// Nope, we need to construct the art, and register it
	CBackDropArt *art = BackDropArtFromBackID(backID, toScreen);
	cache->SetAt(backID, art);
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

	CMapWordToPtr *cache = (useScreenCache ? &backCacheS : &backCacheP);
	cache->FlushCache();
}

#if 0
	POSITION pos = map->GetStartPosition();
	while (pos) {
		  map->GetNextAssoc(pos, backID, objPtr);
		  CBackDropArt *art = (CBackDropArt *)objPtr;
		  delete art;
	}
	map->RemoveAll();	// would it be legal to do RemoveKey in the above loop?
}
#endif


void DestroyBackDropArt() {
	FlushBackDropCache(TRUE);
	FlushBackDropCache(FALSE);
}

// ****************************************************************************
// Pose cache code

static CCache avCache;
