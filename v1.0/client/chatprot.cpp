// Chat.cpp : Chat stuff
//

#include "stdafx.h"
#include "chat.h"

#include "binddoc.h"
#include "chatdoc.h"
#include "dib.h"
#include "script.h"
#include "pageview.h"
#include "mainfrm.h"

#include "chatprot.h"
#include "ui.h"
#include "bbox.h"
#include "pe.h"
#include "avatar.h"
#include "bodycam.h"
#include "userinfo.h"
#include "histent.h"

/////////////////////////////////////////////////////////////////////////////
// Parameters


/////////////////////////////////////////////////////////////////////////////
// Convenience macros

#define PI			3.1415926F
#define DEG2RAD(d)	(float)(d*PI/180.0F)

/////////////////////////////////////////////////////////////////////////////
// Types


// pointer to own userinfo
CUserInfo *puiSelf=NULL;

// IDLE1- every 30 seconds					
// IDLE2- every 10 seconds
// IDLE3- every 10 seconds


/////////////////////////////////////////////////////////////////////////////
// The one and only CClientApp object

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// Static objects with local scope

// REVIEW: hack
// CWAVSound  wavSong;
// CMIDISong  midiSong;

/////////////////////////////////////////////////////////////////////////////
// Forwards

// static void CHATCALLBACK MessageReady( CLID clid, PVOID pv );
static BOOL SendHeartbeat();
static void CALLBACK OnTimer(HWND hwnd, UINT nMsg, UINT nIDEvent, DWORD dwTime );
static void LogName( CUserInfo *pui );
void ShowSay( CUserInfo *pui, const char *psz, BOOL cooked, UCHAR mode = SM_SAY);
static void ShowThink( CUserInfo *pui, const char *psz, BOOL cooked);
static void ShowEmote( CUserInfo *pui, const char *psz, BOOL cooked);
static void ShowGesture( CUserInfo *pui, int);

/////////////////////////////////////////////////////////////////////////////
// Public functions

BOOL ChatInitialize()
{
	extern bCXKeepServer;

	if (!bCXKeepServer)
		InitializeServerConnection();
	else InitializeChannelConnection();

	return TRUE;
}


BOOL ChatGetAvatarName(CString& str)
{
	// if don't know self, just return failure
	if (puiSelf == NULL)
		return FALSE;

	str = puiSelf->GetName();

	return TRUE;
}

BOOL ChatSetAvatarName(CString &str)
{
	// if don't know self, just return success
	if (puiSelf == NULL)
		return TRUE;

	puiSelf->SetName(str);

	// set flag
//	bSendHeartbeat = TRUE;

	return TRUE;
}


BOOL ChatToggleIgnore( LPARAM lParam )
{
	return TRUE;
}

BOOL ChatFind( LPARAM lParam )
{
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// Support functions

CUserInfo::CUserInfo()
{
	m_clid = 0;
	m_flags = 0;
	m_avatarID = 0;
}

CUserInfo::~CUserInfo()
{
}

BOOL CUserInfo::Initialize(void *clid, const char *pch, int cch)
{
	if( pch != NULL )
		m_strName = CString( (LPCTSTR) pch, cch );
	m_clid = clid;
	return TRUE;
}


void CUserInfo::SetName( CString &strNew )
{
	m_strName = strNew;
}

void CUserInfo::GetAttedNick(CString &attedNick) {
	if (IsOperator()) attedNick = "@";
	else attedNick = "";
	attedNick += GetName();
}


// Used to allow CUserInfo to remain undefined in several files
USHORT ExtractAvatarID(void *vdInfo) {
	CUserInfo *uInfo = (CUserInfo *) vdInfo;
	return (uInfo->GetAvatarID());
}


void ShowSay( CUserInfo *pui, const char *psz, BOOL cooked, UCHAR mode)
{
	if (!pui) return;					// haven't made net connection yet!
	// don't show if ignored
	if( pui->Ignored() )
		return;
	void AddAndExecute(HistoryEntry *);
	if (theApp.m_bComicView) {
		UCHAR expr, gest, req;
		ASSERT(cooked);
		CAvatarX *av = GetAvatar(ExtractAvatarID(pui));
		av->GetIndices(expr, gest, req);
		// for now, take the hit that SayEntry will just set expr, gest, and req again!
		AddAndExecute(new SayEntry(pui, psz, cooked, mode, expr, gest, 0, 0, 0, 0, req, av->m_talkTo));
	} else {
		CWordArray empty;
		AddAndExecute(new SayEntry(pui, psz, cooked, mode, 0, 0, 0, 0, 0, 0, FALSE, empty));
	}
}

void ShowSayX(const char *psz, BOOL cooked, UCHAR mode) {
	ShowSay(puiSelf, psz, cooked, mode);
}


static void ShowThink( CUserInfo *pui, const char *psz, BOOL cooked)
{
	ShowSay(pui, psz, cooked, SM_THINK);   // pretend this is whisper for now
}

static void ShowEmote( CUserInfo *pui, const char *psz, BOOL cooked)
{
	ShowSay(pui, psz, cooked, SM_WHISPER);
}


BOOL ReplaceToken( CString& str, const CString& strToken, const CString& strValue )
{
	CString strResult;
	int iIndex;

	// find the token in the string
	iIndex = str.Find(strToken);
	if( iIndex < 0 )
		return FALSE;

	// grab the first part
	strResult = str.Left(iIndex);

	// add the replacement
	strResult += strValue;

	// add the second part
	strResult += str.Mid(iIndex + strToken.GetLength());

	// put back in the right place
	str = strResult;

	return TRUE;
}

// necessary since code elsewhere needs to adjust pui's avatarID, and puiSelf is static
void SetMyPUIAvatarID(UINT avID) {
	if (puiSelf) {
		puiSelf->SetAvatarID((USHORT)avID);
		CAvatarX *av = GetAvatar(avID);
		av->m_userInfo = (void *) puiSelf;
	}
}

void SetUserAvatar(CUserInfo *pui, char *avName) {
	CAvatarX *av = GetAvatar3(avName, pui);
	if (!av) return;  // don't change avatar if new one can't be found

	pui->SetAvatarID(av->m_avatarID);
	av->m_userInfo = (void *) pui;
}

void SetUserAvatarID(CUserInfo *pui, unsigned short avID) {
	pui->SetAvatarID(avID);
	GetAvatar(avID)->m_userInfo = (void *) pui;
}
