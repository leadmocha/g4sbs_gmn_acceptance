#!/bin/bash

if [ -z $1 ];
then
  echo "No date chosen, exiting!"
  exit -1
fi

if [ -z $2 ];
then
  echo "No comment specified. Exiting"
  exit -1
fi

dateCmd=$1
comment=$2

dateNow=`date +"%Y%m%d"`
fileOut="backups/backup_${dateCmd}_${dateNow}_${comment}.tar.bz2"

dirFiles="jlab_batch/${dateCmd} scripts/${dateCmd} results/${dateCmd}"
echo "Creating backup of: ${dirFiles}"
tar jvcf ${fileOut} ${dirFiles}
echo "Finished. Backup file ${fileOut}"
