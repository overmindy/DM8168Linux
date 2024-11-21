# Makefile for mp3dec app

ROOTDIR = ../../../
APP_NAME = test1

SRCDIR = ../../../examples/common
INCDIR = . ../../../examples/common

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES = ipc xdais syslink \
                              ldrmemcfg rpe lindevkit

# List of all external libraries that need to be linked
EXTLIBS_m3video =
EXTLIBS_m3vpss =
EXTLIBS_a8host = $(syslink_PATH)/packages/ti/syslink/lib/syslink.a_debug \
                 $(lindevkit_PATH)/lib/libasound.so \
                 $(linuxutils_PATH)/packages/ti/sdo/linuxutils/cmem/lib/cmem.a470MV \
                 $(ROOTDIR)/lib/libtirpe.a

# List all the components required by the application
COMP_LIST_a8host = rpe
COMP_LIST_c6xdsp = 

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON =

SRCS_a8host = test1.c sysinit_hlos.c

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point
