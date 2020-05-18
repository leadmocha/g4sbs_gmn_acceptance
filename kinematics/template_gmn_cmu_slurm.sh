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

if [ -e ${SLURM_LOCAL_CONFIG} ];
then
  source ${SLURM_LOCAL_CONFIG}
fi

# Configure server that has files we need
# Note that we set a default value following a format like so:
# : ${VAR_NAME:=DEFAULT-VALUE}
#: ${SERVER:=$SLURM_SUBMIT_HOST}
: ${SERVER:=qcd000}
: ${WORKDIR:=/scratch/PBS_$SLURM_JOB_ID}
: ${SCP:="/usr/bin/scp -l 200000"}
: ${SSH:="/usr/bin/ssh"}
: ${PERMDIR:=/raid1/cornejo/SBS/GMnAcceptance/workspace}
: ${PERMBINDIR:=/raid1/cornejo/SBS/GMnAcceptance/install_cluster7}
: ${FIELDDIR:=/raid1/cornejo/SBS/fieldmaps}

G4SBSFILES=("bin/g4sbs" "lib64/libg4sbsroot_rdict.pcm" "lib64/libg4sbsroot.rootmap" "lib64/libg4sbsroot.so" "lib64/libsbscteq.a")

SHRINKMACROFILELIST=("shrinkGMn.C" "shrinkGenerated.C" "shrinkGMn.h" )
#"g4sbs_data.h" "g4sbs_data.cxx")
RUNNUM=`echo ${SLURM_ARRAY_TASK_ID} | awk -F_ '{printf("%03d\n",$1)}'`

ROOTFILE=${ROOTFILEPATH}/${ROOTFILEPREFIX}${RUNNUM}.root
ROOTFILE_SMALL=${ROOTFILEPATH}/${ROOTFILEPREFIX_SMALL}${RUNNUM}.root

SERVPERMDIR=${SERVER}:${PERMDIR}
SERVPERMBINDIR=${SERVER}:${PERMBINDIR}
SERVFIELDDIR=${SERVER}:${FIELDDIR}

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
ERROR_FILE_NOT_COPIED=10
ERROR_FILE_NOT_COPIED_OUT=11

describeCode() {
  desc=""
  case "$1" in
    ${ERROR_FILE_NOT_COPIED})
      echo "FILE COPY IN ERROR"
      ;;
    ${ERROR_FILE_COPIED_OUT})
      echo "FILE COPY OUT ERROR"
      ;;
    *)
      echo "UNKNOWN ERROR CODE"
      ;;
  esac
}

## By default, exit with error code 0 to show success
EXIT_ERROR_CODE=0

needs_stage_out=false
currentstage='NONE'
errorExit() {
  echo ''
  echo '############ ERROR ENCOUNTERED - TERMINATING JOB #############'
  echo ''
  echo "Last Stage ${currentstage}"
  echo "Prior code: ${EXIT_ERROR_CODE}"
  echo "Generating error code: $1 - `describeCode $1`"
  echo "Error message: $2"
  # First, stage out if needed
  if [ ${needs_stage_out} ];
  then
    stageout
  fi
  
  echo ' '
  echo ' '
  echo '############ EXITING WITH ERROR $1 #############'
  exit $1
}

myCopyIn() {
  spdir=$1
  f=${2}
  fdir=`dirname $2`
  fname=`basename $2`
  if [[ "x$3" == "x" || "$3" == "." ]];
  then
    oname=$fname
  elif [ "$3" == "-" ];
  then
    oname=${2}
  else
    oname=${3}
  fi
  ff="${spdir}/${f}"
  echo "Copying ${ff} -> ${oname}"
  attempt=0
  scp_status=0
  while [[ ! -e ${oname} &&  ${attempt} -lt 3 ]];
  do
    if [ ${scp_status} != 0 ];
    then
      echo "Last scp_status=${scp_status}"
      sleep $((attempt*10));
    fi
    attempt=$((attempt+1))
    ${SCP} ${ff} ${oname}
    scp_status=$?
  done
  if [ -e ${oname} ];
  then
    echo "Successfully copied ${ff} -> ${oname} after ${attempt} attempts."
  else
    echo "ERROR: File not successfully copied after ${attempt} attempts."
    errorExit ${ERROR_FILE_NOT_COPIED} "File not successfully copied after ${attempt} attempts."
  fi
}


myCopyOut() {
  if [ ! -e $1 ];
  then
    echo "Error file ${1} does not exist. Cannot copy out!"
    EXIT_ERROR_CODE=${ERROR_FILE_NOT_COPIED_OUT}
  else
    ${SCP} $1 $2
  fi

}



function myBatchCopy()
{
  dirname=`dirname $1`
  if [ "x${dirname}" != "x" ];
  then
    mkdir -p ${dirname}
  fi
  ${SCP} ${SERVPERMDIR}/batch/$1 $1
}

