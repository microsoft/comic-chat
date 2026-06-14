# Microsoft Developer Studio Generated NMAKE File, Based on Chappy.dsp
!IF "$(CFG)" == ""
CFG=Chappy - Win32 Release
!MESSAGE No configuration specified. Defaulting to Chappy - Win32 Release.
!ENDIF 

!IF "$(CFG)" != "Chappy - Win32 Release" && "$(CFG)" != "Chappy - Win32 Debug"
!MESSAGE Invalid configuration "$(CFG)" specified.
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Chappy.mak" CFG="Chappy - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Chappy - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "Chappy - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 
!ERROR An invalid configuration is specified.
!ENDIF 

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE 
NULL=nul
!ENDIF 

CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Chappy - Win32 Release"

OUTDIR=.\Release
INTDIR=.\Release
# Begin Custom Macros
OutDir=.\.\Release
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Chappy.exe"

!ELSE 

ALL : "$(OUTDIR)\Chappy.exe"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\BanDlg.obj"
	-@erase "$(INTDIR)\Chappy.obj"
	-@erase "$(INTDIR)\Chappy.pch"
	-@erase "$(INTDIR)\Chappy.res"
	-@erase "$(INTDIR)\ChppyDoc.obj"
	-@erase "$(INTDIR)\ChppyVw.obj"
	-@erase "$(INTDIR)\chtitems.obj"
	-@erase "$(INTDIR)\enterdlg.obj"
	-@erase "$(INTDIR)\flatTB.obj"
	-@erase "$(INTDIR)\History.obj"
	-@erase "$(INTDIR)\InputRE.obj"
	-@erase "$(INTDIR)\KickDlg.obj"
	-@erase "$(INTDIR)\LogInDlg.obj"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\mschatpr.obj"
	-@erase "$(INTDIR)\mschatx.obj"
	-@erase "$(INTDIR)\PLTree.obj"
	-@erase "$(INTDIR)\PWordDlg.obj"
	-@erase "$(INTDIR)\REdit.obj"
	-@erase "$(INTDIR)\RListCtr.obj"
	-@erase "$(INTDIR)\Room.obj"
	-@erase "$(INTDIR)\RoomHstr.obj"
	-@erase "$(INTDIR)\RTab.obj"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\TB.obj"
	-@erase "$(INTDIR)\Utility.obj"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Chappy.exe"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D\
 "_MBCS" /Fp"$(INTDIR)\Chappy.pch" /Yu"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Release/
CPP_SBRS=.
MTL_PROJ=/nologo /D "NDEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Chappy.res" /d "NDEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Chappy.bsc" 
BSC32_SBRS= \
	
LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /pdb:none /machine:I386\
 /out:"$(OUTDIR)\Chappy.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BanDlg.obj" \
	"$(INTDIR)\Chappy.obj" \
	"$(INTDIR)\Chappy.res" \
	"$(INTDIR)\ChppyDoc.obj" \
	"$(INTDIR)\ChppyVw.obj" \
	"$(INTDIR)\chtitems.obj" \
	"$(INTDIR)\enterdlg.obj" \
	"$(INTDIR)\flatTB.obj" \
	"$(INTDIR)\History.obj" \
	"$(INTDIR)\InputRE.obj" \
	"$(INTDIR)\KickDlg.obj" \
	"$(INTDIR)\LogInDlg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\mschatpr.obj" \
	"$(INTDIR)\mschatx.obj" \
	"$(INTDIR)\PLTree.obj" \
	"$(INTDIR)\PWordDlg.obj" \
	"$(INTDIR)\REdit.obj" \
	"$(INTDIR)\RListCtr.obj" \
	"$(INTDIR)\Room.obj" \
	"$(INTDIR)\RoomHstr.obj" \
	"$(INTDIR)\RTab.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TB.obj" \
	"$(INTDIR)\Utility.obj"

"$(OUTDIR)\Chappy.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

OUTDIR=.\Debug
INTDIR=.\Debug
# Begin Custom Macros
OutDir=.\.\Debug
# End Custom Macros

!IF "$(RECURSE)" == "0" 

ALL : "$(OUTDIR)\Chappy.exe" "$(OUTDIR)\Chappy.bsc"

