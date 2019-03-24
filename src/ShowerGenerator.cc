///////////////////////////////////////////////////////////////////////////
// This code implementation is the intellectual property of the
// LEGEND collaboration. It is based on Geant4, an
// intellectual property of the RD44 GEANT4 collaboration.
//
// *********************
//
// Neither the authors of this software system, nor their employing
// institutes, nor the agencies providing financial support for this
// work make any representation or warranty, express or implied,
// regarding this software system or assume any liability for its use.
// By copying, distributing or modifying the Program (or any work based
// on the Program) you indicate your acceptance of this statement,
// and all its terms.
//////////////////////////////////////////////////////////////////////////////
/// \file ShowerGenerator.cc
/// \brief Implementation of the ShowerGenerator class

#include "ShowerGenerator.hh"


#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4RunManager.hh"
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "Randomize.hh"
#include <math.h>

#include "G4MuonMinus.hh"
#include "G4MuonPlus.hh"
#include "G4Neutron.hh"
#include "G4Alpha.hh"
#include "G4Gamma.hh"
#include "G4Electron.hh"
#include "G4Positron.hh"
#include "G4Geantino.hh"

#include "G4NeutrinoE.hh"
#include "G4NeutrinoMu.hh"
#include "G4AntiNeutrinoE.hh"
#include "G4AntiNeutrinoMu.hh"
#include "G4KaonZeroLong.hh"
#include "G4KaonMinus.hh"
#include "G4KaonPlus.hh"
#include "G4PionMinus.hh"
#include "G4PionPlus.hh"
#include "G4AntiNeutron.hh"
#include "G4Deuteron.hh"

#include "TFile.h"
#include "TStyle.h"
#include "TTree.h"
#include "TChain.h"
#include "TRandom3.h"
#include "TMath.h"
#include <vector>
#include <string>

#include <iostream>
#include <iomanip>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

