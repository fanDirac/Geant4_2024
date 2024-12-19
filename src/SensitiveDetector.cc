#include "../include/SensitiveDetector.hh"

SensitiveDetector::SensitiveDetector(const G4String& name): G4VSensitiveDetector(name), particleCount(0) {}

SensitiveDetector::~SensitiveDetector() {}

G4bool SensitiveDetector::ProcessHits(G4Step* step, G4TouchableHistory*) {
    G4Track* track = step->GetTrack();
    if(step->GetTrack()->GetParticleDefinition() == G4OpticalPhoton::OpticalPhotonDefinition()){
        particleCount++;
    }
    track->SetTrackStatus(fStopAndKill);

    return true;
}

void SensitiveDetector::Initialize(G4HCofThisEvent* ) {
    particleCount = 0;
}

void SensitiveDetector::EndOfEvent(G4HCofThisEvent* ) {
    eventCount++;
    outputFile.open("/home/alina_van/project/results.csv", std::ios::app);
    if (outputFile.is_open()) {
        // outputFile << "PhotonID,ParentBirthX(mm),ParentBirthY(mm),ParentBirthZ(mm)\n";
        outputFile << particleCount << G4endl;
    } else {
        G4cerr << "Error: Unable to open file for writing." << G4endl;
    }  
    outputFile.close();
    if(eventCount % 100 == 0) G4cout << eventCount << G4endl;
}