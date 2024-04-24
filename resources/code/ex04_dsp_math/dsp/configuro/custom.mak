## THIS IS A GENERATED FILE -- DO NOT EDIT
.configuro: .libraries,e674 linker.cmd \
  package/cfg/Dsp_pe674.oe674 \

linker.cmd: package/cfg/Dsp_pe674.xdl
	$(SED) 's"^\"\(package/cfg/Dsp_pe674cfg.cmd\)\"$""\"/home/user/ti-ezsdk_dm816x-evm_5_05_01_04/component-sources/syslink_2_20_00_14/examples/ex04_dsp_math/dsp/configuro/\1\""' package/cfg/Dsp_pe674.xdl > $@
