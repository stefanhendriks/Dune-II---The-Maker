#pragma once

#include "enums.h"

#include <vector>
#include <string>

#include <SDL2/SDL.h>

class Texture;


struct s_TerrainInfo {
    int bloomTimerDuration;         // duration from bloom timer
    int terrainSpiceMinSpice;       // minimal spice in TERRAIN_SPICE cells
    int terrainSpiceMaxSpice;       // maximal spice in TERRAIN_SPICE cells
    int terrainSpiceHillMinSpice;   // minimal spice in TERRAIN_SPICEHILL cells
    int terrainSpiceHillMaxSpice;   // maximal spice in TERRAIN_SPICEHILL cells
    int terrainWallHp;              // HP of a terrain WALL
};