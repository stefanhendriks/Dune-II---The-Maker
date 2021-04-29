/*
 * cMiniMapDrawer.h
 *
 *  Created on: 20-okt-2010
 *      Author: Stefan
 */

#ifndef CMINIMAPDRAWER_H_
#define CMINIMAPDRAWER_H_

// the BuildingListItemState
enum eMinimapStatus {
    NOTAVAILABLE, // show logo / no radar available
    POWERUP,      // powering up (radar available & enough power)
    RENDERMAP,    // map is rendering (radar available & enough power)
    POWERDOWN,    // powering down (radar available & not enough power)
    LOWPOWER,     // map is not rendered (radar available & not enough power)
};

class cMiniMapDrawer {
	public:
		cMiniMapDrawer(cMap *theMap, cPlayer * thePlayer, cMapCamera * theMapCamera);
		~cMiniMapDrawer();

		void draw();

		void setStaticFrame(int value) { iStaticFrame = value; }
		int getStaticFrame() { return iStaticFrame; }

		void setStatus(eMinimapStatus value) { status = value; }

		void drawStaticFrame();

        int getMouseCell(int mouseX, int mouseY);

        void think();

        bool isMouseOver();

        void setPlayer(cPlayer * thePlayer);

	    void onNotify(const s_MouseEvent &event);

protected:

		void drawTerrain();
		void drawViewPortRectangle();
		void drawUnitsAndStructures(bool playerOnly);

		int getRGBColorForTerrainType(int terrainType);

		void drawDoubleDot(int x, int y, int color);
		void drawSingleDot(int x, int y, int color);

		int getMapHeightInPixels();
		int getMapWidthInPixels();

	private:
		void onMouseAt(const s_MouseEvent &event);
		void onMousePressedLeft(const s_MouseEvent &event);

		bool _isMouseOver;

		cMap *map;	// the minimap drawer reads data from here
		cPlayer * m_Player;	// the m_Player used as 'context' (ie, for drawing the rectangle / viewport on the minimap)
		cMapCamera *mapCamera;
		cRectangle *m_RectMinimap; // the minimap (map) itself
		cRectangle *m_RectFullMinimap; // the total space it could take

        eMinimapStatus status;

        int iStaticFrame;
	    int iTrans;							// transparancy

	    // the top left coordinates for the minimap
	    int drawX, drawY;
};

#endif /* CMINIMAPDRAWER_H_ */
