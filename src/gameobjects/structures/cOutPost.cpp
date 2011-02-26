#include "../../include/d2tmh.h"

// Constructor
cOutPost::cOutPost() {
	// other variables (class specific)

}

int cOutPost::getType() {
	return RADAR;
}

cOutPost::~cOutPost() {

}

void cOutPost::think() {
	// last but not least, think like our abstraction
	cAbstractStructure::think();
}

void cOutPost::think_animation() {
	cAbstractStructure::think_animation();
	think_flag();
}

void cOutPost::think_flag() {
	if (isAnimating())
		return; // do no flag animation when animating

	TIMER_flag++;

	if (TIMER_flag > 70) {
		iFrame++;

		// switch between 0 and 3.
		if (iFrame > 3) {
			iFrame = 0;
		}

		TIMER_flag = 0;
	}
}

void cOutPost::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
