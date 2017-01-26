
double dist( double p ){
  // Fit to Bernheim data·
  double thisp = p;
  if( p < 0.0 ) return 0.0;


  double a0 = 1.199e-6/7.8e-10;
  double b0 = -6.0522;
  double c0 = 7.202e2;

  double a1 = 1.6e-9/7.8e-10;
  double b1 = 17.448;

  if( p < 0.048 ){
    return a0*thisp*thisp*exp(-thisp*b0-thisp*thisp*c0);
  } else {
    return a1*exp(-thisp*b1);
  }
}

Double_t func(Double_t *x, Double_t *par)
{
  return dist(x[0]/1e3);
}

double norm = 1.0;
Double_t funcNorm(Double_t *x, Double_t *par)
{
  return dist(x[0]/1e3)/norm;
}

void deutpdist()
{
  //gROOT->Macro("$JC2ROOTSTYLE");
  TCanvas *canvas = new TCanvas("canvas","canvas",500,400);
  TF1 *funorm = new TF1("funorm",func,0.0,350,0);
  norm = 1./funorm->Integral(0.,350);
  TF1 *fnorm = new TF1("fnorm",funcNorm,0.,350,0);
  funorm->SetTitle("Deuteron Momentum Distribution;Nucleon Momentum (MeV/c)");
  fnorm->SetTitle("Deuteron Momentum Normalized Distribution;Nucleon Momentum (MeV/c);Probability (%)");
  funorm->Draw();
  gPad->SetGrid(kTRUE,kTRUE);
  std::cout << "Un-normalized integral: " << funorm->Integral(0.,350) << std::endl;
  std::cout << "Normalized integral: " << fnorm->Integral(0.,350) << std::endl;

  std::cout << "Integral > 200 MeV/c" << funorm->Integral(200,350) << std::endl;
  std::cout << "Ratio: " << funorm->Integral(200,350)/funorm->Integral(0.,350) << std::endl;

  canvas->SaveAs("results/deuteron_momentum_distribution.png");

  /*
     TF1 *f1 = new TF1("f1","1537.1795*x*x*exp(-x*(-6.0522)-x*x*7.202e2)",0,0.048);
     TF1 *f2 = new TF1("f2","(1.6e-9/7.8e-10)*exp(-x*17.448)",0.048,.350);
     TF1 *f3 = new TF1("f3","f1+f2",0,0.350);
     TF1 *f4 = new TF1("f4",Form("f3/%f",f3->Integral(0.,0.350)),0,0.350);
     f3->SetTitle("Deuteron Momentum Distribution");
     f3->Draw();
     std::cout << "Integral: " << f3->Integral(0.,0.350) << std::endl;
     std::cout << "Integral: " << f4->Integral(0.,0.350) << std::endl;
     */
}
