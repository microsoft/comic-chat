
#include "stdafx.h"
#include "chat.h"

#include "binddoc.h"
#include "chatdoc.h"
#include "common.h"
#include "traj.h"
#include "spline.h"
#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "balloon.h"
#include "backdrop.h"
#include "panel.h"
#include "ui.h"
#include "vector2d.h"
#include "chatprot.h"
#include <stdlib.h>
#include <math.h>

#include "userinfo.h"     // for AddStars.  Remove this later?

extern CChatApp theApp;

IMPLEMENT_DYNAMIC(CDamage, CObject);

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

double randfloat();

BOOL CPanelElement::SetBBox(int left, int bottom, int right, int top) {
	m_bbox.Top = (short) top;
	m_bbox.Left = (short) left;
	m_bbox.Bottom = (short) bottom;
	m_bbox.Right = (short) right;
	return TRUE;
}

void DrawPoint(CDC *dc, POINT p, int delta) {
	// Draw a simple plus sign at p
	dc->MoveTo(p.x, p.y - delta);
	dc->LineTo(p.x, p.y + delta);
	dc->MoveTo(p.x - delta, p.y);
	dc->LineTo(p.x + delta, p.y);
}


#define YOFFSET 300
#define DELTA 320
void DrawSpline(CDC *dc, RECT *rect) {
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

CPanel::CPanel() {
	// choose a set seed so that panel always refreshes the
	// same way.
	m_seed = rand();
	m_hasBorder = TRUE;
}

CPanel::~CPanel() {
	POSITION pos = m_elements.GetHeadPosition();
	while (pos != NULL)	{							// for each page
		CPanelElement *pe = (CPanelElement *) m_elements.GetNext(pos);
		delete pe;
	}

	pos = m_bodies.GetHeadPosition();
	while (pos != NULL) {
		CBody *bdy = (CBody *) m_bodies.GetNext(pos);
		delete bdy;
	}
}


COLORREF clrs[] = { RGB(80, 80, 80), RGB(130, 130, 130), RGB(160, 160, 160), RGB(240, 240, 240), RGB(255, 255, 255)};
void DrawRoutes(CDC *dc, CPanel *panel) {
	int i = 0;
	POSITION pos = panel->m_elements.GetTailPosition();
	while (pos != NULL) {
		CBalloon *b = (CBalloon *) panel->m_elements.GetPrev(pos);
		RECT r;
		r.left = b->m_routeRgn.Left;
		r.right = b->m_routeRgn.Right;
		r.top = 0;
		r.bottom = -CUnitPanelPage::unitHeight;
		dc->FillSolidRect(&r, clrs[i]);
		i = (i+1) % sizeof(clrs);
	}
}


BOOL bbox_overlap (RECT *bbox1, RECT *bbox2);

void CUnitPanel::Draw(CDC *dc, POINT *ul, RECT *dmgRect) {
	RECT rect, oldClip, itemBox;

	rect.top = rect.left = 0;
	rect.right = CUnitPanelPage::unitWidth;
	rect.bottom = - CUnitPanelPage::unitHeight;

//	dc->FillSolidRect(&rect, RGB(randfloat()*255, randfloat()*255, randfloat()*255));
//	return;

	dc->GetClipBox(&oldClip);
	dc->IntersectClipRect(&rect);
	dc->IntersectClipRect(dmgRect);

	srand(m_seed);	// always draw panel the same random way

	m_backDrop.Draw(dc, &rect, dmgRect);
//	DrawRoutes(dc, this);					

	// Draw each avatar
	POSITION pePos = m_bodies.GetHeadPosition();

	CBody *bdy = NULL;
	while (pePos != NULL) {
		/*CBody * */bdy = (CBody *) m_bodies.GetNext(pePos);
		bdy->GetBBox(&itemBox);
		if (bbox_overlap(dmgRect, &itemBox))
			bdy->Draw(dc, NULL, dmgRect);
	}


	// Draw each panel element
	pePos = m_elements.GetTailPosition();
	while (pePos != NULL) {						// for each panel
		CPanelElement *pe = (CPanelElement *) m_elements.GetPrev(pePos);
		pe->GetBBox(&itemBox);
		if (bbox_overlap(dmgRect, &itemBox))
			pe->Draw(dc, NULL, dmgRect);
	}

	if (m_hasBorder) DrawBorder(dc, &rect);
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

CSize CUnitPanelPage::GetScrollPage() {
	return CSize(unitWidth + vInterstice, unitHeight + hInterstice);
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
	int panelsToLeft = (nPanels % panelsPerRow);
	d.m_g.left = m_leftX + panelsToLeft * unitWidth;
	if (panelsToLeft > 0) d.m_g.left += vInterstice * panelsToLeft;
	int panelsAbove = nPanels / panelsPerRow;
	d.m_g.top = m_topY - (panelsAbove * unitHeight);
	if (panelsAbove > 0) d.m_g.top -= hInterstice * panelsAbove;
	d.m_g.bottom = d.m_g.top - unitHeight;
	d.m_g.right = d.m_g.left + unitWidth;

	if(nPanels >= 1 && GetChatDoc()->m_bPrompt && !GetChatDoc()->m_bEmbedded)
		GetChatDoc()->SetModifiedFlag(TRUE);
	if(nPanels >= 1)
		GetChatDoc()->m_bEnableFileMenu = TRUE;

	UpdateViewsX(NULL, 0L, &d);
}
	

BOOL CUnitPanelPage::AddPanel(CPanel *newPanel) {	
	m_panels.AddTail(newPanel);
	RefreshLastPanel();
	return TRUE;
}



void RandEmotion(CEmotion &e) {
	e.m_intensity = (float) randfloat();
	e.m_emotion = (float)((randfloat() * 2.0 - 1.0) * PI);
}

extern CBody *GetBodyCamBody();

CBody *CPanel::FetchSpeaker(UINT id) {
	POSITION pos = m_bodies.GetHeadPosition();
	while (pos != NULL) {
		CBody *bdy = (CBody *) m_bodies.GetNext(pos);
		if (bdy->m_avatarID == id)
			return bdy;
	}

	CAvatarX *av = GetAvatar(id);
	CBody *bdy = av->m_body->Clone();
	av->RecordBody(bdy);

	m_bodies.AddTail(bdy);
	return bdy;
}

int mycnt = 0;

BOOL CPanel::ReplaceBody(UINT id) {
	POSITION pos = m_bodies.GetHeadPosition();
	while (pos) {
		CBody *oldBdy = (CBody *) m_bodies.GetAt(pos);
		if (oldBdy->m_avatarID == id) {
			// Create a clone of the requested body, and substitute it.
//			if (bdy->m_requested) 	TRACE("Trying to replace a requested body: %d.\n", mycnt++);
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

CPanel *CPage::RemoveLastPanel() {
	// following refresh would be elegant, but it causes double repaints. why? 
//	RefreshLastPanel();	// so that it redraws
	return ((CPanel *) m_panels.RemoveTail());
}

CPage::~CPage() {
	POSITION pos = m_panels.GetHeadPosition();
	while (pos != NULL)	{							// for each page
		CPanel *panel = (CPanel *) m_panels.GetNext(pos);
		delete panel;
	}
}

BOOL CPanel::AvatarInPanel(UINT avID) {
	POSITION pos = m_bodies.GetHeadPosition();
	while (pos) {
		CBody *b = (CBody *) m_bodies.GetNext(pos);
		if (b->m_avatarID == avID) return TRUE;
	}
	return FALSE;
}


CBalloon *Balloons[30];
int cnt = 0;

void SetBalloons(CPanel *p) {
	cnt = 0;
	POSITION pos = p->m_elements.GetHeadPosition();
	while (pos)
		Balloons[cnt++] = (CBalloon *) p->m_elements.GetNext(pos);
}

int bcnt = 0;
CBody * Bodies[30];

void SetAvatars(CPanel *p) {
	bcnt = 0;
	POSITION pos = p->m_bodies.GetHeadPosition();
	while (pos)
		Bodies[bcnt++] = (CBody *) p->m_bodies.GetNext(pos);
}

CBalloon *CUnitPanelPage::MakeBalloon(const char *mesgPtr, UCHAR mode, CPtrArray *links) {
	switch (mode) {
	case SM_SAY:
		return (new CBWoodringNormal(mesgPtr, links));
		break;
	case SM_WHISPER:
		return (new CBWoodringWhisper(mesgPtr, links));
		break;
	case SM_THINK:
		return (new CBWoodringThink(mesgPtr, links));
		break;
	case SM_ACTION:
		return (new CBWoodringBox(mesgPtr, links));
		break;
	case SM_SHOUT:
//		return (new (CBWoodringShout(mesgPtr));
		break;
	default:
		ASSERT(0);
		break;
	}
	return NULL;
}

extern BOOL newedPanel;
int breakNum = 0;

BOOL CUnitPanelPage::AddLine(UINT id, const char *words, UCHAR mode, CPtrArray *links) {
	int replaceLast;
	CPanel *newP, *oldP;
	void AddSemantics(CPanel *, const char *), PostSemantics(CPanel *, const char *);

	if (mode == SM_ACTION) StartNewPanel();			// start new panel for boxes

	if (strcmp(words, "<Brk>") == 0) {	// Force a new panel, given the break char (for debugging)
		StartNewPanel();
		breakNum++;
		return TRUE;
	}

	if (strcmp(words, "<Chr>") == 0)
		return AddReaction(id);

	newedPanel = FALSE;

	oldP = (CUnitPanel *) m_panels.GetTail();
	if (m_newPanel || oldP->m_elements.GetCount() >= 5 || m_panels.GetCount() < 2 || oldP->AvatarInPanel(id)) {
		newP = new CUnitPanel;
		m_newPanel = FALSE;
		replaceLast = FALSE;
		newedPanel = TRUE;
	} else {
		newP = oldP->Clone();
		replaceLast = TRUE;
	}

	AddSemantics(newP, words);

	// make a new balloon
	CBalloon *newBalloon = MakeBalloon(words, mode, links);
	newBalloon->m_speaker = newP->FetchSpeaker(id);
	
	newP->m_elements.AddTail(newBalloon);	// add balloon to panel

	// if char was in panel, for now, sub body
	newP->ReplaceBody(id);

	newP->LayoutAvatars();					// make a best guess as to avatars & positioning

	char *leftOver = NULL;
	if (!newP->LayoutBalloons(&leftOver)) {

//		if (newP->m_elements.GetCount() <= 1) return TRUE;  // Ignore utterance for now -- won't fit in panel
		delete newP;
		StartNewPanel();
		AddLine(id, words, mode, links);
	} else {
		if (replaceLast) {
			RemoveLastPanel();
			delete oldP;
		}
		PostSemantics(newP, words);
		if (leftOver) {  // djk - quick hack to nuke links across panels.  Needs fixing!!!
			void DestroyLinks(CPtrArray **m_links);
			if (newBalloon->m_links)
				DestroyLinks(&newBalloon->m_links); // for now, no links across continuations
			links = NULL;
		}
		AddPanel(newP);
		void ResetAvatar(int);
		ResetAvatar(id);   // Set the avatar to a new neutral position
		if (leftOver) {
			AddLine(id, leftOver, mode, links);  // for now, no links across continuations
			free(leftOver);
		}
	}

	return TRUE;
}


BOOL CUnitPanelPage::AddReaction(int id) {
	BOOL replaceLast;
	CPanel *newP, *oldP;
	
	
	oldP = (CUnitPanel *) m_panels.GetTail();
	if (m_newPanel || oldP->m_bodies.GetCount() >= 5 || m_panels.GetCount() < 2) {
		newP = new CUnitPanel;
		m_newPanel = FALSE;
		replaceLast = FALSE;
	} else {
		newP = oldP->Clone();
		replaceLast = TRUE;
	}

	// if char was in panel, for now, sub body and redraw
	if (!newP->ReplaceBody(id))
		newP->FetchSpeaker(id);

	newP->LayoutAvatars();					// make a best guess as to avatars & positioning

	char *leftOver = NULL;
	if (!newP->LayoutBalloons(&leftOver)) {
		delete newP;
		StartNewPanel();
		AddReaction(id);
	} else {
		if (replaceLast) {
			RemoveLastPanel();
			delete oldP;
		}
		AddPanel(newP);
		void ResetAvatar(int);
		ResetAvatar(id);   // Set the avatar to a new neutral position
	}

	return TRUE;
}

GetIndex(const CPtrList &list, void *member) {
	int index = 0;
	POSITION pos = list.GetHeadPosition();
	while (pos) {
		void *foo = list.GetNext(pos);
		if (member == foo)
			return index;
		index++;
	}
	return -1;
}

CPanel::CPanel(const CPanel &p) {
	m_seed = p.m_seed;
	m_hasBorder = p.m_hasBorder;

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

BOOL Establishing();

#define BR_SPEAKER		0
#define BR_IMPORTANT	1
#define BR_GOODIDEA		2
#define BR_OK			3

#define MAXBDYPERFRAME	20
class CBodyRecord {
public:
	CBody *m_body;
	CPtrList m_lookAts;
	UCHAR m_priority;
};

#define zoomIn	TRUE					// FALSE for SIG PIX

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
	int maxBodyHeight = (int)(CUnitPanelPage::unitHeight / 1.9);
	int minMargin = 0;
	POSITION pos = m_bodies.GetHeadPosition();
	while (pos) {
		CBody *b = (CBody *) m_bodies.GetNext(pos);
		if (IsSpeaker(b->m_avatarID)) { 	// XXX only grab the speakers (can this create a memory leak?)
			bRecs[bdyCount].m_body = b;
			bRecs[bdyCount].m_priority = BR_SPEAKER;
			bdyCount++;
		}
	}

	OrderAvatars(bRecs, bdyCount, placed);

	for (int i = 0; i < bdyCount; i++) {
		CBody *b = ((CBodyRecord *)(placed[i]))->m_body;
		b->GetDimInfo(width[i], height[i], normHeight[i], headHeight[i], bitArrowX);
		arrowX[i] = ((double) bitArrowX) / width[i];					// initially store arrows as percentage of width from left
		maxNorm = max(maxNorm, normHeight[i]);
	}

	for (i = 0; i < bdyCount; i++) {
		// scale all of them such that maxHeight == unitHeight / 2
		int newHeight  = ROUND(maxBodyHeight * ((float)normHeight[i] / maxNorm));
		float scaleRatio = (float)newHeight / height[i];
		height[i] = newHeight;
		width[i] = ROUND(scaleRatio * width[i]);
		top[i] = -CUnitPanelPage::unitHeight + height[i];
		headHeight[i] = ROUND(scaleRatio * headHeight[i]);
		bdyWidth += width[i];
	}

	sumWidth = bdyWidth + (bdyCount+1) * minMargin;

	double zoomFactor = 1.0;
	if (sumWidth > CUnitPanelPage::unitWidth) {
		// must reduce the size of the avatars
		float reduction = (float)CUnitPanelPage::unitWidth / sumWidth;
		bdyWidth = 0;
		for (i = 0; i < bdyCount; i++) {
			height[i] = ROUND(height[i] * reduction);
			width[i] = ROUND(width[i] * reduction);
			top[i] = -CUnitPanelPage::unitHeight + height[i];
			bdyWidth += width[i];
		}
		AdjustArtToCoord(0, 1.0);
	} else if (zoomIn && !Establishing()) {
		// increase size of avatars
		zoomFactor = (double)CUnitPanelPage::unitWidth / sumWidth;

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
	AdjustArtToCoord(-CUnitPanelPage::unitHeight + maxBodyHeight, zoomFactor);

	m_bodies.RemoveAll();

	int margin = (CUnitPanelPage::unitWidth - bdyWidth) / (bdyCount+1); // margins also between avs and borders
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

BOOL CUnitPanel::IsSpeaker(UINT avID) {
	if (GetAvatar(avID)->m_body->m_requested == TRUE) return TRUE;   // sort of makes them a speaker

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
	brect.right = CUnitPanelPage::unitWidth;
	brect.bottom = -CUnitPanelPage::unitHeight/2;
	if (m_hasBorder) {
		int penWidth = m_borderWidth;
		brect.left += penWidth;
		brect.right -= penWidth;
		brect.top -= penWidth;
	}

	return brect;
}

void ForceFitBalloon(CBalloon *balloon, RECT &freeRect, char **rest) {
	balloon->SetBBox(freeRect.left, freeRect.bottom, freeRect.right, freeRect.top);
	*rest = balloon->SplitHeight(freeRect.top - freeRect.bottom);  // need a fudge factor?
	if (balloon->m_bbox.Top > -250) balloon->DockAtTop(freeRect.top);
}

void ForceFitLabel(CLabel *label, RECT &freeRect, char **rest) {
	*rest = label->SplitHeight(freeRect.top - freeRect.bottom);
}

extern CArrow *foo, *oldfoo;

BOOL CUnitPanel::LayoutBalloons(char **rest) {
	int nb = 0;
	CBalloon *balloons[10];
	*rest = NULL;

	RECT freeRect = GetBalloonRect();
	POSITION pos = m_elements.GetHeadPosition();
	while (pos) {			// stash in an array for easy access
		CBalloon *nextBalloon = (CBalloon *) m_elements.GetNext(pos);
		balloons[nb++] = nextBalloon;
	}

	for (int i = 0; i < nb; i++)
		if (!LayoutBalloon(balloons, nb, i, freeRect)) {  // best guess for layout
			if (i == 0) {
				ForceFitBalloon(balloons[i], freeRect, rest);
				return TRUE;
			} else return FALSE;
	}

	return TRUE;
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

#define MINHOOKHEIGHT	100

int LowestPreviousBottom(CBalloon *balloons[], int index, int lowY) {
	for (int i = 0; i < index; i++)
		lowY = min(lowY, balloons[i]->m_bbox.Bottom);
	return lowY;
}

BOOL NoneToLeft(CBalloon *balloons[], int nb, int index) {
	if (index >= nb-1) return TRUE;
	int thisToLeft = balloons[index]->m_speaker->m_bbox.Left;
	for (int i = index+1; i < nb; i++) {
		if (balloons[i]->m_speaker->m_bbox.Left < thisToLeft)
			return FALSE;
	}

	return TRUE;
}

#define ONELINETHRESHOLD	500

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

void AssignRECTToSRECT(RECT &r, SRECT &s) {
	s.Left = (short) r.left;
	s.Right = (short) r.right;
	s.Bottom = (short) r.bottom;
	s.Top = (short) r.top;
}

void AdjustRouteRgns(CBalloon *balloons[], int index) {
	// subtract out index's routeRgn from other routeRgns
	int left = balloons[index]->m_routeRgn.Left;
	int right = balloons[index]->m_routeRgn.Right;
	int toX = balloons[index]->m_speaker->m_arrowX;
	
	for (int i = 0; i < index; i++) {
		balloons[i]->SetRouteRgn(toX, left, right);
	}
}

BOOL CUnitPanel::LayoutBalloon(CBalloon *balloons[], int nb, int index, RECT& freeRect) {
	BOOL itFit;
	RECT brect;

	GetCloudEstimate(balloons, nb, index, freeRect, brect);
	itFit = GetInterveningBBox(balloons, index, freeRect, brect);
	if (!itFit) {
		return FALSE;
	}

	CBalloon *balloon = balloons[index];
	if (!balloon->SetBBox(brect.left, brect.bottom, brect.right, brect.top))
		return FALSE;	// couldn't build a balloon with this text and size
	if (balloon->m_bbox.Top > -250) balloon->DockAtTop(freeRect.top);
	balloon->GetCloudBBox(&balloon->m_routeRgn);	// y not significant (note: this doesn't exactly provide disjoint routeRgns.  Fix)
	if (balloon->m_routeRgn.Bottom < freeRect.bottom + MINHOOKHEIGHT)
		return FALSE;
	AdjustRouteRgns(balloons, index);
	return TRUE;
}

void CUnitPanel::RearrangeBalloons(CBalloon *balloons[], int nb, RECT& freeRect) {
}

#if 0
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
#endif

extern BOOL printBMP;
void PrintBMP();
CDC *pnlDC = NULL;		// For debugging purposes only

void CUnitPanelPage::Draw(CDC *dc, POINT *, RECT *damage = NULL) {
	// First set up rDC for retained dib section
	RECT panelRect;
	RECT damageRel;			// damage rectangle in panel coords
	HBITMAP GetRetSec(CDC*);
	CDC memDC;
	pnlDC = &memDC;
	VERIFY(memDC.CreateCompatibleDC(dc));
	memDC.m_bPrinting = dc->m_bPrinting;	// this determines which set of bitmaps to use
	memDC.SetMapMode(dc->GetMapMode());
	HBITMAP retSec;
	VERIFY(retSec = GetRetSec(dc));
	CBitmap temp;
	CBitmap *retCBit = temp.FromHandle(retSec);
	CBitmap *bmpOld = memDC.SelectObject(retCBit); // must use a CBitmap

	int truePanelsPerRow = dc->IsPrinting() ? printPanelsPerRow : panelsPerRow;
	SetRect(&panelRect, 0, 0, unitWidth, -unitHeight);
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
				POINT loc2 = AccountForScroll(&loc, TRUE, TRUE, dc->m_bPrinting);
				VERIFY(dc->BitBlt(loc2.x, loc2.y, unitWidth, -unitHeight, &memDC, 0, 0, SRCCOPY));
			} else PrintBMP();
		}
		if (panelCount % truePanelsPerRow == 0) {
			loc.x = 0;
			loc.y -= unitHeight + hInterstice;
		} else
			loc.x += unitWidth + vInterstice;
	}
	memDC.SelectObject(bmpOld);
}


// note: rect guaranteed to be normalized
void CUnitPanelPage::GetBBox(RECT *rect) {
	rect->left = m_leftX;
	rect->top = m_topY;
	int nPanels = m_panels.GetCount();
	int nRows = (nPanels - 1) / panelsPerRow + 1;  // note: assumption that nPanels > 0
	int nColumns = min(nPanels, panelsPerRow);
	rect->right = m_leftX + nColumns * unitWidth + (nColumns - 1)*vInterstice;
	rect->bottom = m_topY - (nRows * unitHeight + (nRows - 1)*hInterstice);
}

// picks a title at random from title.txt: returns a malloced char *
char *GetRandomTitle() {
	char line[100];
	int nTitles;
	FILE *fp;
	CString path = theApp.GetBaseDir();
	path += "\\titles.txt";

	VERIFY(fp = fopen(path, "r"));
	if (!fgets(line, sizeof(line), fp)) {
		fclose(fp);
		return "UNTITLED";
	}
	sscanf(line, "%d", &nTitles);
	if (nTitles < 1) {
		fclose(fp);
		return "EMPTY TITLE FILE";
	}
	int chosenTitle = (int)(randfloat() * nTitles);
	chosenTitle = min(chosenTitle, nTitles-1);
	for (int i = 0; i <= chosenTitle; i++)
		if (!fgets(line, sizeof(line), fp)) {
			fclose(fp);
			return "INDEX OFF!";
	}

	fclose(fp);
	char *nl = strchr(line, '\n');
	if (nl) *nl = '\0';	 // null out newline
	return strdup(line);
}


void CUnitPanelPage::AddTitle() {
	char *GetComicsTitle();
	char *title = GetComicsTitle();
	CString starringStr;
	RECT border;

	CLabel *titleL = new CLabel(title, m_fiTitle);
	titleL->SetBBox(0, -unitHeight/2, unitWidth, -100);
	titleL->GetBBox(&border);
	starringStr.LoadString(ID_STARRING);
	CLabel *starringL = new CLabel(starringStr, m_fiShout);
	starringL->SetBBox(0, -unitHeight, unitWidth, border.bottom);
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
		AddTitle();
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

// eventually will prepend name
void FormatInfo(CString &formatted, const char *info) {
	formatted = info;
	formatted.MakeUpper();
}

#define INFOMARGIN	10
#define MAXINFOTEXTHEIGHT	.5		// as a percentage of pane

void CUnitPanelPage::ShowInfo(unsigned short avID, const char *info) {
	CString formatted;
	FormatInfo(formatted, info);
	int maxBoxHeight = (int) (unitHeight * MAXINFOTEXTHEIGHT);

	while (TRUE) {
		CUnitPanel *newPanel = new CUnitPanel;
		int margin = newPanel->m_borderWidth + INFOMARGIN;
		char *rest = NULL;
		CLabel *box = new CLabel(formatted, m_fiWNormal);
		box->m_format |= FT_LEFT_JUSTIFY;
		newPanel->m_elements.AddTail(box);
		int top = (int)(-100 * ((float)unitHeight/4860));
		int bottom = top - maxBoxHeight;
		box->SetBBox(margin, bottom, unitWidth-margin, top);
		RECT border;
		box->GetBBox(&border);
		if (border.bottom < bottom) {
			rest = box->SplitHeight(maxBoxHeight);
			box->GetBBox(&border);
		}
		// center text box (text inside box is ragged-right)
		int boxWidth = border.right - border.left;
		int newLeftX = (unitWidth - boxWidth) / 2;
		box->SetBBox(newLeftX, border.bottom, newLeftX + boxWidth, border.top);

		CBody *bdy = GetAvatar(avID)->GetBodyFromEmotion(CEmotion(0.0, 0.0));
		bdy->SetBBox(margin, -unitHeight, unitWidth-margin, border.bottom);
		newPanel->m_bodies.AddTail(bdy);

		newPanel->m_backDrop.m_backID = 0;			// no background for now
		AddPanel(newPanel);
		StartNewPanel();							// no adding to this one!
		if (!rest) break;
		formatted = rest;
		free(rest);
	}
}

void AddStarsAux(CPtrArray &stars, int maxStars) {
	int inserted, GetAvatarUpperBound();
	extern CMapStringToPtr mapNickToPtr;
	void *p;
	CString nick;
	extern CUserInfo *puiSelf;

	POSITION pos = mapNickToPtr.GetStartPosition();
	while (pos) {
		mapNickToPtr.GetNextAssoc(pos, nick, p);
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

	// add duplicates as necessary to fill out panel...  (but for now, only if standalone...)
	if (ChatGetConnectionStatus() != CX_DISCONNECTED) return;
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
}


#define ICONSIZE	500
#define ICONSPACE	100
#define BELOWSTARRING	300
#define ROWHEIGHT	500

void CUnitPanelPage::AddStars(CUnitPanel *panel, int topY) {
	CPtrArray stars;
	CPtrArray sLabels;
	if (MyAvatarID() == 0) return;	// not registered yet
	CAvatarX *myAv = MyAvatar();
	int GetAvatarUpperBound();
	CString avatarCredit;

	avatarCredit.LoadString(ID_AVATAR_CREDIT);

	int lineHeight = m_fiShout->m_lineHeight;
	int rowHeight = max(ICONSIZE, lineHeight);
	topY = topY - (BELOWSTARRING*unitHeight/4860);
	int maxStars = (unitHeight + topY) / rowHeight;
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
			CString temp = avatarCredit;
			const char *nickName, *avName;
			
			CAvatarX *star = (CAvatarX *)stars[i];
			star->GetAvatarName(&avName, &nickName);
			VERIFY(ReplaceToken(temp, CString("%1"), nickName));
			VERIFY(ReplaceToken(temp, CString("%2"), avName));

			CLabel *label = new CLabel(temp, m_fiShout);
			label->SetBBox(0, -unitHeight, unitWidth, 0);
			label->GetBBox(&bbox);
			sLabels.Add(label);
			maxWidth = max(maxWidth, bbox.right - bbox.left);
		}

		maxWidth += ICONSIZE + ICONSPACE;
		int iconOffset = (unitWidth - maxWidth) / 2;
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
			((CLabel *)sLabels[i])->SetBBox(textOffset, topY+textVdisp, unitWidth, topY+lineHeight+textVdisp);
			panel->m_elements.AddTail((CLabel *)sLabels[i]);
			topY -= rowHeight;
		}
	}
}


//#define PAGEHEIGHT ((int)(10.5 * 1440)) // printable page 10.5" high, 1440 units/inch
//#define PAGEWIDTH  ((int)(8.15 * 1440))	// printable page 8.15" wide, 1440 units/inch

void CUnitPanelPage::PageSizeInPanels(CPrintInfo *pInfo, int &panelsWide, int &panelsHigh) {
	int pageWidth = pInfo->m_rectDraw.right - pInfo->m_rectDraw.left;
	int pageHeight = pInfo->m_rectDraw.top - pInfo->m_rectDraw.bottom;

	// panelsHigh equation below derives from the fact that
	// panelsHigh * unitHeight + (panelsHigh - 1) * hInterstice < pageHeight
	panelsHigh = (pageHeight + hInterstice) / (unitHeight + hInterstice);
	panelsWide = (pageWidth + vInterstice) / (unitWidth + hInterstice);
}

void CUnitPanelPage::PreparePrintDC(CDC *pDC, CPrintInfo *pInfo, int pageNum) {
	CPoint vpOrigin;
	RECT clipRect;
	int panelsWide, panelsHigh, startPanelRow, mapY;

	int pageWidth = pInfo->m_rectDraw.right - pInfo->m_rectDraw.left;
	int pageHeight = pInfo->m_rectDraw.top - pInfo->m_rectDraw.bottom;

	PageSizeInPanels(pInfo, panelsWide, panelsHigh);
	printPanelsPerRow = panelsWide;				// here we cache this value (side effect, oh well)

	vpOrigin.x = (pageWidth - (panelsWide * unitWidth + (panelsWide - 1) * vInterstice)) / 2;
	vpOrigin.y = -(pageHeight - (panelsHigh * unitHeight + (panelsHigh - 1) * hInterstice)) / 2;
	// now calculate the y that should be mapped to vpOrigin.y
	startPanelRow = (pageNum-1) * panelsHigh;
	mapY = startPanelRow * (unitHeight + hInterstice);
	// pDC->SetWindowOrg(-vpOrigin.x, -(mapY - vpOrigin.y));
	SetPrintOffset(-vpOrigin.x, -mapY - vpOrigin.y);

	clipRect.left = vpOrigin.x;
	clipRect.top = vpOrigin.y;
	clipRect.right = clipRect.left + panelsWide * unitWidth + (panelsWide-1) * vInterstice;
	clipRect.bottom = clipRect.top - panelsHigh * unitHeight - (panelsHigh-1) * hInterstice;
	pDC->IntersectClipRect(&clipRect);
}

int CUnitPanelPage::GetPhysicalPageCount(CPrintInfo *pInfo) {
	int panelsWide, panelsHigh, panelCount;

	PageSizeInPanels(pInfo, panelsWide, panelsHigh);
	panelCount = m_panels.GetCount();
	return ((int) ceil((double) panelCount / (panelsWide * panelsHigh)));
}


int CUnitPanelPage::panelsPerRow =		2;
int CUnitPanelPage::printPanelsPerRow =	0;		// set in CPage::PreparePrintDC
int CUnitPanelPage::panelsPerColumn =	-1;    // negative value means no limit (page never ends)
int CUnitPanelPage::unitWidth =			MINUNITPANELWIDTH-1;  // triggers a resize if not overridden
int CUnitPanelPage::unitHeight =		MINUNITPANELHEIGHT-1;
int CUnitPanelPage::hInterstice =		144;
int CUnitPanelPage::vInterstice =		144;

int CUnitPanel :: m_borderWidth =		60;	  // I hope these initialize in order!
CPen CUnitPanel :: m_borderPen (PS_SOLID, 2*CUnitPanel::m_borderWidth, RGB(0,0,0));

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

void AddTalkTos(CBodyRecord bdys[], int &recCount) {
	int initialCount = recCount;
	for (int i = 0; i < initialCount; i++) {
		CAvatarX *av = GetAvatar(bdys[i].m_body->m_avatarID);
		int nTalkTos = av->m_talkTo.GetUpperBound() + 1;
		for (int j = 0; j < nTalkTos; j++) {
			if (recCount >= 5) return;		// don't add more than 5 people to the panel!!!
			int duplicate = FALSE;
			for (int k = 0; k < recCount; k++) {
				if (av->m_talkTo[j] == bdys[k].m_body->m_avatarID) {
					duplicate = TRUE;
					break;
				}
			}
			if (!duplicate) {
				CAvatarX *theirAv = GetAvatar(av->m_talkTo[j]);
				CEmotion neutral(0.0, 0.0);
				bdys[recCount].m_body = theirAv->GetBodyFromEmotion(neutral);
				bdys[recCount++].m_priority = BR_GOODIDEA;
			}
//			AddLookAt(av->m_talkTo[j], bdys[i]->m_avatarID, TRUE);
		}
	}
}

int EvalPair(CBodyRecord &b1, CBodyRecord &b2, int deltaPlacement) {
	int rating = 0, desiredDir;
	if (deltaPlacement > 0)
		desiredDir = FALSE;
	else {
		desiredDir = TRUE;
		deltaPlacement = -deltaPlacement;
	}

	CAvatarX *av1 = GetAvatar(b1.m_body->m_avatarID);
	int nTalkTos = av1->m_talkTo.GetUpperBound() + 1;
	if (nTalkTos == 0) {
		if (b1.m_body->m_flip != desiredDir)  // talking to world, but I'm not facing other's direction
			rating += 4;
		if (b2.m_body->m_flip == desiredDir)  // talking to world, but he's not facing my direction
			rating += 2;
	} else {
		UINT b2ID = b2.m_body->m_avatarID;
		for (int i = 0; i < nTalkTos; i++) {
			if (av1->m_talkTo[i] == b2ID) {
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

int EvalPlacement(CPtrArray &bdyArray, int nPlaced, CBodyRecord &bdy, int index, int &dir) {
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
	

void DoGreedyOrdering(CBodyRecord bdys[], int recCount, CPtrArray &bdyArray) {
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

/* BEGIN BLOCK FOR SIGGRAPH PIX */
UINT bdyOrder[] = {2, 1};						// FOR SIGGRAPH PIX
BOOL dirOrder[] = {FALSE, TRUE};				// FOR SIGGRAPH PIX

extern int breaknum;

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

		
BOOL OrderAvatars(CBodyRecord bdys[], int &recCount, CPtrArray &placed) {
	placed.SetSize(0, 5);
	if (recCount < 5) AddTalkTos(bdys, recCount);
	DoGreedyOrdering(bdys, recCount, placed);
//	DoForcedOrdering(bdys, recCount, placed);		// FOR SIGGRAPH PIX

	return TRUE;
}

void UpdateHistoresis(CPtrArray &placed, int nPlaced) {
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

void CUnitPanel::AdjustArtToCoord(int fixedY, double zoomFactor) {
	if (m_backDrop.m_mode == BF_NOZOOM) zoomFactor = 1.0;

	int logHeight = ROUND(CUnitPanelPage::unitHeight / zoomFactor);
	int logWidth = ROUND(CUnitPanelPage::unitWidth / zoomFactor);
	int newFixedY = ROUND(fixedY / zoomFactor);
	int delta = fixedY - newFixedY;
	m_backDrop.SetBBox(0, -logHeight + delta, logWidth, delta);
}

