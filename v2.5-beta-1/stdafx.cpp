// stdafx.cpp : source file that includes just the standard includes
//	chat.pch will be the pre-compiled header
//	stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

#ifndef NOGLOBPAL
CPalette        ghPalette;
char logPalBits[(sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * 255)]; 
LOGPALETTE      *gpLogPal=(LOGPALETTE *) logPalBits;	// RamuM,to Avoid new / delete
#endif NOGLOBPAL