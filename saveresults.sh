#!/bin/bash

source date_kin_set.sh

## Loop over every kin in the kinlist
for (( i=0; i<${#kinlist}; i++ )); do
  kin=${kinlist:$i:1}
  if [ $kin -lt 1 -o $kin -gt 7 ]; then
    echo "Skipping bad kin number: $kin"
    continue ## Skip bad kin number
  fi
  kin=$(printf "%02d" $kin)
  csvFilePrefix="results/db/${dateCmd}_kin${kin}_"

  ## Find the next setID
  if [ $findSetID == 1 ]; then
    ## Get a list of the db files for this kinematic, reverse sorted
    ## which puts the last set (by number) ontop (and ignore errors in case
    ## there are none
    thisKinFiles=(`ls -r ${csvFilePrefix}[0-9][0-9][0-9].csv 2>  /dev/null`)
    if [ ${#thisKinFiles[@]} -eq 0 ]; then
      setID='000'
      echo "Found no previous set for ${dateCmd} Kin${kin}. SetID: ${setID} "
    else
      ## Then pick the first entry and pickout the setID number
      lastSetID=`echo ${thisKinFiles[0]} | grep -oP "${csvFilePrefix}\K[0-9][0-9][0-9]"`
      setID=$(printf "%03d" $((lastSetID+1)))
      echo "Found Last SetID: ${lastSetID}. New SetID: ${setID}"
    fi
  fi
  #echo $kin
  #./copy_result2db.sh ${dateCmd} ${kin} ${setID}
  inFile="results/${dateCmd}/kin${kin}/kin${kin}_results.csv"
  inExtraCSVFile="scripts/${dateCmd}/comment_gmn_kin${kin}.csv"
  csvFile="${csvFilePrefix}${setID}.csv"


  echo "Making $csvFile"
  echo "start_genconfig" > $csvFile
  cat $inExtraCSVFile >> $csvFile
  echo "end_genconfig" >> $csvFile
  cat $inFile >> $csvFile
  #csvFile="results/${dateCmd}/kin${kin}/kin${kin}_results.csv"

  echo "Backing up all result files for ${dateCmd} Kin${kin} Set ${setID}"
  tar jcf results/saved/results_${dateCmd}_kin${kin}_${setID}.tar.bz2 results/${dateCmd}/kin${kin} ${csvFile}
done
