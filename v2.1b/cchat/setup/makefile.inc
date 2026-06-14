# Makefile.inc for generating the CAB files for ComicChat
# probaly version dependent text files automatically
#       Created : RamuM, 8/05/96
#

IEXPRESS_DIR = $(BASEDIR)\mstools\

#Free Build, release

MsChat4.CAB: ..\CChat.exe CChat4.inf
	setlocal
	set PATH=$(IEXPRESS_DIR);$(PATH)
	$(IEXPRESS_DIR)\iexpress /n chatcab4.cdf
	endlocal

CChat4.inf: CChat4or.inf
	cl $(C_DEFINES) /I$(ROOT)\h /DSKIP_WINVER /EP verupd.c > verupd.bat
	verupd

MsChat4.exe: ..\CChat.exe
	setlocal
	set PATH=$(IEXPRESS_DIR);$(PATH)
	$(IEXPRESS_DIR)\iexpress /n chatcab4.cdf /O:selfext4.txt,SelfExtractOptions
	endlocal

MsChat4a.exe: ..\CChat.exe
	setlocal
	set PATH=$(IEXPRESS_DIR);$(PATH)
        $(IEXPRESS_DIR)\iexpress /n cchat4a.cdf
	endlocal

..\CChat.exe: $(O)\CChat.exe
	copy $(_NTTREE)\CChat.exe ..\CChat.exe

# First preprocess the c file to generate a bat file
# and then run the bat file with the current targetdir as parameter
#
#VerUpd:
#        cl /I$(BASEDIR)\public\sdk\inc /EP verupd.c > verupd.bat
#        verupd $(O)
