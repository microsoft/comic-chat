/****************************************************************************\
*			 
*     Extracted from MSDN October 1996 Edition.
*
*     FILE:     DIB.H
*
*     PURPOSE:  IconPro Project DIB handling header file
*
*     COMMENTS: Icons are stored in something almost identical to DIB
*               format, which makes it real easy to treat them as DIBs
*               when manipulating them.
*
*     Copyright 1995 Microsoft Corp.
*
*
* History:
*                July '95 - Created
*				 November '96 - Modified by RegisB for CDocSvr control
*
\****************************************************************************/


/****************************************************************************/
// local #defines

// How wide, in bytes, would this many bits be, DWORD aligned?
#define WIDTHBYTES(bits)      ((((bits) + 31)>>5)<<2)
/****************************************************************************/


/****************************************************************************/
// Exported function prototypes
extern LPSTR FindDIBBits (LPSTR lpbi);
extern WORD DIBNumColors (LPSTR lpbi);
extern WORD PaletteSize (LPSTR lpbi);
extern DWORD BytesPerLine( LPBITMAPINFOHEADER lpBMIH );
/****************************************************************************/
