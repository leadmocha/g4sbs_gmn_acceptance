#include "plotAcceptance.h"
const bool kApplyMomentumCut = false;
const double kMinToleratedELoss = -1.0;
const bool kQuickPlot = false;
const bool kFillMapGaps = false;
const bool kSaveCanvas = true;
const int kVerbosity = kWarning; // Disable all Info prints


const int kFillMapSize = 4;
const double kMinStatAngles[8]      = {0, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05, 0.05 };
const double kMinStatMomentumMap[8] = {0, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00, 0.00 };

// Test Cuts
const bool kApplyTestMomentaCut = false;
const double kMinTestMomentaCut = 2.74;
const double kMaxTestMomentaCut = 2.76;
const bool kApplyTestPhiCut = false;
const double kMinTestPhiCut = -0.1;
const double kMaxTestPhiCut =  0.1;

double kMinBBEDep = -0.3; // GeV/c
double kMinHCalEDep = -0.1; // GeV
int kHArm = 0;
int kEArm = 1;
int kBothArms = 2;
const char* kHadronName[2] = { "Neutron", "Proton" };
const char* kHist2DDrawOption = "COLZ"; // "CONTZ";
const char* kHist1DDrawOption = "";
const char* kHistMapDrawOption = "box";
//#define FILL_NORM ev.count
#define FILL_NORM 1.0

int kCanvasX = 300;
int kCanvasY = 300;


// These two are the real values taken from Gregg's CAD sheets
//double kHCalHeight = 3.656; // m
//double kHCalWidth  = 1.366; // m
// These are the values presently in the simulation
double kHCalHeight = .153*24*1.001; // m
double kHCalWidth  = .153*12*1.001; // m
double kBBHeight = 27.*8.5/100.;  // m (taken from g4sbs)
double kBBWidth  = 2.0*37.0/100.; // m (taken from g4sbs)

double kMinPlotVPosDiff =  0.0;
double kMaxPlotVPosDiff =  1.5;
double kMinPlotHPosDiff = -1.0;
double kMaxPlotHPosDiff =  1.5;

                            // 0,    1,    2,    3,    4,    5,    6,   7
double kMinPhi[8]           = {0,  -30, - 35,  -55,  -55,  -55,  -55,  -55 };
double kMaxPhi[8]           = {0,   30,   35,   55,   55,   55,   55,   55 };
double kMinTheta[8]         = {0,   20,   20,   40,   30,   20,   20,   20 };
double kMaxTheta[8]         = {0,   47,   65,   90,   65,   50,   70,   50 };
double kMinPlotPElectron[8] = {0,    0,    0,  0.5,    0,    1,    0,  1.5 };
double kMaxPlotPElectron[8] = {4,    4,    4,  2.0,    4,    5,    4,  6.0 };
double kMinPlotPHadron[8]   = {0,  1.8,  1.8,  2.5,  3.5,  3.5,  4.5,    5 };
double kMaxPlotPHadron[8]   = {4,  3.2,  4.3,  5.5,  7.5,  8.5,  9.5,   10 };
double kMinPlotBBEDep[8]    = {0,   0.,   0.,   0.,   0.,   0.,   0.,   0. };
double kMaxPlotBBEDep[8]    = {0,   4.,   4.,   2.,   4.,   4.,   4.,   5. };
double kMinPlotHCalEDep[8]  = {0,   0.,   0.,   2.,   3.,  3.5,   4.,   4. };
double kMaxPlotHCalEDep[8]  = {0,   4.,   4.,   4.,   6.,  7.0,  8.5,  10. };

double kPlotPositionPadding = 1.15;
double kMinBBX  = -kPlotPositionPadding*kBBHeight/2.;
double kMaxBBX  =  kPlotPositionPadding*kBBHeight/2.;
double kMinBBY  = -kPlotPositionPadding*kBBHeight/2.;
double kMaxBBY  =  kPlotPositionPadding*kBBHeight/2.;

double kMinHCalX  = -kPlotPositionPadding*kHCalHeight/2.;
double kMaxHCalX  =  kPlotPositionPadding*kHCalHeight/2.;
double kMinHCalY  = -kPlotPositionPadding*kHCalHeight/2.;
double kMaxHCalY  =  kPlotPositionPadding*kHCalHeight/2.;

int gKin;
TString gVoff;
double gVoffValue;
double gAcceptedNeutrons = 0;
double gAcceptedProtons = 0;
double gHCalAvgVPositionDiff = 0;
double gHCalAvgHPositionDiff = 0;
double gHCalAvgVPositionDiffE = 0;
double gHCalAvgHPositionDiffE = 0;

bool gMomentumAcceptanceMapBuilt = false;
bool passed_momentum_cut = false;
int kBins = 100;
TChain *chain;
size_t chain_entries;

TH2F *hHArmEDep[2];
TH2F *hEArmEDep[2];

TH2F *hHArmAngles[2];
TH2F *hEArmAngles[2];
TH2F *hBothArmsAngles[2];
TH2F *hHArmAnglesMap;
TH2F *hEArmAnglesMap;
TH2F *hBothArmsAnglesMap;
TH2F *hHArmPositions[2];
TH2F *hEArmPositions[2];
TH2F *hHArmCoincidencePositions[2];
TH2F *hEArmCoincidencePositions[2];
TH2F *hHArmMomenta[2];
TH2F *hEArmMomenta[2];
TH1F *hHArmMomentaRatio[2];
TH1F *hEArmMomentaRatio[2];
TH2F *hBothArmsMomenta[2];
TH2F *hAngleMomenta[2];
TH2F *hAngleMomentaMap;
TH1F *hAngleMomentaMapCounts;

