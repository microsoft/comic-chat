//********************************************************************** 
// 
//  DibUtil.c 
// 
//  Source file for Device-Independent Bitmap (DIB) API.  Provides 
//  the following functions: 
// 
//  FindDIBBits()       - Sets pointer to the DIB bits 
//  DIBWidth()          - Gets the width of the DIB 
//  DIBHeight()         - Gets the height of the DIB 
//  PaletteSize()       - Calculates the buffer size required by a palette 
//  DIBNumColors()      - Calculates number of colors in the DIB's color table 
//  BitmapToDIB()       - Creates a DIB from a bitmap 
// 
// Written by Microsoft Product Support Services, Developer Support. 
// Copyright (C) 1991-1996 Microsoft Corporation. All rights reserved. 
//********************************************************************** 
 
#define     STRICT      // enable strict type checking 
 
#include <windows.h> 
#include <assert.h> 
#include "DibApi.h" 
#include "DibUtil.h" 
#include <stdio.h> 
 
 
/************************************************************************* 
 * 
 * FindDIBBits() 
 * 
 * Parameter: 
 * 
 * LPSTR lpDIB      - pointer to packed-DIB memory block 
 * 
 * Return Value: 
 * 
 * LPSTR            - pointer to the DIB bits 
 * 
 * Description: 
 * 
 * This function calculates the address of the DIB's bits and returns a 
 * pointer to the DIB bits. 
 * 
 ************************************************************************/ 
 
LPSTR FindDIBBits(LPSTR lpDIB) 
{ 
   return (lpDIB + *(LPDWORD)lpDIB + PaletteSize(lpDIB)); 
} 
 
 
/************************************************************************* 
 * 
 * DIBWidth() 
 * 
 * Parameter: 
 * 
 * LPSTR lpDIB      - pointer to packed-DIB memory block 
 * 
 * Return Value: 
 * 
 * DWORD            - width of the DIB 
 * 
 * Description: 
 * 
 * This function gets the width of the DIB from the BITMAPINFOHEADER 
 * width field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER 
 * width field if it is an OS/2-style DIB. 
 * 
 ************************************************************************/ 
 
 
DWORD DIBWidth(LPSTR lpDIB) 
{ 
    LPBITMAPINFOHEADER   lpbmi;  // pointer to a Win 3.0-style DIB 
    LPBITMAPCOREHEADER   lpbmc;  // pointer to an OS/2-style DIB 
 
    // point to the header (whether Win 3.0 and OS/2) 
 
    lpbmi = (LPBITMAPINFOHEADER)lpDIB; 
    lpbmc = (LPBITMAPCOREHEADER)lpDIB; 
 
    // return the DIB width if it is a Win 3.0 DIB 
 
    if (lpbmi->biSize == sizeof(BITMAPINFOHEADER)) 
        return lpbmi->biWidth; 
    else  // it is an OS/2 DIB, so return its width 
        return (DWORD)lpbmc->bcWidth; 
} 
 
 
/************************************************************************* 
 * 
 * DIBHeight() 
 * 
 * Parameter: 
 * 
 * LPSTR lpDIB      - pointer to packed-DIB memory block 
 * 
 * Return Value: 
 * 
 * DWORD            - height of the DIB 
 * 
 * Description: 
 * 
 * This function gets the height of the DIB from the BITMAPINFOHEADER 
 * height field if it is a Windows 3.0-style DIB or from the BITMAPCOREHEADER 
 * height field if it is an OS/2-style DIB. 
 * 
 ************************************************************************/ 
 
DWORD DIBHeight(LPSTR lpDIB) 
{ 
   LPBITMAPINFOHEADER   lpbmi;  // pointer to a Win 3.0-style DIB 
   LPBITMAPCOREHEADER   lpbmc;  // pointer to an OS/2-style DIB 
 
   // point to the header (whether OS/2 or Win 3.0 
 
   lpbmi = (LPBITMAPINFOHEADER)lpDIB; 
   lpbmc = (LPBITMAPCOREHEADER)lpDIB; 
 
    // return the DIB height if it is a Win 3.0 DIB 
    if (lpbmi->biSize == sizeof(BITMAPINFOHEADER)) 
        return lpbmi->biHeight; 
    else  // it is an OS/2 DIB, so return its height 
        return (DWORD)lpbmc->bcHeight; 
} 
 
 
/************************************************************************* 
 * 
 * PaletteSize() 
 * 
 * Parameter: 
 * 
 * LPSTR lpDIB      - pointer to packed-DIB memory block 
 * 
 * Return Value: 
 * 
 * WORD             - size of the color palette of the DIB 
 * 
 * Description: 
 * 
 * This function gets the size required to store the DIB's palette by 
 * multiplying the number of colors by the size of an RGBQUAD (for a 
 * Windows 3.0-style DIB) or by the size of an RGBTRIPLE (for an OS/2- 
 * style DIB). 
 * 
 ************************************************************************/ 
 
