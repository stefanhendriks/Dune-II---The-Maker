#include "../../include/d2tmh.h"

// Constructor
cBarracks::cBarracks() {
}

cBarracks::~cBarracks() {
}

void cBarracks::think() {
	cAbstractStructure::think();
}

int cBarracks::getType() {
	return BARRACKS;
}

void cBarracks::think_animation() {
	// a barracks does not animate, so when set, set it back to false
	if (isAnimating()) {
		setAnimating(false);
	}
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
}

void cBarracks::think_guard() {
 // do nothing
}

// Draw function to draw this structure()
void cBarracks::draw(int iStage) {
	cAbstractStructure::draw(iStage);
}
