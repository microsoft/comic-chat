// The functions in this file are only used when write capability for avatars is desired.

#include "stdafx.h"
#include "avbfile.h"
#include "avatarx.h"


#if defined(AVATAR_WRITE)

// ============================================================================
// CAvatarStream implementation

// Write a string to a stream.

BOOL 
CAvatarStream::WriteString(
LPCTSTR pszVal)
{
	ASSERT (pszVal != NULL);
	UINT nWriteLength = lstrlen (pszVal) + 1; // Null terminated
	return Write (pszVal, nWriteLength) == nWriteLength;
}

// Writes a buffer to the stream with zlib compression. The buffer is stored as
// one DWORD for the uncompressed size, one for the compressed size, and then
// the data. A buffer is allocated, so that the compression can be done in one
// shot. We use ZLIB Level 9 (best) compression.

BOOL 
CAvatarStream::WriteCompressedBuffer(
const void * pvData, 
UINT cbData)
{
	ASSERT (pvData != NULL || cbData > 0);
	
	DWORD dwWriteSize[2];
	PBYTE pbDataAlloc = NULL;

	dwWriteSize[0] = cbData;

	if (cbData == 0) {
		dwWriteSize[1] = 0;
	}
	else
	{
		// The docs say to allocate 110% + 12 bytes to be safe.
		// Sounds kinda hokey, but there's some theory behind it, apparently.
		ULONG nDestSize = cbData + cbData / 8 + 12; 
		pbDataAlloc = (PBYTE)malloc (nDestSize);
		if (pbDataAlloc == NULL) {
			TRACE("Failed to allocate memory.");
			return FALSE;
		}
		if (ZLIB::compress2 (pbDataAlloc, &nDestSize, pvData, cbData, ZLIB_COMPRESSION_LEVEL) != 0)
		{
			TRACE("ZLIB compression failed!");
			goto $abort;
		}
		dwWriteSize[1] = nDestSize;
	}

	if (Write (dwWriteSize, sizeof(dwWriteSize)) != sizeof(dwWriteSize)) {
		goto $abort;
	}

	if (dwWriteSize[1] > 0 && Write (pbDataAlloc, dwWriteSize[1]) != dwWriteSize[1]) {
		goto $abort;
	}

	free (pbDataAlloc);
	return TRUE;

   $abort:
	// Error, clean up.
	free (pbDataAlloc);
    return FALSE;
}

// Begin a variable length section that is marked by a 2-byte length marker.
// The DWORD passed in is written to by this function - when the caller ends the
// section, they must call EndVariableSection with the same variable.

BOOL 
CAvatarStream::BeginVariableSection(
LPDWORD pdwVariableSection)
{
	if (!Write16 (0)) {
		return FALSE;
	}
	*pdwVariableSection = (DWORD)GetPosition ();
	return *pdwVariableSection != (DWORD)AVSTREAM_ERROR;
}

// Figures out how much was written from the start of the section, goes back,
// puts the amount in the file at the marked spot, then returns to the right
// place in the stream.

BOOL 
CAvatarStream::EndVariableSection(
DWORD dwVariableSection)
{
	long lCurPos = GetPosition ();
	return lCurPos != AVSTREAM_ERROR &&
		SetPosition ((long)dwVariableSection - sizeof(INT16), SEEK_SET) &&
		Write16 ((INT16)(lCurPos - (long)dwVariableSection)) &&
		SetPosition (lCurPos, SEEK_SET);
}

// ============================================================================
// CAvatarFileStream implementation

// Write data to a stream, returning the number of bytes written, 
// or AVSTREAM_ERROR for an error.

UINT 
CAvatarFileStream::Write(
const void * pvData, 
UINT cbData)
{
	return m_file != NULL ? fwrite (pvData, 1, cbData, m_file) : (UINT)AVSTREAM_ERROR;
}


// ============================================================================
// CAvatarPalette implementation

// Write the palette to the current position in the stream.

BOOL 
CAvatarPalette::Write(
CAvatarStream * pStream)
{
	ASSERT(m_nColorCount == 0 || m_pclrref != NULL);
	if (!pStream->Write16 ((INT16)m_nColorCount))
	{
		return FALSE;
	}

	if (m_nColorCount > 0) {
		// Write out the COLORREFs as three-byte values. This is the most optimal
		// way of saving them, since they don't compress very well.
		for (UINT i = 0; i < m_nColorCount; i++) {
			if (pStream->Write (m_pclrref + i, 3) != 3) {
				return FALSE;
			}
		}
	}

	return TRUE;
}

// Write the palette to the current position in the stream, as an
// AK_COLORPALETTE record.

BOOL 
CAvatarPalette::WriteAsRecord(
CAvatarStream * pStream)
{
	DWORD dwVariableSection;
	return pStream->Write16 (AK_COLORPALETTE) &&
			pStream->BeginVariableSection (&dwVariableSection) &&
			Write (pStream) &&
			pStream->EndVariableSection (dwVariableSection);
}

// Sets the palette from RGBQUADs, instead of COLORREFs.

BOOL 
CAvatarPalette::SetFrom(
const RGBQUAD * prgbSrc, 
int nCount)
{
	ASSERT(nCount == 0 || prgbSrc != NULL);
	ASSERT(m_pclrref == NULL);
	m_nColorCount = nCount;
	if (nCount > 0) {
		m_pclrref = (COLORREF *)malloc (nCount * sizeof(COLORREF));
		if (m_pclrref == NULL) {
			return FALSE;
		}
		for (int i = nCount - 1; i >= 0; i--)
		{
			m_pclrref[i] = GET_COLORREF_FROM_RGBQUAD(prgbSrc + i);
		}
	}
	return TRUE;

}

// ============================================================================
// CAvatarDIB implementation

// Save a DIB to the current position in the stream.