WORD PaletteSize(LPSTR lpDIB) 
{ 
    // calculate the size required by the palette 
    if (IS_WIN30_DIB (lpDIB)) 
        return (DIBNumColors(lpDIB) * sizeof(RGBQUAD)); 
    else 
        return (DIBNumColors(lpDIB) * sizeof(RGBTRIPLE)); 
} 
 

/************************************************************************* 
 * 
 * DIBBitsPerPixel() 
 * 
 * Parameter: 
 * 
 * LPSTR lpDIB      - pointer to packed-DIB memory block 
 * 
 * Return Value: 
 * 
 * WORD             - number of bits used to code each pixel 
 * 
 ************************************************************************/ 
 
WORD DIBBitsPerPixel(LPSTR lpDIB) 
{ 
    if (IS_WIN30_DIB(lpDIB)) 
        return ((LPBITMAPINFOHEADER)lpDIB)->biBitCount; 
    else 
        return ((LPBITMAPCOREHEADER)lpDIB)->bcBitCount; 
}


/************************************************************************* 
 * 
 * DIBNumColors() 
 * 
 * Parameter: 
 * 
 * LPSTR lpDIB      - pointer to packed-DIB memory block 
 * 
 * Return Value: 
 * 
 * WORD             - number of colors in the color table 
 * 
 * Description: 
 * 
 * This function calculates the number of colors in the DIB's color table 
 * by finding the bits per pixel for the DIB (whether Win3.0 or OS/2-style 
 * DIB). If bits per pixel is 1: colors=2, if 4: colors=16, if 8: colors=256, 
 * if 24, no colors in color table. 
 * 
 ************************************************************************/ 
 
WORD DIBNumColors(LPSTR lpDIB) 
{ 
    WORD wBitCount;  // DIB bit count 
 
    // If this is a Windows-style DIB, the number of colors in the 
    // color table can be less than the number of bits per pixel 
    // allows for (i.e. lpbi->biClrUsed can be set to some value). 
    // If this is the case, return the appropriate value. 
     
 
    if (IS_WIN30_DIB(lpDIB)) 
    { 
        DWORD dwClrUsed; 
 
        dwClrUsed = ((LPBITMAPINFOHEADER)lpDIB)->biClrUsed; 
        if (dwClrUsed) 
 
        return (WORD)dwClrUsed; 
    } 
 
    // Calculate the number of colors in the color table based on 
    // the number of bits per pixel for the DIB. 
     
    if (IS_WIN30_DIB(lpDIB)) 
        wBitCount = ((LPBITMAPINFOHEADER)lpDIB)->biBitCount; 
    else 
        wBitCount = ((LPBITMAPCOREHEADER)lpDIB)->bcBitCount; 
 
    // return number of colors based on bits per pixel 
 
    switch (wBitCount) 
    { 
        case 1: 
            return 2; 
 
        case 4: 
            return 16; 
 
        case 8: 
            return 256; 
 
        default: 
            return 0; 
    } 
} 
 
 
/*************************************************************************
 *
 * DIBToBitmap()
 *
 * Parameters:
 *
 * HDIB hDIB        - specifies the DIB to convert
 *
 * HPALETTE hPal    - specifies the palette to use with the bitmap
 *
 * Return Value:
 *
 * HBITMAP          - identifies the device-dependent bitmap
 *
 * Description:
 *
 * This function creates a bitmap from a DIB using the specified palette.
 * If no palette is specified, default is used.
 *
 * NOTE:
 *
 * The bitmap returned from this function is always a bitmap compatible
 * with the screen (e.g. same bits/pixel and color planes) rather than
 * a bitmap with the same attributes as the DIB.  This behavior is by
 * design, and occurs because this function calls CreateDIBitmap to
 * do its work, and CreateDIBitmap always creates a bitmap compatible
 * with the hDC parameter passed in (because it in turn calls
 * CreateCompatibleBitmap).
 *
 * So for instance, if your DIB is a monochrome DIB and you call this
 * function, you will not get back a monochrome HBITMAP -- you will
 * get an HBITMAP compatible with the screen DC, but with only 2
 * colors used in the bitmap.
 *
 * If your application requires a monochrome HBITMAP returned for a
 * monochrome DIB, use the function SetDIBits().
 *
 * Also, the DIBpassed in to the function is not destroyed on exit. This
 * must be done later, once it is no longer needed.
 *
 * History:   Date      Author               Reason
 *            6/01/91   Garrett McAuliffe    Created
 *            9/15/91   Patrick Schreiber    Added header and comments
 *            3/27/92   Mark Bader           Added comments about resulting
 *                                           bitmap format
 *
 ************************************************************************/
