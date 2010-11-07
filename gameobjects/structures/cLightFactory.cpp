#include "../../include/d2tmh.h"

// Constructor
cLightFactory::cLightFactory()
{
 // other variables (class specific)

}

int cLightFactory::getType() {
	return LIGHTFACTORY;
}

cLightFactory::~cLightFactory()
{

}


void cLightFactory::think()
{
	// last but not least, think like our abstraction
	cAbstractStructure::think();
}

void cLightFactory::think_animation_unitDeploy() {
	if (!isAnimating()) return; // do nothing when not animating

    TIMER_flag++;
    if (TIMER_flag > 70)
    {
        if (iFrame >= 2)
        {
            iFrame=0;
            setAnimating(false);
        }
        else if (iFrame < 1)
            iFrame=1;

        TIMER_flag=0;

        iFrame++;
    }
}
// Specific Animation thinking (flag animation OR its deploy animation)
void cLightFactory::think_animation() {
	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
	think_animation_unitDeploy();
}

void cLightFactory::think_guard()
{

}

// Draw function to draw this structure()
void cLightFactory::draw(int iStage) {
    cAbstractStructure::draw(iStage);
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */
