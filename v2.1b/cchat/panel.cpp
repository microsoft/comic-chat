#include "stdafx.h"
#include "chat.h"
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "traj.h"
#include "spline.h"
#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "balloon.h"
#include "backdrop.h"
#include "pageview.h"
#include "panel.h"
#include "ui.h"
#include "vector2d.h"
#include "format.h"
#include <stdlib.h>
#include <math.h>

extern CChatApp theApp;
extern BOOL		g_bNewedPanel;
extern CBody	*GetBodyCamBody();
extern BOOL		printBMP;

IMPLEMENT_DYNAMIC(CDamage, CObject);

#define INFOMARGIN			10
#define MAXINFOTEXTHEIGHT	.5		// as a percentage of pane
#define YOFFSET				300
#define DELTA				320
#define BR_SPEAKER			0
#define BR_IMPORTANT		1
#define BR_GOODIDEA			2
#define BR_OK				3
#define MAXBDYPERFRAME		20
#define ONELINETHRESHOLD	500
#define MINHOOKHEIGHT		100
#define ICONSIZE			500
#define ICONSPACE			100
#define BELOWSTARRING		300
#define ROWHEIGHT			500
#define bZoomIn				TRUE					// FALSE for SIG PIX

COLORREF	clrs[] = { RGB(80, 80, 80), RGB(130, 130, 130), RGB(160, 160, 160), RGB(240, 240, 240), RGB(255, 255, 255)};
CDC*		pnlDC = NULL;		// For debugging purposes only

double	randfloat();
BOOL	bbox_overlap (RECT *bbox1, RECT *bbox2);
BOOL	Establishing();
void	PrintBMP();

int		CUnitPanelPage::m_panelsPerRow		= 2;
int		CUnitPanelPage::m_printPanelsPerRow	= 0;  // set in CPage::PreparePrintDC
int		CUnitPanelPage::m_panelsPerColumn	= -1; // negative value means no limit (page never ends)
int		CUnitPanelPage::m_unitWidth			= MINUNITPANELWIDTH-1;  // triggers a resize if not overridden
int		CUnitPanelPage::m_unitHeight		= MINUNITPANELHEIGHT-1;
int		CUnitPanelPage::m_hInterstice		= 144;
int		CUnitPanelPage::m_vInterstice		= 144;

int		CUnitPanel::m_borderWidth			= 60;	  // I hope these initialize in order!
CPen	CUnitPanel::m_borderPen(PS_SOLID, 2 * CUnitPanel::m_borderWidth, RGB(0,0,0));

class CBodyRecord
{
public:
	CBody*		m_body;
	CPtrList	m_lookAts;
	UCHAR		m_priority;
};


void DrawPoint(CDC *dc, POINT p, int delta)
{
	// Draw a simple plus sign at p
	dc->MoveTo(p.x, p.y - delta);
	dc->LineTo(p.x, p.y + delta);
	dc->MoveTo(p.x - delta, p.y);
	dc->LineTo(p.x + delta, p.y);
}


void DrawSpline(CDC *dc, RECT *rect)
{
	CPoint pt1(rect->left + DELTA, rect->bottom + DELTA);
	CPoint pt2(pt1.x, pt1.y + YOFFSET);
	CPoint pt3(rect->left + 2*DELTA, rect->bottom + 2*DELTA);
	CPoint pt4(pt3.x, pt3.y + YOFFSET);
	CPoint pt5(rect->left + 3*DELTA, rect->bottom + 3*DELTA);
	CPoint pt6(pt5.x, pt5.y + YOFFSET);

 #if 0
	POINT pts[6];
	pts[0] = pt1;
	pts[1] = pt2;
	pts[2] = pt3;
	pts[3] = pt4;
	pts[4] = pt5;
	pts[5] = pt6;

	for (int i = 0; i < 6; i++) {
		 DrawPoint(dc, pts[i]);
	}
	CBeta card(pts, 6);
	dc->PolyBezier(card.bezpts, card.BezierCount());
#endif
}


void DrawRoutes(CDC *dc, CPanel *panel)
{
	int i = 0;
	POSITION pos = panel->m_elements.GetTailPosition();
	while (pos != NULL)
	{
		CBalloon *b = (CBalloon *) panel->m_elements.GetPrev(pos);
		RECT r;
		r.left = b->m_routeRgn.Left;
		r.right = b->m_routeRgn.Right;
		r.top = 0;
		r.bottom = -CUnitPanelPage::m_unitHeight;
		dc->FillSolidRect(&r, clrs[i]);
		i = (i+1) % sizeof(clrs);
	}
}


void RandEmotion(CEmotion &e)
{
	e.m_intensity = (float) randfloat();
	e.m_emotion = (float)((randfloat() * 2.0 - 1.0) * PI);
}


GetIndex(const CPtrList &list, void *member)
{
	int index = 0;
	POSITION pos = list.GetHeadPosition();
	while (pos)
	{
		void *foo = list.GetNext(pos);
		if (member == foo)
			return index;
		index++;
	}
	return -1;
}


void ForceFitBalloon(CBalloon *pBalloon, RECT &rcFreeRect, char **pszRest, CDWordArray **pprgdwRestFormatting, char **pszURLStartInRest)
{
	ASSERT(pBalloon);
	ASSERT(pszURLStartInRest);
	ASSERT(pszRest);
	ASSERT(pprgdwRestFormatting);

	pBalloon->SetBBox(rcFreeRect.left, rcFreeRect.bottom, rcFreeRect.right, rcFreeRect.top);
	*pszRest = pBalloon->SplitHeight(rcFreeRect.top - rcFreeRect.bottom, pprgdwRestFormatting, pszURLStartInRest);  // need a fudge factor?
	if (pBalloon->m_bbox.Top > -250)
		pBalloon->DockAtTop(rcFreeRect.top);
}


BOOL GetInterveningBBox(CBalloon *balloons[], int index, RECT &freeRect, RECT &irect) {
	RECT cloudbox;
	int mostLeft, mostRight, leftAllowance, rightAllowance, delta;
	void Dock(RECT&);

	int toPtX = balloons[index]->m_speaker->m_arrowX;
	// find region between all routeRgns, above toPtX
	mostLeft = freeRect.left;
	mostRight = freeRect.right;
	for (int i = 0; i < index; i++) {
		balloons[i]->QueryRouteRgn(toPtX, leftAllowance, rightAllowance);
		mostLeft = max(leftAllowance, mostLeft);
		mostRight = min(rightAllowance, mostRight);
	}
	if (mostLeft > irect.left || mostRight < irect.right) {  // irect can't be placed as is
		int potentialClearance = mostRight - mostLeft;
		if (potentialClearance >= (irect.right - irect.left)) {		// we need only shift it
			if (mostLeft > irect.left)
				delta = mostLeft - irect.left;
			else delta = mostRight - irect.right;
			irect.left += delta;
			irect.right += delta;
		} else {											// grab maximal clearance
			irect.left = mostLeft;
			irect.right = mostRight;
		}
	}

	// irect.top must be no higher than bottom of any balloon to its right, and no higher than
	// bottom of any balloon to its right\and no higher than the top of any balloon
	// to its left.
	irect.top = freeRect.top;
	for (i = 0; i < index; i++) {
		balloons[i]->GetCloudBBox(&cloudbox);
		if (cloudbox.right < irect.left) {		// cloud is to the right
			irect.top = min(irect.top, cloudbox.top);
		} else {
			Dock(cloudbox); // dock the cloudbox so that irect's top will be higher
			irect.top = min(irect.top, cloudbox.bottom);
		}
	}

	return TRUE;
}


int LowestPreviousBottom(CBalloon *balloons[], int index, int lowY)
{
	for (int i = 0; i < index; i++)
		lowY = min(lowY, balloons[i]->m_bbox.Bottom);
	return lowY;
}


