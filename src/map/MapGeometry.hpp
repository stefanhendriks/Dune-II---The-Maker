#pragma once

#include "utils/cPoint.h"

class MapGeometry
{
public:
    MapGeometry(int mapWidth, int mapHeight);
    ~MapGeometry() = default;
    void resize(int mapWidth, int mapHeight);

    int getCellX(int c) const;
    int getCellY(int c) const;
    int getCellAbove(int c) const;
    int getCellBelow(int c) const;
    int getCellLeft(int c) const;
    int getCellRight(int c) const;
    int getCellUpperLeft(int c) const;
    int getCellUpperRight(int c) const;
    int getCellLowerLeft(int c) const;
    int getCellLowerRight(int c) const;
    bool isCellAdjacentToOtherCell(int thisCell, int otherCell) const;
    /**
        Returns cell , taking given map width/height into account. This includes the invisible border around the map.
        If you want to take the invisible border into account use getCellWithMapBorders instead.
    **/
    int getCellWithMapDimensions(int x, int y) const;
    /**
        Return map cell; taking the map borders into account. If x or y falls out of bounds, this function will return -1
        If you want to include the invisible map borders, use getCellWithMapDimensions instead.
    **/
    int getCellWithMapBorders(int x, int y) const;
    /**
        Return a cell from an X,Y coordinate.

        Remember that coordinates are 1-64 based. While the array in Map (tCell) is 0-till 4096.

        This means that the coordinate 1,1 is NOT the first row, but it is : 0,0. This also means the
        MAX at the right is *not* MAP_W_MAX, but it is MAP_W_MAX - 1.
        (it is 0-63 instead of 1-64).

        This method will not do any fancy tricks to fix the boundaries, instead it will assert its input and output.

        Use <b>getCellWithMapBorders</b> if you want a safe way to get a cell within the <i>playable</i> map boundaries.
        Use <b>getCellWithMapDimensions</b> if you want a safe way to get a cell within the <i>maximum</i> map boundaries.
    **/
    int makeCell(int x, int y) const;

    bool isValidCell(int c) const;
    bool isWithinBoundaries(int x, int y) const;
    bool isWithinBoundaries(int c) const;
    // ...
    double distance(int cell1, int cell2) const;
    double distance(int x1, int y1, int x2, int y2) const;

    int getMaxCells() const { return maxCells; }
    int getMaxDistanceInPixels() const;

    bool isAtMapBoundaries(int cell) const;
    cPoint fixCoordinatesToBeWithinPlayableMap(int x, int y) const;
    cPoint fixCoordinatesToBeWithinMap(int x, int y) const;
    cPoint getAbsolutePositionFromCell(int cell) const;

    int getAbsoluteYPositionFromCell(int cell) const;
    int getAbsoluteXPositionFromCell(int cell) const;
    int getAbsoluteXPositionFromCellCentered(int cell)  const;
    int getAbsoluteYPositionFromCellCentered(int cell)  const;

    /*  based random functions   */

    //Returns a random cell, disregards playable borders
    int getRandomCell() const;
    int getRandomCellWithinMapWithSafeDistanceFromBorder(int distance) const;
    int getRandomCellFrom(int cell, int distance) const;
    int getRandomCellFromWithRandomDistance(int cell, int distance) const;

private:
    int mapWidth;
    int mapHeight;
    int maxCells;
};