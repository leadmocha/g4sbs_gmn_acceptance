#!/bin/bash
MAXJOBS=6

if [ "x${1}" == "x" ];
then
  echo "Must specify kinematics!"
  exit
fi
if [ "x${2}" == "x" ];
then
  echo "Must specify vertical offset!"
  exit
fi

KINGLOB=${1}
if [ ${1} == "all" ];
then
  KINGLOB="*"
fi
VOFFGLOB=${2}
if [ ${2} == "all" ];
then
  VOFFGLOB="*"
fi

SCRIPTGLOB="scripts/hcal_voffset/run_gmn_kin${KINGLOB}_r[0-9]*_v${VOFFGLOB}.mac";

function runG4sbs()
{
  echo $0
}

#for script in `ls ${SCRIPTGLOB}`;
#do
#  echo "Executing ${script}"
  #time ../build/g4sbs --pre=scripts/preinit_jc2_gmn.mac --post=${script} &> logs/`basename ${script} .mac`.log
#done

#ls ${SCRIPTGLOB} | parallel -j 4 -i "echo hello world: {}<<" --

ls ${SCRIPTGLOB} | parallel --will-cite -P ${MAXJOBS} ./run_g4sbs.sh {}
