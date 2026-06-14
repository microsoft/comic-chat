#include "avbfile.h"

// ============================================================================
// CLASS: CPose
// Class encapsulating a single pose of an avatar.

struct MONOBITMAPINFO 
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD			 bmiColors[2];
};
struct TWOBITBITMAPINFO 
{
	BITMAPINFOHEADER bmiHeader;
	RGBQUAD			 bmiColors[4];
};

class CPose
{
public:
   #if defined(AVATARFILER)
	CPose(LPCTSTR * ppszFiles);
   #else 
	CPose(LPDWORD pdwOffsets, LPBYTE pbyFormats, LPBYTE pbyPaletteTypes);
   #endif
	~CPose();
	BOOL Load(CAvatarStream * pStream, CAvatarPalette * pGlobalPalette);
   #if defined(AVATAR_WRITE)
	BOOL Save(CAvatarStream * pStream, CAvatarFileResourceResolver &frr);
	static BOOL SaveDIB(CAvatarStream * pStream, CAvatarDIB * pDIB, BYTE byFormat, BYTE byPaletteType, CAvatarFileResourceResolver &frr, PVOID pvMarkResource);
	CAvatarDIB * CombineTwoMonochromeImages(CAvatarDIB * pDIB1, CAvatarDIB * pDIB2);
	CAvatarDIB * CombineThreeMonochromeImages(CAvatarDIB * pDIB1, CAvatarDIB * pDIB2, CAvatarDIB * pDIB3);
   #endif // AVATAR_WRITE
	BOOL ConvertFromMaskedMono(CAvatarDIB * pSrcDIB);
	BOOL ConvertFromDualMask(CAvatarDIB * pSrcDIB);
	BOOL ConvertMasksCommon(CAvatarDIB * pSrcDIB, CAvatarDIB * * pDIBsOut, int nNumDIBs);

   #if defined(AVATARFILER)
    CString 		m_strFiles[3];
   #else
	DWORD			m_dwOffsets[3];
   #endif
	BYTE  			m_byFormats[3];
	BYTE  			m_byPaletteTypes[3];
	CAvatarDIB * 	m_pdibs[3];
};

// ============================================================================
// The structs/classes below aren't commented, because they are simple versions
// of classes already in Comic Chat code.

struct FACEREC 
{
	unsigned short poseID;
	UINT  emotion;
	float intensity;
	short xCX;
	short yCX;
	short delta_xCX;
	short delta_yCX;
	UCHAR faceX;
	UCHAR faceY;
};

struct BODYREC
{
	unsigned short poseID;
	UINT  emotion;
	float intensity;
	short xCX;
	short yCX;
};

struct RBODYREC
{
	unsigned short poseID;
	UINT  emotion;
	float intensity;
	UCHAR faceX;
	UCHAR faceY;
};


class CAvatarX 
{
public:
	CAvatarX();
	virtual ~CAvatarX();
   #if defined(AVATAR_READ)
	static CAvatarX* LoadAvatar(CAvatarStream * pStream);
   #endif //AVATAR_READ
	
	void SetStream(CAvatarStream * pStream)
		{ m_pStream = pStream; }
   #if defined(AVATAR_WRITE)
	BOOL Save(CAvatarStream * pStream);
   #endif
   #if defined(AVATAR_NOT_CLIENT)
	BOOL LoadAllImages();
   #endif // AVATAR_NOT_CLIENT
    int GetPoseCount()
		{ return m_arrPoses.GetSize (); }
    CPose* GetPoseFromID(USHORT nID)
		{ return (nID > 0) ? m_arrPoses[nID - 1] : NULL; }

	char *m_name;
	UCHAR m_style;
	UCHAR m_flags;
	USHORT m_icon;
	CAvatarPalette m_palette;
	CAvatarStream * m_pStream;
	char *m_pszOriginalURL;
	char *m_pszNewURL;
	char *m_pszCopyright;
	BYTE m_byUsageFlags;

	CTypedPtrArray<CPtrArray, CPose *> m_arrPoses;

