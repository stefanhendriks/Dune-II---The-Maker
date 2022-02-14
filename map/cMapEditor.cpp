#include "../include/d2tmh.h"
#include <fmt/core.h>

cMapEditor::cMapEditor() {

}

cMapEditor::~cMapEditor() {
}

void cMapEditor::createCell(int cell, int terrainType, int tile) {
    int theTile = tile;
    if (terrainType == TERRAIN_SLAB) {
        theTile = rnd(5);
    }
    map.createCell(cell, terrainType, theTile);
}

void cMapEditor::createRandomField(int cell, int terrainType, int size) {
    assert(terrainType >= TERRAIN_BLOOM);
    assert(terrainType <= TERRAIN_WALL);

    if (cell < 0) return;

    int x = map.getCellX(cell);
    int y = map.getCellY(cell);

    if (x < 0) {
        x = rnd(map.getWidth());
    }

    if (y < 0) {
        y = rnd(map.getHeight());
    }

    if (terrainType == TERRAIN_ROCK && size < 0) {
        size = 150;
    }

    if (terrainType == TERRAIN_MOUNTAIN && size < 0) {
        size = 25;  // very small mountainious spots
    }

    int iDist = 0;

    int iOrgX = x;
    int iOrgY = y;

    for (int i = 0; i < size; i++) {
        int c = map.getCellWithMapBorders(x, y);

        iDist = ABS_length(x, y, iOrgX, iOrgY);

        if (iDist > rnd(4) + 4) {
            if (rnd(100) < 5) {
                createRandomField(c, terrainType, 100);
            }
        }

        if (c > -1) {
            // if we are placing spice: if NOT a rock tile, then place spice on it.
            int terrainTypeOfNewCell = map.getCellType(c);
            if (terrainType == TERRAIN_SPICE) {
                if ((terrainTypeOfNewCell != TERRAIN_ROCK) &&
                    (terrainTypeOfNewCell != TERRAIN_SLAB) &&
                    (terrainTypeOfNewCell != TERRAIN_MOUNTAIN) &&
                    (terrainTypeOfNewCell != TERRAIN_WALL)) {
                    createCell(c, TERRAIN_SPICE, 0);
                }
            } else if (terrainType == TERRAIN_HILL) {
                // create hill
                if (terrainTypeOfNewCell == TERRAIN_SAND) {
                    createCell(c, TERRAIN_HILL, 0);
                } else if (terrainTypeOfNewCell == TERRAIN_SPICE) {
                    createCell(c, TERRAIN_SPICE, 0);
                }
            } else {
                createCell(c, terrainType, 0);
            }
        }

        if (rnd(100) < 25) {
            x = map.getCellX(cell);
            y = map.getCellY(cell);
        }


        // randomly shift the cell one coordinate up/down/left/right
        switch (rnd(4)) {
            case 0:
                x++;
                break;
            case 1:
                y++;
                break;
            case 2:
                x--;
                break;
            case 3:
                y--;
                break;
        }

        cPoint::split(x, y) = map.fixCoordinatesToBeWithinMap(x, y);
    }

    smoothMap();
}

void cMapEditor::createSquaredField(int cell, int terrainType, int size) {
    assert(terrainType >= TERRAIN_BLOOM);
    assert(terrainType <= TERRAIN_WALL);

    if (cell < 0) return;

    int x = map.getCellX(cell);
    int y = map.getCellY(cell);


    for (int fx = x; fx < x+size; fx++) {
        for (int fy = y; fy < y+size; fy++) {
            int c = map.getCellWithMapBorders(fx,fy);
            createCell(c, terrainType, 0);
        }
    }

    smoothMap();
}

int cMapEditor::getWallTerrainIndex(bool up, bool down, bool left, bool right) {
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

    value--;
    return value;
}

/**
 * Return an index of the terrain cell. It does not matter what kind of terrain type
 * we are talking about here, the index remains the same.
 *
 * NOTE: This is not true for walls!
 */
