# Microsoft Developer Studio Generated NMAKE File, Format Version 4.20
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

!IF "$(CFG)" == ""
CFG=Chat2000 - Win32 Debug
!MESSAGE No configuration specified.  Defaulting to Chat2000 - Win32 Debug.
!ENDIF 

!IF "$(CFG)" != "Chat2000 - Win32 Release" && "$(CFG)" !=\
 "Chat2000 - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE on this makefile
!MESSAGE by defining the macro CFG on the command line.  For example:
!MESSAGE 
!MESSAGE NMAKE /f "chat2000.mak" CFG="Chat2000 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Chat2000 - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Chat2000 - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 
################################################################################
# Begin Project
# PROP Target_Last_Scanned "Chat2000 - Win32 Debug"
RSC=rc.exe
CPP=cl.exe
MTL=mktyplib.exe

!IF  "$(CFG)" == "Chat2000 - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 5
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
OUTDIR=.\Release
INTDIR=.\Release

ALL : "$(OUTDIR)\chat2000.exe"

CLEAN : 
	-@erase "$(INTDIR)\BanDlg.obj"
	-@erase "$(INTDIR)\C2Doc.obj"
	-@erase "$(INTDIR)\C2View.obj"
	-@erase "$(INTDIR)\Chat2000.obj"
	-@erase "$(INTDIR)\chat2000.pch"
	-@erase "$(INTDIR)\Chat2000.res"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\EnterDlg.obj"
	-@erase "$(INTDIR)\InfoDlg.obj"
	-@erase "$(INTDIR)\KickBan.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\mschat.obj"
	-@erase "$(INTDIR)\Partic.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TopicDlg.obj"
	-@erase "$(OUTDIR)\chat2000.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)/chat2000.pch" /Yu"stdafx.h" /Fo"$(INTDIR)/" /c 
CPP_OBJS=.\Release/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
MTL_PROJ=/nologo /D "NDEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "NDEBUG"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chat2000.res" /d "NDEBUG" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chat2000.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 /nologo /subsystem:windows /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:no\
 /pdb:"$(OUTDIR)/chat2000.pdb" /machine:I386 /out:"$(OUTDIR)/chat2000.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BanDlg.obj" \
	"$(INTDIR)\C2Doc.obj" \
	"$(INTDIR)\C2View.obj" \
	"$(INTDIR)\Chat2000.obj" \
	"$(INTDIR)\Chat2000.res" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\EnterDlg.obj" \
	"$(INTDIR)\InfoDlg.obj" \
	"$(INTDIR)\KickBan.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\mschat.obj" \
	"$(INTDIR)\Partic.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TopicDlg.obj"

"$(OUTDIR)\chat2000.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Chat2000 - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
OUTDIR=.\Debug
INTDIR=.\Debug

ALL : "$(OUTDIR)\chat2000.exe"

CLEAN : 
	-@erase "$(INTDIR)\BanDlg.obj"
	-@erase "$(INTDIR)\C2Doc.obj"
	-@erase "$(INTDIR)\C2View.obj"
	-@erase "$(INTDIR)\Chat2000.obj"
	-@erase "$(INTDIR)\chat2000.pch"
	-@erase "$(INTDIR)\Chat2000.res"
	-@erase "$(INTDIR)\ChildFrm.obj"
	-@erase "$(INTDIR)\EnterDlg.obj"
	-@erase "$(INTDIR)\InfoDlg.obj"
	-@erase "$(INTDIR)\KickBan.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\mschat.obj"
	-@erase "$(INTDIR)\Partic.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TopicDlg.obj"
	-@erase "$(INTDIR)\vc40.idb"
	-@erase "$(INTDIR)\vc40.pdb"
	-@erase "$(OUTDIR)\chat2000.exe"
	-@erase "$(OUTDIR)\chat2000.ilk"
	-@erase "$(OUTDIR)\chat2000.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /c
CPP_PROJ=/nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chat2000.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\.
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
MTL_PROJ=/nologo /D "_DEBUG" /win32 
# ADD BASE RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
RSC_PROJ=/l 0x409 /fo"$(INTDIR)/Chat2000.res" /d "_DEBUG" /d "_AFXDLL" 
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
BSC32_FLAGS=/nologo /o"$(OUTDIR)/chat2000.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 /nologo /subsystem:windows /debug /machine:I386
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)/chat2000.pdb" /debug /machine:I386\
 /out:"$(OUTDIR)/chat2000.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BanDlg.obj" \
	"$(INTDIR)\C2Doc.obj" \
	"$(INTDIR)\C2View.obj" \
	"$(INTDIR)\Chat2000.obj" \
	"$(INTDIR)\Chat2000.res" \
	"$(INTDIR)\ChildFrm.obj" \
	"$(INTDIR)\EnterDlg.obj" \
	"$(INTDIR)\InfoDlg.obj" \
	"$(INTDIR)\KickBan.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\mschat.obj" \
	"$(INTDIR)\Partic.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TopicDlg.obj"

"$(OUTDIR)\chat2000.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_OBJS)}.obj:
   $(CPP) $(CPP_PROJ) $<  

