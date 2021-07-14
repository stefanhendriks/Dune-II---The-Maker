#include <vector>
#include <algorithm>
#include "../include/d2tmh.h"
#include "cPlayer.h"


cPlayer::cPlayer() {
    itemBuilder = NULL;
    orderProcesser = NULL;
    sidebar = NULL;
    buildingListUpdater = NULL;
    gameControlsContext = NULL;
    char msg[255];
    sprintf(msg, "MAX_STRUCTURETYPES=[%d], sizeof bmp_structure=%d, sizeof(BITMAP *)", MAX_STRUCTURETYPES,
            sizeof(bmp_structure), sizeof(BITMAP *));
    logbook(msg);
    memset(bmp_structure, 0, sizeof(bmp_structure));
    memset(bmp_unit, 0, sizeof(bmp_unit));
    memset(bmp_unit_top, 0, sizeof(bmp_unit_top));
    brain_ = nullptr;
    autoSlabStructures = false;
}

cPlayer::~cPlayer() {
    if (itemBuilder) {
        delete itemBuilder;
    }
    if (orderProcesser) {
        delete orderProcesser;
    }
    if (sidebar) {
        delete sidebar;
    }
    if (buildingListUpdater) {
        delete buildingListUpdater;
    }
    if (gameControlsContext) {
        delete gameControlsContext;
    }
    if (difficultySettings) {
        delete difficultySettings;
    }
    if (brain_) {
        delete brain_;
    }
    // cannot do this in destructor, as Allegro is already shutdown
//    destroyAllegroBitmaps();
}

void cPlayer::destroyAllegroBitmaps() {
    clearStructureTypeBitmaps();
    clearUnitTypeBitmaps();
}

void cPlayer::clearStructureTypeBitmaps() {
    for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
        if (bmp_structure[i]) {
//            char msg[255];
//            sprintf(msg, "clearStructureTypeBitmaps: Destroying bmp_structure for index [%d].", i);
//            logbook(msg);
            destroy_bitmap(bmp_structure[i]);
        } else {
//            char msg[255];
//            sprintf(msg, "clearStructureTypeBitmaps: Index [%d] is null.", i);
//            logbook(msg);
        }
        bmp_structure[i] = nullptr;
    }
}

void cPlayer::setFocusCell(int cll) {
    this->focusCell_ = cll;
}

void cPlayer::clearUnitTypeBitmaps() {
    for (int i = 0; i < MAX_UNITTYPES; i++) {
        if (bmp_unit[i]) {
            if (DEBUGGING) {
                char msg[255];
                sprintf(msg, "clearUnitTypeBitmaps: Destroying bmp_unit for index [%d].", i);
                logbook(msg);
            }
            destroy_bitmap(bmp_unit[i]);
        }
        bmp_unit[i] = nullptr;

        if (bmp_unit_top[i]) {
            if (DEBUGGING) {
                char msg[255];
                sprintf(msg, "clearUnitTypeBitmaps: Destroying bmp_unit_top for index [%d].", i);
                logbook(msg);
            }
            destroy_bitmap(bmp_unit_top[i]);
        }
        bmp_unit_top[i] = nullptr;
    }
}

void cPlayer::setSideBar(cSideBar *theSideBar) {
    assert(theSideBar);

    // delete old reference
    if (sidebar) {
        delete sidebar;
    }

    sidebar = theSideBar;
}

void cPlayer::setItemBuilder(cItemBuilder *theItemBuilder) {
    assert(theItemBuilder);

    // delete old reference
    if (itemBuilder) {
        delete itemBuilder;
    }

    itemBuilder = theItemBuilder;
}

void cPlayer::setOrderProcesser(cOrderProcesser *theOrderProcesser) {
    assert(theOrderProcesser);

    if (orderProcesser) {
        delete orderProcesser;
    }

    orderProcesser = theOrderProcesser;
}

void cPlayer::setBuildingListUpdater(cBuildingListUpdater *theBuildingListUpgrader) {
    assert(theBuildingListUpgrader);

    // delete old reference
    if (buildingListUpdater) {
        delete buildingListUpdater;
    }

    buildingListUpdater = theBuildingListUpgrader;
}

void cPlayer::setGameControlsContext(cGameControlsContext *theGameControlsContext) {
    assert(theGameControlsContext);

    // delete old reference
    if (gameControlsContext) {
        delete gameControlsContext;
    }

    gameControlsContext = theGameControlsContext;
}

void cPlayer::init(int id, brains::cPlayerBrain *brain) {
    if (id < 0 || id >= MAX_PLAYERS) {
        char msg[255];
        sprintf(msg, "Error initializing player, id %d is not valid.", id);
        logbook(msg);
    }
    assert(id >= HUMAN);
    assert(id < MAX_PLAYERS);
    this->id = id;

    setBrain(brain);

    memcpy(pal, general_palette, sizeof(pal));
    house = GENERALHOUSE;

    /**
     * Ok, so this is confusing.
     * There are also aiPlayer classes. They hold some 'brains' I guess. all other state is stored here.
     * So even though there are aiPlayer objects, they are complimentary to this class
     */
    m_Human = (id == HUMAN);

    if (difficultySettings) delete difficultySettings;
    difficultySettings = new cPlayerDifficultySettings();

    // Reset structures amount
    for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
        iStructures[i] = 0;
        iPrimaryBuilding[i] = -1;
        iStructureUpgradeLevel[i] = 0;
    }

    credits = 0;
    maxCredits_ = 1000;
    focusCell_ = 0;

    powerUsage_ = 0;
    powerProduce_ = 0;

    iTeam = -1;
}