int cMapEditor::getDefaultTerrainIndex(bool up, bool down, bool left, bool right) {
    // fix compatibility for now
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
    if (l == true && a == false && r == false && u == true) value = 8; // corner
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

bool cMapEditor::isAboveSpecificTerrainType(int sourceCell, int terrainType) {
    if (sourceCell < 0) return false;
    int above = map.getCellAbove(sourceCell);
    return isSpecificTerrainType(above, terrainType);
}

bool cMapEditor::isSpecificTerrainType(int cell, int terrainType) {
    if (!map.isValidCell(cell)) return false;
    return map.getCellType(cell) == terrainType;
}

bool cMapEditor::isBelowSpecificTerrainType(int sourceCell, int terrainType) {
    if (sourceCell < 0) return false;
    int under = map.getCellBelow(sourceCell);
    return isSpecificTerrainType(under, terrainType);
}

bool cMapEditor::isLeftSpecificTerrainType(int sourceCell, int terrainType) {
    if (sourceCell < 0) return false;
    int left = map.getCellLeft(sourceCell);
    return isSpecificTerrainType(left, terrainType);
}

bool cMapEditor::isRightSpecificTerrainType(int sourceCell, int terrainType) {
    if (sourceCell < 0) return false;
    int right = map.getCellRight(sourceCell);
    return isSpecificTerrainType(right, terrainType);
}

int cMapEditor::smoothMountainCell(int cell) {
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

int cMapEditor::smoothSpiceCell(int cell) {
    bool a, d, l, r;

    a = isAboveSpecificTerrainType(cell, TERRAIN_SPICE) || isAboveSpecificTerrainType(cell, TERRAIN_SPICEHILL);
    d = isBelowSpecificTerrainType(cell, TERRAIN_SPICE) || isBelowSpecificTerrainType(cell, TERRAIN_SPICEHILL);
    l = isLeftSpecificTerrainType(cell, TERRAIN_SPICE) || isLeftSpecificTerrainType(cell, TERRAIN_SPICEHILL);
    r = isRightSpecificTerrainType(cell, TERRAIN_SPICE) || isRightSpecificTerrainType(cell, TERRAIN_SPICEHILL);

    return getDefaultTerrainIndex(a, d, l, r);
}

int cMapEditor::smoothRockCell(int cell) {
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

    return getDefaultTerrainIndex(a, d, l, r);
}

int cMapEditor::smoothSpiceHillCell(int cell) {
    bool a, d, l, r;

    a = isAboveSpecificTerrainType(cell, TERRAIN_SPICEHILL);
    d = isBelowSpecificTerrainType(cell, TERRAIN_SPICEHILL);
    l = isLeftSpecificTerrainType(cell, TERRAIN_SPICEHILL);
    r = isRightSpecificTerrainType(cell, TERRAIN_SPICEHILL);

    return getDefaultTerrainIndex(a, d, l, r);
}

int cMapEditor::smoothSandHillCell(int cell) {
    bool a, d, l, r;

    a = isAboveSpecificTerrainType(cell, TERRAIN_HILL);
    d = isBelowSpecificTerrainType(cell, TERRAIN_HILL);
    l = isLeftSpecificTerrainType(cell, TERRAIN_HILL);
    r = isRightSpecificTerrainType(cell, TERRAIN_HILL);

    return getDefaultTerrainIndex(a, d, l, r);
}

int cMapEditor::smoothWallCell(int cell) {
    bool a, d, l, r;

    a = isAboveSpecificTerrainType(cell, TERRAIN_WALL);
    d = isBelowSpecificTerrainType(cell, TERRAIN_WALL);
    l = isLeftSpecificTerrainType(cell, TERRAIN_WALL);
    r = isRightSpecificTerrainType(cell, TERRAIN_WALL);

    return getWallTerrainIndex(a, d, l, r);
}

void cMapEditor::smoothCell(int cell) {
    int tile = -1;
    int terrainType = map.getCellType(cell);
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
        logbook(fmt::format("Unknown terrain type [{}] .", terrainType));
        map.cellChangeType(cell, TERRAIN_SAND);
        map.cellChangeTile(cell, 0);
        return;
    }

    assert(tile > -1);
    map.cellChangeTile(cell, tile);
}

void cMapEditor::smoothAroundCell(int cell) {
    int above = map.getCellAbove(cell);
    int under = map.getCellBelow(cell);
    int left = map.getCellLeft(cell);
    int right = map.getCellRight(cell);

    if (above > -1) smoothCell(above);
    if (under > -1) smoothCell(under);
    if (left > -1) smoothCell(left);
    if (right > -1) smoothCell(right);
    if (cell > -1) smoothCell(cell);
}

void cMapEditor::removeSingleRockSpots() {
    // soft out rocky spots!
    int startX = 1;
    int startY = 1;
    int endX = map.getWidth() - 1;
    int endY = map.getHeight() - 1;

    for (int x = startX; x < endX; x++) {
        for (int y = startY; y < endY; y++) {
            int cll = map.getCellWithMapDimensions(x, y);
            if (cll < 0) continue;

            int terrainType = map.getCellType(cll);

            // now count how many rock is around it
            if (terrainType == TERRAIN_ROCK) {
                int iC = 0;

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
                    map.cellChangeType(cll, TERRAIN_SAND);
                    map.cellChangeTile(cll, 0);
                }
            }
        }
    }
}

void cMapEditor::smoothMap() {
    for (int c = 0; c < map.getMaxCells(); c++) {
        smoothCell(c);
    }
}
