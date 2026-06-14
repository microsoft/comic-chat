# Binary Files

This repository contains binary files that were part of the original Comic Chat source archives. They are preserved as-is from the historical snapshots. This document catalogs all binary files by category.

---

## Character Art (`.avb`)

Avatar binary files contain Comic Chat character artwork — sprite sheets, gestures, expressions, and animation data. Each `.avb` file corresponds to one named character.

| Version | Location | Characters |
|---------|----------|------------|
| v1.0-pre | `v1.0-pre/comicart/avatars/` | anna, armando, bolo, connor, cro, dan, denise, glenda, hugh, jordan, lance, lynnea, margaret, mike, pedagog, rainbow, susan, tiki, tongtyed, tux, waf, xeno |
| v1.0 | `v1.0/client/comicart/avatars/` | anna, armando, bolo, connor, cro, dan, denise, glenda, hugh, jordan, lance, lynnea, margaret, mike, pedagog, rainbow, susan, tiki, tongtyed, tux, waf, xeno |
| v2.1b | `v2.1b/cchat/comicart/` | anna, armando, bolo, buck, connor, cro, dan, denise, glenda, hugh, jordan, kirby, lance, lynnea, margaret, mike, pedagog, rainbow, susan, tiki, tongtyed, tux, veronica, waf, xeno |
| v2.1b Art Pack 1 | `v2.1b/cchat/artpack1/` | bolo, cro, denise, kevin, kwensa, lynnea, maynard, rebecca, sage, scotty |
| v2.5-beta-1 | `v2.5-beta-1/comicart/` | anna, armando, bolo, buck, connor, cro, dan, denise, glenda, hugh, jordan, kirby, lance, lynnea, margaret, mike, pedagog, rainbow, susan, tiki, tongtyed, tux, veronica, waf, xeno |
| v2.5-beta-1 Art Pack 1 | `v2.5-beta-1/artpack1/` | bolo, cro, denise, kevin, kwensa, lynnea, maynard, rebecca, sage, scotty |
| v2.5-beta-1 Art Pack 1 Archive | `v2.5-beta-1/artpack1/archive/` | bolo, cro, denise, kevin, kwensa, lynnea, maynard, rebecca, sage, scotty |

---

## Backdrop Art (`.bgb`)

Backdrop binary files contain background scene artwork displayed behind characters in comic panels. Present only in v2.5-beta-1 (earlier versions used `.bmp` backdrops).

| File | Description |
|------|-------------|
| `v2.5-beta-1/comicart/buckroom.bgb` | Buck's room scene |
| `v2.5-beta-1/comicart/clouds.bgb` | Sky/clouds scene |
| `v2.5-beta-1/comicart/field.bgb` | Outdoor field scene |
| `v2.5-beta-1/comicart/pastoral.bgb` | Pastoral outdoor scene |
| `v2.5-beta-1/comicart/room.bgb` | Indoor room scene |
| `v2.5-beta-1/comicart/space.bgb` | Space scene |
| `v2.5-beta-1/comicart/yellow.bgb` | Yellow abstract scene |
| `v2.5-beta-1/artpack1/den.bgb` | Den scene (Art Pack 1) |
| `v2.5-beta-1/artpack1/volcano.bgb` | Volcano scene (Art Pack 1) |

---

## Image Resources (`.bmp`, `.gif`, `.jpg`, `.ico`, `.rle`, `.dib`, `.cur`)

UI bitmaps, toolbar images, icons, and backdrop images embedded in application resources.

### Application UI / Resources

| Version | Location | Count | Contents |
|---------|----------|-------|----------|
| v1.0-pre | `v1.0-pre/res/` | 9 | App icon, document icon, toolbar bitmaps, balloon sprites, splash screen |
| v1.0 | `v1.0/client/res/` | 9 | App icon, document icon, toolbar bitmaps, balloon sprites, splash screen |
| v2.1b | `v2.1b/cchat/res/` | 36 | Full UI icon set, toolbar bitmaps, cursors |
| v2.5-beta-1 | `v2.5-beta-1/res/` | 52 | Full UI icon set, toolbar bitmaps, cursors, high-color resources |

### Avatar Thumbnails and Face Tiles

| Version | Location | Count | Contents |
|---------|----------|-------|----------|
| v1.0-pre | `v1.0-pre/comicart/avatars/` | 21 | Character face tiles (`fc_*.bmp`), frog animation frame |
| v1.0 | `v1.0/client/comicart/avatars/` | 21 | Character face tiles (`fc_*.bmp`), frog animation frame |
| v2.1b | `v2.1b/cchat/comicart/` | 7 | Character selection thumbnails |

### Backdrop Bitmaps (v1.0-pre and v1.0)

| File | Description |
|------|-------------|
| `v1.0-pre/comicart/backdrop/field.bmp` | Outdoor field background |
| `v1.0-pre/comicart/backdrop/pastoral.bmp` | Pastoral outdoor background |
| `v1.0-pre/comicart/backdrop/room8bs.bmp` | Indoor room background |
| `v1.0/client/comicart/backdrop/field.bmp` | Outdoor field background |
| `v1.0/client/comicart/backdrop/pastoral.bmp` | Pastoral outdoor background |
| `v1.0/client/comicart/backdrop/room8bs.bmp` | Indoor room background |

### Readme / Documentation Images

| File | Description |
|------|-------------|
| `v1.0-pre/readme.gif` | Comic Chat screenshot used in README |
| `v1.0/client/readme.gif` | Comic Chat screenshot used in README |

### SDK, JChat, and Companion Tool Images

| Location | Count | Contents |
|----------|-------|----------|
| `artifacts/inc/` | 5 | Shared header icons (host, participant, me, etc.) |
| `artifacts/mschatpr/` | 3 | MSChat PR tool images |
| `artifacts/avtools/res/` | 3 | Avatar Filer tool icons and toolbar |
| `artifacts/bettybot/` | 1 | Betty bot icon |
| `artifacts/xcchat/` | 12 | xcchat companion tool resources |
| `v2.1b/xcchat/` | 12 | xcchat companion tool resources |
| `v2.1b/inc/` | 5 | Shared header icons |
| `v2.1b/mschatpr/` | 3 | MSChat PR tool images |
| `artifacts/jchat/html/images/` | 13 | JChat web client HTML images |
| `artifacts/jchat/script/images/` | 34 | JChat scripting sample images |
| `artifacts/jchat/script/simple/` | 3 | JChat simple script sample images |
| `artifacts/jchat/hashjava/doc/images/` | 24 | HashJava documentation images |
| `artifacts/sdk/help/art/` | 7 | SDK help documentation art |
| `artifacts/sdk/samples/*/res/` | ~40 | SDK sample application resources |
| `v2.1b/sdk/help/art/` | 7 | SDK help documentation art |
| `v2.1b/sdk/samples/*/res/` | ~28 | SDK sample application resources |

---

## Compiled Help Files (`.hlp`)

Windows compiled help files (WinHelp format). Source `.rtf` and `.hpj` files are included alongside these in `help/` subdirectories.

| File | Version |
|------|---------|
| `v1.0-pre/cchat.hlp` | v1.0 prerelease |
| `v1.0/client/cchat.hlp` | v1.0 |
| `v1.0/help/cchat.hlp` | v1.0 (help build output) |
| `v2.1b/cchat/cchat.hlp` | v2.1 beta |
| `v2.1b/help/cchat.hlp` | v2.1 beta (help build output) |
| `v2.5-beta-1/cchat.hlp` | v2.5 beta 1 |
| `artifacts/help/cchat.hlp` | Artifacts snapshot |

---

## Fonts (`.ttf`)

| File | Description |
|------|-------------|
| `v1.0/shared/comic.ttf` | Comic Sans MS TrueType font, shipped with Comic Chat 1.0 |
| `v2.1b/cchat/setup/comic.ttf` | Comic Sans MS TrueType font, shipped with Comic Chat 2.1 beta |

---

## Compiled Executables (`.exe`)

### Comic Chat Application Binaries

| File | Description |
|------|-------------|
| `v1.0-pre/chat.exe` | Compiled Comic Chat 1.0 prerelease client |
| `v2.5-beta-1/base/objd/i386/CChat.exe` | Compiled Comic Chat 2.5 beta 1 debug build (x86) |

