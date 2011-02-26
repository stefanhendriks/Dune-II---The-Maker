#include "../../include/d2tmh.h"

// Constructor
cSpiceSilo::cSpiceSilo() {
	// other variables (class specific)

}

int cSpiceSilo::getType() {
	return SILO;
}

cSpiceSilo::~cSpiceSilo() {

}

void cSpiceSilo::think() {
	// think like base class
	cAbstractStructure::think();

}

// Specific Animation thinking (flag animation OR its deploy animation)
void cSpiceSilo::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
}

void cSpiceSilo::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
int cSpiceSilo::getSpiceSiloCapacity() {
	cHitpointCalculator hitpointCalculator;
	float percentage = ((float) getHitPoints() / (float) structures[getType()].hp);
	return hitpointCalculator.getByPercent(1000, percentage);
}
