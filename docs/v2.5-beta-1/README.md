# Comic Chat — v2.5 Beta 1 Source (June 1998)

This folder contains the source code for **Microsoft Comic Chat 2.5 Beta 1**, archived from June 1998. This is the latest source snapshot in the repository. Compared to v2.1b, this version adds IRCX protocol support, RTF-formatted text, file transfer, channel properties, sound notifications, whisper boxes, and a modernized Visual Studio 97 project structure.

## Structure

Unlike v2.1b's multi-folder layout, v2.5-beta-1 uses a mostly flat source tree with two subdirectories:

```
v2.5-beta-1/
├── *.cpp / *.h       # Source files (~92 translation units)
├── chat.dsp          # Visual Studio 97 project file
├── chat.dsw          # Visual Studio 97 workspace file
├── cchat.hlp         # Compiled help file
├── artpack1/         # Art Pack 1 character assets (archived originals)
├── base/             # OLE/COM interface definitions (IDL)
├── comicart/         # Character art (31 characters) and backdrops
│   └── avatars/      # .avb character files
├── res/              # Icons, bitmaps, toolbar art
└── setup/            # Installer configuration
```

## Characters (31)

**Original (21):** Anna, Armando, Bolo, Connor, Cro, Dan, Denise, Glenda, Hugh, Jordan, Lance, Lynnea, Margaret, Mike, Pedagog, Rainbow, Susan, Tiki, Tongtyed, Tux, Xeno

**Art Pack 1 (10):** Buck, Kevin, Kirby, Kwensa, Lynnea, Maynard, Rebecca, Sage, Scotty, Veronica, Waf

## Backdrops (9)

Backdrops use the new `.bgb` format (replacing `.bmp`): Buck's Room, Clouds, Den, Field, Pastoral, Room, Space, Volcano, Yellow

## Building

```batch
cd v2.5-beta-1
NMAKE /f "chat.mak" CFG="chat - Win32 Release"
```

Or open `chat.dsw` in Visual Studio 97 (Visual C++ 5.0). Requires MFC. Targets Windows 95 / Windows NT 4.0 (x86).

## Notable additions over v2.1b

- **IRCX protocol** — `ircproto.cpp` / `nmproto.cpp` add Microsoft's IRCX extensions (authenticated channels, structured data)
- **RTF text** — `rtfcmb.cpp` / `rtfctrl.cpp` enable rich-text formatting in the say box
- **File transfer** — `filesend.cpp` / `filesend.h`
- **Channel properties** — `chanprop.cpp` / `chanprop.h`
- **Sound notifications** — `sounddlg.cpp` / `sounddlg.h`
- **Whisper boxes** — `whisprbx.cpp` / `whisprbx.h`
- **Tab bars and coolbars** — `tabbar.cpp` / `coolbar.cpp` for the modernized toolbar UI
- **Auto-paging** — `autopage.cpp` automatically pages through comic history
- **`.bgb` backdrop format** — new compressed background format replacing plain `.bmp`
- **Visual Studio 97 project** — `chat.dsp` / `chat.dsw` replace the `.mdp` format from v2.1b
