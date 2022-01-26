#ifndef CMAPDRAWER_H_
#define CMAPDRAWER_H_

class cMapDrawer {
	public:
		cMapDrawer(cMap * map, cPlayer * player, cMapCamera * camera);
		~cMapDrawer();

		void setPlayer(cPlayer * thePlayer);

		void drawTerrain();
		void drawShroud();

        void setDrawWithoutShroudTiles(bool value) { m_drawWithoutShroudTiles = value; }

	protected:


	private:
		cMap * m_map;
		cPlayer * m_player;
		cMapCamera * m_camera;

		// bitmap for drawing tiles, and possibly stretching (depending on zoom level)
        BITMAP *m_BmpTemp;

        bool m_drawWithoutShroudTiles;

        int determineWhichShroudTileToDraw(int cll, int playerId) const;

        void drawCellAsColoredTile(float tileWidth, float tileHeight, int iCell, float fDrawX, float fDrawY);
};

#endif /* CMAPDRAWER_H_ */
