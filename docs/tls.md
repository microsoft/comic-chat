# Native TLS (SSL) support for Comic Chat over IRC

Comic Chat (1996) spoke plaintext IRC over an MFC `CAsyncSocket`. Modern IRC
networks expose TLS-only ports (e.g. `irc.libera.chat:6697`). This change adds
**native** TLS using Windows' built-in **SChannel** (SSPI) provider — no
external tunnel (stunnel/ZNC), no third-party crypto library, and no new
runtime dependency beyond `secur32.lib`, which ships with Windows.

> TL;DR: A small `CTlsClient` (in `tlssock.cpp`) wraps the SChannel handshake
> and record encrypt/decrypt. `CIrcSocket` gained a TLS state machine around the
> existing `OnConnect`/`OnReceive`/`Send`. A "Use SSL (TLS)" checkbox in the
> Connect dialog turns it on and defaults the port to 6697. Verified end-to-end
> against Libera (user mode `+Z` = "connected via TLS").

---

## 1. Why this was a good fit

All IRC traffic funnels through a single object, `static CIrcSocket serverConn`:

- one connect site (`InitializeServerConnection` → `serverConn.Connect`)
- ~12 `serverConn.Send(...)` call sites
- one receive path (`CIrcSocket::OnReceive` → `Receive`)

So TLS could be interposed at that choke point without touching the ~12 senders
or the line parser. `CAsyncSocket::Send` is **not** virtual, but every caller
uses the concrete `CIrcSocket` type, so a non-virtual `CIrcSocket::Send`
override is picked up everywhere.

## 2. Components

### `tlssock.h` / `tlssock.cpp` — `CTlsClient`
A minimal SChannel client that owns the credential + security context and
exposes four operations:

- `Begin(serverName, outToken)` — `AcquireCredentialsHandle` +
  first `InitializeSecurityContext` → produces the ClientHello to send.
- `Continue(in, outToken, extraAppData)` — feed received handshake bytes; drives
  `InitializeSecurityContext` to completion, returning `TLS_CONTINUE` /
  `TLS_DONE` / `TLS_ERROR` and emitting tokens to send.
- `Encrypt(plain, cipher)` — `EncryptMessage` into TLS records
  (chunked by `cbMaximumMessage`).
- `Decrypt(in, plainOut, renegotiate)` — `DecryptMessage`, handling record
  framing and leftovers.

### `ircsock.h` / `irc.cpp` — `CIrcSocket` TLS state machine
States: `IRC_TLS_NONE` / `IRC_TLS_HANDSHAKING` / `IRC_TLS_CONNECTED`.

- `OnConnect` (secure): create `CTlsClient`, `Begin`, send the ClientHello, enter
  HANDSHAKING. The IRC login (`NICK`/`USER`) is **deferred** to `SendLogin()`.
- `OnReceive`: read raw bytes with `CAsyncSocket::Receive`, then
  - HANDSHAKING → `Continue`; send tokens; on `TLS_DONE` call `SendLogin()` and
    decrypt any early app data.
  - CONNECTED → `Decrypt`, feed plaintext to `FeedPlainBytes` (the line splitter
    that was factored out of the old `OnReceive`).
- `Send`: when CONNECTED, `Encrypt` then `CAsyncSocket::Send`; while HANDSHAKING,
  queue plaintext and flush it from `SendLogin()`.

### Config + UI
- `CSetupDialog::m_bUseSSL` with `DDX_Check(IDC_USESSL)`, persisted to the
  registry as `UseSSL` next to `ShowComicView`.
- `GetMyUseSSL()` accessor; `InitializeServerConnection` calls
  `serverConn.SetSecure(GetMyUseSSL())` before `Connect`.
- A "Use SS&L (TLS)" checkbox in `IDD_SETUPDIALOG`. Toggling it flips the port
  between 6667 and 6697 (only when it still holds the other default, so a
  hand-typed port is never clobbered).

### Build
`chat.mak`: added `"$(INTDIR)\tlssock.obj"` to both `LINK32_OBJS` lists and
`secur32.lib` to both `LINK32_FLAGS`. The makefile's `.cpp{...}.obj` inference
rule compiles the new file automatically; `tlssock.cpp` includes `stdafx.h`
first to satisfy the precompiled-header build.

---

## 3. The one real gotcha: client-certificate requests

Libera's TLS listener **optionally** requests a client certificate (it supports
SASL EXTERNAL / CertFP). Two wrong turns and the right answer:

1. With `SCH_CRED_NO_DEFAULT_CREDS`, `InitializeSecurityContext` returns
   **`SEC_I_INCOMPLETE_CREDENTIALS` (0x00090320)** to let the app supply a cert.
   If you treat that as an error, the handshake fails.
2. **Removing** `SCH_CRED_NO_DEFAULT_CREDS` makes SChannel try to *satisfy* the
   request with a default credential — which on a machine with smart cards pops a
   Windows **"Select a smart card"** prompt. Not what we want.

**Correct handling:** keep `SCH_CRED_NO_DEFAULT_CREDS` (so SChannel never
auto-selects/prompts) **and** handle `SEC_I_INCOMPLETE_CREDENTIALS` by simply
**re-calling `InitializeSecurityContext`** with the same buffered handshake data.
On the retry SChannel sends an *empty* client certificate and the handshake
proceeds. A small retry guard prevents an infinite loop.

```cpp
if (ss == SEC_I_INCOMPLETE_CREDENTIALS) {
    if (++m_incompleteCredRetries > 4) return TLS_ERROR;
    continue;   // re-invoke ISC; SChannel sends an empty client cert
}
```

## 4. Other notes / limitations (spike scope)

- **Certificate validation is permissive.** `SCH_CRED_MANUAL_CRED_VALIDATION` is
  set and we do not currently walk/verify the server chain (many IRC servers use
  community CAs or self-signed certs). For production, validate the chain with
  `CertGetCertificateChain` / `CertVerifyCertificateChainPolicy` (link
  `crypt32.lib`) and surface failures to the user.
- **No renegotiation / no client-cert auth (CertFP).** `SEC_I_RENEGOTIATE` is
  flagged but not driven; client-cert SASL EXTERNAL is out of scope.
- **Async integration.** A single TLS record can span multiple `OnReceive`
  events or carry several IRC lines, so `Decrypt`/`Continue` buffer partial
  records (`SEC_E_INCOMPLETE_MESSAGE`) and carry `SECBUFFER_EXTRA` across calls.
- **Still ANSI.** The transport is a byte stream, so Comic Chat's `char*` world
  is unaffected.

## 5. How to verify

1. Build: `nmake /f chat.mak CFG="chat - Win32 Debug"`.
2. Run `chat.exe`; in **Connect**, tick **Use SSL (TLS)** (port auto-fills 6697),
   server `irc.libera.chat`, and connect.
3. You should register normally and be able to join channels. On Libera, confirm
   user mode includes **`+Z`** (TLS) — visible in the DbgView trace as
   `:<nick> MODE <nick> :+Ziw`.
4. DbgView shows `Got message:` lines flowing (i.e. decrypt is working) with no
   smart-card prompt and no `TLS: ... failed` traces.
