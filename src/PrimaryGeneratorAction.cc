#include "../include/PrimaryGeneratorAction.hh"


PrimaryGeneratorAction::PrimaryGeneratorAction() {
    fParticleGun = new G4ParticleGun(1);
    G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
    G4String particleName = "e+";

    G4ParticleDefinition* particle = particleTable->FindParticle(particleName);
    if (particle) {
        fParticleGun->SetParticleDefinition(particle);
    } else {
        G4cerr << "Error: particle '" << particleName << "' not found in particle table!" << G4endl;
    }
}

PrimaryGeneratorAction::~PrimaryGeneratorAction() {
    delete fParticleGun;
}
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent) {
    G4double radius = 0.62035 * m;
    // radius = 0.22035 * m; 
    G4ThreeVector position;
    do {
        G4double x = (G4UniformRand() - 0.5) * 2 * radius; 
        G4double y = (G4UniformRand() - 0.5) * 2 * radius; 
        G4double z = (G4UniformRand() - 0.5) * 2 * radius; 

        if (x * x + y * y + z * z <= radius * radius) {
            position.set(x, y, z); 
            break; 
        }
    } while (true);
    // position = G4ThreeVector(470 * mm,0,0);
    fParticleGun->SetParticlePosition(position);

    G4double theta = acos(2 * G4UniformRand() - 1); 
    G4double phi = 2 * M_PI * G4UniformRand();

    G4ThreeVector direction(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
    fParticleGun->SetParticleMomentumDirection(direction);
    fParticleGun->SetParticleEnergy(1.0 * MeV);
    fParticleGun->GeneratePrimaryVertex(anEvent);

    std::ofstream file("/home/alina_van/project/results.csv", std::ios::app);
    if (file.is_open()) {
        file    << position.x() / mm << "," << position.y() / mm 
                << "," << position.z() / mm << ",";
    } else {
        G4cerr << "Error!!! Can't open file to write." << G4endl;
    }
}