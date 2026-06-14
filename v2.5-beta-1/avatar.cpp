#include "stdafx.h"

#include "chat.h"
#include "bbox.h"
#include "pe.h"				// only for CPanelElement definition?
#include "dib.h"
#include "userinfo.h"
#include "avatar.h"
#include "ui.h"
#include "vector2d.h"
#include "math.h"
#include "chatprot.h"
#include "memblst.h"
#include "binddoc.h"
#include "chatdoc.h"
#include <io.h>				// for avfile scan

extern CChatApp theApp;

void RefreshBodyCam(CAvatarX *newAv = NULL);
BOOL RefreshBodyPreview(CAvatarX *av);

CAvatarX::~CAvatarX() {
	TRACE("Deleting avatar: %s.\n", m_name);
	free(m_name);				// free avatar name
	if (m_body) delete m_body;	//   and avatar body
	int i = m_arrPoses.GetSize (); 
	while (i > 0) {
		delete m_arrPoses[--i];
	}
	if (m_pStream != NULL) {
		delete m_pStream;
	}
	free (m_pszOriginalURL);
	free (m_pszNewURL);
	free (m_pszCopyright);
}

CAvatarSimple::~CAvatarSimple() {
	if (m_origID == 0) {
		free(bRec);
	}
}

CAvatarComplex::~CAvatarComplex() {
	if (m_origID == 0) {
		free(fRec);
		free(bRec);
	}
}



// this assumes that m_poseID is also the avRec index
void CBodySingle::GetDimInfo(short &xdim, short &ydim, short &normHeight, short &headHeight, short &faceX) {
	CAvatarSimple *av = (CAvatarSimple *) GetAvatar(m_avatarID);

	CPose* pose;
	if (av->GetPoseFromID (GetPoseID(), &pose))
	{
		xdim = pose->GetDrawing ()->GetWidth();
		ydim = pose->GetDrawing ()->GetHeight();
		headHeight = ydim/2;	// for now, be conservative -- head = half body!
		normHeight = 100;       // rec->normHeight;  -- XXX right now, we're setting normHeight to a constant. Fix?!
	}
	else
	{
		xdim = ydim = 100;
		headHeight = 50;
		normHeight = 100;
	}

	faceX = m_bodyRec->faceX;
	if (m_flip) faceX = xdim - faceX;
}

void CBodyDouble::GetDimInfo(short &xdim, short &ydim, short &normHeight, short &headHeight, short &faceX) {
	CAvatarComplex *av = (CAvatarComplex *) GetAvatar(m_avatarID);

	// this code is adapted from GetBodyBox, does a little less, but is faster
	CPose* head;
	CPose* torso;
	SIZE sizeHead, sizeTorso;
	if (av->GetPosesFromIDs (m_faceRec->poseID, m_torsoRec->poseID, &head, &torso))
	{
		sizeHead.cx = head->GetDrawing ()->GetWidth ();
		sizeHead.cy = head->GetDrawing ()->GetHeight ();
		sizeTorso.cx = torso->GetDrawing ()->GetWidth ();
		sizeTorso.cy = torso->GetDrawing ()->GetHeight ();
	}
	else
	{
		sizeHead.cx = sizeHead.cy = 50;
		sizeTorso.cx = sizeTorso.cy = 50;
	}

	int xOffset = m_torsoRec->xCX + m_faceRec->delta_xCX - m_faceRec->xCX;
	int yOffset = m_torsoRec->yCX + m_faceRec->delta_yCX - m_faceRec->yCX;

	RECT bitRect;
	bitRect.left = min(0, xOffset);
	bitRect.right = max(sizeTorso.cx, xOffset + sizeHead.cx);
	bitRect.top = min(0, yOffset);
	headHeight = yOffset + sizeHead.cy;
	bitRect.bottom = max(sizeTorso.cy, headHeight);

	xdim = (short)(bitRect.right - bitRect.left);
	ydim = (short)(bitRect.bottom - bitRect.top);
	headHeight -= (short)bitRect.top;
	normHeight = 100;  /// XXX right now, we're assuming characters are the same height

	faceX = (short) (m_faceRec->faceX + xOffset - bitRect.left);
	if (m_flip) faceX = xdim - faceX;
}


CPose * CAvatarX::GetPoseFromID(
USHORT nID)
{
	// If not the original avatar, get it from the original avatar.
	if (m_origID != 0)
	{
		CAvatarX * pAvOrig = GetOriginalAvatar ();
		return pAvOrig != NULL ? pAvOrig->GetPoseFromID (nID) : NULL;
	}

	if (nID == 0 || nID > m_arrPoses.GetSize ()) {
		return NULL;
	}

	if (m_arrPoses[nID - 1]->m_pdibs[0] != NULL) {
		return m_arrPoses[nID - 1];
	}
	else if (m_pStream != NULL && m_arrPoses[nID - 1]->Load (m_pStream, &m_palette)) {
		return m_arrPoses[nID - 1];
	}
	else {
		return NULL;
	}
}

