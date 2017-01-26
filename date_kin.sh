#!/bin/bash

#kin=$(printf "%02d" $2)

#if [ ${#kin} -lt 2 ];
#then
#  kin="0$kin"
#fi

if [ -z $1 ]; then
  echo "Date empty"
  exit
fi

if [ -z $2 ]; then
  echo "Kin list empty (use -1 for all kins, 0 for none and to exit)"
  exit
elif [ $2 == 0 ]; then
  echo "Passed 0 for kin list, stopping"
  exit
elif [ ${#2} -gt 7 ]; then
  echo "Kinlist ($2) too long, must be less than 7 kins total"
  exit
fi

dateCmd=$1
kinlist=$2

if [ $kinlist == -1 ]; then
  echo "Will iterate over all kins"
  kinlist="1234567"
fi


