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

void cMapUtils::clearAllCells() {
	for (int c=0; c < MAX_CELLS; c++) {
		map->cell[c].credits = map->cell[c].health = 0;
		map->cell[c].passable = true;
		map->cell[c].tile = 0;
		map->cell[c].type = TERRAIN_SAND;    // refers to gfxdata!

		map->cell[c].smudgetile = -1;
		map->cell[c].smudgetype = -1;

		// clear out the ID stuff
		memset(map->cell[c].id, -1, sizeof(map->cell[c].id));

		for (int i = 0; i < MAX_PLAYERS; i++) {
			map->setVisible(c, i, false);
		}
	}
}
