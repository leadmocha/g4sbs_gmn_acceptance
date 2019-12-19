#ifndef GMNCOLORS_H
#define GMNCOLORS_H

#include <TStyle.h>

class GMnColors {
public:
  GMnColors() {
    MakePaletteSequential();
    MakePaletteDiverging();
    MakePaletteTemperature();
  }

  void SelectPaletteSequential() {
    gStyle->SetPalette(fNumPaletteSequentialColors,fPaletteSequentialColors);
  };

  void SelectPaletteDiverging() {
    MakePaletteDiverging();
  };

  void SelectPaletteTemperature() {
    gStyle->SetPalette(fNumPaletteTemperatureColors,fPaletteTemperatureColors);
  }

  int GetNumPaletteTemperatureColors() { return fNumPaletteTemperatureColors; }

  void DrawHistoTemperature(TH2F *h, double min, double max)
  {
    SelectPaletteTemperature();
    int n = fNumPaletteTemperatureColors;
    double x = min;
    double dx = (max-min)/double(n);
    h->SetContour(n);
    for(int i = 0; i < n; i++) {
      h->SetContourLevel(i,x);
      x+=dx;
    }
    h->SetMinimum(min);
    h->SetMaximum(max);
    h->Draw("COLZ");
    gPad->Update();
  }

  void DrawHistoSymmetric(TH2F *h, double range)
  {
    DrawHistoTemperature(h,-range,range);
  }

private:
  int fNumPaletteSequentialColors;
  int *fPaletteSequentialColors;
  int fNumPaletteTemperatureColors;
  int *fPaletteTemperatureColors;

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

  void MakePaletteTemperature() {
    int nsteps = 7;
    fNumPaletteTemperatureColors = 1+nsteps*2;
    fPaletteTemperatureColors = new Int_t[fNumPaletteTemperatureColors];
    // First, do the reds
    int ii = 0;
    int ic;
    double rgb[3] = {0.,0.,0.};
    for(int i = 0; i < nsteps; i++) {
      if(rgb[2] < 1. ) {
        rgb[2] += 0.25;
      } else {
        rgb[0] += 0.25;
      }
      rgb[1] += 0.125;
      ic = TColor::GetFreeColorIndex();
      fPaletteTemperatureColors[ii++] = ic;
      (void)new TColor(ic, rgb[0], rgb[1], rgb[2]);
    }
    rgb[0] = rgb[1] = rgb[2] = 1.0;
    ic = TColor::GetFreeColorIndex();
    fPaletteTemperatureColors[ii++] = ic;
    (void)new TColor(ic, rgb[0], rgb[1], rgb[2]);
    for(int i = 0; i < nsteps; i++) {
      if(rgb[1] > 0.0 ) {
        rgb[1] -= 0.25;
        rgb[2] = rgb[1];
      } else {
        rgb[0] -= 0.25;
      }
      ic = TColor::GetFreeColorIndex();
      fPaletteTemperatureColors[ii++] = ic;
      (void)new TColor(ic, rgb[0], rgb[1], rgb[2]);
    }
  };


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
