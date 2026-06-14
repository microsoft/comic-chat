#ifndef __TEXTCORE_H__


#if MSVC41
//	RamuM, not needed we have latest richedit.h

// WARNING: RE: LINK defines...
// The following should be in v. 2.0 of richedit.h.  Since our build enviroment only has v. 1.0,
// I'm including these defines here.  Please fix as soon as the new .h is available!!!
#define CFM_LINK		0x00000020		// Exchange hyperlink extension
#define CFE_LINK		0x0020
#define ENM_LINK				0x04000000
#define EN_LINK					0x070b
#ifdef _WIN32
#	define	_WPAD	/##/
#else
#	define	_WPAD	WORD
#endif


typedef struct _enlink
{
    NMHDR nmhdr;
    UINT msg;
    _WPAD   _wPad1;
    WPARAM wParam;
    _WPAD   _wPad2;
    LPARAM lParam;
    CHARRANGE chrg;
} ENLINK;
//END WARNING
#endif //MSVC41

#define TEXTVIEWCLASSNAME	CTextCore
#include "..\inc\TextView.H"

#define __TEXTCORE_H__
#endif __TEXTCORE_H__
