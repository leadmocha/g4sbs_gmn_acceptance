void shrinkGenerated(const char *fileName, const char *out = "shrunk.root")
{
  TFile *fileIn = new TFile(fileName, "READ");
  TTree *treeIn = (TTree*)fileIn->Get("T");
  std::cout << treeIn << std::endl;


  // Disable all branches
  treeIn->SetBranchStatus("*",0);

  // Enable only the ones we are using
  treeIn->SetBranchStatus("ev",1);
  treeIn->SetBranchStatus("ev_gmn",1);
  //treeIn->SetBranchStatus("gen",1);

  TFile *fileOut = new TFile(out, "RECREATE");
  treeIn->CloneTree();
  fileOut->Write();
  fileOut->Close();
}
