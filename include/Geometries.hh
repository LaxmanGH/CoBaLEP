#ifndef Geometries_h
#define Geometries_h 1

//Just a place to store all the variables used in the two geometry definition files

//Ambiguous variables used in pretty much every geometry
G4Box* solid_Rock; //= new G4Box("sol_Rock",50*m,50*m,30*m);
G4Box* solid_Lab; //= new G4Box("sol_Lab",35*m,10*m,4*m);

//Variables for detailed shaping of the surrounding rock,  used in the very first geometry (OriginalWithRock).
G4Tubs* solid_CutOut;
G4SubtractionSolid *solid_Rock2;//Lab cut out
G4SubtractionSolid *solid_Rock3;//Detector space cut out
G4LogicalVolume* logical_Rock;
G4VPhysicalVolume* physical_Rock;


//In addition to the above variables, the following are used in these geometries:
//OriginalWithRangeCuts
//September2018WithCuts
//March2019WithCuts
G4Tubs* inner_CutOut; //Space for detectors and inner rock region
G4SubtractionSolid *outer_Rock; //Outer rock with inner cutout removed
G4SubtractionSolid *outer_Rock2;//Inner cutout AND lab space removed
G4LogicalVolume* logical_outerRock;
G4VPhysicalVolume* physical_outerRock;
G4Tubs* inner_Rock;//Inner rock
G4SubtractionSolid* inner_Rock2;//Inner rock with lab space removed
G4LogicalVolume* logical_innerRock;
G4VPhysicalVolume* physical_innerRock;
G4Region *OuterRockRegion;//For range cuts
G4Region *InnerRockRegion;//For range cuts


//After the geometry overhaul, the only variables from the above that saw continued use were solid_Rock, solid_Lab, and the three outer_Rock variables. All of the additional variables below this were added during the overhaul.

//Variables used exclusively in the 2019 version:
G4Tubs *upperLArShield;
G4LogicalVolume *logical_upperLArShield;
G4VPhysicalVolume *physical_upperLArShield;
G4Tubs *upperSSVessel;
G4LogicalVolume *logical_upperSSVessel;
G4VPhysicalVolume *physical_upperSSVessel;
//Variables used exclusively in the 2020 version:
G4Tubs *vacuumSpace;
G4LogicalVolume *logical_vacuumSpace;
G4VPhysicalVolume *physical_vacuumSpace;
//There was no vacuum space implemented in 2019
G4Tubs *innerSSVessel;
G4LogicalVolume *logical_innerSSVessel;
G4VPhysicalVolume *physical_innerSSVessel;
//There were no chimneys implemented in 2019
G4Tubs *SSChimney;
G4LogicalVolume *logical_SSChimney;
G4VPhysicalVolume *physical_SSChimney;
//Variables used in both versions:
G4Box *waterShield;
G4LogicalVolume *logical_waterShield;
G4VPhysicalVolume *physical_waterShield;
G4Tubs *outerSSVessel;
G4LogicalVolume *logical_outerSSVessel;
G4VPhysicalVolume *physical_outerSSVessel;
G4Tubs *outerLArShield;
G4LogicalVolume *logical_outerLArShield;
G4VPhysicalVolume *physical_outerLArShield;
G4Tubs *innerCopperCryostat;
G4LogicalVolume *logical_innerCopperCryostat1;
G4LogicalVolume *logical_innerCopperCryostat2;
G4LogicalVolume *logical_innerCopperCryostat3;
G4LogicalVolume *logical_innerCopperCryostat4;
G4VPhysicalVolume *physical_innerCopperCryostat;
G4Tubs *innerLArShield;
G4LogicalVolume *logical_innerLArShield1;
G4LogicalVolume *logical_innerLArShield2;
G4LogicalVolume *logical_innerLArShield3;
G4LogicalVolume *logical_innerLArShield4;
G4VPhysicalVolume *physical_innerLArShield;
G4Tubs *ICPCshell;
G4Tubs *borehole;
G4SubtractionSolid *ICPCdetector;
G4LogicalVolume *logical_ICPCdetector;
G4VPhysicalVolume *physical_ICPCdetector;

