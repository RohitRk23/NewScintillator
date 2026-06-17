//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
/// \file optical/OpNovice2/src/DetectorConstruction.cc
/// \brief Implementation of the DetectorConstruction class
//
//
//
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "DetectorConstruction.hh"

#include "DetectorMessenger.hh"

#include "G4NistManager.hh"
#include "G4Material.hh"
#include "G4Element.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
//-------------For PMT------------//
// #include "DMXPmtSD.hh"
#include "G4SDManager.hh"
//-------------For PMT------------//

#include <iostream>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction()
  : G4VUserDetectorConstruction()
  , fDetectorMessenger(nullptr)
{
  fExpHall_x = fExpHall_y = fExpHall_z = 10.0 * m;
  fTank_x = fTank_y = fTank_z = 1.0 * m;

  fTank = nullptr;

  fTankMPT    = new G4MaterialPropertiesTable();
  fWorldMPT   = new G4MaterialPropertiesTable();
  fSurfaceMPT = new G4MaterialPropertiesTable();
  // fMirrorSurfaceMPT = new G4MaterialPropertiesTable(); // For Reflecting surface

  fSurface = new G4OpticalSurface("Surface");
  fSurface->SetType(dielectric_dielectric);
  fSurface->SetFinish(ground);
  fSurface->SetModel(unified);
  fSurface->SetMaterialPropertiesTable(fSurfaceMPT);

  // For Reflecting Surface
  // fMirrorSurface = new G4OpticalSurface("MirrorSurface");
  // fMirrorSurface->SetType(dielectric_metal);
  // fMirrorSurface->SetFinish(polished);
  // fMirrorSurface->SetModel(unified);
  // fMirrorSurface->SetMaterialPropertiesTable(fMirrorSurfaceMPT);

  fTank_LV  = nullptr;
  fWorld_LV = nullptr;

  // fTankMaterial  = G4NistManager::Instance()->FindOrBuildMaterial("G4_WATER");
  fTankMaterial  = G4NistManager::Instance()->FindOrBuildMaterial("G4_BGO");
  fWorldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_AIR");
  fCopyWorldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic"); // For Reflecting Surface
  // fWorldMaterial = G4NistManager::Instance()->FindOrBuildMaterial("G4_Galactic");
  
  //For Crystal
  //
  // G4NistManager* nistManager = G4NistManager::Instance();

  // fWorldMaterial = nistManager->FindOrBuildMaterial("G4_AIR");

  // G4Element *elCa = nistManager->FindOrBuildElement("Ca");
  // G4Element *elW = nistManager->FindOrBuildElement("W");
  // G4Element *elO = nistManager->FindOrBuildElement("O");
  // G4double density = 6.1*g/cm3;
  // fTankMaterial = new G4Material("CalciumTungstate", density, 3);
  // fTankMaterial->AddElement(elCa, 1);
  // fTankMaterial->AddElement(elW, 1);
  // fTankMaterial->AddElement(elO, 4);

  fDetectorMessenger = new DetectorMessenger(this);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction() {
  delete fTankMPT;
  delete fWorldMPT;
  delete fSurfaceMPT;
  // delete fMirrorSurfaceMPT;
  delete fSurface;
  delete fDetectorMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* DetectorConstruction::Construct()
{
  fTankMaterial->SetMaterialPropertiesTable(fTankMPT);
  fTankMaterial->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);

  fWorldMaterial->SetMaterialPropertiesTable(fWorldMPT);

  // ------------- Volumes --------------
  // The experimental Hall

// Additional Volume for making the world refelcting
    //-----------------------------------------------------------------//
    G4Box* copy_world_box = new G4Box("CopyWorld", 6.01*cm,6.01*cm,6.01*cm);

   fCopyWorld_LV = new G4LogicalVolume(copy_world_box, fCopyWorldMaterial, "CopyWorld", 0, 0, 0);

  G4VPhysicalVolume* Copyworld_PV =
    new G4PVPlacement(0, G4ThreeVector(), fCopyWorld_LV, "CopyWorld", 0, false, 0);
    //-----------------------------------------------------------------//

  // G4Box* world_box = new G4Box("World", fExpHall_x, fExpHall_y, fExpHall_z);
  G4Box* world_box = new G4Box("World", 4.*cm,4.*cm,4.*cm);

  fWorld_LV = new G4LogicalVolume(world_box, fWorldMaterial, "World", 0, 0, 0);

  G4VPhysicalVolume* world_PV =
    new G4PVPlacement(0, G4ThreeVector(), fWorld_LV, "World", fCopyWorld_LV, false, 0);

  
  // // The tank
  // G4Box* tank_box = new G4Box("Tank", fTank_x, fTank_y, fTank_z);
  G4VSolid* tank_box = new G4Tubs("Tank", 0.*cm, 2.0*cm, 2.0*cm, 0.*deg, 360.*deg);

  fTank_LV = new G4LogicalVolume(tank_box, fTankMaterial, "Tank", 0, 0, 0);

  fTank = new G4PVPlacement(0, G4ThreeVector(), fTank_LV, "Tank", fWorld_LV, false, 0);

//----------------------------------PMT Addition Part-----------------------------------------//
  //For PMT
  // 1. Define the PMT Material (Quartz or Glass is common)
  G4Material* pmt_mat = G4NistManager::Instance()->FindOrBuildMaterial("G4_GLASS_PLATE");

  // 2. Create the PMT geometry (e.g., a thin disk on the wall)
  G4double pmt_radius = 1.*cm;
  G4double pmt_thickness = 1.*mm;
  G4Tubs* pmt_solid = new G4Tubs("PMT_Solid", 0.*cm, pmt_radius, pmt_thickness, 0.*deg, 360.*deg);

  G4LogicalVolume* pmt_log = new G4LogicalVolume(pmt_solid, pmt_mat, "PMT_LV");

  // 3. Place the PMT on the inside surface of the World box (at 10m)
  // Note: World is 6cm half-width, so we place at 5.90cm
  G4VPhysicalVolume* pmt_phys = new G4PVPlacement(0, G4ThreeVector(0, 0, 3.90*cm), 
                                    pmt_log, "PMT_PV", fWorld_LV, false, 0);

//----------------------------------PMT Addition Part-----------------------------------------//
  //Crystal Volume
  //G4_AIR
//   G4VSolid* tank_box = new G4Tubs("Tank", 0.*m, 0.5*m, 0.5*m, 0.*deg, 360.*deg);
//    fTank_LV =
//     new G4LogicalVolume(tank_box,fTankMaterial,"Tank",0,0,0);
// //    new G4LogicalVolume(SubstrateSolid,fCrystalMaterial,"SubstrateLogical1");
//   fTank =
//     new G4PVPlacement(0,G4ThreeVector(),fTank_LV,"Tank",
//                       fWorld_LV,false,0);

  // ------------- Surface --------------

  G4LogicalBorderSurface* surface =
    new G4LogicalBorderSurface("Surface", fTank, world_PV, fSurface);

  G4OpticalSurface* opticalSurface = dynamic_cast<G4OpticalSurface*>(
    surface->GetSurface(fTank, world_PV)->GetSurfaceProperty());
  G4cout << "******  opticalSurface->DumpInfo:" << G4endl;
  if(opticalSurface)
  {
    opticalSurface->DumpInfo();
  }
  G4cout << "******  end of opticalSurface->DumpInfo" << G4endl;

  // new G4LogicalSkinSurface("MirrorSurface", fWorld_LV, fMirrorSurface); // For Reflecting Surface
//----------------------------------PMT Addition Part-----------------------------------------//

  // // 1. Define the Optical Surface
  // G4OpticalSurface* pmtSurface = new G4OpticalSurface("PmtSurface");
  // pmtSurface->SetType(dielectric_dielectric); // Interaction between two dielectrics (Air and Glass)
  // pmtSurface->SetFinish(polished);            // Smooth interface
  // pmtSurface->SetModel(unified);

  // // 2. Link to a Property Table (You can reuse fSurfaceMPT from the example)
  // pmtSurface->SetMaterialPropertiesTable(fSurfaceMPT); 

  // // 3. Define the Boundary
  // // world_PV is your World Physical Volume, pmt_phys is your PMT Physical Volume
  // new G4LogicalBorderSurface("WorldToPmt", world_PV, pmt_phys, pmtSurface);
  // // 2. Define the Reflectivity (1.0 = 100% reflection)
  // G4double energies[] = {2.0*eV, 8.0*eV}; // Define range covering your scintillation
  // G4double reflectivity[] = {0.0, 0.0};   // 00% reflection
  // G4double efficiency[] = {1.0, 1.0};   // 00% reflection
  // G4MaterialPropertiesTable* PMTMP = new G4MaterialPropertiesTable();
  // PMTMP->AddProperty("REFLECTIVITY", energies, reflectivity, 2);
  // PMTMP->AddProperty("EFFICIENCY", energies, reflectivity, 2);
  // std::cout<<"properties are added "<<std::endl;
  // pmtSurface->SetMaterialPropertiesTable(PMTMP);
//----------------------------------PMT Addition Part-----------------------------------------//
    // 1. Create a new Optical Surface for the World Boundary
G4OpticalSurface* worldSkin = new G4OpticalSurface("WorldSkin");
worldSkin->SetModel(unified);
// worldSkin->SetType(dielectric_dielectric);
worldSkin->SetType(dielectric_metal);
worldSkin->SetFinish(polishedfrontpainted);
std::cout<<"Set the skin "<<std::endl;
// 2. Define the Reflectivity (1.0 = 100% reflection)
G4double energies[] = {2.0*eV, 8.0*eV}; // Define range covering your scintillation
G4double reflectivity[] = {1.0, 1.0};   // 100% reflection
// G4double transmitivity[] = {0.05, 0.05};   // 100% reflection
G4double efficiency[] = {0.0, 0.0};   // 100% reflection
G4MaterialPropertiesTable* worldSkinMPT = new G4MaterialPropertiesTable();
worldSkinMPT->AddProperty("REFLECTIVITY", energies, reflectivity, 2);
// worldSkinMPT->AddProperty("TRANSMITTANCE", energies, transmitivity, 2);
worldSkinMPT->AddProperty("EFFICIENCY", energies, efficiency, 2);
std::cout<<"properties are added "<<std::endl;
worldSkin->SetMaterialPropertiesTable(worldSkinMPT);

// 3. Wrap the World Logical Volume in this surface
// new G4LogicalSkinSurface("WorldSkinSurface", fWorld_LV, worldSkin);
new G4LogicalBorderSurface("WorldBorder", world_PV, Copyworld_PV, worldSkin);
new G4LogicalBorderSurface("CopyWorldBorder", Copyworld_PV, world_PV, worldSkin);
std::cout<<"Surface is defined successfully "<<std::endl;
  return Copyworld_PV;
}

//....oooOO0OOooo........oooOO0OOooo....PMT....oooOO0OOooo........oooOO0OOooo......
// void DetectorConstruction::ConstructSDandField()
// {
//   // Create the SD
//   G4String pmtSDName = "DMXPmtSD";
//   DMXPmtSD* pmtSD = new DMXPmtSD(pmtSDName);
  
//   // Register with Manager
//   G4SDManager::GetSDMpointer()->AddNewDetector(pmtSD);
  
//   // Attach to the Logical Volume
//   SetSensitiveDetector("PMT_LV", pmtSD);
// }
//....oooOO0OOooo........oooOO0OOooo.....PMT...oooOO0OOooo........oooOO0OOooo......


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetSurfaceSigmaAlpha(G4double v)
{
  fSurface->SetSigmaAlpha(v);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();

  G4cout << "Surface sigma alpha set to: " << fSurface->GetSigmaAlpha()
         << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetSurfacePolish(G4double v)
{
  fSurface->SetPolish(v);
  G4RunManager::GetRunManager()->GeometryHasBeenModified();

  G4cout << "Surface polish set to: " << fSurface->GetPolish() << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddTankMPV(const G4String& prop,
                                      G4MaterialPropertyVector* mpv)
{
  fTankMPT->AddProperty(prop, mpv);
  G4cout << "The MPT for the box is now: " << G4endl;
  fTankMPT->DumpTable();
  G4cout << "............." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddWorldMPV(const G4String& prop,
                                       G4MaterialPropertyVector* mpv)
{
  fWorldMPT->AddProperty(prop, mpv);
  G4cout << "The MPT for the world is now: " << G4endl;
  fWorldMPT->DumpTable();
  G4cout << "............." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddSurfaceMPV(const G4String& prop,
                                         G4MaterialPropertyVector* mpv)
{
  fSurfaceMPT->AddProperty(prop, mpv);
  G4cout << "The MPT for the surface is now: " << G4endl;
  fSurfaceMPT->DumpTable();
  G4cout << "............." << G4endl;
}
// This is for Reflecting Surface
// //------------------------------------------------------------------------------//
// void DetectorConstruction::AddMirrorProperty(const G4String& prop, 
//                                             G4MaterialPropertyVector* mpv)
// {
//   fMirrorSurfaceMPT->AddProperty(prop, mpv);
// }
//------------------------------------------------------------------------------//

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddTankMPC(const G4String& prop, G4double v)
{
  fTankMPT->AddConstProperty(prop, v);
  G4cout << "The MPT for the box is now: " << G4endl;
  fTankMPT->DumpTable();
  G4cout << "............." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddWorldMPC(const G4String& prop, G4double v)
{
  fWorldMPT->AddConstProperty(prop, v);
  G4cout << "The MPT for the world is now: " << G4endl;
  fWorldMPT->DumpTable();
  G4cout << "............." << G4endl;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::AddSurfaceMPC(const G4String& prop, G4double v)
{
  fSurfaceMPT->AddConstProperty(prop, v);
  G4cout << "The MPT for the surface is now: " << G4endl;
  fSurfaceMPT->DumpTable();
  G4cout << "............." << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetWorldMaterial(const G4String& mat)
{
  G4Material* pmat = G4NistManager::Instance()->FindOrBuildMaterial(mat);
  if(pmat && fWorldMaterial != pmat)
  {
    fWorldMaterial = pmat;
    if(fWorld_LV)
    {
      fWorld_LV->SetMaterial(fWorldMaterial);
      fWorldMaterial->SetMaterialPropertiesTable(fWorldMPT);
    }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    G4cout << "World material set to " << fWorldMaterial->GetName() << G4endl;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstruction::SetTankMaterial(const G4String& mat)
{
  G4Material* pmat = G4NistManager::Instance()->FindOrBuildMaterial(mat);
  if(pmat && fTankMaterial != pmat)
  {
    fTankMaterial = pmat;
    if(fTank_LV)
    {
      fTank_LV->SetMaterial(fTankMaterial);
      fTankMaterial->SetMaterialPropertiesTable(fTankMPT);
      fTankMaterial->GetIonisation()->SetBirksConstant(0.126 * mm / MeV);
    }
    G4RunManager::GetRunManager()->PhysicsHasBeenModified();
    G4cout << "Tank material set to " << fTankMaterial->GetName() << G4endl;
  }
}
