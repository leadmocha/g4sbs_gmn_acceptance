#!/bin/bash

source date_kin.sh

## Q2 is optional
if [ -z $3 ]; then
  echo "Kin empty"
  Q2=0
else
  Q2=$3
fi


for (( i=0; i<${#kinlist}; i++ )); do
  kin=${kinlist:$i:1}
  if [ $kin -lt 1 -o $kin -gt 7 ]; then
    echo "Skipping bad kin number: $kin"
  fi
  kin=$(printf "%02d" $kin)

  mkdir -p results/${dateCmd}/kin${kin}
  echo "Running plotAcceptance.C for Kin${kin}, Config=${dateCmd}"
  root -b -q plotAcceptance.C+g\($kin,\"$dateCmd\",${Q2}\) &> results/${dateCmd}/kin${kin}/kin${kin}.log

done
