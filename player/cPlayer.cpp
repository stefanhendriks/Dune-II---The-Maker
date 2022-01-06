#include <vector>
#include <algorithm>
#include "../include/d2tmh.h"

#include <fmt/core.h>

cPlayer::cPlayer() {
    itemBuilder = nullptr;
    orderProcesser = nullptr;
    sidebar = nullptr;
    buildingListUpdater = nullptr;
    gameControlsContext = nullptr;
    bmp_flag = nullptr;
    bmp_flag_small = nullptr;
    char msg[255];
    sprintf(msg, "MAX_STRUCTURE_BMPS=[%d], sizeof bmp_structure=%d, sizeof(BITMAP *)=%d", MAX_STRUCTURE_BMPS,
            sizeof(bmp_structure), sizeof(BITMAP *));
    logbook(msg); // no log(), because we can't assume player is fully initialized yet
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
    destroy_bitmap(bmp_flag);
    destroy_bitmap(bmp_flag_small);
    clearStructureTypeBitmaps();
    clearUnitTypeBitmaps();
}

void cPlayer::clearStructureTypeBitmaps() {
    for (int i = 0; i < MAX_STRUCTURE_BMPS; i++) {
        if (bmp_structure[i]) {
//            char msg[255];
//            sprintf(msg, "clearStructureTypeBitmaps: Destroying bmp_structure for index [%d].", i);
//            log(msg);
            destroy_bitmap(bmp_structure[i]);
        } else {
//            char msg[255];
//            sprintf(msg, "clearStructureTypeBitmaps: Index [%d] is null.", i);
//            log(msg);
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
//                char msg[255];
//                sprintf(msg, "clearUnitTypeBitmaps: Destroying bmp_unit for index [%d].", i);
//                log(msg);
            }
            destroy_bitmap(bmp_unit[i]);
        }
        bmp_unit[i] = nullptr;

        if (bmp_unit_top[i]) {
            if (DEBUGGING) {
//                char msg[255];
//                sprintf(msg, "clearUnitTypeBitmaps: Destroying bmp_unit_top for index [%d].", i);
//                log(msg);
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
        logbook(msg); // no log(), as house still has to be set up
    }
    assert(id >= HUMAN);
    assert(id < MAX_PLAYERS);
    this->id = id;
    this->selected_structure = -1;

    bDeployIt=false;
    bDeployedIt=false;

    // by default we're alive
    alive = true;

    spiceQuota = 0;

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
    logbook(msg); // not yet set up house properly.. so use logbook instead of log()
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
        if (sHouseInfo[house].swap_color > -1) {
            int start = sHouseInfo[house].swap_color;
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

        minimapColor = sHouseInfo[house].minimap_color;
        emblemBackgroundColor = getEmblemBackgroundColorForHouse(house);

        destroyAllegroBitmaps();

        int colorDepthBmpScreen = bitmap_color_depth(bmp_screen);

        // use this palette to draw stuff
        select_palette(pal);

        // copy flag(s) with correct color
        BITMAP *flagBmpData = (BITMAP *) gfxdata[BUILDING_FLAG_LARGE].dat;
        bmp_flag = create_bitmap_ex(colorDepthBmpScreen, flagBmpData->w, flagBmpData->h);
        clear_to_color(bmp_flag, makecol(255, 0, 255));
        draw_sprite(bmp_flag, flagBmpData, 0, 0);

        flagBmpData = (BITMAP *) gfxdata[BUILDING_FLAG_SMALL].dat;
        bmp_flag_small = create_bitmap_ex(colorDepthBmpScreen, flagBmpData->w, flagBmpData->h);
        clear_to_color(bmp_flag_small, makecol(255, 0, 255));
        draw_sprite(bmp_flag_small, flagBmpData, 0, 0);

        // now copy / set all structures for this player, with the correct color
        for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
            s_StructureInfo &structureType = sStructureInfo[i];

            if (!structureType.configured) continue;

            bmp_structure[i] = create_bitmap_ex(colorDepthBmpScreen, structureType.bmp->w, structureType.bmp->h);
            if (!bmp_structure[i]) {
                allegro_message("Could not create bmp structure bitmap!? - Imminent crash.");
            }
            clear_to_color(bmp_structure[i], makecol(255, 0, 255));

            draw_sprite(bmp_structure[i], structureType.bmp, 0, 0);

            // flash bitmaps are structure type index * 2
            if (structureType.flash) {
                int j = MAX_STRUCTURETYPES + i;
                BITMAP *bitmap = create_bitmap_ex(colorDepthBmpScreen, structureType.bmp->w, structureType.bmp->h);
                if (!bitmap) {
                    allegro_message("Could not create FLASH bmp structure bitmap!? - Imminent crash.");
                }
                clear_to_color(bitmap, makecol(255, 0, 255));

                draw_sprite(bitmap, structureType.flash, 0, 0);
                bmp_structure[j] = bitmap;
            }

        }


        // same goes for units
        for (int i = 0; i < MAX_UNITTYPES; i++) {
            s_UnitInfo &unitType = sUnitInfo[i];

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

bool cPlayer::hasAlmostReachMaxSpiceStorageCapacity() const {
    return !bEnoughSpiceCapacityToStoreCredits(250);
}

bool cPlayer::bEnoughSpiceCapacityToStoreCredits() const {
    return bEnoughSpiceCapacityToStoreCredits(0);
}

bool cPlayer::bEnoughSpiceCapacityToStoreCredits(int threshold) const {
    return (maxCredits_ - threshold) > credits;
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

std::string cPlayer::asString() const {
    char msg[512];
    sprintf(msg, "Player [id=%d, human=%d, sidebar=%p]", this->id, this->m_Human, this->sidebar);
    return std::string(msg);
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
    return getAllMyUnitsForType(unitType).size();
}

void cPlayer::markUnitsForGroup(const int groupId) const {
    // go over all units, and mark units for this group if selected.
    // and unmark them for the group when not/no longer selected.
    for (auto &pUnit : unit) {
        if (!pUnit.isValid()) continue;
        if (!pUnit.belongsTo(this)) continue;
        if (pUnit.bSelected) {
            pUnit.iGroup = groupId;
            continue;
        }

        // unit belongs to this group, but is not/no longer selected. So unmark it.
        if (pUnit.iGroup == groupId) {
            pUnit.iGroup = -1;
        }
    }
}

std::vector<int> cPlayer::getAllMyUnitsForGroupNr(const int groupId) const {
    std::vector<int> ids = std::vector<int>();
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue;
        if (pUnit.isDead()) continue;
        if (!pUnit.belongsTo(this)) continue;
        if (pUnit.isMarkedForRemoval()) continue; // do not count marked for removal units

        if (pUnit.iGroup == groupId) {
            ids.push_back(i);
        }
    }
    return ids;
}

std::vector<int> cPlayer::getAllMyUnitsWithinViewportRect(const cRectangle &rect) const {
    std::vector<int> ids = std::vector<int>();
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue;
        if (pUnit.isDead()) continue;
        if (!pUnit.belongsTo(this)) continue;
        if (pUnit.isMarkedForRemoval()) continue; // do not count marked for removal units

        if (!rect.isPointWithin(pUnit.center_draw_x(), pUnit.center_draw_y())) {
            continue;
        }

        ids.push_back(i);
    }
    return ids;
};

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
 * Returns the flash bitmap for structure type "index", this structure has been colorized beforehand for this player and is
 * in same color depth as bmp_screen.
 * @param index
 * @return
 */
BITMAP *cPlayer::getStructureBitmapFlash(int index) {
    return getStructureBitmap(MAX_STRUCTURETYPES + index); // by convention flash bmp's are stored starting at MAX + index
}

BITMAP *cPlayer::getFlagBitmap() {
    return bmp_flag;
}

BITMAP *cPlayer::getFlagSmallBitmap() {
    return bmp_flag_small;
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
    if (sUnitInfo[index].shadow) {
        int bmp_width = sUnitInfo[index].bmp_width;
        int bmp_height = sUnitInfo[index].bmp_height;
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

        blit(sUnitInfo[index].shadow, shadow, start_x, start_y, 0, 0, bmp_width, bmp_height);
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
    return credits >= requestedAmount;
}

bool cPlayer::hasEnoughCreditsForUnit(int unitType) {
    if (unitType < 0 || unitType >= MAX_UNITTYPES) return false;
    return this->credits >= sUnitInfo[unitType].cost;
}

bool cPlayer::hasEnoughCreditsForStructure(int structureType) {
    if (structureType < 0 || structureType >= MAX_STRUCTURETYPES) return false;
    return this->credits >= sStructureInfo[structureType].cost;
}

bool cPlayer::hasEnoughCreditsForUpgrade(int upgradeType) {
    if (upgradeType < 0 || upgradeType >= MAX_UPGRADETYPES) return false;
    return this->credits >= sUpgradeInfo[upgradeType].cost;
}

/**
 * Returns house based fading/pulsating color
 * @return
 */
int cPlayer::getHouseFadingColor() const {
    int color = makecol(255, 255, 255);
    if (house == ATREIDES) {
        color = makecol(0, 0, 255);
    }
    if (house == HARKONNEN) {
        color = makecol(255, 0, 0);
    }
    if (house == ORDOS) {
        color = makecol(0, 255, 0);
    }

    // TODO other sHouseInfo (Sardaukar, etc)
    return game.getColorFadeSelected(color);
}

/**
 * Returns the error fading color (red to black pulsating)
 * @return
 */
int cPlayer::getErrorFadingColor() const {
    return game.getColorFadeSelectedRed(255, 0, 0);
}

/**
 * Returns the error fading color (red to black pulsating)
 * @return
 */
int cPlayer::getPrimaryBuildingFadingColor() const {
    return game.getColorFadeSelectedGreen(0, 255, 0);
}

/**
 * Returns the fading white color
 * @return
 */
int cPlayer::getSelectFadingColor() const {
    return game.getColorFadeSelected(255, 255, 255);
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
    sprintf(msg, "increaseStructureAmount result: iStructures[%d(=%s)]=%d", structureType,
            sStructureInfo[structureType].name, iStructures[structureType]);
    log(msg);
}

void cPlayer::decreaseStructureAmount(int structureType) {
    if (structureType < 0) return;
    if (structureType >= MAX_STRUCTURETYPES) return;

    iStructures[structureType]--;

    char msg[255];
    sprintf(msg, "decreaseStructureAmount result: iStructures[%d(=%s)]=%d", structureType,
            sStructureInfo[structureType].name, iStructures[structureType]);
    log(msg);
}

std::string cPlayer::getHouseName() const {
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

/**
 * Returns all structure ids belonging to player
 *
 * NOTE: This is a slow method, as it iterates though all possible unit ids
 *
 * TODO: This can be done smarter because we receive notifications when a structure gets created/destroyed!
 * @return
 */
std::vector<int> cPlayer::getAllMyStructuresAsId() {
    return getAllMyStructuresAsIdForType(-1);
}

std::vector<int> cPlayer::getAllMyStructuresAsIdForType(int structureType) {
    std::vector<int> ids = std::vector<int>();
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *abstractStructure = structure[i];
        if (!abstractStructure) continue;
        if (!abstractStructure->isValid()) continue;
        if (!abstractStructure->belongsTo(this)) continue;

        // should filter by structure type?
        if (structureType > -1) {
            if (abstractStructure->getType() != structureType) continue; // not the same? skip...
        }

        ids.push_back(i);
    }

    return ids;
}

bool cPlayer::isSameTeamAs(const cPlayer *pPlayer) {
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

    gameControlsContext->updateMouseState();
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
 * Think function, called every 100ms.
 */
void cPlayer::think() {
    if (brain_) {
        brain_->think();
    }
}

/**
 * Think function, called every 5 ms.
 */
void cPlayer::thinkFast() {
    if (sidebar) {
        sidebar->think();
    }

    if (brain_) {
        brain_->thinkFast();
    }

    for (auto &notification : notifications) {
        notification.thinkFast();
    }

    // remove notifications no longer visible
    notifications.erase(
            std::remove_if(
                    notifications.begin(),
                    notifications.end(),
                    [](cPlayerNotification m) { return !m.isVisible(); }),
            notifications.end()
    );
}


void cPlayer::setBrain(brains::cPlayerBrain *brain) {
    // delete old brain object if it was set before
    if (brain_) {
        delete brain_;
    }
    // set new brain
    brain_ = brain;
}

bool cPlayer::isStructureTypeAvailableForConstruction(int iStructureType) const {
    cBuildingListItem *pItem = sidebar->getBuildingListItem(eListType::LIST_CONSTYARD, iStructureType);
    return pItem != nullptr;
}

bool cPlayer::canBuildUnitType(int iUnitType) const {
    eListType listType = sUnitInfo[iUnitType].listType;
    cBuildingListItem *pItem = sidebar->getBuildingListItem(listType, iUnitType);
    char msg[255];
    bool result = pItem != nullptr;
    sprintf(msg, "canBuildUnitType(unitType=%d) -> %s", iUnitType, result ? "TRUE" : "FALSE");
    log(msg);
    return result;
}

bool cPlayer::canBuildSpecialType(int iType) const {
    eListType listType = sSpecialInfo[iType].listType;
    cBuildingListItem *pItem = sidebar->getBuildingListItem(listType, iType);

    bool result = pItem != nullptr;

    char msg[255];
    sprintf(msg, "canBuildSpecialType(iType=%d) -> %s", iType, result ? "TRUE" : "FALSE");
    log(msg);

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
    return itemBuilder->getListItemWhichIsBuilding(eListType::LIST_CONSTYARD, 0);
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
    s_UnitInfo &p = sUnitInfo[iUnitType];
    eListType listType = p.listType;
    int subListId = p.subListId;

    return isBuildingAnythingForListAndSublist(listType, subListId);
}

bool cPlayer::isBuildingAnythingForListAndSublist(eListType listType,
                                                  int subListId) const {
    return itemBuilder->isAnythingBeingBuiltForListType(listType, subListId);
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
    cBuildingList *pList = sidebar->getList(eListType::LIST_UPGRADES);
    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = pList->getItem(i);
        if (pItem == nullptr) continue;
        const s_UpgradeInfo &theUpgrade = pItem->getUpgradeInfo();
        if (theUpgrade.providesType != providesType) continue;
        if (theUpgrade.providesTypeId == providesTypeId) {
            return pItem;
        }
    }
    return nullptr;
}


/**
 * Checks if any Upgrade is in progress for the given listType/sublistId
 *
 * @param listType
 * @param sublistId
 * @return
 */
cBuildingListItem *cPlayer::isUpgradingList(eListType listType, int sublistId) const {
    int listId = eListTypeAsInt(listType);
    cBuildingList *upgradesList = sidebar->getList(eListType::LIST_UPGRADES);
    if (upgradesList == nullptr) {
        char msg[255];
        sprintf(msg,
                "isUpgradingList for listType [%d] and sublistId [%d], could not find upgradesList!? - will return FALSE.",
                listId, sublistId);
        log(msg);
        assert(false);
        return nullptr;
    }

    for (int i = 0; i < MAX_ITEMS; i++) {
        cBuildingListItem *pItem = upgradesList->getItem(i);
        if (pItem == nullptr) continue;
        const s_UpgradeInfo &theUpgrade = pItem->getUpgradeInfo();
        // is this upgrade applicable to the listType/sublistId we're interested in?
        if (theUpgrade.providesTypeList == listType && theUpgrade.providesTypeSubList == sublistId) {
            if (pItem->isBuilding()) {
                // it is in progress, so yes!
                return pItem;
            }
        }
    }
    return nullptr;
}

cBuildingListItem *cPlayer::isUpgradingConstyard() const {
    return isUpgradingList(eListType::LIST_CONSTYARD, 0);
}

/**
 * Returns true if anything is built from ConstYard
 */
cBuildingListItem *cPlayer::isBuildingStructure() const {
    return itemBuilder->getListItemWhichIsBuilding(eListType::LIST_CONSTYARD, 0);
}

bool cPlayer::startBuildingUnit(int iUnitType) const {
    s_UnitInfo &unitType = sUnitInfo[iUnitType];
    eListType listType = unitType.listType;
    bool startedBuilding = sidebar->startBuildingItemIfOk(listType, iUnitType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d], with listType[%d] - SUCCESS", unitType.name,
                    iUnitType, eListTypeAsInt(listType));
        } else {
            sprintf(msg, "Wanting to build unit [%s] iUnitType = [%d], with listType[%d] - FAILED", unitType.name,
                    iUnitType, eListTypeAsInt(listType));
        }
        log(msg);
    }
    return startedBuilding;
}

bool cPlayer::startBuildingStructure(int iStructureType) const {
    eListType listType = eListType::LIST_CONSTYARD;

    bool startedBuilding = sidebar->startBuildingItemIfOk(listType, iStructureType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build structure [%s] iStructureType = [%d], with listType[%d] - SUCCESS",
                    sStructureInfo[iStructureType].name, iStructureType, eListTypeAsInt(listType));
        } else {
            sprintf(msg, "Wanting to build structure [%s] iStructureType = [%d], with listType[%d] - FAILED",
                    sStructureInfo[iStructureType].name, iStructureType, eListTypeAsInt(listType));
        }
        log(msg);
    }
    return startedBuilding;
}

bool cPlayer::startBuildingSpecial(int iSpecialType) const {
    eListType listType = eListType::LIST_PALACE;

    bool startedBuilding = sidebar->startBuildingItemIfOk(listType, iSpecialType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to build special [%s] iSpecialType = [%d], with listType[%d] - SUCCESS",
                    sSpecialInfo[iSpecialType].description, iSpecialType, eListTypeAsInt(listType));
        } else {
            sprintf(msg, "Wanting to build special [%s] iSpecialType = [%d], with listType[%d] - FAILED",
                    sSpecialInfo[iSpecialType].description, iSpecialType, eListTypeAsInt(listType));
        }
        log(msg);
    }
    return startedBuilding;
}

