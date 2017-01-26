#ifndef PLOTACCEPTANCE_H
#define PLOTACCEPTANCE_H

#include <TChain.h>
#include <TFile.h>
#include <iostream>
#include <GMnGlobal.h>
#include <TROOT.h>
#include <TError.h>
#include <fstream>
#include <TSystem.h>
#include <TRandom3.h>
#include <TMath.h>
#include <GMnAcceptance.h>
#include <TPaveStats.h>

enum PlotAcceptancePrintType_t {
  kPrintAsInt = 0,
  kPrintAsDouble = 1
};

class FileAndStdOut {
public:
  FileAndStdOut() {};
  FileAndStdOut(const char *fileName) {
    Init(fileName);
  }
  void Init(const char *fileName) {
    fFile.open(fileName,std::ios::out);
  }
  void SetFileName(const char *fileName) { Init(fileName); }
  virtual ~FileAndStdOut() {};
  FileAndStdOut& operator<<(ostream& (*pfun)(ostream&))
  {
    if(is_open())
      pfun(fFile);
    pfun(std::cout);
    return *this;
  }

  bool is_open() { return fFile.is_open(); }
private:
  std::fstream fFile;
};

template<class T>
inline FileAndStdOut& operator<<(FileAndStdOut &s, T v)
{
  if(s.is_open())
    s.fFile << v;
  std::cout << v;
  return s;
}

class PlotAcceptance {
public:
  PlotAcceptance(int kin, const char *date, double Q2);
  virtual ~PlotAcceptance();

  void InitChain();
  TString GetSavePath(bool inSubdir = false);
  void AddVOff(double voff);
  void GetEntry(int entry);
  void DetermineAcceptances();
  void FillHistogramsInAcceptance();
  void Run();
  void SaveResult(const char *var, double val, const char *unit);
  void SaveResultParticleID(double zoneW,
    GMnResultsSigmaSol_t &bad, GMnResultsSigmaSol_t &good);
  void PrintResultCompare(double result1, double result2, int level);
  void PrintTitle(TString title, int level);
  void PrintSigmaSol(GMnResultsSigmaSol_t &r, int level = 0,
   bool printSol = false);
  void PrintFormattedLevelLine(TString pre, TString post,
    int level);
  TString FormatPrintUnit(TString unit);
  void PrintResult(TString title, double result, TString unit,
    int level = 0, bool treatAsInt = 0);
  TString FormatPrintValue(double value,
    PlotAcceptancePrintType_t type);


private:
  TChain *fChain;
  TChain *fChainGen;
  TString fDate;
  Int_t fKin;
  Int_t fChainEntries;

  GMnData_t fData;
  gen_t fGenData;
  FileAndStdOut fOut;
  fstream fOutDB;
  Int_t fNextEntryMilestone;
  bool fApplyResolutionSmearing;
  TRandom3 fRand;
  Int_t fCurrentEntry;
  Double_t fQ2;
  std::vector<GMnAcceptance*> fGMnAcceptances;
  GMnStatus_t fStatus;
};


#endif // PLOTACCEPTANCE_H
