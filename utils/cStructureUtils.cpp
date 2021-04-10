#include "../include/d2tmh.h"


cStructureUtils::cStructureUtils() {
	init(&map);
}

cStructureUtils::~cStructureUtils() {
}

int cStructureUtils::getHeightOfStructureTypeInCells(int structureType) {
	assert(structureType >= 0);
	assert(structureType < MAX_STRUCTURETYPES);
	return structures[structureType].bmp_height / TILESIZE_HEIGHT_PIXELS;
}

int cStructureUtils::getWidthOfStructureTypeInCells(int structureType) {
	assert(structureType >= 0);
	assert(structureType < MAX_STRUCTURETYPES);
	return structures[structureType].bmp_width / TILESIZE_WIDTH_PIXELS;
}


/**
 * This is almost the same as the findStructureToDeployUnit
 *
 * However, whenever the pPlayer has not set any primary building. Try to find a structure that has some free cell around it.
 */
int cStructureUtils::findStarportToDeployUnit(cPlayer * pPlayer) {
	assert(pPlayer);
	int playerId = pPlayer->getId();

	// check primary building first if set
	int primaryBuildingOfStructureType = pPlayer->getPrimaryStructureForStructureType(STARPORT);

	if (primaryBuildingOfStructureType > -1) {
		cAbstractStructure * theStructure = structure[primaryBuildingOfStructureType];
		if (theStructure && theStructure->getNonOccupiedCellAroundStructure() > -1) {
			return primaryBuildingOfStructureType;
		}
	}

	// find a starport that has space around it.
	int starportId = -1;
	int firstFoundStarportId = -1;
	bool foundStarportWithFreeAround = false;
	for (int i=0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];
		if (theStructure && theStructure->getOwner() == playerId) {
			if (theStructure->getType() == STARPORT) {
				// when no id set, always set one id
				if (starportId < 0) {
					starportId = i;
				}

				if (firstFoundStarportId < 0) {
					firstFoundStarportId = i;
				}

				if (theStructure->getNonOccupiedCellAroundStructure() > -1) {
					// when free around structure, override id and break out of loop
					starportId = i;
					foundStarportWithFreeAround = true;
					break;
				}
			}
		}
	}

	if (!foundStarportWithFreeAround) {
		if (primaryBuildingOfStructureType > -1) {
			return primaryBuildingOfStructureType;
		}
		if (firstFoundStarportId > -1) {
			return firstFoundStarportId;
		}
	}
	return starportId;

}

/**
 * Finds a building to deploy a unit from. Does this by first checking if a primary structure has been set;
 * if so, it will check if the primary building has space around it. If not, then it will check other structures.
 * If found other structure to deploy (space around) it will automatically set the primary building to that structure.
 *
 * Returns the structure ID of the found structure.
 *
 */
int cStructureUtils::findStructureToDeployUnit(cPlayer * pPlayer, int structureType) {
	assert(pPlayer);
	assert(structureType > -1);

	int playerId = pPlayer->getId();

	if (DEBUGGING) {
		char msg[255];
		sprintf(msg, "Looking for primary building (type %d, name %s, pPlayer %d)", structureType, structures[structureType].name, playerId);
		logbook(msg);
	}

	// check primary building first if set
	int primaryBuildingOfStructureType = pPlayer->getPrimaryStructureForStructureType(structureType);

	if (primaryBuildingOfStructureType > -1) {
		cAbstractStructure * theStructure = structure[primaryBuildingOfStructureType];
		// this IF is needed, because the structure could be destroyed/replaced
		if (theStructure &&
            theStructure->isValid() &&
            theStructure->getType() == structureType &&
            theStructure->belongsTo(playerId) && // in case this changed...
		    theStructure->getNonOccupiedCellAroundStructure() > -1) {
			return primaryBuildingOfStructureType;
		}
	}

    int structureIdFound = findStructureBy(playerId, structureType, true);

	// assign as primary building
	if (structureIdFound > -1) {
		pPlayer->setPrimaryBuildingForStructureType(structureType, structureIdFound);
	}

	return structureIdFound;
}

/**
 * Depending on list item, return a structure type; used to determine where to deploy this item.
 */
