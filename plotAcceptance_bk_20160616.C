#include "plotAcceptance.h"
#include "shrinkGMn.h"
#include <fstream>

#define FILL_NORM 1.0
#define SIGMA_UNIT 1e34
#define SIGMA_UNIT_STRING #ub
#define FILL_POSITION_NORM

const bool kApplyMomentumCut = false;
const double kMinToleratedELoss = -1.0e3;
const bool kQuickPlot = false;
const bool kFillMapGaps = false;
const bool kSaveCanvas = true;
const int kVerbosity = kWarning; // Disable all Info prints
const double kMinElectronStats = 0.65;
const double kEfficiencyDiffTolerance = 0.02;
const double kEfficiencyDiffPlotRange = 0.02;
const int kBins = 200.;
const bool kSaveIntermediatePlots = false;

const double kQuickPlotPercent = 0.02;


const double kMinDetectedFraction = 0.65*0+0.7;

const bool kPrintDebugBin = true*0;
const int kPrintBinX = 24;
const int kPrintBinY = 25;
const int kPrintBin = kPrintBinX+(kBins+2)*kPrintBinY;

const int kStdErrorValue = -1e6;

const int kFillMapSize = 4;
int gSmallChangeCount = 0;
bool gChangesConverged = false;
const int kConvergenceTestCount = 55;
const double kConvergenceTest = 0.01;
const int kCheckIntermediateAtEntry = 500000;
const int kMinCheckIntermediateEntries = 6000000;

// Test Cuts
const bool kApplyTestHadronMomentaCut = false;
const double kMinTestHadronMomentaCut = 2.74;
const double kMaxTestHadronMomentaCut = 2.76;
const bool kApplyTestElectronMomentaCut = false;
const double kMinTestElectronMomentaCut = 2.74;
const double kMaxTestElectronMomentaCut = 2.76;
const bool kApplyTestPhiCut = false;
const double kMinTestPhiCut = -29;
const double kMaxTestPhiCut = -22;
const bool kApplyTestThetaCut = false;
const double kMinTestThetaCut = 30;
const double kMaxTestThetaCut = 32;
const bool kApplyTestVZCut = false;
const double kMinTestVZCut = -0.050;
const double kMaxTestVZCut = -0.049;
const bool kApplyTestBBPositionCut = false;
const double kMinTestBBPositionXCut = -0.19952;
const double kMaxTestBBPositionXCut = -0.13149;
const double kMinTestBBPositionYCut = -0.88423+1;
const double kMaxTestBBPositionYCut = -0.82882+1;


double kMinBBEDep = 1.0; // GeV/c
double kMinHCalEDep = 1.0; // GeV
int kHArm = 0;
int kEArm = 1;
int kBothArms = 2;
const char* kHadronName[3] = { "Neutron", "Proton", "Both" };
const char* kHist2DDrawOption = "COLZ"; // "CONTZ";
const char* kHist1DDrawOption = "";
const char* kHistMapDrawOption = "box";
//#define FILL_NORM ev.count

int kCanvasX = 300;
int kCanvasY = 300;


// These two are the real values taken from Gregg's CAD sheets
// (questionable!!)
//double kHCalHeight = 3.656; // m
//double kHCalWidth  = 1.366; // m

// These are values from Gregg's CAD
const double kModuleX = 0.1524; // m
const double kModuleY = 0.1524; // m
// Spacing from Ray's prototype pins
const double kModuleSpacingX = 0.00508; // m
const double kModuleSpacingY = 0.00254; // m
// Now some details about how many X,Y modules there are
const double kNumCols = 12;
const double kNumRows = 24;
// Now calculate HCal dimensions when we include the spacing
const double kHCalWidth  = kNumCols*(kModuleX+kModuleSpacingX)-2.*kModuleSpacingX;
const double kHCalHeight = kNumRows*(kModuleY+kModuleSpacingY)-2.*kModuleSpacingY;
// We do not want to accept the outer most module on each axis due to
// efficiency issues (shower may not be fully contained)
const double kExcludeOuterX = 1;
const double kExcludeOuterY = 1;
const double kHCalWidthAccept = kHCalWidth-2.*kExcludeOuterX*(kModuleX+kModuleSpacingX);
const double kHCalHeightAccept = kHCalHeight-2.*kExcludeOuterX*(kModuleY+kModuleSpacingY);

// These are the values presently in the simulation
//const double kModuleX = 0.153;  //m
//const double kModuleY = 0.153;  //m
//const double kModuleSpacingX = 0.575e-2; // m
//const double kModuleSpacingY = 0.194e-2; // m
//const double kNumRows = 24;
//const double kNumCols = 12;
//const double kHCalHeightScale = 1.001;
//const double kHCalWidthScale  = 1.001;
//const double kHCalHeight = kModuleY*kNumRows*kHCalHeightScale;
//const double kHCalWidth  = kModuleX*kNumCols*kHCalWidthScale;
//double kHCalHeight = .153*24*1.001; // m
//double kHCalWidth  = .153*12*1.001; // m
double kBBHeight = 27.*8.5/100.;  // m (taken from g4sbs)
double kBBWidth  = 2.0*37.0/100.; // m (taken from g4sbs)
// Exclude the outer module because the shower may not be fully
// contained in HCal if we accept them
//const double kExcludeCols = 1;
//const double kExcludeRows = 1;
//const double kHCalHeightAccept = kModuleY*(kNumRows-2*kExcludeRows)*kHCalHeightScale;
//const double kHCalWidthAccept  = kModuleX*(kNumCols-2*kExcludeCols)*kHCalWidthScale;

double kMinPlotVPosDiff =  0.0;
double kMaxPlotVPosDiff =  1.5;
double kMinPlotHPosDiff = -0.01;
double kMaxPlotHPosDiff =  0.01;

                            // 0,    1,    2,    3,    4,    5,    6,   7
// open values around n or p acceptance
//double kMinPhi[8]           = {0,  -30,  -35,  -55,  -55,  -55,  -55,  -55 };
//double kMaxPhi[8]           = {0,   30,   35,   55,   55,   55,   55,   55 };
//double kMinTheta[8]         = {0,   20,   20,   40,   30,   20,   20,   20 };
//double kMaxTheta[8]         = {0,   47,   65,   90,   65,   50,   70,   50 };
// Thight values around e+n+p acceptance
double kMinPhi[8]           = {0,  -25,  -25,  -25,  -25,  -25,  -25,  -25 };
double kMaxPhi[8]           = {0,   25,   25,   25,   25,   25,   25,   25 };
//double kMinTheta[8]         = {0,   20,   32,   51,   35,   21,   31,   22 };
//double kMaxTheta[8]         = {0,   47,   55,   78,   62,   48,   58,   49 };
double kMinTheta[8]         = {25, 25, 25, 25, 25, 25, 25, 25};
double kMaxTheta[8]         = {75, 75, 75, 75, 75, 75, 75, 75};
double kMinPlotPElectron[8] = {0,    0,    0,  0.5,    0,    1,    0,  1.5 };
double kMaxPlotPElectron[8] = {4,    4,    4,  2.0,    4,    5,    4,  6.0 };
double kMinPlotPHadron[8]   = {0,  1.8,  1.8,  2.5,  3.5,  3.5,  4.5,    5 };
double kMaxPlotPHadron[8]   = {4,  3.2,  4.3,  5.5,  7.5,  8.5,  9.5,   10 };
double kMinPlotBBEDep[8]    = {0,   0.,   0.,   0.,   0.,   0.,   0.,   0. };
double kMaxPlotBBEDep[8]    = {0,   4.,   4.,   2.,   4.,   4.,   4.,   5. };
double kMinPlotHCalEDep[8]  = {0,   0.,   0.,   2.,   3.,  3.5,   4.,   4. };
double kMaxPlotHCalEDep[8]  = {0,   4.,   4.,   4.,   6.,  7.0,  8.5,  10. };
double kLineCutSlope[8] = { 0., 0.3775, 0.4123, 0.0759, 0.2671, 0.3321, 0.3777, 0.4503 };
double kLineCutIntercept[8] = { 0., -34.0857, -34.2722, -12.5865, -28.4643, -27.4461, -33.2223, -36.1329 };

double kMinPlotMomentumDiffX = -0.075;
double kMaxPlotMomentumDiffX =  0.075;
double kMinPlotMomentumDiffY[8] = { 0.0, 0.42,  0.507, 0.208, 0.358, 0.391, 0.358, 0.270 };
double kMaxPlotMomentumDiffY[8] = { 0.0, 0.421, 0.509, 0.209, 0.359, 0.392, 0.359, 0.271 };
double kMinPlotMomentumDiffZ = -0.2;
double kMaxPlotMomentumDiffZ =  0.2;

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
TString gDate;
TString gVoff;
double gVoffValue;
double gAcceptedNeutrons = 0;
double gAcceptedProtons = 0;
double gHCalAvgVPositionDiff = 0;
double gHCalAvgHPositionDiff = 0;
double gHCalAvgVPositionDiffE = 0;
double gHCalAvgHPositionDiffE = 0;

TChain *chain;
int chain_entries;

TH2F *hHArmEDep[2];
TH2F *hEArmEDep[2];

