/*
	*****************************************************************
	*																*
	*	Module 			: TextView.h, Text View for Microsoft Chat	*
	*																*
	*	Author 			: RamuM, 11/1/96							*
	*																*
	*	Current Owner	: RamuM										*
	*																*
	*	Changes			:											*
	*																*
	*	To Dos			: UniCode Support							*
	*																*
	*****************************************************************
*/


#ifndef __TEXTVIEW_H__

#include <windows.h>
#include <richedit.h>
#include <tchar.h>

#include "MsgType.h"
#include "UrlUtil.h"

#ifndef TEXTVIEWCLASSNAME
#define TEXTVIEWCLASSNAME	CTextView
#endif // TEXTCORE								

// The hwnd being passed to Attach must use these constants.
const TCHAR TEXT_VIEW_WND_CLASS_NAME[]	= _T("RICHEDIT");		// Window Class name used for TextView
const TCHAR TEXT_VIEW_WND_DLL[]			= _T("RICHED32.DLL");	// DLL needed for TextView Window Class
const DWORD TEXT_VIEW_WND_FLAGS			=  ES_SAVESEL|ES_NOHIDESEL|ES_MULTILINE|ES_READONLY|ES_AUTOVSCROLL;	// Style Flags needed for TextView

const float TEXT_VIEW_BUFFER_INFORM_FULL	= ((float) 0.9);	// User is informed when text view buffer is >=90% full	
const float TEXT_VIEW_BUFFER_CUTOFF			= ((float) 0.2);	// First 20% of the history is cut when text view buffer gets full

const short	TEXT_VIEW_BLANK_NEVER			= 0x00;	// Do not leave blank lines between messages
const short	TEXT_VIEW_BLANK_DIFFTYPES		= 0x01;	// Leave blank lines only between messages of different types
const short	TEXT_VIEW_BLANK_ALWAYS			= 0x02;	// Leave blank lines between all messages

const short TEXT_VIEW_BLANK_MIN				= TEXT_VIEW_BLANK_NEVER;
const short TEXT_VIEW_BLANK_MAX				= TEXT_VIEW_BLANK_ALWAYS;

const short TEXT_VIEW_AUTOSCROLL_NEVER		= 0x00;	// Do not AutoScroll
const short TEXT_VIEW_AUTOSCROLL_NOSELECT	= 0x01;	// Do Not AutoScroll if text is selected
const short TEXT_VIEW_AUTOSCROLL_NOMIDDLE	= 0x02;	// Do Not AutoScroll if cursor is in the middle
const short TEXT_VIEW_AUTOSCROLL_ALWAYS		= 0x04;	// AutoScroll always

const short TEXT_VIEW_AUTOSCROLL_MIN		= TEXT_VIEW_AUTOSCROLL_NEVER;
const short TEXT_VIEW_AUTOSCROLL_MAX		= TEXT_VIEW_AUTOSCROLL_ALWAYS;

const short DEFAULT_INDENT					= 205;	// Default indent of text when set, 205 is Magic number (french;-)

const TCHAR g_szHeaderTab[]					= _T("  ");
const short g_nHeaderTabLen					= 2;

// Text Format information for different Chat Message Types
typedef struct MsgTypeProp
{
	CHARFORMAT	CharFormat;	// Character formatting, from richedit
} MSG_TYPE_PROP;


/*
	***** RETURN Values of iDisplayYYY() Fucntions on CTextView Object *****

	 INT return values to be interpreted as follows:
	 0			- Function succeeded
	 -ve (<0)	- Error occured, call GetLastError() for error code
	 +ve (>0)	- Inform Text View Buffer Full event
				- Current Buffer Length crossed high water mark.

	***** dwXXXLen parameters of the iDisplayYYY() Functions on CTextView Object *****

	When passing String parameter values you can chose to
	pass lengths of the strings in the corresponding parameters
	or pass 0 to let the TextView object compute them.

*/

