# chat.mak - modern nmake makefile for Microsoft Chat (v2.5-beta-1-modern)
# Builds CChat.exe with a current Visual Studio C++/MFC (static) toolchain,
# replacing the original NT DDK BUILD.EXE (sources/dirs) system.
#
#   call "<VS>\VC\Auxiliary\Build\vcvars32.bat"
#   nmake /f chat.mak CFG="chat - Win32 Debug"
#
# Notes:
#  - Precompiled headers are intentionally disabled for robustness.
#  - Delay-load helper (dlylddll.c) and NetMeeting (nmproto) are excluded.
#  - icchat_i.c / icchat.h are generated from base\icchat.idl via MIDL.

!IF "$(CFG)" == ""
CFG=chat - Win32 Debug
!ENDIF

OUTDIR=.\Debug
INTDIR=.\Debug

CPP=cl.exe
RSC=rc.exe
MIDL=midl.exe
LINK32=link.exe

ARTINC=..\artifacts\inc
ARTLIB=..\artifacts\lib\i386
SPECTRE=C:\Program Files\Microsoft Visual Studio\18\Community\VC\Tools\MSVC\14.51.36231\ATLMFC\lib\spectre\x86

CPP_PROJ=/nologo /MTd /W3 /GX /Zi /Od /Zc:forScope- /Zc:strictStrings- /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" \
 /I "." /I "$(ARTINC)" /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /c

RSC_PROJ=/l 0x409 /fo"$(INTDIR)\chat.res" /i "." /i "$(ARTINC)" /d "_DEBUG"

LINK32_FLAGS=/nologo /subsystem:windows /FORCE:MULTIPLE /incremental:no /debug \
 /machine:I386 /nodefaultlib:"libc" \
 /LIBPATH:"$(SPECTRE)" /LIBPATH:"$(ARTLIB)" \
 uuid.lib secur32.lib comctl32.lib ole32.lib oleaut32.lib oldnames.lib wsock32.lib \
 shell32.lib winmm.lib imm32.lib winspool.lib comdlg32.lib oledlg.lib wininet.lib zlib.lib \
 /out:"$(OUTDIR)\CChat.exe"

OBJS= \
	"$(INTDIR)\stdafx.obj" \
	"$(INTDIR)\dlylddll.obj" \
	"$(INTDIR)\actions.obj" \
	"$(INTDIR)\admindlg.obj" \
	"$(INTDIR)\arc.obj" \
	"$(INTDIR)\autopage.obj" \
	"$(INTDIR)\avatar.obj" \
	"$(INTDIR)\avatario.obj" \
	"$(INTDIR)\avbfile.obj" \
	"$(INTDIR)\backdrop.obj" \
	"$(INTDIR)\balloon.obj" \
	"$(INTDIR)\bbox.obj" \
	"$(INTDIR)\binddcmt.obj" \
	"$(INTDIR)\binddoc.obj" \
	"$(INTDIR)\bindipfw.obj" \
	"$(INTDIR)\binditem.obj" \
	"$(INTDIR)\bindtarg.obj" \
	"$(INTDIR)\bindview.obj" \
	"$(INTDIR)\bindauto.obj" \
	"$(INTDIR)\bodycam.obj" \
	"$(INTDIR)\ccommon.obj" \
	"$(INTDIR)\ccomp.obj" \
	"$(INTDIR)\chanprop.obj" \
	"$(INTDIR)\chat.obj" \
	"$(INTDIR)\chatbars.obj" \
	"$(INTDIR)\chatDoc.obj" \
	"$(INTDIR)\ChatItem.obj" \
	"$(INTDIR)\chatsrv.obj" \
	"$(INTDIR)\chatView.obj" \
	"$(INTDIR)\chicdial.obj" \
	"$(INTDIR)\childfrm.obj" \
	"$(INTDIR)\colordlg.obj" \
	"$(INTDIR)\coolbar.obj" \
	"$(INTDIR)\dib.obj" \
	"$(INTDIR)\doskey.obj" \
	"$(INTDIR)\filesend.obj" \
	"$(INTDIR)\fonts.obj" \
	"$(INTDIR)\format.obj" \
	"$(INTDIR)\histent.obj" \
	"$(INTDIR)\IpFrame.obj" \
	"$(INTDIR)\ircproto.obj" \
	"$(INTDIR)\ircsock.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\memblst.obj" \
	"$(INTDIR)\mfcbind.obj" \
	"$(INTDIR)\motd.obj" \
	"$(INTDIR)\notif.obj" \
	"$(INTDIR)\notipage.obj" \
	"$(INTDIR)\oleobjct.obj" \
	"$(INTDIR)\PageView.obj" \
	"$(INTDIR)\panel.obj" \
	"$(INTDIR)\print.obj" \
	"$(INTDIR)\proppage.obj" \
	"$(INTDIR)\protsupp.obj" \
	"$(INTDIR)\query.obj" \
	"$(INTDIR)\RoomList.obj" \
	"$(INTDIR)\rtfcmb.obj" \
	"$(INTDIR)\rtfctrl.obj" \
	"$(INTDIR)\rules.obj" \
	"$(INTDIR)\saywnd.obj" \
	"$(INTDIR)\setupdlg.obj" \
	"$(INTDIR)\sounddlg.obj" \
	"$(INTDIR)\spline.obj" \
	"$(INTDIR)\splinutl.obj" \
	"$(INTDIR)\spltchat.obj" \
	"$(INTDIR)\status.obj" \
	"$(INTDIR)\tabbar.obj" \
	"$(INTDIR)\textcore.obj" \
	"$(INTDIR)\textpose.obj" \
	"$(INTDIR)\textview.obj" \
	"$(INTDIR)\traj.obj" \
	"$(INTDIR)\txtfntdg.obj" \
	"$(INTDIR)\urlutil.obj" \
	"$(INTDIR)\userinfo.obj" \
	"$(INTDIR)\userlist.obj" \
	"$(INTDIR)\utils.obj" \
	"$(INTDIR)\vector2d.obj" \
	"$(INTDIR)\webreq.obj" \
	"$(INTDIR)\whisprbx.obj" \
	"$(INTDIR)\jis2sjis.obj" \
	"$(INTDIR)\sjis2jis.obj" \
	"$(INTDIR)\mcithrd.obj" \
	"$(INTDIR)\intl.obj" \
	"$(INTDIR)\icchat_i.obj" \
	"$(INTDIR)\chat.res"

ALL : "$(OUTDIR)\CChat.exe"

"$(INTDIR)" :
	if not exist "$(INTDIR)/$(NULL)" mkdir "$(INTDIR)"

# ---- COM proxy from IDL (generates icchat.h + icchat_i.c at root) ----
icchat_i.c icchat.h : base\icchat.idl
	$(MIDL) /nologo /I "$(ARTINC)" /h icchat.h /iid icchat_i.c base\icchat.idl

# ---- Link ----
"$(OUTDIR)\CChat.exe" : "$(INTDIR)" icchat.h $(OBJS)
	$(LINK32) @<<
$(LINK32_FLAGS) $(OBJS)
<<

# ---- Resource ----
"$(INTDIR)\chat.res" : chat.rc
	$(RSC) $(RSC_PROJ) chat.rc

# ---- C++ sources (no PCH) ----
{.}.cpp{$(INTDIR)}.obj:
	$(CPP) $(CPP_PROJ) $<

# ---- C sources ----
{.}.c{$(INTDIR)}.obj:
	$(CPP) $(CPP_PROJ) $<