/**
 * Sets house value , but also sets up difficulty settings + creates bitmaps for that corresponding house
 * @param iHouse
 */
void cPlayer::setHouse(int iHouse) {
    int currentHouse = house;
    char msg[255];
    sprintf(msg, "cPlayer[%d]::setHouse - Current house is [%d/%s], setting house to [%d/%s]", this->id, currentHouse,
            this->getHouseNameForId(currentHouse).c_str(), iHouse, this->getHouseNameForId(iHouse).c_str());
    logbook(msg);
    house = iHouse;      // use rules of this house

    if (difficultySettings) {
        delete difficultySettings;
    }

    difficultySettings = cPlayerDifficultySettings::createFromHouse(iHouse);

    if (currentHouse != iHouse) {
        // copy entire palette
        memcpy(pal, general_palette, sizeof(pal));

        char msg[255];
        sprintf(msg, "cPlayer[%d]::setHouse - Current house differs from iHouse, preparing palette.", this->id);
        logbook(msg);

        // now set the different colors based upon house
        if (houses[house].swap_color > -1) {
            int start = houses[house].swap_color;
            int s = 144;                // original position (harkonnen)
            char msg[255];
            sprintf(msg, "cPlayer[%d]::setHouse - Swap_color index is %d.", this->id, start);
            logbook(msg);
            for (int j = start; j < (start + 7); j++) {
                // swap everything from S with J
                pal[s] = pal[j];
                s++;
            }
        }

        minimapColor = houses[house].minimap_color;
        emblemBackgroundColor = getEmblemBackgroundColorForHouse(house);

        destroyAllegroBitmaps();

        int colorDepthBmpScreen = bitmap_color_depth(bmp_screen);

        // use this palette to draw stuff
        select_palette(pal);

        // now copy / set all structures for this player, with the correct color
        for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
            s_Structures &structureType = structures[i];

            if (!structureType.configured) continue;

            bmp_structure[i] = create_bitmap_ex(colorDepthBmpScreen, structureType.bmp->w, structureType.bmp->h);
            if (!bmp_structure[i]) {
                allegro_message("Could not create bmp structure bitmap!? - Imminent crash.");
            }
            clear_to_color(bmp_structure[i], makecol(255, 0, 255));

            draw_sprite(bmp_structure[i], structureType.bmp, 0, 0);
        }

        // same goes for units
        for (int i = 0; i < MAX_UNITTYPES; i++) {
            s_UnitP &unitType = units[i];

            bmp_unit[i] = create_bitmap_ex(colorDepthBmpScreen, unitType.bmp->w, unitType.bmp->h);
            if (!bmp_unit[i]) {
                allegro_message("Could not create bmp unit bitmap!? - Imminent crash.");
            }
            clear_to_color(bmp_unit[i], makecol(255, 0, 255));

            draw_sprite(bmp_unit[i], unitType.bmp, 0, 0);

            if (unitType.top) {
                bmp_unit_top[i] = create_bitmap_ex(colorDepthBmpScreen, unitType.bmp->w, unitType.bmp->h);
                clear_to_color(bmp_unit_top[i], makecol(255, 0, 255));

                draw_sprite(bmp_unit_top[i], unitType.top, 0, 0);
            }
        }
    }
}

int cPlayer::getEmblemBackgroundColorForHouse(int houseId) {
    switch (houseId) {
        case ATREIDES:
            return makecol(8, 12, 89);
        case HARKONNEN:
            return makecol(60, 0, 0);
        case ORDOS:
            return makecol(0, 32, 0);
        case SARDAUKAR:
            return makecol(128, 0, 128);
        default:
            return makecol(100, 255, 100);
    }
}

bool cPlayer::bEnoughSpiceCapacityToStoreCredits() const {
    return maxCredits_ > credits;
}

bool cPlayer::bEnoughPower() const {
    if (!game.bSkirmish) {
        // AI cheats on power
        if (!m_Human) {
            // Dune 2 non-skirmish AI cheats; else it will be unplayable in some missions.
            // the Dune 2 AI basically has always enough power, to make it somewhat more 'fair' lets check for
            // ConstYard + Windtrap. If both destroyed we are out of power.
            return hasAtleastOneStructure(WINDTRAP) || hasAtleastOneStructure(CONSTYARD);
        }
    }

    return powerProduce_ >= powerUsage_;
}

bool cPlayer::hasRadarAndEnoughPower() const {
    return getAmountOfStructuresForType(RADAR) > 0 && bEnoughPower();
}

/**
 * This function returns the amount for the given structure type. If structureType param is invalid, then it will
 * return -1
 * @param structureType
 * @return
 */
int cPlayer::getAmountOfStructuresForType(int structureType) const {
    if (structureType < 0 || structureType > RTURRET) return -1;
    return iStructures[structureType];
}

