#include "stdafx.h"
#include "chat.h"

#include "traj.h"
#include "spline.h"
#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"
#include "balloon.h"
#include "backdrop.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "pageview.h"
#include "panel.h"

double randfloat();
int CheckWord(const char *buff, const char *substr);

void AddSemantics(CPanel *p, const char *words) {
#if 0
	if (CheckWord(words, "Ohio")) {
		double r = randfloat();
		p->m_backDrop.m_mode = BF_NOZOOM;
		if (r < 0.0) {
			p->m_backDrop.m_mode = BF_NOZOOM;
			p->m_backDrop.m_backID = 9;
		} else if (r < 0.0) {
			p->m_backDrop.m_mode = BF_NOZOOM;
			p->m_backDrop.m_backID = 13;
		}
	}
#endif
}

void HackLeft(CPanel *p) {
	POSITION pos;
	pos = p->m_elements.GetHeadPosition();
	int minX = 4900;
	while (pos) {
		CBalloon *b = (CBalloon *) p->m_elements.GetNext(pos);
		RECT r;
		b->GetBBox(&r);
		minX = min(r.left, minX);
	}

	pos = p->m_bodies.GetHeadPosition();
	while (pos) {
		CBody *b = (CBody *) p->m_bodies.GetNext(pos);
		RECT r;
		b->GetBBox(&r);
		minX = min(r.left, minX);
	}

	pos = p->m_elements.GetHeadPosition();
	while (pos) {
		CBalloon *b = (CBalloon *) p->m_elements.GetNext(pos);
		RECT r;
		b->GetBBox(&r);
		r.left -= minX;
		r.right -= minX;
		b->SetBBox(r.left, r.bottom, r.right, r.top);
		b->m_routeRgn.Left -= minX;
		b->m_routeRgn.Right -= minX;
	}

	pos = p->m_bodies.GetHeadPosition();
	while (pos) {
		CBody *b = (CBody *) p->m_bodies.GetNext(pos);
		RECT r;
		b->GetBBox(&r);
		r.left -= minX;
		r.right -= minX;
		b->m_arrowX -= minX;
		b->SetBBox(r.left, r.bottom, r.right, r.top);
	}
}

#if 0
void PostSemantics(CPanel *p, const char *words) {
	if (CheckWord(words, "OXio")) {
		CAvatarX *av = GetAvatar(10);
		CBody *bdy = av->GetBodyFromEmotion(CEmotion(0.0, 0.0));		// SIGGRAPH PIX Hack
		HackLeft(p);
		bdy->SetBBox(3500, -4860, 4200, -4000);
		bdy->m_flip = TRUE;
		bdy->m_arrowX = (bdy->m_bbox.Left + bdy->m_bbox.Right) / 2;
		p->m_bodies.AddTail(bdy);
		CBalloon *newBalloon = new CBWoodringMini("What's round on the ends and hi in the middle?", NULL);
		newBalloon->m_speaker = bdy;
		newBalloon->SetBBox(3000, -4000, 4800, -2800);
		RECT bbox;
		newBalloon->GetCloudBBox(&bbox);
		newBalloon->m_routeRgn.Left = (short) bbox.left;
		newBalloon->m_routeRgn.Right = (short) bbox.right;
	    p->m_elements.AddTail(newBalloon);	// add balloon to panel
		p->m_backDrop.m_mode = BF_NOZOOM;
	}
}
#endif
