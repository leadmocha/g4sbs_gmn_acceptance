#!/bin/bash
#SBATCH --nodes=1
#SBATCH --cpus-per-task=1
#SBATCH --mem-per-cpu=2000
#SBATCH --time=120:00:00

## Source file that converts from slurm to PBS
if [ "x${SLURM_SUBMIT_HOST}" == "x" ];
then
  source /home/cornejo/pbs/env_to_slurm.sh
  cd /scratch/PBS_$SLURM_JOB_ID
fi

cd $WORKDIR
SRUN="srun --export=ALL --jobid=${SLURM_JOB_ID} "
SRUN=""

## Configure server that has files we need
SERVER=$SLURM_SUBMIT_HOST
SERVER=qcd000
WORKDIR=/scratch/PBS_$SLURM_JOB_ID
SCP="/usr/bin/scp -l 100000"
SSH="/usr/bin/ssh"
SHRINKMACROFILELIST=("shrinkPulser.C" "g4sbs_data.h" "g4sbs_data.cxx")
#RUNNUM=`echo ${SLURM_ARRAY_TASK_ID} | awk -F_ '{printf("%03d\n",$1)}'`
RUNNUM=`echo ${SLURM_ARRAY_TASK_ID} | awk -F_ '{printf $1 }'`
SLEEP_TIME=$((RUNNUM*${DOSLEEP}))
RUNNUM=$((RUNNUM+${START_RUNNUM}))
RUNNUM=`printf "%03d" ${RUNNUM}`

ROOTFILE=${ROOTFILEPATH}/${ROOTFILEPREFIX}${RUNNUM}.root
ROOTFILE_SMALL=${ROOTFILEPATH}/${ROOTFILEPREFIX_SMALL}${RUNNUM}.root

SERVPERMDIR=${SERVER}:${PERMDIR}
SERVPERMBINDIR=${SERVER}:${PERMBINDIR}
SERVFIELDDIR=${SERVER}:${FIELDDIR}
SERVROOTFILEDIR=${SERVER}:${PROJECT_BASEDIR}

SERVPERMDIR=${PERMDIR}
SERVPERMBINDIR=${PERMBINDIR}
SERVFIELDDIR=${FIELDDIR}
SERVROOTFILEDIR=${PROJECT_BASEDIR}
SCP="/usr/bin/cp "

## Print some useful information out
#/home/cornejo/pbs/output_job_info.sh

## Source the proton cluster environment
#source /raid1/proton/environments/proton_cluster.sh

## Source the repo environment
#echo "Sourcing thingies"
#echo source /raid1/cornejo/clmeg_repo/jc2_utils.sh
#echo source /raid1/cornejo/clmeg_repo/repo.sh jl_2.0
source /raid1/cornejo/clmeg_repo/jc2_utils.sh
#source /raid1/cornejo/clmeg_repo/repo.sh jl_2.0
source /usr/local/megapps/repo.sh jl_2.2

echo server is $SERVER
echo workdir is $WORKDIR
echo permdir is $PERMDIR
echo servpermdir is $SERVPERMDIR
echo hostname of node is `hostname`
echo ------------------------------------------------------
echo -n 'Job is running on node '; cat "NODEFILE"
echo ------------------------------------------------------
echo ' '
echo ' '
RUNMACRO="run.mac"
## List of macros that are needed
DEPENDSFILE="depends.dat"

## Some helper functions
copy_action=false
myDeepCopyHelper()
{
  last_action=false
  if [ ! -e $1 ];
  then
    dirname=`dirname $1`
    if [ "x${dirname}" != "x" ];
    then
      mkdir -p ${dirname}
    fi
    printf "Copying $1 to local disk:  "
    if  ${SCP} $2/$3$1 $1;
    then
      echo "[Success!]"
      copy_action=true
    else
      echo "[Failed!]"
    fi
  fi
}

myDeepCopy()
{
  myDeepCopyHelper $1 ${SERVPERMDIR} $2
}

myDeepCopyBin()
{
  myDeepCopyHelper $1 ${SERVPERMBINDIR} $2
}

myDeepCopyField()
{
  myDeepCopyHelper $1 ${SERVFIELDDIR} $2
}

myBatchCopy()
{
  myDeepCopy $1 batch/
}


myMacroCopy()
{
  ## Only copy if it doesn't already exist
  myDeepCopy $1 $2
  if [ ${copy_action} ]; # Only proceed if it actually copied (and doesn't already exist)
  then
    ## Now see if this has other dependencies, called via /control/execute
    ## Filter out any comments, and look for any non-commented /control/execute.
    ## If a macro name is reference, call myMacroCopy on this file
    ext_macros=`\
      sed -s 's|#.*||g' -- $1 | \
      grep -v '^$' | \
      grep "/control/execute" | \
      sed -s 's|/control/execute||' `
    for f in ${ext_macros[@]};
    do
      myMacroCopy ${f}
    done

    ## Look for a magnetic field that is being specified and copy that
    ## over if needed.
    mag_field=`\
      sed -s 's|#.*||g' -- $1 | \
      grep -v '^$' | \
      grep "/g4sbs/tosfield" | \
      sed -s 's|/g4sbs/tosfield||' `
    for f in ${mag_field[@]};
    do
      #echo "Copying tosca field map: ${SERVFIELDDIR}/${f} -> ${f}"
      myDeepCopyField ${f}
      #ls -lh ${f}
      ## Make the subdirectories if needed
      #fdirname=`dirname ${f}`
      #if [ "x${fdirname}" != "x" ];
      #then
      #  mkdir -p ${fdirname}
      #fi

      ## Copy magnetic field
      #echo "Copying tosca field map: ${SERVFIELDDIR}/${f} -> ${f}"
      #${SCP} ${SERVFIELDDIR}/${f} ${f}
    done
    ## findout if we need to copy the BB field too
    bb_field=`\
      sed -s 's|#.*||g' -- $1 | \
      grep -v '^$' | \
      grep "/g4sbs/bbfield" | \
      sed -s 's|/g4sbs/bbfield||' | \
      sed -s 's| ||g' `
    if [ "x${bb_field}" == "x1" ];
    then
      #echo "Copying tosca field map: map_696.dat"
      myDeepCopyField map_696A.dat
      #${SCP} ${SERVFIELDDIR}/map_696A.dat map_696A.dat
      #ls -lh map_696A.dat
    fi

  fi
}


