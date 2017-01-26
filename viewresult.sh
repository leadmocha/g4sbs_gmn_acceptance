#!/bin/bash

if [ -z $1 ]; then
echo "Date empty"
exit
fi

if [ -z $2 ]; then
echo "Kin empty"
exit
fi

vim results/$1/kin0$2/kin0$2\_results.dat
