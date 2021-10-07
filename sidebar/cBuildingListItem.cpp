#include "../include/d2tmh.h"
#include "cBuildingListItem.h"


/**
 *
 * @param type
 * @param buildId (id of the TYPE, ie structureType ID or unitType ID or upgradeType ID)
 * @param cost
 * @param icon
 * @param totalBuildTime
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(eBuildType type, int buildId, int cost, int icon, int totalBuildTime, cBuildingList *list, int subList, bool queuable) {
    assert(buildId >= 0);
    this->buildId = buildId;
    this->cost = cost;
    this->icon = icon;
    this->totalBuildTime = totalBuildTime;
    this->type = type;
    this->queuable = queuable;
    timerCap = DEBUGGING ? cBuildingListItem::DebugTimerCap : cBuildingListItem::DefaultTimerCap;
    TIMER_flashing = 500;
    progress = 0;
    buildFrameToDraw = 0;
    state = AVAILABLE;
    building = false;
    myList = list; // this can be nullptr! (it will be set from the outside by cBuildingList convenience methods)
    timesToBuild = 0;
    timesOrdered  = 0;
    slotId = -1; // is set later
    this->subList = subList;
    if (cost > 0 && totalBuildTime > 0) {
        creditsPerProgressTime = (float)this->cost / (float)this->totalBuildTime;
    }
    placeIt = false;
    deployIt = false;
    TIMER_progressFrame = 0.0f;
}

cBuildingListItem::~cBuildingListItem() {
    myList = NULL;
}

/**
 * Constructor for Structures
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_StructureInfo entry, cBuildingList *list, int subList) :
                    cBuildingListItem(STRUCTURE, theID, entry.cost, entry.icon, entry.build_time, list, subList, entry.queuable) {
}

/**
 * Constructor for Specials
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_SpecialInfo entry, cBuildingList *list, int subList) :
                    cBuildingListItem(SPECIAL, theID, 0, entry.icon, entry.buildTime, list, subList, false) {
}

/**
 * Constructor for Upgrades
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_UpgradeInfo entry, cBuildingList *list, int subList) :
                    cBuildingListItem(UPGRADE, theID, entry.cost, entry.icon, entry.buildTime, list, subList, false) {
}

cBuildingListItem::cBuildingListItem(int theID, s_StructureInfo entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

cBuildingListItem::cBuildingListItem(int theID, s_SpecialInfo entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

/**
 * Constructor for units
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_UnitInfo entry, cBuildingList *list, int subList) :
                    cBuildingListItem(UNIT, theID, entry.cost, entry.icon, entry.build_time, list, subList, entry.queuable) {
}

cBuildingListItem::cBuildingListItem(int theID, s_UnitInfo entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

cBuildingListItem::cBuildingListItem(int theID, s_UpgradeInfo entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList) {
}

int cBuildingListItem::getCosts() {
    int costs = 0;

    if (type == UNIT) {
        costs = sUnitInfo[buildId].cost;
    } else if (type == STRUCTURE) {
        costs = sStructureInfo[buildId].cost;
    } else if (type == UPGRADE) {
        costs = sUpgradeInfo[buildId].cost;
    }
    return costs;
}

/**
 * Return the amount of money the player gets back
 * @return
 */
float cBuildingListItem::getRefundAmount() {
	float fProgress = progress;
	if (fProgress < 1.0F) {
		return 0.0F;
	}
	return (fProgress * creditsPerProgressTime);
}

void cBuildingListItem::decreaseTimesToBuild() {
	timesToBuild--;
}

void cBuildingListItem::increaseProgress(int byAmount) {
    setProgress(getProgress() + byAmount);
}

/**
 * Duplicated by getTotalBuildtime!?
 * @return
 */
int cBuildingListItem::getBuildTime() {
//    if (DEBUGGING) return 1;
    if (type == STRUCTURE) {
        return sStructureInfo[buildId].build_time;
    }
    if (type == UPGRADE) {
        return sUpgradeInfo[buildId].buildTime;
    }
    if (type == SPECIAL) {
        return sSpecialInfo[buildId].buildTime;
    }
    // assumes units by default
    return sUnitInfo[buildId].build_time;
}

