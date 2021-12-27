#ifndef CITEMBUILDER_H_
#define CITEMBUILDER_H_

#include "definitions.h"
#include "enums.h"

#include "sidebar/cBuildingListItem.h"
#include "sidebar/cBuildingListUpdater.h"

class cPlayer;

class cItemBuilder {

	public:
		cItemBuilder(cPlayer * thePlayer, cBuildingListUpdater * buildingListUpdater);
		virtual ~cItemBuilder();

		// think about the progress being made (time based thinker)
		void think();

		cBuildingListItem * getItem(int position); // return icon from list

		// add item to the list
		void addItemToList(cBuildingListItem * item);
        void removeItemsFromListType(eListType listType, int subListId);
        void removeItemFromList(int position);
        void removeItemsByBuildId(eBuildType buildType, int buildId);
		void removeAllItems();

		bool isBuildListItemTheFirstOfItsListType(cBuildingListItem *item);
		cBuildingListItem *findBuildingListItemOfSameListAs(cBuildingListItem *item);

		void removeItemFromList(cBuildingListItem *item);
		bool isAnotherBuildingListItemInTheSameListBeingBuilt(cBuildingListItem *item);

		bool isAnythingBeingBuiltForListId(int listType, int sublistType);
        bool isAnythingBeingBuiltForListIdAwaitingPlacement(int listType, int sublistType);
        bool isAnythingBeingBuiltForListIdAwaitingDeployment(int listType, int sublistType);

        cBuildingListItem *getListItemWhichIsBuilding(int listType, int sublistType);
        cBuildingListItem *getListItemWhichIsAwaitingPlacement(int listType, int sublistType);
        cBuildingListItem *getListItemWhichIsAwaitingDeployment(int listType, int sublistType);

        cPlayer * getPlayer() { return player; };

	private:
        cBuildingListItem *items[MAX_ITEMS];
        int getFreeSlot();

        cPlayer * player; // the player context for this builder
        cBuildingListUpdater * buildingListUpdater;

		int timers[MAX_ITEMS];

		int getTimerCap(cBuildingListItem *item);
		bool isItemInList(cBuildingListItem *item);

		void startBuilding(cBuildingListItem *item);

        cBuildingListItem * getBuildingListItem(eBuildType buildType, int iBuildId);

        void deployUnit(cBuildingListItem *item, int buildId) const;

    void itemIsDoneBuildingLogic(cBuildingListItem *item);
};

#endif /* CITEMBUILDER_H_ */
