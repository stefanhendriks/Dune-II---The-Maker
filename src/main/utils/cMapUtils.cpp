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
	cellCalculator = new CellCalculator(map);
}

cMapUtils::~cMapUtils() {
	map = NULL;
	delete cellCalculator;
}

bool cMapUtils::isCellWithinMapBorders(int cell) {
	int x = cellCalculator->getX(cell);
	int y = cellCalculator->getY(cell);
	return isWithinMapBorders(x, y);

}

bool cMapUtils::isWithinMapBorders(int x, int y) {
	if (x < 1) return false;
	if (x > map->getWidthMinusBorder()) return false;
	if (y < 1) return false;
	if (y > map->getHeightMinusBorder()) return false;
	return true;
}

bool cMapUtils::isCellVisibleForPlayerId(int playerId, int iCell) {
	assert(playerId >= HUMAN);
	assert(playerId < MAX_PLAYERS);
	cPlayer * thePlayer = &player[playerId];
	int id = thePlayer->getId();
	if (id < HUMAN) {
		char msg[255];
		sprintf(msg, "ID from player is [%d], iCell [%d], playerId is [%d]", id, iCell, playerId);
		logbook(msg);
	}
	return isCellVisible(thePlayer, iCell);
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
	for (int c = 0; c < MAX_CELLS; c++) {
		map->cell[c].spiceInCredits = map->cell[c].hitpoints = 0;
		map->cell[c].passable = true;
		map->cell[c].tileIndexToDraw = 0;
		map->cell[c].terrainTypeGfxDataIndex = TERRAIN_SAND; // refers to gfxdata!

		map->cell[c].smudgeTileToDraw = -1;
		map->cell[c].smudgeTerrainTypeGfxDataIndex = -1;

		// clear out the ID stuff
		memset(map->cell[c].gameObjectId, -1, sizeof(map->cell[c].gameObjectId));

		for (int i = 0; i < MAX_PLAYERS; i++) {
			map->setVisible(c, i, false);
		}
	}
}