BOOL 
CAvatarDIB::Save(
CAvatarStream * pStream)
{
    BITMAPFILEHEADER bfh;

	ASSERT (m_pBMI);

    int iColors;
    int iColorTableSize;
    iColors = NumDIBColorEntries ((LPBITMAPINFO)m_pBMI);
    iColorTableSize = iColors * sizeof(RGBQUAD);

    // Construct the file header.
    bfh.bfType = 0x4D42; // 'BM'
    bfh.bfSize = 
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        iColorTableSize + 
        StorageWidth() * DibHeight();
    bfh.bfReserved1 = 0;
    bfh.bfReserved2 = 0;
    bfh.bfOffBits =
        sizeof(BITMAPFILEHEADER) +
        sizeof(BITMAPINFOHEADER) +
        iColorTableSize;

    // Write the file header.
    UINT iSize = sizeof(bfh);
    if (pStream->Write (&bfh, iSize) != iSize) {
        TRACE("Failed to write file header");
        return FALSE;
    }

    // Write the BITMAPINFO structure.
    ASSERT(m_pBMI);
    iSize = sizeof(BITMAPINFOHEADER) + iColorTableSize;
    if (pStream->Write (m_pBMI, iSize) != iSize) {
        TRACE("Failed to write BITMAPINFO");
        return FALSE;
    }

    // Write the bits.
    iSize = StorageWidth() * DibHeight();
    if (pStream->Write (m_pBits, iSize) != iSize) {
        TRACE("Failed to write bits");
        return FALSE;
    }

    return TRUE;
}

// Convert a DIB to monochrome, replacing the old contents.

BOOL 
CAvatarDIB::ForceToMonochrome()
{
	BOOL bReverseOnly = FALSE;

	// May already be monochrome!
	if (m_pBMI->bmiHeader.biBitCount == 1) {
		// We require monochrome bitmaps to be use a color table of 
		// 0=white, 1=black. If this isn't so, then we need to convert the DIB.
		RGBQUAD* pColors = (RGBQUAD*)(((LPBYTE)m_pBMI) + m_pBMI->bmiHeader.biSize);
		if (GET_COLORREF_FROM_RGBQUAD(pColors) != RGB(0, 0, 0)) {
			// This one's okay.
			return TRUE;
		}
		bReverseOnly = TRUE;
	}

	MONOBITMAPINFO bmiDest;

	// Set up the BITMAPINFO.
	bmiDest.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiDest.bmiHeader.biWidth = m_pBMI->bmiHeader.biWidth;
	bmiDest.bmiHeader.biHeight = m_pBMI->bmiHeader.biHeight;
	bmiDest.bmiHeader.biPlanes = 1;
	bmiDest.bmiHeader.biBitCount = 1;
	bmiDest.bmiHeader.biCompression = BI_RGB;
	bmiDest.bmiHeader.biSizeImage = 0;
	bmiDest.bmiHeader.biXPelsPerMeter = m_pBMI->bmiHeader.biXPelsPerMeter;
	bmiDest.bmiHeader.biYPelsPerMeter = m_pBMI->bmiHeader.biYPelsPerMeter;
	bmiDest.bmiHeader.biClrUsed = 2;
	bmiDest.bmiHeader.biClrImportant = 2;
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[0], RGB(255,255,255));
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[1], RGB(0,0,0));

	// Allocate the bits.
	int nDestLineSize = DIBStorageWidth (bmiDest.bmiHeader.biWidth, 1);
	UINT nBitsAllocSize = nDestLineSize * bmiDest.bmiHeader.biHeight;
	PBYTE pbDestBits = (PBYTE)malloc (nBitsAllocSize);
	if (pbDestBits == NULL) {
		return FALSE;
	}

	ZeroMemory (pbDestBits, nBitsAllocSize);

	UINT nSrcLineSize = StorageWidth ();
	PBYTE pbSrc = (PBYTE)GetBitsAddress ();
	PBYTE pbDest = pbDestBits;
	int nSrcDepth = m_pBMI->bmiHeader.biBitCount;

	if (bReverseOnly)
	{
		for (UINT i = 0; i < nBitsAllocSize; i++)
		{
			*(pbDest++) = 255 - *(pbSrc++);
		}
	}
	else
	{
		// Loop through each scan line. This will be very slow, because 
		// we absolutely have to retrieve the actual color value for each
		// pixel - we can't make any assumptions about which color indices are
		// white and which are black. One thing we can do, however, is 
		// map all non-zero COLORREFs to white, so we don't have to do really
		// boring things like converting 16 bit color values to COLORREF.
	
		int x, y;
		COLORREF clrref = RGB(0,0,0);
		BOOL bOdd;
		for (y = bmiDest.bmiHeader.biHeight; y > 0; y--) {
			x = bmiDest.bmiHeader.biWidth - 1;
			bOdd = (x % 2) == 1;
			while (x >= 0) {
				switch (nSrcDepth) {
					case 4:
					{
						UINT index = bOdd ? (pbSrc[x / 2] & 0x0f) : (pbSrc[x / 2] >> 4);
						clrref = GET_COLORREF_FROM_RGBQUAD(&m_pBMI->bmiColors[index]);
						break;
					}
					case 8:
						clrref = GET_COLORREF_FROM_RGBQUAD(&m_pBMI->bmiColors[pbSrc[x / 2]]);
						break;
					case 16:
						clrref = (COLORREF)*(WORD *)(pbSrc + 2 * x);
						break;
					case 24:
						clrref = RGB(pbSrc[3 * x + 2], pbSrc[3 * x + 1], pbSrc[3 * x]);
						break;
					case 32:
						// Order really doesn't matter here, because we are only interested
						// in whether the color is 0 or something else.
						clrref = ((COLORREF *)pbSrc)[x];
						break;
				}
	
				// Turn on the bit if the color is black
				if (clrref == RGB(0,0,0)) {
					pbDest[x / 8] |= 1 << (7 - x % 8);
				}
	
				x--;
			}
			pbSrc += nSrcLineSize;
			pbDest += nDestLineSize;
		}
	}

	// Replace self with the monochrome DIB by calling Create.

	if (!Create ((BITMAPINFO *)&bmiDest, pbDestBits)) {
		free (pbDestBits);
		return FALSE;
	}

	return TRUE;
}

// ============================================================================
// CAvatarFileDIBImage implementation

// Write an image to a stream.

BOOL 
CAvatarFileDIBImage::Write(
CAvatarStream * pStream)
{
	ASSERT(m_pImage->m_pDib != NULL);
	m_pImage->m_dwStreamOffset = pStream->GetPosition ();
	return m_pImage->m_pDib->Save (pStream);
}