bool cPlayer::startUpgrading(int iUpgradeType) const {
    eListType listType = eListType::LIST_UPGRADES;
    bool startedBuilding = sidebar->startBuildingItemIfOk(listType, iUpgradeType);

    if (DEBUGGING) {
        char msg[255];
        if (startedBuilding) {
            sprintf(msg, "Wanting to start upgrade [%s] iUpgradeType = [%d], with listType[%d] - SUCCESS",
                    sUpgradeInfo[iUpgradeType].description, iUpgradeType, eListTypeAsInt(listType));
        } else {
            sprintf(msg, "Wanting to start upgrade [%s] iUpgradeType = [%d], with listType[%d] - FAILED",
                    sUpgradeInfo[iUpgradeType].description, iUpgradeType, eListTypeAsInt(listType));
        }
        log(msg);
    }
    return startedBuilding;
}

/**
 * Returns true if anything is built and awaiting placement from ConstYard
 */
bool cPlayer::isBuildingStructureAwaitingPlacement() const {
    return itemBuilder->isAnythingBeingBuiltForListIdAwaitingPlacement(eListType::LIST_CONSTYARD, 0);
}

/**
 * Returns true if anything is built & awaiting deployment from Palace
 */
bool cPlayer::isSpecialAwaitingPlacement() const {
    return itemBuilder->isAnythingBeingBuiltForListIdAwaitingDeployment(eListType::LIST_PALACE, 0);
}

/**
 *
 * @return
 */
cBuildingListItem * cPlayer::getSpecialAwaitingPlacement() const {
    return itemBuilder->getListItemWhichIsAwaitingDeployment(eListType::LIST_PALACE, 0);
}



/**
 * Evaluates where to place a structure. Returns a cell where structure can be placed. Will assume the area has
 * proper terrain, is not blocked by any other structure. If there are any friendly units there, it assumes
 * they can be moved away, and thus ignores them. However, enemy units will block placement.
 *
 * When no cell can be found, this function will return -1
 *
 * @param structureType
 * @return
 */
int cPlayer::findCellToPlaceStructure(int structureType) {
    // find place (fast, if possible), where to place it
    // ignore any units (we can move them out of the way). But do take
    // terrain and other structures into consideration!

    const std::vector<int> &allMyStructuresAsId = getAllMyStructuresAsId();
    std::vector<int> potentialCells = std::vector<int>();

    int iWidth = sStructureInfo[structureType].bmp_width / TILESIZE_WIDTH_PIXELS;
    int iHeight = sStructureInfo[structureType].bmp_height / TILESIZE_HEIGHT_PIXELS;

    for (auto &id : allMyStructuresAsId) {
        cAbstractStructure * aStructure = structure[id];

        // go around any structure, and try to find a cell where we can place a structure.
        int iStartX = map.getCellX(aStructure->getCell());
        int iStartY = map.getCellY(aStructure->getCell());

        int iEndX = iStartX + aStructure->getWidth(); // not plus 1 because iStartX is 1st cell
        int iEndY = iStartY + aStructure->getHeight(); // not plus 1 because iStartY is 1st cell

        // start is topleft/above structure, but also take size of the structure to place
        // into acount. So ie, a structure of 2x2 will be attempted (at first) at y - 2.
        // attempt at 'top' first:
        int topLeftX = iStartX - iWidth;
        int topLeftY = iStartY - iHeight;

        // check: from top left to top right
        for (int sx = topLeftX; sx < iEndX; sx++) {
            int cell = map.getCellWithMapBorders(sx, topLeftY);
            if (cell < 0) continue;

            const s_PlaceResult &result = canPlaceStructureAt(cell, structureType);
            bool canPlaceStructureAt = result.success || result.onlyMyUnitsBlock;
            if (canPlaceStructureAt) {
                potentialCells.push_back(cell);
            }
        }

        int bottomLeftX = topLeftX;
        int bottomLeftY = iEndY;
        // check: from bottom left to bottom right
        for (int sx = bottomLeftX; sx < iEndX; sx++) {
            int cell = map.getCellWithMapBorders(sx, bottomLeftY);
            if (cell < 0) continue;

            const s_PlaceResult &result = canPlaceStructureAt(cell, structureType);
            bool canPlaceStructureAt = result.success || result.onlyMyUnitsBlock;
            if (canPlaceStructureAt) {
                potentialCells.push_back(cell);
            }
        }

        // left to structure (not from top!)
        int justLeftX = topLeftX;
        int justLeftY = iStartY - (iHeight - 1);
        for (int sy = justLeftY; sy < iEndY; sy++) {
            int cell = map.getCellWithMapBorders(justLeftX, sy);
            if (cell < 0) continue;

            const s_PlaceResult &result = canPlaceStructureAt(cell, structureType);
            bool canPlaceStructureAt = result.success || result.onlyMyUnitsBlock;
            if (canPlaceStructureAt) {
                potentialCells.push_back(cell);
            }
        }

        // right to structure (not top!)
        int justRightX = iEndX;
        int justRightY = iStartY - (iHeight - 1);
        for (int sy = justRightY; sy < iEndY; sy++) {
            int cell = map.getCellWithMapBorders(justRightX, sy);
            if (cell < 0) continue;

            const s_PlaceResult &result = canPlaceStructureAt(cell, structureType);
            bool canPlaceStructureAt = result.success || result.onlyMyUnitsBlock;
            if (canPlaceStructureAt) {
                potentialCells.push_back(cell);
            }
        }

        // if we have found any we randomly abort
        if (!potentialCells.empty()) {
            if (rnd(100) < 33) {
                break;
            }
        }
    }

    if (!potentialCells.empty()) {
        if (structureType == TURRET || structureType == RTURRET) {
//                // first shuffle, before going through the list
//                std::random_shuffle(potentialCells.begin(), potentialCells.end());
//
//                std::vector<int> potentialFurtherCells = std::vector<int>();
//                int found = 0;
//                double distance = 128; // arbitrary distance as 'border'
//                for (auto &potentialCell : potentialCells) {
//                    double dist = map.distance(centerOfBase, potentialCell);
//                    if (dist > distance) {
//                        potentialFurtherCells.push_back(potentialCell);
//                        found++;
//                        if (found > 5) {
//                            break;
//                        }
//                    }
//                }
//
//                if (!potentialFurtherCells.empty()) {
//                    // shuffle the 5 'furthest'
//                    std::random_shuffle(potentialFurtherCells.begin(), potentialFurtherCells.end());
//                }

            // for now pick random position, but in the future do something more smart
            std::random_shuffle(potentialCells.begin(), potentialCells.end());
        } else {
            // found one, shuffle, and then return the first
            std::random_shuffle(potentialCells.begin(), potentialCells.end());
        }
        return potentialCells.front();
    }

    return -1;
}

