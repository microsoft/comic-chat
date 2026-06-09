# Copilot Instructions for Microsoft Comic Chat

## Architecture

This is **Microsoft Comic Chat** (circa 1996) — a Win32 MFC (Microsoft Foundation Classes) IRC client that renders chat conversations as comic strips in real time. The source lives entirely in `src/`.

### Core subsystems

- **IRC protocol layer** — `irc.cpp`, `ircsock.h`, `chatprot.cpp/h` handle IRC socket communication and protocol parsing.
- **Comic rendering engine** — The heart of the application. An expert system (`semantic.cpp`) determines panel composition, character placement, gestures, expressions, balloon layout, and zoom. Key files:
  - `avatar.cpp/h`, `avatario.cpp/h` — Character loading and rendering
  - `balloon.cpp/h` — Word balloon generation and layout
  - `panel.cpp/h`, `pageview.cpp/h` — Comic panel/page composition
  - `backdrop.cpp/h` — Scene backgrounds
  - `spline.cpp/h`, `splinutl.cpp` — Spline math for balloon outlines
  - `traj.cpp/h`, `vector2d.cpp/h` — Character positioning and trajectories
  - `textpose.cpp` — Text layout within balloons
- **MFC document/view framework** — `chatdoc.cpp/h` (document), `chatview.cpp/h` (comic view), `textview.cpp/h` (plain text fallback view), `mainfrm.cpp/h` (frame window).
- **OLE/COM embedding** — `binddoc.cpp/h`, `binditem.cpp/h`, `bindipfw.cpp/h`, `oleobjct.cpp`, `mfcbind.cpp/h` enable the app to run as an OLE server embedded in Internet Explorer 3.0.
- **UI dialogs** — `setupdlg`, `profdlg`, `avatardl`, `admindlg`, `bothdlg`, `proppage` — connection setup, user profile, character selection, and room administration.

### Data directories

- `src/comicart/avatars/` — Character art assets
- `src/comicart/backdrop/` — Background scene images
- `src/res/` — Application resources (icons, bitmaps, toolbar art)

## Build System

The project uses a Visual C++ 4.x NMAKE makefile (`src/chat.mak`). It targets Win32 x86 with MFC statically linked (Release) or as a shared DLL (Debug).

```
NMAKE /f "chat.mak" CFG="chat - Win32 Release"
NMAKE /f "chat.mak" CFG="chat - Win32 Debug"
```

There is also a `.mdp` (Visual C++ 4.x project) file for IDE use. No modern CMake, MSBuild `.vcxproj`, or Visual Studio solution file exists.

## Conventions

- **Precompiled headers** — All `.cpp` files must `#include "stdafx.h"` first. `chat.h` enforces this with a compile-time check.
- **Units** — Drawing coordinates use TWIPs (1440 units per inch), defined in `common.h`.
- **MFC patterns** — Classes follow MFC conventions: `C`-prefixed class names, `AFX_MSG` message maps, ClassWizard `//{{AFX_` markers. Do not remove ClassWizard delimiters.
- **OLE class factory** — The app registers as an OLE server (`chat.reg`); keep registration entries consistent if modifying COM interfaces.
- **Asset paths** — Character art is loaded from `ComicArt\Avatars` and backdrops from `ComicArt\Backdrop` (hardcoded in `chat.h`).
- **Character encoding** — All strings are ANSI `char*` (no Unicode/wchar_t support).
