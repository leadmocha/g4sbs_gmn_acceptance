double kMinEDep = 0.3; // GeV
int kNeutron = 0;
int kProton = 1;
int kHArm = 0;
int kEArm = 1;
int kBothArms = 2;
const char* kHadronName[2] = { "Neutron", "Proton" };

double kMinPhi[8]   = {0, -30, -30, -30, -30, -30, -50, -50 };
double kMaxPhi[8]   = {0,  30,  30,  35,  35,  35,  40,  50 };
double kMinTheta[8] = {0,  25,  35,  50,  35,  25,  30,  20 };
double kMaxTheta[8] = {0,  40,  50,  80,  60,  45,  60,  70 };
double kThetaDelta = 35.0;
double kMinBBX  = -1.1;
double kMaxBBX  =  1.1;
double kMinBBY  = -1.1;
double kMaxBBY  =  1.1;

double kMinHCALX  = -1.1;
double kMaxHCALX  =  1.1;
double kMinHCALY  = -2.1;
double kMaxHCALY  =  2.1;


int kBins = 100;
TChain *chain;

TH2F *hHArmAngles[2];
TH2F *hEArmAngles[2];
TH2F *hBothArmsAngles[2];
TH2F *hHArmPositions[2];
TH2F *hEArmPositions[2];
TH2F *hBothArmsPositions[2];


enum PlotType_t {
  PlotAngles,
  PlotPositions
};

void draw(int arm, int hadron, PlotType_t type = PlotAngles)
{
  TString drawVariables;
  TString histPostfix;
  switch(type) {
    case PlotAngles:
      drawVariables = "phi:theta";
      histPostfix = "Angles";
      break;
    case PlotPositions:
      histPostfix = "Positions";
      if(arm == kHArm) {
        drawVariables = "hcaly:hcalx";
      } else if ( arm == kEArm ) {
        drawVariables = "bby:bbx";
      } else if ( kArm == kBothArms) {
        return;
      } else {
        return;
      }
      break;
    default:
      return;
      break;
  };
  TCut cuts = Form("ev.nucl==%d&&ev.sigma",hadron);
  TString histName;
  if(arm == kHArm) {
    histName = Form("hHArm%s%d",histPostfix.Data(),hadron);
    cuts = cuts && "Harm.HCAL_box.hit.nhits>0" &&
      Form("Harm.HCAL_box.hit.sumedep>%0.2f",kMinEDep);
  } else if ( arm == kEArm ) {
    histName = Form("hEArm%s%d",histPostfix.Data(),hadron);
    cuts = cuts && "Earm.BBCal.hit.nhits" &&
      Form("Earm.BBCal.hit.sumedep>%0.2f",kMinEDep);
  } else if ( arm == kBothArms ) {
    histName = Form("hBothArms%s%d",histPostfix.Data(),hadron);
    cuts = cuts && "Harm.HCAL_box.hit.nhits&&Earm.BBCal.hit.nhits" &&
      Form("Harm.HCAL_box.hit.sumedep>%0.2f&&Earm.BBCal.hit.sumedep>%0.2f",
          kMinEDep,kMinEDep);
  }
  chain->Draw(Form("%s>>%s",drawVariables.Data(),histName.Data()),cuts,"COLZ");
}

