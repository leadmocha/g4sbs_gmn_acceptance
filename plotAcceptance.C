#include "plotAcceptance.h"
#include "shrinkGMn.h"
#include "GMnAcceptance.h"
#include "GMnColors.h"

// Set the number of vertical offsets to check
//const int kNumVOffs = 4;
//const double kVOffs[kNumVOffs] = { 0, 15, 30, 45};
// As of Jan 2017 it has become obvious that 45 cm will be the minimum
// offset. So to speed up the analysis, I only consider this one
const int kNumVOffs = 1;
const double kVOffs[kNumVOffs] = { 45 };

const bool kQuickPlot = false;
const double kQuickPlotPercent = 0.03;
const bool   kApplyResolutionSmearing = true;
const double kMomentumResolution = 0.011; // In percent
const double kAngularVerticalResolution = 1.2e-3*57.29578;  // in degrees
const double kAngularHorizontalResolution = 1.8e-3*57.29578;  // in degrees
const double kHCalPositionResolution = 0.035; // in m

const bool kApplyTestCuts = false;
const double kTestPhiCutMin = -0.01;
const double kTestPhiCutMax =  0.01;
const double kTestThetaCutMin = 31.99;
const double kTestThetaCutMax = 32.01;

const int kPrintTitleSize = 45; // 40 characters in print titles
const char *kFormatPrintInt     = "%-10d";
const char *kFormatPrintDouble  = "%-12.8f";

void PlotAcceptance::InitChain()
{
  // Delete an old one if present
  if(fChain)
    delete fChain;
  if(fChainGen)
    delete fChainGen;

  fChain = new TChain("T");
  fChainGen = new TChain("TGen");
  std::cout << "Found "
    << fChain->Add(Form("%s/%s/kin%02d/gmn_kin%02d_r*.root",
          getenv("GMnRootfiles"),fDate.Data(),fKin,fKin));
  fChainGen->Add(Form("%s/%s/kin%02d/gmn_kin%02d_r*.root",
          getenv("GMnRootfiles"),fDate.Data(),fKin,fKin));
  fChainEntries = fChain->GetEntries();
  if(kQuickPlot)
    fChainEntries *= kQuickPlotPercent;
  std::cout << " rootfiles with total entries: " << fChainEntries << std::endl;

  fChain->SetBranchAddress("ev",&fData.ev.nsigma);
  fChain->SetBranchAddress("evars",&fData.evars.x);
  fChain->SetBranchAddress("pvars",&fData.pvars.x);
  fChain->SetBranchAddress("nvars",&fData.nvars.x);
  fChain->SetBranchAddress("e_det",&fData.e_det);
  fChain->SetBranchAddress("p_det",&fData.p_det);
  fChain->SetBranchAddress("n_det",&fData.n_det);
  fChainGen->SetBranchAddress("gen",&fGenData.thbb);

  // It is also useful to get the magnetic field components now and save
  // a plot of them
  std::cout << "Saving Field Map histograms" << std::endl;
  TFile *file = fChainGen->GetFile(); // Pointer to one open file
  TString histNames[4] = {"field","field_x","field_y","field_z"};
  TH2F *hField = 0;
  TCanvas *cTmp = new TCanvas("cTmp","cTmp",800,800);
  for(int i = 0; i < 4; i++) {
    hField = (TH2F*)file->Get(histNames[i]);
    hField->Draw("COLZ");
    // Disable all Info prints
    int ignore = gErrorIgnoreLevel;
    gErrorIgnoreLevel = kWarning; // Disable all Info prints
    cTmp->SaveAs(TString::Format("%s/gmn_kin%02d_%s.png",
          GetSavePath().Data(),fKin,histNames[i].Data()));
    gErrorIgnoreLevel = ignore;
  }
}


void PlotAcceptance::SaveResult(const char *var, double val, const char *unit)
{
  fOutDB << var << "," << std::fixed << std::setprecision(8) << val
    << "," << unit << std::endl;
}

