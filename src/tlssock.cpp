// Copyright (c) Microsoft Corporation.
// Licensed under the MIT license.
//
// tlssock.cpp : minimal SChannel (Windows SSPI) TLS client.  See tlssock.h.
//
// This is intentionally compact: it implements just enough of the SChannel
// client flow to wrap a line-oriented IRC stream.  Certificate validation is
// left to SChannel's defaults plus a permissive manual check (many IRC servers
// present self-signed or community-CA certs); tighten Begin() if you need strict
// validation.

#include "stdafx.h"
#include "tlssock.h"

#pragma comment(lib, "secur32.lib")

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#ifndef SCH_CRED_NO_DEFAULT_CREDS
#define SCH_CRED_NO_DEFAULT_CREDS 0x00000010
#endif
#ifndef SCH_CRED_MANUAL_CRED_VALIDATION
#define SCH_CRED_MANUAL_CRED_VALIDATION 0x00000008
#endif

static const DWORD kIscReqFlags =
	ISC_REQ_SEQUENCE_DETECT | ISC_REQ_REPLAY_DETECT | ISC_REQ_CONFIDENTIALITY |
	ISC_REQ_EXTENDED_ERROR | ISC_REQ_ALLOCATE_MEMORY | ISC_REQ_STREAM |
	ISC_REQ_MANUAL_CRED_VALIDATION;

CTlsClient::CTlsClient() {
	SecInvalidateHandle(&m_cred);
	SecInvalidateHandle(&m_ctx);
	memset(&m_sizes, 0, sizeof(m_sizes));
	m_haveCred = m_haveCtx = m_haveSizes = m_complete = FALSE;
	m_incompleteCredRetries = 0;
}

CTlsClient::~CTlsClient() {
	Cleanup();
}

void CTlsClient::Cleanup() {
	if (m_haveCtx)  { DeleteSecurityContext(&m_ctx);    m_haveCtx = FALSE; }
	if (m_haveCred) { FreeCredentialsHandle(&m_cred);   m_haveCred = FALSE; }
	m_recvBuf.RemoveAll();
}

static void AppendBytes(CByteArray &arr, const BYTE *data, int len) {
	int base = arr.GetSize();
	arr.SetSize(base + len);
	memcpy(arr.GetData() + base, data, len);
}

BOOL CTlsClient::Begin(const char *serverName, CByteArray &outToken) {
	m_serverName = serverName;

	SCHANNEL_CRED sc;
	memset(&sc, 0, sizeof(sc));
	sc.dwVersion = SCHANNEL_CRED_VERSION;
	// SCH_CRED_NO_DEFAULT_CREDS keeps SChannel from auto-selecting (and prompting
	// for, e.g. a smart card) a client certificate when the server requests one.
	// Instead ISC returns SEC_I_INCOMPLETE_CREDENTIALS, which Continue() handles by
	// re-calling ISC so SChannel sends an empty client cert and proceeds.
	sc.dwFlags = SCH_CRED_NO_DEFAULT_CREDS | SCH_CRED_MANUAL_CRED_VALIDATION;
	sc.grbitEnabledProtocols = 0;	// let the OS negotiate the best available TLS

	TimeStamp tsExpiry;
	SECURITY_STATUS ss = AcquireCredentialsHandle(
		NULL, UNISP_NAME, SECPKG_CRED_OUTBOUND, NULL, &sc, NULL, NULL,
		&m_cred, &tsExpiry);
	if (ss != SEC_E_OK) {
		TRACE("TLS: AcquireCredentialsHandle failed 0x%lx\n", ss);
		return FALSE;
	}
	m_haveCred = TRUE;

	SecBuffer outBuf;
	outBuf.pvBuffer = NULL;
	outBuf.BufferType = SECBUFFER_TOKEN;
	outBuf.cbBuffer = 0;
	SecBufferDesc outDesc;
	outDesc.ulVersion = SECBUFFER_VERSION;
	outDesc.cBuffers = 1;
	outDesc.pBuffers = &outBuf;

	DWORD attrs = 0;
	ss = InitializeSecurityContext(
		&m_cred, NULL, (SEC_CHAR *)(LPCTSTR)m_serverName, kIscReqFlags, 0, 0,
		NULL, 0, &m_ctx, &outDesc, &attrs, &tsExpiry);
	m_haveCtx = TRUE;

	if (ss != SEC_I_CONTINUE_NEEDED) {
		TRACE("TLS: initial InitializeSecurityContext returned 0x%lx\n", ss);
		return FALSE;
	}
	if (outBuf.cbBuffer && outBuf.pvBuffer) {
		AppendBytes(outToken, (BYTE *)outBuf.pvBuffer, outBuf.cbBuffer);
		FreeContextBuffer(outBuf.pvBuffer);
	}
	return TRUE;
}

