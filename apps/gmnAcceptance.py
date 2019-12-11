#!/usr/bin/env python3

## Highlight any changes in PerFirst row of greater than this value
PerFirst_EffectThr = 0.5
## Highlight any changes compared to global Golden run settings
Golden_EffectThr = 0.5
## Highlight those percent of misidentified values that are bad
PerTotal_EffectThr = 0.99

www_path='results/db/web'

import math;

from GMn import *

from GMnDB import *
db = GMnDB()

from gmnAcceptanceStdInfo import *;

import os ## Access os features (like modifying filesystem)
from jinja2 import Environment, FileSystemLoader
j2_env = Environment(loader=FileSystemLoader(os.path.dirname(os.path.abspath(__file__))))
template_GoldenSummary = j2_env.get_template('templates/gmnAcceptance/GoldenSummary.html')
template_ConfigSetSummary = j2_env.get_template('templates/gmnAcceptance/ConfigSetSummary.html')
template_Compare = j2_env.get_template('templates/gmnAcceptance/Compare.html')
template_Proposal = j2_env.get_template('templates/gmnAcceptance/Proposal.html')
template_KinSummary = j2_env.get_template('templates/gmnAcceptance/KinSummary.html')
template_Styles = j2_env.get_template('templates/gmnAcceptance/styles.css')

stdErrVal = -4e-6


## So we can make a default dictionary of lists
from collections import defaultdict
configsets = defaultdict(list)
configsetsgolden = []
currentConfig = None

def getIntLuminosity(kin):
  ## Given in x10^38/cm^2
  ## Using A = 2 and 1 hr = 3600 seconds
  return (float(GMnProposalInfo['Luminosity'][kin])/2)*(float(
      GMnProposalInfo['RunningTime'][kin])*3600)

def computeStats(sigmaN,sigmaP,kin):
  eventsN = computeExpectedCounts(sigmaN,kin,0)
  eventsP = computeExpectedCounts(sigmaP,kin,1)
  errN = 1.0/math.sqrt(eventsN)
  errP = 1.0/math.sqrt(eventsP)
  return 100.*math.sqrt(errN**2+errP**2)

def percentOf(val1,val2):
  return 100.0*(float(val1)/float(val2))

def percentDiff(val1,val2):
  return percentOf(val1,val2)-100.

def average(val1,val2):
  return (val1+val2)/2.

def processPercentDiff(var,data,golden,sign=0):
  val = 100.0*(data[var]/golden[var]-1.0)
  data['pdiff_%s'%var] = val
  effect = ''
  if sign == 0:
    return
  val *= sign
  if val < -Golden_EffectThr:
    effect = 'effect_bad'
  elif val > Golden_EffectThr:
    effect = 'effect_good'
  data['class_%s'%var] = effect


def processValues(vals,golden={}):
  global configSet
  results = vals
  results['average_solid'] = average(vals['total_solid_proton'],
      vals['total_solid_neutron'])
  ## Check if thetapq is in dictionary (not the radius, but the actual angle)
  if 'thetapq' not in results:
      ## Not in here, so calculate it
      results['thetapq'] = round(results['thetapq_radius']/(100.*
              currentConfig['dist_hcal'])*180./math.pi,1)
  if len(golden): ## That means golden is not empty, so we do comparisons
    myvars = ( ('total_sigma_proton',1), ('total_sigma_neutron',1),
        ('average_solid',1), ('thetapq_radius',0), ('pn_vertical_sep',0),
        ('thetapq',0) )
    for v in myvars:
      processPercentDiff(v[0],results,golden,v[1])
  return results


