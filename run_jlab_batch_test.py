#!/usr/bin/env python

import sys
import os
import stat ## For status/attributes of a file
import re # For regular expressions
import subprocess ## So we can run commands
import tempfile ## For temporary file/directory creation



from xml.etree import ElementTree

kTmpPath = '/volatile/halla/sbs/cornejo/test_jlab_batch_file'
gTmpDir = ''
gBasePath = '/lustre/expphy/work/halla/sbs/cornejo/gmn_acceptance/g4sbs/workspace'

## Global variables
gDate = ''
gKin  = ''


## Run the script
def run(cmd,*args):
  return subprocess.call([cmd] + list(args))
## Main Function (program starts here)
def main():
  global gDate, gKin, gTmpDir
  gTmpDir = tempfile.mkdtemp(dir=kTmpPath)

  ## Open temporary run file
  runFileName = gTmpDir+'/run_test.csh'
  runFile = open(runFileName,'w')
  runFile.write('#!/bin/tcsh\n\n')
  runFile.write('## Specify any global variables\n')

  ## Open the batch file
  batchFileName = (gBasePath + '/jlab_batch/' + gDate +
      '/jlab_gmn_kin' + gKin + '_r000.xml')
  batchFile = open(batchFileName,'rt')
  tree = ElementTree.parse(batchFile)

  for node in tree.iter():
    if node.tag == 'Variable':
      runFile.write('setenv ' + node.attrib.get('name') + ' "' +
          node.attrib.get('value') +'"\n')
    elif node.tag == 'Command':
      runFile.write('\n\n## Now for the command section of the script\n\n ')
      runFile.write(node.text)

  ## Now change into the approriate directory and execute the command
  runFile.close()
  os.chdir(gTmpDir)
  st = os.stat(runFileName)
  os.chmod(runFileName, st.st_mode | stat.S_IEXEC)
  print("Will now run temporary file " + runFileName)
  run(runFileName)

## Run main() function if this file is run directly in the command line
## (as opposed to being imported)
if __name__ == '__main__':
  ## Ensure there are two parameters passed
  if len(sys.argv) <= 2:
    print("Must specify both date and kin number.")
    sys.exit(1)

  gDate = sys.argv[1]
  gKin  = sys.argv[2]

  ## Check that Kin is properly padded with zeros
  if len(gKin) < 2:
    gKin = '0' + gKin

  print("Starting test of jlab batchfile for Kin" + gKin + " " + gDate + " setting.")
  main()
