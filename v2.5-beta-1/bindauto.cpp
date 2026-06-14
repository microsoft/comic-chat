/*
	Microsoft Corp. (C) Copyright 1997
	Developed under contract by Numbers & Co.
----------------------------------------------------------------------------

        name:   CChat - ActiveX automation interface

	 	file:	bindauto.cpp

    comments:	Implements CChatDoc IDispatch interface
     	
----------------------------------------------------------------------------
	Microsoft Corp. (C) Copyright 1997
	Developed under contract by Numbers & Co.
 */


#include "stdafx.h"
#include <afxdisp.h>
#include "binddoc.h"
#include "bindipfw.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif


#define DEFINE_SIMPLE_DISPATCHFUNC( name )							\
STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::name( void )	\
	{																\
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)		\
	ASSERT_VALID(pThis);											\
																	\
	return( pThis->name() );										\
																	\
	}






#define DEFINE_SIMPLE_DSPFUNC( name, menuid ) \
HRESULT CDocObjectServerDoc::				  \
	name( void )							  \
	{										  \
											  \
	DoMenuCommand( menuid );				  \
											  \
	return( S_OK );							  \
											  \
	}



#define DEFINE_SIMPLE_MAPENTRY( name, nseq )	\
	DISP_FUNCTION_ID( CDocObjectServerDoc, "#name", 0x6002##nseq, name,  VT_EMPTY, VTS_NONE )











BEGIN_DISPATCH_MAP( CDocObjectServerDoc, COleServerDoc )
	DEFINE_SIMPLE_MAPENTRY( SessionConnect,   0000 )	
	DEFINE_SIMPLE_MAPENTRY( Open,			  0001 )	
	DEFINE_SIMPLE_MAPENTRY( Close,			  0002 )	
	DEFINE_SIMPLE_MAPENTRY( Save,			  0003 )	
	DEFINE_SIMPLE_MAPENTRY( SaveAs,			  0004 )	
	DEFINE_SIMPLE_MAPENTRY( Createshortcut,	  0005 )	
	DEFINE_SIMPLE_MAPENTRY( Print,			  0006 )	
	DEFINE_SIMPLE_MAPENTRY( PrintSetup,		  0007 )	
	DEFINE_SIMPLE_MAPENTRY( About,			  0008 )	
	DEFINE_SIMPLE_MAPENTRY( Undo,			  0009 )	
	DEFINE_SIMPLE_MAPENTRY( Cut,			  000a )	 
	DEFINE_SIMPLE_MAPENTRY( Copy,			  000b )	
	DEFINE_SIMPLE_MAPENTRY( Paste,			  000c )	
	DEFINE_SIMPLE_MAPENTRY( Delete,			  000d )	
	DEFINE_SIMPLE_MAPENTRY( SelectAll,		  000e )	
	DEFINE_SIMPLE_MAPENTRY( ViewToolbar,	  000f )	 
	DEFINE_SIMPLE_MAPENTRY( ViewTabbar,		  0010 )	
	DEFINE_SIMPLE_MAPENTRY( ViewStatusbar,	  0011 )	
	DEFINE_SIMPLE_MAPENTRY( ViewComics,		  0012 )	
	DEFINE_SIMPLE_MAPENTRY( ViewText,		  0013 )	
	DEFINE_SIMPLE_MAPENTRY( ViewList,		  0014 )	
	DEFINE_SIMPLE_MAPENTRY( ViewIcon,		  0015 )	
	DEFINE_SIMPLE_MAPENTRY( Motd,			  0016 )	
	DEFINE_SIMPLE_MAPENTRY( Options,		  0017 )	 
	DEFINE_SIMPLE_MAPENTRY( SetColor,		  0018 )	
	DEFINE_SIMPLE_MAPENTRY( Bold,			  0019 )	
	DEFINE_SIMPLE_MAPENTRY( Italic,			  001a )	
	DEFINE_SIMPLE_MAPENTRY( Underlined,		  001b )	
	DEFINE_SIMPLE_MAPENTRY( FixedPitch,		  001c )	
	DEFINE_SIMPLE_MAPENTRY( Symbol,			  001d )	
	DEFINE_SIMPLE_MAPENTRY( NewRoom,		  001e )	 
	DEFINE_SIMPLE_MAPENTRY( Leave,			  001f )	
	DEFINE_SIMPLE_MAPENTRY( CreateRoom,		  0020 )	
	DEFINE_SIMPLE_MAPENTRY( ChatroomList,	  0021 )	
	DEFINE_SIMPLE_MAPENTRY( Channelprops,	  0022 )	
	DEFINE_SIMPLE_MAPENTRY( Disconnect,		  0023 )	
	DEFINE_SIMPLE_MAPENTRY( UserList,		  0024 )	
	DEFINE_SIMPLE_MAPENTRY( Invite,			  0025 )	
	DEFINE_SIMPLE_MAPENTRY( Away,			  0026 )	
	DEFINE_SIMPLE_MAPENTRY( MemberGetinfo,	  0027 )	
	DEFINE_SIMPLE_MAPENTRY( Getidentity,	  0028 )	 
	DEFINE_SIMPLE_MAPENTRY( WhisperboxMlist,  0029 )	 
	DEFINE_SIMPLE_MAPENTRY( MemberIgnore,	  002a )	
	DEFINE_SIMPLE_MAPENTRY( SendEmail,		  002b )	
	DEFINE_SIMPLE_MAPENTRY( SendFile,		  002c )	
	DEFINE_SIMPLE_MAPENTRY( Homepage,		  002d )	
	DEFINE_SIMPLE_MAPENTRY( Netmeeting,		  002e )	
	DEFINE_SIMPLE_MAPENTRY( Version,		  002f )	 
	DEFINE_SIMPLE_MAPENTRY( PingUser,		  0030 )	
	DEFINE_SIMPLE_MAPENTRY( Localtime,		  0031 )	
	DEFINE_SIMPLE_MAPENTRY( AddToFavorites,	  0032 )	
	DEFINE_SIMPLE_MAPENTRY( OpenFavorites,	  0033 )	
	DEFINE_SIMPLE_MAPENTRY( Cascade,		  0034 )	 
	DEFINE_SIMPLE_MAPENTRY( Tile,			  0035 )	
	DEFINE_SIMPLE_MAPENTRY( Arrange,		  0036 )	 
	DEFINE_SIMPLE_MAPENTRY( HelpTopics,		  0037 )	
	DEFINE_SIMPLE_MAPENTRY( Freestuff,		  0038 )	
	DEFINE_SIMPLE_MAPENTRY( Productnews,	  0039 )	 
	DEFINE_SIMPLE_MAPENTRY( FAQ,			  003a )	 
	DEFINE_SIMPLE_MAPENTRY( OnlineSupport,	  003b )	
	DEFINE_SIMPLE_MAPENTRY( BestofWeb,		  003c )	
	DEFINE_SIMPLE_MAPENTRY( SearchtheWeb,	  003d )	
	DEFINE_SIMPLE_MAPENTRY( MsHomepage,		  003e )	
	DEFINE_SIMPLE_MAPENTRY( ReleaseNotes,	  003f )	
	DISP_FUNCTION_ID( CDocObjectServerDoc, 
					  "ShowMenu", 
					  0x60020040,
					  ShowMenu, 
					  VT_EMPTY, 
					  VTS_NONE )
	DISP_FUNCTION_ID( CDocObjectServerDoc, 
					  "HideMenu", 
					  0x60020041,
					  HideMenu, 
					  VT_EMPTY, 
					  VTS_NONE )
	DEFINE_SIMPLE_MAPENTRY( Automations,	  0042 )	

// example for how to do a general method - don't delete this yet
//
//	DISP_FUNCTION_ID( CDocObjectServerDoc, 
//					  "CChatPopupMessage", 
//					  0x60020041,
//					  CChatPopupMessage, 
//					  VT_I4, 
//					  VTS_PBOOL VTS_PI4 VTS_PI4 VTS_PI4 VTS_PI4)

END_DISPATCH_MAP()								
												
												
												
												

IMPLEMENT_OLETYPELIB( CDocObjectServerDoc, LIBID_iCChatLib, 1, 0 )















BOOL 
	CDocObjectServerDoc::GetDispatchIID(IID* pIID)
	{
	
	*pIID = IID_ICChatAutomation;

	return( TRUE );

	}





STDMETHODIMP_(ULONG) CDocObjectServerDoc::XCChatAutomation::AddRef()
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	return pThis->ExternalAddRef();
	}

