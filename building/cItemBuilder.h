#ifndef CITEMBUILDER_H_
#define CITEMBUILDER_H_

// forward declaration
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
		void removeItemFromList(int position);
		void removeAllItems();

		bool isBuildListItemTheFirstOfItsListType(cBuildingListItem *item);
		cBuildingListItem *findBuildingListItemOfSameListAs(cBuildingListItem *item);

		void removeItemFromList(cBuildingListItem *item);
		bool isAnotherBuildingListItemInTheSameListBeingBuilt(cBuildingListItem *item);
        bool isAnythingBeingBuiltForListId(int listType, int sublistType);

		int getFreeSlot();

	private:
		cBuildingListItem *items[MAX_ITEMS];
		cPlayer * m_Player; // the m_Player context for this builder
        cBuildingListUpdater * buildingListUpdater;

		int timers[MAX_ITEMS];

		int getTimerCap(cBuildingListItem *item);
		bool isItemInList(cBuildingListItem *item);

		void startBuilding(cBuildingListItem *item);

        cBuildingListItem * getBuildingListItem(eBuildType buildType, int iBuildId);
};

#endif /* CITEMBUILDER_H_ */
