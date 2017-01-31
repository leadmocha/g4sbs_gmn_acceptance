## Read in additional information from the proposal
GMnProposalInfo = {}

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