STDMETHODIMP_(ULONG) CDocObjectServerDoc::XCChatAutomation::Release()
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	return pThis->ExternalRelease();
	}

STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::
	QueryInterface( REFIID iid, LPVOID* ppvObj )
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	return pThis->ExternalQueryInterface(&iid, ppvObj);
	}





STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::
	GetTypeInfoCount( UINT *pctinfo )
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	ASSERT_VALID(pThis);

    LPDISPATCH lpDispatch = (LPDISPATCH)&(pThis->m_xDispatch);
    ASSERT(lpDispatch != NULL);

	return lpDispatch->GetTypeInfoCount( pctinfo );
	}



        
STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::
	GetTypeInfo( UINT iTInfo, LCID lcid, ITypeInfo **ppTInfo )
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	ASSERT_VALID(pThis);

    LPDISPATCH lpDispatch = (LPDISPATCH)&(pThis->m_xDispatch);
    ASSERT(lpDispatch != NULL);

	return lpDispatch->GetTypeInfo( iTInfo, lcid, ppTInfo );
	}



        
        
STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::
	GetIDsOfNames( REFIID riid, LPOLESTR *rgszNames, UINT cNames, LCID lcid, DISPID *rgDispId )
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	ASSERT_VALID(pThis);

    LPDISPATCH lpDispatch = (LPDISPATCH)&(pThis->m_xDispatch);
    ASSERT(lpDispatch != NULL);

	return lpDispatch->GetIDsOfNames( riid, rgszNames, cNames, lcid, rgDispId );
	}



        
        
STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::
	Invoke( DISPID dispIdMember, REFIID riid, LCID lcid, WORD wFlags, DISPPARAMS *pDispParams,
            VARIANT *pVarResult, EXCEPINFO *pExcepInfo, UINT *puArgErr )
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	ASSERT_VALID(pThis);

    LPDISPATCH lpDispatch = (LPDISPATCH)&(pThis->m_xDispatch);
    ASSERT(lpDispatch != NULL);

	return lpDispatch->Invoke( dispIdMember, riid, lcid, wFlags, pDispParams, pVarResult, pExcepInfo, puArgErr );
	}



        



// example for how to do a general method - don't delete this yet
//
// DUAL - VTBL -- CChatPopupMessage
//STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::
//	CChatPopupMessage( VARIANT_BOOL* pbool, long *p1, long *p2, long *ret1, long *ret2 )
//	{
//	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
//	ASSERT_VALID(pThis);
// 
//	return( pThis->CChatPopupMessage( pbool, p1, p2, ret1, ret2 ) );
//
//	}


// DUAL - DSPINTERFACE -- CChatPopupMessage
//HRESULT CDocObjectServerDoc::
//	CChatPopupMessage( VARIANT_BOOL* pbool, long *p1, long *p2, long *ret1, long *ret2 )
//	{
//	TCHAR msg[256];
//
//	if( *pbool == -1 )
//		{
//		wsprintf( msg, "Hi there from CChat!!!, p1 = %d, p2 = %d", *p1, *p2 );
//		::MessageBox( NULL, msg, "CChatPopupMessage", MB_OK );
//		}
//
//	*ret1 = *p1 - *p2;
//
//	if( ret2 != NULL )
//		*ret2 = *p1 + *p2;
//
//	return( S_OK );
//	}



// DUAL - VTBL -- ShowMenu
STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::
	ShowMenu( void )
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	ASSERT_VALID(pThis);
 
	return( pThis->ShowMenu() );

	}


// DUAL - DSPINTERFACE -- ShowMenu
HRESULT CDocObjectServerDoc::
	ShowMenu( void )
	{

	RestoreMenu();

	return( S_OK );
	}





// DUAL - VTBL -- HideMenu
STDMETHODIMP CDocObjectServerDoc::XCChatAutomation::
	HideMenu( void )
	{
	METHOD_PROLOGUE_EX(CDocObjectServerDoc, CChatAutomation)
	ASSERT_VALID(pThis);
 
	return( pThis->HideMenu() );

	}


// DUAL - DSPINTERFACE -- HideMenu
HRESULT CDocObjectServerDoc::
	HideMenu( void )
	{

	CDocObjectIPFrameWnd* pFrameWnd = (CDocObjectIPFrameWnd*)m_pInPlaceFrame;
	pFrameWnd->DestroySharedMenu();

	return( S_OK );
	}





