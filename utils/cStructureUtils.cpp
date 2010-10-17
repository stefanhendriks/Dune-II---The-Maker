/*
 * cStructureUtils.cpp
 *
 *  Created on: 2-aug-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

cStructureUtils::cStructureUtils() {
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
 * However, whenever the player has not set any primary building. Try to find a structure that has some free cell around it.
 */
int cStructureUtils::findStarportToDeployUnit(cPlayer * player) {
	assert(player);
	int playerId = player->getId();

	// check primary building first if set
	int primaryBuildingOfStructureType = player->iPrimaryBuilding[STARPORT];

	if (primaryBuildingOfStructureType > -1) {
		cAbstractStructure * theStructure = structure[primaryBuildingOfStructureType];
		if (theStructure && theStructure->iFreeAround() > -1) {
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

				if (theStructure->iFreeAround() > -1) {
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
 * Finds a building to deploy a unit from. Returns the structure ID of the found structure.
 *
 * Will use primary building set by player first, before looking for alternatives.
 *
 */
int cStructureUtils::findStructureToDeployUnit(cPlayer * player, int structureType) {
	assert(player);
	assert(structureType > -1);

	int playerId = player->getId();

	if (DEBUGGING) {
		char msg[255];
		sprintf(msg, "Looking for primary building (type %d, name %s, player %d)", structureType, structures[structureType].name, playerId);
		logbook(msg);
	}

	// check primary building first if set
	int primaryBuildingOfStructureType = player->iPrimaryBuilding[structureType];

	if (primaryBuildingOfStructureType > -1) {
		cAbstractStructure * theStructure = structure[primaryBuildingOfStructureType];
		if (theStructure && theStructure->iFreeAround() > -1) {
			return primaryBuildingOfStructureType;
		}
	}

	// check other structures now
	for (int i=0; i < MAX_STRUCTURES; i++) {
		cAbstractStructure * theStructure = structure[i];
		if (theStructure && theStructure->getOwner() == playerId) {
			if (theStructure->getType() == structureType) {
				if (theStructure->iFreeAround() > -1) {
					return i; // return this structure
				}
			}
		}
	}

	return -1;
}

/**
 * Depending on list type, return a structure type.
 */
int cStructureUtils::findStructureTypeByTypeOfList(cBuildingList *list, cBuildingListItem *item) {
	assert(list);
	assert(item);
	assert(item->getBuildType() == UNIT);

	int listTypeId = list->getType();
	char msg[255];
	sprintf(msg, "going to find list with listTypeId [%d]", listTypeId);
	cLogger::getInstance()->logCommentLine(msg);
	switch (listTypeId) {
		case LIST_CONSTYARD:
			// a unit, and then built from a constyard list
			assert(false);
			return CONSTYARD;
		case LIST_HEAVYFC:
			return HEAVYFACTORY;
		case LIST_INFANTRY:
			if (item->getBuildId() == INFANTRY || item->getBuildId() == SOLDIER) {
				return BARRACKS;
			}
			return WOR;
		case LIST_LIGHTFC:
			return LIGHTFACTORY;
		case LIST_ORNI:
			return HIGHTECH;
		case LIST_PALACE:
			return PALACE;
		case LIST_NONE:
			assert(false);
			return -1;
		case LIST_STARPORT:
			return STARPORT;
		default:
			assert(false);
			return -1;
	}
	return -1;
}

int cStructureUtils::findClosestStructureTypeToCell(int cell, int structureType, cPlayer * player) {
	assert(player);
	assert(structureType > -1);
	assert(cell >= 0 || cell < MAX_CELLS);

	// We need cell calculation, so get a calculator:
	cCellCalculator cellCalculator;

	int foundStructureId=-1;		// found structure id
	long shortestDistance=9999; // max distance to search in

	int playerId = player->getId();

	for (int i=0; i < MAX_STRUCTURES; i++) {
		if (structure[i]) {												// exists (pointer)
			if (structure[i]->getOwner() == playerId) {     			// same player
				if (structure[i]->getType() == structureType) {   		// type equals parameter
					if (structure[i]->iUnitID < 0) {    				// no other unit is heading to this structure
						long distance = cellCalculator.distance(cell, structure[i]->getCell());

						// if distance is lower than last found distance, it is the closest for now.
						if (distance < shortestDistance) {
							foundStructureId=i;
							shortestDistance=distance;
						}
					}
				}
			}
		}
	}

	return foundStructureId;
}
