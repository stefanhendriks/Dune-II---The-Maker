#include "../../include/d2tmh.h"

// Constructor
cPalace::cPalace() {
	// other variables (class specific)

}

int cPalace::getType() {
	return PALACE;
}

cPalace::~cPalace() {

}

void cPalace::think() {
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

void cPalace::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
