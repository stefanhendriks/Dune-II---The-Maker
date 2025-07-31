#pragma once

#include "utils/cPoint.h"

class MapGeometry
{
public:
    MapGeometry(int mapWidth, int mapHeight);
    ~MapGeometry() = default;

    int getCellX(int c) const;
    int getCellY(int c) const;
    int getCellWithMapDimensions(int x, int y) const;
    int makeCell(int x, int y) const;

    bool isValidCell(int c) const;
    bool isWithinBoundaries(int x, int y) const;
    bool isWithinBoundaries(int c) const;
    // ...
    double distance(int cell1, int cell2) const;
    int getCellWithMapBorders(int x, int y) const;

    int getMaxCells() const { return maxCells; }

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