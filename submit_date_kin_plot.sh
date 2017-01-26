#!/bin/bash

source date_kin.sh

for (( i=0; i<${#kinlist}; i++ )); do
  kin=${kinlist:$i:1}
  if [ $kin -lt 1 -o $kin -gt 7 ]; then
    echo "Skipping bad kin number: $kin"
  fi
  kin=$(printf "%02d" $kin)

  xmlFile="jlab_batch/${dateCmd}/plot/plot_gmn_kin${kin}.xml"
  echo "Submitting ${xmlFile}"
  jsub -xml ${xmlFile}
  #ls ${xmlFile}
done