### Build Utility Executables

These tools were used in the build process for the v1.0 installer.

| File | Description |
|------|-------------|
| `v1.0/build/delvcreg.exe` | Removes VC++ runtime registry entries (build helper) |
| `v1.0/build/showtime.exe` | Build timing utility |
| `v1.0/setup/diantz.exe` | Cabinet compression tool (Microsoft) |
| `v1.0/setup/iexpress.exe` | IExpress self-extracting installer builder |
| `v1.0/setup/wextract.exe` | Windows self-extraction tool |

### SDK and Companion Tool Executables

Prebuilt executable tools from the Comic Chat SDK and companion utilities.

| File | Description |
|------|-------------|
| `artifacts/bettybot/Betty.exe` | "Betty Bot" VB-based IRC bot demo |
| `artifacts/sdk/chat1j/mschat1j.exe` | MSChat1J Java integration tool |
| `artifacts/sdk/chat1j/protocol.exe` | Chat protocol analyzer tool |
| `artifacts/sdk/chatcntl.exe` | Chat control ActiveX sample tool |
| `artifacts/sdk/chatsdk.exe` | Chat SDK installer |
| `artifacts/sdk/chatsdk2.exe` | Chat SDK 2.0 installer |
| `artifacts/sdk/samples/exec2/exec2.exe` | ExecChat sample compiled binary |
| `artifacts/sdk/script1/script1.exe` | Script1 sample compiled binary |
| `v2.1b/sdk/chat1j/mschat1j.exe` | MSChat1J Java integration tool (v2.1b) |
| `v2.1b/sdk/chat1j/protocol.exe` | Chat protocol analyzer tool (v2.1b) |
| `v2.1b/sdk/chatcntl.exe` | Chat control ActiveX sample tool (v2.1b) |
| `v2.1b/sdk/chatsdk.exe` | Chat SDK installer (v2.1b) |
| `v2.1b/sdk/chatsdk2.exe` | Chat SDK 2.0 installer (v2.1b) |
| `v2.1b/sdk/samples/exec2/exec2.exe` | ExecChat sample compiled binary (v2.1b) |

---

## Debug Symbols and Linker Outputs

| File | Description |
|------|-------------|
| `v2.5-beta-1/base/objd/i386/CChat.dbg` | Debug symbol file for CChat.exe |
| `v2.5-beta-1/base/objd/i386/CChat.sym` | Symbol map for CChat.exe |
| `v2.5-beta-1/base/objd/i386/chat.res` | Compiled resource file for v2.5 beta 1 |
| `artifacts/xcchat/objd/i386/xcchat.exp` | xcchat linker export file |
| `artifacts/xcchat/objd/i386/xcchat.lib` | xcchat import library |
| `artifacts/xcchat/objd/i386/xcchat.res` | xcchat compiled resource file |

---

## DLLs and OCX Controls (`.dll`, `.ocx`)

### Redistributable Runtime DLLs

Shipped with the Comic Chat installer to satisfy runtime dependencies on Windows 95/NT.

| File | Description |
|------|-------------|
| `v1.0/shared/msvcrt40.dll` | Visual C++ 4.0 runtime (MSVCRT) |
| `v1.0/shared/riched32.dll` | Rich Edit control DLL |
| `v1.0/setup/advpack.dll` | IE4 advanced pack setup helper |
| `v1.0/setup/w95inf16.dll` | Windows 95 INF installer (16-bit) |
| `v1.0/setup/w95inf32.dll` | Windows 95 INF installer (32-bit) |
| `v2.1b/cchat/setup/msvcrt40.dll` | Visual C++ 4.0 runtime (MSVCRT) |
| `v2.1b/cchat/setup/riched32.dll` | Rich Edit control DLL |

### Chat Library DLLs

| File | Description |
|------|-------------|
| `artifacts/lib/chatsock.dll` | ChatSocket network library DLL |
| `v2.1b/lib/chatsock.dll` | ChatSocket network library DLL (v2.1b) |

