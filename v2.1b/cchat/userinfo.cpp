#include "stdafx.h"
#include "chat.h"
#include "userinfo.h"

#include "bbox.h"
#include "pe.h"
#include "dib.h"
#include "avatar.h"

#include "binddoc.h"	// this group for SelectInMemberList
#include "chatdoc.h"
#include "memblst.h"

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// Types


// pointer to own userinfo
CUserInfo *puiSelf=NULL;


/////////////////////////////////////////////////////////////////////////////
// Support functions

CUserInfo::CUserInfo()
{
	m_uRequests		= 0;
	m_flags			= 0;
	m_avatarID		= 0;
	m_msgCount		= 0;
	m_intervalStart	= 0;
	m_bbValidUDI	= 0;
}


CUserInfo::CUserInfo(const char *attedNick, const char *fullName) {
	m_uRequests		= 0;
	m_flags			= 0;
	m_avatarID		= 0;
	m_msgCount		= 0;
	m_intervalStart	= 0;
	m_bbValidUDI	= 0;

	const char *nickname = attedNick;

	if (*attedNick == SC_HOST || *attedNick == SC_OWNER) {
		nickname++;
		SetOperator(TRUE);
	} else if (*attedNick == SC_SPECTATOR) {
		nickname++;
		SetFlag(UF_SPECTATOR, TRUE);
	} else if (*attedNick == SC_HASVOICE) {
		nickname++;
		SetFlag(UF_HASVOICE, TRUE);
	}

	if (nickname) SetName(nickname);
	if (fullName) {
		BOOL IsIgnored(const char *);
		m_fullName = fullName;
		Ignore(IsIgnored(fullName));
	}
}


CUserInfo::~CUserInfo()
{
}


void CUserInfo::GetAttedNick(CString &attedNick, BOOL bScreen) {
	if (IsOperator()) attedNick = SS_HOST;
	else if (CheckFlag(UF_SPECTATOR)) attedNick = SS_SPECTATOR;
	else attedNick = "";
	attedNick += bScreen ? GetScreenName() : GetName();
}


// Used to allow CUserInfo to remain undefined in several files
USHORT ExtractAvatarID(void *vdInfo) {
	CUserInfo *uInfo = (CUserInfo *) vdInfo;
	return (uInfo->GetAvatarID());
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

void CUserInfo::SetScreenName(const char *name) {
	if (name) {
		m_strScreenName = name;
		m_flags |= UF_SCREENNAME;
	}
	else
		m_flags &= ~UF_SCREENNAME;
}

CString & CUserInfo::GetScreenName() {
	if (m_flags & UF_SCREENNAME)
		return m_strScreenName;
	else
		return m_strName;
}

const char *CUserInfo::GetQualifiedName() {
	const char *nick = GetScreenName();
	static CString strQName;
	if (theApp.m_bShowIdentity && !m_fullName.IsEmpty()) {
		strQName.Format("%s (%s)", nick, m_fullName);
		return strQName;
	} else return nick;
}


void CUserInfo::IncrementRequestInfo(USHORT uRequest)
{
	USHORT	nCredits = m_uRequests & uRequest;

	if (uRequest == RF_PROFILE)
	{
		if (nCredits <= 12)
			m_uRequests += PROFILECREDITS;
		else
			m_uRequests |= 0x000F;	// max profile credits given
		return;
	}

	USHORT nIncrement = 0;

	switch (uRequest)
	{
		case RF_TIME:
			nIncrement = 0x0010;
			break;

		case RF_EMAIL:
			nIncrement = 0x0040;
			break;

		case RF_HOMEPAGE:
			nIncrement = 0x0100;
			break;

		case RF_NETMEETING:
			nIncrement = 0x0400;
			break;

		case RF_VERSION:
			nIncrement = 0x1000;
			break;

		default:
			ASSERT(FALSE);
	}

	if (nCredits < uRequest)
		m_uRequests += nIncrement;
	else
		m_uRequests |= uRequest;
}


void CUserInfo::DecrementRequestInfo(USHORT uRequest)
{
	if (!(m_uRequests & uRequest))
	{
		ASSERT(FALSE);
		return;
	}

	USHORT nDecrement = 0;

	switch (uRequest)
	{
		case RF_PROFILE:
			nDecrement = 0x0001;
			break;

		case RF_TIME:
			nDecrement = 0x0010;
			break;

		case RF_EMAIL:
			nDecrement = 0x0040;
			break;

		case RF_HOMEPAGE:
			nDecrement = 0x0100;
			break;

		case RF_NETMEETING:
			nDecrement = 0x0400;
			break;

		case RF_VERSION:
			nDecrement = 0x1000;
			break;

		default:
			ASSERT(FALSE);
	}

	m_uRequests -= nDecrement;
}


// necessary since code elsewhere needs to adjust pui's avatarID, and puiSelf is static
void SetMyPUIAvatarID(UINT avID) {
	if (puiSelf) {
		puiSelf->SetAvatarID((USHORT)avID);
		CAvatarX *av = GetAvatar(avID);
		av->m_userInfo = (void *) puiSelf;
	}
}


void SetUserAvatarID(CUserInfo *pui, unsigned short avID) {
	pui->SetAvatarID(avID);
	GetAvatar(avID)->m_userInfo = (void *) pui;
}

void CUserInfo::SelectInMemberList(CUserInfo *addressee, BOOL select, BOOL bExtend)
{
	if (!bExtend) {		// clear selection and focus if requested (is there an easier way?)
		int i;
		CMemberListCtrl *lstCtrl = &GetMembers()->m_MemberListBox;
		while ((i = lstCtrl->GetNextItem(-1, LVNI_SELECTED)) != -1)
			lstCtrl->SetItemState(i, 0, (LVIS_SELECTED | LVIS_FOCUSED));
		while ((i = lstCtrl->GetNextItem(-1, LVNI_FOCUSED)) != -1)
			lstCtrl->SetItemState(i, 0, (LVIS_SELECTED | LVIS_FOCUSED));
	}

	if (!addressee) return;

	TRACE("SelectInMemberList %s\n", addressee->GetName());

	int FindMemberListIndex(CUserInfo *, CChatDoc * = NULL);
	int index = FindMemberListIndex(addressee);
	if (index == -1) return;	// in case not found

	UINT state = GetMembers()->m_MemberListBox.GetItemState(index, LVIS_SELECTED | LVIS_FOCUSED );
	if (select) state |= LVIS_SELECTED | LVIS_FOCUSED;
	else state &= ~(LVIS_SELECTED | LVIS_FOCUSED);
	GetMembers()->m_MemberListBox.SetItemState( index, state, LVIS_SELECTED | LVIS_FOCUSED);
}

void MListTalkTosToPuiself() {
	if (!puiSelf) return;		// in case not in room... shouldn't do this
	puiSelf->ClearTalkTos();
	int i = -1;
	CMemberListCtrl *lstCtrl = &GetMembers()->m_MemberListBox;
	while ((i = lstCtrl->GetNextItem(i, LVNI_SELECTED)) != -1) {
		CUserInfo *pui = (CUserInfo *)(lstCtrl->GetItemData(i));
		ASSERT(!pui->IsSelf() || pui == puiSelf);	// sanity check
		if (!pui->IsSelf())
			puiSelf->m_udi.m_talkTos.Add((DWORD) pui);
	}
}



