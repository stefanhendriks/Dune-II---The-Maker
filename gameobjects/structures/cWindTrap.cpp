#include "../../include/d2tmh.h"

// Constructor
cWindTrap::cWindTrap() {

 // other variables (class specific)
 iFade=rnd(63);
 bFadeDir=true;

 // Timers
 TIMER_fade=0;
}

int cWindTrap::getType() {
	return WINDTRAP;
}

cWindTrap::~cWindTrap()
{

}


void cWindTrap::think()
{
	// think like base class
	cAbstractStructure::think();

}

void cWindTrap::think_fade() {
	TIMER_fade++;

    int iTime; // the speed of fading

    // depending on fade direction, fade in slower/faster
	if (bFadeDir) { // go to blue
        iTime = 3;
	} else { // go to black
        iTime = 4;
	}

    // time passed, we may change fade color
    if (TIMER_fade > iTime) {
        if (bFadeDir) {
            iFade++;

			if (iFade > 254) {
                bFadeDir=false;
			}
        } else {
            iFade--;

			if (iFade < 1) {
                bFadeDir=true;
			}
        }

        TIMER_fade=0;
    }
}

void cWindTrap::think_animation() {
	cAbstractStructure::think_animation();
	think_fade(); // windtrap specific blue fading
	cAbstractStructure::think_flag();
}

void cWindTrap::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */

int cWindTrap::getPowerOut() {
	cHitpointCalculator hitpointCalculator;
	float percentage = ((float)getHitPoints() / (float)structures[getType()].hp);
	return hitpointCalculator.getByPercent(structures[getType()].power_give, percentage);
}

int cWindTrap::getMaxPowerOut() {
	return getS_StructuresType().power_give;
}
