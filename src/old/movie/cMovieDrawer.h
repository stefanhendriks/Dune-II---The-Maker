#ifndef CMOVIEDRAWER_H_
#define CMOVIEDRAWER_H_

#include "cMoviePlayer.h"

class cMovieDrawer {
	public:
		cMovieDrawer(cMoviePlayer * playerToDraw, BITMAP * bitmap);
		~cMovieDrawer();

		void drawIfPlaying(int x, int y);

	protected:

	private:
		cMoviePlayer * player;
		BITMAP * destinationBitmap;
};

#endif /* CMOVIEDRAWER_H_ */