// ============================================================================
// CAvatarFileZlibImage implementation

// Write an image to a stream.

BOOL 
CAvatarFileZlibImage::Write(
CAvatarStream * pStream)
{
	ASSERT(m_pImage->m_pDib != NULL);

	LPBITMAPINFO pbmi = m_pImage->m_pDib->GetBitmapInfoAddress ();

	m_pImage->m_dwStreamOffset = pStream->GetPosition ();

	// May have to write out a palette record.
	if (m_pImage->m_byPaletteType == AIP_LOCALPALETTE) {
		int iColors = NumDIBColorEntries ((LPBITMAPINFO)pbmi);
		CAvatarPalette pal;
		if (!pal.SetFrom ((const RGBQUAD *)(((LPBYTE)pbmi) + pbmi->bmiHeader.biSize), iColors)) {
			return FALSE;
		}
		if (!pal.WriteAsRecord (pStream)) {
			return FALSE;
		}
	}

	// Write out the BITMAPINFOHEADER.

	if (pStream->Write (&pbmi->bmiHeader, pbmi->bmiHeader.biSize) != pbmi->bmiHeader.biSize) {
		return FALSE;
	}

	// Write out the bits.

	UINT nWriteSize = m_pImage->m_pDib->StorageWidth () * m_pImage->m_pDib->DibHeight ();
	return pStream->WriteCompressedBuffer (m_pImage->m_pDib->GetBitsAddress (), nWriteSize);
}

// ============================================================================
// CAvatarX implementation

BOOL
CAvatarX::Save(
CAvatarStream * pStream)
{
	ASSERT (pStream != NULL);

	if (!pStream->Open ()) {
		return FALSE;
	}

	CAvatarFileResourceResolver frr (pStream);
	DWORD dwVarSection;

	TRY
	{
		// Write all the common stuff first.

		// Write the header.
		AVATARHEADER avh;
		avh.nMagicNum = AF_MAGICNUM_NEW;
		avh.nType     = GetAvatarType ();
		avh.nVersion  = AVATAR_CURRENT_VERSION;
		if (pStream->Write (&avh, sizeof(avh)) != sizeof(avh)) {
			TRACE("Could not write header.");
			::AfxThrowUserException ();
		}

		// Write the avatar name.
		LPCTSTR pszName = (m_name != NULL) ? m_name : "";
		if (!pStream->Write16 (AK_NAME) ||
				!pStream->WriteString (pszName)) {
			TRACE("Could not write name.");
			::AfxThrowUserException ();
		}

		// Write the style.
		if (!pStream->Write16 (AK_STYLE) ||
				!pStream->Write16 (m_style)) {
			TRACE("Could not write style.");
			::AfxThrowUserException ();
		}

		// Write the avatar flags.
		if (!pStream->Write16 (AK_FLAGS) ||
				!pStream->Write16 (m_flags)) {
			TRACE("Could not write flags.");
			::AfxThrowUserException ();
		}

		// Write the following, but only if they exist:
		//			Copyright message
		//			Original URL
		//			Override URL
		//			Usage Flags

		if (m_pszCopyright != NULL && *m_pszCopyright != '\0') {
			if (!pStream->Write16 (AK_COPYRIGHT) ||
					!pStream->BeginVariableSection (&dwVarSection) ||
					!pStream->WriteString (m_pszCopyright) ||
					!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write copyright.");
				::AfxThrowUserException ();
			}
		}

		if (m_pszOriginalURL != NULL && *m_pszOriginalURL != '\0') {
			if (!pStream->Write16 (AK_ORIGINAL_URL) ||
					!pStream->BeginVariableSection (&dwVarSection) ||
					!pStream->WriteString (m_pszOriginalURL) ||
					!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write original URL.");
				::AfxThrowUserException ();
			}
		}

		if (m_pszNewURL != NULL && *m_pszNewURL != '\0') {
			if (!pStream->Write16 (AK_OVERRIDE_URL) ||
					!pStream->BeginVariableSection (&dwVarSection) ||
					!pStream->WriteString (m_pszNewURL) ||
					!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write original URL.");
				::AfxThrowUserException ();
			}
		}

		if (m_byUsageFlags != 0) {
			if (!pStream->Write16 (AK_USAGE_FLAGS) ||
					!pStream->BeginVariableSection (&dwVarSection) ||
					!pStream->Write8 (m_byUsageFlags) ||
					!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write usage flags.");
				::AfxThrowUserException ();
			}
		}

		// Write a global palette only if there is one.
		if (m_palette.m_nColorCount > 0)
		{	
			if (!m_palette.WriteAsRecord (pStream)) {
				TRACE("Could not write global palette.");
				::AfxThrowUserException ();
			}
		}

		// Write the icon in the new format.
		if (m_icon != INVALID_POSE_ID) {
			
			// Make sure the icon is loaded.
			CPose * pPose = GetPoseFromID (m_icon);
			if (!pPose->Load (m_pStream, &m_palette)) {
				TRACE("Could not load icon during saving.");
				::AfxThrowUserException ();
			}

			if (!pStream->Write16 (AK_ICON_NEW) || 
					!pStream->BeginVariableSection (&dwVarSection)) {
				TRACE("Could not write icon.");
				::AfxThrowUserException ();
			}

			// Icons only have an image, which is the first image in the pose.

			if (!frr.WriteResourceReference (&pPose->m_pdibs[0]) ||
					!pStream->Write8 (pPose->m_byFormats[0]) ||
					!pStream->Write8 (pPose->m_byPaletteTypes[0]))
			{
				TRACE("Could not write icon.");
				::AfxThrowUserException ();
			}
			
			if (!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write icon.");
				::AfxThrowUserException ();
			}
		}

		// Now write things that are specific to each type.

		if (!SaveSpecifics (pStream, frr)) {
			::AfxThrowUserException ();
		}

		// Start writing the data section.

		if (!pStream->Write16 (AK_STARTDATA)) {
			::AfxThrowUserException ();
		}

		// Write out each pose.
		int nPoses = m_arrPoses.GetSize ();
		for (int iPose = 0; iPose < nPoses; iPose++) {
			CPose * pPose = m_arrPoses[iPose];
			if (!pPose->Save (pStream, frr))
			{
				TRACE("Could not write graphics data.");
				::AfxThrowUserException ();
			}
		}

		if (!pStream->Write16 (AK_ENDDATA)) {
			::AfxThrowUserException ();
		}

		// Fix up all resource references.

		if (!frr.FixupReferences ()) {
			::AfxThrowUserException ();
		}
	}
	CATCH_ALL(e)
	{
		pStream->Close ();
		return FALSE;
	}
	END_CATCH_ALL
	pStream->Close ();
	return TRUE;
}

