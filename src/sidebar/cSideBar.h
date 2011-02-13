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

// List ID's corresponding buttons
#define LIST_NONE		0
#define LIST_CONSTYARD	1
#define LIST_INFANTRY	2
#define LIST_LIGHTFC	3
#define LIST_HEAVYFC	4
#define LIST_ORNI		5
#define LIST_STARPORT	6
#define LIST_PALACE		7
#define LIST_MAX		8			// max amount of lists

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
		cSideBar(cPlayer *thePlayer);
		~cSideBar();

		cBuildingList * getList(int listId) { return lists[listId]; }
		void setList(int listId, cBuildingList *list);
		int getSelectedListID() { return selectedListID; }

		void think();	// timer based
		void thinkInteraction(); // fps based
		void thinkUpgradeButton(); // fps based

		// set
		void setPlayer(cPlayer *thePlayer) { player = thePlayer; }
		void setSelectedListId(int value) { selectedListID = value; }

	private:

		// the lists:
		cBuildingList * lists[LIST_MAX];
		cPlayer * player;

		int selectedListID;

		// methods
		void thinkAvailabilityLists();
		void thinkMouseZScrolling(); // think about mouse wheel scrolling
};

#endif /* CSIDEBAR_H_ */
