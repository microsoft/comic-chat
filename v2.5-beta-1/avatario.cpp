#include "stdafx.h"

// We need a very minimal amount of stuff from what was here...all the
// new stuff is in avbfile.cpp

#include "bbox.h"
#include "chat.h"		// for theApp
#include "dib.h"
//#include "chatprot.h"	// for AVBMPPATH
#include "vector2d.h"  // only for PI
#include "pe.h"
#include "avatar.h"
#include "avatario.h"

extern CChatApp theApp;

CAvatarX *LoadAvatarInfo(const char *avName) {

	CString path;										// djk - BETA1 fix
	path.Format("%s\\%s.avb", theApp.GetAvatarDir(), avName);  // djk - BETA1 fix
	if (GetFileAttributes (path) == (DWORD)-1L) {
		return NULL;  // no such avatar, return NULL
	}
	CAvatarX * pAvatar;
	TRY
	{
		CAvatarFileStream * pStream = new CAvatarFileStream (path);
		pAvatar = CAvatarX::LoadAvatar (pStream);
		if (pAvatar != NULL) {
			pAvatar->SetStream (pStream);
			pAvatar->SetNewName (avName); // SSN - synchronize avatar name with filename.
		}
		else {
			delete pStream;
		}
	}
	CATCH_ALL(e)
	{
		pAvatar = NULL;
	}
	END_CATCH_ALL
	return pAvatar;
}

float emFloats[] = {
	(float) 0.0,
	(float) EM_HAPPY,
	(float) EM_COY,
	(float) EM_BORED,
	(float) EM_SCARED,
	(float) EM_SAD,
	(float) EM_ANGRY,
	(float) EM_SHOUT,
	(float) EM_LAUGH,
	(float) EM_NEUTRAL,
	(float) EM_WAVE,
	(float) EM_POINTOTHER,
	(float) EM_POINTSELF,
	(float) EM_DOUBLEPOINT,
	(float) EM_SHRUG,
	(float) EM_3QRWALK,
	(float) EM_SIDEWALK,
	(float) EM_3QFWALK,
};

BYTE IndexToByte(BYTE);
BYTE ByteToIndex(BYTE);

void EmotionToBytes(CEmotion &em, BYTE &emotion, BYTE &intensity) {
	BYTE emVal = 9;  // neutral always safe
	int n = sizeof(emFloats) / sizeof(float);
	for (int i = 1; i < n; i++) 
		if (emFloats[i] == em.m_emotion) {
		emVal = (BYTE) i;
		break;
	}

	BYTE inVal = (BYTE)(em.m_intensity * 10);  // good 'nuff
	emotion = IndexToByte(emVal);
	intensity = IndexToByte(inVal);
}

void BytesToEmotion(CEmotion &em, BYTE emIndex, BYTE inIndex) {
	if (emIndex >= sizeof(emFloats) / sizeof(float)) em.m_emotion = EM_NEUTRAL;
	else em.m_emotion = emFloats[emIndex];
	em.m_intensity = (float)(inIndex / 10.0);
}

float EmotionToFloat(int index)
{
	if (index < 0 || index >= sizeof(emFloats) / sizeof(emFloats[0])) {
		return 0.0;
	}
	else {
		return emFloats[index];
	}

}
