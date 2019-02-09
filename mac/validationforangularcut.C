#include "TMath.h"

void validationforangularcut(void)
{

  //This script is being used to test some equations for geometric cuts
  //to be implemented in the simulation script, as outlined in changelog
  //under September 7 2018. Given a muons's starting position/momentum,
  //calculate its zenith and azimuth angles, then calculate the valid
  //range of these angles which would take the muon within at least 5m
  //of the detector assembly. If either of the muon's angles are outside
  //these ranges, we can kill it, since its secondaries will have no
  //effect on the germanium crystals. I'd also be interested to know what
  //fraction of the muons are killed in this way. So, let's get started.

  //Step 0: create a random number generator
  //and define variables and known constants.
  TRandom3 *rng = new TRandom3();
  rng->SetSeed(0);

  
  double momentumzenith = 0;
  double momentumphi = 0;

  double positionphi = 0;
  double realphi = 0;//The real one to use in calculations

  double psi = 0;//See step 4
  double finalzenith = 0;//The angle that the muonzenith must be under

  int passfail = 0;//To determine fraction of muons passing the cut
  
  double muonpx = 0;
  double muonpy = 0;
  double muonpz = 0;
  
  double muonx = 0;
  double muony = 0;
  double muonz = 18;
  double r = 0;//defined in the usual way for
               //polar coordinates
  
  double L = 0;//horizontal distance between muon's
  //x,y and the point on the edge of the detector
  //assembly it will cross
  
  double R = 11.5;//radius of assembly
  
  TH1F *zenithtest = new TH1F("zenithtest","Zenith test",360,0,360);
  TH1F *phitest = new TH1F("phitest","Phi test",360,0,360);
  TH1F *lengthtest = new TH1F("lengthtest","distance between two points on the edge of a circle",300,0,30);

  
  //Since this test is dumb and doesn't matter, assume z always has
  //momentum>.5 GeV (up to 1), x has momentum from 0 to 0.5 GeV, as does y

    for(int i = 0;i < 10000000;i++)
      {

//Step 1: make some fake muon position and momentum parameters
  //for now assume r = sqrt(x^2+y^2) < R of assembly+5=11.5

	r = rng->Rndm()*360;//temporary
	muonx = (rng->Rndm())*R*TMath::Cos(r);
	muony = (rng->Rndm())*R*TMath::Sin(r);
	//It might seem odd to sample a random angle in order to sample
	//a random position in order to sample a random angle.
	//Just keep in mind this is for testing purposes only, to check
	//calculations and whatnot
	
	r = TMath::Sqrt(muonx*muonx+muony*muony);
	
	muonpz = rng->Rndm(); //between 0.5 and 1
	if(muonpz<0.5)
	  muonpz+=0.5;
	muonpx = rng->Rndm()-0.5;//between -0.5 and 0.5
	muonpy = rng->Rndm()-0.5;//between -0.5 and 0.5
	
  
  //Step 2: calculate zenith and phi angles for the muon's momentum
  
  momentumzenith = TMath::ACos(muonpz/TMath::Sqrt(muonpx*muonpx+muonpy*muonpy+muonpz*muonpz))*180/TMath::Pi();

  momentumphi = TMath::ATan2(muonpy,muonpx)*180/TMath::Pi()+180;


  //Step 3: calculate the muon's positional phi angle w.r.t. the origin
  //In my notes, this is phi_0

  positionphi = TMath::ATan2(muony,muonx)*180/TMath::Pi()+180;

  realphi =momentumphi-positionphi;
  if(realphi<0)
    realphi = 360 + realphi;

  phitest->Fill(realphi);

  //Step 4: Calculate the angle "psi" from my notebook calculations.
  //Too complicated to explain what it is without a drawing.
  //This part will be pure trig magic (tm)
  //Also calculate L

  psi = realphi - TMath::ASin((r/R)*TMath::Sin((realphi*TMath::Pi()/180)));
  //  zenithtest->Fill(psi);
  //a normal expanded vector dot product
  L = TMath::Sqrt(R*R+r*r-2*R*r*TMath::Cos(psi));

  //Step 5: Finally, calculate the muon's critical zenith angle
  //It must be under this angle or it will miss the detector assembly
  //We can compare here as well
  
  finalzenith = TMath::ASin(L/TMath::Sqrt(L*L+muonz*muonz))*180/TMath::Pi();
  
  zenithtest->Fill(finalzenith);
  lengthtest->Fill(L);
  if(momentumzenith>finalzenith)//muon will not pass near the assembly
    passfail++;    
      }

    /*  
    TH1::AddDirectory(kFALSE);

    zenithtest->Draw();

    TCanvas *c2 = new TCanvas("c2");
    c2->cd();
    phitest->Draw();
    
    TCanvas *c3 = new TCanvas("c3");
    c3->cd();
    lengthtest->Draw();
*/
    cout << endl << passfail << endl << endl;
  //  cout << momentumphi << endl;
}

//CJ notes (maybe temporary)
//Zenith calculation: validated
//phi calculations all validated
//L calculation validated (circle line picking results used)


