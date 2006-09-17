#ifdef ALLEGRO_H
#ifndef CMMEALLEGRO_H

class cMMEAllegro : public cMultiMediaEngine {

private:


public:
	cMMEAllegro();
	
	bool setupLib();

	int getMouseX();
	int getMouseY();
	int getMouseZ();

	void drawSprite(MME_SURFACE source, MME_SURFACE dest, int x, int y);
	void doBlit(MME_SURFACE source, MME_SURFACE dest, int x, int y);
	void doBlit(MME_SURFACE source, MME_SURFACE dest, int x, int y, int width, int height);
	void doBlit(MME_SURFACE source, MME_SURFACE dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);

	MME_SURFACE createSurface(int width, int height);
	MME_SURFACE createSurface(int width, int height, int bitdepth);
	void colorSurface(MME_SURFACE surface, int color);
	void clearSurface(MME_SURFACE surface);
};

#endif
#endif
