#ifdef CB32SUPPORT			// entire file should only be compiled if CB32SUPPORT is defined

#include "stdafx.h"
#include "chat.h"

#include "icbcore.h"		// ICb32Core interface


#include "userinfo.h"
#include "chatprot.h"
#include "nmproto.h"
#include "binddoc.h"
#include "chatdoc.h"
#include "resource.h"
#include "ui.h"
#include "histent.h"

#include <mbstring.h>

extern CChatApp theApp;
ICb32Core* CNmProto::m_pCbCore = NULL;
static CChatDoc *g_nmDoc = NULL;

// {8AA56C40-D120-11d0-A041-444553540000}
static const GUID CLSID_Testcore = 
{ 0x8aa56c40, 0xd120, 0x11d0, { 0xa0, 0x41, 0x44, 0x45, 0x53, 0x54, 0x0, 0x0 } };

#define CLSID_Testcore_TEXT  _T("{8AA56C40-D120-11d0-A041-444553540000}")




#define IS_PRIVATE_DATA( s ) ((*s == '\x01') || IS_CHANNEL_CHAR(*s))





BOOL CNmProto::Initialize() {
	HRESULT hr;

	// create cbcore object first so command line can be processed
	CoInitialize( NULL );

	if( FAILED( CoCreateInstance( CLSID_Cb32Core, 
								  NULL, 
								  CLSCTX_INPROC, 
								  IID_ICb32Core,
								  (void **)&m_pCbCore ))) {
		AfxMessageBox( IDS_CANTMAKECORE, MB_OK );
		return( FALSE );
	}

	hr = m_pCbCore->Init( (TCHAR *)(LPCTSTR)theApp.m_lpCmdLine );

	if( hr != S_OK ) {
		if( hr == NMCB_S_CLOSEAPP )
			return( FALSE ); // not an error, second instance handling...
		else {
			// error
			AfxMessageBox( IDS_CANTINITCORE, MB_OK );
			return( FALSE );
		}
	}
	
	return (TRUE);
}

void CNmProto::Uninitialize() {
	g_nmDoc = NULL;
	if( m_pCbCore != NULL ) {
		m_pCbCore->DeInit();
		m_pCbCore->Release();
	}

	CoUninitialize();
}

void CNmProto::GetNewProto() {
	// set up ICb32Core for chattin
	CCb32CoreNotify *pNotify = new CCb32CoreNotify(AfxGetMainWnd());
	if( pNotify == NULL )
		{
		// error
		AfxMessageBox( IDS_CANTMAKENOTIFY, MB_OK );
//		SendMessage( WM_CLOSE );
		}
	else
		{
		// give notify object to core
		if( FAILED( m_pCbCore->SetNotify( (IUnknown *)pNotify ) ) )
			{
			// error
			AfxMessageBox( IDS_CANTCONNECT, MB_OK );
//			SendMessage( WM_CLOSE );
			}

		// we're connected, turn the thing on
		/***TEMPORARY*** dummy INmSysInfo object so StartChat will compile. Replace with a real**/
		/*************** one later															   **/
		INmSysInfo *pDummyInfo;
		/************************/
		if( FAILED( m_pCbCore->StartChat(AfxGetMainWnd()->GetSafeHwnd(), &pDummyInfo ) ) )
			{
			// error
			AfxMessageBox( IDS_CANTCHAT, MB_OK );
//			SendMessage( WM_CLOSE );

			/* pDummyInfo is guarenteed to be NULL here so don't worry about releasing it */
			}

		pDummyInfo->Release(); // release since we aren't going to use it
		}

	if( pNotify != NULL )
		pNotify->Release();
}

void GetNewNmProto() {
	CNmProto::GetNewProto();
}


void CNmProto::ChatPartChannel(CDocument *doc1, BOOL) {
	void GotPartChannel(CChatDoc *);
	CChatDoc *doc = (CChatDoc *)doc1;
	GotPartChannel(doc);
	SetConnectionStatus(CX_DISCONNECTED);
}

