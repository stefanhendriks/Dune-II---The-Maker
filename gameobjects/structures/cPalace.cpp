#include "../../include/d2tmh.h"

// Constructor
cPalace::cPalace()
{
 // other variables (class specific)

}

int cPalace::getType() {
	return PALACE;
}

cPalace::~cPalace()
{

}

void cPalace::think()
{
	cAbstractStructure::think();
}

void cPalace::think_animation() {
	// a palace does not animate, so when set, set it back to false
	if (isAnimating()) {
		setAnimating(false);
	}
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
}

void cPalace::think_guard()
{

}

// Draw function to draw this structure()
void cPalace::draw(int iStage) {
    cAbstractStructure::draw(iStage);
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */
