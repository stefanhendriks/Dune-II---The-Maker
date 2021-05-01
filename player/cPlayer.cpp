/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include <vector>
#include <algorithm>
#include "../include/d2tmh.h"
#include "cPlayer.h"


cPlayer::cPlayer() {
	itemBuilder = NULL;
	orderProcesser = NULL;
	sidebar = NULL;
	structurePlacer = NULL;
	buildingListUpdater = NULL;
	gameControlsContext = NULL;
	char msg[255];
	sprintf(msg, "MAX_STRUCTURETYPES=[%d], sizeof bmp_structure=%d, sizeof(BITMAP *)", MAX_STRUCTURETYPES, sizeof(bmp_structure), sizeof(BITMAP*));
	logbook(msg);
	memset(bmp_structure, 0, sizeof(bmp_structure));
	memset(bmp_unit, 0, sizeof(bmp_unit));
	memset(bmp_unit_top, 0, sizeof(bmp_unit_top));
	brain_ = nullptr;
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
	if (structurePlacer) {
		delete structurePlacer;
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

void cPlayer::setOrderProcesser(cOrderProcesser * theOrderProcesser) {
	assert(theOrderProcesser);

	if (orderProcesser) {
		delete orderProcesser;
	}

	orderProcesser = theOrderProcesser;
}

void cPlayer::setStructurePlacer(cStructurePlacer * theStructurePlacer) {
	assert(theStructurePlacer);

	if (structurePlacer) {
		delete structurePlacer;
	}

	structurePlacer = theStructurePlacer;
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

void cPlayer::init(int id, cPlayerBrain *brain) {
    if (id < 0 || id >= MAX_PLAYERS) {
        char msg[255];
        sprintf(msg, "Error initializing player, id %d is not valid.", id);
        logbook(msg);
    }
    assert(id >= HUMAN);
    assert(id < MAX_PLAYERS);
    this->id = id;

    // delete old brain object if it was set before
    if (brain_) {
        delete brain_;
    }
    // set new brain
    brain_ = brain;

    memcpy(pal, general_palette, sizeof(pal));
	house = GENERALHOUSE;

	/**
	 * Ok, so this is confusing.
	 * There are also aiPlayer classes. They hold some 'brains' I guess. all other state is stored here.
	 * So even though there are aiPlayer objects, they are complimentary to this class
	 */
	m_Human = (id == HUMAN);

	if (difficultySettings) delete difficultySettings;
	difficultySettings = new cPlayerAtreidesDifficultySettings();

	// Reset structures amount
	for (int i = 0 ; i < MAX_STRUCTURETYPES; i++) {
		iStructures[i] = 0;
		iPrimaryBuilding[i] = -1;
		iStructureUpgradeLevel[i] = 0;
	}

	credits	=	0;
    maxCredits_	= 1000;
    focusCell_	=	0;

    powerUsage_=0;
    powerProduce_=0;

    iTeam=-1;
}

/**
 * Sets house value , but also sets up difficulty settings + creates bitmaps for that corresponding house
 * @param iHouse
 */
void cPlayer::setHouse(int iHouse) {
    int currentHouse = house;
    char msg[255];
    sprintf(msg, "cPlayer[%d]::setHouse - Current house is [%d/%s], setting house to [%d/%s]", this->id, currentHouse, this->getHouseNameForId(currentHouse).c_str(), iHouse, this->getHouseNameForId(iHouse).c_str());
    logbook(msg);
    house = iHouse;      // use rules of this house

    if (difficultySettings) {
        delete difficultySettings;
    }

    if (iHouse == ATREIDES) {
        difficultySettings = new cPlayerAtreidesDifficultySettings();
    } else if (iHouse == ORDOS) {
        difficultySettings = new cPlayerOrdosDifficultySettings();
    } else if (iHouse == HARKONNEN) {
        difficultySettings = new cPlayerHarkonnenDifficultySettings();
    } else {
        // for now default is atreides
        // TODO: create for other houses difficultysettings
        difficultySettings = new cPlayerAtreidesDifficultySettings();
    }

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
	switch(houseId) {
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
	if (game.bSkirmish) {
       return powerProduce_ >= powerUsage_;
    }

    // AI cheats on power
    if (id > 0) {
        // original dune 2 , the AI cheats.
        // Unfortunatly D2TM has to cheat too, else the game will
        // be unplayable.
        if (iStructures[WINDTRAP] > 0) {
            // always enough power so it seems?
            return true;
        } else {
            return false; // not enough power
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
    for (int i=0; i < MAX_UNITS; i++) {
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
    for (int i=0; i < MAX_UNITS; i++) {
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
        BITMAP *shadow = create_bitmap_ex(colorDepth, bmp_width,bmp_height);
        clear_to_color(shadow, makecol(255,0,255));

        blit((BITMAP *)units[index].shadow, shadow, start_x, start_y, 0, 0, bmp_width, bmp_height);
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
    switch(house) {
        case ATREIDES: return eHouseBitFlag::Atreides;
        case HARKONNEN: return eHouseBitFlag::Harkonnen;
        case ORDOS: return eHouseBitFlag::Ordos;
        case SARDAUKAR: return eHouseBitFlag::Sardaukar;
        case FREMEN: return eHouseBitFlag::Fremen;
        default:
            return eHouseBitFlag::Unknown;
    }
}

void cPlayer::increaseStructureAmount(int structureType) {
    if (structureType < 0) return;
    if (structureType >= MAX_STRUCTURETYPES) return;
    iStructures[structureType]++;

    char msg[255];
    sprintf(msg, "Player[%d] - increaseStructureAmount result: iStructures[%d(=%s)]=%d", id, structureType, structures[structureType].name, iStructures[structureType]);
    logbook(msg);
}

void cPlayer::decreaseStructureAmount(int structureType) {
    if (structureType < 0) return;
    if (structureType >= MAX_STRUCTURETYPES) return;

    iStructures[structureType]--;

    char msg[255];
    sprintf(msg, "Player[%d] - decreaseStructureAmount result: iStructures[%d(=%s)]=%d", id, structureType, structures[structureType].name, iStructures[structureType]);
    logbook(msg);
}

std::string cPlayer::getHouseName() {
    return getHouseNameForId(house);
}

std::string cPlayer::getHouseNameForId(int house) const {
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

std::vector<int> cPlayer::getAllMyStructures() {
    std::vector<int> ids = std::vector<int>();
    for (int i = 0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure * abstractStructure = structure[i];
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

void cPlayer::substractCredits(int amount)  {
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
 * Think function (called when AI should do something). Delegates to brain (if set).
 */
void cPlayer::think() {
    if (brain_) {
        brain_->think();
    }
}