// Angle acceptance
TH2F *hHArmAngles[3];
TH2F *hEArmAngles;
TH2F *hBothArmsAngles;
TH2F *hGeneratedAngles;
TH2F *hNeutronAngleAcceptance;
TH2F *hProtonAngleAcceptance;
TH2F *hNeutronProtonAngleAcceptance;
TH2F *hAnglesEN[4];
TH2F *hAnglesEP[4];
TH2F *hAnglesEH[4];
TH2F *hAnglesENRG[4];
TH2F *hAnglesEPRG[4];
TH2F *hAnglesEHRG[4];
TH2F *hAnglesNPSum;
TH2F *hAnglesNPDiff;
TH2F *hAnglesNPPercentDiff;
TH2F *hAnglesMap;
TProfile2D *hAnglesSigmaN;
TProfile2D *hAnglesSigmaP;
TProfile2D *hAnglesSolidAngleN;
TProfile2D *hAnglesSolidAngleP;
double gSigmaSumP;
double gSigmaSumN;
double gSigmaSumErrP;
double gSigmaSumErrN;
double gSolidAngleN;
int gSolidAngleBinCount;
std::vector<double> gSolidAngleBinCountVec;
double gSolidAngleP;
double gSolidAngleErrN;
double gSolidAngleErrP;
std::vector<double> gSolidAngleNVec;
std::vector<double> gSolidAnglePVec;
std::vector<double> gSigmaSumNVec;
std::vector<double> gSigmaSumPVec;
std::vector<double> gSigmaSolidAngleNVec;
std::vector<double> gSigmaSolidAnglePVec;
std::vector<double> gIntermediateEntriesCount;
TGraph *graphSigmaSumN;
TGraph *graphSigmaSumP;
TGraph *graphSolidAngle;

// Ratio of particle to generated
TH2F *hAnglesRatioNG; // Neutron singles
TH2F *hAnglesRatioPG; // Proton singles
TH2F *hAnglesRatioEG; // Electron singles
TH2F *hAnglesRatioHG; // Hadron coincidence (n+p)
TH2F *hAnglesRatioAG; // All particle coincidence (e+n+p)
TH2F *hAnglesRatioEGClean; // Electron singles (cleaned of low statistics stuff)

// Ratio of Hadron to Electron
TH2F *hAnglesRatioNE; // Neutron singles
TH2F *hAnglesRatioPE; // Proton singles
TH2F *hAnglesRatioHE; // Hadron coincidence (n+p)

// Momenta acceptance
TH2F *hHArmMomenta[3];
TH2F *hEArmMomenta;
TH2F *hBothArmsMomenta;
TH2F *hHadronAngleMomenta[3];
TH2F *hHadronAngleMomentaMap;
TH2F *hElectronAngleMomenta;
TH2F *hHadronAngleMomentaCoincidence;
TH2F *hElectronAngleMomentaCoincidence;
TH2F *hElectronAngleMomentaMap;
TH1F *hHadronMomentaAvg[2];
TH1F *hElectronMomentaAvg;


TH2F *hHArmAnglesMap;
TH2F *hEArmAnglesMap;
TH2F *hHArmPositions[2];
TH2F *hHArmPositionsInAcceptance[2];
TH2F *hHArmPositionsWeighted[2];
TH2F *hHArmPositionsInAcceptanceWeighted[2];
TH2F *hEArmPositions;
TH2F *hEArmPositionsInAcceptance;
TH2F *hHArmCoincidencePositions[2];
TH2F *hEArmCoincidencePositions;

TH2F *hHAngleVPositionDiffs;
TH2F *hHAngleVPositionDiffsCounts;
TH1F *hHAngleVPositionDiffAvg;
TH2F *hHAngleHPositionDiffs;
TH2F *hHAngleHPositionDiffsCounts;
TH1F *hHAngleHPositionDiffAvg;

TH1F *hHArmMomentumDiff[3];

// Some entry by entry flags
bool passed_test_cuts;
bool passed_quality_check;
bool passed_hcal_position_check;
bool passed_hcal_proton_position_check;
bool passed_hcal_neutron_position_check;

// Global tree variables
ev_small_t ev;
particle_vars_t evars;
particle_vars_t pvars;
particle_vars_t nvars;
bool e_det;
bool p_det;
bool n_det;

int current_entry = -1;

bool checkHCalPosition(double x, double y)
{
  bool status = true;
  if( x <= -kHCalWidthAccept/2. || x >= kHCalWidthAccept/2.)
    status = false;
  if( y <= -kHCalHeightAccept/2. || y >= kHCalHeightAccept/2.)
    status = false;
  return status;
}

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

TString getSavePath(bool inSubdir = false)
{
  TString path(TString::Format("results/hcal_voffset/%s/kin%02d",gDate.Data(),
        gKin));
  if(inSubdir)
    path.Append("/extended/");
  gSystem->mkdir(path);
  return path;
}

void saveCanvas(TCanvas *canvas, const char *comment, const char *ext = "png",
    bool inSubdir = false)
{
  if(!kSaveCanvas)
    return;

  // Silence any debug/info statements
  int level = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kVerbosity;

  // Get the path
  TString path = getSavePath(inSubdir);

  canvas->SaveAs(Form("%s/gmn_kin%02d_voff%s_%s.%s",
        path.Data(),gKin,gVoff.Data(),comment,ext));

  // Return verbosity to previous level
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
  return checkAcceptanceMap(ev.theta,ev.np,
        hHadronAngleMomentaMap);
}

void getEntry(size_t entry)
{
  // Reset flags
  e_det = n_det = p_det = false;

  // Get next entry
  chain->GetEntry(entry);

  // Record current entry
  current_entry = entry;

  passed_quality_check = true;

  // For hadrons, throw away any hadrons that hit on the outer module
  // (because it's possible that their shower is not entirely contained
  // within HCal, and will definitely be hard to resolve)
  if(n_det && !checkHCalPosition(nvars.x,nvars.y)) {
    n_det = false;
  }
  if(p_det && !checkHCalPosition(pvars.x,pvars.y)) {
    p_det = false;
  }


  // The test momenta cut is only used to select out certain momenta to see
  // positions angles etc..
  passed_test_cuts = true;
  // If applicable, did it fail the test momenta cut?
  if(kApplyTestHadronMomentaCut &&
      (ev.np < kMinTestHadronMomentaCut || ev.np > kMaxTestHadronMomentaCut) )
    passed_test_cuts = false;
  // If applicable, did it fail the test phi cut?
  if(kApplyTestPhiCut &&
      (ev.phi < kMinTestPhiCut || ev.phi > kMaxTestPhiCut) )
    passed_test_cuts = false;
  if(kApplyTestThetaCut &&
      (ev.theta < kMinTestThetaCut || ev.theta > kMaxTestThetaCut) )
    passed_test_cuts = false;
  if(kApplyTestVZCut &&
      (ev.vz < kMinTestVZCut || ev.vz > kMaxTestVZCut ) )
      passed_test_cuts = false;
  if(kApplyTestBBPositionCut && ( !e_det ||
        evars.x < kMinTestBBPositionXCut ||
        evars.x > kMaxTestBBPositionXCut ||
        evars.y < kMinTestBBPositionYCut ||
        evars.y > kMaxTestBBPositionYCut ) ) {
    passed_test_cuts = false;
  }
}

void doAnglesRatio()
{

  // Normalize to generated angles
  hAnglesRatioNG->Divide(hHArmAngles[0],hGeneratedAngles);
  hAnglesRatioPG->Divide(hHArmAngles[1],hGeneratedAngles);
  hAnglesRatioHG->Divide(hHArmAngles[2],hGeneratedAngles);
  hAnglesRatioEG->Divide(hEArmAngles,hGeneratedAngles);
  hAnglesRatioAG->Divide(hBothArmsAngles,hGeneratedAngles);
  hAnglesRatioNG->SetMaximum(1.0);
  hAnglesRatioPG->SetMaximum(1.0);
  hAnglesRatioHG->SetMaximum(1.0);
  hAnglesRatioEG->SetMaximum(1.0);
  hAnglesRatioAG->SetMaximum(1.0);
  int nbx = hAnglesNPSum->GetNbinsX();
  int nby = hAnglesNPSum->GetNbinsY();
  int b; // current global bin number
  double bin_th;
  double bin_ph;

  /*
  // Just as a test for my SBS talk, apply the electron efficiency cut
  // and clean to the angles fraction histograms
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      b = bx+(nbx+2)*by;
      bin_th = hAnglesRatioEG->GetXaxis()->GetBinLowEdge(bx);
      bin_ph = hAnglesRatioEG->GetYaxis()->GetBinLowEdge(by);
      if(hAnglesRatioEG->GetBinContent(bx,by) < kMinElectronStats ||
          bin_ph < bin_th*kLineCutSlope[gKin]+kLineCutIntercept[gKin] ) {
        hAnglesRatioNG->SetBinContent(bx,by,0.0);
        hAnglesRatioPG->SetBinContent(bx,by,0.0);
        hAnglesRatioHG->SetBinContent(bx,by,0.0);
        hAnglesRatioEG->SetBinContent(bx,by,0.0);
      }
    }
  }
  */


  for(int i = 0; i < 3; i++) {
    hAnglesENRG[i]->Divide(hAnglesEN[i],hGeneratedAngles);
    hAnglesEPRG[i]->Divide(hAnglesEP[i],hGeneratedAngles);
    hAnglesEHRG[i]->Divide(hAnglesEH[i],hGeneratedAngles);
  }

  // Produce the clean electron singles histogram
  hAnglesRatioEGClean = new TH2F(*hAnglesRatioEG);
  //hAnglesRatioEGClean = hAnglesRatioEG;
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      b = bx+(nbx+2)*by;
      bin_th = hAnglesRatioEGClean->GetXaxis()->GetBinLowEdge(bx);
      bin_ph = hAnglesRatioEGClean->GetYaxis()->GetBinLowEdge(by);
      if(hAnglesRatioEGClean->GetBinContent(bx,by) < kMinElectronStats ||
          bin_ph < bin_th*kLineCutSlope[gKin]+kLineCutIntercept[gKin] ) {
        hAnglesRatioEGClean->SetBinContent(bx,by,0.0);
        hAnglesRatioEGClean->SetBinError(bx,by,1e6);
      }
    }
  }

  hAnglesENRG[3]->Divide(hAnglesENRG[0],hAnglesRatioEGClean);
  hAnglesEPRG[3]->Divide(hAnglesEPRG[0],hAnglesRatioEGClean);
  hAnglesEHRG[3]->Divide(hAnglesEHRG[0],hAnglesRatioEGClean);

  //hAnglesEN[3]->Sumw2();
  //hAnglesEP[3]->Sumw2();
  //hAnglesEH[3]->Sumw2();
  // Now determine the percent difference between neutron and proton
  // detection efficiencies
  //hAnglesNPSum->Add(hAnglesEN[3],hAnglesEP[3],0.5,0.5);
  //hAnglesNPDiff->Add(hAnglesEN[3],hAnglesEP[3],1.,-1.);
  //hAnglesNPPercentDiff->Divide(hAnglesNPDiff,hAnglesNPDiff,1.,1.0);
}

