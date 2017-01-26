#include "shrinkGMn.h"

//const int Neutron = 0;
//const int Proton = 1;
//const int Electron = 2;

bool electron_in_bb;
bool neutron_in_hcal;
bool proton_in_hcal;
size_t electron_in_bb_index;
size_t neutron_in_hcal_index;
size_t proton_in_hcal_index;

// Global tree variables

// HCal Variables
std::vector<double> *hcal_x    = 0;
std::vector<double> *hcal_y    = 0;
std::vector<double> *hcal_z    = 0;
std::vector<double> *hcal_edep    = 0;
std::vector<double> *hcal_p    = 0;
std::vector<double> *hcal_px    = 0;
std::vector<double> *hcal_py    = 0;
std::vector<double> *hcal_pz    = 0;
std::vector<int> *hcal_mid  = 0;
std::vector<int> *hcal_pid  = 0;

// BBCal Variables
std::vector<double> *bbcal_x   = 0;
std::vector<double> *bbcal_y   = 0;
std::vector<double> *bbcal_z   = 0;
std::vector<double> *bbcal_edep    = 0;
std::vector<double> *bbcal_p    = 0;
std::vector<double> *bbcal_px   = 0;
std::vector<double> *bbcal_py   = 0;
std::vector<double> *bbcal_pz   = 0;
std::vector<int> *bbcal_mid = 0;
std::vector<int> *bbcal_pid = 0;



// Other variables
gen_t gen;
ev_t ev;
ev_gmn_elastic_check_t ev_gmn;


// New tree variables
ev_small_t ev_small;
particle_vars_t evars;
particle_vars_t  pvars;
particle_vars_t  nvars;

void ZeroOutVars(particle_vars_t &vars)
{
  vars.x = 0;
  vars.y = 0;
  vars.z = 0;
  vars.edep = 0;
  vars.p = 0;
  vars.px = 0;
  vars.py = 0;
  vars.pz = 0;
//  vars.mid = 0;
//  vars.pid = 0;
}

void isParticleInDetector(std::vector<int> *mid,
    std::vector<int> *pid, std::vector<double> *edep, int particle)

{
  if(!mid || !pid || mid->size() != pid->size()) {
    switch(particle) {
      case Electron:
        electron_in_bb = false;
        electron_in_bb_index = 0;
        break;
      case Proton:
        proton_in_hcal = false;
        proton_in_hcal_index = 0;
        break;
      case Neutron:
        neutron_in_hcal = false;
        neutron_in_hcal_index = 0;
        break;
    };
    return;
  }

  // Select PID
  int which_pid = -1;
  switch(particle) {
    case Electron:
      which_pid = 11; // e- PID
      break;
    case Proton:
      which_pid = 2212;
      break;
    case Neutron:
      which_pid = 2112;
      break;
    default:
      return;
  };

  size_t entries = pid->size();
  for(size_t i = 0; i < entries; i++) {
    if(mid->at(i) == 0 &&  pid->at(i) == which_pid) {
      switch(particle) {
        case Electron:
          electron_in_bb = true;
          electron_in_bb_index = i;
          break;
        case Proton:
          proton_in_hcal = true;
          proton_in_hcal_index = i;
          break;
        case Neutron:
          neutron_in_hcal = true;
          neutron_in_hcal_index = i;
          break;
      };
      return;
    }
  }
}


void shrinkGMn(const char *fileName, const char *out = "shrunk.root")
{
  TFile *fileIn = new TFile(fileName, "READ");
  TTree *treeIn = (TTree*)fileIn->Get("T");
  std::cout << treeIn << std::endl;


  // Disable all branches
  treeIn->SetBranchStatus("*",0);

  // Enable only the ones we are using
  treeIn->SetBranchStatus("ev",1);
  treeIn->SetBranchStatus("ev_gmn",1);
  treeIn->SetBranchStatus("gen",1);
  treeIn->SetBranchStatus("Earm.BBCal.x",1);
  treeIn->SetBranchStatus("Earm.BBCal.y",1);
  treeIn->SetBranchStatus("Earm.BBCal.z",1);
  treeIn->SetBranchStatus("Earm.BBCal.edep",1);
  treeIn->SetBranchStatus("Earm.BBCal.p",1);
  treeIn->SetBranchStatus("Earm.BBCal.px",1);
  treeIn->SetBranchStatus("Earm.BBCal.py",1);
  treeIn->SetBranchStatus("Earm.BBCal.pz",1);
  treeIn->SetBranchStatus("Earm.BBCal.mid",1);
  treeIn->SetBranchStatus("Earm.BBCal.pid",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.x",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.y",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.z",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.edep",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.p",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.px",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.py",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.pz",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.mid",1);
  treeIn->SetBranchStatus("Harm.HCAL_box.pid",1);

  treeIn->SetBranchAddress("ev",&ev.count);
  treeIn->SetBranchAddress("ev_gmn",&ev_gmn.nsigma);
  treeIn->SetBranchAddress("gen",&gen.thbb);
  treeIn->SetBranchAddress("Earm.BBCal.x",&bbcal_x);
  treeIn->SetBranchAddress("Earm.BBCal.y",&bbcal_y);
  treeIn->SetBranchAddress("Earm.BBCal.z",&bbcal_z);
  treeIn->SetBranchAddress("Earm.BBCal.edep",&bbcal_edep);
  treeIn->SetBranchAddress("Earm.BBCal.p",&bbcal_p);
  treeIn->SetBranchAddress("Earm.BBCal.px",&bbcal_px);
  treeIn->SetBranchAddress("Earm.BBCal.py",&bbcal_py);
  treeIn->SetBranchAddress("Earm.BBCal.pz",&bbcal_pz);
  treeIn->SetBranchAddress("Earm.BBCal.mid",&bbcal_mid);
  treeIn->SetBranchAddress("Earm.BBCal.pid",&bbcal_pid);
  treeIn->SetBranchAddress("Harm.HCAL_box.x",&hcal_x);
  treeIn->SetBranchAddress("Harm.HCAL_box.y",&hcal_y);
  treeIn->SetBranchAddress("Harm.HCAL_box.z",&hcal_z);
  treeIn->SetBranchAddress("Harm.HCAL_box.edep",&hcal_edep);
  treeIn->SetBranchAddress("Harm.HCAL_box.p",&hcal_p);
  treeIn->SetBranchAddress("Harm.HCAL_box.px",&hcal_px);
  treeIn->SetBranchAddress("Harm.HCAL_box.py",&hcal_py);
  treeIn->SetBranchAddress("Harm.HCAL_box.pz",&hcal_pz);
  treeIn->SetBranchAddress("Harm.HCAL_box.mid",&hcal_mid);
  treeIn->SetBranchAddress("Harm.HCAL_box.pid",&hcal_pid);

  TFile *fileOut = new TFile(out, "RECREATE");
  fileOut->cd();
  TTree *treeOut = new TTree("T","G4SBS GMn Acceptance Studies Tree");
  //treeOut->Branch("ev",&ev,"count/D:rate/D:solang/D:sigma/D:W2/D:xbj/D:Q2/D:th/D:ph/D:Aperp/D:Apar/D:Pt/D:Pl/D:vx/D:vy/D:vz/D:ep/D:np/D:epx/D:epy/D:epz/D:npx/D:npy/D:npz/D:nth/D:nph/D:pmperp/D:pmpar/D:pmparsm/D:z/D:phperp/D:phih/D:MX2/D:Sx/D:Sy/D:Sz/D:nucl/I:fnucl/I:hadr/I:earmaccept/I:harmaccept/I");
  //treeOut->Branch("ev_gmn",&ev_gmn,"nsigma/D:nAperp/D:nApar/D:psigma/D:pAperp/D:pApar/D");
  treeOut->Branch("ev",&ev_small,"nsigma/D:nAperp/D:nApar/D:psigma/D:pAperp/D:pApar/D:th_rad/D:ph_rad/D:theta/D:phi/D:ep/D:np/D:vz/D");
  treeOut->Branch("evars",&evars,"x/D:y/D:z/D:edep/D:p/D:px/D:py/D:pz/D:mid/I:pid/I");
  treeOut->Branch("pvars",&pvars,"x/D:y/D:z/D:edep/D:p/D:px/D:py/D:pz/D:mid/I:pid/I");
  treeOut->Branch("nvars",&nvars,"x/D:y/D:z/D:edep/D:p/D:px/D:py/D:pz/D:mid/I:pid/I");
  treeOut->Branch("e_det",&electron_in_bb);
  treeOut->Branch("p_det",&proton_in_hcal);
  treeOut->Branch("n_det",&neutron_in_hcal);


  bool goodEvent = false;
  int entries = treeIn->GetEntries();
  for(int i = 0; i < entries; i++) {
    // Reset flags
    goodEvent = false;
    electron_in_bb = neutron_in_hcal = proton_in_hcal = false;

    // Get next entry in input tree
    treeIn->GetEntry(i);

    // Fill out the event variables (small version)
    ev_small.nsigma   = ev_gmn.nsigma;
    ev_small.nAperp   = ev_gmn.nAperp;
    ev_small.nApar    = ev_gmn.nApar;
    ev_small.psigma   = ev_gmn.psigma;
    ev_small.pAperp   = ev_gmn.pAperp;
    ev_small.pApar    = ev_gmn.pApar;
    ev_small.th_rad   = ev.th;
    ev_small.ph_rad   = ev.ph;
    ev_small.theta    = ev.th*180./TMath::Pi();
    ev_small.phi      = ev.ph*180./TMath::Pi();
    ev_small.vz       = ev.vz;

    // Find out if primary particles made it to their respective detectors.
    isParticleInDetector(bbcal_mid,bbcal_pid,bbcal_edep,Electron);
    isParticleInDetector(hcal_mid,hcal_pid,hcal_edep,Proton);
    isParticleInDetector(hcal_mid,hcal_pid,hcal_edep,Neutron);


    // For each of the particles, simplify the variables for the new tree
    if(electron_in_bb) {
      goodEvent   = true;
      evars.x     = bbcal_x->at(electron_in_bb_index);
      evars.y     = bbcal_y->at(electron_in_bb_index);
      evars.z     = bbcal_z->at(electron_in_bb_index);
      evars.edep  = bbcal_edep->at(electron_in_bb_index);
      evars.p     = bbcal_p->at(electron_in_bb_index);
      evars.px    = bbcal_px->at(electron_in_bb_index);
      evars.py    = bbcal_py->at(electron_in_bb_index);
      evars.pz    = bbcal_pz->at(electron_in_bb_index);
//      evars.mid   = bbcal_mid->at(electron_in_bb_index);
//      evars.pid   = bbcal_pid->at(electron_in_bb_index);
    } else {
      ZeroOutVars(evars);
    }
    if(neutron_in_hcal) {
      goodEvent   = true;
      nvars.x     = hcal_x->at(neutron_in_hcal_index);
      nvars.y     = hcal_y->at(neutron_in_hcal_index);
      nvars.z     = hcal_z->at(neutron_in_hcal_index);
      nvars.edep  = hcal_edep->at(neutron_in_hcal_index);
      nvars.p     = hcal_p->at(neutron_in_hcal_index);
      nvars.px    = hcal_px->at(neutron_in_hcal_index);
      nvars.py    = hcal_py->at(neutron_in_hcal_index);
      nvars.pz    = hcal_pz->at(neutron_in_hcal_index);
//      nvars.mid   = hcal_mid->at(neutron_in_hcal_index);
//      nvars.pid   = hcal_pid->at(neutron_in_hcal_index);
    } else {
      ZeroOutVars(nvars);
    }
    if(proton_in_hcal) {
      goodEvent   = true;
      pvars.x     = hcal_x->at(proton_in_hcal_index);
      pvars.y     = hcal_y->at(proton_in_hcal_index);
      pvars.z     = hcal_z->at(proton_in_hcal_index);
      pvars.edep  = hcal_edep->at(proton_in_hcal_index);
      pvars.p     = hcal_p->at(proton_in_hcal_index);
      pvars.px    = hcal_px->at(proton_in_hcal_index);
      pvars.py    = hcal_py->at(proton_in_hcal_index);
      pvars.pz    = hcal_pz->at(proton_in_hcal_index);
//      pvars.mid   = hcal_mid->at(proton_in_hcal_index);
//      pvars.pid   = hcal_pid->at(proton_in_hcal_index);
    } else {
      ZeroOutVars(pvars);
    }

    if(goodEvent||true) {
      treeOut->Fill();
    }
  }

  // Save the configuration (Gen) info tree
  TTree *treeOut2 = new TTree("TGen","G4SBS GMn Acceptance Studies Gen Tree");
  treeOut2->Branch("gen",&gen,"thbb/D:thsbs/D:dbb/D:dsbs/D:dhcal/D:voffhcal/D:drich/D:dsbstrkr/D:Ebeam/D");
  treeIn->GetEntry(0);
  treeOut2->Fill();
  treeOut2->Write();

  // Save the magnetic field histograms
  TH2F *fields[4];
  TString fieldNames[4] = {"field","field_x","field_y","field_z"};
  for(int i = 0 ; i < 4; i++) {
    fields[i] = (TH2F*)fileIn->Get(fieldNames[i]);
    fields[i]->Write(fieldNames[i]);
  }


  // Save and close the out file
  fileOut->Write();
  fileOut->Close();
}