BOOL NoneToLeft(CBalloon *balloons[], int nb, int index)
{
	if (index >= nb-1)
		return TRUE;
	
	int thisToLeft = balloons[index]->m_speaker->m_bbox.Left;
	
	for (int i = index+1; i < nb; i++)
	{
		if (balloons[i]->m_speaker->m_bbox.Left < thisToLeft)
			return FALSE;
	}

	return TRUE;
}


void AssignRECTToSRECT(RECT &r, SRECT &s)
{
	s.Left = (short) r.left;
	s.Right = (short) r.right;
	s.Bottom = (short) r.bottom;
	s.Top = (short) r.top;
}


void AdjustRouteRgns(CBalloon *balloons[], int index)
{
	// subtract out index's routeRgn from other routeRgns
	int left = balloons[index]->m_routeRgn.Left;
	int right = balloons[index]->m_routeRgn.Right;
	int toX = balloons[index]->m_speaker->m_arrowX;
	
	for (int i = 0; i < index; i++)
		balloons[i]->SetRouteRgn(toX, left, right);
}


int ComputeDisplacementPenalty(CPtrArray &bdyArray, int nEntries) {
	int penalty = 0;

	for (int i = 0; i < nEntries; i++) {
		CBodyRecord *r = (CBodyRecord *) bdyArray[i];
		CAvatarX *av = GetAvatar(r->m_body->m_avatarID);
		if (i > 0) {
			int rt = ((CBodyRecord *) bdyArray[i-1])->m_body->m_avatarID;
			if (av->m_lastRight != ((CBodyRecord *) bdyArray[i-1])->m_body->m_avatarID)
				penalty++;
		}
		if (i < nEntries-1) {
			int lt = ((CBodyRecord *) bdyArray[i+1])->m_body->m_avatarID;
			if (av->m_lastLeft != ((CBodyRecord *) bdyArray[i+1])->m_body->m_avatarID)
				penalty++;
		}
	}
	return penalty;
}

int EvalPair(CBodyRecord &b1, CBodyRecord &b2, int deltaPlacement)
{
	int rating = 0, desiredDir;
	if (deltaPlacement > 0)
		desiredDir = FALSE;
	else {
		desiredDir = TRUE;
		deltaPlacement = -deltaPlacement;
	}

	CUserInfo *pui1 = (CUserInfo *)(GetAvatar(b1.m_body->m_avatarID)->m_userInfo);
	ASSERT(pui1);
	// REGISB: 11/13/97 new m_udi in this function
	int nTalkTos = pui1->m_udi.m_talkTos.GetUpperBound() + 1;
	if (nTalkTos == 0) {
		if (b1.m_body->m_flip != desiredDir)  // talking to world, but I'm not facing other's direction
			rating += 4;
		if (b2.m_body->m_flip == desiredDir)  // talking to world, but he's not facing my direction
			rating += 2;
	} else {
		UINT b2ID = b2.m_body->m_avatarID;
		void *pui2 = GetAvatar(b2ID)->m_userInfo;
		for (int i = 0; i < nTalkTos; i++) {
			if (pui1->m_udi.m_talkTos[i] == (DWORD) pui2) {
				if (b1.m_body->m_flip == desiredDir)  // if I'm facing him, then rating gets 2*adjacency
					rating += 4*(deltaPlacement-1);
				else
					rating += 40;						  // if I'm facing away, then heavy penalty
				if (b2.m_body->m_flip == desiredDir)   // if he's facing away, while I'm talking to him, minor penalty
					rating += 4;
			}
		}
	}
	return rating;
}


void AddTalkTos(CBodyRecord bdys[], int &recCount) {
	int initialCount = recCount;
	for (int i = 0; i < initialCount; i++) {
		CAvatarX *av = GetAvatar(bdys[i].m_body->m_avatarID);
		CUserInfo *pui = (CUserInfo *)(av->m_userInfo);
		// REGISB: 11/13/97 new m_udi in this function
		int nTalkTos = pui->m_udi.m_talkTos.GetUpperBound() + 1;
		for (int j = 0; j < nTalkTos; j++) {
			if (recCount >= 5) return;		// don't add more than 5 people to the panel!!!
			int duplicate = FALSE;
			for (int k = 0; k < recCount; k++) {
				CUserInfo *theirPui = (CUserInfo *)(GetAvatar(bdys[k].m_body->m_avatarID)->m_userInfo);
				if (pui->m_udi.m_talkTos[j] == (DWORD)theirPui) {
					duplicate = TRUE;
					break;
				}
			}
			if (!duplicate) {
				CUserInfo *tst = (CUserInfo *)pui->m_udi.m_talkTos[j];
				CAvatarX *theirAv = GetAvatar(((CUserInfo *)pui->m_udi.m_talkTos[j])->GetAvatarID());
				CEmotion neutral(0.0, 0.0);
				bdys[recCount].m_body = theirAv->GetBodyFromEmotion(neutral);
				bdys[recCount].m_body->m_requested = FALSE;   // talktos not required
				bdys[recCount++].m_priority = BR_GOODIDEA;
			}
//			AddLookAt(av->m_talkTo[j], bdys[i]->m_avatarID, TRUE);
		}
	}
}


int EvalPlacement(CPtrArray &bdyArray, int nPlaced, CBodyRecord &bdy, int index, int &dir)
{
	bdyArray.InsertAt(index, &bdy);

	int penalty = ComputeDisplacementPenalty(bdyArray, nPlaced+1);
    int ratingR = penalty, ratingL = penalty;

	bdy.m_body->m_flip = FALSE;

	for (int i = 0; i <= nPlaced; i++) {
		CBodyRecord *rec1 = (CBodyRecord *) (bdyArray[i]);
		for (int j = i+1; j <= nPlaced; j++) {
			CBodyRecord *rec2 = (CBodyRecord *) bdyArray[j];
			ratingR += EvalPair(*rec1, *rec2, j-i) + EvalPair(*rec2, *rec1, i-j);
		}
	}

	bdy.m_body->m_flip = TRUE;

	for (i = 0; i <= nPlaced; i++) {
		CBodyRecord *rec1 = (CBodyRecord *) (bdyArray[i]);
		for (int j = i+1; j <= nPlaced; j++) {
			CBodyRecord *rec2 = (CBodyRecord *) bdyArray[j];
			ratingL += EvalPair(*rec1, *rec2, j-i) + EvalPair(*rec2, *rec1, i-j);
		}
	}

	bdyArray.RemoveAt(index);

//	TRACE("EvalPair: ratingR = %d, ratingL = %d, nPlaced = %d, index = %d.\n",
//		  ratingR, ratingL, nPlaced, index);

	if (ratingR < ratingL) {
		dir = FALSE;
		return ratingR;
	} else if (ratingR > ratingL) {
		dir = TRUE;
		return ratingL;
	} else {  // equiv, so return avatar's last dir
		dir = GetAvatar(bdy.m_body->m_avatarID)->m_lastDir;
		return ratingR; 
	}
}


void DoGreedyOrdering(CBodyRecord bdys[], int recCount, CPtrArray &bdyArray)
{
	int nPlaced = 0, dir, bestRating, bestPosition, bestDir;
	for (int i = 0; i < recCount; i++) {
		bestRating = 1000;
		for (int j = 0; j <= nPlaced; j++) {
			int rating = EvalPlacement(bdyArray, nPlaced, bdys[i], j, dir);
			if (rating < bestRating) {
				bestRating = rating;
				bestPosition = j;
				bestDir = dir;
			}
		}
		// insert element at best position
		bdys[i].m_body->m_flip = bestDir;
		bdyArray.InsertAt(bestPosition, bdys+i);
		nPlaced++;
	}
}


BOOL OrderAvatars(CBodyRecord bdys[], int &recCount, CPtrArray &placed)
{
	placed.SetSize(0, 5);
	if (recCount < 5) 
		AddTalkTos(bdys, recCount);
	DoGreedyOrdering(bdys, recCount, placed);
//	DoForcedOrdering(bdys, recCount, placed);		// FOR SIGGRAPH PIX

	return TRUE;
}

void UpdateHistoresis(CPtrArray &placed, int nPlaced)
{
	for (int i = 0; i < nPlaced; i++) {
		CBodyRecord *r = (CBodyRecord *)(placed[i]);
		CAvatarX *av = GetAvatar(r->m_body->m_avatarID);
		av->m_lastDir = r->m_body->m_flip;

		if (i > 0)
			av->m_lastRight = ((CBodyRecord *)(placed[i-1]))->m_body->m_avatarID;

		if (i < nPlaced-1)
			av->m_lastLeft = ((CBodyRecord *)(placed[i+1]))->m_body->m_avatarID;
	}
}


// picks a title at random from the resources, and returns a strdup'ed char *.
//  Random titles begin at IDS_TITLE1, and continue until a "" is reached, or
//  a non-existent resource is reached (whichever comes first)
char *GetRandomTitle()
{
	CString strTitle;
	static int nTitles = -1;
	if (nTitles == -1) {   // get title count (only need to do this first time)
		UINT titleID = IDS_TITLE1;
		while (TRUE) {
			if (!strTitle.LoadString(titleID) || strTitle.IsEmpty()) break;
			else titleID++;
		}
		nTitles = titleID - IDS_TITLE1;
	}
	if (nTitles < 1) {
		strTitle.LoadString(IDS_NOTITLE);
	} else {
		int chosenTitle = (int)(randfloat() * nTitles);
		chosenTitle = min(chosenTitle, nTitles-1);
		strTitle.LoadString(chosenTitle + IDS_TITLE1);
	}
	return strdup(strTitle);
}


void AddStarsAux(CPtrArray &stars, int maxStars)
{
	int inserted, GetAvatarUpperBound();
	extern CMapStringToPtr *mapNickToPtr;
	void *p;
	CString nick;
	extern CUserInfo *puiSelf;

	POSITION pos = mapNickToPtr->GetStartPosition();
	while (pos) {
		mapNickToPtr->GetNextAssoc(pos, nick, p);
		CUserInfo *pui = (CUserInfo *) p;
		CAvatarX *newAv = GetAvatar(pui->GetAvatarID());
		if (!newAv->m_icon) continue;
		BOOL usDeparted = pui->IsDeparted();

		if (pui == puiSelf) {
			stars.InsertAt(0, newAv);
			continue;
		} else {
			int upper = stars.GetUpperBound();
			inserted = FALSE;
			for (int i = 1; i <= upper; i++) {
				CAvatarX *av = (CAvatarX *)(stars[i]);
				CUserInfo *theirPui = (CUserInfo *) av->m_userInfo;
				BOOL themDeparted = theirPui->IsDeparted();
				if ((!usDeparted && themDeparted)||
					((usDeparted == themDeparted) && (newAv->m_nSends > av->m_nSends))) {
					stars.InsertAt(i, GetAvatar(pui->GetAvatarID()));
					inserted = TRUE;
					break;
				}
			}
			// insert at end if there's room
			if (upper <= maxStars - 1 && !inserted) stars.InsertAt(upper+1, GetAvatar(pui->GetAvatarID()));
		}
	}

	int starsSoFar = stars.GetUpperBound() + 1;
	if (starsSoFar >= maxStars) return;
#if 0
	// add duplicates as necessary to fill out panel...  (but for now, only if standalone...)
	if (GetConnectionStatus() != CX_DISCONNECTED) return;
	int avsUpper = GetAvatarUpperBound();
	for (int i = 1; i <= avsUpper; i++) {
		CAvatarX *newAv = GetAvatar(i);
		if (!newAv->m_icon) continue;
		CUserInfo *newPui = (CUserInfo *) newAv->m_userInfo;
		if (newPui && newPui->GetAvatarID() != newAv->m_avatarID && newAv->m_nSends > 0) {  // then we have changed avs ...
			int starsUpper = stars.GetUpperBound();
			inserted = FALSE;
			for (int j = starsSoFar; j <= starsUpper; j++) {
				CAvatarX *av = (CAvatarX *)(stars[j]);
				if (newAv->m_nSends > av->m_nSends) {
					stars.InsertAt(j, newAv);
					inserted = TRUE;
					break;
				}
			}
			// insert at end if there's room
			if (starsUpper <= maxStars - 1 && !inserted) stars.InsertAt(starsUpper+1, newAv);
		}
	}
#endif
}


BOOL CPanelElement::SetBBox(int left, int bottom, int right, int top)
{
	m_bbox.Top = (short) top;
	m_bbox.Left = (short) left;
	m_bbox.Bottom = (short) bottom;
	m_bbox.Right = (short) right;
	return TRUE;
}


CPanel::CPanel()
{
	// choose a set seed so that panel always refreshes the
	// same way.
	m_seed = rand();
	m_hasBorder = TRUE;
	m_backDrop.m_backID = GetChatDoc() ? GetChatDoc()->GetBackDropID() : 0;
}


CPanel::CPanel(const CPanel &p) {
	m_seed = p.m_seed;
	m_hasBorder = p.m_hasBorder;
	m_backDrop.m_backID = p.m_backDrop.m_backID;

	// copy avatars
	POSITION pos = p.m_bodies.GetHeadPosition();
	while (pos) {
		CBody *b = (CBody *) p.m_bodies.GetNext(pos);
		m_bodies.AddTail(b->Clone());
	}

	// copy balloons
	pos = p.m_elements.GetHeadPosition();
	while (pos) {
		CBalloon *oldB = (CBalloon *) p.m_elements.GetNext(pos);
		CBalloon *newB = oldB->Clone();
		int index = GetIndex(p.m_bodies, oldB->m_speaker);  // trick: must sub in new avatar
		POSITION bdyPos = m_bodies.FindIndex(index);
		CBody *matchingBody = (CBody *) m_bodies.GetAt(bdyPos);
		newB->m_speaker = matchingBody;
		m_elements.AddTail(newB);
	}
}


CPanel::~CPanel()
{
	POSITION pos = m_elements.GetHeadPosition();
	while (pos != NULL)
	{	// for each page
		CPanelElement *pe = (CPanelElement *) m_elements.GetNext(pos);
		delete pe;
	}

	pos = m_bodies.GetHeadPosition();
	while (pos != NULL)
	{
		CBody *bdy = (CBody *) m_bodies.GetNext(pos);
		delete bdy;
	}
}


CBody *CPanel::FetchSpeaker(UINT uID)
{
	POSITION pos = m_bodies.GetHeadPosition();

	while (pos != NULL)
	{
		CBody *bdy = (CBody *) m_bodies.GetNext(pos);
		if (bdy->m_avatarID == uID)
			return bdy;
	}

	CAvatarX *av = GetAvatar(uID);
	CBody *bdy = av->m_body->Clone();
	av->RecordBody(bdy);

	m_bodies.AddTail(bdy);
	return bdy;
}


BOOL CPanel::ReplaceBody(UINT id) {
	POSITION pos = m_bodies.GetHeadPosition();
	while (pos) {
		CBody *oldBdy = (CBody *) m_bodies.GetAt(pos);
		if (oldBdy->m_avatarID == id) {
			// Create a clone of the requested body, and substitute it.
			CAvatarX *av = GetAvatar(id);
			CBody *newBdy = av->m_body->Clone();
			newBdy->m_requested = TRUE;
			m_bodies.SetAt(pos, newBdy);
			av->RecordBody(newBdy);

			// Now replace body in all associated balloons
			POSITION p2 = m_elements.GetHeadPosition();
			while (p2) {
				CBalloon *b = (CBalloon *) m_elements.GetNext(p2);
				if (b->m_speaker->m_avatarID == id)
					b->m_speaker = newBdy;
			}
			// must be done *after* the b->m_speaker->m_avatarID test, since it may be b->m_speaker that's deleted.
			delete oldBdy;
			return TRUE;
		}
		m_bodies.GetNext(pos);
	}
	return FALSE;
}


BOOL CPanel::AvatarInPanel(UINT avID) {
	POSITION pos = m_bodies.GetHeadPosition();
	while (pos) {
		CBody *b = (CBody *) m_bodies.GetNext(pos);
		if (b->m_avatarID == avID) return TRUE;
	}
	return FALSE;
}


void CUnitPanel::Draw(CDC *dc, POINT *ul, RECT *dmgRect)
{
	RECT rect, oldClip, itemBox;

	rect.top = rect.left = 0;
	rect.right = CUnitPanelPage::m_unitWidth;
	rect.bottom = - CUnitPanelPage::m_unitHeight;

//	dc->FillSolidRect(&rect, RGB(randfloat()*255, randfloat()*255, randfloat()*255));
//	return;

	dc->GetClipBox(&oldClip);
	dc->IntersectClipRect(&rect);
	dc->IntersectClipRect(dmgRect);

	m_backDrop.Draw(dc, &rect, dmgRect);
//	DrawRoutes(dc, this);					

	// Draw each avatar
	POSITION pePos = m_bodies.GetHeadPosition();

	CBody *bdy = NULL;
	while (pePos != NULL)
	{
		bdy = (CBody *) m_bodies.GetNext(pePos);
		bdy->GetBBox(&itemBox);
		if (bbox_overlap(dmgRect, &itemBox))
			bdy->Draw(dc, NULL, dmgRect);
	}

	// Draw each panel element
	pePos = m_elements.GetTailPosition();
	while (pePos != NULL)
	{	// for each panel
		CPanelElement *pe = (CPanelElement *) m_elements.GetPrev(pePos);
		pe->GetBBox(&itemBox);
		if (bbox_overlap(dmgRect, &itemBox))
			pe->Draw(dc, NULL, dmgRect);
	}

	if (m_hasBorder)
		DrawBorder(dc, &rect);

	dc->SelectClipRgn(NULL, RGN_COPY);		// clear current clip
	dc->IntersectClipRect(&oldClip);		// reset old clip
}

void CUnitPanel::DrawBorder(CDC *dc, RECT *rect) {
	CPen *oldPen = dc->SelectObject(&m_borderPen);
	dc->BeginPath();
	dc->MoveTo(rect->left, rect->bottom);
	dc->LineTo(rect->left, rect->top);
	dc->LineTo(rect->right, rect->top);
	dc->LineTo(rect->right, rect->bottom);
	dc->CloseFigure();
	dc->EndPath();
	dc->StrokePath();

	dc->SelectObject(oldPen);
}


void CUnitPanel::LayoutAvatars() {
	BOOL OrderAvatars(CBodyRecord [], int&, CPtrArray&);
	CBodyRecord bRecs[MAXBDYPERFRAME];
	CPtrArray placed;
	short width[MAXBDYPERFRAME], height[MAXBDYPERFRAME], normHeight[MAXBDYPERFRAME], top[MAXBDYPERFRAME], headHeight[MAXBDYPERFRAME], maxHeadHeight = 0;
	double arrowX[MAXBDYPERFRAME];
	short bitArrowX;		// dist from left of bitmap of arrowX
	int bdyCount = 0, bdyWidth = 0, sumWidth = 0, maxNorm = 0;
	int nBodies = m_bodies.GetCount();

	// for now, lay them out in order
	ASSERT(nBodies > 0 && nBodies < MAXBDYPERFRAME);
	int maxBodyHeight = (int)(CUnitPanelPage::m_unitHeight / 1.9);
	int minMargin = 0;
	POSITION pos = m_bodies.GetHeadPosition();
	while (pos) {
		CBody *b = (CBody *) m_bodies.GetNext(pos);
		if (IsSpeaker(b)) { 	// Only grab the speakers 
			bRecs[bdyCount].m_body = b;
			bRecs[bdyCount].m_priority = BR_SPEAKER;
			bdyCount++;
		} else 
			delete b;					    // Other bodies get reclaimed
	}

	m_bodies.RemoveAll();

	OrderAvatars(bRecs, bdyCount, placed);
	ASSERT(bdyCount > 0);

	for (int i = 0; i < bdyCount; i++) {
		CBody *b = ((CBodyRecord *)(placed[i]))->m_body;
		b->GetDimInfo(width[i], height[i], normHeight[i], headHeight[i], bitArrowX);
		arrowX[i] = ((double) bitArrowX) / width[i];					// initially store arrows as percentage of width from left
		maxNorm = max(maxNorm, normHeight[i]);
	}

	for (i = 0; i < bdyCount; i++) {
		// scale all of them such that maxHeight == m_unitHeight / 2
		int newHeight  = ROUND(maxBodyHeight * ((float)normHeight[i] / maxNorm));
		float scaleRatio = (float)newHeight / height[i];
		height[i] = newHeight;
		width[i] = ROUND(scaleRatio * width[i]);
		top[i] = -CUnitPanelPage::m_unitHeight + height[i];
		headHeight[i] = ROUND(scaleRatio * headHeight[i]);
		bdyWidth += width[i];
	}

	sumWidth = bdyWidth + (bdyCount+1) * minMargin;

	double zoomFactor = 1.0;
	if (sumWidth > CUnitPanelPage::m_unitWidth) {
		// must reduce the size of the avatars
		float reduction = (float) CUnitPanelPage::m_unitWidth / sumWidth;
		bdyWidth = 0;
		for (i = 0; i < bdyCount; i++) {
			height[i] = ROUND(height[i] * reduction);
			width[i] = ROUND(width[i] * reduction);
			top[i] = -CUnitPanelPage::m_unitHeight + height[i];
			bdyWidth += width[i];
		}
		AdjustArtToCoord(0, 1.0);
	} else if (bZoomIn && !Establishing()) {
		// increase size of avatars
		zoomFactor = (double) CUnitPanelPage::m_unitWidth / sumWidth;

		for (i = 0; i < bdyCount; i++)
			maxHeadHeight = max(maxHeadHeight, headHeight[i]);
		double headFactor = (double)maxBodyHeight / (maxHeadHeight * 1.2);  // don't cut at neck
		zoomFactor = min(zoomFactor, headFactor);
		if (zoomFactor < 1.1) zoomFactor = 1.0;

		bdyWidth = 0;
		for (i = 0; i < bdyCount; i++) {
			height[i] = ROUND(height[i] * zoomFactor);
			width[i] = ROUND(width[i] * zoomFactor);
			bdyWidth += width[i];
		}
	} 
	AdjustArtToCoord(-CUnitPanelPage::m_unitHeight + maxBodyHeight, zoomFactor);

	int margin = (CUnitPanelPage::m_unitWidth - bdyWidth) / (bdyCount+1); // margins also between avs and borders
	int xOffset = margin;
	for (i = 0; i < bdyCount; i++) {
		CBodyRecord *r = (CBodyRecord *) placed[i];
		CBody *b = r->m_body;
		m_bodies.AddTail(b);
		b->SetBBox(xOffset, top[i]-height[i], xOffset+width[i], top[i]);
		b->m_arrowX = b->m_bbox.Left + ROUND(arrowX[i] * (b->m_bbox.Right - b->m_bbox.Left));
		xOffset += width[i] + margin;
	}

	void UpdateHistoresis(CPtrArray &, int);
	UpdateHistoresis(placed, bdyCount);
}

