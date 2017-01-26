void makeGMnGeneratorFile(int kin, int run)
{
  TFile *fileIn = new TFile(TString::Format(
        "rootfiles/hcal_voffset/gmn_kin%02d_r%03d_v0.root",kin,run),"READ");
  TTree *treeIn = (TTree*)fileIn->Get("T");
  treeIn->SetBranchStatus("*",0);
  treeIn->SetBranchStatus("ev",1);
  treeIn->SetBranchStatus("ev_gmn",1);
  //treeIn->SetBranchStatus("gen",1);

  // Now the out file
  TFile *fileOut = new TFile(TString::Format("rootfiles/hcal_voffset/generated_"
        "gmn_kin%02d_r%03d.root",kin,run),"RECREATE");
  treeIn->CloneTree();
  fileOut->Write();
}