// CTextView object to handle all chat message formatting requests.
class TEXTVIEWCLASSNAME
{
	friend LRESULT CALLBACK TVWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

public:
	TEXTVIEWCLASSNAME();
    ~TEXTVIEWCLASSNAME();

	// creates the Text View Window
	BOOL	bCreateTextViewWindow(TCHAR *szName, 
									DWORD dwExStyle, DWORD dwStyle, 
									int x, int y, int nWidth, int nHeight, 
									HWND hwndParent, HINSTANCE hInstance);
	BOOL	AttachTextViewHWnd(HWND hwnd, HINSTANCE hInstance); 	// Set the hwnd for Text View
	HWND	GetTextViewHWnd() { return m_hwnd; }	// Get the current hwnd of Text View
	HWND	DetachTextViewHWnd() ;					// Detaches the hwnd associated with this TextView Object

	// For setting the high water mark and minimu cut off limit of Text View Buffer
	// TODO: GetParams, if needed
	void	SetTextViewBufferParams(FLOAT fBuffCutOff, FLOAT fBuffFull)
	{
		m_fBuffCutOff	= fBuffCutOff;
		m_fBuffFull		= fBuffFull;
	}

	// To manipulate the buffer size associated with TextView
	BOOL	bSetTextViewBufferMaxSize(DWORD dwBuffLength);		// Set the max Size for Text View Buffer
	DWORD	dwGetTextViewBufferMaxSize() { return m_dwBuffMaxSize; }		// Get the max Size of current Text View Buffer

	//To manipulate the current TextView Buffer
	DWORD	dwGetTextViewBufferSize() { return m_dwBuffSize; }		// Get the length of the Current text View Buffer
	DWORD	dwGetTextViewBuffer(LPTSTR szBuffer, DWORD dwBuffLength);	// Get the content of the Current text View Buffer
	DWORD	dwClearTextViewBuffer(DWORD dwMinCut = 0);		// Cuts the top part of the TextView buffer
															// fwMinCut: how many characters have to be cut at least
	DWORD	dwGetSelectedTextSize();
	DWORD	dwGetSelectedText(LPTSTR szBuffer, DWORD dwBuffLength);

	BOOL	bIsTextViewBufferGettingFull() { return (m_dwBuffSize >= m_fBuffFull*m_dwBuffMaxSize); }

	// Default font to be used by Text View
	BOOL	bSetTextViewDefaultFormat(CHARFORMAT *pCharFormat);	// Set the default font for text View Window
	BOOL	bGetTextViewDefaultFormat(CHARFORMAT **ppCharFormat); // Get the default font for text View Window

	// sets/gets the fonts and Fore Ground colors for strings in Text View
	BOOL	bSetDefaultMessageFormat(CHARFORMAT *pCharFormat, MSG_TYPE MsgType, BOOL bHeader);

	BOOL	bSetMessageFormat(CHARFORMAT *pCharFormat, MSG_TYPE MsgType, 
								MEMBER_STATUS MembFrom = msParticipant, 
								BOOL bHeader = FALSE);

	BOOL	bGetDefaultMessageFormat(CHARFORMAT **ppCharFormat, MSG_TYPE MsgType, BOOL bHeader);

	BOOL	bGetMessageFormat(CHARFORMAT **ppCharFormat, MSG_TYPE MsgType, 
								MEMBER_STATUS MembFrom = msParticipant, 
								BOOL bHeader = FALSE);

	// Sets/gets the Messsage strings for the headers and info to appear in buffer
	BOOL	bGetMessageString(LPCTSTR *pszMsgString, DWORD *pdwMsgLen, MSG_TYPE MsgType, MEMBER_STATUS Memb = (MEMBER_STATUS)-1);
	BOOL	bSetMessageString(LPCTSTR pszMsgString, DWORD dwMsgLen, MSG_TYPE MsgType, MEMBER_STATUS Memb = (MEMBER_STATUS)-1); 

