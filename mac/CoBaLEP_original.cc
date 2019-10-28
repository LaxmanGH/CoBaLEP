/// \file CoBaLEP.cc
/// \brief Main program of the  example

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

class T01TrackInformation : public G4VUserTrackInformation 
{
public:
  T01TrackInformation();
  T01TrackInformation(const G4Track* aTrack);
  T01TrackInformation(const T01TrackInformation* aTrackInfo);
  virtual ~T01TrackInformation();
   
  inline void *operator new(size_t);
  inline void operator delete(void *aTrackInfo);
  inline int operator ==(const T01TrackInformation& right) const
  {return (this==&right);}

  void Print() const;

private:
  G4int                 originalTrackID;
  G4ParticleDefinition* particleDefinition;
  G4ThreeVector         originalPosition;
  G4ThreeVector         originalMomentum;
  G4double              originalEnergy;
  G4double              originalTime;
  int generation = 0;

public:
  inline G4int GetOriginalTrackID() const {return originalTrackID;}
  inline G4ParticleDefinition* GetOriginalParticle() const {return particleDefinition;}
  inline G4ThreeVector GetOriginalPosition() const {return originalPosition;}
  inline G4ThreeVector GetOriginalMomentum() const {return originalMomentum;}
  inline G4double GetOriginalEnergy() const {return originalEnergy;}
  inline G4double GetOriginalTime() const {return originalTime;}
};

G4Allocator<T01TrackInformation> aTrackInformationAllocator;

inline void* T01TrackInformation::operator new(size_t)
{ void* aTrackInfo;
  aTrackInfo = (void*)aTrackInformationAllocator.MallocSingle();
  return aTrackInfo;
}

inline void T01TrackInformation::operator delete(void *aTrackInfo)
{ aTrackInformationAllocator.FreeSingle((T01TrackInformation*)aTrackInfo);}




T01TrackInformation::T01TrackInformation()
{
  originalTrackID = 0;
  particleDefinition = 0;
  originalPosition = G4ThreeVector(0.,0.,0.);
  originalMomentum = G4ThreeVector(0.,0.,0.);
  originalEnergy = 0.;
  originalTime = 0.;
}

T01TrackInformation::T01TrackInformation(const G4Track* aTrack)
{
  originalTrackID = aTrack->GetTrackID();
  particleDefinition = aTrack->GetDefinition();
  originalPosition = aTrack->GetPosition();
  originalMomentum = aTrack->GetMomentum();
  originalEnergy = aTrack->GetTotalEnergy();
  originalTime = aTrack->GetGlobalTime();
}

T01TrackInformation::T01TrackInformation(const T01TrackInformation* aTrackInfo)
{
  originalTrackID = aTrackInfo->originalTrackID;
  particleDefinition = aTrackInfo->particleDefinition;
  originalPosition = aTrackInfo->originalPosition;
  originalMomentum = aTrackInfo->originalMomentum;
  originalEnergy = aTrackInfo->originalEnergy;
  originalTime = aTrackInfo->originalTime;
  generation++;
}

T01TrackInformation::~T01TrackInformation(){;}

void T01TrackInformation::Print() const
{
    G4cout 
      << "Original track ID " << originalTrackID 
      << " at " << originalPosition << G4endl;
}


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
  void WriteNuclear(G4Track *track);
  void Close();

  //output files
  TFile *fFile;
  TTree *fTree;

  TFile *nuclearFile;
  TTree *nuclearTree;

  //variables to write to output files
  int         PID;
  int         primaryPID;
//Track info only: the PID of the nucleus before the current one in this decay chain. Is 0 for a primary nucleus

  int parentnucleusPID;
//Step info only: the PID of the nucleus this particle originated from. Is 0 if the particle didn't originate from a nucleus. Is persistent across multiple secondaries. e.g. if a nucleus creates a positron which creates two photons, the positron and the photons will all have the same parentnucleusPID

  int         ParentTrackID;
  double      energy;
  double      kineticenergy;
  double      time;
  double      x;
  double      y;
  double      z;
  double      px;
  double      py;
  double      pz;
  double      muonx;
  double      muony;
  double      muonz;
  double      muonpx;
  double      muonpy;
  double      muonpz;
  double      muonenergy;
  int         eventnumber;
  int         stepnumber;
  double      steplength;
  int         tracknumber;
  int         detectornumber;
  int         isentryevent;
  std::string creatorprocess;
  std::string material;
  double      startx;
  double      starty;
  double      startz;
  int         randomseed;
  double      nuclearx;  
  double      nucleary;  
  double      nuclearz;  
  double startenergy;

  //variables specifically to parse detector number from detector name (used in the Write method)
  G4String    name;
  G4String    num;
  int         j;


  char        outputfilename[100];
  char        nuclearfilename[100];
};