class ExResults:
  ## Constructor
  def __init__(self):
    self.vals = {}
    self['dmz'] = 0.0
    for h in ('P', 'N'):
      for iden in ('neg%sSigmaID'%h, 'pos%sSigmaID'%h):
        self['%s'%iden] = 0.0
        self['%sPerTotal'%iden] = stdErrVal
        self['%sPerOtherPosID'%iden] = stdErrVal
        self['%sPerFirst'%iden] = stdErrVal
        self['class_%sPerTotal'%iden] = ''
        self['class_%sPerOtherPosID'%iden] = ''
        self['class_%sPerFirst'%iden] = ''

  def setParent(self,parent):
    self['dmz'] = parent.width
    self['negPSigmaID']             = parent.negIDSigmaP
    self['posPSigmaID']             = parent.posIDSigmaP
    self['negNSigmaID']             = parent.negIDSigmaN
    self['posNSigmaID']             = parent.posIDSigmaN
    ## Compute the same but take into account the hadron detection efficiency
    neff = getHCalEfficiencyByMomentum(currentConfig['scat_h_p'],0)
    peff = getHCalEfficiencyByMomentum(currentConfig['scat_h_p'],1)
    self['negPSigmaIDEff']          = parent.negIDSigmaP*peff
    self['posPSigmaIDEff']          = parent.posIDSigmaP*peff
    self['negNSigmaIDEff']          = parent.negIDSigmaN*neff
    self['posNSigmaIDEff']          = parent.posIDSigmaN*neff
    self.computeRatios()
    ## Estimate the statistical error on these
    self['rate_neutron']  = computeExpectedHourlyCounts(self['posNSigmaID'],
        currentConfig['kin'],0)
    self['rate_proton']  = computeExpectedHourlyCounts(self['posPSigmaID'],
        currentConfig['kin'],1)
    self['counts_neutron']  = computeExpectedCounts(self['posNSigmaID'],
        currentConfig['kin'],0)
    self['counts_proton']  = computeExpectedCounts(self['posPSigmaID'],
        currentConfig['kin'],1)
    self['stat_error'] = computeStats(self['posNSigmaID'],
        self['posPSigmaID'],currentConfig['kin'])
    ## Compare our estimated counts vs proposal
    self['counts_neutron_pdiff_proposal'] = percentDiff(self['counts_neutron'],
      GMnProposalInfo['CountsNeutron'][currentConfig['kin']])
    #self['counts_proton_pdiff_proposal'] = percentDiff(self['counts_proton'],
    #  GMnProposalInfo['counts_proton_calculated'][currentConfig['kin']])
    ## Compute the various percentage hadron cross sections
    self.__computePer('OtherPosID','negP','posN')
    self.__computePer('OtherPosID','negN','posP')

  def __computeRatios(self,witheff=0):
    pref = 'ratio_'
    post = ''
    if witheff == 1:
      pref = 'ratio_eff_'
      post = 'Eff'
    ## Compute the measured ratio and the ratio due to just positive
    self[pref+'posID'] = self['posNSigmaID'+post]/self['posPSigmaID'+post]
    if self['negPSigmaID'] > 0:
      self[pref+'negID'] = self['negNSigmaID'+post]/self['negPSigmaID'+post]
    else:
      self[pref+'negID'] = 0
    top    = self['posNSigmaID'+post] + self['negPSigmaID'+post]
    bottom = self['posPSigmaID'+post] + self['negNSigmaID'+post]
    self[pref+'measured'] = top/bottom
    self[pref+'overestimate'] = self[pref+'measured']/self[pref+'posID']
    self['pdiff_'+pref+'overestimate'] = (self[pref+'overestimate']-1.0)*100.

  def computeRatios(self):
    ## Compute the measured ratio and the ratio due to just positive
    ## identification
    ## Without hadron detection efficiency factor
    self.__computeRatios(0)
    ## With hadron detection efficiency factor
    self.__computeRatios(1)

  def __computePer(self,per,var1,var2,sign=0):
    val = 100.0*self[var1+'SigmaID']/self[var2+'SigmaID']
    self[var1+'SigmaIDPer'+per] = val
    effect = ''
    #if sign == 0:
    #  return
    #val = sign*self[var1+'SigmaIDPer'+per]
    #if val < -PerTotal_EffectThr:
    #  effect = 'effect_bad'
    #elif val > PerTotal_EffectThr:
    #  effect = 'effect_good'
    #self['class_'+var+'SigmaIDPer'+per] = effect


  def __computePerTotal(self,var,parentVal,sign=0):
    val = 100.0*self['%sSigmaID'%var]/parentVal;
    self['%sSigmaIDPerTotal'%var] = val
    effect = ''
    if sign == 0:
      return
    val = sign*self['%sSigmaIDPerTotal'%var]
    if val < -PerTotal_EffectThr:
      effect = 'effect_bad'
    elif val > PerTotal_EffectThr:
      effect = 'effect_good'
    self['class_%sSigmaIDPerTotal'%var] = effect


  def computePerTotal(self,sigmaTotal):
    ## These are Protons (Neutrons) which were mis-identified as
    ## Neutrons(Protons), so the ratio is with respect to total
    ## integrated cross section with no ID-cut of the Neutron (Proton)
    self.__computePerTotal('negP',sigmaTotal['total_sigma_neutron'],-1)
    self.__computePerTotal('posP',sigmaTotal['total_sigma_neutron'])
    self.__computePerTotal('negN',sigmaTotal['total_sigma_proton'],-1)
    self.__computePerTotal('posN',sigmaTotal['total_sigma_proton'])

  def __computePerFirst(self,var,first,sign=1):
    self['%sSigmaIDPerFirst'%var] = 100.0*(self['%sSigmaID'%var]/
        first['%sSigmaID'%var]-1.0)
    effect = ''
    if sign == 0:
      return
    val = sign*self['%sSigmaIDPerFirst'%var]
    if val < -PerFirst_EffectThr:
      effect = 'effect_bad'
    elif val > PerFirst_EffectThr:
      effect = 'effect_good'
    self['class_%sSigmaIDPerFirst'%var] = effect

  def computePerFirst(self,first):
    self.__computePerFirst('negP',first,-1)
    self.__computePerFirst('posP',first,1)
    self.__computePerFirst('negN',first,-1)
    self.__computePerFirst('posN',first,1)

  ## Overloaded operator to set items via the [] command
  def __setitem__(self,key,val):
    self.vals[key] = val

  ## Overloaded operator to get items via the [] command
  def __getitem__(self,key):
    return self.vals[key]