bool cBuildingListItem::isDoneBuilding() {
    return getProgress() >= getBuildTime();
}

bool cBuildingListItem::isTypeUpgrade() {
    return isType(eBuildType::UPGRADE);
}

bool cBuildingListItem::isTypeStructure() {
    return isType(eBuildType::STRUCTURE);
}

bool cBuildingListItem::isTypeUnit() {
    return isType(eBuildType::UNIT);
}

bool cBuildingListItem::isTypeSpecial() {
    return isType(eBuildType::SPECIAL);
}

bool cBuildingListItem::isType(eBuildType value) {
    return getBuildType() == value;
}

s_UpgradeInfo& cBuildingListItem::getUpgradeInfo() {
    int buildId = getBuildId();
    if (getBuildType() != eBuildType::UPGRADE){
        logbook("ERROR!!! - calling getUpgradeInfo while type is not UPGRADE! - falling back to buildId 1 as safety");
        buildId = 1;
    }
    return sUpgradeInfo[buildId];
}

s_SpecialInfo& cBuildingListItem::getSpecialInfo() {
    int buildId = getBuildId();
    if (getBuildType() != eBuildType::SPECIAL){
        logbook("ERROR!!! - calling gets_Special while type is not SPECIAL! - falling back to buildId 1 as safety");
        buildId = 1;
    }
    return sSpecialInfo[buildId];
}

s_UnitInfo& cBuildingListItem::getUnitInfo() {
    int buildId = getBuildId();
    if (getBuildType() != eBuildType::UNIT){
        logbook("ERROR!!! - calling getUnitInfo while type is not UNIT! - falling back to buildId 1 as safety");
        buildId = 1;
    }
    return sUnitInfo[buildId];
}

s_StructureInfo& cBuildingListItem::getStructureInfo() {
    int buildId = getBuildId();
    if (getBuildType() != eBuildType::STRUCTURE){
        logbook("ERROR!!! - calling getStructureInfo while type is not STRUCTURE! - falling back to buildId 1 as safety");
        buildId = 1;
    }
    return sStructureInfo[buildId];
}

void cBuildingListItem::resetTimesOrdered() {
    timesOrdered = 0;
}

eListType cBuildingListItem::getListType() {
    if (myList) {
        return myList->getType();
    }
    return eListType::LIST_NONE;
}

int cBuildingListItem::calculateBuildProgressFrameBasedOnBuildProgress() {
    // frame to draw (for building in progress)
    int iFrame = health_bar(31, progress, getTotalBuildTime());

    if (iFrame > 31) {
        iFrame = 31;
    }

    return iFrame;
}

void cBuildingListItem::decreaseProgressFrameTimer() {
    TIMER_progressFrame--;
}

float cBuildingListItem::getProgressFrameTimer() {
    return TIMER_progressFrame;
}

void cBuildingListItem::resetProgressFrameTimer() {
    // total time to build is progress * 35 (time unit). (in ticks)
    // divide that by frames (31), and get the time between frames!
    int buildTimeInTicks = getTotalBuildTimeInTicks();
    if (buildTimeInTicks > 0) {
        TIMER_progressFrame = buildTimeInTicks / 31;
    } else {
        TIMER_progressFrame = 0;
    }
}

int cBuildingListItem::getTotalBuildTimeInTicks() const {
    return getInTicks(totalBuildTime);
}

int cBuildingListItem::getInTicks(int getTimeInTicks) const {
    return timerCap * getTimeInTicks;
}

void cBuildingListItem::setTimerCap(int value) {
    timerCap = value;
}

/**
 * Assumes the itembuilder think is called every 5 ms. Meaning for every 1 second (1000ms), the think function
 * is called 200 times. Every tick is 5 ms, times 5 gives the total amount of ms.
 * @return
 */
int cBuildingListItem::getTotalBuildTimeInMs() {
    return getTotalBuildTimeInTicks() * 5; // 5 = ms for every time we call the itemBuilder
}

/**
 * Assumes the itembuilder think is called every 5 ms. Meaning for every 1 second (1000ms), the think function
 * is called 200 times. Every tick is 5 ms, times 5 gives the total amount of ms.
 * @return
 */