// DUAL - VTBL -- All menu commands
DEFINE_SIMPLE_DISPATCHFUNC( SessionConnect	)
DEFINE_SIMPLE_DISPATCHFUNC( Open			)
DEFINE_SIMPLE_DISPATCHFUNC( Close			)
DEFINE_SIMPLE_DISPATCHFUNC( Save			)
DEFINE_SIMPLE_DISPATCHFUNC( SaveAs			)
DEFINE_SIMPLE_DISPATCHFUNC( Createshortcut	)
DEFINE_SIMPLE_DISPATCHFUNC( Print			)
DEFINE_SIMPLE_DISPATCHFUNC( PrintSetup		)
DEFINE_SIMPLE_DISPATCHFUNC( About			)
DEFINE_SIMPLE_DISPATCHFUNC( Undo			)
DEFINE_SIMPLE_DISPATCHFUNC( Cut				)
DEFINE_SIMPLE_DISPATCHFUNC( Copy			)
DEFINE_SIMPLE_DISPATCHFUNC( Paste			)
DEFINE_SIMPLE_DISPATCHFUNC( Delete			)
DEFINE_SIMPLE_DISPATCHFUNC( SelectAll		)
DEFINE_SIMPLE_DISPATCHFUNC( ViewToolbar		)
DEFINE_SIMPLE_DISPATCHFUNC( ViewTabbar		)
DEFINE_SIMPLE_DISPATCHFUNC( ViewStatusbar	)
DEFINE_SIMPLE_DISPATCHFUNC( ViewComics		)
DEFINE_SIMPLE_DISPATCHFUNC( ViewText		)
DEFINE_SIMPLE_DISPATCHFUNC( ViewList		)
DEFINE_SIMPLE_DISPATCHFUNC( ViewIcon		)
DEFINE_SIMPLE_DISPATCHFUNC( Motd			)
DEFINE_SIMPLE_DISPATCHFUNC( Options			)
DEFINE_SIMPLE_DISPATCHFUNC( SetColor		)
DEFINE_SIMPLE_DISPATCHFUNC( Bold			)
DEFINE_SIMPLE_DISPATCHFUNC( Italic			)
DEFINE_SIMPLE_DISPATCHFUNC( Underlined		)
DEFINE_SIMPLE_DISPATCHFUNC( FixedPitch		)
DEFINE_SIMPLE_DISPATCHFUNC( Symbol			)
DEFINE_SIMPLE_DISPATCHFUNC( NewRoom			)
DEFINE_SIMPLE_DISPATCHFUNC( Leave			)
DEFINE_SIMPLE_DISPATCHFUNC( CreateRoom		)
DEFINE_SIMPLE_DISPATCHFUNC( ChatroomList	)
DEFINE_SIMPLE_DISPATCHFUNC( Channelprops	)
DEFINE_SIMPLE_DISPATCHFUNC( Disconnect		)
DEFINE_SIMPLE_DISPATCHFUNC( UserList		)
DEFINE_SIMPLE_DISPATCHFUNC( Invite			)
DEFINE_SIMPLE_DISPATCHFUNC( Away			)
DEFINE_SIMPLE_DISPATCHFUNC( MemberGetinfo	)
DEFINE_SIMPLE_DISPATCHFUNC( Getidentity		)
DEFINE_SIMPLE_DISPATCHFUNC( WhisperboxMlist )
DEFINE_SIMPLE_DISPATCHFUNC( MemberIgnore	)
DEFINE_SIMPLE_DISPATCHFUNC( SendEmail		)
DEFINE_SIMPLE_DISPATCHFUNC( SendFile		)
DEFINE_SIMPLE_DISPATCHFUNC( Homepage		)
DEFINE_SIMPLE_DISPATCHFUNC( Netmeeting		)
DEFINE_SIMPLE_DISPATCHFUNC( Version			)
DEFINE_SIMPLE_DISPATCHFUNC( PingUser		)
DEFINE_SIMPLE_DISPATCHFUNC( Localtime		)
DEFINE_SIMPLE_DISPATCHFUNC( AddToFavorites	)
DEFINE_SIMPLE_DISPATCHFUNC( OpenFavorites	)
DEFINE_SIMPLE_DISPATCHFUNC( Cascade			)
DEFINE_SIMPLE_DISPATCHFUNC( Tile			)
DEFINE_SIMPLE_DISPATCHFUNC( Arrange			)
DEFINE_SIMPLE_DISPATCHFUNC( HelpTopics		)
DEFINE_SIMPLE_DISPATCHFUNC( Freestuff		)
DEFINE_SIMPLE_DISPATCHFUNC( Productnews		)
DEFINE_SIMPLE_DISPATCHFUNC( FAQ				)
DEFINE_SIMPLE_DISPATCHFUNC( OnlineSupport	)
DEFINE_SIMPLE_DISPATCHFUNC( BestofWeb		)
DEFINE_SIMPLE_DISPATCHFUNC( SearchtheWeb	)
DEFINE_SIMPLE_DISPATCHFUNC( MsHomepage		)
DEFINE_SIMPLE_DISPATCHFUNC( ReleaseNotes	)
DEFINE_SIMPLE_DISPATCHFUNC( Automations		)
											
											
											
											
											


// DUAL - DSPINTERFACE -- All menu commands
DEFINE_SIMPLE_DSPFUNC( SessionConnect, 	ID_SESSION_CONNECT			)
DEFINE_SIMPLE_DSPFUNC( Open,			ID_FILE_OPEN				)
DEFINE_SIMPLE_DSPFUNC( Close,			ID_FILE_CLOSE				)
DEFINE_SIMPLE_DSPFUNC( Save,			ID_FILE_SAVE				)
DEFINE_SIMPLE_DSPFUNC( SaveAs,			ID_FILE_SAVE_AS				)
DEFINE_SIMPLE_DSPFUNC( Createshortcut,	ID_FILE_CREATESHORTCUT		)
DEFINE_SIMPLE_DSPFUNC( Print,			ID_FILE_PRINT				)
DEFINE_SIMPLE_DSPFUNC( PrintSetup,		ID_FILE_PRINT_SETUP			)
DEFINE_SIMPLE_DSPFUNC( About,			ID_APP_ABOUT				)
DEFINE_SIMPLE_DSPFUNC( Undo,			ID_EDIT_UNDO				)
DEFINE_SIMPLE_DSPFUNC( Cut,				ID_EDIT_CUT					)
DEFINE_SIMPLE_DSPFUNC( Copy,			ID_EDIT_COPY				)
DEFINE_SIMPLE_DSPFUNC( Paste,			ID_EDIT_PASTE				)
DEFINE_SIMPLE_DSPFUNC( Delete,			ID_EDIT_DELETE				)
DEFINE_SIMPLE_DSPFUNC( SelectAll,		ID_EDIT_SELECTALL			)
DEFINE_SIMPLE_DSPFUNC( ViewToolbar,		ID_VIEW_TOOLBAR				)
DEFINE_SIMPLE_DSPFUNC( ViewTabbar,		ID_VIEW_TABBAR				)
DEFINE_SIMPLE_DSPFUNC( ViewStatusbar,	ID_VIEW_STATUS_BAR			)
DEFINE_SIMPLE_DSPFUNC( ViewComics,		ID_VIEW_COMICS				)
DEFINE_SIMPLE_DSPFUNC( ViewText,		ID_VIEW_TEXT				)
DEFINE_SIMPLE_DSPFUNC( ViewList,		ID_VIEW_LIST				)
DEFINE_SIMPLE_DSPFUNC( ViewIcon,		ID_VIEW_ICON				)
DEFINE_SIMPLE_DSPFUNC( Motd,			ID_MOTD						)
DEFINE_SIMPLE_DSPFUNC( Options,			ID_VIEW_OPTIONS				)
DEFINE_SIMPLE_DSPFUNC( SetColor,		ID_SETCOLOR					)
DEFINE_SIMPLE_DSPFUNC( Bold,			ID_SWITCHBOLD				)
DEFINE_SIMPLE_DSPFUNC( Italic,			ID_SWITCHITALIC				)
DEFINE_SIMPLE_DSPFUNC( Underlined,		ID_SWITCHUNDERLINED			)
DEFINE_SIMPLE_DSPFUNC( FixedPitch,		ID_SWITCHFIXEDPITCH			)
DEFINE_SIMPLE_DSPFUNC( Symbol,			ID_SWITCHSYMBOL				)
DEFINE_SIMPLE_DSPFUNC( NewRoom,			ID_SESSION_NEWROOM			)
DEFINE_SIMPLE_DSPFUNC( Leave,			ID_SESSION_LEAVE			)
DEFINE_SIMPLE_DSPFUNC( CreateRoom,		ID_ROOM_CREATEROOM			)
DEFINE_SIMPLE_DSPFUNC( ChatroomList,	ID_CHATROOM_LIST			)
DEFINE_SIMPLE_DSPFUNC( Channelprops,	ID_CHANNELPROPS				)
DEFINE_SIMPLE_DSPFUNC( Disconnect,		ID_SESSION_DISCONNECT		)
DEFINE_SIMPLE_DSPFUNC( UserList,		ID_USER_LIST				)
DEFINE_SIMPLE_DSPFUNC( Invite,			ID_INVITE					)
DEFINE_SIMPLE_DSPFUNC( Away,			ID_AWAY_TOGGLE				)
DEFINE_SIMPLE_DSPFUNC( MemberGetinfo,	ID_MEMBER_GETINFO			)
DEFINE_SIMPLE_DSPFUNC( Getidentity,		ID_GETIDENTITY				)
DEFINE_SIMPLE_DSPFUNC( WhisperboxMlist,	ID_WHISPERBOX_MLIST			)
DEFINE_SIMPLE_DSPFUNC( MemberIgnore,	ID_MEMBER_IGNORE			)
DEFINE_SIMPLE_DSPFUNC( SendEmail,		ID_SEND_EMAIL				)
DEFINE_SIMPLE_DSPFUNC( SendFile,		ID_SEND_FILE				)
DEFINE_SIMPLE_DSPFUNC( Homepage,		ID_VISIT_HOMEPAGE			)
DEFINE_SIMPLE_DSPFUNC( Netmeeting,		ID_START_NETMEETING			)
DEFINE_SIMPLE_DSPFUNC( Version,			ID_GET_VERSION				)
DEFINE_SIMPLE_DSPFUNC( PingUser,		ID_PING_USER				)
DEFINE_SIMPLE_DSPFUNC( Localtime,		ID_GET_LOCALTIME			)
DEFINE_SIMPLE_DSPFUNC( AddToFavorites,	ID_FAVORITES_ADDTOFAVORITES	)
DEFINE_SIMPLE_DSPFUNC( OpenFavorites,	ID_FAVORITES_OPENFAVORITES	)
DEFINE_SIMPLE_DSPFUNC( Cascade,			ID_WINDOW_CASCADE			)
DEFINE_SIMPLE_DSPFUNC( Tile,			ID_WINDOW_TILE_HORZ			)
DEFINE_SIMPLE_DSPFUNC( Arrange,			ID_WINDOW_ARRANGE			)
DEFINE_SIMPLE_DSPFUNC( HelpTopics,		ID_HELP_TOPICS				)
DEFINE_SIMPLE_DSPFUNC( Freestuff,		ID_HELP_FREESTUFF			)
DEFINE_SIMPLE_DSPFUNC( Productnews,		ID_HELP_PRODUCTNEWS			)
DEFINE_SIMPLE_DSPFUNC( FAQ,				ID_HELP_FAQ					)
DEFINE_SIMPLE_DSPFUNC( OnlineSupport,	ID_HELP_ONLINESUPPORT		)
DEFINE_SIMPLE_DSPFUNC( BestofWeb,		ID_HELP_BESTOFWEB			)
DEFINE_SIMPLE_DSPFUNC( SearchtheWeb,	ID_HELP_SEARCHTHEWEB		)
DEFINE_SIMPLE_DSPFUNC( MsHomepage,		ID_HELP_MSHOMEPAGE			)
DEFINE_SIMPLE_DSPFUNC( ReleaseNotes,	ID_HELP_RELEASENOTES		)
DEFINE_SIMPLE_DSPFUNC( Automations,		ID_VIEW_AUTOMATIONS			)
																	
																	
																	
																	
																	
																	
																	
void 
	CDocObjectServerDoc::DoMenuCommand( int nMenuID )
	{

	// Can't use AfxGetMainWnd() because we get the wrong window if we're embedded
	(AfxGetApp()->m_pMainWnd)->SendMessage( WM_COMMAND, nMenuID );

	}






