#include "stdafx.h"
#include "avbfile.h"
#include "avatarx.h"
	
void main( 
int argc, 
char *argv[ ], 
char *envp[ ] )
{
	if (argc == 4 && !lstrcmpi (argv[1], "bg")) {
		BOOL bRet = FALSE;
		printf ("Backdrop %s:", argv[2]);
		CAvatarFileStream stream (argv[2]);
		CChatBackdrop * pBackdrop = CChatBackdrop::LoadBackdrop (&stream);
		if (pBackdrop) {

			char szInput[1024];

			// Prompt for additional information.

			if (pBackdrop->m_filetype == 0x4d42 /* 'BM' */) {
				printf ("\n\nIf you wish to allow Microsoft Chat users to be able to automatically\n");
				printf ("download this background when needed, you need to upload the file\n");
				printf ("to a web or FTP site.\n\n");
				printf ("Enter the full URL (including the filename) of the location where\n");
				printf ("you will make the file available (or just press Enter if you do not\n");
				printf ("want to use this feature: ");
				gets (szInput);
				if (szInput[0] != '\0') {
					pBackdrop->m_pszOrigURL = strdup (szInput);
				}

				printf ("\n");
				printf ("You can specify whether other users who get your background\n");
				printf ("can publish it themselves on their own web site, and allow\n");
				printf ("other users to automatically download it from there.\n\n");
				printf ("Enter \'n\' to prevent them from doing so, or anything else\n");
				printf ("to allow them to do so: ");
				gets (szInput);
				if (!lstrcmpi (szInput, "n") || !lstrcmpi (szInput, "no")) {
					pBackdrop->m_byUsageFlags |= 1;
				}

				printf ("\n");
				printf ("You may want to have a copyright message appear when a user selects\n");
				printf ("your background in the \'Backgrounds\' page in Microsoft Chat.\n\n");
				printf ("Enter a copyright message, or just press Enter for no message:\n");
				printf ("Message: ");
				gets (szInput);
				if (szInput[0] != '\0') {
					pBackdrop->m_pszCopyright = strdup (szInput);
				}
			}

			CAvatarFileStream streamOut (argv[3], TRUE);
			bRet = pBackdrop->Save (&streamOut);
		}
		if (bRet) {
			printf ("Successfully written as %s\n", argv[3]);
		}
		else {
			printf ("Error in conversion!\n");
		}
	}
	else if (argc == 3) {
		BOOL bRet = FALSE;
		printf ("Character %s:", argv[1]);
		CAvatarFileStream stream (argv[1]);
		if (stream.Open ())
		{
			CAvatarX * pAvatar = CAvatarX::LoadAvatar (&stream);
			stream.Close ();
			if (pAvatar)
			{
				pAvatar->SetStream (&stream);
				if (pAvatar->LoadAllImages ())
				{
					// Set everything for optimized saving.
					int nCount = pAvatar->GetPoseCount ();
					for (int i = 0; i < nCount; i++) {
						CPose * pPose = pAvatar->GetPoseFromID (i + 1);
						pPose->m_byFormats[0] = AIF_LZDEFLATE;
						pPose->m_byFormats[1] = AIF_LZDEFLATE;
						pPose->m_byFormats[2] = AIF_LZDEFLATE;
						pPose->m_byPaletteTypes[0] = AIP_LOCALPALETTE;
						pPose->m_byPaletteTypes[1] = AIP_MONOCHROME;
						pPose->m_byPaletteTypes[2] = AIP_MONOCHROME;
						if (pPose->m_pdibs[0]->GetBitmapInfoAddress ()->bmiHeader.biBitCount == 1) {
							pPose->m_byPaletteTypes[0] = AIP_MASKEDMONO;
						}
						else if (pPose->m_pdibs[1]) {
							pPose->m_byPaletteTypes[1] = AIP_DUALMASK;
						}
					}
				}
				CAvatarFileStream streamOut (argv[2], TRUE);
				bRet = pAvatar->Save (&streamOut);
				delete pAvatar;
			}
		}
		if (bRet) {
			printf ("Successfully written as %s\n", argv[2]);
		}
		else {
			printf ("Error in conversion!\n");
		}
	}
	else
	{
		printf("avbcvt: Converts old Microsoft Chat characters and backgrounds\n");
		printf("        into the newer compressed format\n");
		printf("\n");
		printf("Usage:\n\n");
		printf("    avbcvt <old avb filename> <new avb filename>\n");
		printf("        Converts an old character file and writes it out to\n");
		printf("        a new file. Both filenames must include the .avb\n");
		printf("        extension.\n\n");
		printf("    avbcvt bg <old bmp filename> <new bgb filename>\n");
		printf("        Converts an old background (or any .bmp file) and\n");
		printf("        writes it out to a new file. The old filename must include\n");
		printf("        the .bmp extension, and the new filename must include the\n");
		printf("        the .bgb extension.\n\n");
		printf("        The program will prompt you to enter additional information\n");
		printf("        to store the file - you can choose to enter the info or\n");
		printf("        leave it blank\n\n");
		printf("Examples:\n\n");
		printf("    avbcvt Sam.avb new\\Sam.avb\n");
		printf("        Converts Sam.avb and places the new file in the new\n");
		printf("        subdirectory under the same name.\n\n");
		printf("    avbcvt bg meadow.bmp meadow.bgb\n");
		printf("        Converts meadow.bmp and saves the new file as meadow.bgb.\n");
	}
}


