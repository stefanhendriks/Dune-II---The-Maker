#include "map/MapGeometry.hpp"
#include "utils/RNG.hpp"
#include "utils/d2tm_math.h"
#include <cmath>
#include <cassert>
#include <algorithm>

#define TILESIZE_WIDTH_PIXELS 32
#define TILESIZE_HEIGHT_PIXELS 32

MapGeometry::MapGeometry(int _mapWidth, int _mapHeight)
    : mapWidth(_mapWidth)
    , mapHeight(_mapHeight)
    , maxCells(mapWidth * mapHeight)
{
}

void MapGeometry::resize(int _mapWidth, int _mapHeight)
{
    mapWidth = _mapWidth;
    mapHeight = _mapHeight;
    maxCells = mapWidth * mapHeight;
}

int MapGeometry::getCellX(int c) const
{
    if (!isValidCell(c)) return -1;
    assert(c > -1 && c < maxCells);
    return c % mapWidth;
}

int MapGeometry::getCellY(int c) const
{
    if (!isValidCell(c)) return -1;
    assert(c > -1 && c < maxCells);
    return c / mapWidth;
}

int MapGeometry::getCellAbove(int c) const
{
    if (!isValidCell(c)) return -1;
    int cellAbove = c - mapWidth;

    if (cellAbove < 0) return -1;

    return cellAbove;
}

int MapGeometry::getCellBelow(int c) const
{
    if (!isValidCell(c)) return -1;
    int cellBelow = c + mapWidth;
    if (cellBelow >= maxCells)
        return -1;

    return cellBelow;
}

int MapGeometry::getCellLeft(int c) const
{
    if (!isValidCell(c)) return -1;
    int x = getCellX(c);
    int cellLeft = x - 1;
    if (cellLeft < 0) return -1;
    return c - 1;
}

int MapGeometry::getCellRight(int c) const
{
    if (!isValidCell(c)) return -1;
    int x = getCellX(c);
    int cellRight = x + 1;
    if (cellRight >= maxCells) return -1;
    if (cellRight >= mapWidth) return -1;

    return c + 1;
}

int MapGeometry::getCellUpperLeft(int c) const
{
    if (!isValidCell(c)) return -1;
    int upperLeftCell = getCellAbove(c) - 1;
    if (upperLeftCell < 0) return -1;

    return upperLeftCell;
}

int MapGeometry::getCellUpperRight(int c) const
{
    if (!isValidCell(c)) return -1;
    int upperRightCell = getCellAbove(c) + 1;
    if (upperRightCell < 0) return -1;

    return upperRightCell;
}

int MapGeometry::getCellLowerLeft(int c) const
{
    if (!isValidCell(c)) return -1;
    int lowerLeftCell = getCellBelow(c) - 1;
    if (lowerLeftCell < 0) return -1;
    if (lowerLeftCell >= maxCells) return -1;
    return lowerLeftCell;
}

int MapGeometry::getCellLowerRight(int c) const
{
    if (!isValidCell(c)) return -1;
    int lowerRightCell = getCellBelow(c) + 1;
    if (lowerRightCell >= maxCells) return -1;
    if (lowerRightCell < 0) return -1;

    return lowerRightCell;
}

bool MapGeometry::isCellAdjacentToOtherCell(int thisCell, int otherCell) const
{
    if (getCellAbove(thisCell) == otherCell) return true;
    if (getCellBelow(thisCell) == otherCell) return true;
    if (getCellLeft(thisCell) == otherCell) return true;
    if (getCellRight(thisCell) == otherCell) return true;

    if (getCellUpperLeft(thisCell) == otherCell) return true;
    if (getCellUpperRight(thisCell) == otherCell) return true;
    if (getCellLowerLeft(thisCell) == otherCell) return true;
    if (getCellLowerRight(thisCell) == otherCell) return true;

    return false;
}

int MapGeometry::getCellWithMapDimensions(int x, int y) const
{
    if (x < 0) return -1;
    if (x >= mapWidth) return -1;
    if (y < 0) return -1;
    if (y >= mapHeight) return -1;
    return (y * mapWidth) + x;
}

int MapGeometry::makeCell(int x, int y) const
{
    assert(x > -1 && "makeCell x must be > -1");
    assert(x < mapWidth && "makeCell x must be < width"); // should never be higher!
    assert(y > -1 && "makeCell y must be > -1");
    assert(y < mapHeight && "makeCell y must be < height");

    // create cell
    int result = getCellWithMapDimensions(x, y);

    assert(result < maxCells); // may never be => (will since MAX_CELLS-1 is max in array!)
    assert(result > -1); // may never be < 0

    return result;
}

bool MapGeometry::isValidCell(int c) const {
    return c > -1 && c < maxCells;   //return !(c < 0 || c >= maxCells);
}

bool MapGeometry::isWithinBoundaries(int x, int y) const {
    return x >= 1 && x < mapWidth-1 && y >= 1 && y < mapHeight-1;
}

double MapGeometry::distance(int cell1, int cell2) const
{
    int x1 = getCellX(cell1);
    int y1 = getCellY(cell1);

    int x2 = getCellX(cell2);
    int y2 = getCellY(cell2);
    return ABS_length(x1, y1, x2, y2);
}