BOOL 
CAvatarSimple::GetPoseFromID(
USHORT nID, 
CPose * * pPose)
{
	*pPose = CAvatarX::GetPoseFromID (nID);
	if (*pPose == NULL)
	{
		// Substitute one if we can. On first pass, try to substitute a neutral one,
		// on second pass just pick anything we can get.
		for (int iPass = 0; iPass < 2; iPass++)
		{
			for (int i = 0; i < m_nBodies; i++)
			{
				if (iPass != 0 || (bRec[i].emotion == EM_NEUTRAL && bRec[i].intensity == 0.0))
				{
					if ((*pPose = CAvatarX::GetPoseFromID (bRec[i].poseID)) != NULL)
						return TRUE;
				}
			}
		}
		return FALSE;
	}
	return TRUE;
}

BOOL 
CAvatarComplex::GetPosesFromIDs(
USHORT nHeadID, 
USHORT nTorsoID,
CPose * * pHeadPose,
CPose * * pTorsoPose)
{
	BOOL bRet = TRUE;
	*pHeadPose = CAvatarX::GetPoseFromID (nHeadID);
	if (*pHeadPose == NULL)
	{
		// Substitute one if we can. On first pass, try to substitute a neutral one,
		// on second pass just pick anything we can get.
		for (int iPass = 0; iPass < 2; iPass++)
		{
			for (int i = 0; i < nFaces; i++)
			{
				if (iPass != 0 || (fRec[i].emotion == EM_NEUTRAL && fRec[i].intensity == 0.0))
				{
					if ((*pHeadPose = CAvatarX::GetPoseFromID (fRec[i].poseID)) != NULL)
						goto LoadTorso;
				}
			}
		}
		bRet = FALSE;
	}
LoadTorso:
	*pTorsoPose = CAvatarX::GetPoseFromID (nTorsoID);
	if (*pTorsoPose == NULL)
	{
		// Substitute one if we can. On first pass, try to substitute a neutral one,
		// on second pass just pick anything we can get.
		for (int iPass = 0; iPass < 2; iPass++)
		{
			for (int i = 0; i < nTorsos; i++)
			{
				if (iPass != 0 || (bRec[i].emotion == EM_NEUTRAL && bRec[i].intensity == 0.0))
				{
					if ((*pTorsoPose = CAvatarX::GetPoseFromID (bRec[i].poseID)) != NULL)
						return bRet;
				}
			}
		}
		return FALSE;
	}
	return bRet;
}

void CAvatarX::GetScreenName(const char **screenName) {
	CUserInfo *pui = (CUserInfo *) m_userInfo;
	ASSERT(pui);
	*screenName = (LPCTSTR) pui->GetScreenName();
}

CBody *CAvatarSimple::GetBodyFromEmotion(CEmotion &emotion) {
	CBodySingle *body = new CBodySingle(m_avatarID);

	// Distance metric needs rethinking!
	double nearestAngle = 3*PI;
	double intensityOfNearest = 2.0;
	int nearestI = -1;

	for (int i = 0; i < m_nBodies; i++) {
		int index = (m_lastBody + 1 + i) % m_nBodies;  // start search from index after last body used
		if (bRec[index].emotion > 7) continue;
		double thisAngle = fabs(subtract_angles(bRec[index].emotion, emotion.m_emotion));
		BOOL isFirstNeutral = bRec[index].emotion == EM_NEUTRAL && bRec[index].intensity == 0.0 && nearestI == -1; // consider first neutral if necessary
		if (thisAngle < PI / NEMOTIONS  || isFirstNeutral) {   // from 2PI / (2*NEMOTIONS), since can go half dist in each direction
			double delta_i;
			if (isFirstNeutral && emotion.m_intensity > 0.0) delta_i = 1.5; // less powerful than any correct match
			else delta_i = fabs(emotion.m_intensity - bRec[index].intensity);
			if (delta_i < intensityOfNearest) {
				nearestAngle = thisAngle;
				intensityOfNearest = delta_i;
				nearestI = index;
			}
		}
	}

	SetBody(body, nearestI);
	return body;	
}


