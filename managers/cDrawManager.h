/**
	Class responsible for all drawing
*/
#ifndef CDRAWMANAGER_H_
#define CDRAWMANAGER_H_

class cDrawManager {

	public:
		cDrawManager(cPlayer * thePlayer);
		~cDrawManager();

		void drawCombatState();

		CreditsDrawer *getCreditsDrawer() { return creditsDrawer; }
		cMessageDrawer *getMessageDrawer() { return messageDrawer; }
		cMiniMapDrawer *getMiniMapDrawer() { return miniMapDrawer; }
		cOrderDrawer *getOrderDrawer() { return orderDrawer; }
		cBuildingListDrawer *getBuildingListDrawer() { return sidebarDrawer->getBuildingListDrawer(); }

        void drawMouse();

    protected:
		void drawSidebar();
		void drawCredits();
		void drawStructurePlacing();
		void drawMessage();
		void drawCombatMouse();
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

		cPlayer * m_Player;
};

#endif
