{
  TCanvas *canvas = new TCanvas("canvas","canvas",600,600);
  canvas->Divide(2,2);

  TChain *chain = new TChain("T");
  chain->Add("rootfiles/test_jc2_magtest_*.root");

  canvas->cd(1);
  chain->Draw("Harm.HCal.hit.row:Harm.HCal.hit.col",
      "Harm.HCal.hit.NumPhotoelectrons&&ev.nucl==0","COLZ");
  canvas->cd(2);
  chain->Draw("Harm.HCal.hit.row:Harm.HCal.hit.col",
      "Harm.HCal.hit.NumPhotoelectrons&&ev.nucl==1","COLZ");
  canvas->cd(3);
  chain->Draw("Earm.BBGEM.Track.Y:Earm.BBGEM.Track.X",
      "ev.nucl==0","COLZ");
  canvas->cd(4);
  chain->Draw("Earm.BBGEM.Track.Y:Earm.BBGEM.Track.X",
      "ev.nucl==1","COLZ");
}
