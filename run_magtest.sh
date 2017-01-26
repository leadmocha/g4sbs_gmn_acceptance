#!/bin/bash

if [ -z $1 ];
then
  echo "Nothing to do"
  exit
fi

../../build/g4sbs --pre=scripts/preinit_jc2_gmn.mac --post=scripts/hcal_voffset/run_gmn_kin0${1}_r000.mac
root -b -q shrinkGMn.C\(\"rootfiles/hcal_voffset/gmn_kin0${1}_r000.root\",\"rootfiles/hcal_voffset/gmn_kin0${1}_r000_small.root\"\)
