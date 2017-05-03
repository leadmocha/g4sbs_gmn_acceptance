#include "GMnAcceptance.h"
#include "GMnColors.h"
#include <fstream>
#include <TMath.h>
#include <TBox.h>
#include <TPaveStats.h>

#include <iostream>

#define FILL_NORM 1.0
// Integrated cross sections in terms of nano-barns now
#define SIGMA_UNIT 1e36
#define SOLID_UNIT 1e3
#define SIGMA_UNIT_STRING #pb
#define FILL_POSITION_NORM

const double kMinToleratedELoss = -1.0e3;
const bool kFillMapGaps = false;
const bool kSaveCanvas = true;
const int kVerbosity = kWarning; // Disable all Info prints
const double kMinElectronStats = 0.65;
const double kEfficiencyDiffTolerance = 0.02;
const double kEfficiencyDiffPlotRange = 0.02;
const int kBins = 200.;
const bool kSaveIntermediatePlots = false;
const bool kCleanAcceptanceMap = true;

// These come from the updated proposal on page 5 (given in degrees)
const double kThetaPQCut[8] = {0.0, 2.5, 2.3, 2.0, 1.1, 0.9, 0.7, 0.6 };
const double kExclusionZoneCutStep = 0.025;  // in m
const int kExclusionZoneCutMaxSteps = 10;


const double kMinDetectedFraction = 0.65*0+0.7;

const bool kPrintDebugBin = true*0;
const int kPrintBinX = 24;
const int kPrintBinY = 25;
const int kPrintBin = kPrintBinX+(kBins+2)*kPrintBinY;

const int kStdErrorValue = -1e6;

const int kFillMapSize = 4;
//int gSmallChangeCount = 0;
//bool gChangesConverged = false;
const int kConvergenceTestCount = 55;
const double kConvergenceTest = 0.01;
const int kCheckIntermediateAtEntry = 500000;
const int kMinCheckIntermediateEntries = 6000000;

// Test Cuts
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


int kHArm = 0;
int kEArm = 1;
int kBothArms = 2;
const char* kHadronName[3] = { "Neutron", "Proton", "P+N" };
const char* kHist2DDrawOption = "COLZ"; // "CONTZ";
const char* kHist1DDrawOption = "";
const char* kHistMapDrawOption = "BOX";
//#define FILL_NORM fData.ev.count

int kCanvasX = 600;
int kCanvasY = 600;


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
double kMinPlotHPosDiff = -0.1;
double kMaxPlotHPosDiff =  0.1;

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
double kLineCutSlope[8] = { 0., 0.3775, 0.4123, 0.0759, 0.2671, 0.3321, 0.3777, 0.4503 };
double kLineCutIntercept[8] = { 0., -34.0857, -34.2722, -12.5865, -28.4643, -27.4461, -33.2223, -36.1329 };

double kPlotPositionPadding = 1.15;
double kMinBBX  = -kPlotPositionPadding*kBBHeight/2.;
double kMaxBBX  =  kPlotPositionPadding*kBBHeight/2.;
double kMinBBY  = -kPlotPositionPadding*kBBHeight/2.;
double kMaxBBY  =  kPlotPositionPadding*kBBHeight/2.;

double kMinHCalX  = -kPlotPositionPadding*kHCalHeight/2.;
double kMaxHCalX  =  kPlotPositionPadding*kHCalHeight/2.;
double kMinHCalY  = -kPlotPositionPadding*kHCalHeight/2.;
double kMaxHCalY  =  kPlotPositionPadding*kHCalHeight/2.;



GMnAcceptance::GMnAcceptance(int kin, double voff, TString resultsPath) :
  fKin(kin), fVOff(voff), fResultsPath(resultsPath),fAcceptedNeutrons(0),
  fAcceptedProtons(0)
{
  //Init();
}

GMnAcceptance::~GMnAcceptance()
{
}