	void	SetHeaderSeparate(BOOL bHeaderSeparate) { m_bHeaderSeparate = bHeaderSeparate; }
	
	// Set DBCS system
	void	SetDBCSSystem(BOOL bDBCSSystem)
				{ m_bDBCSSystem = bDBCSSystem; }

	// To insert blanks in between non-contiguous Message Types
	BOOL	bSetInsertBlank(short nInsert);
	short	nGetInsertBlank() { return m_nInsertBlank; }

	BOOL	bSetAutoScroll(short nAutoScroll);
	short	nGetAutoScroll() { return m_nAutoScroll; }

//	BOOL	bSetFormat(PARAFORMAT *pParaFormat);

	// autoscrolls the history window so that the latest text is visible
	BOOL	bAutoScrollTextView(BOOL bJustCheckEndInView = FALSE);

	//TODO: Might need to add lIndent for all iDisplay depending on the need

	// Displays "<Nickname> has [joined|left] the conversation." in the history box
	INT		iDisplayMemberStatus(LPCTSTR szNickname, DWORD dwNameLen,
									MSG_TYPE MsgType,
									MEMBER_STATUS MembAs = msParticipant,
									CHARFORMAT *pCharFormat = NULL);

	// Displays the name of the sender, the potential verb and the recipients if it's for a whisper
	INT		iDisplayMsgHeader(DWORD cbMessToFollow, 
								LPCTSTR szFrom, DWORD dwFromLen,
								LPCTSTR szTo, DWORD dwToLen,
								MSG_TYPE MsgType = mtNormal, 
								MEMBER_STATUS MembFrom = msParticipant,						 
								CHARFORMAT *pCharFormat = NULL);

	// Displays the given message text
	INT		iDisplayMsgText(LPCTSTR szText, DWORD dwTextLen, 
								MSG_TYPE MsgType = mtNormal,
								MEMBER_STATUS MembFrom = msParticipant,
								BOOL bShowURLs = TRUE, 
								BOOL bInformFull = TRUE, 
								BOOL bAppend = FALSE, 
								LONG lIndent = DEFAULT_INDENT,
								CHARFORMAT *pCharFormat = NULL,
								DWORD *prgdwFormatting = NULL,
								INT cFormats = 0);

	// Displays Actions
	INT		iDisplayAction(LPCTSTR szFrom, DWORD dwFromLen,
							LPCTSTR szAction, DWORD dwActionLen, 
							MEMBER_STATUS MembFrom = msParticipant,
							BOOL bShowURLs = TRUE, 
							CHARFORMAT *pCharFormat = NULL,
							DWORD *prgdwFormatting = NULL,
							INT cFormats = 0);

	// Displays Actions
	INT		iDisplayInfo(LPCTSTR szFrom, DWORD dwFromLen,
							LPCTSTR szTo, DWORD dwToLen,
							LPCTSTR szInfo, DWORD dwInfoLen, 
							MSG_TYPE MsgType,
							MEMBER_STATUS MembChange = msParticipant,
							CHARFORMAT *pCharFormat = NULL,
							DWORD *prgdwFormatting = NULL,
							INT cFormats = 0);

	// Handles the URL links
	BOOL	bHandleLink(ENLINK *penlink);

	void	SetURLBrowser(BOOL bNewBrowser=TRUE) { m_bNewBrowser = bNewBrowser; }
	BOOL	GetURLBrowser() { return(m_bNewBrowser); }

	// To Delete or Reset specific MessageTypeProperties
	BOOL	bReSetDefaultMsgTypeProperties(BOOL bReset = TRUE);

	// To Add-On message type properties based on member status
	BOOL	bAddMSMsgFormat(CHARFORMAT *pCharFormat, MEMBER_STATUS Member, BOOL bHeader);

	// ReSets the default Message strings from resource IDs
	BOOL	bReSetMessageStrings(void);

protected:
	// saves the current text selection and moves insertion point to the end
	// returns TRUE if selection needs to be restored
//	BOOL	bSaveAndMoveSel(CHARRANGE *pcr);

