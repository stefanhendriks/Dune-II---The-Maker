/**
 * @file cMapEditor.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2002 - 2026 Stefan Hendriks and contributors
 * @license This software is released under the MIT License. See LICENSE.md.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

#include "cMap.h"

// the map editor is able to change map charactaristics
class cMapEditor {
public:
    explicit cMapEditor(cMap *map);

    void smoothCell(int cell);		// smooth a specific cell
    void smoothMap();				// smooth the entire map
    void smoothAroundCell(int cell);	// smooth cells above, under, left and right given cell
    void removeSingleRockSpots();		// when single rock cells have less than 3 neighbors, remove them

    void createRandomField(int cell, int terrainType, int size); // create a randomized field on the map
    void createSquaredField(int cell, int terrainType, int size); // create a square field on the map

    /**
     * Sets/changes cell state. If cell < 0 will do nothing.
     *
     * @param cell
     * @param terrainType
     * @param tile
     */
    void createCell(int cell, int terrainType, int tile);

private:
    bool isSpecificTerrainType(int cell, int terrainType);

    bool isBelowSpecificTerrainType(int sourceCell, int terrainType);
    bool isAboveSpecificTerrainType(int sourceCell, int terrainType);
    bool isLeftSpecificTerrainType(int sourceCell, int terrainType);
    bool isRightSpecificTerrainType(int sourceCell, int terrainType);

    int getDefaultTerrainIndex(bool up, bool down, bool left, bool right);
    int getWallTerrainIndex(bool up, bool down, bool left, bool right);

    int smoothRockCell(int cell);
    int smoothMountainCell(int cell);
    int smoothSpiceCell(int cell);
    int smoothSpiceHillCell(int cell);
    int smoothSandHillCell(int cell);
    int smoothWallCell(int cell);

    cMap *m_map = nullptr;
    MapGeometry *m_mapGeometry = nullptr;
};
