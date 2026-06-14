// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

// DJK - these are necessary while we're compiling debug builds using MFC4.0
//   and the new commctrl.h that changes the definitions of these structures.
//   Without these, unresolved external errors result.
#define tagLVITEMA		_LV_ITEMA
#define tagLVFINDINFOA	_LV_FINDINFOA
#define tagTCITEMA		_TC_ITEMA

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxole.h>         // MFC OLE classes
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows 95 Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions -- djk - now use chatsock

#ifndef NOGLOBPAL
extern CPalette        ghPalette;
extern LOGPALETTE      *gpLogPal;
#endif NOGLOBPAL

#include "chicdial.h"
