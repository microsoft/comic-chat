/* this ALWAYS GENERATED file contains the definitions for the interfaces */


/* File created by MIDL compiler version 3.01.75 */
/* at Fri Feb 20 16:10:55 1998
 */
/* Compiler settings for xcchat.idl:
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

#ifndef __xcchat_h__
#define __xcchat_h__

#ifdef __cplusplus
extern "C"{
#endif 

/* Forward Declarations */ 

#ifndef __IXControl_FWD_DEFINED__
#define __IXControl_FWD_DEFINED__
typedef interface IXControl IXControl;
#endif 	/* __IXControl_FWD_DEFINED__ */


#ifndef __XControl_FWD_DEFINED__
#define __XControl_FWD_DEFINED__

#ifdef __cplusplus
typedef class XControl XControl;
#else
typedef struct XControl XControl;
#endif /* __cplusplus */

#endif 	/* __XControl_FWD_DEFINED__ */


#ifndef __XProps_FWD_DEFINED__
#define __XProps_FWD_DEFINED__

#ifdef __cplusplus
typedef class XProps XProps;
#else
typedef struct XProps XProps;
#endif /* __cplusplus */

#endif 	/* __XProps_FWD_DEFINED__ */


/* header files for imported files */
#include "oaidl.h"
#include "ocidl.h"

void __RPC_FAR * __RPC_USER MIDL_user_allocate(size_t);
void __RPC_USER MIDL_user_free( void __RPC_FAR * ); 

#ifndef __IXControl_INTERFACE_DEFINED__
#define __IXControl_INTERFACE_DEFINED__

/****************************************
 * Generated header for interface: IXControl
 * at Fri Feb 20 16:10:55 1998
 * using MIDL 3.01.75
 ****************************************/
/* [unique][helpstring][dual][uuid][object] */ 



EXTERN_C const IID IID_IXControl;

#if defined(__cplusplus) && !defined(CINTERFACE)
    
    interface DECLSPEC_UUID("3788BC02-3670-11D1-9787-00C04FB6C76D")
    IXControl : public IDispatch
    {
    public:
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowToolBar( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowToolBar( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_JoinNetMeeting( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_JoinNetMeeting( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowMenu( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowMenu( 
            /* [in] */ BOOL newVal) = 0;
        
        virtual /* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE get_ShowNetMeeting( 
            /* [retval][out] */ BOOL __RPC_FAR *pVal) = 0;
        
        virtual /* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE put_ShowNetMeeting( 
            /* [in] */ BOOL newVal) = 0;
        
    };
    
#else 	/* C style interface */

    typedef struct IXControlVtbl
    {
        BEGIN_INTERFACE
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *QueryInterface )( 
            IXControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [iid_is][out] */ void __RPC_FAR *__RPC_FAR *ppvObject);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *AddRef )( 
            IXControl __RPC_FAR * This);
        
        ULONG ( STDMETHODCALLTYPE __RPC_FAR *Release )( 
            IXControl __RPC_FAR * This);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfoCount )( 
            IXControl __RPC_FAR * This,
            /* [out] */ UINT __RPC_FAR *pctinfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetTypeInfo )( 
            IXControl __RPC_FAR * This,
            /* [in] */ UINT iTInfo,
            /* [in] */ LCID lcid,
            /* [out] */ ITypeInfo __RPC_FAR *__RPC_FAR *ppTInfo);
        
        HRESULT ( STDMETHODCALLTYPE __RPC_FAR *GetIDsOfNames )( 
            IXControl __RPC_FAR * This,
            /* [in] */ REFIID riid,
            /* [size_is][in] */ LPOLESTR __RPC_FAR *rgszNames,
            /* [in] */ UINT cNames,
            /* [in] */ LCID lcid,
            /* [size_is][out] */ DISPID __RPC_FAR *rgDispId);
        
        /* [local] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *Invoke )( 
            IXControl __RPC_FAR * This,
            /* [in] */ DISPID dispIdMember,
            /* [in] */ REFIID riid,
            /* [in] */ LCID lcid,
            /* [in] */ WORD wFlags,
            /* [out][in] */ DISPPARAMS __RPC_FAR *pDispParams,
            /* [out] */ VARIANT __RPC_FAR *pVarResult,
            /* [out] */ EXCEPINFO __RPC_FAR *pExcepInfo,
            /* [out] */ UINT __RPC_FAR *puArgErr);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowToolBar )( 
            IXControl __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowToolBar )( 
            IXControl __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_JoinNetMeeting )( 
            IXControl __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_JoinNetMeeting )( 
            IXControl __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowMenu )( 
            IXControl __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowMenu )( 
            IXControl __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        /* [helpstring][id][propget] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *get_ShowNetMeeting )( 
            IXControl __RPC_FAR * This,
            /* [retval][out] */ BOOL __RPC_FAR *pVal);
        
        /* [helpstring][id][propput] */ HRESULT ( STDMETHODCALLTYPE __RPC_FAR *put_ShowNetMeeting )( 
            IXControl __RPC_FAR * This,
            /* [in] */ BOOL newVal);
        
        END_INTERFACE
    } IXControlVtbl;

    interface IXControl
    {
        CONST_VTBL struct IXControlVtbl __RPC_FAR *lpVtbl;
    };

    

