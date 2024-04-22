#
#  Copyright (c) 2012, Texas Instruments Incorporated
#  All rights reserved.
#
#  Redistribution and use in source and binary forms, with or without
#  modification, are permitted provided that the following conditions
#  are met:
#
#  *  Redistributions of source code must retain the above copyright
#     notice, this list of conditions and the following disclaimer.
#
#  *  Redistributions in binary form must reproduce the above copyright
#     notice, this list of conditions and the following disclaimer in the
#     documentation and/or other materials provided with the distribution.
#
#  *  Neither the name of Texas Instruments Incorporated nor the names of
#     its contributors may be used to endorse or promote products derived
#     from this software without specific prior written permission.
#
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
#  THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
#  PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
#  CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
#  EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
#  PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
#  OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
#  WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
#  OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
#  EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#

#
#  ======== makefile ========
#

EXBASE = .
include $(EXBASE)/products.mak

.PHONY: install

all:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	$(MAKE) -C host all
	$(MAKE) -C dsp all

help:
	@$(ECHO) "make                  # build executables"
	@$(ECHO) "make clean            # clean everything"
	@$(ECHO) "make install          # stage install folder"

# setup install goal
ifeq (install,$(MAKECMDGOALS))
ifeq (,$(EXEC_DIR))
EXEC_DIR=$(CURDIR)/install
endif
override EXEC_DIR:=$(EXEC_DIR)/ex04_dsp_math
endif

SLAVELOADER=$(SYSLINK_INSTALL_DIR)/packages/ti/syslink/bin
SLAVELOADER_DEBUG:=$(SLAVELOADER)/TI816X/samples/slaveloader_debug
SLAVELOADER:=$(SLAVELOADER)/TI816X/samples/slaveloader_release

install:
	@$(ECHO) "#"
	@$(ECHO) "# Making $@ ..."
	@$(MKDIR) $(EXEC_DIR)/debug
	$(INSTALL) run.sh $(EXEC_DIR)/debug
	$(CP) $(SLAVELOADER_DEBUG) $(EXEC_DIR)/debug/slaveloader
	@$(MKDIR) $(EXEC_DIR)/release
	$(INSTALL) run.sh $(EXEC_DIR)/release
	$(CP) $(SLAVELOADER) $(EXEC_DIR)/release/slaveloader
	$(MAKE) -C host EXEC_DIR=$(EXEC_DIR) install
	$(MAKE) -C dsp EXEC_DIR=$(EXEC_DIR) install

clean::
	$(RMDIR) install
	$(MAKE) -C host clean
	$(MAKE) -C dsp clean


#  ======== standard macros ========
ifneq (,$(wildcard $(XDC_INSTALL_DIR)/bin/echo.exe))
    # use these on Windows
    CP      = $(XDC_INSTALL_DIR)/bin/cp
    ECHO    = $(XDC_INSTALL_DIR)/bin/echo
    INSTALL = $(XDC_INSTALL_DIR)/bin/cp
    MKDIR   = $(XDC_INSTALL_DIR)/bin/mkdir -p
    RM      = $(XDC_INSTALL_DIR)/bin/rm -f
    RMDIR   = $(XDC_INSTALL_DIR)/bin/rm -rf
else
    # use these on Linux
    CP      = cp
    ECHO    = echo
    INSTALL = install
    MKDIR   = mkdir -p
    RM      = rm -f
    RMDIR   = rm -rf
endif
