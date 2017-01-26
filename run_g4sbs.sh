PRESCRIPT="scripts/preinit_jc2_gmn.mac"
POSTSCRIPT=$1
LOGFILE=logs/`basename $1 .mac`.log

echo ">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>"
echo "Running g4sbs"
echo "PreInit: ${PRESCRIPT}"
echo "Post: ${POSTSCRIPT}"
echo "Logfile: ${LOGFILE}"
echo "Start time `date`"
time ../../build_g4sbs/g4sbs --pre=${PRESCRIPT} --post=${POSTSCRIPT} &> ${LOGFILE}
echo "Finished! `date`"
echo "<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<"
