#ifndef CITEMBUILDER_H_
#define CITEMBUILDER_H_

#include "definitions.h"
#include "enums.h"

#include "observers/cInputObserver.h"

#include "sidebar/cBuildingListItem.h"
#include "sidebar/cBuildingListUpdater.h"

class cPlayer;
class cBuildingListUpdater;

class cItemBuilder : cInputObserver {

	public:
		cItemBuilder(cPlayer * thePlayer, cBuildingListUpdater * buildingListUpdater);
		virtual ~cItemBuilder();

		// think about the progress being made, called every 5 ms
		void thinkFast();

        void onNotifyMouseEvent(const s_MouseEvent &event) override;
        void onNotifyKeyboardEvent(const cKeyboardEvent &event) override;

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

		bool isAnythingBeingBuiltForListType(eListType listType, int sublistType);
        bool isAnythingBeingBuiltForListIdAwaitingPlacement(eListType listType, int sublistType);
        bool isAnythingBeingBuiltForListIdAwaitingDeployment(eListType listType, int sublistType);

        cBuildingListItem *getListItemWhichIsBuilding(eListType listType, int sublistType);
        cBuildingListItem *getListItemWhichIsAwaitingPlacement(eListType listType, int sublistType);
        cBuildingListItem *getListItemWhichIsAwaitingDeployment(eListType listType, int sublistType);

        cPlayer * getPlayer() { return m_player; }

	private:
        cBuildingListItem *m_items[MAX_ITEMS];

        cPlayer * m_player; // the player context for this builder
        cBuildingListUpdater * m_buildingListUpdater;

		int m_timers[MAX_ITEMS];
        bool m_buildItemMultiplierEnabled;

        int getFreeSlot();

		int getTimerCap(cBuildingListItem *item);
		bool isItemInList(cBuildingListItem *item);

		void startBuilding(cBuildingListItem *item);

        cBuildingListItem * getBuildingListItem(eBuildType buildType, int iBuildId);

        void deployUnit(cBuildingListItem *item, int buildId) const;

        void itemIsDoneBuildingLogic(cBuildingListItem *item);

        void onKeyHold(const cKeyboardEvent &event);

        void onKeyPressed(const cKeyboardEvent &event);
};

#endif /* CITEMBUILDER_H_ */