!ELSE 

ALL : "$(OUTDIR)\Chappy.exe" "$(OUTDIR)\Chappy.bsc"

!ENDIF 

CLEAN :
	-@erase "$(INTDIR)\BanDlg.obj"
	-@erase "$(INTDIR)\BanDlg.sbr"
	-@erase "$(INTDIR)\Chappy.obj"
	-@erase "$(INTDIR)\Chappy.pch"
	-@erase "$(INTDIR)\Chappy.res"
	-@erase "$(INTDIR)\Chappy.sbr"
	-@erase "$(INTDIR)\ChppyDoc.obj"
	-@erase "$(INTDIR)\ChppyDoc.sbr"
	-@erase "$(INTDIR)\ChppyVw.obj"
	-@erase "$(INTDIR)\ChppyVw.sbr"
	-@erase "$(INTDIR)\chtitems.obj"
	-@erase "$(INTDIR)\chtitems.sbr"
	-@erase "$(INTDIR)\enterdlg.obj"
	-@erase "$(INTDIR)\enterdlg.sbr"
	-@erase "$(INTDIR)\flatTB.obj"
	-@erase "$(INTDIR)\flatTB.sbr"
	-@erase "$(INTDIR)\History.obj"
	-@erase "$(INTDIR)\History.sbr"
	-@erase "$(INTDIR)\InputRE.obj"
	-@erase "$(INTDIR)\InputRE.sbr"
	-@erase "$(INTDIR)\KickDlg.obj"
	-@erase "$(INTDIR)\KickDlg.sbr"
	-@erase "$(INTDIR)\LogInDlg.obj"
	-@erase "$(INTDIR)\LogInDlg.sbr"
	-@erase "$(INTDIR)\MainFrm.obj"
	-@erase "$(INTDIR)\MainFrm.sbr"
	-@erase "$(INTDIR)\mschatpr.obj"
	-@erase "$(INTDIR)\mschatpr.sbr"
	-@erase "$(INTDIR)\mschatx.obj"
	-@erase "$(INTDIR)\mschatx.sbr"
	-@erase "$(INTDIR)\PLTree.obj"
	-@erase "$(INTDIR)\PLTree.sbr"
	-@erase "$(INTDIR)\PWordDlg.obj"
	-@erase "$(INTDIR)\PWordDlg.sbr"
	-@erase "$(INTDIR)\REdit.obj"
	-@erase "$(INTDIR)\REdit.sbr"
	-@erase "$(INTDIR)\RListCtr.obj"
	-@erase "$(INTDIR)\RListCtr.sbr"
	-@erase "$(INTDIR)\Room.obj"
	-@erase "$(INTDIR)\Room.sbr"
	-@erase "$(INTDIR)\RoomHstr.obj"
	-@erase "$(INTDIR)\RoomHstr.sbr"
	-@erase "$(INTDIR)\RTab.obj"
	-@erase "$(INTDIR)\RTab.sbr"
	-@erase "$(INTDIR)\StdAfx.obj"
	-@erase "$(INTDIR)\StdAfx.sbr"
	-@erase "$(INTDIR)\TB.obj"
	-@erase "$(INTDIR)\TB.sbr"
	-@erase "$(INTDIR)\Utility.obj"
	-@erase "$(INTDIR)\Utility.sbr"
	-@erase "$(INTDIR)\vc50.idb"
	-@erase "$(INTDIR)\vc50.pdb"
	-@erase "$(OUTDIR)\Chappy.bsc"
	-@erase "$(OUTDIR)\Chappy.exe"
	-@erase "$(OUTDIR)\Chappy.ilk"
	-@erase "$(OUTDIR)\Chappy.pdb"

"$(OUTDIR)" :
    if not exist "$(OUTDIR)/$(NULL)" mkdir "$(OUTDIR)"

CPP_PROJ=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS"\
 /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Chappy.pch" /Yu"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 