/**
 * This function will return the amount of units for given type, but it is not (yet) optimized, so it will
 * loop over all units and count them. Use it with caution.
 * return -1
 * @param unitType
 * @return
 */
int cPlayer::getAmountOfUnitsForType(int unitType) const {
    if (unitType < 0 || unitType > MAX_UNITTYPES) return -1;
    int count = 0;
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != this->getId()) continue;
        if (cUnit.iType == unitType) {
            count++;
        }
    }
    return count;
}

/**
 * This function will return the amount of units for given type, but it is not (yet) optimized, so it will
 * loop over all units and count them. Use it with caution.
 * return -1
 * @param unitTypes (vector of all unitTypes to check)
 * @return
 */
int cPlayer::getAmountOfUnitsForType(std::vector<int> unitTypes) const {
    int count = 0;
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != this->getId()) continue;
        if (std::find(unitTypes.begin(), unitTypes.end(), cUnit.iType) != unitTypes.end()) {
            count++;
        }
    }
    return count;
}


/**
 * Returns the bitmap for structure type "index", this structure has been colorized beforehand for this player and is
 * in same color depth as bmp_screen.
 * @param index
 * @return
 */
BITMAP *cPlayer::getStructureBitmap(int index) {
    if (bmp_structure[index]) {
        return bmp_structure[index];
    }
    return nullptr;
}

/**
 * Returns the bitmap for unit type "index", this unit has been colorized beforehand for this player and is
 * in same color depth as bmp_screen.
 * @param index
 * @return
 */
BITMAP *cPlayer::getUnitBitmap(int index) {
    if (bmp_unit[index]) {
        return bmp_unit[index];
    }
    return nullptr;
}

/**
 * Returns the bitmap for unit top type "index", this top of unit has been colorized beforehand for this player and is
 * in same color depth as bmp_screen.
 * @param index
 * @return
 */
BITMAP *cPlayer::getUnitTopBitmap(int index) {
    if (bmp_unit_top[index]) {
        return bmp_unit_top[index];
    }
    return nullptr;
}

/**
 * Returns the shadow bitmap of unit type "index", using bodyFacing and animationFrame.
 * !!! Be sure to destroy the bitmap returned from here !!!
 * @param index
 * @return
 */
BITMAP *cPlayer::getUnitShadowBitmap(int index, int bodyFacing, int animationFrame) {
    if (units[index].shadow) {
        int bmp_width = units[index].bmp_width;
        int bmp_height = units[index].bmp_height;
        int start_x = bodyFacing * bmp_width;
        int start_y = bmp_height * animationFrame;

        // Carry-all has a bit different offset for shadow
        if (index == CARRYALL) {
            start_x += 2;
            start_y += 2;
        }

        int colorDepth = bitmap_color_depth(bmp_screen);
        BITMAP *shadow = create_bitmap_ex(colorDepth, bmp_width, bmp_height);
        clear_to_color(shadow, makecol(255, 0, 255));

        blit((BITMAP *) units[index].shadow, shadow, start_x, start_y, 0, 0, bmp_width, bmp_height);
        return shadow;
    }
    return nullptr;
}

bool cPlayer::hasWor() const {
    return iStructures[WOR] > 0;
}

bool cPlayer::hasBarracks() const {
    return iStructures[BARRACKS] > 0;
}

bool cPlayer::hasAtleastOneStructure(int structureType) const {
    if (structureType < 0) return false;
    if (structureType >= MAX_STRUCTURETYPES) return false;
    return iStructures[structureType] > 0;
}

bool cPlayer::hasEnoughCreditsFor(float requestedAmount) const {
    return credits > requestedAmount;
}

bool cPlayer::hasEnoughCreditsForUnit(int unitType) {
    if (unitType < 0 || unitType >= MAX_UNITTYPES) return false;
    return this->credits >= units[unitType].cost;
}

bool cPlayer::hasEnoughCreditsForStructure(int structureType) {
    if (structureType < 0 || structureType >= MAX_STRUCTURETYPES) return false;
    return this->credits >= structures[structureType].cost;
}

bool cPlayer::hasEnoughCreditsForUpgrade(int upgradeType) {
    if (upgradeType < 0 || upgradeType >= MAX_UPGRADETYPES) return false;
    return this->credits >= upgrades[upgradeType].cost;
}

/**
 * Returns house based fading/pulsating color
 * @return
 */
int cPlayer::getHouseFadingColor() const {
    int fadeSelect = game.getFadeSelect();
    if (house == ATREIDES) {
        return makecol(0, 0, fadeSelect);
    }
    if (house == HARKONNEN) {
        return makecol(fadeSelect, 0, 0);
    }
    if (house == ORDOS) {
        return makecol(0, fadeSelect, 0);
    }

    // TODO other houses (Sardaukar, etc)
    return makecol(fadeSelect, fadeSelect, fadeSelect);
}

/**
 * Returns the error fading color (red to black pulsating)
 * @return
 */
int cPlayer::getErrorFadingColor() const {
    int fadeSelect = game.getFadeSelect();
    return makecol(fadeSelect, 0, 0); // red fading
}

/**
 * Returns the fading white color
 * @return
 */
int cPlayer::getSelectFadingColor() const {
    int fadeSelect = game.getFadeSelect();
    return makecol(fadeSelect, fadeSelect, fadeSelect); // white fading
}

