# Makefile for MsChat Setup INF files
#
# Generates different INF files for different installations of MsChat
#     FULL - Normal INF.
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

CFLAGS_DEBUG=/DDEBUG
CFLAGS_RETAIL=

# All of the INF files that are built in this makefile
ALLINFS= \
!if "$(CPU)"=="I386"
    $(FULLDIR)\$(CPU)\cchat.inf \
!endif
    $(FULLDIR)\$(CPU)\cchat.inf \

# Inference rules
{.}.inx{$(FULLDIR)\$(CPU)}.inf:
         $(CC) $(CFLAGS_BASE) $(CFLAGS_FULL) $(CFLAGS_RETAIL) $< > $*.tmp
    uniq -u $*.tmp > $*25.inf
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


fullretail: fulldirs \
    $(FULLDIR)\$(CPU)\cchat.inf \


