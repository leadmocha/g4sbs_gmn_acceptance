#!/bin/bash

for kin in `seq -f"%02.0f" 1 7`; do
  for run_date in may; do
    mkdir -p results/hcal_voffset/${run_date}/kin${kin}
    echo "Running plotAcceptance.C for Kin${kin} and Date=${run_date}" || break
    `root -b -q plotAcceptance.C++g\($kin,\"$run_date\"\) &> results/hcal_voffset/${run_date}/kin${kin}/kin${kin}.log` || break 2
  done
done
