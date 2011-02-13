#include "../../include/d2tmh.h"

// Constructor
cWor::cWor()
{
 // other variables (class specific)
}

int cWor::getType() {
	return WOR;
}

cWor::~cWor()
{

}

void cWor::think()
{
	// think like base class
	cAbstractStructure::think();

}

void cWor::think_animation() {
	// a wor does not animate, so when set, set it back to false
	if (isAnimating()) {
		setAnimating(false);
	}

	cAbstractStructure::think_animation();
	cAbstractStructure::think_flag();
}

void cWor::think_guard()
{

}

/*  STRUCTURE SPECIFIC FUNCTIONS  */