// ============================================================================
// CAvatarSimple implementation

// Write out records specific to a simple avatar.

BOOL 
CAvatarSimple::SaveSpecifics(
CAvatarStream * pStream, 
CAvatarFileResourceResolver& frr)
{
	if (!pStream->Write16 (AK_NBODIES2)) {
		return FALSE;
	}

	if (!pStream->Write16 (m_nBodies)) {
		return FALSE;
	}

	// Write out each body.
	int i;
	for (i = 0; i < m_nBodies; i++) {
		// Make sure the pose is loaded.
		CPose * pPose = GetPoseFromID (bRec[i].poseID);
		if (pPose == NULL || !pPose->Load (m_pStream, &m_palette)) {
			return FALSE;
		}
		
		if (!frr.WriteResourceReference (&pPose->m_pdibs[0]) ||
				!frr.WriteResourceReference (&pPose->m_pdibs[1]) ||
				!frr.WriteResourceReference (&pPose->m_pdibs[2]) ||
				!pStream->Write16 (bRec[i].emotion) ||
				!pStream->Write8 ((INT8)(bRec[i].intensity * 255.0f)) ||
				!pStream->Write16 (bRec[i].faceX) ||
				!pStream->Write16 (bRec[i].faceY) ||
				pStream->Write (pPose->m_byFormats, sizeof(pPose->m_byFormats)) != sizeof(pPose->m_byFormats) ||
				pStream->Write (pPose->m_byPaletteTypes, sizeof(pPose->m_byPaletteTypes)) != sizeof(pPose->m_byPaletteTypes))
		{
			return FALSE;
		}
	}

	return TRUE;
}


// ============================================================================
// CAvatarComplex implementation

// Write out records specific to a complex avatar.

BOOL 
CAvatarComplex::SaveSpecifics(
CAvatarStream * pStream, 
CAvatarFileResourceResolver& frr)
{
	// Write out faces.

	if (!pStream->Write16 (AK_NFACES2)) {
		return FALSE;
	}

	if (!pStream->Write16 (nFaces)) {
		return FALSE;
	}

	// Write out each face.
	int i;
	for (i = 0; i < nFaces; i++) {
		// Make sure the pose is loaded.
		CPose * pPose = GetPoseFromID (fRec[i].poseID);
		if (pPose == NULL || !pPose->Load (m_pStream, &m_palette)) {
			return FALSE;
		}
		
		if (!frr.WriteResourceReference (&pPose->m_pdibs[0]) ||
				!frr.WriteResourceReference (&pPose->m_pdibs[1]) ||
				!frr.WriteResourceReference (&pPose->m_pdibs[2]) ||
				!pStream->Write16 (fRec[i].emotion) ||
				!pStream->Write8 ((INT8)(fRec[i].intensity * 255.0f)) ||
				!pStream->Write16 (fRec[i].xCX) ||
				!pStream->Write16 (fRec[i].yCX) ||
				!pStream->Write16 (fRec[i].delta_xCX) ||
				!pStream->Write16 (fRec[i].delta_yCX) ||
				!pStream->Write16 (fRec[i].faceX) ||
				!pStream->Write16 (fRec[i].faceY) ||
				pStream->Write (pPose->m_byFormats, sizeof(pPose->m_byFormats)) != sizeof(pPose->m_byFormats) ||
				pStream->Write (pPose->m_byPaletteTypes, sizeof(pPose->m_byPaletteTypes)) != sizeof(pPose->m_byPaletteTypes))
		{
			return FALSE;
		}
	}

	// Write out torsos.

	if (!pStream->Write16 (AK_NTORSOS2)) {
		return FALSE;
	}

	if (!pStream->Write16 (nTorsos)) {
		return FALSE;
	}

	// Write out each torso.

	for (i = 0; i < nTorsos; i++) {
		// Make sure the pose is loaded.
		CPose * pPose = GetPoseFromID (bRec[i].poseID);
		if (pPose == NULL || !pPose->Load (m_pStream, &m_palette)) {
			return FALSE;
		}
		
		if (!frr.WriteResourceReference (&pPose->m_pdibs[0]) ||
				!frr.WriteResourceReference (&pPose->m_pdibs[1]) ||
				!frr.WriteResourceReference (&pPose->m_pdibs[2]) ||
				!pStream->Write16 (bRec[i].emotion) ||
				!pStream->Write8 ((INT8)(bRec[i].intensity * 255.0f)) ||
				!pStream->Write16 (bRec[i].xCX) ||
				!pStream->Write16 (bRec[i].yCX) ||
				pStream->Write (pPose->m_byFormats, sizeof(pPose->m_byFormats)) != sizeof(pPose->m_byFormats) ||
				pStream->Write (pPose->m_byPaletteTypes, sizeof(pPose->m_byPaletteTypes)) != sizeof(pPose->m_byPaletteTypes))
		{
			return FALSE;
		}
	}

	return TRUE;
}

// ============================================================================
// CPose implementation

// Save a single pose, including all three masks. Saves in the formats specified
// in the member variables byFormats and byPaletteTypes, doing conversion as necessary. 

