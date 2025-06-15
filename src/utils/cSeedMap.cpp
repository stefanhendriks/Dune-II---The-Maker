#include "cSeedMap.h"

#include "data/gfxdata.h"

#include <cassert>

cSeedMap::cSeedMap() : map(kMapWidth*kMapHeight, TERRAIN_SAND) {
}

void cSeedMap::setCellType(int x, int y, int type) {
	assert(x > -1);
	assert(x < kMapWidth);
	assert(y > -1);
	assert(y < kMapHeight);
	assert(type > -1);
	assert(type < TERRAIN_WALL);
	map[x + kMapWidth * y] = type;
}

int cSeedMap::getCellType(int x, int y) {
	assert(x > -1);
	assert(x < kMapWidth);
	assert(y > -1);
	assert(y < kMapHeight);
	return map[x + kMapWidth * y];
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
