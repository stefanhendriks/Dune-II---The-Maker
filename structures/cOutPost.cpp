#include "../d2tmh.h"

// Constructor
cOutPost::cOutPost()
{
 // other variables (class specific)
 
}

int cOutPost::getType() {
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
	cAbstractStructure::think_flag();
}

void cOutPost::think_guard()
{

}

// Draw function to draw this structure()
void cOutPost::draw(int iStage) {   
cAbstractStructure::draw(iStage);   
}


/*  STRUCTURE SPECIFIC FUNCTIONS  */