void GMnAcceptance::Init()
{
  // Init the results structure
  fResults.voff = fVOff;

  // int numTotalBins = (kBins+2)*(kBins+2);

  // Specify the position "exclusion" zones where protons and neutrons
  // won't be identified for their given electron scattering angle

  // Specify the position "exclusion" zones where protons and neutrons
  // won't be identified for their given electron scattering angle
  // (the exclusion zone is given in meters)
  int steps = 0;
  for(double r = 0.0; r < fThetaPQRadius && steps < kExclusionZoneCutMaxSteps;
      r += kExclusionZoneCutStep) {
    fPositionExclusionZone.push_back(r);
    steps++;
  }

  for(int i = 0; i < 3; i++) {
    // HArm Angular Acceptance
    fhHArmAngles[i] = MakeAnglesHisto(TString::Format(
          "HArm Angular Acceptance: %s;#theta_{e} (deg);#phi_{e} (deg)",
          kHadronName[i]),
        TString::Format("fhHArmAngles%d",i));
    fhHArmAngles[i]->Sumw2();
  }

  // EArm and Both Angular Acceptance
  fhEArmAngles = MakeAnglesHisto("EArm Angular Acceptance","fhEArmAngles");
  fhBothArmsAngles = MakeAnglesHisto("Coincidence(e,n,p) Angular Acceptance",
      "fhBothArmsAngles");
  fhGeneratedAngles = MakeAnglesHisto("Angles Generated","fhGeneratedAngles");
  fhEArmAngles->Sumw2();
  fhBothArmsAngles->Sumw2();
  fhGeneratedAngles->Sumw2();

  for(int i = 0; i < 2; i++) {
    // Position histograms
    fhHArmPositions[i] = MakeHCalPositionHisto("","fhHArmPositions",i);
    fhHArmPositionsWeighted[i] = MakeHCalPositionHisto("","fhHArmPositionsWeighted",i);
    fhHArmPositionsWithTrig[i] = MakeHCalPositionHisto("","fhHArmPositionsWithTrig",i);
    fhHArmPositionsWeightedWithTrig[i] = MakeHCalPositionHisto("","fhHArmPositionsWeightedWithTrig",i);
    fhHArmPositionsInAcceptance[i] = MakeHCalPositionHisto("in Acceptance",
        "fhHArmPositionsInAcceptance",i);
    fhHArmPositionsInAcceptanceWeighted[i] = MakeHCalPositionHisto("in Acceptance",
        "fhHArmPositionsInAcceptanceWeighted",i);
    fhHArmCoincidencePositions[i] = MakeHCalPositionHisto("in Coincidence",
        "fhHArmCoincidencePositions",i);
  }
  gStyle->SetOptStat(1110);

  // EArm Position histograms
  fhEArmPositions =  MakeBBPositionHisto("","fhEArmPositions");
  fhEArmPositionsInAcceptance = MakeBBPositionHisto("in Acceptance",
      "fhEArmPositionsInAcceptance");
  fhEArmCoincidencePositions = MakeBBPositionHisto("in Coincidence",
      "fhEArmCoincidencePositions");

  fhHAngleVPositionP = MakeAnglesProfile("HCal Vertical Proton Position",
      "fhHAngleVPositionP");
  fhHAngleVPositionN = MakeAnglesProfile("HCal Vertical Neutron Position",
      "fhHAngleVPositionN");
  fhHAngleHPositionP = MakeAnglesProfile("HCal Horizontal Proton Position",
      "fhHAngleHPositionP");
  fhHAngleHPositionN = MakeAnglesProfile("HCal Horizontal Neutron Position",
      "fhHAngleHPositionN");
  fhHAngleVPositionDiffs = MakeAnglesProfile("HCal Vertical Position Difference",
      "fhHAngleVPositionDiffs");
  fhHAngleHPositionDiffs = MakeAnglesProfile("HCal Horizontal Position Difference",
      "fhHAngleHPositionDiffs");
  fhHAngleVPositionDiffAvg  = MakeHCalPositionDiffHisto(
      "fhHAngleVPositionDiffAvg",1);
  fhHAngleHPositionDiffAvg  = MakeHCalPositionDiffHisto(
      "fhHAngleHPositionDiffAvg",0);
  fhHAngleDetectedN = MakeAnglesHisto("Neutrons Detected","fhHAngleDetectedN");
  fhHAngleDetectedP = MakeAnglesHisto("Protons Detected","fhHAngleDetectedP");
  for(size_t i = 0; i < fPositionExclusionZone.size(); i++) {
    fhHAngleGoodIdentificationN.push_back(MakeAnglesHisto(
          TString::Format("Neutrons Good Identification (%gcm exclusion zone)",
            fPositionExclusionZone[i]*100.),
          TString::Format("fhHAngleGoodIdentificationN%zu",i)));
    fhHAngleGoodIdentificationP.push_back(MakeAnglesHisto(
          TString::Format("Protons Good Identification (%gcm exclusion zone)",
            fPositionExclusionZone[i]*100.),
          TString::Format("fhHAngleGoodIdentificationP%zu",i)));
    fhHAngleBadIdentificationN.push_back(MakeAnglesHisto(
          TString::Format("Neutrons Bad Identification (%gcm exclusion zone)",
            fPositionExclusionZone[i]*100.),
          TString::Format("fhHAngleBadIdentificationN%zu",i)));
    fhHAngleBadIdentificationP.push_back(MakeAnglesHisto(
          TString::Format("Protons Bad Identification (%gcm exclusion zone)",
            fPositionExclusionZone[i]*100.),
          TString::Format("fhHAngleBadIdentificationP%zu",i)));
  }

  // Initialize the Map histograms
  fhHArmAnglesMap = MakeAnglesHisto(
      "Hadron Arm Acceptance (combined);#theta_{e} (deg);#phi_{e} (deg)",
      "fhHArmAnglesMap");
  fhEArmAnglesMap = MakeAnglesHisto(
      "Electron Arm Acceptance (combined);#theta_{e} (deg);#phi_{e} (deg)",
      "fhEArmAnglesMap");
  fhNeutronAngleAcceptance = MakeAnglesHisto(
      "Neutron to Electron Count Ratio;#theta_{e} (deg);#phi_{e} (deg)",
      "fhNeutronAngleAcceptanceM");
  fhProtonAngleAcceptance = MakeAnglesHisto(
      "Proton to Electron Count Ratio;#theta_{e} (deg);#phi_{e} (deg)",
      "fhProtonAngleAcceptanceM");
  fhNeutronProtonAngleAcceptance = MakeAnglesHisto(
      "N&P Coincidence to Electron Count Ratio;#theta_{e} (deg);#phi_{e} (deg)",
      "fhNeutronProtonAngleAcceptanceM");

  fhAnglesRatioNG = MakeAnglesHisto("Neutron Detected Fraction","fhAnglesRatioNG",1.0);
  fhAnglesRatioPG = MakeAnglesHisto("Proton Detected Fraction","fhAnglesRatioPG",1.0);
  fhAnglesRatioEG = MakeAnglesHisto("Electron Detected Fraction","fhAnglesRatioEG",1.0);
  fhAnglesRatioHG = MakeAnglesHisto("Hadron in Coincidence Detected Fraction","fhAnglesRatioHG",1.0);
  fhAnglesRatioAG = MakeAnglesHisto("All in Coincidence Detected Fraction","fhAnglesRatioAG",1.0);
  fhAnglesRatioNE = MakeAnglesHisto("Neutron:Electron Ratio","fhAnglesRatioNE",1.0);
  fhAnglesRatioPE = MakeAnglesHisto("Proton:Electron Ratio","fhAnglesRatioPE",1.0);
  fhAnglesRatioEGClean = MakeAnglesHisto("Electron Detected Fraction (High Stats)","fhAnglesRatioEGClean",1.0);
  fhAnglesEN[0] = MakeAnglesHisto("Detected: e and n","fhAnglesEN0",1.0);
  fhAnglesEN[1] = MakeAnglesHisto("Detected: e not n","fhAnglesEN1",1.0);
  fhAnglesEN[2] = MakeAnglesHisto("Detected: n not e","fhAnglesEN2",1.0);
  fhAnglesEN[3] = MakeAnglesHisto("Detected: e and n (norm total e)","fhAnglesEN3",1.0);
  fhAnglesEP[0] = MakeAnglesHisto("Detected: e and p","fhAnglesEP0",1.0);
  fhAnglesEP[1] = MakeAnglesHisto("Detected: e not p","fhAnglesEP1",1.0);
  fhAnglesEP[2] = MakeAnglesHisto("Detected: p not e","fhAnglesEP2",1.0);
  fhAnglesEP[3] = MakeAnglesHisto("Detected: e and p (norm total e)","fhAnglesEP3",1.0);
  fhAnglesEH[0] = MakeAnglesHisto("Detected: e and n+p","fhAnglesEH0",1.0);
  fhAnglesEH[1] = MakeAnglesHisto("Detected: e not n or p","fhAnglesEH1",1.0);
  fhAnglesEH[2] = MakeAnglesHisto("Detected: n+p not e","fhAnglesEH2",1.0);
  fhAnglesEH[3] = MakeAnglesHisto("Detected: e and n+p (norm total e)","fhAnglesEH3",1.0);
  fhAnglesENRG[0] = MakeAnglesHisto("Detected: e and n","fhAnglesENRG0",1.0);
  fhAnglesENRG[1] = MakeAnglesHisto("Detected: e not n","fhAnglesENRG1",1.0);
  fhAnglesENRG[2] = MakeAnglesHisto("Detected: n not e","fhAnglesENRG2",1.0);
  fhAnglesENRG[3] = MakeAnglesHisto("Detected: e and n (norm total e)","fhAnglesENRG3",1.0);
  fhAnglesEPRG[0] = MakeAnglesHisto("Detected: e and p","fhAnglesEPRG0",1.0);
  fhAnglesEPRG[1] = MakeAnglesHisto("Detected: e not p","fhAnglesEPRG1",1.0);
  fhAnglesEPRG[2] = MakeAnglesHisto("Detected: p not e","fhAnglesEPRG2",1.0);
  fhAnglesEPRG[3] = MakeAnglesHisto("Detected: e and p (norm total e)","fhAnglesEPRG3",1.0);
  fhAnglesEHRG[0] = MakeAnglesHisto("Detected: e and n+p","fhAnglesEHRG0",1.0);
  fhAnglesEHRG[1] = MakeAnglesHisto("Detected: e not n or p","fhAnglesEHRG1",1.0);
  fhAnglesEHRG[2] = MakeAnglesHisto("Detected: n+p not e","fhAnglesEHRG2",1.0);
  fhAnglesEHRG[3] = MakeAnglesHisto("Detected: e and n+p (norm total e)","fhAnglesEHRG3",1.0);


  fhAnglesNPSum = MakeAnglesHisto("Neutron+Proton Efficiency","fhAnglesNPSum");
  fhAnglesNPDiff = MakeAnglesHisto("Neutron-Proton Efficiency","fhAnglesNPDiff");
  fhAnglesNPPercentDiff = MakeAnglesHisto("Neutron Proton Efficiency Difference","fhAnglesNPPercentDiff");
  fhAnglesNPPercentDiffBad = MakeAnglesHisto("Neutron Proton Efficiency Difference (Bad)","fhAnglesNPPercentDiffBad");

  fhAnglesMap = MakeAnglesHisto("Acceptance Map","fhAnglesMap");
  fhAnglesSigmaN = MakeAnglesProfile("Neutron d#sigma/d#Omega*sin#theta_{e} (pb)","fhAnglesSigmaN");
  fhAnglesSigmaP = MakeAnglesProfile("Proton d#sigma/d#Omega*sin#theta_{e} (pb)","fhAnglesSigmaP");
  fhAnglesSigmaE = MakeAnglesProfile("E(N+P) d#sigma/d#Omega*sin#theta_{e} (pb)","fhAnglesSigmaE");
  //fhAnglesSolidAngleN = MakeAnglesProfile("Neutron d#Omega*sin#theta_{e} (sr)","fhAnglesSolidAngleN");
  //fhAnglesSolidAngleP = MakeAnglesProfile("Proton d#Omega (sr)","fhAnglesSolidAngleP");
  //fhAnglesSolidAngleN->SetMinimum(0.30);
  //fhAnglesSolidAngleP->SetMinimum(0.30);

  //fhBothArmsAnglesMap->SetFillStyle(4001);

  ///////////////////////////////////////////////
  /// Draw stuff
  fCanvasAngles = MakeCanvas("fCanvasAngles",3,2);
  fCanvasAnglesDetected = MakeCanvas("fCanvasAnglesDetected",4,3);
  fCanvasAnglesRatio = MakeCanvas("fCanvasAnglesRatio",4,1);
  fCanvasPositions = MakeCanvas("fCanvasPositions",3,2);
  fCanvasPositionsWeighted = MakeCanvas("fCanvasPositionsWeighted",2,1);
  fCanvasPositionsWithTrig = MakeCanvas("fCanvasPositionsWithTrig",2,1);
  fCanvasPositionsWeightedWithTrig = MakeCanvas("fCanvasPositionsWeightedWithTrig",2,1);
  fCanvasPositionsInAcceptance = MakeCanvas("fCanvasPositionsInAcceptance",3,1);
  fCanvasPositionsInAcceptanceWeighted = MakeCanvas("fCanvasPositionsInAcceptanceWeighted",2,1);
  //fCanvasAcceptance = MakeCanvas("fCanvasAcceptance",1,1,2.);
  fCanvasPositionDiffs = MakeCanvas("fCanvasPositionDiffs",2,2);
  fCanvasAnglesNPEfficiency = MakeCanvas("fCanvasAnglesNPEfficiency",1,1,2);
  fCanvasAnglesAcceptance = MakeCanvas("fCanvasAnglesAcceptance",3,1,2);
  fCanvasAnglesIntermediate = MakeCanvas("fCanvasAnglesIntermediate",3,1);
  fCanvasIntermediateGraphs = MakeCanvas("fCanvasIntermediateGraphs",3,1);
  fCanvasIdentification = MakeCanvas("fCanvasIdentification",2,2,1.5);
  fCanvasElectronRates = MakeCanvas("fCanvasElectronRates",1,1,1.5);

}