CBody *CAvatarComplex::GetBodyFromEmotion(CEmotion &emotion) {
	CBodyDouble *body = new CBodyDouble(m_avatarID);

	// Distance metric needs rethinking!
	double nearestAngle = 3*PI;
	double intensityOfNearest = 2.0;
	int nearestI;

	for (int i = 0; i < nFaces; i++) {
		double thisAngle = fabs(subtract_angles(fRec[i].emotion, emotion.m_emotion));
		if (thisAngle <= nearestAngle) {
			double delta_i = fabs(emotion.m_intensity - fRec[i].intensity);
			if (thisAngle == nearestAngle && delta_i >= intensityOfNearest)
				continue;
			nearestAngle = thisAngle;
			intensityOfNearest = delta_i;
			nearestI = i;
		}
	}

	SetFace(body, nearestI);
//	TRACE("Face = %d.\n", nearestI);


	intensityOfNearest = 2.0;
	for (i = 0; i < nTorsos; i++) {
		int index = (m_lastTorso + 1 + i) % nTorsos;  // start search from index after last body used
		if (bRec[index].emotion > 7) continue;
		double thisAngle = fabs(subtract_angles(bRec[index].emotion, emotion.m_emotion));
//		TRACE("%d: input = %f, body = %f, thisAngle = %f\n", i, emotion.m_emotion, bRec[i].emotion, thisAngle);
		if (thisAngle < PI / NEMOTIONS  ||    // from 2PI / (2*NEMOTIONS), since can go half dist in each direction
			(bRec[index].emotion == EM_NEUTRAL && bRec[index].intensity == 0)) { // consider neutrals, too
			double delta_i = fabs(emotion.m_intensity - bRec[index].intensity);
			if (delta_i < intensityOfNearest) {
				intensityOfNearest = delta_i;
				nearestI = index;
			}
		}
	}
//	TRACE("Nearest I = %d.\n", nearestI);
	SetTorso(body, nearestI);
//	TRACE("TORSO = %d.\n", nearestI);

	return body;	
}

void CAvatarComplex::GetHeadAndBodyFromEmotion(CEmotion &emotion, int& fIndex, int& tIndex) {
	// Distance metric needs rethinking!
	double nearestAngle = 3*PI;
	double intensityOfNearest = 2.0;
	fIndex = tIndex = -1;

	if (emotion.m_emotion <= 2*PI) {		// Otherwise, can't use this metric
		for (int i = 0; i < nFaces; i++) {
			double thisAngle = fabs(subtract_angles(fRec[i].emotion, emotion.m_emotion));
			if (thisAngle <= nearestAngle) {
				double delta_i = fabs(emotion.m_intensity - fRec[i].intensity);
				if (thisAngle == nearestAngle && delta_i >= intensityOfNearest)
					continue;
				nearestAngle = thisAngle;
				intensityOfNearest = delta_i;
				fIndex = i;
			}
		}
	} else {
		for (int i = 0; i < nTorsos; i++) {
			if (emotion.m_emotion == bRec[i].emotion) {
				tIndex = i;
				break;
			}
		}
	}
}

void CAvatarSimple::GetBodyIndexFromEmotion(CEmotion &emotion, int& bIndex) {
	// Distance metric needs rethinking!
	double nearestAngle = 3*PI;
	double intensityOfNearest = 2.0;
	bIndex = -1;

	if (emotion.m_emotion <= 2*PI) {		// Otherwise, can't use this metric
		for (int i = 0; i < m_nBodies; i++) {
			double thisAngle = fabs(subtract_angles(bRec[i].emotion, emotion.m_emotion));
			if (thisAngle <= nearestAngle) {
				double delta_i = fabs(emotion.m_intensity - bRec[i].intensity);
				if (thisAngle == nearestAngle && delta_i >= intensityOfNearest)
					continue;
				nearestAngle = thisAngle;
				intensityOfNearest = delta_i;
				bIndex = i;
			}
		}
	} else {
		for (int i = 0; i < m_nBodies; i++) {
			if (emotion.m_emotion == bRec[i].emotion) {
				bIndex = i;
				break;
			}
		}
	}
}

CBody *CAvatarComplex::GetBodyFromEmotion(CEmotionOpts &emOpts) {
	int bestIndex, fIndex, tIndex, foundF = -1, foundT = -1;
	UCHAR minPriority;
	CBodyDouble *body = new CBodyDouble(m_avatarID);

	while (TRUE) {
		minPriority = 0;
		for (int i = 0; i < emOpts.m_nOpts; i++) {
			if (emOpts.m_priorities[i] > minPriority) {
				bestIndex = i;
				minPriority = emOpts.m_priorities[i];
			}
		}
		if (!minPriority) break;
		GetHeadAndBodyFromEmotion(emOpts.m_emotions[bestIndex], fIndex, tIndex);
		emOpts.m_priorities[bestIndex] = 0;   // nuke this entry, so we don't kill again
		if (fIndex >= 0 && foundF < 0) {
			SetFace(body, fIndex);
			foundF = fIndex;
		}
		if (tIndex >= 0 && foundT < 0) {
			SetTorso(body, tIndex);
			foundT = tIndex;
		}
		if (foundF >= 0 && foundT >= 0) break;	// filled constraints -- don't need to continue
	}
	if (foundF < 0) SetFaceNeutral(body);
	if (foundT < 0) SetTorsoNeutral(body);
//	TRACE("Face: %d.  Torso: %d.\n", body->m_faceRec - fRec, body->m_torsoRec - bRec);
	return body;
}

CBody *CAvatarSimple::GetBodyFromEmotion(CEmotionOpts &emOpts) {;
	int bestIndex, bIndex, foundB = -1;
	UCHAR minPriority;
	CBodySingle *body = new CBodySingle(m_avatarID);

	// find first exact match w/ highest priority
	while (TRUE) {
		minPriority = 0;
		for (int i = 0; i < emOpts.m_nOpts; i++) {
			if (emOpts.m_priorities[i] > minPriority) {
				bestIndex = i;
				minPriority = emOpts.m_priorities[i];
			}
		}
		if (!minPriority) break;
		GetBodyIndexFromEmotion(emOpts.m_emotions[bestIndex], bIndex);
		emOpts.m_priorities[bestIndex] = 0;   // nuke this entry, so we don't kill again
		if (bIndex >= 0 && foundB < 0) {
			SetBody(body, bIndex);
			foundB = bIndex;
			break;
		}
	}
	if (foundB < 0) SetBodyNeutral(body);
//	TRACE("Face: %d.  Torso: %d.\n", body->m_faceRec - fRec, body->m_torsoRec - bRec);
	return body;
}


void CAvatarComplex::SetTorsoNeutral(CBodyDouble *body) {
	int c = m_lastTorso;
	for (int i = 0; i < nTorsos; i++) {
		c = (c+1) % nTorsos;
		if (bRec[c].emotion == EM_NEUTRAL && bRec[c].intensity == 0.0) {
			SetTorso(body, c);
			return;
		}
	}

	SetTorso(body, 0);		// Oh well, just set it to first
}

void CAvatarComplex::SetFaceNeutral(CBodyDouble *body) {
	int c = m_lastFace;
	for (int i = 0; i < nFaces; i++) {
		c = (c+1) % nFaces;
		if (fRec[c].emotion == EM_NEUTRAL && fRec[c].intensity == 0.0) {
			SetFace(body, c);
			return;
		}
	}

	SetFace(body, 0);		// Oh well, just set it to first
}

void CAvatarSimple::SetBodyNeutral(CBodySingle *body) {
	int c = m_lastBody;
	for (int i = 0; i < m_nBodies; i++) {
		c = (c+1) % m_nBodies;
		if (bRec[c].emotion == EM_NEUTRAL && bRec[c].intensity == 0.0) {
			SetBody(body, c);
			return;
		}
	}

	SetBody(body, 0);		// Oh well, just set it to first
}

void ResetAvatar(int avID) {
	CAvatarX *av = GetAvatar(avID);
	if (av->m_freeze == AF_TEMPFROZEN)		// Reset temporary freezes to unfrozen
		av->m_freeze = AF_UNFROZEN;
	if (av->m_freeze == AF_UNFROZEN)		// Make unfrozen avatars neutral
		av->SetNeutral();
}


void CAvatarComplex::SetNeutral() {
	CBodyDouble *body = new CBodyDouble(m_avatarID);
	SetTorsoNeutral(body);
	SetFaceNeutral(body);
//	body->m_requested = FALSE;
	UpdateBody(body);
}

void CAvatarSimple::SetNeutral() {
	CBody *body = GetBodyFromEmotion(CEmotion(0.0, 0.0)); // creates the only body that's known...
//	body->m_requested = FALSE;
	UpdateBody(body);
}

void CAvatarX::UpdateBody(CBody *newBody) {
	if (newBody->IsSame(m_body)) 
		delete newBody;
	else {
		if (m_body) delete m_body;
		m_body = newBody;

		if (!RefreshBodyPreview(this)) // will check for validity
			if (m_avatarID == MyAvatarID())
				RefreshBodyCam();
	}
}


void CAvatarComplex::DifferentTorso(int torsoIndex) {
	if (m_body->GetClass() == BC_BODYDOUBLE) {
		CBodyDouble *newBody = new CBodyDouble;
		*newBody = *((CBodyDouble *) m_body);
		newBody->m_torsoRec = bRec + torsoIndex;
		UpdateBody(newBody);
	}
}


//UINT myAvatarID = 0;

static CPtrArray avatars;   // + 1 for NULL

int GetAvatarUpperBound () inline {
	return avatars.GetUpperBound();
}

