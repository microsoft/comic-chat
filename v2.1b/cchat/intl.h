/* This file contains definitions for international support.
**
**  Copyright (c) 1996 Microsoft Corporation.  All Rights Reserved.
*/

// REGISB added 09/30/97 !need to be in sync with values in Format.H!
#define wBold			0x0100
#define wItalic			0x0200
#define wUnderline		0x0400
#define wFixedPitch		0x0800
#define wSymbol			0x1000


#define IEXPLORE
//#include <fechrcnv.h>

// Index value for iChrCnv 
#define ICHRCNV_NONE   0
#define ICHRCNV_AUTO   1
#define ICHRCNV_JIS    2
#define ICHRCNV_EUCJP  3
#define ICHRCNV_SJIS   4
#define DBCS_CHARSET(cp) \
     (((cp) == 932)?SHIFTJIS_CHARSET  \
     :(((cp) == 949)?HANGEUL_CHARSET   \
     :(((cp) == 950)?CHINESEBIG5_CHARSET:GB2312_CHARSET))) 

#define CP_AUTODETECT -1


// The followings are ugly hard code but RC.EXE converts them to unicode
// based on the current ACP so we hide these in the C code otherwise we
// need to have resource written in unicode.

#define JAPAN_DEFAULTFONT "‚l‚r ‚oƒSƒVƒbƒN" 
#define KOREA_DEFAULTFONT "±¼¸²" 
#define TRADCHINA_DEFAULTFONT "·s²Ó©úÅé" 
#define SIMPLECHINA_DEFAULTFONT "ËÎÌå" 

#define DBCS_DEFAULTFONT(cp) \
     (((cp) == 932)?JAPAN_DEFAULTFONT  \
     :(((cp) == 949)?KOREA_DEFAULTFONT   \
     :(((cp) == 950)?TRADCHINA_DEFAULTFONT:SIMPLECHINA_DEFAULTFONT))) 


#define _IsFECodePage(cp) \
	((cp) == 932 || (cp) == 949 || (cp) == 950 || (cp) == 936)

#define ELEFLAG_NOBREAK	0x00000100		// from tw.h

typedef struct _mimecset {
    BOOL         fFECodePage; // Put first since used the most
    BOOL         fValidCodePage; // Code page is valid on this machine
    BOOL         fValidAltCodePage; // AltCP is valid on this machine
    TCHAR        *Lang_str;	// string value defined for language.
    TCHAR        *Mime_str;	// string value defined for the mime charset.
    TCHAR        *AliasName; // alias charaset name table
    int          CodePage;	// NLS codepage
    int          AltCP;         // Alternative codepage
    int          iChrCnv;       // Index of FEChrCnv + 1
    BOOL        (*IsWrapUp)(BOOL fDBCS, LPCTSTR psz);
    BOOL        (*IsWrapDown)(BOOL fDBCS, LPCTSTR psz);

} MIMECSET;
typedef MIMECSET *PMIMECSET;
typedef struct _mimecsettable MIMECSETTABLE, *PMIMECSETTABLE;
struct _mimecsettable {
    PMIMECSET pmime;
    MIMECSETTABLE *pnext;
};

// Hard coded database for default script/encoding information
// The later half of the struct is actually a union of MIMECSET
typedef struct {
    int iLangID;   // resource id for default script name
    int iEncodeID; // resource id for default encoding name
    int iCp;       // windows codepage
    int iInetCp;   // codepage id for internet encoding
    int iChrCnv;   // internal idx for ichrcnv
    BOOL        (*IsWrapUp)(BOOL fDBCS, LPCTSTR psz);   // func ptr for wrap-up
    BOOL        (*IsWrapDown)(BOOL fDBCS, LPCTSTR psz); // func ptr for wrap-down
    
} SCRIPTINFO, *PSCRIPTINFO;

#define IsFEPMime(p) (p)->fFECodePage
#define IsInstalledPMime(p) (p)->fValidCodePage
#define IsAltInstalledPMime(p) (p)->fValidAltCodePage
#define IsWrapDownChar(p, fDBCS, psz) ((p)->IsWrapDown?(p)->IsWrapDown(fDBCS, psz):FALSE)
#define IsWrapUpChar(p, fDBCS, psz)   ((p)->IsWrapUp?(p)->IsWrapUp(fDBCS, psz):FALSE)
#define IsCodepageNeedAutoDetect(codepage) ((codepage) == 932)
//extern PMIMECSETTABLE _aMimeCharSet;
#define aMimeCharSet ((const MIMECSETTABLE *)_aMimeCharSet)
#define GETMIMECP(p)     (p)->pMime->CodePage
#define GETMIMECKEY(p) (p)->pMime->iChrCnv

#define cpPmime(p)   (p)->CodePage		/* code page from pmime */
#define pmimeImime(imime) GetMimeCharsetEntryAt((imime))
#define cpImime(imime) cpPmime(pmimeImime(imime)) /* code page from imime */

// isspace doesn't work with non-ascii characters.
// I don't check codepage here because this works for ascii codepage
// anyway.
//
#undef isspace
#define isspace(c) ((c==' ')||(c=='\t')||(c=='\n')||(c=='\r')||(c=='\v')|| \
                    (c=='\f'))

// prototypes
void CharConv_Init(void **ppctxt);
void CharConv_DeInit(void **ppctxt);
LPTSTR CharPrevEx(int codepage, LPCTSTR lpszSt, LPCTSTR lpsz);
LPTSTR CharNextEx(int codepage, LPCTSTR lpsz);
LPTSTR FindBreakableCharForFE(PSCRIPTINFO pMime, LPTSTR lpszStart, int textLen, BOOL *pbWrapupAtTop);
BOOL FindSubStringForINTLThatFits(
	void	*vMime, 
	HDC		hdc,
	LPCTSTR	szString, 
	int		cbString,
	DWORD	*prgdwFormatting,
	int		cFormats,
	LPCTSTR	szFixedPitchName,
	LPCTSTR	szSymbolName,
	int		*pcbFit,  
	BOOL	*pbHasBlankOrAlike,
	LPSIZE	lpSize,
	int		nMaxExtent
);

void GetTextExtentOfLongestWordForINTL(
	PSCRIPTINFO	pMime, 
	HDC			hdc, 
	LPCSTR		szString,
	int			cbString,
	DWORD		*prgdwFormatting,
	int			cFormats,
	LPCTSTR		szFixedPitchName,
	LPCTSTR		szSymbolName,
	LPSIZE		lpSize,
	ULONG		lFlags
);

// For use of charset tag
void SetCharsetFromAlias(const char *alias, struct Mwin *tw);
LPCTSTR GetCharsetFromContent(LPCTSTR szContent);

BOOL IsDBCSLeadByteDoc(struct _www *pdoc, BYTE ch);

void InitMimeCsetTable(void);
void FreeMimeCsetTable(MIMECSETTABLE * pMimeTbl);
void SetDefaultDialogFont(HWND hDlg, int idCtl);
void RemoveDefaultDialogFont(HWND hDlg);
void PatchLangForISO639(UINT cp, TCHAR *szLang, int cchLang);
int TranslateStrCodepage(char *pSzFrom, int cchFrom, char *pSzTo, int cchTo, PMIMECSET pMime, BOOL bRestore);
BOOL MimeCharSetList_Init(MIMECSETTABLE * pTbl);
BOOL MimeCharSetList_Cleanup(void);
PMIMECSET GetMimeCharsetEntryAt(int position);
