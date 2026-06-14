// Copyright (c) Microsoft Corpration
//
// File Name:   sjis2jis.c
// Owner:       Tetsuhide Akaishi
// Revision:    1.00  02/21/'93  Tetsuhide Akaishi
//

#include "windows.h"
#include "fechrcnv.h"
#include "tchar.h"
#include "cdebug.h"
// for ASSERT and FAIL
//
SZTHISFILE

void ShiftJISChar_to_JISChar ( UCHAR *pShiftJIS, UCHAR *pJIS )

// The ShiftJISChar_to_JISChar function convert one character string 
// as Shift JIS code to a JIS code string. 
//
// UCHAR *pShiftJIS     Points to the character string to be converted.
//
// UCHAR *pJIS          Points to a buffer that receives the convert string
//                      from Shift JIS Code to JIS.
//
// Return Value
// None 

{
	USHORT	hi_code, low_code;

	hi_code = (*pShiftJIS);
	low_code = *(pShiftJIS+1);
	hi_code -= (hi_code > 0x9f ? 0xb1 : 0x71);
	hi_code = hi_code * 2 + 1;
	if ( low_code > 0x9e ) {
		low_code -= 0x7e;
		hi_code ++;
	}
	else {
		if ( low_code > 0x7e ) {
			low_code --;
		}
		low_code -= 0x1f;
	}
	*(pJIS) = (UCHAR)hi_code;
	*(pJIS+1) = (UCHAR)low_code;
	return;
}


int ShiftJIS_to_JIS ( UCHAR *pShiftJIS, int ShiftJIS_len,
                                                UCHAR *pJIS, int JIS_len )

// The ShiftJIS_to_JIS function convert a character string as Shift JIS code 
// to a JIS code string. 
//
// UCHAR *pShiftJIS     Points to the character string to be converted.
//
// int   ShiftJIS_len   Specifies the size in bytes of the string pointed
//                      to by the pShiftJIS parameter. If this value is -1,
//                      the string is assumed to be NULL terminated and the
//                      length is calculated automatically.
//
// UCHAR *pJIS          Points to a buffer that receives the convert string
//                      from Shift JIS Code to JIS.
//         
// int   JIS_len        Specifies the size, in JIS characters of the buffer
//                      pointed to by the pJIS parameter. If the value is zero,
//                      the function returns the number of JIS characters 
//                      required for the buffer, and makes no use of the pJIS 
//                      buffer.
//
// Return Value
// If the function succeeds, and JIS_len is nonzero, the return value is the 
// number of JIS characters written to the buffer pointed to by pJIS.
//
// If the function succeeds, and JIS_len is zero, the return value is the
// required size, in JIS characters, for a buffer that can receive the 
// converted string.
//
// If the function fails, the return value is -1. The error mean pJIS buffer
// is small for setting converted strings.
//