void computeAcceptance()
{
  int nbx = hAnglesMap->GetNbinsX();
  int nby = hAnglesMap->GetNbinsY();

  double cn; // neutron count
  double cp; // proton count
  double diff; // difference
  double avg; // average
  double pdiff; // percent difference
  double en; // electron+neutron coincident count
  double ep; // electron+proton coincidence count
  int b = -1; // bin corresponding to (bx,by)
  double dOmega; // bin solid angle
  gSigmaSumN = 0.0;
  gSigmaSumP = 0.0;
  gSigmaSumErrN = 0.0;
  gSigmaSumErrP = 0.0;
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      // Determine the gloab bin corresponding to (bx,by)
      b = hAnglesENRG[3]->GetBin(bx,by);

      // Get the counts (actually a ratio...)
      cn = hAnglesENRG[3]->GetBinContent(b);
      cp = hAnglesEPRG[3]->GetBinContent(b);
      en = hAnglesENRG[3]->GetBinError(b);
      ep = hAnglesEPRG[3]->GetBinError(b);
      diff = cn-cp;
      avg = (cn+cp)/2.;
      pdiff = diff/avg;
      if(cn > 0 && cp > 0 && TMath::Abs(pdiff) <= kEfficiencyDiffTolerance
          //&& cn >= 0.8 && cp >= 0.8
          ) {
        hAnglesNPSum->SetBinContent(b,avg);
        hAnglesNPDiff->SetBinContent(b,diff);
        hAnglesNPPercentDiff->SetBinContent(b,pdiff);
        hAnglesMap->SetBinContent(b,1.0);
        gSolidAngleBinCount++;

        // Compute the integrated cross section with efficiency
        dOmega = hAnglesSigmaN->GetXaxis()->GetBinWidth(bx)*
          hAnglesSigmaN->GetYaxis()->GetBinWidth(by);
        gSigmaSumN += hAnglesSigmaN->GetBinContent(b)*dOmega*
          hAnglesENRG[3]->GetBinContent(b);
        gSigmaSumP += hAnglesSigmaP->GetBinContent(b)*dOmega*
          hAnglesEPRG[3]->GetBinContent(b);
        gSigmaSumErrN += TMath::Power(hAnglesSigmaN->GetBinError(b),2)*dOmega;
        gSigmaSumErrP += TMath::Power(hAnglesSigmaP->GetBinError(b),2)*dOmega;
      } else {
        hAnglesNPSum->SetBinContent(b,kStdErrorValue);
        hAnglesNPDiff->SetBinContent(b,kStdErrorValue);
        hAnglesNPPercentDiff->SetBinContent(b,kStdErrorValue);
        hAnglesMap->SetBinContent(b,0.0);

        hAnglesSigmaN->SetBinContent(b,0);
        hAnglesSigmaP->SetBinContent(b,0);
        hAnglesSigmaN->SetBinEntries(b,0);
        hAnglesSigmaP->SetBinEntries(b,0);
        hAnglesSolidAngleN->SetBinContent(b,0);
        hAnglesSolidAngleP->SetBinContent(b,0);
        hAnglesSolidAngleN->SetBinEntries(b,0);
        hAnglesSolidAngleP->SetBinEntries(b,0);
      }
    }
  }
  gSigmaSumErrN = TMath::Sqrt(gSigmaSumErrN);
  gSigmaSumErrP = TMath::Sqrt(gSigmaSumErrP);
  hAnglesNPSum->SetMinimum(-kEfficiencyDiffPlotRange);
  hAnglesNPDiff->SetMinimum(-kEfficiencyDiffPlotRange);
  hAnglesNPPercentDiff->SetMinimum(-kEfficiencyDiffPlotRange);
  hAnglesNPSum->SetMaximum(kEfficiencyDiffPlotRange);
  hAnglesNPDiff->SetMaximum(kEfficiencyDiffPlotRange);
  hAnglesNPPercentDiff->SetMaximum(kEfficiencyDiffPlotRange);

  //hAnglesSigmaN->Multiply(hAnglesMap);
  //hAnglesSigmaP->Multiply(hAnglesMap);
  //gSigmaSumN = hAnglesSigmaN->IntegralAndError(1,nbx,1,nby,gSigmaSumErrN,"width");
  //gSigmaSumP = hAnglesSigmaP->IntegralAndError(1,nbx,1,nby,gSigmaSumErrP,"width");
  gSolidAngleN = hAnglesSolidAngleN->IntegralAndError(1,nbx,1,nby,gSolidAngleErrN,"width");
  gSolidAngleP = hAnglesSolidAngleP->IntegralAndError(1,nbx,1,nby,gSolidAngleErrP,"width");

  // Convert solid angle back to radians
  gSolidAngleN *= TMath::Power(TMath::Pi()/180.0,2);
  gSolidAngleP *= TMath::Power(TMath::Pi()/180.0,2);
  gSolidAngleErrN *= TMath::Power(TMath::Pi()/180.0,2);
  gSolidAngleErrP *= TMath::Power(TMath::Pi()/180.0,2);
  gSigmaSumN *= TMath::Power(TMath::Pi()/180.0,2);
  gSigmaSumP *= TMath::Power(TMath::Pi()/180.0,2);
  gSigmaSumErrN *= TMath::Power(TMath::Pi()/180.0,2);
  gSigmaSumErrP *= TMath::Power(TMath::Pi()/180.0,2);
  gSolidAngleNVec.push_back(gSolidAngleN);
  gSolidAnglePVec.push_back(gSolidAngleP);
  gSigmaSumNVec.push_back(gSigmaSumN);
  gSigmaSumPVec.push_back(gSigmaSumP);
  gIntermediateEntriesCount.push_back(current_entry);
  hAnglesMap->SetFillColor(kMagenta+1);
  gSolidAngleBinCountVec.push_back(gSolidAngleBinCount);
}

void drawIntermediateAcceptance(TCanvas *canvasHistos, TCanvas* canvasGraphs)
{
  canvasHistos->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesNPPercentDiff->Draw(kHist2DDrawOption);
  canvasHistos->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesSigmaN->Draw(kHist2DDrawOption);
  canvasHistos->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesSigmaP->Draw(kHist2DDrawOption);
  if(kSaveIntermediatePlots) {
    saveCanvas(canvasHistos,Form("intermediate_plots_%d",current_entry),"png",true);
  }

  // Fill the histograms
  if(graphSigmaSumN)
    delete graphSigmaSumN;
  if(graphSigmaSumP)
    delete graphSigmaSumP;
  if(graphSolidAngle)
    delete graphSolidAngle;
  graphSigmaSumN = new TGraph(gIntermediateEntriesCount.size(),
      gIntermediateEntriesCount.data(),gSigmaSumNVec.data());
  graphSigmaSumP = new TGraph(gIntermediateEntriesCount.size(),
      gIntermediateEntriesCount.data(),gSigmaSumPVec.data());
  graphSolidAngle = new TGraph(gIntermediateEntriesCount.size(),
      gIntermediateEntriesCount.data(),gSolidAngleNVec.data());
      //gIntermediateEntriesCount.data(),gSolidAngleBinCountVec.data());
  graphSigmaSumN->SetMarkerStyle(20);
  graphSigmaSumP->SetMarkerStyle(20);
  graphSolidAngle->SetMarkerStyle(20);
  graphSigmaSumN->SetTitle("Neutron Cross Section;Number of Entries;Integrated Sigma (#mub)");
  graphSigmaSumP->SetTitle("Proton Cross Section;Number of Entries;Integrated Sigma (#mub)");
  graphSolidAngle->SetTitle("Acceptance Solid Angle;Number of Entries);d#Omega (sr)");
  canvasGraphs->cd(1);
  graphSigmaSumN->Draw("AP");
  canvasGraphs->cd(2);
  graphSigmaSumP->Draw("AP");
  canvasGraphs->cd(3);
  graphSolidAngle->Draw("AP");
  saveCanvas(canvasGraphs,"intermediate_graphs");
}