void InitializeAvatars() {
	avatars.SetSize(0, 100);
	avatars.Add(NULL);				// first entry is NULL
}

void DestroyAvatars() {
	for (int i = GetAvatarUpperBound(); i >= 1; i--) {
		CAvatarX *av = (CAvatarX *) avatars[i];
		if (av) delete av;
	}

	avatars.SetSize(1);  // First element, NULL, retained
}

CAvatarX *LoadAvatar(const char *avName) {
	// for now, assumes it's not been loaded!
	CAvatarX *LoadAvatarInfo(const char *);
	CAvatarX *av = LoadAvatarInfo(avName);
	if (!av) return NULL;    // invalid avName
	return(av->IndexAvatar());
}

CAvatarX *CAvatarX::IndexAvatar() {
	m_avatarID = avatars.Add(this);
	SetNeutral();
	return this;
}


CAvatarX *MyAvatar() {
	if (GetChatDoc() && GetChatDoc()->m_myAvatarID != 0) 
		return ((CAvatarX *) avatars[GetChatDoc()->m_myAvatarID]);
	else return NULL;
}

const char *MyAvatarURL() {
	return MyAvatar ()->Url ();
}

UINT MyAvatarID() {
	if (GetChatDoc()) return GetChatDoc()->m_myAvatarID;
	else return 0;
}

#if 0
	avatars[10] = MakeAvatar_Simple("GreekChorus", "kibbitz", 10, 0, 129, 97, 40);


	// Give all of the avatars a neutral expression by default.  Note that this has to
	// be done after setting avatars[] array, since SetNeutral may refresh BodyCam, which
	// may require a BodyDraw, which may require a GetAvatar, which may require the avatars
	// array to be set.
	for (int i = 1; i <= NAVATARS; i++) {
		avatars[i]->SetNeutral();
		avatars[i]->m_lastRight = i-1;
		avatars[i]->m_lastLeft = i+1;
		avatars[i]->m_lastDir = TRUE;  // FALSE - SIGGRAPH PIX
	}
	
}
#endif

CAvatarX *GetAvatar(USHORT avatarID) {
	return ((CAvatarX *) avatars[avatarID]);
}

CAvatarX *GetAvatar(const char *name) {		// perhaps should have a hash table to map names to avatars
	int upperBound = avatars.GetUpperBound();
	for (int i = 1; i <= upperBound; i++) {
		if (!stricmp(((CAvatarX *) avatars[i])->m_name, name))
			return ((CAvatarX *) avatars[i]);
	}
	return NULL;
}

void SetMyAvatar(UINT avID, BOOL bBroadcast) {
	void SetMyPUIAvatarID(UINT);		// sets the avatarID in my CUserInfo
	extern void SetMyCharacter(const char *);

	if (avID != MyAvatarID()) {
		ASSERT(GetChatDoc());
		GetChatDoc()->m_myAvatarID = avID;
		SetMyPUIAvatarID(avID);
		CAvatarX *av = MyAvatar();
		RefreshBodyCam(av);
		const char *originalName = av->OriginalName();
		SetMyCharacter(originalName);	    // duplicated info -- arg.
		if (!theApp.m_bNoRefresh && bBroadcast)
			GetChatDoc()->m_proto->ChatAnnounceNewAvatar(originalName, av->Url ());
	}
}

BOOL SetMyAvatar(const char *szAvName, BOOL bBroadCast)
{
	ASSERT(GetChatDoc());

	GetChatDoc()->m_myAvatarID = 0;  // force reset in SetMyAvatar(UINT)

	CAvatarX *pAv = GetAvatar3(szAvName, g_puiSelf);
	ASSERT(pAv);

	SetMyAvatar(pAv->m_avatarID, bBroadCast);
	return TRUE;
}

static int nextAvatarName = -1;
static CStringArray strFiles;

void GetAllAvatarNames() {
// build file search strings
    CString strPattern;
	struct _finddata_t fd;
	long hFind;

    strFiles.RemoveAll();
	strPattern = theApp.GetAvatarDir();
	strPattern += "\\*.avb";
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
		_findclose (hFind);
	}
}

void ResetAvatarNames() { nextAvatarName = -1; }   // force avatar name list reset
	
void GetNextAvatarName(char *avName) {
	if (nextAvatarName == -1)
		GetAllAvatarNames();
	int upperBound = strFiles.GetUpperBound();
	if (upperBound == -1) {
		strcpy(avName, "_NoArt");
		return;
	}
	nextAvatarName++;
    if (nextAvatarName > upperBound)
		nextAvatarName = 0;
	if (MyAvatar() && !stricmp(strFiles[nextAvatarName],MyAvatar()->m_name) && (upperBound > 1)) {
			GetNextAvatarName(avName);
			return;
		}
	strcpy(avName, (strFiles[nextAvatarName]));
}