bool passcuts(double x, double y, double px, double py, double pz, double assemblyheight, double assemblyradius)
{//Calculate if this muon will pass within 5m of
 //the detector assembly. Return true or false.

  double r = TMath::Sqrt(x*x+y*y);//polar radius
  double R = assemblyradius+5;//Diameter will be +10
  double H = assemblyheight+10;
  double samplingheight = 18;//hardcoded...Bad
  double L = 0;
  double m = 0; //R + m = r
  
  //All angles will be used in degrees
  //because that's how I understand best
  double positionphi = 0;
  //angle of r with respect to the x axis
  //so if r = x, y = 0, phi = 0
  //if r = y, x = 0, phi = 90
  double momentumphi = 0;//angle of muon's momentum
  double momentumzenith = 0;//w.r.t. x and z axes

  double realphi = 0;//The one we actually use
  //Before calculations, we rotate the frame of
  //reference so that realphi = 0 lies along the
  //x axis. This simplifies the picture a bit

  double psi = 0;//Used for calculations
  //The polar angle between two lines
  //One from the origin to the muon
  //The other from the origin to the edge of the
  //assembly+5 that the muon will hit at its
  //widest angle. Hard to explain without pictures

  double criticalzenith = 0;//momemntumzenith has to
                            //be less than this

  double criticalphilow = 0;
  double criticalphihigh = 0;
  
  //The cut varies wildly depending on if r>R or not
  //For r<R, any phi could be valid but the zenith
  //angle cut is phi dependent. For r>R both cuts
  //need to be made but are independent

  if(r<R)
    {
      //calculate realphi and momentumzenith

        momentumzenith = TMath::ACos(pz/TMath::Sqrt(px*px+py*py+pz*pz))*180/TMath::Pi();
	
        momentumphi = TMath::ATan2(py,px)*180/TMath::Pi()+180;

	  positionphi = TMath::ATan2(y,x)*180/TMath::Pi()+180;

	realphi = momentumphi - positionphi;
	if(realphi<0)
	  realphi+=360;
	
	//psi is actually a function of phi
	//Deriving it requires a fair bit of trig
	//and explaining how it works in these
	//comments would be nearly impossible
	//So consider the next couple calculations
	//trigonometry magic and trust me
	//I used a script, validationforangularcut.C
	//to verify these calculations

	  psi = realphi - TMath::ASin((r/R)*TMath::Sin((realphi*TMath::Pi()/180)));

	  //L is just the dot product of R and r
	  //(if R is from the origin to the edge
	  //the muon would cross if exiting)
  L = TMath::Sqrt(R*R+r*r-2*R*r*TMath::Cos(psi));

  //There we have it - the maximum horizontal dis-
  //tance the muon can travel before it's invalid
  //for approaching within 5m of the assembly.

  //Now we use this to calculate the maximum allow-
  //able zenith angle.

  criticalzenith = TMath::ASin(L/TMath::Sqrt(L*L+samplingheight*samplingheight))*180/TMath::Pi();
  
  if(momentumzenith>criticalzenith)
    {
      //G4cout << "1" << G4endl;
      return false;
    }
  // return true;
    }//if(r<R)

  else
    {
//r>R and the muon is not directly above assembly+5
//Phi and zenith cuts required but they're simple

      psi = TMath::ASin(R/r)*180/TMath::Pi();
      momentumphi = TMath::ATan2(py,px)*180/TMath::Pi()+180;
      L = TMath::Sqrt(r*r-R*R);
        positionphi = TMath::ATan2(y,x)*180/TMath::Pi()+180;
	criticalphilow = 180-psi+positionphi;
	criticalphihigh = 180+psi+positionphi;
	if(criticalphilow>360)
	  criticalphilow = criticalphilow-360;
	if(criticalphihigh>360)
	  criticalphihigh = criticalphihigh-360;
	if(momentumphi > criticalphihigh  || momentumphi < criticalphilow)
	  {
	    //	    G4cout << "r: " << r << G4endl << "momentumphi: " << momentumphi << G4endl << "positionphi: " << positionphi  << G4endl << "Lowball: " << 180-psi+positionphi << " Highball: " << 180+psi+positionphi << G4endl;
	    return false;	
	  }
	
	m = r - R;
        momentumzenith = TMath::ACos(-pz/TMath::Sqrt(px*px+py*py+pz*pz))*180/TMath::Pi();
	
	criticalzenith = TMath::ATan(m/(H+samplingheight))*180/TMath::Pi();//smallest acceptable zenith
	//G4cout << "Momentum zenith: " << momentumzenith << "Lowball: " << criticalzenith;
	if(momentumzenith<criticalzenith)
	  {
	    //G4cout << "3" << G4endl;
	    return false;
	  }
	
	criticalzenith = TMath::ATan((2*R+m)/(samplingheight))*180/TMath::Pi();//largest acceptable zenith
	//G4cout << " Highball: " << criticalzenith << G4endl;
	if(momentumzenith>criticalzenith)
	  {
	    //G4cout << "4" << G4endl;
	    return false;
	  }
    }


  //If none of the if statements above returned false
  return true;



  


  
}


