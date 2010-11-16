/*
 * cRandomMapGenerator.cpp
 *
 *  Created on: 16 nov. 2010
 *      Author: Stefan
 */

#include "../include/d2tmh.h"

cRandomMapGenerator::cRandomMapGenerator() {
}

cRandomMapGenerator::~cRandomMapGenerator() {

}

int cRandomMapGenerator::getWallTerrainIndex(bool up, bool down, bool left, bool right) {
	bool a = up;
	bool u = down;
	bool l = left;
	bool r = right;

	// decide the proper tile!
	int value = -1;
	if (l == false && a == false && r == false && u == false) value = 12; // lonely tile
	if (l == true && a == false && r == false && u == false) value = 10; // at the left only
	if (l == false && a == false && r == true && u == false) value = 10; // at the right only
	if (l == false && a == true && r == false && u == false) value = 12; // at the top only
	if (l == false && a == false && r == false && u == true) value = 8; // at the bottom only
	if (l == true && a == false && r == true && u == false) value = 10; // one way connection  (left/right)
	if (l == false && a == true && r == false && u == true) value = 8; // one way connection  (up/down)

	// corners
	if (l == true && a == true && r == false && u == false) value = 3; // corner
	if (l == true && a == false && r == false && u == true) value = 11; // corner
	if (l == false && a == true && r == true && u == false) value = 2; // corner
	if (l == false && a == false && r == true && u == true) value = 1; // corner

	// Border fixes
	if (l == true && a == true && r == false && u == true) value = 5; // border LEFT
	if (l == false && a == true && r == true && u == true) value = 4; // border RIGHT
	if (l == true && a == true && r == true && u == false) value = 7; // border UNDER
	if (l == true && a == false && r == true && u == true) value = 6; // border ABOVE

	// accompanied WALL (all sides WALL too)
	if (l == true && a == true && r == true && u == true) value = 9; // not so lonely WALL

	return value;
}

/**
 * Return an index of the terrain cell. It does not matter what kind of terrain type
 * we are talking about here, the index remains the same.
 *
 * NOTE: This is not true for walls!
 */
int cRandomMapGenerator::getDefaultTerrainIndex(bool up, bool down, bool left, bool right) {
	// fix compatability for now
	bool a = up;
	bool u = down;
	bool l = left;
	bool r = right;

	int value = -1;
	if (l == false && a == false && r == false && u == false) value = 10; // lonely tile
	if (l == true && a == false && r == false && u == false) value = 12; // at the left only
	if (l == false && a == false && r == true && u == false) value = 13; // at the right only
	if (l == false && a == true && r == false && u == false) value = 14; // at the top only
	if (l == false && a == false && r == false && u == true) value = 15; // at the bottom only
	if (l == true && a == false && r == true && u == false) value = 11; // one way connection  (left/right)
	if (l == false && a == true && r == false && u == true) value = 16; // one way connection  (up/down)

	// corners
	if (l == true && a == true && r == false && u == false) value = 7; // corner
	if (l == true && a == false && r == false && u == true)	value = 8; // corner
	if (l == false && a == true && r == true && u == false) value = 9; // corner
	if (l == false && a == false && r == true && u == true) value = 6; // corner

	// Border fixes
	if (l == true && a == true && r == false && u == true) value = 3; // border LEFT
	if (l == false && a == true && r == true && u == true) value = 2; // border RIGHT
	if (l == true && a == true && r == true && u == false) value = 5; // border UNDER
	if (l == true && a == false && r == true && u == true) value = 4; // border ABOVE

	// accompanied rock (all sides rock too)
	if (l == true && a == true && r == true && u == true) value = 1; // not so lonely rock

	// make 0 based
	value--;

	return value;
}

bool cRandomMapGenerator::isAboveSpecificTerrainType(int sourceCell, int terrainType) {
	if (sourceCell < 0) return false;
	int above = CELL_ABOVE(sourceCell);
	return isSpecificTerrainType(above, terrainType);
}

bool cRandomMapGenerator::isSpecificTerrainType(int cell, int terrainType) {
	if (cell < 0) return false;
	if (cell >= MAX_CELLS) return false;
	return map.cell[cell].type == terrainType;
}

bool cRandomMapGenerator::isBelowSpecificTerrainType(int sourceCell, int terrainType) {
	if (sourceCell < 0) return false;
	int under = CELL_UNDER(sourceCell);
	return isSpecificTerrainType(under, terrainType);
}

bool cRandomMapGenerator::isLeftSpecificTerrainType(int sourceCell, int terrainType) {
	if (sourceCell < 0) return false;
	int left = CELL_LEFT(sourceCell);
	return isSpecificTerrainType(left, terrainType);
}

bool cRandomMapGenerator::isRightSpecificTerrainType(int sourceCell, int terrainType) {
	if (sourceCell < 0) return false;
	int right = CELL_RIGHT(sourceCell);
	return isSpecificTerrainType(right, terrainType);
}

int cRandomMapGenerator::smoothMountainCell(int cell) {
	bool a, d, l, r;

	a = isAboveSpecificTerrainType(cell, TERRAIN_MOUNTAIN);
	d = isBelowSpecificTerrainType(cell, TERRAIN_MOUNTAIN);
	l = isLeftSpecificTerrainType(cell, TERRAIN_MOUNTAIN);
	r = isRightSpecificTerrainType(cell, TERRAIN_MOUNTAIN);

	/*
	if ((above_type >= ROCK1 && above_type <= ROCK9))
	a = true;

	// rock under?
	if ((under_type >= ROCK1 && under_type <= ROCK9))
	u = true;

	// rock left?
	if ((left_type >= ROCK1 && left_type <= ROCK9))
	l = true;

	// rock right?
	if ((right_type >= ROCK1 && right_type <= ROCK9))
	r = true;
	*/

	return getDefaultTerrainIndex(a, d, l, r);
}

int cRandomMapGenerator::smoothSpiceCell(int cell) {
	bool a, d, l, r;

	a = isAboveSpecificTerrainType(cell, TERRAIN_SPICE) || isAboveSpecificTerrainType(cell, TERRAIN_SPICEHILL);
	d = isBelowSpecificTerrainType(cell, TERRAIN_SPICE) || isBelowSpecificTerrainType(cell, TERRAIN_SPICEHILL);
	l = isLeftSpecificTerrainType(cell, TERRAIN_SPICE) || isLeftSpecificTerrainType(cell, TERRAIN_SPICEHILL);
	r = isRightSpecificTerrainType(cell, TERRAIN_SPICE) || isRightSpecificTerrainType(cell, TERRAIN_SPICEHILL);

	return getDefaultTerrainIndex(a, d, l, r);
}

int cRandomMapGenerator::smoothRockCell(int cell) {
	bool a, d, l, r;

	// rock above?
	a = isAboveSpecificTerrainType(cell, TERRAIN_ROCK) ||
		isAboveSpecificTerrainType(cell, TERRAIN_SLAB) ||
		isAboveSpecificTerrainType(cell, TERRAIN_MOUNTAIN) ||
		isAboveSpecificTerrainType(cell, TERRAIN_WALL);

	d = isBelowSpecificTerrainType(cell, TERRAIN_ROCK) ||
		isBelowSpecificTerrainType(cell, TERRAIN_SLAB) ||
		isBelowSpecificTerrainType(cell, TERRAIN_MOUNTAIN) ||
		isBelowSpecificTerrainType(cell, TERRAIN_WALL);

	l = isLeftSpecificTerrainType(cell, TERRAIN_ROCK) ||
		isLeftSpecificTerrainType(cell, TERRAIN_SLAB) ||
		isLeftSpecificTerrainType(cell, TERRAIN_MOUNTAIN) ||
		isLeftSpecificTerrainType(cell, TERRAIN_WALL);

	r = isRightSpecificTerrainType(cell, TERRAIN_ROCK) ||
		isRightSpecificTerrainType(cell, TERRAIN_SLAB) ||
		isRightSpecificTerrainType(cell, TERRAIN_MOUNTAIN) ||
		isRightSpecificTerrainType(cell, TERRAIN_WALL);


	/*
	// Extra for debris! (broken stuff)
	if ((above_type >= STR_B_BOTTOM && above_type <= STR_TURRET))
	a = true;

	// rock under?
	if ((under_type >= STR_B_BOTTOM && under_type <= STR_TURRET))
	u = true;

	// rock left?
	if ((left_type >= STR_B_BOTTOM && left_type <= STR_TURRET))
	l = true;

	// rock right?
	if ((right_type >= STR_B_BOTTOM && right_type <= STR_TURRET))
	r = true; */

	return getDefaultTerrainIndex(a, d, l, r);
}

int cRandomMapGenerator::smoothSpiceHillCell(int cell) {
	bool a, d, l, r;

	a = isAboveSpecificTerrainType(cell, TERRAIN_SPICEHILL);
	d = isBelowSpecificTerrainType(cell, TERRAIN_SPICEHILL);
	l = isLeftSpecificTerrainType(cell, TERRAIN_SPICEHILL);
	r = isRightSpecificTerrainType(cell, TERRAIN_SPICEHILL);

	return getDefaultTerrainIndex(a, d, l, r);
}

