//*********************************************************************
//*                  Microsoft Windows                               **
//*            Copyright(c) Microsoft Corp., 1994-1996               **
//*********************************************************************
//
// DLYLDDLL.H - globals for delay loading of DLLs
//
#ifndef __DLYLDDLL_H__
#define __DLYLDDLL_H__

#ifdef __cplusplus
extern "C" {
#endif

#define ENSURE_LOADED(_hinst, _dll)   (_hinst ? TRUE : (BOOL)(_hinst = LoadLibrary(_dll)))

extern HINSTANCE g_hinstRatings;
extern const TCHAR c_tszRatingsDLL[];

#ifdef __cplusplus
}
#endif

#endif //__DLYLDDLL_H__