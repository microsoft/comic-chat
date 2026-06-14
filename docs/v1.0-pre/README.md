# Comic Chat — v1.0 Pre-release Source (August 1996)

This folder contains the source code for a pre-release build of **Microsoft Comic Chat**, archived from August 1996. The internal version identifier is `rup 206, "Beta 2"`, built on `DJKLAPTOP`. This is the earliest source snapshot in this repository, predating the v1.0 release build.

## Contents

```
client/
├── *.cpp / *.h       # Source files (~60 translation units)
├── chat.mak          # NMAKE makefile (Visual C++ 4.x)
├── chat.mdp          # Visual C++ 4.x project file
├── chat.exe          # Pre-built binary (August 20, 1996)
├── cchat.hlp         # Compiled help file
├── comicart/
│   ├── avatars/      # Character art (22 characters)
│   └── backdrop/     # Background scenes (3 backdrops)
└── res/              # Icons, bitmaps, toolbar art
```

## Characters (22)

Anna, Armando, Bolo, Connor, Cro, Dan, Denise, Glenda, Hugh, Jordan, Lance, Lynnea, Margaret, Mike, Pedagog, Rainbow, Susan, Tiki, Tongtyed, Tux, Waf, Xeno

## Backdrops (3)

Field, Pastoral, Room

## Building

```batch
cd v1.0-pre\client
NMAKE /f "chat.mak" CFG="chat - Win32 Release"
```

Requires Visual C++ 4.x and MFC 4.x. Targets Windows 95 / Windows NT 4.0 (x86).

## Notes

- `readme.txt` contains the original July 1996 README distributed with the beta
- `readme.htm` / `readme.gif` are the HTML version of the readme with screenshot
- `lhwrap.dll` / `lhwrap.lib` are LH (LZH compression) wrapper libraries included in the original archive
- The `Debug/` build output folder is excluded via `.gitignore`
