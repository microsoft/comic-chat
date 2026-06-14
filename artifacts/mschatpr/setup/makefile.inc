# Makefile.inc for generating the CAB and HTM files for MsChatPr
#       Created : RegisB, 07/09/97
#

IEXPRESS_DIR = $(BASEDIR)\mstools\

#Free Build, release

MsChatPr.Cab: ..\MsChatPr.Ocx MsChatPr.Inf
	setlocal
	set PATH=$(IEXPRESS_DIR);$(PATH)
	$(IEXPRESS_DIR)\iexpress /n MsChatPr.cdf
	endlocal

..\MsChatPr.Ocx: $(O)\MsChatPr.Ocx
	copy $(_NTTREE)\MsChatPr.Ocx ..\MsChatPr.Ocx


MsChatPr.Inf: CPrOr.Inf
	cl $(C_DEFINES) /I$(ROOT)\h /DSKIP_WINVER /EP verupd.c > verupd.bat
	verupd

MsChatPrCab.Htm: CPrCabOr.Htm
	cl $(C_DEFINES) /I$(ROOT)\h /DSKIP_WINVER /EP verupd.c > verupd.bat
	verupd

MsChatPrOcx.Htm: CPrOcxOr.Htm
	cl $(C_DEFINES) /I$(ROOT)\h /DSKIP_WINVER /EP verupd.c > verupd.bat
	verupd