BOOL CNmProto::ChatSendToChannel(const char *szAnnotations, const char *szMessage, char *szNMText/*=NULL*/, BYTE byteMode /*=0*/) {
	SendMessageText((char*) szAnnotations, (char*) szMessage, NULL /*szAddressee*/, szNMText);
	return TRUE;
}

// HAVE CHATSENDPRIVMESG CALL SPECIAL SENDMESSAGE FOR THIS CLASS...
BOOL CNmProto::ChatSendPrivMesg(const char *szAddressee, const char *szAnnotations, const char *szMessage, char *szNMText /*=NULL*/, BOOL bAsNotice /*=FALSE*/, BYTE byteMode /*=0*/) {
	SendMessageText((char*) szAnnotations, (char*) szMessage, szAddressee, szNMText);
	return TRUE;
}

void CNmProto::SendMessageText(char *szAnnotations, char *szMessage, const char *szAddressee, char *szNMText /*= NULL*/) {
	INmMember *toInter = NULL;
	ULONG uBuffer;
	HRESULT hr;

	if (szAddressee) {
		CUserInfo *LookupPui(const char *, CChatDoc* = NULL);
		NmUserInfo *user = (NmUserInfo *) LookupPui(szAddressee);
		if (!user) return;  // not registered yet, so can't send to them.
		toInter = user->m_pMember;
	}


	if( szNMText != NULL )
		{
		// If we have stuff specifically for NM then we know we gots trouble (rich text
		// formating in szMessage). No need to check any of the private prefixes, just send 
		// szNMText by normal buffer and szMessage by private buffer.
		TRACE( "CNmProto::SendMessageText sending rich text to %s\n", 
				(szAddressee!=NULL)?szAddressee:"Everybody" );

		// put annotations back together with the message
		CString cstrMessage;
		if( szAnnotations != NULL )
			{
			cstrMessage = szAnnotations;
			cstrMessage += szMessage;
			szMessage = (char *)(LPCTSTR)cstrMessage;
			}

		uBuffer = lstrlen( szMessage ) + sizeof( TCHAR );

		hr = m_pCbCore->
			SendMessageData( toInter, 
							 lstrlen( szNMText ) + sizeof( TCHAR ), 
								(byte *)szNMText,								// Plain text just for NM
							 uBuffer, (byte *)szMessage );						// Rich text plus who knows 
																				//	what for CChat
		}
	else
	if( szAnnotations != NULL )
		{
		// message is annotated and not rich. Send annotation by hidden buffer and text by normal buffer so 
		// non comic chats (you know who you are) will get the message. 
		TRACE( "CNmProto::SendMessageText sending annotated text to %s\n", 
				(szAddressee!=NULL)?szAddressee:"Everybody" );

		uBuffer = lstrlen( szAnnotations ) + sizeof( TCHAR );

		hr = m_pCbCore->
			SendMessageData( toInter, 
							 lstrlen( szMessage ) + sizeof( TCHAR ), 
								(byte *)szMessage,						// Just text for NM
							 uBuffer, (byte *)szAnnotations );			// Annotation for CChat
		}
	else
	if( IS_PRIVATE_DATA( szMessage ) )
		{
		// private CChat data, sneak it past NM
		TRACE( "CNmProto::SendMessageText sending private data to %s\n", 
				(szAddressee!=NULL)?szAddressee:"Everybody" );

		uBuffer = lstrlen( szMessage ) + sizeof( TCHAR );
		hr = m_pCbCore->SendMessageData( toInter, 
									     0, NULL,								// Nuthin for NM
										 uBuffer, (byte *)(LPCTSTR)szMessage );	// Private for CChat
		}
	else
		{
		// normal message, use normal buffer
		TRACE( "CNmProto::SendMessageText sending message text to %s\n", 
				(szAddressee!=NULL)?szAddressee:"Everybody" );

		uBuffer = lstrlen( szMessage ) + sizeof( TCHAR );
		hr = m_pCbCore->SendMessageData( toInter, 
									     uBuffer, (byte *)(LPCTSTR)szMessage,	// Text for everybody
										 0, NULL );								// No secrets
		}

	switch( hr ) {
		case S_OK:
			break;

		case NMCB_S_CANTSENDYET:
			TRACE0( "CNmProto::SendMessageText **WARNING** NMCB_S_CANTSENDYET\n" );
			break;

		case NMCB_E_CANTWHISPER:
			TRACE0( "CNmProto::SendMessageText **WARNING** NMCB_E_CANTWHISPER\n" );
			break;

		case NMCB_E_CANTWHISPERNM1:
			TRACE0( "CNmProto::SendMessageText **WARNING** NMCB_E_CANTWHISPERNM1\n" );
			break;

		default:
			TRACE( "CNmProto::SendMessageText **WARNING** unknown HRESULT = %0x,\n", hr );
			break;
	}
}

