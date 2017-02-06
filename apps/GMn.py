## Standard GMn Configuration and Utilities

import math;

## Read in additional information from the proposal
GMnProposalInfo = {}

## Returned luminosity will be set in /fb/hr
def getLuminosity(kin,unit=1):
  return float(GMnProposalInfo['Luminosity'][kin])/unit

def getRunningTime(kin,unit=1):
  return float(GMnProposalInfo['RunningTime'][kin])*unit

def computeExpectedCounts(sigma,kin,particle):
  ## Give luminosity in terms of x10^38
  bbEff = 0.75;
  LiveTime = 0
  if particle == 0:
  return int(getLuminosity(kin)*getRunningTime(kin)*sigma)

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
      print "Skipping bad proposal line, with only %d entries: %s" %(
          len(items),line)
      continue

    ## Alright, we can proceed. Add this new line to the dictionary
    GMnProposalInfo[items[0]] = [0,1,2,3,4,5,6,7]
    ## Now add all the kins
    for kin in range(1,8):
      GMnProposalInfo[items[0]][kin] = items[kin]
  ## Now that we have read the proposal fix some units
  ## Convert proposal luminosity given in x10^-38/A/cm^2/s to /fb/hr
  for kin in range(1,8):
    GMnProposalInfo['Luminosity'][kin] = float(GMnProposalInfo['Luminosity'][kin])*360/2
  ## and now let's do some calculations of our own
  GMnProposalInfo['counts_neutron'] = [0,1,2,3,4,5,6,7]
  GMnProposalInfo['counts_proton'] = [0,1,2,3,4,5,6,7]
  for kin in range(1,8):
    GMnProposalInfo['counts_neutron'][kin] = computeExpectedCounts(
        float(GMnProposalInfo['SigmaNCoin'][kin]),kin)
    GMnProposalInfo['counts_proton'][kin] = computeExpectedCounts(
        float(GMnProposalInfo['SigmaPCoin'][kin]),kin)

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