void PlotAcceptance::SaveResultParticleID(double zoneW,
    GMnResultsSigmaSol_t &bad, GMnResultsSigmaSol_t &good)
{
  fOutDB  << "exclusion_zone" << std::fixed 
    << "," << std::setprecision(8)<< zoneW
    << "," << std::setprecision(8) << bad.sigmaP
    << "," << std::setprecision(8) << bad.sigmaN
    << "," << std::setprecision(8) << good.sigmaP
    << "," << std::setprecision(8) << good.sigmaN
    << "," << std::setprecision(8) << bad.solidP
    << "," << std::setprecision(8) << bad.solidN
    << "," << std::setprecision(8) << good.solidP
    << "," << std::setprecision(8) << good.solidN
    << std::endl;
}
void PlotAcceptance::PrintResultCompare(double result1, double result2, int level)
{
  double diff = 200.*(result1-result2)/(result1+result2);
  for(int l = 0; l < 0; l++) {
    fOut << "  ";
  }
  fOut << TString::Format("%-50s %-10.3f (%%)","",diff) << std::endl;
}

void PlotAcceptance::PrintTitle(TString title, int level)
{
  if(!title.IsNull()) {
    PrintFormattedLevelLine(title.Append(":"),"",level-1);
    int length = title.Length();
    TString line;
    for(int l = 0; l < length; l++) {
      line.Append("-");
    }
    PrintFormattedLevelLine(line,"",level-1);
  }
}

void PlotAcceptance::PrintSigmaSol(GMnResultsSigmaSol_t &r, int level,
   bool printSol )
{
  PrintResult("Proton  Integrated Sigma",r.sigmaP,"pb",level);
  PrintResult("Neutron Integrated Sigma",r.sigmaN,"pb",level);
  if(printSol) {
    PrintResult("Proton  Solid Angle",r.solidP,"msr",level);
    PrintResult("Neutron Solid Angle",r.solidN,"msr",level);
  }
}

void PlotAcceptance::PrintFormattedLevelLine(TString pre, TString post,
    int level)
{
  TString line;
  if(!pre.IsNull()) {
    for(int l = 0; l < level; l++) {
      line.Append("  ");
    }
    line += pre;
  }
  if(!post.IsNull()) {
    int num_spaces = kPrintTitleSize-int(line.Length());
    for(int s = 0; s < num_spaces; s++) {
      line.Append(" ");
    }
    line += post;
  }
  fOut << line.Data() << std::endl;
}

TString PlotAcceptance::FormatPrintUnit(TString unit)
{
  if(!unit.IsNull())
    unit.Prepend(" (").Append(")");
  return unit;
}

void PlotAcceptance::PrintResult(TString title, double result, TString unit,
    int level, bool treatAsInt)
{
  if(!title.IsNull())
    title.Append(":");

  TString value;
  if(treatAsInt)
    value.Append(FormatPrintValue(result,kPrintAsInt));
  else
    value.Append(FormatPrintValue(result,kPrintAsDouble));
  value.Append(FormatPrintUnit(unit));
  // Finally, print the line
  PrintFormattedLevelLine(title,value,level);
}

TString PlotAcceptance::FormatPrintValue(double value,
    PlotAcceptancePrintType_t type)
{
  switch(type) {
    case kPrintAsInt:
      return TString::Format(kFormatPrintInt,int(value));
      break;
    case kPrintAsDouble:
      return TString::Format(kFormatPrintDouble,value);
        break;
  }
  // In case option not supported, return nothing
  return TString();
}

PlotAcceptance::PlotAcceptance(int kin, const char *date, double Q2) :
  fKin(kin),fQ2(Q2),fChain(0),fChainGen(0)
{
  fDate = TString(date);
  fRand.SetSeed(1000);

  // Load personal style sheet
  gROOT->Macro("$JC2ROOTSTYLE");
  gStyle->SetPadRightMargin(.15);
  GMnColorsInit();
  
  // Initialize the chain
  InitChain();

  // Initialize the output results file
  fOut.SetFileName(TString::Format("%s/kin%02d_results.dat",
        GetSavePath(false).Data(),fKin));
  fOutDB.open(TString::Format("%s/kin%02d_results.csv",
        GetSavePath(false).Data(),fKin),std::ios::out);
}

PlotAcceptance::~PlotAcceptance()
{
  fOutDB.close();
}

void PlotAcceptance::AddVOff(double voff)
{
  fGMnAcceptances.push_back(new GMnAcceptance(fKin,voff,
        GetSavePath(false)));
}

TString PlotAcceptance::GetSavePath(bool inSubdir)
{
  TString path(TString::Format("results/%s/kin%02d",fDate.Data(),
        fKin));
  if(inSubdir)
    path.Append("/extended/");
  gSystem->mkdir(path,kTRUE);
  return path;
}

