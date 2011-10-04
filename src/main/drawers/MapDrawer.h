#ifndef CMAPDRAWER_H_
#define CMAPDRAWER_H_

class MapDrawer {
	public:
		MapDrawer(cMap * theMap, cPlayer * thePlayer, cMapCamera * theCamera);
		~MapDrawer();

		void drawTerrain();
		void drawShroud();

	protected:

	private:
		cMap * map;
		cPlayer * player;
		cMapCamera * camera;
};

#endif /* CMAPDRAWER_H_ */
