/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Wed Nov 05 13:23:46 1997
 */
/* Compiler settings for icbcore.idl:
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

#ifndef __icbcore_h__
#define __icbcore_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __ICb32Core_FWD_DEFINED__
#define __ICb32Core_FWD_DEFINED__
typedef interface ICb32Core ICb32Core;
#endif 	/* __ICb32Core_FWD_DEFINED__ */


#ifndef __ICb32CoreNotify_FWD_DEFINED__
#define __ICb32CoreNotify_FWD_DEFINED__
typedef interface ICb32CoreNotify ICb32CoreNotify;
#endif 	/* __ICb32CoreNotify_FWD_DEFINED__ */


#ifndef __Cb32Core_FWD_DEFINED__
#define __Cb32Core_FWD_DEFINED__

#ifdef __cplusplus
typedef class Cb32Core Cb32Core;
#else
typedef struct Cb32Core Cb32Core;
#endif /* __cplusplus */

#endif 	/* __Cb32Core_FWD_DEFINED__ */


/* header files for imported files */
#include "objidl.h"
#include "oleidl.h"
#include "oaidl.h"
#include "imsconf2.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

/****************************************
 * Generated header for interface: __MIDL_itf_icbcore_0000
 * at Wed Nov 05 13:23:46 1997
 * using MIDL 3.01.75
 ****************************************/
/* [local] */ 


//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//  Copyright 1995-1997 Microsoft Corporation. All Rights Reserved.
//
//  File: icbcore.h
//
//--------------------------------------------------------------------------



////////////////////////////////////////////////////////////////////////////
//
// Constants
enum {
  CB32OP_CLOSEAPP = 0,
  CB32OP_SHOWUI = 1
};


////////////////////////////////////////////////////////////////////////////
//  Return Codes

#define NMCB_S(e) (0x01000300UL | (ULONG) (e))
#define NMCB_E(e) (0x81000300UL | (ULONG) (e))

enum {

// Cb32core specific return codes
//
  NMCB_S_CANTSENDYET		= NMCB_S((ULONG) 0x0001),
  NMCB_S_CLOSEAPP		= NMCB_S((ULONG) 0x0002),
  NMCB_S_LASTCODE		= NMCB_E((ULONG) 0x00FF)
};

enum {

// Cb32core specific error return codes
//
  NMCB_E_NOTINITIALIZED	= NMCB_E((ULONG) 0x0001),
  NMCB_E_NOTCHATTING		= NMCB_E((ULONG) 0x0002),
  NMCB_E_UNKNOWNSENDER	= NMCB_E((ULONG) 0x0003),
  NMCB_E_PARTIALMSG		= NMCB_E((ULONG) 0x0004),
  NMCB_E_CANTWHISPER		= NMCB_E((ULONG) 0x0005),
  NMCB_E_CANTWHISPERNM1	= NMCB_E((ULONG) 0x0006),
  NMCB_E_LASTCODE		= NMCB_E((ULONG) 0x00FF)
};


////////////////////////////////////////////////////////////////////////////
//  Interface Definitions


extern RPC_IF_HANDLE __MIDL_itf_icbcore_0000_v0_0_c_ifspec;
extern RPC_IF_HANDLE __MIDL_itf_icbcore_0000_v0_0_s_ifspec;

#ifndef __ICb32Core_INTERFACE_DEFINED__
#define __ICb32Core_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICb32Core
 * at Wed Nov 05 13:23:46 1997
 * using MIDL 3.01.75
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ICb32Core __RPC_FAR *LPCB32CORE;