CTlsClient::Result CTlsClient::Continue(const BYTE *in, int inLen,
										CByteArray &outToken, CByteArray &extraAppData) {
	if (inLen > 0) AppendBytes(m_recvBuf, in, inLen);

	TimeStamp tsExpiry;
	while (TRUE) {
		if (m_recvBuf.GetSize() == 0) return TLS_CONTINUE;

		SecBuffer inBufs[2];
		inBufs[0].pvBuffer = m_recvBuf.GetData();
		inBufs[0].cbBuffer = m_recvBuf.GetSize();
		inBufs[0].BufferType = SECBUFFER_TOKEN;
		inBufs[1].pvBuffer = NULL;
		inBufs[1].cbBuffer = 0;
		inBufs[1].BufferType = SECBUFFER_EMPTY;
		SecBufferDesc inDesc;
		inDesc.ulVersion = SECBUFFER_VERSION;
		inDesc.cBuffers = 2;
		inDesc.pBuffers = inBufs;

		SecBuffer outBuf;
		outBuf.pvBuffer = NULL;
		outBuf.cbBuffer = 0;
		outBuf.BufferType = SECBUFFER_TOKEN;
		SecBufferDesc outDesc;
		outDesc.ulVersion = SECBUFFER_VERSION;
		outDesc.cBuffers = 1;
		outDesc.pBuffers = &outBuf;

		DWORD attrs = 0;
		SECURITY_STATUS ss = InitializeSecurityContext(
			&m_cred, &m_ctx, (SEC_CHAR *)(LPCTSTR)m_serverName, kIscReqFlags, 0, 0,
			&inDesc, 0, NULL, &outDesc, &attrs, &tsExpiry);

		// Any output token must be sent regardless of the result code.
		if (outBuf.cbBuffer && outBuf.pvBuffer) {
			AppendBytes(outToken, (BYTE *)outBuf.pvBuffer, outBuf.cbBuffer);
			FreeContextBuffer(outBuf.pvBuffer);
		}

		if (ss == SEC_E_INCOMPLETE_MESSAGE)
			return TLS_CONTINUE;	// need more bytes; keep the buffer intact

		// Server requested a client certificate.  We don't supply one; re-call ISC
		// with the same input so SChannel sends an empty client cert and proceeds.
		// (Guarded so a misbehaving server can't spin us forever.)
		if (ss == SEC_I_INCOMPLETE_CREDENTIALS) {
			if (++m_incompleteCredRetries > 4) {
				TRACE("TLS: too many SEC_I_INCOMPLETE_CREDENTIALS\n");
				return TLS_ERROR;
			}
			continue;	// re-invoke ISC with the same buffered handshake data
		}

		if (ss == SEC_I_CONTINUE_NEEDED || ss == SEC_E_OK) {
			// Carry over any unprocessed bytes (SECBUFFER_EXTRA).
			int consumed = m_recvBuf.GetSize();
			int extra = 0;
			if (inBufs[1].BufferType == SECBUFFER_EXTRA)
				extra = inBufs[1].cbBuffer;
			if (extra > 0) {
				int start = consumed - extra;
				CByteArray keep;
				AppendBytes(keep, m_recvBuf.GetData() + start, extra);
				m_recvBuf.RemoveAll();
				m_recvBuf.Append(keep);
			} else {
				m_recvBuf.RemoveAll();
			}

			if (ss == SEC_E_OK) {
				if (QueryContextAttributes(&m_ctx, SECPKG_ATTR_STREAM_SIZES, &m_sizes) == SEC_E_OK)
					m_haveSizes = TRUE;
				m_complete = TRUE;
				// Leftover bytes after the handshake are early application data.
				if (m_recvBuf.GetSize() > 0) {
					extraAppData.Append(m_recvBuf);
					m_recvBuf.RemoveAll();
				}
				return TLS_DONE;
			}
			// CONTINUE: loop again only if extra bytes remain to process.
			if (extra <= 0)
				return TLS_CONTINUE;
			continue;
		}

		TRACE("TLS: handshake InitializeSecurityContext failed 0x%lx\n", ss);
		return TLS_ERROR;
	}
}

