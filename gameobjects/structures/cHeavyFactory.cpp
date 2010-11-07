#include "../../include/d2tmh.h"

// Constructor
cHeavyFactory::cHeavyFactory()
{

 // other variables (class specific)

}


int cHeavyFactory::getType() {
	return HEAVYFACTORY;
}


cHeavyFactory::~cHeavyFactory()
{

}


/*  OVERLOADED FUNCTIONS  */

void cHeavyFactory::think()
{
	// last but not least, think like our abstraction
	cAbstractStructure::think();
}

// think animation for unit deployment (when building unit is finished)
void cHeavyFactory::think_animation_unitDeploy() {
	if (isAnimating() == false) return; // only do when animating

    // show animation (unit finished)
    TIMER_flag++;
    if (TIMER_flag > 70) {
        if(iFrame > 2) // 2 animation frames
        {
            iFrame=0;
            setAnimating(false);
        }
        else if (iFrame < 1)
            iFrame=1;

        iFrame++;

        TIMER_flag=0;
    }
}

// Specific Animation thinking (flag animation OR its deploy animation)
void cHeavyFactory::think_animation()
{
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
	think_animation_unitDeploy();
}

void cHeavyFactory::think_guard()
{

}

// Draw function to draw this structure()
void cHeavyFactory::draw(int iStage) {
    cAbstractStructure::draw(iStage);
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */
