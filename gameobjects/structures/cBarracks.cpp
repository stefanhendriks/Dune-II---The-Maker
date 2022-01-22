#include "../../include/d2tmh.h"

// Constructor
cBarracks::cBarracks() {
}

cBarracks::~cBarracks() {
}

void cBarracks::thinkFast() {
    cAbstractStructure::thinkFast();
}

int cBarracks::getType() const {
	return BARRACKS;
}

void cBarracks::think_animation() {
	// a barracks does not animate, so when set, set it back to false
	if (isAnimating()) {
		setAnimating(false);
	}
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag_new();
}

void cBarracks::think_guard() {
 // do nothing
}
