
for r in `seq 7 9`; do
  time ../../build/g4sbs --pre=scripts/preinit_jc2_gmn.mac --post=scripts/hcal_voffset/run_gmn_kin02_r00${r}_v0.mac;
done 
