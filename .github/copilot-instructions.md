# Copilot Instructions for Microsoft Comic Chat

## Repository Layout

This is a historical source archive. Each top-level folder is a standalone snapshot — files are not shared between versions.

| Folder | Version | Toolchain |
|---|---|---|
| `v1.0-pre/` | 1.0 prerelease (internal) | Visual C++ 4.x |
| `v1.0/client/` | Comic Chat 1.0 (Aug 1996, shipped with IE 3.0) | Visual C++ 4.x |
| `v2.1b/cchat/` | Comic Chat 2.1 beta (Feb 1998) | Visual C++ 5.x |
| `v2.5-beta-1/` | Comic Chat 2.5 beta 1 (Jun 1998) | Visual C++ 5.x |
| `artifacts/` | Companion tools: avatar editor, Java client, SDK | various |

`file dates.txt` records original file modification timestamps from the archive.

## Build System

No modern build system exists. All versions use NMAKE makefiles targeting Win32 x86.

**v1.0** (from `v1.0/client/`):
```bat
NMAKE /f "chat.mak" CFG="chat - Win32 Release"
NMAKE /f "chat.mak" CFG="chat - Win32 Debug"
```
Open `chat.mdp` in Visual C++ 4.x for IDE use.

**v2.5-beta-1** (from `v2.5-beta-1/`):
Open `chat.dsp` in Visual C++ 5.x. The `.dsp`/`.dsw` project format replaced `.mdp` in VC5.

## Architecture

### Comic rendering pipeline

The core of the application is a rule-based expert system that turns IRC messages into comic panels in real time:

1. **`semantic.cpp`** — Central expert system. Given an incoming message, it decides: which characters appear, their gestures/expressions, balloon type and shape, panel zoom, and whether to advance to a new panel. This is the largest and most complex file.
2. **`traj.cpp/h`**, **`vector2d.cpp/h`** — 2D character positioning and trajectory math.
3. **`avatar.cpp/h`**, **`avatario.cpp/h`** — Character sprite loading and rendering. In v2.5, `avbfile.cpp/h` adds direct `.avb` binary format parsing (shared with `artifacts/avtools/`).
4. **`balloon.cpp/h`** — Word balloon generation: outline splines, tail geometry, and text layout.
5. **`spline.cpp/h`**, **`splinutl.cpp`** — Spline math used for balloon outlines.
6. **`panel.cpp/h`**, **`pageview.cpp/h`** — Comic panel layout and page composition.
7. **`backdrop.cpp/h`** — Background scene rendering.
8. **`textpose.cpp`** — Text layout within balloons.

### MFC document/view framework

Follows standard MFC architecture:
- `chatdoc.cpp/h` — Document (holds chat session state)
- `chatview.cpp/h` — Primary view (renders the comic strip)
- `textview.cpp/h` — Plain-text fallback view (for non-comic mode)
- `mainfrm.cpp/h` — Main SDI frame window

In v2.5, `childfrm.cpp/h` adds MDI child frame support, and `chatbars.cpp/h` + `coolbar.cpp/h` + `tabbar.cpp/h` manage the multi-toolbar/tabbed UI.

### IRC / protocol layer

- **v1.0**: Single protocol — `irc.cpp`, `ircsock.h`, `chatprot.cpp/h`
- **v2.5**: Multi-protocol abstraction — `ircproto.cpp/h` (IRC), `nmproto.cpp/h` (NetMeeting/CB32), `protsupp.cpp/h` (shared support), `chatsrv.cpp/h` (server abstraction). The `#define CB32SUPPORT` in `nmproto.h` gates the NetMeeting path.

### OLE/COM embedding

`binddoc.cpp/h`, `binditem.cpp/h`, `bindipfw.cpp/h`, `oleobjct.cpp`, `mfcbind.cpp/h` implement an OLE Document Server so the app can be embedded in Internet Explorer 3.0/4.0. `chat.reg` / `oldchat.reg` contain the COM registration entries. `mschat.h` (v2.5) is the public COM automation interface.

### New in v2.5 vs v1.0

- **`actions.cpp/h`** — Scripted automation actions triggered by channel events
- **`rules.cpp/h`** — Channel rule sets
- **`notif.cpp/h`**, **`notipage.cpp/h`** — Notification system
- **`rtfctrl.cpp/h`**, **`rtfcmb.cpp/h`** — RTF text rendering in text view
- **`userlist.cpp/h`**, **`userinfo.cpp`** — User list management (extracted from document)
- **`mcithrd.cpp/h`** — MCI background thread for sound playback
- **`webreq.cpp/h`** — HTTP requests (avatar/backdrop downloads)
- **`filesend.cpp/h`** — File transfer
- **`jis2sjis.cpp`**, **`sjis2jis.cpp`**, **`intl.c/h`** — Japanese (Shift-JIS) and internationalization support

### Companion tools (`artifacts/`)

- **`avtools/`** — Avatar editor (`avatarfiler`), AVB file converter (`avbcvt`), record appender (`addrec`). Uses `avbfile.cpp/h` shared with v2.5.
- **`sdk/`** — Public Chat SDK with COM interfaces (`mschatpr.idl`), samples, and documentation
- **`jchat/`** — Java IRC client companion
- **`bettybot/`** — Automated chat bot
- **`xcchat/`** — ActiveX control embedding host

## Conventions

- **Precompiled headers** — All `.cpp` files must `#include "stdafx.h"` as the very first include. `chat.h` enforces this with a compile-time `#error`.
- **Units** — All drawing coordinates are in TWIPs (1440 per inch). Defined in `common.h` (v1.0) / `defines.h` (v2.5).
- **MFC class naming** — `C`-prefixed class names (`CChatDoc`, `CBalloon`, etc.), `AFX_MSG` message maps, `ON_COMMAND` / `ON_UPDATE_COMMAND_UI` macros.
- **ClassWizard markers** — `//{{AFX_` and `//}}AFX_` comment blocks are maintained by ClassWizard. Do not add or remove code inside these delimiters manually.
- **OLE registration** — Keep `chat.reg` and COM interface GUIDs consistent when modifying automation interfaces.
- **Asset paths** — Character art loads from `ComicArt\Avatars` and backdrops from `ComicArt\Backdrop` (hardcoded, backslash-separated Windows paths).
- **Character encoding** — All strings are ANSI `char*`. No Unicode or `wchar_t`. Japanese support uses runtime Shift-JIS conversion via `jis2sjis.cpp`.
- **AVB file format** — The `.avb` (avatar) and `.bgb` (background) binary formats are documented implicitly in `avbfile.h`. The `AVATAR_NOT_CLIENT` preprocessor flag enables `avbfile.cpp` for use in tools outside the chat client.
