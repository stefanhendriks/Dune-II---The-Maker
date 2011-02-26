#include "../../include/d2tmh.h"

// Constructor
cHighTech::cHighTech() {

	// other variables (class specific)

}

int cHighTech::getType() {
	return HIGHTECH;
}

cHighTech::~cHighTech() {

}

void cHighTech::think() {
	// last but not least, think like our abstraction
	cAbstractStructure::think();
}

void cHighTech::think_animation_unitDeploy() {
	if (!isAnimating())
		return; // do nothing when not animating

	// show animation (unit finished)
	TIMER_flag++;
	if (TIMER_flag > 70) {
		if (iFrame > 2) // 2 animation frames
		{
			iFrame = 0;
			setAnimating(false);
		} else if (iFrame < 1)
			iFrame = 1;

		iFrame++;

		TIMER_flag = 0;
	}
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cHighTech::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
	think_animation_unitDeploy();
}

void cHighTech::think_guard() {

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */
