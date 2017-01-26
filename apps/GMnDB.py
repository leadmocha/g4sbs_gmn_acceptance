import glob ## So we can get a glob of all files
from GMnKin import *

class GMnDB:
  def __init__(self):
    self.golden   = {}
    self.allKins  = {}

  def loadDB(self):
    files = glob.glob("results/db/*.csv")
    for csvFile in files:
      ## Remove begining of string
      fileName = csvFile.replace('results/db/','').replace(".csv",'')
      ind = fileName.index("_kin");
      config = fileName[:ind];
      kin = int(fileName[ind+4:ind+6]);
      setID = int(fileName[ind+7:]);
      #print "Config: %s, Kin: %02d, Set: %03d" % (config, kin, setID);
      data = GMnKin(kin,config,setID)
      data.readData(csvFile)
      name = '%s_k%d_s%d'%(config,kin,setID)
      self.allKins[name] = data;
      if data.golden:
        self.golden[name] = data;
    ## Sort the kinematics
    self.golden = sorted(self.golden.values())
    self.allKins = sorted(self.allKins.values())

  def getKins(config,setID):
    kins = []
    for k in range(1,7):
      kin = "%02d" % k;
      #kins.append(self.allKins[kin]

  def printDebug(self):
    print "Golden Kinematics Debug: "
    print self.golden
    print "Other Kineamtics Debug: "
    print self.allKins

  def getConfigSet(self,config,setID):
    result = {}
    for item in self.allKins:
      if item.config == config and item.setID == setID:
        result[item.kin] = item;
    return result;

  def getConfigSets(self,config):
    result = {}
    for item in self.allKins:
      if item.config == config:
        result[item.kin] = item;
    return result;

  ## Return the golden run by a specified kin
  def getGoldenByKin(self,kin):
    for item in self.golden:
      if item.kin == kin:
        return item
    ## If we have gotten this far, it means this kin has no golden run
    ## what a freaking shame!
    return None

  ## Return the non-golden run by a specified kin
  def getNonGoldenByKin(self,kin):
    results = []
    for item in self.allKins:
      if item.kin == kin and item.golden == 0:
        results.append(item)
    return results


  def getAllByKin(self,kin):
    result = []
    for item in self.allKins:
      if item.kin == kin:
        result.append(item)
    return result


