// Copyright (c) 1995  Microsoft Corpration
//
// File Name : fechrcnv.h
// Owner     : Tetsuhide Akaishi
// Revision  : 1.00 07/20/'95 Tetsuhide Akaishi
//

// Shift JIS Kanji Code Check
#define SJISISKANJI(c) ( ( (UCHAR)(c) >= 0x81 && (UCHAR)(c) <= 0x9f ) || \
                         ( (UCHAR)(c) >= 0xe0 && (UCHAR)(c) <= 0xfc ) )

// Shift JIS Kana Code Check
#define SJISISKANA(c) ( (UCHAR)(c) >= 0xa1 && (UCHAR)(c) <= 0xdf )

#define ESC     0x1b
#define SO      0x0e
#define SI      0x0f

// Define for JIS Code Kanji and Kana IN/OUT characters
#define KANJI_IN_1ST_CHAR       '$'
#define KANJI_IN_2ND_CHAR1      'B'
#define KANJI_IN_2ND_CHAR2      '@'
#define KANJI_IN_STR            "$B"
#define KANJI_IN_LEN             3
#define KANJI_OUT_1ST_CHAR      '('
#define KANJI_OUT_2ND_CHAR1     'J'
#define KANJI_OUT_2ND_CHAR2     'B'
#define KANJI_OUT_LEN            3
#define KANJI_OUT_STR           "(J"

// Define for Japanese Code Type
#define CODE_UNKNOWN            0
#define CODE_ONLY_SBCS          0
#define CODE_JPN_JIS            1
#define CODE_JPN_EUC            2
#define CODE_JPN_SJIS           3

// Minimum length to determine if the string is EUC
#define MIN_JPN_DETECTLEN      16

#pragma pack(8)
typedef struct _dbcs_status
{
    int nCodeSet;
    UCHAR cSavedByte;
    BOOL fESC;
} DBCS_STATUS;

typedef struct _conv_context
{
    DBCS_STATUS dStatus0;
    DBCS_STATUS dStatus;
    
    BOOL blkanji0;  // Kanji In Mode
    BOOL blkanji;   // Kanji In Mode
    BOOL blkana;    // Kana Mode
    int  nCurrentCodeSet;
} CONV_CONTEXT;
#pragma pack()

// ----------------------------------
// Public Functions for All FarEast
//-----------------------------------

//--------------------------------
// Internal Functions for Japanese
//--------------------------------

// Detect Japanese Code
// int DetectJPNCode ( UCHAR *string, int len );

// Convert from Shift JIS to JIS
extern int ShiftJIS_to_JIS (
    UCHAR *pShiftJIS,
    int ShiftJIS_len,
    UCHAR *pJIS,
    int JIS_len
    );

// Convert from JIS to Shift JIS
extern int JIS_to_ShiftJIS (
    CONV_CONTEXT *pcontext,
    UCHAR *pShiftJIS,
    int ShiftJIS_len,
    UCHAR *pJIS,
    int JIS_len
    );

// our routines to simplify conversions - RamuM
extern int OurShiftJIS_to_JIS (UCHAR *pShiftJIS, 
							   int ShiftJIS_len, 
							   UCHAR **ppJIS, 
							   int JIS_len=0);

extern int OurJIS_to_ShiftJIS (UCHAR *pJIS, 
							   int JIS_len, 
							   UCHAR **ppSJIS, 
							   int ShiftJIS_len=0);

