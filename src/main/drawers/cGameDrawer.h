/**
 Class responsible for all drawing
 */
#ifndef CGAMEDRAWER_H_
#define CGAMEDRAWER_H_

class cGameDrawer {

	public:
		cGameDrawer(cPlayer * thePlayer);
		~cGameDrawer();

		void draw();
		void destroy();

		CreditsDrawer *getCreditsDrawer() {
			return creditsDrawer;
		}
		cMessageDrawer *getMessageDrawer() {
			return messageDrawer;
		}
		cMessageBarDrawer *getMessageBarDrawer() {
			return messageBarDrawer;
		}
		cMiniMapDrawer *getMiniMapDrawer() {
			return miniMapDrawer;
		}
		cSideBarDrawer *getSidebarDrawer() {
			return sidebarDrawer;
		}
		cMouseDrawer *getMouseDrawer() {
			return mouseDrawer;
		}
		cGuiDrawer * getGuiDrawer() {
			return guiDrawer;
		}

		int getDrawXForCell(int cell);
		int getDrawYForCell(int cell);

	protected:
		void drawSidebar();
		void drawUpgradeButton();
		void drawOrderButton();
		void drawCredits();
		void drawMap();
		void drawStructurePlacing();
		void drawMessage();
		void drawMouse();
		void drawRallyPoint();

	private:
		cSideBarDrawer * sidebarDrawer;
		CreditsDrawer * creditsDrawer;
		cOrderDrawer * orderDrawer;
		cUpgradeDrawer * upgradeDrawer;
		cMapDrawer * mapDrawer;
		cMiniMapDrawer * miniMapDrawer;
		cParticleDrawer * particleDrawer;
		cMessageDrawer * messageDrawer;
		cMessageBarDrawer * messageBarDrawer;
		cPlaceItDrawer *placeitDrawer;
		cStructureDrawer * structureDrawer;
		cMouseDrawer * mouseDrawer;
		cGuiDrawer * guiDrawer;

		// TODO: unitDrawer

		// TODO: bullet/projectile drawer

		cPlayer * player;
};

#endif