CAvatarX *GetAvatar2(const char *avName) {
	int upperBound = avatars.GetUpperBound();
	for (int i = 1; i <= upperBound; i++) {
		if (!stricmp(((CAvatarX *)avatars[i])->m_name, avName)) {
			return (CAvatarX *) avatars[i];
		}
	}
	return (LoadAvatar(avName));
}

// pui is void * so others need not know about CUserInfo
CAvatarX *GetAvatar3(const char *avName, void *pui, BOOL bRandomIfNotFound) {
	CAvatarX *orig = NULL;
	int upperBound = avatars.GetUpperBound();
	for (int i = 1; i <= upperBound; i++) {
		CAvatarX *av = (CAvatarX *) avatars[i];
		if (!stricmp(av->OriginalName(), avName)) {
			if (av->m_userInfo == NULL || av->m_userInfo == pui) {
				av->m_flags &= ~OTHERMAPPED;
				return av; // it's not being used! return the original
			}
			else if (av->m_origID == 0) orig = av;
		}
	}

	if (orig) {  // found the original, but it's being used.  So dup it.
		CAvatarX *newAv = orig->DupAvatar();
		newAv->IndexAvatar();
		newAv->m_flags &= ~OTHERMAPPED;
		return newAv;
	}

	CAvatarX *newAv = LoadAvatar(avName);
	if (!newAv && bRandomIfNotFound) {
		char avatarstr[256];
		GetNextAvatarName(avatarstr);
		newAv = GetAvatar3(avatarstr, pui);
		newAv->m_flags |= OTHERMAPPED;
	}
	return newAv;
}


CBodySingle::CBodySingle(const CBodySingle &b)
:CBody(b) {
	m_bodyRec = b.m_bodyRec;
}

CBodyDouble::CBodyDouble(const CBodyDouble &b)
:CBody(b) {
	m_faceRec = b.m_faceRec;
	m_torsoRec = b.m_torsoRec;
}

CBodyUnary::CBodyUnary(const CBodyUnary &b)
:CBodySingle(b) {
	m_bodyID = b.m_bodyID;
}

void CEmotionOpts::Add(double emotion, double intensity, int priority, int flags) {
	for (int i = 0; i < m_nOpts; i++) {
		if (m_emotions[i].m_emotion == emotion) {
			if (flags & OVERRIDEBYPRIORITY) {
				if (m_priorities[i] < priority) {
					m_priorities[i] = priority;
					m_emotions[i].m_intensity = (float)intensity;
				}
				return;
			} else if (flags & ADDPRIORITY) {
				m_priorities[i] = max(m_priorities[i] + priority, 255);
				m_emotions[i].m_intensity = (float) max(m_emotions[i].m_intensity, intensity);
				return;
			}
		}
	}

	if (m_nOpts >= MAXEMOPTS) return;

	// otherwise add a new entry
	m_emotions[m_nOpts].m_emotion = (float) emotion;
	m_emotions[m_nOpts].m_intensity = (float) intensity;
	m_priorities[m_nOpts++] = priority;
}

void CAvatarComplex::RecordBody(CBody *bdy) {
	switch (bdy->GetClass()) {
		case BC_BODYDOUBLE:			// only care about this case
			CBodyDouble *d = (CBodyDouble *) bdy;
			m_lastFace = d->m_faceRec - fRec;
			m_lastTorso = d->m_torsoRec - bRec;
	}
}

void CAvatarSimple::RecordBody(CBody *bdy) {
	switch(bdy->GetClass()) {
		case BC_BODYSINGLE:
			CBodySingle *d = (CBodySingle *) bdy;
			m_lastBody = d->m_bodyRec - bRec;
	}
}


void CAvatarComplex::GetIndices(CHAR &chFaceIndex, CHAR &chTorsoIndex, BYTE &bbRequested) {
	ASSERT(m_body->GetClass() == BC_BODYDOUBLE);
	CBodyDouble *b = (CBodyDouble *) m_body;
	chFaceIndex = b->m_faceRec - fRec;
	chTorsoIndex = b->m_torsoRec - bRec;
	bbRequested = (m_freeze != AF_UNFROZEN);
}

void CAvatarSimple::GetIndices(CHAR &chFaceIndex, CHAR &chTorsoIndex, BYTE &bbRequested) {
	ASSERT(m_body->GetClass() == BC_BODYSINGLE);
	CBodySingle *b = (CBodySingle *) m_body;
	chTorsoIndex = b->m_bodyRec - bRec;
	chFaceIndex = 0;
	bbRequested = (m_freeze != AF_UNFROZEN);
}

