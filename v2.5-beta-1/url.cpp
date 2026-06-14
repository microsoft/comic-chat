#include "stdafx.h"
#include "url.h"
#include "..\inc\urlutil.h"

#include "chat.h"	// just so we can tell if it's embedded...
#include "userinfo.h"
#include "chatprot.h"
#include "binddoc.h"
#include "chatDoc.h"
#include "ui.h"		// for GetPrimaryView

#define MAX_URL_INTEXT	6

extern CChatApp theApp;

COLORREF linkColor = RGB(0, 0, 255);
CUrlRec g_urlRec;

void DestroyLinks(CPtrArray **links) {
	if (!*links) return;
	int nLinks = (*links)->GetUpperBound();
	for (int i = 0; i <= nLinks; i++) {
		CLink *link = (CLink *) ((**links)[i]);
		delete link;
	}
	delete (*links);
	*links = NULL;
}

CPtrArray *CopyLinks(CPtrArray *links) {
	if (!links) return NULL;
	CPtrArray *newLinks = new CPtrArray;
	int upper = links->GetUpperBound();
	for (int i = 0; i <= upper; i++) {
		CLink *unit = (CLink *) (*links)[i];
		CLink *newUnit = new CLink(*unit);
		newLinks->Add(newUnit);
	}
	return newLinks;
}


CPtrArray *AddLink(CPtrArray *links, const char *url, int start, int length) {
	if (!links) links = new CPtrArray;
	CLink *unit = new CLink(start, length, url);
	links->Add(unit);
	return links;
}


CPtrArray *IdentifyURLs(const char *mesg) {
	int nUrlBounds[MAX_URL_INTEXT*2];
	int	nUrlNum = MAX_URL_INTEXT;

	CPtrArray *links = NULL;
	const char *start = mesg;
	if (g_urlRec.HrIdentifyUrls(mesg, nUrlBounds, &nUrlNum) != ERROR_SUCCESS) return NULL; // just say we didn't find any
	for (int i = 0; i < nUrlNum; i++) {
		int start = nUrlBounds[2*i];
		int end = nUrlBounds[2*i+1];
		int len = end - start;
		char *szURL = (char *) malloc (sizeof(TCHAR) * (len+1));
		strncpy(szURL, mesg + start, len);
		szURL[len] = '\0';
		links = AddLink(links, szURL, start, nUrlBounds[2*i+1] - start);
		free(szURL);
	}
	return links;
}


BOOL FLaunchBrowser(LPCTSTR cszURL) {
	return(g_urlRec.bLaunchUrl(GetFrame()->GetSafeHwnd(), cszURL, theApp.m_bEmbedded));
}




