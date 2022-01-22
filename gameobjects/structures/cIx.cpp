#include "../../include/d2tmh.h"

// Constructor
cIx::cIx() {
    // other variables (class specific)
}

int cIx::getType() const {
    return IX;
}

cIx::~cIx() {

}

void cIx::thinkFast() {
    cAbstractStructure::thinkFast();
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cIx::think_animation() {
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cIx::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
