#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4UIExecutive.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "include/DetectorConstruction.hh"
#include "include/Physics.hh"
#include "include/PrimaryGeneratorAction.hh"
#include "include/ActionInitialization.hh"
int main(int argc, char** argv) {
    G4RunManager* runManager = new G4RunManager();
    std::ofstream file("/home/alina_van/project/results.csv");
    file << "x,y,z,count\n";
    file.close();
    runManager->SetUserInitialization(new DetectorConstruction());
    runManager->SetUserInitialization(new PhysicsList());
    runManager->SetUserInitialization(new ActionInitialization());
    runManager->Initialize();
    
    G4UIExecutive* ui = new G4UIExecutive(argc, argv);

    G4VisManager* visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager* UImanager =  G4UImanager::GetUIpointer();
    UImanager->ApplyCommand("/control/macroPath /home/alina_van/project");
    UImanager->ApplyCommand("/control/execute vis.mac");

    ui->SessionStart();

    return 0;
}