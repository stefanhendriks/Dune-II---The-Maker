#include "../../include/d2tmh.h"
#include "cStructureFactory.h"


cStructureFactory *cStructureFactory::instance = NULL;

cStructureFactory::cStructureFactory() {
}

cStructureFactory::~cStructureFactory() {
}

cStructureFactory *cStructureFactory::getInstance() {
	if (instance == nullptr) {
		instance = new cStructureFactory();
	}
	return instance;
}

cAbstractStructure *cStructureFactory::createStructureInstance(int type) {
	// Depending on type, create proper derived class. The constructor
    // will take care of the rest
    if (type == CONSTYARD)		return new cConstYard;
    if (type == STARPORT)		return new cStarPort;
    if (type == WINDTRAP)		return new cWindTrap;
    if (type == SILO)			return new cSpiceSilo;
    if (type == REFINERY)		return new cRefinery;
    if (type == RADAR)			return new cOutPost;
    if (type == PALACE)			return new cPalace;
    if (type == HIGHTECH)		return new cHighTech;
    if (type == LIGHTFACTORY)      return new cLightFactory;
    if (type == HEAVYFACTORY)      return new cHeavyFactory;
    if (type == TURRET)      return new cGunTurret;
    if (type == RTURRET)      return new cRocketTurret;
    if (type == REPAIR)      return new cRepairFacility;
    if (type == IX)      return new cIx;
    if (type == WOR)      return new cWor;
    if (type == BARRACKS)      return new cBarracks;
	return nullptr;
}

void cStructureFactory::deleteStructureInstance(cAbstractStructure *pStructure) {
    // delete memory acquired
    structure[pStructure->getStructureId()] = nullptr;
    delete pStructure;
}


/**
	Shorter version, creates structure at full health.
**/
cAbstractStructure* cStructureFactory::createStructure(int iCell, int iStructureType, int iPlayer) {
	return createStructure(iCell, iStructureType, iPlayer, 100);
}

/**
	Create a structure, place it and return a reference to this created class.

	This method will return NULL when either an error occurred, or the creation
	of a non-structure type (ie SLAB/WALL) is done.
**/
cAbstractStructure* cStructureFactory::createStructure(int iCell, int iStructureType, int iPlayer, int iPercent) {
	assert(iPlayer >= 0);
	assert(iPlayer <= MAX_PLAYERS);

    if (iPercent > 100) iPercent = 100;

    updatePlayerCatalogAndPlaceNonStructureTypeIfApplicable(iCell, iStructureType, iPlayer);
	clearFogForStructureType(iCell, iStructureType, 2, iPlayer);

	// SLAB and WALL is not a real structure. The terrain is manipulated
	// therefore quit here, as we won't place real structure.
    if (iStructureType == SLAB1 || iStructureType == SLAB4 || iStructureType == WALL) {
		return nullptr;
	}

    float fPercent = (float)iPercent/100; // divide by 100 (to make it 0.x)

    s_StructureInfo &structureInfo = sStructureInfo[iStructureType];
    int hp = structureInfo.hp;
    if (hp < 0) {
        cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "create structure", "Structure to create has no hp, aborting creation.");
        return nullptr;
    }

    int iNewId = getFreeSlot();

    // fail
    if (iNewId < 0) {
        cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "create structure", "No free slot available, returning NULL");
        return nullptr;
    }

    cAbstractStructure *str = createStructureInstance(iStructureType);

	if (str == NULL) {
        cLogger::getInstance()->log(LOG_INFO, COMP_STRUCTURES, "create structure", "cannot create structure: createStructureInstance returned NULL");
		return NULL; // fail
	}

    cPoint absTopLeft = map.getAbsolutePositionFromCell(iCell);
    cPlayer * player = &players[iPlayer];

    for (auto flag : structureInfo.flags) {
        cPoint pos = cPoint(
            absTopLeft.x + flag.relX,
            absTopLeft.y + flag.relY
        );

        if (flag.big) {
            str->addFlag(cFlag::createBigFlag(player, pos));
        } else {
            str->addFlag(cFlag::createSmallFlag(player, pos));
        }
    }

    // calculate actual health
    float fHealth = hp * fPercent;

    if (DEBUGGING) {
        char msg2[255];
        sprintf(msg2, "Structure with id [%d] has [%d] hp , fhealth is [%f]", iStructureType, hp, fHealth);
        logbook(msg2);
    }

    int structureSize = structureInfo.bmp_width * structureInfo.bmp_height;

    // assign to array
	structure[iNewId] = str;

    // Now set it up for location & player
    str->setCell(iCell);
    str->setOwner(iPlayer);
    str->setBuildingFase(1); // prebuild
    str->TIMER_prebuild = std::min(structureSize/16, 250); // prebuild timer. A structure of 64x64 will result in 256, bigger structure has longer timer
    str->TIMER_decay = rnd(1000) + 100;
    str->fConcrete = (1 - fPercent);
	str->setHitPoints((int)fHealth);
    str->setFrame(rnd(1)); // random start frame (flag)
    str->setStructureId(iNewId);
	str->setWidth(structureInfo.bmp_width / TILESIZE_WIDTH_PIXELS);
	str->setHeight(structureInfo.bmp_height / TILESIZE_HEIGHT_PIXELS);

 	// clear fog around structure
	clearFogForStructureType(iCell, str);

	// additional forces: (UNITS)
	if (iStructureType == REFINERY) {
		REINFORCE(iPlayer, HARVESTER, iCell+2, iCell+2);
	}

    structureUtils.putStructureOnDimension(MAPID_STRUCTURES, str);

    // handle update
    s_GameEvent event {
            .eventType = eGameEventType::GAME_EVENT_CREATED,
            .entityType = eBuildType::STRUCTURE,
            .entityID = str->getStructureId(),
            .player = str->getPlayer(),
            .entitySpecificType = iStructureType
    };

    game.onNotify(event);

    return str;
}


