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

//A more careful reimplementation of the trig cut

bool passcuts(double x, double y, double px, double py, double pz, double assemblyheight, double assemblyradius, double minimumheight)
{//Calculate if this muon will pass within 5m of
 //the detector assembly. Return true or false.
  //Within 5m of the assembly is the target volume.
  //Assumed to be cylindrical target.

  //There are two discrete possibilities:
  //1. The muon is directly above the target volume
  //2. It is not

  //In case 1, theoretically the muon can have
  //any azimuth angle phi, but the range of
  //zenith angles theta it can have is limited
  //and is also dependent on the phi value as
  //well as its relative position above the
  //target volume.

  //In case 2, the range of acceptable phi values
  //is limited, and the range of zenith values
  //is also slightly dependent on the muon's
  //trajectory relative to the center of the
  //target volume. More on this later.

  //First, assign variables

  double polarradius = TMath::Sqrt(x*x+y*y);
  //e.g. the horizontal component of a line conn-
  //ecting the center of the target volume with
  //the position of the particle

  double targetvolumeradius = assemblyradius+5;
  double targetvolumeheight = assemblyheight+10;

  //minimumheight is the distance of closest
  //approach of the sampling plane and the top
  //of the cylindrical target volume.
  //Right now, sampling plane is at 18*m
  //Target volume top is at 0+5*m
  double maximumheight = minimumheight+targetvolumeheight;

  double z = (minimumheight+maximumheight)/2;

  //We should already be able to determine whether
  //the particle is directly above the target
  //volume or not.

  bool isabove = false;
  if(polarradius < targetvolumeradius)
    isabove = true;
  //You can define the particle's angles in two ways

  //First, its theta and phi for its momentum
  //e.g. what direction it's heading
  //Second the theta and phi relative to the
  //center of the target volume, 'position angles'

  //I'm going to be converting radians to degrees so
  //I can do sanity checks, so I'll just define that
  //constant now.
  const double RtoD = 180/TMath::Pi();

  double momentumphi = TMath::ATan2(py,px);
  double momentumtheta = TMath::ACos(pz/TMath::Sqrt(px*px+py*py+pz*pz));
  double positionphi = TMath::ATan2(y,x);
  double positiontheta = TMath::ACos(z/TMath::Sqrt(x*x+y*y+z*z));

  //The reason positionphi is so useful is because
  //if we subtract it from every other phi angle
  //we use, we can effectively align the particle
  //to be along the x axis w.r.t. the taget volume
  //Similarly, we generally subtract the zenith
  //angle from 90 degrees to get the zenith angle
  //with respect to the PARTICLE, not the origin.
  //But notice we used ACos, not ASin, when
  //calculating theta, so only momentumtheta needs
  //to be corrected since pz is negative

  momentumtheta = TMath::Pi() - momentumtheta;

  if(positionphi<0)
    positionphi = positionphi + (TMath::Pi()*2);

  momentumphi = momentumphi - positionphi;
  if(momentumphi<0)
    momentumphi = momentumphi+(2*TMath::Pi());
  else if(momentumphi>(2*TMath::Pi()))
    momentumphi = momentumphi-(2*TMath::Pi());
  if(momentumphi<0)
    momentumphi = momentumphi+(2*TMath::Pi());


  //G4cout << G4endl << "Sanity check!" << G4endl << G4endl << "Position (x y z): " << x << " " << y << " " << z << G4endl << "Momentum (px py pz): " << px << " " << py << " " << pz << G4endl << "Momentum (phi theta): " << momentumphi*RtoD << " " << momentumtheta*RtoD << G4endl << "Position (phi theta): " << positionphi*RtoD << " " << positiontheta*RtoD << G4endl << G4endl << G4endl;

  //At this point, I think it's finally time to start calculations. Declare calculation variables:

  double realpath = 0;
  //realpath is the solvable. How far horizontally
  //can the particle travel before no longer being
  //"above" the target volume?
  //If we know this calculating the critical zenith
  //angle is trivial. Speaking of which...

  double maximumtheta = 0;
  //If the particle's momentumzenith exceeds this
  //value, it misses.
  //There is no minimumtheta for the r<R case.

  double epsilon = 0;
  double psi = 0;
  double phistar = 0;

  if(momentumphi<TMath::Pi())
    phistar = TMath::Pi() - momentumphi;//Complementary
  else
    phistar = momentumphi - TMath::Pi();

  //Trig calculations have to be done in radians

  
  //Forget the z axis exists, for now.
  //From above, the target volume is a circle.
  //After our offset corrections, we can say
  //polarradius (r) is a line along the x axis
  //(e.g. on phi=0) starting at the origin.
  //realpath is an imaginary line, with one end
  //on polarradius (the non-origin endpoint)
  //and the other end on the edge of the circle.
  //momentumphi determines which edge of the
  //circle the other end of realpath is on.
  //Draw a line connecting the origin with the
  //end of realpath that lies on the edge of the
  //circle. A line from the center to the edge
  //is, of course, the radius R. So we know the
  //length of two sides immediately: polarradius
  //and targetvolumeradius.
  
  //phistar is a complementary angle to phi
  //It is the angle between polarradius and
  //realpath

  //epsilon is the angle between realpath and the
  //targetvolumeradius

  //psi is the final angle, between polarradius
  //and targetvolumeradius, across from realpath

      //Use the side side angle trig method
  
      //Law of sines: sin(a)/A = sin(b)/B e.g.
      //a = arcsin(A*sin(b)/B)
      //We know targetvolumeradius and polarradius already
      epsilon = TMath::ASin(polarradius*TMath::Sin(phistar)/targetvolumeradius);
      
      psi = (TMath::Pi()) - epsilon - phistar;
      
      //Use law of sines again
      //sin(psi)/realpath = sin(phistar)/t.v.radius
      realpath = targetvolumeradius*TMath::Sin(psi)/TMath::Sin(phistar);

  
  //Time for another sanity check!
  //G4cout << G4endl << "Sanity check 2!" << G4endl << G4endl << "Radius: " << targetvolumeradius << G4endl << "Distance to particle along x axis: " << polarradius << G4endl << "Phistar: " << phistar*RtoD << G4endl << "Epsilon: " << epsilon*RtoD << G4endl << "psi: "<< psi*RtoD << G4endl << "Realpath: " << realpath << G4endl;

  //The previous calculations of parameters
  //are independent of whether the particle starts
  //directly above the target volume. However, the
  //actual limitations on the azimuth and zenith
  //angles will change. For a particle directly
  //above (r<R), there is no minimum zenith angle
  //but there is a maximum one.

  if(isabove)
    {
      //With the maximum allowable path
      //length in hand, we can calculate maximum
      //allowable zenith angle for r < R
      maximumtheta = TMath::ASin(realpath/TMath::Sqrt(realpath*realpath+minimumheight*minimumheight));
      
      //G4cout <<"INNER maximum allowable zenith: " << maximumtheta*RtoD << G4endl << "Calculated zenith: " << momentumtheta*RtoD << G4endl;

      if(momentumtheta<maximumtheta)
	{
	  //G4cout << "1" << G4endl;
	  return true;
	}      
      else
	{
	  //G4cout << "2" << G4endl;
	  return false;
	}
    }//if(isabove)

  else
    {//Particle is not directly above the assembly

      //This case is a bit trickier so I saved it
      //for last. In the previous situation, there
      //was no minimum or maximum phi, and no
      //minimum theta. However, all four of those
      //parameters are needed for this case.

      //Let's start with the phi's - we've already
      //calculated the parameters we need for that.

      double rho = TMath::ASin(targetvolumeradius/polarradius);
      //Draw a line, from the particle, tangent to
      //the circle. rho is the angle between the
      //polar radius and this line. You can draw
      //a second such line. Any phi between these
      //two lines is valid.
      double minimumphi = (TMath::Pi())-rho;
      double maximumphi = (TMath::Pi())+rho;

      //G4cout << "OUTER Minimum allowable azimuth: " << minimumphi*RtoD << G4endl << "Maximum allowable azimuth: " << maximumphi*RtoD << G4endl << "Calculated azimuth: " << momentumphi*RtoD << G4endl;

      if( momentumphi < minimumphi || momentumphi > maximumphi )
	{
	  //G4cout << "2" << G4endl;
	  return false;
	}

      else
	{//Easy azimuth cut - now zenith cut time


	  //The minimum allowable zenith angle is
	  //for a particle hitting the bottom cor-
	  //ner of the target volume on the near
	  //side. This is a maximum height,
	  //minimum horizontal distance scenario.
	  //We already calculated the maximum
	  //horizontal distance (realpath), the 
	  //minimum height and the maximum height.

	  //To calculate minimum path length, we
	  //need to calculate the chord that the
	  //particle would hypothetically carve out
	  //if it passed fully through the detector
	  //on its current trajectory.

	  //For maximum theta, do the opposite
	  //i.e. minimum height, maximum horizontal
	  //We know minimumheight

	  double chord=2*targetvolumeradius*TMath::Sin((TMath::Pi()/2)-epsilon);
	  //chord's argument has already been converted and halved
	  //G4cout << "Chord: " << chord << G4endl << "Epsilon: " << epsilon*RtoD << G4endl << "psi: " << psi*RtoD << G4endl << "phistar: " << phistar*RtoD << G4endl << G4endl;
	  //And subtract this from realpath
	  	  
	  double minimumtheta = TMath::ATan((realpath-chord)/maximumheight);

	  maximumtheta = TMath::ATan(realpath/minimumheight);
	  
	  //G4cout << "Minimum allowable zenith: " << minimumtheta*RtoD << G4endl << "Maximum allowable zenith: " << maximumtheta*RtoD << G4endl << "Calculated zenith: " << momentumtheta*RtoD << G4endl << G4endl;
	  
	  if(momentumtheta < minimumtheta || momentumtheta > maximumtheta)
	    {
	      //G4cout << "2" << G4endl;
	      return false;
	    }
	  else
	    {//It actually passed
	      //G4cout << "1" << G4endl;
	      return true;
	    }
	}//zenith cut

    }//is not above

}//passcuts





