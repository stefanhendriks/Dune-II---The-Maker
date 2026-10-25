/**
 * @file PreviewMap.hpp
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

#include <array>
#include <string>
#include <vector>

static constexpr int MAX_SKIRMISHMAP_PLAYERS = 5;
static constexpr int RANDOM_CELL = -1; // value in Bloom= / Field= meaning: place at a random cell

// pure map data, no SDL resources
struct s_PreviewMap {
    int width, height;              // width & height of map
    std::vector<int> terrainType;   // terrainType per cell (for map creation)
    std::array<int, MAX_SKIRMISHMAP_PLAYERS> iStartCell; // starting locations
    std::vector<int> blooms;        // terrain bloom cells, RANDOM_CELL for a random one (empty when the map has none)
    std::vector<int> fields;        // spice field cells, RANDOM_CELL for a random one (empty when the map has none)
    bool validMap = true;           // is this a valid map? (is false, when map can be loaded but has invalid data)
    std::string name;               // name of map
    std::string author;             // author of map
    std::string description;        // description of map
};
