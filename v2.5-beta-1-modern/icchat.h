

/* this ALWAYS GENERATED file contains the definitions for the interfaces */


 /* File created by MIDL compiler version 8.01.0628 */
/* at Mon Jan 18 19:14:07 2038
 */
/* Compiler settings for base\icchat.idl:
    Oicf, W1, Zp8, env=Win32 (32b run), target_arch=X86 8.01.0628 
    protocol : dce , ms_ext, c_ext, robust
    error checks: allocation ref bounds_check enum stub_data 
    VC __declspec() decoration level: 
         __declspec(uuid()), __declspec(selectany), __declspec(novtable)
         DECLSPEC_UUID(), MIDL_INTERFACE()
*/
/* @@MIDL_FILE_HEADING(  ) */

#pragma warning( disable: 4049 )  /* more than 64k source lines */


/* verify that the <rpcndr.h> version is high enough to compile this file*/
#ifndef __REQUIRED_RPCNDR_H_VERSION__
#define __REQUIRED_RPCNDR_H_VERSION__ 475
#endif

#include "rpc.h"
#include "rpcndr.h"

#ifndef __RPCNDR_H_VERSION__
#error this stub requires an updated version of <rpcndr.h>
#endif /* __RPCNDR_H_VERSION__ */

#ifndef COM_NO_WINDOWS_H
#include "windows.h"
#include "ole2.h"
#endif /*COM_NO_WINDOWS_H*/

#ifndef __icchat_h__
#define __icchat_h__

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
#pragma once
#endif

#ifndef DECLSPEC_XFGVIRT
#if defined(_CONTROL_FLOW_GUARD_XFG)
#define DECLSPEC_XFGVIRT(base, func) __declspec(xfg_virtual(base, func))
#else
#define DECLSPEC_XFGVIRT(base, func)
#endif
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

