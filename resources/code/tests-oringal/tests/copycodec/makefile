# Makefile for SPHDEC1_COPY
MODULE_NAME = rpetests

-include config/codecs.mk
-include ../makerules/env.mk

SRCDIR =
INCDIR =

# List all the external components/interfaces, whose interface header files 
#  need to be included for this component
INCLUDE_EXTERNAL_INTERFACES =  xdc xdais

# Common source files and CFLAGS across all platforms and cores
SRCS_COMMON = 
CFLAGS_LOCAL_COMMON =

# Core/SoC/platform specific source files and CFLAGS
# Example: 
#   SRCS_<core/SoC/platform-name> = 
#   CFLAGS_LOCAL_<core/SoC/platform-name> =

SRCS_a8host = 

SRCS_m3video = 

SRCS_c6xdsp = sphdec1_copy.c

# Include common make files
include $(ROOTDIR)/makerules/common.mk

# OBJs and libraries are built by using rule defined in rules_<target>.mk 
#     and need not be explicitly specified here

# Nothing beyond this point