int cStructureUtils::findStructureTypeByTypeOfList(cBuildingListItem *item) {
    if (!item) return -1;
    if (item->isTypeUpgrade()) {
        return -1; // no clue
    }
    if (item->isTypeStructure()) {
        return -1;
    }

    if (item->isTypeSpecial()) {
        const s_Special &special = item->getS_Special();
        if (special.providesType == UNIT) {
	        return special.deployAtStructure;
	    }
        return -1;
	}

    int unitBuildId = item->getBuildId();
    return getStructureTypeByUnitBuildId(unitBuildId);
}

int cStructureUtils::getStructureTypeByUnitBuildId(int unitBuildId) const {
    switch(unitBuildId) {
        case INFANTRY:
            return BARRACKS;
        case SOLDIER:
            return BARRACKS;
        case TROOPER:
            return WOR;
        case TROOPERS:
            return WOR;
        case QUAD:
            return LIGHTFACTORY;
        case TRIKE:
            return LIGHTFACTORY;
        case RAIDER:
            return LIGHTFACTORY;
        case TANK:
            return HEAVYFACTORY;
        case SIEGETANK:
            return HEAVYFACTORY;
        case LAUNCHER:
            return HEAVYFACTORY;
        case DEVASTATOR:
            return HEAVYFACTORY;
        case HARVESTER:
            return HEAVYFACTORY;
        case MCV:
            return HEAVYFACTORY;
        case SONICTANK:
            return HEAVYFACTORY;
        case DEVIATOR:
            return HEAVYFACTORY;
        case ORNITHOPTER:
            return HIGHTECH;
        case CARRYALL:
            return HIGHTECH;
        default:
            char msg[255];
            sprintf(msg, "Item buildId is [%d], which is not mapped", unitBuildId);
            logbook(msg);
            assert(false);
            return -1;
	}
}

int cStructureUtils::findClosestStructureTypeWhereNoUnitIsHeadingToComparedToCell(int cell, int structureType, cPlayer * pPlayer) {
	assert(pPlayer);
	assert(structureType > -1);
	assert(cell >= 0 || cell < MAX_CELLS);

	int foundStructureId=-1;	// found structure id
	long shortestDistance=9999; // max distance to search in

	int playerId = pPlayer->getId();

	for (int i=0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure *pStructure = structure[i];
        if (pStructure == nullptr) continue;
        if (pStructure->getOwner() != playerId) continue;
        if (pStructure->getType() != structureType) continue;

        if (pStructure->iUnitID < 0) {	// no other unit is heading to this structure
            long distance = map.distance(cell, pStructure->getCell());

            // if distance is lower than last found distance, it is the closest for now.
            if (distance < shortestDistance) {
                foundStructureId=i;
                shortestDistance=distance;
            }
        }
	}

	return foundStructureId;
}

void cStructureUtils::putStructureOnDimension(int dimensionId, cAbstractStructure * theStructure) {
	if (!theStructure) {
	    logbook("!theStruture");
	    // bail
	    return;
	}

	int cellOfStructure = theStructure->getCell();

	assert(cellOfStructure > -1);

	for (int w = 0; w < theStructure->getWidth(); w++) {
		for (int h = 0; h < theStructure->getHeight(); h++)	{
			int xOfStructureCell = map.getCellX(cellOfStructure);
			int yOfStructureCell = map.getCellY(cellOfStructure);

			int iCell = map.makeCell(xOfStructureCell + w, yOfStructureCell + h);

			map.cellSetIdForLayer(iCell, dimensionId, theStructure->getStructureId());
		}
	}
}

bool cStructureUtils::isStructureVisibleOnScreen(cAbstractStructure *structure) {
	if (!structure) return false;

	int drawX = structure->iDrawX();
	int drawY = structure->iDrawY();
	int width = mapCamera->factorZoomLevel(structure->getWidthInPixels());
	int height = mapCamera->factorZoomLevel(structure->getHeightInPixels());

	return (drawX + width  >= 0 && drawX < game.screen_x) &&
	       (drawY + height >= 0 && drawY < game.screen_y);
}

bool cStructureUtils::isMouseOverStructure(cAbstractStructure *structure, int screenX, int screenY) {
	assert(structure);

	// translate the structure coordinates to screen coordinates
	int drawX = structure->iDrawX();
	int drawY = structure->iDrawY();
	int width = mapCamera->factorZoomLevel(structure->getWidthInPixels());
	int height = mapCamera->factorZoomLevel(structure->getHeightInPixels());

    return cRectangle::isWithin(screenX, screenY, drawX, drawY, width, height);
}

