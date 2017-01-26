#!/bin/bash

for kin in `seq -f"%02.0f" 2 2`; do
  for run_date in 2016_04_20 2016_05_31; do
    mkdir -p results/hcal_voffset/${run_date}/kin${kin}
    for voff in 0; do
      echo "Running plotAcceptance.C for Kin${kin} and HCal Voff=${voff} Date=${run_date}" || break
      `root -b -q plotAcceptance.C++g\($kin,\"$voff\",\"$run_date\"\) &> results/hcal_voffset/${run_date}/kin${kin}/kin${kin}_voff${voff}.log` || break 2
    done
  done
done