#ifdef COBJMACROS


#define IXControl_QueryInterface(This,riid,ppvObject)	\
    (This)->lpVtbl -> QueryInterface(This,riid,ppvObject)

#define IXControl_AddRef(This)	\
    (This)->lpVtbl -> AddRef(This)

#define IXControl_Release(This)	\
    (This)->lpVtbl -> Release(This)


#define IXControl_GetTypeInfoCount(This,pctinfo)	\
    (This)->lpVtbl -> GetTypeInfoCount(This,pctinfo)

#define IXControl_GetTypeInfo(This,iTInfo,lcid,ppTInfo)	\
    (This)->lpVtbl -> GetTypeInfo(This,iTInfo,lcid,ppTInfo)

#define IXControl_GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)	\
    (This)->lpVtbl -> GetIDsOfNames(This,riid,rgszNames,cNames,lcid,rgDispId)

#define IXControl_Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)	\
    (This)->lpVtbl -> Invoke(This,dispIdMember,riid,lcid,wFlags,pDispParams,pVarResult,pExcepInfo,puArgErr)


#define IXControl_get_ShowToolBar(This,pVal)	\
    (This)->lpVtbl -> get_ShowToolBar(This,pVal)

#define IXControl_put_ShowToolBar(This,newVal)	\
    (This)->lpVtbl -> put_ShowToolBar(This,newVal)

#define IXControl_get_JoinNetMeeting(This,pVal)	\
    (This)->lpVtbl -> get_JoinNetMeeting(This,pVal)

#define IXControl_put_JoinNetMeeting(This,newVal)	\
    (This)->lpVtbl -> put_JoinNetMeeting(This,newVal)

#define IXControl_get_ShowMenu(This,pVal)	\
    (This)->lpVtbl -> get_ShowMenu(This,pVal)

#define IXControl_put_ShowMenu(This,newVal)	\
    (This)->lpVtbl -> put_ShowMenu(This,newVal)

#define IXControl_get_ShowNetMeeting(This,pVal)	\
    (This)->lpVtbl -> get_ShowNetMeeting(This,pVal)

#define IXControl_put_ShowNetMeeting(This,newVal)	\
    (This)->lpVtbl -> put_ShowNetMeeting(This,newVal)

#endif /* COBJMACROS */


#endif 	/* C style interface */



/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXControl_get_ShowToolBar_Proxy( 
    IXControl __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IXControl_get_ShowToolBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXControl_put_ShowToolBar_Proxy( 
    IXControl __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IXControl_put_ShowToolBar_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXControl_get_JoinNetMeeting_Proxy( 
    IXControl __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IXControl_get_JoinNetMeeting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXControl_put_JoinNetMeeting_Proxy( 
    IXControl __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IXControl_put_JoinNetMeeting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXControl_get_ShowMenu_Proxy( 
    IXControl __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IXControl_get_ShowMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXControl_put_ShowMenu_Proxy( 
    IXControl __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IXControl_put_ShowMenu_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propget] */ HRESULT STDMETHODCALLTYPE IXControl_get_ShowNetMeeting_Proxy( 
    IXControl __RPC_FAR * This,
    /* [retval][out] */ BOOL __RPC_FAR *pVal);


void __RPC_STUB IXControl_get_ShowNetMeeting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);


/* [helpstring][id][propput] */ HRESULT STDMETHODCALLTYPE IXControl_put_ShowNetMeeting_Proxy( 
    IXControl __RPC_FAR * This,
    /* [in] */ BOOL newVal);


void __RPC_STUB IXControl_put_ShowNetMeeting_Stub(
    IRpcStubBuffer *This,
    IRpcChannelBuffer *_pRpcChannelBuffer,
    PRPC_MESSAGE _pRpcMessage,
    DWORD *_pdwStubPhase);



#endif 	/* __IXControl_INTERFACE_DEFINED__ */



#ifndef __XCCHATLib_LIBRARY_DEFINED__
#define __XCCHATLib_LIBRARY_DEFINED__

/****************************************
 * Generated header for library: XCCHATLib
 * at Fri Feb 20 16:10:55 1998
 * using MIDL 3.01.75
 ****************************************/
/* [helpstring][version][uuid] */ 



EXTERN_C const IID LIBID_XCCHATLib;

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_XControl;

class DECLSPEC_UUID("3788BC03-3670-11D1-9787-00C04FB6C76D")
XControl;
#endif

#ifdef __cplusplus
EXTERN_C const CLSID CLSID_XProps;

class DECLSPEC_UUID("0044C943-5A70-11D1-9787-00C04FB6C76D")
XProps;
#endif
#endif /* __XCCHATLib_LIBRARY_DEFINED__ */

/* Additional Prototypes for ALL interfaces */

/* end of Additional Prototypes */

#ifdef __cplusplus
}
#endif

#endif
