#!/bin/bash
# build.sh <ucf> [tops...]: build fpga_lf/hf/felica .bit for the XC3S50A into out_<ucf>/
set -e
U=${1:-fpga_hi70.ucf}; shift || true
TOPS=${@:-fpga_lf fpga_hf fpga_felica}
P=xc3s50a-4-vq100
source /opt/Xilinx/14.7/ISE_DS/settings64.sh /opt/Xilinx/14.7/ISE_DS >/dev/null
export LD_LIBRARY_PATH=/root/codex/ProxMark/tools/compat:$LD_LIBRARY_PATH
O=out_${U%.ucf}; mkdir -p $O
for T in $TOPS; do
    W=build_${U%.ucf}_$T; rm -rf $W; mkdir $W; cd $W
    echo "run -ifn ../$T.v -ifmt Verilog -ofn $T.ngc -ofmt NGC -p $P -top $T -opt_mode area -opt_level 2 -resource_sharing yes -fsm_style bram -fsm_encoding compact -vlgincdir {..}" > $T.xst
    xst -ifn $T.xst > xst.log
    ngdbuild -aul -p $P -nt timestamp -uc ../$U $T.ngc $T.ngd > ngd.log
    map -p $P -o $T.ncd $T.ngd > map.log
    par -w $T.ncd $T-placed.ncd > par.log
    bitgen -w -g StartUpClk:CClk -g UnusedPin:Pullnone $T-placed.ncd ../$O/$T.bit > bitgen.log
    grep -E 'Number of occupied Slices|Number of bonded IOBs' map.log | sed "s/^/  $T: /"
    grep -E 'timing errors|constraints were met|Timing Score' par.log | head -2 | sed "s/^/  $T: /"
    cd ..
done
ls -la $O