IO::IO(): G4UImessenger()  {    G4cout << "IO created" << G4endl; }


void IO::Open(char* inputseed)
{
  
  //Name output file based on the seed number given at the start of the program
  char prefix[15] = "output";
  char suffix[10] = ".root";
  
  strcat(outputfilename, prefix);
  strcat(outputfilename, inputseed);
  strcat(outputfilename, suffix);

  G4cout << outputfilename << G4endl;
  
  fFile = new TFile(outputfilename,"recreate");
  fTree = new TTree("fTree","fTree");

  char nuclearprefix[12] = "nuclearinfo";

  strcat(nuclearfilename, nuclearprefix);
  strcat(nuclearfilename, inputseed);
  strcat(nuclearfilename, suffix);

  G4cout << nuclearfilename << G4endl;

  nuclearFile = new TFile(nuclearfilename,"recreate");
  nuclearTree = new TTree("nuclearTree","nuclearTree");

  //Branches to be filled
  fTree->Branch("PID",&PID,"PID/I");
  //PDG encoding particle ID
  fTree->Branch("ParentTrackID",&ParentTrackID,"ParentTrackID/I");
  fTree->Branch("energy",&energy,"energy/D");
  fTree->Branch("kineticenergy", &kineticenergy, "kineticenergy/D");
  fTree->Branch("time",&time,"time/D");
  fTree->Branch("x",&x,"x/D");
  fTree->Branch("y",&y,"y/D");
  fTree->Branch("z",&z,"z/D");
  fTree->Branch("px",&px,"px/D");
  fTree->Branch("py",&py,"py/D");
  fTree->Branch("pz",&pz,"pz/D");
  fTree->Branch("muonx",&muonx,"muonx/D");
  fTree->Branch("muony",&muony,"muony/D");
  fTree->Branch("muonz",&muonz,"muonz/D");
  fTree->Branch("muonpx",&muonpx,"muonpx/D");
  fTree->Branch("muonpy",&muonpy,"muonpy/D");
  fTree->Branch("muonpz",&muonpz,"muonpz/D");
  fTree->Branch("muonenergy",&muonenergy,"muonenergy/D");
  fTree->Branch("eventnumber",&eventnumber,"eventnumber/I");
  fTree->Branch("stepnumber",&stepnumber,"stepnumber/I");
  fTree->Branch("steplength",&steplength,"steplength/D");
  fTree->Branch("tracknumber",&tracknumber,"tracknumber/I");
  fTree->Branch("detectornumber",&detectornumber,"detectornumber/I");
  //fTree->Branch("isentryevent", &isentryevent, "isentryevent/I");
  fTree->Branch("creatorprocess", &creatorprocess);
  fTree->Branch("startx",&startx,"startx/D");
  fTree->Branch("starty",&starty,"starty/D");
  fTree->Branch("startz",&startz,"startz/D");
  fTree->Branch("randomseed",&randomseed,"randomseed/I");
  fTree->Branch("parentnucleusPID",&parentnucleusPID,"parentnucleusPID/I");
  fTree->Branch("nuclearx",&nuclearx,"nuclearx/D");
  fTree->Branch("nucleary",&nucleary,"nucleary/D");
  fTree->Branch("nuclearz",&nuclearz,"nuclearz/D");
  randomseed = atoi(inputseed);

  nuclearTree->Branch("material", &material);
  nuclearTree->Branch("PID",&PID,"PID/I");
  nuclearTree->Branch("x",&x,"x/D");
  nuclearTree->Branch("y",&y,"y/D");
  nuclearTree->Branch("z",&z,"z/D");
  nuclearTree->Branch("muonpx",&muonpx,"muonpx/D");
  nuclearTree->Branch("muonpy",&muonpy,"muonpy/D");
  nuclearTree->Branch("muonpz",&muonpz,"muonpz/D");
  nuclearTree->Branch("tracknumber",&tracknumber,"tracknumber/I");
  nuclearTree->Branch("randomseed",&randomseed,"randomseed/I");
  nuclearTree->Branch("primaryPID",&primaryPID,"primaryPID/I");
  nuclearTree->Branch("creatorprocess",&creatorprocess);
  nuclearTree->Branch("startenergy",&startenergy,"startenergy/D");
  nuclearTree->Branch("time",&time,"time/D");
  G4cout << "Output files opened." << G4endl;

}