.c{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cpp{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

.cxx{$(CPP_SBRS)}.sbr:
   $(CPP) $(CPP_PROJ) $<  

################################################################################
# Begin Target

# Name "Chat2000 - Win32 Release"
# Name "Chat2000 - Win32 Debug"

!IF  "$(CFG)" == "Chat2000 - Win32 Release"

!ELSEIF  "$(CFG)" == "Chat2000 - Win32 Debug"

!ENDIF 

################################################################################
# Begin Source File

SOURCE=.\ReadMe.txt

!IF  "$(CFG)" == "Chat2000 - Win32 Release"

!ELSEIF  "$(CFG)" == "Chat2000 - Win32 Debug"

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chat2000.cpp
DEP_CPP_CHAT2=\
	".\C2Doc.h"\
	".\C2View.h"\
	".\Chat2000.h"\
	".\ChildFrm.h"\
	".\globals.h"\
	".\MainFrm.h"\
	".\mschat.h"\
	".\partic.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\Chat2000.obj" : $(SOURCE) $(DEP_CPP_CHAT2) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Chat2000 - Win32 Release"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS"\
 /Fp"$(INTDIR)/chat2000.pch" /Yc"stdafx.h" /Fo"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chat2000.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ELSEIF  "$(CFG)" == "Chat2000 - Win32 Debug"

# ADD CPP /Yc"stdafx.h"

BuildCmds= \
	$(CPP) /nologo /MDd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_AFXDLL" /D "_MBCS" /Fp"$(INTDIR)/chat2000.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)/" /Fd"$(INTDIR)/" /c $(SOURCE) \
	

"$(INTDIR)\StdAfx.obj" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

"$(INTDIR)\chat2000.pch" : $(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
   $(BuildCmds)

!ENDIF 

# End Source File
################################################################################
# Begin Source File

SOURCE=.\MainFrm.cpp
DEP_CPP_MAINF=\
	".\Chat2000.h"\
	".\MainFrm.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\ChildFrm.cpp
DEP_CPP_CHILD=\
	".\Chat2000.h"\
	".\ChildFrm.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\ChildFrm.obj" : $(SOURCE) $(DEP_CPP_CHILD) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\C2Doc.cpp
DEP_CPP_C2DOC=\
	".\C2Doc.h"\
	".\C2View.h"\
	".\Chat2000.h"\
	".\globals.h"\
	".\mschat.h"\
	".\partic.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\C2Doc.obj" : $(SOURCE) $(DEP_CPP_C2DOC) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\C2View.cpp
DEP_CPP_C2VIE=\
	".\BanDlg.h"\
	".\C2Doc.h"\
	".\C2View.h"\
	".\Chat2000.h"\
	".\EnterDlg.h"\
	".\globals.h"\
	".\InfoDlg.h"\
	".\KickBan.h"\
	".\MainFrm.h"\
	".\mschat.h"\
	".\partic.h"\
	".\StdAfx.h"\
	".\TopicDlg.h"\
	

"$(INTDIR)\C2View.obj" : $(SOURCE) $(DEP_CPP_C2VIE) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Chat2000.rc
DEP_RSC_CHAT20=\
	".\res\C2Doc.ico"\
	".\res\Chat2000.ico"\
	".\res\Chat2000.rc2"\
	".\res\Toolbar.bmp"\
	

"$(INTDIR)\Chat2000.res" : $(SOURCE) $(DEP_RSC_CHAT20) "$(INTDIR)"
   $(RSC) $(RSC_PROJ) $(SOURCE)


# End Source File
################################################################################
# Begin Source File

SOURCE=.\mschat.cpp
DEP_CPP_MSCHA=\
	".\globals.h"\
	".\mschat.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\mschat.obj" : $(SOURCE) $(DEP_CPP_MSCHA) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\EnterDlg.cpp
DEP_CPP_ENTER=\
	".\Chat2000.h"\
	".\EnterDlg.h"\
	".\globals.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\EnterDlg.obj" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\KickBan.cpp
DEP_CPP_KICKB=\
	".\Chat2000.h"\
	".\KickBan.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\KickBan.obj" : $(SOURCE) $(DEP_CPP_KICKB) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\Partic.cpp
DEP_CPP_PARTI=\
	".\globals.h"\
	".\partic.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\Partic.obj" : $(SOURCE) $(DEP_CPP_PARTI) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\BanDlg.cpp
DEP_CPP_BANDL=\
	".\BanDlg.h"\
	".\Chat2000.h"\
	".\globals.h"\
	".\partic.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\BanDlg.obj" : $(SOURCE) $(DEP_CPP_BANDL) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\InfoDlg.cpp
DEP_CPP_INFOD=\
	".\Chat2000.h"\
	".\InfoDlg.h"\
	".\StdAfx.h"\
	

"$(INTDIR)\InfoDlg.obj" : $(SOURCE) $(DEP_CPP_INFOD) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
################################################################################
# Begin Source File

SOURCE=.\TopicDlg.cpp
DEP_CPP_TOPIC=\
	".\Chat2000.h"\
	".\StdAfx.h"\
	".\TopicDlg.h"\
	

"$(INTDIR)\TopicDlg.obj" : $(SOURCE) $(DEP_CPP_TOPIC) "$(INTDIR)"\
 "$(INTDIR)\chat2000.pch"


# End Source File
# End Target
# End Project
################################################################################
################################################################################
# Section Chat2000 : {D6526FE0-E651-11CF-99CB-00C04FD64497}
# 	0:10:MSChat.cpp:F:\Random Projects\Chat2000\MSChat.cpp
# 	0:8:MSChat.h:F:\Random Projects\Chat2000\MSChat.h
# 	2:21:DefaultSinkHeaderFile:mschat.h
# 	2:16:DefaultSinkClass:CMSChat
# End Section
################################################################################
################################################################################
# Section Chat2000 : {D3E56780-E651-11CF-99CB-00C04FD64497}
# 	2:5:Class:CMSChat
# 	2:10:HeaderFile:mschat.h
# 	2:8:ImplFile:mschat.cpp
# End Section
################################################################################
################################################################################
# Section OLE Controls
# 	{D6526FE0-E651-11CF-99CB-00C04FD64497}
# End Section
################################################################################