TCanvas* GMnAcceptance::MakeCanvas(const char *name, int cols, int rows,
    double scale)
{
  TCanvas *fCanvas = new TCanvas(TString::Format("%s_v%g",name,fVOff),
      name,cols*kCanvasX*scale,rows*kCanvasY*scale);
  fCanvas->Divide(cols,rows);
  return fCanvas;
}

TH2F* GMnAcceptance::MakeHCalPositionHisto(const char *title, const char* name,
    int hadron)
{
  TH2F *histo = new TH2F(TString::Format("%s%d_v%g",name,hadron,fVOff),
      TString::Format("HCal Position%s%s: %s;x (m);y (m)",(*title==0?"":" "),
        title,kHadronName[hadron]),
      kBins,kMinHCalX,kMaxHCalX,kBins,kMinHCalY,kMaxHCalY);
  histo->SetStats(kFALSE);
  return histo;
}

TH1F* GMnAcceptance::MakeHCalPositionDiffHisto(const char* name, int axis)
{
  TString axisLabel;
  double min,max;
  if(axis == 0) {
    axisLabel = "Horizontal";
    min = kMinPlotHPosDiff;
    max = kMaxPlotHPosDiff;
  } else {
    axisLabel = "Vertical";
    min = kMinPlotVPosDiff;
    max = kMaxPlotVPosDiff;
  }
  TH1F *histo = new TH1F(TString::Format("%s_v%g",name,fVOff),
      TString::Format("HCal P+N %s Position Difference;%s Difference (m)",
        axisLabel.Data(),axisLabel.Data()),kBins,min,max);
  //histo->SetStats(kFALSE);
  return histo;
}


TH2F* GMnAcceptance::MakeBBPositionHisto(const char *title, const char* name)
{
  TH2F *histo = new TH2F(TString::Format("%s_v%g",name,fVOff),
      TString::Format("BBCal Position%s%s;x (m);y (m)",(*title==0?"":" "),
        title),
      kBins,kMinBBX,kMaxBBX,kBins,kMinBBY,kMaxBBY);
  histo->SetStats(kFALSE);
  return histo;
}


TH2F* GMnAcceptance::MakeAnglesHisto(const char *title, const char* name)
{
  TH2F *histo = new TH2F(TString::Format("%s_v%g",name,fVOff),
      TString::Format("%s;#theta_{e} (deg);#phi_{e} (deg)",title),
      kBins,kMinTheta[fKin],kMaxTheta[fKin],kBins,kMinPhi[fKin],kMaxPhi[fKin]);
  histo->SetStats(kFALSE);
  return histo;
}

TH2F* GMnAcceptance::MakeAnglesHisto(const char *title, const char* name, double max)
{
  TH2F *histo = MakeAnglesHisto(title,name);
  histo->SetMaximum(max);
  return histo;
}

TProfile2D* GMnAcceptance::MakeAnglesProfile(const char *title, const char* name)
{
  TProfile2D *profile = new TProfile2D(TString::Format("%s_v%g",name,fVOff),
      TString::Format("%s;#theta_{e} (deg);#phi_{e} (deg)",title),
      kBins,kMinTheta[fKin],kMaxTheta[fKin],kBins,kMinPhi[fKin],kMaxPhi[fKin]);
  profile->SetStats(kFALSE);
  return profile;
}

bool GMnAcceptance::CheckHCalPosition(double x, double y)
{
  bool status = true;
  if( x <= -kHCalWidthAccept/2. || x >= kHCalWidthAccept/2.)
    status = false;
  if( y <= -kHCalHeightAccept/2. || y >= kHCalHeightAccept/2.)
    status = false;
  return status;
}


void GMnAcceptance::ProcessData(GMnData_t data)
{
  fData = data;
  // Fix vertical offset on neutron and proton
  fData.pvars.y -= fVOff/100.; // because data is is m
  fData.nvars.y -= fVOff/100.; // because data is is m

  // For hadrons, throw away any hadrons that hit on the outer module
  // (because it's possible that their shower is not entirely contained
  // within HCal, and will definitely be hard to resolve)
  if(fData.n_det && !CheckHCalPosition(fData.nvars.x,fData.nvars.y)) {
    fData.n_det = false;
  }
  if(fData.p_det && !CheckHCalPosition(fData.pvars.x,fData.pvars.y)) {
    fData.p_det = false;
  }
}