TH2F *hHAngleVPositions[2];
TH2F *hHAngleVPositionsCounts[2];
TH2F *hHAngleVPositionDiffs;
TH1F *hHAngleVPositionDiffAvg;
TH2F *hHAngleHPositions[2];
TH2F *hHAngleHPositionsCounts[2];
TH2F *hHAngleHPositionDiffs;
TH1F *hHAngleHPositionDiffAvg;

const int Neutron = 0;
const int Proton = 1;
const int Electron = 2;

// Global tree variables
// HCal Variables
double hcal_npart_CAL;
std::vector<double> *hcal_x    = 0;
std::vector<double> *hcal_y    = 0;
std::vector<double> *hcal_z    = 0;
std::vector<double> *hcal_edep    = 0;
std::vector<double> *hcal_p    = 0;
std::vector<int> *hcal_mid  = 0;
std::vector<int> *hcal_pid  = 0;
// BBCal Variables
double bbcal_npart_CAL;
std::vector<double> *bbcal_x   = 0;
std::vector<double> *bbcal_y   = 0;
std::vector<double> *bbcal_z   = 0;
std::vector<double> *bbcal_edep    = 0;
std::vector<double> *bbcal_p    = 0;
std::vector<int> *bbcal_mid = 0;
std::vector<int> *bbcal_pid = 0;

// Data structures in g4sbs, copied here for lazines :)
typedef struct {
  Double_t thbb, thsbs, dbb, dsbs, dhcal,voffhcal, drich, dsbstrkr, Ebeam;
} gen_t;

typedef struct {
  Double_t count, rate, solang, sigma, W2, xbj, Q2, th, ph;
  Double_t Aperp, Apar;
  Double_t Pt, Pl;
  Double_t vx, vy, vz;
  Double_t ep, np;
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

// Other variables
gen_t gen;
ev_t ev;

bool electron_in_bb;
bool hadron_in_hcal;
size_t electron_in_bb_index;
size_t hadron_in_hcal_index;

// Simplified variables
double theta;
double phi;
double electron_momentum;
double hadron_momentum;
bool passed_electron_eloss_cut;
bool passed_hadron_eloss_cut;

bool passed_quality_check;
bool passed_test_cuts;

void fixStatBox(TH1F* hist, TVirtualPad *pad = 0)
{
  if(!hist) {
    std::cerr << "What?!?! No histogram!" << std::endl;
    return;
  }

  if(!pad) {
    // Use the current pad then
    pad = gPad;
  }

  pad->Update();
  TPaveStats *stats = (TPaveStats*)hist->GetListOfFunctions()->FindObject("stats");

  if(!stats) {
    std::cerr << "What?!?! No stats!" << std::endl;
    return;
  }

  stats->SetY1NDC(0.75);
  stats->SetY2NDC(0.925);
  stats->SetX1NDC(0.60);
  stats->SetX2NDC(0.85);
  stats->Draw();
  pad->Modified();
  pad->Update();
}

void saveCanvas(TCanvas *canvas, const char *comment, const char *ext = "png")
{
  if(!kSaveCanvas)
    return;

  int level = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kVerbosity;
  canvas->SaveAs(Form("results/hcal_voffset/gmn_kin%02d_voff%s_%s.%s",
        gKin,gVoff.Data(),comment,ext));
  gErrorIgnoreLevel = level;
}

TCanvas* makeCanvas(const char *name, int cols, int rows, double scale = 1.0)
{
  TCanvas *canvas = new TCanvas(name,name,cols*kCanvasX*scale,rows*kCanvasY*scale);
  canvas->Divide(cols,rows);
  return canvas;
}

void buildMap(TH2F **hists, TH2F* map, double min_stat, TH1F *h_counts = 0)
{
  int nbx = hists[0]->GetNbinsX();
  int nby = hists[0]->GetNbinsY();
  if(nbx != hists[1]->GetNbinsX() || nby != hists[1]->GetNbinsY()) {
    std::cout << "Yikes!!!! Not the same number of bins!!" << std::endl;
    return;
  }
  double result = 0;
  double v0,v1;
  double e0,e1;
  double diff,err;

  double max0 = -1;
  double max1 = -1;
  double min0 = -1;
  double min1 = -1;
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      v0 = hists[0]->GetBinContent(bx,by);
      v1 = hists[1]->GetBinContent(bx,by);
      if(v0 > max0)
        max0 = v0;
      if(v1 > max1)
        max1 = v1;
    }
  }
  min0 = min_stat*max0;
  min1 = min_stat*max1;
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      if( hists[0]->GetXaxis()->GetBinLowEdge(bx) !=
          hists[1]->GetXaxis()->GetBinLowEdge(bx) ) {
        std::cout << "Yikes!!! Not the same low edge x-bin: " << bx << std::endl;
        return;
      }
      if( hists[0]->GetYaxis()->GetBinLowEdge(by) !=
          hists[1]->GetYaxis()->GetBinLowEdge(by) ) {
        std::cout << "Yikes!!! Not the same low edge y-bin: " << by << std::endl;
        return;
      }
      result = 0.0;
      v0 = hists[0]->GetBinContent(bx,by);
      v1 = hists[1]->GetBinContent(bx,by);
      e0 = hists[0]->GetBinError(bx,by);
      e1 = hists[1]->GetBinError(bx,by);

      // Only work with bins that are non-zero
      if(v0>0 && v1 >0) {
        // Compute the difference of contents and associated error
        /*
        diff = v0-v1;
        err = TMath::Sqrt(e0*e0 + e1*e1);
        if( TMath::Abs(v0-v1) <= 2*err )
          result = 1;
        */
        if(v0 >= min0 && v1 >= min1)
          result = 1.;

        // Fill the counts histogram, if available
        if(h_counts)
          h_counts->Fill(v0);
      }
      map->SetBinContent(bx,by,result);
    }
  }

  //hists[0]->SetMinimum(min0);
  //hists[1]->SetMinimum(min1);

  // Fix gaps which are likely just do to statistics. Allow values
  // if they are surrounded by a valid region all around
  bool modified = true;
  while(modified&&kFillMapGaps) {
    modified = false;
    for(int bx = 2; bx < nbx; bx++) {
      for(int by = 2; by < nby; by++) {
        if((map->GetBinContent(bx,by+1) +  // bin above
              map->GetBinContent(bx,by-1) +  // bin below
              map->GetBinContent(bx-1,by) +  // bin to left
              map->GetBinContent(bx+1,by) >= kFillMapSize) &&
            (map->GetBinContent(bx,by)==0)) { // bin to right
          map->SetBinContent(bx,by,1.0);
          modified = true;
        }
      }
    }
  }
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
      case Neutron:
        hadron_in_hcal = false;
        hadron_in_hcal_index = 0;
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
        case Neutron:
          if(edep->at(i)>kMinHCalEDep) {
            hadron_in_hcal = true;
            hadron_in_hcal_index = i;
          } else {
            hadron_in_hcal = false;
            hadron_in_hcal_index = -1;
          }
          break;
      };
      return;
    }
  }
}

