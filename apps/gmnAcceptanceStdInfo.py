class StdGMnInfo:
  ## Constructor
  def __init__(self):
    self.unit =  {}
    self.title = {}
    self.definition = {}
    self.maxBdl = 1e6 # To be set by user, default is ridiculous

  ## Add a new definition
  def add(self,var,title='',unit='',definition='',footnote=''):
    self.title[var]      = title
    self.unit[var]       = unit
    self.definition[var] = definition

## Define the units
info = StdGMnInfo()
info.unit['sigma'] = 'pb'
info.unit['thetapq_radius'] = 'cm'
info.unit['solid'] = 'msr'
info.unit['voff'] = 'cm'
#info.unit['Q2'] = u'GeV\u00B2'
info.unit['DMZ'] = 'cm'
info.unit['det_angle'] = '&deg;'
info.unit['beam_e'] = 'GeV'
info.unit['det_distance'] = 'm'

## Vertical Separation
info.add('bdl','&int;Bdl','Tm','')
info.add('Q2',u'Q\u00B2',u'GeV\u00B2','')
info.add('pn_vertical_sep','&lang;&Delta;Y<sub>P-N</sub>&rang;','cm',
    'Average vertical separation of protons to neutrons.')
info.add('thetapq','&theta;<sub>pq</sub> Cut','&deg;','')
info.add('thetapq_radius','&theta;<sub>pq</sub> Cut Radius','cm','')
    #'For any given (&theta;<sub>e</sub>,&phi;<sub>e</sub>) a cut position cut '
    #' is made such that a maximum particle 
    #' a maximum displacement 
    #' a radius (&theta;<sub>pq</sub>&cdot;d_<sub>hcal</sub>) is defined as
    #'&theta;<sub>pq</sub> is the angle between direction of the nucleon\'s momentum (<b><i>p</i></b>), reconstructed from the position of the hit on HCal, and the momentum-transfer vector (
info.add('dmz','DMZ','cm','Starting from the lowest (highest) hcal position where the Proton (Neutron) is positively identified, the DMZ represents a line above (below) this point where no particle is identified.')
info.add('posID','Positive-ID','','A particle is positively identified if it satisfies the &theta;<sub>pq</sub> cut.')
info.add('negID','Negative-ID','','A particle is negatively identified (misidentified) if it satisfies the &theta;<sub>pq</sub> cut of the <b>other</b> hadron.')
info.add('sigma','Cross Section &int;&sigma;d&Omega;','pb','The corresponding cross section of the hadron integrated over the acceptance and weighted by the efficiency.')
info.add('IDPerTotal','%-of no cut Sigma','','The ratio of the Cross section of the Proton (Neutron) scaled by the total cross section of the Neutron (Proton) with no &theta;<sub>pq</sub> cut.')
info.add('IDPerOtherPosID','%-of Other &sigma;','','The ratio of the cross section of the Proton (Neutron) scaled by the cross section of the Neutron (Proton) that satisfied the &theta;<sub>pq</sub> cut.')
info.add('IDPerFirst','%-Diff of First Row','','Percent Difference of this value compared to first row in the given table.')
#info.add('ratio_posID','&sigma;<sub>n</sub>/&sigma;<sub>p</sub>','','Percent difference of the ')
info.add('ratio_posID','R<sub></sub>','','')
info.add('ratio_measured','R<sub>meas</sub>','','')
info.add('pdiff_ratio_overestimate','%-Diff','','')
info.add('ratio_eff_posID','R<sub></sub>','','')
info.add('ratio_eff_measured','R<sub>meas</sub>','','')
info.add('pdiff_ratio_eff_overestimate','%-Diff','','')
info.add('Luminosity','Luminosity',u'x10\u00B3\u2078/cm\u00B2s','','Taken from the proposal')
info.add('RunningTime','Running Time','hr','','Taken from the proposal')
info.add('InelPContam','Inelastic Proton Contamination (bkg/total)','%','','Taken from the proposal')
info.add('InelNContam','Inelastic Neutron Contamination (bkg/total)','%','','Taken from the proposal')
info.add('scat_e_p','P<sub>e\'</sub>','GeV/c','','')
info.add('scat_h_p','P<sub>h\'</sub>','GeV/c','','')
info.add('stat_error','Estimated Statistical Error','',u'Statistical errors for &sigma;<sub>n,p</sub> are estimated by &delta;&sigma;<sub>h</sub>\u22481/\u221AN<sub>h</sub>, where N<sub>h</sub> = &sigma;<sub>h</sub>&int;Ldt, L is the Luminosity and t is the running time in seconds. The statistical error for R is then taken as &delta;R = \u221A( &delta;&sigma;<sub>n</sub>\u00B2 +  &delta;&sigma;<sub>p</sub>\u00B2 ).','')
