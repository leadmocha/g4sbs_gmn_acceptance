#!/bin/bash

for dateCmd in may may_v2 july;
do
#  for kin in `seq -f"%02.f" 1 7`;
#  for kin in `seq -f"%02.f" 1 3`;
#  for kin in `seq -f"%02.f" 4 5`;
  for kin in `seq -f"%02.f" 6 7`;
  do
    for run in `seq -f"%03.f" 0 9`;
    do
      xmlFile="jlab_batch/hcal_voffset/${dateCmd}/jlab_gmn_kin${kin}_r${run}.xml"
      echo "Submitting ${xmlFile}"
      jsub -xml ${xmlFile}
      #ls ${xmlFile}
    done
  done
done
