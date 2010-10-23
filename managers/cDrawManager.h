/**
	Class responsible for all drawing
*/
#ifndef CDRAWMANAGER_H_
#define CDRAWMANAGER_H_

class cDrawManager {

	public:
		cDrawManager(cPlayer * thePlayer);
		~cDrawManager();

		void draw();

		CreditsDrawer *getCreditsDrawer() { return creditsDrawer; }
		cMessageDrawer *getMessageDrawer() { return messageDrawer; }
		cMiniMapDrawer *getMiniMapDrawer() { return miniMapDrawer; }

	protected:
		void drawSidebar();
		void drawUpgradeButton();
		void drawOrderButton();
		void drawCredits();
		void drawMap();
		void drawStructurePlacing();
		void drawMessage();

	private:
		cSideBarDrawer * sidebarDrawer;
		CreditsDrawer * creditsDrawer;
		cOrderDrawer * orderDrawer;
		cUpgradeDrawer * upgradeDrawer;
		cMapDrawer * mapDrawer;
		cMiniMapDrawer * miniMapDrawer;
		cParticleDrawer * particleDrawer;
		cMessageDrawer * messageDrawer;
		cPlaceItDrawer *placeitDrawer;
		cStructureDrawer * structureDrawer;

		// TODO: unitDrawer
		// TODO: bullet/projectile drawer

		cPlayer * player;
};

#endif
