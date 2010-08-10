/*
 * cMapDrawer.h
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#ifndef CMAPDRAWER_H_
#define CMAPDRAWER_H_

#define TILESIZE_WIDTH_PIXELS 32
#define TILESIZE_HEIGHT_PIXELS 32

class cMapDrawer {
	public:
		cMapDrawer(cMap * theMap, cPlayer * thePlayer);
		~cMapDrawer();

		// draw everything related to the map for the player given in the constructor
		void draw();

	protected:

	private:
		cMap * map;
		cPlayer * player;

    	// the width and height of the viewport
		// calculated at the constructor
    	int viewportWidth;
    	int viewportHeight;
};

#endif /* CMAPDRAWER_H_ */