/**
	If this is a SLAB1, SLAB4, or WALL. Make changes in terrain.
    Also updates player catalog of structure types.
**/
void cStructureFactory::updatePlayerCatalogAndPlaceNonStructureTypeIfApplicable(int iCell, int iStructureType, int iPlayer) {
    // add this structure to the array of the player (for some score management)
    cPlayer &cPlayer = players[iPlayer];
    cPlayer.increaseStructureAmount(iStructureType);

	if (iStructureType == SLAB1) {
		mapEditor.createCell(iCell, TERRAIN_SLAB, 0);
		return; // done
	}

    if (iStructureType == SLAB4) {
		if (map.occupied(iCell) == false) {
			if (map.getCellType(iCell) == TERRAIN_ROCK) {
				mapEditor.createCell(iCell, TERRAIN_SLAB, 0);
			}
		}

        int cellRight = map.getCellRight(iCell);
        if (map.occupied(cellRight) == false) {
			if (map.getCellType(cellRight) == TERRAIN_ROCK) {
				mapEditor.createCell(cellRight, TERRAIN_SLAB, 0);
			}
		}

        int oneRowBelowCell = map.getCellBelow(iCell);
        if (map.occupied(oneRowBelowCell) == false) {
			if (map.getCellType(oneRowBelowCell) == TERRAIN_ROCK) {
				mapEditor.createCell(oneRowBelowCell, TERRAIN_SLAB, 0);
			}
		}

        int rightToRowBelowCell = map.getCellRight(oneRowBelowCell);
        if (map.occupied(rightToRowBelowCell) == false) {
			if (map.getCellType(rightToRowBelowCell) == TERRAIN_ROCK) {
				mapEditor.createCell(rightToRowBelowCell, TERRAIN_SLAB, 0);
			}
		}

		return;
    }

    if (iStructureType == WALL) {
    	mapEditor.createCell(iCell, TERRAIN_WALL, 0);
		// change surrounding walls here
        mapEditor.smoothAroundCell(iCell);
		return;
    }
}



/**
	Clear fog around structure, using a cAbstractStructure class.
**/
void cStructureFactory::clearFogForStructureType(int iCell, cAbstractStructure *str) {
	if (str == NULL) {
		return;
	}

	clearFogForStructureType(iCell, str->getType(), sStructureInfo[str->getType()].sight, str->getOwner());
}

