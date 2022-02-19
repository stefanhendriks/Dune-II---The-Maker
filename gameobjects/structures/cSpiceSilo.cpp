#include "cSpiceSilo.h"

#include "d2tmc.h"
#include "definitions.h"

cSpiceSilo::cSpiceSilo() {
    // other variables (class specific)

}

int cSpiceSilo::getType() const {
    return SILO;
}


void cSpiceSilo::thinkFast() {
    // think like base class
    cAbstractStructure::thinkFast();
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cSpiceSilo::think_animation() {
    cAbstractStructure::think_animation();
    cAbstractStructure::think_flag_new();
}

void cSpiceSilo::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
int cSpiceSilo::getSpiceSiloCapacity() {
    float percentage = ((float) getHitPoints() / (float) sStructureInfo[getType()].hp);
    return 1000 * percentage;
}
