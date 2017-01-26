#include <TFile.h>
#include <TRandom3.h>
#include <TTree.h>

const double kMomentumResolution = 0.011; // In percent
const double kAngularVerticalResolution = 1.2e-3*57.29578;  // in degrees
const double kAngularHorizontalResolution = 1.8e-3*57.29578;  // in degrees
const double kHCalPositionResolution = 0.035; // in m
const int kNumEntries = 100;

void generateResolution()
{
  TFile *file = new TFile("rootfiles/hcal_voffset/GMnRamdon.root","RECREATE");
  TTree *tree = new TTree("TRand","Random (Resolution) Numbers for GMn Acceptance Studies");

  GMnResolutionSmearing_t smear;

  tree->Branch("smear",&smear.nx,"nx/D:ny/D:px/D:py/D:theta/D:phi/D");

  double r;
  double a;
  double nx;
  double ny;
  double nx;
  double ny;
  TRandom3 rand(1234);
  double pi2 = 2*TMath::Pi();
  for(int entry = 0; entry < kNumEntries; entry++) {
    // Smear HCal position for proton/neutron separately
    r  = rand.Gaus(0.,kHCalPositionResolution);
    a = rand.Uniform(0.0,pi2);
    smear.nx = r*TMath::Cos(a);
    smear.ny = r*TMath::Sin(a);
    r  = r.Gaus(0.,kHCalPositionResolution);
    a = r.Uniform(0.0,pi2);
    smear.px = r*TMath::Cos(a);
    smear.py = r*TMath::Sin(a);
    // Smear angles
    //smear.phi = rand.Gaus(0.,
  }
}