eHouseBitFlag cPlayer::getHouseBitFlag() {
    switch (house) {
        case ATREIDES:
            return eHouseBitFlag::Atreides;
        case HARKONNEN:
            return eHouseBitFlag::Harkonnen;
        case ORDOS:
            return eHouseBitFlag::Ordos;
        case SARDAUKAR:
            return eHouseBitFlag::Sardaukar;
        case FREMEN:
            return eHouseBitFlag::Fremen;
        default:
            return eHouseBitFlag::Unknown;
    }
}

void cPlayer::increaseStructureAmount(int structureType) {
    if (structureType < 0) return;
    if (structureType >= MAX_STRUCTURETYPES) return;
    iStructures[structureType]++;

    char msg[255];
    sprintf(msg, "Player[%d] - increaseStructureAmount result: iStructures[%d(=%s)]=%d", id, structureType,
            structures[structureType].name, iStructures[structureType]);
    logbook(msg);
}

void cPlayer::decreaseStructureAmount(int structureType) {
    if (structureType < 0) return;
    if (structureType >= MAX_STRUCTURETYPES) return;

    iStructures[structureType]--;

    char msg[255];
    sprintf(msg, "Player[%d] - decreaseStructureAmount result: iStructures[%d(=%s)]=%d", id, structureType,
            structures[structureType].name, iStructures[structureType]);
    logbook(msg);
}

std::string cPlayer::getHouseName() {
    return getHouseNameForId(house);
}

std::string cPlayer::getHouseNameForId(int house) {
    if (house == ATREIDES) {
        return "Atreides";
    } else if (house == HARKONNEN) {
        return "Harkonnen";
    } else if (house == ORDOS) {
        return "Ordos";
    } else if (house == SARDAUKAR) {
        return "Sardaukar";
    } else if (house == FREMEN) {
        return "Fremen";
    } else if (house == MERCENARY) {
        return "Mercenary";
    } else if (house == GENERALHOUSE) {
        return "Generalhouse (none)";
    } else if (house == CORRINO) {
        return "Corrino / House for worms";
    }
    return "Unknown !?";
}

void cPlayer::giveCredits(float amountToGive) {
    credits += amountToGive;
}

// TODO: This could be done smarter once we receive notifications when a unit gets created/destroyed!
std::vector<int> cPlayer::getAllMyUnits() {
    std::vector<int> ids = std::vector<int>();
    for (int i = 0; i < MAX_UNITTYPES; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.isDead()) continue;
        if (cUnit.iPlayer != getId()) continue;
        ids.push_back(i);
    }
    return ids;
}

// TODO: This can be done smarter because we receive notifications when a structure gets created/destroyed!
std::vector<int> cPlayer::getAllMyStructuresAsId() {
    std::vector<int> ids = std::vector<int>();
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *abstractStructure = structure[i];
        if (!abstractStructure) continue;
        if (!abstractStructure->isValid()) continue;
        if (!abstractStructure->belongsTo(this)) continue;
        ids.push_back(i);
    }
    return ids;
}

bool cPlayer::isSameTeamAs(cPlayer *pPlayer) {
    if (pPlayer == nullptr) return false;
    return pPlayer->iTeam == iTeam;
}

/**
 * Update player state
 */
void cPlayer::update() {
    powerUsage_ = structureUtils.getTotalPowerUsageForPlayer(this);
    powerProduce_ = structureUtils.getTotalPowerOutForPlayer(this);
    // update spice capacity
    maxCredits_ = structureUtils.getTotalSpiceCapacityForPlayer(this);
}

int cPlayer::getCredits() {
    return credits;
}

void cPlayer::setCredits(int credits) {
    this->credits = credits;
}

void cPlayer::substractCredits(int amount) {
    credits -= amount;
}

float cPlayer::getMaxCredits() {
    return maxCredits_;
}

int cPlayer::getPowerProduced() {
    return powerProduce_;
}

int cPlayer::getPowerUsage() {
    return powerUsage_;
}

/**
 * Gives the player credits, but caps at max spice/credits capacity.
 * @param amount
 */
void cPlayer::dumpCredits(int amount) {
    giveCredits(amount);
    if (credits > maxCredits_) {
        credits = maxCredits_;
    }
}

/**
 * Think function (called when AI should do something). Delegates to brain (if set). Called for every 100ms tick.
 */
void cPlayer::think() {
    if (brain_) {
        brain_->think();
    }
}

void cPlayer::setBrain(brains::cPlayerBrain *brain) {
    // delete old brain object if it was set before
    if (brain_) {
        delete brain_;
    }
    // set new brain
    brain_ = brain;
}

bool cPlayer::canBuildStructureType(int iStructureType) const {
    cBuildingListItem *pItem = sidebar->getBuildingListItem(LIST_CONSTYARD, iStructureType);
    return pItem != nullptr;
}

bool cPlayer::canBuildUnitType(int iUnitType) const {
    int listId = units[iUnitType].listId;
    cBuildingListItem *pItem = sidebar->getBuildingListItem(listId, iUnitType);
    char msg[255];
    bool result = pItem != nullptr;
    sprintf(msg, "cPlayer[%d] canBuildUnitType(unitType=%d) -> %s", getId(), iUnitType, result ? "TRUE" : "FALSE");
    logbook(msg);
    return result;
}

