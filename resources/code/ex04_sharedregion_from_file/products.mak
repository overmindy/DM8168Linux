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
#  ======== products.mak ========
#

DEPOT = _your_depot_folder_

BIOS_INSTALL_DIR        = $(DEPOT)/_your_bios_install_
CGT_ARM_PREFIX          = $(DEPOT)/_your_cs_arm_codegen_install_/bin/arm-none-linux-gnueabi-
IPC_INSTALL_DIR         = $(DEPOT)/_your_ipc_install_
SYSLINK_INSTALL_DIR     = $(DEPOT)/_your_syslink_install_
CGT_C674_ELF_INSTALL_DIR= $(DEPOT)/_your_ti_dsp_codegen_install_
XDC_INSTALL_DIR         = $(DEPOT)/_your_xdctools_install_

# Use this goal to print your product variables.
.show:
	@echo "BIOS_INSTALL_DIR         = $(BIOS_INSTALL_DIR)"
	@echo "CGT_ARM_PREFIX           = $(CGT_ARM_PREFIX)"
	@echo "IPC_INSTALL_DIR          = $(IPC_INSTALL_DIR)"
	@echo "SYSLINK_INSTALL_DIR      = $(SYSLINK_INSTALL_DIR)"
	@echo "CGT_C674_ELF_INSTALL_DIR = $(CGT_C674_ELF_INSTALL_DIR)"
	@echo "XDC_INSTALL_DIR          = $(XDC_INSTALL_DIR)"

# look for other products.mak file to override local settings
ifneq (,$(wildcard $(EXBASE)/../products.mak))
include $(EXBASE)/../products.mak
else
ifneq (,$(wildcard $(EXBASE)/../../products.mak))
include $(EXBASE)/../../products.mak/
# fix SYSLINK_INSTALL_DIR because product uses $(CURDIR)
SYSLINK_INSTALL_DIR = $(word 1,$(subst /examples, examples,$(CURDIR)))
endif
endif
