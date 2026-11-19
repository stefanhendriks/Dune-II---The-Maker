/**
 * @file TerrainTypes.hpp
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

// Terrain types of a map cell. These are game values, not graphics ids.
// The values are contiguous (BLOOM..WALL), code compares against that range.
// The terrain sprites in sdl_data.dat share these indexes (checked in test_GfxNameConstants.cpp), so the map drawer can
// turn a terrain type into a TextureID with toTextureID(); gfxnames::TERRAIN_x are the entry names.
constexpr int TERRAIN_BLOOM = 117;
constexpr int TERRAIN_HILL = 118;
constexpr int TERRAIN_MOUNTAIN = 119;
constexpr int TERRAIN_ROCK = 120;
constexpr int TERRAIN_SAND = 121;
constexpr int TERRAIN_SLAB = 122;
constexpr int TERRAIN_SPICE = 123;
constexpr int TERRAIN_SPICEHILL = 124;
constexpr int TERRAIN_WALL = 125;