double MapGeometry::distance(int x1, int y1, int x2, int y2) const
{
    if (x1 == x2 && y1 == y2) return 1;

    int A = abs(x2 - x1) * abs(x2 - x1);
    int B = abs(y2 - y1) * abs(y2 - y1);
    return sqrt((double) (A + B));
}

int MapGeometry::getCellWithMapBorders(int x, int y) const
{
    // internal vars are 1 based (ie 64x64 means 0-63, which really means 1...62 are valid)
    int maxHeight = (mapHeight - 2); // hence the -2!
    int maxWidth = (mapWidth - 2);

    if (x < 1) return -1;
    if (y < 1) return -1;
    if (x > maxWidth) return -1;
    if (y > maxHeight) return -1;

    return getCellWithMapDimensions(x, y);
}

/**
 * Returns a random cell, disregards playable borders
 * @return
 */
int MapGeometry::getRandomCell() const
{
    return RNG::rnd(maxCells);
}


int MapGeometry::getRandomCellWithinMapWithSafeDistanceFromBorder(int distance) const
{
    return getCellWithMapBorders(
               distance + RNG::rnd(mapWidth - (distance * 2)),
               distance + RNG::rnd(mapHeight - (distance * 2))
           );
}

bool MapGeometry::isWithinBoundaries(int c) const //rip
{
    if (!isValidCell(c)) return false;
    return isWithinBoundaries(getCellX(c), getCellY(c));
}

/**
 * returns a (randomized) cell. Taking cell param as 'start' position and 'distance' the amount of cells you want to
 * move away. This function will not chose a random position between cell and position. Hence, the amount of cells
 * to move is guaranteed to be so many cells away.
 * @param cell
 * @param distance
 */
int MapGeometry::getRandomCellFrom(int cell, int distance) const
{
    int startX = getCellX(cell);
    int startY = getCellY(cell);
    int xDir = RNG::rnd(100) < 50 ? -1 : 1;
    int yDir = RNG::rnd(100) < 50 ? -1 : 1;
    int newX = (startX - distance) + (xDir * distance);
    int newY = (startY - distance) + (yDir * distance);
    return getCellWithMapBorders(newX, newY);
}

/**
 * Like getRandomCellFrom, but will randomize x/y coordinate. Using 'distance' to create a square around 'cell' (as center)
 * Hence, this could result in a cell being returned much closer than distance. But never further away than distance.
 * @param cell
 * @param distance
 * @return
 */
int MapGeometry::getRandomCellFromWithRandomDistance(int cell, int distance) const
{
    int startX = getCellX(cell);
    int startY = getCellY(cell);
    int newX = (startX - distance) + (RNG::rnd(distance * 2));
    int newY = (startY - distance) + (RNG::rnd(distance * 2));
    return getCellWithMapBorders(newX, newY);
}

int MapGeometry::getMaxDistanceInPixels() const
{
    int tileWidth = TILESIZE_WIDTH_PIXELS;
    int tileHeight = TILESIZE_HEIGHT_PIXELS;
    int maxWidthDistance = mapWidth * tileWidth;
    int maxHeightDistance = mapHeight * tileHeight;
    return ABS_length(0, 0, maxWidthDistance, maxHeightDistance);
}

bool MapGeometry::isAtMapBoundaries(int cell) const
{
    bool validCell = isValidCell(cell);
    if (!validCell) return false;

    int maxHeight = (mapHeight - 2); // hence the -2!
    int maxWidth = (mapWidth - 2);

    int x = getCellX(cell);
    int y = getCellY(cell);

    if (x == 1 || x == maxWidth) return true;
    if (y == 1 || y == maxHeight) return true;

    return false;
}

cPoint MapGeometry::fixCoordinatesToBeWithinPlayableMap(int x, int y) const
{
    return {std::clamp(x, 1, mapWidth - 2), std::clamp(y, 1, mapHeight - 2)};
}

cPoint MapGeometry::fixCoordinatesToBeWithinMap(int x, int y) const
{
    return {std::clamp(x, 0, mapWidth - 1), std::clamp(y, 0, mapHeight - 1)};
}


cPoint MapGeometry::getAbsolutePositionFromCell(int cell) const
{
    return cPoint(getAbsoluteXPositionFromCell(cell), getAbsoluteYPositionFromCell(cell));
}

int MapGeometry::getAbsoluteYPositionFromCell(int cell) const
{
    if (cell < 0) return -1;
    return getCellY(cell) * TILESIZE_HEIGHT_PIXELS;
}

int MapGeometry::getAbsoluteXPositionFromCell(int cell) const
{
    if (cell < 0) return -1;
    return getCellX(cell) * TILESIZE_WIDTH_PIXELS;
}

int MapGeometry::getAbsoluteXPositionFromCellCentered(int cell)  const
{
    return getAbsoluteXPositionFromCell(cell) + (TILESIZE_WIDTH_PIXELS / 2);
}

int MapGeometry::getAbsoluteYPositionFromCellCentered(int cell)  const
{
    return getAbsoluteYPositionFromCell(cell) + (TILESIZE_HEIGHT_PIXELS / 2);
}