eCantBuildReason cPlayer::canBuildUnit(int iUnitType, bool checkIfAffordable) {
    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.
    char msg[255];
    sprintf(msg, "canBuildUnit: Wanting to build iUnitType = [%d(=%s)] allowed?...", iUnitType, sUnitInfo[iUnitType].name);
    log(msg);

    // CHECK 1: Do we have the money?
    if (checkIfAffordable) {
        if (!hasEnoughCreditsForUnit(iUnitType)) {
            char msg[255];
            sprintf(msg, "canBuildUnit: FALSE, because cost %d higher than credits %d", sUnitInfo[iUnitType].cost, getCredits());
            log(msg);
            return eCantBuildReason::NOT_ENOUGH_MONEY; // NOPE
        }
    }

    // CHECK 2: Are we building this unit type?
    if (isBuildingSomethingInSameListSubListAsUnitType(iUnitType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because already building unitType");
        log(msg);
        return eCantBuildReason::ALREADY_BUILDING;
    }

    int iStrucType = structureUtils.getStructureTypeByUnitBuildId(iUnitType);

    // Do the reality-check, do we have the building needed?
    if (!hasAtleastOneStructure(iStrucType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because we do not own the required structure type [%s] for this unit: [%s]",
                sStructureInfo[iStrucType].name, sUnitInfo[iUnitType].name);
        log(msg);
        return eCantBuildReason::REQUIRES_STRUCTURE;
    }

    if (iUnitType == DEVASTATOR || iUnitType == SONICTANK || iUnitType == DEVIATOR) {
        if (!hasAtleastOneStructure(IX)) {
            char msg[255];
            sprintf(msg, "canBuildUnit: FALSE, because we do not own the required ADDITIONAL structure type [%s] for this unit: [%s]",
                    sStructureInfo[IX].name, sUnitInfo[iUnitType].name);
            log(msg);
            return eCantBuildReason::REQUIRES_ADDITIONAL_STRUCTURE;
        }
    }

    if (!canBuildUnitType(iUnitType)) {
        // not available to build (not in list)
        // assume it requires an upgrade?
        char msg[255];
        sprintf(msg, "canBuildUnit: REQUIRES_UPGRADE, because we can't find it in the expected list [%d] for this unit: [%s]",
                eListTypeAsInt(sUnitInfo[iUnitType].listType), sUnitInfo[iUnitType].name);
        log(msg);
        return eCantBuildReason::REQUIRES_UPGRADE;
    }

    log("canBuildUnit: ALLOWED");

    return eCantBuildReason::NONE;
}

eCantBuildReason cPlayer::canBuildSpecial(int iType) {
    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.
    char msg[255];
    s_SpecialInfo &special = sSpecialInfo[iType];
    sprintf(msg, "canBuildSpecial: Wanting to build iType = [%d(=%s)] allowed?...", iType, special.description);
    log(msg);

    // Do we have the building needed?
    int iStrucType = PALACE; // TODO: get from "special" data structure?
    if (!hasAtleastOneStructure(iStrucType)) {
        char msg[255];
        sprintf(msg, "canBuildUnit: FALSE, because we do not own the required structure type [%s] for [%s]",
                sStructureInfo[iStrucType].name, special.description);
        log(msg);
        return eCantBuildReason::REQUIRES_STRUCTURE;
    }

    // Are we building this thing already?
    if (isBuildingAnythingForListAndSublist(special.listType, special.subListId)) {
        char msg[255];
        sprintf(msg, "canBuildSpecial: FALSE, because already building it");
        log(msg);
        return eCantBuildReason::ALREADY_BUILDING;
    }

    if (!canBuildSpecialType(iType)) {
        // not available to build (not in list)
        // assume it requires an upgrade?
        char msg[255];
        sprintf(msg, "canBuildUnit: REQUIRES_UPGRADE, because we can't find it in the expected list [%d] for this special: [%s]",
                eListTypeAsInt(special.listType), special.description);
        log(msg);
        return eCantBuildReason::REQUIRES_UPGRADE;
    }

    log("canBuildSpecial: ALLOWED");

    return eCantBuildReason::NONE;
}

eCantBuildReason cPlayer::canBuildUnit(int iUnitType) {
    return canBuildUnit(iUnitType, true);
}

bool cPlayer::canBuildUnitBool(int iUnitType) {
    return canBuildUnit(iUnitType, false) == eCantBuildReason::NONE;
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
            sprintf(msg, "Visible = %d", isVisibleForPlayer);
            log(msg);
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
        log(msg);
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
        log(msg);
    }


    return (iTargets[rnd(iT)]);
}

eCantBuildReason cPlayer::canBuildStructure(int iStructureType) {
    char msg[255];
    sprintf(msg, "canBuildStructure: Wanting to build iStructureType = [%d]", iStructureType);
    log(msg);

    assert(iStructureType > -1 && "Structure type must be > -1");
    assert(iStructureType < MAX_STRUCTURETYPES && "Structure type must be < MAX_STRUCTURETYPES");

    // Once known, a check will be made to see if the AI has a structure to produce that
    // unit type. If not, it will return false.
    const s_StructureInfo &structureType = sStructureInfo[iStructureType];
    sprintf(msg, "canBuildStructure: Wanting to build iStructureType = [%d(=%s)], allowed?...",
            iStructureType, structureType.name);
    log(msg);

    // CHECK 1: Do we have the money?
    if (!hasEnoughCreditsForStructure(iStructureType)) {
        char msg[255];
        sprintf(msg, "canBuildStructure: FALSE, reason NOT_ENOUGH_MONEY: because cost %d > credits %d", structureType.cost,
                getCredits());
        log(msg);
        return eCantBuildReason::NOT_ENOUGH_MONEY; // NOPE
    }

    // Do the reality-check, do we have the building needed?
    if (!hasAtleastOneStructure(CONSTYARD)) {
        char msg[255];
        sprintf(msg,
                "canBuildStructure: FALSE, reason REQUIRES_STRUCTURE: we do not own the required structure type [%s] for this structure: [%s]",
                sStructureInfo[CONSTYARD].name, structureType.name);
        log(msg);
        return eCantBuildReason::REQUIRES_STRUCTURE;
    }

    // CHECK 2: Are we building another structure already?
    if (isBuildingStructure()) {
        char msg[255];
        sprintf(msg, "canBuildStructure: FALSE, reason ALREADY_BUILDING");
        log(msg);
        return eCantBuildReason::ALREADY_BUILDING;
    }

    if (!isStructureTypeAvailableForConstruction(iStructureType)) {
        // not available to build (not in list)
        // assume it requires an upgrade?
        char msg[255];
        sprintf(msg, "canBuildStructure: FALSE, reason REQUIRES_UPGRADE: because we can't find structure in list");
        log(msg);
        return eCantBuildReason::REQUIRES_UPGRADE;
    }

    log("canBuildStructure: ALLOWED");

    return eCantBuildReason::NONE;
}

cAbstractStructure *cPlayer::placeStructure(int destinationCell, int iStructureTypeId, int healthPercentage) {
    cStructureFactory *pStructureFactory = cStructureFactory::getInstance();
    bool canPlace = canPlaceStructureAt(destinationCell, iStructureTypeId).success;
    if (!canPlace) {
        return nullptr;
    }
    return pStructureFactory->createStructure(destinationCell, iStructureTypeId, getId(), healthPercentage);
}

/**
 * Place building list item
 * @param destinationCell
 * @param itemToPlace
 * @return
 */