BOOL
CPose::Save(
CAvatarStream * pStream,
CAvatarFileResourceResolver& frr)
{
	if (m_pdibs[0] == NULL) {
		return FALSE;
	}

	if (!pStream->Open ()) {
		return FALSE;
	}

	// If the masks are not monochrome, let's force them to be converted right now!
	for (int iDib = 1; iDib < 3; iDib++) {
		if (m_pdibs[iDib] != NULL) {
			if (!m_pdibs[iDib]->ForceToMonochrome ()) {
				return FALSE;
			}
		}					 
	}


	// Three storage scenarios are possible:
	//     AIP_MASKEDMONO bitmap containing all three images
	//     Main image, plus AIP_DUALMASK containing both masks
	//     All three images separately

	// If the save format is Masked Mono, but there are no masks, then just save
	// as a monochrome image.
    if (m_byPaletteTypes[0] == AIP_MASKEDMONO && m_pdibs[1] == NULL && m_pdibs[2] == NULL)
	{
		m_byPaletteTypes[0] = AIP_MONOCHROME;
	}

    if (m_byPaletteTypes[0] == AIP_MASKEDMONO) {
		// Need to combine all three images into one and save it.
		CAvatarDIB * pDIB = CombineThreeMonochromeImages (m_pdibs[0], m_pdibs[1], m_pdibs[2]);
		if (pDIB == NULL) {
			pStream->Close ();
			return FALSE;
		}
		// Save the temporary DIB, then delete it.
		if (!SaveDIB (pStream, pDIB, m_byFormats[0], m_byPaletteTypes[0], frr, &m_pdibs[0])) {
			delete pDIB;
			pStream->Close ();
			return FALSE;
		}
		delete pDIB;
	}
	else
	{
		// Save the image bitmap.
		if (!SaveDIB (pStream, m_pdibs[0], m_byFormats[0], m_byPaletteTypes[0], frr, &m_pdibs[0])) {
			pStream->Close ();
			return FALSE;
		}

		if (m_byPaletteTypes[1] == AIP_DUALMASK && m_pdibs[1] != NULL && m_pdibs[2] != NULL) {
			// Need to combine both mask bitmaps into one.
			CAvatarDIB * pDIB = CombineTwoMonochromeImages (m_pdibs[1], m_pdibs[2]);
			if (pDIB == NULL) {
				pStream->Close ();
				return FALSE;
			}
			// Save the temporary DIB, then delete it.
			if (!SaveDIB (pStream, pDIB, m_byFormats[1], m_byPaletteTypes[1], frr, &m_pdibs[1])) {
				delete pDIB;
				pStream->Close ();
				return FALSE;
			}
			delete pDIB;
		}
		else
		{
			// Save masks separately.
			for (int i = 1; i < 3; i++) {
				if (m_pdibs[i] != NULL) {
					if (!SaveDIB (pStream, m_pdibs[i], m_byFormats[i], m_byPaletteTypes[i], frr, &m_pdibs[i])) {
						pStream->Close ();
						return FALSE;
					}
				}
			}
		}
	}

	pStream->Close ();
	return TRUE;
}

// Save a single DIB from a pose.

BOOL 
CPose::SaveDIB(
CAvatarStream * pStream, 
CAvatarDIB * pDIB, 
BYTE byFormat, 
BYTE byPaletteType, 
CAvatarFileResourceResolver &frr,
PVOID pvMarkResource)
{
	AVATARIMAGE im;
	if (!frr.MarkResourcePosition (pvMarkResource))
	{
		return FALSE;
	}

	// Construct the right CAvatarFileImage and write through it.
	im.m_byFormat = byFormat;
	im.m_byPaletteType = byPaletteType;
	im.m_pDib = pDIB;
	BOOL bRet;
	switch (im.m_byFormat) {
		case AIF_DIB:
			bRet = CAvatarFileDIBImage(&im).Write (pStream);
			break;
		case AIF_LZDEFLATE:
			bRet = CAvatarFileZlibImage(&im).Write (pStream);
			break;
		default:
			bRet = FALSE;
			break;
	}
	return bRet;
}

// Lookup table that combines two 4-bit values into 8-bit value of pairs.
// eg. Start with abcd and mnop => end up with ambncodp
// To combine two bytes abcdefgh and mnopqrst into one word, form two pairs
// abcdmnop and efghqrst, use this lookup table to get ambncodp and eqfrgsht,
// and then put them back together with MAKEWORD.

static const BYTE byLookupCombine[] = {
	0, 1, 4, 5, 16, 17, 20, 21, 64, 65, 68, 69, 80, 81, 84, 85, 
	2, 3, 6, 7, 18, 19, 22, 23, 66, 67, 70, 71, 82, 83, 86, 87, 
	8, 9, 12, 13, 24, 25, 28, 29, 72, 73, 76, 77, 88, 89, 92, 93, 
	10, 11, 14, 15, 26, 27, 30, 31, 74, 75, 78, 79, 90, 91, 94, 95, 
	32, 33, 36, 37, 48, 49, 52, 53, 96, 97, 100, 101, 112, 113, 116, 117, 
	34, 35, 38, 39, 50, 51, 54, 55, 98, 99, 102, 103, 114, 115, 118, 119, 
	40, 41, 44, 45, 56, 57, 60, 61, 104, 105, 108, 109, 120, 121, 124, 125, 
	42, 43, 46, 47, 58, 59, 62, 63, 106, 107, 110, 111, 122, 123, 126, 127, 
	128, 129, 132, 133, 144, 145, 148, 149, 192, 193, 196, 197, 208, 209, 212, 213, 
	130, 131, 134, 135, 146, 147, 150, 151, 194, 195, 198, 199, 210, 211, 214, 215, 
	136, 137, 140, 141, 152, 153, 156, 157, 200, 201, 204, 205, 216, 217, 220, 221, 
	138, 139, 142, 143, 154, 155, 158, 159, 202, 203, 206, 207, 218, 219, 222, 223, 
	160, 161, 164, 165, 176, 177, 180, 181, 224, 225, 228, 229, 240, 241, 244, 245, 
	162, 163, 166, 167, 178, 179, 182, 183, 226, 227, 230, 231, 242, 243, 246, 247, 
	168, 169, 172, 173, 184, 185, 188, 189, 232, 233, 236, 237, 248, 249, 252, 253, 
	170, 171, 174, 175, 186, 187, 190, 191, 234, 235, 238, 239, 250, 251, 254, 255, 
};

// Combine three monochrome DIBs into one 2-bit DIB. If one of the three DIBs isn't around,
// it just saves 0 for it's bits.