void CAvatarComplex::SetIndices(CHAR chFaceIndex, CHAR chTorsoIndex, BYTE bbRequested) {
	ASSERT(m_body->GetClass() == BC_BODYDOUBLE);
	CBodyDouble *b = (CBodyDouble *) m_body;
	if (chFaceIndex >= 0 && chFaceIndex < nFaces)
		b->m_faceRec = fRec + chFaceIndex;
	if (chTorsoIndex >= 0 && chTorsoIndex < nTorsos) 
		b->m_torsoRec = bRec + chTorsoIndex;
	b->m_requested = bbRequested; 
}

void CAvatarSimple::SetIndices(CHAR chFaceIndex, CHAR chTorsoIndex, BYTE bbRequested) {
	ASSERT(m_body->GetClass() == BC_BODYSINGLE);
	CBodySingle *b = (CBodySingle *) m_body;
	if (chTorsoIndex >= 0 && chTorsoIndex < m_nBodies) 
		b->m_bodyRec = bRec + chTorsoIndex;
	b->m_requested = bbRequested; 
}

void CAvatarComplex::GetEmotions(CEmotion &face, CEmotion &torso) {
	ASSERT(m_body->GetClass() == BC_BODYDOUBLE);
	CBodyDouble *b = (CBodyDouble *) m_body;
	face.m_emotion = b->m_faceRec->emotion;
	face.m_intensity = b->m_faceRec->intensity;
	torso.m_emotion = b->m_torsoRec->emotion;
	torso.m_intensity = b->m_torsoRec->intensity;
}

void CAvatarSimple::GetEmotions(CEmotion &face, CEmotion &torso) {
	ASSERT(m_body->GetClass() == BC_BODYSINGLE);
	CBodySingle *b = (CBodySingle *) m_body;
	face.m_emotion = b->m_bodyRec->emotion;
	face.m_intensity = b->m_bodyRec->intensity;
	torso.m_emotion = torso.m_intensity = (float)0.0;		// just set to zero to ensure non-random behavior
}

void CAvatarSimple::SetEmotions(CEmotion &face, CEmotion &torso) {
	ASSERT(m_body->GetClass() == BC_BODYSINGLE);
	CBodySingle *b = (CBodySingle *) m_body;
	// try to match emotion, w/ closest intensity
	double bestDelta = 1000.0;
	int index, endIndex, bestIndex = -1;
	index = endIndex = b->m_bodyRec - bRec;
	do {
		index = (index + 1) % m_nBodies;
		if (bRec[index].emotion == face.m_emotion) {
			float delta = (float) fabs(bRec[index].intensity - face.m_intensity);
			if (delta < bestDelta) {
				bestDelta = delta;
				bestIndex = index;
			}
		}
	} while (index != endIndex);
	if (bestIndex >= 0) b->m_bodyRec = bRec + bestIndex;
	else SetBodyNeutral(b);
}

void CAvatarComplex::SetEmotions(CEmotion &face, CEmotion &torso) {
	ASSERT(m_body->GetClass() == BC_BODYDOUBLE);
	CBodyDouble *b = (CBodyDouble *) m_body;

	// try to match face emotion, w/ closest intensity
	double bestDelta = 1000.0;
	int index, endIndex, bestIndex = -1;
	index = endIndex = b->m_faceRec - fRec;
	do {
		index = (index + 1) % nFaces;
		if (fRec[index].emotion == face.m_emotion) {
			float delta = (float) fabs(fRec[index].intensity - face.m_intensity);
			if (delta < bestDelta) {
				bestDelta = delta;
				bestIndex = index;
			}
		}
	} while (index != endIndex);
	if (bestIndex >= 0) b->m_faceRec = fRec + bestIndex;
	else SetFaceNeutral(b);

	// try to match body gesture, w/ closest intensity
	bestDelta = 1000.0;
	index = endIndex = b->m_torsoRec - bRec;
	bestIndex = -1;
	do {
		index = (index + 1) % nTorsos;
		if (bRec[index].emotion == torso.m_emotion) {
			float delta = (float) fabs(bRec[index].intensity - torso.m_intensity);
			if (delta < bestDelta) {
				bestDelta = delta;
				bestIndex = index;
			}
		}
	} while (index != endIndex);
	if (bestIndex >= 0) b->m_torsoRec = bRec + bestIndex;
	else SetTorsoNeutral(b);
}

static int nextInstanceID = 1;

