#ifndef __AVBFILE_H__
#define __AVBFILE_H__


#include "dib.h"

// Some color conversion stuff.
#define GET_COLORREF_FROM_RGBQUAD(prgb) \
	RGB((prgb)->rgbRed, (prgb)->rgbGreen, (prgb)->rgbBlue)
#define SET_RGBQUAD_FROM_COLORREF(prgb, clrref) \
	*(COLORREF*)(prgb) = RGB(GetBValue (clrref), GetGValue(clrref), GetRValue(clrref))

#pragma pack(push, 1)

// ============================================================================
// Defines that can be used with the AVBFILE code.
// Turn these defines on or off to add/remove functionality.
//
// AVATAR_NOT_CLIENT: Used in programs that aren't the chat client,
//	e.g. file creators and copiers.
// 			#define AVATAR_NOT_CLIENT
//
// AVATAR_WRITE: Capable of writing Avatars
//			#define AVATAR_WRITE
//
// AVATAR_READ: Capable of reading Avatars
//			#define AVATAR_READ

#define AVATAR_READ
//#define AVATAR_WRITE
//#define AVATAR_NOT_CLIENT

// ZLIB library interface. This is kept in a namespace for convenience.

namespace ZLIB
{
	extern "C" int compress (void * pbDest, ULONG * pulDest, const void * pbSrc, ULONG ulSrc);
	extern "C" int compress2 (void *  pbDest, ULONG * destLen, const void * pbSrc, ULONG ulSrc, int nLevel);
	extern "C" int uncompress (void * pbDest, ULONG * pulDest, const void * pbSrc, ULONG ulSrc);
};
#define ZLIB_COMPRESSION_LEVEL 9

// Some typedefs.

typedef UCHAR 	INT8;
typedef	USHORT 	INT16;
typedef	ULONG	INT32;

// Magic numbers for Avatar files. The old file used to have the magic number
// 0x81, but we changed it to 0x8181 for the new format.

#define AF_MAGICNUM			0x81
#define AF_MAGICNUM_NEW		0x8181

// Avatar file types. AT_SIMPLE is a simple avatar, AT_COMPLEX is a complex
// avatar, and AT_BACKGROUND is a background.

#define AT_SIMPLE		1
#define AT_COMPLEX		2
#define AT_BACKDROP  	3

// Current version
#define AVATAR_CURRENT_VERSION 2

// Miscellaneous defines
#define INVALID_POSE_ID 0
#define AVSTREAM_ERROR		(-1L)
#define MAX_COMPRESSBUFFERSIZE	(2048L * 1024L)		// 2Mb
#define MAX_PALETTE_SIZE (2048)


// ============================================================================
// ENUM: AVATARIMAGEFORMAT
// Image format as stored in a file.
// 		AIF_DIB				DIB (same format as a .BMP file)
//		AIF_LZDEFLATE		Deflate compressed 

enum AVATARIMAGEFORMAT {
	AIF_DIB = 0,
	AIF_LZDEFLATE = 1,
};

// ============================================================================
// ENUM: AVATARIMAGEPALETTE
// Picture format as stored in a file.
// 		AIP_NOPALETTE		No palette or palette defined by internal file
//		AIP_GLOBALPALETTE	Uses global palette
//		AIP_LOCALPALETTE	Uses local palette 
//							Pointer to image data has a palette record, immediately
//							followed by actual image data.
//		AIP_MONOCHROME		Monochrome image (1 bpp)
//		AIP_MASKEDMONO		Monochrome image with a mask (2 bpp).
//							00 = blank, 01 = aura, 10 = black, 11 = white.
//		AIP_DUALMASK		Two masks in one 2 bpp bitmap
//							Bit 0 = mask 1, bit 1 = mask 2

enum AVATARIMAGEPALETTE {
	AIP_NOPALETTE = 0,
	AIP_GLOBALPALETTE = 1,
	AIP_LOCALPALETTE = 2,
	AIP_MONOCHROME = 3,
	AIP_MASKEDMONO = 4,
	AIP_DUALMASK = 5,
};

// ============================================================================
// STRUCT: AVATARHEADER
// Header at the start of a file.

struct AVATARHEADER
{
	INT16 nMagicNum;
	INT16 nType;
	INT16 nVersion;
};