def buildGoldenExData(data,nocut_data):
  results = []
  is_first = 1
  first_data = ExResults()
  #index = 0
  for d in data:
    #index += 1
    #if index > DisplayMaxEX:
    #  break
    ex = ExResults()
    ex.setParent(d)
    ex.computePerTotal(nocut_data)
    if is_first:
      first_data = ex
      is_first = 0
    else:
      ex.computePerFirst(first_data)
    results.append(ex)
  return results,first_data

def buildExData(data,values,golden_data):
  results = []
  #index = 0
  for d in data:
    #index += 1
    #if index > DisplayMaxEX:
    #  break
    ex = ExResults()
    ex.setParent(d)
    ex.computePerTotal(values)
    ex.computePerFirst(golden_data)
    results.append(ex)
  return results

class ConfigSet:
  ## Constructor
  def __init__(self,kin,config,setID,Q2,vals,golden):
    self.vals = vals
    self.vals['kin'] = kin
    self.vals['Q2'] = Q2
    self.vals['config'] = config
    self.vals['set'] = setID
    self.vals['golden'] = golden
    #self.vals['luminosity'] = GMnProposalInfo[kin]['Luminosity']
    #self.vals['running_time'] = GMnProposalInfo[kin]['RunningTime']
    self.vals['integrated_luminosity'] = getIntLuminosity(kin)
    if golden:
      self.vals['golden_class'] = 'golden_kin'
    ## Calculate some of the outgoing energies
    GMnComputeKinematics(self)

  def __setitem__(self,key,val):
    self.vals[key] = val

  def __getitem__(self,key):
    return self.vals[key]

  ## Comparator for sorting purposes
  def __lt__(self,other):
    if self['kin'] != other['kin']:
      return self['kin'] < other['kin']
    elif self['config'] == other['config']:
      return self['set'] < other['set']
    return self['config'] < other['config']

class CompareProposal:
  def __init__(self,results,kin):
    self.vals = results
    self['ProposalCountsNeutron'] = GMnProposalInfo['CountsNeutron'][kin]
    self['SimulationCountsNeutron'] = self['exclusions'][0]['counts_neutron']
    self.doCompare('CountsNeutron')

  ## Overloaded operator to set items via the [] command
  def __setitem__(self,key,val):
    self.vals[key] = val

  ## Overloaded operator to get items via the [] command
  def __getitem__(self,key):
    return self.vals[key]

  ## Do actual comparison
  def doCompare(self,var):
    self['PDiff'+var] = percentDiff(self['Simulation'+var],self['Proposal'+var])

