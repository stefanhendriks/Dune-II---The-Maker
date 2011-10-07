/*
 * cItemBuilder.h
 *
 * Responsible for building items (cBuildListItems)
 *
 * Each player has an itemBuilder (also AI). The correct setup should
 * be done by the cPlayerFactory.
 *
 *  Created on: Aug 8, 2009
 *      Author: Stefan
 */

#ifndef CITEMBUILDER_H_
#define CITEMBUILDER_H_

#include "../include/definitions.h"
#include "../sidebar/cBuildingListItem.h"


// forward declaration
class cPlayer;


class cItemBuilder {

	public:
		cItemBuilder(cPlayer * thePlayer);
		virtual ~cItemBuilder();

		// think about the progress being made (time based thinker)
		void think();

		cBuildingListItem * getItem(int position); // return icon from list

		// add item to the list
		void addItemToList(cBuildingListItem * item);
		void removeItemFromList(int position);
		void removeAllItems();

		bool isTheFirstListType(cBuildingListItem *item);
		cBuildingListItem *getSimilarListType(cBuildingListItem *item);

		void removeItemFromList(cBuildingListItem *item);
		bool isASimilarItemBeingBuilt(cBuildingListItem *item);

		int getFreeSlot();

		void setPlayer(cPlayer * thePlayer) {
			player = thePlayer;
		}

	private:
		cBuildingListItem *items[MAX_ITEMS];
		cPlayer * player; // the player context for this builder

		int timers[MAX_ITEMS];

		int getTimerCap(cBuildingList *list, cBuildingListItem *item);
		int getBuildTime(cBuildingList *list, cBuildingListItem *item);
		bool isItemInList(cBuildingListItem *item);
};

#endif /* CITEMBUILDER_H_ */
