/******************************************************************************\
*
*  MODULE:      AVATED.C
*
*  CREATED:		Regis Brid, 07/20/97
*
*  Copyright (c) 1992-1996 Microsoft Corporation
*
\******************************************************************************/

#include <windows.h>
#include <stdlib.h>
#include "DibApi.H"

static COLORREF g_crBackG24 = 0L;

typedef struct tagBITMAPINFO256
{
	BITMAPINFOHEADER    bmiHeader;
	RGBQUAD             bmiColors[256];
} BITMAPINFO256;

COLORREF g_rgcrBasic[] = { RGB(0,0,0), RGB(128,0,0), RGB(0,128,0), RGB(128,128,0),
						   RGB(0,0,128), RGB(128,0,128), RGB(0,128,128), RGB(128,128,128),
						   RGB(192,192,192), RGB(255,0,0), RGB(0,255,0), RGB(255,255,0),
						   RGB(0,0,255), RGB(255,0,255), RGB(0,255,255), RGB(255,255,255)
						};


DWORD dwDeltaColor(COLORREF cr1, COLORREF cr2)
{
	return (abs(((cr1 >> 16) & 0xFF) - ((cr2 >> 16) & 0xFF)) + 
			abs(((cr1 >> 8) & 0xFF) - ((cr2 >> 8) & 0xFF)) + 
			abs((cr1 & 0xFF) - (cr2 & 0xFF)));
}


DWORD dwDeltaColor2(COLORREF cr1, BYTE r, BYTE g, BYTE b)
{
	return (abs(((cr1 >> 16) & 0xFF) - b) + 
			abs(((cr1 >> 8) & 0xFF) - g) + 
			abs((cr1 & 0xFF) - r));
}


COLORREF crClosestBasic(COLORREF cr)
{
	COLORREF	crClosest;
	DWORD		dwDelta, dwMinDelta = 0xFFFFFF;
	short		i;

	for (i = 0; i <= 15; i++)
		if ((dwDelta = dwDeltaColor(cr, g_rgcrBasic[i])) < dwMinDelta)
		{
			dwMinDelta = dwDelta;
			crClosest = g_rgcrBasic[i];
		}

	return crClosest;
}



/******************************************************************************\
*
*  FUNCTION:    DllMain
*
*  INPUTS:      hDLL       - DLL module handle
*               dwReason   - reason being called (e.g. process attaching)
*               lpReserved - reserved
*
*  RETURNS:     TRUE if initialization passed, or
*               FALSE if initialization failed.
*
*  COMMENTS:    On DLL_PROCESS_ATTACH registers the SPINCUBECLASS
*
*               DLL initialization serialization is guaranteed within a
*               process (if multiple threads then DLL entry points are
*               serialized), but is not guaranteed across processes.
*
*               When synchronization objects are created, it is necesary
*               to check the return code of GetLastError even if the create
*               call succeeded. If the object existed, ERROR_ALREADY_EXISTED
*               will be returned.
*
*               If your DLL uses any C runtime functions then you should
*               always call _CRT_INIT so that the C runtime can initialize
*               itself appropriately. Failure to do this may result in
*               indeterminate behavior. When the DLL entry point is called
*               for DLL_PROCESS_ATTACH & DLL_THREAD_ATTACH circumstances,
*               _CRT_INIT should be called before any other initilization
*               is performed. When the DLL entry point is called for
*               DLL_PROCESS_DETACH & DLL_THREAD_DETACH circumstances,
*               _CRT_INIT should be called after all cleanup has been
*               performed, i.e. right before the function returns.
*
\******************************************************************************/

BOOL WINAPI DllMain (HANDLE hDLL, DWORD dwReason, LPVOID lpReserved)
{
	return TRUE;
}