void IO::Write(G4Track *track, int isadetector, int isentry)
{
  
  PID = track->GetDefinition()->GetPDGEncoding();
  ParentTrackID = track->GetParentID();
  energy = track->GetStep()->GetTotalEnergyDeposit()/CLHEP::keV;
  kineticenergy = track->GetDynamicParticle()->GetKineticEnergy()/CLHEP::keV;
  time = track->GetGlobalTime()/CLHEP::ns;

  if(isentry==0) //Step did NOT start in LAr and end up in Ge
    {
      x = track->GetStep()->GetPreStepPoint()->GetPosition().x()/CLHEP::mm;
      y = track->GetStep()->GetPreStepPoint()->GetPosition().y()/CLHEP::mm;
      z = track->GetStep()->GetPreStepPoint()->GetPosition().z()/CLHEP::mm;
      material = track->GetStep()->GetPreStepPoint()->GetMaterial()->GetName();
    }

  if(isentry==1) //Step started in LAr and ended in Ge; so called 'entry event'
    {//Turns out this is defunct. I left it in for legacy purposes.
      x = track->GetStep()->GetPostStepPoint()->GetPosition().x()/CLHEP::mm;
      y = track->GetStep()->GetPostStepPoint()->GetPosition().y()/CLHEP::mm;
      z = track->GetStep()->GetPostStepPoint()->GetPosition().z()/CLHEP::mm;
      material = track->GetStep()->GetPostStepPoint()->GetMaterial()->GetName();
    }
  px = track->GetMomentumDirection().x();
  py = track->GetMomentumDirection().y();
  pz = track->GetMomentumDirection().z();

  muonx = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetX0()/CLHEP::mm;
  muony = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetY0()/CLHEP::mm;
  muonz = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetZ0()/CLHEP::mm;

  muonpx = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetPx()/CLHEP::keV;
  muonpy = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetPy()/CLHEP::keV;
  muonpz = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetPz()/CLHEP::keV;
  
  muonenergy = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetKineticEnergy()/CLHEP::keV;
  
  eventnumber = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetEventID();
  stepnumber = savedsteps;
  steplength = track->GetStep()->GetStepLength()/CLHEP::mm;
  tracknumber = track->GetTrackID();
  isentryevent = isentry;
  if(PID!=13) //muons have no creator process and cause a seg fault
    creatorprocess = track->GetCreatorProcess()->GetProcessName();
  else
    creatorprocess = "None";

  startx = track->GetVertexPosition().x()/CLHEP::mm;
  starty = track->GetVertexPosition().y()/CLHEP::mm;
  startz = track->GetVertexPosition().z()/CLHEP::mm;
  
  if(isadetector==1)
    {
      //below steps just used to parse detector number from detector name
      if(isentry==0)
	name = track->GetStep()->GetPreStepPoint()->GetPhysicalVolume()->GetName();
      if(isentry==1)
	name = track->GetStep()->GetPostStepPoint()->GetPhysicalVolume()->GetName();
      
      j=0;
      num ="";  
      for(std::string::size_type i = 0; i < name.size(); ++i)
	{

	  if(isdigit(name[i]))
	    {
	      num+=name[i];
	      j++;
	    }//if(isdigit...
           
	}//for(std...
 
      detectornumber = atoi(num);
    }//if(isadetector==1

  if(isadetector==0)
    {
      detectornumber = 1181; //Used in implementation of LAr as its own detector
      
    }//if(isadetector==0

  if(track->GetUserInformation())
    {//If this particle originated from a nucleus
      T01TrackInformation* info = (T01TrackInformation*)(track->GetUserInformation());
      parentnucleusPID = info->GetOriginalParticle()->GetPDGEncoding();
      nuclearx = info->GetOriginalPosition().x();
      nucleary = info->GetOriginalPosition().y();
      nuclearz = info->GetOriginalPosition().z();
  }
  else
    parentnucleusPID = nuclearx = nucleary = nuclearz = 0;

  fTree->Fill();
  
} //Io::Write

void IO::WriteNuclear(G4Track *track)
{
  //PID = track->GetDefinition()->GetPDGEncoding();;
   if(track->GetUserInformation())
     {//If particle is a nuclear DAUGHTER of a nucleus
      T01TrackInformation* info = (T01TrackInformation*)(track->GetUserInformation());
      primaryPID = info->GetOriginalParticle()->GetPDGEncoding();//Only tracks back one isotope
    }
   else//Is a primary nucleus
    primaryPID = 0;

  PID = track->GetDefinition()->GetPDGEncoding();  
  creatorprocess = track->GetCreatorProcess()->GetProcessName();  
  //Why did I remove this?
  material = track->GetLogicalVolumeAtVertex()->GetMaterial()->GetName();
  x = track->GetPosition().x();
  y = track->GetPosition().y();
  z = track->GetPosition().z();
  tracknumber = track->GetTrackID();
  muonpx = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetPx()/CLHEP::keV;
  muonpy = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetPy()/CLHEP::keV;
  muonpz = G4EventManager::GetEventManager()->GetConstCurrentEvent()->GetPrimaryVertex()->GetPrimary()->GetPz()/CLHEP::keV;
  startenergy = track->GetKineticEnergy();
  time = track->GetGlobalTime()/CLHEP::ns;


  nuclearTree->Fill();
}