void  plotAcceptance(int kin = 1)
{

  TCanvas *canvasAngles = new TCanvas("canvasAngles","canvasAngles",600,900);
  canvasAngles->Divide(2,3);
  TCanvas *canvasPositions = new TCanvas("canvasPositions","canvasPositions",600,600);
  canvasPositions->Divide(2,2);

  chain = new TChain("T");
  //chain->Add("jc2_output_test.root");
  //chain->Add("jc2_gmn_acceptance_kin01.root");
  chain->Add(Form("rootfiles/hcal_voffset/gmn_kin%02d_r*.root",kin));
  chain->SetAlias("theta","ev.th*57.2958");
  chain->SetAlias("phi","ev.ph*57.2958");
  chain->SetAlias("bbx","(Earm.BBCal.hit.xhit*TMath::Cos(gen.thbb)-"
      "Earm.BBCal.hit.zhit*TMath::Sin(gen.thbb))");
  chain->SetAlias("bby","(Earm.BBCal.hit.yhit)");
  chain->SetAlias("bbz","(Earm.BBCal.hit.xhit*TMath::Sin(gen.thbb)+"
      "Earm.BBCal.hit.zhit*TMath::Cos(gen.thbb))");
  chain->SetAlias("hcalx","(Harm.HCAL_box.hit.xhit*TMath::Cos(-gen.thsbs)-"
      "Harm.HCAL_box.hit.zhit*TMath::Sin(-gen.thsbs))");
  chain->SetAlias("hcaly","(Harm.HCAL_box.hit.yhit)");
  chain->SetAlias("hcalz","(Harm.HCAL_box.hit.xhit*TMath::Sin(-gen.thsbs)+"
      "Harm.HCAL_box.hit.zhit*TMath::Cos(-gen.thsbs))");

  for(int i = 0; i < 2; i++) {
    hHArmAngles[i] = new TH2F(Form("hHArmAngles%d",i),
        Form("Hadron Arm Acceptance: %s;#theta (deg);#phi (deg)",kHadronName[i]),
        kBins,kMinTheta[kin],kMaxTheta[kin],kBins,kMinPhi[kin],kMaxPhi[kin]);
    hHArmAngles[i]->SetStats(false);
    hEArmAngles[i] = new TH2F(Form("hEArmAngles%d",i),
        Form("Electron Arm Acceptance: %s;#theta (deg);#phi (deg)",kHadronName[i]),
        kBins,kMinTheta[kin],kMaxTheta[kin],kBins,kMinPhi[kin],kMaxPhi[kin]);
    hEArmAngles[i]->SetStats(false);
    hBothArmsAngles[i] = new TH2F(Form("hBothArmsAngles%d",i),
        Form("Electron & Hadron Coincidence Acceptance: %s;#theta (deg);#phi (deg)",kHadronName[i]),
        kBins,kMinTheta[kin],kMaxTheta[kin],kBins,kMinPhi[kin],kMaxPhi[kin]);
    hBothArmsAngles[i]->SetStats(false);

    hHArmPositions[i] = new TH2F(Form("hHArmPositions%d",i),
        Form("HCAL Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinHCALX,kMaxHCALX,kBins,kMinHCALY,kMaxHCALY);
    hHArmPositions[i]->SetStats(false);
    hEArmPositions[i] = new TH2F(Form("hEArmPositions%d",i),
        Form("BigBite Position: %s;x (m); y(m)",kHadronName[i]),
        kBins,kMinBBX,kMaxBBX,kBins,kMinBBY,kMaxBBY);
    hEArmPositions[i]->SetStats(false);
  }


  ///////////////////////////////////////////////
  // DRAW Angles
  // Draw Harm side
  canvasAngles->cd(1);
  draw(kHArm,kProton,PlotAngles);
  canvasAngles->cd(2);
  draw(kHArm,kNeutron,PlotAngles);
  // Draw Earm side
  canvasAngles->cd(3);
  draw(kEArm,kProton,PlotAngles);
  canvasAngles->cd(4);
  draw(kEArm,kNeutron,PlotAngles);

  // Draw coincidence plots
  canvasAngles->cd(5);
  draw(kBothArms,kProton,PlotAngles);
  canvasAngles->cd(6);
  draw(kBothArms,kNeutron,PlotAngles);

  /////////////////////////////////////////////
  // DRAW Positions
  // Draw Harm side
  canvasPositions->cd(1);
  draw(kHArm,kProton,PlotPositions);
  canvasPositions->cd(2);
  draw(kHArm,kNeutron,PlotPositions);
  // Draw Earm side
  canvasPositions->cd(3);
  draw(kEArm,kProton,PlotPositions);
  canvasPositions->cd(4);
  draw(kEArm,kNeutron,PlotPositions);

  canvasAngles->SaveAs(Form("results/hcal_voffset/gmn_kin%02d_angles.png",kin));
  canvasPositions->SaveAs(Form("results/hcal_voffset/gmn_kin%02d_positions.png",kin));
}