double computeLastDiff(std::vector<double> vec)
{
  if(vec.size()<2)
    return -1e6;
  double current = vec[vec.size()-1];
  double previous = vec[vec.size()-2];
  return 200*(current-previous)/(current+previous);
}

int computeLastDiffCount(std::vector<double> vec)
{
  if(vec.size()<2)
    return -1e6;
  return vec[vec.size()-1]-vec[vec.size()-2];
}

void printIntermediateStats()
{
  int counts = gIntermediateEntriesCount.size();
  std::cout << "vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv" << std::endl;
  std::cout << "Results after " << current_entry << " entries:"
    << std::endl;
  std::cout << "SolidAngle Neutron: " << gSolidAngleN << " +/- " <<
    gSolidAngleErrN << " sr" << std::endl;
  std::cout << "SolidAngle Proton:  " << gSolidAngleP << " +/- " <<
    gSolidAngleErrP << " sr" << std::endl;
  std::cout << "Sigma Neutron:      " << gSigmaSumN << std::endl;
  std::cout << "Sigma Proton:       " << gSigmaSumP << std::endl;
  std::cout << "Bin Count:          " << gSolidAngleBinCount << std::endl;
  if(counts>1) {
    double diffSigmaN = computeLastDiff(gSigmaSumNVec);
    double diffSigmaP = computeLastDiff(gSigmaSumPVec);
    double diffSolidAngle = computeLastDiff(gSolidAngleNVec);
    int diffBinCount = computeLastDiffCount(gSolidAngleBinCountVec);
    /* if(TMath::Abs(diffSigmaN) <= kConvergenceTest &&
        TMath::Abs(diffSigmaP) <= kConvergenceTest && 
       TMath::Abs(diffSolidAngle) <= kConvergenceTest) { */
    /*if(TMath::Abs(diffBinCount) <= kConvergenceTest) {
      gSmallChangeCount++;
    } else {
      gSmallChangeCount = 0;
    }*/
    std::cout << "Percent Differences of last two points" << std::endl;
    std::cout << "Solid Angle:   " << diffSolidAngle << " %" << std::endl;
    std::cout << "Sigma Neutron: " << diffSigmaN << " %" << std::endl;
    std::cout << "Sigma Proton:  " << diffSigmaP << " %" << std::endl;
    std::cout << "BinCount: " << diffBinCount << std::endl;
  }
  std::cout << "^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << std::endl;
}

bool checkConvergence()
{
  return false;
  if(kConvergenceTestCount <= 0)
    return true;

  if(gSolidAngleBinCountVec.size() < size_t(kConvergenceTestCount))
    return false;

  int min = 1e6;
  int max = -1e6;
  int value = 0;
  for(size_t i = gSolidAngleBinCountVec.size()-kConvergenceTestCount;
      i < gSolidAngleBinCountVec.size(); i++) {
    value = gSolidAngleBinCountVec[i];
    if(value < min)
      min = value;
    if(value > max)
      max = value;
  }
  return ((max-min) <= kConvergenceTest);
}

bool inAcceptance()
{
  if(hAnglesMap->GetBinContent(hAnglesMap->FindBin(ev.theta,ev.phi)) == 1)
    return true;

  return false;
}

void fillPositionOfAcceptedHistograms()
{
 for( int entry = 0; entry < chain_entries && !gChangesConverged; entry++ ) {
    getEntry(entry);

    if(!passed_quality_check)
      continue;

    if(!passed_test_cuts)
      continue;

    if(!inAcceptance())
      continue;
    if(e_det) {
      hEArmPositionsInAcceptance->Fill(evars.x,evars.y,FILL_NORM);
    }
    if(n_det) {
      hHArmPositionsInAcceptance[0]->Fill(nvars.x,nvars.y,FILL_NORM);
      hHArmPositionsInAcceptanceWeighted[0]->Fill(nvars.x,nvars.y,ev.nsigma*SIGMA_UNIT);
    }
    if(p_det) {
      hHArmPositionsInAcceptance[1]->Fill(pvars.x,pvars.y,FILL_NORM);
      hHArmPositionsInAcceptanceWeighted[1]->Fill(pvars.x,pvars.y,ev.nsigma*SIGMA_UNIT);
    }

 }

}

