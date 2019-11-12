/// \file CoBaLEP.cc
/// \brief Main program of the  example
//MCNP comparison version
#include "DetectorConstruction.hh"
#include "PrimaryGeneratorAction.hh"
#include <G4GeneralParticleSource.hh>

#include "G4RunManager.hh"
#include "G4UImanager.hh"

#include "Shielding.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif

#include "Randomize.hh"
#include <G4UserRunAction.hh>
#include <G4UImessenger.hh>
#include <TFile.h>
#include <TTree.h>
#include <G4UserEventAction.hh>
#include <CLHEP/Units/SystemOfUnits.h>
#include "G4EventManager.hh"
#include <iostream>
#include <time.h>
#include<stdlib.h>
#include <typeinfo>
#include "G4UserTrackingAction.hh"
#include "globals.hh"
#include "G4TrackingManager.hh"
#include "G4Track.hh"
#include "G4TrackVector.hh"
//#include "T01TrackInformation.hh"
#include "G4Allocator.hh"
#include "G4VUserTrackInformation.hh"
#include "G4ios.hh"
#include "G4GDMLParser.hh"

//This module requires a random seed to be input after the first macro argument
//It uses C++'s class system to safely pass variables between different methods
//This version also has an option for hits in the LAr as a channel. It's currently broken.
//There is NO summing in this version. Cuts are implemented on a case by case basis.


//Some templates copied directly from the Geant4 Users' Guide

const int maxsteps=100000000;
int currentsteps=0; //global ints just to keep track of file size limits
int savedsteps = 0;

//////////////////////////////////////////////////

//Class used to store and retrieve user-defined information from the track of the particle.
//Currently the Userinformation stores information about the nucleus which created the particle (if any).


//mediator class IO that all action classes incorporate, to pass information for writing etc.

class IO : public G4UImessenger
{

public:

  //constructor and destructor
  IO();
  ~IO(){};

  //member functions
  void Open(char* inputseed);
  void Write(G4Track *track, int isadetector, int isentry);
  void Close();

  //output files
  TFile *fFile;
  TTree *fTree;

  //variables to write to output files
  int         PID;
  double      kineticenergy;
  double      px;
  double      py;
  double      pz;
  double      x;
  double      y;
  double      z;
  int         eventnumber;
  int         tracknumber;
  std::string endboundary;
  int         randomseed;
  double startenergy;

  char        outputfilename[100];
};



IO::IO(): G4UImessenger()  {    G4cout << "IO created" << G4endl; }


void IO::Open(char* inputseed)
{
  
  //Name output file based on the seed number given at the start of the program
  char prefix[15] = "boundaryoutput";
  char suffix[10] = ".root";
  
  strcat(outputfilename, prefix);
  strcat(outputfilename, inputseed);
  strcat(outputfilename, suffix);

  G4cout << outputfilename << G4endl;
  
  fFile = new TFile(outputfilename,"recreate");
  fTree = new TTree("fTree","fTree");

  //Branches to be filled
  fTree->Branch("PID",&PID,"PID/I");
  //PDG encoding particle ID
  fTree->Branch("kineticenergy", &kineticenergy, "kineticenergy/D");
  fTree->Branch("px",&px,"px/D");
  fTree->Branch("py",&py,"py/D");
  fTree->Branch("pz",&pz,"pz/D");
  fTree->Branch("x",&x,"x/D");
  fTree->Branch("y",&y,"y/D");
  fTree->Branch("z",&z,"z/D");
  fTree->Branch("eventnumber",&eventnumber,"eventnumber/I");
  fTree->Branch("tracknumber",&tracknumber,"tracknumber/I");
  fTree->Branch("randomseed",&randomseed,"randomseed/I");
  fTree->Branch("endboundary", &endboundary);
  fTree->Branch("startenergy",&startenergy,"startenergy/D");
  G4cout << "Output files opened." << G4endl;
  randomseed = atoi(inputseed);
}


void IO::Write(G4Track *track, int isadetector, int isentry)
{
  
  PID = track->GetDefinition()->GetPDGEncoding();
  kineticenergy = track->GetDynamicParticle()->GetKineticEnergy()/CLHEP::keV;
  
  x = track->GetStep()->GetPostStepPoint()->GetPosition().x()/CLHEP::mm;
  y = track->GetStep()->GetPostStepPoint()->GetPosition().y()/CLHEP::mm;
  z = track->GetStep()->GetPostStepPoint()->GetPosition().z()/CLHEP::mm;

  px = track->GetMomentumDirection().x();
  py = track->GetMomentumDirection().y();
  pz = track->GetMomentumDirection().z();
  
  startenergy = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy()/CLHEP::keV;
  
  eventnumber = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  tracknumber = track->GetTrackID();
endboundary = track->GetStep()->GetPostStepPoint()->GetPhysicalVolume()->GetName();

  fTree->Fill();
  
} //Io::Write

void IO::Close()
{
  fFile->cd();
  fTree->Write();
  fFile->Close();

  G4cout << "Output file written and closed." << G4endl;
  
}

//////////////////////////////////////////////////

 class RunAction: public G4UserRunAction 
{
     
public:
     
  // Constructor and destructor
  RunAction(IO *io=0, char* inputseed="0") : G4UserRunAction(), fio(io), inputseed(inputseed){};
  ~RunAction(){};

  //Member functions
  IO* fio;
  double t1;
  char* inputseed;
  
