#ifndef CSIDEBARDRAWER_H_
#define CSIDEBARDRAWER_H_

class cSideBarDrawer {
	public:
		cSideBarDrawer(cPlayer * thePlayer);
		virtual ~cSideBarDrawer();

		void draw();

		cBuildingListDrawer * getBuildingListDrawer() { return buildingListDrawer; }

    void setPlayer(cPlayer *pPlayer);

protected:
		void drawHouseGui();
		void drawBuildingLists();
		void drawCapacities();

		void drawCandybar();

		void drawMinimap();
	private:
        cPlayer * player;
		cBuildingListDrawer * buildingListDrawer;
		cSideBar * sidebar;

		// the 'candybar' is the bar with the ball at the top. Colored with the house color.
		BITMAP *candybar;
		cTextDrawer *textDrawer;

		int sidebarColor;

        void createCandyBar();

        void drawPowerUsage() const;
        void drawCreditsUsage();
};

#endif /* CSIDEBARDRAWER_H_ */
