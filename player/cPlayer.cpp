/*

  Dune II - The Maker

  Author : Stefan Hendriks
  Contact: stefanhen83@gmail.com
  Website: http://dune2themaker.fundynamic.com

  2001 - 2020 (c) code by Stefan Hendriks

  */

#include "../include/d2tmh.h"


cPlayer::cPlayer() {
	itemBuilder = NULL;
	orderProcesser = NULL;
	sidebar = NULL;
	structurePlacer = NULL;
	upgradeBuilder = NULL;
	buildingListUpdater = NULL;
	gameControlsContext = NULL;
	char msg[255];
	sprintf(msg, "MAX_STRUCTURETYPES=[%d], sizeof bmp_structure=%d, sizeof(BITMAP *)", MAX_STRUCTURETYPES, sizeof(bmp_structure), sizeof(BITMAP*));
	logbook(msg);
	memset(bmp_structure, 0, sizeof(bmp_structure));
	memset(bmp_unit, 0, sizeof(bmp_unit));
	memset(bmp_unit_top, 0, sizeof(bmp_unit_top));
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
	if (upgradeBuilder) {
		delete upgradeBuilder;
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
            if (DEBUGGING) {
                char msg[255];
                sprintf(msg, "clearStructureTypeBitmaps: Destroying bmp_structure for index [%d].", i);
                logbook(msg);
            }
            destroy_bitmap(bmp_structure[i]);
        } else {
            if (DEBUGGING) {
                char msg[255];
                sprintf(msg, "clearStructureTypeBitmaps: Index [%d] is null.", i);
                logbook(msg);
            }
        }
        bmp_structure[i] = nullptr;
    }
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

void cPlayer::setUpgradeBuilder(cUpgradeBuilder *theUpgradeBuilder) {
	assert(theUpgradeBuilder);

	// delete old reference
	if (upgradeBuilder) {
		delete upgradeBuilder;
	}

	upgradeBuilder = theUpgradeBuilder;
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

void cPlayer::init(int id) {
    if (id < 0 || id >= MAX_PLAYERS) {
        char msg[255];
        sprintf(msg, "Error initializing player, id %d is not valid.", id);
        logbook(msg);
    }
    assert(id >= HUMAN);
    assert(id < MAX_PLAYERS);
    this->id = id;

    memcpy(pal, general_palette, sizeof(pal));
	house = GENERALHOUSE;

	/**
	 * Ok, so this is confusing.
	 * There are also aiPlayer classes. They hold some 'brains' I guess. all other state is stored here.
	 * So even though there are aiPlayer objects, they are complentary to this class
	 */
	m_Human = (id == HUMAN);

	difficultySettings = new cPlayerAtreidesDifficultySettings();

	// Reset structures amount
	for (int i = 0 ; i < MAX_STRUCTURETYPES; i++) {
		iStructures[i] = 0;
		iPrimaryBuilding[i] = -1;
	}

	credits	=	0;
	max_credits	= 1000;
	focus_cell	=	0;

	use_power=0;
	has_power=0;

    iTeam=-1;

    TIMER_think=rnd(10);        // timer for thinking itself (calling main routine)
    TIMER_attack=-1;			// -1 = determine if its ok to attack, > 0 is , decrease timer, 0 = attack
}

void cPlayer::setHouse(int iHouse) {
    int currentHouse = house;
    char msg[255];
    sprintf(msg, "cPlayer::setHouse - Current house is [%d], setting house to [%d]", currentHouse, iHouse);
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

        // now set the different colors based upon house
        if (houses[house].swap_color > -1) {
            int start = houses[house].swap_color;
            int s = 144;                // original position (harkonnen)
            for (int j = start; j < (start + 7); j++) {
                // swap everything from S with J
                pal[s] = pal[j];
                s++;
            }
        }

        minimapColor = getRGBColorForHouse(house);

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
                alert("Could not create bmp structure bitmap!?", "", "", "Uh oh...", NULL, 13, 0);
            }
            clear_to_color(bmp_structure[i], makecol(255, 0, 255));

            draw_sprite(bmp_structure[i], structureType.bmp, 0, 0);
        }

        // same goes for units
        for (int i = 0; i < MAX_UNITTYPES; i++) {
            s_UnitP &unitType = units[i];

            bmp_unit[i] = create_bitmap_ex(colorDepthBmpScreen, unitType.bmp->w, unitType.bmp->h);
            if (!bmp_unit[i]) {
                alert("Could not create bmp unit bitmap!?", "", "", "Uh oh...", NULL, 13, 0);
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

int cPlayer::getRGBColorForHouse(int houseId) {
	switch(houseId) {
		case ATREIDES:
			return makecol(0, 0, 255);
		case HARKONNEN:
			return makecol(255, 0, 0);
		case ORDOS:
			return makecol(0, 255, 0);
		case SARDAUKAR:
			return makecol(255, 0, 255);
		default:
			return makecol(100, 255, 100);
	}
}

bool cPlayer::bEnoughPower() const {
	if (game.bSkirmish) {
       return has_power >= use_power;
    }

    // AI cheats on power
    if (id > 0) {
        // original dune 2 , the AI cheats.
        // Unfortunatly D2TM has to cheat too, else the game will
        // be unplayable.
        if (iStructures[WINDTRAP] > 0) {
            // always enough power so it seems
            return true;
        } else {
            return false; // not enough power
        }
    }

    return has_power >= use_power;
}

int cPlayer::getAmountOfStructuresForType(int structureType) const {
	assert(structureType >= 0);
	assert(structureType <= RTURRET);
	return iStructures[structureType];
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