void GMnAcceptance::ProcessFirstPassEntry(int entry, GMnStatus_t status,
    GMnData_t data)
{
  ProcessData(data);

  // Fill the generated angles histogram
  fhGeneratedAngles->Fill(fData.ev.theta,fData.ev.phi);
  double vDiff;
  double hDiff;
  ///////////////////////////////////////////////////////////////////////////
  // Fill the position histograms
  if(fData.e_det) {
    fhEArmPositions->Fill(fData.evars.x,fData.evars.y,FILL_NORM);
  }
  if(fData.n_det) {
    fhHArmPositions[0]->Fill(fData.nvars.x,fData.nvars.y,FILL_NORM);
    fhHArmPositionsWeighted[0]->Fill(fData.nvars.x,fData.nvars.y,fData.ev.nsigma*SIGMA_UNIT);
    // Positions in coincidence (triggered) with electron detector
    if(fData.e_det) {
      fhHArmPositionsWithTrig[0]->Fill(fData.nvars.x,fData.nvars.y,FILL_NORM);
      fhHArmPositionsWeightedWithTrig[0]->Fill(fData.nvars.x,fData.nvars.y,fData.ev.nsigma*SIGMA_UNIT);
    }
  }
  if(fData.p_det) {
    fhHArmPositions[1]->Fill(fData.pvars.x,fData.pvars.y,FILL_NORM);
    fhHArmPositionsWeighted[1]->Fill(fData.pvars.x,fData.pvars.y,fData.ev.psigma*SIGMA_UNIT);
    // Positions in coincidence (triggered) with electron detector
    if(fData.e_det) {
      fhHArmPositionsWithTrig[1]->Fill(fData.pvars.x,fData.pvars.y,FILL_NORM);
      fhHArmPositionsWeightedWithTrig[1]->Fill(fData.pvars.x,fData.pvars.y,fData.ev.psigma*SIGMA_UNIT);
    }
  }

  if(fData.e_det&&fData.n_det&&fData.p_det) {
    fhEArmCoincidencePositions->Fill(fData.evars.x,fData.evars.y,FILL_NORM);
    fhHArmCoincidencePositions[0]->Fill(fData.nvars.x,fData.nvars.y,FILL_NORM);
    fhHArmCoincidencePositions[1]->Fill(fData.pvars.x,fData.pvars.y,FILL_NORM);
  }

  ///////////////////////////////////////////////////////////////////////////
  // Fill the angles and momenta fhistograms
  if(fData.e_det) {
    fhEArmAngles->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    fhAnglesSigmaE->Fill(fData.ev.theta,fData.ev.phi,
        (fData.ev.nsigma+fData.ev.psigma)*SIGMA_UNIT*TMath::Sin(fData.ev.th_rad));

    // Did we see a neutron?
    if(fData.n_det) {
      fhHAngleVPositionN->Fill(fData.ev.theta,fData.ev.phi,fData.nvars.y,FILL_NORM);
      fhHAngleHPositionN->Fill(fData.ev.theta,fData.ev.phi,fData.nvars.x,FILL_NORM);
      fhAnglesEN[0]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    } else {
      fhAnglesEN[1]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    }

    // Did we see a proton?
    if(fData.p_det) {
      fhHAngleVPositionP->Fill(fData.ev.theta,fData.ev.phi,fData.pvars.y,FILL_NORM);
      fhHAngleHPositionP->Fill(fData.ev.theta,fData.ev.phi,fData.pvars.x,FILL_NORM);
      fhAnglesEP[0]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    } else {
      fhAnglesEP[1]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    }

    // Did we see both hadrons?
    if(fData.n_det&&fData.p_det) {
      fhAnglesEH[0]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    }

    // Did we see none of the hadrons?
    if(!fData.n_det&&!fData.p_det) {
      fhAnglesEH[1]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    }
  }

  if(fData.n_det) {
    fhHArmAngles[0]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    fhAnglesSigmaN->Fill(fData.ev.theta,fData.ev.phi,fData.ev.nsigma*SIGMA_UNIT*TMath::Sin(fData.ev.th_rad));
    //fhAnglesSolidAngleN->Fill(fData.ev.theta,fData.ev.phi,TMath::Sin(fData.ev.th_rad));
    //hHArmEDep[0]->Fill(theta,hcal_edep->at(fData.n_det_index),FILL_NORM);
    if(!fData.e_det)
      fhAnglesEN[2]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
  }

  if(fData.p_det) {
    fhHArmAngles[1]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
    fhAnglesSigmaP->Fill(fData.ev.theta,fData.ev.phi,fData.ev.psigma*SIGMA_UNIT*TMath::Sin(fData.ev.th_rad));
    //fhAnglesSolidAngleP->Fill(fData.ev.theta,fData.ev.phi,TMath::Sin(fData.ev.th_rad));
    //hHArmEDep[1]->Fill(theta,hcal_edep->at(fData.p_det_index),FILL_NORM);
    if(!fData.e_det)
      fhAnglesEP[2]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);
  }
  if(fData.n_det&&fData.p_det) {
    fhHArmAngles[2]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);


    if(!fData.e_det)
      fhAnglesEH[2]->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);


    if(fData.e_det) {
      fhBothArmsAngles->Fill(fData.ev.theta,fData.ev.phi,FILL_NORM);

      // Fill the proton-neutron position differences
      vDiff = fData.pvars.y-fData.nvars.y;
      hDiff = fData.pvars.x-fData.nvars.x;
      fhHAngleVPositionDiffs->Fill(fData.ev.theta,fData.ev.phi,vDiff,FILL_NORM);
      fhHAngleHPositionDiffs->Fill(fData.ev.theta,fData.ev.phi,hDiff,FILL_NORM);
      fhHAngleVPositionDiffAvg->Fill(vDiff,FILL_NORM);
      fhHAngleHPositionDiffAvg->Fill(hDiff,FILL_NORM);
    }
  }

}

bool GMnAcceptance::InAcceptance()
{
  if(fhAnglesMap->GetBinContent(fhAnglesMap->FindBin(fData.ev.theta,
          fData.ev.phi)) == 1)
    return true;

  return false;
}

void GMnAcceptance::ProcessSecondPassEntry(int entry, GMnStatus_t status,
    GMnData_t data)
{
  ProcessData(data);

  if(!InAcceptance())
    return;

  int b = fhAnglesMap->FindBin(fData.ev.theta,fData.ev.phi);

  if(fData.e_det) {
    fhEArmPositionsInAcceptance->Fill(fData.evars.x,fData.evars.y,FILL_NORM);
  }
  if(fData.n_det) {
    fAcceptedNeutrons++;
    fhHArmPositionsInAcceptance[0]->Fill(fData.nvars.x,fData.nvars.y,FILL_NORM);
    fhHArmPositionsInAcceptanceWeighted[0]->Fill(fData.nvars.x,
        fData.nvars.y,fData.ev.nsigma*SIGMA_UNIT);
    fhHAngleDetectedN->Fill(fData.ev.theta,fData.ev.phi);
    IdentifyAndFill(0,b,fhHAngleBadIdentificationN,fhHAngleGoodIdentificationN);
  }
  if(fData.p_det) {
    fAcceptedProtons++;
    fhHArmPositionsInAcceptance[1]->Fill(fData.pvars.x,fData.pvars.y,FILL_NORM);
    fhHArmPositionsInAcceptanceWeighted[1]->Fill(fData.pvars.x,
        fData.pvars.y,fData.ev.psigma*SIGMA_UNIT);
    fhHAngleDetectedP->Fill(fData.ev.theta,fData.ev.phi);
    IdentifyAndFill(0,b,fhHAngleBadIdentificationP,fhHAngleGoodIdentificationP);
  }
}