function myMacroCopy()
{
  ## Only copy if it doesn't already exist
  if [ ! -e $1 ];
  then
    dirname=`dirname $1`
    if [ "x${dirname}" != "x" ];
    then
      mkdir -p ${dirname}
    fi
    echo "Copying $1 to local disk"
    #${SCP} ${SERVPERMDIR}/$2$1 $1
    myCopyIn ${SERVPERMDIR} $2$1 $1
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

    ## findout if we need to copy the BB field too
    bb_field=`\
      sed -s 's|#.*||g' -- $1 | \
      grep -v '^$' | \
      grep "/g4sbs/bbfield" | \
      sed -s 's|/g4sbs/bbfield||' | \
      sed -s 's| ||g' `
    bb_field=1 # This is because it seems to always be needed, sigh...
    if [ "x${bb_field}" == "x1" ];
    then
      echo "Copying tosca field map: map_696.dat"
      #${SCP} ${SERVFIELDDIR}/map_696A.dat map_696A.dat
      myCopyIn ${SERVFIELDDIR} map_696A.dat map_696A.dat
      ls -lh map_696A.dat
    fi

  fi
}



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

  ## Copy over the binary and libraries
  for gf in ${G4SBSFILES[@]};
  do
    myCopyIn ${SERVPERMBINDIR} ${gf}
    #echo "Copying ${SERVPERMBINDIR}/${gf} -> ."
    #${SCP} ${SERVPERMBINDIR}/${gf} .
  done
  #${SCP} ${SERVPERMBINDIR}/g4sbs g4sbs
  #${SCP} ${SERVPERMBINDIR}/G4SBSDict_rdict.pcm G4SBSDict_rdict.pcm

  ## Copy pre and post macros (and also copy any external macros referenced)
  myMacroCopy ${PREMACRO}
  myMacroCopy ${POSTMACRO}

  ## Look for a magnetic field that is being specified and copy that
  ## over if needed.
  if [ "x${FIELDMAP}" != "x" ];
  then
    ## Make the subdirectories if needed
    FHEADER=${FIELDMAP}.header
    FIELDMAP=${FIELDMAP}.map
    fdirname=`dirname ${FIELDMAP}`
    if [ "x${fdirname}" != "x" ];
    then
      mkdir -p ${fdirname}
    fi

    ## Copy magnetic field
    echo "Copying tosca field map header: ${SERVPERMDIR}/${FHEADER} -> ${FIELDMAP}"
    #${SCP} ${SERVPERMDIR}/${FHEADER} ${FIELDMAP}
    myCopyIn ${SERVPERMDIR} ${FHEADER} ${FIELDMAP}

    ## Also, copy the real field map with no headers, and concatenate the two
    FBODY=GMn_from_Bogdan_Kin03_20160806_no_header.table
    FBODYTMP=${fdirname}/field.body
    echo "Copying tosca field map body: ${SERVFIELDDIR}/${FBODY} -> ${FBODYTMP}"
    #${SCP} ${SERVFIELDDIR}/${FBODY} ${FBODYTMP}
    myCopyIn ${SERVFIELDDIR} ${FBODY} ${FBODYTMP}
    cat ${FBODYTMP} >> ${FIELDMAP}
    rm ${FBODYTMP}
    ls -lh ${FIELDMAP}*
  fi


  ## Copy over macros needed to shrink root file
  if [ "x${SHRINKROOTFILE}" != "x" ];
  then
    echo "Copying SHRINKMACROFILELIST"
    for cpFile in ${SHRINKMACROFILELIST[@]};
    do
      echo "Copying macros/${cpFile}"
      #${SCP} ${SERVPERMDIR}/macros/${cpFile} .
      myCopyIn ${SERVPERMDIR} ${cpFile} .
      ls ${cpFile}
    done
  fi

  ## Ensure necessary directories are created locally
  mkdir -p ${ROOTFILEPATH}

  ## Create run macro
  echo
  echo "/control/execute ${POSTMACRO}" > ${RUNMACRO}
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

  needs_stage_out=true
  ${SRUN} ./g4sbs --pre=${PREMACRO} --post=${RUNMACRO}

  if [ "x${SHRINKROOTFILE}" != "x" ];
  then
    echo "Shrinking ROOT file"
    ${SRUN} root -b -q ${SHRINKFILE}\(\"${ROOTFILE}\",\"${ROOTFILE_SMALL}\"\)
  fi

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
  ls
  ls ${ROOTFILEPATH}

  ## Copy output rootfile
  if [ "x${SHRINKROOTFILE}" != "x" ];
  then
    myCopyOut ${ROOTFILE_SMALL} ${SERVPERMDIR}/${ROOTFILE}
  else
    myCopyOut ${ROOTFILE} ${SERVPERMDIR}/${ROOTFILE}
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

exit ${EXIT_ERROR_CODE}