cAbstractStructure *cPlayer::placeItem(int destinationCell, cBuildingListItem *itemToPlace) {
    int iStructureTypeId = itemToPlace->getBuildId();
    cStructureFactory *pStructureFactory = cStructureFactory::getInstance();

    bool canPlace = canPlaceStructureAt(destinationCell, iStructureTypeId).success;
    if (!canPlace) {
        return nullptr;
    }

    if (autoSlabStructures) {
        pStructureFactory->slabStructure(destinationCell, iStructureTypeId, getId());
    }

    int slabbed = pStructureFactory->getSlabStatus(destinationCell, iStructureTypeId);
    int height = sStructureInfo[iStructureTypeId].bmp_height / TILESIZE_HEIGHT_PIXELS;
    int width = sStructureInfo[iStructureTypeId].bmp_width / TILESIZE_WIDTH_PIXELS;
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
    if (event.eventType == eGameEventType::GAME_EVENT_PLAYER_DEFEATED) {
        char msg[255];
        sprintf(msg, "Player %d (%s) has been defeated.", event.player->getId(), event.player->getHouseName().c_str());
        addNotification(msg, eNotificationType::BAD);
    }

    if (event.eventType == eGameEventType::GAME_EVENT_SPECIAL_LAUNCHED) {
        char msg[255];
        sprintf(msg, "%s launched!", event.buildingListItem->getNameString().c_str());
        addNotification(msg, eNotificationType::BAD);
    }

    if (event.player == this) {
        if (event.eventType == eGameEventType::GAME_EVENT_SPECIAL_SELECT_TARGET) {
            char msg[255];
            sprintf(msg, "%s is ready for deployment.", event.buildingListItem->getNameString().c_str());
            addNotification(msg, eNotificationType::PRIORITY);
        }

        if (event.eventType == eGameEventType::GAME_EVENT_CREATED) {
            // it is mine
        }

        if (event.eventType == eGameEventType::GAME_EVENT_DISCOVERED) {
            onEntityDiscovered(event);
        }

        if (event.entityType == eBuildType::STRUCTURE) {
            if (event.eventType == eGameEventType::GAME_EVENT_DESTROYED) {
                buildingListUpdater->onStructureDestroyed(event.entitySpecificType);
            } else if (event.eventType == eGameEventType::GAME_EVENT_CREATED) {
                buildingListUpdater->onStructureCreated(event.entitySpecificType);
            }
        } else if (event.entityType == eBuildType::UNIT) {
            if (event.eventType == eGameEventType::GAME_EVENT_DESTROYED) {
                onMyUnitDestroyed(event);
            }
        }

        if (event.eventType == eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED) {
            if (event.entityType == eBuildType::UPGRADE) {
                char msg[255];
                sprintf(msg, "Upgrade: %s completed.", event.buildingListItem->getNameString().c_str());
                addNotification(msg, eNotificationType::PRIORITY);
            }
        }

        if (event.eventType == eGameEventType::GAME_EVENT_LIST_ITEM_FINISHED ||
            event.eventType == eGameEventType::GAME_EVENT_LIST_ITEM_ADDED ||
            event.eventType == eGameEventType::GAME_EVENT_LIST_ITEM_CANCELLED ||
            event.eventType == eGameEventType::GAME_EVENT_SPECIAL_LAUNCH) {
            if (cBuildingListItem::isAutoBuild(event.entityType, event.entitySpecificType)) {
                startBuilding(event.entityType, event.entitySpecificType);
            }
        }
    }

    // pass event to brain
    if (brain_) {
        brain_->onNotify(event);
    }

    if (sidebar) {
        sidebar->onNotify(event);
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


int cPlayer::getInfantryType() {
    // TODO: make this configurable within house props
    switch (house) {
        case HARKONNEN:
            return TROOPERS;
        case ATREIDES:
            return INFANTRY;
        case ORDOS:
            return INFANTRY;
        case SARDAUKAR:
            return TROOPERS;
        case FREMEN:
            return UNIT_FREMEN_THREE;
    }

    return INFANTRY;
}

int cPlayer::getSoldierType() {
    // TODO: make this configurable within house props
    switch (house) {
        case HARKONNEN:
            return TROOPER;
        case ATREIDES:
            return SOLDIER;
        case ORDOS:
            return SOLDIER;
        case SARDAUKAR:
            return TROOPER;
        case FREMEN:
            return UNIT_FREMEN_ONE;
    }

    return SOLDIER;
}

int cPlayer::getSpecialUnitType() {
    // TODO: make this configurable within house props
    switch (house) {
        case HARKONNEN:
            return DEVASTATOR;
        case ATREIDES:
            return SONICTANK;
        case ORDOS:
            return DEVIATOR;
        case SARDAUKAR:
            return DEVASTATOR;
        case FREMEN:
            return TRIKE;
    }

    return TRIKE;
}

bool cPlayer::hasEnoughPowerFor(int structureType) const {
    assert(structureType > -1 && "hasEnoughPowerFor called with structureType < 0!");
    assert(structureType < MAX_STRUCTURETYPES && "hasEnoughPowerFor called with structureType >= MAX_STRUCTURETYPES!");
    int powerLeft = powerProduce_ - powerUsage_;
    return sStructureInfo[structureType].power_drain <= powerLeft;
}

void cPlayer::logStructures() {
    log("cPlayer::logStructures() START");
    for (int i = 0; i < MAX_STRUCTURETYPES; i++) {
        char msg[255];
        sprintf(msg, "[%s] amount [%d]", sStructureInfo[i].name, iStructures[i]);
        log(msg);
    }
    log("cPlayer::logStructures() END");
}

void cPlayer::cancelBuildingListItem(cBuildingListItem *item) {
    this->sidebar->cancelBuildingListItem(item);
}

void cPlayer::cancelStructureBuildingListItemBeingBuilt() {
    cBuildingListItem *pItem = getStructureBuildingListItemBeingBuilt();
    if (pItem) {
        cancelBuildingListItem(pItem);
    }
}

/**
<p>
	This function will check if at iCell (the upper left corner of a structure) a structure
	can be placed of type "iStructureType". This is calling  canPlaceStructureAt without any
    unitID to ignore. Meaning, any unit, structure, or invalid terrain type will make this function return false.
 </p>
 <p>
 <b>Returns:</b><br>
 <ul>
 <li>result object</li>
 <ul>
 </p>

 * @param iCell
 * @param iStructureType
 * @param iUnitIDToIgnore
 * @return
 */
s_PlaceResult cPlayer::canPlaceStructureAt(int iCell, int iStructureType) {
    if (iStructureType != SLAB4) {
        return canPlaceStructureAt(iCell, iStructureType, -1);
    }
    // SLAB4 logic is a bit different
    return canPlaceConcreteAt(iCell);
}


/**
<p>
	This function will check if at iCell (the upper left corner of a structure) a structure
	can be placed of type "iStructureType". If iUnitIDTOIgnore is > -1, then if any unit is
	supposedly 'blocking' this structure from placing, it will be ignored.
 </p>
<p>
	Ie, you will use the iUnitIDToIgnore value when you want to create a Const Yard on the
	location of an MCV.
</p>
 <p>
	If you know the structure can be placed, you can use getSlabStatus to get the amount of 'slabs' are covering
    the structure dimensions in order to calculate the structure health upon placement.
</p>
 <p>
 <b>Returns:</b><br>
 <ul>
 <li>s_PlaceResult = if success there is nothing in the way to place structure</li>
 <ul>
 </p>

 * @param iCell
 * @param iStructureType
 * @param iUnitIDToIgnore
 * @return
 */
s_PlaceResult cPlayer::canPlaceStructureAt(int iCell, int iStructureType, int iUnitIDToIgnore) {
    s_PlaceResult result;

    if (!map.isValidCell(iCell)) {
        result.outOfBounds = true;
        return result;
    }

    // checks if this structure can be placed on this cell
    int w = sStructureInfo[iStructureType].bmp_width / TILESIZE_WIDTH_PIXELS;
    int h = sStructureInfo[iStructureType].bmp_height / TILESIZE_HEIGHT_PIXELS;

    int x = map.getCellX(iCell);
    int y = map.getCellY(iCell);

    bool foundUnitFromOtherPlayerThanMe = false;

    for (int cx = 0; cx < w; cx++) {
        for (int cy = 0; cy < h; cy++) {
            int cll = map.getCellWithMapBorders(cx + x, cy + y);

            if (!result.badTerrain && !map.isValidTerrainForStructureAtCell(cll)) {
                result.badTerrain = true;
            }

            // another structure found on this location, "blocked"
            int structureId = map.getCellIdStructuresLayer(cll);
            if (structureId > -1) {
                result.structureIds.insert(structureId);
            }

            int idOfUnitAtCell = map.getCellIdUnitLayer(cll);
            if (idOfUnitAtCell > -1) {
                if (unit[idOfUnitAtCell].isValid() && unit[idOfUnitAtCell].getPlayer() != this) {
                    foundUnitFromOtherPlayerThanMe = true;
                }
                if (iUnitIDToIgnore > -1) {
                    if (idOfUnitAtCell != iUnitIDToIgnore) {
                        result.unitIds.insert(idOfUnitAtCell);
                    }
                } else {
                    result.unitIds.insert(idOfUnitAtCell);
                }
            }
        }
    }

    result.success = (result.badTerrain == false && result.unitIds.empty() && result.structureIds.empty());
    result.onlyMyUnitsBlock = (result.badTerrain == false && !foundUnitFromOtherPlayerThanMe && result.structureIds.empty());

    return result;
}

void cPlayer::log(const std::string& txt) const {
  if (DEBUGGING) {
    logbook(fmt::format("PLAYER [{}(={})] : {}", getId(), getHouseName(), txt));
  }
}

bool cPlayer::startUpgradingForUnitIfPossible(int iUpgradeType) const {
    cBuildingListItem *upgrade = isUpgradeAvailableToGrantUnit(iUpgradeType);
    if(upgrade && upgrade->isAvailable() && !upgrade->isBuilding()) {
        return startUpgrading(upgrade->getBuildId());
    }
    return false;
}

bool cPlayer::startBuilding(cBuildingListItem *pItem) {
    if (!pItem) {
        log("startBuilding: Cannot start building null item!");
        return false;
    }
    
    return startBuilding(pItem->getBuildType(), pItem->getBuildId());
}

void cPlayer::onEntityDiscovered(const s_GameEvent &event) {
//    if (!event.player->isHuman()) { // todo == interacting player? (have a mentat/sound thing that listens to events??)
//        // do nothing
//        return;
//    }
    if (game.iMusicType != MUSIC_PEACE) {
        // nothing to do here music-wise
        return;
    }

    bool discoveringPlayerIsSameTeamAsThisPlayer = isSameTeamAs(event.player);

    int voiceId = -1;

    // when state of music is not attacking, do attacking stuff and say "Warning enemy unit approaching
    bool triggerMusic = false;
    if (event.entityType == eBuildType::UNIT) {
        cUnit &pUnit = unit[event.entityID];
        bool detectedEntityIsHuman = pUnit.getPlayer()->isHuman();

        // unit discovered is NOT the same team, so enemy detected / music trigger
        if (detectedEntityIsHuman || isHuman()) {
            if (discoveringPlayerIsSameTeamAsThisPlayer && !isSameTeamAs(pUnit.getPlayer())) {
                triggerMusic = true;
                if (pUnit.iType == SANDWORM) {
                    voiceId = SOUND_VOICE_10_ATR;
                } else {
                    voiceId = SOUND_VOICE_09_ATR;
                }
            }
        } else {
            // this entity, nor I am human. So do not care about music stuff.
            return;
        }
    } else if (event.entityType == eBuildType::STRUCTURE) {
        cAbstractStructure *pStructure = structure[event.entityID];
        bool detectedEntityIsHuman = pStructure->getPlayer()->isHuman();

        // structure discovered is NOT the same team, so enemy detected / music trigger
        if (detectedEntityIsHuman || isHuman()) {
            if (discoveringPlayerIsSameTeamAsThisPlayer && !isSameTeamAs(pStructure->getPlayer())) {
                // only things that can harm us will trigger attack music?
                if (event.entitySpecificType == RTURRET || event.entitySpecificType == TURRET) {
                    triggerMusic = true;
                }
            }
        } else {
            // this entity, nor I am human. So do not care about music stuff.
            return;
        }
    }

    if (triggerMusic) {
        playMusicByType(MUSIC_ATTACK);
    }

    if (voiceId > -1) {
        play_voice(voiceId);
    }
}

bool cPlayer::startBuilding(eBuildType buildType, int buildId) {
    switch (buildType) {
        case SPECIAL:
            return startBuildingSpecial(buildId);
        case STRUCTURE:
            return startBuildingStructure(buildId);
        case UNIT:
            return startBuildingUnit(buildId);
        case UPGRADE:
            return startUpgrading(buildId);
        default:
            assert(false && "startBuilding: Unknown buildType !?");
    }
    return false;
}

bool cPlayer::couldBuildSpecial(int iType) {
    s_SpecialInfo &special = sSpecialInfo[iType];
    if (special.house & getHouseBitFlag()) {
        // it is applicable for this house
        return true;
    }
    return false;
}

/**
 * Checks if 'requestedUnitType' is a unit Type this house can build. If not, it will return house specific
 * alternatives. Returns requestedUnitType if the type is allowed for house.
 *
 * @param requestedUnitType
 * @return
 */
int cPlayer::getSameOrSimilarUnitType(int requestedUnitType) {
    // correct type for specific player type
    if (requestedUnitType == DEVASTATOR || requestedUnitType == DEVIATOR || requestedUnitType == SONICTANK) {
        return getSpecialUnitType();
    }

    if (requestedUnitType == SOLDIER || requestedUnitType == TROOPER) {
        return getSoldierType();
    }

    if (requestedUnitType == INFANTRY || requestedUnitType == TROOPERS) {
        return getInfantryType();
    }

    if (requestedUnitType == QUAD || requestedUnitType == TRIKE || requestedUnitType == RAIDER) {
        return getScoutingUnitType();
    }
    return requestedUnitType;
}

/**
 * Concrete slabs logic
 * @param iCell
 * @return
 */
s_PlaceResult cPlayer::canPlaceConcreteAt(int iCell) {
    s_PlaceResult result;

    if (!map.isValidCell(iCell)) {
        result.outOfBounds = true;
        return result;
    }

    result.success = true;
    result.onlyMyUnitsBlock = false;

    return result;
}

void cPlayer::onMyUnitDestroyed(const s_GameEvent &event) {
    cUnit &pUnit = unit[event.entityID];

    // If a harvester died, and it is the last. And we have atleast one REFINERY; then send a Harvester to that
    // player
    if (pUnit.isHarvester()) { // a harvester died
        addNotification("You've lost a Harvester.", eNotificationType::PRIORITY);

        const std::vector<int> &refineries = getAllMyStructuresAsIdForType(REFINERY);

        if (!refineries.empty()) { // and its player still has a refinery)
            int harvesters = getAmountOfUnitsForType(HARVESTER);
            // check if the player has any harvester left

            // if 1, or less
            if (harvesters < 2) {
                addNotification("You have one Harvester left.", eNotificationType::NEUTRAL);
            }

            // No harvester found, deliver one
            if (harvesters < 1) {
                // deliver
                cAbstractStructure *refinery = pUnit.findClosestStructureType(REFINERY);

                // found a refinery, deliver harvester to that
                if (refinery) {
                    REINFORCE(id, HARVESTER, refinery->getCell(), -1);
                }
            }

            for (auto &structureId: refineries) {
                cAbstractStructure *pStructure = structure[structureId];
                pStructure->unitIsNoLongerInteractingWithStructure(event.entityID);
            }
        } else {
            int harvesters = getAmountOfUnitsForType(HARVESTER);

            if (harvesters < 1) {
                addNotification("No harvesters and refineries left!", eNotificationType::BAD);
            }
        }
    }
}

/**
 * Returns all unit ids belonging to player
 *
 * NOTE: This is a slow method, as it iterates though all possible unit ids
 *
 * TODO: This could be done smarter once we receive notifications when a unit gets created/destroyed!
 * @return
 */
std::vector<int> cPlayer::getAllMyUnits() {
    return getAllMyUnitsForType(-1);
}

/**
 * Returns a vector of all unit ID's for unit type
 *
 * @param unitType
 * @return
 */
std::vector<int> cPlayer::getAllMyUnitsForType(int unitType) const {
    std::vector<int> ids = std::vector<int>();
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &pUnit = unit[i];
        if (!pUnit.isValid()) continue;
        if (pUnit.isDead()) continue;
        if (!pUnit.belongsTo(this)) continue;
        if (pUnit.isMarkedForRemoval()) continue; // do not count marked for removal units

        // check for unit type?
        if (unitType > -1) {
            if (pUnit.iType != unitType) continue; // not the same type? skip
        }

        ids.push_back(i);
    }
    return ids;
}

bool cPlayer::hasMetQuota() {
    return spiceQuota > 0 && hasEnoughCreditsFor(spiceQuota);
}

bool cPlayer::evaluateStillAlive() {
    alive = false;
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *abstractStructure = structure[i];
        if (!abstractStructure) continue;
        if (!abstractStructure->isValid()) continue;
        if (!abstractStructure->belongsTo(this)) continue;
        alive = true;
        break;
    }

    if (!alive) {
        // check units now
        for (int i = 0; i < MAX_UNITS; i++) {
            cUnit &pUnit = unit[i];
            if (!pUnit.isValid()) continue;
            if (pUnit.isAirbornUnit()) continue; // do not count airborn units
            if (pUnit.isDead()) continue; // in case we have some 'half-dead' units that got pass the isValid check...
            // a better way for this would be to have such units in a separate collection.
            if (!pUnit.belongsTo(this)) continue;
            alive = true;
            break;
        }
    }
    return isAlive();
}

