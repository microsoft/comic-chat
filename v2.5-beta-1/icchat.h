/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Mon Mar 16 14:19:37 1998
 */
/* Compiler settings for icchat.idl:
    Oicf (OptLev=i2), W1, Zp8, env=Win32, ms_ext, c_ext
    error checks: none
*/
//@@MIDL_FILE_HEADING(  )
#include "rpc.h"
#include "rpcndr.h"
#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __icchat_h__
#define __icchat_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICChatAutomation_FWD_DEFINED__
#define __ICChatAutomation_FWD_DEFINED__
typedef interface ICChatAutomation ICChatAutomation;
#endif 	/* __ICChatAutomation_FWD_DEFINED__ */


#ifndef __Document_FWD_DEFINED__
#define __Document_FWD_DEFINED__

#ifdef __cplusplus
typedef class Document Document;
#else
typedef struct Document Document;
#endif /* __cplusplus */

#endif 	/* __Document_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __ICChatAutomation_INTERFACE_DEFINED__
#define __ICChatAutomation_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICChatAutomation
 * at Mon Mar 16 14:19:37 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][oleautomation][object][uuid] */ 



EXTERN_C const IID IID_ICChatAutomation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("241af502-8fb6-11cf-adc5-00aa00badf6f")
    ICChatAutomation : public IDispatch
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE SessionConnect( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Open( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Close( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Save( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SaveAs( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Createshortcut( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Print( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PrintSetup( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE About( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Undo( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Cut( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Copy( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Paste( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Delete( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SelectAll( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ViewToolbar( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ViewTabbar( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ViewStatusbar( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ViewComics( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ViewText( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ViewList( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ViewIcon( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Motd( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Options( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetColor( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Bold( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Italic( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Underlined( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FixedPitch( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Symbol( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE NewRoom( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Leave( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE CreateRoom( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ChatroomList( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Channelprops( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Disconnect( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE UserList( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Invite( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Away( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MemberGetinfo( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Getidentity( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE WhisperboxMlist( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MemberIgnore( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendEmail( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendFile( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Homepage( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Netmeeting( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Version( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE PingUser( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Localtime( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE AddToFavorites( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OpenFavorites( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Cascade( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Tile( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Arrange( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HelpTopics( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Freestuff( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Productnews( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE FAQ( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnlineSupport( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE BestofWeb( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SearchtheWeb( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MsHomepage( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReleaseNotes( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ShowMenu( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE HideMenu( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE Automations( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICChatAutomationVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICChatAutomation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICChatAutomation __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            ICChatAutomation __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            ICChatAutomation __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            ICChatAutomation __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            ICChatAutomation __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SessionConnect )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Open )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Close )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Save )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SaveAs )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Createshortcut )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Print )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PrintSetup )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *About )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Undo )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cut )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Copy )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Paste )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Delete )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SelectAll )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewToolbar )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewTabbar )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewStatusbar )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewComics )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewText )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewList )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ViewIcon )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Motd )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Options )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetColor )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Bold )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Italic )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Underlined )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FixedPitch )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Symbol )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *NewRoom )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Leave )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *CreateRoom )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChatroomList )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Channelprops )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Disconnect )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *UserList )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invite )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Away )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MemberGetinfo )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Getidentity )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *WhisperboxMlist )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MemberIgnore )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendEmail )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendFile )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Homepage )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Netmeeting )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Version )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *PingUser )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Localtime )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *AddToFavorites )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OpenFavorites )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Cascade )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Tile )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Arrange )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HelpTopics )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Freestuff )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Productnews )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *FAQ )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnlineSupport )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *BestofWeb )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SearchtheWeb )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MsHomepage )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReleaseNotes )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ShowMenu )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *HideMenu )( 
            ICChatAutomation __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Automations )( 
            ICChatAutomation __RPC_FAR * This);
        
        END_INTERFACE
    } ICChatAutomationVtbl;

    interface ICChatAutomation
    {
        CONST_VTBL struct ICChatAutomationVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICChatAutomation_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICChatAutomation_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICChatAutomation_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICChatAutomation_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define ICChatAutomation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define ICChatAutomation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define ICChatAutomation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define ICChatAutomation_SessionConnect(This)	\
    (This)->lpVtbl -> SessionConnect(This)

#define ICChatAutomation_Open(This)	\
    (This)->lpVtbl -> Open(This)

#define ICChatAutomation_Close(This)	\
    (This)->lpVtbl -> Close(This)

#define ICChatAutomation_Save(This)	\
    (This)->lpVtbl -> Save(This)

#define ICChatAutomation_SaveAs(This)	\
    (This)->lpVtbl -> SaveAs(This)

#define ICChatAutomation_Createshortcut(This)	\
    (This)->lpVtbl -> Createshortcut(This)

#define ICChatAutomation_Print(This)	\
    (This)->lpVtbl -> Print(This)

#define ICChatAutomation_PrintSetup(This)	\
    (This)->lpVtbl -> PrintSetup(This)

#define ICChatAutomation_About(This)	\
    (This)->lpVtbl -> About(This)

#define ICChatAutomation_Undo(This)	\
    (This)->lpVtbl -> Undo(This)

#define ICChatAutomation_Cut(This)	\
    (This)->lpVtbl -> Cut(This)

#define ICChatAutomation_Copy(This)	\
    (This)->lpVtbl -> Copy(This)

#define ICChatAutomation_Paste(This)	\
    (This)->lpVtbl -> Paste(This)

#define ICChatAutomation_Delete(This)	\
    (This)->lpVtbl -> Delete(This)

#define ICChatAutomation_SelectAll(This)	\
    (This)->lpVtbl -> SelectAll(This)

#define ICChatAutomation_ViewToolbar(This)	\
    (This)->lpVtbl -> ViewToolbar(This)

#define ICChatAutomation_ViewTabbar(This)	\
    (This)->lpVtbl -> ViewTabbar(This)

#define ICChatAutomation_ViewStatusbar(This)	\
    (This)->lpVtbl -> ViewStatusbar(This)

#define ICChatAutomation_ViewComics(This)	\
    (This)->lpVtbl -> ViewComics(This)

#define ICChatAutomation_ViewText(This)	\
    (This)->lpVtbl -> ViewText(This)

#define ICChatAutomation_ViewList(This)	\
    (This)->lpVtbl -> ViewList(This)

#define ICChatAutomation_ViewIcon(This)	\
    (This)->lpVtbl -> ViewIcon(This)

#define ICChatAutomation_Motd(This)	\
    (This)->lpVtbl -> Motd(This)

#define ICChatAutomation_Options(This)	\
    (This)->lpVtbl -> Options(This)

#define ICChatAutomation_SetColor(This)	\
    (This)->lpVtbl -> SetColor(This)

#define ICChatAutomation_Bold(This)	\
    (This)->lpVtbl -> Bold(This)

#define ICChatAutomation_Italic(This)	\
    (This)->lpVtbl -> Italic(This)

#define ICChatAutomation_Underlined(This)	\
    (This)->lpVtbl -> Underlined(This)

#define ICChatAutomation_FixedPitch(This)	\
    (This)->lpVtbl -> FixedPitch(This)

#define ICChatAutomation_Symbol(This)	\
    (This)->lpVtbl -> Symbol(This)

#define ICChatAutomation_NewRoom(This)	\
    (This)->lpVtbl -> NewRoom(This)

#define ICChatAutomation_Leave(This)	\
    (This)->lpVtbl -> Leave(This)

#define ICChatAutomation_CreateRoom(This)	\
    (This)->lpVtbl -> CreateRoom(This)

#define ICChatAutomation_ChatroomList(This)	\
    (This)->lpVtbl -> ChatroomList(This)

#define ICChatAutomation_Channelprops(This)	\
    (This)->lpVtbl -> Channelprops(This)

#define ICChatAutomation_Disconnect(This)	\
    (This)->lpVtbl -> Disconnect(This)

#define ICChatAutomation_UserList(This)	\
    (This)->lpVtbl -> UserList(This)

#define ICChatAutomation_Invite(This)	\
    (This)->lpVtbl -> Invite(This)

#define ICChatAutomation_Away(This)	\
    (This)->lpVtbl -> Away(This)

#define ICChatAutomation_MemberGetinfo(This)	\
    (This)->lpVtbl -> MemberGetinfo(This)

#define ICChatAutomation_Getidentity(This)	\
    (This)->lpVtbl -> Getidentity(This)

#define ICChatAutomation_WhisperboxMlist(This)	\
    (This)->lpVtbl -> WhisperboxMlist(This)

#define ICChatAutomation_MemberIgnore(This)	\
    (This)->lpVtbl -> MemberIgnore(This)

#define ICChatAutomation_SendEmail(This)	\
    (This)->lpVtbl -> SendEmail(This)

#define ICChatAutomation_SendFile(This)	\
    (This)->lpVtbl -> SendFile(This)

#define ICChatAutomation_Homepage(This)	\
    (This)->lpVtbl -> Homepage(This)

#define ICChatAutomation_Netmeeting(This)	\
    (This)->lpVtbl -> Netmeeting(This)

#define ICChatAutomation_Version(This)	\
    (This)->lpVtbl -> Version(This)

#define ICChatAutomation_PingUser(This)	\
    (This)->lpVtbl -> PingUser(This)

#define ICChatAutomation_Localtime(This)	\
    (This)->lpVtbl -> Localtime(This)

#define ICChatAutomation_AddToFavorites(This)	\
    (This)->lpVtbl -> AddToFavorites(This)

#define ICChatAutomation_OpenFavorites(This)	\
    (This)->lpVtbl -> OpenFavorites(This)

#define ICChatAutomation_Cascade(This)	\
    (This)->lpVtbl -> Cascade(This)

#define ICChatAutomation_Tile(This)	\
    (This)->lpVtbl -> Tile(This)

#define ICChatAutomation_Arrange(This)	\
    (This)->lpVtbl -> Arrange(This)

#define ICChatAutomation_HelpTopics(This)	\
    (This)->lpVtbl -> HelpTopics(This)

#define ICChatAutomation_Freestuff(This)	\
    (This)->lpVtbl -> Freestuff(This)

#define ICChatAutomation_Productnews(This)	\
    (This)->lpVtbl -> Productnews(This)

#define ICChatAutomation_FAQ(This)	\
    (This)->lpVtbl -> FAQ(This)

#define ICChatAutomation_OnlineSupport(This)	\
    (This)->lpVtbl -> OnlineSupport(This)

#define ICChatAutomation_BestofWeb(This)	\
    (This)->lpVtbl -> BestofWeb(This)

#define ICChatAutomation_SearchtheWeb(This)	\
    (This)->lpVtbl -> SearchtheWeb(This)

#define ICChatAutomation_MsHomepage(This)	\
    (This)->lpVtbl -> MsHomepage(This)

#define ICChatAutomation_ReleaseNotes(This)	\
    (This)->lpVtbl -> ReleaseNotes(This)

#define ICChatAutomation_ShowMenu(This)	\
    (This)->lpVtbl -> ShowMenu(This)

#define ICChatAutomation_HideMenu(This)	\
    (This)->lpVtbl -> HideMenu(This)

#define ICChatAutomation_Automations(This)	\
    (This)->lpVtbl -> Automations(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICChatAutomation_SessionConnect_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_SessionConnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Open_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Open_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Close_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Close_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Save_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Save_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_SaveAs_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_SaveAs_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Createshortcut_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Createshortcut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Print_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Print_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_PrintSetup_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_PrintSetup_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_About_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_About_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Undo_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Undo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Cut_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Cut_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Copy_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Copy_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Paste_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Paste_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Delete_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Delete_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_SelectAll_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_SelectAll_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ViewToolbar_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ViewToolbar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ViewTabbar_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ViewTabbar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ViewStatusbar_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ViewStatusbar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ViewComics_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ViewComics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ViewText_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ViewText_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ViewList_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ViewList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ViewIcon_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ViewIcon_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Motd_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Motd_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Options_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Options_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_SetColor_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_SetColor_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Bold_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Bold_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Italic_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Italic_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Underlined_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Underlined_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_FixedPitch_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_FixedPitch_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Symbol_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Symbol_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_NewRoom_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_NewRoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Leave_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Leave_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_CreateRoom_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_CreateRoom_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ChatroomList_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ChatroomList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Channelprops_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Channelprops_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Disconnect_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Disconnect_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_UserList_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_UserList_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Invite_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Invite_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Away_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Away_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_MemberGetinfo_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_MemberGetinfo_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Getidentity_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Getidentity_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_WhisperboxMlist_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_WhisperboxMlist_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_MemberIgnore_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_MemberIgnore_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_SendEmail_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_SendEmail_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_SendFile_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_SendFile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Homepage_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Homepage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Netmeeting_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Netmeeting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Version_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Version_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_PingUser_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_PingUser_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Localtime_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Localtime_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_AddToFavorites_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_AddToFavorites_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_OpenFavorites_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_OpenFavorites_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Cascade_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Cascade_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Tile_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Tile_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Arrange_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Arrange_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_HelpTopics_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_HelpTopics_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Freestuff_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Freestuff_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Productnews_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Productnews_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_FAQ_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_FAQ_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_OnlineSupport_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_OnlineSupport_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_BestofWeb_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_BestofWeb_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_SearchtheWeb_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_SearchtheWeb_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_MsHomepage_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_MsHomepage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ReleaseNotes_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ReleaseNotes_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_ShowMenu_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_ShowMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_HideMenu_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_HideMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICChatAutomation_Automations_Proxy( 
    ICChatAutomation __RPC_FAR * This);


void __RPC_STUB ICChatAutomation_Automations_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICChatAutomation_INTERFACE_DEFINED__ */



#ifndef __iCChatLib_LIBRARY_DEFINED__
#define __iCChatLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: iCChatLib
 * at Mon Mar 16 14:19:37 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_iCChatLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_Document;

class DECLSPEC_UUID("241af500-8fb6-11cf-adc5-00aa00badf6f")
Document;
#endif
#endif /* __iCChatLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
