/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://d2tm.duneii.com

  2001 - 2010 (c) code by Stefan Hendriks

  A player has a sidebar. The sidebar contains lists of items that can be built. These
  items can be structures/units but also special weapons, etc.

  In order to build items, a player has an itemBuilder.

  A player can upgrade the sidebar lists. Therefore a cBuildingListUpgrader is used.

  */
#ifndef PLAYER_H
#define PLAYER_H

class cPlayer {

	public:
		cPlayer();
		~cPlayer();

		PALETTE pal;		// each player has its own 256 color scheme (used for coloring units)

		void set_house(int iHouse);
		void init();

		int iTeam;

		int use_power;
		int has_power;

		float credits;		// the credits this player has
		float max_credits;	// max credits a player can have (influenced by silo's)

		int focus_cell;		// this is the cell that will be showed in the game centralized upon map loading

		int house;			// house

		int iPrimaryBuilding[MAX_STRUCTURETYPES];	// remember the primary ID (structure id) of each structure type
		int iStructures[MAX_STRUCTURETYPES]; // remember what is built for each type of structure

		bool bEnoughPower();

		int TIMER_think;        // timer for thinking itself (calling main routine)
		int TIMER_attack;       // -1 = determine if its ok to attack, > 0 is , decrease timer, 0 = attack

		int upgradeLevel[LIST_MAX];

		// set
		void setItemBuilder(cItemBuilder *theItemBuilder);
		void setSideBar(cSideBar *theSideBar);

		// get
		cPlayerDifficultySettings *getDifficultySettings() { return difficultySettings; }
		cItemBuilder *getItemBuilder() { return itemBuilder; }
		cSideBar *getSideBar() { return sidebar; }

		// delete
		void deleteSideBar() { if (sidebar) delete sidebar; }

	private:
		cPlayerDifficultySettings *difficultySettings;

		cItemBuilder * itemBuilder; // each player can build items
		cSideBar * sidebar;			// each player has a sidebar
};

#endif