	// Makes sure the given length of text can be added to text view buffer
	BOOL	bCanAdd2Buffer(DWORD dwMsgLen);

	// Registers the text links for URLS in current text
	void	RegisterTextLinks(int iStart, LPINT nUrlBounds, INT nUrlNum);

	// Sets the indent for the text
	BOOL	bSetIndent(LONG lIndent=DEFAULT_INDENT);

	// Sets the default properties for all message types.
	BOOL	bSetDefaultMsgTypeProperties();

	// Delete any allocated Msg Type Properties
	void	ClearDefaultMsgTypeProperties(void);

	// Delete any allocated Message Strings
	void	ClearMessageStrings(void);

	void	ZeroMessageStrings(void);

	BOOL	bSetString(LPTSTR *pszDest, USHORT *pnLen, LPCTSTR szSrc, DWORD dwLen);
private:
	HWND			m_hwnd;				// handle to history window
	HINSTANCE		m_hInstance;		// handle to the current instance, to load resources

#ifdef STRICT
	WNDPROC			m_lpfnProcTV;
#else
	FARPROC			m_lpfnProcTV;
#endif STRICT
	BOOL			m_bThumb;

	DWORD			m_dwBuffSize;		// current length of text buffer
	DWORD			m_dwBuffMaxSize;	// max text buffer length, 0 if no limit set.

	FLOAT			m_fBuffCutOff;		// Min amount of Buffer to be cutoff
	FLOAT			m_fBuffFull;		// Buffer Full notification limit

    HINSTANCE		m_hInstRichEd32;	// handle for rich edit DLL
	HCURSOR			m_hCursorHand;		// Hand cursor for display on top of URLs

	CUrlRec			m_urlrec;			// For URL recognition and handling

	CHARFORMAT		m_cfFont;			// default font name and size

	short			m_nInsertBlank;		// Flag for inserting blank lines
	short			m_nAutoScroll;		// Flag for AutoScroll

	BOOL			m_bNewBrowser;		// Detrmine where to launch URLs
	// TODO: Optimize this logic
	BOOL			m_bHeader;			// State Flag to see if the text message is preceeded by Header
	BOOL			m_bCallHeader;		// State Flag to know when we are printing Header
	BOOL			m_bHeaderSeparate;	// State Flag to know if header and text are on separate lines
	BOOL			m_bDBCSSystem;		// Are we on a DBCS system?

	MSG_TYPE		m_mtLastMsgType;	// latest entry type

	MSG_TYPE_PROP	*m_pMsgTypePropHead[msEndEnum][mtBeginActions];	// Message type Specific Properties	

	MSG_TYPE_PROP	*m_pMsgTypePropText[msEndEnum][mtBeginInfo];	// Message type Specific Properties	

	MSG_TYPE_PROP	*m_pMsgTypePropInfo[mtEndEnum-mtBeginInfo];		// Message type Specific Properties	

	// Make sure the default Resource ID's are Shorts
//	USHORT			m_rguResMsgType[mtEndEnum];		// Resource IDs for the message headers
//	USHORT			m_rguResStatus[msEndEnum];		// Resource IDs for the status headers "Host ..."

	LPTSTR			m_szMsgType[mtEndEnum];			// Resource strings for the message headers	
	LPTSTR			m_szMembStatus[msEndEnum];		// Resource strings for the status headers "Host ..."
	USHORT			m_nMsgTypeLen[mtEndEnum];
	USHORT			m_nMembStatusLen[msEndEnum];

	MSG_TYPE_PROP	m_URLMsgTypeProp;
	MSG_TYPE_PROP	m_HeadMsgTypeProp;
	MSG_TYPE_PROP	m_TextMsgTypeProp;
	MSG_TYPE_PROP	m_InfoMsgTypeProp;

//	int		m_nLogPixelsY;			// For converting twips to points.

};


#define __TEXTVIEW_H__
#endif __TEXTVIEW_H__
