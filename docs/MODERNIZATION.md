# Modernizing Microsoft Comic Chat (2026)

This document is the landing page for the work done to get the 1996 Microsoft
Comic Chat MFC client to **build, run, and look right on a modern high-DPI
Windows machine** — and to optionally talk TLS to today's IRC networks. It links
to the detailed write-ups and summarizes every change in one place.

| Area | Detail doc |
|------|-----------|
| High-DPI rendering + UI scaling + UX polish | [`dpi-awareness.md`](dpi-awareness.md) |
| Native TLS over IRC (SChannel) | [`tls.md`](tls.md) *(on the `tls-schannel` branch / PR)* |
| Modern build of the 2.5 beta-1 client | [`../v2.5-beta-1-modern/README.md`](../v2.5-beta-1-modern/README.md) |

---

## 1. Building with the modern VC++ / MFC toolchain

The project still uses its original `nmake` makefile (`src/chat.mak`), but it now
compiles and links with a current Visual Studio C++/MFC toolchain:

```bat
call "<VS>\VC\Auxiliary\Build\vcvars32.bat"
cd src
nmake /f chat.mak CFG="chat - Win32 Debug"
```

What had to change to build under a modern compiler:

- **Legacy C++ that no longer compiles**
  - For-loop variable scoping: `for (i = 0; ...)` reusing an out-of-scope `i`
    → `for (int i = 0; ...)`.
  - Trailing `inline` placement (`void f() inline { }` → `inline void f() { }`).
  - Const-correctness on `strchr`/`strrchr` results assigned to `char*`.
  - A missing type on `extern bCXKeepServer` → `extern BOOL bCXKeepServer`.
  - A richedit `ENLINK` / `CFE_LINK` redefinition guarded with `#ifndef`.
  - An `ID_EM_*` string-id collision with modern `afxres.h`.
- **MFC/OLE binder macro clashes** (`DECLARE_OLECMD_MAP` / `BEGIN_OLECMD_MAP`)
  renamed to project-local macros.
- **Linker**: link `uuid.lib` (the makefile referenced a long-gone `uuid3.lib`)
  plus the spectre-mitigated ATLMFC libs; stub the missing **Lernout & Hauspie
  TTS** wrapper (`lhwrap.dll`'s `initproc` / `ttsproc` / `exitproc`) that the
  original `chat.exe` imported for text-to-speech.

> Behavioral note: a couple of `/FORCE`-style and `nodefaultlib` quirks of the
> original makefile are preserved; see `chat.mak`.

## 2. High-DPI: making it render correctly

Comic Chat draws the comic page in device-independent **TWIPs**, but the rest of
the UI (member-list icons, the bodycam emotion wheel, the Say box and toolbar)
uses hardcoded 96-DPI pixels. Full story in [`dpi-awareness.md`](dpi-awareness.md);
in brief:

- **Declare DPI awareness** (`SetProcessDPIAware`) so the app renders at native
  resolution instead of being bitmap-stretched.
- **Comic panels**: size the off-screen panel bitmap from the DC's *real*
  `LPtoDP` extent (not `GetDeviceCaps`), rebuild it when that extent changes,
  and blit with `StretchBlt` as a guard. Fixes panels clipped on the right/bottom.
- **Pixel-based surfaces**: a single `DpiScale(n)` helper (seeded once from the
  screen DPI) scales the member-list face icons, the bodycam emotion wheel and
  its face icons, the Say-box font, the four action-balloon toolbar buttons, and
  the Say pane's minimum height. `DpiScale(n) == n` at 96 DPI, so standard-DPI
  users are unaffected.
- **Rejected** uniform OS scaling (DPI virtualization / GDI Scaling): the latter
  opened the window gigantic because Comic Chat persists window geometry in raw
  pixels.

## 3. UX polish

- **Mouse-wheel scrolling** in the comic area (`CPageView::OnMouseWheel`).
- **Panels-per-row auto-fit**: the column count now fits the window width
  (`FitPanelsWide`) instead of a fixed/persisted 2; the reflow is deferred via a
  posted message to avoid a `SetPanelsWide → WM_SIZE` re-entrancy crash.
- **Balloon word-wrap**: a single word is no longer split mid-character to fit a
  narrow estimate (so "Test" no longer renders as "Tes" / "t"); the balloon grows
  to keep the word whole.

## 4. Native TLS over IRC (separate branch)

Comic Chat (1996) predates TLS and speaks only plaintext IRC, so it can't reach
TLS-only ports such as `irc.libera.chat:6697`. The `tls-schannel` branch adds
**native TLS via the built-in Windows SChannel/SSPI provider** — no external
tunnel (stunnel/ZNC), no third-party crypto library. A "Use SSL (TLS)" checkbox
in the Connect dialog turns it on (and defaults the port to 6697). Full design,
the client-certificate gotcha, and the permissive-validation caveat are in
[`tls.md`](tls.md).

## 5. Verification

All of the above was verified end-to-end on a 144-DPI (150%) display: build,
launch, connect to Libera, comic view with an uncut title, correctly-scaled
member faces / avatar preview / emotion wheel, legible Say box, mouse-wheel
scroll, 3+ panels per row, and clean balloon word-wrap. Standard-DPI behavior is
unchanged.

## 6. The 2.5 beta-1 client (`v2.5-beta-1-modern/`)

Everything above targets the `v1.0-pre-modern/` tree (the v1.0-pre client). The
repository also contains the more advanced **Microsoft Chat 2.5 beta-1** (June
1998) source, which originally built with the **Windows NT DDK `BUILD.EXE`**
system rather than `nmake`.

[`v2.5-beta-1-modern/`](../v2.5-beta-1-modern) is a copy of that tree brought up
on the modern toolchain with its own clean `nmake` makefile, so it **builds and
runs** today:

```bat
call "<VS>\VC\Auxiliary\Build\vcvars32.bat"
cd v2.5-beta-1-modern
nmake /f chat.mak CFG="chat - Win32 Debug"
```

Highlights of the bring-up (full list in the folder's
[`README.md`](../v2.5-beta-1-modern/README.md)):

- A new `chat.mak` replacing the DDK `sources`/`dirs` build; `MIDL` for the COM
  proxy; the delay-load DLL thunks compiled in; `wininet.lib` linked.
- The same class of legacy-C++ cleanups as `v1.0-pre-modern`, plus 2.5-specific
  ones (`INT8/16/32` SDK collision, `MyGetCommandMap`, socket-handle reattach, …).
- **Two real blockers**: removing the MFC-4.0 `tagLVITEMA → _LV_ITEMA` remap from
  `stdafx.h` (it broke linking against modern MFC common controls), and adding a
  **Common Controls v6 manifest** so the rebar toolbar's `RB_INSERTBAND` succeeds
  (that failure was the startup crash).
- Re-created the missing `chatver.h` / `chatver.rc` version stamp.
- The DPI, mouse-wheel and panels-per-row work above, ported across.

Not ported: native TLS transport (2.5's SSPI is for IRC *authentication*, not
SChannel) and main-toolbar glyph scaling. 2.5's balloon wrapper is already
international/word-aware, so the `src` word-wrap fix is unnecessary there.