// ============================================================================
// STRUCT: AVATARICONDATA
// Data stored for an icon.

struct AVATARICONDATA
{
	INT32 dwOffset;
	INT8  byFormat;
	INT8  byPalette;
};

// ============================================================================
// UNION: AVATARBODYDATA
// Data stored for a body. The old version was slightly larger (with padding
// bytes), but had less information.

union AVATARBODYDATA
{
	struct	// Don't use the olddata struct, it's there for compatibility only
	{
		INT32 dwImageOffset;
		INT32 dwMaskOffset;
		INT32 dwAuraOffset;
		INT16 nEmotion;
		BYTE  byIntensity;
		INT16 x;
		INT16 y;
		BYTE  byPadding[16];
	} olddata;
	struct
	{
		INT32 dwImageOffset;
		INT32 dwMaskOffset;
		INT32 dwAuraOffset;
		INT16 nEmotion;
		BYTE  byIntensity;
		INT16 x;
		INT16 y;
		BYTE  byImageFormat;
		BYTE  byMaskFormat;
		BYTE  byAuraFormat;
		BYTE  byImagePaletteType;
		BYTE  byMaskPaletteType;
		BYTE  byAuraPaletteType;
	} newdata;
};

// ============================================================================
// UNION: AVATARFACEDATA
// Data stored for a face. The old version was slightly larger (with padding
// bytes), but had less information.

union AVATARFACEDATA
{
	struct	// Don't use the olddata struct, it's there for compatibility only
	{
		INT32 dwImageOffset;
		INT32 dwMaskOffset;
		INT32 dwAuraOffset;
		INT16 nEmotion;
		BYTE  byIntensity;
		INT16 cx;
		INT16 cy;
		INT16 cxDelta;
		INT16 cyDelta;
		INT16 x;
		INT16 y;
		BYTE  byPadding[16];
	} olddata;
	struct
	{
		INT32 dwImageOffset;
		INT32 dwMaskOffset;
		INT32 dwAuraOffset;
		INT16 nEmotion;
		BYTE  byIntensity;
		INT16 cx;
		INT16 cy;
		INT16 cxDelta;
		INT16 cyDelta;
		INT16 x;
		INT16 y;
		BYTE  byImageFormat;
		BYTE  byMaskFormat;
		BYTE  byAuraFormat;
		BYTE  byImagePaletteType;
		BYTE  byMaskPaletteType;
		BYTE  byAuraPaletteType;
	} newdata;
};

// ============================================================================
// UNION: AVATARTORSODATA
// Data stored for a torso. The old version was slightly larger (with padding
// bytes), but had less information.

union AVATARTORSODATA
{
	struct	// Don't use the olddata struct, it's there for compatibility only
	{
		INT32 dwImageOffset;
		INT32 dwMaskOffset;
		INT32 dwAuraOffset;
		INT16 nEmotion;
		BYTE  byIntensity;
		INT16 cx;
		INT16 cy;
		BYTE  byPadding[16];
	} olddata;
	struct
	{
		INT32 dwImageOffset;
		INT32 dwMaskOffset;
		INT32 dwAuraOffset;
		INT16 nEmotion;
		BYTE  byIntensity;
		INT16 cx;
		INT16 cy;
		BYTE  byImageFormat;
		BYTE  byMaskFormat;
		BYTE  byAuraFormat;
		BYTE  byImagePaletteType;
		BYTE  byMaskPaletteType;
		BYTE  byAuraPaletteType;
	} newdata;
};

// ============================================================================
// ENUM: AVATARRECORDTYPE
// Record codes for each record. Records numbered less than 256 are from the 
// old file format. Records numbered 256 or higher are newer records, and have
// a 2-byte record size right after it. This will allow avatars from future versions
// to be compatible with various versions of the client, which could be important
// if there are several versions floating around exchanging avatars.

enum AVATARRECORDTYPE
{
	AK_NAME=1,
	AK_FLAGS=2,
	AK_ICON=3,
	AK_NFACES=4,
	AK_NTORSOS=5,
	AK_STARTDATA=6,
	AK_ENDDATA=7,
	AK_STYLE=8,
	AK_NBODIES=9,
	AK_NFACES2=10,
	AK_NTORSOS2=11,
	AK_NBODIES2=12,

