{
  double minCounts = 2;
  gROOT->Macro("results/hcal_voffset/gmn_kin01_angles.C");
  TH2F *hBothArmsAngles0 = (TH2F*)gDirectory->Get("hBothArmsAngles0");
  TH2F *hBothArmsAngles1 = (TH2F*)gDirectory->Get("hBothArmsAngles1");
  TCanvas *cv = new TCanvas("cv","cv",300,300);
  int nbinsx0 = hBothArmsAngles0->GetNbinsX();
  int nbinsy0 = hBothArmsAngles0->GetNbinsY();
  int nbinsx1 = hBothArmsAngles1->GetNbinsX();
  int nbinsy1 = hBothArmsAngles1->GetNbinsY();
  TH2F *hOverlap = new TH2F(*hBothArmsAngles0);
  double set = 0.0;
  if(nbinsx0 == nbinsx1 && nbinsy0 == nbinsy1) {
    for(int bx = 0; bx < nbinsx0; bx++ ) {
      for(int by = 0; by < nbinsy0; by++ ) {
        set = 0.0;
        if(hBothArmsAngles0->GetBinContent(bx,by) > minCounts &&
            hBothArmsAngles1->GetBinContent(bx,by) > minCounts) {
          set = 1.0;
        }
        hOverlap->SetBinContent(bx,by,set);
      }
    }
    hOverlap->SetFillColor(kGreen);
    hOverlap->Draw("BOX");
  }
}