void fillHistograms(TCanvas *canvasHistos, TCanvas *canvasGraphs)
{
  // Ensure the errors are computed properly before normalizing
  hHArmAngles[0]->Sumw2();
  hHArmAngles[1]->Sumw2();
  hHArmAngles[2]->Sumw2();
  hEArmAngles->Sumw2();
  hBothArmsAngles->Sumw2();
  hGeneratedAngles->Sumw2();

  double vDiff;
  double hDiff;
  int binEAngles;
  int binNAngles;
  int binPAngles;
  int countEAngles = 0;
  int countPAngles = 0;
  int countNAngles = 0;
  for( int entry = 0; entry < chain_entries && !gChangesConverged; entry++ ) {
    getEntry(entry);
    hGeneratedAngles->Fill(ev.theta,ev.phi,FILL_NORM);
    if(!passed_quality_check)
      continue;

    if(!passed_test_cuts)
      continue;

    binEAngles = binNAngles = binPAngles = -1;

    ///////////////////////////////////////////////////////////////////////////
    // Fill the position histograms
    if(e_det) {
      hEArmPositions->Fill(evars.x,evars.y,FILL_NORM);
    }
    if(n_det) {
      hHArmPositions[0]->Fill(nvars.x,nvars.y,FILL_NORM);
      hHArmPositionsWeighted[0]->Fill(nvars.x,nvars.y,ev.nsigma*SIGMA_UNIT);
    }
    if(p_det) {
      hHArmPositions[1]->Fill(pvars.x,pvars.y,FILL_NORM);
      hHArmPositionsWeighted[1]->Fill(pvars.x,pvars.y,ev.nsigma*SIGMA_UNIT);
    }

    if(e_det&&n_det&&p_det) {
      hEArmCoincidencePositions->Fill(evars.x,evars.y,FILL_NORM);
      hHArmCoincidencePositions[0]->Fill(nvars.x,nvars.y,FILL_NORM);
      hHArmCoincidencePositions[1]->Fill(pvars.x,pvars.y,FILL_NORM);
    }

    ///////////////////////////////////////////////////////////////////////////
    // Fill the angles and momenta histograms
    if(e_det) {
      binEAngles = hEArmAngles->Fill(ev.theta,ev.phi,FILL_NORM);
      hEArmMomenta->Fill(evars.p,ev.ep,FILL_NORM);
      hElectronAngleMomenta->Fill(ev.theta,ev.ep,FILL_NORM);

      // Did we see a hadron
      if(n_det) {
        hAnglesEN[0]->Fill(ev.theta,ev.phi,FILL_NORM);
      } else {
        hAnglesEN[1]->Fill(ev.theta,ev.phi,FILL_NORM);
      }

      if(p_det) {
        hAnglesEP[0]->Fill(ev.theta,ev.phi,FILL_NORM);
      } else {
        hAnglesEP[1]->Fill(ev.theta,ev.phi,FILL_NORM);
      }

      if(n_det&&p_det) {
        hAnglesEH[0]->Fill(ev.theta,ev.phi,FILL_NORM);
      }

      if(!n_det&&!p_det) {
        hAnglesEH[1]->Fill(ev.theta,ev.phi,FILL_NORM);
      }
    }

    if(n_det) {
      hHadronAngleMomenta[0]->Fill(ev.theta,nvars.p,FILL_NORM);
      binNAngles = hHArmAngles[0]->Fill(ev.theta,ev.phi,FILL_NORM);
      hAnglesSigmaN->Fill(ev.theta,ev.phi,ev.nsigma*SIGMA_UNIT*TMath::Sin(ev.th_rad));
      hAnglesSolidAngleN->Fill(ev.theta,ev.phi,TMath::Sin(ev.th_rad));
      hHArmMomenta[0]->Fill(nvars.p,ev.np,FILL_NORM);
      //hHArmEDep[0]->Fill(theta,hcal_edep->at(n_det_index),FILL_NORM);
      gAcceptedNeutrons++;
      if(!e_det)
        hAnglesEN[2]->Fill(ev.theta,ev.phi,FILL_NORM);
    }

    if(p_det) {
      hHadronAngleMomenta[1]->Fill(ev.theta,pvars.p,FILL_NORM);
      binPAngles = hHArmAngles[1]->Fill(ev.theta,ev.phi,FILL_NORM);
      hHArmMomenta[1]->Fill(pvars.p,ev.np,FILL_NORM);
      hAnglesSigmaP->Fill(ev.theta,ev.phi,ev.psigma*SIGMA_UNIT*TMath::Sin(ev.th_rad));
      hAnglesSolidAngleP->Fill(ev.theta,ev.phi,TMath::Sin(ev.th_rad));
      //hHArmEDep[1]->Fill(theta,hcal_edep->at(p_det_index),FILL_NORM);
      gAcceptedProtons++;
      if(!e_det)
        hAnglesEP[2]->Fill(ev.theta,ev.phi,FILL_NORM);
    }
    if(n_det&&p_det) {
      hHadronAngleMomenta[2]->Fill(ev.theta,ev.np,FILL_NORM);
      hHArmAngles[2]->Fill(ev.theta,ev.phi,FILL_NORM);

      hHadronMomentaAvg[0]->Fill(nvars.p,FILL_NORM);
      hHadronMomentaAvg[1]->Fill(pvars.p,FILL_NORM);

      if(!e_det)
        hAnglesEH[2]->Fill(ev.theta,ev.phi,FILL_NORM);


      if(e_det) {

        hHArmMomentumDiff[0]->Fill(pvars.px-nvars.px,FILL_NORM);
        hHArmMomentumDiff[1]->Fill(pvars.py-nvars.py,FILL_NORM);
        hHArmMomentumDiff[2]->Fill(pvars.pz-nvars.pz,FILL_NORM);

        hElectronMomentaAvg->Fill(evars.p,FILL_NORM);
        hHadronAngleMomentaCoincidence->Fill(ev.theta,ev.np,FILL_NORM);
        hElectronAngleMomentaCoincidence->Fill(ev.theta,ev.ep,FILL_NORM);
        hBothArmsAngles->Fill(ev.theta,ev.phi,FILL_NORM);
        //hBothArmsMomenta[ev.nucl]->Fill(evars.p,nvars.p,FILL_NORM);
        hBothArmsMomenta->Fill(ev.ep,ev.np,FILL_NORM);

        // Fill the proton-neutron position differences
        vDiff = pvars.y-nvars.y;
        hDiff = pvars.x-nvars.x;
        hHAngleVPositionDiffs->Fill(ev.theta,ev.phi,vDiff);
        hHAngleHPositionDiffs->Fill(ev.theta,ev.phi,hDiff);
        hHAngleVPositionDiffsCounts->Fill(ev.theta,ev.phi);
        hHAngleHPositionDiffsCounts->Fill(ev.theta,ev.phi);
        hHAngleVPositionDiffAvg->Fill(vDiff,FILL_NORM);
        hHAngleHPositionDiffAvg->Fill(hDiff,FILL_NORM);
      }
    }

    if(kPrintDebugBin) {
      if(binEAngles == kPrintBin || binNAngles == kPrintBin ||
          binPAngles == kPrintBin) {
        int status = 0;
        if(binEAngles == kPrintBin)
          status += 1;
        if(binNAngles == kPrintBin)
          status += 2;
        if(binPAngles == kPrintBin)
          status += 4;
        //std::cout
        //  << "Entry: " << entry
        //  << "\tBin: " << kPrintBin;
        //std::cout << " \t";
        switch(status) {
          case 1:
            std::cout << "e    ";
            break;
          case 2:
            std::cout << "  n  ";
            break;
          case 3:
            std::cout << "e n  ";
            break;
          case 4:
            std::cout << "    p";
            break;
          case 5:
            std::cout << "e   p";
            break;
          case 6:
            std::cout << "  n p";
            break;
          case 7:
            std::cout << "e n p";
            break;
          default:
            break;
        }
        std::cout
          << " \tTh:  " << ev.theta
          << " \tPh:  " << ev.phi
          << " \tPe:  " << ev.ep
          << " \tPh:  " << ev.np
          << std::endl;
      }
    }

    // Compute a running acceptance
    if(entry>=kMinCheckIntermediateEntries && entry%kCheckIntermediateAtEntry==0) {
      doAnglesRatio();
      computeAcceptance();
      drawIntermediateAcceptance(canvasHistos,canvasGraphs);
      printIntermediateStats();
      //if(gSmallChangeCount >= kConvergenceTestCount)
      if(checkConvergence())
        gChangesConverged = true;
    }
  }

  // Get the vertical and horizontal proton-neutron position differences
  double vv,cv;
  double vh,ch;
  for(int bx = 1; bx <= kBins; bx++) {
    for(int by = 1; by <= kBins; by++) {
      vv = hHAngleVPositionDiffs->GetBinContent(bx,by);
      cv = hHAngleVPositionDiffsCounts->GetBinContent(bx,by);
      vh = hHAngleHPositionDiffs->GetBinContent(bx,by);
      ch = hHAngleHPositionDiffsCounts->GetBinContent(bx,by);
      if(cv>0) {
        hHAngleVPositionDiffs->SetBinContent(bx,by,vv/cv);
      }
      if(ch>0) {
        hHAngleHPositionDiffs->SetBinContent(bx,by,vh/ch);
      }

    }
  }

  // Normalize the Position Weighted histograms so that they correspond
  // to an average cross section.
  //hHArmPositionsWeighted[0]->Divide(hHArmPositions[0]);
  //hHArmPositionsWeighted[1]->Divide(hHArmPositions[1]);

  gHCalAvgVPositionDiff  = hHAngleVPositionDiffAvg->GetMean();
  gHCalAvgVPositionDiffE = hHAngleVPositionDiffAvg->GetRMS()/TMath::Sqrt(
      hHAngleVPositionDiffAvg->GetEntries());
  gHCalAvgHPositionDiff  = hHAngleHPositionDiffAvg->GetMean();
  gHCalAvgHPositionDiffE = hHAngleHPositionDiffAvg->GetRMS()/TMath::Sqrt(
      hHAngleHPositionDiffAvg->GetEntries());

  doAnglesRatio();
  computeAcceptance();
  fillPositionOfAcceptedHistograms();
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
  hHArmAngles[2]->Draw(kHist2DDrawOption);
  canvas->cd(4);
  gPad->SetGrid(kTRUE,kTRUE);
  hEArmAngles->Draw(kHist2DDrawOption);
  canvas->cd(5);
  gPad->SetGrid(kTRUE,kTRUE);
  hGeneratedAngles->Draw(kHist2DDrawOption);
  canvas->cd(6);
  gPad->SetGrid(kTRUE,kTRUE);
  hBothArmsAngles->Draw(kHist2DDrawOption);
}

void drawAnglesDetected(TCanvas *canvas)
{
  for(int i = 0; i < 4; i++) {
    canvas->cd(1+i);
    gPad->SetGrid(kTRUE,kTRUE);
    hAnglesENRG[i]->SetMinimum(0.0);
    hAnglesENRG[i]->SetMaximum(0.0);
    hAnglesENRG[i]->Draw(kHist2DDrawOption);
    canvas->cd(5+i);
    gPad->SetGrid(kTRUE,kTRUE);
    hAnglesEPRG[i]->SetMinimum(0.0);
    hAnglesEPRG[i]->SetMaximum(0.0);
    hAnglesEPRG[i]->Draw(kHist2DDrawOption);
    canvas->cd(9+i);
    gPad->SetGrid(kTRUE,kTRUE);
    hAnglesEHRG[i]->SetMinimum(0.0);
    hAnglesEHRG[i]->SetMaximum(0.0);
    hAnglesEHRG[i]->Draw(kHist2DDrawOption);
  }
}

void drawAnglesRatio(TCanvas *canvas)
{
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesRatioNG->Draw(kHist2DDrawOption);
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesRatioPG->Draw(kHist2DDrawOption);
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesRatioHG->Draw(kHist2DDrawOption);
  canvas->cd(4);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesRatioEG->Draw(kHist2DDrawOption);
}

void drawMomentumAcceptance(TCanvas *canvas)
{
  double maxH = hHadronAngleMomenta[0]->GetMaximum();
  double maxE = hElectronAngleMomenta->GetMaximum();
  if(hHadronAngleMomenta[1]->GetMaximum() > maxH)
    maxH = hHadronAngleMomenta[1]->GetMaximum();

  hHadronAngleMomenta[0]->SetMaximum(maxH);
  hHadronAngleMomenta[1]->SetMaximum(maxH);
  hHadronAngleMomenta[2]->SetMaximum(maxH);
  hHadronAngleMomentaCoincidence->SetMaximum(maxH);
  hElectronAngleMomenta->SetMaximum(maxE);
  hElectronAngleMomentaCoincidence->SetMaximum(maxE);

  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hHadronAngleMomenta[0]->Draw(kHist2DDrawOption);
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hHadronAngleMomenta[1]->Draw(kHist2DDrawOption);
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  hHadronAngleMomenta[2]->Draw(kHist2DDrawOption);
  canvas->cd(4);
  gPad->SetGrid(kTRUE,kTRUE);
  hElectronAngleMomenta->Draw(kHist2DDrawOption);
  canvas->cd(5);
  gPad->SetGrid(kTRUE,kTRUE);
  hElectronAngleMomentaCoincidence->Draw(kHist2DDrawOption);
  canvas->cd(6);
  gPad->SetGrid(kTRUE,kTRUE);
  hHadronAngleMomentaCoincidence->Draw(kHist2DDrawOption);
}


/*
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
hEArmMomenta->Draw(kHist2DDrawOption);
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
*/

void drawHCalBox()
{
  TBox *box = new TBox(-kHCalWidth/2.,-kHCalHeight/2.,
      kHCalWidth/2.,kHCalHeight/2.);
  box->SetFillStyle(0);
  box->SetLineColor(kRed);
  box->SetLineWidth(2);
  box->Draw();

  TBox *boxAccept = new TBox(-kHCalWidthAccept/2.,-kHCalHeightAccept/2.,
      kHCalWidthAccept/2.,kHCalHeightAccept/2.);
  boxAccept->SetFillStyle(0);
  boxAccept->SetLineColor(kRed);
  boxAccept->SetLineWidth(1);
  boxAccept->Draw();

}