#ifdef __cplusplus
extern "C"{
#endif 


#ifndef __ICChatAutomation_INTERFACE_DEFINED__
#define __ICChatAutomation_INTERFACE_DEFINED__

/* interface ICChatAutomation */
/* [unique][helpstring][dual][oleautomation][object][uuid] */ 


EXTERN_C const IID IID_ICChatAutomation;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    MIDL_INTERFACE("241af502-8fb6-11cf-adc5-00aa00badf6f")
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
        
        DECLSPEC_XFGVIRT(IUnknown, QueryInterface)
        HRESULT ( STDMETHODCALLTYPE *QueryInterface )( 
            ICChatAutomation * This,
            /* [in] */ REFIID riid,
            /* [annotation][iid_is][out] */ 
            _COM_Outptr_  void **ppvObject);
        
        DECLSPEC_XFGVIRT(IUnknown, AddRef)
        ULONG ( STDMETHODCALLTYPE *AddRef )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(IUnknown, Release)
        ULONG ( STDMETHODCALLTYPE *Release )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfoCount)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfoCount )( 
            ICChatAutomation * This,
            /* [out] */ UINT *pctinfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetTypeInfo)
        HRESULT ( STDMETHODCALLTYPE *GetTypeInfo )( 
            ICChatAutomation * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo **ppTInfo);
        
        DECLSPEC_XFGVIRT(IDispatch, GetIDsOfNames)
        HRESULT ( STDMETHODCALLTYPE *GetIDsOfNames )( 
            ICChatAutomation * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR *rgszNames,
            /* [range][in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID *rgDispId);
        
        DECLSPEC_XFGVIRT(IDispatch, Invoke)
        /* [local] */ HRESULT ( STDMETHODCALLTYPE *Invoke )( 
            ICChatAutomation * This,
            /* [annotation][in] */ 
            _In_  DISPID dispIdMember,
            /* [annotation][in] */ 
            _In_  REFIID riid,
            /* [annotation][in] */ 
            _In_  LCID lcid,
            /* [annotation][in] */ 
            _In_  WORD wFlags,
            /* [annotation][out][in] */ 
            _In_  DISPPARAMS *pDispParams,
            /* [annotation][out] */ 
            _Out_opt_  VARIANT *pVarResult,
            /* [annotation][out] */ 
            _Out_opt_  EXCEPINFO *pExcepInfo,
            /* [annotation][out] */ 
            _Out_opt_  UINT *puArgErr);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, SessionConnect)
        HRESULT ( STDMETHODCALLTYPE *SessionConnect )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Open)
        HRESULT ( STDMETHODCALLTYPE *Open )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Close)
        HRESULT ( STDMETHODCALLTYPE *Close )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Save)
        HRESULT ( STDMETHODCALLTYPE *Save )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, SaveAs)
        HRESULT ( STDMETHODCALLTYPE *SaveAs )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Createshortcut)
        HRESULT ( STDMETHODCALLTYPE *Createshortcut )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Print)
        HRESULT ( STDMETHODCALLTYPE *Print )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, PrintSetup)
        HRESULT ( STDMETHODCALLTYPE *PrintSetup )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, About)
        HRESULT ( STDMETHODCALLTYPE *About )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Undo)
        HRESULT ( STDMETHODCALLTYPE *Undo )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Cut)
        HRESULT ( STDMETHODCALLTYPE *Cut )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Copy)
        HRESULT ( STDMETHODCALLTYPE *Copy )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Paste)
        HRESULT ( STDMETHODCALLTYPE *Paste )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Delete)
        HRESULT ( STDMETHODCALLTYPE *Delete )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, SelectAll)
        HRESULT ( STDMETHODCALLTYPE *SelectAll )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ViewToolbar)
        HRESULT ( STDMETHODCALLTYPE *ViewToolbar )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ViewTabbar)
        HRESULT ( STDMETHODCALLTYPE *ViewTabbar )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ViewStatusbar)
        HRESULT ( STDMETHODCALLTYPE *ViewStatusbar )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ViewComics)
        HRESULT ( STDMETHODCALLTYPE *ViewComics )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ViewText)
        HRESULT ( STDMETHODCALLTYPE *ViewText )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ViewList)
        HRESULT ( STDMETHODCALLTYPE *ViewList )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ViewIcon)
        HRESULT ( STDMETHODCALLTYPE *ViewIcon )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Motd)
        HRESULT ( STDMETHODCALLTYPE *Motd )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Options)
        HRESULT ( STDMETHODCALLTYPE *Options )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, SetColor)
        HRESULT ( STDMETHODCALLTYPE *SetColor )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Bold)
        HRESULT ( STDMETHODCALLTYPE *Bold )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Italic)
        HRESULT ( STDMETHODCALLTYPE *Italic )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Underlined)
        HRESULT ( STDMETHODCALLTYPE *Underlined )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, FixedPitch)
        HRESULT ( STDMETHODCALLTYPE *FixedPitch )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Symbol)
        HRESULT ( STDMETHODCALLTYPE *Symbol )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, NewRoom)
        HRESULT ( STDMETHODCALLTYPE *NewRoom )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Leave)
        HRESULT ( STDMETHODCALLTYPE *Leave )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, CreateRoom)
        HRESULT ( STDMETHODCALLTYPE *CreateRoom )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ChatroomList)
        HRESULT ( STDMETHODCALLTYPE *ChatroomList )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Channelprops)
        HRESULT ( STDMETHODCALLTYPE *Channelprops )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Disconnect)
        HRESULT ( STDMETHODCALLTYPE *Disconnect )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, UserList)
        HRESULT ( STDMETHODCALLTYPE *UserList )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Invite)
        HRESULT ( STDMETHODCALLTYPE *Invite )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Away)
        HRESULT ( STDMETHODCALLTYPE *Away )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, MemberGetinfo)
        HRESULT ( STDMETHODCALLTYPE *MemberGetinfo )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Getidentity)
        HRESULT ( STDMETHODCALLTYPE *Getidentity )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, WhisperboxMlist)
        HRESULT ( STDMETHODCALLTYPE *WhisperboxMlist )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, MemberIgnore)
        HRESULT ( STDMETHODCALLTYPE *MemberIgnore )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, SendEmail)
        HRESULT ( STDMETHODCALLTYPE *SendEmail )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, SendFile)
        HRESULT ( STDMETHODCALLTYPE *SendFile )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Homepage)
        HRESULT ( STDMETHODCALLTYPE *Homepage )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Netmeeting)
        HRESULT ( STDMETHODCALLTYPE *Netmeeting )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Version)
        HRESULT ( STDMETHODCALLTYPE *Version )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, PingUser)
        HRESULT ( STDMETHODCALLTYPE *PingUser )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Localtime)
        HRESULT ( STDMETHODCALLTYPE *Localtime )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, AddToFavorites)
        HRESULT ( STDMETHODCALLTYPE *AddToFavorites )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, OpenFavorites)
        HRESULT ( STDMETHODCALLTYPE *OpenFavorites )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Cascade)
        HRESULT ( STDMETHODCALLTYPE *Cascade )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Tile)
        HRESULT ( STDMETHODCALLTYPE *Tile )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Arrange)
        HRESULT ( STDMETHODCALLTYPE *Arrange )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, HelpTopics)
        HRESULT ( STDMETHODCALLTYPE *HelpTopics )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Freestuff)
        HRESULT ( STDMETHODCALLTYPE *Freestuff )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Productnews)
        HRESULT ( STDMETHODCALLTYPE *Productnews )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, FAQ)
        HRESULT ( STDMETHODCALLTYPE *FAQ )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, OnlineSupport)
        HRESULT ( STDMETHODCALLTYPE *OnlineSupport )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, BestofWeb)
        HRESULT ( STDMETHODCALLTYPE *BestofWeb )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, SearchtheWeb)
        HRESULT ( STDMETHODCALLTYPE *SearchtheWeb )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, MsHomepage)
        HRESULT ( STDMETHODCALLTYPE *MsHomepage )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ReleaseNotes)
        HRESULT ( STDMETHODCALLTYPE *ReleaseNotes )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, ShowMenu)
        HRESULT ( STDMETHODCALLTYPE *ShowMenu )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, HideMenu)
        HRESULT ( STDMETHODCALLTYPE *HideMenu )( 
            ICChatAutomation * This);
        
        DECLSPEC_XFGVIRT(ICChatAutomation, Automations)
        HRESULT ( STDMETHODCALLTYPE *Automations )( 
            ICChatAutomation * This);
        
        END_INTERFACE
    } ICChatAutomationVtbl;

    interface ICChatAutomation
    {
        CONST_VTBL struct ICChatAutomationVtbl *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICChatAutomation_QueryInterface(This,riid,ppvObject)	\
    ( (This)->lpVtbl -> QueryInterface(This,riid,ppvObject) ) 

#define ICChatAutomation_AddRef(This)	\
    ( (This)->lpVtbl -> AddRef(This) ) 

#define ICChatAutomation_Release(This)	\
    ( (This)->lpVtbl -> Release(This) ) 


#define ICChatAutomation_GetTypeInfoCount(This,pctinfo)	\
    ( (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo) ) 

#define ICChatAutomation_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    ( (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo) ) 

#define ICChatAutomation_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    ( (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId) ) 

#define ICChatAutomation_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    ( (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr) ) 


#define ICChatAutomation_SessionConnect(This)	\
    ( (This)->lpVtbl -> SessionConnect(This) ) 

#define ICChatAutomation_Open(This)	\
    ( (This)->lpVtbl -> Open(This) ) 

#define ICChatAutomation_Close(This)	\
    ( (This)->lpVtbl -> Close(This) ) 

#define ICChatAutomation_Save(This)	\
    ( (This)->lpVtbl -> Save(This) ) 

#define ICChatAutomation_SaveAs(This)	\
    ( (This)->lpVtbl -> SaveAs(This) ) 

#define ICChatAutomation_Createshortcut(This)	\
    ( (This)->lpVtbl -> Createshortcut(This) ) 

#define ICChatAutomation_Print(This)	\
    ( (This)->lpVtbl -> Print(This) ) 

#define ICChatAutomation_PrintSetup(This)	\
    ( (This)->lpVtbl -> PrintSetup(This) ) 

#define ICChatAutomation_About(This)	\
    ( (This)->lpVtbl -> About(This) ) 

#define ICChatAutomation_Undo(This)	\
    ( (This)->lpVtbl -> Undo(This) ) 

#define ICChatAutomation_Cut(This)	\
    ( (This)->lpVtbl -> Cut(This) ) 

#define ICChatAutomation_Copy(This)	\
    ( (This)->lpVtbl -> Copy(This) ) 

#define ICChatAutomation_Paste(This)	\
    ( (This)->lpVtbl -> Paste(This) ) 

#define ICChatAutomation_Delete(This)	\
    ( (This)->lpVtbl -> Delete(This) ) 

#define ICChatAutomation_SelectAll(This)	\
    ( (This)->lpVtbl -> SelectAll(This) ) 

#define ICChatAutomation_ViewToolbar(This)	\
    ( (This)->lpVtbl -> ViewToolbar(This) ) 

#define ICChatAutomation_ViewTabbar(This)	\
    ( (This)->lpVtbl -> ViewTabbar(This) ) 

#define ICChatAutomation_ViewStatusbar(This)	\
    ( (This)->lpVtbl -> ViewStatusbar(This) ) 

#define ICChatAutomation_ViewComics(This)	\
    ( (This)->lpVtbl -> ViewComics(This) ) 

#define ICChatAutomation_ViewText(This)	\
    ( (This)->lpVtbl -> ViewText(This) ) 

#define ICChatAutomation_ViewList(This)	\
    ( (This)->lpVtbl -> ViewList(This) ) 

#define ICChatAutomation_ViewIcon(This)	\
    ( (This)->lpVtbl -> ViewIcon(This) ) 

#define ICChatAutomation_Motd(This)	\
    ( (This)->lpVtbl -> Motd(This) ) 

#define ICChatAutomation_Options(This)	\
    ( (This)->lpVtbl -> Options(This) ) 

#define ICChatAutomation_SetColor(This)	\
    ( (This)->lpVtbl -> SetColor(This) ) 

#define ICChatAutomation_Bold(This)	\
    ( (This)->lpVtbl -> Bold(This) ) 

#define ICChatAutomation_Italic(This)	\
    ( (This)->lpVtbl -> Italic(This) ) 

#define ICChatAutomation_Underlined(This)	\
    ( (This)->lpVtbl -> Underlined(This) ) 

#define ICChatAutomation_FixedPitch(This)	\
    ( (This)->lpVtbl -> FixedPitch(This) ) 

#define ICChatAutomation_Symbol(This)	\
    ( (This)->lpVtbl -> Symbol(This) ) 

#define ICChatAutomation_NewRoom(This)	\
    ( (This)->lpVtbl -> NewRoom(This) ) 

#define ICChatAutomation_Leave(This)	\
    ( (This)->lpVtbl -> Leave(This) ) 

#define ICChatAutomation_CreateRoom(This)	\
    ( (This)->lpVtbl -> CreateRoom(This) ) 

#define ICChatAutomation_ChatroomList(This)	\
    ( (This)->lpVtbl -> ChatroomList(This) ) 

#define ICChatAutomation_Channelprops(This)	\
    ( (This)->lpVtbl -> Channelprops(This) ) 

#define ICChatAutomation_Disconnect(This)	\
    ( (This)->lpVtbl -> Disconnect(This) ) 

#define ICChatAutomation_UserList(This)	\
    ( (This)->lpVtbl -> UserList(This) ) 

#define ICChatAutomation_Invite(This)	\
    ( (This)->lpVtbl -> Invite(This) ) 

#define ICChatAutomation_Away(This)	\
    ( (This)->lpVtbl -> Away(This) ) 

#define ICChatAutomation_MemberGetinfo(This)	\
    ( (This)->lpVtbl -> MemberGetinfo(This) ) 

#define ICChatAutomation_Getidentity(This)	\
    ( (This)->lpVtbl -> Getidentity(This) ) 

#define ICChatAutomation_WhisperboxMlist(This)	\
    ( (This)->lpVtbl -> WhisperboxMlist(This) ) 

#define ICChatAutomation_MemberIgnore(This)	\
    ( (This)->lpVtbl -> MemberIgnore(This) ) 

#define ICChatAutomation_SendEmail(This)	\
    ( (This)->lpVtbl -> SendEmail(This) ) 

#define ICChatAutomation_SendFile(This)	\
    ( (This)->lpVtbl -> SendFile(This) ) 

#define ICChatAutomation_Homepage(This)	\
    ( (This)->lpVtbl -> Homepage(This) ) 

#define ICChatAutomation_Netmeeting(This)	\
    ( (This)->lpVtbl -> Netmeeting(This) ) 

#define ICChatAutomation_Version(This)	\
    ( (This)->lpVtbl -> Version(This) ) 

#define ICChatAutomation_PingUser(This)	\
    ( (This)->lpVtbl -> PingUser(This) ) 

#define ICChatAutomation_Localtime(This)	\
    ( (This)->lpVtbl -> Localtime(This) ) 

#define ICChatAutomation_AddToFavorites(This)	\
    ( (This)->lpVtbl -> AddToFavorites(This) ) 

#define ICChatAutomation_OpenFavorites(This)	\
    ( (This)->lpVtbl -> OpenFavorites(This) ) 

#define ICChatAutomation_Cascade(This)	\
    ( (This)->lpVtbl -> Cascade(This) ) 

#define ICChatAutomation_Tile(This)	\
    ( (This)->lpVtbl -> Tile(This) ) 

#define ICChatAutomation_Arrange(This)	\
    ( (This)->lpVtbl -> Arrange(This) ) 

#define ICChatAutomation_HelpTopics(This)	\
    ( (This)->lpVtbl -> HelpTopics(This) ) 

#define ICChatAutomation_Freestuff(This)	\
    ( (This)->lpVtbl -> Freestuff(This) ) 

#define ICChatAutomation_Productnews(This)	\
    ( (This)->lpVtbl -> Productnews(This) ) 

#define ICChatAutomation_FAQ(This)	\
    ( (This)->lpVtbl -> FAQ(This) ) 

#define ICChatAutomation_OnlineSupport(This)	\
    ( (This)->lpVtbl -> OnlineSupport(This) ) 

#define ICChatAutomation_BestofWeb(This)	\
    ( (This)->lpVtbl -> BestofWeb(This) ) 

#define ICChatAutomation_SearchtheWeb(This)	\
    ( (This)->lpVtbl -> SearchtheWeb(This) ) 

#define ICChatAutomation_MsHomepage(This)	\
    ( (This)->lpVtbl -> MsHomepage(This) ) 

#define ICChatAutomation_ReleaseNotes(This)	\
    ( (This)->lpVtbl -> ReleaseNotes(This) ) 

#define ICChatAutomation_ShowMenu(This)	\
    ( (This)->lpVtbl -> ShowMenu(This) ) 

#define ICChatAutomation_HideMenu(This)	\
    ( (This)->lpVtbl -> HideMenu(This) ) 

#define ICChatAutomation_Automations(This)	\
    ( (This)->lpVtbl -> Automations(This) ) 

#endif /* COBJMACROS */


#endif 	/* C style interface */




#endif 	/* __ICChatAutomation_INTERFACE_DEFINED__ */



#ifndef __iCChatLib_LIBRARY_DEFINED__
#define __iCChatLib_LIBRARY_DEFINED__

/* library iCChatLib */
/* [helpstring][version][uuid] */ 


EXTERN_C const IID LIBID_iCChatLib;

EXTERN_C const CLSID CLSID_Document;

#ifdef __cplusplus

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