BOOL CUnitPanel::IsSpeaker(CBody *bdy) {
	if (bdy->m_requested) return TRUE;   // sort of makes them a speaker

	UINT avID = bdy->m_avatarID;
	POSITION pos = m_elements.GetHeadPosition();
	while (pos) {
		CPanelElement *e = (CPanelElement *) m_elements.GetNext(pos);
		if (e->GetType() & PE_BALLOON) {
			CBalloon *b = (CBalloon *) e;
			if (b->m_speaker->m_avatarID == avID) return TRUE;
		}
	}
	return FALSE;
}



RECT CUnitPanel::GetBalloonRect() {
	RECT brect;
	brect.left = brect.top = 0;
	brect.right = CUnitPanelPage::m_unitWidth;
	brect.bottom = -CUnitPanelPage::m_unitHeight / 2;
	if (m_hasBorder) {
		int penWidth = m_borderWidth;
		brect.left += penWidth;
		brect.right -= penWidth;
		brect.top -= penWidth;
	}

	return brect;
}


BOOL CUnitPanel::LayoutBalloons(char **pszRest, CDWordArray **pprgdwRestFormatting, char **pszURLStartInRest)
{
	int			nb = 0;
	CBalloon*	balloons[10];
	
	*pszRest = NULL;
	*pszURLStartInRest = NULL;
	*pprgdwRestFormatting = NULL;

	RECT		rcFreeRect = GetBalloonRect();
	POSITION	pos = m_elements.GetHeadPosition();

	srand(m_seed);	// always layout panel the same random way

	while (pos) {	// stash in an array for easy access
		CBalloon *nextBalloon = (CBalloon *) m_elements.GetNext(pos);
		balloons[nb++] = nextBalloon;
	}

	for (int i = 0; i < nb; i++)
		if (!LayoutBalloon(balloons, nb, i, rcFreeRect)) {  // best guess for layout
			if (i == 0 && nb == 1) {
				ForceFitBalloon(balloons[i], rcFreeRect, pszRest, pprgdwRestFormatting, pszURLStartInRest);
				return TRUE;
			} else return FALSE;
		}

	return TRUE;
}

void CUnitPanel::GetCloudEstimate(CBalloon *balloons[], int nb, int index, RECT& freeRect, RECT& brect) {
	int len, lineHeight, goalWidth;

	CBalloon *balloon = balloons[index];
	int area = balloon->AreaEstimate(&len, &lineHeight);
	int maxWidth = freeRect.right - freeRect.left;

	BOOL canBeTall = TRUE; // NoneToLeft(balloons, nb, index);
	if (len <= ONELINETHRESHOLD) {
		goalWidth = len;
	} else if (canBeTall) {
		int potentialHeight = LowestPreviousBottom(balloons, index, freeRect.top) - freeRect.bottom + MINHOOKHEIGHT;
		int minWidth = area / potentialHeight;
		minWidth = max(minWidth, balloon->WidestWord());
		goalWidth = minWidth + (int)(randfloat() * (maxWidth - minWidth));
	} else {
		// it should be wide, aim for no more than two or three lines
		// pick a random number between 1 and 3 inclusive
		int goalLines = 1 + (int)(randfloat() * 3.0);
		goalLines = min(3, goalLines);
		goalWidth = area / (goalLines * lineHeight);
	}

	// randomly place brect in x, guaranteeing that it overlaps character
	goalWidth = min(goalWidth+200, maxWidth); // the + N is a fudge factor.  FIX!!!
	goalWidth = min(goalWidth, len+200);		// won't be wider than len (FIX FUDGE)
	if (balloon->GetType() & PE_BOX) brect.left = freeRect.left;
	else {
		int toPtX = balloon->m_speaker->m_arrowX;
		int leftLimit = toPtX - goalWidth;
		int rightLimit = toPtX;
		int startX = leftLimit + (int)(randfloat() * (rightLimit - leftLimit));
		if (startX < freeRect.left) startX = freeRect.left;
		if (startX + goalWidth > freeRect.right) startX = freeRect.right - goalWidth;
		brect.left = startX;
	}
	brect.right = brect.left + goalWidth;  // top and bottom of brect computed elsewhere
}


BOOL CUnitPanel::LayoutBalloon(CBalloon *balloons[], int nb, int index, RECT& freeRect)
{
	BOOL itFit;
	RECT brect;

	GetCloudEstimate(balloons, nb, index, freeRect, brect);
	itFit = GetInterveningBBox(balloons, index, freeRect, brect);
	if (!itFit)
		return FALSE;

	CBalloon *balloon = balloons[index];
	if (!balloon->SetBBox(brect.left, brect.bottom, brect.right, brect.top))
		return FALSE;	// couldn't build a balloon with this text and size
	if (balloon->m_bbox.Top > -250)
		balloon->DockAtTop(freeRect.top);
	balloon->GetCloudBBox(&balloon->m_routeRgn);	// y not significant (note: this doesn't exactly provide disjoint routeRgns.  Fix)
	if (balloon->m_routeRgn.Bottom < freeRect.bottom + MINHOOKHEIGHT)
		return FALSE;
	AdjustRouteRgns(balloons, index);
	return TRUE;
}


void CUnitPanel::AdjustArtToCoord(int fixedY, double zoomFactor) {
	if (m_backDrop.m_mode == BF_NOZOOM) zoomFactor = 1.0;

	int logHeight = ROUND(CUnitPanelPage::m_unitHeight / zoomFactor);
	int logWidth = ROUND(CUnitPanelPage::m_unitWidth / zoomFactor);
	int newFixedY = ROUND(fixedY / zoomFactor);
	int delta = fixedY - newFixedY;
	m_backDrop.SetBBox(0, -logHeight + delta, logWidth, delta);
}


void CUnitPanel::RearrangeBalloons(CBalloon *balloons[], int nb, RECT& freeRect) {
}


CPage::~CPage() {
	POSITION pos = m_panels.GetHeadPosition();
	while (pos != NULL)	{							// for each page
		CPanel *panel = (CPanel *) m_panels.GetNext(pos);
		delete panel;
	}
}


CPanel *CPage::RemoveLastPanel() {
	// following refresh would be elegant, but it causes double repaints. why? 
//	RefreshLastPanel();	// so that it redraws
	return ((CPanel *) m_panels.RemoveTail());
}


CSize CUnitPanelPage::GetScrollPage() {
	return CSize(m_unitWidth + m_vInterstice, m_unitHeight + m_hInterstice);
}

void CUnitPanelPage::RefreshLastPanel() {
	int nPanels = m_panels.GetCount();
	ASSERT(nPanels > 0);
	nPanels--;
	RefreshPanelN(nPanels);
}

void CUnitPanelPage::RefreshPanelN(int nPanels) {	// nPanels starts at 0
	CDamage d;
	void UpdateViewsX(CView* pSender, LPARAM lHint = 0L, CObject* pHint = NULL, BOOL scrollToView = FALSE);

	// Calculate area that will be damaged
	int panelsToLeft = (nPanels % m_panelsPerRow);
	d.m_g.left = m_leftX + panelsToLeft * m_unitWidth;
	if (panelsToLeft > 0) d.m_g.left += m_vInterstice * panelsToLeft;
	int panelsAbove = nPanels / m_panelsPerRow;
	d.m_g.top = m_topY - (panelsAbove * m_unitHeight);
	if (panelsAbove > 0) d.m_g.top -= m_hInterstice * panelsAbove;
	d.m_g.bottom = d.m_g.top - m_unitHeight;
	d.m_g.right = d.m_g.left + m_unitWidth;

	UpdateViewsX(m_doc->m_view, 0L, &d);
}
	

