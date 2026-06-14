# Comic Chat — v1.0 Source (August 1996)

This folder contains the source code for **Microsoft Comic Chat 1.0**, archived from August 1996. It shares the same internal version identifier as `v1.0-pre` (`rup 206, "Beta 2"`, built on `DJKLAPTOP`), but includes the full build infrastructure, installer, help system, and shared runtime libraries that were absent from the pre-release snapshot. Approximately 99 of the 111 shared source files differ between the two snapshots.

## Structure

```
v1.0/
├── client/       # Main application source (~116 files)
│   ├── comicart/
│   │   ├── avatars/  # Character art (22 characters)
│   │   └── backdrop/ # Background scenes (3 backdrops)
│   └── res/          # Icons, bitmaps, toolbar art
├── build/        # Build scripts (build.bat, build.mak) and tools
├── doc/          # Internal documentation
├── help/         # Help source (cchat.rtf, cchat.hpj) and compiled help
├── setup/        # Installer configuration (.cdf, .inf, .reg, IExpress tools)
└── shared/       # Shared runtime files (comic.ttf, msvcrt40.dll, riched32.dll)
```

## Characters (22)

Anna, Armando, Bolo, Connor, Cro, Dan, Denise, Glenda, Hugh, Jordan, Lance, Lynnea, Margaret, Mike, Pedagog, Rainbow, Susan, Tiki, Tongtyed, Tux, Waf, Xeno

## Backdrops (3)

Field, Pastoral, Room

## Building

```batch
cd v1.0\client
NMAKE /f "chat.mak" CFG="chat - Win32 Release"
```

Or use the top-level build wrapper:

```batch
cd v1.0\build
build exe
```

Requires Visual C++ 4.x (`vcvars32.bat`) and MFC 4.x. Targets Windows 95 / Windows NT 4.0 (x86). Open `client/chat.mdp` in Visual C++ 4.x for IDE use.

## Notable additions over v1.0-pre

- **Build system** — `build/build.bat` and `build.mak` automate full rebuild and drop to a share
- **Installer** — `setup/` contains IExpress-based installer configuration (`.cdf`, `.inf`)
- **Help system** — `help/` contains the compiled help file and RTF source (`cchat.rtf`)
- **Shared runtimes** — `shared/` bundles `comic.ttf`, `msvcrt40.dll`, and `riched32.dll`
- **Internal docs** — `doc/content/contdev.doc` contains internal developer content documentation