void GMnAcceptance::IdentifyAndFill(int hadron, int bin,
    std::vector<TH2F*> &bad, std::vector<TH2F*> &good)
{
  if(hadron < 0 || hadron > 1)
    return;

  // Define the half way line between protons and neutrons for this bin
  double half = 0.5*(fhHAngleVPositionP->GetBinContent(bin)+
      fhHAngleVPositionN->GetBinContent(bin));

  // Average Position of the hadrons for this bin
  double xP = fhHAngleHPositionP->GetBinContent(bin);
  double yP = fhHAngleVPositionP->GetBinContent(bin);
  double xN = fhHAngleHPositionN->GetBinContent(bin);
  double yN = fhHAngleVPositionN->GetBinContent(bin);

  // Circle defining acceptance of the other particle
  double x;
  double y;
  double xbad;
  double ybad;
  double xgood;
  double ygood;
  bool crossed = false;
  if(hadron == 0) { // Neutron
    x     = fData.nvars.x;
    y     = fData.nvars.y;
    // Mis-identified if inside proton region of acceptance and
    // above the horizontal line
    crossed = (y >= half);
    // Protons average position
    xbad  = xP;
    ybad  = yP;
    // Neutron's identification circle
    xgood  = xN;
    ygood  = yN;
  } else {
    x     = fData.pvars.x;
    y     = fData.pvars.y;
    // Mis-identified if inside neutron region of acceptance and
    // below the horizontal line
    crossed = (y <= half);
    // Neutron's average position
    xbad  = xN;
    ybad  = yN;
    // Proton's average position
    xgood  = xP;
    ygood  = yP;
  }
  bool insideGood = InsideThetaPQ(xgood,ygood,x,y);
  bool insideBad = InsideThetaPQ(xbad,ybad,x,y);

  double exclusionLow = 0;
  double exclusionHigh = 0;
  double res = 0;
  for(size_t i = 0; i < fPositionExclusionZone.size(); i++) {
    res = 0;
    // Check if the particle is in the exclusion zone
    //exclusionLow  = half - fPositionExclusionZone[i]/2.;
    //exclusionHigh = half + fPositionExclusionZone[i]/2.;
    exclusionLow  = yN + (fThetaPQRadius-fPositionExclusionZone[i]);
    exclusionHigh = yP - (fThetaPQRadius-fPositionExclusionZone[i]);
    // Only deal with particles that are outside the exclusion zone
    // which is now some fraction of the fThetaPQRadius
    if(y<exclusionLow || y>exclusionHigh) {
      // If the particle crossed the half-way mark, check if it is
      // mis-identified
      if(crossed&&insideBad) {
        bad[i]->AddBinContent(bin);
      } else if (!crossed&&insideGood) {
        good[i]->AddBinContent(bin);
      }
    }
  }
}


bool GMnAcceptance::InsideThetaPQ(double ox, double oy, double vx, double vy)
{
  return (fThetaPQRadius>=TMath::Sqrt(TMath::Power(ox-vx,2)+
        TMath::Power(oy-vy,2)));
}

void GMnAcceptance::SetGenData(gen_t gen)
{
 fGenData = gen;
 fThetaPQRadius = fGenData.dhcal*kThetaPQCut[fKin]*TMath::Pi()/180.;
 //std::cout << "dhcal: " << fGenData.dhcal << " m " << std::endl;
 //std::cout << "ThetaPQ Radius: " << fThetaPQRadius << " m" << std::endl;
 Init();
}

void GMnAcceptance::DoAnglesRatio()
{
  // Normalize to generated angles
  fhAnglesRatioNG->Divide(fhHArmAngles[0],fhGeneratedAngles);
  fhAnglesRatioPG->Divide(fhHArmAngles[1],fhGeneratedAngles);
  fhAnglesRatioHG->Divide(fhHArmAngles[2],fhGeneratedAngles);
  fhAnglesRatioEG->Divide(fhEArmAngles,fhGeneratedAngles);
  fhAnglesRatioAG->Divide(fhBothArmsAngles,fhGeneratedAngles);
  fhAnglesRatioNG->SetMaximum(1.0);
  fhAnglesRatioPG->SetMaximum(1.0);
  fhAnglesRatioHG->SetMaximum(1.0);
  fhAnglesRatioEG->SetMaximum(1.0);
  fhAnglesRatioAG->SetMaximum(1.0);
  int nbx = fhAnglesNPSum->GetNbinsX();
  int nby = fhAnglesNPSum->GetNbinsY();
  int b; // current global bin number
  double bin_th;
  double bin_ph;

  /*
  // Just as a test for my SBS talk, apply the electron efficiency cut
  // and clean to the angles fraction histograms
  for(int bx = 1; bx <= nbx; bx++) {
  for(int by = 1; by <= nby; by++) {
  b = bx+(nbx+2)*by;
  bin_th = fhAnglesRatioEG->GetXaxis()->GetBinLowEdge(bx);
  bin_ph = fhAnglesRatioEG->GetYaxis()->GetBinLowEdge(by);
  if(fhAnglesRatioEG->GetBinContent(bx,by) < kMinElectronStats ||
  bin_ph < bin_th*kLineCutSlope[fKin]+kLineCutIntercept[fKin] ) {
  fhAnglesRatioNG->SetBinContent(bx,by,0.0);
  fhAnglesRatioPG->SetBinContent(bx,by,0.0);
  fhAnglesRatioHG->SetBinContent(bx,by,0.0);
  fhAnglesRatioEG->SetBinContent(bx,by,0.0);
  }
  }
  }
  */


  for(int i = 0; i < 3; i++) {
    fhAnglesENRG[i]->Divide(fhAnglesEN[i],fhGeneratedAngles);
    fhAnglesEPRG[i]->Divide(fhAnglesEP[i],fhGeneratedAngles);
    fhAnglesEHRG[i]->Divide(fhAnglesEH[i],fhGeneratedAngles);
  }

  // Produce the clean electron singles histogram
  fhAnglesRatioEGClean = new TH2F(*fhAnglesRatioEG);
  //fhAnglesRatioEGClean = fhAnglesRatioEG;
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      b = bx+(nbx+2)*by;
      bin_th = fhAnglesRatioEGClean->GetXaxis()->GetBinLowEdge(bx);
      bin_ph = fhAnglesRatioEGClean->GetYaxis()->GetBinLowEdge(by);
      if(fhAnglesRatioEGClean->GetBinContent(bx,by) < kMinElectronStats ||
          bin_ph < bin_th*kLineCutSlope[fKin]+kLineCutIntercept[fKin] ) {
        fhAnglesRatioEGClean->SetBinContent(bx,by,0.0);
        fhAnglesRatioEGClean->SetBinError(bx,by,1e6);
      }
    }
  }

  fhAnglesENRG[3]->Divide(fhAnglesENRG[0],fhAnglesRatioEGClean);
  fhAnglesEPRG[3]->Divide(fhAnglesEPRG[0],fhAnglesRatioEGClean);
  fhAnglesEHRG[3]->Divide(fhAnglesEHRG[0],fhAnglesRatioEGClean);

  //fhAnglesEN[3]->Sumw2();
  //fhAnglesEP[3]->Sumw2();
  //fhAnglesEH[3]->Sumw2();
  // Now determine the percent difference between neutron and proton
  // detection efficiencies
  //fhAnglesNPSum->Add(fhAnglesEN[3],fhAnglesEP[3],0.5,0.5);
  //fhAnglesNPDiff->Add(fhAnglesEN[3],fhAnglesEP[3],1.,-1.);
  //fhAnglesNPPercentDiff->Divide(fhAnglesNPDiff,fhAnglesNPDiff,1.,1.0);
}