BOOL CUnitPanelPage::AddPanel(CPanel *newPanel) {	
	m_panels.AddTail(newPanel);
	RefreshLastPanel();
	return TRUE;
}


CBalloon *CUnitPanelPage::MakeBalloon(const char *szMesg, BYTE byteMode, CDWordArray *prgdwFormatting, const char *szURLStart)
{
	switch (byteMode)
	{
	case SM_SAY:
		return (new CBWoodringNormal(szMesg, prgdwFormatting, szURLStart));
		break;
	case SM_WHISPER:
		return (new CBWoodringWhisper(szMesg, prgdwFormatting, szURLStart));
		break;
	case SM_THINK:
		return (new CBWoodringThink(szMesg, prgdwFormatting, szURLStart));
		break;
	case SM_ACTION:
		return (new CBWoodringBox(szMesg, prgdwFormatting, szURLStart));
		break;
	case SM_SHOUT:
//		return (new (CBWoodringShout(szMesg));
		break;
	default:
		ASSERT(0);
		break;
	}
	return NULL;
}


BOOL CUnitPanelPage::AddLine(UINT uID, const char *szWords, BYTE byteMode, CDWordArray *prgdwFormatting, const char *szURLStart)
{
	BOOL	bReplaceLast;
	CPanel	*pNewP, *pOldP;
//	void AddSemantics(CPanel *, const char *), PostSemantics(CPanel *, const char *);

	if (byteMode == SM_ACTION)
		StartNewPanel();			// start new panel for boxes

	if (strcmp(szWords, "<Brk>") == 0)
	{	// Force a new panel, given the break char (for debugging)
		StartNewPanel();
		return TRUE;
	}

	if (strcmp(szWords, "<Chr>") == 0)
		return AddReaction(uID);

	g_bNewedPanel = FALSE;

	pOldP = (CUnitPanel*) m_panels.GetTail();
	if (m_newPanel || pOldP->m_elements.GetCount() >= 5 || m_panels.GetCount() < 2 || pOldP->AvatarInPanel(uID))
	{
		pNewP = new CUnitPanel;
		m_newPanel = FALSE;
		bReplaceLast = FALSE;
		g_bNewedPanel = TRUE;
	}
	else
	{
		pNewP = pOldP->Clone();
		bReplaceLast = TRUE;
	}

	// AddSemantics(newP, words);

	// make a new balloon
	CBalloon *newBalloon = MakeBalloon(szWords, byteMode, prgdwFormatting, szURLStart);
	newBalloon->m_speaker = pNewP->FetchSpeaker(uID);
	
	pNewP->m_elements.AddTail(newBalloon);	// add balloon to panel

	// if char was in panel, for now, sub body
	pNewP->ReplaceBody(uID);

	pNewP->LayoutAvatars();					// make a best guess as to avatars & positioning

	char		*szLeftOverString = NULL, *szURLStartInLeftOver = NULL;
	CDWordArray	*prgdwLeftOverFormatting = NULL;

	if (!pNewP->LayoutBalloons(&szLeftOverString, &prgdwLeftOverFormatting, &szURLStartInLeftOver))
	{
		// if (pNewP->m_elements.GetCount() <= 1) return TRUE;  // Ignore utterance for now -- won't fit in panel
		delete pNewP;
		StartNewPanel();
		AddLine(uID, szWords, byteMode, prgdwFormatting, szURLStart);
	}
	else
	{
		if (bReplaceLast)
		{
			RemoveLastPanel();
			delete pOldP;
		}
		//PostSemantics(pNewP, words);
		AddPanel(pNewP);
		void ResetAvatar(int);
		ResetAvatar(uID);   // Set the avatar to a new neutral position
		if (szLeftOverString)
		{
			AddLine(uID, szLeftOverString, byteMode, prgdwLeftOverFormatting, szURLStartInLeftOver);
			free(szLeftOverString);
			if (szURLStartInLeftOver)
				delete [] szURLStartInLeftOver;
			if (prgdwLeftOverFormatting)
			{
				prgdwLeftOverFormatting->RemoveAll();
				delete prgdwLeftOverFormatting;
				prgdwLeftOverFormatting = NULL;
			}
		}
	}

	return TRUE;
}


BOOL CUnitPanelPage::AddReaction(UINT uID)
{
	BOOL	bReplaceLast;
	CPanel	*pNewP, *pOldP;
	
	pOldP = (CUnitPanel *) m_panels.GetTail();
	if (m_newPanel || pOldP->m_bodies.GetCount() >= 5 || m_panels.GetCount() < 2)
	{
		pNewP = new CUnitPanel;
		m_newPanel = FALSE;
		bReplaceLast = FALSE;
	}
	else
	{
		pNewP = pOldP->Clone();
		bReplaceLast = TRUE;
	}

	// if char was in panel, for now, sub body and redraw
	if (!pNewP->ReplaceBody(uID))
		pNewP->FetchSpeaker(uID);

	pNewP->LayoutAvatars();					// make a best guess as to avatars & positioning

	char		*szLeftOverString = NULL, *szURLStartInLeftOver = NULL;
	CDWordArray	*prgdwLeftOverFormatting = NULL;

	if (!pNewP->LayoutBalloons(&szLeftOverString, &prgdwLeftOverFormatting, &szURLStartInLeftOver))
	{
		delete pNewP;
		StartNewPanel();
		AddReaction(uID);
	}
	else
	{
		if (bReplaceLast)
		{
			RemoveLastPanel();
			delete pOldP;
		}
		AddPanel(pNewP);
		void ResetAvatar(int);
		ResetAvatar(uID);   // Set the avatar to a new neutral position
	}

	ASSERT(prgdwLeftOverFormatting == NULL);

	return TRUE;
}


void CUnitPanelPage::Draw(CPageView *pView, CDC *dc, POINT *, RECT *damage = NULL) 
{
	// First set up rDC for retained dib section
	RECT	panelRect;
	RECT	damageRel;			// damage rectangle in panel coords
	CDC		memDC;
	pnlDC = &memDC;

	VERIFY(memDC.CreateCompatibleDC(dc));

	// Palette Operations definitely needed, RamuM
	CPalette *oldPal;
	CPalette *curPal = dc->GetCurrentPalette();
#ifdef NOGLOBPAL
	if (oldPal = memDC.SelectPalette(GetPalette(pDC), TRUE))
#else NOGLOBAPAL
	if (oldPal = memDC.SelectPalette(curPal, TRUE))
#endif NOGLOBPAL
		memDC.RealizePalette();

	memDC.m_bPrinting = dc->m_bPrinting;	// this determines which set of bitmaps to use
	memDC.SetMapMode(dc->GetMapMode());

	// REGISB: added 09/15/97
	// Update text color
	memDC.SetTextColor(theApp.m_comicsColor);

	POINT point;
	GetBrushOrgEx(memDC.GetSafeHdc(),&point);
	int iOldMode = memDC.SetStretchBltMode(STRETCHMODE); //COLORONCOLOR);
	SetBrushOrgEx(memDC.GetSafeHdc(),point.x,point.y,&point);

	HBITMAP retSec;
	VERIFY(retSec = pView->GetRetSec(dc));
	CBitmap temp;
	CBitmap *retCBit = temp.FromHandle(retSec);
	CBitmap *bmpOld = memDC.SelectObject(retCBit); // must use a CBitmap


	int truePanelsPerRow = dc->IsPrinting() ? m_printPanelsPerRow : m_panelsPerRow;
	SetRect(&panelRect, 0, 0, m_unitWidth, -m_unitHeight);
	int panelCount = 0;
	POINT loc;
	loc.x = loc.y = 0;
	POSITION panelPos = m_panels.GetHeadPosition();
	while (panelPos != NULL) {						// for each panel
		// XXX - add panel intersection test (then must change loc incrementing)
		CPanel *panel = (CPanel *) m_panels.GetNext(panelPos);
		panelCount++;
		SetRect(&damageRel, damage->left-loc.x, damage->top-loc.y, damage->right-loc.x, damage->bottom-loc.y);
		if (bbox_overlap(&damageRel, &panelRect)) {
			panel->Draw(&memDC, &loc, &damageRel);
			if (!dc->m_bPrinting || !printBMP) {
				POINT loc2 = pView->AccountForScroll(&loc, TRUE, TRUE, dc->m_bPrinting);
				VERIFY(dc->BitBlt(loc2.x, loc2.y, m_unitWidth, -m_unitHeight, &memDC, 0, 0, SRCCOPY));
			} else PrintBMP();
		}
		if (panelCount % truePanelsPerRow == 0) {
			loc.x = 0;
			loc.y -= m_unitHeight + m_hInterstice;
		} else
			loc.x += m_unitWidth + m_vInterstice;
	}

	GetBrushOrgEx(memDC.GetSafeHdc(),&point);
	memDC.SetStretchBltMode(iOldMode);
	SetBrushOrgEx(memDC.GetSafeHdc(),point.x,point.y,&point);

	memDC.SelectObject(bmpOld);

	memDC.SelectPalette(oldPal,TRUE);
}


