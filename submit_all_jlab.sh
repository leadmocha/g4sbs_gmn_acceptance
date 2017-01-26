#!/bin/bash

#for voffset in 30 45;
#do
#  for kin in `seq -f"%02.f" 1 7`;
#  do
#    for run in `seq -f"%03.f" 0 9`;
#    do
#      xmlFile="jlab_batch/hcal_voffset/jlab_gmn_kin${kin}_r${run}_v${voffset}.xml"
#      echo "Submitting ${xmlFile}"
#      jsub -xml ${xmlFile}
#    done
#  done
#done

for kin in `seq 1 7`;
do
  ./submit_date_kin.sh july $kin
done
