// stdafx.cpp : source file that includes just the standard includes
//	chat.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// Modern build: opt in to Common Controls v6 so that rebar/listview/tab
// REBARBANDINFO/LVITEM struct sizes match the loaded comctl32 (the v2.5
// CoolBar rebar requires this or RB_INSERTBAND fails). Also enables themes.
#pragma comment(linker, "/manifestdependency:\"type='win32' " \
	"name='Microsoft.Windows.Common-Controls' version='6.0.0.0' " \
	"processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifndef NOGLOBPAL
CPalette        ghPalette;
char logPalBits[(sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 255)]; 
LOGPALETTE      *gpLogPal=(LOGPALETTE *) logPalBits;	// RamuM,to Avoid new / delete
#endif NOGLOBPAL