GMnResultsSigmaSol_t GMnAcceptance::ComputeAcceptance(TH2F *nEff, TH2F *pEff)
{
  GMnResultsSigmaSol_t result;
  result.sigmaP = 0;
  result.sigmaN = 0;
  result.solidN = 0;
  result.solidP = 0;
  // Get number of bins in map
  int nbx = fhAnglesMap->GetNbinsX();
  int nby = fhAnglesMap->GetNbinsY();
  int b = -1; // bin corresponding to (bx,by)

  // Variables to hold temporary bin information
  double ne = 1.0; // Neutron bin-efficiency
  double pe = 1.0; // Proton  bin-efficiency
  double dOmega; // bin solid angle
  double sinTheta;  // sin(theta) of bin
  // Now loop step through all the valid bins in the map and use them to
  // compute the integrated cross sections and solid angles.
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      // Check this bin is in the acceptance map
      b = fhAnglesMap->GetBin(bx,by);
      if(fhAnglesMap->GetBinContent(b)) {
        // Compute bin angle data (remember, these are in degrees! must convert
        // to srad)
        dOmega = fhAnglesSigmaN->GetXaxis()->GetBinWidth(bx)*
          fhAnglesSigmaN->GetYaxis()->GetBinWidth(by);
        sinTheta = TMath::Sin(fhAnglesSigmaN->GetXaxis()->GetBinCenter(bx)
            *TMath::Pi()/180.0);
        ne = fhAnglesENRG[3]->GetBinContent(b);
        pe = fhAnglesEPRG[3]->GetBinContent(b);
        if(nEff)
          ne *= nEff->GetBinContent(b);
        if(pEff)
          pe *= pEff->GetBinContent(b);

        // Compute the integrated cross section with efficiency
        result.sigmaN += fhAnglesSigmaN->GetBinContent(b)*dOmega*ne;
        result.sigmaP += fhAnglesSigmaP->GetBinContent(b)*dOmega*pe;
        //result.sigmaNE += TMath::Power(fhAnglesSigmaN->GetBinError(b),2)*dOmega;
        //result.sigmaPE += TMath::Power(fhAnglesSigmaP->GetBinError(b),2)*dOmega;

        // Add this bin to the solid angle computation with efficiency
        result.solidN += dOmega*sinTheta*ne;
        result.solidP += dOmega*sinTheta*pe;
      }
    }
  }
  // Finalize Errors
  //result.sigmaNE = TMath::Sqrt(result.sigmaNE);
  //result.sigmaPE = TMath::Sqrt(result.sigmaPE);

  // Convert results for solid angle from degres back to mili-radians
  result.solidN *= TMath::Power(TMath::Pi()/180.0,2)*SOLID_UNIT;
  result.solidP *= TMath::Power(TMath::Pi()/180.0,2)*SOLID_UNIT;
  //result.solidEN *= TMath::Power(TMath::Pi()/180.0,2);
  //result.solidEP *= TMath::Power(TMath::Pi()/180.0,2);
  result.sigmaN *= TMath::Power(TMath::Pi()/180.0,2);
  result.sigmaP *= TMath::Power(TMath::Pi()/180.0,2);
  //fSigmaSumErrN *= TMath::Power(TMath::Pi()/180.0,2);
  //fSigmaSumErrP *= TMath::Power(TMath::Pi()/180.0,2);

  return result;
}

void GMnAcceptance::CleanAcceptanceMap()
{
  int nbx = fhAnglesMap->GetNbinsX();
  int nby = fhAnglesMap->GetNbinsY();
  int b = -1; // bin corresponding to (bx,by)
  TH2F *map = new TH2F(*fhAnglesMap); // Make a copy of the map
  int count[4];
  int sum;
  for(int bx = 2; bx <= nbx; bx++) {
    for(int by = 2; by <= nby; by++) {
      // Determine the global bin corresponding to (bx,by)
      b = map->GetBin(bx,by);
      sum = map->GetBinContent(bx+1,by) + map->GetBinContent(bx-1,by) +
        map->GetBinContent(bx,by+1) + map->GetBinContent(bx,by-1);
      if( map->GetBinContent(bx,by) && sum == 0 ) {
        // Found a single lonely bin. Lets exclude this one too.
        //std::cout << "Clearing bin = (" << bx << ", " << by << ")" << std::endl;
        // Clear previously set content
        fhAnglesMap->SetBinContent(b,0.0);
      } else if ( !map->GetBinContent(bx,by) && sum == 4) {
        // Somehow this bin is empty, but its neighbors are not. Better
        // include it.
        fhAnglesMap->SetBinContent(b,1.0);
      }
    }
  }
  delete map;
}

void GMnAcceptance::ComputeElectronTriggerRate()
{
  // Quickly integrate electron
  int nbx = fhEArmAngles->GetNbinsX();
  int nby = fhEArmAngles->GetNbinsY();
  double dOmega;
  double ee;
  double sinTheta;
  double sigmaE = 0.0;
  double solidE = 0.0;
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      dOmega = fhEArmAngles->GetXaxis()->GetBinWidth(bx)*
        fhEArmAngles->GetYaxis()->GetBinWidth(by);
      sinTheta = TMath::Sin(fhEArmAngles->GetXaxis()->GetBinCenter(bx)
          *TMath::Pi()/180.0);
      ee = fhAnglesRatioEGClean->GetBinContent(bx,by);
      sigmaE += fhAnglesSigmaE->GetBinContent(bx,by)*dOmega*ee;
      solidE += dOmega*sinTheta*ee;
    }
  }
  sigmaE *= TMath::Power(TMath::Pi()/180.0,2);
  solidE *= TMath::Power(TMath::Pi()/180.0,2)*1e3;
  std::cout << "SigmaE:  " << sigmaE  << " pb"  << std::endl;
  std::cout << "SolidE:  " << solidE  << " msr" << std::endl;
}

void GMnAcceptance::DetermineAcceptanceMap()
{
  int nbx = fhAnglesMap->GetNbinsX();
  int nby = fhAnglesMap->GetNbinsY();

  double cn; // neutron count
  double cp; // proton count
  double diff; // difference
  double avg; // average
  double pdiff; // percent difference
  int b = -1; // bin corresponding to (bx,by)
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      // Determine the global bin corresponding to (bx,by)
      b = fhAnglesENRG[3]->GetBin(bx,by);

      // Get the counts (actually a ratio...)
      cn = fhAnglesENRG[3]->GetBinContent(b);
      cp = fhAnglesEPRG[3]->GetBinContent(b);

      // Compute differences between ratios
      diff = cn-cp;
      avg = (cn+cp)/2.;
      if(avg!=0)
        pdiff = diff/avg;
      else
        pdiff = kStdErrorValue;

      // Fill the histograms with these differences.
      // We'll clean this up later.
      fhAnglesNPSum->SetBinContent(b,avg);
      fhAnglesNPDiff->SetBinContent(b,diff);
      fhAnglesNPPercentDiff->SetBinContent(b,pdiff);
      fhAnglesNPPercentDiffBad->SetBinContent(b,pdiff);

      // At first pass, a point falls in the acceptance if the following
      // conditions are met
      fhAnglesMap->SetBinContent(b, (cn > 0 && cp > 0 &&
            TMath::Abs(pdiff) <= kEfficiencyDiffTolerance ));
    }
  }

  // At this point, we compute the electron trigger rates
  ComputeElectronTriggerRate();

  // At this point, we loop over the map once more and clean it up as
  // necessary.
  if( kCleanAcceptanceMap ) {
    CleanAcceptanceMap();
  }

  // Lastly now that we have an acceptance map, let's clean up the other
  // histograms so that we can calculate integrated cross sections and
  // solid angles.
  for(int bx = 1; bx <= nbx; bx++) {
    for(int by = 1; by <= nby; by++) {
      // Determine the global bin corresponding to (bx,by)
      b = fhAnglesENRG[3]->GetBin(bx,by);
      // If this bin is not in the acceptance, clean up other histograms
      if(!fhAnglesMap->GetBinContent(b)) {
        fhAnglesNPSum->SetBinContent(b,kStdErrorValue);
        fhAnglesNPDiff->SetBinContent(b,kStdErrorValue);
        fhAnglesNPPercentDiff->SetBinContent(b,kStdErrorValue);
        //fhAnglesNPPercentDiff->SetBinEntries(b,0);

        fhAnglesSigmaN->SetBinContent(b,0);
        fhAnglesSigmaP->SetBinContent(b,0);
        fhAnglesSigmaN->SetBinEntries(b,0);
        fhAnglesSigmaP->SetBinEntries(b,0);
      } else {
        fhAnglesNPPercentDiffBad->SetBinContent(b,kStdErrorValue);
      }
    }
  }

  fhAnglesNPSum->SetMinimum(-kEfficiencyDiffPlotRange);
  fhAnglesNPDiff->SetMinimum(-kEfficiencyDiffPlotRange);
  fhAnglesNPPercentDiff->SetMinimum(-kEfficiencyDiffPlotRange);
  fhAnglesNPPercentDiffBad->SetMinimum(-kEfficiencyDiffPlotRange*3);
  fhAnglesNPSum->SetMaximum(kEfficiencyDiffPlotRange);
  fhAnglesNPDiff->SetMaximum(kEfficiencyDiffPlotRange);
  fhAnglesNPPercentDiff->SetMaximum(kEfficiencyDiffPlotRange);
  fhAnglesNPPercentDiffBad->SetMaximum(kEfficiencyDiffPlotRange*3);

  //fhAnglesSigmaN->Multiply(fhAnglesMap);
  //fhAnglesSigmaP->Multiply(fhAnglesMap);
  //fSigmaSumN = fhAnglesSigmaN->IntegralAndError(1,nbx,1,nby,fSigmaSumErrN,"width");
  //fSigmaSumP = fhAnglesSigmaP->IntegralAndError(1,nbx,1,nby,fSigmaSumErrP,"width");
  //fSolidAngleN = fhAnglesSolidAngleN->IntegralAndError(1,nbx,1,nby,fSolidAngleErrN,"width");
  //fSolidAngleP = fhAnglesSolidAngleP->IntegralAndError(1,nbx,1,nby,fSolidAngleErrP,"width");

  // Convert solid angle back to radians
  //fSolidAngleN *= TMath::Power(TMath::Pi()/180.0,2);
  //fSolidAngleP *= TMath::Power(TMath::Pi()/180.0,2);
  //fSolidAngleErrN *= TMath::Power(TMath::Pi()/180.0,2);
  //fSolidAngleErrP *= TMath::Power(TMath::Pi()/180.0,2);
  //fSigmaSumN *= TMath::Power(TMath::Pi()/180.0,2);
  //fSigmaSumP *= TMath::Power(TMath::Pi()/180.0,2);
  //fSigmaSumErrN *= TMath::Power(TMath::Pi()/180.0,2);
  //fSigmaSumErrP *= TMath::Power(TMath::Pi()/180.0,2);
  //fSolidAngleNVec.push_back(fSolidAngleN);
  //fSolidAnglePVec.push_back(fSolidAngleP);
  //fSigmaSumNVec.push_back(fSigmaSumN);
  //fSigmaSumPVec.push_back(fSigmaSumP);
  //fIntermediateEntriesCount.push_back(current_entry);
  //fhAnglesMap->SetFillColor(kMagenta+1);
  //fSolidAngleBinCountVec.push_back(fSolidAngleBinCount);
}

void GMnAcceptance::FinalizeFirstPass()
{
  fResults.pnvsep = fhHAngleVPositionDiffAvg->GetMean();
  fHCalAvgHPositionDiff  = fhHAngleHPositionDiffAvg->GetMean();
  fHCalAvgHPositionDiffE = fhHAngleHPositionDiffAvg->GetRMS()/TMath::Sqrt(
      fhHAngleHPositionDiffAvg->GetEntries());


  DoAnglesRatio();
  DetermineAcceptanceMap();
  fResults.sigmaSol = ComputeAcceptance();

  // Draw scattering angle acceptance
  fCanvasElectronRates->cd(0);
  fhAnglesSigmaE->Draw(kHist2DDrawOption);
  SaveCanvas(fCanvasElectronRates,"electron_rates");

  // Draw scattering angle acceptance
  DrawAngles(fCanvasAngles);
  SaveCanvas(fCanvasAngles,"angles");

  // Draw scattering angle acceptance
  DrawAnglesDetected(fCanvasAnglesDetected);
  SaveCanvas(fCanvasAnglesDetected,"angles_detected");

  // Draw ratios of angles
  DrawAnglesRatio(fCanvasAnglesRatio);
  SaveCanvas(fCanvasAnglesRatio,"angles_ratio");

  // Draw position acceptances
  gGMnColors->SelectPaletteSequential();
  DrawPositions(fCanvasPositions);
  SaveCanvas(fCanvasPositions,"positions");
  DrawPositionsWeighted(fCanvasPositionsWeighted);
  SaveCanvas(fCanvasPositionsWeighted,"positions_weighted");
  DrawPositionsWithTrig(fCanvasPositionsWithTrig,
      fCanvasPositionsWeightedWithTrig);
  SaveCanvas(fCanvasPositionsWithTrig,"positions_with_trig");
  SaveCanvas(fCanvasPositionsWeightedWithTrig,"positions_weighted_with_trig");
  gGMnColors->SelectPaletteDiverging();

  // Draw position differences
  fCanvasPositionDiffs->cd(1);
  fhHAngleVPositionDiffs->SetMinimum(0);
  fhHAngleVPositionDiffs->Draw(kHist2DDrawOption);
  fCanvasPositionDiffs->cd(2);
  fhHAngleHPositionDiffs->Draw(kHist2DDrawOption);
  fCanvasPositionDiffs->cd(3);
  gPad->SetLogy();
  fhHAngleVPositionDiffAvg->Draw(kHist1DDrawOption);
  FixStatBox(fhHAngleVPositionDiffAvg);
  fCanvasPositionDiffs->cd(4);
  fhHAngleHPositionDiffAvg->Draw(kHist1DDrawOption);
  FixStatBox(fhHAngleHPositionDiffAvg);
  SaveCanvas(fCanvasPositionDiffs,"proton_neutron_position_diffs");
}

void GMnAcceptance::FinalizeSecondPass()
{
  // Draw position acceptances
  gGMnColors->SelectPaletteSequential();
  DrawPositionsInAcceptance(fCanvasPositionsInAcceptance);
  SaveCanvas(fCanvasPositionsInAcceptance,"positions_in_acceptance");
  DrawPositionsInAcceptanceWeighted(fCanvasPositionsInAcceptanceWeighted);
  SaveCanvas(fCanvasPositionsInAcceptanceWeighted,"positions_weighted_in_acceptance");
  gGMnColors->SelectPaletteDiverging();


  fCanvasAnglesNPEfficiency->cd(0);
  gPad->SetGrid(kTRUE,kTRUE);
  fhAnglesNPPercentDiff->Draw(kHist2DDrawOption);
  SaveCanvas(fCanvasAnglesNPEfficiency,"neutron_proton_efficiency");
  SaveCanvas(fCanvasAnglesNPEfficiency,"neutron_proton_efficiency","C");
  fhAnglesNPPercentDiffBad->Draw(kHist2DDrawOption);
  SaveCanvas(fCanvasAnglesNPEfficiency,"neutron_proton_efficiency_bad");
  SaveCanvas(fCanvasAnglesNPEfficiency,"neutron_proton_efficiency_bad","C");

  fCanvasAnglesAcceptance->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhAnglesMap->SetFillColor(kMagenta+1);
  fhAnglesMap->Draw(kHistMapDrawOption);
  fCanvasAnglesAcceptance->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhAnglesSigmaN->Draw(kHist2DDrawOption);
  fCanvasAnglesAcceptance->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  fhAnglesSigmaP->Draw(kHist2DDrawOption);
  SaveCanvas(fCanvasAnglesAcceptance,"acceptance");

  fCanvasAnglesNPEfficiency->cd(0);
  gPad->SetGrid(kTRUE,kTRUE);
  fhEArmAngles->Draw(kHist2DDrawOption);
  //fCanvasAnglesNPEfficiency->SaveAs(Form("results/histo%02d.C",fKin));
  SaveCanvas(fCanvasAnglesNPEfficiency,"efficiency_histogram","C");

  // Finalize Identified hadron results
  fResults.thetaPQRadius = fThetaPQRadius;
  for(size_t i = 0; i < fPositionExclusionZone.size(); i++) {
    // Normalize the Identified hadron histograms
    fhHAngleGoodIdentificationN[i]->Divide(fhHAngleDetectedN);
    fhHAngleGoodIdentificationP[i]->Divide(fhHAngleDetectedP);
    fhHAngleBadIdentificationN[i]->Divide(fhHAngleDetectedN);
    fhHAngleBadIdentificationP[i]->Divide(fhHAngleDetectedP);
    fResults.goodIdentify.push_back(ComputeAcceptance(
          fhHAngleGoodIdentificationN[i], fhHAngleGoodIdentificationP[i]));
    fResults.badIdentify.push_back(ComputeAcceptance(
          fhHAngleBadIdentificationN[i], fhHAngleBadIdentificationP[i]));
    fResults.exclusionZoneWidth.push_back(fPositionExclusionZone[i]);

    // Now draw and save them
    fCanvasIdentification->cd(1);
    gPad->SetGrid(kTRUE,kTRUE);
    fhHAngleBadIdentificationN[i]->Draw(kHist2DDrawOption);
    //fhHAngleVPositionN->Draw(kHist2DDrawOption);
    fCanvasIdentification->cd(2);
    gPad->SetGrid(kTRUE,kTRUE);
    fhHAngleBadIdentificationP[i]->Draw(kHist2DDrawOption);
    //fhHAngleVPositionP->Draw(kHist2DDrawOption);

    fCanvasIdentification->cd(3);
    gPad->SetGrid(kTRUE,kTRUE);
    fhHAngleGoodIdentificationN[i]->Draw(kHist2DDrawOption);
    fCanvasIdentification->cd(4);
    gPad->SetGrid(kTRUE,kTRUE);
    fhHAngleGoodIdentificationP[i]->Draw(kHist2DDrawOption);
    //fhHAngleVPositionP->Draw(kHist2DDrawOption);
    SaveCanvas(fCanvasIdentification,TString::Format("identification_%gcm",
          fPositionExclusionZone[i]*100.));
    SaveCanvas(fCanvasIdentification,TString::Format("identification_%gcm",
          fPositionExclusionZone[i]*100.),"C");
  }
}