std::vector<cPlayerNotification> &cPlayer::getNotifications() {
    return notifications;
}

void cPlayer::addNotification(const char *msg, eNotificationType type) {
    notifications.push_back(cPlayerNotification(msg, type));
    std::sort(notifications.begin(), notifications.end(), [](const cPlayerNotification &lhs, const cPlayerNotification &rhs) {
        return lhs.getTimer() > rhs.getTimer();
    });
    if (DEBUGGING) {
        char logbookMsg[512];
        sprintf(logbookMsg, "addNotification : type %s - %s", eNotificationTypeString(type), msg);
        log(logbookMsg);
    }
}

cAbstractStructure *cPlayer::getSelectedStructure() const {
    if (selected_structure < 0) return nullptr;
    return structure[selected_structure];
}

bool cPlayer::isNotDeployingSomething() {
    return bDeployIt == false && bDeployedIt == false;
}

void cPlayer::deselectStructure() {
    selected_structure = -1;
}

std::vector<int> cPlayer::getSelectedUnits() const {
    std::vector<int> ids = std::vector<int>();
    for (int i = 0; i < MAX_UNITS; i++) {
        cUnit &cUnit = unit[i];
        if (!cUnit.isValid()) continue;
        if (!cUnit.belongsTo(this)) continue;
        if (cUnit.bSelected) {
            ids.push_back(i);
        }
    }
    return ids;
}