CAvatarDIB * 
CPose::CombineThreeMonochromeImages(
CAvatarDIB * pDIB1, 
CAvatarDIB * pDIB2,
CAvatarDIB * pDIB3)
{
	CAvatarDIB * pDIBRet = NULL;

	ASSERT (pDIB1 != NULL || pDIB2 != NULL || pDIB3 != NULL);
	CAvatarDIB * pDIBInfo = pDIB1 != NULL ? pDIB1 : pDIB2;

	// Check if all DIBs passed in are indeed monochrome.
	if ((pDIB1 != NULL && pDIB1->GetBitmapInfoAddress ()->bmiHeader.biBitCount != 1) ||
			(pDIB2 != NULL && pDIB2->GetBitmapInfoAddress ()->bmiHeader.biBitCount != 1) ||
			(pDIB3 != NULL && pDIB3->GetBitmapInfoAddress ()->bmiHeader.biBitCount != 1))
	{
		return NULL;
	}

	// Special case check - it is possible that the DIBs have
	// their pixels inverted.
	BOOL bInvertImage[3] = { FALSE, FALSE, FALSE };
	CAvatarDIB* pDIB[3] = { pDIB1, pDIB2, pDIB3 };
	for (int iImage = 0; iImage < 3; iImage++) {
		if (pDIB[iImage] != NULL) {
			RGBQUAD* pColors = pDIB[iImage]->GetClrTabAddress ();
			if (GET_COLORREF_FROM_RGBQUAD(pColors) == RGB(0, 0, 0)) {
				bInvertImage[iImage] = TRUE;
			}
		}
	}

	LPBITMAPINFOHEADER pbmihSrc = &pDIBInfo->GetBitmapInfoAddress ()->bmiHeader;
	PBYTE pbDestBits;
	PBYTE pbSrcBits[3];
	pbSrcBits[0] = pDIB1 != NULL ? (PBYTE)pDIB1->GetBitsAddress () : NULL;
	pbSrcBits[1] = pDIB2 != NULL ? (PBYTE)pDIB2->GetBitsAddress () : NULL;
	pbSrcBits[2] = pDIB3 != NULL ? (PBYTE)pDIB3->GetBitsAddress () : NULL;

	TWOBITBITMAPINFO bmiDest;

	// Set up the BITMAPINFO.
	bmiDest.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiDest.bmiHeader.biWidth = pbmihSrc->biWidth;
	bmiDest.bmiHeader.biHeight = pbmihSrc->biHeight;
	bmiDest.bmiHeader.biPlanes = 1;
	bmiDest.bmiHeader.biBitCount = 2;
	bmiDest.bmiHeader.biCompression = BI_RGB;
	bmiDest.bmiHeader.biSizeImage = 0;
	bmiDest.bmiHeader.biXPelsPerMeter = pbmihSrc->biXPelsPerMeter;
	bmiDest.bmiHeader.biYPelsPerMeter = pbmihSrc->biYPelsPerMeter;
	bmiDest.bmiHeader.biClrUsed = 4;
	bmiDest.bmiHeader.biClrImportant = 4;
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[0], RGB(255,255,255));
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[1], RGB(0,0,0));
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[2], RGB(255,0,0));
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[3], RGB(255,0,0));

	UINT nBitsAllocSize = DIBStorageWidth (bmiDest.bmiHeader.biWidth, 2) * 
		bmiDest.bmiHeader.biHeight;
	pbDestBits = (PBYTE)malloc (nBitsAllocSize);
	ZeroMemory (pbDestBits, nBitsAllocSize);
	if (pbDestBits == NULL) {
		return NULL;
	}

	TRY
	{
		pDIBRet = new CAvatarDIB;
		if (!pDIBRet->Create ((LPBITMAPINFO)&bmiDest, pbDestBits)) {
			AfxThrowMemoryException ();
		}
	}
	CATCH_ALL(e)
	{
		free (pbDestBits);
		if (pDIBRet!= NULL) {
			delete pDIBRet;
		}
		return NULL;
	}
	END_CATCH_ALL

	// In the second pass, write out the bits. Use a lookup table for speed.

	int nWordsPerDestScanLine = pDIBRet->StorageWidth () / 2;
	int nSrcScanLineSize = pDIBInfo->StorageWidth ();
	LPWORD pwDest = (LPWORD)pbDestBits;
	int nSrcOffset = 0;
	BYTE byBits[3];
	BYTE byDestBits[2];
	BYTE byLookupIndex[2];
	for (int y = 0; y < bmiDest.bmiHeader.biHeight; y++) {
		for (int x = 0; x < nWordsPerDestScanLine; x++) {
			// Get the three bit values. This function is typically used with bitmap 0
			// being the image, bitmap 1 being the mask, and bitmap 2 being the aura.
			// So, if there is no mask, use the same mask as the aura.
			if (pbSrcBits[0] != NULL) {
				byBits[0] = bInvertImage[0] ? ~((pbSrcBits[0])[nSrcOffset + x]) : (pbSrcBits[0])[nSrcOffset + x];
			}
			else {
				byBits[0] = 0;
			}
			if (pbSrcBits[2] != NULL) {
				byBits[2] = bInvertImage[2] ? ~((pbSrcBits[2])[nSrcOffset + x]) : (pbSrcBits[2])[nSrcOffset + x];
			}
			else {
				byBits[2] = 0;
			}
			if (pbSrcBits[1] != NULL) {
				byBits[1] = bInvertImage[1] ? ~((pbSrcBits[1])[nSrcOffset + x]) : (pbSrcBits[1])[nSrcOffset + x];
			}
			else {
				byBits[1] = byBits[2];
			}

			// The 3 bit to 2 bit compression uses the fact that we can never have
			// bit 1 = 1, bit 2 = 0 (bitmap 1 is wholly contained within bitmap 2),
			// and that we don't care what bit 0 is when bit 1 = 0.
			// So, the compression for the source values is:
			// 00x => 00, 010 => 10, 011 => 11, 100 => 01.
			// So, the two destination bits are defined this way:
			// Dest. Bit 0 = (Src. Bit 1 == 1) ? Src. Bit 0 : Src. Bit 2.
			// Dest. Bit 1 = Src. Bit 1.
			// This is done with one single logical op.

			byDestBits[0] = (byBits[0] & byBits[1]) | (~byBits[1] & byBits[2]);
			byDestBits[1] = byBits[1];

			// Combine two bytes into a word. For description, see byLookupCombine above.
			byLookupIndex[0] = ((byDestBits[0] & 0xf0) >> 4) | (byDestBits[1] & 0xf0);
			byLookupIndex[1] = (byDestBits[0] & 0x0f) | ((byDestBits[1] & 0x0f) << 4);
			pwDest[x] = MAKEWORD (byLookupCombine[byLookupIndex[0]], byLookupCombine[byLookupIndex[1]]);
		}
		nSrcOffset += nSrcScanLineSize;
		pwDest += nWordsPerDestScanLine;
	}

	return pDIBRet;
}

