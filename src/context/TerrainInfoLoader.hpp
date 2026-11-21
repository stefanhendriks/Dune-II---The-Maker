/**
 * @file TerrainInfoLoader.hpp
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

#include <string>

struct s_TerrainInfo;

// Fills the terrain info from the [TERRAIN] section of terrain.ini.
// A key that is absent keeps its default (the value the game always used).
class TerrainInfoLoader {
public:
    // Returns false when the file could not be opened (everything then keeps the default values)
    bool load(const std::string& filename, s_TerrainInfo& terrainInfo) const;
};
