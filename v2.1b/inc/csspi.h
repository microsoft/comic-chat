#ifndef __CSSPI_H__
#define __CSSPI_H__

//+-------------------------------------------------------------------------------------------
//
//  Security:   Defines for using SSPI.
//
//  Synopsis:   All information retrieved from the January 1997 version of MSDN.  Search on 
//              the term "SSPI" for detail information.  I only transfered the minimum
//              defines required for a client IRCX application.
//
//				Sorry, but this section ain't pretty.
//
//  Rem:		From KentCe simple example
//--------------------------------------------------------------------------------------------

#define SECPKG_CRED_OUTBOUND		2
#define SECBUFFER_TOKEN				2
#define ISC_REQ_CONFIDENTIALITY		0x10
#define ISC_REG_USE_SESSION_KEY		0x20
#define ISC_REQ_PROMPT_FOR_CREDS	0x40
#define ISC_REQ_USE_SUPPLIED_CREDS	0x80
#define SECURITY_NATIVE_DREP		0x10

#define SEC_E_NO_CREDENTIALS		((HRESULT)0x8009030EL)
#define SEC_I_COMPLETE_NEEDED		((HRESULT)0x00090313L)
#define SEC_I_COMPLETE_AND_CONTINUE	((HRESULT)0x00090314L)

typedef struct _SecHandle {
    unsigned long dwLower;
    unsigned long dwUpper;
} SecHandle, * PSecHandle;

typedef SecHandle   CredHandle;
typedef PSecHandle  PCredHandle;

typedef SecHandle   CtxtHandle;
typedef PSecHandle  PCtxtHandle;

typedef struct _SecBuffer {
    unsigned long cbBuffer;     
    unsigned long BufferType;   
    void * pvBuffer;            
} SecBuffer, * PSecBuffer;

typedef struct _SecBufferDesc {
    unsigned long ulVersion;      
    unsigned long cBuffers;       
    PSecBuffer pBuffers;          
} SecBufferDesc, * PSecBufferDesc;

typedef DWORD
(__stdcall * ACQUIRE_CREDENTIALS_HANDLE_FN)(
    CHAR *,
    CHAR *,
    unsigned long,
    void *,
    void *,
    void *, 
    void *,
    PCredHandle,
    DWORD *);

typedef DWORD
(__stdcall * FREE_CREDENTIALS_HANDLE_FN)(
    PCredHandle );

typedef DWORD
(__stdcall * DELETE_SECURITY_CONTEXT_FN)(
    PCtxtHandle );

typedef DWORD
(__stdcall * INITIALIZE_SECURITY_CONTEXT_FN)(
    PCredHandle,
    PCtxtHandle,
    CHAR *,
    unsigned long,
    unsigned long,
    unsigned long,
    PSecBufferDesc,
    unsigned long,
    PCtxtHandle,
    PSecBufferDesc,
    unsigned long *,
    DWORD *);

typedef DWORD
(__stdcall * COMPLETE_AUTH_TOKEN_FN)(
    PCtxtHandle,
    PSecBufferDesc);

typedef struct _SECURITY_FUNCTION_TABLE {
   unsigned long                       dwVersion;
   void *                              Reserved1;
   void *                              Reserved2;
   ACQUIRE_CREDENTIALS_HANDLE_FN       AcquireCredentialsHandle;
   FREE_CREDENTIALS_HANDLE_FN          FreeCredentialHandle;
   void *                              Reserved3;
   INITIALIZE_SECURITY_CONTEXT_FN      InitializeSecurityContext;    
   void *                              Reserved4;
   COMPLETE_AUTH_TOKEN_FN              CompleteAuthToken;
   DELETE_SECURITY_CONTEXT_FN          DeleteSecurityContext;
} SecurityFunctionTable, * PSecurityFunctionTable;

typedef PSecurityFunctionTable
(__stdcall * INIT_SECURITY_INTERFACE)(void);

#define SECURITY_ENTRYPOINT "InitSecurityInterfaceA"

#endif // CSSPI.H