// Combine two monochrome DIBs into one 2-bit DIB. If one of the two DIBs isn't around,
// it just saves 0 for it's bits.

CAvatarDIB * 
CPose::CombineTwoMonochromeImages(
CAvatarDIB * pDIB1, 
CAvatarDIB * pDIB2)
{
	CAvatarDIB * pDIBRet = NULL;

	ASSERT (pDIB1 != NULL || pDIB2 != NULL);
	CAvatarDIB * pDIBInfo = pDIB1 != NULL ? pDIB1 : pDIB2;

	if ((pDIB1 != NULL && pDIB1->GetBitmapInfoAddress ()->bmiHeader.biBitCount != 1) ||
			(pDIB1 != NULL && pDIB1->GetBitmapInfoAddress ()->bmiHeader.biBitCount != 1))
	{
		return NULL;
	}

	// Check if all DIBs passed in are indeed monochrome.
	LPBITMAPINFOHEADER pbmihSrc = &pDIBInfo->GetBitmapInfoAddress ()->bmiHeader;
	PBYTE pbDestBits;
	PBYTE pbSrcBits[2];
	pbSrcBits[0] = pDIB1 != NULL ? (PBYTE)pDIB1->GetBitsAddress () : NULL;
	pbSrcBits[1] = pDIB2 != NULL ? (PBYTE)pDIB2->GetBitsAddress () : NULL;

	TWOBITBITMAPINFO bmiDest;

	// Set up the BITMAPINFO.
	bmiDest.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiDest.bmiHeader.biWidth = pbmihSrc->biWidth;
	bmiDest.bmiHeader.biHeight = pbmihSrc->biHeight;
	bmiDest.bmiHeader.biPlanes = 1;
	bmiDest.bmiHeader.biBitCount = 2;
	bmiDest.bmiHeader.biCompression = BI_RGB;
	bmiDest.bmiHeader.biSizeImage = 0;
	bmiDest.bmiHeader.biXPelsPerMeter = pbmihSrc->biXPelsPerMeter;
	bmiDest.bmiHeader.biYPelsPerMeter = pbmihSrc->biYPelsPerMeter;
	bmiDest.bmiHeader.biClrUsed = 4;
	bmiDest.bmiHeader.biClrImportant = 4;
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[0], RGB(255,255,255));
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[1], RGB(0,0,0));
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[2], RGB(255,0,0));
	SET_RGBQUAD_FROM_COLORREF(&bmiDest.bmiColors[3], RGB(255,0,0));

	UINT nBitsAllocSize = DIBStorageWidth (bmiDest.bmiHeader.biWidth, 2) * 
				bmiDest.bmiHeader.biHeight;
	pbDestBits = (PBYTE)malloc (nBitsAllocSize);
	ZeroMemory (pbDestBits, nBitsAllocSize);
	if (pbDestBits == NULL) {
		return NULL;
	}

	TRY
	{
		pDIBRet = new CAvatarDIB;
		if (!pDIBRet->Create ((LPBITMAPINFO)&bmiDest, pbDestBits)) {
			AfxThrowMemoryException ();
		}
	}
	CATCH_ALL(e)
	{
		free (pbDestBits);
		if (pDIBRet!= NULL) {
			delete pDIBRet;
		}
		return NULL;
	}
	END_CATCH_ALL

	// In the second pass, write out the bits. Use a lookup table for speed.

	int nWordsPerDestScanLine = pDIBRet->StorageWidth () / 2;
	int nSrcScanLineSize = pDIBInfo->StorageWidth ();
	LPWORD pwDest = (LPWORD)pbDestBits;
	int nSrcOffset = 0;
	BYTE byBits[2];
	BYTE byLookupIndex[2];
	for (int y = 0; y < bmiDest.bmiHeader.biHeight; y++) {
		for (int x = 0; x < nWordsPerDestScanLine; x++) {
			byBits[0] = ((pbSrcBits[0] != NULL) ? (pbSrcBits[0])[nSrcOffset + x] : 0);
		    byBits[1] = ((pbSrcBits[1] != NULL) ? (pbSrcBits[1])[nSrcOffset + x] : 0);
			// Combine two bytes into a word. For description, see byLookupCombine above.
			byLookupIndex[0] = ((byBits[0] & 0xf0) >> 4) | (byBits[1] & 0xf0);
			byLookupIndex[1] = (byBits[0] & 0x0f) | ((byBits[1] & 0x0f) << 4);
			pwDest[x] = MAKEWORD (byLookupCombine[byLookupIndex[0]], byLookupCombine[byLookupIndex[1]]);
		}
		nSrcOffset += nSrcScanLineSize;
		pwDest += nWordsPerDestScanLine;
	}

	return pDIBRet;
}

// ============================================================================
// CAvatarFileResourceResolver implementation

// Writes a reference to a resource. Adds a map entry so we know where to come
// back to write the reference.

BOOL 
CAvatarFileResourceResolver::WriteResourceReference(
PVOID pvResource)
{
	if (!m_pStream->Write32 (0)) {
		return FALSE;
	}

	// If the reference is to a NULL object, it never adds it to the map,
	// and the reference is ultimately written out as 0. This allows adding
	// "null" type objects without any special code.

	if (pvResource != NULL) {
		long lPosition = m_pStream->GetPosition () - sizeof(INT32);
		if (lPosition == AVSTREAM_ERROR) {
			return FALSE;
		}
		m_mapResourceReferenceToPtr.SetAt ((PVOID)lPosition, pvResource);
	}
	return TRUE;
}