//Implementation of a potential LNGS-specific geometry
//Suggested by Steve at Dec 2019 collaboration meeting
G4Box *LNGSrock;
G4Box *LNGSlabLower;
G4Tubs *LNGSlabUpper;
G4LogicalVolume *logical_LNGSrock;
G4LogicalVolume *logical_LNGSlabLower;
G4LogicalVolume *logical_LNGSlabUpper;
G4VPhysicalVolume *physical_LNGSrock;
G4VPhysicalVolume *physical_LNGSlabLower;
G4VPhysicalVolume *physical_LNGSlabUpper;
G4RotationMatrix *rotm;
G4Tubs *LNGSsmallWaterShield;
G4LogicalVolume *logical_LNGSsmallWaterShield;
G4VPhysicalVolume *physical_LNGSsmallWaterShield;
G4Box *LNGSlargeWaterShield;
G4LogicalVolume *logical_LNGSlargeWaterShield;
G4VPhysicalVolume *physical_LNGSlargeWaterShield;
G4Tubs *LNGSsteelVessel;
G4LogicalVolume *logical_LNGSsteelVessel;
G4VPhysicalVolume *physical_LNGSsteelVessel;
G4Tubs *LNGSargonShield;
G4LogicalVolume *logical_LNGSargonShield;
G4VPhysicalVolume *physical_LNGSargonShield;
G4Tubs *LNGSsteelNeck;
G4LogicalVolume *logical_LNGSsteelNeck;
G4VPhysicalVolume *physical_LNGSsteelNeck;
G4Tubs *LNGSargonNeck;
G4LogicalVolume *logical_LNGSargonNeck;
G4VPhysicalVolume *physical_LNGSargonNeck;
G4Tubs *LNGScopperCladding;
G4LogicalVolume *logical_LNGScopperCladding;
G4VPhysicalVolume *physical_LNGScopperCladding;
//Optional poly shield implementation
//Search keyword 'Bernhard' for more information
G4Tubs *polysolid;
G4Tubs *Gd2O3solid;
G4Tubs *subtractGd2O3;
G4SubtractionSolid *polyshield;
G4SubtractionSolid *Gd2O3shield;
G4LogicalVolume *logical_polyshield;
G4LogicalVolume *logical_Gd2O3shield;
G4VPhysicalVolume *physical_polyshield;
G4VPhysicalVolume *physical_Gd2O3shield;

//Optional kill surface
//Search keyword 'wrapper' for more information
G4Tubs *wrapsolid;
//G4LogicalVolume *logical_wrap;
//~Artificial
G4LogicalVolume *logical_wrap1;
G4LogicalVolume *logical_wrap2;
G4LogicalVolume *logical_wrap3;
G4LogicalVolume *logical_wrap4;
G4VPhysicalVolume *physical_wrap;

//Diagnostics subtraction solids
G4SubtractionSolid *subsolid1;
G4SubtractionSolid *subsolid2;
G4SubtractionSolid *subsolid3;
G4SubtractionSolid *subsolid4;

G4Tubs *GeColumn;

//Trig study variables
G4Tubs* innerer_CutOut;
G4Tubs* innerer_Rock;

//For LAR pathlength studies
G4Tubs *inner_Argon;
G4Tubs *outer_Copper;
G4LogicalVolume *logical_Argon;
G4LogicalVolume *logical_Copper;
G4VPhysicalVolume *physical_Argon;
G4VPhysicalVolume *physical_Copper;

//For neutron multiplicity study
G4Box* solid_innerWorld;
G4LogicalVolume* logical_innerWorld;
G4VPhysicalVolume* physical_innerWorld;
G4Box* solid_neutronRock;
G4LogicalVolume* logical_neutronRock;
G4VPhysicalVolume* physical_neutronRock;
G4String rock_name;
G4ThreeVector rock_position;

//For LArColumn showering study
G4Box* solid_neutronLAr;
G4LogicalVolume* logical_neutronLAr;
G4VPhysicalVolume* physical_neutronLAr;
G4String LAr_name;
G4ThreeVector LAr_position;
//For XeColumn showering study
G4Box* solid_neutronXe;
G4LogicalVolume* logical_neutronXe;
G4VPhysicalVolume* physical_neutronXe;
G4String Xe_name;
G4ThreeVector Xe_position;


//For MCNP comparison studies
G4Box *outerSteel;
G4Box *h2oShield;
G4Box *argonShield;
G4Box *geTarget;
G4LogicalVolume *logicalSteel;
G4PVPlacement *physicalSteel;
G4LogicalVolume *logicalh2o;
G4PVPlacement *physicalh2o;
G4LogicalVolume *logicalAr1;
G4PVPlacement *physicalAr1;
G4LogicalVolume *logicalAr2;
G4PVPlacement *physicalAr2;
G4LogicalVolume *logicalGe;
G4PVPlacement *physicalGe;

//For poly validation studies
G4Box *polysquare;
G4Box *Gd2O3square;
G4Box *Larquare;
G4Box *Gesquare;
G4LogicalVolume *logical_polysquare;
G4LogicalVolume *logical_Gd2O3square;
G4LogicalVolume *logical_Larquare;
G4LogicalVolume *logical_Gesquare;
G4VPhysicalVolume *physical_polysquare;
G4VPhysicalVolume *physical_Gd2O3square;
G4VPhysicalVolume *physical_Larquare;
G4VPhysicalVolume *physical_Gesquare;
//For LEGEND-200 geometry mockup, to be used by Giuseppe et. al
G4Tubs *L200watershield;
G4Tubs *L200steelvessel;
G4Tubs *L200argonshield;
G4Tubs *L200detector;
G4Tubs *L200fakeshroud;
G4Tubs *L200rocksolid;
G4SubtractionSolid *L200rock;
G4LogicalVolume *logical_L200watershield;
G4LogicalVolume *logical_L200steelvessel;
G4LogicalVolume *logical_L200argonshield;
G4LogicalVolume *logical_L200detector;
G4LogicalVolume *logical_L200fakeshroud;
G4LogicalVolume *logical_L200rock;
G4VPhysicalVolume *physical_L200watershield;
G4VPhysicalVolume *physical_L200steelvessel;
G4VPhysicalVolume *physical_L200argonshield;
G4VPhysicalVolume *physical_L200detector;
G4VPhysicalVolume *physical_L200fakeshroud;
G4VPhysicalVolume *physical_L200rock;

#endif
