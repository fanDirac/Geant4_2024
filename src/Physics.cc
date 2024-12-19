#include "../include/Physics.hh"

PhysicsList::PhysicsList(){
    RegisterPhysics (new G4EmStandardPhysics_option4());
    RegisterPhysics (new G4OpticalPhysics());
}

PhysicsList::~PhysicsList(){}