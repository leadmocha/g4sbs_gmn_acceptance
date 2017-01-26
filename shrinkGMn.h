#ifndef SHRINKGMN_H
#define SHRINKGMN_H

const int Neutron = 0;
const int Proton = 1;
const int Electron = 2;

// Data structures in g4sbs, copied here for lazines :)
typedef struct {
  Double_t thbb, thsbs, dbb, dsbs, dhcal,voffhcal, drich, dsbstrkr, Ebeam;
} gen_t;

// Data structure for new tree for the event being generated
typedef struct {
  // Generated neutron
  double nsigma;
  double nAperp;
  double nApar;
  // Generated proton
  double psigma;
  double pAperp;
  double pApar;
  // Generated angles (first two radians, next two degrees)
  double th_rad;
  double ph_rad;
  double theta;
  double phi;
  // Generated momenta for electron and hadron
  double ep;
  double np;
  // Generated position along target length
  double vz;
} ev_small_t;

// Data structure for new tree particle variables
typedef struct {
  double x;
  double y;
  double z;
  double edep;
  double p;
  double px;
  double py;
  double pz;
//  int mid;
//  int pid;
} particle_vars_t;

typedef struct {
  Double_t count, rate, solang, sigma, W2, xbj, Q2, th, ph;
  Double_t Aperp, Apar;
  Double_t Pt, Pl;
  Double_t vx, vy, vz;
  Double_t ep, np, nip;
  Double_t epx, epy, epz;
  Double_t npx, npy, npz;
  Double_t nth, nph;
  Double_t pmperp, pmpar, pmparsm;
  Double_t z, phperp, phih, MX;
  Double_t Sx, Sy, Sz; //polarization: only meaningful for gun generator!
  Int_t nucl, fnucl;
  Int_t hadr;
  Int_t earmaccept, harmaccept;
} ev_t;

typedef struct {
  // Generated neutron
  Double_t nsigma;
  Double_t nAperp;
  Double_t nApar;
  // Generated proton
  Double_t psigma;
  Double_t pAperp;
  Double_t pApar;
} ev_gmn_elastic_check_t;

#endif // SHRINKGMN_H
