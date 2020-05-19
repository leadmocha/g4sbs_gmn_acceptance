#!/bin/bash

source date_kin.sh

## See if anything was specified for setID
setID=${dateCmd#*/}
dateConf=${dateCmd%/${setID}}
findSetID=0
if [ "x${setID}" == "x" ];
then
  if [ -z $3 ]; then
    echo "SetID empty"
    echo ${setID}
    exit
  elif [ $3 == "next" ]; then
    echo "The next SetID will be determined for each kinematic"
    findSetID=1
    setID=0
  else
    setID=$(printf "%03d" $3)
  fi
fi

setID=$(printf "%03d" $setID)
