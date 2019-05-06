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
///////////////////////////////////////////////////////////////////////////
/// \file ShowerGenerator.hh
/// \brief Definition of the ShowerGenerator class

#ifndef ShowerGenerator_h
#define ShowerGenerator_h 1

#include "G4VPrimaryGenerator.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include <vector>
#include <string>

class G4Event;

class ShowerGenerator : public G4VPrimaryGenerator
{
  public:
    ShowerGenerator();
    virtual ~ShowerGenerator();

    // static access method
    // static const ShowerGenerator* Instance();

    // method from the base class
    virtual void GeneratePrimaryVertex(G4Event*);

    double Distribution(double Energy, double CosTheta);

    inline double GetStartEnergy(){return start_energy; }
    inline double GetStartCosTheta(){return start_costheta; }


  private:
    TFile *inputfile;
    TTree *datatree;

   double        start_energy;    //muon start energy in surface GeV
   double        start_costheta;    //muon start angle

   double particle_energy;
   double particle_momentumX;  //GeV
   double particle_momentumY;  //GeV
   double particle_momentumZ;  //GeV

  double weight; //Weighted probability of muon generation at surface
  
   int max_entries;

   G4ThreeVector particle_position;
   G4double particle_time;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

/*
#ifndef WeightedGenerator_h
#define WeightedGenerator_h 1

#include "G4VPrimaryGenerator.hh"
#include "globals.hh"

#include "TFile.h"
#include "TTree.h"
#include "TChain.h"
#include <vector>
#include <string>

class G4Event;

class WeightedGenerator : public G4VPrimaryGenerator
{
  public:
    WeightedGenerator();
    virtual ~WeightedGenerator();

    // static access method
    // static const WeightedGenerator* Instance();

    // method from the base class
    virtual void GeneratePrimaryVertex(G4Event*);

    double Distribution(double Energy, double CosTheta);

    inline double GetStartEnergy(){return startenergy; }
    inline double GetStartCosTheta(){return startcostheta; }


  private:
    TFile *inputfile;
    TTree *datatree;

   double        startenergy;    //muon start energy in surface GeV
   double        startcostheta;    //muon start angle

   double  *energy;
   double  *px;  //GeV
   double  *py;  //GeV
   double  *pz;  //GeV

   double *x;
   double *y;
   double *z;

   int max_entries;

   G4ThreeVector particleposition;
   G4double particletime;

};
#endif
*/
