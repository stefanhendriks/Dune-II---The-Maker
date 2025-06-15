/**
 * a cListItem is  used within a BuildingList , it has a picture and any unit or
 * structure ID attached. At the same time this item is responsible for 'building'
 * the item it represents. It has therefor also other properties; progress a timer, etc.
 *
 *
 */
#pragma once

#include "structs.h"

#include <string>

class cBuildingList;

class cBuildingListItem {

public:
    ~cBuildingListItem();

	// uber constructor
    cBuildingListItem(eBuildType type, int buildId, int cost, int icon, cBuildingList *list, int subList, bool queuable);

	// easier constructors (they have no LIST, this is intentional and assigned later to the item by add**toList functions in the cBuildingList.cpp class
    cBuildingListItem(int theID, s_StructureInfo entry, int subList);
    cBuildingListItem(int theID, s_SpecialInfo entry, int subList);
    cBuildingListItem(int theID, s_UnitInfo entry, int subList);
    cBuildingListItem(int theID, s_UpgradeInfo entry, int subList);

    static const int DefaultTimerCap = 35;
    static const int DebugTimerCap = 2;

	// getters
	int getTotalBuildTimeInMs();
	int getProgressBuildTimeInMs(); // how much time has passed for build item
	int getIconId() { return icon; }
    std::string getInfo();

    std::string getTypeString();
    std::string getNameString();

	/**
	 * The buildId is the 'type', ie if eBuildType == STRUCTURE, then buildId refers to what *kind* of structure. Ie, the
	 * structure type. This could be WINDTRAP, REFINERY, etc. If eBuildType is UNIT, then buildId is for TANK, QUAD, etc.
	 *
	 * @return
	 */
	int getBuildId() { return buildId; }

	/**
	 * The index of the sublist
	 * @return
	 */
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

	s_SpecialInfo& getSpecialInfo();
	s_UpgradeInfo& getUpgradeInfo();
	s_UnitInfo& getUnitInfo();
	s_StructureInfo& getStructureInfo();

	int getCosts();

	float getCreditsPerProgressTime() { return creditsPerProgressTime; }

	float getRefundAmount();

	bool shouldPlaceIt() { return placeIt; }
	bool shouldDeployIt() { return deployIt; }
	bool isQueuable() { return queuable; }

	// setters
	void setIconId(int value) { icon = value; }
	void setBuildCost(int value) { cost = value; }
	void setIsBuilding(bool value) { building = value; }
	void stopBuilding() {
	    setIsBuilding(false);
	    resetProgress();
	}
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
	void setDeployIt(bool value) { deployIt = value; }
	void setList(cBuildingList *theList) { myList = theList; }

	cBuildingList *getList() { return myList; }	// returns the list it belongs to

    /**
     * List type (integer)
     * @return
     */
    eListType getListType();

    void increaseProgress(int byAmount);

    void resetProgress() {
	    progress = 0;
        buildFrameToDraw = 0;
        TIMER_progressFrame = 0.0f;
	}

    int getTotalBuildTime() const;

    bool isDoneBuilding();

	bool isTypeUpgrade();
	bool isTypeSpecial();
	bool isTypeUnit();
	bool isTypeStructure();

    void resetTimesOrdered();

    void increaseBuildProgressFrame() { buildFrameToDraw++; }
    int getBuildProgressFrame() { return buildFrameToDraw; }

    int calculateBuildProgressFrameBasedOnBuildProgress();

    void decreaseProgressFrameTimer();

    float getProgressFrameTimer();

    void resetProgressFrameTimer();

    void setTimerCap(int i);

    int getTotalBuildTimeInTicks() const;

    static int getTotalBuildTimeInTicks(eBuildType type, int buildId);
    static int getListId(eBuildType type, int buildId);
    static bool isAutoBuild(eBuildType type, int buildId);

    bool isAutoBuild();

    bool isFlashing();

    void decreaseFlashingTimer();

private:
	void setProgress(int value) { progress = value; }

	int icon;				// the icon ID to draw (from datafile)
	int buildId;			// the ID to build .. (ie TRIKE, or CONSTYARD)
	eBuildType type;		// .. of this type of thing (ie, UNIT or STRUCTURE)
	int cost;				// price
	bool building;			// building this item? (default = false)
	eBuildingListItemState state;
	int progress;			// progress building this item
	int buildFrameToDraw;   // for the progress drawing
	int timesToBuild;		// the amount of times to build this item (queueing) (meaning, when building = true, this should be 1...)
	int timesOrdered;		// the amount of times this item has been ordered (starport related)
	int slotId;			 	// return index of items[] array (set after adding item to list, default is < 0)

	float creditsPerProgressTime; // credits to pay for each progress point. (calculated at creation)
	bool placeIt;			// when true, this item is ready for placement
	bool deployIt;			// when true, this item is ready for deployment (FYI, super weapons)
	bool queuable;			// when true, this item can be ordered multiple times to build

	int subList;            // subList id's allow us to distinguish built items within the same buildingList.

	float TIMER_progressFrame; // timer used for progress drawing animation
    int TIMER_flashing;        // if > 0 then this item is 'flashing'
	int timerCap;           // passed in by item builder (determined by power outage, etc)

	cBuildingList *myList;

    cBuildingListItem(int theID, s_StructureInfo entry, cBuildingList* list, int subList);
    cBuildingListItem(int theID, s_UnitInfo entry, cBuildingList* list, int subList);
    cBuildingListItem(int theID, s_UpgradeInfo entry, cBuildingList* list, int subList);
    cBuildingListItem(int theID, s_SpecialInfo entry, cBuildingList *list, int subList);

    bool isType(eBuildType value);

    int getInTicks(int getTimeInTicks) const;
};