   #if defined(AVATAR_READ)
	virtual BOOL HandleLoadTag(CAvatarStream * pStream, INT16 tag, INT16 size, long & nResourcesAdjustment);
	USHORT CreatePose(CAvatarStream * pStream, DWORD dwOffset, BYTE byFormat, BYTE byPaletteType);
	USHORT CreatePoseWithMask(CAvatarStream * pStream, LPDWORD pdwOffsets, LPBYTE pbyFormats, LPBYTE pbyPaletteTypes);
   #endif // AVATAR_READ
   #if defined(AVATAR_WRITE)
	virtual INT16 GetAvatarType() = 0;
	virtual BOOL SaveSpecifics(CAvatarStream * pStream, CAvatarFileResourceResolver& frr) = 0;
   #endif // AVATAR_READ
   #if defined(AVATARFILER)
	USHORT CreatePose(LPCTSTR pszFile, LPCTSTR pszFileMask = NULL, LPCTSTR pszFileAura = NULL);
   #endif // AVATARFILER
};


class CAvatarSimple : public CAvatarX 
{
public:
	CAvatarSimple()
		{ bRec = NULL; m_nBodies = 0; }
	virtual ~CAvatarSimple()
		{ free (bRec); }
	
	RBODYREC *bRec;
	short m_nBodies;

   #if defined(AVATAR_READ)
	virtual BOOL HandleLoadTag(CAvatarStream * pStream, INT16 tag, INT16 size, long & nResourcesAdjustment);
	BOOL LoadBodyRecs(CAvatarStream * pStream, BOOL bOldTag, long & nResourcesAdjustment);
   #endif // AVATAR_READ
   #if defined(AVATAR_WRITE)
	virtual INT16 GetAvatarType()
		{ return AT_SIMPLE; }
	virtual BOOL SaveSpecifics(CAvatarStream * pStream, CAvatarFileResourceResolver& frr);
   #endif // AVATAR_WRITE
};

class CAvatarComplex : public CAvatarX 
{
public:
	CAvatarComplex()
		{ bRec = NULL; fRec = NULL; nFaces = nTorsos = 0; }
	virtual ~CAvatarComplex()
		{ free (bRec); free (fRec); }
	
	FACEREC *fRec;
	BODYREC *bRec;
	short nFaces;
	short nTorsos;

   #if defined(AVATAR_READ)
	virtual BOOL HandleLoadTag(CAvatarStream * pStream, INT16 tag, INT16 size, long & nResourcesAdjustment);
	BOOL LoadFaceRecs(CAvatarStream * pStream, BOOL bOldTag, long & nResourcesAdjustment);
	BOOL LoadTorsoRecs(CAvatarStream * pStream, BOOL bOldTag, long & nResourcesAdjustment);
   #endif // AVATAR_READ
   #if defined(AVATAR_WRITE)
	virtual INT16 GetAvatarType()
		{ return AT_COMPLEX; }
	virtual BOOL SaveSpecifics(CAvatarStream * pStream, CAvatarFileResourceResolver& frr);
   #endif // AVATAR_WRITE
};

class CChatBackdrop
{
public:
	CChatBackdrop()
		{ m_pDIB = NULL; m_pszOrigURL = m_pszNewURL = m_pszCopyright = NULL; m_byUsageFlags = 0; }
	virtual ~CChatBackdrop()
		{ if (m_pDIB) delete m_pDIB; free (m_pszOrigURL); free (m_pszNewURL); free (m_pszCopyright); }
	static CChatBackdrop* LoadBackdrop(CAvatarStream * pStream);

   #if defined(AVATAR_READ)
	virtual BOOL Load(CAvatarStream * pStream);
	BOOL LoadFromBmp(CAvatarStream * pStream);
   #endif // AVATAR_READ
   #if defined(AVATAR_WRITE)
	virtual BOOL Save(CAvatarStream * pStream);
   #endif // AVATAR_WRITE
   
	CAvatarDIB* m_pDIB;
	LPSTR 		m_pszOrigURL;
	LPSTR 		m_pszNewURL;
	LPSTR 		m_pszCopyright;
	BYTE		m_byUsageFlags;
   #if defined(AVATAR_NOT_CLIENT)
    WORD		m_filetype;
   #endif // AVATAR_NOT_CLIENT
};