CPP_OBJS=.\Debug/
CPP_SBRS=.\Debug/
MTL_PROJ=/nologo /D "_DEBUG" /mktyplib203 /win32 
RSC_PROJ=/l 0x409 /fo"$(INTDIR)\Chappy.res" /d "_DEBUG" 
BSC32=bscmake.exe
BSC32_FLAGS=/nologo /o"$(OUTDIR)\Chappy.bsc" 
BSC32_SBRS= \
	"$(INTDIR)\BanDlg.sbr" \
	"$(INTDIR)\Chappy.sbr" \
	"$(INTDIR)\ChppyDoc.sbr" \
	"$(INTDIR)\ChppyVw.sbr" \
	"$(INTDIR)\chtitems.sbr" \
	"$(INTDIR)\enterdlg.sbr" \
	"$(INTDIR)\flatTB.sbr" \
	"$(INTDIR)\History.sbr" \
	"$(INTDIR)\InputRE.sbr" \
	"$(INTDIR)\KickDlg.sbr" \
	"$(INTDIR)\LogInDlg.sbr" \
	"$(INTDIR)\MainFrm.sbr" \
	"$(INTDIR)\mschatpr.sbr" \
	"$(INTDIR)\mschatx.sbr" \
	"$(INTDIR)\PLTree.sbr" \
	"$(INTDIR)\PWordDlg.sbr" \
	"$(INTDIR)\REdit.sbr" \
	"$(INTDIR)\RListCtr.sbr" \
	"$(INTDIR)\Room.sbr" \
	"$(INTDIR)\RoomHstr.sbr" \
	"$(INTDIR)\RTab.sbr" \
	"$(INTDIR)\StdAfx.sbr" \
	"$(INTDIR)\TB.sbr" \
	"$(INTDIR)\Utility.sbr"

"$(OUTDIR)\Chappy.bsc" : "$(OUTDIR)" $(BSC32_SBRS)
    $(BSC32) @<<
  $(BSC32_FLAGS) $(BSC32_SBRS)
<<

LINK32=link.exe
LINK32_FLAGS=/nologo /subsystem:windows /incremental:yes\
 /pdb:"$(OUTDIR)\Chappy.pdb" /debug /machine:I386 /out:"$(OUTDIR)\Chappy.exe" 
LINK32_OBJS= \
	"$(INTDIR)\BanDlg.obj" \
	"$(INTDIR)\Chappy.obj" \
	"$(INTDIR)\Chappy.res" \
	"$(INTDIR)\ChppyDoc.obj" \
	"$(INTDIR)\ChppyVw.obj" \
	"$(INTDIR)\chtitems.obj" \
	"$(INTDIR)\enterdlg.obj" \
	"$(INTDIR)\flatTB.obj" \
	"$(INTDIR)\History.obj" \
	"$(INTDIR)\InputRE.obj" \
	"$(INTDIR)\KickDlg.obj" \
	"$(INTDIR)\LogInDlg.obj" \
	"$(INTDIR)\MainFrm.obj" \
	"$(INTDIR)\mschatpr.obj" \
	"$(INTDIR)\mschatx.obj" \
	"$(INTDIR)\PLTree.obj" \
	"$(INTDIR)\PWordDlg.obj" \
	"$(INTDIR)\REdit.obj" \
	"$(INTDIR)\RListCtr.obj" \
	"$(INTDIR)\Room.obj" \
	"$(INTDIR)\RoomHstr.obj" \
	"$(INTDIR)\RTab.obj" \
	"$(INTDIR)\StdAfx.obj" \
	"$(INTDIR)\TB.obj" \
	"$(INTDIR)\Utility.obj"

"$(OUTDIR)\Chappy.exe" : "$(OUTDIR)" $(DEF_FILE) $(LINK32_OBJS)
    $(LINK32) @<<
  $(LINK32_FLAGS) $(LINK32_OBJS)
<<

!ENDIF 