currentstage='NONE'

printbanner()
{
  printf "\n############ %6s: %-20s #############\n" "$1" "$2"
}
printstart()
{
  printbanner "START" "$currentstage"
}

printend()
{
  printbanner "END" "$currentstage"
}



###############################################################
#                                                             #
#    Transfer files from server to local disk.                #
#                                                             #
###############################################################
stagein()
{

  currentstage='STAGE IN'
  printstart
  echo "In Work directory ${WORKDIR}"


  mkdir -p ${WORKDIR}
  cd ${WORKDIR}

  ## Copy over the binary
  #${SCP} ${SERVPERMBINDIR}/g4sbs g4sbs
  #${SCP} ${SERVPERMBINDIR}/G4SBSDict_rdict.pcm G4SBSDict_rdict.pcm
  myDeepCopyBin g4sbs
  myDeepCopyBin G4SBSDict_rdict.pcm

  ## Copy over the mandatory field map for BB
  myDeepCopyField map_696A.dat

  ## Copy pre and post macros (and also copy any external macros referenced)
  myMacroCopy ${PREMACRO}
  myMacroCopy ${POSTMACRO}
  
  ## Copy over macros needed to shrink root file
  #if [ "x${SHRINKROOTFILE}" != "x" ];
  #then
  #  echo "Copying SHRINKMACROFILELIST"
  #  for cpFile in ${SHRINKMACROFILELIST[@]};
  #  do
  #    echo "Copying macros/${cpFile}"
  #    ${SCP} ${SERVPERMDIR}/macros/${cpFile} .
  #    ls ${cpFile}
  #  done
  #fi


  ## Ensure necessary directories are created locally
  mkdir -p ${ROOTFILEPATH}

  ## Create run macro
  echo
  echo "/control/execute ${POSTMACRO}" > ${RUNMACRO}
  if [ "x${POST_TEMPLATE}" != "x" ];
  then
    myMacroCopy ${POST_TEMPLATE}
    cat ${POST_TEMPLATE} >> ${RUNMACRO}
  fi
  echo "/g4sbs/eventstatusevery ${PRINTEVERY}" >> ${RUNMACRO}
  echo "/g4sbs/filename ${ROOTFILE}" >> ${RUNMACRO}
  echo "/g4sbs/run ${NUMEVENTS}" >> ${RUNMACRO}

  echo "Directory listing of ${WORKDIR}"
  ls

  printend
}

############################################################
#                                                          #
#    Execute the run.  Do not run in the background.       #
#                                                          #
############################################################

runprogram()
{
  echo ' '
  echo ' ############ START: RUN PROGRAM #############'
  echo ' '
  cd ${WORKDIR}
  echo "In directory `pwd`"

  ${SRUN} ./g4sbs --pre=${PREMACRO} --post=${RUNMACRO}

  #if [ "x${SHRINKROOTFILE}" != "x" ];
  #then
  #  echo "Shrinking ROOT file"
  #  ${SRUN} root -b -q shrinkPulser.C\(\"${ROOTFILE}\",\"${ROOTFILE_SMALL}\"\)
  #fi

  echo ' '
  echo ' ############ END: RUN PROGRAM #############'
  echo ' '
}

###########################################################
#                                                         #
#   Copy necessary files back to permanent directory.     #
#                                                         #
###########################################################

stageout()
{
  echo ' '
  echo ' ############ START: STAGE OUT #############'
  echo ' '
  cd ${WORKDIR}
  echo "ls ${WORKDIR}"
  ls
  echo "ls ${ROOTFILEPATH}"
  ls ${ROOTFILEPATH}

  ## Copy output rootfile
  if [ "x${SHRINKROOTFILE}" != "x" ];
  then
    ${SCP} ${ROOTFILE_SMALL} ${SERVROOTFILEDIR}/${ROOTFILE}
  else
    ${SCP} ${ROOTFILE} ${SERVROOTFILEDIR}/${ROOTFILE}
  fi

  echo ' '
  echo ' ############ END: STAGE OUT #############'
  echo ' '
}

#####################################################################
#                                                                   #
#  React to early termination signal.                               #
#                                                                   #
#####################################################################

early()
{
  echo ' '
  echo ' ############ WARNING:  EARLY TERMINATION #############'
  echo ' '
}

trap 'early; stageout' 2 9 15


##################################################
#                                                #
#   Staging in, running the job, and staging out #
#   were specified above as functions.  Now      #
#   call these functions to perform the actual   #
#   file transfers and program execution.        #
#                                                #
##################################################

echo "Sleeping for ${SLEEP_TIME}"
sleep ${SLEEP_TIME}
stagein
runprogram
stageout

###############################################################
#                                                             #
#   The epilogue script automatically deletes the directory   #
#   created on the local disk (including all files contained  #
#   therein.                                                  #
#                                                             #
###############################################################

exit 0

