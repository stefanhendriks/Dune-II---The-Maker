/*
 * cMapDrawer.h
 *
 *  Created on: 10-aug-2010
 *      Author: Stefan
 */

#ifndef CMAPDRAWER_H_
#define CMAPDRAWER_H_

class cMapDrawer {
	public:
		cMapDrawer(cMap * theMap, cPlayer * thePlayer, cMapCamera * theCamera);
		~cMapDrawer();

		// draw everything related to the map for the player given in the constructor
		void draw();

	protected:

		void drawTerrain();

	private:
		cMap * map;
		cPlayer * player;
		cMapCamera * camera;

};

#endif /* CMAPDRAWER_H_ */