	AK_ICON_NEW=256,
	AK_COLORPALETTE=257,
	AK_BACKDROP=258,
	AK_COPYRIGHT=259,
	AK_ORIGINAL_URL=260,
	AK_OVERRIDE_URL=261,
	AK_USAGE_FLAGS=262,
	AK_OFFSET_ADJUSTMENT=263,
};

// ============================================================================
// CLASS: CAvatarStream
// A generic source for an AVB file stream. Very similar to a generic I/O 
// stream. Provides standard Write/ReadXXXX functions, as well as some basic
// virtual functions for stream I/O, which are implemented by derived classes.

class CAvatarStream
{
public:
   #if defined(AVATAR_WRITE)
	BOOL Write32(INT32 val32)
		{ return Write (&val32, sizeof(val32)) == sizeof(val32); }
	BOOL Write16(INT16 val16)
		{ return Write (&val16, sizeof(val16)) == sizeof(val16); }
	BOOL Write8(INT8 val8)
		{ return Write (&val8, sizeof(val8)) == sizeof(val8); }
	BOOL WriteString(LPCTSTR pszVal);
	BOOL WriteCompressedBuffer(const void * pvData, UINT cbData);
	BOOL BeginVariableSection(LPDWORD pdwVariableSection);
	BOOL EndVariableSection(DWORD dwVariableSection);
   #endif // AVATAR_WRITE
	BOOL Read32(INT32 * pval32)
		{ return Read (pval32, sizeof(*pval32)) == sizeof(*pval32); }
	BOOL Read16(INT16 * pval16)
		{ return Read (pval16, sizeof(*pval16)) == sizeof(*pval16); }
	BOOL Read8(INT8 * pval8)
		{ return Read (pval8, sizeof(*pval8)) == sizeof(*pval8); }
	BOOL ReadString(LPTSTR pszVal, UINT cbBufMax);
   #if defined(AVATAR_READ)
	BOOL AllocAndReadCompressedBuffer(void * * pvData, UINT * pcbData);
   #endif // AVATAR_READ
	
	virtual BOOL Open() = 0;
	virtual BOOL Close() = 0;
   #if defined(AVATAR_WRITE)
	virtual UINT Write(const void * pvData, UINT cbData) = 0;
   #endif // AVATAR_WRITE
	virtual UINT Read(LPVOID pvData, UINT cbData) = 0;
	virtual long GetPosition() = 0;
	virtual BOOL SetPosition(long lPosition, UINT nFrom) = 0;
};

// ============================================================================
// CLASS: CAvatarFileStream
// CAvatarStream implemented over standard C runtime FILE handles.

class CAvatarFileStream : public CAvatarStream
{
public:
	CAvatarFileStream(LPCTSTR pszFile, BOOL bWrite = FALSE);
	virtual ~CAvatarFileStream();

	virtual BOOL Open();
	virtual BOOL Close();
   #if defined(AVATAR_WRITE)
	virtual UINT Write(const void * pvData, UINT cbData);
   #endif // AVATAR_WRITE
	virtual UINT Read(LPVOID pvData, UINT cbData);
	virtual long GetPosition();
	virtual BOOL SetPosition(long lPosition, UINT nFrom);

protected:
	TCHAR m_szFileName[_MAX_PATH];
	UINT  m_nOpenCount;
	FILE* m_file;
   #if defined(AVATAR_WRITE)
	BOOL  m_bWrite;
   #endif
};

// ============================================================================
// CLASS: CAvatarPalette
// The palette class

class CAvatarPalette
{
public:
	CAvatarPalette()
		{ m_nColorCount = 0; m_pclrref = NULL; }
	~CAvatarPalette()
		{ free (m_pclrref); }
	BOOL SetFrom(const COLORREF * pclrrefSrc, int nCount);
   #if defined(AVATAR_READ)
	BOOL Read(CAvatarStream * pStream);
   #endif // AVATAR_READ
   #if defined(AVATAR_WRITE)
    BOOL SetFrom(const RGBQUAD * prgbSrc, int nCount);
	BOOL Write(CAvatarStream * pStream);
	BOOL WriteAsRecord(CAvatarStream * pStream);
   #endif // AVATAR_WRITE
	UINT		m_nColorCount;
	COLORREF *  m_pclrref;
};