void drawBBBox()
{
  TBox *box = new TBox(-kBBWidth/2.,-kBBHeight/2.,kBBWidth/2.,kBBHeight/2.);
  box->SetFillStyle(0);
  box->SetLineColor(kBlue);
  box->SetLineWidth(2);
  box->Draw();
}

void drawPositionsInAcceptanceWeighted(TCanvas *canvas)
{
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmPositionsInAcceptanceWeighted[0]->Draw(kHist2DDrawOption);
  drawHCalBox();
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmPositionsInAcceptanceWeighted[1]->Draw(kHist2DDrawOption);
  drawHCalBox();
}


void drawPositionsWeighted(TCanvas *canvas)
{
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmPositionsWeighted[0]->Draw(kHist2DDrawOption);
  drawHCalBox();
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmPositionsWeighted[1]->Draw(kHist2DDrawOption);
  drawHCalBox();
}

void drawPositionsInAcceptance(TCanvas *canvas)
{
  // Draw the singles allowed positions
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmPositionsInAcceptance[0]->Draw(kHist2DDrawOption);
  drawHCalBox();
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmPositionsInAcceptance[1]->Draw(kHist2DDrawOption);
  drawHCalBox();
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  hEArmPositionsInAcceptance->Draw(kHist2DDrawOption);
  drawBBBox();
}

void drawPositions(TCanvas *canvas)
{
  // Draw the singles allowed positions
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
  hEArmPositions->Draw(kHist2DDrawOption);
  drawBBBox();

  // Draw the 3-particle coincidence positions
  canvas->cd(4);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmCoincidencePositions[0]->Draw(kHist2DDrawOption);
  drawHCalBox();
  canvas->cd(5);
  gPad->SetGrid(kTRUE,kTRUE);
  hHArmCoincidencePositions[1]->Draw(kHist2DDrawOption);
  drawHCalBox();
  canvas->cd(6);
  gPad->SetGrid(kTRUE,kTRUE);
  hEArmCoincidencePositions->Draw(kHist2DDrawOption);
  drawBBBox();
}

