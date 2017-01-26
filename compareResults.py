#!/usr/bin/env python

import sys
import os
import stat ## For status/attributes of a file
import re # For regular expressions
import subprocess ## So we can run commands
import tempfile ## For temporary file/directory creation
import glob ## So we can get a glob of all files

goldenKins = []
otherKins = []

## Data for each exclusion zone setting
class ExclusionData:
  def __init__(self,width,negIDSigmaP,negIDSigmaN,posIDSigmaP,posIDSigmaN,
      negIDSolP,negIDSolN,posIDSolP,posIDSolN):
    self.width = float(width)
    self.negIDSigmaP  = float(negIDSigmaP)
    self.negIDSigmaN  = float(negIDSigmaN)
    self.posIDSigmaP  = float(posIDSigmaP)
    self.posIDSigmaN  = float(posIDSigmaN)
    self.negIDSolP    = float(negIDSolP)
    self.negIDSolN    = float(negIDSolN)
    self.posIDSolP    = float(posIDSolP)
    self.posIDSolN    = float(posIDSolN)

  ## Print debug info
  def __repr__(self):
    return ',width=%g,misP=%g,misN=%g,posP=%g,posN=%g' % (self.width,
        self.negIDSigmaP,self.negIDSigmaN,
        self.posIDSigmaP,self.posIDSigmaN)

## A class of data for each kin setting
class KinData:
  def __init__(self,kin,config,setID):
    self.kin = kin
    self.config = config ## I also call this dateCmd or date in some scripts
    self.setID = setID
    self.values = {}
    self.units = {}
    self.exclusions = []
    self.golden = False
    self.comment = ''

  ## Print out details of this kin set
  def __repr__(self):
    result = 'kin=%02d,config=%s,set=%d'% (self.kin, self.config, self.setID)
    result += ',pn_vertical_sep=%g %s' % (self.values['pn_vertical_sep'],
        self.units['pn_vertical_sep'])
    for ex in self.exclusions:
      result += ex.__repr__()
    return result

  ## Compare Kin sets for sorting purposes
  def __cmp__(self,other):
    ## Sort by kin, config, setID
    if self.kin == other.kin:
      if self.config == other.config:
        return other.setID - other.setID
      else:
        return 2*(self.config.lower() < other.config.lower())-1
    else:
      return self.kin - other.kin

  def readData(self,csv):
    f = open(csv,'r')
    for line in f:
      items = line.strip().split(',')
      ## Skip empty lines
      if len(items) < 2:
        continue

      ## Check special valued lines
      if items[0] == 'golden':
        if (items[1].lower() == 'yes' or items[1].lower() == '1'):
          self.golden = True
        else:
          self.golden = False ## The default, but doesn't hurt to set it again
      elif items[0] == 'exclusion_zone':
        ex = ExclusionData(items[1],
            items[2],items[3],items[4],items[5],
            items[6],items[7],items[8],items[9])
        self.exclusions.append(ex)
      elif items[0] == 'comment':
        self.comment = items[1]
      else:
        self.values[items[0]] = float(items[1])
        if len(items) > 2:
          self.units[items[0]] = items[2]

  def printKinSummary(self):
    print "%2d  %s %2d  %12.8f %12.8f" % (self.kin,self.config.center(10),
        self.setID,
        self.values['total_sigma_proton'], self.values['total_sigma_neutron'])

## Main Function (program starts here)
def main():
  files = glob.glob("results/db/*.csv")
  for csvFile in files:
    ## Remove begining of string
    fileName = csvFile.replace('results/db/','').replace(".csv",'')
    tmp = re.split('_',fileName)
    config = tmp[0]
    kin  = int(tmp[1].replace("kin",''))
    setID  = int(tmp[2].replace("set",''))
    data = KinData(kin,config,setID)
    data.readData(csvFile)
    if data.golden:
      goldenKins.append(data)
    else:
      otherKins.append(data)

  ## Sort the kinematics
  goldenKins.sort()
  otherKins.sort()

  ## Print
  print "Golden Kinematics:"
  print "Kin %s Set %s %s %s %s %s" % ( 'config'.center(10), 'Proton'.center(13),
      'Neutron'.center(13), 'AvgSep'.center(12), 'ThetaPQ'.center(12),
      'Comment'.center(12))
  for k in goldenKins:
    k.printKinSummary()
  print "\n\n\nOtherKinematics:"
  print "Kin %s Set %s %s %s %s %s" % ( 'config'.center(10), 'Proton'.center(15),
      'Neutron'.center(15), 'AvgSep'.center(15), 'ThetaPQ'.center(15),
      'Comment'.center(15))
  for k in otherKins:
    k.printKinSummary()

## Run main() function if this file is run directly in the command line
## (as opposed to being imported)
if __name__ == '__main__':
  main()