HBITMAP FAR DIBToBitmap(HDIB hDIB, HPALETTE hPal)
{
   LPSTR lpDIBHdr, lpDIBBits;  // pointer to DIB header, pointer to DIB bits
   HBITMAP hBitmap;            // handle to device-dependent bitmap
   HDC hDC;                    // handle to DC
   HPALETTE hOldPal = NULL;    // handle to a palette

   /* if invalid handle, return NULL */

   if (!hDIB)
      return NULL;

   /* lock memory block and get a pointer to it */
   lpDIBHdr = GlobalLock(hDIB);

   /* get a pointer to the DIB bits */
   lpDIBBits = FindDIBBits(lpDIBHdr);

   /* get a DC */
   hDC = GetDC(NULL);
   if (!hDC)
   {
      /* clean up and return NULL */
      GlobalUnlock(hDIB);
      return NULL;
   }

   /* select and realize palette */
   if (hPal)
      hOldPal = SelectPalette(hDC, hPal, FALSE);
   RealizePalette(hDC);

   /* create bitmap from DIB info. and bits */
   hBitmap = CreateDIBitmap(hDC, (LPBITMAPINFOHEADER)lpDIBHdr, CBM_INIT,
                lpDIBBits, (LPBITMAPINFO)lpDIBHdr, DIB_RGB_COLORS);

   /* restore previous palette */
   if (hOldPal)
      SelectPalette(hDC, hOldPal, FALSE);

   /* clean up */
   ReleaseDC(NULL, hDC);
   GlobalUnlock(hDIB);

   /* return handle to the bitmap */
   return hBitmap;
}


/************************************************************************* 
 * 
 * BitmapToDIB() 
 * 
 * Parameters: 
 * 
 * HBITMAP hBitmap  - specifies the bitmap to convert 
 * 
 * HPALETTE hPal    - specifies the palette to use with the bitmap 
 * 
 * Return Value: 
 * 
 * HDIB             - identifies the device-dependent bitmap 
 * 
 * Description: 
 * 
 * This function creates a DIB from a bitmap using the specified palette. 
 * 
 ************************************************************************/ 
 
