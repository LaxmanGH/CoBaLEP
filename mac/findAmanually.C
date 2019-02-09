#include "TF1.h"
#include "Math/WrappedTF1.h"
#include "Math/GaussIntegrator.h"

void findAmanually(void)
{//Last line of the script outputs the value of the normalization constant A
 //for the function plotted from 1 to 2500 GeV 

  TF1 fitfunc("fitfunc","TMath::Exp(-([0]*[1]*([2]-1)))*TMath::Power((x+[3]*(1-TMath::Exp(-([0]*[1])))),-[2])",1,2500);

  fitfunc.SetParameters(0.4,4.3,3.77,693);//second parameter is height (default 4.3)

  ROOT::Math::WrappedTF1 wf1(fitfunc);
  ROOT::Math::GaussIntegrator ig;
  ig.SetFunction(wf1);
  ig.SetRelTolerance(0.001);
  cout << 1/ig.Integral(1, 2500) << endl;
  

}