TH2F *makeAnglesHisto(const char *title, const char* name)
{
  TH2F *histo = new TH2F(name,
      Form("%s;#theta_{e} (deg);#phi_{e} (deg)",title),
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  histo->SetStats(kFALSE);
  return histo;
}

TH2F *makeAnglesHisto(const char *title, const char* name, double max)
{
  TH2F *histo = makeAnglesHisto(title,name);
  histo->SetMaximum(max);
  return histo;
}

TProfile2D *makeAnglesProfile(const char *title, const char* name)
{
  TProfile2D *profile = new TProfile2D(name,
      Form("%s;#theta_{e} (deg);#phi_{e} (deg)",title),
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  profile->SetStats(kFALSE);
  return profile;
}


void make_palette()
{
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
}

void plotAcceptance(int kin = 1, TString voff = "0", TString date="2016_04_20")
{
  // Load personal style sheet
  gROOT->Macro("$JC2ROOTSTYLE");
  gStyle->SetPadRightMargin(.15);
  make_palette();

  // Set some of the global (non constant) variables
  gKin = kin;
  gVoff = voff;
  gVoffValue = voff.Atof()/100.;
  gDate = date;

  if(chain)
    delete chain;

  chain = new TChain("T");
  std::cout << "Found "
    << chain->Add(Form("rootfiles/hcal_voffset/%s/kin%02d/gmn_kin%02d_r*_v%s.root",
          gDate.Data(),kin,kin,voff.Data()));
  chain_entries = chain->GetEntries();
  if(kQuickPlot)
    chain_entries *= kQuickPlotPercent;
  //chain_entries = 5e5;
  std::cout << " rootfiles with total entries: " << chain_entries << std::endl;

  chain->SetBranchAddress("ev",&ev.nsigma);
  chain->SetBranchAddress("evars",&evars.x);
  chain->SetBranchAddress("pvars",&pvars.x);
  chain->SetBranchAddress("nvars",&nvars.x);
  chain->SetBranchAddress("e_det",&e_det);
  chain->SetBranchAddress("p_det",&p_det);
  chain->SetBranchAddress("n_det",&n_det);

  for(int i = 0; i < 3; i++) {
    // HArm Angular Acceptance
    hHArmAngles[i] = new TH2F(Form("hHArmAngles%d",i),
        Form("HArm Angular Acceptance: %s;#theta_{e} (deg);#phi_{e} (deg)",
          kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],
        kBins,kMinPhi[gKin],kMaxPhi[gKin]);
    hHArmAngles[i]->SetStats(false);

    // HArm Momenta Acceptance
    hHArmMomenta[i] = new TH2F(Form("hHArmMomenta%d",i),
        Form("%s Momentum;Final %s Momentum (GeV/c);Initial %s Momentum (GeV/c)",
          kHadronName[i],kHadronName[i],kHadronName[i]),
        kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin],
        kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
    hHArmMomenta[i]->SetStats(false);

    hHadronAngleMomenta[i] = new TH2F(Form("hHadronAngleMomenta%d",i),
        Form("HArm Momentum Acceptance: %s;#theta_{e} (deg);Hadron Momentum (GeV/c);",
          kHadronName[i]),
        kBins,kMinTheta[gKin],kMaxTheta[gKin],
        kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
    hHadronAngleMomenta[i]->SetStats(kFALSE);

  }

  // EArm and Both Angular Acceptance
  hEArmAngles = new TH2F("hEArmAngles",
      "EArm Angular Acceptance;#theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],
      kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hEArmAngles->SetStats(false);
  hBothArmsAngles = new TH2F("hBothArmsAngles",
      "Coincidence(e,n,p) Angular Acceptance;#theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],
      kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hBothArmsAngles->SetStats(false);
  hGeneratedAngles = new TH2F("hGeneratedAngles",
      "Angles Generated;#theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],
      kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hGeneratedAngles->SetStats(false);


  // EArm and Both Momentum Acceptance
  hEArmMomenta = new TH2F("hEArmMomenta",
      "Electron Momentum;Final Electron Momentum (GeV/c);"
      "Initial Electron Momentum",
      kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin],
      kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin]);
  hEArmMomenta->SetStats(false);
  hBothArmsMomenta = new TH2F("hBothArmsMomenta",
      "Electron vs Hadron Momentum Coincidence;Electron Momentum (GeV/c);"
      "Hadron Momentum (GeV/c)",
      kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin],
      kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
  hBothArmsMomenta->SetStats(false);
  hElectronAngleMomenta = new TH2F("hElectronAngleMomenta",
      "Electron Momentum Acceptance;#theta_{e} (deg);"
      "Electron Momentum (GeV/c);",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],
      kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin]);
  hElectronAngleMomenta->SetStats(kFALSE);
  hHadronAngleMomentaCoincidence = new TH2F("hHadronAngleMomentaCoincidence",
      "Hadron Momentum Acceptance Coincidence(e,n,p);#theta_{e} (deg);"
      "Hadron Momentum (GeV/c);",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],
      kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
  hHadronAngleMomentaCoincidence->SetStats(kFALSE);
  hElectronAngleMomentaCoincidence = new TH2F("hElectronAngleMomentaCoincidence",
      "Electron Momentum Acceptance Coincidence(e,n,p);#theta_{e} (deg);"
      "Electron Momentum (GeV/c);",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],
      kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin]);
  hElectronAngleMomentaCoincidence->SetStats(kFALSE);


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

    // Position histograms
    hHArmPositions[i] = new TH2F(Form("hHArmPositions%d",i),
        Form("HArm Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinHCalX,kMaxHCalX,kBins,kMinHCalY,kMaxHCalY);
    hHArmPositions[i]->SetStats(false);
    hHArmPositionsWeighted[i] = new TH2F(Form("hHArmPositionsWeighted%d",i),
        Form("HArm Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinHCalX,kMaxHCalX,kBins,kMinHCalY,kMaxHCalY);
    hHArmPositionsWeighted[i]->SetStats(false);
    hHArmPositionsInAcceptance[i] = new TH2F(Form("hHArmPositionsInAcceptance%d",i),
        Form("HArm Position In Acceptance: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinHCalX,kMaxHCalX,kBins,kMinHCalY,kMaxHCalY);
    hHArmPositionsInAcceptance[i]->SetStats(false);
    hHArmPositionsInAcceptanceWeighted[i] = new TH2F(Form("hHArmPositionsInAcceptanceWeighted%d",i),
        Form("HArm Position In Acceptance: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinHCalX,kMaxHCalX,kBins,kMinHCalY,kMaxHCalY);
    hHArmPositionsInAcceptanceWeighted[i]->SetStats(false);


    hHArmCoincidencePositions[i] = new TH2F(Form("hHArmCoincidencePositions%d",i),
        Form("HArm Coincidence Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinHCalX,kMaxHCalX,kBins,kMinHCalY,kMaxHCalY);
    hHArmCoincidencePositions[i]->SetStats(false);

    hHadronMomentaAvg[i] = new TH1F(Form("hHadronMomentaAvg%d",i),
        Form("%s Momentum;%s Momentum (GeV/c)",kHadronName[i],kHadronName[i]),
        kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
  }
  gStyle->SetOptStat(1110);

  // EArm Position histograms
  hEArmPositions = new TH2F("hEArmPositions",
      "EArm Position;x (m); y(m)",
      kBins,kMinBBX,kMaxBBX,
      kBins,kMinBBY,kMaxBBY);
  hEArmPositions->SetStats(false);
  hEArmPositionsInAcceptance = new TH2F("hEArmPositionsInAcceptance",
      "EArm Position in Acceptance;x (m); y(m)",
      kBins,kMinBBX,kMaxBBX,
      kBins,kMinBBY,kMaxBBY);
  hEArmPositionsInAcceptance->SetStats(false);

  hEArmCoincidencePositions = new TH2F("hEArmCoincidencePositions",
      "EArm Coincidence Position;x (m); y(m)",
      kBins,kMinBBX,kMaxBBX,
      kBins,kMinBBY,kMaxBBY);
  hEArmCoincidencePositions->SetStats(false);


  hHAngleVPositionDiffs = new TH2F("hHAngleVPositionDiffs",
      "HArm Vertical Position Difference; #theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hHAngleVPositionDiffs->SetStats(false);
  hHAngleVPositionDiffsCounts = new TH2F(*hHAngleVPositionDiffs);
  hHAngleVPositionDiffAvg = new TH1F("hHAngleVPositionDiffAvg",
      "HArm Vertical Position Avg Difference; Avg Difference (m)",
      kBins,kMinPlotVPosDiff,kMaxPlotVPosDiff);
  hHAngleVPositionDiffAvg->SetStats(true);
  hHAngleHPositionDiffs = new TH2F("hHAngleHPositionDiffs",
      "HArm Horizontal Position Difference; #theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hHAngleHPositionDiffs->SetStats(false);
  hHAngleHPositionDiffsCounts = new TH2F(*hHAngleHPositionDiffs);
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

  //hBothArmsAnglesMap = new TH2F(Form("hBothArmsAnglesMap"),
  //    Form("Electron & Hadron Coincidence Acceptance (combined);#theta_{e} (deg);#phi_{e} (deg)"),
  //    kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  //hBothArmsAnglesMap->SetStats(false);
  hNeutronAngleAcceptance = new TH2F("hNeutronAngleAcceptanceM",
      //hBothArmsAnglesMap->SetFillColorAlpha(kGreen+2,1.0);
      "Neutron to Electron Count Ratio;#theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hNeutronAngleAcceptance->SetStats(false);
  hProtonAngleAcceptance = new TH2F("hProtonAngleAcceptanceM",
      "Proton to Electron Count Ratio;#theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hProtonAngleAcceptance->SetStats(false);
  hNeutronProtonAngleAcceptance = new TH2F("hNeutronProtonAngleAcceptanceM",
      "N&P Coincidence to Electron Count Ratio;#theta_{e} (deg);#phi_{e} (deg)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],kBins,kMinPhi[gKin],kMaxPhi[gKin]);
  hNeutronProtonAngleAcceptance->SetStats(false);

  hAnglesRatioNG = makeAnglesHisto("Neutron Detected Fraction","hAnglesRatioNG",1.0);
  hAnglesRatioPG = makeAnglesHisto("Proton Detected Fraction","hAnglesRatioPG",1.0);
  hAnglesRatioEG = makeAnglesHisto("Electron Detected Fraction","hAnglesRatioEG",1.0);
  hAnglesRatioHG = makeAnglesHisto("Hadron in Coincidence Detected Fraction","hAnglesRatioHG",1.0);
  hAnglesRatioAG = makeAnglesHisto("All in Coincidence Detected Fraction","hAnglesRatioAG",1.0);
  hAnglesRatioNE = makeAnglesHisto("Neutron:Electron Ratio","hAnglesRatioNE",1.0);
  hAnglesRatioPE = makeAnglesHisto("Proton:Electron Ratio","hAnglesRatioPE",1.0);
  hAnglesRatioEGClean = makeAnglesHisto("Electron Detected Fraction (High Stats)","hAnglesRatioEGClean",1.0);
  hAnglesEN[0] = makeAnglesHisto("Detected: e and n","hAnglesEN0",1.0);
  hAnglesEN[1] = makeAnglesHisto("Detected: e not n","hAnglesEN1",1.0);
  hAnglesEN[2] = makeAnglesHisto("Detected: n not e","hAnglesEN2",1.0);
  hAnglesEN[3] = makeAnglesHisto("Detected: e and n (norm total e)","hAnglesEN3",1.0);
  hAnglesEP[0] = makeAnglesHisto("Detected: e and p","hAnglesEP0",1.0);
  hAnglesEP[1] = makeAnglesHisto("Detected: e not p","hAnglesEP1",1.0);
  hAnglesEP[2] = makeAnglesHisto("Detected: p not e","hAnglesEP2",1.0);
  hAnglesEP[3] = makeAnglesHisto("Detected: e and p (norm total e)","hAnglesEP3",1.0);
  hAnglesEH[0] = makeAnglesHisto("Detected: e and n+p","hAnglesEH0",1.0);
  hAnglesEH[1] = makeAnglesHisto("Detected: e not n or p","hAnglesEH1",1.0);
  hAnglesEH[2] = makeAnglesHisto("Detected: n+p not e","hAnglesEH2",1.0);
  hAnglesEH[3] = makeAnglesHisto("Detected: e and n+p (norm total e)","hAnglesEH3",1.0);
  hAnglesENRG[0] = makeAnglesHisto("Detected: e and n","hAnglesENRG0",1.0);
  hAnglesENRG[1] = makeAnglesHisto("Detected: e not n","hAnglesENRG1",1.0);
  hAnglesENRG[2] = makeAnglesHisto("Detected: n not e","hAnglesENRG2",1.0);
  hAnglesENRG[3] = makeAnglesHisto("Detected: e and n (norm total e)","hAnglesENRG3",1.0);
  hAnglesEPRG[0] = makeAnglesHisto("Detected: e and p","hAnglesEPRG0",1.0);
  hAnglesEPRG[1] = makeAnglesHisto("Detected: e not p","hAnglesEPRG1",1.0);
  hAnglesEPRG[2] = makeAnglesHisto("Detected: p not e","hAnglesEPRG2",1.0);
  hAnglesEPRG[3] = makeAnglesHisto("Detected: e and p (norm total e)","hAnglesEPRG3",1.0);
  hAnglesEHRG[0] = makeAnglesHisto("Detected: e and n+p","hAnglesEHRG0",1.0);
  hAnglesEHRG[1] = makeAnglesHisto("Detected: e not n or p","hAnglesEHRG1",1.0);
  hAnglesEHRG[2] = makeAnglesHisto("Detected: n+p not e","hAnglesEHRG2",1.0);
  hAnglesEHRG[3] = makeAnglesHisto("Detected: e and n+p (norm total e)","hAnglesEHRG3",1.0);


  hAnglesNPSum = makeAnglesHisto("Neutron+Proton Efficiency","hAnglesNPSum");
  hAnglesNPDiff = makeAnglesHisto("Neutron-Proton Efficiency","hAnglesNPDiff");
  hAnglesNPPercentDiff = makeAnglesHisto("Neutron Proton Efficiency Difference","hAnglesNPPercentDiff");

  hAnglesMap = makeAnglesHisto("Acceptance Map","hAnglesMap");
  hAnglesSigmaN = makeAnglesProfile("Neutron d#sigma/d#Omega*sin#theta_{e} (#mub)","hAnglesSigmaN");
  hAnglesSigmaP = makeAnglesProfile("Proton d#sigma/d#Omega*sin#theta_{e} (#mub)","hAnglesSigmaP");
  hAnglesSolidAngleN = makeAnglesProfile("Neutron d#Omega*sin#theta_{e} (sr)","hAnglesSolidAngleN");
  hAnglesSolidAngleP = makeAnglesProfile("Proton d#Omega (sr)","hAnglesSolidAngleP");
  hAnglesSolidAngleN->SetMinimum(0.30);
  hAnglesSolidAngleP->SetMinimum(0.30);

  graphSigmaSumN = 0;
  graphSigmaSumP = 0;
  graphSolidAngle = 0;

  //hBothArmsAnglesMap->SetFillStyle(4001);

  hHadronAngleMomentaMap = new TH2F(Form("hHadronAngleMomentaMap"),
      "Hadron Arm Acceptances (combined);#theta_{e} (deg);Hadron Momentum (GeV/c)",
      kBins,kMinTheta[gKin],kMaxTheta[gKin],
      kBins,kMinPlotPHadron[gKin],kMaxPlotPHadron[gKin]);
  hHadronAngleMomentaMap->SetStats(kFALSE);
  hHadronAngleMomentaMap->SetFillColor(kGreen+2);


  hElectronMomentaAvg = new TH1F("hElectronMomentaAvg",
      "Electron Momentum;Electron Momentum (GeV/c)",
      kBins,kMinPlotPElectron[gKin],kMaxPlotPElectron[gKin]);

  // Prepare the momentum difference histograms
  TString axisLabel[3] = { "x", "y", "z" };
  for(int axis = 0; axis < 3; axis++) {
    double min,max;
    switch(axis) {
      case 0:
        min = kMinPlotMomentumDiffX;
        max = kMaxPlotMomentumDiffX;
        break;
      case 1:
        min = kMinPlotMomentumDiffY[gKin];
        max = kMaxPlotMomentumDiffY[gKin];
        break;
      case 2:
        min = kMinPlotMomentumDiffZ;
        max = kMaxPlotMomentumDiffZ;
        break;
      default:
        continue;
        break;
    }
    hHArmMomentumDiff[axis] = new TH1F(
        Form("hHArmMomentumDiff%s",axisLabel[axis].Data()),
        Form("Proton-Neutron Momentum Difference (P%s);Momentum (GeV/c)",
          axisLabel[axis].Data()),kBins,min,max);
  }

  ///////////////////////////////////////////////
  /// Draw stuff
  TCanvas *canvasAngles = makeCanvas("canvasAngles",3,2);
  TCanvas *canvasAnglesDetected = makeCanvas("canvasAnglesDetected",4,3);
  TCanvas *canvasAnglesRatio = makeCanvas("canvasAnglesRatio",4,1);
  //TCanvas *canvasEDep = makeCanvas("canvasEDep",2,2);
  TCanvas *canvasPositions = makeCanvas("canvasPositions",3,2);
  TCanvas *canvasPositionsWeighted = makeCanvas("canvasPositionsWeighted",2,1);
  TCanvas *canvasPositionsInAcceptance = makeCanvas("canvasPositionsInAcceptance",3,1);
  TCanvas *canvasPositionsInAcceptanceWeighted = makeCanvas("canvasPositionsInAcceptanceWeighted",2,1);
  //TCanvas *canvasMomenta = makeCanvas("canvasMomenta",2,3);
  TCanvas *canvasMomentumAcceptance = makeCanvas("canvasMomentumAcceptance",3,2);
  //TCanvas *canvasAcceptance = makeCanvas("canvasAcceptance",1,1,2.);
  TCanvas *canvasPositionDiffs = makeCanvas("canvasPositionDiffs",2,2);
  TCanvas *canvasMomentumDiffs = makeCanvas("canvasMomentumDiffs",3,1);
  TCanvas *canvasMomentumAvg = makeCanvas("canvasMomentumAvg",2,2);
  TCanvas *canvasAnglesNPEfficiency = makeCanvas("canvasAnglesNPEfficiency",1,1,2);
  TCanvas *canvasAnglesAcceptance = makeCanvas("canvasAnglesAcceptance",3,1,2);
  TCanvas *canvasAnglesIntermediate = makeCanvas("canvasAnglesIntermediate",3,1);
  TCanvas *canvasIntermediateGraphs = makeCanvas("canvasIntermediateGraphs",3,1);

  // Walk the tree and fill the histograms
  fillHistograms(canvasAnglesIntermediate,canvasIntermediateGraphs);
  if(gChangesConverged) {
    std::cout << "Solid angle stable after " << current_entry << " entries."
      << std::endl;
    printIntermediateStats();
  } else {
    printIntermediateStats();
    std::cout << "WARNING: Solid angle did not reach stability!" << std::endl;
  }

  // Build Momentum space map
  drawMomentumAcceptance(canvasMomentumAcceptance);
  saveCanvas(canvasMomentumAcceptance,"momentum_accetance");

  // Draw scattering angle acceptance
  drawAngles(canvasAngles);
  saveCanvas(canvasAngles,"angles");

  // Draw scattering angle acceptance
  drawAnglesDetected(canvasAnglesDetected);
  saveCanvas(canvasAnglesDetected,"angles_detected");

  // Draw ratios of angles
  drawAnglesRatio(canvasAnglesRatio);
  saveCanvas(canvasAnglesRatio,"angles_ratio");

  // Draw position acceptances
  drawPositions(canvasPositions);
  saveCanvas(canvasPositions,"positions");
  drawPositionsWeighted(canvasPositionsWeighted);
  saveCanvas(canvasPositionsWeighted,"positions_weighted");
  // Draw position acceptances
  drawPositionsInAcceptance(canvasPositionsInAcceptance);
  saveCanvas(canvasPositionsInAcceptance,"positions_in_acceptance");
  drawPositionsInAcceptanceWeighted(canvasPositionsInAcceptanceWeighted);
  saveCanvas(canvasPositionsInAcceptanceWeighted,"positions_weighted_in_acceptance");


  // Draw momentum differences
  for(int axis = 0; axis < 3; axis++) {
    canvasMomentumDiffs->cd(axis+1);
    hHArmMomentumDiff[axis]->Draw();
    fixStatBox(hHArmMomentumDiff[axis]);
  }
  saveCanvas(canvasMomentumDiffs,"momentum_diffs");

  canvasMomentumAvg->cd(1);
  hHadronMomentaAvg[0]->Draw();
  fixStatBox(hHadronMomentaAvg[0]);
  canvasMomentumAvg->cd(2);
  hHadronMomentaAvg[1]->Draw();
  fixStatBox(hHadronMomentaAvg[1]);
  canvasMomentumAvg->cd(3);
  hElectronMomentaAvg->Draw();
  fixStatBox(hElectronMomentaAvg);
  saveCanvas(canvasMomentumAvg,"momentum_avg");

  // Draw momentum of electrons and hadrons
  //drawMomenta(canvasMomenta);
  //saveCanvas(canvasMomenta,"momenta");

  // Draw the final angular acceptance overlayed
  //canvasAcceptance->cd(0);
  //hHArmAnglesMap->Draw("box");
  //hEArmAnglesMap->Draw("SAME box");
  //hEArmAnglesMap->Draw("box");
  //hBothArmsAnglesMap->Draw("SAME box");
  //hHArmAnglesMap->Draw("SAME box");
  //saveCanvas(canvasAcceptance,"angle_acceptance");

  // Draw Energy Deposited
  //canvasEDep->cd(1);
  //hHArmEDep[0]->Draw(kHist2DDrawOption);
  //canvasEDep->cd(2);
  //hHArmEDep[1]->Draw(kHist2DDrawOption);
  //canvasEDep->cd(3);
  //hEArmEDep[0]->Draw(kHist2DDrawOption);
  //canvasEDep->cd(4);
  //hEArmEDep[1]->Draw(kHist2DDrawOption);
  //saveCanvas(canvasEDep,"edep");

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

  std::cout << "Average Momenta Kick: " << hHArmMomentumDiff[1]->GetMean()
    << " GeV/c" << std::endl;
  std::cout << "Average Hadron momentum: " << hHadronMomentaAvg[0]->GetMean()
    << " GeV/c" << std::endl;

  if(kPrintDebugBin) {
    std::cout << "MinTheta: "
      << hEArmAngles->GetXaxis()->GetBinLowEdge(kPrintBinX) << std::endl;
    std::cout << "MaxTheta: "
      << hEArmAngles->GetXaxis()->GetBinLowEdge(kPrintBinX) +
      hEArmAngles->GetXaxis()->GetBinWidth(kPrintBinX) << std::endl;
    std::cout << "MinPhi:   "
      << hEArmAngles->GetYaxis()->GetBinLowEdge(kPrintBinY) << std::endl;
    std::cout << "MaxPhi:   "
      << hEArmAngles->GetYaxis()->GetBinLowEdge(kPrintBinY) +
      hEArmAngles->GetYaxis()->GetBinWidth(kPrintBinY) << std::endl;
  }

  canvasAnglesNPEfficiency->cd(0);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesNPPercentDiff->Draw(kHist2DDrawOption);
  saveCanvas(canvasAnglesNPEfficiency,"neutron_proton_efficiency");

  canvasAnglesAcceptance->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesMap->Draw(kHistMapDrawOption);
  canvasAnglesAcceptance->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesSigmaN->Draw(kHist2DDrawOption);
  canvasAnglesAcceptance->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  hAnglesSigmaP->Draw(kHist2DDrawOption);
  saveCanvas(canvasAnglesAcceptance,"acceptance");

  canvasAnglesNPEfficiency->cd(0);
  gPad->SetGrid(kTRUE,kTRUE);
  hEArmAngles->Draw(kHist2DDrawOption);
  //canvasAnglesNPEfficiency->SaveAs(Form("results/hcal_voffset/histo%02d.C",gKin));
  saveCanvas(canvasAnglesNPEfficiency,"efficiency_histogram","C");

  std::cout << "Results for kin " << gKin << " voffset=" << voff.Data() << std::endl;
  std::cout << "Sigma*SolidAngle Neutron: " << gSigmaSumN << " +/- " << gSigmaSumErrN << std::endl;
  std::cout << "Sigma*SolidAngle Proton:  " << gSigmaSumP << " +/- " << gSigmaSumErrP << std::endl;
  std::cout << "SolidAngle(both):         " << gSolidAngleN << " +- " << gSolidAngleErrN << std::endl;
  std::cout << "Entries processed:        " << current_entry+1 << std::endl;
  std::cout << "---------------------------------------------" << std::endl;

  fstream outFile;
  outFile.open(
      TString::Format("%s/kin%02d_voff%s.dat",getSavePath().Data(),gKin,voff.Data()),
      std::ios::out);
  outFile << "Results for kin " << gKin << " voffset=" << voff.Data() << std::endl;
  outFile << "Sigma*SolidAngle Neutron: " << gSigmaSumN << " +/- " << gSigmaSumErrN << std::endl;
  outFile << "Sigma*SolidAngle Proton:  " << gSigmaSumP << " +/- " << gSigmaSumErrP << std::endl;
  outFile << "SolidAngle(both):         " << gSolidAngleN << " +- " << gSolidAngleErrN << std::endl;
  outFile << "Entries processed:        " << current_entry+1 << std::endl;
  outFile.close();
}
