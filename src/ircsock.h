// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.

class CTlsClient;	// see tlssock.h

class CIrcSocket : public CAsyncSocket {
public:
	CIrcSocket();
	virtual ~CIrcSocket();

	virtual void OnConnect(int nErrorCode);
	virtual void OnClose(int nErrorCode);
	virtual void OnReceive(int nErrorCode);
	virtual void OnOutOfBandData(int nErrorCode) { TRACE("Out of Band socket on error %d.\n", nErrorCode); }
	virtual void ProcessMessage(char *);

	// --- Native TLS (SChannel) support -------------------------------------
	// Set before Connect(); when TRUE the connection is wrapped in TLS.
	void SetSecure(BOOL bSecure) { m_bSecure = bSecure; }

	// Non-virtual override of CAsyncSocket::Send.  All IRC traffic goes through
	// serverConn.Send(...) on the concrete CIrcSocket type, so this override is
	// picked up at every call site; when secure it encrypts before sending.
	int Send(const void *lpBuf, int nBufLen, int nFlags = 0);

private:
	void SendLogin();							// send NICK/USER once the link is ready
	void FeedPlainBytes(char *data, int len);	// split into lines -> ProcessMessage

	BOOL m_bSecure;
	int m_tlsState;								// IRC_TLS_* below
	CTlsClient *m_tls;
	CByteArray m_pendingPlain;					// plaintext queued during handshake

	char m_lineBuf[2048];						// accumulated plaintext awaiting full lines
	int m_lineLen;
};

#define IRC_TLS_NONE		0
#define IRC_TLS_HANDSHAKING	1
#define IRC_TLS_CONNECTED	2