int cPlayer::getStructureTypeBeingBuilt() const {
    cBuildingListItem *pItem = getStructureBuildingListItemBeingBuilt();
    if (pItem) {
        return pItem->getBuildId();
    }
    return -1;
}

cBuildingListItem *cPlayer::getStructureBuildingListItemBeingBuilt() const {
    return itemBuilder->getListItemWhichIsBuilding(LIST_CONSTYARD, 0);
}

/**
 * Checks if anything is built in the list/sublist combination for this unitType. This could mean the unitType
 * itself is being built OR that another unit is built using the same list/sublist as this unitType. Example: A QUAD
 * and a TRIKE share the same list/sublist. If a QUAD is being built and you pass in the unitType TRIKE then this
 * function will return true.
 * @param iUnitType
 * @return
 */
bool cPlayer::isBuildingSomethingInSameListSubListAsUnitType(int iUnitType) const {
    int listId = units[iUnitType].listId;
    int subListId = units[iUnitType].subListId;

    return itemBuilder->isAnythingBeingBuiltForListId(listId, subListId);
}


/**
 * Checks if the given structureType is available for upgrading. If so, returns the corresponding cBuildingListItem
 * which can be used later (ie, to execute an upgrade, or check other info)
 *
 * @param iStructureType
 * @return
 */
cBuildingListItem *cPlayer::isUpgradeAvailableToGrantStructure(int iStructureType) const {
    return isUpgradeAvailableToGrant(STRUCTURE, iStructureType);
}


/**
 * Checks if the given unitType is available for upgrading. If so, returns the corresponding cBuildingListItem
 * which can be used later (ie, to execute an upgrade, or check other info)
 *
 * @param iUnitTYpe
 * @return
 */
cBuildingListItem *cPlayer::isUpgradeAvailableToGrantUnit(int iUnitType) const {
    return isUpgradeAvailableToGrant(UNIT, iUnitType);
}

/**
 * Checks if an upgrade exists for the 'providesType' (ie UNIT/STRUCTURE) and a specific kind of that type (providesTypeId), ie
 * a QUAD, or RTURRET, etc.
 *
 * @param providesType (UNIT/STRUCTURE)
 * @param providesTypeId (ex: QUAD, TANK, RTURRET, etc)
 * @return
 */
cBuildingListItem *cPlayer::isUpgradeAvailableToGrant(eBuildType providesType, int providesTypeId) const {
    cBuildingList *pList = sidebar->getList(LIST_UPGRADES);
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = pList->getItem(i);
        if (pItem == nullptr) continue;
        const s_Upgrade &theUpgrade = pItem->getS_Upgrade();
        if (theUpgrade.providesType != providesType) continue;
        if (theUpgrade.providesTypeId == providesTypeId) {
            return pItem;
        }
    }
    return nullptr;
}


/**
 * Checks if any Upgrade is in progress for the given listId/sublistId
 *
 * @param listId
 * @param sublistId
 * @return
 */
cBuildingListItem *cPlayer::isUpgradingList(int listId, int sublistId) const {
    cBuildingList *upgradesList = sidebar->getList(LIST_UPGRADES);
    if (upgradesList == nullptr) {
        char msg[255];
        sprintf(msg,
                "AI[%d] - isUpgradingList for listId [%d] and sublistId [%d], could not find upgradesList!? - will return FALSE.",
                getId(), listId, sublistId);
        logbook(msg);
        assert(false);
        return nullptr;
    }

    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = upgradesList->getItem(i);
        if (pItem == nullptr) continue;
        const s_Upgrade &theUpgrade = pItem->getS_Upgrade();
        // is this upgrade applicable to the listId/sublistId we're interested in?
        if (theUpgrade.providesTypeList == listId && theUpgrade.providesTypeSubList == sublistId) {
            if (pItem->isBuilding()) {
                // it is in progress, so yes!
                return pItem;
            }
        }
    }
    return nullptr;
}

cBuildingListItem *cPlayer::isUpgradingConstyard() const {
    return isUpgradingList(LIST_CONSTYARD, 0);
}

/**
 * Returns true if anything is built from ConstYard
 */
cBuildingListItem *cPlayer::isBuildingStructure() const {
    return itemBuilder->getListItemWhichIsBuilding(LIST_CONSTYARD, 0);
}

bool cPlayer::startBuildingUnit(int iUnitType) const {
    s_UnitP &unitType = units[iUnitType];
    int listId = unitType.listId;
    bool startedBuilding = sidebar->startBuildingItemIfOk(listId, iUnitType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d], with listId[%d] - SUCCESS", unitType.name,
                    iUnitType, listId);
        } else {
            sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d], with listId[%d] - FAILED", unitType.name,
                    iUnitType, listId);
        }
        logbook(msg);
    }
    return startedBuilding;
}

bool cPlayer::startBuildingStructure(int iStructureType) const {
    int listId = LIST_CONSTYARD;

    bool startedBuilding = sidebar->startBuildingItemIfOk(listId, iStructureType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build structure [%s] iStructureType = [%d], with listId[%d] - SUCCESS",
                    structures[iStructureType].name, iStructureType, listId);
        } else {
            sprintf(msg, "Wanting to build structure [%s] iStructureType = [%d], with listId[%d] - FAILED",
                    structures[iStructureType].name, iStructureType, listId);
        }
        logbook(msg);
    }
    return startedBuilding;
}

