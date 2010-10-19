/*
 * cMapUtils.cpp
 *
 *  Created on: 19-okt-2010
 *      Author: Stefan
 */

#include "../d2tmh.h"

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
	assert(iCell >= 0);
	assert(iCell < MAX_CELLS);
	assert(thePlayer);

	int playerId = thePlayer->getId();
	assert(playerId >= HUMAN);
	assert(playerId < MAX_PLAYERS);

	return map->isVisible(iCell, playerId);
}
