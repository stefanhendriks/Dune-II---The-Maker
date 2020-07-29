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
		cMapDrawer(cMap * theMap, const cPlayer& thePlayer, cMapCamera * theCamera);
		~cMapDrawer();

		void drawTerrain();
		void drawShroud();

	protected:


	private:
		cMap * map;
		const cPlayer& player;
		cMapCamera * camera;
		cCellCalculator * cellCalculator;
};

#endif /* CMAPDRAWER_H_ */
