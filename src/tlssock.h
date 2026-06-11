// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//
// tlssock.h : a minimal SChannel (Windows SSPI) TLS client used to add native
// SSL/TLS support to Comic Chat's IRC connection without an external tunnel.
//
// CTlsClient wraps the handshake and the encrypt/decrypt of an already-connected
// stream socket.  The owning socket performs the raw TCP I/O and hands raw bytes
// to/from this class.  See docs/tls.md for the design notes.

#ifndef _TLSSOCK_H_
#define _TLSSOCK_H_

#define SECURITY_WIN32
#include <security.h>
#include <schannel.h>

class CTlsClient {
public:
	CTlsClient();
	~CTlsClient();

	enum Result { TLS_ERROR, TLS_CONTINUE, TLS_DONE };

	// Acquire credentials and produce the initial ClientHello token (returned in
	// outToken, which the caller must send to the server).  serverName is used for
	// SNI and (optional) certificate validation.
	BOOL Begin(const char *serverName, CByteArray &outToken);

	// Feed handshake bytes received from the server.  Any token that must be sent
	// back is appended to outToken.  When the handshake finishes, returns TLS_DONE
	// and any application data that arrived early is placed in extraAppData.
	Result Continue(const BYTE *in, int inLen, CByteArray &outToken, CByteArray &extraAppData);

	// Encrypt plaintext into one or more TLS records (appended to cipher).
	BOOL Encrypt(const BYTE *plain, int len, CByteArray &cipher);

	// Decrypt received ciphertext.  Decrypted plaintext is appended to plainOut.
	// Sets renegotiate if the server asked to renegotiate (not supported here).
	BOOL Decrypt(const BYTE *in, int inLen, CByteArray &plainOut, BOOL &renegotiate);

	BOOL IsComplete() const { return m_complete; }

private:
	void Cleanup();

	CredHandle m_cred;
	CtxtHandle m_ctx;
	SecPkgContext_StreamSizes m_sizes;
	CByteArray m_recvBuf;   // accumulated ciphertext spanning record boundaries
	CString m_serverName;
	BOOL m_haveCred;
	BOOL m_haveCtx;
	BOOL m_haveSizes;
	BOOL m_complete;
	int m_incompleteCredRetries;
};

#endif // _TLSSOCK_H_
