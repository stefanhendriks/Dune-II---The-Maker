#ifndef CMAPDRAWER_H_
#define CMAPDRAWER_H_

class cMapDrawer {
	public:
		cMapDrawer(cMap * theMap, cPlayer * thePlayer, cMapCamera * theCamera);
		~cMapDrawer();

		void setPlayer(cPlayer * thePlayer);

		void drawTerrain(int startX, int startY);
		void drawShroud(int startX, int startY);

	protected:


	private:
		cMap * map;
		cPlayer * player;
		cMapCamera * camera;

		// bitmap for drawing tiles, and possibly stretching (depending on zoom level)
        BITMAP *bmp_temp;

    int determineWhichShroudTileToDraw(int cll, int playerId) const;

    void drawCellAsColoredTile(float tileWidth, float tileHeight, int iCell, float fDrawX, float fDrawY);
};

#endif /* CMAPDRAWER_H_ */
