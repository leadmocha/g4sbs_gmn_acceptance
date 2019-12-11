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
      negIDSolP,negIDSolN,posIDSolP,posIDSolN,units):
    self.units        = units
    ## Do some conversions to cm, fm, and msr if not already there
    unit_width        = 1.0
    unit_sigma        = 1.0
    unit_solid        = 1.0
    ## So far, the only other unit I've put in was pb on the DB, so look
    ## for that and convert it to fm
    if units['sigma'] == 'pb':
      unit_sigma *= 1e3
    self.width        = float(width)*unit_width
    self.negIDSigmaP  = float(negIDSigmaP)*unit_sigma
    self.negIDSigmaN  = float(negIDSigmaN)*unit_sigma
    self.posIDSigmaP  = float(posIDSigmaP)*unit_sigma
    self.posIDSigmaN  = float(posIDSigmaN)*unit_sigma
    self.negIDSolP    = float(negIDSolP)*unit_solid
    self.negIDSolN    = float(negIDSolN)*unit_solid
    self.posIDSolP    = float(posIDSolP)*unit_solid
    self.posIDSolN    = float(posIDSolN)*unit_solid
    self.golden = 0

  ## Set the status of golden (defalt is not golden)
  def setGolden(self,golden):
    self.golden = golden

  # Compare ExclusionData for sorting purposes
  def __lt__(self,other):
    if self.golden != other.golden:
      #print("%f - %f = %f  (s.w=%f, o.w=%f)"%(other.golden,self.golden,
      #  other.golden-self.golden,self.width,other.width))
      return self.golden < other.golden
    else:
      return self.width < other.width
      #print("%f - %f = %f (r=%f)"%(self.width,other.width,self.width-other.width,
      #  r))

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
  def __lt__(self,other):
    ## Sort by kin, config, setID
    if self.kin == other.kin:
      if self.config == other.config:
        return self.setID < other.setID
      else:
        return self.config.lower() < other.config.lower()
    else:
      return self.kin < other.kin


  def readData(self,csv):
    f = open(csv,'r')
    hcal = GMnHCalSetData()
    found_gold = 0
    in_config = 0
    ## Specify the default exclusion zone units (unless changed by the config)
    ## This is just because previously I was lazy and didn't add units to
    ## the cross sections, but now I wish I had not mixed pb and fb
    exclusion_zone_units = { 'sigma':'pb', 'solid':'msr','width':'cm'}
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
        elif items[0] == 'exclusion_zone_unit':
          exclusion_zone_units[items[1]] = items[2]
        elif items[0] == 'exclusion_zone':
          ex = GMnExclusionData(items[1],
              items[2],items[3],items[4],items[5],
              items[6],items[7],items[8],items[9],exclusion_zone_units)
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
            ## Convert picobarn to femptobarn
            if items[2] == 'pb':
              hcal.values[items[0]] *= 1e3
              hcal.units[items[0]] = items[2]
            else:
              hcal.units[items[0]] = items[2]