void CNmProto::SetConnectionStatus(int iStat) {
	m_iStatus = iStat;
	if (iStat == CX_INCHANNEL) {
		CString leftMesg;
		leftMesg.LoadString(IDS_NMCONNECTED);
		((CChatDoc *)m_doc)->SaveConnectStatus(leftMesg);
	}
	UpdateStatus();
}


// NOTIFICATION CODE (Started out as cb32 notify.cpp -- developed under
//   contract by Numbers & Co.


CCb32CoreNotify::CCb32CoreNotify( CWnd *pUI)
	{

	TRACE0( "CCb32CoreNotify::CCb32CoreNotify\n" );

	ASSERT( pUI != NULL );

    m_pUI = pUI;

	// start out with ref count = 1
	m_uRef = 1;

	}/* CCb32CoreNotify::CCb32CoreNotify */










CCb32CoreNotify::~CCb32CoreNotify( void )
	{

	TRACE0( "CCb32CoreNotify::~CCb32CoreNotify\n" );

	}/* CCb32CoreNotify::~CCb32CoreNotify */




CString GetAddr(INmMember *pMember) {
	CString s;
	ULONG id;
	pMember->GetID(&id);
	s.Format("%lu", id);
	return s;
}



STDMETHODIMP 
	CCb32CoreNotify::QueryInterface( REFIID riid, void** ppvObj )
	{

	TRACE0( "CCb32CoreNotify::QueryInterface\n" );

	*ppvObj = NULL;
	if( riid == IID_ICb32CoreNotify )
		{
		TRACE0( "Returning ICb32CoreNotify interface\n" );
		*ppvObj = (ICb32CoreNotify *)this;
		}
	else
	if( riid == IID_IUnknown )
		{
		TRACE0( "Returning IUnknown interface\n" );
		*ppvObj = (IUnknown *)this;
		}
	else
		{
		// bad interface, punt
		TRACE0( "unknown interface\n" );
		return( E_NOINTERFACE );
		}

	this->AddRef();
	return( S_OK );

	}/* CCb32CoreNotify::QueryInterface */




STDMETHODIMP_(ULONG)
	CCb32CoreNotify::AddRef( void )
	{

	m_uRef++;

	return( m_uRef );
	
	}/* CCb32CoreNotify::AddRef */



STDMETHODIMP_(ULONG)
	CCb32CoreNotify::Release( void )
	{

	// we started out at 1, so we die at 1 (using 0 makes a lovely leak)
	if( m_uRef-- <= 1 ) 
		{
		TRACE0( "deleting CCb32CoreNotify object\n" );
		delete this;
		return( 0 );
		}
	else
		return( m_uRef );

	}/* CCb32CoreNotify::Release */



