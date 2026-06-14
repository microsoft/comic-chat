# Comic Chat — v2.1 Beta Source (February 1998)

This folder contains the source code for a beta build of **Microsoft Comic Chat 2.1**, archived from February 1998. This version shipped with Internet Explorer 4.0 and added OLE automation scripting, multi-server support, an SDK for third-party bots, and Art Pack 1 (10 new characters).

## Repository Structure

```
v2.1b/
├── cchat/        # Main application source
│   ├── artpack1/ # Art Pack 1 character assets (10 new characters)
│   ├── base/     # OLE/COM interface definitions (IDL)
│   ├── comicart/ # Original character art (21 characters) and backdrops
│   └── res/      # Icons, bitmaps, toolbar art
├── core/         # Shared core library
├── sdk/          # Comic Chat SDK (OLE automation, samples, docs)
│   ├── bin/      # Pre-built SDK binaries
│   ├── chat1j/   # JChat Java client source
│   ├── help/     # SDK documentation
│   ├── include/  # Public headers
│   └── samples/  # Sample bots and extensions (VB, IE4)
├── avated/       # Avatar editor tool source
├── docs/         # Internal design documents
├── help/         # User help content
├── inc/          # Shared include files
├── lib/          # Shared libraries
├── mschatpr/     # MSChat ActiveX control
├── nmstart/      # NetMeeting integration
├── russian/      # Russian localization
├── setup/        # Installer
├── webmeet/      # Web-based meeting integration (IE4 demo, VB demo)
└── xcchat/       # xcchat ActiveX control (embeds Comic Chat in a web page)
```

## Characters (31)

**Original (21):** Anna, Armando, Bolo, Connor, Cro, Dan, Denise, Glenda, Hugh, Jordan, Lance, Lynnea, Margaret, Mike, Pedagog, Rainbow, Susan, Tiki, Tongtyed, Tux, Waf, Xeno

**Art Pack 1 (10):** Buck, Kevin, Kirby, Kwensa, Maynard, Rebecca, Sage, Scotty, Veronica, Waf

## Backdrops (7)

Buck's Room, Clouds, Field, Pastoral, Room, Space, Yellow

## Building

```batch
cd v2.1b\cchat
NMAKE /f "chat.mak" CFG="chat - Win32 Release"
```

Requires Visual C++ 4.x and MFC 4.x. Targets Windows 95 / Windows NT 4.0 (x86).

## Notable additions over v1.0

- **OLE automation** — `icchat.idl` / `icbcore.idl` define the scripting interface for bots and extensions
- **Comic Chat SDK** — `sdk/` contains headers, pre-built binaries, VB and IE4 samples, and the JChat Java client
- **xcchat ActiveX control** — embeds Comic Chat in a web page (`xcchat/xcchat.idl`)
- **Art Pack 1** — 10 additional characters in `cchat/artpack1/`
- **WebMeet** — prototype web-based meeting integration (`webmeet/`)
- **Russian localization** — `russian/` subfolder