// note: rect guaranteed to be normalized
void CUnitPanelPage::GetBBox(RECT *rect) {
	rect->left = m_leftX;
	rect->top = m_topY;
	int nPanels = m_panels.GetCount();
	int nRows = (nPanels - 1) / m_panelsPerRow + 1;  // note: assumption that nPanels > 0
	int nColumns = min(nPanels, m_panelsPerRow);
	rect->right = m_leftX + nColumns * m_unitWidth + (nColumns - 1) * m_vInterstice;
	rect->bottom = m_topY - (nRows * m_unitHeight + (nRows - 1) * m_hInterstice);
} 


void CUnitPanelPage::AddTitle(const char *title) {
	CString starringStr;
	RECT border;

	CLabel *titleL = new CLabel(title, m_fiTitle);
	titleL->SetBBox(0, -m_unitHeight/2, m_unitWidth, -100);
	titleL->GetBBox(&border);
	starringStr.LoadString(ID_STARRING);
	CLabel *starringL = new CLabel(starringStr, m_fiShout);
	starringL->SetBBox(0, -m_unitHeight, m_unitWidth, border.bottom);
	CUnitPanel *newPanel = new CUnitPanel;
	newPanel->m_hasBorder = FALSE;
	newPanel->m_backDrop.m_backID = 0;				// no background
	newPanel->m_elements.AddTail(titleL);
	newPanel->m_elements.AddTail(starringL);
	starringL->GetBBox(&border);
	AddStars(newPanel, border.bottom);
	AddPanel(newPanel);
}


void CUnitPanelPage::UpdateTitle() {
	if (m_panels.IsEmpty())
		AddTitle(GetChatDoc()->GetComicsTitle());
	else {
		CUnitPanel *firstPanel = (CUnitPanel *) m_panels.GetHead();
		int count = firstPanel->m_elements.GetCount();
		for (int i = 0; i < count-2; i++) 			// -2 leaves in the title and starring labels
			delete ((CPanelElement *)firstPanel->m_elements.RemoveTail());
		CLabel *starring = (CLabel *) firstPanel->m_elements.GetTail();
		RECT border;
		starring->GetBBox(&border);
		AddStars(firstPanel, border.bottom);
		RefreshPanelN(0);
	}
}


void CUnitPanelPage::ShowInfo(USHORT avID, const char *szInfo)
{
	// szInfo is a control full string

	CString			strFormatted;
	CDWordArray*	prgdwFormatting = new CDWordArray;
	CDWordArray*	prgdwRestFormatting = NULL;
	RECT			rcBorder;

	void Capitalize(char *);

	char* szControlFull = strdup(szInfo);

	Capitalize(szControlFull);

	char* szControlLess = SzControlLess(szControlFull, prgdwFormatting);
	
	int iMaxBoxHeight = (int) (m_unitHeight * MAXINFOTEXTHEIGHT);

	while (TRUE)
	{
		CUnitPanel	*newPanel = new CUnitPanel;
		int			iMargin = newPanel->m_borderWidth + INFOMARGIN;
		char		*szRest = NULL;
		CLabel		*box = new CLabel(szControlLess, m_fiWNormal, prgdwFormatting);

		box->m_format |= FT_LEFT_JUSTIFY;
		newPanel->m_elements.AddTail(box);

		int			iTop = (int)(-100 * ((float) m_unitHeight / 4860)) - box->m_fontI->m_topOffset;
		int			iBottom = iTop - iMaxBoxHeight;

		box->SetBBox(iMargin, iBottom, m_unitWidth-iMargin, iTop);
		box->GetBBox(&rcBorder);

		if (rcBorder.bottom < iBottom)
		{
			szRest = box->SplitHeight(iMaxBoxHeight, &prgdwRestFormatting);
			box->GetBBox(&rcBorder);
		}
		// center text box (text inside box is ragged-right)
		int iBoxWidth = rcBorder.right - rcBorder.left;
		int iNewLeftX = (m_unitWidth - iBoxWidth) / 2;
		box->SetBBox(iNewLeftX, rcBorder.bottom, iNewLeftX + iBoxWidth, rcBorder.top);

		CBody *bdy = GetAvatar(avID)->GetBodyFromEmotion(CEmotion(0.0, 0.0));
		bdy->SetBBox(iMargin, -m_unitHeight, m_unitWidth-iMargin, rcBorder.bottom);
		newPanel->m_bodies.AddTail(bdy);

		newPanel->m_backDrop.m_backID = 0;			// no background for now
		AddPanel(newPanel);
		StartNewPanel();							// no adding to this one!

		if (szControlLess != szControlFull)
			free(szControlLess);
		szControlLess = szRest;
		if (prgdwFormatting)
		{
			prgdwFormatting->RemoveAll();
			delete prgdwFormatting;
		}
		if (!szRest) break;
		prgdwFormatting = prgdwRestFormatting;
	}

	free(szControlFull);
}


void CUnitPanelPage::AddStars(CUnitPanel *panel, int topY) {
	CPtrArray stars;
	CPtrArray sLabels;
	if (MyAvatarID() == 0) return;	// not registered yet
	CAvatarX *myAv = MyAvatar();
	int GetAvatarUpperBound();
	CString avatarCredit;

//	avatarCredit.LoadString(ID_AVATAR_CREDIT);

	int lineHeight = m_fiShout->m_lineHeight;
	int rowHeight = max(ICONSIZE, lineHeight);
	topY = topY - (BELOWSTARRING*m_unitHeight/4860);
	int maxStars = (m_unitHeight + topY) / rowHeight;
	topY -= rowHeight;			// start at bottom of first row

	// first get those that are actually logged in (one per person)
	AddStarsAux(stars, maxStars);
	int nStars = min(maxStars, stars.GetUpperBound()+1);

	int nCols = 1;			// FIX
	int nRows = nStars / nCols;
	if (nStars % nCols) nRows++;		// too tired to think of short expression

	if (nCols == 1) {
		int maxWidth = 0;
		for (int i = 0; i < nStars; i++) {
			RECT bbox;
			const char *nickName;
			
			CAvatarX *star = (CAvatarX *)stars[i];
			star->GetScreenName(&nickName);

			CLabel *label = new CLabel(nickName, m_fiShout);
			label->SetBBox(0, -m_unitHeight, m_unitWidth, 0);
			label->GetBBox(&bbox);
			sLabels.Add(label);
			maxWidth = max(maxWidth, bbox.right - bbox.left);
		}

		maxWidth += ICONSIZE + ICONSPACE;
		int iconOffset = (m_unitWidth - maxWidth) / 2;
		if (iconOffset < 0) iconOffset = 0;
		int textOffset = iconOffset + ICONSIZE + ICONSPACE;
		int iconVdisp = (rowHeight - ICONSIZE)/2;			// center text or icon vertically in row
		int textVdisp = (rowHeight - lineHeight)/2;

		for (i = 0; i < nStars; i++) {
			((CLabel *)sLabels[i])->m_format |= FT_LEFT_JUSTIFY;
			CBodyUnary *b = new CBodyUnary(((CAvatarX *)stars[i])->m_avatarID);
			b->m_bodyID = ((CAvatarX *)stars[i])->m_icon;
			int vertOffset = 
			b->SetBBox(iconOffset, topY+iconVdisp, iconOffset+ICONSIZE, topY+ICONSIZE+iconVdisp);
			panel->m_elements.AddTail(b);
			((CLabel *)sLabels[i])->SetBBox(textOffset, topY+textVdisp, m_unitWidth, topY+lineHeight+textVdisp);
			panel->m_elements.AddTail((CLabel *)sLabels[i]);
			topY -= rowHeight;
		}
	}
}