// Marks the current position in the file as being the location of the resource.
// The caller should call this immediately before writing out the resource.

BOOL 
CAvatarFileResourceResolver::MarkResourcePosition(
PVOID pvResource)
{
	long lPosition = m_pStream->GetPosition ();
	if (lPosition == AVSTREAM_ERROR) {
		return FALSE;
	}
	m_mapPtrToResourceOffset.SetAt (pvResource, (PVOID)lPosition);
	return TRUE;
}

// Goes back and fixes up all the offset references written with WriteResourceReference.
// If a referenced resource was never written, leaves the offset as 0.

BOOL 
CAvatarFileResourceResolver::FixupReferences()
{
	POSITION pos = m_mapResourceReferenceToPtr.GetStartPosition ();
	long lReference;
	PVOID pvResource;
	long lOffset;
	while (pos) {
		m_mapResourceReferenceToPtr.GetNextAssoc (pos, (PVOID&)lReference, pvResource);
		if (m_mapPtrToResourceOffset.Lookup (pvResource, (PVOID&)lOffset)) {
			if (!m_pStream->SetPosition (lReference, SEEK_SET)) {
				return FALSE;
			}
			if (!m_pStream->Write32 ((INT32)lOffset)) {
				return FALSE;
			}
		}
	}
	return TRUE;
}

#endif // AVATAR_WRITE

// ============================================================================
// CChatBackdrop implementation

#if defined(AVATAR_WRITE)

// Saves the backdrop to an Avatar format file.

BOOL
CChatBackdrop::Save(
CAvatarStream* pStream)
{
	ASSERT (m_pDIB != NULL);

	if (!pStream->Open ()) {
		return FALSE;
	}

	CAvatarFileResourceResolver frr (pStream);

	TRY
	{
		// Write the header.
		AVATARHEADER avh;
		avh.nMagicNum = AF_MAGICNUM_NEW;
		avh.nType     = AT_BACKDROP;
		avh.nVersion  = AVATAR_CURRENT_VERSION;
		if (pStream->Write (&avh, sizeof(avh)) != sizeof(avh)) {
			TRACE("Could not write header.");
			::AfxThrowUserException ();
		}
	
		DWORD dwVarSection;

		// Write the following, but only if they exist:
		//			Copyright message
		//			Original URL
		//			Override URL
		//			Usage Flags

		if (m_pszCopyright != NULL && *m_pszCopyright != '\0') {
			if (!pStream->Write16 (AK_COPYRIGHT) ||
					!pStream->BeginVariableSection (&dwVarSection) ||
					!pStream->WriteString (m_pszCopyright) ||
					!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write copyright.");
				::AfxThrowUserException ();
			}
		}

		if (m_pszOrigURL != NULL && *m_pszOrigURL != '\0') {
			if (!pStream->Write16 (AK_ORIGINAL_URL) ||
					!pStream->BeginVariableSection (&dwVarSection) ||
					!pStream->WriteString (m_pszOrigURL) ||
					!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write original URL.");
				::AfxThrowUserException ();
			}
		}

		if (m_pszNewURL != NULL && *m_pszNewURL != '\0') {
			if (!pStream->Write16 (AK_OVERRIDE_URL) ||
					!pStream->BeginVariableSection (&dwVarSection) ||
					!pStream->WriteString (m_pszNewURL) ||
					!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write original URL.");
				::AfxThrowUserException ();
			}
		}

		if (m_byUsageFlags != 0) {
			if (!pStream->Write16 (AK_USAGE_FLAGS) ||
					!pStream->BeginVariableSection (&dwVarSection) ||
					!pStream->Write8 (m_byUsageFlags) ||
					!pStream->EndVariableSection (dwVarSection)) {
				TRACE("Could not write usage flags.");
				::AfxThrowUserException ();
			}
		}

		// Write out the backdrop record. This is a record with a size field,
		// then the image offset, an image format, and a palette type. 
		// We always use the LZDEFLATE compression to write the thing.

		if (!pStream->Write16 (AK_BACKDROP) ||
				!pStream->BeginVariableSection (&dwVarSection) ||
				!frr.WriteResourceReference (&m_pDIB) ||
				!pStream->Write8 (AIF_LZDEFLATE) ||
				!pStream->Write8 (AIP_LOCALPALETTE) ||
				!pStream->EndVariableSection (dwVarSection)) {
			::AfxThrowUserException ();
		}

		// Write the data section. This consists of the startdata and enddata
		// markers flanking the actual backdrop image.

		if (!pStream->Write16 (AK_STARTDATA)) {
			::AfxThrowUserException ();
		}

		if (!CPose::SaveDIB (pStream, m_pDIB, AIF_LZDEFLATE, AIP_LOCALPALETTE, frr, &m_pDIB)) {
			::AfxThrowUserException ();
		}

		if (!pStream->Write16 (AK_ENDDATA)) {
			::AfxThrowUserException ();
		}

		// Fix up resource references.

		if (!frr.FixupReferences ()) {
			::AfxThrowUserException ();
		}
	}	
	CATCH_ALL(e)
	{
		pStream->Close ();
		return FALSE;
	}
	END_CATCH_ALL

	return TRUE;
}

#endif // AVATAR_WRITE

// This code is of use to non-client programs, like converters.

#if defined(AVATAR_NOT_CLIENT)

// ============================================================================
// CAvatarX implementation

// Loads all images in the Avatar. Used for things like conversion.

BOOL
CAvatarX::LoadAllImages()
{
	ASSERT (m_pStream != NULL);
	
	if (!m_pStream->Open ()) {
		return FALSE;
	}

	BOOL bRet = TRUE;

	int i;
	int nMax = m_arrPoses.GetSize ();
	for (i = 0; bRet && i < nMax; i++) {
		bRet = m_arrPoses[i]->Load (m_pStream, &m_palette);
	}

	m_pStream->Close ();
	return bRet;
}

#endif // AVATAR_NOT_CLIENT