void PlotAcceptance::GetEntry(int entry)
{
    // Get entry from tree
    fChain->GetEntry(entry);

    // Record current entry
    fCurrentEntry = entry;

    // Print out progress
    if(100.*double(entry+1)/double(fChainEntries) >= fNextEntryMilestone) {
      std::cout << "Processed " << fNextEntryMilestone << " %" << std::endl;
      fNextEntryMilestone += 10.;
    }
}

void PlotAcceptance::DetermineAcceptances()
{
  fNextEntryMilestone = 10;
  fApplyResolutionSmearing = false;  // Do not smear first pass
  std::cout << "Processing First Pass" << std::endl;
  for( int entry = 0; entry < fChainEntries; entry++) {
    GetEntry(entry);

    // Check if this entry passed the test selection cuts
    if(kApplyTestCuts && ( fData.ev.phi < kTestPhiCutMin ||
          fData.ev.phi > kTestPhiCutMax || fData.ev.theta < kTestThetaCutMin ||
          fData.ev.theta > kTestThetaCutMax )) {
      continue;
    }

    // Apply resolution smearing?
    if(fApplyResolutionSmearing) {
      // Smear angles by gausians
      fData.ev.theta += fRand.Gaus(0.0,kAngularHorizontalResolution);
      fData.ev.phi   += fRand.Gaus(0.0,kAngularVerticalResolution);

      // Smear HCAL positions for neutrons and protons separately
      Double_t r  = fRand.Gaus(0.0,kHCalPositionResolution);
      Double_t a = fRand.Uniform(0.0,2*TMath::Pi());
      fData.nvars.x += r*TMath::Cos(a);
      fData.nvars.y += r*TMath::Sin(a);
      r  = fRand.Gaus(0.0,kHCalPositionResolution);
      a = fRand.Uniform(0.0,2*TMath::Pi());
      fData.pvars.x += r*TMath::Cos(a);
      fData.pvars.y += r*TMath::Sin(a);
    }

    for(size_t v = 0; v < fGMnAcceptances.size(); v++) {
      fGMnAcceptances[v]->ProcessFirstPassEntry(entry,fStatus,fData);
    }
  }
  for(size_t v = 0; v < fGMnAcceptances.size(); v++) {
      fGMnAcceptances[v]->FinalizeFirstPass();
  }
}

void PlotAcceptance::FillHistogramsInAcceptance()
{
  fNextEntryMilestone = 10;
  std::cout << "Processing Second Pass" << std::endl;
  // Smear second pass if user enabled resolution smearing
  fApplyResolutionSmearing = true&&kApplyResolutionSmearing;
  if(fApplyResolutionSmearing) {
    std::cout << "Applying resolution smearing with the following:" << std::endl
      << "Angular (vertical) resolution: " << kAngularVerticalResolution
      << " deg" << std::endl << "Angular (horizontal) resolution: "
      << kAngularHorizontalResolution << " deg" << std::endl
      << "HCalPositionResolution: " << kHCalPositionResolution << " m"
      << std::endl;
  }
  for( int entry = 0; entry < fChainEntries; entry++) {
    // Get next entry
    GetEntry(entry);

    for(size_t v = 0; v < fGMnAcceptances.size(); v++) {
      fGMnAcceptances[v]->ProcessSecondPassEntry(entry,fStatus,fData);
    }
  }
  for(size_t v = 0; v < fGMnAcceptances.size(); v++) {
      fGMnAcceptances[v]->FinalizeSecondPass();
  }
}

