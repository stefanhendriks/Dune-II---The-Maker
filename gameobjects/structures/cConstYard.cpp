#include "../../include/d2tmh.h"

// Constructor
cConstYard::cConstYard() {
    frames = 0;
}

cConstYard::~cConstYard() {
}

int cConstYard::getType() const {
	return CONSTYARD;
}

void cConstYard::think() {
	cAbstractStructure::think();
}

void cConstYard::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
}

void cConstYard::think_guard() {
 // do nothing
}