ShowerGenerator::ShowerGenerator()
{
  const char* muon_path = "/home/usd.local/cj.barton/Muon_GUORE/Truon_GUORE/mac";
  inputfile = new TFile(Form("%s/SmallMuonFile.root", muon_path) ,"READ");
  datatree = (TTree*)inputfile->Get("muontree");

  //  TRandom3 *ran3 = new TRandom3(50000);
  //ran3->SetSeed(time(NULL));

  
  max_entries = datatree->GetEntries();

  start_energy = 0;    //muon start energy in surface GeV
  start_costheta = 0;    //muon start angle
  particle_energy = 0;
  particle_momentumX = 0;  //GeV
  particle_momentumY = 0;  //GeV
  particle_momentumZ = 0;  //GeV
  weight = 0;
  
   datatree->SetBranchAddress("startenergy", &start_energy);
   datatree->SetBranchAddress("startcostheta", &start_costheta);
   datatree->SetBranchAddress("energy", &start_energy);
   datatree->SetBranchAddress("px", &particle_momentumX);
   datatree->SetBranchAddress("py", &particle_momentumY);
   datatree->SetBranchAddress("pz", &particle_momentumZ);
   datatree->SetBranchAddress("weight", &weight);
	 G4cout << max_entries << " " << inputfile->GetName() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ShowerGenerator::~ShowerGenerator()
{
  delete inputfile;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ShowerGenerator::GeneratePrimaryVertex(G4Event* anEvent)
{

  bool validmuon = false;
  double xx = 0;
  double yy = 0;
  double zz = 17.999;
  double comparator = 0;
  double comparee = 0;
  int counter = 0;
  int justforfun = 0;
  double energynorm = 0;
  while(!validmuon)
    {
  // Select random entry within file
  //Flat version (stable:) int ev_ID = (int)(G4UniformRand()*max_entries);
  comparator = G4UniformRand(); //should be between 0 and 1
  comparee = 0;
  counter = 0;
  justforfun++;
  //  G4cout << "Comparator: " << std::setprecision(20) << comparator << G4endl;
  while(comparee<comparator)
    {

      datatree->GetEntry(counter);

      comparee += weight; //weight is normalized so that total sum is 1.0 to high precision
      if(counter>=max_entries)
	G4cout << G4endl << "Sampling method error" << G4endl; //should never happen
      counter++;
    }

  //use the initial parameters of whichever muon was selected using comparator
  
  datatree->GetEntry(counter-1);
  //for all shower particles
  particle_time = 0.0*s;


  xx = (G4UniformRand()-0.5)*99;//from -49.5m to 49.5m
  yy = (G4UniformRand()-0.5)*99;



  validmuon = passcuts(xx,yy,particle_momentumX,particle_momentumY,particle_momentumZ,13,6.5);
  
    }//while !validmuon

  G4cout <<"It took " << justforfun << " attempts to sample a valid muon." << G4endl;
  particle_position.setX(xx*m);
  particle_position.setY(yy*m);
  particle_position.setZ(zz*m);

	G4PrimaryVertex* vertex = new G4PrimaryVertex(particle_position,particle_time);

	G4ParticleDefinition* particle_definition = 0;
	particle_definition = G4MuonMinus::MuonMinusDefinition();

  // Set momenta, no rotations done here (unlike in MaGe)
       double px_MJD = particle_momentumX;
       double py_MJD = particle_momentumY;
       double pz_MJD = particle_momentumZ;
       //       double pEnergy = particle_energy;
       //G4cout << "Counter: " << counter << G4endl;
       //G4cout << G4endl << "MUON MOMENTUM: " << px_MJD << " " << py_MJD << " " << pz_MJD << " " << G4endl;

       //Temporarily implemented as of March 2019
       //Retroactively make the energy a flat distribution from 0 to 200 TeV
       //First normalize the momenta so the particle thinks it's at 1 GeV

       energynorm = TMath::Sqrt(px_MJD*px_MJD+py_MJD*py_MJD+pz_MJD*pz_MJD);
       px_MJD = px_MJD/energynorm;
       py_MJD = py_MJD/energynorm;
       pz_MJD = pz_MJD/energynorm;
       //Now multiple by a random number between 0 and 20000
       //Should technically be 200000, but I want statistics!dammit!
       energynorm = G4UniformRand()*20000;
       px_MJD = px_MJD*energynorm;//G4RandExponential::shoot(20000);
       py_MJD = py_MJD*energynorm;//G4RandExponential::shoot(20000);
       pz_MJD = pz_MJD*energynorm;//G4RandExponential::shoot(20000);
       energynorm = TMath::Sqrt(px_MJD*px_MJD+py_MJD*py_MJD+pz_MJD*pz_MJD);
       //G4cout <<"Particle energy (in GeV): " << energynorm << G4endl;
       //G4cout <<"Px: "<< px_MJD << "Py: " <<py_MJD << "Pz: " << pz_MJD<< G4endl;
  G4ThreeVector momentum(px_MJD*GeV,py_MJD*GeV,pz_MJD*GeV);

	G4PrimaryParticle* thePrimaryParticle =
    new G4PrimaryParticle(particle_definition,
						  px_MJD*GeV,
						  py_MJD*GeV,
						  pz_MJD*GeV);
	vertex->SetPrimary(thePrimaryParticle);
  // vertex->SetWeight(Distribution(start_energy,start_costheta));  // Implemented elsewhere

  anEvent->AddPrimaryVertex(vertex);


}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//weighting with incident muon distribution
//https://escholarship.org/uc/item/6jm8g76d#page-3
double ShowerGenerator::Distribution(double Energy, double CosTheta)
{
  double CosThetaStar = sqrt(
	(pow(CosTheta,2) + pow(0.102573,2) -0.068287*pow(CosTheta,0.958633)+0.0407253*pow(CosTheta,0.817285) )/
	(1+pow(0.102573,2)-0.068287+0.0407253));

  double I;
  I = 0.14*pow(Energy*(1+3.64/(Energy*pow(CosThetaStar,1.29))),-2.7)
	  *((1./(1+(1.1*Energy*CosThetaStar)/115.))+(0.054/(1+(1.1*Energy*CosThetaStar)/850.)));

  return (I);
}


/*Deprecated

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WeightedGenerator::WeightedGenerator()
{
  const char* muon_path = "/home/CJ.Barton/Muon_GUORE/Truon_GUORE/mac";
  inputfile = new TFile(Form("%s/SmallMuonFile.root", muon_path) ,"READ");
  datatree = (TTree*)inputfile->Get("muontree");

  max_entries = datatree->GetEntries();

  startenergy = 0;    //muon start energy in surface GeV
  startcostheta = 0;    //muon start angle
  energy = 0;
  px = 0;  //GeV
  py = 0;  //GeV
  pz = 0;  //GeV
  x = 0;
  y = 0;
  z = 0;

   datatree->SetBranchAddress("startenergy", &startenergy);
   datatree->SetBranchAddress("startcostheta", &startcostheta);
   datatree->SetBranchAddress("energy", &energy);
   datatree->SetBranchAddress("px", &px);
   datatree->SetBranchAddress("py", &py);
   datatree->SetBranchAddress("pz", &pz);
   datatree->SetBranchAddress("x", &x);
   datatree->SetBranchAddress("y", &y);
   datatree->SetBranchAddress("z", &z);

	 G4cout << max_entries << " " << inputfile->GetName() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WeightedGenerator::~WeightedGenerator()
{
  delete inputfile;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void WeightedGenerator::GeneratePrimaryVertex(G4Event* anEvent)
{
  // Select random entry within file
  int ev_ID = (int)(G4UniformRand()*max_entries);
  datatree->GetEntry(ev_ID);

  particletime = 0.0*s;
  double xx = x[ev_ID];
  double yy = y[ev_ID];
  double zz = z[ev_ID];
  double pxx = px[ev_ID];
  double pyy = py[ev_ID];
  double pzz = pz[ev_ID];
  particleposition.setX(xx*m);
  particleposition.setY(yy*m);
  particleposition.setZ(zz*m);

	G4PrimaryVertex* vertex = new G4PrimaryVertex(particleposition,particletime);
	
 
	G4ThreeVector momentum(pxx*GeV,pyy*GeV,pzz*GeV);
G4ParticleDefinition* particledefinition = G4MuonMinus::MuonMinusDefinition();
	G4PrimaryParticle* thePrimaryParticle =
    new G4PrimaryParticle(particledefinition,
			  pxx*GeV,
			  pyy*GeV,
			  pzz*GeV);
	vertex->SetPrimary(thePrimaryParticle);
  // vertex->SetWeight(Distribution(start_energy,start_costheta));  // Totally useless for now

  anEvent->AddPrimaryVertex(vertex);


  // Output information, maybe not so useful
   G4cout << "Entry #: " << ev_ID << G4endl;
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

//weighting with incident muon distribution
//https://escholarship.org/uc/item/6jm8g76d#page-3
double WeightedGenerator::Distribution(double Energy, double CosTheta)
{
  double CosThetaStar = sqrt(
	(pow(CosTheta,2) + pow(0.102573,2) -0.068287*pow(CosTheta,0.958633)+0.0407253*pow(CosTheta,0.817285) )/
	(1+pow(0.102573,2)-0.068287+0.0407253));

  double I;
  I = 0.14*pow(Energy*(1+3.64/(Energy*pow(CosThetaStar,1.29))),-2.7)
	  *((1./(1+(1.1*Energy*CosThetaStar)/115.))+(0.054/(1+(1.1*Energy*CosThetaStar)/850.)));

  return (I);
}
*/
