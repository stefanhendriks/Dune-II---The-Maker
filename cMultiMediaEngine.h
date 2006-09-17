/**
	MultiMediaEngine

	Responsible for calling correct library functions. Layer for multi-lib-support.

*/

#ifndef CMULTIMEDIAENGINE_H

/** Compile time must-have definitions */
#ifdef ALLEGRO_H 
	#define MME_SURFACE			BITMAP *
	#define MME_SCREENBUFFER	screen
#endif

/** Class */
class cMultiMediaEngine {

private:
	
public:
	
	virtual void drawSprite(MME_SURFACE source, MME_SURFACE dest, int x, int y) = 0;
	virtual void doBlit(MME_SURFACE source, MME_SURFACE dest, int x, int y) = 0;
	virtual void doBlit(MME_SURFACE source, MME_SURFACE dest, int x, int y, int width, int height) = 0;
	virtual void doBlit(MME_SURFACE source, MME_SURFACE dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height) = 0;
};


#endif
