#include "cBuildingListItem.h"

#include "utils/cLog.h"
#include "game/cGame.h"
#include "include/d2tmc.h"
#include "sidebar/cBuildingList.h"
#include "context/cInfoContext.h"
#include "context/cGameObjectContext.h"
#include "game/cGameSettings.h"
#include <format>
#include <cassert>

cBuildingListItem::cBuildingListItem(eBuildType type, int buildId, int cost, int icon, cBuildingList *list, int subList, bool queuable)
{
    assert(buildId >= 0);
    m_icon = icon;
    m_buildId = buildId;
    m_type = type;
    m_cost = cost;
    m_building = false;
    m_state = AVAILABLE;
    m_progress = 0;
    m_buildFrameToDraw = 0;
    m_timesToBuild = 0;
    m_timesOrdered  = 0;
    m_slotId = -1; // is set later

    m_creditsPerProgressTime = 0;
    m_placeIt = false;
    m_deployIt = false;
    m_queuable = queuable;

//    this->totalBuildTime = totalBuildTime;
    m_subList = subList;

    m_TIMER_progressFrame = 0.0f;
    m_TIMER_flashing = 500;

    m_timerCap = game.m_gameSettings->isCheatMode() ? cBuildingListItem::DebugTimerCap : cBuildingListItem::DefaultTimerCap;

    m_myList = list; // this can be nullptr! (it will be set from the outside by cBuildingList convenience methods)

    int totalBuildTime = getTotalBuildTime();
    if (m_cost > 0 && totalBuildTime > 0) {
        m_creditsPerProgressTime = (float)m_cost / (float)totalBuildTime;
    }

    cLogger::getInstance()->log(LOG_DEBUG, COMP_STRUCTURES, "cBuildingListItem",
        std::format("constructor [{}], cost = {}, totalBuildTime = {}, creditsPerProgressTime = {}",
            getNameString().c_str(), m_cost, totalBuildTime, m_creditsPerProgressTime));
}

cBuildingListItem::~cBuildingListItem()
{
    m_myList = NULL;
}

/**
 * Constructor for Structures
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_StructureInfo entry, cBuildingList *list, int subList) :
    cBuildingListItem(STRUCTURE, theID, entry.cost, entry.icon, list, subList, entry.queuable)
{
}

/**
 * Constructor for Specials
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_SpecialInfo entry, cBuildingList *list, int subList) :
    cBuildingListItem(SPECIAL, theID, 0, entry.icon, list, subList, false)
{
}

/**
 * Constructor for Upgrades
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_UpgradeInfo entry, cBuildingList *list, int subList) :
    cBuildingListItem(UPGRADE, theID, entry.cost, entry.icon, list, subList, false)
{
}

cBuildingListItem::cBuildingListItem(int theID, s_StructureInfo entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList)
{
}

cBuildingListItem::cBuildingListItem(int theID, s_SpecialInfo entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList)
{
}

/**
 * Constructor for units
 * @param theID
 * @param entry
 * @param list
 * @param subList
 */
cBuildingListItem::cBuildingListItem(int theID, s_UnitInfo entry, cBuildingList *list, int subList) :
    cBuildingListItem(UNIT, theID, entry.cost, entry.icon, list, subList, entry.queuable)
{
}

cBuildingListItem::cBuildingListItem(int theID, s_UnitInfo entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList)
{
}

cBuildingListItem::cBuildingListItem(int theID, s_UpgradeInfo entry, int subList) : cBuildingListItem(theID, entry, nullptr, subList)
{
}

/**
 * Return the amount of money the player gets back
 * @return
 */
float cBuildingListItem::getRefundAmount()
{
    float fProgress = m_progress;
    if (fProgress < 1.0F) {
        return 0.0F;
    }
    return (fProgress * m_creditsPerProgressTime);
}

void cBuildingListItem::decreaseTimesToBuild()
{
    m_timesToBuild--;
}

void cBuildingListItem::increaseProgress(int byAmount)
{
    setProgress(getProgress() + byAmount);
}

/**
 * Returns the (total) build time of this item, which is the same as retrieving the buildTime property from the
 * appropriate *info struct.
 * @return
 */
int cBuildingListItem::getTotalBuildTime() const
{
    if (m_type == STRUCTURE) {
        return game.m_infoContext->getStructureInfo(m_buildId).buildTime;
    }
    if (m_type == UPGRADE) {
        return game.m_infoContext->getUpgradeInfo(m_buildId).buildTime;
    }
    if (m_type == SPECIAL) {
        return game.m_infoContext->getSpecialInfo(m_buildId).buildTime;
    }
    // assumes units by default
    return game.m_infoContext->getUnitInfo(m_buildId).buildTime;
}

