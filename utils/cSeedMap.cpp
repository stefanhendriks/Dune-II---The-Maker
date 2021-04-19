#include "../include/d2tmh.h"

cSeedMap::cSeedMap() {
	memset(map, TERRAIN_SAND, sizeof(map));
}

void cSeedMap::setCellType(int x, int y, int type) {
	assert(x > -1);
	assert(x < 64);
	assert(y > -1);
	assert(y < 64);
	assert(type > -1);
	assert(type < TERRAIN_WALL);
	map[x][y] = type;
}

int cSeedMap::getCellType(int x, int y) {
	assert(x > -1);
	assert(x < 64);
	assert(y > -1);
	assert(y < 64);
	return map[x][y];
}

char cSeedMap::getCellTypeCharacter(int x, int y) {
	int type = getCellType(x, y);

	switch (type) {
		case TERRAIN_SAND: return '.'; break;
		case TERRAIN_HILL: return 'H'; break;
		case TERRAIN_SPICE: return '-'; break;
		case TERRAIN_SPICEHILL: return '+'; break;
		case TERRAIN_ROCK: return '%'; break;
		case TERRAIN_MOUNTAIN: return 'R'; break;
	}

	return 'X'; // error
}