void CAvatarX::Initialize() 
{ 
	m_freeze 			= AF_UNFROZEN; 
	m_name 				= NULL; 
	m_style 			= 0; 
	m_body 				= NULL; 
	m_userInfo 			= NULL; 
	m_avatarID 			= 0; 
	m_origID 			= 0; 
	m_flags 			= 0; 
	m_nSends 			= 0; 
	m_nCopies 			= 0; 
	m_lastDir 			= FALSE; 
	m_lastLeft 			= 0;
	m_lastRight 		= 0; 
	m_iconIndex 		= -1; 
	m_pStream 			= NULL; 
	m_pszOriginalURL 	= NULL;
	m_pszNewURL			= NULL;
	m_pszCopyright		= NULL;
	// m_talkTo.SetSize(0,5); 
	m_arrPoses.SetSize (0, 16); 
}

CAvatarX::CAvatarX(const CAvatarX &b) {
	Initialize();
	char name[_MAX_FNAME+10]; // + to handle the #%d
	sprintf(name, "%s#%d", b.m_name, b.m_nCopies);
	m_name = strdup(name);
	m_origID = b.m_avatarID;
	m_flags = b.m_flags;
	m_icon = b.m_icon;				   // poseID for icon		
	m_pStream = NULL;

}

void 
CAvatarX::SetNewName(
LPCSTR pszName)
{
	free (m_name);
	m_name = strdup (pszName);
}

CAvatarSimple::CAvatarSimple(const CAvatarSimple &b) : CAvatarX(b) {
	bRec = b.bRec;
	m_nBodies = b.m_nBodies;
	m_lastBody = -1;
}

CAvatarComplex::CAvatarComplex(const CAvatarComplex &b) : CAvatarX(b) {
	fRec = b.fRec;
	bRec = b.bRec;
	nFaces = b.nFaces;
	nTorsos = b.nTorsos;
	m_lastFace = m_lastTorso = -1;
}

CAvatarX *
CAvatarX::GetOriginalAvatar()
{
	CAvatarX * pAv = this;
	while (pAv != NULL && pAv->m_origID != 0) {
		pAv = GetAvatar (pAv->m_origID);
	}
	return pAv;
}

const char *CAvatarX::OriginalName() {
	CAvatarX * pAv = GetOriginalAvatar ();
	return pAv ? pAv->m_name : NULL;
}

const char *CAvatarX::Url() 
{
	CAvatarX * pAv = GetOriginalAvatar ();
	return pAv ? (pAv->m_pszNewURL ? pAv->m_pszNewURL : pAv->m_pszOriginalURL) : NULL;
}

const char *CAvatarX::Copyright() 
{
	CAvatarX * pAv = GetOriginalAvatar ();
	return pAv ? pAv->m_pszCopyright : NULL;
}

BOOL NullAvatar() {
	return (MyAvatar() == NULL);
}

double randfloat();

void CAvatarComplex::SetSequential(void *p, int n) {
	UINT seed = (UINT) GetTickCount();
	srand(seed);
//	randfloat();  // first randfloat doesn't seem to be random.  Why?
	int head = (int)(randfloat() * nFaces);
	int torso = (int)(randfloat() * nTorsos);

	TRACE("head = %d, torso = %d\n", head, torso);
	CUserInfo *pui = (CUserInfo *) p;
	pui->m_udi.m_chExpr = head;
	pui->m_udi.m_chGest = torso;
	pui->m_udi.m_bbCooked = TRUE;
}

void CAvatarSimple::SetSequential(void *p, int n) {
	int body = n % m_nBodies;

	CUserInfo *pui = (CUserInfo *) p;
	pui->m_udi.m_chExpr = body;
	pui->m_udi.m_chGest = body;
	pui->m_udi.m_bbCooked = TRUE;
}

CPose::CPose(
LPDWORD pdwOffsets, 
LPBYTE pbyFormats, 
LPBYTE pbyPaletteTypes)
{
	ASSERT (pdwOffsets != NULL);
	ASSERT (pbyFormats != NULL);
	ASSERT (pbyPaletteTypes != NULL);

	memcpy (m_dwOffsets, pdwOffsets, sizeof(m_dwOffsets));
	memcpy (m_byFormats, pbyFormats, sizeof(m_byFormats));
	memcpy (m_byPaletteTypes, pbyPaletteTypes, sizeof(m_byPaletteTypes));

	// Handling for special types of poses.
	if (m_byPaletteTypes[0] == AIP_MASKEDMONO) {
		m_dwOffsets[1] = m_dwOffsets[2] = 0;
	}
	else if (m_byPaletteTypes[1] == AIP_DUALMASK && m_dwOffsets[1] != 0) {
		m_dwOffsets[2] = 0;
	}

	ZeroMemory (m_pdibs, sizeof(m_pdibs));
}

CPose::~CPose()
{
	for (int i = 0; i < 3; i++) {
		if (m_pdibs[i]) {
			delete m_pdibs[i];
		}
	}
}