{
    BOOL    kanji_in = FALSE;      // Kanji Mode
    BOOL    kana_in = FALSE;       // Kana  Mode
    int     re;                    // Convert Lenght
    int     i;                     // Loop Counter

    if ( ShiftJIS_len == -1 ) {
        // If length is not set, last character of the strings is NULL.
        ShiftJIS_len = strlen ( (const char *) pShiftJIS ) + 1;
    }
    i = 0;
    re = 0;
    if ( JIS_len == 0 ) {
        // Only retrun the required size
        while ( i < ShiftJIS_len ) {
            if ( SJISISKANJI(*pShiftJIS) ) {  // Is this charcter 2 bytes Kanji?
                if ( kana_in ) {            // Kana Mode?
                    re ++;
                    kana_in = FALSE;         // Reset Kana Mode;
                }
                if ( kanji_in == FALSE ) {  // Kanji Mode?
                    re += KANJI_IN_LEN;
                    kanji_in = TRUE;        // Set Kanji Mode
                }

                i+=2;
                re += 2;
                pShiftJIS+=2;
            }
            else if ( SJISISKANA(*pShiftJIS) ) {
                if ( kanji_in ) {
                    re += KANJI_OUT_LEN;
                    kanji_in = FALSE;
                }
                if ( kana_in == FALSE ) {
                     re ++;
                     kana_in = TRUE;
                }
                i++;
                re++;
                pShiftJIS++;
            }
            else {
                if ( kana_in ) {
                    re ++;
                    kana_in = FALSE;
                }
                if ( kanji_in ) {
                    re += KANJI_OUT_LEN;
                    kanji_in = FALSE;
                }
                i++;
                re++;
                pShiftJIS++;
            }
        }
        if ( kana_in ) {
            re ++;
            kana_in = FALSE;
        }
        if ( kanji_in ) {
            re += KANJI_OUT_LEN;
            kanji_in = FALSE;
        }
        return ( re );
    }
    while ( i < ShiftJIS_len ) {
        if ( SJISISKANJI(*pShiftJIS) ) {  // Is this charcter 2 bytes Kanji?
            if ( kana_in ) {            // Kana Mode?
                if ( re >= JIS_len ) {   // Buffer Over?
                    return ( -1 );
                }
                (*pJIS++) = SI;     // Set Kana Out Charcter
                re ++;
                kana_in = FALSE;         // Reset Kana Mode;
            }
            if ( kanji_in == FALSE ) {  // Kanji Mode?
                if ( re + KANJI_IN_LEN > JIS_len ) {   // Buffer Over?
                    return ( -1 );
                }
                (*pJIS++) = ESC;    // Set Kanji In Charcter
                (*pJIS++) = KANJI_IN_1ST_CHAR;
                (*pJIS++) = KANJI_IN_2ND_CHAR1;
                re += KANJI_IN_LEN;
                kanji_in = TRUE;        // Set Kanji Mode
            }

            if ( re + 2 > JIS_len ) {   // Buffer Over?
                return ( -1 );
            }
            ShiftJISChar_to_JISChar ( pShiftJIS, pJIS );
            i+=2;
            re += 2;
            pShiftJIS+=2;
            pJIS += 2;
        }
        else if ( SJISISKANA(*pShiftJIS) ) {
            if ( kanji_in ) {
                if ( re + KANJI_OUT_LEN > JIS_len ) {   // Buffer Over?
                    return ( -1 );
                }
                // Set Kanji Out Charcter
                (*pJIS++) = ESC;
                (*pJIS++) = KANJI_OUT_1ST_CHAR;
                (*pJIS++) = KANJI_OUT_2ND_CHAR2;  // Changing from CHAR1 to CHAR2 (standard IRC) - djk & regisb
                re += KANJI_OUT_LEN;
                kanji_in = FALSE;
            }
            if ( kana_in == FALSE ) {
                if ( re >= JIS_len ) {   // Buffer Over?
                    return ( -1 );
                }
                (*pJIS++) = SO;	// Set Kana In Charcter
                re ++;
                kana_in = TRUE;
            }
            if ( re >= JIS_len ) {   // Buffer Over?
                return ( -1 );
            }
            (*pJIS++) = (*pShiftJIS++) & 0x7f;
            i++;
            re++;
        }
        else {
            if ( kana_in ) {
                if ( re >= JIS_len ) {   // Buffer Over?
                    return ( -1 );
                }
                (*pJIS++) = SI;	// Set Kana Out Charcter
                re ++;
                kana_in = FALSE;
            }
            if ( kanji_in ) {
                if ( re + KANJI_OUT_LEN > JIS_len ) {   // Buffer Over?
                    return ( -1 );
                }
                // Set Kanji Out Charcter
                (*pJIS++) = ESC;
                (*pJIS++) = KANJI_OUT_1ST_CHAR;
                (*pJIS++) = KANJI_OUT_2ND_CHAR2;   // Changing from CHAR1 to CHAR2 (standard IRC) - djk & regisb
                re += KANJI_OUT_LEN;
                kanji_in = FALSE;
            }
            if ( re >= JIS_len ) {   // Buffer Over?
                return ( -1 );
            }
            (*pJIS++) = (*pShiftJIS++);
            i++;
            re++;
        }
    }
    if ( kana_in ) {
        if ( re >= JIS_len ) {   // Buffer Over?
            return ( -1 );
        }
        (*pJIS++) = SI;	// Set Kana Out Charcter
        re ++;
        kana_in = FALSE;
    }
    if ( kanji_in ) {
        if ( re + KANJI_OUT_LEN  > JIS_len ) {   // Buffer Over?
            return ( -1 );
        }
        // Set Kanji Out Charcter
        (*pJIS++) = ESC;
        (*pJIS++) = KANJI_OUT_1ST_CHAR;
        (*pJIS++) = KANJI_OUT_2ND_CHAR2;		// Changing from CHAR1 to CHAR2 (standard IRC) - djk & regisb
        re += KANJI_OUT_LEN;
        kanji_in = FALSE;
    }
    return ( re );
}


// TO simplify th e conversions and to automaticlly allocate buffers - RamuM
int OurShiftJIS_to_JIS (UCHAR *pShiftJIS, int ShiftJIS_len, UCHAR **ppJIS, int JIS_len)
{
	// need to convert to jis
	INT	nSJisLen;
	INT nJisLen;

	if ((nSJisLen = ShiftJIS_len) == -1)
		nSJisLen= _tcslen((TCHAR*)pShiftJIS)+1;	// should include null termination in length

	// Always find the length of JIS string, since it depends on the input string - RamuM
	nJisLen = ShiftJIS_to_JIS(pShiftJIS, nSJisLen, NULL, 0);

	if (nJisLen <= 0 || !ppJIS)
		return (nJisLen);

	// No length provided, means allocate the buffer
	if (JIS_len <= 0)
	{
		*ppJIS = (UCHAR*) new TCHAR[nJisLen];
		if (!*ppJIS)
			return (-1);
		ZeroMemory(*ppJIS, nJisLen);
	}
	else if (JIS_len < nJisLen)
		return(-1);

	// ConvertINetString(CP_JPN_SJ, CP_ISO_2202_JP, szText, &cbText, szTextMessage, (LPINT) &cbTextMessage);
	int i = ShiftJIS_to_JIS(pShiftJIS, nSJisLen, (UCHAR*) *ppJIS, nJisLen);
	ASSERT(i > 0, "ShiftJIS_to_JIS failed in OurShiftJIS_to_JIS");

	// We allocated  the buffer and returning an error value
	if ((JIS_len <= 0) && (i <= 0))
	{
		delete [] *ppJIS;
		*ppJIS = NULL;
	}

#ifdef DEBUG
	if (i > 0)	// the ShiftJIS_to_JIS conversion succeeded
	{
		CONV_CONTEXT ccontext;
		ZeroMemory(&ccontext,sizeof(CONV_CONTEXT));

		TCHAR *szTemp;
		szTemp = new TCHAR[nSJisLen];

		if (!szTemp)
			return (-1);

		int j = JIS_to_ShiftJIS(&ccontext, (UCHAR*) *ppJIS, i, (UCHAR*) szTemp, nSJisLen);
		ASSERT(j > 0, "Debug: JIS_to_ShiftJIS failed in OurShiftJIS_to_JIS");
		ASSERT(j == nSJisLen, "Debug: JIS_to_ShiftJIS returns unequal string in OurShiftJIS_to_JIS");

		delete [] szTemp;
	}
#endif DEBUG

	return (i);
}