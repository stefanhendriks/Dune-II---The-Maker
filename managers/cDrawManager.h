/**
	Class responsible for all drawing
*/
#ifndef CDRAWMANAGER_H_
#define CDRAWMANAGER_H_

class cDrawManager {

	public:
		cDrawManager(const cPlayer & thePlayer);
		~cDrawManager();

		void draw();

		CreditsDrawer *getCreditsDrawer() { return creditsDrawer; }
		cMessageDrawer *getMessageDrawer() { return messageDrawer; }
		cMessageBarDrawer *getMessageBarDrawer() { return messageBarDrawer; }
		cMiniMapDrawer *getMiniMapDrawer() { return miniMapDrawer; }
		cSideBarDrawer *getSidebarDrawer() { return sidebarDrawer; }
		cOrderDrawer *getOrderDrawer() { return orderDrawer; }
		cBuildingListDrawer *getBuildingListDrawer() { return sidebarDrawer->getBuildingListDrawer(); }

	protected:
		void drawSidebar();
		void drawCredits();
		void drawStructurePlacing();
		void drawMessage();
		void drawMouse();
		void drawRallyPoint();

	private:
		cSideBarDrawer * sidebarDrawer;
		CreditsDrawer * creditsDrawer;
		cOrderDrawer * orderDrawer;
		cMapDrawer * mapDrawer;
		cMiniMapDrawer * miniMapDrawer;
		cParticleDrawer * particleDrawer;
		cMessageDrawer * messageDrawer;
		cMessageBarDrawer * messageBarDrawer;
		cPlaceItDrawer *placeitDrawer;
		cStructureDrawer * structureDrawer;
		cMouseDrawer * mouseDrawer;

		// TODO: unitDrawer

		// TODO: bullet/projectile drawer

		const cPlayer& m_Player;
};

#endif