STDMETHODIMP 
	CCb32CoreNotify::ChatStatus( BOOL bInProgress )
	{

	TRACE0( "CCb32CoreNotify::ChatStatus\n" );

	if( bInProgress ) {
		// conference is operational
		BOOL bProcessAddChannel(const char *szChannelName, CRoomInfo *proto);
		CRoomInfo *proto = new CNmProto;
		cui.m_pvIrcProto = proto;		// used by some UI handlers in chat.cpp (ircproto is a misnomer)
		bProcessAddChannel("NetMeeting", proto);
		g_nmDoc = GetChatDoc();
	} else
		// conference is gone, zap entire roster
		if (g_nmDoc) GetDefaultProto()->ChatPartChannel(g_nmDoc, TRUE);

	return( S_OK );

	}/* CCb32CoreNotify::ChatStatus */



STDMETHODIMP 
	CCb32CoreNotify::ReceivedMessage( BOOL bEcho,
									  TCHAR *pszMemberName,
									  INmMember *pMember,
									  TCHAR *pszWhisperToName,
									  INmMember *pWhisperTo,
									  ULONG uBuffer,
									  byte *pBuffer, 
									  ULONG uCChatBuffer,
									  byte *pCChatBuffer,
									  HRESULT hr ) {

	TRACE0( "CCb32CoreNotify::ReceivedMessage\n" );

	if (!g_nmDoc) return(S_OK);  // closing up
	if (bEcho && pMember != pWhisperTo) return (S_OK);  // We've already shown this text via local echo (whisper to self OK though)
	if (!pMember) return(S_OK);	// We got a message from a non-existent member (e.g., we sent after leaving the NM conference)
	void OnTextMsg(CChatDoc *, char *nick, const char *id, char *mesg, BYTE msgType = MT_CHANNELSEND | MT_PRVMSG, CDWordArray *talkTos = NULL);
	CString nick = GetAddr(pMember);
	BYTE msgType = pszWhisperToName ? MT_PRIVATEMSG : MT_CHANNELSEND;
	msgType |= MT_PRVMSG;

	if( pCChatBuffer != NULL )
		{
		// if pCChatBuffer is just an annotation (no message) then append pBuffer to it
		CString cstrMessage;
		if( IS_ANNOTATED( (const char *)pCChatBuffer ) )
			{
			// look for end of annotation to see if there is a message past it
			TRACE( "CCb32CoreNotify::ReceivedMessage received annotation\n" );
			TCHAR *pstr = (TCHAR *)_mbsstr( (const unsigned char *)pCChatBuffer, 
											 (const unsigned char *)_T(")") );

			if( lstrlen( pstr ) <= 2 ) // includes the blank after the ")"
				{
				// no message past annotation, append pBuffer
				TRACE( "CCb32CoreNotify::ReceivedMessage appending NM text to annotation\n" );
				cstrMessage = pCChatBuffer;
				cstrMessage += pBuffer;
				pCChatBuffer = (byte *)(LPCTSTR)cstrMessage;
				}
			}

		TRACE( "CCb32CoreNotify::ReceivedMessage received private data/text from %s\n", pszMemberName );
		OnTextMsg(g_nmDoc, UnConst(nick), nick, UnConst((const char *)pCChatBuffer), msgType);
		}
	else
		{
		TRACE( "CCb32CoreNotify::ReceivedMessage received message text from %s\n", pszMemberName );
		OnTextMsg(g_nmDoc, UnConst(nick), nick, UnConst((const char *)pBuffer), msgType);
		}

	return( S_OK );

	}/* CCb32CoreNotify::ReceivedMessage */



   
STDMETHODIMP 
	CCb32CoreNotify::MemberJoinedConference( INmMember *pMember, HRESULT hr ) {

	TRACE0( "CCb32CoreNotify::MemberJoinedConference\n" );
	return( S_OK );

}/* CCb32CoreNotify::MemberJoinedConference */



STDMETHODIMP 
	CCb32CoreNotify::MemberLeftConference( INmMember *pMember )
	{

	TRACE0( "CCb32CoreNotify::MemberLeftConference\n" );
	return( S_OK );

	}/* CCb32CoreNotify::MemberLeftConference */



