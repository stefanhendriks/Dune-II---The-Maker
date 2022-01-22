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

void cConstYard::thinkFast() {
    cAbstractStructure::thinkFast();
}

void cConstYard::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag_new();
}

void cConstYard::think_guard() {
 // do nothing
}