bool checkAcceptanceMap(double x, double y, TH2F *map)
{
  // Determine the global bin that corresponds to this x,y
  int bin = map->FindFixBin(x,y);

  // Determine total number of bins in map
  int nbins = map->GetNbinsX()*map->GetNbinsY();

  // Is this the underflow or overflow bin?
  if(bin == 0 || bin > nbins+1) {
    return false;
  }

  // Now check if this x,y fall within the accepted map
  return map->GetBinContent(bin)>0;

}

bool checkMomentumAcceptance()
{
  return checkAcceptanceMap(theta,ev.np,
        hAngleMomentaMap);
}

void getEntry(size_t entry)
{
  // Reset flags
  electron_in_bb = hadron_in_hcal = false;

  // Get next entry
  chain->GetEntry(entry);

  // Find out if primary particles made it to their respective detectors.
  isParticleInDetector(bbcal_mid,bbcal_pid,bbcal_edep,Electron);
  isParticleInDetector(hcal_mid,hcal_pid,hcal_edep,ev.nucl);

  // Compute proper coordinates
  theta = ev.th*180./TMath::Pi();
  phi = ev.ph*180./TMath::Pi();
  passed_electron_eloss_cut = true;
  if(electron_in_bb) {
    // Initial particle energy
    electron_momentum = bbcal_p->at(electron_in_bb_index);
    if(electron_momentum/ev.ep < kMinToleratedELoss) {
      passed_electron_eloss_cut = false;
      electron_in_bb = false;
    }
  }

  passed_hadron_eloss_cut = true;
  if(hadron_in_hcal) {
    // Initial particle energy
    hadron_momentum = hcal_p->at(hadron_in_hcal_index);
    if(hadron_momentum/ev.np < kMinToleratedELoss) {
      passed_hadron_eloss_cut = false;
      hadron_in_hcal = false;
    }
  }

  passed_quality_check = true;

  passed_momentum_cut = true;
  if(!passed_hadron_eloss_cut || !passed_electron_eloss_cut) {
    passed_momentum_cut = false;
  }

  if(kApplyMomentumCut && gMomentumAcceptanceMapBuilt && hadron_in_hcal) {
    if(!checkMomentumAcceptance()) {
      passed_momentum_cut = false;
    }
  }

  // The test momenta cut is only used to select out certain momenta to see
  // positions angles etc..
  passed_test_cuts = true;
  // If applicable, did it fail the test momenta cut?
  if(kApplyTestMomentaCut &&
      (ev.np < kMinTestMomentaCut || ev.np > kMaxTestMomentaCut) )
    passed_test_cuts = false;
  // If applicable, did it fail the test phi cut?
  if(kApplyTestPhiCut &&
      (phi < kMinTestPhiCut || phi > kMaxTestPhiCut) )
    passed_test_cuts = false;
}

