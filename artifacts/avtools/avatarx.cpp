#include "stdafx.h"
#include "avbfile.h"
#include "avatarx.h"

CAvatarX::CAvatarX()
{
	m_name = NULL;
	m_style = 0;
	m_flags = 0;
	m_icon = 0;
	m_arrPoses.SetSize (0, 200);
	m_pStream = NULL;
	m_pszOriginalURL = NULL;
	m_pszNewURL = NULL;
	m_pszCopyright = NULL;
	m_byUsageFlags = 0;
}

CAvatarX::~CAvatarX()
{
	int i = m_arrPoses.GetSize (); 
	while (i > 0) {
		delete m_arrPoses[--i];
	}
	free (m_name);
	free (m_pszOriginalURL);
	free (m_pszNewURL);
	free (m_pszCopyright);
}

#if defined(AVATARFILER)

CPose::CPose(
LPCTSTR * ppszFiles)
{
	ASSERT (ppszFiles != NULL);
	for (int i = 0; i < 3; i++) {
		if (ppszFiles[i] != NULL) {
			m_strFiles[i] = ppszFiles[i];
			m_byFormats[i] = AIF_DIB;
			m_byPaletteTypes[i] = AIP_NOPALETTE;
		}
	}
	ZeroMemory (m_pdibs, sizeof(m_pdibs));
}

#else

CPose::CPose(
LPDWORD pdwOffsets, 
LPBYTE pbyFormats, 
LPBYTE pbyPaletteTypes)
{
	ASSERT (pdwOffsets != NULL);
	ASSERT (pbyFormats != NULL);
	ASSERT (pbyPaletteTypes != NULL);

	memcpy (m_dwOffsets, pdwOffsets, sizeof(m_dwOffsets));
	memcpy (m_byFormats, pbyFormats, sizeof(m_byFormats));
	memcpy (m_byPaletteTypes, pbyPaletteTypes, sizeof(m_byPaletteTypes));

	// Handling for special types of poses.
	if (m_byPaletteTypes[0] == AIP_MASKEDMONO) {
		m_dwOffsets[1] = m_dwOffsets[2] = 0;
	}
	else if (m_byPaletteTypes[1] == AIP_DUALMASK) {
		m_dwOffsets[2] = 0;
	}

	ZeroMemory (m_pdibs, sizeof(m_pdibs));
}
#endif

CPose::~CPose()
{
	for (int i = 0; i < 3; i++) {
		if (m_pdibs[i]) {
			delete m_pdibs[i];
		}
	}
}
