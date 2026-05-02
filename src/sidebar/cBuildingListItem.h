/**
 * a cListItem is  used within a BuildingList , it has a picture and any unit or
 * structure ID attached. At the same time this item is responsible for 'building'
 * the item it represents. It has therefor also other properties; progress a timer, etc.
 *
 *
 */
#pragma once

#include <string>
#include "include/enums.h"

class cBuildingList;
struct s_SpecialInfo;
struct s_UpgradeInfo;
struct s_UnitInfo;
struct s_StructureInfo;
struct s_TerrainInfo;

class cBuildingListItem {

public:
    ~cBuildingListItem();

    // uber constructor
    cBuildingListItem(eBuildType type, int buildId, int cost, int icon, cBuildingList *list, int subList, bool queuable);

    // easier constructors (they have no LIST, this is intentional and assigned later to the item by add**toList functions in the cBuildingList.cpp class
    cBuildingListItem(int theID, s_StructureInfo entry, int subList);
    cBuildingListItem(int theID, s_SpecialInfo entry, int subList);
    cBuildingListItem(int theID, s_UnitInfo entry, int subList);
    cBuildingListItem(int theID, s_UpgradeInfo entry);

    static const int DefaultTimerCap = 35;
    static const int DebugTimerCap = 2;

    // getters
    int getTotalBuildTimeInMs();
    int getProgressBuildTimeInMs(); // how much time has passed for build item
    int getIconId() {
        return m_icon;
    }
    std::string getInfo();

    std::string getTypeString();
    std::string getNameString();

    /**
     * The buildId is the 'type', ie if eBuildType == STRUCTURE, then buildId refers to what *kind* of structure. Ie, the
     * structure type. This could be WINDTRAP, REFINERY, etc. If eBuildType is UNIT, then buildId is for TANK, QUAD, etc.
     *
     * @return
     */
    int getBuildId() {
        return m_buildId;
    }

    /**
     * The index of the sublist
     * @return
     */
    int getSubList() {
        return m_subList;
    }

    eBuildType getBuildType() {
        return m_type;
    }
    int getBuildCost() const {
        return m_cost;
    }
    int getProgress() {
        return m_progress;
    }
    bool isBuilding() {
        return m_building;
    }
    bool isState(eBuildingListItemState value) {
        return m_state == value;
    }

    /**
     * Returns if item is available to build, if not, then it won't be drawn
     * @return
     */
    bool isAvailable() {
        return isState(eBuildingListItemState::AVAILABLE);
    }

    /**
     * Item is pending because upgrade is in progress (used for building items)
     * @return
     */
    bool isPendingUpgrading() {
        return isState(eBuildingListItemState::PENDING_UPGRADE);
    }

    /**
     * Item is pending because building is in progress (used for upgrade items)
     * @return
     */
    bool isPendingBuilding() {
        return isState(eBuildingListItemState::PENDING_BUILDING);
    }

    int getTimesToBuild() {
        return m_timesToBuild;
    }
    int getSlotId() {
        return m_slotId;    // return index of items[] array (set after adding item to list, default is < 0)
    }
    int getTimesOrdered() {
        return m_timesOrdered;
    }

    s_SpecialInfo &getSpecialInfo();
    s_UpgradeInfo &getUpgradeInfo();
    s_UnitInfo &getUnitInfo();
    s_StructureInfo &getStructureInfo();

    float getCreditsPerProgressTime() {
        return m_creditsPerProgressTime;
    }

    float getRefundAmount();

    bool shouldPlaceIt() {
        return m_placeIt;
    }
    bool shouldDeployIt() {
        return m_deployIt;
    }
    bool isQueuable() {
        return m_queuable;
    }