HDIB BitmapToDIB(HBITMAP hBitmap, HPALETTE hPal) 
{ 
    BITMAP              bm;         // bitmap structure 
    BITMAPINFOHEADER    bi;         // bitmap header 
    LPBITMAPINFOHEADER  lpbi;       // pointer to BITMAPINFOHEADER 
    DWORD               dwLen;      // size of memory block 
    HANDLE              hDIB, h;    // handle to DIB, temp handle 
    HDC                 hDC;        // handle to DC 
    WORD                biBits;     // bits per pixel 
 
    // check if bitmap handle is valid 
 
    if (!hBitmap) 
        return NULL; 
 
    // fill in BITMAP structure, return NULL if it didn't work 
 
    if (!GetObject(hBitmap, sizeof(bm), (LPSTR)&bm)) 
        return NULL; 
 
    // if no palette is specified, use default palette 
 
    if (hPal == NULL) 
        hPal = GetStockObject(DEFAULT_PALETTE); 
 
    // calculate bits per pixel 
 
    biBits = bm.bmPlanes * bm.bmBitsPixel; 
 
    // make sure bits per pixel is valid 
 
    if (biBits <= 1) 
        biBits = 1; 
    else if (biBits <= 4) 
        biBits = 4; 
    else if (biBits <= 8) 
        biBits = 8; 
    else // if greater than 8-bit, force to 24-bit 
        biBits = 24; 
 
    // initialize BITMAPINFOHEADER 
 
    bi.biSize = sizeof(BITMAPINFOHEADER); 
    bi.biWidth = bm.bmWidth; 
    bi.biHeight = bm.bmHeight; 
    bi.biPlanes = 1; 
    bi.biBitCount = biBits; 
    bi.biCompression = BI_RGB; 
    bi.biSizeImage = 0; 
    bi.biXPelsPerMeter = 0; 
    bi.biYPelsPerMeter = 0; 
    bi.biClrUsed = 0; 
    bi.biClrImportant = 0; 
 
    // calculate size of memory block required to store BITMAPINFO 
 
    dwLen = bi.biSize + PaletteSize((LPSTR)&bi); 
 
    // get a DC 
 
    hDC = GetDC(NULL); 
 
    // select and realize our palette 
 
    hPal = SelectPalette(hDC, hPal, FALSE); 
    RealizePalette(hDC); 
 
    // alloc memory block to store our bitmap 
 
    hDIB = GlobalAlloc(GHND, dwLen); 
 
    // if we couldn't get memory block 
 
    if (!hDIB) 
    { 
      // clean up and return NULL 
 
      SelectPalette(hDC, hPal, TRUE); 
      RealizePalette(hDC); 
      ReleaseDC(NULL, hDC); 
      return NULL; 
    } 
 
    // lock memory and get pointer to it 
 
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB); 
 
    /// use our bitmap info. to fill BITMAPINFOHEADER 
 
    *lpbi = bi; 
 
    // call GetDIBits with a NULL lpBits param, so it will calculate the 
    // biSizeImage field for us     
 
    GetDIBits(hDC, hBitmap, 0, (UINT)bi.biHeight, NULL, (LPBITMAPINFO)lpbi, 
        DIB_RGB_COLORS); 
 
    // get the info. returned by GetDIBits and unlock memory block 
 
    bi = *lpbi; 
    GlobalUnlock(hDIB); 
 
    // if the driver did not fill in the biSizeImage field, make one up  
    if (bi.biSizeImage == 0) 
        bi.biSizeImage = WIDTHBYTES((DWORD)bm.bmWidth * biBits) * bm.bmHeight; 
 
    // realloc the buffer big enough to hold all the bits 
 
    dwLen = bi.biSize + PaletteSize((LPSTR)&bi) + bi.biSizeImage; 
 
    if (h = GlobalReAlloc(hDIB, dwLen, 0)) 
        hDIB = h; 
    else 
    { 
        // clean up and return NULL 
 
        GlobalFree(hDIB); 
        hDIB = NULL; 
        SelectPalette(hDC, hPal, TRUE); 
        RealizePalette(hDC); 
        ReleaseDC(NULL, hDC); 
        return NULL; 
    } 
 
    // lock memory block and get pointer to it */ 
 
    lpbi = (LPBITMAPINFOHEADER)GlobalLock(hDIB); 
 
    // call GetDIBits with a NON-NULL lpBits param, and actualy get the 
    // bits this time 
 
    if (GetDIBits(hDC, hBitmap, 0, (UINT)bi.biHeight, (LPSTR)lpbi + 
            (WORD)lpbi->biSize + PaletteSize((LPSTR)lpbi), (LPBITMAPINFO)lpbi, 
            DIB_RGB_COLORS) == 0) 
    { 
        // clean up and return NULL 
 
        GlobalUnlock(hDIB); 
        hDIB = NULL; 
        SelectPalette(hDC, hPal, TRUE); 
        RealizePalette(hDC); 
        ReleaseDC(NULL, hDC); 
        return NULL; 
    } 
 
    bi = *lpbi; 
 
    // clean up  
    GlobalUnlock(hDIB); 
    SelectPalette(hDC, hPal, TRUE); 
    RealizePalette(hDC); 
    ReleaseDC(NULL, hDC); 
 
    // return handle to the DIB 
    return hDIB; 
} 

