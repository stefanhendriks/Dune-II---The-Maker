/**
 * @file TerrainInfoLoader.cpp
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

#include "context/TerrainInfoLoader.hpp"

#include "gameobjects/sTerrainInfo.h"
#include "utils/Log.h"
#include "utils/cIniFile.h"

bool TerrainInfoLoader::load(const std::string& filename, s_TerrainInfo& terrainInfo) const
{
    cIniFile file;
    const bool loaded = file.load(filename);
    if (!loaded) {
        Logger::error(COMP_GAMERULES, "TerrainInfoLoader::load", "Could not open file [{}], using default terrain values", filename);
    }
    // Without a file, a [TERRAIN] section or a key, the original values are used
    const cSection section = (loaded && file.hasSection("TERRAIN")) ? file.getSection("TERRAIN") : cSection();
    terrainInfo.bloomTimerDuration = section.getInt("BloomTimerDuration", 200);
    terrainInfo.terrainSpiceMinSpice = section.getInt("SpiceMinSpice", 50);
    terrainInfo.terrainSpiceMaxSpice = section.getInt("SpiceMaxSpice", 125);
    terrainInfo.terrainSpiceHillMinSpice = section.getInt("SpiceHillMinSpice", 75);
    terrainInfo.terrainSpiceHillMaxSpice = section.getInt("SpiceHillMaxSpice", 150);
    terrainInfo.terrainWallHp = section.getInt("WallHitPoints", 100);
    return loaded;
}