/******************************************************************************\
*
*  FUNCTION:    ClipAvatar
*
*  INPUTS:      - hdcSrc:		DC of source bitmap
*               - hdcDest:		DC of destination bitmap
*               - pnXDest:		pointer to X-offset of final bitmap/original bitmap
*               - pnYDest:		pointer to Y-offset of final bitmap/original bitmap
*               - pnWDest:		pointer to width of final bitmap
*               - pnHDest:		pointer to height of final bitmap
*               - lBackground:	< 0  ==> background color is given by lower left pixel
*                               >= 0 ==> specifies the background color (RGB value)
*
*  RETURNS:     0 if successful, > 0 otherwise
*
\******************************************************************************/

UINT CALLBACK ClipAvatar(HDC hdcSrc, HDC hdcDest, LPINT pnXDest, LPINT pnYDest, LPINT pnWDest, LPINT pnHDest, LONG lBackground)
{
	DWORD			/*dwPixColor,*/ dwBackground;
	INT				iColumn, iLine;
	WORD			wPS, wBPP;
	WORD			wWidth, wHeight;
    LPSTR			lpDIBHdr = NULL, lpDIBBits;  // pointer to DIB header, pointer to DIB bits 
	PBYTE			pbBits;
	HDIB			hDibSrc;
	HBITMAP			hBmpSrc;
	UINT			uErr = ERROR_GEN_FAILURE;
	WORD			wLeftMin, wRightMax, wTopLess, wBottomMost;
	BOOL			bRet;
	INT				iColorIndex;
	RGBQUAD			rgbBackground;
	LPBITMAPINFO	lpbi;

	*pnXDest = *pnYDest = *pnWDest = *pnHDest = 0;

	if (!hdcSrc || !hdcDest)
		return ERROR_INVALID_HANDLE;

	hBmpSrc = (HBITMAP) GetCurrentObject(hdcSrc, OBJ_BITMAP);

	if (!hBmpSrc)
		return ERROR_GEN_FAILURE;

	hDibSrc = BitmapToDIB(hBmpSrc, NULL); 

	if (!hDibSrc)
		goto exit;

    // lock memory block and get a pointer to it 
    lpDIBHdr = GlobalLock(hDibSrc); 
 
    // get a pointer to the DIB bits 
    lpDIBBits = FindDIBBits(lpDIBHdr); 

	wWidth = (WORD) DIBWidth(lpDIBHdr);

	wHeight = (WORD) DIBHeight(lpDIBHdr);
	
	wPS = PaletteSize(lpDIBHdr);
	
	wBPP = DIBBitsPerPixel(lpDIBHdr); 

	if (24 != wBPP && 8 != wBPP)	// this algo only deals with 24 and 8 bits/pixel pictures
		goto exit;

	wLeftMin = wWidth;
	wTopLess = wHeight;
	wRightMax = wBottomMost = 0;

	pbBits = (PBYTE) lpDIBBits;

	lpbi = (LPBITMAPINFO) lpDIBHdr;

	// define the background color
	if (lBackground < 0L)
	{
		// Background color is the color of the first pixel
		if (8 == wBPP)
		{
			rgbBackground = lpbi->bmiColors[*pbBits];
			dwBackground = (DWORD) ((rgbBackground.rgbBlue << 16) + (rgbBackground.rgbGreen << 8) + rgbBackground.rgbRed);
		}
		else
			dwBackground = (*pbBits << 16) + (*(pbBits+1) << 8) + *(pbBits+2);
	}
	else
	{
		if (8 == wBPP)
		{
			for (iColorIndex = 0; iColorIndex < 256; iColorIndex == 9 ? iColorIndex = 246 : iColorIndex++)
			{
				rgbBackground = lpbi->bmiColors[iColorIndex];
				dwBackground = (rgbBackground.rgbBlue << 16) + (rgbBackground.rgbGreen << 8) + rgbBackground.rgbRed;
				if ((DWORD) lBackground == dwBackground)
					break;
			}
			if (iColorIndex > 256)
				goto exit;
		}
		else
			dwBackground = (DWORD) lBackground;
	}

	// rectify background color - find closest to 16 basic color
	dwBackground = (DWORD) crClosestBasic((COLORREF) dwBackground);

	if (8 == wBPP)
		for (iLine = wHeight-1; iLine >= 0; iLine--)
		{
			for (iColumn = 0; iColumn < wWidth; iColumn++)
			{
				rgbBackground = lpbi->bmiColors[*pbBits];
				if (dwDeltaColor2((COLORREF) dwBackground, rgbBackground.rgbRed, rgbBackground.rgbGreen, rgbBackground.rgbBlue) > 1)
				// if (dwBackground != (DWORD) ((rgbBackground.rgbBlue << 16) + (rgbBackground.rgbGreen << 8) + rgbBackground.rgbRed))
				{
					if (iColumn < wLeftMin)		wLeftMin	= iColumn;
					if (iColumn > wRightMax)	wRightMax	= iColumn;
					if (iLine < wTopLess)		wTopLess	= iLine;
					if (iLine > wBottomMost)	wBottomMost = iLine;
				}
				pbBits++;
			}
			pbBits += (4-(wWidth%4))%4;
		}
	else
		for (iLine = wHeight-1; iLine >= 0; iLine--)
		{
			for (iColumn = 0; iColumn < wWidth; iColumn++)
			{
				// dwPixColor = (*pbBits << 16) + (*(pbBits+1) << 8) + *(pbBits+2);
				// pbBits += 3;
				if (dwDeltaColor2((COLORREF) dwBackground, *(pbBits+2), *(pbBits+1), *pbBits) > 3)
				// if (dwPixColor != dwBackground)
				{
					if (iColumn < wLeftMin)		wLeftMin	= iColumn;
					if (iColumn > wRightMax)	wRightMax	= iColumn;
					if (iLine < wTopLess)		wTopLess	= iLine;
					if (iLine > wBottomMost)	wBottomMost	= iLine;
				}
				else
					g_crBackG24 = (COLORREF) ((*pbBits << 16) + (*(pbBits+1) << 8) + *(pbBits+2));

				pbBits += 3;
			}
			pbBits += (4-((wWidth*3)%4))%4;
		}

	if (wRightMax >= wLeftMin && wBottomMost >= wTopLess)
	{
		bRet = BitBlt(hdcDest,	// handle to destination device context
					  0,		// x-coordinate of destination rectangle’s upper-left corner
					  0,		// y-coordinate of destination rectangle’s upper-left corner
					  wRightMax-wLeftMin+1,		// width of destination rectangle
					  wBottomMost-wTopLess+1,	// height of destination rectangle 
					  hdcSrc,	// handle to source device context
					  wLeftMin,	// x-coordinate of source rectangle’s upper-left corner
					  wTopLess,	// y-coordinate of source rectangle’s upper-left corner
					  SRCCOPY	// raster operation code
					 );
		if (!bRet)
			goto exit;

		*pnXDest = (int) wLeftMin;
		*pnYDest = (int) wTopLess;
		*pnWDest = (int) wRightMax-wLeftMin+1;
		*pnHDest = (int) wBottomMost-wTopLess+1;
	}


	uErr = 0;

exit:
	if (lpDIBHdr)
		GlobalUnlock(hDibSrc);

	if (hDibSrc)
	{
		GlobalFree(hDibSrc);
		CloseHandle(hDibSrc);
	}

	if (hBmpSrc)
	{
		DeleteObject(hBmpSrc);
		CloseHandle(hBmpSrc);
	}

	return uErr;
}