ShowerGenerator::ShowerGenerator()
{
  const char* muon_path = "$WORKINGDIR/mac";
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
  double zz = 17.999999;
  double comparator = 0;
  double comparee = 0;
  int counter = 0;
  int attemptsatmuon = 0;
  double energynorm = 0;

  while(!validmuon)
    {
      // Select random entry within file
      //Flat version (stable:) 
      //int ev_ID = (int)(G4UniformRand()*max_entries);
      comparator = G4UniformRand(); //should be between 0 and 1
      comparee = 0;
      counter = 0;
      
      //  G4cout << "Comparator: " << std::setprecision(20) << comparator << G4endl;
      while(comparee<comparator)
	{
	  
	  datatree->GetEntry(counter);
	  
	  comparee += weight; //weight is normalized so that total sum is 1.0 to high precision
	  if(counter>max_entries)
	    G4cout << G4endl << "Sampling method error" << G4endl; //should never happen
	  counter++;
	}
      
      //use the initial parameters of whichever muon was selected using comparator
      
      datatree->GetEntry(counter-1);
      //datatree->GetEntry(ev_ID);
      
      xx = (G4UniformRand()-0.5)*99.9;//from -49.95m to 49.95m
      yy = (G4UniformRand()-0.5)*99.9;
      
      attemptsatmuon++;
      validmuon = passcuts(xx,yy,particle_momentumX,particle_momentumY,particle_momentumZ,13,6.5,13);

    }//while !validmuon
  
  G4cout <<"It took " << attemptsatmuon << " attempts to sample a valid muon." << G4endl;
 
  //for all shower particles
  particle_time = 0.0*s;

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
  
  //Temporarily implemented as of March 2019

  //Retroactively make the energy a flat distribution from 0 to 20 TeV
  //First normalize the momenta so the particle thinks it's at 1 GeV  
  energynorm = TMath::Sqrt(px_MJD*px_MJD+py_MJD*py_MJD+pz_MJD*pz_MJD);
  px_MJD = px_MJD/energynorm;
  py_MJD = py_MJD/energynorm;
  pz_MJD = pz_MJD/energynorm;
  //Now multiple by a random number between 0 and 2000
  //Should technically be 200000, but I want statistics!dammit!
  energynorm = G4UniformRand()*2000;
  px_MJD = px_MJD*energynorm;
  py_MJD = py_MJD*energynorm;
  pz_MJD = pz_MJD*energynorm;
  energynorm = TMath::Sqrt(px_MJD*px_MJD+py_MJD*py_MJD+pz_MJD*pz_MJD);
  G4cout <<"Particle energy (in GeV): " << energynorm << G4endl;
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

bool oldpasscuts(double x, double y, double px, double py, double pz, double assemblyheight, double assemblyradius)
{//Calculate if this muon will pass within 5m of
 //the detector assembly. Return true or false.

  double r = TMath::Sqrt(x*x+y*y);//polar radius
  double R = assemblyradius+5;//Diameter will be +10
  double H = assemblyheight+10;
  double samplingheight = 13;//hardcoded...Bad
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




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

WeightedGenerator::WeightedGenerator()
{
  const char* muon_path = "$WORKINGDIR/mac";
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
