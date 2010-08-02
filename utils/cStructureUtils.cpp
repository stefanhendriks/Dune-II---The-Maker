/*
 * cStructureUtils.cpp
 *
 *  Created on: 2-aug-2010
 *      Author: Stefan
 */

#include "..\d2tmh.h"

cStructureUtils::cStructureUtils() {
	// TODO Auto-generated constructor stub

}

cStructureUtils::~cStructureUtils() {
	// TODO Auto-generated destructor stub
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
