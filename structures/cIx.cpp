#include "../d2tmh.h"

// Constructor
cIx::cIx() {

 // other variables (class specific)

}

int cIx::getType() {
	return IX;
}

cIx::~cIx()
{

}


void cIx::think()
{

	// last but not least, think like our abstraction
	cAbstractStructure::think();
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cIx::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
}

void cIx::think_guard()
{

}

// Draw function to draw this structure()
void cIx::draw(int iStage) {
	cAbstractStructure::draw(iStage);
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */
