#ifdef ALLEGRO_H
#ifndef CMMEALLEGRO_H

class cMMEAllegro : public cMultiMediaEngine {

private:

public:
	cMMEAllegro();

	void drawSprite(MME_SURFACE source, MME_SURFACE dest, int x, int y);
	void doBlit(MME_SURFACE source, MME_SURFACE dest, int x, int y);
	void doBlit(MME_SURFACE source, MME_SURFACE dest, int x, int y, int width, int height);
	void doBlit(MME_SURFACE source, MME_SURFACE dest, int source_x, int source_y, int dest_x, int dest_y, int width, int height);
};

#endif
#endif
