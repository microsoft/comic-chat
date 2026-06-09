# Microsoft Comic Chat

Microsoft Comic Chat is a graphical IRC chat client developed at Microsoft in 1996. Unlike traditional text-based chat clients, Comic Chat renders conversations as comic strips in real time — placing participants as cartoon characters with word balloons, expressions, and gestures automatically determined by a built-in expert system.

![Comic Chat](src/readme.gif)

## How It Works

As users type messages, each Comic Chat client automatically determines:

- Which characters to place in each panel
- Gestures and facial expressions based on message content
- Character placement and orientation
- Word balloon shape, outline, and layout
- When to advance to a new panel
- Zoom factor for each panel

The application connects to standard IRC servers and is fully interoperable with text-based IRC clients — non-Comic Chat users are automatically assigned characters so the entire conversation is rendered graphically.

## Building

The project targets Win32 (x86) using Visual C++ 4.x with MFC. Build with NMAKE:

```batch
REM Release build (MFC statically linked)
NMAKE /f "chat.mak" CFG="chat - Win32 Release"

REM Debug build (MFC shared DLL)
NMAKE /f "chat.mak" CFG="chat - Win32 Debug"
```

Alternatively, open `chat.mdp` in Visual C++ 4.x.

### Requirements

- Visual C++ 4.x (or compatible NMAKE toolchain)
- MFC 4.x libraries
- Windows 95 or Windows NT 4.0 (original target)
- 486 processor, 8 MB RAM, 256-color video (minimum)

## Repository Structure

```
src/
├── chat.mak          # NMAKE makefile
├── chat.mdp          # Visual C++ 4.x project file
├── chat.cpp/h        # Application entry point (CChatApp)
├── chatdoc.cpp/h     # MFC document (chat session state)
├── chatview.cpp/h    # Comic strip view
├── textview.cpp/h    # Plain text fallback view
├── mainfrm.cpp/h     # Main frame window
├── irc.cpp           # IRC protocol implementation
├── chatprot.cpp/h    # IRC protocol parsing
├── semantic.cpp      # Comic layout expert system
├── avatar.cpp/h      # Character rendering
├── balloon.cpp/h     # Word balloon generation
├── panel.cpp/h       # Comic panel composition
├── backdrop.cpp/h    # Scene backgrounds
├── comicart/
│   ├── avatars/      # Character art assets
│   └── backdrop/     # Background scene images
└── res/              # Icons, bitmaps, toolbar art
```

## History

Comic Chat was released as part of Internet Explorer 3.0 in 1996 and could run standalone or embedded as an OLE server within the browser. It was developed by the Microsoft Research group and represented a novel approach to graphical chat by applying comics conventions to real-time conversation.

## License

This project is licensed under the [MIT License](LICENSE.md).