.c{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_OBJS)}.obj::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.c{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cpp{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<

.cxx{$(CPP_SBRS)}.sbr::
   $(CPP) @<<
   $(CPP_PROJ) $< 
<<


!IF "$(CFG)" == "Chappy - Win32 Release" || "$(CFG)" == "Chappy - Win32 Debug"
SOURCE=.\BanDlg.cpp
DEP_CPP_BANDL=\
	".\BanDlg.h"\
	".\Chappy.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\BanDlg.obj" : $(SOURCE) $(DEP_CPP_BANDL) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\BanDlg.obj"	"$(INTDIR)\BanDlg.sbr" : $(SOURCE) $(DEP_CPP_BANDL)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\Chappy.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_CHAPP=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\MainFrm.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	

"$(INTDIR)\Chappy.obj" : $(SOURCE) $(DEP_CPP_CHAPP) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_CHAPP=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\MainFrm.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	

"$(INTDIR)\Chappy.obj"	"$(INTDIR)\Chappy.sbr" : $(SOURCE) $(DEP_CPP_CHAPP)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\Chappy.rc
DEP_RSC_CHAPPY=\
	".\res\Chappy.ico"\
	".\res\Chappy.rc2"\
	".\res\ChppyDoc.ico"\
	".\res\Earth.ico"\
	".\res\ico00002.ico"\
	".\res\ico00003.ico"\
	".\res\ico00004.ico"\
	".\res\icon1.ico"\
	".\res\room_lis.ico"\
	".\res\Toolbar.bmp"\
	".\res\toolbar1.bmp"\
	

"$(INTDIR)\Chappy.res" : $(SOURCE) $(DEP_RSC_CHAPPY) "$(INTDIR)"
	$(RSC) $(RSC_PROJ) $(SOURCE)


SOURCE=.\ChppyDoc.cpp
DEP_CPP_CHPPY=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\ChppyDoc.obj" : $(SOURCE) $(DEP_CPP_CHPPY) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\ChppyDoc.obj"	"$(INTDIR)\ChppyDoc.sbr" : $(SOURCE) $(DEP_CPP_CHPPY)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\ChppyVw.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_CHPPYV=\
	".\BanDlg.h"\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\enterdlg.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\KickDlg.h"\
	".\LogInDlg.h"\
	".\MainFrm.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\PWordDlg.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	

"$(INTDIR)\ChppyVw.obj" : $(SOURCE) $(DEP_CPP_CHPPYV) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_CHPPYV=\
	".\BanDlg.h"\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\enterdlg.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\KickDlg.h"\
	".\LogInDlg.h"\
	".\MainFrm.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\PWordDlg.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	

"$(INTDIR)\ChppyVw.obj"	"$(INTDIR)\ChppyVw.sbr" : $(SOURCE) $(DEP_CPP_CHPPYV)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\chtitems.cpp
DEP_CPP_CHTIT=\
	".\chtitems.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\chtitems.obj" : $(SOURCE) $(DEP_CPP_CHTIT) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\chtitems.obj"	"$(INTDIR)\chtitems.sbr" : $(SOURCE) $(DEP_CPP_CHTIT)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\enterdlg.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_ENTER=\
	".\Chappy.h"\
	".\enterdlg.h"\
	".\Globals.h"\
	

"$(INTDIR)\enterdlg.obj" : $(SOURCE) $(DEP_CPP_ENTER) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_ENTER=\
	".\Chappy.h"\
	".\enterdlg.h"\
	".\Globals.h"\
	

"$(INTDIR)\enterdlg.obj"	"$(INTDIR)\enterdlg.sbr" : $(SOURCE) $(DEP_CPP_ENTER)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\flatTB.cpp
DEP_CPP_FLATT=\
	".\FlatTB.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\flatTB.obj" : $(SOURCE) $(DEP_CPP_FLATT) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\flatTB.obj"	"$(INTDIR)\flatTB.sbr" : $(SOURCE) $(DEP_CPP_FLATT)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\History.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_HISTO=\
	".\Chappy.h"\
	".\Globals.h"\
	".\History.h"\
	".\REdit.h"\
	".\Utility.h"\
	

"$(INTDIR)\History.obj" : $(SOURCE) $(DEP_CPP_HISTO) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_HISTO=\
	".\Chappy.h"\
	".\Globals.h"\
	".\History.h"\
	".\REdit.h"\
	".\Utility.h"\
	

"$(INTDIR)\History.obj"	"$(INTDIR)\History.sbr" : $(SOURCE) $(DEP_CPP_HISTO)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\InputRE.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_INPUT=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	".\Utility.h"\
	

"$(INTDIR)\InputRE.obj" : $(SOURCE) $(DEP_CPP_INPUT) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_INPUT=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	".\Utility.h"\
	

"$(INTDIR)\InputRE.obj"	"$(INTDIR)\InputRE.sbr" : $(SOURCE) $(DEP_CPP_INPUT)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\KickDlg.cpp
DEP_CPP_KICKD=\
	".\Chappy.h"\
	".\KickDlg.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\KickDlg.obj" : $(SOURCE) $(DEP_CPP_KICKD) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\KickDlg.obj"	"$(INTDIR)\KickDlg.sbr" : $(SOURCE) $(DEP_CPP_KICKD)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\LogInDlg.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_LOGIN=\
	".\Chappy.h"\
	".\LogInDlg.h"\
	

"$(INTDIR)\LogInDlg.obj" : $(SOURCE) $(DEP_CPP_LOGIN) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_LOGIN=\
	".\Chappy.h"\
	".\LogInDlg.h"\
	

"$(INTDIR)\LogInDlg.obj"	"$(INTDIR)\LogInDlg.sbr" : $(SOURCE) $(DEP_CPP_LOGIN)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\MainFrm.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_MAINF=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\MainFrm.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	

"$(INTDIR)\MainFrm.obj" : $(SOURCE) $(DEP_CPP_MAINF) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_MAINF=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\MainFrm.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	

"$(INTDIR)\MainFrm.obj"	"$(INTDIR)\MainFrm.sbr" : $(SOURCE) $(DEP_CPP_MAINF)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\mschatpr.cpp
DEP_CPP_MSCHA=\
	".\chtitems.h"\
	".\mschatpr.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\mschatpr.obj" : $(SOURCE) $(DEP_CPP_MSCHA) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\mschatpr.obj"	"$(INTDIR)\mschatpr.sbr" : $(SOURCE) $(DEP_CPP_MSCHA)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\mschatx.cpp
DEP_CPP_MSCHAT=\
	".\mschatx.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\mschatx.obj" : $(SOURCE) $(DEP_CPP_MSCHAT) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\mschatx.obj"	"$(INTDIR)\mschatx.sbr" : $(SOURCE) $(DEP_CPP_MSCHAT)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\PLTree.cpp
DEP_CPP_PLTRE=\
	".\Chappy.h"\
	".\PLTree.h"\
	".\Utility.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\PLTree.obj" : $(SOURCE) $(DEP_CPP_PLTRE) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\PLTree.obj"	"$(INTDIR)\PLTree.sbr" : $(SOURCE) $(DEP_CPP_PLTRE)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\PWordDlg.cpp
DEP_CPP_PWORD=\
	".\Chappy.h"\
	".\PWordDlg.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\PWordDlg.obj" : $(SOURCE) $(DEP_CPP_PWORD) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\PWordDlg.obj"	"$(INTDIR)\PWordDlg.sbr" : $(SOURCE) $(DEP_CPP_PWORD)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\REdit.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_REDIT=\
	".\Chappy.h"\
	".\REdit.h"\
	

"$(INTDIR)\REdit.obj" : $(SOURCE) $(DEP_CPP_REDIT) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_REDIT=\
	".\Chappy.h"\
	".\REdit.h"\
	

"$(INTDIR)\REdit.obj"	"$(INTDIR)\REdit.sbr" : $(SOURCE) $(DEP_CPP_REDIT)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\RListCtr.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_RLIST=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	".\Utility.h"\
	

"$(INTDIR)\RListCtr.obj" : $(SOURCE) $(DEP_CPP_RLIST) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_RLIST=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	".\Utility.h"\
	

"$(INTDIR)\RListCtr.obj"	"$(INTDIR)\RListCtr.sbr" : $(SOURCE) $(DEP_CPP_RLIST)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\Room.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_ROOM_=\
	".\Chappy.h"\
	".\chtitems.h"\
	".\Globals.h"\
	".\History.h"\
	".\KickDlg.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	

"$(INTDIR)\Room.obj" : $(SOURCE) $(DEP_CPP_ROOM_) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_ROOM_=\
	".\Chappy.h"\
	".\chtitems.h"\
	".\Globals.h"\
	".\History.h"\
	".\KickDlg.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	

"$(INTDIR)\Room.obj"	"$(INTDIR)\Room.sbr" : $(SOURCE) $(DEP_CPP_ROOM_)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\RoomHstr.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_ROOMH=\
	".\Chappy.h"\
	".\History.h"\
	".\REdit.h"\
	".\RoomHstr.h"\
	

"$(INTDIR)\RoomHstr.obj" : $(SOURCE) $(DEP_CPP_ROOMH) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_ROOMH=\
	".\Chappy.h"\
	".\History.h"\
	".\REdit.h"\
	".\RoomHstr.h"\
	

"$(INTDIR)\RoomHstr.obj"	"$(INTDIR)\RoomHstr.sbr" : $(SOURCE) $(DEP_CPP_ROOMH)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\RTab.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_RTAB_=\
	".\Chappy.h"\
	".\Globals.h"\
	".\RTab.h"\
	

"$(INTDIR)\RTab.obj" : $(SOURCE) $(DEP_CPP_RTAB_) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_RTAB_=\
	".\Chappy.h"\
	".\Globals.h"\
	".\RTab.h"\
	

"$(INTDIR)\RTab.obj"	"$(INTDIR)\RTab.sbr" : $(SOURCE) $(DEP_CPP_RTAB_)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\StdAfx.cpp
DEP_CPP_STDAF=\
	".\StdAfx.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"

CPP_SWITCHES=/nologo /MT /W3 /GX /Zi /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS"\
 /D "_MBCS" /Fp"$(INTDIR)\Chappy.pch" /Yc"stdafx.h" /Fo"$(INTDIR)\\"\
 /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\Chappy.pch" : $(SOURCE) $(DEP_CPP_STDAF)\
 "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

CPP_SWITCHES=/nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D\
 "_WINDOWS" /D "_MBCS" /FR"$(INTDIR)\\" /Fp"$(INTDIR)\Chappy.pch" /Yc"stdafx.h"\
 /Fo"$(INTDIR)\\" /Fd"$(INTDIR)\\" /FD /c 

"$(INTDIR)\StdAfx.obj"	"$(INTDIR)\StdAfx.sbr"	"$(INTDIR)\Chappy.pch" : \
$(SOURCE) $(DEP_CPP_STDAF) "$(INTDIR)"
	$(CPP) @<<
  $(CPP_SWITCHES) $(SOURCE)
<<


!ENDIF 

SOURCE=.\TB.cpp

!IF  "$(CFG)" == "Chappy - Win32 Release"

DEP_CPP_TB_CP=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	

"$(INTDIR)\TB.obj" : $(SOURCE) $(DEP_CPP_TB_CP) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"

DEP_CPP_TB_CP=\
	".\Chappy.h"\
	".\ChppyDoc.h"\
	".\ChppyVw.h"\
	".\chtitems.h"\
	".\FlatTB.h"\
	".\Globals.h"\
	".\History.h"\
	".\InputRE.h"\
	".\mschatpr.h"\
	".\mschatx.h"\
	".\PLTree.h"\
	".\REdit.h"\
	".\RListCtr.h"\
	".\Room.h"\
	".\RoomHstr.h"\
	".\RTab.h"\
	".\TB.h"\
	

"$(INTDIR)\TB.obj"	"$(INTDIR)\TB.sbr" : $(SOURCE) $(DEP_CPP_TB_CP) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ENDIF 

SOURCE=.\Utility.cpp
DEP_CPP_UTILI=\
	".\Chappy.h"\
	".\Utility.h"\
	

!IF  "$(CFG)" == "Chappy - Win32 Release"


"$(INTDIR)\Utility.obj" : $(SOURCE) $(DEP_CPP_UTILI) "$(INTDIR)"\
 "$(INTDIR)\Chappy.pch"


!ELSEIF  "$(CFG)" == "Chappy - Win32 Debug"


"$(INTDIR)\Utility.obj"	"$(INTDIR)\Utility.sbr" : $(SOURCE) $(DEP_CPP_UTILI)\
 "$(INTDIR)" "$(INTDIR)\Chappy.pch"


!ENDIF 


!ENDIF 

