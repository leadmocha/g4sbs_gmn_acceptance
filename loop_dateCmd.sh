#!/bin/bash

if [ -z $1 ];
then
  echo "Missing command to execute"
  exit
fi

if [ -z $2 ];
then
  echo "Date missing"
  exit
fi

cmd=$1
dateCmd=$2

for kin in `seq 1 7`;
do
  $cmd $dateCmd $kin
done
