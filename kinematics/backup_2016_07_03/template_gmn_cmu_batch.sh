#!/bin/bash -f

## Configure PBS parameters
#PBS -N g4sbs_k%%$kin%%\_r%%$runString%%\_voff%%$voffset%%
#PBS -j oe
#PBS -l     cput=16:00:00
#PBS -l walltime=16:00:00
#PBS -q %%$cmuQueue%%
#PBS -l mem=1gb
#PBS -m a
#PBS -M jcornejo\@andrew.cmu.edu
#PBS -l nodes=1:ppn=1

## Output useful job information
${HOME}/pbs/output_job_info.sh

## Configure server that has files we need
SERVER=$PBS_O_HOST
SERVER=megux16
WORKDIR=/scratch/PBS_$PBS_JOBID
SCP="/usr/bin/scp -l 100000"
SSH="/usr/bin/ssh"
PERMDIR=${HOME}/SBS/Simulation/g4sbs/workspace
SERVPERMDIR=${SERVER}:${PERMDIR}

echo server is $SERVER
echo workdir is $WORKDIR
echo permdir is $PERMDIR
echo servpermdir is $SERVPERMDIR
echo hostname of node is `hostname`
echo ------------------------------------------------------
echo -n 'Job is running on node '; cat $PBS_NODEFILE
echo ------------------------------------------------------
echo ' '
echo ' '

###############################################################
#                                                             #
#    Transfer files from server to local disk.                #
#                                                             #
###############################################################

stagein()
{
 cd ${WORKDIR}
 echo ' '

 echo Creating local disk structure
 mkdir -p %%$macPath%%
 mkdir -p %%$rootPath%%
 mkdir -p %%$pbsPath%%

 echo Transferring files from server to compute node
 echo Writing files in node directory  ${WORKDIR}

 echo Copying magnetic field maps
 ${SCP} ${SERVPERMDIR}/map_696A.dat .
 ${SCP} ${SERVPERMDIR}/fieldmaps/GEN-map0.table .
 echo Copying run macro
 ${SCP} ${SERVPERMDIR}/$macroFileName
 echo Copying the G4SBS executable
 ${SCP} ${SERVPERMDIR}/../../build_g4sbs/g4sbs .
 echo Copying the Pre-init script
 ${SCP} ${SERVPERMDIR}/scripts/preinit_jc2_gmn.mac scripts/
 echo Copying the post scripts
 ${SCP} ${SERVPERMDIR}/%%$macroFileName%% %%$macroFileName%%
 ${SCP} ${SERVPERMDIR}/%%$runMacroFileName%% %%$runMacroFileName%%


 echo Files in node work directory are as follows:
 ls -l
}

############################################################
#                                                          #
#    Execute the run.  Do not run in the background.       #
#                                                          #
############################################################

runprogram()
{
  echo Sourcing proton environment file
  source ${HOME}/environments/proton.sh
  echo Sourcing Geant4.10
  source ${HOME}/environments/geant410.sh
  cd ${WORKDIR}
  echo Executing G4SBS
  time ./g4sbs --pre=scripts/preinit_jc2_gmn.mac --post=%%$runMacroFileName%%
}

###########################################################
#                                                         #
#   Copy necessary files back to permanent directory.     #
#                                                         #
###########################################################

stageout()
{
 echo ' '
 echo Transferring files from compute nodes to server
 echo Writing files in permanent directory  \${PERMDIR}
 cd ${WORKDIR}

 echo All files in node workdir are
 ls -l

 echo Copying rootfile to ${SERVPERMDIR}/rootfiles/
 ${SCP} %%$rootFileName%% ${SERVPERMDIR}/%%$rootFileName%%

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

exit

