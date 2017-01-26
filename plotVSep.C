void plotVSep(int kin, const char *date)
{
  TFile *file = new TFile(TString::Format("/lustre/expphy/volatile/halla/sbs/cornejo/rootfiles/hcal_voffset/%s/kin0%d/gmn_kin0%d_r000_small.root",date,kin,kin),"READ");
  TTree *tree = (TTree*)file->Get("T");
  TCanvas *canvas = new TCanvas("canvas","canvas",1200,800);
  canvas->Divide(3,2);
  canvas->cd(1);
  tree->Draw("100*(pvars.y-nvars.y)","e_det&&p_det&&n_det");
  canvas->cd(2);
  tree->Draw("1000*(pvars.py-nvars.py)","e_det&&p_det&&n_det");
  canvas->cd(3);
  tree->Draw("200*(pvars.py-nvars.py)/(pvars.p+nvars.p)","e_det&&p_det&&n_det");
  TH2F *h = new TH2F("h","",10,-1.5,1.5,10,-1.5,1.5);
  canvas->cd(4);
  h->Draw();
  tree->Draw("pvars.y:pvars.x","e_det&&p_det&&n_det","SAME");
  canvas->cd(5);
  h->Draw();
  tree->Draw("nvars.y:nvars.x","e_det&&p_det&&n_det","SAME");
}