BOOL CTlsClient::Encrypt(const BYTE *plain, int len, CByteArray &cipher) {
	if (!m_complete || !m_haveSizes) return FALSE;

	int maxMsg = (int)m_sizes.cbMaximumMessage;
	BYTE *scratch = (BYTE *)malloc(m_sizes.cbHeader + maxMsg + m_sizes.cbTrailer);
	if (!scratch) return FALSE;

	int offset = 0;
	BOOL ok = TRUE;
	while (offset < len) {
		int chunk = min(maxMsg, len - offset);
		memcpy(scratch + m_sizes.cbHeader, plain + offset, chunk);

		SecBuffer bufs[4];
		bufs[0].pvBuffer = scratch;
		bufs[0].cbBuffer = m_sizes.cbHeader;
		bufs[0].BufferType = SECBUFFER_STREAM_HEADER;
		bufs[1].pvBuffer = scratch + m_sizes.cbHeader;
		bufs[1].cbBuffer = chunk;
		bufs[1].BufferType = SECBUFFER_DATA;
		bufs[2].pvBuffer = scratch + m_sizes.cbHeader + chunk;
		bufs[2].cbBuffer = m_sizes.cbTrailer;
		bufs[2].BufferType = SECBUFFER_STREAM_TRAILER;
		bufs[3].pvBuffer = NULL;
		bufs[3].cbBuffer = 0;
		bufs[3].BufferType = SECBUFFER_EMPTY;
		SecBufferDesc desc;
		desc.ulVersion = SECBUFFER_VERSION;
		desc.cBuffers = 4;
		desc.pBuffers = bufs;

		SECURITY_STATUS ss = EncryptMessage(&m_ctx, 0, &desc, 0);
		if (ss != SEC_E_OK) {
			TRACE("TLS: EncryptMessage failed 0x%lx\n", ss);
			ok = FALSE;
			break;
		}
		AppendBytes(cipher, (BYTE *)bufs[0].pvBuffer, bufs[0].cbBuffer);
		AppendBytes(cipher, (BYTE *)bufs[1].pvBuffer, bufs[1].cbBuffer);
		AppendBytes(cipher, (BYTE *)bufs[2].pvBuffer, bufs[2].cbBuffer);
		offset += chunk;
	}
	free(scratch);
	return ok;
}

BOOL CTlsClient::Decrypt(const BYTE *in, int inLen, CByteArray &plainOut, BOOL &renegotiate) {
	renegotiate = FALSE;
	if (!m_complete) return FALSE;
	if (inLen > 0) AppendBytes(m_recvBuf, in, inLen);

	while (m_recvBuf.GetSize() > 0) {
		SecBuffer bufs[4];
		bufs[0].pvBuffer = m_recvBuf.GetData();
		bufs[0].cbBuffer = m_recvBuf.GetSize();
		bufs[0].BufferType = SECBUFFER_DATA;
		for (int i = 1; i < 4; i++) {
			bufs[i].pvBuffer = NULL;
			bufs[i].cbBuffer = 0;
			bufs[i].BufferType = SECBUFFER_EMPTY;
		}
		SecBufferDesc desc;
		desc.ulVersion = SECBUFFER_VERSION;
		desc.cBuffers = 4;
		desc.pBuffers = bufs;

		SECURITY_STATUS ss = DecryptMessage(&m_ctx, &desc, 0, NULL);
		if (ss == SEC_E_INCOMPLETE_MESSAGE)
			return TRUE;	// wait for more ciphertext

		if (ss == SEC_E_OK) {
			SecBuffer *pData = NULL, *pExtra = NULL;
			for (int i = 0; i < 4; i++) {
				if (bufs[i].BufferType == SECBUFFER_DATA && !pData)  pData = &bufs[i];
				if (bufs[i].BufferType == SECBUFFER_EXTRA && !pExtra) pExtra = &bufs[i];
			}
			if (pData && pData->cbBuffer)
				AppendBytes(plainOut, (BYTE *)pData->pvBuffer, pData->cbBuffer);

			if (pExtra && pExtra->cbBuffer) {
				CByteArray keep;
				AppendBytes(keep, (BYTE *)pExtra->pvBuffer, pExtra->cbBuffer);
				m_recvBuf.RemoveAll();
				m_recvBuf.Append(keep);
				continue;	// another record is buffered
			}
			m_recvBuf.RemoveAll();
			return TRUE;
		}

		if (ss == SEC_I_RENEGOTIATE) {
			renegotiate = TRUE;
			m_recvBuf.RemoveAll();
			return TRUE;
		}
		if (ss == SEC_I_CONTEXT_EXPIRED) {
			m_recvBuf.RemoveAll();
			return FALSE;	// server closed the secure channel
		}

		TRACE("TLS: DecryptMessage failed 0x%lx\n", ss);
		return FALSE;
	}
	return TRUE;
}