    // setters
    void setIconId(int value) {
        m_icon = value;
    }
    void setBuildCost(int value) {
        m_cost = value;
    }
    void setIsBuilding(bool value) {
        m_building = value;
    }
    void stopBuilding() {
        setIsBuilding(false);
        resetProgress();
    }
    void setStatusPendingUpgrade() {
        m_state = eBuildingListItemState::PENDING_UPGRADE;
    }
    void setStatusAvailable() {
        m_state = eBuildingListItemState::AVAILABLE;
    }
    void setStatusPendingBuilding() {
        m_state = eBuildingListItemState::PENDING_BUILDING;
    }
    void setIsAvailable(bool value) {
        value ? m_state = eBuildingListItemState::AVAILABLE : eBuildingListItemState::UNAVAILABLE;
    }
    void setTimesToBuild(int value) {
        m_timesToBuild = value;
    }
    void setTimesOrdered(int value) {
        m_timesOrdered = value;
    }
    void increaseTimesToBuild() {
        m_timesToBuild++;
    }
    void increaseTimesToBuildNTimes(int amount) {
        if (m_queuable) {
            m_timesToBuild += amount;
        }
        else {
            m_timesToBuild = 1; // not queueable always means 1
        }
    }
    void decreaseTimesToBuild();
    void increaseTimesOrdered() {
        m_timesOrdered++;
    }
    void decreaseTimesOrdered() {
        m_timesOrdered--;
    }
    void setSlotId(int value) {
        m_slotId = value;
    }
    void setPlaceIt(bool value) {
        m_placeIt = value;
    }
    void setDeployIt(bool value) {
        m_deployIt = value;
    }
    void setList(cBuildingList *theList) {
        m_myList = theList;
    }

    cBuildingList *getList() {
        return m_myList;    // returns the list it belongs to
    }

    /**
     * List type (integer)
     * @return
     */
    eListType getListType();

    void increaseProgress(int byAmount);

    void resetProgress() {
        m_progress = 0;
        m_buildFrameToDraw = 0;
        m_TIMER_progressFrame = 0.0f;
    }

    int getTotalBuildTime() const;

    bool isDoneBuilding();

    bool isTypeUpgrade();
    bool isTypeSpecial();
    bool isTypeUnit();
    bool isTypeStructure();

    void resetTimesOrdered();

    void increaseBuildProgressFrame() {
        m_buildFrameToDraw++;
    }
    int getBuildProgressFrame() {
        return m_buildFrameToDraw;
    }

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
    cBuildingListItem(int theID, s_UpgradeInfo entry, int subList);

    void setProgress(int value) {
        m_progress = value;
    }

    int m_icon;				// the icon ID to draw (from datafile)
    int m_buildId;			// the ID to build .. (ie TRIKE, or CONSTYARD)
    eBuildType m_type;		// .. of this type of thing (ie, UNIT or STRUCTURE)
    int m_cost;				// price
    bool m_building;			// building this item? (default = false)
    eBuildingListItemState m_state;
    int m_progress;			// progress building this item
    int m_buildFrameToDraw;   // for the progress drawing
    int m_timesToBuild;		// the amount of times to build this item (queueing) (meaning, when building = true, this should be 1...)
    int m_timesOrdered;		// the amount of times this item has been ordered (starport related)
    int m_slotId;			 	// return index of items[] array (set after adding item to list, default is < 0)

    float m_creditsPerProgressTime; // credits to pay for each progress point. (calculated at creation)
    bool m_placeIt;			// when true, this item is ready for placement
    bool m_deployIt;			// when true, this item is ready for deployment (FYI, super weapons)
    bool m_queuable;			// when true, this item can be ordered multiple times to build

    int m_subList;            // subList id's allow us to distinguish built items within the same buildingList.

    float m_TIMER_progressFrame; // timer used for progress drawing animation
    int m_TIMER_flashing;        // if > 0 then this item is 'flashing'
    int m_timerCap;           // passed in by item builder (determined by power outage, etc)

    cBuildingList *m_myList;

    cBuildingListItem(int theID, s_StructureInfo entry, cBuildingList *list, int subList);
    cBuildingListItem(int theID, s_UnitInfo entry, cBuildingList *list, int subList);
    cBuildingListItem(int theID, s_UpgradeInfo entry, cBuildingList *list, int subList);
    cBuildingListItem(int theID, s_SpecialInfo entry, cBuildingList *list, int subList);

    bool isType(eBuildType value);

    int getInTicks(int getTimeInTicks) const;
};