void GMnAcceptance::FixStatBox(TH1F* hist, TVirtualPad *pad)
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

void GMnAcceptance::SaveCanvas(TCanvas *canvas, const char *comment,
    const char *ext, bool inSubdir)
{
  if(!kSaveCanvas)
    return;

  // Silence any debug/info statements
  int level = gErrorIgnoreLevel;
  gErrorIgnoreLevel = kVerbosity;

  // Get the path
  TString path = GetSavePath(inSubdir);

  canvas->SaveAs(Form("%s/gmn_kin%02d_v%g_%s.%s",
        path.Data(),fKin,fVOff,comment,ext));

  // Return verbosity to previous level
  gErrorIgnoreLevel = level;
}

void GMnAcceptance::DrawPositionsInAcceptanceWeighted(TCanvas *canvas)
{
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsInAcceptanceWeighted[0]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsInAcceptanceWeighted[1]->Draw(kHist2DDrawOption);
  DrawHCalBox();
}


void GMnAcceptance::DrawPositionsWeighted(TCanvas *canvas)
{
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsWeighted[0]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsWeighted[1]->Draw(kHist2DDrawOption);
  DrawHCalBox();
}

void GMnAcceptance::DrawPositionsWithTrig(TCanvas *c, TCanvas *cTrig)
{
  c->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsWithTrig[0]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  c->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsWithTrig[1]->Draw(kHist2DDrawOption);
  DrawHCalBox();

  cTrig->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsWeightedWithTrig[0]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  cTrig->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsWeightedWithTrig[1]->Draw(kHist2DDrawOption);
  DrawHCalBox();
}


void GMnAcceptance::DrawPositionsInAcceptance(TCanvas *canvas)
{
  // Draw the singles allowed positions
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsInAcceptance[0]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositionsInAcceptance[1]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  fhEArmPositionsInAcceptance->Draw(kHist2DDrawOption);
  DrawBBBox();
}

void GMnAcceptance::DrawPositions(TCanvas *canvas)
{
  // Draw the singles allowed positions
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositions[0]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmPositions[1]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  fhEArmPositions->Draw(kHist2DDrawOption);
  DrawBBBox();

  // Draw the 3-particle coincidence positions
  canvas->cd(4);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmCoincidencePositions[0]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  canvas->cd(5);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmCoincidencePositions[1]->Draw(kHist2DDrawOption);
  DrawHCalBox();
  canvas->cd(6);
  gPad->SetGrid(kTRUE,kTRUE);
  fhEArmCoincidencePositions->Draw(kHist2DDrawOption);
  DrawBBBox();
}

void GMnAcceptance::DrawHCalBox()
{
  TBox *box = new TBox(-kHCalWidth/2.,-kHCalHeight/2.,
      kHCalWidth/2.,kHCalHeight/2.);
  box->SetFillStyle(0);
  box->SetLineColor(kGreen+1);
  box->SetLineWidth(2);
  box->Draw();

  TBox *boxAccept = new TBox(-kHCalWidthAccept/2.,-kHCalHeightAccept/2.,
      kHCalWidthAccept/2.,kHCalHeightAccept/2.);
  boxAccept->SetFillStyle(0);
  boxAccept->SetLineColor(kGreen+1);
  boxAccept->SetLineWidth(1);
  boxAccept->Draw();

}

void GMnAcceptance::DrawBBBox()
{
  TBox *box = new TBox(-kBBWidth/2.,-kBBHeight/2.,kBBWidth/2.,kBBHeight/2.);
  box->SetFillStyle(0);
  box->SetLineColor(kBlue);
  box->SetLineWidth(2);
  box->Draw();
}

void GMnAcceptance::DrawAngles(TCanvas *canvas)
{
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmAngles[0]->Draw(kHist2DDrawOption);
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmAngles[1]->Draw(kHist2DDrawOption);
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  fhHArmAngles[2]->Draw(kHist2DDrawOption);
  canvas->cd(4);
  gPad->SetGrid(kTRUE,kTRUE);
  fhEArmAngles->Draw(kHist2DDrawOption);
  canvas->cd(5);
  gPad->SetGrid(kTRUE,kTRUE);
  fhGeneratedAngles->Draw(kHist2DDrawOption);
  canvas->cd(6);
  gPad->SetGrid(kTRUE,kTRUE);
  fhBothArmsAngles->Draw(kHist2DDrawOption);
}

void GMnAcceptance::DrawAnglesDetected(TCanvas *canvas)
{
  for(int i = 0; i < 4; i++) {
    canvas->cd(1+i);
    gPad->SetGrid(kTRUE,kTRUE);
    fhAnglesENRG[i]->SetMinimum(0.0);
    fhAnglesENRG[i]->SetMaximum(0.0);
    fhAnglesENRG[i]->Draw(kHist2DDrawOption);
    canvas->cd(5+i);
    gPad->SetGrid(kTRUE,kTRUE);
    fhAnglesEPRG[i]->SetMinimum(0.0);
    fhAnglesEPRG[i]->SetMaximum(0.0);
    fhAnglesEPRG[i]->Draw(kHist2DDrawOption);
    canvas->cd(9+i);
    gPad->SetGrid(kTRUE,kTRUE);
    fhAnglesEHRG[i]->SetMinimum(0.0);
    fhAnglesEHRG[i]->SetMaximum(0.0);
    fhAnglesEHRG[i]->Draw(kHist2DDrawOption);
  }
}

void GMnAcceptance::DrawAnglesRatio(TCanvas *canvas)
{
  canvas->cd(1);
  gPad->SetGrid(kTRUE,kTRUE);
  fhAnglesRatioNG->Draw(kHist2DDrawOption);
  canvas->cd(2);
  gPad->SetGrid(kTRUE,kTRUE);
  fhAnglesRatioPG->Draw(kHist2DDrawOption);
  canvas->cd(3);
  gPad->SetGrid(kTRUE,kTRUE);
  fhAnglesRatioHG->Draw(kHist2DDrawOption);
  canvas->cd(4);
  gPad->SetGrid(kTRUE,kTRUE);
  fhAnglesRatioEG->Draw(kHist2DDrawOption);
}
