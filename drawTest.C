void MakePalette() {
  const Int_t NRGBs = 5;
  const Int_t NCont = 255;

  Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
  Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
  Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
  Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
  TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
  gStyle->SetNumberContours(NCont);
}


void drawTest()
{
  MakePalette();
  //gROOT->Macro("$JC2ROOTSTYLE");
  double bins = 100;
  double minX = -1.5;
  double maxX =  1.5;
  double minY = -1.25;
  double maxY =  1.75;

  //TFile *file = new TFile("/lustre/expphy/volatile/halla/sbs/cornejo/rootfiles/hcal_voffset/may_v2/kin03/gmn_kin03_r000.root");
  TChain *T = new TChain("T");
  T->Add("/lustre/expphy/volatile/halla/sbs/cornejo/rootfiles/hcal_voffset/may_v2/kin03/gmn_kin03_r*.root");
  //T->Add("/lustre/expphy/volatile/halla/sbs/cornejo/rootfiles/hcal_voffset/may_v2/kin03/gmn_kin03_r000.root");
  //TTree *T = (TTree*)file->Get("T");
  TCut cut("TMath::Abs(ev.phi)<.3&&TMath::Abs(ev.theta-1.019272*180/TMath::Pi())<0.25");

  TCanvas *canvas = new TCanvas("canvas","canvas",800,800);
  TCanvas *canvas2 = new TCanvas("canvas2","canvas2",800,800);
  canvas2->Divide(2,2);
  TH2F *hNXY = new TH2F("hNXY","Neutron Image on HCAL (|#Delta#phi|,|#Delta#theta|<0.25#circ)"
      ";Position in HCAL X (m);Position in HCAL Y (m)",bins,minX,maxX,bins,minY,maxY);
  TH2F *hPXY = new TH2F("hPXY","Proton Image on HCAL (|#Delta#phi|,|#Delta#theta|<0.25#circ)"
      ";Position in HCAL X (m);Position in HCAL Y (m)",bins,minX,maxX,bins,minY,maxY);

  TH1F *hNX  = new TH1F("hNX", "",bins,minX,maxX);
  TH1F *hPX  = new TH1F("hPX", "",bins,minX,maxX);
  TH1F *hNY  = new TH1F("hNY", "",bins,minY,maxY);
  TH1F *hPY  = new TH1F("hPY", "",bins,minY,maxY);
  std::cout << "b=" << bins << " x0=" << minX << " x1=" << maxX << " y0="
    << minY << " y1=" << maxY << std::endl;
  //hNX->SetStats(false);
  //hPX->SetStats(false);
  //hNY->SetStats(false);
  //hPY->SetStats(false);

  canvas2->cd(1);
  T->Draw("nvars.x>>hNX",TCut("n_det")&&cut);
  canvas2->cd(2);
  T->Draw("pvars.x>>hPX",TCut("p_det")&&cut);
  canvas2->cd(3);
  T->Draw("nvars.y+.15>>hNY",TCut("n_det")&&cut);
  canvas2->cd(4);
  T->Draw("pvars.y+.15>>hPY",TCut("p_det")&&cut);

  double nx = hNX->GetMean();
  double ny = hNY->GetMean();
  double px = hPX->GetMean();
  double py = hPY->GetMean();
  double my = (ny+py)/2.;
  canvas2->SaveAs("drawTest_x_y.png");

  canvas->cd(0);
  gPad->SetGrid(true,true);
  hNXY->SetMarkerColor(kRed+1);
  hPXY->SetMarkerColor(kBlue+1);
  hNXY->SetStats(false);
  hPXY->SetStats(false);
  T->Draw("nvars.y+.15:nvars.x>>hNXY",TCut("n_det")&&cut);
  T->Draw("pvars.y+.15:pvars.x>>hPXY",TCut("p_det")&&cut);

  TArc *cN = new TArc(nx,ny,0.38397244);
  TArc *cP = new TArc(px,py,0.38397244);
  cN->SetLineColor(kPink+9);
  cP->SetLineColor(kViolet+9);
  cN->SetFillColorAlpha(kPink+9,0.);
  cP->SetFillColorAlpha(kViolet+9,0.);
  cN->SetLineWidth(3);
  cP->SetLineWidth(3);
  TLine *line = new TLine(minX,my,maxX,my);
  line->SetLineWidth(3);
  line->SetLineColor(1);

  line->SetLineStyle(9);
  cN->SetLineStyle(9);
  cP->SetLineStyle(9);

  // Now draw Proton mis-identified
  hPXY->Draw("COLZ");
  cN->Draw();
  //cP->Draw();
  line->Draw();
  canvas->SaveAs("drawTest_proton.png");

  hNXY->Draw("COLZ");
  //cN->Draw();
  cP->Draw();
  line->Draw();
  canvas->SaveAs("drawTest_neutron.png");

  //  hNXY->Draw("SAME");
  //  hPXY->Draw("SAME");
}
