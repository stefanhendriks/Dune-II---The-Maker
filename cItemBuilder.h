/*
 * cItemBuilder.h
 *
 * Responsible for building items (cBuildListItems)
 *
 *  Created on: Aug 8, 2009
 *      Author: Stefan
 */

#ifndef CITEMBUILDER_H_
#define CITEMBUILDER_H_

class cItemBuilder {

	public:
		static cItemBuilder *getInstance();

		cItemBuilder();
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

		int getFreeSlot();

	private:
		cBuildingListItem *items[MAX_ITEMS];
		int timers[MAX_ITEMS];

		int getTimerCap(cBuildingList *list, cBuildingListItem *item);
		int getBuildTime(cBuildingList *list, cBuildingListItem *item);
		bool isItemInList(cBuildingListItem *item);

		static cItemBuilder *instance;
};

#endif /* CITEMBUILDER_H_ */