void CUnitPanelPage::PageSizeInPanels(CPrintInfo *pInfo, int &panelsWide, int &panelsHigh) {
	int pageWidth = pInfo->m_rectDraw.right - pInfo->m_rectDraw.left;
	int pageHeight = pInfo->m_rectDraw.top - pInfo->m_rectDraw.bottom;

	// panelsHigh equation below derives from the fact that
	// panelsHigh * m_unitHeight + (panelsHigh - 1) * m_hInterstice < pageHeight
	panelsHigh = (pageHeight + m_hInterstice) / (m_unitHeight + m_hInterstice);
	panelsWide = (pageWidth + m_vInterstice) / (m_unitWidth + m_hInterstice);
}


void CUnitPanelPage::PreparePrintDC(CDC *pDC, CPrintInfo *pInfo, int pageNum) {
	CPoint vpOrigin;
	RECT clipRect;
	int panelsWide, panelsHigh, startPanelRow, mapY;

	int pageWidth = pInfo->m_rectDraw.right - pInfo->m_rectDraw.left;
	int pageHeight = pInfo->m_rectDraw.top - pInfo->m_rectDraw.bottom;

	PageSizeInPanels(pInfo, panelsWide, panelsHigh);
	m_printPanelsPerRow = panelsWide;				// here we cache this value (side effect, oh well)

	vpOrigin.x = (pageWidth - (panelsWide * m_unitWidth + (panelsWide - 1) * m_vInterstice)) / 2;
	vpOrigin.y = -(pageHeight - (panelsHigh * m_unitHeight + (panelsHigh - 1) * m_hInterstice)) / 2;
	// now calculate the y that should be mapped to vpOrigin.y
	startPanelRow = (pageNum-1) * panelsHigh;
	mapY = startPanelRow * (m_unitHeight + m_hInterstice);
	// pDC->SetWindowOrg(-vpOrigin.x, -(mapY - vpOrigin.y));
	SetPrintOffset(-vpOrigin.x, -mapY - vpOrigin.y);

	clipRect.left = vpOrigin.x;
	clipRect.top = vpOrigin.y;
	clipRect.right = clipRect.left + panelsWide * m_unitWidth + (panelsWide-1) * m_vInterstice;
	clipRect.bottom = clipRect.top - panelsHigh * m_unitHeight - (panelsHigh-1) * m_hInterstice;
	pDC->IntersectClipRect(&clipRect);
}


int CUnitPanelPage::GetPhysicalPageCount(CPrintInfo *pInfo) {
	int panelsWide, panelsHigh, panelCount;

	PageSizeInPanels(pInfo, panelsWide, panelsHigh);
	panelCount = m_panels.GetCount();
	return ((int) ceil((double) panelCount / (panelsWide * panelsHigh)));
}



/* BEGIN BLOCK FOR SIGGRAPH PIX */
UINT bdyOrder[] = {2, 1};						// FOR SIGGRAPH PIX
BOOL dirOrder[] = {FALSE, TRUE};				// FOR SIGGRAPH PIX

void DoForcedOrdering(CBodyRecord bdys[], int recCount, CPtrArray &bdyArray) {
	TRACE("Entering forced w/ reccount of %d.\n", recCount);
	for (int i = 0; i < sizeof(bdyOrder)/sizeof(UINT); i++) {
		for (int j = 0; j < recCount; j++) {
			if (bdys[j].m_body->m_avatarID == bdyOrder[i]) {
				bdys[j].m_body->m_flip = dirOrder[i];
				bdyArray.Add(bdys+j);
				break;
			}
		}
	}
	ASSERT(recCount == bdyArray.GetUpperBound()+1);   // if not, a body not in bdyOrder!
	TRACE("Leaving forced w/ placed of %d.\n", bdyArray.GetUpperBound()+1);
}
/* END BLOCK FOR SIGGRAPH PIX */

		
#if 0
void PrintBodyAvID(CPanel *p, const char *str) {
	POSITION pos = p->m_bodies.GetHeadPosition();
	int i = 0;
	while (pos) {
		CBody *bdy = (CBody *) p->m_bodies.GetNext(pos);
		TRACE("(%x) %s: body #%d has av of %d.\n", p, str, i++, bdy->m_avatarID);
	}
	pos = p->m_elements.GetHeadPosition();
	i = 0;
	while (pos) {
		CBalloon *b = (CBalloon *) p->m_bodies.GetNext(pos);
		TRACE("(%x) %s: balloon #%d has av of %d.\n", p, str, i++, b->m_speaker->m_avatarID);
	}
}
#endif

#if 0
/* Explicitly commented by RamuM
CPalette *lastPal;

void InstallGrays(CDC *dc, BOOL realize = FALSE) {
	LOGPALETTE *lp = (LOGPALETTE *) malloc(sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 255);

	lp->palVersion = 0x300;
	lp->palNumEntries = 256;
	for (int i = 0; i < 256; i++) {
		lp->palPalEntry[i].peRed = i;
		lp->palPalEntry[i].peGreen = i;
		lp->palPalEntry[i].peBlue = i;
		lp->palPalEntry[i].peFlags = 0;
	}

	CPalette *pal = new CPalette();  // This is not being cleaned up - TEST XXXXXXXXXXXXX
	lastPal = pal;
	VERIFY(pal->CreatePalette(lp));
	VERIFY(dc->SelectPalette(pal, FALSE));    // should save and reinstall old palette?
	if (realize) {
		int n = dc->RealizePalette();
		TRACE("%d colors realized.\n", n);
	}
	free(lp);
}

void DrawLines(CDC *dc) {
	for (int i = 0; i < 256; i++) {
		CPen pen(PS_SOLID, 30, PALETTERGB(i, i, i));
		CPen *oldPen = dc->SelectObject(&pen);
		dc->MoveTo(i*15, 0);
		dc->LineTo(i*15, -8000);
		dc->SelectObject(oldPen);
	}
}

*/
#endif

//#define PAGEHEIGHT ((int)(10.5 * 1440)) // printable page 10.5" high, 1440 units/inch
//#define PAGEWIDTH  ((int)(8.15 * 1440))	// printable page 8.15" wide, 1440 units/inch

// REGISB 09/17/97 not used
//void ForceFitLabel(CLabel *pLabel, RECT &rcFreeRect, char **pszRest, CDWordArray **pprgdwRestFormatting)
//{
//*pszRest = pLabel->SplitHeight(rcFreeRect.top - rcFreeRect.bottom, pprgdwRestFormatting);
//}

