#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

// ======================================================================
// This file can be used with to build the following programs:
//
// ADDREC - adds or modifies certain records in an .AVB or .BGB file.
// Required build flags:
//			ADDREC
//
// CHANGEURL - changes the URL of an .AVB or .BGB file, if possible.
// Required build flags:
//			CHANGEURL

// Records which we recognize - their names and their codes.

enum RecordsSupported {
	recName = 0,
	recCopyright,
	recOrigURL,
	recNewURL,
	recUsageFlag,
};

LPCSTR pszRecordsSupported[] = {
	"name",
	"copyright",
	"origurl",
	"newurl",
	"usageflag",
};
  
WORD wRecordsSupported[] = {
	1,			/* AK_NAME */
	259,		/* AK_COPYRIGHT */
	260,		/* AK_ORIGINAL_URL */
	261,		/* AK_NEW_URL */
	262,		/* AK_USAGE_FLAGS */
};
#define WHICH_RECORD_IS_NEW_URL 3
#define WHICH_RECORD_IS_USAGE_FLAG 4

// Index of argvs for each record to be modified. A 0 here indicates that
// the record does not have to be modified. A -1 indicates that the record
// has been modified, and any duplicates should not be written out.

int nRecordsToBeWorkedOn[] = {
	0,
	0,
	0,
	0,
	0,
};

#define NUMRECS (sizeof(wRecordsSupported) / sizeof(wRecordsSupported[0]))

// Sizes of old style records. A 0 means it's a null terminated string. A negative value
// means a 2 byte count plus the given number of bytes times the count (the format of
// the face, torso, body records).

int nOldRecordSizes[] = {
	0,
	0,
	2,
	4,
	-43,
	-35,
	0,			// AK_STARTDATA - size doesn't matter.
	0,			// AK_ENDDATA - size doesn't matter.
	2,
	-35,
	-33,
	-25,
	-25,
};

int
WriteOutRecord(
int iRec,
LPCSTR pszArg,
FILE * file)
{
	int nArgLen;

	fwrite (wRecordsSupported + iRec, 1, sizeof(WORD), file);
	nArgLen = sizeof(WORD);

	// What we write out depends on the tag.
	if (iRec == recName) {
		fwrite (pszArg, 1, lstrlen (pszArg) + 1, file);
		nArgLen += lstrlen (pszArg) + 1;
	}
	else if (iRec == recUsageFlag) {
		BYTE byUsage = (BYTE)atoi (pszArg);
		WORD wRecSize = sizeof(byUsage);
		fwrite (&wRecSize, 1, sizeof(wRecSize), file);
		fwrite (&byUsage, 1, sizeof(byUsage), file);
		nArgLen += sizeof(wRecSize) + sizeof(byUsage);
	}
	else {
		WORD wRecSize = lstrlen (pszArg) + 1;
		fwrite (&wRecSize, 1, sizeof(wRecSize), file);
		fwrite (pszArg, 1, lstrlen (pszArg) + 1, file);
		nArgLen += sizeof(wRecSize) + lstrlen (pszArg) + 1;
	}
	return nArgLen;
}

