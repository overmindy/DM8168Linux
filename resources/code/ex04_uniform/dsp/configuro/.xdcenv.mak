#
_XDCBUILDCOUNT = 
ifneq (,$(findstring path,$(_USEXDCENV_)))
override XDCPATH = /home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/syslink_2_20_00_14/packages;/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/bios_6_33_05_46/packages;/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/ipc_1_24_03_32/packages;/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/xdctools_3_23_03_53/packages
override XDCROOT = /home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/xdctools_3_23_03_53
override XDCBUILDCFG = ./config.bld
endif
ifneq (,$(findstring args,$(_USEXDCENV_)))
override XDCARGS = 
override XDCTARGETS = 
endif
#
ifeq (0,1)
PKGPATH = /home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/syslink_2_20_00_14/packages;/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/bios_6_33_05_46/packages;/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/ipc_1_24_03_32/packages;/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/xdctools_3_23_03_53/packages;/home/elainay/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/xdctools_3_23_03_53/packages;..
HOSTOS = Linux
endif
