/****************************************************************************\
*			 
*     Extracted from MSDN October 1996 Edition.
*
*     FILE:     DIB.C
*
*     PURPOSE:  DIB functions for IconPro Project
*
*     COMMENTS: Icons are stored in a format almost identical to DIBs. For
*               this reason, it is easiest to deal with the individual
*               icon images as DIBs (or DIBSections). This has the added
*               advantage of retaining color depth even on low-end displays.
*
*     FUNCTIONS:
*      EXPORTS: 
*               FindDIBBits()      - Locate the image bits in a DIB
*               DIBNumColors()     - Calculate number of color table entries
*               PaletteSize()      - Calculate number of color table bytes
*               BytesPerLine()     - Calculate number of bytes per scan line
*
*     Copyright 1995 Microsoft Corp.
*
*
* History:
*                July '95 - Created
*				 November '96 - Modified by RegisB for CDocSvr control
*
\****************************************************************************/
#include <Windows.h>
#include "Dib.H"


/****************************************************************************/
/* Local Function Prototypes */
BOOL CopyColorTable( LPBITMAPINFO lpTarget, LPBITMAPINFO lpSource );
/****************************************************************************/



/****************************************************************************
*
*     FUNCTION: FindDIBits
*
*     PURPOSE:  Locate the image bits in a CF_DIB format DIB.
*
*     PARAMS:   LPSTR lpbi - pointer to the CF_DIB memory block
*
*     RETURNS:  LPSTR - pointer to the image bits
*
* History:
*                July '95 - Copied <g>
*
\****************************************************************************/
LPSTR FindDIBBits( LPSTR lpbi )
{
   return ( lpbi + *(LPDWORD)lpbi + PaletteSize( lpbi ) );
}
/* End FindDIBits() *********************************************************/



/****************************************************************************
*
*     FUNCTION: DIBNumColors
*
*     PURPOSE:  Calculates the number of entries in the color table.
*
*     PARAMS:   LPSTR lpbi - pointer to the CF_DIB memory block
*
*     RETURNS:  WORD - Number of entries in the color table.
*
* History:
*                July '95 - Copied <g>
*
\****************************************************************************/
WORD DIBNumColors( LPSTR lpbi )
{
    WORD wBitCount;
    DWORD dwClrUsed;

    dwClrUsed = ((LPBITMAPINFOHEADER) lpbi)->biClrUsed;

    if (dwClrUsed)
        return (WORD) dwClrUsed;

    wBitCount = ((LPBITMAPINFOHEADER) lpbi)->biBitCount;

    switch (wBitCount)
    {
        case 1: return 2;
        case 4: return 16;
        case 8:	return 256;
        default:return 0;
    }
    return 0;
}
/* End DIBNumColors() ******************************************************/



/****************************************************************************
*
*     FUNCTION: PaletteSize
*
*     PURPOSE:  Calculates the number of bytes in the color table.
*
*     PARAMS:   LPSTR lpbi - pointer to the CF_DIB memory block
*
*     RETURNS:  WORD - number of bytes in the color table
*
*
* History:
*                July '95 - Copied <g>
*
\****************************************************************************/
WORD PaletteSize( LPSTR lpbi )
{
    return ( DIBNumColors( lpbi ) * sizeof( RGBQUAD ) );
}
/* End PaletteSize() ********************************************************/



/****************************************************************************
*
*     FUNCTION: BytesPerLine
*
*     PURPOSE:  Calculates the number of bytes in one scan line.
*
*     PARAMS:   LPBITMAPINFOHEADER lpBMIH - pointer to the BITMAPINFOHEADER
*                                           that begins the CF_DIB block
*
*     RETURNS:  DWORD - number of bytes in one scan line (DWORD aligned)
*
* History:
*                July '95 - Created
*
\****************************************************************************/
DWORD BytesPerLine( LPBITMAPINFOHEADER lpBMIH )
{
    return WIDTHBYTES(lpBMIH->biWidth * lpBMIH->biPlanes * lpBMIH->biBitCount);
}
/* End BytesPerLine() ********************************************************/