void 
main(
int argc,
char * argv[],
char * envp[])
{
	HANDLE hSrcFile = NULL;
	PBYTE pbBuf = NULL;
	FILE * fileDest = NULL;
	LPCSTR pszSrcFileName = NULL;
	LPCSTR pszDestFileName = NULL;

	try
	{
	   #if defined(ADDREC)
		if (argc < 4 || (argc % 2 == 0))
			throw ("ADDREC - adds or modifies certain records in an .AVB or .BGB file.\n"
				   "\n"
				   "Usage:\n"
				   "\tADDREC <infile> <outfile> [<recordtype> <data>] [<recordtype> <data>] ...\n"
 				   "\n"
 				   "\tThe following record types and data are supported:\n"
				   "\tNAME\t\t<new name>\n"
				   "\tORIGURL\t\t<original URL>\n"
				   "\tNEWURL\t\t<new URL>\tOverrides\n"
				   "\tCOPYRIGHT\t\t<copyright message>\n"
				   "\tUSAGE FLAGS\t\t<usage flags value>\n"
				   "\n"
				   "For details consult the documentation.\n");
		pszSrcFileName = argv[1];
		pszDestFileName = argv[2];
	   #elif defined(CHANGEURL)
	    if (argc != 3)
			throw ("CHANGEURL - changes the URL of an .AVB or .BGB file, if possible.\n"
				   "\n"
				   "Usage:\n"
				   "\tCHANGEURL <file> <newurl>\n"
				   "\n"
				   "Note: CHANGEURL will fail if the author of the file has not\n"
				   "      given permission to change the URL.\n");

		// Generate a temporary filename.
		char szTempDir[_MAX_PATH], szTempPath[_MAX_PATH];
		GetTempPath (sizeof(szTempDir), szTempDir);
		if (GetTempFileName (szTempDir, "CAU", 0, szTempPath) == 0) {
			throw ("Couldn't get a temporary filename");
		}
	    pszSrcFileName = argv[1];
		pszDestFileName = szTempPath;
	   #endif

		// Process arguments.

	   #if defined(ADDREC)
		for (int i = 3; i < argc; i += 2) {
			for (int iRec = 0; iRec < NUMRECS; iRec++) {
				if (!stricmp (pszRecordsSupported[iRec], argv[i])) {
					nRecordsToBeWorkedOn[iRec] = i + 1;
					break;
				}
			}
		}
	   #elif defined(CHANGEURL)
	    nRecordsToBeWorkedOn[WHICH_RECORD_IS_NEW_URL] = 2;
	   #endif

		// Look for the file.
		if (GetFileAttributes (pszSrcFileName) == (DWORD)-1L)
			throw ("File not found.");

		// Open the file.
		hSrcFile = CreateFile (pszSrcFileName, GENERIC_READ, 0, NULL, OPEN_EXISTING, 0, NULL);
		if (hSrcFile == INVALID_HANDLE_VALUE)
			throw ("Error reading file");

		// Read the whole file into memory.
		DWORD dwSize = GetFileSize (hSrcFile, NULL);
		DWORD dwSizeRead;
		if (dwSize == (DWORD)-1L) 
			throw ("Error reading file");
		pbBuf = (PBYTE)malloc (dwSize);
		if (pbBuf == NULL)
			throw ("Out of memory.");
		if (!ReadFile (hSrcFile, pbBuf, dwSize, &dwSizeRead, NULL)  || dwSizeRead != dwSize)
			throw ("Error reading file");

		CloseHandle (hSrcFile);
		hSrcFile = NULL;

		// Open the destination file for writing.

		fileDest = fopen (pszDestFileName, "wb");
		if (fileDest == NULL)
			throw ("Error writing file");

		DWORD dwSrcOffset = 0;

		// Copy out the header.

		fwrite (pbBuf, 1, 3 * sizeof(WORD), fileDest);
		dwSrcOffset += 3 * sizeof(WORD);

		// Write out an adjustment record. The first word is the record type,
		// the second word is the record size, the 3rd/4th words is the adjustment size.
		WORD wAdjRec[4];
		wAdjRec[0] = 263; /* AK_OFFSET_ADJUSTMENT */
		wAdjRec[1] = sizeof(DWORD);
		fwrite (wAdjRec, 1, sizeof(wAdjRec), fileDest);

		WORD tag;
		int nRecordSize;
		long nSizeChange = 0;
		while (TRUE) {

			tag = *(LPWORD)(pbBuf + dwSrcOffset);
			if (tag == 6 /* AK_STARTDATA */) {
				break;
			}

		   #if defined(CHANGEURL)
			// If this is the usage flags tag, check if changing the URL
			// is permitted.
			if (tag == wRecordsSupported[WHICH_RECORD_IS_USAGE_FLAG] &&
					(pbBuf[dwSrcOffset + 2 * sizeof(WORD)] & 1) == 1) {
				throw ("Sorry, the author of this file has not permitted others to change the URL.");
			}
		   #endif

			// Calculate the length of the record.
			if (tag < 256) {
				if (nOldRecordSizes[tag] < 0) {
					WORD nCount = *(LPWORD)(pbBuf + dwSrcOffset + sizeof(WORD));
					nRecordSize = sizeof(WORD) - ((int)nCount) * nOldRecordSizes[tag];
				}
				else if (nOldRecordSizes[tag] == 0) {
					nRecordSize = 0;
					while (pbBuf[dwSrcOffset + sizeof(WORD) + nRecordSize] != 0) {
						nRecordSize++;
					}
					nRecordSize++;
				}
				else {
					nRecordSize = nOldRecordSizes[tag];
				}
			}
			else {
				// Have to add sizeof(WORD), because the record size field
				// doesn't take itself into account.
				nRecordSize = (int)(*(LPWORD)(pbBuf + dwSrcOffset + sizeof(WORD))) + sizeof(WORD);
			}

			// Is this one of the special records? If so, we may have to replace or
			// omit it.
			
			BOOL bSkipWrite = FALSE;
			if (tag == 3 /* AK_ICON */ || tag == 256 /* AK_ICON_NEW */ || tag == 258 /* AK_BACKDROP */) {
				// Before writing out this tag, write out everything else.
				for (int iRec = 0; iRec < NUMRECS; iRec++) {
					if (nRecordsToBeWorkedOn[iRec] > 0) {
						nSizeChange += WriteOutRecord (iRec, argv[nRecordsToBeWorkedOn[iRec]], fileDest);
						nRecordsToBeWorkedOn[iRec] = -1;
					}
				}
			}
			else {
				for (int iRec = 0; iRec < NUMRECS; iRec++) {
					if (wRecordsSupported[iRec] == tag &&
							nRecordsToBeWorkedOn[iRec] != 0) {
						if (nRecordsToBeWorkedOn[iRec] > 0) {
							nSizeChange += WriteOutRecord (iRec, argv[nRecordsToBeWorkedOn[iRec]], fileDest);
							nRecordsToBeWorkedOn[iRec] = -1;
						} 
						else {
							// Omit this record, so we do nothing.
						}
						bSkipWrite = TRUE;
						break;
					}
				}
			}

			if (bSkipWrite) {
				nSizeChange -= sizeof(WORD) + nRecordSize;
			} else {
				fwrite (&tag, 1, sizeof(WORD), fileDest);
				fwrite (pbBuf + dwSrcOffset + sizeof(WORD), 1, nRecordSize, fileDest);
			}

			dwSrcOffset += sizeof(WORD) + (DWORD)nRecordSize;
		}

		// Write out the rest of the file.

		fwrite (pbBuf + dwSrcOffset, 1, dwSize - dwSrcOffset, fileDest);

		// Go back and change the offset value.

		fseek (fileDest, 5 * sizeof(WORD), SEEK_SET);
		nSizeChange += sizeof(wAdjRec);
		fwrite (&nSizeChange, 1, sizeof(nSizeChange), fileDest);

		fclose (fileDest);
		fileDest = NULL;

	   #if defined(CHANGEURL)
	    // Have to copy the file back over.
		if (!CopyFile (pszDestFileName, pszSrcFileName, FALSE)) {
			throw ("Failed to overwrite original file.");
		}
	   #endif

		puts ("Changes succeeded.");
	}
	catch (LPCSTR pszErrorMsg)
	{
		puts (pszErrorMsg);
	}

	if (hSrcFile != NULL && hSrcFile != INVALID_HANDLE_VALUE) {
		CloseHandle (hSrcFile);
	}
	free (pbBuf);
	if (fileDest != NULL) {
		fclose (fileDest);
	}

   #if defined(CHANGEURL)
    if (pszDestFileName) {
		DeleteFile (pszDestFileName);
	}
   #endif
   
}
