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
    if (playerId < HUMAN || playerId >= MAX_PLAYERS) return false;
	return isCellVisible(&player[playerId], iCell);
}


bool cMapUtils::isCellVisible(cPlayer *thePlayer, int iCell) {
    if (!thePlayer) return false;
    if (iCell < 0) return false;
    if (iCell >= MAX_CELLS) return false;
    int playerId = thePlayer->getId();
	return map->isVisible(iCell, playerId);
}

void cMapUtils::clearAllCells() {
	for (int c=0; c < MAX_CELLS; c++) {
	    map->cellInit(c);
	}
}