void cPlayer::deselectAllUnits() {
    const std::vector<int> &ids = getAllMyUnits();
    for (auto i : ids) {
        unit[i].bSelected = false;
    }
}

bool cPlayer::selectUnitsFromGroup(int groupId) {
    const std::vector<int> &ids = getAllMyUnitsForGroupNr(groupId);
    return selectUnits(ids);
}

bool cPlayer::selectUnits(const std::vector<int> &ids) const {
    bool infantrySelected = false;
    bool unitSelected = false;

    // check if there is a harvester in this group
    auto position = std::find_if(ids.begin(), ids.end(), [&](const int &id) { return unit[id].isHarvester(); });
    bool hasHarvesterSelected = position != ids.end();

    position = std::find_if(ids.begin(), ids.end(),
                            [&](const int &id) { return !unit[id].isHarvester() && !unit[id].isAirbornUnit(); });
    bool nonAirbornNonHarvesterUnitSelected = position != ids.end();

    if (hasHarvesterSelected && !nonAirbornNonHarvesterUnitSelected) {
        // select all the harvester units, skip airborn
        for (auto id: ids) {
            cUnit &pUnit = unit[id];
            if (pUnit.isAirbornUnit()) continue;
            if (!pUnit.isHarvester()) continue;
            // only check if it has not been selected, so we only play sound when we truly select a new unit.
            if (!pUnit.bSelected) {
                pUnit.bSelected = true;
                unitSelected = true; // do it here, instead of iterating again
            }
        }
    } else {
        // select all the non-harvester, non-airborn units
        for (auto id: ids) {
            cUnit &pUnit = unit[id];
            if (pUnit.isAirbornUnit()) continue;
            if (pUnit.isHarvester()) continue;
            // only check if it has not been selected, so we only play sound when we truly select a new unit.
            if (!pUnit.bSelected) {
                pUnit.bSelected = true;
                if (pUnit.isInfantryUnit()) {
                    infantrySelected = true;
                } else {
                    unitSelected = true;
                }
            }
        }
    }

    if (unitSelected) {
        play_sound_id(SOUND_REPORTING);
    }

    if (infantrySelected) {
        play_sound_id(SOUND_YESSIR);
    }

    // return true if we selected any unit
    return unitSelected || infantrySelected;
}

void cPlayer::setContextMouseState(eMouseState newState) {
    gameControlsContext->setMouseState(newState);
}

bool cPlayer::isContextMouseState(eMouseState state) {
    return gameControlsContext->isState(state);
}