{
  TCanvas *canvas = new TCanvas("canvas","canvas",600,900);
  canvas->Divide(2,3);

  TChain *chain = new TChain("T");
  chain->Add("jc2_output_test.root");

  canvas->cd(1);
  chain->Draw("Harm.HCAL_box.hit.yhit:Harm.HCAL_box.hit.xhit",
      "ev.nucl==0&&Harm.HCAL_box.hit.nhits","COLZ");
  //chain->Draw("Harm.HCAL_box.vy:Harm.HCAL_box.vx",
  //    "ev.nucl==0","COLZ");
  canvas->cd(2);
  chain->Draw("Harm.HCAL_box.hit.yhit:Harm.HCAL_box.hit.xhit",
      "ev.nucl==1&&Harm.HCAL_box.hit.nhits","COLZ");
  canvas->cd(3);
  chain->Draw("Harm.HCAL_box.hit.yhit",
      "ev.nucl==0&&Harm.HCAL_box.hit.nhits","COLZ");
  canvas->cd(4);
  chain->Draw("Harm.HCAL_box.hit.yhit",
      "ev.nucl==1&&Harm.HCAL_box.hit.nhits","COLZ");
  canvas->cd(5);
  chain->Draw("Harm.HCAL_box.hit.sumedep",
      "ev.nucl==0&&Harm.HCAL_box.hit.nhits","COLZ");
  canvas->cd(6);
  chain->Draw("Harm.HCAL_box.hit.sumedep",
      "ev.nucl==1&&Harm.HCAL_box.hit.nhits","COLZ");
}
