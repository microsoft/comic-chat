# Microsoft Comic Chat

Microsoft Comic Chat is a Microsoft-developed internet chat client released in 1996 that transformed traditional text-based conversations into automatically generated comic strips. Instead of plain text, users communicate through cartoon avatars, with messages displayed in speech bubbles inside dynamically created comic panels that conveyed gestures, emotions, and narrative flow. Built as an IRC client and originally a Microsoft Research project, it uses algorithms to mimic comic storytelling techniques, such as panel layout and character expressions, to make online communication more visual and expressive. It was later bundled with Internet Explorer and MSN before being discontinued in the early 2000s.

![Comic Chat](v1.0-pre/readme.gif)

## How It Works

As users type messages, each Comic Chat client automatically determines:

- Which characters to place in each panel
- Gestures and facial expressions based on message content
- Character placement and orientation
- Word balloon shape, outline, and layout
- When to advance to a new panel
- Zoom factor for each panel

The application connects to standard IRC servers and is fully interoperable with text-based IRC clients. Non-Comic Chat users are automatically assigned characters so the entire conversation is rendered graphically.

## Repository Structure

This repository contains source snapshots from multiple versions of Comic Chat:

```
v1.0/               # Comic Chat 1.0 (August 1996, shipped with IE 3.0)
│   ├── client/     # Win32 MFC client source
│   ├── help/       # Help file source
│   ├── setup/      # IExpress installer
│   └── shared/     # Redistributable DLLs and fonts
v2.1b/              # Comic Chat 2.1 beta (February 1998)
v2.5-beta-1/        # Comic Chat 2.5 beta 1 (June 1998)
artifacts/          # Companion tools: avatar editor, Java client, SDK
file dates.txt      # Original file timestamps from archive
```

## Building

The project targets Win32 (x86) using Visual C++ 4.x with MFC.

**Comic Chat 1.0** — source is in `v1.0/client/`:

```batch
cd v1.0\client
REM Release build (MFC statically linked)
NMAKE /f "chat.mak" CFG="chat - Win32 Release"

REM Debug build (MFC shared DLL)
NMAKE /f "chat.mak" CFG="chat - Win32 Debug"
```

Alternatively, open `chat.mdp` in Visual C++ 4.x. For **Comic Chat 2.5 beta 1**, source is in `v2.5-beta-1/` and uses a Visual C++ 5.x project (`chat.dsp`).

### Requirements

- Visual C++ 4.x (v1.0) or Visual C++ 5.x (v2.5-beta-1)
- MFC 4.x libraries
- Windows 95 or Windows NT 4.0 (original target)
- 486 processor, 8 MB RAM, 256-color video (minimum)

## History

Comic Chat was originally a Microsoft Research project and shipped as part of Internet Explorer 3.0 in August 1996. It could run standalone or embedded as an OLE server within the browser. Version 2.0 added NetMeeting integration and was bundled with Internet Explorer 4.0 and Windows 98. The 2.5 beta added further protocol support and UI improvements before the product was eventually discontinued in the early 2000s.

## License

This project is licensed under the [MIT License](LICENSE).

## Contributing

This project welcomes contributions and suggestions.  Most contributions require you to agree to a
Contributor License Agreement (CLA) declaring that you have the right to, and actually do, grant us
the rights to use your contribution. For details, visit https://cla.opensource.microsoft.com.

When you submit a pull request, a CLA bot will automatically determine whether you need to provide
a CLA and decorate the PR appropriately (e.g., status check, comment). Simply follow the instructions
provided by the bot. You will only need to do this once across all repos using our CLA.

This project has adopted the [Microsoft Open Source Code of Conduct](https://opensource.microsoft.com/codeofconduct/).
For more information see the [Code of Conduct FAQ](https://opensource.microsoft.com/codeofconduct/faq/) or
contact [opencode@microsoft.com](mailto:opencode@microsoft.com) with any additional questions or comments.

## Trademarks

This project may contain trademarks or logos for projects, products, or services. Authorized use of Microsoft 
trademarks or logos is subject to and must follow 
[Microsoft's Trademark & Brand Guidelines](https://www.microsoft.com/en-us/legal/intellectualproperty/trademarks/usage/general).
Use of Microsoft trademarks or logos in modified versions of this project must not cause confusion or imply Microsoft sponsorship.
Any use of third-party trademarks or logos are subject to those third-party's policies.