void fillHistograms()
{
  int trig = -1;
  int be,bh,bb = -1;
  for( size_t entry = 0; entry < chain_entries; entry++ ) {
    be = bh = bb = -1;
    getEntry(entry);
    trig = -1;
    if(theta >= 3.05300000000000011e+01 && theta < 3.08000000000000007e+01
       &&  phi >= -1.86000000000000014e+01 && phi < -1.80000000000000000e+01) {
      if(electron_in_bb) {
        if(hadron_in_hcal) {
          trig = 3;
        } else {
          trig = 1;
        }
      } else if (hadron_in_hcal) {
        trig = 2;
      } else {
        trig = 0;
      }
      if(trig > 5) {
        std::cout << "Ent: " << std::setfill(' ') << std::setw(8) << entry;
        std::cout << "\tTrig: " << trig;
        std::cout << "\tQual: " << passed_quality_check;
        //std::cout << "\tNucl: " << (ev.nucl == 0 ? "N" : "P");
        std::cout << "\tNucl: " << ev.nucl;
        std::cout << "\tPe: " << ev.ep;
        std::cout << "\tPn: " << ev.np;
        //std::cout << "\tTh: " << theta;
        //std::cout << "\tPh: " << phi;
        std::cout << std::endl;
      }
    } else {
      //continue;
    }
    if(!passed_quality_check)
      continue;

    if(kApplyTestMomentaCut && !passed_test_cuts)
      continue;

    ///////////////////////////////////////////////////////////////////////////
    // Fill the position histograms
    if(electron_in_bb) {
      hEArmPositions[ev.nucl]->Fill(bbcal_x->at(electron_in_bb_index),
          bbcal_y->at(electron_in_bb_index),FILL_NORM);
    }

    if(hadron_in_hcal) {
      hHArmPositions[ev.nucl]->Fill(hcal_x->at(hadron_in_hcal_index),
          hcal_y->at(hadron_in_hcal_index),FILL_NORM);
    }

    if(electron_in_bb&&hadron_in_hcal) {
      hEArmCoincidencePositions[ev.nucl]->Fill(
          bbcal_x->at(electron_in_bb_index),bbcal_y->at(electron_in_bb_index),
          FILL_NORM);
      hHArmCoincidencePositions[ev.nucl]->Fill(
          hcal_x->at(hadron_in_hcal_index),hcal_y->at(hadron_in_hcal_index),
          FILL_NORM);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Fill the angles and momenta histograms
    if(passed_momentum_cut) {
      if(electron_in_bb) {
        be = hEArmAngles[ev.nucl]->Fill(theta,phi,FILL_NORM);
        hEArmMomenta[ev.nucl]->Fill(electron_momentum,ev.ep,FILL_NORM);
        hEArmMomentaRatio[ev.nucl]->Fill(electron_momentum/ev.ep,FILL_NORM);
        hEArmEDep[ev.nucl]->Fill(theta,bbcal_edep->at(electron_in_bb_index),FILL_NORM);
      }
      if(hadron_in_hcal) {
        bh = hHArmAngles[ev.nucl]->Fill(theta,phi,FILL_NORM);
        hHArmMomenta[ev.nucl]->Fill(hadron_momentum,ev.np,FILL_NORM);
        hHArmMomentaRatio[ev.nucl]->Fill(hadron_momentum/ev.np,FILL_NORM);
        hHArmEDep[ev.nucl]->Fill(theta,hcal_edep->at(hadron_in_hcal_index),FILL_NORM);
      }
      if(electron_in_bb&&hadron_in_hcal) {
        bb = hBothArmsAngles[ev.nucl]->Fill(theta,phi,FILL_NORM);
        //hBothArmsMomenta[ev.nucl]->Fill(electron_momentum,hadron_momentum,FILL_NORM);
        hBothArmsMomenta[ev.nucl]->Fill(ev.ep,ev.np,FILL_NORM);
        hHAngleVPositions[ev.nucl]->Fill(theta,phi,
            hcal_y->at(hadron_in_hcal_index));
        hHAngleVPositionsCounts[ev.nucl]->Fill(theta,phi,1.0);
        hHAngleHPositions[ev.nucl]->Fill(theta,phi,
            hcal_x->at(hadron_in_hcal_index));
        hHAngleHPositionsCounts[ev.nucl]->Fill(theta,phi,1.0);
        if(ev.nucl==0) {
          gAcceptedNeutrons++;
        } else if (ev.nucl == 1) {
          gAcceptedProtons++;
        }
      }
    }
  }

  // Create the maps
  buildMap(hHArmAngles,hHArmAnglesMap,kMinStatAngles[gKin]);
  buildMap(hEArmAngles,hEArmAnglesMap,kMinStatAngles[gKin]);
  buildMap(hBothArmsAngles,hBothArmsAnglesMap,kMinStatAngles[gKin]);
  //hHArmAnglesMap->Add(hHArmAngles[0],hHArmAngles[1],1.0,-1.0);
  //hEArmAnglesMap->Add(hEArmAngles[0],hEArmAngles[1],1.0,-1.0);
  //hBothArmsAnglesMap->Add(hBothArmsAngles[0],hBothArmsAngles[1],1.0,-1.0);

  // Compute the vertical position differences
  double c0,c1;
  double pv0,pv1;
  double ph0,ph1;
  double diffV;
  double diffH;
  for(int bx = 1; bx <= kBins; bx++) {
    for(int by = 1; by <= kBins; by++) {
      c0 = hHAngleVPositionsCounts[0]->GetBinContent(bx,by);
      c1 = hHAngleVPositionsCounts[1]->GetBinContent(bx,by);
      pv0 = hHAngleVPositions[0]->GetBinContent(bx,by);
      pv1 = hHAngleVPositions[1]->GetBinContent(bx,by);
      ph0 = hHAngleHPositions[0]->GetBinContent(bx,by);
      ph1 = hHAngleHPositions[1]->GetBinContent(bx,by);
      if(c0 > 0 && c1 >0 && hBothArmsAnglesMap->GetBinContent(bx,by) > 0 ) {
        diffV = (pv1/c1)-(pv0/c0);
        diffH = (ph1/c1)-(ph0/c0);
        hHAngleVPositionDiffs->SetBinContent(bx,by,diffV);
        hHAngleHPositionDiffs->SetBinContent(bx,by,diffH);
        hHAngleVPositionDiffAvg->Fill(diffV);
        hHAngleHPositionDiffAvg->Fill(diffH);
      }
    }
  }
  gHCalAvgVPositionDiff  = hHAngleVPositionDiffAvg->GetMean();
  gHCalAvgVPositionDiffE = hHAngleVPositionDiffAvg->GetRMS()/TMath::Sqrt(
      hHAngleVPositionDiffAvg->GetEntries());
  gHCalAvgHPositionDiff  = hHAngleHPositionDiffAvg->GetMean();
  gHCalAvgHPositionDiffE = hHAngleHPositionDiffAvg->GetRMS()/TMath::Sqrt(
      hHAngleHPositionDiffAvg->GetEntries());
}


void drawAngles(TCanvas *canvas)
{
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmAngles[0]->Draw(kHist2DDrawOption);
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmAngles[1]->Draw(kHist2DDrawOption);
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmAnglesMap->Draw(kHistMapDrawOption);
  canvas->cd(4);
  gPad->SetGrid(kTRUE,kTRUE);
  hEArmAngles[0]->Draw(kHist2DDrawOption);
  canvas->cd(5);
  gPad->SetGrid(kTRUE,kTRUE);
  hEArmAngles[1]->Draw(kHist2DDrawOption);
  canvas->cd(6);
  gPad->SetGrid(kTRUE,kTRUE);
  hEArmAnglesMap->Draw(kHistMapDrawOption);
  canvas->cd(7);
  gPad->SetGrid(kTRUE,kTRUE);
  hBothArmsAngles[0]->Draw(kHist2DDrawOption);
  canvas->cd(8);
  gPad->SetGrid(kTRUE,kTRUE);
  hBothArmsAngles[1]->Draw(kHist2DDrawOption);
  canvas->cd(9);
  gPad->SetGrid(kTRUE,kTRUE);
  hBothArmsAnglesMap->Draw(kHistMapDrawOption);
}

void buildMomentumAcceptance(TCanvas *canvas)
{
  for( size_t entry = 0; entry < chain_entries; entry++ ) {
    getEntry(entry);
    if(!passed_quality_check)
      continue;

    // Start filling in histograms
    if(hadron_in_hcal && passed_hadron_eloss_cut) {
      //hAngleMomenta[ev.nucl]->Fill(hadron_momentum,theta,FILL_NORM);
      hAngleMomenta[ev.nucl]->Fill(theta,ev.np,FILL_NORM);
      //hAngleMomenta[ev.nucl]->Fill(ev.Q2,theta,FILL_NORM);
    }
  }
  buildMap(hAngleMomenta,hAngleMomentaMap,kMinStatMomentumMap[gKin],
      hAngleMomentaMapCounts);
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hAngleMomenta[0]->Draw(kHist2DDrawOption);
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hAngleMomenta[1]->Draw(kHist2DDrawOption);
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  hAngleMomentaMap->Draw(kHistMapDrawOption);
  hAngleMomentaMap->Draw(kHist2DDrawOption);
  //hAngleMomentaMapCounts->Draw();
  gMomentumAcceptanceMapBuilt = true;
}

void drawMomenta(TCanvas *canvas)
{
  canvas->cd(1);
  hHArmMomenta[0]->Draw(kHist2DDrawOption);
  //gPad->SetLogy(kTRUE);
  //hHArmMomentaRatio[0]->Draw(kHist1DDrawOption);
  canvas->cd(2);
  hHArmMomenta[1]->Draw(kHist2DDrawOption);
  //gPad->SetLogy(kTRUE);
  //hHArmMomentaRatio[1]->Draw(kHist1DDrawOption);
  canvas->cd(3);
  hEArmMomenta[0]->Draw(kHist2DDrawOption);
  //gPad->SetLogy(kTRUE);
  //hEArmMomentaRatio[0]->Draw(kHist1DDrawOption);
  canvas->cd(4);
  hEArmMomenta[1]->Draw(kHist2DDrawOption);
  //gPad->SetLogy(kTRUE);
  //hEArmMomentaRatio[1]->Draw(kHist1DDrawOption);
  canvas->cd(5);
  hBothArmsMomenta[0]->Draw(kHist2DDrawOption);
  canvas->cd(6);
  hBothArmsMomenta[1]->Draw(kHist2DDrawOption);
}

void drawHCalBox()
{
  TBox *box = new TBox(-kHCalWidth/2.,-kHCalHeight/2.,
      kHCalWidth/2.,kHCalHeight/2.);
  box->SetFillStyle(0);
  box->SetLineColor(kRed);
  box->SetLineWidth(2);
  box->Draw();
}

void drawBBBox()
{
  TBox *box = new TBox(-kBBWidth/2.,-kBBHeight/2.,kBBWidth/2.,kBBHeight/2.);
  box->SetFillStyle(0);
  box->SetLineColor(kBlue);
  box->SetLineWidth(2);
  box->Draw();
}


void drawPositions(TCanvas *canvas, bool coincidence = false)
{
  if(coincidence) {
    canvas->cd(1);
    gPad->SetGrid(kTRUE,kTRUE);
    hHArmCoincidencePositions[0]->Draw(kHist2DDrawOption);
    drawHCalBox();
    canvas->cd(2);
    gPad->SetGrid(kTRUE,kTRUE);
    hHArmCoincidencePositions[1]->Draw(kHist2DDrawOption);
    drawHCalBox();
    canvas->cd(3);
    gPad->SetGrid(kTRUE,kTRUE);
    hEArmCoincidencePositions[0]->Draw(kHist2DDrawOption);
    drawBBBox();
    canvas->cd(4);
    gPad->SetGrid(kTRUE,kTRUE);
    hEArmCoincidencePositions[1]->Draw(kHist2DDrawOption);
    drawBBBox();
  } else {
    canvas->cd(1);
    gPad->SetGrid(kTRUE,kTRUE);
    hHArmPositions[0]->Draw(kHist2DDrawOption);
    drawHCalBox();
    canvas->cd(2);
    gPad->SetGrid(kTRUE,kTRUE);
    hHArmPositions[1]->Draw(kHist2DDrawOption);
    drawHCalBox();
    canvas->cd(3);
    gPad->SetGrid(kTRUE,kTRUE);
    hEArmPositions[0]->Draw(kHist2DDrawOption);
    drawBBBox();
    canvas->cd(4);
    gPad->SetGrid(kTRUE,kTRUE);
    hEArmPositions[1]->Draw(kHist2DDrawOption);
    drawBBBox();
  }
}


void  plotAcceptance(int kin = 1, TString voff = "0")
{
  // Load personal style sheet
  gROOT->Macro("$JC2ROOTSTYLE");
  gStyle->SetPadRightMargin(.15);

  // Set some of the global (non constant) variables
  gKin = kin;
  gVoff = voff;
  gVoffValue = voff.Atof()/100.;

  if(chain)
    delete chain;

  chain = new TChain("T");
  std::cout << "Found "
    << chain->Add(Form("rootfiles/hcal_voffset/gmn_kin%02d_r*_v%s.root",kin,voff.Data()));
  chain_entries = chain->GetEntries();
  if(kQuickPlot)
    chain_entries *= .1;
  std::cout << " rootfiles with total entries: " << chain_entries << std::endl;

  // Disable all branches
  chain->SetBranchStatus("*",0);

  // Enable only the ones we are using
  chain->SetBranchStatus("ev",1);
  chain->SetBranchStatus("gen",1);
  chain->SetBranchStatus("Earm.BBCal.x",1);
  chain->SetBranchStatus("Earm.BBCal.y",1);
  chain->SetBranchStatus("Earm.BBCal.z",1);
  chain->SetBranchStatus("Earm.BBCal.edep",1);
  chain->SetBranchStatus("Earm.BBCal.p",1);
  chain->SetBranchStatus("Earm.BBCal.mid",1);
  chain->SetBranchStatus("Earm.BBCal.pid",1);
  chain->SetBranchStatus("Harm.HCAL_box.x",1);
  chain->SetBranchStatus("Harm.HCAL_box.y",1);
  chain->SetBranchStatus("Harm.HCAL_box.z",1);
  chain->SetBranchStatus("Harm.HCAL_box.edep",1);
  chain->SetBranchStatus("Harm.HCAL_box.p",1);
  chain->SetBranchStatus("Harm.HCAL_box.mid",1);
  chain->SetBranchStatus("Harm.HCAL_box.pid",1);

  chain->SetBranchAddress("ev",&ev.count);
  chain->SetBranchAddress("gen",&gen.thbb);
  chain->SetBranchAddress("Earm.BBCal.x",&bbcal_x);
  chain->SetBranchAddress("Earm.BBCal.y",&bbcal_y);
  chain->SetBranchAddress("Earm.BBCal.z",&bbcal_z);
  chain->SetBranchAddress("Earm.BBCal.edep",&bbcal_edep);
  chain->SetBranchAddress("Earm.BBCal.p",&bbcal_p);
  chain->SetBranchAddress("Earm.BBCal.mid",&bbcal_mid);
  chain->SetBranchAddress("Earm.BBCal.pid",&bbcal_pid);
  chain->SetBranchAddress("Harm.HCAL_box.x",&hcal_x);
  chain->SetBranchAddress("Harm.HCAL_box.y",&hcal_y);
  chain->SetBranchAddress("Harm.HCAL_box.z",&hcal_z);
  chain->SetBranchAddress("Harm.HCAL_box.edep",&hcal_edep);
  chain->SetBranchAddress("Harm.HCAL_box.p",&hcal_p);
  chain->SetBranchAddress("Harm.HCAL_box.mid",&hcal_mid);
  chain->SetBranchAddress("Harm.HCAL_box.pid",&hcal_pid);


  for(int i = 0; i < 2; i++) {
    hHArmEDep[i] = new TH2F(Form("hHArmEDep%d",i),
        Form("Hadron Arm Energy Deposited: %s;#theta_{e} (deg);Energy Deposited (GeV)",kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],
        kBins,kMinPlotHCalEDep[gKin],kMaxPlotHCalEDep[gKin]);
    hHArmEDep[i]->SetStats(false);
    hEArmEDep[i] = new TH2F(Form("hEArmEDep%d",i),
        Form("Electron Arm Energy Deposited: %s;#theta_{e} (deg);Energy Deposited (GeV)",kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],
        kBins,kMinPlotBBEDep[gKin],kMaxPlotBBEDep[gKin]);
    hEArmEDep[i]->SetStats(false);


    hHArmAngles[i] = new TH2F(Form("hHArmAngles%d",i),
        Form("Hadron Arm Acceptance: %s;#theta_{e} (deg);#phi_{e} (deg)",kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
    hHArmAngles[i]->SetStats(false);
    hEArmAngles[i] = new TH2F(Form("hEArmAngles%d",i),
        Form("Electron Arm Acceptance: %s;#theta_{e} (deg);#phi_{e} (deg)",kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
    hEArmAngles[i]->SetStats(false);
    hBothArmsAngles[i] = new TH2F(Form("hBothArmsAngles%d",i),
        Form("Electron & Hadron Coincidence Acceptance: %s;#theta_{e} (deg);#phi_{e} (deg)",kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
    hBothArmsAngles[i]->SetStats(false);

    hHArmPositions[i] = new TH2F(Form("hHArmPositions%d",i),
        Form("HCAL Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinHCalX,kMaxHCalX,kBins,kMinHCalY,kMaxHCalY);
    hHArmPositions[i]->SetStats(false);
    hEArmPositions[i] = new TH2F(Form("hEArmPositions%d",i),
        Form("BigBite Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinBBX,kMaxBBX,kBins,kMinBBY,kMaxBBY);
    hEArmPositions[i]->SetStats(false);

    hHArmCoincidencePositions[i] = new TH2F(Form("hHArmCoincidencePositions%d",i),
        Form("HCAL Coincidence Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinHCalX,kMaxHCalX,kBins,kMinHCalY,kMaxHCalY);
    hHArmCoincidencePositions[i]->SetStats(false);
    hEArmCoincidencePositions[i] = new TH2F(Form("hEArmCoincidencePositions%d",i),
        Form("BigBite Coincidence Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinBBX,kMaxBBX,kBins,kMinBBY,kMaxBBY);
    hEArmCoincidencePositions[i]->SetStats(false);

    hHArmMomenta[i] = new TH2F(Form("hHArmMomenta%d",i),
        Form("%s Momentum;Final %s Momentum (GeV/c);Initial %s Momentum (GeV/c)",
          kHadronName[i],kHadronName[i],kHadronName[i]),
        kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin],
        kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
    hHArmMomenta[i]->SetStats(false);
    hEArmMomenta[i] = new TH2F(Form("hEArmMomenta%d",i),
        Form("Electron Momentum: %s;Final Electron Momentum (GeV/c);"
          "Initial Electron Momentum",kHadronName[i]),
        kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin],
        kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin]);
    hEArmMomenta[i]->SetStats(false);
    hBothArmsMomenta[i] = new TH2F(Form("hBothArmsMomenta%d",i),
        Form("Electron and %s Momentum Coincidence;Electron Momentum (GeV/c);"
         "%s Momentum (GeV/c)",kHadronName[i],kHadronName[i]),
        kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin],
        kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
    hBothArmsMomenta[i]->SetStats(false);

    hHArmMomentaRatio[i] = new TH1F(Form("hHArmMomentaRatio%d",i),
        Form("%s Momentum;Initial/Final %s Momentum",
          kHadronName[i],kHadronName[i]),
        kBins,0.9,1.01);
    hHArmMomentaRatio[i]->SetStats(false);
    hEArmMomentaRatio[i] = new TH1F(Form("hEArmMomentaRatio%d",i),
        Form("Electron Momentum: %s;Initial/Final Electron Momentum",
        kHadronName[i]),
        kBins,0.5,1.01);
    hEArmMomentaRatio[i]->SetStats(false);

    hAngleMomenta[i] = new TH2F(Form("hAngleMomenta%d",i),
        Form("Hadron Arm Acceptance: %s;#theta_{e} (deg);Hadron Momentum (GeV/c);",
          kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],
        kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
    hAngleMomenta[i]->SetStats(kFALSE);

    hHAngleVPositions[i] = new TH2F(Form("hHAngleVPositions%d",i),
        Form("%s HArm Vertical Position;#theta_{e} (deg);#phi_{e} (deg)",
          kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
    hHAngleVPositions[i]->SetStats(false);
    hHAngleVPositionsCounts[i] = new TH2F(Form("hHAngleVPositionsCounts%d",i),
        Form("%s HArm Vertical Position Count;#theta_{e} (deg);#phi_{e} (deg)",
          kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
    hHAngleVPositionsCounts[i]->SetStats(false);
    hHAngleHPositions[i] = new TH2F(Form("hHAngleHPositions%d",i),
        Form("%s HArm Horizontal Position;#theta_{e} (deg);#phi_{e} (deg)",
          kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
    hHAngleHPositions[i]->SetStats(false);
    hHAngleHPositionsCounts[i] = new TH2F(Form("hHAngleHPositionsCounts%d",i),
        Form("%s HArm Horizontal Position Count;#theta_{e} (deg);#phi_{e} (deg)",
          kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
    hHAngleHPositionsCounts[i]->SetStats(false);

  }
  gStyle->SetOptStat(1110);
  hHAngleVPositionDiffs = new TH2F("hHAngleVPositionDiffs",
      "HArm Vertical Position Difference; #theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hHAngleVPositionDiffs->SetStats(false);
  hHAngleVPositionDiffAvg = new TH1F("hHAngleVPositionDiffAvg",
      "HArm Vertical Position Avg Difference; Avg Difference (m)",
      kBins,kMinPlotVPosDiff,kMaxPlotVPosDiff);
  hHAngleVPositionDiffAvg->SetStats(true);
  hHAngleHPositionDiffs = new TH2F("hHAngleHPositionDiffs",
      "HArm Horizontal Position Difference; #theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hHAngleHPositionDiffs->SetStats(false);
  hHAngleHPositionDiffAvg = new TH1F("hHAngleHPositionDiffAvg",
      "HArm Horizontal Position Avg Difference; Avg Difference (m)",
      kBins,kMinPlotHPosDiff,kMaxPlotHPosDiff);
  hHAngleHPositionDiffAvg->SetStats(true);

  // Initialize the Map histograms
  hHArmAnglesMap = new TH2F(Form("hHArmAnglesMap"),
      Form("Hadron Arm Acceptance (combined);#theta_{e} (deg);#phi_{e} (deg)"),
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hHArmAnglesMap->SetStats(false);
  hHArmAnglesMap->SetFillColorAlpha(kBlue+2,1.0);
  hEArmAnglesMap = new TH2F(Form("hEArmAnglesMap"),
      Form("Electron Arm Acceptance (combined);#theta_{e} (deg);#phi_{e} (deg)"),
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hEArmAnglesMap->SetStats(false);
  hEArmAnglesMap->SetFillColorAlpha(kRed+2,1.0);
  hBothArmsAnglesMap = new TH2F(Form("hBothArmsAnglesMap"),
      Form("Electron & Hadron Coincidence Acceptance (combined);#theta_{e} (deg);#phi_{e} (deg)"),
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hBothArmsAnglesMap->SetStats(false);
  //hBothArmsAnglesMap->SetFillStyle(4001);
  hBothArmsAnglesMap->SetFillColorAlpha(kGreen+2,1.0);

  hAngleMomentaMap = new TH2F(Form("hAngleMomentaMap"),
      "Hadron Arm Acceptances (combined);#theta_{e} (deg);Hadron Momentum (GeV/c)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],
      kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
  hAngleMomentaMap->SetStats(kFALSE);
  hAngleMomentaMap->SetFillColor(kGreen+2);
  hAngleMomentaMapCounts = new TH1F(Form("hAngleMomentaMapCounts"),"",100,0.,0.);


  ///////////////////////////////////////////////
  /// Draw stuff
  TCanvas *canvasAngles = makeCanvas("canvasAngles",3,3);
  TCanvas *canvasEDep = makeCanvas("canvasEDep",2,2);
  TCanvas *canvasSinglePositions = makeCanvas("canvasSinglePositions",2,2);
  TCanvas *canvasCoincidencePositions = makeCanvas("canvasCoincidencePositions",2,2);
  TCanvas *canvasMomenta = makeCanvas("canvasMomenta",2,3);
  TCanvas *canvasMomentumAcceptance = makeCanvas("canvasMomentumAcceptance",3,1);
  TCanvas *canvasAcceptance = makeCanvas("canvasAcceptance",1,1,2.);
  TCanvas *canvasPositionDiffs = makeCanvas("canvasPositionDiffs",2,2);

  // Build Momentum space map
  buildMomentumAcceptance(canvasMomentumAcceptance);
  saveCanvas(canvasMomentumAcceptance,"momentum_accetance");

  // With the map built, now fill in the rest of the histograms
  fillHistograms();

  // Draw scattering angle acceptance
  drawAngles(canvasAngles);
  saveCanvas(canvasAngles,"angles");

  // Draw position acceptances
  drawPositions(canvasSinglePositions,false);
  saveCanvas(canvasSinglePositions,"positions");
  drawPositions(canvasCoincidencePositions,true);
  saveCanvas(canvasCoincidencePositions,"positions_coincidence");

  // Draw momentum of electrons and hadrons
  drawMomenta(canvasMomenta);
  saveCanvas(canvasMomenta,"momenta");

  // Draw the final angular acceptance overlayed
  canvasAcceptance->cd(0);
  hHArmAnglesMap->Draw("box");
  hEArmAnglesMap->Draw("SAME box");
  //hEArmAnglesMap->Draw("box");
  hBothArmsAnglesMap->Draw("SAME box");
  //hHArmAnglesMap->Draw("SAME box");
  saveCanvas(canvasAcceptance,"angle_acceptance");

  // Draw Energy Deposited
  canvasEDep->cd(1);
  hHArmEDep[0]->Draw(kHist2DDrawOption);
  canvasEDep->cd(2);
  hHArmEDep[1]->Draw(kHist2DDrawOption);
  canvasEDep->cd(3);
  hEArmEDep[0]->Draw(kHist2DDrawOption);
  canvasEDep->cd(4);
  hEArmEDep[1]->Draw(kHist2DDrawOption);
  saveCanvas(canvasEDep,"edep");

  canvasPositionDiffs->cd(1);
  hHAngleVPositionDiffs->SetMinimum(0);
  hHAngleVPositionDiffs->Draw(kHist2DDrawOption);
  canvasPositionDiffs->cd(2);
  hHAngleHPositionDiffs->Draw(kHist2DDrawOption);
  canvasPositionDiffs->cd(3);
  hHAngleVPositionDiffAvg->Draw(kHist1DDrawOption);
  fixStatBox(hHAngleVPositionDiffAvg);
  canvasPositionDiffs->cd(4);
  hHAngleHPositionDiffAvg->Draw(kHist1DDrawOption);
  fixStatBox(hHAngleHPositionDiffAvg);
  saveCanvas(canvasPositionDiffs,"proton_neutron_position_diffs");

  std::cout << "---------------------------------------------" << std::endl;
  std::cout << "For Kin " << gKin << std::endl;
  std::cout << "Accepted Neutrons: " << gAcceptedNeutrons << std::endl;
  std::cout << "Accepted Protons:  " << gAcceptedProtons  << std::endl;
  std::cout << "Percent Diff:      " <<
    200.*(gAcceptedNeutrons-gAcceptedProtons)/
    (gAcceptedNeutrons+gAcceptedProtons) << std::endl;

  std::cout << "Average Proton-Neutron vertical position difference:   " <<
    gHCalAvgVPositionDiff << " +/- " << gHCalAvgVPositionDiffE << std::endl;
  std::cout << "Average Proton-Neutron horizontal position difference: " <<
    gHCalAvgHPositionDiff << " +/- " << gHCalAvgHPositionDiffE << std::endl;
  std::cout << "---------------------------------------------" << std::endl;
}
