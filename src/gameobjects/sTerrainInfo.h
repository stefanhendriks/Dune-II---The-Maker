/**
 * @file sTerrainInfo.h
 *
 * Dune 2 - The Maker
 *
 * @author Stefan Hendriks & the D2TM Team
 * @www http://www.dune2themaker.com
 * @copyright Copyright (c) 2001 - 2026 D2TM Team
 * @license This software is released under the MIT License.
 *
 * Note: Dune 2 is a trademark of Westwood Studios/Electronic Arts.
 *
 * This is a non-commercial educational project.
 */

#pragma once

struct s_TerrainInfo {
    int bloomTimerDuration;         // duration from bloom timer
    int terrainSpiceMinSpice;       // minimal spice in TERRAIN_SPICE cells
    int terrainSpiceMaxSpice;       // maximal spice in TERRAIN_SPICE cells
    int terrainSpiceHillMinSpice;   // minimal spice in TERRAIN_SPICEHILL cells
    int terrainSpiceHillMaxSpice;   // maximal spice in TERRAIN_SPICEHILL cells
    int terrainWallHp;              // HP of a terrain WALL
};