### SDK OCX Controls and Third-Party DLLs

| File | Description |
|------|-------------|
| `artifacts/bettybot/MSVBVM50.DLL` | Visual Basic 5 runtime (for Betty.exe) |
| `artifacts/sdk/samples/exec2/nmstart.ocx` | NetMeeting startup OCX used by ExecChat |
| `artifacts/sdk/samples/prchckrs/html/playbd.ocx` | Playboard OCX for property checker sample |
| `v2.1b/sdk/samples/exec2/nmstart.ocx` | NetMeeting startup OCX (v2.1b) |
| `v2.1b/sdk/samples/prchckrs/html/playbd.ocx` | Playboard OCX (v2.1b) |

---

## Static Libraries (`.lib`)

| File | Description |
|------|-------------|
| `artifacts/lib/i386/zlib.lib` | zlib compression library (x86) |
| `artifacts/lib/alpha/zlib.lib` | zlib compression library (Alpha AXP) |
| `artifacts/lib/chatsock.lib` | ChatSocket import library |
| `v2.1b/lib/chatsock.lib` | ChatSocket import library (v2.1b) |
| `artifacts/jchat/jdk114/lib/javai.lib` | Java VM import library (JDK 1.1.4) |
| `artifacts/jchat/jdk114/lib/javai_g.lib` | Java VM debug import library (JDK 1.1.4) |
| `artifacts/jchat/sdk20/Lib/i386/javadbg.lib` | Java debug library (SDK 2.0) |
| `artifacts/jchat/sdk20/Lib/i386/msjava.lib` | Microsoft Java VM import library |

---

## Java Class Files and Archives

JChat (Java-based Comic Chat client) and related Java tools.

| Location | Count | Description |
|----------|-------|-------------|
| `artifacts/jchat/chat1j/` | ~8 | JChat client compiled classes |
| `artifacts/jchat/chat1j/ob/` | ~12 | JChat compiled output |
| `artifacts/jchat/hashjava/sbktech/tools/hashjava/*/` | ~175 | HashJava utility compiled classes |
| `artifacts/jchat/proxy/ob/` | ~12 | JChat proxy compiled output |
| `artifacts/jchat/script/ob/` | ~20 | JChat script engine compiled output |
| `artifacts/jchat/html/ob/` | ~12 | JChat HTML applet compiled output |
| `artifacts/sdk/chat1j/` | ~8 | SDK JChat classes |
| `artifacts/jchat/jdk114/lib/classes.zip` | 1 | JDK 1.1.4 standard library archive |
| `artifacts/sdk/samples/script/ob/` | ~4 | Script sample compiled output |

---

## Visual Basic Form Resources (`.frx`)

Binary resource files associated with VB `.frm` form files. Each `.frx` is paired with a corresponding `.frm` source file in the same directory.