bool cPlayer::startBuildingSpecial(int iSpecialType) const {
    int listId = LIST_PALACE;

    bool startedBuilding = sidebar->startBuildingItemIfOk(listId, iSpecialType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build special [%s] iSpecialType = [%d], with listId[%d] - SUCCESS",
                    specials[iSpecialType].description, iSpecialType, listId);
        } else {
            sprintf(msg, "Wanting to build special [%s] iSpecialType = [%d], with listId[%d] - FAILED",
                    specials[iSpecialType].description, iSpecialType, listId);
        }
        logbook(msg);
    }
    return startedBuilding;
}

bool cPlayer::startUpgrading(int iUpgradeType) const {
    int listId = LIST_UPGRADES;
    bool startedBuilding = sidebar->startBuildingItemIfOk(listId, iUpgradeType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to start upgrade [%s] iUpgradeType = [%d], with listId[%d] - SUCCESS",
                    upgrades[iUpgradeType].description, iUpgradeType, listId);
        } else {
            sprintf(msg, "Wanting to start upgrade [%s] iUpgradeType = [%d], with listId[%d] - FAILED",
                    upgrades[iUpgradeType].description, iUpgradeType, listId);
        }
        logbook(msg);
    }
    return startedBuilding;
}

/**
 * Returns true if anything is built from ConstYard
 */
bool cPlayer::isBuildingStructureAwaitingPlacement() const {
    return itemBuilder->isAnythingBeingBuiltForListIdAwaitingPlacement(LIST_CONSTYARD, 0);
}

int cPlayer::findCellToPlaceStructure(int iStructureType) {
    // loop through all structures, and try to place structure
    // next to them:
    //         ww
    //           ss
    //           ss

    // s = structure:
    // w = start point (+ width of structure).

    // so starting at : x - width , y - height.
    // ending at      : x + width of structure + width of type
    // ...            : y + height of s + height of type

    if (iStructureType < 0) return -1;

    int iWidth = structures[iStructureType].bmp_width / 32;
    int iHeight = structures[iStructureType].bmp_height / 32;

    int iDistance = 0;

    int iGoodCells[50]; // remember 50 possible locations
    int iGoodCellID = 0;

    // clear out table of good locations
    memset(iGoodCells, -1, sizeof(iGoodCells));

    bool bGood = false;

    cStructureFactory *pStructureFactory = cStructureFactory::getInstance();

    // loop through structures of player
    for (int i = 0; i < MAX_STRUCTURES; i++) {

        // valid
        cAbstractStructure *pStructure = structure[i];
        if (pStructure == nullptr) continue;
        if (pStructure->getOwner() != getId()) continue;

        // scan around
        int c1 = pStructure->getCell();

        int iStartX = map.getCellX(c1);
        int c = pStructure->getCell();
        int iStartY = map.getCellY(c);

        int iEndX = iStartX + pStructure->getWidth() + 1;
        int iEndY = iStartY + pStructure->getHeight() + 1;

        int topLeftX = iStartX - iWidth;
        int topLeftY = iStartY - iHeight;

//        // check above structure if it can place
        for (int sx = topLeftX; sx < iEndX; sx++) {
            int sy = iStartY;
            int cell = map.getCellWithMapDimensions(sx, sy);

            bool r = pStructureFactory->canPlaceStructureAt(cell, iStructureType);

            if (r) {
                if (iStructureType == TURRET || iStructureType == RTURRET) {
                    // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                    int iDist = map.distance(map.getCellWithMapDimensions(sx, sy), getFocusCell());

                    if (iDist >= iDistance) {
                        iDistance = iDist;
                        iGoodCells[iGoodCellID] = cell;
                        // do not increment iGoodCell_player->getId(), so we overwrite (and end up further and further)
                    }

                } else {
                    // for turrets, the most far counts
                    iGoodCells[iGoodCellID] = cell;
                    iGoodCellID++;
                }
            }
        } // sx

        int underNeathStructureY = iStartY + pStructure->getHeight();
        // check underneath structure
        for (int sx = topLeftX; sx < iEndX; sx++) {
            int sy = underNeathStructureY;
            int cell = map.getCellWithMapDimensions(sx, sy);

            bool r = pStructureFactory->canPlaceStructureAt(cell, iStructureType);

            if (r) {
                if (iStructureType == TURRET || iStructureType == RTURRET) {
                    // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                    int iDist = map.distance(map.getCellWithMapDimensions(sx, sy), getFocusCell());

                    if (iDist >= iDistance) {
                        iDistance = iDist;
                        iGoodCells[iGoodCellID] = cell;
                        // do not increment iGoodCell_player->getId(), so we overwrite (and end up further and further)
                    }

                } else {
                    // for turrets, the most far counts
                    iGoodCells[iGoodCellID] = cell;
                    iGoodCellID++;
                }
            }
        } // sx

        // check left side
        for (int sy = topLeftY; sy < iEndY; sy++) {
            int sx = topLeftX;
            int cell = map.getCellWithMapDimensions(sx, sy);

            bool r = pStructureFactory->canPlaceStructureAt(cell, iStructureType);

            if (r) {
                if (iStructureType == TURRET || iStructureType == RTURRET) {
                    // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                    int iDist = map.distance(map.getCellWithMapDimensions(sx, sy), getFocusCell());

                    if (iDist >= iDistance) {
                        iDistance = iDist;
                        iGoodCells[iGoodCellID] = cell;
                        // do not increment iGoodCell_player->getId(), so we overwrite (and end up further and further)
                    }

                } else {
                    // for turrets, the most far counts
                    iGoodCells[iGoodCellID] = cell;
                    iGoodCellID++;
                }
            }
        } // sx

        // check right side
        int rightOfStructure = iStartX + pStructure->getWidth();
        for (int sy = topLeftY; sy < iEndY; sy++) {
            int sx = rightOfStructure;
            int cell = map.getCellWithMapDimensions(sx, sy);

            bool r = pStructureFactory->canPlaceStructureAt(cell, iStructureType);

            if (r) {
                if (iStructureType == TURRET || iStructureType == RTURRET) {
                    // for turrets, find the most 'far out' places (so they end up at the 'outer ring' of the base)
                    int iDist = map.distance(map.getCellWithMapDimensions(sx, sy), getFocusCell());

                    if (iDist >= iDistance) {
                        iDistance = iDist;
                        iGoodCells[iGoodCellID] = cell;
                        // do not increment iGoodCell_player->getId(), so we overwrite (and end up further and further)
                    }

                } else {
                    // for turrets, the most far counts
                    iGoodCells[iGoodCellID] = cell;
                    iGoodCellID++;
                }
            }
        } // sx

        if (iGoodCellID > 10) {
            break; // found 10 good spots
        }
    }

    if (iGoodCellID > 0) {
        char msg[255];
        sprintf(msg, "Found %d possible cells to place structureType [%d(=%s)]", iGoodCellID, iStructureType,
                structures[iStructureType].name);
        logbook(msg);
        return (iGoodCells[rnd(iGoodCellID)]);
    }

    return -1;
}