EXTERN_C const IID IID_ICb32Core;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("C7047721-CABE-11d0-A041-444553540000")
    ICb32Core : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE Init( 
            /* [in] */ TCHAR __RPC_FAR *pszCommandLine) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SetNotify( 
            /* [in] */ IUnknown __RPC_FAR *pNotify) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE StartChat( 
            /* [in] */ HWND hMainWnd,
            /* [out] */ INmSysInfo __RPC_FAR *__RPC_FAR *ppSysInfo) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE DeInit( void) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SendMessageData( 
            /* [in] */ INmMember __RPC_FAR *pMember,
            /* [in] */ ULONG uBuffer,
            /* [size_is][in] */ byte __RPC_FAR *pBuffer,
            /* [in] */ ULONG uCChatBuffer,
            /* [size_is][in] */ byte __RPC_FAR *pCChatBuffer) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE OnIdle( void) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICb32CoreVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICb32Core __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICb32Core __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICb32Core __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Init )( 
            ICb32Core __RPC_FAR * This,
            /* [in] */ TCHAR __RPC_FAR *pszCommandLine);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SetNotify )( 
            ICb32Core __RPC_FAR * This,
            /* [in] */ IUnknown __RPC_FAR *pNotify);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *StartChat )( 
            ICb32Core __RPC_FAR * This,
            /* [in] */ HWND hMainWnd,
            /* [out] */ INmSysInfo __RPC_FAR *__RPC_FAR *ppSysInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *DeInit )( 
            ICb32Core __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SendMessageData )( 
            ICb32Core __RPC_FAR * This,
            /* [in] */ INmMember __RPC_FAR *pMember,
            /* [in] */ ULONG uBuffer,
            /* [size_is][in] */ byte __RPC_FAR *pBuffer,
            /* [in] */ ULONG uCChatBuffer,
            /* [size_is][in] */ byte __RPC_FAR *pCChatBuffer);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *OnIdle )( 
            ICb32Core __RPC_FAR * This);
        
        END_INTERFACE
    } ICb32CoreVtbl;

    interface ICb32Core
    {
        CONST_VTBL struct ICb32CoreVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICb32Core_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICb32Core_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICb32Core_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICb32Core_Init(This,pszCommandLine)	\
    (This)->lpVtbl -> Init(This,pszCommandLine)

#define ICb32Core_SetNotify(This,pNotify)	\
    (This)->lpVtbl -> SetNotify(This,pNotify)

#define ICb32Core_StartChat(This,hMainWnd,ppSysInfo)	\
    (This)->lpVtbl -> StartChat(This,hMainWnd,ppSysInfo)

#define ICb32Core_DeInit(This)	\
    (This)->lpVtbl -> DeInit(This)

#define ICb32Core_SendMessageData(This,pMember,uBuffer,pBuffer,uCChatBuffer,pCChatBuffer)	\
    (This)->lpVtbl -> SendMessageData(This,pMember,uBuffer,pBuffer,uCChatBuffer,pCChatBuffer)

#define ICb32Core_OnIdle(This)	\
    (This)->lpVtbl -> OnIdle(This)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICb32Core_Init_Proxy( 
    ICb32Core __RPC_FAR * This,
    /* [in] */ TCHAR __RPC_FAR *pszCommandLine);


void __RPC_STUB ICb32Core_Init_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32Core_SetNotify_Proxy( 
    ICb32Core __RPC_FAR * This,
    /* [in] */ IUnknown __RPC_FAR *pNotify);


void __RPC_STUB ICb32Core_SetNotify_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32Core_StartChat_Proxy( 
    ICb32Core __RPC_FAR * This,
    /* [in] */ HWND hMainWnd,
    /* [out] */ INmSysInfo __RPC_FAR *__RPC_FAR *ppSysInfo);


void __RPC_STUB ICb32Core_StartChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32Core_DeInit_Proxy( 
    ICb32Core __RPC_FAR * This);


void __RPC_STUB ICb32Core_DeInit_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32Core_SendMessageData_Proxy( 
    ICb32Core __RPC_FAR * This,
    /* [in] */ INmMember __RPC_FAR *pMember,
    /* [in] */ ULONG uBuffer,
    /* [size_is][in] */ byte __RPC_FAR *pBuffer,
    /* [in] */ ULONG uCChatBuffer,
    /* [size_is][in] */ byte __RPC_FAR *pCChatBuffer);


void __RPC_STUB ICb32Core_SendMessageData_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32Core_OnIdle_Proxy( 
    ICb32Core __RPC_FAR * This);


void __RPC_STUB ICb32Core_OnIdle_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICb32Core_INTERFACE_DEFINED__ */


#ifndef __ICb32CoreNotify_INTERFACE_DEFINED__
#define __ICb32CoreNotify_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: ICb32CoreNotify
 * at Wed Nov 05 13:23:46 1997
 * using MIDL 3.01.75
 ****************************************/
/* [unique][uuid][object] */ 


typedef /* [unique] */ ICb32CoreNotify __RPC_FAR *LPCB32CORENOTIFY;


EXTERN_C const IID IID_ICb32CoreNotify;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("C7047722-CABE-11d0-A041-444553540000")
    ICb32CoreNotify : public IUnknown
    {
    public:
        virtual HRESULT STDMETHODCALLTYPE ChatStatus( 
            /* [in] */ BOOL bInProgress) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE ReceivedMessage( 
            /* [in] */ BOOL bEcho,
            /* [in] */ TCHAR __RPC_FAR *pszMemberName,
            /* [in] */ INmMember __RPC_FAR *pMember,
            /* [in] */ TCHAR __RPC_FAR *pszWhisperedToName,
            /* [in] */ INmMember __RPC_FAR *pWhisperedTo,
            /* [in] */ ULONG uBuffer,
            /* [size_is][in] */ byte __RPC_FAR *pBuffer,
            /* [in] */ ULONG uCChatBuffer,
            /* [size_is][in] */ byte __RPC_FAR *pCChatBuffer,
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MemberJoinedConference( 
            /* [in] */ INmMember __RPC_FAR *pMember,
            /* [in] */ HRESULT hr) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MemberLeftConference( 
            /* [in] */ INmMember __RPC_FAR *pMember) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MemberJoinedChat( 
            /* [in] */ INmMember __RPC_FAR *pMember) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE MemberLeftChat( 
            /* [in] */ INmMember __RPC_FAR *pMember) = 0;
        
        virtual HRESULT STDMETHODCALLTYPE SpecialOps( 
            /* [in] */ DWORD dwSpecial) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct ICb32CoreNotifyVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            ICb32CoreNotify __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            ICb32CoreNotify __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            ICb32CoreNotify __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ChatStatus )( 
            ICb32CoreNotify __RPC_FAR * This,
            /* [in] */ BOOL bInProgress);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *ReceivedMessage )( 
            ICb32CoreNotify __RPC_FAR * This,
            /* [in] */ BOOL bEcho,
            /* [in] */ TCHAR __RPC_FAR *pszMemberName,
            /* [in] */ INmMember __RPC_FAR *pMember,
            /* [in] */ TCHAR __RPC_FAR *pszWhisperedToName,
            /* [in] */ INmMember __RPC_FAR *pWhisperedTo,
            /* [in] */ ULONG uBuffer,
            /* [size_is][in] */ byte __RPC_FAR *pBuffer,
            /* [in] */ ULONG uCChatBuffer,
            /* [size_is][in] */ byte __RPC_FAR *pCChatBuffer,
            /* [in] */ HRESULT hr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MemberJoinedConference )( 
            ICb32CoreNotify __RPC_FAR * This,
            /* [in] */ INmMember __RPC_FAR *pMember,
            /* [in] */ HRESULT hr);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MemberLeftConference )( 
            ICb32CoreNotify __RPC_FAR * This,
            /* [in] */ INmMember __RPC_FAR *pMember);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MemberJoinedChat )( 
            ICb32CoreNotify __RPC_FAR * This,
            /* [in] */ INmMember __RPC_FAR *pMember);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *MemberLeftChat )( 
            ICb32CoreNotify __RPC_FAR * This,
            /* [in] */ INmMember __RPC_FAR *pMember);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *SpecialOps )( 
            ICb32CoreNotify __RPC_FAR * This,
            /* [in] */ DWORD dwSpecial);
        
        END_INTERFACE
    } ICb32CoreNotifyVtbl;

    interface ICb32CoreNotify
    {
        CONST_VTBL struct ICb32CoreNotifyVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define ICb32CoreNotify_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define ICb32CoreNotify_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define ICb32CoreNotify_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define ICb32CoreNotify_ChatStatus(This,bInProgress)	\
    (This)->lpVtbl -> ChatStatus(This,bInProgress)

#define ICb32CoreNotify_ReceivedMessage(This,bEcho,pszMemberName,pMember,pszWhisperedToName,pWhisperedTo,uBuffer,pBuffer,uCChatBuffer,pCChatBuffer,hr)	\
    (This)->lpVtbl -> ReceivedMessage(This,bEcho,pszMemberName,pMember,pszWhisperedToName,pWhisperedTo,uBuffer,pBuffer,uCChatBuffer,pCChatBuffer,hr)

#define ICb32CoreNotify_MemberJoinedConference(This,pMember,hr)	\
    (This)->lpVtbl -> MemberJoinedConference(This,pMember,hr)

#define ICb32CoreNotify_MemberLeftConference(This,pMember)	\
    (This)->lpVtbl -> MemberLeftConference(This,pMember)

#define ICb32CoreNotify_MemberJoinedChat(This,pMember)	\
    (This)->lpVtbl -> MemberJoinedChat(This,pMember)

#define ICb32CoreNotify_MemberLeftChat(This,pMember)	\
    (This)->lpVtbl -> MemberLeftChat(This,pMember)

#define ICb32CoreNotify_SpecialOps(This,dwSpecial)	\
    (This)->lpVtbl -> SpecialOps(This,dwSpecial)

#endif /* COBJMACROS */


#endif 	/* C style interface */



HRESULT STDMETHODCALLTYPE ICb32CoreNotify_ChatStatus_Proxy( 
    ICb32CoreNotify __RPC_FAR * This,
    /* [in] */ BOOL bInProgress);


void __RPC_STUB ICb32CoreNotify_ChatStatus_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32CoreNotify_ReceivedMessage_Proxy( 
    ICb32CoreNotify __RPC_FAR * This,
    /* [in] */ BOOL bEcho,
    /* [in] */ TCHAR __RPC_FAR *pszMemberName,
    /* [in] */ INmMember __RPC_FAR *pMember,
    /* [in] */ TCHAR __RPC_FAR *pszWhisperedToName,
    /* [in] */ INmMember __RPC_FAR *pWhisperedTo,
    /* [in] */ ULONG uBuffer,
    /* [size_is][in] */ byte __RPC_FAR *pBuffer,
    /* [in] */ ULONG uCChatBuffer,
    /* [size_is][in] */ byte __RPC_FAR *pCChatBuffer,
    /* [in] */ HRESULT hr);


void __RPC_STUB ICb32CoreNotify_ReceivedMessage_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32CoreNotify_MemberJoinedConference_Proxy( 
    ICb32CoreNotify __RPC_FAR * This,
    /* [in] */ INmMember __RPC_FAR *pMember,
    /* [in] */ HRESULT hr);


void __RPC_STUB ICb32CoreNotify_MemberJoinedConference_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32CoreNotify_MemberLeftConference_Proxy( 
    ICb32CoreNotify __RPC_FAR * This,
    /* [in] */ INmMember __RPC_FAR *pMember);


void __RPC_STUB ICb32CoreNotify_MemberLeftConference_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32CoreNotify_MemberJoinedChat_Proxy( 
    ICb32CoreNotify __RPC_FAR * This,
    /* [in] */ INmMember __RPC_FAR *pMember);


void __RPC_STUB ICb32CoreNotify_MemberJoinedChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32CoreNotify_MemberLeftChat_Proxy( 
    ICb32CoreNotify __RPC_FAR * This,
    /* [in] */ INmMember __RPC_FAR *pMember);


void __RPC_STUB ICb32CoreNotify_MemberLeftChat_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


HRESULT STDMETHODCALLTYPE ICb32CoreNotify_SpecialOps_Proxy( 
    ICb32CoreNotify __RPC_FAR * This,
    /* [in] */ DWORD dwSpecial);


void __RPC_STUB ICb32CoreNotify_SpecialOps_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __ICb32CoreNotify_INTERFACE_DEFINED__ */



#ifndef __Cb32Core_LIBRARY_DEFINED__
#define __Cb32Core_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: Cb32Core
 * at Wed Nov 05 13:23:46 1997
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_Cb32Core;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_Cb32Core;

class DECLSPEC_UUID("C7047720-CABE-11d0-A041-444553540000")
Cb32Core;
#endif
#endif /* __Cb32Core_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

unsigned long             __RPC_USER  HWND_UserSize(     unsigned long __RPC_FAR *, unsigned long            , HWND __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  HWND_UserMarshal(  unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, HWND __RPC_FAR * ); 
unsigned char __RPC_FAR * __RPC_USER  HWND_UserUnmarshal(unsigned long __RPC_FAR *, unsigned char __RPC_FAR *, HWND __RPC_FAR * ); 
void                      __RPC_USER  HWND_UserFree(     unsigned long __RPC_FAR *, HWND __RPC_FAR * ); 

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
