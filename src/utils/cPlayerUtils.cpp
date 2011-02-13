/*
 * cPlayerUtils.cpp
 *
 *  Created on: 20-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"


cPlayerUtils::cPlayerUtils() {
	player = NULL;
}

cPlayerUtils::cPlayerUtils(cPlayer *thePlayer) {
	assert(thePlayer);
	player = thePlayer;
}

cPlayerUtils::~cPlayerUtils() {
}

// convencience / wrapper for player
int cPlayerUtils::getAmountOfStructuresForStructureTypeForPlayer(cPlayer * player, int structureType) {
	assert(player);
	return player->getAmountOfStructuresForType(structureType);
}

// convencience / wrapper for player, context aware
int cPlayerUtils::getAmountOfStructuresForStructureTypeForPlayer(int structureType) {
	return getAmountOfStructuresForStructureTypeForPlayer(player, structureType);
}