  void BeginOfRunAction(const G4Run*)
  {
    G4cout << G4endl <<  "Beginning run, starting timer" << G4endl;
    t1=double(clock());
    fio->Open(inputseed);
  } //Initialize TTree, open TFile, get clock going

  void EndOfRunAction(const G4Run*)
  {
    fio->Close();
    t1=(double(clock())-t1);
    G4cout << G4endl << "Time:   " << t1/1000000 << G4endl << "steps simulated: " << currentsteps << G4endl << "steps saved: " << savedsteps << G4endl << G4endl;
  } //Write TTree, close TFile, stop clock

 };
       
//////////////////////////////////////////////////

class TrackingAction: public G4UserTrackingAction 
{
     
  //--------
public:
  //--------
     
  // Constructor and destructor
  TrackingAction(IO *io=0) : G4UserTrackingAction(), fio(io){};
  TrackingAction() : G4UserTrackingAction(){};
  ~TrackingAction(){};
  
    //Member functions
  IO *fio;
  void UserTrackingAction();
  void PreUserTrackingAction(const G4Track* aTrack)
  {
    if(aTrack->GetTrackID()>1)//~~Particle is a secondary
      {
      }
  }

};//TrackingAction
       

/////////////////////////////////////////////////

class EventAction: public G4UserEventAction 
//Not currently implemented for anything, but this structure should work in case we need it later.
{
     
  //--------
public:
  //--------
     
  // Constructor and destructor
  EventAction(){};
  ~EventAction(){};
};

class SteppingAction: public G4UserSteppingAction 
{
     
public:

  // Constructor and destructor

  SteppingAction(IO *io=0) : G4UserSteppingAction(), fio(io){};
  ~SteppingAction(){};

  //Member functions
  IO* fio;
  void UserSteppingAction(const G4Step* step);

}; //class steppingaction
       

void SteppingAction::UserSteppingAction(const G4Step* step)
{
  
  currentsteps++;

  if(savedsteps<maxsteps)
    {//hits inside detector detection

      
      //G4cout << step->GetTrack()->GetTrackID() << G4endl;
      const G4VPhysicalVolume* prePoint = step->GetPreStepPoint()->GetPhysicalVolume();
      const G4VPhysicalVolume* postPoint = step->GetPostStepPoint()->GetPhysicalVolume();

      if(step->GetTrack()->GetTrackID()>1)//Particle is a secondary
	step->GetTrack()->SetTrackStatus(fStopAndKill);

      else if(prePoint&&postPoint)
	{//Make sure both points exist first...
	  if(prePoint->GetName()!=postPoint->GetName())
	    {//boundary change
	      fio->Write(step->GetTrack(), 1, 0);
	      savedsteps++;
	      }
	}

    }   //if maxsteps
  
}	     //void SteppingAction

//////////////////////////////////////////////////
//////////////////////////////////////////////////


int main(int argc,char** argv)
{

  // Choose the Random engine
  //1st argument of argv is the particle source macro (usually weighted.mac)
  //2nd argument is a random number seed
  char* inputseed;
  inputseed= argv[2]; //This is not an optional argument. A null value for this crashes the program
  G4cout << G4endl << "Input seed: " <<  inputseed << G4endl;
  G4String s = inputseed;
  int ranengine = atoi(s);
  CLHEP::HepRandom::setTheEngine(new CLHEP::RanecuEngine);
  CLHEP::HepRandom::setTheSeed(ranengine);

  // Construct the default run manager
  //
  G4RunManager * runManager = new G4RunManager;

  // Set mandatory initialization classes
  //
  // Detector construction
  DetectorConstruction* detector = new DetectorConstruction();
  runManager->SetUserInitialization(detector);

  // Physics list
  runManager->SetUserInitialization(new Shielding);

  // Primary generator action
  PrimaryGeneratorAction* gen_action = new PrimaryGeneratorAction();
  runManager->SetUserAction(gen_action);

  //Custom implemented classes
  IO *io = new IO;
  runManager->SetUserAction(new RunAction(io, inputseed));
  runManager->SetUserAction(new EventAction());  
  runManager->SetUserAction(new TrackingAction(io));  
  runManager->SetUserAction(new SteppingAction(io));

  
  // Initialize G4 kernel
  //
  runManager->Initialize();

  //Comment out when not actively building geometries
  //G4VPhysicalVolume* W = G4TransportationManager::GetTransportationManager()->GetNavigatorForTracking()->GetWorldVolume();
  //G4GDMLParser parser;
  //parser.Write("geom/Suspicious2018.gdml", W, false);

#ifdef G4VIS_USE
  // Initialize visualization
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif

  // Get the pointer to the User Interface manager
   G4UImanager* UImanager = G4UImanager::GetUIpointer(); 

  if (argc!=1) {
    // batch mode
    G4String command = "/control/execute ";
    G4String fileName = argv[1];

    UImanager->ApplyCommand(command+fileName);
  }
  else {
    // interactive mode : define UI session
    inputseed="413413413";
#ifdef G4UI_USE
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
    //    UImanager->ApplyCommand("/control/execute vis.mac");
#endif
    ui->SessionStart();
    delete ui;
#endif
  }

  // Job termination
  // Free the store: user actions, physics_list and detector_description are
  // owned and deleted by the run manager, so they should not be deleted
  // in the main() program !

#ifdef G4VIS_USE
  delete visManager;
#endif
  delete runManager;

  return 0;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo.....
