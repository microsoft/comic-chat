##########################################################################
#
#   Microsoft Confidential
#   Copyright (C) Microsoft Corporation 1991-96
#   All Rights Reserved.
#
##########################################################################
#	Derived from common.inc in ccshell and modified for 
#	Chat client by RamuM, July 23 96
#
# Common include file 'sources' files in the chat project.
#
#
# Required definitions:
#
#     COMMONINC_DIR
#        Path to root containing this inc file.
#
#     FREEBUILD 
#        Set by makefile.def file.  One of the following:
#          0      - build debug
#          1      - build retail
#             
#
# Definitions used if defined:
#
#     USE_LEGO
#        Define lego macros
#
#     TARGET_WIN95
#        Build component explicitly for the Win95 platform.  If this is 
#        not defined, default is NT.
#
#     TARGET_BOTH
#        Build component so it will run in both environments.
#

!include $(DEVROOT)\common.inc
!if !defined(COMMONINC_DIR)
!error  COMMONINC_DIR must be defined in sources file
!endif


#Setting MFC_VER to be 40 for Chat
MFC_VER = 40

#
# In case you have an old makefile.def
#
!if "$(_OBJ_DIR)" == ""
_OBJ_DIR        = obj
!endif
!if "$(O)" == ""
O               = $(_OBJ_DIR)\$(TARGET_DIRECTORY)
!endif


#
# Set private paths
#

INCLUDES        = .;$(COMMONINC_DIR)\inc;$(INCLUDES)


#
# Set common options
#

# Defining this allows windows.h to include other headers
NOT_LEAN_AND_MEAN= 1

# Don't link to NT libs 
#USE_NOLIBS      = 1
NO_NTDLL =1

!if defined(TARGET_WIN95)
#  Don't define WINNT
CHICAGO_PRODUCT = 1

#  Since CHICAGO_PRODUCT is defined, we need to define WIN32_DEFINE correctly
WIN32_DEFINE    = -DWIN32=100 -D_WIN32_WINNT=0x0400
!endif

UMTYPE          = windows

#
# Additional compiler flags
#

C_DEFINES       = $(C_DEFINES) -DWIN32 -D_WIN32

!if !defined(TARGET_WIN95)
C_DEFINES       = $(C_DEFINES) -DNT 

!  if !defined(NOT_UNICODE)
C_DEFINES       = $(C_DEFINES) -DUNICODE -D_UNICODE
!  endif
!endif

# Backwards compatibility with some personal command files
!if "$(SHELLDBG)" == "1"
C_DEFINES       = $(C_DEFINES) -DDEBUG -DFULL_DEBUG

!elseif "$(FREEBUILD)" == "0"
C_DEFINES       = $(C_DEFINES) -DDEBUG

!endif

# Assume we're building for Nashville only right now
C_DEFINES       = $(C_DEFINES) -DNASHVILLE

# This define helps us distinguish our development environment
C_DEFINES       = $(C_DEFINES) -DWINNT_ENV

# IE40-only features 
!ifdef FEATURE_IE40
C_DEFINES       = $(C_DEFINES) -DFEATURE_IE40 -DNASH
!endif


# Make warnings equivalent to errors

#!if !defined(MSC_WARNING_LEVEL)
MSC_WARNING_LEVEL= /W3
#!endif
#MSC_WARNING_LEVEL= $(MSC_WARNING_LEVEL) /WX


#
# Additional linker flags
#
#Not Needed

LINKER_FLAGS    = $(LINKER_FLAGS) -map


