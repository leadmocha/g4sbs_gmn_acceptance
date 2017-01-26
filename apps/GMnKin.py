## Configuration modes as we parse the CSV file
kCONFIGMODE_NONE         = 0
kCONFIGMODE_GEN          = 1
kCONFIGMODE_HCAL_VOFFSET = 2

## A class of data for each hcal setting in a kin
class GMnHCalSetData:
  def __init__(self):
    self.values = {}
    self.units = {}
    self.exclusions = []

  ## Return string of details of this HCal setting
  def __repr__(self):
    result = 'voffset=%g %s' % (
        self.values['hcal_voffset'],self.units['hcal_voffset'])
    for ex in self.exclusions:
      result += ex.__repr__()
    return result;

## Data for each exclusion zone setting
class GMnExclusionData:
  def __init__(self,width,negIDSigmaP,negIDSigmaN,posIDSigmaP,posIDSigmaN,
      negIDSolP,negIDSolN,posIDSolP,posIDSolN):
    self.width        = float(width)
    self.negIDSigmaP  = float(negIDSigmaP)
    self.negIDSigmaN  = float(negIDSigmaN)
    self.posIDSigmaP  = float(posIDSigmaP)
    self.posIDSigmaN  = float(posIDSigmaN)
    self.negIDSolP    = float(negIDSolP)
    self.negIDSolN    = float(negIDSolN)
    self.posIDSolP    = float(posIDSolP)
    self.posIDSolN    = float(posIDSolN)
    self.golden = 0

  ## Set the status of golden (defalt is not golden)
  def setGolden(self,golden):
    self.golden = golden

  # Compare ExclusionData for sorting purposes
  def __cmp__(self,other):
    if self.golden != other.golden:
      #print("%f - %f = %f  (s.w=%f, o.w=%f)"%(other.golden,self.golden,
      #  other.golden-self.golden,self.width,other.width))
      return 10*(other.golden - self.golden)
    else:
      r = (self.width - other.width)/abs(self.width-other.width)
      #print"%f - %f = %f (r=%f)"%(self.width,other.width,self.width-other.width,
      #  r)
      return r

  ## Print debug info
  def __repr__(self):
    return '(width=%g,misP=%g,misN=%g,posP=%g,posN=%g,gold=%d)' % (self.width,
        self.negIDSigmaP,self.negIDSigmaN,
        self.posIDSigmaP,self.posIDSigmaN,self.golden)

## A class of data for each kin setting
class GMnKin:
  def __init__(self,kin,config,setID):
    self.kin = kin
    self.config = config ## I also call this dateCmd or date in some scripts
    self.setID = setID
    self.golden = 0
    self.comment = ''
    self.Q2 = 0
    self.gen_config = {}
    self.hcal_set = {}

  ## Print out details of this kin set
  def __repr__(self):
    result = 'kin=%02d,config=%s,set=%d,golden=%d,'%(self.kin,
        self.config, self.setID,self.golden)
    result += self.hcal_set.__repr__()
#    result += ',pn_vertical_sep=%g %s' % (self.values['pn_vertical_sep'],
#        self.units['pn_vertical_sep'])
#    for ex in self.exclusions:
#      result += ex.__repr__()
    return result

  ## Compare Kin sets for sorting purposes
  def __cmp__(self,other):
    ## Sort by kin, config, setID
    if self.kin == other.kin:
      if self.config == other.config:
        return self.setID - other.setID
      else:
        return 2*(self.config.lower() < other.config.lower())-1
    else:
      return self.kin - other.kin


  def readData(self,csv):
    f = open(csv,'r')
    hcal = GMnHCalSetData()
    found_gold = 0
    in_config = 0
    for line in f:
      ## Get rid of the new-line stuff at the end of each line
      line = line.rstrip();
      ## Check if it's the start of a new HCal config
      if line == 'start_hcal_voffset_config':
        ## Found new HCal configuration for this set
        hcal = GMnHCalSetData()
        found_gold = 0
        in_config = kCONFIGMODE_HCAL_VOFFSET
      elif line == 'end_hcal_voffset_config':
        ## Reached the end of this HCal config set, push results to list
        ## Sort exclusions
        hcal.exclusions.sort()
        self.hcal_set[hcal.values['hcal_voffset']] = hcal
      elif line == 'start_genconfig':
        in_config = kCONFIGMODE_GEN
      elif line == 'end_genconfig':
        in_config = kCONFIGMODE_NONE
      elif line.startswith("comment,"):
        ## Comment line for this set
        self.comment = line[8:]
      else: ## Remaining lines should be CSV
        items = line.strip().split(',')
        ## Skip empty lines
        if len(items) < 2:
          continue

        ## Check special valued lines
        if items[0] == 'golden':
          if (items[1].lower() == 'yes' or items[1].lower() == '1'):
            self.golden = 1
          else:
            self.golden = 0 ## The default, but doesn't hurt to set it again
        elif items[0] == 'exclusion_zone':
          ex = GMnExclusionData(items[1],
              items[2],items[3],items[4],items[5],
              items[6],items[7],items[8],items[9])
          # The last entry marks whether this is the golden "cut" or not
          if len(items)==11 and items[10] == 'yes':
            found_gold = 1
            ex.setGolden(1)
          hcal.exclusions.append(ex)
        #elif items[0] == 'comment':
        #  self.comment = items[1]
        elif items[0] == 'Q2':
          self.Q2 = float(items[1])
        elif in_config == kCONFIGMODE_GEN:
          self.gen_config[items[0]] = float(items[1])
        elif in_config == kCONFIGMODE_HCAL_VOFFSET:
          hcal.values[items[0]] = float(items[1])
          if len(items) > 2:
            hcal.units[items[0]] = items[2]