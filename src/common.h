// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

#ifndef _COMMON_H_
#define _COMMON_H_

#define UNITSPERINCH	1440		// we're using TWIMs

#define DEFAULTDELTA 100
void DrawPoint(CDC *, POINT, int delta = DEFAULTDELTA);

// Per-display DPI for scaling the app's *pixel-based* UI surfaces (member-list
// icons, the bodycam emotion wheel, etc.).  The main comic page is drawn in
// device-independent TWIPs and needs no help, but these surfaces use hardcoded
// pixel sizes tuned for 96 DPI; multiply them through DpiScale() so they stay
// physically the same size (and proportional to the comic) on high-DPI screens.
// g_screenDpi is initialized once in CChatApp::InitInstance.
extern int g_screenDpi;
inline int DpiScale(int n96) { return ::MulDiv(n96, g_screenDpi, 96); }

#endif // _COMMON_H_

