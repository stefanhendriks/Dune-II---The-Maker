#ifndef CMAPDRAWER_H_
#define CMAPDRAWER_H_

class cMapDrawer {
	public:
		cMapDrawer(cMap * theMap, cPlayer * thePlayer, cMapCamera * theCamera);
		~cMapDrawer();

		void drawTerrain();
		void drawShroud();

	protected:

	private:
		cMap * map;
		cPlayer * player;
		cMapCamera * camera;
};

#endif /* CMAPDRAWER_H_ */