bool cBuildingListItem::isDoneBuilding()
{
    return getProgress() >= getTotalBuildTime();
}

bool cBuildingListItem::isTypeUpgrade()
{
    return isType(eBuildType::UPGRADE);
}

bool cBuildingListItem::isTypeStructure()
{
    return isType(eBuildType::STRUCTURE);
}

bool cBuildingListItem::isTypeUnit()
{
    return isType(eBuildType::UNIT);
}

bool cBuildingListItem::isTypeSpecial()
{
    return isType(eBuildType::SPECIAL);
}

bool cBuildingListItem::isType(eBuildType value)
{
    return getBuildType() == value;
}

s_UpgradeInfo &cBuildingListItem::getUpgradeInfo()
{
    int buildId = getBuildId();
    if (getBuildType() != eBuildType::UPGRADE) {
        logbook("ERROR!!! - calling getUpgradeInfo while type is not UPGRADE! - falling back to buildId 1 as safety");
        buildId = 1;
    }
    return game.m_infoContext->getUpgradeInfo(buildId);
}

s_SpecialInfo &cBuildingListItem::getSpecialInfo()
{
    int buildId = getBuildId();
    if (getBuildType() != eBuildType::SPECIAL) {
        logbook("ERROR!!! - calling gets_Special while type is not SPECIAL! - falling back to buildId 1 as safety");
        buildId = 1;
    }
    return game.m_infoContext->getSpecialInfo(buildId);
}

s_UnitInfo &cBuildingListItem::getUnitInfo()
{
    int buildId = getBuildId();
    if (getBuildType() != eBuildType::UNIT) {
        logbook("ERROR!!! - calling getUnitInfo while type is not UNIT! - falling back to buildId 1 as safety");
        buildId = 1;
    }
    return game.m_infoContext->getUnitInfo(buildId);
}

s_StructureInfo &cBuildingListItem::getStructureInfo()
{
    int buildId = getBuildId();
    if (getBuildType() != eBuildType::STRUCTURE) {
        logbook("ERROR!!! - calling getStructureInfo while type is not STRUCTURE! - falling back to buildId 1 as safety");
        buildId = 1;
    }
    return game.m_infoContext->getStructureInfo(buildId);
}

void cBuildingListItem::resetTimesOrdered()
{
    m_timesOrdered = 0;
}

eListType cBuildingListItem::getListType()
{
    if (m_myList) {
        return m_myList->getType();
    }
    return eListType::LIST_NONE;
}

int cBuildingListItem::calculateBuildProgressFrameBasedOnBuildProgress()
{
    // frame to draw (for building in progress)
//    int iFrame = health_bar(31, m_progress, getTotalBuildTime());
    int iFrame = healthBar(31, m_progress, getTotalBuildTime());

    if (iFrame > 31) {
        iFrame = 31;
    }

    return iFrame;
}

void cBuildingListItem::decreaseProgressFrameTimer()
{
    m_TIMER_progressFrame--;
}

float cBuildingListItem::getProgressFrameTimer()
{
    return m_TIMER_progressFrame;
}

void cBuildingListItem::resetProgressFrameTimer()
{
    // total time to build is progress * 35 (time unit). (in ticks)
    // divide that by frames (31), and get the time between frames!
    int buildTimeInTicks = getTotalBuildTimeInTicks();
    if (buildTimeInTicks > 0) {
        m_TIMER_progressFrame = buildTimeInTicks / 31;
    }
    else {
        m_TIMER_progressFrame = 0;
    }
}

int cBuildingListItem::getTotalBuildTimeInTicks() const
{
    return getInTicks(getTotalBuildTime());
}

int cBuildingListItem::getInTicks(int getTimeInTicks) const
{
    return m_timerCap * getTimeInTicks;
}

void cBuildingListItem::setTimerCap(int value)
{
    m_timerCap = value;
}

/**
 * Assumes the itembuilder think is called every 5 ms. Meaning for every 1 second (1000ms), the think function
 * is called 200 times. Every tick is 5 ms, times 5 gives the total amount of ms.
 * @return
 */
int cBuildingListItem::getTotalBuildTimeInMs()
{
    return getTotalBuildTimeInTicks() * 5; // 5 = ms for every time we call the itemBuilder
}

