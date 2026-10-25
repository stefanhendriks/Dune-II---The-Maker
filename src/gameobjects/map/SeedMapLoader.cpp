/**
 * @file SeedMapLoader.cpp
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

#include "gameobjects/map/SeedMapLoader.hpp"

#include "gameobjects/map/MapGeometry.hpp"
#include "gameobjects/map/cMapEditor.h"
#include "gameobjects/map/cPreviewMaps.h"
#include "gameobjects/map/cSeedMapGenerator.h"
#include "utils/Log.h"

#include <format>
#include <vector>

namespace SeedMapLoader {

void loadIntoMap(cMap *map, const MapGeometry *mapGeometry, int seed)
{
    Logger::info(COMP_SCENARIOINI, "SeedMapLoader::loadIntoMap", "Generating seed map with seed {}.", seed);

    auto seedGenerator = cSeedMapGenerator(seed);
    auto seedMap = seedGenerator.generateSeedMap();
    Logger::info(COMP_SCENARIOINI, "SeedMapLoader::loadIntoMap", "Seedmap generated");

    auto mapEditor = cMapEditor(map);
    for (int mapY = 0; mapY < 64; mapY++) {
        for (int mapX = 0; mapX < 64; mapX++) {
            int type = seedMap.getCellType(mapX, mapY);
            int iCell = mapGeometry->makeCell(mapX, mapY);
            mapEditor.createCell(iCell, type, 0);
        }
    }

    Logger::info(COMP_SCENARIOINI, "SeedMapLoader::loadIntoMap", "Seedmap converted into D2TM map.");
}

s_PreviewMap toPreviewMap(int seed)
{
    auto seedMap = cSeedMapGenerator(seed).generateSeedMap();

    // same layout as skirmish previews (cPreviewMaps::loadSkirmish): a 1 cell border around the seed map
    constexpr int seedMapSize = 64;
    s_PreviewMap previewMap;
    previewMap.width = seedMapSize + 2;
    previewMap.height = seedMapSize + 2;
    previewMap.name = std::format("Seed {}", seed);
    previewMap.iStartCell.fill(-1);
    previewMap.terrainType = std::vector<int>(previewMap.width * previewMap.height, -1);

    MapGeometry mapGeom(previewMap.width, previewMap.height);
    for (int mapY = 0; mapY < seedMapSize; mapY++) {
        for (int mapX = 0; mapX < seedMapSize; mapX++) {
            previewMap.terrainType[mapGeom.makeCell(mapX + 1, mapY + 1)] = seedMap.getCellType(mapX, mapY);
        }
    }
    return previewMap;
}

}
