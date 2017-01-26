#!/bin/bash

source date_kin.sh

for (( i=0; i<${#kinlist}; i++ )); do
  kin=${kinlist:$i:1}
  if [ $kin -lt 1 -o $kin -gt 7 ]; then
    echo "Skipping bad kin number: $kin"
  fi
  kin=$(printf "%02d" $kin)

  for run in `seq -f"%03.f" 0 9`;
  do
    xmlFile="jlab_batch/${dateCmd}/jlab_gmn_kin${kin}_r${run}.xml"
    echo "Submitting ${xmlFile}"
    jsub -xml ${xmlFile}
    #ls ${xmlFile}
  done
done
