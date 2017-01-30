#!/bin/bash

source date_kin.sh

findSetID=0
if [ -z $3 ]; then
  echo "SetID empty"
  exit
elif [ $3 == "next" ]; then
  echo "The next SetID will be determined for each kinematic"
  findSetID=1
  setID=0
else
  setID=$(printf "%03d" $3)
fi


