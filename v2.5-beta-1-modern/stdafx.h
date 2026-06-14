// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

// DJK - these were necessary in 1998 when MFC 4.0's prebuilt library used the
//   old common-control struct tags (_LV_ITEMA etc.) while the SDK's commctrl.h
//   had switched to the new tags. The modern MFC library and SDK both use the
//   new tags, so remapping them here would instead BREAK linkage. Left disabled.
//#define tagLVITEMA		_LV_ITEMA
//#define tagLVFINDINFOA	_LV_FINDINFOA
//#define tagTCITEMA		_TC_ITEMA

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions -- djk - now use chatsock
// Unicode<==>ANSI conversion macros are in afxconv.h for MFC 4.2 on, but in
// afxpriv.h before that.
#if (_MFC_VER > 0x0410)
#include <afxconv.h>
#else
#include <afxpriv.h>
#endif
#include <afxtempl.h>

#include <process.h>
#include <wininet.h>

#include <mbstring.h>
#include <shlobj.h>
#include <mmsystem.h>

#ifndef NOGLOBPAL
extern CPalette        ghPalette;
extern LOGPALETTE      *gpLogPal;
#endif NOGLOBPAL

#include "chicdial.h"
#include "coolbar.h"
