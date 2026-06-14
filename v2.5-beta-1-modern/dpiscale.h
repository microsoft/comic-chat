// dpiscale.h - per-display DPI scaling for pixel-based UI surfaces.
//
// The comic page itself is drawn in device-independent TWIPs and needs no help,
// but several surfaces (member-list icons, the bodycam emotion wheel, the say
// toolbar, mouse-wheel deltas, ...) use hardcoded pixel sizes tuned for 96 DPI.
// Run those through DpiScale() so they stay physically the same size on
// high-DPI displays. g_screenDpi is initialized once in CChatApp::InitInstance.

#ifndef _DPISCALE_H_
#define _DPISCALE_H_

extern int g_screenDpi;
inline int DpiScale(int n96) { return ::MulDiv(n96, g_screenDpi, 96); }

#endif // _DPISCALE_H_