void IO::Close()
{
  fFile->cd();
  fTree->Write();
  fFile->Close();
  nuclearFile->cd();
  nuclearTree->Write();
  nuclearFile->Close();


  G4cout << "Output files written and closed." << G4endl;
  
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
    if(aTrack->GetDefinition()->GetPDGEncoding()>10000000&&aTrack->GetDefinition()->GetPDGEncoding()!=1000080160&&aTrack->GetDefinition()->GetPDGEncoding()!=1000140280)//~?
      {//Particle is a nucleus, its secondaries should be tagged

	//if(aTrack->GetUserInformation())
	//{	 
	//  G4cout <<"Daughter nuclei found!"<< G4endl;
	//  T01TrackInformation* orinfo = (T01TrackInformation*)(aTrack->GetUserInformation());
	//  G4cout << orinfo->GetOriginalParticle()->GetPDGEncoding() << G4endl << G4endl;
	//  G4cout << aTrack->GetDefinition()->GetPDGEncoding() << G4endl;
	//  }
	G4Track* theTrack = (G4Track*)aTrack;
	fio->WriteNuclear(theTrack);//Done BEFORE TrackInformation is assigned	
	T01TrackInformation* anInfo = new T01TrackInformation(aTrack);//When declaring from a TRACK, original particle information is overwritten
	//This means that in a long nuclear decay chain, the information about which nucleus started it is lost...
	theTrack->SetUserInformation(anInfo);
	}
  }

  void PostUserTrackingAction(const G4Track* aTrack)
  {
    if(aTrack->GetUserInformation())
      {
	G4TrackVector* secondaries = fpTrackingManager->GimmeSecondaries();
	if(secondaries)
	  {
	    T01TrackInformation* info = (T01TrackInformation*)(aTrack->GetUserInformation());
	    size_t nSeco = secondaries->size();
	    if(nSeco>0)
	      {
		for(size_t i=0;i<nSeco;i++)
		  { 
		    T01TrackInformation* infoNew = new T01TrackInformation(info);
		    //When declaring from an INFO, original particle information is kept
		    (*secondaries)[i]->SetUserInformation(infoNew);
		  }
	      }
	  }//if(secondaries
      }//if(aTrack->GetUser
  }//PostUser



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

  //Member functions
  //void EndOfEventAction(const G4Event*)
  //{
  //  G4cout <<"WRINKLY BEANS 9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999"<<G4endl <<G4endl << G4endl << G4endl;
  //}
};
       

//void EventAction::UserEventAction(const G4Event *event)
//{
//  G4cout << "Wrinkly toes 9999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999999" << G4endl << G4endl << G4endl;
//}
//maybe changing event number should go here? May be extraneous

//////////////////////////////////////////////////

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
  
  int PID =step->GetTrack()->GetDefinition()->GetPDGEncoding(); 
  G4String material;
  if(PID!=13)
    material = step->GetPreStepPoint()->GetMaterial()->GetName();  
  else
    material = "Muonic";
//cuts go here
  currentsteps++;

  if(savedsteps<maxsteps)
    {//hits inside detector detection
      
      G4String whatgeometry = step->GetPreStepPoint()->GetPhysicalVolume()->GetName();

      const G4String detectornames = "phy_Det";
      const G4String fillgasname = "phy_FillGas"; //LAr surrounding detector
      const G4String liquidargon = "phy_liquidArgon";
      
      //if(material!="G4_AIR"&&material!="Rock")
      //{
	  //T01TrackInformation* info = (T01TrackInformation*)(step->GetTrack()->GetUserInformation());
	  //if(info)
	    //G4cout << material << G4endl << PID << G4endl << info->GetOriginalParticle()->GetPDGEncoding() << G4endl << G4endl;
	    //}//Material

      if(material=="GermaniumEnriched")//~strstr(whatgeometry.c_str(),detectornames.c_str())) //if pre step point originates in a detector
	{
	  fio->Write(step->GetTrack(), 1, 0); //non-entry event
	 savedsteps++;
	}//if(strstr

    }   //if maxsteps

  //if(currentsteps>=maxsteps)
  //  {
  //    G4cout << G4endl << "Simulation is too large. Force exit." << G4endl;
  //    return;
  //  }
	    
  
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
