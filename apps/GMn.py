## Standard GMn Configuration and Utilities

##
LiveTime = 0.80 ## From original proposal page 44
BigBiteEfficiency = 0.75 ## From original proposal page 44
## Convert proposal luminosity given in x10^-38/A/cm^2/s to /fb/hr
LuminosityToFbHr = 360./2.

import math;
from collections import OrderedDict

## Read in additional information from the proposal
GMnProposalInfo = OrderedDict() ## Because I want to keep the order I read them in

## Efficiencies from original proposal with BigHAND
## Not really used much, except for comparison
def getBigHANDEfficiency(kin,particle):
  proton  = ( 0, 78.4, 86.0, 93.8, 98.2, 96.5, 99.0, 99.0 )
  neutron = ( 0, 73.0, 80.9, 86.6, 86.6, 91.3, 91.1, 91.1 )
  if particle == 0:
    return neutron[kin]/100
  elif particle == 1:
    return proton[kin]/100

class HCalEfficiencyTable:
  def __init__(self):
    self.protonP = []
    self.protonE = []
    self.neutronP = []
    self.neutronE = []
    self.readEfficiencies(self.protonP,self.protonE,'proton')
    self.readEfficiencies(self.neutronP,self.neutronE,'neutron')

  def readEfficiencies(self,momenta,effs,name):
    f = open('efficiencies/hcal_'+name+'.txt','r')
    for line in f:
      ## Get rid of the new-line stuff at the end of each line
      line = line.rstrip();
      ## Get stuff only without comments
      line = line.split('#',1)[0]
      ## Skip line if it is all comments
      if not line:
        continue
      ## Finally, split on comma
      items = line.rsplit(',')
      if len(items) != 2:
        print("Skipping bad efficiency line")
        continue
      momenta.append(float(items[0]))
      effs.append(float(items[1])/100.)

## HCal efficiencies taken from the updated proposal
HCalEffs = HCalEfficiencyTable()
def getHCalEfficiencyByMomentum(momentum,particle):
  if particle == 1:
    for i,p in enumerate(HCalEffs.protonP):
      if momentum <= p:
        return HCalEffs.protonE[i]
  elif particle == 0:
    for i,p in enumerate(HCalEffs.neutronP):
      if momentum <= p:
        return HCalEffs.neutronE[i]
  return 0.0

## HCal efficiencies taken from the updated proposal
## These are estimated from the proposal from what is "known" about
## each kinematic point.
def getHCalEfficiency(kin,particle):
  momenta = (1e6, 2.64, 3.20, 4.03, 5.39, 6.20, 7.27, 8.08)
  return getHCalEfficiencyByMomentum(momenta[int(kin)],particle)

## Returned luminosity will be set in /fb/hr
def getLuminosity(kin,unit=1):
  return float(GMnProposalInfo['Luminosity'][kin])/unit

def getRunningTime(kin,unit=1):
  ## We can do "all" running time if that's prefered. But for now, only
  ## the full luminosity running is considered
  return float(GMnProposalInfo['RunningTime'][kin])
  #return float(GMnProposalInfo['RunningTime'][kin]) + \
  #    0.5*float(GMnProposalInfo['RunningTimeLHalf'][kin]) + \
  #    0.1*float(GMnProposalInfo['RunningTimeL10Per'][kin])*unit

def computeExpectedHourlyCounts(sigma,kin,particle):
  #eff = LiveTime*BigBiteEfficiency*getBigHANDEfficiency(kin,particle)
  eff = LiveTime*BigBiteEfficiency*getHCalEfficiency(kin,particle)
  return getLuminosity(kin)*sigma*eff

def computeExpectedCounts(sigma,kin,particle):
  return computeExpectedHourlyCounts(sigma,kin,particle)*getRunningTime(kin)

def readGMnProposal(filename='kinematics/proposal_info.txt'):
  global GMnProposalInfo
  ## Open file
  f = open(filename,'r')
  for line in f:
    ## Get rid of the new-line stuff at the end of each line
    line = line.rstrip();
    ## Get stuff only without comments
    line = line.split('#',1)[0]
    ## Skip line if it is all comments
    if not line:
      continue
    ## Finally, split on whitespaces
    items = line.rsplit()
    ## Ensure there are exactly 8 items
    if len(items) != 8:
      print("Skipping bad proposal line, with only %d entries: %s" %(
          len(items),line))
      continue

    ## Alright, we can proceed. Add this new line to the dictionary
    GMnProposalInfo[items[0]] = [0,1,2,3,4,5,6,7]
    ## Now add all the kins
    for kin in range(1,8):
      GMnProposalInfo[items[0]][kin] = items[kin]
  ## Now that we have read the proposal fix some units
  ## Convert proposal luminosity given in x10^-38/A/cm^2/s to /fb/hr
  for kin in range(1,8):
    ## Trying to get rid of an annoying floating point error that happens
    ## when we do 0.7*1.80.  So multiply by 10 and then divide after the
    ## operation. Sigh...
    GMnProposalInfo['Luminosity'][kin] = (10.*LuminosityToFbHr * \
        float(GMnProposalInfo['Luminosity'][kin]))/10.
  ## and now let's do some calculations of our own
  #GMnProposalInfo['counts_proton_calculated'] = [0,1,2,3,4,5,6,7]
  #GMnProposalInfo['counts_neutron_calculated'] = [0,1,2,3,4,5,6,7]
  #GMnProposalInfo['CountsNeutronPDiff'] = [0,1,2,3,4,5,6,7]
  for kin in range(1,8):
    GMnProposalInfo['CountsNeutron'][kin] = float(
        GMnProposalInfo['CountsNeutron'][kin])
    #GMnProposalInfo['counts_proton_calculated'][kin] = computeExpectedCounts(
    #    float(GMnProposalInfo['SigmaPCoin'][kin]),kin,1)
    #GMnProposalInfo['counts_neutron_calculated'][kin] = computeExpectedCounts(
    #    float(GMnProposalInfo['SigmaNCoin'][kin]),kin,0)
    #GMnProposalInfo['CountsNeutronPDiff'][kin] = 100*(
    #    float(GMnProposalInfo['counts_neutron_calculated'][kin]) \
    #        /float(GMnProposalInfo['CountsNeutron'][kin])-1)
    #GMnProposalInfo['CountsNeutronPDiff'][kin] = '{:2.2f}%'.format(GMnProposalInfo['CountsNeutronPDiff'][kin])

## Compute the scattering kinematics (momenta)
def GMnComputeKinematics(config):
  Xb = 1.0 #
  ## Electron Arm
  Pa = config['beam_e'] # GeV
  Ma = 0.511e-3 # GeV
  CosThc = math.cos(config['theta_bb']*0.017453293)
  ## Hadron Arm
  Md = 0.938272046 # GeV
  CosThd = math.cos(config['theta_sbs']*0.017453293)
  Pc = (Pa*Md*Xb)/(Pa*(1-CosThc)+Md*Xb)
  q2 = Pa**2+Pc**2-2*Pa*Pc*CosThc
  q  = q2**0.5
  config['scat_e_p'] = Pc
  config['scat_h_p'] = q