int cRandomMapGenerator::smoothSandHillCell(int cell) {
	bool a, d, l, r;

	a = isAboveSpecificTerrainType(cell, TERRAIN_HILL);
	d = isBelowSpecificTerrainType(cell, TERRAIN_HILL);
	l = isLeftSpecificTerrainType(cell, TERRAIN_HILL);
	r = isRightSpecificTerrainType(cell, TERRAIN_HILL);

	return getDefaultTerrainIndex(a, d, l, r);
}

int cRandomMapGenerator::smoothWallCell(int cell) {
	bool a, d, l, r;

	a = isAboveSpecificTerrainType(cell, TERRAIN_WALL);
	d = isBelowSpecificTerrainType(cell, TERRAIN_WALL);
	l = isLeftSpecificTerrainType(cell, TERRAIN_WALL);
	r = isRightSpecificTerrainType(cell, TERRAIN_WALL);

	return getWallTerrainIndex(a, d, l, r);
}

void cRandomMapGenerator::smoothCell(int cell) {
	int tile = -1;
	int terrainType = map.cell[cell].type;
	if (terrainType == TERRAIN_ROCK) {
		tile = smoothRockCell(cell);
	} else if (terrainType == TERRAIN_MOUNTAIN) {
		tile = smoothMountainCell(cell);
	} else if (terrainType == TERRAIN_SPICE) {
		tile = smoothSpiceCell(cell);
	} else if (terrainType == TERRAIN_SPICEHILL) {
		tile = smoothSpiceHillCell(cell);
	} else if (terrainType == TERRAIN_HILL) {
		tile = smoothSandHillCell(cell);
	} else if (terrainType == TERRAIN_WALL) {
		tile = smoothWallCell(cell);
	} else if (terrainType == TERRAIN_SAND || terrainType == TERRAIN_BLOOM || terrainType == TERRAIN_SLAB) {
		tile = 0; // always the same
	} else {
		// unknown terrain type
		assert(false);
	}

	assert(tile > -1);
	map.cell[cell].tile = tile;
}

void cRandomMapGenerator::smoothAroundCell(int cell) {
    int above = CELL_ABOVE(cell);
    int under = CELL_UNDER(cell);
    int left  = CELL_LEFT(cell);
    int right = CELL_RIGHT(cell);

    if (above > -1)	smoothCell(above);
    if (under > -1)	smoothCell(under);
    if (left > -1)	smoothCell(left);
    if (right > -1)	smoothCell(right);
    if (cell > -1)	smoothCell(cell);
}

void cRandomMapGenerator::removeSingleRockSpots() {
	// soft out rocky spots!
	int startX = 1;
	int startY = 1;
	int endX = map.getWidth() - 1;
	int endY = map.getHeight() - 1;
	cCellCalculator * cellCalculator = map.getCellCalculator();

	for (int x=startX; x < endX; x++) {
		for (int y=startY; y< endY; y++)
		{
			int cll = cellCalculator->getCell(x, y);
			int terrainType = map.cell[cll].type;

			// now count how many rock is around it
			if (terrainType == TERRAIN_ROCK) {
				int iC=0;

				if (isLeftSpecificTerrainType(cll, TERRAIN_ROCK)) iC++;
				if (isRightSpecificTerrainType(cll, TERRAIN_ROCK)) iC++;
				if (isAboveSpecificTerrainType(cll, TERRAIN_ROCK)) iC++;
				if (isBelowSpecificTerrainType(cll, TERRAIN_ROCK)) iC++;

				if (isLeftSpecificTerrainType(cll, TERRAIN_MOUNTAIN)) iC++;
				if (isRightSpecificTerrainType(cll, TERRAIN_MOUNTAIN)) iC++;
				if (isAboveSpecificTerrainType(cll, TERRAIN_MOUNTAIN)) iC++;
				if (isBelowSpecificTerrainType(cll, TERRAIN_MOUNTAIN)) iC++;

				// when only 1 neighbor, then make sand as well
				if (iC < 2) {
					map.cell[cll].type = TERRAIN_SAND;
					map.cell[cll].tile = 0;
				}
			}
		}
	}
}

void cRandomMapGenerator::smoothMap() {
	for (int c = 0; c < MAX_CELLS; c++) {
		smoothCell(c);
	}
}