int cStructureUtils::getTotalPowerUsageForPlayer(cPlayer * pPlayer) {
	assert(pPlayer);
	int totalPowerUsage = 0;

	for (int i = 0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];
		if (theStructure) {
			if (theStructure->getPlayer()->getId() == pPlayer->getId()) {
				int powerUsageOfStructure = theStructure->getPowerUsage();
				totalPowerUsage += powerUsageOfStructure;
			}
		}
	}

	return totalPowerUsage;
}

int cStructureUtils::getTotalSpiceCapacityForPlayer(cPlayer * pPlayer) {
	int totalCapacity = 0;
	for (int i = 0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];
		if (theStructure == nullptr) continue;
		if (!theStructure->isValid()) continue;
        if (theStructure->getPlayer()->getId() != pPlayer->getId()) continue; // does not belong to pPlayer

        int capacity = 0;
        if (theStructure->getType() == SILO) {
            cSpiceSilo * spiceSilo = dynamic_cast<cSpiceSilo*>(theStructure);
            capacity = spiceSilo->getSpiceSiloCapacity();
        } else if (theStructure->getType() == REFINERY) {
            cRefinery * refinery = dynamic_cast<cRefinery*>(theStructure);
            capacity = refinery->getSpiceSiloCapacity();
        } else if (theStructure->getType() == CONSTYARD) {
            capacity = 5;
        }
        totalCapacity += capacity;
	}
	return totalCapacity;
}

int cStructureUtils::getTotalPowerOutForPlayer(cPlayer * pPlayer) {
	assert(pPlayer);
	int totalPowerOut = 0;
	for (int i = 0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];
		if (theStructure == nullptr) continue;
		if (!theStructure->isValid()) continue;
		if (theStructure->getPlayer()->getId() != pPlayer->getId()) continue; // not for pPlayer

		// TODO abstract it further so it wont need to cast/check for structure type
        if (theStructure->getType() == WINDTRAP) {
            cWindTrap * windTrap = dynamic_cast<cWindTrap*>(theStructure);
            int powerOutOfStructure = windTrap->getPowerOut();
            totalPowerOut += powerOutOfStructure;
        } else if (theStructure->getType() == CONSTYARD) {
            totalPowerOut += 5;
        }
	}

	if (!game.bSkirmish) {
	    // ?? (mission 9 etc AI has no power)
	}
	return totalPowerOut;
}

void cStructureUtils::init(cMap *pMap) {
//    cellCalculator = new cCellCalculator(pMap);
}

int cStructureUtils::findHiTechToDeployAirUnit(cPlayer *pPlayer) {
    assert(pPlayer);

    int playerId = pPlayer->getId();

    // check primary building first if set
    int primaryBuildingOfStructureType = pPlayer->getPrimaryStructureForStructureType(HIGHTECH);

    if (primaryBuildingOfStructureType > -1) {
        cAbstractStructure * theStructure = structure[primaryBuildingOfStructureType];
        // this IF is needed, because the structure could be destroyed/replaced
        if (theStructure &&
            theStructure->isValid() &&
            theStructure->getType() == HIGHTECH &&
            theStructure->belongsTo(playerId) // in case this changed...
            ) {
            return primaryBuildingOfStructureType;
        }
    }

    int structureIdFound = findStructureBy(playerId, HIGHTECH, false);

    // assign as primary building
    if (structureIdFound > -1) {
        pPlayer->setPrimaryBuildingForStructureType(HIGHTECH, structureIdFound);
    }

    return structureIdFound;
}

int cStructureUtils::findStructureBy(int iPlayer, int iType, bool bFreeAround) {
    if (iPlayer < 0) return -1;
    if (iPlayer > AI_WORM) return -1;
    if (iType < 0) return -1;

    for (int i=0; i < MAX_STRUCTURES; i++) {
        cAbstractStructure * theStructure = structure[i];
        if (theStructure &&
            theStructure->isValid() &&
            theStructure->belongsTo(iPlayer) &&
            theStructure->getType() == iType) {
            if (bFreeAround) {
                if (theStructure->getNonOccupiedCellAroundStructure() > -1) {
                    return i;
                } else {
                    continue; // look for next structure with free around
                }
            }
            return i; // return this structure
            break;
        }
    }
    return -1;
}
