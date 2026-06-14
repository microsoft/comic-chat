// ui.cpp : implementation of the CUI class
//

#include "stdafx.h"
#include "chat.h"
#include <io.h>

#include "mainfrm.h"
#include "ui.h"

#include "chatprot.h"

/////////////////////////////////////////////////////////////////////////////
// Convenience macros

#define pMainFrame	((CMainFrame *) theApp.m_pMainWnd)

/////////////////////////////////////////////////////////////////////////////
// Static data

CUI cui;		// object constructed statically

/////////////////////////////////////////////////////////////////////////////
// External statics

extern CChatApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CUI operations

void 
CUI::Log( CString& str, int eLogOptions, COLORREF crTextColor)
{
//	GetLog()->Log( str, eLogOptions, crTextColor ); -djk (get rid of for now)
}

void 
CUI::Say( CString& str )
{
	ChatSendText(str, SM_SAY);
}

void
CUI::Think( CString& str )
{
	ChatSendText(str, SM_THINK);
}

void
CUI::Emote( CString& str )
{
	ChatSendText(str, SM_WHISPER);
}

void
CUI::Action( CString & str )
{
	ChatSendText(str, SM_ACTION);
}

void
CUI::PreSay(CString& str) {
	ChatPreSendText(str);
}

void 
CUI::DoVerb( CString& strVerb )
{
	// send to server
	Say( strVerb );
}

void 
CUI::Error( UINT nID )
{
	CString strMessage;

	strMessage.LoadString(nID);

	Error( strMessage );
}

void
CUI::Error( CString strMessage )
{
	CString strError;

	strError.LoadString(IDS_ERROR);

	pMainFrame->MessageBox( strMessage, strError, MB_OK | MB_ICONSTOP | MB_TASKMODAL );
}

void 
CUI::Warning( UINT nID )
{
	CString strMessage;

	strMessage.LoadString(nID);

	Warning( strMessage );
}

void 
CUI::Warning( CString strMessage )
{
	CString strWarning;

	strWarning.LoadString(IDS_WARNING);

	pMainFrame->MessageBox( strMessage, strWarning, MB_OK | MB_ICONINFORMATION | MB_TASKMODAL );
}

BOOL CUI::FindFileInPath(	const char *	pszFile,
							CString 		strPath,
							CString& 		strFileFound )
{
	int ich;
	CString strOnePath;
	CString strFullName;

	strFileFound.Empty();
	while( !strPath.IsEmpty() )
	{
		// find ;
		ich = strPath.Find(';');

		// if not found, set index to end
		if( ich == -1 )
			ich = strPath.GetLength();

		// grab up to there
		strOnePath = strPath.Left(ich);

		// yank
		if( ich < strPath.GetLength() )
			strPath = strPath.Mid(ich+1);
		else
			strPath.Empty();

		// compose
		strFullName = strOnePath;
		strFullName += '\\';
		strFullName += pszFile;

		// check if the file exists
		if( _access( strFullName, 0 ) == 0 )
		{
			// yep
			strFileFound = strFullName;
			return TRUE;
		}

		// nope, keep going

	}

	return FALSE;
}


		


