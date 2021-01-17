/**
 * a cListItem is  used within a BuildingList , it has a picture and any unit or
 * structure ID attached. At the same time this item is responsible for 'building'
 * the item it represents. It has therefor also other properties; progress a timer, etc.
 *
 *
 */
#ifndef CBUILDINGLISTITEM
#define CBUILDINGLISTITEM

class cBuildingListItem {

public:
    ~cBuildingListItem();

	// uber constructor
    cBuildingListItem(eBuildType type, int buildId, int cost, int icon, int totalBuildTime, cBuildingList *list, int subList, bool queuable);

	// easier constructors
    cBuildingListItem(int theID, s_Structures entry, int subList);
    cBuildingListItem(int theID, s_UnitP entry, int subList);
    cBuildingListItem(int theID, s_Upgrade entry, int subList);

	// gettters
	int getTotalBuildTime() { return totalBuildTime; }
	int getIconId() { return icon; }

	/**
	 * The buildId is the 'type', ie if eBuildType == STRUCTURE, then buildId refers to what *kind* of structure. Ie, the
	 * structure type. This could be WINDTRAP, REFINERY, etc. If eBuildType is UNIT, then buildId is for TANK, QUAD, etc.
	 *
	 * @return
	 */
	int getBuildId() { return buildId; }

	int getSubList() { return subList; }
	eBuildType getBuildType() { return type; }
	int getBuildCost() const { return cost; }
	int getProgress() { return progress; }
	bool isBuilding() { return building; }
	bool isState(eBuildingListItemState value) { return state == value; }

	/**
	 * Returns if item is available to build, if not, then it won't be drawn
	 * @return
	 */
	bool isAvailable() { return isState(eBuildingListItemState::AVAILABLE); }

	/**
	 * Item is pending because upgrade is in progress (used for building items)
	 * @return
	 */
	bool isPendingUpgrading() { return isState(eBuildingListItemState::PENDING_UPGRADE); }

	/**
	 * Item is pending because building is in progress (used for upgrade items)
	 * @return
	 */
	bool isPendingBuilding() { return isState(eBuildingListItemState::PENDING_BUILDING); }

	int getTimesToBuild() { return timesToBuild; }
	int getSlotId() { return slotId; } // return index of items[] array (set after adding item to list, default is < 0)
	int getTimesOrdered() { return timesOrdered; }

	s_Upgrade getS_Upgrade();
	s_UnitP getS_UnitP();
	s_Structures getS_Structures();

	int getCosts();

	float getCreditsPerProgressTime() { return creditsPerProgressTime; }

	float getRefundAmount();

	bool shouldPlaceIt() { return placeIt; }
	bool isQueuable() { return queuable; }

	// setters
	void setIconId(int value) { icon = value; }
	void setBuildCost(int value) { cost = value; }
	void setIsBuilding(bool value) { building = value; }
	void setStatusPendingUpgrade() { state = eBuildingListItemState::PENDING_UPGRADE; }
	void setStatusAvailable() { state = eBuildingListItemState::AVAILABLE; }
	void setStatusPendingBuilding() { state = eBuildingListItemState::PENDING_BUILDING; }
	void setIsAvailable(bool value) { value ? state = eBuildingListItemState::AVAILABLE : eBuildingListItemState::UNAVAILABLE; }
	void setTimesToBuild(int value) { timesToBuild = value; }
	void setTimesOrdered(int value) { timesOrdered = value; }
	void increaseTimesToBuild() { timesToBuild++; }
	void increaseTimesToBuildNTimes(int amount) {
	    if (queuable) {
            timesToBuild += amount;
	    } else {
            timesToBuild = 1; // not queueable always means 1
        }
	}
	void decreaseTimesToBuild();
	void increaseTimesOrdered() { timesOrdered++; }
	void decreaseTimesOrdered() { timesOrdered--; }
	void setSlotId(int value) { slotId = value; }
	void setPlaceIt(bool value) { placeIt = value; }
	void setList(cBuildingList *theList) { myList = theList; }

	cBuildingList *getList() { return myList; }	// returns the list it belongs to

    void increaseProgress(int byAmount);
	void resetProgress() { progress = 0; }

    int getBuildTime();

    bool isDoneBuilding();

	bool isTypeUpgrade();

    void resetTimesOrdered();

private:
	void setProgress(int value) { progress = value; }

	int icon;				// the icon ID to draw (from datafile)
	int buildId;			// the ID to build .. (ie TRIKE, or CONSTYARD)
	eBuildType type;		// .. of this type of thing (ie, UNIT or STRUCTURE)
	int cost;				// price
	bool building;			// building this item? (default = false)
	eBuildingListItemState state;
	int progress;			// progress building this item
	int timesToBuild;		// the amount of times to build this item (queueing) (meaning, when building = true, this should be 1...)
	int timesOrdered;		// the amount of times this item has been ordered (starport related)
	int slotId;			 	// return index of items[] array (set after adding item to list, default is < 0)

	float creditsPerProgressTime; // credits to pay for each progress point. (calculated at creation)
	bool placeIt;			// when true, this item is ready for placement
	bool queuable;			// when true, this item can be ordered multiple times to build

	int totalBuildTime;		// total time it takes to build.
	int subList;            // subList id's allow us to distinguish built items within the same buildingList.

	cBuildingList *myList;

    cBuildingListItem(int theID, s_Structures entry, cBuildingList* list, int subList);
    cBuildingListItem(int theID, s_UnitP entry, cBuildingList* list, int subList);
    cBuildingListItem(int theID, s_Upgrade entry, cBuildingList* list, int subList);
};

#endif
