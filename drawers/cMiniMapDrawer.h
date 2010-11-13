/*
 * cMiniMapDrawer.h
 *
 *  Created on: 20-okt-2010
 *      Author: Stefan
 */

#ifndef CMINIMAPDRAWER_H_
#define CMINIMAPDRAWER_H_

class cMiniMapDrawer {
	public:
		cMiniMapDrawer(cMap *theMap, cPlayer *thePlayer, cMapCamera * theMapCamera);
		~cMiniMapDrawer();

		void draw();
		void interact();

		void setStaticFrame(int value) { iStaticFrame = value; }
		int getStaticFrame() { return iStaticFrame; }

		int getStatus() { return iStatus; }
		void setStatus(int value) { iStatus = value; }

		void drawStaticFrame();

		int getDrawStartX();
		int getDrawStartY();

	protected:

		void drawTerrain();
		void drawViewPortRectangle();
		void drawUnitsAndStructures();

		int getRGBColorForTerrainType(int terrainType);

		void drawDot(int x, int y, int color);

	private:
		cMap *map;	// the minimap drawer reads data from here
		cPlayer *player;	// the player used as 'context' (ie, for drawing the rectangle / viewport on the minimap)
		cMapCamera *mapCamera;

		int iStaticFrame;
	    int iStatus;                       // 0 = show minimap , -1 = no minimap (static animation, of going down), 1 = static animation
	    int iTrans;							// transparancy
};

#endif /* CMINIMAPDRAWER_H_ */
