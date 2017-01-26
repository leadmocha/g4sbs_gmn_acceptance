import math;

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