/**
 * Assumes the itembuilder think is called every 5 ms. Meaning for every 1 second (1000ms), the think function
 * is called 200 times. Every tick is 5 ms, times 5 gives the total amount of ms.
 * @return
 */
int cBuildingListItem::getProgressBuildTimeInMs()
{
    return getInTicks(m_progress) * 5; // 5 = ms for every time we call the itemBuilder
}

int cBuildingListItem::getTotalBuildTimeInTicks(eBuildType type, int buildId)
{
    int buildTime = 0;
    switch (type) {
        case UNIT:
            buildTime = game.m_infoContext->getUnitInfo(buildId).buildTime;
            break;
        case STRUCTURE:
            buildTime = game.m_infoContext->getStructureInfo(buildId).buildTime;
            break;
        case SPECIAL:
            buildTime = game.m_infoContext->getSpecialInfo(buildId).buildTime;
            break;
        case UPGRADE:
            buildTime = game.m_infoContext->getUpgradeInfo(buildId).buildTime;
            break;
        case BULLET:
        default:
            buildTime = 0;
            break;
    }
    return buildTime * 35;
}

int cBuildingListItem::getListId(eBuildType type, int buildId)
{
    switch (type) {
        case UNIT:
            return eListTypeAsInt(game.m_infoContext->getUnitInfo(buildId).listType);
        case STRUCTURE:
            return game.m_infoContext->getStructureInfo(buildId).list;
        case SPECIAL:
            return eListTypeAsInt(game.m_infoContext->getSpecialInfo(buildId).listType);
            break;
        case UPGRADE:
//            return game.m_infoContext->getUpgradeInfo(buildId).;
            return eListTypeAsInt(eListType::LIST_UPGRADES);
            break;
        default:
            return -1;
    }
}

bool cBuildingListItem::isAutoBuild(eBuildType type, int buildId)
{
    switch (type) {
        case SPECIAL:
            return game.m_infoContext->getSpecialInfo(buildId).autoBuild;
        default:
            return false;
    }
}

bool cBuildingListItem::isAutoBuild()
{
    return cBuildingListItem::isAutoBuild(m_type, m_buildId);
}

std::string cBuildingListItem::getInfo()
{
    std::string msg;
    int seconds = getTotalBuildTimeInMs() / 1000;
    if (isBuilding()) {
        int secondsInProgress = getProgressBuildTimeInMs() / 1000;
        seconds -= secondsInProgress;
    }

    if (isTypeStructure()) {
        s_StructureInfo structureType = getStructureInfo();
        msg = std::format("${} | {} | {} Power | {} Secs", getBuildCost(), structureType.name, (structureType.power_give - structureType.power_drain), seconds);
    }
    else if (isTypeUnit()) {
        s_UnitInfo unitType = getUnitInfo();
        if (getBuildCost() > 0) {
            msg = std::format("${} | {} | {} Secs", getBuildCost(), unitType.name, seconds);
        }
        else {
            msg = std::format("{} | {} Secs", game.m_infoContext->getUnitInfo(getBuildId()).name, seconds);
        }
    }
    else if (isTypeUpgrade()) {
        s_UpgradeInfo upgrade = getUpgradeInfo();
        msg = std::format("UPGRADE: ${} | {} | {} Secs", getBuildCost(), upgrade.description, seconds);
    }
    else if (isTypeSpecial()) {
        s_SpecialInfo special = getSpecialInfo();
        msg = std::format("${} | {} | {} Secs", getBuildCost(), special.description, seconds);
    }
    else {
        msg = "ERROR: UNKNOWN BUILD TYPE";
    }
    return msg;
}

bool cBuildingListItem::isFlashing()
{
    return m_TIMER_flashing > 0;
}

void cBuildingListItem::decreaseFlashingTimer()
{
    if (m_TIMER_flashing > 0) m_TIMER_flashing--;
}

std::string cBuildingListItem::getTypeString()
{
    return std::string(eBuildTypeString(m_type));
}

std::string cBuildingListItem::getNameString()
{
    if (isTypeStructure()) {
        s_StructureInfo info = getStructureInfo();
        return std::string(info.name);
    }
    else if (isTypeUnit()) {
        s_UnitInfo &info = getUnitInfo();
        return std::string(info.name);
    }
    else if (isTypeUpgrade()) {
        s_UpgradeInfo upgrade = getUpgradeInfo();
        return std::string(upgrade.description);
    }
    else if (isTypeSpecial()) {
        s_SpecialInfo special = getSpecialInfo();
        return std::string(special.description);
    }

    return std::string("Unknown type.");
}
