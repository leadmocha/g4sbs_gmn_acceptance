#!/usr/bin/env python

from GMnDB import *
db = GMnDB()

import os ## Access os features (like modifying filesystem)
from jinja2 import Environment, FileSystemLoader
j2_env = Environment(loader=FileSystemLoader(os.path.dirname(os.path.abspath(__file__))))
template = j2_env.get_template('templates/fieldClampStudy.html')

def getKinValue(kin,name,res_name,result,mult=1.0):
  result[res_name] = kin.values[name]*mult
  result['unit_%s'%res_name] = kin.units[name]

def percentDiff(val1,val2,name,result):
  pdiff = 100*(val2/val1-1.0)
  result[name] = pdiff
  result['class_%s'%name] = 'effect_good' if pdiff > 0 else 'effect_bad'

## Main Function (program starts here)
def main():
  ## Load up the results database
  db.loadDB()

  noclamp = db.getConfigSet('october_v2',0)
  withclamp = db.getConfigSet('october',0)
  #print(noclamp)
  #print(withclamp)
  voffs = [15,45]
  hcal_sets = []
  for hcal_voff in voffs:
    hcal_set = {}
    hcal_set['voff'] = hcal_voff
    hcal_set['sigma_unit'] = 'nb'
    hcal_set['solid_unit'] = 'msr'
    hcal_set['voff_unit'] = 'cm'
    hcal_set['kins'] = []
    hcal_set['kins_unit'] = []
    hcal_set['Q2_unit'] = u'GeV\u00B2'
    for kin in range(1,8):
      kin_res = {}
      kin_res['kin'] = kin
      kin_res['Q2'] = noclamp[kin].Q2
      kin_res['Q2'] = noclamp[kin].Q2
      valsn = noclamp[kin].hcal_set[hcal_voff]
      valsw = withclamp[kin].hcal_set[hcal_voff]
      ## Integrated Cross Section Stuff
      #kin_res['psigman'] = noclamp[kin].hcal_set[hcal_voff].values['total_sigma_proton']
      getKinValue(valsn,'total_sigma_proton','psigman',kin_res)
      getKinValue(valsw,'total_sigma_proton','psigmaw',kin_res)
      percentDiff(kin_res['psigman'],kin_res['psigmaw'],'psigmadiff',kin_res);
      getKinValue(valsn,'total_sigma_neutron','nsigman',kin_res)
      getKinValue(valsw,'total_sigma_neutron','nsigmaw',kin_res)
      percentDiff(kin_res['nsigman'],kin_res['nsigmaw'],'nsigmadiff',kin_res);

      ## Solid angle stuff
      getKinValue(valsn,'total_solid_proton','psolidn',kin_res,1e3)
      getKinValue(valsw,'total_solid_proton','psolidw',kin_res,1e3)
      percentDiff(kin_res['psolidn'],kin_res['psolidw'],'psoliddiff',kin_res);
      getKinValue(valsn,'total_solid_neutron','nsolidn',kin_res,1e3)
      getKinValue(valsw,'total_solid_neutron','nsolidw',kin_res,1e3)
      percentDiff(kin_res['nsolidn'],kin_res['nsolidw'],'nsoliddiff',kin_res);

      ## Finally push results to list
      hcal_set['kins'].append(kin_res)
    hcal_sets.append(hcal_set);
  out = file('results/db/web/fieldClampStudy.html','w')
  out.write(template.render(hcal_sets=hcal_sets).encode('utf-8'))
  print 'Done'

  #db.printDebug()

## Run main() function if this file is run directly in the command line
## (as opposed to being imported)
if __name__ == '__main__':
  main()
