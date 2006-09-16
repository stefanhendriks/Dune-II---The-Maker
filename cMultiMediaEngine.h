/**
	MultiMediaEngine

	Responsible for calling correct library functions. Layer for multi-lib-support.

*/

#ifndef CMULTIMEDIAENGINE_H

/** Compile time must-have definitions */
#ifdef ALLEGRO_H 
	#define MME_SURFACE	BITMAP *
#endif

/** Class */
class cMultiMediaEngine {

private:
	
public:
	virtual void drawSprite(MME_SURFACE source, MME_SURFACE dest, int x, int y) = 0;

};


#endif