void PlotAcceptance::Run()
{
  // First get the generated tree and pass it to the analyzers
  fChainGen->GetEntry(0);
  for(size_t v = 0; v < fGMnAcceptances.size(); v++) {
      fGMnAcceptances[v]->SetGenData(fGenData);
  }


  // Walk the tree, fill histograms and determine the acceptance
  DetermineAcceptances();

  // Now that the acceptances are determined, walk the tree again and
  // fill any histograms within the acceptances
  FillHistogramsInAcceptance();

  // Print results
  fOut << "----------------------------------------------"
    << "--------------------------------" << std::endl;
  fOut << "- - - - - - - - - - - - - - - KIN "
    << (fKin<10?"0":"") << fKin << " RESULTS - - - - - - - - - - - - - - - - -"
    << std::endl;
  fOut << "----------------------------------------------"
    << "--------------------------------" << std::endl;
  /*
  if(fQ2>0) {
    SaveResult("Q2",fQ2,"GeV^2");
  }*/
  for(size_t v = 0; v < fGMnAcceptances.size(); v++) {
      GMnResults_t results = fGMnAcceptances[v]->GetResults();

      fOutDB << "start_hcal_voffset_config" << std::endl;
      PrintResult("HCal Vertical Offset",results.voff,"cm",0,true);
      PrintResult("Avg P-N Vertical Separation",results.pnvsep*100.,"cm");
      SaveResult("hcal_voffset",results.voff,"cm");
      SaveResult("pn_vertical_sep",results.pnvsep*100,"cm");

      PrintTitle("Results with no ThetaPQ Cut",1);
      PrintSigmaSol(results.sigmaSol,1);
      SaveResult("total_sigma_proton",results.sigmaSol.sigmaP,"pb");
      SaveResult("total_solid_proton",results.sigmaSol.solidP,"msr");
      SaveResult("total_sigma_neutron",results.sigmaSol.sigmaN,"pb");
      SaveResult("total_solid_neutron",results.sigmaSol.solidN,"msr");

      PrintTitle("Results with ThetaPQ Cut/Identification",1);
      PrintResult("ThetaPQ Radius",results.thetaPQRadius*100,"cm",1);
      SaveResult("thetapq_radius",results.thetaPQRadius*100,"cm");
      if(fApplyResolutionSmearing) {
        PrintResult("Angular Vertical Resolution",kAngularVerticalResolution,
            "deg",1);
        PrintResult("Angular Horizontal Resolution",kAngularHorizontalResolution,
            "deg",1);
        PrintResult("HCal Position Resolution",kHCalPositionResolution*100,
            "cm",1);
        SaveResult("resolution_phi",kAngularVerticalResolution,"deg");
        SaveResult("resolution_theta",kAngularHorizontalResolution,"deg");
        SaveResult("resolution_hcal_position",kHCalPositionResolution*100,
            "cm");
      }
      for(size_t i = 0; i < results.goodIdentify.size(); i++) {
        fOut << std::endl;
        PrintResult("Results for Exclusion Zone",results.exclusionZoneWidth[i]*100,
            "cm",1,true);
        PrintTitle("Mis-identified Hadrons",2);
        PrintSigmaSol(results.badIdentify[i],2,false);
        PrintTitle("Positively Identified Hadrons",2);
        PrintSigmaSol(results.goodIdentify[i],2,false);
        SaveResultParticleID(results.exclusionZoneWidth[i]*100,
            results.badIdentify[i],results.goodIdentify[i]);
      }
      fOutDB << "end_hcal_voffset_config" << std::endl;

      // Print a new line
      fOut << "----------------------------------------------"
        << "--------------------------------" << std::endl;
  }

  /*
  std::cout << "Accepted Neutrons: " << fAcceptedNeutrons << std::endl;
  std::cout << "Accepted Protons:  " << fAcceptedProtons  << std::endl;
  std::cout << "Percent Diff:      " <<
    200.*(fAcceptedNeutrons-fAcceptedProtons)/
    (fAcceptedNeutrons+fAcceptedProtons) << std::endl;

  std::cout << "Average Proton-Neutron vertical position difference:   " <<
    fHCalAvgVPositionDiff << " +/- " << fHCalAvgVPositionDiffE << std::endl;
  std::cout << "Average Proton-Neutron horizontal position difference: " <<
    fHCalAvgHPositionDiff << " +/- " << fHCalAvgHPositionDiffE << std::endl;

  //std::cout << "Average Hadron momentum: " << hHadronMomentaAvg[0]->GetMean()
  //  << " GeV/c" << std::endl;
  */
}

///////////////////////////////////////////////////////////////////////////////
//
void plotAcceptance(int kin = 1, const char *date = "may", double Q2 = 0.)
{
  PlotAcceptance *plotter = new PlotAcceptance(kin,date,Q2);
  for(int i = 0; i < kNumVOffs; i++) {
    plotter->AddVOff(kVOffs[i]);
  }

  plotter->Run();
}

// Need to include other source files to make CINT compilation happy
#include "GMnAcceptance.C"
