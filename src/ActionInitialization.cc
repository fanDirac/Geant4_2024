#include "../include/ActionInitialization.hh"

ActionInitialization::ActionInitialization(){}
ActionInitialization::~ActionInitialization(){}

void ActionInitialization::Build() const {
    SetUserAction(new PrimaryGeneratorAction());
}