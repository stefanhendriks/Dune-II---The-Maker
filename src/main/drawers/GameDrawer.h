/**
 Class responsible for all drawing
 */
#ifndef CGAMEDRAWER_H_
#define CGAMEDRAWER_H_

class GameDrawer {

	public:
		GameDrawer(cPlayer * thePlayer);
		~GameDrawer();

		void draw();
		void destroy();

		void setMessage(const char msg[255]) {
			messageBarDrawer->setMessage(msg);
		}

		CreditsDrawer *getCreditsDrawer() {
			return creditsDrawer;
		}

		MessageBarDrawer *getMessageBarDrawer() {
			return messageBarDrawer;
		}
		MiniMapDrawer *getMiniMapDrawer() {
			return miniMapDrawer;
		}
		SideBarDrawer *getSidebarDrawer() {
			return sidebarDrawer;
		}
		MouseDrawer *getMouseDrawer() {
			return mouseDrawer;
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
		SideBarDrawer * sidebarDrawer;
		CreditsDrawer * creditsDrawer;
		OrderDrawer * orderDrawer;
		UpgradeDrawer * upgradeDrawer;
		MapDrawer * mapDrawer;
		MiniMapDrawer * miniMapDrawer;
		ParticleDrawer * particleDrawer;
		MessageBarDrawer * messageBarDrawer;
		PlaceItDrawer *placeitDrawer;
		StructureDrawer * structureDrawer;
		MouseDrawer * mouseDrawer;

		// TODO: unitDrawer

		// TODO: bullet/projectile drawer

		cPlayer * player;
};

#endif