| File | Associated Form |
|------|----------------|
| `artifacts/bettybot/Betty.frx` | Betty bot main form |
| `artifacts/sdk/samples/chanfltr/main.frx` | Channel filter sample |
| `artifacts/sdk/samples/checkrs/checker.frx` | Checker sample |
| `artifacts/sdk/samples/exec2/about.frx` | ExecChat about dialog |
| `artifacts/sdk/samples/exec2/answer.frx` | ExecChat answer dialog |
| `artifacts/sdk/samples/exec2/main.frx` | ExecChat main form |
| `artifacts/sdk/samples/exec2/pretext.frx` | ExecChat pretext dialog |
| `artifacts/sdk/samples/exec2/roomrule.frx` | ExecChat room rules dialog |
| `artifacts/sdk/samples/exec2/rooms.frx` | ExecChat rooms dialog |
| `artifacts/sdk/samples/execchat/execc.frx` | ExecChat core form |
| `artifacts/sdk/samples/execchat/sendtor.frx` | ExecChat send-to room form |
| `artifacts/sdk/samples/execchat/whisptog.frx` | ExecChat whisper toggle form |
| `artifacts/sdk/samples/execchat/whisptom.frx` | ExecChat whisper-to-me form |
| `artifacts/sdk/samples/trnscrpt/form2.frx` | Transcript sample form |
| `v2.1b/sdk/samples/checkrs/checker.frx` | Checker sample (v2.1b) |
| `v2.1b/sdk/samples/exec2/about.frx` | ExecChat about dialog (v2.1b) |
| `v2.1b/sdk/samples/exec2/answer.frx` | ExecChat answer dialog (v2.1b) |
| `v2.1b/sdk/samples/exec2/main.frx` | ExecChat main form (v2.1b) |
| `v2.1b/sdk/samples/exec2/options.frx` | ExecChat options dialog (v2.1b) |
| `v2.1b/sdk/samples/exec2/pretext.frx` | ExecChat pretext dialog (v2.1b) |
| `v2.1b/sdk/samples/exec2/warnings.frx` | ExecChat warnings dialog (v2.1b) |
| `v2.1b/sdk/samples/execchat/execc.frx` | ExecChat core form (v2.1b) |
| `v2.1b/sdk/samples/execchat/sendtor.frx` | ExecChat send-to room form (v2.1b) |
| `v2.1b/sdk/samples/execchat/whisptog.frx` | ExecChat whisper toggle form (v2.1b) |
| `v2.1b/sdk/samples/execchat/whisptom.frx` | ExecChat whisper-to-me form (v2.1b) |
| `v2.1b/sdk/samples/trnscrpt/form2.frx` | Transcript sample form (v2.1b) |

---

## OCA Type Library Cache (`.oca`)

OCA files are auto-generated type library caches created by Visual Basic when registering an OCX. Paired with `.ocx` files.

| File | Description |
|------|-------------|
| `artifacts/sdk/samples/exec2/nmstart.oca` | Cached type library for nmstart.ocx |
| `v2.1b/sdk/samples/exec2/nmstart.oca` | Cached type library for nmstart.ocx (v2.1b) |

---

## Visual C++ Binary Project Files (`.mdp`)

Microsoft Developer Studio project files in binary format (VC++ 4.x).

| File | Description |
|------|-------------|
| `v1.0-pre/chat.mdp` | VC++ 4.x project for Comic Chat (prerelease) |
| `v1.0/client/chat.mdp` | VC++ 4.x project for Comic Chat 1.0 |

---

## Documents (`.doc`, `.xls`, `.obd`)

Design and specification documents from the original development team.

| File | Description |
|------|-------------|
| `v1.0/doc/content/contdev.doc` | Content development guide (Word document) |
| `artifacts/docs/procx.xls` | Process/protocol specification (Excel) |
| `artifacts/docs/uiocx.xls` | UI OCX specification (Excel) |
| `artifacts/docs/cchat/avfiles.obd` | Avatar file format documentation (OBD) |
| `v2.1b/docs/procx.xls` | Process/protocol specification (v2.1b) |
| `v2.1b/docs/uiocx.xls` | UI OCX specification (v2.1b) |

---

## NLS Locale Data (`.nls`)

Windows National Language Support codepage tables for Russian localization.

| File | Description |
|------|-------------|
| `artifacts/russian/c_20866.nls` | KOI8-R codepage map |
| `artifacts/russian/cp_20866.nls` | KOI8-R codepage data |
| `v2.1b/russian/c_20866.nls` | KOI8-R codepage map (v2.1b) |
| `v2.1b/russian/cp_20866.nls` | KOI8-R codepage data (v2.1b) |

---

## Miscellaneous

| File | Description |
|------|-------------|
| `artifacts/jchat/iedcache.slm.v6` | IE disk cache SLM snapshot (v6 format) |
| `artifacts/jchat/jdk114/lib/DeIsL1.isu` | InstallShield uninstall log (JDK 1.1.4 installer) |
| `artifacts/jchat/jdk114/lib/DeIsL2.isu` | InstallShield uninstall log (JDK 1.1.4 installer) |
| `artifacts/sdk/samples/exec2/chatui.ctx` | Help context file for ExecChat sample |
| `v2.1b/sdk/samples/exec2/chatui.ctx` | Help context file for ExecChat sample (v2.1b) |
| `v2.1b/cchat/setup/choice.com` | DOS `CHOICE` command (used in setup batch scripts) |
