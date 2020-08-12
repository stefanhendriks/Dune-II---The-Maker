/*
 * cMapUtils.cpp
 *
 *  Created on: 19-okt-2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cMapUtils::cMapUtils(cMap *theMap) {
	assert(theMap);
	map = theMap;
}

cMapUtils::~cMapUtils() {
	map = NULL;
}

bool cMapUtils::isCellVisibleForPlayerId(int playerId, int iCell) {
	assert(playerId >= HUMAN);
	assert(playerId < MAX_PLAYERS);
	return isCellVisible(&player[playerId], iCell);
}


bool cMapUtils::isCellVisible(cPlayer *thePlayer, int iCell) {
//	assert(iCell >= 0);
//	assert(iCell < MAX_CELLS);
//	assert(thePlayer);
    if (!thePlayer) return false;
    if (iCell < 0) return false;
    if (iCell >= MAX_CELLS) return false;

	int playerId = thePlayer->getId();
	assert(playerId >= HUMAN);
	assert(playerId < MAX_PLAYERS);

	return map->isVisible(iCell, playerId);
}

void cMapUtils::clearAllCells() {
	for (int c=0; c < MAX_CELLS; c++) {
	    map->cellInit(c);
	}
}
