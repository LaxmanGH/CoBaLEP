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
/// \file DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class

#include "DetectorConstruction.hh"
#include "DetectorMessenger.hh"

// use of stepping action to set the accounting volume

#include "G4RunManager.hh"
#include "G4NistManager.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Cons.hh"
#include "G4Orb.hh"
#include "G4Sphere.hh"
#include "G4Trd.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4PhysicalConstants.hh"
#include "G4Polyhedra.hh"
#include "G4Polycone.hh"
#include "G4SubtractionSolid.hh"
#include "G4Isotope.hh"
#include "G4Element.hh"
#include "G4Torus.hh"
#include "G4UnionSolid.hh"

#include "G4String.hh"
#include "math.h"
#include "TMath.h"

#include "G4VisAttributes.hh"
#include "G4Color.hh"
#include "G4Colour.hh"

#include "G4Material.hh"
#include "G4UIcommand.hh"

#ifndef M_PI
#define M_PI    3.14159265358979323846f
#endif

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction()
{
  detectorMessenger = new DetectorMessenger(this);
  detector_type = "GERDA";
  innerVessel_FillMaterial = "ArgonLiquid";
  checkOverlaps = false;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{
  delete detectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{

  //Geometry options are: September2018WithCuts (default), OriginalWithRock, NewRock, NoRock,
  //and NeutronMultiplicityValidation (for neutron study only)
  //March2019WithCuts (not currently used)
  //TrigStudy (for trig efficiency calculations)
  //LArColumn (for LAr showering studies) and LArStudy (for pathlength studies)
  //pCDR for the most up-to-date version of the pCDR geometry

  //If possible, import an existing geometry using GDML
  //G4String inputgeometry = "/home/usd.local/cj.barton/workingfolder/geom/September2018WithCuts.gdml";
  //inputparser.Read(inputgeometry, false);
  //G4VPhysicalVolume* W = inputparser.GetWorldVolume();
  //W->GetLogicalVolume()->SetVisAttributes(G4VisAttributes::Invisible);

  //return W;


  
  //Otherwise, build one manually
  
  #include "Detector_Materials.icc"
  

  // World

  G4Box* solid_World = new G4Box("sol_World",50*m,50*m,18*m);
  G4LogicalVolume* logical_World = new G4LogicalVolume(solid_World,mat_vacuum,"log_World");
  logical_World->SetVisAttributes (G4VisAttributes::Invisible);
  G4VPhysicalVolume* physical_World = new G4PVPlacement(0,G4ThreeVector(),logical_World,"phy_World",0,false,0,checkOverlaps);
  G4String detector_name;

  G4double assemblyzoffset = -6.5*m;
  //for displacing the detector assembly from the center of the world
  //May change with different geometry selections
  
  G4String RockOption = "pCDR";
  G4String LArScintillators = "NotImplementedYet";
  G4String DetectorComposition = "NotImplementedYet";
	
	
  #include "Detector_CJStyle.icc"
  //#include "Detector_AlStyle.icc"

  //fill gas

  G4Tubs* solid_FillGas = new G4Tubs("sol_FillGas", 0, 0.94999*m ,0.94999*m, 0, 2*M_PI);
  G4LogicalVolume* logical_FillGas = new G4LogicalVolume(solid_FillGas, mat_FillGas, "log_FillGas" );
  logical_FillGas->SetVisAttributes ( new G4VisAttributes(G4Colour(0.5, 0.6, 0.5, 0.5) )); //grey 50% transparent
  G4VPhysicalVolume* physical_FillGas =
    new G4PVPlacement(0,G4ThreeVector(0,0,assemblyzoffset),logical_FillGas,"phy_FillGas",logical_World,false,0,checkOverlaps);


  // Detector placement

  G4double detector_radius = 35*mm;
  G4double detector_height = 50*mm;

  G4Tubs* solid_DetGeCrystal = new G4Tubs("sol_DetGeCrystal", 0*mm, detector_radius ,detector_height/2., 0, 2*M_PI);
  G4LogicalVolume* logical_DetGeCrystal = new G4LogicalVolume(solid_DetGeCrystal, mat_Detector, "log_DetGeCrystal" );
  logical_DetGeCrystal->SetVisAttributes ( new G4VisAttributes(G4Colour(0.1,0.9,0.1)) );

  int layer;
  int tower;
  int crystal;

  G4double detector_verticalGap        = 2*cm;
  G4double detector_horizontalGapSmall = 0.5*cm;  //in between one tower
  G4double detector_horizontalGapBig   = 4*cm;  //between the towers
  G4double detector_dX;
  G4double detector_dY;
  G4double detector_dZ;

  int tower_indexX[30] = {-4, 0, 4, -6,-2,2,6, -8,-4,0,4,8, -10,-6,-2,2,6,10, -8,-4, 0, 4, 8, -6,-2, 2, 6,  -4,  0,  4};
  int tower_indexY[30] = {12,12,12,  8, 8,8,8,  4, 4,4,4,4,   0, 0, 0,0,0, 0, -4,-4,-4,-4,-4, -8,-8,-8,-8, -12,-12,-12};

  //30 towers a 4 crystals in one layer !
  for (int i = 0; i<1080; i++){
    //for (int i = 0; i<2; i++){
    layer = 4-i/120;
    tower = (i%120)/4;
    crystal = (i%120)%4;
    detector_dX = tower_indexX[tower]*detector_radius + (2*tower_indexX[tower]/4)*detector_horizontalGapBig;
    detector_dY = tower_indexY[tower]*detector_radius + (tower_indexY[tower]/4)*detector_horizontalGapBig;
    detector_dZ = layer*(2*detector_height+detector_verticalGap);

    //in tower:
    //
    // 		0  1
    //		3  2
    //

    switch(crystal){
    case 0:
      detector_dX += -detector_radius-detector_horizontalGapSmall;
      detector_dY += +detector_radius+detector_horizontalGapSmall;
      break;
    case 1:
      detector_dX += +detector_radius+detector_horizontalGapSmall;
      detector_dY += +detector_radius+detector_horizontalGapSmall;
      break;
    case 2:
      detector_dX += +detector_radius+detector_horizontalGapSmall;
      detector_dY += -detector_radius-detector_horizontalGapSmall;
      break;
    case 3:
      detector_dX += -detector_radius-detector_horizontalGapSmall;
      detector_dY += -detector_radius-detector_horizontalGapSmall;
      break;
    }
    detector_name = "phy_DetGeCrystal_";
    detector_name += G4UIcommand::ConvertToString(i);
    //G4cout << i << " " << tower << " "  << detector_dX << " " << detector_dY<< " " << detector_dZ<< G4endl;
    G4ThreeVector detector_position = G4ThreeVector(detector_dX,detector_dY,detector_dZ) ;
    new G4PVPlacement(0,detector_position,logical_DetGeCrystal,detector_name,logical_FillGas,false,i,checkOverlaps);

  }


  //inner Vessel

  //The inner vessel is shaped like a coffee container. Since I
  //can't make this shape now that G4Polycone is deprecated
  //or something, I'll make two disks with a hollow tube between

  //First, the cylinder
  G4Tubs* solid_innerVessel_1 = new G4Tubs("sol_innervVessel_1",.950001*m,.999999*m,.999999*m,0,2*M_PI);
  G4LogicalVolume* logical_innerVessel_1 = new G4LogicalVolume(solid_innerVessel_1, mat_Cu, "log_innerVessel_1" );
  logical_innerVessel_1->SetVisAttributes ( new G4VisAttributes(G4Colour(0.62, 0.3, 0.2,0.7) ));//brown of course
  G4VPhysicalVolume* physical_innerVessel_1 = new G4PVPlacement(0,G4ThreeVector(0,0,assemblyzoffset),logical_innerVessel_1,"phy_innerVessel_1",logical_World,false,0,checkOverlaps);

  //Now, the two caps
  G4Tubs* solid_innerVessel_2 = new G4Tubs("sol_innervVessel_2",.0*m,.9500005*m,.025*m,0,2*M_PI);
  G4LogicalVolume* logical_innerVessel_2 = new G4LogicalVolume(solid_innerVessel_2, mat_Cu, "log_innerVessel_2" );
  logical_innerVessel_2->SetVisAttributes ( new G4VisAttributes(G4Colour(0.62, 0.3, 0.2,0.7) ));
  G4VPhysicalVolume* physical_innerVessel_2 = new G4PVPlacement(0,G4ThreeVector(0,0,assemblyzoffset+.975*m),logical_innerVessel_2,"phy_innerVessel_2",logical_World,false,0,checkOverlaps);

  //We want the total height of the caps plus the cylinder to be 2m
  //If we place the cap at +-1m, due to their 5cm thickness, it will
  //be 2.05m tall (gaining 2.5cm per cap). We don't want that, so we
  //modify the positional placement accordingly.

  G4VPhysicalVolume* physical_innerVessel_3 = new G4PVPlacement(0,G4ThreeVector(0,0,assemblyzoffset-.975*m),logical_innerVessel_2,"phy_innerVessel_3",logical_World,false,0,checkOverlaps);

  double materialmass = 0;

  if (detector_type.contains("GERDA"))
    {
#include "Detector_GERDAStyle.icc"
    }
  else
    {
#include "Detector_MJDStyle.icc"
    }

  double detectorvolume = 1080*solid_DetGeCrystal->GetCubicVolume()/CLHEP::cm3;
  double detectormassremoved = .005323*detectorvolume/CLHEP::kg;
  materialmass+= (logical_FillGas->GetMass()/CLHEP::kg);
  materialmass-=detectormassremoved;
  G4cout << G4endl << G4endl << "Mass of liquid argon in this setup: " << materialmass << G4endl <<G4endl;
    return physical_World;
}//Construct

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
#include "G4RunManager.hh"

void DetectorConstruction::UpdateGeometry()
{
  G4RunManager::GetRunManager()->DefineWorldVolume(Construct());
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetOverlapsCheck(G4bool f_check)
{
  checkOverlaps = f_check;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetFillGas(G4String f_gas)
{
  innerVessel_FillMaterial = f_gas;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetShieldStyle(G4String f_type)
{
  detector_type = f_type;
}

  //polycone implementation (deprecated, apparently)
  //G4double innerVessel_Z[6] = {1*m,0.95001*m,0.95001*m,-0.95001*m,-0.95001*m,-1*m};
  //G4double innerVessel_RMin[6] = {0*m,0*m,0.95001*m,0.95001*m,0*m,0*m};
  //G4double innerVessel_RMax[6] = {.9999*m,.9999*m,.9999*m,.9999*m,.9999*m,.9999*m};

  //G4Polycone* solid_innerVessel = new G4Polycone("sol_innerVessel", 0, 2*M_PI,6,innerVessel_Z,innerVessel_RMin,innerVessel_RMax);
  //G4LogicalVolume* logical_innerVessel = new G4LogicalVolume(solid_innerVessel, mat_Cu, "log_innerVessel" );
  //logical_innerVessel->SetVisAttributes ( new G4VisAttributes(G4Colour(0.62, 0.3, 0.2,0.7) ));
  //G4VPhysicalVolume* physical_innerVessel = new G4PVPlacement(0,G4ThreeVector(0,0,-6.5),logical_innerVessel,"phy_innerVessel",logical_World,false,0,checkOverlaps);