eCantBuildReason cPlayer::canBuildUnit(int iUnitType) {
    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.
    char msg[255];
    sprintf(msg, "canBuildUnit: Wanting to build iUnitType = [%d(=%s)] for player [%d(=%s)]; allowed?...", iUnitType,
            units[iUnitType].name, getId(), getHouseName().c_str());
    logbook(msg);

    // CHECK 1: Do we have the money?
    if (!hasEnoughCreditsForUnit(iUnitType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because cost %d higher than credits %d", units[iUnitType].cost,
                getCredits());
        logbook(msg);
        return eCantBuildReason::NOT_ENOUGH_MONEY; // NOPE
    }

    // CHECK 2: Are we building this unit type?
    if (isBuildingSomethingInSameListSubListAsUnitType(iUnitType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because already building unitType");
        logbook(msg);
        return eCantBuildReason::ALREADY_BUILDING;
    }

    int iStrucType = structureUtils.getStructureTypeByUnitBuildId(iUnitType);

    // Do the reality-check, do we have the building needed?
    if (!hasAtleastOneStructure(iStrucType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because we do not own the required structure type [%s] for this unit: [%s]",
                structures[iStrucType].name, units[iUnitType].name);
        logbook(msg);
        return eCantBuildReason::REQUIRES_STRUCTURE;
    }

    if (!canBuildUnitType(iUnitType)) {
        // not available to build (not in list)
        // assume it requires an upgrade?
        return eCantBuildReason::REQUIRES_UPGRADE;
    }

    logbook("canBuildUnit: ALLOWED");

    return eCantBuildReason::NONE;
}

int cPlayer::findRandomUnitTarget(int playerIndexToAttack) {
    // Randomly assemble list, and then pick one
    int iTargets[100];
    memset(iTargets, -1, sizeof(iTargets));

    int maxTargets = 0;

    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (cUnit.iPlayer != playerIndexToAttack) continue;
        // unit belongs to player of the player we wish to attack

        bool isVisibleForPlayer = map.isVisible(cUnit.getCell(), this);

        if (DEBUGGING) {
            char msg[255];
            sprintf(msg, "AI %d (House %d) -> Visible = %d", getId(), getHouse(), isVisibleForPlayer);
            logbook(msg);
        }

        // HACK HACK: the AI player does not need to discover an enemy player yet
        if (isVisibleForPlayer || game.bSkirmish) {
            iTargets[maxTargets] = i;
            maxTargets++;

            if (maxTargets > 99)
                break;
        }
    }

    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "Targets %d", maxTargets);
        logbook(msg);
    }

    return iTargets[rnd(maxTargets)];
}

int cPlayer::findRandomStructureTarget(int iAttackPlayer) {
    // Randomly assemble list, and then pick one
    int iTargets[100];
    memset(iTargets, -1, sizeof(iTargets));

    int iT = 0;

    for (int i = 0; i < MAX_STRUCTURES; i++)
        if (structure[i])
            if (structure[i]->getOwner() == iAttackPlayer)
                if (map.isVisible(structure[i]->getCell(), this) ||
                    game.bSkirmish) {
                    iTargets[iT] = i;

                    iT++;

                    if (iT > 99)
                        break;
                }


    if (DEBUGGING) {
        char msg[255];
        sprintf(msg, "STR] Targets %d", iT);
        logbook(msg);
    }


    return (iTargets[rnd(iT)]);
}

