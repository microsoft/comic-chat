# Makefile for MsChat Setup INF files
#
# Generates different INF files for different installations of NetMeeting
#     FULL - Normal INF.
#     RUSSIAN - Russian specific version of INF
#
#
#


# Suffixes used by this makefile
.SUFFIXES : .inx .inf


# Definitions
# Target directories for INF files
FULLDIR=full

# Base switches for C preprocessor
#
# /EP - Run through the preprocessor only
# /W4 - Warning level 4
# /nologo - Don't display compiler logo
# /c - Don't build an EXE
CFLAGS_BASE=/EP /W4 /nologo /c

CFLAGS_RUSSIAN=/DRUSSIAN

CFLAGS_DEBUG=/DDEBUG
CFLAGS_RETAIL=

# All of the INF files that are built in this makefile
ALLINFS= \
!if "$(CPU)"=="I386"
    $(FULLDIR)\$(CPU)\cchat21.inf \
    $(FULLDIR)\$(CPU)\russian\cchat21.inf \
!endif
    $(FULLDIR)\$(CPU)\cchat21.inf \
    $(FULLDIR)\$(CPU)\russian\cchat21.inf \

# Inference rules
{.}.inx{$(FULLDIR)\$(CPU)}.inf:
         $(CC) $(CFLAGS_BASE) $(CFLAGS_FULL) $(CFLAGS_RETAIL) $< > $*.tmp
    uniq -u $*.tmp > $@
    del $*.tmp

{.}.inx{$(FULLDIR)\$(CPU)\russian}.inf:
         $(CC) $(CFLAGS_BASE) $(CFLAGS_FULL) $(CFLAGS_RETAIL) $(CFLAGS_RUSSIAN) $< >$*.tmp
    uniq -u $*.tmp > $@
    del $*.tmp

# BUILD TARGETS

# Default build target -- MUST BE FIRST
default: all

$(ALLINFS): ..\..\h\chatver.h

all: full

full: fulldirs fullretail

alldirs: fulldirs

retail: alldirs fullretail

debug: alldirs fullretail

fulldirs:
    -md $(FULLDIR)
    -md $(FULLDIR)\$(CPU)
    -md $(FULLDIR)\$(CPU)\russian


fullretail: fulldirs \
    $(FULLDIR)\$(CPU)\cchat21.inf \
    $(FULLDIR)\$(CPU)\russian\cchat21.inf \