/**
	Clear the cells of a structure on location of iCell, the size that is cleared is
	determined from the iStructureType
**/
void cStructureFactory::clearFogForStructureType(int iCell, int iStructureType, int iSight, int iPlayer) {
	int iWidth = sStructureInfo[iStructureType].bmp_width / 32;;
	int iHeight = sStructureInfo[iStructureType].bmp_height / 32;

    int iCellX = map.getCellX(iCell);
    int iCellY = map.getCellY(iCell);
	int iCellXMax = iCellX + iWidth;
	int iCellYMax = iCellY + iHeight;

	for (int x = iCellX; x < iCellXMax; x++) {
		for (int y = iCellY; y < iCellYMax; y++) {
            map.clearShroud(map.makeCell(x, y), iSight, iPlayer);
		}
	}
}

/**
	return free slot in array of structures.
**/
int cStructureFactory::getFreeSlot() {
	for (int i=0; i < MAX_STRUCTURES; i++) {
		if (structure[i] == nullptr) {
			return i;
		}
	}

    return -1; // NONE
}

/**
<p>
 This function will return how many (if any) slabs are found for the surface of a structure type. This function
 does *not* check if a structure can be placed at this location.
 </p>
<p>
	Ie, given a constyard of 2x2 (4 cells), this can return 0 (no slabs) or 4 (fully slabbed) or in between.
</p>
 <p>
 <b>Returns:</b><br>
 Any amount of cells that are "slabbed".
 </p>
**/
int cStructureFactory::getSlabStatus(int iCell, int iStructureType) {
    if (!map.isValidCell(iCell)) return 0;

    // checks if this structure can be placed on this cell
    int w = sStructureInfo[iStructureType].bmp_width / TILESIZE_WIDTH_PIXELS;
    int h = sStructureInfo[iStructureType].bmp_height / TILESIZE_HEIGHT_PIXELS;

    int slabs = 0;

    int x = map.getCellX(iCell);
    int y = map.getCellY(iCell);

    for (int cx = 0; cx < w; cx++) {
        for (int cy = 0; cy < h; cy++) {
            int cll = map.getCellWithMapBorders(cx + x, cy + y);

            if (cll < 0) {
                continue;
            }

            // If the 'terrain' type is 'slab' increase value of found slabs.
            if (map.getCellType(cll) == TERRAIN_SLAB) {
                slabs++;
            }
        }
    }

    return slabs; // ranges from 0 to <max slabs possible of building (ie height * width in cells)
}

void cStructureFactory::createSlabForStructureType(int iCell, int iStructureType) {
    // DUPLICATED BY slabStructure?
	assert(iCell > -1);

	int height = sStructureInfo[iStructureType].bmp_height / 32;
	int width = sStructureInfo[iStructureType].bmp_width / 32;

	int cellX = map.getCellX(iCell);
	int cellY = map.getCellY(iCell);

	int endCellX = cellX + width;
	int endCellY = cellY + height;
	for (int x = cellX; x < endCellX; x++) {
		for (int y = cellY; y < endCellY; y++) {
			int cell = map.getCellWithMapDimensions(x, y);
			mapEditor.createCell(cell, TERRAIN_SLAB, 0);
		}
	}
}

void cStructureFactory::deleteAllExistingStructures() {
	for (int i=0; i < MAX_STRUCTURES; i++) {
		// clear out all structures
        cAbstractStructure *pStructure = structure[i];
        if (pStructure) {
            delete pStructure;
		}
		// clear pointer
        structure[i] = nullptr;
	}
}

void cStructureFactory::destroy() {
    cStructureFactory::getInstance()->deleteAllExistingStructures();
    if (instance) {
        delete instance;
    }
}


void cStructureFactory::slabStructure(int iCll, int iStructureType, int iPlayer) {
    const s_StructureInfo &sStructures = sStructureInfo[iStructureType];

    int width = sStructures.bmp_width / TILESIZE_WIDTH_PIXELS;
    int height = sStructures.bmp_height / TILESIZE_HEIGHT_PIXELS;

    int x = map.getCellX(iCll);
    int y = map.getCellY(iCll);

    int endX = x + width;
    int endY = y + height;

    for (int sx = x; sx < endX; sx++) {
        for (int sy = y; sy < endY; sy++) {
            createStructure(map.getCellWithMapBorders(sx, sy), SLAB1, iPlayer);
        }
    }
}