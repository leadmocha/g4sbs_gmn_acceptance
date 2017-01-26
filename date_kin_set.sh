#!/bin/bash

source date_kin.sh

if [ -z $3 ]; then
  echo "SetID empty"
  exit
fi

setID=$(printf "%03d" $3)

