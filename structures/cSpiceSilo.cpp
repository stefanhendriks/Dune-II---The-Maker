#include "../d2tmh.h"

// Constructor
cSpiceSilo::cSpiceSilo() {
 // other variables (class specific)

}

int cSpiceSilo::getType() {
	return SILO;
}


cSpiceSilo::~cSpiceSilo()
{

}


void cSpiceSilo::think()
{
	// think like base class
	cAbstractStructure::think();

}

// Specific Animation thinking (flag animation OR its deploy animation)
void cSpiceSilo::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
}

void cSpiceSilo::think_guard()
{

}

// Draw function to draw this structure()
void cSpiceSilo::draw(int iStage) {
	cAbstractStructure::draw(iStage);
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */
