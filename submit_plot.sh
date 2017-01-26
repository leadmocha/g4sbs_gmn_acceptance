#!/bin/bash

# for dateCmd in may may_v2 july;
for dateCmd in july;
do
#  for kin in `seq -f"%02.f" 1 3`;
#  for kin in `seq -f"%02.f" 4 5`;
  for kin in `seq -f"%02.f" 1 7`;
  do
    xmlFile="jlab_batch/hcal_voffset/${dateCmd}/plot/plot_gmn_kin${kin}.xml"
    echo "Submitting ${xmlFile}"
    jsub -xml ${xmlFile}
    #ls ${xmlFile}
  done
done
