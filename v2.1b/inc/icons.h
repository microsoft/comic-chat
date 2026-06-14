/****************************************************************************\
*			 
*     Extracted from MSDN October 1996 Edition.
*			 
*     FILE:     Icons.H
*
*     PURPOSE:  IconPro Project Icon handling header file
*
*     COMMENTS: 
*               
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
// Structs

// These first two structs represent how the icon information is stored
// when it is bound into a EXE or DLL file. Structure members are WORD
// aligned and the last member of the structure is the ID instead of
// the imageoffset.
#pragma pack( push )
#pragma pack( 2 )

typedef struct
{
	BYTE	bWidth;               // Width of the image
	BYTE	bHeight;              // Height of the image (times 2)
	BYTE	bColorCount;          // Number of colors in image (0 if >=8bpp)
	BYTE	bReserved;            // Reserved
	WORD	wPlanes;              // Color Planes
	WORD	wBitCount;            // Bits per pixel
	DWORD	dwBytesInRes;         // how many bytes in this resource?
	WORD	nID;                  // the ID
} MEMICONDIRENTRY, *PMEMICONDIRENTRY;

typedef struct 
{
	WORD			idReserved;   // Reserved
	WORD			idType;       // resource type (1 for icons)
	WORD			idCount;      // how many images?
	MEMICONDIRENTRY	idEntries[1]; // the entries for each image
} MEMICONDIR, *PMEMICONDIR;

#pragma pack( pop )

// The following two structs are for the use of this program in
// manipulating icons. They are more closely tied to the operation
// of this program than the structures listed above. One of the
// main differences is that they provide a pointer to the DIB
// information of the masks.
typedef struct
{
	UINT			uWidth, uHeight, uColors;	// Width, Height and bpp
	PBYTE			pbBits;						// ptr to DIB bits
	DWORD			dwNumBytes;					// how many bytes?
	LPBITMAPINFO	pbi;						// ptr to header
	PBYTE			pbXOR;						// ptr to XOR image bits
	PBYTE			pbAND;						// ptr to AND image bits
} ICONIMAGE, *PICONIMAGE;

typedef struct
{
	UINT		uNumImages;                      // How many images?
	ICONIMAGE	IconImages[1];                   // Image entries
} ICONRESOURCE, *PICONRESOURCE;
/****************************************************************************/
