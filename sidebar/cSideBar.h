/*
 * cSideBar.h
 *
 * The sidebar consists out of several parts:
 * - buildLists (the icons that represent them, and a list of icons of the
 *   selected list)
 * - a minimap
 * - capacity indicators
 *
 *  Created on: Aug 1, 2009
 *      Author: Stefan
 */

#ifndef CSIDEBAR_H_
#define CSIDEBAR_H_

// forward declaration
class cPlayer;
class cUpgradeUtils;

// List ID's corresponding buttons
#define LIST_NONE		0
#define LIST_CONSTYARD	1
#define LIST_FOOT_UNITS	2 // infantry, soldier, trooper, troopers, etc.
#define LIST_UNITS	    3 // quad, trike, tanks, air units
#define LIST_STARPORT	4 // special list to order stuff
#define LIST_PALACE		5 // REMOVE? (ie super weapon icon should always show top left/right?)
#define LIST_MAX		6 // max amount of lists

#define SUBLIST_LIGHTFCTRY 0 // quad, trike, etc
#define SUBLIST_HEAVYFCTRY 1 // tanks
#define SUBLIST_HIGHTECH   2 // orni, carry-all

/**
 * Basically a container:
 * - building lists
 * - minimap
 * - capacities (max spice, max power)
 *
 * NOTE: Use the cSideBarFactory to construct a cSideBar class.
 *
 */
class cSideBar {

	public:
		cSideBar(cPlayer& thePlayer);
		~cSideBar();

		cBuildingList * getList(int listId) { return lists[listId]; }
		void setList(int listId, cBuildingList *list);
		int getSelectedListID() { return selectedListID; }

		void think();	// timer based
		void thinkInteraction(); // fps based
		void thinkUpgradeButton(); // fps based

		// set
		// no set Player, re-create Sidebar object instead if needed
		void setSelectedListId(int value) { selectedListID = value; }

		static const int VerticalCandyBarWidth = 24;
		static const int SidebarWidthWithoutCandyBar = 200;
		static const int SidebarWidth = SidebarWidthWithoutCandyBar + VerticalCandyBarWidth; // including vertical candy bar
		static const int TopBarHeight = 42;
		static const int HeightOfMinimap = 200 + 8;

	private:

        cUpgradeUtils * upgradeUtils;

        // the lists:
		cBuildingList * lists[LIST_MAX];
		cPlayer & m_Player;

		int selectedListID;

		// methods
		void thinkAvailabilityLists();
};

#endif /* CSIDEBAR_H_ */
