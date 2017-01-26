#ifndef GMNCOLORS_H
#define GMNCOLORS_H

#include <TStyle.h>

class GMnColors {
public:
  GMnColors() {
    MakePaletteSequential();
    MakePaletteDiverging();
  }

  void SelectPaletteSequential() {
    gStyle->SetPalette(fNumPaletteSequentialColors,fPaletteSequentialColors);
  };

  void SelectPaletteDiverging() {
    MakePaletteDiverging();
  };

private:
  int fNumPaletteSequentialColors;
  int *fPaletteSequentialColors;

  void MakePaletteDiverging() {
    const Int_t NRGBs = 5;
    const Int_t NCont = 255;

    Double_t stops[NRGBs] = { 0.00, 0.34, 0.61, 0.84, 1.00 };
    Double_t red[NRGBs]   = { 0.00, 0.00, 0.87, 1.00, 0.51 };
    Double_t green[NRGBs] = { 0.00, 0.81, 1.00, 0.20, 0.00 };
    Double_t blue[NRGBs]  = { 0.51, 1.00, 0.12, 0.00, 0.00 };
    TColor::CreateGradientColorTable(NRGBs, stops, red, green, blue, NCont);
    gStyle->SetNumberContours(NCont);

  }

  void MakePaletteSequential() {
    fNumPaletteSequentialColors = 9;
    fPaletteSequentialColors = new Int_t[fNumPaletteSequentialColors];
    for(int i = 0; i < fNumPaletteSequentialColors; i++) {
      fPaletteSequentialColors[i] = 1900+i;
    }

    (void)new TColor(1900,255./255.,247./255.,236./255.);
    (void)new TColor(1901,254./255.,232./255.,200./255.);
    (void)new TColor(1902,253./255.,212./255.,158./255.);
    (void)new TColor(1903,253./255.,187./255.,132./255.);
    (void)new TColor(1904,252./255.,141./255.,89./255.);
    (void)new TColor(1905,239./255.,101./255.,72./255.);
    (void)new TColor(1906,215./255.,48./255.,31./255.);
    (void)new TColor(1907,179./255.,0./255.,0./255.);
    (void)new TColor(1908,127./255.,0./255.,0./255.);
  };
};

GMnColors *gGMnColors;
void GMnColorsInit() {
  gGMnColors = new GMnColors();
}

#endif // GMNCOLORS_H