## Main Function (program starts here)
def main():
  global dates, voffs, currentConfig

  ## Load up the results database
  db.loadDB()

  ## Load up the proposal information
  readGMnProposal()

  ## A variable that will hold all the results
  goldens = []

  hcal_voff = 45 ## Only consider the 45cm offset for now
  results_compare_proposal = []
  for kin in range(1,8):
    ## Get the golden run for this kin
    tmp = db.getGoldenByKin(kin)
    results = {}
    results['golden'] = []
    results['proposal'] = []
    results['others'] = []
    golden = {}
    golden['golden'] = 1
    golden['proposal'] = 0
    golden['Q2'] = tmp.Q2;
    golden['kin'] = tmp.kin;
    golden['config'] = tmp.config
    golden['comment'] = tmp.comment
    golden['set'] = tmp.setID
    currentConfig = ConfigSet(tmp.kin,tmp.config,tmp.setID,tmp.Q2,tmp.gen_config,tmp.golden)
    golden['values'] = processValues(tmp.hcal_set[hcal_voff].values)
    golden_ex = ExResults()
    golden['exclusions'],golden_ex = buildGoldenExData(
        tmp.hcal_set[hcal_voff].exclusions,golden['values'])
    goldens.append(golden)
    results['golden'].append(golden)
    configsets['%s/%s'%(tmp.config,tmp.setID)].append(currentConfig)
    configsetsgolden.append(currentConfig)

    ## Get the other non golden kinematics
    otherKins = db.getNonGoldenByKin(kin)
    for k in otherKins:
      result = {}
      result['golden'] = 0
      result['proposal'] = 0
      result['Q2'] = k.Q2;
      result['kin'] = k.kin;
      result['config'] = k.config
      result['comment'] = k.comment
      result['set'] = k.setID
      currentConfig = ConfigSet(k.kin,k.config,k.setID,k.Q2,k.gen_config,
              k.golden)
      ## Set the hcal vertical offset desired (the same for all except for
      ## proposal numbers which are forced to be at 0)
      hvoff = hcal_voff
      if k.config.startswith('proposal'):
        hvoff = 0
      result['values'] = processValues(k.hcal_set[hvoff].values,golden['values'])
      result_ex = ExResults()
      result['exclusions'] = buildExData(
          k.hcal_set[hvoff].exclusions,result['values'],golden_ex)
      configsets['%s/%s'%(k.config,k.setID)].append(currentConfig)
      ## Finally, store the results
      if k.config.startswith('proposal'):
        result['proposal'] = 1
        results_compare_proposal.append(CompareProposal(result,k.kin))
        results['proposal'].append(result)
      else:
        results['others'].append(result)

    ## Get the Particle ID info for this kinematic
    outKinSummary = open(www_path+'/gmnAcceptance_Kin%02d_Summary.html' %
        (tmp.kin),'w',encoding='utf-8')
    outKinSummary.write(template_KinSummary.render(stdErrVal=stdErrVal,
      golden=golden,info=info,results=results,proposal=GMnProposalInfo
      ))

  #out = open(www_path+'/gmnAcceptance.html','w')
  outGoldenSummary = open(www_path+'/gmnAcceptance_GoldenSummary.html','w',
          encoding='utf-8')
  outGoldenSummary.write(template_GoldenSummary.render(
    goldens=goldens,configsetsgolden=configsetsgolden,info=info))

  for i in configsets:
    configsets[i].sort()
  #configsets.sort()
  outConfigSetSummary = open(www_path+'/gmnAcceptance_ConfigSetSummary.html',
          'w',encoding='utf-8')
  outConfigSetSummary.write(template_ConfigSetSummary.render(
    configsets=configsets,info=info,proposal=GMnProposalInfo,compare=results_compare_proposal))

  ## Proposal numbers
  outProposal = open(www_path+'/gmnAcceptance_Proposal.html','w',
          encoding='utf-8')
  #outProposal.write(template_Proposal.render(
  #  configsets=configsets,info=info,proposal=GMnProposalInfo))

  ## Test only (compare to proposal)
  outCompare = open(www_path+'/gmnAcceptance_Compare.html','w',encoding='utf-8')
  outCompare.write(template_Compare.render(
    configsets=configsets,info=info,proposal=GMnProposalInfo,compare=results_compare_proposal))
  ## Copy the style sheet over too
  outStyle = open(www_path+'/gmnAcceptance_style.css','w',encoding='utf-8')
  outStyle.write(template_Styles.render(info=info))

  ## Done!!
  print("Done!")

  #db.printDebug()

## Run main() function if this file is run directly in the command line
## (as opposed to being imported)
if __name__ == '__main__':
  main()