eCantBuildReason cPlayer::canBuildStructure(int iStructureType) {
    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.
    char msg[255];
    s_Structures &structureType = structures[iStructureType];
    sprintf(msg, "canBuildStructure: Wanting to build iStructureType = [%d(=%s)] for player [%d(=%s)]; allowed?...",
            iStructureType, structureType.name, getId(), getHouseName().c_str());
    logbook(msg);

    // CHECK 1: Do we have the money?
    if (!hasEnoughCreditsForStructure(iStructureType)) {
        char msg[255];
        sprintf(msg, "canBuildStructure: FALSE, because cost %d higher than credits %d", structureType.cost,
                getCredits());
        logbook(msg);
        return eCantBuildReason::NOT_ENOUGH_MONEY; // NOPE
    }

    // Do the reality-check, do we have the building needed?
    if (!hasAtleastOneStructure(CONSTYARD)) {
        char msg[255];
        sprintf(msg,
                "canBuildStructure: FALSE, because we do not own the required structure type [%s] for this structure: [%s]",
                structures[CONSTYARD].name, structureType.name);
        logbook(msg);
        return eCantBuildReason::REQUIRES_STRUCTURE;
    }

    // CHECK 2: Are we building another structure already?
    if (isBuildingStructure()) {
        char msg[255];
        sprintf(msg, "canBuildStructure: FALSE, because already building (another) structure");
        logbook(msg);
        return eCantBuildReason::ALREADY_BUILDING;
    }

    if (!canBuildStructureType(iStructureType)) {
        // not available to build (not in list)
        // assume it requires an upgrade?
        return eCantBuildReason::REQUIRES_UPGRADE;
    }

    logbook("canBuildStructure: ALLOWED");

    return eCantBuildReason::NONE;
}

cAbstractStructure *cPlayer::placeStructure(int destinationCell, int iStructureTypeId, int healthPercentage) {
    // create structure
    cStructureFactory *pStructureFactory = cStructureFactory::getInstance();
    return pStructureFactory->createStructure(destinationCell, iStructureTypeId, getId(), healthPercentage);
}

cAbstractStructure *cPlayer::placeStructure(int destinationCell, cBuildingListItem *itemToPlace) {
    int iStructureTypeId = itemToPlace->getBuildId();
    // create structure
    cStructureFactory *pStructureFactory = cStructureFactory::getInstance();

    bool canPlace = pStructureFactory->canPlaceStructureAt(destinationCell, iStructureTypeId);
    if (!canPlace) {
        return nullptr;
    }

    if (autoSlabStructures) {
        pStructureFactory->slabStructure(destinationCell, iStructureTypeId, getId());
    }

    int slabbed = pStructureFactory->getSlabStatus(destinationCell, iStructureTypeId);
    int height = structures[iStructureTypeId].bmp_height / TILESIZE_HEIGHT_PIXELS;
    int width = structures[iStructureTypeId].bmp_width / TILESIZE_WIDTH_PIXELS;
    int surface = width * height;

    int healthPercentage = 50 + health_bar(50, slabbed, surface); // the minimum is 50% (with no slabs)

    cAbstractStructure *pStructure = pStructureFactory->createStructure(destinationCell,
                                                                        iStructureTypeId,
                                                                        getId(),
                                                                        healthPercentage);

    buildingListUpdater->onBuildItemCompleted(itemToPlace);
    itemToPlace->decreaseTimesToBuild();
    itemToPlace->setPlaceIt(false);
    itemToPlace->setIsBuilding(false);
    itemToPlace->resetProgress();
    if (itemToPlace->getTimesToBuild() < 1) {
        itemBuilder->removeItemFromList(itemToPlace);
    }

    return pStructure;
}

void cPlayer::onNotify(const s_GameEvent &event) {
    // notify building list updater if it was a structure of mine. So it gets removed from the building list.
    if (event.player == this) {
        if (event.entityType == eBuildType::STRUCTURE) {
            if (event.eventType == eGameEventType::GAME_EVENT_DESTROYED) {
                buildingListUpdater->onStructureDestroyed(event.entitySpecificType);
            } else if (event.eventType == eGameEventType::GAME_EVENT_CREATED) {
                buildingListUpdater->onStructureCreated(event.entitySpecificType);
            }
        }
    }

    // pass event to brain
    if (brain_) {
        brain_->onNotify(event);
    }
}

void cPlayer::setAutoSlabStructures(bool value) {
    autoSlabStructures = value;
}

int cPlayer::getScoutingUnitType() {
    // TODO: make this configurable within house props
    switch (house) {
        case HARKONNEN:
            return QUAD;
        case ATREIDES:
            return TRIKE;
        case ORDOS:
            return RAIDER;
        case SARDAUKAR:
            return QUAD;
        case FREMEN:
            return TRIKE;
    }

    return TRIKE;
}

bool cPlayer::hasEnoughPowerFor(int structureType) const {
    assert(structureType > -1);
    int powerLeft = powerProduce_ - powerUsage_;
    return structures[structureType].power_drain <= powerLeft;
}
