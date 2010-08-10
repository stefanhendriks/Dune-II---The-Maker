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

	protected:
		void drawSidebar();
		void drawUpgradeButton();
		void drawOrderButton();
		void drawCredits();

	private:
		cSideBarDrawer * sidebarDrawer;
		CreditsDrawer * creditsDrawer;
		cOrderDrawer * orderDrawer;
		cUpgradeDrawer * upgradeDrawer;
		cPlayer * player;
};

#endif
