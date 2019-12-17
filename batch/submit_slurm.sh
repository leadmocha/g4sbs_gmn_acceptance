#!/bin/bash

PROJECT_BASEDIR=/raid1/cornejo/sbs/GMnAcceptance
PERMDIR=${PROJECT_BASEDIR}/workspace
PERMBINDIR=${PROJECT_BASEDIR}/build
FIELDDIR=/raid1/cornejo/sbs/fieldmaps

DO_SUBMIT=1
SHRINKROOTFILE=""
SLEEP_BETWEEN_SUBMITS=3
NUMEVENTS=100000
pre_macros=( "ckov_scint" )
pre_macros=( "nockov_noscint" )
##post_macros=("gmn/gmn_k1" "gmn/gmn_k2" "gmn/gmn_k3" "gmn/gmn_k4" "gmn/gmn_k5" "gmn/gmn_k6" "gmn/gmn_k7" "gmn/gmn_k1_noclamp" "gmn/gmn_k2_noclamp" "gmn/gmn_k3_noclamp" "gmn/gmn_k4_noclamp" "gmn/gmn_k5_noclamp" "gmn/gmn_k6_noclamp" "gmn/gmn_k7_noclamp" )
post_macros=("gmn_k1" "gmn_k2" "gmn_k3" ) # "gmn_k1_noclamp")
post_template_options=( "with_clamp" "no_clamp")
post_template_options=( "no_clamp" "with_clamp" )
START_RUNNUM=0
numRuns=50
maxSimultaneous=300
QUEUE=green
QUEUE=red
RUNLISTFILE=""
DELETEOLDROOTFILE="1"
PRINTEVERY=100
DOSLEEP=4

## Source the specified config file to change any of the defaults listed above
if [ "x$1" != "x" ];
then
  echo "Sourcing config file: $1"
  ## Source the specified config file
  source $1
fi

expvar=""
function exportVAR()
{
  ## Dereference the variable
  var=\$"$1"
  ## Get variable contents
  val=`eval "expr \"${var}\""`
  if [ "x${expvar}" != "x" ];
  then
    expvar="${expvar},"
  fi
  expvar="${expvar}$1=${val}"

}

## Compute end run number
endRun=$((${numRuns}+${START_RUNNUM}))
endRun=$((${endRun}-1))
RUNLIST=""
if [ "x${RUNLISTFILE}" != "x" ];
then
  RUNLIST=`cat ${RUNLISTFILE}`
  RUNARRAY="["
  for run in `cat ${RUNLISTFILE}`;
  do
    if [ ${RUNARRAY} != "[" ];
    then
      RUNARRAY="${RUNARRAY},"
    fi
    RUNARRAY="${RUNARRAY}${run}"
  done
  RUNARRAY="${RUNARRAY}]"
else
  RUNLIST=`seq -f"%03.0f" ${START_RUNNUM} ${endRun}`
  RUNARRAY="[${START_RUNNUM}-${endRun}]"
fi
if [ ${maxSimultaneous} != 0 ];
then
  RUNARRAY="${RUNARRAY}%${maxSimultaneous}"
fi




#COUNT=0
for pre in ${pre_macros[@]};
do
  for post in ${post_macros[@]};
  do
    if [ ${#post_template_options} == 0 ];
    then
      post_template_options=("-")
    fi
    for post_t in ${post_template_options[@]};
    do
      PREMACRO="scripts/preinit_${pre}.mac"
      POSTMACRO="scripts/${post}.mac"
      if [ "${post_t}" != "-" ];
      then
        POST_TEMPLATE="scripts/post_templates/${post_t}.mac"
        exportVAR POST_TEMPLATE
        ppost="${post}_${post_t}"
      fi
      #echo "Generating scripts for post=\"${post}\" pre=\"${pre}\" "
      #mkdir -p ${PROJECT_BASEDIR}/rootfiles/${post}_${pre}
      RUNNAME="${ppost}_${pre}_${RUN}"
      ROOTFILEPATH="rootfiles/${ppost}_${pre}"
      ROOTFILEPREFIX="${ppost}_${pre}_"
      ROOTFILEPREFIX_SMALL="small_${ROOTFILEPREFIX}"

      mkdir -p ${PROJECT_BASEDIR}/${ROOTFILEPATH}
      if [ "${DELETEOLDROOTFILE}" == "1" ];
      then
        for RUN in ${RUNLIST[@]};
        do
          ROOTFILE=${PROJECT_BASEDIR}/"${ROOTFILEPATH}/${ROOTFILEPREFIX}${RUN}.root"
          if [ -e ${ROOTFILE} ];
          then
            echo "Deleting old rootfile: ${ROOTFILE}"
            rm ${ROOTFILE}
          fi
        done
      fi
      if [ "${DO_SUBMIT}x" == "1x" ];
      then
        echo submitting ${pre} ${ppost}
        ## Export the variables to the script
        exportVAR ROOTFILEPATH
        exportVAR ROOTFILEPREFIX
        exportVAR ROOTFILEPREFIX_SMALL
        exportVAR NUMEVENTS
        exportVAR PREMACRO
        exportVAR POSTMACRO
        exportVAR PRINTEVERY
        exportVAR PROJECT_BASEDIR
        exportVAR PERMDIR
        exportVAR PERMBINDIR
        exportVAR FIELDDIR
        exportVAR START_RUNNUM
        exportVAR DOSLEEP
        if [ "x${SHRINKROOTFILE}" != "x" ];
        then
          exportVAR SHRINKROOTFILE
        fi

        cd logs

        sbatch --job-name="${ppost}_${pre}" -p ${QUEUE} --array ${RUNARRAY} --export=`echo ${expvar}` ../sbatch_script.sh
        cd ../
        sleep ${SLEEP_BETWEEN_SUBMITS}
      fi
    done
  done
done
