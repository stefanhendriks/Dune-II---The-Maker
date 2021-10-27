#include "../../include/d2tmh.h"

// Constructor
cOutPost::cOutPost()
{
 // other variables (class specific)

}

int cOutPost::getType() const {
	return RADAR;
}

cOutPost::~cOutPost()
{

}


void cOutPost::think()
{
	// last but not least, think like our abstraction
	cAbstractStructure::think();
}

void cOutPost::think_animation() {
	cAbstractStructure::think_animation();
	think_flag();
}

void cOutPost::think_flag() {
	if (isAnimating()) return; // do no flag animation when animating

	TIMER_flag++;

    if (TIMER_flag > 35) {
        iFrame++;

		// switch between 0 and 7.
		if (iFrame > 7) {
            iFrame=0;
		}

        TIMER_flag=0;
    }
}

void cOutPost::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
