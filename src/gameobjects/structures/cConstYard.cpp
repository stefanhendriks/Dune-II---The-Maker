#include "../../include/d2tmh.h"

// Constructor
cConstYard::cConstYard() {
}

cConstYard::~cConstYard() {
}

int cConstYard::getType() {
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
