#ifndef __FORMAT_H__
#define __FORMAT_H__

// Constants used in text formatting
const char chCtlBold			= 0x02;	// bold mode switch
const char chCtlColor			= 0x03;	// color definition
const char chCtlLink			= 0x0C;	// URL mode switch
const char chCtlFixedPitchFont	= 0x11;	// fixed pitch font switch
const char chCtlSymbol			= 0x12;	// symbol font switch
const char chCtlItalic			= 0x16;	// italic mode switch
const char chCtlUnderline		= 0x1F;	// underline mode switch

// REGISB: !need to be in sync with values in Intl.H!
const WORD wBold				= 0x0100;
const WORD wItalic				= 0x0200;
const WORD wUnderline			= 0x0400;
const WORD wFixedPitch			= 0x0800;
const WORD wSymbol				= 0x1000;
const WORD wForeground			= 0x2000;
const WORD wBackground			= 0x4000;
const WORD wLink				= 0x8000;

const char* const FIXEDPITCHFACENAMES[]	= { _T("Courier"), _T("Courier New"), _T("LinePrinter"), _T("Terminal"), _T("Fixedsys") } ;
const char* const SYMBOLFACENAMES[]		= { _T("Symbol"), _T("Wingdings"), _T("HM Phonetic"), _T("Marlett") } ;
const short FIXEDPITCHNUMBER		= 5;
const short SYMBOLNUMBER			= 4;

const short MAX_URL_INTEXT			= 16;	// Maximum allowed URLs in single text message

extern BYTE			GetColorCode(COLORREF cr);
extern COLORREF		GetRBGColor(BYTE byteCode);
extern short		nResettingSequence(const char *szInput, char *szResetSeq);
extern short		nFillFormatting(char *szFormatting, WORD wCurFormat, WORD wNextFormat, char chFirstFormattedChar);
extern const char*	SzSkipOneFormat(const char *szInput, WORD *pwFormat);
extern char*		SzControlLess(char *szInput, CDWordArray *prgdwFormatting);
extern char*		SzControlFull(const char *szInput, CDWordArray *prgdwFormatting);
extern CSize		GetFormattedTextExtent(CDC *pdc, LPCTSTR szInput, DWORD cbLen, CDWordArray *prgdwFormatting);
extern COLORREF		GetRBGColor(BYTE byteCode);
extern short		FFixedPitchFont(LPCTSTR cszFaceName);
extern short		FSymbolFont(LPCTSTR cszFaceName);
extern short		nGetSpecialFontIndex(BOOL bFixedPitchFont);
extern CDWordArray* PRGDWGetFormatting(CRichEditCtrl *pRichEdit, CFont *pDefaultFont, COLORREF crDefaultColor);
extern CDWordArray* AddFormat(CDWordArray *prgdwFormatting, DWORD dwElement);
extern CDWordArray* InsertFormat(CDWordArray *prgdwFormatting, BOOL bAddFormat, WORD wFormat, WORD wOffset);
extern CDWordArray* CopyFormatting(CDWordArray *prgdwFormatting);
extern CDWordArray* CopyLinksFormatting(CDWordArray *prgdwFormatting);
extern BOOL			bFormattingsEqual(CDWordArray* prgdwFormatting1, CDWordArray *prgdwFormatting2);
extern CDWordArray* CutFormattingArray(CDWordArray *prgdwFormatting, SHORT nNewStringLenth);
extern CDWordArray* PullFormattingOffsets(CDWordArray *prgdwFormatting, SHORT nDeltaOffset);
extern void			PushFormattingOffsets(CDWordArray *prgdwFormatting, SHORT nDeltaOffset);
extern void			PushFormattingOffsetsDW(DWORD *prgdwFormatting, INT cFormats, SHORT nDeltaOffset);
extern BOOL			bURLPresent(CDWordArray *prgdwFormatting);
extern BOOL			bSizorPresent(CDWordArray *prgdwFormatting);
extern BOOL			bLOGFONTToCHARFORMAT(LOGFONT *pLogFont, COLORREF crColor, DWORD dwMask, CHARFORMAT *pCharFormat);
extern void			MatchFont(LOGFONT &lf);

#endif // __FORMAT_H__