int cBuildingListItem::getProgressBuildTimeInMs() {
    return getInTicks(progress) * 5; // 5 = ms for every time we call the itemBuilder
}

const int cBuildingListItem::getTotalBuildTimeInTicks(eBuildType type, int buildId) {
    int buildTime = 0;
    switch (type) {
        case UNIT:
            buildTime = sUnitInfo[buildId].build_time;
            break;
        case STRUCTURE:
            buildTime = sStructureInfo[buildId].build_time;
            break;
        case SPECIAL:
            buildTime = sSpecialInfo[buildId].buildTime;
            break;
        case UPGRADE:
            buildTime = sUpgradeInfo[buildId].buildTime;
            break;
        case BULLET:
            buildTime = 0;
        default:
            buildTime = 0;
    }
    return buildTime * 35;
}

const int cBuildingListItem::getListId(eBuildType type, int buildId) {
    switch (type) {
        case UNIT:
            return sUnitInfo[buildId].listId;
        case STRUCTURE:
            return sStructureInfo[buildId].list;
        case SPECIAL:
            return sSpecialInfo[buildId].listId;
            break;
        case UPGRADE:
//            return sUpgradeInfo[buildId].;
            return LIST_UPGRADES;
            break;
    }
    return -1;
}

const bool cBuildingListItem::isAutoBuild(eBuildType type, int buildId) {
    switch (type) {
        case SPECIAL:
            return sSpecialInfo[buildId].autoBuild;
            break;
    }
    return false;
}

bool cBuildingListItem::isAutoBuild() {
    return cBuildingListItem::isAutoBuild(type, buildId);
}

std::string cBuildingListItem::getInfo() {
    char msg[255];
    int seconds = getTotalBuildTimeInMs() / 1000;
    if (isBuilding()) {
        int secondsInProgress = getProgressBuildTimeInMs() / 1000;
        seconds -= secondsInProgress;
    }

    if (isTypeStructure()) {
        s_StructureInfo structureType = getStructureInfo();
        sprintf(msg, "$%d | %s | %d Power | %d Secs", getBuildCost(), structureType.name, (structureType.power_give - structureType.power_drain), seconds);
    } else if (isTypeUnit()) {
        s_UnitInfo unitType = getUnitInfo();
        if (getBuildCost() > 0) {
            sprintf(msg, "$%d | %s | %d Secs", getBuildCost(), unitType.name, seconds);
        } else {
            sprintf(msg, "%s | %d Secs", sUnitInfo[getBuildId()].name, seconds);
        }
    } else if (isTypeUpgrade()){
        s_UpgradeInfo upgrade = getUpgradeInfo();
        sprintf(msg, "UPGRADE: $%d | %s | %d Secs", getBuildCost(), upgrade.description, seconds);
    } else if (isTypeSpecial()) {
        s_SpecialInfo special = getSpecialInfo();
        sprintf(msg, "$%d | %s | %d Secs", getBuildCost(), special.description, seconds);
    } else {
        sprintf(msg, "ERROR: UNKNOWN BUILD TYPE");
    }
    return std::string(msg);
}

bool cBuildingListItem::isFlashing() {
    return TIMER_flashing > 0;
}

void cBuildingListItem::decreaseFlashingTimer() {
    if (TIMER_flashing > 0) TIMER_flashing--;
}

std::string cBuildingListItem::getTypeString() {
    return std::string(eBuildTypeString(this->type));
}

std::string cBuildingListItem::getNameString() {
    if (isTypeStructure()) {
        s_StructureInfo info = getStructureInfo();
        return std::string(info.name);
    } else if (isTypeUnit()) {
        s_UnitInfo &info = getUnitInfo();
        return std::string(info.name);
    } else if (isTypeUpgrade()){
        s_UpgradeInfo upgrade = getUpgradeInfo();
        return std::string(upgrade.description);
    } else if (isTypeSpecial()) {
        s_SpecialInfo special = getSpecialInfo();
        return std::string(special.description);
    }

    return std::string("Unknown type.");
}
