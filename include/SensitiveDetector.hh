#ifndef SENSITIVEDETECTOR_HH
#define SENSITIVEDETECTOR_HH

#include "G4VSensitiveDetector.hh"
#include "G4Step.hh"
#include "G4SystemOfUnits.hh"
#include "G4Track.hh"
#include "G4ThreeVector.hh"
#include "G4ios.hh"
#include "G4OpticalPhoton.hh"

class SensitiveDetector : public G4VSensitiveDetector {
public:
    SensitiveDetector(const G4String& name);
    ~SensitiveDetector();
    virtual G4bool ProcessHits(G4Step* step, G4TouchableHistory* history) override;
    virtual void Initialize(G4HCofThisEvent* hce) override;
    virtual void EndOfEvent(G4HCofThisEvent* hce) override;  
private:
    G4int particleCount = 0;
    G4int eventCount = 0;
    std::ofstream outputFile;
};

#endif 