// ============================================================================
// CLASS: CAvatarDIB
// Basic derivative of CDIB class. The CDIB class does not support our stream
// class, so we have created and used our own derived class here.

class CAvatarDIB : public CDIB
{
public:
	virtual BOOL Load(CAvatarStream * pStream);
   #if defined(AVATAR_WRITE)
    virtual BOOL Save(CAvatarStream * pStream);
	BOOL ForceToMonochrome();
   #endif // AVATAR_WRITE
    BOOL Create(BITMAPINFO* pBMI, BYTE* pBits); // Create from existing mem
};


// ============================================================================
// STRUCT: AVATARIMAGE
// A memory reference to an image. This struct should not contain any virtual
// functions or constructor/destructors - it should essentially be a C-style struct.

struct AVATARIMAGE
{
public:
	// These fields only refer to what is in the file.
	DWORD 			 m_dwStreamOffset;
	BYTE    		 m_byFormat;
	BYTE			 m_byPaletteType;
	CAvatarPalette * m_pGlobalPalette;
	
	// These fields only refer to what is in memory.
	CAvatarDIB *    m_pDib;
};

// ============================================================================
// CLASS: CAvatarFileImage
// An image in a stream. The image is always loaded or saved at the current
// stream position.

class CAvatarFileImage
{
public:
	CAvatarFileImage(AVATARIMAGE * pImage)
		{ ASSERT(pImage != NULL); m_pImage = pImage; }
   #if defined(AVATAR_READ)
	virtual BOOL Read(CAvatarStream * pStream) = 0;
   #endif // AVATAR_READ
   #if defined(AVATAR_WRITE)
	virtual BOOL Write(CAvatarStream * pStream) = 0;
   #endif // AVATAR_WRITE
   
protected:
   #if defined(AVATAR_READ)
	BOOL SetProperPosition(CAvatarStream * pStream);
	BOOL GetProperPalette(CAvatarStream * pStream, CAvatarPalette * pPalette);
   #endif // AVATAR_READ
	AVATARIMAGE * m_pImage;
};

// ============================================================================
// CLASS: CAvatarFileDIBImage
// An image stored as a DIB (.BMP) format.

class CAvatarFileDIBImage : public CAvatarFileImage
{
public:
	CAvatarFileDIBImage(AVATARIMAGE * pImage) : CAvatarFileImage(pImage) { };
   #if defined(AVATAR_READ)
	virtual BOOL Read(CAvatarStream * pStream);
   #endif // AVATAR_READ
   #if defined(AVATAR_WRITE)
	virtual BOOL Write(CAvatarStream * pStream);
   #endif // AVATAR_WRITE
};

class CAvatarFileZlibImage : public CAvatarFileImage
{
public:
	CAvatarFileZlibImage(AVATARIMAGE * pImage) : CAvatarFileImage(pImage) { };
   #if defined(AVATAR_READ)
	virtual BOOL Read(CAvatarStream * pStream);
   #endif
   #if defined(AVATAR_WRITE)
	virtual BOOL Write(CAvatarStream * pStream);
   #endif // AVATAR_WRITE
};

#if defined(AVATAR_WRITE)

// ============================================================================
// CLASS: CAvatarFileResourceResolver
// A little class that lets you write a reference to a particular resource
// in the stream. Then, after you write out all the resources, it can 
// automatically fix up all references to the resource in the file.
// Important note: the code in this class generates exceptions. For efficiency, 
// handling of these exceptions is not done within the class. If one of the
// functions raises an exception, the return code should be taken to be FALSE.

class CAvatarFileResourceResolver
{
public:
	CAvatarFileResourceResolver(CAvatarStream * pStream)
		{ m_pStream = pStream; }
	BOOL WriteResourceReference(PVOID pvResource);
	BOOL MarkResourcePosition(PVOID pvResource);
	BOOL FixupReferences();
protected:
	CMapPtrToPtr m_mapResourceReferenceToPtr;
	CMapPtrToPtr m_mapPtrToResourceOffset;
	CAvatarStream * m_pStream;
};

#endif //AVATAR_WRITE

#pragma pack(pop)

#endif // __AVBFILE_H__
