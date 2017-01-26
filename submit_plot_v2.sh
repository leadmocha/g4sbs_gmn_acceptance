#!/bin/bash

for dateCmd in may_v2;
do
  for kin in `seq -f"%02.f" 2 2`;
  do
    xmlFile="jlab_batch/hcal_voffset/${dateCmd}/plot/plot_gmn_kin${kin}.xml"
    echo "Submitting ${xmlFile}"
    jsub -xml ${xmlFile}
    #ls ${xmlFile}
  done
done
