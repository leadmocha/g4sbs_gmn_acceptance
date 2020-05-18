#ifndef GMNACCEPTANCE_H
#define GMNACCEPTANCE_H

#include "GMnGlobal.h"

#include <TCanvas.h>
#include <TH2F.h>
#include <TProfile2D.h>
#include <TGraph.h>
#include <Fit/Fitter.h>
#include <Math/Functor.h>

class GMnAcceptance {
public:
  GMnAcceptance(int kin, double voff, TString resultsPath);
  virtual ~GMnAcceptance();

  void FinalizeFirstPass();
  void FinalizeSecondPass();
  void ProcessFirstPassEntry(int entry, GMnStatus_t status, GMnData_t data);
  void ProcessSecondPassEntry(int entry, GMnStatus_t status, GMnData_t data);
  bool CheckHCalPosition(double x, double y);
  GMnResults_t GetResults() { return fResults; };
  void SetGenData(gen_t gen);

private:
  double fThetaPQRadius;
  int fKin;
  double fVOff;
  TString fResultsPath;

  double fAcceptedNeutrons;
  double fAcceptedProtons;
  double fHCalAvgVPositionDiff;
  double fHCalAvgVPositionDiffE;
  double fHCalAvgHPositionDiff;
  double fHCalAvgHPositionDiffE;

  GMnData_t fData;
  gen_t     fGenData;
  GMnStatus_t fStatus;

  TCanvas *fCanvasAngles;
  TCanvas *fCanvasAnglesDetected;
  TCanvas *fCanvasAnglesRatio;
  TCanvas *fCanvasPositions;
  TCanvas *fCanvasPositionsWeighted;
  TCanvas *fCanvasPositionsWithTrig;
  TCanvas *fCanvasPositionsWeightedWithTrig;
  TCanvas *fCanvasPositionsInAcceptance;
  TCanvas *fCanvasPositionsInAcceptanceWeighted;
  //TCanvas *fCanvasAcceptance;
  TCanvas *fCanvasPositionDiffs;
  TCanvas *fCanvasAnglesNPEfficiency;
  TCanvas *fCanvasAnglesAcceptance;
  TCanvas *fCanvasAnglesIntermediate;
  TCanvas *fCanvasIntermediateGraphs;
  TCanvas *fCanvasIdentification;
  TCanvas *fCanvasElectronRates;

  std::vector<TCanvas*> fCanvasGeneric;

  // Internal functions
  TCanvas* MakeCanvas(const char *name, int cols, int rows, double scale = 1);
  void Init();
  TProfile2D* MakeAnglesProfile(const char *title, const char* name);
  TH2F* MakeAnglesHisto(const char *title, const char* name, double max);
  TH2F* MakeAnglesHisto(const char *title, const char* name);
  TH2F* MakeHCalPositionHisto(const char *title, const char* name, int hadron);
  TH1F* MakeHCalPositionDiffHisto(const char* name, int axis);
  TH2F* MakeBBPositionHisto(const char *title, const char* name);
  void DoAnglesRatio();
  void DetermineAcceptanceMap();
  void ComputeElectronTriggerRate();
  void CleanAcceptanceMap();
  GMnResultsSigmaSol_t ComputeAcceptance(TH2F *nEff = 0, TH2F *pEff = 0);
  bool InAcceptance();
  void ProcessData(GMnData_t data);
  void SaveCanvas(TCanvas *canvas, const char *comment, const char *ext = "png",
    bool inSubdir = false);
  void IdentifyAndFill(int hadron, int bin, std::vector<TH2F*> &good,
      std::vector<TH2F*> &bad);
  TString GetSavePath(bool inSubdir = false) {
    if(inSubdir)
      return fResultsPath + TString("/extended");

    return fResultsPath;
  }
  void FixStatBox(TH1F* hist, TVirtualPad *pad = 0);
  void DrawPositions(TCanvas *canvas);
  void DrawPositionsWeighted(TCanvas *canvas);
  void DrawPositionsWithTrig(TCanvas *canvasPos, TCanvas *canvasPosWeighted);
  void DrawPositionsInAcceptance(TCanvas *canvas);
  void DrawPositionsInAcceptanceWeighted(TCanvas *canvas);
  void DrawHCalBox();
  void DrawBBBox();
  void DrawAngles(TCanvas *canvas);
  void DrawAnglesRatio(TCanvas *canvas);
  void DrawAnglesDetected(TCanvas *canvas);
  bool InsideThetaPQ(double ox, double oy, double vx, double vy);
  void SaveAcceptanceMap(TString filename);
  TCanvas* DrawPositionsGenericH(TH2F *hN, TH2F *hP);

// Histograms:
private:

  // Histograms
  // Angle acceptance
  TH2F *fhHArmAngles[3];
  TH2F *fhEArmAngles;
  TH2F *fhBothArmsAngles;
  TH2F *fhGeneratedAngles;
  TH2F *fhNeutronAngleAcceptance;
  TH2F *fhProtonAngleAcceptance;
  TH2F *fhNeutronProtonAngleAcceptance;
  TH2F *fhAnglesEN[4];
  TH2F *fhAnglesEP[4];
  TH2F *fhAnglesEH[4];
  TH2F *fhAnglesENRG[4];
  TH2F *fhAnglesEPRG[4];
  TH2F *fhAnglesEHRG[4];
  TH2F *fhAnglesNPSum;
  TH2F *fhAnglesNPDiff;
  TH2F *fhAnglesNPPercentDiff;
  TH2F *fhAnglesNPPercentDiffBad;
  TH2F *fhAnglesMap;
  TProfile2D *fhAnglesSigmaN;
  TProfile2D *fhAnglesSigmaP;
  TProfile2D *fhAnglesSigmaE;
  //TProfile2D *fhAnglesSolidAngleN;
  //TProfile2D *fhAnglesSolidAngleP;
  double fSigmaSumP;
  double fSigmaSumN;
  double fSigmaSumErrP;
  double fSigmaSumErrN;
  double fSolidAngleN;
  int fSolidAngleBinCount;
  std::vector<double> fSolidAngleBinCountVec;
  double fSolidAngleP;
  double fSolidAngleErrN;
  double fSolidAngleErrP;
  std::vector<double> fSolidAngleNVec;
  std::vector<double> fSolidAnglePVec;
  std::vector<double> fSigmaSumNVec;
  std::vector<double> fSigmaSumPVec;
  std::vector<double> fSigmaSolidAngleNVec;
  std::vector<double> fSigmaSolidAnglePVec;
  std::vector<double> fIntermediateEntriesCount;
  TGraph *graphSigmaSumN;
  TGraph *graphSigmaSumP;
  TGraph *graphSolidAngle;

  // Ratio of particle to generated
  TH2F *fhAnglesRatioNG; // Neutron singles
  TH2F *fhAnglesRatioPG; // Proton singles
  TH2F *fhAnglesRatioEG; // Electron singles
  TH2F *fhAnglesRatioHG; // Hadron coincidence (n+p)
  TH2F *fhAnglesRatioAG; // All particle coincidence (e+n+p)
  TH2F *fhAnglesRatioEGClean; // Electron singles (cleaned of low statistics stuff)

  // Ratio of Hadron to Electron
  TH2F *fhAnglesRatioNE; // Neutron singles
  TH2F *fhAnglesRatioPE; // Proton singles
  TH2F *fhAnglesRatioHE; // Hadron coincidence (n+p)

  TH2F *fhHArmAnglesMap;
  TH2F *fhEArmAnglesMap;
  TH2F *fhHArmPositions[2];
  TH2F *fhHArmPositionsInAcceptance[2];
  TH2F *fhHArmPositionsWeighted[2];
  TH2F *fhHArmPositionsWithTrig[2];
  TH2F *fhHArmPositionsWeightedWithTrig[2];
  TH2F *fhHArmPositionsInAcceptanceWeighted[2];
  TH2F *fhEArmPositions;
  TH2F *fhEArmPositionsInAcceptance;
  TH2F *fhHArmCoincidencePositions[2];
  TH2F *fhEArmCoincidencePositions;
  TH2F *fhHArmZvsMomentum[2];

  std::vector<std::vector<TH2F*> > fhHArmPositionSlices[2];

  TProfile2D *fhHAngleVPositionN;
  TProfile2D *fhHAngleVPositionP;
  TProfile2D *fhHAngleHPositionN;
  TProfile2D *fhHAngleHPositionP;
  TProfile2D *fhHAngleVPositionDiffs;
  TProfile2D *fhHAngleHPositionDiffs;
  TH1F *fhHAngleVPositionDiffAvg;
  TH1F *fhHAngleHPositionDiffAvg;

  TH2F *fhHAngleDetectedN;
  TH2F *fhHAngleDetectedP;
  std::vector<double> fPositionExclusionZone;
  std::vector<TH2F*>  fhHAngleGoodIdentificationN;
  std::vector<TH2F*>  fhHAngleGoodIdentificationP;
  std::vector<TH2F*>  fhHAngleBadIdentificationN;
  std::vector<TH2F*>  fhHAngleBadIdentificationP;
  GMnResults_t fResults;
};

class MyEdgeFit {
public:
  MyEdgeFit() : fN(0), fFCN(0), fFitter(0), fPhMin(0), fPhMax(0) {
  };

  MyEdgeFit(std::vector<double> x, std::vector<double> y)
    : fN(0), fFCN(0), fFitter(0), fPhMin(0), fPhMax(0) {
    SetVecs(x,y);
  }

  void SetVecs(std::vector<double> x, std::vector<double> y) {
    if(x.size() == y.size()) {
      fXs.clear();
      fYs.clear();
      fXs = x;
      fYs = y;
      fN = fXs.size();

      fFCN = new ROOT::Math::Functor(this,&MyEdgeFit::Chi2,3);
      fFitter = new ROOT::Fit::Fitter();
      fPars[0] = 90.;
      fPars[1] = 0.0;
      fPars[2] = 45.0;
      fFitter->SetFCN(*fFCN,fPars);
      fFitter->Config().ParSettings(0).SetName("X");
      fFitter->Config().ParSettings(1).SetName("Y");
      fFitter->Config().ParSettings(2).SetName("R");
    }
  }

  bool Fit() {
    bool ok = fFitter->FitFCN();
    if(!ok) {
      Error("Boo!","Fit failed :(");
    } else {
      const ROOT::Fit::FitResult &result = fFitter->Result();
      result.Print(std::cout);
      for(int i = 0; i < 3; i++) {
        fPars[i] = result.Parameter(i);
      }
    }
    return ok;
  }

  /*
  void DrawFit(const char *opt = "") {
    if(fArc)
      delete fArc;
    fArc = new TArc(fPars[0],fPars[1],fPars[2]);
    for(int i = 0; i < 3; i++) {
      std::cout << "p[" << i << "] = " << fPars[i] << std::endl;
    }
    fArc->SetLineColor(kRed);
    fArc->SetLineWidth(4);
    fArc->SetFillStyle(0);
    fArc->Draw(opt);
    gPad->Update();
  }*/

  double EvalRadius( double x, double y) {
    return TMath::Sqrt(
        TMath::Power(x-fPars[0],2) +
        TMath::Power(y-fPars[1],2) );
  }

  double Radius() { return fPars[2]; }

  double Chi2(const double *par) {
    if(fN <= 0) {
      return -1;
    }
    Double_t f = 0;
    for (Int_t i=0;i<fN;i++) {
      Double_t u = fXs[i] - par[0];
      Double_t v = fYs[i] - par[1];
      Double_t dr = par[2] - std::sqrt(u*u+v*v);
      f += dr*dr;
    }
    return f;
  }

private:
  std::vector<double> fXs;
  std::vector<double> fYs;
  int fN;
  ROOT::Math::Functor *fFCN;
  ROOT::Fit::Fitter *fFitter;
  double fPars[3];
  //TArc *fArc;
  double fPhMin;
  double fPhMax;
};

void excludeOutliers(std::vector<double> &v0, std::vector<double> &v1, bool doSecondPass = true)
{
  int n = v0.size();
  if(n  <= 3 )
    return;
  double mean = 0;
  double sigma = 0;
  std::vector<double> t0 = v0;
  std::vector<double> t1 = v1;
  v0.clear();
  v1.clear();
  for(int i = 0; i < n; i++) {
    mean += t0[i];
  }
  mean /= double(n);
  for(int i = 0; i < n; i++) {
    sigma += TMath::Power((t0[i]-mean),2);
  }
  sigma = TMath::Sqrt(sigma/(n-1));

  // Now find the outliers
  for(int i = 0; i < n; i++) {
    if(TMath::Abs(t0[i]-mean)<3.5*sigma) {
      v0.push_back(t0[i]);
      v1.push_back(t1[i]);
    }
  }

  if(doSecondPass) {
    excludeOutliers(v0,v1,false);
  }
}

#endif // GMNACCEPTANCE_H