STDMETHODIMP 
	CCb32CoreNotify::MemberJoinedChat( INmMember *pMember )
	{
	if (!g_nmDoc) return (S_OK);

	TRACE0( "CCb32CoreNotify::MemberJoinedChat\n" );

	CString s = GetAddr(pMember);
	BSTR name;
	pMember->GetName(&name);

	NmUserInfo *nmUser = new NmUserInfo(pMember);

	void SetMyRealName(const char *);	// Show this real name for now
	if (nmUser->IsSelf()) SetMyRealName(nmUser->GetScreenName());

	ULONG ver;
	pMember->GetNmVersion(&ver);
//	nmUser->ComicUser(ver >= 3);

	AddAndExecute(new JoinEntry(nmUser, FALSE), g_nmDoc);

	return( S_OK );

	}/* CCb32CoreNotify::MemberJoinedChat */

NmUserInfo::NmUserInfo(INmMember *pMember) {
	m_pMember = pMember;
	pMember->AddRef();
	m_strName = GetAddr(pMember);

	BSTR fullName;
	pMember->GetName(&fullName);
	TCHAR *tcFullName = BSTR_to_TCHAR(fullName);
	SysFreeString(fullName);
	if (tcFullName) {
		m_fullName = (char *) tcFullName;
		delete [] tcFullName;
	}
}

NmUserInfo::~NmUserInfo() {
	m_pMember->Release();
}

TCHAR * NmUserInfo::BSTR_to_TCHAR( BSTR bs )
	/*
		Converts UNICODE bs BSTR to an equivalent TCHAR. A TCHAR
		buffer is alloc'd and returned. Caller must delete it when done.
		NULL is returned if failure
	 */
	{

	int nALength;
	TCHAR *pszTemp;
	int nBLen;
	int bTLen;


	if( bs == NULL )
		return( NULL );

	// convert it to street language
	nBLen = SysStringLen( bs );
	nALength = nBLen*2 + sizeof (TCHAR);
	pszTemp = new TCHAR[ nALength ];
	if( pszTemp == NULL )
		return( NULL );

	if( (bTLen = WideCharToMultiByte( CP_ACP, 0, 
									  bs, nBLen,
									  pszTemp, nALength,
									  NULL, NULL ))
		!= 0 )
		{
		*(pszTemp + bTLen) = TEXT('\0');
		return( pszTemp );
		}
	else
		{
		delete pszTemp;
		return( NULL );
		}

	}/* CChatClub::BSTR_to_TCHAR */

void CNmProto::OnIdle(LONG) {
	if (m_pCbCore) m_pCbCore->OnIdle();
}


STDMETHODIMP 
	CCb32CoreNotify::MemberLeftChat( INmMember *pMember ) {

	TRACE0( "CCb32CoreNotify::MemberLeftChat\n" );
	if (!g_nmDoc) return(S_OK);

	CString s = GetAddr(pMember);

	if (s != "0") // conference has shut down
		AddAndExecute(new PartEntry((const char *)s), g_nmDoc);

	return( S_OK );

}/* CCb32CoreNotify::MemberLeftChat */



STDMETHODIMP 
	CCb32CoreNotify::SpecialOps( DWORD dwSpecial )
	{

	TRACE0( "CCb32CoreNotify::SpecialOps\n" );

	switch( dwSpecial )
		{
		case CB32OP_CLOSEAPP:
			if (!theApp.SaveAllModified()) return (S_FALSE);

			g_nmDoc = NULL;		// may get MemberLeftChat's after this (shouldn't add to doc)
			theApp.HideApplication();
			theApp.CloseAllDocuments(FALSE);  // need to clean up pMembers before close!
			m_pUI->PostMessage( WM_CLOSE );
			break;


		case CB32OP_SHOWUI:
			if( m_pUI->IsIconic() )
				m_pUI->ShowWindow( SW_RESTORE );
			else
				m_pUI->SetForegroundWindow();

			m_pUI->SetFocus();

			break;


		default:
			break;
		}

	return( S_OK );

	}/* CCb32CoreNotify::SpecialOps */

#endif CB32SUPPORT