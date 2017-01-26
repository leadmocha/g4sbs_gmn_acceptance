#!/bin/bash

for dateCmd in generated;
do
#  for kin in `seq -f"%02.f" 1 7`;
#  for kin in `seq -f"%02.f" 1 3`;
#  for kin in `seq -f"%02.f" 4 5`;
  for kin in `seq -f"%02.f" 1 7`;
  do
    for run in `seq -f"%03.f" 0 9`;
    do
      xmlFile="jlab_batch/${dateCmd}/jlab_gmn_kin${kin}_r${run}.xml"
      if [ -e ${xmlFile} ];
      then
        echo "Submitting ${xmlFile}"
        jsub -xml ${xmlFile}
        #ls ${xmlFile}
      fi
    done
  done
done