/******************************************************************************\
*
*  FUNCTION:    GetAura
*
*  INPUTS:      - hdcSrc:			DC of source bitmap
*               - hdcDest:			DC of destination bitmap
*               - nAuraSize:		radius of aura belt
*               - lBackground:		< 0  ==> background color is given by lower left pixel
*                                   >= 0 ==> specifies the background color (RGB value)
*               - lForcedAuraColor:	< 0  ==> aura color is opposite of background color
*                                   >= 0 ==> specifies the aura color 
*
*  RETURNS:     0 if successful, > 0 otherwise
*
\******************************************************************************/

UINT CALLBACK GetAura(HDC hdcSrc, HDC hdcDest, INT nAuraSize, LONG lBackground)
{
	DWORD			dwPixColor, dwBackground;
	INT				iColumn, iLine;
	WORD			wPS, wNC, wBPP;
	WORD			wWidth, wHeight;
    LPSTR			lpDIBHdr1 = NULL, lpDIBHdr2 = NULL, lpDIBBits;  // pointer to DIB header, pointer to DIB bits 
	PBYTE			pbBits;
	HDIB			hDibSrc, hDibTmp = NULL;
	HBITMAP			hBmpSrc, hBmpOld = NULL, hBmpTmp1 = NULL, hBmpTmp2 = NULL;
	UINT			uErr = ERROR_GEN_FAILURE;
	BOOL			bRet;
	HDC				hdcTmp = NULL;
	PBYTE			bpBitsTmp;
	BITMAPINFO256	bmpiTmp;
	LPBITMAPINFO	lpbi;
	HBRUSH			hbrush = NULL;
	RECT			rcTmp;
	INT				nCnt, nDeltaX[16], nDeltaY[16], iColorIndex;
	FLOAT			flSR2By2 = (FLOAT) 0.707;
	BYTE			byteBackR, byteBackG, byteBackB, byteBackgroundIndex = 0;
	RGBQUAD			rgbBackground;

	if (!hdcSrc || !hdcDest)
		return ERROR_INVALID_HANDLE;

	hBmpSrc = (HBITMAP) GetCurrentObject(hdcSrc, OBJ_BITMAP);

	if (!hBmpSrc)
		return ERROR_GEN_FAILURE;

	hDibSrc = BitmapToDIB(hBmpSrc, NULL); 

	if (!hDibSrc)
		goto exit;

    // lock memory block and get a pointer to it 
    lpDIBHdr1 = GlobalLock(hDibSrc); 
 
    // get a pointer to the DIB bits 
    lpDIBBits = FindDIBBits(lpDIBHdr1); 

	wWidth = (WORD) DIBWidth(lpDIBHdr1);

	wHeight = (WORD) DIBHeight(lpDIBHdr1);
	
	wPS = PaletteSize(lpDIBHdr1);
	
	wNC = DIBNumColors(lpDIBHdr1); 

	wBPP = DIBBitsPerPixel(lpDIBHdr1); 

	pbBits = (PBYTE) lpDIBBits;

	if (24 != wBPP && 8 != wBPP)	// this algo only deals with 8 and 24 bits/pixel pictures
		goto exit;

	// create a new bitmap with background color and belt as big as the AuraSize
	lpbi = (LPBITMAPINFO) lpDIBHdr1;

	CopyMemory(&bmpiTmp.bmiHeader, &(lpbi->bmiHeader), sizeof(BITMAPINFOHEADER));
	if (wNC > 0 && wNC <= 256)
		CopyMemory(&bmpiTmp.bmiColors, &(lpbi->bmiColors), wNC * sizeof(lpbi->bmiColors));

	bmpiTmp.bmiHeader.biWidth += nAuraSize*2;
	bmpiTmp.bmiHeader.biHeight += nAuraSize*2;
	if (8 == wBPP)
		bmpiTmp.bmiHeader.biSizeImage = (bmpiTmp.bmiHeader.biWidth+(4-(bmpiTmp.bmiHeader.biWidth%4))%4)*bmpiTmp.bmiHeader.biHeight;
	else
		bmpiTmp.bmiHeader.biSizeImage = bmpiTmp.bmiHeader.biWidth*bmpiTmp.bmiHeader.biHeight*3+((4-(bmpiTmp.bmiHeader.biWidth%4))%4)*bmpiTmp.bmiHeader.biHeight;

	hBmpTmp1 = CreateDIBSection(hdcSrc,
								(CONST BITMAPINFO*) &bmpiTmp,			// pointer to structure containing bitmap size, format, and color data
								DIB_RGB_COLORS,		// color data type indicator: RGB values or palette indices
								(VOID*) &bpBitsTmp,	// pointer to variable to receive a pointer to the bitmap’s bit values
								NULL,				// optional handle to a file mapping object
								0L					// offset to the bitmap bit values within the file mapping object
								);

	if (!hBmpTmp1)
		goto exit;

	hdcTmp = CreateCompatibleDC(hdcSrc);	// copy of the original DC that will 
											// hold the newly created bitmap

	// select new bitmap into compatible DC
	hBmpOld = SelectObject(hdcTmp, hBmpTmp1);

	// define the background color
	if (lBackground < 0L)
	{
		// Background color is the color of the first pixel
		if (8 == wBPP)
		{
			byteBackgroundIndex = *pbBits;
			rgbBackground = bmpiTmp.bmiColors[byteBackgroundIndex];
			dwBackground = (DWORD) ((rgbBackground.rgbBlue << 16) + (rgbBackground.rgbGreen << 8) + rgbBackground.rgbRed);
		}
		else
		{
			dwBackground = (*pbBits << 16) + (*(pbBits+1) << 8) + *(pbBits+2);
			byteBackB = *pbBits;
			byteBackG = *(pbBits+1);
			byteBackR = *(pbBits+2);
		}
	}
	else
	{
		if (8 == wBPP)
		{
			for (iColorIndex = 0; iColorIndex < 256; iColorIndex == 9 ? iColorIndex = 246 : iColorIndex++)
			{
				rgbBackground = bmpiTmp.bmiColors[iColorIndex];
				dwBackground = (rgbBackground.rgbBlue << 16) + (rgbBackground.rgbGreen << 8) + rgbBackground.rgbRed;
				if ((DWORD) lBackground == dwBackground)
				{
					byteBackgroundIndex = iColorIndex;
					break;
				}
			}
			if (byteBackgroundIndex != iColorIndex)
				goto exit;
		}
		else
		{
			dwBackground = (DWORD) lBackground;
			if (dwDeltaColor(dwBackground, g_crBackG24) < 8)
			{
				byteBackB = (BYTE) ((g_crBackG24 >> 16) & 0xFF);
				byteBackG = (BYTE) ((g_crBackG24 >> 8) & 0xFF);
				byteBackR = (BYTE) (g_crBackG24 & 0xFF);
			}
			else
				if (dwDeltaColor2(dwBackground, *(pbBits+2), *(pbBits+1), *pbBits) < 8)
				{
					byteBackB = *pbBits;
					byteBackG = *(pbBits+1);
					byteBackR = *(pbBits+2);
				}
				else
				{
					byteBackB = (BYTE) ((dwBackground >> 16) & 0xFF);
					byteBackG = (BYTE) ((dwBackground >> 8) & 0xFF);
					byteBackR = (BYTE) (dwBackground & 0xFF);
				}
		}
	}

	// rectify background color - find closest to 16 basic color
	dwBackground = (DWORD) crClosestBasic((COLORREF) dwBackground);

	if (24 == wBPP)
	{
		// paint the whole rectangle with the background color
		hbrush = CreateSolidBrush((COLORREF) (byteBackB << 16) + (byteBackG << 8) + byteBackR);
		if (!hbrush)
			goto exit;

		rcTmp.left = rcTmp.top = 0L;
		rcTmp.right = bmpiTmp.bmiHeader.biWidth+1;
		rcTmp.bottom = bmpiTmp.bmiHeader.biHeight+1;

		if (!FillRect(hdcTmp, &rcTmp, hbrush))
			goto exit;
	}
	else
		FillMemory(bpBitsTmp, (bmpiTmp.bmiHeader.biWidth+(4-(bmpiTmp.bmiHeader.biWidth%4))%4)*bmpiTmp.bmiHeader.biHeight, byteBackgroundIndex);

	// copy the original bitmap to the center if the new one. 
	bRet = BitBlt(hdcTmp,		// handle to destination device context
				  nAuraSize,	// x-coordinate of destination rectangle’s upper-left corner
				  nAuraSize,	// y-coordinate of destination rectangle’s upper-left corner
				  wWidth,		// width of destination rectangle
				  wHeight,		// height of destination rectangle 
				  hdcSrc,		// handle to source device context
				  0,			// x-coordinate of source rectangle’s upper-left corner
				  0,			// y-coordinate of source rectangle’s upper-left corner
				  SRCCOPY		// raster operation code
				 );
	if (!bRet)
		goto exit;
	
	if (24 == wBPP)
	{
		rcTmp.left = rcTmp.top = 0L;
		rcTmp.right = nAuraSize+1;
		rcTmp.bottom = bmpiTmp.bmiHeader.biHeight+1;

		if (!FillRect(hdcTmp, &rcTmp, hbrush))
			goto exit;

		rcTmp.left = bmpiTmp.bmiHeader.biWidth-nAuraSize-1;
		rcTmp.top = 0L;
		rcTmp.right = bmpiTmp.bmiHeader.biWidth+1;
		rcTmp.bottom = bmpiTmp.bmiHeader.biHeight+1;

		if (!FillRect(hdcTmp, &rcTmp, hbrush))
			goto exit;
	}

	// any non-backcolor pixel becomes black in the new bitmap
	hDibTmp = BitmapToDIB(hBmpTmp1, NULL); 

	if (!hDibTmp)
		goto exit;

    // lock memory block and get a pointer to it 
    lpDIBHdr2 = GlobalLock(hDibTmp); 
 
    // get a pointer to the DIB bits 
    lpDIBBits = FindDIBBits(lpDIBHdr2); 

	wWidth = (WORD) DIBWidth(lpDIBHdr2);

	wHeight = (WORD) DIBHeight(lpDIBHdr2);
	
	pbBits = (PBYTE) lpDIBBits;

	if (8 == wBPP)
		for (iLine = wHeight-1; iLine >= 0; iLine--)
		{
			for (iColumn = 0; iColumn < wWidth; iColumn++)
			{
				rgbBackground = bmpiTmp.bmiColors[*pbBits];
				if (dwDeltaColor2((COLORREF) dwBackground, rgbBackground.rgbRed, rgbBackground.rgbGreen, rgbBackground.rgbBlue) > 1)
				// if (dwBackground != (DWORD) ((rgbBackground.rgbBlue << 16) + (rgbBackground.rgbGreen << 8) + rgbBackground.rgbRed))
					*pbBits = 255;
				pbBits++;
			}
			pbBits += (4-(wWidth%4))%4;
		}
	else
		for (iLine = wHeight-1; iLine >= 0; iLine--)
		{
			for (iColumn = 0; iColumn < wWidth; iColumn++)
			{
				//if (dwDeltaColor2((COLORREF) dwBackground, *(pbBits+2), *(pbBits+1), *pbBits) > 1)
				//{
				//	*pbBits = 255; *(pbBits+1) = 255; *(pbBits+2) = 255;
				//}
				//else
				//{
				//	*pbBits = byteBackB; *(pbBits+1) = byteBackG; *(pbBits+2) = byteBackR;
				//}

				dwPixColor = (*pbBits << 16) + (*(pbBits+1) << 8) + *(pbBits+2);
				if (dwPixColor != dwBackground)
					if (abs(*pbBits - byteBackB) > 1 || abs(*(pbBits+1) - byteBackG) > 1 || abs(*(pbBits+2) - byteBackR) > 1)
					{
						*pbBits = 255; *(pbBits+1) = 255; *(pbBits+2) = 255;
					}
					else
					{
						*pbBits = byteBackB; *(pbBits+1) = byteBackG; *(pbBits+2) = byteBackR;
					}
				pbBits += 3;
			}
			pbBits += (4-((wWidth*3)%4))%4;
		}

	hBmpTmp2 = DIBToBitmap(hDibTmp, NULL);
	if (!hBmpTmp2)
		goto exit;

	SelectObject(hdcTmp, hBmpTmp2);

	bRet = BitBlt(hdcDest,	// handle to destination device context
				  0,		// x-coordinate of destination rectangle’s upper-left corner
				  0,		// y-coordinate of destination rectangle’s upper-left corner
				  bmpiTmp.bmiHeader.biWidth,	// width of destination rectangle
				  bmpiTmp.bmiHeader.biHeight,	// height of destination rectangle 
				  hdcTmp,	// handle to source device context
				  0,		// x-coordinate of source rectangle’s upper-left corner
				  0,		// y-coordinate of source rectangle’s upper-left corner
				  SRCCOPY	// raster operation code
				 );
	if (!bRet)
		goto exit;

	// duplicate the bitmap all around the centered one
	wWidth  = bmpiTmp.bmiHeader.biWidth-2*nAuraSize;
	wHeight = bmpiTmp.bmiHeader.biHeight-2*nAuraSize;

	for (nCnt = 0; nCnt < 16; nCnt++)
	{
		nDeltaX[nCnt] = nDeltaY[nCnt] = 0;
	}

	nDeltaX[0] = nDeltaY[4] = nAuraSize;
	nDeltaX[2] = nDeltaY[6] = -nAuraSize;

	nDeltaX[8] = nDeltaX[12] = nDeltaY[14] = nDeltaY[8] = (INT) ((LONG) nAuraSize * flSR2By2);
	nDeltaX[14] = nDeltaX[10] = nDeltaY[10] = nDeltaY[12] = (INT) ((LONG) -nAuraSize * flSR2By2);

	if (nAuraSize > 1)
	{
		nDeltaX[1] = nDeltaY[5] = nAuraSize / 2;
		nDeltaX[3] = nDeltaY[7] = -nAuraSize / 2;

		nDeltaX[9] = nDeltaX[13] = nDeltaY[15] = nDeltaY[9] = (INT) ((LONG) nAuraSize * flSR2By2) / 2;
		nDeltaX[15] = nDeltaX[11] = nDeltaY[11] = nDeltaY[13] = (INT) ((LONG) -nAuraSize * flSR2By2) / 2;
	}

	for (nCnt = 0; nCnt < 16; nCnt++)
	{
		if (nDeltaX[nCnt] || nDeltaY[nCnt])
		{
			bRet = BitBlt(hdcDest,		// handle to destination device context
						  nAuraSize+nDeltaX[nCnt],	// x-coordinate of destination rectangle’s upper-left corner
						  nAuraSize+nDeltaY[nCnt],	// y-coordinate of destination rectangle’s upper-left corner
						  wWidth,		// width of destination rectangle
						  wHeight,		// height of destination rectangle 
						  hdcTmp,		// handle to source device context
						  nAuraSize,	// x-coordinate of source rectangle’s upper-left corner
						  nAuraSize,	// y-coordinate of source rectangle’s upper-left corner
						  SRCPAINT		// raster operation code
						 );
			if (!bRet)
				uErr = ERROR_GEN_FAILURE;
		}
	}

	if (hBmpOld)
		SelectObject(hdcTmp, hBmpOld);

	uErr = 0;

exit:
	if (lpDIBHdr1)
		GlobalUnlock(hDibSrc);

	if (hDibSrc)
	{
		GlobalFree(hDibSrc);
		CloseHandle(hDibSrc);
	}

	if (lpDIBHdr2)
		GlobalUnlock(hDibTmp);

	if (hDibTmp)
	{
		GlobalFree(hDibTmp);
		CloseHandle(hDibTmp);
	}

	if (hBmpSrc)
	{
		DeleteObject(hBmpSrc);
		CloseHandle(hBmpSrc);
	}

	if (hBmpTmp1)
	{
		DeleteObject(hBmpTmp1);
		CloseHandle(hBmpTmp1);
	}

	if (hBmpTmp2)
	{
		DeleteObject(hBmpTmp2);
		CloseHandle(hBmpTmp2);
	}

	if (hdcTmp)
		DeleteDC(hdcTmp);

	if (hbrush)
	{
		DeleteObject(hbrush);
		CloseHandle(hbrush);
	}

	return uErr;
}
